#include "app.h"
#include "chartwin.h"
#include "numwin.h"
#include "mainwin.h"
#include "module_mgr.h"
#include "vartree.h"
#include "datalog.h"
#include "layout.h"
#include "util.h"
#include <string.h>
#include <commctrl.h>

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

OS_App g_app;

/* 崩溃处理器：异常后把代码与栈写入 openscope.log 再终止，避免闪退无痕 */
typedef USHORT(WINAPI* os_capture_stack_fn)(ULONG, ULONG, PVOID*, PULONG);

static void crash_write(const char* fmt, ...)
{
    char line[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    os_log_file_write_raw(line);
}

static LONG WINAPI os_crash_filter(EXCEPTION_POINTERS* ep)
{
    char ts[40];
    HMODULE hk = GetModuleHandleA("kernel32.dll");
    os_capture_stack_fn cap = hk ? (os_capture_stack_fn)GetProcAddress(hk, "CaptureStackBackTrace") : NULL;
    if (!cap) {
        HMODULE hb = GetModuleHandleA("KERNELBASE.dll");
        if (hb) cap = (os_capture_stack_fn)GetProcAddress(hb, "CaptureStackBackTrace");
    }
    os_time_iso(os_time_us(), ts, sizeof(ts));
    if (ep && ep->ExceptionRecord) {
        HMODULE hm = NULL;
        char mod[260] = "?";
        DWORD64 off = 0;
        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                               (LPCSTR)ep->ExceptionRecord->ExceptionAddress, &hm) && hm) {
            GetModuleFileNameA(hm, mod, sizeof(mod));
            off = (DWORD64)((BYTE*)ep->ExceptionRecord->ExceptionAddress - (BYTE*)hm);
        }
        crash_write("%s [FATAL] unhandled exception code=0x%08X at 0x%p  %s+0x%llX",
                    ts, ep->ExceptionRecord->ExceptionCode,
                    ep->ExceptionRecord->ExceptionAddress, mod, (unsigned long long)off);
    } else {
        crash_write("%s [FATAL] unhandled exception (no record)", ts);
    }
    if (cap) {
        PVOID frames[32];
        USHORT n = cap(0, 32, frames, NULL);
        USHORT i;
        for (i = 0; i < n; i++) {
            HMODULE hmod = NULL;
            char mod[260] = "?";
            DWORD64 off = 0;
            if (frames[i] &&
                GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   (LPCSTR)frames[i], &hmod) && hmod) {
                GetModuleFileNameA(hmod, mod, sizeof(mod));
                off = (DWORD64)((BYTE*)frames[i] - (BYTE*)hmod);
            }
            crash_write("%s [FATAL]   #%02u 0x%p  %s+0x%llX",
                        ts, (unsigned)i, frames[i], mod, (unsigned long long)off);
        }
    }
    if (ep && ep->ContextRecord) {
        /* 手动 RBP 帧链回溯（x64） */
        DWORD64* frame = (DWORD64*)ep->ContextRecord->Rbp;
        USHORT i;
        for (i = 0; i < 24 && frame &&
             (ULONG_PTR)frame > 0x10000 &&
             (ULONG_PTR)frame < 0x7FFFFFFFFFFFULL; i++) {
            DWORD64 ret = frame[1];
            if (ret < 0x10000 || ret > 0x7FFFFFFFFFFFULL) break;
            {
                HMODULE hmod = NULL;
                char mod[260] = "?";
                DWORD64 off = 0;
                if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                       (LPCSTR)ret, &hmod) && hmod) {
                    GetModuleFileNameA(hmod, mod, sizeof(mod));
                    off = ret - (DWORD64)(BYTE*)hmod;
                }
                crash_write("%s [FATAL]   #%02u 0x%llX  %s+0x%llX",
                            ts, (unsigned)i, ret, mod, (unsigned long long)off);
            }
            frame = (DWORD64*)*frame;
        }
    }
    MessageBoxW(NULL, L"OpenScope 发生未处理异常，详细信息已写入日志文件 openscope.log。",
                L"OpenScope 崩溃", MB_OK | MB_ICONERROR);
    return EXCEPTION_EXECUTE_HANDLER;
}

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

