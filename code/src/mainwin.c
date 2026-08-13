#include "app.h"
#include "mainwin.h"
#include "chartwin.h"
#include "numwin.h"
#include "vartree.h"
#include "datasrv.h"
#include "datalog.h"
#include "module_mgr.h"
#include "layout.h"
#include "theme.h"
#include "version.h"
#include "helpwin.h"
#include "tilecalc.h"
#include <commctrl.h>
#include <commdlg.h>
#include <string.h>

#define IDC_BTN_OPEN     2001
#define IDC_BTN_CONNECT  2002
#define IDC_BTN_DISCON   2003
#define IDC_BTN_START    2004
#define IDC_BTN_STOP     2005
#define IDC_BTN_LOGSTART 2006
#define IDC_BTN_LOGSTOP  2007
#define IDC_BTN_REPLAY   2008
#define IDC_BTN_REPLAYSTOP 2009
#define IDC_BTN_ABOUT    2010
#define IDM_EXIT         2011
#define IDM_LAYOUT_SAVE  2021
#define IDM_LAYOUT_LOAD  2022
#define IDM_WIN_CHART    2012
#define IDM_WIN_NUM      2013
#define IDM_WIN_MODULE_BASE 2200

#define IDM_TREE_ALL     2301
#define IDM_TREE_NONE    2302
#define IDM_TREE_WRITE   2303
#define IDM_TREE_RELOAD  2304
#define IDM_TREE_ADD_CHART 2305
#define IDM_TREE_ADD_NUM   2306
#define IDM_TAB_CLOSE      2501
#define IDM_TAB_RENAME     2502
#define IDM_TAB_ADD_CHART  2503 /* N11: 在当前 tab 添加波形窗口 */
#define IDM_TAB_ADD_NUM    2504 /* N11: 在当前 tab 添加数值窗口 */
#define IDM_TAB_MAXIMIZE   2505 /* N11: 最大化/还原当前窗口 */
#define IDM_TAB_FULLSCREEN 2506 /* Bug3: 全屏/退出全屏 */
#define IDM_TAB_MINIMIZE   2507 /* Bug6: 最小化/还原当前窗口（缩进 tab 底部最小化条） */

#define IDM_LOG_COPY    2601 /* Bug13: 复制选中消息到剪贴板 */
#define IDM_LOG_CLEAR   2602 /* Bug13: 全部清除 */

#define IDM_THEME_DARK  2701 /* F20: 深色模式开关（菜单勾选项） */

/* 连接配置控件（直接放主界面工具栏，不弹对话框） */
#define IDC_CFG_DEVICE   2101
#define IDC_CFG_IFACE    2102
#define IDC_CFG_SPEED    2103
#define IDC_CFG_EMU      2104
#define IDC_CFG_REFRESH  2105

/* 芯片核心预置列表（J-Link "Device=" 名称）。
 * F17：纯 SWD/JTAG 内存读写/读 Flash 地址内容只需通用核心名（CoreSight AHB-AP 架构一致），
 * 不需要具体芯片型号；具体型号仅在烧录 Flash 算法时需要。实测错误的具体型号会让 J-Link 连接挂起。 */
static const wchar_t* g_devices[] = {
    L"Cortex-M4", L"Cortex-M3", L"Cortex-M0+", L"Cortex-M0",
    L"Cortex-M7", L"Cortex-M23", L"Cortex-M33", L"Cortex-A5",
    L"Cortex-R4", L"Cortex-A7"
};

static int g_emu_count = -1; /* 最近一次 J-Link 扫描到的设备数（-1=未扫描） */

#define IDD_PICK_OK      2401
#define IDD_PICK_CANCEL  2402
#define IDD_PICK_EDIT    2403
#define IDD_PICK_LIST    2404
#define IDM_PICK_ADD_CHART 2405 /* 搜索/选变量对话框列表右键：添加到波形窗口 */
#define IDM_PICK_ADD_NUM   2406 /* 搜索/选变量对话框列表右键：添加到数值窗口 */
#define IDD_EDIT_OK      2411
#define IDD_EDIT_CANCEL  2412
#define IDD_EDIT_TEXT    2413
#define IDD_EDIT_VALUE   2414

typedef struct PickState {
    int ids[300];
    int count;
    struct PickResult* pr; /* N13a: 多选结果输出（lpCreateParams 传入） */
} PickState;

/* N13a: 变量多选结果（模糊搜索对话框确定后回填） */
typedef struct PickResult {
    int count;
    int ids[512];
} PickResult;

typedef struct EditState {
    int* result;
    wchar_t* text_out;
} EditState;

typedef struct ModWinMenuItem {
    OS_Module* mod;
    void* ctx;
    const OS_WindowType* wt;
} ModWinMenuItem;

/* F20: WM_CTLCOLOR* 统一返回主题画刷（定义在 os_mainwin_proc 前，供对话框先使用） */
static LRESULT theme_ctlcolor(HDC hdc, int edit);
/* F20: 标准控件 SetWindowTheme 暗色化（定义在 os_mainwin_apply_theme 前，供对话框先使用） */
static void ctrl_dark_theme(HWND h);

static const wchar_t* g_main_class = L"OpenScopeMain";
static const wchar_t* g_right_class = L"OSRightPanel";
static HMENU g_menu;
static HMENU g_menu_file; /* F20: 深色模式菜单项所在菜单（勾选状态同步） */
static HMENU g_menu_win;
static ModWinMenuItem g_modwin_menu[64];
static int g_modwin_menu_count;

/* 工具栏按钮（菜单栏正下方一行） */
static const struct { int id; const wchar_t* text; } g_tool_btns[] = {
    { IDC_BTN_OPEN, L"打开ELF" }, { IDC_BTN_CONNECT, L"连接" },
    { IDC_BTN_DISCON, L"断开" }, { IDC_BTN_START, L"开始采集" },
    { IDC_BTN_STOP, L"停止采集" }, { IDC_BTN_LOGSTART, L"记录" },
    { IDC_BTN_LOGSTOP, L"停止记录" }, { IDC_BTN_REPLAY, L"离线回放" },
    { IDC_BTN_REPLAYSTOP, L"停止回放" },
    /* Bug8: 自动隐藏/钉住归位到左侧 elf 变量树（OSTreePin 钉图标），不再放菜单栏下固定按键 */
};

/* ---------- 工具 ---------- */

#define LOG_STRIP_H 10 /* F22: 消息栏收起后底部细条高度 */

static void set_status(int part, const wchar_t* text)
{
    if (g_app.hStatus) SendMessageW(g_app.hStatus, SB_SETTEXTW, part, (LPARAM)text);
}

static void set_status_utf8(int part, const char* text)
{
    wchar_t w[512];
    os_utf8_to_wide_buf(text, w, 512);
    set_status(part, w);
}

/* Bug7: 日志跨线程安全。os_log 可能被采集线程/模块线程调用，而 ListView 属于主线程。
 * 非主线程调用时经 PostMessage(WM_OS_LOG) 交给主线程插入，避免跨线程 SendMessage 与
 * 模态对话框（如 GetSaveFileNameW）互相等待导致界面卡死。 */
typedef struct OS_LogMsg { int level; wchar_t text[1024]; } OS_LogMsg;

static void log_insert_now(int level, const wchar_t* line)
{
    LVITEMW item;
    wchar_t t[64], lvl[16], msg[1024];
    int64_t us = os_time_us();
    int minute = (int)(us / 1000000 / 60);
    int sec = (int)(us / 1000000 % 60);
    int msec = (int)(us % 1000000 / 1000);
    if (!g_app.hLog || !IsWindow(g_app.hLog)) return;
    _snwprintf(t, 64, L"%02d:%02d.%03d", minute % 60, sec, msec);
    switch (level) {
    case OS_LOG_WARN: _snwprintf(lvl, 16, L"警告"); break;
    case OS_LOG_ERROR: _snwprintf(lvl, 16, L"错误"); break;
    case OS_LOG_DEBUG: _snwprintf(lvl, 16, L"调试"); break;
    default: _snwprintf(lvl, 16, L"信息"); break;
    }
    _snwprintf(msg, 1024, L"%s", line ? line : L"");
    memset(&item, 0, sizeof(item));
    item.mask = LVIF_TEXT;
    item.iItem = 0;
    item.iSubItem = 0;
    item.pszText = t;
    item.iItem = ListView_InsertItem(g_app.hLog, &item);
    ListView_SetItemText(g_app.hLog, item.iItem, 1, lvl);
    ListView_SetItemText(g_app.hLog, item.iItem, 2, msg);
    if (ListView_GetItemCount(g_app.hLog) > 5000)
        ListView_DeleteItem(g_app.hLog, ListView_GetItemCount(g_app.hLog) - 1);
}

/* Bug13: 复制选中消息到剪贴板（时间 + 级别 + 消息，每行一条） */
static void log_copy_selected(void)
{
    static wchar_t buf[4096];
    int count = 0, i, pos = 0;
    int n = g_app.hLog ? ListView_GetItemCount(g_app.hLog) : 0;
    if (n <= 0) return;
    buf[0] = 0;
    for (i = 0; i < n; i++) {
        wchar_t t[64], lvl[16], msg[1024];
        if (!(ListView_GetItemState(g_app.hLog, i, LVIS_SELECTED) & LVIS_SELECTED))
            continue;
        memset(t, 0, sizeof(t)); memset(lvl, 0, sizeof(lvl)); memset(msg, 0, sizeof(msg));
        ListView_GetItemText(g_app.hLog, i, 0, t, 64);
        ListView_GetItemText(g_app.hLog, i, 1, lvl, 16);
        ListView_GetItemText(g_app.hLog, i, 2, msg, 1024);
        pos += _snwprintf(buf + pos, (sizeof(buf) / sizeof(buf[0])) - pos,
                          L"%s %s %s\r\n", t, lvl, msg);
        if (pos >= (int)(sizeof(buf) / sizeof(buf[0])) - 16) break;
        count++;
    }
    if (count <= 0) return;
    if (OpenClipboard(g_app.hMain)) {
        size_t nch = wcslen(buf) + 1;
        HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, nch * sizeof(wchar_t));
        if (hg) {
            void* dst = GlobalLock(hg);
            if (dst) {
                memcpy(dst, buf, nch * sizeof(wchar_t));
                GlobalUnlock(hg);
                EmptyClipboard();
                SetClipboardData(CF_UNICODETEXT, hg);
            } else {
                GlobalFree(hg);
            }
        }
        CloseClipboard();
    }
    os_log(OS_LOG_INFO, "已复制 %d 条消息", count);
}

/* Bug13: 全部清除消息 */
static void log_clear_all(void)
{
    if (g_app.hLog && IsWindow(g_app.hLog))
        ListView_DeleteAllItems(g_app.hLog);
    os_log(OS_LOG_INFO, "已清除全部消息");
}

/* Bug13: 消息区域右键菜单：复制选中 / 全部清除 */
static void log_context_menu(void)
{
    HMENU m = CreatePopupMenu();
    POINT pt;
    int has_sel = 0, i;
    int n = g_app.hLog ? ListView_GetItemCount(g_app.hLog) : 0;
    for (i = 0; i < n && !has_sel; i++)
        if (ListView_GetItemState(g_app.hLog, i, LVIS_SELECTED) & LVIS_SELECTED) has_sel = 1;
    AppendMenuW(m, MF_STRING | (has_sel ? 0 : MF_GRAYED), IDM_LOG_COPY, L"复制选中");
    AppendMenuW(m, MF_STRING | (n > 0 ? 0 : MF_GRAYED), IDM_LOG_CLEAR, L"全部清除");
    GetCursorPos(&pt);
    TrackPopupMenu(m, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_app.hMain, NULL);
    DestroyMenu(m);
}

void os_mainwin_append_log(int level, const wchar_t* line)
{
    DWORD tid;
    if (!g_app.hMain || !IsWindow(g_app.hMain)) return;
    tid = GetWindowThreadProcessId(g_app.hMain, NULL);
    if (GetCurrentThreadId() != tid) {
        OS_LogMsg* m = (OS_LogMsg*)malloc(sizeof(OS_LogMsg));
        if (!m) return;
        m->level = level;
        _snwprintf(m->text, 1024, L"%s", line ? line : L"");
        if (!PostMessageW(g_app.hMain, WM_OS_LOG, (WPARAM)m, 0)) { free(m); return; }
        return;
    }
    log_insert_now(level, line);
}

void os_fw_log(int level, const char* fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    /* 模块日志统一走 os_log：文件 + UI 双通道 */
    os_log(level, "%s", buf);
}

void os_fw_post(UINT msg, WPARAM w, LPARAM l)
{
    if (g_app.hMain) PostMessage(g_app.hMain, msg, w, l);
}

void os_mainwin_update_buttons(void)
{
    int hasDriver = g_app.driver != NULL;
    int conn = g_app.connected ? 1 : 0;
    int running = g_app.acq_state == OS_ACQ_RUNNING;
    int replay = g_app.acq_state == OS_ACQ_REPLAY;
    HWND b;
#define ENB(id, on) do { b = GetDlgItem(g_app.hMain, (id)); if (b) EnableWindow(b, (on)); } while (0)
    ENB(IDC_BTN_CONNECT, hasDriver && !conn && !replay);
    ENB(IDC_BTN_DISCON, conn);
    ENB(IDC_BTN_START, conn && g_app.watch_count > 0 && !running && !replay);
    ENB(IDC_BTN_STOP, running);
    ENB(IDC_BTN_LOGSTART, conn && !g_app.log_csv && !replay);
    ENB(IDC_BTN_LOGSTOP, g_app.log_csv != NULL);
    ENB(IDC_BTN_REPLAY, !running && !replay);
    ENB(IDC_BTN_REPLAYSTOP, replay);
    ENB(IDC_BTN_OPEN, 1);
#undef ENB
}

static void refresh_status(void)
{
    wchar_t w[512];
    if (g_app.connected && g_app.driver) {
        wchar_t dn[128];
        os_utf8_to_wide_buf(g_app.driver->name, dn, 128);
        _snwprintf(w, 512, L"已连接 %s", dn);
    } else {
        _snwprintf(w, 512, L"未连接");
    }
    set_status(0, w);
    if (g_app.elf_path[0])
        set_status(1, g_app.elf_path);
    else
        set_status(1, L"未加载 ELF");
    switch (g_app.acq_state) {
    case OS_ACQ_RUNNING: set_status(2, L"采集中"); break;
    case OS_ACQ_REPLAY: set_status(2, L"离线回放中"); break;
    case OS_ACQ_STOPPED: set_status(2, L"已停止"); break;
    default: set_status(2, L"空闲"); break;
    }
    _snwprintf(w, 512, L"观测 %d / 叶子 %d / 样本 %ld",
               g_app.watch_count, g_app.leaf_count, (long)g_app.total_samples);
    set_status(3, w);
}

/* 供窗口模块调用：观测数变化后刷新状态栏/按钮 */
void os_mainwin_refresh_status(void)
{
    refresh_status();
}

/* N9(b): 判断 leaf_id 是否仍被任一窗口（含模块窗口）使用 */
int os_win_leaf_used(int leaf_id)
{
    int i, k, g;
    for (i = 0; i < g_app.win_count; i++) {
        OS_WinItem* wi = &g_app.wins[i];
        char nm[300];
        for (g = 0; g < wi->group_count; g++) {
            HWND w = wi->group[g];
            if (!w || !IsWindow(w)) continue;
            if (wi->is_module) {
                if (wi->mod && wi->mod->api_version >= 3 && wi->mod->win_enum_var) {
                    for (k = 0; ; k++) {
                        if (!wi->mod->win_enum_var(wi->mod_ctx, w, k, nm, sizeof(nm))) break;
                        if (os_vartree_find_by_name(nm) == leaf_id) return 1;
                    }
                }
            } else {
                for (k = 0; ; k++) {
                    if (os_chart_is(w)) {
                        if (!os_chart_var_name(w, k, nm, sizeof(nm))) break;
                    } else if (os_num_is(w)) {
                        if (!os_num_var_name(w, k, nm, sizeof(nm))) break;
                    } else break;
                    if (os_vartree_find_by_name(nm) == leaf_id) return 1;
                }
            }
        }
    }
    return 0;
}

/* 从窗口移除变量后：若不再被任何窗口引用则自动取消观测（联动左侧勾选框） */
void os_win_auto_unwatch(int leaf_id)
{
    if (leaf_id < 0) return;
    if (!os_win_leaf_used(leaf_id)) {
        os_vartree_set_watch(leaf_id, 0);
        os_vartree_set_check_ui(g_app.hTree, leaf_id, 0);
        os_mainwin_refresh_status();
        os_mainwin_update_buttons();
        os_log(OS_LOG_DEBUG, "变量已从所有窗口移除，取消观测: id=%d", leaf_id);
    }
}

/* ---------- 布局（Tab 标签页） ---------- */

static int g_cur_tab = -1; /* 当前激活窗口在 g_app.wins 中的下标 */
static HWND g_cur_win = NULL; /* N11/Bug3: 最近获得焦点的窗口（菜单“当前窗口”指向） */

/* 子窗口获得焦点时登记，供“最大化/全屏当前窗口”菜单使用 */
void os_win_mark_active(HWND w)
{
    int i, k;
    for (i = 0; i < g_app.win_count; i++) {
        for (k = 0; k < g_app.wins[i].group_count; k++) {
            if (g_app.wins[i].group[k] == w) { g_cur_win = w; return; }
        }
    }
}

static void layout(void);
static void add_win_item(HWND hwnd, int is_module, OS_Module* mod, void* ctx, const wchar_t* title);
static void tab_set_title(int idx, const wchar_t* name);

/* Bug6: 平铺列间分隔带（拖拽调列宽）与 tab 底部最小化条（点击还原）。
 * 几何由 layout_tab_pages 填充（tab 客户区坐标），tab 子类命中/绘制共用。 */
#define OS_TILE_GAP 6    /* 平铺列间分隔带宽度 */
#define OS_MINIBAR_H 26  /* 最小化条高度（有最小化窗口时显示） */
static RECT g_tile_gaps[OS_MAX_GROUP];      /* 分隔带矩形 */
static int  g_tile_gap_a[OS_MAX_GROUP];     /* 分隔带左列 group 下标 */
static int  g_tile_gap_b[OS_MAX_GROUP];     /* 分隔带右列 group 下标 */
static int  g_tile_gap_count;
static RECT g_minbar_btns[OS_MAX_GROUP];    /* 最小化条按钮矩形 */
static int  g_minbar_btn_idx[OS_MAX_GROUP]; /* 按钮对应 group 下标 */
static int  g_minbar_count;

/* 增删窗口后比例归一：和>0 等比缩放到 1，否则均分（布局安全网） */
static void tile_normalize(OS_WinItem* wi)
{
    double sum = 0.0;
    int k;
    for (k = 0; k < wi->group_count; k++) sum += wi->col_ratio[k];
    if (sum <= 0.0001) { os_tile_ratios_init(wi->col_ratio, wi->group_count); return; }
    for (k = 0; k < wi->group_count; k++) wi->col_ratio[k] /= sum;
}

