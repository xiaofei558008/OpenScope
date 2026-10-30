/* 兼容层：旧 XcpMaster API -> XcpSession。见 xcp_master.h。 */
#include "xcp_master.h"
#include "xcp_session.h"
#include "xcp_tl.h"

#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------ XcpIo -> XcpTl 适配（serial 等旧传输） */
#define IO_SLICE_MS 50

typedef struct IoTl {
    XcpTl base;
    XcpIo io;
    xcp_mutex_t tx;
    xcp_atomic_t stop;
} IoTl;

static int io_send(XcpTl* tl, const uint8_t* p, int n)
{
    IoTl* t = (IoTl*)tl;
    int r;
    if (xcp_atomic_load(&t->stop)) return XCP_TL_CLOSED;
    xcp_mutex_lock(&t->tx);
    r = t->io.send(t->io.ctx, p, n);
    xcp_mutex_unlock(&t->tx);
    if (r != n) return XCP_TL_ERROR;
    xcp_atomic_add(&tl->stats.tx_packets, 1);
    return 0;
}

static int io_recv(XcpTl* tl, uint8_t* p, int cap, int timeout_ms)
{
    IoTl* t = (IoTl*)tl;
    uint64_t deadline = xcp_now_ms() + (uint64_t)(timeout_ms > 0 ? timeout_ms : 0);
    for (;;) {
        uint64_t t0 = xcp_now_ms();
        int slice, r;
        if (xcp_atomic_load(&t->stop)) return XCP_TL_CLOSED;
        if (t0 >= deadline) return XCP_TL_TIMEOUT;
        slice = (int)(deadline - t0);
        if (slice > IO_SLICE_MS) slice = IO_SLICE_MS;
        r = t->io.recv(t->io.ctx, p, cap, slice);
        if (r > 0) { xcp_atomic_add(&tl->stats.rx_packets, 1); return r; }
        /* 旧接口无法区分“超时”和“出错”：都当作无数据。立即失败时让出 CPU，避免空转。 */
        if (xcp_now_ms() - t0 < 2) xcp_sleep_ms(5);
    }
}

static void io_shutdown(XcpTl* tl) { xcp_atomic_store(&((IoTl*)tl)->stop, 1); }
static void io_destroy(XcpTl* tl)
{
    IoTl* t = (IoTl*)tl;
    xcp_mutex_destroy(&t->tx);
    free(t);
}
static const XcpTlVtbl k_io = { "io", io_send, io_recv, io_shutdown, io_destroy };

static XcpTl* tl_from_io(const XcpIo* io)
{
    IoTl* t;
    if (!io || !io->send || !io->recv) return NULL;
    t = (IoTl*)calloc(1, sizeof(IoTl));
    if (!t) return NULL;
    t->base.v = &k_io;
    t->base.max_packet = 0xFFFF;
    t->io = *io;
    xcp_mutex_init(&t->tx);
    return &t->base;
}

/* ------------------------------------------------ DTO 兼容队列 */
#define DQ_SLOTS 256
#define DQ_SLOT  1024
typedef struct DtoQ {
    xcp_mutex_t mu;
    xcp_cond_t cv;
    uint8_t buf[DQ_SLOTS][DQ_SLOT];
    int len[DQ_SLOTS];
    int head, count;
    long dropped;
} DtoQ;

static void on_daq(void* user, const uint8_t* p, int n)
{
    DtoQ* q = (DtoQ*)user;
    int slot;
    if (n > DQ_SLOT) return;
    xcp_mutex_lock(&q->mu);
    if (q->count == DQ_SLOTS) { q->head = (q->head + 1) % DQ_SLOTS; q->count--; q->dropped++; }
    slot = (q->head + q->count) % DQ_SLOTS;
    memcpy(q->buf[slot], p, (size_t)n);
    q->len[slot] = n;
    q->count++;
    xcp_cond_broadcast(&q->cv);
    xcp_mutex_unlock(&q->mu);
}

/* ------------------------------------------------ 公共 */
static int map_rc(XcpMaster* m, int rc)
{
    m->last_rc = rc;
    if (rc == XCP_OK) { m->last_err = 0; return 0; }
    if (XCP_IS_SLAVE_ERR(rc)) { m->last_err = XCP_SLAVE_CODE(rc); return -2; }
    m->last_err = 0;
    return -1;
}

void xcp_master_init(XcpMaster* m, const XcpIo* io)
{
    memset(m, 0, sizeof(*m));
    if (io) m->io = *io;
    m->max_cto = 8;
    m->max_dto = 8;
    m->ag = 1;
    m->tl = tl_from_io(io);
}

void xcp_master_init_tl(XcpMaster* m, XcpTl* tl)
{
    memset(m, 0, sizeof(*m));
    m->max_cto = 8;
    m->max_dto = 8;
    m->ag = 1;
    m->tl = tl;
}

