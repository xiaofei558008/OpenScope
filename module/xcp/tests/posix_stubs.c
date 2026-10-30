/* 仅 Linux/CI 构建 xcp_smoke 用：替代 Win32 serial/tcp 传输的注册占位。 */
#include "../xcp_transport.h"
#include <stddef.h>
static const XcpTransport k_serial = { "serial", "Serial (stub)", NULL, NULL, NULL, NULL };
static const XcpTransport k_tcp = { "tcp", "TCP (stub)", NULL, NULL, NULL, NULL };
void xcp_serial_register(void) { xcp_transport_register(&k_serial); }
void xcp_tcp_register(void) { xcp_transport_register(&k_tcp); }
