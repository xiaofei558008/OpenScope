/*
 * OpenScope 网络远程操作模块（network.dll）
 *
 * 能力：OS_CAP_NET。自研 RFC6455 WebSocket（netws）+ 二进制协议会话（netsession）+
 * 时序样本编解码（netcodec：Gorilla 式 delta/XOR + 多核并行压缩）。
 *
 * 服务端（本地探针侧）：监听 → 每客户端一线程（一对多 fan-out）→ HELLO/ELF_SYNC/
 *   WATCH_LIST（驱动本地采集）/SAMPLE_BATCH（采集流）/WRITE_VAR（写 MCU 回 ACK）/
 *   LOG_REQ（采集历史分块回传）。
 * 客户端（远端显示侧）：连接 → HELLO → 收 ELF_SYNC/SAMPLE_BATCH/ACK/CHUNK。
 *
 * 需求 14 全链路：
 *   远端 WATCH_LIST → 服务端 set_watch 勾选叶 + acq_start 启动采集 → 宿主采集线程
 *   周期调用 OS_CMD_NET_PUSH → 按客户端各自的监视列表广播 SAMPLE_BATCH（flat 编码）
 *   → 远端 push_sample 注入波形/数值窗口。
 *   远端 WRITE_VAR → 服务端 write_leaf 写入 MCU → ACK 回传。
 *   远端 LOG_REQ → 服务端 ring_copy 取环缓冲 → encode_parallel（多核压缩）→ CHUNK
 *   分块流 → 远端 accum 重组 → decode_parallel → push_sample 注入（异步传输）。
 */
#include "module_api.h"
#include "netproto.h"
#include "netcodec.h"
#include "netsession.h"
#include "netws.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NET_MAX_CLIENTS 16
#define NET_MAX_WATCH   256
#define NET_CHUNK_SIZE  4096
#define NET_RING_CAP    65536  /* 异步回传最多复制 65536 个样本 */
#define NET_WRITE_ACK_MS 3000  /* 网络写值等待对端 ACK 超时 */
#define NET_VAR_MAX     2048   /* ELF 变量表上限（大 ELF 有数百~上千叶） */
#define NET_RECV_CAP    524288 /* 会话接收缓冲 512KB：容纳大 ELF 变量表帧 */

/* 每个网络对端（服务端的客户端或客户端的服务端）一份会话状态 */
typedef struct NetClient {
    OS_WSConn*  conn;
    int         watch_ids[NET_MAX_WATCH]; /* 该对端下达的监视叶 id 列表 */
    int         watch_count;
    OS_NetAccum chunk_acc;                /* CHUNK 流重组累计器 */
} NetClient;

static const OS_Framework* g_fw;
static OS_DriverInfo g_info;
static OS_NetCfg g_cfg;
static int g_listen = -1;
static volatile LONG g_running;
static HANDLE g_thread;
static NetClient g_clients[NET_MAX_CLIENTS];
static CRITICAL_SECTION g_cli_cs;
static HANDLE g_ack_evt;               /* 网络写值 ACK 事件（NET_WRITE 同步等待用） */
static volatile LONG g_ack_code;
static char g_ack_msg[128];
/* 远端监视引用：每叶被多少个远端客户端监视（重建全局监视集用，≤4096 叶） */
#define NET_REF_CAP 4096
static int g_net_ref[NET_REF_CAP];
static uint8_t g_local_watch[NET_REF_CAP]; /* 首个远端监视到达时的本地勾选快照 */

/* 重建探针侧全局监视集 = 本地勾选 ∪ 各远端监视并集；空集则停止采集 */
static void rebuild_watches(void)
{
    int i, k, total = 0;
    if (!g_fw || g_fw->api_version < 4 || !g_fw->set_watch || !g_fw->acq_start ||
        !g_fw->leaf_watched || !g_fw->acq_stop)
        return;
    for (i = 0; i < g_fw->leaf_count() && i < NET_REF_CAP; i++)
        g_fw->set_watch(i, (g_local_watch[i] || g_net_ref[i] > 0) ? 1 : 0);
    for (k = 0; k < NET_MAX_CLIENTS; k++) {
        NetClient* c = &g_clients[k];
        if (!c->conn) continue;
        for (i = 0; i < c->watch_count; i++)
            if (c->watch_ids[i] >= 0 && c->watch_ids[i] < NET_REF_CAP) total++;
    }
    if (total > 0) {
        int rc = g_fw->acq_start();
        if (g_fw) g_fw->log(rc == 0 ? OS_LOG_INFO : OS_LOG_WARN,
                            "network: 监视并集重建 -> 采集启动 rc=%d", rc);
    } else {
        g_fw->acq_stop();
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 监视并集为空 -> 采集停止");
    }
}

