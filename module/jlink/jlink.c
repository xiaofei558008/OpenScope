/*
 * OpenScope J-Link 驱动模块（jlink.dll）
 *
 * 能力：OS_CAP_DRIVER
 *  - OS_CMD_SCAN      扫描 USB 上的 J-Link 仿真器（EMU_GetNumDevices/GetDeviceInfo）
 *  - OS_CMD_CONNECT   使用主界面控件构造的配置连接 MCU（Bug14：不再弹芯片配置对话框）
 *  - OS_CMD_DISCONNECT 断开（JLINKARM_Close）
 *  - OS_CMD_READ_MEM / OS_CMD_WRITE_MEM  内存读写（互斥序列化，AD-11）
 *  - OS_CMD_GET_INFO  驱动/仿真器信息
 *  - OS_CMD_HALT / GO / RESET
 *
 * JLink_x64.dll 与模块同目录（dll/），运行时 LoadLibrary + GetProcAddress 绑定。
 */
#include "jlink.h"
#include "module_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct JLinkCtx {
    OS_JLinkApi api;
    CRITICAL_SECTION cs;      /* 串行化读写（AD-11） */
    OS_ConnectCfg cfg;        /* 配置对话框保存的连接参数 */
    int connected;
    int dll_used;             /* Bug16: 是否已使用过 DLL（首连后置1，后续连接先整库重载防脏会话） */
    ULONGLONG last_reconnect_ms; /* 自动重连节流时间戳 */
    ULONGLONG last_reconnect_log_ms; /* 重连成功日志节流时间戳（Bug10: 5s 一次） */
    char dll_dir[MAX_PATH];
    OS_DriverInfo info;
} JLinkCtx;

static JLinkCtx g_ctx;
static const OS_Framework* g_fw;
static HMODULE g_hmod;

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
    DWORD n = GetModuleFileNameA(g_hmod ? g_hmod : GetModuleHandleA("jlink.dll"), buf, MAX_PATH);
    if (n == 0) { _snprintf(out, cap, "JLink_x64.dll"); return; }
    slash = strrchr(buf, '\\');
    if (slash) *slash = 0;
    _snprintf(out, cap, "%s", buf);
}

int os_jlink_bind(OS_JLinkApi* api, char* err, int err_len)
{
    char path[MAX_PATH];
    HMODULE h;
    if (err && err_len > 0) err[0] = 0;
    memset(api, 0, sizeof(*api));
    module_dir(g_ctx.dll_dir, sizeof(g_ctx.dll_dir));
    _snprintf(path, sizeof(path), "%s\\JLink_x64.dll", g_ctx.dll_dir);
    h = LoadLibraryA(path);
    if (!h) {
        if (err) _snprintf(err, err_len, "加载 JLink_x64.dll 失败: %s (err=%lu)", path, GetLastError());
        return OS_ERR_FAIL;
    }
    api->h = h;
#define BIND(field, sym) do { \
        *(void**)&api->field = (void*)GetProcAddress(h, "JLINKARM_" sym); \
        if (!api->field) { \
            if (err) _snprintf(err, err_len, "缺少导出 JLINKARM_" sym); \
            return OS_ERR_FAIL; \
        } \
    } while (0)
    BIND(open, "Open");
    BIND(close, "Close");
    BIND(is_connected, "IsConnected");
    BIND(exec_cmd, "ExecCommand");
    BIND(connect, "Connect");
    BIND(tif_select, "TIF_Select");
    BIND(halt, "Halt");
    BIND(go, "Go");
    BIND(reset, "Reset");
    BIND(set_speed, "SetSpeed");
    BIND(read_mem, "ReadMem");
    BIND(write_mem, "WriteMem");
    BIND(get_dll_version, "GetDLLVersion");
    BIND(get_hw_version, "GetHardwareVersion");
    BIND(get_emu_caps, "GetEmuCaps");
    BIND(emu_get_num_devices, "EMU_GetNumDevices");
    BIND(emu_get_list, "EMU_GetList");
    BIND(emu_select_by_index, "EMU_SelectByIndex");
    BIND(emu_select_by_usbsn, "EMU_SelectByUSBSN");
#undef BIND
    /* 可选导出 */
    api->get_fw_string = (void (*)(char*, int))GetProcAddress(h, "JLINKARM_GetFirmwareString");
    return OS_ERR_OK;
}

void os_jlink_unbind(OS_JLinkApi* api)
{
    if (api->h) {
        FreeLibrary(api->h);
        api->h = NULL;
    }
}

