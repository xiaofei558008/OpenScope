#include "workspace.h"
#include "app.h"
#include "util.h"
#include "vartree.h"
#include "a2l_import.h"
#include "elf.h"

#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static OS_Workspace g_ws;
static int g_next_id = 1;

void os_ws_init(void)
{
    memset(&g_ws, 0, sizeof(g_ws));
    _snprintf(g_ws.name, sizeof(g_ws.name), "Default");
    g_ws.project_count = 0;
    g_ws.active = 0;
    g_next_id = 1;
    /* 默认：每个已加载探针驱动一个 Project + XCP，便于 J-Link/ST-Link/XCP 并行 */
    if (g_app.driver_count > 0) {
        int i;
        for (i = 0; i < g_app.driver_count && g_ws.project_count < OS_MAX_PROJECTS - 1; i++) {
            char nm[64];
            _snprintf(nm, sizeof(nm), "%s",
                      g_app.drivers[i] && g_app.drivers[i]->name
                          ? g_app.drivers[i]->name : "Probe");
            os_ws_add_project(nm, OS_PROJ_PROBE, i);
        }
    } else {
        os_ws_add_project("Probe", OS_PROJ_PROBE, 0);
    }
    os_ws_add_project("XCP", OS_PROJ_XCP, -1);
    g_ws.active = 0;
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
    p->driver_index = driver_index;
    p->xcp_baud = 115200;
    p->iface = 0;
    p->speed_khz = 0;
    p->probe_index = -1;
    _snprintf(p->device, sizeof(p->device), "Cortex-M4");
    _snprintf(p->xcp_transport, sizeof(p->xcp_transport), "serial");
    p->elf = NULL;
    g_ws.project_count++;
    return g_ws.project_count - 1;
}

int os_ws_remove_project(int index)
{
    int i;
    OS_Project* doomed;
    if (index < 0 || index >= g_ws.project_count || g_ws.project_count <= 1)
        return -1;
    doomed = &g_ws.projects[index];
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
    if (pr && pr->name[0])
        _snprintf(out, cap, "%s::%s", pr->name, L->name);
    else
        _snprintf(out, cap, "%s", L->name);
}

static void ws_default_path(wchar_t* out, int cap)
{
    wchar_t dir[MAX_PATH];
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", dir, MAX_PATH) > 0)
        _snwprintf(out, cap, L"%s\\OpenScope\\workspace.json", dir);
    else
        _snwprintf(out, cap, L"workspace.json");
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
    wchar_t path[MAX_PATH];
    char esc[512];
    if (path_w && path_w[0]) _snwprintf(path, MAX_PATH, L"%s", path_w);
    else ws_default_path(path, MAX_PATH);
    f = _wfopen(path, L"wb");
    if (!f) return -1;
    json_escape(g_ws.name, esc, sizeof(esc));
    fprintf(f, "{\n  \"name\": \"%s\",\n  \"active\": %d,\n  \"projects\": [\n",
            esc, g_ws.active);
    for (i = 0; i < g_ws.project_count; i++) {
        OS_Project* p = &g_ws.projects[i];
        char elf8[MAX_PATH], a2l8[MAX_PATH];
        char nesc[128], tesc[32], pesc[80], eesc[MAX_PATH], aesc[MAX_PATH], desc[160];
        os_wide_to_utf8_buf(p->elf_path, elf8, sizeof(elf8));
        os_wide_to_utf8_buf(p->a2l_path, a2l8, sizeof(a2l8));
        json_escape(p->name, nesc, sizeof(nesc));
        json_escape(p->xcp_transport, tesc, sizeof(tesc));
        json_escape(p->xcp_port, pesc, sizeof(pesc));
        json_escape(elf8, eesc, sizeof(eesc));
        json_escape(a2l8, aesc, sizeof(aesc));
        json_escape(p->device[0] ? p->device : "Cortex-M4", desc, sizeof(desc));
        fprintf(f,
                "    {\"id\":%d,\"name\":\"%s\",\"kind\":%d,\"driver_index\":%d,"
                "\"elf\":\"%s\",\"a2l\":\"%s\",\"xcp_transport\":\"%s\","
                "\"xcp_port\":\"%s\",\"xcp_baud\":%d,"
                "\"iface\":%d,\"speed_khz\":%d,\"probe_index\":%d,\"device\":\"%s\"}%s\n",
                p->id, nesc, (int)p->kind, p->driver_index,
                eesc, aesc, tesc, pesc, p->xcp_baud,
                p->iface, p->speed_khz, p->probe_index, desc,
                (i + 1 < g_ws.project_count) ? "," : "");
    }
    fprintf(f, "  ]\n}\n");
    fclose(f);
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
            pr->iface = json_int(chunk, "iface", 0);
            pr->speed_khz = json_int(chunk, "speed_khz", 0);
            pr->probe_index = json_int(chunk, "probe_index", -1);
            json_str(chunk, "device", pr->device, sizeof(pr->device));
            if (!pr->device[0]) _snprintf(pr->device, sizeof(pr->device), "Cortex-M4");
            pr->elf = NULL;
            g_ws.project_count++;
            p = end + 1;
        }
    }
    free(buf);
    if (g_ws.project_count <= 0) {
        os_ws_init();
        return 0;
    }
    if (g_ws.active < 0 || g_ws.active >= g_ws.project_count) g_ws.active = 0;
    _snwprintf(g_ws.path, MAX_PATH, L"%s", path);
    /* 恢复各 Project 的 ELF / A2L（符号进叶表） */
    for (i = 0; i < g_ws.project_count; i++) {
        OS_Project* pr = &g_ws.projects[i];
        if (pr->elf_path[0])
            os_vartree_merge_elf(pr->id, pr->elf_path);
        if (pr->a2l_path[0])
            os_a2l_import(pr->a2l_path, pr->id);
    }
    if (g_app.hTree) os_vartree_fill_tree(g_app.hTree);
    return 0;
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