void xcp_master_free(XcpMaster* m)
{
    if (!m) return;
    if (m->s) xcp_session_destroy(m->s); /* 同时销毁其 tl */
    else if (m->tl) xcp_tl_destroy(m->tl);
    m->s = NULL;
    m->tl = NULL;
    if (m->dtoq) {
        DtoQ* q = (DtoQ*)m->dtoq;
        xcp_cond_destroy(&q->cv);
        xcp_mutex_destroy(&q->mu);
        free(q);
        m->dtoq = NULL;
    }
    m->connected = 0;
}

XcpSession* xcp_master_session(XcpMaster* m) { return m ? m->s : NULL; }
int xcp_master_link_ok(XcpMaster* m) { return m && m->s && xcp_session_link_ok(m->s); }

static int ensure_session(XcpMaster* m)
{
    XcpSessionCfg cfg;
    DtoQ* q;
    if (m->s) return 0;
    if (!m->tl) return -1;
    q = (DtoQ*)calloc(1, sizeof(DtoQ));
    if (!q) return -1;
    xcp_mutex_init(&q->mu);
    xcp_cond_init(&q->cv);
    memset(&cfg, 0, sizeof(cfg));
    cfg.tmo.t[1] = 1000; /* 与旧实现的 1000ms 固定超时一致 */
    cfg.on_daq = on_daq;
    cfg.user = q;
    if (xcp_session_create(&m->s, m->tl, &cfg) != XCP_OK) {
        xcp_cond_destroy(&q->cv);
        xcp_mutex_destroy(&q->mu);
        free(q);
        return -1;
    }
    m->tl = NULL;
    m->dtoq = q;
    return 0;
}

int xcp_master_connect(XcpMaster* m)
{
    XcpConnInfo ci;
    int rc;
    if (!m || ensure_session(m) != 0) return -1;
    rc = xcp_connect(m->s, 0, &ci);
    if (rc != XCP_OK) return map_rc(m, rc);
    m->resource = ci.resource;
    m->max_cto = (uint8_t)ci.max_cto;
    m->max_dto = ci.max_dto;
    m->byte_order = ci.byte_order;
    m->ag = ci.ag;
    m->connected = 1;
    m->last_err = 0;
    return 0;
}

int xcp_master_disconnect(XcpMaster* m)
{
    int rc;
    if (!m || !m->s) return -1;
    rc = xcp_disconnect(m->s);
    m->connected = 0;
    return map_rc(m, rc);
}

int xcp_master_upload(XcpMaster* m, uint32_t addr, void* dst, int n)
{
    uint8_t* out = (uint8_t*)dst;
    int got = 0, ag, per;
    if (!m || !m->s || !m->connected || n < 0 || (n > 0 && !dst)) return -1;
    ag = m->ag;
    if (n % ag) return -1;
    per = (m->max_cto - ag) / ag; /* SHORT_UPLOAD 每帧元素数（旧实现用 SHORT_UPLOAD，保持） */
    if (per > 255) per = 255;
    if (per <= 0) return -1;
    while (got < n) {
        int chunk = (n - got) / ag, rc;
        if (chunk > per) chunk = per;
        rc = xcp_short_upload(m->s, 0, addr + (uint32_t)(got / ag), (uint8_t)chunk, out + got);
        if (rc != XCP_OK) return map_rc(m, rc);
        got += chunk * ag;
    }
    m->last_err = 0;
    return 0;
}

int xcp_master_download(XcpMaster* m, uint32_t addr, const void* src, int n)
{
    if (!m || !m->s || !m->connected || n <= 0 || !src) return -1;
    if (n % m->ag) return -1;
    return map_rc(m, xcp_download(m->s, 0, addr, (uint32_t)(n / m->ag), src));
}

int xcp_master_checksum_ex(XcpMaster* m, uint32_t addr, uint32_t size, uint8_t* type, uint32_t* sum)
{
    if (!m || !m->s || !m->connected) return -1;
    return map_rc(m, xcp_build_checksum(m->s, 0, addr, size, type, sum, NULL));
}

int xcp_master_checksum(XcpMaster* m, uint32_t addr, uint32_t size, uint32_t* sum)
{
    uint8_t t = 0;
    uint32_t v = 0;
    int rc = xcp_master_checksum_ex(m, addr, size, &t, &v);
    if (rc != 0) return rc;
    if (t != XCP_CHECKSUM_ADD_14) return -1;
    if (sum) *sum = v;
    return 0;
}

/* 本仓库测试从机的密钥：key = seed XOR 0xFF（4 字节） */
static int test_key(void* u, uint8_t res, const uint8_t* seed, int len, uint8_t* key, int* klen)
{
    int i;
    (void)u; (void)res;
    if (len != 4 || *klen < 4) return -1;
    for (i = 0; i < 4; i++) key[i] = (uint8_t)(seed[i] ^ 0xFF);
    *klen = 4;
    return 0;
}