/* Bug16：高速（12000kHz）采集中掉线/断开后，JLink_x64.dll 的会话内部状态可能损坏，
 * 表现为后续 JLINKARM_Open 返回垃圾值（用户实测 rc=-488389840，本机实测 rc=-637353168）。
 * 这种损坏无法靠重试 open 恢复，唯一可靠复位是整库重载：FreeLibrary + LoadLibrary + 重绑符号。 */
static int jlink_reload(void)
{
    char err[256];
    int rc;
    /* 用 TryEnter 而非 Enter：若采集线程卡死在 JLINKARM_ReadMem 内（DLL 内部挂起），
     * 锁被占住——此时绝不能 FreeLibrary 一个仍有活动调用的 DLL（进程会崩），
     * 干净地失败即可；等卡死的读返回后，下一次连接重载自然成功。 */
    if (!TryEnterCriticalSection(&g_ctx.cs)) {
        if (g_fw) g_fw->log(OS_LOG_WARN, "J-Link 连接: J-Link 读取疑似卡死，跳过 DLL 重载");
        return OS_ERR_FAIL;
    }
    g_ctx.connected = 0;
    os_jlink_unbind(&g_ctx.api);
    rc = os_jlink_bind(&g_ctx.api, err, sizeof(err));
    LeaveCriticalSection(&g_ctx.cs);
    if (rc != OS_ERR_OK) {
        if (g_fw) g_fw->log(OS_LOG_ERROR, "J-Link 连接: 重载 JLink_x64.dll 失败: %s", err);
        return OS_ERR_FAIL;
    }
    if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 连接: JLink_x64.dll 已重载（DLL v%u）",
                        (unsigned)g_ctx.api.get_dll_version());
    return OS_ERR_OK;
}

/* ---------------- 连接/读写实现 ---------------- */

