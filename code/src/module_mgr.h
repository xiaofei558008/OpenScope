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

/* 日志来源前缀 "<目标别名> <模块>: 消息"（target_alias.h；docs/multi_target_design.md §10）。
 * 别名按驱动下标：加载后默认 "<驱动>0"；连接成功后按芯片自动推导（tc3x/stm32g4…），用户值优先。 */
int  os_modmgr_driver_alias(int drv_idx, char* out, int cap);           /* 返回 1=用户值 0=自动 -1=无 */
int  os_modmgr_set_driver_alias(int drv_idx, const char* alias, int user); /* user=1 且 alias 空 → 清除用户值 */
void os_modmgr_auto_alias(int drv_idx, const char* device, const char* info_text);  /* 连接成功后 */
void os_modmgr_auto_alias_pre(int drv_idx, const char* device);                     /* 连接前（仅能推导出芯片时） */

#endif
