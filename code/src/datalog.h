#ifndef OS_DATALOG_H
#define OS_DATALOG_H

#include <windows.h>

/* 兼容旧 CSV 直写接口（录制主路径已改用 spool 临时缓存） */
int os_datalog_start(const wchar_t* path);
void os_datalog_stop(void);
void os_datalog_append(void);

/* 离线回放（实时节奏，--replay 测试钩子路径） */
int os_replay_start(const wchar_t* path);
void os_replay_stop(void);
void os_replay_tick(void);

/* 录制临时缓存：RAM ≤10MB → %LOCALAPPDATA%\OpenScope\cache\recording.csv
 * 仅在 recording=1 时由采集写入；重新开始采集前必须 clear。 */
void os_spool_clear(void);                 /* 丢弃上次临时缓存（文件+内存） */
int  os_spool_begin(void);                 /* 清空后建立新缓存（采集线程侧 / 开录时） */
void os_spool_push(OS_Sample* s, int n);   /* 采集线程写入 */
void os_spool_end(void);                   /* 收尾落盘，保留临时文件供保存 */
int  os_spool_has_data(void);              /* 临时文件里是否有样本 */
int  os_spool_save_as(const wchar_t* path);/* 把临时文件复制到用户路径 */

/* 回放全量加载：整文件解析 + min/max 桶缓存（波形"全部显示"数据源） */
int  os_replay_load_all(const wchar_t* path);
void os_buckets_clear(void);

#endif
