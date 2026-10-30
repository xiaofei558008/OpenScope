/* D 块单测：Mock 从机的 DAQ / 标定页 / Seed&Key / 故障注入（进程内，无传输）。 */
#include "../xcp_mock.h"

#include <stdio.h>
#include <string.h>

static int g_fail, g_pass;
#define EXPECT(c, what) do { if (c) g_pass++; else { g_fail++; printf("FAIL %s (line %d)\n", what, __LINE__); } } while (0)

static XcpMock* M;
static uint8_t R[XCP_MAX_CTO];

static int tx(const uint8_t* c, int n)
{
    XcpMockResult r = xcp_mock_input(M, c, n, R, (int)sizeof(R));
    return r.len;
}
#define TX(...) do { const uint8_t _c[] = { __VA_ARGS__ }; rl = tx(_c, (int)sizeof(_c)); } while (0)
#define OK() (rl >= 1 && R[0] == XCP_PID_RES)
#define ERR(e) (rl == 2 && R[0] == XCP_PID_ERR && R[1] == (e))

/* 自定义 Seed&Key：100 字节种子（强制多段），key[i] = seed[i] * 3 + i + res */
static void my_seed(void* u, uint8_t res, uint32_t ctr, uint8_t* seed, int* len)
{
    int i;
    (void)u; (void)res;
    for (i = 0; i < 100; i++) seed[i] = (uint8_t)(i * 7 + ctr);
    *len = 100;
}
static int my_key(void* u, uint8_t res, const uint8_t* seed, int n, uint8_t* key, int cap)
{
    int i;
    (void)u;
    if (n > cap) return -1;
    for (i = 0; i < n; i++) key[i] = (uint8_t)(seed[i] * 3 + i + res);
    return n;
}

static int unlock_custom(uint8_t res, int corrupt)
{
    uint8_t seed[128], key[128], c[XCP_MAX_CTO];
    int got = 0, total, rl, i, sent = 0;
    c[0] = XCP_CMD_GET_SEED; c[1] = 0; c[2] = res;
    rl = tx(c, 3);
    if (!OK()) return -1;
    total = R[1];
    if (total == 0) return 0;
    for (;;) {
        int chunk = rl - 2;
        memcpy(seed + got, R + 2, (size_t)chunk);
        got += chunk;
        if (got >= total) break;
        c[1] = 1;
        rl = tx(c, 3);
        if (!OK() || R[1] != total - got) return -2;
    }
    my_key(NULL, res, seed, total, key, 128);
    if (corrupt) key[50] ^= 1;
    while (sent < total) {
        int chunk = total - sent > XCP_MAX_CTO - 2 ? XCP_MAX_CTO - 2 : total - sent;
        c[0] = XCP_CMD_UNLOCK;
        c[1] = (uint8_t)(total - sent);
        memcpy(c + 2, key + sent, (size_t)chunk);
        rl = tx(c, 2 + chunk);
        sent += chunk;
        if (!OK()) return R[1];
    }
    for (i = 0; i < 0; i++) {}
    return R[0] == XCP_PID_RES ? 0 : -3;
}

static void test_seed_key(void)
{
    int rl;
    XcpSlave* s = xcp_mock_slave(M);
    xcp_slave_lock(s, XCP_RES_CAL_PAG | XCP_RES_DAQ);
    TX(XCP_CMD_CONNECT, 0);
    EXPECT(OK(), "CONNECT");
    /* 默认算法（兼容老主机）：XOR 0xFF */
    TX(XCP_CMD_GET_SEED, 0, XCP_RES_DAQ);
    EXPECT(OK() && R[1] == 4 && R[2] == 0x11, "默认 4 字节种子");
    TX(XCP_CMD_UNLOCK, 4, 0xEE, 0xDD, 0xCC, 0xBB);
    EXPECT(OK() && R[1] == XCP_RES_CAL_PAG, "默认密钥解锁 DAQ，CAL 仍锁");
    TX(XCP_CMD_SHORT_DOWNLOAD, 1, 0, 0, 0x00, 0x00, 0x00, 0x20, 0x55);
    EXPECT(ERR(XCP_ERR_ACCESS_LOCKED), "CAL 锁定时写入被拒");
    TX(XCP_CMD_UNLOCK, 4, 1, 2, 3, 4);
    EXPECT(ERR(XCP_ERR_SEQUENCE), "无 GET_SEED 直接 UNLOCK → SEQUENCE");

    xcp_slave_set_seed_key(s, my_seed, my_key, NULL);
    EXPECT(unlock_custom(XCP_RES_CAL_PAG, 1) == XCP_ERR_ACCESS_LOCKED, "错误密钥 → ACCESS_LOCKED");
    EXPECT(!xcp_slave_is_connected(s), "错误密钥后断开会话");
    TX(XCP_CMD_CONNECT, 0);
    EXPECT(unlock_custom(XCP_RES_CAL_PAG, 0) == 0, "100 字节多段种子/密钥解锁");
    EXPECT(xcp_slave_protection(s) == 0, "全部资源已解锁");
    TX(XCP_CMD_GET_SEED, 0, XCP_RES_CAL_PAG);
    EXPECT(OK() && R[1] == 0, "已解锁资源种子长度 0");
    TX(XCP_CMD_GET_SEED, 0, 0x03);
    EXPECT(ERR(XCP_ERR_OUT_OF_RANGE), "多资源位 GET_SEED 被拒");
    xcp_slave_set_seed_key(s, NULL, NULL, NULL);
}

