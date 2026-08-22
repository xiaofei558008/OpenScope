/* 网络内核单元测试：varint / 帧编解码 / 时序样本编解码（无损往返 + 压缩）/ 分块重组。
 * 不依赖 socket，纯内存验证。 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "netproto.h"
#include "netcodec.h"
#include "netbuf.h"

static int g_fail;

#define CHECK(c, m) do { if (c) { /*printf("PASS %s\n", m);*/ } else { printf("FAIL %s\n", m); g_fail++; } } while (0)

static int chunk_cb(void* ud, uint32_t idx, uint32_t total, const uint8_t* d, uint32_t l)
{
    OS_NetAccum* a = (OS_NetAccum*)ud;
    (void)idx; (void)total;
    return os_net_accum_append(a, d, l) == 0 ? 0 : -1;
}

static void test_varint(void)
{
    uint64_t vals[] = { 0, 1, 127, 128, 300, 65535, 1ULL << 40, ~0ULL };
    int i, bad = 0;
    for (i = 0; i < (int)(sizeof(vals) / sizeof(vals[0])); i++) {
        uint8_t buf[16]; uint64_t v; int w, c;
        w = os_net_put_uvarint(buf, sizeof(buf), vals[i]);
        if (w <= 0 || os_net_get_uvarint(buf, w, &v, &c) != 0 || v != vals[i]) bad++;
    }
    CHECK(bad == 0, "varint 往返一致");
}

static void test_frame(void)
{
    uint8_t buf[256], payload[16];
    OS_NetFrame f;
    int n, i;
    for (i = 0; i < 16; i++) payload[i] = (uint8_t)i;
    n = os_net_frame_encode(buf, sizeof(buf), OS_NET_MSG_SAMPLE_BATCH, 1, 42, payload, 16);
    CHECK(n == OS_NET_FRAME_HDR + 16, "帧编码长度");
    CHECK(os_net_frame_decode(buf, n, &f) == n, "帧解码长度");
    CHECK(f.type == OS_NET_MSG_SAMPLE_BATCH && f.seq == 42 && f.len == 16, "帧字段");
    CHECK(memcmp(f.payload, payload, 16) == 0, "帧 payload");
    CHECK(os_net_frame_decode(buf, OS_NET_FRAME_HDR - 1, &f) == 0, "帧不完整返回0");
    buf[0] = 0x00;
    CHECK(os_net_frame_decode(buf, n, &f) == -1, "帧 magic 错误返回-1");
}

static void test_codec(void)
{
    OS_NetSample in[100], out[100];
    uint8_t buf[4096];
    int n = 100, enc, dec, i, ok = 1;
    for (i = 0; i < n; i++) { in[i].ts_us = 1000000LL + i * 250; in[i].value = i * 0.5 + (i % 3); }
    enc = os_net_codec_encode(in, n, buf, sizeof(buf));
    CHECK(enc > 0, "样本编码");
    dec = os_net_codec_decode(buf, enc, out, 100);
    CHECK(dec == n, "样本解码数量");
    for (i = 0; i < n; i++)
        if (out[i].ts_us != in[i].ts_us || out[i].value != in[i].value) { ok = 0; break; }
    CHECK(ok, "样本无损往返");
    /* 常量值应显著压缩 */
    for (i = 0; i < n; i++) { in[i].ts_us = 1000LL + i * 100; in[i].value = 3.25; }
    enc = os_net_codec_encode(in, n, buf, sizeof(buf));
    printf("  codec: 100 常量样本 %d 字节（原始 %d 字节，%.1f%%）\n", enc, n * 16, enc * 100.0 / (n * 16));
    CHECK(enc > 0 && enc < n * 16 / 2, "常量样本压缩 >50%");
}

