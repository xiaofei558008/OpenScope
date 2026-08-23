#include "netws.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OS_WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

/* ---------------- SHA1 ---------------- */
typedef struct { uint32_t s[5]; uint64_t nbits; uint8_t buf[64]; int buflen; } SHA1;

static uint32_t rol32(uint32_t x, int n) { return (x << n) | (x >> (32 - n)); }

static void sha1_init(SHA1* c)
{
    c->s[0]=0x67452301u; c->s[1]=0xEFCDAB89u; c->s[2]=0x98BADCFEu;
    c->s[3]=0x10325476u; c->s[4]=0xC3D2E1F0u; c->nbits=0; c->buflen=0;
}

static void sha1_block(SHA1* c, const uint8_t* p)
{
    uint32_t w[80], a, b, cc, dd, ee, f, k, t;
    int i;
    for (i = 0; i < 16; i++)
        w[i] = ((uint32_t)p[i*4]<<24)|((uint32_t)p[i*4+1]<<16)|((uint32_t)p[i*4+2]<<8)|(uint32_t)p[i*4+3];
    for (i = 16; i < 80; i++) w[i] = rol32(w[i-3]^w[i-8]^w[i-14]^w[i-16], 1);
    a=c->s[0]; b=c->s[1]; cc=c->s[2]; dd=c->s[3]; ee=c->s[4];
    for (i = 0; i < 80; i++) {
        if (i < 20)      { f = (b & cc) | ((~b) & dd);         k = 0x5A827999u; }
        else if (i < 40) { f = b ^ cc ^ dd;                    k = 0x6ED9EBA1u; }
        else if (i < 60) { f = (b & cc) | (b & dd) | (cc & dd); k = 0x8F1BBCDCu; }
        else             { f = b ^ cc ^ dd;                    k = 0xCA62C1D6u; }
        t = rol32(a,5) + f + ee + k + w[i];
        ee = dd; dd = cc; cc = rol32(b,30); b = a; a = t;
    }
    c->s[0]+=a; c->s[1]+=b; c->s[2]+=cc; c->s[3]+=dd; c->s[4]+=ee;
}

static void sha1_update(SHA1* c, const uint8_t* d, int n)
{
    c->nbits += (uint64_t)n * 8;
    while (n > 0) {
        int take = 64 - c->buflen; if (take > n) take = n;
        memcpy(c->buf + c->buflen, d, take); c->buflen += take; d += take; n -= take;
        if (c->buflen == 64) { sha1_block(c, c->buf); c->buflen = 0; }
    }
}

static void sha1_final(SHA1* c, uint8_t out[20])
{
    uint64_t bits = c->nbits; uint8_t pad = 0x80, z = 0, lb[8]; int i;
    sha1_update(c, &pad, 1);
    while (c->buflen != 56) sha1_update(c, &z, 1);
    for (i = 0; i < 8; i++) lb[i] = (uint8_t)(bits >> (56 - i*8));
    sha1_update(c, lb, 8);
    for (i = 0; i < 5; i++) {
        out[i*4]  = (uint8_t)(c->s[i]>>24); out[i*4+1] = (uint8_t)(c->s[i]>>16);
        out[i*4+2]= (uint8_t)(c->s[i]>>8);  out[i*4+3] = (uint8_t)c->s[i];
    }
}

/* ---------------- base64 ---------------- */
static const char b64tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static int b64_encode(const uint8_t* in, int n, char* out, int cap)
{
    int i, o = 0;
    for (i = 0; i + 2 < n; i += 3) {
        uint32_t v = ((uint32_t)in[i]<<16)|((uint32_t)in[i+1]<<8)|in[i+2];
        if (o + 4 >= cap) return -1;
        out[o++]=b64tab[(v>>18)&63]; out[o++]=b64tab[(v>>12)&63]; out[o++]=b64tab[(v>>6)&63]; out[o++]=b64tab[v&63];
    }
    if (i < n) {
        uint32_t v = (uint32_t)in[i] << 16; int rem = n - i;
        if (rem == 2) v |= (uint32_t)in[i+1] << 8;
        if (o + 4 >= cap) return -1;
        out[o++]=b64tab[(v>>18)&63]; out[o++]=b64tab[(v>>12)&63];
        out[o++]=(rem==2)?b64tab[(v>>6)&63]:'='; out[o++]='=';
    }
    out[o] = 0;
    return o;
}

