/*
 * OpenScope UI: main window, tree view, log, dialogs.
 */
#include "app.h"

#include <commctrl.h>
#include <commdlg.h>
#include <string.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")

#define IDC_TREE   1001
#define IDC_LOG    1002
#define IDC_STATUS 1003

#define IDC_BTN_ELF     1010
#define IDC_BTN_SCAN    1011
#define IDC_BTN_CONN    1012
#define IDC_BTN_DISC    1013
#define IDC_BTN_START   1014
#define IDC_BTN_STOP    1015
#define IDC_BTN_REPLAY  1016
#define IDC_BTN_SCOPE   1017
#define IDC_BTN_NUM     1018

#define IDM_FILE_LOADELF 2001
#define IDM_FILE_REPLAY  2002
#define IDM_FILE_EXIT    2003
#define IDM_OP_SCAN      2010
#define IDM_OP_CONNECT   2011
#define IDM_OP_DISCONNECT 2012
#define IDM_OP_START     2013
#define IDM_OP_STOP      2014
#define IDM_OP_WRITE     2015
#define IDM_OP_HALT      2016
#define IDM_OP_GO        2017
#define IDM_RATE_BASE    2020
#define IDM_HELP_ABOUT   2030
#define IDM_WIN_BASE     0x4000

/* 对话框控件 ID */
#define IDC_D_EDIT     3001
#define IDC_D_LIST     3002
#define IDC_D_OK       3003
#define IDC_D_CANCEL   3004
#define IDC_D_RADIO_SWD 3005
#define IDC_D_RADIO_JTAG 3006
#define IDC_D_SPEED    3007
#define IDC_D_DEVICE   3008
#define IDC_D_SERIAL   3009
#define IDC_D_RESET    3010
#define IDC_D_SCAN     3011
#define IDC_D_VALUE    3012
#define IDC_D_NAME     3013
#define IDC_D_STATIC   3014

static HANDLE g_dialog_done = NULL;
static int g_pick_ok = 0;
static char g_pick_result[256];
static int g_write_leaf_id = -1;
static char g_write_leaf_name[256];

static HFONT g_font = NULL;

OS_App g_app;

/* ------------------------- UTF-8 helpers -------------------------- */

static void utf8_to_wide(const char* s, wchar_t* w, int cap)
{
    MultiByteToWideChar(CP_UTF8, 0, s, -1, w, cap);
}

static void wide_to_utf8(const wchar_t* w, char* s, int cap)
{
    WideCharToMultiByte(CP_UTF8, 0, w, -1, s, cap, NULL, NULL);
}

/* ------------------------- log / status --------------------------- */

void os_log(int level, const char* fmt, ...)
{
    char buf[1024];
    char* copy;
    va_list ap;
    const char* tag = "[I]";
    if (level == OS_LOG_WARN) tag = "[W]";
    else if (level == OS_LOG_ERROR) tag = "[E]";
    else if (level == OS_LOG_DEBUG) tag = "[D]";
    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    buf[sizeof(buf) - 1] = 0;
    if (!g_app.hMain) {
        OutputDebugStringA(buf);
        return;
    }
    copy = (char*)malloc(strlen(buf) + strlen(tag) + 4);
    if (!copy) return;
    _snprintf(copy, strlen(buf) + strlen(tag) + 4, "%s %s", tag, buf);
    if (!PostMessageW(g_app.hMain, WM_APP_LOG_MSG, 0, (LPARAM)copy)) {
        free(copy);
    }
}

void os_status(const char* fmt, ...)
{
    char buf[512];
    char* copy;
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    buf[sizeof(buf) - 1] = 0;
    if (!g_app.hMain) return;
    copy = (char*)malloc(strlen(buf) + 1);
    if (!copy) return;
    strcpy(copy, buf);
    if (!PostMessageW(g_app.hMain, WM_APP_STATUS_MSG, 0, (LPARAM)copy))
        free(copy);
}

/* ------------------------- dialog plumbing ------------------------ */

static void dialog_pump(HWND dlg, HWND parent)
{
    if (parent) EnableWindow(parent, FALSE);
    if (g_dialog_done) ResetEvent(g_dialog_done);
    while (1) {
        DWORD r = MsgWaitForMultipleObjects(1, &g_dialog_done, FALSE, INFINITE, QS_ALLINPUT);
        if (r == WAIT_OBJECT_0) break;
        {
            MSG msg;
            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    if (parent) EnableWindow(parent, TRUE);
                    PostQuitMessage((int)msg.wParam);
                    return;
                }
                if (!IsDialogMessageW(dlg, &msg)) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
        }
    }
    if (parent) {
        EnableWindow(parent, TRUE);
        SetForegroundWindow(parent);
    }
}

static HWND dlg_make(HINSTANCE hInst, const wchar_t* cls, const wchar_t* title,
                     HWND parent, int w, int h)
{
    RECT rc;
    if (parent) GetWindowRect(parent, &rc);
    else { rc.left = 0; rc.top = 0; rc.right = 640; rc.bottom = 480; }
    return CreateWindowExW(WS_EX_DLGMODALFRAME, cls, title,
                           WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                           rc.left + (rc.right - rc.left - w) / 2,
                           rc.top + (rc.bottom - rc.top - h) / 2,
                           w, h, parent, NULL, hInst, NULL);
}