int xcp_master_unlock_cal(XcpMaster* m)
{
    int rc;
    if (!m || !m->s) return -1;
    xcp_set_seed_key(m->s, test_key, NULL);
    rc = xcp_unlock(m->s, XCP_RES_CAL_PAG);
    xcp_set_seed_key(m->s, NULL, NULL);
    return map_rc(m, rc);
}

static int cmd_ok(XcpMaster* m, const uint8_t* cmd, int n)
{
    if (!m || !m->s) return -1;
    return map_rc(m, xcp_command(m->s, cmd, n, NULL, 0, NULL, NULL));
}

int xcp_master_daq_free(XcpMaster* m)
{
    uint8_t cmd = XCP_CMD_FREE_DAQ;
    return cmd_ok(m, &cmd, 1);
}

int xcp_master_daq_alloc(XcpMaster* m, int n_daq, int n_odt, int n_entry)
{
    uint8_t cmd[8];
    int rc, d, o;
    rc = xcp_master_daq_free(m);
    if (rc != 0) return rc;
    cmd[0] = XCP_CMD_ALLOC_DAQ;
    cmd[1] = 0;
    xcp_s_wr16(m->s, cmd + 2, (uint16_t)n_daq);
    rc = cmd_ok(m, cmd, 4);
    if (rc != 0) return rc;
    for (d = 0; d < n_daq; d++) {
        cmd[0] = XCP_CMD_ALLOC_ODT;
        cmd[1] = 0;
        xcp_s_wr16(m->s, cmd + 2, (uint16_t)d);
        cmd[4] = (uint8_t)n_odt;
        rc = cmd_ok(m, cmd, 5);
        if (rc != 0) return rc;
        for (o = 0; o < n_odt; o++) {
            cmd[0] = XCP_CMD_ALLOC_ODT_ENTRY;
            cmd[1] = 0;
            xcp_s_wr16(m->s, cmd + 2, (uint16_t)d);
            cmd[4] = (uint8_t)o;
            cmd[5] = (uint8_t)n_entry;
            rc = cmd_ok(m, cmd, 6);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int xcp_master_daq_entry(XcpMaster* m, uint16_t daq, uint8_t odt, uint8_t entry,
                         uint32_t addr, uint8_t size)
{
    uint8_t cmd[8];
    int rc;
    if (!m || !m->s) return -1;
    cmd[0] = XCP_CMD_SET_DAQ_PTR;
    cmd[1] = 0;
    xcp_s_wr16(m->s, cmd + 2, daq);
    cmd[4] = odt;
    cmd[5] = entry;
    rc = cmd_ok(m, cmd, 6);
    if (rc != 0) return rc;
    cmd[0] = XCP_CMD_WRITE_DAQ;
    cmd[1] = 0xFF;
    cmd[2] = size;
    cmd[3] = 0;
    xcp_s_wr32(m->s, cmd + 4, addr);
    return cmd_ok(m, cmd, 8);
}

int xcp_master_daq_start(XcpMaster* m, uint16_t daq, uint16_t event)
{
    uint8_t cmd[8];
    int rc;
    if (!m || !m->s) return -1;
    cmd[0] = XCP_CMD_SET_DAQ_LIST_MODE;
    cmd[1] = XCP_DAQ_FLAG_TIMESTAMP;
    xcp_s_wr16(m->s, cmd + 2, daq);
    xcp_s_wr16(m->s, cmd + 4, event);
    cmd[6] = 1;
    cmd[7] = 0;
    rc = cmd_ok(m, cmd, 8);
    if (rc != 0) return rc;
    cmd[0] = XCP_CMD_START_STOP_DAQ_LIST;
    cmd[1] = XCP_DAQ_START_START;
    xcp_s_wr16(m->s, cmd + 2, daq);
    return cmd_ok(m, cmd, 4);
}

int xcp_master_recv_dto(XcpMaster* m, uint8_t* dst, int cap, int timeout_ms)
{
    DtoQ* q;
    int n = 0;
    uint64_t deadline;
    if (!m || !m->dtoq || !dst) return -1;
    q = (DtoQ*)m->dtoq;
    deadline = xcp_now_ms() + (uint64_t)(timeout_ms > 0 ? timeout_ms : 0);
    xcp_mutex_lock(&q->mu);
    while (q->count == 0) {
        uint64_t now = xcp_now_ms();
        if (now >= deadline) break;
        xcp_cond_wait_ms(&q->cv, &q->mu, (int)(deadline - now));
    }
    if (q->count > 0) {
        n = q->len[q->head];
        if (n > cap) n = -1;
        else memcpy(dst, q->buf[q->head], (size_t)n);
        q->head = (q->head + 1) % DQ_SLOTS;
        q->count--;
    }
    xcp_mutex_unlock(&q->mu);
    return n;
}
