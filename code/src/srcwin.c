/*
 * 源码窗口：等宽字体绘制，左侧行号/断点槽，双击槽切换硬件断点。
 */
#include "srcwin.h"
#include "app.h"
#include "mainwin.h"
#include "module_api.h"
#include "util.h"
#include "theme.h"
#include "elf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OS_MAGIC_SRC 0x53524331u /* 'SRC1' */
#define GUTTER_W     56
#define LINE_H       16
#define MAX_SRC_LINES 200000

#define IDM_SRC_BP_ADD    5601
#define IDM_SRC_BP_DEL    5602

typedef struct {
    int      line;     /* 1-based */
    uint32_t addr;
} SrcBp;

typedef struct OS_SrcWin {
    DWORD magic;
    HWND hwnd;
    wchar_t path[MAX_PATH];
    wchar_t title[128];
    wchar_t** lines;
    int line_count;
    int top;           /* 首可见行 0-based */
    int caret_line;    /* 高亮行 0-based，-1=无 */
    int pc_line;       /* 当前 PC 行 0-based，-1=无 */
    SrcBp bps[OS_BP_MAX];
    int bp_count;
    HFONT font;
    int line_h;
} OS_SrcWin;

static const wchar_t* g_src_class = L"OSSrcWin";

static OS_SrcWin* src_from(HWND hwnd)
{
    OS_SrcWin* s = (OS_SrcWin*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (!s || s->magic != OS_MAGIC_SRC) return NULL;
    return s;
}

static void src_free_lines(OS_SrcWin* s)
{
    int i;
    if (!s || !s->lines) return;
    for (i = 0; i < s->line_count; i++) free(s->lines[i]);
    free(s->lines);
    s->lines = NULL;
    s->line_count = 0;
}

static int src_load_file(OS_SrcWin* s, const wchar_t* path)
{
    HANDLE h;
    DWORD sz = 0, rd = 0;
    char* raw = NULL;
    wchar_t* wide = NULL;
    int i, n, cap, start;
    src_free_lines(s);
    if (!path || !path[0]) return -1;
    h = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return -1;
    sz = GetFileSize(h, NULL);
    if (sz == INVALID_FILE_SIZE || sz > 8 * 1024 * 1024) {
        CloseHandle(h);
        return -1;
    }
    raw = (char*)malloc(sz + 1);
    if (!raw) { CloseHandle(h); return -1; }
    if (!ReadFile(h, raw, sz, &rd, NULL)) { free(raw); CloseHandle(h); return -1; }
    CloseHandle(h);
    raw[rd] = 0;

    /* UTF-8 BOM */
    if (rd >= 3 && (unsigned char)raw[0] == 0xEF &&
        (unsigned char)raw[1] == 0xBB && (unsigned char)raw[2] == 0xBF) {
        wide = os_utf8_to_wide(raw + 3);
    } else {
        /* 尝试 UTF-8，失败则按 ACP */
        int need = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, raw, -1, NULL, 0);
        if (need > 0) {
            wide = (wchar_t*)malloc((size_t)need * sizeof(wchar_t));
            if (wide) MultiByteToWideChar(CP_UTF8, 0, raw, -1, wide, need);
        } else {
            need = MultiByteToWideChar(CP_ACP, 0, raw, -1, NULL, 0);
            wide = (wchar_t*)malloc((size_t)need * sizeof(wchar_t));
            if (wide) MultiByteToWideChar(CP_ACP, 0, raw, -1, wide, need);
        }
    }
    free(raw);
    if (!wide) return -1;

    cap = 256;
    s->lines = (wchar_t**)calloc((size_t)cap, sizeof(wchar_t*));
    if (!s->lines) { free(wide); return -1; }
    n = 0;
    start = 0;
    for (i = 0; ; i++) {
        if (wide[i] == L'\n' || wide[i] == 0) {
            int len = i - start;
            wchar_t* line;
            if (len > 0 && wide[start + len - 1] == L'\r') len--;
            line = (wchar_t*)malloc((size_t)(len + 1) * sizeof(wchar_t));
            if (!line) break;
            memcpy(line, wide + start, (size_t)len * sizeof(wchar_t));
            line[len] = 0;
            if (n >= cap) {
                int nc = cap * 2;
                wchar_t** nl;
                if (nc > MAX_SRC_LINES) nc = MAX_SRC_LINES;
                nl = (wchar_t**)realloc(s->lines, (size_t)nc * sizeof(wchar_t*));
                if (!nl) { free(line); break; }
                s->lines = nl;
                cap = nc;
            }
            if (n < MAX_SRC_LINES) s->lines[n++] = line;
            else { free(line); break; }
            if (wide[i] == 0) break;
            start = i + 1;
        }
    }
    free(wide);
    s->line_count = n;
    s->top = 0;
    _snwprintf(s->path, MAX_PATH, L"%s", path);
    {
        const wchar_t* base = wcsrchr(path, L'\\');
        base = base ? base + 1 : path;
        _snwprintf(s->title, 128, L"%s", base);
        SetWindowTextW(s->hwnd, s->title);
    }
    return 0;
}

