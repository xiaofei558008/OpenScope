/* XCP 模块：与探针（J-Link/ST-Link）、网络模块并行。
 * 能力 OS_CAP_XCP，不占用仿真器下拉。
 * 传输经 XcpTransport：serial / can / tcp（物理层无关）。 */
#include "xcp_transport.h"
#include "module_api.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef struct XcpCtx {
    CRITICAL_SECTION cs;
    const XcpTransport* tr;
    void* link;
    XcpMaster master;
    int connected;
    OS_DriverInfo info;
    char last_opts[128];
} XcpCtx;

typedef struct TrMap {
    const XcpTransport* tr;
    int local;
    char name[64];
} TrMap;

static XcpCtx g_ctx;
static const OS_Framework* g_fw;
static TrMap g_map[32];
static int g_map_n;

static void xlog(int level, const char* fmt, ...)
{
    char buf[512];
    va_list ap;
    if (!g_fw || !g_fw->log) return;
    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    g_fw->log(level, "%s", buf);
}

static void fill_info(void)
{
    memset(&g_ctx.info, 0, sizeof(g_ctx.info));
    _snprintf(g_ctx.info.name, sizeof(g_ctx.info.name), "xcp");
    _snprintf(g_ctx.info.version, sizeof(g_ctx.info.version), "1.27.0");
    _snprintf(g_ctx.info.dll_version, sizeof(g_ctx.info.dll_version), "1.4");
    if (g_ctx.connected && g_ctx.tr)
        _snprintf(g_ctx.info.emulator, sizeof(g_ctx.info.emulator),
                  "XCP/%s %s", g_ctx.tr->name, g_ctx.last_opts);
    else
        _snprintf(g_ctx.info.emulator, sizeof(g_ctx.info.emulator), "未连接");
    g_ctx.info.connected = g_ctx.connected;
}

static int mod_scan(OS_ScanReq* req)
{
    int n = 0, t, i;
    if (!req || !req->items || req->capacity <= 0) return OS_ERR_INVALID_ARG;
    g_map_n = 0;
    for (t = 0; t < xcp_transport_count() && n < req->capacity; t++) {
        const XcpTransport* tr = xcp_transport_at(t);
        char names[16][64];
        int found;
        if (!tr || !tr->scan) continue;
        found = tr->scan(names, 16);
        for (i = 0; i < found && n < req->capacity && g_map_n < 32; i++) {
            memset(&req->items[n], 0, sizeof(req->items[n]));
            req->items[n].index = n;
            _snprintf(req->items[n].name, sizeof(req->items[n].name),
                      "%s:%s", tr->name, names[i]);
            _snprintf(req->items[n].serial, sizeof(req->items[n].serial), "%s", tr->name);
            g_map[g_map_n].tr = tr;
            g_map[g_map_n].local = i;
            _snprintf(g_map[g_map_n].name, sizeof(g_map[g_map_n].name), "%s", names[i]);
            g_map_n++;
            n++;
        }
    }
    req->count = n;
    xlog(OS_LOG_INFO, "XCP 扫描: %d 个端点（serial/can/tcp）", n);
    return OS_ERR_OK;
}

static int mod_disconnect(void)
{
    EnterCriticalSection(&g_ctx.cs);
    if (g_ctx.connected) xcp_master_disconnect(&g_ctx.master);
    if (g_ctx.tr && g_ctx.tr->close && g_ctx.link) g_ctx.tr->close(g_ctx.link);
    g_ctx.link = NULL;
    g_ctx.tr = NULL;
    g_ctx.connected = 0;
    g_ctx.last_opts[0] = 0;
    LeaveCriticalSection(&g_ctx.cs);
    xlog(OS_LOG_INFO, "XCP 已断开");
    return OS_ERR_OK;
}

/* device=transport 名（serial/can/tcp）；serial=端口或 host:port；
 * speed_khz=串口波特率（>0）；probe_index=扫描下标。 */
