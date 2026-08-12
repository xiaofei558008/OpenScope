#ifndef OS_CHARTWIN_H
#define OS_CHARTWIN_H

#include <windows.h>

void os_chart_register(void);
HWND os_chart_create(HWND parent, int x, int y, int w, int h, const wchar_t* title);
void os_chart_push(HWND hwnd, const OS_Sample* s);
void os_chart_add_var(HWND hwnd, int leaf_id);
void os_chart_remove_var(HWND hwnd, int idx);
int os_chart_is(HWND hwnd);
int os_chart_var_name(HWND hwnd, int idx, char* out, int cap);
void os_chart_rebind(HWND hwnd); /* 需求2：ELF 重载后按变量名重绑 leaf_id */

#endif
