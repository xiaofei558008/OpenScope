/* XCP 主机协议核心：会话 + RX 线程 + 命令引擎。参见 xcp_session.h。 */
#include "xcp_session.h"
#include "xcp_proto.h"
#include "xcp_checksum.h"
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
    uint8_t  pend_errpkt[8];   /* 完整 ERR 包（BUILD_CHECKSUM/DOWNLOAD_NEXT 带附加参数） */
    int      pend_errlen;

    XcpConnInfo info;          /* st 保护 */
    xcp_atomic_t connected;
    xcp_atomic_t link_dead;
    xcp_atomic_t stopping;
    int need_synch;            /* cmd_lock 保护：上次失败后链路可能失步 */
    int last_err;              /* cmd_lock 保护 */
    uint8_t last_errpkt[8];    /* cmd_lock 保护 */
    int last_errlen;
    XcpSeedKeyFn sk_fn;        /* cmd_lock 保护 */
    void* sk_user;
    int in_unlock;             /* 防止解锁流程中再次自动解锁 */
    XcpCommModeInfo cmi;       /* cmd_lock 保护 */
    int cmi_valid;

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
    case XCP_E_UNSUPPORTED: return "unsupported / no callback";
    case XCP_E_MISMATCH: return "checksum mismatch";
    case XCP_E_SEEDKEY: return "seed&key callback failed";
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
    } else if (is_err && n >= 2 && p[1] == XCP_ERR_CMD_SYNCH) {
        /* ERR_CMD_SYNCH 只可能是 SYNCH 的应答：在途命令不是 SYNCH，必是早先超时 SYNCH 的迟到应答，丢弃 */
        xcp_atomic_add(&s->n_stray, 1);
    } else if (is_err) {
        s->pend_err = n >= 2 ? p[1] : XCP_ERR_GENERIC;
        s->pend_errlen = n < 8 ? n : 8;
        memcpy(s->pend_errpkt, p, (size_t)s->pend_errlen);
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
/* 一次收发（不含重试）。npk>1 为主机块模式：连续发送 npk 个包（间隔 min_st_100us），
 * 只等最后一个包的应答；从机中途回 ERR 时立即停止发送。计时从最后一个包发出后开始。
 * 返回 XCP_OK / XCP_E_TIMEOUT / XCP_E_LINK / XCP_E_CLOSED / XCP_E_SLAVE(x) */
static int exchange_multi(XcpSession* s, const uint8_t* const* pk, const int* pl, int npk, int min_st_100us,
                          uint8_t* buf, int cap, int* out_len, int frames, int strip, int tmo_ms, int synch)
{
    int rc = 0, state, i;
    if (xcp_atomic_load(&s->stopping)) return XCP_E_CLOSED;
    if (xcp_atomic_load(&s->link_dead)) return XCP_E_LINK;
    xcp_mutex_lock(&s->st);
    s->pend_active = 1;
    s->pend_synch = synch;
    s->pend_state = PR_NONE;
    s->pend_buf = buf;
    s->pend_cap = cap;
    s->pend_len = 0;
    s->pend_errlen = 0;
    s->pend_frames = frames < 1 ? 1 : frames;
    s->pend_got = 0;
    s->pend_strip = strip;
    s->pend_tmo_ms = tmo_ms;
    s->pend_deadline = xcp_now_ms() + (uint64_t)tmo_ms; /* 发送前开始计时，避免极快应答竞争 */
    xcp_mutex_unlock(&s->st);

    for (i = 0; i < npk; i++) {
        if (i > 0) {
            int early;
            if (min_st_100us > 0) xcp_sleep_ms((min_st_100us + 9) / 10); /* 向上取整到 ms */
            xcp_mutex_lock(&s->st);
            early = s->pend_state != PR_NONE;
            xcp_mutex_unlock(&s->st);
            if (early) break; /* 从机已回 ERR：不再发后续包 */
        }
        rc = xcp_tl_send(s->tl, pk[i], pl[i]);
        if (rc != 0) break;
    }

    xcp_mutex_lock(&s->st);
    if (npk > 1) s->pend_deadline = xcp_now_ms() + (uint64_t)tmo_ms;
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
    else if (state == PR_ERR) {
        rc = XCP_E_SLAVE(s->pend_err);
        s->last_errlen = s->pend_errlen;
        memcpy(s->last_errpkt, s->pend_errpkt, (size_t)s->pend_errlen);
    }
    else if (state == PR_LINK) rc = XCP_E_LINK;
    else if (state == PR_CLOSED) rc = XCP_E_CLOSED;
    xcp_mutex_unlock(&s->st);
    if (rc == XCP_E_LINK && xcp_atomic_load(&s->stopping)) rc = XCP_E_CLOSED;
    return rc;
}

static int exchange(XcpSession* s, const uint8_t* cmd, int len, uint8_t* buf, int cap,
                    int* out_len, int frames, int strip, int tmo_ms, int synch)
{
    return exchange_multi(s, &cmd, &len, 1, 0, buf, cap, out_len, frames, strip, tmo_ms, synch);
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

static int unlock_locked(XcpSession* s, uint8_t resource);

/* 命令所属的受保护资源（ERR_ACCESS_LOCKED 时自动解锁用）；0 = 未知 */
static uint8_t cmd_resource(uint8_t c)
{
    if ((c >= XCP_CMD_COPY_CAL_PAGE && c <= XCP_CMD_DOWNLOAD)) return XCP_RES_CAL_PAG; /* 0xE4..0xF0 */
    if ((c >= XCP_CMD_ALLOC_ODT_ENTRY && c <= XCP_CMD_CLEAR_DAQ_LIST) || c == XCP_CMD_WRITE_DAQ_MULTIPLE)
        return XCP_RES_DAQ;
    if (c >= XCP_CMD_PROGRAM_VERIFY && c <= XCP_CMD_PROGRAM_START) return XCP_RES_PGM;
    return 0;
}

static int exec_multi_locked(XcpSession* s, const uint8_t* const* pk, const int* pl, int npk, int min_st,
                             uint8_t* rsp, int cap, int* rsp_len, const XcpCmdOpts* opts)
{
    const uint8_t* cmd = pk[0];
    int len = pl[0];
    uint8_t code = cmd[0];
    int n_locked = 0;
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
        rc = exchange_multi(s, pk, pl, npk, min_st, rsp, cap, &rl, frames, strip, tmo, 0);
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
        if (s->last_err == XCP_ERR_ACCESS_LOCKED && s->sk_fn && !s->in_unlock && n_locked < 2) {
            /* 表 218：ERR_ACCESS_LOCKED -> 预动作 Unlock slave -> repeat 2 times */
            uint8_t res = cmd_resource(code);
            int urc;
            n_locked++;
            if (res) urc = unlock_locked(s, res);
            else urc = XCP_E_SLAVE(XCP_ERR_ACCESS_LOCKED);
            if (urc != XCP_OK) return rc;
            if (opts && opts->pre) {
                urc = opts->pre(s, opts->pre_user);
                if (urc != XCP_OK) return urc;
            }
            xcp_atomic_add(&s->n_retry, 1);
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

static int exec_locked(XcpSession* s, const uint8_t* cmd, int len, uint8_t* rsp, int cap,
                       int* rsp_len, const XcpCmdOpts* opts)
{
    return exec_multi_locked(s, &cmd, &len, 1, 0, rsp, cap, rsp_len, opts);
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
static int comm_mode_locked(XcpSession* s);

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
        s->cmi_valid = 0;
        memset(&s->cmi, 0, sizeof(s->cmi));
        if (ci.optional) (void)comm_mode_locked(s); /* 取 MAX_BS/MIN_ST；失败则按无主机块模式 */
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

static int comm_mode_locked(XcpSession* s)
{
    uint8_t c = XCP_CMD_GET_COMM_MODE_INFO, rsp[SMALL_RSP];
    int n = 0, rc = exec_locked(s, &c, 1, rsp, (int)sizeof(rsp), &n, NULL);
    if (rc != XCP_OK) return rc;
    if (n < 7) return XCP_E_PROTO; /* 规范 8 字节；兼容旧从机少 DRIVER_VERSION 的 7 字节 */
    memset(&s->cmi, 0, sizeof(s->cmi));
    s->cmi.optional = rsp[2];
    s->cmi.max_bs = rsp[4];
    s->cmi.min_st = rsp[5];
    s->cmi.queue_size = rsp[6];
    s->cmi.driver_version = n >= 8 ? rsp[7] : 0;
    s->cmi_valid = 1;
    return XCP_OK;
}

int xcp_get_comm_mode_info(XcpSession* s, XcpCommModeInfo* out)
{
    int rc = enter(s);
    if (rc) return rc;
    rc = comm_mode_locked(s);
    if (rc == XCP_OK && out) *out = s->cmi;
    leave(s);
    return rc;
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

/* ================================================================== 第 2 轮 */
int xcp_session_comm_mode(XcpSession* s, XcpCommModeInfo* out)
{
    int v;
    xcp_mutex_lock(&s->cmd_lock);
    v = s->cmi_valid;
    if (v && out) *out = s->cmi;
    xcp_mutex_unlock(&s->cmd_lock);
    return v;
}

int xcp_session_last_err_packet(XcpSession* s, uint8_t* out8)
{
    int n;
    xcp_mutex_lock(&s->cmd_lock);
    n = s->last_errlen;
    if (out8 && n > 0) memcpy(out8, s->last_errpkt, (size_t)n);
    xcp_mutex_unlock(&s->cmd_lock);
    return n;
}

static int dl_off(uint32_t ag) { return ag < 2 ? 2 : (int)ag; } /* DOWNLOAD/NEXT 数据起始位置 */

/* 写一块（<=255 元素）：npk>1 即主机块模式。mp 为本块起始地址（预动作 SET_MTA 用）。 */
static int download_chunk_locked(XcpSession* s, MtaPre* mp, const uint8_t* src, uint32_t n, int block)
{
    XcpConnInfo ci = s->info;
    uint32_t ag = ci.ag, off = (uint32_t)dl_off(ag), per = (ci.max_cto - off) / ag;
    uint32_t npk = block ? (n + per - 1) / per : 1, i, sent = 0;
    int min_st = block && s->cmi_valid ? s->cmi.min_st : 0;
    int rc = XCP_E_PROTO, attempt;
    uint8_t* mem;
    const uint8_t** pk;
    int* pl;
    XcpCmdOpts o;
    if (per == 0 || n == 0 || n > 255 || (!block && n > per)) return XCP_E_ARG;
    mem = (uint8_t*)calloc(npk, ci.max_cto);
    pk = (const uint8_t**)malloc(npk * sizeof(*pk));
    pl = (int*)malloc(npk * sizeof(*pl));
    if (!mem || !pk || !pl) { free(mem); free(pk); free(pl); return XCP_E_NOMEM; }
    for (i = 0; i < npk; i++) {
        uint8_t* p = mem + (size_t)i * ci.max_cto;
        uint32_t c = n - sent;
        if (c > per) c = per;
        p[0] = i == 0 ? XCP_CMD_DOWNLOAD : XCP_CMD_DOWNLOAD_NEXT;
        p[1] = (uint8_t)(n - sent); /* 剩余元素数（DOWNLOAD 为总数） */
        memcpy(p + off, src + (size_t)sent * ag, (size_t)c * ag);
        pk[i] = p;
        pl[i] = (int)(off + c * ag);
        sent += c;
    }
    memset(&o, 0, sizeof(o));
    o.pre = pre_set_mta;
    o.pre_user = mp;
    for (attempt = 0; attempt < 3; attempt++) {
        rc = exec_multi_locked(s, pk, pl, (int)npk, min_st, NULL, 0, NULL, &o);
        if (XCP_IS_SLAVE_ERR(rc) && npk > 1) {
            /* 块中途出错时，已在路上的 DOWNLOAD_NEXT 还会各自引出 ERR：先 SYNCH 把它们排空 */
            int sr = synch_locked(s);
            if (sr == XCP_E_TIMEOUT || sr == XCP_E_LINK || sr == XCP_E_CLOSED) { rc = sr; break; }
        }
        /* 表 218 DOWNLOAD_NEXT：ERR_SEQUENCE -> SET_MTA -> repeat 2 times */
        if (rc == XCP_E_SLAVE(XCP_ERR_SEQUENCE) && npk > 1 && attempt < 2) {
            xcp_atomic_add(&s->n_retry, 1);
            rc = set_mta_locked(s, mp->ext, mp->addr);
            if (rc != XCP_OK) break;
            continue;
        }
        break;
    }
    free(mem); free(pk); free(pl);
    return rc;
}

static int download_impl(XcpSession* s, uint8_t ext, uint32_t addr, uint32_t n, const void* src, int allow_block)
{
    const uint8_t* in = (const uint8_t*)src;
    uint32_t done = 0, per, maxc, ag;
    int rc, block;
    MtaPre mp;
    XcpConnInfo ci;
    if (!src && n) return XCP_E_ARG;
    rc = enter(s);
    if (rc) return rc;
    ci = s->info;
    ag = ci.ag;
    if (!xcp_atomic_load(&s->connected)) { rc = XCP_E_NOTCONN; goto out; }
    per = (ci.max_cto - (uint32_t)dl_off(ag)) / ag;
    block = allow_block && s->cmi_valid && (s->cmi.optional & 0x01) && s->cmi.max_bs >= 2;
    maxc = block ? per * s->cmi.max_bs : per;
    if (maxc > 255) maxc = 255;
    if (per == 0) { rc = XCP_E_PROTO; goto out; }
    mp.ext = ext;
    mp.addr = addr;
    rc = set_mta_locked(s, ext, addr);
    while (rc == XCP_OK && done < n) {
        uint32_t c = n - done;
        if (c > maxc) c = maxc;
        rc = download_chunk_locked(s, &mp, in + (size_t)done * ag, c, block && c > per);
        done += c;
        mp.addr += c;
    }
out:
    leave(s);
    return rc;
}

int xcp_download(XcpSession* s, uint8_t ext, uint32_t addr, uint32_t n, const void* src)
{
    return download_impl(s, ext, addr, n, src, 1);
}

int xcp_download_noblock(XcpSession* s, uint8_t ext, uint32_t addr, uint32_t n, const void* src)
{
    return download_impl(s, ext, addr, n, src, 0);
}

int xcp_download_max_elems(XcpSession* s)
{
    XcpConnInfo ci;
    xcp_session_info(s, &ci);
    return ci.max_cto / ci.ag - 1;
}

int xcp_download_max(XcpSession* s, uint8_t ext, uint32_t addr, const void* src)
{
    uint8_t pkt[256];
    int rc, n;
    MtaPre mp;
    XcpCmdOpts o;
    XcpConnInfo ci;
    if (!src) return XCP_E_ARG;
    rc = enter(s);
    if (rc) return rc;
    ci = s->info;
    if (!xcp_atomic_load(&s->connected)) { rc = XCP_E_NOTCONN; goto out; }
    n = ci.max_cto / ci.ag - 1;
    memset(pkt, 0, sizeof(pkt));
    pkt[0] = XCP_CMD_DOWNLOAD_MAX;
    memcpy(pkt + ci.ag, src, (size_t)n * ci.ag);
    mp.ext = ext;
    mp.addr = addr;
    memset(&o, 0, sizeof(o));
    o.pre = pre_set_mta;
    o.pre_user = &mp;
    rc = set_mta_locked(s, ext, addr);
    if (rc == XCP_OK) rc = exec_locked(s, pkt, (int)(ci.ag + (uint32_t)n * ci.ag), NULL, 0, NULL, &o);
out:
    leave(s);
    return rc;
}

int xcp_short_download(XcpSession* s, uint8_t ext, uint32_t addr, uint8_t n, const void* src)
{
    uint8_t pkt[256];
    int rc;
    XcpConnInfo ci;
    if (!src && n) return XCP_E_ARG;
    rc = enter(s);
    if (rc) return rc;
    ci = s->info;
    if (!xcp_atomic_load(&s->connected)) { rc = XCP_E_NOTCONN; goto out; }
    if (ci.max_cto < 8 || (uint32_t)n * ci.ag > (uint32_t)ci.max_cto - 8u) { rc = XCP_E_ARG; goto out; }
    pkt[0] = XCP_CMD_SHORT_DOWNLOAD;
    pkt[1] = n;
    pkt[2] = 0;
    pkt[3] = ext;
    xcp_s_wr32(s, pkt + 4, addr);
    if (n) memcpy(pkt + 8, src, (size_t)n * ci.ag);
    rc = exec_locked(s, pkt, 8 + (int)((uint32_t)n * ci.ag), NULL, 0, NULL, NULL);
out:
    leave(s);
    return rc;
}

int xcp_modify_bits(XcpSession* s, uint8_t ext, uint32_t addr, uint8_t shift, uint16_t and_mask, uint16_t xor_mask)
{
    uint8_t pkt[6];
    int rc;
    MtaPre mp;
    XcpCmdOpts o;
    if (shift > 31) return XCP_E_ARG;
    rc = enter(s);
    if (rc) return rc;
    if (!xcp_atomic_load(&s->connected)) { rc = XCP_E_NOTCONN; goto out; }
    pkt[0] = XCP_CMD_MODIFY_BITS;
    pkt[1] = shift;
    xcp_s_wr16(s, pkt + 2, and_mask);
    xcp_s_wr16(s, pkt + 4, xor_mask);
    mp.ext = ext;
    mp.addr = addr;
    memset(&o, 0, sizeof(o));
    o.pre = pre_set_mta;
    o.pre_user = &mp;
    rc = set_mta_locked(s, ext, addr);
    if (rc == XCP_OK) rc = exec_locked(s, pkt, 6, NULL, 0, NULL, &o);
out:
    leave(s);
    return rc;
}

static int checksum_locked(XcpSession* s, uint8_t ext, uint32_t addr, uint32_t n,
                           uint8_t* type, uint32_t* sum, uint32_t* max_block)
{
    uint8_t pkt[8], rsp[SMALL_RSP];
    int rc, got = 0;
    MtaPre mp;
    XcpCmdOpts o;
    if (max_block) *max_block = 0;
    pkt[0] = XCP_CMD_BUILD_CHECKSUM;
    pkt[1] = pkt[2] = pkt[3] = 0;
    xcp_s_wr32(s, pkt + 4, n);
    mp.ext = ext;
    mp.addr = addr;
    memset(&o, 0, sizeof(o));
    o.tmo_index = 2;
    o.pre = pre_set_mta;
    o.pre_user = &mp;
    rc = set_mta_locked(s, ext, addr);
    if (rc != XCP_OK) return rc;
    rc = exec_locked(s, pkt, 8, rsp, (int)sizeof(rsp), &got, &o);
    if (rc == XCP_E_SLAVE(XCP_ERR_OUT_OF_RANGE)) {
        if (max_block && s->last_errlen >= 8) *max_block = xcp_s_rd32(s, s->last_errpkt + 4);
        return rc;
    }
    if (rc != XCP_OK) return rc;
    if (got < 8) return XCP_E_PROTO;
    if (type) *type = rsp[1];
    if (sum) *sum = xcp_s_rd32(s, rsp + 4);
    return XCP_OK;
}

int xcp_build_checksum(XcpSession* s, uint8_t ext, uint32_t addr, uint32_t n,
                       uint8_t* type, uint32_t* sum, uint32_t* max_block)
{
    int rc = enter(s);
    if (rc) return rc;
    if (!xcp_atomic_load(&s->connected)) rc = XCP_E_NOTCONN;
    else rc = checksum_locked(s, ext, addr, n, type, sum, max_block);
    leave(s);
    return rc;
}

int xcp_checksum_verify(XcpSession* s, uint8_t ext, uint32_t addr, const void* data, uint32_t n,
                        XcpUserChecksumFn ufn, void* uuser, XcpChecksumResult* res)
{
    const uint8_t* d = (const uint8_t*)data;
    uint32_t off = 0, block = n, ag, maxb = 0;
    int rc, shrunk = 0;
    XcpChecksumResult r;
    if (!data || n == 0) return XCP_E_ARG;
    memset(&r, 0, sizeof(r));
    rc = enter(s);
    if (rc) return rc;
    ag = s->info.ag;
    if (!xcp_atomic_load(&s->connected)) { rc = XCP_E_NOTCONN; goto out; }
    while (off < n) {
        uint32_t c = n - off, local = 0, sum = 0;
        uint8_t type = 0;
        if (c > block) c = block;
        rc = checksum_locked(s, ext, addr + off, c, &type, &sum, &maxb);
        if (rc == XCP_E_SLAVE(XCP_ERR_OUT_OF_RANGE) && maxb > 0 && maxb < c && !shrunk) {
            /* 表 217：ERR_OUT_OF_RANGE -> 换参数重试。块按 4 字节对齐，保证 ADD_2x/ADD_44 也合法 */
            uint32_t a = 4u / ag ? 4u / ag : 1u;
            block = (maxb / a) * a;
            if (block == 0) block = maxb;
            shrunk = 1;
            xcp_atomic_add(&s->n_retry, 1);
            continue;
        }
        if (rc != XCP_OK) goto out;
        r.type = type;
        r.blocks++;
        r.block_elems = block;
        if (type == XCP_CKS_USER_DEFINED) {
            if (!ufn) { rc = XCP_E_UNSUPPORTED; goto out; }
            if (ufn(uuser, d + (size_t)off * ag, c * ag, &local) != 0) { rc = XCP_E_UNSUPPORTED; goto out; }
        } else if (xcp_checksum_compute(type, d + (size_t)off * ag, c * ag, s->info.byte_order, &local) != 0) {
            rc = XCP_E_PROTO;
            goto out;
        }
        if (local != sum) {
            r.bad_offset = off;
            r.slave_sum = sum;
            r.local_sum = local;
            rc = XCP_E_MISMATCH;
            goto out;
        }
        r.slave_sum = sum;
        r.local_sum = local;
        off += c;
    }
    rc = XCP_OK;
out:
    if (res) *res = r;
    leave(s);
    return rc;
}

void xcp_set_seed_key(XcpSession* s, XcpSeedKeyFn fn, void* user)
{
    xcp_mutex_lock(&s->cmd_lock);
    s->sk_fn = fn;
    s->sk_user = user;
    xcp_mutex_unlock(&s->cmd_lock);
}

static int unlock_locked(XcpSession* s, uint8_t resource)
{
    uint8_t seed[256], key[256], pkt[256], rsp[SMALL_RSP];
    int rc = XCP_E_PROTO, attempt;
    uint32_t mc = s->info.max_cto;
    if (!s->sk_fn) return XCP_E_UNSUPPORTED;
    s->in_unlock = 1;
    for (attempt = 0; attempt < 3; attempt++) {
        int n = 0, L, got, part, klen = 255, rem, off, kr;
        /* GET_SEED(mode 0, resource) */
        pkt[0] = XCP_CMD_GET_SEED; pkt[1] = 0; pkt[2] = resource;
        rc = exec_locked(s, pkt, 3, rsp, (int)sizeof(rsp), &n, NULL);
        if (rc != XCP_OK) break;
        if (n < 2) { rc = XCP_E_PROTO; break; }
        L = rsp[1];
        if (L == 0) { rc = XCP_OK; break; } /* 未保护 */
        part = L;
        if (part > (int)mc - 2) part = (int)mc - 2;
        if (part > n - 2) part = n - 2;
        if (part <= 0) { rc = XCP_E_PROTO; break; }
        memcpy(seed, rsp + 2, (size_t)part);
        got = part;
        while (got < L) { /* GET_SEED(mode 1) 取剩余 */
            pkt[1] = 1; pkt[2] = 0;
            rc = exec_locked(s, pkt, 3, rsp, (int)sizeof(rsp), &n, NULL);
            if (rc != XCP_OK) break;
            if (n < 3 || rsp[1] != L - got) { rc = XCP_E_PROTO; break; }
            part = rsp[1];
            if (part > (int)mc - 2) part = (int)mc - 2;
            if (part > n - 2) part = n - 2;
            memcpy(seed + got, rsp + 2, (size_t)part);
            got += part;
        }
        if (rc != XCP_OK) break;
        kr = s->sk_fn(s->sk_user, resource, seed, L, key, &klen);
        if (kr != 0 || klen <= 0 || klen > 255) { rc = XCP_E_SEEDKEY; break; }
        /* UNLOCK(剩余长度, key 段) */
        rem = klen; off = 0;
        while (rem > 0) {
            part = rem;
            if (part > (int)mc - 2) part = (int)mc - 2;
            pkt[0] = XCP_CMD_UNLOCK;
            pkt[1] = (uint8_t)rem;
            memcpy(pkt + 2, key + off, (size_t)part);
            rc = exec_locked(s, pkt, 2 + part, rsp, (int)sizeof(rsp), &n, NULL);
            if (rc != XCP_OK) break;
            off += part;
            rem -= part;
        }
        if (rc == XCP_E_SLAVE(XCP_ERR_SEQUENCE) && attempt < 2) { /* 表 217：预动作 GET_SEED，repeat 2 */
            xcp_atomic_add(&s->n_retry, 1);
            continue;
        }
        if (rc == XCP_E_SLAVE(XCP_ERR_ACCESS_LOCKED)) {
            xcp_atomic_store(&s->connected, 0); /* 规范：key 错误后从机进入断开状态 */
            break;
        }
        if (rc == XCP_OK && n >= 2 && (rsp[1] & resource)) rc = XCP_E_SLAVE(XCP_ERR_ACCESS_LOCKED);
        break;
    }
    s->in_unlock = 0;
    return rc;
}

int xcp_unlock(XcpSession* s, uint8_t resource)
{
    int rc;
    if (resource != XCP_RES_CAL_PAG && resource != XCP_RES_DAQ &&
        resource != XCP_RES_STIM && resource != XCP_RES_PGM) return XCP_E_ARG;
    rc = enter(s);
    if (rc) return rc;
    if (!xcp_atomic_load(&s->connected)) rc = XCP_E_NOTCONN;
    else rc = unlock_locked(s, resource);
    leave(s);
    return rc;
}

int xcp_unlock_all(XcpSession* s, uint8_t* prot_out)
{
    static const uint8_t res[4] = { XCP_RES_CAL_PAG, XCP_RES_DAQ, XCP_RES_STIM, XCP_RES_PGM };
    XcpStatus st;
    int rc, i;
    rc = xcp_get_status(s, &st);
    if (rc != XCP_OK) return rc;
    for (i = 0; i < 4; i++) {
        if (!(st.protection & res[i])) continue;
        rc = xcp_unlock(s, res[i]);
        if (rc != XCP_OK) return rc;
    }
    rc = xcp_get_status(s, &st);
    if (rc == XCP_OK && prot_out) *prot_out = st.protection;
    return rc;
}
