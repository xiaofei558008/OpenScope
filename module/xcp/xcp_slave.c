#include "xcp_slave.h"

#include <stdlib.h>
#include <string.h>

typedef struct XcpOdtEntry {
    uint32_t addr;
    uint8_t  size;
    uint8_t  used;
} XcpOdtEntry;

typedef struct XcpOdt {
    XcpOdtEntry ent[XCP_MAX_ODT_ENTRY];
    int         nent;
    int         alloc;
} XcpOdt;

typedef struct XcpDaq {
    XcpOdt   odt[XCP_MAX_ODT];
    int      nodt;
    int      alloc;
    uint8_t  mode;
    uint16_t event;
    uint8_t  prescaler;
    uint8_t  priority;
    uint8_t  prescale_cnt;
    uint8_t  running;
    uint8_t  selected;
} XcpDaq;

struct XcpSlave {
    uint8_t* ram;
    uint8_t* refpage;
    uint32_t base;
    uint32_t size;
    int      connected;
    uint8_t  protect;
    uint8_t  session;
    uint16_t session_id;
    uint8_t  seg_mode;
    uint8_t  ecu_page;
    uint8_t  xcp_page;
    uint32_t mta;
    int      mta_id;          /* 1 = MTA 指向标识字符串 */
    int      dl_remain;       /* DOWNLOAD 块传输剩余字节，0 = 不在块里 */
    uint8_t  seed_res;
    /* Seed&Key（可插拔） */
    uint8_t  seed[XCP_SLAVE_MAX_SEED];
    int      seed_len;
    int      seed_sent;
    uint8_t  key_buf[XCP_SLAVE_MAX_SEED];
    int      key_len;          /* 已收字节 */
    int      key_total;        /* 期望总长，0 = 未开始 */
    XcpSlaveSeedFn seed_fn;
    XcpSlaveKeyFn  key_fn;
    void*    sk_user;
    uint32_t seed_ctr;
    char     id[48];
    XcpDaq   daq[XCP_MAX_DAQ];
    int      n_daq;
    int      alloc_step;      /* 0 空闲，1 FREE 之后，2 ALLOC_DAQ 之后 */
    uint16_t ptr_daq;
    uint8_t  ptr_odt;
    uint8_t  ptr_entry;
    int      ptr_valid;
    uint8_t  dto_q[XCP_SLAVE_DTO_Q][XCP_MAX_DTO];
    int      dto_len[XCP_SLAVE_DTO_Q];
    uint32_t dto_overflow;
    int      dto_h, dto_t, dto_n;
    uint32_t clock;
    int      msb;             /* 1 = Motorola 字节序 */
    int      ag;              /* 地址粒度 1/2/4 字节 */
    uint8_t  rq[XCP_SLAVE_RSP_Q][XCP_MAX_CTO];  /* 从机块模式 UPLOAD 的后续 RES 帧 */
    int      rql[XCP_SLAVE_RSP_Q];
    int      rqh, rqn;
};

static uint16_t R16(const XcpSlave* s, const uint8_t* p)
{ return s->msb ? (uint16_t)((p[0] << 8) | p[1]) : xcp_rd16(p); }
static uint32_t R32(const XcpSlave* s, const uint8_t* p)
{ return s->msb ? ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3] : xcp_rd32(p); }
static void W16(const XcpSlave* s, uint8_t* p, uint16_t v)
{ if (s->msb) { p[0] = (uint8_t)(v >> 8); p[1] = (uint8_t)v; } else xcp_wr16(p, v); }
static void W32(const XcpSlave* s, uint8_t* p, uint32_t v)
{ if (s->msb) { p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16); p[2] = (uint8_t)(v >> 8); p[3] = (uint8_t)v; } else xcp_wr32(p, v); }


static int put_err(uint8_t* rsp, uint8_t code)
{
    rsp[0] = XCP_PID_ERR;
    rsp[1] = code;
    return 2;
}

static uint8_t* work_ptr(XcpSlave* s, uint32_t addr, uint32_t len)
{
    if (!s->ram || len == 0) return NULL;
    if (addr < s->base) return NULL;
    if (addr - s->base > s->size) return NULL;
    if (len > s->size - (addr - s->base)) return NULL;
    return s->ram + (addr - s->base);
}

static void default_seed(void* user, uint8_t resource, uint32_t ctr, uint8_t* seed, int* seed_len);
static int default_key(void* user, uint8_t resource, const uint8_t* seed, int seed_len, uint8_t* key, int key_cap);

XcpSlave* xcp_slave_create(uint32_t base, uint32_t size)
{
    XcpSlave* s;
    if (size == 0 || size > 16u * 1024u * 1024u) return NULL;
    s = (XcpSlave*)calloc(1, sizeof(XcpSlave));
    if (!s) return NULL;
    s->ram = (uint8_t*)calloc(1, size);
    s->refpage = (uint8_t*)calloc(1, size);
    if (!s->ram || !s->refpage) {
        free(s->ram);
        free(s->refpage);
        free(s);
        return NULL;
    }
    s->base = base;
    s->size = size;
    memcpy(s->id, XCP_ID_TEXT, sizeof(XCP_ID_TEXT));
    s->seed_fn = default_seed;
    s->key_fn = default_key;
    s->ag = 1;
    return s;
}

void xcp_slave_destroy(XcpSlave* s)
{
    if (!s) return;
    free(s->ram);
    free(s->refpage);
    free(s);
}

/* 主机访问：按 XCP 当前页选择工作页(RAM, page0)或参考页(FLASH, page1)。 */
static uint8_t* page_ptr(XcpSlave* s, uint32_t addr, uint32_t len)
{
    uint8_t* p = work_ptr(s, addr, len);
    if (!p) return NULL;
    if (s->xcp_page == 1) return s->refpage + (p - s->ram);
    return p;
}