static void test_cal_pages(void)
{
    int rl;
    XcpSlave* s = xcp_mock_slave(M);
    uint8_t* ram = xcp_slave_ptr(s, 0x20000100, 4);
    uint8_t* flash = xcp_slave_ref_ptr(s, 0x20000100, 4);
    memcpy(flash, "\x01\x02\x03\x04", 4);
    memcpy(ram, "\xA1\xA2\xA3\xA4", 4);
    TX(XCP_CMD_GET_CAL_PAGE, 2, 0);
    EXPECT(OK() && R[3] == 0, "初始 XCP 页 = 0 (RAM)");
    TX(XCP_CMD_SHORT_UPLOAD, 4, 0, 0, 0x00, 0x01, 0x00, 0x20);
    EXPECT(OK() && R[1] == 0xA1, "RAM 页上传");
    TX(XCP_CMD_SET_CAL_PAGE, 0x83, 0, 1);
    EXPECT(OK(), "SET_CAL_PAGE ALL → FLASH");
    TX(XCP_CMD_GET_CAL_PAGE, 1, 0);
    EXPECT(OK() && R[3] == 1, "ECU 页 = 1");
    TX(XCP_CMD_SHORT_UPLOAD, 4, 0, 0, 0x00, 0x01, 0x00, 0x20);
    EXPECT(OK() && R[1] == 0x01 && R[4] == 0x04, "FLASH 页上传");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x00, 0x01, 0x00, 0x20);
    TX(XCP_CMD_DOWNLOAD, 2, 9, 9);
    EXPECT(ERR(XCP_ERR_WRITE_PROTECTED), "FLASH 页下载 → WRITE_PROTECTED");
    TX(XCP_CMD_SET_CAL_PAGE, 0x02, 0, 0);
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x00, 0x01, 0x00, 0x20);
    TX(XCP_CMD_DOWNLOAD, 2, 0x77, 0x66);
    EXPECT(OK() && ram[0] == 0x77 && ram[1] == 0x66, "切回 RAM 后下载");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x00, 0x01, 0x00, 0x20);
    TX(XCP_CMD_UPLOAD, 3);
    EXPECT(OK() && rl == 4 && R[1] == 0x77 && R[3] == 0xA3, "SET_MTA+UPLOAD 回读");
    TX(XCP_CMD_COPY_CAL_PAGE, 0, 1, 0, 0);
    EXPECT(OK() && ram[0] == 0x01, "COPY FLASH→RAM 恢复参考值");
    TX(XCP_CMD_SET_CAL_PAGE, 0x01, 0, 2);
    EXPECT(ERR(XCP_ERR_PAGE_NOT_VALID), "非法页号");
    TX(XCP_CMD_SET_CAL_PAGE, 0x00, 0, 0);
    EXPECT(ERR(XCP_ERR_MODE_NOT_VALID), "非法模式");
    TX(XCP_CMD_SET_CAL_PAGE, 0x01, 1, 0);
    EXPECT(ERR(XCP_ERR_SEGMENT_NOT_VALID), "非法段");
    TX(XCP_CMD_SET_CAL_PAGE, 0x83, 0, 0);
}

