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

/* 当前可视视图：X 时间窗 + Y 值域 */
typedef struct OS_ChartView {
    int64_t x0, x1;     /* 可见时间窗 (us)，have_t=1 时有效 */
    int64_t full0, full1;/* 全部数据时间窗 (us) */
    int have_t;         /* 是否有可用时间戳 */
    int nvis;           /* 最大可见点数（无时间戳时按索引映射） */
    double ylo, yhi;    /* 可见 Y 值域（全局，含边距） */
    int have_data;      /* 是否有可见数据点 */
} OS_ChartView;

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
    /* N7: 视图/缩放状态 */
    int view_all;       /* 1 = 整体展示全部已录波形 */
    int fit_x;          /* 1 = X 自动(跟随最新)，0 = 手动 vx0/vx1 */
    int fit_y;          /* 1 = Y 自动缩放，0 = 手动 vylo/vyhi */
    int64_t vx0, vx1;   /* 手动 X 时间窗 (us) */
    double vylo, vyhi;  /* 手动 Y 值域 */
    int multiaxis;      /* Ctrl+B 多坐标轴：每路独立 Y 轴 */
} OS_ChartWin;

static const wchar_t* g_chart_class = L"OSChartWin";
static const COLORREF g_pal[] = {
    RGB(255, 220, 0), RGB(0, 230, 255), RGB(120, 255, 0), RGB(255, 80, 220),
    RGB(255, 90, 90), RGB(255, 170, 0), RGB(200, 200, 255), RGB(0, 255, 170)
};

#define MENU_CHART_ADD       3001
#define MENU_CHART_PAUSE     3002
#define MENU_CHART_CLEAR     3003
#define MENU_CHART_WRITE     3004
#define MENU_CHART_CLOSE     3005
#define MENU_CHART_FITALL    3006
#define MENU_CHART_MULTIAXIS 3007
#define MENU_CHART_ZOOMRESET 3008

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
    os_log(OS_LOG_DEBUG, "波形窗口添加变量: id=%d", leaf_id);
    InvalidateRect(hwnd, NULL, TRUE);
}

int os_chart_is(HWND hwnd)
{
    OS_ChartWin* cw = cw_from_hwnd(hwnd);
    return cw ? 1 : 0;
}

int os_chart_var_name(HWND hwnd, int idx, char* out, int cap)
{
    OS_ChartWin* cw = cw_from_hwnd(hwnd);
    const OS_Leaf* L;
    if (!cw || idx < 0 || idx >= cw->series_count || !out || cap <= 0) return 0;
    L = os_vartree_leaf(cw->series[idx].leaf_id);
    if (!L) return 0;
    _snprintf(out, cap, "%s", L->name);
    return 1;
}

static void fmt_time(double sec, wchar_t* out, int outlen)
{
    if (sec >= 60.0) _snwprintf(out, outlen, L"%dm%02ds", (int)(sec / 60.0), (int)sec % 60);
    else if (sec >= 1.0) _snwprintf(out, outlen, L"%.1fs", sec);
    else if (sec >= 0.001) _snwprintf(out, outlen, L"%.0fms", sec * 1000.0);
    else _snwprintf(out, outlen, L"0");
}

/* 某路数据在当前视图下可见的起始索引 */
static int chart_vis_start(const OS_Series* sr, int view_all, int npoints)
{
    if (view_all) return 0;
    return (sr->count > npoints) ? sr->count - npoints : 0;
}

/* 计算当前可视视图：X 时间窗 + 全局 Y 值域 */
static void chart_compute_view(OS_ChartWin* cw, OS_ChartView* v)
{
    int i, j;
    int64_t vis0 = INT64_MAX, vis1 = INT64_MIN;
    memset(v, 0, sizeof(*v));
    v->full0 = INT64_MAX; v->full1 = INT64_MIN;
    v->ylo = 1e300; v->yhi = -1e300;
    v->nvis = 0;
    for (i = 0; i < cw->series_count; i++) {
        OS_Series* sr = &cw->series[i];
        int start = chart_vis_start(sr, cw->view_all, cw->npoints);
        for (j = start; j < sr->count; j++) {
            int idx = (sr->head - sr->count + j) % OS_CHART_HIST;
            if (idx < 0) idx += OS_CHART_HIST;
            v->have_data = 1;
            if (sr->val[idx] < v->ylo) v->ylo = sr->val[idx];
            if (sr->val[idx] > v->yhi) v->yhi = sr->val[idx];
            if (sr->ts[idx] != 0 && sr->ts[idx] != -1) {
                v->have_t = 1;
                if (sr->ts[idx] < v->full0) v->full0 = sr->ts[idx];
                if (sr->ts[idx] > v->full1) v->full1 = sr->ts[idx];
                if (sr->ts[idx] < vis0) vis0 = sr->ts[idx];
                if (sr->ts[idx] > vis1) vis1 = sr->ts[idx];
            }
        }
        if (sr->count - start > v->nvis) v->nvis = sr->count - start;
    }
    /* 决定可见 X 时间窗 */
    if (v->have_t) {
        if (cw->view_all) { v->x0 = v->full0; v->x1 = v->full1; }
        else if (cw->fit_x) { v->x0 = vis0; v->x1 = vis1; }
        else { v->x0 = cw->vx0; v->x1 = cw->vx1; }
    }
    /* 决定 Y 值域（手动优先，否则自动 + 8% 边距） */
    if (!cw->fit_y) { v->ylo = cw->vylo; v->yhi = cw->vyhi; }
    if (!v->have_data) { v->ylo = -1.0; v->yhi = 1.0; }
    if (v->yhi - v->ylo < 1e-12) { v->yhi += 1.0; v->ylo -= 1.0; }
    else if (cw->fit_y) {
        double pad = (v->yhi - v->ylo) * 0.08;
        v->ylo -= pad; v->yhi += pad;
    }
}

