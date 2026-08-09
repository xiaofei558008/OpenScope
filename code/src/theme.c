/*
 * OpenScope 界面主题（F20）：白色（默认）/黑色（参考 IAR / Notepad++ 深色）。
 *
 * 方案：
 *  1. 自定义绘制（主窗/分隔条/树/日志/波形/数值/对话框）全部经 os_theme() 取色，
 *     切换主题后 RedrawWindow 立即生效，完全可控。
 *  2. 标准控件（按钮/组合框/编辑框）依赖父窗口 WM_CTLCOLOR 返回主题画刷；
 *     标题栏/菜单/tab 等系统绘制部分叠加 DWM 沉浸式暗色 + SetPreferredAppMode。
 *  3. 持久化：layout.ini [layout] 的 theme 键（0=白色 1=黑色）。
 */
#include "theme.h"
#include <stdio.h>
#include <string.h>
#include <commctrl.h>

static int g_dark; /* 0=白色（默认） 1=黑色 */
static HWND g_main_hwnd; /* 主窗口句柄（mainwin.c WM_CREATE 时注册） */

/* 调色板：g_pal_light[TH_COUNT] / g_pal_dark[TH_COUNT] */
static const COLORREF g_pal_light[TH_COUNT] = {
    /* TH_BG */ RGB(240, 240, 240),
    /* TH_PANEL */ RGB(240, 240, 240),
    /* TH_TEXT */ RGB(0, 0, 0),
    /* TH_DIMTEXT */ RGB(120, 120, 120),
    /* TH_BORDER */ RGB(128, 128, 128),
    /* TH_EDIT_BG */ RGB(255, 255, 255),
    /* TH_EDIT_TEXT */ RGB(0, 0, 0),
    /* TH_TREE_BG */ RGB(255, 255, 255),
    /* TH_TREE_TEXT */ RGB(0, 0, 0),
    /* TH_TREE_SEL_BG */ RGB(0, 120, 215),
    /* TH_TREE_SEL_TEXT */ RGB(255, 255, 255),
    /* TH_TREE_LINE */ RGB(0, 0, 0),
    /* TH_LOG_BG */ RGB(255, 255, 255),
    /* TH_LOG_TEXT */ RGB(0, 0, 0),
    /* TH_LOG_GRID */ RGB(230, 230, 230),
    /* TH_TAB_BG */ RGB(240, 240, 240),
    /* TH_TAB_TEXT */ RGB(0, 0, 0),
    /* TH_STATUS_BG */ RGB(240, 240, 240),
    /* TH_STATUS_TEXT */ RGB(0, 0, 0),
    /* TH_CHART_PLOT_BG */ RGB(12, 12, 18),
    /* TH_CHART_GRID */ RGB(45, 45, 58),
    /* TH_CHART_AXIS */ RGB(150, 150, 160),
};

static const COLORREF g_pal_dark[TH_COUNT] = {
    /* TH_BG */ RGB(43, 43, 43),
    /* TH_PANEL */ RGB(50, 50, 52),
    /* TH_TEXT */ RGB(220, 220, 220),
    /* TH_DIMTEXT */ RGB(140, 140, 145),
    /* TH_BORDER */ RGB(80, 80, 85),
    /* TH_EDIT_BG */ RGB(30, 30, 32),
    /* TH_EDIT_TEXT */ RGB(220, 220, 220),
    /* TH_TREE_BG */ RGB(30, 30, 32),
    /* TH_TREE_TEXT */ RGB(215, 215, 215),
    /* TH_TREE_SEL_BG */ RGB(0, 90, 160),
    /* TH_TREE_SEL_TEXT */ RGB(255, 255, 255),
    /* TH_TREE_LINE */ RGB(90, 90, 95),
    /* TH_LOG_BG */ RGB(28, 28, 30),
    /* TH_LOG_TEXT */ RGB(205, 205, 205),
    /* TH_LOG_GRID */ RGB(60, 60, 64),
    /* TH_TAB_BG */ RGB(50, 50, 52),
    /* TH_TAB_TEXT */ RGB(210, 210, 210),
    /* TH_STATUS_BG */ RGB(50, 50, 52),
    /* TH_STATUS_TEXT */ RGB(210, 210, 210),
    /* TH_CHART_PLOT_BG */ RGB(12, 12, 18),
    /* TH_CHART_GRID */ RGB(45, 45, 58),
    /* TH_CHART_AXIS */ RGB(150, 150, 160),
};

/* 缓存的实心画刷（随主题切换重建；WM_CTLCOLOR 返回需要持久的画刷句柄） */
static HBRUSH g_brushes[TH_COUNT];

static void theme_rebuild_brushes(void)
{
    int i;
    for (i = 0; i < TH_COUNT; i++) {
        if (g_brushes[i]) DeleteObject(g_brushes[i]);
        g_brushes[i] = CreateSolidBrush(os_theme((OS_ThemeColor)i));
    }
}

void os_theme_set_main(HWND h)
{
    g_main_hwnd = h;
}

