#ifndef OS_NETCODEC_H
#define OS_NETCODEC_H
/* 时序样本无损编解码：时间戳 delta+varint，浮点值 XOR-delta（Gorilla 式）。纯 C、可单测。 */
#include <stdint.h>

typedef struct OS_NetSample {
    int64_t ts_us;
    double  value;
} OS_NetSample;

/* 编码 n 个样本到 out（cap 字节）。返回写入字节数；不足返回 -1。 */
int os_net_codec_encode(const OS_NetSample* s, int n, uint8_t* out, int cap);
/* 解码到 s（最多 max 个）。返回样本数；数据损坏/不足返回 -1。 */
int os_net_codec_decode(const uint8_t* in, int len, OS_NetSample* s, int max);

/* 多核并行压缩（需求 14）：把 n 个样本按 nthreads 切连续块，每块一线程编码，
 * 输出 [nthreads][n][块长1][块1]...[块长k][块k]。nthreads<=1 时退化为串行。 */
int os_net_codec_encode_parallel(const OS_NetSample* s, int n, int nthreads, uint8_t* out, int cap);
int os_net_codec_decode_parallel(const uint8_t* in, int len, OS_NetSample* s, int max);

#endif
