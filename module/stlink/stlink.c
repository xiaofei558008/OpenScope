/*
 * OpenScope ST-Link 驱动模块（stlink.dll）
 *
 * 能力：OS_CAP_DRIVER
 *  - OS_CMD_SCAN       getStLinkList 枚举 ST-Link 探针
 *  - OS_CMD_CONNECT    connectStLink（HOTPLUG 非侵入，无需芯片型号）
 *  - OS_CMD_DISCONNECT disconnect + deleteInterfaceList
 *  - OS_CMD_READ_MEM / OS_CMD_WRITE_MEM  内存读写（互斥序列化，AD-11）
 *  - OS_CMD_GET_INFO   getDeviceGeneralInf
 *  - OS_CMD_GET_FREQ   返回支持的速度档位（主界面联动用）
 *
 * CubeProgrammer_API.dll 运行时 LoadLibrary + GetProcAddress 绑定（AD-12）：
 * 优先 dll\stlink\（随包自含），回退 ST 安装目录 bin\（依赖 DLL 所在目录）。
 */
#include "stlink.h"
#include "module_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OS_STLINK_MAX_PROBES 16

typedef struct StLinkCtx {
    OS_StLinkApi api;
    CRITICAL_SECTION cs;      /* 串行化读写（AD-11） */
    OS_ConnectCfg cfg;
    int connected;
    OS_DriverInfo info;
    char dll_dir[MAX_PATH];
    char loaders_dir[MAX_PATH]; /* setLoadersPath 目标（含 FlashLoader/ExternalLoader） */
    OS_StLinkDcp probes[OS_STLINK_MAX_PROBES]; /* 最近一次扫描的探针副本（含 freq 档位） */
    int probe_count;
    ULONGLONG last_reconnect_ms;      /* 自动重连节流时间戳 */
    ULONGLONG last_reconnect_log_ms;  /* 重连成功日志节流（对齐 jlink，5s） */
} StLinkCtx;

static StLinkCtx g_ctx;
static const OS_Framework* g_fw;
static HMODULE g_hmod;
static void (*g_stlink_free)(void*); /* msvcrt.dll 的 free（CubeProgrammer_API.dll 用 msvcrt 分配 readMemory 缓冲） */

#define OS_STLINK_RECONNECT_MIN_MS 500
#define OS_STLINK_RECONNECT_LOG_MS 5000

static int mod_scan(OS_ScanReq* req);
static int mod_connect_ex(char* err, int errlen);
static int mod_disconnect(void);

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) g_hmod = hinst;
    return TRUE;
}

/* ---------------- DLL 路径与绑定 ---------------- */

static void module_dir(char* out, int cap)
{
    char buf[MAX_PATH];
    char* slash;
    DWORD n = GetModuleFileNameA(g_hmod ? g_hmod : GetModuleHandleA("stlink.dll"), buf, MAX_PATH);
    if (n == 0) { _snprintf(out, cap, "."); return; }
    slash = strrchr(buf, '\\');
    if (slash) *slash = 0;
    _snprintf(out, cap, "%s", buf);
}

/* 定位 CubeProgrammer_API.dll：优先 dll\stlink\（随包），回退 ST 安装目录。
 * 依赖 DLL（Qt/OpenSSL/xerces 等）位于 bin\，用 SetDllDirectoryW 加入搜索路径。 */
