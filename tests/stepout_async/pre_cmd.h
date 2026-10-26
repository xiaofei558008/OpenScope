/* 强制包含（只用于编译 code/src/cmd_async.c）：钉住/放开驱动 DLL、投递完成消息改为测试计数/队列 */
#ifndef PRE_CMD_H
#define PRE_CMD_H
#include <windows.h>
#ifndef GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 0x4
#endif
#define GetModuleHandleExW pin_GetModuleHandleExW
#define FreeLibrary        pin_FreeLibrary
#define PostMessageW       pin_PostMessageW
BOOL pin_GetModuleHandleExW(DWORD fl, LPCWSTR addr, HMODULE* out);
BOOL pin_FreeLibrary(HMODULE h);
BOOL pin_PostMessageW(HWND h, UINT msg, WPARAM wp, LPARAM lp);
#endif
