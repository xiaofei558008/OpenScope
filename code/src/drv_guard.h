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
 *  5. 独占开始时先 drv_drain()：等独占前已进锁的调用离开，之后独占者独享厂商 DLL。
 *  6. 限时等锁以 QPC 截止时刻为准（drv_try_cs_until），不受 Windows 15.6ms 计时粒度影响。
 *  7. 卸载交接：deinit 时仍有线程卡在厂商 DLL → drv_guard_shutdown_or_orphan 登记收尾函数，
 *     由最后离开独占区的线程执行；mod_init 先 drv_guard_wait_orphan。
 */
#ifndef OS_DRV_GUARD_H
#define OS_DRV_GUARD_H

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "module_api.h"

/* header-only：全部函数声明为 static inline（DRV_FN）。未被某个驱动引用的 inline 函数
 * 不会触发 MSVC C4505 / gcc -Wunused-function，因此不需要任何 pragma 关警告。 */
#if defined(_MSC_VER) && !defined(__cplusplus)
#define DRV_FN static __inline
#else
#define DRV_FN static inline
#endif

#ifndef OS_ERR_AGAIN
#define OS_ERR_AGAIN -9
#endif
#ifndef OS_DRV_UI_LOCK_MS
#define OS_DRV_UI_LOCK_MS 50
#endif
/* 限时等锁的“粗睡”阈值：剩余时间 > 此值才 Sleep(1)（Windows 默认时钟粒度 15.6ms，
 * Sleep(1) 实际可能睡满一个 tick）；剩余 ≤ 此值改为 SwitchToThread/Sleep(0) 让出，
 * 最后 OS_DRV_SPIN_US 微秒纯自旋，保证在截止时刻（QPC）准时返回。 */
#ifndef OS_DRV_COARSE_MS
#define OS_DRV_COARSE_MS 20
#endif
#ifndef OS_DRV_SPIN_US
#define OS_DRV_SPIN_US 500
#endif
/* 第五轮：粗睡阈值自适应上限。Sleep(1) 在 Windows 上通常 ≤1 tick（15.6ms），但 CPU 有竞争时
 * 线程到点后还要等调度（再 1~2 个 tick），实测偶发 >20ms → 等锁超时越界到 ~67ms（50+17）。
 * 记录本进程观察到的 Sleep(1) 最坏耗时，阈值取 max(OS_DRV_COARSE_MS, 最坏+2ms)，不超过此上限。 */
#ifndef OS_DRV_COARSE_MAX_MS
#define OS_DRV_COARSE_MAX_MS 40
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
    /* 卸载交接（风险1）：deinit 时重连/连接仍卡在厂商 DLL 里 → 不阻塞宿主、也不立即释放；
     * 由最后一个离开独占区的线程执行 cleanup_fn（关会话、卸厂商 DLL、删锁），见 drv_guard_shutdown_or_orphan */
    volatile LONG orphaned, cleanup_claimed, cleanup_done;
    void (*cleanup_fn)(void);
#ifdef _WIN32
    HMODULE pin;
#endif
} OS_DrvGuard;

DRV_FN LONGLONG drv_qpc(void)
{
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (LONGLONG)c.QuadPart;
}

DRV_FN LONGLONG drv_qpf(void)
{
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);   /* 开机后恒定；每次查询，避免 static 缓存的数据竞争 */
    return f.QuadPart > 0 ? (LONGLONG)f.QuadPart : 1;
}

DRV_FN ULONGLONG drv_now_ms(void)
{
    LONGLONG f = drv_qpf(), c = drv_qpc();
    return (ULONGLONG)(c / (f / 1000 ? f / 1000 : 1));
}

/* 现在起 ms 毫秒后的 QPC 截止时刻 */
DRV_FN LONGLONG drv_deadline(DWORD ms)
{
    return drv_qpc() + (LONGLONG)ms * drv_qpf() / 1000;
}

DRV_FN void drv_cpu_relax(void)
{
#if defined(_MSC_VER)
    YieldProcessor();
#elif defined(__i386__) || defined(__x86_64__)
    __builtin_ia32_pause();
#endif
}

DRV_FN void drv_guard_init(OS_DrvGuard* g, CRITICAL_SECTION* cs, int (*fn)(void), DWORD min_interval_ms)
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

DRV_FN int drv_is_owner(OS_DrvGuard* g)
{
    return InterlockedCompareExchange(&g->owner_tid, 0, 0) == (LONG)GetCurrentThreadId();
}

DRV_FN int drv_is_reconnecting(OS_DrvGuard* g)
{
    return InterlockedCompareExchange(&g->reconnecting, 0, 0) != 0 && !drv_is_owner(g);
}

