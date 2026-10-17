#include "xcp_eth.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

typedef struct XcpEthClient {
    SOCKET   s;
    uint16_t tx_ctr;
    uint16_t rx_ctr;
} XcpEthClient;

struct XcpEthServer {
    SOCKET    listen_s;
    XcpSlave* slave;
    HANDLE    thread;
    volatile LONG stop;
    int       port;
};

static void wsa_once(void)
{
    static LONG once;
    if (InterlockedCompareExchange(&once, 1, 0) == 0) {
        WSADATA w;
        WSAStartup(MAKEWORD(2, 2), &w);
    }
}

static int send_all(SOCKET s, const uint8_t* p, int n)
{
    int off = 0;
    while (off < n) {
        int r = send(s, (const char*)p + off, n - off, 0);
        if (r <= 0) return -1;
        off += r;
    }
    return 0;
}

static int recv_all(SOCKET s, uint8_t* p, int n)
{
    int off = 0;
    while (off < n) {
        int r = recv(s, (char*)p + off, n - off, 0);
        if (r <= 0) return -1;
        off += r;
    }
    return 0;
}

static int write_frame(SOCKET s, uint16_t* ctr, const uint8_t* payload, int len)
{
    uint8_t hdr[4];
    if (len < 1 || len > 0xFFFF) return -1;
    xcp_wr16(hdr, (uint16_t)len);
    xcp_wr16(hdr + 2, *ctr);
    *ctr = (uint16_t)(*ctr + 1);
    if (send_all(s, hdr, 4) != 0) return -1;
    return send_all(s, payload, len);
}

static int read_frame(SOCKET s, uint8_t* dst, int cap, uint16_t* ctr)
{
    uint8_t hdr[4];
    int len;
    if (recv_all(s, hdr, 4) != 0) return -1;
    len = xcp_rd16(hdr);
    if (ctr) *ctr = xcp_rd16(hdr + 2);
    if (len < 1 || len > cap) return -1;
    if (recv_all(s, dst, len) != 0) return -1;
    return len;
}

static int client_send(void* ctx, const uint8_t* data, int len)
{
    XcpEthClient* c = (XcpEthClient*)ctx;
    if (write_frame(c->s, &c->tx_ctr, data, len) != 0) return -1;
    return len;
}

static int client_recv(void* ctx, uint8_t* data, int cap, int timeout_ms)
{
    XcpEthClient* c = (XcpEthClient*)ctx;
    DWORD t = timeout_ms < 0 ? 0 : (DWORD)timeout_ms;
    setsockopt(c->s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&t, sizeof(t));
    return read_frame(c->s, data, cap, &c->rx_ctr);
}

XcpEthClient* xcp_eth_connect(const char* host, int port)
{
    XcpEthClient* c;
    struct sockaddr_in a;
    wsa_once();
    if (!host || port <= 0 || port > 65535) return NULL;
    c = (XcpEthClient*)calloc(1, sizeof(*c));
    if (!c) return NULL;
    c->s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (c->s == INVALID_SOCKET) { free(c); return NULL; }
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_port = htons((u_short)port);
    if (inet_pton(AF_INET, host, &a.sin_addr) != 1) {
        closesocket(c->s);
        free(c);
        return NULL;
    }
    if (connect(c->s, (struct sockaddr*)&a, sizeof(a)) != 0) {
        closesocket(c->s);
        free(c);
        return NULL;
    }
    return c;
}

void xcp_eth_close(XcpEthClient* c)
{
    if (!c) return;
    closesocket(c->s);
    free(c);
}

void xcp_eth_io(XcpEthClient* c, XcpIo* io)
{
    if (!io) return;
    io->ctx = c;
    io->send = client_send;
    io->recv = client_recv;
}

static DWORD WINAPI serve_thread(LPVOID p)
{
    XcpEthServer* sv = (XcpEthServer*)p;
    SOCKET cli = accept(sv->listen_s, NULL, NULL);
    uint16_t tx = 0, rx = 0;
    if (cli == INVALID_SOCKET) return 0;
    while (!sv->stop) {
        uint8_t cmd[XCP_MAX_CTO];
        uint8_t rsp[XCP_MAX_CTO];
        int n, rn;
        n = read_frame(cli, cmd, (int)sizeof(cmd), &rx);
        if (n < 1) break;
        rn = xcp_slave_input(sv->slave, cmd, n, rsp, (int)sizeof(rsp));
        if (rn > 0 && write_frame(cli, &tx, rsp, rn) != 0) break;
    }
    closesocket(cli);
    return 0;
}

XcpEthServer* xcp_eth_serve(XcpSlave* slave, int port)
{
    XcpEthServer* sv;
    struct sockaddr_in a;
    int alen;
    if (!slave) return NULL;
    wsa_once();
    sv = (XcpEthServer*)calloc(1, sizeof(*sv));
    if (!sv) return NULL;
    sv->slave = slave;
    sv->listen_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sv->listen_s == INVALID_SOCKET) { free(sv); return NULL; }
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.sin_port = htons((u_short)(port < 0 ? 0 : port));
    if (bind(sv->listen_s, (struct sockaddr*)&a, sizeof(a)) != 0 ||
        listen(sv->listen_s, 1) != 0) {
        closesocket(sv->listen_s);
        free(sv);
        return NULL;
    }
    alen = (int)sizeof(a);
    if (getsockname(sv->listen_s, (struct sockaddr*)&a, &alen) != 0) {
        closesocket(sv->listen_s);
        free(sv);
        return NULL;
    }
    sv->port = ntohs(a.sin_port);
    sv->thread = CreateThread(NULL, 0, serve_thread, sv, 0, NULL);
    if (!sv->thread) {
        closesocket(sv->listen_s);
        free(sv);
        return NULL;
    }
    return sv;
}

int xcp_eth_port(XcpEthServer* s)
{
    return s ? s->port : 0;
}

void xcp_eth_serve_stop(XcpEthServer* s)
{
    if (!s) return;
    s->stop = 1;
    closesocket(s->listen_s);
    if (s->thread) {
        WaitForSingleObject(s->thread, 2000);
        CloseHandle(s->thread);
    }
    free(s);
}
