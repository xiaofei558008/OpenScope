#include "chartview.h"

/* 缩放最小 X 时间窗：1ms（与历史滚轮缩放下限一致） */
#define OS_CV_MIN_SPAN_US 1000.0
/* Y 值域最小跨度：避免退化为 0 导致除法除零 */
#define OS_CV_MIN_SPAN_Y  1e-12

int os_cv_zoom_x(int64_t x0, int64_t x1, double f, double factor,
                 int64_t full0, int64_t full1, int64_t* nx0, int64_t* nx1)
{
    double span, anchor, n0, n1;
    int64_t i0, i1;
    if (!nx0 || !nx1 || x1 <= x0) return 0;
    if (f < 0.0) f = 0.0;
    if (f > 1.0) f = 1.0;
    span = (double)(x1 - x0) * factor;
    if (span < OS_CV_MIN_SPAN_US) span = OS_CV_MIN_SPAN_US;
    anchor = (double)x0 + f * (double)(x1 - x0);  /* 锚点时刻（保持不漂移） */
    n0 = anchor - f * span;
    n1 = n0 + span;
    /* 夹紧到数据全量范围（独立夹紧，不做“保持 span”的连带回拉，避免放大出窗） */
    if (full0 > INT64_MIN && n0 < (double)full0) n0 = (double)full0;
    if (full1 < INT64_MAX && n1 > (double)full1) n1 = (double)full1;
    if (n1 <= n0) return 0;
    i0 = (int64_t)n0;
    i1 = (int64_t)n1;
    if (i0 == x0 && i1 == x1) return 0;  /* 无变化（例如已在最小窗且继续放大） */
    *nx0 = i0;
    *nx1 = i1;
    return 1;
}

int os_cv_zoom_y(double ylo, double yhi, double f, double factor,
                 double* ny0, double* ny1)
{
    double span, anchor, n0, n1;
    if (!ny0 || !ny1 || !(yhi > ylo)) return 0;
    if (f < 0.0) f = 0.0;
    if (f > 1.0) f = 1.0;
    span = (yhi - ylo) * factor;
    if (span < OS_CV_MIN_SPAN_Y) span = OS_CV_MIN_SPAN_Y;
    anchor = ylo + f * (yhi - ylo);  /* 锚点值（保持不漂移） */
    n0 = anchor - f * span;
    n1 = n0 + span;
    if (n1 <= n0) return 0;
    if (n0 == ylo && n1 == yhi) return 0;  /* 无变化 */
    *ny0 = n0;
    *ny1 = n1;
    return 1;
}

void os_cv_box_x(int64_t x0, int64_t x1, int pl, int pr, int bl, int br,
                 int64_t* nx0, int64_t* nx1)
{
    double f0, f1;
    if (!nx0 || !nx1) return;
    *nx0 = x0; *nx1 = x1;
    if (x1 <= x0 || pr <= pl || br <= bl) return;
    f0 = (double)(bl - pl) / (double)(pr - pl);
    f1 = (double)(br - pl) / (double)(pr - pl);
    if (f1 <= f0) return;
    *nx0 = x0 + (int64_t)(f0 * (double)(x1 - x0));
    *nx1 = x0 + (int64_t)(f1 * (double)(x1 - x0));
    if (*nx1 <= *nx0) *nx1 = *nx0 + 1;  /* 保底非空窗 */
}

void os_cv_box_y(double yhi, double ylo, int pt, int pb, int bt, int bb,
                 double* nyhi, double* nylo)
{
    double span, f0, f1;
    if (!nyhi || !nylo) return;
    *nyhi = yhi; *nylo = ylo;
    if (!(yhi > ylo) || pb <= pt || bb <= bt) return;
    f0 = (double)(bt - pt) / (double)(pb - pt);  /* 顶框边：高值 */
    f1 = (double)(bb - pt) / (double)(pb - pt);  /* 底框边：低值 */
    if (f1 <= f0) return;
    span = yhi - ylo;
    *nyhi = yhi - f0 * span;
    *nylo = yhi - f1 * span;
    if (!(*nyhi > *nylo)) { *nyhi = yhi; *nylo = ylo; }
}
