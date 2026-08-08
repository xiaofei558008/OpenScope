#include "app.h"
#include "vartree.h"
#include <commctrl.h>
#include <string.h>

static char g_missing[64][256];
static int  g_missing_count;

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
    os_log(OS_LOG_INFO, "ELF 变量表重建: %d 个叶子（原观测 %d 个，缺失 %d 个）",
           g_app.leaf_count, old_count, g_missing_count);
    return g_app.leaf_count;
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
    ins.item.mask = TVIF_TEXT | TVIF_PARAM;
    ins.item.pszText = (LPWSTR)text;
    ins.item.lParam = lparam;
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
                _snprintf(leafpath, 300, "%s.%s", name, c->name ? c->name : "anon");
                os_utf8_to_wide_buf(leafpath, wname, 320);
                _snwprintf(wfull, 420, L"%s  @0x%llX  [bit %u:%u]",
                           wname, (unsigned long long)(addr + (uint64_t)c->member_offset),
                           c->bit_size, c->bit_offset);
                add_node(tree, h, wfull, (LPARAM)-1, 0, 0);
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

void os_vartree_fill_tree(HWND hTree)
{
    int i, n;
    wchar_t wname[320];
    HTREEITEM h;
    TreeView_DeleteAllItems(hTree);
    if (!g_app.elf) return;
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
