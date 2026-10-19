/*
 * 可停靠命令栏实现：拖拽吸附 + 浮动宿主 + ini 持久化。
 */
#include "dockbar.h"
#include "app.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

#define SNAP_PX 28
#define IDC_DOCK_GRIP 9001

static const wchar_t* k_cls = L"OSDockBar";
static const wchar_t* k_float_cls = L"OSDockFloat";
static OS_DockBar g_bars[OS_DOCKBAR_COUNT];
static HWND g_main;
static int g_drag_id = -1;
static POINT g_drag_start;
static OS_DockSide g_preview = OS_DOCK_TOP;
static int g_preview_on;

static void config_path(wchar_t* out, int cap)
{
    wchar_t dir[MAX_PATH];
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", dir, MAX_PATH) == 0 || !dir[0]) {
        GetModuleFileNameW(NULL, dir, MAX_PATH);
        {
            wchar_t* slash = wcsrchr(dir, L'\\');
            if (slash) *slash = 0;
        }
    } else {
        _snwprintf(dir, MAX_PATH, L"%s\\OpenScope", dir);
        CreateDirectoryW(dir, NULL);
    }
    _snwprintf(out, cap, L"%s\\dockbars.ini", dir);
}

static OS_DockSide parse_side(const wchar_t* s)
{
    if (!s) return OS_DOCK_TOP;
    if (_wcsicmp(s, L"Bottom") == 0) return OS_DOCK_BOTTOM;
    if (_wcsicmp(s, L"Left") == 0) return OS_DOCK_LEFT;
    if (_wcsicmp(s, L"Right") == 0) return OS_DOCK_RIGHT;
    if (_wcsicmp(s, L"Float") == 0) return OS_DOCK_FLOAT;
    return OS_DOCK_TOP;
}

static const wchar_t* side_name(OS_DockSide d)
{
    switch (d) {
    case OS_DOCK_BOTTOM: return L"Bottom";
    case OS_DOCK_LEFT: return L"Left";
    case OS_DOCK_RIGHT: return L"Right";
    case OS_DOCK_FLOAT: return L"Float";
    default: return L"Top";
    }
}

/* 命令栏为水平排布：只吸附上/下，左右视为浮动（避免栏宽 34px 裁掉按钮） */
static OS_DockSide hit_edge(HWND main, POINT pt_screen)
{
    RECT rc;
    POINT pt = pt_screen;
    GetClientRect(main, &rc);
    ScreenToClient(main, &pt);
    if (pt.y >= 0 && pt.y < SNAP_PX && pt.x >= 0 && pt.x < rc.right) return OS_DOCK_TOP;
    if (pt.y > rc.bottom - SNAP_PX && pt.y <= rc.bottom && pt.x >= 0 && pt.x < rc.right)
        return OS_DOCK_BOTTOM;
    return OS_DOCK_FLOAT;
}

static OS_DockSide normalize_side(OS_DockSide s)
{
    if (s == OS_DOCK_LEFT || s == OS_DOCK_RIGHT) return OS_DOCK_TOP;
    return s;
}

static void show_bar_children(HWND bar)
{
    HWND c;
    if (!bar) return;
    for (c = GetWindow(bar, GW_CHILD); c; c = GetWindow(c, GW_HWNDNEXT)) {
        ShowWindow(c, SW_SHOW);
        EnableWindow(c, TRUE);
    }
    InvalidateRect(bar, NULL, TRUE);
}

static void size_bar_to_float_client(OS_DockBar* b)
{
    RECT crc;
    if (!b || !b->hFloat || !b->hwnd) return;
    GetClientRect(b->hFloat, &crc);
    if (crc.right < 80) crc.right = 420;
    if (crc.bottom < OS_DOCKBAR_THICK) crc.bottom = OS_DOCKBAR_THICK;
    MoveWindow(b->hwnd, 0, 0, crc.right, crc.bottom, TRUE);
    show_bar_children(b->hwnd);
}

static void destroy_float(OS_DockBar* b)
{
    if (b->hFloat && IsWindow(b->hFloat)) {
        /* 必须先把栏从浮动窗卸下，否则 DestroyWindow 会连带销毁子控件（按钮） */
        if (b->hwnd && IsWindow(b->hwnd)) {
            SetParent(b->hwnd, g_main);
            ShowWindow(b->hwnd, SW_SHOW);
            show_bar_children(b->hwnd);
        }
        DestroyWindow(b->hFloat);
    }
    b->hFloat = NULL;
}

