/*
 * 源码目录索引：扫描 srcmap 根目录，支持模糊搜索打开文件。
 */
#ifndef OS_SRCIDX_H
#define OS_SRCIDX_H

#include <windows.h>
#include "srcmap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OS_SRCIDX_MAX 8192

typedef struct OS_SrcIdx {
    wchar_t paths[OS_SRCIDX_MAX][MAX_PATH];
    int     count;
} OS_SrcIdx;

void os_srcidx_clear(OS_SrcIdx* idx);
/* 递归扫描 m->roots 下常见源码扩展名 */
void os_srcidx_rebuild(OS_SrcIdx* idx, const OS_SrcMap* m);

/* 模糊匹配：needle 各字符按序出现（不区分大小写）；写入 out 索引，返回个数 */
int  os_srcidx_fuzzy(const OS_SrcIdx* idx, const wchar_t* needle,
                     int* out, int max_out);

/* 填充源码 TreeView（目录节点 + 文件叶子；lParam=文件路径堆指针，由调用方在重建前清空） */
void os_srcidx_fill_tree(HWND tree, const OS_SrcIdx* idx);

/* Ctrl+P：模糊搜索并打开；选中后调用 on_open(path)；取消返回 0，打开返回 1 */
typedef void (*OS_SrcOpenFn)(const wchar_t* path, void* user);
int  os_srcidx_quick_open(HWND owner, const OS_SrcIdx* idx, OS_SrcOpenFn on_open, void* user);

#ifdef __cplusplus
}
#endif

#endif
