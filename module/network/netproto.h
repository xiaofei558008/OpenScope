#ifndef OS_NETPROTO_H
#define OS_NETPROTO_H
/* 网络模块传输无关内核：varint / 消息帧 / 分块。纯 C、无 socket 依赖、可单测。 */
#include <stdint.h>

#define OS_NET_MSG_HELLO        1
#define OS_NET_MSG_ELF_SYNC     2
#define OS_NET_MSG_WATCH_LIST   3
#define OS_NET_MSG_SAMPLE_BATCH 4
#define OS_NET_MSG_WRITE_VAR    5
#define OS_NET_MSG_ACK          6
#define OS_NET_MSG_CHUNK        7
#define OS_NET_MSG_BYE          8
#define OS_NET_MSG_ELF_REQ      9 /* 请求对端发回它的 ELF 变量表 */

#define OS_NET_MAGIC 0x314E534Fu /* "OSN1" little-endian */

#define OS_NET_FRAME_HDR 14 /* 4(magic)+1(type)+1(flags)+4(seq)+4(len) */

typedef struct OS_NetFrame {
    uint8_t  type;
    uint8_t  flags;
    uint32_t seq;
    uint32_t len;
    const uint8_t* payload; /* 指向输入缓冲内部，不拷贝 */
} OS_NetFrame;

/* LEB128 无符号 varint：返回写入字节数，cap 不足返回 -1 */
int os_net_put_uvarint(uint8_t* out, int cap, uint64_t v);
/* 读取 varint：成功返回 0，*consumed=消耗字节；数据不足返回 -1 */
int os_net_get_uvarint(const uint8_t* in, int len, uint64_t* v, int* consumed);

/* 帧编码：返回总字节数（含帧头+payload）；cap 不足返回 -1 */
int os_net_frame_encode(uint8_t* out, int cap, uint8_t type, uint8_t flags,
                        uint32_t seq, const uint8_t* payload, uint32_t len);
/* 帧解码：完整帧返回帧头+payload 总长；不足一帧返回 0；magic 错误返回 -1 */
int os_net_frame_decode(const uint8_t* in, int len, OS_NetFrame* f);

/* 分块：把 data 按 chunk_size 切块，逐块回调 cb（idx 从 0 起）。返回块数，len==0 返回 0。 */
int os_net_chunk_split(const uint8_t* data, uint32_t len, uint32_t chunk_size, void* ud,
                       int (*cb)(void* ud, uint32_t idx, uint32_t total, const uint8_t* d, uint32_t l));

/* 可增长字节缓冲（重组/异步缓冲用） */
typedef struct OS_NetAccum {
    uint8_t* buf;
    uint32_t len;
    uint32_t cap;
} OS_NetAccum;
void os_net_accum_init(OS_NetAccum* a);
void os_net_accum_free(OS_NetAccum* a);
int  os_net_accum_append(OS_NetAccum* a, const uint8_t* d, uint32_t l);

#endif
