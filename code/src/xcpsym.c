#include "xcpsym.h"
#include "app.h"
#include "mapfile.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

static CRITICAL_SECTION g_cs;
static INIT_ONCE g_once = INIT_ONCE_STATIC_INIT;

static OS_MapSym* g_map;
static int        g_map_count;
static wchar_t    g_map_path[MAX_PATH];

static OS_XcpSym* g_cache;
static int        g_cache_count;
static int        g_cache_valid;
static OS_ElfFile* g_cache_elf;
static int        g_cache_leaf_count;

static BOOL CALLBACK xcpsym_once(PINIT_ONCE once, PVOID param, PVOID* ctx)
{
    (void)once; (void)param; (void)ctx;
    InitializeCriticalSection(&g_cs);
    return TRUE;
}

static void xcpsym_lock(void)
{
    InitOnceExecuteOnce(&g_once, xcpsym_once, NULL, NULL);
    EnterCriticalSection(&g_cs);
}

static void xcpsym_unlock(void)
{
    LeaveCriticalSection(&g_cs);
}

static uint32_t xcpsym_hash(const char* s)
{
    uint32_t h = 2166136261u;
    while (s && *s) {
        h ^= (uint8_t)*s++;
        h *= 16777619u;
    }
    return h;
}

static int xcpsym_cmp(const void* a, const void* b)
{
    const OS_XcpSym* x = (const OS_XcpSym*)a;
    const OS_XcpSym* y = (const OS_XcpSym*)b;
    return _stricmp(x->name, y->name);
}

static int xcpsym_utf8(const wchar_t* w, char* out, int cap)
{
    int n;
    if (!w || !out || cap <= 0) return -1;
    n = WideCharToMultiByte(CP_UTF8, 0, w, -1, out, cap, NULL, NULL);
    if (n <= 0) out[0] = 0;
    return n > 0 ? 0 : -1;
}

/* 重建缓存：ELF 叶优先，MAP 同名符号跳过。调用方必须持锁。 */
static void xcpsym_rebuild_locked(void)
{
    int total, i, n = 0, hcap = 1;
    int* hash;
    OS_XcpSym* arr;

    if (g_cache_valid && g_cache_elf == g_app.elf &&
        g_cache_leaf_count == g_app.leaf_count)
        return;

    total = g_app.leaf_count + g_map_count;
    if (total < 0) total = 0;
    while (hcap < total * 2 + 8) hcap <<= 1;
    hash = (int*)calloc((size_t)hcap, sizeof(int));
    arr = (OS_XcpSym*)calloc((size_t)(total > 0 ? total : 1), sizeof(OS_XcpSym));
    if (!hash || !arr) {
        free(hash);
        free(arr);
        return;
    }

    for (i = 0; i < g_app.leaf_count; i++) {
        const OS_Leaf* L = &g_app.leaves[i];
        OS_XcpSym* s;
        uint32_t h;
        int slot;
        if (!L->name[0]) continue;
        h = xcpsym_hash(L->name) & (uint32_t)(hcap - 1);
        slot = (int)h;
        while (hash[slot]) slot = (slot + 1) & (hcap - 1);
        s = &arr[n];
        _snprintf(s->name, sizeof(s->name), "%s", L->name);
        s->address = L->address;
        s->size = L->size ? L->size : 1;
        s->kind = (int)L->kind;
        s->is_signed = L->is_signed;
        s->is_float = (L->kind == OS_TYPE_FLOAT);
        s->is_bitfield = L->is_bitfield;
        s->bit_offset = L->bit_offset;
        s->bit_size = L->bit_size;
        s->leaf_id = i;
        s->from_map = 0;
        hash[slot] = n + 1;
        n++;
    }

    for (i = 0; i < g_map_count; i++) {
        const OS_MapSym* m = &g_map[i];
        OS_XcpSym* s;
        uint32_t h;
        int slot;
        if (!m->name[0]) continue;
        h = xcpsym_hash(m->name) & (uint32_t)(hcap - 1);
        slot = (int)h;
        while (hash[slot] && strcmp(arr[hash[slot] - 1].name, m->name) != 0)
            slot = (slot + 1) & (hcap - 1);
        if (hash[slot]) continue; /* ELF 已有同名符号 */
        s = &arr[n];
        _snprintf(s->name, sizeof(s->name), "%s", m->name);
        s->address = m->address;
        s->size = m->size ? m->size : 4;
        s->kind = OS_TYPE_UINT;
        s->is_signed = 0;
        s->leaf_id = -1;
        s->from_map = 1;
        hash[slot] = n + 1;
        n++;
    }

    free(hash);
    if (n > 1) qsort(arr, (size_t)n, sizeof(OS_XcpSym), xcpsym_cmp);
    free(g_cache);
    g_cache = arr;
    g_cache_count = n;
    g_cache_valid = 1;
    g_cache_elf = g_app.elf;
    g_cache_leaf_count = g_app.leaf_count;
}

