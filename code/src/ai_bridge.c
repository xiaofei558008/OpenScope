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
    {"drivers.list", "驱动模块列表", "{}"},
    {"drivers.select", "选择当前驱动", "{\"index\":0}"},
    {"probe.scan", "扫描仿真器", "{}"},
    {"connect", "连接 MCU", "{\"device\":\"Cortex-M4\",\"iface\":0,\"speed_khz\":4000,\"probe_index\":-1}"},
    {"disconnect", "断开 MCU", "{}"},
    {"halt", "Halt CPU", "{}"},
    {"go", "Go/Run CPU", "{}"},
    {"reset", "复位目标", "{}"},
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
        "\"driver_index\":%d,\"elf\":\"%s\",\"mod_count\":%d,\"poll_interval_ms\":%d}",
        OS_VERSION_STR, OS_API_VERSION, connected, acq, g_app.recording,
        g_app.leaf_count, g_app.watch_count, desc, di, esc, g_app.mod_count,
        g_app.poll_interval_ms);
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
    _snprintf(out, cap,
        "{\"ok\":true,\"connected\":%d,\"emulator\":\"%s\",\"dll\":\"%s\"}",
        c, info.emulator[0] ? info.emulator : "", info.dll_version);
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
    char path8[MAX_PATH];
    wchar_t pathw[MAX_PATH];
    int rc;
    if (json_get_str(params, "path", path8, sizeof(path8)) != 0 || !path8[0])
        return ai_fail(out, cap, OS_ERR_INVALID_ARG, "path required");
    os_utf8_to_wide_buf(path8, pathw, MAX_PATH);
    rc = os_mainwin_open_elf(pathw);
    if (rc != 0) return ai_fail(out, cap, OS_ERR_FAIL, "elf load failed");
    _snprintf(out, cap, "{\"ok\":true,\"leaf_count\":%d,\"path\":\"%s\"}",
              g_app.leaf_count, path8);
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
        _snprintf(out, cap, "{\"ok\":true,\"recording\":0,\"saved\":1,\"path\":\"%s\"}", path8);
        return OS_ERR_OK;
    }
    if (was_running) os_ds_start();
    os_mainwin_update_buttons();
    return ai_ok(out, cap, "\"recording\":0,\"saved\":0");
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
    if (strcmp(method, "probe.scan") == 0) return method_scan(result_json, result_cap);
    if (strcmp(method, "connect") == 0) return method_connect(params, result_json, result_cap);
    if (strcmp(method, "disconnect") == 0) return method_disconnect(result_json, result_cap);
    if (strcmp(method, "halt") == 0) return method_drv_simple(OS_CMD_HALT, result_json, result_cap);
    if (strcmp(method, "go") == 0) return method_drv_simple(OS_CMD_GO, result_json, result_cap);
    if (strcmp(method, "reset") == 0) return method_drv_simple(OS_CMD_RESET, result_json, result_cap);
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
    if (strcmp(method, "module.command") == 0)
        return method_module_command(params, result_json, result_cap);
    if (strncmp(method, "module.", 7) == 0)
        return method_module_info(method, result_json, result_cap);

    return ai_fail(result_json, result_cap, OS_ERR_INVALID_ARG, "unknown method");
}