static int setup_daq(int ts)
{
    int rl;
    XcpSlave* s = xcp_mock_slave(M);
    uint8_t* p = xcp_slave_ptr(s, 0x20000000, 8);
    memcpy(p, "\x10\x11\x12\x13\x20\x21", 6);
    TX(XCP_CMD_FREE_DAQ);
    if (!OK()) return -1;
    TX(XCP_CMD_ALLOC_DAQ, 0, 2, 0);
    TX(XCP_CMD_ALLOC_ODT, 0, 0, 0, 2);       /* DAQ0: 2 ODT */
    TX(XCP_CMD_ALLOC_ODT, 0, 1, 0, 1);       /* DAQ1: 1 ODT */
    TX(XCP_CMD_ALLOC_ODT_ENTRY, 0, 0, 0, 0, 1);
    TX(XCP_CMD_ALLOC_ODT_ENTRY, 0, 0, 0, 1, 1);
    TX(XCP_CMD_ALLOC_ODT_ENTRY, 0, 1, 0, 0, 1);
    TX(XCP_CMD_SET_DAQ_PTR, 0, 0, 0, 0, 0);
    TX(XCP_CMD_WRITE_DAQ, 0xFF, 4, 0, 0x00, 0x00, 0x00, 0x20);
    TX(XCP_CMD_SET_DAQ_PTR, 0, 0, 0, 1, 0);
    TX(XCP_CMD_WRITE_DAQ, 0xFF, 2, 0, 0x04, 0x00, 0x00, 0x20);
    TX(XCP_CMD_SET_DAQ_PTR, 0, 1, 0, 0, 0);
    TX(XCP_CMD_WRITE_DAQ, 0xFF, 1, 0, 0x05, 0x00, 0x00, 0x20);
    if (!OK()) return -2;
    TX(XCP_CMD_SET_DAQ_LIST_MODE, (uint8_t)(ts ? XCP_DAQ_FLAG_TIMESTAMP : 0), 0, 0, 0, 0, 1, 0);
    TX(XCP_CMD_SET_DAQ_LIST_MODE, 0, 1, 0, 1, 0, 2, 0);  /* DAQ1：事件 1，预分频 2 */
    TX(XCP_CMD_START_STOP_DAQ_LIST, 2, 0, 0);
    if (!OK() || R[1] != 0) return -3;
    TX(XCP_CMD_START_STOP_DAQ_LIST, 2, 1, 0);
    if (!OK() || R[1] != 2) return -4;  /* FIRST_PID of DAQ1 = 2 */
    return 0;
}

static void drain(void) { uint8_t b[XCP_MAX_DTO]; while (xcp_mock_pop(M, b, sizeof b) > 0) {} }

static void test_daq(void)
{
    int rl, n, i;
    uint8_t b[XCP_MAX_DTO];
    EXPECT(setup_daq(1) == 0, "动态 DAQ 配置 2 列表/3 ODT");
    TX(XCP_CMD_ALLOC_DAQ, 0, 1, 0);
    EXPECT(ERR(XCP_ERR_SEQUENCE), "ALLOC_DAQ 顺序错误");
    TX(XCP_CMD_GET_DAQ_PROCESSOR_INFO);
    EXPECT(OK() && (R[1] & 0x01), "PROCESSOR_INFO 动态");
    TX(XCP_CMD_GET_DAQ_RESOLUTION_INFO);
    EXPECT(OK() && (R[5] & 7) == 4, "时间戳 4 字节");
    xcp_mock_tick(M, 0, 1234);
    EXPECT(xcp_mock_pop(M, b, sizeof b) == 0, "未启动不产生 DTO");
    TX(XCP_CMD_START_STOP_SYNCH, XCP_DAQ_SYNCH_START_SEL);
    EXPECT(OK(), "START_STOP_SYNCH start selected");
    TX(XCP_CMD_GET_STATUS);
    EXPECT(OK() && (R[1] & XCP_SS_DAQ_RUNNING), "GET_STATUS DAQ_RUNNING");
    xcp_mock_tick(M, 0, 0x01020304);
    n = xcp_mock_pop(M, b, sizeof b);
    EXPECT(n == 1 + 4 + 4 && b[0] == 0 && xcp_rd32(b + 1) == 0x01020304 && b[5] == 0x10 && b[8] == 0x13,
           "ODT0：PID0 + 时间戳 + u32");
    n = xcp_mock_pop(M, b, sizeof b);
    EXPECT(n == 3 && b[0] == 1 && b[1] == 0x20, "ODT1：PID1 无时间戳");
    xcp_mock_tick(M, 1, 10);
    EXPECT(xcp_mock_pop(M, b, sizeof b) == 0, "预分频 2：第 1 次事件不发");
    xcp_mock_tick(M, 1, 20);
    n = xcp_mock_pop(M, b, sizeof b);
    EXPECT(n == 2 && b[0] == 2 && b[1] == 0x21, "预分频 2：第 2 次事件发 PID2");
    TX(XCP_CMD_FREE_DAQ);
    EXPECT(ERR(XCP_ERR_DAQ_ACTIVE), "运行中 FREE_DAQ → DAQ_ACTIVE");
    /* 1kHz × 1000 次，队列不溢出（主机及时取） */
    for (i = 0; i < 1000; i++) { xcp_mock_tick(M, 0, (uint32_t)i * 1000u); drain(); }
    EXPECT(xcp_slave_dto_overflow(xcp_mock_slave(M)) == 0, "1kHz 无溢出");
    TX(XCP_CMD_START_STOP_SYNCH, XCP_DAQ_SYNCH_STOP_ALL);
    EXPECT(OK(), "停止全部 DAQ");
    xcp_mock_tick(M, 0, 1);
    EXPECT(xcp_mock_pop(M, b, sizeof b) == 0, "停止后无 DTO");
}

