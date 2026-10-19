/*
 * OpenScope AI 总线（框架侧）：把全部主机操作暴露为 JSON 方法，
 * 供 aiapi.dll（本机 WebSocket JSON-RPC / MCP）调用。
 * 新增软件能力时在此登记方法，并同步写入 tools.list / help。
 */
#include "app.h"
#include "mainwin.h"
#include "module_mgr.h"
#include "datasrv.h"
#include "datalog.h"
#include "util.h"
#include "version.h"
#include "elf.h"
#include "srcwin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AI_OUT_MIN 256

/* ---------- 极简 JSON 字段提取（仅支持一层对象内的 string/number/bool） ---------- */

static const char* json_skip_ws(const char* p)
{
    while (p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) p++;
    return p;
}

static int json_find_key(const char* j, const char* key, const char** val)
{
    char pat[96];
    const char* p;
    int klen;
    if (!j || !key || !val) return -1;
    klen = (int)strlen(key);
    if (klen <= 0 || klen > 80) return -1;
    _snprintf(pat, sizeof(pat), "\"%s\"", key);
    p = j;
    while ((p = strstr(p, pat)) != NULL) {
        const char* q = p + klen + 2;
        q = json_skip_ws(q);
        if (*q == ':') {
            *val = json_skip_ws(q + 1);
            return 0;
        }
        p += klen + 2;
    }
    return -1;
}

static int json_get_str(const char* j, const char* key, char* out, int cap)
{
    const char* v;
    int i = 0;
    if (json_find_key(j, key, &v) != 0 || !out || cap < 2) return -1;
    if (*v != '"') return -1;
    v++;
    while (*v && *v != '"' && i < cap - 1) {
        if (*v == '\\' && v[1]) { v++; out[i++] = *v++; }
        else out[i++] = *v++;
    }
    out[i] = 0;
    return (*v == '"') ? 0 : -1;
}

static int json_get_int(const char* j, const char* key, int* out)
{
    const char* v;
    if (json_find_key(j, key, &v) != 0 || !out) return -1;
    if (*v == '"') {
        char tmp[64];
        if (json_get_str(j, key, tmp, sizeof(tmp)) != 0) return -1;
        *out = atoi(tmp);
        return 0;
    }
    if (*v == 't' || *v == 'T') { *out = 1; return 0; }
    if (*v == 'f' || *v == 'F') { *out = 0; return 0; }
    *out = atoi(v);
    return 0;
}

static int json_get_u64(const char* j, const char* key, uint64_t* out)
{
    const char* v;
    char tmp[64];
    if (!out) return -1;
    if (json_find_key(j, key, &v) != 0) return -1;
    if (*v == '"') {
        if (json_get_str(j, key, tmp, sizeof(tmp)) != 0) return -1;
        v = tmp;
    }
    if (v[0] == '0' && (v[1] == 'x' || v[1] == 'X'))
        *out = _strtoui64(v + 2, NULL, 16);
    else
        *out = _strtoui64(v, NULL, 0);
    return 0;
}

static int json_get_bool(const char* j, const char* key, int defv)
{
    int v = defv;
    if (json_get_int(j, key, &v) != 0) return defv;
    return v ? 1 : 0;
}

static int ai_ok(char* out, int cap, const char* body)
{
    if (!out || cap < AI_OUT_MIN) return OS_ERR_INVALID_ARG;
    if (body && body[0])
        _snprintf(out, cap, "{\"ok\":true,%s}", body);
    else
        _snprintf(out, cap, "{\"ok\":true}");
    return OS_ERR_OK;
}

static int ai_fail(char* out, int cap, int code, const char* msg)
{
    if (out && cap > 0)
        _snprintf(out, cap, "{\"ok\":false,\"error\":%d,\"message\":\"%s\"}",
                  code, msg ? msg : "fail");
    return code != 0 ? code : OS_ERR_FAIL;
}

static void hex_encode(const uint8_t* data, int n, char* out, int cap)
{
    static const char* H = "0123456789abcdef";
    int i, o = 0;
    for (i = 0; i < n && o + 2 < cap; i++) {
        out[o++] = H[(data[i] >> 4) & 0xF];
        out[o++] = H[data[i] & 0xF];
    }
    if (o < cap) out[o] = 0;
}

static int hex_decode(const char* hex, uint8_t* out, int cap)
{
    int n = 0, hi = -1;
    const char* p = hex;
    if (!hex || !out) return -1;
    while (*p && n < cap) {
        int v;
        char c = *p++;
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') continue;
        if (c >= '0' && c <= '9') v = c - '0';
        else if (c >= 'a' && c <= 'f') v = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') v = c - 'A' + 10;
        else return -1;
        if (hi < 0) hi = v;
        else { out[n++] = (uint8_t)((hi << 4) | v); hi = -1; }
    }
    return (hi < 0) ? n : -1;
}

static void json_escape(const char* s, char* out, int cap)
{
    int i = 0;
    if (!out || cap < 1) return;
    if (!s) { out[0] = 0; return; }
    while (*s && i < cap - 2) {
        if (*s == '"' || *s == '\\') { out[i++] = '\\'; out[i++] = *s++; }
        else if ((unsigned char)*s < 0x20) { s++; }
        else out[i++] = *s++;
    }
    out[i] = 0;
}

/* ---------- 内置工具表（help / tools.list 自动输出） ---------- */

typedef struct {
    const char* name;
    const char* desc;
    const char* params; /* JSON schema 摘要字符串 */
} AI_Tool;

static const AI_Tool g_tools[] = {
    {"help", "完整帮助（Markdown）：协议、工具表、示例", "{}"},
    {"tools.list", "列出全部可用工具（含已加载模块动态扩展）", "{}"},
    {"status", "软件/连接/采集/ELF 状态摘要", "{}"},
    {"modules.list", "已加载 DLL 模块列表", "{}"},
    {"drivers.list", "探针驱动列表（仅 J-Link/ST-Link）", "{}"},
    {"drivers.select", "选择探针驱动", "{\"index\":0}"},
    {"probe.scan", "扫描仿真器", "{}"},
    {"xcp.scan", "扫描 XCP 传输端点（串口/CAN/TCP）", "{}"},
    {"xcp.connect", "连接 XCP（与探针/网络并行）", "{\"transport\":\"serial\",\"port\":\"COM3\",\"baud\":115200}"},
    {"xcp.disconnect", "断开 XCP", "{}"},
    {"xcp.status", "XCP 连接状态", "{}"},
    {"connect", "连接 MCU（attach=1 非侵入接入）", "{\"device\":\"Cortex-M4\",\"iface\":0,\"speed_khz\":4000,\"probe_index\":-1,\"attach\":0}"},
    {"disconnect", "断开 MCU", "{}"},
    {"halt", "Halt CPU（停住）", "{}"},
    {"go", "Go/Run CPU（继续运行）", "{}"},
    {"reset", "复位并停住（Reset & Halt）", "{}"},
    {"reset_run", "复位后运行（Reset & Go）", "{}"},
    {"step", "单步（需已 Halt）", "{}"},
    {"step_in", "步入", "{}"},
    {"step_over", "步过", "{}"},
    {"step_out", "步出（跑到 LR）", "{}"},
    {"bp.set", "设置硬件断点", "{\"address\":\"0x08001234\",\"note\":\"\"}"},
    {"bp.clear", "清除断点（all=1 全清）", "{\"address\":\"0x08001234\",\"all\":0}"},
    {"bp.list", "列出当前硬件断点", "{}"},
    {"bp.set_line", "按源码文件+行号设硬件断点", "{\"path\":\"D:\\\\proj\\\\main.c\",\"line\":100,\"note\":\"\"}"},
    {"bp.clear_line", "按源码文件+行号清断点", "{\"path\":\"D:\\\\proj\\\\main.c\",\"line\":100}"},
    {"src.status", "源码索引与 DWARF 行表摘要", "{}"},
    {"src.roots", "查询/添加源码根目录（add=路径则追加）", "{\"add\":\"D:\\\\proj\"}"},
    {"src.files", "列出/模糊搜索源码文件", "{\"needle\":\"main.c\",\"limit\":50,\"offset\":0}"},
    {"src.open", "打开源码窗口（可选跳行）", "{\"path\":\"D:\\\\proj\\\\main.c\",\"line\":100}"},
    {"src.line_to_addr", "源码行→指令地址", "{\"path\":\"main.c\",\"line\":100}"},
    {"src.addr_to_line", "指令地址→源码行", "{\"address\":\"0x0800114E\"}"},
    {"src.follow_pc", "按 PC 打开并高亮源码行（可省略 address=用当前 CPU PC）", "{\"address\":\"0x0800114E\"}"},
    {"cpu.state", "读 MCU 运行状态与 PC", "{}"},
    {"elf.load", "加载 ELF/out 并刷新变量表", "{\"path\":\"D:\\\\fw.elf\"}"},
    {"elf.reload", "重新加载当前 ELF", "{}"},
    {"vars.list", "列出叶变量（可选 needle 过滤，limit 限制条数）", "{\"needle\":\"\",\"limit\":50,\"offset\":0}"},
    {"vars.find", "模糊搜索叶变量", "{\"needle\":\"g_\",\"limit\":32}"},
    {"vars.read", "读叶变量最新样本（需已观测或刚读）", "{\"id\":0,\"name\":\"\"}"},
    {"vars.write", "写叶变量", "{\"id\":0,\"name\":\"\",\"value\":1.0}"},
    {"vars.watch", "设置观测开关", "{\"id\":0,\"name\":\"\",\"on\":1}"},
    {"mem.read", "按地址读内存（返回 hex）", "{\"address\":\"0x20000000\",\"size\":16}"},
    {"mem.write", "按地址写内存（data 为 hex）", "{\"address\":\"0x20000000\",\"data\":\"0102aabb\"}"},
    {"acq.start", "开始采集", "{}"},
    {"acq.stop", "停止采集", "{}"},
    {"acq.state", "采集状态", "{}"},
    {"flash.download", "下载固件到 Flash", "{\"path\":\"D:\\\\fw.elf\",\"address\":0,\"skip_erase\":0,\"verify\":1,\"reset_run\":1,\"device\":\"\"}"},
    {"flash.erase", "整片擦除", "{\"device\":\"\",\"reset_run\":1}"},
    {"recording.start", "开始录制（无需弹窗）", "{}"},
    {"recording.stop", "停止录制并可保存 CSV", "{\"path\":\"D:\\\\cap.csv\"}"},
    {"log.read", "读取运行时日志（since 游标增量拉取）", "{\"since\":0,\"limit\":100,\"min_level\":0}"},
    {"log.clear", "清空日志环形缓冲（不影响日志文件）", "{}"},
    {"log.subscribe", "订阅实时日志推送（WebSocket 通知 method=log.event）", "{\"min_level\":0}"},
    {"log.unsubscribe", "取消本连接的日志订阅", "{}"},
    {"alerts.read", "读取弹窗/严重反馈（MessageBox、崩溃等；since 游标）", "{\"since\":0,\"limit\":32}"},
    {"ui.silent", "查询/设置 AI 静默（不弹窗，确认框自动应答）", "{\"on\":1,\"yesno_default\":1}"},
    {"module.command", "透传 OS_CMD_* 到指定模块（高级）", "{\"module\":\"jlink\",\"cmd\":3}"},
};
static const int g_tool_n = (int)(sizeof(g_tools) / sizeof(g_tools[0]));

