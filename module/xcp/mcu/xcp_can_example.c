/**
 * CAN 物理层示例骨架。
 * 对协议层仍是完整 XCP payload；CAN ID / DLC / 多帧由板级完成。
 */
#include "xcp_port.h"

extern int board_can_send(uint32_t id, const uint8_t* data, int len);
extern int board_can_recv(uint32_t* id, uint8_t* data, int cap); /* 无帧返回 0 */

#ifndef XCP_CAN_TX_ID
#define XCP_CAN_TX_ID 0x456
#define XCP_CAN_RX_ID 0x123
#endif

static XcpPort g_port;

static int can_send(void* user, const uint8_t* data, int len)
{
    (void)user;
    /* 简化：单帧 ≤8；多帧请按 ISO-TP / XCP-on-CAN 规范自行拆 */
    if (len <= 8) return board_can_send(XCP_CAN_TX_ID, data, len);
    return -1;
}

static int can_recv(void* user, uint8_t* data, int cap)
{
    uint32_t id = 0;
    int n;
    (void)user;
    n = board_can_recv(&id, data, cap);
    if (n <= 0) return 0;
    if (id != XCP_CAN_RX_ID) return 0;
    return n;
}

void xcp_can_port_init(void)
{
    g_port.user = 0;
    g_port.send = can_send;
    g_port.recv = can_recv;
    g_port.poll = 0;
}

XcpPort* xcp_can_port(void) { return &g_port; }
