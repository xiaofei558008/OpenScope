#ifndef OS_APP_H
#define OS_APP_H

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include "elf.h"
#include "module_api.h"
#include "util.h"

#define OS_MAX_LEAVES        4096
#define OS_RING_CAP          8192
#define OS_MAX_MODULES       32
#define OS_MAX_WINS          64
#define OS_MAX_GROUP         8   /* N11: 一个 tab 最多容纳的窗口数 */
#define OS_MAX_CHART_SERIES  16
#define OS_CHART_HIST        8192
#define OS_MAX_NUM_ROWS      128

#define WM_OS_SAMPLES     (WM_APP + 1)
#define WM_OS_ACQ_STATE   (WM_APP + 2)
#define WM_OS_SPLIT       (WM_APP + 3)
#define WM_OS_REPLAY_TICK (WM_APP + 4)
#define WM_OS_WIN_CLOSED  (WM_APP + 5)
#define WM_OS_CHART_FITALL (WM_APP + 6) /* 停止采集：波形窗口整体展示全部波形 */
#define WM_OS_CHART_LIVE  (WM_APP + 7)  /* 开始采集：波形窗口回到跟随最新 */
#define WM_OS_WIN_MAXIMIZE (WM_APP + 8) /* N11: 窗口最大化/还原填满当前 tab (wParam=HWND) */
#define WM_OS_TREE_AUTOHIDE (WM_APP + 9) /* N9(d): 变量栏自动隐藏开关 (wParam=0/1，测试钩子) */
#define WM_OS_WIN_FULLSCREEN (WM_APP + 10) /* Bug3: 单窗口全屏/退出全屏 (wParam=HWND) */

#define IDI_APP 1 /* 应用图标资源（version.rc） */

typedef enum {
    OS_ACQ_STOPPED = 0,
    OS_ACQ_RUNNING,
    OS_ACQ_REPLAY
} OS_AcqState;

/* 叶子变量：可直接读写的变量单元（基础类型/枚举/指针/位域/字符串） */
typedef struct OS_Leaf {
    int         id;
    char        name[256];
    uint64_t    address;
    uint32_t    size;
    OS_TypeKind kind;
    int         is_signed;
    int         is_ptr;
    int         is_bitfield;
    uint8_t     bit_offset;
    uint8_t     bit_size;
    OS_EnumVal  enums[64];
    int         enum_count;
    volatile LONG watched;
    OS_Sample   sample;
} OS_Leaf;

/* 右侧窗口区的一个 tab。N11: 一个 tab 可容纳多个窗口（group[0]==hwnd）。 */
typedef struct OS_WinItem {
    HWND        hwnd;
    int         is_module;
    OS_Module*  mod;
    void*       mod_ctx;
    wchar_t     title[128];
    int         active;
    HWND        group[OS_MAX_GROUP];          /* tab 内全部窗口 */
    int         group_count;                  /* 组内窗口数（含主窗口） */
    int         group_max;                    /* 最大化窗口下标，-1=平铺 */
    wchar_t     group_title[OS_MAX_GROUP][128]; /* 各窗口标题（关闭主窗口后提升用） */
} OS_WinItem;

typedef struct OS_App {
    HINSTANCE hInst;
    HWND hMain, hTree, hRight, hLog, hStatus, hSplitV, hBtnBar, hTab;
    HWND hTreeStrip;       /* N9(d): 变量栏隐藏后左侧细条（悬停展开） */
    HWND hTreePin;         /* N12: 变量栏顶部钉图标按钮（钉住/自动隐藏） */
    int tree_w, log_h;
    int tree_auto;         /* N9(d): 1=变量栏自动隐藏，0=钉住常显 */
    int tree_hidden;       /* 当前变量栏是否已自动隐藏 */
    HWND fs_win;           /* Bug3: 当前全屏窗口（NULL=无） */

    OS_ElfFile* elf;
    wchar_t elf_path[MAX_PATH];
    uint64_t elf_mtime;

    OS_Leaf leaves[OS_MAX_LEAVES];
    int leaf_count;
    int watch_count;

    OS_Module mods[OS_MAX_MODULES];
    void* mod_ctx[OS_MAX_MODULES];
    int mod_count;
    OS_Module* driver;
    void* driver_ctx;
    OS_Module* winmods[OS_MAX_MODULES];
    void* winmod_ctx[OS_MAX_MODULES];
    int winmod_count;
    OS_Framework fw;

    volatile LONG acq_state;
    volatile LONG connected;
    HANDLE hPoll;
    volatile LONG stop_poll;
    int poll_interval_ms;
    CRITICAL_SECTION ring_cs;
    OS_Sample ring[OS_RING_CAP];
    int ring_head, ring_tail;
    volatile LONG total_samples;

    FILE* log_csv;
    wchar_t log_path[MAX_PATH];
    wchar_t shot_path[MAX_PATH]; /* --shot=<路径>：窗口创建后自动截图调试 */
    wchar_t rename_tab[MAX_PATH]; /* --rename-tab=<名>：首个窗口创建后自动重命名（测试钩子） */
    wchar_t replay_path[MAX_PATH]; /* --replay=<csv>：启动后自动离线回放（测试钩子） */

    struct OS_Replay* replay;

    OS_WinItem wins[OS_MAX_WINS];
    int win_count;
} OS_App;

extern OS_App g_app;

/* 框架回调实现（分布于各文件） */
void os_fw_log(int level, const char* fmt, ...);
void os_fw_post(UINT msg, WPARAM w, LPARAM l);
const struct OS_Variable* os_fw_find(const char* name);
int os_fw_leaf_count(void);
const char* os_fw_leaf_name(int id);
const OS_Sample* os_fw_leaf_sample(int id);
int os_fw_pick_variable(HWND parent, char* out, int out_len);
int os_fw_write_leaf(int id, double value, char* err, int err_len);
void os_fw_on_elf_reloaded(void);
int os_fw_leaf_find(const char* needle, int* ids, int max_ids);

/* 主窗口控制（mainwin.c） */
void os_mainwin_update_buttons(void);
void os_mainwin_append_log(int level, const wchar_t* line);
void os_mainwin_tile(void);
int os_mainwin_reload_elf(void);

#endif