uint8_t* xcp_slave_ref_ptr(XcpSlave* s, uint32_t addr, uint32_t len)
{
    uint8_t* p;
    if (!s) return NULL;
    p = work_ptr(s, addr, len);
    return p ? s->refpage + (p - s->ram) : NULL;
}

static void default_seed(void* user, uint8_t resource, uint32_t ctr, uint8_t* seed, int* seed_len)
{
    static const uint8_t def[4] = { 0x11, 0x22, 0x33, 0x44 };
    (void)user; (void)resource; (void)ctr;
    memcpy(seed, def, 4);
    *seed_len = 4;
}

/* 兼容旧行为：key = seed XOR 0xFF（xcp_master_unlock_cal 依赖） */
static int default_key(void* user, uint8_t resource, const uint8_t* seed, int seed_len, uint8_t* key, int key_cap)
{
    int i;
    (void)user; (void)resource;
    if (seed_len > key_cap) return -1;
    for (i = 0; i < seed_len; i++) key[i] = (uint8_t)(seed[i] ^ 0xFF);
    return seed_len;
}

void xcp_slave_set_seed_key(XcpSlave* s, XcpSlaveSeedFn seed_fn, XcpSlaveKeyFn key_fn, void* user)
{
    if (!s) return;
    s->seed_fn = seed_fn ? seed_fn : default_seed;
    s->key_fn = key_fn ? key_fn : default_key;
    s->sk_user = user;
}

uint8_t xcp_slave_protection(const XcpSlave* s) { return s ? s->protect : 0; }
int xcp_slave_is_connected(const XcpSlave* s) { return s ? s->connected : 0; }
uint8_t xcp_slave_ecu_page(const XcpSlave* s) { return s ? s->ecu_page : 0; }
uint8_t xcp_slave_xcp_page(const XcpSlave* s) { return s ? s->xcp_page : 0; }
uint32_t xcp_slave_dto_overflow(const XcpSlave* s) { return s ? s->dto_overflow : 0; }

uint8_t* xcp_slave_ptr(XcpSlave* s, uint32_t addr, uint32_t len)
{
    if (!s) return NULL;
    return work_ptr(s, addr, len);
}

int xcp_slave_set_format(XcpSlave* s, int msb_first, int ag)
{
    if (!s || s->connected || (ag != 1 && ag != 2 && ag != 4)) return -1;
    s->msb = msb_first ? 1 : 0;
    s->ag = ag;
    return 0;
}

int xcp_slave_byte_order(const XcpSlave* s) { return s ? s->msb : 0; }
int xcp_slave_ag(const XcpSlave* s) { return s ? s->ag : 1; }

int xcp_slave_pop_rsp(XcpSlave* s, uint8_t* dst, int cap)
{
    int n;
    if (!s || s->rqn <= 0) return 0;
    n = s->rql[s->rqh];
    if (n > cap) return -1;
    memcpy(dst, s->rq[s->rqh], (size_t)n);
    s->rqh = (s->rqh + 1) % XCP_SLAVE_RSP_Q;
    s->rqn--;
    return n;
}

int xcp_slave_dto_pending(const XcpSlave* s) { return s ? s->dto_n : 0; }

void xcp_slave_lock(XcpSlave* s, uint8_t resource)
{
    if (s) s->protect = (uint8_t)(s->protect | resource);
}

static int daq_running(const XcpSlave* s)
{
    int i;
    for (i = 0; i < s->n_daq; i++)
        if (s->daq[i].running) return 1;
    return 0;
}

static void refresh_session(XcpSlave* s)
{
    if (daq_running(s)) s->session = (uint8_t)(s->session | XCP_SS_DAQ_RUNNING);
    else s->session = (uint8_t)(s->session & (uint8_t)~XCP_SS_DAQ_RUNNING);
}

static int push_dto(XcpSlave* s, const uint8_t* p, int n)
{
    if (n <= 0 || n > XCP_MAX_DTO) return -1;
    if (s->dto_n >= XCP_SLAVE_DTO_Q) { s->dto_overflow++; return -1; }
    memcpy(s->dto_q[s->dto_t], p, (size_t)n);
    s->dto_len[s->dto_t] = n;
    s->dto_t = (s->dto_t + 1) % XCP_SLAVE_DTO_Q;
    s->dto_n++;
    return 0;
}

int xcp_slave_pop_dto(XcpSlave* s, uint8_t* dst, int cap)
{
    int n;
    if (!s || s->dto_n <= 0) return 0;
    n = s->dto_len[s->dto_h];
    if (n > cap) return -1;
    memcpy(dst, s->dto_q[s->dto_h], (size_t)n);
    s->dto_h = (s->dto_h + 1) % XCP_SLAVE_DTO_Q;
    s->dto_n--;
    return n;
}

static int read_mem(XcpSlave* s, uint8_t* dst, int n)
{
    if (s->mta_id) {
        int idn = (int)strlen(s->id);
        if (s->mta > (uint32_t)idn || n > idn - (int)s->mta) return -1;
        memcpy(dst, s->id + s->mta, (size_t)n);
        s->mta += (uint32_t)n;
        return 0;
    }
    {
        uint8_t* p = page_ptr(s, s->mta, (uint32_t)n);
        if (!p) return -1;
        memcpy(dst, p, (size_t)n);
        s->mta += (uint32_t)n;
        return 0;
    }
}

static int write_mem(XcpSlave* s, const uint8_t* src, int n)
{
    uint8_t* p;
    if (s->mta_id) return -1;
    if (s->xcp_page != 0) return -2; /* FLASH 参考页只读 */
    p = work_ptr(s, s->mta, (uint32_t)n);
    if (!p) return -1;
    memcpy(p, src, (size_t)n);
    s->mta += (uint32_t)n;
    return 0;
}

