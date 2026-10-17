#ifndef XCP_SLAVE_H
#define XCP_SLAVE_H

#include "xcp_proto.h"

/* 进程内 XCP 从机（标定 + 动态 DAQ）。编程资源未实现，对应命令返回 CMD_UNKNOWN。
 * 工作页 page0 可读写；参考页 page1 只通过 COPY_CAL_PAGE 交换。 */

typedef struct XcpSlave XcpSlave;

XcpSlave* xcp_slave_create(uint32_t base, uint32_t size);
void      xcp_slave_destroy(XcpSlave* s);

/* 工作页指针（测试直接铺数据）。越界返回 NULL。 */
uint8_t*  xcp_slave_ptr(XcpSlave* s, uint32_t addr, uint32_t len);

/* 把某个资源锁上（GET_SEED / UNLOCK 用）。resource 为 XCP_RES_*。 */
void      xcp_slave_lock(XcpSlave* s, uint8_t resource);

/* 处理一条 CTO。rsp_cap >= XCP_MAX_CTO。返回应答字节数；0 表示块传输尚未结束、不应答。 */
int       xcp_slave_input(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp, int rsp_cap);

/* 对正在运行、且事件通道匹配的 DAQ 列表产生 DTO（含 PID，首 ODT 带 4 字节时间戳）。 */
void      xcp_slave_tick(XcpSlave* s, uint16_t event, uint32_t time_us);

/* 取出一条 DTO。返回长度，0 表示队列空。 */
int       xcp_slave_pop_dto(XcpSlave* s, uint8_t* dst, int cap);

#endif
