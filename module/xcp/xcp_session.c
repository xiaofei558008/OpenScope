/* XCP 主机协议核心：会话 + RX 线程 + 命令引擎。参见 xcp_session.h。 */
#include "xcp_session.h"
#include "xcp_proto.h"
#include <stdlib.h>
#include <string.h>

#define RX_BUF      65536
#define RX_SLICE_MS 200
#define SMALL_RSP   512
#define ID_MAX_LEN  (16u * 1024u * 1024u)

enum { PR_NONE = 0, PR_RES, PR_ERR, PR_LINK, PR_CLOSED };

struct XcpSession {
    XcpTl* tl;
    XcpSessionCfg cfg;
    xcp_thread_t rx;
    xcp_mutex_t cmd_lock;      /* 串行化命令（单命令在途） */
    xcp_mutex_t st;            /* 保护 pend_* / info */
    xcp_cond_t  cv;

    /* 在途命令（st 保护） */
    int      pend_active;
    int      pend_synch;       /* 等 ERR_CMD_SYNCH，期间丢弃其他 RES/ERR（迟到应答） */
    int      pend_state;       /* PR_* */
    int      pend_tmo_ms;
    uint64_t pend_deadline;
    uint8_t* pend_buf;
    int      pend_cap, pend_len;
    int      pend_frames, pend_got, pend_strip;
    uint8_t  pend_err;

    XcpConnInfo info;          /* st 保护 */
    xcp_atomic_t connected;
    xcp_atomic_t link_dead;
    xcp_atomic_t stopping;
    int need_synch;            /* cmd_lock 保护：上次失败后链路可能失步 */
    int last_err;              /* cmd_lock 保护 */

    xcp_atomic_t n_res, n_err, n_ev, n_serv, n_daq, n_stray;
    xcp_atomic_t n_tmo, n_synch, n_retry, n_busy, n_pending;
    uint8_t* rxbuf;
};

