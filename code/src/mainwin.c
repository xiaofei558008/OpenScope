#include "app.h"
#include "mainwin.h"
#include "chartwin.h"
#include "numwin.h"
#include "vartree.h"
#include "datasrv.h"
#include "datalog.h"
#include "module_mgr.h"
#include <commctrl.h>
#include <commdlg.h>
#include <string.h>

#define IDC_BTN_OPEN     2001
#define IDC_BTN_CONNECT  2002
#define IDC_BTN_DISCON   2003
#define IDC_BTN_START    2004
#define IDC_BTN_STOP     2005
#define IDC_BTN_LOGSTART 2006
#define IDC_BTN_LOGSTOP  2007
#define IDC_BTN_REPLAY   2008
#define IDC_BTN_REPLAYSTOP 2009
#define IDC_BTN_ABOUT    2010
#define IDM_EXIT         2011
#define IDM_WIN_CHART    2012
#define IDM_WIN_NUM      2013
#define IDM_WIN_MODULE_BASE 2200

#define IDM_TREE_ALL     2301
#define IDM_TREE_NONE    2302
#define IDM_TREE_WRITE   2303
#define IDM_TREE_RELOAD  2304

#define IDD_PICK_OK      2401
#define IDD_PICK_CANCEL  2402
#define IDD_PICK_EDIT    2403
#define IDD_PICK_LIST    2404
#define IDD_EDIT_OK      2411
#define IDD_EDIT_CANCEL  2412
#define IDD_EDIT_TEXT    2413
#define IDD_EDIT_VALUE   2414

typedef struct PickState {
    int* result;
    int ids[300];
    int count;
} PickState;

typedef struct EditState {
    int* result;
    wchar_t* text_out;
} EditState;

typedef struct ModWinMenuItem {
    OS_Module* mod;
    void* ctx;
    const OS_WindowType* wt;
} ModWinMenuItem;

static const wchar_t* g_main_class = L"OpenScopeMain";
static HMENU g_menu;
static HMENU g_menu_win;
static ModWinMenuItem g_modwin_menu[64];
static int g_modwin_menu_count;

/* ---------- 工具 ---------- */

static void set_status(int part, const wchar_t* text)
{
    if (g_app.hStatus) SendMessageW(g_app.hStatus, SB_SETTEXTW, part, (LPARAM)text);
}

static void set_status_utf8(int part, const char* text)
{
    wchar_t w[512];
    os_utf8_to_wide_buf(text, w, 512);
    set_status(part, w);
}

void os_mainwin_append_log(int level, const wchar_t* line)
{
    LVITEMW item;
    wchar_t t[64], lvl[16], msg[1024];
    int64_t us = os_time_us();
    int minute = (int)(us / 1000000 / 60);
    int sec = (int)(us / 1000000 % 60);
    int msec = (int)(us % 1000000 / 1000);
    if (!g_app.hLog || !IsWindow(g_app.hLog)) return;
    _snwprintf(t, 64, L"%02d:%02d.%03d", minute % 60, sec, msec);
    switch (level) {
    case OS_LOG_WARN: _snwprintf(lvl, 16, L"警告"); break;
    case OS_LOG_ERROR: _snwprintf(lvl, 16, L"错误"); break;
    case OS_LOG_DEBUG: _snwprintf(lvl, 16, L"调试"); break;
    default: _snwprintf(lvl, 16, L"信息"); break;
    }
    _snwprintf(msg, 1024, L"%s", line ? line : L"");
    memset(&item, 0, sizeof(item));
    item.mask = LVIF_TEXT;
    item.iItem = 0;
    item.iSubItem = 0;
    item.pszText = t;
    item.iItem = ListView_InsertItem(g_app.hLog, &item);
    ListView_SetItemText(g_app.hLog, item.iItem, 1, lvl);
    ListView_SetItemText(g_app.hLog, item.iItem, 2, msg);
    if (ListView_GetItemCount(g_app.hLog) > 5000)
        ListView_DeleteItem(g_app.hLog, ListView_GetItemCount(g_app.hLog) - 1);
}

