/**
 * XCP 标定页：手工连接串口/TCP/CAN，按地址或符号做 SHORT_UPLOAD / DOWNLOAD。
 *
 * 符号来源（两种都支持，可直接在本页加载）：
 *   - ELF/out：调用主框架加载，符号带 DWARF 类型（可读写十进制/浮点值）
 *   - .map  ：GNU ld / Keil armlink / IAR 映射文件，仅地址（默认 4 字节无符号）
 * 符号统一由 xcpsym 注册表管理；AI 接口通过同一注册表做 xcp.read_sym / write_sym。
 */
#include "xcpwin.h"
#include "app.h"
#include "mainwin.h"
#include "module_api.h"
#include "util.h"
#include "xcpsym.h"

#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IDC_XW_TRANSPORT 5201
#define IDC_XW_PORT      5202
#define IDC_XW_BAUD      5203
#define IDC_XW_SCAN      5204
#define IDC_XW_CONNECT   5205
#define IDC_XW_DISCON    5206
#define IDC_XW_STATUS    5207
#define IDC_XW_SYMLIST   5208
#define IDC_XW_FILTER    5209
#define IDC_XW_ADDR      5210
#define IDC_XW_SIZE      5211
#define IDC_XW_HEX       5212
#define IDC_XW_READ      5213
#define IDC_XW_WRITE     5214
#define IDC_XW_PICK      5215
#define IDC_XW_REFRESH   5216
#define IDC_XW_CLOSE     5217
#define IDC_XW_LOADELF   5218
#define IDC_XW_LOADMAP   5219
#define IDC_XW_MAPINFO   5220
#define IDC_XW_VALUE     5221
#define IDC_XW_WRITEVAL  5222
#define IDC_XW_READVAL   5223
#define IDC_XW_TYPEINFO  5224

#define XW_SYM_LIMIT     5000

static HWND g_xcpwin;
static OS_XcpSym* g_sel;   /* 当前选中符号（静态保存，避免每帧查询） */
static int g_sel_valid;

static void xw_set_status(HWND hwnd, const wchar_t* s)
{
    HWND h = GetDlgItem(hwnd, IDC_XW_STATUS);
    if (h) SetWindowTextW(h, s ? s : L"");
}

static void xw_set_typeinfo(HWND hwnd, const wchar_t* s)
{
    HWND h = GetDlgItem(hwnd, IDC_XW_TYPEINFO);
    if (h) SetWindowTextW(h, s ? s : L"");
}

static const char* xw_transport(HWND hwnd)
{
    LRESULT i = SendMessageW(GetDlgItem(hwnd, IDC_XW_TRANSPORT), CB_GETCURSEL, 0, 0);
    if (i == 1) return "can";
    if (i == 2) return "tcp";
    return "serial";
}

static void xw_update_mapinfo(HWND hwnd)
{
    wchar_t w[512], wp[MAX_PATH];
    char p8[MAX_PATH];
    HWND h = GetDlgItem(hwnd, IDC_XW_MAPINFO);
    if (!h) return;
    os_xcpsym_map_path(p8, sizeof(p8));
    if (!p8[0]) {
        SetWindowTextW(h, L"MAP: 未加载");
        return;
    }
    os_utf8_to_wide_buf(p8, wp, MAX_PATH);
    {
        wchar_t* slash = wcsrchr(wp, L'\\');
        _snwprintf(w, 512, L"MAP: %s", slash ? slash + 1 : wp);
    }
    SetWindowTextW(h, w);
}

