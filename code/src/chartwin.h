#ifndef OS_CHARTWIN_H
#define OS_CHARTWIN_H

#include <windows.h>

void os_chart_register(void);
HWND os_chart_create(HWND parent, int x, int y, int w, int h, const wchar_t* title);
void os_chart_push(HWND hwnd, const OS_Sample* s);
void os_chart_add_var(HWND hwnd, int leaf_id);
int os_chart_is(HWND hwnd);

#endif
