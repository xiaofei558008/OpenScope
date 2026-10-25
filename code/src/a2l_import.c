/*
 * ASAP2 / A2L 精简导入：标量 CHARACTERISTIC / MEASUREMENT + ECU_ADDRESS。
 * 不解析 AXIS/MAP 曲线表（第一期）。
 */
#include "a2l_import.h"
#include "app.h"
#include "vartree.h"
#include "xcpsym.h"
#include "workspace.h"
#include "util.h"

#include <commdlg.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* skip_ws(char* p)
{
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
    return p;
}

static char* read_ident(char* p, char* out, int cap)
{
    int n = 0;
    p = skip_ws(p);
    if (*p == '"') {
        p++;
        while (*p && *p != '"' && n < cap - 1) out[n++] = *p++;
        if (*p == '"') p++;
    } else {
        while (*p && !isspace((unsigned char)*p) && *p != '/' && n < cap - 1)
            out[n++] = *p++;
    }
    out[n] = 0;
    return p;
}

static int parse_addr(const char* s, uint64_t* out)
{
    char* end = NULL;
    unsigned long long v;
    if (!s || !s[0]) return -1;
    v = strtoull(s, &end, 0);
    if (end == s) return -1;
    *out = (uint64_t)v;
    return 0;
}

static int a2l_dtype_size(const char* t)
{
    if (!_stricmp(t, "UBYTE") || !_stricmp(t, "SBYTE")) return 1;
    if (!_stricmp(t, "UWORD") || !_stricmp(t, "SWORD")) return 2;
    if (!_stricmp(t, "ULONG") || !_stricmp(t, "SLONG") ||
        !_stricmp(t, "FLOAT32_IEEE") || !_stricmp(t, "FLOAT32")) return 4;
    if (!_stricmp(t, "FLOAT64_IEEE") || !_stricmp(t, "FLOAT64") ||
        !_stricmp(t, "A_UINT64") || !_stricmp(t, "A_INT64")) return 8;
    return 4;
}

static int a2l_is_float(const char* t)
{
    return !_stricmp(t, "FLOAT32_IEEE") || !_stricmp(t, "FLOAT32") ||
           !_stricmp(t, "FLOAT64_IEEE") || !_stricmp(t, "FLOAT64");
}

static int a2l_is_signed(const char* t)
{
    return t[0] == 'S' || t[0] == 's' || a2l_is_float(t) ||
           !_stricmp(t, "A_INT64");
}

static int add_leaf_from_a2l(const char* name, uint64_t addr, uint32_t size,
                             int is_float, int is_signed, int project_id)
{
    OS_Leaf* L;
    int id;
    if (!name || !name[0] || size == 0 || size > 64) return -1;
    if (g_app.leaf_count >= OS_MAX_LEAVES) return -1;
    /* 同 project 同名则更新地址 */
    for (id = 0; id < g_app.leaf_count; id++) {
        if (g_app.leaves[id].project_id == project_id &&
            strcmp(g_app.leaves[id].name, name) == 0) {
            g_app.leaves[id].address = addr;
            g_app.leaves[id].size = size;
            g_app.leaves[id].from_a2l = 1;
            return 0;
        }
    }
    id = g_app.leaf_count++;
    L = &g_app.leaves[id];
    memset(L, 0, sizeof(*L));
    L->id = id;
    _snprintf(L->name, sizeof(L->name), "%s", name);
    L->address = addr;
    L->size = size;
    L->kind = is_float ? OS_TYPE_FLOAT : OS_TYPE_INT;
    L->is_signed = is_signed ? 1 : 0;
    L->from_a2l = 1;
    L->project_id = project_id;
    return 1;
}