static void test_chunk(void)
{
    uint8_t data[100000];
    int i, total, ok = 1;
    OS_NetAccum acc;
    for (i = 0; i < (int)sizeof(data); i++) data[i] = (uint8_t)(i * 31 + 7);
    os_net_accum_init(&acc);
    total = os_net_chunk_split(data, sizeof(data), 8192, &acc, chunk_cb);
    CHECK(total == (int)((sizeof(data) + 8191) / 8192), "分块数量");
    CHECK(acc.len == sizeof(data), "重组长度");
    for (i = 0; i < (int)sizeof(data); i++) if (acc.buf[i] != data[i]) { ok = 0; break; }
    CHECK(ok, "分块无损重组");
    os_net_accum_free(&acc);
    /* 边界：恰好整块 */
    os_net_accum_init(&acc);
    total = os_net_chunk_split(data, 8192, 8192, &acc, chunk_cb);
    CHECK(total == 1 && acc.len == 8192, "分块整块边界");
    os_net_accum_free(&acc);
}

static void test_spool(void)
{
    OS_NetSpool sp;
    uint8_t data[10000], out[10000];
    uint32_t outlen = 0;
    int i, ok = 1;
    for (i = 0; i < (int)sizeof(data); i++) data[i] = (uint8_t)(i * 13 + 5);
    os_net_spool_init(&sp, 1024); /* 小 RAM 容量，强制转盘 */
    for (i = 0; i < (int)sizeof(data); i += 77) {
        uint32_t l = (uint32_t)(sizeof(data) - i < 77 ? sizeof(data) - i : 77);
        if (os_net_spool_append(&sp, data + i, l) != 0) { ok = 0; break; }
    }
    CHECK(sp.spilled == 1, "异步缓冲转盘触发");
    if (os_net_spool_read(&sp, out, sizeof(out), &outlen) != 0) ok = 0;
    CHECK(outlen == sizeof(data), "异步缓冲读回长度");
    if (ok) for (i = 0; i < (int)sizeof(data); i++) if (out[i] != data[i]) { ok = 0; break; }
    CHECK(ok, "异步缓冲 RAM+盘无损读回");
    os_net_spool_free(&sp);
}

static void test_parallel(void)
{
    OS_NetSample in[1000], out[1000];
    uint8_t buf[32768];
    int n = 1000, enc, dec, i, ok = 1;
    for (i = 0; i < n; i++) { in[i].ts_us = 1000000LL + i * 250; in[i].value = i * 0.1 + (i % 5) * 0.3; }
    enc = os_net_codec_encode_parallel(in, n, 8, buf, sizeof(buf)); /* 8 核并行 */
    CHECK(enc > 0, "并行压缩编码");
    dec = os_net_codec_decode_parallel(buf, enc, out, 1000);
    CHECK(dec == n, "并行压缩解码数量");
    for (i = 0; i < n; i++)
        if (out[i].ts_us != in[i].ts_us || out[i].value != in[i].value) { ok = 0; break; }
    CHECK(ok, "并行压缩无损往返（8 核）");
    printf("  parallel: 1000 样本 %d 字节（8 线程）\n", enc);
}

static void test_flat(void)
{
    OS_NetSample in[8], out[8];
    uint8_t buf[512];
    int n = 8, enc, dec, i, ok = 1;
    for (i = 0; i < n; i++) { in[i].var_id = 100 + i; in[i].ts_us = 2000000LL + i * 1000; in[i].value = 1.5 + i; }
    enc = os_net_codec_encode_flat(in, n, buf, sizeof(buf));
    CHECK(enc > 0, "flat 编码");
    dec = os_net_codec_decode_flat(buf, enc, out, 8);
    CHECK(dec == n, "flat 解码数量");
    for (i = 0; i < n; i++)
        if (out[i].var_id != in[i].var_id || out[i].ts_us != in[i].ts_us || out[i].value != in[i].value) { ok = 0; break; }
    CHECK(ok, "flat 无损往返（含 var_id）");
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    test_varint();
    test_frame();
    test_codec();
    test_chunk();
    test_spool();
    test_parallel();
    test_flat();
    if (g_fail == 0) { printf("ALL PASS\n"); return 0; }
    printf("FAILED: %d\n", g_fail);
    return 1;
}