static HWND dlg_ctl(HWND parent, const wchar_t* cls, const wchar_t* text,
                    DWORD style, int id, int x, int y, int w, int h)
{
    return CreateWindowExW(0, cls, text,
                           WS_CHILD | WS_VISIBLE | style,
                           x, y, w, h, parent, (HMENU)(INT_PTR)id,
                           g_app.hInst, NULL);
}

static void dlg_done_close(HWND dlg)
{
    DestroyWindow(dlg);
}

/* ------------------------- add-variable dialog -------------------- */

static LRESULT CALLBACK pick_proc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE: {
        dlg_ctl(h, L"STATIC", L"搜索变量（支持模糊匹配）:", 0, IDC_D_STATIC, 10, 8, 220, 18);
        dlg_ctl(h, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, IDC_D_EDIT, 10, 28, 260, 22);
        dlg_ctl(h, L"LISTBOX", L"", WS_BORDER | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, IDC_D_LIST, 10, 56, 260, 200);
        dlg_ctl(h, L"BUTTON", L"确定", BS_DEFPUSHBUTTON | WS_TABSTOP, IDC_D_OK, 120, 268, 70, 26);
        dlg_ctl(h, L"BUTTON", L"取消", BS_PUSHBUTTON | WS_TABSTOP, IDC_D_CANCEL, 200, 268, 70, 26);
        SetTimer(h, 1, 250, NULL);
        g_pick_ok = 0;
        g_pick_result[0] = 0;
        SetFocus(GetDlgItem(h, IDC_D_EDIT));
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == IDC_D_OK || LOWORD(wp) == IDC_D_LIST) {
            if (HIWORD(wp) == LBN_DBLCLK || LOWORD(wp) == IDC_D_OK) {
                HWND lb = GetDlgItem(h, IDC_D_LIST);
                int sel = (int)SendMessageW(lb, LB_GETCURSEL, 0, 0);
                if (sel == LB_ERR) break;
                SendMessageW(lb, LB_GETTEXT, sel, (LPARAM)g_pick_result);
                g_pick_ok = 1;
                dlg_done_close(h);
            }
        } else if (LOWORD(wp) == IDC_D_CANCEL) {
            dlg_done_close(h);
        }
        return 0;
    case WM_TIMER: {
        char needle[256];
        char text[256];
        int ids[512];
        int n, i;
        HWND lb = GetDlgItem(h, IDC_D_LIST);
        GetWindowTextA(GetDlgItem(h, IDC_D_EDIT), needle, sizeof(needle));
        SendMessageW(lb, LB_RESETCONTENT, 0, 0);
        if (needle[0] == 0) {
            n = g_app.leaf_count < 512 ? g_app.leaf_count : 512;
            for (i = 0; i < n; ++i) ids[i] = i;
        } else {
            n = fw_leaf_find_local(needle, ids, 512);
        }
        for (i = 0; i < n; ++i) {
            OS_Leaf* L = &g_app.leaves[ids[i]];
            _snprintf(text, sizeof(text), "%s  @0x%llX", L->path,
                      (unsigned long long)L->address);
            {
                wchar_t wt[320];
                utf8_to_wide(text, wt, 320);
                SendMessageW(lb, LB_ADDSTRING, 0, (LPARAM)wt);
            }
        }
        return 0;
    }
    case WM_CLOSE:
        dlg_done_close(h);
        return 0;
    case WM_DESTROY:
        KillTimer(h, 1);
        if (g_dialog_done) SetEvent(g_dialog_done);
        return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

static int fw_leaf_find_local(const char* needle, int* ids, int max_ids);

int ui_pick_variable(HWND parent, char* out, int out_len)
{
    HWND dlg;
    if (!g_app.elf) {
        MessageBoxW(parent, L"请先加载 ELF 文件", L"添加变量", MB_OK | MB_ICONINFORMATION);
        return 0;
    }
    dlg = dlg_make(g_app.hInst, L"OSPickVarDlg", L"添加变量", parent, 280, 306);
    if (!dlg) return 0;
    dialog_pump(dlg, parent);
    if (g_pick_ok && out && out_len > 0)
        _snprintf(out, out_len, "%s", g_pick_result);
    return g_pick_ok;
}

/* ------------------------- connect dialog ------------------------- */

