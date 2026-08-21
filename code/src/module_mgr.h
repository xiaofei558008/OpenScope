#ifndef OS_MODULE_MGR_H
#define OS_MODULE_MGR_H

#include <windows.h>

/* 扫描 dll 目录并加载所有模块；返回加载成功数量 */
int os_modmgr_load(void);
void os_modmgr_shutdown(void);

/* AD-13：多驱动选择 */
int os_modmgr_driver_count(void);
const char* os_modmgr_driver_name(int idx);
int os_modmgr_driver_index(void);   /* 当前 g_app.driver 在 drivers[] 中的下标，-1=无 */
int os_modmgr_select_driver(int idx); /* 断开旧连接并切换当前驱动；返回 OS_ERR_OK/OS_ERR_INVALID_ARG */

#endif
