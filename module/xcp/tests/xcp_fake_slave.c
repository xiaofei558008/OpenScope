#include "xcp_fake_slave.h"
#include "../xcp_plat.h"
#include "../xcp_proto.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FMEM 8192
#define FID_MAX 70000

struct FakeSlave {
    FakeCfg c;
    xcp_sock_t ls, cs;
    struct sockaddr_in peer;
    int have_peer;
    xcp_thread_t th;
    xcp_atomic_t stop;
    xcp_mutex_t mu;     /* 故障表、tx、计数 */
    uint16_t ctr;
    int drop[256], busy[256], errn[256], delay_n[256], delay_ms[256], pend_n[256], pend_ms[256];
    uint8_t err_code[256];
    int count[256];
    uint8_t mem[FMEM];
    uint8_t* id;
    int idlen;
    int mta_id;         /* MTA 指向 GET_ID 数据 */
    uint32_t mta;       /* 字节偏移 */
    uint8_t rb[70000];
    int rlen;
    xcp_atomic_t kill_client;
};

static void wr16(const FakeSlave* f, uint8_t* p, uint16_t v)
{
    if (f->c.byte_order) { p[0] = (uint8_t)(v >> 8); p[1] = (uint8_t)v; }
    else { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
}
static void wr32(const FakeSlave* f, uint8_t* p, uint32_t v)
{
    if (f->c.byte_order) { p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16); p[2] = (uint8_t)(v >> 8); p[3] = (uint8_t)v; }
    else { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24); }
}
static uint32_t rd32(const FakeSlave* f, const uint8_t* p)
{
    if (f->c.byte_order) return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

int fake_inject(FakeSlave* f, const uint8_t* pkt, int len)
{
    uint8_t buf[70000];
    int rc = -1;
    if (len <= 0 || len > 65535) return -1;
    xcp_mutex_lock(&f->mu);
    buf[0] = (uint8_t)len; buf[1] = (uint8_t)(len >> 8);
    buf[2] = (uint8_t)f->ctr; buf[3] = (uint8_t)(f->ctr >> 8);
    memcpy(buf + 4, pkt, (size_t)len);
    f->ctr++;
    if (f->c.udp) {
        if (f->have_peer)
            rc = (int)sendto(f->ls, (const char*)buf, len + 4, 0, (struct sockaddr*)&f->peer, sizeof(f->peer)) == len + 4 ? 0 : -1;
    } else if (f->cs != XCP_INVALID_SOCK) {
        int off = 0;
        rc = 0;
        while (off < len + 4) {
            int w = (int)send(f->cs, (const char*)buf + off, len + 4 - off, 0);
            if (w <= 0) { rc = -1; break; }
            off += w;
        }
    }
    xcp_mutex_unlock(&f->mu);
    return rc;
}

static void reply_err(FakeSlave* f, uint8_t code)
{
    uint8_t p[2];
    p[0] = XCP_PID_ERR; p[1] = code;
    fake_inject(f, p, 2);
}

static void send_data(FakeSlave* f, const uint8_t* src, uint32_t bytes)
{
    /* RES 帧：PID + (AG-1) 填充 + 数据；块模式时多帧 */
    uint8_t p[300];
    int ag = f->c.ag;
    uint32_t per = (uint32_t)((f->c.max_cto - ag) / ag) * (uint32_t)ag, off = 0;
    do {
        uint32_t n = bytes - off;
        if (n > per) n = per;
        memset(p, 0, (size_t)ag);
        p[0] = XCP_PID_RES;
        memcpy(p + ag, src + off, n);
        fake_inject(f, p, (int)(ag + n));
        off += n;
    } while (off < bytes);
}

static void handle(FakeSlave* f, const uint8_t* q, int n)
{
    uint8_t r[300];
    uint8_t cmd = q[0];
    int drop, busy, errn, dms = 0, pms = 0;
    uint8_t ecode = 0;
    xcp_mutex_lock(&f->mu);
    f->count[cmd]++;
    drop = f->drop[cmd] > 0 ? f->drop[cmd]-- : 0;
    busy = (!drop && f->busy[cmd] > 0) ? f->busy[cmd]-- : 0;
    errn = (!drop && !busy && f->errn[cmd] > 0) ? f->errn[cmd]-- : 0;
    ecode = f->err_code[cmd];
    if (!drop && !busy && !errn && f->delay_n[cmd] > 0) { f->delay_n[cmd]--; dms = f->delay_ms[cmd]; }
    if (!drop && !busy && !errn && f->pend_n[cmd] > 0) { f->pend_n[cmd]--; pms = f->pend_ms[cmd]; }
    xcp_mutex_unlock(&f->mu);
    if (drop) return;
    if (busy) { reply_err(f, XCP_ERR_CMD_BUSY); return; }
    if (errn) { reply_err(f, ecode); return; }
    if (dms) xcp_sleep_ms(dms);
    if (pms) {
        uint8_t ev[2];
        int left = pms;
        ev[0] = XCP_PID_EV; ev[1] = 0x05;
        while (left > 0) { /* 每 pms/3 发一次 CMD_PENDING */
            int st = pms / 3 > 0 ? pms / 3 : 1;
            fake_inject(f, ev, 2);
            xcp_sleep_ms(st);
            left -= st;
        }
    }
    memset(r, 0, sizeof(r));
    r[0] = XCP_PID_RES;
    switch (cmd) {
    case XCP_CMD_CONNECT: {
        int agb = f->c.ag == 4 ? 2 : (f->c.ag == 2 ? 1 : 0);
        r[1] = f->c.resource;
        r[2] = (uint8_t)((f->c.byte_order ? 1 : 0) | (agb << 1) | (f->c.block ? 0x40 : 0) | 0x80);
        r[3] = (uint8_t)f->c.max_cto;
        wr16(f, r + 4, (uint16_t)f->c.max_dto);
        r[6] = 0x01; r[7] = 0x01;
        fake_inject(f, r, 8);
        break;
    }
    case XCP_CMD_DISCONNECT: fake_inject(f, r, 1); break;
    case XCP_CMD_SYNCH: reply_err(f, XCP_ERR_CMD_SYNCH); break;
    case XCP_CMD_GET_STATUS:
        r[1] = f->c.session_status; r[2] = f->c.protection; r[3] = f->c.state_number;
        wr16(f, r + 4, f->c.session_cfg_id);
        fake_inject(f, r, 6);
        break;
    case XCP_CMD_GET_COMM_MODE_INFO:
        r[2] = 0x01; r[4] = 16; r[5] = 2; r[6] = 0; r[7] = 0x13;
        fake_inject(f, r, 8);
        break;
    case XCP_CMD_GET_ID: {
        static const char txt[] = "FAKE-XCP-ECU";
        uint8_t type = n >= 2 ? q[1] : 0;
        if (type == 0 || type == 1) {
            uint32_t L = (uint32_t)(type == 0 ? strlen(txt) : 8);
            r[1] = 1;
            wr32(f, r + 4, L);
            memcpy(r + 8, type == 0 ? txt : "fake_ecu", L);
            fake_inject(f, r, (int)(8 + L));
        } else if (type == 4) {
            r[1] = 0;
            wr32(f, r + 4, (uint32_t)f->idlen);
            f->mta_id = 1; f->mta = 0;
            fake_inject(f, r, 8);
        } else {
            wr32(f, r + 4, 0);
            fake_inject(f, r, 8);
        }
        break;
    }
    case XCP_CMD_SET_MTA:
        f->mta_id = 0;
        f->mta = rd32(f, q + 4) * (uint32_t)f->c.ag;
        fake_inject(f, r, 1);
        break;
    case XCP_CMD_UPLOAD: {
        uint32_t bytes = (uint32_t)q[1] * (uint32_t)f->c.ag;
        const uint8_t* src;
        uint32_t lim;
        if (!f->c.block && bytes > (uint32_t)(f->c.max_cto - f->c.ag)) { reply_err(f, XCP_ERR_OUT_OF_RANGE); break; }
        src = f->mta_id ? f->id : f->mem;
        lim = f->mta_id ? (uint32_t)f->idlen + 4u : FMEM;
        if (f->mta + bytes > lim) { reply_err(f, XCP_ERR_ACCESS_DENIED); break; }
        send_data(f, src + f->mta, bytes);
        f->mta += bytes;
        break;
    }
    case XCP_CMD_SHORT_UPLOAD: {
        uint32_t bytes = (uint32_t)q[1] * (uint32_t)f->c.ag;
        uint32_t off = rd32(f, q + 4) * (uint32_t)f->c.ag;
        if (bytes > (uint32_t)(f->c.max_cto - f->c.ag) || off + bytes > FMEM) { reply_err(f, XCP_ERR_OUT_OF_RANGE); break; }
        memcpy(r + f->c.ag, f->mem + off, bytes);
        fake_inject(f, r, (int)(f->c.ag + bytes));
        break;
    }
    default:
        reply_err(f, XCP_ERR_CMD_UNKNOWN);
        break;
    }
}

static void process_buf(FakeSlave* f)
{
    int off = 0;
    while (f->rlen - off >= 4) {
        int len = f->rb[off] | (f->rb[off + 1] << 8);
        if (f->rlen - off < 4 + len) break;
        if (len > 0) handle(f, f->rb + off + 4, len);
        off += 4 + len;
    }
    memmove(f->rb, f->rb + off, (size_t)(f->rlen - off));
    f->rlen -= off;
}

static void fake_thread(void* arg)
{
    FakeSlave* f = (FakeSlave*)arg;
    while (!xcp_atomic_load(&f->stop)) {
        if (f->c.udp) {
            struct sockaddr_in from;
            socklen_t fl = sizeof(from);
            int r;
            if (xcp_sock_wait_readable(f->ls, 50) <= 0) continue;
            r = (int)recvfrom(f->ls, (char*)f->rb, (int)sizeof(f->rb), 0, (struct sockaddr*)&from, &fl);
            if (r <= 0) continue;
            xcp_mutex_lock(&f->mu);
            f->peer = from; f->have_peer = 1;
            xcp_mutex_unlock(&f->mu);
            f->rlen = r;
            process_buf(f);
            f->rlen = 0;
        } else {
            if (f->cs == XCP_INVALID_SOCK) {
                xcp_sock_t c;
                if (xcp_sock_wait_readable(f->ls, 50) <= 0) continue;
                c = accept(f->ls, NULL, NULL);
                if (c == XCP_INVALID_SOCK) continue;
                { int one = 1; setsockopt(c, IPPROTO_TCP, TCP_NODELAY, (const char*)&one, sizeof(one)); }
                xcp_mutex_lock(&f->mu);
                f->cs = c;
                xcp_mutex_unlock(&f->mu);
                f->rlen = 0;
                continue;
            }
            if (xcp_atomic_load(&f->kill_client)) {
                xcp_mutex_lock(&f->mu);
                xcp_sock_close(f->cs);
                f->cs = XCP_INVALID_SOCK;
                xcp_mutex_unlock(&f->mu);
                xcp_atomic_store(&f->kill_client, 0);
                continue;
            }
            {
                int r = xcp_sock_wait_readable(f->cs, 50);
                if (r <= 0) continue;
                r = (int)recv(f->cs, (char*)f->rb + f->rlen, (int)sizeof(f->rb) - f->rlen, 0);
                if (r <= 0) {
                    xcp_mutex_lock(&f->mu);
                    xcp_sock_close(f->cs);
                    f->cs = XCP_INVALID_SOCK;
                    xcp_mutex_unlock(&f->mu);
                    continue;
                }
                f->rlen += r;
                process_buf(f);
            }
        }
    }
}

FakeSlave* fake_start(const FakeCfg* cfg, int* port_out)
{
    FakeSlave* f = (FakeSlave*)calloc(1, sizeof(FakeSlave));
    struct sockaddr_in a;
    socklen_t al = sizeof(a);
    int i;
    if (!f) return NULL;
    xcp_net_init();
    f->c = *cfg;
    if (!f->c.ag) f->c.ag = 1;
    if (!f->c.max_cto) f->c.max_cto = 8;
    if (!f->c.max_dto) f->c.max_dto = 8;
    f->cs = XCP_INVALID_SOCK;
    xcp_mutex_init(&f->mu);
    for (i = 0; i < FMEM; i++) f->mem[i] = (uint8_t)(i * 7 + 3);
    f->ls = socket(AF_INET, f->c.udp ? SOCK_DGRAM : SOCK_STREAM, 0);
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = htonl(0x7F000001);
    a.sin_port = 0;
    if (bind(f->ls, (struct sockaddr*)&a, sizeof(a)) != 0) goto fail;
    if (!f->c.udp && listen(f->ls, 1) != 0) goto fail;
    getsockname(f->ls, (struct sockaddr*)&a, &al);
    *port_out = ntohs(a.sin_port);
    if (xcp_thread_start(&f->th, fake_thread, f) != 0) goto fail;
    return f;
fail:
    xcp_sock_close(f->ls);
    free(f);
    return NULL;
}

void fake_stop(FakeSlave* f)
{
    if (!f) return;
    xcp_atomic_store(&f->stop, 1);
    xcp_thread_join(&f->th);
    xcp_sock_close(f->cs);
    xcp_sock_close(f->ls);
    xcp_mutex_destroy(&f->mu);
    free(f->id);
    free(f);
    xcp_net_cleanup();
}

void fake_close_client(FakeSlave* f) { xcp_atomic_store(&f->kill_client, 1); }

#define FSET(field, v) do { xcp_mutex_lock(&f->mu); field; xcp_mutex_unlock(&f->mu); } while (0)
void fake_drop(FakeSlave* f, uint8_t c, int n) { FSET(f->drop[c] = n, 0); }
void fake_busy(FakeSlave* f, uint8_t c, int n) { FSET(f->busy[c] = n, 0); }
void fake_err(FakeSlave* f, uint8_t c, uint8_t code, int n) { FSET((f->errn[c] = n, f->err_code[c] = code), 0); }
void fake_delay(FakeSlave* f, uint8_t c, int ms, int n) { FSET((f->delay_n[c] = n, f->delay_ms[c] = ms), 0); }
void fake_pending(FakeSlave* f, uint8_t c, int ms, int n) { FSET((f->pend_n[c] = n, f->pend_ms[c] = ms), 0); }
int fake_count(FakeSlave* f, uint8_t c) { int v; xcp_mutex_lock(&f->mu); v = f->count[c]; xcp_mutex_unlock(&f->mu); return v; }
void fake_set_id(FakeSlave* f, const uint8_t* d, int len)
{
    xcp_mutex_lock(&f->mu);
    free(f->id);
    f->id = (uint8_t*)calloc(1, (size_t)len + 8);
    memcpy(f->id, d, (size_t)len);
    f->idlen = len;
    xcp_mutex_unlock(&f->mu);
}
void fake_set_mem(FakeSlave* f, uint32_t off, const uint8_t* d, int len)
{
    xcp_mutex_lock(&f->mu);
    if (off + (uint32_t)len <= FMEM) memcpy(f->mem + off, d, (size_t)len);
    xcp_mutex_unlock(&f->mu);
}
