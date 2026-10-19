/*
 * 源码目录关联配置。
 */
#include "srcmap.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <shlobj.h>

#define IDC_SM_LIST   5001
#define IDC_SM_ADD    5002
#define IDC_SM_DEL    5003
#define IDC_SM_FROM   5004
#define IDC_SM_TO     5005
#define IDC_SM_ADDMAP 5006
#define IDC_SM_OK     5007
#define IDC_SM_CANCEL 5008
#define IDC_SM_ELF    5009

static void ensure_dir(void)
{
    wchar_t dir[MAX_PATH];
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", dir, MAX_PATH) > 0) {
        _snwprintf(dir, MAX_PATH, L"%s\\OpenScope", dir);
        CreateDirectoryW(dir, NULL);
    }
}

static unsigned elf_hash(const wchar_t* path)
{
    unsigned h = 2166136261u;
    if (!path) return 0;
    while (*path) {
        h ^= (unsigned)(*path++);
        h *= 16777619u;
    }
    return h;
}

static void ini_path_for_elf(const wchar_t* elf, wchar_t* out, int cap)
{
    wchar_t dir[MAX_PATH];
    ensure_dir();
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", dir, MAX_PATH) == 0 || !dir[0]) {
        GetModuleFileNameW(NULL, dir, MAX_PATH);
        {
            wchar_t* s = wcsrchr(dir, L'\\');
            if (s) *s = 0;
        }
        _snwprintf(out, cap, L"%s\\srcmap_%08x.ini", dir, elf_hash(elf));
    } else {
        _snwprintf(out, cap, L"%s\\OpenScope\\srcmap_%08x.ini", dir, elf_hash(elf));
    }
}

void os_srcmap_init(OS_SrcMap* m)
{
    if (!m) return;
    memset(m, 0, sizeof(*m));
}

void os_srcmap_load_for_elf(OS_SrcMap* m, const wchar_t* elf_path)
{
    wchar_t path[MAX_PATH], key[32], buf[MAX_PATH];
    int i, n;
    if (!m) return;
    os_srcmap_init(m);
    if (elf_path && elf_path[0])
        _snwprintf(m->elf_path, MAX_PATH, L"%s", elf_path);
    ini_path_for_elf(m->elf_path, path, MAX_PATH);
    n = (int)GetPrivateProfileIntW(L"roots", L"count", 0, path);
    if (n > OS_SRCMAP_MAX_ROOTS) n = OS_SRCMAP_MAX_ROOTS;
    for (i = 0; i < n; i++) {
        _snwprintf(key, 32, L"r%d", i);
        GetPrivateProfileStringW(L"roots", key, L"", buf, MAX_PATH, path);
        if (buf[0]) {
            _snwprintf(m->roots[m->root_count], MAX_PATH, L"%s", buf);
            m->root_count++;
        }
    }
    n = (int)GetPrivateProfileIntW(L"maps", L"count", 0, path);
    if (n > OS_SRCMAP_MAX_MAPS) n = OS_SRCMAP_MAX_MAPS;
    for (i = 0; i < n; i++) {
        _snwprintf(key, 32, L"from%d", i);
        GetPrivateProfileStringW(L"maps", key, L"", buf, MAX_PATH, path);
        if (!buf[0]) continue;
        _snwprintf(m->from_prefix[m->map_count], MAX_PATH, L"%s", buf);
        _snwprintf(key, 32, L"to%d", i);
        GetPrivateProfileStringW(L"maps", key, L"", buf, MAX_PATH, path);
        _snwprintf(m->to_prefix[m->map_count], MAX_PATH, L"%s", buf);
        m->map_count++;
    }
}

