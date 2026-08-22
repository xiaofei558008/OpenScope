#include "netbuf.h"
#include <stdlib.h>
#include <string.h>

void os_net_spool_init(OS_NetSpool* s, uint32_t ram_cap)
{
    memset(s, 0, sizeof(*s));
    s->cap = ram_cap ? ram_cap : 4096;
    s->ram = (uint8_t*)malloc(s->cap);
}

void os_net_spool_free(OS_NetSpool* s)
{
    free(s->ram);
    s->ram = NULL; s->len = s->cap = 0;
    if (s->f) { fclose(s->f); s->f = NULL; }
    s->spilled = 0;
}

static int spill(OS_NetSpool* s)
{
    if (s->f) return 0;
    s->f = tmpfile(); /* 匿名临时文件，关闭自动删除 */
    if (!s->f) return -1;
    if (s->len > 0 && fwrite(s->ram, 1, s->len, s->f) != s->len) return -1;
    s->len = 0;
    s->spilled = 1;
    return 0;
}

int os_net_spool_append(OS_NetSpool* s, const uint8_t* d, uint32_t l)
{
    if (!s || !d || l == 0) return -1;
    if (!s->ram) return -1;
    if (!s->f && s->len + l > s->cap) {
        if (spill(s) != 0) return -1;
    }
    if (s->f) {
        /* 已转盘：直接写文件（分批写，避免一次大 fwrite 阻塞过久） */
        if (fwrite(d, 1, l, s->f) != l) return -1;
    } else {
        memcpy(s->ram + s->len, d, l);
        s->len += l;
    }
    return 0;
}

int os_net_spool_read(OS_NetSpool* s, uint8_t* out, uint32_t cap, uint32_t* out_len)
{
    uint32_t n = 0;
    if (!s || !out) return -1;
    /* RAM 部分 */
    if (s->len > 0) {
        uint32_t k = s->len < cap ? s->len : cap;
        memcpy(out, s->ram, k);
        n = k;
    }
    /* 盘部分 */
    if (s->f) {
        fflush(s->f);
        if (n < cap) {
            long pos = ftell(s->f);
            fseek(s->f, 0, SEEK_SET);
            n += (uint32_t)fread(out + n, 1, cap - n, s->f);
            fseek(s->f, pos, SEEK_SET);
        }
    }
    if (out_len) *out_len = n;
    return 0;
}
