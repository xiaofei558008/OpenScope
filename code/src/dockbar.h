/*
 * 可停靠命令栏：主栏 / 调试栏均可拖出浮动、吸附主窗上下左右。
 */
#ifndef OS_DOCKBAR_H
#define OS_DOCKBAR_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum OS_DockSide {
    OS_DOCK_TOP = 0,
    OS_DOCK_BOTTOM,
    OS_DOCK_LEFT,
    OS_DOCK_RIGHT,
    OS_DOCK_FLOAT
} OS_DockSide;

#define OS_DOCKBAR_MAIN  0
#define OS_DOCKBAR_DEBUG 1
#define OS_DOCKBAR_COUNT 2

#define OS_DOCKBAR_GRIP  14   /* 抓取条像素 */
#define OS_DOCKBAR_THICK 34   /* 水平停靠高度 / 垂直停靠宽度 */

typedef struct OS_DockBar {
    int         id;
    HWND        hwnd;       /* 栏本体（按钮父窗） */
    HWND        hFloat;     /* 浮动宿主；dock!=FLOAT 时为 NULL */
    OS_DockSide dock;
    OS_DockSide last_dock;  /* 浮动前停靠边 */
    int         float_x, float_y, float_w, float_h;
    wchar_t     title[64];
} OS_DockBar;

void os_dockbar_register(HINSTANCE inst);
/* 创建两栏（子窗挂在 main 上）；返回 0 成功 */
int  os_dockbar_create(HWND main, HINSTANCE inst);
void os_dockbar_destroy(void);
OS_DockBar* os_dockbar_get(int id);
HWND        os_dockbar_hwnd(int id);

/* 按停靠状态缩小 content（客户区坐标）；并 MoveWindow 各栏 */
void os_dockbar_apply_layout(HWND main, RECT* content);

/* 持久化 %LOCALAPPDATA%\OpenScope\dockbars.ini */
void os_dockbar_load(void);
void os_dockbar_save(void);

/* 强制某栏停靠到 side（FLOAT 时用当前浮窗位置） */
void os_dockbar_set_dock(int id, OS_DockSide side);

#ifdef __cplusplus
}
#endif

#endif
