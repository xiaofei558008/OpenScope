#include "util.h"
#include "module_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

static void (*g_log_fn)(int level, const wchar_t* line);
static FILE* g_log_file;
static CRITICAL_SECTION g_log_cs;
static int g_log_cs_ok;
static wchar_t g_log_override[MAX_PATH]; /* --log= 测试钩子 */

void os_log_set(void (*fn)(int level, const wchar_t* line)) { g_log_fn = fn; }

void os_log_file_open(const wchar_t* path)
{
    long pos;
    if (g_log_file) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    if (!path || !path[0]) return;
    g_log_file = _wfopen(path, L"ab");
    if (!g_log_file) return;
    fseek(g_log_file, 0, SEEK_END);
    pos = ftell(g_log_file);
    if (pos == 0) fputs("\xEF\xBB\xBF", g_log_file);
    fflush(g_log_file);
}

void os_log_file_set_override(const wchar_t* path)
{
    if (path && path[0]) _snwprintf(g_log_override, MAX_PATH, L"%s", path);
}

void os_log_file_auto_open(void)
{
    wchar_t path[MAX_PATH];
    wchar_t exe[MAX_PATH];
    wchar_t* slash;
    FILE* f;
    DWORD n;
    if (g_log_override[0]) {
        f = _wfopen(g_log_override, L"ab");
        if (f) {
            fclose(f);
            os_log_file_open(g_log_override);
            return;
        }
    }
    n = GetModuleFileNameW(NULL, exe, MAX_PATH);
    if (n && (slash = wcsrchr(exe, L'\\'))) {
        *slash = 0;
        _snwprintf(path, MAX_PATH, L"%s\\openscope.log", exe);
        f = _wfopen(path, L"ab");
        if (f) {
            fclose(f);
            os_log_file_open(path);
            return;
        }
    }
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", exe, MAX_PATH) > 0) {
        _snwprintf(path, MAX_PATH, L"%s\\OpenScope", exe);
        CreateDirectoryW(path, NULL);
        _snwprintf(path, MAX_PATH, L"%s\\OpenScope\\openscope.log", exe);
        os_log_file_open(path);
    }
}

void os_log_file_write_raw(const char* line)
{
    if (!g_log_cs_ok) {
        InitializeCriticalSection(&g_log_cs);
        g_log_cs_ok = 1;
    }
    EnterCriticalSection(&g_log_cs);
    if (g_log_file && line) {
        fputs(line, g_log_file);
        fputc('\n', g_log_file);
        fflush(g_log_file);
    }
    LeaveCriticalSection(&g_log_cs);
}

void os_save_window_bmp(HWND hwnd, const wchar_t* path)
{
    RECT rc;
    HDC wdc, mem;
    HBITMAP bmp, old;
    BITMAPINFO bmi;
    BITMAPFILEHEADER bfh;
    void* bits;
    FILE* f;
    int i, stride;
    if (!hwnd || !IsWindow(hwnd) || !path) return;
    GetClientRect(hwnd, &rc);
    if (rc.right <= 0 || rc.bottom <= 0) return;
    /* 先强制同步重绘，再抓取窗口内容 */
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    wdc = GetDC(hwnd);
    mem = CreateCompatibleDC(wdc);
    bmp = CreateCompatibleBitmap(wdc, rc.right, rc.bottom);
    if (!bmp) { ReleaseDC(hwnd, wdc); return; }
    old = (HBITMAP)SelectObject(mem, bmp);
    /* 用 WM_PRINT 抓取窗口自身的绘制输出（非交互会话也能拿到真实绘制内容） */
    SendMessageW(hwnd, WM_PRINT, (WPARAM)mem, PRF_CLIENT | PRF_ERASEBKGND | PRF_NONCLIENT);
    SelectObject(mem, old);
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = rc.right;
    bmi.bmiHeader.biHeight = -rc.bottom;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    stride = ((rc.right * 4 + 3) / 4) * 4;
    bits = malloc((size_t)stride * rc.bottom);
    if (bits) {
        GetDIBits(mem, bmp, 0, rc.bottom, bits, &bmi, DIB_RGB_COLORS);
        f = _wfopen(path, L"wb");
        if (f) {
            memset(&bfh, 0, sizeof(bfh));
            bfh.bfType = 0x4D42;
            bfh.bfOffBits = sizeof(bfh) + sizeof(BITMAPINFOHEADER);
            bfh.bfSize = bfh.bfOffBits + (DWORD)stride * rc.bottom;
            fwrite(&bfh, 1, sizeof(bfh), f);
            fwrite(&bmi.bmiHeader, 1, sizeof(BITMAPINFOHEADER), f);
            fwrite(bits, 1, (size_t)stride * rc.bottom, f);
            fclose(f);
        }
        free(bits);
    }
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(hwnd, wdc);
    (void)i;
}