static int leaf_by_name_or_id(const char* params, int* id_out)
{
    int id = -1;
    char name[256];
    if (json_get_int(params, "id", &id) == 0 && id >= 0 && id < g_app.leaf_count) {
        *id_out = id;
        return 0;
    }
    if (json_get_str(params, "name", name, sizeof(name)) == 0 && name[0]) {
        int i;
        for (i = 0; i < g_app.leaf_count; i++) {
            if (strcmp(g_app.leaves[i].name, name) == 0) {
                *id_out = i;
                return 0;
            }
        }
        return OS_ERR_INVALID_ARG;
    }
    return OS_ERR_INVALID_ARG;
}

static int method_help(char* out, int cap)
{
    /* 动态帮助：协议说明 + 工具表；AI 客户端应优先调 tools.list */
    int n = 0, i;
    n += _snprintf(out + n, cap - n,
        "{\"ok\":true,\"version\":\"%s\",\"api\":%d,\"endpoint\":\"ws://127.0.0.1:18765\","
        "\"protocol\":\"JSON-RPC 2.0 over WebSocket (text frames)\","
        "\"mcp\":[\"tools/list\",\"tools/call\"],"
        "\"cli\":\"scripts/os_cli.py\",\"mcp_stdio\":\"scripts/openscope_mcp.py\","
        "\"note\":\"新模块加载后自动出现在 modules.list；实现 OS_CMD_AI_DESCRIBE 可扩展 tools\","
        "\"tools\":[", OS_VERSION_STR, OS_API_VERSION);
    for (i = 0; i < g_tool_n && n < cap - 128; i++) {
        char de[256], pe[256];
        json_escape(g_tools[i].desc, de, sizeof(de));
        json_escape(g_tools[i].params, pe, sizeof(pe));
        n += _snprintf(out + n, cap - n, "%s{\"name\":\"%s\",\"description\":\"%s\",\"params\":%s}",
                       i ? "," : "", g_tools[i].name, de, g_tools[i].params);
    }
    if (n < cap - 2) { out[n++] = ']'; out[n++] = '}'; out[n] = 0; }
    return OS_ERR_OK;
}

static int method_tools_list(char* out, int cap)
{
    int n = 0, i, m;
    char extra[8192];
    extra[0] = 0;
    /* 向各模块询问可选 AI 描述 */
    for (m = 0; m < g_app.mod_count; m++) {
        OS_Module* mod = &g_app.mods[m];
        char buf[4096];
        int rc;
        if (!mod->command) continue;
        buf[0] = 0;
        rc = mod->command(g_app.mod_ctx[m], OS_CMD_AI_DESCRIBE, NULL, buf);
        if (rc > 0 && buf[0]) {
            int el = (int)strlen(extra);
            if (el > 0 && el < (int)sizeof(extra) - 2) {
                extra[el++] = ','; extra[el] = 0;
            }
            _snprintf(extra + strlen(extra), sizeof(extra) - strlen(extra), "%s", buf);
        }
    }
    n += _snprintf(out + n, cap - n, "{\"ok\":true,\"tools\":[");
    for (i = 0; i < g_tool_n && n < cap - 160; i++) {
        char de[256];
        json_escape(g_tools[i].desc, de, sizeof(de));
        n += _snprintf(out + n, cap - n,
            "%s{\"name\":\"%s\",\"description\":\"%s\",\"inputSchema\":{\"type\":\"object\"}}",
            i ? "," : "", g_tools[i].name, de);
    }
    if (extra[0] && n < cap - 8) {
        n += _snprintf(out + n, cap - n, ",%s", extra);
    }
    /* 每个已加载模块再挂一条 module.<name>.info */
    for (m = 0; m < g_app.mod_count && n < cap - 200; m++) {
        char de[320], nm[80];
        json_escape(g_app.mods[m].name, nm, sizeof(nm));
        json_escape(g_app.mods[m].description, de, sizeof(de));
        n += _snprintf(out + n, cap - n,
            ",{\"name\":\"module.%s.info\",\"description\":\"模块 %s：%s\",\"inputSchema\":{\"type\":\"object\"}}",
            nm, nm, de);
    }
    if (n < cap - 2) { out[n++] = ']'; out[n++] = '}'; out[n] = 0; }
    return OS_ERR_OK;
}

static int method_status2(char* out, int cap)
{
    char elf8[MAX_PATH], esc[MAX_PATH], drv[64], desc[80];
    int di = os_modmgr_driver_index();
    int connected = (int)g_app.connected;
    int acq = (int)g_app.acq_state;
    elf8[0] = 0;
    if (g_app.elf_path[0]) os_wide_to_utf8_buf(g_app.elf_path, elf8, sizeof(elf8));
    json_escape(elf8, esc, sizeof(esc));
    _snprintf(drv, sizeof(drv), "%s",
              (g_app.driver && g_app.driver->name[0]) ? g_app.driver->name : "");
    json_escape(drv, desc, sizeof(desc));
    _snprintf(out, cap,
        "{\"ok\":true,\"version\":\"%s\",\"api\":%d,\"connected\":%d,\"acq_state\":%d,"
        "\"recording\":%d,\"leaf_count\":%d,\"watch_count\":%d,\"driver\":\"%s\","
        "\"driver_index\":%d,\"elf\":\"%s\",\"mod_count\":%d,\"poll_interval_ms\":%d,"
        "\"log_head_seq\":%llu,\"log_ring_cap\":%d,"
        "\"alert_head_seq\":%llu,\"alert_ring_cap\":%d,\"ui_silent\":%d}",
        OS_VERSION_STR, OS_API_VERSION, connected, acq, g_app.recording,
        g_app.leaf_count, g_app.watch_count, desc, di, esc, g_app.mod_count,
        g_app.poll_interval_ms,
        (unsigned long long)os_log_ring_head_seq(), OS_LOG_RING_CAP,
        (unsigned long long)os_alert_ring_head_seq(), OS_ALERT_RING_CAP,
        os_ui_silent());
    return OS_ERR_OK;
}

