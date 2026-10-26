/*
 * drv_guard.h —— 探针驱动会话锁 + 后台重连（header-only，C99，MSVC/gcc 通用）
 *
 * 规则（Bug4 修复）：
 *  1. 重连（open/qry/open_core、DLL 重载……）一律在后台线程里、且不持 g_ctx.cs 执行；
 *     只在“换入新句柄”时短暂持锁。重连期间 reconnecting=1。
 *  2. 面向 UI 的命令（CPU 状态、断点、寄存器、小块读）用 drv_lock_ui()：
 *     正在重连，或 OS_DRV_UI_LOCK_MS（默认 50ms，可用环境变量
 *     OPENSCOPE_DRV_LOCK_MS 覆盖）内拿不到锁 → 返回 OS_ERR_AGAIN，绝不无限等。
 *  3. 重连线程自己（以及同步 CONNECT 的调用线程）是“独占者”，drv_lock_ui 对它不做
 *     reconnecting 检查，避免自锁。
 *  4. Windows 下后台线程持有本 DLL 的引用（GetModuleHandleEx 钉住），退出时
 *     FreeLibraryAndExitThread，宿主 FreeLibrary 驱动 DLL 时不会把正在跑的代码卸掉。
 */
#ifndef OS_DRV_GUARD_H
#define OS_DRV_GUARD_H

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "module_api.h"

#if defined(_MSC_VER)
#pragma warning(disable: 4505)  /* 未引用的 static 函数（header-only） */
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#ifndef OS_ERR_AGAIN
#define OS_ERR_AGAIN -9
#endif
#ifndef OS_DRV_UI_LOCK_MS
#define OS_DRV_UI_LOCK_MS 50
#endif
#ifndef OS_DRV_SMALL_IO
#define OS_DRV_SMALL_IO 256   /* ≤ 此字节数的读/写走限时锁（UI 读局部变量、断点 FPB 写等） */
#endif

typedef struct OS_DrvGuard {
    CRITICAL_SECTION* cs;
    volatile LONG reconnecting;      /* 1 = 有重连/连接正在进行（会话不可用） */
    volatile LONG owner_tid;         /* 正在重连/连接的线程 id（独占者） */
    volatile LONG ui_lock_ms;
    volatile LONG last_kick_ms;      /* 低 32 位 tick，仅用于节流 */
    DWORD min_interval_ms;
    CRITICAL_SECTION thr_cs;         /* 保护 thr（发起者写、下一个发起者/关闭者读） */
    HANDLE thr;
    int (*reconnect_fn)(void);       /* 后台执行；调用时不持 cs */
#ifdef _WIN32
    HMODULE pin;
#endif
} OS_DrvGuard;

static ULONGLONG drv_now_ms(void)
{
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (ULONGLONG)(c.QuadPart / (f.QuadPart / 1000 ? f.QuadPart / 1000 : 1));
}

static void drv_guard_init(OS_DrvGuard* g, CRITICAL_SECTION* cs, int (*fn)(void), DWORD min_interval_ms)
{
    char env[16];
    DWORD n;
    memset(g, 0, sizeof(*g));
    g->cs = cs;
    g->reconnect_fn = fn;
    g->min_interval_ms = min_interval_ms;
    g->ui_lock_ms = OS_DRV_UI_LOCK_MS;
    InitializeCriticalSection(&g->thr_cs);
    n = GetEnvironmentVariableA("OPENSCOPE_DRV_LOCK_MS", env, sizeof(env));
    if (n > 0 && n < sizeof(env)) {
        int v = atoi(env);
        if (v >= 1 && v <= 10000) g->ui_lock_ms = v;
    }
}

static int drv_is_owner(OS_DrvGuard* g)
{
    return InterlockedCompareExchange(&g->owner_tid, 0, 0) == (LONG)GetCurrentThreadId();
}

static int drv_is_reconnecting(OS_DrvGuard* g)
{
    return InterlockedCompareExchange(&g->reconnecting, 0, 0) != 0 && !drv_is_owner(g);
}

/* 限时加锁。成功返回 OS_ERR_OK（持有 cs）；否则 OS_ERR_AGAIN（未持锁）。 */
static int drv_lock_timed(OS_DrvGuard* g, DWORD timeout_ms)
{
    ULONGLONG t0 = drv_now_ms();
    unsigned spins = 0;
    for (;;) {
        if (drv_is_reconnecting(g)) return OS_ERR_AGAIN;
        if (TryEnterCriticalSection(g->cs)) {
            if (drv_is_reconnecting(g)) { LeaveCriticalSection(g->cs); return OS_ERR_AGAIN; }
            return OS_ERR_OK;
        }
        if (drv_now_ms() - t0 >= timeout_ms) return OS_ERR_AGAIN;
        if (++spins < 16) SwitchToThread(); else Sleep(1);
    }
}