/* 计算某路在可见区间的独立 Y 值域（Ctrl+B 多坐标轴） */
static void chart_series_range(OS_ChartWin* cw, const OS_Series* sr, double* ylo, double* yhi)
{
    int start = chart_vis_start(sr, cw->view_all, cw->npoints);
    int j, idx;
    *ylo = 1e300; *yhi = -1e300;
    for (j = start; j < sr->count; j++) {
        idx = (sr->head - sr->count + j) % OS_CHART_HIST;
        if (idx < 0) idx += OS_CHART_HIST;
        if (sr->val[idx] < *ylo) *ylo = sr->val[idx];
        if (sr->val[idx] > *yhi) *yhi = sr->val[idx];
    }
    if (*yhi - *ylo < 1e-12) { *yhi += 1.0; *ylo -= 1.0; }
    else { double pad = (*yhi - *ylo) * 0.08; *ylo -= pad; *yhi += pad; }
}

static void chart_draw(OS_ChartWin* cw, HDC hdc)
{
    RECT rc, plot;
    int th = 26, lw = 56, xh = 18, i, j;
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    OS_ChartView v;
    GetClientRect(cw->hwnd, &rc);
    plot = rc;
    plot.top += th;
    plot.left += lw;
    plot.bottom -= xh;
    plot.right -= 6;
    chart_compute_view(cw, &v);
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
        if (cw->multiaxis) {
            RECT mr = { rc.right - 190, 2, rc.right - 22, th - 2 };
            DrawTextW(hdc, L"[多坐标轴]", -1, &mr, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
    }
    /* 绘图区背景 */
    {
        HBRUSH br = CreateSolidBrush(RGB(12, 12, 18));
        FillRect(hdc, &plot, br);
        DeleteObject(br);
    }
    /* 网格 */
    {
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(45, 45, 58));
        HPEN old = (HPEN)SelectObject(hdc, pen);
        int nx = 10, ny = 8;
        for (i = 1; i < nx; i++) {
            int x = plot.left + (plot.right - plot.left) * i / nx;
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
            double val = v.yhi - (v.yhi - v.ylo) * i / 4.0;
            int y = plot.top + (plot.bottom - plot.top) * i / 4;
            _snprintf(txt, 64, "%.3g", val);
            os_utf8_to_wide_buf(txt, wtxt, 128);
            r.left = 2; r.top = y - 8; r.right = plot.left - 2; r.bottom = y + 8;
            DrawTextW(hdc, wtxt, -1, &r, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
    }
    /* 时间轴（底部） */
    {
        RECT r;
        double dur = v.have_t ? (double)(v.x1 - v.x0) / 1e6 : (double)v.nvis;
        if (dur < 0) dur = 0;
        SetTextColor(hdc, RGB(150, 150, 160));
        for (i = 0; i <= 4; i++) {
            int x = plot.left + (plot.right - plot.left) * i / 4;
            double sec = dur * i / 4.0;
            wchar_t wt[64];
            fmt_time(sec, wt, 64);
            r.left = x - 40; r.top = plot.bottom + 1;
            r.right = x + 40; r.bottom = rc.bottom - 1;
            DrawTextW(hdc, wt, -1, &r, DT_CENTER | DT_TOP | DT_SINGLELINE);
        }
    }
    /* 曲线 */
    for (i = 0; i < cw->series_count; i++) {
        OS_Series* sr = &cw->series[i];
        int start = chart_vis_start(sr, cw->view_all, cw->npoints);
        int npts = sr->count - start;
        double s_ylo = v.ylo, s_yhi = v.yhi;
        int first = 1;
        HPEN pen, old;
        if (cw->multiaxis) chart_series_range(cw, sr, &s_ylo, &s_yhi);
        pen = CreatePen(PS_SOLID, 1, sr->color);
        old = (HPEN)SelectObject(hdc, pen);
        if (npts > 0) {
            for (j = 0; j < npts; j++) {
                int idx = (sr->head - sr->count + start + j) % OS_CHART_HIST;
                int x, y;
                if (idx < 0) idx += OS_CHART_HIST;
                if (v.have_t) {
                    int64_t t = sr->ts[idx];
                    int64_t denom = v.x1 - v.x0;
                    if (t == 0 || t == -1) { first = 1; continue; }
                    if (t < v.x0 || t > v.x1) { first = 1; continue; }
                    if (denom > 0)
                        x = plot.left + (int)((double)(plot.right - plot.left) * (double)(t - v.x0) / (double)denom);
                    else
                        x = plot.left + (plot.right - plot.left) / 2;
                } else {
                    x = (npts > 1) ? plot.left + (plot.right - plot.left) * j / (npts - 1) : plot.left;
                }
                if (x < plot.left) x = plot.left;
                if (x > plot.right) x = plot.right;
                y = plot.bottom - (int)((sr->val[idx] - s_ylo) / (s_yhi - s_ylo) * (plot.bottom - plot.top));
                if (y < plot.top) y = plot.top;
                if (y > plot.bottom) y = plot.bottom;
                if (first) { MoveToEx(hdc, x, y, NULL); first = 0; }
                else LineTo(hdc, x, y);
            }
        }
        SelectObject(hdc, old);
        DeleteObject(pen);
    }
    /* 无数据提示 */
    {
        int total = 0;
        wchar_t wt[128];
        RECT r;
        for (i = 0; i < cw->series_count; i++) total += cw->series[i].count;
        if (total == 0 && cw->series_count > 0) {
            SetTextColor(hdc, RGB(130, 140, 155));
            _snwprintf(wt, 128, L"等待采集数据…（请连接 MCU 并开始采集）");
            r = plot;
            DrawTextW(hdc, wt, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
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
        p->fit_x = 1;  /* 新建窗口默认跟随最新（与暂停态相反），保证 X 时间窗有效 */
        p->fit_y = 1;
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
    case WM_PRINT:
        if (cw) chart_draw(cw, (HDC)wParam);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_LBUTTONDOWN: {
        if (cw) {
            int x = (short)LOWORD(lParam);
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (x >= rc.right - 22 && (short)HIWORD(lParam) < 26) {
                PostMessage(g_app.hMain, WM_OS_WIN_CLOSED, (WPARAM)hwnd, 0);
                return 0;
            }
            SetFocus(hwnd); /* 便于接收 F / Ctrl+B 快捷键 */
        }
        return 0;
    }
    case WM_RBUTTONUP: {
        HMENU m = CreatePopupMenu();
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        AppendMenuW(m, MF_STRING, MENU_CHART_ADD, L"添加变量...");
        AppendMenuW(m, MF_STRING | (cw && cw->paused ? MF_CHECKED : 0), MENU_CHART_PAUSE, L"暂停刷新");
        AppendMenuW(m, MF_STRING | (cw && cw->multiaxis ? MF_CHECKED : 0), MENU_CHART_MULTIAXIS, L"多坐标轴 (Ctrl+B)");
        AppendMenuW(m, MF_STRING, MENU_CHART_FITALL, L"全局显示 (F)");
        AppendMenuW(m, MF_STRING, MENU_CHART_ZOOMRESET, L"重置缩放");
        AppendMenuW(m, MF_STRING, MENU_CHART_CLEAR, L"清除数据");
        AppendMenuW(m, MF_STRING, MENU_CHART_WRITE, L"写入值...");
        AppendMenuW(m, MF_SEPARATOR, 0, NULL);
        AppendMenuW(m, MF_STRING, MENU_CHART_CLOSE, L"关闭窗口");
        ClientToScreen(hwnd, &pt);
        TrackPopupMenu(m, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(m);
        return 0;
    }
    case WM_MOUSEWHEEL: {
        /* 滚轮缩放 X 轴（围绕鼠标位置），Ctrl+滚轮缩放 Y 轴 */
        short delta = (short)HIWORD(wParam);
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        ScreenToClient(hwnd, &pt);
        if (cw) {
            RECT rc, plot;
            GetClientRect(hwnd, &rc);
            plot = rc;
            plot.top += 26; plot.left += 56; plot.bottom -= 18; plot.right -= 6;
            if (pt.x >= plot.left && pt.x <= plot.right &&
                pt.y >= plot.top && pt.y <= plot.bottom) {
                OS_ChartView v;
                double factor = (delta > 0) ? 0.8 : 1.25;
                chart_compute_view(cw, &v);
                if (LOWORD(wParam) & MK_CONTROL) {
                    /* Ctrl+滚轮：Y 轴缩放 */
                    if (!v.have_data) return 0;
                    {
                        double f = (double)(plot.bottom - pt.y) / (plot.bottom - plot.top);
                        if (f < 0.0) f = 0.0;
                        if (f > 1.0) f = 1.0;
                        double span = (v.yhi - v.ylo) * factor;
                        if (span < 1e-12) span = 1e-12;
                        cw->vylo = v.ylo + f * ((v.yhi - v.ylo) - span);
                        cw->vyhi = cw->vylo + span;
                        cw->fit_y = 0;
                        os_log(OS_LOG_DEBUG, "波形 Y 轴缩放: [%g,%g]", cw->vylo, cw->vyhi);
                    }
                } else {
                    /* X 轴缩放 */
                    if (v.have_t && v.x1 > v.x0) {
                        double f = (double)(pt.x - plot.left) / (plot.right - plot.left);
                        if (f < 0.0) f = 0.0;
                        if (f > 1.0) f = 1.0;
                        double span = (double)(v.x1 - v.x0) * factor;
                        if (span < 1000.0) span = 1000.0; /* 不小于 1ms */
                        int64_t nx0 = v.x0 + (int64_t)(f * ((double)(v.x1 - v.x0) - span));
                        int64_t nx1 = nx0 + (int64_t)span;
                        if (v.full0 != INT64_MAX) { if (nx0 < v.full0) nx0 = v.full0; }
                        if (v.full1 != INT64_MIN) { if (nx1 > v.full1) nx1 = v.full1; }
                        if (nx1 > nx0) { cw->vx0 = nx0; cw->vx1 = nx1; cw->fit_x = 0; }
                        os_log(OS_LOG_DEBUG, "波形 X 轴缩放: [%lld,%lld] us",
                               (long long)cw->vx0, (long long)cw->vx1);
                    }
                }
                cw->view_all = 0; /* 缩放退出整体展示 */
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        return 0;
    }
    case WM_KEYDOWN:
        if (cw) {
            switch (wParam) {
            case 'F': case 'f': /* 全局显示：整体展示全部波形 */
                cw->view_all = 1; cw->fit_x = 1; cw->fit_y = 1;
                os_log(OS_LOG_DEBUG, "波形全局显示 (F)");
                InvalidateRect(hwnd, NULL, TRUE);
                break;
            case 'B': case 'b': /* Ctrl+B 多坐标轴 */
                if (GetKeyState(VK_CONTROL) & 0x8000) {
                    cw->multiaxis = !cw->multiaxis;
                    os_log(OS_LOG_DEBUG, "波形多坐标轴: %d", cw->multiaxis);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                break;
            }
        }
        return 0;
    case WM_OS_CHART_FITALL:
        if (cw) {
            cw->view_all = 1; cw->fit_x = 1; cw->fit_y = 1;
            cw->paused = 1;
            os_log(OS_LOG_DEBUG, "波形整体展示 (停止采集)");
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    case WM_OS_CHART_LIVE:
        if (cw) {
            cw->view_all = 0; cw->fit_x = 1; cw->fit_y = 1;
            cw->paused = 0;
            os_log(OS_LOG_DEBUG, "波形跟随最新 (开始采集)");
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case MENU_CHART_ADD: {
            int id = -1;
            if (os_dlg_pick_var(hwnd, &id) == 0 && id >= 0) os_chart_add_var(hwnd, id);
            break;
        }
        case MENU_CHART_PAUSE:
            if (cw) {
                cw->paused = !cw->paused;
                /* 停止记录后整体展示整个波形；恢复后回到跟随最新 */
                cw->view_all = cw->paused ? 1 : 0;
                if (!cw->paused) { cw->fit_x = 1; cw->fit_y = 1; }
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        case MENU_CHART_FITALL:
            if (cw) {
                cw->view_all = 1; cw->fit_x = 1; cw->fit_y = 1;
                os_log(OS_LOG_DEBUG, "波形全局显示 (菜单)");
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        case MENU_CHART_MULTIAXIS:
            if (cw) {
                cw->multiaxis = !cw->multiaxis;
                os_log(OS_LOG_DEBUG, "波形多坐标轴: %d (菜单)", cw->multiaxis);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        case MENU_CHART_ZOOMRESET:
            if (cw) {
                cw->view_all = 0; cw->fit_x = 1; cw->fit_y = 1;
                InvalidateRect(hwnd, NULL, TRUE);
            }
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
            PostMessage(g_app.hMain, WM_OS_WIN_CLOSED, (WPARAM)hwnd, 0);
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
                         WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP,
                         x, y, w, h, parent, NULL, g_app.hInst, NULL);
}