static void test_cmd_faults(void)
{
    int rl;
    XcpMockResult r;
    uint8_t c[8] = { XCP_CMD_GET_STATUS };
    xcp_mock_clear_faults(M);
    EXPECT(xcp_mock_parse_fault(M, "busy:FD:2") == 0, "解析 busy 规则");
    TX(XCP_CMD_GET_STATUS);
    EXPECT(ERR(XCP_ERR_CMD_BUSY), "第 1 次 BUSY");
    TX(XCP_CMD_GET_STATUS);
    EXPECT(ERR(XCP_ERR_CMD_BUSY), "第 2 次 BUSY");
    TX(XCP_CMD_GET_STATUS);
    EXPECT(OK(), "第 3 次正常（重试成功）");

    EXPECT(xcp_mock_parse_fault(M, "err:F4:22") == 0, "解析 err 规则");
    TX(XCP_CMD_SHORT_UPLOAD, 1, 0, 0, 0, 0, 0, 0x20);
    EXPECT(ERR(XCP_ERR_OUT_OF_RANGE), "注入 OUT_OF_RANGE");
    EXPECT(xcp_mock_parse_fault(M, "err:ED:25") == 0, "解析 ACCESS_LOCKED 规则");
    TX(XCP_CMD_SHORT_DOWNLOAD, 1, 0, 0, 0, 0, 0, 0x20, 1);
    EXPECT(ERR(XCP_ERR_ACCESS_LOCKED), "注入 ACCESS_LOCKED");

    EXPECT(xcp_mock_parse_fault(M, "timeout:any:1") == 0, "解析 timeout 规则");
    TX(XCP_CMD_GET_STATUS);
    EXPECT(rl == 0, "超时：无应答");
    TX(XCP_CMD_SYNCH);
    EXPECT(ERR(XCP_ERR_CMD_SYNCH), "SYNCH 恢复 → ERR_CMD_SYNCH");

    /* 执行但丢应答：主机重试时会发现状态已改变 */
    EXPECT(xcp_mock_parse_fault(M, "timeout:ED:1:1") == 0, "解析 timeout exec 规则");
    TX(XCP_CMD_SHORT_DOWNLOAD, 1, 0, 0, 0x00, 0x02, 0x00, 0x20, 0x5C);
    EXPECT(rl == 0 && *xcp_slave_ptr(xcp_mock_slave(M), 0x20000200, 1) == 0x5C, "执行后丢应答");

    EXPECT(xcp_mock_parse_fault(M, "pending:FD:300") == 0, "解析 pending 规则");
    r = xcp_mock_input(M, c, 1, R, sizeof R);
    {
        uint8_t ev[8];
        int n = xcp_mock_pop(M, ev, sizeof ev);
        EXPECT(n == 2 && ev[0] == XCP_PID_EV && ev[1] == 0x05, "EV_CMD_PENDING");
    }
    EXPECT(r.len > 0 && r.delay_ms == 300 && R[0] == XCP_PID_RES, "推迟 300ms 的应答");
    EXPECT(xcp_mock_parse_fault(M, "bogus:1") != 0 && xcp_mock_parse_fault(M, "err:XYZ:1") != 0, "非法规则被拒");
    xcp_mock_clear_faults(M);
}