/* 最小化条按钮矩形（pr 为完整页区域，条在底部 OS_MINIBAR_H 高） */
static void tile_minbar_geom(const RECT* pr, const OS_WinItem* wi)
{
    int k, x = pr->left + 4;
    g_minbar_count = 0;
    for (k = 0; k < wi->group_count && g_minbar_count < OS_MAX_GROUP; k++) {
        RECT* b;
        if (!wi->group_min[k]) continue;
        b = &g_minbar_btns[g_minbar_count];
        b->left = x;
        b->top = pr->bottom - OS_MINIBAR_H + 3;
        b->right = x + 170;
        b->bottom = pr->bottom - 3;
        g_minbar_btn_idx[g_minbar_count] = k;
        g_minbar_count++;
        x += 176;
    }
}

static void layout_tab_pages(void)
{
    RECT rc, pr, work;
    OS_WinItem* wi;
    int n, k, x;
    int min_count = 0, tiled = 0, last_tiled = -1;
    int usable_w, avail_h;
    double sumr;
    if (!g_app.hTab || !IsWindow(g_app.hTab)) return;
    if (g_cur_tab < 0 || g_cur_tab >= g_app.win_count) return;
    GetClientRect(g_app.hTab, &rc);
    pr = rc;
    SendMessageW(g_app.hTab, TCM_ADJUSTRECT, FALSE, (LPARAM)&pr);
    wi = &g_app.wins[g_cur_tab];
    n = wi->group_count;
    g_tile_gap_count = 0;
    g_minbar_count = 0;
    if (n <= 0) return;
    for (k = 0; k < n; k++) if (wi->group_min[k]) min_count++;
    work = pr;
    if (min_count > 0) work.bottom -= OS_MINIBAR_H;
    tile_minbar_geom(&pr, wi);
    if (wi->group_max >= 0 && wi->group_max < n && !wi->group_min[wi->group_max]) {
        /* N11 最大化：该窗口填满 tab（减去最小化条），其余隐藏 */
        for (k = 0; k < n; k++) {
            HWND gw = wi->group[k];
            if (!gw || !IsWindow(gw)) continue;
            if (gw == g_app.fs_win) continue; /* 全屏窗口不受平铺/最大化影响 */
            if (k == wi->group_max) {
                MoveWindow(gw, work.left, work.top, work.right - work.left,
                           work.bottom - work.top, TRUE);
                ShowWindow(gw, SW_SHOW);
            } else {
                ShowWindow(gw, SW_HIDE);
            }
        }
        return;
    }
    /* Bug6 平铺：非最小化窗口按列宽比例排列（非最小化列间归一），
     * 列间留 OS_TILE_GAP 分隔带供鼠标拖拽调宽；最小化窗口隐藏。 */
    tile_normalize(wi);
    sumr = 0.0;
    for (k = 0; k < n; k++) {
        if (!wi->group_min[k]) { sumr += wi->col_ratio[k]; tiled++; last_tiled = k; }
    }
    if (tiled <= 0 || sumr <= 0.0) {
        /* 全部最小化：只留最小化条 */
        for (k = 0; k < n; k++) {
            HWND gw = wi->group[k];
            if (gw && IsWindow(gw) && gw != g_app.fs_win) ShowWindow(gw, SW_HIDE);
        }
        return;
    }
    usable_w = (work.right - work.left) - OS_TILE_GAP * (tiled - 1);
    avail_h = work.bottom - work.top;
    x = work.left;
    {
        int seen = 0;
        for (k = 0; k < n; k++) {
            HWND gw = wi->group[k];
            int wk;
            if (!gw || !IsWindow(gw)) continue;
            if (gw == g_app.fs_win) continue;
            if (wi->group_min[k]) { ShowWindow(gw, SW_HIDE); continue; }
            seen++;
            if (k == last_tiled)
                wk = work.right - x; /* 最后一列吃剩余宽度（消除舍入误差） */
            else
                wk = (int)(wi->col_ratio[k] / sumr * usable_w + 0.5);
            if (wk < 20) wk = 20;
            MoveWindow(gw, x, work.top, wk, avail_h, TRUE);
            ShowWindow(gw, SW_SHOW);
            /* 分隔带矩形（最后一列之后没有；右列下表循环结束后统一回填） */
            if (k != last_tiled && g_tile_gap_count < OS_MAX_GROUP) {
                RECT* grect = &g_tile_gaps[g_tile_gap_count];
                grect->left = x + wk;
                grect->top = work.top;
                grect->right = x + wk + OS_TILE_GAP;
                grect->bottom = work.bottom;
                g_tile_gap_a[g_tile_gap_count] = k;
                g_tile_gap_b[g_tile_gap_count] = k;
                g_tile_gap_count++;
            }
            x += wk + OS_TILE_GAP;
        }
    }
    /* 修正：分隔带右列下标 = 下一个非最小化列（上面循环按出现顺序已保证 a<b 相邻可见） */
    {
        int g;
        for (g = 0; g < g_tile_gap_count; g++) {
            int a = g_tile_gap_a[g], j;
            for (j = a + 1; j < n; j++) {
                if (!wi->group_min[j]) { g_tile_gap_b[g] = j; break; }
            }
        }
    }
}

static void show_tab(int idx)
{
    int i, k;
    for (i = 0; i < g_app.win_count; i++) {
        OS_WinItem* wi = &g_app.wins[i];
        for (k = 0; k < wi->group_count; k++) {
            HWND w = wi->group[k];
            if (!w || !IsWindow(w) || w == g_app.fs_win) continue;
            /* Bug6: 当前 tab 的最小化窗口保持隐藏（缩进最小化条） */
            if (i == idx && wi->group_min[k] && wi->group_max < 0) continue;
            ShowWindow(w, (i == idx) ? SW_SHOW : SW_HIDE);
        }
    }
    g_cur_tab = idx;
    layout_tab_pages();
}

static void rebuild_tabs(void)
{
    int i, cur = g_cur_tab;
    if (!g_app.hTab || !IsWindow(g_app.hTab)) return;
    SendMessageW(g_app.hTab, TCM_DELETEALLITEMS, 0, 0);
    for (i = 0; i < g_app.win_count; i++) {
        TCITEMW ti;
        memset(&ti, 0, sizeof(ti));
        ti.mask = TCIF_TEXT | TCIF_PARAM;
        ti.pszText = g_app.wins[i].title;
        ti.lParam = i;
        SendMessageW(g_app.hTab, TCM_INSERTITEM, i, (LPARAM)&ti);
    }
    if (cur < 0 || cur >= g_app.win_count) cur = g_app.win_count ? 0 : -1;
    g_cur_tab = cur;
    if (cur >= 0) SendMessageW(g_app.hTab, TCM_SETCURSEL, cur, 0);
    show_tab(cur);
}

void os_mainwin_tile(void)
{
    rebuild_tabs();
}

int os_mainwin_active_tab(void)
{
    return g_cur_tab;
}

void os_mainwin_select_tab(int idx)
{
    if (idx < 0 || idx >= g_app.win_count) return;
    g_cur_tab = idx;
    SendMessageW(g_app.hTab, TCM_SETCURSEL, idx, 0);
    show_tab(idx);
}

void os_mainwin_refresh_layout(void)
{
    layout();
}

HWND os_win_create_by_type(const char* type, const wchar_t* title)
{
    HWND h;
    if (strcmp(type, "chart") == 0) {
        h = os_chart_create(g_app.hTab, 0, 0, 200, 150, title);
        if (h) add_win_item(h, 0, NULL, NULL, title);
        return h;
    }
    if (strcmp(type, "num") == 0) {
        h = os_num_create(g_app.hTab, 0, 0, 200, 150, title);
        if (h) add_win_item(h, 0, NULL, NULL, title);
        return h;
    }
    {
        int i;
        for (i = 0; i < g_modwin_menu_count; i++) {
            ModWinMenuItem* it = &g_modwin_menu[i];
            if (it->mod && it->wt && strcmp(it->wt->type, type) == 0) {
                char title8[256];
                os_wide_to_utf8_buf(title, title8, sizeof(title8));
                h = it->mod->create_window(it->ctx, it->wt->type, g_app.hTab,
                                           0, 0, 200, 150, title8);
                if (h) add_win_item(h, 1, it->mod, it->ctx, title);
                return h;
            }
        }
    }
    return NULL;
}

/* 右侧面板：转发子控件（Tab）通知到主窗口 */
/* F16: tab/右侧空白处右键 -> 新建窗口（新 tab）菜单 */
static void new_win_context_menu(void)
{
    POINT pt;
    HMENU m = CreatePopupMenu();
    int i;
    GetCursorPos(&pt);
    AppendMenuW(m, MF_STRING, IDM_WIN_CHART, L"新建波形窗口");
    AppendMenuW(m, MF_STRING, IDM_WIN_NUM, L"新建数值窗口");
    for (i = 0; i < g_modwin_menu_count; i++) {
        wchar_t wname[128];
        os_utf8_to_wide_buf(g_modwin_menu[i].wt->display_name ? g_modwin_menu[i].wt->display_name
                                                              : g_modwin_menu[i].wt->type, wname, 128);
        AppendMenuW(m, MF_STRING, IDM_WIN_MODULE_BASE + i, wname);
    }
    TrackPopupMenu(m, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_app.hMain, NULL);
    DestroyMenu(m);
}

static LRESULT CALLBACK right_panel_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_NOTIFY) {
        if (g_app.hMain) {
            SendMessageW(g_app.hMain, WM_NOTIFY, wp, lp);
            return 0;
        }
    }
    if (msg == WM_RBUTTONUP) { /* F16: 右侧空白处右键新建窗口 */
        if (g_app.hMain) new_win_context_menu();
        return 0;
    }
    if (msg == WM_ERASEBKGND) { /* F20: 右侧面板背景随主题 */
        RECT rc;
        HDC hdc = (HDC)wp;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_BG));
        return 1;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void layout(void)
{
    RECT rc;
    int bw, bh = 34, sw, logh, right_h, right_w;
    int parts[4];
    GetClientRect(g_app.hMain, &rc);
    bw = rc.right;
    sw = g_app.tree_w;
    logh = g_app.log_h;
    /* F22: 消息栏收起时右侧窗口区占满（无分隔条/消息栏），只留底部细条 */
    if (g_app.log_hidden)
        right_h = rc.bottom - bh - 22 - LOG_STRIP_H;
    else
        right_h = rc.bottom - bh - logh - 22 - 5; /* F14: 5px 横向分隔条（消息栏上下拉伸） */
    right_w = bw - sw - 5;
    /* 工具栏一行（菜单栏正下方）：全部按钮 + 接口/速度/J-Link设备/刷新 */
    {
        static const struct { int id; int fixed_w; int combo; } items[] = {
            { IDC_BTN_OPEN, 0, 0 }, { IDC_BTN_CONNECT, 0, 0 }, { IDC_BTN_DISCON, 0, 0 },
            { IDC_BTN_START, 0, 0 }, { IDC_BTN_STOP, 0, 0 }, { IDC_BTN_LOGSTART, 0, 0 },
            { IDC_BTN_LOGSTOP, 0, 0 }, { IDC_BTN_REPLAY, 0, 0 }, { IDC_BTN_REPLAYSTOP, 0, 0 },
            { IDC_CFG_DEVICE, 140, 1 }, { IDC_CFG_IFACE, 62, 1 }, { IDC_CFG_SPEED, 92, 1 },
            { IDC_CFG_EMU, 240, 1 }, { IDC_CFG_REFRESH, 48, 0 },
        };
        int i, x = 6;
        for (i = 0; i < (int)(sizeof(items) / sizeof(items[0])); i++) {
            HWND c = GetDlgItem(g_app.hMain, items[i].id);
            if (!c) continue;
            if (items[i].combo) {
                MoveWindow(c, x, 5, items[i].fixed_w, 120, TRUE);
                x += items[i].fixed_w + 6;
            } else if (items[i].fixed_w) {
                MoveWindow(c, x, 5, items[i].fixed_w, 24, TRUE);
                x += items[i].fixed_w + 6;
            } else {
                const wchar_t* text = NULL;
                int k;
                SIZE sz;
                HDC hdc = GetDC(c ? c : g_app.hMain);
                SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
                for (k = 0; k < (int)(sizeof(g_tool_btns) / sizeof(g_tool_btns[0])); k++) {
                    if (g_tool_btns[k].id == items[i].id) { text = g_tool_btns[k].text; break; }
                }
                GetTextExtentPoint32W(hdc, text ? text : L"??",
                                      text ? (int)wcslen(text) : 2, &sz);
                ReleaseDC(c ? c : g_app.hMain, hdc);
                MoveWindow(c, x, 5, sz.cx + 18, 24, TRUE);
                x += sz.cx + 24;
            }
        }
    }
    /* N9(d): 变量栏自动隐藏后，只留左侧细条；右侧窗口区从细条右侧开始 */
    if (g_app.tree_hidden) {
        if (g_app.hTreeStrip && IsWindow(g_app.hTreeStrip)) {
            MoveWindow(g_app.hTreeStrip, 0, bh, 8, right_h, TRUE);
            ShowWindow(g_app.hTreeStrip, SW_SHOW); /* Bug12: 隐藏变量栏时显示左侧细条 */
        }
        if (IsWindow(g_app.hTree)) ShowWindow(g_app.hTree, SW_HIDE);
        sw = 8;
        right_w = bw - sw - 5;
    } else {
        if (g_app.hTreeStrip && IsWindow(g_app.hTreeStrip))
            ShowWindow(g_app.hTreeStrip, SW_HIDE);
        if (IsWindow(g_app.hTree)) ShowWindow(g_app.hTree, SW_SHOW);
    }
    MoveWindow(g_app.hSplitV, sw, bh, 5, right_h, TRUE);
    MoveWindow(g_app.hTree, 0, bh, g_app.tree_w, right_h, TRUE);
    /* N12: 钉图标浮在变量栏右上角（隐藏时一并隐藏） */
    if (g_app.hTreePin && IsWindow(g_app.hTreePin)) {
        if (g_app.tree_hidden) {
            ShowWindow(g_app.hTreePin, SW_HIDE);
        } else {
            MoveWindow(g_app.hTreePin, g_app.tree_w - 34, bh + 4, 30, 22, TRUE);
            ShowWindow(g_app.hTreePin, SW_SHOW);
        }
    }
    MoveWindow(g_app.hRight, sw + 5, bh, right_w - 5, right_h, TRUE);
    if (g_app.hTab && IsWindow(g_app.hTab))
        MoveWindow(g_app.hTab, 0, 0, right_w - 5, right_h, TRUE);
    if (g_app.log_hidden) {
        /* F22: 消息栏收起——隐藏消息栏与分隔条，显示底部细条 */
        if (g_app.hLogStrip && IsWindow(g_app.hLogStrip)) {
            MoveWindow(g_app.hLogStrip, 0, rc.bottom - 22 - LOG_STRIP_H, bw, LOG_STRIP_H, TRUE);
            ShowWindow(g_app.hLogStrip, SW_SHOW);
        }
        if (IsWindow(g_app.hSplitH)) ShowWindow(g_app.hSplitH, SW_HIDE);
        if (IsWindow(g_app.hLog)) ShowWindow(g_app.hLog, SW_HIDE);
    } else {
        if (g_app.hLogStrip && IsWindow(g_app.hLogStrip)) ShowWindow(g_app.hLogStrip, SW_HIDE);
        if (g_app.hSplitH && IsWindow(g_app.hSplitH)) {
            MoveWindow(g_app.hSplitH, 0, bh + right_h, bw, 5, TRUE);
            ShowWindow(g_app.hSplitH, SW_SHOW);
        }
        if (IsWindow(g_app.hLog)) {
            MoveWindow(g_app.hLog, 0, bh + right_h + 5, bw, logh, TRUE);
            ShowWindow(g_app.hLog, SW_SHOW);
        }
    }
    MoveWindow(g_app.hStatus, 0, rc.bottom - 22, bw, 22, TRUE);
    parts[0] = 170; parts[1] = 420; parts[2] = 620; parts[3] = -1;
    SendMessageW(g_app.hStatus, SB_SETPARTS, 4, (LPARAM)parts);
    /* Bug3: 全屏窗口随主窗口缩放同步铺满整个客户区 */
    if (g_app.fs_win && IsWindow(g_app.fs_win))
        MoveWindow(g_app.fs_win, 0, 0, rc.right, rc.bottom, TRUE);
    os_mainwin_tile();
}

/* ---------- 分割条 ---------- */

/* Bug18: 设置变量栏完全隐藏/展开状态并刷新布局（消息栏 log_set_hidden 同款交互） */
static void tree_set_hidden(int hidden)
{
    g_app.tree_hidden = hidden ? 1 : 0;
    layout();
    os_log(OS_LOG_INFO, "变量栏%s", g_app.tree_hidden ? "已完全隐藏" : "已展开");
}

static LRESULT CALLBACK split_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_LBUTTONDBLCLK: /* Bug18: 双击左侧分隔条 → 变量栏完全隐藏/展开 */
        tree_set_hidden(g_app.tree_hidden ? 0 : 1);
        return 0;
    case WM_LBUTTONDOWN:
        SetCapture(hwnd);
        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        return 0;
    case WM_MOUSEMOVE:
        if (GetCapture() == hwnd) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(GetParent(hwnd), &pt);
            SendMessage(GetParent(hwnd), WM_OS_SPLIT, (WPARAM)pt.x, 0);
        }
        return 0;
    case WM_LBUTTONUP:
        if (GetCapture() == hwnd) ReleaseCapture();
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_BORDER));
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/* ---------- F14 横向分隔条（消息栏上下拉伸） + F22 抽屉收起/弹出 ---------- */

/* F22: 设置消息栏收起/展开状态并刷新布局 */
static void log_set_hidden(int hidden)
{
    g_app.log_hidden = hidden ? 1 : 0;
    layout();
    os_log(OS_LOG_INFO, "消息栏%s", g_app.log_hidden ? "已收起" : "已展开");
}

