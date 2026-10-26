/* ai_step.c — 见 ai_step.h */
#include "ai_step.h"
#include "cmd_async.h"

static volatile LONG g_lk;          /* 小自旋锁：只保护下面几个字段，临界区内不调用驱动 */
static OS_CmdJob*    g_job;         /* 在途的 AI StepOut（NULL = 无） */
static int           g_last_rc;     /* 最近一次完成的结果 */
static int           g_have_last;   /* 0 = 从未完成过 */
static volatile LONG g_abort;       /* 关停：打断等待、拒绝新请求 */
static unsigned      g_gen;         /* 每次启动 +1 */
static unsigned      g_job_gen;     /* g_job 的代号 */
static unsigned      g_last_gen;    /* g_last_rc 属于哪一代 */

static void lk(void)   { while (InterlockedCompareExchange(&g_lk, 1, 0) != 0) { if (!SwitchToThread()) Sleep(0); } }
static void unlk(void) { InterlockedExchange(&g_lk, 0); }

/* 持锁调用：job 已完成则收割（工作线程在 finished=1 后只剩 PostMessage(NULL 跳过)+FreeLibrary，很快结束） */
static int reap_locked(void)
{
    OS_CmdJob* j = g_job;
    if (!j || !InterlockedCompareExchange(&j->finished, 0, 0)) return 0;
    g_last_rc = j->rc;
    g_last_gen = g_job_gen;
    g_have_last = 1;
    g_job = NULL;
    unlk();
    os_cmd_finish(j, INFINITE);   /* 锁外等线程句柄（微秒级） */
    lk();
    return 1;
}

int os_ai_step_out(const OS_Module* drv, void* ctx, int driver_index, DWORD wait_ms, int* out_rc)
{
    OS_CmdJob* j;
    unsigned my;
    LARGE_INTEGER f, t0, t;
    if (out_rc) *out_rc = OS_ERR_FAIL;
    if (InterlockedCompareExchange(&g_abort, 0, 0)) return OS_ERR_CANCELED;
    lk();
    reap_locked();
    if (g_job) { unlk(); return OS_ERR_AGAIN; }
    j = os_cmd_start(drv, ctx, OS_CMD_STEP_OUT, driver_index, NULL, 0);
    if (!j) { unlk(); return OS_ERR_FAIL; }
    g_job = j;
    g_job_gen = ++g_gen;
    my = g_job_gen;
    unlk();
    if (!wait_ms) return 0;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t0);
    for (;;) {
        int mine, fin = 0;
        lk();
        mine = (g_job == j);
        if (mine) fin = InterlockedCompareExchange(&j->finished, 0, 0) != 0;
        if (mine && fin) reap_locked();
        if (!mine || fin) {
            /* 自己收割了，或别的线程（step_out.status / 下一次 step_out）先收割了：结果在 g_last_rc；
             * 被 abort 拿走（未完成）则 g_last_gen 不是本代 → 报 pending */
            int done = g_have_last && g_last_gen == my, rc = g_last_rc;
            unlk();
            if (!done) return 0;
            if (out_rc) *out_rc = rc;
            return 1;
        }
        unlk();
        if (InterlockedCompareExchange(&g_abort, 0, 0)) return 0;   /* 关停：job 交给 abort 处理 */
        QueryPerformanceCounter(&t);
        if ((t.QuadPart - t0.QuadPart) * 1000 / f.QuadPart >= (LONGLONG)wait_ms) return 0;
        Sleep(5);
    }
}

int os_ai_step_status(int* out_rc)
{
    int r;
    lk();
    reap_locked();
    if (g_job) r = 0;
    else if (g_have_last) { r = 1; if (out_rc) *out_rc = g_last_rc; }
    else r = -1;
    unlk();
    return r;
}

int os_ai_step_busy(void)
{
    int b;
    lk();
    b = g_job != NULL && !InterlockedCompareExchange(&g_job->finished, 0, 0);
    unlk();
    return b;
}

int os_ai_step_abort(DWORD ms)
{
    OS_CmdJob* j;
    InterlockedExchange(&g_abort, 1);
    lk();
    j = g_job;
    g_job = NULL;          /* 等待者看到 g_job != j 即不再碰 j */
    unlk();
    if (!j) return 1;
    if (!InterlockedCompareExchange(&j->finished, 0, 0))
        j->drv->command(j->ctx, OS_CMD_HALT, NULL, NULL);   /* 驱动 ≤10ms 内退出轮询 */
    return os_cmd_abandon(j, ms);
}