static void xw_fill_symbols(HWND hwnd)
{
    HWND list = GetDlgItem(hwnd, IDC_XW_SYMLIST);
    wchar_t filt[128];
    char f8[128];
    OS_XcpSym* syms;
    int total, n, i;
    if (!list) return;
    SendMessageW(list, LB_RESETCONTENT, 0, 0);
    GetWindowTextW(GetDlgItem(hwnd, IDC_XW_FILTER), filt, 128);
    os_wide_to_utf8_buf(filt, f8, sizeof(f8));
    total = os_xcpsym_count();
    if (total <= 0) {
        xw_update_mapinfo(hwnd);
        return;
    }
    syms = (OS_XcpSym*)malloc((size_t)total * sizeof(OS_XcpSym));
    if (!syms) return;
    n = os_xcpsym_search(f8, 0, total, syms, total);
    if (n > XW_SYM_LIMIT) n = XW_SYM_LIMIT;
    SendMessageW(list, LB_INITSTORAGE, (WPARAM)n, (LPARAM)(n * 48));
    for (i = 0; i < n; i++) {
        wchar_t line[512];
        char buf[512];
        const char* src = syms[i].from_map ? "MAP" : "ELF";
        _snprintf(buf, sizeof(buf), "%s  @0x%08llX  %uB  [%s]",
                  syms[i].name, (unsigned long long)syms[i].address,
                  (unsigned)syms[i].size, src);
        os_utf8_to_wide_buf(buf, line, 512);
        SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)line);
        SendMessageW(list, LB_SETITEMDATA,
                     (WPARAM)SendMessageW(list, LB_GETCOUNT, 0, 0) - 1, (LPARAM)i);
    }
    free(syms);
    xw_update_mapinfo(hwnd);
}

/* 从注册表读取当前列表项对应符号（按过滤后的序号需要重新查询） */
static int xw_sym_from_item(HWND hwnd, int item_index, OS_XcpSym* out)
{
    HWND list = GetDlgItem(hwnd, IDC_XW_SYMLIST);
    wchar_t filt[128];
    char f8[128];
    OS_XcpSym* syms;
    int total, n, idx, rc = -1;
    if (!list || !out || item_index < 0) return -1;
    GetWindowTextW(GetDlgItem(hwnd, IDC_XW_FILTER), filt, 128);
    os_wide_to_utf8_buf(filt, f8, sizeof(f8));
    total = os_xcpsym_count();
    if (total <= 0) return -1;
    syms = (OS_XcpSym*)malloc((size_t)total * sizeof(OS_XcpSym));
    if (!syms) return -1;
    n = os_xcpsym_search(f8, 0, total, syms, total);
    if (n > XW_SYM_LIMIT) n = XW_SYM_LIMIT;
    if (item_index < n) {
        *out = syms[item_index];
        rc = 0;
    }
    free(syms);
    (void)idx;
    return rc;
}

static const char* xw_type_name(const OS_XcpSym* s)
{
    if (s->is_float) return s->size == 8 ? "FLOAT64" : "FLOAT32";
    if (s->is_bitfield) return "BITFIELD";
    switch (s->size) {
    case 1: return s->is_signed ? "SBYTE" : "UBYTE";
    case 2: return s->is_signed ? "SWORD" : "UWORD";
    case 4: return s->is_signed ? "SLONG" : "ULONG";
    case 8: return s->is_signed ? "INT64" : "UINT64";
    default: return "BYTES";
    }
}

static void xw_apply_sym(HWND hwnd, int item_index)
{
    OS_XcpSym s;
    wchar_t wa[32], ws[16], wt[256], wv[256];
    if (xw_sym_from_item(hwnd, item_index, &s) != 0) return;
    if (s.size > 64) s.size = 64;
    _snwprintf(wa, 32, L"0x%08llX", (unsigned long long)s.address);
    _snwprintf(ws, 16, L"%u", (unsigned)s.size);
    SetWindowTextW(GetDlgItem(hwnd, IDC_XW_ADDR), wa);
    SetWindowTextW(GetDlgItem(hwnd, IDC_XW_SIZE), ws);
    if (!g_sel) g_sel = (OS_XcpSym*)malloc(sizeof(OS_XcpSym));
    if (g_sel) { *g_sel = s; g_sel_valid = 1; }
    {
        char info[256];
        _snprintf(info, sizeof(info), "%s  %s  %uB%s",
                  s.name, xw_type_name(&s), (unsigned)s.size,
                  s.from_map ? "（MAP，无类型信息）" : "");
        os_utf8_to_wide_buf(info, wt, 256);
        xw_set_typeinfo(hwnd, wt);
    }
    SetWindowTextW(GetDlgItem(hwnd, IDC_XW_VALUE), L"");
    (void)wv;
}

