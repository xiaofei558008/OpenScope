#include "app.h"
#include "helpwin.h"
#include "theme.h"
#include "util.h"
#include <string.h>

/* 需求12：帮助文档（F1）。readme.md 经 version.rc 以 RCDATA 内嵌进 exe，
 * 运行时取出 UTF-8 文本转宽字符（\n -> \r\n）填入只读多行 EDIT。
 * 内嵌资源保证安装版脱离源码目录也能显示帮助。 */

#define OS_HELP_CLASS L"OSHelpWin"
#define IDC_HELP_EDIT 4101

static HWND g_help; /* 单例：重复 F1 只前置，不开多窗 */

/* 从 exe 资源读出 readme 文本并转为 EDIT 可用的宽字符（CRLF 换行）。
 * 返回 NULL 表示资源缺失（不应发生；version.rc 内嵌 readme.md）。 */
static wchar_t* help_load_text(void)
{
    HRSRC hr;
    HGLOBAL hg;
    const char* bytes;
    DWORD sz, i, wlen;
    wchar_t* w;
    wchar_t* out;
    const wchar_t* p;
    wchar_t* q;
    hr = FindResourceW(g_app.hInst, MAKEINTRESOURCEW(IDR_HELP_MD), RT_RCDATA);
    if (!hr) return NULL;
    sz = SizeofResource(g_app.hInst, hr);
    hg = LoadResource(g_app.hInst, hr);
    if (!hg || !sz) return NULL;
    bytes = (const char*)LockResource(hg);
    if (!bytes) return NULL;
    /* 跳过 UTF-8 BOM */
    if (sz >= 3 && (uint8_t)bytes[0] == 0xEF && (uint8_t)bytes[1] == 0xBB &&
        (uint8_t)bytes[2] == 0xBF) {
        bytes += 3;
        sz -= 3;
    }
    wlen = (DWORD)MultiByteToWideChar(CP_UTF8, 0, bytes, (int)sz, NULL, 0);
    if (!wlen) return NULL;
    w = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
    if (!w) return NULL;
    MultiByteToWideChar(CP_UTF8, 0, bytes, (int)sz, w, (int)wlen);
    w[wlen] = 0;
    /* EDIT 控件需要 CRLF：统计额外字符后二次展开 */
    {
        DWORD extra = 0;
        for (i = 0; i < wlen; i++)
            if (w[i] == L'\n' && (i == 0 || w[i - 1] != L'\r')) extra++;
        out = (wchar_t*)malloc((wlen + extra + 1) * sizeof(wchar_t));
        if (!out) { free(w); return NULL; }
        for (p = w, q = out; *p; p++) {
            if (*p == L'\n' && (p == w || p[-1] != L'\r')) *q++ = L'\r';
            *q++ = *p;
        }
        *q = 0;
    }
    free(w);
    return out;
}

static LRESULT CALLBACK help_proc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE: {
        wchar_t* text = help_load_text();
        HWND ed = CreateWindowExW(0, L"EDIT", L"",
                                  WS_CHILD | WS_VISIBLE | WS_VSCROLL |
                                  ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_WANTRETURN,
                                  0, 0, 100, 100, h, (HMENU)(INT_PTR)IDC_HELP_EDIT,
                                  g_app.hInst, NULL);
        SendMessageW(ed, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        if (text) {
            SetWindowTextW(ed, text);
            free(text);
        } else {
            SetWindowTextW(ed, L"（帮助资源缺失：请确认安装包完整）");
        }
        return 0;
    }
    case WM_SIZE: {
        HWND ed = GetDlgItem(h, IDC_HELP_EDIT);
        if (ed) MoveWindow(ed, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;
    }
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(h, IDC_HELP_EDIT));
        return 0;
    case WM_CTLCOLOREDIT: {
        /* 跟随 F20 主题（白/黑） */
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, os_theme(TH_EDIT_TEXT));
        SetBkColor(hdc, os_theme(TH_EDIT_BG));
        return (LRESULT)os_theme_brush(TH_EDIT_BG);
    }
    case WM_ERASEBKGND: {
        RECT rc;
        GetClientRect(h, &rc);
        FillRect((HDC)wParam, &rc, os_theme_brush(TH_BG));
        return 1;
    }
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE || wParam == VK_F1) { /* F1 再按关闭 */
            DestroyWindow(h);
            return 0;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(h);
        return 0;
    case WM_DESTROY:
        g_help = NULL;
        return 0;
    }
    return DefWindowProcW(h, msg, wParam, lParam);
}

void os_help_register(void)
{
    WNDCLASSEXW wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = help_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(g_app.hInst, MAKEINTRESOURCEW(IDI_APP));
    wc.lpszClassName = OS_HELP_CLASS;
    RegisterClassExW(&wc);
}

void os_help_show(HWND parent)
{
    RECT pr;
    int w = 760, h = 580, x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    if (g_help && IsWindow(g_help)) {
        SetForegroundWindow(g_help);
        return;
    }
    if (parent && GetWindowRect(parent, &pr)) {
        x = pr.left + (pr.right - pr.left - w) / 2;
        y = pr.top + (pr.bottom - pr.top - h) / 2;
        if (x < 0) x = 0;
        if (y < 0) y = 0;
    }
    g_help = CreateWindowExW(WS_EX_DLGMODALFRAME, OS_HELP_CLASS,
                             L"OpenScope 帮助 — 用户说明书",
                             WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
                             WS_MINIMIZEBOX | WS_VISIBLE | WS_CLIPCHILDREN,
                             x, y, w, h, parent, NULL, g_app.hInst, NULL);
    if (g_help) {
        HICON hi = LoadIconW(g_app.hInst, MAKEINTRESOURCEW(IDI_APP));
        if (hi) {
            SendMessageW(g_help, WM_SETICON, ICON_BIG, (LPARAM)hi);
            SendMessageW(g_help, WM_SETICON, ICON_SMALL, (LPARAM)hi);
        }
        ShowWindow(g_help, SW_SHOW);
        SetForegroundWindow(g_help);
    }
    os_log(OS_LOG_INFO, "帮助文档已打开 (F1)");
}