static void ensure_float(OS_DockBar* b)
{
    RECT rc, crc;
    if (b->hFloat && IsWindow(b->hFloat)) {
        size_bar_to_float_client(b);
        return;
    }
    if (b->float_w < 200) b->float_w = 520;
    if (b->float_h < 70) b->float_h = OS_DOCKBAR_THICK + 40; /* 含标题栏 */
    if (b->float_x == 0 && b->float_y == 0) {
        GetWindowRect(g_main, &rc);
        b->float_x = rc.left + 80;
        b->float_y = rc.top + 80;
    }
    b->hFloat = CreateWindowExW(WS_EX_TOOLWINDOW, k_float_cls, b->title,
                                WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE,
                                b->float_x, b->float_y, b->float_w, b->float_h,
                                g_main, NULL, g_app.hInst, (LPVOID)(INT_PTR)b->id);
    if (!b->hFloat) return;
    SetParent(b->hwnd, b->hFloat);
    /* 按浮动窗真实客户区铺满，勿用外框尺寸硬减 */
    GetClientRect(b->hFloat, &crc);
    MoveWindow(b->hwnd, 0, 0,
               crc.right > 0 ? crc.right : b->float_w,
               crc.bottom > 0 ? crc.bottom : OS_DOCKBAR_THICK, TRUE);
    ShowWindow(b->hwnd, SW_SHOW);
    show_bar_children(b->hwnd);
}

void os_dockbar_set_dock(int id, OS_DockSide side)
{
    OS_DockBar* b;
    if (id < 0 || id >= OS_DOCKBAR_COUNT) return;
    b = &g_bars[id];
    side = normalize_side(side);
    if (side != OS_DOCK_FLOAT && b->dock == OS_DOCK_FLOAT)
        destroy_float(b);
    if (side == OS_DOCK_FLOAT) {
        if (b->dock != OS_DOCK_FLOAT) b->last_dock = normalize_side(b->dock);
        b->dock = OS_DOCK_FLOAT;
        ensure_float(b);
    } else {
        b->dock = side;
        b->last_dock = side;
        if (b->hwnd) {
            SetParent(b->hwnd, g_main);
            ShowWindow(b->hwnd, SW_SHOW);
            show_bar_children(b->hwnd);
        }
    }
    os_dockbar_save();
    if (g_main) {
        /* 同步布局：先发 SIZE 再强制重绘，确保按钮 MoveWindow 生效 */
        SendMessageW(g_main, WM_SIZE, SIZE_RESTORED, 0);
        InvalidateRect(g_main, NULL, TRUE);
        if (b->hwnd) InvalidateRect(b->hwnd, NULL, TRUE);
    }
}

static void paint_grip(HWND hwnd, HDC hdc)
{
    RECT rc;
    int i;
    GetClientRect(hwnd, &rc);
    FillRect(hdc, &rc, GetSysColorBrush(COLOR_BTNFACE));
    for (i = 4; i + 4 < rc.bottom && i < 28; i += 4) {
        SetPixel(hdc, 4, i, GetSysColor(COLOR_BTNSHADOW));
        SetPixel(hdc, 6, i, GetSysColor(COLOR_BTNHIGHLIGHT));
        SetPixel(hdc, 8, i, GetSysColor(COLOR_BTNSHADOW));
    }
}

