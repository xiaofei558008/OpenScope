/* XCP 驱动模块：通道下拉里的 "xcp"。
 * 连接用工具栏上的 IP/端口（主程序在 OS_ConnectCfg.serial / speed_khz 里传入），
 * 读写走 SHORT_UPLOAD / DOWNLOAD。传输是 XCP on Ethernet（TCP）。 */
#include "xcp_eth.h"
#include "module_api.h"

#include <stdio.h>
#include <string.h>

typedef struct XcpCtx {
    CRITICAL_SECTION cs;
    XcpEthClient* cli;
    XcpMaster master;
    int connected;
    char host[64];
    int port;
    OS_DriverInfo info;
} XcpCtx;

static XcpCtx g_ctx;
static const OS_Framework* g_fw;

static void fill_info(void)
{
    memset(&g_ctx.info, 0, sizeof(g_ctx.info));
    _snprintf(g_ctx.info.name, sizeof(g_ctx.info.name), "xcp");
    _snprintf(g_ctx.info.version, sizeof(g_ctx.info.version), "1.23.0");
    _snprintf(g_ctx.info.dll_version, sizeof(g_ctx.info.dll_version), "1.3");
    if (g_ctx.connected)
        _snprintf(g_ctx.info.emulator, sizeof(g_ctx.info.emulator),
                  "XCP %s:%d", g_ctx.host, g_ctx.port);
    else
        _snprintf(g_ctx.info.emulator, sizeof(g_ctx.info.emulator), "未连接");
    g_ctx.info.connected = g_ctx.connected;
}

static int mod_scan(OS_ScanReq* req)
{
    if (!req || !req->items || req->capacity <= 0) return OS_ERR_INVALID_ARG;
    memset(&req->items[0], 0, sizeof(req->items[0]));
    req->items[0].index = 0;
    _snprintf(req->items[0].name, sizeof(req->items[0].name), "XCP Ethernet");
    _snprintf(req->items[0].serial, sizeof(req->items[0].serial), "tcp");
    req->count = 1;
    if (g_fw) g_fw->log(OS_LOG_INFO, "XCP 扫描: 以太网从机通道（用工具栏 IP/端口连接）");
    return OS_ERR_OK;
}

static int mod_disconnect(void)
{
    EnterCriticalSection(&g_ctx.cs);
    if (g_ctx.connected) xcp_master_disconnect(&g_ctx.master);
    xcp_eth_close(g_ctx.cli);
    g_ctx.cli = NULL;
    g_ctx.connected = 0;
    LeaveCriticalSection(&g_ctx.cs);
    if (g_fw) g_fw->log(OS_LOG_INFO, "XCP 已断开");
    return OS_ERR_OK;
}

static int mod_connect(const OS_ConnectCfg* cfg)
{
    XcpIo io;
    const char* host;
    int port, rc;
    EnterCriticalSection(&g_ctx.cs);
    if (g_ctx.connected) { LeaveCriticalSection(&g_ctx.cs); return OS_ERR_OK; }
    host = (cfg && cfg->serial[0]) ? cfg->serial : "127.0.0.1";
    port = (cfg && cfg->speed_khz > 0 && cfg->speed_khz <= 65535) ? cfg->speed_khz : 5555;
    _snprintf(g_ctx.host, sizeof(g_ctx.host), "%s", host);
    g_ctx.port = port;
    if (g_fw) g_fw->log(OS_LOG_INFO, "XCP 连接 %s:%d", g_ctx.host, g_ctx.port);
    g_ctx.cli = xcp_eth_connect(g_ctx.host, g_ctx.port);
    if (!g_ctx.cli) {
        if (g_fw) g_fw->log(OS_LOG_ERROR, "XCP TCP 连接失败 %s:%d", g_ctx.host, g_ctx.port);
        LeaveCriticalSection(&g_ctx.cs);
        return OS_ERR_NO_DEVICE;
    }
    xcp_eth_io(g_ctx.cli, &io);
    xcp_master_init(&g_ctx.master, &io);
    rc = xcp_master_connect(&g_ctx.master);
    if (rc != 0) {
        if (g_fw) g_fw->log(OS_LOG_ERROR, "XCP CONNECT 失败 rc=%d err=%d", rc, g_ctx.master.last_err);
        xcp_eth_close(g_ctx.cli);
        g_ctx.cli = NULL;
        LeaveCriticalSection(&g_ctx.cs);
        return OS_ERR_FAIL;
    }
    g_ctx.connected = 1;
    LeaveCriticalSection(&g_ctx.cs);
    if (g_fw) g_fw->log(OS_LOG_INFO, "XCP 已连接 resource=0x%02X max_cto=%u",
                        g_ctx.master.resource, g_ctx.master.max_cto);
    return OS_ERR_OK;
}

