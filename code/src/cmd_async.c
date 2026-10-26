/* cmd_async.c — 见 cmd_async.h */
#include "cmd_async.h"
#include <stdlib.h>

static DWORD WINAPI cmd_thread(LPVOID p)
{
    OS_CmdJob* j = (OS_CmdJob*)p;
    HWND n;
    j->rc = j->drv->command(j->ctx, j->cmd, NULL, NULL);
    InterlockedExchange(&j->finished, 1);
    n = (HWND)InterlockedCompareExchangePointer((PVOID volatile*)&j->notify, NULL, NULL);
    if (n) PostMessageW(n, j->msg, 0, (LPARAM)j);
    if (j->pin) FreeLibrary(j->pin);   /* 本函数在主程序里：放开驱动 DLL 后正常返回 */
    return 0;
}

OS_CmdJob* os_cmd_start(const OS_Module* drv, void* ctx, int cmd, int driver_index, HWND notify, UINT msg)
{
    OS_CmdJob* j;
    if (!drv || !drv->command) return NULL;
    j = (OS_CmdJob*)calloc(1, sizeof(*j));
    if (!j) return NULL;
    j->drv = drv; j->ctx = ctx; j->cmd = cmd; j->driver_index = driver_index;
    j->notify = notify; j->msg = msg; j->rc = OS_ERR_FAIL;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)(void*)drv->command, &j->pin))
        j->pin = NULL;
    j->thread = CreateThread(NULL, 0, cmd_thread, j, 0, NULL);
    if (!j->thread) {
        if (j->pin) FreeLibrary(j->pin);
        free(j);
        return NULL;
    }
    return j;
}

int os_cmd_finish(OS_CmdJob* job, DWORD ms)
{
    if (!job) return 1;
    if (WaitForSingleObject(job->thread, ms) != WAIT_OBJECT_0) return 0;
    CloseHandle(job->thread);
    free(job);
    return 1;
}

int os_cmd_abandon(OS_CmdJob* job, DWORD ms)
{
    if (!job) return 1;
    InterlockedExchangePointer((PVOID volatile*)&job->notify, NULL);
    return os_cmd_finish(job, ms);
}