/* F22: 消息栏收起后的底部细条（点击弹出，水平方向） */
static LRESULT CALLBACK log_strip_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_PANEL));
        /* 顶部边框线（与分隔条同色） */
        {
            HPEN pen = CreatePen(PS_SOLID, 1, os_theme(TH_BORDER));
            HGDIOBJ op = SelectObject(hdc, pen);
            MoveToEx(hdc, rc.left, rc.top, NULL);
            LineTo(hdc, rc.right, rc.top);
            SelectObject(hdc, op);
            DeleteObject(pen);
        }
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, os_theme(TH_TEXT));
        SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
        DrawTextW(hdc, L"▲ 消息（点击展开）", -1, &rc,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_LBUTTONDOWN:
        log_set_hidden(0);
        return 0;
    case WM_SETCURSOR:
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return 1;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK splith_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_LBUTTONDBLCLK: /* F22: 双击横向分隔条 → 消息栏抽屉收起/弹出 */
        log_set_hidden(g_app.log_hidden ? 0 : 1);
        return 0;
    case WM_LBUTTONDOWN:
        SetCapture(hwnd);
        SetCursor(LoadCursor(NULL, IDC_SIZENS));
        return 0;
    case WM_MOUSEMOVE:
        if (GetCapture() == hwnd) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(GetParent(hwnd), &pt);
            SendMessage(GetParent(hwnd), WM_OS_SPLIT_V, (WPARAM)pt.y, 0);
        }
        return 0;
    case WM_LBUTTONUP:
        if (GetCapture() == hwnd) ReleaseCapture();
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_BORDER));
        /* F22: 分隔条中央画一个小向下三角，提示"双击收起" */
        {
            HPEN pen = CreatePen(PS_SOLID, 1, os_theme(TH_TEXT));
            HGDIOBJ op = SelectObject(hdc, pen);
            HBRUSH ob = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            int cx = (rc.left + rc.right) / 2, cy = (rc.top + rc.bottom) / 2;
            POINT tri[3];
            tri[0].x = cx - 4; tri[0].y = cy - 1;
            tri[1].x = cx + 4; tri[1].y = cy - 1;
            tri[2].x = cx;     tri[2].y = cy + 2;
            Polygon(hdc, tri, 3);
            SelectObject(hdc, ob);
            SelectObject(hdc, op);
            DeleteObject(pen);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/* ---------- N9(d) 变量栏自动隐藏/钉住 ---------- */

static ULONGLONG g_tree_in_ms; /* 光标最后在树区的时间戳（GetTickCount64，避免 32 位溢出） */

static void refresh_tree_pin(void)
{
    /* Bug8: 自动隐藏/钉住唯一入口是左侧变量树 OSTreePin 钉图标，刷新其颜色（金=钉住/灰=自动隐藏） */
    if (g_app.hTreePin && IsWindow(g_app.hTreePin)) InvalidateRect(g_app.hTreePin, NULL, TRUE);
}

static void tree_auto_expand(const wchar_t* why)
{
    char why8[64];
    if (!g_app.tree_hidden) return;
    g_app.tree_hidden = 0;
    layout();
    /* os_log %ls 在 C locale 下转多字节中文失败（vsnprintf 返回 -1 产生空行），故先转 UTF-8 窄串 */
    os_wide_to_utf8_buf(why, why8, sizeof(why8));
    os_log(OS_LOG_INFO, "变量栏展开: %s", why8);
}

static void tree_auto_tick(void)
{
    POINT pt;
    int x;
    if (!g_app.hMain || !g_app.hTree) return;
    GetCursorPos(&pt);
    ScreenToClient(g_app.hMain, &pt);
    x = pt.x;
    if (g_app.tree_hidden) {
        /* 隐藏态（手动完全隐藏或自动隐藏）：光标进入左侧细条即展开。
         * 用户反馈修复：旧实现不区分模式——钉住状态（默认）下双击分隔条隐藏后，
         * 光标恰好停在细条上（x≤10），200ms 后悬停判定立即把树重新弹出，
         * "不能全部缩进左侧全隐藏"的根因。只有自动隐藏模式（tree_auto=1）
         * 才悬停展开；钉住模式必须显式点击细条/双击分隔条展开。 */
        if (g_app.tree_auto && x >= 0 && x <= 10) tree_auto_expand(L"悬停细条");
        return;
    }
    if (!g_app.tree_auto) {
        /* 钉住：始终展开 */
        tree_auto_expand(L"钉住");
        return;
    }
    /* 展开态：光标仍在树区或分隔条上 → 保持；离开超过 800ms → 自动隐藏 */
    if (x >= 0 && x <= g_app.tree_w + 12) {
        g_tree_in_ms = GetTickCount64();
        return;
    }
    if (GetTickCount64() - g_tree_in_ms > 800) {
        g_app.tree_hidden = 1;
        layout();
        os_log(OS_LOG_INFO, "变量栏自动隐藏");
    }
}

static LRESULT CALLBACK tree_strip_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        RECT rc;
        HDC hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_PANEL));
        SetBkMode(hdc, TRANSPARENT);
        SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
        SetTextColor(hdc, os_theme(TH_TEXT));
        DrawTextW(hdc, L"❯", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_LBUTTONDOWN:
        tree_auto_expand(L"点击细条");
        return 0;
    case WM_SETCURSOR:
        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        return 1;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/* N12: 变量栏顶部的钉图标按钮（GDI 绘制：钉住=金色，自动隐藏=灰） */
static void pin_draw(HDC hdc, RECT* rc, int pinned)
{
    int cx = (rc->left + rc->right) / 2;
    int cy = (rc->top + rc->bottom) / 2 + 1;
    POINT tri[3];
    HBRUSH br, oldbr;
    HPEN oldpen;
    SetBkMode(hdc, TRANSPARENT);
    br = CreateSolidBrush(pinned ? RGB(212, 175, 55) : RGB(150, 150, 150));
    oldbr = (HBRUSH)SelectObject(hdc, br);
    oldpen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
    /* 钉头 */
    Ellipse(hdc, cx - 5, cy - 7, cx + 5, cy + 3);
    /* 钉尖 */
    tri[0].x = cx;     tri[0].y = cy + 1;
    tri[1].x = cx - 6; tri[1].y = cy + 11;
    tri[2].x = cx + 6; tri[2].y = cy + 11;
    Polygon(hdc, tri, 3);
    SelectObject(hdc, oldbr);
    SelectObject(hdc, oldpen);
    DeleteObject(br);
    /* 高光弧线 */
    {
        HPEN hp = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        SelectObject(hdc, hp);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Arc(hdc, cx - 5, cy - 7, cx + 5, cy + 3, cx - 3, cy - 5, cx + 1, cy - 8);
        DeleteObject(hp);
    }
}

static LRESULT CALLBACK tree_pin_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_PANEL));
        pin_draw(hdc, &rc, g_app.tree_auto ? 0 : 1);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_LBUTTONDOWN:
        /* Bug8: 树钉图标是自动隐藏/钉住唯一入口：切换状态并刷新图标颜色 */
        g_app.tree_auto = g_app.tree_auto ? 0 : 1;
        refresh_tree_pin();
        layout();
        os_log(OS_LOG_INFO, "变量栏%s", g_app.tree_auto ? "改为自动隐藏（未钉住）" : "已钉住常显");
        return 0;
    case WM_SETCURSOR:
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return 1;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/* ---------- ELF ---------- */

static int load_elf_path(const wchar_t* path)
{
    char utf8[MAX_PATH];
    char err[256];
    OS_ElfFile* elf;
    wchar_t wmsg[2048];
    int i;
    os_wide_to_utf8_buf(path, utf8, MAX_PATH);
    elf = os_elf_open(utf8, err, sizeof(err));
    if (!elf) {
        /* 不弹窗：只在下方日志窗口警告（用户删除/改名 ELF 后启动不再打扰） */
        os_log(OS_LOG_WARN, "ELF 加载失败: %s", err);
        return -1;
    }
    if (g_app.elf) os_elf_close(g_app.elf);
    g_app.elf = elf;
    _snwprintf(g_app.elf_path, MAX_PATH, L"%s", path);
    g_app.elf_mtime = os_file_mtime_ms(path);
    os_vartree_build();
    os_vartree_fill_tree(g_app.hTree);
    os_layout_apply_pending(); /* 布局恢复时未解析的变量，ELF 就绪后补挂 */
    {
        /* 需求2：ELF 重载后叶下标可能漂移——全部本地窗口按变量名重绑 leaf_id，
         * 否则窗口会静默绑到错误变量（错地址/错数据）。 */
        int wi, k;
        for (wi = 0; wi < g_app.win_count; wi++) {
            for (k = 0; k < g_app.wins[wi].group_count; k++) {
                HWND w = g_app.wins[wi].group[k];
                if (!w || !IsWindow(w) || g_app.wins[wi].is_module) continue;
                if (os_chart_is(w)) os_chart_rebind(w);
                else if (os_num_is(w)) os_num_rebind(w);
            }
        }
    }
    /* 缺失变量提示 */
    if (os_vartree_missing_count() > 0) {
        wchar_t list[1800] = L"";
        for (i = 0; i < os_vartree_missing_count() && i < 12; i++) {
            wchar_t tmp[300];
            os_utf8_to_wide_buf(os_vartree_missing_at(i), tmp, 300);
            _snwprintf(list + wcslen(list), 1800 - (int)wcslen(list), L"%s\n", tmp);
        }
        _snwprintf(wmsg, 2048, L"以下 %d 个变量在新 ELF 中未找到，是否忽略？\n\n%s",
                   os_vartree_missing_count(), list);
        MessageBoxW(g_app.hMain, wmsg, L"变量缺失", MB_YESNO | MB_ICONWARNING);
    }
    /* 通知模块 */
    for (i = 0; i < g_app.winmod_count; i++) {
        if (g_app.winmods[i]->command)
            g_app.winmods[i]->command(g_app.winmod_ctx[i], OS_CMD_ELF_RELOADED, NULL, NULL);
    }
    os_log(OS_LOG_INFO, "已加载 ELF: %ls (%d 位, %s)",
           path, os_elf_bits(g_app.elf), os_elf_arch_name(g_app.elf));
    refresh_status();
    os_mainwin_update_buttons();
    return 0;
}

int os_mainwin_reload_elf(void)
{
    if (!g_app.elf_path[0]) return -1;
    return load_elf_path(g_app.elf_path);
}

int os_mainwin_open_elf(const wchar_t* path)
{
    if (!path || !path[0]) return -1;
    return load_elf_path(path);
}

static void cmd_open_elf(void)
{
    OPENFILENAMEW ofn;
    wchar_t file[MAX_PATH] = L"";
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_app.hMain;
    ofn.lpstrFilter = L"ELF 文件\0*.elf;*.axf;*.out\0所有文件\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (GetOpenFileNameW(&ofn)) os_mainwin_open_elf(file);
}

static void check_elf_mtime(void)
{
    uint64_t mt;
    if (!g_app.elf_path[0] || !g_app.elf) return;
    mt = os_file_mtime_ms(g_app.elf_path);
    if (mt && mt != g_app.elf_mtime) {
        g_app.elf_mtime = mt;
        if (MessageBoxW(g_app.hMain,
                        L"检测到 ELF 文件已更新（可能重新编译），是否立即重新加载并刷新变量地址？",
                        L"ELF 更新", MB_YESNO | MB_ICONQUESTION) == IDYES) {
            load_elf_path(g_app.elf_path);
        }
    }
}

/* ---------- 连接 / 采集 / 记录 ---------- */

static void cmd_connect(void)
{
    int rc, c = 0;
    OS_DriverInfo info;
    OS_ConnectCfg cfg;
    LRESULT ifi, emu, dci;
    if (!g_app.driver || !g_app.driver->command) {
        os_log(OS_LOG_ERROR, "未加载驱动模块（jlink.dll）");
        set_status(0, L"未加载驱动模块");
        return;
    }
    /* 无仿真器：弹窗提示（需求：扫描不到 J-Link 设备时告知用户） */
    if (g_emu_count <= 0) {
        MessageBoxW(g_app.hMain,
                    L"没有发现 JLink 设备，请确认仿真器已插入 USB 并点击「刷新」。",
                    L"J-Link", MB_OK | MB_ICONWARNING);
        return;
    }
    /* 直接读取界面配置，不再弹配置对话框 */
    memset(&cfg, 0, sizeof(cfg));
    ifi = SendMessageW(GetDlgItem(g_app.hMain, IDC_CFG_IFACE), CB_GETCURSEL, 0, 0);
    cfg.iface = (ifi == 1) ? OS_IF_JTAG : OS_IF_SWD;
    /* N8: 速度下拉可手工输入，直接解析输入框文本（"0 (自动)" 或非法输入 -> 0 自动） */
    {
        wchar_t sbuf[64];
        char sb8[64];
        HWND hs = GetDlgItem(g_app.hMain, IDC_CFG_SPEED);
        SendMessageW(hs, WM_GETTEXT, 64, (LPARAM)sbuf);
        os_wide_to_utf8_buf(sbuf, sb8, sizeof(sb8));
        cfg.speed_khz = atoi(sb8);
        if (cfg.speed_khz < 0) cfg.speed_khz = 0;
        g_app.speed_khz = cfg.speed_khz; /* 采集线程块读合并成本模型按实际速度计算 */
    }
    emu = SendMessageW(GetDlgItem(g_app.hMain, IDC_CFG_EMU), CB_GETCURSEL, 0, 0);
    cfg.probe_index = (emu == CB_ERR) ? -1 : (int)emu;
    /* 芯片核心：从界面下拉读取（F17 核心名列表，默认 Cortex-M4；纯内存读写无需具体型号） */
    dci = SendMessageW(GetDlgItem(g_app.hMain, IDC_CFG_DEVICE), CB_GETCURSEL, 0, 0);
    if (dci >= 0 && dci < (LRESULT)(sizeof(g_devices) / sizeof(g_devices[0]))) {
        char dev8[128];
        os_wide_to_utf8_buf(g_devices[dci], dev8, sizeof(dev8));
        _snprintf(cfg.device, sizeof(cfg.device), "%s", dev8);
    } else {
        _snprintf(cfg.device, sizeof(cfg.device), "%s", "Cortex-M4");
    }
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_CONNECT, &cfg, NULL);
    if (rc != OS_ERR_OK) {
        set_status(0, L"连接失败");
        os_log(OS_LOG_ERROR, "连接失败 (rc=%d)，请检查仿真器与目标板", rc);
        os_mainwin_update_buttons();
        return;
    }
    g_app.driver->command(g_app.driver_ctx, OS_CMD_IS_CONNECTED, NULL, &c);
    g_app.connected = c;
    memset(&info, 0, sizeof(info));
    g_app.driver->command(g_app.driver_ctx, OS_CMD_GET_INFO, NULL, &info);
    os_log(OS_LOG_INFO, "已连接: %s（%s, J-Link DLL %s, HW %d, FW %d）",
           info.emulator[0] ? info.emulator : "?", info.name, info.dll_version,
           info.hw_version, info.fw_version);
    refresh_status();
    os_mainwin_update_buttons();
}

static void cmd_disconnect(void)
{
    if (g_app.driver && g_app.driver->command) {
        g_app.driver->command(g_app.driver_ctx, OS_CMD_DISCONNECT, NULL, NULL);
    }
    g_app.connected = 0;
    os_log(OS_LOG_INFO, "已断开连接");
    refresh_status();
    os_mainwin_update_buttons();
}

/* 扫描 J-Link 设备并填充主界面下拉（返回设备数；不弹窗，结果进日志/下拉） */
static int cfg_fill_emus(void)
{
    OS_DeviceInfo items[16];
    OS_ScanReq req;
    HWND h;
    int i, n;
    if (!g_app.driver || !g_app.driver->command) return -1;
    memset(&req, 0, sizeof(req));
    req.items = items;
    req.capacity = 16;
    g_app.driver->command(g_app.driver_ctx, OS_CMD_SCAN, &req, NULL);
    n = req.count;
    g_emu_count = n;
    h = GetDlgItem(g_app.hMain, IDC_CFG_EMU);
    if (!h) return n;
    SendMessageW(h, CB_RESETCONTENT, 0, 0);
    for (i = 0; i < n && i < 16; i++) {
        wchar_t line[256];
        _snwprintf(line, 256, L"%hs (SN:%hs)", items[i].name, items[i].serial);
        SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)line);
    }
    if (n > 0) {
        SendMessageW(h, CB_SETCURSEL, 0, 0);
    } else {
        SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)L"未发现 J-Link 设备");
        SendMessageW(h, CB_SETCURSEL, 0, 0);
    }
    os_log(OS_LOG_INFO, "J-Link 设备扫描: %d 个", n);
    return n;
}

/* 主界面连接配置初始化：MCU型号/接口/速度下拉 + 扫描设备列表（模块加载后调用） */
void os_mainwin_cfg_init(void)
{
    HWND h;
    const wchar_t* ifaces[] = { L"SWD", L"JTAG" };
    /* N8: 更多速度预置（kHz），下拉可编辑允许手工输入任意值。
     * J-Link Pro/Ultra+ SWD/JTAG 最高 50MHz——加入 20/30/40/50MHz 预置；
     * 更高速度缩短块读的线上传输时间（合并跨空隙读取时尤为关键）。 */
    static const int speeds[] = { 0, 50, 100, 200, 400, 1000, 2000, 4000, 5000, 8000, 10000, 12000, 15000, 20000, 25000, 30000, 40000, 50000 };
    int i;
    h = GetDlgItem(g_app.hMain, IDC_CFG_DEVICE);
    if (h) {
        for (i = 0; i < (int)(sizeof(g_devices) / sizeof(g_devices[0])); i++)
            SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)g_devices[i]);
        SendMessageW(h, CB_SETCURSEL, 0, 0); /* 默认 Cortex-M4 */
    }
    h = GetDlgItem(g_app.hMain, IDC_CFG_IFACE);
    if (h) {
        for (i = 0; i < 2; i++) SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)ifaces[i]);
        SendMessageW(h, CB_SETCURSEL, 0, 0);
    }
    h = GetDlgItem(g_app.hMain, IDC_CFG_SPEED);
    if (h) {
        for (i = 0; i < (int)(sizeof(speeds) / sizeof(speeds[0])); i++) {
            wchar_t b[32];
            if (speeds[i] == 0) _snwprintf(b, 32, L"0 (自动)");
            else _snwprintf(b, 32, L"%d", speeds[i]);
            SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)b);
        }
        SendMessageW(h, CB_SETCURSEL, 7, 0); /* 默认 4000 */
    }
    cfg_fill_emus();
}

/* 向所有原生波形窗口广播视图消息（FITALL=整体展示，LIVE=跟随最新）。
 * Bug19：遍历每个 tab 的 group[] 全部窗口，不只 group[0]。 */
static void chart_broadcast(UINT msg)
{
    int i, k;
    for (i = 0; i < g_app.win_count; i++) {
        if (g_app.wins[i].is_module) continue;
        for (k = 0; k < g_app.wins[i].group_count; k++) {
            HWND w = g_app.wins[i].group[k];
            if (w && IsWindow(w) && os_chart_is(w))
                PostMessage(w, msg, 0, 0);
        }
    }
}

