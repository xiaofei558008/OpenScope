#include "app.h"
#include "vartree.h"
#include <commctrl.h>
#include <string.h>

static char g_missing[64][256];
static int  g_missing_count;
/* 需求2：树填充（删除重建）期间屏蔽 TVN_ITEMCHANGED 勾选联动——
 * 重建时树节点 lParam（旧叶 id+1）指向新叶表的错误变量，通知回写会误置观测标志 */
static int  g_tree_filling;

static void add_leaf(const char* name, uint64_t addr, uint32_t size, OS_TypeKind kind,
                     int is_signed, int is_ptr, int is_bitfield, int bit_off, int bit_size,
                     const OS_Type* enum_type)
{
    OS_Leaf* L;
    int i;
    if (g_app.leaf_count >= OS_MAX_LEAVES) return;
    if (size < 1) size = 1;
    L = &g_app.leaves[g_app.leaf_count];
    memset(L, 0, sizeof(*L));
    L->id = g_app.leaf_count;
    _snprintf(L->name, sizeof(L->name), "%s", name);
    L->address = addr;
    L->size = size;
    L->kind = kind;
    L->is_signed = is_signed;
    L->is_ptr = is_ptr;
    L->is_bitfield = is_bitfield;
    L->bit_offset = (uint8_t)bit_off;
    L->bit_size = (uint8_t)bit_size;
    if (enum_type) {
        for (i = 0; i < enum_type->child_count && i < 64; i++) {
            const OS_Type* e = enum_type->children[i];
            if (!e) continue;
            _snprintf(L->enums[L->enum_count].name, 64, "%s", e->name ? e->name : "");
            L->enums[L->enum_count].value = e->enum_value;
            L->enum_count++;
        }
    }
    g_app.leaf_count++;
}

static int storage_size(const OS_Type* t)
{
    if (t->size > 0 && t->size <= 8) return (int)t->size;
    if (t->bit_size > 0) return ((t->bit_offset + t->bit_size + 7) / 8 + 3) / 4 * 4;
    return 4;
}

static void flatten(const OS_Type* t, uint64_t base, const char* path, int depth)
{
    int i;
    char p[300];
    if (depth > 64) return;
    if (g_app.leaf_count >= OS_MAX_LEAVES) return;
    if (!t) return;
    switch (t->kind) {
    case OS_TYPE_STRUCT:
    case OS_TYPE_UNION:
        if (!t->children || t->child_count == 0) {
            add_leaf(path, base, t->size ? t->size : 1, OS_TYPE_OTHER, 0, 0, 0, 0, 0, NULL);
            return;
        }
        for (i = 0; i < t->child_count; i++) {
            const OS_Type* c = t->children[i];
            if (!c) continue;
            _snprintf(p, sizeof(p), "%s.%s", path, c->name ? c->name : "anon");
            if (c->is_bitfield) {
                add_leaf(p, base + (uint64_t)c->member_offset, (uint32_t)storage_size(c),
                         (c->is_signed ? OS_TYPE_INT : OS_TYPE_UINT), c->is_signed, 0,
                         1, c->bit_offset, c->bit_size, NULL);
            } else {
                flatten(c, base + (uint64_t)c->member_offset, p, depth + 1);
            }
        }
        break;
    case OS_TYPE_ARRAY: {
        const OS_Type* elem = (t->child_count > 0) ? t->children[0] : NULL;
        int n = t->array_count;
        if (n < 0) n = 0;
        if (!elem) {
            add_leaf(path, base, t->size ? t->size : 1, OS_TYPE_OTHER, 0, 0, 0, 0, 0, NULL);
            break;
        }
        if (elem->size == 1 && n > 16) {
            add_leaf(path, base, (uint32_t)n, OS_TYPE_STRING, 0, 0, 0, 0, 0, NULL);
            break;
        }
        if (n > 1024) n = 1024;
        for (i = 0; i < n; i++) {
            _snprintf(p, sizeof(p), "%s[%d]", path, i);
            if (elem->kind == OS_TYPE_STRUCT || elem->kind == OS_TYPE_UNION) {
                flatten(elem, base + (uint64_t)i * (elem->size ? elem->size : 1), p, depth + 1);
            } else {
                add_leaf(p, base + (uint64_t)i * (elem->size ? elem->size : 1),
                         elem->size ? elem->size : 1, elem->kind, elem->is_signed, elem->is_ptr,
                         0, 0, 0, elem->kind == OS_TYPE_ENUM ? elem : NULL);
            }
        }
        break;
    }
    default:
        add_leaf(path, base, t->size ? t->size : 1, t->kind, t->is_signed, t->is_ptr,
                 0, 0, 0, t->kind == OS_TYPE_ENUM ? t : NULL);
        break;
    }
}

