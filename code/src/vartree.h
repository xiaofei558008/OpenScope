#ifndef OS_VARTREE_H
#define OS_VARTREE_H

#include <windows.h>

/* 从 g_app.elf 重建叶子变量表（保留原观测与 A2L 叶），返回叶子数 */
int os_vartree_build(void);
void os_vartree_clear(void);
/* 仅合并指定 Project 的 ELF：替换该 project 的非 A2L/非 synth 叶，保留其它 Project */
int os_vartree_merge_elf(int project_id, const wchar_t* path_w);

/* 重建左侧 TreeView（按设备/Project 分组） */
void os_vartree_fill_tree(HWND hTree);
/* 树填充进行中 */
int os_vartree_is_filling(void);

/* 设备根节点 lParam 编解码（负数哨兵，避免与叶 id+1 / -1 分组节点冲突） */
#define OS_TREE_LP_DEVICE_BASE (-100000)
LPARAM os_vartree_device_lparam(int project_id);
int    os_vartree_lparam_project(LPARAM lp); /* >=0=设备根；-1=普通节点 */

/* 模糊搜索叶子（子串、不区分大小写），返回匹配数 */
int os_vartree_search(const char* needle, int max, int* out_ids);
int os_vartree_find_by_name(const char* name);
int os_vartree_find_by_name_proj(const char* name, int project_id);

const OS_Leaf* os_vartree_leaf(int id);
int os_vartree_set_watch(int id, int on);
/* 兼容旧调用：树无勾选框，空实现 */
void os_vartree_set_check_ui(HWND hTree, int id, int on);

/* 重载后未找到的原观测变量（最多 64 个） */
int os_vartree_missing_count(void);
const char* os_vartree_missing_at(int i);
/* 按完整叶名称在变量树中定位并选中（供命令行测试钩子/自动化使用） */
int os_vartree_select_leaf(HWND hTree, const wchar_t* leaf_name);
/* Ctrl+H 快速搜索：按叶 id 列表定位变量（展开祖先、滚动可见、多选），返回定位个数 */
int os_vartree_locate_ids(HWND hTree, const int* ids, int n);

/* 需求14：远端同步变量——add 进合成列表并立即进叶表（1=新增 2=更新地址 0=无变化）；
 * synths_apply 在 ELF 重载后把合成列表重新应用进叶表（os_vartree_build 内部调用）。 */
int os_vartree_add_synth(const char* name, uint64_t addr, uint32_t size);
int os_vartree_synths_apply(void);
/* 每设备 MAP 符号：加入该设备变量树（同名更新地址），1=新增 2=更新 0=无变化 */
int os_vartree_add_map_symbol(int project_id, const char* name,
                              uint64_t addr, uint32_t size);
/* 删除设备叶（only_map=1 只删 MAP 叶；0=删该设备全部叶并压缩叶表）。返回删除数。
 * 叶 id 会变化：调用方随后需按 name+project 重绑窗口（os_chart_rebind/os_num_rebind）。 */
int os_vartree_remove_project(int project_id, int only_map);

#endif