static void cmd_start_acq(void)
{
    if (!g_app.connected) {
        MessageBoxW(g_app.hMain, L"请先连接 MCU", L"采集", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (os_ds_start() == 0) {
        chart_broadcast(WM_OS_CHART_LIVE); /* 开始采集：波形回到跟随最新 */
        refresh_status();
        os_mainwin_update_buttons();
    }
}

static void cmd_stop_acq(void)
{
    os_ds_stop();
    chart_broadcast(WM_OS_CHART_FITALL); /* 停止记录后波形整体展示到整个区域 */
    refresh_status();
    os_mainwin_update_buttons();
}

static void cmd_log_start(void)
{
    OPENFILENAMEW ofn;
    wchar_t file[MAX_PATH] = L"data.csv";
    int was_running = 0;
    /* Bug7: 采集运行中弹文件对话框会与采集线程跨线程日志/模态循环互卡导致界面卡死。
     * 与 cmd_replay_open 一致：先停采集，对话框关闭后再恢复。 */
    if (g_app.acq_state == OS_ACQ_RUNNING) {
        os_ds_stop();
        was_running = 1;
    }
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_app.hMain;
    ofn.lpstrFilter = L"CSV 文件\0*.csv\0所有文件\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"csv";
    ofn.Flags = OFN_OVERWRITEPROMPT;
    if (GetSaveFileNameW(&ofn)) {
        if (os_datalog_start(file) != 0)
            MessageBoxW(g_app.hMain, L"无法创建记录文件", L"记录", MB_OK | MB_ICONERROR);
    }
    if (was_running) os_ds_start(); /* 对话框关闭后恢复采集 */
    os_mainwin_update_buttons();
}

static void cmd_log_stop(void)
{
    os_datalog_stop();
    os_mainwin_update_buttons();
}

static void cmd_replay_open(void)
{
    OPENFILENAMEW ofn;
    wchar_t file[MAX_PATH] = L"";
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_app.hMain;
    ofn.lpstrFilter = L"CSV 记录\0*.csv\0所有文件\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetOpenFileNameW(&ofn)) return;
    if (g_app.acq_state == OS_ACQ_RUNNING) os_ds_stop();
    if (os_replay_start(file) != 0) {
        MessageBoxW(g_app.hMain, L"无法打开回放文件", L"回放", MB_OK | MB_ICONERROR);
        return;
    }
    SetTimer(g_app.hMain, 2, 10, NULL);
    refresh_status();
    os_mainwin_update_buttons();
}

static void cmd_replay_stop(void)
{
    KillTimer(g_app.hMain, 2);
    os_replay_stop();
    refresh_status();
    os_mainwin_update_buttons();
}

/* ---------- 窗口管理 ---------- */

static void add_win_item(HWND hwnd, int is_module, OS_Module* mod, void* ctx, const wchar_t* title)
{
    OS_WinItem* wi;
    if (g_app.win_count >= OS_MAX_WINS) {
        DestroyWindow(hwnd);
        return;
    }
    wi = &g_app.wins[g_app.win_count++];
    memset(wi, 0, sizeof(*wi));
    wi->hwnd = hwnd;
    wi->group[0] = hwnd;
    wi->group_count = 1;
    wi->group_max = -1;
    wi->is_module = is_module;
    wi->mod = mod;
    wi->mod_ctx = ctx;
    wi->active = 1;
    _snwprintf(wi->title, 128, L"%s", title ? title : L"");
    _snwprintf(wi->group_title[0], 128, L"%s", title ? title : L"");
    os_tile_ratios_init(wi->col_ratio, wi->group_count); /* Bug6: 平铺列宽默认均分 */
    g_cur_tab = g_app.win_count - 1;
    os_mainwin_tile();
    if (g_app.rename_tab[0] && g_app.win_count == 1)
        tab_set_title(0, g_app.rename_tab); /* 测试钩子：首个窗口创建后重命名 */
    if (g_app.shot_path[0]) {
        wchar_t p[MAX_PATH];
        _snwprintf(p, MAX_PATH, L"%s.%d.bmp", g_app.shot_path, g_app.win_count);
        os_log(OS_LOG_INFO, "截图: %ls", p);
        os_save_window_bmp(hwnd, p);
    }
}

/* N11: 在指定 tab 内新建窗口。tab<0 → 新建独立 tab；tab 有效 → 附加到该 tab 组。 */
HWND os_win_add_to_tab(int tab, const char* type, const wchar_t* title)
{
    HWND h;
    int i;
    /* 创建窗口（父为 tab 页区域），模块/原生分别处理 */
    if (strcmp(type, "chart") == 0)
        h = os_chart_create(g_app.hTab, 0, 0, 200, 150, title);
    else if (strcmp(type, "num") == 0)
        h = os_num_create(g_app.hTab, 0, 0, 200, 150, title);
    else {
        h = NULL;
        for (i = 0; i < g_modwin_menu_count; i++) {
            ModWinMenuItem* it = &g_modwin_menu[i];
            if (it->mod && it->wt && strcmp(it->wt->type, type) == 0) {
                char title8[256];
                os_wide_to_utf8_buf(title ? title : L"", title8, sizeof(title8));
                h = it->mod->create_window(it->ctx, it->wt->type, g_app.hTab,
                                           0, 0, 200, 150, title8);
                if (h && tab >= 0 && tab < g_app.win_count) {
                    /* 模块窗口附加到已有 tab */
                    OS_WinItem* wi = &g_app.wins[tab];
                    if (wi->group_count >= OS_MAX_GROUP) { DestroyWindow(h); return NULL; }
                    wi->group[wi->group_count] = h;
                    _snwprintf(wi->group_title[wi->group_count], 128, L"%s",
                               title ? title : L"");
                    wi->group_count++;
                    os_tile_ratios_init(wi->col_ratio, wi->group_count); /* Bug6: 均分 */
                    os_mainwin_tile();
                    return h;
                }
                if (h) add_win_item(h, 1, it->mod, it->ctx, title);
                return h;
            }
        }
        return NULL;
    }
    if (!h) return NULL;
    if (tab >= 0 && tab < g_app.win_count) {
        /* 附加到已有 tab */
        OS_WinItem* wi = &g_app.wins[tab];
        if (wi->group_count >= OS_MAX_GROUP) { DestroyWindow(h); return NULL; }
        wi->group[wi->group_count] = h;
        _snwprintf(wi->group_title[wi->group_count], 128, L"%s", title ? title : L"");
        wi->group_count++;
        os_tile_ratios_init(wi->col_ratio, wi->group_count); /* Bug6: 均分 */
        os_log(OS_LOG_INFO, "窗口已附加到当前标签: %ls (tab%d 共%d个)",
               title ? title : L"", tab, wi->group_count);
        os_mainwin_tile();
        return h;
    }
    add_win_item(h, 0, NULL, NULL, title);
    return h;
}

static void cmd_add_window(int native_chart)
{
    wchar_t title[128];
    static int chart_no = 1, num_no = 1;
    if (native_chart) _snwprintf(title, 128, L"波形窗口 %d", chart_no++);
    else _snwprintf(title, 128, L"数值窗口 %d", num_no++);
    os_win_add_to_tab(-1, native_chart ? "chart" : "num", title);
}

/* N11: 在当前 tab 内附加一个新的波形/数值窗口 */
static void cmd_add_to_tab(int native_chart)
{
    wchar_t title[128];
    static int tchart_no = 1, tnum_no = 1;
    if (g_cur_tab < 0 || g_cur_tab >= g_app.win_count) { cmd_add_window(native_chart); return; }
    if (native_chart) _snwprintf(title, 128, L"波形窗口 %d", tchart_no++);
    else _snwprintf(title, 128, L"数值窗口 %d", tnum_no++);
    os_win_add_to_tab(g_cur_tab, native_chart ? "chart" : "num", title);
}

void os_mainwin_rebuild_window_menu(void)
{
    int i;
    wchar_t wname[128];
    if (!g_menu_win) return;
    while (GetMenuItemCount(g_menu_win) > 0)
        DeleteMenu(g_menu_win, 0, MF_BYPOSITION);
    AppendMenuW(g_menu_win, MF_STRING, IDM_WIN_CHART, L"波形窗口");
    AppendMenuW(g_menu_win, MF_STRING, IDM_WIN_NUM, L"数值窗口");
    g_modwin_menu_count = 0;
    for (i = 0; i < g_app.winmod_count; i++) {
        const OS_WindowType* wt = g_app.winmods[i]->window_types;
        while (wt && wt->type && g_modwin_menu_count < 64) {
            os_utf8_to_wide_buf(wt->display_name ? wt->display_name : wt->type, wname, 128);
            AppendMenuW(g_menu_win, MF_STRING, IDM_WIN_MODULE_BASE + g_modwin_menu_count,
                        wname);
            g_modwin_menu[g_modwin_menu_count].mod = g_app.winmods[i];
            g_modwin_menu[g_modwin_menu_count].ctx = g_app.winmod_ctx[i];
            g_modwin_menu[g_modwin_menu_count].wt = wt;
            g_modwin_menu_count++;
            wt++;
        }
    }
    DrawMenuBar(g_app.hMain);
}

/* ---------- 对话框 ---------- */

static void run_modal(HWND dlg, HWND owner)
{
    MSG msg;
    ShowWindow(dlg, SW_SHOW);
    UpdateWindow(dlg);
    if (owner) EnableWindow(owner, FALSE);
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsWindow(dlg)) break;
        if (IsDialogMessageW(dlg, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (owner) {
        EnableWindow(owner, TRUE);
        SetForegroundWindow(owner);
    }
    if (IsWindow(dlg)) DestroyWindow(dlg);
}

static void pick_refresh(HWND dlg)
{
    PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
    wchar_t wtext[512];
    char text[512];
    HWND hList = GetDlgItem(dlg, IDD_PICK_LIST);
    HWND hEdit = GetDlgItem(dlg, IDD_PICK_EDIT);
    int i;
    if (!st) return;
    GetWindowTextW(hEdit, wtext, 512);
    WideCharToMultiByte(CP_UTF8, 0, wtext, -1, text, sizeof(text), NULL, NULL);
    st->count = os_vartree_search(text, 300, st->ids);
    SendMessageW(hList, LVM_DELETEALLITEMS, 0, 0);
    for (i = 0; i < st->count; i++) {
        const OS_Leaf* L = os_vartree_leaf(st->ids[i]);
        char full[420];
        wchar_t wfull[420];
        LVITEMW it;
        if (!L) continue;
        _snprintf(full, 420, "%s @0x%llX", L->name, (unsigned long long)L->address);
        os_utf8_to_wide_buf(full, wfull, 420);
        memset(&it, 0, sizeof(it));
        it.mask = LVIF_TEXT;
        it.iItem = i;
        it.pszText = wfull;
        SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&it);
    }
}

/* N13a: 收集 ListView 中全部选中项（多选）到结果 */
static void pick_collect(HWND dlg, PickState* st)
{
    HWND hList = GetDlgItem(dlg, IDD_PICK_LIST);
    int n = (int)SendMessageW(hList, LVM_GETITEMCOUNT, 0, 0);
    int i, out = 0;
    if (!st || !st->pr) return;
    st->pr->count = 0;
    for (i = 0; i < n && out < 512; i++) {
        if (ListView_GetItemState(hList, i, LVIS_SELECTED)) {
            if (i < st->count) st->pr->ids[out++] = st->ids[i];
        }
    }
    st->pr->count = out;
}

/* 前向声明：pick_proc 的右键添加复用树右键的批量添加逻辑（定义在后面） */
static void add_ids_to_native(int chart, const int* ids, int n);

/* 用户反馈：搜索/选变量对话框列表右键菜单——添加选中项到波形/数值窗口。
 * 对话框保持打开，可连续添加；无选中项时菜单置灰。 */
static void pick_show_add_menu(HWND dlg)
{
    HMENU m = CreatePopupMenu();
    POINT pt;
    int has_sel = 0, i2;
    HWND hList = GetDlgItem(dlg, IDD_PICK_LIST);
    int n2 = hList ? (int)SendMessageW(hList, LVM_GETITEMCOUNT, 0, 0) : 0;
    for (i2 = 0; i2 < n2 && !has_sel; i2++)
        if (SendMessageW(hList, LVM_GETITEMSTATE, (WPARAM)i2, LVIS_SELECTED) & LVIS_SELECTED)
            has_sel = 1;
    AppendMenuW(m, MF_STRING | (has_sel ? 0 : MF_GRAYED), IDM_PICK_ADD_CHART, L"添加到波形窗口");
    AppendMenuW(m, MF_STRING | (has_sel ? 0 : MF_GRAYED), IDM_PICK_ADD_NUM, L"添加到数值窗口");
    GetCursorPos(&pt);
    TrackPopupMenu(m, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, dlg, NULL);
    DestroyMenu(m);
}

static LRESULT CALLBACK pick_proc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE: {
        PickState* st = (PickState*)calloc(1, sizeof(PickState));
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        HWND hList;
        LVCOLUMNW lc;
        if (!st) return -1;
        if (cs) st->pr = (PickResult*)cs->lpCreateParams;
        SetWindowLongPtrW(dlg, GWLP_USERDATA, (LONG_PTR)st);
        CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                      10, 10, 420, 24, dlg, (HMENU)IDD_PICK_EDIT, g_app.hInst, NULL);
        /* N13a: 列表改 ListView（报表模式，原生支持 Ctrl+单击多选 / Shift 起止范围选） */
        hList = CreateWindowW(WC_LISTVIEWW, L"",
                              WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LVS_REPORT | LVS_SHOWSELALWAYS,
                              10, 40, 420, 280, dlg, (HMENU)IDD_PICK_LIST, g_app.hInst, NULL);
        ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
        memset(&lc, 0, sizeof(lc));
        lc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lc.pszText = L"变量（名称 @ 地址）";
        lc.cx = 400;
        SendMessageW(hList, LVM_INSERTCOLUMNW, 0, (LPARAM)&lc);
        /* F20: 列表配色随主题 */
        ListView_SetBkColor(hList, os_theme(TH_LOG_BG));
        ListView_SetTextColor(hList, os_theme(TH_LOG_TEXT));
        ListView_SetTextBkColor(hList, os_theme(TH_LOG_BG));
        os_theme_listview_header(hList);
        CreateWindowW(L"BUTTON", L"确定", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                      310, 330, 60, 26, dlg, (HMENU)IDD_PICK_OK, g_app.hInst, NULL);
        CreateWindowW(L"BUTTON", L"取消", WS_CHILD | WS_VISIBLE,
                      380, 330, 60, 26, dlg, (HMENU)IDD_PICK_CANCEL, g_app.hInst, NULL);
        return 0;
    }
    case WM_ERASEBKGND: { /* F20 */
        RECT rc;
        HDC hdc = (HDC)wParam;
        GetClientRect(dlg, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_BG));
        return 1;
    }
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
        return theme_ctlcolor((HDC)wParam, 0);
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
        return theme_ctlcolor((HDC)wParam, 1);
    case WM_DESTROY: {
        PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
        free(st);
        return 0;
    }
    case WM_SIZE:
        MoveWindow(GetDlgItem(dlg, IDD_PICK_EDIT), 10, 10, LOWORD(lParam) - 20, 24, TRUE);
        MoveWindow(GetDlgItem(dlg, IDD_PICK_LIST), 10, 40, LOWORD(lParam) - 20, HIWORD(lParam) - 90, TRUE);
        MoveWindow(GetDlgItem(dlg, IDD_PICK_OK), LOWORD(lParam) - 140, HIWORD(lParam) - 36, 60, 26, TRUE);
        MoveWindow(GetDlgItem(dlg, IDD_PICK_CANCEL), LOWORD(lParam) - 70, HIWORD(lParam) - 36, 60, 26, TRUE);
        return 0;
    case WM_OS_PICK_TEST_SELECT: {
        /* N13a 测试钩子：跨进程无法伪造键盘 Ctrl/Shift 状态，也无法用指针式
         * LVM_SETITEMSTATE 编组；此钩子由回归脚本发送，在对话框进程内
         * 程序化选中 [start, start+count) 范围（等价 Ctrl/Shift 手选结果）。 */
        HWND hList = GetDlgItem(dlg, IDD_PICK_LIST);
        int n = (int)SendMessageW(hList, LVM_GETITEMCOUNT, 0, 0);
        int start = (int)wParam, cnt = (int)lParam, i;
        LVITEMW it;
        memset(&it, 0, sizeof(it));
        it.mask = LVIF_STATE;
        it.stateMask = LVIS_SELECTED;
        for (i = 0; i < n; i++) {
            it.iItem = i;
            it.state = 0;
            SendMessageW(hList, LVM_SETITEMSTATE, (WPARAM)i, (LPARAM)&it);
        }
        for (i = start; i < n && i < start + cnt; i++) {
            it.iItem = i;
            it.state = LVIS_SELECTED;
            SendMessageW(hList, LVM_SETITEMSTATE, (WPARAM)i, (LPARAM)&it);
        }
        return 0;
    }
    case WM_OS_PICK_TEST_ADD: {
        /* 测试钩子：等价右键菜单"添加到波形/数值窗口"（wParam=1 波形, 0 数值）。
         * 对话框保持打开，可连续多次添加。 */
        PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
        int ids[512], i, n;
        if (!st || !st->pr) return 0;
        pick_collect(dlg, st); /* 回填选中项 */
        n = st->pr->count;
        if (n > 512) n = 512;
        for (i = 0; i < n; i++) ids[i] = st->pr->ids[i];
        add_ids_to_native(wParam ? 1 : 0, ids, n);
        os_log(OS_LOG_INFO, "搜索对话框添加变量: %d 个到%s", n,
               wParam ? "波形窗口" : "数值窗口");
        return 0;
    }
    case WM_CONTEXTMENU:
        /* 列表右键（含测试注入）→ 添加变量上下文菜单。
         * 用户反馈修复：旧实现同时处理 NM_RCLICK 和 WM_CONTEXTMENU——真实右键时
         * ListView 先发 NM_RCLICK（弹出菜单1），菜单1关闭后继续处理右键释放又发
         * WM_CONTEXTMENU（弹出菜单2）→ "点击任意选项后再次弹窗"。
         * 只保留 WM_CONTEXTMENU（右键链的最后一环，其后无后续弹窗）；
         * 且仅当右键来自变量列表时才弹出（编辑框等其他区域不弹）。 */
        if ((HWND)wParam == GetDlgItem(dlg, IDD_PICK_LIST))
            pick_show_add_menu(dlg);
        return 0;
    case WM_NOTIFY: {
        NMHDR* h = (NMHDR*)lParam;
        if (h && h->idFrom == IDD_PICK_LIST) {
            if (h->code == NM_DBLCLK) {
                /* 双击列表项 = 确定（全部选中项） */
                PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
                pick_collect(dlg, st);
                DestroyWindow(dlg);
                return 0;
            }
            if (h->code == LVN_KEYDOWN) {
                /* N13a: Ctrl+A 全选 */
                NMLVKEYDOWN* kv = (NMLVKEYDOWN*)lParam;
                if (kv && kv->wVKey == 'A' && (GetKeyState(VK_CONTROL) & 0x8000)) {
                    HWND hList = GetDlgItem(dlg, IDD_PICK_LIST);
                    int n = (int)SendMessageW(hList, LVM_GETITEMCOUNT, 0, 0);
                    int i;
                    LVITEMW it;
                    memset(&it, 0, sizeof(it));
                    it.mask = LVIF_STATE;
                    it.stateMask = LVIS_SELECTED;
                    for (i = 0; i < n; i++) {
                        it.iItem = i;
                        it.state = LVIS_SELECTED;
                        SendMessageW(hList, LVM_SETITEMSTATE, (WPARAM)i, (LPARAM)&it);
                    }
                    return 0;
                }
            }
        }
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_PICK_EDIT:
            if (HIWORD(wParam) == EN_CHANGE) pick_refresh(dlg);
            return 0;
        case IDD_PICK_OK: {
            PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            pick_collect(dlg, st);
            DestroyWindow(dlg);
            return 0;
        }
        case IDD_PICK_CANCEL:
            DestroyWindow(dlg);
            return 0;
        case IDM_PICK_ADD_CHART: { /* 列表右键：添加到波形窗口（对话框保持打开） */
            PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            int ids[512], i, n;
            if (!st || !st->pr) return 0;
            pick_collect(dlg, st);
            n = st->pr->count;
            if (n > 512) n = 512;
            for (i = 0; i < n; i++) ids[i] = st->pr->ids[i];
            add_ids_to_native(1, ids, n);
            os_log(OS_LOG_INFO, "搜索对话框添加变量: %d 个到波形窗口", n);
            return 0;
        }
        case IDM_PICK_ADD_NUM: { /* 列表右键：添加到数值窗口 */
            PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            int ids[512], i, n;
            if (!st || !st->pr) return 0;
            pick_collect(dlg, st);
            n = st->pr->count;
            if (n > 512) n = 512;
            for (i = 0; i < n; i++) ids[i] = st->pr->ids[i];
            add_ids_to_native(0, ids, n);
            os_log(OS_LOG_INFO, "搜索对话框添加变量: %d 个到数值窗口", n);
            return 0;
        }
        }
        break;
    }
    return DefWindowProcW(dlg, msg, wParam, lParam);
}