static LRESULT CALLBACK connect_proc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE: {
        dlg_ctl(h, L"BUTTON", L"SWD", BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP, IDC_D_RADIO_SWD, 14, 12, 60, 20);
        dlg_ctl(h, L"BUTTON", L"JTAG", BS_AUTORADIOBUTTON | WS_TABSTOP, IDC_D_RADIO_JTAG, 78, 12, 60, 20);
        dlg_ctl(h, L"STATIC", L"时钟 (kHz):", 0, IDC_D_STATIC, 14, 40, 90, 18);
        dlg_ctl(h, L"EDIT", L"4000", WS_BORDER | WS_TABSTOP | ES_NUMBER, IDC_D_SPEED, 110, 38, 90, 22);
        dlg_ctl(h, L"STATIC", L"目标器件:", 0, IDC_D_STATIC, 14, 68, 90, 18);
        dlg_ctl(h, L"EDIT", L"", WS_BORDER | WS_TABSTOP, IDC_D_DEVICE, 110, 66, 130, 22);
        dlg_ctl(h, L"STATIC", L"序列号(可选):", 0, IDC_D_STATIC, 14, 96, 90, 18);
        dlg_ctl(h, L"EDIT", L"", WS_BORDER | WS_TABSTOP, IDC_D_SERIAL, 110, 94, 130, 22);
        dlg_ctl(h, L"BUTTON", L"复位后连接 (connect under reset)", BS_AUTOCHECKBOX | WS_TABSTOP, IDC_D_RESET, 14, 124, 220, 20);
        dlg_ctl(h, L"BUTTON", L"扫描设备", BS_PUSHBUTTON | WS_TABSTOP, IDC_D_SCAN, 14, 152, 90, 26);
        dlg_ctl(h, L"LISTBOX", L"", WS_BORDER | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, IDC_D_LIST, 14, 186, 246, 92);
        dlg_ctl(h, L"BUTTON", L"连接", BS_DEFPUSHBUTTON | WS_TABSTOP, IDC_D_OK, 104, 290, 72, 28);
        dlg_ctl(h, L"BUTTON", L"取消", BS_PUSHBUTTON | WS_TABSTOP, IDC_D_CANCEL, 188, 290, 72, 28);
        SendDlgItemMessageW(h, IDC_D_RADIO_SWD, BM_SETCHECK, BST_CHECKED, 0);
        if (g_app.cfg.speed_khz > 0) {
            wchar_t wt[32];
            _snwprintf(wt, 32, L"%d", g_app.cfg.speed_khz);
            SetDlgItemTextW(h, IDC_D_SPEED, wt);
        }
        if (g_app.cfg.device[0]) {
            wchar_t wt[160];
            utf8_to_wide(g_app.cfg.device, wt, 160);
            SetDlgItemTextW(h, IDC_D_DEVICE, wt);
        }
        if (g_app.cfg.serial[0]) {
            wchar_t wt[96];
            utf8_to_wide(g_app.cfg.serial, wt, 96);
            SetDlgItemTextW(h, IDC_D_SERIAL, wt);
        }
        if (g_app.cfg.interface == OS_IF_JTAG)
            SendDlgItemMessageW(h, IDC_D_RADIO_JTAG, BM_SETCHECK, BST_CHECKED, 0);
        if (g_app.cfg.connect_under_reset)
            SendDlgItemMessageW(h, IDC_D_RESET, BM_SETCHECK, BST_CHECKED, 0);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == IDC_D_SCAN) {
            OS_DeviceInfo items[32];
            OS_ScanReq req;
            int i;
            HWND lb = GetDlgItem(h, IDC_D_LIST);
            req.items = items;
            req.capacity = 32;
            req.count = 0;
            SendMessageW(lb, LB_RESETCONTENT, 0, 0);
            if (os_driver_scan(&req) == OS_ERR_OK && req.count > 0) {
                for (i = 0; i < req.count; ++i) {
                    wchar_t wt[256];
                    _snwprintf(wt, 256, L"[%d] %s  SN:%hs", i, utf8_w(items[i].name),
                               items[i].serial);
                    SendMessageW(lb, LB_ADDSTRING, 0, (LPARAM)wt);
                }
            } else {
                SendMessageW(lb, LB_ADDSTRING, 0, (LPARAM)L"没有发现 J-Link 设备");
                MessageBoxW(h, L"没有发现 J-Link 设备", L"扫描", MB_OK | MB_ICONWARNING);
            }
        } else if (LOWORD(wp) == IDC_D_OK) {
            char buf[128];
            int err;
            OS_ConnectCfg cfg;
            memset(&cfg, 0, sizeof(cfg));
            cfg.interface = (SendDlgItemMessageW(h, IDC_D_RADIO_JTAG, BM_GETCHECK, 0, 0) == BST_CHECKED)
                                ? OS_IF_JTAG : OS_IF_SWD;
            GetDlgItemTextA(h, IDC_D_SPEED, buf, sizeof(buf));
            cfg.speed_khz = atoi(buf);
            GetDlgItemTextA(h, IDC_D_DEVICE, buf, sizeof(buf));
            _snprintf(cfg.device, sizeof(cfg.device), "%s", buf);
            GetDlgItemTextA(h, IDC_D_SERIAL, buf, sizeof(buf));
            _snprintf(cfg.serial, sizeof(cfg.serial), "%s", buf);
            cfg.connect_under_reset =
                (SendDlgItemMessageW(h, IDC_D_RESET, BM_GETCHECK, 0, 0) == BST_CHECKED);
            cfg.probe_index = -1;
            g_app.cfg = cfg;
            err = os_driver_connect(&cfg);
            if (err == OS_ERR_OK) {
                os_status("已连接: %s", g_app.dinfo.name[0] ? g_app.dinfo.name : "MCU");
                os_log(OS_LOG_INFO, "连接成功: %s %s (FW %s)",
                       g_app.dinfo.name, g_app.dinfo.dll_version, g_app.dinfo.fw_version);
                dlg_done_close(h);
            } else {
                wchar_t msg[256];
                _snwprintf(msg, 256, L"连接失败 (err=%d)。\n请检查 J-Link 设备、仿真口和时钟速度。", err);
                MessageBoxW(h, msg, L"连接失败", MB_OK | MB_ICONERROR);
            }
        } else if (LOWORD(wp) == IDC_D_CANCEL) {
            dlg_done_close(h);
        }
        return 0;
    case WM_CLOSE:
        dlg_done_close(h);
        return 0;
    case WM_DESTROY:
        if (g_dialog_done) SetEvent(g_dialog_done);
        return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

