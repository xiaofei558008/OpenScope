/*
 * 调试栏线框图标：统一 1.5px 描边、圆角几何，暗/亮主题自适应。
 * 视觉参考阿里巴巴 iconfont 线性风格（本机自绘，免外链资源）。
 */
#include "toolicon.h"
#include "theme.h"

#include <commctrl.h>
#include <string.h>

/* 与 mainwin.c 中 IDC_BTN_CPU_* 保持一致 */
#define IDC_BTN_CPU_RUN      2017
#define IDC_BTN_CPU_STOP     2018
#define IDC_BTN_CPU_RHALT    2019
#define IDC_BTN_CPU_STEP     2020
#define IDC_BTN_CPU_STEPIN   2041
#define IDC_BTN_CPU_STEPOUT  2042
#define IDC_BTN_CPU_STEPOVER 2043
#define IDC_BTN_BP           2044

int os_toolicon_from_ctrl(int ctrl_id)
{
    switch (ctrl_id) {
    case IDC_BTN_CPU_RUN: return TI_RUN;
    case IDC_BTN_CPU_STOP: return TI_STOP;
    case IDC_BTN_CPU_RHALT: return TI_RESET_HALT;
    case IDC_BTN_CPU_STEP: return TI_STEP;
    case IDC_BTN_CPU_STEPIN: return TI_STEP_IN;
    case IDC_BTN_CPU_STEPOUT: return TI_STEP_OUT;
    case IDC_BTN_CPU_STEPOVER: return TI_STEP_OVER;
    case IDC_BTN_BP: return TI_BP;
    default: return -1;
    }
}

static void stroke_line(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c, int w)
{
    HPEN pen = CreatePen(PS_SOLID, w > 0 ? w : 2, c);
    HPEN old = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    SelectObject(hdc, old);
    DeleteObject(pen);
}

static void stroke_poly(HDC hdc, POINT* pts, int n, COLORREF c, int w, int fill)
{
    HPEN pen = CreatePen(PS_SOLID, w > 0 ? w : 2, c);
    HBRUSH br = fill ? CreateSolidBrush(c) : (HBRUSH)GetStockObject(NULL_BRUSH);
    HPEN oldp = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldb = (HBRUSH)SelectObject(hdc, br);
    Polygon(hdc, pts, n);
    SelectObject(hdc, oldp);
    SelectObject(hdc, oldb);
    DeleteObject(pen);
    if (fill) DeleteObject(br);
}

static void stroke_ellipse(HDC hdc, int l, int t, int r, int b, COLORREF c, int w)
{
    HPEN pen = CreatePen(PS_SOLID, w > 0 ? w : 2, c);
    HPEN oldp = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldb = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Ellipse(hdc, l, t, r, b);
    SelectObject(hdc, oldp);
    SelectObject(hdc, oldb);
    DeleteObject(pen);
}

