/* ai_step.h — AI 的 step_out 异步化（第五轮 任务4）
 *
 * 旧实现：ai_bridge 在 AI 客户端线程里同步调 drv->command(STEP_OUT)，驱动轮询最长 2s，
 * 期间该线程不可中断（aiapi 关停只给 2s 期限，会被拖到边缘）。
 * 现在：STEP_OUT 在 cmd_async 工作线程里执行（同 UI 的 StepOut，钉住驱动 DLL），AI 线程：
 *  - 兼容模式（step_out {}）：分片等待完成（每 5ms 检查一次），完成后照旧回 {"ok":true} / 失败；
 *    关停（os_ai_step_abort）可随时打断等待；等满 wait_ms 仍未完成则回 pending（极少见：驱动自身上限 2s）。
 *  - 异步模式（step_out {"async":1}）：立即回 {"ok":true,"pending":1}，之后用 step_out.status 轮询。
 * 同一时刻只允许一个 AI StepOut（多个 AI 客户端之间互斥）；UI 的 StepOut 与之由驱动的 g_so_busy 互斥。
 * 取消：任何一方发 HALT / DISCONNECT（驱动 ≤10ms 内退出轮询，结果为 CANCELED）。
 * 所有函数可在任意线程调用。 */
#ifndef OS_AI_STEP_H
#define OS_AI_STEP_H

#include <windows.h>
#include "module_api.h"

/* 启动并（可选）等待。返回：1 = 已完成（*out_rc 为驱动结果）；0 = 仍在进行（pending）；
 * OS_ERR_AGAIN = 已有 AI StepOut 在进行；OS_ERR_CANCELED = 正在关停；OS_ERR_FAIL = 启动失败 */
int  os_ai_step_out(const OS_Module* drv, void* ctx, int driver_index, DWORD wait_ms, int* out_rc);
/* 查询：1 = 最近一次已完成（*out_rc）；0 = 进行中；-1 = 从未启动 */
int  os_ai_step_status(int* out_rc);
int  os_ai_step_busy(void);
/* 退出：打断所有等待，HALT 在途的 StepOut 并最多等 ms；之后拒绝新的 step_out。返回 1 = 已结束 */
int  os_ai_step_abort(DWORD ms);

#endif
