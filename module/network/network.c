/*
 * OpenScope 网络远程操作模块（network.dll）
 *
 * 能力：OS_CAP_NET。自研 RFC6455 WebSocket（netws）+ 二进制协议会话（netsession）+
 * 时序样本编解码（netcodec）。
 * 服务端（本地探针侧）：监听 → 每客户端一线程（一对多 fan-out）→ HELLO/ELF_SYNC/
 *   WATCH_LIST/SAMPLE_BATCH/WRITE_VAR(写 MCU 回 ACK)。
 * 客户端（远端显示侧）：连接 → HELLO → 收 ELF_SYNC/SAMPLE_BATCH/ACK。
 * 样本推送/ELF 同步由宿主经 OS_CMD_NET_PUSH/OS_CMD_NET_SYNC_ELF 触发，广播到所有客户端。
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

static const OS_Framework* g_fw;
static OS_DriverInfo g_info;
static OS_NetCfg g_cfg;
static int g_listen = -1;
static volatile LONG g_running;
static HANDLE g_thread;
static OS_WSConn* g_clients[NET_MAX_CLIENTS];
static CRITICAL_SECTION g_cli_cs;
static int g_watch_ids[256];
static int g_watch_count;

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

static int resolve_name_to_id(const char* name)
{
    int i, n;
    if (!g_fw || !g_fw->find_variable) return -1;
    if (!g_fw->find_variable(name)) return -1;
    n = g_fw->leaf_count();
    for (i = 0; i < n; i++) {
        const char* nm = g_fw->leaf_name(i);
        if (nm && strcmp(nm, name) == 0) return i;
    }
    return -1;
}

static int send_msg(OS_WSConn* c, uint8_t type, uint32_t seq, const uint8_t* payload, uint32_t len)
{
    uint8_t frame[65536];
    int n = os_net_frame_encode(frame, sizeof(frame), type, 0, seq, payload, len);
    if (n < 0) return -1;
    return os_ws_send_bin(c, frame, (uint32_t)n);
}

/* 把监视变量打包成 SAMPLE_BATCH 发给单个客户端 */
static int push_samples_to(OS_WSConn* c)
{
    OS_NetSample samples[256];
    uint8_t payload[8192];
    int n = 0, i, enc;
    if (g_watch_count <= 0) return 0;
    for (i = 0; i < g_watch_count && n < 256; i++) {
        const OS_Sample* s = g_fw ? g_fw->leaf_sample(g_watch_ids[i]) : NULL;
        if (s && s->size) {
            samples[n].ts_us = s->ts_us;
            samples[n].value = s->value;
            samples[n].var_id = g_watch_ids[i];
            n++;
        }
    }
    if (n <= 0) return 0;
    enc = os_net_codec_encode_flat(samples, n, payload, sizeof(payload));
    if (enc < 0) return -1;
    return send_msg(c, OS_NET_MSG_SAMPLE_BATCH, 0, payload, (uint32_t)enc);
}

static void broadcast_samples(void)
{
    int i;
    EnterCriticalSection(&g_cli_cs);
    for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i]) push_samples_to(g_clients[i]);
    LeaveCriticalSection(&g_cli_cs);
}

static void broadcast_elf(void)
{
    OS_NetVar vars[512]; uint8_t payload[65536];
    int cnt = build_varlist(vars, 512);
    int enc = os_net_encode_varlist(vars, cnt, payload, sizeof(payload));
    int i;
    if (enc < 0) return;
    EnterCriticalSection(&g_cli_cs);
    for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i]) send_msg(g_clients[i], OS_NET_MSG_ELF_SYNC, 0, payload, (uint32_t)enc);
    LeaveCriticalSection(&g_cli_cs);
    if (g_fw) g_fw->log(OS_LOG_INFO, "network: 广播 ELF 变量表 %d 项", cnt);
}

static void handle_msg(OS_WSConn* c, const OS_NetFrame* f)
{
    uint8_t ack[256];
    int n;
    switch (f->type) {
    case OS_NET_MSG_HELLO: {
        OS_NetVar vars[512]; uint8_t payload[65536];
        int cnt = build_varlist(vars, 512);
        int enc = os_net_encode_varlist(vars, cnt, payload, sizeof(payload));
        if (enc >= 0) send_msg(c, OS_NET_MSG_ELF_SYNC, 0, payload, (uint32_t)enc);
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: HELLO -> 发送 ELF 变量表 %d 项", cnt);
        break;
    }
    case OS_NET_MSG_WATCH_LIST: {
        char names[256][OS_NET_NAME_MAX];
        int i, cnt = os_net_decode_names(f->payload, (int)f->len, names, 256);
        g_watch_count = 0;
        for (i = 0; i < cnt && g_watch_count < 256; i++) {
            int id = resolve_name_to_id(names[i]);
            if (id >= 0) g_watch_ids[g_watch_count++] = id;
        }
        n = os_net_encode_ack(0, "watch ok", ack, sizeof(ack));
        if (n >= 0) send_msg(c, OS_NET_MSG_ACK, 0, ack, (uint32_t)n);
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: WATCH_LIST %d 项 -> %d 个监视变量", cnt, g_watch_count);
        break;
    }
    case OS_NET_MSG_WRITE_VAR: {
        char name[OS_NET_NAME_MAX], text[64];
        int id, code = -1;
        if (os_net_decode_write(f->payload, (int)f->len, name, sizeof(name), text, sizeof(text)) == 0) {
            id = resolve_name_to_id(name);
            if (id >= 0 && g_fw && g_fw->write_leaf) {
                char err[128] = "";
                code = g_fw->write_leaf(id, atof(text), err, sizeof(err));
                if (g_fw) g_fw->log(OS_LOG_INFO, "network: WRITE_VAR %s=%s -> %d", name, text, code);
            }
        }
        n = os_net_encode_ack(code, code == 0 ? "ok" : "write failed", ack, sizeof(ack));
        if (n >= 0) send_msg(c, OS_NET_MSG_ACK, 0, ack, (uint32_t)n);
        break;
    }
    case OS_NET_MSG_SAMPLE_BATCH: {
        /* 远端样本 → 注入本地采集通道（波形/数值窗口显示） */
        OS_NetSample samples[256];
        int cnt = os_net_codec_decode_flat(f->payload, (int)f->len, samples, 256);
        int i;
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
        break;
    }
    case OS_NET_MSG_ELF_SYNC: {
        OS_NetVar vars[512];
        int cnt = os_net_decode_varlist(f->payload, (int)f->len, vars, 512);
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 收到 ELF 变量表 %d 项", cnt);
        break;
    }
    default:
        break;
    }
}

