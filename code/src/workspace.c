#include "workspace.h"
#include "app.h"
#include "util.h"
#include "vartree.h"
#include "a2l_import.h"
#include "elf.h"
#include "module_mgr.h"
#include "xcpsym.h"

#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static OS_Workspace g_ws;
static int g_next_id = 1;

static int name_has_ci(const char* s, const char* needle)
{
    size_t nl;
    if (!s || !needle) return 0;
    nl = strlen(needle);
    if (!nl) return 0;
    for (; *s; s++) {
        size_t i;
        for (i = 0; i < nl; i++) {
            char a = s[i], b = needle[i];
            if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
            if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
            if (a != b) break;
        }
        if (i == nl) return 1;
    }
    return 0;
}

const char* os_ws_dev_kind_name(int dev_kind)
{
    switch (dev_kind) {
    case OS_DEVK_JLINK:  return "J-Link";
    case OS_DEVK_STLINK: return "ST-Link";
    case OS_DEVK_DAS:    return "miniWiggler";
    case OS_DEVK_XCP:    return "XCP";
    case OS_DEVK_NET:    return "网络";
    default:             return "设备";
    }
}

const char* os_ws_dev_kind_short(int dev_kind)
{
    switch (dev_kind) {
    case OS_DEVK_JLINK:  return "JLink";
    case OS_DEVK_STLINK: return "STLink";
    case OS_DEVK_DAS:    return "DAS";
    case OS_DEVK_XCP:    return "XCP";
    case OS_DEVK_NET:    return "NET";
    default:             return "DEV";
    }
}

int os_ws_dev_kind_from_module(const char* module_name)
{
    if (!module_name) return OS_DEVK_UNKNOWN;
    if (name_has_ci(module_name, "jlink")) return OS_DEVK_JLINK;
    if (name_has_ci(module_name, "stlink")) return OS_DEVK_STLINK;
    if (name_has_ci(module_name, "das") || name_has_ci(module_name, "miniwiggler"))
        return OS_DEVK_DAS;
    if (name_has_ci(module_name, "xcp")) return OS_DEVK_XCP;
    if (name_has_ci(module_name, "network") || name_has_ci(module_name, "net"))
        return OS_DEVK_NET;
    return OS_DEVK_UNKNOWN;
}

int os_ws_first_driver_for_kind(int dev_kind)
{
    const char* needle = NULL;
    int i;
    switch (dev_kind) {
    case OS_DEVK_JLINK:  needle = "jlink"; break;
    case OS_DEVK_STLINK: needle = "stlink"; break;
    case OS_DEVK_DAS:    needle = "das"; break;
    default: return -1;
    }
    for (i = 0; i < g_app.driver_count; i++) {
        if (name_has_ci(os_modmgr_driver_name(i), needle)) return i;
    }
    return -1;
}

int os_ws_bind_device_driver(OS_Project* p)
{
    if (!p) return -1;
    if (p->kind != OS_PROJ_PROBE) {
        p->driver_index = -1;
        return 0;
    }
    p->driver_index = os_ws_first_driver_for_kind(p->dev_kind);
    return p->driver_index >= 0 ? 0 : -1;
}

int os_ws_device_index_by_id(int id)
{
    int i;
    for (i = 0; i < g_ws.project_count; i++)
        if (g_ws.projects[i].id == id) return i;
    return -1;
}

int os_ws_default_device_index(void)
{
    int i;
    for (i = 0; i < g_ws.project_count; i++)
        if (g_ws.projects[i].kind == OS_PROJ_PROBE) return i;
    return g_ws.project_count > 0 ? 0 : -1;
}

