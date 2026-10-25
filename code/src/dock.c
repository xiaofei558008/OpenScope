/*
 * dock —— Terminator 式平铺停靠管理器 Win32 外壳（见 dock.h）
 */
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dock.h"
#include "dock_tree.h"
#include "theme.h"

#define DK_MAX_PANELS 64
#define DK_MAX_HOSTS  17          /* 0 = 主宿主，1.. = 浮动窗 */
#define DK_CAP_H      22          /* 窗格标题栏（tab 条）高度 */
#define DK_GAP        5
#define DK_MIN        80
#define DK_TAB_MAXW   170
#define DK_CLOSE_W    20
#define DK_DRAG_THRESH 6
#define DK_EDGE_FRAC  0.25
#define DK_THROTTLE_MS 33          /* 分隔条拖动中重排最多 ~30Hz */
#define DK_TIMER_FLUSH 0xD0C1

typedef struct {
    int     used;
    HWND    hwnd;
    wchar_t title[128];
} DkPanel;

typedef struct {
    int     used;
    HWND    frame;                 /* 浮动窗（主宿主为 NULL） */
    HWND    host;                  /* 停靠宿主客户窗 */
    DtTree  tree;
    int     focus_leaf;
} DkHost;

enum { DRAG_NONE = 0, DRAG_SPLIT, DRAG_TAB_PENDING, DRAG_TAB };

static DkPanel g_p[DK_MAX_PANELS];
static DkHost  g_h[DK_MAX_HOSTS];
static DockCallbacks g_cb;
static HWND    g_parent, g_overlay, g_excluded;
static int     g_active_panel = -1;
static int     g_registered;

static struct {
    int   mode;
    int   host;              /* 发起宿主 */
    int   split;             /* DRAG_SPLIT */
    int   panel;             /* DRAG_TAB* */
    POINT start;             /* 屏幕坐标 */
    DWORD last_tick;
    int   pending;
    /* 当前落点 */
    int   t_host, t_leaf, t_zone;
} g_drag;

/* 浮动窗口拖动（标题栏）回停靠 */
static struct { int host, moved, t_host, t_leaf, t_zone; } g_fmove = { -1 };

static const wchar_t* HOST_CLASS = L"OSDockHost";
static const wchar_t* FLOAT_CLASS = L"OSDockFloat";
static const wchar_t* OVL_CLASS = L"OSDockOverlay";

/* ------------------------------------------------------------------ */
static int panel_of_hwnd(HWND h)
{
    int i;
    if (!h) return -1;
    for (i = 0; i < DK_MAX_PANELS; i++) if (g_p[i].used && g_p[i].hwnd == h) return i;
    return -1;
}

static int host_of_panel(int id)
{
    int i;
    for (i = 0; i < DK_MAX_HOSTS; i++) if (g_h[i].used && dt_find_leaf(&g_h[i].tree, id) >= 0) return i;
    return -1;
}

static int host_of_hwnd(HWND h)
{
    int i;
    for (i = 0; i < DK_MAX_HOSTS; i++) if (g_h[i].used && (g_h[i].host == h || g_h[i].frame == h)) return i;
    return -1;
}

static DtRect to_dt(RECT r) { DtRect d; d.l = r.left; d.t = r.top; d.r = r.right; d.b = r.bottom; return d; }
static RECT to_rc(DtRect d) { RECT r; r.left = d.l; r.top = d.t; r.right = d.r; r.bottom = d.b; return r; }

/* 窗格内容区（去掉标题栏） */
static RECT leaf_body(const DtNode* L)
{
    RECT r = to_rc(L->rc);
    r.top += DK_CAP_H;
    if (r.bottom < r.top) r.bottom = r.top;
    return r;
}

/* tab 矩形（宿主客户坐标）；返回 tab 数；close 输出关闭按钮矩形 */
static int leaf_tabs(const DtNode* L, RECT* tabs, int max, RECT* close)
{
    int n = L->ntabs, k, w, x;
    int avail = (L->rc.r - L->rc.l) - DK_CLOSE_W - 4;
    if (n <= 0) return 0;
    w = avail / n;
    if (w > DK_TAB_MAXW) w = DK_TAB_MAXW;
    if (w < 24) w = 24;
    x = L->rc.l + 2;
    for (k = 0; k < n && k < max; k++) {
        tabs[k].left = x; tabs[k].right = x + w - 2;
        tabs[k].top = L->rc.t + 2; tabs[k].bottom = L->rc.t + DK_CAP_H;
        x += w;
    }
    if (close) {
        close->right = L->rc.r - 2; close->left = close->right - DK_CLOSE_W + 2;
        close->top = L->rc.t + 2; close->bottom = L->rc.t + DK_CAP_H - 2;
    }
    return n;
}

