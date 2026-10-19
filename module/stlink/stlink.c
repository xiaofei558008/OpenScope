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
    int voltage_mv;                   /* 连接时/刷新读到的目标电压 mV；未知 -1 */
    ULONGLONG last_reconnect_ms;      /* 自动重连节流时间戳 */
    ULONGLONG last_reconnect_log_ms;  /* 重连成功日志节流（对齐 jlink，5s） */
} StLinkCtx;

static StLinkCtx g_ctx;
static const OS_Framework* g_fw;
/* 仅在 massErase/downloadFile 期间转发 loadBar；connectStLink 也会打进度，会误显示 100% */
static volatile LONG g_accept_flash_progress;
static HMODULE g_hmod;
static void (*g_stlink_free)(void*); /* msvcrt.dll 的 free（CubeProgrammer_API.dll 用 msvcrt 分配 readMemory 缓冲） */

#define OS_STLINK_RECONNECT_MIN_MS 500
#define OS_STLINK_RECONNECT_LOG_MS 5000

static int mod_scan(OS_ScanReq* req);
static int mod_connect_ex(char* err, int errlen);
static int mod_disconnect(void);
static int mod_read(OS_MemReq* req);
static int mod_write(OS_MemReq* req);

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
        const char* api_lib =
            "C:\\Program Files\\STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\api\\lib";
        char bundled_loaders[MAX_PATH];
        MultiByteToWideChar(CP_ACP, 0, cands[i].depdir, -1, wdep, MAX_PATH);
        SetDllDirectoryW(wdep);
        h = LoadLibraryA(cands[i].dll);
        if (h) {
            /* FlashLoader 路径：优先已安装的 api\lib（器件数据库更完整），
             * 否则用随包 dll\stlink。错配会导致 connectStLink
             * “Unable to list supported devices” 甚至 DLL 内部崩溃。 */
            _snprintf(bundled_loaders, sizeof(bundled_loaders), "%s\\stlink", g_ctx.dll_dir);
            if (GetFileAttributesA(api_lib) != INVALID_FILE_ATTRIBUTES)
                _snprintf(g_ctx.loaders_dir, sizeof(g_ctx.loaders_dir), "%s", api_lib);
            else
                _snprintf(g_ctx.loaders_dir, sizeof(g_ctx.loaders_dir), "%s", bundled_loaders);
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
    /* 烧录：可选（旧版 CubeProgrammer 可能无导出） */
    api->download_file = (int (*)(const wchar_t*, unsigned int, unsigned int, unsigned int, const wchar_t*))
        GetProcAddress(h, "downloadFile");
    api->mass_erase = (int (*)(char*))GetProcAddress(h, "massErase");
    /* CPU 控制 / 读寄存器（可选） */
    api->halt_cpu = (int (*)(void))GetProcAddress(h, "Halt");
    api->run_cpu = (int (*)(void))GetProcAddress(h, "Run");
    api->step_cpu = (int (*)(void))GetProcAddress(h, "Step");
    api->core_reset = (int (*)(void))GetProcAddress(h, "CoreReset");
    api->system_reset = (int (*)(void))GetProcAddress(h, "SystemReset");
    api->read_cortex_reg = (int (*)(unsigned int, unsigned int*))GetProcAddress(h, "readCortexReg");
    api->get_core_state = (int (*)(void))GetProcAddress(h, "getCoreState");
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

/* ST-Link 内部日志回调：wchar_t → UTF-8 → OS_Framework::log。
 * 只透传错误与警告，丢弃其余——readMemory 每次调用都会经 logMessage 打印 ~10 条
 * "UPLOADING/Size/Address/Read progress/Reading data/r ap/Data read successfully/
 *  Time elapsed"（多为 Info/Verbosity），高速采集下会刷爆 UI 消息队列 + 同步落盘，
 * 导致界面卡死。DisplayManager MSGTYPE: 0=Normal 1=Info 2=GreenInfo 3=Title
 * 4=Warning 5=Error 6..8=Verbosity 9=GreenInfoNoPopup 10=WarningNoPopup 11=ErrorNoPopup */
static void stlink_log_cb(int msgType, const wchar_t* str)
{
    char utf8[1024];
    int level, len;
    if (!g_fw || !str) return;
    if (msgType == 5 || msgType == 11) level = OS_LOG_ERROR;
    else if (msgType == 4 || msgType == 10) level = OS_LOG_WARN;
    else return;
    len = WideCharToMultiByte(CP_UTF8, 0, str, -1, utf8, sizeof(utf8) - 1, NULL, NULL);
    if (len <= 0) return;
    utf8[len - 1] = 0;
    while (len > 1 && (utf8[len - 2] == '\n' || utf8[len - 2] == '\r')) utf8[--len - 1] = 0;
    if (utf8[0] == 0) return; /* 跳过空行 */
    g_fw->log(level, "ST-Link: %s", utf8);
}

/* 进度条回调：官方示例把三者都设为真实函数，connectStLink 会调用进度条，
 * 空指针会导致崩溃。loadBar 参数不统一：常见 (cur,total) 或 (percent,100)；
 * n<=0 时勿切 marquee（会与百分比交替导致进度条狂闪）。 */
static void stlink_init_progress_cb(void)
{
    /* 故意空：CubeProgrammer 会多次 init，若每次打回 0% 会造成数字乱跳 */
}
static void stlink_load_bar_cb(int x, int n)
{
    int pct;
    if (!g_fw || !g_fw->flash_progress) return;
    if (!InterlockedCompareExchange(&g_accept_flash_progress, 0, 0)) return;
    if (n <= 0) {
        if (x >= 0 && x <= 100)
            g_fw->flash_progress(x, "烧录");
        return;
    }
    /* (percent, 100) 与 (cur, total) 两种用法兼容 */
    if (n == 100 && x >= 0 && x <= 100)
        pct = x;
    else
        pct = (int)((long long)x * 100 / n);
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    /* phase 名保持稳定，勿把百分比写进 phase（否则宿主峰值逻辑每步重置） */
    g_fw->flash_progress(pct, "烧录");
}

static void stlink_install_callbacks(void)
{
    OS_StLinkCallbacks cb;
    memset(&cb, 0, sizeof(cb));
    cb.logMessage = stlink_log_cb;
    cb.initProgressBar = stlink_init_progress_cb;
    cb.loadBar = stlink_load_bar_cb;
    if (g_ctx.api.set_display_callbacks) g_ctx.api.set_display_callbacks(cb);
    /* verbosity 0 = 库不打印任何消息（readMemory 每次刷屏会卡死 UI）；错误由本模块自行记录 */
    if (g_ctx.api.set_verbosity_level) g_ctx.api.set_verbosity_level(0);
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

/* conn_mode: HOTPLUG=采集；UNDER_RESET=烧录/擦除（目标运行时擦除会失败）
 * speed_cap_khz: >0 时限制上限（烧录建议 ≤8000，过高易擦除失败） */
static int mod_connect_mode(int conn_mode, int rst_mode, int speed_cap_khz,
                            char* err, int errlen)
{
    OS_StLinkDcp* list = NULL;
    OS_StLinkDcp dcp;
    int n, i, idx = 0, rc, freq, want_khz;
    const char* mode_name = "HOTPLUG";
    if (err && errlen > 0) err[0] = 0;
    if (conn_mode == OS_STLINK_MODE_UNDER_RESET) mode_name = "UNDER_RESET";
    else if (conn_mode == OS_STLINK_MODE_NORMAL) mode_name = "NORMAL";
    if (!g_ctx.api.h) {
        if (err) _snprintf(err, errlen, "CubeProgrammer_API.dll 未加载");
        if (g_fw) g_fw->log(OS_LOG_ERROR, "ST-Link 连接: CubeProgrammer_API.dll 未加载");
        return OS_ERR_FAIL;
    }
    want_khz = g_ctx.cfg.speed_khz;
    if (speed_cap_khz > 0 && (want_khz <= 0 || want_khz > speed_cap_khz))
        want_khz = speed_cap_khz;
    if (g_fw) {
        g_fw->log(OS_LOG_INFO, "ST-Link 连接: iface=%s speed_khz=%d mode=%s probe_idx=%d serial='%s'",
                  g_ctx.cfg.iface == OS_IF_JTAG ? "JTAG" : "SWD",
                  want_khz, mode_name, g_ctx.cfg.probe_index,
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
    dcp.dbgPort = (g_ctx.cfg.iface == OS_IF_JTAG) ? OS_STLINK_PORT_JTAG : OS_STLINK_PORT_SWD;
    dcp.connectionMode = conn_mode;
    dcp.resetMode = rst_mode;
    dcp.shared = 0;
    freq = pick_freq(&dcp, g_ctx.cfg.iface, want_khz);
    if (freq > 0) {
        dcp.frequency = freq;
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 连接: 频率 %d kHz（SWD/JTAG 档位就近）", freq);
    } else {
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 连接: 频率=自动（DLL 默认 %d kHz）", dcp.frequency);
    }
    /* CubeProgrammer 在 LoadersPath 错误时可能内部崩溃；SEH 兜底避免拖垮宿主 */
    __try {
        rc = g_ctx.api.connect_stlink(dcp);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        rc = -999;
        if (g_fw) g_fw->log(OS_LOG_ERROR, "ST-Link connectStLink 异常 code=0x%08lX（检查 LoadersPath）",
                            (unsigned long)GetExceptionCode());
    }
    if (rc != 0) {
        if (err) _snprintf(err, errlen, "connectStLink 失败 (rc=%d)", rc);
        if (g_fw) g_fw->log(OS_LOG_ERROR, "ST-Link 连接: connectStLink 失败 rc=%d", rc);
        g_ctx.connected = 0;
        g_ctx.voltage_mv = -1;
        return OS_ERR_NO_DEVICE;
    }
    g_ctx.connected = 1;
    /* CubeProgrammer DCP.targetVoltage 为短字符串，如 "3.30"（单位 V） */
    g_ctx.voltage_mv = -1;
    if (dcp.targetVoltage[0]) {
        double v = atof(dcp.targetVoltage);
        if (v > 0.1 && v < 20.0) g_ctx.voltage_mv = (int)(v * 1000.0 + 0.5);
    }
    if (g_fw && g_ctx.voltage_mv > 0)
        g_fw->log(OS_LOG_INFO, "ST-Link 目标电压: %d mV", g_ctx.voltage_mv);
    return OS_ERR_OK;
}

static int mod_connect_ex(char* err, int errlen)
{
    /* Attach / 默认采集：HOTPLUG 非侵入；Under Reset 仅在显式指定时 */
    if (g_ctx.cfg.connect_under_reset && !g_ctx.cfg.attach) {
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 连接: Under Reset");
        return mod_connect_mode(OS_STLINK_MODE_UNDER_RESET, OS_STLINK_RST_HARDWARE, 0, err, errlen);
    }
    if (g_ctx.cfg.attach && g_fw)
        g_fw->log(OS_LOG_INFO, "ST-Link Attach: HOTPLUG 非侵入接入");
    return mod_connect_mode(OS_STLINK_MODE_HOTPLUG, OS_STLINK_RST_SOFTWARE, 0, err, errlen);
}

/* DHCSR 单步兜底：写 0xE000EDF0 = DBGKEY|C_DEBUGEN|C_HALT|C_STEP */
static int stlink_step_dhcsr(void)
{
    OS_MemReq wr;
    uint8_t data[4];
    uint32_t v = 0xA05F0005u;
    if (g_ctx.api.halt_cpu) g_ctx.api.halt_cpu();
    data[0] = (uint8_t)(v);
    data[1] = (uint8_t)(v >> 8);
    data[2] = (uint8_t)(v >> 16);
    data[3] = (uint8_t)(v >> 24);
    memset(&wr, 0, sizeof(wr));
    wr.address = 0xE000EDF0u;
    wr.size = 4;
    wr.data = data;
    return mod_write(&wr);
}

static int stlink_write_u32(uint32_t addr, uint32_t v)
{
    OS_MemReq wr;
    uint8_t data[4];
    data[0] = (uint8_t)(v);
    data[1] = (uint8_t)(v >> 8);
    data[2] = (uint8_t)(v >> 16);
    data[3] = (uint8_t)(v >> 24);
    memset(&wr, 0, sizeof(wr));
    wr.address = addr;
    wr.size = 4;
    wr.data = data;
    return mod_write(&wr);
}

/* ---- Flash Patch Breakpoint（Cortex-M3/M4 rev1，最多 6 个代码比较器）---- */
#define STLINK_FPB_CTRL  0xE0002000u
#define STLINK_FPB_COMP0 0xE0002008u
#define STLINK_FPB_MAX   6

static struct {
    uint32_t addr; /* 0=空闲；存对齐后地址（含 halfword 信息在 bit1） */
    int      temp; /* 1=StepOut 临时，不进 BP_LIST */
} g_fpb[STLINK_FPB_MAX];

static uint32_t stlink_fpb_comp_word(uint32_t addr)
{
    /* rev1: address[28:2] | REPLACE | ENABLE；REPLACE 选上/下半字 */
    uint32_t comp = (addr & 0x1FFFFFFCu) | 1u;
    if (addr & 2u) comp |= (2u << 30); /* 上半字 */
    else           comp |= (1u << 30); /* 下半字 */
    return comp;
}

static int stlink_fpb_slot_write(int slot, uint32_t addr_or_zero)
{
    uint32_t comp = addr_or_zero ? stlink_fpb_comp_word(addr_or_zero) : 0;
    if (slot < 0 || slot >= STLINK_FPB_MAX) return -1;
    if (stlink_write_u32(STLINK_FPB_CTRL, 3u) < 0) return -1; /* KEY|ENABLE */
    return stlink_write_u32(STLINK_FPB_COMP0 + (uint32_t)slot * 4u, comp);
}

static int stlink_fpb_find(uint32_t addr)
{
    int i;
    addr &= ~1u;
    for (i = 0; i < STLINK_FPB_MAX; i++)
        if (g_fpb[i].addr && (g_fpb[i].addr & ~1u) == addr) return i;
    return -1;
}

static int stlink_fpb_alloc(uint32_t addr, int temp)
{
    int i, slot = -1;
    addr &= ~1u;
    i = stlink_fpb_find(addr);
    if (i >= 0) {
        /* 已有用户永久断点时，临时 StepOut 不得改写 temp，否则随后 free 会删掉用户 BP */
        if (!temp)
            g_fpb[i].temp = 0;
        return i;
    }
    for (i = 0; i < STLINK_FPB_MAX; i++)
        if (!g_fpb[i].addr) { slot = i; break; }
    if (slot < 0) return -1;
    if (stlink_fpb_slot_write(slot, addr) < 0) return -1;
    g_fpb[slot].addr = addr;
    g_fpb[slot].temp = temp;
    return slot;
}

static int stlink_fpb_free_slot(int slot)
{
    int i, any = 0;
    if (slot < 0 || slot >= STLINK_FPB_MAX) return -1;
    stlink_fpb_slot_write(slot, 0);
    g_fpb[slot].addr = 0;
    g_fpb[slot].temp = 0;
    for (i = 0; i < STLINK_FPB_MAX; i++)
        if (g_fpb[i].addr) any = 1;
    if (!any) stlink_write_u32(STLINK_FPB_CTRL, 2u); /* KEY, disable unit */
    return 0;
}

static int stlink_bp_set(const OS_BpReq* req)
{
    uint32_t addr;
    int slot;
    if (!req) return OS_ERR_INVALID_ARG;
    addr = req->address & ~1u;
    if (!addr) return OS_ERR_INVALID_ARG;
    slot = stlink_fpb_alloc(addr, 0);
    if (slot < 0) {
        if (g_fw) g_fw->log(OS_LOG_ERROR, "ST-Link 断点：FPB 槽已满（最多 %d）", STLINK_FPB_MAX);
        return OS_ERR_FAIL;
    }
    if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 断点: 设置 0x%08X slot=%d", addr, slot);
    return OS_ERR_OK;
}

static int stlink_bp_clear(const OS_BpReq* req)
{
    int i;
    if (!req) return OS_ERR_INVALID_ARG;
    if (req->clear_all) {
        for (i = 0; i < STLINK_FPB_MAX; i++)
            if (g_fpb[i].addr) stlink_fpb_free_slot(i);
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 断点: 已全部清除");
        return OS_ERR_OK;
    }
    i = stlink_fpb_find(req->address);
    if (i < 0) return OS_ERR_FAIL;
    stlink_fpb_free_slot(i);
    if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 断点: 清除 0x%08X", req->address & ~1u);
    return OS_ERR_OK;
}

static int stlink_bp_list(OS_BpList* list)
{
    int i;
    if (!list || !list->items || list->capacity <= 0) return OS_ERR_INVALID_ARG;
    list->count = 0;
    for (i = 0; i < STLINK_FPB_MAX && list->count < list->capacity; i++) {
        if (!g_fpb[i].addr || g_fpb[i].temp) continue;
        list->items[list->count].address = g_fpb[i].addr;
        list->items[list->count].id = i;
        list->items[list->count].enabled = 1;
        list->count++;
    }
    return OS_ERR_OK;
}

static int stlink_step_out(void)
{
    unsigned int lr = 0;
    int i, slot, keep_user = 0, pre;
    uint32_t dhcsr;
    OS_MemReq rd;
    uint8_t buf[4];
    if (!g_ctx.api.read_cortex_reg) {
        if (g_fw) g_fw->log(OS_LOG_ERROR, "ST-Link StepOut: 无 readCortexReg");
        return OS_ERR_FAIL;
    }
    if (g_ctx.api.read_cortex_reg(14, &lr) != 0) return OS_ERR_FAIL;
    lr &= ~1u;
    pre = stlink_fpb_find(lr);
    keep_user = (pre >= 0 && !g_fpb[pre].temp);
    slot = stlink_fpb_alloc(lr, 1);
    if (slot < 0) {
        if (g_fw) g_fw->log(OS_LOG_WARN, "ST-Link StepOut: 无空闲 FPB 槽");
        return OS_ERR_FAIL;
    }
    if (g_ctx.api.run_cpu) g_ctx.api.run_cpu();
    memset(&rd, 0, sizeof(rd));
    rd.address = 0xE000EDF0u;
    rd.size = 4;
    rd.data = buf;
    for (i = 0; i < 200; i++) {
        Sleep(10);
        if (mod_read(&rd) >= 0) {
            dhcsr = (uint32_t)buf[0] | ((uint32_t)buf[1] << 8) |
                    ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
            if (dhcsr & (1u << 17)) break; /* S_HALT */
        }
    }
    /* 用户已在 LR 处设断点：只借用，不释放 */
    if (!keep_user)
        stlink_fpb_free_slot(slot);
    else
        g_fpb[slot].temp = 0;
    if (g_ctx.api.halt_cpu) g_ctx.api.halt_cpu();
    if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link StepOut: LR=0x%08X%s",
                        lr, keep_user ? " (保留用户断点)" : "");
    return OS_ERR_OK;
}

/* 烧录前切 Under Reset（硬件复位）；失败则尝试 NORMAL */
static int stlink_enter_flash_mode(void)
{
    int rc;
    if (g_ctx.connected) {
        if (g_ctx.api.disconnect) g_ctx.api.disconnect();
        g_ctx.connected = 0;
    }
    if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 烧录: 切换 Under Reset 连接（Halt+复位，避免擦除失败）");
    rc = mod_connect_mode(OS_STLINK_MODE_UNDER_RESET, OS_STLINK_RST_HARDWARE, 8000, NULL, 0);
    if (rc == OS_ERR_OK) return OS_ERR_OK;
    if (g_fw) g_fw->log(OS_LOG_WARN, "ST-Link 烧录: Under Reset 失败，改试 NORMAL");
    return mod_connect_mode(OS_STLINK_MODE_NORMAL, OS_STLINK_RST_HARDWARE, 8000, NULL, 0);
}

/* 烧录后恢复 HOTPLUG，便于继续采变量 */
static void stlink_leave_flash_mode(void)
{
    if (g_ctx.connected) {
        if (g_ctx.api.disconnect) g_ctx.api.disconnect();
        g_ctx.connected = 0;
    }
    if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 烧录: 恢复 HOTPLUG 连接");
    if (mod_connect_mode(OS_STLINK_MODE_HOTPLUG, OS_STLINK_RST_SOFTWARE, 0, NULL, 0) != OS_ERR_OK) {
        if (g_fw) g_fw->log(OS_LOG_WARN, "ST-Link 烧录后 HOTPLUG 重连失败，请手动重新连接");
    }
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
    int i;
    if (g_ctx.api.h && g_ctx.connected) {
        for (i = 0; i < STLINK_FPB_MAX; i++)
            if (g_fpb[i].addr) stlink_fpb_free_slot(i);
        if (TryEnterCriticalSection(&g_ctx.cs)) {
            if (g_ctx.api.h && g_ctx.connected) g_ctx.api.disconnect();
            LeaveCriticalSection(&g_ctx.cs);
        }
    }
    memset(g_fpb, 0, sizeof(g_fpb));
    g_ctx.connected = 0;
    g_ctx.voltage_mv = -1;
    memset(&g_ctx.info, 0, sizeof(g_ctx.info));
    if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 已断开");
    return OS_ERR_OK;
}

static int mod_read(OS_MemReq* req)
{
    unsigned char* p = NULL;
    int r, retried = 0, attempt;
    if (!req || !req->data) return OS_ERR_INVALID_ARG;
    if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
    for (;;) {
        r = -1;
        p = NULL;
        for (attempt = 0; attempt < 3; attempt++) {
            EnterCriticalSection(&g_ctx.cs);
            /* readMemory 双重指针：DLL 分配缓冲回填，成功返回 0 */
            r = g_ctx.api.read_memory((unsigned int)req->address, &p, req->size);
            LeaveCriticalSection(&g_ctx.cs);
            if (r == 0 && p) {
                memcpy(req->data, p, req->size);
                if (g_stlink_free) g_stlink_free(p); /* 用 msvcrt 的 free 释放，避免跨 CRT 崩溃/泄漏 */
                return (int)req->size;
            }
            if (p && g_stlink_free) g_stlink_free(p);
            p = NULL;
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
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        if (g_ctx.api.halt_cpu) {
            int rc = g_ctx.api.halt_cpu();
            return (rc == 0) ? OS_ERR_OK : OS_ERR_FAIL;
        }
        return OS_ERR_FAIL;
    case OS_CMD_GO:
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        if (g_ctx.api.run_cpu) {
            int rc = g_ctx.api.run_cpu();
            return (rc == 0) ? OS_ERR_OK : OS_ERR_FAIL;
        }
        /* 兜底：从 Flash 基址 execute */
        if (g_ctx.api.execute) return g_ctx.api.execute(0x08000000);
        return OS_ERR_FAIL;
    case OS_CMD_RESET:
        /* Reset & Halt：核复位并停住 */
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        if (g_ctx.api.core_reset) {
            int rc = g_ctx.api.core_reset();
            return (rc == 0) ? OS_ERR_OK : OS_ERR_FAIL;
        }
        {
            int rc = g_ctx.api.reset(OS_STLINK_RST_SOFTWARE);
            if (rc == 0 && g_ctx.api.halt_cpu) g_ctx.api.halt_cpu();
            return (rc == 0) ? OS_ERR_OK : OS_ERR_FAIL;
        }
    case OS_CMD_RESET_RUN:
        /* 复位后运行 */
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        if (g_ctx.api.system_reset) {
            int rc = g_ctx.api.system_reset();
            if (rc != 0) return OS_ERR_FAIL;
            if (g_ctx.api.run_cpu) g_ctx.api.run_cpu();
            return OS_ERR_OK;
        }
        {
            int rc = g_ctx.api.reset(OS_STLINK_RST_SOFTWARE);
            if (rc != 0) return OS_ERR_FAIL;
            if (g_ctx.api.run_cpu) g_ctx.api.run_cpu();
            else if (g_ctx.api.execute) g_ctx.api.execute(0x08000000);
            return OS_ERR_OK;
        }
    case OS_CMD_STEP:
    case OS_CMD_STEP_IN:
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        if (g_ctx.api.step_cpu) {
            int rc = g_ctx.api.step_cpu();
            return (rc == 0) ? OS_ERR_OK : OS_ERR_FAIL;
        }
        return (stlink_step_dhcsr() >= 0) ? OS_ERR_OK : OS_ERR_FAIL;
    case OS_CMD_STEP_OVER:
        /* 迭代1：等同 Step */
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        if (g_ctx.api.step_cpu) {
            int rc = g_ctx.api.step_cpu();
            return (rc == 0) ? OS_ERR_OK : OS_ERR_FAIL;
        }
        return (stlink_step_dhcsr() >= 0) ? OS_ERR_OK : OS_ERR_FAIL;
    case OS_CMD_STEP_OUT:
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        return stlink_step_out();
    case OS_CMD_BP_SET:
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        return stlink_bp_set((const OS_BpReq*)in);
    case OS_CMD_BP_CLEAR:
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        return stlink_bp_clear((const OS_BpReq*)in);
    case OS_CMD_BP_LIST:
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        return stlink_bp_list((OS_BpList*)out);
    case OS_CMD_GET_CPU_STATE: {
        OS_CpuState* st = (OS_CpuState*)out;
        uint32_t dhcsr = 0;
        OS_MemReq mreq;
        uint8_t buf[4];
        int rc;
        if (!st) return OS_ERR_INVALID_ARG;
        memset(st, 0, sizeof(*st));
        st->halted = -1;
        st->powered = -1;
        st->under_reset = -1;
        st->voltage_mv = -1;
        _snprintf(st->state, sizeof(st->state), "—");
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        st->voltage_mv = g_ctx.voltage_mv;
        /* DHCSR @ 0xE000EDF0：S_HALT=bit17，S_RESET_ST=bit25；能读通则视为有电 */
        memset(&mreq, 0, sizeof(mreq));
        mreq.address = 0xE000EDF0u;
        mreq.size = 4;
        mreq.data = buf;
        rc = mod_read(&mreq);
        if (rc >= 0) {
            dhcsr = (uint32_t)buf[0] | ((uint32_t)buf[1] << 8) |
                    ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
            st->powered = 1;
            st->halted = (dhcsr & (1u << 17)) ? 1 : 0;
            st->under_reset = (dhcsr & (1u << 25)) ? 1 : 0;
            if (dhcsr & (1u << 19)) st->halted = 1; /* LOCKUP → 当 STOP */
        }
        /* 仅在已停住时读 PC（readCortexReg 可能强制 halt） */
        if (st->halted == 1 && g_ctx.api.read_cortex_reg) {
            unsigned int pc = 0;
            if (g_ctx.api.read_cortex_reg(15, &pc) == 0) {
                st->pc = pc;
                st->pc_valid = 1;
            }
        }
        if (st->under_reset == 1)
            _snprintf(st->state, sizeof(st->state), "RESET");
        else if (st->halted == 1)
            _snprintf(st->state, sizeof(st->state), "STOP");
        else if (st->halted == 0)
            _snprintf(st->state, sizeof(st->state), "RUNNING");
        else if (st->powered == 1)
            _snprintf(st->state, sizeof(st->state), "POWERON");
        else
            _snprintf(st->state, sizeof(st->state), "—");
        (void)g_ctx.api.get_core_state; /* 保留绑定，DHCSR 路径已够用 */
        return OS_ERR_OK;
    }
    case OS_CMD_FLASH_ERASE: {
        const OS_FlashReq* req = (const OS_FlashReq*)in;
        int rc, ok;
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        if (!g_ctx.api.mass_erase) {
            if (g_fw) g_fw->log(OS_LOG_ERROR, "ST-Link 不支持 massErase");
            return OS_ERR_FAIL;
        }
        EnterCriticalSection(&g_ctx.cs);
        rc = stlink_enter_flash_mode();
        if (rc != OS_ERR_OK) {
            LeaveCriticalSection(&g_ctx.cs);
            return rc;
        }
        if (g_fw && g_fw->flash_progress) g_fw->flash_progress(0, "整片擦除…");
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link massErase…");
        InterlockedExchange(&g_accept_flash_progress, 1);
        rc = g_ctx.api.mass_erase(NULL);
        InterlockedExchange(&g_accept_flash_progress, 0);
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link massErase rc=%d", rc);
        ok = (rc == 0);
        if (ok && g_fw && g_fw->flash_progress) g_fw->flash_progress(100, "擦除完成");
        if (ok && req && req->reset_run && g_ctx.api.reset)
            g_ctx.api.reset(OS_STLINK_RST_SOFTWARE);
        stlink_leave_flash_mode();
        LeaveCriticalSection(&g_ctx.cs);
        return ok ? OS_ERR_OK : OS_ERR_FAIL;
    }
    case OS_CMD_FLASH_DOWNLOAD: {
        const OS_FlashReq* req = (const OS_FlashReq*)in;
        uint32_t addr;
        int rc, ok, skip;
        if (!req || !req->path[0]) return OS_ERR_INVALID_ARG;
        if (!g_ctx.api.h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
        if (!g_ctx.api.download_file) {
            if (g_fw) g_fw->log(OS_LOG_ERROR, "ST-Link 不支持 downloadFile");
            return OS_ERR_FAIL;
        }
        addr = req->address ? req->address : 0x08000000u;
        skip = req->skip_erase ? 1 : 0;
        EnterCriticalSection(&g_ctx.cs);
        rc = stlink_enter_flash_mode();
        if (rc != OS_ERR_OK) {
            LeaveCriticalSection(&g_ctx.cs);
            return rc;
        }
        /* 需要擦除时：先 massErase，再 downloadFile(skipErase=1)。
         * HOTPLUG 下 downloadFile 自带扇区擦除常报 failed to erase memory；
         * Under Reset + 整片擦除更稳（与 CubeProgrammer GUI 流程一致）。 */
        if (!skip && g_ctx.api.mass_erase) {
            if (g_fw && g_fw->flash_progress) g_fw->flash_progress(0, "烧录前擦除…");
            if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link 烧录前 massErase…");
            InterlockedExchange(&g_accept_flash_progress, 1);
            rc = g_ctx.api.mass_erase(NULL);
            InterlockedExchange(&g_accept_flash_progress, 0);
            if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link massErase rc=%d", rc);
            if (rc != 0) {
                stlink_leave_flash_mode();
                LeaveCriticalSection(&g_ctx.cs);
                return OS_ERR_FAIL;
            }
            skip = 1;
        }
        if (g_fw && g_fw->flash_progress) g_fw->flash_progress(0, "下载固件…");
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link downloadFile '%ls' @0x%08X skipErase=%d verify=%d",
                            req->path, addr, skip, req->verify);
        InterlockedExchange(&g_accept_flash_progress, 1);
        rc = g_ctx.api.download_file(req->path, addr,
                                     (unsigned)skip,
                                     (unsigned)(req->verify ? 1 : 0), NULL);
        InterlockedExchange(&g_accept_flash_progress, 0);
        if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link downloadFile rc=%d", rc);
        /* 若未先 massErase 且仍失败：再试 massErase + skip 下载 */
        if (rc != 0 && !req->skip_erase && g_ctx.api.mass_erase) {
            if (g_fw) g_fw->log(OS_LOG_WARN, "ST-Link downloadFile 失败，重试 massErase+skipErase");
            InterlockedExchange(&g_accept_flash_progress, 1);
            if (g_ctx.api.mass_erase(NULL) == 0) {
                rc = g_ctx.api.download_file(req->path, addr, 1,
                                             (unsigned)(req->verify ? 1 : 0), NULL);
                if (g_fw) g_fw->log(OS_LOG_INFO, "ST-Link downloadFile 重试 rc=%d", rc);
            }
            InterlockedExchange(&g_accept_flash_progress, 0);
        }
        ok = (rc == 0);
        if (ok && req->reset_run) {
            if (g_ctx.api.reset) g_ctx.api.reset(OS_STLINK_RST_SOFTWARE);
            if (g_ctx.api.execute) g_ctx.api.execute(addr);
        }
        stlink_leave_flash_mode();
        LeaveCriticalSection(&g_ctx.cs);
        return ok ? OS_ERR_OK : OS_ERR_FAIL;
    }
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
        if (g_ctx.api.set_loaders_path && g_ctx.loaders_dir[0]) {
            g_ctx.api.set_loaders_path(g_ctx.loaders_dir); /* 必需：否则无法识别设备 */
            if (fw) fw->log(OS_LOG_INFO, "stlink 模块: LoadersPath=%s", g_ctx.loaders_dir);
        }
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
    "1.0.1",
    "ST-Link 驱动模块：扫描/连接/读写/烧录 MCU（CubeProgrammer_API）",
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