void os_ws_init(void)
{
    int i;
    memset(&g_ws, 0, sizeof(g_ws));
    _snprintf(g_ws.name, sizeof(g_ws.name), "Default");
    g_ws.project_count = 0;
    g_ws.active = 0;
    g_next_id = 1;
    /* 优先恢复上次保存的设备列表（workspace.json） */
    if (os_ws_load(NULL) == 0 && g_ws.project_count > 0)
        return;
    memset(&g_ws, 0, sizeof(g_ws));
    _snprintf(g_ws.name, sizeof(g_ws.name), "Default");
    g_next_id = 1;
    /* 首次运行：每个已加载数据源模块一个设备，便于 J-Link/ST-Link/DAS/XCP/网络并行 */
    for (i = 0; i < g_app.driver_count && g_ws.project_count < OS_MAX_PROJECTS; i++) {
        int idx, kind;
        char nm[64];
        _snprintf(nm, sizeof(nm), "%s",
                  g_app.drivers[i] && g_app.drivers[i]->name ? g_app.drivers[i]->name : "Probe");
        kind = os_ws_dev_kind_from_module(nm);
        if (kind == OS_DEVK_UNKNOWN) kind = OS_DEVK_JLINK;
        idx = os_ws_add_device(kind, NULL);
        if (idx >= 0) g_ws.projects[idx].driver_index = i; /* 绑定到实际驱动槽位 */
    }
    if (g_app.driver_count == 0) {
        /* 无驱动模块时也放一个 J-Link 占位设备，用户可右键修改/加载符号 */
        os_ws_add_device(OS_DEVK_JLINK, "J-Link 1");
    }
    if (g_app.xcpmod) os_ws_add_device(OS_DEVK_XCP, "XCP 1");
    if (g_app.netmod) os_ws_add_device(OS_DEVK_NET, "网络 1");
    /* 默认当前设备优先 J-Link（用户最常用），否则第一个 */
    g_ws.active = 0;
    for (i = 0; i < g_ws.project_count; i++) {
        if (g_ws.projects[i].dev_kind == OS_DEVK_JLINK) { g_ws.active = i; break; }
    }
    if (g_app.hTree) os_vartree_fill_tree(g_app.hTree);
    os_log(OS_LOG_INFO, "设备列表: 新建 %d 个默认设备（右键空白处可继续添加）", g_ws.project_count);
    os_ws_save_auto();
}

OS_Workspace* os_ws(void) { return &g_ws; }

OS_Project* os_ws_active(void)
{
    if (g_ws.active < 0 || g_ws.active >= g_ws.project_count) return NULL;
    return &g_ws.projects[g_ws.active];
}

int os_ws_active_id(void)
{
    OS_Project* p = os_ws_active();
    return p ? p->id : 0;
}

OS_Project* os_ws_project_at(int index)
{
    if (index < 0 || index >= g_ws.project_count) return NULL;
    return &g_ws.projects[index];
}

OS_Project* os_ws_project(int id)
{
    int i;
    for (i = 0; i < g_ws.project_count; i++)
        if (g_ws.projects[i].id == id) return &g_ws.projects[i];
    return NULL;
}

int os_ws_add_project(const char* name, OS_ProjKind kind, int driver_index)
{
    OS_Project* p;
    if (g_ws.project_count >= OS_MAX_PROJECTS) return -1;
    p = &g_ws.projects[g_ws.project_count];
    memset(p, 0, sizeof(*p));
    p->id = g_next_id++;
    _snprintf(p->name, sizeof(p->name), "%s", name && name[0] ? name : "Project");
    p->kind = kind;
    p->dev_kind = (kind == OS_PROJ_XCP) ? OS_DEVK_XCP
                : (kind == OS_PROJ_NET) ? OS_DEVK_NET : OS_DEVK_UNKNOWN;
    if (kind == OS_PROJ_PROBE && driver_index >= 0)
        p->dev_kind = os_ws_dev_kind_from_module(os_modmgr_driver_name(driver_index));
    p->driver_index = driver_index;
    p->xcp_baud = 115200;
    p->iface = 0;
    p->speed_khz = 0;
    p->probe_index = -1;
    _snprintf(p->device, sizeof(p->device), "Cortex-M4");
    _snprintf(p->xcp_transport, sizeof(p->xcp_transport), "serial");
    _snprintf(p->xcp_port, sizeof(p->xcp_port), "COM3");
    _snprintf(p->net_ip, sizeof(p->net_ip), "127.0.0.1");
    p->net_port = 10000;
    p->net_mode = 0;
    p->elf = NULL;
    g_ws.project_count++;
    return g_ws.project_count - 1;
}

int os_ws_add_device(int dev_kind, const char* name)
{
    OS_ProjKind kind;
    char nm[64];
    int idx, i, n = 0;
    OS_Project* p;
    switch (dev_kind) {
    case OS_DEVK_XCP: kind = OS_PROJ_XCP; break;
    case OS_DEVK_NET: kind = OS_PROJ_NET; break;
    default:          kind = OS_PROJ_PROBE; break;
    }
    if (!name || !name[0]) {
        for (i = 0; i < g_ws.project_count; i++)
            if (g_ws.projects[i].dev_kind == dev_kind) n++;
        _snprintf(nm, sizeof(nm), "%s %d", os_ws_dev_kind_name(dev_kind), n + 1);
        name = nm;
    }
    idx = os_ws_add_project(name, kind, -1);
    if (idx < 0) return -1;
    p = &g_ws.projects[idx];
    p->dev_kind = dev_kind;
    if (dev_kind == OS_DEVK_STLINK) p->speed_khz = 4000;
    if (dev_kind == OS_DEVK_JLINK) p->speed_khz = 4000;
    if (dev_kind == OS_DEVK_DAS) { p->speed_khz = 0; _snprintf(p->device, sizeof(p->device), "TC375"); }
    os_ws_bind_device_driver(p);
    os_ws_save_auto();
    return idx;
}

