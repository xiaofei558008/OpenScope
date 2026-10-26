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

/* ---- 第四轮 任务2：UI 与 AI 客户端的连接互斥 ----
 * 进程内同一时刻只允许一个 CONNECT/Attach 在途（不论来自 UI 还是 AI 线程）。
 *  - os_conn_claim(owner)：拿到返回 1；已被占用返回 0（调用方回 AGAIN / 忽略点击，不等待）。
 *  - os_conn_set_active(job)：登记在途 job，供另一方取消（UI 断开 → 取消 AI 连接，AI disconnect → 取消 UI 连接）。
 *  - os_conn_cancel_active()：取消在途 job（并让 AI 的 AGAIN 重试循环停止）。调用方随后照常发 DISCONNECT。
 * 所有函数可在任意线程调用。 */
#define OS_CONN_OWNER_NONE 0
#define OS_CONN_OWNER_UI   1
#define OS_CONN_OWNER_AI   2
int  os_conn_claim(int owner);
void os_conn_release(int owner);
int  os_conn_owner(void);
void os_conn_set_active(OS_ConnJob* job);
int  os_conn_cancel_active(void);   /* 返回 1 = 确有在途连接被取消 */

typedef struct OS_ConnResult {
    int rc;
    int connected;
    OS_DriverInfo info;
    int busy_owner;        /* rc==OS_ERR_AGAIN 且因互斥被拒时 = 占用方，否则 0 */
} OS_ConnResult;

/* AI 线程用的阻塞连接：claim → 工作线程 CONNECT（钉住 DLL、可被取消）→ 等结果 → release。
 * 驱动后台重连（AGAIN）时每 250ms 重试，最多 again_wait_ms；期间被取消立即返回 OS_ERR_CANCELED。
 * 不向任何窗口发消息（不会与 UI 线程 SendMessage 互等）。返回 res->rc。 */
int  os_conn_connect_sync(int owner, const OS_Module* drv, void* ctx, const OS_ConnectCfg* cfg,
                          int attach, int driver_index, DWORD again_wait_ms, OS_ConnResult* res);

#endif