static int xw_parse_u32(HWND hwnd, int idc, uint32_t* out)
{
    wchar_t w[64];
    char a[64];
    GetWindowTextW(GetDlgItem(hwnd, idc), w, 64);
    os_wide_to_utf8_buf(w, a, sizeof(a));
    if (!a[0]) return -1;
    *out = (uint32_t)strtoul(a, NULL, 0);
    return 0;
}

static int xw_read_bytes(uint32_t addr, uint32_t size, uint8_t* buf)
{
    OS_MemReq req;
    int rc;
    if (!g_app.xcpmod || !g_app.xcpmod->command || !g_app.xcp_connected)
        return OS_ERR_NOT_CONNECTED;
    memset(&req, 0, sizeof(req));
    req.address = addr;
    req.size = size;
    req.data = buf;
    rc = g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_READ_MEM, &req, NULL);
    return rc < 0 ? rc : (int)req.size;
}

static int xw_write_bytes(uint32_t addr, const uint8_t* buf, uint32_t size)
{
    OS_MemReq req;
    if (!g_app.xcpmod || !g_app.xcpmod->command || !g_app.xcp_connected)
        return OS_ERR_NOT_CONNECTED;
    memset(&req, 0, sizeof(req));
    req.address = addr;
    req.size = size;
    req.data = (void*)buf;
    return g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_WRITE_MEM, &req, NULL);
}

static void xw_do_connect(HWND hwnd)
{
    OS_ConnectCfg cfg;
    wchar_t wp[64], wb[32];
    char port8[64], baud8[32];
    int rc, c = 0;
    if (!g_app.xcpmod || !g_app.xcpmod->command) {
        xw_set_status(hwnd, L"未加载 xcp.dll");
        return;
    }
    memset(&cfg, 0, sizeof(cfg));
    cfg.probe_index = 0;
    _snprintf(cfg.device, sizeof(cfg.device), "%s", xw_transport(hwnd));
    GetWindowTextW(GetDlgItem(hwnd, IDC_XW_PORT), wp, 64);
    GetWindowTextW(GetDlgItem(hwnd, IDC_XW_BAUD), wb, 32);
    os_wide_to_utf8_buf(wp, port8, sizeof(port8));
    os_wide_to_utf8_buf(wb, baud8, sizeof(baud8));
    _snprintf(cfg.serial, sizeof(cfg.serial), "%s", port8);
    cfg.speed_khz = atoi(baud8);
    if (cfg.speed_khz <= 0) cfg.speed_khz = 115200;
    _snprintf(g_app.xcp_transport, sizeof(g_app.xcp_transport), "%s", cfg.device);
    _snprintf(g_app.xcp_port, sizeof(g_app.xcp_port), "%s", port8);
    g_app.xcp_baud = cfg.speed_khz;
    rc = g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_CONNECT, &cfg, NULL);
    if (rc != OS_ERR_OK) {
        g_app.xcp_connected = 0;
        xw_set_status(hwnd, L"连接失败");
        return;
    }
    g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_IS_CONNECTED, NULL, &c);
    g_app.xcp_connected = c;
    xw_set_status(hwnd, c ? L"已连接" : L"未连接");
    os_mainwin_request_ui_sync();
}

static void xw_do_disconnect(HWND hwnd)
{
    if (g_app.xcpmod && g_app.xcpmod->command)
        g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_DISCONNECT, NULL, NULL);
    g_app.xcp_connected = 0;
    xw_set_status(hwnd, L"已断开");
    os_mainwin_request_ui_sync();
}