static int jlink_do_connect(const OS_ConnectCfg* cfg, char* errbuf, int errlen)
{
    OS_JLinkApi* a = &g_ctx.api;
    char cmd[192];
    char res[256];
    int rc;
    if (errbuf && errlen > 0) errbuf[0] = 0;
    if (g_fw) {
        g_fw->log(OS_LOG_INFO,
                  "J-Link 连接: device='%s' iface=%s speed_khz=%d probe_idx=%d serial='%s'",
                  cfg->device[0] ? cfg->device : "(空)",
                  cfg->iface == OS_IF_JTAG ? "JTAG" : "SWD",
                  cfg->speed_khz, cfg->probe_index,
                  cfg->serial[0] ? cfg->serial : "(空)");
    }
    if (!a->h) {
        if (errbuf) _snprintf(errbuf, errlen, "JLink_x64.dll 未加载");
        if (g_fw) g_fw->log(OS_LOG_ERROR, "J-Link 连接: JLink_x64.dll 未加载");
        return OS_ERR_FAIL;
    }
    /* Bug16：高速（12000kHz）掉线/采集中断开后，DLL 会话状态可能损坏——open 返回垃圾值
     * （用户 -488389840、本机 -637353168）甚至挂起。首连用初始绑定；此后每次连接都先整库
     * 重载（FreeLibrary+LoadLibrary+重绑），保证 open 永远从全新会话开始，杜绝脏会话复用。 */
    if (g_ctx.dll_used) {
        if (jlink_reload() != OS_ERR_OK) {
            if (errbuf) _snprintf(errbuf, errlen, "JLink_x64.dll 重载失败");
            return OS_ERR_FAIL;
        }
        a = &g_ctx.api;
    }
    g_ctx.dll_used = 1;
    /* Bug16(新)+需求21：仿真器选择必须在 JLINKARM_Open 之前调用（J-Link SDK 契约）。
     * 原实现 open 之后才 EMU_SelectByIndex——DLL 内部会话状态不一致，实测该调用返回 -1
     * 后 DLL 内部 AV 崩溃（JLink_x64.dll+0x18C36E，连接即闪退，与变量个数无关）。
     * 移到 open 前可正确选中仿真器（rc=0），同时避免 DLL 弹出"Device Selection"设备选择框。 */
    if (cfg->serial[0]) {
        unsigned long sn = strtoul(cfg->serial, NULL, 10);
        if (a->emu_select_by_usbsn) {
            rc = a->emu_select_by_usbsn((uint32_t)sn);
            if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 连接: EMU_SelectByUSBSN(%lu) rc=%d", sn, rc);
        }
    } else if (cfg->probe_index >= 0) {
        if (a->emu_select_by_index) {
            rc = a->emu_select_by_index(cfg->probe_index);
            if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 连接: EMU_SelectByIndex(%d) rc=%d",
                                cfg->probe_index, rc);
        }
    }
    rc = a->open(NULL);
    if (rc != 0) {
        /* 兜底：首连失败或重载后仍失败（硬件/USB 级问题），再重载重试一次 */
        if (g_fw) g_fw->log(OS_LOG_WARN, "J-Link 连接: JLINKARM_Open 失败 rc=%d，重载 JLink DLL 后重试", rc);
        if (jlink_reload() == OS_ERR_OK) {
            a = &g_ctx.api; /* 重载后符号指针已重绑 */
            rc = a->open(NULL);
            if (rc != 0 && g_fw)
                g_fw->log(OS_LOG_ERROR, "J-Link 连接: 重载 DLL 后 JLINKARM_Open 仍失败 rc=%d", rc);
        }
        if (rc != 0) {
            if (errbuf) _snprintf(errbuf, errlen, "JLINKARM_Open 失败 (rc=%d)", rc);
            if (g_fw) g_fw->log(OS_LOG_ERROR, "J-Link 连接: JLINKARM_Open 失败 rc=%d", rc);
            return OS_ERR_FAIL;
        }
    }
    /* 需求21：open 成功后抑制 DLL 的信息弹窗（如固件更新提示等），并重设设备名兜底 */
    _snprintf(cmd, sizeof(cmd), "SuppressInfoDialogs = 1");
    memset(res, 0, sizeof(res));
    rc = a->exec_cmd(cmd, res, sizeof(res));
    if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 连接: '%s' rc=%d", cmd, rc);
    if (a->tif_select) {
        rc = a->tif_select(cfg->iface == OS_IF_JTAG ? 0 : 1);
        if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 连接: TIF_Select(%s) rc=%d",
                            cfg->iface == OS_IF_JTAG ? "JTAG" : "SWD", rc);
    }
    _snprintf(cmd, sizeof(cmd), "Device = %s", cfg->device[0] ? cfg->device : "Cortex-M4");
    memset(res, 0, sizeof(res));
    rc = a->exec_cmd(cmd, res, sizeof(res));
    if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 连接: '%s' rc=%d -> %s", cmd, rc, res[0] ? res : "(空)");
    /* 时钟速度 */
    if (cfg->speed_khz > 0) {
        rc = a->set_speed(cfg->speed_khz);
        if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 连接: SetSpeed(%d) rc=%d", cfg->speed_khz, rc);
    }
    rc = a->is_connected();
    if (!rc) rc = a->connect();
    if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 连接: Connect rc=%d", rc);
    if (rc != 0) {
        if (errbuf) _snprintf(errbuf, errlen, "JLINKARM_Connect 失败 (rc=%d)，请检查目标板/电源/接线", rc);
        a->close();
        return OS_ERR_NO_DEVICE;
    }
    g_ctx.connected = 1;
    return OS_ERR_OK;
}

static void jlink_refresh_info(void)
{
    OS_JLinkApi* a = &g_ctx.api;
    OS_DriverInfo* d = &g_ctx.info;
    memset(d, 0, sizeof(*d));
    _snprintf(d->name, sizeof(d->name), "%s", "jlink");
    _snprintf(d->version, sizeof(d->version), "%s", "1.15.0");
    if (a->get_dll_version) _snprintf(d->dll_version, sizeof(d->dll_version), "%d", a->get_dll_version());
    if (a->get_hw_version) d->hw_version = a->get_hw_version();
    if (a->get_fw_string && g_ctx.connected) {
        char fwbuf[128];
        memset(fwbuf, 0, sizeof(fwbuf));
        a->get_fw_string(fwbuf, sizeof(fwbuf));
        _snprintf(d->emulator, sizeof(d->emulator), "%s", fwbuf);
        d->fw_version = atoi(fwbuf);
    } else {
        _snprintf(d->emulator, sizeof(d->emulator), "%s", "未连接");
    }
    d->connected = g_ctx.connected;
}

/* ---------------- 模块命令 ---------------- */

