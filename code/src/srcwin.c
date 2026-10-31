/*
 * 源码窗口：等宽字体绘制，左侧行号/断点槽，双击槽切换硬件断点。
 */
#include "dpi.h"
#include "osfont.h"
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
#define GUTTER_W     os_px(56)
#define LINE_H       os_px(16)
#define MAX_SRC_LINES 200000

#define IDM_SRC_BP_ADD    5601
#define IDM_SRC_BP_DEL    5602
#define IDM_SRC_COPY      5603
#define IDM_SRC_SELECTALL 5604

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
    int sel_a;         /* 框选锚点行，-1=无选区 */
    int sel_b;         /* 框选终点行 */
    int selecting;     /* 鼠标拖选中 */
    SrcBp bps[OS_BP_MAX];
    int bp_count;
    HFONT font;
    int line_h;
    int want_top1;     /* 布局恢复的首行（1-based，0=无）：停靠到最终尺寸前不被临时尺寸截断 */
} OS_SrcWin;

static const wchar_t* g_src_class = L"OSSrcWin";

static OS_SrcWin* src_from(HWND hwnd)
{
    OS_SrcWin* s = (OS_SrcWin*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (!s || s->magic != OS_MAGIC_SRC) return NULL;
    return s;
}

/* Bug1: 同一文件的路径可能来自源码树（D:\\a\\b.c）、ELF/IAR 行表（D:\\a/b.c、.\\、大小写不同）
 * 或布局恢复：统一成绝对、反斜杠、规范化路径后再比较；仍不同则按卷序列号+文件索引判同一文件。 */
static void src_norm_path(const wchar_t* in, wchar_t* out, int cap)
{
    wchar_t tmp[MAX_PATH];
    int i;
    DWORD n;
    if (!in) { if (cap) out[0] = 0; return; }
    _snwprintf(tmp, MAX_PATH, L"%s", in);
    tmp[MAX_PATH - 1] = 0;
    for (i = 0; tmp[i]; i++) if (tmp[i] == L'/') tmp[i] = L'\\';
    n = GetFullPathNameW(tmp, (DWORD)cap, out, NULL);
    if (n == 0 || n >= (DWORD)cap) { _snwprintf(out, cap, L"%s", tmp); out[cap - 1] = 0; }
}

static int src_file_id(const wchar_t* p, BY_HANDLE_FILE_INFORMATION* fi)
{
    HANDLE h = CreateFileW(p, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
                           OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    int ok;
    if (h == INVALID_HANDLE_VALUE) return 0;
    ok = GetFileInformationByHandle(h, fi) ? 1 : 0;
    CloseHandle(h);
    return ok;
}

int os_src_same_path(const wchar_t* a, const wchar_t* b)
{
    wchar_t na[MAX_PATH], nb[MAX_PATH];
    BY_HANDLE_FILE_INFORMATION fa, fb;
    if (!a || !b || !a[0] || !b[0]) return 0;
    if (_wcsicmp(a, b) == 0) return 1;
    src_norm_path(a, na, MAX_PATH);
    src_norm_path(b, nb, MAX_PATH);
    if (_wcsicmp(na, nb) == 0) return 1;
    /* 基名不同必不同：避免对每个窗口都打开文件 */
    {
        const wchar_t* ba = wcsrchr(na, L'\\'), *bb = wcsrchr(nb, L'\\');
        ba = ba ? ba + 1 : na; bb = bb ? bb + 1 : nb;
        if (_wcsicmp(ba, bb) != 0) return 0;
    }
    if (src_file_id(na, &fa) && src_file_id(nb, &fb))
        return fa.dwVolumeSerialNumber == fb.dwVolumeSerialNumber &&
               fa.nFileIndexHigh == fb.nFileIndexHigh && fa.nFileIndexLow == fb.nFileIndexLow;
    return 0;
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
    src_norm_path(path, s->path, MAX_PATH);
    {
        const wchar_t* base = wcsrchr(s->path, L'\\');
        base = base ? base + 1 : s->path;
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

/* Bug4 扫查：BP_LIST 是 UI 线程上的同步探针调用（要拿驱动锁）。os_src_refresh_bps/打开窗口会对
 * 每个源码窗口各调一次；改为 300 ms 内共用一份缓存，本地设/清断点后失效。 */
static OS_BpInfo g_bpc_items[OS_BP_MAX];
static int g_bpc_count = -1;
static ULONGLONG g_bpc_tick;
static const void* g_bpc_drv;
static void src_bp_cache_invalidate(void) { g_bpc_count = -1; }

#define SRC_TIMER_BP_RETRY 0x5B01   /* Bug4：BP_LIST 忙时的重试定时器 */

static void src_sync_bps_from_driver(OS_SrcWin* s)
{
    OS_BpInfo* items = g_bpc_items;
    OS_BpList list;
    char path8[MAX_PATH];
    int i, rc;
    if (!g_app.driver || !OS_APP_CONNECTED() || !g_app.driver->command || !g_app.elf) {
        s->bp_count = 0;
        return;
    }
    memset(&list, 0, sizeof(list));
    if (g_bpc_count >= 0 && g_bpc_drv == (const void*)g_app.driver && GetTickCount64() - g_bpc_tick < 300) {
        list.count = g_bpc_count;
    } else {
        memset(g_bpc_items, 0, sizeof(g_bpc_items));
        list.items = g_bpc_items;
        list.capacity = OS_BP_MAX;
        g_bpc_count = -1;
        rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_LIST, NULL, &list);
        if (rc == OS_ERR_AGAIN) {
            /* Bug4（驱动侧）：驱动忙/重连中——保留本窗口上次的断点标记，500ms 后重试 */
            SetTimer(s->hwnd, SRC_TIMER_BP_RETRY, 500, NULL);
            return;
        }
        if (rc != OS_ERR_OK) { s->bp_count = 0; return; }
        if (list.count > OS_BP_MAX) list.count = OS_BP_MAX;
        g_bpc_count = list.count;
        g_bpc_tick = GetTickCount64();
        g_bpc_drv = (const void*)g_app.driver;
    }
    s->bp_count = 0;
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

    if (!OS_APP_CONNECTED() || !g_app.driver || !g_app.driver->command) {
        os_msgbox(s->hwnd, L"请先连接仿真器后再设置断点", L"源码断点", MB_OK | MB_ICONINFORMATION);
        return;
    }
    os_wide_to_utf8_buf(s->path, path8, sizeof(path8));

    if (ix >= 0) {
        memset(&req, 0, sizeof(req));
        req.address = s->bps[ix].addr;
        rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_CLEAR, &req, NULL);
        src_bp_cache_invalidate();
        if (rc == OS_ERR_AGAIN) {   /* Bug4：驱动忙/重连中，不弹框 */
            os_log(OS_LOG_WARN, "源码断点: 驱动忙/重连中，%s:%d 未清除，请稍后重试", path8, line1);
            return;
        }
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
    {
        /* 行表无该行（注释/空行/声明/被优化）时 addr_of_line 会就近取上一可执行行：明确告知，不静默挪位 */
        char f8[MAX_PATH];
        int got = 0;
        if (os_elf_line_of_addr(g_app.elf, addr, f8, sizeof(f8), &got) && got > 0 && got != line1) {
            wchar_t msg[384];
            _snwprintf(msg, 384,
                       L"第 %d 行没有生成代码（注释/空行/声明，或被编译器优化合并）。\n"
                       L"最近的可执行行是第 %d 行（0x%08X），请在有代码的行设置断点。",
                       line1, got, addr & ~1u);
            os_msgbox(s->hwnd, msg, L"源码断点", MB_OK | MB_ICONINFORMATION);
            os_log(OS_LOG_WARN, "源码断点: %s:%d 无代码（最近可执行行 %d @ 0x%08X）", path8, line1, got, addr & ~1u);
            return;
        }
    }
    memset(&req, 0, sizeof(req));
    req.address = addr;
    _snprintf(req.note, sizeof(req.note), "%s:%d", path8, line1);
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_BP_SET, &req, NULL);
    src_bp_cache_invalidate();
    if (rc == OS_ERR_AGAIN) {   /* Bug4：驱动忙/重连中——记日志，不弹框 */
        os_log(OS_LOG_WARN, "源码断点: 驱动忙/重连中，%s:%d 未设置，请稍后重试", path8, line1);
        return;
    }
    if (rc != OS_ERR_OK) {
        const wchar_t* why;
        wchar_t msg[384];
        switch (rc) {
        case OS_ERR_BUSY:          why = L"硬件断点槽已满：请先清除不用的断点（各驱动上限：ST-Link 6、J-Link 视内核、DAS/TC3xx 8）。"; break;
        case OS_ERR_UNSUPPORTED:   why = L"当前驱动不支持硬件断点。"; break;
        case OS_ERR_NOT_CONNECTED: why = L"仿真器未连接。"; break;
        case OS_ERR_INVALID_ARG:   why = L"断点地址无效。"; break;
        default:                   why = L"驱动返回错误（详见下方日志）。"; break;
        }
        _snwprintf(msg, 384, L"设置硬件断点失败：%s\n\n%s:%d @ 0x%08X（驱动 %S，rc=%d）",
                   why, s->title, line1, addr & ~1u, g_app.driver->name, rc);
        os_msgbox(s->hwnd, msg, L"源码断点", MB_OK | MB_ICONWARNING);
        os_log(OS_LOG_WARN, "源码断点设置失败: %s:%d @ 0x%08X rc=%d", path8, line1, addr & ~1u, rc);
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

static void src_sel_range(const OS_SrcWin* s, int* lo, int* hi)
{
    if (!s || s->sel_a < 0 || s->sel_b < 0) { *lo = *hi = -1; return; }
    *lo = s->sel_a < s->sel_b ? s->sel_a : s->sel_b;
    *hi = s->sel_a > s->sel_b ? s->sel_a : s->sel_b;
}

static void src_copy_selection(OS_SrcWin* s)
{
    int lo, hi, i;
    size_t total = 0;
    wchar_t* buf;
    HGLOBAL hg;
    if (!s) return;
    src_sel_range(s, &lo, &hi);
    if (lo < 0) {
        if (s->caret_line >= 0 && s->caret_line < s->line_count) {
            lo = hi = s->caret_line;
        } else return;
    }
    if (lo < 0 || hi >= s->line_count) return;
    for (i = lo; i <= hi; i++)
        total += (s->lines[i] ? wcslen(s->lines[i]) : 0) + 2; /* \r\n */
    buf = (wchar_t*)malloc((total + 1) * sizeof(wchar_t));
    if (!buf) return;
    buf[0] = 0;
    for (i = lo; i <= hi; i++) {
        if (s->lines[i]) wcscat(buf, s->lines[i]);
        if (i < hi) wcscat(buf, L"\r\n");
    }
    if (!OpenClipboard(s->hwnd)) { free(buf); return; }
    EmptyClipboard();
    hg = GlobalAlloc(GMEM_MOVEABLE, (wcslen(buf) + 1) * sizeof(wchar_t));
    if (hg) {
        wchar_t* p = (wchar_t*)GlobalLock(hg);
        if (p) {
            wcscpy(p, buf);
            GlobalUnlock(hg);
            SetClipboardData(CF_UNICODETEXT, hg);
        } else GlobalFree(hg);
    }
    CloseClipboard();
    free(buf);
}

static void src_paint(OS_SrcWin* s, HDC hdc)
{
    RECT rc, gr, tr;
    int y, i, visible, sel_lo, sel_hi;
    HBRUSH bg = os_theme_brush(TH_BG);
    COLORREF text = os_theme(TH_TEXT);
    COLORREF muted = os_theme(TH_DIMTEXT);
    GetClientRect(s->hwnd, &rc);
    FillRect(hdc, &rc, bg);
    SelectObject(hdc, s->font ? s->font : GetStockObject(ANSI_FIXED_FONT));
    SetBkMode(hdc, TRANSPARENT);
    visible = (rc.bottom - rc.top) / s->line_h + 1;
    src_sel_range(s, &sel_lo, &sel_hi);

    /* gutter */
    gr = rc;
    gr.right = GUTTER_W;
    FillRect(hdc, &gr, os_theme_brush(TH_PANEL));

    for (i = 0; i < visible; i++) {
        int li = s->top + i;
        int in_sel;
        wchar_t num[16];
        if (li < 0 || li >= s->line_count) break;
        y = i * s->line_h;
        in_sel = (sel_lo >= 0 && li >= sel_lo && li <= sel_hi);

        if (li == s->pc_line) {
            RECT hr = { GUTTER_W, y, rc.right, y + s->line_h };
            HBRUSH hb = CreateSolidBrush(RGB(90, 70, 20));
            FillRect(hdc, &hr, hb);
            DeleteObject(hb);
        } else if (in_sel) {
            RECT hr = { GUTTER_W, y, rc.right, y + s->line_h };
            FillRect(hdc, &hr, os_theme_brush(TH_TREE_SEL_BG));
        } else if (li == s->caret_line) {
            RECT hr = { GUTTER_W, y, rc.right, y + s->line_h };
            HBRUSH hb = CreateSolidBrush(os_theme_dark() ? RGB(40, 60, 90) : RGB(220, 232, 246));
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

        SetTextColor(hdc, in_sel ? os_theme(TH_TREE_SEL_TEXT) : text);
        tr.left = GUTTER_W + 6; tr.right = rc.right - 4;
        DrawTextW(hdc, s->lines[li] ? s->lines[li] : L"", -1, &tr,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }
}

static LRESULT CALLBACK src_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    OS_SrcWin* s = src_from(hwnd);
    switch (msg) {
    case WM_TIMER:
        if (wp == SRC_TIMER_BP_RETRY) {   /* Bug4 */
            OS_SrcWin* st = src_from(hwnd);
            KillTimer(hwnd, SRC_TIMER_BP_RETRY);
            if (st) { src_sync_bps_from_driver(st); InvalidateRect(hwnd, NULL, FALSE); }
            return 0;
        }
        break;
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        s = (OS_SrcWin*)calloc(1, sizeof(OS_SrcWin));
        if (!s) return -1;
        s->magic = OS_MAGIC_SRC;
        s->hwnd = hwnd;
        s->caret_line = -1;
        s->pc_line = -1;
        s->sel_a = s->sel_b = -1;
        s->selecting = 0;
        s->line_h = LINE_H;
        s->font = os_font_create(os_font_mono_face(), 13, FW_NORMAL); /* UI P0 */
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
        if (s && s->want_top1 > 0) {
            s->top = s->want_top1 - 1;   /* 保持到用户滚动/跳转为止：停靠过程中的临时尺寸不截断恢复的首行 */
        }
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
        s->want_top1 = 0;
        src_update_scroll(s);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_MOUSEWHEEL:
        if (s) {
            int d = GET_WHEEL_DELTA_WPARAM(wp);
            s->top -= (d > 0) ? 3 : -3;
            s->want_top1 = 0;
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
        int has_bp, sel_lo, sel_hi;
        if (!s) break;
        li = s->top + y / s->line_h;
        if (li < 0 || li >= s->line_count) return 0;
        s->caret_line = li;
        InvalidateRect(hwnd, NULL, FALSE);
        if (msg == WM_RBUTTONDOWN) {
            SetFocus(hwnd);
            return 0;
        }
        has_bp = src_has_bp(s, li + 1) >= 0;
        src_sel_range(s, &sel_lo, &sel_hi);
        m = CreatePopupMenu();
        os_theme_menu_show(m);
        AppendMenuW(m, MF_STRING | (sel_lo < 0 && s->caret_line < 0 ? MF_GRAYED : 0),
                    IDM_SRC_COPY, L"复制\tCtrl+C");
        AppendMenuW(m, MF_STRING, IDM_SRC_SELECTALL, L"全选\tCtrl+A");
        AppendMenuW(m, MF_SEPARATOR, 0, NULL);
        if (has_bp)
            AppendMenuW(m, MF_STRING, IDM_SRC_BP_DEL, L"取消断点");
        else
            AppendMenuW(m, MF_STRING, IDM_SRC_BP_ADD, L"添加断点");
        GetCursorPos(&pt);
        {
            int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                     pt.x, pt.y, 0, hwnd, NULL);
            os_theme_menu_end();
            DestroyMenu(m);
            if (cmd == IDM_SRC_BP_ADD || cmd == IDM_SRC_BP_DEL)
                src_toggle_bp(s, li + 1);
            else if (cmd == IDM_SRC_COPY)
                src_copy_selection(s);
            else if (cmd == IDM_SRC_SELECTALL && s->line_count > 0) {
                s->sel_a = 0;
                s->sel_b = s->line_count - 1;
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        int x = (short)LOWORD(lp);
        int y = (short)HIWORD(lp);
        int li;
        if (!s) break;
        li = s->top + y / s->line_h;
        if (li < 0) li = 0;
        if (li >= s->line_count) li = s->line_count - 1;
        if (li >= 0 && li < s->line_count) {
            s->caret_line = li;
            if (x >= GUTTER_W) {
                if (!(wp & MK_SHIFT) || s->sel_a < 0)
                    s->sel_a = li;
                s->sel_b = li;
                s->selecting = 1;
                SetCapture(hwnd);
            } else {
                s->sel_a = s->sel_b = -1;
            }
        }
        SetFocus(hwnd);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    case WM_MOUSEMOVE:
        if (s && s->selecting && (wp & MK_LBUTTON)) {
            int y = (short)HIWORD(lp);
            int li = s->top + y / s->line_h;
            if (li < 0) li = 0;
            if (li >= s->line_count) li = s->line_count - 1;
            if (li != s->sel_b) {
                s->sel_b = li;
                s->caret_line = li;
                s->want_top1 = 0;
                /* 拖出可视区时滚动 */
                if (y < 0) { s->top--; src_update_scroll(s); }
                else {
                    RECT rc; GetClientRect(hwnd, &rc);
                    if (y > rc.bottom) { s->top++; src_update_scroll(s); }
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        break;
    case WM_LBUTTONUP:
        if (s && s->selecting) {
            s->selecting = 0;
            ReleaseCapture();
            return 0;
        }
        break;
    case WM_KEYDOWN:
        if (!s) break;
        if ((GetKeyState(VK_CONTROL) & 0x8000) && wp == 'C') {
            src_copy_selection(s);
            return 0;
        }
        if ((GetKeyState(VK_CONTROL) & 0x8000) && wp == 'A') {
            if (s->line_count > 0) {
                s->sel_a = 0;
                s->sel_b = s->line_count - 1;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        if (wp == VK_ESCAPE) {
            s->sel_a = s->sel_b = -1;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (wp == VK_UP || wp == VK_DOWN || wp == VK_PRIOR || wp == VK_NEXT ||
            wp == VK_HOME || wp == VK_END) {
            int li = s->caret_line < 0 ? s->top : s->caret_line;
            RECT rc; int page;
            s->want_top1 = 0;
            GetClientRect(hwnd, &rc);
            page = (rc.bottom - rc.top) / s->line_h;
            if (page < 1) page = 1;
            if (wp == VK_UP) li--;
            else if (wp == VK_DOWN) li++;
            else if (wp == VK_PRIOR) li -= page;
            else if (wp == VK_NEXT) li += page;
            else if (wp == VK_HOME) li = 0;
            else if (wp == VK_END) li = s->line_count - 1;
            if (li < 0) li = 0;
            if (li >= s->line_count) li = s->line_count > 0 ? s->line_count - 1 : 0;
            s->caret_line = li;
            if (GetKeyState(VK_SHIFT) & 0x8000) {
                if (s->sel_a < 0) s->sel_a = li;
                s->sel_b = li;
            } else {
                s->sel_a = s->sel_b = -1;
            }
            if (li < s->top) s->top = li;
            if (li >= s->top + page) s->top = li - page + 1;
            src_update_scroll(s);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        break;
    case WM_COPY:
        if (s) src_copy_selection(s);
        return 0;
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
            if (s && os_src_same_path(s->path, path)) {
                os_mainwin_select_tab(i);
                if (goto_line > 0) {
                    s->caret_line = goto_line - 1;
                    s->top = goto_line - 5; s->want_top1 = 0;
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
    {
        const wchar_t* fs = wcsrchr(path, L'/');
        if (fs && (!base || fs > base)) base = fs;
    }
    base = base ? base + 1 : path;
    /* 停靠默认布局里预留的空源码窗口：直接复用（保持其停靠位置） */
    h = NULL;
    for (i = 0; i < g_app.win_count && !h; i++)
        for (j = 0; j < g_app.wins[i].group_count; j++) {
            OS_SrcWin* e = src_from(g_app.wins[i].group[j]);
            if (e && !e->path[0]) { h = e->hwnd; os_mainwin_set_win_title(h, base); os_mainwin_select_tab(i); break; }
        }
    if (!h) h = os_win_create_by_type("source", base);
    s = src_from(h);
    if (!s) return h;
    if (src_load_file(s, path) != 0) {
        os_msgbox(h, L"无法打开源码文件", L"源码", MB_OK | MB_ICONWARNING);
        return h;
    }
    src_sync_bps_from_driver(s);
    if (goto_line > 0) {
        s->caret_line = goto_line - 1;
        s->top = goto_line - 5; s->want_top1 = 0;
        if (s->top < 0) s->top = 0;
    }
    src_update_scroll(s);
    InvalidateRect(h, NULL, FALSE);
    return h;
}

void os_src_refresh_bps(void)
{
    int i, j;
    src_bp_cache_invalidate();   /* 外部（断点窗/AI）改了断点：重新取一次，之后各窗口共用 */
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
                os_src_same_path(s->path, path)) {
                s->pc_line = line1 - 1;
                if (s->pc_line < 0) s->pc_line = 0;
                if (s->pc_line >= s->line_count && s->line_count > 0)
                    s->pc_line = s->line_count - 1;
                s->caret_line = s->pc_line;
                s->top = s->pc_line - 5; s->want_top1 = 0;
                if (s->top < 0) s->top = 0;
                src_update_scroll(s);
            } else {
                s->pc_line = -1;
            }
            InvalidateRect(s->hwnd, NULL, FALSE);
        }
    }
}

int os_src_get_state(HWND hwnd, wchar_t* path, int cap, int* top1)
{
    OS_SrcWin* s = src_from(hwnd);
    if (!s) return -1;
    if (path && cap > 0) _snwprintf(path, cap - 1, L"%s", s->path), path[cap - 1] = 0;
    if (top1) *top1 = s->want_top1 > 0 ? s->want_top1 : s->top + 1;
    return 0;
}

int os_src_restore(HWND hwnd, const wchar_t* path, int top1)
{
    OS_SrcWin* s = src_from(hwnd);
    if (!s || !path || !path[0]) return -1;
    if (src_load_file(s, path) != 0) return -1;
    src_sync_bps_from_driver(s);
    s->top = top1 > 0 ? top1 - 1 : 0;
    s->want_top1 = top1 > 0 ? top1 : 0;
    src_update_scroll(s);
    InvalidateRect(hwnd, NULL, FALSE);
    return 0;
}

/* 测试/自动化：各源码窗口状态（路径、首行、PC 行、断点数、是否可见/父窗口类别） */
int os_src_describe_json(char* out, int cap)
{
    int i, j, len = 0, first = 1;
    len += _snprintf(out + len, cap - len, "{\"ok\":true,\"src\":[");
    for (i = 0; i < g_app.win_count; i++)
        for (j = 0; j < g_app.wins[i].group_count; j++) {
            OS_SrcWin* s = src_from(g_app.wins[i].group[j]);
            char p8[MAX_PATH * 3], esc[MAX_PATH * 6];
            int k, e = 0;
            HWND top;
            if (!s || len >= cap - 600) continue;
            WideCharToMultiByte(CP_UTF8, 0, s->path, -1, p8, sizeof(p8), NULL, NULL);
            for (k = 0; p8[k] && e < (int)sizeof(esc) - 2; k++) {
                if (p8[k] == '\\' || p8[k] == '"') esc[e++] = '\\';
                esc[e++] = p8[k];
            }
            esc[e] = 0;
            top = GetAncestor(s->hwnd, GA_ROOT);
            len += _snprintf(out + len, cap - len,
                "%s{\"hwnd\":%lld,\"path\":\"%s\",\"top\":%d,\"lines\":%d,\"pc_line\":%d,\"bps\":%d,"
                "\"visible\":%d,\"floating\":%d}",
                first ? "" : ",", (long long)(INT_PTR)s->hwnd, esc, s->top + 1, s->line_count,
                s->pc_line >= 0 ? s->pc_line + 1 : 0, s->bp_count,
                IsWindowVisible(s->hwnd) ? 1 : 0, top != g_app.hMain ? 1 : 0);
            first = 0;
        }
    len += _snprintf(out + len, cap - len, "]}");
    return len;
}

/* UI P1b: F9 toggles a breakpoint on the caret line of a source window */
int os_src_toggle_bp_caret(HWND hwnd)
{
    OS_SrcWin* s = src_from(hwnd);
    if (!s || s->caret_line < 0 || s->caret_line >= s->line_count) return 0;
    src_toggle_bp(s, s->caret_line + 1);
    InvalidateRect(hwnd, NULL, FALSE);
    return 1;
}
