/*
 * .map 解析：兼容 GNU ld (-Map)、Keil/ARM armlink、IAR 的常见符号表行。
 *
 * 识别三类行：
 *   A) GNU ld      : 0x20000000  g_counter
 *   B) Keil armlink: g_counter   0x20000000  Data  4  main.o(.data)
 *   C) 地址在前    : 0x20000000  4  Data  RW  g_counter
 * 其余行（段汇总、内存配置、对象文件路径等）忽略。
 */
#include "mapfile.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_MAX_SYMS  65536
#define MAP_HASH_CAP  131072 /* 2 的幂，装载因子 <= 0.5 */

typedef struct MapBuild {
    OS_MapSym* syms;
    int        count;
    int        cap;
    int*       hash;   /* 槽位存 syms 下标 +1；0=空 */
} MapBuild;

static uint32_t map_hash_name(const char* s)
{
    uint32_t h = 2166136261u;
    while (s && *s) {
        h ^= (uint8_t)*s++;
        h *= 16777619u;
    }
    return h;
}

static int map_build_init(MapBuild* b)
{
    memset(b, 0, sizeof(*b));
    b->hash = (int*)calloc(MAP_HASH_CAP, sizeof(int));
    if (!b->hash) return -1;
    return 0;
}

static void map_build_free(MapBuild* b)
{
    free(b->syms);
    free(b->hash);
    memset(b, 0, sizeof(*b));
}

static int map_name_bad(const char* s)
{
    size_t i, n;
    static const char* bad[] = {
        "PROVIDE", "ALIGN", "ASSERT", "KEEP", "SORT", "FILL", "ENTRY",
        "LOAD", "ORIGIN", "LENGTH", "SIZEOF", "ADDR", "DEFINED"
    };
    if (!s || !s[0]) return 1;
    if (s[0] == '.' || s[0] == '*' || s[0] == '#' || s[0] == '(' || s[0] == '-')
        return 1;
    n = strlen(s);
    if (n >= 2 && (strcmp(s + n - 2, ".o") == 0)) return 1;
    if (n >= 4 && (strcmp(s + n - 4, ".obj") == 0)) return 1;
    if (n >= 2 && (strcmp(s + n - 2, ".a") == 0)) return 1;
    if (n >= 4 && (strcmp(s + n - 4, ".lib") == 0)) return 1;
    if (strchr(s, '/') || strchr(s, '\\') || strchr(s, '=') ||
        strchr(s, '<') || strchr(s, '>') || strchr(s, '"') || strchr(s, '\'') ||
        strchr(s, '#'))
        return 1;
    for (i = 0; i < n; i++) {
        unsigned char c = (unsigned char)s[i];
        if (!(isalnum(c) || c == '_' || c == '$' || c == '.' || c == ':' ||
              c == '@' || c == '?'))
            return 1;
    }
    for (i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
        if (_stricmp(s, bad[i]) == 0) return 1;
    }
    return 0;
}

static int map_hex_token(const char* s, uint64_t* out)
{
    char buf[32];
    int i, j = 0;
    unsigned long long v;
    if (!s || !(s[0] == '0' && (s[1] == 'x' || s[1] == 'X')))
        return 0;
    /* Keil/IAR 常用 0x2000'0248 / 0x2000_0248 作为千位分隔 */
    for (i = 2; s[i]; i++) {
        unsigned char c = (unsigned char)s[i];
        if (c == '\'' || c == '_') continue;
        if (!isxdigit(c)) return 0;
        if (j >= (int)sizeof(buf) - 1) return 0;
        buf[j++] = (char)c;
    }
    if (j == 0) return 0;
    buf[j] = 0;
    v = _strtoui64(buf, NULL, 16);
    if (out) *out = (uint64_t)v;
    return 1;
}

static int map_dec_token(const char* s, uint32_t* out)
{
    char buf[32];
    int i, j = 0;
    unsigned long v;
    if (!s || !isdigit((unsigned char)s[0])) return 0;
    for (i = 0; s[i]; i++) {
        unsigned char c = (unsigned char)s[i];
        if (c == '\'' || c == '_') continue;
        if (!isdigit(c)) return 0;
        if (j >= (int)sizeof(buf) - 1) return 0;
        buf[j++] = (char)c;
    }
    buf[j] = 0;
    v = strtoul(buf, NULL, 10);
    if (v > 0x1000000u) return 0;
    if (out) *out = (uint32_t)v;
    return 1;
}

static int map_size_token(const char* s, uint32_t* out)
{
    uint64_t h;
    if (map_hex_token(s, &h)) {
        if (h > 0x1000000u) return 0;
        if (out) *out = (uint32_t)h;
        return 1;
    }
    return map_dec_token(s, out);
}