static int method_modules_list(char* out, int cap)
{
    int n = 0, i;
    n += _snprintf(out + n, cap - n, "{\"ok\":true,\"modules\":[");
    for (i = 0; i < g_app.mod_count && n < cap - 200; i++) {
        char nm[80], ver[40], de[280];
        json_escape(g_app.mods[i].name, nm, sizeof(nm));
        json_escape(g_app.mods[i].version, ver, sizeof(ver));
        json_escape(g_app.mods[i].description, de, sizeof(de));
        n += _snprintf(out + n, cap - n,
            "%s{\"name\":\"%s\",\"version\":\"%s\",\"capabilities\":%u,\"description\":\"%s\"}",
            i ? "," : "", nm, ver, (unsigned)g_app.mods[i].capabilities, de);
    }
    if (n < cap - 2) { out[n++] = ']'; out[n++] = '}'; out[n] = 0; }
    return OS_ERR_OK;
}

static int method_drivers_list(char* out, int cap)
{
    int n = 0, i, cur = os_modmgr_driver_index();
    n += _snprintf(out + n, cap - n, "{\"ok\":true,\"current\":%d,\"drivers\":[", cur);
    for (i = 0; i < g_app.driver_count && n < cap - 120; i++) {
        char nm[80];
        json_escape(g_app.drivers[i]->name, nm, sizeof(nm));
        n += _snprintf(out + n, cap - n, "%s{\"index\":%d,\"name\":\"%s\"}",
                       i ? "," : "", i, nm);
    }
    if (n < cap - 2) { out[n++] = ']'; out[n++] = '}'; out[n] = 0; }
    return OS_ERR_OK;
}

static int method_connect(const char* params, char* out, int cap)
{
    OS_ConnectCfg cfg;
    OS_DriverInfo info;
    int rc, c = 0;
    char device[128], serial[64];
    if (!g_app.driver || !g_app.driver->command)
        return ai_fail(out, cap, OS_ERR_NO_DEVICE, "no driver");
    memset(&cfg, 0, sizeof(cfg));
    cfg.probe_index = -1;
    cfg.iface = OS_IF_SWD;
    cfg.speed_khz = 4000;
    _snprintf(cfg.device, sizeof(cfg.device), "Cortex-M4");
    if (params) {
        int v;
        if (json_get_int(params, "iface", &v) == 0) cfg.iface = v;
        if (json_get_int(params, "speed_khz", &v) == 0) cfg.speed_khz = v;
        if (json_get_int(params, "probe_index", &v) == 0) cfg.probe_index = v;
        if (json_get_int(params, "connect_under_reset", &v) == 0) cfg.connect_under_reset = v;
        if (json_get_int(params, "attach", &v) == 0) cfg.attach = v;
        if (json_get_str(params, "device", device, sizeof(device)) == 0 && device[0])
            _snprintf(cfg.device, sizeof(cfg.device), "%s", device);
        if (json_get_str(params, "serial", serial, sizeof(serial)) == 0)
            _snprintf(cfg.serial, sizeof(cfg.serial), "%s", serial);
    }
    g_app.speed_khz = cfg.speed_khz;
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_CONNECT, &cfg, NULL);
    if (rc != OS_ERR_OK)
        return ai_fail(out, cap, rc, "connect failed");
    g_app.driver->command(g_app.driver_ctx, OS_CMD_IS_CONNECTED, NULL, &c);
    g_app.connected = c;
    memset(&info, 0, sizeof(info));
    g_app.driver->command(g_app.driver_ctx, OS_CMD_GET_INFO, NULL, &info);
    os_mainwin_update_buttons();
    /* Attach 且有监视变量时自动开采集 */
    if (cfg.attach && c && g_app.watch_count > 0 && g_app.acq_state != OS_ACQ_RUNNING)
        os_fw_acq_start();
    _snprintf(out, cap,
        "{\"ok\":true,\"connected\":%d,\"attach\":%d,\"emulator\":\"%s\",\"dll\":\"%s\"}",
        c, cfg.attach ? 1 : 0, info.emulator[0] ? info.emulator : "", info.dll_version);
    return OS_ERR_OK;
}

static int method_disconnect(char* out, int cap)
{
    if (g_app.acq_state == OS_ACQ_RUNNING) os_ds_stop();
    if (g_app.driver && g_app.driver->command)
        g_app.driver->command(g_app.driver_ctx, OS_CMD_DISCONNECT, NULL, NULL);
    g_app.connected = 0;
    os_mainwin_update_buttons();
    return ai_ok(out, cap, "\"connected\":0");
}

static int method_drv_simple(int cmd, char* out, int cap)
{
    int rc;
    if (!g_app.driver || !g_app.driver->command)
        return ai_fail(out, cap, OS_ERR_NO_DEVICE, "no driver");
    rc = g_app.driver->command(g_app.driver_ctx, cmd, NULL, NULL);
    if (rc != OS_ERR_OK) return ai_fail(out, cap, rc, "command failed");
    return ai_ok(out, cap, NULL);
}

static int method_scan(char* out, int cap)
{
    OS_DeviceInfo items[32];
    OS_ScanReq req;
    int i, n = 0, rc;
    if (!g_app.driver || !g_app.driver->command)
        return ai_fail(out, cap, OS_ERR_NO_DEVICE, "no driver");
    memset(&req, 0, sizeof(req));
    req.items = items;
    req.capacity = 32;
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_SCAN, &req, NULL);
    if (rc != OS_ERR_OK && req.count <= 0)
        return ai_fail(out, cap, rc, "scan failed");
    n += _snprintf(out + n, cap - n, "{\"ok\":true,\"count\":%d,\"devices\":[", req.count);
    for (i = 0; i < req.count && n < cap - 160; i++) {
        char sn[80], nm[160];
        json_escape(items[i].serial, sn, sizeof(sn));
        json_escape(items[i].name, nm, sizeof(nm));
        n += _snprintf(out + n, cap - n,
            "%s{\"index\":%d,\"serial\":\"%s\",\"name\":\"%s\"}",
            i ? "," : "", items[i].index, sn, nm);
    }
    if (n < cap - 2) { out[n++] = ']'; out[n++] = '}'; out[n] = 0; }
    return OS_ERR_OK;
}

static int method_elf_load(const char* params, char* out, int cap)
{
    char path8[MAX_PATH], esc[MAX_PATH * 2];
    wchar_t pathw[MAX_PATH];
    int rc;
    if (json_get_str(params, "path", path8, sizeof(path8)) != 0 || !path8[0])
        return ai_fail(out, cap, OS_ERR_INVALID_ARG, "path required");
    os_utf8_to_wide_buf(path8, pathw, MAX_PATH);
    rc = os_mainwin_open_elf(pathw);
    if (rc != 0) return ai_fail(out, cap, OS_ERR_FAIL, "elf load failed");
    json_escape(path8, esc, sizeof(esc));
    _snprintf(out, cap, "{\"ok\":true,\"leaf_count\":%d,\"path\":\"%s\"}",
              g_app.leaf_count, esc);
    return OS_ERR_OK;
}

static int method_vars_list(const char* params, char* out, int cap)
{
    char needle[128] = "";
    int limit = 50, offset = 0, i, n = 0, emitted = 0, skipped = 0;
    if (params) {
        json_get_str(params, "needle", needle, sizeof(needle));
        json_get_int(params, "limit", &limit);
        json_get_int(params, "offset", &offset);
    }
    if (limit <= 0) limit = 50;
    if (limit > 500) limit = 500;
    if (offset < 0) offset = 0;
    n += _snprintf(out + n, cap - n, "{\"ok\":true,\"total\":%d,\"vars\":[", g_app.leaf_count);
    for (i = 0; i < g_app.leaf_count && emitted < limit && n < cap - 220; i++) {
        char nm[280];
        if (needle[0] && !strstr(g_app.leaves[i].name, needle)) continue;
        if (skipped < offset) { skipped++; continue; }
        json_escape(g_app.leaves[i].name, nm, sizeof(nm));
        n += _snprintf(out + n, cap - n,
            "%s{\"id\":%d,\"name\":\"%s\",\"addr\":\"0x%llX\",\"size\":%u,\"watched\":%d}",
            emitted ? "," : "", i, nm,
            (unsigned long long)g_app.leaves[i].address,
            (unsigned)g_app.leaves[i].size,
            g_app.leaves[i].watched ? 1 : 0);
        emitted++;
    }
    if (n < cap - 2) { out[n++] = ']'; out[n++] = '}'; out[n] = 0; }
    return OS_ERR_OK;
}