void os_vartree_clear(void)
{
    g_app.leaf_count = 0;
    g_app.watch_count = 0;
}

static int was_watched(const char* name)
{
    int i;
    for (i = 0; i < g_app.leaf_count; i++) {
        if (g_app.leaves[i].watched && !strcmp(g_app.leaves[i].name, name)) return 1;
    }
    return 0;
}

int os_vartree_build(void)
{
    static char old_watch[OS_MAX_LEAVES][256];
    static int old_count;
    char err[256];
    int i, n;
    /* 保存旧观测（按全名） */
    old_count = 0;
    for (i = 0; i < g_app.leaf_count && old_count < OS_MAX_LEAVES; i++) {
        if (g_app.leaves[i].watched) {
            _snprintf(old_watch[old_count], 256, "%s", g_app.leaves[i].name);
            old_count++;
        }
    }
    os_vartree_clear();
    if (!g_app.elf) return 0;
    n = os_elf_var_count(g_app.elf);
    for (i = 0; i < n; i++) {
        const OS_Variable* v = os_elf_var_at(g_app.elf, i);
        if (!v) continue;
        if (v->type) {
            flatten(v->type, v->address, v->name, 0);
        } else {
            add_leaf(v->name, v->address, v->symbol_size ? (uint32_t)v->symbol_size : 4,
                     OS_TYPE_OTHER, 0, 0, 0, 0, 0, NULL);
        }
    }
    /* 恢复观测，记录缺失 */
    g_missing_count = 0;
    for (i = 0; i < old_count; i++) {
        int found = 0, j;
        for (j = 0; j < g_app.leaf_count; j++) {
            if (!strcmp(g_app.leaves[j].name, old_watch[i])) {
                g_app.leaves[j].watched = 1;
                g_app.watch_count++;
                found = 1;
                break;
            }
        }
        if (!found && g_missing_count < 64) {
            _snprintf(g_missing[g_missing_count], 256, "%s", old_watch[i]);
            g_missing_count++;
        }
    }
    (void)err;
    /* 需求14：远端同步变量在 ELF 重建后重新应用（ELF 重载不丢网络变量） */
    if (g_app.synth_count > 0)
        os_vartree_synths_apply();
    os_log(OS_LOG_INFO, "ELF 变量表重建: %d 个叶子（原观测 %d 个，缺失 %d 个）",
           g_app.leaf_count, old_count, g_missing_count);
    return g_app.leaf_count;
}

/* ---------- 需求14：远端同步变量（上传/下载 ELF 应用） ---------- */

static OS_TypeKind synth_kind_for_size(uint32_t size)
{
    /* 无 DWARF 类型的远端变量：按大小给默认类型（4→有符号整型，8→整型64，
     * 其余按无符号字节宽处理），便于数值窗口显示与写入解析 */
    switch (size) {
    case 1: return OS_TYPE_INT;
    case 2: return OS_TYPE_INT;
    case 4: return OS_TYPE_INT;
    case 8: return OS_TYPE_INT;
    default: return OS_TYPE_UINT;
    }
}

/* 把合成变量挂进叶表（不检查重名——调用方先查） */
static void synth_leaf_append(const char* name, uint64_t addr, uint32_t size)
{
    OS_Leaf* L;
    if (g_app.leaf_count >= OS_MAX_LEAVES) return;
    L = &g_app.leaves[g_app.leaf_count];
    memset(L, 0, sizeof(*L));
    L->id = g_app.leaf_count;
    _snprintf(L->name, sizeof(L->name), "%s", name);
    L->address = addr;
    L->size = (size < 1) ? 4 : (size > 8 ? 8 : size);
    L->kind = synth_kind_for_size(L->size);
    L->is_signed = 1;
    L->is_synth = 1;
    g_app.leaf_count++;
}