void ui_show_connect_dialog(HWND parent)
{
    HWND dlg = dlg_make(g_app.hInst, L"OSConnectDlg", L"连接 MCU", parent, 280, 330);
    if (dlg) dialog_pump(dlg, parent);
}

/* ------------------------- write-value dialog --------------------- */

static LRESULT CALLBACK write_proc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE: {
        wchar_t wt[320];
        OS_Leaf* L;
        dlg_ctl(h, L"STATIC", L"变量:", 0, IDC_D_STATIC, 14, 16, 60, 18);
        utf8_to_wide(g_write_leaf_name, wt, 320);
        dlg_ctl(h, L"STATIC", wt, 0, IDC_D_NAME, 78, 16, 180, 18);
        dlg_ctl(h, L"STATIC", L"新值:", 0, IDC_D_STATIC, 14, 48, 60, 18);
        dlg_ctl(h, L"EDIT", L"0", WS_BORDER | WS_TABSTOP, IDC_D_VALUE, 78, 46, 120, 22);
        dlg_ctl(h, L"BUTTON", L"写入", BS_DEFPUSHBUTTON | WS_TABSTOP, IDC_D_OK, 70, 86, 70, 28);
        dlg_ctl(h, L"BUTTON", L"取消", BS_PUSHBUTTON | WS_TABSTOP, IDC_D_CANCEL, 150, 86, 70, 28);
        if (g_write_leaf_id >= 0 && g_write_leaf_id < g_app.leaf_count) {
            L = &g_app.leaves[g_write_leaf_id];
            if (L->last.size) {
                _snwprintf(wt, 320, L"%g", L->last.value);
                SetDlgItemTextW(h, IDC_D_VALUE, wt);
            }
        }
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == IDC_D_OK) {
            char buf[128];
            char err[256];
            GetDlgItemTextA(h, IDC_D_VALUE, buf, sizeof(buf));
            if (os_write_leaf(g_write_leaf_id, atof(buf), err, sizeof(err)) != OS_ERR_OK) {
                wchar_t wt[320];
                utf8_to_wide(err, wt, 320);
                MessageBoxW(h, wt, L"写入失败", MB_OK | MB_ICONERROR);
            } else {
                dlg_done_close(h);
            }
        } else if (LOWORD(wp) == IDC_D_CANCEL) {
            dlg_done_close(h);
        }
        return 0;
    case WM_CLOSE:
        dlg_done_close(h);
        return 0;
    case WM_DESTROY:
        if (g_dialog_done) SetEvent(g_dialog_done);
        return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

void ui_show_write_dialog(HWND parent)
{
    HWND tv = g_app.hTree;
    HTREEITEM sel = TreeView_GetSelection(tv);
    TVITEMW item;
    OS_Leaf* L;
    HWND dlg;
    if (!sel) {
        MessageBoxW(parent, L"请先在变量树中选择一个叶变量", L"写变量", MB_OK | MB_ICONINFORMATION);
        return;
    }
    memset(&item, 0, sizeof(item));
    item.hItem = sel;
    item.mask = TVIF_PARAM;
    TreeView_GetItem(tv, &item);
    g_write_leaf_id = (int)item.lParam;
    if (g_write_leaf_id < 0 || g_write_leaf_id >= g_app.leaf_count) {
        MessageBoxW(parent, L"所选节点不是叶变量，请展开结构体后选择子项", L"写变量", MB_OK | MB_ICONINFORMATION);
        return;
    }
    L = &g_app.leaves[g_write_leaf_id];
    _snprintf(g_write_leaf_name, sizeof(g_write_leaf_name), "%s", L->path);
    dlg = dlg_make(g_app.hInst, L"OSWriteDlg", L"写变量", parent, 280, 130);
    if (dlg) dialog_pump(dlg, parent);
}

/* ------------------------- file dialogs --------------------------- */

int ui_choose_file(const char* filter, const char* title, char* out, int cap, int save)
{
    OPENFILENAMEA ofn;
    char file[MAX_PATH];
    memset(&ofn, 0, sizeof(ofn));
    file[0] = 0;
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_app.hMain;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = title;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    if (save) {
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
        if (!GetSaveFileNameA(&ofn)) return 0;
    } else {
        if (!GetOpenFileNameA(&ofn)) return 0;
    }
    _snprintf(out, cap, "%s", file);
    return 1;
}

