/**
 * UART 物理层示例（伪代码/可适配 HAL）。
 * 帧：[u16 LE len][payload] —— 与 OpenScope xcp_serial 一致。
 *
 * 用法概要：
 *   1) 实现 uart_tx / uart_rx_byte（或 DMA 环形缓冲）
 *   2) 调用 xcp_uart_init + 主循环 xcp_mcu_poll(&g_port)
 *   3) xcp_mcu_set_cal_mem(标定区, 大小)
 */
#include "xcp_port.h"

#include <stdint.h>
#include <string.h>

/* ---- 板级需实现 ---- */
extern int  board_uart_write(const uint8_t* data, int len);
extern int  board_uart_read(uint8_t* data, int cap); /* 非阻塞，返回已读字节 */

typedef struct {
    uint8_t rx[512];
    int     rx_len;
} UartFramer;

static UartFramer g_fr;
static XcpPort g_port;

static int uart_send(void* user, const uint8_t* data, int len)
{
    uint8_t hdr[2];
    (void)user;
    if (len <= 0 || len > 0xFFFF) return -1;
    hdr[0] = (uint8_t)(len & 0xFF);
    hdr[1] = (uint8_t)((len >> 8) & 0xFF);
    if (board_uart_write(hdr, 2) != 0) return -1;
    return board_uart_write(data, len);
}

static int uart_recv(void* user, uint8_t* data, int cap)
{
    uint8_t tmp[64];
    int n, need;
    (void)user;
    n = board_uart_read(tmp, (int)sizeof(tmp));
    if (n > 0) {
        if (g_fr.rx_len + n > (int)sizeof(g_fr.rx)) g_fr.rx_len = 0;
        memcpy(g_fr.rx + g_fr.rx_len, tmp, (size_t)n);
        g_fr.rx_len += n;
    }
    if (g_fr.rx_len < 2) return 0;
    need = (int)g_fr.rx[0] | ((int)g_fr.rx[1] << 8);
    if (need <= 0 || need > cap) { g_fr.rx_len = 0; return -1; }
    if (g_fr.rx_len < 2 + need) return 0;
    memcpy(data, g_fr.rx + 2, (size_t)need);
    g_fr.rx_len -= (2 + need);
    if (g_fr.rx_len > 0)
        memmove(g_fr.rx, g_fr.rx + 2 + need, (size_t)g_fr.rx_len);
    return need;
}

void xcp_uart_init(void)
{
    memset(&g_fr, 0, sizeof(g_fr));
    memset(&g_port, 0, sizeof(g_port));
    g_port.send = uart_send;
    g_port.recv = uart_recv;
}

XcpPort* xcp_uart_port(void) { return &g_port; }

/* 主循环示例：
 *   xcp_uart_init();
 *   xcp_mcu_set_cal_mem(cal_page, sizeof(cal_page));
 *   for (;;) { xcp_mcu_poll(xcp_uart_port()); }
 */
