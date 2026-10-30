#include "xcp_mock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EVQ 16

struct XcpMock {
    XcpSlave*       s;
    XcpFault        f[XCP_MOCK_MAX_FAULTS];
    int             nf;
    XcpMockDaqFault df;
    uint8_t         held[XCP_MAX_DTO];   /* 乱序：暂扣的 DTO */
    int             held_len;
    uint8_t         dupbuf[XCP_MAX_DTO];
    int             dup_len;
    uint8_t         evq[EVQ][8];
    int             evlen[EVQ];
    int             evn;
    XcpMockStats    st;
};

XcpMock* xcp_mock_create(uint32_t base, uint32_t size)
{
    XcpMock* m = (XcpMock*)calloc(1, sizeof(XcpMock));
    if (!m) return NULL;
    m->s = xcp_slave_create(base, size);
    if (!m->s) { free(m); return NULL; }
    return m;
}

void xcp_mock_destroy(XcpMock* m)
{
    if (!m) return;
    xcp_slave_destroy(m->s);
    free(m);
}

XcpSlave* xcp_mock_slave(XcpMock* m) { return m ? m->s : NULL; }

int xcp_mock_add_fault(XcpMock* m, const XcpFault* f)
{
    if (!m || !f || m->nf >= XCP_MOCK_MAX_FAULTS) return -1;
    m->f[m->nf++] = *f;
    return 0;
}

void xcp_mock_clear_faults(XcpMock* m)
{
    if (!m) return;
    m->nf = 0;
    memset(&m->df, 0, sizeof(m->df));
}

void xcp_mock_set_daq_fault(XcpMock* m, const XcpMockDaqFault* f)
{
    if (m && f) m->df = *f;
}

static int parse_cmd(const char* t, uint8_t* out)
{
    char* e;
    unsigned long v;
    if (!t || !*t) return -1;
    if (strcmp(t, "any") == 0) { *out = 0; return 0; }
    v = strtoul(t, &e, 16);
    if (*e || v > 0xFF) return -1;
    *out = (uint8_t)v;
    return 0;
}

int xcp_mock_parse_fault(XcpMock* m, const char* spec)
{
    char buf[96], *tok[6];
    int n = 0;
    XcpFault f;
    char* p;
    if (!m || !spec || strlen(spec) >= sizeof(buf)) return -1;
    strcpy(buf, spec);
    p = buf;
    while (n < 6) {
        tok[n++] = p;
        p = strchr(p, ':');
        if (!p) break;
        *p++ = 0;
    }
    memset(&f, 0, sizeof(f));
    f.count = 1;
    if (!strcmp(tok[0], "drop") || !strcmp(tok[0], "swap") || !strcmp(tok[0], "dup")) {
        int v;
        if (n < 2) return -1;
        v = atoi(tok[1]);
        if (v < 0) return -1;
        if (tok[0][1] == 'r') m->df.drop_every = v;
        else if (tok[0][0] == 's') m->df.swap_every = v;
        else m->df.dup_every = v;
        return 0;
    }
    if (n < 2 || parse_cmd(tok[1], &f.cmd)) return -1;
    if (!strcmp(tok[0], "timeout")) {
        f.type = XCP_FAULT_TIMEOUT;
        if (n > 2) f.count = atoi(tok[2]);
        if (n > 3) f.exec = (uint8_t)atoi(tok[3]);
    } else if (!strcmp(tok[0], "err")) {
        if (n < 3) return -1;
        f.type = XCP_FAULT_ERROR;
        f.err = (uint8_t)strtoul(tok[2], NULL, 16);
        if (n > 3) f.count = atoi(tok[3]);
    } else if (!strcmp(tok[0], "busy")) {
        f.type = XCP_FAULT_ERROR;
        f.err = XCP_ERR_CMD_BUSY;
        if (n > 2) f.count = atoi(tok[2]);
    } else if (!strcmp(tok[0], "pending") || !strcmp(tok[0], "delay")) {
        if (n < 3) return -1;
        f.type = tok[0][0] == 'p' ? XCP_FAULT_PENDING : XCP_FAULT_DELAY;
        f.delay_ms = atoi(tok[2]);
        if (n > 3) f.count = atoi(tok[3]);
    } else
        return -1;
    return xcp_mock_add_fault(m, &f);
}

static XcpFault* match(XcpMock* m, uint8_t cmd)
{
    int i;
    for (i = 0; i < m->nf; i++) {
        XcpFault* f = &m->f[i];
        if (f->type == XCP_FAULT_NONE || f->count == 0) continue;
        if (f->cmd != 0 && f->cmd != cmd) continue;
        if (f->skip > 0) { f->skip--; continue; }
        if (f->count > 0) f->count--;
        return f;
    }
    return NULL;
}