int os_vartree_add_synth(const char* name, uint64_t addr, uint32_t size)
{
    int i, idx;
    if (!name || !name[0]) return 0;
    /* 已有同名叶（本地解析或既有合成）：更新地址（对端编译产物地址可能不同） */
    idx = os_vartree_find_by_name(name);
    if (idx >= 0) {
        if (g_app.leaves[idx].address == addr) return 0;
        g_app.leaves[idx].address = addr;
        return 2;
    }
    /* 合成列表去重 */
    for (i = 0; i < g_app.synth_count; i++) {
        if (!strcmp(g_app.synth_name[i], name)) {
            if (g_app.synth_addr[i] == addr) return 0;
            g_app.synth_addr[i] = addr;
            g_app.synth_size[i] = size;
            return 2;
        }
    }
    if (g_app.synth_count >= 256) return 0; /* 上限 */
    i = g_app.synth_count++;
    _snprintf(g_app.synth_name[i], 256, "%s", name);
    g_app.synth_addr[i] = addr;
    g_app.synth_size[i] = size;
    synth_leaf_append(name, addr, size);
    return 1;
}

/* ELF 重载后把合成列表重新应用进叶表（在 os_vartree_build 末尾调用）。
 * 返回实际应用数（跳过与本地叶重名且地址相同的项）。 */
int os_vartree_synths_apply(void)
{
    int i, n = 0;
    for (i = 0; i < g_app.synth_count; i++) {
        int idx = os_vartree_find_by_name(g_app.synth_name[i]);
        if (idx >= 0) {
            if (g_app.leaves[idx].address == g_app.synth_addr[i]) continue;
            g_app.leaves[idx].address = g_app.synth_addr[i];
            n++;
            continue;
        }
        synth_leaf_append(g_app.synth_name[i], g_app.synth_addr[i], g_app.synth_size[i]);
        n++;
    }
    return n;
}

int os_vartree_missing_count(void) { return g_missing_count; }
const char* os_vartree_missing_at(int i)
{
    if (i < 0 || i >= g_missing_count) return NULL;
    return g_missing[i];
}

int os_vartree_find_by_name(const char* name)
{
    int i;
    for (i = 0; i < g_app.leaf_count; i++) {
        if (!strcmp(g_app.leaves[i].name, name)) return i;
    }
    return -1;
}

static int tree_select_recursive(HWND tree, HTREEITEM h, const wchar_t* target)
{
    wchar_t wtext[420];
    TVITEMW it;
    HTREEITEM child;
    memset(&it, 0, sizeof(it));
    it.mask = TVIF_TEXT | TVIF_HANDLE;
    it.hItem = h;
    it.pszText = wtext;
    it.cchTextMax = 420;
    if (SendMessageW(tree, TVM_GETITEMW, 0, (LPARAM)&it) && wtext[0]) {
        /* 前缀匹配：叶文本 = "名  @地址" 或 "名  @地址  [bit x:y]" */
        if (wcsncmp(wtext, target, wcslen(target)) == 0 &&
            (wtext[wcslen(target)] == 0 || wtext[wcslen(target)] == L' ')) {
            SendMessageW(tree, TVM_SELECTITEM, TVGN_CARET, (LPARAM)h);
            return 1;
        }
    }
    child = (HTREEITEM)SendMessageW(tree, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)h);
    while (child) {
        if (tree_select_recursive(tree, child, target)) return 1;
        child = (HTREEITEM)SendMessageW(tree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)child);
    }
    return 0;
}

int os_vartree_select_leaf(HWND hTree, const wchar_t* leaf_name)
{
    char name[512];
    int id;
    const OS_Leaf* L;
    wchar_t target[600];
    HTREEITEM h;
    if (!hTree || !leaf_name || !leaf_name[0]) return -1;
    os_wide_to_utf8_buf(leaf_name, name, sizeof(name));
    id = os_vartree_find_by_name(name);
    os_log(OS_LOG_DEBUG, "select_leaf: name='%s' find=%d", name, id);
    if (id < 0) return -1;
    L = &g_app.leaves[id];
    _snwprintf(target, 600, L"%hs  @0x%llX", L->name, (unsigned long long)L->address);
    os_log(OS_LOG_DEBUG, "select_leaf: target='%ls'", target);
    h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    os_log(OS_LOG_DEBUG, "select_leaf: root=%p", (void*)h);
    while (h) {
        if (tree_select_recursive(hTree, h, target)) return 0;
        h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)h);
    }
    return -1;
}