static void test_daq_faults(void)
{
    uint8_t b[XCP_MAX_DTO];
    int seq[64], n = 0, i, k, rl;
    XcpMockStats st;
    XcpSlave* s = xcp_mock_slave(M);
    /* 只用 DAQ0 ODT0，载荷里放序号 */
    EXPECT(setup_daq(0) == 0, "重新配置 DAQ");
    TX(XCP_CMD_START_STOP_DAQ_LIST, 0, 1, 0);   /* 停 DAQ1 的选择 → 只启动 DAQ0 */
    TX(XCP_CMD_START_STOP_DAQ_LIST, 1, 0, 0);
    EXPECT(xcp_mock_parse_fault(M, "drop:5") == 0 && xcp_mock_parse_fault(M, "swap:7") == 0, "DAQ 故障规则");
    /* 每个 DTO 的全局序号 key = tick*2 + pid（ODT1 载荷带 u16 tick），检查单调性 */
    for (i = 0; i < 20; i += 2) {
        for (k = i; k < i + 2; k++) {
            xcp_wr32(xcp_slave_ptr(s, 0x20000000, 4), (uint32_t)k);
            xcp_wr16(xcp_slave_ptr(s, 0x20000004, 2), (uint16_t)k);
            xcp_mock_tick(M, 0, (uint32_t)k);
        }
        while (xcp_mock_pop(M, b, sizeof b) > 0 && n < 64)
            seq[n++] = b[0] == 0 ? (int)xcp_rd32(b + 1) * 2 : (int)xcp_rd16(b + 1) * 2 + 1;
    }
    xcp_mock_stats(M, &st);
    /* 每 tick 2 个 DTO(ODT0/ODT1)，共 40；每 5 丢 1 → 8 丢 */
    EXPECT(st.dto_dropped == 8, "丢包计数 8");
    EXPECT(n == 32, "收到 32 个 DTO");
    {
        int ooo = 0, missing = 0, seen[40] = { 0 };
        for (i = 0; i < n; i++) { seen[seq[i]] = 1; if (i && seq[i] < seq[i - 1]) ooo++; }
        for (i = 0; i < 40; i++) missing += !seen[i];
        EXPECT(ooo > 0, "出现乱序 DTO");
        EXPECT(missing == 8, "缺失 8 个 DTO");
        EXPECT(st.dto_swapped > 0, "乱序计数");
    }
    (void)rl;
    xcp_mock_clear_faults(M);
    TX(XCP_CMD_START_STOP_SYNCH, 0);
    drain();
}

/* 评审 §2-1：从机块模式 UPLOAD（最多 255 元素，多帧 RES） */
static void test_block_upload(void)
{
    int rl, i, got, frames = 0;
    uint8_t out[1024], f[XCP_MAX_CTO];
    XcpSlave* s = xcp_mock_slave(M);
    uint8_t* p = xcp_slave_ptr(s, 0x20000400, 255);
    for (i = 0; i < 255; i++) p[i] = (uint8_t)(i ^ 0x5A);
    TX(XCP_CMD_CONNECT, 0);
    EXPECT(OK() && (R[2] & XCP_COMM_SLAVE_BLOCK), "CONNECT 声明从机块模式");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x00, 0x04, 0x00, 0x20);
    TX(XCP_CMD_UPLOAD, 200);
    EXPECT(OK() && rl == 64, "UPLOAD 200：首帧 1+63");
    memcpy(out, R + 1, 63); got = 63; frames = 1;
    while ((rl = xcp_mock_pop_rsp(M, f, sizeof f)) > 0) {
        EXPECT(f[0] == XCP_PID_RES, "余帧 PID=RES");
        memcpy(out + got, f + 1, (size_t)(rl - 1)); got += rl - 1; frames++;
    }
    EXPECT(got == 200 && frames == 4 && memcmp(out, p, 200) == 0, "UPLOAD 200 字节 4 帧数据一致");
    TX(XCP_CMD_UPLOAD, 55);
    EXPECT(OK() && rl == 56 && R[1] == p[200] && xcp_mock_pop_rsp(M, f, sizeof f) == 0, "MTA 自增，续传 55 字节单帧");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x00, 0x04, 0x00, 0x20);
    TX(XCP_CMD_UPLOAD, 255);
    got = rl - 1;
    while ((rl = xcp_mock_pop_rsp(M, f, sizeof f)) > 0) got += rl - 1;
    EXPECT(got == 255, "UPLOAD 255（最大）");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x00, 0x04, 0x00, 0x20);
    TX(XCP_CMD_UPLOAD, 200);
    TX(XCP_CMD_GET_STATUS);
    EXPECT(OK() && xcp_mock_pop_rsp(M, f, sizeof f) == 0, "新命令丢弃未取余帧");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0xF0, 0x0F, 0x00, 0x20);
    TX(XCP_CMD_UPLOAD, 200);
    EXPECT(ERR(XCP_ERR_OUT_OF_RANGE) && xcp_mock_pop_rsp(M, f, sizeof f) == 0, "越界块上传 → OUT_OF_RANGE，无余帧");
    TX(XCP_CMD_UPLOAD, 0);
    EXPECT(ERR(XCP_ERR_OUT_OF_RANGE), "UPLOAD 0 被拒");
}

