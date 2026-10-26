/* 最小 Win32 shim（仅供 Linux/gcc 下跑 drv_reconnect 测试；Windows 上用真正的 windows.h） */
#ifndef OS_TEST_WIN32_SHIM_H
#define OS_TEST_WIN32_SHIM_H
#define _GNU_SOURCE
#include <pthread.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <wchar.h>

#define WINAPI
#define WIN32_LEAN_AND_MEAN_DONE
typedef int BOOL;
typedef int32_t LONG;
typedef uint32_t DWORD, UINT;
typedef int64_t LONGLONG;
typedef uint64_t ULONGLONG;
typedef uintptr_t WPARAM, ULONG_PTR;
typedef intptr_t LPARAM, LRESULT, INT_PTR;
typedef void *HANDLE, *HMODULE, *HINSTANCE, *HWND, *LPVOID;
typedef const wchar_t* LPCWSTR;
typedef void (*FARPROC)(void);
typedef DWORD (*LPTHREAD_START_ROUTINE)(LPVOID);
typedef union { struct { DWORD LowPart; LONG HighPart; } u; LONGLONG QuadPart; } LARGE_INTEGER;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#define MAX_PATH 260
#define INFINITE 0xFFFFFFFFu
#define WAIT_OBJECT_0 0u
#define WAIT_TIMEOUT 258u
#define LOAD_WITH_ALTERED_SEARCH_PATH 8
#define _snprintf snprintf
#define _vsnprintf vsnprintf

typedef struct { pthread_mutex_t m; } CRITICAL_SECTION;
static inline void InitializeCriticalSection(CRITICAL_SECTION* c)
{
    pthread_mutexattr_t a; pthread_mutexattr_init(&a);
    pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&c->m, &a); pthread_mutexattr_destroy(&a);
}
static inline void DeleteCriticalSection(CRITICAL_SECTION* c) { pthread_mutex_destroy(&c->m); }
static inline void EnterCriticalSection(CRITICAL_SECTION* c) { pthread_mutex_lock(&c->m); }
static inline void LeaveCriticalSection(CRITICAL_SECTION* c) { pthread_mutex_unlock(&c->m); }
static inline BOOL TryEnterCriticalSection(CRITICAL_SECTION* c) { return pthread_mutex_trylock(&c->m) == 0; }

static inline LONG InterlockedCompareExchange(volatile LONG* p, LONG x, LONG cmp)
{ __atomic_compare_exchange_n((LONG*)p, &cmp, x, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); return cmp; }
static inline LONG InterlockedExchange(volatile LONG* p, LONG x) { return __atomic_exchange_n((LONG*)p, x, __ATOMIC_SEQ_CST); }
static inline LONG InterlockedIncrement(volatile LONG* p) { return __atomic_add_fetch((LONG*)p, 1, __ATOMIC_SEQ_CST); }
static inline LONG InterlockedDecrement(volatile LONG* p) { return __atomic_sub_fetch((LONG*)p, 1, __ATOMIC_SEQ_CST); }

static inline DWORD GetCurrentThreadId(void) { return (DWORD)syscall(SYS_gettid); }
static inline void Sleep(DWORD ms) { struct timespec t = { (time_t)(ms / 1000), (long)(ms % 1000) * 1000000L }; while (nanosleep(&t, &t) == -1 && errno == EINTR) {} }
static inline BOOL SwitchToThread(void) { sched_yield(); return TRUE; }
static inline BOOL QueryPerformanceFrequency(LARGE_INTEGER* f) { f->QuadPart = 1000000000LL; return TRUE; }
static inline BOOL QueryPerformanceCounter(LARGE_INTEGER* c)
{ struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); c->QuadPart = (LONGLONG)t.tv_sec * 1000000000LL + t.tv_nsec; return TRUE; }
static inline ULONGLONG GetTickCount64(void) { LARGE_INTEGER c; QueryPerformanceCounter(&c); return (ULONGLONG)(c.QuadPart / 1000000); }
static inline DWORD GetEnvironmentVariableA(const char* n, char* b, DWORD cap)
{ const char* v = getenv(n); size_t l; if (!v) return 0; l = strlen(v); if (l + 1 > cap) return (DWORD)(l + 1); memcpy(b, v, l + 1); return (DWORD)l; }

/* 线程：HANDLE = ShimThread*；WaitForSingleObject 等 done；CloseHandle 可在线程结束前调用 */
typedef struct ShimThread {
    pthread_t t; pthread_mutex_t m; pthread_cond_t cv;
    int done, closed; LPTHREAD_START_ROUTINE fn; LPVOID arg;
} ShimThread;
static inline void* shim_thread_main(void* p)
{
    ShimThread* s = (ShimThread*)p; int fr;
    s->fn(s->arg);
    pthread_mutex_lock(&s->m); s->done = 1; pthread_cond_broadcast(&s->cv); fr = s->closed; pthread_mutex_unlock(&s->m);
    if (fr) { pthread_mutex_destroy(&s->m); pthread_cond_destroy(&s->cv); free(s); }
    return NULL;
}
static inline HANDLE CreateThread(void* sa, size_t st, LPTHREAD_START_ROUTINE fn, LPVOID arg, DWORD fl, DWORD* tid)
{
    ShimThread* s = (ShimThread*)calloc(1, sizeof(*s)); (void)sa; (void)st; (void)fl; (void)tid;
    if (!s) return NULL;
    pthread_mutex_init(&s->m, NULL); pthread_cond_init(&s->cv, NULL); s->fn = fn; s->arg = arg;
    if (pthread_create(&s->t, NULL, shim_thread_main, s) != 0) { free(s); return NULL; }
    return s;
}
static inline DWORD WaitForSingleObject(HANDLE h, DWORD ms)
{
    ShimThread* s = (ShimThread*)h; DWORD r;
    struct timespec dl; clock_gettime(CLOCK_REALTIME, &dl);
    if (ms != INFINITE) { dl.tv_sec += ms / 1000; dl.tv_nsec += (long)(ms % 1000) * 1000000L; if (dl.tv_nsec >= 1000000000L) { dl.tv_sec++; dl.tv_nsec -= 1000000000L; } }
    pthread_mutex_lock(&s->m);
    while (!s->done) {
        if (ms == INFINITE) pthread_cond_wait(&s->cv, &s->m);
        else if (pthread_cond_timedwait(&s->cv, &s->m, &dl) == ETIMEDOUT) break;
    }
    r = s->done ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    pthread_mutex_unlock(&s->m);
    return r;
}
static inline BOOL CloseHandle(HANDLE h)
{
    ShimThread* s = (ShimThread*)h; int done;
    pthread_mutex_lock(&s->m); done = s->done; if (!done) s->closed = 1; pthread_mutex_unlock(&s->m);
    if (done) { pthread_join(s->t, NULL); pthread_mutex_destroy(&s->m); pthread_cond_destroy(&s->cv); free(s); }
    else pthread_detach(s->t);
    return TRUE;
}
/* LoadLibrary 系列由测试的 fake_mcd.c 提供（经 pre.h 重定向） */
HMODULE LoadLibraryExA(const char* p, HANDLE f, DWORD fl);
FARPROC GetProcAddress(HMODULE h, const char* n);
BOOL FreeLibrary(HMODULE h);
#endif