int os_vartree_search(const char* needle, int max, int* out_ids)
{
    int i, n = 0;
    if (!needle || !*needle || max <= 0) return 0;
    for (i = 0; i < g_app.leaf_count && n < max; i++) {
        if (_strnicmp(g_app.leaves[i].name, needle, strlen(needle)) == 0 ||
            strstr(g_app.leaves[i].name, needle) != NULL) {
            out_ids[n++] = i;
        }
    }
    return n;
}

/* Ctrl+H 快速搜索：按叶 id（lParam=id+1）在树中递归查找节点 */
static HTREEITEM tree_find_by_leaf_id(HWND tree, HTREEITEM h, int leaf_id)
{
    HTREEITEM child;
    TVITEMW item;
    memset(&item, 0, sizeof(item));
    item.mask = TVIF_PARAM;
    item.hItem = h;
    if (TreeView_GetItem(tree, &item) && item.lParam == (LPARAM)(leaf_id + 1))
        return h;
    child = (HTREEITEM)SendMessageW(tree, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)h);
    while (child) {
        HTREEITEM r = tree_find_by_leaf_id(tree, child, leaf_id);
        if (r) return r;
        child = (HTREEITEM)SendMessageW(tree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)child);
    }
    return NULL;
}

/* Ctrl+H 快速搜索：按叶 id 列表定位变量——展开全部祖先、滚动到可见、
 * 多选显示（首个为光标项）。返回定位到的个数。 */
int os_vartree_locate_ids(HWND hTree, const int* ids, int n)
{
    HTREEITEM found = NULL, h;
    int i, ok = 0;
    if (!hTree || !ids || n <= 0) return 0;
    for (i = 0; i < n; i++) {
        HTREEITEM item, parent;
        h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_ROOT, 0);
        while (h && !found) {
            found = tree_find_by_leaf_id(hTree, h, ids[i]);
            h = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)h);
        }
        if (!found) { found = NULL; continue; }
        item = found;
        /* 展开全部祖先链 */
        parent = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)item);
        while (parent) {
            SendMessageW(hTree, TVM_EXPAND, TVE_EXPAND, (LPARAM)parent);
            parent = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)parent);
        }
        if (i == 0) {
            /* 首个：光标选中 + 滚动可见 */
            SendMessageW(hTree, TVM_SELECTITEM, TVGN_CARET, (LPARAM)item);
            SendMessageW(hTree, TVM_ENSUREVISIBLE, 0, (LPARAM)item);
        } else {
            /* 其余：多选高亮（树已开 TVS_EX_MULTISELECT） */
            TreeView_SetItemState(hTree, item, TVIS_SELECTED, TVIS_SELECTED);
        }
        ok++;
        found = NULL;
    }
    return ok;
}

const OS_Leaf* os_vartree_leaf(int id)
{
    if (id < 0 || id >= g_app.leaf_count) return NULL;
    return &g_app.leaves[id];
}

int os_vartree_set_watch(int id, int on)
{
    OS_Leaf* L;
    if (id < 0 || id >= g_app.leaf_count) return -1;
    L = &g_app.leaves[id];
    if (on && !L->watched) g_app.watch_count++;
    if (!on && L->watched) g_app.watch_count--;
    L->watched = on ? 1 : 0;
    return 0;
}

static void set_check(HWND tree, HTREEITEM h, int checked); /* 定义在下方 fill_tree 区 */

/* 递归查找 lParam=id+1 的树节点并设置勾选（配合 set_watch 联动左侧勾选框） */
static int set_check_walk(HWND hTree, HTREEITEM node, int id, int on)
{
    HTREEITEM child;
    TVITEMW item;
    while (node) {
        memset(&item, 0, sizeof(item));
        item.mask = TVIF_PARAM;
        item.hItem = node;
        if (TreeView_GetItem(hTree, &item) && item.lParam == (LPARAM)(id + 1)) {
            set_check(hTree, node, on);
            return 1;
        }
        child = TreeView_GetChild(hTree, node);
        if (child && set_check_walk(hTree, child, id, on)) return 1;
        node = TreeView_GetNextSibling(hTree, node);
    }
    return 0;
}

/* 程序化设置某个叶变量在左侧树的勾选状态（lParam=id+1）。
 * 供“添加变量到窗口”时联动勾选，避免整树重建丢失展开状态。 */