static int method_vars_find(const char* params, char* out, int cap)
{
    char needle[128] = "";
    int ids[64], nfind, limit = 32, i, n = 0;
    if (params) {
        json_get_str(params, "needle", needle, sizeof(needle));
        json_get_int(params, "limit", &limit);
    }
    if (limit <= 0 || limit > 64) limit = 32;
    nfind = os_fw_leaf_find(needle, ids, limit);
    n += _snprintf(out + n, cap - n, "{\"ok\":true,\"count\":%d,\"ids\":[", nfind);
    for (i = 0; i < nfind; i++)
        n += _snprintf(out + n, cap - n, "%s%d", i ? "," : "", ids[i]);
    if (n < cap - 2) { out[n++] = ']'; out[n++] = '}'; out[n] = 0; }
    return OS_ERR_OK;
}

static int method_vars_read(const char* params, char* out, int cap)
{
    int id;
    const OS_Sample* s;
    char nm[280], tx[96];
    if (leaf_by_name_or_id(params, &id) != 0)
        return ai_fail(out, cap, OS_ERR_INVALID_ARG, "id or name required");
    s = os_fw_leaf_sample(id);
    json_escape(g_app.leaves[id].name, nm, sizeof(nm));
    if (!s) {
        _snprintf(out, cap,
            "{\"ok\":true,\"id\":%d,\"name\":\"%s\",\"has_sample\":0,\"addr\":\"0x%llX\"}",
            id, nm, (unsigned long long)g_app.leaves[id].address);
        return OS_ERR_OK;
    }
    json_escape(s->text, tx, sizeof(tx));
    _snprintf(out, cap,
        "{\"ok\":true,\"id\":%d,\"name\":\"%s\",\"has_sample\":1,\"value\":%.12g,"
        "\"text\":\"%s\",\"ts_us\":%lld,\"addr\":\"0x%llX\"}",
        id, nm, s->value, tx, (long long)s->ts_us,
        (unsigned long long)s->address);
    return OS_ERR_OK;
}

static int method_vars_write(const char* params, char* out, int cap)
{
    int id;
    double value = 0;
    char err[128] = "", vs[64];
    const char* pv;
    if (leaf_by_name_or_id(params, &id) != 0)
        return ai_fail(out, cap, OS_ERR_INVALID_ARG, "id or name required");
    if (json_find_key(params, "value", &pv) == 0) {
        if (*pv == '"') {
            if (json_get_str(params, "value", vs, sizeof(vs)) == 0)
                value = atof(vs);
        } else value = atof(pv);
    } else
        return ai_fail(out, cap, OS_ERR_INVALID_ARG, "value required");
    if (os_fw_write_leaf(id, value, err, sizeof(err)) != 0)
        return ai_fail(out, cap, OS_ERR_FAIL, err[0] ? err : "write failed");
    return ai_ok(out, cap, NULL);
}

static int method_vars_watch(const char* params, char* out, int cap)
{
    int id, on = 1;
    if (leaf_by_name_or_id(params, &id) != 0)
        return ai_fail(out, cap, OS_ERR_INVALID_ARG, "id or name required");
    json_get_int(params, "on", &on);
    if (os_fw_set_watch(id, on ? 1 : 0) != 0)
        return ai_fail(out, cap, OS_ERR_FAIL, "watch failed");
    _snprintf(out, cap, "{\"ok\":true,\"id\":%d,\"watched\":%d,\"watch_count\":%d}",
              id, on ? 1 : 0, g_app.watch_count);
    return OS_ERR_OK;
}

static int method_mem_read(const char* params, char* out, int cap)
{
    uint64_t addr = 0;
    int size = 0, rc;
    OS_MemReq req;
    uint8_t* buf;
    char hex[4096];
    if (!g_app.driver || !g_app.driver->command)
        return ai_fail(out, cap, OS_ERR_NOT_CONNECTED, "no driver");
    if (json_get_u64(params, "address", &addr) != 0 ||
        json_get_int(params, "size", &size) != 0 || size <= 0 || size > 2048)
        return ai_fail(out, cap, OS_ERR_INVALID_ARG, "address/size invalid");
    buf = (uint8_t*)malloc((size_t)size);
    if (!buf) return ai_fail(out, cap, OS_ERR_FAIL, "oom");
    memset(&req, 0, sizeof(req));
    req.address = addr;
    req.size = (uint32_t)size;
    req.data = buf;
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_READ_MEM, &req, NULL);
    if (rc < 0) { free(buf); return ai_fail(out, cap, rc, "read failed"); }
    hex_encode(buf, size, hex, sizeof(hex));
    free(buf);
    _snprintf(out, cap,
        "{\"ok\":true,\"address\":\"0x%llX\",\"size\":%d,\"data\":\"%s\"}",
        (unsigned long long)addr, size, hex);
    return OS_ERR_OK;
}

static int method_mem_write(const char* params, char* out, int cap)
{
    uint64_t addr = 0;
    char hex[4096];
    uint8_t buf[2048];
    int n, rc;
    OS_MemReq req;
    if (!g_app.driver || !g_app.driver->command)
        return ai_fail(out, cap, OS_ERR_NOT_CONNECTED, "no driver");
    if (json_get_u64(params, "address", &addr) != 0 ||
        json_get_str(params, "data", hex, sizeof(hex)) != 0)
        return ai_fail(out, cap, OS_ERR_INVALID_ARG, "address/data required");
    n = hex_decode(hex, buf, sizeof(buf));
    if (n <= 0) return ai_fail(out, cap, OS_ERR_INVALID_ARG, "bad hex");
    memset(&req, 0, sizeof(req));
    req.address = addr;
    req.size = (uint32_t)n;
    req.data = buf;
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_WRITE_MEM, &req, NULL);
    if (rc != OS_ERR_OK) return ai_fail(out, cap, rc, "write failed");
    _snprintf(out, cap, "{\"ok\":true,\"address\":\"0x%llX\",\"size\":%d}",
              (unsigned long long)addr, n);
    return OS_ERR_OK;
}

static int method_flash(const char* params, int erase, char* out, int cap)
{
    OS_FlashReq req;
    char path8[MAX_PATH], device[128];
    int rc, was_running;
    if (!g_app.driver || !g_app.driver->command)
        return ai_fail(out, cap, OS_ERR_NO_DEVICE, "no driver");
    if (!g_app.connected)
        return ai_fail(out, cap, OS_ERR_NOT_CONNECTED, "not connected");
    memset(&req, 0, sizeof(req));
    req.verify = 1;
    req.reset_run = 1;
    if (params) {
        if (json_get_str(params, "path", path8, sizeof(path8)) == 0 && path8[0])
            os_utf8_to_wide_buf(path8, req.path, MAX_PATH);
        {
            uint64_t a = 0;
            if (json_get_u64(params, "address", &a) == 0) req.address = (uint32_t)a;
        }
        req.skip_erase = json_get_bool(params, "skip_erase", 0);
        req.verify = json_get_bool(params, "verify", 1);
        req.reset_run = json_get_bool(params, "reset_run", 1);
        if (json_get_str(params, "device", device, sizeof(device)) == 0)
            _snprintf(req.device, sizeof(req.device), "%s", device);
    }
    if (!erase && !req.path[0])
        return ai_fail(out, cap, OS_ERR_INVALID_ARG, "path required");
    was_running = (g_app.acq_state == OS_ACQ_RUNNING);
    if (was_running) os_ds_stop();
    rc = g_app.driver->command(g_app.driver_ctx,
        erase ? OS_CMD_FLASH_ERASE : OS_CMD_FLASH_DOWNLOAD, &req, NULL);
    if (was_running) os_ds_start();
    if (rc != OS_ERR_OK) return ai_fail(out, cap, rc, erase ? "erase failed" : "download failed");
    return ai_ok(out, cap, erase ? "\"erased\":1" : "\"downloaded\":1");
}