/* 解析命令行：OpenScope.exe [elf] [--select-leaf=名] [--layout-load=文件] [--layout-save=文件] */
static void parse_cmdline(wchar_t* cmd, wchar_t** elf, wchar_t** select_leaf,
                          wchar_t** layout_load, wchar_t** layout_save, int* no_layout)
{
    wchar_t* tok = cmd;
    int first = 1;
    *elf = NULL;
    *select_leaf = NULL;
    *layout_load = NULL;
    *layout_save = NULL;
    *no_layout = 0;
    while (tok && *tok) {
        wchar_t* sp;
        while (*tok == L' ') tok++;
        if (!*tok) break;
        sp = wcschr(tok, L' ');
        if (sp) *sp = 0; /* 先截断尾部空格再匹配，避免 "--no-layout " 匹配失败 */
        if (wcsncmp(tok, L"--select-leaf=", 14) == 0) *select_leaf = tok + 14;
        else if (wcsncmp(tok, L"--layout-load=", 14) == 0) *layout_load = tok + 14;
        else if (wcsncmp(tok, L"--layout-save=", 14) == 0) *layout_save = tok + 14;
        else if (wcscmp(tok, L"--no-layout") == 0) *no_layout = 1;
        else if (wcsncmp(tok, L"--shot=", 7) == 0) _snwprintf(g_app.shot_path,
                                                             MAX_PATH, L"%s", tok + 7);
        else if (wcsncmp(tok, L"--rename-tab=", 13) == 0) _snwprintf(g_app.rename_tab,
                                                                     MAX_PATH, L"%s", tok + 13);
        else if (wcsncmp(tok, L"--replay=", 9) == 0) _snwprintf(g_app.replay_path,
                                                                MAX_PATH, L"%s", tok + 9);
        else if (first) { *elf = tok; first = 0; }
        tok = sp ? sp + 1 : NULL;
    }
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
    os_log_file_auto_open();
    SetUnhandledExceptionFilter(os_crash_filter);
    os_log(OS_LOG_INFO, "OpenScope 启动 (version 1.8.2)");
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
    os_mainwin_update_buttons(); /* 模块加载后启用“连接”按钮 */
    os_mainwin_rebuild_window_menu();
    os_mainwin_cfg_init(); /* 主界面连接配置：接口/速度/J-Link 设备列表 */
    ShowWindow(hMain, nCmdShow);
    UpdateWindow(hMain);
    {
        wchar_t* elf = NULL, *sel = NULL, *llo = NULL, *lsv = NULL;
        int no_layout = 0;
        parse_cmdline(lpCmdLine, &elf, &sel, &llo, &lsv, &no_layout);
        if (!no_layout) {
            if (llo) os_layout_load_from(llo);
            else os_layout_restore_auto(); /* 恢复上次调试界面布局 */
        }
        if (elf) os_mainwin_open_elf(elf);
        if (sel)
            os_log(OS_LOG_INFO, "命令行选中叶变量: %ls (rc=%d)",
                   sel, os_vartree_select_leaf(g_app.hTree, sel));
        if (g_app.replay_path[0] && os_replay_start(g_app.replay_path) == 0)
            SetTimer(hMain, 2, 10, NULL); /* 测试钩子：--replay 自动开始离线回放 */
        if (!no_layout && lsv) os_layout_save_to(lsv);
    }
    while (GetMessage(&msg, NULL, 0, 0)) {
        /* N3: 就地重命名编辑框的回车/ESC 在分发前拦截（不子类化编辑框） */
        HWND te = os_tab_edit_hwnd();
        if (te && msg.message == WM_KEYDOWN && msg.hwnd == te &&
            (msg.wParam == VK_RETURN || msg.wParam == VK_ESCAPE)) {
            os_tab_edit_handle_key(msg.wParam);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    DeleteCriticalSection(&g_app.ring_cs);
    return (int)msg.wParam;
}
