#include "netcodec.h"
#include "netproto.h"
#include <string.h>
#include <windows.h>
#include <stdlib.h>

static uint64_t f2u(double v) { uint64_t u; memcpy(&u, &v, 8); return u; }
static double  u2f(uint64_t u) { double v; memcpy(&v, &u, 8); return v; }
static int64_t zigzag(int64_t v) { return (v << 1) ^ (v >> 63); }
static int64_t unzigzag(uint64_t v) { return (int64_t)(v >> 1) ^ -(int64_t)(v & 1); }

/* 前导/尾随零字节计数（0..8） */
static int lead0(uint64_t v) { int n = 0; while (n < 8 && !(v & 0xFF00000000000000ULL)) { n++; v <<= 8; } return n; }
static int tail0(uint64_t v) { int n = 0; while (n < 8 && !(v & 0xFFULL)) { n++; v >>= 8; } return n; }

static int put_u64le(uint8_t* out, int cap, uint64_t v)
{
    int i;
    if (cap < 8) return -1;
    for (i = 0; i < 8; i++) out[i] = (uint8_t)(v >> (i * 8));
    return 8;
}

static uint64_t get_u64le(const uint8_t* p)
{
    int i; uint64_t v = 0;
    for (i = 0; i < 8; i++) v |= (uint64_t)p[i] << (i * 8);
    return v;
}

int os_net_codec_encode(const OS_NetSample* s, int n, uint8_t* out, int cap)
{
    uint8_t* p = out;
    int i, w, rem = cap;
    if (n <= 0 || !s || !out) return -1;
    w = os_net_put_uvarint(p, rem, (uint64_t)n); if (w < 0) return -1; p += w; rem -= w;
    /* 首个样本：完整时间戳 + 原始 double */
    w = os_net_put_uvarint(p, rem, (uint64_t)s[0].ts_us); if (w < 0) return -1; p += w; rem -= w;
    w = put_u64le(p, rem, f2u(s[0].value)); if (w < 0) return -1; p += w; rem -= w;
    for (i = 1; i < n; i++) {
        int64_t dts = s[i].ts_us - s[i - 1].ts_us;
        uint64_t x = f2u(s[i].value) ^ f2u(s[i - 1].value);
        w = os_net_put_uvarint(p, rem, (uint64_t)zigzag(dts)); if (w < 0) return -1; p += w; rem -= w;
        if (x == 0) {
            if (rem < 1) return -1;
            *p++ = 0x00; rem -= 1;
        } else {
            int lead = lead0(x), trail = tail0(x), sig = 8 - lead - trail;
            uint64_t m = x >> (trail * 8);
            int k;
            if (rem < 1 + sig) return -1;
            *p++ = (uint8_t)((lead << 4) | (sig - 1)); rem -= 1;
            for (k = 0; k < sig; k++) { *p++ = (uint8_t)(m >> (k * 8)); rem -= 1; }
        }
    }
    return (int)(p - out);
}

int os_net_codec_decode(const uint8_t* in, int len, OS_NetSample* s, int max)
{
    const uint8_t* p = in;
    uint64_t nv;
    int i, c, n, rem = len;
    if (!in || !s || max <= 0) return -1;
    if (os_net_get_uvarint(p, rem, &nv, &c) != 0) return -1; p += c; rem -= c;
    n = (int)nv;
    if (n < 0 || n > max) return -1;
    if (os_net_get_uvarint(p, rem, &nv, &c) != 0) return -1; p += c; rem -= c;
    s[0].ts_us = (int64_t)nv;
    if (rem < 8) return -1;
    s[0].value = u2f(get_u64le(p)); p += 8; rem -= 8;
    for (i = 1; i < n; i++) {
        uint64_t dz;
        uint8_t ctrl;
        if (os_net_get_uvarint(p, rem, &dz, &c) != 0) return -1; p += c; rem -= c;
        s[i].ts_us = s[i - 1].ts_us + unzigzag(dz);
        if (rem < 1) return -1;
        ctrl = *p++; rem -= 1;
        if (ctrl == 0x00) {
            s[i].value = s[i - 1].value;
        } else {
            int lead = ctrl >> 4, sig = (ctrl & 0x0F) + 1, trail = 8 - lead - sig, k;
            uint64_t m = 0, x;
            if (rem < sig) return -1;
            for (k = 0; k < sig; k++) m |= (uint64_t)p[k] << (k * 8);
            p += sig; rem -= sig;
            x = m << (trail * 8);
            s[i].value = u2f(f2u(s[i - 1].value) ^ x);
        }
    }
    return n;
}

int os_net_codec_encode_flat(const OS_NetSample* s, int n, uint8_t* out, int cap)
{
    uint8_t* p = out; int i, w, rem = cap;
    w = os_net_put_uvarint(p, rem, (uint64_t)n); if (w < 0) return -1; p += w; rem -= w;
    for (i = 0; i < n; i++) {
        w = os_net_put_uvarint(p, rem, (uint64_t)(uint32_t)s[i].var_id); if (w < 0) return -1; p += w; rem -= w;
        w = put_u64le(p, rem, (uint64_t)s[i].ts_us); if (w < 0) return -1; p += w; rem -= w;
        w = put_u64le(p, rem, f2u(s[i].value)); if (w < 0) return -1; p += w; rem -= w;
    }
    return (int)(p - out);
}

