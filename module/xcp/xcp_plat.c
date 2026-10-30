#include "xcp_plat.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <process.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

long xcp_atomic_load(const xcp_atomic_t* a) { return InterlockedCompareExchange((volatile long*)&a->v, 0, 0); }
void xcp_atomic_store(xcp_atomic_t* a, long v) { InterlockedExchange(&a->v, v); }
long xcp_atomic_add(xcp_atomic_t* a, long d) { return InterlockedExchangeAdd(&a->v, d) + d; }

void xcp_mutex_init(xcp_mutex_t* m) { InitializeSRWLock(&m->l); }
void xcp_mutex_destroy(xcp_mutex_t* m) { (void)m; }
void xcp_mutex_lock(xcp_mutex_t* m) { AcquireSRWLockExclusive(&m->l); }
void xcp_mutex_unlock(xcp_mutex_t* m) { ReleaseSRWLockExclusive(&m->l); }
void xcp_cond_init(xcp_cond_t* c) { InitializeConditionVariable(&c->c); }
void xcp_cond_destroy(xcp_cond_t* c) { (void)c; }
int xcp_cond_wait_ms(xcp_cond_t* c, xcp_mutex_t* m, int ms)
{
    if (ms < 0) ms = 0;
    if (SleepConditionVariableSRW(&c->c, &m->l, (DWORD)ms, 0)) return 0;
    return GetLastError() == ERROR_TIMEOUT ? 1 : 0;
}
void xcp_cond_broadcast(xcp_cond_t* c) { WakeAllConditionVariable(&c->c); }

typedef struct { xcp_thread_fn fn; void* arg; } ThrBox;
static unsigned __stdcall thr_tramp(void* p)
{
    ThrBox b = *(ThrBox*)p;
    free(p);
    b.fn(b.arg);
    return 0;
}
int xcp_thread_start(xcp_thread_t* t, xcp_thread_fn fn, void* arg)
{
    ThrBox* b = (ThrBox*)malloc(sizeof(ThrBox));
    if (!b) return -1;
    b->fn = fn; b->arg = arg;
    t->h = (HANDLE)_beginthreadex(NULL, 0, thr_tramp, b, 0, &t->id);
    if (!t->h) { free(b); return -1; }
    return 0;
}
void xcp_thread_join(xcp_thread_t* t)
{
    if (!t->h) return;
    WaitForSingleObject(t->h, INFINITE);
    CloseHandle(t->h);
    t->h = NULL;
}
int xcp_thread_is_self(const xcp_thread_t* t) { return t->h && GetCurrentThreadId() == t->id; }

/* QPC：不受 15.6 ms 系统节拍粒度影响（GetTickCount64 会） */
uint64_t xcp_now_ms(void)
{
    static LARGE_INTEGER f; /* 频率开机后不变；并发首次写入同值，无害 */
    LARGE_INTEGER c;
    if (f.QuadPart == 0) QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (uint64_t)(c.QuadPart / f.QuadPart) * 1000u + (uint64_t)(c.QuadPart % f.QuadPart) * 1000u / (uint64_t)f.QuadPart;
}
void xcp_sleep_ms(int ms) { Sleep(ms > 0 ? (DWORD)ms : 0); }

static xcp_atomic_t g_wsa_refs;
int xcp_net_init(void)
{
    WSADATA w;
    if (WSAStartup(MAKEWORD(2, 2), &w) != 0) return -1;
    xcp_atomic_add(&g_wsa_refs, 1);
    return 0;
}
void xcp_net_cleanup(void) { xcp_atomic_add(&g_wsa_refs, -1); WSACleanup(); }
void xcp_sock_close(xcp_sock_t s) { if (s != INVALID_SOCKET) closesocket(s); }
void xcp_sock_shutdown(xcp_sock_t s) { if (s != INVALID_SOCKET) shutdown(s, SD_BOTH); }

#else /* POSIX */
#include <errno.h>
#include <time.h>
#include <sys/select.h>

