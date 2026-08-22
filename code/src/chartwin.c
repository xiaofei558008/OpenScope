#include "app.h"
#include "chartwin.h"
#include "mainwin.h"
#include "vartree.h"
#include "theme.h"
#include "chartview.h"
#include <string.h>

#define OS_MAGIC_CHART 0x43484152u /* 'CHAR' */

typedef struct OS_Series {
    int leaf_id;
    char name[256];     /* 添加时的变量全名：ELF 重载后按名重绑（需求2），防止叶下标漂移绑错变量 */
    COLORREF color;
    int64_t ts[OS_CHART_HIST];
    double val[OS_CHART_HIST];
    int head, count;
    /* 回放全量桶缓存（长时间采集落盘 CSV 的"全部显示"数据源）。
     * 存在时优先渲染桶 min/max 包络，覆盖整个文件时间跨度；
     * RAM 环仅用于实时采集/实时回放。 */
    OS_Bucket* buckets;
    int nbuckets;
    int64_t b_t0, b_t1;
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
    /* F23: 平滑缩放动画（X/Y 独立目标，逐帧插值消除跳变）。
     * X 缩放不动 Y、Y 缩放不动 X：避免滚轮 X 缩放破坏手动 Y 缩放 / 向陈旧 Y 值乱动。 */
    int anim_on;        /* 动画定时器运行中 */
    int anim_x_on;      /* X 轴动画进行中 */
    int anim_y_on;      /* Y 轴动画进行中 */
    int64_t anim_x0, anim_x1;  /* 动画目标 X 时间窗 (us) */
    double anim_y0, anim_y1;   /* 动画目标 Y 值域 */
    /* F23: 左键拖拽平移（超过阈值进入平移，否则视为单击测量标记） */
    POINT drag0;        /* 按下起点（客户区） */
    int64_t drag_x0, drag_x1;  /* 拖拽起点视图 X (us) */
    double drag_y0, drag_y1;   /* 拖拽起点视图 Y */
    int64_t drag_full0, drag_full1; /* 拖拽起点全量数据时间窗：平移夹紧边界（不拖出数据范围） */
    int dragging;       /* 1=正在平移拖拽（位移已超阈值） */
    /* F23: Ctrl+左键框选局部放大 */
    int boxing;         /* 1=框选进行中 */
    POINT box0, box1;   /* 框选矩形（客户区） */
} OS_ChartWin;

#define CHART_ANIM_TIMER 5  /* F23: 平滑缩放动画定时器 id */

/* F23: 手动视图操作（滚轮缩放/框选）前，把当前“自动视图”同步进 vx0/vx1/vylo/vyhi，
 * 保证动画/平移从当前实际显示的画面起步，而不是从陈旧的 0 值起跳（跳变根因 A）。 */
static void chart_sync_manual(OS_ChartWin* cw, const OS_ChartView* v)
{
    if (cw->fit_x) { cw->vx0 = v->x0; cw->vx1 = v->x1; }
    if (cw->fit_y) { cw->vylo = v->ylo; cw->vyhi = v->yhi; }
}

/* F23: 启动平滑缩放动画——X 轴目标（只动 X，Y 保持自动/手动不被打扰，跳变根因 B）。 */
static void chart_anim_to_x(OS_ChartWin* cw, int64_t x0, int64_t x1)
{
    cw->anim_x0 = x0; cw->anim_x1 = x1;
    cw->anim_x_on = 1;
    if (!cw->anim_on) {
        cw->anim_on = 1;
        SetTimer(cw->hwnd, CHART_ANIM_TIMER, 16, NULL);
    }
}

/* F23: 启动平滑缩放动画——Y 轴目标（只动 Y）。 */
static void chart_anim_to_y(OS_ChartWin* cw, double y0, double y1)
{
    cw->anim_y0 = y0; cw->anim_y1 = y1;
    cw->anim_y_on = 1;
    if (!cw->anim_on) {
        cw->anim_on = 1;
        SetTimer(cw->hwnd, CHART_ANIM_TIMER, 16, NULL);
    }
}

/* 用户反馈优化：X 轴滚动条（WS_HSCROLL）与视图双向同步。
 *  - 手动 X 模式（fit_x=0 且放大到小于全量）→ 显示滚动条：thumb 位置=可见窗起点，
 *    page=可见窗宽（占全量比例），拖拽 thumb 平移时间窗到想观察的点；
 *  - 跟随模式（fit_x=1）/ 全局显示 → 隐藏滚动条（视图自动管理）。
 * 滚动条尺度：0..1000 整数刻度映射 full0..full1，避免 int64 µs 超出滚动条 int 范围。 */
#define SCROLL_SCALE 1000

static void chart_sync_scrollbar(OS_ChartWin* cw, const OS_ChartView* v)
{
    SCROLLINFO si;
    int show = 0;
    int64_t full_span, view_span;
    int pos, page;
    if (v->have_t && v->full1 > v->full0 && !cw->fit_x && !cw->view_all) {
        full_span = v->full1 - v->full0;
        view_span = v->x1 - v->x0;
        if (view_span > 0 && view_span < full_span) show = 1;
    }
    ShowScrollBar(cw->hwnd, SB_HORZ, show);
    if (!show) return;
    full_span = v->full1 - v->full0;
    view_span = v->x1 - v->x0;
    pos = (int)((v->x0 - v->full0) * SCROLL_SCALE / full_span);
    page = (int)(view_span * SCROLL_SCALE / full_span);
    if (page < 1) page = 1;
    if (pos < 0) pos = 0;
    if (pos > SCROLL_SCALE - page) pos = SCROLL_SCALE - page;
    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = SCROLL_SCALE;
    si.nPage = (UINT)page;
    si.nPos = pos;
    SetScrollInfo(cw->hwnd, SB_HORZ, &si, TRUE);
}

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
    const OS_Leaf* L;
    int i;
    if (!cw) return;
    for (i = 0; i < cw->series_count; i++) {
        if (cw->series[i].leaf_id == leaf_id) return;
    }
    if (cw->series_count >= OS_MAX_CHART_SERIES) return;
    L = os_vartree_leaf(leaf_id);
    memset(&cw->series[cw->series_count], 0, sizeof(OS_Series));
    cw->series[cw->series_count].leaf_id = leaf_id;
    if (L) _snprintf(cw->series[cw->series_count].name, 256, "%s", L->name);
    cw->series[cw->series_count].color = g_pal[cw->series_count % 8];
    /* 回放全量桶缓存已加载 → 新系列直接挂接并整体展示（全部显示） */
    if (leaf_id >= 0 && leaf_id < OS_MAX_LEAVES &&
        g_app.buckets[leaf_id].b && g_app.buckets[leaf_id].nb > 0) {
        cw->series[cw->series_count].buckets = g_app.buckets[leaf_id].b;
        cw->series[cw->series_count].nbuckets = g_app.buckets[leaf_id].nb;
        cw->series[cw->series_count].b_t0 = g_app.buckets[leaf_id].t0;
        cw->series[cw->series_count].b_t1 = g_app.buckets[leaf_id].t1;
        cw->view_all = 1;
        cw->fit_x = 1;
        cw->fit_y = 1;
        os_log(OS_LOG_INFO, "波形桶缓存: %s %d 桶 [%lld,%lld]us",
               cw->series[cw->series_count].name,
               cw->series[cw->series_count].nbuckets,
               (long long)cw->series[cw->series_count].b_t0,
               (long long)cw->series[cw->series_count].b_t1);
    }
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