/* ------------------------------------------------------------------ */
static void host_layout(int hi)
{
    DkHost* H = &g_h[hi];
    RECT rc;
    int i, n = 0;
    HDWP dw;
    if (!H->used || !H->host) return;
    GetClientRect(H->host, &rc);
    dt_layout(&H->tree, to_dt(rc));
    for (i = 0; i < DK_MAX_PANELS; i++) if (g_p[i].used) n++;
    dw = BeginDeferWindowPos(n > 0 ? n : 1);
    for (i = 0; i < DK_MAX_PANELS; i++) {
        int leaf;
        DkPanel* P = &g_p[i];
        if (!P->used || !IsWindow(P->hwnd) || P->hwnd == g_excluded) continue;
        leaf = dt_find_leaf(&H->tree, i);
        if (leaf < 0) continue;
        if (GetParent(P->hwnd) != H->host) SetParent(P->hwnd, H->host);
        if (dt_active_id(&H->tree, leaf) == i) {
            RECT b = leaf_body(&H->tree.n[leaf]);
            if (dw) dw = DeferWindowPos(dw, P->hwnd, NULL, b.left, b.top, b.right - b.left, b.bottom - b.top,
                                       SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        } else {
            if (dw) dw = DeferWindowPos(dw, P->hwnd, NULL, 0, 0, 0, 0,
                                       SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
        }
    }
    if (dw) EndDeferWindowPos(dw);
    InvalidateRect(H->host, NULL, FALSE);   /* 宿主 WS_CLIPCHILDREN：只重画标题栏/分隔条 */
    if (H->frame) {
        /* 浮动窗标题 = 各面板标题 */
        wchar_t t[256] = L"";
        int ids[DT_MAX_TABS], c = dt_list(&H->tree, ids, DT_MAX_TABS), k;
        for (k = 0; k < c && k < DT_MAX_TABS; k++) {
            if (k) wcsncat(t, L" | ", 255 - wcslen(t));
            wcsncat(t, g_p[ids[k]].title, 255 - wcslen(t));
        }
        SetWindowTextW(H->frame, t[0] ? t : L"OpenScope");
    }
}

void dock_relayout(void)
{
    int i;
    for (i = 0; i < DK_MAX_HOSTS; i++) if (g_h[i].used) host_layout(i);
}

static void notify_changed(void) { if (g_cb.layout_changed) g_cb.layout_changed(); }

/* 浮动窗空了 → 销毁 */
static void cleanup_floats(void)
{
    int i;
    for (i = 1; i < DK_MAX_HOSTS; i++) {
        if (g_h[i].used && dt_empty(&g_h[i].tree)) {
            HWND f = g_h[i].frame, hh = g_h[i].host;
            int k;
            /* 先把仍挂在该浮动宿主下的面板救回主宿主（否则 DestroyWindow 连带销毁面板） */
            for (k = 0; k < DK_MAX_PANELS; k++)
                if (g_p[k].used && g_p[k].hwnd && IsWindow(g_p[k].hwnd) && hh && GetParent(g_p[k].hwnd) == hh) {
                    ShowWindow(g_p[k].hwnd, SW_HIDE);
                    SetParent(g_p[k].hwnd, g_h[0].host);
                }
            g_h[i].used = 0;
            g_h[i].frame = g_h[i].host = NULL;
            if (f && IsWindow(f)) DestroyWindow(f);
        }
    }
}

/* ------------------------------------------------------------------ */
/* 绘制：双缓冲，只覆盖标题栏和分隔条（子窗口被裁掉） */
static void host_paint(int hi, HDC dc)
{
    DkHost* H = &g_h[hi];
    RECT rc;
    HDC mem;
    HBITMAP bmp, old;
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    int i;
    GetClientRect(H->host, &rc);
    if (rc.right <= 0 || rc.bottom <= 0) return;
    mem = CreateCompatibleDC(dc);
    bmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
    old = (HBITMAP)SelectObject(mem, bmp);
    FillRect(mem, &rc, os_theme_brush(TH_BORDER));
    SelectObject(mem, font);
    SetBkMode(mem, TRANSPARENT);
    if (dt_empty(&H->tree)) {
        RECT t = rc;
        FillRect(mem, &rc, os_theme_brush(TH_BG));
        SetTextColor(mem, os_theme(TH_DIMTEXT));
        DrawTextW(mem, L"窗口区为空：从菜单「窗口」新建，或把面板标题拖到这里", -1, &t,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    for (i = 0; i < DT_MAX_NODES; i++) {
        DtNode* L = &H->tree.n[i];
        RECT cap, tabs[DT_MAX_TABS], cl, body;
        int n, k, focused;
        if (L->type != DT_LEAF) continue;
        cap = to_rc(L->rc); cap.bottom = cap.top + DK_CAP_H;
        FillRect(mem, &cap, os_theme_brush(TH_PANEL));
        body = leaf_body(L);
        FillRect(mem, &body, os_theme_brush(TH_BG));
        focused = (i == H->focus_leaf);
        n = leaf_tabs(L, tabs, DT_MAX_TABS, &cl);
        for (k = 0; k < n; k++) {
            int act = (k == L->active);
            int id = L->tabs[k];
            RECT tr = tabs[k];
            if (act) {
                FillRect(mem, &tr, os_theme_brush(TH_EDIT_BG));
                if (focused) {
                    RECT ac = tr; ac.bottom = ac.top + 2;
                    HBRUSH b = CreateSolidBrush(RGB(0, 120, 215));
                    FillRect(mem, &ac, b);
                    DeleteObject(b);
                }
            }
            SetTextColor(mem, act ? os_theme(TH_TEXT) : os_theme(TH_DIMTEXT));
            tr.left += 6; tr.right -= 2;
            DrawTextW(mem, g_p[id].title[0] ? g_p[id].title : L"(窗口)", -1, &tr,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
        }
        SetTextColor(mem, os_theme(TH_DIMTEXT));
        DrawTextW(mem, L"\x00D7", -1, &cl, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    BitBlt(dc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
    SelectObject(mem, old);
    DeleteObject(bmp);
    DeleteDC(mem);
}

/* ------------------------------------------------------------------ */
/* 落点高亮 */
static void overlay_show(const RECT* scr)
{
    if (!g_overlay) {
        g_overlay = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                                    OVL_CLASS, L"", WS_POPUP, 0, 0, 10, 10, NULL, NULL,
                                    GetModuleHandleW(NULL), NULL);
        if (g_overlay) SetLayeredWindowAttributes(g_overlay, 0, 96, LWA_ALPHA);
    }
    if (!g_overlay) return;
    if (!scr) { ShowWindow(g_overlay, SW_HIDE); return; }
    SetWindowPos(g_overlay, HWND_TOPMOST, scr->left, scr->top, scr->right - scr->left, scr->bottom - scr->top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

/* 屏幕点 → 目标（宿主/叶子/区域）。exclude_host：不能投放到的宿主（如被拖走的单面板浮动窗） */
static int find_target(POINT scr, int exclude_host, int src_panel, int* out_leaf, int* out_zone, RECT* out_scr)
{
    int pass, i;
    /* 浮动窗在上层：先查浮动窗，再查主宿主 */
    for (pass = 0; pass < 2; pass++) {
        for (i = (pass == 0 ? 1 : 0); i < (pass == 0 ? DK_MAX_HOSTS : 1); i++) {
            DkHost* H = &g_h[i];
            RECT wr;
            POINT c;
            int leaf, zone;
            if (!H->used || i == exclude_host || !IsWindowVisible(H->host)) continue;
            GetWindowRect(H->host, &wr);
            if (!PtInRect(&wr, scr)) continue;
            c = scr;
            ScreenToClient(H->host, &c);
            if (dt_empty(&H->tree)) {
                RECT cr;
                GetClientRect(H->host, &cr);
                *out_leaf = -1; *out_zone = DT_ZONE_CENTER;
                MapWindowPoints(H->host, NULL, (LPPOINT)&cr, 2);
                *out_scr = cr;
                return i;
            }
            leaf = dt_hit_leaf(&H->tree, c.x, c.y);
            if (leaf < 0) return -1;
            zone = dt_zone_at(H->tree.n[leaf].rc, c.x, c.y, DK_EDGE_FRAC);
            /* 拖自己唯一的 tab 到自己窗格：无意义 */
            if (src_panel >= 0 && H->tree.n[leaf].ntabs == 1 && H->tree.n[leaf].tabs[0] == src_panel) return -1;
            if (src_panel >= 0 && zone == DT_ZONE_CENTER && dt_find_leaf(&H->tree, src_panel) == leaf) return -1;
            {
                RECT zr = to_rc(dt_zone_rect(H->tree.n[leaf].rc, zone));
                MapWindowPoints(H->host, NULL, (LPPOINT)&zr, 2);
                *out_scr = zr;
            }
            *out_leaf = leaf; *out_zone = zone;
            return i;
        }
    }
    return -1;
}

static int float_create(int x, int y, int w, int h)
{
    int i;
    for (i = 1; i < DK_MAX_HOSTS; i++) {
        if (!g_h[i].used) {
            DkHost* H = &g_h[i];
            HWND owner = GetAncestor(g_parent, GA_ROOT);
            memset(H, 0, sizeof(*H));
            dt_init(&H->tree, DK_GAP, DK_MIN);
            H->focus_leaf = -1;
            H->used = 1;
            H->frame = CreateWindowExW(WS_EX_TOOLWINDOW, FLOAT_CLASS, L"OpenScope",
                                       WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPCHILDREN,
                                       x, y, w, h, owner, NULL, GetModuleHandleW(NULL), (LPVOID)(INT_PTR)i);
            if (!H->frame) { H->used = 0; return -1; }
            H->host = CreateWindowExW(0, HOST_CLASS, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
                                      0, 0, 10, 10, H->frame, NULL, GetModuleHandleW(NULL), (LPVOID)(INT_PTR)i);
            {
                RECT cr;
                GetClientRect(H->frame, &cr);
                MoveWindow(H->host, 0, 0, cr.right, cr.bottom, FALSE);
            }
            ShowWindow(H->frame, SW_SHOWNOACTIVATE);
            return i;
        }
    }
    return -1;
}

/* 自动放置新面板到主宿主：拆分焦点窗格（宽→右，高→下），太小则作为 tab */
static void auto_place(int id)
{
    DkHost* H = &g_h[0];
    int leaf, zone;
    RECT rc;
    if (dt_empty(&H->tree)) { H->focus_leaf = dt_insert(&H->tree, -1, id, DT_ZONE_CENTER); return; }
    GetClientRect(H->host, &rc);
    dt_layout(&H->tree, to_dt(rc));
    leaf = H->focus_leaf;
    if (leaf < 0 || H->tree.n[leaf].type != DT_LEAF) {
        /* 最大的窗格 */
        int i, best = -1, ba = -1;
        for (i = 0; i < DT_MAX_NODES; i++) {
            DtNode* L = &H->tree.n[i];
            int a;
            if (L->type != DT_LEAF) continue;
            a = (L->rc.r - L->rc.l) * (L->rc.b - L->rc.t);
            if (a > ba) { ba = a; best = i; }
        }
        leaf = best;
    }
    {
        DtNode* L = &H->tree.n[leaf];
        int w = L->rc.r - L->rc.l, h = L->rc.b - L->rc.t;
        if (w >= h) zone = (w >= 2 * DK_MIN + DK_GAP) ? DT_ZONE_RIGHT : DT_ZONE_CENTER;
        else zone = (h >= 2 * (DK_MIN + DK_CAP_H) + DK_GAP) ? DT_ZONE_BOTTOM : DT_ZONE_CENTER;
    }
    H->focus_leaf = dt_insert(&H->tree, leaf, id, zone);
}

/* 把面板移到目标；target_host<0 → 新浮动窗（在 scr 位置） */
static void move_panel_to(int id, int t_host, int t_leaf, int t_zone, POINT scr)
{
    int src = host_of_panel(id);
    RECT pr;
    int pw = 480, ph = 320;
    if (src < 0) return;
    if (g_p[id].hwnd && IsWindow(g_p[id].hwnd)) {
        GetWindowRect(g_p[id].hwnd, &pr);
        pw = pr.right - pr.left; ph = pr.bottom - pr.top + DK_CAP_H;
        if (pw < 240) pw = 240;
        if (ph < 180) ph = 180;
    }
    if (t_host < 0) {
        int fh;
        RECT fr;
        SetRect(&fr, 0, 0, pw, ph);
        AdjustWindowRectEx(&fr, WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, FALSE, WS_EX_TOOLWINDOW);
        fh = float_create(scr.x - 60, scr.y - 10, fr.right - fr.left, fr.bottom - fr.top);
        if (fh < 0) return;
        dt_remove(&g_h[src].tree, id);
        g_h[fh].focus_leaf = dt_insert(&g_h[fh].tree, -1, id, DT_ZONE_CENTER);
    } else {
        /* 目标叶子可能因移除源而塌缩重编号：先在目标树插入再从源移除（同树时先移除需重新定位） */
        if (src == t_host) {
            int keep_active = (t_leaf >= 0) ? dt_active_id(&g_h[t_host].tree, t_leaf) : -1;
            dt_remove(&g_h[src].tree, id);
            if (keep_active >= 0) t_leaf = dt_find_leaf(&g_h[t_host].tree, keep_active);
            if (t_leaf < 0 && !dt_empty(&g_h[t_host].tree)) t_leaf = dt_first_leaf(&g_h[t_host].tree);
            g_h[t_host].focus_leaf = dt_insert(&g_h[t_host].tree, t_leaf, id, t_zone);
        } else {
            g_h[t_host].focus_leaf = dt_insert(&g_h[t_host].tree, t_leaf, id, dt_empty(&g_h[t_host].tree) ? DT_ZONE_CENTER : t_zone);
            dt_remove(&g_h[src].tree, id);
        }
        if (g_h[src].focus_leaf >= 0 && g_h[src].tree.n[g_h[src].focus_leaf].type != DT_LEAF)
            g_h[src].focus_leaf = dt_first_leaf(&g_h[src].tree);
    }
    g_active_panel = id;
    cleanup_floats();
    dock_relayout();
    notify_changed();
}

/* ------------------------------------------------------------------ */
static void drag_end(int commit, POINT scr)
{
    int mode = g_drag.mode;
    g_drag.mode = DRAG_NONE;
    overlay_show(NULL);
    if (GetCapture()) ReleaseCapture();
    if (mode == DRAG_SPLIT) {
        KillTimer(g_h[g_drag.host].host, DK_TIMER_FLUSH);
        host_layout(g_drag.host);
        notify_changed();
        return;
    }
    if (mode != DRAG_TAB || !commit) return;
    if (g_drag.t_host >= 0) {
        move_panel_to(g_drag.panel, g_drag.t_host, g_drag.t_leaf, g_drag.t_zone, scr);
    } else {
        /* 宿主外 → 浮动；落在发起宿主内部（无效落点）→ 不动 */
        RECT wr;
        int src = g_drag.host;
        GetWindowRect(g_h[0].host, &wr);
        if (PtInRect(&wr, scr) && src == 0) return;
        if (src > 0 && dt_panel_count(&g_h[src].tree) == 1) {
            /* 单面板浮动窗被拖到空处：移动浮动窗本身 */
            RECT fr;
            GetWindowRect(g_h[src].frame, &fr);
            SetWindowPos(g_h[src].frame, NULL, scr.x - 60, scr.y - 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            return;
        }
        move_panel_to(g_drag.panel, -1, -1, 0, scr);
    }
}

static void drag_update_tab(POINT scr)
{
    RECT zr;
    int exclude = -1, leaf = -1, zone = 0, th;
    if (g_drag.host > 0 && dt_panel_count(&g_h[g_drag.host].tree) == 1) exclude = g_drag.host;
    th = find_target(scr, exclude, g_drag.panel, &leaf, &zone, &zr);
    g_drag.t_host = th; g_drag.t_leaf = leaf; g_drag.t_zone = zone;
    if (th >= 0) overlay_show(&zr);
    else {
        /* 宿主外：预览浮动位置 */
        RECT wr, pr;
        GetWindowRect(g_h[0].host, &wr);
        if (PtInRect(&wr, scr) && g_drag.host == 0) { overlay_show(NULL); return; }
        SetRect(&pr, scr.x - 60, scr.y - 10, scr.x + 360, scr.y + 250);
        overlay_show(&pr);
    }
}

/* ------------------------------------------------------------------ */
static LRESULT CALLBACK host_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    int hi = (int)(INT_PTR)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    DkHost* H = (hi >= 0 && hi < DK_MAX_HOSTS) ? &g_h[hi] : NULL;
    switch (msg) {
    case WM_NCCREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        break;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        if (H && H->used) host_paint(hi, dc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_SIZE:
        if (H && H->used && g_drag.mode != DRAG_SPLIT) host_layout(hi);
        return 0;
    case WM_SETCURSOR:
        if (H && LOWORD(lp) == HTCLIENT) {
            POINT p;
            int sp;
            GetCursorPos(&p);
            ScreenToClient(hwnd, &p);
            sp = (g_drag.mode == DRAG_SPLIT) ? g_drag.split : dt_hit_splitter(&H->tree, p.x, p.y);
            if (sp >= 0) {
                SetCursor(LoadCursor(NULL, H->tree.n[sp].vert ? IDC_SIZENS : IDC_SIZEWE));
                return TRUE;
            }
        }
        break;
    case WM_LBUTTONDOWN: {
        int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp), sp, leaf;
        if (!H) break;
        sp = dt_hit_splitter(&H->tree, x, y);
        if (sp >= 0) {
            g_drag.mode = DRAG_SPLIT; g_drag.host = hi; g_drag.split = sp; g_drag.last_tick = 0;
            SetCapture(hwnd);
            return 0;
        }
        leaf = dt_hit_leaf(&H->tree, x, y);
        if (leaf >= 0 && y < H->tree.n[leaf].rc.t + DK_CAP_H) {
            RECT tabs[DT_MAX_TABS], cl;
            POINT pt = { x, y };
            int n = leaf_tabs(&H->tree.n[leaf], tabs, DT_MAX_TABS, &cl), k;
            H->focus_leaf = leaf;
            if (PtInRect(&cl, pt)) {
                int id = dt_active_id(&H->tree, leaf);
                if (id >= 0 && g_cb.close_panel) g_cb.close_panel(g_p[id].hwnd);
                return 0;
            }
            for (k = 0; k < n; k++) {
                if (PtInRect(&tabs[k], pt)) {
                    int id = H->tree.n[leaf].tabs[k];
                    H->tree.n[leaf].active = k;
                    g_active_panel = id;
                    host_layout(hi);
                    if (g_cb.panel_activated) g_cb.panel_activated(g_p[id].hwnd);
                    g_drag.mode = DRAG_TAB_PENDING; g_drag.host = hi; g_drag.panel = id;
                    GetCursorPos(&g_drag.start);
                    SetCapture(hwnd);
                    return 0;
                }
            }
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }
    case WM_LBUTTONDBLCLK: {
        int sp = H ? dt_hit_splitter(&H->tree, GET_X_LPARAM(lp), GET_Y_LPARAM(lp)) : -1;
        if (sp >= 0) { dt_equalize(&H->tree, sp); host_layout(hi); notify_changed(); }
        return 0;
    }
    case WM_MOUSEMOVE: {
        POINT scr;
        if (!H) break;
        GetCursorPos(&scr);
        if (g_drag.mode == DRAG_SPLIT) {
            POINT c = scr;
            ScreenToClient(hwnd, &c);
            if (dt_drag_splitter(&H->tree, g_drag.split, c.x, c.y)) {
                DWORD now = GetTickCount();
                if (now - g_drag.last_tick >= DK_THROTTLE_MS) {
                    g_drag.last_tick = now;
                    host_layout(hi);
                    UpdateWindow(hwnd);
                } else {
                    SetTimer(hwnd, DK_TIMER_FLUSH, DK_THROTTLE_MS, NULL);
                }
            }
            return 0;
        }
        if (g_drag.mode == DRAG_TAB_PENDING) {
            if (abs(scr.x - g_drag.start.x) > DK_DRAG_THRESH || abs(scr.y - g_drag.start.y) > DK_DRAG_THRESH) {
                g_drag.mode = DRAG_TAB;
                SetCursor(LoadCursor(NULL, IDC_SIZEALL));
            } else return 0;
        }
        if (g_drag.mode == DRAG_TAB) { drag_update_tab(scr); return 0; }
        break;
    }
    case WM_TIMER:
        if (wp == DK_TIMER_FLUSH) {
            KillTimer(hwnd, DK_TIMER_FLUSH);
            if (H) { g_drag.last_tick = GetTickCount(); host_layout(hi); }
            return 0;
        }
        break;
    case WM_LBUTTONUP: {
        POINT scr;
        GetCursorPos(&scr);
        if (g_drag.mode == DRAG_TAB_PENDING) { g_drag.mode = DRAG_NONE; ReleaseCapture(); return 0; }
        if (g_drag.mode != DRAG_NONE) drag_end(1, scr);
        return 0;
    }
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE && g_drag.mode != DRAG_NONE) { POINT p = { 0, 0 }; drag_end(0, p); return 0; }
        break;
    case WM_CAPTURECHANGED:
        if (g_drag.mode != DRAG_NONE && (HWND)lp != hwnd) {
            POINT p = { 0, 0 };
            int m = g_drag.mode;
            g_drag.mode = DRAG_NONE;
            overlay_show(NULL);
            if (m == DRAG_SPLIT && H) host_layout(hi);
            (void)p;
        }
        break;
    case WM_PARENTNOTIFY:
        /* 点击面板内容 → 焦点窗格跟随 */
        if (H && LOWORD(wp) == WM_LBUTTONDOWN) {
            POINT c = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            int leaf = dt_hit_leaf(&H->tree, c.x, c.y);
            if (leaf >= 0 && leaf != H->focus_leaf) {
                H->focus_leaf = leaf;
                g_active_panel = dt_active_id(&H->tree, leaf);
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK float_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    int hi = (int)(INT_PTR)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_NCCREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        break;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_SIZE:
        if (hi > 0 && hi < DK_MAX_HOSTS && g_h[hi].used && g_h[hi].host)
            MoveWindow(g_h[hi].host, 0, 0, LOWORD(lp), HIWORD(lp), TRUE);
        return 0;
    case WM_ENTERSIZEMOVE:
        g_fmove.host = hi; g_fmove.moved = 0; g_fmove.t_host = -1;
        break;
    case WM_MOVING: {
        POINT scr;
        RECT zr;
        int leaf = -1, zone = 0, th;
        if (g_fmove.host != hi) break;
        g_fmove.moved = 1;
        GetCursorPos(&scr);
        th = find_target(scr, hi, -1, &leaf, &zone, &zr);
        g_fmove.t_host = th; g_fmove.t_leaf = leaf; g_fmove.t_zone = zone;
        overlay_show(th >= 0 ? &zr : NULL);
        break;
    }
    case WM_EXITSIZEMOVE:
        overlay_show(NULL);
        if (g_fmove.host == hi && g_fmove.moved && g_fmove.t_host >= 0) {
            /* 整个浮动窗重新停靠：第一个面板按落点，其余作为同窗格 tab */
            int ids[DT_MAX_TABS], c = dt_list(&g_h[hi].tree, ids, DT_MAX_TABS), k;
            POINT scr;
            int th = g_fmove.t_host, tl = g_fmove.t_leaf, tz = g_fmove.t_zone;
            GetCursorPos(&scr);
            g_fmove.host = -1;
            for (k = 0; k < c && k < DT_MAX_TABS; k++) {
                if (k == 0) move_panel_to(ids[0], th, tl, tz, scr);
                else move_panel_to(ids[k], th, dt_find_leaf(&g_h[th].tree, ids[0]), DT_ZONE_CENTER, scr);
            }
            return 0;
        }
        g_fmove.host = -1;
        notify_changed();
        break;
    case WM_CLOSE:
        if (hi > 0 && hi < DK_MAX_HOSTS && g_h[hi].used) {
            int ids[DT_MAX_TABS], c = dt_list(&g_h[hi].tree, ids, DT_MAX_TABS), k;
            for (k = 0; k < c && k < DT_MAX_TABS; k++)
                if (g_cb.close_panel) g_cb.close_panel(g_p[ids[k]].hwnd);
        }
        return 0;   /* 面板全部销毁后 dock_sync → cleanup_floats 销毁本窗 */
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK ovl_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    return DefWindowProcW(hwnd, msg, wp, lp);
}

/* ------------------------------------------------------------------ */
HWND dock_init(HWND parent, const DockCallbacks* cb)
{
    HINSTANCE inst = GetModuleHandleW(NULL);
    if (!g_registered) {
        WNDCLASSW wc;
        memset(&wc, 0, sizeof(wc));
        wc.lpfnWndProc = host_proc;
        wc.hInstance = inst;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszClassName = HOST_CLASS;
        wc.style = CS_DBLCLKS;
        RegisterClassW(&wc);
        wc.lpfnWndProc = float_proc;
        wc.lpszClassName = FLOAT_CLASS;
        wc.style = 0;
        RegisterClassW(&wc);
        wc.lpfnWndProc = ovl_proc;
        wc.lpszClassName = OVL_CLASS;
        wc.hbrBackground = CreateSolidBrush(RGB(0, 120, 215));
        RegisterClassW(&wc);
        g_registered = 1;
    }
    memset(g_p, 0, sizeof(g_p));
    memset(g_h, 0, sizeof(g_h));
    memset(&g_drag, 0, sizeof(g_drag));
    if (cb) g_cb = *cb;
    g_parent = parent;
    dt_init(&g_h[0].tree, DK_GAP, DK_MIN);
    g_h[0].focus_leaf = -1;
    g_h[0].used = 1;
    g_h[0].host = CreateWindowExW(0, HOST_CLASS, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                  0, 0, 100, 100, parent, NULL, inst, (LPVOID)(INT_PTR)0);
    return g_h[0].host;
}

HWND dock_host(void) { return g_h[0].host; }

void dock_move_host(int x, int y, int w, int h)
{
    if (g_h[0].host) MoveWindow(g_h[0].host, x, y, w, h, TRUE);
}

int dock_is_dragging(void) { return g_drag.mode == DRAG_SPLIT || g_drag.mode == DRAG_TAB; }

void dock_set_excluded(HWND panel)
{
    g_excluded = panel;
    dock_relayout();
}

void dock_sync(const HWND* panels, const wchar_t* const* titles, int n)
{
    int i, k;
    /* 1) 移除已消失的面板 */
    for (i = 0; i < DK_MAX_PANELS; i++) {
        int alive = 0;
        if (!g_p[i].used) continue;
        for (k = 0; k < n; k++) if (panels[k] == g_p[i].hwnd) { alive = 1; break; }
        if (!alive || !IsWindow(g_p[i].hwnd)) {
            int hh = host_of_panel(i);
            if (hh >= 0) dt_remove(&g_h[hh].tree, i);
            g_p[i].used = 0;
            g_p[i].hwnd = NULL;
            if (g_active_panel == i) g_active_panel = -1;
        }
    }
    for (i = 0; i < DK_MAX_HOSTS; i++)
        if (g_h[i].used && g_h[i].focus_leaf >= 0 && g_h[i].tree.n[g_h[i].focus_leaf].type != DT_LEAF)
            g_h[i].focus_leaf = dt_first_leaf(&g_h[i].tree);
    /* 2) 新增 / 更新标题 */
    for (k = 0; k < n; k++) {
        int id = panel_of_hwnd(panels[k]);
        if (!panels[k] || !IsWindow(panels[k])) continue;
        if (id < 0) {
            for (i = 0; i < DK_MAX_PANELS; i++) if (!g_p[i].used) break;
            if (i >= DK_MAX_PANELS) continue;
            id = i;
            g_p[id].used = 1;
            g_p[id].hwnd = panels[k];
            g_p[id].title[0] = 0;
            auto_place(id);
            g_active_panel = id;
        }
        if (titles && titles[k]) _snwprintf(g_p[id].title, 127, L"%s", titles[k]);
    }
    cleanup_floats();
    dock_relayout();
}

void dock_set_title(HWND panel, const wchar_t* title)
{
    int id = panel_of_hwnd(panel);
    if (id < 0 || !title) return;
    _snwprintf(g_p[id].title, 127, L"%s", title);
    dock_relayout();
}

void dock_activate(HWND panel)
{
    int id = panel_of_hwnd(panel), hh;
    if (id < 0) return;
    hh = host_of_panel(id);
    if (hh < 0) return;
    g_h[hh].focus_leaf = dt_activate(&g_h[hh].tree, id);
    g_active_panel = id;
    host_layout(hh);
    if (g_h[hh].frame) SetWindowPos(g_h[hh].frame, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

HWND dock_active_panel(void)
{
    return (g_active_panel >= 0 && g_p[g_active_panel].used) ? g_p[g_active_panel].hwnd : NULL;
}

void dock_apply_theme(void)
{
    int i;
    for (i = 0; i < DK_MAX_HOSTS; i++) if (g_h[i].used && g_h[i].host) InvalidateRect(g_h[i].host, NULL, FALSE);
}

void dock_reset(void)
{
    int i, ids[DK_MAX_PANELS], c = 0;
    for (i = 0; i < DK_MAX_PANELS; i++) if (g_p[i].used) ids[c++] = i;
    for (i = 0; i < DK_MAX_HOSTS; i++) if (g_h[i].used) dt_init(&g_h[i].tree, DK_GAP, DK_MIN);
    g_h[0].focus_leaf = -1;
    cleanup_floats();
    for (i = 0; i < c; i++) { g_h[0].focus_leaf = -1; auto_place(ids[i]); }
    dt_equalize(&g_h[0].tree, -1);
    dock_relayout();
    notify_changed();
}

int dock_split_active(int vertical)
{
    int hh, leaf, id;
    POINT p = { 0, 0 };
    if (g_active_panel < 0) return -1;
    id = g_active_panel;
    hh = host_of_panel(id);
    if (hh < 0) return -1;
    leaf = dt_find_leaf(&g_h[hh].tree, id);
    if (g_h[hh].tree.n[leaf].ntabs < 2) return -1;   /* 只有一个 tab：无可拆 */
    /* Terminator: Ctrl+Shift+O 上下拆，Ctrl+Shift+E 左右拆 */
    move_panel_to(id, hh, leaf, vertical ? DT_ZONE_BOTTOM : DT_ZONE_RIGHT, p);
    return 0;
}

int dock_move_panel(HWND panel, HWND target_panel, int zone)
{
    int id = panel_of_hwnd(panel), tid, th;
    POINT p;
    if (id < 0) return -1;
    GetCursorPos(&p);
    if (!target_panel) { move_panel_to(id, -1, -1, 0, p); return 0; }
    tid = panel_of_hwnd(target_panel);
    th = host_of_panel(tid);
    if (tid < 0 || th < 0 || tid == id) return -1;
    move_panel_to(id, th, dt_find_leaf(&g_h[th].tree, tid), zone, p);
    return 0;
}

/* ------------------------------------------------------------------ */
/* 持久化 */
typedef struct { int (*idx)(HWND, void*); void* u; } MapOut;
static int map_out_cb(int id, void* u)
{
    MapOut* m = (MapOut*)u;
    if (id < 0 || id >= DK_MAX_PANELS || !g_p[id].used) return -1;
    return m->idx(g_p[id].hwnd, m->u);
}

void dock_save(void (*emit)(const char*, const char*, void*), int (*panel_index)(HWND, void*), void* u)
{
    char buf[4096], line[4200];
    MapOut m;
    int i;
    m.idx = panel_index; m.u = u;
    emit("version", "1", u);
    if (dt_serialize(&g_h[0].tree, buf, sizeof(buf), map_out_cb, &m) >= 0) emit("main", buf, u);
    for (i = 1; i < DK_MAX_HOSTS; i++) {
        RECT r;
        if (!g_h[i].used || !g_h[i].frame) continue;
        if (dt_serialize(&g_h[i].tree, buf, sizeof(buf), map_out_cb, &m) < 0 || buf[0] == 'E') continue;
        GetWindowRect(g_h[i].frame, &r);
        _snprintf(line, sizeof(line) - 1, "%ld,%ld,%ld,%ld|%s", r.left, r.top, r.right - r.left, r.bottom - r.top, buf);
        line[sizeof(line) - 1] = 0;
        emit("float", line, u);
    }
}

static char g_ld_main[4096];
static char g_ld_float[DK_MAX_HOSTS][4200];
static int  g_ld_nfloat, g_ld_have;

void dock_load_begin(void) { g_ld_main[0] = 0; g_ld_nfloat = 0; g_ld_have = 0; }

void dock_load_line(const char* key, const char* val)
{
    if (!key || !val) return;
    if (!strcmp(key, "main")) { _snprintf(g_ld_main, sizeof(g_ld_main) - 1, "%s", val); g_ld_have = 1; }
    else if (!strcmp(key, "float") && g_ld_nfloat < DK_MAX_HOSTS - 1) {
        _snprintf(g_ld_float[g_ld_nfloat], sizeof(g_ld_float[0]) - 1, "%s", val);
        g_ld_nfloat++;
    }
}

static HWND g_fixed[16];
static int  g_nfixed;
void dock_set_fixed(const HWND* fixed, int n)
{
    int i;
    g_nfixed = n > 16 ? 16 : n;
    for (i = 0; i < g_nfixed; i++) g_fixed[i] = fixed[i];
}

typedef struct { const HWND* by; int n; } MapIn;
static int map_in_cb(int num, void* u)
{
    MapIn* m = (MapIn*)u;
    if (num >= DOCK_FIXED_BASE) {
        num -= DOCK_FIXED_BASE;
        return (num < g_nfixed && g_fixed[num]) ? panel_of_hwnd(g_fixed[num]) : -1;
    }
    if (num < 0 || num >= m->n || !m->by[num]) return -1;
    return panel_of_hwnd(m->by[num]);
}

int dock_load_apply(const HWND* by_index, int n)
{
    MapIn m;
    int i, placed[DK_MAX_PANELS];
    DtTree tmp;
    if (!g_ld_have) return -1;               /* 旧格式：保持自动放置结果 */
    m.by = by_index; m.n = n;
    dt_init(&tmp, DK_GAP, DK_MIN);
    if (dt_parse(&tmp, g_ld_main, map_in_cb, &m) != 0) return -1;
    /* 清空现有放置 */
    for (i = 0; i < DK_MAX_HOSTS; i++) if (g_h[i].used) dt_init(&g_h[i].tree, DK_GAP, DK_MIN);
    cleanup_floats();
    g_h[0].tree = tmp;
    g_h[0].focus_leaf = dt_first_leaf(&g_h[0].tree);
    for (i = 0; i < g_ld_nfloat; i++) {
        int x, y, w, h, fh;
        const char* bar = strchr(g_ld_float[i], '|');
        if (!bar || sscanf(g_ld_float[i], "%d,%d,%d,%d", &x, &y, &w, &h) != 4) continue;
        {
            RECT r;
            SetRect(&r, x, y, x + w, y + h);
            if (!MonitorFromRect(&r, MONITOR_DEFAULTTONULL)) { x = 100; y = 100; }
        }
        if (w < 200) w = 200;
        if (h < 150) h = 150;
        dt_init(&tmp, DK_GAP, DK_MIN);
        if (dt_parse(&tmp, bar + 1, map_in_cb, &m) != 0 || dt_empty(&tmp)) continue;
        /* 主树已含的面板不得重复 */
        {
            int ids[DT_MAX_TABS], c = dt_list(&tmp, ids, DT_MAX_TABS), k;
            for (k = 0; k < c && k < DT_MAX_TABS; k++) if (dt_find_leaf(&g_h[0].tree, ids[k]) >= 0) dt_remove(&tmp, ids[k]);
            if (dt_empty(&tmp)) continue;
        }
        fh = float_create(x, y, w, h);
        if (fh < 0) continue;
        g_h[fh].tree = tmp;
        g_h[fh].focus_leaf = dt_first_leaf(&tmp);
    }
    /* 未被布局引用的面板 → 自动放置 */
    memset(placed, 0, sizeof(placed));
    for (i = 0; i < DK_MAX_PANELS; i++)
        if (g_p[i].used && host_of_panel(i) < 0) auto_place(i);
    dock_relayout();
    return 0;
}

/* ------------------------------------------------------------------ */
static int jesc(const wchar_t* w, char* out, int cap)
{
    char u8[512];
    int o = 0, i;
    WideCharToMultiByte(CP_UTF8, 0, w, -1, u8, sizeof(u8), NULL, NULL);
    for (i = 0; u8[i] && o < cap - 3; i++) {
        if (u8[i] == '"' || u8[i] == '\\') out[o++] = '\\';
        out[o++] = u8[i];
    }
    out[o] = 0;
    return o;
}

int dock_describe_json(char* out, int cap)
{
    int o = 0, i, first = 1;
    char tree[4096];
    dt_serialize(&g_h[0].tree, tree, sizeof(tree), NULL, NULL);
    o += _snprintf(out + o, cap - o, "{\"ok\":true,\"main\":\"%s\",\"hosts\":[", tree);
    for (i = 0; i < DK_MAX_HOSTS && o < cap - 512; i++) {
        DkHost* H = &g_h[i];
        RECT hr;
        int k, f2 = 1;
        if (!H->used || !H->host) continue;
        GetWindowRect(H->host, &hr);
        dt_serialize(&H->tree, tree, sizeof(tree), NULL, NULL);
        o += _snprintf(out + o, cap - o, "%s{\"host\":%d,\"float\":%d,\"tree\":\"%s\",\"rect\":[%ld,%ld,%ld,%ld],\"panes\":[",
                       first ? "" : ",", i, H->frame ? 1 : 0, tree, hr.left, hr.top, hr.right, hr.bottom);
        if (H->frame) {
            RECT fr;
            GetWindowRect(H->frame, &fr);
            o += 0;
        }
        first = 0;
        for (k = 0; k < DT_MAX_NODES && o < cap - 400; k++) {
            DtNode* L = &H->tree.n[k];
            RECT tabs[DT_MAX_TABS], cl, r = to_rc(L->rc);
            int n, t;
            if (L->type != DT_LEAF) continue;
            MapWindowPoints(H->host, NULL, (LPPOINT)&r, 2);
            n = leaf_tabs(L, tabs, DT_MAX_TABS, &cl);
            o += _snprintf(out + o, cap - o, "%s{\"leaf\":%d,\"rect\":[%ld,%ld,%ld,%ld],\"active\":%d,\"tabs\":[",
                           f2 ? "" : ",", k, r.left, r.top, r.right, r.bottom, L->active);
            f2 = 0;
            for (t = 0; t < n && o < cap - 300; t++) {
                char ti[300];
                RECT tr = tabs[t];
                MapWindowPoints(H->host, NULL, (LPPOINT)&tr, 2);
                jesc(g_p[L->tabs[t]].title, ti, sizeof(ti));
                o += _snprintf(out + o, cap - o, "%s{\"id\":%d,\"title\":\"%s\",\"hwnd\":%llu,\"cx\":%ld,\"cy\":%ld}",
                               t ? "," : "", L->tabs[t], ti, (unsigned long long)(UINT_PTR)g_p[L->tabs[t]].hwnd,
                               (tr.left + tr.right) / 2, (tr.top + tr.bottom) / 2);
            }
            o += _snprintf(out + o, cap - o, "]}");
        }
        o += _snprintf(out + o, cap - o, "],\"splitters\":[");
        f2 = 1;
        for (k = 0; k < DT_MAX_NODES && o < cap - 200; k++) {
            DtNode* S = &H->tree.n[k];
            RECT r;
            if (S->type != DT_SPLIT) continue;
            r = to_rc(S->split_rc);
            MapWindowPoints(H->host, NULL, (LPPOINT)&r, 2);
            o += _snprintf(out + o, cap - o, "%s{\"node\":%d,\"vert\":%d,\"ratio\":%.4f,\"rect\":[%ld,%ld,%ld,%ld]}",
                           f2 ? "" : ",", k, S->vert, S->ratio, r.left, r.top, r.right, r.bottom);
            f2 = 0;
        }
        o += _snprintf(out + o, cap - o, "]}");
    }
    o += _snprintf(out + o, cap - o, "]}");
    return o;
}

int dock_apply_tree(const char* main_tree, const HWND* by_index, int n)
{
    int rc;
    dock_load_begin();
    dock_load_line("main", main_tree);
    rc = dock_load_apply(by_index, n);
    dock_load_begin();
    notify_changed();
    return rc;
}