/* N13a: 共享对话框运行：多选结果写入 pr */
static int run_pick_dialog_titled(HWND owner, PickResult* pr, const wchar_t* title)
{
    HWND dlg;
    pr->count = 0;
    dlg = CreateWindowExW(WS_EX_DLGMODALFRAME, L"OSDlgPick", title,
                          WS_POPUP | WS_CAPTION | WS_SYSMENU,
                          CW_USEDEFAULT, CW_USEDEFAULT, 460, 400, owner, NULL, g_app.hInst, pr);
    if (!dlg) return -1;
    run_modal(dlg, owner);
    return (pr->count > 0) ? 0 : -1;
}

static int run_pick_dialog(HWND owner, PickResult* pr)
{
    return run_pick_dialog_titled(owner, pr, L"添加变量（模糊搜索·支持多选）");
}

/* N13a: 多选版本：成功返回 0，out_ids 写入全部选中叶变量 id，out_count 为个数；取消返回 -1 */
int os_dlg_pick_vars(HWND owner, int* out_ids, int max_out, int* out_count)
{
    PickResult pr;
    int i, n;
    if (g_app.leaf_count <= 0) {
        MessageBoxW(owner, L"请先加载 ELF 文件", L"选择变量", MB_OK | MB_ICONINFORMATION);
        return -1;
    }
    if (run_pick_dialog(owner, &pr) != 0) return -1;
    n = pr.count;
    if (n > max_out) n = max_out;
    for (i = 0; i < n; i++) out_ids[i] = pr.ids[i];
    if (out_count) *out_count = n;
    return 0;
}

/* Ctrl+H 快速搜索：模糊搜索弹窗 → 定位选中变量到左侧树（展开祖先/滚动/多选） */
static void tree_find_vars(HWND hwnd)
{
    PickResult pr;
    int ids[512], n = 0, i, ok;
    if (g_app.leaf_count <= 0) {
        MessageBoxW(hwnd, L"请先加载 ELF 文件", L"搜索变量", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (run_pick_dialog_titled(hwnd, &pr, L"搜索变量（模糊搜索·Ctrl+F）") != 0) return;
    n = pr.count;
    if (n > 512) n = 512;
    for (i = 0; i < n; i++) ids[i] = pr.ids[i];
    /* 树若已收起则展开，确保定位结果可见 */
    if (g_app.tree_hidden) tree_auto_expand(L"搜索定位");
    ok = os_vartree_locate_ids(g_app.hTree, ids, n);
    os_log(OS_LOG_INFO, "搜索定位变量: 选中 %d 个，定位 %d 个", n, ok);
}

int os_dlg_pick_var(HWND owner, int* out_leaf_id)
{
    int ids[1], n = 0;
    if (os_dlg_pick_vars(owner, ids, 1, &n) != 0) return -1;
    if (n > 0) {
        if (out_leaf_id) *out_leaf_id = ids[0];
        return 0;
    }
    return -1;
}

int os_dlg_edit_value(HWND owner, int leaf_id)
{
    const OS_Leaf* L = os_vartree_leaf(leaf_id);
    HWND dlg;
    wchar_t wname[420], wcur[80];
    wchar_t text_out[512];
    int res = 0;
    if (!L) return -1;
    dlg = CreateWindowExW(WS_EX_DLGMODALFRAME, L"OSDlgEdit", L"写入变量值",
                          WS_POPUP | WS_CAPTION | WS_SYSMENU,
                          CW_USEDEFAULT, CW_USEDEFAULT, 440, 160, owner, NULL, g_app.hInst, &res);
    if (!dlg) return -1;
    os_utf8_to_wide_buf(L->name, wname, 420);
    os_utf8_to_wide_buf(L->sample.text, wcur, 80);
    {
        EditState* es = (EditState*)calloc(1, sizeof(EditState));
        if (es) {
            es->result = &res;
            es->text_out = text_out;
            SetWindowLongPtrW(dlg, GWLP_USERDATA, (LONG_PTR)es);
        }
    }
    CreateWindowW(L"EDIT", wcur, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                  10, 46, 420, 26, dlg, (HMENU)IDD_EDIT_VALUE, g_app.hInst, NULL);
    CreateWindowW(L"BUTTON", L"确定", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                  300, 88, 60, 26, dlg, (HMENU)IDD_EDIT_OK, g_app.hInst, NULL);
    CreateWindowW(L"BUTTON", L"取消", WS_CHILD | WS_VISIBLE,
                  370, 88, 60, 26, dlg, (HMENU)IDD_EDIT_CANCEL, g_app.hInst, NULL);
    {
        wchar_t wlabel[520];
        _snwprintf(wlabel, 520, L"%s  @0x%llX   当前: %s",
                   wname, (unsigned long long)L->address, wcur[0] ? wcur : L"(未读取)");
        CreateWindowW(L"STATIC", wlabel, WS_CHILD | WS_VISIBLE,
                      10, 12, 420, 26, dlg, NULL, g_app.hInst, NULL);
    }
    run_modal(dlg, owner);
    if (res == 1) {
        char text[512];
        char err[256];
        WideCharToMultiByte(CP_UTF8, 0, text_out, -1, text, sizeof(text), NULL, NULL);
        if (os_ds_write_leaf(leaf_id, text, err, sizeof(err)) != 0) {
            wchar_t werr[300];
            os_utf8_to_wide_buf(err, werr, 300);
            MessageBoxW(owner, werr, L"写入失败", MB_OK | MB_ICONERROR);
            return -1;
        }
        return 0;
    }
    return -1;
}

static LRESULT CALLBACK edit_proc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_ERASEBKGND: { /* F20 */
        RECT rc;
        HDC hdc = (HDC)wParam;
        GetClientRect(dlg, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_BG));
        return 1;
    }
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
        return theme_ctlcolor((HDC)wParam, 0);
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
        return theme_ctlcolor((HDC)wParam, 1);
    case WM_DESTROY: {
        EditState* st = (EditState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
        free(st);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_EDIT_OK:
        {
            EditState* st = (EditState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            if (st) {
                if (st->result) *st->result = 1;
                if (st->text_out)
                    GetWindowTextW(GetDlgItem(dlg, IDD_EDIT_VALUE), st->text_out, 512);
            }
            DestroyWindow(dlg);
            return 0;
        }
        case IDD_EDIT_CANCEL:
        {
            EditState* st = (EditState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            if (st && st->result) *st->result = 0;
            DestroyWindow(dlg);
            return 0;
        }
        }
        break;
    }
    return DefWindowProcW(dlg, msg, wParam, lParam);
}

/* ---------- 框架回调 ---------- */

int os_fw_pick_variable(HWND parent, char* out, int out_len)
{
    int id = -1;
    const OS_Leaf* L;
    if (os_dlg_pick_var(parent, &id) != 0) return 0;
    L = os_vartree_leaf(id);
    if (!L) return 0;
    _snprintf(out, out_len, "%s", L->name);
    return 1;
}

int os_fw_write_leaf(int id, double value, char* err, int err_len)
{
    char text[64];
    _snprintf(text, 64, "%.17g", value);
    return os_ds_write_leaf(id, text, err, err_len);
}

void os_fw_on_elf_reloaded(void)
{
    os_mainwin_reload_elf();
}

/* ---------- 主窗口 ---------- */

/* Bug4: 收集全部选中叶变量 id（Ctrl 多选）。首个选中项用 TVGN_NEXTSELECTED + NULL。 */
static int tree_selected_ids(int* out, int max)
{
    HTREEITEM h = TreeView_GetNextItem(g_app.hTree, NULL, TVGN_NEXTSELECTED);
    int n = 0;
    while (h && n < max) {
        TVITEMW item;
        memset(&item, 0, sizeof(item));
        item.mask = TVIF_PARAM;
        item.hItem = h;
        if (TreeView_GetItem(g_app.hTree, &item) && item.lParam > 0)
            out[n++] = (int)(item.lParam - 1);
        h = TreeView_GetNextItem(g_app.hTree, h, TVGN_NEXTSELECTED);
    }
    return n;
}

/* Bug4 测试钩子辅助：深度优先收集全部叶子项（lParam>0）到 out，返回个数。
 * 跨进程无法用指针式 TVM_SETITEMSTATE/TVM_GETITEM 编组，故叶子枚举必须在进程内完成。 */
static int tree_collect_leaves(HTREEITEM h, HTREEITEM* out, int max)
{
    int n = 0;
    while (h && n < max) {
        TVITEMW it;
        HTREEITEM child;
        memset(&it, 0, sizeof(it));
        it.mask = TVIF_PARAM;
        it.hItem = h;
        if (TreeView_GetItem(g_app.hTree, &it) && it.lParam > 0)
            out[n++] = h;
        child = (HTREEITEM)SendMessageW(g_app.hTree, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)h);
        if (child) n += tree_collect_leaves(child, out + n, max - n);
        h = (HTREEITEM)SendMessageW(g_app.hTree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)h);
    }
    return n;
}

/* Bug4: 右键命中未选中项 -> 清空多选，单选该项（保持与单选树一致的手感） */
static void tree_context_select_hit(void)
{
    TVHITTESTINFO ht;
    POINT pt;
    memset(&ht, 0, sizeof(ht));
    GetCursorPos(&pt);
    ScreenToClient(g_app.hTree, &pt);
    ht.pt = pt;
    TreeView_HitTest(g_app.hTree, &ht);
    if (ht.hItem && !(TreeView_GetItemState(g_app.hTree, ht.hItem, TVIS_SELECTED) & TVIS_SELECTED))
        TreeView_SelectItem(g_app.hTree, ht.hItem);
}

static void tree_context_menu(HWND hwnd, LPARAM lParam)
{
    HTREEITEM h;
    LPARAM lp = -1;
    int sel_count;
    HMENU m;
    POINT pt;
    (void)hwnd;
    (void)lParam;
    tree_context_select_hit();
    h = TreeView_GetSelection(g_app.hTree);
    if (h) {
        TVITEMW item;
        memset(&item, 0, sizeof(item));
        item.mask = TVIF_PARAM;
        item.hItem = h;
        if (TreeView_GetItem(g_app.hTree, &item)) lp = item.lParam;
    }
    {
        int tmp[1];
        sel_count = tree_selected_ids(tmp, 1);
    }
    m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, IDM_TREE_ALL, L"全选观测");
    AppendMenuW(m, MF_STRING, IDM_TREE_NONE, L"清除全部观测");
    AppendMenuW(m, MF_STRING | (sel_count == 1 && lp > 0 ? 0 : MF_GRAYED), IDM_TREE_WRITE, L"写入值...");
    AppendMenuW(m, MF_SEPARATOR, 0, NULL);
    AppendMenuW(m, MF_STRING | (sel_count <= 0 ? MF_GRAYED : 0), IDM_TREE_ADD_CHART, L"添加到波形窗口");
    AppendMenuW(m, MF_STRING | (sel_count <= 0 ? MF_GRAYED : 0), IDM_TREE_ADD_NUM, L"添加到数值窗口");
    AppendMenuW(m, MF_SEPARATOR, 0, NULL);
    AppendMenuW(m, MF_STRING, IDM_TREE_RELOAD, L"重新加载 ELF");
    GetCursorPos(&pt);
    TrackPopupMenu(m, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_app.hMain, NULL);
    DestroyMenu(m);
}

static HWND active_win_hwnd(void)
{
    if (g_cur_tab >= 0 && g_cur_tab < g_app.win_count)
        return g_app.wins[g_cur_tab].hwnd;
    return NULL;
}

/* Bug4: 将叶 id 列表批量添加到原生波形/数值窗口（chart=1 波形，0 数值）：
 * 优先当前激活窗口，否则已有同类型窗口，否则新建。树右键与搜索对话框共用。 */
static void add_ids_to_native(int chart, const int* ids, int n)
{
    int i;
    HWND w;
    if (!ids || n <= 0) return;
    w = active_win_hwnd();
    if (!w || g_app.wins[g_cur_tab].is_module ||
        (chart ? !os_chart_is(w) : !os_num_is(w))) {
        w = NULL;
        for (i = 0; i < g_app.win_count; i++) {
            OS_WinItem* wi = &g_app.wins[i];
            if (wi->is_module || !wi->hwnd) continue;
            if (chart ? os_chart_is(wi->hwnd) : os_num_is(wi->hwnd)) { w = wi->hwnd; break; }
        }
        if (!w) {
            cmd_add_window(chart ? 1 : 0);
            if (g_app.win_count > 0) w = g_app.wins[g_app.win_count - 1].hwnd;
        }
    }
    if (w) {
        for (i = 0; i < n; i++) {
            if (chart) os_chart_add_var(w, ids[i]);
            else os_num_add_var(w, ids[i]);
        }
    }
}

static void tree_add_to_native(int chart)
{
    int ids[512], n = 0;
    if ((n = tree_selected_ids(ids, 512)) <= 0) return;
    add_ids_to_native(chart, ids, n);
    os_log(OS_LOG_INFO, "树右键批量添加变量: %d 个", n);
}


static void tab_context_menu(void)
{
    POINT pt;
    int i, hit = -1;
    HMENU m;
    GetCursorPos(&pt);
    for (i = 0; i < g_app.win_count; i++) {
        RECT r;
        if (SendMessageW(g_app.hTab, TCM_GETITEMRECT, i, (LPARAM)&r)) {
            MapWindowPoints(g_app.hTab, NULL, (LPPOINT)&r, 2);
            if (PtInRect(&r, pt)) { hit = i; break; }
        }
    }
    if (hit < 0) { /* F16: tab 标签条空白处右键 -> 新建窗口 */
        new_win_context_menu();
        return;
    }
    g_cur_tab = hit;
    SendMessageW(g_app.hTab, TCM_SETCURSEL, hit, 0);
    show_tab(hit);
    m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, IDM_TAB_RENAME, L"重命名标签");
    AppendMenuW(m, MF_STRING, IDM_TAB_ADD_CHART, L"在当前标签添加波形窗口");
    AppendMenuW(m, MF_STRING, IDM_TAB_ADD_NUM, L"在当前标签添加数值窗口");
    if (g_app.wins[hit].group_count > 1) {
        AppendMenuW(m, MF_STRING, IDM_TAB_MAXIMIZE, L"最大化/还原当前窗口");
        AppendMenuW(m, MF_STRING, IDM_TAB_MINIMIZE, L"最小化/还原当前窗口");
    }
    AppendMenuW(m, MF_STRING, IDM_TAB_FULLSCREEN, L"全屏/退出全屏");
    AppendMenuW(m, MF_SEPARATOR, 0, NULL);
    AppendMenuW(m, MF_STRING, IDM_TAB_CLOSE, L"关闭窗口");
    TrackPopupMenu(m, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_app.hMain, NULL);
    DestroyMenu(m);
}

/* ---------- Tab 就地重命名（N3：不弹窗，直接在标签上编辑） ---------- */

static HWND g_tab_edit = NULL;       /* 就地重命名 EDIT 控件 */
static int g_tab_edit_idx = -1;
static int g_tab_edit_closing = 0;   /* 防止 DestroyWindow 触发 WM_KILLFOCUS 二次提交 */

static void tab_rename_commit(void);
static void tab_rename_cancel(void);

/* 编辑框保持原生 EDIT 行为（文本显示/输入/选择）。回车/ESC 由主消息循环在分发前
   拦截（见 main.c），点击别处导致失焦则由定时器轮询提交——不做窗口过程子类化，
   避免覆盖 EditWndProc 破坏其内部文本缓冲。 */
HWND os_tab_edit_hwnd(void)
{
    return g_tab_edit;
}

void os_tab_edit_handle_key(WPARAM key)
{
    if (key == VK_RETURN) tab_rename_commit();
    else if (key == VK_ESCAPE) tab_rename_cancel();
}

/* 点击别处导致编辑框失焦 → 提交（由主窗口 WM_TIMER 3 周期调用） */
void os_tab_edit_focus_check(void)
{
    HWND f;
    if (!g_tab_edit || g_tab_edit_closing) return;
    f = GetFocus();
    if (!f || f == g_tab_edit) return;           /* 无焦点或仍在编辑框 → 保持 */
    if (GetAncestor(f, GA_ROOT) == g_app.hMain)
        tab_rename_commit();                      /* 焦点转到本应用其他窗口 → 提交 */
    /* 焦点在本应用之外（其他程序/桌面）→ 保留编辑框，避免误提交 */
}

static void tab_set_title(int idx, const wchar_t* name)
{
    TCITEMW ti;
    char name8[300];
    if (idx < 0 || idx >= g_app.win_count || !name || !name[0]) return;
    _snwprintf(g_app.wins[idx].title, 128, L"%s", name);
    memset(&ti, 0, sizeof(ti));
    ti.mask = TCIF_TEXT;
    ti.pszText = g_app.wins[idx].title;
    SendMessageW(g_app.hTab, TCM_SETITEM, idx, (LPARAM)&ti);
    os_wide_to_utf8_buf(name, name8, sizeof(name8));
    os_log(OS_LOG_INFO, "标签已重命名: %s", name8);
}

