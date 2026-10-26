/* target_alias.h — 日志来源前缀与目标别名（docs/multi_target_design.md §2 Target.id、§10）
 *
 * 实时日志统一为 "<目标别名> <模块>: <消息>"，如 "tc3x das: 已连接 ..."。
 *  - 目标别名 = 设备/目标的短名，字符集 [a-z0-9_]，最长 OS_ALIAS_MAX-1。
 *    自动默认：由芯片推导（TC375 → tc3x，STM32G474RE → stm32g4），推不出则用 "<驱动>0"（jlink0）。
 *    用户可改（将来的设备管理器 / RPC targets.alias），用户值优先于自动值。
 *  - 多目标（M2/M3）后 alias 即 Target.id，日志改由 v6 fw->log_inst 按实例输出，格式不变。
 * 纯函数、无全局状态，可单独编译测试（tests/target_alias/）。 */
#ifndef OS_TARGET_ALIAS_H
#define OS_TARGET_ALIAS_H

#define OS_ALIAS_MAX 24

#ifdef __cplusplus
extern "C" {
#endif

/* 把任意文本规范为别名字符集（小写、非 [a-z0-9_] 去掉），返回长度；结果可能为空 */
int  os_alias_sanitize(const char* in, char* out, int cap);
/* 由芯片/器件描述推导别名；chip_text 可为 cfg.device + " " + info.emulator 拼接；
 * 推不出时用 "<drv>0"。drv 为空时用 "dev0"。返回 1 = 由芯片推导，0 = 用了驱动默认 */
int  os_alias_from_chip(const char* chip_text, const char* drv, char* out, int cap);
/* 驱动默认别名 "<drv>0" */
void os_alias_default(const char* drv, char* out, int cap);
/* 去掉模块历史自带的来源前缀："<mod>:" "<mod>：" "<mod> 模块:" "<mod> 模块：" "<mod>模块:"（及后随空格）。
 * 返回剩余消息起点（未匹配则返回 msg） */
const char* os_log_strip_src(const char* msg, const char* mod);
/* 组装一行："<alias> <mod>: <msg>"；alias 为空 → "<mod>: <msg>"；mod 也空 → "<msg>" */
void os_log_format_src(const char* alias, const char* mod, const char* msg, char* out, int cap);

#ifdef __cplusplus
}
#endif
#endif