int os_ws_remove_device(int index)
{
    if (os_ws_remove_project(index) != 0) return -1;
    os_ws_save_auto();
    return 0;
}

int os_ws_remove_project(int index)
{
    int i;
    OS_Project* doomed;
    if (index < 0 || index >= g_ws.project_count || g_ws.project_count <= 0)
        return -1;
    doomed = &g_ws.projects[index];
    os_vartree_remove_project(doomed->id, 0); /* 该设备的变量一并移除（叶 id 会压缩） */
    if (doomed->elf) {
        if (g_app.elf == doomed->elf) { g_app.elf = NULL; g_app.elf_path[0] = 0; }
        os_elf_close(doomed->elf);
        doomed->elf = NULL;
    }
    for (i = index; i < g_ws.project_count - 1; i++)
        g_ws.projects[i] = g_ws.projects[i + 1];
    g_ws.project_count--;
    if (g_ws.active >= g_ws.project_count) g_ws.active = g_ws.project_count - 1;
    return 0;
}

void os_ws_set_active(int index)
{
    if (index >= 0 && index < g_ws.project_count)
        g_ws.active = index;
}

int os_ws_resolve_mod(const OS_Project* pr, OS_Module** out_mod, void** out_ctx)
{
    if (!pr || !out_mod) return -1;
    *out_mod = NULL;
    if (out_ctx) *out_ctx = NULL;
    if (pr->kind == OS_PROJ_NET) {
        /* 网络设备不是轮询数据源：样本由 network.dll 推入，不参与 READ_MEM */
        return -1;
    }
    if (pr->kind == OS_PROJ_XCP) {
        if (!g_app.xcpmod || !g_app.xcp_connected) return -1;
        *out_mod = g_app.xcpmod;
        if (out_ctx) *out_ctx = g_app.xcpmod_ctx;
        return 0;
    }
    if (pr->driver_index >= 0 && pr->driver_index < g_app.driver_count) {
        OS_Module* m = g_app.drivers[pr->driver_index];
        void* ctx = g_app.driver_ctxs[pr->driver_index];
        int c = 0;
        if (!m || !m->command) return -1;
        m->command(ctx, OS_CMD_IS_CONNECTED, NULL, &c);
        if (!c) return -1;
        *out_mod = m;
        if (out_ctx) *out_ctx = ctx;
        return 0;
    }
    /* driver_index 未绑定时才用全局当前驱动（旧会话兼容） */
    if (pr->driver_index < 0 && g_app.driver && OS_APP_CONNECTED()) {
        *out_mod = g_app.driver;
        if (out_ctx) *out_ctx = g_app.driver_ctx;
        return 0;
    }
    return -1;
}

void os_ws_leaf_label(const OS_Leaf* L, char* out, int cap)
{
    OS_Project* pr;
    if (!out || cap < 8) return;
    out[0] = 0;
    if (!L) return;
    pr = os_ws_project(L->project_id);
    if (pr && pr->name[0]) {
        int i, dup = 0;
        for (i = 0; i < g_ws.project_count; i++)
            if (g_ws.projects[i].id != pr->id && !strcmp(g_ws.projects[i].name, pr->name)) { dup = 1; break; }
        /* 同名设备（例如两个 J-Link 都叫默认名）在混窗图例中加 #id 区分 */
        if (dup) _snprintf(out, cap, "%s #%d::%s", pr->name, pr->id, L->name);
        else _snprintf(out, cap, "%s::%s", pr->name, L->name);
    } else {
        _snprintf(out, cap, "%s", L->name);
    }
}