static int mod_scan(OS_ScanReq* req)
{
    OS_JLinkApi* a = &g_ctx.api;
    int n, i;
    if (!req || !req->items || req->capacity <= 0) return OS_ERR_INVALID_ARG;
    if (!a->h) return OS_ERR_FAIL;
    n = a->emu_get_list(OS_JLINK_HOST_USB_OR_IP, NULL, 0);
    if (n < 0) n = 0;
    if (n > req->capacity) n = req->capacity;
    if (n > 0) {
        OS_JLinkEmuInfo* infos = (OS_JLinkEmuInfo*)calloc((size_t)n, sizeof(OS_JLinkEmuInfo));
        if (infos) {
            int got = a->emu_get_list(OS_JLINK_HOST_USB_OR_IP, infos, n);
            if (got < 0) got = 0;
            if (got > n) got = n;
            for (i = 0; i < got; i++) {
                OS_DeviceInfo* it = &req->items[i];
                memset(it, 0, sizeof(*it));
                it->index = i;
                _snprintf(it->serial, sizeof(it->serial), "%08lu",
                          (unsigned long)infos[i].SerialNumber);
                if (infos[i].acProduct[0])
                    _snprintf(it->name, sizeof(it->name), "%s", infos[i].acProduct);
                else if (infos[i].acNickname[0])
                    _snprintf(it->name, sizeof(it->name), "%s", infos[i].acNickname);
                else
                    _snprintf(it->name, sizeof(it->name), "J-Link #%d", i);
            }
            free(infos);
            n = got;
        }
    }
    req->count = n;
    if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 扫描: 发现 %d 个设备", n);
    return OS_ERR_OK;
}

static int mod_connect_ex(char* err, int errlen)
{
    char ebuf[256];
    int rc;
    if (g_ctx.connected) return OS_ERR_OK; /* 已在对话框内连接 */
    rc = jlink_do_connect(&g_ctx.cfg, ebuf, sizeof(ebuf));
    if (rc != OS_ERR_OK && g_ctx.cfg.probe_index >= 0) {
        /* EMU_SelectByIndex 在部分旧固件上返回 -1，去掉显式选择后自动重试一次 */
        OS_ConnectCfg saved = g_ctx.cfg;
        g_ctx.cfg.probe_index = -1;
        g_ctx.cfg.serial[0] = 0;
        if (g_fw) g_fw->log(OS_LOG_WARN, "J-Link 连接: 显式选择仿真器失败，改用自动选择重试");
        rc = jlink_do_connect(&g_ctx.cfg, ebuf, sizeof(ebuf));
        if (rc != OS_ERR_OK) g_ctx.cfg = saved;
    }
    if (rc == OS_ERR_OK) {
        jlink_refresh_info();
        if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 已连接: %s", g_ctx.info.emulator);
    } else {
        g_ctx.connected = 0;
        if (g_fw) g_fw->log(OS_LOG_ERROR, "J-Link 连接失败: %s", ebuf);
        if (err && errlen > 0) _snprintf(err, errlen, "%s", ebuf);
    }
    return rc;
}

static int mod_disconnect(void)
{
    OS_JLinkApi* a = &g_ctx.api;
    if (a->h && g_ctx.connected) {
        /* Bug16：close 与 in-flight 读互斥——采集中点击断开时，采集线程可能正卡在
         * JLINKARM_ReadMem 内；并发 close 会损坏 DLL 会话（Open 返回垃圾值/内部 AV）。
         * 用 TryEnter 不阻塞 UI：若读正在执行（锁被占）则跳过 close，交给下一次连接前的
         * 整库重载清理（jlink_reload 持锁等读返回后才 FreeLibrary，同样安全）。 */
        if (TryEnterCriticalSection(&g_ctx.cs)) {
            if (a->h && g_ctx.connected) a->close();
            LeaveCriticalSection(&g_ctx.cs);
        }
    }
    g_ctx.connected = 0;
    memset(&g_ctx.info, 0, sizeof(g_ctx.info));
    if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 已断开");
    return OS_ERR_OK;
}

/* 自动重连节流：掉线后至少间隔这么久才再次重连，避免边缘目标高频重连刷日志 */
#define OS_JLINK_RECONNECT_MIN_MS 500
/* Bug10: 重连成功日志节流（5s 一次）。边缘目标高速下每 500ms 掉线重连一次，
 * 若每次打日志会 2 条/秒刷屏；采集线程已改为时间基停摆判定，无需每次重连都提醒。 */
#define OS_JLINK_RECONNECT_LOG_MS 5000

