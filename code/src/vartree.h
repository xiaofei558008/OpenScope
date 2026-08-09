#ifndef OS_VARTREE_H
#define OS_VARTREE_H

#include <windows.h>

/* 从 g_app.elf 重建叶子变量表（保留原观测选择），返回叶子数 */
int os_vartree_build(void);
void os_vartree_clear(void);

/* 重建左侧 TreeView */
void os_vartree_fill_tree(HWND hTree);

/* 模糊搜索叶子（子串、不区分大小写），返回匹配数 */
int os_vartree_search(const char* needle, int max, int* out_ids);
int os_vartree_find_by_name(const char* name);

const OS_Leaf* os_vartree_leaf(int id);
int os_vartree_set_watch(int id, int on);
/* 程序化设置叶变量在左侧树中的勾选状态（配合 set_watch 联动显示） */
void os_vartree_set_check_ui(HWND hTree, int id, int on);

/* 重载后未找到的原观测变量（最多 64 个） */
int os_vartree_missing_count(void);
const char* os_vartree_missing_at(int i);
/* 按完整叶名称在变量树中定位并选中（供命令行测试钩子/自动化使用） */
int os_vartree_select_leaf(HWND hTree, const wchar_t* leaf_name);

#endif
