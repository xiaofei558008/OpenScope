#ifndef OS_MODULE_MGR_H
#define OS_MODULE_MGR_H

#include <windows.h>

/* 扫描 dll 目录并加载所有模块；返回加载成功数量 */
int os_modmgr_load(void);
void os_modmgr_shutdown(void);

#endif
