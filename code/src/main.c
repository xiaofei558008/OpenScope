#include "app.h"
#include "chartwin.h"
#include "numwin.h"
#include "mainwin.h"
#include "module_mgr.h"
#include "vartree.h"
#include <string.h>
#include <commctrl.h>

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

OS_App g_app;

static void init_fw(void)
{
    memset(&g_app.fw, 0, sizeof(g_app.fw));
    g_app.fw.api_version = OS_API_VERSION;
    g_app.fw.log = os_fw_log;
    g_app.fw.post_msg = os_fw_post;
    g_app.fw.find_variable = os_fw_find;
    g_app.fw.leaf_count = os_fw_leaf_count;
    g_app.fw.leaf_name = os_fw_leaf_name;
    g_app.fw.leaf_sample = os_fw_leaf_sample;
    g_app.fw.pick_variable = os_fw_pick_variable;
    g_app.fw.write_leaf = os_fw_write_leaf;
    g_app.fw.on_elf_reloaded = os_fw_on_elf_reloaded;
    g_app.fw.leaf_find = os_fw_leaf_find;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX icc;
    MSG msg;
    HWND hMain;
    (void)hPrevInstance;
    (void)lpCmdLine;
    g_app.hInst = hInstance;
    g_app.tree_w = 340;
    g_app.log_h = 170;
    g_app.poll_interval_ms = 20;
    InitializeCriticalSection(&g_app.ring_cs);
    init_fw();
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES | ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icc);
    os_chart_register();
    os_num_register();
    os_mainwin_register();
    hMain = CreateWindowW(L"OpenScopeMain", L"OpenScope - MCU 变量采集与标定",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 1280, 800,
                          NULL, NULL, hInstance, NULL);
    if (!hMain) return 1;
    os_log_set(os_mainwin_append_log);
    os_modmgr_load();
    os_mainwin_rebuild_window_menu();
    ShowWindow(hMain, nCmdShow);
    UpdateWindow(hMain);
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    DeleteCriticalSection(&g_app.ring_cs);
    return (int)msg.wParam;
}
