#ifndef XCP_CAN_H
#define XCP_CAN_H

#include "xcp_master.h"

/* XCP 传输与 OpenScope 网络模块无关。
 * 网络模块把一台电脑上 OpenScope 采到的样本送给另一台电脑上的 OpenScope。
 * XCP 主机经 CAN 适配器跟 ECU 从机说话。协议层只认 XcpIo：
 * send 一次发一个 CTO，recv 一次收一个 CTO 或 DTO。
 * 适配器自己处理 CAN ID、DLC 和多帧拼接。
 *
 * 计划中的后端（各自实现本结构，当前都还没接入）：
 *   zlg    周立功 ControlCAN / zlgcan
 *   kvaser Kvaser CANlib
 */

typedef struct XcpCanBackend {
    const char* name;
    int  (*scan)(char names[][64], int cap);          /* 返回发现的设备数 */
    int  (*open)(void** out, int index);              /* 0 成功 */
    void (*close)(void* ctx);
    void (*bind)(void* ctx, XcpIo* io);
} XcpCanBackend;

/* 已接入的后端数量。目前为 0。 */
int xcp_can_backend_count(void);
const XcpCanBackend* xcp_can_backend(int index);

#endif