int os_net_codec_decode_flat(const uint8_t* in, int len, OS_NetSample* s, int max)
{
    uint64_t nv; int c, i, n, rem = len; const uint8_t* p = in;
    if (os_net_get_uvarint(p, rem, &nv, &c) != 0) return -1; p += c; rem -= c;
    n = (int)nv;
    if (n < 0 || n > max) return -1;
    for (i = 0; i < n; i++) {
        if (os_net_get_uvarint(p, rem, &nv, &c) != 0) return -1; p += c; rem -= c;
        s[i].var_id = (int)nv;
        if (rem < 8) return -1;
        s[i].ts_us = (int64_t)get_u64le(p); p += 8; rem -= 8;
        if (rem < 8) return -1;
        s[i].value = u2f(get_u64le(p)); p += 8; rem -= 8;
    }
    return n;
}

/* ---------------- 多核并行压缩 ---------------- */

typedef struct EncJob {
    const OS_NetSample* s;
    int n;
    uint8_t* out;
    int cap;
    int len;
} EncJob;

static DWORD WINAPI enc_worker(LPVOID p)
{
    EncJob* j = (EncJob*)p;
    j->len = os_net_codec_encode(j->s, j->n, j->out, j->cap);
    return 0;
}

int os_net_codec_encode_parallel(const OS_NetSample* s, int n, int nthreads, uint8_t* out, int cap)
{
    EncJob jobs[16]; HANDLE hs[16];
    uint8_t* scratch; int chunk, i, off, nw, total;
    if (n <= 0 || !s || !out) return -1;
    if (nthreads > 16) nthreads = 16;
    /* 少样本时退化为单块，但仍必须输出并行包格式（decode_parallel 只认此格式）：
     * [1][n][块长][块数据]。此前直接返回串行格式，decode_parallel 会错读。 */
    if (nthreads <= 1 || n <= nthreads) {
        uint8_t tmp[512];
        int sl = os_net_codec_encode(s, n, tmp, sizeof(tmp));
        int w1, w2, w3;
        if (sl < 0) return -1;
        w1 = os_net_put_uvarint(out, cap, 1);
        if (w1 < 0) return -1;
        w2 = os_net_put_uvarint(out + w1, cap - w1, (uint64_t)n);
        if (w2 < 0) return -1;
        w3 = os_net_put_uvarint(out + w1 + w2, cap - w1 - w2, (uint64_t)sl);
        if (w3 < 0 || w1 + w2 + w3 + sl > cap) return -1;
        memcpy(out + w1 + w2 + w3, tmp, sl);
        return w1 + w2 + w3 + sl;
    }
    chunk = (n + nthreads - 1) / nthreads;
    scratch = (uint8_t*)malloc((size_t)n * 16 + 64);
    if (!scratch) return -1;
    nw = 0;
    for (i = 0; i < nthreads; i++) {
        int a = i * chunk, b = a + chunk; if (b > n) b = n;
        if (a >= n) break;
        jobs[nw].s = s + a; jobs[nw].n = b - a;
        jobs[nw].out = scratch + (size_t)a * 16; jobs[nw].cap = (b - a) * 16 + 8; jobs[nw].len = -1;
        hs[nw] = CreateThread(NULL, 0, enc_worker, &jobs[nw], 0, NULL);
        if (hs[nw]) nw++;
    }
    for (i = 0; i < nw; i++) WaitForSingleObject(hs[i], INFINITE);
    /* 组装：nthreads(实际块数) + n + 每块[长度 + 数据] */
    off = 0;
    { int w = os_net_put_uvarint(out + off, cap - off, (uint64_t)nw); if (w < 0) { free(scratch); return -1; } off += w; }
    { int w = os_net_put_uvarint(out + off, cap - off, (uint64_t)n); if (w < 0) { free(scratch); return -1; } off += w; }
    total = off;
    for (i = 0; i < nw; i++) {
        int w = os_net_put_uvarint(out + off, cap - off, (uint64_t)jobs[i].len); if (w < 0) { free(scratch); return -1; } off += w;
        if (off + jobs[i].len > cap) { free(scratch); return -1; }
        if (jobs[i].len > 0) memcpy(out + off, jobs[i].out, jobs[i].len);
        off += jobs[i].len;
    }
    total = off;
    free(scratch);
    return total;
}

int os_net_codec_decode_parallel(const uint8_t* in, int len, OS_NetSample* s, int max)
{
    uint64_t nw, n; int c, off = 0, i, k;
    if (!in || !s) return -1;
    if (os_net_get_uvarint(in + off, len - off, &nw, &c) != 0) return -1; off += c;
    if (os_net_get_uvarint(in + off, len - off, &n, &c) != 0) return -1; off += c;
    if ((int)n > max) return -1;
    k = 0;
    for (i = 0; i < (int)nw; i++) {
        uint64_t cl; int got;
        if (os_net_get_uvarint(in + off, len - off, &cl, &c) != 0) return -1; off += c;
        if (off + (int)cl > len) return -1;
        if (cl > 0) {
            got = os_net_codec_decode(in + off, (int)cl, s + k, max - k);
            if (got < 0) return -1;
            k += got;
        }
        off += (int)cl;
    }
    return k;
}