static void tab_rename_commit(void)
{
    HWND e = g_tab_edit;
    wchar_t text[160];
    wchar_t* p, * q;
    int idx = g_tab_edit_idx;
    if (!e || idx < 0 || idx >= g_app.win_count) return;
    g_tab_edit_closing = 1;
    GetWindowTextW(e, text, 160);
    /* 去首尾空白；空名 → 默认名 Default */
    for (p = text; *p == L' ' || *p == L'\t'; p++) ;
    q = p + wcslen(p);
    while (q > p && (q[-1] == L' ' || q[-1] == L'\t')) q--;
    *q = 0;
    if (!p[0]) _snwprintf(text, 160, L"Default");
    else _snwprintf(text, 160, L"%s", p);
    g_tab_edit = NULL;
    g_tab_edit_idx = -1;
    DestroyWindow(e);
    tab_set_title(idx, text);
    g_tab_edit_closing = 0;
}

static void tab_rename_cancel(void)
{
    HWND e = g_tab_edit;
    g_tab_edit_closing = 1;
    g_tab_edit = NULL;
    g_tab_edit_idx = -1;
    if (e) DestroyWindow(e);
    g_tab_edit_closing = 0;
}

static void tab_rename(int idx)
{
    RECT r;
    HWND e;
    if (idx < 0 || idx >= g_app.win_count) return;
    tab_rename_cancel(); /* 若正在编辑其他标签，先取消 */
    if (!SendMessageW(g_app.hTab, TCM_GETITEMRECT, idx, (LPARAM)&r)) return;
    e = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT,
                        r.left + 2, r.top + 2, (r.right - r.left) - 4, (r.bottom - r.top) - 2,
                        g_app.hTab, NULL, g_app.hInst, NULL);
    if (!e) return;
    SetWindowTextW(e, g_app.wins[idx].title);
    SendMessageW(e, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    SendMessageW(e, EM_SETSEL, 0, -1); /* 全选，便于直接输入替换 */
    { /* N3: 记录就地编辑初始文本（供 UI 回归用，跨进程 WM_GETTEXT 对子控件不可靠） */
        char title8[300];
        os_wide_to_utf8_buf(g_app.wins[idx].title, title8, sizeof(title8));
        os_log(OS_LOG_INFO, "就地编辑开始: %s", title8);
    }
    g_tab_edit = e;
    g_tab_edit_idx = idx;
    SetFocus(e);
}

static void tab_rename_hit(void)
{
    POINT pt;
    int i;
    GetCursorPos(&pt);
    for (i = 0; i < g_app.win_count; i++) {
        RECT r;
        if (SendMessageW(g_app.hTab, TCM_GETITEMRECT, i, (LPARAM)&r)) {
            MapWindowPoints(g_app.hTab, NULL, (LPPOINT)&r, 2);
            if (PtInRect(&r, pt)) {
                tab_rename(i);
                return;
            }
        }
    }
}

static void tree_select_all(int on)
{
    int i;
    for (i = 0; i < g_app.leaf_count; i++) {
        os_vartree_set_watch(i, on);
    }
    os_vartree_fill_tree(g_app.hTree);
    refresh_status();
    os_mainwin_update_buttons();
}

/* ---------- F20 主题应用 ---------- */

/* WM_CTLCOLOR* 统一返回主题画刷（edit=输入框/列表，否则按钮/静态文本） */
static LRESULT theme_ctlcolor(HDC hdc, int edit)
{
    if (edit) {
        SetTextColor(hdc, os_theme(TH_EDIT_TEXT));
        SetBkColor(hdc, os_theme(TH_EDIT_BG));
        return (LRESULT)os_theme_brush(TH_EDIT_BG);
    }
    SetTextColor(hdc, os_theme(TH_TEXT));
    SetBkColor(hdc, os_theme(TH_BG));
    return (LRESULT)os_theme_brush(TH_BG);
}

/* ---------- F20: 状态栏与 tab 控件完整自绘（标准消息无法可靠改色，子类化接管） ---------- */

#define STATUS_MAX_PARTS 8
static int     g_status_parts[STATUS_MAX_PARTS];
static int     g_status_nparts;
static wchar_t g_status_text[STATUS_MAX_PARTS][256];
static WNDPROC g_status_oldproc;
static WNDPROC g_tab_oldproc;

/* SB_SETTEXTW(=0x040B) 走子类拦截：存下文本，随后 WM_PAINT 用主题色自绘 */
static void status_capture_text(int part, const wchar_t* text)
{
    if (part < 0 || part >= STATUS_MAX_PARTS) return;
    if (!text) text = L"";
    _snwprintf(g_status_text[part], 256, L"%ls", text);
}

static void status_capture_parts(int n, const int* parts)
{
    int i;
    if (n > STATUS_MAX_PARTS) n = STATUS_MAX_PARTS;
    g_status_nparts = n;
    for (i = 0; i < n; i++) g_status_parts[i] = parts[i];
}

static LRESULT CALLBACK status_theme_proc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_ERASEBKGND:
        return 1; /* 背景交给 WM_PAINT 统一绘制 */
    case SB_SETPARTS:
        status_capture_parts((int)wParam, (const int*)lParam);
        break;
    case SB_SETTEXTW: /* Unicode 构建下 SB_SETTEXT 同值，仅列一个避免 C2196 */
        status_capture_text((int)wParam, (const wchar_t*)lParam);
        InvalidateRect(h, NULL, TRUE);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc;
        RECT rc;
        int i, x;
        hdc = BeginPaint(h, &ps);
        GetClientRect(h, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_STATUS_BG));
        /* 顶部边框线（与分隔条同色，浅色主题下与原视觉一致） */
        {
            HPEN pen = CreatePen(PS_SOLID, 1, os_theme(TH_BORDER));
            HGDIOBJ op = SelectObject(hdc, pen);
            MoveToEx(hdc, rc.left, rc.top, NULL);
            LineTo(hdc, rc.right, rc.top);
            SelectObject(hdc, op);
            DeleteObject(pen);
        }
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, os_theme(TH_STATUS_TEXT));
        SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
        x = 0;
        for (i = 0; i < g_status_nparts; i++) {
            RECT tr;
            int end = g_status_parts[i];
            if (end < 0 || end > rc.right) end = rc.right;
            if (end <= x) break;
            tr.left = x + 4; tr.top = rc.top + 1;
            tr.right = end - 2; tr.bottom = rc.bottom - 1;
            DrawTextW(hdc, g_status_text[i], -1, &tr,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            x = end;
        }
        EndPaint(h, &ps);
        return 0;
    }
    }
    return CallWindowProcW(g_status_oldproc, h, msg, wParam, lParam);
}

/* ---------- F20: 组合框完整自绘（CBS_DROPDOWNLIST 闭合面不受 SetWindowTheme 影响） ---------- */

static HWND    g_combo_hwnd[8];
static WNDPROC g_combo_old[8];
static int     g_combo_count;

static int combo_index(HWND h)
{
    int i;
    for (i = 0; i < g_combo_count; i++)
        if (g_combo_hwnd[i] == h) return i;
    return -1;
}

static void combo_draw_arrow(HDC hdc, RECT* rc)
{
    int cx = (rc->left + rc->right) / 2;
    int cy = (rc->top + rc->bottom) / 2;
    int i;
    for (i = 0; i < 4; i++) {
        MoveToEx(hdc, cx - 4 - i / 2, cy - 1 + i, NULL);
        LineTo(hdc, cx + 4 + i / 2, cy - 1 + i);
    }
}

static LRESULT CALLBACK combo_theme_proc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc;
        RECT rc, tr, ar;
        wchar_t txt[256];
        int n = 0;
        txt[0] = 0;
        hdc = BeginPaint(h, &ps);
        GetClientRect(h, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_EDIT_BG));
        {
            HPEN pen = CreatePen(PS_SOLID, 1, os_theme(TH_BORDER));
            HGDIOBJ op = SelectObject(hdc, pen);
            HBRUSH ob = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, rc.left, rc.top, rc.right - 1, rc.bottom - 1);
            SelectObject(hdc, ob);
            SelectObject(hdc, op);
            DeleteObject(pen);
        }
        /* 选中项文本：DROPDOWNLIST 用 CB_GETCURSEL+CB_GETLBTEXT；DROPDOWN 用窗口文本 */
        {
            LRESULT sel = SendMessageW(h, CB_GETCURSEL, 0, 0);
            if (sel != CB_ERR) {
                n = (int)SendMessageW(h, CB_GETLBTEXTLEN, sel, 0);
                if (n > 0 && n < 256)
                    SendMessageW(h, CB_GETLBTEXT, sel, (LPARAM)txt);
            } else {
                GetWindowTextW(h, txt, 256);
                n = (int)wcslen(txt);
            }
        }
        tr = rc;
        tr.left += 3; tr.top += 1; tr.right -= 22; tr.bottom -= 1;
        if (n > 0) {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, os_theme(TH_EDIT_TEXT));
            SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
            DrawTextW(hdc, txt, -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
        /* 下拉箭头 */
        ar.left = rc.right - 18; ar.top = rc.top; ar.right = rc.right - 2; ar.bottom = rc.bottom;
        {
            HPEN pen = CreatePen(PS_SOLID, 1, os_theme(TH_EDIT_TEXT));
            HGDIOBJ op = SelectObject(hdc, pen);
            combo_draw_arrow(hdc, &ar);
            SelectObject(hdc, op);
            DeleteObject(pen);
        }
        EndPaint(h, &ps);
        return 0;
    }
    }
    {
        int i = combo_index(h);
        if (i >= 0) return CallWindowProcW(g_combo_old[i], h, msg, wParam, lParam);
    }
    return DefWindowProcW(h, msg, wParam, lParam);
}

static void combo_install_theme(HWND h)
{
    if (g_combo_count < 8) {
        g_combo_hwnd[g_combo_count] = h;
        g_combo_old[g_combo_count] = (WNDPROC)SetWindowLongPtrW(h, GWLP_WNDPROC,
                                                                (LONG_PTR)combo_theme_proc);
        g_combo_count++;
    }
}

/* F20: 标准控件（按钮/组合框/编辑框/tab）暗色化。
 * SetPreferredAppMode(FORCE) 后，控件仍需 SetWindowTheme("DarkMode_Explorer")
 * 才按暗色主题渲染；切回浅色时用 "Explorer" 还原。动态加载避免改链接。 */
static void ctrl_dark_theme(HWND h)
{
    static HRESULT (WINAPI* fn)(HWND, const wchar_t*, const wchar_t*);
    static int inited;
    const wchar_t* name;
    if (!inited) {
        HMODULE ux = LoadLibraryW(L"uxtheme.dll");
        if (ux) fn = (HRESULT (WINAPI*)(HWND, const wchar_t*, const wchar_t*))
                       GetProcAddress(ux, "SetWindowTheme");
        inited = 1;
    }
    if (!fn) return;
    name = os_theme_dark() ? L"DarkMode_Explorer" : L"Explorer";
    fn(h, name, NULL);
    SendMessageW(h, WM_THEMECHANGED, 0, 0);
    RedrawWindow(h, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
}

static void apply_dark_theme_controls(HWND root)
{
    HWND child = GetWindow(root, GW_CHILD);
    while (child) {
        wchar_t cls[64];
        GetClassNameW(child, cls, 64);
        if (wcscmp(cls, L"Button") == 0 || wcscmp(cls, L"ComboBox") == 0 ||
            wcscmp(cls, L"Edit") == 0 || wcscmp(cls, L"SysTabControl32") == 0)
            ctrl_dark_theme(child);
        child = GetWindow(child, GW_HWNDNEXT);
    }
}

/* tab 控件：背景（tab 项以外区域）用主题色填充；tab 项本身由 uxtheme 暗色渲染。
 * Bug6: 平铺列间分隔带拖拽调列宽（IDC_SIZEWE 光标 + 实时重排），
 *       tab 底部最小化条自绘（点击按钮还原窗口）。 */
static int    g_tile_drag_on;                    /* 分隔带拖拽进行中 */
static int    g_tile_drag_gap;                   /* 拖拽的分隔带下标 */
static int    g_tile_drag_x0;                    /* 拖拽起点 x（tab 客户区） */
static double g_tile_drag_ratios[OS_MAX_GROUP];  /* 拖拽起点比例快照 */

static int tab_gap_hit(int x, int y)
{
    int g;
    for (g = 0; g < g_tile_gap_count; g++) {
        if (y >= g_tile_gaps[g].top && y < g_tile_gaps[g].bottom &&
            x >= g_tile_gaps[g].left && x < g_tile_gaps[g].right)
            return g;
    }
    return -1;
}

static int tab_minbar_hit(int x, int y)
{
    int b;
    for (b = 0; b < g_minbar_count; b++) {
        if (x >= g_minbar_btns[b].left && x < g_minbar_btns[b].right &&
            y >= g_minbar_btns[b].top && y < g_minbar_btns[b].bottom)
            return b;
    }
    return -1;
}

static void tab_minbar_paint(HWND h)
{
    /* 最小化条：每个最小化窗口一个按钮（标题 + 点击还原） */
    HDC hdc;
    int b;
    RECT bar;
    if (g_minbar_count <= 0 || g_cur_tab < 0 || g_cur_tab >= g_app.win_count) return;
    hdc = GetDC(h);
    if (!hdc) return;
    bar = g_minbar_btns[0];
    bar.left = 0;
    {
        RECT rc;
        GetClientRect(h, &rc);
        bar.right = rc.right;
        bar.top = g_minbar_btns[0].top - 3;
        bar.bottom = rc.bottom;
    }
    FillRect(hdc, &bar, os_theme_brush(TH_PANEL));
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, os_theme(TH_TAB_TEXT));
    SelectObject(hdc, (HFONT)SendMessageW(h, WM_GETFONT, 0, 0));
    for (b = 0; b < g_minbar_count; b++) {
        OS_WinItem* wi = &g_app.wins[g_cur_tab];
        int gi = g_minbar_btn_idx[b];
        RECT* r = &g_minbar_btns[b];
        HBRUSH frame = os_theme_brush(TH_BORDER);
        /* 按钮面 + 边框 + 标题（▢ 还原提示） */
        FillRect(hdc, r, os_theme_brush(TH_EDIT_BG));
        FrameRect(hdc, r, frame);
        if (gi >= 0 && gi < wi->group_count) {
            wchar_t cap[160];
            RECT tr = *r;
            tr.left += 6;
            tr.right -= 4;
            _snwprintf(cap, 160, L"▢ %ls", wi->group_title[gi]);
            DrawTextW(hdc, cap, -1, &tr,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
    }
    ReleaseDC(h, hdc);
}

static LRESULT CALLBACK tab_theme_proc(HWND h, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_SETCURSOR:
        /* Bug6: 悬停分隔带显示横向调整光标 */
        if (LOWORD(lParam) == HTCLIENT) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(h, &pt);
            if (tab_gap_hit(pt.x, pt.y) >= 0) {
                SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                return TRUE;
            }
        }
        break;
    case WM_LBUTTONDOWN: {
        int x = (int)(short)LOWORD(lParam), y = (int)(short)HIWORD(lParam);
        int b = tab_minbar_hit(x, y);
        int g;
        if (b >= 0 && g_cur_tab >= 0 && g_cur_tab < g_app.win_count) {
            /* Bug6: 点击最小化条按钮 -> 还原该窗口 */
            OS_WinItem* wi = &g_app.wins[g_cur_tab];
            int gi = g_minbar_btn_idx[b];
            wi->group_min[gi] = 0;
            os_log(OS_LOG_INFO, "窗口还原: tab%d %d/%d", g_cur_tab, gi, wi->group_count);
            layout_tab_pages();
            InvalidateRect(h, NULL, TRUE);
            return 0;
        }
        g = tab_gap_hit(x, y);
        if (g >= 0 && g_cur_tab >= 0 && g_cur_tab < g_app.win_count) {
            /* Bug6: 开始分隔带拖拽（快照起点比例，移动中相对起点增量应用） */
            OS_WinItem* wi = &g_app.wins[g_cur_tab];
            int k;
            g_tile_drag_on = 1;
            g_tile_drag_gap = g;
            g_tile_drag_x0 = x;
            for (k = 0; k < wi->group_count; k++)
                g_tile_drag_ratios[k] = wi->col_ratio[k];
            SetCapture(h);
            SetCursor(LoadCursor(NULL, IDC_SIZEWE));
            return 0;
        }
        break;
    }
    case WM_MOUSEMOVE:
        /* Bug6: 拖拽中仅响应按住左键的移动——SetCapture 期间窗口尺寸变化会让
         * 静止的物理光标收到 wParam=0 的 WM_MOUSEMOVE（命中测试重评估），
         * 不加过滤会把物理光标位置误当拖拽位移，列宽跳变 */
        if (g_tile_drag_on && (wParam & MK_LBUTTON) &&
            g_cur_tab >= 0 && g_cur_tab < g_app.win_count) {
            int x = (int)(short)LOWORD(lParam);
            OS_WinItem* wi = &g_app.wins[g_cur_tab];
            int a = g_tile_gap_a[g_tile_drag_gap];
            int b = g_tile_gap_b[g_tile_drag_gap];
            RECT rc, pr;
            int total, k;
            int tiled = 0;
            for (k = 0; k < wi->group_count; k++)
                if (!wi->group_min[k]) tiled++;
            /* 可用总宽 = 页区域宽 - 分隔带占用（与 layout_tab_pages 一致） */
            GetClientRect(h, &rc);
            pr = rc;
            SendMessageW(h, TCM_ADJUSTRECT, FALSE, (LPARAM)&pr);
            total = (pr.right - pr.left) - OS_TILE_GAP * (tiled - 1);
            /* 从拖拽起点快照出发，相对总位移应用（避免增量累积误差） */
            for (k = 0; k < wi->group_count; k++) wi->col_ratio[k] = g_tile_drag_ratios[k];
            if (os_tile_ratios_drag2(wi->col_ratio, a, b, total, x - g_tile_drag_x0, 60))
                layout_tab_pages();
            return 0;
        }
        break;
    case WM_LBUTTONUP:
        if (g_tile_drag_on) {
            int a, b;
            OS_WinItem* wi;
            g_tile_drag_on = 0;
            ReleaseCapture();
            if (g_cur_tab < 0 || g_cur_tab >= g_app.win_count) return 0;
            wi = &g_app.wins[g_cur_tab];
            a = g_tile_gap_a[g_tile_drag_gap];
            b = g_tile_gap_b[g_tile_drag_gap];
            os_log(OS_LOG_INFO, "列宽调整: tab%d 列%d/列%d 比例 %.2f/%.2f",
                   g_cur_tab, a, b, wi->col_ratio[a], wi->col_ratio[b]);
            return 0;
        }
        break;
    case WM_ERASEBKGND: {
        RECT rc;
        GetClientRect(h, &rc);
        FillRect((HDC)wParam, &rc, os_theme_brush(TH_TAB_BG));
        return 1;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, os_theme(TH_TAB_TEXT));
        SetBkColor(hdc, os_theme(TH_TAB_BG));
        return (LRESULT)os_theme_brush(TH_TAB_BG);
    }
    case WM_PAINT: {
        LRESULT r = CallWindowProcW(g_tab_oldproc, h, msg, wParam, lParam);
        /* 旧 proc 绘完 tab 项后，把“页区域”（tab 项下方空区）补成主题色 */
        if (g_app.win_count == 0) { /* 有窗口时页区域被子窗口覆盖，无需补色 */
            HDC hdc = GetDC(h);
            RECT rc, strip;
            GetClientRect(h, &rc);
            strip = rc;
            if (TabCtrl_GetItemCount(h) > 0) {
                RECT ir;
                if (TabCtrl_GetItemRect(h, TabCtrl_GetItemCount(h) - 1, &ir))
                    strip.top = ir.bottom + 2;
            } else {
                strip.top = 2;
            }
            if (strip.top < rc.bottom) {
                HBRUSH ob = (HBRUSH)SelectObject(hdc, os_theme_brush(TH_TAB_BG));
                PatBlt(hdc, rc.left, strip.top, rc.right - rc.left, rc.bottom - strip.top, PATCOPY);
                SelectObject(hdc, ob);
            }
            ReleaseDC(h, hdc);
        }
        tab_minbar_paint(h); /* Bug6: 底部最小化条（有最小化窗口时） */
        return r;
    }
    }
    return CallWindowProcW(g_tab_oldproc, h, msg, wParam, lParam);
}

