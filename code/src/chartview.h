#ifndef OS_CHARTVIEW_H
#define OS_CHARTVIEW_H

#include <stdint.h>

/* F23 波形视图数学：纯函数，无 Win32/UI 依赖，可单元测试。
 *
 * 所有 X 轴缩放/平移围绕“锚点比例 f∈[0,1]”进行：f=0 保持左边界不动、
 * f=0.5 保持鼠标所在时刻不动、f=1 保持右边界不动，从而保证缩放时光标
 * 下的波形位置不漂移（丝滑连续的关键）。
 *
 * X 时间窗单位为 us（int64），Y 值域为 double。返回值 1=视图有变化、0=不变。 */

/* X 轴以锚点比例 f 缩放时间窗 [x0,x1]，factor<1 放大 / >1 缩小。
 * 最小窗 1000us（1ms）；结果夹紧到 [full0,full1]（数据全量时间范围；
 * 传 INT64_MIN/MAX 表示不夹紧）。 */
int os_cv_zoom_x(int64_t x0, int64_t x1, double f, double factor,
                 int64_t full0, int64_t full1, int64_t* nx0, int64_t* nx1);

/* Y 轴以锚点比例 f 缩放值域 [ylo,yhi]，factor<1 放大 / >1 缩小。 */
int os_cv_zoom_y(double ylo, double yhi, double f, double factor,
                 double* ny0, double* ny1);

/* 框选局部放大：绘图区 X 区间 [pl,pr] 内框选 [bl,br]，映射到 X 时间窗 [x0,x1]。 */
void os_cv_box_x(int64_t x0, int64_t x1, int pl, int pr, int bl, int br,
                 int64_t* nx0, int64_t* nx1);

/* 框选局部放大 Y：绘图区 Y 区间 [pt,pb]（顶部=最大值 yhi）内框选 [bt,bb]，
 * 映射到新的 Y 值域 [nylo,nyhi]（顶框边=高值，底框边=低值）。 */
void os_cv_box_y(double yhi, double ylo, int pt, int pb, int bt, int bb,
                 double* nyhi, double* nylo);

#endif /* OS_CHARTVIEW_H */
