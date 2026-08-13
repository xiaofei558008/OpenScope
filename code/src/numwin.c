#include "app.h"
#include "numwin.h"
#include "mainwin.h"
#include "vartree.h"
#include "datasrv.h"
#include "util.h"
#include "theme.h"
#include <commctrl.h>
#include <string.h>

#define OS_MAGIC_NUM 0x4E554D31u /* 'NUM1' */

/* 列定义：0=实时勾选框, 1=变量, 2=地址, 3=数值, 4=最小值, 5=最大值 */
#define NUM_COL_CHECK 0
#define NUM_COL_NAME  1
#define NUM_COL_ADDR  2
#define NUM_COL_VAL   3
#define NUM_COL_VALIDX 3
#define NUM_COL_MIN   4
#define NUM_COL_MAX   5

typedef struct OS_NumWin {
    DWORD magic;
    HWND hwnd;
    HWND list;
    wchar_t title[128];
    int leaf_ids[OS_MAX_NUM_ROWS];
    char names[OS_MAX_NUM_ROWS][256]; /* 添加时的变量全名：ELF 重载后按名重绑（需求2） */
    int count;
    HWND edit;       /* 就地编辑 EDIT 控件（NULL=无） */
    int edit_row;    /* 正在编辑的行 */
    /* 用户反馈：min/max 列——记录运行过程中每个变量的最小/最大值 */
    double vmin[OS_MAX_NUM_ROWS];
    double vmax[OS_MAX_NUM_ROWS];
    int    has_mm[OS_MAX_NUM_ROWS];  /* 1=已收到首个样本（有有效 min/max） */
} OS_NumWin;

static const wchar_t* g_num_class = L"OSNumWin";

#define MENU_NUM_ADD   3101
#define MENU_NUM_REMOVE 3102
#define MENU_NUM_WRITE 3103
#define MENU_NUM_CLOSE 3104