static HMODULE stlink_load_lib(char* err, int err_len)
{
    struct { const char* dll; char depdir[MAX_PATH]; } cands[4];
    wchar_t wdep[MAX_PATH];
    int n = 0, i;
    char bundled[MAX_PATH], binapi[MAX_PATH], binbin[MAX_PATH];
    const char* st_bin = "C:\\Program Files\\STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\bin";

    module_dir(g_ctx.dll_dir, sizeof(g_ctx.dll_dir));
    _snprintf(bundled, sizeof(bundled), "%s\\stlink\\CubeProgrammer_API.dll", g_ctx.dll_dir);
    _snprintf(binapi, sizeof(binapi),
              "C:\\Program Files\\STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\api\\lib\\CubeProgrammer_API.dll");
    _snprintf(binbin, sizeof(binbin),
              "C:\\Program Files\\STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\bin\\CubeProgrammer_API.dll");

    cands[n].dll = bundled;
    _snprintf(cands[n].depdir, sizeof(cands[n].depdir), "%s\\stlink", g_ctx.dll_dir);
    _snprintf(g_ctx.loaders_dir, sizeof(g_ctx.loaders_dir), "%s\\stlink", g_ctx.dll_dir);
    n++;
    cands[n].dll = binbin;
    _snprintf(cands[n].depdir, sizeof(cands[n].depdir), "%s", st_bin);
    _snprintf(g_ctx.loaders_dir, sizeof(g_ctx.loaders_dir), "%s",
              "C:\\Program Files\\STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\api\\lib");
    n++;
    cands[n].dll = binapi;
    _snprintf(cands[n].depdir, sizeof(cands[n].depdir), "%s", st_bin);
    _snprintf(g_ctx.loaders_dir, sizeof(g_ctx.loaders_dir), "%s",
              "C:\\Program Files\\STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\api\\lib");
    n++;

    for (i = 0; i < n; i++) {
        HMODULE h;
        MultiByteToWideChar(CP_ACP, 0, cands[i].depdir, -1, wdep, MAX_PATH);
        SetDllDirectoryW(wdep);
        h = LoadLibraryA(cands[i].dll);
        if (h) {
            if (err && err_len > 0)
                _snprintf(err, err_len, "%s", cands[i].dll);
            return h;
        }
    }
    if (err && err_len > 0)
        _snprintf(err, err_len, "未找到 CubeProgrammer_API.dll（dll\\stlink\\ 与 STM32CubeProgrammer 安装目录均无）");
    return NULL;
}

int os_stlink_bind(OS_StLinkApi* api, char* err, int err_len)
{
    HMODULE h;
    if (err && err_len > 0) err[0] = 0;
    memset(api, 0, sizeof(*api));
    h = stlink_load_lib(err, err_len);
    if (!h) return OS_ERR_FAIL;
    api->h = h;
#define BIND(field, sym) do { \
        *(void**)&api->field = (void*)GetProcAddress(h, sym); \
        if (!api->field) { \
            if (err) _snprintf(err, err_len, "缺少导出 " sym); \
            FreeLibrary(h); api->h = NULL; \
            return OS_ERR_FAIL; \
        } \
    } while (0)
    BIND(set_display_callbacks, "setDisplayCallbacks");
    BIND(set_verbosity_level, "setVerbosityLevel");
    BIND(set_loaders_path, "setLoadersPath");
    BIND(get_stlink_list, "getStLinkList");
    BIND(delete_interface_list, "deleteInterfaceList");
    BIND(connect_stlink, "connectStLink");
    BIND(check_device_connection, "checkDeviceConnection");
    BIND(disconnect, "disconnect");
    BIND(read_memory, "readMemory");
    BIND(write_memory, "writeMemory");
    BIND(reset, "reset");
    BIND(execute, "execute");
    BIND(get_device_general_inf, "getDeviceGeneralInf");
#undef BIND
    /* readMemory 的缓冲由 CubeProgrammer_API.dll 用 msvcrt.dll 分配（dumpbin /imports 实测），
     * 跨 CRT（本模块 v145/ucrt）free 会崩溃；绑定 msvcrt 的 free 用于正确释放。 */
    {
        HMODULE hm = GetModuleHandleA("msvcrt.dll");
        g_stlink_free = hm ? (void (*)(void*))GetProcAddress(hm, "free") : NULL;
    }
    return OS_ERR_OK;
}

void os_stlink_unbind(OS_StLinkApi* api)
{
    if (api->h) {
        FreeLibrary(api->h);
        api->h = NULL;
    }
}