long xcp_atomic_load(const xcp_atomic_t* a) { return __atomic_load_n(&a->v, __ATOMIC_SEQ_CST); }
void xcp_atomic_store(xcp_atomic_t* a, long v) { __atomic_store_n(&a->v, v, __ATOMIC_SEQ_CST); }
long xcp_atomic_add(xcp_atomic_t* a, long d) { return __atomic_add_fetch(&a->v, d, __ATOMIC_SEQ_CST); }

void xcp_mutex_init(xcp_mutex_t* m) { pthread_mutex_init(&m->m, NULL); }
void xcp_mutex_destroy(xcp_mutex_t* m) { pthread_mutex_destroy(&m->m); }
void xcp_mutex_lock(xcp_mutex_t* m) { pthread_mutex_lock(&m->m); }
void xcp_mutex_unlock(xcp_mutex_t* m) { pthread_mutex_unlock(&m->m); }
void xcp_cond_init(xcp_cond_t* c)
{
    pthread_condattr_t a;
    pthread_condattr_init(&a);
    pthread_condattr_setclock(&a, CLOCK_MONOTONIC);
    pthread_cond_init(&c->c, &a);
    pthread_condattr_destroy(&a);
}
void xcp_cond_destroy(xcp_cond_t* c) { pthread_cond_destroy(&c->c); }
int xcp_cond_wait_ms(xcp_cond_t* c, xcp_mutex_t* m, int ms)
{
    struct timespec ts;
    if (ms < 0) ms = 0;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ts.tv_sec += ms / 1000;
    ts.tv_nsec += (long)(ms % 1000) * 1000000L;
    if (ts.tv_nsec >= 1000000000L) { ts.tv_sec++; ts.tv_nsec -= 1000000000L; }
    return pthread_cond_timedwait(&c->c, &m->m, &ts) == ETIMEDOUT ? 1 : 0;
}
void xcp_cond_broadcast(xcp_cond_t* c) { pthread_cond_broadcast(&c->c); }

typedef struct { xcp_thread_fn fn; void* arg; } ThrBox;
static void* thr_tramp(void* p)
{
    ThrBox b = *(ThrBox*)p;
    free(p);
    b.fn(b.arg);
    return NULL;
}
int xcp_thread_start(xcp_thread_t* t, xcp_thread_fn fn, void* arg)
{
    ThrBox* b = (ThrBox*)malloc(sizeof(ThrBox));
    if (!b) return -1;
    b->fn = fn; b->arg = arg;
    if (pthread_create(&t->t, NULL, thr_tramp, b) != 0) { free(b); t->valid = 0; return -1; }
    t->valid = 1;
    return 0;
}
void xcp_thread_join(xcp_thread_t* t)
{
    if (!t->valid) return;
    pthread_join(t->t, NULL);
    t->valid = 0;
}
int xcp_thread_is_self(const xcp_thread_t* t) { return t->valid && pthread_equal(t->t, pthread_self()); }

uint64_t xcp_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000u + (uint64_t)(ts.tv_nsec / 1000000L);
}
void xcp_sleep_ms(int ms)
{
    struct timespec ts;
    if (ms < 0) ms = 0;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    while (nanosleep(&ts, &ts) != 0 && errno == EINTR) {}
}

int xcp_net_init(void) { return 0; }
void xcp_net_cleanup(void) {}
void xcp_sock_close(xcp_sock_t s) { if (s >= 0) close(s); }
void xcp_sock_shutdown(xcp_sock_t s) { if (s >= 0) shutdown(s, SHUT_RDWR); }
#endif

int xcp_sock_wait_readable(xcp_sock_t s, int ms)
{
    fd_set rf;
    struct timeval tv;
    int r;
    if (s == XCP_INVALID_SOCK) return -1;
    FD_ZERO(&rf);
    FD_SET(s, &rf);
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    r = select((int)(s + 1), &rf, NULL, NULL, &tv);
    if (r < 0) {
#ifndef _WIN32
        if (errno == EINTR) return 0;
#endif
        return -1;
    }
    return r > 0 ? 1 : 0;
}