static int mod_read(OS_MemReq* req)
{
    int rc;
    if (!req || !req->data || req->size == 0) return OS_ERR_INVALID_ARG;
    if (req->size > 4096) return OS_ERR_INVALID_ARG;
    EnterCriticalSection(&g_ctx.cs);
    if (!g_ctx.connected) { LeaveCriticalSection(&g_ctx.cs); return OS_ERR_NOT_CONNECTED; }
    rc = xcp_master_upload(&g_ctx.master, (uint32_t)req->address, req->data, (int)req->size);
    LeaveCriticalSection(&g_ctx.cs);
    return rc == 0 ? (int)req->size : OS_ERR_TIMEOUT;
}

static int mod_write(OS_MemReq* req)
{
    int rc;
    if (!req || !req->data || req->size == 0) return OS_ERR_INVALID_ARG;
    if (req->size > 4096) return OS_ERR_INVALID_ARG;
    EnterCriticalSection(&g_ctx.cs);
    if (!g_ctx.connected) { LeaveCriticalSection(&g_ctx.cs); return OS_ERR_NOT_CONNECTED; }
    rc = xcp_master_download(&g_ctx.master, (uint32_t)req->address, req->data, (int)req->size);
    LeaveCriticalSection(&g_ctx.cs);
    return rc == 0 ? OS_ERR_OK : OS_ERR_TIMEOUT;
}

static int mod_command(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx;
    switch (cmd) {
    case OS_CMD_SCAN: return mod_scan((OS_ScanReq*)in);
    case OS_CMD_CONNECT: return mod_connect((const OS_ConnectCfg*)in);
    case OS_CMD_DISCONNECT: return mod_disconnect();
    case OS_CMD_IS_CONNECTED:
        if (out) *(int*)out = g_ctx.connected ? 1 : 0;
        return OS_ERR_OK;
    case OS_CMD_READ_MEM: return mod_read((OS_MemReq*)in);
    case OS_CMD_WRITE_MEM: return mod_write((OS_MemReq*)in);
    case OS_CMD_GET_INFO:
        if (out) { fill_info(); memcpy(out, &g_ctx.info, sizeof(g_ctx.info)); }
        return OS_ERR_OK;
    case OS_CMD_HALT:
    case OS_CMD_GO:
    case OS_CMD_RESET:
        return g_ctx.connected ? OS_ERR_OK : OS_ERR_NOT_CONNECTED;
    default:
        return OS_ERR_FAIL;
    }
}

static int mod_init(const OS_Framework* fw, void** out_ctx)
{
    (void)out_ctx;
    g_fw = fw;
    InitializeCriticalSection(&g_ctx.cs);
    g_ctx.connected = 0;
    g_ctx.cli = NULL;
    if (fw) fw->log(OS_LOG_INFO, "xcp 模块: ASAM XCP 1.3（以太网 TCP，默认端口 5555）");
    return OS_ERR_OK;
}

static void mod_deinit(void* ctx)
{
    (void)ctx;
    if (g_ctx.connected || g_ctx.cli) mod_disconnect();
    DeleteCriticalSection(&g_ctx.cs);
}

static const OS_Module g_module = {
    OS_API_VERSION,
    OS_CAP_DRIVER,
    "xcp",
    "1.23.0",
    "XCP 1.3 协议：以太网连接从机，上传/下载标定内存",
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
