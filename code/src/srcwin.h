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
/* Bug1: 两个路径是否指同一源文件（斜杠/相对/大小写/同一文件索引） */
int  os_src_same_path(const wchar_t* a, const wchar_t* b);
/* 打开/切换到指定本机源文件；已打开则聚焦并可选跳行 */
HWND os_src_open_file(HWND parent_tab, const wchar_t* path, int goto_line);
/* 刷新所有源码窗断点标记（驱动 BP 变化后） */
void os_src_refresh_bps(void);
/* 布局持久化：取/恢复源码窗口的文件与首可见行（1-based） */
int  os_src_get_state(HWND hwnd, wchar_t* path, int cap, int* top1);
int  os_src_restore(HWND hwnd, const wchar_t* path, int top1);
int  os_src_describe_json(char* out, int cap);
/* 将 PC 高亮到指定文件行（1-based）；line<=0 清除全部 PC 高亮 */
int  os_src_toggle_bp_caret(HWND hwnd); /* UI P1b: F9; 1 = toggled */
void os_src_set_pc_line(const wchar_t* path, int line1);

#ifdef __cplusplus
}
#endif

#endif