static LRESULT CALLBACK dockbar_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    OS_DockBar* b = NULL;
    int id = (int)(INT_PTR)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (id >= 0 && id < OS_DOCKBAR_COUNT) b = &g_bars[id];

    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        id = (int)(INT_PTR)cs->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)id);
        CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY,
                      0, 0, OS_DOCKBAR_GRIP, OS_DOCKBAR_THICK, hwnd,
                      (HMENU)IDC_DOCK_GRIP, g_app.hInst, NULL);
        return 0;
    }
    case WM_SIZE: {
        HWND grip = GetDlgItem(hwnd, IDC_DOCK_GRIP);
        int w = LOWORD(lp), h = HIWORD(lp);
        if (grip) {
            if (b && (b->dock == OS_DOCK_LEFT || b->dock == OS_DOCK_RIGHT))
                MoveWindow(grip, 0, 0, w, OS_DOCKBAR_GRIP, TRUE);
            else
                MoveWindow(grip, 0, 0, OS_DOCKBAR_GRIP, h, TRUE);
        }
        return 0;
    }
    case WM_CTLCOLORSTATIC:
        return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
    case WM_COMMAND:
        /* 按钮命令转发给主窗 */
        if (g_main) PostMessageW(g_main, WM_COMMAND, wp, lp);
        return 0;
    case WM_LBUTTONDOWN:
        if (b && (HWND)lp == GetDlgItem(hwnd, IDC_DOCK_GRIP) ||
            GetDlgCtrlID((HWND)lp) == IDC_DOCK_GRIP) {
            /* fallthrough via notify from grip — handled below */
        }
        break;
    case WM_NOTIFY:
        return 0;
    case WM_DRAWITEM:
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, GetSysColorBrush(COLOR_BTNFACE));
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_LBUTTONDBLCLK:
        if (b) {
            if (b->dock == OS_DOCK_FLOAT)
                os_dockbar_set_dock(b->id, b->last_dock != OS_DOCK_FLOAT ? b->last_dock : OS_DOCK_TOP);
            else
                os_dockbar_set_dock(b->id, OS_DOCK_FLOAT);
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

/* 抓取条子类：开始拖拽 */
static WNDPROC g_grip_old;

static LRESULT CALLBACK grip_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    HWND bar = GetParent(hwnd);
    int id = bar ? (int)(INT_PTR)GetWindowLongPtrW(bar, GWLP_USERDATA) : -1;
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        paint_grip(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN:
        if (id >= 0 && id < OS_DOCKBAR_COUNT) {
            g_drag_id = id;
            GetCursorPos(&g_drag_start);
            SetCapture(hwnd);
            g_preview_on = 1;
        }
        return 0;
    case WM_LBUTTONDBLCLK:
        if (id >= 0) {
            OS_DockBar* b = &g_bars[id];
            if (b->dock == OS_DOCK_FLOAT)
                os_dockbar_set_dock(id, b->last_dock != OS_DOCK_FLOAT ? b->last_dock : OS_DOCK_TOP);
            else
                os_dockbar_set_dock(id, OS_DOCK_FLOAT);
        }
        return 0;
    case WM_MOUSEMOVE:
        if (g_drag_id >= 0 && (wp & MK_LBUTTON)) {
            POINT pt;
            GetCursorPos(&pt);
            g_preview = hit_edge(g_main, pt);
            /* 拖出客户区较远则预览为浮动 */
            {
                RECT rc;
                POINT c = pt;
                GetClientRect(g_main, &rc);
                ScreenToClient(g_main, &c);
                if (c.x < -40 || c.y < -40 || c.x > rc.right + 40 || c.y > rc.bottom + 40)
                    g_preview = OS_DOCK_FLOAT;
            }
            if (g_bars[g_drag_id].dock == OS_DOCK_FLOAT && g_bars[g_drag_id].hFloat) {
                int dx = pt.x - g_drag_start.x, dy = pt.y - g_drag_start.y;
                if (g_preview == OS_DOCK_FLOAT) {
                    RECT fr;
                    GetWindowRect(g_bars[g_drag_id].hFloat, &fr);
                    SetWindowPos(g_bars[g_drag_id].hFloat, NULL,
                                 fr.left + dx, fr.top + dy, 0, 0,
                                 SWP_NOSIZE | SWP_NOZORDER);
                    g_drag_start = pt;
                }
            }
            InvalidateRect(g_main, NULL, FALSE);
        }
        return 0;
    case WM_LBUTTONUP:
        if (g_drag_id >= 0) {
            POINT pt;
            OS_DockSide side;
            GetCursorPos(&pt);
            side = hit_edge(g_main, pt);
            {
                RECT rc;
                POINT c = pt;
                GetClientRect(g_main, &rc);
                ScreenToClient(g_main, &c);
                if (c.x < -40 || c.y < -40 || c.x > rc.right + 40 || c.y > rc.bottom + 40)
                    side = OS_DOCK_FLOAT;
            }
            if (side == OS_DOCK_FLOAT) {
                g_bars[g_drag_id].float_x = pt.x - 40;
                g_bars[g_drag_id].float_y = pt.y - 10;
            }
            os_dockbar_set_dock(g_drag_id, side);
            g_drag_id = -1;
            g_preview_on = 0;
            ReleaseCapture();
            InvalidateRect(g_main, NULL, TRUE);
        }
        return 0;
    }
    return CallWindowProcW(g_grip_old, hwnd, msg, wp, lp);
}