void os_xcpsym_invalidate(void)
{
    xcpsym_lock();
    g_cache_valid = 0;
    xcpsym_unlock();
}

int os_xcpsym_load_map(const wchar_t* path_w, char* err, int errcap)
{
    char path8[MAX_PATH];
    OS_MapSym* syms = NULL;
    int n;
    if (err && errcap > 0) err[0] = 0;
    if (!path_w || !path_w[0]) {
        if (err) _snprintf(err, errcap, "路径为空");
        return -1;
    }
    if (xcpsym_utf8(path_w, path8, sizeof(path8)) != 0) {
        if (err) _snprintf(err, errcap, "路径编码失败");
        return -1;
    }
    n = os_map_load(path8, &syms, err, errcap);
    if (n < 0) return -1;
    xcpsym_lock();
    free(g_map);
    g_map = syms;
    g_map_count = n;
    _snwprintf(g_map_path, MAX_PATH, L"%s", path_w);
    g_cache_valid = 0;
    xcpsym_unlock();
    return n;
}

int os_xcpsym_clear_map(void)
{
    xcpsym_lock();
    free(g_map);
    g_map = NULL;
    g_map_count = 0;
    g_map_path[0] = 0;
    g_cache_valid = 0;
    xcpsym_unlock();
    return 0;
}

void os_xcpsym_map_path(char* out, int cap)
{
    if (!out || cap <= 0) return;
    out[0] = 0;
    xcpsym_lock();
    if (g_map_path[0]) xcpsym_utf8(g_map_path, out, cap);
    xcpsym_unlock();
}

int os_xcpsym_count(void)
{
    int n;
    xcpsym_lock();
    xcpsym_rebuild_locked();
    n = g_cache_count;
    xcpsym_unlock();
    return n;
}

int os_xcpsym_get(int index, OS_XcpSym* out)
{
    int ok = -1;
    if (!out) return -1;
    xcpsym_lock();
    xcpsym_rebuild_locked();
    if (index >= 0 && index < g_cache_count) {
        *out = g_cache[index];
        ok = 0;
    }
    xcpsym_unlock();
    return ok;
}

int os_xcpsym_find(const char* name, OS_XcpSym* out)
{
    int i, found = -1;
    if (!name || !name[0] || !out) return -1;
    xcpsym_lock();
    xcpsym_rebuild_locked();
    for (i = 0; i < g_cache_count; i++) {
        if (strcmp(g_cache[i].name, name) == 0) {
            *out = g_cache[i];
            found = 0;
            break;
        }
    }
    xcpsym_unlock();
    return found;
}

int os_xcpsym_search(const char* needle, int offset, int limit,
                     OS_XcpSym* out, int cap)
{
    int i, n = 0, skipped = 0;
    char low[256];
    if (!out || cap <= 0) return 0;
    if (limit <= 0) limit = 100;
    low[0] = 0;
    if (needle && needle[0]) {
        int k;
        for (k = 0; needle[k] && k < (int)sizeof(low) - 1; k++)
            low[k] = (char)tolower((unsigned char)needle[k]);
        low[k] = 0;
    }
    xcpsym_lock();
    xcpsym_rebuild_locked();
    for (i = 0; i < g_cache_count && n < cap && n < limit; i++) {
        char name_low[256];
        int k, match = 1;
        if (low[0]) {
            for (k = 0; g_cache[i].name[k] && k < (int)sizeof(name_low) - 1; k++)
                name_low[k] = (char)tolower((unsigned char)g_cache[i].name[k]);
            name_low[k] = 0;
            match = strstr(name_low, low) != NULL;
        }
        if (!match) continue;
        if (skipped < offset) { skipped++; continue; }
        out[n++] = g_cache[i];
    }
    xcpsym_unlock();
    return n;
}

uint64_t os_xcpsym_bit_mask(const OS_XcpSym* s)
{
    uint64_t mask;
    if (!s || !s->is_bitfield || s->bit_size == 0 || s->bit_size >= 64)
        return 0;
    mask = ((uint64_t)1 << s->bit_size) - 1;
    return mask << s->bit_offset;
}

static uint64_t xcpsym_load_le(const uint8_t* p, int n)
{
    uint64_t v = 0;
    int i;
    if (n > 8) n = 8;
    for (i = 0; i < n; i++) v |= ((uint64_t)p[i]) << (8 * i);
    return v;
}

static void xcpsym_store_le(uint8_t* p, uint64_t v, int n)
{
    int i;
    if (n > 8) n = 8;
    for (i = 0; i < n; i++) p[i] = (uint8_t)((v >> (8 * i)) & 0xFF);
}

