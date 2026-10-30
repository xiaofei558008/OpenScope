#ifndef XCP_SESSION_H
#define XCP_SESSION_H
/* XCP 主机协议核心（A 块）：每设备一个 XcpSession，无全局单例。
 *
 *  - 每会话一个 RX 线程，按首字节 PID 分流：
 *      0xFF RES / 0xFE ERR -> 命令引擎
 *      0xFD EV             -> on_event（EV_CMD_PENDING 先由引擎消费以重启超时）
 *      0xFC SERV           -> on_serv
 *      0x00..0xFB          -> on_daq（DAQ DTO，交给 B 块解码）
 *  - 命令引擎：单命令在途（标准通信模型 + 从机块模式），超时 t1..t7，
 *    超时 -> 预动作 SYNCH(+可选 SET_MTA 等) -> 重试 2 次（§7.6 Table 217..221），
 *    ERR_CMD_BUSY / ERR_PGM_ACTIVE -> 等 t7 重试，ERR_RESOURCE_TEMPORARY_NOT_ACCESSIBLE -> 重试，
 *    EV_CMD_PENDING -> 重启超时计时。
 *  - 多字节参数按 CONNECT 返回的 BYTE_ORDER 编解码，长度按 ADDRESS_GRANULARITY。
 *
 * 线程：所有 xcp_* 命令函数线程安全（内部串行化）。回调在 RX 线程里执行，
 *       回调中禁止调用命令函数（会返回 XCP_E_WOULDBLOCK），需转投到其他线程。 */
#include <stdint.h>
#include "xcp_tl.h"

/* ---- 返回码：0 成功；负数为主机侧错误；XCP_E_SLAVE(code) 为从机 ERR ---- */
#define XCP_OK              0
#define XCP_E_TIMEOUT      (-1)
#define XCP_E_LINK         (-2)   /* 传输断开/出错，会话需重建 */
#define XCP_E_PROTO        (-3)   /* 应答格式不对 */
#define XCP_E_ARG          (-4)
#define XCP_E_NOTCONN      (-5)
#define XCP_E_CLOSED       (-6)   /* 会话正在销毁 */
#define XCP_E_WOULDBLOCK   (-7)   /* 在 RX 线程（回调）里调用了命令 */
#define XCP_E_NOMEM        (-8)
#define XCP_E_UNSUPPORTED  (-9)   /* 未提供回调（Seed&Key / USER_DEFINED 校验）等 */
#define XCP_E_MISMATCH     (-10)  /* 校验和与本地计算不一致 */
#define XCP_E_SEEDKEY      (-11)  /* Seed&Key 回调计算失败 */
#define XCP_E_SLAVE_BASE    0x100
#define XCP_E_SLAVE(code)  (XCP_E_SLAVE_BASE | (int)(code))
#define XCP_IS_SLAVE_ERR(r) ((r) >= XCP_E_SLAVE_BASE && (r) <= XCP_E_SLAVE_BASE + 0xFF)
#define XCP_SLAVE_CODE(r)  ((r) & 0xFF)

/* 事件码（Table 22） */
#define XCP_EV_RESUME_MODE        0x00u
#define XCP_EV_CLEAR_DAQ          0x01u
#define XCP_EV_STORE_DAQ          0x02u
#define XCP_EV_STORE_CAL          0x03u
#define XCP_EV_CMD_PENDING        0x05u
#define XCP_EV_DAQ_OVERLOAD       0x06u
#define XCP_EV_SESSION_TERMINATED 0x07u
#define XCP_EV_TIME_SYNC          0x08u
#define XCP_EV_STIM_TIMEOUT       0x09u
#define XCP_EV_SLEEP              0x0Au
#define XCP_EV_WAKE_UP            0x0Bu
#define XCP_EV_ECU_STATE_CHANGE   0x0Cu
#define XCP_EV_USER               0xFEu
#define XCP_EV_TRANSPORT          0xFFu

/* 超时下标：tmo[1]..tmo[7] 对应 t1..t7（毫秒），tmo[0] 未用 */
typedef struct XcpTimeouts { int t[8]; } XcpTimeouts;

typedef void (*XcpPacketCb)(void* user, const uint8_t* pkt, int len);
typedef void (*XcpLinkCb)(void* user, int reason);

typedef struct XcpSessionCfg {
    XcpTimeouts tmo;           /* 0 表示用默认：t1=2000 t2=5000 t3..t5=5000 t6=2000 t7=200 */
    int busy_retries;          /* BUSY/PGM_ACTIVE 最大重试（规范为∞，这里设上限），默认 50 */
    int temp_na_retries;       /* RES_TEMP_NOT_ACCESSIBLE 重试上限，默认 3 */
    int connect_retries;       /* CONNECT 超时重试上限（规范∞），默认 2 */
    XcpPacketCb on_event;      /* EV 包（含 PID） */
    XcpPacketCb on_serv;       /* SERV 包 */
    XcpPacketCb on_daq;        /* DAQ DTO 包 */
    XcpLinkCb   on_link_lost;  /* 传输断开（RX 线程退出前调用一次） */
    void* user;
} XcpSessionCfg;