int os_ws_accept_key(const char* key, char* out, int cap)
{
    SHA1 c; uint8_t dig[20]; char comb[256]; int n;
    n = _snprintf(comb, sizeof(comb), "%s%s", key ? key : "", OS_WS_GUID);
    sha1_init(&c); sha1_update(&c, (const uint8_t*)comb, n); sha1_final(&c, dig);
    return b64_encode(dig, 20, out, cap);
}

/* ---------------- 帧编解码 ---------------- */
int os_ws_frame_encode(uint8_t* out, int cap, int fin, int opcode,
                       const uint8_t* payload, uint64_t len, int mask, const uint8_t mask_key[4])
{
    uint8_t* p = out; int ext = (len < 126) ? 0 : (len < 65536) ? 2 : 8; int i;
    if (cap < 2 + ext + (mask ? 4 : 0) + (int)len) return -1;
    p[0] = (uint8_t)((fin ? 0x80 : 0) | (opcode & 0x0F));
    if (ext == 0) { p[1] = (uint8_t)((mask ? 0x80 : 0) | len); p += 2; }
    else if (ext == 2) { p[1] = (uint8_t)((mask ? 0x80 : 0) | 126); p[2]=(uint8_t)(len>>8); p[3]=(uint8_t)len; p += 4; }
    else { p[1] = (uint8_t)((mask ? 0x80 : 0) | 127); for (i = 0; i < 8; i++) p[2+i] = (uint8_t)(len >> (56 - i*8)); p += 10; }
    if (mask) {
        uint64_t k;
        p[0]=mask_key[0]; p[1]=mask_key[1]; p[2]=mask_key[2]; p[3]=mask_key[3]; p += 4;
        for (k = 0; k < len; k++) p[k] = payload[k] ^ mask_key[k & 3];
        p += len;
    } else if (len) {
        memcpy(p, payload, (size_t)len); p += len;
    }
    return (int)(p - out);
}

int os_ws_frame_decode(const uint8_t* in, int len, int* fin, int* opcode,
                       uint8_t* payload, int payload_cap, uint64_t* plen, int* consumed)
{
    uint64_t l; int hdr = 2, mask = 0, i; uint8_t mkey[4] = {0,0,0,0};
    if (len < 2) return 1;
    *fin = (in[0] & 0x80) ? 1 : 0;
    *opcode = in[0] & 0x0F;
    mask = (in[1] & 0x80) ? 1 : 0;
    l = in[1] & 0x7F;
    if (l == 126) { if (len < 4) return 1; l = ((uint64_t)in[2]<<8)|in[3]; hdr = 4; }
    else if (l == 127) { if (len < 10) return 1; l = 0; for (i=0;i<8;i++) l=(l<<8)|in[2+i]; hdr = 10; }
    if (mask) { if (len < hdr + 4) return 1; memcpy(mkey, in + hdr, 4); hdr += 4; }
    if (len < hdr + (int)l) return 1;
    if ((int)l > payload_cap) return -1;
    if (mask) for (i = 0; i < (int)l; i++) payload[i] = in[hdr+i] ^ mkey[i & 3];
    else if (l) memcpy(payload, in + hdr, (size_t)l);
    *plen = l; *consumed = hdr + (int)l;
    return 0;
}

/* ---------------- TCP ---------------- */

/* 发送锁：样本广播线程与客户端会话线程可能同时向同一连接发帧，逐帧互斥防交错 */
struct OS_WSConn { SOCKET s; int is_client; CRITICAL_SECTION send_cs; };

static int g_ws_inited;
static void ws_ensure_init(void)
{
    if (!g_ws_inited) { WSADATA wd; WSAStartup(MAKEWORD(2,2), &wd); g_ws_inited = 1; }
}

