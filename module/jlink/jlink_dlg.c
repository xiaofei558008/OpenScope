/*
 * OpenScope J-Link 配置对话框（OS_CMD_CONFIGURE 实现）
 *
 * 内容：仿真接口（SWD/JTAG）、时钟速度、目标器件型号、J-Link 设备扫描列表、
 *       连接 / 断开按钮。未发现设备时弹窗提示（FR7）。
 */
#include "jlink.h"
#include "module_api.h"

#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IDC_IFACE      1001
#define IDC_SPEED      1002
#define IDC_DEVICE     1003
#define IDC_LIST       1004
#define IDC_REFRESH    1005
#define IDC_CONNECT    1006
#define IDC_DISCONNECT 1007
#define IDC_STATUS     1008

#define DLG_W 430
#define DLG_H 330

typedef struct DlgData {
    int probe_index;
    int iface;
    int speed_khz;
    char device[128];
} DlgData;

static void os_w2u(const wchar_t* w, char* s, int cap)
{
    if (!w || cap <= 0) return;
    WideCharToMultiByte(CP_UTF8, 0, w, -1, s, cap, NULL, NULL);
    s[cap - 1] = 0;
}

static void os_u2w(const char* s, wchar_t* w, int cap)
{
    if (!s || cap <= 0) return;
    MultiByteToWideChar(CP_UTF8, 0, s, -1, w, cap);
    w[cap - 1] = 0;
}

static BOOL CALLBACK dlg_set_font_child(HWND c, LPARAM lp)
{
    SendMessageW(c, WM_SETFONT, (WPARAM)lp, TRUE);
    return TRUE;
}

static void dlg_center(HWND hwnd, HWND parent)
{
    RECT r, p;
    int x, y;
    GetWindowRect(hwnd, &r);
    if (parent && IsWindow(parent)) {
        GetWindowRect(parent, &p);
    } else {
        p.left = 0;
        p.top = 0;
        p.right = GetSystemMetrics(SM_CXSCREEN);
        p.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    x = p.left + ((p.right - p.left) - (r.right - r.left)) / 2;
    y = p.top + ((p.bottom - p.top) - (r.bottom - r.top)) / 2;
    SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

static HWND dlg_make(HWND parent, const wchar_t* cls, const wchar_t* text,
                     int id, int x, int y, int w, int h, DWORD style)
{
    return CreateWindowExW(0, cls, text, style | WS_CHILD | WS_VISIBLE,
                           x, y, w, h, parent, (HMENU)(INT_PTR)id,
                           GetModuleHandleW(NULL), NULL);
}

static void dlg_set_status(HWND dlg, const wchar_t* text)
{
    HWND h = GetDlgItem(dlg, IDC_STATUS);
    if (h) SetWindowTextW(h, text);
}

static void dlg_scan(HWND dlg)
{
    OS_DeviceInfo items[16];
    HWND list;
    int n, i;
    list = GetDlgItem(dlg, IDC_LIST);
    SendMessageW(list, LB_RESETCONTENT, 0, 0);
    n = os_jlink_scan_devices(items, 16);
    if (n < 0) {
        dlg_set_status(dlg, L"扫描失败：JLink_x64.dll 未加载");
        return;
    }
    if (n == 0) {
        MessageBoxW(dlg, L"没有发现 JLink 设备，请检查 USB 连接与驱动。",
                    L"J-Link 扫描", MB_OK | MB_ICONWARNING);
        dlg_set_status(dlg, L"未发现 J-Link 设备");
        return;
    }
    for (i = 0; i < n; i++) {
        wchar_t line[256];
        _snwprintf(line, 256, L"[%d] %hs (SN: %hs)", items[i].index,
                   items[i].name, items[i].serial);
        SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)line);
    }
    SendMessageW(list, LB_SETCURSEL, 0, 0);
    {
        wchar_t st[128];
        _snwprintf(st, 128, L"发现 %d 个 J-Link 设备", n);
        dlg_set_status(dlg, st);
    }
}

static void dlg_connect(HWND dlg, DlgData* d)
{
    OS_ConnectCfg* cfg = os_jlink_cfg();
    wchar_t wbuf[256];
    char err[256] = "";
    int rc;
    LRESULT sel;
    sel = SendMessageW(GetDlgItem(dlg, IDC_LIST), LB_GETCURSEL, 0, 0);
    d->probe_index = (sel == LB_ERR) ? -1 : (int)sel;
    d->iface = (int)SendMessageW(GetDlgItem(dlg, IDC_IFACE), CB_GETCURSEL, 0, 0);
    if (d->iface != OS_IF_JTAG) d->iface = OS_IF_SWD;
    d->speed_khz = (int)SendMessageW(GetDlgItem(dlg, IDC_SPEED), CB_GETCURSEL, 0, 0);
    GetDlgItemTextW(dlg, IDC_DEVICE, wbuf, 256);
    os_w2u(wbuf, d->device, sizeof(d->device));

    cfg->iface = d->iface;
    cfg->speed_khz = d->speed_khz;
    cfg->probe_index = d->probe_index;
    _snprintf(cfg->device, sizeof(cfg->device), "%s", d->device);

    rc = os_jlink_connect_now(err, sizeof(err));
    if (rc == OS_ERR_OK) {
        dlg_set_status(dlg, L"已连接 MCU");
        MessageBoxW(dlg, L"已连接 MCU。", L"J-Link", MB_OK | MB_ICONINFORMATION);
    } else {
        wchar_t werr[300];
        os_u2w(err[0] ? err : "连接失败", werr, 300);
        dlg_set_status(dlg, L"连接失败");
        MessageBoxW(dlg, werr, L"J-Link 连接失败", MB_OK | MB_ICONERROR);
    }
}

