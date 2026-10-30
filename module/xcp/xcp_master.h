#ifndef XCP_MASTER_H
#define XCP_MASTER_H

#include "xcp_proto.h"

/* 主机（兼容层）。第 2 轮起内部改为 XcpSession：独立 RX 线程按 PID 分流、超时/SYNCH/重试、
 * 不再“发一收一”，MAX_CTO/MAX_DTO 不再截到 64。函数签名与返回约定保持不变：
 *   0 成功；-1 传输失败/超时（调用方应丢链路）；-2 从机 ERR（码在 last_err，会话保留）。
 * 新代码请直接用 xcp_session.h。 */

struct XcpSession;
struct XcpTl;

typedef struct XcpIo {
    void* ctx;
    int (*send)(void* ctx, const uint8_t* data, int len);
    int (*recv)(void* ctx, uint8_t* data, int cap, int timeout_ms);
} XcpIo;

typedef struct XcpMaster {
    XcpIo    io;
    int      connected;
    uint8_t  max_cto;      /* CONNECT 原值（<=255） */
    uint16_t max_dto;      /* CONNECT 原值 */
    uint8_t  resource;
    int      last_err;     /* 从机 ERR 码，0 表示无 */
    /* 第 2 轮新增（内部） */
    uint8_t  byte_order;
    uint8_t  ag;
    struct XcpSession* s;  /* 首次 connect 时创建 */
    struct XcpTl* tl;      /* 尚未交给会话的传输 */
    void*    dtoq;         /* recv_dto 兼容队列 */
} XcpMaster;

/* 旧接口：基于 XcpIo（serial 等）。io 的 recv 由会话 RX 线程以 <=50ms 切片调用；
 * 调用方在 xcp_master_free 之后才能关闭 io 底层句柄。 */
void xcp_master_init(XcpMaster* m, const XcpIo* io);
/* 新：直接给 XcpTl（tcp/udp），所有权交给 m */
void xcp_master_init_tl(XcpMaster* m, struct XcpTl* tl);
/* 释放会话/传输/队列（可重复调用）。旧代码没有这个函数——迁移后必须调用。 */
void xcp_master_free(XcpMaster* m);
struct XcpSession* xcp_master_session(XcpMaster* m);
int  xcp_master_link_ok(XcpMaster* m);

/* 返回 0 成功，-1 传输失败/超时，-2 从机错误（m->last_err）。 */
int  xcp_master_connect(XcpMaster* m);
int  xcp_master_disconnect(XcpMaster* m);
int  xcp_master_upload(XcpMaster* m, uint32_t addr, void* dst, int n);
int  xcp_master_download(XcpMaster* m, uint32_t addr, const void* src, int n);
int  xcp_master_checksum(XcpMaster* m, uint32_t addr, uint32_t size, uint32_t* sum); /* 仅接受 ADD_14（旧语义） */
int  xcp_master_checksum_ex(XcpMaster* m, uint32_t addr, uint32_t size, uint8_t* type, uint32_t* sum);
int  xcp_master_unlock_cal(XcpMaster* m); /* 用本从机的测试密钥解开 CAL 锁 */

/* DAQ：释放、分配、写一条表项、选中并启动、同步启动。 */
int  xcp_master_daq_free(XcpMaster* m);
int  xcp_master_daq_alloc(XcpMaster* m, int n_daq, int n_odt, int n_entry);
int  xcp_master_daq_entry(XcpMaster* m, uint16_t daq, uint8_t odt, uint8_t entry,
                          uint32_t addr, uint8_t size);
int  xcp_master_daq_start(XcpMaster* m, uint16_t daq, uint16_t event);
/* 取一条 DTO（RX 线程收到后入队，队满丢最旧）。timeout_ms<=0 时不等待。 */
int  xcp_master_recv_dto(XcpMaster* m, uint8_t* dst, int cap, int timeout_ms);

#endif