/* ------------------------------------------------------------------ 字节序 */
uint16_t xcp_s_rd16(const XcpSession* s, const uint8_t* p)
{
    if (s && s->info.byte_order) return (uint16_t)((p[0] << 8) | p[1]);
    return (uint16_t)(p[0] | (p[1] << 8));
}
uint32_t xcp_s_rd32(const XcpSession* s, const uint8_t* p)
{
    if (s && s->info.byte_order)
        return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
void xcp_s_wr16(const XcpSession* s, uint8_t* p, uint16_t v)
{
    if (s && s->info.byte_order) { p[0] = (uint8_t)(v >> 8); p[1] = (uint8_t)v; }
    else { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
}
void xcp_s_wr32(const XcpSession* s, uint8_t* p, uint32_t v)
{
    if (s && s->info.byte_order) {
        p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16); p[2] = (uint8_t)(v >> 8); p[3] = (uint8_t)v;
    } else {
        p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
    }
}

const char* xcp_strerror(int rc)
{
    if (rc == XCP_OK) return "ok";
    if (XCP_IS_SLAVE_ERR(rc)) {
        switch (XCP_SLAVE_CODE(rc)) {
        case 0x10: return "ERR_CMD_BUSY";
        case 0x11: return "ERR_DAQ_ACTIVE";
        case 0x12: return "ERR_PGM_ACTIVE";
        case 0x20: return "ERR_CMD_UNKNOWN";
        case 0x21: return "ERR_CMD_SYNTAX";
        case 0x22: return "ERR_OUT_OF_RANGE";
        case 0x23: return "ERR_WRITE_PROTECTED";
        case 0x24: return "ERR_ACCESS_DENIED";
        case 0x25: return "ERR_ACCESS_LOCKED";
        case 0x26: return "ERR_PAGE_NOT_VALID";
        case 0x27: return "ERR_MODE_NOT_VALID";
        case 0x28: return "ERR_SEGMENT_NOT_VALID";
        case 0x29: return "ERR_SEQUENCE";
        case 0x2A: return "ERR_DAQ_CONFIG";
        case 0x30: return "ERR_MEMORY_OVERFLOW";
        case 0x31: return "ERR_GENERIC";
        case 0x32: return "ERR_VERIFY";
        case 0x33: return "ERR_RESOURCE_TEMPORARY_NOT_ACCESSIBLE";
        case 0x34: return "ERR_SUBCMD_UNKNOWN";
        default: return "ERR_(slave)";
        }
    }
    switch (rc) {
    case XCP_E_TIMEOUT: return "timeout";
    case XCP_E_LINK: return "link lost";
    case XCP_E_PROTO: return "protocol error";
    case XCP_E_ARG: return "bad argument";
    case XCP_E_NOTCONN: return "not connected";
    case XCP_E_CLOSED: return "session closing";
    case XCP_E_WOULDBLOCK: return "command from RX callback";
    case XCP_E_NOMEM: return "out of memory";
    default: return "error";
    }
}

/* ------------------------------------------------------------------ RX 线程 */
static void wake_pending(XcpSession* s, int state)
{
    xcp_mutex_lock(&s->st);
    if (s->pend_active && s->pend_state == PR_NONE) s->pend_state = state;
    xcp_cond_broadcast(&s->cv);
    xcp_mutex_unlock(&s->st);
}

static void on_cto_response(XcpSession* s, const uint8_t* p, int n)
{
    int is_err = p[0] == XCP_PID_ERR;
    xcp_atomic_add(is_err ? &s->n_err : &s->n_res, 1);
    xcp_mutex_lock(&s->st);
    if (!s->pend_active || s->pend_state != PR_NONE) {
        xcp_atomic_add(&s->n_stray, 1);
    } else if (s->pend_synch) {
        if (is_err && n >= 2 && p[1] == XCP_ERR_CMD_SYNCH) {
            s->pend_state = PR_RES;
            s->pend_len = 0;
        } else if (is_err && n >= 2 && p[1] == XCP_ERR_CMD_UNKNOWN) {
            s->pend_err = p[1];
            s->pend_state = PR_ERR;
        } else {
            xcp_atomic_add(&s->n_stray, 1); /* 被超时放弃的命令的迟到应答 */
        }
    } else if (is_err) {
        s->pend_err = n >= 2 ? p[1] : XCP_ERR_GENERIC;
        s->pend_state = PR_ERR;
    } else if (s->pend_frames <= 1 && s->pend_strip == 0) {
        int c = n < s->pend_cap ? n : s->pend_cap;
        if (c > 0) memcpy(s->pend_buf, p, (size_t)c);
        s->pend_len = n; /* 真实长度，调用方可检测截断 */
        s->pend_state = PR_RES;
    } else {
        int pay = n - s->pend_strip, room = s->pend_cap - s->pend_len;
        if (pay > room) pay = room;
        if (pay > 0) { memcpy(s->pend_buf + s->pend_len, p + s->pend_strip, (size_t)pay); s->pend_len += pay; }
        s->pend_got++;
        s->pend_deadline = xcp_now_ms() + (uint64_t)s->pend_tmo_ms; /* 每收一帧重置计时 */
        if (s->pend_got >= s->pend_frames || s->pend_len >= s->pend_cap) s->pend_state = PR_RES;
    }
    if (s->pend_state != PR_NONE) xcp_cond_broadcast(&s->cv);
    xcp_mutex_unlock(&s->st);
}

static void rx_thread(void* arg)
{
    XcpSession* s = (XcpSession*)arg;
    int reason = XCP_E_LINK;
    for (;;) {
        int n;
        if (xcp_atomic_load(&s->stopping)) { reason = XCP_E_CLOSED; break; }
        n = xcp_tl_recv(s->tl, s->rxbuf, RX_BUF, RX_SLICE_MS);
        if (n == XCP_TL_TIMEOUT || n == XCP_TL_TOOBIG) continue;
        if (n < 0) {
            reason = xcp_atomic_load(&s->stopping) ? XCP_E_CLOSED : XCP_E_LINK;
            break;
        }
        switch (s->rxbuf[0]) {
        case XCP_PID_RES:
        case XCP_PID_ERR:
            on_cto_response(s, s->rxbuf, n);
            break;
        case XCP_PID_EV:
            xcp_atomic_add(&s->n_ev, 1);
            if (n >= 2 && s->rxbuf[1] == XCP_EV_CMD_PENDING) {
                xcp_atomic_add(&s->n_pending, 1);
                xcp_mutex_lock(&s->st);
                if (s->pend_active) s->pend_deadline = xcp_now_ms() + (uint64_t)s->pend_tmo_ms;
                xcp_mutex_unlock(&s->st);
            } else if (n >= 2 && s->rxbuf[1] == XCP_EV_SESSION_TERMINATED) {
                xcp_atomic_store(&s->connected, 0);
            }
            if (s->cfg.on_event) s->cfg.on_event(s->cfg.user, s->rxbuf, n);
            break;
        case XCP_PID_SERV:
            xcp_atomic_add(&s->n_serv, 1);
            if (s->cfg.on_serv) s->cfg.on_serv(s->cfg.user, s->rxbuf, n);
            break;
        default: /* 0x00..0xFB：DAQ */
            xcp_atomic_add(&s->n_daq, 1);
            if (s->cfg.on_daq) s->cfg.on_daq(s->cfg.user, s->rxbuf, n);
            break;
        }
    }
    xcp_atomic_store(&s->link_dead, 1);
    xcp_atomic_store(&s->connected, 0);
    wake_pending(s, reason == XCP_E_CLOSED ? PR_CLOSED : PR_LINK);
    if (reason == XCP_E_LINK && s->cfg.on_link_lost) s->cfg.on_link_lost(s->cfg.user, reason);
}

/* ------------------------------------------------------------------ 生命周期 */
int xcp_session_create(XcpSession** out, XcpTl* tl, const XcpSessionCfg* cfg)
{
    static const int def_tmo[8] = { 0, 2000, 5000, 5000, 5000, 5000, 2000, 200 };
    XcpSession* s;
    int i;
    if (!out || !tl) return XCP_E_ARG;
    *out = NULL;
    s = (XcpSession*)calloc(1, sizeof(XcpSession));
    if (!s) return XCP_E_NOMEM;
    s->rxbuf = (uint8_t*)malloc(RX_BUF);
    if (!s->rxbuf) { free(s); return XCP_E_NOMEM; }
    if (cfg) s->cfg = *cfg;
    for (i = 1; i < 8; i++) if (s->cfg.tmo.t[i] <= 0) s->cfg.tmo.t[i] = def_tmo[i];
    if (s->cfg.busy_retries <= 0) s->cfg.busy_retries = 50;
    if (s->cfg.temp_na_retries <= 0) s->cfg.temp_na_retries = 3;
    if (s->cfg.connect_retries <= 0) s->cfg.connect_retries = 2;
    s->tl = tl;
    s->info.ag = 1;
    s->info.max_cto = 8;
    s->info.max_dto = 8;
    xcp_mutex_init(&s->cmd_lock);
    xcp_mutex_init(&s->st);
    xcp_cond_init(&s->cv);
    if (xcp_thread_start(&s->rx, rx_thread, s) != 0) {
        xcp_cond_destroy(&s->cv);
        xcp_mutex_destroy(&s->st);
        xcp_mutex_destroy(&s->cmd_lock);
        free(s->rxbuf);
        free(s);
        return XCP_E_NOMEM;
    }
    *out = s;
    return XCP_OK;
}

void xcp_session_destroy(XcpSession* s)
{
    if (!s) return;
    xcp_atomic_store(&s->stopping, 1);
    xcp_tl_shutdown(s->tl);
    wake_pending(s, PR_CLOSED);
    xcp_thread_join(&s->rx);
    /* 等其他线程上的在途命令退出（它们已被唤醒为 CLOSED） */
    xcp_mutex_lock(&s->cmd_lock);
    xcp_mutex_unlock(&s->cmd_lock);
    xcp_tl_destroy(s->tl);
    xcp_cond_destroy(&s->cv);
    xcp_mutex_destroy(&s->st);
    xcp_mutex_destroy(&s->cmd_lock);
    free(s->rxbuf);
    free(s);
}

int xcp_session_connected(XcpSession* s) { return s && xcp_atomic_load(&s->connected) != 0; }
int xcp_session_link_ok(XcpSession* s) { return s && xcp_atomic_load(&s->link_dead) == 0; }
void xcp_session_info(XcpSession* s, XcpConnInfo* out)
{
    xcp_mutex_lock(&s->st);
    *out = s->info;
    xcp_mutex_unlock(&s->st);
}
int xcp_session_last_slave_err(XcpSession* s)
{
    int e;
    xcp_mutex_lock(&s->cmd_lock);
    e = s->last_err;
    xcp_mutex_unlock(&s->cmd_lock);
    return e;
}
void xcp_session_stats(XcpSession* s, XcpSessionStats* o)
{
    memset(o, 0, sizeof(*o));
    o->rx_res = xcp_atomic_load(&s->n_res);
    o->rx_err = xcp_atomic_load(&s->n_err);
    o->rx_ev = xcp_atomic_load(&s->n_ev);
    o->rx_serv = xcp_atomic_load(&s->n_serv);
    o->rx_daq = xcp_atomic_load(&s->n_daq);
    o->rx_stray = xcp_atomic_load(&s->n_stray);
    o->timeouts = xcp_atomic_load(&s->n_tmo);
    o->synchs = xcp_atomic_load(&s->n_synch);
    o->retries = xcp_atomic_load(&s->n_retry);
    o->busy_waits = xcp_atomic_load(&s->n_busy);
    o->cmd_pending = xcp_atomic_load(&s->n_pending);
    o->tl_ctr_gaps = xcp_atomic_load(&s->tl->stats.rx_ctr_gaps);
    o->tl_bad_frames = xcp_atomic_load(&s->tl->stats.rx_bad_frames);
    o->tl_tx = xcp_atomic_load(&s->tl->stats.tx_packets);
    o->tl_rx = xcp_atomic_load(&s->tl->stats.rx_packets);
}

/* ------------------------------------------------------------------ 命令引擎 */
/* 一次收发（不含重试）。返回 XCP_OK / XCP_E_TIMEOUT / XCP_E_LINK / XCP_E_CLOSED / XCP_E_SLAVE(x) */
static int exchange(XcpSession* s, const uint8_t* cmd, int len, uint8_t* buf, int cap,
                    int* out_len, int frames, int strip, int tmo_ms, int synch)
{
    int rc, state;
    if (xcp_atomic_load(&s->stopping)) return XCP_E_CLOSED;
    if (xcp_atomic_load(&s->link_dead)) return XCP_E_LINK;
    xcp_mutex_lock(&s->st);
    s->pend_active = 1;
    s->pend_synch = synch;
    s->pend_state = PR_NONE;
    s->pend_buf = buf;
    s->pend_cap = cap;
    s->pend_len = 0;
    s->pend_frames = frames < 1 ? 1 : frames;
    s->pend_got = 0;
    s->pend_strip = strip;
    s->pend_tmo_ms = tmo_ms;
    s->pend_deadline = xcp_now_ms() + (uint64_t)tmo_ms; /* 发送前开始计时，避免极快应答竞争 */
    xcp_mutex_unlock(&s->st);

    rc = xcp_tl_send(s->tl, cmd, len);

    xcp_mutex_lock(&s->st);
    if (rc != 0 && s->pend_state == PR_NONE) s->pend_state = PR_LINK;
    while (s->pend_state == PR_NONE) {
        uint64_t now = xcp_now_ms();
        if (now >= s->pend_deadline) break;
        xcp_cond_wait_ms(&s->cv, &s->st, (int)(s->pend_deadline - now));
    }
    state = s->pend_state;
    if (out_len) *out_len = s->pend_len;
    s->pend_active = 0;
    s->pend_buf = NULL;
    rc = XCP_E_TIMEOUT;
    if (state == PR_RES) rc = XCP_OK;
    else if (state == PR_ERR) { rc = XCP_E_SLAVE(s->pend_err); }
    else if (state == PR_LINK) rc = XCP_E_LINK;
    else if (state == PR_CLOSED) rc = XCP_E_CLOSED;
    xcp_mutex_unlock(&s->st);
    if (rc == XCP_E_LINK && xcp_atomic_load(&s->stopping)) rc = XCP_E_CLOSED;
    return rc;
}

static int default_tmo_index(uint8_t cmd, const uint8_t* p, int len)
{
    switch (cmd) {
    case XCP_CMD_CONNECT: return (len >= 2 && p[1] == 1) ? 6 : 1; /* USER_DEFINED -> t6 */
    case XCP_CMD_BUILD_CHECKSUM: return 2;
    case XCP_CMD_PROGRAM_START: return 3;
    case XCP_CMD_PROGRAM_CLEAR: return 4;
    case XCP_CMD_PROGRAM:
    case XCP_CMD_PROGRAM_NEXT:
    case XCP_CMD_PROGRAM_MAX:
    case XCP_CMD_PROGRAM_RESET:
    case XCP_CMD_PROGRAM_VERIFY: return 5;
    case XCP_CMD_PROGRAM_FORMAT:
    case XCP_CMD_PROGRAM_PREPARE: return 1;
    default: return 1;
    }
}

/* ERR_PGM_ACTIVE 按表 217..221 “wait t7 repeat ∞” 的命令 */
static int pgm_active_waits(uint8_t cmd)
{
    return cmd == XCP_CMD_DISCONNECT || cmd == XCP_CMD_SET_REQUEST || cmd == XCP_CMD_GET_SEED ||
           cmd == XCP_CMD_UNLOCK || cmd == XCP_CMD_SET_MTA || cmd == XCP_CMD_UPLOAD ||
           cmd == XCP_CMD_SHORT_UPLOAD || cmd == XCP_CMD_BUILD_CHECKSUM;
}

static int synch_locked(XcpSession* s)
{
    uint8_t c = XCP_CMD_SYNCH;
    int i, rc = XCP_E_TIMEOUT;
    for (i = 0; i < 3; i++) { /* 首次 + 重复 2 次 */
        rc = exchange(s, &c, 1, NULL, 0, NULL, 1, 0, s->cfg.tmo.t[1], 1);
        xcp_atomic_add(&s->n_synch, 1);
        if (rc != XCP_E_TIMEOUT) break;
        xcp_atomic_add(&s->n_tmo, 1);
    }
    if (rc == XCP_OK) s->need_synch = 0;
    return rc;
}

static int exec_locked(XcpSession* s, const uint8_t* cmd, int len, uint8_t* rsp, int cap,
                       int* rsp_len, const XcpCmdOpts* opts)
{
    uint8_t code = cmd[0];
    int tmo_idx = (opts && opts->tmo_index >= 1 && opts->tmo_index <= 7) ? opts->tmo_index
                                                                          : default_tmo_index(code, cmd, len);
    int tmo = s->cfg.tmo.t[tmo_idx];
    int frames = opts ? opts->frames : 1, strip = opts ? opts->strip : 0;
    int n_tmo = 0, n_busy = 0, n_tna = 0, rc, rl = 0;
    uint8_t small[SMALL_RSP];
    if (!rsp) { rsp = small; cap = (int)sizeof(small); }

    if (code == XCP_CMD_SYNCH) return synch_locked(s);
    if (s->need_synch && code != XCP_CMD_CONNECT) {
        rc = synch_locked(s);
        if (rc == XCP_E_TIMEOUT || rc == XCP_E_LINK || rc == XCP_E_CLOSED) return rc;
        s->need_synch = 0; /* ERR_CMD_UNKNOWN 等：从机仍在应答，视为同步 */
    }
    for (;;) {
        rc = exchange(s, cmd, len, rsp, cap, &rl, frames, strip, tmo, 0);
        if (rc == XCP_OK) {
            if (rsp_len) *rsp_len = rl;
            s->last_err = 0;
            return XCP_OK;
        }
        if (rc == XCP_E_LINK || rc == XCP_E_CLOSED) return rc;
        if (rc == XCP_E_TIMEOUT) {
            xcp_atomic_add(&s->n_tmo, 1);
            if (code == XCP_CMD_CONNECT) { /* 预动作无，repeat ∞（有上限） */
                if (++n_tmo > s->cfg.connect_retries) return XCP_E_TIMEOUT;
                if (cmd[1] == 1) xcp_sleep_ms(s->cfg.tmo.t[7]); /* USER_DEFINED：wait t7 */
                xcp_atomic_add(&s->n_retry, 1);
                continue;
            }
            if (++n_tmo > 2) { s->need_synch = 1; return XCP_E_TIMEOUT; }
            rc = synch_locked(s); /* 预动作 SYNCH */
            if (rc == XCP_E_TIMEOUT) { s->need_synch = 1; return XCP_E_TIMEOUT; }
            if (rc == XCP_E_LINK || rc == XCP_E_CLOSED) return rc;
            if (opts && opts->pre) {
                rc = opts->pre(s, opts->pre_user);
                if (rc != XCP_OK) return rc;
            }
            xcp_atomic_add(&s->n_retry, 1);
            continue;
        }
        /* 从机 ERR */
        s->last_err = XCP_SLAVE_CODE(rc);
        if (s->last_err == XCP_ERR_CMD_BUSY ||
            (s->last_err == XCP_ERR_PGM_ACTIVE && pgm_active_waits(code))) {
            if (++n_busy > s->cfg.busy_retries) return rc;
            xcp_atomic_add(&s->n_busy, 1);
            xcp_sleep_ms(s->cfg.tmo.t[7]);
            continue;
        }
        if (s->last_err == XCP_ERR_RES_TEMP_NOT_ACC) {
            if (++n_tna > s->cfg.temp_na_retries) return rc;
            xcp_atomic_add(&s->n_retry, 1);
            xcp_sleep_ms(s->cfg.tmo.t[7]);
            continue;
        }
        return rc; /* 其余错误交调用方（显示错误 / 换参数 / Seed&Key 等） */
    }
}

static int enter(XcpSession* s)
{
    if (!s) return XCP_E_ARG;
    if (xcp_thread_is_self(&s->rx)) return XCP_E_WOULDBLOCK;
    if (xcp_atomic_load(&s->stopping)) return XCP_E_CLOSED;
    xcp_mutex_lock(&s->cmd_lock);
    return XCP_OK;
}
static void leave(XcpSession* s) { xcp_mutex_unlock(&s->cmd_lock); }

int xcp_command(XcpSession* s, const uint8_t* cmd, int len,
                uint8_t* rsp, int cap, int* rsp_len, const XcpCmdOpts* opts)
{
    int rc;
    if (!cmd || len < 1 || cap < 0) return XCP_E_ARG;
    rc = enter(s);
    if (rc) return rc;
    rc = exec_locked(s, cmd, len, rsp, cap, rsp_len, opts);
    leave(s);
    return rc;
}

/* ------------------------------------------------------------------ 标准命令 */
int xcp_connect(XcpSession* s, uint8_t mode, XcpConnInfo* out)
{
    uint8_t cmd[2], rsp[SMALL_RSP];
    int rc, n = 0;
    XcpConnInfo ci;
    cmd[0] = XCP_CMD_CONNECT;
    cmd[1] = mode;
    rc = enter(s);
    if (rc) return rc;
    rc = exec_locked(s, cmd, 2, rsp, (int)sizeof(rsp), &n, NULL);
    if (rc == XCP_OK) {
        if (n < 8) { rc = XCP_E_PROTO; goto done; }
        memset(&ci, 0, sizeof(ci));
        ci.resource = rsp[1];
        ci.comm_mode_basic = rsp[2];
        ci.byte_order = (uint8_t)(rsp[2] & 0x01);
        switch ((rsp[2] >> 1) & 0x03) {
        case 0: ci.ag = 1; break;
        case 1: ci.ag = 2; break;
        case 2: ci.ag = 4; break;
        default: rc = XCP_E_PROTO; goto done;
        }
        ci.slave_block_mode = (uint8_t)((rsp[2] >> 6) & 1);
        ci.optional = (uint8_t)((rsp[2] >> 7) & 1);
        ci.max_cto = rsp[3];
        ci.max_dto = ci.byte_order ? (uint16_t)((rsp[4] << 8) | rsp[5]) : (uint16_t)(rsp[4] | (rsp[5] << 8));
        ci.protocol_ver = rsp[6];
        ci.transport_ver = rsp[7];
        if (ci.max_cto < 8 || ci.max_dto < 8 || (ci.max_cto % ci.ag) != 0 || (ci.max_dto % ci.ag) != 0) {
            rc = XCP_E_PROTO;
            goto done;
        }
        xcp_mutex_lock(&s->st);
        s->info = ci;
        xcp_mutex_unlock(&s->st);
        xcp_atomic_store(&s->connected, 1);
        s->need_synch = 0;
        if (out) *out = ci;
    }
done:
    leave(s);
    return rc;
}

int xcp_disconnect(XcpSession* s)
{
    uint8_t c = XCP_CMD_DISCONNECT;
    int rc = xcp_command(s, &c, 1, NULL, 0, NULL, NULL);
    if (rc == XCP_OK || rc == XCP_E_LINK) xcp_atomic_store(&s->connected, 0);
    return rc;
}

int xcp_synch(XcpSession* s)
{
    uint8_t c = XCP_CMD_SYNCH;
    return xcp_command(s, &c, 1, NULL, 0, NULL, NULL);
}

int xcp_get_status(XcpSession* s, XcpStatus* out)
{
    uint8_t c = XCP_CMD_GET_STATUS, rsp[SMALL_RSP];
    int n = 0, rc = xcp_command(s, &c, 1, rsp, (int)sizeof(rsp), &n, NULL);
    if (rc != XCP_OK) return rc;
    if (n < 6) return XCP_E_PROTO;
    if (out) {
        out->session_status = rsp[1];
        out->protection = rsp[2];
        out->state_number = rsp[3];
        out->session_cfg_id = xcp_s_rd16(s, rsp + 4);
    }
    return XCP_OK;
}

int xcp_get_comm_mode_info(XcpSession* s, XcpCommModeInfo* out)
{
    uint8_t c = XCP_CMD_GET_COMM_MODE_INFO, rsp[SMALL_RSP];
    int n = 0, rc = xcp_command(s, &c, 1, rsp, (int)sizeof(rsp), &n, NULL);
    if (rc != XCP_OK) return rc;
    if (n < 8) return XCP_E_PROTO;
    if (out) {
        out->optional = rsp[2];
        out->max_bs = rsp[4];
        out->min_st = rsp[5];
        out->queue_size = rsp[6];
        out->driver_version = rsp[7];
    }
    return XCP_OK;
}

static int set_mta_locked(XcpSession* s, uint8_t ext, uint32_t addr)
{
    uint8_t cmd[8];
    cmd[0] = XCP_CMD_SET_MTA;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = ext;
    xcp_s_wr32(s, cmd + 4, addr);
    return exec_locked(s, cmd, 8, NULL, 0, NULL, NULL);
}

int xcp_set_mta(XcpSession* s, uint8_t ext, uint32_t addr)
{
    int rc = enter(s);
    if (rc) return rc;
    rc = set_mta_locked(s, ext, addr);
    leave(s);
    return rc;
}

typedef struct MtaPre { uint8_t ext; uint32_t addr; } MtaPre;
static int pre_set_mta(XcpSession* s, void* u)
{
    MtaPre* m = (MtaPre*)u;
    return set_mta_locked(s, m->ext, m->addr);
}

/* 从当前 MTA 上传 n 个 AG 元素。mp 非空时超时预动作为 SYNCH + SET_MTA(当前块地址)。 */
static int upload_locked(XcpSession* s, uint32_t n, uint8_t* dst, MtaPre* mp)
{
    XcpConnInfo ci = s->info;
    uint32_t ag = ci.ag, per_frame, done = 0;
    if (!xcp_atomic_load(&s->connected)) return XCP_E_NOTCONN;
    per_frame = (ci.max_cto - ag) / ag; /* 每帧元素数：PID + (AG-1) 对齐填充 */
    if (per_frame == 0) return XCP_E_PROTO;
    while (done < n) {
        uint32_t chunk = n - done, frames;
        uint8_t cmd[2];
        XcpCmdOpts o;
        int rc, got = 0;
        if (ci.slave_block_mode) { if (chunk > 255) chunk = 255; }
        else if (chunk > per_frame) chunk = per_frame;
        frames = (chunk + per_frame - 1) / per_frame;
        cmd[0] = XCP_CMD_UPLOAD;
        cmd[1] = (uint8_t)chunk;
        memset(&o, 0, sizeof(o));
        o.frames = (int)frames;
        o.strip = (int)ag;
        if (mp) { o.pre = pre_set_mta; o.pre_user = mp; }
        rc = exec_locked(s, cmd, 2, dst + done * ag, (int)(chunk * ag), &got, &o);
        if (rc != XCP_OK) return rc;
        if ((uint32_t)got < chunk * ag) return XCP_E_PROTO;
        done += chunk;
        if (mp) mp->addr += chunk; /* 下一块的地址（单位：AG 元素） */
    }
    return XCP_OK;
}

int xcp_upload(XcpSession* s, uint8_t ext, uint32_t addr, uint32_t n, void* dst)
{
    MtaPre mp;
    int rc;
    if (!dst && n) return XCP_E_ARG;
    rc = enter(s);
    if (rc) return rc;
    mp.ext = ext;
    mp.addr = addr;
    rc = set_mta_locked(s, ext, addr);
    if (rc == XCP_OK) rc = upload_locked(s, n, (uint8_t*)dst, &mp);
    leave(s);
    return rc;
}

int xcp_upload_at_mta(XcpSession* s, uint32_t n, void* dst)
{
    int rc;
    if (!dst && n) return XCP_E_ARG;
    rc = enter(s);
    if (rc) return rc;
    rc = upload_locked(s, n, (uint8_t*)dst, NULL);
    leave(s);
    return rc;
}

int xcp_short_upload(XcpSession* s, uint8_t ext, uint32_t addr, uint8_t n, void* dst)
{
    uint8_t cmd[8], rsp[SMALL_RSP];
    int rc, got = 0;
    uint32_t ag;
    XcpConnInfo ci;
    if (!dst && n) return XCP_E_ARG;
    rc = enter(s);
    if (rc) return rc;
    ci = s->info;
    ag = ci.ag;
    if (!xcp_atomic_load(&s->connected)) { rc = XCP_E_NOTCONN; goto out; }
    if ((uint32_t)n * ag > (uint32_t)ci.max_cto - ag) { rc = XCP_E_ARG; goto out; }
    cmd[0] = XCP_CMD_SHORT_UPLOAD;
    cmd[1] = n;
    cmd[2] = 0;
    cmd[3] = ext;
    xcp_s_wr32(s, cmd + 4, addr);
    rc = exec_locked(s, cmd, 8, rsp, (int)sizeof(rsp), &got, NULL);
    if (rc == XCP_OK) {
        if ((uint32_t)got < ag + (uint32_t)n * ag) rc = XCP_E_PROTO;
        else memcpy(dst, rsp + ag, (size_t)n * ag);
    }
out:
    leave(s);
    return rc;
}

void xcp_id_free(XcpId* id)
{
    if (!id) return;
    free(id->data);
    id->data = NULL;
    id->len = 0;
}

int xcp_get_id(XcpSession* s, uint8_t type, XcpId* out)
{
    uint8_t cmd[2], rsp[SMALL_RSP];
    int rc, n = 0, attempt;
    uint32_t len, ag;
    if (!out) return XCP_E_ARG;
    memset(out, 0, sizeof(*out));
    rc = enter(s);
    if (rc) return rc;
    ag = s->info.ag;
    /* 上传阶段超时：MTA 由从机设置，主机无法单独恢复，故整体重做 GET_ID（共 3 次） */
    for (attempt = 0; attempt < 3; attempt++) {
        cmd[0] = XCP_CMD_GET_ID;
        cmd[1] = type;
        rc = exec_locked(s, cmd, 2, rsp, (int)sizeof(rsp), &n, NULL);
        if (rc != XCP_OK) break;
        if (n < 8) { rc = XCP_E_PROTO; break; }
        out->mode = rsp[1];
        len = xcp_s_rd32(s, rsp + 4);
        out->len = len;
        if (len == 0) break; /* 该类型不可用 */
        if (len > ID_MAX_LEN) { rc = XCP_E_PROTO; break; }
        out->data = (uint8_t*)malloc((size_t)len + ag + 1);
        if (!out->data) { rc = XCP_E_NOMEM; break; }
        if (rsp[1] & 0x01) { /* TRANSFER_MODE=1：数据就在应答里 */
            if ((uint32_t)n < 8 + len || n > (int)sizeof(rsp)) { rc = XCP_E_PROTO; break; }
            memcpy(out->data, rsp + 8, len);
        } else {           /* 从 MTA 上传：元素数 = len / AG（向上取整） */
            rc = upload_locked(s, (len + ag - 1) / ag, out->data, NULL);
            if (rc == XCP_E_TIMEOUT && attempt < 2) {
                xcp_id_free(out);
                if (synch_locked(s) != XCP_OK) break;
                continue;
            }
            if (rc != XCP_OK) break;
        }
        out->data[len] = 0; /* 便于当字符串用（不计入 len） */
        break;
    }
    if (rc != XCP_OK) xcp_id_free(out);
    leave(s);
    return rc;
}
