/**
 * TCP 物理层示例（LwIP / BSD socket 风格）。
 * 帧：[u16 LE len][u16 LE ctr][payload] —— 与 OpenScope xcp_tcp 一致。
 */
#include "xcp_port.h"

#include <string.h>

extern int board_tcp_send(const uint8_t* data, int len);
extern int board_tcp_recv(uint8_t* data, int cap); /* 非阻塞 */

typedef struct {
    uint8_t buf[600];
    int     len;
    uint16_t ctr;
} TcpFramer;

static TcpFramer g_fr;
static XcpPort g_port;

static int tcp_send(void* user, const uint8_t* data, int len)
{
    uint8_t hdr[4];
    (void)user;
    if (len <= 0 || len > 0xFFFF) return -1;
    hdr[0] = (uint8_t)(len & 0xFF);
    hdr[1] = (uint8_t)((len >> 8) & 0xFF);
    hdr[2] = (uint8_t)(g_fr.ctr & 0xFF);
    hdr[3] = (uint8_t)((g_fr.ctr >> 8) & 0xFF);
    g_fr.ctr++;
    if (board_tcp_send(hdr, 4) != 0) return -1;
    return board_tcp_send(data, len);
}

static int tcp_recv(void* user, uint8_t* data, int cap)
{
    uint8_t tmp[128];
    int n, need;
    (void)user;
    n = board_tcp_recv(tmp, (int)sizeof(tmp));
    if (n > 0) {
        if (g_fr.len + n > (int)sizeof(g_fr.buf)) g_fr.len = 0;
        memcpy(g_fr.buf + g_fr.len, tmp, (size_t)n);
        g_fr.len += n;
    }
    if (g_fr.len < 4) return 0;
    need = (int)g_fr.buf[0] | ((int)g_fr.buf[1] << 8);
    if (need <= 0 || need > cap) { g_fr.len = 0; return -1; }
    if (g_fr.len < 4 + need) return 0;
    memcpy(data, g_fr.buf + 4, (size_t)need);
    g_fr.len -= (4 + need);
    if (g_fr.len > 0)
        memmove(g_fr.buf, g_fr.buf + 4 + need, (size_t)g_fr.len);
    return need;
}

void xcp_tcp_port_init(void)
{
    memset(&g_fr, 0, sizeof(g_fr));
    g_port.send = tcp_send;
    g_port.recv = tcp_recv;
}

XcpPort* xcp_tcp_port(void) { return &g_port; }