static OS_NumWin* num_from_hwnd(HWND hwnd)
{
    OS_NumWin* nw = (OS_NumWin*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (!nw || nw->magic != OS_MAGIC_NUM) return NULL;
    return nw;
}

static int num_row_of(OS_NumWin* nw, int leaf_id)
{
    int i;
    for (i = 0; i < nw->count; i++) {
        if (nw->leaf_ids[i] == leaf_id) return i;
    }
    return -1;
}

static void num_end_edit(OS_NumWin* nw, int commit); /* 定义在下方就地编辑区 */

/* min/max 显示格式：整型类按整数（%.0f），浮点按 %.6g */
static void num_fmt_val(const OS_Leaf* L, double v, char* out, int cap)
{
    if (!L) { _snprintf(out, cap, "%.6g", v); return; }
    if (L->kind == OS_TYPE_FLOAT) _snprintf(out, cap, "%.6g", v);
    else _snprintf(out, cap, "%.0f", v);
}

/* 更新第 row 行的 min/max 并刷新显示（首个样本初始化，之后取极值） */
static void num_update_minmax(OS_NumWin* nw, int row, double v)
{
    wchar_t wmin[64], wmax[64];
    char tmin[64], tmax[64];
    const OS_Leaf* L = os_vartree_leaf(nw->leaf_ids[row]);
    if (row < 0 || row >= nw->count || !nw->list) return;
    if (!nw->has_mm[row]) {
        nw->vmin[row] = nw->vmax[row] = v;
        nw->has_mm[row] = 1;
    } else {
        if (v < nw->vmin[row]) nw->vmin[row] = v;
        if (v > nw->vmax[row]) nw->vmax[row] = v;
    }
    num_fmt_val(L, nw->vmin[row], tmin, sizeof(tmin));
    num_fmt_val(L, nw->vmax[row], tmax, sizeof(tmax));
    os_utf8_to_wide_buf(tmin, wmin, 64);
    os_utf8_to_wide_buf(tmax, wmax, 64);
    ListView_SetItemText(nw->list, row, NUM_COL_MIN, wmin);
    ListView_SetItemText(nw->list, row, NUM_COL_MAX, wmax);
}

/* 重置第 row 行 min/max（添加/重绑时清空为 "-"） */
static void num_reset_minmax(OS_NumWin* nw, int row)
{
    nw->has_mm[row] = 0;
    nw->vmin[row] = nw->vmax[row] = 0;
    if (nw->list) {
        ListView_SetItemText(nw->list, row, NUM_COL_MIN, L"-");
        ListView_SetItemText(nw->list, row, NUM_COL_MAX, L"-");
    }
}

void os_num_push(HWND hwnd, const OS_Sample* s)
{
    OS_NumWin* nw = num_from_hwnd(hwnd);
    int row;
    wchar_t wname[300], waddr[64], wval[80];
    if (!nw || !s) return;
    row = num_row_of(nw, s->var_id);
    if (row < 0) return;
    /* N10: 未勾选“实时更新”的行冻结，值保持不变，方便对比/手动写入 */
    if (!ListView_GetCheckState(nw->list, row)) return;
    os_utf8_to_wide_buf(os_fw_leaf_name(s->var_id) ? os_fw_leaf_name(s->var_id) : "", wname, 300);
    _snwprintf(waddr, 64, L"0x%llX", (unsigned long long)s->address);
    os_utf8_to_wide_buf(s->text, wval, 80);
    ListView_SetItemText(nw->list, row, NUM_COL_NAME, wname);
    ListView_SetItemText(nw->list, row, NUM_COL_ADDR, waddr);
    ListView_SetItemText(nw->list, row, NUM_COL_VAL, wval);
    /* min/max 跟随实时样本更新 */
    num_update_minmax(nw, row, s->value);
}

void os_num_add_var(HWND hwnd, int leaf_id)
{
    OS_NumWin* nw = num_from_hwnd(hwnd);
    LVITEMW item;
    const OS_Leaf* L;
    wchar_t wname[300], waddr[64];
    if (!nw) return;
    if (num_row_of(nw, leaf_id) >= 0 || nw->count >= OS_MAX_NUM_ROWS) return;
    L = os_vartree_leaf(leaf_id);
    os_utf8_to_wide_buf(L ? L->name : "?", wname, 300);
    _snwprintf(waddr, 64, L"0x%llX", (unsigned long long)(L ? L->address : 0));
    memset(&item, 0, sizeof(item));
    item.mask = LVIF_TEXT;
    item.pszText = L"";
    item.iItem = nw->count;
    item.iSubItem = 0;
    ListView_InsertItem(nw->list, &item);
    ListView_SetCheckState(nw->list, nw->count, TRUE); /* 默认实时更新 */
    ListView_SetItemText(nw->list, nw->count, NUM_COL_NAME, wname);
    ListView_SetItemText(nw->list, nw->count, NUM_COL_ADDR, waddr);
    ListView_SetItemText(nw->list, nw->count, NUM_COL_VAL, L"");
    nw->leaf_ids[nw->count] = leaf_id;
    _snprintf(nw->names[nw->count], 256, "%s", L ? L->name : "");
    num_reset_minmax(nw, nw->count); /* min/max 列初始 "-"，首个样本后开始记录 */
    nw->count++;
    /* N9(a): 加入窗口的变量自动纳入采集（观测勾选），否则多变量恒为 0 */
    os_vartree_set_watch(leaf_id, 1);
    os_vartree_set_check_ui(g_app.hTree, leaf_id, 1);
    os_mainwin_refresh_status();
    os_mainwin_update_buttons();
    os_log(OS_LOG_INFO, "数值窗口添加变量: id=%d (观测 %d)", leaf_id, g_app.watch_count);
}

/* N9(b): 移除第 row 行变量；若不再被任何窗口引用则自动取消观测 */
void os_num_remove_var(HWND hwnd, int row)
{
    OS_NumWin* nw = num_from_hwnd(hwnd);
    int removed_id, k;
    if (!nw || row < 0 || row >= nw->count) return;
    removed_id = nw->leaf_ids[row];
    if (nw->edit_row == row) num_end_edit(nw, 1);
    ListView_DeleteItem(nw->list, row);
    for (k = row; k < nw->count - 1; k++) {
        nw->leaf_ids[k] = nw->leaf_ids[k + 1];
        _snprintf(nw->names[k], 256, "%s", nw->names[k + 1]);
    }
    nw->count--;
    os_log(OS_LOG_INFO, "数值窗口移除变量: id=%d (剩余 %d 行)", removed_id, nw->count);
    os_win_auto_unwatch(removed_id);
}

int os_num_is(HWND hwnd)
{
    OS_NumWin* nw = num_from_hwnd(hwnd);
    return nw ? 1 : 0;
}

/* 需求2：ELF 重新加载后叶表重建、叶下标可能漂移——按变量全名重绑 leaf_id 并
 * 刷新地址列。旧实现只存下标，重编译增删变量后窗口会静默绑到错误变量。
 * 缺失变量的行移除（观测勾选由 os_vartree_build 按名恢复）。 */
void os_num_rebind(HWND hwnd)
{
    OS_NumWin* nw = num_from_hwnd(hwnd);
    int i, ok = 0, miss = 0;
    if (!nw) return;
    for (i = 0; i < nw->count; ) {
        int id;
        if (!nw->names[i][0]) { i++; continue; }
        id = os_vartree_find_by_name(nw->names[i]);
        if (id >= 0) {
            const OS_Leaf* L = os_vartree_leaf(id);
            wchar_t waddr[64];
            if (id != nw->leaf_ids[i])
                os_log(OS_LOG_INFO, "数值变量重绑: %s id=%d->%d @0x%llX",
                       nw->names[i], nw->leaf_ids[i], id,
                       (unsigned long long)(L ? L->address : 0));
            nw->leaf_ids[i] = id;
            _snwprintf(waddr, 64, L"0x%llX", (unsigned long long)(L ? L->address : 0));
            ListView_SetItemText(nw->list, i, NUM_COL_ADDR, waddr);
            ListView_SetItemText(nw->list, i, NUM_COL_VAL, L"");
            num_reset_minmax(nw, i); /* 重绑后地址可能已变，min/max 历史作废 */
            ok++;
            i++;
        } else {
            int k;
            os_log(OS_LOG_WARN, "数值变量重绑缺失（移除）: %s", nw->names[i]);
            if (nw->edit_row == i) num_end_edit(nw, 0);
            ListView_DeleteItem(nw->list, i);
            for (k = i; k < nw->count - 1; k++) {
                nw->leaf_ids[k] = nw->leaf_ids[k + 1];
                _snprintf(nw->names[k], 256, "%s", nw->names[k + 1]);
                if (nw->edit_row == k + 1) nw->edit_row = k;
            }
            nw->count--;
            miss++;
        }
    }
    if (ok || miss)
        os_log(OS_LOG_INFO, "数值窗口变量重绑: 成功 %d 缺失 %d", ok, miss);
}

/* F20: 按当前主题刷新数值窗口内部列表颜色 */
void os_num_apply_theme(HWND hwnd)
{
    OS_NumWin* nw = num_from_hwnd(hwnd);
    if (nw && nw->list && IsWindow(nw->list)) {
        ListView_SetBkColor(nw->list, os_theme(TH_LOG_BG));
        ListView_SetTextColor(nw->list, os_theme(TH_LOG_TEXT));
        ListView_SetTextBkColor(nw->list, os_theme(TH_LOG_BG));
        os_theme_listview_header(nw->list); /* 列头自绘主题色 */
        InvalidateRect(nw->list, NULL, TRUE);
    }
    if (nw) InvalidateRect(hwnd, NULL, TRUE);
}

int os_num_var_name(HWND hwnd, int idx, char* out, int cap)
{
    OS_NumWin* nw = num_from_hwnd(hwnd);
    const OS_Leaf* L;
    if (!nw || idx < 0 || idx >= nw->count || !out || cap <= 0) return 0;
    L = os_vartree_leaf(nw->leaf_ids[idx]);
    if (!L) return 0;
    _snprintf(out, cap, "%s", L->name);
    return 1;
}

/* ---------------- N10 就地编辑 ---------------- */

static void num_end_edit(OS_NumWin* nw, int commit)
{
    wchar_t wtext[512];
    char utf8[512], err[128];
    int row;
    HWND he;
    if (!nw || !nw->edit) return;
    he = nw->edit;
    row = nw->edit_row;
    GetWindowTextW(he, wtext, 512);
    nw->edit = NULL;
    nw->edit_row = -1;
    DestroyWindow(he);
    if (commit && row >= 0 && row < nw->count && wtext[0]) {
        err[0] = 0;
        os_wide_to_utf8_buf(wtext, utf8, sizeof(utf8));
        if (os_ds_write_leaf(nw->leaf_ids[row], utf8, err, sizeof(err)) == 0) {
            const OS_Leaf* L = os_vartree_leaf(nw->leaf_ids[row]);
            uint8_t raw[8];
            int sz = 0;
            OS_Sample tmp;
            /* 写入成功：回读样本会刷新该行（若未冻结）；冻结时直接显示写入值 */
            ListView_SetItemText(nw->list, row, NUM_COL_VAL, wtext);
            /* min/max 同步记录手动写入值（解析为数值后并入极值） */
            if (L && os_parse_text(utf8, raw, 8, &sz, L->kind, L->is_signed, 0, 0, 0,
                                   L->enums, L->enum_count)) {
                memset(&tmp, 0, sizeof(tmp));
                os_format_raw(tmp.text, sizeof(tmp.text), raw, sz, L->kind, L->is_signed,
                              L->is_ptr, L->is_bitfield, L->bit_offset, L->bit_size,
                              &tmp.value, L->enums, L->enum_count);
                num_update_minmax(nw, row, tmp.value);
            }
            os_log(OS_LOG_INFO, "数值窗口就地写入 行%d: %s", row, utf8);
        } else {
            os_log(OS_LOG_ERROR, "数值窗口写入失败: %s", err[0] ? err : "未知错误");
        }
    }
    if (IsWindow(nw->list)) SetFocus(nw->list);
}

static WNDPROC g_orig_edit_proc;

static LRESULT CALLBACK num_edit_proc(HWND he, UINT msg, WPARAM wParam, LPARAM lParam)
{
    OS_NumWin* nw = (OS_NumWin*)GetWindowLongPtrW(he, GWLP_USERDATA);
    if (nw && msg == WM_KEYDOWN && wParam == VK_RETURN) {
        num_end_edit(nw, 1);  /* Enter 一次写入后不再重复写入 */
        return 0;
    }
    if (nw && msg == WM_KEYDOWN && wParam == VK_ESCAPE) {
        num_end_edit(nw, 0);  /* Esc 取消 */
        return 0;
    }
    if (nw && msg == WM_KILLFOCUS) {
        num_end_edit(nw, 1);  /* 点击别处视为提交 */
        return 0;
    }
    return CallWindowProcW(g_orig_edit_proc, he, msg, wParam, lParam);
}

static void num_start_edit(OS_NumWin* nw, int row)
{
    RECT rc;
    wchar_t wtext[256];
    HWND he;
    HFONT hf;
    if (!nw || !nw->list || row < 0 || row >= nw->count) return;
    if (nw->edit) num_end_edit(nw, 1);
    if (!ListView_GetSubItemRect(nw->list, row, NUM_COL_VAL, LVIR_BOUNDS, &rc)) return;
    he = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                         WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                         rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                         nw->list, NULL, g_app.hInst, NULL);
    if (!he) return;
    nw->edit = he;
    nw->edit_row = row;
    SetWindowLongPtrW(he, GWLP_USERDATA, (LONG_PTR)nw);
    ListView_GetItemText(nw->list, row, NUM_COL_VAL, wtext, 256);
    SetWindowTextW(he, wtext);
    hf = (HFONT)SendMessageW(nw->list, WM_GETFONT, 0, 0);
    if (hf) SendMessageW(he, WM_SETFONT, (WPARAM)hf, TRUE);
    g_orig_edit_proc = (WNDPROC)SetWindowLongPtrW(he, GWLP_WNDPROC, (LONG_PTR)num_edit_proc);
    ShowWindow(he, SW_SHOW);
    SetFocus(he);
    SendMessageW(he, EM_SETSEL, 0, -1);
}