static void xw_do_scan(HWND hwnd)
{
    OS_ScanReq req;
    OS_DeviceInfo items[32];
    if (!g_app.xcpmod || !g_app.xcpmod->command) return;
    memset(&req, 0, sizeof(req));
    req.items = items;
    req.capacity = 32;
    g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_SCAN, &req, NULL);
    if (req.count > 0 && items[0].name[0]) {
        const char* p = strchr(items[0].name, ':');
        wchar_t w[64];
        os_utf8_to_wide_buf(p ? p + 1 : items[0].name, w, 64);
        SetWindowTextW(GetDlgItem(hwnd, IDC_XW_PORT), w);
    }
    {
        wchar_t msg[64];
        _snwprintf(msg, 64, L"扫描到 %d 个端点", req.count);
        xw_set_status(hwnd, msg);
    }
}

static void xw_do_read(HWND hwnd)
{
    uint32_t addr = 0, size = 0;
    uint8_t buf[256];
    char hex[1200];
    wchar_t wh[1200];
    int i, n, rc;
    if (xw_parse_u32(hwnd, IDC_XW_ADDR, &addr) != 0 ||
        xw_parse_u32(hwnd, IDC_XW_SIZE, &size) != 0 || size == 0 || size > sizeof(buf)) {
        xw_set_status(hwnd, L"地址/长度无效");
        return;
    }
    rc = xw_read_bytes(addr, size, buf);
    if (rc < 0) {
        xw_set_status(hwnd, rc == OS_ERR_NOT_CONNECTED ? L"请先连接 XCP" : L"UPLOAD 失败");
        return;
    }
    n = 0;
    for (i = 0; i < (int)size && n < (int)sizeof(hex) - 4; i++)
        n += _snprintf(hex + n, sizeof(hex) - n, "%02X%s", buf[i],
                       (i + 1 < (int)size) ? " " : "");
    os_utf8_to_wide_buf(hex, wh, 1200);
    SetWindowTextW(GetDlgItem(hwnd, IDC_XW_HEX), wh);
    if (g_sel_valid && g_sel) {
        char val[128];
        if (os_xcpsym_format_value(g_sel, buf, (int)size, val, sizeof(val)) == 0) {
            wchar_t wv[128];
            os_utf8_to_wide_buf(val, wv, 128);
            SetWindowTextW(GetDlgItem(hwnd, IDC_XW_VALUE), wv);
        }
    }
    xw_set_status(hwnd, L"读取成功");
}

static int hex_nibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int parse_hex_bytes(const char* s, uint8_t* out, int cap)
{
    int n = 0, hi = -1, v;
    for (; *s && n < cap; s++) {
        if (*s == ' ' || *s == '\t' || *s == ',' || *s == '-') continue;
        v = hex_nibble(*s);
        if (v < 0) return -1;
        if (hi < 0) hi = v;
        else { out[n++] = (uint8_t)((hi << 4) | v); hi = -1; }
    }
    if (hi >= 0) return -1;
    return n;
}

static void xw_do_write(HWND hwnd)
{
    uint32_t addr = 0;
    uint8_t buf[256];
    wchar_t wh[1200];
    char hex[1200];
    int n, rc;
    if (xw_parse_u32(hwnd, IDC_XW_ADDR, &addr) != 0) {
        xw_set_status(hwnd, L"地址无效");
        return;
    }
    GetWindowTextW(GetDlgItem(hwnd, IDC_XW_HEX), wh, 1200);
    os_wide_to_utf8_buf(wh, hex, sizeof(hex));
    n = parse_hex_bytes(hex, buf, (int)sizeof(buf));
    if (n <= 0) {
        xw_set_status(hwnd, L"HEX 数据无效");
        return;
    }
    rc = xw_write_bytes(addr, buf, (uint32_t)n);
    xw_set_status(hwnd, (rc == OS_ERR_OK) ? L"写入成功" : L"DOWNLOAD 失败");
}