/* 需求2：ELF 重新加载后叶表重建、叶下标可能漂移——按变量全名重绑 leaf_id。
 * 旧实现只存下标，重编译增删变量后窗口会静默绑到错误变量（显示错地址/错数据）。
 * 缺失变量的系列移除；重绑后清空历史（地址可能已变，旧样本不再可比）。 */
void os_chart_rebind(HWND hwnd)
{
    OS_ChartWin* cw = cw_from_hwnd(hwnd);
    int i, ok = 0, miss = 0;
    if (!cw) return;
    for (i = 0; i < cw->series_count; ) {
        int id;
        if (!cw->series[i].name[0]) { i++; continue; }
        id = os_vartree_find_by_name(cw->series[i].name);
        if (id >= 0) {
            const OS_Leaf* L = os_vartree_leaf(id);
            if (id != cw->series[i].leaf_id)
                os_log(OS_LOG_INFO, "波形变量重绑: %s id=%d->%d @0x%llX",
                       cw->series[i].name, cw->series[i].leaf_id, id,
                       (unsigned long long)(L ? L->address : 0));
            cw->series[i].leaf_id = id;
            cw->series[i].head = 0;
            cw->series[i].count = 0;
            /* 重绑后旧桶缓存作废（加载新文件时重新挂接） */
            cw->series[i].buckets = NULL;
            cw->series[i].nbuckets = 0;
            ok++;
            i++;
        } else {
            int k;
            os_log(OS_LOG_WARN, "波形变量重绑缺失（移除）: %s", cw->series[i].name);
            for (k = i; k < cw->series_count - 1; k++) cw->series[k] = cw->series[k + 1];
            cw->series_count--;
            miss++;
        }
    }
    if (cw->sel >= cw->series_count) cw->sel = cw->series_count - 1;
    if (ok || miss) {
        /* 观测勾选由 os_vartree_build 按名恢复，无需再处理 */
        os_log(OS_LOG_INFO, "波形窗口变量重绑: 成功 %d 缺失 %d", ok, miss);
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

int os_chart_is(HWND hwnd)
{
    OS_ChartWin* cw = cw_from_hwnd(hwnd);
    return cw ? 1 : 0;
}

/* 回放全量加载后挂接桶缓存（按叶 id 找到系列）。清空历史环，
 * 波形从此以桶包络渲染整个文件时间跨度。 */
void os_chart_attach_buckets(HWND hwnd, int leaf_id, OS_Bucket* b, int nb,
                             int64_t t0, int64_t t1)
{
    OS_ChartWin* cw = cw_from_hwnd(hwnd);
    int i;
    if (!cw || !b || nb <= 0 || t1 <= t0) return;
    for (i = 0; i < cw->series_count; i++) {
        if (cw->series[i].leaf_id == leaf_id) {
            cw->series[i].buckets = b;
            cw->series[i].nbuckets = nb;
            cw->series[i].b_t0 = t0;
            cw->series[i].b_t1 = t1;
            cw->series[i].head = 0;
            cw->series[i].count = 0; /* 环让位给桶 */
            cw->view_all = 1;  /* 整体展示整个文件跨度 */
            cw->fit_x = 1;
            cw->fit_y = 1;
            os_log(OS_LOG_INFO, "波形桶缓存: %s %d 桶 [%lld,%lld]us",
                   cw->series[i].name, nb, (long long)t0, (long long)t1);
        }
    }
    InvalidateRect(hwnd, NULL, TRUE);
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

/* 某路数据在当前视图下可见的起始索引。
 * 用户反馈修复：手动视图（fit_x=0）与全局视图（view_all=1）必须覆盖【全部历史】——
 * 旧实现手动缩放后仍只扫"最后 npoints 点"，缩放/平移出最后600点窗口后曲线直接消失、
 * Y 值域/HUD/测量全部取错窗口。只有"跟随最新"（fit_x=1）才限定最后 npoints 点。 */
static int chart_vis_start(const OS_Series* sr, int view_all, int fit_x, int npoints)
{
    if (view_all || !fit_x) return 0;
    return (sr->count > npoints) ? sr->count - npoints : 0;
}

/* 计算当前可视视图：X 时间窗 + 全局 Y 值域 */
static void chart_compute_view(OS_ChartWin* cw, OS_ChartView* v)
{
    int i, j;
    int64_t vis0 = INT64_MAX, vis1 = INT64_MIN;
    double wylo = 1e300, wyhi = -1e300;  /* 手动 X 窗内 Y 值域（fit_x=0 时覆盖旧行为） */
    int have_wy = 0;
    memset(v, 0, sizeof(*v));
    v->full0 = INT64_MAX; v->full1 = INT64_MIN;
    v->ylo = 1e300; v->yhi = -1e300;
    v->nvis = 0;
    for (i = 0; i < cw->series_count; i++) {
        OS_Series* sr = &cw->series[i];
        int start = chart_vis_start(sr, cw->view_all, cw->fit_x, cw->npoints);
        int64_t scan_hi;  /* 扫描上界（手动窗右沿提前退出；跟随/全局无上界） */
        if (sr->count <= 0 && !(sr->buckets && sr->nbuckets > 0)) continue;
        /* 回放全量桶缓存：全量范围 = 桶时间跨度（环为空时也有效） */
        if (sr->buckets && sr->nbuckets > 0 && sr->b_t1 > sr->b_t0) {
            v->have_t = 1;
            v->have_data = 1;
            if (sr->b_t0 < v->full0) v->full0 = sr->b_t0;
            if (sr->b_t1 > v->full1) v->full1 = sr->b_t1;
        }
        /* O(1) 全量数据范围：环形缓冲按时间追加，最旧样本=最小 ts、最新=最大 ts。
         * 用户反馈修复（关键）：full0/full1 必须是【全部】样本的数据范围——滚轮缩放/
         * 框选/拖拽的夹紧边界。旧实现与可见窗口（最后 npoints=600 点）混用：录制超过
         * 600 点后 full 被截断到最后 600 点窗口，任何缩放/平移都被夹在那个小窗内——
         * "停止采集后只能显示一小段波形、不能拖拽"的根因。 */
        if (sr->count > 0) {
            int oidx = (sr->head - sr->count) % OS_CHART_HIST;
            int nidx = (sr->head - 1 + OS_CHART_HIST) % OS_CHART_HIST;
            int64_t ot, nt;
            if (oidx < 0) oidx += OS_CHART_HIST;
            ot = sr->ts[oidx];
            nt = sr->ts[nidx];
            if (ot != 0 && ot != -1) {
                v->have_t = 1;
                if (ot < v->full0) v->full0 = ot;
            }
            if (nt != 0 && nt != -1) {
                v->have_t = 1;
                if (nt > v->full1) v->full1 = nt;
            }
        }
        scan_hi = INT64_MAX;
        if (!cw->fit_x && !cw->view_all) scan_hi = cw->vx1;
        for (j = start; j < sr->count; j++) {
            int idx = (sr->head - sr->count + j) % OS_CHART_HIST;
            int64_t t;
            if (idx < 0) idx += OS_CHART_HIST;
            v->have_data = 1;
            t = sr->ts[idx];
            if (t != 0 && t != -1) {
                v->have_t = 1;
                if (t < vis0) vis0 = t;
                if (t > vis1) vis1 = t;
                if (t > scan_hi) break;  /* 时间有序：越过手动窗右沿即结束扫描 */
            }
            if (sr->val[idx] < v->ylo) v->ylo = sr->val[idx];
            if (sr->val[idx] > v->yhi) v->yhi = sr->val[idx];
            /* 手动 X 窗（fit_x=0 且非全局）：另统计窗内 Y 值域，缩放后 Y 刻度跟随可见段 */
            if (!cw->fit_x && !cw->view_all && t != 0 && t != -1 &&
                t >= cw->vx0 && t <= cw->vx1) {
                if (sr->val[idx] < wylo) wylo = sr->val[idx];
                if (sr->val[idx] > wyhi) wyhi = sr->val[idx];
                have_wy = 1;
            }
        }
        if (sr->count - start > v->nvis) v->nvis = sr->count - start;
    }
    /* 手动 X 窗内有样本 → Y 值域跟随可见段（旧行为用"最后600点"的 Y，缩放后刻度错乱） */
    if (have_wy) { v->ylo = wylo; v->yhi = wyhi; }
    /* 决定可见 X 时间窗 */
    if (v->have_t) {
        if (cw->view_all) { v->x0 = v->full0; v->x1 = v->full1; }
        else if (cw->fit_x) { v->x0 = vis0; v->x1 = vis1; }
        else { v->x0 = cw->vx0; v->x1 = cw->vx1; }
    }
    /* 桶缓存系列的 Y 值域：按可见 X 窗扫描桶（fit_y 自动时；
     * 手动 X 窗的 wylo/wyhi 不覆盖桶——这里同样按窗扫描扩展） */
    if (cw->fit_y && v->have_t && v->x1 > v->x0) {
        for (i = 0; i < cw->series_count; i++) {
            OS_Series* sr = &cw->series[i];
            double b0, b1;
            int bi, be;
            if (!sr->buckets || sr->nbuckets <= 0 || sr->b_t1 <= sr->b_t0) continue;
            b0 = (double)(v->x0 - sr->b_t0) * sr->nbuckets / (sr->b_t1 - sr->b_t0);
            b1 = (double)(v->x1 - sr->b_t0) * sr->nbuckets / (sr->b_t1 - sr->b_t0);
            bi = (int)b0;
            if (bi < 0) bi = 0;
            be = (int)b1 + 1;
            if (be > sr->nbuckets - 1) be = sr->nbuckets - 1;
            if (be < bi) continue;
            for (; bi <= be; bi++) {
                if (sr->buckets[bi].n <= 0) continue;
                if (sr->buckets[bi].mn < v->ylo) v->ylo = sr->buckets[bi].mn;
                if (sr->buckets[bi].mx > v->yhi) v->yhi = sr->buckets[bi].mx;
            }
        }
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
    int start = chart_vis_start(sr, cw->view_all, cw->fit_x, cw->npoints);
    int j, idx;
    *ylo = 1e300; *yhi = -1e300;
    for (j = start; j < sr->count; j++) {
        idx = (sr->head - sr->count + j) % OS_CHART_HIST;
        if (idx < 0) idx += OS_CHART_HIST;
        /* 手动 X 窗：只统计窗内样本（fit_x=0 时窗为 [vx0,vx1]） */
        if (!cw->fit_x && !cw->view_all) {
            int64_t t = sr->ts[idx];
            if (t == 0 || t == -1 || t < cw->vx0 || t > cw->vx1) continue;
        }
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
static int chart_sample_at_time(const OS_Series* sr, int view_all, int fit_x, int npoints, int64_t t)
{
    int start = chart_vis_start(sr, view_all, fit_x, npoints);
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

/* 回放全量桶缓存渲染：min/max 包络（max 折线 + min 折线 + 宽列竖线），
 * 可见桶 ≤120 时画圆点。覆盖整个文件时间跨度，长时间采集"全部显示"的数据源。 */
static void chart_draw_buckets(HDC hdc, OS_Series* sr,
                               const RECT* plot, const RECT* lane, int64_t x0, int64_t x1,
                               double ylo, double yhi)
{
    HPEN pen, old;
    int i, i0, i1, vis;
    int first_max = 1, first_min = 1;
    double span;
    int dots;
    if (!sr->buckets || sr->nbuckets <= 0 || sr->b_t1 <= sr->b_t0) return;
    if (x1 <= x0) return;
    span = (double)(sr->b_t1 - sr->b_t0);
    i0 = (int)((double)(x0 - sr->b_t0) * sr->nbuckets / span);
    i1 = (int)((double)(x1 - sr->b_t0) * sr->nbuckets / span) + 1;
    if (i0 < 0) i0 = 0;
    if (i1 > sr->nbuckets - 1) i1 = sr->nbuckets - 1;
    if (i1 < i0) return;
    vis = i1 - i0 + 1;
    dots = (vis > 0 && vis <= 120);
    pen = CreatePen(PS_SOLID, 1, sr->color);
    old = (HPEN)SelectObject(hdc, pen);
    if (dots) {
        HBRUSH dot_br = CreateSolidBrush(sr->color);
        HBRUSH dot_obr = (HBRUSH)SelectObject(hdc, dot_br);
        for (i = i0; i <= i1; i++) {
            int64_t tm = sr->b_t0 + (int64_t)((i + 0.5) * span / sr->nbuckets);
            int xp = map_x(plot, x0, x1, tm);
            if (sr->buckets[i].n <= 0) continue;
            {
                int ymx = map_y(lane, sr->buckets[i].mx, ylo, yhi);
                int ymn = map_y(lane, sr->buckets[i].mn, ylo, yhi);
                Ellipse(hdc, xp - 2, ymx - 2, xp + 2, ymx + 2);
                if (ymn != ymx) Ellipse(hdc, xp - 2, ymn - 2, xp + 2, ymn + 2);
            }
        }
        SelectObject(hdc, dot_obr);
        DeleteObject(dot_br);
    } else {
        /* 包络两条折线（max/min）+ 列宽 ≥2px 时竖线填充 */
        for (i = i0; i <= i1; i++) {
            int64_t t0b = sr->b_t0 + (int64_t)((double)i * span / sr->nbuckets);
            int64_t t1b = sr->b_t0 + (int64_t)((double)(i + 1) * span / sr->nbuckets);
            int64_t tm = sr->b_t0 + (int64_t)((i + 0.5) * span / sr->nbuckets);
            int xm, xb0, xb1, ymx, ymn;
            if (sr->buckets[i].n <= 0) { first_max = first_min = 1; continue; }
            xm = map_x(plot, x0, x1, tm);
            ymx = map_y(lane, sr->buckets[i].mx, ylo, yhi);
            ymn = map_y(lane, sr->buckets[i].mn, ylo, yhi);
            if (first_max) { MoveToEx(hdc, xm, ymx, NULL); first_max = 0; }
            else LineTo(hdc, xm, ymx);
            if (first_min) { MoveToEx(hdc, xm, ymn, NULL); first_min = 0; }
            else LineTo(hdc, xm, ymn);
            xb0 = map_x(plot, x0, x1, t0b);
            xb1 = map_x(plot, x0, x1, t1b);
            if (xb1 - xb0 >= 2 && ymn != ymx) {
                MoveToEx(hdc, xb0, ymx, NULL);
                LineTo(hdc, xb0, ymn);
            }
        }
    }
    SelectObject(hdc, old);
    DeleteObject(pen);
}

/* 大数据量下采样渲染：可见点数超过像素列数×2 时，按像素列做 min/max 包络竖线，
 * 把每帧 GDI 调用从 O(历史点数 65536) 降到 O(像素宽 ~1000)，消除高速采集的界面卡死。
 * 时间轴上的多点在渲染前已按 t 映射到列，重叠点仅保留该列 min/max。 */
#define DECIM_MAX_COLS 4096
static void chart_draw_decim(HDC hdc, OS_Series* sr,
                             const RECT* plot, const RECT* lane, int64_t x0, int64_t x1,
                             double ylo, double yhi, int have_t, int start, int npts)
{
    static double colmin[DECIM_MAX_COLS], colmax[DECIM_MAX_COLS];
    static char   colset[DECIM_MAX_COLS];
    int pw = plot->right - plot->left;
    int ncol = pw;
    int j;
    HPEN pen, old;
    if (ncol > DECIM_MAX_COLS) ncol = DECIM_MAX_COLS;
    if (ncol < 2) return;
    memset(colset, 0, (size_t)ncol);
    for (j = 0; j < npts; j++) {
        int idx = (sr->head - sr->count + start + j) % OS_CHART_HIST;
        int x;
        if (idx < 0) idx += OS_CHART_HIST;
        if (have_t) {
            int64_t t = sr->ts[idx];
            if (t == 0 || t == -1) continue;
            if (t < x0) continue;
            if (t > x1) break; /* 时间有序：越过右沿即结束 */
            x = map_x(plot, x0, x1, t);
        } else {
            x = plot->left + (npts > 1 ? (int)((long long)pw * j / (npts - 1)) : 0);
        }
        x -= plot->left;
        if (x < 0) x = 0;
        if (x >= ncol) x = ncol - 1;
        if (!colset[x]) { colmin[x] = colmax[x] = sr->val[idx]; colset[x] = 1; }
        else {
            if (sr->val[idx] < colmin[x]) colmin[x] = sr->val[idx];
            if (sr->val[idx] > colmax[x]) colmax[x] = sr->val[idx];
        }
    }
    pen = CreatePen(PS_SOLID, 1, sr->color);
    old = (HPEN)SelectObject(hdc, pen);
    for (j = 0; j < ncol; j++) {
        int x, ymx, ymn;
        if (!colset[j]) continue;
        x = plot->left + j;
        ymx = map_y(lane, colmax[j], ylo, yhi);
        ymn = map_y(lane, colmin[j], ylo, yhi);
        MoveToEx(hdc, x, ymn, NULL);
        LineTo(hdc, x, ymx);
    }
    SelectObject(hdc, old);
    DeleteObject(pen);
}

/* 绘制一路曲线：折线 + （可见点少时）采样圆点 */
static void chart_draw_series(HDC hdc, OS_ChartWin* cw, OS_Series* sr,
                              const RECT* plot, const RECT* lane, int64_t x0, int64_t x1,
                              double ylo, double yhi, int have_t, int view_all)
{
    int start = chart_vis_start(sr, view_all, cw->fit_x, cw->npoints);
    int npts = sr->count - start;
    int j, first = 1;
    int vis_npts = 0;
    HPEN pen, old;
    int dots;
    /* 桶缓存存在 → 以桶包络渲染整个文件跨度（长时间采集"全部显示"），跳过环渲染 */
    if (sr->buckets && sr->nbuckets > 0) {
        chart_draw_buckets(hdc, sr, plot, lane, x0, x1, ylo, yhi);
        return;
    }
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
    /* 大数据量：可见点超过像素列数 2 倍时，降采样为 min/max 包络（消除 O(65536) LineTo 卡死） */
    if (!dots && vis_npts > (plot->right - plot->left) * 2 && (plot->right - plot->left) > 8) {
        chart_draw_decim(hdc, sr, plot, lane, x0, x1, ylo, yhi, have_t, start, npts);
        return;
    }
    pen = CreatePen(PS_SOLID, 1, sr->color);
    old = (HPEN)SelectObject(hdc, pen);
    if (dots) {
        /* 采样圆点复用同一把画刷，避免每点 CreateSolidBrush / DeleteObject 的 GDI 开销 */
        HBRUSH dot_br = CreateSolidBrush(sr->color);
        HBRUSH dot_obr = (HBRUSH)SelectObject(hdc, dot_br);
        for (j = 0; j < npts; j++) {
            int idx = (sr->head - sr->count + start + j) % OS_CHART_HIST;
            int x, y;
            if (idx < 0) idx += OS_CHART_HIST;
            if (have_t) {
                int64_t t = sr->ts[idx];
                if (t == 0 || t == -1) { first = 1; continue; }
                if (t < x0) { first = 1; continue; }
                if (t > x1) break; /* 时间有序：越过窗口右沿即结束（65k 缓冲全量扫描优化） */
                x = map_x(plot, x0, x1, t);
            } else {
                x = (npts > 1) ? plot->left + (plot->right - plot->left) * j / (npts - 1) : plot->left;
            }
            y = map_y(lane, sr->val[idx], ylo, yhi);
            if (first) { MoveToEx(hdc, x, y, NULL); first = 0; }
            else LineTo(hdc, x, y);
            Ellipse(hdc, x - 2, y - 2, x + 2, y + 2);
        }
        SelectObject(hdc, dot_obr);
        DeleteObject(dot_br);
    } else {
        for (j = 0; j < npts; j++) {
            int idx = (sr->head - sr->count + start + j) % OS_CHART_HIST;
            int x, y;
            if (idx < 0) idx += OS_CHART_HIST;
            if (have_t) {
                int64_t t = sr->ts[idx];
                if (t == 0 || t == -1) { first = 1; continue; }
                if (t < x0) { first = 1; continue; }
                if (t > x1) break; /* 时间有序：越过窗口右沿即结束 */
                x = map_x(plot, x0, x1, t);
            } else {
                x = (npts > 1) ? plot->left + (plot->right - plot->left) * j / (npts - 1) : plot->left;
            }
            y = map_y(lane, sr->val[idx], ylo, yhi);
            if (first) { MoveToEx(hdc, x, y, NULL); first = 0; }
            else LineTo(hdc, x, y);
        }
    }
    SelectObject(hdc, old);
    DeleteObject(pen);
}

/* 桶序列在时间 t 处的近似值（min/max 均值），无桶/无覆盖返回 0 */
static int chart_val_at_time_bk(const OS_Series* sr, int64_t t, double* v)
{
    int i;
    if (!sr->buckets || sr->nbuckets <= 0 || sr->b_t1 <= sr->b_t0) return 0;
    if (t < sr->b_t0 || t > sr->b_t1) return 0;
    i = (int)((double)(t - sr->b_t0) * sr->nbuckets / (sr->b_t1 - sr->b_t0));
    if (i < 0) i = 0;
    if (i >= sr->nbuckets) i = sr->nbuckets - 1;
    if (sr->buckets[i].n <= 0) return 0;
    *v = (sr->buckets[i].mn + sr->buckets[i].mx) / 2.0;
    return 1;
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
        double vv, d, bkv;
        int64_t st;
        int x, y;
        if (sr->buckets && sr->nbuckets > 0) {
            /* 桶缓存系列：取桶 min/max 均值近似 */
            if (!chart_val_at_time_bk(sr, t, &bkv)) continue;
            vv = bkv;
            st = t;
        } else {
            int idx = chart_sample_at_time(sr, cw->view_all, cw->fit_x, cw->npoints, t);
            if (idx < 0 || idx >= OS_CHART_HIST || sr->count <= 0) continue;
            vv = sr->val[idx];
            st = sr->ts[idx];
        }
        x = map_x(plot, v->x0, v->x1, st);
        y = map_y(plot, vv, v->ylo, v->yhi);
        d = (double)(x - px) * (x - px) + (double)(y - py) * (y - py);
        if (d < best_d) { best_d = d; best_i = i; best_v = vv; best_t = st; }
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
    chart_sync_scrollbar(cw, &v); /* X 轴滚动条与视图同步（手动模式显示，跟随模式隐藏） */
    /* 标题栏 */
    {
        HBRUSH br = CreateSolidBrush(os_theme(TH_PANEL));
        FillRect(hdc, &rc, br);
        DeleteObject(br);
        FrameRect(hdc, &rc, os_theme_brush(TH_BORDER));
        SelectObject(hdc, font);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, os_theme(TH_TEXT));
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
        HBRUSH br = CreateSolidBrush(os_theme(TH_CHART_PLOT_BG));
        FillRect(hdc, &plot, br);
        DeleteObject(br);
    }
    /* 网格：竖直网格共享；水平网格叠加模式全局 / 堆叠模式逐 lane */
    {
        HPEN pen = CreatePen(PS_SOLID, 1, os_theme(TH_CHART_GRID));
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
            HPEN hpen = CreatePen(PS_SOLID, 1, os_theme(TH_CHART_GRID));
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
        SetTextColor(hdc, os_theme(TH_CHART_AXIS));
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
        SetTextColor(hdc, os_theme(TH_CHART_AXIS));
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
                wchar_t wt[180], wdx[64];
                RECT r;
                int64_t dx_us = cw->mt1 - cw->mt0;
                double dx_sec = (double)dx_us / 1e6;
                /* 短间隔用 µs/ms；长间隔用人类可读时分秒 */
                if (dx_sec < 0.001) _snwprintf(wdx, 64, L"%lldµs", (long long)dx_us);
                else if (dx_sec < 1.0) _snwprintf(wdx, 64, L"%.3fms", dx_sec * 1000.0);
                else fmt_time(dx_sec, wdx, 64);
                _snwprintf(wt, 180, L"ΔX=%s  ΔY=%g", wdx, cw->mv1 - cw->mv0);
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
                    double hval;
                    int have_hval = 0;
                    char line[340], tn[64];
                    if (!L) continue;
                    if (sr->buckets && sr->nbuckets > 0) {
                        /* 桶缓存系列：HUD 显示桶 min/max 均值 */
                        if (!chart_val_at_time_bk(sr, t, &hval)) continue;
                        have_hval = 1;
                    } else if (sr->count > 0) {
                        int idx = chart_sample_at_time(sr, cw->view_all, cw->fit_x, cw->npoints, t);
                        hval = sr->val[idx];
                        have_hval = 1;
                    }
                    if (!have_hval) continue;
                    chart_type_name(L, tn, 64);
                    _snprintf(line, 340, "%s = %.6g (%s)", L->name, hval, tn);
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
    /* F23: Ctrl+左键框选——绘制选区矩形（半透明填充 + 虚线边框） */
    if (cw->boxing && cw->box0.x >= 0) {
        int x0 = cw->box0.x, y0 = cw->box0.y;
        int x1 = cw->box1.x, y1 = cw->box1.y;
        int t;
        RECT r;
        if (x0 > x1) { t = x0; x0 = x1; x1 = t; }
        if (y0 > y1) { t = y0; y0 = y1; y1 = t; }
        if (x0 < plot.left) x0 = plot.left;
        if (x1 > plot.right) x1 = plot.right;
        if (y0 < plot.top) y0 = plot.top;
        if (y1 > plot.bottom) y1 = plot.bottom;
        if (x1 > x0 && y1 > y0) {
            /* 亮青虚线边框 + 主题蓝填充：绘图区在两种主题下均为深色底，
             * 亮青在白/黑主题都清晰可见（跳变根因 E：原白色边框在浅色主题不可见） */
            HPEN pen = CreatePen(PS_DOT, 1, RGB(80, 200, 255));
            HPEN old = (HPEN)SelectObject(hdc, pen);
            r.left = x0; r.top = y0; r.right = x1; r.bottom = y1;
            FillRect(hdc, &r, os_theme_brush(TH_TREE_SEL_BG));
            Rectangle(hdc, x0, y0, x1, y1);
            SelectObject(hdc, old);
            DeleteObject(pen);
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
        if (cw) {
            if (cw->anim_on) KillTimer(hwnd, CHART_ANIM_TIMER); /* F23 */
            free(cw);
        }
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
                /* 用户反馈优化（原 Ctrl+框选手感不符）：绘图区内按下——
                 *   普通左键拖拽 → 框选局部放大（boxing，示波器惯例：框住想看的区域即放大）
                 *   Ctrl+左键拖拽 → 平移视图（pan）
                 *   未位移松开 → 单击设测量标记（原 N13e 测量功能不变） */
                chart_plot_rect(cw, &plot);
                if (x >= plot.left && x <= plot.right && y >= plot.top && y <= plot.bottom) {
                    OS_ChartView v;
                    chart_compute_view(cw, &v);
                    if (LOWORD(wParam) & MK_CONTROL) {
                        /* Ctrl+拖拽 = 平移 */
                        cw->drag0.x = x;
                        cw->drag0.y = y;
                        cw->drag_x0 = v.x0; cw->drag_x1 = v.x1;
                        cw->drag_y0 = v.ylo; cw->drag_y1 = v.yhi;
                        cw->drag_full0 = v.full0; cw->drag_full1 = v.full1; /* 夹紧边界 */
                        cw->dragging = 0;
                    } else {
                        /* 普通拖拽 = 框选局部放大：框选起点同步当前视图，映射基于此刻画面 */
                        chart_sync_manual(cw, &v);
                        cw->boxing = 1;
                        cw->box0.x = cw->box1.x = x;
                        cw->box0.y = cw->box1.y = y;
                    }
                    SetCapture(hwnd);
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
            /* F23: 框选——更新框选矩形右下角 */
            if (cw->boxing && GetCapture() == hwnd) {
                cw->box1.x = x;
                cw->box1.y = y;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            /* F23: 拖拽平移——位移超过阈值进入平移，按下即平移视图。
             * 注意先判定阈值再平移：穿越阈值的那次 MOUSEMOVE 同时应用平移，
             * 避免"首次移动只标记不动作"导致单步移动无效果。 */
            if (GetCapture() == hwnd && !cw->boxing && !cw->dragging &&
                (cw->drag0.x != 0 || cw->drag0.y != 0)) {
                int dx = x - cw->drag0.x;
                int dy = y - cw->drag0.y;
                if (dx * dx + dy * dy > 36) {
                    cw->dragging = 1;
                    os_log(OS_LOG_DEBUG, "波形拖拽平移开始");
                }
            }
            if (cw->dragging && GetCapture() == hwnd) {
                int dx = x - cw->drag0.x;
                int dy = y - cw->drag0.y;
                /* X 平移始终有效（fit_x 转手动，从拖拽起点快照平移）。
                 * 夹紧到数据全量范围：全局视图拖拽不再拖出界（用户反馈：
                 * 全局显示下拖拽平移"没实现"——实际是拖出数据范围显示空白）。 */
                if (cw->drag_x1 > cw->drag_x0 && plot.right > plot.left) {
                    double us_px = (double)(cw->drag_x1 - cw->drag_x0) / (plot.right - plot.left);
                    int64_t nx0 = cw->drag_x0 - (int64_t)(dx * us_px);
                    int64_t nx1 = cw->drag_x1 - (int64_t)(dx * us_px);
                    if (nx1 > nx0) {
                        if (cw->drag_full1 > cw->drag_full0) {
                            int64_t span = nx1 - nx0;
                            if (nx0 < cw->drag_full0) { nx0 = cw->drag_full0; nx1 = nx0 + span; }
                            if (nx1 > cw->drag_full1) { nx1 = cw->drag_full1; nx0 = nx1 - span; }
                            if (nx0 < cw->drag_full0) nx0 = cw->drag_full0; /* span 大于全量时兜底 */
                        }
                        cw->vx0 = nx0;
                        cw->vx1 = nx1;
                    }
                }
                /* Y 平移仅在 Y 已手动锁定（fit_y=0）时有效；fit_y=1 时保持自动、
                 * 随 X 窗内数据每帧重算——避免水平拖拽把 Y 从自动冻结到起点快照（跳变根因 C） */
                if (!cw->fit_y && plot.bottom > plot.top && cw->drag_y1 > cw->drag_y0) {
                    double v_px = (cw->drag_y1 - cw->drag_y0) / (plot.bottom - plot.top);
                    cw->vylo = cw->drag_y0 + dy * v_px;
                    cw->vyhi = cw->drag_y1 + dy * v_px;
                }
                cw->fit_x = 0; cw->view_all = 0;
                cw->m0.x = cw->m1.x = -1;
                InvalidateRect(hwnd, NULL, TRUE);
                return 0;
            }
            InvalidateRect(hwnd, NULL, FALSE);
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        if (cw) {
            int x = (short)LOWORD(lParam);
            int y = (short)HIWORD(lParam);
            RECT plot;
            chart_plot_rect(cw, &plot);
            if (cw->boxing && GetCapture() == hwnd) {
                /* 普通左键拖拽 = 框选局部放大（纯函数映射 + 平滑动画） */
                OS_ChartView v;
                int x0 = cw->box0.x, x1 = cw->box1.x;
                int y0 = cw->box0.y, y1 = cw->box1.y;
                int t;
                int64_t nx0 = 0, nx1 = 0;
                double nyhi = 0, nylo = 0;
                int has_x = 0, has_y = 0;
                int applied = 0;
                if (x0 > x1) { t = x0; x0 = x1; x1 = t; }
                if (y0 > y1) { t = y0; y0 = y1; y1 = t; }
                if (x1 - x0 >= 6 && y1 - y0 >= 6 && plot.right > plot.left && plot.bottom > plot.top) {
                    chart_compute_view(cw, &v);
                    chart_sync_manual(cw, &v);  /* 起点=当前视图，动画从此画面平滑过渡 */
                    if (v.have_t && v.x1 > v.x0) {
                        os_cv_box_x(v.x0, v.x1, plot.left, plot.right, x0, x1, &nx0, &nx1);
                        if (nx1 > nx0) has_x = 1;
                    }
                    if (v.yhi > v.ylo) {
                        os_cv_box_y(v.yhi, v.ylo, plot.top, plot.bottom, y0, y1, &nyhi, &nylo);
                        if (nyhi > nylo) has_y = 1;
                    }
                    if (has_x || has_y) {
                        if (has_x) chart_anim_to_x(cw, nx0, nx1);
                        if (has_y) chart_anim_to_y(cw, nylo, nyhi);
                        cw->fit_x = 0; cw->fit_y = 0; cw->view_all = 0;
                        cw->m0.x = cw->m1.x = -1;
                        os_log(OS_LOG_INFO, "波形框选缩放: X=[%lld,%lld]us Y=[%g,%g]",
                               (long long)(has_x ? nx0 : cw->vx0),
                               (long long)(has_x ? nx1 : cw->vx1),
                               has_y ? nylo : cw->vylo, has_y ? nyhi : cw->vyhi);
                        applied = 1;
                    }
                }
                cw->boxing = 0;
                if (applied) {
                    ReleaseCapture();
                    InvalidateRect(hwnd, NULL, TRUE);
                    return 0;
                }
                /* 框太小（<6px，视为单击）或无可缩放数据 → 落到测量标记逻辑 */
                cw->drag0.x = cw->drag0.y = 0;
            }
            if (cw->dragging && GetCapture() == hwnd) {
                cw->dragging = 0;
                ReleaseCapture();
                os_log(OS_LOG_DEBUG, "波形拖拽平移结束: X=[%lld,%lld]us",
                       (long long)cw->vx0, (long long)cw->vx1);
                InvalidateRect(hwnd, NULL, TRUE);
                return 0;
            }
            /* 未拖拽（位移未超阈值）→ 视为单击：设置测量锚点 */
            if (GetCapture() == hwnd && x >= plot.left && x <= plot.right &&
                y >= plot.top && y <= plot.bottom) {
                OS_ChartView v;
                chart_compute_view(cw, &v);
                chart_set_mark(cw, &v, &plot, x, y);
                ReleaseCapture();
                cw->drag0.x = cw->drag0.y = 0;
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        return 0;
    }
    case WM_CAPTURECHANGED:
        if (cw) {
            cw->boxing = 0;
            cw->dragging = 0;
            cw->drag0.x = cw->drag0.y = 0;
        }
        return 0;
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
        /* F23: 滚轮缩放 X 轴（围绕鼠标位置），Ctrl+滚轮缩放 Y 轴。
         * 连续缩放：factor 随 delta 连续变化（pow 指数），取代步进 0.8/1.25，
         * 配合平滑动画插值消除跳变。 */
        short delta = (short)HIWORD(wParam);
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        ScreenToClient(hwnd, &pt);
        if (cw) {
            RECT rc, plot;
            OS_ChartView v;
            double factor = pow(0.8, (double)delta / 120.0); /* 连续缩放因子 */
            GetClientRect(hwnd, &rc);
            plot = rc;
            plot.top += 26; plot.left += 56; plot.bottom -= 18; plot.right -= 6;
            if (pt.x >= plot.left && pt.x <= plot.right &&
                pt.y >= plot.top && pt.y <= plot.bottom) {
                chart_compute_view(cw, &v);
                if (LOWORD(wParam) & MK_CONTROL) {
                    /* Ctrl+滚轮：Y 轴缩放（只动 Y，X 自动/手动保持不被打扰——跳变根因 B）。
                     * Y 缩放不退出整体展示：X 窗保持全局。 */
                    if (!v.have_data) return 0;
                    {
                        double f = (double)(plot.bottom - pt.y) / (plot.bottom - plot.top);
                        double ny0, ny1;
                        if (f < 0.0) f = 0.0;
                        if (f > 1.0) f = 1.0;
                        if (os_cv_zoom_y(v.ylo, v.yhi, f, factor, &ny0, &ny1)) {
                            chart_sync_manual(cw, &v);  /* 起点=当前自动 Y（若自动），避免起跳 */
                            cw->fit_y = 0;
                            os_log(OS_LOG_DEBUG, "波形 Y 轴缩放: [%g,%g] (目标)", ny0, ny1);
                            chart_anim_to_y(cw, ny0, ny1);
                        }
                    }
                } else {
                    /* X 轴缩放（只动 X：Y 保持自动/手动不被打扰——跳变根因 B）。
                     * 用户反馈 bug：旧代码 view_all=0 无条件执行——全局视图滚轮缩小
                     * （no-op，已在全量范围）后 view_all 被清除、fit_x=1 跌回"最后
                     * npoints 窗口"→ 界面跳转到小段波形。改为缩放真正生效才退出整体展示。 */
                    if (v.have_t && v.x1 > v.x0) {
                        double f = (double)(pt.x - plot.left) / (plot.right - plot.left);
                        int64_t nx0, nx1;
                        if (f < 0.0) f = 0.0;
                        if (f > 1.0) f = 1.0;
                        if (os_cv_zoom_x(v.x0, v.x1, f, factor, v.full0, v.full1, &nx0, &nx1)) {
                            chart_sync_manual(cw, &v);  /* 起点=当前自动视图，避免动画从陈旧值起跳 */
                            cw->fit_x = 0;
                            cw->view_all = 0; /* 仅缩放生效时退出整体展示 */
                            os_log(OS_LOG_DEBUG, "波形 X 轴缩放: [%lld,%lld] us (目标; 起点=[%lld,%lld])",
                                   (long long)nx0, (long long)nx1,
                                   (long long)cw->vx0, (long long)cw->vx1);
                            chart_anim_to_x(cw, nx0, nx1);
                        }
                    }
                }
                cw->m0.x = cw->m1.x = -1; /* 缩放后清除测量标记 */
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        return 0;
    }
    case WM_TIMER:
        /* F23: 平滑缩放动画——逐帧向目标插值（X/Y 独立），差值过小直接吸附收敛，
         * 避免整数插值步进为 0 导致永不收敛/定时器泄漏（跳变根因 D）。
         * 每帧 45% 收敛（0.3 时 ~6-8 帧才到位，手感拖沓；0.45 约 3-4 帧，更跟手）。 */
        if (cw && wParam == CHART_ANIM_TIMER && cw->anim_on) {
            int done_x = 1, done_y = 1;
            if (cw->anim_x_on) {
                int64_t d0 = cw->anim_x0 - cw->vx0;
                int64_t d1 = cw->anim_x1 - cw->vx1;
                int64_t s0, s1;
                if (d0 > 1) { s0 = d0 * 45 / 100; if (s0 < 1) s0 = 1; cw->vx0 += s0; done_x = 0; }
                else if (d0 < -1) { s0 = -d0 * 45 / 100; if (s0 < 1) s0 = 1; cw->vx0 -= s0; done_x = 0; }
                else cw->vx0 = cw->anim_x0;
                if (d1 > 1) { s1 = d1 * 45 / 100; if (s1 < 1) s1 = 1; cw->vx1 += s1; done_x = 0; }
                else if (d1 < -1) { s1 = -d1 * 45 / 100; if (s1 < 1) s1 = 1; cw->vx1 -= s1; done_x = 0; }
                else cw->vx1 = cw->anim_x1;
            }
            if (cw->anim_y_on) {
                double dy0 = cw->anim_y0 - cw->vylo;
                double dy1 = cw->anim_y1 - cw->vyhi;
                if (dy0 > 1e-9 || dy0 < -1e-9) { cw->vylo += dy0 * 0.45; done_y = 0; }
                else cw->vylo = cw->anim_y0;
                if (dy1 > 1e-9 || dy1 < -1e-9) { cw->vyhi += dy1 * 0.45; done_y = 0; }
                else cw->vyhi = cw->anim_y1;
            }
            if (done_x && done_y) {
                cw->anim_on = 0;
                cw->anim_x_on = 0;
                cw->anim_y_on = 0;
                KillTimer(hwnd, CHART_ANIM_TIMER);
            }
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    case WM_KEYDOWN:
        if (cw) {
            switch (wParam) {
            case 'F': case 'f': /* 全局显示：整体展示全部波形 */
                cw->view_all = 1; cw->fit_x = 1; cw->fit_y = 1;
                cw->m0.x = cw->m1.x = -1; /* 清除测量标记（视图跳变后像素位置已无效） */
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
    /* 用户反馈优化：X 轴滚动条拖拽平移时间窗（滚动条只在手动 X 缩放后显示） */
    case WM_HSCROLL: {
        if (cw && LOWORD(wParam) != SB_ENDSCROLL) {
            OS_ChartView v;
            SCROLLINFO si;
            int64_t full_span, view_span;
            int pos, page;
            chart_compute_view(cw, &v);
            if (!v.have_t || v.full1 <= v.full0) return 0;
            memset(&si, 0, sizeof(si));
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_HORZ, &si);
            full_span = v.full1 - v.full0;
            view_span = v.x1 - v.x0;
            if (view_span <= 0) view_span = full_span > 100 ? full_span / 100 : 1;
            page = (int)si.nPage;
            pos = (int)si.nPos;
            switch (LOWORD(wParam)) {
            case SB_THUMBTRACK:
            case SB_THUMBPOSITION: pos = (int)lParam; break; /* 拇指拖动位置在 lParam */
            case SB_LINELEFT:  pos -= SCROLL_SCALE / 100; break; /* 1% 微调 */
            case SB_LINERIGHT: pos += SCROLL_SCALE / 100; break;
            case SB_PAGELEFT:  pos -= page; break;               /* 一屏 */
            case SB_PAGERIGHT: pos += page; break;
            case SB_LEFT:      pos = 0; break;
            case SB_RIGHT:     pos = SCROLL_SCALE; break;
            default: return 0;
            }
            if (page < 1) page = 1;
            if (page >= SCROLL_SCALE) page = SCROLL_SCALE;
            if (pos < 0) pos = 0;
            if (pos > SCROLL_SCALE - page) pos = SCROLL_SCALE - page;
            cw->fit_x = 0; cw->view_all = 0;
            cw->m0.x = cw->m1.x = -1; /* 平移后测量标记像素位置已失效 */
            cw->vx0 = v.full0 + (int64_t)pos * full_span / SCROLL_SCALE;
            cw->vx1 = cw->vx0 + view_span;
            if (cw->vx1 > v.full1) { cw->vx1 = v.full1; cw->vx0 = cw->vx1 - view_span; }
            /* 回写滚动条位置（LINE/PAGE 需显式更新） */
            {
                SCROLLINFO si2;
                memset(&si2, 0, sizeof(si2));
                si2.cbSize = sizeof(si2);
                si2.fMask = SIF_POS;
                si2.nPos = pos;
                SetScrollInfo(hwnd, SB_HORZ, &si2, TRUE);
            }
            os_log(OS_LOG_DEBUG, "波形滚动条平移: X=[%lld,%lld]us", (long long)cw->vx0, (long long)cw->vx1);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    }
    case WM_OS_CHART_FITALL:
        if (cw) {
            cw->view_all = 1; cw->fit_x = 1; cw->fit_y = 1;
            cw->paused = 1;
            cw->m0.x = cw->m1.x = -1; /* 清除测量标记 */
            os_log(OS_LOG_DEBUG, "波形整体展示 (停止采集)");
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    case WM_OS_CHART_QUERY:
        /* 测试钩子（Bug19 回归）：返回 (series_count<<16) | series[0].count（截断 0xFFFF） */
        if (cw) {
            int c0 = cw->series_count > 0 ? cw->series[0].count : 0;
            if (c0 > 0xFFFF) c0 = 0xFFFF;
            return (LRESULT)((cw->series_count << 16) | c0);
        }
        return 0;
    case WM_OS_CHART_SHOT: {
        /* 测试钩子：渲染当前视图存 BMP（WM_PRINT），供回归脚本验证曲线实际绘制 */
        if (cw) {
            wchar_t p[MAX_PATH];
            wchar_t* slash;
            GetModuleFileNameW(NULL, p, MAX_PATH);
            slash = wcsrchr(p, L'\\');
            if (slash) slash[1] = 0;
            wcscat(p, L"chart_shot.bmp");
            os_save_window_bmp(hwnd, p);
        }
        return 0;
    }
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
                cw->m0.x = cw->m1.x = -1; /* 清除测量标记 */
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
                    cw->series[k].buckets = NULL; /* 桶缓存一并清除 */
                    cw->series[k].nbuckets = 0;
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
    /* WS_HSCROLL：X 轴滚动条（用户反馈：缩放后拖拽滚动条平移时间窗到想观察的点） */
    return CreateWindowW(g_chart_class, title ? title : L"波形窗口",
                         WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP | WS_HSCROLL,
                         x, y, w, h, parent, NULL, g_app.hInst, NULL);
}