static int src_has_bp(OS_SrcWin* s, int line1)
{
    int i;
    for (i = 0; i < s->bp_count; i++)
        if (s->bps[i].line == line1) return i;
    return -1;
}

static void src_sync_bps_from_driver(OS_SrcWin* s)
{
    OS_BpInfo items[OS_BP_MAX];
    OS_BpList list;
    char path8[MAX_PATH];
    int i;
    s->bp_count = 0;
    if (!g_app.driver || !g_app.connected || !g_app.driver->command || !g_app.elf)
        return;
    memset(items, 0, sizeof(items));
    memset(&list, 0, sizeof(list));
    list.items = items;
    list.capacity = OS_BP_MAX;
    if (g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_LIST, NULL, &list) != OS_ERR_OK)
        return;
    os_wide_to_utf8_buf(s->path, path8, sizeof(path8));
    for (i = 0; i < list.count && s->bp_count < OS_BP_MAX; i++) {
        char file[512];
        int line = 0;
        if (!os_elf_line_of_addr(g_app.elf, items[i].address, file, sizeof(file), &line))
            continue;
        /* 仅保留属于本文件的断点 */
        {
            const char* b1 = file, *b2, *t;
            for (t = file; *t; t++) if (*t == '/' || *t == '\\') b1 = t + 1;
            b2 = path8;
            for (t = path8; *t; t++) if (*t == '/' || *t == '\\') b2 = t + 1;
            if (_stricmp(b1, b2) != 0) continue;
        }
        s->bps[s->bp_count].line = line;
        s->bps[s->bp_count].addr = items[i].address;
        s->bp_count++;
    }
}

