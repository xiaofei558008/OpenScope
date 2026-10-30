#ifndef XCP_TL_H
#define XCP_TL_H
/* 会话级传输层抽象（每会话一个实例，无全局状态）。
 * 协议核心只认“一个完整 XCP 包”（PID 起始，不含传输层头尾）。
 * 新传输（CAN/CAN FD/USB/串口）只需实现 XcpTlVtbl。
 *
 * 线程约定：
 *   send     —— 任意线程，实现内部自带发送锁；
 *   recv     —— 只由会话 RX 线程调用；
 *   shutdown —— 任意线程，使阻塞中的 recv 尽快返回 XCP_TL_CLOSED；
 *   destroy  —— RX 线程 join 之后调用。 */
#include <stdint.h>
#include "xcp_plat.h"

#define XCP_TL_TIMEOUT   0
#define XCP_TL_CLOSED   (-1)
#define XCP_TL_ERROR    (-2)
#define XCP_TL_TOOBIG   (-3)

typedef struct XcpTl XcpTl;

typedef struct XcpTlVtbl {
    const char* kind;                       /* "tcp" / "udp" / 将来 "can" */
    int  (*send)(XcpTl* tl, const uint8_t* pkt, int len);       /* 0 成功 */
    int  (*recv)(XcpTl* tl, uint8_t* pkt, int cap, int timeout_ms); /* >0 长度 / 0 超时 / <0 */
    void (*shutdown)(XcpTl* tl);
    void (*destroy)(XcpTl* tl);
} XcpTlVtbl;

typedef struct XcpTlStats {
    xcp_atomic_t tx_packets;
    xcp_atomic_t rx_packets;
    xcp_atomic_t rx_ctr_gaps;   /* 从机 CTR 不连续次数（以太网头） */
    xcp_atomic_t rx_bad_frames; /* LEN 非法 / 截断 */
} XcpTlStats;

struct XcpTl {
    const XcpTlVtbl* v;
    int max_packet;  /* 传输层能承载的最大 XCP 包（不含头） */
    XcpTlStats stats;
};

/* XCP on Ethernet（ASAM Part3）：头 = LEN(WORD, Intel) + CTR(WORD, Intel)。
 * UDP 一个数据报可含多个 XCP 包，按 LEN 逐个拆。
 * err_out 可为 NULL。失败返回 NULL。 */
XcpTl* xcp_tl_tcp_open(const char* host, int port, int connect_timeout_ms, int* err_out);
XcpTl* xcp_tl_udp_open(const char* host, int port, int* err_out);

static __inline int  xcp_tl_send(XcpTl* t, const uint8_t* p, int n) { return t->v->send(t, p, n); }
static __inline int  xcp_tl_recv(XcpTl* t, uint8_t* p, int c, int ms) { return t->v->recv(t, p, c, ms); }
static __inline void xcp_tl_shutdown(XcpTl* t) { t->v->shutdown(t); }
static __inline void xcp_tl_destroy(XcpTl* t) { if (t) t->v->destroy(t); }

#endif