static void session_serve(OS_WSConn* c)
{
    uint8_t buf[65536];
    int op;
    while (g_running) {
        int n = os_ws_recv(c, buf, sizeof(buf), &op);
        if (n <= 0) break;
        if (op == OS_WS_OP_BIN && n >= OS_NET_FRAME_HDR) {
            OS_NetFrame f;
            if (os_net_frame_decode(buf, n, &f) > 0) handle_msg(c, &f);
        }
    }
    os_ws_close(c);
}

static DWORD WINAPI client_thread(LPVOID p)
{
    OS_WSConn* c = (OS_WSConn*)p;
    int i;
    session_serve(c);
    EnterCriticalSection(&g_cli_cs);
    for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i] == c) { g_clients[i] = NULL; break; }
    LeaveCriticalSection(&g_cli_cs);
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
        for (i = 0; i < NET_MAX_CLIENTS; i++) if (!g_clients[i]) { g_clients[i] = c; slot = i; break; }
        LeaveCriticalSection(&g_cli_cs);
        if (slot < 0) { os_ws_close(c); continue; }
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 客户端接入（第 %d 个）", slot + 1);
        { HANDLE th = CreateThread(NULL, 0, client_thread, c, 0, NULL); if (th) CloseHandle(th); }
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
        OS_WSConn* c;
        if (in) memcpy(&g_cfg, in, sizeof(g_cfg));
        c = os_ws_connect(g_cfg.ip, g_cfg.port);
        if (!c) { if (g_fw) g_fw->log(OS_LOG_ERROR, "network: 连接失败 %s:%d", g_cfg.ip, g_cfg.port); return OS_ERR_FAIL; }
        { uint8_t z = 0; send_msg(c, OS_NET_MSG_HELLO, 0, &z, 0); }
        { HANDLE th = CreateThread(NULL, 0, client_thread, c, 0, NULL); if (th) CloseHandle(th); }
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 已连接 %s:%d", g_cfg.ip, g_cfg.port);
        return OS_ERR_OK;
    }
    case OS_CMD_NET_STOP:
        g_running = 0;
        if (g_listen >= 0) { os_ws_close_listen(g_listen); g_listen = -1; }
        { int i; EnterCriticalSection(&g_cli_cs); for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i]) { os_ws_close(g_clients[i]); g_clients[i] = NULL; } LeaveCriticalSection(&g_cli_cs); }
        if (g_thread) { WaitForSingleObject(g_thread, 2000); CloseHandle(g_thread); g_thread = NULL; }
        if (g_fw) g_fw->log(OS_LOG_INFO, "network: 已停止");
        return OS_ERR_OK;
    case OS_CMD_NET_SYNC_ELF:
        broadcast_elf();
        return OS_ERR_OK;
    case OS_CMD_NET_PUSH:
        broadcast_samples();
        return OS_ERR_OK;
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
    memset(&g_info, 0, sizeof(g_info));
    _snprintf(g_info.name, sizeof(g_info.name), "%s", "network");
    _snprintf(g_info.version, sizeof(g_info.version), "%s", "0.2.0");
    _snprintf(g_info.dll_version, sizeof(g_info.dll_version), "%s", "ws-rfc6455");
    _snprintf(g_info.emulator, sizeof(g_info.emulator), "%s", "未连接");
    if (fw) fw->log(OS_LOG_INFO, "network 模块: 已初始化（WebSocket + 协议会话 + 一对多 fan-out 就绪）");
    return OS_ERR_OK;
}

static void mod_deinit(void* ctx)
{
    (void)ctx;
    g_running = 0;
    { int i; EnterCriticalSection(&g_cli_cs); for (i = 0; i < NET_MAX_CLIENTS; i++) if (g_clients[i]) { os_ws_close(g_clients[i]); g_clients[i] = NULL; } LeaveCriticalSection(&g_cli_cs); }
    if (g_listen >= 0) { os_ws_close_listen(g_listen); g_listen = -1; }
    if (g_thread) { WaitForSingleObject(g_thread, 2000); CloseHandle(g_thread); g_thread = NULL; }
    DeleteCriticalSection(&g_cli_cs);
}

static const OS_Module g_module = {
    OS_API_VERSION,
    OS_CAP_NET,
    "network",
    "0.2.0",
    "网络远程操作模块：WebSocket 传输 + 协议会话 + 一对多 fan-out",
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
