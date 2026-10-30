/* A 块单测：会话 + 命令引擎 + TCP/UDP 传输，对假从机（回环）。 */
#include "../xcp_session.h"
#include "../xcp_proto.h"
#include "xcp_fake_slave.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fail, g_checks;
#define CHECK(c) do { g_checks++; if (!(c)) { g_fail++; printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); } } while (0)
#define CHECK_RC(r, want) do { int r_ = (r); g_checks++; if (r_ != (want)) { g_fail++; \
    printf("  FAIL %s:%d: %s = %d (%s), want %d\n", __FILE__, __LINE__, #r, r_, xcp_strerror(r_), want); } } while (0)

typedef struct Cnt {
    xcp_atomic_t ev, serv, daq, link, pending_seen, daq_bytes;
    xcp_atomic_t last_ev, last_serv_byte;
} Cnt;
static void cb_ev(void* u, const uint8_t* p, int n)
{
    Cnt* c = (Cnt*)u;
    if (n >= 2 && p[1] == XCP_EV_CMD_PENDING) xcp_atomic_add(&c->pending_seen, 1);
    else { xcp_atomic_add(&c->ev, 1); xcp_atomic_store(&c->last_ev, p[1]); }
}
static void cb_serv(void* u, const uint8_t* p, int n) { Cnt* c = (Cnt*)u; xcp_atomic_add(&c->serv, 1); if (n >= 3) xcp_atomic_store(&c->last_serv_byte, p[2]); }
static void cb_daq(void* u, const uint8_t* p, int n) { Cnt* c = (Cnt*)u; (void)p; xcp_atomic_add(&c->daq, 1); xcp_atomic_add(&c->daq_bytes, n); }
static void cb_link(void* u, int r) { Cnt* c = (Cnt*)u; (void)r; xcp_atomic_add(&c->link, 1); }

typedef struct Rig { FakeSlave* f; XcpSession* s; Cnt cnt; int port; } Rig;

static int rig_up(Rig* r, const FakeCfg* fc, int t1)
{
    XcpSessionCfg sc;
    XcpTl* tl;
    memset(r, 0, sizeof(*r));
    r->f = fake_start(fc, &r->port);
    if (!r->f) return -1;
    tl = fc->udp ? xcp_tl_udp_open("127.0.0.1", r->port, NULL) : xcp_tl_tcp_open("127.0.0.1", r->port, 1000, NULL);
    if (!tl) return -2;
    memset(&sc, 0, sizeof(sc));
    sc.tmo.t[1] = t1;
    sc.tmo.t[7] = 10;
    sc.on_event = cb_ev; sc.on_serv = cb_serv; sc.on_daq = cb_daq; sc.on_link_lost = cb_link;
    sc.user = &r->cnt;
    return xcp_session_create(&r->s, tl, &sc);
}
static void rig_down(Rig* r) { xcp_session_destroy(r->s); fake_stop(r->f); }

static FakeCfg base_cfg(int udp)
{
    FakeCfg c;
    memset(&c, 0, sizeof(c));
    c.udp = udp; c.ag = 1; c.max_cto = 8; c.max_dto = 8;
    c.resource = XCP_RES_CAL_PAG | XCP_RES_DAQ;
    c.session_status = 0x41; c.protection = 0x05; c.session_cfg_id = 0x1234;
    return c;
}

static void wait_for(xcp_atomic_t* a, long v, int ms)
{
    uint64_t t0 = xcp_now_ms();
    while (xcp_atomic_load(a) < v && xcp_now_ms() - t0 < (uint64_t)ms) xcp_sleep_ms(2);
}

/* 1. CONNECT 参数解析：Intel，MAX_CTO=255，MAX_DTO=1400（不再被截到 64） */
static void t_connect_intel(int udp)
{
    Rig r; FakeCfg c = base_cfg(udp); XcpConnInfo ci;
    c.max_cto = 255; c.max_dto = udp ? 1400 : 4000; c.block = 1;
    CHECK_RC(rig_up(&r, &c, 300), 0);
    CHECK_RC(xcp_connect(r.s, 0, &ci), XCP_OK);
    CHECK(ci.byte_order == 0 && ci.ag == 1 && ci.slave_block_mode == 1 && ci.optional == 1);
    CHECK(ci.max_cto == 255 && ci.max_dto == c.max_dto);
    CHECK(ci.resource == c.resource && ci.protocol_ver == 1 && ci.transport_ver == 1);
    CHECK(xcp_session_connected(r.s));
    CHECK_RC(xcp_disconnect(r.s), XCP_OK);
    CHECK(!xcp_session_connected(r.s));
    rig_down(&r);
}

/* 2. Motorola + AG=WORD/DWORD；GET_STATUS 的 SESSION_CONFIGURATION_ID 按大端解析；SHORT_UPLOAD 对齐填充 */
static void t_connect_motorola(int udp, int ag)
{
    Rig r; FakeCfg c = base_cfg(udp); XcpConnInfo ci; XcpStatus st; uint8_t buf[16];
    XcpCommModeInfo cm;
    c.byte_order = 1; c.ag = ag; c.max_cto = 64; c.max_dto = 0x0102 * ag;
    CHECK_RC(rig_up(&r, &c, 300), 0);
    CHECK_RC(xcp_connect(r.s, 0, &ci), XCP_OK);
    CHECK(ci.byte_order == 1 && ci.ag == ag && ci.max_dto == 0x0102 * ag && ci.slave_block_mode == 0);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
    CHECK(st.session_status == 0x41 && st.protection == 0x05 && st.session_cfg_id == 0x1234);
    CHECK_RC(xcp_get_comm_mode_info(r.s, &cm), XCP_OK);
    CHECK(cm.optional == 1 && cm.max_bs == 16 && cm.min_st == 2 && cm.driver_version == 0x13);
    /* 地址 0x10 元素 -> 字节偏移 0x10*ag；假从机内存 mem[i]=i*7+3 */
    CHECK_RC(xcp_short_upload(r.s, 0, 0x10, (uint8_t)(8 / ag), buf), XCP_OK);
    CHECK(buf[0] == (uint8_t)(0x10 * ag * 7 + 3) && buf[7] == (uint8_t)((0x10 * ag + 7) * 7 + 3));
    rig_down(&r);
}

/* 3. 超时：从机完全不答 -> SYNCH 预动作 -> 重试 2 次 -> TIMEOUT；时间约 3*t1 + SYNCH */
static void t_timeout(int udp)
{
    Rig r; FakeCfg c = base_cfg(udp); XcpStatus st; XcpSessionStats ss; uint64_t t0, dt;
    CHECK_RC(rig_up(&r, &c, 100), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    fake_drop(r.f, XCP_CMD_GET_STATUS, 100);
    t0 = xcp_now_ms();
    CHECK_RC(xcp_get_status(r.s, &st), XCP_E_TIMEOUT);
    dt = xcp_now_ms() - t0;
    CHECK(dt >= 300 && dt < 1500);
    CHECK(fake_count(r.f, XCP_CMD_GET_STATUS) == 3);
    CHECK(fake_count(r.f, XCP_CMD_SYNCH) == 2);
    xcp_session_stats(r.s, &ss);
    CHECK(ss.timeouts == 3 && ss.retries == 2);
    /* 恢复后下一条命令正常 */
    fake_drop(r.f, XCP_CMD_GET_STATUS, 0);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
    rig_down(&r);
}

/* 4. SYNCH + 重试成功；迟到应答被丢弃，不会错配给下一条命令 */
static void t_synch_retry(int udp)
{
    Rig r; FakeCfg c = base_cfg(udp); XcpStatus st; XcpSessionStats ss; XcpCommModeInfo cm;
    CHECK_RC(rig_up(&r, &c, 100), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    fake_drop(r.f, XCP_CMD_GET_STATUS, 1);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
    CHECK(st.session_cfg_id == 0x1234);
    CHECK(fake_count(r.f, XCP_CMD_SYNCH) == 1);
    /* 迟到：GET_COMM_MODE_INFO 第一次 250ms 后才答（t1=100） */
    fake_delay(r.f, XCP_CMD_GET_COMM_MODE_INFO, 250, 1);
    CHECK_RC(xcp_get_comm_mode_info(r.s, &cm), XCP_OK);
    CHECK(cm.max_bs == 16);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);  /* 不能收到 COMM_MODE_INFO 的旧应答 */
    CHECK(st.session_status == 0x41 && st.session_cfg_id == 0x1234);
    xcp_session_stats(r.s, &ss);
    CHECK(ss.rx_stray >= 1);
    CHECK_RC(xcp_synch(r.s), XCP_OK);
    rig_down(&r);
}

/* 5. ERR 处理：BUSY 等 t7 重试；RES_TEMP_NOT_ACC 重试；OUT_OF_RANGE 直接返回；BUSY 超上限返回 */
static void t_err(int udp)
{
    Rig r; FakeCfg c = base_cfg(udp); XcpStatus st; XcpSessionStats ss; XcpId id;
    CHECK_RC(rig_up(&r, &c, 200), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    fake_busy(r.f, XCP_CMD_GET_STATUS, 3);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
    CHECK(fake_count(r.f, XCP_CMD_GET_STATUS) == 4);
    xcp_session_stats(r.s, &ss);
    CHECK(ss.busy_waits == 3);
    fake_err(r.f, XCP_CMD_GET_STATUS, XCP_ERR_RES_TEMP_NOT_ACC, 2);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
    fake_err(r.f, XCP_CMD_GET_ID, XCP_ERR_OUT_OF_RANGE, 1);
    CHECK_RC(xcp_get_id(r.s, 0, &id), XCP_E_SLAVE(XCP_ERR_OUT_OF_RANGE));
    CHECK(xcp_session_last_slave_err(r.s) == XCP_ERR_OUT_OF_RANGE);
    CHECK(fake_count(r.f, XCP_CMD_GET_ID) == 1);
    fake_busy(r.f, XCP_CMD_GET_STATUS, 1000);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_E_SLAVE(XCP_ERR_CMD_BUSY));
    fake_busy(r.f, XCP_CMD_GET_STATUS, 0);
    { uint8_t cmd = 0xC0; CHECK_RC(xcp_command(r.s, &cmd, 1, NULL, 0, NULL, NULL), XCP_E_SLAVE(XCP_ERR_CMD_UNKNOWN)); }
    rig_down(&r);
}

/* 6. EV_CMD_PENDING 重启超时：从机 300ms 后才答（t1=120），期间每 100ms 发 CMD_PENDING */
static void t_cmd_pending(int udp)
{
    Rig r; FakeCfg c = base_cfg(udp); XcpStatus st; XcpSessionStats ss;
    CHECK_RC(rig_up(&r, &c, 120), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    fake_pending(r.f, XCP_CMD_GET_STATUS, 300, 1);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
    xcp_session_stats(r.s, &ss);
    CHECK(ss.timeouts == 0 && ss.cmd_pending >= 2);
    CHECK(fake_count(r.f, XCP_CMD_GET_STATUS) == 1);
    rig_down(&r);
}

/* 7. EV/SERV 路由（空闲时和命令在途时）；EV_SESSION_TERMINATED 清连接标志 */
static void t_ev_serv(int udp)
{
    Rig r; FakeCfg c = base_cfg(udp); XcpStatus st; int i;
    uint8_t ev[2] = { XCP_PID_EV, XCP_EV_STORE_CAL }, serv[6] = { XCP_PID_SERV, 0x01, 'h', 'i', 0, 0 };
    CHECK_RC(rig_up(&r, &c, 300), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    fake_inject(r.f, ev, 2);
    fake_inject(r.f, serv, 4);
    fake_delay(r.f, XCP_CMD_GET_STATUS, 50, 1);
    for (i = 0; i < 5; i++) fake_inject(r.f, ev, 2);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
    wait_for(&r.cnt.ev, 6, 1000);
    wait_for(&r.cnt.serv, 1, 1000);
    CHECK(xcp_atomic_load(&r.cnt.ev) == 6 && xcp_atomic_load(&r.cnt.last_ev) == XCP_EV_STORE_CAL);
    CHECK(xcp_atomic_load(&r.cnt.serv) == 1 && xcp_atomic_load(&r.cnt.last_serv_byte) == 'h');
    CHECK(xcp_atomic_load(&r.cnt.daq) == 0);
    ev[1] = XCP_EV_SESSION_TERMINATED;
    fake_inject(r.f, ev, 2);
    wait_for(&r.cnt.ev, 7, 1000);
    CHECK(!xcp_session_connected(r.s));
    rig_down(&r);
}

/* 8. DAQ 路由：后台线程狂发 DAQ，同时跑命令，命令全部正确，DAQ 一个不丢（TCP） */
typedef struct DaqPump { FakeSlave* f; int n; } DaqPump;
static void pump(void* a)
{
    DaqPump* p = (DaqPump*)a; int i; uint8_t pk[12];
    for (i = 0; i < p->n; i++) {
        pk[0] = (uint8_t)(i % 0xFC); memset(pk + 1, 0xAB, 11);
        fake_inject(p->f, pk, 12);
        if ((i & 63) == 0) xcp_sleep_ms(1);
    }
}
static void t_daq(int udp)
{
    Rig r; FakeCfg c = base_cfg(udp); XcpStatus st; xcp_thread_t th; DaqPump dp; int i, ok = 0;
    CHECK_RC(rig_up(&r, &c, 500), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    dp.f = r.f; dp.n = 5000;
    xcp_thread_start(&th, pump, &dp);
    for (i = 0; i < 200; i++) if (xcp_get_status(r.s, &st) == XCP_OK && st.session_cfg_id == 0x1234) ok++;
    xcp_thread_join(&th);
    wait_for(&r.cnt.daq, dp.n, 2000);
    CHECK(ok == 200);
    if (udp) CHECK(xcp_atomic_load(&r.cnt.daq) >= dp.n * 9 / 10); /* UDP 允许极少丢失 */
    else CHECK(xcp_atomic_load(&r.cnt.daq) == dp.n);
    CHECK(xcp_atomic_load(&r.cnt.ev) == 0 && xcp_atomic_load(&r.cnt.serv) == 0);
    {
        XcpSessionStats ss; xcp_session_stats(r.s, &ss);
        CHECK(ss.rx_daq == xcp_atomic_load(&r.cnt.daq) && ss.rx_res >= 201);
        if (!udp) CHECK(ss.tl_ctr_gaps == 0);
    }
    rig_down(&r);
}

/* 9. GET_ID：内联（TRANSFER_MODE=1）、不可用（len=0）、上传（块模式/非块、AG=1/4、Motorola） */
static void t_get_id(int udp, int block, int ag, int bo)
{
    Rig r; FakeCfg c = base_cfg(udp); XcpId id; uint8_t* blob; int L = 3001, i;
    c.block = block; c.ag = ag; c.byte_order = bo; c.max_cto = ag == 4 ? 252 : 255; c.max_dto = 512;
    if (!block) c.max_cto = ag == 4 ? 64 : 63;
    CHECK_RC(rig_up(&r, &c, 300), 0);
    blob = (uint8_t*)malloc((size_t)L);
    for (i = 0; i < L; i++) blob[i] = (uint8_t)(i * 13 + 1);
    fake_set_id(r.f, blob, L);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    CHECK_RC(xcp_get_id(r.s, 0, &id), XCP_OK);
    CHECK(id.len == 12 && id.data && memcmp(id.data, "FAKE-XCP-ECU", 12) == 0 && (id.mode & 1));
    xcp_id_free(&id);
    CHECK_RC(xcp_get_id(r.s, 3, &id), XCP_OK);
    CHECK(id.len == 0 && id.data == NULL);
    CHECK_RC(xcp_get_id(r.s, 4, &id), XCP_OK);
    CHECK(id.len == (uint32_t)L && id.data && memcmp(id.data, blob, (size_t)L) == 0 && !(id.mode & 1));
    xcp_id_free(&id);
    /* 上传中途丢帧 -> 整体重做 GET_ID */
    fake_drop(r.f, XCP_CMD_UPLOAD, 1);
    CHECK_RC(xcp_get_id(r.s, 4, &id), XCP_OK);
    CHECK(id.len == (uint32_t)L && id.data && memcmp(id.data, blob, (size_t)L) == 0);
    xcp_id_free(&id);
    free(blob);
    rig_down(&r);
}

/* 10. UPLOAD + 超时后 SYNCH+SET_MTA 预动作 */
static void t_upload(int udp)
{
    Rig r; FakeCfg c = base_cfg(udp); uint8_t buf[1000]; int i, bad = 0;
    c.block = 1; c.max_cto = 40;
    CHECK_RC(rig_up(&r, &c, 150), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    fake_drop(r.f, XCP_CMD_UPLOAD, 1);
    CHECK_RC(xcp_upload(r.s, 0, 100, 1000, buf), XCP_OK);
    for (i = 0; i < 1000; i++) if (buf[i] != (uint8_t)((100 + i) * 7 + 3)) bad++;
    CHECK(bad == 0);
    CHECK(fake_count(r.f, XCP_CMD_SET_MTA) == 2); /* 初始 + 预动作 */
    rig_down(&r);
}

/* 11. 链路断开：在途命令返回 LINK，回调一次，后续命令立刻失败 */
static void t_link_loss(void)
{
    Rig r; FakeCfg c = base_cfg(0); XcpStatus st;
    CHECK_RC(rig_up(&r, &c, 2000), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    fake_drop(r.f, XCP_CMD_GET_STATUS, 1);
    fake_close_client(r.f);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_E_LINK);
    wait_for(&r.cnt.link, 1, 1000);
    CHECK(xcp_atomic_load(&r.cnt.link) == 1);
    CHECK(!xcp_session_link_ok(r.s) && !xcp_session_connected(r.s));
    CHECK_RC(xcp_get_status(r.s, &st), XCP_E_LINK);
    rig_down(&r);
}

/* 12. 回调中调用命令 -> WOULDBLOCK（不会死锁） */
static XcpSession* g_reent_s; static xcp_atomic_t g_reent_rc;
static void cb_reent(void* u, const uint8_t* p, int n) { XcpStatus st; (void)u; (void)p; (void)n; xcp_atomic_store(&g_reent_rc, xcp_get_status(g_reent_s, &st)); }
static void t_reentrancy(void)
{
    FakeCfg c = base_cfg(0); FakeSlave* f; int port; XcpSession* s; XcpSessionCfg sc; uint8_t ev[2] = { XCP_PID_EV, 0 };
    uint64_t t0;
    f = fake_start(&c, &port);
    memset(&sc, 0, sizeof(sc)); sc.on_event = cb_reent; sc.tmo.t[1] = 200;
    CHECK_RC(xcp_session_create(&s, xcp_tl_tcp_open("127.0.0.1", port, 1000, NULL), &sc), 0);
    g_reent_s = s; xcp_atomic_store(&g_reent_rc, 1);
    CHECK_RC(xcp_connect(s, 0, NULL), XCP_OK);
    fake_inject(f, ev, 2);
    t0 = xcp_now_ms();
    while (xcp_atomic_load(&g_reent_rc) == 1 && xcp_now_ms() - t0 < 1000) xcp_sleep_ms(2);
    CHECK(xcp_atomic_load(&g_reent_rc) == XCP_E_WOULDBLOCK);
    xcp_session_destroy(s);
    fake_stop(f);
}

/* 13. 销毁时在途命令被唤醒（CLOSED），销毁总时长有界 */
typedef struct Blk { XcpSession* s; xcp_atomic_t rc; } Blk;
static void blk_fn(void* a) { Blk* b = (Blk*)a; XcpStatus st; xcp_atomic_store(&b->rc, xcp_get_status(b->s, &st)); }
static void t_destroy_inflight(void)
{
    Rig r; FakeCfg c = base_cfg(0); xcp_thread_t th; Blk b; uint64_t t0;
    CHECK_RC(rig_up(&r, &c, 5000), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    fake_drop(r.f, XCP_CMD_GET_STATUS, 1);
    b.s = r.s; xcp_atomic_store(&b.rc, 99);
    xcp_thread_start(&th, blk_fn, &b);
    xcp_sleep_ms(100);
    t0 = xcp_now_ms();
    xcp_session_destroy(r.s);   /* 文档约定：销毁后不得再调；这里线程已在调用内部 */
    xcp_thread_join(&th);
    CHECK(xcp_now_ms() - t0 < 1000);
    CHECK(xcp_atomic_load(&b.rc) == XCP_E_CLOSED);
    fake_stop(r.f);
}

/* 14. 多会话并发：4 个从机（2 TCP + 2 UDP，不同字节序/AG），每个 2 个线程并发发命令 */
typedef struct Worker { XcpSession* s; uint16_t want; int ok; int n; } Worker;
static void worker(void* a)
{
    Worker* w = (Worker*)a; int i; XcpStatus st; uint8_t b[8];
    for (i = 0; i < w->n; i++) {
        if (xcp_get_status(w->s, &st) == XCP_OK && st.session_cfg_id == w->want) w->ok++;
        if (xcp_short_upload(w->s, 0, 4, 1, b) == XCP_OK) w->ok++;
    }
}
static void t_multi(void)
{
    enum { NS = 4, NW = 2 };
    Rig r[NS]; Worker w[NS * NW]; xcp_thread_t th[NS * NW]; DaqPump dp[NS]; xcp_thread_t pt[NS]; int i;
    for (i = 0; i < NS; i++) {
        FakeCfg c = base_cfg(i >= 2);
        c.byte_order = i & 1; c.ag = (i == 3) ? 4 : 1; c.max_cto = (i == 3) ? 16 : 8;
        c.session_cfg_id = (uint16_t)(0x1000 + i);
        CHECK_RC(rig_up(&r[i], &c, 500), 0);
        CHECK_RC(xcp_connect(r[i].s, 0, NULL), XCP_OK);
    }
    for (i = 0; i < NS * NW; i++) {
        w[i].s = r[i / NW].s; w[i].want = (uint16_t)(0x1000 + i / NW); w[i].ok = 0; w[i].n = 150;
        xcp_thread_start(&th[i], worker, &w[i]);
    }
    for (i = 0; i < NS; i++) { dp[i].f = r[i].f; dp[i].n = 1000; xcp_thread_start(&pt[i], pump, &dp[i]); }
    for (i = 0; i < NS * NW; i++) { xcp_thread_join(&th[i]); CHECK(w[i].ok == 2 * w[i].n); }
    for (i = 0; i < NS; i++) xcp_thread_join(&pt[i]);
    for (i = 0; i < NS; i++) {
        wait_for(&r[i].cnt.daq, 1000, 2000);
        if (i < 2) CHECK(xcp_atomic_load(&r[i].cnt.daq) == 1000);
        else CHECK(xcp_atomic_load(&r[i].cnt.daq) >= 900);
        rig_down(&r[i]);
    }
}

/* 15. 无从机：TCP 连接失败返回 NULL；UDP CONNECT 超时（重试上限） */
static void t_no_slave(void)
{
    XcpSession* s; XcpSessionCfg sc; XcpTl* tl; FakeCfg c = base_cfg(1); FakeSlave* f; int port;
    f = fake_start(&c, &port); fake_stop(f); /* 拿到一个空闲端口 */
    CHECK(xcp_tl_tcp_open("127.0.0.1", port, 300, NULL) == NULL);
    tl = xcp_tl_udp_open("127.0.0.1", port, NULL);
    CHECK(tl != NULL);
    memset(&sc, 0, sizeof(sc)); sc.tmo.t[1] = 80; sc.connect_retries = 2;
    CHECK_RC(xcp_session_create(&s, tl, &sc), 0);
    CHECK_RC(xcp_connect(s, 0, NULL), XCP_E_TIMEOUT);
    xcp_session_destroy(s);
}

int main(void)
{
    int udp;
    setvbuf(stdout, NULL, _IONBF, 0);
    for (udp = 0; udp < 2; udp++) {
        printf("[%s]\n", udp ? "UDP" : "TCP");
        printf(" connect_intel\n"); t_connect_intel(udp);
        printf(" connect_motorola ag2\n"); t_connect_motorola(udp, 2);
        printf(" connect_motorola ag4\n"); t_connect_motorola(udp, 4);
        printf(" timeout\n"); t_timeout(udp);
        printf(" synch_retry\n"); t_synch_retry(udp);
        printf(" err\n"); t_err(udp);
        printf(" cmd_pending\n"); t_cmd_pending(udp);
        printf(" ev_serv\n"); t_ev_serv(udp);
        printf(" daq\n"); t_daq(udp);
        printf(" get_id block ag1 intel\n"); t_get_id(udp, 1, 1, 0);
        printf(" get_id noblock ag1 motorola\n"); t_get_id(udp, 0, 1, 1);
        printf(" get_id block ag4 motorola\n"); t_get_id(udp, 1, 4, 1);
        printf(" get_id noblock ag4 intel\n"); t_get_id(udp, 0, 4, 0);
        printf(" upload\n"); t_upload(udp);
    }
    printf("[misc]\n");
    printf(" link_loss\n"); t_link_loss();
    printf(" reentrancy\n"); t_reentrancy();
    printf(" destroy_inflight\n"); t_destroy_inflight();
    printf(" multi_session\n"); t_multi();
    printf(" no_slave\n"); t_no_slave();
    printf("%s: %d checks, %d failed\n", g_fail ? "FAIL" : "PASS", g_checks, g_fail);
    return g_fail ? 1 : 0;
}
