/* 仅用于 Linux 单测：把 xcp.c / module_api.h 用到的少量 Win32 API 映射到 POSIX。
 * 不进产品构建。 */
#ifndef SHIM_WINDOWS_H
#define SHIM_WINDOWS_H
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#define MAX_PATH 260
typedef void* HWND;
typedef void* HANDLE;
typedef long LONG;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned int UINT;
typedef uintptr_t WPARAM;
typedef intptr_t LPARAM;
#define TRUE 1
#define FALSE 0
#define __declspec(x)
#define WINAPI
typedef struct { pthread_mutex_t m; } CRITICAL_SECTION;
static inline void InitializeCriticalSection(CRITICAL_SECTION* c)
{
    pthread_mutexattr_t a;
    pthread_mutexattr_init(&a);
    pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&c->m, &a);
    pthread_mutexattr_destroy(&a);
}
static inline void DeleteCriticalSection(CRITICAL_SECTION* c) { pthread_mutex_destroy(&c->m); }
static inline void EnterCriticalSection(CRITICAL_SECTION* c) { pthread_mutex_lock(&c->m); }
static inline void LeaveCriticalSection(CRITICAL_SECTION* c) { pthread_mutex_unlock(&c->m); }
static inline LONG InterlockedCompareExchange(volatile LONG* p, LONG x, LONG cmp)
{
    __atomic_compare_exchange_n(p, &cmp, x, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return cmp;
}
static inline LONG InterlockedExchange(volatile LONG* p, LONG v) { return __atomic_exchange_n(p, v, __ATOMIC_SEQ_CST); }
static inline LONG InterlockedIncrement(volatile LONG* p) { return __atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST); }
static inline LONG InterlockedDecrement(volatile LONG* p) { return __atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST); }
static inline void Sleep(DWORD ms) { struct timespec t; t.tv_sec = ms / 1000; t.tv_nsec = (long)(ms % 1000) * 1000000L; while (nanosleep(&t, &t) && errno == EINTR) {} }
static inline DWORD GetTickCount(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return (DWORD)(t.tv_sec * 1000 + t.tv_nsec / 1000000); }
#define _snprintf snprintf
#define _vsnprintf vsnprintf
#endif