/* ST-Link 内部日志回调：wchar_t → UTF-8 → OS_Framework::log */
static void stlink_log_cb(int msgType, const wchar_t* str)
{
    char utf8[1024];
    int level;
    if (!g_fw || !str) return;
    /* DisplayManager MSGTYPE: 0=Normal 1=Info 2=GreenInfo 3=Title 4=Warning 5=Error
     * 6..8=Verbosity 9=GreenInfoNoPopup 10=WarningNoPopup 11=ErrorNoPopup */
    if (msgType == 5 || msgType == 11) level = OS_LOG_ERROR;
    else if (msgType == 4 || msgType == 10) level = OS_LOG_WARN;
    else if (msgType >= 6 && msgType <= 8) level = OS_LOG_DEBUG;
    else level = OS_LOG_INFO;
    {
        int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, utf8, sizeof(utf8) - 1, NULL, NULL);
        if (len <= 0) { utf8[0] = 0; len = 1; }
        utf8[len - 1] = 0;
        while (len > 1 && (utf8[len - 2] == '\n' || utf8[len - 2] == '\r')) utf8[--len - 1] = 0;
        g_fw->log(level, "ST-Link: %s", utf8);
    }
}

/* 进度条回调：官方示例把三者都设为真实函数，connectStLink 会调用进度条，
 * 空指针会导致崩溃——用空实现占位。 */
static void stlink_init_progress_cb(void) {}
static void stlink_load_bar_cb(int x, int n) { (void)x; (void)n; }

static void stlink_install_callbacks(void)
{
    OS_StLinkCallbacks cb;
    memset(&cb, 0, sizeof(cb));
    cb.logMessage = stlink_log_cb;
    cb.initProgressBar = stlink_init_progress_cb;
    cb.loadBar = stlink_load_bar_cb;
    if (g_ctx.api.set_display_callbacks) g_ctx.api.set_display_callbacks(cb);
    if (g_ctx.api.set_verbosity_level) g_ctx.api.set_verbosity_level(1); /* 仅警告/错误/成功 */
}

/* ---------------- 连接/读写实现 ---------------- */

/* 从 freq 档位中选最接近（≤ speed_khz）的频率；speed=0 返回 0（用 DLL 默认）。
 * 档位可能升序或降序，故不假设顺序：取 ≤speed 的最大档，低于所有档时取最小档。 */
static int pick_freq(const OS_StLinkDcp* d, int iface, int speed_khz)
{
    const unsigned int* freqs;
    int n, i, best = 0, minf = 0;
    if (speed_khz <= 0) return 0;
    if (iface == OS_IF_JTAG) { freqs = d->freq.jtagFreq; n = (int)d->freq.jtagFreqNumber; }
    else                     { freqs = d->freq.swdFreq;  n = (int)d->freq.swdFreqNumber; }
    if (n < 0 || n > 12) n = 12;
    for (i = 0; i < n; i++) {
        int f = (int)freqs[i];
        if (f <= 0) continue;
        if (f <= speed_khz && f > best) best = f;
        if (minf == 0 || f < minf) minf = f;
    }
    return best > 0 ? best : minf;
}

static int stlink_scan_probes(OS_StLinkDcp** out_list, int* out_n)
{
    OS_StLinkDcp* list = NULL;
    int n, i;
    if (!g_ctx.api.h) return OS_ERR_FAIL;
    n = g_ctx.api.get_stlink_list(&list, 0);
    if (n < 0) n = 0;
    if (n > OS_STLINK_MAX_PROBES) n = OS_STLINK_MAX_PROBES;
    g_ctx.probe_count = n;
    for (i = 0; i < n; i++) g_ctx.probes[i] = list[i];
    *out_list = list;
    *out_n = n;
    return OS_ERR_OK;
}

