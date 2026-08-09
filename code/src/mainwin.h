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
void os_mainwin_refresh_status(void);
int os_mainwin_open_elf(const wchar_t* path);
/* N9(b): 变量是否仍被任一窗口引用；从窗口移除后自动取消观测 */
int  os_win_leaf_used(int leaf_id);
void os_win_auto_unwatch(int leaf_id);
int os_mainwin_active_tab(void);
void os_mainwin_select_tab(int idx);
void os_mainwin_refresh_layout(void);
HWND os_win_create_by_type(const char* type, const wchar_t* title);
/* N11: 在 tab 内新建 type 窗口（tab<0=新建 tab，tab 有效=附加到该 tab 组） */
HWND os_win_add_to_tab(int tab, const char* type, const wchar_t* title);
/* N11/Bug3: 子窗口获得焦点时登记为“当前窗口” */
void os_win_mark_active(HWND w);
/* N3: 就地重命名编辑框——句柄/回车ESC处理/失焦提交（main.c 消息循环 + 定时器调用） */
HWND os_tab_edit_hwnd(void);
void os_tab_edit_handle_key(WPARAM key);
void os_tab_edit_focus_check(void);
void os_mainwin_cfg_init(void);

#endif
