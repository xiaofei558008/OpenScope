#ifndef OS_HELPWIN_H
#define OS_HELPWIN_H

#include <windows.h>

/* 需求12：帮助文档窗口。readme.md 内嵌为 exe 资源（IDR_HELP_MD），
 * F1 / 帮助菜单"帮助文档"弹出只读说明窗口（安装版无需附带 readme.md）。 */
void os_help_register(void);
void os_help_show(HWND parent); /* 已打开则前置；随主题着色 */

#endif