static int mod_connect_ex(char* err, int errlen)
{
    OS_StLinkDcp* list = NULL;
    OS_StLinkDcp dcp;
    int n, i, idx = 0, rc, freq;
    if (err && errlen > 0) err[0] = 0;
    if (!g_ctx.api.h) {
        if (err) _snprintf(err, errlen, "CubeProgrammer_API.dll 未加载");
        if (g_fw) g_fw->log(OS_LOG_ERROR, "ST-Link 连接: CubeProgrammer_API.dll 未加载");
        return OS_ERR_FAIL;
    }
    if (g_fw) {
        g_fw->log(OS_LOG_INFO, "ST-Link 连接: iface=%s speed_khz=%d probe_idx=%d serial='%s'",
                  g_ctx.cfg.iface == OS_IF_JTAG ? "JTAG" : "SWD",
                  g_ctx.cfg.speed_khz, g_ctx.cfg.probe_index,
                  g_ctx.cfg.serial[0] ? g_ctx.cfg.serial : "(空)");
    }
    if (stlink_scan_probes(&list, &n) != OS_ERR_OK) return OS_ERR_FAIL;
    if (n <= 0) {
        if (g_ctx.api.delete_interface_list) g_ctx.api.delete_interface_list();
        if (err) _snprintf(err, errlen, "未发现 ST-Link 设备");
        if (g_fw) g_fw->log(OS_LOG_ERROR, "ST-Link 连接失败: 未发现 ST-Link 设备");
        return OS_ERR_NO_DEVICE;
    }
    if (g_ctx.cfg.serial[0]) {
        for (i = 0; i < n; i++)
            if (strcmp(list[i].serialNumber, g_ctx.cfg.serial) == 0) { idx = i; break; }
    } else if (g_ctx.cfg.probe_index >= 0 && g_ctx.cfg.probe_index < n) {
        idx = g_ctx.cfg.probe_index;
    }
    dcp = list[idx];
    if (g_ctx.api.delete_interface_list) g_ctx.api.delete_interface_list();
    /* 覆盖连接参数：SWD/JTAG + HOTPLUG（非侵入，不 halt 目标）+ 频率档位 */
    dcp.dbgPort = (g_ctx.cfg.iface == OS_IF_JTAG) ? OS_STLINK_PORT_JTAG : OS_STLINK_PORT_SWD;
    dcp.connectionMode = OS_STLINK_MODE_HOTPLUG;
    dcp.shared = 0;
    freq = pick_freq(&dcp, g_ctx.cfg.iface, g_ctx.cfg.speed_khz);
    if (freq > 0) {
        dcp.frequency = freq;
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 连接: 频率 %d kHz（SWD/JTAG 档位就近）", freq);
    } else {
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 连接: 频率=自动（DLL 默认 %d kHz）", dcp.frequency);
    }
    rc = g_ctx.api.connect_stlink(dcp);
    if (rc != 0) {
        if (err) _snprintf(err, errlen, "connectStLink 失败 (rc=%d)", rc);
        if (g_fw) g_fw->log(OS_LOG_ERROR, "ST-Link 连接: connectStLink 失败 rc=%d", rc);
        return OS_ERR_NO_DEVICE;
    }
    g_ctx.connected = 1;
    return OS_ERR_OK;
}

static void stlink_refresh_info(void)
{
    OS_DriverInfo* d = &g_ctx.info;
    OS_StLinkGenInf* gi = NULL;
    memset(d, 0, sizeof(*d));
    _snprintf(d->name, sizeof(d->name), "%s", "stlink");
    _snprintf(d->version, sizeof(d->version), "%s", "1.0.0");
    _snprintf(d->dll_version, sizeof(d->dll_version), "%s", "CubeProgrammer");
    if (g_ctx.api.get_device_general_inf && g_ctx.connected) {
        gi = g_ctx.api.get_device_general_inf();
        if (gi) {
            _snprintf(d->emulator, sizeof(d->emulator), "%s (CPU:%s)", gi->name, gi->cpu);
            d->hw_version = gi->deviceId;
            d->fw_version = 0;
        }
    } else {
        _snprintf(d->emulator, sizeof(d->emulator), "%s", "未连接");
    }
    d->connected = g_ctx.connected;
}

/* ---------------- 模块命令 ---------------- */