/* 精确限时 TryEnter：在 QPC 截止时刻 deadline 之前拿到 cs 返回 1，否则 0。
 * check_reconn=1 时，一旦发现正在重连立刻放弃（返回 0，不持锁）。
 * 等待策略（Windows 计时粒度 15.6ms 的修正）：
 *   剩余 > OS_DRV_COARSE_MS   → Sleep(1)（可能睡满 15.6ms，但离截止还远，不会越界）
 *   剩余 > OS_DRV_SPIN_US     → SwitchToThread()，无可让线程时 Sleep(0)
 *   最后 OS_DRV_SPIN_US 微秒  → 纯自旋（pause）
 * 截止判断只用 QPC，因此超时返回的误差 = 一次 TryEnter + 一次让出，CPU 不饱和时为微秒级。 */
static volatile LONG g_drv_sleep1_worst_us;   /* 观察到的 Sleep(1) 最坏耗时（每个包含本头文件的模块各一份） */

/* 当前粗睡阈值（QPC 计数） */
DRV_FN LONGLONG drv_coarse_qpc(LONGLONG f)
{
    LONGLONG us = (LONGLONG)InterlockedCompareExchange(&g_drv_sleep1_worst_us, 0, 0) + 2000;
    if (us < (LONGLONG)OS_DRV_COARSE_MS * 1000) us = (LONGLONG)OS_DRV_COARSE_MS * 1000;
    if (us > (LONGLONG)OS_DRV_COARSE_MAX_MS * 1000) us = (LONGLONG)OS_DRV_COARSE_MAX_MS * 1000;
    return f * us / 1000000;
}

/* Sleep(1) 并记录实际耗时 */
DRV_FN void drv_sleep1_tracked(LONGLONG f)
{
    LONGLONG t = drv_qpc(), us;
    LONG old;
    Sleep(1);
    us = (drv_qpc() - t) * 1000000 / f;
    if (us > 1000000) us = 1000000;
    old = InterlockedCompareExchange(&g_drv_sleep1_worst_us, 0, 0);
    while ((LONG)us > old) {
        LONG seen = InterlockedCompareExchange(&g_drv_sleep1_worst_us, (LONG)us, old);
        if (seen == old) break;
        old = seen;
    }
}

DRV_FN int drv_try_cs_until(OS_DrvGuard* g, CRITICAL_SECTION* cs, LONGLONG deadline, int check_reconn)
{
    LONGLONG f = drv_qpf();
    LONGLONG coarse = drv_coarse_qpc(f);
    LONGLONG spin = f * OS_DRV_SPIN_US / 1000000;
    for (;;) {
        LONGLONG left;
        if (check_reconn && g && drv_is_reconnecting(g)) return 0;
        if (TryEnterCriticalSection(cs)) return 1;
        left = deadline - drv_qpc();
        if (left <= 0) return 0;
        if (left > coarse) { drv_sleep1_tracked(f); coarse = drv_coarse_qpc(f); }
        else if (left > spin) { if (!SwitchToThread()) Sleep(0); }
        else drv_cpu_relax();
    }
}

/* 限时加锁。成功返回 OS_ERR_OK（持有 cs）；否则 OS_ERR_AGAIN（未持锁）。 */
DRV_FN int drv_lock_until(OS_DrvGuard* g, LONGLONG deadline)
{
    if (drv_is_reconnecting(g)) return OS_ERR_AGAIN;
    if (!drv_try_cs_until(g, g->cs, deadline, 1)) return OS_ERR_AGAIN;
    if (drv_is_reconnecting(g)) { LeaveCriticalSection(g->cs); return OS_ERR_AGAIN; }
    return OS_ERR_OK;
}

DRV_FN int drv_lock_timed(OS_DrvGuard* g, DWORD timeout_ms)
{
    return drv_lock_until(g, drv_deadline(timeout_ms));
}

DRV_FN int drv_lock_ui(OS_DrvGuard* g)
{
    return drv_lock_timed(g, (DWORD)g->ui_lock_ms);
}

/* 工作线程（采集）用的阻塞加锁：仍然尊重 reconnecting（返回 AGAIN），但锁本身会等。 */
DRV_FN int drv_lock_worker(OS_DrvGuard* g)
{
    if (drv_is_reconnecting(g)) return OS_ERR_AGAIN;
    EnterCriticalSection(g->cs);
    if (drv_is_reconnecting(g)) { LeaveCriticalSection(g->cs); return OS_ERR_AGAIN; }
    return OS_ERR_OK;
}

DRV_FN void drv_unlock(OS_DrvGuard* g) { LeaveCriticalSection(g->cs); }

