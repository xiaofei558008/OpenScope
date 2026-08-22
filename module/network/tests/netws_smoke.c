/* WebSocket 内核单元测试：握手 Accept 键（RFC6455 已知向量）+ 帧编解码（掩码/长度扩展/往返）。 */
#include <stdio.h>
#include <string.h>
#include "netws.h"

static int g_fail;
#define CHECK(c, m) do { if (!(c)) { printf("FAIL %s\n", m); g_fail++; } } while (0)

static void test_accept_key(void)
{
    char acc[64];
    int n = os_ws_accept_key("dGhlIHNhbXBsZSBub25jZQ==", acc, sizeof(acc));
    CHECK(n > 0, "accept_key 产出");
    CHECK(strcmp(acc, "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=") == 0, "accept_key RFC6455 已知向量");
    printf("  accept_key: %s\n", acc);
}

static void test_frame(void)
{
    uint8_t payload[300], out[700], back[700];
    uint8_t mkey[4] = {0x11, 0x22, 0x33, 0x44};
    int i, n, fin, op; uint64_t plen; int consumed;
    for (i = 0; i < (int)sizeof(payload); i++) payload[i] = (uint8_t)(i * 7 + 3);

    /* 短 payload（<126）+ 掩码 */
    n = os_ws_frame_encode(out, sizeof(out), 1, OS_WS_OP_BIN, payload, 125, 1, mkey);
    CHECK(n > 0, "短帧编码");
    CHECK(os_ws_frame_decode(out, n, &fin, &op, back, sizeof(back), &plen, &consumed) == 0, "短帧解码");
    CHECK(fin == 1 && op == OS_WS_OP_BIN && plen == 125 && consumed == n, "短帧字段");
    CHECK(memcmp(back, payload, 125) == 0, "短帧掩码往返无损");

    /* 中 payload（126..65535）不掩码 */
    n = os_ws_frame_encode(out, sizeof(out), 1, OS_WS_OP_BIN, payload, 300, 0, NULL);
    CHECK(n > 0, "中帧编码");
    CHECK(os_ws_frame_decode(out, n, &fin, &op, back, sizeof(back), &plen, &consumed) == 0, "中帧解码");
    CHECK(plen == 300 && memcmp(back, payload, 300) == 0, "中帧不掩码往返无损");

    /* 不完整帧返回 1 */
    CHECK(os_ws_frame_decode(out, n - 1, &fin, &op, back, sizeof(back), &plen, &consumed) == 1, "不完整帧返回1");

    /* 空 payload */
    n = os_ws_frame_encode(out, sizeof(out), 1, OS_WS_OP_PING, NULL, 0, 0, NULL);
    CHECK(n == 2 && out[0] == 0x89 && out[1] == 0x00, "空帧编码");
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    test_accept_key();
    test_frame();
    if (g_fail == 0) { printf("ALL PASS\n"); return 0; }
    printf("FAILED: %d\n", g_fail);
    return 1;
}