/* 把当前主题刷到树/日志/状态栏/各窗口（theme.c 切换时也会调用） */
void os_mainwin_apply_theme(void)
{
    int i, k;
    if (g_app.hTree && IsWindow(g_app.hTree)) {
        SendMessageW(g_app.hTree, TVM_SETBKCOLOR, 0, os_theme(TH_TREE_BG));
        SendMessageW(g_app.hTree, TVM_SETTEXTCOLOR, 0, os_theme(TH_TREE_TEXT));
        SendMessageW(g_app.hTree, TVM_SETLINECOLOR, 0, os_theme(TH_TREE_LINE));
        InvalidateRect(g_app.hTree, NULL, TRUE);
    }
    if (g_app.hLog && IsWindow(g_app.hLog)) {
        ListView_SetBkColor(g_app.hLog, os_theme(TH_LOG_BG));
        ListView_SetTextColor(g_app.hLog, os_theme(TH_LOG_TEXT));
        ListView_SetTextBkColor(g_app.hLog, os_theme(TH_LOG_BG));
        InvalidateRect(g_app.hLog, NULL, TRUE);
        /* 列头（SysHeader32）SetWindowTheme 无效，改用共享自绘子类 */
        os_theme_listview_header(g_app.hLog);
    }
    if (g_app.hStatus && IsWindow(g_app.hStatus)) {
        /* 状态栏已子类化自绘（status_theme_proc），这里仅触发重绘 */
        InvalidateRect(g_app.hStatus, NULL, TRUE);
    }
    if (g_app.hLogStrip && IsWindow(g_app.hLogStrip)) /* F22: 底部细条随主题重绘 */
        InvalidateRect(g_app.hLogStrip, NULL, TRUE);
    for (i = 0; i < g_app.win_count; i++) {
        for (k = 0; k < g_app.wins[i].group_count; k++) {
            HWND w = g_app.wins[i].group[k];
            if (!w || !IsWindow(w)) continue;
            if (os_num_is(w)) os_num_apply_theme(w);
            else if (os_chart_is(w)) InvalidateRect(w, NULL, TRUE);
        }
    }
    if (g_app.hTab && IsWindow(g_app.hTab)) InvalidateRect(g_app.hTab, NULL, TRUE);
    if (g_app.hRight && IsWindow(g_app.hRight)) InvalidateRect(g_app.hRight, NULL, TRUE);
    /* 标准控件（按钮/组合框/编辑框/tab）暗色主题 */
    if (g_app.hMain && IsWindow(g_app.hMain)) {
        apply_dark_theme_controls(g_app.hMain);
        if (g_app.hRight && IsWindow(g_app.hRight))
            apply_dark_theme_controls(g_app.hRight);
    }
}

