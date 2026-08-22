#ifndef OS_NETWS_H
#define OS_NETWS_H
/* 自研 RFC6455 WebSocket：握手（SHA1+base64）+ 帧编解码（掩码/长度扩展）+ TCP 客户端/服务端。
 * 纯逻辑部分（帧、握手 key）可脱离 socket 单测；连接部分用回环集成测试。 */
#include <stdint.h>

#define OS_WS_OP_CONT  0x0
#define OS_WS_OP_TEXT  0x1
#define OS_WS_OP_BIN   0x2
#define OS_WS_OP_CLOSE 0x8
#define OS_WS_OP_PING  0x9
#define OS_WS_OP_PONG  0xA

/* 握手：Sec-WebSocket-Key -> Sec-WebSocket-Accept（SHA1+base64）。返回写出字节数，cap 不足 -1 */
int os_ws_accept_key(const char* key, char* out, int cap);

/* 帧编码：fin/opcode + 长度(7/16/64bit) + 掩码 + payload。返回总字节，cap 不足 -1 */
int os_ws_frame_encode(uint8_t* out, int cap, int fin, int opcode,
                       const uint8_t* payload, uint64_t len, int mask, const uint8_t mask_key[4]);
/* 帧解码：完整帧返回 0，*plen=payload 长度（去掩码后写入 payload，cap 上限）；
 * 数据不足返回 1；错误返回 -1。*consumed=整帧字节数。 */
int os_ws_frame_decode(const uint8_t* in, int len, int* fin, int* opcode,
                       uint8_t* payload, int payload_cap, uint64_t* plen, int* consumed);

typedef struct OS_WSConn OS_WSConn;

int        os_ws_listen(const char* bind_ip, int port);       /* 监听 socket，失败 -1 */
void       os_ws_close_listen(int lsock);
OS_WSConn* os_ws_accept(int lsock);                           /* 接受连接+服务端握手，失败 NULL */
OS_WSConn* os_ws_connect(const char* host, int port);         /* 连接+客户端握手，失败 NULL */
void       os_ws_close(OS_WSConn* c);
int        os_ws_send_bin(OS_WSConn* c, const uint8_t* data, uint32_t len);
int        os_ws_send_text(OS_WSConn* c, const char* s);
/* 收一帧：payload 写入 buf，*opcode 输出，返回 payload 长度；对端关闭返回 0；错误 -1 */
int        os_ws_recv(OS_WSConn* c, uint8_t* buf, int cap, int* opcode);

#endif