static uint64_t xw_load_le(const uint8_t* p, int n)
{
    uint64_t v = 0;
    int i;
    if (n > 8) n = 8;
    for (i = 0; i < n; i++) v |= ((uint64_t)p[i]) << (8 * i);
    return v;
}

static void xw_store_le(uint8_t* p, uint64_t v, int n)
{
    int i;
    if (n > 8) n = 8;
    for (i = 0; i < n; i++) p[i] = (uint8_t)((v >> (8 * i)) & 0xFF);
}

/* 按选中符号的类型写入十进制/浮点值（位域做 read-modify-write） */
static void xw_do_write_value(HWND hwnd)
{
    OS_XcpSym sym;
    wchar_t wv[128];
    char text[128], err[128];
    uint8_t enc[8], raw[8];
    uint32_t addr = 0;
    int n, rc;
    if (xw_parse_u32(hwnd, IDC_XW_ADDR, &addr) != 0) {
        xw_set_status(hwnd, L"地址无效");
        return;
    }
    GetWindowTextW(GetDlgItem(hwnd, IDC_XW_VALUE), wv, 128);
    os_wide_to_utf8_buf(wv, text, sizeof(text));
    if (!text[0]) {
        xw_set_status(hwnd, L"请输入值");
        return;
    }
    if (g_sel_valid && g_sel) {
        sym = *g_sel;
    } else {
        uint32_t size = 4;
        xw_parse_u32(hwnd, IDC_XW_SIZE, &size);
        if (size == 0 || size > 8) size = 4;
        memset(&sym, 0, sizeof(sym));
        sym.address = addr;
        sym.size = size;
        sym.kind = OS_TYPE_UINT;
    }
    sym.address = addr;
    n = os_xcpsym_encode_value(&sym, text, enc, (int)sizeof(enc), err, sizeof(err));
    if (n <= 0) {
        wchar_t we[128];
        os_utf8_to_wide_buf(err, we, 128);
        xw_set_status(hwnd, we[0] ? we : L"值无效");
        return;
    }
    if (sym.is_bitfield && sym.size <= 8) {
        uint64_t mask = os_xcpsym_bit_mask(&sym);
        uint64_t base, val;
        int rr = xw_read_bytes(addr, sym.size, raw);
        if (rr < 0) {
            xw_set_status(hwnd, L"位域读取失败");
            return;
        }
        base = xw_load_le(raw, (int)sym.size);
        val = xw_load_le(enc, n);
        base = (base & ~mask) | ((val << sym.bit_offset) & mask);
        xw_store_le(raw, base, (int)sym.size);
        rc = xw_write_bytes(addr, raw, sym.size);
    } else {
        rc = xw_write_bytes(addr, enc, (uint32_t)n);
    }
    xw_set_status(hwnd, (rc == OS_ERR_OK) ? L"写入成功" : L"DOWNLOAD 失败");
}

static void xw_load_elf_dialog(HWND hwnd)
{
    OPENFILENAMEW ofn;
    wchar_t file[MAX_PATH] = L"";
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"ELF/out 文件\0*.elf;*.out;*.axf\0所有文件\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetOpenFileNameW(&ofn)) return;
    if (os_mainwin_open_elf(file) != 0) {
        xw_set_status(hwnd, L"ELF/out 加载失败（详见消息栏）");
        return;
    }
    os_xcpsym_invalidate();
    xw_fill_symbols(hwnd);
    xw_set_status(hwnd, L"ELF/out 已加载，符号表已刷新");
}

