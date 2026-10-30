/* XCP on Ethernet 传输（TCP/UDP），ASAM XCP Part3 Ethernet。
 * 帧：LEN(WORD LE) CTR(WORD LE) 包体；主机 CTR 每发一包 +1，从机 CTR 独立，用于丢包检测。 */
#include "xcp_tl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/select.h>
#endif

#define ETH_HDR 4
#define ETH_MAX_PKT 0xFFFF
#define ETH_SLICE_MS 100  /* 单次 select 上限，保证 shutdown 标志及时被看到 */

typedef struct EthTl {
    XcpTl base;
    xcp_sock_t sock;
    int is_udp;
    xcp_mutex_t tx_lock;
    uint16_t tx_ctr;
    xcp_atomic_t stop;
    int rx_ctr_valid;
    uint16_t rx_ctr_expect;
    /* 接收缓冲（TCP 流 / UDP 数据报），只由 RX 线程访问 */
    uint8_t* rb;
    int rb_cap, rb_len, rb_off;
    uint8_t* tb; /* 发送拼帧缓冲，tx_lock 保护 */
} EthTl;

static int sock_errno(void)
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

static int is_transient(int e)
{
#ifdef _WIN32
    return e == WSAEWOULDBLOCK || e == WSAEINTR || e == WSAETIMEDOUT;
#else
    return e == EAGAIN || e == EWOULDBLOCK || e == EINTR;
#endif
}

static int udp_ignorable(int e)
{
    /* UDP 上 ICMP 端口不可达会以 ECONNREFUSED/WSAECONNRESET 报到下一次 recv，视作无数据 */
#ifdef _WIN32
    return e == WSAECONNRESET || e == WSAECONNREFUSED;
#else
    return e == ECONNREFUSED;
#endif
}

static void check_ctr(EthTl* t, uint16_t ctr)
{
    if (t->rx_ctr_valid && ctr != t->rx_ctr_expect)
        xcp_atomic_add(&t->base.stats.rx_ctr_gaps, 1);
    t->rx_ctr_valid = 1;
    t->rx_ctr_expect = (uint16_t)(ctr + 1u);
}

static int eth_send(XcpTl* tl, const uint8_t* pkt, int len)
{
    EthTl* t = (EthTl*)tl;
    int off = 0, total, rc = 0;
    if (!pkt || len <= 0 || len > ETH_MAX_PKT) return XCP_TL_ERROR;
    if (xcp_atomic_load(&t->stop)) return XCP_TL_CLOSED;
    xcp_mutex_lock(&t->tx_lock);
    t->tb[0] = (uint8_t)len;
    t->tb[1] = (uint8_t)(len >> 8);
    t->tb[2] = (uint8_t)t->tx_ctr;
    t->tb[3] = (uint8_t)(t->tx_ctr >> 8);
    memcpy(t->tb + ETH_HDR, pkt, (size_t)len);
    total = len + ETH_HDR;
    t->tx_ctr++;
    while (off < total) {
        int wr = (int)send(t->sock, (const char*)t->tb + off, total - off, 0);
        if (wr <= 0) {
            if (wr < 0 && is_transient(sock_errno())) continue;
            rc = XCP_TL_CLOSED;
            break;
        }
        off += wr;
        if (t->is_udp) break; /* 数据报整体发送 */
    }
    xcp_mutex_unlock(&t->tx_lock);
    if (rc == 0) xcp_atomic_add(&t->base.stats.tx_packets, 1);
    return rc;
}

/* 从 rb[rb_off..rb_len) 取一个完整包；1 成功，0 不完整，<0 错误 */
static int take_packet(EthTl* t, uint8_t* pkt, int cap, int* out_len)
{
    int avail = t->rb_len - t->rb_off;
    const uint8_t* p = t->rb + t->rb_off;
    int len;
    if (avail < ETH_HDR) return 0;
    len = (int)p[0] | ((int)p[1] << 8);
    if (len == 0) {
        xcp_atomic_add(&t->base.stats.rx_bad_frames, 1);
        return XCP_TL_ERROR;
    }
    if (avail < ETH_HDR + len) {
        if (t->is_udp) { /* 截断的数据报：丢弃剩余部分 */
            xcp_atomic_add(&t->base.stats.rx_bad_frames, 1);
            t->rb_off = t->rb_len;
        }
        return 0;
    }
    check_ctr(t, (uint16_t)(p[2] | (p[3] << 8)));
    t->rb_off += ETH_HDR + len;
    if (len > cap) return XCP_TL_TOOBIG;
    memcpy(pkt, p + ETH_HDR, (size_t)len);
    *out_len = len;
    return 1;
}