int ui_choose_csv_path(char* out, int cap)
{
    return ui_choose_file("CSV 文件 (*.csv)|*.csv|所有文件 (*.*)|*.*||",
                          "选择数据日志保存位置", out, cap, 1);
}

/* ------------------------- tree ----------------------------------- */

static HTREEITEM tree_add(HWND tv, HTREEITEM parent, const char* text, LPARAM lp)
{
    TVINSERTSTRUCTW is;
    wchar_t wt[512];
    utf8_to_wide(text, wt, 512);
    memset(&is, 0, sizeof(is));
    is.hParent = parent;
    is.hInsertAfter = TVI_LAST;
    is.item.mask = TVIF_TEXT | TVIF_PARAM;
    is.item.pszText = wt;
    is.item.lParam = lp;
    return TreeView_InsertItem(tv, &is);
}

static void tree_add_type(HWND tv, HTREEITEM parent, OS_Variable* v, OS_Type* t,
                          uint64_t addr, const char* prefix)
{
    char text[320];
    char path[256];
    int i;
    if (!t) return;
    if (t->kind == OS_TYPE_STRUCT || t->kind == OS_TYPE_UNION) {
        for (i = 0; i < t->child_count; ++i) {
            OS_Type* m = t->children[i];
            HTREEITEM h;
            if (!m) continue;
            _snprintf(path, sizeof(path), "%s.%s", prefix, m->name ? m->name : "?");
            if (m->kind == OS_TYPE_STRUCT || m->kind == OS_TYPE_UNION ||
                m->kind == OS_TYPE_ARRAY) {
                if (m->kind == OS_TYPE_ARRAY)
                    _snprintf(text, sizeof(text), "%s  [%s x%d]  @0x%llX",
                              m->name ? m->name : "?", m->children && m->children[0] && m->children[0]->name
                                  ? m->children[0]->name : "?", m->array_count,
                              (unsigned long long)(addr + (uint64_t)m->member_offset));
                else
                    _snprintf(text, sizeof(text), "%s  [%s]  @0x%llX",
                              m->name ? m->name : "?", m->name ? m->name : "?",
                              (unsigned long long)(addr + (uint64_t)m->member_offset));
                h = tree_add(tv, parent, text, -1);
                if (m->kind == OS_TYPE_ARRAY) {
                    /* 数组元素：只显示元素类型，不展开全部元素 */
                    OS_Type* elem = m->children && m->children[0] ? m->children[0] : NULL;
                    char etxt[320];
                    if (elem && elem->kind == OS_TYPE_STRUCT)
                        _snprintf(etxt, sizeof(etxt), "元素: %s", elem->name ? elem->name : "?");
                    else if (elem)
                        _snprintf(etxt, sizeof(etxt), "元素: %s", elem->name ? elem->name : "?");
                    else
                        _snprintf(etxt, sizeof(etxt), "元素");
                    tree_add(tv, h, etxt, -1);
                }
                tree_add_type(tv, h, v, m, addr + (uint64_t)m->member_offset, path);
            } else {
                const char* tn = m->name ? m->name : "?";
                _snprintf(text, sizeof(text), "%s  : %s  @0x%llX", tn,
                          m->name ? m->name : "?", (unsigned long long)(addr + (uint64_t)m->member_offset));
                {
                    /* 找到对应的叶 id */
                    int j, leafid = -1;
                    for (j = 0; j < g_app.leaf_count; ++j)
                        if (strcmp(g_app.leaves[j].path, path) == 0) { leafid = j; break; }
                    tree_add(tv, parent, text, leafid);
                }
            }
        }
    } else if (t->kind == OS_TYPE_ARRAY) {
        /* 顶层数组：显示整体 */
        OS_Type* elem = t->children && t->children[0] ? t->children[0] : NULL;
        _snprintf(text, sizeof(text), "%s  [%s x%d]  @0x%llX",
                  v->name, elem && elem->name ? elem->name : "?", t->array_count,
                  (unsigned long long)addr);
        tree_add(tv, parent, text, -1);
    }
    (void)v;
}