int os_a2l_import(const wchar_t* path_w, int project_id)
{
    FILE* f;
    char* buf = NULL;
    long sz;
    char* p;
    int added = 0;
    if (!path_w || !path_w[0]) return -1;
    f = _wfopen(path_w, L"rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0 || sz > 32 * 1024 * 1024) { fclose(f); return -1; }
    buf = (char*)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return -1; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return -1; }
    fclose(f);
    buf[sz] = 0;

    p = buf;
    while (*p) {
        char* line = p;
        char* nl = strchr(p, '\n');
        char kind[32], name[256], dtype[64], addr_s[64];
        uint64_t addr = 0;
        int size, is_f, is_s, rc;
        if (nl) { *nl = 0; p = nl + 1; } else { p += strlen(p); }
        line = skip_ws(line);
        if (!*line || line[0] == '/' || line[0] == '*') continue;
        /* /begin CHARACTERISTIC name "desc" type ... ECU_ADDRESS 0x.. */
        if (_strnicmp(line, "/begin", 6) != 0) continue;
        line = skip_ws(line + 6);
        line = read_ident(line, kind, sizeof(kind));
        if (_stricmp(kind, "CHARACTERISTIC") != 0 &&
            _stricmp(kind, "MEASUREMENT") != 0)
            continue;
        line = read_ident(line, name, sizeof(name));
        if (!name[0]) continue;
        /* 跳过描述字符串 */
        line = skip_ws(line);
        if (*line == '"') {
            line++;
            while (*line && *line != '"') line++;
            if (*line == '"') line++;
        }
        line = read_ident(line, dtype, sizeof(dtype)); /* type 或 datatype */
        if (!dtype[0]) _snprintf(dtype, sizeof(dtype), "ULONG");
        /* 在后续若干 token / 同行中找 ECU_ADDRESS 或地址字面量 */
        addr_s[0] = 0;
        {
            char* q = line;
            while (*q) {
                char tok[64];
                q = read_ident(q, tok, sizeof(tok));
                if (!_stricmp(tok, "ECU_ADDRESS") || !_stricmp(tok, "ADDRESS")) {
                    q = read_ident(q, addr_s, sizeof(addr_s));
                    break;
                }
                if (tok[0] == '0' && (tok[1] == 'x' || tok[1] == 'X')) {
                    _snprintf(addr_s, sizeof(addr_s), "%s", tok);
                    break;
                }
            }
        }
        /* 若本行没有地址，向后扫描最多 12 行（含 SYMBOL_LINK） */
        if (!addr_s[0]) {
            char* q = p;
            int rows = 0;
            char sym_link[256];
            sym_link[0] = 0;
            while (*q && rows < 12) {
                char* e = strchr(q, '\n');
                char line2[512], tok[64];
                int n = e ? (int)(e - q) : (int)strlen(q);
                if (n >= (int)sizeof(line2)) n = (int)sizeof(line2) - 1;
                memcpy(line2, q, (size_t)n);
                line2[n] = 0;
                {
                    char* r = skip_ws(line2);
                    if (_strnicmp(r, "/end", 4) == 0) break;
                    r = read_ident(r, tok, sizeof(tok));
                    if (!_stricmp(tok, "ECU_ADDRESS") || !_stricmp(tok, "ADDRESS")) {
                        read_ident(r, addr_s, sizeof(addr_s));
                    } else if (!_stricmp(tok, "SYMBOL_LINK")) {
                        read_ident(r, sym_link, sizeof(sym_link));
                        if (sym_link[0]) _snprintf(name, sizeof(name), "%s", sym_link);
                    }
                    if (addr_s[0] && (!sym_link[0] || rows >= 3)) break;
                }
                q = e ? e + 1 : q + strlen(q);
                rows++;
            }
        }
        if (parse_addr(addr_s, &addr) != 0 || addr == 0) continue;
        size = a2l_dtype_size(dtype);
        is_f = a2l_is_float(dtype);
        is_s = a2l_is_signed(dtype);
        rc = add_leaf_from_a2l(name, addr, (uint32_t)size, is_f, is_s, project_id);
        if (rc > 0) added++;
    }
    free(buf);
    if (added > 0) {
        os_xcpsym_invalidate();
        if (g_app.hTree) os_vartree_fill_tree(g_app.hTree);
        os_log(OS_LOG_INFO, "A2L 导入: +%d 变量 (project=%d)", added, project_id);
    }
    return added;
}

int os_a2l_import_dialog(HWND owner, int project_id)
{
    OPENFILENAMEW ofn;
    wchar_t path[MAX_PATH];
    path[0] = 0;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner ? owner : g_app.hMain;
    ofn.lpstrFilter = L"ASAP2 A2L (*.a2l)\0*.a2l\0All\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetOpenFileNameW(&ofn)) return 0;
    {
        int n = os_a2l_import(path, project_id);
        if (n > 0) {
            OS_Project* pr = os_ws_project(project_id);
            if (pr) _snwprintf(pr->a2l_path, MAX_PATH, L"%s", path);
        }
        return n;
    }
}