/* 独占开始后调用：等已经在锁内的调用（独占开始前进入的读/UI 命令）全部离开，最多 wait_ms。
 * 之后再没有线程能进锁（都会看到 reconnecting=1 → AGAIN），独占者可以不持锁调用厂商 DLL。 */
#ifndef OS_DRV_DRAIN_MS
#define OS_DRV_DRAIN_MS 2000
#endif
DRV_FN void drv_drain(OS_DrvGuard* g, DWORD wait_ms)
{
    if (drv_try_cs_until(NULL, g->cs, drv_deadline(wait_ms), 0)) LeaveCriticalSection(g->cs);
}

/* 同步独占（用户点 CONNECT）：成功返回 OS_ERR_OK，已有重连在跑则 OS_ERR_AGAIN。 */
DRV_FN int drv_begin_exclusive(OS_DrvGuard* g)
{
    if (InterlockedCompareExchange(&g->reconnecting, 1, 0) != 0) return OS_ERR_AGAIN;
    InterlockedExchange(&g->owner_tid, (LONG)GetCurrentThreadId());
    drv_drain(g, OS_DRV_DRAIN_MS);
    return OS_ERR_OK;
}

/* 卸载交接：恰好一个线程（deinit 或最后离开独占区者）执行 cleanup_fn */
DRV_FN void drv_orphan_try_cleanup(OS_DrvGuard* g)
{
    if (!InterlockedCompareExchange(&g->orphaned, 0, 0)) return;
    if (InterlockedCompareExchange(&g->reconnecting, 0, 0)) return;
    if (InterlockedCompareExchange(&g->cleanup_claimed, 1, 0) != 0) return;
    if (g->cleanup_fn) g->cleanup_fn();
    InterlockedExchange(&g->cleanup_done, 1);
}

DRV_FN void drv_end_exclusive(OS_DrvGuard* g)
{
    InterlockedExchange(&g->owner_tid, 0);
    InterlockedExchange(&g->reconnecting, 0);
    drv_orphan_try_cleanup(g);   /* deinit 已交接：本线程是最后一个用户 → 收尾 */
}

DRV_FN DWORD WINAPI drv_reconnect_thread(LPVOID p)
{
    OS_DrvGuard* g = (OS_DrvGuard*)p;
#ifdef _WIN32
    HMODULE pin = g->pin;
#endif
    InterlockedExchange(&g->owner_tid, (LONG)GetCurrentThreadId());
    drv_drain(g, OS_DRV_DRAIN_MS);
    if (g->reconnect_fn) g->reconnect_fn();
    drv_end_exclusive(g);
#ifdef _WIN32
    if (pin) FreeLibraryAndExitThread(pin, 0);
#endif
    return 0;
}

/* 请求后台重连（节流 min_interval_ms；已在跑则不重复）。总是立即返回。
 * 返回 OS_ERR_AGAIN（重连已在进行/已启动）或 OS_ERR_NOT_CONNECTED（节流中）。 */
DRV_FN int drv_kick_reconnect(OS_DrvGuard* g)
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

/* mod_init 开头调用：上一次 deinit 交接出去的收尾若还没完成（线程仍卡在厂商 DLL），最多等 wait_ms。
 * 返回 1 = 可以重新初始化；0 = 仍在收尾（调用方应返回 OS_ERR_AGAIN，不得覆盖锁/会话）。 */
DRV_FN int drv_guard_wait_orphan(OS_DrvGuard* g, DWORD wait_ms)
{
    LONGLONG dl = drv_deadline(wait_ms);
    if (!InterlockedCompareExchange(&g->orphaned, 0, 0)) return 1;
    while (!InterlockedCompareExchange(&g->cleanup_done, 0, 0)) {
        if (drv_qpc() >= dl) return 0;
        Sleep(10);
    }
    return 1;
}

/* 卸载前调用：等后台重连结束（最多 wait_ms）。返回 1 表示线程已结束，0 表示仍在跑。 */
DRV_FN int drv_guard_shutdown(OS_DrvGuard* g, DWORD wait_ms)
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

/* 卸载（风险1 改进）：最多等 wait_ms，等后台重连线程结束、且没有任何线程处于独占区（包括宿主
 * 工作线程里正在执行的同步 CONNECT）。
 *  返回 1：已空闲，调用方照常同步清理；
 *  返回 0：仍有线程卡在厂商 DLL 里。此时登记 cleanup_fn 并交接：卡住的线程一旦返回、离开独占区，
 *          由它执行 cleanup_fn（关会话、卸厂商 DLL、删锁）——不再“永久泄漏”，宿主也不被阻塞。
 *          调用方此时不得再碰锁/会话/厂商 DLL。后台线程钉住了本模块 DLL（GetModuleHandleEx），
 *          宿主的 FreeLibrary 不会把正在执行的代码卸掉；宿主侧的连接线程同样须钉住驱动 DLL。 */
