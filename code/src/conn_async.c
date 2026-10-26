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
