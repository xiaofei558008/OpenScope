/*
 * OpenScope 布局保存/恢复/导入导出（request.md 新增需求 1/2）。
 *
 * 格式：UTF-8 文本，windows 逐条 [win]，vars 用 '|' 分隔（变量完整叶名）。
 * 关闭时自动保存到默认路径；启动时自动恢复；菜单“保存布局为.../加载布局...”可分享。
 */
#include "app.h"
#include "layout.h"
#include "chartwin.h"
#include "numwin.h"
#include "vartree.h"
#include "mainwin.h"
#include "theme.h"

#include <io.h>
#include <stdio.h>
#include <string.h>

#define LAYOUT_VERSION 1
#define MAX_WIN_VARS   64
#define MAX_PENDING    512

typedef struct PendingVar {
    HWND  hwnd;
    char  name[256];
} PendingVar;

static PendingVar g_pending[MAX_PENDING];
static int g_pending_count;
static char g_probe[2];

void os_layout_apply_var(HWND h, const char* name);

/* ---------------- 窗口类型 ---------------- */

static int win_type_of(HWND hwnd, char* type, int cap, OS_Module** mod, void** ctx)
{
    int i;
    *mod = NULL;
    *ctx = NULL;
    if (os_chart_is(hwnd)) { _snprintf(type, cap, "%s", "chart"); return 1; }
    if (os_num_is(hwnd)) { _snprintf(type, cap, "%s", "num"); return 1; }
    for (i = 0; i < g_app.win_count; i++) {
        OS_WinItem* wi = &g_app.wins[i];
        if (wi->hwnd == hwnd && wi->is_module && wi->mod) {
            const OS_WindowType* wt = wi->mod->window_types;
            if (wt && wt->type) _snprintf(type, cap, "%s", wt->type);
            else _snprintf(type, cap, "%s", "module");
            *mod = wi->mod;
            *ctx = wi->mod_ctx;
            return 1;
        }
    }
    return 0;
}

static int win_var_count(HWND hwnd, int is_module, OS_Module* m, void* ctx)
{
    int n = 0;
    if (is_module && m && m->api_version >= 3 && m->win_enum_var) {
        while (m->win_enum_var(ctx, hwnd, n, g_probe, 1)) n++;
        return n;
    }
    if (os_chart_is(hwnd)) {
        while (os_chart_var_name(hwnd, n, g_probe, 1)) n++;
        return n;
    }
    if (os_num_is(hwnd)) {
        while (os_num_var_name(hwnd, n, g_probe, 1)) n++;
        return n;
    }
    return 0;
}

static int win_var_name(HWND hwnd, int is_module, OS_Module* m, void* ctx,
                        int idx, char* out, int cap)
{
    if (is_module && m && m->api_version >= 3 && m->win_enum_var)
        return m->win_enum_var(ctx, hwnd, idx, out, cap);
    if (os_chart_is(hwnd)) return os_chart_var_name(hwnd, idx, out, cap);
    if (os_num_is(hwnd)) return os_num_var_name(hwnd, idx, out, cap);
    return 0;
}

/* ---------------- 保存 ---------------- */

static void write_utf8_bom(FILE* f)
{
    fputs("\xEF\xBB\xBF", f);
}

static void write_utf8_line(FILE* f, const char* fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    fputs(buf, f);
    fputc('\n', f);
}