static int eth_recv(XcpTl* tl, uint8_t* pkt, int cap, int timeout_ms)
{
    EthTl* t = (EthTl*)tl;
    uint64_t deadline = xcp_now_ms() + (uint64_t)(timeout_ms > 0 ? timeout_ms : 0);
    for (;;) {
        int n = 0, r, rd;
        r = take_packet(t, pkt, cap, &n);
        if (r == 1) { xcp_atomic_add(&t->base.stats.rx_packets, 1); return n; }
        if (r == XCP_TL_TOOBIG) return XCP_TL_TOOBIG;
        if (r < 0) {
            if (!t->is_udp) return XCP_TL_ERROR; /* TCP 流已失步，必须断开 */
            t->rb_off = t->rb_len;               /* UDP：丢弃本数据报剩余部分 */
            continue;
        }
        if (xcp_atomic_load(&t->stop)) return XCP_TL_CLOSED;
        /* 压缩缓冲 */
        if (t->rb_off > 0) {
            memmove(t->rb, t->rb + t->rb_off, (size_t)(t->rb_len - t->rb_off));
            t->rb_len -= t->rb_off;
            t->rb_off = 0;
        }
        {
            uint64_t now = xcp_now_ms();
            int wait = now >= deadline ? 0 : (int)(deadline - now);
            if (wait > ETH_SLICE_MS) wait = ETH_SLICE_MS;
            r = xcp_sock_wait_readable(t->sock, wait);
            if (r < 0) return xcp_atomic_load(&t->stop) ? XCP_TL_CLOSED : XCP_TL_ERROR;
            if (r == 0) {
                if (xcp_now_ms() >= deadline) return XCP_TL_TIMEOUT;
                continue;
            }
        }
        if (t->is_udp) {
            if (t->rb_len != 0) t->rb_len = 0; /* 上个数据报残余已丢 */
            rd = (int)recv(t->sock, (char*)t->rb, t->rb_cap, 0);
            if (rd < 0) {
                int e = sock_errno();
                if (is_transient(e) || udp_ignorable(e)) continue;
                return xcp_atomic_load(&t->stop) ? XCP_TL_CLOSED : XCP_TL_ERROR;
            }
            if (rd == 0 && xcp_atomic_load(&t->stop)) return XCP_TL_CLOSED;
            t->rb_len = rd;
            t->rb_off = 0;
        } else {
            rd = (int)recv(t->sock, (char*)t->rb + t->rb_len, t->rb_cap - t->rb_len, 0);
            if (rd == 0) return XCP_TL_CLOSED;
            if (rd < 0) {
                if (is_transient(sock_errno())) continue;
                return xcp_atomic_load(&t->stop) ? XCP_TL_CLOSED : XCP_TL_ERROR;
            }
            t->rb_len += rd;
        }
    }
}

static void eth_shutdown(XcpTl* tl)
{
    EthTl* t = (EthTl*)tl;
    xcp_atomic_store(&t->stop, 1);
    if (!t->is_udp) xcp_sock_shutdown(t->sock);
}

static void eth_destroy(XcpTl* tl)
{
    EthTl* t = (EthTl*)tl;
    if (!t) return;
    xcp_sock_close(t->sock);
    xcp_mutex_destroy(&t->tx_lock);
    free(t->rb);
    free(t->tb);
    free(t);
    xcp_net_cleanup();
}

static const XcpTlVtbl k_tcp = { "tcp", eth_send, eth_recv, eth_shutdown, eth_destroy };
static const XcpTlVtbl k_udp = { "udp", eth_send, eth_recv, eth_shutdown, eth_destroy };

