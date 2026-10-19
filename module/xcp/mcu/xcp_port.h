/**
 * MCU 端 XCP 物理层无关端口（可拷贝到固件工程）。
 *
 * 实现本接口即可接入任意物理层：UART / CAN / TCP(LwIP) / USB-CDC …
 * 协议层（xcp_slave 或本目录简化从机）只调用 send/recv/poll。
 *
 * 帧约定（与主机 OpenScope xcp_serial / xcp_tcp 对齐）：
 *   UART : [u16 LE len][payload]
 *   TCP  : [u16 LE len][u16 LE ctr][payload]
 *   CAN  : 适配器内完成 ID/DLC/多帧，对协议层仍是完整 payload
 */
#ifndef XCP_PORT_H
#define XCP_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XcpPort {
    void* user;
    /** 发送一帧 XCP payload（不含物理层头）；成功返回 0 */
    int (*send)(void* user, const uint8_t* data, int len);
    /** 尝试接收一帧；有数据返回长度，无数据返回 0，错误返回负 */
    int (*recv)(void* user, uint8_t* data, int cap);
    /** 可选：驱动轮询 / 喂狗；可为 NULL */
    void (*poll)(void* user);
} XcpPort;

/** 日志钩子：level 0=INFO 1=WARN 2=ERROR 3=DEBUG；可为 NULL */
typedef void (*XcpLogFn)(int level, const char* msg);
void xcp_port_set_log(XcpLogFn fn);
void xcp_port_log(int level, const char* fmt, ...);

/** 处理一帧主机 CTO，写出响应；返回响应长度，0=无响应，负=错误 */
int xcp_mcu_on_cto(const uint8_t* cmd, int cmd_len, uint8_t* rsp, int rsp_cap);

/** 主循环友好：从 port 收一帧、处理、发回 */
void xcp_mcu_poll(XcpPort* port);

/** 标定内存窗口（示例）：基址 + 大小；读写经此窗口 */
void xcp_mcu_set_cal_mem(uint8_t* base, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