static LRESULT CALLBACK num_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    OS_NumWin* nw = num_from_hwnd(hwnd);
    switch (msg) {
    case WM_NCCREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        OS_NumWin* p = (OS_NumWin*)calloc(1, sizeof(OS_NumWin));
        LVCOLUMNW col;
        if (!p) return FALSE;
        p->magic = OS_MAGIC_NUM;
        p->hwnd = hwnd;
        p->edit_row = -1;
        if (cs->lpszName) _snwprintf(p->title, 128, L"%s", cs->lpszName);
        else _snwprintf(p->title, 128, L"数值窗口");
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)p);
        p->list = CreateWindowW(WC_LISTVIEWW, L"",
                                WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS |
                                WS_HSCROLL,
                                0, 0, 10, 10, hwnd, NULL, g_app.hInst, NULL);
        ListView_SetExtendedListViewStyle(p->list,
            LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
        memset(&col, 0, sizeof(col));
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
        col.fmt = LVCFMT_CENTER;
        col.cx = 56;
        col.pszText = L"实时";
        ListView_InsertColumn(p->list, NUM_COL_CHECK, &col);
        col.fmt = LVCFMT_LEFT;
        col.cx = 190;
        col.pszText = L"变量";
        ListView_InsertColumn(p->list, NUM_COL_NAME, &col);
        col.cx = 90;
        col.pszText = L"地址";
        ListView_InsertColumn(p->list, NUM_COL_ADDR, &col);
        col.cx = 120;
        col.pszText = L"数值";
        ListView_InsertColumn(p->list, NUM_COL_VAL, &col);
        /* 用户反馈：min/max 列——记录运行过程中变量的最小/最大值 */
        col.fmt = LVCFMT_RIGHT;
        col.cx = 100;
        col.pszText = L"最小值";
        ListView_InsertColumn(p->list, NUM_COL_MIN, &col);
        col.pszText = L"最大值";
        ListView_InsertColumn(p->list, NUM_COL_MAX, &col);
        SendMessage(p->list, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        os_num_apply_theme(hwnd); /* F20: 应用当前主题颜色 */
        return TRUE;
    }
    case WM_NCDESTROY:
        if (nw) free(nw);
        return 0;
    case WM_OS_NUM_TEST_DUMP:
        /* 测试钩子：逐行日志输出 min/max（跨进程无法读 ListView 文本，回归断言用日志） */
        if (nw) {
            int k;
            for (k = 0; k < nw->count; k++) {
                if (nw->has_mm[k])
                    os_log(OS_LOG_INFO, "数值minmax: 行%d min=%g max=%g",
                           k, nw->vmin[k], nw->vmax[k]);
                else
                    os_log(OS_LOG_INFO, "数值minmax: 行%d 无样本", k);
            }
        }
        return 0;
    case WM_SIZE: {
        if (nw && nw->list) MoveWindow(nw->list, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        HBRUSH br;
        GetClientRect(hwnd, &rc);
        br = CreateSolidBrush(os_theme(TH_PANEL));
        FillRect(hdc, &rc, br);
        DeleteObject(br);
        FrameRect(hdc, &rc, os_theme_brush(TH_BORDER));
        if (nw) {
            SetBkMode(hdc, TRANSPARENT);
            SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
            SetTextColor(hdc, os_theme(TH_TEXT));
            RECT tr = { 4, 2, rc.right - 22, 24 };
            DrawTextW(hdc, nw->title, -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            RECT xr = { rc.right - 20, 2, rc.right - 4, 24 };
            DrawTextW(hdc, L"×", -1, &xr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_SETFOCUS:
        os_win_mark_active(hwnd); /* N11/Bug3: 登记为“当前窗口” */
        return 0;
    case WM_LBUTTONDBLCLK: {
        /* Bug3: 双击标题栏（非关闭按钮区域）切换全屏 */
        RECT rc;
        GetClientRect(hwnd, &rc);
        if ((short)HIWORD(lParam) < 24 && (short)LOWORD(lParam) < rc.right - 22)
            PostMessage(g_app.hMain, WM_OS_WIN_FULLSCREEN, (WPARAM)hwnd, 0);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        int x = (short)LOWORD(lParam);
        RECT rc;
        GetClientRect(hwnd, &rc);
        if (x >= rc.right - 22 && (short)HIWORD(lParam) < 24) {
            PostMessage(g_app.hMain, WM_OS_WIN_CLOSED, (WPARAM)hwnd, 0);
        }
        return 0;
    }
    case WM_NOTIFY: {
        LPNMHDR h = (LPNMHDR)lParam;
        if (h && h->hwndFrom == (nw ? nw->list : NULL)) {
            if (h->code == NM_DBLCLK) {
                LPNMITEMACTIVATE ia = (LPNMITEMACTIVATE)lParam;
                if (ia->iItem >= 0 && ia->iItem < nw->count) {
                    if (ia->iSubItem == NUM_COL_VALIDX) num_start_edit(nw, ia->iItem);
                    else if (ia->iSubItem == NUM_COL_CHECK) {
                        /* 点击勾选列不编辑，仅切换实时状态（系统已处理） */
                    }
                }
                return 0;
            }
            if (h->code == NM_RCLICK) {
                HMENU m = CreatePopupMenu();
                POINT pt;
                int row = ListView_GetNextItem(nw->list, -1, LVNI_SELECTED);
                AppendMenuW(m, MF_STRING, MENU_NUM_ADD, L"添加变量...");
                AppendMenuW(m, MF_STRING | (row < 0 ? MF_GRAYED : 0), MENU_NUM_REMOVE, L"移除选中变量");
                AppendMenuW(m, MF_STRING | (row < 0 ? MF_GRAYED : 0), MENU_NUM_WRITE, L"写入值（就地编辑）");
                AppendMenuW(m, MF_SEPARATOR, 0, NULL);
                AppendMenuW(m, MF_STRING, MENU_NUM_CLOSE, L"关闭窗口");
                GetCursorPos(&pt);
                TrackPopupMenu(m, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(m);
                return 0;
            }
        }
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case MENU_NUM_ADD: {
            /* N13a: 多选：一次添加全部选中变量 */
            int ids[16], n = 0, i;
            if (os_dlg_pick_vars(hwnd, ids, 16, &n) == 0) {
                for (i = 0; i < n && i < 16; i++) os_num_add_var(hwnd, ids[i]);
                os_log(OS_LOG_INFO, "数值窗口批量添加变量: %d 个", n);
            }
            break;
        }
        case MENU_NUM_REMOVE: {
            int row = nw ? ListView_GetNextItem(nw->list, -1, LVNI_SELECTED) : -1;
            if (nw && row >= 0) os_num_remove_var(hwnd, row);
            break;
        }
        case MENU_NUM_WRITE: {
            int row = nw ? ListView_GetNextItem(nw->list, -1, LVNI_SELECTED) : -1;
            if (nw && row >= 0 && row < nw->count) num_start_edit(nw, row);
            break;
        }
        case MENU_NUM_CLOSE:
            PostMessage(g_app.hMain, WM_OS_WIN_CLOSED, (WPARAM)hwnd, 0);
            break;
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void os_num_register(void)
{
    WNDCLASSW wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = num_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = g_num_class;
    RegisterClassW(&wc);
}

HWND os_num_create(HWND parent, int x, int y, int w, int h, const wchar_t* title)
{
    HWND hw = CreateWindowW(g_num_class, title ? title : L"数值窗口",
                            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                            x, y, w, h, parent, NULL, g_app.hInst, NULL);
    return hw;
}