DRV_FN int drv_guard_shutdown_or_orphan(OS_DrvGuard* g, DWORD wait_ms, void (*cleanup_fn)(void))
{
    LONGLONG dl = drv_deadline(wait_ms);
    int thr_done = drv_guard_shutdown(g, wait_ms);
    while (thr_done && InterlockedCompareExchange(&g->reconnecting, 0, 0) && drv_qpc() < dl) Sleep(10);
    if (thr_done && !InterlockedCompareExchange(&g->reconnecting, 0, 0)) return 1;
    g->cleanup_fn = cleanup_fn;
    InterlockedExchange(&g->orphaned, 1);
    drv_orphan_try_cleanup(g);    /* 竞态：线程恰在此刻结束 → 由本线程收尾 */
    return 0;
}

/* ===== 第五轮：断开时拿不到锁 → 延迟关闭厂商会话（而不是泄漏到下次重载/卸载） =====
 * 场景：采集线程正卡在厂商读函数里（持锁），UI 点断开。此时不能并发 Close（Bug16：会损坏会话），
 * 也不能阻塞 UI。做法：登记 pending，由一个短命后台线程等锁（drv_lock_timed，尊重独占区）后
 * 执行 close_fn；若期间有人以独占身份先到（重连/重新连接/卸载），由它调用 drv_defer_run 先关。
 * close_fn 在持锁或独占区内调用，只在 pending 被本次取走时执行一次。 */
typedef struct OS_DrvDefer {
    OS_DrvGuard*   g;
    void         (*close_fn)(void);
    volatile LONG  pending;
    volatile LONG  running;   /* 后台关闭线程存活 */
    volatile LONG  quit;      /* 卸载：让后台线程放弃等待 */
} OS_DrvDefer;

DRV_FN void drv_defer_init(OS_DrvDefer* d, OS_DrvGuard* g, void (*close_fn)(void))
{
    d->g = g; d->close_fn = close_fn;
    InterlockedExchange(&d->pending, 0);
    InterlockedExchange(&d->quit, 0);
}

DRV_FN int drv_defer_pending(OS_DrvDefer* d) { return InterlockedCompareExchange(&d->pending, 0, 0) != 0; }

/* 调用方须持锁或处于独占区 */
DRV_FN void drv_defer_run(OS_DrvDefer* d)
{
    if (InterlockedExchange(&d->pending, 0) && d->close_fn) d->close_fn();
}

DRV_FN DWORD WINAPI drv_defer_thread(LPVOID p)
{
    OS_DrvDefer* d = (OS_DrvDefer*)p;
#ifdef _WIN32
    HMODULE pin = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)(void*)&drv_defer_thread, &pin);
#endif
    while (drv_defer_pending(d) && !InterlockedCompareExchange(&d->quit, 0, 0)) {
        if (drv_lock_timed(d->g, 20) == OS_ERR_OK) {
            drv_defer_run(d);
            drv_unlock(d->g);
            break;
        }
        Sleep(5);   /* 独占区（重连/连接）进行中：它会自己 drv_defer_run */
    }
    InterlockedExchange(&d->running, 0);
#ifdef _WIN32
    if (pin) FreeLibraryAndExitThread(pin, 0);
#endif
    return 0;
}

/* 登记延迟关闭并确保后台线程在跑；立即返回。线程创建失败时 pending 保留，由下次独占者执行 */
DRV_FN void drv_defer_request(OS_DrvDefer* d)
{
    HANDLE h;
    InterlockedExchange(&d->pending, 1);
    if (InterlockedCompareExchange(&d->running, 1, 0) != 0) return;
    h = CreateThread(NULL, 0, drv_defer_thread, d, 0, NULL);
    if (h) CloseHandle(h);
    else InterlockedExchange(&d->running, 0);
}

/* 卸载：让后台线程退出（最多 wait_ms）；返回 1 = 已退出。之后调用方（独占）自行 drv_defer_run */
DRV_FN int drv_defer_stop(OS_DrvDefer* d, DWORD wait_ms)
{
    LONGLONG dl = drv_deadline(wait_ms);
    InterlockedExchange(&d->quit, 1);
    while (InterlockedCompareExchange(&d->running, 0, 0) && drv_qpc() < dl) Sleep(1);
    return !InterlockedCompareExchange(&d->running, 0, 0);
}

#endif /* OS_DRV_GUARD_H */
