/* das_util.c — 见 das_util.h（纯 C，无平台依赖） */
#include "das_util.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int das_range_is_protected(uint64_t addr, uint32_t size)
{
    uint64_t end = addr + (size ? size : 1); /* 半开区间 */
    if (addr < 0x90000000ull && end > 0x80000000ull) return 1;
    if (addr < 0xB0000000ull && end > 0xA0000000ull) return 1;
    return 0;
}

int das_addr_is_sfr(uint64_t addr) { return addr >= 0xF0000000ull; }

int das_split_chunks(uint64_t addr, uint32_t size, uint32_t max_bytes,
                     uint64_t* out_addr, uint32_t* out_len, int max_chunks)
{
    int n = 0;
    if (!max_bytes) return -1;
    while (size) {
        uint32_t l = size < max_bytes ? size : max_bytes;
        if (n >= max_chunks) return -1;
        if (out_addr) out_addr[n] = addr;
        if (out_len) out_len[n] = l;
        n++; addr += l; size -= l;
    }
    return n;
}

void das_align4(uint64_t addr, uint32_t size, uint64_t* a_addr, uint32_t* a_size)
{
    uint64_t s = addr & ~3ull, e = (addr + size + 3) & ~3ull;
    if (a_addr) *a_addr = s;
    if (a_size) *a_size = (uint32_t)(e - s);
}

const char* das_state_text(uint32_t st, int* halted)
{
    int h = -1; const char* t = "\xE2\x80\x94"; /* — */
    switch (st) {
    case 1: h = 0; t = "RUNNING"; break;          /* MCD_CORE_STATE_RUNNING */
    case 2: case 3: h = 1; t = "STOP"; break;     /* HALTED / DEBUG */
    default: break;
    }
    if (halted) *halted = h;
    return t;
}

int das_parse_core_index(const char* d)
{
    const char* p;
    if (!d) return 0;
    for (p = d; *p; p++) if (isdigit((unsigned char)*p)) {
        /* 仅在 "core"/"cpu"/"tc" 前缀或纯数字时接受 */
        size_t pre = (size_t)(p - d); char low[8] = {0}; size_t i;
        if (pre > 6) return 0;
        for (i = 0; i < pre; i++) low[i] = (char)tolower((unsigned char)d[i]);
        if (pre == 0 || !strcmp(low, "core") || !strcmp(low, "cpu") || !strcmp(low, "tc")) {
            int v = atoi(p); return (v >= 0 && v < 8) ? v : 0;
        }
        return 0;
    }
    return 0;
}
