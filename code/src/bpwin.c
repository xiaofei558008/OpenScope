/*
 * 硬件断点面板：调用当前驱动 OS_CMD_BP_SET / CLEAR / LIST。
 */
#include "bpwin.h"
#include "app.h"
#include "module_api.h"
#include "util.h"
#include "srcwin.h"

#include <stdio.h>
#include <string.h>

#define IDC_BP_LIST    5101
#define IDC_BP_ADDR    5102
#define IDC_BP_NOTE    5103
#define IDC_BP_SET     5104
#define IDC_BP_CLEAR   5105
#define IDC_BP_CLRALL  5106
#define IDC_BP_REFRESH 5107
#define IDC_BP_CLOSE   5108
#define IDC_BP_STATUS  5109

/* 宿主侧备注（驱动不存 note） */
static struct {
    uint32_t addr;
    char     note[64];
} g_bp_notes[OS_BP_MAX];
static int g_bp_note_n;

static void bp_note_set(uint32_t addr, const char* note)
{
    int i;
    addr &= ~1u;
    for (i = 0; i < g_bp_note_n; i++) {
        if ((g_bp_notes[i].addr & ~1u) == addr) {
            if (note && note[0])
                _snprintf(g_bp_notes[i].note, sizeof(g_bp_notes[i].note), "%s", note);
            else
                g_bp_notes[i].note[0] = 0;
            return;
        }
    }
    if (!note || !note[0]) return;
    if (g_bp_note_n >= OS_BP_MAX) return;
    g_bp_notes[g_bp_note_n].addr = addr;
    _snprintf(g_bp_notes[g_bp_note_n].note, sizeof(g_bp_notes[0].note), "%s", note);
    g_bp_note_n++;
}

static void bp_note_del(uint32_t addr)
{
    int i, j;
    addr &= ~1u;
    for (i = 0; i < g_bp_note_n; i++) {
        if ((g_bp_notes[i].addr & ~1u) != addr) continue;
        for (j = i; j < g_bp_note_n - 1; j++)
            g_bp_notes[j] = g_bp_notes[j + 1];
        g_bp_note_n--;
        return;
    }
}

static const char* bp_note_get(uint32_t addr)
{
    int i;
    addr &= ~1u;
    for (i = 0; i < g_bp_note_n; i++)
        if ((g_bp_notes[i].addr & ~1u) == addr) return g_bp_notes[i].note;
    return "";
}

static void bp_set_status(HWND hwnd, const wchar_t* text)
{
    HWND s = GetDlgItem(hwnd, IDC_BP_STATUS);
    if (s) SetWindowTextW(s, text ? text : L"");
}

static int bp_parse_addr(HWND hwnd, uint32_t* out)
{
    wchar_t w[64];
    char a[64];
    unsigned long v;
    char* end = NULL;
    GetWindowTextW(GetDlgItem(hwnd, IDC_BP_ADDR), w, 64);
    os_wide_to_utf8_buf(w, a, sizeof(a));
    if (!a[0]) return -1;
    v = strtoul(a, &end, 0);
    if (end == a) return -1;
    *out = (uint32_t)v;
    return 0;
}

static void bp_refresh_list(HWND hwnd)
{
    HWND lb = GetDlgItem(hwnd, IDC_BP_LIST);
    OS_BpInfo items[OS_BP_MAX];
    OS_BpList list;
    int i, rc;
    wchar_t line[160];
    char note[64];
    if (!lb) return;
    SendMessageW(lb, LB_RESETCONTENT, 0, 0);
    if (!g_app.driver || !g_app.connected || !g_app.driver->command) {
        bp_set_status(hwnd, L"未连接驱动");
        return;
    }
    memset(items, 0, sizeof(items));
    memset(&list, 0, sizeof(list));
    list.items = items;
    list.capacity = OS_BP_MAX;
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_LIST, NULL, &list);
    if (rc != OS_ERR_OK) {
        bp_set_status(hwnd, L"BP_LIST 失败（驱动可能不支持）");
        return;
    }
    for (i = 0; i < list.count; i++) {
        _snprintf(note, sizeof(note), "%s", bp_note_get(items[i].address));
        if (note[0])
            _snwprintf(line, 160, L"[%d] 0x%08X  %hs", items[i].id,
                       (unsigned)items[i].address, note);
        else
            _snwprintf(line, 160, L"[%d] 0x%08X", items[i].id,
                       (unsigned)items[i].address);
        SendMessageW(lb, LB_ADDSTRING, 0, (LPARAM)line);
        SendMessageW(lb, LB_SETITEMDATA, (WPARAM)i, (LPARAM)(ULONG_PTR)items[i].address);
    }
    {
        wchar_t st[64];
        _snwprintf(st, 64, L"共 %d 个断点", list.count);
        bp_set_status(hwnd, st);
    }
}