static int mod_scan(OS_ScanReq* req)
{
    OS_StLinkDcp* list = NULL;
    int n, i;
    if (!req || !req->items || req->capacity <= 0) return OS_ERR_INVALID_ARG;
    if (!g_ctx.api.h) return OS_ERR_FAIL;
    if (stlink_scan_probes(&list, &n) != OS_ERR_OK) return OS_ERR_FAIL;
    for (i = 0; i < n; i++) {
        OS_DeviceInfo* it = &req->items[i];
        memset(it, 0, sizeof(*it));
        it->index = i;
        _snprintf(it->serial, sizeof(it->serial), "%s", list[i].serialNumber);
        if (list[i].board[0])
            _snprintf(it->name, sizeof(it->name), "%s", list[i].board);
        else if (list[i].firmwareVersion[0])
            _snprintf(it->name, sizeof(it->name), "ST-Link %s", list[i].firmwareVersion);
        else
            _snprintf(it->name, sizeof(it->name), "ST-Link #%d", i);
    }
    if (g_ctx.api.delete_interface_list) g_ctx.api.delete_interface_list();
    req->count = n;
    if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 扫描: 发现 %d 个设备", n);
    return OS_ERR_OK;
}

static int mod_disconnect(void)
{
    if (g_ctx.api.h && g_ctx.connected) {
        if (TryEnterCriticalSection(&g_ctx.cs)) {
            if (g_ctx.api.h && g_ctx.connected) g_ctx.api.disconnect();
            LeaveCriticalSection(&g_ctx.cs);
        }
    }
    g_ctx.connected = 0;
    memset(&g_ctx.info, 0, sizeof(g_ctx.info));
    if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 已断开");
    return OS_ERR_OK;
}

static int mod_read(OS_MemReq* req)
{
    unsigned char* p = NULL;
    int r, retried = 0;
    if (!req || !req->data) return OS_ERR_INVALID_ARG;
    if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
    for (;;) {
        EnterCriticalSection(&g_ctx.cs);
        /* readMemory 双重指针：DLL 分配缓冲回填，成功返回 0 */
        r = g_ctx.api.read_memory((unsigned int)req->address, &p, req->size);
        LeaveCriticalSection(&g_ctx.cs);
        if (r == 0 && p) {
            memcpy(req->data, p, req->size);
            if (g_stlink_free) g_stlink_free(p); /* 用 msvcrt 的 free 释放，避免跨 CRT 崩溃/泄漏 */
            return (int)req->size;
        }
        if (retried) break;
        if (g_ctx.api.check_device_connection && !g_ctx.api.check_device_connection() &&
            (ULONGLONG)GetTickCount64() - g_ctx.last_reconnect_ms >= OS_STLINK_RECONNECT_MIN_MS) {
            if (g_fw && (ULONGLONG)GetTickCount64() - g_ctx.last_reconnect_log_ms >= OS_STLINK_RECONNECT_LOG_MS) {
                g_ctx.last_reconnect_log_ms = (ULONGLONG)GetTickCount64();
                g_fw->log(OS_LOG_WARN, "ST-Link 读取失败：连接丢失，自动重连恢复");
            }
            mod_disconnect();
            if (mod_connect_ex(NULL, 0) == OS_ERR_OK) {
                g_ctx.last_reconnect_ms = (ULONGLONG)GetTickCount64();
                retried = 1;
                continue;
            }
        }
        break;
    }
    return OS_ERR_TIMEOUT;
}

static int mod_write(OS_MemReq* req)
{
    int r;
    if (!req || !req->data) return OS_ERR_INVALID_ARG;
    if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
    EnterCriticalSection(&g_ctx.cs);
    r = g_ctx.api.write_memory((unsigned int)req->address, (char*)req->data, req->size);
    LeaveCriticalSection(&g_ctx.cs);
    return r == 0 ? OS_ERR_OK : OS_ERR_TIMEOUT;
}

