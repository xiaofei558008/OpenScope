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

#endif
