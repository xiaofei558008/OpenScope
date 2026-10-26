/* 强制包含（gcc -include / MSVC /FI）：先拿 windows.h，再把驱动里的 DLL 加载重定向到假 DLL */
#ifndef OS_TEST_PRE_PROBE_H
#define OS_TEST_PRE_PROBE_H
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
HMODULE fake_LoadLibraryA(const char* p);
FARPROC fake_GetProcAddress(HMODULE h, const char* n);
BOOL    fake_FreeLibrary(HMODULE h);
#define LoadLibraryA   fake_LoadLibraryA
#define GetProcAddress fake_GetProcAddress
#define FreeLibrary    fake_FreeLibrary
#endif