/* 评审 §2-4：运行时字节序 + AG */
static void test_format(void)
{
    int rl, i, got;
    uint8_t f[XCP_MAX_CTO], out[600];
    XcpMock* m2;
    XcpMock* save = M;
    XcpSlave* s;
    uint8_t* p;
    EXPECT(xcp_slave_set_format(xcp_mock_slave(M), 1, 2) == -1, "已连接时不可改格式");
    m2 = xcp_mock_create(0x20000000, 4096);
    s = xcp_mock_slave(m2);
    EXPECT(xcp_slave_set_format(s, 0, 3) == -1, "AG=3 非法");
    EXPECT(xcp_slave_set_format(s, 1, 2) == 0, "Motorola + AG2");
    M = m2;
    p = xcp_slave_ptr(s, 0x20000000, 600);
    for (i = 0; i < 600; i++) p[i] = (uint8_t)i;
    TX(XCP_CMD_CONNECT, 0);
    EXPECT(OK() && (R[2] & 0x07) == (0x01 | (1 << 1)), "COMM_MODE_BASIC：BYTE_ORDER=1, AG=WORD");
    EXPECT(R[4] == 0 && R[5] == 64, "MAX_DTO 按 Motorola 编码");
    TX(XCP_CMD_SHORT_UPLOAD, 4, 0, 0, 0x20, 0x00, 0x00, 0x10);      /* 地址 BE 0x20000010，4 个 WORD */
    EXPECT(OK() && rl == 2 + 8 && R[1] == 0 && R[2] == 0x10 && R[9] == 0x17, "SHORT_UPLOAD：BE 地址，AG2 对齐填充，8 字节");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x20, 0x00, 0x00, 0x00);
    TX(XCP_CMD_UPLOAD, 255);                                           /* 510 字节 */
    EXPECT(OK() && rl == 64 && R[1] == 0, "AG2 块上传首帧 2+62");
    memcpy(out, R + 2, (size_t)(rl - 2)); got = rl - 2;
    while ((rl = xcp_mock_pop_rsp(M, f, sizeof f)) > 0) {
        EXPECT((rl - 2) % 2 == 0, "AG2 每帧数据为 WORD 整数倍");
        memcpy(out + got, f + 2, (size_t)(rl - 2)); got += rl - 2;
    }
    EXPECT(got == 510 && memcmp(out, p, 510) == 0, "AG2 UPLOAD 255 元素 = 510 字节");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x20, 0x00, 0x01, 0x00);
    TX(XCP_CMD_DOWNLOAD, 2, 0xAA, 0xBB, 0xCC, 0xDD);
    EXPECT(OK() && p[0x100] == 0xAA && p[0x103] == 0xDD, "AG2 DOWNLOAD 2 元素");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x20, 0x00, 0x01, 0x00);
    TX(XCP_CMD_BUILD_CHECKSUM, 0, 0, 0, 0, 0, 0, 2);                   /* 2 个 WORD，BE */
    EXPECT(OK() && R[7] == ((0xAA + 0xBB + 0xCC + 0xDD) & 0xFF) && R[6] == 0x03, "BUILD_CHECKSUM：元素单位 + BE 结果");
    TX(XCP_CMD_GET_DAQ_PROCESSOR_INFO);
    EXPECT(OK() && R[2] == 0 && R[3] == XCP_MAX_DAQ, "PROCESSOR_INFO MAX_DAQ 按 BE");
    TX(XCP_CMD_FREE_DAQ);
    TX(XCP_CMD_ALLOC_DAQ, 0, 0, 1);
    TX(XCP_CMD_ALLOC_ODT, 0, 0, 0, 1);
    TX(XCP_CMD_ALLOC_ODT_ENTRY, 0, 0, 0, 0, 1);
    TX(XCP_CMD_SET_DAQ_PTR, 0, 0, 0, 0, 0);
    TX(XCP_CMD_WRITE_DAQ, 0xFF, 5, 0, 0x20, 0x00, 0x00, 0x00);
    EXPECT(ERR(XCP_ERR_OUT_OF_RANGE), "AG2 ODT 条目上限 4 元素");
    TX(XCP_CMD_WRITE_DAQ, 0xFF, 2, 0, 0x20, 0x00, 0x00, 0x00);
    EXPECT(OK(), "WRITE_DAQ 2 元素(4 字节)");
    TX(XCP_CMD_SET_DAQ_LIST_MODE, XCP_DAQ_FLAG_TIMESTAMP, 0, 0, 0, 0, 1, 0);
    TX(XCP_CMD_START_STOP_DAQ_LIST, 1, 0, 0);
    xcp_mock_tick(M, 0, 0x01020304);
    rl = xcp_mock_pop(M, f, sizeof f);
    EXPECT(rl == 2 + 4 + 4 && f[0] == 0 && f[1] == 0 && f[2] == 0x01 && f[5] == 0x04 && f[6] == 0x00 && f[9] == 0x03,
           "DTO：PID+填充、BE 时间戳、4 字节数据");
    M = save;
    xcp_mock_destroy(m2);
    /* AG4 + Intel 快速检查 */
    m2 = xcp_mock_create(0x20000000, 4096);
    EXPECT(xcp_slave_set_format(xcp_mock_slave(m2), 0, 4) == 0, "AG4");
    M = m2;
    TX(XCP_CMD_CONNECT, 0);
    EXPECT(OK() && (R[2] & 0x07) == (2 << 1), "COMM_MODE_BASIC AG=DWORD");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x00, 0x00, 0x00, 0x20);
    TX(XCP_CMD_UPLOAD, 255);
    got = rl - 4;
    while ((rl = xcp_mock_pop_rsp(M, f, sizeof f)) > 0) got += rl - 4;
    EXPECT(got == 1020, "AG4 UPLOAD 255 元素 = 1020 字节");
    TX(XCP_CMD_SET_MTA, 0, 0, 0, 0x00, 0x01, 0x00, 0x20);
    TX(XCP_CMD_DOWNLOAD, 1, 0, 0, 1, 2, 3, 4);
    EXPECT(OK() && *xcp_slave_ptr(xcp_mock_slave(M), 0x20000103, 1) == 4, "AG4 DOWNLOAD 数据偏移 4");
    M = save;
    xcp_mock_destroy(m2);
}