static void bp_do_set(HWND hwnd)
{
    OS_BpReq req;
    uint32_t addr;
    wchar_t wn[64];
    char note[64];
    int rc;
    if (bp_parse_addr(hwnd, &addr) != 0 || !addr) {
        bp_set_status(hwnd, L"请输入有效地址（如 0x08001234）");
        return;
    }
    if (!g_app.driver || !g_app.connected) {
        bp_set_status(hwnd, L"未连接");
        return;
    }
    memset(&req, 0, sizeof(req));
    req.address = addr;
    GetWindowTextW(GetDlgItem(hwnd, IDC_BP_NOTE), wn, 64);
    os_wide_to_utf8_buf(wn, note, sizeof(note));
    _snprintf(req.note, sizeof(req.note), "%s", note);
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_SET, &req, NULL);
    if (rc != OS_ERR_OK) {
        bp_set_status(hwnd, L"设置失败（槽满或不支持）");
        os_log(OS_LOG_ERROR, "断点设置失败: 0x%08X rc=%d", addr, rc);
        return;
    }
    bp_note_set(addr, note);
    os_log(OS_LOG_INFO, "断点已设置: 0x%08X", addr & ~1u);
    os_src_refresh_bps();
    bp_refresh_list(hwnd);
}

static void bp_do_clear_sel(HWND hwnd)
{
    HWND lb = GetDlgItem(hwnd, IDC_BP_LIST);
    int sel;
    OS_BpReq req;
    uint32_t addr;
    int rc;
    if (!lb || !g_app.driver || !g_app.connected) return;
    sel = (int)SendMessageW(lb, LB_GETCURSEL, 0, 0);
    if (sel < 0) {
        /* 无选中则尝试用地址框 */
        if (bp_parse_addr(hwnd, &addr) != 0 || !addr) {
            bp_set_status(hwnd, L"请选中列表项或输入地址");
            return;
        }
    } else {
        addr = (uint32_t)(ULONG_PTR)SendMessageW(lb, LB_GETITEMDATA, (WPARAM)sel, 0);
    }
    memset(&req, 0, sizeof(req));
    req.address = addr;
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_CLEAR, &req, NULL);
    if (rc != OS_ERR_OK) {
        bp_set_status(hwnd, L"清除失败");
        return;
    }
    bp_note_del(addr);
    os_log(OS_LOG_INFO, "断点已清除: 0x%08X", addr & ~1u);
    os_src_refresh_bps();
    bp_refresh_list(hwnd);
}

