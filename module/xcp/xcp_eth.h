#ifndef XCP_ETH_H
#define XCP_ETH_H

/* XCP on Ethernet（TCP）：2 字节长度 + 2 字节计数 + 载荷。长度与计数为小端。 */

#include "xcp_master.h"
#include "xcp_slave.h"

typedef struct XcpEthClient XcpEthClient;
typedef struct XcpEthServer XcpEthServer;

XcpEthClient* xcp_eth_connect(const char* host, int port);
void          xcp_eth_close(XcpEthClient* c);
void          xcp_eth_io(XcpEthClient* c, XcpIo* io);

/* 在 127.0.0.1 上监听。port=0 由系统分配。失败返回 NULL。 */
XcpEthServer* xcp_eth_serve(XcpSlave* slave, int port);
int           xcp_eth_port(XcpEthServer* s);
void          xcp_eth_serve_stop(XcpEthServer* s);

#endif