static void ws_default_path(wchar_t* out, int cap)
{
    wchar_t dir[MAX_PATH];
    /* 测试/多实例可指定独立工作区（自动化脚本用） */
    if (GetEnvironmentVariableW(L"OPENSCOPE_WORKSPACE", out, cap) > 0 && out[0])
        return;
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", dir, MAX_PATH) > 0) {
        wchar_t sub[MAX_PATH];
        _snwprintf(sub, MAX_PATH, L"%s\\OpenScope", dir);
        CreateDirectoryW(sub, NULL);
        _snwprintf(out, cap, L"%s\\workspace.json", sub);
    } else {
        _snwprintf(out, cap, L"workspace.json");
    }
}

static void json_escape(const char* in, char* out, int cap)
{
    int o = 0;
    if (!in) in = "";
    while (*in && o < cap - 2) {
        unsigned char c = (unsigned char)*in++;
        if (c == '"' || c == '\\') {
            if (o + 2 >= cap) break;
            out[o++] = '\\';
            out[o++] = (char)c;
        } else if (c < 0x20) {
            continue;
        } else {
            out[o++] = (char)c;
        }
    }
    out[o] = 0;
}

int os_ws_save(const wchar_t* path_w)
{
    FILE* f;
    int i;
    wchar_t path[MAX_PATH], tmp[MAX_PATH];
    char esc[512];
    if (path_w && path_w[0]) _snwprintf(path, MAX_PATH, L"%s", path_w);
    else ws_default_path(path, MAX_PATH);
    /* 原子写：先写同目录临时文件再替换，避免两个 OpenScope 实例同时保存时读到半截 JSON */
    _snwprintf(tmp, MAX_PATH, L"%s.tmp%lu", path, (unsigned long)GetCurrentProcessId());
    f = _wfopen(tmp, L"wb");
    if (!f) return -1;
    json_escape(g_ws.name, esc, sizeof(esc));
    fprintf(f, "{\n  \"version\": 2,\n  \"name\": \"%s\",\n  \"active\": %d,\n  \"projects\": [\n",
            esc, g_ws.active);
    for (i = 0; i < g_ws.project_count; i++) {
        OS_Project* p = &g_ws.projects[i];
        char elf8[MAX_PATH], a2l8[MAX_PATH], map8[MAX_PATH];
        char nesc[128], tesc[32], pesc[80], eesc[MAX_PATH], aesc[MAX_PATH], mesc[MAX_PATH], desc[160];
        char seresc[96], ipesc[96];
        os_wide_to_utf8_buf(p->elf_path, elf8, sizeof(elf8));
        os_wide_to_utf8_buf(p->a2l_path, a2l8, sizeof(a2l8));
        os_wide_to_utf8_buf(p->map_path, map8, sizeof(map8));
        json_escape(p->name, nesc, sizeof(nesc));
        json_escape(p->xcp_transport, tesc, sizeof(tesc));
        json_escape(p->xcp_port, pesc, sizeof(pesc));
        json_escape(elf8, eesc, sizeof(eesc));
        json_escape(a2l8, aesc, sizeof(aesc));
        json_escape(map8, mesc, sizeof(mesc));
        json_escape(p->device[0] ? p->device : "Cortex-M4", desc, sizeof(desc));
        json_escape(p->serial, seresc, sizeof(seresc));
        json_escape(p->net_ip, ipesc, sizeof(ipesc));
        fprintf(f,
                "    {\"id\":%d,\"name\":\"%s\",\"kind\":%d,\"dev_kind\":%d,\"driver_index\":%d,"
                "\"elf\":\"%s\",\"a2l\":\"%s\",\"map\":\"%s\",\"xcp_transport\":\"%s\","
                "\"xcp_port\":\"%s\",\"xcp_baud\":%d,"
                "\"iface\":%d,\"speed_khz\":%d,\"probe_index\":%d,\"under_reset\":%d,\"attach\":%d,"
                "\"serial\":\"%s\",\"device\":\"%s\","
                "\"net_ip\":\"%s\",\"net_port\":%d,\"net_mode\":%d}%s\n",
                p->id, nesc, (int)p->kind, p->dev_kind, p->driver_index,
                eesc, aesc, mesc, tesc, pesc, p->xcp_baud,
                p->iface, p->speed_khz, p->probe_index, p->connect_under_reset,
                p->attach_default, seresc, desc,
                ipesc, p->net_port, p->net_mode,
                (i + 1 < g_ws.project_count) ? "," : "");
    }
    fprintf(f, "  ]\n}\n");
    fclose(f);
    if (!MoveFileExW(tmp, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(tmp);
        return -1;
    }
    _snwprintf(g_ws.path, MAX_PATH, L"%s", path);
    return 0;
}

/* 极简 JSON 字符串字段提取：找 "key": "value" 或 "key": number */
static int json_str(const char* src, const char* key, char* out, int cap)
{
    char pat[80];
    const char* p;
    int n = 0;
    _snprintf(pat, sizeof(pat), "\"%s\"", key);
    p = strstr(src, pat);
    if (!p) { if (out && cap) out[0] = 0; return -1; }
    p = strchr(p + strlen(pat), ':');
    if (!p) return -1;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '"') {
        p++;
        while (*p && *p != '"' && n < cap - 1) {
            if (*p == '\\' && p[1]) p++;
            out[n++] = *p++;
        }
        out[n] = 0;
        return 0;
    }
    while (*p && *p != ',' && *p != '}' && *p != ']' && n < cap - 1)
        out[n++] = *p++;
    while (n > 0 && (out[n - 1] == ' ' || out[n - 1] == '\t')) n--;
    out[n] = 0;
    return 0;
}