/* OS_CMD_GET_FREQ：主界面切换 ST-Link 时刷新速度下拉档位 */
static int mod_get_freq(OS_FreqList* out)
{
    int i;
    const OS_StLinkDcp* d;
    if (!out) return OS_ERR_INVALID_ARG;
    memset(out, 0, sizeof(*out));
    if (g_ctx.probe_count <= 0) return OS_ERR_FAIL;
    d = &g_ctx.probes[0];
    out->swd_n = d->freq.swdFreqNumber;
    if (out->swd_n > 16) out->swd_n = 16;
    for (i = 0; i < out->swd_n; i++) out->swd[i] = (int)d->freq.swdFreq[i];
    out->jtag_n = d->freq.jtagFreqNumber;
    if (out->jtag_n > 16) out->jtag_n = 16;
    for (i = 0; i < out->jtag_n; i++) out->jtag[i] = (int)d->freq.jtagFreq[i];
    return OS_ERR_OK;
}

static int mod_command(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx;
    switch (cmd) {
    case OS_CMD_SCAN:
        return mod_scan((OS_ScanReq*)in);
    case OS_CMD_CONNECT:
        if (in) memcpy(&g_ctx.cfg, in, sizeof(g_ctx.cfg));
        return mod_connect_ex(NULL, 0);
    case OS_CMD_DISCONNECT:
        return mod_disconnect();
    case OS_CMD_IS_CONNECTED:
        if (out) *(int*)out = g_ctx.connected ? 1 : 0;
        return OS_ERR_OK;
    case OS_CMD_READ_MEM:
        return mod_read((OS_MemReq*)in);
    case OS_CMD_WRITE_MEM:
        return mod_write((OS_MemReq*)in);
    case OS_CMD_GET_INFO:
        if (out) {
            stlink_refresh_info();
            memcpy(out, &g_ctx.info, sizeof(g_ctx.info));
        }
        return OS_ERR_OK;
    case OS_CMD_GET_FREQ:
        return mod_get_freq((OS_FreqList*)out);
    case OS_CMD_HALT:
        return OS_ERR_FAIL; /* CubeProgrammer C API 无直接 halt */
    case OS_CMD_GO:
        if (g_ctx.api.h && g_ctx.connected) return g_ctx.api.execute(0x08000000);
        return OS_ERR_NOT_CONNECTED;
    case OS_CMD_RESET:
        if (g_ctx.api.h && g_ctx.connected) return g_ctx.api.reset(OS_STLINK_RST_SOFTWARE);
        return OS_ERR_NOT_CONNECTED;
    case OS_CMD_ELF_RELOADED:
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 模块: ELF 已重载");
        return OS_ERR_OK;
    default:
        return OS_ERR_FAIL;
    }
}

static int mod_init(const OS_Framework* fw, void** out_ctx)
{
    char err[512] = "";
    (void)out_ctx;
    g_fw = fw;
    InitializeCriticalSection(&g_ctx.cs);
    memset(&g_ctx.cfg, 0, sizeof(g_ctx.cfg));
    g_ctx.cfg.iface = OS_IF_SWD;
    g_ctx.cfg.speed_khz = 4000;
    g_ctx.cfg.probe_index = -1;
    if (os_stlink_bind(&g_ctx.api, err, sizeof(err)) != OS_ERR_OK) {
        if (fw) fw->log(OS_LOG_WARN, "stlink 模块: %s", err);
    } else {
        stlink_install_callbacks();
        if (g_ctx.api.set_loaders_path && g_ctx.loaders_dir[0])
            g_ctx.api.set_loaders_path(g_ctx.loaders_dir); /* 必需：否则 connectStLink 无法识别设备 */
        if (fw) fw->log(OS_LOG_INFO, "stlink 模块: 已绑定 CubeProgrammer_API.dll (%s)", err);
    }
    return OS_ERR_OK;
}

static void mod_deinit(void* ctx)
{
    (void)ctx;
    mod_disconnect();
    os_stlink_unbind(&g_ctx.api);
    DeleteCriticalSection(&g_ctx.cs);
}

static const OS_Module g_module = {
    OS_API_VERSION,
    OS_CAP_DRIVER,
    "stlink",
    "1.0.0",
    "ST-Link 驱动模块：扫描/连接/读写 MCU 内存（CubeProgrammer_API）",
    NULL,
    mod_init,
    mod_deinit,
    mod_command,
    NULL, NULL, NULL, NULL, NULL, NULL
};

const OS_Module* os_module_get(void)
{
    return &g_module;
}
