#ifndef OS_THEME_H
#define OS_THEME_H

#include <windows.h>

/* F20: 界面主题（白色默认 / 黑色）。所有组件取色统一走 os_theme()，
 * 画刷用 os_theme_brush()（返回缓存的实心画刷，调用方不得 DeleteObject）。 */

typedef enum {
    TH_BG = 0,        /* 主窗口/工具栏/对话框背景 */
    TH_PANEL,         /* 右侧面板/分组/标题栏背景 */
    TH_TEXT,          /* 常规文本 */
    TH_DIMTEXT,       /* 次要标注 */
    TH_BORDER,        /* 边框/分隔条 */
    TH_EDIT_BG,       /* 输入框背景 */
    TH_EDIT_TEXT,     /* 输入框文本 */
    TH_TREE_BG,       /* 变量树背景 */
    TH_TREE_TEXT,
    TH_TREE_SEL_BG,   /* 变量树选中 */
    TH_TREE_SEL_TEXT,
    TH_TREE_LINE,     /* 变量树连线 */
    TH_LOG_BG,        /* 消息列表背景 */
    TH_LOG_TEXT,
    TH_LOG_GRID,      /* 消息列表网格线 */
    TH_TAB_BG,        /* tab 控件背景 */
    TH_TAB_TEXT,
    TH_STATUS_BG,     /* 状态栏 */
    TH_STATUS_TEXT,
    TH_CHART_PLOT_BG, /* 波形绘图区背景 */
    TH_CHART_GRID,    /* 波形网格 */
    TH_CHART_AXIS,    /* 波形坐标标注 */
    TH_COUNT
} OS_ThemeColor;

int  os_theme_dark(void);
void os_theme_set_dark(int on);
void os_theme_set_main(HWND h); /* 主窗口创建时注册（用于 DWM 暗色标题栏） */
COLORREF os_theme(OS_ThemeColor id);
/* 返回缓存的实心画刷（随主题切换重建），调用方不得删除 */
HBRUSH os_theme_brush(OS_ThemeColor id);
/* 把主题应用到主窗口及其所有子窗口/已建窗口（树/日志/状态栏/数字窗口列表等） */
void os_theme_apply(HWND hMain);
/* 从持久化位置读回主题（读 layout.ini 的 theme 键） */
void os_theme_load(void);
/* 把 ListView 的列头（SysHeader32）子类化自绘为主题色（可重复调用，按句柄去重） */
void os_theme_listview_header(HWND list);

#endif