void os_fw_log(int level, const char* fmt, ...)
{
    char buf[1024];
    wchar_t w[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    os_utf8_to_wide_buf(buf, w, 1024);
    os_mainwin_append_log(level, w);
}

void os_fw_post(UINT msg, WPARAM w, LPARAM l)
{
    if (g_app.hMain) PostMessage(g_app.hMain, msg, w, l);
}

void os_mainwin_update_buttons(void)
{
    int hasDriver = g_app.driver != NULL;
    int conn = g_app.connected ? 1 : 0;
    int running = g_app.acq_state == OS_ACQ_RUNNING;
    int replay = g_app.acq_state == OS_ACQ_REPLAY;
    HWND b;
#define ENB(id, on) do { b = GetDlgItem(g_app.hMain, (id)); if (b) EnableWindow(b, (on)); } while (0)
    ENB(IDC_BTN_CONNECT, hasDriver && !conn && !replay);
    ENB(IDC_BTN_DISCON, conn);
    ENB(IDC_BTN_START, conn && g_app.watch_count > 0 && !running && !replay);
    ENB(IDC_BTN_STOP, running);
    ENB(IDC_BTN_LOGSTART, conn && !g_app.log_csv && !replay);
    ENB(IDC_BTN_LOGSTOP, g_app.log_csv != NULL);
    ENB(IDC_BTN_REPLAY, !running && !replay);
    ENB(IDC_BTN_REPLAYSTOP, replay);
    ENB(IDC_BTN_ABOUT, 1);
    ENB(IDC_BTN_OPEN, 1);
#undef ENB
}

static void refresh_status(void)
{
    wchar_t w[512];
    if (g_app.connected && g_app.driver) {
        wchar_t dn[128];
        os_utf8_to_wide_buf(g_app.driver->name, dn, 128);
        _snwprintf(w, 512, L"已连接 %s", dn);
    } else {
        _snwprintf(w, 512, L"未连接");
    }
    set_status(0, w);
    if (g_app.elf_path[0])
        set_status(1, g_app.elf_path);
    else
        set_status(1, L"未加载 ELF");
    switch (g_app.acq_state) {
    case OS_ACQ_RUNNING: set_status(2, L"采集中"); break;
    case OS_ACQ_REPLAY: set_status(2, L"离线回放中"); break;
    default: set_status(2, L"空闲"); break;
    }
    _snwprintf(w, 512, L"观测 %d / 叶子 %d / 样本 %ld",
               g_app.watch_count, g_app.leaf_count, (long)g_app.total_samples);
    set_status(3, w);
}

/* ---------- 布局 ---------- */

void os_mainwin_tile(void)
{
    RECT rc;
    int i, n, cols = 2, rows, cw, ch, x, y, m = 3, gap = 4;
    if (!g_app.hRight) return;
    GetClientRect(g_app.hRight, &rc);
    n = 0;
    for (i = 0; i < g_app.win_count; i++) if (g_app.wins[i].active && g_app.wins[i].hwnd) n++;
    if (n == 0) return;
    rows = (n + cols - 1) / cols;
    cw = (rc.right - m * 2 - gap * (cols - 1)) / cols;
    ch = (rc.bottom - m * 2 - gap * (rows - 1)) / rows;
    i = 0;
    for (int k = 0; k < g_app.win_count; k++) {
        OS_WinItem* wi = &g_app.wins[k];
        if (!wi->active || !wi->hwnd) continue;
        x = m + (i % cols) * (cw + gap);
        y = m + (i / cols) * (ch + gap);
        MoveWindow(wi->hwnd, x, y, cw, ch, TRUE);
        i++;
    }
}

static void layout(void)
{
    RECT rc;
    int bw, bh = 34, sw, logh, right_h, right_w;
    int parts[4];
    GetClientRect(g_app.hMain, &rc);
    bw = rc.right;
    sw = g_app.tree_w;
    logh = g_app.log_h;
    right_h = rc.bottom - bh - logh - 22;
    right_w = bw - sw - 5;
    MoveWindow(g_app.hSplitV, sw, bh, 5, right_h, TRUE);
    MoveWindow(g_app.hTree, 0, bh, sw, right_h, TRUE);
    MoveWindow(g_app.hRight, sw + 5, bh, right_w - 5, right_h, TRUE);
    MoveWindow(g_app.hLog, 0, bh + right_h, bw, logh, TRUE);
    MoveWindow(g_app.hStatus, 0, rc.bottom - 22, bw, 22, TRUE);
    parts[0] = 170; parts[1] = 420; parts[2] = 620; parts[3] = -1;
    SendMessageW(g_app.hStatus, SB_SETPARTS, 4, (LPARAM)parts);
    os_mainwin_tile();
    {
        static const struct { int id; const wchar_t* text; } btns[] = {
            { IDC_BTN_OPEN, L"打开ELF" }, { IDC_BTN_CONNECT, L"连接" },
            { IDC_BTN_DISCON, L"断开" }, { IDC_BTN_START, L"开始采集" },
            { IDC_BTN_STOP, L"停止采集" }, { IDC_BTN_LOGSTART, L"记录" },
            { IDC_BTN_LOGSTOP, L"停止记录" }, { IDC_BTN_REPLAY, L"离线回放" },
            { IDC_BTN_REPLAYSTOP, L"停止回放" }, { IDC_BTN_ABOUT, L"关于" },
        };
        int i, x = 6;
        for (i = 0; i < (int)(sizeof(btns) / sizeof(btns[0])); i++) {
            HWND b = GetDlgItem(g_app.hMain, btns[i].id);
            SIZE sz;
            HDC hdc = GetDC(b ? b : g_app.hMain);
            SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
            GetTextExtentPoint32W(hdc, btns[i].text, (int)wcslen(btns[i].text), &sz);
            ReleaseDC(b ? b : g_app.hMain, hdc);
            if (b) MoveWindow(b, x, 5, sz.cx + 18, 24, TRUE);
            x += sz.cx + 24;
        }
    }
}

/* ---------- 分割条 ---------- */

static LRESULT CALLBACK split_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_LBUTTONDOWN:
        SetCapture(hwnd);
        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        return 0;
    case WM_MOUSEMOVE:
        if (GetCapture() == hwnd) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(GetParent(hwnd), &pt);
            SendMessage(GetParent(hwnd), WM_OS_SPLIT, (WPARAM)pt.x, 0);
        }
        return 0;
    case WM_LBUTTONUP:
        if (GetCapture() == hwnd) ReleaseCapture();
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/* ---------- ELF ---------- */