void os_log(int level, const char* fmt, ...)
{
    char buf[1024];
    wchar_t wbuf[1024];
    char line[1152];
    char ts[40];
    const char* lv = "INFO";
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    os_time_iso(os_time_us(), ts, sizeof(ts));
    switch (level) {
    case OS_LOG_WARN: lv = "WARN"; break;
    case OS_LOG_ERROR: lv = "ERROR"; break;
    case OS_LOG_DEBUG: lv = "DEBUG"; break;
    default: break;
    }
    _snprintf(line, sizeof(line), "%s [%s] %s", ts, lv, buf);
    os_log_file_write_raw(line);
    os_utf8_to_wide_buf(buf, wbuf, 1024);
    OutputDebugStringW(wbuf);
    OutputDebugStringW(L"\n");
    if (g_log_fn) g_log_fn(level, wbuf);
}

int64_t os_time_us(void)
{
    FILETIME ft;
    ULARGE_INTEGER u;
    GetSystemTimeAsFileTime(&ft);
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    return (int64_t)((u.QuadPart - 116444736000000000ULL) / 10);
}

void os_time_iso(int64_t us, char* out, int outlen)
{
    time_t sec = (time_t)(us / 1000000);
    int usec = (int)(us % 1000000);
    struct tm tmv;
    if (sec < 0) sec = 0;
    localtime_s(&tmv, &sec);
    snprintf(out, outlen, "%04d-%02d-%02d %02d:%02d:%02d.%06d",
             tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
             tmv.tm_hour, tmv.tm_min, tmv.tm_sec, usec);
}

wchar_t* os_utf8_to_wide(const char* s)
{
    int n;
    wchar_t* w;
    if (!s) return NULL;
    n = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    w = (wchar_t*)malloc((n + 1) * sizeof(wchar_t));
    if (!w) return NULL;
    MultiByteToWideChar(CP_UTF8, 0, s, -1, w, n + 1);
    return w;
}

char* os_wide_to_utf8(const wchar_t* s)
{
    int n;
    char* c;
    if (!s) return NULL;
    n = WideCharToMultiByte(CP_UTF8, 0, s, -1, NULL, 0, NULL, NULL);
    c = (char*)malloc(n + 1);
    if (!c) return NULL;
    WideCharToMultiByte(CP_UTF8, 0, s, -1, c, n + 1, NULL, NULL);
    return c;
}

void os_utf8_to_wide_buf(const char* s, wchar_t* out, int outlen)
{
    if (!s) { if (outlen > 0) out[0] = 0; return; }
    MultiByteToWideChar(CP_UTF8, 0, s, -1, out, outlen);
    out[outlen - 1] = 0;
}

void os_wide_to_utf8_buf(const wchar_t* s, char* out, int outlen)
{
    if (!s) { if (outlen > 0) out[0] = 0; return; }
    WideCharToMultiByte(CP_UTF8, 0, s, -1, out, outlen, NULL, NULL);
    out[outlen - 1] = 0;
}

uint64_t os_file_mtime_ms(const wchar_t* path)
{
    WIN32_FILE_ATTRIBUTE_DATA fd;
    ULARGE_INTEGER u;
    if (!GetFileAttributesExW(path, GetFileExInfoStandard, &fd)) return 0;
    u.LowPart = fd.ftLastWriteTime.dwLowDateTime;
    u.HighPart = fd.ftLastWriteTime.dwHighDateTime;
    return u.QuadPart / 10000;
}

static uint64_t read_le(const uint8_t* raw, int size)
{
    uint64_t v = 0;
    int i;
    for (i = 0; i < size && i < 8; i++) v |= ((uint64_t)raw[i]) << (8 * i);
    return v;
}

static void write_le(uint8_t* out, uint64_t v, int size)
{
    int i;
    for (i = 0; i < size; i++) out[i] = (uint8_t)(v >> (8 * i));
}

double os_decode_value(const uint8_t* raw, int size, OS_TypeKind kind, int is_signed,
                       int is_bitfield, int bit_offset, int bit_size, int* ok)
{
    uint64_t v;
    int bits;
    if (ok) *ok = 1;
    if (!raw || size < 1 || size > 8) { if (ok) *ok = 0; return 0.0; }
    if (kind == OS_TYPE_FLOAT) {
        if (size == 4) {
            float f;
            memcpy(&f, raw, 4);
            return (double)f;
        }
        if (size == 8) {
            double d;
            memcpy(&d, raw, 8);
            return d;
        }
        if (ok) *ok = 0;
        return 0.0;
    }
    if (kind == OS_TYPE_STRING || kind == OS_TYPE_VOID) {
        if (ok) *ok = 0;
        return 0.0;
    }
    v = read_le(raw, size);
    bits = size * 8;
    if (is_bitfield) {
        bits = bit_size;
        if (bits <= 0 || bits > 64) bits = 32;
        if (bit_offset >= 0 && bit_offset < 64) v >>= bit_offset;
        if (bits < 64) v &= ((1ULL << bits) - 1);
    }
    if (kind == OS_TYPE_BOOL) return v ? 1.0 : 0.0;
    if (is_signed && bits < 64 && (v & (1ULL << (bits - 1)))) v |= ~((1ULL << bits) - 1);
    return (double)(int64_t)v;
}