int os_theme_dark(void)
{
    return g_dark;
}

COLORREF os_theme(OS_ThemeColor id)
{
    if ((int)id < 0 || id >= TH_COUNT) id = TH_BG;
    return g_dark ? g_pal_dark[id] : g_pal_light[id];
}

HBRUSH os_theme_brush(OS_ThemeColor id)
{
    if ((int)id < 0 || id >= TH_COUNT) id = TH_BG;
    if (!g_brushes[id]) theme_rebuild_brushes();
    return g_brushes[id];
}

/* ---------------- 系统暗色（标题栏/菜单/tab 等系统绘制部分） ---------------- */

/* uxtheme.dll 未文档化接口（Windows 10 1809+） */
typedef int  (WINAPI* OS_Theme_SetPreferredAppMode)(int);      /* ordinal 135 */
typedef void (WINAPI* OS_Theme_RefreshPolicy)(void);           /* ordinal 104 */
typedef BOOL (WINAPI* OS_Theme_AllowDarkModeForWindow)(HWND, BOOL); /* ordinal 133 */
typedef void (WINAPI* OS_Theme_FlushMenuThemes)(void);         /* ordinal 136 */

#define OS_DARK_MODE_ALLOW 1
#define OS_DARK_MODE_FORCE 2

static int g_sys_dark_ok; /* uxtheme 接口是否可用 */

static void sys_dark_apply(int dark)
{
    static OS_Theme_SetPreferredAppMode set_mode;
    static OS_Theme_RefreshPolicy refresh;
    static OS_Theme_AllowDarkModeForWindow allow_dark;
    static OS_Theme_FlushMenuThemes flush_menu;
    static int inited;
    if (!inited) {
        /* 用 LoadLibraryW 而非 GetModuleHandleW：dwmapi/uxtheme 可能尚未加载进进程 */
        HMODULE ux = LoadLibraryW(L"uxtheme.dll");
        if (ux) {
            set_mode = (OS_Theme_SetPreferredAppMode)GetProcAddress(ux, (LPCSTR)135);
            refresh = (OS_Theme_RefreshPolicy)GetProcAddress(ux, (LPCSTR)104);
            allow_dark = (OS_Theme_AllowDarkModeForWindow)GetProcAddress(ux, (LPCSTR)133);
            flush_menu = (OS_Theme_FlushMenuThemes)GetProcAddress(ux, (LPCSTR)136);
        }
        g_sys_dark_ok = (set_mode != NULL) && (refresh != NULL);
        inited = 1;
    }
    if (g_sys_dark_ok) {
        set_mode(dark ? OS_DARK_MODE_FORCE : OS_DARK_MODE_ALLOW);
        refresh();
    }
    /* 菜单栏/弹出菜单暗色：窗口级声明 + 冲刷菜单主题（仅 FlushMenuThemes 后菜单才跟随） */
    if (g_main_hwnd && IsWindow(g_main_hwnd) && allow_dark)
        allow_dark(g_main_hwnd, dark ? TRUE : FALSE);
    if (flush_menu) flush_menu();
    /* DWM 沉浸式暗色标题栏/菜单（DWMWA_USE_IMMERSIVE_DARK_MODE=20，旧版=19） */
    if (g_main_hwnd && IsWindow(g_main_hwnd)) {
        static int attr;
        static int dwm_inited;
        if (!dwm_inited) {
            HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
            if (dwm) {
                HRESULT (WINAPI* fn)(HWND, int, const void*, DWORD) =
                    (HRESULT (WINAPI*)(HWND, int, const void*, DWORD))GetProcAddress(dwm, "DwmSetWindowAttribute");
                if (fn) {
                    BOOL ok = FALSE;
                    if (fn(g_main_hwnd, 20, &ok, sizeof(ok)) == 0) attr = 20;
                    else if (fn(g_main_hwnd, 19, &ok, sizeof(ok)) == 0) attr = 19;
                }
            }
            dwm_inited = 1;
        }
        if (attr) {
            BOOL v = dark ? TRUE : FALSE;
            HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
            if (dwm) {
                HRESULT (WINAPI* fn)(HWND, int, const void*, DWORD) =
                    (HRESULT (WINAPI*)(HWND, int, const void*, DWORD))GetProcAddress(dwm, "DwmSetWindowAttribute");
                if (fn) fn(g_main_hwnd, attr, &v, sizeof(v));
            }
        }
    }
    /* WM_THEMECHANGED 广播给全部子窗口，让标准控件按新模式重绘 */
    if (g_main_hwnd && IsWindow(g_main_hwnd)) {
        SendMessageW(g_main_hwnd, WM_THEMECHANGED, 0, 0);
        {
            HWND child = GetWindow(g_main_hwnd, GW_CHILD);
            while (child) {
                SendMessageW(child, WM_THEMECHANGED, 0, 0);
                child = GetWindow(child, GW_HWNDNEXT);
            }
        }
    }
}

