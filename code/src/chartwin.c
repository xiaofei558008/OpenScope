#include "app.h"
#include "chartwin.h"
#include "mainwin.h"
#include "vartree.h"
#include <string.h>

#define OS_MAGIC_CHART 0x43484152u /* 'CHAR' */

typedef struct OS_Series {
    int leaf_id;
    COLORREF color;
    int64_t ts[OS_CHART_HIST];
    double val[OS_CHART_HIST];
    int head, count;
} OS_Series;

typedef struct OS_ChartWin {
    DWORD magic;
    HWND hwnd;
    wchar_t title[128];
    OS_Series series[OS_MAX_CHART_SERIES];
    int series_count;
    int npoints;
    int paused;
    int sel;
    uint64_t last_paint;
} OS_ChartWin;

static const wchar_t* g_chart_class = L"OSChartWin";
static const COLORREF g_pal[] = {
    RGB(255, 220, 0), RGB(0, 230, 255), RGB(120, 255, 0), RGB(255, 80, 220),
    RGB(255, 90, 90), RGB(255, 170, 0), RGB(200, 200, 255), RGB(0, 255, 170)
};

#define MENU_CHART_ADD    3001
#define MENU_CHART_PAUSE  3002
#define MENU_CHART_CLEAR  3003
#define MENU_CHART_WRITE  3004
#define MENU_CHART_CLOSE  3005