static LRESULT CALLBACK float_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    int id = (int)(INT_PTR)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return 0;
    }
    case WM_SIZE:
        if (id >= 0 && id < OS_DOCKBAR_COUNT && g_bars[id].hwnd) {
            RECT wr;
            MoveWindow(g_bars[id].hwnd, 0, 0, LOWORD(lp), HIWORD(lp), TRUE);
            show_bar_children(g_bars[id].hwnd);
            GetWindowRect(hwnd, &wr);
            g_bars[id].float_w = wr.right - wr.left;
            g_bars[id].float_h = wr.bottom - wr.top;
            /* 通知主窗重排栏内按钮位置 */
            if (g_main) PostMessageW(g_main, WM_SIZE, SIZE_RESTORED, 0);
        }
        return 0;
    case WM_MOVE:
        if (id >= 0 && id < OS_DOCKBAR_COUNT) {
            RECT rc;
            GetWindowRect(hwnd, &rc);
            g_bars[id].float_x = rc.left;
            g_bars[id].float_y = rc.top;
        }
        return 0;
    case WM_CLOSE:
        /* 关闭浮动窗 = 吸回上次 dock */
        if (id >= 0)
            os_dockbar_set_dock(id, g_bars[id].last_dock != OS_DOCK_FLOAT
                                    ? g_bars[id].last_dock : OS_DOCK_TOP);
        return 0;
    case WM_COMMAND:
        if (g_main) PostMessageW(g_main, WM_COMMAND, wp, lp);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void os_dockbar_register(HINSTANCE inst)
{
    WNDCLASSW wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = dockbar_proc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = k_cls;
    RegisterClassW(&wc);
    wc.lpfnWndProc = float_proc;
    wc.lpszClassName = k_float_cls;
    wc.style = CS_DBLCLKS;
    RegisterClassW(&wc);
}

int os_dockbar_create(HWND main, HINSTANCE inst)
{
    int i;
    g_main = main;
    memset(g_bars, 0, sizeof(g_bars));
    g_bars[OS_DOCKBAR_MAIN].id = OS_DOCKBAR_MAIN;
    g_bars[OS_DOCKBAR_DEBUG].id = OS_DOCKBAR_DEBUG;
    _snwprintf(g_bars[OS_DOCKBAR_MAIN].title, 64, L"主命令栏");
    _snwprintf(g_bars[OS_DOCKBAR_DEBUG].title, 64, L"调试");
    g_bars[OS_DOCKBAR_MAIN].dock = OS_DOCK_TOP;
    g_bars[OS_DOCKBAR_MAIN].last_dock = OS_DOCK_TOP;
    g_bars[OS_DOCKBAR_DEBUG].dock = OS_DOCK_TOP;
    g_bars[OS_DOCKBAR_DEBUG].last_dock = OS_DOCK_TOP;

    for (i = 0; i < OS_DOCKBAR_COUNT; i++) {
        HWND grip;
        g_bars[i].hwnd = CreateWindowExW(0, k_cls, g_bars[i].title,
                                         WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
                                         0, 0, 400, OS_DOCKBAR_THICK, main, NULL, inst,
                                         (LPVOID)(INT_PTR)i);
        if (!g_bars[i].hwnd) return -1;
        grip = GetDlgItem(g_bars[i].hwnd, IDC_DOCK_GRIP);
        if (grip) {
            g_grip_old = (WNDPROC)SetWindowLongPtrW(grip, GWLP_WNDPROC, (LONG_PTR)grip_proc);
        }
    }
    os_dockbar_load();
    for (i = 0; i < OS_DOCKBAR_COUNT; i++) {
        if (g_bars[i].dock == OS_DOCK_FLOAT)
            ensure_float(&g_bars[i]);
    }
    return 0;
}

void os_dockbar_destroy(void)
{
    int i;
    os_dockbar_save();
    for (i = 0; i < OS_DOCKBAR_COUNT; i++) {
        destroy_float(&g_bars[i]);
        if (g_bars[i].hwnd && IsWindow(g_bars[i].hwnd))
            DestroyWindow(g_bars[i].hwnd);
        g_bars[i].hwnd = NULL;
    }
}

OS_DockBar* os_dockbar_get(int id)
{
    if (id < 0 || id >= OS_DOCKBAR_COUNT) return NULL;
    return &g_bars[id];
}

HWND os_dockbar_hwnd(int id)
{
    OS_DockBar* b = os_dockbar_get(id);
    return b ? b->hwnd : NULL;
}