static int load_elf_path(const wchar_t* path)
{
    char utf8[MAX_PATH];
    char err[256];
    OS_ElfFile* elf;
    wchar_t wmsg[2048];
    int i;
    os_wide_to_utf8_buf(path, utf8, MAX_PATH);
    elf = os_elf_open(utf8, err, sizeof(err));
    if (!elf) {
        os_utf8_to_wide_buf(err, wmsg, 2048);
        MessageBoxW(g_app.hMain, wmsg, L"ELF 加载失败", MB_OK | MB_ICONERROR);
        return -1;
    }
    if (g_app.elf) os_elf_close(g_app.elf);
    g_app.elf = elf;
    _snwprintf(g_app.elf_path, MAX_PATH, L"%s", path);
    g_app.elf_mtime = os_file_mtime_ms(path);
    os_vartree_build();
    os_vartree_fill_tree(g_app.hTree);
    /* 缺失变量提示 */
    if (os_vartree_missing_count() > 0) {
        wchar_t list[1800] = L"";
        for (i = 0; i < os_vartree_missing_count() && i < 12; i++) {
            wchar_t tmp[300];
            os_utf8_to_wide_buf(os_vartree_missing_at(i), tmp, 300);
            _snwprintf(list + wcslen(list), 1800 - (int)wcslen(list), L"%s\n", tmp);
        }
        _snwprintf(wmsg, 2048, L"以下 %d 个变量在新 ELF 中未找到，是否忽略？\n\n%s",
                   os_vartree_missing_count(), list);
        MessageBoxW(g_app.hMain, wmsg, L"变量缺失", MB_YESNO | MB_ICONWARNING);
    }
    /* 通知模块 */
    for (i = 0; i < g_app.winmod_count; i++) {
        if (g_app.winmods[i]->command)
            g_app.winmods[i]->command(g_app.winmod_ctx[i], OS_CMD_ELF_RELOADED, NULL, NULL);
    }
    os_log(OS_LOG_INFO, "已加载 ELF: %ls (%d 位, %s)",
           path, os_elf_bits(g_app.elf), os_elf_arch_name(g_app.elf));
    refresh_status();
    os_mainwin_update_buttons();
    return 0;
}

int os_mainwin_reload_elf(void)
{
    if (!g_app.elf_path[0]) return -1;
    return load_elf_path(g_app.elf_path);
}

static void cmd_open_elf(void)
{
    OPENFILENAMEW ofn;
    wchar_t file[MAX_PATH] = L"";
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_app.hMain;
    ofn.lpstrFilter = L"ELF 文件\0*.elf;*.axf;*.out\0所有文件\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (GetOpenFileNameW(&ofn)) load_elf_path(file);
}

static void check_elf_mtime(void)
{
    uint64_t mt;
    if (!g_app.elf_path[0] || !g_app.elf) return;
    mt = os_file_mtime_ms(g_app.elf_path);
    if (mt && mt != g_app.elf_mtime) {
        g_app.elf_mtime = mt;
        if (MessageBoxW(g_app.hMain,
                        L"检测到 ELF 文件已更新（可能重新编译），是否立即重新加载并刷新变量地址？",
                        L"ELF 更新", MB_YESNO | MB_ICONQUESTION) == IDYES) {
            load_elf_path(g_app.elf_path);
        }
    }
}

/* ---------- 连接 / 采集 / 记录 ---------- */

static void cmd_connect(void)
{
    int rc, c = 0;
    OS_DriverInfo info;
    if (!g_app.driver || !g_app.driver->command) {
        MessageBoxW(g_app.hMain, L"未加载驱动模块（jlink.dll）", L"连接", MB_OK | MB_ICONERROR);
        return;
    }
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_CONFIGURE, (void*)g_app.hMain, NULL);
    if (rc == OS_ERR_CANCELED) return;
    if (rc != OS_ERR_OK) {
        MessageBoxW(g_app.hMain, L"配置失败", L"连接", MB_OK | MB_ICONERROR);
        return;
    }
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_CONNECT, NULL, NULL);
    if (rc != OS_ERR_OK) {
        MessageBoxW(g_app.hMain, L"连接 MCU 失败，请检查仿真器与目标板", L"连接", MB_OK | MB_ICONERROR);
        return;
    }
    g_app.driver->command(g_app.driver_ctx, OS_CMD_IS_CONNECTED, NULL, &c);
    g_app.connected = c;
    memset(&info, 0, sizeof(info));
    g_app.driver->command(g_app.driver_ctx, OS_CMD_GET_INFO, NULL, &info);
    os_log(OS_LOG_INFO, "已连接: %s（%s, J-Link DLL %s, HW %d, FW %d）",
           info.emulator[0] ? info.emulator : "?", info.name, info.dll_version,
           info.hw_version, info.fw_version);
    refresh_status();
    os_mainwin_update_buttons();
}

static void cmd_disconnect(void)
{
    if (g_app.driver && g_app.driver->command) {
        g_app.driver->command(g_app.driver_ctx, OS_CMD_DISCONNECT, NULL, NULL);
    }
    g_app.connected = 0;
    os_log(OS_LOG_INFO, "已断开连接");
    refresh_status();
    os_mainwin_update_buttons();
}