void os_refresh_tree(void)
{
    HWND tv = g_app.hTree;
    HTREEITEM root;
    int i;
    if (!tv) return;
    TreeView_DeleteAllItems(tv);
    root = tree_add(tv, NULL, "全局变量", -1);
    if (!g_app.elf) {
        tree_add(tv, root, "（未加载 ELF）", -1);
        TreeView_Expand(tv, root, TVE_EXPAND);
        return;
    }
    for (i = 0; i < os_elf_var_count(g_app.elf); ++i) {
        OS_Variable* v = (OS_Variable*)os_elf_var_at(g_app.elf, i);
        char text[320];
        HTREEITEM h;
        if (!v || !v->name) continue;
        if (v->type && (v->type->kind == OS_TYPE_STRUCT || v->type->kind == OS_TYPE_UNION)) {
            _snprintf(text, sizeof(text), "%s  [%s]  @0x%llX", v->name,
                      v->type->name ? v->type->name : "struct",
                      (unsigned long long)v->address);
            h = tree_add(tv, root, text, -1);
            tree_add_type(tv, h, v, v->type, v->address, v->name);
        } else if (v->type && v->type->kind == OS_TYPE_ARRAY) {
            tree_add_type(tv, root, v, v->type, v->address, v->name);
        } else {
            const char* tn = v->type && v->type->name ? v->type->name : "raw";
            _snprintf(text, sizeof(text), "%s  : %s  @0x%llX  (%llu B)", v->name, tn,
                      (unsigned long long)v->address, (unsigned long long)v->symbol_size);
            {
                int j, leafid = -1;
                for (j = 0; j < g_app.leaf_count; ++j)
                    if (strcmp(g_app.leaves[j].path, v->name) == 0) { leafid = j; break; }
                tree_add(tv, root, text, leafid);
            }
        }
    }
    TreeView_Expand(tv, root, TVE_EXPAND);
}

/* ------------------------- main window ---------------------------- */

static void append_log_text(const char* utf8)
{
    wchar_t wt[1024];
    int n;
    utf8_to_wide(utf8, wt, 1024);
    n = (int)SendMessageW(g_app.hLog, LB_ADDSTRING, 0, (LPARAM)wt);
    if (n >= 800) SendMessageW(g_app.hLog, LB_DELETESTRING, 0, 0);
    SendMessageW(g_app.hLog, LB_SETCURSEL, n > 0 ? n - 1 : 0, 0);
}

static void refresh_buttons(void)
{
    int i;
    int conn = g_app.connected;
    EnableWindow(g_app.hBtn[2], !conn);   /* 连接 */
    EnableWindow(g_app.hBtn[3], conn);    /* 断开 */
    EnableWindow(g_app.hBtn[4], conn && !g_app.acq_running); /* 开始 */
    EnableWindow(g_app.hBtn[5], conn && g_app.acq_running);  /* 停止 */
    EnableWindow(g_app.hBtn[6], !g_app.replay_running);      /* 回放 */
    for (i = 0; i < g_app.nbtns; ++i)
        if (!g_app.hBtn[i]) return;
}

static void make_menu(void)
{
    HMENU bar = CreateMenu();
    HMENU mFile = CreateMenu();
    HMENU mOp = CreateMenu();
    HMENU mWin = CreateMenu();
    HMENU mRate = CreateMenu();
    HMENU mHelp = CreateMenu();
    AppendMenuA(mFile, MF_STRING, IDM_FILE_LOADELF, "加载 ELF...");
    AppendMenuA(mFile, MF_STRING, IDM_FILE_REPLAY, "回放 CSV...");
    AppendMenuA(mFile, MF_SEPARATOR, 0, NULL);
    AppendMenuA(mFile, MF_STRING, IDM_FILE_EXIT, "退出");
    AppendMenuA(mOp, MF_STRING, IDM_OP_SCAN, "扫描设备");
    AppendMenuA(mOp, MF_STRING, IDM_OP_CONNECT, "连接...");
    AppendMenuA(mOp, MF_STRING, IDM_OP_DISCONNECT, "断开连接");
    AppendMenuA(mOp, MF_SEPARATOR, 0, NULL);
    AppendMenuA(mOp, MF_STRING, IDM_OP_START, "开始采集");
    AppendMenuA(mOp, MF_STRING, IDM_OP_STOP, "停止采集");
    AppendMenuA(mOp, MF_SEPARATOR, 0, NULL);
    AppendMenuA(mOp, MF_STRING, IDM_OP_WRITE, "写变量...");
    AppendMenuA(mOp, MF_STRING, IDM_OP_HALT, "暂停目标 (Halt)");
    AppendMenuA(mOp, MF_STRING, IDM_OP_GO, "继续目标 (Go)");
    AppendMenuA(mOp, MF_SEPARATOR, 0, NULL);
    AppendMenuA(mRate, MF_STRING, IDM_RATE_BASE + 0, "50 Hz");
    AppendMenuA(mRate, MF_STRING, IDM_RATE_BASE + 1, "100 Hz");
    AppendMenuA(mRate, MF_STRING, IDM_RATE_BASE + 2, "200 Hz");
    AppendMenuA(mRate, MF_STRING, IDM_RATE_BASE + 3, "500 Hz");
    AppendMenuA(mRate, MF_STRING, IDM_RATE_BASE + 4, "1000 Hz");
    AppendMenuA(mOp, MF_POPUP, (UINT_PTR)mRate, "采集频率");
    AppendMenuA(mHelp, MF_STRING, IDM_HELP_ABOUT, "关于");
    os_build_window_menu(mWin);
    AppendMenuA(bar, MF_POPUP, (UINT_PTR)mFile, "文件");
    AppendMenuA(bar, MF_POPUP, (UINT_PTR)mOp, "操作");
    AppendMenuA(bar, MF_POPUP, (UINT_PTR)mWin, "窗口");
    AppendMenuA(bar, MF_POPUP, (UINT_PTR)mHelp, "帮助");
    SetMenu(g_app.hMain, bar);
    CheckMenuRadioItem(mRate, IDM_RATE_BASE, IDM_RATE_BASE + 4,
                       IDM_RATE_BASE + 1, MF_BYCOMMAND);
}

