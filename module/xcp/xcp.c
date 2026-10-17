/* XCP 驱动：通道下拉里的 "xcp"。
 * 与网络模块无关。网络模块在两台 OpenScope 之间传样本。
 * 本驱动的传输是 CAN（周立功、Kvaser 等），协议层走 XcpIo。
 * 后端还没接入时，扫描结果为空，连接返回没有设备。 */
#include "xcp_can.h"
#include "module_api.h"

#include <stdio.h>
#include <string.h>

typedef struct XcpCtx {
    CRITICAL_SECTION cs;
    const XcpCanBackend* be;
    void* can;
    XcpMaster master;
    int connected;
    OS_DriverInfo info;
} XcpCtx;

typedef struct CanMap {
    const XcpCanBackend* be;
    int local;
} CanMap;

static XcpCtx g_ctx;
static const OS_Framework* g_fw;
static CanMap g_map[16];
static int g_map_n;

static void fill_info(void)
{
    memset(&g_ctx.info, 0, sizeof(g_ctx.info));
    _snprintf(g_ctx.info.name, sizeof(g_ctx.info.name), "xcp");
    _snprintf(g_ctx.info.version, sizeof(g_ctx.info.version), "1.23.1");
    _snprintf(g_ctx.info.dll_version, sizeof(g_ctx.info.dll_version), "1.3");
    if (g_ctx.connected && g_ctx.be)
        _snprintf(g_ctx.info.emulator, sizeof(g_ctx.info.emulator), "XCP CAN %s", g_ctx.be->name);
    else
        _snprintf(g_ctx.info.emulator, sizeof(g_ctx.info.emulator), "未连接");
    g_ctx.info.connected = g_ctx.connected;
}

static int mod_scan(OS_ScanReq* req)
{
    int n = 0, b, i;
    if (!req || !req->items || req->capacity <= 0) return OS_ERR_INVALID_ARG;
    g_map_n = 0;
    for (b = 0; b < xcp_can_backend_count() && n < req->capacity; b++) {
        const XcpCanBackend* be = xcp_can_backend(b);
        char names[8][64];
        int found;
        if (!be || !be->scan) continue;
        found = be->scan(names, 8);
        for (i = 0; i < found && n < req->capacity && g_map_n < 16; i++) {
            memset(&req->items[n], 0, sizeof(req->items[n]));
            req->items[n].index = n;
            _snprintf(req->items[n].name, sizeof(req->items[n].name), "%s", names[i]);
            _snprintf(req->items[n].serial, sizeof(req->items[n].serial), "%s", be->name);
            g_map[g_map_n].be = be;
            g_map[g_map_n].local = i;
            g_map_n++;
            n++;
        }
    }
    req->count = n;
    if (g_fw) {
        if (n == 0)
            g_fw->log(OS_LOG_INFO, "XCP 扫描: 没有 CAN 接口。后续接入周立功、Kvaser；不走 OpenScope 网络");
        else
            g_fw->log(OS_LOG_INFO, "XCP 扫描: %d 个 CAN 接口", n);
    }
    return OS_ERR_OK;
}

static int mod_disconnect(void)
{
    EnterCriticalSection(&g_ctx.cs);
    if (g_ctx.connected) xcp_master_disconnect(&g_ctx.master);
    if (g_ctx.be && g_ctx.be->close && g_ctx.can) g_ctx.be->close(g_ctx.can);
    g_ctx.can = NULL;
    g_ctx.be = NULL;
    g_ctx.connected = 0;
    LeaveCriticalSection(&g_ctx.cs);
    if (g_fw) g_fw->log(OS_LOG_INFO, "XCP 已断开");
    return OS_ERR_OK;
}

static int mod_connect(const OS_ConnectCfg* cfg)
{
    const XcpCanBackend* be;
    XcpIo io;
    void* can = NULL;
    int idx, rc;
    EnterCriticalSection(&g_ctx.cs);
    if (g_ctx.connected) { LeaveCriticalSection(&g_ctx.cs); return OS_ERR_OK; }
    idx = (cfg && cfg->probe_index >= 0) ? cfg->probe_index : 0;
    if (idx < 0 || idx >= g_map_n || !g_map[idx].be) {
        if (g_fw) g_fw->log(OS_LOG_ERROR, "XCP 连接失败: 没有 CAN 接口（后续接入周立功、Kvaser）");
        LeaveCriticalSection(&g_ctx.cs);
        return OS_ERR_NO_DEVICE;
    }
    be = g_map[idx].be;
    idx = g_map[idx].local;
    if (!be->open || !be->bind) {
        LeaveCriticalSection(&g_ctx.cs);
        return OS_ERR_NO_DEVICE;
    }
    rc = be->open(&can, idx);
    if (rc != 0 || !can) {
        if (g_fw) g_fw->log(OS_LOG_ERROR, "XCP 打开 CAN 失败 (%s)", be->name);
        LeaveCriticalSection(&g_ctx.cs);
        return OS_ERR_NO_DEVICE;
    }
    be->bind(can, &io);
    xcp_master_init(&g_ctx.master, &io);
    rc = xcp_master_connect(&g_ctx.master);
    if (rc != 0) {
        if (g_fw) g_fw->log(OS_LOG_ERROR, "XCP CONNECT 失败 rc=%d err=%d", rc, g_ctx.master.last_err);
        if (be->close) be->close(can);
        LeaveCriticalSection(&g_ctx.cs);
        return OS_ERR_FAIL;
    }
    g_ctx.be = be;
    g_ctx.can = can;
    g_ctx.connected = 1;
    LeaveCriticalSection(&g_ctx.cs);
    if (g_fw) g_fw->log(OS_LOG_INFO, "XCP 已连接 %s resource=0x%02X max_cto=%u",
                        be->name, g_ctx.master.resource, g_ctx.master.max_cto);
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
    g_ctx.can = NULL;
    g_ctx.be = NULL;
    if (fw) fw->log(OS_LOG_INFO, "xcp 模块: ASAM XCP 1.3，传输预留 CAN（周立功、Kvaser），与网络模块无关");
    return OS_ERR_OK;
}

static void mod_deinit(void* ctx)
{
    (void)ctx;
    if (g_ctx.connected || g_ctx.can) mod_disconnect();
    DeleteCriticalSection(&g_ctx.cs);
}

static const OS_Module g_module = {
    OS_API_VERSION,
    OS_CAP_DRIVER,
    "xcp",
    "1.23.1",
    "XCP 1.3 协议：经 CAN 适配器上传/下载标定内存",
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