static int method_recording_start(char* out, int cap)
{
    if (g_app.recording) return ai_ok(out, cap, "\"recording\":1,\"already\":1");
    g_app.recording = 1;
    if (g_app.acq_state == OS_ACQ_RUNNING) {
        os_ds_stop();
        if (os_ds_start() != 0) {
            g_app.recording = 0;
            return ai_fail(out, cap, OS_ERR_FAIL, "restart acq for recording failed");
        }
    } else if (os_spool_begin() != 0) {
        g_app.recording = 0;
        return ai_fail(out, cap, OS_ERR_FAIL, "spool begin failed");
    }
    os_mainwin_update_buttons();
    return ai_ok(out, cap, "\"recording\":1");
}

static int method_recording_stop(const char* params, char* out, int cap)
{
    char path8[MAX_PATH] = "";
    wchar_t pathw[MAX_PATH];
    int was_running = (g_app.acq_state == OS_ACQ_RUNNING);
    if (params) json_get_str(params, "path", path8, sizeof(path8));
    if (!g_app.recording && !os_spool_has_data())
        return ai_ok(out, cap, "\"recording\":0,\"saved\":0");
    if (was_running) os_ds_stop();
    else os_spool_end();
    g_app.recording = 0;
    if (path8[0] && os_spool_has_data()) {
        os_utf8_to_wide_buf(path8, pathw, MAX_PATH);
        if (os_spool_save_as(pathw) != 0) {
            if (was_running) os_ds_start();
            return ai_fail(out, cap, OS_ERR_FAIL, "save failed");
        }
        os_spool_clear();
        if (was_running) os_ds_start();
        os_mainwin_update_buttons();
        {
            char esc[MAX_PATH * 2];
            json_escape(path8, esc, sizeof(esc));
            _snprintf(out, cap, "{\"ok\":true,\"recording\":0,\"saved\":1,\"path\":\"%s\"}", esc);
        }
        return OS_ERR_OK;
    }
    if (was_running) os_ds_start();
    os_mainwin_update_buttons();
    return ai_ok(out, cap, "\"recording\":0,\"saved\":0");
}

static const char* log_level_name(int level)
{
    switch (level) {
    case OS_LOG_WARN:  return "WARN";
    case OS_LOG_ERROR: return "ERROR";
    case OS_LOG_DEBUG: return "DEBUG";
    default:           return "INFO";
    }
}

static int method_log_read(const char* params, char* out, int cap)
{
    OS_LogEntry entries[128];
    uint64_t since = 0, next = 0;
    int limit = 100, min_level = OS_LOG_DEBUG, n, i, o = 0;
    char ts[40], esc[640];
    if (params) {
        const char* pv;
        int v;
        if (json_find_key(params, "since", &pv) == 0)
            since = _strtoui64(*pv == '"' ? pv + 1 : pv, NULL, 0);
        if (json_get_int(params, "limit", &v) == 0) limit = v;
        if (json_get_int(params, "min_level", &v) == 0) min_level = v;
    }
    if (limit <= 0) limit = 100;
    if (limit > 128) limit = 128;
    n = os_log_ring_copy(since, min_level, entries, limit, &next);
    o += _snprintf(out + o, cap - o,
        "{\"ok\":true,\"count\":%d,\"since\":%llu,\"next_since\":%llu,\"lines\":[",
        n, (unsigned long long)since, (unsigned long long)next);
    for (i = 0; i < n && o < cap - 200; i++) {
        json_escape(entries[i].text, esc, sizeof(esc));
        os_time_iso(entries[i].ts_us, ts, sizeof(ts));
        o += _snprintf(out + o, cap - o,
            "%s{\"seq\":%llu,\"level\":%d,\"level_name\":\"%s\",\"ts\":\"%s\",\"text\":\"%s\"}",
            i ? "," : "",
            (unsigned long long)entries[i].seq,
            entries[i].level, log_level_name(entries[i].level), ts, esc);
    }
    if (o < cap - 2) { out[o++] = ']'; out[o++] = '}'; out[o] = 0; }
    return OS_ERR_OK;
}

static int method_log_clear(char* out, int cap)
{
    os_log_ring_clear();
    _snprintf(out, cap, "{\"ok\":true,\"head_seq\":%llu}",
              (unsigned long long)os_log_ring_head_seq());
    return OS_ERR_OK;
}

static int method_alerts_read(const char* params, char* out, int cap)
{
    OS_Alert entries[64];
    uint64_t since = 0, next = 0;
    int limit = 32, n, i, o = 0;
    char ts[40], esc_t[160], esc_x[640];
    if (params) {
        const char* pv;
        int v;
        if (json_find_key(params, "since", &pv) == 0)
            since = _strtoui64(*pv == '"' ? pv + 1 : pv, NULL, 0);
        if (json_get_int(params, "limit", &v) == 0) limit = v;
    }
    if (limit <= 0) limit = 32;
    if (limit > 64) limit = 64;
    n = os_alert_ring_copy(since, entries, limit, &next);
    o += _snprintf(out + o, cap - o,
        "{\"ok\":true,\"count\":%d,\"since\":%llu,\"next_since\":%llu,\"ui_silent\":%d,\"alerts\":[",
        n, (unsigned long long)since, (unsigned long long)next, os_ui_silent());
    for (i = 0; i < n && o < cap - 280; i++) {
        json_escape(entries[i].title, esc_t, sizeof(esc_t));
        json_escape(entries[i].text, esc_x, sizeof(esc_x));
        os_time_iso(entries[i].ts_us, ts, sizeof(ts));
        o += _snprintf(out + o, cap - o,
            "%s{\"seq\":%llu,\"level\":%d,\"level_name\":\"%s\",\"ts\":\"%s\","
            "\"type\":%d,\"result\":%d,\"silenced\":%d,\"title\":\"%s\",\"text\":\"%s\"}",
            i ? "," : "",
            (unsigned long long)entries[i].seq,
            entries[i].level, log_level_name(entries[i].level), ts,
            entries[i].type, entries[i].result, entries[i].silenced,
            esc_t, esc_x);
    }
    if (o < cap - 2) { out[o++] = ']'; out[o++] = '}'; out[o] = 0; }
    return OS_ERR_OK;
}

static int method_ui_silent(const char* params, char* out, int cap)
{
    int on = -1, yn = -1;
    if (params) {
        json_get_int(params, "on", &on);
        json_get_int(params, "yesno_default", &yn);
    }
    if (yn >= 0) os_ui_set_yesno_default(yn ? 1 : 0);
    if (on >= 0) os_ui_set_silent(on ? 1 : 0);
    _snprintf(out, cap,
        "{\"ok\":true,\"ui_silent\":%d,\"yesno_default\":%d,\"alert_head_seq\":%llu}",
        os_ui_silent(), os_ui_yesno_default(),
        (unsigned long long)os_alert_ring_head_seq());
    return OS_ERR_OK;
}

static int method_module_command(const char* params, char* out, int cap)
{
    char name[64];
    int cmd = 0, i;
    if (json_get_str(params, "module", name, sizeof(name)) != 0 ||
        json_get_int(params, "cmd", &cmd) != 0)
        return ai_fail(out, cap, OS_ERR_INVALID_ARG, "module/cmd required");
    for (i = 0; i < g_app.mod_count; i++) {
        if (strcmp(g_app.mods[i].name, name) == 0) {
            int rc;
            if (!g_app.mods[i].command)
                return ai_fail(out, cap, OS_ERR_FAIL, "no command");
            /* 无 in/out 的简单命令；复杂命令请用专用方法 */
            rc = g_app.mods[i].command(g_app.mod_ctx[i], cmd, NULL, NULL);
            _snprintf(out, cap, "{\"ok\":%s,\"rc\":%d}",
                      rc == OS_ERR_OK ? "true" : "false", rc);
            return rc == OS_ERR_OK ? OS_ERR_OK : rc;
        }
    }
    return ai_fail(out, cap, OS_ERR_INVALID_ARG, "module not found");
}

static int method_module_info(const char* method, char* out, int cap)
{
    /* method = module.<name>.info */
    const char* p;
    char name[64];
    int i, nlen;
    if (strncmp(method, "module.", 7) != 0) return -1;
    p = method + 7;
    nlen = 0;
    while (p[nlen] && p[nlen] != '.' && nlen < 63) {
        name[nlen] = p[nlen];
        nlen++;
    }
    name[nlen] = 0;
    if (strcmp(p + nlen, ".info") != 0) return -1;
    for (i = 0; i < g_app.mod_count; i++) {
        if (strcmp(g_app.mods[i].name, name) == 0) {
            char de[280], ver[40];
            json_escape(g_app.mods[i].description, de, sizeof(de));
            json_escape(g_app.mods[i].version, ver, sizeof(ver));
            _snprintf(out, cap,
                "{\"ok\":true,\"name\":\"%s\",\"version\":\"%s\",\"capabilities\":%u,"
                "\"description\":\"%s\"}",
                name, ver, (unsigned)g_app.mods[i].capabilities, de);
            return OS_ERR_OK;
        }
    }
    return ai_fail(out, cap, OS_ERR_INVALID_ARG, "module not found");
}

