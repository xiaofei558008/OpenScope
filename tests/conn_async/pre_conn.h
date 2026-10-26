/* 强制包含（gcc -include / MSVC /FI）：把 conn_async.c 用到的系统调用重定向到测试的假实现，
 * 这样 Windows 上也能用真实 windows.h 编译同一份测试。 */
#ifndef PRE_CONN_H
#define PRE_CONN_H
#include <windows.h>
#define GetModuleHandleExW fake_GetModuleHandleExW
#define FreeLibrary        fake_FreeLibrary
#define PostMessageW       fake_PostMessageW
BOOL fake_GetModuleHandleExW(DWORD fl, LPCWSTR addr, HMODULE* out);
BOOL fake_FreeLibrary(HMODULE h);
BOOL fake_PostMessageW(HWND h, UINT msg, WPARAM wp, LPARAM lp);
#endif
