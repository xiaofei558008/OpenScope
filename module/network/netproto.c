#include "netproto.h"
#include <stdlib.h>
#include <string.h>

int os_net_put_uvarint(uint8_t* out, int cap, uint64_t v)
{
    int n = 0;
    while (1) {
        uint8_t b = (uint8_t)(v & 0x7F);
        v >>= 7;
        if (v) b |= 0x80;
        if (n >= cap) return -1;
        out[n++] = b;
        if (!(b & 0x80)) break;
    }
    return n;
}

int os_net_get_uvarint(const uint8_t* in, int len, uint64_t* v, int* consumed)
{
    uint64_t r = 0;
    int shift = 0, i = 0;
    while (i < len && i < 10) {
        uint8_t b = in[i++];
        r |= (uint64_t)(b & 0x7F) << shift;
        if (!(b & 0x80)) {
            if (v) *v = r;
            if (consumed) *consumed = i;
            return 0;
        }
        shift += 7;
    }
    return -1; /* 超长或数据不足 */
}

static void put_le32(uint8_t* p, uint32_t v)
{
    p[0] = (uint8_t)(v); p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
}

static uint32_t get_le32(const uint8_t* p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

int os_net_frame_encode(uint8_t* out, int cap, uint8_t type, uint8_t flags,
                        uint32_t seq, const uint8_t* payload, uint32_t len)
{
    if (cap < OS_NET_FRAME_HDR + (int)len) return -1;
    put_le32(out, OS_NET_MAGIC);
    out[4] = type;
    out[5] = flags;
    put_le32(out + 6, seq);
    put_le32(out + 10, len);
    if (len && payload) memcpy(out + OS_NET_FRAME_HDR, payload, len);
    return OS_NET_FRAME_HDR + (int)len;
}

int os_net_frame_decode(const uint8_t* in, int len, OS_NetFrame* f)
{
    if (len < OS_NET_FRAME_HDR) return 0;
    if (get_le32(in) != OS_NET_MAGIC) return -1;
    f->type = in[4];
    f->flags = in[5];
    f->seq = get_le32(in + 6);
    f->len = get_le32(in + 10);
    if (len < OS_NET_FRAME_HDR + (int)f->len) return 0; /* payload 未到齐 */
    f->payload = (f->len > 0) ? (in + OS_NET_FRAME_HDR) : NULL;
    return OS_NET_FRAME_HDR + (int)f->len;
}

int os_net_chunk_split(const uint8_t* data, uint32_t len, uint32_t chunk_size, void* ud,
                       int (*cb)(void* ud, uint32_t idx, uint32_t total, const uint8_t* d, uint32_t l))
{
    uint32_t idx = 0, off = 0, total;
    if (!data || len == 0 || chunk_size == 0 || !cb) return 0;
    total = (len + chunk_size - 1) / chunk_size;
    while (off < len) {
        uint32_t l = len - off;
        if (l > chunk_size) l = chunk_size;
        cb(ud, idx, total, data + off, l);
        off += l;
        idx++;
    }
    return (int)total;
}

void os_net_accum_init(OS_NetAccum* a)
{
    memset(a, 0, sizeof(*a));
}

void os_net_accum_free(OS_NetAccum* a)
{
    free(a->buf);
    a->buf = NULL; a->len = a->cap = 0;
}

int os_net_accum_append(OS_NetAccum* a, const uint8_t* d, uint32_t l)
{
    uint32_t need = a->len + l;
    if (need > a->cap) {
        uint32_t nc = a->cap ? a->cap : 64;
        uint8_t* nb;
        while (nc < need) nc <<= 1;
        nb = (uint8_t*)realloc(a->buf, nc);
        if (!nb) return -1;
        a->buf = nb;
        a->cap = nc;
    }
    if (l && d) memcpy(a->buf + a->len, d, l);
    a->len = need;
    return 0;
}

/* CHUNK 流：首块 payload = [4B 总字节数][数据]，后续块 = 纯数据。总字节数含 4B 头本身。
 * 发送端把整块数据 os_net_chunk_split 后逐块经本函数编码为 CHUNK 帧 payload；
 * 接收端每块 os_net_chunk_stream_feed 喂入累计器，到齐后复制到 out 并复位累计器。 */
int os_net_chunk_stream_encode(uint32_t idx, uint32_t total_len, const uint8_t* d, uint32_t l,
                               uint8_t* out, int cap)
{
    if (idx == 0) {
        if (cap < (int)l + 4) return -1;
        put_le32(out, total_len);
        memcpy(out + 4, d, l);
        return (int)l + 4;
    }
    if (cap < (int)l) return -1;
    memcpy(out, d, l);
    return (int)l;
}

int os_net_chunk_stream_feed(OS_NetAccum* a, const uint8_t* d, int len,
                             uint8_t* out, int cap)
{
    uint32_t total;
    if (!a || !d || len <= 0) return -1;
    if (os_net_accum_append(a, d, (uint32_t)len) != 0) return -1;
    if (a->len < 4) return 0; /* 头还没到齐 */
    total = get_le32(a->buf);
    if (total < 4 || total > (uint32_t)cap + 4) { os_net_accum_free(a); return -1; }
    if (a->len < total) return 0; /* 数据块未到齐，继续等 */
    memcpy(out, a->buf + 4, total - 4); /* 剥掉 4 字节总长头 */
    os_net_accum_free(a);
    return (int)(total - 4);
}
