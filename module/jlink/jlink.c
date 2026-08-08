/*
 * OpenScope J-Link 驱动模块（jlink.dll）
 *
 * 能力：OS_CAP_DRIVER
 *  - OS_CMD_SCAN      扫描 USB 上的 J-Link 仿真器（EMU_GetNumDevices/GetDeviceInfo）
 *  - OS_CMD_CONFIGURE 弹配置对话框（SWD/JTAG、时钟速度、目标器件、连接/断开）
 *  - OS_CMD_CONNECT   使用对话框保存的配置连接 MCU
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

OS_ConnectCfg* os_jlink_cfg(void) { return &g_ctx.cfg; }

int os_jlink_scan_devices(OS_DeviceInfo* items, int cap)
{
    OS_ScanReq req;
    req.items = items;
    req.capacity = cap;
    req.count = 0;
    return mod_scan(&req);
}

int os_jlink_connect_now(char* err, int errlen)
{
    return mod_connect_ex(err, errlen);
}

int os_jlink_disconnect_now(void)
{
    return mod_disconnect();
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
    api->get_fw_string = (char* (*)(void))GetProcAddress(h, "JLINKARM_GetFirmwareString");
    return OS_ERR_OK;
}

void os_jlink_unbind(OS_JLinkApi* api)
{
    if (api->h) {
        FreeLibrary(api->h);
        api->h = NULL;
    }
}

/* ---------------- 连接/读写实现 ---------------- */

static int jlink_do_connect(const OS_ConnectCfg* cfg, char* errbuf, int errlen)
{
    OS_JLinkApi* a = &g_ctx.api;
    char cmd[192];
    char res[256];
    int rc;
    if (errbuf && errlen > 0) errbuf[0] = 0;
    if (!a->h) {
        if (errbuf) _snprintf(errbuf, errlen, "JLink_x64.dll 未加载");
        return OS_ERR_FAIL;
    }
    if (a->open(NULL) != 0) {
        if (errbuf) _snprintf(errbuf, errlen, "JLINKARM_Open 失败");
        return OS_ERR_FAIL;
    }
    /* 选择仿真器（按序号，若有） */
    if (cfg->serial[0]) {
        unsigned long sn = strtoul(cfg->serial, NULL, 10);
        if (a->emu_select_by_usbsn((uint32_t)sn) != 0) {
            _snprintf(cmd, sizeof(cmd), "SelectEmuBySN = %s", cfg->serial);
            a->exec_cmd(cmd, res);
        }
    } else if (cfg->probe_index >= 0) {
        if (a->emu_select_by_index(cfg->probe_index) != 0) {
            _snprintf(cmd, sizeof(cmd), "SelectEmuByIndex = %d", cfg->probe_index);
            a->exec_cmd(cmd, res);
        }
    }
    /* 接口 */
    _snprintf(cmd, sizeof(cmd), "SelectInterface = %s", cfg->iface == OS_IF_JTAG ? "JTAG" : "SWD");
    a->exec_cmd(cmd, res);
    /* 目标器件 */
    if (cfg->device[0]) {
        _snprintf(cmd, sizeof(cmd), "SetDevice = %s", cfg->device);
        a->exec_cmd(cmd, res);
    }
    /* 时钟速度 */
    if (cfg->speed_khz > 0) {
        a->set_speed(cfg->speed_khz);
    }
    /* 连接 */
    rc = a->connect();
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
    char tmp[64];
    memset(d, 0, sizeof(*d));
    _snprintf(d->name, sizeof(d->name), "%s", "jlink");
    _snprintf(d->version, sizeof(d->version), "%s", "0.1.0");
    if (a->get_dll_version) _snprintf(d->dll_version, sizeof(d->dll_version), "%d", a->get_dll_version());
    if (a->get_hw_version) d->hw_version = a->get_hw_version();
    if (a->get_fw_string && g_ctx.connected) {
        const char* s = a->get_fw_string();
        if (s) {
            _snprintf(tmp, sizeof(tmp), "%s", s);
            _snprintf(d->emulator, sizeof(d->emulator), "%s", tmp);
            d->fw_version = atoi(tmp);
        }
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
    if (g_ctx.connected) return OS_ERR_OK; /* 已在对话框内连接 */
    int rc = jlink_do_connect(&g_ctx.cfg, ebuf, sizeof(ebuf));
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
    if (a->h && g_ctx.connected) a->close();
    g_ctx.connected = 0;
    memset(&g_ctx.info, 0, sizeof(g_ctx.info));
    if (g_fw) g_fw->log(OS_LOG_INFO, "J-Link 已断开");
    return OS_ERR_OK;
}

static int mod_read(OS_MemReq* req)
{
    OS_JLinkApi* a = &g_ctx.api;
    int r;
    if (!req || !req->data) return OS_ERR_INVALID_ARG;
    if (!a->h || !g_ctx.connected) return OS_ERR_NOT_CONNECTED;
    EnterCriticalSection(&g_ctx.cs);
    r = a->read_mem((uint32_t)req->address, req->size, (uint8_t*)req->data);
    LeaveCriticalSection(&g_ctx.cs);
    return r >= 0 ? r : OS_ERR_TIMEOUT;
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
    return r >= 0 ? r : OS_ERR_TIMEOUT;
}

static int mod_command(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx;
    switch (cmd) {
    case OS_CMD_SCAN:
        return mod_scan((OS_ScanReq*)in);
    case OS_CMD_CONFIGURE:
        return os_jlink_show_config_dialog((HWND)in);
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
    "0.1.0",
    "J-Link 驱动模块：扫描/连接/读写 MCU 内存",
    NULL,
    mod_init,
    mod_deinit,
    mod_command,
    NULL, NULL, NULL, NULL
};

const OS_Module* os_module_get(void)
{
    return &g_module;
}