void os_srcmap_save(const OS_SrcMap* m)
{
    wchar_t path[MAX_PATH], key[32], num[16];
    int i;
    if (!m) return;
    ini_path_for_elf(m->elf_path, path, MAX_PATH);
    _snwprintf(num, 16, L"%d", m->root_count);
    WritePrivateProfileStringW(L"roots", L"count", num, path);
    for (i = 0; i < m->root_count; i++) {
        _snwprintf(key, 32, L"r%d", i);
        WritePrivateProfileStringW(L"roots", key, m->roots[i], path);
    }
    _snwprintf(num, 16, L"%d", m->map_count);
    WritePrivateProfileStringW(L"maps", L"count", num, path);
    for (i = 0; i < m->map_count; i++) {
        _snwprintf(key, 32, L"from%d", i);
        WritePrivateProfileStringW(L"maps", key, m->from_prefix[i], path);
        _snwprintf(key, 32, L"to%d", i);
        WritePrivateProfileStringW(L"maps", key, m->to_prefix[i], path);
    }
    if (m->elf_path[0])
        WritePrivateProfileStringW(L"meta", L"elf", m->elf_path, path);
}

static int file_exists_w(const wchar_t* p)
{
    DWORD a = GetFileAttributesW(p);
    return (a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY));
}

int os_srcmap_resolve(const OS_SrcMap* m, const char* compile_path_utf8,
                      wchar_t* out_local, int out_cap)
{
    wchar_t comp[MAX_PATH], try_path[MAX_PATH];
    const wchar_t* base;
    int i;
    if (!m || !compile_path_utf8 || !out_local || out_cap <= 0) return 0;
    os_utf8_to_wide_buf(compile_path_utf8, comp, MAX_PATH);
    if (!comp[0]) return 0;

    /* 已是本机绝对路径且存在 */
    if ((comp[0] == L'\\' || (comp[0] && comp[1] == L':')) && file_exists_w(comp)) {
        _snwprintf(out_local, out_cap, L"%s", comp);
        return 1;
    }

    /* 前缀映射 */
    for (i = 0; i < m->map_count; i++) {
        size_t fl = wcslen(m->from_prefix[i]);
        if (fl > 0 && _wcsnicmp(comp, m->from_prefix[i], fl) == 0) {
            _snwprintf(try_path, MAX_PATH, L"%s%s", m->to_prefix[i], comp + fl);
            if (file_exists_w(try_path)) {
                _snwprintf(out_local, out_cap, L"%s", try_path);
                return 1;
            }
        }
    }

    /* 在源码根下按文件名查找 */
    base = wcsrchr(comp, L'/');
    if (!base) base = wcsrchr(comp, L'\\');
    base = base ? base + 1 : comp;
    for (i = 0; i < m->root_count; i++) {
        _snwprintf(try_path, MAX_PATH, L"%s\\%s", m->roots[i], base);
        if (file_exists_w(try_path)) {
            _snwprintf(out_local, out_cap, L"%s", try_path);
            return 1;
        }
        /* 尝试保留相对子路径尾部 */
        {
            const wchar_t* p = comp;
            while (*p == L'/' || *p == L'\\') p++;
            _snwprintf(try_path, MAX_PATH, L"%s\\%s", m->roots[i], p);
            /* 把 / 换成 \ */
            {
                wchar_t* q;
                for (q = try_path; *q; q++)
                    if (*q == L'/') *q = L'\\';
            }
            if (file_exists_w(try_path)) {
                _snwprintf(out_local, out_cap, L"%s", try_path);
                return 1;
            }
        }
    }
    return 0;
}

typedef struct {
    OS_SrcMap* m;
    OS_SrcMap  edit;
    int       done;   /* 1=关闭 */
    int       ok;
} SrcMapDlg;

static void sm_refresh_list(HWND dlg, OS_SrcMap* m)
{
    HWND lb = GetDlgItem(dlg, IDC_SM_LIST);
    int i;
    SendMessageW(lb, LB_RESETCONTENT, 0, 0);
    for (i = 0; i < m->root_count; i++)
        SendMessageW(lb, LB_ADDSTRING, 0, (LPARAM)m->roots[i]);
}

