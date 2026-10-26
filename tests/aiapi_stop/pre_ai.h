/* 强制包含：aiapi.c 的 DLL 钉住/放开改为测试计数（Windows 上同样通过 /FI 使用） */
#ifndef PRE_AI_H
#define PRE_AI_H
#include <winsock2.h>
#include <windows.h>
#ifndef GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 0x4
#endif
#define GetModuleHandleExW fake_GetModuleHandleExW
#define FreeLibrary        fake_FreeLibrary
BOOL fake_GetModuleHandleExW(DWORD fl, LPCWSTR addr, HMODULE* out);
BOOL fake_FreeLibrary(HMODULE h);
/* 真实 DLL 里线程以 FreeLibraryAndExitThread 结束；测试里放开计数后普通 return */
#define AI_THREAD_RETURN(pin, code) do { if (pin) fake_FreeLibrary(pin); return (code); } while (0)
#endif