static int mod_connect(const OS_ConnectCfg* cfg)
{
    const XcpTransport* tr = NULL;
    XcpIo io;
    void* link = NULL;
    char opts[160];
    int idx, rc, baud;
    EnterCriticalSection(&g_ctx.cs);
    if (g_ctx.connected) { LeaveCriticalSection(&g_ctx.cs); return OS_ERR_OK; }

    opts[0] = 0;
    if (cfg && cfg->device[0])
        tr = xcp_transport_by_name(cfg->device);
    if (!tr && cfg && cfg->probe_index >= 0 && cfg->probe_index < g_map_n)
        tr = g_map[cfg->probe_index].tr;
    if (!tr)
        tr = xcp_transport_by_name("serial");
    if (!tr || !tr->open || !tr->bind) {
        xlog(OS_LOG_ERROR, "XCP 连接失败: 无可用传输");
        LeaveCriticalSection(&g_ctx.cs);
        return OS_ERR_NO_DEVICE;
    }

    baud = (cfg && cfg->speed_khz > 0) ? cfg->speed_khz : 115200;
    if (cfg && cfg->serial[0]) {
        if (strcmp(tr->name, "serial") == 0 && strchr(cfg->serial, ':') == NULL)
            _snprintf(opts, sizeof(opts), "%s:%d", cfg->serial, baud);
        else
            _snprintf(opts, sizeof(opts), "%s", cfg->serial);
        idx = 0;
    } else if (cfg && cfg->probe_index >= 0 && cfg->probe_index < g_map_n &&
               g_map[cfg->probe_index].tr == tr) {
        if (strcmp(tr->name, "serial") == 0)
            _snprintf(opts, sizeof(opts), "%s:%d", g_map[cfg->probe_index].name, baud);
        else
            _snprintf(opts, sizeof(opts), "%s", g_map[cfg->probe_index].name);
        idx = g_map[cfg->probe_index].local;
    } else {
        idx = 0;
        opts[0] = 0;
    }

    xlog(OS_LOG_INFO, "XCP 连接: transport=%s opts='%s'", tr->name, opts);
    rc = tr->open(&link, idx, opts[0] ? opts : NULL);
    if (rc != 0 || !link) {
        xlog(OS_LOG_ERROR, "XCP 打开传输失败 (%s)", tr->name);
        LeaveCriticalSection(&g_ctx.cs);
        return OS_ERR_NO_DEVICE;
    }
    tr->bind(link, &io);
    if (!io.send || !io.recv) {
        if (tr->close) tr->close(link);
        LeaveCriticalSection(&g_ctx.cs);
        return OS_ERR_FAIL;
    }
    xcp_master_init(&g_ctx.master, &io);
    rc = xcp_master_connect(&g_ctx.master);
    if (rc != 0) {
        xlog(OS_LOG_ERROR, "XCP CONNECT 失败 rc=%d err=%d", rc, g_ctx.master.last_err);
        if (tr->close) tr->close(link);
        LeaveCriticalSection(&g_ctx.cs);
        return OS_ERR_FAIL;
    }
    g_ctx.tr = tr;
    g_ctx.link = link;
    g_ctx.connected = 1;
    _snprintf(g_ctx.last_opts, sizeof(g_ctx.last_opts), "%s", opts);
    LeaveCriticalSection(&g_ctx.cs);
    xlog(OS_LOG_INFO, "XCP 已连接 %s resource=0x%02X max_cto=%u",
         tr->name, g_ctx.master.resource, g_ctx.master.max_cto);
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
    if (rc != 0) xlog(OS_LOG_WARN, "XCP UPLOAD 失败 addr=0x%08X size=%u",
                      (unsigned)req->address, (unsigned)req->size);
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
    if (rc != 0) xlog(OS_LOG_WARN, "XCP DOWNLOAD 失败 addr=0x%08X", (unsigned)req->address);
    return rc == 0 ? OS_ERR_OK : OS_ERR_TIMEOUT;
}

static int mod_ai_describe(char* out)
{
    if (!out) return OS_ERR_INVALID_ARG;
    _snprintf(out, 4096,
        "["
        "{\"name\":\"xcp.scan\",\"desc\":\"扫描 XCP 传输端点（串口/CAN/TCP）\",\"params\":\"{}\"},"
        "{\"name\":\"xcp.connect\",\"desc\":\"连接 XCP\",\"params\":\"{\\\"transport\\\":\\\"serial\\\",\\\"port\\\":\\\"COM3\\\",\\\"baud\\\":115200}\"},"
        "{\"name\":\"xcp.disconnect\",\"desc\":\"断开 XCP\",\"params\":\"{}\"},"
        "{\"name\":\"xcp.status\",\"desc\":\"XCP 连接状态\",\"params\":\"{}\"}"
        "]");
    return OS_ERR_OK;
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
    case OS_CMD_AI_DESCRIBE:
        return mod_ai_describe((char*)out);
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
    g_ctx.link = NULL;
    g_ctx.tr = NULL;
    xcp_transport_init_builtins();
    xlog(OS_LOG_INFO,
         "xcp 模块: ASAM XCP 1.3，传输 serial/can/tcp（与探针、网络并行）");
    return OS_ERR_OK;
}

static void mod_deinit(void* ctx)
{
    (void)ctx;
    if (g_ctx.connected || g_ctx.link) mod_disconnect();
    DeleteCriticalSection(&g_ctx.cs);
}

static const OS_Module g_module = {
    OS_API_VERSION,
    OS_CAP_XCP,
    "xcp",
    "1.27.0",
    "XCP 1.3：串口/CAN/TCP 传输（物理层无关），与探针/网络并行",
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
