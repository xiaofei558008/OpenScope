/* xcp_mock_slave：XCP-on-TCP 模拟从机（Windows 构建为 xcp_mock_slave.exe，放 D:）。
 * 用法：xcp_mock_slave [--port 5555] [--fault SPEC]... [--lock cal,daq] [--seconds N]
 *                      [--byte-order intel|motorola] [--ag 1|2|4]
 *   --seconds N：到时即退出（无论是否有连接、客户端是否已断开）。
 *   --byte-order/--ag：协议字段字节序与地址粒度（XCP-on-TCP 头始终为 Intel，按 Part3 规定）；
 *                      演示变量也按所选字节序存放。
 *   SPEC 见 xcp_mock.h：timeout:F5:1  err:F0:25:2  busy:any:3  pending:F3:500  delay:FF:3000
 *                       drop:50  swap:20  dup:100
 * 内存：0x20000000..+64KiB。演示变量：
 *   0x20000000 u32 counter（1ms 事件 0 自增）  0x20000004 f32 sine（1kHz 更新）
 *   0x20000008 u16 ramp（10ms 事件 1 自增）
 * 事件：0 = 1ms，1 = 10ms。DAQ 时间戳 4 字节 1us。
 * 仅监听 127.0.0.1（测试用），一次一个主机连接。 */
#include "xcp_mock.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET sock_t;
#define CLOSESOCK closesocket
static uint64_t now_us(void)
{
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (uint64_t)(c.QuadPart * 1000000.0 / f.QuadPart);
}
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
typedef int sock_t;
#define INVALID_SOCKET (-1)
#define CLOSESOCK close
static uint64_t now_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000u + (uint64_t)ts.tv_nsec / 1000u;
}
#endif

#define BASE 0x20000000u

static uint16_t g_ctr;
static int g_msb;

static int send_pkt(sock_t c, const uint8_t* p, int n)
{
    uint8_t buf[4 + XCP_MAX_CTO + XCP_MAX_DTO];
    if (n < 0 || n > XCP_MAX_CTO + XCP_MAX_DTO) return -1;
    buf[0] = (uint8_t)n; buf[1] = (uint8_t)(n >> 8);
    buf[2] = (uint8_t)g_ctr; buf[3] = (uint8_t)(g_ctr >> 8);
    g_ctr++;
    memcpy(buf + 4, p, (size_t)n);
    return (int)send(c, (const char*)buf, (size_t)(4 + n), 0) == 4 + n ? 0 : -1;
}

static uint32_t rd32m(const uint8_t* p)
{
    return g_msb ? ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3] : xcp_rd32(p);
}
static void wr32m(uint8_t* p, uint32_t v)
{
    if (g_msb) { p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16); p[2] = (uint8_t)(v >> 8); p[3] = (uint8_t)v; }
    else xcp_wr32(p, v);
}

static void send_reply(sock_t c, XcpMock* m, const uint8_t* rsp, int n)
{
    uint8_t more[XCP_MAX_CTO];
    int k;
    send_pkt(c, rsp, n);
    while ((k = xcp_mock_pop_rsp(m, more, (int)sizeof(more))) > 0) send_pkt(c, more, k);
}

static void update_vars(XcpSlave* s, uint64_t t_us, int ev)
{
    uint8_t* p = xcp_slave_ptr(s, BASE, 16);
    if (ev == 0) {
        uint32_t c = rd32m(p) + 1;
        float f = (float)sin((double)t_us * 2e-6 * 3.14159265);
        uint32_t fb;
        memcpy(&fb, &f, 4);
        wr32m(p, c);
        wr32m(p + 4, fb);
    } else {
        uint16_t r = (uint16_t)(g_msb ? (p[8] << 8 | p[9]) : xcp_rd16(p + 8));
        r++;
        if (g_msb) { p[8] = (uint8_t)(r >> 8); p[9] = (uint8_t)r; } else xcp_wr16(p + 8, r);
    }
}

