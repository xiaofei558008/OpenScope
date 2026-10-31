/* netbar.c - see netbar.h. Encryption is shown as "Unencrypted LAN" (amber) until TLS exists. */
#include "netbar.h"
#include "theme.h"
#include "dpi.h"
#include "osfont.h"
#include <stdio.h>
#include <wchar.h>

#define NB_CLASS L"OSNetBar"
static HWND g_nb;
static OS_NetBarState g_st = NB_OFF;
static wchar_t g_role[32] = L"LOCAL", g_peer[128], g_lock[64], g_sum[160];
static int g_rtt = -1;

static COLORREF st_color(void)
{
    switch (g_st) {
    case NB_OK: return os_theme(TH_OK);
    case NB_SLOW: return os_theme(TH_WARN);
    case NB_LOST: return os_theme(TH_ERROR);
    default: return os_theme(TH_DIMTEXT);
    }
}

static int seg(HDC dc, int x, int h, const wchar_t* t, COLORREF c)
{
    RECT r = { x, 0, x + 2000, h };
    SIZE sz;
    SetTextColor(dc, c);
    DrawTextW(dc, t, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    GetTextExtentPoint32W(dc, t, (int)wcslen(t), &sz);
    return x + sz.cx + os_px(14);
}

static void nb_paint(HWND hwnd, HDC dc)
{
    RECT rc;
    HDC mem;
    HBITMAP bmp, old;
    wchar_t t[160];
    int x, h, d;
    HBRUSH b;
    GetClientRect(hwnd, &rc);
    if (rc.right <= 0 || rc.bottom <= 0) return;
    mem = CreateCompatibleDC(dc);
    bmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
    old = (HBITMAP)SelectObject(mem, bmp);
    FillRect(mem, &rc, os_theme_brush(os_theme_dark() ? TH_NET_BG : TH_NET_BG));
    SelectObject(mem, os_font_ui());
    SetBkMode(mem, TRANSPARENT);
    h = rc.bottom;
    d = os_px(8);
    x = os_px(8);
    b = CreateSolidBrush(st_color());
    {
        HGDIOBJ ob = SelectObject(mem, b), op = SelectObject(mem, GetStockObject(NULL_PEN));
        Ellipse(mem, x, (h - d) / 2, x + d, (h - d) / 2 + d);
        SelectObject(mem, ob); SelectObject(mem, op);
    }
    DeleteObject(b);
    x += d + os_px(8);
    _snwprintf(t, 160, L"%s%s", g_role, wcscmp(g_role, L"LOCAL") == 0 ? L" (controlled)" : L" (controller)");
    x = seg(mem, x, h, t, os_theme(TH_NET_TEXT));
    x = seg(mem, x, h, g_peer[0] ? g_peer : (g_st == NB_OFF ? L"network off" : L"-"), os_theme(TH_TEXT));
    if (g_rtt >= 0) _snwprintf(t, 160, L"RTT %d ms", g_rtt); else wcscpy(t, L"RTT -");
    x = seg(mem, x, h, t, g_rtt > 100 ? os_theme(TH_WARN) : os_theme(TH_DIMTEXT));
    x = seg(mem, x, h, L"Unencrypted LAN", os_theme(TH_WARN));
    _snwprintf(t, 160, L"input lock: %s", g_lock[0] ? g_lock : L"local");
    seg(mem, x, h, t, os_theme(TH_DIMTEXT));
    BitBlt(dc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
    SelectObject(mem, old);
    DeleteObject(bmp);
    DeleteDC(mem);
}

static LRESULT CALLBACK nb_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        nb_paint(hwnd, dc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

HWND os_netbar_create(HWND parent)
{
    static int reg;
    if (!reg) {
        WNDCLASSW wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.lpfnWndProc = nb_proc;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.lpszClassName = NB_CLASS;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);
        reg = 1;
    }
    g_nb = CreateWindowW(NB_CLASS, L"", WS_CHILD | WS_VISIBLE, 0, 0, 100, 26, parent, NULL, GetModuleHandleW(NULL), NULL);
    os_netbar_set(NB_OFF, L"LOCAL", NULL, -1, NULL);
    return g_nb;
}

HWND os_netbar_hwnd(void) { return g_nb; }
int  os_netbar_height(void) { return g_nb ? os_px(26) : 0; }

void os_netbar_set(OS_NetBarState st, const wchar_t* role, const wchar_t* peer, int rtt_ms, const wchar_t* lock)
{
    g_st = st;
    _snwprintf(g_role, 32, L"%s", role ? role : L"LOCAL"); g_role[31] = 0;
    _snwprintf(g_peer, 128, L"%s", peer ? peer : L""); g_peer[127] = 0;
    _snwprintf(g_lock, 64, L"%s", lock ? lock : L""); g_lock[63] = 0;
    g_rtt = rtt_ms;
    _snwprintf(g_sum, 160, L"Net:%s%s%s", st == NB_OFF ? L"off" : st == NB_LOST ? L"lost" : g_role,
               g_peer[0] ? L" " : L"", g_peer);
    g_sum[159] = 0;
    if (g_nb) InvalidateRect(g_nb, NULL, FALSE);
}

const wchar_t* os_netbar_summary(void) { return g_sum; }