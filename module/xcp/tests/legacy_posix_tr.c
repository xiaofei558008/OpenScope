/* 仅 Linux 单测：替代 Win32 专用的 xcp_serial.c / xcp_tcp.c。
 *  - serial：扫描为空；
 *  - tcp：旧 xcp_tcp.c 行为的 POSIX 版（[u16 len][u16 ctr] 帧、单线程发一收一），
 *    供“迁移前”的旧 xcp.c 对比运行；迁移后的 xcp.c 对 tcp 改走 xcp_eth.c，只用到这里的 scan。 */
#define _POSIX_C_SOURCE 200809L
#include "../xcp_transport.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

static int ser_scan(char names[][64], int cap) { (void)names; (void)cap; return 0; }
static int ser_open(void** out, int i, const char* o) { (void)out; (void)i; (void)o; return -1; }
static void ser_close(void* c) { (void)c; }
static void ser_bind(void* c, XcpIo* io) { (void)c; if (io) { io->ctx = NULL; io->send = NULL; io->recv = NULL; } }
static const XcpTransport g_ser = { "serial", "串口", ser_scan, ser_open, ser_close, ser_bind };
void xcp_serial_register(void) { xcp_transport_register(&g_ser); }

typedef struct { int fd; uint16_t ctr; } T;
static int t_scan(char names[][64], int cap) { if (cap < 1) return 0; snprintf(names[0], 64, "127.0.0.1:5555"); return 1; }
static int t_open(void** out, int idx, const char* opts)
{
    char host[64] = "127.0.0.1"; int port = 5555; const char* c; struct sockaddr_in a; T* t; int fd, one = 1;
    (void)idx;
    if (opts && (c = strrchr(opts, ':'))) { size_t n = (size_t)(c - opts); if (n > 63) n = 63; memcpy(host, opts, n); host[n] = 0; port = atoi(c + 1); }
    fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&a, 0, sizeof(a)); a.sin_family = AF_INET; a.sin_port = htons((uint16_t)port); inet_pton(AF_INET, host, &a.sin_addr);
    if (connect(fd, (struct sockaddr*)&a, sizeof(a)) != 0) { close(fd); return -1; }
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
    t = (T*)calloc(1, sizeof(T)); t->fd = fd; *out = t; return 0;
}
static void t_close(void* c) { T* t = (T*)c; if (!t) return; close(t->fd); free(t); }
static int t_send(void* c, const uint8_t* d, int n)
{
    T* t = (T*)c; uint8_t b[70000];
    b[0] = (uint8_t)n; b[1] = (uint8_t)(n >> 8); b[2] = (uint8_t)t->ctr; b[3] = (uint8_t)(t->ctr >> 8); t->ctr++;
    memcpy(b + 4, d, (size_t)n);
    return send(t->fd, b, (size_t)n + 4, MSG_NOSIGNAL) == n + 4 ? n : -1;
}
static int rd_exact(int fd, uint8_t* p, int n, int ms)
{
    int off = 0;
    while (off < n) {
        fd_set rf; struct timeval tv; int r;
        FD_ZERO(&rf); FD_SET(fd, &rf); tv.tv_sec = ms / 1000; tv.tv_usec = (ms % 1000) * 1000;
        if (select(fd + 1, &rf, NULL, NULL, &tv) <= 0) return -1;
        r = (int)recv(fd, p + off, (size_t)(n - off), 0);
        if (r <= 0) return -1;
        off += r;
    }
    return 0;
}
static int t_recv(void* c, uint8_t* d, int cap, int ms)
{
    T* t = (T*)c; uint8_t h[4]; int len;
    if (ms <= 0) ms = 500;
    if (rd_exact(t->fd, h, 4, ms)) return -1;
    len = h[0] | (h[1] << 8);
    if (len <= 0 || len > cap) return -1;
    return rd_exact(t->fd, d, len, ms) ? -1 : len;
}
static void t_bind(void* c, XcpIo* io) { io->ctx = c; io->send = t_send; io->recv = t_recv; }
static const XcpTransport g_tcp = { "tcp", "TCP", t_scan, t_open, t_close, t_bind };
void xcp_tcp_register(void) { xcp_transport_register(&g_tcp); }