int main(int argc, char** argv)
{
    int port = 5555, i, seconds = 0, msb = 0, ag = 1;
    XcpMock* m = xcp_mock_create(BASE, 64 * 1024);
    sock_t ls, c;
    struct sockaddr_in a;
    int one = 1;
    uint64_t t0;
    if (!m) return 1;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--port") && i + 1 < argc) port = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--seconds") && i + 1 < argc) seconds = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--fault") && i + 1 < argc) {
            if (xcp_mock_parse_fault(m, argv[++i]) != 0) {
                fprintf(stderr, "bad fault spec: %s\n", argv[i]);
                return 2;
            }
        } else if (!strcmp(argv[i], "--lock") && i + 1 < argc) {
            const char* l = argv[++i];
            if (strstr(l, "cal")) xcp_slave_lock(xcp_mock_slave(m), XCP_RES_CAL_PAG);
            if (strstr(l, "daq")) xcp_slave_lock(xcp_mock_slave(m), XCP_RES_DAQ);
        } else if (!strcmp(argv[i], "--byte-order") && i + 1 < argc) {
            const char* b = argv[++i];
            if (!strcmp(b, "motorola") || !strcmp(b, "msb")) msb = 1;
            else if (!strcmp(b, "intel") || !strcmp(b, "lsb")) msb = 0;
            else { fprintf(stderr, "bad --byte-order: %s\n", b); return 2; }
        } else if (!strcmp(argv[i], "--ag") && i + 1 < argc) {
            ag = atoi(argv[++i]);
        } else {
            fprintf(stderr, "usage: %s [--port N] [--fault SPEC]... [--lock cal,daq] [--seconds N] [--byte-order intel|motorola] [--ag 1|2|4]\n", argv[0]);
            return 2;
        }
    }
    if (xcp_slave_set_format(xcp_mock_slave(m), msb, ag) != 0) {
        fprintf(stderr, "bad --ag (1/2/4)\n");
        return 2;
    }
    g_msb = msb;
#ifdef _WIN32
    { WSADATA w; WSAStartup(MAKEWORD(2, 2), &w); }
#endif
    ls = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (const char*)&one, sizeof(one));
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_port = htons((uint16_t)port);
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(ls, (struct sockaddr*)&a, sizeof(a)) != 0 || listen(ls, 1) != 0) {
        fprintf(stderr, "bind/listen 127.0.0.1:%d failed\n", port);
        return 1;
    }
    printf("xcp_mock_slave listening 127.0.0.1:%d\n", port);
    fflush(stdout);
    t0 = now_us();
    for (;;) {
        uint8_t rx[4 + 1024];
        int have = 0;
        uint64_t next1 = now_us(), next10 = next1;
        uint64_t pend_at = 0;
        uint8_t pend[XCP_MAX_CTO];
        int pend_len = 0;
        {
            /* 等连接时也检查 --seconds（客户端断开后不会卡在 accept） */
            fd_set as;
            struct timeval atv = { 0, 100000 };
            FD_ZERO(&as);
            FD_SET(ls, &as);
            if (select((int)ls + 1, &as, NULL, NULL, &atv) <= 0) {
                if (seconds && now_us() - t0 > (uint64_t)seconds * 1000000u) break;
                continue;
            }
        }
        c = accept(ls, NULL, NULL);
        if (c == INVALID_SOCKET) break;
        setsockopt(c, IPPROTO_TCP, TCP_NODELAY, (const char*)&one, sizeof(one));
        g_ctr = 0;
        for (;;) {
            fd_set rs;
            struct timeval tv = { 0, 500 };
            uint64_t t;
            uint8_t pkt[XCP_MAX_DTO];
            int n;
            FD_ZERO(&rs);
            FD_SET(c, &rs);
            if (select((int)c + 1, &rs, NULL, NULL, &tv) > 0) {
                n = (int)recv(c, (char*)rx + have, (size_t)((int)sizeof(rx) - have), 0);
                if (n <= 0) break;
                have += n;
                while (have >= 4) {
                    int L = rx[0] | (rx[1] << 8);
                    if (L < 1 || L > XCP_MAX_CTO) { have = -1; break; }
                    if (have < 4 + L) break;
                    {
                        uint8_t rsp[XCP_MAX_CTO];
                        XcpMockResult r = xcp_mock_input(m, rx + 4, L, rsp, (int)sizeof(rsp));
                        if (r.len > 0) {
                            if (r.delay_ms > 0) {
                                memcpy(pend, rsp, (size_t)r.len);
                                pend_len = r.len;
                                pend_at = now_us() + (uint64_t)r.delay_ms * 1000u;
                            } else
                                send_reply(c, m, rsp, r.len);
                        }
                    }
                    memmove(rx, rx + 4 + L, (size_t)(have - 4 - L));
                    have -= 4 + L;
                }
                if (have < 0) break;
            }
            t = now_us();
            while (t >= next1) {
                update_vars(xcp_mock_slave(m), next1, 0);
                xcp_mock_tick(m, 0, (uint32_t)(next1 - t0));
                next1 += 1000;
            }
            while (t >= next10) {
                update_vars(xcp_mock_slave(m), next10, 1);
                xcp_mock_tick(m, 1, (uint32_t)(next10 - t0));
                next10 += 10000;
            }
            if (pend_len && t >= pend_at) {
                send_reply(c, m, pend, pend_len);
                pend_len = 0;
            }
            while ((n = xcp_mock_pop(m, pkt, (int)sizeof(pkt))) > 0)
                if (send_pkt(c, pkt, n) != 0) break;
            if (seconds && t - t0 > (uint64_t)seconds * 1000000u) break;
        }
        CLOSESOCK(c);
        if (seconds && now_us() - t0 > (uint64_t)seconds * 1000000u) break;
    }
    CLOSESOCK(ls);
    xcp_mock_destroy(m);
    return 0;
}