static int build_varlist(OS_NetVar* v, int max)
{
    int i, n = 0;
    if (!g_fw) return 0;
    n = g_fw->leaf_count();
    if (n > max) n = max;
    for (i = 0; i < n; i++) {
        const char* nm = g_fw->leaf_name(i);
        const OS_Sample* s = g_fw->leaf_sample(i);
        memset(&v[i], 0, sizeof(v[i]));
        if (nm) _snprintf(v[i].name, OS_NET_NAME_MAX, "%s", nm);
        v[i].addr = g_fw->leaf_addr ? g_fw->leaf_addr(i) : 0;
        v[i].size = s ? (uint32_t)s->size : 0;
    }
    return n;
}

/* 把我的 ELF 变量表编码发送给对端（大 ELF 数百叶：变量表/载荷全走堆） */
static int send_varlist(OS_WSConn* c, const char* why)
{
    OS_NetVar* vars;
    uint8_t* payload;
    int cnt, enc, rc = -1;
    if (!g_fw) return -1;
    vars = (OS_NetVar*)malloc((size_t)NET_VAR_MAX * sizeof(OS_NetVar));
    if (!vars) return -1;
    cnt = build_varlist(vars, NET_VAR_MAX);
    payload = (uint8_t*)malloc((size_t)NET_VAR_MAX * (OS_NET_NAME_MAX + 16) + 64);
    if (payload) {
        enc = os_net_encode_varlist(vars, cnt, payload,
                                    (int)((size_t)NET_VAR_MAX * (OS_NET_NAME_MAX + 16) + 64));
        if (enc >= 0) rc = send_msg(c, OS_NET_MSG_ELF_SYNC, 0, payload, (uint32_t)enc);
        free(payload);
    }
    free(vars);
    if (g_fw && why) g_fw->log(OS_LOG_INFO, "network: %s ELF 变量表 %d 项", why, cnt);
    return rc;
}

static int resolve_name_to_id(const char* name)
{
    int i, n;
    if (!g_fw || !name) return -1;
    /* 直接按叶变量全路径名匹配（find_variable 只认顶层变量，如 "g_cfg.a" 会失败） */
    n = g_fw->leaf_count();
    for (i = 0; i < n; i++) {
        const char* nm = g_fw->leaf_name(i);
        if (nm && strcmp(nm, name) == 0) return i;
    }
    return -1;
}

/* 帧编码统一走堆（ELF 变量表可到数百 KB），不依赖调用线程栈大小 */
static int send_msg(OS_WSConn* c, uint8_t type, uint32_t seq, const uint8_t* payload, uint32_t len)
{
    uint8_t* frame = (uint8_t*)malloc(OS_NET_FRAME_HDR + (size_t)len);
    int n, rc;
    if (!frame) return -1;
    n = os_net_frame_encode(frame, OS_NET_FRAME_HDR + (int)len, type, 0, seq, payload, len);
    if (n < 0) { free(frame); return -1; }
    rc = os_ws_send_bin(c, frame, (uint32_t)n);
    free(frame);
    return rc;
}

/* 把该客户端的监视变量最新样本打包成 SAMPLE_BATCH 发送 */
static int push_samples_to(NetClient* cl)
{
    OS_NetSample samples[NET_MAX_WATCH];
    uint8_t payload[16384];
    int n = 0, i, enc;
    if (cl->watch_count <= 0) return 0;
    for (i = 0; i < cl->watch_count && n < NET_MAX_WATCH; i++) {
        const OS_Sample* s = g_fw ? g_fw->leaf_sample(cl->watch_ids[i]) : NULL;
        if (s && s->size) {
            samples[n].ts_us = s->ts_us;
            samples[n].value = s->value;
            samples[n].var_id = cl->watch_ids[i];
            n++;
        }
    }
    if (n <= 0) return 0;
    enc = os_net_codec_encode_flat(samples, n, payload, sizeof(payload));
    if (enc < 0) return -1;
    return send_msg(cl->conn, OS_NET_MSG_SAMPLE_BATCH, 0, payload, (uint32_t)enc);
}

