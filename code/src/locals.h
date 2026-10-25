/*
 * 局部变量（Locals）：CPU 停住时按 PC + DWARF 求当前函数的参数/局部变量。
 *  - 引擎 os_locals_snapshot()：读寄存器（OS_CMD_REG_READ）→ 选函数/作用域 → 求 frame_base/CFA →
 *    求每个变量的位置 → 读内存/寄存器 → 按类型格式化（结构体/数组可展开）
 *  - 窗口 "locals"：停核时刷新，运行时置灰；不轮询（由 CPU 状态刷新驱动）
 */
#ifndef OS_LOCALS_H
#define OS_LOCALS_H

#include <windows.h>
#include <stdint.h>
#include "module_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OS_LocRow {
    char name[96];
    char value[160];
    char type[80];
    char where[48];      /* "0x70001234" / "D4" / "常量" / "" */
    char key[192];       /* 展开状态键：顶层名/成员路径 */
    int  level;          /* 0 = 顶层 */
    int  is_param;
    int  expandable;
    int  expanded;
} OS_LocRow;

enum {
    OS_LOCS_OK = 0,
    OS_LOCS_RUNNING,
    OS_LOCS_NOT_CONNECTED,
    OS_LOCS_NO_ELF,
    OS_LOCS_NO_FUNC,
    OS_LOCS_REG_FAIL,
    OS_LOCS_UNSUPPORTED
};

typedef struct OS_LocSnap {
    int        status;          /* OS_LOCS_* */
    char       msg[160];
    uint32_t   pc;
    char       func[128];
    char       file[260];
    int        line;
    OS_RegSet  regs;
    int        have_fb;
    uint64_t   frame_base;
    int        have_cfa;
    uint64_t   cfa;
    OS_LocRow* rows;
    int        nrows, caprows;
} OS_LocSnap;

/* is_expanded(key)：返回 1 展开；为 NULL 时按 expand_depth 全展开到该层 */
int  os_locals_snapshot(OS_LocSnap* s, int (*is_expanded)(const char* key, void* u), void* u, int expand_depth);
void os_locals_free(OS_LocSnap* s);
/* JSON（aiapi debug.locals）；返回写入长度 */
int  os_locals_to_json(const OS_LocSnap* s, char* out, int cap);

/* 窗口 */
void os_locals_register(void);
HWND os_locals_create(HWND parent, int x, int y, int w, int h, const wchar_t* title);
int  os_locals_is(HWND hwnd);
int  os_locals_window_count(void);
HWND os_locals_open(void);                     /* 菜单：打开/聚焦局部变量窗口 */
/* CPU 状态刷新时调用（主线程）：halted=1 且 PC 变化/从运行转停 → 刷新；运行 → 置灰 */
void os_locals_on_cpu(int halted, uint32_t pc, int connected);

#ifdef __cplusplus
}
#endif
#endif