int os_layout_save_to(const wchar_t* path)
{
    FILE* f;
    RECT r;
    int i, active;
    if (!path) return -1;
    f = _wfopen(path, L"wb");
    if (!f) return -1;
    write_utf8_bom(f);
    write_utf8_line(f, "[layout]");
    write_utf8_line(f, "version=%d", LAYOUT_VERSION);
    GetWindowRect(g_app.hMain, &r);
    write_utf8_line(f, "main_x=%d", r.left);
    write_utf8_line(f, "main_y=%d", r.top);
    write_utf8_line(f, "main_w=%d", r.right - r.left);
    write_utf8_line(f, "main_h=%d", r.bottom - r.top);
    write_utf8_line(f, "tree_w=%d", g_app.tree_w);
    write_utf8_line(f, "log_h=%d", g_app.log_h);
    write_utf8_line(f, "log_hidden=%d", g_app.log_hidden); /* F22: 消息栏抽屉收起状态 */
    active = os_mainwin_active_tab();
    write_utf8_line(f, "active=%d", active);
    write_utf8_line(f, "theme=%d", os_theme_dark() ? 1 : 0); /* F20 */
    write_utf8_line(f, "wins=%d", g_app.win_count);
    for (i = 0; i < g_app.win_count; i++) {
        OS_WinItem* wi = &g_app.wins[i];
        char type[64] = "?";
        OS_Module* mod = NULL;
        void* ctx = NULL;
        char title8[512];
        int nv, k;
        if (!wi->hwnd) continue;
        if (!win_type_of(wi->hwnd, type, sizeof(type), &mod, &ctx)) continue;
        os_wide_to_utf8_buf(wi->title, title8, sizeof(title8));
        write_utf8_line(f, "[win]");
        write_utf8_line(f, "type=%s", type);
        write_utf8_line(f, "title=%s", title8);
        nv = win_var_count(wi->hwnd, wi->is_module, mod, ctx);
        if (nv > MAX_WIN_VARS) nv = MAX_WIN_VARS;
        {
            char vars[MAX_WIN_VARS][300];
            int count = 0;
            for (k = 0; k < nv; k++) {
                if (win_var_name(wi->hwnd, wi->is_module, mod, ctx, k,
                                 vars[count], sizeof(vars[count])))
                    count++;
            }
            write_utf8_line(f, "vars=%s", count ? vars[0] : "");
            for (k = 1; k < count; k++)
                write_utf8_line(f, "vars+=%s", vars[k]);
        }
    }
    fclose(f);
    os_log(OS_LOG_INFO, "布局已保存: %ls (%d 个窗口)", path, g_app.win_count);
    return 0;
}

/* ---------------- 默认路径 ---------------- */

static void default_path(wchar_t* out, int cap, int for_save)
{
    wchar_t exe[MAX_PATH];
    wchar_t dir[MAX_PATH];
    wchar_t* slash;
    DWORD n = GetModuleFileNameW(NULL, exe, MAX_PATH);
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", dir, MAX_PATH) > 0) {
        _snwprintf(out, cap, L"%s\\OpenScope", dir);
        CreateDirectoryW(out, NULL);
        _snwprintf(out, cap, L"%s\\layout.ini", out);
        return;
    }
    if (n && (slash = wcsrchr(exe, L'\\'))) {
        *slash = 0;
        _snwprintf(out, cap, L"%s\\layout.ini", exe);
        if (for_save) {
            FILE* t = _wfopen(out, L"ab");
            if (t) { fclose(t); return; }
        }
    }
    _snwprintf(out, cap, L"layout.ini");
}

void os_layout_save_auto(void)
{
    wchar_t path[MAX_PATH];
    default_path(path, MAX_PATH, 1);
    os_layout_save_to(path);
}

/* ---------------- 加载 ---------------- */

typedef struct LayoutWin {
    char type[64];
    char title[512];
    char vars[MAX_WIN_VARS][300];
    int  nvars;
} LayoutWin;

typedef struct LayoutData {
    int main_x, main_y, main_w, main_h;
    int tree_w, log_h, active;
    LayoutWin wins[OS_MAX_WINS];
    int nwins;
} LayoutData;

static void parse_key(char* line, char* key, int keycap, char* val, int valcap)
{
    char* eq = strchr(line, '=');
    if (!eq) {
        /* 无 '=' 的行：整行作为 key（兼容旧版多变量续行 "vars+name"） */
        _snprintf(key, keycap, "%s", line);
        val[0] = 0;
        return;
    }
    *eq = 0;
    _snprintf(key, keycap, "%s", line);
    _snprintf(val, valcap, "%s", eq + 1);
}

