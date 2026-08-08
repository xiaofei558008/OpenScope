#ifndef OS_DATALOG_H
#define OS_DATALOG_H

#include <windows.h>

/* CSV 记录 */
int os_datalog_start(const wchar_t* path);
void os_datalog_stop(void);
void os_datalog_append(void);

/* 离线回放 */
int os_replay_start(const wchar_t* path);
void os_replay_stop(void);
void os_replay_tick(void);

#endif
