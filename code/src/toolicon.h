#ifndef OS_TOOLICON_H
#define OS_TOOLICON_H

#include <windows.h>

/* 调试命令栏统一线框图标（风格对齐 iconfont 线性图标）。
 * 按钮用 BS_OWNERDRAW，由 os_toolicon_draw 绘制。 */

typedef enum {
    TI_RUN = 0,
    TI_STOP,
    TI_RESET_HALT,
    TI_STEP,
    TI_STEP_IN,
    TI_STEP_OUT,
    TI_STEP_OVER,
    TI_BP,
    TI_COUNT
} OS_ToolIcon;

/* 将控件 ID 映射到图标；非调试按钮返回 -1 */
int  os_toolicon_from_ctrl(int ctrl_id);
/* 绘制 DRAWITEM；已处理返回 1 */
int  os_toolicon_draw(const DRAWITEMSTRUCT* dis);
/* 创建带图标的调试按钮（owner-draw），tooltip 用 tip */
HWND os_toolicon_create_btn(HWND parent, int ctrl_id, int x, int y, int w, int h,
                            const wchar_t* tip);

#endif