static INT_PTR CALLBACK srcmap_dlg_proc(HWND dlg, UINT msg, WPARAM wp, LPARAM lp)
{
    SrcMapDlg* st = (SrcMapDlg*)GetWindowLongPtrW(dlg, GWLP_USERDATA);
    switch (msg) {
    case WM_INITDIALOG: {
        st = (SrcMapDlg*)lp;
        SetWindowLongPtrW(dlg, GWLP_USERDATA, (LONG_PTR)st);
        SetWindowTextW(GetDlgItem(dlg, IDC_SM_ELF),
                       st->edit.elf_path[0] ? st->edit.elf_path : L"(未加载 ELF，将保存为默认)");
        sm_refresh_list(dlg, &st->edit);
        if (st->edit.map_count > 0) {
            SetWindowTextW(GetDlgItem(dlg, IDC_SM_FROM), st->edit.from_prefix[0]);
            SetWindowTextW(GetDlgItem(dlg, IDC_SM_TO), st->edit.to_prefix[0]);
        }
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDC_SM_ADD: {
            BROWSEINFOW bi;
            LPITEMIDLIST pidl;
            wchar_t folder[MAX_PATH];
            memset(&bi, 0, sizeof(bi));
            bi.hwndOwner = dlg;
            bi.lpszTitle = L"选择源码根目录";
            bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
            pidl = SHBrowseForFolderW(&bi);
            if (pidl && SHGetPathFromIDListW(pidl, folder)) {
                if (st->edit.root_count < OS_SRCMAP_MAX_ROOTS) {
                    _snwprintf(st->edit.roots[st->edit.root_count], MAX_PATH, L"%s", folder);
                    st->edit.root_count++;
                    sm_refresh_list(dlg, &st->edit);
                } else {
                    MessageBoxW(dlg, L"最多 8 个源码根目录", L"源码目录", MB_OK | MB_ICONWARNING);
                }
                CoTaskMemFree(pidl);
            }
            return TRUE;
        }
        case IDC_SM_DEL: {
            HWND lb = GetDlgItem(dlg, IDC_SM_LIST);
            int sel = (int)SendMessageW(lb, LB_GETCURSEL, 0, 0);
            int i;
            if (sel < 0 || sel >= st->edit.root_count) return TRUE;
            for (i = sel; i < st->edit.root_count - 1; i++)
                memcpy(st->edit.roots[i], st->edit.roots[i + 1], sizeof(st->edit.roots[0]));
            st->edit.root_count--;
            sm_refresh_list(dlg, &st->edit);
            return TRUE;
        }
        case IDC_SM_ADDMAP: {
            wchar_t from[MAX_PATH], to[MAX_PATH];
            GetWindowTextW(GetDlgItem(dlg, IDC_SM_FROM), from, MAX_PATH);
            GetWindowTextW(GetDlgItem(dlg, IDC_SM_TO), to, MAX_PATH);
            if (!from[0] || !to[0]) {
                MessageBoxW(dlg, L"请填写编译路径前缀与本机前缀", L"源码目录", MB_OK | MB_ICONWARNING);
                return TRUE;
            }
            if (st->edit.map_count >= OS_SRCMAP_MAX_MAPS) st->edit.map_count = OS_SRCMAP_MAX_MAPS - 1;
            /* 简化：写入第 0 条（或追加） */
            if (st->edit.map_count == 0) st->edit.map_count = 1;
            _snwprintf(st->edit.from_prefix[0], MAX_PATH, L"%s", from);
            _snwprintf(st->edit.to_prefix[0], MAX_PATH, L"%s", to);
            MessageBoxW(dlg, L"路径前缀映射已更新", L"源码目录", MB_OK | MB_ICONINFORMATION);
            return TRUE;
        }
        case IDC_SM_OK:
            *st->m = st->edit;
            os_srcmap_save(st->m);
            st->ok = 1;
            st->done = 1;
            DestroyWindow(dlg);
            return TRUE;
        case IDC_SM_CANCEL:
        case IDCANCEL:
            st->ok = 0;
            st->done = 1;
            DestroyWindow(dlg);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

/* 无资源模板：运行时创建简易对话框窗口类 */
static const wchar_t* k_sm_cls = L"OSDlgSrcMap";

static LRESULT CALLBACK sm_wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_CREATE) {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        SrcMapDlg* st = (SrcMapDlg*)cs->lpCreateParams;
        HWND c;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)st);
        CreateWindowW(L"STATIC", L"当前 ELF:", WS_CHILD | WS_VISIBLE, 12, 12, 70, 20, hwnd, NULL, cs->hInstance, NULL);
        c = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
                          84, 12, 400, 20, hwnd, (HMENU)IDC_SM_ELF, cs->hInstance, NULL);
        SetWindowTextW(c, st->edit.elf_path[0] ? st->edit.elf_path : L"(未加载 ELF)");
        CreateWindowW(L"STATIC", L"源码根目录（用于按文件名解析 DWARF 路径）:",
                      WS_CHILD | WS_VISIBLE, 12, 40, 480, 20, hwnd, NULL, cs->hInstance, NULL);
        c = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", L"",
                            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
                            12, 64, 380, 140, hwnd, (HMENU)IDC_SM_LIST, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        CreateWindowW(L"BUTTON", L"添加…", WS_CHILD | WS_VISIBLE, 400, 64, 80, 26,
                      hwnd, (HMENU)IDC_SM_ADD, cs->hInstance, NULL);
        CreateWindowW(L"BUTTON", L"删除", WS_CHILD | WS_VISIBLE, 400, 96, 80, 26,
                      hwnd, (HMENU)IDC_SM_DEL, cs->hInstance, NULL);
        CreateWindowW(L"STATIC", L"编译路径前缀 → 本机前缀（可选）:",
                      WS_CHILD | WS_VISIBLE, 12, 216, 400, 20, hwnd, NULL, cs->hInstance, NULL);
        CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                        12, 240, 220, 24, hwnd, (HMENU)IDC_SM_FROM, cs->hInstance, NULL);
        CreateWindowW(L"STATIC", L"→", WS_CHILD | WS_VISIBLE, 238, 242, 20, 20, hwnd, NULL, cs->hInstance, NULL);
        CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                        258, 240, 220, 24, hwnd, (HMENU)IDC_SM_TO, cs->hInstance, NULL);
        CreateWindowW(L"BUTTON", L"应用映射", WS_CHILD | WS_VISIBLE, 400, 238, 80, 26,
                      hwnd, (HMENU)IDC_SM_ADDMAP, cs->hInstance, NULL);
        CreateWindowW(L"BUTTON", L"确定", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                      300, 290, 80, 28, hwnd, (HMENU)IDC_SM_OK, cs->hInstance, NULL);
        CreateWindowW(L"BUTTON", L"取消", WS_CHILD | WS_VISIBLE,
                      390, 290, 80, 28, hwnd, (HMENU)IDC_SM_CANCEL, cs->hInstance, NULL);
        sm_refresh_list(hwnd, &st->edit);
        if (st->edit.map_count > 0) {
            SetWindowTextW(GetDlgItem(hwnd, IDC_SM_FROM), st->edit.from_prefix[0]);
            SetWindowTextW(GetDlgItem(hwnd, IDC_SM_TO), st->edit.to_prefix[0]);
        }
        return 0;
    }
    if (msg == WM_COMMAND) {
        SrcMapDlg* st = (SrcMapDlg*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        if (srcmap_dlg_proc(hwnd, msg, wp, 0)) return 0;
        (void)st;
        return 0;
    }
    if (msg == WM_CLOSE) {
        SrcMapDlg* st = (SrcMapDlg*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        if (st) st->done = 1;
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void os_srcmap_dialog(HWND owner, OS_SrcMap* m)
{
    SrcMapDlg st;
    HWND hwnd;
    MSG msg;
    WNDCLASSW wc;
    static int reg;
    if (!m) return;
    memset(&st, 0, sizeof(st));
    st.m = m;
    st.edit = *m;
    if (!reg) {
        memset(&wc, 0, sizeof(wc));
        wc.lpfnWndProc = sm_wnd_proc;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = k_sm_cls;
        RegisterClassW(&wc);
        reg = 1;
    }
    hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME, k_sm_cls, L"源码目录关联",
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
