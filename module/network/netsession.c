#include "netsession.h"
#include "netproto.h"
#include <string.h>

static int put_str(uint8_t* p, int cap, const char* s)
{
    int n = (int)strlen(s), w, total = 0;
    w = os_net_put_uvarint(p, cap, (uint64_t)n); if (w < 0) return -1;
    p += w; cap -= w; total += w;
    if (cap < n) return -1;
    if (n) memcpy(p, s, n);
    return total + n;
}

static int get_str(const uint8_t* p, int len, char* out, int out_cap, int* consumed)
{
    uint64_t n; int c;
    if (os_net_get_uvarint(p, len, &n, &c) != 0) return -1;
    if ((int)n >= out_cap) return -1;
    if (len < c + (int)n) return -1;
    if (n) memcpy(out, p + c, (size_t)n);
    out[n] = 0;
    *consumed = c + (int)n;
    return 0;
}

int os_net_encode_varlist(const OS_NetVar* v, int n, uint8_t* out, int cap)
{
    uint8_t* p = out; int i, w, rem = cap;
    w = os_net_put_uvarint(p, rem, (uint64_t)n); if (w < 0) return -1; p += w; rem -= w;
    for (i = 0; i < n; i++) {
        w = put_str(p, rem, v[i].name); if (w < 0) return -1; p += w; rem -= w;
        w = os_net_put_uvarint(p, rem, v[i].addr); if (w < 0) return -1; p += w; rem -= w;
        w = os_net_put_uvarint(p, rem, v[i].size); if (w < 0) return -1; p += w; rem -= w;
    }
    return (int)(p - out);
}

int os_net_decode_varlist(const uint8_t* in, int len, OS_NetVar* v, int max)
{
    uint64_t n, a, sz; int c, i, rem = len; const uint8_t* p = in;
    if (os_net_get_uvarint(p, rem, &n, &c) != 0) return -1; p += c; rem -= c;
    if ((int)n > max) return -1;
    for (i = 0; i < (int)n; i++) {
        if (get_str(p, rem, v[i].name, OS_NET_NAME_MAX, &c) != 0) return -1; p += c; rem -= c;
        if (os_net_get_uvarint(p, rem, &a, &c) != 0) return -1; p += c; rem -= c;
        if (os_net_get_uvarint(p, rem, &sz, &c) != 0) return -1; p += c; rem -= c;
        v[i].addr = a; v[i].size = (uint32_t)sz;
    }
    return (int)n;
}

int os_net_encode_names(const char* const* names, int n, uint8_t* out, int cap)
{
    uint8_t* p = out; int i, w, rem = cap;
    w = os_net_put_uvarint(p, rem, (uint64_t)n); if (w < 0) return -1; p += w; rem -= w;
    for (i = 0; i < n; i++) {
        w = put_str(p, rem, names[i]); if (w < 0) return -1; p += w; rem -= w;
    }
    return (int)(p - out);
}

int os_net_decode_names(const uint8_t* in, int len, char names[][OS_NET_NAME_MAX], int max)
{
    uint64_t n; int c, i, rem = len; const uint8_t* p = in;
    if (os_net_get_uvarint(p, rem, &n, &c) != 0) return -1; p += c; rem -= c;
    if ((int)n > max) return -1;
    for (i = 0; i < (int)n; i++) {
        if (get_str(p, rem, names[i], OS_NET_NAME_MAX, &c) != 0) return -1; p += c; rem -= c;
    }
    return (int)n;
}

int os_net_encode_write(const char* name, const char* text, uint8_t* out, int cap)
{
    uint8_t* p = out; int w, rem = cap;
    w = put_str(p, rem, name); if (w < 0) return -1; p += w; rem -= w;
    w = put_str(p, rem, text); if (w < 0) return -1; p += w; rem -= w;
    return (int)(p - out);
}

int os_net_decode_write(const uint8_t* in, int len, char* name, int name_cap, char* text, int text_cap)
{
    int c, rem = len; const uint8_t* p = in;
    if (get_str(p, rem, name, name_cap, &c) != 0) return -1; p += c; rem -= c;
    if (get_str(p, rem, text, text_cap, &c) != 0) return -1;
    return 0;
}

int os_net_encode_ack(int code, const char* msg, uint8_t* out, int cap)
{
    uint8_t* p = out; int w, rem = cap;
    uint64_t z = ((uint64_t)(code < 0 ? ((-(int64_t)code) << 1) - 1 : (int64_t)code << 1));
    w = os_net_put_uvarint(p, rem, z); if (w < 0) return -1; p += w; rem -= w;
    w = put_str(p, rem, msg ? msg : ""); if (w < 0) return -1; p += w; rem -= w;
    return (int)(p - out);
}

int os_net_decode_ack(const uint8_t* in, int len, int* code, char* msg, int msg_cap)
{
    uint64_t z; int c, rem = len; const uint8_t* p = in;
    if (os_net_get_uvarint(p, rem, &z, &c) != 0) return -1; p += c; rem -= c;
    /* zigzag 还原：偶→z/2，奇→-(z/2)-1（编码侧 (-code<<1)-1 的逆运算） */
    if (code) *code = (int)((z & 1) ? -(int64_t)(z >> 1) - 1 : (int64_t)(z >> 1));
    if (get_str(p, rem, msg, msg_cap, &c) != 0) return -1;
    return 0;
}
