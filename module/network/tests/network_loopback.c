/* 集成测试：内存回环模拟"本地采集 → 编码 → 分块 → 传输 → 重组 → 解码 → 远端"全链路。
 * 不依赖真实 socket，验证 protocol→codec→chunk→reassemble→decode 端到端无损。 */
#include <stdio.h>
#include <string.h>
#include "netproto.h"
#include "netcodec.h"

static int chunk_cb(void* ud, uint32_t idx, uint32_t total, const uint8_t* d, uint32_t l)
{
    OS_NetAccum* a = (OS_NetAccum*)ud;
    (void)idx; (void)total;
    return os_net_accum_append(a, d, l) == 0 ? 0 : -1;
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    enum { N = 1000 };
    OS_NetSample src[N], dst[N];
    uint8_t payload[32768], frame[32768 + 64];
    OS_NetAccum acc;
    OS_NetFrame f;
    int i, enc, flen, dec, fail = 0;

    /* 本地端：生成 1000 个样本（模拟 4kHz 采集） */
    for (i = 0; i < N; i++) { src[i].ts_us = 2000000LL + i * 250; src[i].value = i * 0.25 + (i % 7) * 0.1; }

    /* 编码 → 帧封装 */
    enc = os_net_codec_encode(src, N, payload, sizeof(payload));
    if (enc <= 0) { printf("FAIL encode\n"); return 1; }
    flen = os_net_frame_encode(frame, sizeof(frame), OS_NET_MSG_SAMPLE_BATCH, 0, 1, payload, (uint32_t)enc);
    if (flen <= 0) { printf("FAIL frame\n"); return 1; }

    /* 分块传输（模拟网络）→ 重组 */
    {
        int chunks;
        os_net_accum_init(&acc);
        chunks = os_net_chunk_split(frame, (uint32_t)flen, 1024, &acc, chunk_cb);
        if (acc.len != (uint32_t)flen) { printf("FAIL reassemble len %u != %d\n", acc.len, flen); return 1; }
        /* 远端：帧解码 → 样本解码 */
        if (os_net_frame_decode(acc.buf, (int)acc.len, &f) != flen) { printf("FAIL frame decode\n"); return 1; }
        if (f.type != OS_NET_MSG_SAMPLE_BATCH) { printf("FAIL type\n"); return 1; }
        dec = os_net_codec_decode(f.payload, (int)f.len, dst, N);
        if (dec != N) { printf("FAIL decode count %d\n", dec); return 1; }
        for (i = 0; i < N; i++)
            if (dst[i].ts_us != src[i].ts_us || dst[i].value != src[i].value) { fail++; break; }
        printf("loopback: %d 样本 %d 字节 -> 帧 %d 字节 -> 分块 %d 块 -> 重组解码 %d 样本%s\n",
               N, enc, flen, chunks, dec, fail ? " FAIL" : " OK");
        os_net_accum_free(&acc);
    }
    printf(fail ? "FAIL\n" : "PASS\n");
    return fail ? 1 : 0;
}
