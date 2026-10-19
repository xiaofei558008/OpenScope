/*
 * 源码窗口：行号栏 + 双击切换硬件断点（红点）。
 */
#ifndef OS_SRCWIN_H
#define OS_SRCWIN_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

void os_src_register(void);
HWND os_src_create(HWND parent, int x, int y, int w, int h, const wchar_t* title);
int  os_src_is(HWND hwnd);
/* 打开/切换到指定本机源文件；已打开则聚焦并可选跳行 */
HWND os_src_open_file(HWND parent_tab, const wchar_t* path, int goto_line);
/* 刷新所有源码窗断点标记（驱动 BP 变化后） */
void os_src_refresh_bps(void);
/* 将 PC 高亮到指定文件行（1-based）；line<=0 清除全部 PC 高亮 */
void os_src_set_pc_line(const wchar_t* path, int line1);

#ifdef __cplusplus
}
#endif

#endif
