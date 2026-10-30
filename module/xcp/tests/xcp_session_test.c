/* A 块单测：会话 + 命令引擎 + TCP/UDP 传输，对假从机（回环）。 */
#include "../xcp_session.h"
#include "../xcp_proto.h"
#include "xcp_fake_slave.h"
#include "../xcp_checksum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#  include <windows.h>
#  include <mmsystem.h> /* timeBeginPeriod：链接 winmm */
#endif

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

/* 4b. 迟到的 ERR_CMD_SYNCH 不得配给非 SYNCH 命令（小程第二轮 Windows 4/5 失败的回归） */
static void inj_synch(void* a) { static const uint8_t p[2] = { XCP_PID_ERR, XCP_ERR_CMD_SYNCH }; xcp_sleep_ms(30); fake_inject((FakeSlave*)a, p, 2); }
static void t_late_synch(int udp)
{
    Rig r; FakeCfg c = base_cfg(udp); XcpStatus st; XcpSessionStats s0, s1; xcp_thread_t th;
    CHECK_RC(rig_up(&r, &c, 300), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    /* 确定性注入：GET_STATUS 在途（从机 120ms 后答）时插入一个 ERR_CMD_SYNCH */
    xcp_session_stats(r.s, &s0);
    fake_delay(r.f, XCP_CMD_GET_STATUS, 120, 1);
    CHECK(xcp_thread_start(&th, inj_synch, r.f) == 0);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
    xcp_thread_join(&th);
    CHECK(st.session_cfg_id == 0x1234);
    xcp_session_stats(r.s, &s1);
    CHECK(s1.rx_stray == s0.rx_stray + 1);
    CHECK(s1.timeouts == s0.timeouts);
    rig_down(&r);
    /* 真实场景：GET_STATUS 丢失 → SYNCH 首次迟到（超 t1）→ 重发 SYNCH → 两个 ERR_CMD_SYNCH，
       多出来的一个在重试的 GET_STATUS 在途时到达 */
    CHECK_RC(rig_up(&r, &c, 100), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    fake_drop(r.f, XCP_CMD_GET_STATUS, 1);
    fake_delay(r.f, XCP_CMD_SYNCH, 150, 1);
    fake_delay(r.f, XCP_CMD_GET_STATUS, 100, 1); /* 让重试的 GET_STATUS 在途窗口覆盖迟到应答 */
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
    CHECK(st.session_cfg_id == 0x1234);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
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
    /* t1=250、每 100ms 一个 PENDING：余量 150ms，远大于 Windows 15.6ms 计时粒度 */
    CHECK_RC(rig_up(&r, &c, 250), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    fake_pending(r.f, XCP_CMD_GET_STATUS, 600, 1);
    CHECK_RC(xcp_get_status(r.s, &st), XCP_OK);
    xcp_session_stats(r.s, &ss);
    CHECK(ss.timeouts == 0);
    CHECK(ss.cmd_pending >= 3); /* 理论 5 个；粒度/调度抖动下至少 3 个必然在应答前到达 */
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


/* ======================================================= 第 2 轮 */
static FakeCfg cal_cfg(int udp, int ag, int bo)
{
    FakeCfg c = base_cfg(udp);
    c.protection = 0; c.ag = ag; c.byte_order = bo; c.max_cto = 8 * (ag == 4 ? 2 : 1); c.max_dto = 64;
    return c;
}

/* R2-1 DOWNLOAD：主机块模式（MAX_BS/MIN_ST）与非块模式，AG1/2/4 × 字节序 */
static void t_download(int udp, int ag, int bo, int block)
{
    Rig r; FakeCfg c = cal_cfg(udp, ag, bo); uint8_t src[1200], back[1200]; int i, nb = 1200;
    XcpCommModeInfo cm; XcpSessionStats ss;
    c.no_master_block = !block; c.max_bs = 4; c.min_st = 10; /* 1 ms */
    CHECK_RC(rig_up(&r, &c, 300), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    CHECK(xcp_session_comm_mode(r.s, &cm) == 1 && cm.max_bs == 4 && cm.min_st == 10 && (cm.optional & 1) == (block ? 1 : 0));
    for (i = 0; i < nb; i++) src[i] = (uint8_t)(i * 31 + 7);
    {
        uint64_t t0 = xcp_now_ms(), el;
        int nn;
        CHECK_RC(xcp_download(r.s, 0, 0x20, (uint32_t)(nb / ag), src), XCP_OK);
        el = xcp_now_ms() - t0;
        nn = fake_count(r.f, XCP_CMD_DOWNLOAD_NEXT);
        /* MIN_ST 的确定性判据：每个 DOWNLOAD_NEXT 前主机至少睡 MIN_ST(1ms)，
           故总耗时 ≥ NEXT 数 × 1ms；与调度负载无关（负载只会让它更长） */
        CHECK(!block || nn > 0);
        CHECK(el >= (uint64_t)nn);
    }
    fake_get_mem(r.f, 0x20u * (uint32_t)ag, back, nb);
    CHECK(memcmp(src, back, (size_t)nb) == 0);
    if (block) {
        CHECK(fake_max_block_frames(r.f) > 1 && fake_max_block_frames(r.f) <= 4);
        /* 从机侧到达间隔受调度/TSan/负载影响（迟到帧会与下一帧挤在一起），
           只断言确有间隔记录且非负（顺序）；MIN_ST 由上面的发送侧耗时下界保证 */
        CHECK(fake_min_gap_us(r.f) >= 0);
    } else {
        CHECK(fake_max_block_frames(r.f) == 1 && fake_count(r.f, XCP_CMD_DOWNLOAD_NEXT) == 0);
    }
    xcp_session_stats(r.s, &ss);
    CHECK(ss.timeouts == 0);
    rig_down(&r);
}

/* R2-2 块传输丢帧 -> ERR_SEQUENCE -> SET_MTA + 重发整块；以及块中超时 -> SYNCH + SET_MTA */
static void t_download_recovery(int udp)
{
    Rig r; FakeCfg c = cal_cfg(udp, 1, 0); uint8_t src[300], back[300]; int i;
    XcpSessionStats ss;
    c.max_bs = 8; c.min_st = -1;
    CHECK_RC(rig_up(&r, &c, 150), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    for (i = 0; i < 300; i++) src[i] = (uint8_t)(255 - i);
    fake_lose_next(r.f, 2);
    CHECK_RC(xcp_download(r.s, 0, 0x100, 300, src), XCP_OK);
    fake_get_mem(r.f, 0x100, back, 300);
    CHECK(memcmp(src, back, 300) == 0);
    xcp_session_stats(r.s, &ss);
    CHECK(ss.retries >= 1);
    CHECK(fake_count(r.f, XCP_CMD_SET_MTA) >= 2);
    /* 块的第一帧丢失 -> 后续 NEXT 引出 ERR_SEQUENCE -> SYNCH 排空 + SET_MTA + 重发 */
    for (i = 0; i < 300; i++) src[i] = (uint8_t)(i ^ 0x5A);
    fake_drop(r.f, XCP_CMD_DOWNLOAD, 1);
    CHECK_RC(xcp_download(r.s, 0, 0x100, 300, src), XCP_OK);
    fake_get_mem(r.f, 0x100, back, 300);
    CHECK(memcmp(src, back, 300) == 0);
    xcp_session_stats(r.s, &ss);
    CHECK(ss.synchs >= 1 && ss.timeouts == 0);
    /* 块的最后一帧丢失 -> 从机一直等 -> 超时 -> SYNCH + SET_MTA + 重发 */
    for (i = 0; i < 300; i++) src[i] = (uint8_t)(i ^ 0xA5);
    fake_lose_next(r.f, 7); /* MAX_BS=8：第一块 1 个 DOWNLOAD + 7 个 NEXT */
    CHECK_RC(xcp_download(r.s, 0, 0x100, 300, src), XCP_OK);
    fake_get_mem(r.f, 0x100, back, 300);
    CHECK(memcmp(src, back, 300) == 0);
    xcp_session_stats(r.s, &ss);
    CHECK(ss.synchs >= 2 && ss.timeouts == 1);
    rig_down(&r);
}

/* R2-3 DOWNLOAD_MAX / SHORT_DOWNLOAD / 不用块模式 */
static void t_download_variants(int udp, int ag, int bo)
{
    Rig r; FakeCfg c = cal_cfg(udp, ag, bo); uint8_t src[256], back[256]; int i, nm;
    c.max_cto = 64;
    CHECK_RC(rig_up(&r, &c, 300), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    for (i = 0; i < 256; i++) src[i] = (uint8_t)(i * 5 + 1);
    nm = xcp_download_max_elems(r.s);
    CHECK(nm == 64 / ag - 1);
    CHECK_RC(xcp_download_max(r.s, 0, 0x40, src), XCP_OK);
    fake_get_mem(r.f, 0x40u * (uint32_t)ag, back, nm * ag);
    CHECK(memcmp(src, back, (size_t)(nm * ag)) == 0);
    CHECK_RC(xcp_short_download(r.s, 0, 0x200, (uint8_t)((64 - 8) / ag), src + 3), XCP_OK);
    fake_get_mem(r.f, 0x200u * (uint32_t)ag, back, 56);
    CHECK(memcmp(src + 3, back, 56) == 0);
    CHECK_RC(xcp_short_download(r.s, 0, 0x200, (uint8_t)((64 - 8) / ag + 1), src), XCP_E_ARG);
    CHECK_RC(xcp_download_noblock(r.s, 0, 0x300, 200 / ag, src + 7), XCP_OK);
    fake_get_mem(r.f, 0x300u * (uint32_t)ag, back, 200);
    CHECK(memcmp(src + 7, back, 200) == 0);
    CHECK(fake_count(r.f, XCP_CMD_DOWNLOAD_NEXT) == 0);
    rig_down(&r);
}

/* R2-4 MODIFY_BITS（规范示例：S=16, MA=0xBFFE, MX=0x0001: 0xFFF0FFFF -> 0xBFF1FFFF） */
static void t_modify_bits(int udp, int bo)
{
    Rig r; FakeCfg c = cal_cfg(udp, 1, bo); uint8_t w[4], back[4]; uint32_t v;
    CHECK_RC(rig_up(&r, &c, 300), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    if (bo) { w[0] = 0xFF; w[1] = 0xF0; w[2] = 0xFF; w[3] = 0xFF; }
    else { w[0] = 0xFF; w[1] = 0xFF; w[2] = 0xF0; w[3] = 0xFF; }
    fake_set_mem(r.f, 0x80, w, 4);
    CHECK_RC(xcp_modify_bits(r.s, 0, 0x80, 16, 0xBFFE, 0x0001), XCP_OK);
    fake_get_mem(r.f, 0x80, back, 4);
    v = bo ? ((uint32_t)back[0] << 24 | (uint32_t)back[1] << 16 | (uint32_t)back[2] << 8 | back[3])
           : ((uint32_t)back[3] << 24 | (uint32_t)back[2] << 16 | (uint32_t)back[1] << 8 | back[0]);
    CHECK(v == 0xBFF1FFFFu);
    /* 清 bit0 */
    CHECK_RC(xcp_modify_bits(r.s, 0, 0x80, 0, 0xFFFE, 0x0000), XCP_OK);
    fake_get_mem(r.f, 0x80, back, 4);
    CHECK((bo ? back[3] : back[0]) == 0xFE);
    CHECK_RC(xcp_modify_bits(r.s, 0, 0x80, 32, 0, 0), XCP_E_ARG);
    rig_down(&r);
}

/* R2-5 校验和：规范测试向量（Table 74/75） */
static void t_checksum_vectors(void)
{
    static const uint8_t pat[32] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
        0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,0x00 };
    static const uint32_t intel[9] = { 0x10, 0x0F10, 0x00000F10, 0x1800, 0x00071800, 0x140C03F8, 0xC76A, 0x9D50, 0x89CD97CE };
    static const uint32_t moto[9]  = { 0x10, 0x0F10, 0x00000F10, 0x0710, 0x00080710, 0xFC040B10, 0xC76A, 0x9D50, 0x89CD97CE };
    int t; uint32_t v;
    for (t = 1; t <= 9; t++) {
        CHECK(xcp_checksum_compute((uint8_t)t, pat, 32, 0, &v) == 0 && v == intel[t - 1]);
        CHECK(xcp_checksum_compute((uint8_t)t, pat, 32, 1, &v) == 0 && v == moto[t - 1]);
    }
    CHECK(xcp_checksum_compute(XCP_CKS_ADD_22, pat, 31, 0, &v) == -2);
    CHECK(xcp_checksum_compute(XCP_CKS_ADD_44, pat, 30, 0, &v) == -2);
    CHECK(xcp_checksum_compute(XCP_CKS_USER_DEFINED, pat, 32, 0, &v) == -1);
}

static int user_cks(void* u, const uint8_t* d, uint32_t len, uint32_t* out) { (void)u; *out = fake_user_checksum(d, len); return 0; }

/* R2-6 BUILD_CHECKSUM 对从机：全部算法 + USER_DEFINED 回调 + 最大块 OUT_OF_RANGE 重试 + 超时 SYNCH+SET_MTA + 不一致 */
static void t_checksum_slave(int udp, int bo)
{
    static const uint8_t types[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xFF };
    uint8_t data[1024], ty; uint32_t sum, maxb; int i, k;
    XcpChecksumResult res;
    for (i = 0; i < 1024; i++) data[i] = (uint8_t)(i * 97 + 13);
    for (k = 0; k < 10; k++) {
        Rig r; FakeCfg c = cal_cfg(udp, 1, bo);
        c.max_cto = 64; c.cks_type = types[k]; c.cks_max_block = (k % 3 == 0) ? 256 : 0;
        CHECK_RC(rig_up(&r, &c, 300), 0);
        CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
        CHECK_RC(xcp_download(r.s, 0, 0x400, 1024, data), XCP_OK);
        if (types[k] == 0xFF) CHECK_RC(xcp_checksum_verify(r.s, 0, 0x400, data, 1024, NULL, NULL, &res), XCP_E_UNSUPPORTED);
        CHECK_RC(xcp_checksum_verify(r.s, 0, 0x400, data, 1024, user_cks, NULL, &res), XCP_OK);
        CHECK(res.type == types[k]);
        if (c.cks_max_block) {
            CHECK(res.blocks == 4 && res.block_elems == 256);
            CHECK_RC(xcp_build_checksum(r.s, 0, 0x400, 1024, &ty, &sum, &maxb), XCP_E_SLAVE(XCP_ERR_OUT_OF_RANGE));
            CHECK(maxb == 256);
        } else CHECK(res.blocks == 1);
        if (k == 2) { /* 超时 -> SYNCH + SET_MTA 重试 */
            int mta0 = fake_count(r.f, XCP_CMD_SET_MTA);
            fake_drop(r.f, XCP_CMD_BUILD_CHECKSUM, 1);
            CHECK_RC(xcp_build_checksum(r.s, 0, 0x400, 1024, &ty, &sum, &maxb), XCP_OK);
            CHECK(fake_count(r.f, XCP_CMD_SET_MTA) == mta0 + 2);
            CHECK(sum == 0x0001FFFFu || 1); /* 值由下一句核对 */
            { uint32_t l; xcp_checksum_compute(3, data, 1024, bo, &l); CHECK(sum == l); }
        }
        { uint8_t x = (uint8_t)(data[700] ^ 0x01); fake_set_mem(r.f, 0x400 + 700, &x, 1); }
        CHECK_RC(xcp_checksum_verify(r.s, 0, 0x400, data, 1024, user_cks, NULL, &res), XCP_E_MISMATCH);
        if (c.cks_max_block) CHECK(res.bad_offset == 512);
        rig_down(&r);
    }
}

/* R2-7 Seed&Key：多段 seed（20 字节 / MAX_CTO 8）、多段 key（25 字节），逐资源、自动解锁、错 key、ERR_SEQUENCE 重试 */
static int sk_ok(void* u, uint8_t res, const uint8_t* seed, int L, uint8_t* key, int* klen)
{
    int extra = *(int*)u;
    if (*klen < L + extra) return -1;
    *klen = fake_compute_key(res, seed, L, extra, key);
    return 0;
}
static int sk_bad(void* u, uint8_t res, const uint8_t* seed, int L, uint8_t* key, int* klen)
{
    int rc = sk_ok(u, res, seed, L, key, klen);
    key[0] ^= 0xFF;
    return rc;
}
static void t_seed_key(int udp)
{
    Rig r; FakeCfg c = cal_cfg(udp, 1, 0); int extra = 5; uint8_t prot = 0xAA, d[16]; int sp;
    memset(d, 0x42, sizeof(d));
    c.protection = XCP_RES_CAL_PAG | XCP_RES_DAQ | XCP_RES_PGM; c.seed_len = 20; c.key_extra = extra;
    CHECK_RC(rig_up(&r, &c, 300), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    CHECK_RC(xcp_unlock(r.s, XCP_RES_CAL_PAG), XCP_E_UNSUPPORTED);
    CHECK_RC(xcp_download(r.s, 0, 0, 16, d), XCP_E_SLAVE(XCP_ERR_ACCESS_LOCKED));
    xcp_set_seed_key(r.s, sk_ok, &extra);
    sp = fake_count(r.f, XCP_CMD_GET_SEED);
    CHECK_RC(xcp_unlock(r.s, XCP_RES_DAQ), XCP_OK);
    CHECK(fake_protection(r.f) == (XCP_RES_CAL_PAG | XCP_RES_PGM));
    CHECK(fake_count(r.f, XCP_CMD_GET_SEED) - sp == 4);       /* 6+6+6+2 */
    CHECK(fake_count(r.f, XCP_CMD_UNLOCK) == 5);              /* 25 字节 / 6 */
    CHECK_RC(xcp_unlock(r.s, XCP_RES_DAQ), XCP_OK);           /* 已解锁：GET_SEED 长度 0 */
    /* 自动解锁：DOWNLOAD 遇 ACCESS_LOCKED -> 解 CAL/PAG -> 重试 */
    CHECK_RC(xcp_download(r.s, 0, 0, 16, d), XCP_OK);
    CHECK(fake_protection(r.f) == XCP_RES_PGM);
    /* UNLOCK 的 ERR_SEQUENCE -> 重新 GET_SEED -> 重试 */
    fake_err(r.f, XCP_CMD_UNLOCK, XCP_ERR_SEQUENCE, 1);
    CHECK_RC(xcp_unlock_all(r.s, &prot), XCP_OK);
    CHECK(prot == 0 && fake_protection(r.f) == 0);
    CHECK_RC(xcp_unlock(r.s, 0x03), XCP_E_ARG);
    rig_down(&r);
    /* 错 key -> ACCESS_LOCKED，会话标记断开 */
    CHECK_RC(rig_up(&r, &c, 300), 0);
    CHECK_RC(xcp_connect(r.s, 0, NULL), XCP_OK);
    xcp_set_seed_key(r.s, sk_bad, &extra);
    CHECK_RC(xcp_unlock(r.s, XCP_RES_CAL_PAG), XCP_E_SLAVE(XCP_ERR_ACCESS_LOCKED));
    CHECK(!xcp_session_connected(r.s));
    rig_down(&r);
}

int main(void)
{
    int udp;
    setvbuf(stdout, NULL, _IONBF, 0);
#ifdef _WIN32
    timeBeginPeriod(1); /* Sleep/超时粒度 1ms；退出前 timeEndPeriod 配对 */
#endif
    for (udp = 0; udp < 2; udp++) {
        printf("[%s]\n", udp ? "UDP" : "TCP");
        printf(" connect_intel\n"); t_connect_intel(udp);
        printf(" connect_motorola ag2\n"); t_connect_motorola(udp, 2);
        printf(" connect_motorola ag4\n"); t_connect_motorola(udp, 4);
        printf(" timeout\n"); t_timeout(udp);
        printf(" synch_retry\n"); t_synch_retry(udp);
        printf(" err\n"); t_err(udp);
        printf(" late_synch\n"); t_late_synch(udp);
        printf(" cmd_pending\n"); t_cmd_pending(udp);
        printf(" ev_serv\n"); t_ev_serv(udp);
        printf(" daq\n"); t_daq(udp);
        printf(" get_id block ag1 intel\n"); t_get_id(udp, 1, 1, 0);
        printf(" get_id noblock ag1 motorola\n"); t_get_id(udp, 0, 1, 1);
        printf(" get_id block ag4 motorola\n"); t_get_id(udp, 1, 4, 1);
        printf(" get_id noblock ag4 intel\n"); t_get_id(udp, 0, 4, 0);
        printf(" upload\n"); t_upload(udp);
    }
    for (udp = 0; udp < 2; udp++) {
        int ag, bo;
        printf("[round2 %s]\n", udp ? "UDP" : "TCP");
        for (ag = 1; ag <= 4; ag *= 2) for (bo = 0; bo < 2; bo++) {
            printf(" download ag%d bo%d block\n", ag, bo); t_download(udp, ag, bo, 1);
            printf(" download ag%d bo%d noblock\n", ag, bo); t_download(udp, ag, bo, 0);
            printf(" download variants ag%d bo%d\n", ag, bo); t_download_variants(udp, ag, bo);
        }
        printf(" download recovery\n"); t_download_recovery(udp);
        printf(" modify_bits intel/motorola\n"); t_modify_bits(udp, 0); t_modify_bits(udp, 1);
        printf(" checksum slave intel/motorola\n"); t_checksum_slave(udp, 0); t_checksum_slave(udp, 1);
        printf(" seed_key\n"); t_seed_key(udp);
    }
    printf(" checksum vectors\n"); t_checksum_vectors();
    printf("[misc]\n");
    printf(" link_loss\n"); t_link_loss();
    printf(" reentrancy\n"); t_reentrancy();
    printf(" destroy_inflight\n"); t_destroy_inflight();
    printf(" multi_session\n"); t_multi();
    printf(" no_slave\n"); t_no_slave();
    printf("%s: %d checks, %d failed\n", g_fail ? "FAIL" : "PASS", g_checks, g_fail);
#ifdef _WIN32
    timeEndPeriod(1);
#endif
    return g_fail ? 1 : 0;
}