static int json_int(const char* src, const char* key, int def)
{
    char buf[64];
    if (json_str(src, key, buf, sizeof(buf)) != 0) return def;
    return atoi(buf);
}

int os_ws_load(const wchar_t* path_w)
{
    FILE* f;
    char* buf = NULL;
    long sz;
    wchar_t path[MAX_PATH];
    const char* p;
    int i;
    if (path_w && path_w[0]) _snwprintf(path, MAX_PATH, L"%s", path_w);
    else ws_default_path(path, MAX_PATH);
    f = _wfopen(path, L"rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0 || sz > 4 * 1024 * 1024) { fclose(f); return -1; }
    buf = (char*)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return -1; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return -1; }
    fclose(f);
    buf[sz] = 0;

    memset(&g_ws, 0, sizeof(g_ws));
    g_next_id = 1;
    json_str(buf, "name", g_ws.name, sizeof(g_ws.name));
    if (!g_ws.name[0]) _snprintf(g_ws.name, sizeof(g_ws.name), "Default");
    g_ws.active = json_int(buf, "active", 0);

    p = strstr(buf, "\"projects\"");
    if (p) p = strchr(p, '[');
    if (p) {
        p++;
        while (*p && g_ws.project_count < OS_MAX_PROJECTS) {
            const char* obj = strchr(p, '{');
            const char* end;
            char chunk[2048];
            int n;
            OS_Project* pr;
            char tmp[MAX_PATH];
            if (!obj) break;
            end = strchr(obj, '}');
            if (!end) break;
            n = (int)(end - obj + 1);
            if (n >= (int)sizeof(chunk)) n = (int)sizeof(chunk) - 1;
            memcpy(chunk, obj, (size_t)n);
            chunk[n] = 0;
            pr = &g_ws.projects[g_ws.project_count];
            memset(pr, 0, sizeof(*pr));
            pr->id = json_int(chunk, "id", g_next_id);
            if (pr->id >= g_next_id) g_next_id = pr->id + 1;
            json_str(chunk, "name", pr->name, sizeof(pr->name));
            if (!pr->name[0]) _snprintf(pr->name, sizeof(pr->name), "Project");
            pr->kind = (OS_ProjKind)json_int(chunk, "kind", 0);
            pr->dev_kind = json_int(chunk, "dev_kind", OS_DEVK_UNKNOWN);
            pr->driver_index = json_int(chunk, "driver_index", -1);
            pr->xcp_baud = json_int(chunk, "xcp_baud", 115200);
            json_str(chunk, "xcp_transport", pr->xcp_transport, sizeof(pr->xcp_transport));
            if (!pr->xcp_transport[0])
                _snprintf(pr->xcp_transport, sizeof(pr->xcp_transport), "serial");
            json_str(chunk, "xcp_port", pr->xcp_port, sizeof(pr->xcp_port));
            if (json_str(chunk, "elf", tmp, sizeof(tmp)) == 0 && tmp[0])
                os_utf8_to_wide_buf(tmp, pr->elf_path, MAX_PATH);
            if (json_str(chunk, "a2l", tmp, sizeof(tmp)) == 0 && tmp[0])
                os_utf8_to_wide_buf(tmp, pr->a2l_path, MAX_PATH);
            if (json_str(chunk, "map", tmp, sizeof(tmp)) == 0 && tmp[0])
                os_utf8_to_wide_buf(tmp, pr->map_path, MAX_PATH);
            pr->iface = json_int(chunk, "iface", 0);
            pr->speed_khz = json_int(chunk, "speed_khz", 0);
            pr->probe_index = json_int(chunk, "probe_index", -1);
            pr->connect_under_reset = json_int(chunk, "under_reset", 0);
            pr->attach_default = json_int(chunk, "attach", 0);
            json_str(chunk, "serial", pr->serial, sizeof(pr->serial));
            json_str(chunk, "device", pr->device, sizeof(pr->device));
            if (!pr->device[0]) _snprintf(pr->device, sizeof(pr->device), "Cortex-M4");
            json_str(chunk, "net_ip", pr->net_ip, sizeof(pr->net_ip));
            if (!pr->net_ip[0]) _snprintf(pr->net_ip, sizeof(pr->net_ip), "127.0.0.1");
            pr->net_port = json_int(chunk, "net_port", 10000);
            pr->net_mode = json_int(chunk, "net_mode", 0);
            /* 旧工程没有 dev_kind：按类别/驱动名推导（J-Link/ST-Link/DAS） */
            if (pr->dev_kind == OS_DEVK_UNKNOWN) {
                if (pr->kind == OS_PROJ_XCP) pr->dev_kind = OS_DEVK_XCP;
                else if (pr->kind == OS_PROJ_NET) pr->dev_kind = OS_DEVK_NET;
                else pr->dev_kind = os_ws_dev_kind_from_module(
                    os_modmgr_driver_name(pr->driver_index));
                if (pr->dev_kind == OS_DEVK_UNKNOWN) pr->dev_kind = OS_DEVK_JLINK;
            }
            pr->elf = NULL;
            g_ws.project_count++;
            p = end + 1;
        }
    }
    free(buf);
    if (g_ws.project_count <= 0) return -1;
    if (g_ws.active < 0 || g_ws.active >= g_ws.project_count) g_ws.active = 0;
    _snwprintf(g_ws.path, MAX_PATH, L"%s", path);
    /* 驱动下标是运行时值：按设备类型重新绑定（J-Link/ST-Link/DAS 各一个实例） */
    for (i = 0; i < g_ws.project_count; i++) {
        OS_Project* pr = &g_ws.projects[i];
        if (pr->kind == OS_PROJ_PROBE) os_ws_bind_device_driver(pr);
    }
    /* 恢复各设备 ELF / A2L（符号进叶表） */
    for (i = 0; i < g_ws.project_count; i++) {
        OS_Project* pr = &g_ws.projects[i];
        if (pr->elf_path[0])
            os_vartree_merge_elf(pr->id, pr->elf_path);
        if (pr->a2l_path[0])
            os_a2l_import(pr->a2l_path, pr->id);
        if (pr->map_path[0]) {
            char err[256];
            if (os_xcpsym_load_map(pr->map_path, err, sizeof(err)) >= 0) {
                OS_XcpSym s;
                int k, total = os_xcpsym_count();
                for (k = 0; k < total; k++)
                    if (os_xcpsym_get(k, &s) == 0 && s.from_map && s.name[0])
                        os_vartree_add_map_symbol(pr->id, s.name, s.address, s.size);
            } else {
                os_log(OS_LOG_WARN, "设备 %s 的 MAP 恢复失败: %s", pr->name, err);
            }
        }
    }
    if (g_app.hTree) os_vartree_fill_tree(g_app.hTree);
    os_log(OS_LOG_INFO, "设备列表已恢复: %d 个设备（active=%d）", g_ws.project_count, g_ws.active);
    return 0;
}

int os_ws_save_auto(void)
{
    return os_ws_save(NULL);
}

int os_ws_load_auto(void)
{
    return os_ws_load(NULL);
}

int os_ws_save_dialog(HWND owner)
{
    OPENFILENAMEW ofn;
    wchar_t path[MAX_PATH];
    _snwprintf(path, MAX_PATH, L"%s", g_ws.path[0] ? g_ws.path : L"workspace.json");
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner ? owner : g_app.hMain;
    ofn.lpstrFilter = L"OpenScope Workspace (*.json)\0*.json\0All\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = L"json";
    if (!GetSaveFileNameW(&ofn)) return -1;
    return os_ws_save(path);
}

int os_ws_load_dialog(HWND owner)
{
    OPENFILENAMEW ofn;
    wchar_t path[MAX_PATH];
    path[0] = 0;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner ? owner : g_app.hMain;
    ofn.lpstrFilter = L"OpenScope Workspace (*.json)\0*.json\0All\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetOpenFileNameW(&ofn)) return -1;
    return os_ws_load(path);
}