typedef struct XcpConnInfo {
    uint8_t  resource;         /* CAL/PAG 0x01, DAQ 0x04, STIM 0x08, PGM 0x10 */
    uint8_t  comm_mode_basic;
    uint8_t  byte_order;       /* 0 Intel, 1 Motorola */
    uint8_t  ag;               /* 地址粒度字节数：1/2/4 */
    uint8_t  slave_block_mode;
    uint8_t  optional;         /* 支持 GET_COMM_MODE_INFO */
    uint16_t max_cto;          /* 原值，不截断 */
    uint16_t max_dto;
    uint8_t  protocol_ver;
    uint8_t  transport_ver;
} XcpConnInfo;

typedef struct XcpStatus {
    uint8_t  session_status;   /* STORE_CAL_REQ 0x01 ... DAQ_RUNNING 0x40 RESUME 0x80 */
    uint8_t  protection;       /* 资源保护位 */
    uint8_t  state_number;     /* V1.3 ECU_STATES，旧从机为 0 */
    uint16_t session_cfg_id;
} XcpStatus;

typedef struct XcpCommModeInfo {
    uint8_t optional;          /* bit0 MASTER_BLOCK_MODE, bit1 INTERLEAVED_MODE */
    uint8_t max_bs;
    uint8_t min_st;            /* 100 µs 单位 */
    uint8_t queue_size;
    uint8_t driver_version;
} XcpCommModeInfo;

typedef struct XcpId {
    uint8_t  mode;             /* bit0 TRANSFER_MODE, bit1 COMPRESSED_ENCRYPTED */
    uint32_t len;              /* 字节 */
    uint8_t* data;             /* malloc，xcp_id_free 释放；len==0 时为 NULL */
} XcpId;

typedef struct XcpSessionStats {
    long rx_res, rx_err, rx_ev, rx_serv, rx_daq;
    long rx_stray;             /* 没有在途命令时收到的 RES/ERR（迟到应答） */
    long timeouts, synchs, retries, busy_waits, cmd_pending;
    long tl_ctr_gaps, tl_bad_frames, tl_tx, tl_rx;
} XcpSessionStats;

typedef struct XcpSession XcpSession;

/* 每次命令的可选参数 */
typedef int (*XcpPreAction)(XcpSession* s, void* user);   /* 在 SYNCH 之后、重试之前调用（锁内） */
typedef struct XcpCmdOpts {
    int tmo_index;             /* 0 = 按命令码查表 */
    int frames;                /* 期望 RES 帧数（从机块模式 UPLOAD），0/1 = 单帧 */
    int strip;                 /* 多帧时每帧去掉的前缀字节数（PID+对齐填充） */
    XcpPreAction pre;          /* 超时后的额外预动作（如 SET_MTA） */
    void* pre_user;
} XcpCmdOpts;

/* 创建：接管 tl 所有权，启动 RX 线程。cfg 可为 NULL。 */
int  xcp_session_create(XcpSession** out, XcpTl* tl, const XcpSessionCfg* cfg);
/* 销毁：关闭传输、唤醒在途命令（返回 XCP_E_CLOSED）、join RX 线程、释放。
 * 不可在回调里调用。 */
void xcp_session_destroy(XcpSession* s);

int  xcp_session_connected(XcpSession* s);
int  xcp_session_link_ok(XcpSession* s);
void xcp_session_info(XcpSession* s, XcpConnInfo* out);
void xcp_session_stats(XcpSession* s, XcpSessionStats* out);
int  xcp_session_last_slave_err(XcpSession* s);

/* 通用命令（带完整错误处理）。rsp_len 可为 NULL。 */
int xcp_command(XcpSession* s, const uint8_t* cmd, int len,
                uint8_t* rsp, int cap, int* rsp_len, const XcpCmdOpts* opts);

/* 标准命令 */
int xcp_connect(XcpSession* s, uint8_t mode, XcpConnInfo* out);
int xcp_disconnect(XcpSession* s);
int xcp_synch(XcpSession* s);
int xcp_get_status(XcpSession* s, XcpStatus* out);
int xcp_get_comm_mode_info(XcpSession* s, XcpCommModeInfo* out);
int xcp_get_id(XcpSession* s, uint8_t type, XcpId* out);
void xcp_id_free(XcpId* id);

/* 内存读取（n 为 AG 元素个数；dst 需 n*AG 字节） */
int xcp_set_mta(XcpSession* s, uint8_t ext, uint32_t addr);
int xcp_upload(XcpSession* s, uint8_t ext, uint32_t addr, uint32_t n, void* dst);   /* SET_MTA + UPLOAD（块模式时多帧） */
int xcp_upload_at_mta(XcpSession* s, uint32_t n, void* dst);                      /* 从当前 MTA，无 SET_MTA 预动作 */
int xcp_short_upload(XcpSession* s, uint8_t ext, uint32_t addr, uint8_t n, void* dst);

/* ---- 第 2 轮：标定写入 / 块模式 / 位操作 / 校验和 / Seed&Key ---- */