static void broadcast_samples(void)
{
    int i;
    EnterCriticalSection(&g_cli_cs);
    for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i].conn) push_samples_to(&g_clients[i]);
    LeaveCriticalSection(&g_cli_cs);
}

static void broadcast_elf(void)
{
    int i;
    EnterCriticalSection(&g_cli_cs);
    for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i].conn)
        send_varlist(g_clients[i].conn, NULL);
    LeaveCriticalSection(&g_cli_cs);
    if (g_fw) g_fw->log(OS_LOG_INFO, "network: 广播 ELF 变量表完成");
}

/* 服务端：远端 LOG_REQ → 环缓冲历史 → 多核压缩 → CHUNK 分块流回传（异步传输） */
static void send_log_history(OS_WSConn* c)
{
    OS_Sample* samples;
    uint8_t* enc;
    uint8_t chunkbuf[NET_CHUNK_SIZE + 4];
    int n, enc_len;
    if (!g_fw || !g_fw->ring_copy) return;
    samples = (OS_Sample*)malloc((size_t)NET_RING_CAP * sizeof(OS_Sample));
    if (!samples) return;
    n = g_fw->ring_copy(samples, NET_RING_CAP);
    if (n <= 0) {
        /* 无历史数据：回一个空 ACK 告知对端 */
        uint8_t ack[256]; int alen;
        free(samples);
        alen = os_net_encode_ack(-1, "no data", ack, sizeof(ack));
        if (alen >= 0) send_msg(c, OS_NET_MSG_ACK, 0, ack, (uint32_t)alen);
        return;
    }
    /* 最坏情形每样本 ~32B（时间戳 delta + 全异或 double） */
    enc = (uint8_t*)malloc((size_t)n * 32 + 4096);
    if (!enc) { free(samples); return; }
    enc_len = os_net_codec_encode_parallel(samples, n, 8, enc, (int)((size_t)n * 32 + 4096));
    if (enc_len > 0) {
        uint32_t off = 0, idx = 0;
        while (off < (uint32_t)enc_len) {
            uint32_t l = (uint32_t)enc_len - off;
            int clen;
            if (l > NET_CHUNK_SIZE) l = NET_CHUNK_SIZE;
            clen = os_net_chunk_stream_encode(idx, (uint32_t)enc_len + 4, enc + off, l,
                                              chunkbuf, sizeof(chunkbuf));
            if (clen < 0) break;
            if (send_msg(c, OS_NET_MSG_CHUNK, 0, chunkbuf, (uint32_t)clen) != 0) break;
            off += l; idx++;
        }
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 历史回传 %d 样本（压缩 %d 字节，%d 块）",
                            n, enc_len, idx);
    }
    free(enc);
    free(samples);
}