static int drv_lock_ui(OS_DrvGuard* g)
{
    return drv_lock_timed(g, (DWORD)g->ui_lock_ms);
}

/* 工作线程（采集）用的阻塞加锁：仍然尊重 reconnecting（返回 AGAIN），但锁本身会等。 */
static int drv_lock_worker(OS_DrvGuard* g)
{
    if (drv_is_reconnecting(g)) return OS_ERR_AGAIN;
    EnterCriticalSection(g->cs);
    if (drv_is_reconnecting(g)) { LeaveCriticalSection(g->cs); return OS_ERR_AGAIN; }
    return OS_ERR_OK;
}

static void drv_unlock(OS_DrvGuard* g) { LeaveCriticalSection(g->cs); }

/* 同步独占（用户点 CONNECT）：成功返回 OS_ERR_OK，已有重连在跑则 OS_ERR_AGAIN。 */
static int drv_begin_exclusive(OS_DrvGuard* g)
{
    if (InterlockedCompareExchange(&g->reconnecting, 1, 0) != 0) return OS_ERR_AGAIN;
    InterlockedExchange(&g->owner_tid, (LONG)GetCurrentThreadId());
    return OS_ERR_OK;
}

static void drv_end_exclusive(OS_DrvGuard* g)
{
    InterlockedExchange(&g->owner_tid, 0);
    InterlockedExchange(&g->reconnecting, 0);
}

static DWORD WINAPI drv_reconnect_thread(LPVOID p)
{
    OS_DrvGuard* g = (OS_DrvGuard*)p;
#ifdef _WIN32
    HMODULE pin = g->pin;
#endif
    InterlockedExchange(&g->owner_tid, (LONG)GetCurrentThreadId());
    if (g->reconnect_fn) g->reconnect_fn();
    drv_end_exclusive(g);
#ifdef _WIN32
    if (pin) FreeLibraryAndExitThread(pin, 0);
#endif
    return 0;
}

/* 请求后台重连（节流 min_interval_ms；已在跑则不重复）。总是立即返回。
 * 返回 OS_ERR_AGAIN（重连已在进行/已启动）或 OS_ERR_NOT_CONNECTED（节流中）。 */
static int drv_kick_reconnect(OS_DrvGuard* g)
{
    LONG now = (LONG)(DWORD)drv_now_ms();
    LONG last;
    if (InterlockedCompareExchange(&g->reconnecting, 0, 0)) return OS_ERR_AGAIN;
    last = InterlockedCompareExchange(&g->last_kick_ms, 0, 0);
    if (last && (DWORD)(now - last) < g->min_interval_ms) return OS_ERR_NOT_CONNECTED;
    if (InterlockedCompareExchange(&g->reconnecting, 1, 0) != 0) return OS_ERR_AGAIN;
    InterlockedExchange(&g->last_kick_ms, now ? now : 1);
    EnterCriticalSection(&g->thr_cs);
    if (g->thr) { CloseHandle(g->thr); g->thr = NULL; }   /* 上一个线程已结束（reconnecting 曾为 0） */
#ifdef _WIN32
    g->pin = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                       (LPCWSTR)(void*)&drv_reconnect_thread, &g->pin);
#endif
    g->thr = CreateThread(NULL, 0, drv_reconnect_thread, g, 0, NULL);
    if (!g->thr) {
#ifdef _WIN32
        if (g->pin) { FreeLibrary(g->pin); g->pin = NULL; }
#endif
        LeaveCriticalSection(&g->thr_cs);
        InterlockedExchange(&g->reconnecting, 0);
        return OS_ERR_NOT_CONNECTED;
    }
    LeaveCriticalSection(&g->thr_cs);
    return OS_ERR_AGAIN;
}

/* 卸载前调用：等后台重连结束（最多 wait_ms）。返回 1 表示线程已结束，0 表示仍在跑。 */
static int drv_guard_shutdown(OS_DrvGuard* g, DWORD wait_ms)
{
    int done = 1;
    HANDLE h;
    EnterCriticalSection(&g->thr_cs);
    h = g->thr;
    LeaveCriticalSection(&g->thr_cs);
    if (h) {
        done = WaitForSingleObject(h, wait_ms) == WAIT_OBJECT_0;
        if (done) {
            EnterCriticalSection(&g->thr_cs);
            if (g->thr == h) { CloseHandle(h); g->thr = NULL; }
            LeaveCriticalSection(&g->thr_cs);
        }
    }
    return done;
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif /* OS_DRV_GUARD_H */