static OS_ChartWin* cw_from_hwnd(HWND hwnd)
{
    OS_ChartWin* cw = (OS_ChartWin*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (!cw || cw->magic != OS_MAGIC_CHART) return NULL;
    return cw;
}

void os_chart_push(HWND hwnd, const OS_Sample* s)
{
    OS_ChartWin* cw = cw_from_hwnd(hwnd);
    OS_Series* sr;
    int i;
    uint64_t now;
    if (!cw || !s || cw->paused) return;
    sr = NULL;
    for (i = 0; i < cw->series_count; i++) {
        if (cw->series[i].leaf_id == s->var_id) { sr = &cw->series[i]; break; }
    }
    if (!sr) return;
    sr->ts[sr->head] = s->ts_us;
    sr->val[sr->head] = s->value;
    sr->head = (sr->head + 1) % OS_CHART_HIST;
    if (sr->count < OS_CHART_HIST) sr->count++;
    now = GetTickCount64();
    if (now - cw->last_paint > 33) {
        cw->last_paint = now;
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

void os_chart_add_var(HWND hwnd, int leaf_id)
{
    OS_ChartWin* cw = cw_from_hwnd(hwnd);
    int i;
    if (!cw) return;
    for (i = 0; i < cw->series_count; i++) {
        if (cw->series[i].leaf_id == leaf_id) return;
    }
    if (cw->series_count >= OS_MAX_CHART_SERIES) return;
    memset(&cw->series[cw->series_count], 0, sizeof(OS_Series));
    cw->series[cw->series_count].leaf_id = leaf_id;
    cw->series[cw->series_count].color = g_pal[cw->series_count % 8];
    cw->series_count++;
    InvalidateRect(hwnd, NULL, TRUE);
}

static void chart_draw(OS_ChartWin* cw, HDC hdc)
{
    RECT rc, plot;
    int th = 26, i, j;
    double ymin = 1e300, ymax = -1e300;
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    wchar_t buf[320];
    GetClientRect(cw->hwnd, &rc);
    plot = rc;
    plot.top += th;
    /* 标题栏 */
    {
        HBRUSH br = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        FillRect(hdc, &rc, br);
        DeleteObject(br);
        FrameRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
        SelectObject(hdc, font);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
        RECT tr = { 4, 2, rc.right - 22, th - 2 };
        DrawTextW(hdc, cw->title, -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        RECT xr = { rc.right - 20, 2, rc.right - 4, th - 2 };
        DrawTextW(hdc, L"×", -1, &xr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    /* 绘图区背景 */
    {
        HBRUSH br = CreateSolidBrush(RGB(12, 12, 18));
        FillRect(hdc, &plot, br);
        DeleteObject(br);
    }
    /* 计算可见数据范围 */
    for (i = 0; i < cw->series_count; i++) {
        OS_Series* sr = &cw->series[i];
        int start = sr->count > cw->npoints ? sr->count - cw->npoints : 0;
        for (j = start; j < sr->count; j++) {
            int idx = (sr->head - sr->count + j) % OS_CHART_HIST;
            if (idx < 0) idx += OS_CHART_HIST;
            if (sr->val[idx] < ymin) ymin = sr->val[idx];
            if (sr->val[idx] > ymax) ymax = sr->val[idx];
        }
    }
    if (ymin > ymax) { ymin = -1.0; ymax = 1.0; }
    if (ymax - ymin < 1e-12) { ymax += 1.0; ymin -= 1.0; }
    {
        double pad = (ymax - ymin) * 0.08;
        ymin -= pad;
        ymax += pad;
    }
    /* 网格 */
    {
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(45, 45, 58));
        HPEN old = (HPEN)SelectObject(hdc, pen);
        int nx = 10, ny = 8;
        for (i = 1; i < nx; i++) {
            int x = plot.left + plot.right * i / nx;
            MoveToEx(hdc, x, plot.top, NULL);
            LineTo(hdc, x, plot.bottom);
        }
        for (i = 1; i < ny; i++) {
            int y = plot.top + (plot.bottom - plot.top) * i / ny;
            MoveToEx(hdc, plot.left, y, NULL);
            LineTo(hdc, plot.right, y);
        }
        SelectObject(hdc, old);
        DeleteObject(pen);
    }
    /* 坐标标注 */
    SelectObject(hdc, font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(150, 150, 160));
    {
        char txt[64];
        wchar_t wtxt[128];
        RECT r;
        for (i = 0; i <= 4; i++) {
            double v = ymax - (ymax - ymin) * i / 4.0;
            int y = plot.top + (plot.bottom - plot.top) * i / 4;
            _snprintf(txt, 64, "%.3g", v);
            os_utf8_to_wide_buf(txt, wtxt, 128);
            r.left = 2; r.top = y - 8; r.right = plot.left - 2; r.bottom = y + 8;
            DrawTextW(hdc, wtxt, -1, &r, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
    }
    /* 曲线 */
    for (i = 0; i < cw->series_count; i++) {
        OS_Series* sr = &cw->series[i];
        int start = sr->count > cw->npoints ? sr->count - cw->npoints : 0;
        int npts = sr->count - start;
        int first = 1;
        HPEN pen = CreatePen(PS_SOLID, 1, sr->color);
        HPEN old = (HPEN)SelectObject(hdc, pen);
        if (npts > 1) {
            for (j = 0; j < npts; j++) {
                int idx = (sr->head - sr->count + start + j) % OS_CHART_HIST;
                if (idx < 0) idx += OS_CHART_HIST;
                {
                    int x = plot.left + (plot.right - plot.left) * j / (npts - 1);
                    int y = plot.bottom - (int)((sr->val[idx] - ymin) / (ymax - ymin) * (plot.bottom - plot.top));
                    if (y < plot.top) y = plot.top;
                    if (y > plot.bottom) y = plot.bottom;
                    if (first) { MoveToEx(hdc, x, y, NULL); first = 0; }
                    else LineTo(hdc, x, y);
                }
            }
        }
        SelectObject(hdc, old);
        DeleteObject(pen);
    }
    /* 图例 */
    {
        int ly = plot.top + 4;
        for (i = 0; i < cw->series_count; i++) {
            OS_Series* sr = &cw->series[i];
            const OS_Leaf* L = os_vartree_leaf(sr->leaf_id);
            HPEN pen = CreatePen(PS_SOLID, 2, sr->color);
            HPEN old = (HPEN)SelectObject(hdc, pen);
            MoveToEx(hdc, plot.left + 8, ly + 6, NULL);
            LineTo(hdc, plot.left + 34, ly + 6);
            SelectObject(hdc, old);
            DeleteObject(pen);
            SetTextColor(hdc, sr->color);
            if (L) {
                char nm[300];
                wchar_t wnm[300], wv[80];
                _snprintf(nm, 300, "%s = %s", L->name, L->sample.text);
                os_utf8_to_wide_buf(nm, wnm, 300);
                os_utf8_to_wide_buf(L->sample.text, wv, 80);
                RECT r = { plot.left + 38, ly - 4, plot.left + 420, ly + 14 };
                DrawTextW(hdc, wnm, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            ly += 16;
            if (ly > plot.bottom - 8) break;
        }
    }
    (void)buf;
}

static LRESULT CALLBACK chart_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    OS_ChartWin* cw = cw_from_hwnd(hwnd);
    switch (msg) {
    case WM_NCCREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        OS_ChartWin* p = (OS_ChartWin*)calloc(1, sizeof(OS_ChartWin));
        if (!p) return FALSE;
        p->magic = OS_MAGIC_CHART;
        p->hwnd = hwnd;
        p->npoints = 600;
        if (cs->lpszName) _snwprintf(p->title, 128, L"%s", cs->lpszName);
        else _snwprintf(p->title, 128, L"波形窗口");
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)p);
        return TRUE;
    }
    case WM_NCDESTROY: {
        if (cw) free(cw);
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (cw) chart_draw(cw, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_LBUTTONDOWN: {
        if (cw) {
            int x = (short)LOWORD(lParam);
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (x >= rc.right - 22 && (short)HIWORD(lParam) < 26) {
                PostMessage(GetParent(hwnd), WM_OS_WIN_CLOSED, (WPARAM)hwnd, 0);
                return 0;
            }
        }
        return 0;
    }
    case WM_RBUTTONUP: {
        HMENU m = CreatePopupMenu();
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        AppendMenuW(m, MF_STRING, MENU_CHART_ADD, L"添加变量...");
        AppendMenuW(m, MF_STRING | (cw && cw->paused ? MF_CHECKED : 0), MENU_CHART_PAUSE, L"暂停刷新");
        AppendMenuW(m, MF_STRING, MENU_CHART_CLEAR, L"清除数据");
        AppendMenuW(m, MF_STRING, MENU_CHART_WRITE, L"写入值...");
        AppendMenuW(m, MF_SEPARATOR, 0, NULL);
        AppendMenuW(m, MF_STRING, MENU_CHART_CLOSE, L"关闭窗口");
        ClientToScreen(hwnd, &pt);
        TrackPopupMenu(m, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(m);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case MENU_CHART_ADD: {
            int id = -1;
            if (os_dlg_pick_var(hwnd, &id) == 0 && id >= 0) os_chart_add_var(hwnd, id);
            break;
        }
        case MENU_CHART_PAUSE:
            if (cw) { cw->paused = !cw->paused; }
            break;
        case MENU_CHART_CLEAR:
            if (cw) {
                int k;
                for (k = 0; k < cw->series_count; k++) {
                    cw->series[k].head = 0;
                    cw->series[k].count = 0;
                }
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        case MENU_CHART_WRITE:
            if (cw && cw->series_count > 0) {
                os_dlg_edit_value(hwnd, cw->series[cw->sel < cw->series_count ? cw->sel : 0].leaf_id);
            }
            break;
        case MENU_CHART_CLOSE:
            PostMessage(GetParent(hwnd), WM_OS_WIN_CLOSED, (WPARAM)hwnd, 0);
            break;
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void os_chart_register(void)
{
    WNDCLASSW wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = chart_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = g_chart_class;
    RegisterClassW(&wc);
}

HWND os_chart_create(HWND parent, int x, int y, int w, int h, const wchar_t* title)
{
    return CreateWindowW(g_chart_class, title ? title : L"波形窗口",
                         WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                         x, y, w, h, parent, NULL, g_app.hInst, NULL);
}
