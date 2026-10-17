/* XCP 1.3 协议冒烟：进程内主从回环 + 本机 TCP。 */
#include "xcp_master.h"
#include "xcp_slave.h"
#include "xcp_eth.h"

#include <windows.h>
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
    uint8_t rsp[XCP_MAX_CTO];
    int rsp_len;
    int ready;
} Loop;

static int loop_send(void* ctx, const uint8_t* data, int len)
{
    Loop* L = (Loop*)ctx;
    uint8_t rsp[XCP_MAX_CTO];
    int n = xcp_slave_input(L->slave, data, len, rsp, (int)sizeof(rsp));
    if (n > 0) {
        memcpy(L->rsp, rsp, (size_t)n);
        L->rsp_len = n;
        L->ready = 1;
    }
    return len;
}

static int loop_recv(void* ctx, uint8_t* data, int cap, int timeout_ms)
{
    Loop* L = (Loop*)ctx;
    int n;
    (void)timeout_ms;
    if (L->ready) {
        if (L->rsp_len > cap) return -1;
        memcpy(data, L->rsp, (size_t)L->rsp_len);
        n = L->rsp_len;
        L->ready = 0;
        return n;
    }
    return xcp_slave_pop_dto(L->slave, data, cap);
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
    io.ctx = &lp;
    io.send = loop_send;
    io.recv = loop_recv;
    xcp_master_init(&m, &io);

    expect(xcp_master_connect(&m) == 0, "CONNECT");
    expect(m.max_cto == XCP_MAX_CTO && (m.resource & XCP_RES_DAQ), "CONNECT 资源与 MAX_CTO");

    cmd[0] = XCP_CMD_SYNCH;
    loop_send(&lp, cmd, 1);
    n = loop_recv(&lp, rsp, (int)sizeof(rsp), 0);
    expect(n == 2 && rsp[0] == XCP_PID_ERR && rsp[1] == XCP_ERR_CMD_SYNCH, "SYNCH 返回 CMD_SYNCH");

    cmd[0] = XCP_CMD_PROGRAM_START;
    loop_send(&lp, cmd, 1);
    n = loop_recv(&lp, rsp, (int)sizeof(rsp), 0);
    expect(n == 2 && rsp[1] == XCP_ERR_CMD_UNKNOWN, "未实现的编程命令");

    p = xcp_slave_ptr(s, 0x20000010, 4);
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
    loop_send(&lp, cmd, 5);
    n = loop_recv(&lp, rsp, (int)sizeof(rsp), 0);
    expect(n == 1 && rsp[0] == XCP_PID_RES, "COPY 工作页到参考页");
    memset(big, 0xA5, 16);
    xcp_master_download(&m, 0x20000200, big, 16);
    cmd[2] = 1; cmd[4] = 0;
    loop_send(&lp, cmd, 5);
    loop_recv(&lp, rsp, (int)sizeof(rsp), 0);
    xcp_master_upload(&m, 0x20000200, buf, 16);
    expect(buf[0] == 0x5A && buf[15] == 0x5A, "参考页拷回后数据还原");

    cmd[0] = XCP_CMD_ALLOC_DAQ;
    cmd[1] = 0;
    xcp_wr16(cmd + 2, 1);
    loop_send(&lp, cmd, 4);
    n = loop_recv(&lp, rsp, (int)sizeof(rsp), 0);
    expect(rsp[1] == XCP_ERR_SEQUENCE, "未 FREE_DAQ 就 ALLOC 被拒");

    expect(xcp_master_daq_alloc(&m, 1, 1, 2) == 0, "动态 DAQ 分配");
    expect(xcp_master_daq_entry(&m, 0, 0, 0, 0x20000010, 4) == 0, "WRITE_DAQ");
    expect(xcp_master_daq_start(&m, 0, 0) == 0, "启动 DAQ 列表");
    p = xcp_slave_ptr(s, 0x20000010, 4);
    p[0] = 0x78; p[1] = 0x56; p[2] = 0x34; p[3] = 0x12;
    xcp_slave_tick(s, 0, 1000);
    n = xcp_master_recv_dto(&m, dto, (int)sizeof(dto), 0);
    expect(n == 1 + 4 + 4 && dto[0] == 0, "DTO：PID + 时间戳 + 4 字节");
    expect(dto[5] == 0x78 && dto[8] == 0x12, "DTO 载荷");

    cmd[0] = XCP_CMD_GET_ID;
    cmd[1] = 0;
    loop_send(&lp, cmd, 2);
    n = loop_recv(&lp, rsp, (int)sizeof(rsp), 0);
    expect(n == 8 && xcp_rd32(rsp + 4) == (uint32_t)strlen(XCP_ID_TEXT), "GET_ID 长度");
    cmd[0] = XCP_CMD_UPLOAD;
    cmd[1] = (uint8_t)strlen(XCP_ID_TEXT);
    loop_send(&lp, cmd, 2);
    n = loop_recv(&lp, rsp, (int)sizeof(rsp), 0);
    expect(n == 1 + (int)strlen(XCP_ID_TEXT) && memcmp(rsp + 1, XCP_ID_TEXT, strlen(XCP_ID_TEXT)) == 0,
           "GET_ID 之后 UPLOAD 标识串");

    expect(xcp_master_disconnect(&m) == 0, "DISCONNECT");
    xcp_slave_destroy(s);
    (void)rc;
}

static void test_tcp(void)
{
    XcpSlave* s = xcp_slave_create(0x20000000, 1024);
    XcpEthServer* sv;
    XcpEthClient* cli;
    XcpIo io;
    XcpMaster m;
    uint8_t* p;
    uint8_t buf[8];
    int i;

    p = xcp_slave_ptr(s, 0x20000000, 4);
    p[0] = 0xDE; p[1] = 0xAD; p[2] = 0xBE; p[3] = 0xEF;
    sv = xcp_eth_serve(s, 0);
    expect(sv && xcp_eth_port(sv) > 0, "TCP 从机监听");
    cli = NULL;
    for (i = 0; i < 50 && !cli; i++) {
        Sleep(20);
        cli = xcp_eth_connect("127.0.0.1", xcp_eth_port(sv));
    }
    expect(cli != NULL, "TCP 主机连上从机");
    if (cli) {
        xcp_eth_io(cli, &io);
        xcp_master_init(&m, &io);
        expect(xcp_master_connect(&m) == 0, "TCP CONNECT");
        expect(xcp_master_upload(&m, 0x20000000, buf, 4) == 0, "TCP UPLOAD");
        expect(buf[0] == 0xDE && buf[3] == 0xEF, "TCP 载荷");
        expect(xcp_master_download(&m, 0x20000004, "WXYZ", 4) == 0, "TCP DOWNLOAD");
        expect(p[4] == 'W' && p[7] == 'Z', "TCP 写入从机内存");
        xcp_master_disconnect(&m);
        xcp_eth_close(cli);
    }
    xcp_eth_serve_stop(sv);
    xcp_slave_destroy(s);
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("xcp smoke\n");
    test_loop();
    test_tcp();
    if (g_fail) {
        printf("XCP SMOKE FAIL (%d)\n", g_fail);
        return 1;
    }
    printf("XCP SMOKE ALL PASS\n");
    return 0;
}