static void set_nonblock(xcp_sock_t s, int on)
{
#ifdef _WIN32
    u_long v = (u_long)on;
    ioctlsocket(s, FIONBIO, &v);
#else
    int f = fcntl(s, F_GETFL, 0);
    fcntl(s, F_SETFL, on ? (f | O_NONBLOCK) : (f & ~O_NONBLOCK));
#endif
}

static int connect_timeout(xcp_sock_t s, const struct sockaddr* a, int alen, int ms)
{
    fd_set wf, ef;
    struct timeval tv;
    int r, err = 0;
    socklen_t el = (socklen_t)sizeof(err);
    set_nonblock(s, 1);
    r = connect(s, a, alen);
    if (r != 0) {
        int e = sock_errno();
#ifdef _WIN32
        if (e != WSAEWOULDBLOCK) return -1;
#else
        if (e != EINPROGRESS) return -1;
#endif
        FD_ZERO(&wf); FD_SET(s, &wf);
        FD_ZERO(&ef); FD_SET(s, &ef);
        tv.tv_sec = ms / 1000;
        tv.tv_usec = (ms % 1000) * 1000;
        r = select((int)(s + 1), NULL, &wf, &ef, &tv);
        if (r <= 0) return -1;
        if (getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&err, &el) != 0 || err != 0) return -1;
    }
    set_nonblock(s, 0);
    return 0;
}

static XcpTl* eth_open(const char* host, int port, int udp, int conn_ms, int* err_out)
{
    struct addrinfo hints, *ai = NULL;
    char ports[16];
    EthTl* t;
    xcp_sock_t s;
    if (err_out) *err_out = 0;
    if (xcp_net_init() != 0) { if (err_out) *err_out = -1; return NULL; }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = udp ? SOCK_DGRAM : SOCK_STREAM;
    snprintf(ports, sizeof(ports), "%d", port > 0 ? port : 5555);
    if (getaddrinfo(host && host[0] ? host : "127.0.0.1", ports, &hints, &ai) != 0 || !ai) {
        if (err_out) *err_out = -2;
        xcp_net_cleanup();
        return NULL;
    }
    s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (s == XCP_INVALID_SOCK) goto fail_ai;
    if (udp) {
        int rcvbuf = 1 << 20;
        setsockopt(s, SOL_SOCKET, SO_RCVBUF, (const char*)&rcvbuf, sizeof(rcvbuf));
        if (connect(s, ai->ai_addr, (int)ai->ai_addrlen) != 0) goto fail_sock;
    } else {
        int one = 1;
        if (connect_timeout(s, ai->ai_addr, (int)ai->ai_addrlen, conn_ms > 0 ? conn_ms : 2000) != 0)
            goto fail_sock;
        setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char*)&one, sizeof(one));
    }
    freeaddrinfo(ai);
    t = (EthTl*)calloc(1, sizeof(EthTl));
    if (!t) { xcp_sock_close(s); xcp_net_cleanup(); if (err_out) *err_out = -4; return NULL; }
    t->base.v = udp ? &k_udp : &k_tcp;
    t->base.max_packet = udp ? (1472 - ETH_HDR) : ETH_MAX_PKT;
    t->sock = s;
    t->is_udp = udp;
    t->rb_cap = udp ? 65536 : 2 * (ETH_MAX_PKT + ETH_HDR);
    t->rb = (uint8_t*)malloc((size_t)t->rb_cap);
    t->tb = (uint8_t*)malloc(ETH_MAX_PKT + ETH_HDR);
    xcp_mutex_init(&t->tx_lock);
    if (!t->rb || !t->tb) { eth_destroy(&t->base); if (err_out) *err_out = -4; return NULL; }
    return &t->base;
fail_sock:
    xcp_sock_close(s);
fail_ai:
    freeaddrinfo(ai);
    xcp_net_cleanup();
    if (err_out) *err_out = -3;
    return NULL;
}

XcpTl* xcp_tl_tcp_open(const char* host, int port, int connect_timeout_ms, int* err_out)
{
    return eth_open(host, port, 0, connect_timeout_ms, err_out);
}

XcpTl* xcp_tl_udp_open(const char* host, int port, int* err_out)
{
    return eth_open(host, port, 1, 0, err_out);
}