LRESULT CALLBACK os_mainwin_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE: {
        HMENU mFile, mAcq, mLog, mHelp;
        int i;
        g_app.hMain = hwnd;
        os_theme_set_main(hwnd);
        g_app.hBtnBar = hwnd;
        for (i = 0; i < (int)(sizeof(g_tool_btns) / sizeof(g_tool_btns[0])); i++) {
            HWND b = CreateWindowW(L"BUTTON", g_tool_btns[i].text,
                                   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                   6 + i * 84, 5, 80, 24, hwnd, (HMENU)(INT_PTR)g_tool_btns[i].id,
                                   g_app.hInst, NULL);
            SendMessageW(b, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        }
        /* 连接配置控件（MCU型号/接口/速度/J-Link设备/刷新，随工具栏一行布局） */
        {
            HWND c;
            c = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP,
                              584, 5, 140, 200, hwnd, (HMENU)IDC_CFG_DEVICE, g_app.hInst, NULL);
            SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
            combo_install_theme(c); /* F20: 闭合面自绘（暗色） */
            c = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP,
                              650, 5, 62, 120, hwnd, (HMENU)IDC_CFG_IFACE, g_app.hInst, NULL);
            SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
            combo_install_theme(c);
            c = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_TABSTOP,
                              716, 5, 92, 200, hwnd, (HMENU)IDC_CFG_SPEED, g_app.hInst, NULL);
            SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
            combo_install_theme(c);
            c = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP,
                              812, 5, 260, 120, hwnd, (HMENU)IDC_CFG_EMU, g_app.hInst, NULL);
            SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
            combo_install_theme(c);
            c = CreateWindowW(L"BUTTON", L"刷新", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                              1078, 5, 48, 24, hwnd, (HMENU)IDC_CFG_REFRESH, g_app.hInst, NULL);
            SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        }
        g_app.hTree = CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEWW, L"",
                                      WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS |
                                      TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_CHECKBOXES,
                                      0, 34, 340, 400, hwnd, NULL, g_app.hInst, NULL);
        SendMessageW(g_app.hTree, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        TreeView_SetUnicodeFormat(g_app.hTree, TRUE);
        /* Bug4: 树扩展样式开启多选（Ctrl 单击连续多选 / Shift 起止范围选）。
         * 注意 TreeView_SetExtendedStyle 签名是 (hwnd, mask, style)，mask 指定要
         * 修改的位，style 是目标值。旧代码 style=0 会把 TVS_EX_MULTISELECT 清为 0
         * （禁用多选），应设为 TVS_EX_MULTISELECT 才真正开启。 */
        TreeView_SetExtendedStyle(g_app.hTree, TVS_EX_MULTISELECT, TVS_EX_MULTISELECT);
        g_app.hSplitV = CreateWindowW(L"OSSplitter", L"", WS_CHILD | WS_VISIBLE,
                                      340, 34, 5, 400, hwnd, NULL, g_app.hInst, NULL);
        g_app.hRight = CreateWindowExW(WS_EX_CLIENTEDGE, g_right_class, L"",
                                      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                                     345, 34, 500, 400, hwnd, NULL, g_app.hInst, NULL);
        g_app.hTab = CreateWindowW(WC_TABCONTROLW, L"",
                                   WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_FIXEDWIDTH,
                                   0, 0, 100, 100, g_app.hRight, NULL, g_app.hInst, NULL);
        SendMessageW(g_app.hTab, TCM_SETITEMSIZE, 0, MAKELPARAM(150, 22));
        SendMessageW(g_app.hTab, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        /* F20: 子类化 tab 控件，背景/文字用主题色 */
        g_tab_oldproc = (WNDPROC)SetWindowLongPtrW(g_app.hTab, GWLP_WNDPROC,
                                                   (LONG_PTR)tab_theme_proc);
        g_app.hTreeStrip = CreateWindowW(L"OSTreeStrip", L"", WS_CHILD,
                                         0, 34, 8, 400, hwnd, NULL, g_app.hInst, NULL);
        g_app.hTreePin = CreateWindowW(L"OSTreePin", L"", WS_CHILD | WS_VISIBLE,
                                       0, 36, 30, 22, hwnd, NULL, g_app.hInst, NULL);
        g_app.hSplitH = CreateWindowW(L"OSSplitterH", L"", WS_CHILD | WS_VISIBLE,
                                      0, 500, 800, 5, hwnd, NULL, g_app.hInst, NULL);
        g_app.hLogStrip = CreateWindowW(L"OSLogStrip", L"", WS_CHILD, /* F22 */
                                        0, 500, 800, LOG_STRIP_H, hwnd, NULL, g_app.hInst, NULL);
        g_app.hLog = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
                                     WS_CHILD | WS_VISIBLE | LVS_REPORT,
                                     0, 505, 800, 165, hwnd, NULL, g_app.hInst, NULL);
        /* Bug13: 不设 LVS_SINGLESEL 即支持多选（Ctrl 连续选择 / Shift 起止范围选）+ 整行选中 */
        ListView_SetExtendedListViewStyle(g_app.hLog,
            LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
        {
            LVCOLUMNW col;
            memset(&col, 0, sizeof(col));
            col.mask = LVCF_TEXT | LVCF_WIDTH;
            col.cx = 90;
            col.pszText = L"时间";
            ListView_InsertColumn(g_app.hLog, 0, &col);
            col.cx = 50;
            col.pszText = L"级别";
            ListView_InsertColumn(g_app.hLog, 1, &col);
            col.cx = 640;
            col.pszText = L"消息";
            ListView_InsertColumn(g_app.hLog, 2, &col);
        }
        g_app.hStatus = CreateWindowW(STATUSCLASSNAMEW, L"", WS_CHILD | WS_VISIBLE,
                                      0, 0, 0, 0, hwnd, NULL, g_app.hInst, NULL);
        /* F20: 子类化状态栏，整体用主题色自绘（标准 SB_SET*COLOR 消息无效） */
        g_status_oldproc = (WNDPROC)SetWindowLongPtrW(g_app.hStatus, GWLP_WNDPROC,
                                                      (LONG_PTR)status_theme_proc);
        /* 菜单 */
        mFile = CreateMenu();
        mAcq = CreateMenu();
        mLog = CreateMenu();
        g_menu_win = CreateMenu();
        mHelp = CreateMenu();
        g_menu_file = mFile;
        AppendMenuW(mFile, MF_STRING, IDC_BTN_OPEN, L"打开 ELF 文件...\tCtrl+O");
        AppendMenuW(mFile, MF_STRING, IDM_LAYOUT_SAVE, L"保存布局为...");
        AppendMenuW(mFile, MF_STRING, IDM_LAYOUT_LOAD, L"加载布局...");
        AppendMenuW(mFile, MF_SEPARATOR, 0, NULL);
        /* F20: 界面风格设置——深色模式（勾选项，勾选状态 WM_INITMENU 同步） */
        AppendMenuW(mFile, MF_STRING, IDM_THEME_DARK, L"深色模式");
        AppendMenuW(mFile, MF_SEPARATOR, 0, NULL);
        AppendMenuW(mFile, MF_STRING, IDM_EXIT, L"退出\tAlt+F4");
        AppendMenuW(mAcq, MF_STRING, IDC_BTN_CONNECT, L"连接...\tF5");
        AppendMenuW(mAcq, MF_STRING, IDC_BTN_DISCON, L"断开\tF6");
        AppendMenuW(mAcq, MF_SEPARATOR, 0, NULL);
        AppendMenuW(mAcq, MF_STRING, IDC_BTN_START, L"开始采集\tF7");
        AppendMenuW(mAcq, MF_STRING, IDC_BTN_STOP, L"停止采集\tF8");
        AppendMenuW(mLog, MF_STRING, IDC_BTN_LOGSTART, L"开始 CSV 记录...");
        AppendMenuW(mLog, MF_STRING, IDC_BTN_LOGSTOP, L"停止记录");
        AppendMenuW(mLog, MF_SEPARATOR, 0, NULL);
        AppendMenuW(mLog, MF_STRING, IDC_BTN_REPLAY, L"离线回放...");
        AppendMenuW(mLog, MF_STRING, IDC_BTN_REPLAYSTOP, L"停止回放");
        AppendMenuW(mHelp, MF_STRING, IDM_HELP_DOC, L"帮助文档\tF1");
        AppendMenuW(mHelp, MF_STRING, IDC_BTN_ABOUT, L"关于 OpenScope");
        g_menu = CreateMenu();
        AppendMenuW(g_menu, MF_POPUP, (UINT_PTR)mFile, L"文件(&F)");
        AppendMenuW(g_menu, MF_POPUP, (UINT_PTR)mAcq, L"采集(&A)");
        AppendMenuW(g_menu, MF_POPUP, (UINT_PTR)mLog, L"记录/回放(&L)");
        AppendMenuW(g_menu, MF_POPUP, (UINT_PTR)g_menu_win, L"窗口(&W)");
        AppendMenuW(g_menu, MF_POPUP, (UINT_PTR)mHelp, L"帮助(&H)");
        SetMenu(hwnd, g_menu);
        os_mainwin_rebuild_window_menu();
        SetTimer(hwnd, 1, 2000, NULL);
        SetTimer(hwnd, 3, 200, NULL); /* N9(d): 变量栏自动隐藏轮询 */
        g_app.tree_auto = 0;          /* 默认钉住常显（不影响既有操作） */
        refresh_tree_pin();
        os_log_set(os_mainwin_append_log);
        os_log(OS_LOG_INFO, "OpenScope 已启动");
        refresh_status();
        os_mainwin_update_buttons();
        os_theme_apply(hwnd); /* F20: 主题（标题栏/树/日志/状态栏/控件色） */
        return 0;
    }
    case WM_SIZE:
        layout();
        return 0;
    case WM_ERASEBKGND: /* F20: 主窗口背景随主题（工具行间隙等） */
    {
        RECT rc;
        HDC hdc = (HDC)wParam;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, os_theme_brush(TH_BG));
        return 1;
    }
    case WM_INITMENU: /* F20: 菜单打开时同步深色模式勾选状态 */
        CheckMenuItem(g_menu_file, IDM_THEME_DARK,
                      MF_BYCOMMAND | (os_theme_dark() ? MF_CHECKED : MF_UNCHECKED));
        break;
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
        return theme_ctlcolor((HDC)wParam, 0);
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
        return theme_ctlcolor((HDC)wParam, 1);
    case WM_CLOSE:
        os_layout_save_auto(); /* 关闭时保存布局，便于下次恢复 */
        DestroyWindow(hwnd);
        return 0;
    case WM_OS_SPLIT: {
        /* 用户反馈优化：拖拽分隔条到最左（x<60）即完全隐藏变量栏——旧实现 120px
         * 下限夹紧，"拉不过去"；从隐藏态细条位置向右拖（x≥16）则展开并按拖拽
         * 位置设定宽度。宽度的可用下限同步放宽到 60px（窄树仍可用）。 */
        int x = (int)wParam;
        if (g_app.tree_hidden) {
            if (x >= 16) {
                g_app.tree_hidden = 0;
                if (x < 60) x = 60;
                if (x > 900) x = 900;
                g_app.tree_w = x;
                os_log(OS_LOG_INFO, "变量栏展开: 拖拽分隔条 (宽 %d)", x);
            }
        } else if (x < 60) {
            g_app.tree_hidden = 1;
            os_log(OS_LOG_INFO, "变量栏已完全隐藏: 拖拽分隔条到左侧");
        } else {
            if (x > 900) x = 900;
            if (x < 60) x = 60;
            g_app.tree_w = x;
        }
        layout();
        return 0;
    }
    case WM_OS_SPLIT_V: {
        /* F14: 消息栏上下拉伸：log_h = 主窗口高 - 34(工具行) - 27(状态栏+分隔条) - 分隔条 y */
        RECT rv;
        int y = (int)wParam;
        GetClientRect(hwnd, &rv);
        g_app.log_h = rv.bottom - 34 - 27 - y;
        if (g_app.log_h < 40) g_app.log_h = 40;
        if (g_app.log_h > rv.bottom - 120) g_app.log_h = rv.bottom - 120;
        layout();
        return 0;
    }
    case WM_OS_TREE_TEST_SELECT: {
        /* Bug4 测试钩子：程序化选中树文档序叶子项 [start, start+count)（等价 Ctrl 手选）。
         * 返回实际选中叶子数；调用方可用返回值和后续"批量添加"日志验证多选行为。 */
        int start = (int)wParam, cnt = (int)lParam, n, i, sel = 0;
        HTREEITEM items[1024];
        HTREEITEM root = (HTREEITEM)SendMessageW(g_app.hTree, TVM_GETNEXTITEM, TVGN_ROOT, 0);
        n = tree_collect_leaves(root, items, 1024);
        for (i = 0; i < n; i++) {
            TVITEMW it;
            memset(&it, 0, sizeof(it));
            it.mask = TVIF_STATE;
            it.stateMask = TVIS_SELECTED;
            it.hItem = items[i];
            it.state = (i >= start && i < start + cnt) ? TVIS_SELECTED : 0;
            TreeView_SetItem(g_app.hTree, &it);
            if (it.state) sel++;
        }
        return sel;
    }
    case WM_OS_LOG_TEST_SELECT: {
        /* Bug13 测试钩子：程序化选中日志 [start, start+count)（等价 Shift 起止范围选）。
         * 跨进程无法用指针式 LVM_SETITEMSTATE 编组（会读到发送方进程地址崩溃），
         * 故由主线程进程内设置，返回实际选中数。 */
        int start = (int)wParam, cnt = (int)lParam, n, i, sel = 0;
        LVITEMW it;
        n = g_app.hLog ? ListView_GetItemCount(g_app.hLog) : 0;
        memset(&it, 0, sizeof(it));
        it.mask = LVIF_STATE;
        it.stateMask = LVIS_SELECTED;
        for (i = 0; i < n; i++) {
            it.iItem = i;
            it.state = (i >= start && i < start + cnt) ? LVIS_SELECTED : 0;
            SendMessageW(g_app.hLog, LVM_SETITEMSTATE, (WPARAM)i, (LPARAM)&it);
            if (it.state) sel++;
        }
        return sel;
    }
    case WM_TIMER:
        if (wParam == 1) check_elf_mtime();
        else if (wParam == 2) os_replay_tick();
        else if (wParam == 3) { tree_auto_tick(); os_tab_edit_focus_check(); }
        return 0;
    /* Windows 将 WM_MOUSEWHEEL 发送给拥有键盘焦点的窗口，而非鼠标下方的窗口。
     * 当用户点击左侧变量树后，焦点在树——此后在波形绘图区滚轮缩放会被树吞掉。
     * 主窗口拦截 WM_MOUSEWHEEL，用 WindowFromPoint 找到光标下的真实子窗口
     * 并转发给它（chartwin 的 chart_proc 已实现完整的滚轮缩放/框选/拖拽逻辑）。 */
    case WM_MOUSEWHEEL: {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        HWND target = WindowFromPoint(pt);
        /* 仅当光标下的窗口是子控件（非主窗口自身）时转发；主窗口直接 break。 */
        if (target && target != hwnd && IsChild(hwnd, target))
            return SendMessageW(target, WM_MOUSEWHEEL, wParam, lParam);
        break; /* 主窗口自身 → 默认处理 */
    }
    case WM_OS_SAMPLES:
        os_ds_drain();
        refresh_status();
        return 0;
    case WM_OS_LOG: {
        /* Bug7: 非主线程日志在此插入 ListView（主线程上下文） */
        OS_LogMsg* m = (OS_LogMsg*)wParam;
        if (m) { log_insert_now(m->level, m->text); free(m); }
        return 0;
    }
    case WM_OS_ACQ_STATE:
        refresh_status();
        os_mainwin_update_buttons();
        return 0;
    case WM_OS_WIN_CLOSED: {
        HWND w = (HWND)wParam;
        int i, k, found = 0;
        if (w == g_app.fs_win) g_app.fs_win = NULL; /* 全屏窗口被关闭 */
        if (w == g_cur_win) g_cur_win = NULL;
        for (i = 0; i < g_app.win_count && !found; i++) {
            OS_WinItem* wi = &g_app.wins[i];
            for (k = 0; k < wi->group_count; k++) {
                if (wi->group[k] == w) {
                    /* 模块窗口销毁回调 */
                    if (wi->is_module && wi->mod && wi->mod->destroy_window)
                        wi->mod->destroy_window(wi->mod_ctx, w);
                    if (IsWindow(w)) DestroyWindow(w);
                    found = 1;
                    if (wi->group_count == 1) {
                        /* tab 内只剩这一个窗口 → 关闭整个 tab */
                        memmove(&g_app.wins[i], &g_app.wins[i + 1],
                                sizeof(OS_WinItem) * (g_app.win_count - i - 1));
                        g_app.win_count--;
                        if (g_cur_tab >= g_app.win_count) g_cur_tab = g_app.win_count - 1;
                    } else {
                        /* N11: 移除组内窗口；若关闭的是主窗口则提升组内下一个 */
                        int gi;
                        for (gi = k; gi < wi->group_count - 1; gi++) {
                            wi->group[gi] = wi->group[gi + 1];
                            _snwprintf(wi->group_title[gi], 128, L"%s", wi->group_title[gi + 1]);
                            wi->group_min[gi] = wi->group_min[gi + 1]; /* Bug6 */
                        }
                        wi->group_count--;
                        wi->group_min[wi->group_count] = 0;
                        os_tile_ratios_init(wi->col_ratio, wi->group_count); /* Bug6: 重新均分 */
                        if (k == 0) {
                            wi->hwnd = wi->group[0];
                            _snwprintf(wi->title, 128, L"%s", wi->group_title[0]);
                        }
                        if (wi->group_max >= wi->group_count) wi->group_max = -1;
                        if (wi->group_max == k) wi->group_max = -1;
                    }
                    break;
                }
            }
        }
        os_mainwin_tile();
        return 0;
    }
    case WM_OS_WIN_MAXIMIZE: {
        /* N11: 最大化/还原：wParam=HWND，所属 tab 内该窗口填满 tab（其余隐藏） */
        HWND w = (HWND)wParam;
        int i, k;
        for (i = 0; i < g_app.win_count; i++) {
            OS_WinItem* wi = &g_app.wins[i];
            for (k = 0; k < wi->group_count; k++) {
                if (wi->group[k] == w) {
                    if (wi->group_max == k) wi->group_max = -1;   /* 还原平铺 */
                    else wi->group_max = k;
                    os_log(OS_LOG_INFO, "窗口%s: tab%d %d/%d",
                           wi->group_max == k ? "最大化" : "还原", i, k, wi->group_count);
                    layout_tab_pages();
                    return 0;
                }
            }
        }
        return 0;
    }
    case WM_OS_WIN_ADD_VAR: {
        /* 测试钩子：wParam=目标窗口 HWND，lParam=叶 id，向指定窗口添加变量 */
        HWND w = (HWND)wParam;
        if (w && IsWindow(w)) {
            if (os_chart_is(w)) os_chart_add_var(w, (int)lParam);
            else if (os_num_is(w)) os_num_add_var(w, (int)lParam);
        }
        return 0;
    }
    case WM_OS_WIN_MINIMIZE: {
        /* Bug6: 最小化/还原：wParam=HWND，缩进 tab 底部最小化条（点击条上按钮还原） */
        HWND w = (HWND)wParam;
        int i, k;
        for (i = 0; i < g_app.win_count; i++) {
            OS_WinItem* wi = &g_app.wins[i];
            for (k = 0; k < wi->group_count; k++) {
                if (wi->group[k] == w) {
                    wi->group_min[k] = !wi->group_min[k];
                    if (wi->group_min[k] && wi->group_max == k) wi->group_max = -1;
                    os_log(OS_LOG_INFO, "窗口%s: tab%d %d/%d",
                           wi->group_min[k] ? "最小化" : "还原", i, k, wi->group_count);
                    layout_tab_pages();
                    InvalidateRect(g_app.hTab, NULL, TRUE);
                    return 0;
                }
            }
        }
        return 0;
    }
    case WM_OS_WIN_FULLSCREEN: {
        /* Bug3: 单窗口全屏/退出全屏。全屏时临时改父为主窗口并铺满整个客户区，
           退出时改回 tab 页并重新平铺。 */
        HWND w = (HWND)wParam;
        if (g_app.fs_win == w) {
            HWND old = g_app.fs_win;
            g_app.fs_win = NULL;
            if (old && IsWindow(old)) {
                SetParent(old, g_app.hTab);
                ShowWindow(old, SW_SHOW);
            }
            layout();
            os_log(OS_LOG_INFO, "窗口退出全屏");
        } else if (w && IsWindow(w)) {
            RECT rc;
            g_app.fs_win = w;
            SetParent(w, g_app.hMain);
            GetClientRect(g_app.hMain, &rc);
            MoveWindow(w, 0, 0, rc.right, rc.bottom, TRUE);
            BringWindowToTop(w);
            SetFocus(w);
            os_log(OS_LOG_INFO, "窗口全屏");
        }
        return 0;
    }
    case WM_OS_TREE_AUTOHIDE:
        /* N9(d) 测试钩子：wParam=1 开启自动隐藏，0=钉住 */
        g_app.tree_auto = wParam ? 1 : 0;
        refresh_tree_pin();
        layout();
        return 0;
    case WM_OS_LOG_HIDE:
        /* F22 测试钩子：wParam=1 收起消息栏，0=展开 */
        log_set_hidden(wParam ? 1 : 0);
        return 0;
    case WM_OS_TREE_HIDE:
        /* Bug18 测试钩子：wParam=1 变量栏完全隐藏，0=展开 */
        tree_set_hidden(wParam ? 1 : 0);
        return 0;
    case WM_NOTIFY: {
        LPNMHDR h = (LPNMHDR)lParam;
        if (h && h->hwndFrom == g_app.hTab) {
            if (h->code == TCN_SELCHANGE) {
                int sel = (int)SendMessageW(g_app.hTab, TCM_GETCURSEL, 0, 0);
                if (sel >= 0 && sel < g_app.win_count) {
                    g_cur_tab = sel;
                    show_tab(sel);
                }
                os_log(OS_LOG_DEBUG, "Tab 切换 -> %d", sel);
                return 0;
            }
            if (h->code == NM_RCLICK) {
                tab_context_menu();
                return 0;
            }
            if (h->code == NM_DBLCLK) {
                tab_rename_hit();
                return 0;
            }
        }
        if (h && h->hwndFrom == g_app.hLog) {
            /* Bug13: 消息区域右键菜单（复制选中 / 全部清除） */
            if (h->code == NM_RCLICK) {
                log_context_menu();
                return 0;
            }
        }
        if (h && h->hwndFrom == g_app.hTree) {
            if (h->code == NM_RCLICK) {
                tree_context_menu(hwnd, lParam);
                return 0;
            }
            if (h->code == TVN_ITEMCHANGEDW) {
                NMTVITEMCHANGE* p = (NMTVITEMCHANGE*)lParam;
                /* 需求2：树填充期间屏蔽勾选联动（重建时节点 lParam 是旧叶表 id，
                 * 回写会误置新叶表同名位置的观测标志）；仅响应勾选框位真实翻转，
                 * 选择状态变化不处理（uStateNew 含 TVIS_SELECTED 时图像位为 0） */
                if (!os_vartree_is_filling() && (p->uChanged & TVIF_STATE) &&
                    ((p->uStateOld ^ p->uStateNew) & TVIS_STATEIMAGEMASK)) {
                    LPARAM lp = p->lParam;
                    if (lp > 0) {
                        int id = (int)(lp - 1);
                        int checked = ((p->uStateNew & TVIS_STATEIMAGEMASK) >> 12) == 2;
                        os_vartree_set_watch(id, checked);
                        refresh_status();
                        os_mainwin_update_buttons();
                    }
                }
                return 0;
            }
        }
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BTN_OPEN: cmd_open_elf(); break;
        case IDC_BTN_CONNECT: cmd_connect(); break;
        case IDC_BTN_DISCON: cmd_disconnect(); break;
        case IDC_CFG_REFRESH:
            if (cfg_fill_emus() <= 0)
                MessageBoxW(hwnd, L"没有发现 JLink 设备，请确认仿真器已插入 USB 并点击「刷新」。",
                            L"J-Link", MB_OK | MB_ICONWARNING);
            break;
        case IDC_BTN_START: cmd_start_acq(); break;
        case IDC_BTN_STOP: cmd_stop_acq(); break;
        case IDC_BTN_LOGSTART: cmd_log_start(); break;
        case IDC_BTN_LOGSTOP: cmd_log_stop(); break;
        case IDC_BTN_REPLAY: cmd_replay_open(); break;
        case IDC_BTN_REPLAYSTOP: cmd_replay_stop(); break;
        case IDM_HELP_DOC: os_help_show(hwnd); break; /* 需求12：帮助文档（F1 同路径） */
        case IDC_BTN_ABOUT: {
            wchar_t about[640];
            _snwprintf(about, 640,
                       L"OpenScope v%s\n\n"
                       L"MCU 变量采集与标定工具（类 CANape）\n"
                       L"C + Win32 + 动态模块架构\n\n"
                       L"晶圆上的生物技术开发和提供支持\n"
                       L"网址: www.opendebugger.com",
                       OS_VERSION_WIDE);
            MessageBoxW(hwnd, about, L"关于", MB_OK | MB_ICONINFORMATION);
            break;
        }
        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;
        case IDM_LAYOUT_SAVE: {
            OPENFILENAMEW ofn;
            wchar_t file[MAX_PATH] = L"layout.ini";
            memset(&ofn, 0, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = L"布局文件\0*.ini;*.layout\0所有文件\0*.*\0";
            ofn.lpstrFile = file;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = L"ini";
            ofn.Flags = OFN_OVERWRITEPROMPT;
            if (GetSaveFileNameW(&ofn)) os_layout_save_to(file);
            break;
        }
        case IDM_LAYOUT_LOAD: {
            OPENFILENAMEW ofn;
            wchar_t file[MAX_PATH] = L"";
            memset(&ofn, 0, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = L"布局文件\0*.ini;*.layout\0所有文件\0*.*\0";
            ofn.lpstrFile = file;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            if (GetOpenFileNameW(&ofn)) os_layout_load_from(file);
            break;
        }
        case IDM_THEME_DARK: /* F20: 切换深色/白色主题并持久化 */
            os_theme_set_dark(!os_theme_dark());
            os_layout_save_auto(); /* theme 键随布局持久化 */
            os_log(OS_LOG_INFO, "界面主题: %s", os_theme_dark() ? "深色" : "白色");
            break;
        case IDM_WIN_CHART: cmd_add_window(1); break;
        case IDM_WIN_NUM: cmd_add_window(0); break;
        case IDM_TREE_ALL: tree_select_all(1); break;
        case IDM_TREE_NONE: tree_select_all(0); break;
        case IDM_TREE_RELOAD: os_mainwin_reload_elf(); break;
        case IDM_TREE_ADD_CHART: tree_add_to_native(1); break;
        case IDM_TREE_ADD_NUM: tree_add_to_native(0); break;
        case IDM_TREE_FIND: tree_find_vars(hwnd); break; /* Ctrl+H 快速搜索变量 */
        case IDM_LOG_COPY: log_copy_selected(); break;
        case IDM_LOG_CLEAR: log_clear_all(); break;
        case IDM_TAB_CLOSE:
            if (g_cur_tab >= 0 && g_cur_tab < g_app.win_count)
                PostMessage(hwnd, WM_OS_WIN_CLOSED,
                            (WPARAM)g_app.wins[g_cur_tab].hwnd, 0);
            break;
        case IDM_TAB_RENAME:
            tab_rename(g_cur_tab);
            break;
        case IDM_TAB_ADD_CHART: cmd_add_to_tab(1); break;
        case IDM_TAB_ADD_NUM: cmd_add_to_tab(0); break;
        case IDM_TAB_MAXIMIZE: {
            /* 最大化/还原当前 tab 内活动窗口（无活动→首个），填满 tab */
            OS_WinItem* wi;
            int k = 0, i;
            if (g_cur_tab < 0 || g_cur_tab >= g_app.win_count) break;
            wi = &g_app.wins[g_cur_tab];
            if (g_cur_win) {
                for (i = 0; i < wi->group_count; i++)
                    if (wi->group[i] == g_cur_win) { k = i; break; }
            }
            if (wi->group_max == k) wi->group_max = -1;
            else wi->group_max = k;
            os_log(OS_LOG_INFO, "窗口%s: tab%d %d/%d",
                   wi->group_max == k ? "最大化" : "还原", g_cur_tab, k, wi->group_count);
            layout_tab_pages();
            break;
        }
        case IDM_TAB_MINIMIZE: {
            /* Bug6: 最小化/还原当前 tab 内活动窗口（无活动→首个），缩进 tab 底部最小化条 */
            OS_WinItem* wi;
            int k = 0, i;
            if (g_cur_tab < 0 || g_cur_tab >= g_app.win_count) break;
            wi = &g_app.wins[g_cur_tab];
            if (g_cur_win) {
                for (i = 0; i < wi->group_count; i++)
                    if (wi->group[i] == g_cur_win) { k = i; break; }
            }
            wi->group_min[k] = !wi->group_min[k];
            if (wi->group_min[k] && wi->group_max == k) wi->group_max = -1; /* 最小化取消最大化 */
            os_log(OS_LOG_INFO, "窗口%s: tab%d %d/%d",
                   wi->group_min[k] ? "最小化" : "还原", g_cur_tab, k, wi->group_count);
            layout_tab_pages();
            InvalidateRect(g_app.hTab, NULL, TRUE); /* 最小化条出现/消失需重绘 */
            break;
        }
        case IDM_TAB_FULLSCREEN:
            if (g_cur_tab >= 0 && g_cur_tab < g_app.win_count) {
                HWND w = g_cur_win ? g_cur_win : g_app.wins[g_cur_tab].hwnd;
                PostMessage(hwnd, WM_OS_WIN_FULLSCREEN, (WPARAM)w, 0);
            }
            break;
        case IDM_TREE_WRITE: {
            HTREEITEM h = TreeView_GetSelection(g_app.hTree);
            if (h) {
                TVITEMW item;
                memset(&item, 0, sizeof(item));
                item.mask = TVIF_PARAM;
                item.hItem = h;
                if (TreeView_GetItem(g_app.hTree, &item) && item.lParam > 0)
                    os_dlg_edit_value(hwnd, (int)(item.lParam - 1));
            }
            break;
        }
        default:
            if (LOWORD(wParam) >= IDM_WIN_MODULE_BASE &&
                LOWORD(wParam) < IDM_WIN_MODULE_BASE + g_modwin_menu_count) {
                int idx = LOWORD(wParam) - IDM_WIN_MODULE_BASE;
                ModWinMenuItem* it = &g_modwin_menu[idx];
                HWND h;
                wchar_t title[128];
                if (it->mod && it->mod->create_window) {
                    char title8[128];
                    os_utf8_to_wide_buf(it->wt->display_name ? it->wt->display_name : it->wt->type,
                                        title, 128);
                    os_wide_to_utf8_buf(title, title8, 128);
                h = it->mod->create_window(it->ctx, it->wt->type, g_app.hTab,
                                               0, 0, 200, 150, title8);
                    if (h) add_win_item(h, 1, it->mod, it->ctx, title);
                }
            }
            break;
        }
        return 0;
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        KillTimer(hwnd, 2);
        KillTimer(hwnd, 3);
        g_tab_edit = NULL; /* 就地编辑框随主窗口销毁，避免消息循环读到悬空句柄 */
        g_tab_edit_idx = -1;
        os_ds_stop();
        os_datalog_stop();
        os_replay_stop();
        os_modmgr_shutdown();
        if (g_app.elf) os_elf_close(g_app.elf);
        g_app.elf = NULL;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void os_mainwin_register(void)
{
    WNDCLASSW wc;
    {
        WNDCLASSEXW wcx;
        memset(&wcx, 0, sizeof(wcx));
        wcx.cbSize = sizeof(wcx);
        wcx.lpfnWndProc = os_mainwin_proc;
        wcx.hInstance = g_app.hInst;
        wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcx.hIcon = LoadIconW(g_app.hInst, MAKEINTRESOURCEW(IDI_APP));
        wcx.hIconSm = LoadIconW(g_app.hInst, MAKEINTRESOURCEW(IDI_APP));
        wcx.lpszClassName = g_main_class;
        RegisterClassExW(&wcx);
    }
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = split_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_SIZEWE);
    wc.style = CS_DBLCLKS; /* Bug18: 需要 CS_DBLCLKS 才能收到 WM_LBUTTONDBLCLK（双击折叠） */
    wc.lpszClassName = L"OSSplitter";
    RegisterClassW(&wc);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = splith_proc;   /* F14: 消息栏上下拉伸分隔条 + F22: 双击抽屉收起 */
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_SIZENS);
    wc.style = CS_DBLCLKS; /* F22: 需要 CS_DBLCLKS 才能收到 WM_LBUTTONDBLCLK */
    wc.lpszClassName = L"OSSplitterH";
    RegisterClassW(&wc);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = log_strip_proc; /* F22: 消息栏收起后的底部细条 */
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    wc.lpszClassName = L"OSLogStrip";
    RegisterClassW(&wc);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = tree_strip_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_SIZEWE);
    wc.lpszClassName = L"OSTreeStrip";
    RegisterClassW(&wc);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = tree_pin_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"OSTreePin";
    RegisterClassW(&wc);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = right_panel_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = g_right_class;
    RegisterClassW(&wc);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = pick_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"OSDlgPick";
    RegisterClassW(&wc);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = edit_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"OSDlgEdit";
    RegisterClassW(&wc);
}
