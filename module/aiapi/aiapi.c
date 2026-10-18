/*
 * OpenScope AI 接口模块（aiapi.dll）
 *
 * 能力：OS_CAP_AI。在 127.0.0.1 上监听 WebSocket，承接 JSON-RPC 2.0 /
 * MCP 风格 tools/list、tools/call，转发到框架 fw->ai_call。
 *
 * 默认端口 18765；可用环境变量 OPENSCOPE_AI_PORT 或启动参数 --ai-port= 覆盖
 * （后者由主程序写入环境变量后再加载模块）。
 *
 * 新模块自动出现在 modules.list / tools.list（宿主扫描 + OS_CMD_AI_DESCRIBE）。
 */
#include "module_api.h"
#include "netws.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AI_DEFAULT_PORT 18765
#define AI_RECV_CAP     (256 * 1024)
#define AI_RESULT_CAP   (256 * 1024)
#define AI_MAX_CLIENTS  8

static const OS_Framework* g_fw;
static volatile LONG g_running;
static HANDLE g_thread;
static int g_listen = -1;
static int g_port = AI_DEFAULT_PORT;

typedef struct AiClient {
    OS_WSConn*    conn;
    volatile LONG subscribed;
    int           min_level; /* OS_LOG_* 严重度下限 */
    int           listening; /* 已注册 fw log listener */
} AiClient;

static int log_severity(int level)
{
    switch (level) {
    case OS_LOG_ERROR: return 3;
    case OS_LOG_WARN:  return 2;
    case OS_LOG_DEBUG: return 0;
    default:           return 1;
    }
}

static void json_escape_text(const char* s, char* out, int cap)
{
    int i = 0;
    if (!out || cap < 1) return;
    if (!s) { out[0] = 0; return; }
    while (*s && i < cap - 2) {
        if (*s == '"' || *s == '\\') { out[i++] = '\\'; out[i++] = *s++; }
        else if ((unsigned char)*s < 0x20) { s++; }
        else out[i++] = *s++;
    }
    out[i] = 0;
}

static const char* log_level_name(int level)
{
    switch (level) {
    case OS_LOG_WARN:  return "WARN";
    case OS_LOG_ERROR: return "ERROR";
    case OS_LOG_DEBUG: return "DEBUG";
    default:           return "INFO";
    }
}

/* 宿主日志回调 → WebSocket JSON-RPC 通知（无 id） */
static void ai_on_log(const OS_AiLogEntry* e, void* user)
{
    AiClient* cl = (AiClient*)user;
    char esc[640], msg[900];
    if (!cl || !e || !cl->conn) return;
    if (InterlockedCompareExchange(&cl->subscribed, 1, 1) != 1) return;
    if (log_severity(e->level) < log_severity(cl->min_level)) return;
    json_escape_text(e->text, esc, sizeof(esc));
    _snprintf(msg, sizeof(msg),
        "{\"jsonrpc\":\"2.0\",\"method\":\"log.event\",\"params\":"
        "{\"seq\":%llu,\"level\":%d,\"level_name\":\"%s\",\"ts_us\":%lld,\"text\":\"%s\"}}",
        (unsigned long long)e->seq, e->level, log_level_name(e->level),
        (long long)e->ts_us, esc);
    os_ws_send_text(cl->conn, msg);
}

static int json_extract_str(const char* j, const char* key, char* out, int cap)
{
    char pat[96];
    const char* p;
    int i = 0, klen;
    if (!j || !key || !out || cap < 2) return -1;
    klen = (int)strlen(key);
    if (klen <= 0 || klen > 80) return -1;
    _snprintf(pat, sizeof(pat), "\"%s\"", key);
    p = strstr(j, pat);
    while (p) {
        const char* q = p + klen + 2;
        while (*q == ' ' || *q == '\t') q++;
        if (*q == ':') {
            q++;
            while (*q == ' ' || *q == '\t') q++;
            if (*q != '"') return -1;
            q++;
            while (*q && *q != '"' && i < cap - 1) {
                if (*q == '\\' && q[1]) { q++; out[i++] = *q++; }
                else out[i++] = *q++;
            }
            out[i] = 0;
            return (*q == '"') ? 0 : -1;
        }
        p = strstr(p + 1, pat);
    }
    return -1;
}

