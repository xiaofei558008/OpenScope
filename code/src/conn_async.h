/* conn_async.h — 后台线程执行驱动 CONNECT/Attach（第三轮 任务4）
 *
 * 目的：探针连接（J-Link/ST-Link/DAS）可能耗时数秒，原先在 UI 线程同步调用会冻结界面。
 * 这里把 CONNECT + IS_CONNECTED + GET_INFO 放到工作线程，完成后 PostMessage(hwnd, msg, 0, job)
 * 回到主线程；主线程在消息处理里调用 os_conn_finish() 回收。
 *
 * 线程/生命周期约定：
 *  - 只在主线程调用 os_conn_start / os_conn_cancel / os_conn_finish / os_conn_abandon。
 *  - 工作线程期间用 GetModuleHandleExW(FROM_ADDRESS) 把驱动 DLL 钉住，线程结束才放开：
 *    退出/切换驱动时即使 FreeLibrary，代码也不会被卸掉。
 *  - 取消：os_conn_cancel 只置标志；调用方随后照常给驱动发 OS_CMD_DISCONNECT（驱动在
 *    厂商调用之间的取消点返回 OS_ERR_CANCELED）。CONNECT 已成功但已取消 → 工作线程补发
 *    DISCONNECT，结果统一为 OS_ERR_CANCELED。
 */
#ifndef OS_CONN_ASYNC_H
#define OS_CONN_ASYNC_H

#include <windows.h>
#include "module_api.h"

typedef struct OS_ConnJob {
    /* 输入（start 时拷贝） */
    const OS_Module* drv;
    void* ctx;
    OS_ConnectCfg cfg;
    int attach;            /* 0 = 连接，1 = Attach */
    int driver_index;      /* 发起时的驱动序号（多探针时用于回填 Project） */
    HWND notify;
    UINT msg;
    /* 输出（工作线程写，收到完成消息后主线程读） */
    int rc;                /* CONNECT 结果；已取消 → OS_ERR_CANCELED */
    int connected;         /* IS_CONNECTED */
    OS_DriverInfo info;    /* GET_INFO（仅 rc==OK） */
    /* 内部 */
    volatile LONG cancelled;
    volatile LONG finished; /* 工作线程已写完结果 */
    HANDLE thread;
    HMODULE pin;
} OS_ConnJob;

/* 启动；失败返回 NULL（此时未调用驱动）。 */
OS_ConnJob* os_conn_start(const OS_Module* drv, void* ctx, const OS_ConnectCfg* cfg,
                          int attach, int driver_index, HWND notify, UINT msg);
/* 置取消标志（不阻塞）。 */
void os_conn_cancel(OS_ConnJob* job);
int  os_conn_is_cancelled(const OS_ConnJob* job);
/* 主线程收到完成消息后先调用：取消标志在工作线程最后一次检查之后才置上（窗口极小）时，
 * 结果仍是 OK —— 这里补发 DISCONNECT 并改为 OS_ERR_CANCELED。返回最终 rc。 */
int  os_conn_settle(OS_ConnJob* job);
/* 等工作线程结束（ms），结束返回 1 并释放 job；超时返回 0，job 保持有效。 */
int  os_conn_finish(OS_ConnJob* job, DWORD ms);
/* 退出时用：取消 + 等 ms；仍未结束则有意泄漏 job（线程仍钉着 DLL，结束时自行放开）。
 * 返回 1 = 已结束并释放。 */
int  os_conn_abandon(OS_ConnJob* job, DWORD ms);

#endif