static void handle_msg(NetClient* cl, const OS_NetFrame* f)
{
    uint8_t ack[256];
    int n;
    switch (f->type) {
    case OS_NET_MSG_HELLO: {
        send_varlist(cl->conn, "HELLO -> 发送");
        break;
    }
    case OS_NET_MSG_WATCH_LIST: {
        /* 服务端角色：远端下达的监视列表 → 勾选叶 + 启动采集（需求 14 核心链路）。
         * 客户端角色收到 WATCH_LIST 只记录（供日志核对），不驱动本地采集。
         * 空列表 = 停止下达：撤销该客户端全部监视；全部远端撤销后停采。 */
        char names[256][OS_NET_NAME_MAX];
        int i, cnt = os_net_decode_names(f->payload, (int)f->len, names, 256);
        int new_ids[NET_MAX_WATCH], new_n = 0;
        static int local_snap;
        if (!local_snap && g_listen >= 0 && g_fw && g_fw->api_version >= 4 && g_fw->leaf_watched) {
            /* 首个远端监视到达：快照本地勾选（后续重建保留本地勾选） */
            for (i = 0; i < g_fw->leaf_count() && i < NET_REF_CAP; i++)
                g_local_watch[i] = g_fw->leaf_watched(i) ? 1 : 0;
            local_snap = 1;
        }
        for (i = 0; i < cnt && new_n < NET_MAX_WATCH; i++) {
            int id = resolve_name_to_id(names[i]);
            if (id >= 0 && id < NET_REF_CAP) new_ids[new_n++] = id;
        }
        if (g_listen >= 0) {
            /* 撤销旧监视引用 → 应用新列表并计数 → 重建全局监视集 */
            for (i = 0; i < cl->watch_count; i++) {
                int id = cl->watch_ids[i];
                if (id >= 0 && id < NET_REF_CAP && g_net_ref[id] > 0) g_net_ref[id]--;
            }
            cl->watch_count = new_n;
            for (i = 0; i < new_n; i++) {
                cl->watch_ids[i] = new_ids[i];
                g_net_ref[new_ids[i]]++;
            }
            rebuild_watches();
        } else {
            cl->watch_count = new_n;
            for (i = 0; i < new_n; i++) cl->watch_ids[i] = new_ids[i];
        }
        n = os_net_encode_ack(0, "watch ok", ack, sizeof(ack));
        if (n >= 0) send_msg(cl->conn, OS_NET_MSG_ACK, 0, ack, (uint32_t)n);
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: WATCH_LIST %d 项 -> %d 个监视变量%s",
                            cnt, cl->watch_count, cnt == 0 ? "（停止下达）" : "");
        break;
    }
    case OS_NET_MSG_WRITE_VAR: {
        char name[OS_NET_NAME_MAX], text[64];
        int id, code = -1;
        char err[128] = "";
        if (os_net_decode_write(f->payload, (int)f->len, name, sizeof(name), text, sizeof(text)) == 0) {
            id = resolve_name_to_id(name);
            if (id >= 0 && g_fw && g_fw->write_leaf) {
                code = g_fw->write_leaf(id, atof(text), err, sizeof(err));
            } else if (id < 0) {
                _snprintf(err, sizeof(err), "变量未解析");
            }
            if (g_fw) g_fw->log(code == 0 ? OS_LOG_INFO : OS_LOG_WARN,
                                "network: WRITE_VAR %s=%s -> %d (%s)", name, text, code, err);
        }
        n = os_net_encode_ack(code, code == 0 ? "ok" : "write failed", ack, sizeof(ack));
        if (n >= 0) send_msg(cl->conn, OS_NET_MSG_ACK, 0, ack, (uint32_t)n);
        break;
    }
    case OS_NET_MSG_SAMPLE_BATCH: {
        /* 远端样本 → 注入本地采集通道（波形/数值窗口显示） */
        OS_NetSample samples[256];
        int cnt = os_net_codec_decode_flat(f->payload, (int)f->len, samples, 256);
        int i;
        static ULONGLONG s_last_batch_log;
        static LONG s_batch_total;
        ULONGLONG now_ms = GetTickCount64();
        InterlockedAdd(&s_batch_total, cnt);
        for (i = 0; i < cnt; i++) {
            if (g_fw && g_fw->push_sample) {
                OS_Sample s;
                memset(&s, 0, sizeof(s));
                s.ts_us = samples[i].ts_us;
                s.value = samples[i].value;
                s.var_id = samples[i].var_id;
                s.size = 8;
                if (g_fw->leaf_addr) s.address = g_fw->leaf_addr(samples[i].var_id);
                _snprintf(s.text, sizeof(s.text), "%g", samples[i].value);
                g_fw->push_sample(&s);
            }
        }
        if (now_ms - s_last_batch_log >= 1000) { /* 节流：每秒一条统计 */
            s_last_batch_log = now_ms;
            if (g_fw) g_fw->log(OS_LOG_INFO, "network: 样本注入 %d 个（累计 %d）",
                                cnt, s_batch_total);
        }
        break;
    }
    case OS_NET_MSG_CHUNK: {
        /* 异步历史回传块 → 累计重组 → 解码注入（大缓冲全部走堆，避免会话线程栈溢出） */
        uint8_t* big = (uint8_t*)malloc((size_t)NET_RING_CAP * 32 + 4096);
        OS_NetSample* samples = NULL;
        int got = 0, cnt = 0;
        if (!big) break;
        got = os_net_chunk_stream_feed(&cl->chunk_acc, f->payload, (int)f->len,
                                       big, (int)((size_t)NET_RING_CAP * 32 + 4096));
        if (got > 0) {
            samples = (OS_NetSample*)malloc((size_t)NET_RING_CAP * sizeof(OS_NetSample));
            if (samples) {
                cnt = os_net_codec_decode_parallel(big, got, samples, NET_RING_CAP);
                { int i;
                  for (i = 0; i < cnt && g_fw && g_fw->push_sample; i++) {
                      OS_Sample s;
                      memset(&s, 0, sizeof(s));
                      s.ts_us = samples[i].ts_us;
                      s.value = samples[i].value;
                      s.var_id = samples[i].var_id;
                      s.size = 8;
                      if (g_fw->leaf_addr) s.address = g_fw->leaf_addr(samples[i].var_id);
                      _snprintf(s.text, sizeof(s.text), "%g", samples[i].value);
                      g_fw->push_sample(&s);
                  } }
                if (g_fw) g_fw->log(OS_LOG_INFO, "network: 历史数据回传完成 %d 样本（异步传输）", cnt);
                free(samples);
            }
        }
        free(big);
        break;
    }
    case OS_NET_MSG_ACK: {
        int code = -1; char msg[128] = "";
        os_net_decode_ack(f->payload, (int)f->len, &code, msg, sizeof(msg));
        InterlockedExchange(&g_ack_code, code);
        if (msg[0]) _snprintf(g_ack_msg, sizeof(g_ack_msg), "%s", msg);
        SetEvent(g_ack_evt);
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 收到 ACK code=%d msg=%s", code, msg);
        break;
    }
    case OS_NET_MSG_ELF_SYNC: {
        /* 对端变量表：核对项数进日志（两侧同 ELF 时名称/地址一致，便于快速采集） */
        OS_NetVar* vars = (OS_NetVar*)malloc((size_t)NET_VAR_MAX * sizeof(OS_NetVar));
        int cnt = 0;
        if (vars) {
            cnt = os_net_decode_varlist(f->payload, (int)f->len, vars, NET_VAR_MAX);
            free(vars);
        }
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 收到 ELF 变量表 %d 项", cnt);
        break;
    }
    case OS_NET_MSG_ELF_REQ: {
        /* 对端请求 ELF 变量表 → 回发我的变量表（双向同步的"下载"方向） */
        send_varlist(cl->conn, "收到 ELF 请求 -> 回发");
        break;
    }
    case OS_NET_MSG_LOG_REQ: {
        if (g_listen >= 0) send_log_history(cl->conn);
        break;
    }
    default:
        break;
    }
}

