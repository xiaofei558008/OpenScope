#ifndef OS_MAINWIN_H
#define OS_MAINWIN_H

#include <windows.h>

/* 变量模糊搜索选择对话框：成功返回 0 并写 leaf id，取消返回非 0 */
int os_dlg_pick_var(HWND owner, int* out_leaf_id);
/* 变量值编辑/写入对话框：成功返回 0 */
int os_dlg_edit_value(HWND owner, int leaf_id);

LRESULT CALLBACK os_mainwin_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void os_mainwin_register(void);
void os_mainwin_rebuild_window_menu(void);

#endif