void os_vartree_set_check_ui(HWND hTree, int id, int on)
{
    HTREEITEM root;
    if (!hTree || !IsWindow(hTree) || id < 0) return;
    root = TreeView_GetRoot(hTree);
    if (root) set_check_walk(hTree, root, id, on);
}

/* ---------- TreeView 填充 ---------- */

static void set_check(HWND tree, HTREEITEM h, int checked)
{
    TVITEMW item;
    memset(&item, 0, sizeof(item));
    item.mask = TVIF_STATE;
    item.hItem = h;
    item.stateMask = TVIS_STATEIMAGEMASK;
    item.state = INDEXTOSTATEIMAGEMASK(checked ? 2 : 1);
    TreeView_SetItem(tree, &item);
}

static HTREEITEM add_node(HWND tree, HTREEITEM parent, const wchar_t* text, LPARAM lparam,
                          int leaf, int checked)
{
    TVINSERTSTRUCTW ins;
    memset(&ins, 0, sizeof(ins));
    ins.hParent = parent;
    ins.hInsertAfter = TVI_LAST;
    ins.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
    ins.item.pszText = (LPWSTR)text;
    ins.item.lParam = lparam;
    /* 需求2：重建后勾选状态随 watched 恢复（填充期间通知被 g_tree_filling 屏蔽，不会回写） */
    ins.item.stateMask = TVIS_STATEIMAGEMASK;
    ins.item.state = INDEXTOSTATEIMAGEMASK(checked ? 2 : 1);
    return TreeView_InsertItem(tree, &ins);
}

static void tree_add_leaf(HWND tree, HTREEITEM parent, const OS_Leaf* L)
{
    wchar_t wname[320], wfull[420];
    os_utf8_to_wide_buf(L->name, wname, 320);
    _snwprintf(wfull, 420, L"%s  @0x%llX", wname, (unsigned long long)L->address);
    add_node(tree, parent, wfull, (LPARAM)(L->id + 1), 1, L->watched ? 1 : 0);
}

static void tree_add_type(HWND tree, HTREEITEM parent, const OS_Type* t, const char* name,
                          uint64_t addr)
{
    wchar_t wname[320], wfull[420];
    HTREEITEM h;
    int i;
    if (!t) return;
    os_utf8_to_wide_buf(name, wname, 320);
    if (t->kind == OS_TYPE_STRUCT || t->kind == OS_TYPE_UNION) {
        _snwprintf(wfull, 420, L"%s {...} (%u B)", wname, t->size);
        h = add_node(tree, parent, wfull, (LPARAM)-1, 0, 0);
        for (i = 0; i < t->child_count; i++) {
            const OS_Type* c = t->children[i];
            if (!c) continue;
            if (c->is_bitfield) {
                char leafpath[300];
                int bid;
                _snprintf(leafpath, 300, "%s.%s", name, c->name ? c->name : "anon");
                bid = os_vartree_find_by_name(leafpath);
                os_utf8_to_wide_buf(leafpath, wname, 320);
                _snwprintf(wfull, 420, L"%s  @0x%llX  [bit %u:%u]",
                           wname, (unsigned long long)(addr + (uint64_t)c->member_offset),
                           c->bit_size, c->bit_offset);
                /* 位域也是可读写的叶变量：挂叶 id，右键可添加/写值 */
                add_node(tree, h, wfull, (LPARAM)(bid >= 0 ? bid + 1 : -1), 0, 0);
            } else {
                char child[300];
                _snprintf(child, 300, "%s.%s", name, c->name ? c->name : "anon");
                tree_add_type(tree, h, c, child, addr + (uint64_t)c->member_offset);
            }
        }
        return;
    }
    if (t->kind == OS_TYPE_ARRAY) {
        const OS_Type* elem = t->child_count > 0 ? t->children[0] : NULL;
        int n = t->array_count;
        if (n < 0) n = 0;
        if (n > 512) n = 512;
        _snwprintf(wfull, 420, L"%s [%d]", wname, t->array_count);
        h = add_node(tree, parent, wfull, (LPARAM)-1, 0, 0);
        if (!elem) return;
        if (elem->size == 1 && t->array_count > 16) {
            wchar_t w2[420];
            _snwprintf(w2, 420, L"%s (string)", wname);
            add_node(tree, h, w2, (LPARAM)-1, 0, 0);
            return;
        }
        for (i = 0; i < n; i++) {
            char child[300];
            _snprintf(child, 300, "%s[%d]", name, i);
            tree_add_type(tree, h, elem, child, addr + (uint64_t)i * (elem->size ? elem->size : 1));
        }
        return;
    }
    /* 叶子：查 leaf id */
    {
        int id = os_vartree_find_by_name(name);
        if (id >= 0) {
            tree_add_leaf(tree, parent, &g_app.leaves[id]);
        } else {
            os_utf8_to_wide_buf(name, wname, 320);
            _snwprintf(wfull, 420, L"%s  @0x%llX", wname, (unsigned long long)addr);
            add_node(tree, parent, wfull, (LPARAM)-1, 0, 0);
        }
    }
}

