#ifndef OS_NUMWIN_H
#define OS_NUMWIN_H

#include <windows.h>

void os_num_register(void);
HWND os_num_create(HWND parent, int x, int y, int w, int h, const wchar_t* title);
void os_num_push(HWND hwnd, const OS_Sample* s);
void os_num_add_var(HWND hwnd, int leaf_id);

#endif