int os_ws_listen(const char* bind_ip, int port)
{
    SOCKET s; struct sockaddr_in a;
    ws_ensure_init();
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return -1;
    { int one = 1; setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&one, sizeof(one)); }
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET; a.sin_port = htons((u_short)port);
    a.sin_addr.s_addr = bind_ip && bind_ip[0] ? inet_addr(bind_ip) : INADDR_ANY;
    if (bind(s, (struct sockaddr*)&a, sizeof(a)) != 0 || listen(s, SOMAXCONN) != 0) {
        closesocket(s); return -1;
    }
    return (int)s;
}

void os_ws_close_listen(int lsock) { if (lsock >= 0) closesocket((SOCKET)lsock); }

static int recv_all(SOCKET s, uint8_t* buf, int n)
{
    int got = 0;
    while (got < n) { int r = recv(s, (char*)buf + got, n - got, 0); if (r <= 0) return -1; got += r; }
    return 0;
}

static int send_all(SOCKET s, const uint8_t* buf, int n)
{
    int sent = 0;
    while (sent < n) { int r = send(s, (const char*)buf + sent, n - sent, 0); if (r <= 0) return -1; sent += r; }
    return 0;
}

static int http_header(const char* req, const char* name, char* out, int cap)
{
    const char* p = req; int nlen = (int)strlen(name);
    while (p && *p) {
        const char* e = strstr(p, "\r\n"); if (!e) break;
        if (_strnicmp(p, name, nlen) == 0 && p[nlen] == ':') {
            const char* v = p + nlen + 1; int vl = (int)(e - v), i = 0;
            while (i < vl && (v[i]==' '||v[i]=='\t')) i++;
            if (vl - i >= cap) return -1;
            memcpy(out, v + i, vl - i); out[vl - i] = 0;
            return 0;
        }
        p = e + 2;
    }
    return -1;
}

static int ws_server_handshake(SOCKET s)
{
    char req[2048]; int n = 0, found = 0; char key[128], acc[64], resp[512];
    while (n < (int)sizeof(req) - 1) {
        int r = recv(s, req + n, 1, 0);
        if (r <= 0) return -1;
        n += r; req[n] = 0;
        if (n >= 4 && strstr(req, "\r\n\r\n")) { found = 1; break; }
    }
    if (!found) return -1;
    if (http_header(req, "Sec-WebSocket-Key", key, sizeof(key)) != 0) return -1;
    if (os_ws_accept_key(key, acc, sizeof(acc)) < 0) return -1;
    _snprintf(resp, sizeof(resp),
        "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n", acc);
    return send_all(s, (const uint8_t*)resp, (int)strlen(resp));
}

static int ws_client_handshake(SOCKET s, const char* host)
{
    char req[512], resp[2048], acc[64], exp[64], *p;
    int n = 0, found = 0;
    static const char* key = "dGhlIHNhbXBsZSBub25jZQ==";
    _snprintf(req, sizeof(req),
        "GET / HTTP/1.1\r\nHost: %s\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: %s\r\nSec-WebSocket-Version: 13\r\n\r\n", host, key);
    if (send_all(s, (const uint8_t*)req, (int)strlen(req)) != 0) return -1;
    while (n < (int)sizeof(resp) - 1) {
        int r = recv(s, resp + n, 1, 0);
        if (r <= 0) return -1;
        n += r; resp[n] = 0;
        if (n >= 4 && strstr(resp, "\r\n\r\n")) { found = 1; break; }
    }
    if (!found) return -1;
    if (http_header(resp, "Sec-WebSocket-Accept", acc, sizeof(acc)) != 0) return -1;
    if (os_ws_accept_key(key, exp, sizeof(exp)) < 0) return -1;
    p = acc + strlen(acc); while (p > acc && (p[-1]==' '||p[-1]=='\t'||p[-1]=='\r'||p[-1]=='\n')) *--p = 0;
    return strcmp(acc, exp) == 0 ? 0 : -1;
}

OS_WSConn* os_ws_accept(int lsock)
{
    SOCKET c = accept((SOCKET)lsock, NULL, NULL);
    OS_WSConn* w;
    if (c == INVALID_SOCKET) return NULL;
    if (ws_server_handshake(c) != 0) { closesocket(c); return NULL; }
    w = (OS_WSConn*)calloc(1, sizeof(OS_WSConn));
    if (!w) { closesocket(c); return NULL; }
    w->s = c; w->is_client = 0;
    InitializeCriticalSection(&w->send_cs);
    return w;
}