void os_dockbar_apply_layout(HWND main, RECT* content)
{
    int top = 0, bottom = 0, left = 0, right = 0;
    int i;
    int cw, ch;
    if (!content) return;
    cw = content->right - content->left;
    ch = content->bottom - content->top;

    /* 先统计占用 */
    for (i = 0; i < OS_DOCKBAR_COUNT; i++) {
        if (g_bars[i].dock == OS_DOCK_TOP) top += OS_DOCKBAR_THICK;
        else if (g_bars[i].dock == OS_DOCK_BOTTOM) bottom += OS_DOCKBAR_THICK;
        else if (g_bars[i].dock == OS_DOCK_LEFT) left += OS_DOCKBAR_THICK;
        else if (g_bars[i].dock == OS_DOCK_RIGHT) right += OS_DOCKBAR_THICK;
    }

    /* 放置停靠栏 */
    {
        int ty = content->top, by = content->bottom;
        int lx = content->left, rx = content->right;
        for (i = 0; i < OS_DOCKBAR_COUNT; i++) {
            OS_DockBar* b = &g_bars[i];
            if (!b->hwnd) continue;
            if (b->dock == OS_DOCK_FLOAT) {
                if (b->hFloat && IsWindow(b->hFloat))
                    size_bar_to_float_client(b);
                else
                    ShowWindow(b->hwnd, SW_SHOW);
                continue;
            }
            ShowWindow(b->hwnd, SW_SHOW);
            /* 仅上/下水平停靠；历史 Left/Right 已在 normalize 中折到 Top */
            if (b->dock == OS_DOCK_BOTTOM) {
                by -= OS_DOCKBAR_THICK;
                MoveWindow(b->hwnd, content->left + left, by,
                           cw - left - right, OS_DOCKBAR_THICK, TRUE);
            } else {
                MoveWindow(b->hwnd, content->left + left, ty,
                           cw - left - right, OS_DOCKBAR_THICK, TRUE);
                ty += OS_DOCKBAR_THICK;
            }
            show_bar_children(b->hwnd);
            (void)lx; (void)rx;
        }
    }

    content->left += left;
    content->right -= right;
    content->top += top;
    content->bottom -= bottom;
    (void)main;
    (void)cw;
    (void)ch;
}

void os_dockbar_load(void)
{
    wchar_t path[MAX_PATH], buf[64];
    int i;
    config_path(path, MAX_PATH);
    for (i = 0; i < OS_DOCKBAR_COUNT; i++) {
        const wchar_t* key = (i == OS_DOCKBAR_MAIN) ? L"main" : L"debug";
        GetPrivateProfileStringW(L"dock", key, L"Top", buf, 64, path);
        g_bars[i].dock = normalize_side(parse_side(buf));
        g_bars[i].last_dock = g_bars[i].dock == OS_DOCK_FLOAT ? OS_DOCK_TOP : g_bars[i].dock;
        g_bars[i].float_x = (int)GetPrivateProfileIntW(key, L"x", 100, path);
        g_bars[i].float_y = (int)GetPrivateProfileIntW(key, L"y", 100, path);
        g_bars[i].float_w = (int)GetPrivateProfileIntW(key, L"w", 520, path);
        g_bars[i].float_h = (int)GetPrivateProfileIntW(key, L"h", 74, path);
        if (g_bars[i].float_w < 200) g_bars[i].float_w = 520;
        if (g_bars[i].float_h < 70) g_bars[i].float_h = 74;
    }
    /* 默认：两栏都 Top（主在上、调试在下 —— 按创建序叠加） */
}

void os_dockbar_save(void)
{
    wchar_t path[MAX_PATH], num[32];
    int i;
    config_path(path, MAX_PATH);
    for (i = 0; i < OS_DOCKBAR_COUNT; i++) {
        const wchar_t* key = (i == OS_DOCKBAR_MAIN) ? L"main" : L"debug";
        WritePrivateProfileStringW(L"dock", key, side_name(g_bars[i].dock), path);
        _snwprintf(num, 32, L"%d", g_bars[i].float_x);
        WritePrivateProfileStringW(key, L"x", num, path);
        _snwprintf(num, 32, L"%d", g_bars[i].float_y);
        WritePrivateProfileStringW(key, L"y", num, path);
        _snwprintf(num, 32, L"%d", g_bars[i].float_w);
        WritePrivateProfileStringW(key, L"w", num, path);
        _snwprintf(num, 32, L"%d", g_bars[i].float_h);
        WritePrivateProfileStringW(key, L"h", num, path);
    }
}