static void cmd_start_acq(void)
{
    if (!g_app.connected) {
        MessageBoxW(g_app.hMain, L"请先连接 MCU", L"采集", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (os_ds_start() == 0) {
        refresh_status();
        os_mainwin_update_buttons();
    }
}

static void cmd_stop_acq(void)
{
    os_ds_stop();
    refresh_status();
    os_mainwin_update_buttons();
}

static void cmd_log_start(void)
{
    OPENFILENAMEW ofn;
    wchar_t file[MAX_PATH] = L"data.csv";
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_app.hMain;
    ofn.lpstrFilter = L"CSV 文件\0*.csv\0所有文件\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"csv";
    ofn.Flags = OFN_OVERWRITEPROMPT;
    if (GetSaveFileNameW(&ofn)) {
        if (os_datalog_start(file) != 0)
            MessageBoxW(g_app.hMain, L"无法创建记录文件", L"记录", MB_OK | MB_ICONERROR);
    }
    os_mainwin_update_buttons();
}

static void cmd_log_stop(void)
{
    os_datalog_stop();
    os_mainwin_update_buttons();
}

static void cmd_replay_open(void)
{
    OPENFILENAMEW ofn;
    wchar_t file[MAX_PATH] = L"";
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_app.hMain;
    ofn.lpstrFilter = L"CSV 记录\0*.csv\0所有文件\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetOpenFileNameW(&ofn)) return;
    if (g_app.acq_state == OS_ACQ_RUNNING) os_ds_stop();
    if (os_replay_start(file) != 0) {
        MessageBoxW(g_app.hMain, L"无法打开回放文件", L"回放", MB_OK | MB_ICONERROR);
        return;
    }
    SetTimer(g_app.hMain, 2, 10, NULL);
    refresh_status();
    os_mainwin_update_buttons();
}

static void cmd_replay_stop(void)
{
    KillTimer(g_app.hMain, 2);
    os_replay_stop();
    refresh_status();
    os_mainwin_update_buttons();
}

/* ---------- 窗口管理 ---------- */

static void add_win_item(HWND hwnd, int is_module, OS_Module* mod, void* ctx, const wchar_t* title)
{
    OS_WinItem* wi;
    if (g_app.win_count >= OS_MAX_WINS) {
        DestroyWindow(hwnd);
        return;
    }
    wi = &g_app.wins[g_app.win_count++];
    memset(wi, 0, sizeof(*wi));
    wi->hwnd = hwnd;
    wi->is_module = is_module;
    wi->mod = mod;
    wi->mod_ctx = ctx;
    wi->active = 1;
    _snwprintf(wi->title, 128, L"%s", title ? title : L"");
    os_mainwin_tile();
}

static void cmd_add_window(int native_chart)
{
    wchar_t title[128];
    HWND h;
    static int chart_no = 1, num_no = 1;
    if (native_chart) {
        _snwprintf(title, 128, L"波形窗口 %d", chart_no++);
        h = os_chart_create(g_app.hRight, 0, 0, 200, 150, title);
    } else {
        _snwprintf(title, 128, L"数值窗口 %d", num_no++);
        h = os_num_create(g_app.hRight, 0, 0, 200, 150, title);
    }
    if (h) add_win_item(h, 0, NULL, NULL, title);
}

void os_mainwin_rebuild_window_menu(void)
{
    int i;
    wchar_t wname[128];
    if (!g_menu_win) return;
    while (GetMenuItemCount(g_menu_win) > 0)
        DeleteMenu(g_menu_win, 0, MF_BYPOSITION);
    AppendMenuW(g_menu_win, MF_STRING, IDM_WIN_CHART, L"波形窗口");
    AppendMenuW(g_menu_win, MF_STRING, IDM_WIN_NUM, L"数值窗口");
    g_modwin_menu_count = 0;
    for (i = 0; i < g_app.winmod_count; i++) {
        const OS_WindowType* wt = g_app.winmods[i]->window_types;
        while (wt && wt->type && g_modwin_menu_count < 64) {
            os_utf8_to_wide_buf(wt->display_name ? wt->display_name : wt->type, wname, 128);
            AppendMenuW(g_menu_win, MF_STRING, IDM_WIN_MODULE_BASE + g_modwin_menu_count,
                        wname);
            g_modwin_menu[g_modwin_menu_count].mod = g_app.winmods[i];
            g_modwin_menu[g_modwin_menu_count].ctx = g_app.winmod_ctx[i];
            g_modwin_menu[g_modwin_menu_count].wt = wt;
            g_modwin_menu_count++;
            wt++;
        }
    }
    DrawMenuBar(g_app.hMain);
}

/* ---------- 对话框 ---------- */

static void run_modal(HWND dlg, HWND owner)
{
    MSG msg;
    ShowWindow(dlg, SW_SHOW);
    UpdateWindow(dlg);
    if (owner) EnableWindow(owner, FALSE);
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsWindow(dlg)) break;
        if (IsDialogMessageW(dlg, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (owner) {
        EnableWindow(owner, TRUE);
        SetForegroundWindow(owner);
    }
    if (IsWindow(dlg)) DestroyWindow(dlg);
}

static void pick_refresh(HWND dlg)
{
    PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
    wchar_t wtext[512];
    char text[512];
    HWND hList = GetDlgItem(dlg, IDD_PICK_LIST);
    HWND hEdit = GetDlgItem(dlg, IDD_PICK_EDIT);
    int i;
    GetWindowTextW(hEdit, wtext, 512);
    WideCharToMultiByte(CP_UTF8, 0, wtext, -1, text, sizeof(text), NULL, NULL);
    st->count = os_vartree_search(text, 300, st->ids);
    SendMessageW(hList, LB_RESETCONTENT, 0, 0);
    for (i = 0; i < st->count; i++) {
        const OS_Leaf* L = os_vartree_leaf(st->ids[i]);
        char full[420];
        wchar_t wfull[420];
        if (!L) continue;
        _snprintf(full, 420, "%s @0x%llX", L->name, (unsigned long long)L->address);
        os_utf8_to_wide_buf(full, wfull, 420);
        SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)wfull);
    }
}

static LRESULT CALLBACK pick_proc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE: {
        PickState* st = (PickState*)calloc(1, sizeof(PickState));
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        if (!st) return -1;
        if (cs) st->result = (int*)cs->lpCreateParams;
        SetWindowLongPtrW(dlg, GWLP_USERDATA, (LONG_PTR)st);
        CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                      10, 10, 380, 24, dlg, (HMENU)IDD_PICK_EDIT, g_app.hInst, NULL);
        CreateWindowW(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                      10, 40, 380, 270, dlg, (HMENU)IDD_PICK_LIST, g_app.hInst, NULL);
        CreateWindowW(L"BUTTON", L"确定", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                      270, 320, 60, 26, dlg, (HMENU)IDD_PICK_OK, g_app.hInst, NULL);
        CreateWindowW(L"BUTTON", L"取消", WS_CHILD | WS_VISIBLE,
                      340, 320, 60, 26, dlg, (HMENU)IDD_PICK_CANCEL, g_app.hInst, NULL);
        return 0;
    }
    case WM_DESTROY: {
        PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
        free(st);
        return 0;
    }
    case WM_SIZE:
        MoveWindow(GetDlgItem(dlg, IDD_PICK_EDIT), 10, 10, LOWORD(lParam) - 20, 24, TRUE);
        MoveWindow(GetDlgItem(dlg, IDD_PICK_LIST), 10, 40, LOWORD(lParam) - 20, HIWORD(lParam) - 90, TRUE);
        MoveWindow(GetDlgItem(dlg, IDD_PICK_OK), LOWORD(lParam) - 140, HIWORD(lParam) - 36, 60, 26, TRUE);
        MoveWindow(GetDlgItem(dlg, IDD_PICK_CANCEL), LOWORD(lParam) - 70, HIWORD(lParam) - 36, 60, 26, TRUE);
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_PICK_EDIT:
            if (HIWORD(wParam) == EN_CHANGE) pick_refresh(dlg);
            return 0;
        case IDD_PICK_LIST:
            if (HIWORD(wParam) == LBN_DBLCLK) {
                PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
                int sel = (int)SendMessageW(GetDlgItem(dlg, IDD_PICK_LIST), LB_GETCURSEL, 0, 0);
                if (sel >= 0 && sel < st->count) {
                    if (st->result) *st->result = st->ids[sel] + 1;
                    DestroyWindow(dlg);
                }
            }
            return 0;
        case IDD_PICK_OK: {
            PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            int sel = (int)SendMessageW(GetDlgItem(dlg, IDD_PICK_LIST), LB_GETCURSEL, 0, 0);
            if (sel < 0) sel = 0;
            if (st->count > 0 && sel >= 0 && sel < st->count) {
                if (st->result) *st->result = st->ids[sel] + 1;
                DestroyWindow(dlg);
            }
            return 0;
        }
        case IDD_PICK_CANCEL:
        {
            PickState* st = (PickState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            if (st && st->result) *st->result = 0;
            DestroyWindow(dlg);
            return 0;
        }
        }
        break;
    }
    return DefWindowProcW(dlg, msg, wParam, lParam);
}