static void src_toggle_bp(OS_SrcWin* s, int line1)
{
    OS_BpReq req;
    uint32_t addr = 0;
    char path8[MAX_PATH];
    int ix = src_has_bp(s, line1);
    int rc;

    if (!g_app.connected || !g_app.driver || !g_app.driver->command) {
        os_msgbox(s->hwnd, L"请先连接仿真器后再设置断点", L"源码断点", MB_OK | MB_ICONINFORMATION);
        return;
    }
    os_wide_to_utf8_buf(s->path, path8, sizeof(path8));

    if (ix >= 0) {
        memset(&req, 0, sizeof(req));
        req.address = s->bps[ix].addr;
        rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_CLEAR, &req, NULL);
        if (rc == OS_ERR_OK) {
            int j;
            for (j = ix; j < s->bp_count - 1; j++) s->bps[j] = s->bps[j + 1];
            s->bp_count--;
            os_log(OS_LOG_INFO, "源码断点清除: %s:%d @ 0x%08X", path8, line1, req.address);
        } else {
            os_msgbox(s->hwnd, L"清除断点失败", L"源码断点", MB_OK | MB_ICONWARNING);
        }
        InvalidateRect(s->hwnd, NULL, FALSE);
        return;
    }

    if (!g_app.elf) {
        os_msgbox(s->hwnd, L"请先加载带调试信息的 ELF", L"源码断点", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (os_elf_line_count(g_app.elf) <= 0) {
        os_msgbox(s->hwnd,
                  L"当前 ELF 没有可用的 .debug_line 行表。\n"
                  L"请用 -g 重新编译（勿用 -g0 / strip），并确认未剥离调试节。",
                  L"源码断点", MB_OK | MB_ICONWARNING);
        return;
    }
    if (!os_elf_addr_of_line(g_app.elf, path8, line1, &addr) || !addr) {
        wchar_t msg[384];
        _snwprintf(msg, 384,
                   L"无法将 %s:%d 映射到指令地址。\n"
                   L"行表共 %d 条、文件 %d 个。\n"
                   L"请确认打开的是编译进该 ELF 的源文件（文件名一致），"
                   L"或在可执行语句行设置断点。",
                   s->title, line1,
                   os_elf_line_count(g_app.elf),
                   os_elf_line_file_count(g_app.elf));
        os_msgbox(s->hwnd, msg, L"源码断点", MB_OK | MB_ICONWARNING);
        os_log(OS_LOG_WARN, "行映射失败: %s:%d (rows=%d files=%d)",
               path8, line1, os_elf_line_count(g_app.elf), os_elf_line_file_count(g_app.elf));
        return;
    }
    memset(&req, 0, sizeof(req));
    req.address = addr;
    _snprintf(req.note, sizeof(req.note), "%s:%d", path8, line1);
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_SET, &req, NULL);
    if (rc != OS_ERR_OK) {
        os_msgbox(s->hwnd, L"设置硬件断点失败（槽满或不支持）", L"源码断点", MB_OK | MB_ICONWARNING);
        return;
    }
    if (s->bp_count < OS_BP_MAX) {
        s->bps[s->bp_count].line = line1;
        s->bps[s->bp_count].addr = addr & ~1u;
        s->bp_count++;
    }
    os_log(OS_LOG_INFO, "源码断点设置: %s:%d @ 0x%08X", path8, line1, addr & ~1u);
    InvalidateRect(s->hwnd, NULL, FALSE);
}

static void src_update_scroll(OS_SrcWin* s)
{
    RECT rc;
    int visible, max_top;
    SCROLLINFO si;
    if (!s) return;
    GetClientRect(s->hwnd, &rc);
    visible = (rc.bottom - rc.top) / (s->line_h > 0 ? s->line_h : LINE_H);
    if (visible < 1) visible = 1;
    max_top = s->line_count - visible;
    if (max_top < 0) max_top = 0;
    if (s->top > max_top) s->top = max_top;
    if (s->top < 0) s->top = 0;
    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = s->line_count > 0 ? s->line_count - 1 : 0;
    si.nPage = (UINT)visible;
    si.nPos = s->top;
    SetScrollInfo(s->hwnd, SB_VERT, &si, TRUE);
}

