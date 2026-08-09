#ifndef OS_NUMWIN_H
#define OS_NUMWIN_H

#include <windows.h>

void os_num_register(void);
HWND os_num_create(HWND parent, int x, int y, int w, int h, const wchar_t* title);
void os_num_push(HWND hwnd, const OS_Sample* s);
void os_num_add_var(HWND hwnd, int leaf_id);
void os_num_remove_var(HWND hwnd, int row);
int os_num_is(HWND hwnd);
int os_num_var_name(HWND hwnd, int idx, char* out, int cap);
/* F20: 按当前主题刷新数值窗口内部列表颜色 */
void os_num_apply_theme(HWND hwnd);

#endif