int os_dlg_pick_var(HWND owner, int* out_leaf_id)
{
    HWND dlg;
    int res = 0;
    if (g_app.leaf_count <= 0) {
        MessageBoxW(owner, L"请先加载 ELF 文件", L"选择变量", MB_OK | MB_ICONINFORMATION);
        return -1;
    }
    dlg = CreateWindowExW(WS_EX_DLGMODALFRAME, L"OSDlgPick", L"添加变量（模糊搜索）",
                          WS_POPUP | WS_CAPTION | WS_SYSMENU,
                          CW_USEDEFAULT, CW_USEDEFAULT, 420, 370, owner, NULL, g_app.hInst, &res);
    if (!dlg) return -1;
    run_modal(dlg, owner);
    if (res > 0) {
        if (out_leaf_id) *out_leaf_id = res - 1;
        return 0;
    }
    return -1;
}

int os_dlg_edit_value(HWND owner, int leaf_id)
{
    const OS_Leaf* L = os_vartree_leaf(leaf_id);
    HWND dlg;
    wchar_t wname[420], wcur[80];
    wchar_t text_out[512];
    int res = 0;
    if (!L) return -1;
    dlg = CreateWindowExW(WS_EX_DLGMODALFRAME, L"OSDlgEdit", L"写入变量值",
                          WS_POPUP | WS_CAPTION | WS_SYSMENU,
                          CW_USEDEFAULT, CW_USEDEFAULT, 440, 160, owner, NULL, g_app.hInst, &res);
    if (!dlg) return -1;
    os_utf8_to_wide_buf(L->name, wname, 420);
    os_utf8_to_wide_buf(L->sample.text, wcur, 80);
    {
        EditState* es = (EditState*)calloc(1, sizeof(EditState));
        if (es) {
            es->result = &res;
            es->text_out = text_out;
            SetWindowLongPtrW(dlg, GWLP_USERDATA, (LONG_PTR)es);
        }
    }
    CreateWindowW(L"EDIT", wcur, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                  10, 46, 420, 26, dlg, (HMENU)IDD_EDIT_VALUE, g_app.hInst, NULL);
    CreateWindowW(L"BUTTON", L"确定", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                  300, 88, 60, 26, dlg, (HMENU)IDD_EDIT_OK, g_app.hInst, NULL);
    CreateWindowW(L"BUTTON", L"取消", WS_CHILD | WS_VISIBLE,
                  370, 88, 60, 26, dlg, (HMENU)IDD_EDIT_CANCEL, g_app.hInst, NULL);
    {
        wchar_t wlabel[520];
        _snwprintf(wlabel, 520, L"%s  @0x%llX   当前: %s",
                   wname, (unsigned long long)L->address, wcur[0] ? wcur : L"(未读取)");
        CreateWindowW(L"STATIC", wlabel, WS_CHILD | WS_VISIBLE,
                      10, 12, 420, 26, dlg, NULL, g_app.hInst, NULL);
    }
    run_modal(dlg, owner);
    if (res == 1) {
        char text[512];
        char err[256];
        WideCharToMultiByte(CP_UTF8, 0, text_out, -1, text, sizeof(text), NULL, NULL);
        if (os_ds_write_leaf(leaf_id, text, err, sizeof(err)) != 0) {
            wchar_t werr[300];
            os_utf8_to_wide_buf(err, werr, 300);
            MessageBoxW(owner, werr, L"写入失败", MB_OK | MB_ICONERROR);
            return -1;
        }
        return 0;
    }
    return -1;
}

