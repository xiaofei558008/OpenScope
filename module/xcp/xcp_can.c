#include "xcp_transport.h"
#include "xcp_can.h"

/* CAN 后端仍为空壳；接口与 XcpTransport 对齐，便于接入周立功/Kvaser。 */

static int can_scan(char names[][64], int cap)
{
    (void)names;
    (void)cap;
    return 0;
}

static int can_open(void** out, int index, const char* opts)
{
    (void)out; (void)index; (void)opts;
    return -1;
}

static void can_close(void* ctx) { (void)ctx; }

static void can_bind(void* ctx, XcpIo* io)
{
    (void)ctx;
    if (io) { io->ctx = NULL; io->send = NULL; io->recv = NULL; }
}

static const XcpTransport g_can = {
    "can",
    "CAN",
    can_scan,
    can_open,
    can_close,
    can_bind
};

void xcp_can_register(void)
{
    xcp_transport_register(&g_can);
}

/* 兼容旧 API */
int xcp_can_backend_count(void)
{
    return 0;
}

const XcpCanBackend* xcp_can_backend(int index)
{
    (void)index;
    return NULL;
}