static int json_extract_id(const char* j, char* out, int cap)
{
    const char* p = strstr(j, "\"id\"");
    int i = 0;
    if (!p || !out || cap < 2) { if (out && cap) out[0] = 0; return -1; }
    p += 4;
    while (*p == ' ' || *p == '\t') p++;
    if (*p != ':') return -1;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '"') {
        p++;
        while (*p && *p != '"' && i < cap - 1) out[i++] = *p++;
        out[i] = 0;
        return 0;
    }
    if (*p == 'n' && strncmp(p, "null", 4) == 0) { out[0] = 0; return 0; }
    while ((*p >= '0' && *p <= '9') || *p == '-') {
        if (i >= cap - 1) break;
        out[i++] = *p++;
    }
    out[i] = 0;
    return i > 0 ? 0 : -1;
}

/* 提取 params 对象原文（从 "params": 后第一个 { 到匹配 }） */
static int json_extract_params_obj(const char* j, char* out, int cap)
{
    const char* p = strstr(j, "\"params\"");
    int depth, i = 0;
    if (!out || cap < 3) return -1;
    out[0] = '{'; out[1] = '}'; out[2] = 0;
    if (!p) return 0;
    p += 8;
    while (*p && *p != '{' && *p != '"' && *p != 'n') p++;
    if (*p == 'n') return 0; /* null */
    if (*p == '"') {
        /* arguments 可能是字符串化 JSON——少见，忽略 */
        return 0;
    }
    if (*p != '{') return 0;
    depth = 0;
    while (*p && i < cap - 1) {
        char c = *p++;
        out[i++] = c;
        if (c == '{') depth++;
        else if (c == '}') {
            depth--;
            if (depth == 0) break;
        } else if (c == '"') {
            while (*p && i < cap - 1) {
                out[i++] = *p;
                if (*p == '\\' && p[1]) { p++; out[i++] = *p++; }
                else { p++; if (p[-1] == '"') break; }
            }
        }
    }
    out[i] = 0;
    return 0;
}

static void send_rpc_result(OS_WSConn* c, const char* id, const char* result_json)
{
    char* msg;
    int need, id_is_num = 1, i;
    if (!c || !result_json) return;
    if (!id || !id[0]) {
        /* notification — 不回包 */
        return;
    }
    for (i = 0; id[i]; i++) {
        if (!((id[i] >= '0' && id[i] <= '9') || id[i] == '-')) { id_is_num = 0; break; }
    }
    need = (int)strlen(result_json) + 128;
    msg = (char*)malloc((size_t)need);
    if (!msg) return;
    if (id_is_num)
        _snprintf(msg, need, "{\"jsonrpc\":\"2.0\",\"id\":%s,\"result\":%s}", id, result_json);
    else
        _snprintf(msg, need, "{\"jsonrpc\":\"2.0\",\"id\":\"%s\",\"result\":%s}", id, result_json);
    os_ws_send_text(c, msg);
    free(msg);
}

static void send_rpc_error(OS_WSConn* c, const char* id, int code, const char* message)
{
    char msg[512];
    int id_is_num = 1, i;
    if (!c) return;
    if (!id || !id[0]) return;
    for (i = 0; id[i]; i++) {
        if (!((id[i] >= '0' && id[i] <= '9') || id[i] == '-')) { id_is_num = 0; break; }
    }
    if (id_is_num)
        _snprintf(msg, sizeof(msg),
            "{\"jsonrpc\":\"2.0\",\"id\":%s,\"error\":{\"code\":%d,\"message\":\"%s\"}}",
            id, code, message ? message : "error");
    else
        _snprintf(msg, sizeof(msg),
            "{\"jsonrpc\":\"2.0\",\"id\":\"%s\",\"error\":{\"code\":%d,\"message\":\"%s\"}}",
            id, code, message ? message : "error");
    os_ws_send_text(c, msg);
}