int os_xcpsym_format_value(const OS_XcpSym* s, const uint8_t* data, int n,
                           char* out, int cap)
{
    uint64_t raw;
    if (!s || !data || n <= 0 || !out || cap <= 0) return -1;
    if (n > (int)s->size && s->size > 0) n = (int)s->size;
    raw = xcpsym_load_le(data, n);
    if (s->is_bitfield) {
        uint64_t mask = os_xcpsym_bit_mask(s);
        unsigned long long v;
        if (!mask) return -1;
        v = (unsigned long long)((raw & mask) >> s->bit_offset);
        if (s->is_signed && s->bit_size > 0 && s->bit_size < 64) {
            uint64_t sign = (uint64_t)1 << (s->bit_size - 1);
            if (v & sign) v |= ~(((uint64_t)1 << s->bit_size) - 1);
            _snprintf(out, cap, "%lld", (long long)v);
        } else {
            _snprintf(out, cap, "%llu", v);
        }
        return 0;
    }
    if (s->is_float && n == 4) {
        float f;
        memcpy(&f, data, 4);
        _snprintf(out, cap, "%.9g", (double)f);
        return 0;
    }
    if (s->is_float && n == 8) {
        double d;
        memcpy(&d, data, 8);
        _snprintf(out, cap, "%.17g", d);
        return 0;
    }
    if (n == 1) {
        if (s->is_signed) _snprintf(out, cap, "%d", (int)(int8_t)raw);
        else             _snprintf(out, cap, "%u", (unsigned)(uint8_t)raw);
    } else if (n == 2) {
        if (s->is_signed) _snprintf(out, cap, "%d", (int)(int16_t)raw);
        else             _snprintf(out, cap, "%u", (unsigned)(uint16_t)raw);
    } else if (n == 4) {
        if (s->is_signed) _snprintf(out, cap, "%d", (int)(int32_t)raw);
        else             _snprintf(out, cap, "%u", (unsigned)(uint32_t)raw);
    } else if (n == 8) {
        if (s->is_signed) _snprintf(out, cap, "%lld", (long long)(int64_t)raw);
        else             _snprintf(out, cap, "%llu", (unsigned long long)raw);
    } else {
        int i, p = 0;
        for (i = 0; i < n && p < cap - 3; i++)
            p += _snprintf(out + p, cap - p, "%02X", data[i]);
        return 0;
    }
    return 0;
}

static int xcpsym_parse_number(const char* text, int is_signed, int is_float,
                               double* fout, uint64_t* iout)
{
    char buf[128];
    char* end = NULL;
    int i;
    if (!text) return -1;
    for (i = 0; text[i] && i < (int)sizeof(buf) - 1; i++) {
        if (text[i] == ' ' || text[i] == '\t') continue;
        buf[i] = text[i];
    }
    buf[i] = 0;
    /* 去掉所有空白 */
    {
        int j = 0, k;
        for (k = 0; buf[k]; k++)
            if (buf[k] != ' ' && buf[k] != '\t') buf[j++] = buf[k];
        buf[j] = 0;
    }
    if (!buf[0]) return -1;
    if (buf[0] == '0' && (buf[1] == 'x' || buf[1] == 'X')) {
        unsigned long long v = _strtoui64(buf + 2, &end, 16);
        if (!end || *end) return -1;
        *iout = (uint64_t)v;
        *fout = (double)v;
        return 0;
    }
    if (is_float || strchr(buf, '.') || strchr(buf, 'e') || strchr(buf, 'E')) {
        double d = strtod(buf, &end);
        if (!end || *end) return -1;
        *fout = d;
        *iout = (uint64_t)(int64_t)d;
        return 0;
    }
    if (is_signed) {
        long long v = _strtoi64(buf, &end, 10);
        if (!end || *end) return -1;
        *iout = (uint64_t)v;
        *fout = (double)v;
    } else {
        unsigned long long v = _strtoui64(buf, &end, 10);
        if (!end || *end) return -1;
        *iout = (uint64_t)v;
        *fout = (double)v;
    }
    return 0;
}

int os_xcpsym_encode_value(const OS_XcpSym* s, const char* text,
                           uint8_t* out, int cap, char* err, int errcap)
{
    double fv = 0;
    uint64_t iv = 0;
    int n;
    if (err && errcap > 0) err[0] = 0;
    if (!s || !text || !out) return -1;
    n = (int)s->size;
    if (n <= 0 || n > 8 || cap < n) {
        if (err) _snprintf(err, errcap, "变量大小不支持（%u 字节）", (unsigned)s->size);
        return -1;
    }
    if (xcpsym_parse_number(text, s->is_signed, s->is_float, &fv, &iv) != 0) {
        if (err) _snprintf(err, errcap, "数值格式无效");
        return -1;
    }
    memset(out, 0, (size_t)n);
    if (s->is_float && n == 4) {
        float f = (float)fv;
        memcpy(out, &f, 4);
        return 4;
    }
    if (s->is_float && n == 8) {
        memcpy(out, &fv, 8);
        return 8;
    }
    xcpsym_store_le(out, iv, n);
    return n;
}