static void trim_crlf(char* s)
{
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) s[--n] = 0;
}

int os_layout_load_from(const wchar_t* path)
{
    FILE* f;
    char line[1024];
    LayoutData* ld;
    int i, ok = 0;
    /* LayoutData 含 64×64 变量名缓冲区，约 1.3MB，必须堆分配避免主线程栈溢出 */
    ld = (LayoutData*)calloc(1, sizeof(LayoutData));
    if (!ld) return -1;
    ld->main_x = -1;
    ld->main_y = -1;
    ld->tree_w = -1;
    ld->log_h = -1;
    ld->active = -1;
    if (!path) return -1;
    f = _wfopen(path, L"rb");
    if (!f) { free(ld); return -1; }
    {
        LayoutWin* cur = NULL;
        while (fgets(line, sizeof(line), f)) {
            char key[64], val[900];
            trim_crlf(line);
            if (strcmp(line, "[win]") == 0) {
                if (ld->nwins < OS_MAX_WINS) cur = &ld->wins[ld->nwins++];
                else cur = NULL;
                continue;
            }
            parse_key(line, key, sizeof(key), val, sizeof(val));
            if (!cur) {
                if (!strcmp(key, "main_x")) ld->main_x = atoi(val);
                else if (!strcmp(key, "main_y")) ld->main_y = atoi(val);
                else if (!strcmp(key, "main_w")) ld->main_w = atoi(val);
                else if (!strcmp(key, "main_h")) ld->main_h = atoi(val);
                else if (!strcmp(key, "tree_w")) ld->tree_w = atoi(val);
                else if (!strcmp(key, "log_h")) ld->log_h = atoi(val);
                else if (!strcmp(key, "log_hidden")) g_app.log_hidden = atoi(val); /* F22 */
                else if (!strcmp(key, "active")) ld->active = atoi(val);
                else if (!strcmp(key, "theme")) os_theme_set_dark(atoi(val)); /* F20 */
            } else {
                if (!strcmp(key, "type")) _snprintf(cur->type, sizeof(cur->type), "%s", val);
                else if (!strcmp(key, "title")) _snprintf(cur->title, sizeof(cur->title), "%s", val);
                else if (!strcmp(key, "vars") && val[0]) {
                    if (cur->nvars < MAX_WIN_VARS)
                        _snprintf(cur->vars[cur->nvars++], sizeof(cur->vars[0]), "%s", val);
                } else if (key[0] == 'v' && strncmp(key, "vars+", 5) == 0) {
                    /* 兼容新格式 "vars+=name" 与旧格式 "vars+name"（无 '='） */
                    const char* nm = val[0] ? val : key + 5;
                    if (nm[0] && cur->nvars < MAX_WIN_VARS)
                        _snprintf(cur->vars[cur->nvars++], sizeof(cur->vars[0]), "%s", nm);
                }
            }
        }
    }
    fclose(f);

    /* 应用：主窗口尺寸/布局参数 */
    if (ld->main_w > 200 && ld->main_h > 120) {
        /* Bug2 修复：若保存位置在屏幕外（多显示器移除/窗口被拖出屏幕/最小化关闭
         * 保存的是 -32768 哨兵），直接把主窗口 SetWindowPos 到屏外或传 CW_USEDEFAULT
         * 都会让重开出现“任务栏有图标但窗口不可见”。只在坐标有效且与任一显示器
         * 相交时才恢复位置；否则保留 CreateWindow 的默认级联位置、仅恢复尺寸。 */
        if (ld->main_x >= 0 && ld->main_y >= 0) {
            RECT rc;
            rc.left = ld->main_x; rc.top = ld->main_y;
            rc.right = rc.left + ld->main_w; rc.bottom = rc.top + ld->main_h;
            if (MonitorFromRect(&rc, MONITOR_DEFAULTTONULL)) {
                SetWindowPos(g_app.hMain, NULL, ld->main_x, ld->main_y,
                             ld->main_w, ld->main_h, SWP_NOZORDER);
            } else {
                RECT cur;
                GetWindowRect(g_app.hMain, &cur);
                SetWindowPos(g_app.hMain, NULL, cur.left, cur.top,
                             ld->main_w, ld->main_h, SWP_NOZORDER);
                os_log(OS_LOG_INFO, "布局主窗口位置 (%d,%d) 在屏幕外，回退默认位置",
                       ld->main_x, ld->main_y);
            }
        } else {
            /* 最小化关闭哨兵或无效负坐标：只恢复尺寸，位置交给系统默认 */
            RECT cur;
            GetWindowRect(g_app.hMain, &cur);
            SetWindowPos(g_app.hMain, NULL, cur.left, cur.top,
                         ld->main_w, ld->main_h, SWP_NOZORDER);
        }
    }
    if (ld->tree_w >= 120) g_app.tree_w = ld->tree_w;
    if (ld->log_h >= 60) g_app.log_h = ld->log_h;
    os_mainwin_refresh_layout();

    /* 创建窗口并挂变量 */
    for (i = 0; i < ld->nwins; i++) {
        LayoutWin* w = &ld->wins[i];
        wchar_t wtitle[512];
        HWND h;
        int k;
        if (!w->type[0]) continue;
        os_utf8_to_wide_buf(w->title[0] ? w->title : w->type, wtitle, 512);
        h = os_win_create_by_type(w->type, wtitle);
        if (!h) continue;
        for (k = 0; k < w->nvars; k++) {
            if (w->vars[k][0]) os_layout_apply_var(h, w->vars[k]);
        }
    }
    if (ld->active >= 0 && ld->active < g_app.win_count)
        os_mainwin_select_tab(ld->active);
    os_log(OS_LOG_INFO, "布局已加载: %ls (%d 个窗口)", path, ld->nwins);
    free(ld);
    ok = 1;
    return ok ? 0 : -1;
}