int os_ai_call(const char* method, const char* params_json,
               char* result_json, int result_cap)
{
    const char* params = params_json ? params_json : "{}";
    int idx;

    if (!method || !method[0] || !result_json || result_cap < AI_OUT_MIN)
        return OS_ERR_INVALID_ARG;
    result_json[0] = 0;

    if (strcmp(method, "help") == 0) return method_help(result_json, result_cap);
    if (strcmp(method, "tools.list") == 0 || strcmp(method, "tools/list") == 0)
        return method_tools_list(result_json, result_cap);
    if (strcmp(method, "status") == 0) return method_status2(result_json, result_cap);
    if (strcmp(method, "modules.list") == 0) return method_modules_list(result_json, result_cap);
    if (strcmp(method, "drivers.list") == 0) return method_drivers_list(result_json, result_cap);
    if (strcmp(method, "drivers.select") == 0) {
        if (json_get_int(params, "index", &idx) != 0)
            return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "index required");
        if (os_modmgr_select_driver(idx) != OS_ERR_OK)
            return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "bad index");
        os_mainwin_update_buttons();
        return method_drivers_list(result_json, result_cap);
    }
    if (strcmp(method, "xcp.scan") == 0) {
        OS_ScanReq req;
        OS_DeviceInfo items[32];
        int i, n = 0;
        if (!g_app.xcpmod || !g_app.xcpmod->command)
            return ai_fail(result_json, result_cap, OS_ERR_NO_DEVICE, "no xcp");
        memset(&req, 0, sizeof(req));
        req.items = items; req.capacity = 32;
        g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_SCAN, &req, NULL);
        n += _snprintf(result_json + n, result_cap - n, "{\"ok\":true,\"count\":%d,\"endpoints\":[", req.count);
        for (i = 0; i < req.count && n < result_cap - 160; i++) {
            char nm[140], tr[80];
            json_escape(items[i].name, nm, sizeof(nm));
            json_escape(items[i].serial, tr, sizeof(tr));
            n += _snprintf(result_json + n, result_cap - n,
                "%s{\"index\":%d,\"name\":\"%s\",\"transport\":\"%s\"}",
                i ? "," : "", i, nm, tr);
        }
        if (n < result_cap - 2) { result_json[n++] = ']'; result_json[n++] = '}'; result_json[n] = 0; }
        return OS_ERR_OK;
    }
    if (strcmp(method, "xcp.connect") == 0) {
        OS_ConnectCfg cfg;
        char transport[32] = "serial", port[64] = "", baud_s[32];
        int baud = 115200, rc, c = 0;
        if (!g_app.xcpmod || !g_app.xcpmod->command)
            return ai_fail(result_json, result_cap, OS_ERR_NO_DEVICE, "no xcp");
        memset(&cfg, 0, sizeof(cfg));
        json_get_str(params, "transport", transport, sizeof(transport));
        json_get_str(params, "port", port, sizeof(port));
        if (json_get_int(params, "baud", &baud) != 0) baud = 115200;
        if (!port[0]) json_get_str(params, "serial", port, sizeof(port));
        _snprintf(cfg.device, sizeof(cfg.device), "%s", transport[0] ? transport : "serial");
        _snprintf(cfg.serial, sizeof(cfg.serial), "%s", port);
        cfg.speed_khz = baud;
        cfg.probe_index = 0;
        rc = g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_CONNECT, &cfg, NULL);
        if (rc != OS_ERR_OK)
            return ai_fail(result_json, result_cap, rc, "xcp.connect failed");
        g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_IS_CONNECTED, NULL, &c);
        g_app.xcp_connected = c;
        _snprintf(baud_s, sizeof(baud_s), "%d", baud);
        {
            char te[40], pe[140];
            json_escape(cfg.device, te, sizeof(te));
            json_escape(cfg.serial, pe, sizeof(pe));
            _snprintf(result_json, result_cap,
                "{\"ok\":true,\"connected\":%d,\"transport\":\"%s\",\"port\":\"%s\",\"baud\":%d}",
                c, te, pe, baud);
        }
        return OS_ERR_OK;
    }
    if (strcmp(method, "xcp.disconnect") == 0) {
        if (!g_app.xcpmod || !g_app.xcpmod->command)
            return ai_fail(result_json, result_cap, OS_ERR_NO_DEVICE, "no xcp");
        g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_DISCONNECT, NULL, NULL);
        g_app.xcp_connected = 0;
        return ai_ok(result_json, result_cap, "\"connected\":0");
    }
    if (strcmp(method, "xcp.status") == 0) {
        int c = 0;
        OS_DriverInfo info;
        memset(&info, 0, sizeof(info));
        if (g_app.xcpmod && g_app.xcpmod->command) {
            g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_IS_CONNECTED, NULL, &c);
            g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_GET_INFO, NULL, &info);
            g_app.xcp_connected = c;
        }
        {
            char esc[160];
            json_escape(info.emulator[0] ? info.emulator : "", esc, sizeof(esc));
            _snprintf(result_json, result_cap,
                "{\"ok\":true,\"connected\":%d,\"loaded\":%d,\"info\":\"%s\"}",
                c, g_app.xcpmod ? 1 : 0, esc);
        }
        return OS_ERR_OK;
    }
    if (strcmp(method, "probe.scan") == 0) return method_scan(result_json, result_cap);
    if (strcmp(method, "connect") == 0) return method_connect(params, result_json, result_cap);
    if (strcmp(method, "disconnect") == 0) return method_disconnect(result_json, result_cap);
    if (strcmp(method, "halt") == 0) return method_drv_simple(OS_CMD_HALT, result_json, result_cap);
    if (strcmp(method, "go") == 0) return method_drv_simple(OS_CMD_GO, result_json, result_cap);
    if (strcmp(method, "reset") == 0) return method_drv_simple(OS_CMD_RESET, result_json, result_cap);
    if (strcmp(method, "reset_run") == 0)
        return method_drv_simple(OS_CMD_RESET_RUN, result_json, result_cap);
    if (strcmp(method, "step") == 0)
        return method_drv_simple(OS_CMD_STEP, result_json, result_cap);
    if (strcmp(method, "step_in") == 0)
        return method_drv_simple(OS_CMD_STEP_IN, result_json, result_cap);
    if (strcmp(method, "step_over") == 0)
        return method_drv_simple(OS_CMD_STEP_OVER, result_json, result_cap);
    if (strcmp(method, "step_out") == 0)
        return method_drv_simple(OS_CMD_STEP_OUT, result_json, result_cap);
    if (strcmp(method, "bp.set") == 0) {
        OS_BpReq req;
        uint64_t addr64 = 0;
        char note[64];
        int rc;
        if (!g_app.driver || !g_app.connected)
            return ai_fail(result_json, result_cap, OS_ERR_NOT_CONNECTED, "not connected");
        if (json_get_u64(params, "address", &addr64) != 0 || addr64 == 0)
            return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "address required");
        memset(&req, 0, sizeof(req));
        req.address = (uint32_t)addr64;
        if (json_get_str(params, "note", note, sizeof(note)) == 0)
            _snprintf(req.note, sizeof(req.note), "%s", note);
        rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_SET, &req, NULL);
        if (rc != OS_ERR_OK)
            return ai_fail(result_json, result_cap, rc, "bp.set failed");
        os_src_refresh_bps();
        _snprintf(result_json, result_cap,
            "{\"ok\":true,\"address\":%u,\"address_hex\":\"0x%08X\"}",
            (unsigned)(req.address & ~1u), (unsigned)(req.address & ~1u));
        return OS_ERR_OK;
    }
    if (strcmp(method, "bp.clear") == 0) {
        OS_BpReq req;
        uint64_t addr64 = 0;
        int all = 0, rc;
        if (!g_app.driver || !g_app.connected)
            return ai_fail(result_json, result_cap, OS_ERR_NOT_CONNECTED, "not connected");
        all = json_get_bool(params, "all", 0);
        memset(&req, 0, sizeof(req));
        req.clear_all = all ? 1 : 0;
        if (!all) {
            if (json_get_u64(params, "address", &addr64) != 0 || addr64 == 0)
                return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "address or all required");
            req.address = (uint32_t)addr64;
        }
        rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_CLEAR, &req, NULL);
        if (rc != OS_ERR_OK)
            return ai_fail(result_json, result_cap, rc, "bp.clear failed");
        os_src_refresh_bps();
        return ai_ok(result_json, result_cap, NULL);
    }
    if (strcmp(method, "bp.list") == 0) {
        OS_BpInfo items[OS_BP_MAX];
        OS_BpList list;
        int i, n = 0, rc;
        if (!g_app.driver || !g_app.connected)
            return ai_fail(result_json, result_cap, OS_ERR_NOT_CONNECTED, "not connected");
        memset(items, 0, sizeof(items));
        memset(&list, 0, sizeof(list));
        list.items = items;
        list.capacity = OS_BP_MAX;
        rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_LIST, NULL, &list);
        if (rc != OS_ERR_OK)
            return ai_fail(result_json, result_cap, rc, "bp.list failed");
        n += _snprintf(result_json + n, result_cap - n,
            "{\"ok\":true,\"count\":%d,\"breakpoints\":[", list.count);
        for (i = 0; i < list.count && n < result_cap - 80; i++) {
            n += _snprintf(result_json + n, result_cap - n,
                "%s{\"id\":%d,\"address\":%u,\"address_hex\":\"0x%08X\",\"enabled\":%d}",
                i ? "," : "", items[i].id, (unsigned)items[i].address,
                (unsigned)items[i].address, items[i].enabled);
        }
        if (n < result_cap - 2) {
            result_json[n++] = ']';
            result_json[n++] = '}';
            result_json[n] = 0;
        }
        return OS_ERR_OK;
    }
    if (strcmp(method, "bp.set_line") == 0 || strcmp(method, "bp.clear_line") == 0) {
        char path[MAX_PATH];
        int line = 0, rc, is_set;
        uint32_t addr = 0;
        OS_BpReq req;
        is_set = (strcmp(method, "bp.set_line") == 0);
        if (!g_app.driver || !g_app.connected)
            return ai_fail(result_json, result_cap, OS_ERR_NOT_CONNECTED, "not connected");
        if (!g_app.elf)
            return ai_fail(result_json, result_cap, OS_ERR_FAIL, "no elf");
        if (json_get_str(params, "path", path, sizeof(path)) != 0 || !path[0])
            return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "path required");
        if (json_get_int(params, "line", &line) != 0 || line <= 0)
            return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "line required");
        if (!os_elf_addr_of_line(g_app.elf, path, line, &addr) || !addr)
            return ai_fail(result_json, result_cap, OS_ERR_FAIL, "line map failed");
        memset(&req, 0, sizeof(req));
        req.address = addr;
        if (is_set) {
            char note[64];
            if (json_get_str(params, "note", note, sizeof(note)) == 0)
                _snprintf(req.note, sizeof(req.note), "%s", note);
            else
                _snprintf(req.note, sizeof(req.note), "%s:%d", path, line);
            rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_SET, &req, NULL);
            if (rc != OS_ERR_OK)
                return ai_fail(result_json, result_cap, rc, "bp.set_line failed");
            os_src_refresh_bps();
            {
                char esc[MAX_PATH * 2];
                json_escape(path, esc, sizeof(esc));
                _snprintf(result_json, result_cap,
                    "{\"ok\":true,\"path\":\"%s\",\"line\":%d,\"address\":%u,\"address_hex\":\"0x%08X\"}",
                    esc, line, (unsigned)(addr & ~1u), (unsigned)(addr & ~1u));
            }
            return OS_ERR_OK;
        }
        rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_CLEAR, &req, NULL);
        if (rc != OS_ERR_OK)
            return ai_fail(result_json, result_cap, rc, "bp.clear_line failed");
        os_src_refresh_bps();
        {
            char esc[MAX_PATH * 2];
            json_escape(path, esc, sizeof(esc));
            _snprintf(result_json, result_cap,
                "{\"ok\":true,\"path\":\"%s\",\"line\":%d,\"address\":%u,\"address_hex\":\"0x%08X\"}",
                esc, line, (unsigned)(addr & ~1u), (unsigned)(addr & ~1u));
        }
        return OS_ERR_OK;
    }
    if (strcmp(method, "src.status") == 0) {
        int rows = g_app.elf ? os_elf_line_count(g_app.elf) : 0;
        int files = g_app.elf ? os_elf_line_file_count(g_app.elf) : 0;
        _snprintf(result_json, result_cap,
            "{\"ok\":true,\"src_files\":%d,\"src_roots\":%d,\"line_rows\":%d,\"line_files\":%d,"
            "\"elf_loaded\":%d}",
            os_mainwin_src_file_count(), os_mainwin_src_root_count(),
            rows, files, g_app.elf ? 1 : 0);
        return OS_ERR_OK;
    }
    if (strcmp(method, "src.roots") == 0) {
        char add[MAX_PATH];
        int i, n = 0;
        if (params && json_get_str(params, "add", add, sizeof(add)) == 0 && add[0]) {
            if (os_mainwin_src_add_root(add) != 0)
                return ai_fail(result_json, result_cap, OS_ERR_FAIL, "add root failed");
        }
        n += _snprintf(result_json + n, result_cap - n,
            "{\"ok\":true,\"count\":%d,\"roots\":[", os_mainwin_src_root_count());
        for (i = 0; i < os_mainwin_src_root_count() && n < result_cap - 300; i++) {
            char root[MAX_PATH], esc[MAX_PATH * 2];
            if (os_mainwin_src_get_root(i, root, sizeof(root)) != 0) continue;
            json_escape(root, esc, sizeof(esc));
            n += _snprintf(result_json + n, result_cap - n, "%s\"%s\"", i ? "," : "", esc);
        }
        if (n < result_cap - 2) {
            result_json[n++] = ']';
            result_json[n++] = '}';
            result_json[n] = 0;
        }
        return OS_ERR_OK;
    }
    if (strcmp(method, "src.files") == 0) {
        char needle[260], body[8192];
        int limit = 50, offset = 0, ids[128], nmatch, i, shown = 0, bn = 0;
        needle[0] = 0;
        body[0] = 0;
        if (params) {
            json_get_str(params, "needle", needle, sizeof(needle));
            json_get_int(params, "limit", &limit);
            json_get_int(params, "offset", &offset);
        }
        if (limit <= 0) limit = 50;
        if (limit > 128) limit = 128;
        if (offset < 0) offset = 0;
        if (os_mainwin_src_file_count() <= 0)
            os_mainwin_src_refresh_index();
        if (needle[0])
            nmatch = os_mainwin_src_find(needle, ids, 128);
        else {
            nmatch = os_mainwin_src_file_count();
            if (nmatch > 128) nmatch = 128;
            for (i = 0; i < nmatch; i++) ids[i] = i;
        }
        for (i = offset; i < nmatch && shown < limit && bn < (int)sizeof(body) - 400; i++) {
            char path[MAX_PATH], esc[MAX_PATH * 2];
            if (os_mainwin_src_get_file(ids[i], path, sizeof(path)) != 0) continue;
            json_escape(path, esc, sizeof(esc));
            bn += _snprintf(body + bn, (int)sizeof(body) - bn,
                "%s{\"index\":%d,\"path\":\"%s\"}", shown ? "," : "", ids[i], esc);
            shown++;
        }
        _snprintf(result_json, result_cap,
            "{\"ok\":true,\"total\":%d,\"count\":%d,\"files\":[%s]}",
            nmatch, shown, body);
        return OS_ERR_OK;
    }
    if (strcmp(method, "src.open") == 0) {
        char path[MAX_PATH], esc[MAX_PATH * 2];
        int line = 0;
        if (json_get_str(params, "path", path, sizeof(path)) != 0 || !path[0])
            return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "path required");
        json_get_int(params, "line", &line);
        if (os_mainwin_src_open_safe(path, line) != 0)
            return ai_fail(result_json, result_cap, OS_ERR_FAIL, "open failed");
        json_escape(path, esc, sizeof(esc));
        _snprintf(result_json, result_cap, "{\"ok\":true,\"path\":\"%s\",\"line\":%d}", esc, line);
        return OS_ERR_OK;
    }
    if (strcmp(method, "src.line_to_addr") == 0) {
        char path[MAX_PATH], esc[MAX_PATH * 2];
        int line = 0;
        uint32_t addr = 0;
        if (!g_app.elf)
            return ai_fail(result_json, result_cap, OS_ERR_FAIL, "no elf");
        if (json_get_str(params, "path", path, sizeof(path)) != 0 || !path[0])
            return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "path required");
        if (json_get_int(params, "line", &line) != 0 || line <= 0)
            return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "line required");
        if (!os_elf_addr_of_line(g_app.elf, path, line, &addr) || !addr)
            return ai_fail(result_json, result_cap, OS_ERR_FAIL, "line map failed");
        json_escape(path, esc, sizeof(esc));
        _snprintf(result_json, result_cap,
            "{\"ok\":true,\"path\":\"%s\",\"line\":%d,\"address\":%u,\"address_hex\":\"0x%08X\"}",
            esc, line, (unsigned)(addr & ~1u), (unsigned)(addr & ~1u));
        return OS_ERR_OK;
    }
    if (strcmp(method, "src.addr_to_line") == 0) {
        uint64_t addr64 = 0;
        char file[512], esc[1024];
        int line = 0;
        if (!g_app.elf)
            return ai_fail(result_json, result_cap, OS_ERR_FAIL, "no elf");
        if (json_get_u64(params, "address", &addr64) != 0)
            return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "address required");
        if (!os_elf_line_of_addr(g_app.elf, (uint32_t)addr64, file, sizeof(file), &line))
            return ai_fail(result_json, result_cap, OS_ERR_FAIL, "addr map failed");
        json_escape(file, esc, sizeof(esc));
        _snprintf(result_json, result_cap,
            "{\"ok\":true,\"address\":%u,\"address_hex\":\"0x%08X\",\"path\":\"%s\",\"line\":%d}",
            (unsigned)(addr64 & ~1u), (unsigned)(addr64 & ~1u), esc, line);
        return OS_ERR_OK;
    }
    if (strcmp(method, "src.follow_pc") == 0) {
        uint64_t addr64 = 0;
        char path[MAX_PATH], esc[MAX_PATH * 2];
        int line = 0;
        if (params && json_get_u64(params, "address", &addr64) == 0 && addr64 != 0) {
            /* use provided */
        } else {
            OS_CpuState st;
            int rc;
            if (!g_app.driver || !g_app.connected)
                return ai_fail(result_json, result_cap, OS_ERR_NOT_CONNECTED, "not connected");
            memset(&st, 0, sizeof(st));
            st.halted = -1;
            rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_GET_CPU_STATE, NULL, &st);
            if (rc != OS_ERR_OK || !st.pc_valid)
                return ai_fail(result_json, result_cap, OS_ERR_FAIL, "no pc");
            addr64 = st.pc;
        }
        if (os_mainwin_src_follow_pc_safe((uint32_t)addr64, path, sizeof(path), &line) != 0)
            return ai_fail(result_json, result_cap, OS_ERR_FAIL, "follow failed");
        json_escape(path, esc, sizeof(esc));
        _snprintf(result_json, result_cap,
            "{\"ok\":true,\"address\":%u,\"address_hex\":\"0x%08X\",\"path\":\"%s\",\"line\":%d}",
            (unsigned)(addr64 & ~1u), (unsigned)(addr64 & ~1u), esc, line);
        return OS_ERR_OK;
    }
    if (strcmp(method, "cpu.state") == 0) {
        OS_CpuState st;
        int rc;
        char src_path[MAX_PATH] = "";
        int src_line = 0;
        if (!g_app.driver || !g_app.connected)
            return ai_fail(result_json, result_cap, OS_ERR_NOT_CONNECTED, "not connected");
        memset(&st, 0, sizeof(st));
        st.halted = -1;
        rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_GET_CPU_STATE, NULL, &st);
        if (rc != OS_ERR_OK)
            return ai_fail(result_json, result_cap, rc, "cpu.state failed");
        if (st.halted == 1 && st.pc_valid && g_app.elf) {
            char file[512];
            if (os_elf_line_of_addr(g_app.elf, st.pc, file, sizeof(file), &src_line) && src_line > 0)
                _snprintf(src_path, sizeof(src_path), "%s", file);
        }
        {
            char esc[40], pesc[MAX_PATH * 2];
            json_escape(st.state[0] ? st.state : "-", esc, sizeof(esc));
            json_escape(src_path, pesc, sizeof(pesc));
            _snprintf(result_json, result_cap,
                "{\"ok\":true,\"pc\":%u,\"pc_hex\":\"0x%08X\",\"halted\":%d,\"pc_valid\":%d,"
                "\"powered\":%d,\"under_reset\":%d,\"voltage_mv\":%d,\"state\":\"%s\","
                "\"source_path\":\"%s\",\"source_line\":%d}",
                (unsigned)st.pc, (unsigned)st.pc,
                st.halted, st.pc_valid, st.powered, st.under_reset, st.voltage_mv, esc,
                pesc, src_line);
        }
        return OS_ERR_OK;
    }
    if (strcmp(method, "elf.load") == 0) return method_elf_load(params, result_json, result_cap);
    if (strcmp(method, "elf.reload") == 0) {
        if (os_mainwin_reload_elf() != 0)
            return ai_fail(result_json, result_cap, OS_ERR_FAIL, "reload failed");
        _snprintf(result_json, result_cap, "{\"ok\":true,\"leaf_count\":%d}", g_app.leaf_count);
        return OS_ERR_OK;
    }
    if (strcmp(method, "vars.list") == 0) return method_vars_list(params, result_json, result_cap);
    if (strcmp(method, "vars.find") == 0) return method_vars_find(params, result_json, result_cap);
    if (strcmp(method, "vars.read") == 0) return method_vars_read(params, result_json, result_cap);
    if (strcmp(method, "vars.write") == 0) return method_vars_write(params, result_json, result_cap);
    if (strcmp(method, "vars.watch") == 0) return method_vars_watch(params, result_json, result_cap);
    if (strcmp(method, "mem.read") == 0) return method_mem_read(params, result_json, result_cap);
    if (strcmp(method, "mem.write") == 0) return method_mem_write(params, result_json, result_cap);
    if (strcmp(method, "acq.start") == 0) {
        int rc = os_fw_acq_start();
        if (rc != 0) return ai_fail(result_json, result_cap, rc, "acq start failed");
        os_mainwin_update_buttons();
        return ai_ok(result_json, result_cap, "\"acq_state\":1");
    }
    if (strcmp(method, "acq.stop") == 0) {
        os_fw_acq_stop();
        os_mainwin_update_buttons();
        return ai_ok(result_json, result_cap, "\"acq_state\":0");
    }
    if (strcmp(method, "acq.state") == 0) {
        _snprintf(result_json, result_cap,
            "{\"ok\":true,\"acq_state\":%d,\"watch_count\":%d,\"total_samples\":%ld}",
            (int)g_app.acq_state, g_app.watch_count, (long)g_app.total_samples);
        return OS_ERR_OK;
    }
    if (strcmp(method, "flash.download") == 0)
        return method_flash(params, 0, result_json, result_cap);
    if (strcmp(method, "flash.erase") == 0)
        return method_flash(params, 1, result_json, result_cap);
    if (strcmp(method, "recording.start") == 0)
        return method_recording_start(result_json, result_cap);
    if (strcmp(method, "recording.stop") == 0)
        return method_recording_stop(params, result_json, result_cap);
    if (strcmp(method, "log.read") == 0)
        return method_log_read(params, result_json, result_cap);
    if (strcmp(method, "log.clear") == 0)
        return method_log_clear(result_json, result_cap);
    if (strcmp(method, "alerts.read") == 0)
        return method_alerts_read(params, result_json, result_cap);
    if (strcmp(method, "ui.silent") == 0)
        return method_ui_silent(params, result_json, result_cap);
    /* subscribe/unsubscribe 由 aiapi.dll 在传输层处理；宿主侧返回提示 */
    if (strcmp(method, "log.subscribe") == 0 ||
        strcmp(method, "log.unsubscribe") == 0) {
        _snprintf(result_json, result_cap,
            "{\"ok\":true,\"note\":\"handled by aiapi WebSocket transport\"}");
        return OS_ERR_OK;
    }
    if (strcmp(method, "module.command") == 0)
        return method_module_command(params, result_json, result_cap);
    if (strncmp(method, "module.", 7) == 0)
        return method_module_info(method, result_json, result_cap);

    return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "unknown method");
}
