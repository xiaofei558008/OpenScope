#ifndef OS_XCPWIN_H
#define OS_XCPWIN_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 打开/激活 XCP 标定调试窗口（手工 CONNECT / 读写标定内存）。 */
void os_xcpwin_show(HWND owner);

/** 隐藏 XCP 窗口（AI 控制界面时使用）。 */
void os_xcpwin_hide(void);

/** AI/主线程：刷新窗口上的连接状态文案（可空操作）。 */
void os_xcpwin_refresh(void);

#ifdef __cplusplus
}
#endif

#endif
