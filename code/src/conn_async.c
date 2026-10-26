/* conn_async.c — 见 conn_async.h */
#include "conn_async.h"
#include <stdlib.h>
#include <string.h>

static DWORD WINAPI conn_thread(LPVOID p)
{
    OS_ConnJob* j = (OS_ConnJob*)p;
    int rc = OS_ERR_CANCELED, c = 0;
    if (!InterlockedCompareExchange(&j->cancelled, 0, 0)) {
        rc = j->drv->command(j->ctx, OS_CMD_CONNECT, &j->cfg, NULL);
        if (rc == OS_ERR_OK && InterlockedCompareExchange(&j->cancelled, 0, 0)) {
            /* 连上的瞬间用户点了断开：DISCONNECT 可能先于会话建立到达，这里补一次 */
            j->drv->command(j->ctx, OS_CMD_DISCONNECT, NULL, NULL);
            rc = OS_ERR_CANCELED;
        }
        if (rc == OS_ERR_OK) {
            j->drv->command(j->ctx, OS_CMD_IS_CONNECTED, NULL, &c);
            memset(&j->info, 0, sizeof(j->info));
            j->drv->command(j->ctx, OS_CMD_GET_INFO, NULL, &j->info);
        }
    }
    j->rc = rc;
    j->connected = (rc == OS_ERR_OK) ? c : 0;
    InterlockedExchange(&j->finished, 1);
    {
        HWND n = (HWND)InterlockedCompareExchangePointer((PVOID volatile*)&j->notify, NULL, NULL);
        if (n) PostMessageW(n, j->msg, 0, (LPARAM)j);
    }
    /* 只钉驱动 DLL；本函数在主程序里，放开后正常返回即可 */
    if (j->pin) FreeLibrary(j->pin);
    return 0;
}

OS_ConnJob* os_conn_start(const OS_Module* drv, void* ctx, const OS_ConnectCfg* cfg,
                          int attach, int driver_index, HWND notify, UINT msg)
{
    OS_ConnJob* j;
    if (!drv || !drv->command || !cfg) return NULL;
    j = (OS_ConnJob*)calloc(1, sizeof(*j));
    if (!j) return NULL;
    j->drv = drv; j->ctx = ctx; j->cfg = *cfg; j->attach = attach;
    j->driver_index = driver_index; j->notify = notify; j->msg = msg;
    j->rc = OS_ERR_FAIL;
    /* 钉住驱动 DLL（command 指针所在模块）；拿不到也继续（静态链接/测试桩） */
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            (LPCWSTR)(void*)drv->command, &j->pin))
        j->pin = NULL;
    j->thread = CreateThread(NULL, 0, conn_thread, j, 0, NULL);
    if (!j->thread) {
        if (j->pin) FreeLibrary(j->pin);
        free(j);
        return NULL;
    }
    return j;
}

void os_conn_cancel(OS_ConnJob* job)
{
    if (job) InterlockedExchange(&job->cancelled, 1);
}

int os_conn_is_cancelled(const OS_ConnJob* job)
{
    return job ? (int)InterlockedCompareExchange((volatile LONG*)&job->cancelled, 0, 0) : 0;
}

int os_conn_settle(OS_ConnJob* job)
{
    if (!job) return OS_ERR_FAIL;
    if (job->rc == OS_ERR_OK && os_conn_is_cancelled(job)) {
        job->drv->command(job->ctx, OS_CMD_DISCONNECT, NULL, NULL);
        job->rc = OS_ERR_CANCELED;
        job->connected = 0;
    }
    return job->rc;
}

int os_conn_finish(OS_ConnJob* job, DWORD ms)
{
    if (!job) return 1;
    if (WaitForSingleObject(job->thread, ms) != WAIT_OBJECT_0) return 0;
    CloseHandle(job->thread);
    free(job);
    return 1;
}

