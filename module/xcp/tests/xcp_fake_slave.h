#ifndef XCP_FAKE_SLAVE_H
#define XCP_FAKE_SLAVE_H
/* 测试用假 XCP 从机（XCP on Ethernet TCP/UDP，回环口，单客户端）。
 * 支持字节序/AG/块模式/MAX_CTO/MAX_DTO 配置与故障注入。 */
#include <stdint.h>

typedef struct FakeCfg {
    int udp;
    int byte_order;     /* 0 Intel 1 Motorola */
    int ag;             /* 1/2/4 */
    int block;          /* 从机块模式 */
    int max_cto;        /* <=255 */
    int max_dto;
    uint8_t resource;
    uint8_t session_status, protection, state_number;
    uint16_t session_cfg_id;
    /* 第 2 轮 */
    int no_master_block;  /* GET_COMM_MODE_INFO 不声明主机块模式 */
    int max_bs;           /* 0 -> 16 */
    int min_st;           /* 100us 单位；0 -> 2；-1 -> 0 */
    uint8_t cks_type;     /* 0 -> ADD_14 */
    uint32_t cks_max_block; /* BUILD_CHECKSUM 最大块（元素），0 = 不限 */
    int seed_len;         /* 0 -> 4 */
    int key_extra;        /* key 长度 = seed_len + key_extra */
} FakeCfg;

typedef struct FakeSlave FakeSlave;

FakeSlave* fake_start(const FakeCfg* cfg, int* port_out);
void fake_stop(FakeSlave* f);
void fake_close_client(FakeSlave* f);             /* 模拟断链 */

/* 故障注入（对下 n 个该命令码的请求生效） */
void fake_drop(FakeSlave* f, uint8_t cmd, int n);          /* 不应答 */
void fake_busy(FakeSlave* f, uint8_t cmd, int n);          /* 回 ERR_CMD_BUSY */
void fake_err(FakeSlave* f, uint8_t cmd, uint8_t code, int n);
void fake_delay(FakeSlave* f, uint8_t cmd, int ms, int n); /* 延迟 ms 后再应答（可制造迟到应答） */
void fake_pending(FakeSlave* f, uint8_t cmd, int ms, int n); /* 先发 EV_CMD_PENDING，ms 后应答 */
void fake_set_id(FakeSlave* f, const uint8_t* data, int len); /* GET_ID type 4 内容 */
void fake_set_mem(FakeSlave* f, uint32_t off, const uint8_t* data, int len);

/* 从测试线程主动发送 EV/SERV/DAQ 等任意包 */
int  fake_inject(FakeSlave* f, const uint8_t* pkt, int len);
int  fake_count(FakeSlave* f, uint8_t cmd);  /* 收到的该命令次数 */

/* 第 2 轮 */
void fake_get_mem(FakeSlave* f, uint32_t off, uint8_t* out, int len);
void fake_lose_next(FakeSlave* f, int k);     /* 当作丢失：忽略下一个块里的第 k 个 DOWNLOAD_NEXT（1 起） */
int  fake_min_gap_us(FakeSlave* f);            /* 同一块内 DOWNLOAD_NEXT 最小到达间隔（微秒），无则 -1 */
int  fake_max_block_frames(FakeSlave* f);      /* 观测到的最大块帧数（DOWNLOAD + NEXT） */
uint8_t fake_protection(FakeSlave* f);
/* 假从机 Seed&Key 算法：key[i] = seed[(L-1-i) % L] ^ resource ^ i，长度 L+key_extra */
int  fake_compute_key(uint8_t resource, const uint8_t* seed, int seed_len, int key_extra, uint8_t* key);
/* 假从机 USER_DEFINED 校验：ADD_14 ^ 0xA5A5A5A5 */
uint32_t fake_user_checksum(const uint8_t* d, uint32_t len);

#endif