static void xw_load_map_dialog(HWND hwnd)
{
    OPENFILENAMEW ofn;
    wchar_t file[MAX_PATH] = L"";
    char err[256];
    wchar_t werr[256];
    int n;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"MAP 文件\0*.map;*.txt\0所有文件\0*.*\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetOpenFileNameW(&ofn)) return;
    n = os_xcpsym_load_map(file, err, sizeof(err));
    if (n < 0) {
        os_utf8_to_wide_buf(err[0] ? err : "MAP 解析失败", werr, 256);
        xw_set_status(hwnd, werr);
        return;
    }
    xw_fill_symbols(hwnd);
    {
        wchar_t msg[128];
        _snwprintf(msg, 128, L"MAP 已加载 %d 个符号", n);
        xw_set_status(hwnd, msg);
    }
}

static LRESULT CALLBACK xcpwin_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE: {
        CreateWindowW(L"STATIC", L"传输", WS_CHILD | WS_VISIBLE, 10, 12, 40, 20, hwnd, 0, g_app.hInst, 0);
        CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                      55, 10, 80, 120, hwnd, (HMENU)(UINT_PTR)IDC_XW_TRANSPORT, g_app.hInst, 0);
        CreateWindowW(L"STATIC", L"端口", WS_CHILD | WS_VISIBLE, 145, 12, 36, 20, hwnd, 0, g_app.hInst, 0);
        CreateWindowW(L"EDIT", L"COM5", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                      185, 10, 90, 22, hwnd, (HMENU)(UINT_PTR)IDC_XW_PORT, g_app.hInst, 0);
        CreateWindowW(L"STATIC", L"波特", WS_CHILD | WS_VISIBLE, 285, 12, 36, 20, hwnd, 0, g_app.hInst, 0);
        CreateWindowW(L"EDIT", L"115200", WS_CHILD | WS_VISIBLE | WS_BORDER,
                      325, 10, 70, 22, hwnd, (HMENU)(UINT_PTR)IDC_XW_BAUD, g_app.hInst, 0);
        CreateWindowW(L"BUTTON", L"扫描", WS_CHILD | WS_VISIBLE, 410, 9, 60, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_SCAN, g_app.hInst, 0);
        CreateWindowW(L"BUTTON", L"连接", WS_CHILD | WS_VISIBLE, 475, 9, 60, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_CONNECT, g_app.hInst, 0);
        CreateWindowW(L"BUTTON", L"断开", WS_CHILD | WS_VISIBLE, 540, 9, 60, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_DISCON, g_app.hInst, 0);
        CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE, 610, 12, 180, 20, hwnd, (HMENU)(UINT_PTR)IDC_XW_STATUS, g_app.hInst, 0);

        CreateWindowW(L"BUTTON", L"加载 ELF/out...", WS_CHILD | WS_VISIBLE,
                      10, 40, 110, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_LOADELF, g_app.hInst, 0);
        CreateWindowW(L"BUTTON", L"加载 MAP...", WS_CHILD | WS_VISIBLE,
                      126, 40, 90, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_LOADMAP, g_app.hInst, 0);
        CreateWindowW(L"STATIC", L"MAP: 未加载", WS_CHILD | WS_VISIBLE,
                      222, 44, 190, 20, hwnd, (HMENU)(UINT_PTR)IDC_XW_MAPINFO, g_app.hInst, 0);
        CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                      420, 41, 210, 22, hwnd, (HMENU)(UINT_PTR)IDC_XW_FILTER, g_app.hInst, 0);
        CreateWindowW(L"BUTTON", L"筛选", WS_CHILD | WS_VISIBLE,
                      636, 40, 54, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_REFRESH, g_app.hInst, 0);
        CreateWindowW(L"BUTTON", L"关闭", WS_CHILD | WS_VISIBLE,
                      700, 40, 60, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_CLOSE, g_app.hInst, 0);

        CreateWindowW(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                      10, 70, 780, 220, hwnd, (HMENU)(UINT_PTR)IDC_XW_SYMLIST, g_app.hInst, 0);

        CreateWindowW(L"STATIC", L"地址", WS_CHILD | WS_VISIBLE, 10, 302, 40, 20, hwnd, 0, g_app.hInst, 0);
        CreateWindowW(L"EDIT", L"0x20000000", WS_CHILD | WS_VISIBLE | WS_BORDER,
                      55, 300, 105, 22, hwnd, (HMENU)(UINT_PTR)IDC_XW_ADDR, g_app.hInst, 0);
        CreateWindowW(L"STATIC", L"长度", WS_CHILD | WS_VISIBLE, 168, 302, 36, 20, hwnd, 0, g_app.hInst, 0);
        CreateWindowW(L"EDIT", L"4", WS_CHILD | WS_VISIBLE | WS_BORDER,
                      208, 300, 42, 22, hwnd, (HMENU)(UINT_PTR)IDC_XW_SIZE, g_app.hInst, 0);
        CreateWindowW(L"STATIC", L"值", WS_CHILD | WS_VISIBLE, 258, 302, 24, 20, hwnd, 0, g_app.hInst, 0);
        CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                      284, 300, 110, 22, hwnd, (HMENU)(UINT_PTR)IDC_XW_VALUE, g_app.hInst, 0);
        CreateWindowW(L"BUTTON", L"选符号", WS_CHILD | WS_VISIBLE,
                      400, 299, 60, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_PICK, g_app.hInst, 0);
        CreateWindowW(L"BUTTON", L"XCP读", WS_CHILD | WS_VISIBLE,
                      466, 299, 60, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_READ, g_app.hInst, 0);
        CreateWindowW(L"BUTTON", L"写 HEX", WS_CHILD | WS_VISIBLE,
                      532, 299, 64, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_WRITE, g_app.hInst, 0);
        CreateWindowW(L"BUTTON", L"写值", WS_CHILD | WS_VISIBLE,
                      602, 299, 60, 24, hwnd, (HMENU)(UINT_PTR)IDC_XW_WRITEVAL, g_app.hInst, 0);
        CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE,
                      670, 302, 120, 20, hwnd, (HMENU)(UINT_PTR)IDC_XW_TYPEINFO, g_app.hInst, 0);

        CreateWindowW(L"STATIC", L"HEX（空格分隔，读/写原始字节）", WS_CHILD | WS_VISIBLE, 10, 334, 300, 18, hwnd, 0, g_app.hInst, 0);
        CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
                      10, 354, 780, 100, hwnd, (HMENU)(UINT_PTR)IDC_XW_HEX, g_app.hInst, 0);

        {
            HWND cb = GetDlgItem(hwnd, IDC_XW_TRANSPORT);
            SendMessageW(cb, CB_ADDSTRING, 0, (LPARAM)L"serial");
            SendMessageW(cb, CB_ADDSTRING, 0, (LPARAM)L"can");
            SendMessageW(cb, CB_ADDSTRING, 0, (LPARAM)L"tcp");
            SendMessageW(cb, CB_SETCURSEL, 0, 0);
            if (g_app.xcp_port[0]) {
                wchar_t w[64];
                os_utf8_to_wide_buf(g_app.xcp_port, w, 64);
                SetWindowTextW(GetDlgItem(hwnd, IDC_XW_PORT), w);
            }
            if (g_app.xcp_baud > 0) {
                wchar_t w[32];
                _snwprintf(w, 32, L"%d", g_app.xcp_baud);
                SetWindowTextW(GetDlgItem(hwnd, IDC_XW_BAUD), w);
            }
        }
        xw_fill_symbols(hwnd);
        xw_set_status(hwnd, g_app.xcp_connected ? L"已连接" : L"未连接");
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_XW_SCAN: xw_do_scan(hwnd); break;
        case IDC_XW_CONNECT: xw_do_connect(hwnd); break;
        case IDC_XW_DISCON: xw_do_disconnect(hwnd); break;
        case IDC_XW_REFRESH: xw_fill_symbols(hwnd); break;
        case IDC_XW_READ: xw_do_read(hwnd); break;
        case IDC_XW_WRITE: xw_do_write(hwnd); break;
        case IDC_XW_WRITEVAL: xw_do_write_value(hwnd); break;
        case IDC_XW_LOADELF: xw_load_elf_dialog(hwnd); break;
        case IDC_XW_LOADMAP: xw_load_map_dialog(hwnd); break;
        case IDC_XW_CLOSE: DestroyWindow(hwnd); break;
        case IDC_XW_PICK: {
            int id = 0;
            if (os_dlg_pick_var(hwnd, &id) == 0) {
                OS_XcpSym s;
                memset(&s, 0, sizeof(s));
                _snprintf(s.name, sizeof(s.name), "%s", g_app.leaves[id].name);
                s.address = g_app.leaves[id].address;
                s.size = g_app.leaves[id].size;
                s.kind = (int)g_app.leaves[id].kind;
                s.is_signed = g_app.leaves[id].is_signed;
                s.is_float = (g_app.leaves[id].kind == OS_TYPE_FLOAT);
                s.is_bitfield = g_app.leaves[id].is_bitfield;
                s.bit_offset = g_app.leaves[id].bit_offset;
                s.bit_size = g_app.leaves[id].bit_size;
                s.leaf_id = id;
                if (!g_sel) g_sel = (OS_XcpSym*)malloc(sizeof(OS_XcpSym));
                if (g_sel) { *g_sel = s; g_sel_valid = 1; }
                {
                    wchar_t wa[32], ws[16], wt[256];
                    _snwprintf(wa, 32, L"0x%08llX", (unsigned long long)s.address);
                    _snwprintf(ws, 16, L"%u", (unsigned)s.size);
                    SetWindowTextW(GetDlgItem(hwnd, IDC_XW_ADDR), wa);
                    SetWindowTextW(GetDlgItem(hwnd, IDC_XW_SIZE), ws);
                    os_utf8_to_wide_buf(s.name, wt, 256);
                    xw_set_typeinfo(hwnd, wt);
                }
            }
            break;
        }
        case IDC_XW_SYMLIST:
            if (HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == LBN_DBLCLK) {
                LRESULT i = SendMessageW(GetDlgItem(hwnd, IDC_XW_SYMLIST), LB_GETCURSEL, 0, 0);
                if (i != LB_ERR) xw_apply_sym(hwnd, (int)i);
            }
            break;
        }
        return 0;
    case WM_DESTROY:
        if (g_xcpwin == hwnd) g_xcpwin = NULL;
        free(g_sel);
        g_sel = NULL;
        g_sel_valid = 0;
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void os_xcpwin_show(HWND owner)
{
    WNDCLASSW wc;
    if (g_xcpwin && IsWindow(g_xcpwin)) {
        ShowWindow(g_xcpwin, SW_SHOW);
        SetForegroundWindow(g_xcpwin);
        os_xcpsym_invalidate();
        xw_fill_symbols(g_xcpwin);
        return;
    }
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = xcpwin_proc;
    wc.hInstance = g_app.hInst;
    wc.lpszClassName = L"OpenScopeXcpWin";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);
    g_xcpwin = CreateWindowExW(WS_EX_TOOLWINDOW, L"OpenScopeXcpWin", L"XCP 标定 / 调试",
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                               CW_USEDEFAULT, CW_USEDEFAULT, 820, 530,
                               owner ? owner : g_app.hMain, NULL, g_app.hInst, NULL);
}

void os_xcpwin_hide(void)
{
    if (g_xcpwin && IsWindow(g_xcpwin))
        ShowWindow(g_xcpwin, SW_HIDE);
}

void os_xcpwin_refresh(void)
{
    if (g_xcpwin && IsWindow(g_xcpwin)) {
        xw_set_status(g_xcpwin, g_app.xcp_connected ? L"已连接" : L"未连接");
        if (IsWindowVisible(g_xcpwin))
            xw_fill_symbols(g_xcpwin);
    }
}