static void session_serve(int slot, OS_WSConn* my)
{
    uint8_t* buf = (uint8_t*)malloc(NET_RECV_CAP);
    int op;
    if (!buf) return;
    while (g_running && g_clients[slot].conn == my) {
        int n = os_ws_recv(my, buf, NET_RECV_CAP, &op);
        if (n <= 0) break;
        if (op == OS_WS_OP_BIN && n >= OS_NET_FRAME_HDR) {
            OS_NetFrame f;
            if (os_net_frame_decode(buf, n, &f) > 0) handle_msg(&g_clients[slot], &f);
        }
    }
    free(buf);
}

static DWORD WINAPI client_thread(LPVOID p)
{
    int slot = (int)(INT_PTR)p;
    OS_WSConn* my;
    EnterCriticalSection(&g_cli_cs);
    my = g_clients[slot].conn;
    LeaveCriticalSection(&g_cli_cs);
    if (!my) return 0;
    session_serve(slot, my);
    /* 槽位被复用（NET_STOP 后新连接）时不清理新连接，只释放自己持有的连接 */
    EnterCriticalSection(&g_cli_cs);
    if (g_clients[slot].conn == my) {
        g_clients[slot].conn = NULL;
        os_net_accum_free(&g_clients[slot].chunk_acc);
    }
    LeaveCriticalSection(&g_cli_cs);
    os_ws_close(my);
    if (g_fw) g_fw->log(OS_LOG_INFO, "network: 对端断开（槽 %d）", slot + 1);
    return 0;
}