static int map_append(MapBuild* b, int slot, const char* name,
                      uint64_t addr, uint32_t size)
{
    if (b->count >= b->cap) {
        int ncap = b->cap ? b->cap * 2 : 1024;
        OS_MapSym* ns;
        if (ncap > MAP_MAX_SYMS) ncap = MAP_MAX_SYMS;
        if (ncap <= b->cap) return 0;
        ns = (OS_MapSym*)realloc(b->syms, (size_t)ncap * sizeof(OS_MapSym));
        if (!ns) return 0;
        b->syms = ns;
        b->cap = ncap;
    }
    memset(&b->syms[b->count], 0, sizeof(b->syms[b->count]));
    _snprintf(b->syms[b->count].name, sizeof(b->syms[b->count].name), "%s", name);
    b->syms[b->count].address = addr;
    b->syms[b->count].size = size;
    b->hash[slot] = b->count + 1;
    b->count++;
    return 1;
}

static int map_add(MapBuild* b, const char* name, uint64_t addr, uint32_t size)
{
    uint32_t h;
    int slot, i;
    if (b->count >= MAP_MAX_SYMS) return 0;
    if (map_name_bad(name)) return 0;
    h = map_hash_name(name) & (MAP_HASH_CAP - 1);
    slot = (int)h;
    for (i = 0; i < MAP_HASH_CAP; i++) {
        int v = b->hash[slot];
        if (v == 0)
            return map_append(b, slot, name, addr, size);
        if (strcmp(b->syms[v - 1].name, name) == 0) {
            /* 同名重复：保留首次地址；若首次大小未知而本次已知则补上 */
            if (b->syms[v - 1].size == 0 && size) b->syms[v - 1].size = size;
            return 0;
        }
        slot = (slot + 1) & (MAP_HASH_CAP - 1);
    }
    return 0;
}

static int map_split(char* line, char* tok[], int max)
{
    int n = 0;
    char* p = line;
    while (*p && n < max) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        tok[n++] = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) *p++ = 0;
    }
    return n;
}

static int map_cmp_name(const void* a, const void* b)
{
    const OS_MapSym* x = (const OS_MapSym*)a;
    const OS_MapSym* y = (const OS_MapSym*)b;
    return _stricmp(x->name, y->name);
}

int os_map_load(const char* utf8_path, OS_MapSym** out, char* err, int errcap)
{
    FILE* f;
    MapBuild b;
    char line[2048];
    int lineno = 0;
    int in_memcfg = 0;
    if (err && errcap > 0) err[0] = 0;
    if (!utf8_path || !utf8_path[0] || !out) {
        if (err) _snprintf(err, errcap, "路径为空");
        return -1;
    }
    *out = NULL;
    f = fopen(utf8_path, "rb");
    if (!f) {
        if (err) _snprintf(err, errcap, "无法打开文件");
        return -1;
    }
    if (map_build_init(&b) != 0) {
        fclose(f);
        if (err) _snprintf(err, errcap, "内存不足");
        return -1;
    }
    while (fgets(line, sizeof(line), f)) {
        char* tok[12];
        int nt;
        lineno++;
        {
            size_t n = strlen(line);
            while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r' ||
                             line[n - 1] == '\0'))
                line[--n] = 0;
            if (lineno == 1 && (unsigned char)line[0] == 0xEF &&
                (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF)
                memmove(line, line + 3, strlen(line + 3) + 1);
        }
        /* GNU ld 的 Memory Configuration 段列出 FLASH/RAM 区域，不是变量符号 */
        if (strstr(line, "Memory Configuration")) {
            in_memcfg = 1;
            continue;
        }
        if (strstr(line, "Linker script and memory map")) {
            in_memcfg = 0;
            continue;
        }
        if (in_memcfg) continue;
        nt = map_split(line, tok, 12);
        if (nt < 2) continue;
        {
            uint64_t addr = 0;
            uint32_t size = 0;
            if (map_hex_token(tok[0], &addr)) {
                /* A) 地址在前：0xADDR  name  [size] */
                if (!map_hex_token(tok[1], NULL) && !map_name_bad(tok[1])) {
                    if (nt >= 3) map_size_token(tok[2], &size);
                    map_add(&b, tok[1], addr, size);
                    continue;
                }
                /* C) Keil 某些列表：0xADDR  size  Type  RW  name */
                if (nt >= 5 && map_size_token(tok[1], &size) &&
                    !map_name_bad(tok[4])) {
                    map_add(&b, tok[4], addr, size);
                    continue;
                }
                continue;
            }
        }
        {
            uint64_t addr = 0;
            uint32_t size = 0;
            int i;
            if (map_hex_token(tok[1], &addr) && !map_name_bad(tok[0])) {
                /* B) Keil/IAR：name  0xADDR  Type  size ... */
                for (i = 2; i < nt; i++) {
                    if (map_size_token(tok[i], &size)) break;
                }
                map_add(&b, tok[0], addr, size);
            }
        }
    }
    fclose(f);
    if (b.count > 1)
        qsort(b.syms, (size_t)b.count, sizeof(OS_MapSym), map_cmp_name);
    free(b.hash);
    *out = b.syms;
    return b.count;
}

void os_map_free(OS_MapSym* syms)
{
    free(syms);
}