/* ---------------- 列表列头（SysHeader32）自绘 ---------------- */

/* SetWindowTheme 对 SysHeader32 无效（Win11 上列头仍浅色），改为子类化自绘。
 * 一个 ListView 只有一个 header，重复调用按 hwnd 去重。 */
#define OS_THM_HDR_MAX 8
typedef struct { HWND hdr; WNDPROC old; } OS_ThmHdr;
static OS_ThmHdr g_thm_hdrs[OS_THM_HDR_MAX];
static int g_thm_hdr_count;

static LRESULT CALLBACK thm_hdr_proc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_ERASEBKGND:
        return 1; /* 背景交给 WM_PAINT */
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc;
        RECT rc;
        int n, i, x = 0;
        hdc = BeginPaint(h, &ps);
        GetClientRect(h, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_LOG_BG));
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, os_theme(TH_LOG_TEXT));
        SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
        n = Header_GetItemCount(h);
        for (i = 0; i < n; i++) {
            HDITEMW hi;
            wchar_t buf[256];
            memset(&hi, 0, sizeof(hi));
            hi.mask = HDI_TEXT | HDI_WIDTH;
            hi.pszText = buf;
            hi.cchTextMax = 256;
            if (Header_GetItem(h, i, &hi)) {
                RECT tr;
                tr.left = x + 4; tr.right = x + hi.cxy - 2;
                tr.top = rc.top; tr.bottom = rc.bottom;
                DrawTextW(hdc, buf, -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            {
                HPEN pen = CreatePen(PS_SOLID, 1, os_theme(TH_BORDER));
                HGDIOBJ op = SelectObject(hdc, pen);
                MoveToEx(hdc, x + hi.cxy - 1, rc.top + 2, NULL);
                LineTo(hdc, x + hi.cxy - 1, rc.bottom - 2);
                SelectObject(hdc, op);
                DeleteObject(pen);
            }
            x += hi.cxy;
        }
        EndPaint(h, &ps);
        return 0;
    }
    }
    {
        int i;
        for (i = 0; i < g_thm_hdr_count; i++)
            if (g_thm_hdrs[i].hdr == h) return CallWindowProcW(g_thm_hdrs[i].old, h, msg, wParam, lParam);
    }
    return DefWindowProcW(h, msg, wParam, lParam);
}

void os_theme_listview_header(HWND list)
{
    HWND hdr;
    int i;
    if (!list || !IsWindow(list)) return;
    hdr = (HWND)SendMessageW(list, LVM_GETHEADER, 0, 0);
    if (!hdr || !IsWindow(hdr)) return;
    for (i = 0; i < g_thm_hdr_count; i++)
        if (g_thm_hdrs[i].hdr == hdr) return; /* 已子类化 */
    if (g_thm_hdr_count >= OS_THM_HDR_MAX) return;
    g_thm_hdrs[g_thm_hdr_count].hdr = hdr;
    g_thm_hdrs[g_thm_hdr_count].old =
        (WNDPROC)SetWindowLongPtrW(hdr, GWLP_WNDPROC, (LONG_PTR)thm_hdr_proc);
    g_thm_hdr_count++;
    RedrawWindow(hdr, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
}

/* ---------------- 主题应用 ---------------- */

/* mainwin.c 定义：把树/日志/状态栏/数字窗口列表等控件颜色刷成当前主题 */
void os_mainwin_apply_theme(void);

void os_theme_apply(HWND hMain)
{
    if (!hMain) hMain = g_main_hwnd;
    sys_dark_apply(g_dark);
    if (hMain && IsWindow(hMain)) {
        RedrawWindow(hMain, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE | RDW_UPDATENOW);
    }
    os_mainwin_apply_theme();
}

void os_theme_set_dark(int on)
{
    if (g_dark == (on ? 1 : 0)) return;
    g_dark = on ? 1 : 0;
    theme_rebuild_brushes();
    os_theme_apply(g_main_hwnd);
}

/* ---------------- 持久化：layout.ini [layout] theme 键 ---------------- */

static void theme_default_path(wchar_t* out, int cap)
{
    wchar_t dir[MAX_PATH];
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", dir, MAX_PATH) > 0)
        _snwprintf(out, cap, L"%s\\OpenScope\\layout.ini", dir);
    else
        _snwprintf(out, cap, L"layout.ini");
}

void os_theme_load(void)
{
    wchar_t path[MAX_PATH];
    char line[512];
    FILE* f;
    theme_default_path(path, MAX_PATH);
    f = _wfopen(path, L"rb");
    if (!f) return;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "theme=", 6) == 0) {
            g_dark = atoi(line + 6) ? 1 : 0;
            break;
        }
    }
    fclose(f);
    theme_rebuild_brushes();
    /* F20: 必须在 CreateWindowW 之前设置，否则后续创建的公共控件（tab/状态栏/按钮）
     * 仍按系统浅色渲染（SetPreferredAppMode 只影响其后创建的窗口）。 */
    sys_dark_apply(g_dark);
}
