/* cmd_async.h — 在工作线程执行一条耗时驱动命令（第四轮 任务1：StepOut）
 *
 * 与 conn_async 同一模式：
 *  - os_cmd_start 拷贝参数、用 GetModuleHandleExW(FROM_ADDRESS) 钉住驱动 DLL，起线程执行
 *    drv->command(ctx, cmd, NULL, NULL)，完成后 PostMessage(notify, msg, 0, job)，最后放开 DLL。
 *  - 取消不在这里：调用方给驱动发 HALT / DISCONNECT（驱动的轮询在 ≤10ms 内返回 OS_ERR_CANCELED）。
 *  - 只在主线程调用 start / finish / abandon。
 */
#ifndef OS_CMD_ASYNC_H
#define OS_CMD_ASYNC_H

#include <windows.h>
#include "module_api.h"

typedef struct OS_CmdJob {
    const OS_Module* drv;
    void* ctx;
    int cmd;
    int driver_index;
    HWND notify;           /* 原子读写（abandon 置 NULL） */
    UINT msg;
    int rc;                /* 工作线程写；收到完成消息后主线程读 */
    volatile LONG finished;
    HANDLE thread;
    HMODULE pin;
} OS_CmdJob;

OS_CmdJob* os_cmd_start(const OS_Module* drv, void* ctx, int cmd, int driver_index, HWND notify, UINT msg);
/* 等线程结束（ms）：结束返回 1 并释放 job；超时返回 0（job 仍有效） */
int  os_cmd_finish(OS_CmdJob* job, DWORD ms);
/* 退出用：不再投递 + 等 ms；仍未结束则有意泄漏 job（线程钉着 DLL，结束后自行放开）。返回 1 = 已释放 */
int  os_cmd_abandon(OS_CmdJob* job, DWORD ms);

#endif
