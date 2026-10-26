/* 强制包含（gcc -include / MSVC /FI）：先拿 windows.h，再把 das.c 里的 DLL 加载重定向到假 MCD */
#ifndef OS_TEST_PRE_H
#define OS_TEST_PRE_H
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
HMODULE fake_LoadLibraryExA(const char* p, HANDLE f, DWORD fl);
FARPROC fake_GetProcAddress(HMODULE h, const char* n);
BOOL    fake_FreeLibrary(HMODULE h);
#define LoadLibraryExA fake_LoadLibraryExA
#define GetProcAddress fake_GetProcAddress
#define FreeLibrary    fake_FreeLibrary
#endif
