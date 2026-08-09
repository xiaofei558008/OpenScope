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
    int stacked;        /* N13c: Ctrl+B 堆叠排列：1=每路一行（独立Y轴左置），0=全部叠加 */
    /* N13e/f/g: 光标 + Δ 测量 */
    POINT cursor;       /* 鼠标位置（客户区） */
    int hover_plot;     /* 鼠标在绘图区内 */
    POINT m0, m1;       /* 测量锚点1/2（客户区），x<0=未设置 */
    int64_t mt0, mt1;   /* 锚点时间 (us) */
    double mv0, mv1;    /* 锚点值 */
    int mark_sid;       /* 测量所用系列下标 */
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
#define MENU_CHART_REMOVE    3009

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
    /* N9(a): 加入窗口的变量自动纳入采集（观测勾选），否则 poll 线程
     * 只采集 watched 叶子，多变量显示恒为 0。 */
    os_vartree_set_watch(leaf_id, 1);
    os_vartree_set_check_ui(g_app.hTree, leaf_id, 1);
    os_mainwin_refresh_status();
    os_mainwin_update_buttons();
    InvalidateRect(hwnd, NULL, TRUE);
    os_log(OS_LOG_INFO, "波形窗口添加变量: id=%d (观测 %d)", leaf_id, g_app.watch_count);
}

/* N9(b): 移除第 idx 路变量；若该变量不再被任何窗口引用则自动取消观测 */
void os_chart_remove_var(HWND hwnd, int idx)
{
    OS_ChartWin* cw = cw_from_hwnd(hwnd);
    int removed_id, k;
    if (!cw || idx < 0 || idx >= cw->series_count) return;
    removed_id = cw->series[idx].leaf_id;
    for (k = idx; k < cw->series_count - 1; k++) cw->series[k] = cw->series[k + 1];
    cw->series_count--;
    if (cw->sel >= cw->series_count) cw->sel = cw->series_count - 1;
    os_log(OS_LOG_INFO, "波形窗口移除变量: id=%d (剩余 %d 路)", removed_id, cw->series_count);
    os_win_auto_unwatch(removed_id);
    InvalidateRect(hwnd, NULL, TRUE);
}

int os_chart_is(HWND hwnd)
{
    OS_ChartWin* cw = cw_from_hwnd(hwnd);
    return cw ? 1 : 0;
}