static void push_ev(XcpMock* m, uint8_t code)
{
    if (m->evn >= EVQ) return;
    m->evq[m->evn][0] = XCP_PID_EV;
    m->evq[m->evn][1] = code;
    m->evlen[m->evn] = 2;
    m->evn++;
}

XcpMockResult xcp_mock_input(XcpMock* m, const uint8_t* cmd, int len, uint8_t* rsp, int rsp_cap)
{
    XcpMockResult r;
    XcpFault* f;
    memset(&r, 0, sizeof(r));
    if (!m || !cmd || len < 1) return r;
    f = match(m, cmd[0]);
    if (f) {
        m->st.faults_hit++;
        r.faulted = 1;
        switch (f->type) {
        case XCP_FAULT_TIMEOUT:
            if (f->exec) xcp_slave_input(m->s, cmd, len, rsp, rsp_cap);
            r.len = 0;
            return r;
        case XCP_FAULT_ERROR:
            rsp[0] = XCP_PID_ERR;
            rsp[1] = f->err;
            r.len = 2;
            return r;
        case XCP_FAULT_PENDING:
            push_ev(m, 0x05); /* EV_CMD_PENDING */
            r.delay_ms = f->delay_ms;
            break;
        case XCP_FAULT_DELAY:
            r.delay_ms = f->delay_ms;
            break;
        default:
            break;
        }
    }
    r.len = xcp_slave_input(m->s, cmd, len, rsp, rsp_cap);
    if (r.len < 0) r.len = 0;
    return r;
}

void xcp_mock_tick(XcpMock* m, uint16_t event, uint32_t time_us)
{
    if (m) xcp_slave_tick(m->s, event, time_us);
}

/* 从从机队列取下一条，并应用丢包；返回长度，0 空 */
static int next_raw(XcpMock* m, uint8_t* dst, int cap)
{
    for (;;) {
        int n = xcp_slave_pop_dto(m->s, dst, cap);
        if (n <= 0) return n;
        m->st.dto_in++;
        if (m->df.drop_every > 0 && m->st.dto_in % (uint32_t)m->df.drop_every == 0) {
            m->st.dto_dropped++;
            continue;
        }
        return n;
    }
}

int xcp_mock_pop(XcpMock* m, uint8_t* dst, int cap)
{
    int n;
    if (!m || !dst) return 0;
    if (m->evn > 0) {
        n = m->evlen[0];
        if (n > cap) return -1;
        memcpy(dst, m->evq[0], (size_t)n);
        memmove(m->evq[0], m->evq[1], sizeof(m->evq[0]) * (size_t)(m->evn - 1));
        memmove(m->evlen, m->evlen + 1, sizeof(int) * (size_t)(m->evn - 1));
        m->evn--;
        return n;
    }
    if (m->dup_len) {
        n = m->dup_len;
        memcpy(dst, m->dupbuf, (size_t)n);
        m->dup_len = 0;
        m->st.dto_out++;
        return n;
    }
    n = next_raw(m, dst, cap);
    if (n <= 0) {
        /* 队列空：吐出暂扣的乱序包 */
        if (m->held_len) {
            n = m->held_len;
            memcpy(dst, m->held, (size_t)n);
            m->held_len = 0;
            m->st.dto_out++;
            return n;
        }
        return n;
    }
    if (m->held_len) {
        /* 先发后来的包（dst），再发暂扣的旧包 → 交换顺序 */
        uint8_t tmp[XCP_MAX_DTO];
        memcpy(tmp, m->held, (size_t)m->held_len);
        m->dup_len = m->held_len;
        memcpy(m->dupbuf, tmp, (size_t)m->held_len);
        m->held_len = 0;
        m->st.dto_out++;
        return n;
    }
    if (m->df.swap_every > 0 && m->st.dto_in % (uint32_t)m->df.swap_every == 0) {
        memcpy(m->held, dst, (size_t)n);
        m->held_len = n;
        m->st.dto_swapped++;
        return xcp_mock_pop(m, dst, cap);
    }
    if (m->df.dup_every > 0 && m->st.dto_in % (uint32_t)m->df.dup_every == 0) {
        memcpy(m->dupbuf, dst, (size_t)n);
        m->dup_len = n;
        m->st.dto_dup++;
    }
    m->st.dto_out++;
    return n;
}

void xcp_mock_stats(const XcpMock* m, XcpMockStats* out)
{
    if (m && out) *out = m->st;
}