static int mod_read(OS_MemReq* req)
{
    OS_JLinkApi* a = &g_ctx.api;
    int r, retried = 0;
    if (!req || !req->data) return OS_ERR_INVALID_ARG;
    if (!a->h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
    for (;;) {
        EnterCriticalSection(&g_ctx.cs);
        r = a->read_mem((uint32_t)req->address, req->size, (uint8_t*)req->data);
        LeaveCriticalSection(&g_ctx.cs);
        /* JLINKARM_ReadMem 成功返回 0；失败返回正值（未能读取的字节数）或负错误码。
         * 严禁用 r>=0 判定成功——失败返回 1 会被当成成功，零缓冲被推为有效样本（Bug 9）。 */
        if (r == 0) return (int)req->size;
        if (retried) break; /* 已重连重试过，不再重试 */
        /* 掉线（IsConnected=0）时自动重连一次恢复采集（Bug 9：避免永久全 0） */
        if (a->is_connected && !a->is_connected() &&
            (ULONGLONG)GetTickCount64() - g_ctx.last_reconnect_ms >= OS_JLINK_RECONNECT_MIN_MS) {
            if (g_fw && (ULONGLONG)GetTickCount64() - g_ctx.last_reconnect_log_ms >= OS_JLINK_RECONNECT_LOG_MS) {
                g_ctx.last_reconnect_log_ms = (ULONGLONG)GetTickCount64();
                g_fw->log(OS_LOG_WARN, "J-Link 读取失败：连接丢失，自动重连恢复");
            }
            mod_disconnect();
            /* Bug16：高速掉线后 DLL/USB 会话短暂损坏，open 可能返回垃圾值（-488389840）；
             * 重载已给全新会话，但 USB 层恢复仍需时间——重试数次（间隔 1.5s），
             * 任一成功即恢复采集，不轻易让采集线程退出。 */
            {
                int attempt;
                for (attempt = 0; attempt < 3; attempt++) {
                    if (mod_connect_ex(NULL, 0) == OS_ERR_OK) {
                        g_ctx.last_reconnect_ms = (ULONGLONG)GetTickCount64();
                        retried = 1;
                        break;
                    }
                    if (attempt < 2) Sleep(1500);
                }
            }
            if (retried) continue;
        }
        break;
    }
    return OS_ERR_TIMEOUT;
}

static int mod_write(OS_MemReq* req)
{
    OS_JLinkApi* a = &g_ctx.api;
    int r;
    if (!req || !req->data) return OS_ERR_INVALID_ARG;
    if (!a->h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
    EnterCriticalSection(&g_ctx.cs);
    r = a->write_mem((uint32_t)req->address, req->size, (const uint8_t*)req->data);
    LeaveCriticalSection(&g_ctx.cs);
    /* JLINKARM_WriteMem 成功返回 0（>0 表示未能写入的字节数，负值为错误） */
    return r == 0 ? OS_ERR_OK : OS_ERR_TIMEOUT;
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
            jlink_refresh_info();
            memcpy(out, &g_ctx.info, sizeof(g_ctx.info));
        }
        return OS_ERR_OK;
    case OS_CMD_HALT:
        if (g_ctx.api.h && g_ctx.connected) return g_ctx.api.halt();
        return OS_ERR_NOT_CONNECTED;
    case OS_CMD_GO:
        if (g_ctx.api.h && g_ctx.connected) return g_ctx.api.go();
        return OS_ERR_NOT_CONNECTED;
    case OS_CMD_RESET:
        if (g_ctx.api.h && g_ctx.connected) return g_ctx.api.reset();
        return OS_ERR_NOT_CONNECTED;
    case OS_CMD_ELF_RELOADED:
        if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 模块: ELF 已重载");
        return OS_ERR_OK;
    default:
        return OS_ERR_FAIL;
    }
}

static int mod_init(const OS_Framework* fw, void** out_ctx)
{
    char err[256] = "";
    (void)out_ctx;
    g_fw = fw;
    InitializeCriticalSection(&g_ctx.cs);
    memset(&g_ctx.cfg, 0, sizeof(g_ctx.cfg));
    g_ctx.cfg.iface = OS_IF_SWD;
    g_ctx.cfg.speed_khz = 4000;
    g_ctx.cfg.probe_index = -1;
    if (os_jlink_bind(&g_ctx.api, err, sizeof(err)) != OS_ERR_OK) {
        if (fw) fw->log(OS_LOG_WARN, "jlink 模块: %s", err);
    } else if (fw) {
        fw->log(OS_LOG_INFO, "jlink 模块: 已绑定 JLink_x64.dll (DLL v%d)",
                g_ctx.api.get_dll_version ? g_ctx.api.get_dll_version() : 0);
    }
    return OS_ERR_OK;
}

static void mod_deinit(void* ctx)
{
    (void)ctx;
    mod_disconnect();
    os_jlink_unbind(&g_ctx.api);
    DeleteCriticalSection(&g_ctx.cs);
}

static const OS_Module g_module = {
    OS_API_VERSION,
    OS_CAP_DRIVER,
    "jlink",
    "1.15.0",
    "J-Link 驱动模块：扫描/连接/读写 MCU 内存",
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