static DWORD WINAPI server_thread(LPVOID p)
{
    (void)p;
    while (g_running) {
        OS_WSConn* c = os_ws_accept(g_listen);
        int i, slot = -1;
        if (!c) { if (g_running) Sleep(20); continue; }
        EnterCriticalSection(&g_cli_cs);
        for (i = 0; i < NET_MAX_CLIENTS; i++) if (!g_clients[i].conn) { slot = i; break; }
        if (slot >= 0) {
            g_clients[slot].conn = c;
            g_clients[slot].watch_count = 0;
            os_net_accum_init(&g_clients[slot].chunk_acc);
        }
        LeaveCriticalSection(&g_cli_cs);
        if (slot < 0) { os_ws_close(c); continue; }
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 客户端接入（第 %d 个）", slot + 1);
        { HANDLE th = CreateThread(NULL, 0, client_thread, (LPVOID)(INT_PTR)slot, 0, NULL);
          if (th) CloseHandle(th); }
    }
    return 0;
}

/* ---------------- 模块命令 ---------------- */

static int mod_command(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx; (void)out;
    switch (cmd) {
    case OS_CMD_GET_INFO:
        if (out) memcpy(out, &g_info, sizeof(g_info));
        return OS_ERR_OK;
    case OS_CMD_NET_START:
        if (in) memcpy(&g_cfg, in, sizeof(g_cfg));
        if (g_listen >= 0) os_ws_close_listen(g_listen);
        g_listen = os_ws_listen(g_cfg.ip[0] ? g_cfg.ip : NULL, g_cfg.port);
        if (g_listen < 0) { if (g_fw) g_fw->log(OS_LOG_ERROR, "network: 监听失败 %s:%d", g_cfg.ip, g_cfg.port); return OS_ERR_FAIL; }
        g_running = 1;
        g_thread = CreateThread(NULL, 0, server_thread, NULL, 0, NULL);
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 服务端监听 %s:%d", g_cfg.ip[0] ? g_cfg.ip : "0.0.0.0", g_cfg.port);
        return OS_ERR_OK;
    case OS_CMD_NET_CONNECT: {
        OS_WSConn* c; int slot = -1, i;
        if (in) memcpy(&g_cfg, in, sizeof(g_cfg));
        g_running = 1; /* 客户端会话循环也依赖此标志（纯客户端不调用 NET_START） */
        c = os_ws_connect(g_cfg.ip, g_cfg.port);
        if (!c) { if (g_fw) g_fw->log(OS_LOG_ERROR, "network: 连接失败 %s:%d", g_cfg.ip, g_cfg.port); return OS_ERR_FAIL; }
        EnterCriticalSection(&g_cli_cs);
        for (i = 0; i < NET_MAX_CLIENTS; i++) if (!g_clients[i].conn) { slot = i; break; }
        if (slot >= 0) {
            g_clients[slot].conn = c;
            g_clients[slot].watch_count = 0;
            os_net_accum_init(&g_clients[slot].chunk_acc);
        }
        LeaveCriticalSection(&g_cli_cs);
        if (slot < 0) { os_ws_close(c); return OS_ERR_FAIL; }
        { uint8_t z = 0; send_msg(c, OS_NET_MSG_HELLO, 0, &z, 0); }
        { HANDLE th = CreateThread(NULL, 0, client_thread, (LPVOID)(INT_PTR)slot, 0, NULL);
          if (th) CloseHandle(th); }
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 已连接 %s:%d", g_cfg.ip, g_cfg.port);
        return OS_ERR_OK;
    }
    case OS_CMD_NET_STOP:
        g_running = 0;
        if (g_listen >= 0) { os_ws_close_listen(g_listen); g_listen = -1; }
        { int i; EnterCriticalSection(&g_cli_cs);
          /* shutdown 唤醒会话线程，连接由会话线程自持自释放（槽位随后腾空复用） */
          for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i].conn)
              os_ws_shutdown(g_clients[i].conn);
          LeaveCriticalSection(&g_cli_cs); }
        if (g_thread) { WaitForSingleObject(g_thread, 2000); CloseHandle(g_thread); g_thread = NULL; }
        Sleep(50); /* 给会话线程腾槽窗口 */
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 已停止");
        return OS_ERR_OK;
    case OS_CMD_NET_SYNC_ELF:
        broadcast_elf();
        return OS_ERR_OK;
    case OS_CMD_NET_ELF_PULL: {
        int i; uint8_t z = 0;
        EnterCriticalSection(&g_cli_cs);
        for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i].conn)
            send_msg(g_clients[i].conn, OS_NET_MSG_ELF_REQ, 0, &z, 0);
        LeaveCriticalSection(&g_cli_cs);
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 请求远端 ELF 变量表");
        return OS_ERR_OK;
    }
    case OS_CMD_NET_PUSH:
        broadcast_samples();
        return OS_ERR_OK;
    case OS_CMD_NET_WATCH: {
        /* 远端显示侧：把我的勾选叶名称列表发给对端（对端据此采集并回传） */
        char names[NET_MAX_WATCH][OS_NET_NAME_MAX];
        const char* ptrs[NET_MAX_WATCH];
        uint8_t payload[16384];
        int i, cnt = 0, enc;
        if (!g_fw || g_fw->api_version < 4 || !g_fw->leaf_watched) {
            if (g_fw) g_fw->log(OS_LOG_WARN, "network: 框架不支持监视列表导出");
            return OS_ERR_FAIL;
        }
        for (i = 0; i < g_fw->leaf_count() && cnt < NET_MAX_WATCH; i++) {
            if (g_fw->leaf_watched(i)) {
                const char* nm = g_fw->leaf_name(i);
                if (nm) {
                    _snprintf(names[cnt], OS_NET_NAME_MAX, "%s", nm);
                    ptrs[cnt] = names[cnt];
                    cnt++;
                }
            }
        }
        if (cnt <= 0) {
            if (g_fw) g_fw->log(OS_LOG_WARN, "network: 没有勾选的观测变量，未发送监视列表");
            return OS_ERR_FAIL;
        }
        enc = os_net_encode_names(ptrs, cnt, payload, sizeof(payload));
        if (enc < 0) return OS_ERR_FAIL;
        { int i2, sent = 0;
          EnterCriticalSection(&g_cli_cs);
          for (i2 = 0; i2 < NET_MAX_CLIENTS; i2++) if (g_clients[i2].conn)
              if (send_msg(g_clients[i2].conn, OS_NET_MSG_WATCH_LIST, 0, payload, (uint32_t)enc) == 0) sent++;
          LeaveCriticalSection(&g_cli_cs);
          if (g_fw) g_fw->log(OS_LOG_INFO, "network: 发送监视列表 %d 项 -> %d 个对端", cnt, sent);
          if (sent <= 0) return OS_ERR_NOT_CONNECTED; }
        return OS_ERR_OK;
    }
    case OS_CMD_NET_WRITE: {
        /* 网络写变量：发给所有对端，等待首个 ACK（≤3s），返回 0/负错误码 */
        OS_NetWriteReq* req = (OS_NetWriteReq*)in;
        uint8_t payload[512];
        int i, enc, sent = 0;
        DWORD rc;
        if (!req || !req->name[0]) return OS_ERR_INVALID_ARG;
        enc = os_net_encode_write(req->name, req->value, payload, sizeof(payload));
        if (enc < 0) return OS_ERR_FAIL;
        InterlockedExchange(&g_ack_code, -1);
        g_ack_msg[0] = 0;
        ResetEvent(g_ack_evt);
        EnterCriticalSection(&g_cli_cs);
        for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i].conn)
            if (send_msg(g_clients[i].conn, OS_NET_MSG_WRITE_VAR, 0, payload, (uint32_t)enc) == 0) sent++;
        LeaveCriticalSection(&g_cli_cs);
        if (sent <= 0) return OS_ERR_NOT_CONNECTED;
        rc = WaitForSingleObject(g_ack_evt, NET_WRITE_ACK_MS);
        if (rc != WAIT_OBJECT_0) return OS_ERR_TIMEOUT;
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 网络写入 %s=%s -> ACK %d (%s)",
                            req->name, req->value, g_ack_code, g_ack_msg[0] ? g_ack_msg : "-");
        return g_ack_code == 0 ? OS_ERR_OK : OS_ERR_FAIL;
    }
    case OS_CMD_NET_WATCH_STOP: {
        /* 停止下达：发送空监视列表，探针侧撤销本客户端监视并在无人监视时停采 */
        uint8_t payload[16];
        int enc = os_net_encode_names(NULL, 0, payload, sizeof(payload));
        int i2, sent = 0;
        if (enc < 0) return OS_ERR_FAIL;
        EnterCriticalSection(&g_cli_cs);
        for (i2 = 0; i2 < NET_MAX_CLIENTS; i2++) if (g_clients[i2].conn)
            if (send_msg(g_clients[i2].conn, OS_NET_MSG_WATCH_LIST, 0, payload, (uint32_t)enc) == 0) sent++;
        LeaveCriticalSection(&g_cli_cs);
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 停止下达（空监视列表 -> %d 个对端）", sent);
        return sent > 0 ? OS_ERR_OK : OS_ERR_NOT_CONNECTED;
    }
    case OS_CMD_NET_LOG_PULL: {
        int i; uint8_t z = 0;
        EnterCriticalSection(&g_cli_cs);
        for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i].conn)
            send_msg(g_clients[i].conn, OS_NET_MSG_LOG_REQ, 0, &z, 0);
        LeaveCriticalSection(&g_cli_cs);
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 请求远端采集历史（异步传输）");
        return OS_ERR_OK;
    }
    case OS_CMD_ELF_RELOADED:
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: ELF 已重载");
        return OS_ERR_OK;
    default:
        return OS_ERR_FAIL;
    }
}

