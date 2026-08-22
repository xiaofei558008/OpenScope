#ifndef OS_NETBUF_H
#define OS_NETBUF_H
/* 异步批量传输的本地缓冲：RAM 累积，超出容量自动转盘（匿名临时文件），读回时 RAM+盘拼接。
 * 对应需求 14 的"先 log 到本地 RAM/硬盘，远端停止后再传输"。纯 C、可单测。 */
#include <stdint.h>
#include <stdio.h>

typedef struct OS_NetSpool {
    uint8_t* ram;      /* RAM 缓冲 */
    uint32_t len;      /* 已用 RAM 字节 */
    uint32_t cap;      /* RAM 容量（字节） */
    FILE*    f;        /* 溢出文件（tmpfile，匿名） */
    int      spilled;  /* 是否已转盘 */
} OS_NetSpool;

void os_net_spool_init(OS_NetSpool* s, uint32_t ram_cap);
void os_net_spool_free(OS_NetSpool* s);
/* 追加 l 字节：RAM 未满进 RAM，满了先转盘再写文件。返回 0 成功，-1 失败。 */
int  os_net_spool_append(OS_NetSpool* s, const uint8_t* d, uint32_t l);
/* 读回全部字节到 out（cap 上限），*out_len=实际长度。返回 0 成功。 */
int  os_net_spool_read(OS_NetSpool* s, uint8_t* out, uint32_t cap, uint32_t* out_len);

#endif
