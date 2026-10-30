#ifndef XCP_MOCK_H
#define XCP_MOCK_H

/* D 块：测试用 Mock 从机 = xcp_slave（DAQ/标定页/Seed&Key）+ 可配置故障注入。
 * 传输无关：xcp_mock_input 处理一条 CTO，xcp_mock_pop 取异步包（DAQ DTO、EV）。
 * TCP 服务端外壳见 xcp_mock_main.c（XCP-on-TCP 帧 [u16 len][u16 ctr][payload]）。 */

#include "xcp_slave.h"

enum {
    XCP_FAULT_NONE = 0,
    XCP_FAULT_TIMEOUT,   /* 吞掉应答（命令仍执行与否见 exec 字段） */
    XCP_FAULT_ERROR,     /* 不执行命令，直接回 ERR <err> */
    XCP_FAULT_PENDING,   /* 先发 EV_CMD_PENDING，应答推迟 delay_ms（由外壳实现） */
    XCP_FAULT_DELAY      /* 正常执行，应答推迟 delay_ms（> 主机 t1 即超时） */
};

#define XCP_FAULT_ANY_CMD 0x00u
#define XCP_MOCK_MAX_FAULTS 16

typedef struct XcpFault {
    uint8_t  type;
    uint8_t  cmd;       /* 命令 PID，0 = 任意命令（SYNCH/CONNECT 也算） */
    uint8_t  err;       /* XCP_FAULT_ERROR 的错误码（ERR_CMD_BUSY/ACCESS_LOCKED/OUT_OF_RANGE…） */
    uint8_t  exec;      /* TIMEOUT 时是否仍执行命令（1 = 执行但丢应答，模拟应答丢失） */
    int      skip;      /* 先放过前 skip 次匹配 */
    int      count;     /* 触发次数，-1 = 无限 */
    int      delay_ms;  /* PENDING/DELAY 用 */
} XcpFault;

typedef struct XcpMockDaqFault {
    int drop_every;     /* 每 N 个 DTO 丢 1 个，0 = 关 */
    int swap_every;     /* 每 N 个 DTO 与下一个交换顺序（乱序），0 = 关 */
    int dup_every;      /* 每 N 个 DTO 重复发 1 次，0 = 关 */
} XcpMockDaqFault;

typedef struct XcpMock XcpMock;

typedef struct XcpMockResult {
    int len;            /* 应答长度；0 = 不应答（超时或块传输中间帧） */
    int delay_ms;       /* 外壳应在 delay_ms 后再发应答 */
    int faulted;        /* 本次是否命中故障规则 */
} XcpMockResult;

XcpMock*  xcp_mock_create(uint32_t base, uint32_t size);
void      xcp_mock_destroy(XcpMock* m);
XcpSlave* xcp_mock_slave(XcpMock* m);

int       xcp_mock_add_fault(XcpMock* m, const XcpFault* f);
void      xcp_mock_clear_faults(XcpMock* m);
void      xcp_mock_set_daq_fault(XcpMock* m, const XcpMockDaqFault* f);
/* 文本规则："timeout:CMD[:count[:exec]]" "err:CMD:CODE[:count]" "busy:CMD[:count]"
 * "pending:CMD:ms[:count]" "delay:CMD:ms[:count]" "drop:N" "swap:N" "dup:N"；CMD 为十六进制或 any */
int       xcp_mock_parse_fault(XcpMock* m, const char* spec);

XcpMockResult xcp_mock_input(XcpMock* m, const uint8_t* cmd, int len, uint8_t* rsp, int rsp_cap);
/* 产生事件 DTO（转调 xcp_slave_tick） */
void      xcp_mock_tick(XcpMock* m, uint16_t event, uint32_t time_us);
/* 取出下一条异步包（EV 优先，其次经丢包/乱序/重复处理后的 DTO）。0 = 空 */
int       xcp_mock_pop(XcpMock* m, uint8_t* dst, int cap);
/* 从机块模式余帧（UPLOAD > 一帧）。外壳在发出应答首帧后立即依次发送。0 = 无 */
int       xcp_mock_pop_rsp(XcpMock* m, uint8_t* dst, int cap);

typedef struct XcpMockStats {
    uint32_t dto_in, dto_out, dto_dropped, dto_swapped, dto_dup, faults_hit;
} XcpMockStats;
void      xcp_mock_stats(const XcpMock* m, XcpMockStats* out);

#endif
