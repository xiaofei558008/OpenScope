#include "xcp_transport.h"
#include <string.h>

#define XCP_TR_MAX 8

static const XcpTransport* g_tr[XCP_TR_MAX];
static int g_tr_n;

void xcp_transport_register(const XcpTransport* t)
{
    int i;
    if (!t || !t->name || g_tr_n >= XCP_TR_MAX) return;
    for (i = 0; i < g_tr_n; i++)
        if (g_tr[i] && strcmp(g_tr[i]->name, t->name) == 0) {
            g_tr[i] = t;
            return;
        }
    g_tr[g_tr_n++] = t;
}

int xcp_transport_count(void)
{
    return g_tr_n;
}

const XcpTransport* xcp_transport_at(int index)
{
    if (index < 0 || index >= g_tr_n) return NULL;
    return g_tr[index];
}

const XcpTransport* xcp_transport_by_name(const char* name)
{
    int i;
    if (!name || !name[0]) return NULL;
    for (i = 0; i < g_tr_n; i++)
        if (g_tr[i] && strcmp(g_tr[i]->name, name) == 0)
            return g_tr[i];
    return NULL;
}

/* 各后端声明 */
void xcp_serial_register(void);
void xcp_can_register(void);
void xcp_tcp_register(void);

void xcp_transport_init_builtins(void)
{
    g_tr_n = 0;
    xcp_serial_register();
    xcp_can_register();
    xcp_tcp_register();
}