static void handle_request(AiClient* cl, const char* req)
{
    OS_WSConn* c;
    char method[128], id[64], params[65536], result[AI_RESULT_CAP];
    char name[128], args[65536];
    int rc;

    if (!cl || !cl->conn) return;
    c = cl->conn;
    if (!g_fw || !g_fw->ai_call) {
        send_rpc_error(c, "null", -32000, "ai_call unavailable");
        return;
    }
    id[0] = 0;
    json_extract_id(req, id, sizeof(id));
    if (json_extract_str(req, "method", method, sizeof(method)) != 0) {
        send_rpc_error(c, id[0] ? id : "null", -32600, "method required");
        return;
    }
    json_extract_params_obj(req, params, sizeof(params));

    /* 实时日志订阅：本连接注册宿主 listener，推送 method=log.event */
    if (strcmp(method, "log.subscribe") == 0) {
        int ml = OS_LOG_DEBUG, v;
        const char* p = strstr(params, "\"min_level\"");
        if (p) {
            p = strchr(p, ':');
            if (p) ml = atoi(p + 1);
        }
        (void)v;
        cl->min_level = ml;
        InterlockedExchange(&cl->subscribed, 1);
        if (!cl->listening && g_fw->log_add_listener) {
            if (g_fw->log_add_listener(ai_on_log, cl) == 0)
                cl->listening = 1;
        }
        _snprintf(result, sizeof(result),
            "{\"ok\":true,\"subscribed\":1,\"min_level\":%d,\"head_seq\":%llu}",
            cl->min_level,
            (unsigned long long)(g_fw->log_ring_head_seq ? g_fw->log_ring_head_seq() : 0));
        send_rpc_result(c, id, result);
        return;
    }
    if (strcmp(method, "log.unsubscribe") == 0) {
        InterlockedExchange(&cl->subscribed, 0);
        if (cl->listening && g_fw->log_remove_listener) {
            g_fw->log_remove_listener(ai_on_log, cl);
            cl->listening = 0;
        }
        send_rpc_result(c, id, "{\"ok\":true,\"subscribed\":0}");
        return;
    }

    /* MCP: tools/list → tools.list；tools/call → 真实工具名 */
    if (strcmp(method, "tools/list") == 0) {
        rc = g_fw->ai_call("tools.list", "{}", result, sizeof(result));
        if (rc == OS_ERR_OK) send_rpc_result(c, id, result);
        else send_rpc_error(c, id, -32000, result[0] ? result : "tools.list failed");
        return;
    }
    if (strcmp(method, "tools/call") == 0) {
        if (json_extract_str(params, "name", name, sizeof(name)) != 0) {
            /* 有的客户端把 name 放顶层 params */
            if (json_extract_str(req, "name", name, sizeof(name)) != 0) {
                send_rpc_error(c, id, -32602, "tools/call needs name");
                return;
            }
        }
        /* arguments 对象 */
        {
            const char* ap = strstr(params, "\"arguments\"");
            if (!ap) ap = strstr(req, "\"arguments\"");
            if (ap) {
                /* 借用 extract：临时把 key 换成假的再解析困难——直接扫 { */
                const char* p = ap;
                int depth = 0, i = 0;
                while (*p && *p != '{') p++;
                if (*p == '{') {
                    while (*p && i < (int)sizeof(args) - 1) {
                        char ch = *p++;
                        args[i++] = ch;
                        if (ch == '{') depth++;
                        else if (ch == '}') {
                            depth--;
                            if (depth == 0) break;
                        } else if (ch == '"') {
                            while (*p && i < (int)sizeof(args) - 1) {
                                args[i++] = *p;
                                if (*p == '\\' && p[1]) { p++; args[i++] = *p++; }
                                else { p++; if (p[-1] == '"') break; }
                            }
                        }
                    }
                    args[i] = 0;
                } else _snprintf(args, sizeof(args), "{}");
            } else _snprintf(args, sizeof(args), "%s", params);
        }
        rc = g_fw->ai_call(name, args, result, sizeof(result));
        if (rc == OS_ERR_OK) {
            /* MCP tools/call 包装：content 文本 + 结构化 result */
            char* wrap = (char*)malloc(strlen(result) + 128);
            if (wrap) {
                _snprintf(wrap, strlen(result) + 128,
                    "{\"content\":[{\"type\":\"text\",\"text\":%s}],\"isError\":false,\"structuredContent\":%s}",
                    result, result);
                /* text 字段需要是 JSON 字符串——上面直接塞对象不合法。
                 * 简化：只返回 structured 兼容字段 + 原始 result 作为 result。 */
                free(wrap);
            }
            send_rpc_result(c, id, result);
        } else {
            send_rpc_error(c, id, -32000, result[0] ? result : "tool failed");
        }
        return;
    }
    if (strcmp(method, "initialize") == 0) {
        _snprintf(result, sizeof(result),
            "{\"protocolVersion\":\"2024-11-05\",\"capabilities\":{\"tools\":{}},"
            "\"serverInfo\":{\"name\":\"OpenScope\",\"version\":\"aiapi\"}}");
        send_rpc_result(c, id, result);
        return;
    }
    if (strcmp(method, "notifications/initialized") == 0 ||
        strcmp(method, "ping") == 0) {
        if (id[0]) send_rpc_result(c, id, "{\"ok\":true}");
        return;
    }

    rc = g_fw->ai_call(method, params, result, sizeof(result));
    if (rc == OS_ERR_OK) send_rpc_result(c, id, result);
    else send_rpc_error(c, id, -32000, result[0] ? result : "call failed");
}

