/* XCP 1.3 协议冒烟：进程内主从回环。传输不走 TCP。
 * 第 2 轮：XcpMaster 内部有 RX 线程，回环改为线程安全（锁 + 应答队列 + 条件变量），
 * 原来直接 loop_send/loop_recv 的裸命令改走会话 xcp_command；期望值与迁移前逐条相同。 */
#include "../xcp_master.h"
#include "../xcp_slave.h"
#include "../xcp_transport.h"
#include "../xcp_can.h"
#include "../xcp_session.h"
#include "../xcp_plat.h"

#include <stdio.h>
#include <string.h>

static int g_fail;

static void expect(int ok, const char* what)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) g_fail++;
}

typedef struct Loop {
    XcpSlave* slave;
    xcp_mutex_t mu;
    xcp_cond_t cv;
    uint8_t rsp[16][XCP_MAX_CTO];
    int rsp_len[16];
    int head, count;
} Loop;

static int loop_send(void* ctx, const uint8_t* data, int len)
{
    Loop* L = (Loop*)ctx;
    uint8_t rsp[XCP_MAX_CTO];
    int n;
    xcp_mutex_lock(&L->mu);
    n = xcp_slave_input(L->slave, data, len, rsp, (int)sizeof(rsp));
    if (n > 0 && L->count < 16) {
        int slot = (L->head + L->count) % 16;
        memcpy(L->rsp[slot], rsp, (size_t)n);
        L->rsp_len[slot] = n;
        L->count++;
    }
    xcp_cond_broadcast(&L->cv);
    xcp_mutex_unlock(&L->mu);
    return len;
}

static int loop_recv(void* ctx, uint8_t* data, int cap, int timeout_ms)
{
    Loop* L = (Loop*)ctx;
    int n = 0;
    uint64_t deadline = xcp_now_ms() + (uint64_t)(timeout_ms > 0 ? timeout_ms : 0);
    xcp_mutex_lock(&L->mu);
    for (;;) {
        uint64_t now;
        if (L->count > 0) {
            n = L->rsp_len[L->head];
            if (n > cap) n = -1;
            else memcpy(data, L->rsp[L->head], (size_t)n);
            L->head = (L->head + 1) % 16;
            L->count--;
            break;
        }
        n = xcp_slave_pop_dto(L->slave, data, cap);
        if (n > 0) break;
        now = xcp_now_ms();
        if (now >= deadline) { n = -1; break; }
        xcp_cond_wait_ms(&L->cv, &L->mu, (int)(deadline - now));
    }
    xcp_mutex_unlock(&L->mu);
    return n;
}

static void loop_tick(Loop* L, uint16_t ev, uint32_t t)
{
    xcp_mutex_lock(&L->mu);
    xcp_slave_tick(L->slave, ev, t);
    xcp_cond_broadcast(&L->cv);
    xcp_mutex_unlock(&L->mu);
}

static uint8_t* loop_ptr(Loop* L, uint32_t addr, uint32_t len)
{
    /* 测试线程在没有在途命令时铺数据；仍加锁以免与 RX 线程的 pop_dto 竞争 */
    uint8_t* p;
    xcp_mutex_lock(&L->mu);
    p = xcp_slave_ptr(L->slave, addr, len);
    xcp_mutex_unlock(&L->mu);
    return p;
}

/* 裸命令：经会话发送（不再绕过 RX 线程直接读 io）。返回应答长度；ERR 时为 [FE, code] 两字节。 */
static int raw(XcpMaster* m, const uint8_t* cmd, int n, uint8_t* rsp, int cap)
{
    int len = 0, rc = xcp_command(xcp_master_session(m), cmd, n, rsp, cap, &len, NULL);
    if (rc == XCP_OK) return len;
    if (XCP_IS_SLAVE_ERR(rc)) { rsp[0] = XCP_PID_ERR; rsp[1] = (uint8_t)XCP_SLAVE_CODE(rc); return 2; }
    return -1;
}