/* 评审 §2-3：错误密钥 = DISCONNECT 语义，清 DAQ 与 DTO 队列 */
static void test_wrong_key_clears(void)
{
    int rl;
    uint8_t b[XCP_MAX_DTO];
    XcpSlave* s = xcp_mock_slave(M);
    TX(XCP_CMD_CONNECT, 0);
    EXPECT(setup_daq(1) == 0, "配置 DAQ");
    TX(XCP_CMD_START_STOP_SYNCH, XCP_DAQ_SYNCH_START_SEL);
    xcp_mock_tick(M, 0, 1);
    xcp_mock_tick(M, 0, 2);
    EXPECT(xcp_slave_dto_pending(s) > 0, "DTO 队列有积压");
    xcp_slave_lock(s, XCP_RES_CAL_PAG);
    TX(XCP_CMD_GET_SEED, 0, XCP_RES_CAL_PAG);
    TX(XCP_CMD_UNLOCK, 4, 0, 0, 0, 0);
    EXPECT(ERR(XCP_ERR_ACCESS_LOCKED) && !xcp_slave_is_connected(s), "错误密钥断开");
    EXPECT(xcp_slave_dto_pending(s) == 0 && xcp_mock_pop(M, b, sizeof b) == 0, "DTO 队列已清空");
    xcp_mock_tick(M, 0, 3);
    EXPECT(xcp_slave_dto_pending(s) == 0, "DAQ 已停止，不再产生 DTO");
    TX(XCP_CMD_CONNECT, 0);
    TX(XCP_CMD_GET_STATUS);
    EXPECT(OK() && !(R[1] & XCP_SS_DAQ_RUNNING), "重连后 DAQ 未运行");
    TX(XCP_CMD_GET_DAQ_LIST_INFO, 0, 0, 0);
    EXPECT(ERR(XCP_ERR_OUT_OF_RANGE), "DAQ 列表已释放");
    /* DISCONNECT 同样清理 */
    EXPECT(setup_daq(1) == 0, "重新配置 DAQ");
    TX(XCP_CMD_START_STOP_SYNCH, XCP_DAQ_SYNCH_START_SEL);
    xcp_mock_tick(M, 0, 4);
    TX(XCP_CMD_DISCONNECT);
    EXPECT(OK() && xcp_slave_dto_pending(s) == 0, "DISCONNECT 清 DTO 队列");
    TX(XCP_CMD_CONNECT, 0);
    xcp_slave_set_seed_key(s, NULL, NULL, NULL);
    TX(XCP_CMD_GET_SEED, 0, XCP_RES_CAL_PAG);
    TX(XCP_CMD_UNLOCK, 4, 0xEE, 0xDD, 0xCC, 0xBB);
}