static void layout(HWND hwnd)
{
    RECT rc;
    int x = 4, y = 4, bh = 30, i;
    GetClientRect(hwnd, &rc);
    for (i = 0; i < g_app.nbtns; ++i) {
        int bw = i == 0 ? 84 : (i >= 7 ? 60 : 52);
        MoveWindow(g_app.hBtn[i], x, y, bw, bh, TRUE);
        x += bw + 4;
    }
    y = 4 + bh + 4;
    MoveWindow(g_app.hTree, 4, y, (rc.right - 4) / 2 - 2, rc.bottom - y - 150, TRUE);
    MoveWindow(g_app.hLog, (rc.right - 4) / 2 + 2, y, (rc.right - 4) / 2 - 6,
               rc.bottom - y - 150, TRUE);
    SendMessageW(g_app.hStatus, WM_SIZE, 0, 0);
}

static void handle_window_menu(UINT id)
{
    int idx = (int)(id - IDM_WIN_BASE);
    int mi = idx / 16;
    int wt = idx % 16;
    if (mi >= 0 && mi < g_app.mod_count) {
        const OS_WindowType* wts = g_app.mods[mi]->window_types;
        if (wts && wts[wt].type)
            os_create_window_for_type(wts[wt].type);
    }
}

static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE: {
        static const char* labels[] = {
            "加载ELF", "扫描", "连接", "断开", "开始", "停止", "回放", "波形", "数值"
        };
        static const int ids[] = {
            IDC_BTN_ELF, IDC_BTN_SCAN, IDC_BTN_CONN, IDC_BTN_DISC,
            IDC_BTN_START, IDC_BTN_STOP, IDC_BTN_REPLAY, IDC_BTN_SCOPE, IDC_BTN_NUM
        };
        int i;
        g_app.hMain = hwnd;
        g_app.nbtns = 9;
        for (i = 0; i < g_app.nbtns; ++i) {
            wchar_t wt[64];
            utf8_to_wide(labels[i], wt, 64);
            g_app.hBtn[i] = dlg_ctl(hwnd, L"BUTTON", wt, BS_PUSHBUTTON, ids[i], 0, 0, 50, 30);
        }
        g_app.hTree = CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEWW, L"",
                                      WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES |
                                      TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP,
                                      0, 0, 0, 0, hwnd, (HMENU)IDC_TREE, g_app.hInst, NULL);
        g_app.hLog = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", L"",
                                     WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOINTEGRALHEIGHT,
                                     0, 0, 0, 0, hwnd, (HMENU)IDC_LOG, g_app.hInst, NULL);
        g_app.hStatus = CreateWindowExW(0, STATUSCLASSNAMEW, L"就绪",
                                        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
                                        0, 0, 0, 0, hwnd, (HMENU)IDC_STATUS, g_app.hInst, NULL);
        g_app.hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        SendMessageW(g_app.hTree, WM_SETFONT, (WPARAM)g_app.hFont, TRUE);
        SendMessageW(g_app.hLog, WM_SETFONT, (WPARAM)g_app.hFont, TRUE);
        make_menu();
        os_refresh_tree();
        refresh_buttons();
        os_start_monitor();
        return 0;
    }
    case WM_SIZE:
        layout(hwnd);
        return 0;
    case WM_COMMAND: {
        UINT id = LOWORD(wp);
        if (id >= IDM_WIN_BASE && id < IDM_WIN_BASE + 0x400) {
            handle_window_menu(id);
            return 0;
        }
        switch (id) {
        case IDC_BTN_ELF:
        case IDM_FILE_LOADELF: {
            char path[MAX_PATH];
            if (ui_choose_file("ELF 文件 (*.elf;*.out;*.axf)|*.elf;*.out;*.axf|所有文件 (*.*)|*.*||",
                               "选择 ELF 文件", path, sizeof(path), 0)) {
                os_load_elf(path);
            }
            break;
        }
        case IDC_BTN_REPLAY:
        case IDM_FILE_REPLAY: {
            char path[MAX_PATH];
            if (g_app.acq_running) os_stop_acq();
            if (ui_choose_file("CSV 文件 (*.csv)|*.csv|所有文件 (*.*)|*.*||",
                               "选择回放数据文件", path, sizeof(path), 0)) {
                if (os_start_replay(path) != OS_ERR_OK)
                    os_log(OS_LOG_ERROR, "回放启动失败");
            }
            break;
        }
        case IDM_FILE_EXIT:
            DestroyWindow(hwnd);
            break;
        case IDC_BTN_SCAN:
        case IDM_OP_SCAN: {
            OS_DeviceInfo items[32];
            OS_ScanReq req;
            int i;
            req.items = items;
            req.capacity = 32;
            req.count = 0;
            if (os_driver_scan(&req) == OS_ERR_OK && req.count > 0) {
                os_log(OS_LOG_INFO, "发现 %d 个 J-Link 设备:", req.count);
                for (i = 0; i < req.count; ++i)
                    os_log(OS_LOG_INFO, "  [%d] %s SN:%s", i, items[i].name, items[i].serial);
            } else {
                os_log(OS_LOG_WARN, "没有发现 J-Link 设备");
                MessageBoxW(hwnd, L"没有发现 J-Link 设备", L"扫描",
                            MB_OK | MB_ICONWARNING);
            }
            break;
        }
        case IDC_BTN_CONN:
        case IDM_OP_CONNECT:
            ui_show_connect_dialog(hwnd);
            refresh_buttons();
            break;
        case IDC_BTN_DISC:
        case IDM_OP_DISCONNECT:
            if (g_app.acq_running) os_stop_acq();
            os_driver_disconnect();
            os_status("已断开连接");
            os_log(OS_LOG_INFO, "已断开连接");
            refresh_buttons();
            break;
        case IDC_BTN_START:
        case IDM_OP_START:
            os_start_acq();
            refresh_buttons();
            break;
        case IDC_BTN_STOP:
        case IDM_OP_STOP:
            os_stop_acq();
            refresh_buttons();
            break;
        case IDM_OP_WRITE:
            ui_show_write_dialog(hwnd);
            break;
        case IDM_OP_HALT:
            os_driver_halt();
            break;
        case IDM_OP_GO:
            os_driver_go();
            break;
        case IDC_BTN_SCOPE:
            os_create_window_for_type("scope.line");
            break;
        case IDC_BTN_NUM:
            os_create_window_for_type("scope.value");
            break;
        case IDM_HELP_ABOUT:
            ui_show_about(hwnd);
            break;
        default:
            if (id >= IDM_RATE_BASE && id < IDM_RATE_BASE + 5) {
                static const int rates[] = { 50, 100, 200, 500, 1000 };
                g_app.sample_hz = rates[id - IDM_RATE_BASE];
                CheckMenuRadioItem(GetMenu(hwnd), IDM_RATE_BASE, IDM_RATE_BASE + 4,
                                   id, MF_BYCOMMAND);
                os_status("采集频率: %d Hz", g_app.sample_hz);
            }
            break;
        }
        return 0;
    }
    case WM_APP_ELF_CHANGED:
        InterlockedExchange(&g_app.elf_reload_pending, 0);
        os_prompt_elf_changed();
        return 0;
    case WM_APP_LOG_MSG: {
        char* s = (char*)lp;
        if (s) {
            append_log_text(s);
            free(s);
        }
        return 0;
    }
    case WM_APP_STATUS_MSG: {
        char* s = (char*)lp;
        wchar_t wt[512];
        if (s) {
            utf8_to_wide(s, wt, 512);
            SendMessageW(g_app.hStatus, SB_SETTEXT, 0, (LPARAM)wt);
            free(s);
        }
        return 0;
    }
    case WM_APP_ACQ_END:
        os_status("采集已停止");
        refresh_buttons();
        return 0;
    case WM_APP_REPLAY_END:
        os_status("回放已结束");
        refresh_buttons();
        return 0;
    case WM_CLOSE:
        os_stop_acq();
        os_stop_replay();
        os_stop_monitor();
        os_driver_disconnect();
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void ui_show_about(HWND parent)
{
    MessageBoxW(parent,
                L"OpenScope v0.1\n"
                L"类 CANape 的 MCU 变量采集/标定工具\n"
                L"框架 + 模块化 DLL 架构，ELF/DWARF 变量解析，J-Link 驱动",
                L"关于 OpenScope", MB_OK | MB_ICONINFORMATION);
}

