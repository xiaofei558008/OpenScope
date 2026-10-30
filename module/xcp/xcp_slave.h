#ifndef XCP_SLAVE_H
#define XCP_SLAVE_H

#include "xcp_proto.h"

/* 进程内 XCP 从机（标定 + 动态 DAQ）。编程资源未实现，对应命令返回 CMD_UNKNOWN。
 * 工作页 page0(RAM) 可读写；参考页 page1(FLASH) 只读，可 SET_CAL_PAGE 切换后上传，或 COPY_CAL_PAGE 交换。 */

typedef struct XcpSlave XcpSlave;

#ifndef XCP_SLAVE_DTO_Q
#define XCP_SLAVE_DTO_Q    256   /* DTO 队列深度（满则计 overflow 丢弃） */
#endif
#define XCP_SLAVE_MAX_SEED 128   /* 种子/密钥最大长度，可超 MAX_CTO-2 走多段 */

/* 可插拔 Seed&Key：seed_fn 产生种子（ctr 每次 GET_SEED 自增，可做随机/滚动种子），
 * key_fn 计算期望密钥，返回 key 长度（<0 失败）。签名对齐 Part4 XCP_ComputeKeyFromSeed。
 * 传 NULL 恢复默认：种子 11 22 33 44，key = seed XOR 0xFF。 */
typedef void (*XcpSlaveSeedFn)(void* user, uint8_t resource, uint32_t ctr, uint8_t* seed, int* seed_len);
typedef int  (*XcpSlaveKeyFn)(void* user, uint8_t resource, const uint8_t* seed, int seed_len,
                              uint8_t* key, int key_cap);
void      xcp_slave_set_seed_key(XcpSlave* s, XcpSlaveSeedFn seed_fn, XcpSlaveKeyFn key_fn, void* user);

/* 参考页（FLASH，page1）指针。SET_CAL_PAGE 切到 page1 后 UPLOAD/校验读它，写入回 WRITE_PROTECTED。 */
uint8_t*  xcp_slave_ref_ptr(XcpSlave* s, uint32_t addr, uint32_t len);
uint8_t   xcp_slave_protection(const XcpSlave* s);
int       xcp_slave_is_connected(const XcpSlave* s);
uint8_t   xcp_slave_ecu_page(const XcpSlave* s);
uint8_t   xcp_slave_xcp_page(const XcpSlave* s);
uint32_t  xcp_slave_dto_overflow(const XcpSlave* s);

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
