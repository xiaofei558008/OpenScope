/*
 * dock —— Terminator 式平铺停靠管理器（Win32 外壳；纯逻辑在 dock_tree.c）
 *
 * 用法（宿主无关，任何页面都可复用）：
 *   dock_init(parent, &cb)              创建停靠宿主窗口（parent 的子窗口）
 *   dock_move_host(x,y,w,h)             放置宿主
 *   dock_sync(hwnds, titles, n)         告知当前全部面板（新增的自动放置，消失的移除）
 *   dock_activate(hwnd)                 切到该面板（所在窗格的 tab 激活/浮动窗前置）
 *   dock_save(emit,u) / dock_load_*     持久化（layout.ini [dock] 节）
 *   dock_reset()                        重置布局（全部回主宿主自动平铺）
 *   dock_split_active(vertical)         Ctrl+Shift+O/E：把当前窗格的当前 tab 拆到新分割
 * 鼠标：拖窗格标题栏上的 tab → 落点高亮（左/右/上/下/中=tab）→ 松开即拆分/合并；
 *       拖到宿主外 → 浮动窗口；拖浮动窗口（标题栏或 tab）回宿主 → 重新停靠；
 *       拖分隔条调大小（最小尺寸约束，拖动中节流重排），双击分隔条均分。
 */
#ifndef OS_DOCK_H
#define OS_DOCK_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DockCallbacks {
    void (*close_panel)(HWND panel);              /* 窗格 × / 浮动窗关闭 → 宿主负责销毁并再 dock_sync */
    void (*panel_activated)(HWND panel);          /* 可选：用户点选某面板 */
    void (*layout_changed)(void);                 /* 可选：布局变化（可用于自动保存） */
} DockCallbacks;

HWND dock_init(HWND parent, const DockCallbacks* cb);
HWND dock_host(void);
void dock_move_host(int x, int y, int w, int h);
void dock_sync(const HWND* panels, const wchar_t* const* titles, int n);
void dock_set_title(HWND panel, const wchar_t* title);
void dock_activate(HWND panel);
HWND dock_active_panel(void);
void dock_set_excluded(HWND panel);               /* 全屏等临时脱离停靠的窗口（NULL 取消） */
void dock_relayout(void);
void dock_dpi_changed(void);                       /* UI P0: per-monitor DPI change */
void dock_reset(void);
int  dock_split_active(int vertical);
int  dock_is_dragging(void);                       /* 分隔条/标题拖动中（供波形节流重绘） */
void dock_apply_theme(void);

/* 持久化：emit(key, value) 逐行写出；panel_index(hwnd) → 写入的窗口序号（<0 跳过） */
void dock_save(void (*emit)(const char* key, const char* val, void* u),
               int (*panel_index)(HWND, void*), void* u);
void dock_load_begin(void);
void dock_load_line(const char* key, const char* val);
/* 窗口已按序号创建后调用：by_index[i] = 第 i 个保存窗口的 HWND（可为 NULL） */
int  dock_load_apply(const HWND* by_index, int n);
/* 固定面板（变量树/源码文件/消息等系统面板）：持久化编号 1000+k */
#define DOCK_FIXED_BASE 1000
void dock_set_fixed(const HWND* fixed, int n);
/* 直接应用一棵树串（编号 = by_index 下标 / 1000+k 固定面板）；未引用面板自动放置 */
int  dock_apply_tree(const char* main_tree, const HWND* by_index, int n);

/* 测试/自动化：JSON 描述（树串、面板、各 tab/分隔条的屏幕坐标） */
int  dock_describe_json(char* out, int cap);
/* 测试钩子：程序化移动（idx 为 dock 面板编号，zone 见 dock_tree.h；target<0 = 浮动） */
int  dock_move_panel(HWND panel, HWND target_panel, int zone);

#ifdef __cplusplus
}
#endif
#endif