int ui_confirm(const char* title, const char* text, UINT flags)
{
    wchar_t wt[2048], tt[256];
    utf8_to_wide(text, wt, 2048);
    utf8_to_wide(title, tt, 256);
    return MessageBoxW(g_app.hMain, wt, tt, flags);
}

static int fw_leaf_find_local(const char* needle, int* ids, int max_ids)
{
    return fw_leaf_find(needle, ids, max_ids);
}

void ui_init(HINSTANCE hInst)
{
    WNDCLASSW wc;
    INITCOMMONCONTROLSEX icc;
    g_app.hInst = hInst;
    g_dialog_done = CreateEventW(NULL, TRUE, FALSE, NULL);
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"OpenScopeMain";
    RegisterClassW(&wc);

    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = pick_proc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"OSPickVarDlg";
    RegisterClassW(&wc);

    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = connect_proc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"OSConnectDlg";
    RegisterClassW(&wc);

    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = write_proc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"OSWriteDlg";
    RegisterClassW(&wc);
}

int ui_create_main_window(int nCmdShow)
{
    g_app.hMain = CreateWindowExW(0, L"OpenScopeMain", L"OpenScope - MCU 变量采集与标定",
                                  WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                  1080, 720, NULL, NULL, g_app.hInst, NULL);
    if (!g_app.hMain) return -1;
    ShowWindow(g_app.hMain, nCmdShow);
    UpdateWindow(g_app.hMain);
    return 0;
}