OS_WSConn* os_ws_connect(const char* host, int port)
{
    SOCKET s; struct sockaddr_in a; OS_WSConn* w;
    ws_ensure_init();
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return NULL;
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET; a.sin_port = htons((u_short)port);
    a.sin_addr.s_addr = inet_addr(host);
    if (a.sin_addr.s_addr == INADDR_NONE) {
        struct hostent* he = gethostbyname(host);
        if (!he) { closesocket(s); return NULL; }
        memcpy(&a.sin_addr, he->h_addr_list[0], he->h_length);
    }
    if (connect(s, (struct sockaddr*)&a, sizeof(a)) != 0) { closesocket(s); return NULL; }
    if (ws_client_handshake(s, host) != 0) { closesocket(s); return NULL; }
    w = (OS_WSConn*)calloc(1, sizeof(OS_WSConn));
    if (!w) { closesocket(s); return NULL; }
    w->s = s; w->is_client = 1;
    InitializeCriticalSection(&w->send_cs);
    return w;
}

void os_ws_shutdown(OS_WSConn* c)
{
    if (!c || c->s == INVALID_SOCKET) return;
    shutdown(c->s, SD_BOTH); /* 唤醒对端 recv，由会话线程随后 os_ws_close 释放 */
}

void os_ws_close(OS_WSConn* c)
{
    if (!c) return;
    DeleteCriticalSection(&c->send_cs);
    if (c->s != INVALID_SOCKET) closesocket(c->s);
    free(c);
}

int os_ws_send_bin(OS_WSConn* c, const uint8_t* data, uint32_t len)
{
    static const uint8_t mkey[4] = {0x12,0x34,0x56,0x78};
    uint8_t* f = (uint8_t*)malloc(14 + (size_t)len + 8);
    int n, r;
    if (!f) return -1;
    n = os_ws_frame_encode(f, 14 + (int)len + 8, 1, OS_WS_OP_BIN, data, len, c->is_client, mkey);
    if (n < 0) { free(f); return -1; }
    EnterCriticalSection(&c->send_cs);
    r = send_all(c->s, f, n);
    LeaveCriticalSection(&c->send_cs);
    free(f);
    return r == 0 ? 0 : -1;
}

int os_ws_send_text(OS_WSConn* c, const char* s)
{
    return os_ws_send_bin(c, (const uint8_t*)s, (uint32_t)strlen(s));
}

int os_ws_recv(OS_WSConn* c, uint8_t* buf, int cap, int* opcode)
{
    uint8_t hdr[10]; uint64_t l; int op, mask, hlen = 2, i; uint8_t mkey[4] = {0,0,0,0};
    if (recv_all(c->s, hdr, 2) != 0) return 0;
    op = hdr[0] & 0x0F;
    mask = (hdr[1] & 0x80) ? 1 : 0;
    l = hdr[1] & 0x7F;
    if (l == 126) { if (recv_all(c->s, hdr + 2, 2) != 0) return -1; l = ((uint64_t)hdr[2]<<8)|hdr[3]; hlen = 4; }
    else if (l == 127) { if (recv_all(c->s, hdr + 2, 8) != 0) return -1; l = 0; for (i=0;i<8;i++) l=(l<<8)|hdr[2+i]; hlen = 10; }
    if (mask) { if (recv_all(c->s, mkey, 4) != 0) return -1; hlen += 4; }
    if (l > (uint64_t)cap) return -1;
    if (l && recv_all(c->s, buf, (int)l) != 0) return -1;
    if (mask) for (i = 0; i < (int)l; i++) buf[i] ^= mkey[i & 3];
    if (op == OS_WS_OP_PING) {
        static const uint8_t pkey[4] = {0x12,0x34,0x56,0x78};
        uint8_t f[128]; int h = os_ws_frame_encode(f, sizeof(f), 1, OS_WS_OP_PONG, buf, (uint32_t)l, c->is_client, pkey);
        if (h > 0) send_all(c->s, f, h);
        return os_ws_recv(c, buf, cap, opcode);
    }
    if (op == OS_WS_OP_CLOSE) return 0;
    if (opcode) *opcode = op;
    return (int)l;
}
