#ifndef XCP_TRANSPORT_H
#define XCP_TRANSPORT_H

#include "xcp_master.h"

/* XCP 物理层无关传输接口。
 * 协议层只认 XcpIo（一发一收一包 CTO/DTO）。
 * 各适配器自管成帧、超时与设备枚举。
 *
 * 内置：
 *   serial  — Win32 COM，帧格式 [u16 LE len][payload]
 *   can     — 预留（周立功/Kvaser），当前无后端时扫描为空
 *   tcp     — 预留 XCP-on-TCP（[u16 LE len][u16 LE ctr][payload]），当前可连本机测试口
 *   loopback— 进程内（仅测试）
 *
 * 与 OpenScope network.dll 无关（那是两台 OpenScope 之间的样本同步）。
 */

typedef struct XcpTransport {
    const char* name;   /* "serial" / "can" / "tcp" / "loopback" */
    const char* label;  /* UI 显示 */
    /* 扫描设备名写入 names[i]；返回数量。opts 可为 NULL。 */
    int  (*scan)(char names[][64], int cap);
    /* open：index 来自 scan；opts 可选 "COM3" / "COM3:115200" / "127.0.0.1:5555" / baud 字符串 */
    int  (*open)(void** out, int index, const char* opts);
    void (*close)(void* ctx);
    void (*bind)(void* ctx, XcpIo* io);
} XcpTransport;

int xcp_transport_count(void);
const XcpTransport* xcp_transport_at(int index);
const XcpTransport* xcp_transport_by_name(const char* name);

/* 内部注册（各 .c 的 xcp_*_register 在模块 init 时调用） */
void xcp_transport_register(const XcpTransport* t);
void xcp_transport_init_builtins(void);

#endif