int os_vartree_is_filling(void) { return g_tree_filling; }

void os_vartree_fill_tree(HWND hTree)
{
    int i, n;
    wchar_t wname[320];
    HTREEITEM h;
    g_tree_filling = 1;
    TreeView_DeleteAllItems(hTree);
    if (g_app.elf) {
        n = os_elf_var_count(g_app.elf);
        for (i = 0; i < n; i++) {
            const OS_Variable* v = os_elf_var_at(g_app.elf, i);
            if (!v) continue;
            os_utf8_to_wide_buf(v->name, wname, 320);
            if (v->type && (v->type->kind == OS_TYPE_STRUCT || v->type->kind == OS_TYPE_UNION ||
                            v->type->kind == OS_TYPE_ARRAY)) {
                h = add_node(hTree, NULL, wname, (LPARAM)-1, 0, 0);
                tree_add_type(hTree, h, v->type, v->name, v->address);
            } else if (v->type) {
                tree_add_type(hTree, NULL, v->type, v->name, v->address);
            } else {
                wchar_t wfull[420];
                _snwprintf(wfull, 420, L"%s  @0x%llX  [%llu B]",
                           wname, (unsigned long long)v->address,
                           (unsigned long long)v->symbol_size);
                add_node(hTree, NULL, wfull, (LPARAM)-1, 0, 0);
            }
        }
    }
    /* 需求14：远端同步变量（上传/下载 ELF 应用进来的）追加展示——本机无 ELF 时也显示 */
    for (i = 0; i < g_app.leaf_count; i++) {
        if (g_app.leaves[i].is_synth)
            tree_add_leaf(hTree, NULL, &g_app.leaves[i]);
    }
    g_tree_filling = 0;
    (void)set_check;
}

/* ---------- 框架回调 ---------- */

const struct OS_Variable* os_fw_find(const char* name)
{
    int idx;
    if (!g_app.elf || !name) return NULL;
    idx = os_elf_find_var(g_app.elf, name);
    if (idx < 0) return NULL;
    return os_elf_var_at(g_app.elf, idx);
}

int os_fw_leaf_count(void) { return g_app.leaf_count; }

/* 需求14：远端变量列表应用（上传/下载 ELF） */
int os_fw_leaf_add(const char* name, uint64_t addr, uint32_t size)
{
    return os_vartree_add_synth(name, addr, size);
}

void os_fw_leaf_sync_done(int added, int updated)
{
    if (added > 0 || updated > 0) {
        os_log(OS_LOG_INFO, "网络变量同步: 新增 %d 个，更新地址 %d 个，当前叶表 %d",
               added, updated, g_app.leaf_count);
        if (g_app.hMain) PostMessage(g_app.hMain, WM_OS_LEAF_SYNC, 0, 0);
    }
}

uint32_t os_fw_leaf_size(int id)
{
    const OS_Leaf* L = os_vartree_leaf(id);
    return L ? L->size : 0;
}

const char* os_fw_leaf_name(int id)
{
    const OS_Leaf* L = os_vartree_leaf(id);
    return L ? L->name : NULL;
}

const OS_Sample* os_fw_leaf_sample(int id)
{
    const OS_Leaf* L = os_vartree_leaf(id);
    return L ? &L->sample : NULL;
}

int os_fw_leaf_find(const char* needle, int* ids, int max_ids)
{
    return os_vartree_search(needle, max_ids, ids);
}

uint64_t os_fw_leaf_addr(int id)
{
    const OS_Leaf* L = os_vartree_leaf(id);
    return L ? L->address : 0;
}