static int cmd_connect(XcpSlave* s, uint8_t* rsp)
{
    s->connected = 1;
    s->dl_remain = 0;
    rsp[0] = XCP_PID_RES;
    rsp[1] = XCP_RES_CAL_PAG | XCP_RES_DAQ;
    rsp[2] = (uint8_t)(XCP_CMB | (s->msb ? 0x01 : 0) | ((s->ag == 4 ? 2 : s->ag == 2 ? 1 : 0) << 1));
    rsp[3] = XCP_MAX_CTO;
    W16(s, rsp + 4, XCP_MAX_DTO);
    rsp[6] = XCP_PROTOCOL_VERSION;
    rsp[7] = XCP_TRANSPORT_VERSION;
    return 8;
}

static int cmd_get_status(XcpSlave* s, uint8_t* rsp)
{
    refresh_session(s);
    rsp[0] = XCP_PID_RES;
    rsp[1] = s->session;
    rsp[2] = s->protect;
    rsp[3] = XCP_RES_CAL_PAG | XCP_RES_DAQ;
    rsp[4] = 0;
    W16(s, rsp + 5, s->session_id);
    return 7;
}

static int cmd_get_id(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int n;
    if (len < 2) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (cmd[1] > 1) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    n = (int)strlen(s->id);
    s->mta_id = 1;
    s->mta = 0;
    rsp[0] = XCP_PID_RES;
    rsp[1] = 0; /* 模式 0：随后 UPLOAD */
    rsp[2] = 0;
    rsp[3] = 0;
    W32(s, rsp + 4, (uint32_t)n);
    return 8;
}

static int cmd_set_mta(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    if (len < 8) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (cmd[3] != 0) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    s->mta_id = 0;
    s->mta = R32(s, cmd + 4);
    rsp[0] = XCP_PID_RES;
    return 1;
}

/* UPLOAD n 个 AG 元素（1..255）。超过一帧时走从机块模式：首帧作为应答返回，
 * 余帧进 rq 队列（xcp_slave_pop_rsp 取出，外壳紧随首帧发送）。每帧 [RES][AG-1 填充][数据]。 */
static int cmd_upload(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    uint8_t buf[255 * 4];
    int n, bytes, per, off, first;
    if (len < 2) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    n = cmd[1];
    if (n < 1) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    bytes = n * s->ag;
    per = ((XCP_MAX_CTO - s->ag) / s->ag) * s->ag;
    if (read_mem(s, buf, bytes) != 0) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    first = bytes < per ? bytes : per;
    memset(rsp, 0, (size_t)s->ag);
    rsp[0] = XCP_PID_RES;
    memcpy(rsp + s->ag, buf, (size_t)first);
    for (off = first; off < bytes; off += per) {
        int c = bytes - off < per ? bytes - off : per;
        int t = (s->rqh + s->rqn) % XCP_SLAVE_RSP_Q;
        if (s->rqn >= XCP_SLAVE_RSP_Q) return put_err(rsp, XCP_ERR_MEMORY_OVERFLOW);
        memset(s->rq[t], 0, (size_t)s->ag);
        s->rq[t][0] = XCP_PID_RES;
        memcpy(s->rq[t] + s->ag, buf + off, (size_t)c);
        s->rql[t] = s->ag + c;
        s->rqn++;
    }
    return s->ag + first;
}

static int cmd_short_upload(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int n;
    if (len < 8) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    n = cmd[1] * s->ag;
    if (n < 1 || n > XCP_MAX_CTO - s->ag) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    if (cmd[3] != 0) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    s->mta_id = 0;
    s->mta = R32(s, cmd + 4);
    memset(rsp, 0, (size_t)s->ag);
    rsp[0] = XCP_PID_RES;
    if (read_mem(s, rsp + s->ag, n) != 0) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    return s->ag + n;
}

static int accept_download_bytes(XcpSlave* s, const uint8_t* data, int n, uint8_t* rsp)
{
    if (n > s->dl_remain) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    int wr = write_mem(s, data, n);
    if (wr != 0) {
        s->dl_remain = 0;
        return put_err(rsp, wr == -2 ? XCP_ERR_WRITE_PROTECTED : XCP_ERR_OUT_OF_RANGE);
    }
    s->dl_remain -= n;
    if (s->dl_remain > 0) return 0;
    rsp[0] = XCP_PID_RES;
    return 1;
}

/* DOWNLOAD/DOWNLOAD_NEXT 数据偏移：AG1/2 为 2，AG4 为 4；每帧字节数取 AG 整数倍 */
static int dl_off(const XcpSlave* s) { return s->ag == 4 ? 4 : 2; }
static int dl_per(const XcpSlave* s) { return ((XCP_MAX_CTO - dl_off(s)) / s->ag) * s->ag; }

static int cmd_download(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int n, have;
    if (s->protect & XCP_RES_CAL_PAG) return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    if (len < 2) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (s->dl_remain) return put_err(rsp, XCP_ERR_SEQUENCE);
    n = cmd[1] * s->ag;
    if (n < 1) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    have = len - dl_off(s);
    if (have > n) have = n;
    if (have > dl_per(s)) have = dl_per(s);
    if (have < 1) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    s->dl_remain = n;
    return accept_download_bytes(s, cmd + dl_off(s), have, rsp);
}

static int cmd_download_next(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int n, have;
    if (s->protect & XCP_RES_CAL_PAG) return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    if (!s->dl_remain) return put_err(rsp, XCP_ERR_SEQUENCE);
    if (len < 2) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    n = cmd[1] * s->ag;
    if (n != s->dl_remain) return put_err(rsp, XCP_ERR_SEQUENCE);
    have = len - dl_off(s);
    if (have > s->dl_remain) have = s->dl_remain;
    if (have > dl_per(s)) have = dl_per(s);
    if (have < 1) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    return accept_download_bytes(s, cmd + dl_off(s), have, rsp);
}

