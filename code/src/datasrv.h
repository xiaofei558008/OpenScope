#ifndef OS_DATASRV_H
#define OS_DATASRV_H

#include <windows.h>

/* 开始/停止采集轮询线程 */
int os_ds_start(void);
void os_ds_stop(void);
/* 上次停止超时的采集线程是否仍在运行（仍卡在驱动里） */
int  os_ds_poll_busy(void);
/* 任一 Workspace Project / XCP / 全局探针已连接即可采集 */
int os_ds_any_connected(void);

/* 推送一批样本（轮询线程/回放线程调用）：入环形缓冲、更新最新值、写日志、通知 UI */
void os_ds_push_batch(OS_Sample* samples, int n);

/* UI 线程收到 WM_OS_SAMPLES 后调用：取出样本分发给窗口与模块 */
void os_ds_drain(void);

/* 写入叶子变量（按文本解析），成功后回读并产生 written 样本；返回 0 成功 */
int os_ds_write_leaf(int id, const char* text, char* err, int errlen);

#endif
