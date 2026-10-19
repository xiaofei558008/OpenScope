/* XCP-on-TCP 预留实现。
 * 帧： [u16 LE len][u16 LE ctr][payload]（ASAM Ethernet 风格简化）。
 * opts="host:port"，默认 127.0.0.1:5555。
 * 无服务端时 open 失败；便于接入 MCU LwIP / PC 从机测试口。
 */
#include "xcp_transport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

typedef struct TcpCtx {
    SOCKET sock;
    uint16_t ctr;
    char host[64];
    int port;
} TcpCtx;

static int g_wsa;

static void wsa_ensure(void)
{
    WSADATA w;
    if (!g_wsa) {
        if (WSAStartup(MAKEWORD(2, 2), &w) == 0) g_wsa = 1;
    }
}

static int tcp_scan(char names[][64], int cap)
{
    if (cap < 1) return 0;
    _snprintf(names[0], 64, "127.0.0.1:5555");
    return 1;
}

static int parse_host_port(const char* opts, char* host, int host_cap, int* port)
{
    const char* colon;
    if (!opts || !opts[0]) {
        _snprintf(host, host_cap, "127.0.0.1");
        *port = 5555;
        return 0;
    }
    colon = strrchr(opts, ':');
    if (colon) {
        size_t n = (size_t)(colon - opts);
        if (n >= (size_t)host_cap) n = (size_t)host_cap - 1;
        memcpy(host, opts, n);
        host[n] = 0;
        *port = atoi(colon + 1);
        if (*port <= 0) *port = 5555;
    } else {
        _snprintf(host, host_cap, "%s", opts);
        *port = 5555;
    }
    return 0;
}

static int tcp_open(void** out, int index, const char* opts)
{
    TcpCtx* t;
    char host[64], names[4][64];
    int port = 5555;
    struct sockaddr_in addr;
    SOCKET s;
    (void)index;
    if (!out) return -1;
    *out = NULL;
    wsa_ensure();
    if (!g_wsa) return -1;
    if (opts && opts[0])
        parse_host_port(opts, host, sizeof(host), &port);
    else {
        if (tcp_scan(names, 4) < 1) return -1;
        parse_host_port(names[0], host, sizeof(host), &port);
    }
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return -1;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((u_short)port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
        closesocket(s);
        return -1;
    }
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        closesocket(s);
        return -1;
    }
    {
        DWORD tv = 500;
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
        setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
    }
    t = (TcpCtx*)calloc(1, sizeof(TcpCtx));
    if (!t) { closesocket(s); return -1; }
    t->sock = s;
    t->port = port;
    _snprintf(t->host, sizeof(t->host), "%s", host);
    *out = t;
    return 0;
}

static void tcp_close(void* ctx)
{
    TcpCtx* t = (TcpCtx*)ctx;
    if (!t) return;
    if (t->sock != INVALID_SOCKET) closesocket(t->sock);
    free(t);
}

static int tcp_send_all(SOCKET s, const uint8_t* p, int n)
{
    int off = 0;
    while (off < n) {
        int wr = send(s, (const char*)(p + off), n - off, 0);
        if (wr <= 0) return -1;
        off += wr;
    }
    return 0;
}

static int tcp_recv_exact(SOCKET s, uint8_t* p, int n, int timeout_ms)
{
    int off = 0;
    DWORD t0 = GetTickCount();
    while (off < n) {
        int rd;
        if ((int)(GetTickCount() - t0) > timeout_ms) return -1;
        rd = recv(s, (char*)(p + off), n - off, 0);
        if (rd == 0) return -1;
        if (rd < 0) {
            int e = WSAGetLastError();
            if (e == WSAETIMEDOUT || e == WSAEWOULDBLOCK) {
                Sleep(1);
                continue;
            }
            return -1;
        }
        off += rd;
    }
    return 0;
}

static int tcp_send(void* ctx, const uint8_t* data, int len)
{
    TcpCtx* t = (TcpCtx*)ctx;
    uint8_t hdr[4];
    if (!t || !data || len <= 0 || len > 0xFFFF) return -1;
    hdr[0] = (uint8_t)(len & 0xFF);
    hdr[1] = (uint8_t)((len >> 8) & 0xFF);
    hdr[2] = (uint8_t)(t->ctr & 0xFF);
    hdr[3] = (uint8_t)((t->ctr >> 8) & 0xFF);
    t->ctr++;
    if (tcp_send_all(t->sock, hdr, 4) != 0) return -1;
    if (tcp_send_all(t->sock, data, len) != 0) return -1;
    return len;
}

static int tcp_recv(void* ctx, uint8_t* data, int cap, int timeout_ms)
{
    TcpCtx* t = (TcpCtx*)ctx;
    uint8_t hdr[4];
    int len;
    if (!t || !data || cap <= 0) return -1;
    if (timeout_ms <= 0) timeout_ms = 500;
    if (tcp_recv_exact(t->sock, hdr, 4, timeout_ms) != 0) return -1;
    len = (int)hdr[0] | ((int)hdr[1] << 8);
    if (len <= 0 || len > cap) return -1;
    if (tcp_recv_exact(t->sock, data, len, timeout_ms) != 0) return -1;
    return len;
}

static void tcp_bind(void* ctx, XcpIo* io)
{
    if (!io) return;
    io->ctx = ctx;
    io->send = tcp_send;
    io->recv = tcp_recv;
}

static const XcpTransport g_tcp = {
    "tcp",
    "TCP",
    tcp_scan,
    tcp_open,
    tcp_close,
    tcp_bind
};

void xcp_tcp_register(void)
{
    xcp_transport_register(&g_tcp);
}