static int cmd_download_max(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int n = ((XCP_MAX_CTO - s->ag) / s->ag) * s->ag;
    if (s->protect & XCP_RES_CAL_PAG) return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    if (len < s->ag + n) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    { int wr = write_mem(s, cmd + s->ag, n); if (wr != 0) return put_err(rsp, wr == -2 ? XCP_ERR_WRITE_PROTECTED : XCP_ERR_OUT_OF_RANGE); }
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_short_download(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int n;
    if (s->protect & XCP_RES_CAL_PAG) return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    if (len < 8) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    n = cmd[1] * s->ag;
    if (n < 1 || 8 + n > len || n > XCP_MAX_CTO - 8) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    if (cmd[3] != 0) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    s->mta_id = 0;
    s->mta = R32(s, cmd + 4);
    { int wr = write_mem(s, cmd + 8, n); if (wr != 0) return put_err(rsp, wr == -2 ? XCP_ERR_WRITE_PROTECTED : XCP_ERR_OUT_OF_RANGE); }
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_modify_bits(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    uint8_t* p;
    uint16_t and_m, xor_m;
    int shift, i;
    uint8_t b;
    if (s->protect & XCP_RES_CAL_PAG) return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    if (len < 6) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (s->mta_id) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    if (s->xcp_page != 0) return put_err(rsp, XCP_ERR_WRITE_PROTECTED);
    p = work_ptr(s, s->mta, 1);
    if (!p) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    shift = cmd[1];
    and_m = R16(s, cmd + 2);
    xor_m = R16(s, cmd + 4);
    for (i = 0; i < shift; i++) {
        and_m = (uint16_t)((and_m << 1) | 1u);
        xor_m = (uint16_t)(xor_m << 1);
    }
    b = *p;
    *p = (uint8_t)((b & (uint8_t)and_m) ^ (uint8_t)xor_m);
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_checksum(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    uint32_t n, i, sum = 0;
    uint8_t* p;
    if (len < 8) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    n = R32(s, cmd + 4);
    if (n > 0x01000000u) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    n *= (uint32_t)s->ag;
    if (s->mta_id) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    p = page_ptr(s, s->mta, n);
    if (!p || n == 0) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    for (i = 0; i < n; i++) sum += p[i];
    s->mta += n;
    rsp[0] = XCP_PID_RES;
    rsp[1] = XCP_CHECKSUM_ADD_14;
    rsp[2] = 0;
    rsp[3] = 0;
    W32(s, rsp + 4, sum);
    return 8;
}

static void clear_daq(XcpSlave* s);

/* 结束会话（DISCONNECT 与错误密钥共用）：停止并清空 DAQ、清 DTO/块应答队列、块下载与 Seed&Key 状态 */
static void end_session(XcpSlave* s)
{
    s->connected = 0;
    s->dl_remain = 0;
    s->seed_res = 0;
    s->key_total = 0;
    s->key_len = 0;
    clear_daq(s);
    s->dto_h = s->dto_t = s->dto_n = 0;
    s->rqh = s->rqn = 0;
    refresh_session(s);
}

/* GET_SEED：mode 0 = 新种子首段，mode 1 = 余段（种子长度 > MAX_CTO-2 时）。
 * 应答 [RES][剩余长度][种子片段]。资源未上锁时长度 0。 */
static int cmd_get_seed(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int chunk, remain;
    if (len < 3) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (cmd[1] == 0) {
        uint8_t res = cmd[2];
        if (res != XCP_RES_CAL_PAG && res != XCP_RES_DAQ && res != XCP_RES_STIM && res != XCP_RES_PGM)
            return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
        if ((s->protect & res) == 0) {
            rsp[0] = XCP_PID_RES;
            rsp[1] = 0;
            return 2;
        }
        s->seed_len = 0;
        s->seed_fn(s->sk_user, res, s->seed_ctr++, s->seed, &s->seed_len);
        if (s->seed_len < 1 || s->seed_len > XCP_SLAVE_MAX_SEED) return put_err(rsp, XCP_ERR_GENERIC);
        s->seed_res = res;
        s->seed_sent = 0;
        s->key_len = 0;
        s->key_total = 0;
    } else if (cmd[1] == 1) {
        if (!s->seed_res || s->seed_sent == 0 || s->seed_sent >= s->seed_len)
            return put_err(rsp, XCP_ERR_SEQUENCE);
    } else
        return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    remain = s->seed_len - s->seed_sent;
    chunk = remain > XCP_MAX_CTO - 2 ? XCP_MAX_CTO - 2 : remain;
    rsp[0] = XCP_PID_RES;
    rsp[1] = (uint8_t)remain;
    memcpy(rsp + 2, s->seed + s->seed_sent, (size_t)chunk);
    s->seed_sent += chunk;
    return 2 + chunk;
}

/* UNLOCK [len=剩余 key 字节][key 片段]。首帧的 len 为 key 总长。
 * 密钥错误：按 V1.3 规范回 ERR_ACCESS_LOCKED 并断开会话。 */
static int cmd_unlock(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int remain, chunk, klen;
    uint8_t expect[XCP_SLAVE_MAX_SEED];
    if (len < 2) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (!s->seed_res || s->seed_sent < s->seed_len) return put_err(rsp, XCP_ERR_SEQUENCE);
    remain = cmd[1];
    if (s->key_total == 0) {
        if (remain < 1 || remain > XCP_SLAVE_MAX_SEED) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
        s->key_total = remain;
        s->key_len = 0;
    } else if (remain != s->key_total - s->key_len)
        return put_err(rsp, XCP_ERR_SEQUENCE);
    chunk = remain > XCP_MAX_CTO - 2 ? XCP_MAX_CTO - 2 : remain;
    if (len < 2 + chunk) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    memcpy(s->key_buf + s->key_len, cmd + 2, (size_t)chunk);
    s->key_len += chunk;
    if (s->key_len < s->key_total) {
        rsp[0] = XCP_PID_RES;
        rsp[1] = s->protect;
        return 2;
    }
    klen = s->key_fn(s->sk_user, s->seed_res, s->seed, s->seed_len, expect, (int)sizeof(expect));
    if (klen != s->key_total || memcmp(expect, s->key_buf, (size_t)klen) != 0) {
        end_session(s);
        return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    }
    s->protect = (uint8_t)(s->protect & (uint8_t)~s->seed_res);
    s->seed_res = 0;
    s->key_total = 0;
    rsp[0] = XCP_PID_RES;
    rsp[1] = s->protect;
    return 2;
}

static int cmd_set_cal_page(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    if (len < 4) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (s->protect & XCP_RES_CAL_PAG) return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    if (cmd[2] != 0) return put_err(rsp, XCP_ERR_SEGMENT_NOT_VALID);
    if (cmd[3] > 1) return put_err(rsp, XCP_ERR_PAGE_NOT_VALID);
    if ((cmd[1] & 0x83) == 0) return put_err(rsp, XCP_ERR_MODE_NOT_VALID);
    if (cmd[1] & 0x01) s->ecu_page = cmd[3];
    if (cmd[1] & 0x02) s->xcp_page = cmd[3];
    if (cmd[1] & 0x80) { s->ecu_page = cmd[3]; s->xcp_page = cmd[3]; }
    if ((cmd[1] & 0x83) == 0) return put_err(rsp, XCP_ERR_MODE_NOT_VALID);
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_get_cal_page(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    if (len < 3) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (cmd[2] != 0) return put_err(rsp, XCP_ERR_SEGMENT_NOT_VALID);
    rsp[0] = XCP_PID_RES;
    rsp[1] = 0;
    rsp[2] = 0;
    if (cmd[1] == 1) rsp[3] = s->ecu_page;
    else if (cmd[1] == 2) rsp[3] = s->xcp_page;
    else return put_err(rsp, XCP_ERR_MODE_NOT_VALID);
    return 4;
}

static int cmd_copy_cal_page(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    if (len < 5) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (s->protect & XCP_RES_CAL_PAG) return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    if (cmd[1] != 0 || cmd[3] != 0) return put_err(rsp, XCP_ERR_SEGMENT_NOT_VALID);
    if (cmd[2] > 1 || cmd[4] > 1) return put_err(rsp, XCP_ERR_PAGE_NOT_VALID);
    if (cmd[2] == cmd[4]) { rsp[0] = XCP_PID_RES; return 1; }
    if (cmd[2] == 0 && cmd[4] == 1) memcpy(s->refpage, s->ram, s->size);
    else memcpy(s->ram, s->refpage, s->size);
    rsp[0] = XCP_PID_RES;
    return 1;
}

static void clear_daq(XcpSlave* s)
{
    memset(s->daq, 0, sizeof(s->daq));
    s->n_daq = 0;
    s->alloc_step = 0;
    s->ptr_valid = 0;
}

static int cmd_free_daq(XcpSlave* s, uint8_t* rsp)
{
    if (daq_running(s)) return put_err(rsp, XCP_ERR_DAQ_ACTIVE);
    clear_daq(s);
    s->alloc_step = 1;
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_alloc_daq(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int n;
    if (len < 4) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (s->alloc_step != 1) return put_err(rsp, XCP_ERR_SEQUENCE);
    n = R16(s, cmd + 2);
    if (n < 1 || n > XCP_MAX_DAQ) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    s->n_daq = n;
    s->alloc_step = 2;
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_alloc_odt(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int daq, n;
    if (len < 5) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (s->alloc_step < 2) return put_err(rsp, XCP_ERR_SEQUENCE);
    daq = R16(s, cmd + 2);
    n = cmd[4];
    if (daq < 0 || daq >= s->n_daq || n < 1 || n > XCP_MAX_ODT)
        return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    s->daq[daq].nodt = n;
    s->daq[daq].alloc = 1;
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_alloc_odt_entry(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int daq, odt, n;
    if (len < 6) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    if (s->alloc_step < 2) return put_err(rsp, XCP_ERR_SEQUENCE);
    daq = R16(s, cmd + 2);
    odt = cmd[4];
    n = cmd[5];
    if (daq >= s->n_daq || !s->daq[daq].alloc || odt >= s->daq[daq].nodt)
        return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    if (n < 1 || n > XCP_MAX_ODT_ENTRY) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    s->daq[daq].odt[odt].alloc = n;
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_set_daq_ptr(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int daq, odt, ent;
    if (len < 6) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    daq = R16(s, cmd + 2);
    odt = cmd[4];
    ent = cmd[5];
    if (daq >= s->n_daq || odt >= s->daq[daq].nodt || ent >= s->daq[daq].odt[odt].alloc)
        return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    s->ptr_daq = (uint16_t)daq;
    s->ptr_odt = (uint8_t)odt;
    s->ptr_entry = (uint8_t)ent;
    s->ptr_valid = 1;
    rsp[0] = XCP_PID_RES;
    return 1;
}

/* ODT 条目最大元素数（字节数 ≤ 8，AG1 保持原来的 7） */
static int max_entry(const XcpSlave* s) { return s->ag == 1 ? 7 : 8 / s->ag; }

static int write_one_entry(XcpSlave* s, uint8_t bit_off, uint8_t size, uint8_t ext, uint32_t addr, uint8_t* rsp)
{
    XcpOdt* o;
    int used = 0, i;
    if (!s->ptr_valid) return put_err(rsp, XCP_ERR_SEQUENCE);
    if (ext != 0 || bit_off != 0xFF || size < 1 || size > max_entry(s))
        return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    size = (uint8_t)(size * s->ag);
    if (!work_ptr(s, addr, size)) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    o = &s->daq[s->ptr_daq].odt[s->ptr_odt];
    for (i = 0; i < o->alloc; i++)
        if (o->ent[i].used) used += o->ent[i].size;
    if (used + size + s->ag + 4 > XCP_MAX_DTO) return put_err(rsp, XCP_ERR_DAQ_CONFIG);
    if (s->ptr_entry >= o->alloc) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    o->ent[s->ptr_entry].addr = addr;
    o->ent[s->ptr_entry].size = size;
    o->ent[s->ptr_entry].used = 1;
    if (o->nent < s->ptr_entry + 1) o->nent = s->ptr_entry + 1;
    s->ptr_entry++;
    if (s->ptr_entry >= o->alloc) s->ptr_valid = 0;
    return 0;
}

static int cmd_write_daq(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int r;
    if (s->protect & XCP_RES_DAQ) return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    if (len < 8) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    r = write_one_entry(s, cmd[1], cmd[2], cmd[3], R32(s, cmd + 4), rsp);
    if (r) return r;
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_write_daq_multiple(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int n, i;
    if (s->protect & XCP_RES_DAQ) return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    if (len < 2) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    n = cmd[1];
    if (n < 1 || 2 + n * 7 > len) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    for (i = 0; i < n; i++) {
        const uint8_t* e = cmd + 2 + i * 7;
        int r = write_one_entry(s, e[0], e[1], e[2], R32(s, e + 3), rsp);
        if (r) return r;
    }
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_set_daq_mode(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int daq, ev;
    if (len < 8) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    daq = R16(s, cmd + 2);
    ev = R16(s, cmd + 4);
    if (daq >= s->n_daq || !s->daq[daq].alloc) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    if (ev >= XCP_MAX_EVENT) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    if (cmd[1] & XCP_DAQ_FLAG_DIRECTION) return put_err(rsp, XCP_ERR_MODE_NOT_VALID);
    s->daq[daq].mode = cmd[1];
    s->daq[daq].event = (uint16_t)ev;
    s->daq[daq].prescaler = cmd[6] ? cmd[6] : 1;
    s->daq[daq].priority = cmd[7];
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_get_daq_mode(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int daq;
    if (len < 4) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    daq = R16(s, cmd + 2);
    if (daq >= s->n_daq) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    rsp[0] = XCP_PID_RES;
    rsp[1] = s->daq[daq].mode;
    rsp[2] = 0;
    rsp[3] = 0;
    W16(s, rsp + 4, s->daq[daq].event);
    rsp[6] = s->daq[daq].prescaler;
    rsp[7] = s->daq[daq].priority;
    return 8;
}

static int first_pid(const XcpSlave* s, int daq)
{
    int i, pid = 0;
    for (i = 0; i < daq; i++) pid += s->daq[i].nodt;
    return pid;
}

static int cmd_start_stop_list(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int daq;
    if (s->protect & XCP_RES_DAQ) return put_err(rsp, XCP_ERR_ACCESS_LOCKED);
    if (len < 4) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    daq = R16(s, cmd + 2);
    if (daq >= s->n_daq || !s->daq[daq].alloc) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
    if (cmd[1] == XCP_DAQ_START_STOP) s->daq[daq].running = 0;
    else if (cmd[1] == XCP_DAQ_START_START) {
        s->daq[daq].running = 1;
        s->daq[daq].selected = 1;
        s->daq[daq].prescale_cnt = 0;
    } else if (cmd[1] == XCP_DAQ_START_SELECT) s->daq[daq].selected = 1;
    else return put_err(rsp, XCP_ERR_MODE_NOT_VALID);
    rsp[0] = XCP_PID_RES;
    rsp[1] = (uint8_t)first_pid(s, daq);
    return 2;
}

static int cmd_start_stop_synch(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp)
{
    int i;
    if (len < 2) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
    for (i = 0; i < s->n_daq; i++) {
        if (cmd[1] == XCP_DAQ_SYNCH_STOP_ALL) s->daq[i].running = 0;
        else if (cmd[1] == XCP_DAQ_SYNCH_START_SEL && s->daq[i].selected) {
            s->daq[i].running = 1;
            s->daq[i].prescale_cnt = 0;
        } else if (cmd[1] == XCP_DAQ_SYNCH_STOP_SEL && s->daq[i].selected)
            s->daq[i].running = 0;
        else if (cmd[1] > 2) return put_err(rsp, XCP_ERR_MODE_NOT_VALID);
    }
    rsp[0] = XCP_PID_RES;
    return 1;
}

static int cmd_read_daq(XcpSlave* s, uint8_t* rsp)
{
    XcpOdtEntry* e;
    if (!s->ptr_valid) return put_err(rsp, XCP_ERR_SEQUENCE);
    e = &s->daq[s->ptr_daq].odt[s->ptr_odt].ent[s->ptr_entry];
    rsp[0] = XCP_PID_RES;
    rsp[1] = e->used ? 0xFF : 0;
    rsp[2] = (uint8_t)(e->used ? e->size / s->ag : 0);
    rsp[3] = 0;
    W32(s, rsp + 4, e->used ? e->addr : 0);
    s->ptr_entry++;
    if (s->ptr_entry >= s->daq[s->ptr_daq].odt[s->ptr_odt].alloc) s->ptr_valid = 0;
    return 8;
}

void xcp_slave_tick(XcpSlave* s, uint16_t event, uint32_t time_us)
{
    int d, o, e, pos;
    uint8_t pkt[XCP_MAX_DTO];
    if (!s) return;
    s->clock = time_us;
    for (d = 0; d < s->n_daq; d++) {
        XcpDaq* q = &s->daq[d];
        int pid;
        if (!q->running || q->event != event) continue;
        q->prescale_cnt++;
        if (q->prescale_cnt < q->prescaler) continue;
        q->prescale_cnt = 0;
        pid = first_pid(s, d);
        for (o = 0; o < q->nodt; o++) {
            XcpOdt* od = &q->odt[o];
            pos = 0;
            memset(pkt, 0, (size_t)s->ag);
            pkt[0] = (uint8_t)(pid + o);
            pos = s->ag;     /* 绝对 ODT 号 + AG 对齐填充 */
            if (o == 0 && (q->mode & XCP_DAQ_FLAG_TIMESTAMP)) {
                W32(s, pkt + pos, time_us);
                pos += 4;
            }
            for (e = 0; e < od->nent; e++) {
                uint8_t* p;
                if (!od->ent[e].used) continue;
                p = work_ptr(s, od->ent[e].addr, od->ent[e].size);
                if (!p) continue;
                if (pos + od->ent[e].size > XCP_MAX_DTO) break;
                memcpy(pkt + pos, p, od->ent[e].size);
                pos += od->ent[e].size;
            }
            if (pos > s->ag) push_dto(s, pkt, pos);
        }
    }
}

int xcp_slave_input(XcpSlave* s, const uint8_t* cmd, int len, uint8_t* rsp, int rsp_cap)
{
    if (!s || !cmd || !rsp || len < 1 || rsp_cap < XCP_MAX_CTO)
        return rsp ? put_err(rsp, XCP_ERR_GENERIC) : -1;
    s->rqh = s->rqn = 0;  /* 新命令：丢弃未取走的块模式余帧 */
    if (cmd[0] == XCP_CMD_SYNCH) return put_err(rsp, XCP_ERR_CMD_SYNCH);
    if (cmd[0] == XCP_CMD_CONNECT) return cmd_connect(s, rsp);
    if (!s->connected) return put_err(rsp, XCP_ERR_CMD_BUSY);
    switch (cmd[0]) {
    case XCP_CMD_DISCONNECT:
        end_session(s);
        rsp[0] = XCP_PID_RES;
        return 1;
    case XCP_CMD_GET_STATUS: return cmd_get_status(s, rsp);
    case XCP_CMD_GET_COMM_MODE_INFO:
        rsp[0] = XCP_PID_RES;
        rsp[1] = 0;
        rsp[2] = 0x01; /* 主机块模式 */
        rsp[3] = 0;
        rsp[4] = 0;
        rsp[5] = 0;
        rsp[6] = 1;
        return 7;
    case XCP_CMD_GET_ID: return cmd_get_id(s, cmd, len, rsp);
    case XCP_CMD_SET_REQUEST:
        if (len < 4) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
        s->session = (uint8_t)((s->session & XCP_SS_DAQ_RUNNING) | (cmd[1] & 0x0Du));
        s->session_id = R16(s, cmd + 2);
        rsp[0] = XCP_PID_RES;
        return 1;
    case XCP_CMD_GET_SEED: return cmd_get_seed(s, cmd, len, rsp);
    case XCP_CMD_UNLOCK: return cmd_unlock(s, cmd, len, rsp);
    case XCP_CMD_SET_MTA: return cmd_set_mta(s, cmd, len, rsp);
    case XCP_CMD_UPLOAD: return cmd_upload(s, cmd, len, rsp);
    case XCP_CMD_SHORT_UPLOAD: return cmd_short_upload(s, cmd, len, rsp);
    case XCP_CMD_BUILD_CHECKSUM: return cmd_checksum(s, cmd, len, rsp);
    case XCP_CMD_DOWNLOAD: return cmd_download(s, cmd, len, rsp);
    case XCP_CMD_DOWNLOAD_NEXT: return cmd_download_next(s, cmd, len, rsp);
    case XCP_CMD_DOWNLOAD_MAX: return cmd_download_max(s, cmd, len, rsp);
    case XCP_CMD_SHORT_DOWNLOAD: return cmd_short_download(s, cmd, len, rsp);
    case XCP_CMD_MODIFY_BITS: return cmd_modify_bits(s, cmd, len, rsp);
    case XCP_CMD_SET_CAL_PAGE: return cmd_set_cal_page(s, cmd, len, rsp);
    case XCP_CMD_GET_CAL_PAGE: return cmd_get_cal_page(s, cmd, len, rsp);
    case XCP_CMD_GET_PAG_PROCESSOR_INFO:
        rsp[0] = XCP_PID_RES;
        rsp[1] = 1;
        rsp[2] = 0;
        return 3;
    case XCP_CMD_GET_SEGMENT_INFO:
        if (len < 5) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
        if (cmd[2] != 0) return put_err(rsp, XCP_ERR_SEGMENT_NOT_VALID);
        if (cmd[1] == 0) {
            rsp[0] = XCP_PID_RES;
            rsp[1] = rsp[2] = rsp[3] = 0;
            W32(s, rsp + 4, s->base);
            W32(s, rsp + 8, s->size);
            return 12;
        }
        if (cmd[1] == 1) {
            rsp[0] = XCP_PID_RES;
            rsp[1] = 2; /* 两页 */
            rsp[2] = 0;
            rsp[3] = 0;
            rsp[4] = 0;
            rsp[5] = 0;
            return 6;
        }
        return put_err(rsp, XCP_ERR_MODE_NOT_VALID);
    case XCP_CMD_GET_PAGE_INFO:
        if (len < 4) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
        if (cmd[2] != 0) return put_err(rsp, XCP_ERR_SEGMENT_NOT_VALID);
        if (cmd[3] > 1) return put_err(rsp, XCP_ERR_PAGE_NOT_VALID);
        rsp[0] = XCP_PID_RES;
        rsp[1] = cmd[3] == 0 ? 0x03 : 0x01; /* 工作页读写，参考页只读 */
        rsp[2] = 0;
        return 3;
    case XCP_CMD_SET_SEGMENT_MODE:
        if (len < 3) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
        if (cmd[2] != 0) return put_err(rsp, XCP_ERR_SEGMENT_NOT_VALID);
        s->seg_mode = cmd[1];
        rsp[0] = XCP_PID_RES;
        return 1;
    case XCP_CMD_GET_SEGMENT_MODE:
        if (len < 3) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
        if (cmd[2] != 0) return put_err(rsp, XCP_ERR_SEGMENT_NOT_VALID);
        rsp[0] = XCP_PID_RES;
        rsp[1] = s->seg_mode;
        return 2;
    case XCP_CMD_COPY_CAL_PAGE: return cmd_copy_cal_page(s, cmd, len, rsp);
    case XCP_CMD_FREE_DAQ: return cmd_free_daq(s, rsp);
    case XCP_CMD_ALLOC_DAQ: return cmd_alloc_daq(s, cmd, len, rsp);
    case XCP_CMD_ALLOC_ODT: return cmd_alloc_odt(s, cmd, len, rsp);
    case XCP_CMD_ALLOC_ODT_ENTRY: return cmd_alloc_odt_entry(s, cmd, len, rsp);
    case XCP_CMD_SET_DAQ_PTR: return cmd_set_daq_ptr(s, cmd, len, rsp);
    case XCP_CMD_WRITE_DAQ: return cmd_write_daq(s, cmd, len, rsp);
    case XCP_CMD_WRITE_DAQ_MULTIPLE: return cmd_write_daq_multiple(s, cmd, len, rsp);
    case XCP_CMD_SET_DAQ_LIST_MODE: return cmd_set_daq_mode(s, cmd, len, rsp);
    case XCP_CMD_GET_DAQ_LIST_MODE: return cmd_get_daq_mode(s, cmd, len, rsp);
    case XCP_CMD_START_STOP_DAQ_LIST: return cmd_start_stop_list(s, cmd, len, rsp);
    case XCP_CMD_START_STOP_SYNCH: return cmd_start_stop_synch(s, cmd, len, rsp);
    case XCP_CMD_GET_DAQ_CLOCK:
        rsp[0] = XCP_PID_RES;
        rsp[1] = rsp[2] = rsp[3] = 0;
        W32(s, rsp + 4, s->clock);
        return 8;
    case XCP_CMD_READ_DAQ: return cmd_read_daq(s, rsp);
    case XCP_CMD_GET_DAQ_PROCESSOR_INFO:
        rsp[0] = XCP_PID_RES;
        rsp[1] = 0x01 | 0x02 | 0x10; /* 动态、预分频、时间戳 */
        W16(s, rsp + 2, XCP_MAX_DAQ);
        W16(s, rsp + 4, XCP_MAX_EVENT);
        rsp[6] = 0;
        rsp[7] = 0;
        return 8;
    case XCP_CMD_GET_DAQ_RESOLUTION_INFO:
        rsp[0] = XCP_PID_RES;
        rsp[1] = (uint8_t)s->ag;
        rsp[2] = (uint8_t)max_entry(s);
        rsp[3] = (uint8_t)s->ag;
        rsp[4] = (uint8_t)max_entry(s);
        rsp[5] = XCP_TS_MODE;
        W16(s, rsp + 6, 1);
        return 8;
    case XCP_CMD_GET_DAQ_LIST_INFO: {
        int daq;
        if (len < 4) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
        daq = R16(s, cmd + 2);
        if (daq >= s->n_daq) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
        rsp[0] = XCP_PID_RES;
        rsp[1] = 0;
        rsp[2] = (uint8_t)s->daq[daq].nodt;
        rsp[3] = (uint8_t)(s->daq[daq].nodt ? s->daq[daq].odt[0].alloc : 0);
        W16(s, rsp + 4, s->daq[daq].event);
        rsp[6] = (uint8_t)first_pid(s, daq);
        return 7;
    }
    case XCP_CMD_GET_DAQ_EVENT_INFO: {
        int ev;
        if (len < 4) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
        ev = R16(s, cmd + 2);
        if (ev >= XCP_MAX_EVENT) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
        rsp[0] = XCP_PID_RES;
        rsp[1] = 0x04; /* DAQ */
        rsp[2] = XCP_MAX_DAQ;
        rsp[3] = 0;
        rsp[4] = (uint8_t)(ev == 0 ? 10 : 100);
        rsp[5] = 6; /* 1ms 单位 */
        rsp[6] = 0;
        return 7;
    }
    case XCP_CMD_CLEAR_DAQ_LIST: {
        int daq, o, e;
        if (len < 4) return put_err(rsp, XCP_ERR_CMD_SYNTAX);
        if (daq_running(s)) return put_err(rsp, XCP_ERR_DAQ_ACTIVE);
        daq = R16(s, cmd + 2);
        if (daq >= s->n_daq) return put_err(rsp, XCP_ERR_OUT_OF_RANGE);
        for (o = 0; o < s->daq[daq].nodt; o++)
            for (e = 0; e < XCP_MAX_ODT_ENTRY; e++)
                s->daq[daq].odt[o].ent[e].used = 0;
        s->daq[daq].running = 0;
        rsp[0] = XCP_PID_RES;
        return 1;
    }
    case XCP_CMD_TRANSPORT_LAYER_CMD:
    case XCP_CMD_USER_CMD:
        return put_err(rsp, XCP_ERR_SUBCMD_UNKNOWN);
    case XCP_CMD_PROGRAM_START:
    case XCP_CMD_PROGRAM_CLEAR:
    case XCP_CMD_PROGRAM:
    case XCP_CMD_PROGRAM_RESET:
    case XCP_CMD_GET_PGM_PROCESSOR_INFO:
    case XCP_CMD_GET_SECTOR_INFO:
    case XCP_CMD_PROGRAM_PREPARE:
    case XCP_CMD_PROGRAM_FORMAT:
    case XCP_CMD_PROGRAM_NEXT:
    case XCP_CMD_PROGRAM_MAX:
    case XCP_CMD_PROGRAM_VERIFY:
        return put_err(rsp, XCP_ERR_CMD_UNKNOWN);
    default:
        return put_err(rsp, XCP_ERR_CMD_UNKNOWN);
    }
}