static void dlg_init(HWND dlg)
{
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    HWND h;
    const wchar_t* ifaces[] = { L"SWD", L"JTAG" };
    const int speeds[] = { 0, 100, 400, 1000, 2000, 4000, 5000 };
    int i;
    OS_ConnectCfg* cfg = os_jlink_cfg();

    dlg_make(dlg, L"STATIC", L"仿真接口:", 0, 14, 14, 90, 18, SS_LEFT);
    h = dlg_make(dlg, L"COMBOBOX", L"", IDC_IFACE, 110, 12, 120, 120,
                 CBS_DROPDOWNLIST | WS_TABSTOP);
    for (i = 0; i < 2; i++) SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)ifaces[i]);
    SendMessageW(h, CB_SETCURSEL, cfg->iface == OS_IF_JTAG ? 1 : 0, 0);

    dlg_make(dlg, L"STATIC", L"时钟速度(kHz):", 0, 14, 44, 90, 18, SS_LEFT);
    h = dlg_make(dlg, L"COMBOBOX", L"", IDC_SPEED, 110, 42, 120, 160,
                 CBS_DROPDOWNLIST | WS_TABSTOP);
    for (i = 0; i < 7; i++) {
        wchar_t buf[32];
        if (speeds[i] == 0) _snwprintf(buf, 32, L"0 (自动)");
        else _snwprintf(buf, 32, L"%d", speeds[i]);
        SendMessageW(h, CB_ADDSTRING, 0, (LPARAM)buf);
    }
    SendMessageW(h, CB_SETCURSEL, 5, 0); /* 默认 4000 */

    dlg_make(dlg, L"STATIC", L"目标器件:", 0, 14, 74, 90, 18, SS_LEFT);
    h = dlg_make(dlg, L"EDIT", L"", IDC_DEVICE, 110, 72, 200, 22,
                 WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP);
    if (cfg->device[0]) {
        wchar_t wd[128];
        os_u2w(cfg->device, wd, 128);
        SetWindowTextW(h, wd);
    } else {
        SetWindowTextW(h, L"STM32F407VG");
    }

    dlg_make(dlg, L"STATIC", L"J-Link 设备:", 0, 14, 106, 200, 18, SS_LEFT);
    dlg_make(dlg, L"LISTBOX", L"", IDC_LIST, 14, 124, 296, 130,
             WS_BORDER | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP);
    dlg_make(dlg, L"BUTTON", L"刷新", IDC_REFRESH, 320, 124, 90, 26,
             BS_PUSHBUTTON | WS_TABSTOP);
    dlg_make(dlg, L"BUTTON", L"连接", IDC_CONNECT, 320, 158, 90, 26,
             BS_PUSHBUTTON | WS_TABSTOP);
    dlg_make(dlg, L"BUTTON", L"断开", IDC_DISCONNECT, 320, 192, 90, 26,
             BS_PUSHBUTTON | WS_TABSTOP);
    dlg_make(dlg, L"BUTTON", L"关闭", IDOK, 320, 260, 90, 26,
             BS_PUSHBUTTON | WS_TABSTOP);
    dlg_make(dlg, L"STATIC", L"就绪", IDC_STATUS, 14, 264, 296, 40, SS_LEFT);

    SendMessageW(dlg, WM_SETFONT, (WPARAM)font, TRUE);
    EnumChildWindows(dlg, dlg_set_font_child, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
}

static LRESULT CALLBACK cfg_proc(HWND dlg, UINT msg, WPARAM wp, LPARAM lp)
{
    DlgData* d = (DlgData*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDC_REFRESH:
            dlg_scan(dlg);
            return 0;
        case IDC_CONNECT:
            if (d) dlg_connect(dlg, d);
            return 0;
        case IDC_DISCONNECT:
            os_jlink_disconnect_now();
            dlg_set_status(dlg, L"已断开");
            return 0;
        case IDOK:
        case IDCANCEL:
            DestroyWindow(dlg);
            return 0;
        }
        return 0;
    case WM_CLOSE:
        DestroyWindow(dlg);
        return 0;
    case WM_DESTROY:
        if (d) free(d);
        return 0;
    }
    return DefWindowProcW(dlg, msg, wp, lp);
}

int os_jlink_show_config_dialog(HWND parent)
{
    static const wchar_t* cls = L"OSJLinkCfg";
    WNDCLASSEXW wc;
    HWND dlg;
    MSG msg;
    DlgData* d;

    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = cfg_proc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = cls;
    RegisterClassExW(&wc);

    d = (DlgData*)calloc(1, sizeof(DlgData));
    if (!d) return OS_ERR_FAIL;
    d->probe_index = -1;
    d->iface = OS_IF_SWD;
    d->speed_khz = 4000;

    dlg = CreateWindowExW(WS_EX_DLGMODALFRAME, cls, L"J-Link 连接配置",
                          WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                          CW_USEDEFAULT, CW_USEDEFAULT, DLG_W, DLG_H,
                          parent, NULL, GetModuleHandleW(NULL), NULL);
    if (!dlg) {
        free(d);
        return OS_ERR_FAIL;
    }
    SetWindowLongPtrW(dlg, GWLP_USERDATA, (LONG_PTR)d);
    dlg_init(dlg);
    dlg_center(dlg, parent);
    dlg_scan(dlg);

    /* 模态循环 */
    while (IsWindow(dlg) && GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (!IsDialogMessageW(dlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    return OS_ERR_OK;
}
