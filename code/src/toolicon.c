/*
 * 调试栏线框图标：统一 1.5px 描边、圆角几何，暗/亮主题自适应。
 * 视觉参考阿里巴巴 iconfont 线性风格（本机自绘，免外链资源）。
 */
#include "toolicon.h"
#include "theme.h"

#include <commctrl.h>
#include <string.h>
#include "dpi.h"
#pragma comment(lib, "gdiplus.lib")

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

static void draw_glyph_gdi(HDC hdc, RECT rc, int icon, COLORREF fg)
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


/* ---------------- UI P1: GDI+ flat API (declared here, C-callable) ---------------- */
typedef struct { UINT32 GdiplusVersion; void* DebugEventCallback; BOOL SuppressBackgroundThread; BOOL SuppressExternalCodecs; } OS_GpStartupInput;
typedef struct { float X, Y; } OS_GpPointF;
typedef void OS_GpGraphics; typedef void OS_GpPen; typedef void OS_GpBrush;
int  WINAPI GdiplusStartup(ULONG_PTR* token, const OS_GpStartupInput* in, void* out);
int  WINAPI GdipCreateFromHDC(HDC hdc, OS_GpGraphics** g);
int  WINAPI GdipDeleteGraphics(OS_GpGraphics* g);
int  WINAPI GdipSetSmoothingMode(OS_GpGraphics* g, int mode);
int  WINAPI GdipSetPixelOffsetMode(OS_GpGraphics* g, int mode);
int  WINAPI GdipCreatePen1(DWORD argb, float width, int unit, OS_GpPen** pen);
int  WINAPI GdipDeletePen(OS_GpPen* pen);
int  WINAPI GdipSetPenLineCap197819(OS_GpPen* pen, int start, int end, int dash);
int  WINAPI GdipSetPenLineJoin(OS_GpPen* pen, int join);
int  WINAPI GdipDrawLines(OS_GpGraphics* g, OS_GpPen* pen, const OS_GpPointF* pts, int n);
int  WINAPI GdipDrawPolygon(OS_GpGraphics* g, OS_GpPen* pen, const OS_GpPointF* pts, int n);
int  WINAPI GdipDrawEllipse(OS_GpGraphics* g, OS_GpPen* pen, float x, float y, float w, float h);
int  WINAPI GdipCreateSolidFill(DWORD argb, OS_GpBrush** br);
int  WINAPI GdipDeleteBrush(OS_GpBrush* br);
int  WINAPI GdipFillEllipse(OS_GpGraphics* g, OS_GpBrush* br, float x, float y, float w, float h);
int  WINAPI GdipFillPolygon2(OS_GpGraphics* g, OS_GpBrush* br, const OS_GpPointF* pts, int n);

static int g_gp_state; /* 0 untried, 1 ok, -1 failed */
static int gp_ready(void)
{
    if (!g_gp_state) {
        ULONG_PTR tok = 0;
        OS_GpStartupInput in = { 1, NULL, FALSE, FALSE };
        g_gp_state = (GdiplusStartup(&tok, &in, NULL) == 0) ? 1 : -1; /* once per process */
    }
    return g_gp_state > 0;
}

/* glyph program: op 'M' polyline start, 'L' point, 'Z' close (polygon), 'C' circle cx,cy,r, 'F' filled circle, 'E' end */
typedef struct { char op; float a, b, c; } GOp;
static const GOp G_RUN[]   = {{'M',7,5},{'L',18,12},{'L',7,19},{'Z'},{'E'}};
static const GOp G_HALT[]  = {{'M',8,6},{'L',8,18},{'M',16,6},{'L',16,18},{'E'}};
static const GOp G_OVER[]  = {{'M',4,13},{'L',8,8},{'L',14,7},{'L',19,10},{'M',19,5},{'L',19,10},{'L',14,10},{'C',12,19,1.8f},{'E'}};
static const GOp G_IN[]    = {{'M',12,3},{'L',12,14},{'M',7.5f,9.5f},{'L',12,14},{'L',16.5f,9.5f},{'C',12,19.5f,1.8f},{'E'}};
static const GOp G_OUT[]   = {{'M',12,15},{'L',12,4},{'M',7.5f,8.5f},{'L',12,4},{'L',16.5f,8.5f},{'C',12,19.5f,1.8f},{'E'}};
static const GOp G_RTC[]   = {{'M',4,6},{'L',11,12},{'L',4,18},{'Z'},{'M',15,5},{'L',15,19},{'M',13,5},{'L',17,5},{'M',13,19},{'L',17,19},{'E'}};
static const GOp G_RESET[] = {{'M',5,9},{'L',8,6},{'L',12,4.5f},{'L',16,6},{'L',19,10},{'L',19,14},{'L',16,18},{'L',12,19.5f},{'L',8,18},{'L',5,15},
                              {'M',4.5f,4},{'L',5,9},{'L',10,8.5f},{'F',12,12,1.5f},{'E'}};