/* 缓存的 GET_COMM_MODE_INFO（CONNECT 时若 OPTIONAL=1 自动读取）；无效返回 0 */
int xcp_session_comm_mode(XcpSession* s, XcpCommModeInfo* out);
/* 最近一次从机 ERR 包（含 PID，最多 8 字节）；返回长度 */
int xcp_session_last_err_packet(XcpSession* s, uint8_t* out8);

/* SET_MTA + DOWNLOAD。n 为 AG 元素个数。
 * 从机支持主机块模式（GET_COMM_MODE_INFO.MASTER_BLOCK_MODE 且 MAX_BS>=2）时
 * 用 DOWNLOAD + DOWNLOAD_NEXT*，每块 <= min(MAX_BS*每帧元素, 255)，帧间隔 >= MIN_ST；
 * 否则每帧一个 DOWNLOAD。超时：SYNCH + SET_MTA + 重发整块（2 次）；
 * DOWNLOAD_NEXT 的 ERR_SEQUENCE：SET_MTA + 重发整块（2 次）。 */
int xcp_download(XcpSession* s, uint8_t ext, uint32_t addr, uint32_t n, const void* src);
/* 强制不用块模式（用于对比/兼容） */
int xcp_download_noblock(XcpSession* s, uint8_t ext, uint32_t addr, uint32_t n, const void* src);
/* SET_MTA + DOWNLOAD_MAX：固定写 xcp_download_max_elems() 个元素 */
int xcp_download_max_elems(XcpSession* s);
int xcp_download_max(XcpSession* s, uint8_t ext, uint32_t addr, const void* src);
/* SHORT_DOWNLOAD：n <= (MAX_CTO-8)/AG */
int xcp_short_download(XcpSession* s, uint8_t ext, uint32_t addr, uint8_t n, const void* src);
/* SET_MTA + MODIFY_BITS：A = (A & ~((~MA)<<S)) ^ (MX<<S)，A 为 MTA 处 32 位 */
int xcp_modify_bits(XcpSession* s, uint8_t ext, uint32_t addr, uint8_t shift, uint16_t and_mask, uint16_t xor_mask);

/* SET_MTA + BUILD_CHECKSUM(n 元素)，超时 t2。ERR_OUT_OF_RANGE 时 *max_block 返回从机允许的最大块（元素），否则 0。 */
int xcp_build_checksum(XcpSession* s, uint8_t ext, uint32_t addr, uint32_t n,
                       uint8_t* type, uint32_t* sum, uint32_t* max_block);
/* USER_DEFINED 校验的外部计算（对应 A2L 指定的 checksum DLL；本轮只给回调） */
typedef int (*XcpUserChecksumFn)(void* user, const uint8_t* data, uint32_t len, uint32_t* out);
typedef struct XcpChecksumResult {
    uint8_t  type;
    uint32_t blocks;        /* 实际分成的块数 */
    uint32_t block_elems;   /* 使用的块大小（元素） */
    uint32_t bad_offset;    /* 不一致时：块起始元素偏移 */
    uint32_t slave_sum, local_sum;
} XcpChecksumResult;
/* 用 BUILD_CHECKSUM 验证从机内存 == data（n 元素）。整段被拒（OUT_OF_RANGE+最大块）时按最大块切分重试。
 * 返回 0 一致；XCP_E_MISMATCH 不一致；USER_DEFINED 且无 ufn 返回 XCP_E_UNSUPPORTED。 */
int xcp_checksum_verify(XcpSession* s, uint8_t ext, uint32_t addr, const void* data, uint32_t n,
                        XcpUserChecksumFn ufn, void* uuser, XcpChecksumResult* res);

/* Seed&Key（只有回调接口；外部 DLL 加载器待 Tom 确认）。
 * 回调：由 seed 计算 key；*key_len 进入时为容量（255），返回实际长度；返回 0 成功。 */
typedef int (*XcpSeedKeyFn)(void* user, uint8_t resource, const uint8_t* seed, int seed_len,
                            uint8_t* key, int* key_len);
void xcp_set_seed_key(XcpSession* s, XcpSeedKeyFn fn, void* user);
/* GET_SEED(0/1 多段) + UNLOCK(多段) 解锁单个资源（XCP_RES_*）。资源未保护时直接返回 0。
 * 设置了回调后，命令遇到 ERR_ACCESS_LOCKED 会自动解锁对应资源并重试（最多 2 次）。 */
int xcp_unlock(XcpSession* s, uint8_t resource);
/* 按 GET_STATUS 的保护位逐个解锁；*prot_out 返回最终保护状态 */
int xcp_unlock_all(XcpSession* s, uint8_t* prot_out);

/* 按会话字节序编解码 */
uint16_t xcp_s_rd16(const XcpSession* s, const uint8_t* p);
uint32_t xcp_s_rd32(const XcpSession* s, const uint8_t* p);
void     xcp_s_wr16(const XcpSession* s, uint8_t* p, uint16_t v);
void     xcp_s_wr32(const XcpSession* s, uint8_t* p, uint32_t v);

const char* xcp_strerror(int rc);

#endif