static void test_loop(void)
{
    XcpSlave* s = xcp_slave_create(0x20000000, 4096);
    Loop lp;
    XcpIo io;
    XcpMaster m;
    uint8_t buf[128];
    uint8_t big[80];
    uint8_t dto[XCP_MAX_DTO];
    uint32_t sum = 0, i;
    uint8_t* p;
    int rc, n;
    uint8_t cmd[8], rsp[XCP_MAX_CTO];

    memset(&lp, 0, sizeof(lp));
    lp.slave = s;
    xcp_mutex_init(&lp.mu);
    xcp_cond_init(&lp.cv);
    io.ctx = &lp;
    io.send = loop_send;
    io.recv = loop_recv;
    xcp_master_init(&m, &io);

    expect(xcp_master_connect(&m) == 0, "CONNECT");
    expect(m.max_cto == XCP_MAX_CTO && (m.resource & XCP_RES_DAQ), "CONNECT 资源与 MAX_CTO");

    expect(xcp_synch(xcp_master_session(&m)) == XCP_OK, "SYNCH 返回 CMD_SYNCH");

    cmd[0] = XCP_CMD_PROGRAM_START;
    n = raw(&m, cmd, 1, rsp, (int)sizeof(rsp));
    expect(n == 2 && rsp[1] == XCP_ERR_CMD_UNKNOWN, "未实现的编程命令");

    p = loop_ptr(&lp, 0x20000010, 4);
    p[0] = 0x11; p[1] = 0x22; p[2] = 0x33; p[3] = 0x44;
    expect(xcp_master_upload(&m, 0x20000010, buf, 4) == 0, "SHORT_UPLOAD");
    expect(buf[0] == 0x11 && buf[3] == 0x44, "上传字节");

    for (i = 0; i < 80; i++) big[i] = (uint8_t)(i * 3 + 1);
    expect(xcp_master_download(&m, 0x20000100, big, 80) == 0, "块 DOWNLOAD（超过单包）");
    memset(buf, 0, sizeof(buf));
    expect(xcp_master_upload(&m, 0x20000100, buf, 80) == 0, "块下载后上传");
    expect(memcmp(buf, big, 80) == 0, "80 字节一致");

    expect(xcp_master_checksum(&m, 0x20000100, 80, &sum) == 0, "BUILD_CHECKSUM");
    {
        uint32_t want = 0;
        for (i = 0; i < 80; i++) want += big[i];
        expect(sum == want, "ADD_14 校验和");
    }

    expect(xcp_master_upload(&m, 0x30000000, buf, 4) == -2, "越界地址");
    expect(m.last_err == XCP_ERR_OUT_OF_RANGE, "错误码 OUT_OF_RANGE");

    xcp_slave_lock(s, XCP_RES_CAL_PAG);
    expect(xcp_master_download(&m, 0x20000010, buf, 4) == -2, "加锁后 DOWNLOAD 被拒");
    expect(m.last_err == XCP_ERR_ACCESS_LOCKED, "ACCESS_LOCKED");
    expect(xcp_master_unlock_cal(&m) == 0, "UNLOCK 测试密钥");
    expect(xcp_master_download(&m, 0x20000020, "ABCD", 4) == 0, "解锁后可写");

    memset(big, 0x5A, 16);
    xcp_master_download(&m, 0x20000200, big, 16);
    cmd[0] = XCP_CMD_COPY_CAL_PAGE;
    cmd[1] = 0; cmd[2] = 0; cmd[3] = 0; cmd[4] = 1;
    n = raw(&m, cmd, 5, rsp, (int)sizeof(rsp));
    expect(n == 1 && rsp[0] == XCP_PID_RES, "COPY 工作页到参考页");
    memset(big, 0xA5, 16);
    xcp_master_download(&m, 0x20000200, big, 16);
    cmd[2] = 1; cmd[4] = 0;
    raw(&m, cmd, 5, rsp, (int)sizeof(rsp));
    xcp_master_upload(&m, 0x20000200, buf, 16);
    expect(buf[0] == 0x5A && buf[15] == 0x5A, "参考页拷回后数据还原");

    cmd[0] = XCP_CMD_ALLOC_DAQ;
    cmd[1] = 0;
    xcp_wr16(cmd + 2, 1);
    n = raw(&m, cmd, 4, rsp, (int)sizeof(rsp));
    expect(rsp[1] == XCP_ERR_SEQUENCE, "未 FREE_DAQ 就 ALLOC 被拒");

    expect(xcp_master_daq_alloc(&m, 1, 1, 2) == 0, "动态 DAQ 分配");
    expect(xcp_master_daq_entry(&m, 0, 0, 0, 0x20000010, 4) == 0, "WRITE_DAQ");
    expect(xcp_master_daq_start(&m, 0, 0) == 0, "启动 DAQ 列表");
    p = loop_ptr(&lp, 0x20000010, 4);
    xcp_mutex_lock(&lp.mu);
    p[0] = 0x78; p[1] = 0x56; p[2] = 0x34; p[3] = 0x12;
    xcp_mutex_unlock(&lp.mu);
    loop_tick(&lp, 0, 1000);
    n = xcp_master_recv_dto(&m, dto, (int)sizeof(dto), 1000); /* DTO 由 RX 线程异步入队 */
    expect(n == 1 + 4 + 4 && dto[0] == 0, "DTO：PID + 时间戳 + 4 字节");
    expect(dto[5] == 0x78 && dto[8] == 0x12, "DTO 载荷");

    cmd[0] = XCP_CMD_GET_ID;
    cmd[1] = 0;
    n = raw(&m, cmd, 2, rsp, (int)sizeof(rsp));
    expect(n == 8 && xcp_rd32(rsp + 4) == (uint32_t)strlen(XCP_ID_TEXT), "GET_ID 长度");
    cmd[0] = XCP_CMD_UPLOAD;
    cmd[1] = (uint8_t)strlen(XCP_ID_TEXT);
    n = raw(&m, cmd, 2, rsp, (int)sizeof(rsp));
    expect(n == 1 + (int)strlen(XCP_ID_TEXT) && memcmp(rsp + 1, XCP_ID_TEXT, strlen(XCP_ID_TEXT)) == 0,
           "GET_ID 之后 UPLOAD 标识串");

    expect(xcp_master_disconnect(&m) == 0, "DISCONNECT");
    xcp_master_free(&m); /* 先停 RX 线程，再销毁从机 */
    xcp_cond_destroy(&lp.cv);
    xcp_mutex_destroy(&lp.mu);
    xcp_slave_destroy(s);
    (void)rc;
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("xcp smoke\n");
    expect(xcp_can_backend_count() == 0 && xcp_can_backend(0) == 0,
           "CAN 后端尚未接入（周立功、Kvaser）");
    xcp_transport_init_builtins();
    expect(xcp_transport_count() >= 3, "内置传输 serial/can/tcp");
    expect(xcp_transport_by_name("serial") != 0, "serial 传输");
    expect(xcp_transport_by_name("tcp") != 0, "tcp 传输");
    expect(xcp_transport_by_name("can") != 0, "can 传输占位");
    test_loop();
    if (g_fail) {
        printf("XCP SMOKE FAIL (%d)\n", g_fail);
        return 1;
    }
    printf("XCP SMOKE ALL PASS\n");
    return 0;
}
