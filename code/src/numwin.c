#include "app.h"
#include "numwin.h"
#include "mainwin.h"
#include "vartree.h"
#include <commctrl.h>
#include <string.h>

#define OS_MAGIC_NUM 0x4E554D31u /* 'NUM1' */

typedef struct OS_NumWin {
    DWORD magic;
    HWND hwnd;
    HWND list;
    wchar_t title[128];
    int leaf_ids[OS_MAX_NUM_ROWS];
    int count;
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

void os_num_push(HWND hwnd, const OS_Sample* s)
{
    OS_NumWin* nw = num_from_hwnd(hwnd);
    int row;
    wchar_t wname[300], waddr[64], wval[80];
    if (!nw || !s) return;
    row = num_row_of(nw, s->var_id);
    if (row < 0) return;
    os_utf8_to_wide_buf(os_fw_leaf_name(s->var_id) ? os_fw_leaf_name(s->var_id) : "", wname, 300);
    _snwprintf(waddr, 64, L"0x%llX", (unsigned long long)s->address);
    os_utf8_to_wide_buf(s->text, wval, 80);
    ListView_SetItemText(nw->list, row, 0, wname);
    ListView_SetItemText(nw->list, row, 1, waddr);
    ListView_SetItemText(nw->list, row, 2, wval);
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
    item.pszText = wname;
    item.iItem = nw->count;
    item.iSubItem = 0;
    ListView_InsertItem(nw->list, &item);
    ListView_SetItemText(nw->list, nw->count, 1, waddr);
    ListView_SetItemText(nw->list, nw->count, 2, L"");
    nw->leaf_ids[nw->count] = leaf_id;
    nw->count++;
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
        if (cs->lpszName) _snwprintf(p->title, 128, L"%s", cs->lpszName);
        else _snwprintf(p->title, 128, L"数值窗口");
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)p);
        p->list = CreateWindowW(WC_LISTVIEWW, L"",
                                WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
                                0, 0, 10, 10, hwnd, NULL, g_app.hInst, NULL);
        ListView_SetExtendedListViewStyle(p->list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        memset(&col, 0, sizeof(col));
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
        col.cx = 220;
        col.fmt = LVCFMT_LEFT;
        col.pszText = L"变量";
        ListView_InsertColumn(p->list, 0, &col);
        col.cx = 100;
        col.pszText = L"地址";
        ListView_InsertColumn(p->list, 1, &col);
        col.cx = 160;
        col.pszText = L"数值";
        ListView_InsertColumn(p->list, 2, &col);
        SendMessage(p->list, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        return TRUE;
    }
    case WM_NCDESTROY:
        if (nw) free(nw);
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
        br = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        FillRect(hdc, &rc, br);
        DeleteObject(br);
        FrameRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
        if (nw) {
            SetBkMode(hdc, TRANSPARENT);
            SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
            SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
            RECT tr = { 4, 2, rc.right - 22, 24 };
            DrawTextW(hdc, nw->title, -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            RECT xr = { rc.right - 20, 2, rc.right - 4, 24 };
            DrawTextW(hdc, L"×", -1, &xr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        int x = (short)LOWORD(lParam);
        RECT rc;
        GetClientRect(hwnd, &rc);
        if (x >= rc.right - 22 && (short)HIWORD(lParam) < 24) {
            PostMessage(GetParent(hwnd), WM_OS_WIN_CLOSED, (WPARAM)hwnd, 0);
        }
        return 0;
    }
    case WM_NOTIFY: {
        LPNMHDR h = (LPNMHDR)lParam;
        if (h && h->hwndFrom == (nw ? nw->list : NULL)) {
            if (h->code == NM_DBLCLK) {
                int row = ListView_GetNextItem(nw->list, -1, LVNI_SELECTED);
                if (row >= 0 && row < nw->count) os_dlg_edit_value(hwnd, nw->leaf_ids[row]);
                return 0;
            }
            if (h->code == NM_RCLICK) {
                HMENU m = CreatePopupMenu();
                POINT pt;
                int row = ListView_GetNextItem(nw->list, -1, LVNI_SELECTED);
                AppendMenuW(m, MF_STRING, MENU_NUM_ADD, L"添加变量...");
                AppendMenuW(m, MF_STRING | (row < 0 ? MF_GRAYED : 0), MENU_NUM_REMOVE, L"移除选中变量");
                AppendMenuW(m, MF_STRING | (row < 0 ? MF_GRAYED : 0), MENU_NUM_WRITE, L"写入值...");
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
            int id = -1;
            if (os_dlg_pick_var(hwnd, &id) == 0 && id >= 0) os_num_add_var(hwnd, id);
            break;
        }
        case MENU_NUM_REMOVE: {
            int row = nw ? ListView_GetNextItem(nw->list, -1, LVNI_SELECTED) : -1;
            if (nw && row >= 0 && row < nw->count) {
                int k;
                ListView_DeleteItem(nw->list, row);
                for (k = row; k < nw->count - 1; k++) nw->leaf_ids[k] = nw->leaf_ids[k + 1];
                nw->count--;
            }
            break;
        }
        case MENU_NUM_WRITE: {
            int row = nw ? ListView_GetNextItem(nw->list, -1, LVNI_SELECTED) : -1;
            if (nw && row >= 0 && row < nw->count) os_dlg_edit_value(hwnd, nw->leaf_ids[row]);
            break;
        }
        case MENU_NUM_CLOSE:
            PostMessage(GetParent(hwnd), WM_OS_WIN_CLOSED, (WPARAM)hwnd, 0);
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