static int mod_init(const OS_Framework* fw, void** out_ctx)
{
    (void)out_ctx;
    g_fw = fw;
    InitializeCriticalSection(&g_cli_cs);
    g_ack_evt = CreateEventW(NULL, TRUE, FALSE, NULL);
    memset(&g_info, 0, sizeof(g_info));
    _snprintf(g_info.name, sizeof(g_info.name), "%s", "network");
    _snprintf(g_info.version, sizeof(g_info.version), "%s", "0.3.0");
    _snprintf(g_info.dll_version, sizeof(g_info.dll_version), "%s", "ws-rfc6455");
    _snprintf(g_info.emulator, sizeof(g_info.emulator), "%s", "未连接");
    if (fw) fw->log(OS_LOG_INFO, "network 模块: 已初始化（WebSocket + 监视采集链路 + 异步历史回传 + 一对多 fan-out 就绪）");
    return OS_ERR_OK;
}

static void mod_deinit(void* ctx)
{
    (void)ctx;
    g_running = 0;
    { int i; EnterCriticalSection(&g_cli_cs);
      for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i].conn)
          os_ws_shutdown(g_clients[i].conn);
      LeaveCriticalSection(&g_cli_cs); }
    if (g_listen >= 0) { os_ws_close_listen(g_listen); g_listen = -1; }
    if (g_thread) { WaitForSingleObject(g_thread, 2000); CloseHandle(g_thread); g_thread = NULL; }
    Sleep(50); /* 让会话线程退出并释放连接后再卸载模块 */
    if (g_ack_evt) { CloseHandle(g_ack_evt); g_ack_evt = NULL; }
    DeleteCriticalSection(&g_cli_cs);
}

static const OS_Module g_module = {
    OS_API_VERSION,
    OS_CAP_NET,
    "network",
    "0.3.0",
    "网络远程操作模块：WebSocket 传输 + 监视采集链路 + 异步历史回传 + 一对多 fan-out",
    NULL,
    mod_init,
    mod_deinit,
    mod_command,
    NULL, NULL, NULL, NULL, NULL, NULL
};

const OS_Module* os_module_get(void)
{
    return &g_module;
}