static LRESULT CALLBACK edit_proc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_DESTROY: {
        EditState* st = (EditState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
        free(st);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDD_EDIT_OK:
        {
            EditState* st = (EditState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            if (st) {
                if (st->result) *st->result = 1;
                if (st->text_out)
                    GetWindowTextW(GetDlgItem(dlg, IDD_EDIT_VALUE), st->text_out, 512);
            }
            DestroyWindow(dlg);
            return 0;
        }
        case IDD_EDIT_CANCEL:
        {
            EditState* st = (EditState*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
            if (st && st->result) *st->result = 0;
            DestroyWindow(dlg);
            return 0;
        }
        }
        break;
    }
    return DefWindowProcW(dlg, msg, wParam, lParam);
}

/* ---------- 框架回调 ---------- */

int os_fw_pick_variable(HWND parent, char* out, int out_len)
{
    int id = -1;
    const OS_Leaf* L;
    if (os_dlg_pick_var(parent, &id) != 0) return 0;
    L = os_vartree_leaf(id);
    if (!L) return 0;
    _snprintf(out, out_len, "%s", L->name);
    return 1;
}

int os_fw_write_leaf(int id, double value, char* err, int err_len)
{
    char text[64];
    _snprintf(text, 64, "%.17g", value);
    return os_ds_write_leaf(id, text, err, err_len);
}

void os_fw_on_elf_reloaded(void)
{
    os_mainwin_reload_elf();
}

/* ---------- 主窗口 ---------- */

static void tree_context_menu(HWND hwnd, LPARAM lParam)
{
    HTREEITEM h = TreeView_GetSelection(g_app.hTree);
    LPARAM lp = -1;
    HMENU m;
    POINT pt;
    (void)hwnd;
    (void)lParam;
    if (h) {
        TVITEMW item;
        memset(&item, 0, sizeof(item));
        item.mask = TVIF_PARAM;
        item.hItem = h;
        if (TreeView_GetItem(g_app.hTree, &item)) lp = item.lParam;
    }
    m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, IDM_TREE_ALL, L"全选观测");
    AppendMenuW(m, MF_STRING, IDM_TREE_NONE, L"清除全部观测");
    AppendMenuW(m, MF_STRING | (lp <= 0 ? MF_GRAYED : 0), IDM_TREE_WRITE, L"写入值...");
    AppendMenuW(m, MF_SEPARATOR, 0, NULL);
    AppendMenuW(m, MF_STRING, IDM_TREE_RELOAD, L"重新加载 ELF");
    GetCursorPos(&pt);
    TrackPopupMenu(m, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_app.hMain, NULL);
    DestroyMenu(m);
}

static void tree_select_all(int on)
{
    int i;
    for (i = 0; i < g_app.leaf_count; i++) {
        os_vartree_set_watch(i, on);
    }
    os_vartree_fill_tree(g_app.hTree);
    refresh_status();
    os_mainwin_update_buttons();
}

