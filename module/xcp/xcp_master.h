#ifndef XCP_MASTER_H
#define XCP_MASTER_H

#include "xcp_proto.h"

/* 主机。传输一次发一整包 CTO、一次收一整包（应答或 DTO）。 */

typedef struct XcpIo {
    void* ctx;
    int (*send)(void* ctx, const uint8_t* data, int len);
    int (*recv)(void* ctx, uint8_t* data, int cap, int timeout_ms);
} XcpIo;

typedef struct XcpMaster {
    XcpIo    io;
    int      connected;
    uint8_t  max_cto;
    uint16_t max_dto;
    uint8_t  resource;
    int      last_err; /* 从机 ERR 码，0 表示无 */
} XcpMaster;

void xcp_master_init(XcpMaster* m, const XcpIo* io);

/* 返回 0 成功，负数为传输失败，正数是从机错误码。 */
int  xcp_master_connect(XcpMaster* m);
int  xcp_master_disconnect(XcpMaster* m);
int  xcp_master_upload(XcpMaster* m, uint32_t addr, void* dst, int n);
int  xcp_master_download(XcpMaster* m, uint32_t addr, const void* src, int n);
int  xcp_master_checksum(XcpMaster* m, uint32_t addr, uint32_t size, uint32_t* sum);
int  xcp_master_unlock_cal(XcpMaster* m); /* 用本从机的测试密钥解开 CAL 锁 */

/* DAQ：释放、分配、写一条表项、选中并启动、同步启动。 */
int  xcp_master_daq_free(XcpMaster* m);
int  xcp_master_daq_alloc(XcpMaster* m, int n_daq, int n_odt, int n_entry);
int  xcp_master_daq_entry(XcpMaster* m, uint16_t daq, uint8_t odt, uint8_t entry,
                          uint32_t addr, uint8_t size);
int  xcp_master_daq_start(XcpMaster* m, uint16_t daq, uint16_t event);
int  xcp_master_recv_dto(XcpMaster* m, uint8_t* dst, int cap, int timeout_ms);

#endif