void os_layout_restore_auto(void)
{
    wchar_t path[MAX_PATH];
    default_path(path, MAX_PATH, 0);
    if (_waccess(path, 0) == 0) os_layout_load_from(path);
}

/* ---------------- 变量挂接与延迟解析 ---------------- */

void os_layout_apply_var(HWND h, const char* name)
{
    int id;
    int i;
    OS_WinItem* wi = NULL;
    if (!h || !name || !name[0]) return;
    for (i = 0; i < g_app.win_count; i++) {
        if (g_app.wins[i].hwnd == h) { wi = &g_app.wins[i]; break; }
    }
    id = os_vartree_find_by_name(name);
    if (id < 0) {
        if (g_pending_count < MAX_PENDING) {
            g_pending[g_pending_count].hwnd = h;
            _snprintf(g_pending[g_pending_count].name, sizeof(g_pending[0].name),
                      "%s", name);
            g_pending_count++;
        }
        return;
    }
    if (wi && wi->is_module && wi->mod && wi->mod->api_version >= 2 && wi->mod->win_add_var) {
        wi->mod->win_add_var(wi->mod_ctx, h, id);
    } else if (os_chart_is(h)) {
        os_chart_add_var(h, id);
    } else if (os_num_is(h)) {
        os_num_add_var(h, id);
    }
}

void os_layout_apply_pending(void)
{
    int i, kept = 0;
    for (i = 0; i < g_pending_count; i++) {
        if (!IsWindow(g_pending[i].hwnd)) continue; /* 窗口已关：丢弃 */
        if (os_vartree_find_by_name(g_pending[i].name) < 0) {
            g_pending[kept++] = g_pending[i];        /* 仍未解析：保留 */
            continue;
        }
        os_layout_apply_var(g_pending[i].hwnd, g_pending[i].name);
    }
    g_pending_count = kept;
}