void os_format_raw(char* out, int outlen, const uint8_t* raw, int size,
                   OS_TypeKind kind, int is_signed, int is_ptr, int is_bitfield, int bit_offset, int bit_size,
                   double* out_val, const OS_EnumVal* enums, int enum_count)
{
    int ok = 1;
    double v = 0.0;
    int i;
    if (!out || outlen <= 0) return;
    out[0] = 0;
    v = os_decode_value(raw, size, kind, is_signed, is_bitfield, bit_offset, bit_size, &ok);
    if (out_val) *out_val = ok ? v : 0.0;
    if (!ok) {
        snprintf(out, outlen, "<?>");
        return;
    }
    switch (kind) {
    case OS_TYPE_FLOAT:
        snprintf(out, outlen, "%.6g", v);
        break;
    case OS_TYPE_BOOL:
        snprintf(out, outlen, "%s", v ? "true" : "false");
        break;
    case OS_TYPE_INT:
    case OS_TYPE_UINT:
        if (is_ptr) snprintf(out, outlen, "0x%llX", (unsigned long long)(uint64_t)v);
        else if (is_signed) snprintf(out, outlen, "%lld", (long long)v);
        else snprintf(out, outlen, "%llu", (unsigned long long)v);
        break;
    case OS_TYPE_PTR:
        snprintf(out, outlen, "0x%llX", (unsigned long long)(uint64_t)v);
        break;
    case OS_TYPE_ENUM:
        for (i = 0; i < enum_count; i++) {
            if (enums && enums[i].value == (int64_t)v) {
                snprintf(out, outlen, "%s", enums[i].name);
                return;
            }
        }
        snprintf(out, outlen, "%lld", (long long)v);
        break;
    case OS_TYPE_STRING: {
        int n = size;
        if (n >= outlen) n = outlen - 1;
        memcpy(out, raw, n);
        out[n] = 0;
        break;
    }
    case OS_TYPE_OTHER:
    default: {
        char tmp[128];
        int p = 0;
        if (size > 32) size = 32;
        tmp[p++] = '0';
        tmp[p++] = 'x';
        for (i = 0; i < size && p < (int)sizeof(tmp) - 3; i++) {
            snprintf(tmp + p, sizeof(tmp) - p, "%02X", raw[i]);
            p += 2;
        }
        tmp[p] = 0;
        snprintf(out, outlen, "%s", tmp);
        break;
    }
    }
}

int os_parse_text(const char* text, uint8_t* out, int max_size, int* out_size,
                  OS_TypeKind kind, int is_signed, int is_bitfield, int bit_offset, int bit_size,
                  const OS_EnumVal* enums, int enum_count)
{
    int64_t ival;
    uint64_t uv;
    double dval;
    char* end = NULL;
    int i;
    if (!text || !out || max_size <= 0) return 0;
    if (out_size) *out_size = 0;
    if (max_size > 8) max_size = 8;
    switch (kind) {
    case OS_TYPE_INT:
        ival = strtoll(text, &end, 0);
        if (end == text) return 0;
        memset(out, 0, max_size);
        write_le(out, (uint64_t)ival, max_size);
        if (out_size) *out_size = max_size;
        return 1;
    case OS_TYPE_UINT:
    case OS_TYPE_PTR:
        uv = strtoull(text, &end, 0);
        if (end == text) return 0;
        memset(out, 0, max_size);
        write_le(out, uv, max_size);
        if (out_size) *out_size = max_size;
        return 1;
    case OS_TYPE_FLOAT:
        dval = strtod(text, &end);
        if (end == text) return 0;
        memset(out, 0, max_size);
        if (max_size >= 8) {
            double d = dval;
            memcpy(out, &d, 8);
            if (out_size) *out_size = 8;
        } else {
            float f = (float)dval;
            memcpy(out, &f, 4);
            if (out_size) *out_size = 4;
        }
        return 1;
    case OS_TYPE_BOOL:
        if (!_stricmp(text, "true") || !strcmp(text, "1")) out[0] = 1;
        else if (!_stricmp(text, "false") || !strcmp(text, "0")) out[0] = 0;
        else return 0;
        if (out_size) *out_size = 1;
        return 1;
    case OS_TYPE_ENUM:
        for (i = 0; i < enum_count; i++) {
            if (enums && !_stricmp(text, enums[i].name)) {
                memset(out, 0, max_size);
                write_le(out, (uint64_t)enums[i].value, max_size);
                if (out_size) *out_size = max_size;
                return 1;
            }
        }
        ival = strtoll(text, &end, 0);
        if (end == text) return 0;
        memset(out, 0, max_size);
        write_le(out, (uint64_t)ival, max_size);
        if (out_size) *out_size = max_size;
        return 1;
    case OS_TYPE_STRING:
        ival = (int64_t)strlen(text);
        if (ival >= max_size) ival = max_size - 1;
        memcpy(out, text, (size_t)ival);
        out[ival] = 0;
        if (out_size) *out_size = (int)ival + 1;
        return 1;
    case OS_TYPE_OTHER:
    default:
        uv = strtoull(text, &end, 0);
        if (end == text) return 0;
        memset(out, 0, max_size);
        write_le(out, uv, max_size);
        if (out_size) *out_size = max_size;
        return 1;
    }
}