static const GOp G_CONN[]  = {{'M',4,20},{'L',8,16},{'M',20,4},{'L',16,8},{'M',8,16},{'L',6,12},{'L',12,6},{'L',18,12},{'L',12,18},
                              {'M',9,9},{'L',11,11},{'M',13,13},{'L',15,15},{'E'}};
static const GOp G_DISC[]  = {{'M',4,20},{'L',7,17},{'M',20,4},{'L',17,7},{'M',7,17},{'L',5,13},{'L',9,9},{'M',17,7},{'L',19,11},{'L',15,15},
                              {'M',4,4},{'L',20,20},{'E'}};
static const GOp G_BP[]    = {{'C',12,12,6.5f},{'F',12,12,2.5f},{'E'}};

static const GOp* glyph_prog(int icon)
{
    switch (icon) {
    case TI_RUN: return G_RUN;
    case TI_STOP: case TI_HALT: return G_HALT;
    case TI_RESET_HALT: return G_RESET;
    case TI_STEP_OVER: return G_OVER;
    case TI_STEP_IN: return G_IN;
    case TI_STEP_OUT: return G_OUT;
    case TI_RUN_TO_CURSOR: return G_RTC;
    case TI_CONNECT: return G_CONN;
    case TI_DISCONNECT: return G_DISC;
    case TI_BP: return G_BP;
    default: return NULL; /* TI_STEP keeps legacy GDI glyph */
    }
}

COLORREF os_toolicon_color(int icon)
{
    switch (icon) {
    case TI_RUN: case TI_RUN_TO_CURSOR: case TI_CONNECT: return RGB(0x3f, 0xb9, 0x50);
    case TI_STOP: case TI_HALT: return RGB(0xe3, 0xb3, 0x41);
    case TI_STEP: case TI_STEP_IN: case TI_STEP_OUT: case TI_STEP_OVER: return RGB(0x58, 0xa6, 0xff);
    case TI_RESET_HALT: return RGB(0xff, 0x7b, 0x72);
    case TI_DISCONNECT: return RGB(0x8b, 0x94, 0x9e);
    case TI_BP: return RGB(0xf8, 0x51, 0x49);
    default: return RGB(0xc9, 0xd1, 0xd9);
    }
}

static DWORD argb_of(COLORREF c) { return 0xFF000000u | ((DWORD)GetRValue(c) << 16) | ((DWORD)GetGValue(c) << 8) | GetBValue(c); }

static COLORREF lighten(COLORREF c)
{
    return RGB(GetRValue(c) + (255 - GetRValue(c)) / 4, GetGValue(c) + (255 - GetGValue(c)) / 4, GetBValue(c) + (255 - GetBValue(c)) / 4);
}