static void draw_glyph(HDC hdc, RECT rc, int icon, COLORREF fg)
{
    int cx = (rc.left + rc.right) / 2;
    int cy = (rc.top + rc.bottom) / 2;
    int s = (rc.right - rc.left) < (rc.bottom - rc.top)
            ? (rc.right - rc.left) : (rc.bottom - rc.top);
    int u = s / 5; /* 单位格 */
    if (u < 2) u = 2;

    switch (icon) {
    case TI_RUN: { /* 播放三角 */
        POINT p[3];
        p[0].x = cx - u; p[0].y = cy - u * 3 / 2;
        p[1].x = cx - u; p[1].y = cy + u * 3 / 2;
        p[2].x = cx + u * 3 / 2; p[2].y = cy;
        stroke_poly(hdc, p, 3, fg, 2, 1);
        break;
    }
    case TI_STOP: { /* 方块停止 */
        RECT r = { cx - u, cy - u, cx + u, cy + u };
        HBRUSH br = CreateSolidBrush(fg);
        FillRect(hdc, &r, br);
        DeleteObject(br);
        break;
    }
    case TI_RESET_HALT: { /* 复位：圆 + 箭头 */
        stroke_ellipse(hdc, cx - u * 3 / 2, cy - u * 3 / 2,
                       cx + u * 3 / 2, cy + u * 3 / 2, fg, 2);
        stroke_line(hdc, cx, cy - u, cx + u, cy - u / 2, fg, 2);
        stroke_line(hdc, cx, cy - u, cx, cy + u / 2, fg, 2);
        break;
    }
    case TI_STEP: { /* 单步：竖线 + 三角 */
        POINT p[3];
        stroke_line(hdc, cx - u, cy - u * 3 / 2, cx - u, cy + u * 3 / 2, fg, 2);
        p[0].x = cx - u / 2; p[0].y = cy - u;
        p[1].x = cx - u / 2; p[1].y = cy + u;
        p[2].x = cx + u; p[2].y = cy;
        stroke_poly(hdc, p, 3, fg, 2, 1);
        break;
    }
    case TI_STEP_IN: { /* 向下箭头入 */
        stroke_line(hdc, cx, cy - u * 3 / 2, cx, cy + u / 2, fg, 2);
        stroke_line(hdc, cx - u, cy, cx, cy + u, fg, 2);
        stroke_line(hdc, cx + u, cy, cx, cy + u, fg, 2);
        stroke_line(hdc, cx - u, cy + u * 3 / 2, cx + u, cy + u * 3 / 2, fg, 2);
        break;
    }
    case TI_STEP_OUT: { /* 向上箭头出 */
        stroke_line(hdc, cx, cy + u * 3 / 2, cx, cy - u / 2, fg, 2);
        stroke_line(hdc, cx - u, cy, cx, cy - u, fg, 2);
        stroke_line(hdc, cx + u, cy, cx, cy - u, fg, 2);
        stroke_line(hdc, cx - u, cy - u * 3 / 2, cx + u, cy - u * 3 / 2, fg, 2);
        break;
    }
    case TI_STEP_OVER: { /* 弧线跨过 */
        stroke_ellipse(hdc, cx - u * 3 / 2, cy - u, cx + u * 3 / 2, cy + u * 2, fg, 2);
        /* 遮住下半圆 */
        {
            RECT cover = { cx - u * 2, cy + u / 3, cx + u * 2, cy + u * 2 };
            FillRect(hdc, &cover, os_theme_brush(TH_PANEL));
        }
        stroke_line(hdc, cx + u / 2, cy - u / 2, cx + u * 3 / 2, cy - u / 2, fg, 2);
        stroke_line(hdc, cx + u, cy - u, cx + u * 3 / 2, cy - u / 2, fg, 2);
        stroke_line(hdc, cx + u, cy, cx + u * 3 / 2, cy - u / 2, fg, 2);
        break;
    }
    case TI_BP: { /* 断点圆点 */
        HBRUSH br = CreateSolidBrush(RGB(220, 64, 64));
        HBRUSH old = (HBRUSH)SelectObject(hdc, br);
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(180, 40, 40));
        HPEN oldp = (HPEN)SelectObject(hdc, pen);
        Ellipse(hdc, cx - u, cy - u, cx + u, cy + u);
        SelectObject(hdc, old);
        SelectObject(hdc, oldp);
        DeleteObject(br);
        DeleteObject(pen);
        break;
    }
    default:
        break;
    }
}

int os_toolicon_draw(const DRAWITEMSTRUCT* dis)
{
    int icon;
    RECT rc;
    COLORREF fg, bg;
    int disabled, hot, pushed;
    if (!dis || dis->CtlType != ODT_BUTTON) return 0;
    icon = os_toolicon_from_ctrl((int)dis->CtlID);
    if (icon < 0) return 0;
    rc = dis->rcItem;
    disabled = (dis->itemState & ODS_DISABLED) != 0;
    pushed = (dis->itemState & ODS_SELECTED) != 0;
    hot = (dis->itemState & ODS_FOCUS) != 0;
    bg = os_theme(TH_PANEL);
    fg = disabled ? os_theme(TH_DIMTEXT) : os_theme(TH_TEXT);
    {
        HBRUSH br = CreateSolidBrush(bg);
        FillRect(dis->hDC, &rc, br);
        DeleteObject(br);
    }
    if (pushed || hot) {
        RECT ir = rc;
        InflateRect(&ir, -1, -1);
        FrameRect(dis->hDC, &ir, os_theme_brush(TH_BORDER));
        if (pushed) OffsetRect(&rc, 1, 1);
    }
    draw_glyph(dis->hDC, rc, icon, fg);
    return 1;
}

HWND os_toolicon_create_btn(HWND parent, int ctrl_id, int x, int y, int w, int h,
                            const wchar_t* tip)
{
    static HWND tip_hwnd;
    HWND b = CreateWindowW(L"BUTTON", tip ? tip : L"",
                           WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                           x, y, w, h, parent, (HMENU)(INT_PTR)ctrl_id,
                           GetModuleHandleW(NULL), NULL);
    if (!b) return NULL;
    if (tip && tip[0]) {
        TOOLINFOW ti;
        if (!tip_hwnd) {
            tip_hwnd = CreateWindowExW(0, TOOLTIPS_CLASSW, NULL,
                                       WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
                                       0, 0, 0, 0, parent, NULL,
                                       GetModuleHandleW(NULL), NULL);
            if (tip_hwnd)
                SetWindowPos(tip_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        if (tip_hwnd) {
            memset(&ti, 0, sizeof(ti));
            ti.cbSize = sizeof(ti);
            ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
            ti.hwnd = parent;
            ti.uId = (UINT_PTR)b;
            ti.lpszText = (LPWSTR)tip;
            SendMessageW(tip_hwnd, TTM_ADDTOOLW, 0, (LPARAM)&ti);
        }
    }
    return b;
}