static void bp_do_clear_all(HWND hwnd)
{
    OS_BpReq req;
    int rc;
    if (!g_app.driver || !g_app.connected) return;
    if (os_msgbox(hwnd, L"清除全部硬件断点？", L"断点", MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;
    memset(&req, 0, sizeof(req));
    req.clear_all = 1;
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_CLEAR, &req, NULL);
    if (rc != OS_ERR_OK) {
        bp_set_status(hwnd, L"全部清除失败");
        return;
    }
    g_bp_note_n = 0;
    os_log(OS_LOG_INFO, "断点已全部清除");
    os_src_refresh_bps();
    bp_refresh_list(hwnd);
}

typedef struct {
    int done;
} BpDlg;

static const wchar_t* k_bp_cls = L"OSDlgBreakpoints";

static LRESULT CALLBACK bp_wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    BpDlg* st;
    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        HWND c;
        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        st = (BpDlg*)cs->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)st);

        CreateWindowW(L"STATIC", L"地址", WS_CHILD | WS_VISIBLE,
                      12, 14, 40, 20, hwnd, NULL, cs->hInstance, NULL);
        c = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"0x08000000",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                            54, 12, 140, 24, hwnd, (HMENU)IDC_BP_ADDR, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);

        CreateWindowW(L"STATIC", L"备注", WS_CHILD | WS_VISIBLE,
                      206, 14, 40, 20, hwnd, NULL, cs->hInstance, NULL);
        c = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                            248, 12, 160, 24, hwnd, (HMENU)IDC_BP_NOTE, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);

        c = CreateWindowW(L"BUTTON", L"设置", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
                          420, 10, 70, 28, hwnd, (HMENU)IDC_BP_SET, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);

        c = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", L"",
                            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | WS_TABSTOP,
                            12, 48, 478, 200, hwnd, (HMENU)IDC_BP_LIST, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);

        c = CreateWindowW(L"BUTTON", L"清除选中", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                          12, 260, 90, 28, hwnd, (HMENU)IDC_BP_CLEAR, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);
        c = CreateWindowW(L"BUTTON", L"全部清除", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                          110, 260, 90, 28, hwnd, (HMENU)IDC_BP_CLRALL, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);
        c = CreateWindowW(L"BUTTON", L"刷新", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                          210, 260, 70, 28, hwnd, (HMENU)IDC_BP_REFRESH, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);
        c = CreateWindowW(L"BUTTON", L"关闭", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                          420, 260, 70, 28, hwnd, (HMENU)IDC_BP_CLOSE, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);

        c = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE,
                          12, 298, 478, 22, hwnd, (HMENU)IDC_BP_STATUS, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);

        bp_refresh_list(hwnd);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDC_BP_SET: bp_do_set(hwnd); return 0;
        case IDC_BP_CLEAR: bp_do_clear_sel(hwnd); return 0;
        case IDC_BP_CLRALL: bp_do_clear_all(hwnd); return 0;
        case IDC_BP_REFRESH: bp_refresh_list(hwnd); return 0;
        case IDC_BP_CLOSE:
        case IDCANCEL:
            st = (BpDlg*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            if (st) st->done = 1;
            DestroyWindow(hwnd);
            return 0;
        case IDC_BP_LIST:
            if (HIWORD(wp) == LBN_DBLCLK) {
                bp_do_clear_sel(hwnd);
                return 0;
            }
            if (HIWORD(wp) == LBN_SELCHANGE) {
                HWND lb = GetDlgItem(hwnd, IDC_BP_LIST);
                int sel = (int)SendMessageW(lb, LB_GETCURSEL, 0, 0);
                wchar_t a[32];
                uint32_t addr;
                if (sel >= 0) {
                    addr = (uint32_t)(ULONG_PTR)SendMessageW(lb, LB_GETITEMDATA, (WPARAM)sel, 0);
                    _snwprintf(a, 32, L"0x%08X", (unsigned)addr);
                    SetWindowTextW(GetDlgItem(hwnd, IDC_BP_ADDR), a);
                    {
                        wchar_t wn[64];
                        os_utf8_to_wide_buf(bp_note_get(addr), wn, 64);
                        SetWindowTextW(GetDlgItem(hwnd, IDC_BP_NOTE), wn);
                    }
                }
            }
            return 0;
        }
        break;
    case WM_CLOSE:
        st = (BpDlg*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        if (st) st->done = 1;
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void os_bpwin_dialog(HWND owner)
{
    BpDlg st;
    HWND hwnd;
    MSG msg;
    WNDCLASSW wc;
    static int reg;
    memset(&st, 0, sizeof(st));
    if (!reg) {
        memset(&wc, 0, sizeof(wc));
        wc.lpfnWndProc = bp_wnd_proc;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = k_bp_cls;
        RegisterClassW(&wc);
        reg = 1;
    }
    hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME, k_bp_cls, L"断点",
                           WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                           (GetSystemMetrics(SM_CXSCREEN) - 520) / 2,
                           (GetSystemMetrics(SM_CYSCREEN) - 360) / 2,
                           520, 360, owner, NULL, GetModuleHandleW(NULL), &st);
    if (!hwnd) return;
    EnableWindow(owner, FALSE);
    while (!st.done && IsWindow(hwnd) && GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (!IsDialogMessageW(hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    EnableWindow(owner, TRUE);
    SetForegroundWindow(owner);
}
