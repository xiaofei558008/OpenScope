/**
 * MCU 端精简 XCP 从机（CONNECT / DISCONNECT / SHORT_UPLOAD / DOWNLOAD / SET_MTA）。
 * 不依赖 Windows；与主机协议号一致（见 ../xcp_proto.h）。
 */
#include "xcp_port.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef XCP_PID_RES
#define XCP_PID_RES 0xFFu
#define XCP_PID_ERR 0xFEu
#define XCP_CMD_CONNECT        0xFFu
#define XCP_CMD_DISCONNECT     0xFEu
#define XCP_CMD_GET_STATUS     0xFDu
#define XCP_CMD_SYNCH          0xFCu
#define XCP_CMD_SET_MTA        0xF6u
#define XCP_CMD_UPLOAD         0xF5u
#define XCP_CMD_SHORT_UPLOAD   0xF4u
#define XCP_CMD_DOWNLOAD       0xF0u
#define XCP_ERR_CMD_UNKNOWN    0x20u
#define XCP_ERR_OUT_OF_RANGE   0x22u
#define XCP_ERR_CMD_SYNCH      0x00u
#define XCP_RES_CAL            0x01u
#define XCP_MAX_CTO            255
#endif

static XcpLogFn g_log;
static uint8_t* g_cal;
static uint32_t g_cal_size;
static uint32_t g_mta;
static int g_connected;

void xcp_port_set_log(XcpLogFn fn) { g_log = fn; }

void xcp_port_log(int level, const char* fmt, ...)
{
    char buf[160];
    va_list ap;
    if (!g_log) return;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    g_log(level, buf);
}

void xcp_mcu_set_cal_mem(uint8_t* base, uint32_t size)
{
    g_cal = base;
    g_cal_size = size;
    g_mta = 0;
}

static int in_range(uint32_t addr, uint32_t n)
{
    if (!g_cal || g_cal_size == 0) return 0;
    if (addr < (uint32_t)(uintptr_t)g_cal) return 0;
    if (addr + n > (uint32_t)(uintptr_t)g_cal + g_cal_size) return 0;
    return 1;
}

int xcp_mcu_on_cto(const uint8_t* cmd, int cmd_len, uint8_t* rsp, int rsp_cap)
{
    uint8_t c;
    if (!cmd || cmd_len < 1 || !rsp || rsp_cap < 2) return -1;
    c = cmd[0];
    switch (c) {
    case XCP_CMD_CONNECT:
        g_connected = 1;
        if (rsp_cap < 8) return -1;
        rsp[0] = XCP_PID_RES;
        rsp[1] = 0x00; /* resource: CAL */
        rsp[1] = XCP_RES_CAL;
        rsp[2] = 0x00; /* COMM_MODE_BASIC */
        rsp[3] = (uint8_t)XCP_MAX_CTO;
        rsp[4] = 0xFF; rsp[5] = 0x00; /* MAX_DTO */
        rsp[6] = 0x01; rsp[7] = 0x01; /* protocol/transport ver */
        xcp_port_log(0, "XCP CONNECT");
        return 8;
    case XCP_CMD_DISCONNECT:
        g_connected = 0;
        rsp[0] = XCP_PID_RES;
        xcp_port_log(0, "XCP DISCONNECT");
        return 1;
    case XCP_CMD_GET_STATUS:
        rsp[0] = XCP_PID_RES;
        rsp[1] = g_connected ? 0x00 : 0x00;
        rsp[2] = 0; rsp[3] = 0; rsp[4] = 0; rsp[5] = 0;
        return 6;
    case XCP_CMD_SYNCH:
        rsp[0] = XCP_PID_ERR;
        rsp[1] = XCP_ERR_CMD_SYNCH;
        return 2;
    case XCP_CMD_SET_MTA:
        if (cmd_len < 8) { rsp[0] = XCP_PID_ERR; rsp[1] = XCP_ERR_CMD_UNKNOWN; return 2; }
        g_mta = (uint32_t)cmd[4] | ((uint32_t)cmd[5] << 8) |
                ((uint32_t)cmd[6] << 16) | ((uint32_t)cmd[7] << 24);
        rsp[0] = XCP_PID_RES;
        return 1;
    case XCP_CMD_SHORT_UPLOAD: {
        uint8_t n = cmd[1];
        uint32_t addr;
        if (cmd_len < 8) { rsp[0] = XCP_PID_ERR; rsp[1] = XCP_ERR_CMD_UNKNOWN; return 2; }
        addr = (uint32_t)cmd[4] | ((uint32_t)cmd[5] << 8) |
               ((uint32_t)cmd[6] << 16) | ((uint32_t)cmd[7] << 24);
        if (!in_range(addr, n) || rsp_cap < 1 + n) {
            rsp[0] = XCP_PID_ERR; rsp[1] = XCP_ERR_OUT_OF_RANGE; return 2;
        }
        rsp[0] = XCP_PID_RES;
        memcpy(rsp + 1, (void*)(uintptr_t)addr, n);
        return 1 + n;
    }
    case XCP_CMD_UPLOAD: {
        uint8_t n = cmd[1];
        if (!in_range(g_mta, n) || rsp_cap < 1 + n) {
            rsp[0] = XCP_PID_ERR; rsp[1] = XCP_ERR_OUT_OF_RANGE; return 2;
        }
        rsp[0] = XCP_PID_RES;
        memcpy(rsp + 1, (void*)(uintptr_t)g_mta, n);
        g_mta += n;
        return 1 + n;
    }
    case XCP_CMD_DOWNLOAD: {
        uint8_t n = cmd[1];
        if (cmd_len < 2 + n || !in_range(g_mta, n)) {
            rsp[0] = XCP_PID_ERR; rsp[1] = XCP_ERR_OUT_OF_RANGE; return 2;
        }
        memcpy((void*)(uintptr_t)g_mta, cmd + 2, n);
        g_mta += n;
        rsp[0] = XCP_PID_RES;
        return 1;
    }
    default:
        rsp[0] = XCP_PID_ERR;
        rsp[1] = XCP_ERR_CMD_UNKNOWN;
        xcp_port_log(1, "XCP unknown cmd 0x%02X", c);
        return 2;
    }
}

void xcp_mcu_poll(XcpPort* port)
{
    uint8_t cmd[256], rsp[256];
    int n, m;
    if (!port || !port->recv || !port->send) return;
    if (port->poll) port->poll(port->user);
    n = port->recv(port->user, cmd, (int)sizeof(cmd));
    if (n <= 0) return;
    m = xcp_mcu_on_cto(cmd, n, rsp, (int)sizeof(rsp));
    if (m > 0) port->send(port->user, rsp, m);
}
