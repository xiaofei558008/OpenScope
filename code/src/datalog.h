#ifndef OS_DATALOG_H
#define OS_DATALOG_H

#include <windows.h>

/* CSV 记录 */
int os_datalog_start(const wchar_t* path);
void os_datalog_stop(void);
void os_datalog_append(void);

/* 离线回放（实时节奏，--replay 测试钩子路径） */
int os_replay_start(const wchar_t* path);
void os_replay_stop(void);
void os_replay_tick(void);

/* 长时间采集自动落盘：RAM ≤10MB → 时间戳 CSV（采集线程调用） */
int  os_spool_begin(void);
void os_spool_push(OS_Sample* s, int n);
void os_spool_end(void);

/* 回放全量加载：整文件解析 + min/max 桶缓存（波形"全部显示"数据源） */
int  os_replay_load_all(const wchar_t* path);
void os_buckets_clear(void);

#endif