static DWORD WINAPI client_thread(LPVOID p)
{
    AiClient cl;
    uint8_t* buf;
    memset(&cl, 0, sizeof(cl));
    cl.conn = (OS_WSConn*)p;
    cl.min_level = OS_LOG_DEBUG;
    buf = (uint8_t*)malloc(AI_RECV_CAP);
    if (!buf) { os_ws_close(cl.conn); return 0; }
    while (InterlockedCompareExchange(&g_running, 1, 1) == 1) {
        int op = 0, n = os_ws_recv(cl.conn, buf, AI_RECV_CAP - 1, &op);
        if (n <= 0) break;
        if (op != OS_WS_OP_TEXT && op != OS_WS_OP_BIN) continue;
        buf[n] = 0;
        handle_request(&cl, (const char*)buf);
    }
    InterlockedExchange(&cl.subscribed, 0);
    if (cl.listening && g_fw && g_fw->log_remove_listener)
        g_fw->log_remove_listener(ai_on_log, &cl);
    free(buf);
    os_ws_close(cl.conn);
    return 0;
}

static DWORD WINAPI listen_thread(LPVOID p)
{
    (void)p;
    if (g_fw) g_fw->log(OS_LOG_INFO, "aiapi: WebSocket JSON-RPC 监听 127.0.0.1:%d", g_port);
    while (InterlockedCompareExchange(&g_running, 1, 1) == 1) {
        OS_WSConn* c = os_ws_accept(g_listen);
        HANDLE th;
        if (!c) {
            if (InterlockedCompareExchange(&g_running, 1, 1) != 1) break;
            Sleep(50);
            continue;
        }
        th = CreateThread(NULL, 0, client_thread, c, 0, NULL);
        if (th) CloseHandle(th);
        else os_ws_close(c);
    }
    return 0;
}

static int ai_start(int port)
{
    if (g_running) return OS_ERR_BUSY;
    g_port = port > 0 ? port : AI_DEFAULT_PORT;
    g_listen = os_ws_listen("127.0.0.1", g_port);
    if (g_listen < 0) {
        if (g_fw) g_fw->log(OS_LOG_ERROR, "aiapi: 监听 127.0.0.1:%d 失败", g_port);
        return OS_ERR_FAIL;
    }
    InterlockedExchange(&g_running, 1);
    g_thread = CreateThread(NULL, 0, listen_thread, NULL, 0, NULL);
    if (!g_thread) {
        InterlockedExchange(&g_running, 0);
        os_ws_close_listen(g_listen);
        g_listen = -1;
        return OS_ERR_FAIL;
    }
    return OS_ERR_OK;
}

static void ai_stop(void)
{
    InterlockedExchange(&g_running, 0);
    if (g_listen >= 0) {
        os_ws_close_listen(g_listen);
        g_listen = -1;
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = NULL;
    }
}

static int ai_init(const OS_Framework* fw, void** out_ctx)
{
    char env[32];
    int port = AI_DEFAULT_PORT;
    g_fw = fw;
    *out_ctx = (void*)1;
    if (GetEnvironmentVariableA("OPENSCOPE_AI_PORT", env, sizeof(env)) > 0)
        port = atoi(env);
    if (fw && fw->api_version < 5) {
        if (fw->log) fw->log(OS_LOG_WARN, "aiapi: 需要 API v5（ai_call），当前 %d", fw->api_version);
        return OS_ERR_FAIL;
    }
    if (!fw || !fw->ai_call) {
        if (fw && fw->log) fw->log(OS_LOG_ERROR, "aiapi: 框架未提供 ai_call");
        return OS_ERR_FAIL;
    }
    return ai_start(port);
}

static void ai_deinit(void* ctx)
{
    (void)ctx;
    ai_stop();
    g_fw = NULL;
}

static int ai_command(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx;
    (void)in;
    if (cmd == OS_CMD_AI_DESCRIBE && out) {
        /* 自身工具已由宿主 tools.list 覆盖；此处返回空片段 */
        char* buf = (char*)out;
        buf[0] = 0;
        return 0;
    }
    if (cmd == OS_CMD_GET_INFO && out) {
        OS_DriverInfo* info = (OS_DriverInfo*)out;
        memset(info, 0, sizeof(*info));
        _snprintf(info->name, sizeof(info->name), "aiapi");
        _snprintf(info->version, sizeof(info->version), "1.0");
        info->connected = g_running ? 1 : 0;
        return OS_ERR_OK;
    }
    return OS_ERR_FAIL;
}

static const OS_Module g_mod = {
    OS_API_VERSION,
    OS_CAP_AI,
    "aiapi",
    "1.1.0",
    "AI/CLI/MCP 本机 WebSocket JSON-RPC + 实时日志（127.0.0.1:18765）",
    NULL,
    ai_init,
    ai_deinit,
    ai_command,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

OS_MODULE_EXPORT const OS_Module* os_module_get(void)
{
    return &g_mod;
}
