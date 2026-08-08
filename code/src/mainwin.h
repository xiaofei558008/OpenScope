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
void os_mainwin_update_buttons(void);
int os_mainwin_open_elf(const wchar_t* path);
int os_mainwin_active_tab(void);
void os_mainwin_select_tab(int idx);
void os_mainwin_refresh_layout(void);
HWND os_win_create_by_type(const char* type, const wchar_t* title);
void os_mainwin_cfg_init(void);

#endif