static void src_paint(OS_SrcWin* s, HDC hdc)
{
    RECT rc, gr, tr;
    int y, i, visible;
    HBRUSH bg = os_theme_brush(TH_BG);
    COLORREF text = os_theme(TH_TEXT);
    COLORREF muted = os_theme(TH_DIMTEXT);
    GetClientRect(s->hwnd, &rc);
    FillRect(hdc, &rc, bg);
    SelectObject(hdc, s->font ? s->font : GetStockObject(ANSI_FIXED_FONT));
    SetBkMode(hdc, TRANSPARENT);
    visible = (rc.bottom - rc.top) / s->line_h + 1;

    /* gutter */
    gr = rc;
    gr.right = GUTTER_W;
    FillRect(hdc, &gr, os_theme_brush(TH_PANEL));

    for (i = 0; i < visible; i++) {
        int li = s->top + i;
        wchar_t num[16];
        if (li < 0 || li >= s->line_count) break;
        y = i * s->line_h;

        if (li == s->pc_line) {
            RECT hr = { GUTTER_W, y, rc.right, y + s->line_h };
            HBRUSH hb = CreateSolidBrush(RGB(90, 70, 20));
            FillRect(hdc, &hr, hb);
            DeleteObject(hb);
        } else if (li == s->caret_line) {
            RECT hr = { GUTTER_W, y, rc.right, y + s->line_h };
            HBRUSH hb = CreateSolidBrush(RGB(40, 60, 90));
            FillRect(hdc, &hr, hb);
            DeleteObject(hb);
        }

        /* BP 红点 */
        if (src_has_bp(s, li + 1) >= 0) {
            HBRUSH rb = CreateSolidBrush(RGB(220, 40, 40));
            HBRUSH old = (HBRUSH)SelectObject(hdc, rb);
            Ellipse(hdc, 6, y + 3, 18, y + 15);
            SelectObject(hdc, old);
            DeleteObject(rb);
        }

        SetTextColor(hdc, muted);
        _snwprintf(num, 16, L"%d", li + 1);
        tr.left = 20; tr.top = y; tr.right = GUTTER_W - 4; tr.bottom = y + s->line_h;
        DrawTextW(hdc, num, -1, &tr, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        SetTextColor(hdc, text);
        tr.left = GUTTER_W + 6; tr.right = rc.right - 4;
        DrawTextW(hdc, s->lines[li] ? s->lines[li] : L"", -1, &tr,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }
}

static LRESULT CALLBACK src_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    OS_SrcWin* s = src_from(hwnd);
    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        s = (OS_SrcWin*)calloc(1, sizeof(OS_SrcWin));
        if (!s) return -1;
        s->magic = OS_MAGIC_SRC;
        s->hwnd = hwnd;
        s->caret_line = -1;
        s->pc_line = -1;
        s->line_h = LINE_H;
        s->font = CreateFontW(15, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              FIXED_PITCH | FF_MODERN, L"Consolas");
        if (cs->lpszName) _snwprintf(s->title, 128, L"%s", cs->lpszName);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)s);
        return 0;
    }
    case WM_DESTROY:
        if (s) {
            src_free_lines(s);
            if (s->font) DeleteObject(s->font);
            s->magic = 0;
            free(s);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        }
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (s) src_paint(s, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_SIZE:
        if (s) src_update_scroll(s);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    case WM_VSCROLL: {
        SCROLLINFO si;
        int pos;
        if (!s) break;
        memset(&si, 0, sizeof(si));
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        pos = si.nPos;
        switch (LOWORD(wp)) {
        case SB_LINEUP: pos--; break;
        case SB_LINEDOWN: pos++; break;
        case SB_PAGEUP: pos -= (int)si.nPage; break;
        case SB_PAGEDOWN: pos += (int)si.nPage; break;
        case SB_THUMBTRACK: pos = si.nTrackPos; break;
        }
        s->top = pos;
        src_update_scroll(s);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_MOUSEWHEEL:
        if (s) {
            int d = GET_WHEEL_DELTA_WPARAM(wp);
            s->top -= (d > 0) ? 3 : -3;
            src_update_scroll(s);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    case WM_LBUTTONDBLCLK: {
        int x = (short)LOWORD(lp);
        int y = (short)HIWORD(lp);
        int li;
        if (!s) break;
        li = s->top + y / s->line_h;
        if (li < 0 || li >= s->line_count) return 0;
        s->caret_line = li;
        if (x < GUTTER_W)
            src_toggle_bp(s, li + 1);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP: {
        int y = (short)HIWORD(lp);
        int li;
        POINT pt;
        HMENU m;
        int has_bp;
        if (!s) break;
        li = s->top + y / s->line_h;
        if (li < 0 || li >= s->line_count) return 0;
        s->caret_line = li;
        InvalidateRect(hwnd, NULL, FALSE);
        if (msg == WM_RBUTTONDOWN) {
            SetFocus(hwnd);
            return 0;
        }
        /* WM_RBUTTONUP：弹出菜单 */
        has_bp = src_has_bp(s, li + 1) >= 0;
        m = CreatePopupMenu();
        if (has_bp)
            AppendMenuW(m, MF_STRING, IDM_SRC_BP_DEL, L"取消断点");
        else
            AppendMenuW(m, MF_STRING, IDM_SRC_BP_ADD, L"添加断点");
        GetCursorPos(&pt);
        {
            int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                     pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(m);
            if (cmd == IDM_SRC_BP_ADD || cmd == IDM_SRC_BP_DEL)
                src_toggle_bp(s, li + 1);
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        int y = (short)HIWORD(lp);
        int li;
        if (!s) break;
        li = s->top + y / s->line_h;
        if (li >= 0 && li < s->line_count) s->caret_line = li;
        SetFocus(hwnd);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_SETFOCUS:
        os_win_mark_active(hwnd);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void os_src_register(void)
{
    WNDCLASSW wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = src_proc;
    wc.hInstance = g_app.hInst ? g_app.hInst : GetModuleHandleW(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = g_src_class;
    wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    RegisterClassW(&wc);
}

HWND os_src_create(HWND parent, int x, int y, int w, int h, const wchar_t* title)
{
    return CreateWindowExW(WS_EX_CLIENTEDGE, g_src_class,
                           title ? title : L"源码",
                           WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS,
                           x, y, w, h, parent, NULL,
                           g_app.hInst ? g_app.hInst : GetModuleHandleW(NULL), NULL);
}

int os_src_is(HWND hwnd)
{
    return src_from(hwnd) != NULL;
}

HWND os_src_open_file(HWND parent_tab, const wchar_t* path, int goto_line)
{
    int i, j;
    HWND h;
    OS_SrcWin* s;
    const wchar_t* base;
    (void)parent_tab;
    if (!path || !path[0]) return NULL;
    /* 已打开：聚焦 */
    for (i = 0; i < g_app.win_count; i++) {
        for (j = 0; j < g_app.wins[i].group_count; j++) {
            HWND w = g_app.wins[i].group[j];
            s = src_from(w);
            if (s && _wcsicmp(s->path, path) == 0) {
                os_mainwin_select_tab(i);
                if (goto_line > 0) {
                    s->caret_line = goto_line - 1;
                    s->top = goto_line - 5;
                    if (s->top < 0) s->top = 0;
                    src_update_scroll(s);
                }
                src_sync_bps_from_driver(s);
                InvalidateRect(w, NULL, FALSE);
                SetFocus(w);
                return w;
            }
        }
    }
    base = wcsrchr(path, L'\\');
    base = base ? base + 1 : path;
    h = os_win_create_by_type("source", base);
    s = src_from(h);
    if (!s) return h;
    if (src_load_file(s, path) != 0) {
        os_msgbox(h, L"无法打开源码文件", L"源码", MB_OK | MB_ICONWARNING);
        return h;
    }
    src_sync_bps_from_driver(s);
    if (goto_line > 0) {
        s->caret_line = goto_line - 1;
        s->top = goto_line - 5;
        if (s->top < 0) s->top = 0;
    }
    src_update_scroll(s);
    InvalidateRect(h, NULL, FALSE);
    return h;
}

void os_src_refresh_bps(void)
{
    int i, j;
    for (i = 0; i < g_app.win_count; i++) {
        for (j = 0; j < g_app.wins[i].group_count; j++) {
            OS_SrcWin* s = src_from(g_app.wins[i].group[j]);
            if (s) {
                src_sync_bps_from_driver(s);
                InvalidateRect(s->hwnd, NULL, FALSE);
            }
        }
    }
}

void os_src_set_pc_line(const wchar_t* path, int line1)
{
    int i, j;
    for (i = 0; i < g_app.win_count; i++) {
        for (j = 0; j < g_app.wins[i].group_count; j++) {
            OS_SrcWin* s = src_from(g_app.wins[i].group[j]);
            if (!s) continue;
            if (line1 > 0 && path && path[0] &&
                _wcsicmp(s->path, path) == 0) {
                s->pc_line = line1 - 1;
                if (s->pc_line < 0) s->pc_line = 0;
                if (s->pc_line >= s->line_count && s->line_count > 0)
                    s->pc_line = s->line_count - 1;
                s->caret_line = s->pc_line;
                s->top = s->pc_line - 5;
                if (s->top < 0) s->top = 0;
                src_update_scroll(s);
            } else {
                s->pc_line = -1;
            }
            InvalidateRect(s->hwnd, NULL, FALSE);
        }
    }
}
