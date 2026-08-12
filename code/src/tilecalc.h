#ifndef OS_TILECALC_H
#define OS_TILECALC_H

/* Bug6：tab 内平铺窗口的列宽比例纯计算（无 Win32 依赖，便于单元测试）。
 * 平铺按列排布，每列宽度 = ratio * 可用总宽；相邻列间的分隔带可拖拽调整比例。 */

/* 初始化为均分（和=1）。n<=0 或 ratios==NULL 安全返回。 */
void os_tile_ratios_init(double* ratios, int n);

/* 拖拽列 k 与列 k+1 之间的分隔带 dx 像素（右为正）后的新比例。
 * total_px = 全部列可用总像素（不含分隔带）；min_px = 每列最小像素。
 * 两列此消彼长、其余列不变；任一列触到最小宽度则夹紧（另一列补偿，保持和=1）。
 * 返回 1=比例有变化，0=无变化/参数非法（NULL、越界、total<=0、n<2）。 */
int os_tile_ratios_drag(double* ratios, int n, int k, int total_px, int dx_px, int min_px);

/* 同 os_tile_ratios_drag，但直接指定两列下标 a/b（平铺跳过最小化列时，
 * 相邻可见列在比例数组中不一定相邻）。仅调整 ratios[a] 与 ratios[b]，和保持不变。 */
int os_tile_ratios_drag2(double* ratios, int a, int b, int total_px, int dx_px, int min_px);

#endif
