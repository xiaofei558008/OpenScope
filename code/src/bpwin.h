/*
 * 硬件断点面板：设置 / 清除 / 列表（经驱动 OS_CMD_BP_*）。
 */
#ifndef OS_BPWIN_H
#define OS_BPWIN_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 模态断点对话框（需已连接驱动） */
void os_bpwin_dialog(HWND owner);

#ifdef __cplusplus
}
#endif

#endif