/* 图例命中：返回鼠标所指的系列下标（未命中返回 -1） */
static int chart_legend_hit(OS_ChartWin* cw, int x, int y)
{
    RECT rc;
    int plot_top, plot_bottom, ly, i;
    if (!cw) return -1;
    GetClientRect(cw->hwnd, &rc);
    plot_top = 26;
    plot_bottom = rc.bottom - 18;
    if (x < 56 || y < plot_top + 4 || y >= plot_bottom - 8) return -1;
    ly = plot_top + 4;
    for (i = 0; i < cw->series_count; i++) {
        if (y >= ly - 4 && y < ly + 14) return i;
        ly += 16;
        if (ly > plot_bottom - 8) break;
    }
    return -1;
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

/* ---------- N13 辅助：绘图区/坐标映射 ---------- */

/* 绘图区客户坐标（左 56px 为 Y 轴标注槽，底部 18px 为 X 轴标注） */
static void chart_plot_rect(OS_ChartWin* cw, RECT* plot)
{
    RECT rc;
    GetClientRect(cw->hwnd, &rc);
    *plot = rc;
    plot->top += 26;
    plot->left += 56;
    plot->bottom -= 18;
    plot->right -= 6;
}

static int map_x(const RECT* plot, int64_t x0, int64_t x1, int64_t t)
{
    int x;
    if (x1 <= x0) return plot->left + (plot->right - plot->left) / 2;
    x = plot->left + (int)((double)(plot->right - plot->left) * (double)(t - x0) / (double)(x1 - x0));
    if (x < plot->left) x = plot->left;
    if (x > plot->right) x = plot->right;
    return x;
}

static int map_y(const RECT* lane, double val, double ylo, double yhi)
{
    int y;
    if (yhi - ylo < 1e-12) yhi = ylo + 1.0;
    y = lane->bottom - (int)((val - ylo) / (yhi - ylo) * (lane->bottom - lane->top));
    if (y < lane->top) y = lane->top;
    if (y > lane->bottom) y = lane->bottom;
    return y;
}

/* 某系列时间上距 t 最近的样本下标（缓冲下标），无样本返回 0 */
static int chart_sample_at_time(const OS_Series* sr, int view_all, int npoints, int64_t t)
{
    int start = chart_vis_start(sr, view_all, npoints);
    int64_t best = INT64_MAX;
    int j, bi = -1;
    for (j = start; j < sr->count; j++) {
        int idx = (sr->head - sr->count + j) % OS_CHART_HIST;
        int64_t dt;
        if (idx < 0) idx += OS_CHART_HIST;
        dt = sr->ts[idx] - t;
        if (dt < 0) dt = -dt;
        if (dt < best) { best = dt; bi = idx; }
    }
    return (bi >= 0) ? bi : 0;
}

/* N13f: 叶变量的数据类型文本，如 uint32_t / int16_t / float / enum */
static void chart_type_name(const OS_Leaf* L, char* out, int cap)
{
    const char* base = "other";
    if (!L || cap <= 0) return;
    switch (L->kind) {
    case OS_TYPE_INT:  base = L->is_signed ? "int" : "uint"; break;
    case OS_TYPE_UINT: base = L->is_signed ? "int" : "uint"; break;
    case OS_TYPE_FLOAT: base = "float"; break;
    case OS_TYPE_BOOL: base = "bool"; break;
    case OS_TYPE_ENUM: base = "enum"; break;
    case OS_TYPE_STRING: base = "char"; break;
    case OS_TYPE_PTR: base = "ptr"; break;
    default: base = "other"; break;
    }
    if (L->kind == OS_TYPE_INT || L->kind == OS_TYPE_UINT) {
        int bits = (int)(L->size ? L->size : 4) * 8;
        _snprintf(out, cap, "%s%d_t%s%s", L->is_signed ? "int" : "uint", bits,
                  L->is_ptr ? "*" : "", L->is_bitfield ? ":bf" : "");
    } else {
        _snprintf(out, cap, "%s%s%s", base, L->is_ptr ? "*" : "",
                  L->is_bitfield ? ":bf" : "");
    }
}

/* 绘制一路曲线：折线 + （可见点少时）采样圆点 */
static void chart_draw_series(HDC hdc, OS_ChartWin* cw, OS_Series* sr,
                              const RECT* plot, const RECT* lane, int64_t x0, int64_t x1,
                              double ylo, double yhi, int have_t, int view_all)
{
    int start = chart_vis_start(sr, view_all, cw->npoints);
    int npts = sr->count - start;
    int j, first = 1;
    int vis_npts = 0;
    HPEN pen, old;
    int dots;
    /* Bug5 修复：圆点按“可见时间窗 [x0,x1] 内实际绘制的采样点数”判定，而非缓冲区总点数——
       否则录制时间越长 npts 越大，放大后圆点也全部消失。 */
    if (have_t && x1 > x0) {
        for (j = 0; j < npts; j++) {
            int idx = (sr->head - sr->count + start + j) % OS_CHART_HIST;
            if (idx < 0) idx += OS_CHART_HIST;
            if (sr->ts[idx] == 0 || sr->ts[idx] == -1) continue;
            if (sr->ts[idx] < x0 || sr->ts[idx] > x1) continue;
            vis_npts++;
        }
    } else {
        vis_npts = npts;
    }
    dots = (vis_npts > 0 && vis_npts <= 120); /* N13d/Bug5: 放大后采样点圆点 */
    {
        static int dots_was = 0; /* 状态变化才记日志，避免每帧刷屏 */
        if (dots) {
            if (!dots_was) os_log(OS_LOG_DEBUG, "波形采样点圆点: 可见 %d 点", vis_npts);
            dots_was = 1;
        } else {
            dots_was = 0;
        }
    }
    pen = CreatePen(PS_SOLID, 1, sr->color);
    old = (HPEN)SelectObject(hdc, pen);
    for (j = 0; j < npts; j++) {
        int idx = (sr->head - sr->count + start + j) % OS_CHART_HIST;
        int x, y;
        if (idx < 0) idx += OS_CHART_HIST;
        if (have_t) {
            int64_t t = sr->ts[idx];
            if (t == 0 || t == -1) { first = 1; continue; }
            if (t < x0 || t > x1) { first = 1; continue; }
            x = map_x(plot, x0, x1, t);
        } else {
            x = (npts > 1) ? plot->left + (plot->right - plot->left) * j / (npts - 1) : plot->left;
        }
        y = map_y(lane, sr->val[idx], ylo, yhi);
        if (first) { MoveToEx(hdc, x, y, NULL); first = 0; }
        else LineTo(hdc, x, y);
        if (dots) {
            HBRUSH br = CreateSolidBrush(sr->color);
            HBRUSH obr = (HBRUSH)SelectObject(hdc, br);
            Ellipse(hdc, x - 2, y - 2, x + 2, y + 2);
            SelectObject(hdc, obr);
            DeleteObject(br);
        }
    }
    SelectObject(hdc, old);
    DeleteObject(pen);
}

/* N13e/g: 在像素 (px,py) 处取时间 + 最近系列样本，设置测量锚点 */
static void chart_set_mark(OS_ChartWin* cw, const OS_ChartView* v, const RECT* plot, int px, int py)
{
    int64_t t;
    int i, best_i = -1;
    double best_d = 1e300, best_v = 0;
    int64_t best_t;
    if (!v->have_t || v->x1 <= v->x0) return;
    t = v->x0 + (int64_t)((double)(v->x1 - v->x0) * (px - plot->left) / (double)(plot->right - plot->left));
    best_t = t;
    for (i = 0; i < cw->series_count; i++) {
        OS_Series* sr = &cw->series[i];
        int idx = chart_sample_at_time(sr, cw->view_all, cw->npoints, t);
        double vv, d;
        int x, y;
        if (idx < 0 || idx >= OS_CHART_HIST || sr->count <= 0) continue;
        vv = sr->val[idx];
        x = map_x(plot, v->x0, v->x1, sr->ts[idx]);
        y = map_y(plot, vv, v->ylo, v->yhi);
        d = (double)(x - px) * (x - px) + (double)(y - py) * (y - py);
        if (d < best_d) { best_d = d; best_i = i; best_v = vv; best_t = sr->ts[idx]; }
    }
    if (best_i < 0) return;
    if (cw->m0.x >= 0 && cw->m1.x >= 0) { cw->m0.x = cw->m1.x = -1; }
    if (cw->m0.x < 0) {
        cw->m0.x = px; cw->m0.y = py;
        cw->mt0 = best_t; cw->mv0 = best_v; cw->mark_sid = best_i;
        os_log(OS_LOG_INFO, "波形测量标记1: t=%lldus 值=%g", (long long)best_t, best_v);
    } else {
        cw->m1.x = px; cw->m1.y = py;
        cw->mt1 = best_t; cw->mv1 = best_v;
        os_log(OS_LOG_INFO, "波形测量Δ: ΔX=%lldus ΔY=%g",
               (long long)(best_t - cw->mt0), best_v - cw->mv0);
    }
}

static void chart_draw(OS_ChartWin* cw, HDC hdc)
{
    RECT rc, plot;
    int th = 26, i, j;
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    OS_ChartView v;
    GetClientRect(cw->hwnd, &rc);
    plot = rc;
    plot.top += th;
    plot.left += 56;
    plot.bottom -= 18;
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
        /* Bug5: 不再绘制内部标题文字“波形窗口 N”（tab 标签已展示名称），保留右上 × */
        RECT xr = { rc.right - 20, 2, rc.right - 4, th - 2 };
        DrawTextW(hdc, L"×", -1, &xr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        if (cw->stacked) {
            RECT mr = { rc.right - 190, 2, rc.right - 22, th - 2 };
            DrawTextW(hdc, L"[逐行堆叠]", -1, &mr, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
    }
    /* 绘图区背景 */
    {
        HBRUSH br = CreateSolidBrush(RGB(12, 12, 18));
        FillRect(hdc, &plot, br);
        DeleteObject(br);
    }
    /* 网格：竖直网格共享；水平网格叠加模式全局 / 堆叠模式逐 lane */
    {
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(45, 45, 58));
        HPEN old = (HPEN)SelectObject(hdc, pen);
        int nx = 10;
        for (i = 1; i < nx; i++) {
            int x = plot.left + (plot.right - plot.left) * i / nx;
            MoveToEx(hdc, x, plot.top, NULL);
            LineTo(hdc, x, plot.bottom);
        }
        SelectObject(hdc, old);
        DeleteObject(pen);
        if (cw->stacked && cw->series_count > 0) {
            int lane_h = (plot.bottom - plot.top) / cw->series_count;
            for (i = 0; i < cw->series_count; i++) {
                RECT ln = plot;
                ln.top = plot.top + i * lane_h;
                ln.bottom = (i == cw->series_count - 1) ? plot.bottom : ln.top + lane_h;
                HPEN sep = CreatePen(PS_DOT, 1, RGB(82, 82, 98));
                HPEN os = (HPEN)SelectObject(hdc, sep);
                if (i > 0) { MoveToEx(hdc, ln.left, ln.top - 1, NULL); LineTo(hdc, ln.right, ln.top - 1); }
                for (j = 1; j < 3; j++) {
                    int yy = ln.top + (ln.bottom - ln.top) * j / 3;
                    MoveToEx(hdc, ln.left, yy, NULL);
                    LineTo(hdc, ln.right, yy);
                }
                SelectObject(hdc, os);
                DeleteObject(sep);
            }
        } else {
            HPEN hpen = CreatePen(PS_SOLID, 1, RGB(45, 45, 58));
            HPEN oh = (HPEN)SelectObject(hdc, hpen);
            for (i = 1; i < 8; i++) {
                int y = plot.top + (plot.bottom - plot.top) * i / 8;
                MoveToEx(hdc, plot.left, y, NULL);
                LineTo(hdc, plot.right, y);
            }
            SelectObject(hdc, oh);
            DeleteObject(hpen);
        }
    }
    /* Y 轴标注 + 曲线 */
    if (cw->stacked && cw->series_count > 0) {
        /* N13b/c: 堆叠模式：每路独立一行，独立 Y 轴放界面左侧 */
        int lane_h = (plot.bottom - plot.top) / cw->series_count;
        for (i = 0; i < cw->series_count; i++) {
            OS_Series* sr = &cw->series[i];
            const OS_Leaf* L = os_vartree_leaf(sr->leaf_id);
            double sy0, sy1;
            RECT ln = plot;
            ln.top = plot.top + i * lane_h;
            ln.bottom = (i == cw->series_count - 1) ? plot.bottom : ln.top + lane_h;
            chart_series_range(cw, sr, &sy0, &sy1);
            SelectObject(hdc, font);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, sr->color);
            for (j = 0; j <= 2; j++) {
                double val = sy1 - (sy1 - sy0) * j / 2.0;
                int yy = ln.top + (ln.bottom - ln.top) * j / 2;
                char txt[64];
                wchar_t wt[128];
                RECT r;
                _snprintf(txt, 64, "%.3g", val);
                os_utf8_to_wide_buf(txt, wt, 128);
                r.left = 2; r.top = yy - 8; r.right = plot.left - 2; r.bottom = yy + 8;
                DrawTextW(hdc, wt, -1, &r, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
            }
            if (L) {
                wchar_t wnm[340];
                char nm[300];
                RECT tr;
                _snprintf(nm, 300, "%s", L->name);
                os_utf8_to_wide_buf(nm, wnm, 340);
                tr.left = ln.left + 4; tr.top = ln.top + 1;
                tr.right = ln.left + 360; tr.bottom = ln.top + 15;
                DrawTextW(hdc, wnm, -1, &tr, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            chart_draw_series(hdc, cw, sr, &plot, &ln, v.x0, v.x1, sy0, sy1, v.have_t, cw->view_all);
        }
    } else {
        SelectObject(hdc, font);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(150, 150, 160));
        for (i = 0; i <= 4; i++) {
            double val = v.yhi - (v.yhi - v.ylo) * i / 4.0;
            int y = plot.top + (plot.bottom - plot.top) * i / 4;
            char txt[64];
            wchar_t wt[128];
            RECT r;
            _snprintf(txt, 64, "%.3g", val);
            os_utf8_to_wide_buf(txt, wt, 128);
            r.left = 2; r.top = y - 8; r.right = plot.left - 2; r.bottom = y + 8;
            DrawTextW(hdc, wt, -1, &r, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
        for (i = 0; i < cw->series_count; i++) {
            OS_Series* sr = &cw->series[i];
            chart_draw_series(hdc, cw, sr, &plot, &plot, v.x0, v.x1, v.ylo, v.yhi, v.have_t, cw->view_all);
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
    /* N13e: 测量标记 + Δ 读值 */
    if (cw->m0.x >= 0) {
        int sid = (cw->mark_sid >= 0 && cw->mark_sid < cw->series_count) ? cw->mark_sid : 0;
        COLORREF mc = cw->series[sid].color;
        HPEN mpen = CreatePen(PS_DASHDOT, 1, mc);
        HPEN old = (HPEN)SelectObject(hdc, mpen);
        SelectObject(hdc, font);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, mc);
        MoveToEx(hdc, cw->m0.x, plot.top, NULL);
        LineTo(hdc, cw->m0.x, plot.bottom);
        if (cw->m1.x >= 0) {
            MoveToEx(hdc, cw->m0.x, cw->m0.y, NULL);
            LineTo(hdc, cw->m1.x, cw->m1.y);
            MoveToEx(hdc, cw->m1.x, plot.top, NULL);
            LineTo(hdc, cw->m1.x, plot.bottom);
            {
                wchar_t wt[180];
                RECT r;
                _snwprintf(wt, 180, L"ΔX=%lldus  ΔY=%g",
                           (long long)(cw->mt1 - cw->mt0), cw->mv1 - cw->mv0);
                r.left = cw->m0.x + 8; r.top = plot.top + 4;
                r.right = cw->m0.x + 320; r.bottom = plot.top + 22;
                if (r.right > plot.right) { r.left = cw->m0.x - 320; r.right = cw->m0.x - 8; }
                DrawTextW(hdc, wt, -1, &r, DT_LEFT | DT_TOP | DT_SINGLELINE);
            }
        }
        SelectObject(hdc, old);
        DeleteObject(mpen);
    }
    /* N13f/g: 十字光标 + HUD 数值（同一 X 各系列 Y 值同时显示） */
    if (cw->hover_plot && cw->cursor.x >= plot.left && cw->cursor.x <= plot.right &&
        cw->cursor.y >= plot.top && cw->cursor.y <= plot.bottom) {
        int64_t t;
        HPEN cpen = CreatePen(PS_DOT, 1, RGB(190, 190, 210));
        HPEN old = (HPEN)SelectObject(hdc, cpen);
        MoveToEx(hdc, cw->cursor.x, plot.top, NULL);
        LineTo(hdc, cw->cursor.x, plot.bottom);
        MoveToEx(hdc, plot.left, cw->cursor.y, NULL);
        LineTo(hdc, plot.right, cw->cursor.y);
        SelectObject(hdc, old);
        DeleteObject(cpen);
        if (v.have_t && v.x1 > v.x0) {
            t = v.x0 + (int64_t)((double)(v.x1 - v.x0) * (cw->cursor.x - plot.left) / (double)(plot.right - plot.left));
            {
                char buf[900];
                int nlines = 0, ii;
                wchar_t wbuf[1000];
                RECT r;
                buf[0] = 0;
                for (ii = 0; ii < cw->series_count; ii++) {
                    OS_Series* sr = &cw->series[ii];
                    const OS_Leaf* L = os_vartree_leaf(sr->leaf_id);
                    int idx;
                    char line[340], tn[64];
                    if (!L || sr->count <= 0) continue;
                    idx = chart_sample_at_time(sr, cw->view_all, cw->npoints, t);
                    chart_type_name(L, tn, 64);
                    _snprintf(line, 340, "%s = %.6g (%s)", L->name, sr->val[idx], tn);
                    if (nlines < 8) {
                        if (nlines) strncat(buf, "\n", sizeof(buf) - strlen(buf) - 1);
                        strncat(buf, line, sizeof(buf) - strlen(buf) - 1);
                    }
                    nlines++;
                }
                if (buf[0]) {
                    int hpx = nlines * 14 + 8, wpx = 320;
                    int bx = cw->cursor.x + 12, by = cw->cursor.y + 12;
                    os_utf8_to_wide_buf(buf, wbuf, 1000);
                    if (bx + wpx > plot.right) bx = cw->cursor.x - wpx - 12;
                    if (by + hpx > plot.bottom) by = cw->cursor.y - hpx - 12;
                    r.left = bx; r.top = by; r.right = bx + wpx; r.bottom = by + hpx;
                    {
                        HBRUSH bgb = CreateSolidBrush(RGB(24, 26, 38));
                        HPEN bpen = CreatePen(PS_SOLID, 1, RGB(90, 95, 115));
                        HBRUSH ob = (HBRUSH)SelectObject(hdc, bgb);
                        HPEN obp = (HPEN)SelectObject(hdc, bpen);
                        Rectangle(hdc, r.left, r.top, r.right, r.bottom);
                        SelectObject(hdc, ob);
                        DeleteObject(bgb);
                        SelectObject(hdc, obp);
                        DeleteObject(bpen);
                    }
                    SetTextColor(hdc, RGB(220, 225, 235));
                    DrawTextW(hdc, wbuf, -1, &r, DT_LEFT | DT_TOP | DT_NOCLIP);
                }
            }
        }
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
    /* 图例（叠加模式；堆叠模式每 lane 已有标题） */
    if (!cw->stacked) {
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
        p->cursor.x = p->cursor.y = -1;  /* N13e/f/g: 初始无光标/测量标记 */
        p->m0.x = p->m1.x = -1;
        p->mark_sid = -1;
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
        /* Bug11: 双缓冲绘制——先画到内存 DC 再一次性 BitBlt，
         * 避免采集时鼠标在波形区域滑动（WM_MOUSEMOVE 高频重绘）整窗闪烁 */
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (cw) {
            RECT rc;
            HDC mdc;
            HBITMAP bmp, obmp;
            int w, h;
            GetClientRect(hwnd, &rc);
            w = rc.right - rc.left;
            h = rc.bottom - rc.top;
            if (w > 0 && h > 0) {
                mdc = CreateCompatibleDC(hdc);
                bmp = CreateCompatibleBitmap(hdc, w, h);
                obmp = (HBITMAP)SelectObject(mdc, bmp);
                chart_draw(cw, mdc);
                BitBlt(hdc, 0, 0, w, h, mdc, 0, 0, SRCCOPY);
                SelectObject(mdc, obmp);
                DeleteObject(bmp);
                DeleteDC(mdc);
            } else {
                chart_draw(cw, hdc);
            }
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_PRINT:
        if (cw) chart_draw(cw, (HDC)wParam);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_SETFOCUS:
        os_win_mark_active(hwnd); /* N11/Bug3: 登记为“当前窗口” */
        return 0;
    case WM_LBUTTONDBLCLK: {
        /* Bug3: 双击标题栏（非关闭按钮区域）切换全屏 */
        RECT rc;
        GetClientRect(hwnd, &rc);
        if ((short)HIWORD(lParam) < 26 && (short)LOWORD(lParam) < rc.right - 22)
            PostMessage(g_app.hMain, WM_OS_WIN_FULLSCREEN, (WPARAM)hwnd, 0);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        if (cw) {
            int x = (short)LOWORD(lParam);
            int y = (short)HIWORD(lParam);
            RECT rc, plot;
            int hit;
            GetClientRect(hwnd, &rc);
            if (x >= rc.right - 22 && y < 26) {
                PostMessage(g_app.hMain, WM_OS_WIN_CLOSED, (WPARAM)hwnd, 0);
                return 0;
            }
            /* N9(b): 点击图例选中对应系列 */
            hit = chart_legend_hit(cw, x, y);
            if (hit >= 0) {
                cw->sel = hit;
                InvalidateRect(hwnd, NULL, FALSE);
            } else {
                /* N13e: 绘图区内点击设置测量锚点（两次测量 Δ） */
                chart_plot_rect(cw, &plot);
                if (x >= plot.left && x <= plot.right && y >= plot.top && y <= plot.bottom) {
                    OS_ChartView v;
                    chart_compute_view(cw, &v);
                    chart_set_mark(cw, &v, &plot, x, y);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
            SetFocus(hwnd); /* 便于接收 F / Ctrl+B 快捷键 */
        }
        return 0;
    }
    case WM_MOUSEMOVE: {
        /* N13e/f/g: 记录光标位置，绘图区内显示十字光标 + HUD 数值 */
        if (cw) {
            RECT plot;
            int x = (short)LOWORD(lParam);
            int y = (short)HIWORD(lParam);
            TRACKMOUSEEVENT tme;
            cw->cursor.x = x;
            cw->cursor.y = y;
            chart_plot_rect(cw, &plot);
            cw->hover_plot = (x >= plot.left && x <= plot.right && y >= plot.top && y <= plot.bottom) ? 1 : 0;
            InvalidateRect(hwnd, NULL, FALSE);
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        if (cw) {
            cw->hover_plot = 0;
            cw->cursor.x = cw->cursor.y = -1;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    case WM_RBUTTONUP: {
        HMENU m = CreatePopupMenu();
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        int has_sel = 0;
        if (cw) {
            int hit = chart_legend_hit(cw, pt.x, pt.y);
            if (hit >= 0) {
                cw->sel = hit;
                InvalidateRect(hwnd, NULL, FALSE);
                has_sel = 1;
            } else if (cw->sel >= 0 && cw->sel < cw->series_count) {
                has_sel = 1;
            }
        }
        AppendMenuW(m, MF_STRING, MENU_CHART_ADD, L"添加变量...");
        AppendMenuW(m, MF_STRING | (has_sel ? 0 : MF_GRAYED), MENU_CHART_REMOVE, L"移除选中变量");
        AppendMenuW(m, MF_STRING | (cw && cw->paused ? MF_CHECKED : 0), MENU_CHART_PAUSE, L"暂停刷新");
        AppendMenuW(m, MF_STRING | (cw && cw->stacked ? MF_CHECKED : 0), MENU_CHART_MULTIAXIS, L"逐行堆叠 (Ctrl+B)");
        AppendMenuW(m, MF_STRING, MENU_CHART_FITALL, L"全局显示 (F)");
        AppendMenuW(m, MF_STRING, MENU_CHART_ZOOMRESET, L"重置缩放");
        AppendMenuW(m, MF_STRING, MENU_CHART_CLEAR, L"清除数据");
        AppendMenuW(m, MF_STRING | (has_sel ? 0 : MF_GRAYED), MENU_CHART_WRITE, L"写入值...");
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
                cw->m0.x = cw->m1.x = -1; /* 缩放后清除测量标记 */
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
            case 'B': case 'b': /* Ctrl+B：逐行堆叠 / 全部叠加 */
                if (GetKeyState(VK_CONTROL) & 0x8000) {
                    cw->stacked = !cw->stacked;
                    os_log(OS_LOG_DEBUG, "波形多坐标轴: %d", cw->stacked);
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
            /* N13a: 多选：一次添加全部选中变量 */
            int ids[OS_MAX_CHART_SERIES], n = 0, i;
            if (os_dlg_pick_vars(hwnd, ids, OS_MAX_CHART_SERIES, &n) == 0) {
                for (i = 0; i < n; i++) os_chart_add_var(hwnd, ids[i]);
                os_log(OS_LOG_INFO, "波形窗口批量添加变量: %d 个", n);
            }
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
                cw->stacked = !cw->stacked;
                os_log(OS_LOG_DEBUG, "波形多坐标轴: %d (菜单)", cw->stacked);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        case MENU_CHART_ZOOMRESET:
            if (cw) {
                cw->view_all = 0; cw->fit_x = 1; cw->fit_y = 1;
                cw->m0.x = cw->m1.x = -1;
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
        case MENU_CHART_REMOVE:
            if (cw && cw->sel >= 0 && cw->sel < cw->series_count) {
                os_chart_remove_var(hwnd, cw->sel);
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