int os_conn_abandon(OS_ConnJob* job, DWORD ms)
{
    if (!job) return 1;
    os_conn_cancel(job);
    /* 窗口即将销毁：不再投递（线程读到 NULL 就不发；读到旧值也只是投递给已销毁窗口，无害） */
    InterlockedExchangePointer((PVOID volatile*)&job->notify, NULL);
    return os_conn_finish(job, ms);
}

/* ---------------- 第四轮 任务2：UI / AI 连接互斥 ---------------- */
static volatile LONG g_owner;          /* OS_CONN_OWNER_* */
static volatile LONG g_slot_cancel;    /* 本次占用期间被取消（含 AGAIN 重试等待中） */
static volatile LONG g_act_lock;       /* 保护 g_active 的极短自旋锁（无需初始化） */
static OS_ConnJob* g_active;

static void act_lock(void)   { while (InterlockedCompareExchange(&g_act_lock, 1, 0) != 0) SwitchToThread(); }
static void act_unlock(void) { InterlockedExchange(&g_act_lock, 0); }

int os_conn_claim(int owner)
{
    if (InterlockedCompareExchange(&g_owner, owner, OS_CONN_OWNER_NONE) != OS_CONN_OWNER_NONE) return 0;
    InterlockedExchange(&g_slot_cancel, 0);
    return 1;
}

void os_conn_release(int owner)
{
    InterlockedCompareExchange(&g_owner, OS_CONN_OWNER_NONE, owner);
}

int os_conn_owner(void) { return (int)InterlockedCompareExchange(&g_owner, 0, 0); }

void os_conn_set_active(OS_ConnJob* job)
{
    act_lock();
    g_active = job;
    if (job && InterlockedCompareExchange(&g_slot_cancel, 0, 0)) os_conn_cancel(job);   /* 登记前已被取消 */
    act_unlock();
}

int os_conn_cancel_active(void)
{
    int any;
    act_lock();
    any = os_conn_owner() != OS_CONN_OWNER_NONE;
    if (any) InterlockedExchange(&g_slot_cancel, 1);
    if (g_active) os_conn_cancel(g_active);
    act_unlock();
    return any;
}

int os_conn_connect_sync(int owner, const OS_Module* drv, void* ctx, const OS_ConnectCfg* cfg,
                         int attach, int driver_index, DWORD again_wait_ms, OS_ConnResult* res)
{
    ULONGLONG t0;
    memset(res, 0, sizeof(*res));
    if (!os_conn_claim(owner)) {
        res->rc = OS_ERR_AGAIN;
        res->busy_owner = os_conn_owner();
        return res->rc;
    }
    t0 = GetTickCount64();
    for (;;) {
        OS_ConnJob* j = os_conn_start(drv, ctx, cfg, attach, driver_index, NULL, 0);
        if (!j) { res->rc = OS_ERR_FAIL; break; }
        os_conn_set_active(j);
        WaitForSingleObject(j->thread, INFINITE);   /* 可取消：断开 → 驱动取消点 → 线程返回 */
        act_lock();                                 /* 先注销再读/释放：另一方不会再碰 j */
        g_active = NULL;
        act_unlock();
        os_conn_settle(j);                          /* 最后一刻被取消但已连上 → 补 DISCONNECT */
        res->rc = j->rc;
        res->connected = j->connected;
        res->info = j->info;
        os_conn_finish(j, INFINITE);
        if (res->rc != OS_ERR_AGAIN) break;
        if (InterlockedCompareExchange(&g_slot_cancel, 0, 0)) { res->rc = OS_ERR_CANCELED; break; }
        if (GetTickCount64() - t0 >= again_wait_ms) break;
        {   /* 等 250ms 再试；分片等待，被取消时 ≤10ms 内返回 */
            ULONGLONG w = GetTickCount64();
            while (GetTickCount64() - w < 250 && !InterlockedCompareExchange(&g_slot_cancel, 0, 0)) Sleep(10);
        }
        if (InterlockedCompareExchange(&g_slot_cancel, 0, 0)) { res->rc = OS_ERR_CANCELED; break; }
    }
    os_conn_release(owner);
    return res->rc;
}
