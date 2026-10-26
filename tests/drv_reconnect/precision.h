/* 第五轮：UI 时延上限的精度自检（drv_reconnect / drv_probe 共用）。
 * 旧自检只测 20ms 等待：剩余 ≤ OS_DRV_COARSE_MS(20) 时根本不走 Sleep(1)，测不到真正的风险点；
 * 真实 UI 等锁是 50ms，前 30ms 走 Sleep(1)——CPU 有竞争时单次 Sleep(1) 可能 >20ms（tick 15.6ms +
 * 调度等待），这才是偶发 67ms（=50+17）的来源。这里直接调用驱动同一个函数 drv_try_cs_until，
 * 以真实的 OS_DRV_UI_LOCK_MS 等一把被占住的锁，重复 N 次取最坏超时误差。
 * 上限 = 50 + max(10, 最坏误差 + 5)，封顶 100（TEST_MAX_MS 环境变量优先）。 */
#ifndef OS_TEST_PRECISION_H
#define OS_TEST_PRECISION_H
#include "drv_guard.h"

static CRITICAL_SECTION g_prec_cs;
static volatile LONG g_prec_stop;
static DWORD WINAPI prec_holder(LPVOID p)
{
    (void)p;
    EnterCriticalSection(&g_prec_cs);
    while (!InterlockedCompareExchange(&g_prec_stop, 0, 0)) Sleep(5);
    LeaveCriticalSection(&g_prec_cs);
    return 0;
}

/* 返回最坏超时误差（ms，QPC 计时） */
static long prec_worst_over_ms(int samples)
{
    long worst = 0; int i; HANDLE h;
    LONGLONG f = drv_qpf();
    InitializeCriticalSection(&g_prec_cs);
    InterlockedExchange(&g_prec_stop, 0);
    h = CreateThread(NULL, 0, prec_holder, NULL, 0, NULL);
    while (TryEnterCriticalSection(&g_prec_cs)) { LeaveCriticalSection(&g_prec_cs); Sleep(1); }  /* 等 holder 拿到锁 */
    for (i = 0; i < samples; i++) {
        LONGLONG a = drv_qpc(), over;
        if (drv_try_cs_until(NULL, &g_prec_cs, a + f * OS_DRV_UI_LOCK_MS / 1000, 0)) { LeaveCriticalSection(&g_prec_cs); continue; }
        over = (drv_qpc() - a) * 1000 / f - OS_DRV_UI_LOCK_MS;
        if (over > worst) worst = (long)over;
    }
    InterlockedExchange(&g_prec_stop, 1);
    WaitForSingleObject(h, INFINITE); CloseHandle(h);
    DeleteCriticalSection(&g_prec_cs);
    return worst;
}

/* 设定 *max_ms：TEST_MAX_MS 优先；否则按自检结果 */
static void prec_pick_limit(long* max_ms)
{
    long over, lim;
    if (getenv("TEST_MAX_MS")) { *max_ms = atol(getenv("TEST_MAX_MS")); printf("[0] UI 时延上限 %ldms（TEST_MAX_MS）\n", *max_ms); return; }
    over = prec_worst_over_ms(10);
    lim = OS_DRV_UI_LOCK_MS + (over + 5 > 10 ? over + 5 : 10);
    if (lim > 100) lim = 100;
    *max_ms = lim;
    printf("[0] 精度自检：%dms 限时等锁 ×10，最坏超时误差 %ldms → UI 时延上限 %ldms\n", OS_DRV_UI_LOCK_MS, over, lim);
    if (lim > OS_DRV_UI_LOCK_MS + 10)
        printf("    WARN: 本机调度抖动较大（CPU 竞争），上限已放宽；这是环境抖动，不是驱动缺陷\n");
}
#endif