int os_toolicon_paint(HDC hdc, RECT rc, int icon, int state)
{
    const GOp* p = glyph_prog(icon);
    OS_GpGraphics* g = NULL;
    OS_GpPen* pen = NULL;
    OS_GpBrush* br = NULL;
    OS_GpPointF pts[16];
    COLORREF col;
    float box, sc, ox, oy;
    int n = 0, closed = 0;
    if (!p || !gp_ready()) return 0;
    if (GdipCreateFromHDC(hdc, &g) != 0) return 0;
    col = state == 2 ? RGB(0x48, 0x4f, 0x58) : state == 1 ? lighten(os_toolicon_color(icon)) : os_toolicon_color(icon);
    box = (float)min(rc.right - rc.left, rc.bottom - rc.top) * 0.82f;
    sc = box / 24.0f;
    ox = rc.left + ((rc.right - rc.left) - box) / 2.0f;
    oy = rc.top + ((rc.bottom - rc.top) - box) / 2.0f;
    GdipSetSmoothingMode(g, 4 /* AntiAlias */);
    GdipSetPixelOffsetMode(g, 2 /* HighQuality */);
    GdipCreatePen1(argb_of(col), 1.75f * sc < 1.0f ? 1.0f : 1.75f * sc, 2 /* UnitPixel */, &pen);
    GdipSetPenLineCap197819(pen, 2, 2, 2 /* Round */);
    GdipSetPenLineJoin(pen, 2 /* Round */);
    GdipCreateSolidFill(argb_of(col), &br);
#define FLUSH() do { if (n >= 2) { if (closed) GdipDrawPolygon(g, pen, pts, n); else GdipDrawLines(g, pen, pts, n); } n = 0; closed = 0; } while (0)
    for (; p->op != 'E'; p++) {
        switch (p->op) {
        case 'M': FLUSH(); /* fallthrough */
        case 'L': if (n < 16) { pts[n].X = ox + p->a * sc; pts[n].Y = oy + p->b * sc; n++; } break;
        case 'Z': closed = 1; FLUSH(); break;
        case 'C': FLUSH(); GdipDrawEllipse(g, pen, ox + (p->a - p->c) * sc, oy + (p->b - p->c) * sc, 2 * p->c * sc, 2 * p->c * sc); break;
        case 'F': FLUSH(); GdipFillEllipse(g, br, ox + (p->a - p->c) * sc, oy + (p->b - p->c) * sc, 2 * p->c * sc, 2 * p->c * sc); break;
        }
    }
    FLUSH();
#undef FLUSH
    GdipDeleteBrush(br);
    GdipDeletePen(pen);
    GdipDeleteGraphics(g);
    return 1;
}

/* hover tracking for owner-draw debug buttons: GWLP_USERDATA bit0 = hot */
static LRESULT CALLBACK btn_hover_proc(HWND h, UINT m, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR ref)
{
    (void)id; (void)ref;
    if (m == WM_MOUSEMOVE && !(GetWindowLongPtrW(h, GWLP_USERDATA) & 1)) {
        TRACKMOUSEEVENT t = { sizeof(t), TME_LEAVE, h, 0 };
        TrackMouseEvent(&t);
        SetWindowLongPtrW(h, GWLP_USERDATA, 1);
        InvalidateRect(h, NULL, FALSE);
    } else if (m == WM_MOUSELEAVE) {
        SetWindowLongPtrW(h, GWLP_USERDATA, 0);
        InvalidateRect(h, NULL, FALSE);
    } else if (m == WM_NCDESTROY) {
        RemoveWindowSubclass(h, btn_hover_proc, 1);
    }
    return DefSubclassProc(h, m, wp, lp);
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
    hot = 0; /* UI P1: hover tracked by subclass (GWLP_USERDATA bit0), focus no longer = hover */
    (void)hot;
    bg = os_theme(TH_PANEL);
    fg = disabled ? os_theme(TH_DIMTEXT) : os_theme(TH_TEXT);
    {
        HBRUSH br = CreateSolidBrush(bg);
        FillRect(dis->hDC, &rc, br);
        DeleteObject(br);
    }
    hot = (GetWindowLongPtrW(dis->hwndItem, GWLP_USERDATA) & 1) != 0;
    if (pushed || hot) {
        RECT ir = rc;
        InflateRect(&ir, -1, -1);
        FrameRect(dis->hDC, &ir, os_theme_brush(TH_BORDER));
        if (pushed) OffsetRect(&rc, 1, 1);
    }
    {
        int st = disabled ? 2 : (hot || (GetWindowLongPtrW(dis->hwndItem, GWLP_USERDATA) & 1)) ? 1 : 0;
        if (!os_toolicon_paint(dis->hDC, rc, icon, st)) draw_glyph_gdi(dis->hDC, rc, icon, fg);
    }
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
    SetWindowSubclass(b, btn_hover_proc, 1, 0); /* UI P1 hover */
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
