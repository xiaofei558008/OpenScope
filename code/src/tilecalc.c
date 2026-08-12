#include "tilecalc.h"
#include <math.h>

void os_tile_ratios_init(double* ratios, int n)
{
    int i;
    if (!ratios || n <= 0) return;
    for (i = 0; i < n; i++) ratios[i] = 1.0 / n;
}

int os_tile_ratios_drag(double* ratios, int n, int k, int total_px, int dx_px, int min_px)
{
    if (!ratios || n < 2 || k < 0 || k >= n - 1 || total_px <= 0) return 0;
    return os_tile_ratios_drag2(ratios, k, k + 1, total_px, dx_px, min_px);
}

int os_tile_ratios_drag2(double* ratios, int a, int b, int total_px, int dx_px, int min_px)
{
    double dr, minr, na, nb;
    if (!ratios || a < 0 || b < 0 || a == b || total_px <= 0) return 0;
    dr = (double)dx_px / (double)total_px;
    minr = (double)min_px / (double)total_px;
    if (minr * 2.0 > ratios[a] + ratios[b]) return 0; /* 两列都不够最小宽度：不变化 */
    na = ratios[a] + dr;
    nb = ratios[b] - dr;
    if (na < minr) { nb -= (minr - na); na = minr; }
    if (nb < minr) { na -= (minr - nb); nb = minr; }
    if (na < minr || nb < minr) return 0; /* 双向夹紧仍越界：不变化 */
    /* 浮点噪声容差：夹紧回同一值时（如已在最小宽度继续同向拖）判无变化 */
    if (fabs(na - ratios[a]) < 1e-12 && fabs(nb - ratios[b]) < 1e-12) return 0;
    ratios[a] = na;
    ratios[b] = nb;
    return 1;
}