/* 0004：GET_ID + UPLOAD 在 AG1/2/4 × Intel/Motorola 下，标识串按 AG 补零 */
static void test_get_id_ag(void)
{
    static const int ags[3] = { 1, 2, 4 };
    int a, bo, rl, idn = (int)strlen(XCP_ID_TEXT);
    XcpMock* save = M;
    for (bo = 0; bo < 2; bo++)
        for (a = 0; a < 3; a++) {
            int ag = ags[a], elems = (idn + ag - 1) / ag, len;
            char what[80];
            XcpMock* m2 = xcp_mock_create(0x20000000, 4096);
            uint8_t c[2];
            M = m2;
            xcp_slave_set_format(xcp_mock_slave(m2), bo, ag);
            TX(XCP_CMD_CONNECT, 0);
            TX(XCP_CMD_GET_ID, 0);
            len = bo ? (R[4] << 24 | R[5] << 16 | R[6] << 8 | R[7]) : (int)xcp_rd32(R + 4);
            snprintf(what, sizeof what, "GET_ID 长度=13 字节（AG%d %s）", ag, bo ? "Motorola" : "Intel");
            EXPECT(OK() && rl == 8 && len == idn, what);
            c[0] = XCP_CMD_UPLOAD; c[1] = (uint8_t)elems;
            rl = tx(c, 2);
            snprintf(what, sizeof what, "UPLOAD %d 元素 = %d 字节，补零（AG%d %s）", elems, elems * ag, ag, bo ? "Motorola" : "Intel");
            EXPECT(OK() && rl == ag + elems * ag && memcmp(R + ag, XCP_ID_TEXT, (size_t)idn) == 0 &&
                   (elems * ag == idn || R[ag + idn] == 0) && R[ag + elems * ag - 1] == (elems * ag == idn ? 'P' : 0), what);
            rl = tx(c, 2);
            snprintf(what, sizeof what, "读过补零边界 → OUT_OF_RANGE（AG%d）", ag);
            EXPECT(ERR(XCP_ERR_OUT_OF_RANGE), what);
            /* 分两段读：先 1 元素再余下 */
            TX(XCP_CMD_GET_ID, 0);
            c[1] = 1; rl = tx(c, 2);
            EXPECT(OK() && R[ag] == 'O', "分段读首元素");
            c[1] = (uint8_t)(elems - 1); rl = tx(c, 2);
            EXPECT(OK() && rl == ag + (elems - 1) * ag, "分段读余下元素");
            xcp_mock_destroy(m2);
        }
    M = save;
}

int main(void)
{
    M = xcp_mock_create(0x20000000, 4096);
    test_seed_key();
    test_cal_pages();
    test_daq();
    test_cmd_faults();
    test_daq_faults();
    test_block_upload();
    test_format();
    test_wrong_key_clears();
    test_get_id_ag();
    xcp_mock_destroy(M);
    printf("xcp_mock_test: %d pass, %d fail\n", g_pass, g_fail);
    return g_fail ? 1 : 0;
}
