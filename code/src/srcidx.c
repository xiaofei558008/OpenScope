/*
 * 源码目录索引与 Ctrl+P 快速打开。
 */
#include "srcidx.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <commctrl.h>

static int is_src_ext(const wchar_t* name)
{
    const wchar_t* dot = wcsrchr(name, L'.');
    if (!dot || !dot[1]) return 0;
    return (_wcsicmp(dot, L".c") == 0 || _wcsicmp(dot, L".h") == 0 ||
            _wcsicmp(dot, L".cpp") == 0 || _wcsicmp(dot, L".hpp") == 0 ||
            _wcsicmp(dot, L".cc") == 0 || _wcsicmp(dot, L".cxx") == 0 ||
            _wcsicmp(dot, L".s") == 0 || _wcsicmp(dot, L".S") == 0 ||
            _wcsicmp(dot, L".asm") == 0 || _wcsicmp(dot, L".inc") == 0);
}

void os_srcidx_clear(OS_SrcIdx* idx)
{
    if (!idx) return;
    idx->count = 0;
}

static void scan_dir(OS_SrcIdx* idx, const wchar_t* dir, int depth)
{
    wchar_t pattern[MAX_PATH], full[MAX_PATH];
    WIN32_FIND_DATAW fd;
    HANDLE h;
    if (!idx || !dir || !dir[0] || depth > 24) return;
    if (idx->count >= OS_SRCIDX_MAX) return;
    _snwprintf(pattern, MAX_PATH, L"%s\\*", dir);
    h = FindFirstFileW(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.cFileName[0] == L'.' &&
            (fd.cFileName[1] == 0 || (fd.cFileName[1] == L'.' && fd.cFileName[2] == 0)))
            continue;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (_wcsicmp(fd.cFileName, L".git") == 0 ||
                _wcsicmp(fd.cFileName, L"Debug") == 0 ||
                _wcsicmp(fd.cFileName, L"Release") == 0)
                continue;
            _snwprintf(full, MAX_PATH, L"%s\\%s", dir, fd.cFileName);
            scan_dir(idx, full, depth + 1);
        } else if (is_src_ext(fd.cFileName)) {
            if (idx->count < OS_SRCIDX_MAX) {
                _snwprintf(idx->paths[idx->count], MAX_PATH, L"%s\\%s", dir, fd.cFileName);
                idx->count++;
            }
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

void os_srcidx_rebuild(OS_SrcIdx* idx, const OS_SrcMap* m)
{
    int i;
    if (!idx) return;
    os_srcidx_clear(idx);
    if (!m) return;
    for (i = 0; i < m->root_count; i++)
        scan_dir(idx, m->roots[i], 0);
    os_log(OS_LOG_INFO, "源码索引: %d 个文件（%d 个根目录）", idx->count, m->root_count);
}

int os_srcidx_fuzzy(const OS_SrcIdx* idx, const wchar_t* needle, int* out, int max_out)
{
    int i, n = 0;
    if (!idx || !out || max_out <= 0) return 0;
    for (i = 0; i < idx->count && n < max_out; i++) {
        const wchar_t* hay = idx->paths[i];
        const wchar_t* h = hay;
        const wchar_t* nd = needle ? needle : L"";
        int ok = 1;
        if (nd[0]) {
            while (*nd) {
                wchar_t want = *nd++;
                int found = 0;
                if (want >= L'A' && want <= L'Z') want += 32;
                if (want == L'/') want = L'\\';
                while (*h) {
                    wchar_t c = *h++;
                    if (c >= L'A' && c <= L'Z') c += 32;
                    if (c == L'/') c = L'\\';
                    if (c == want) { found = 1; break; }
                }
                if (!found) { ok = 0; break; }
            }
        }
        if (ok) out[n++] = i;
    }
    return n;
}

void os_srcidx_fill_tree(HWND tree, const OS_SrcIdx* idx)
{
    int i;
    TVINSERTSTRUCTW is;
    HTREEITEM root;
    if (!tree) return;
    TreeView_DeleteAllItems(tree);
    if (!idx || idx->count <= 0) {
        memset(&is, 0, sizeof(is));
        is.hParent = TVI_ROOT;
        is.hInsertAfter = TVI_LAST;
        is.item.mask = TVIF_TEXT;
        is.item.pszText = L"（请先配置源码目录关联）";
        TreeView_InsertItem(tree, &is);
        return;
    }
    memset(&is, 0, sizeof(is));
    is.hParent = TVI_ROOT;
    is.hInsertAfter = TVI_LAST;
    is.item.mask = TVIF_TEXT;
    is.item.pszText = L"源码文件";
    root = TreeView_InsertItem(tree, &is);
    for (i = 0; i < idx->count; i++) {
        const wchar_t* path = idx->paths[i];
        const wchar_t* base = wcsrchr(path, L'\\');
        base = base ? base + 1 : path;
        memset(&is, 0, sizeof(is));
        is.hParent = root;
        is.hInsertAfter = TVI_LAST;
        is.item.mask = TVIF_TEXT | TVIF_PARAM;
        is.item.pszText = (LPWSTR)base;
        is.item.lParam = (LPARAM)(i + 1); /* 1-based index into idx->paths */
        TreeView_InsertItem(tree, &is);
    }
    TreeView_Expand(tree, root, TVE_EXPAND);
}

/* ---- Ctrl+P 对话框 ---- */
#define IDC_QP_EDIT 5201
#define IDC_QP_LIST 5202

typedef struct {
    const OS_SrcIdx* idx;
    OS_SrcOpenFn on_open;
    void* user;
    int done;
    int ok;
    int matches[256];
    int match_n;
} QpDlg;

static void qp_filter(HWND hwnd, QpDlg* st)
{
    wchar_t needle[128];
    HWND lb = GetDlgItem(hwnd, IDC_QP_LIST);
    int i;
    GetWindowTextW(GetDlgItem(hwnd, IDC_QP_EDIT), needle, 128);
    st->match_n = os_srcidx_fuzzy(st->idx, needle, st->matches,
                                  (int)(sizeof(st->matches) / sizeof(st->matches[0])));
    SendMessageW(lb, LB_RESETCONTENT, 0, 0);
    for (i = 0; i < st->match_n; i++) {
        int ix = st->matches[i];
        SendMessageW(lb, LB_ADDSTRING, 0, (LPARAM)st->idx->paths[ix]);
    }
    if (st->match_n > 0) SendMessageW(lb, LB_SETCURSEL, 0, 0);
}

static void qp_accept(HWND hwnd, QpDlg* st)
{
    HWND lb = GetDlgItem(hwnd, IDC_QP_LIST);
    int sel = (int)SendMessageW(lb, LB_GETCURSEL, 0, 0);
    int ix;
    if (sel < 0 || sel >= st->match_n) return;
    ix = st->matches[sel];
    if (st->on_open) st->on_open(st->idx->paths[ix], st->user);
    st->ok = 1;
    st->done = 1;
    DestroyWindow(hwnd);
}

static LRESULT CALLBACK qp_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    QpDlg* st = (QpDlg*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        HWND c;
        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        st = (QpDlg*)cs->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)st);
        c = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                            12, 12, 480, 24, hwnd, (HMENU)IDC_QP_EDIT, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);
        c = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", L"",
                            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | WS_TABSTOP,
                            12, 44, 480, 280, hwnd, (HMENU)IDC_QP_LIST, cs->hInstance, NULL);
        SendMessageW(c, WM_SETFONT, (WPARAM)font, TRUE);
        SetFocus(GetDlgItem(hwnd, IDC_QP_EDIT));
        qp_filter(hwnd, st);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == IDC_QP_EDIT && HIWORD(wp) == EN_CHANGE) {
            qp_filter(hwnd, st);
            return 0;
        }
        if (LOWORD(wp) == IDC_QP_LIST && HIWORD(wp) == LBN_DBLCLK) {
            qp_accept(hwnd, st);
            return 0;
        }
        if (LOWORD(wp) == IDOK) { qp_accept(hwnd, st); return 0; }
        if (LOWORD(wp) == IDCANCEL) {
            st->done = 1;
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    case WM_CLOSE:
        st->done = 1;
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

int os_srcidx_quick_open(HWND owner, const OS_SrcIdx* idx, OS_SrcOpenFn on_open, void* user)
{
    static int reg;
    QpDlg st;
    HWND hwnd;
    MSG msg;
    WNDCLASSW wc;
    if (!idx) return 0;
    memset(&st, 0, sizeof(st));
    st.idx = idx;
    st.on_open = on_open;
    st.user = user;
    if (!reg) {
        memset(&wc, 0, sizeof(wc));
        wc.lpfnWndProc = qp_proc;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = L"OSDlgQuickOpen";
        RegisterClassW(&wc);
        reg = 1;
    }
    hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, L"OSDlgQuickOpen",
                           L"快速打开源码（Ctrl+P）",
                           WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                           (GetSystemMetrics(SM_CXSCREEN) - 520) / 2,
                           (GetSystemMetrics(SM_CYSCREEN) - 380) / 2,
                           520, 380, owner, NULL, GetModuleHandleW(NULL), &st);
    if (!hwnd) return 0;
    EnableWindow(owner, FALSE);
    while (!st.done && IsWindow(hwnd) && GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN) {
            qp_accept(hwnd, &st);
            continue;
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            st.done = 1;
            DestroyWindow(hwnd);
            continue;
        }
        if (!IsDialogMessageW(hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    EnableWindow(owner, TRUE);
    SetForegroundWindow(owner);
    return st.ok;
}
