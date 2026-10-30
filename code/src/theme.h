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

    /* UI P0: layout-spec.md colors (Terminator-style dark) */
    TH_GAP,           /* pane gap line (#0b0d10) */
    TH_TITLE,         /* pane title bar (#1b2027) */
    TH_TITLE_FOCUS,   /* focused pane title (#1f3b63) */
    TH_CMDBAR,        /* command bar (#14181d) */
    TH_ACCENT,        /* accent (#2f81f7) */
    TH_NET_BG,        /* network bar bg (#10261f) */
    TH_NET_TEXT,      /* network bar text (#7ee2b8) */
    TH_XCP,           /* XCP text (#a5d6ff) */
    TH_WARN,          /* warning (#e3b341) */
    TH_ERROR,         /* error (#ff7b72) */
    TH_OK,            /* run / connected (#3fb950) */
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
/* 菜单栏/弹出菜单刷主题底色（MIM_BACKGROUND）；主菜单与 CreatePopupMenu 后调用 */
void os_theme_menu(HMENU menu);
/* TrackPopupMenu 前：刷 HMENU + FlushMenuThemes，并短暂钩住 #32768 暗色；返回后调 os_theme_menu_end */
void os_theme_menu_show(HMENU menu);
void os_theme_menu_end(void);
/* 递归给窗口树打系统暗色 + DarkMode_Explorer（浮窗/对话框也调用） */
void os_theme_window_tree(HWND root);
/* 给任意 HWND（含 combo 下拉 list）打 AllowDarkMode + DarkMode_Explorer */
void os_theme_hwnd_dark(HWND hwnd);
/* Bug3: 暗色下去掉 WS_EX_CLIENTEDGE（系统 3D 凹边用浅色 COLOR_3DHILIGHT/BTNFACE 画，面板上沿露白条）；
 * 浅色恢复。记住窗口原本是否带边框，可重复调用。 */
void os_theme_client_edge(HWND hwnd);

#endif
