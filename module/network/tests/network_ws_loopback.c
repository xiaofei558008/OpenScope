/* WebSocket + 协议会话集成测试：本机回环（服务端线程 + 客户端）。
 * 验证握手 → HELLO/ELF_SYNC → WATCH_LIST/ACK → WRITE_VAR/ACK 全链路。 */
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "netws.h"
#include "netproto.h"
#include "netsession.h"

static int g_fail;
#define CHECK(c, m) do { if (!(c)) { printf("FAIL %s\n", m); g_fail++; } } while (0)

static int ws_send_msg(OS_WSConn* c, uint8_t type, const uint8_t* payload, uint32_t len)
{
    uint8_t frame[4096];
    int n = os_net_frame_encode(frame, sizeof(frame), type, 0, 1, payload, len);
    if (n < 0) return -1;
    return os_ws_send_bin(c, frame, (uint32_t)n);
}

static int ws_recv_msg(OS_WSConn* c, OS_NetFrame* f, uint8_t* buf, int cap)
{
    int op, n;
    for (;;) {
        n = os_ws_recv(c, buf, cap, &op);
        if (n <= 0) return -1;
        if (op == OS_WS_OP_BIN && n >= OS_NET_FRAME_HDR) {
            if (os_net_frame_decode(buf, n, f) > 0) return 0;
        }
    }
}

/* 服务端：接受 1 个连接，按协议应答 */
static DWORD WINAPI server_thread(LPVOID p)
{
    int lsock = (int)(INT_PTR)p;
    OS_WSConn* c = os_ws_accept(lsock);
    OS_NetFrame f; uint8_t buf[4096];
    OS_NetVar vars[3];
    int i, cnt;
    if (!c) { printf("FAIL accept\n"); g_fail++; return 1; }

    /* 1. 收 HELLO → 回 ELF_SYNC */
    if (ws_recv_msg(c, &f, buf, sizeof(buf)) != 0 || f.type != OS_NET_MSG_HELLO) { printf("FAIL hello\n"); g_fail++; return 1; }
    for (i = 0; i < 3; i++) { _snprintf(vars[i].name, OS_NET_NAME_MAX, "var_%d", i); vars[i].addr = 0x20000000 + i*4; vars[i].size = 4; }
    cnt = os_net_encode_varlist(vars, 3, buf, sizeof(buf));
    ws_send_msg(c, OS_NET_MSG_ELF_SYNC, buf, (uint32_t)cnt);

    /* 2. 收 WATCH_LIST → 回 ACK */
    if (ws_recv_msg(c, &f, buf, sizeof(buf)) != 0 || f.type != OS_NET_MSG_WATCH_LIST) { printf("FAIL watch\n"); g_fail++; return 1; }
    cnt = os_net_encode_ack(0, "watch ok", buf, sizeof(buf));
    ws_send_msg(c, OS_NET_MSG_ACK, buf, (uint32_t)cnt);

    /* 3. 收 WRITE_VAR → 回 ACK */
    if (ws_recv_msg(c, &f, buf, sizeof(buf)) != 0 || f.type != OS_NET_MSG_WRITE_VAR) { printf("FAIL write\n"); g_fail++; return 1; }
    cnt = os_net_encode_ack(0, "ok", buf, sizeof(buf));
    ws_send_msg(c, OS_NET_MSG_ACK, buf, (uint32_t)cnt);

    os_ws_close(c);
    return 0;
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    int lsock = os_ws_listen("127.0.0.1", 10000);
    HANDLE th; OS_WSConn* c; OS_NetFrame f; uint8_t buf[4096];
    if (lsock < 0) { printf("FAIL listen\n"); return 1; }
    th = CreateThread(NULL, 0, server_thread, (LPVOID)(INT_PTR)lsock, 0, NULL);
    Sleep(100);

    c = os_ws_connect("127.0.0.1", 10000);
    if (!c) { printf("FAIL connect\n"); return 1; }

    /* 客户端：HELLO → ELF_SYNC */
    { uint8_t z = 0; ws_send_msg(c, OS_NET_MSG_HELLO, &z, 0); }
    CHECK(ws_recv_msg(c, &f, buf, sizeof(buf)) == 0 && f.type == OS_NET_MSG_ELF_SYNC, "收 ELF_SYNC");
    {
        OS_NetVar vars[8]; int n = os_net_decode_varlist(f.payload, (int)f.len, vars, 8);
        CHECK(n == 3, "ELF_SYNC 3 项");
        CHECK(n == 3 && strcmp(vars[0].name, "var_0") == 0 && vars[0].addr == 0x20000000u, "ELF_SYNC 字段");
    }

    /* WATCH_LIST → ACK */
    { const char* names[2] = { "var_0", "var_1" }; int n = os_net_encode_names(names, 2, buf, sizeof(buf)); ws_send_msg(c, OS_NET_MSG_WATCH_LIST, buf, (uint32_t)n); }
    CHECK(ws_recv_msg(c, &f, buf, sizeof(buf)) == 0 && f.type == OS_NET_MSG_ACK, "收 WATCH ACK");

    /* WRITE_VAR → ACK */
    { int n = os_net_encode_write("var_0", "123.5", buf, sizeof(buf)); ws_send_msg(c, OS_NET_MSG_WRITE_VAR, buf, (uint32_t)n); }
    CHECK(ws_recv_msg(c, &f, buf, sizeof(buf)) == 0 && f.type == OS_NET_MSG_ACK, "收 WRITE ACK");

    os_ws_close(c);
    os_ws_close_listen(lsock);
    WaitForSingleObject(th, 3000);
    CloseHandle(th);

    if (g_fail == 0) { printf("ALL PASS\n"); return 0; }
    printf("FAILED: %d\n", g_fail);
    return 1;
}