LRESULT CALLBACK os_mainwin_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE: {
        static const struct { int id; const wchar_t* text; } btns[] = {
            { IDC_BTN_OPEN, L"打开ELF" }, { IDC_BTN_CONNECT, L"连接" },
            { IDC_BTN_DISCON, L"断开" }, { IDC_BTN_START, L"开始采集" },
            { IDC_BTN_STOP, L"停止采集" }, { IDC_BTN_LOGSTART, L"记录" },
            { IDC_BTN_LOGSTOP, L"停止记录" }, { IDC_BTN_REPLAY, L"离线回放" },
            { IDC_BTN_REPLAYSTOP, L"停止回放" }, { IDC_BTN_ABOUT, L"关于" },
        };
        HMENU mFile, mAcq, mLog, mHelp;
        int i;
        g_app.hMain = hwnd;
        g_app.hBtnBar = hwnd;
        for (i = 0; i < (int)(sizeof(btns) / sizeof(btns[0])); i++) {
            HWND b = CreateWindowW(L"BUTTON", btns[i].text,
                                   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                   6 + i * 84, 5, 80, 24, hwnd, (HMENU)(INT_PTR)btns[i].id,
                                   g_app.hInst, NULL);
            SendMessageW(b, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        }
        g_app.hTree = CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEWW, L"",
                                      WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS |
                                      TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_CHECKBOXES,
                                      0, 34, 340, 400, hwnd, NULL, g_app.hInst, NULL);
        SendMessageW(g_app.hTree, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        TreeView_SetUnicodeFormat(g_app.hTree, TRUE);
        g_app.hSplitV = CreateWindowW(L"OSSplitter", L"", WS_CHILD | WS_VISIBLE,
                                      340, 34, 5, 400, hwnd, NULL, g_app.hInst, NULL);
        g_app.hRight = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_OWNERDRAW,
                                     345, 34, 500, 400, hwnd, NULL, g_app.hInst, NULL);
        g_app.hLog = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
                                     WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
                                     0, 500, 800, 170, hwnd, NULL, g_app.hInst, NULL);
        ListView_SetExtendedListViewStyle(g_app.hLog, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        {
            LVCOLUMNW col;
            memset(&col, 0, sizeof(col));
            col.mask = LVCF_TEXT | LVCF_WIDTH;
            col.cx = 90;
            col.pszText = L"时间";
            ListView_InsertColumn(g_app.hLog, 0, &col);
            col.cx = 50;
            col.pszText = L"级别";
            ListView_InsertColumn(g_app.hLog, 1, &col);
            col.cx = 640;
            col.pszText = L"消息";
            ListView_InsertColumn(g_app.hLog, 2, &col);
        }
        g_app.hStatus = CreateWindowW(STATUSCLASSNAMEW, L"", WS_CHILD | WS_VISIBLE,
                                      0, 0, 0, 0, hwnd, NULL, g_app.hInst, NULL);
        /* 菜单 */
        mFile = CreateMenu();
        mAcq = CreateMenu();
        mLog = CreateMenu();
        g_menu_win = CreateMenu();
        mHelp = CreateMenu();
        AppendMenuW(mFile, MF_STRING, IDC_BTN_OPEN, L"打开 ELF 文件...\tCtrl+O");
        AppendMenuW(mFile, MF_SEPARATOR, 0, NULL);
        AppendMenuW(mFile, MF_STRING, IDM_EXIT, L"退出\tAlt+F4");
        AppendMenuW(mAcq, MF_STRING, IDC_BTN_CONNECT, L"连接...\tF5");
        AppendMenuW(mAcq, MF_STRING, IDC_BTN_DISCON, L"断开\tF6");
        AppendMenuW(mAcq, MF_SEPARATOR, 0, NULL);
        AppendMenuW(mAcq, MF_STRING, IDC_BTN_START, L"开始采集\tF7");
        AppendMenuW(mAcq, MF_STRING, IDC_BTN_STOP, L"停止采集\tF8");
        AppendMenuW(mLog, MF_STRING, IDC_BTN_LOGSTART, L"开始 CSV 记录...");
        AppendMenuW(mLog, MF_STRING, IDC_BTN_LOGSTOP, L"停止记录");
        AppendMenuW(mLog, MF_SEPARATOR, 0, NULL);
        AppendMenuW(mLog, MF_STRING, IDC_BTN_REPLAY, L"离线回放...");
        AppendMenuW(mLog, MF_STRING, IDC_BTN_REPLAYSTOP, L"停止回放");
        AppendMenuW(mHelp, MF_STRING, IDC_BTN_ABOUT, L"关于 OpenScope");
        g_menu = CreateMenu();
        AppendMenuW(g_menu, MF_POPUP, (UINT_PTR)mFile, L"文件(&F)");
        AppendMenuW(g_menu, MF_POPUP, (UINT_PTR)mAcq, L"采集(&A)");
        AppendMenuW(g_menu, MF_POPUP, (UINT_PTR)mLog, L"记录/回放(&L)");
        AppendMenuW(g_menu, MF_POPUP, (UINT_PTR)g_menu_win, L"窗口(&W)");
        AppendMenuW(g_menu, MF_POPUP, (UINT_PTR)mHelp, L"帮助(&H)");
        SetMenu(hwnd, g_menu);
        os_mainwin_rebuild_window_menu();
        SetTimer(hwnd, 1, 2000, NULL);
        os_log_set(os_mainwin_append_log);
        os_log(OS_LOG_INFO, "OpenScope 已启动");
        refresh_status();
        os_mainwin_update_buttons();
        return 0;
    }
    case WM_SIZE:
        layout();
        return 0;
    case WM_OS_SPLIT:
        if ((int)wParam > 120 && (int)wParam < 900) g_app.tree_w = (int)wParam;
        layout();
        return 0;
    case WM_TIMER:
        if (wParam == 1) check_elf_mtime();
        else if (wParam == 2) os_replay_tick();
        return 0;
    case WM_OS_SAMPLES:
        os_ds_drain();
        refresh_status();
        return 0;
    case WM_OS_ACQ_STATE:
        refresh_status();
        os_mainwin_update_buttons();
        return 0;
    case WM_OS_WIN_CLOSED: {
        HWND w = (HWND)wParam;
        int i;
        for (i = 0; i < g_app.win_count; i++) {
            if (g_app.wins[i].hwnd == w) {
                OS_WinItem* wi = &g_app.wins[i];
                if (wi->is_module && wi->mod && wi->mod->destroy_window)
                    wi->mod->destroy_window(wi->mod_ctx, w);
                if (IsWindow(w)) DestroyWindow(w);
                memmove(&g_app.wins[i], &g_app.wins[i + 1],
                        sizeof(OS_WinItem) * (g_app.win_count - i - 1));
                g_app.win_count--;
                break;
            }
        }
        os_mainwin_tile();
        return 0;
    }
    case WM_NOTIFY: {
        LPNMHDR h = (LPNMHDR)lParam;
        if (h && h->hwndFrom == g_app.hTree) {
            if (h->code == NM_RCLICK) {
                tree_context_menu(hwnd, lParam);
                return 0;
            }
            if (h->code == TVN_ITEMCHANGEDW) {
                NMTVITEMCHANGE* p = (NMTVITEMCHANGE*)lParam;
                if (p->uChanged & TVIF_STATE) {
                    LPARAM lp = p->lParam;
                    if (lp > 0) {
                        int id = (int)(lp - 1);
                        int checked = ((p->uStateNew & TVIS_STATEIMAGEMASK) >> 12) == 2;
                        os_vartree_set_watch(id, checked);
                        refresh_status();
                        os_mainwin_update_buttons();
                    }
                }
                return 0;
            }
        }
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BTN_OPEN: cmd_open_elf(); break;
        case IDC_BTN_CONNECT: cmd_connect(); break;
        case IDC_BTN_DISCON: cmd_disconnect(); break;
        case IDC_BTN_START: cmd_start_acq(); break;
        case IDC_BTN_STOP: cmd_stop_acq(); break;
        case IDC_BTN_LOGSTART: cmd_log_start(); break;
        case IDC_BTN_LOGSTOP: cmd_log_stop(); break;
        case IDC_BTN_REPLAY: cmd_replay_open(); break;
        case IDC_BTN_REPLAYSTOP: cmd_replay_stop(); break;
        case IDC_BTN_ABOUT:
            MessageBoxW(hwnd, L"OpenScope v0.1\n\nMCU 变量采集与标定工具（类 CANape）\n"
                              L"C + Win32 + 动态模块架构", L"关于", MB_OK | MB_ICONINFORMATION);
            break;
        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;
        case IDM_WIN_CHART: cmd_add_window(1); break;
        case IDM_WIN_NUM: cmd_add_window(0); break;
        case IDM_TREE_ALL: tree_select_all(1); break;
        case IDM_TREE_NONE: tree_select_all(0); break;
        case IDM_TREE_RELOAD: os_mainwin_reload_elf(); break;
        case IDM_TREE_WRITE: {
            HTREEITEM h = TreeView_GetSelection(g_app.hTree);
            if (h) {
                TVITEMW item;
                memset(&item, 0, sizeof(item));
                item.mask = TVIF_PARAM;
                item.hItem = h;
                if (TreeView_GetItem(g_app.hTree, &item) && item.lParam > 0)
                    os_dlg_edit_value(hwnd, (int)(item.lParam - 1));
            }
            break;
        }
        default:
            if (LOWORD(wParam) >= IDM_WIN_MODULE_BASE &&
                LOWORD(wParam) < IDM_WIN_MODULE_BASE + g_modwin_menu_count) {
                int idx = LOWORD(wParam) - IDM_WIN_MODULE_BASE;
                ModWinMenuItem* it = &g_modwin_menu[idx];
                HWND h;
                wchar_t title[128];
                if (it->mod && it->mod->create_window) {
                    os_utf8_to_wide_buf(it->wt->display_name ? it->wt->display_name : it->wt->type,
                                        title, 128);
                    h = it->mod->create_window(it->ctx, it->wt->type, g_app.hRight,
                                               0, 0, 200, 150, title);
                    if (h) add_win_item(h, 1, it->mod, it->ctx, title);
                }
            }
            break;
        }
        return 0;
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        KillTimer(hwnd, 2);
        os_ds_stop();
        os_datalog_stop();
        os_replay_stop();
        os_modmgr_shutdown();
        if (g_app.elf) os_elf_close(g_app.elf);
        g_app.elf = NULL;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void os_mainwin_register(void)
{
    WNDCLASSW wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = os_mainwin_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = g_main_class;
    RegisterClassW(&wc);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = split_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_SIZEWE);
    wc.lpszClassName = L"OSSplitter";
    RegisterClassW(&wc);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = pick_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"OSDlgPick";
    RegisterClassW(&wc);
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = edit_proc;
    wc.hInstance = g_app.hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"OSDlgEdit";
    RegisterClassW(&wc);
}
