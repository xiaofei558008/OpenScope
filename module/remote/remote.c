/*
 * OpenScope 远程调试服务端模块（remote.dll）—— ORP v1（docs/remote_debug_design.md v0.4）
 *
 * 能力：OS_CAP_AI（与 aiapi 相同，经 fw->ai_call 调宿主；退出时先于驱动卸载）。
 * 复用 aiapi 的传输实现（netws.c：握手在客户端线程内并带超时、超大请求丢弃不断线、连接名额原子计数）。
 *
 * 配置（环境变量；主程序命令行 --remote-* 写入同名变量）：
 *   OPENSCOPE_REMOTE_PORT            监听端口，默认 18766（--remote-port=）
 *   OPENSCOPE_REMOTE_HOST            127.0.0.1（默认）或 0.0.0.0（--remote-host=）
 *   OPENSCOPE_REMOTE_DISABLE=1       不启动服务（--remote-off）
 *   OPENSCOPE_REMOTE_AUTO_ELEVATE=1  仅测试：auth.elevate 不弹窗自动批准（仅 127.0.0.1 生效）
 *   OPENSCOPE_REMOTE_PAIR_CODE=NNNNNN 仅测试：固定配对码（可重复使用）
 *
 * 鉴权：任何监听地址都必须先 pair.claim（6 位配对码，打印在日志栏）拿 token，再 session.hello。
 */
#include <winsock2.h>
#include <ws2tcpip.h>
#include "module_api.h"
#include "netws.h"
#include <mmsystem.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ws2_32.lib")

#define R_DEFAULT_PORT   18766
#define R_RECV_CAP       (256 * 1024)
#define R_RESULT_CAP     (256 * 1024)
#define R_MAX_CLIENTS    8
#define R_MAX_PENDING    16
#define R_HANDSHAKE_MS   5000
#define R_DRAIN_MAX      (64u * 1024u * 1024u)
#define R_MAX_FRAME      65536
#define R_HB_INTERVAL_S  10
#define R_HB_TIMEOUT_US  (3LL * R_HB_INTERVAL_S * 1000000LL)
#define R_SESSION_KEEP_US (120LL * 1000000LL)
#define R_CONTROL_GRACE_US (15LL * 1000000LL)
#define R_PAIR_TTL_US    (300LL * 1000000LL)
#define R_MAX_SESS       32
#define R_MAX_TOKENS     64
#define R_MAX_SUBS       8
#define R_SUB_VARS       16
#define R_MAX_TRK        16
#define R_RING           65536
#define R_REQ_CACHE      16
#define R_ELEVATE_TIMEOUT_MS 30000
#define S_TMP            70000

enum { ROLE_VIEWER = 0, ROLE_OPERATOR = 1, ROLE_ADMIN = 2 };
enum { E_PARSE = -32700, E_INVREQ = -32600, E_NOMETHOD = -32601, E_PARAMS = -32602,
       E_SRVFAIL = -32000, E_TOOMANY = -32001, E_UNAUTH = -32002, E_FORBIDDEN = -32003,
       E_LOCKED = -32004, E_OFFLINE = -32005, E_EXPIRED = -32006 };

/* 连接引用计数：客户端线程、会话、推流线程各持一份引用，最后一个释放者关闭连接 */
typedef struct RConn { OS_WSConn* ws; volatile LONG refs; } RConn;
static void rc_ref(RConn* c) { if (c) InterlockedIncrement(&c->refs); }
static void rc_unref(RConn* c) { if (c && InterlockedDecrement(&c->refs) == 0) { os_ws_close(c->ws); free(c); } }

typedef struct Sub {
    int     used, ready;            /* ready=0：meta 未发出，推流跳过（保证 meta 先于帧） */
    int     id, mode, kind, f64;    /* mode 0 latest / 1 minmax / 2 raw */
    double  rate_hz;
    int     nvars, vars[R_SUB_VARS];
    int64_t next_us, last_v[R_SUB_VARS];
} Sub;

typedef struct ReqCache { char rid[72]; char res[256]; } ReqCache;

typedef struct Sess {
    int      used;
    char     sid[20];
    int      tok, role;
    char     client[8];
    int      accept;                 /* bit1 = kind1, bit2 = kind2 */
    RConn*   conn;
    int64_t  last_rx, detached_at;
    uint32_t seq;
    Sub      subs[R_MAX_SUBS];
    ReqCache rc[R_REQ_CACHE];
    int      rc_next;
} Sess;

typedef struct Token { int used; char tok[48]; int role; char client[8]; } Token;

typedef struct Trk { int used, id, refs; int64_t idle_since; int64_t* ts; double* v; int head, count; int64_t last_ts; } Trk;

typedef struct VarInfo { int id; char name[256]; char addr[24]; int size; char type[24]; int writable; } VarInfo;

typedef struct Rec { int vid; int64_t ts; double a, b, c; } Rec;
typedef struct Out { RConn* conn; uint8_t* data; int len; struct Out* next; } Out;

static const OS_Framework* g_fw;
static volatile LONG g_running, g_clients;
static HANDLE g_thread, g_pump;
static int g_listen = -1, g_port = R_DEFAULT_PORT, g_auto_elevate;
static char g_host[32] = "127.0.0.1", g_fixed_code[8], g_code[8], g_device_id[24];
static int64_t g_code_exp;
static CRITICAL_SECTION g_cs; static int g_cs_ready;
static Sess  g_sess[R_MAX_SESS];
static Token g_tok[R_MAX_TOKENS];
static Trk   g_trk[R_MAX_TRK];
static int   g_control = -1; static int64_t g_control_since;
static int   g_next_stream = 1;
static VarInfo* g_vars; static int g_nvars, g_vars_cap; static int64_t g_vars_at;
static RConn* g_live[R_MAX_CLIENTS + R_MAX_PENDING];
static int64_t* s_ts; static double* s_v; static Rec* s_rec;   /* 仅推流线程使用 */

static int64_t now_us(void)
{
    FILETIME ft; ULARGE_INTEGER u;
    GetSystemTimeAsFileTime(&ft);
    u.LowPart = ft.dwLowDateTime; u.HighPart = ft.dwHighDateTime;
    return (int64_t)((u.QuadPart - 116444736000000000ULL) / 10);
}

static void rlog2(int lv, const char* fmt, const char* a, const char* b)
{
    if (g_fw && g_fw->log) g_fw->log(lv, fmt, a ? a : "", b ? b : "");
}

static void rand_bytes(uint8_t* p, int n)
{
    typedef BOOLEAN (WINAPI *GenFn)(PVOID, ULONG);   /* RtlGenRandom */
    static GenFn fn;
    if (!fn) { HMODULE h = LoadLibraryA("advapi32.dll"); if (h) fn = (GenFn)GetProcAddress(h, "SystemFunction036"); }
    if (!fn || !fn(p, (ULONG)n)) { int i; for (i = 0; i < n; i++) p[i] = (uint8_t)(rand() ^ (GetTickCount() >> (i & 7))); }
}

static void rand_hex(char* out, int nbytes)
{
    static const char* H = "0123456789abcdef";
    uint8_t b[32]; int i;
    rand_bytes(b, nbytes);
    for (i = 0; i < nbytes; i++) { out[2*i] = H[b[i] >> 4]; out[2*i+1] = H[b[i] & 15]; }
    out[2*nbytes] = 0;
}

/* ---------------- 迷你 JSON 读取 ---------------- */
static const char* js_ws(const char* p) { while (p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) p++; return p; }

static const char* js_skip(const char* p)
{
    p = js_ws(p);
    if (!p || !*p) return NULL;
    if (*p == '"') {
        p++;
        while (*p && *p != '"') { if (*p == '\\' && p[1]) p++; p++; }
        return *p ? p + 1 : NULL;
    }
    if (*p == '{' || *p == '[') {
        int depth = 0;
        while (*p) {
            if (*p == '"') { p = js_skip(p); if (!p) return NULL; continue; }
            if (*p == '{' || *p == '[') depth++;
            else if (*p == '}' || *p == ']') { depth--; if (depth == 0) return p + 1; }
            p++;
        }
        return NULL;
    }
    while (*p && *p != ',' && *p != '}' && *p != ']' && *p != ' ' && *p != '\r' && *p != '\n' && *p != '\t') p++;
    return p;
}

static const char* js_get(const char* obj, const char* key)   /* obj 指向 '{'，返回顶层 key 的值 */
{
    const char* p = js_ws(obj);
    size_t kl = strlen(key);
    if (!p || *p != '{') return NULL;
    p++;
    for (;;) {
        const char* ks; const char* ke;
        p = js_ws(p);
        if (!p || *p != '"') return NULL;
        ks = p + 1; ke = js_skip(p); if (!ke) return NULL;
        p = js_ws(ke);
        if (*p != ':') return NULL;
        p = js_ws(p + 1);
        if ((size_t)(ke - 1 - ks) == kl && strncmp(ks, key, kl) == 0) return p;
        p = js_skip(p); if (!p) return NULL;
        p = js_ws(p);
        if (*p == ',') { p++; continue; }
        return NULL;
    }
}

static int js_str(const char* v, char* out, int cap)
{
    int i = 0;
    if (!v || *v != '"' || cap < 1) return -1;
    v++;
    while (*v && *v != '"' && i < cap - 1) {
        if (*v == '\\' && v[1]) {
            v++;
            out[i++] = (*v == 'n') ? '\n' : (*v == 't') ? '\t' : *v;
            v++;
        } else out[i++] = *v++;
    }
    out[i] = 0;
    return *v == '"' ? 0 : -1;
}

static int js_num(const char* v, double* d)
{
    char* e;
    if (!v) return -1;
    if (*v == '"') { char b[64]; if (js_str(v, b, sizeof(b))) return -1; *d = strtod(b, &e); return (e != b && *e == 0) ? 0 : -1; }
    if (*v == 't' && strncmp(v, "true", 4) == 0) { *d = 1; return 0; }
    if (*v == 'f' && strncmp(v, "false", 5) == 0) { *d = 0; return 0; }
    *d = strtod(v, &e);
    return e != v ? 0 : -1;
}

static int js_getstr(const char* o, const char* k, char* out, int cap) { const char* v = js_get(o, k); return v ? js_str(v, out, cap) : -1; }
static int js_getnum(const char* o, const char* k, double* d) { const char* v = js_get(o, k); return (v && *v != 'n') ? js_num(v, d) : -1; }

static void js_esc(const char* s, char* out, int cap)
{
    int i = 0;
    if (cap < 1) return;
    while (s && *s && i < cap - 7) {
        unsigned char ch = (unsigned char)*s++;
        if (ch == '"' || ch == '\\') { out[i++] = '\\'; out[i++] = (char)ch; }
        else if (ch < 0x20) i += _snprintf(out + i, cap - i, "\\u%04x", ch);
        else out[i++] = (char)ch;
    }
    out[i] = 0;
}

/* ---------------- 宿主调用 ---------------- */
static int host(const char* m, const char* params, char* out, int cap)
{
    out[0] = 0;
    if (!g_fw || !g_fw->ai_call) { _snprintf(out, cap, "{\"ok\":false,\"message\":\"no host\"}"); return OS_ERR_FAIL; }
    return g_fw->ai_call(m, params ? params : "{}", out, cap);
}

static void host_msg(const char* out, char* msg, int cap)
{
    if (js_getstr(out, "message", msg, cap) != 0) _snprintf(msg, cap, "host call failed");
}

/* ---------------- 发送 ---------------- */
typedef struct Ctx { RConn* conn; int sess; char rid[80]; } Ctx;   /* rid：id 原文（数字或带引号字符串），空=通知 */

static void send_text(RConn* c, const char* s) { if (c) os_ws_send_text(c->ws, s); }

static void reply_raw(Ctx* x, const char* result)
{
    char* m; size_t n;
    if (!x->rid[0]) return;
    n = strlen(result) + strlen(x->rid) + 64;
    m = (char*)malloc(n);
    if (!m) return;
    _snprintf(m, n, "{\"jsonrpc\":\"2.0\",\"id\":%s,\"result\":%s}", x->rid, result);
    send_text(x->conn, m);
    free(m);
}

static void reply_err(Ctx* x, int code, const char* msg, const char* data_json)
{
    char m[1400], esc[600];
    js_esc(msg, esc, sizeof(esc));
    _snprintf(m, sizeof(m), "{\"jsonrpc\":\"2.0\",\"id\":%s,\"error\":{\"code\":%d,\"message\":\"%s\"%s%s}}",
              x->rid[0] ? x->rid : "null", code, esc, data_json ? ",\"data\":" : "", data_json ? data_json : "");
    if (x->rid[0] || code == E_PARSE || code == E_INVREQ) send_text(x->conn, m);
}

/* ---------------- 变量表（经宿主 vars.list 分页拉取，2s 缓存；调用方持 g_cs） ---------------- */
static int vars_refresh(int force)
{
    char* out; int off = 0, total = -1, n = 0;
    int64_t t = now_us();
    if (!force && g_vars && t - g_vars_at < 2000000) return g_nvars;
    out = (char*)malloc(R_RESULT_CAP);
    if (!out) return g_nvars;
    for (;;) {
        char prm[64]; const char* arr; const char* p; double d; int got = 0;
        _snprintf(prm, sizeof(prm), "{\"offset\":%d,\"limit\":500}", off);
        if (host("vars.list", prm, out, R_RESULT_CAP) != OS_ERR_OK) break;
        if (js_getnum(out, "total", &d) == 0) total = (int)d;
        if (total > g_vars_cap) {
            VarInfo* nv = (VarInfo*)realloc(g_vars, sizeof(VarInfo) * (size_t)total);
            if (!nv) break;
            g_vars = nv; g_vars_cap = total;
        }
        arr = js_get(out, "vars");
        if (!arr || *arr != '[') break;
        p = js_ws(arr + 1);
        while (p && *p == '{' && n < g_vars_cap) {
            VarInfo* v = &g_vars[n];
            memset(v, 0, sizeof(*v));
            v->id = js_getnum(p, "id", &d) == 0 ? (int)d : -1;
            js_getstr(p, "name", v->name, sizeof(v->name));
            js_getstr(p, "addr", v->addr, sizeof(v->addr));
            v->size = js_getnum(p, "size", &d) == 0 ? (int)d : 0;
            if (js_getstr(p, "type", v->type, sizeof(v->type)) != 0) strcpy(v->type, "other");
            v->writable = js_getnum(p, "writable", &d) == 0 ? (int)d : 1;
            n++; off++; got++;
            p = js_skip(p); p = js_ws(p);
            if (p && *p == ',') p = js_ws(p + 1); else break;
        }
        if (total < 0 || off >= total || !got) break;
    }
    free(out);
    g_nvars = n; g_vars_at = t;
    return n;
}

static VarInfo* var_by_id(int id)
{
    int i;
    if (id >= 0 && id < g_nvars && g_vars[id].id == id) return &g_vars[id];
    for (i = 0; i < g_nvars; i++) if (g_vars[i].id == id) return &g_vars[i];
    return NULL;
}

static VarInfo* var_by_name(const char* nm)
{
    int i;
    for (i = 0; i < g_nvars; i++) if (strcmp(g_vars[i].name, nm) == 0) return &g_vars[i];
    return NULL;
}

static VarInfo* var_from_json(const char* v)   /* id 数字或名称字符串 */
{
    double d; char nm[256];
    if (!v) return NULL;
    if (*v == '"') { if (js_str(v, nm, sizeof(nm))) return NULL; return var_by_name(nm); }
    if (js_num(v, &d) == 0) return var_by_id((int)d);
    return NULL;
}

static VarInfo* var_from_params(const char* prm)
{
    const char* v = js_get(prm, "id");
    char nm[256];
    if (v && *v != 'n') return var_from_json(v);
    if (js_getstr(prm, "name", nm, sizeof(nm)) == 0) return var_by_name(nm);
    return NULL;
}

/* 解析 vars:[id|name,...]；返回个数，未知变量返回 -1（持 g_cs） */
static int parse_var_list(const char* va, int* vids, int max)
{
    const char* p; int nv = 0;
    if (!va || *va != '[') return 0;
    p = js_ws(va + 1);
    while (p && *p && *p != ']') {
        VarInfo* v = var_from_json(p);
        if (!v) return -1;
        if (nv < max) vids[nv++] = v->id;
        p = js_skip(p); p = js_ws(p); if (p && *p == ',') p = js_ws(p + 1);
    }
    return nv;
}

/* ---------------- 控制权结构（持 g_cs） ---------------- */
static void control_json(int me, char* out, int cap)
{
    Sess* h = (g_control >= 0 && g_sess[g_control].used) ? &g_sess[g_control] : NULL;
    if (!h) { _snprintf(out, cap, "{\"held_by_me\":false,\"holder\":null,\"since\":null}"); return; }
    _snprintf(out, cap, "{\"held_by_me\":%s,\"holder\":{\"session_id\":\"%s\",\"client\":\"%s\"},\"since\":%lld}",
              g_control == me ? "true" : "false", h->sid, h->client, (long long)g_control_since);
}

static void holder_json(char* out, int cap)
{
    Sess* h = (g_control >= 0 && g_sess[g_control].used) ? &g_sess[g_control] : NULL;
    if (h) _snprintf(out, cap, "{\"holder\":{\"session_id\":\"%s\",\"client\":\"%s\"}}", h->sid, h->client);
    else _snprintf(out, cap, "{\"holder\":null}");
}

/* ---------------- 配对码 ---------------- */
static void lan_ip(char* out, int cap)
{
    char hn[256]; struct addrinfo hints, *res = NULL, *a;
    _snprintf(out, cap, "%s", g_host);
    if (strcmp(g_host, "0.0.0.0") != 0) return;
    _snprintf(out, cap, "127.0.0.1");
    if (gethostname(hn, sizeof(hn)) != 0) return;
    memset(&hints, 0, sizeof(hints)); hints.ai_family = AF_INET;
    if (getaddrinfo(hn, NULL, &hints, &res) != 0) return;
    for (a = res; a; a = a->ai_next) {   /* 优先私网地址（跳过代理 TUN 198.18.x / 链路本地 169.254.x） */
        unsigned char* b = (unsigned char*)&((struct sockaddr_in*)a->ai_addr)->sin_addr;
        if ((b[0] == 192 && b[1] == 168) || b[0] == 10 || (b[0] == 172 && b[1] >= 16 && b[1] <= 31)) {
            _snprintf(out, cap, "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);
            break;
        }
    }
    freeaddrinfo(res);
}

static void new_pair_code(void)
{
    char ip[64], line[400];
    if (g_fixed_code[0]) strcpy(g_code, g_fixed_code);
    else {
        uint8_t b[4]; uint32_t r;
        rand_bytes(b, 4);
        r = (((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | b[3]) % 1000000u;
        _snprintf(g_code, sizeof(g_code), "%06u", r);
    }
    g_code_exp = now_us() + R_PAIR_TTL_US;
    lan_ip(ip, sizeof(ip));
    _snprintf(line, sizeof(line), "orp://pair?d=%s&c=%s&r=ws%%3A%%2F%%2F%s%%3A%d", g_device_id, g_code, ip, g_port);
    if (g_fw && g_fw->log) g_fw->log(OS_LOG_INFO, "remote: 配对码 %s（5 分钟有效，用后即换）  二维码内容: %s", g_code, line);
}

/* ---------------- 采样环（持 g_cs） ---------------- */
static Trk* trk_find(int id) { int i; for (i = 0; i < R_MAX_TRK; i++) if (g_trk[i].used && g_trk[i].id == id) return &g_trk[i]; return NULL; }

static Trk* trk_add(int id)
{
    Trk* t = trk_find(id); int i;
    if (t) { t->refs++; return t; }
    for (i = 0; i < R_MAX_TRK; i++) {
        if (g_trk[i].used) continue;
        t = &g_trk[i];
        memset(t, 0, sizeof(*t));
        t->ts = (int64_t*)malloc(sizeof(int64_t) * R_RING);
        t->v = (double*)malloc(sizeof(double) * R_RING);
        if (!t->ts || !t->v) { free(t->ts); free(t->v); memset(t, 0, sizeof(*t)); return NULL; }
        t->used = 1; t->id = id; t->refs = 1;
        return t;
    }
    /* 没有空槽：回收一个闲置的 */
    for (i = 0; i < R_MAX_TRK; i++) {
        if (g_trk[i].refs == 0) {
            t = &g_trk[i];
            t->id = id; t->refs = 1; t->head = t->count = 0; t->last_ts = 0;
            return t;
        }
    }
    return NULL;
}

static void trk_release(int id) { Trk* t = trk_find(id); if (t && t->refs > 0 && --t->refs == 0) t->idle_since = now_us(); }

static void sess_release_subs(Sess* s)
{
    int k, j;
    for (k = 0; k < R_MAX_SUBS; k++) if (s->subs[k].used) for (j = 0; j < s->subs[k].nvars; j++) trk_release(s->subs[k].vars[j]);
}

static void trk_push(Trk* t, int64_t ts, double v)
{
    t->ts[t->head] = ts; t->v[t->head] = v;
    t->head = (t->head + 1) % R_RING;
    if (t->count < R_RING) t->count++;
    t->last_ts = ts;
}

static int trk_range(Trk* t, int64_t since, int64_t upto, int64_t* ots, double* ov, int max)   /* (since, upto] 升序 */
{
    int i, n = 0, start = (t->head - t->count + R_RING) % R_RING, k = t->count;
    while (k > 0) { int idx = (start + k - 1) % R_RING; if (t->ts[idx] <= since) break; k--; }
    for (i = k; i < t->count && n < max; i++) {
        int idx = (start + i) % R_RING;
        if (t->ts[idx] > upto) break;
        ots[n] = t->ts[idx]; ov[n] = t->v[idx]; n++;
    }
    return n;
}

/* ---------------- 二进制帧（小端；头 16B + n u16 + 记录） ---------------- */
static void put16(uint8_t* p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void put32(uint8_t* p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24); }
static void putval(uint8_t* p, double v, int f64) { if (f64) memcpy(p, &v, 8); else { float f = (float)v; memcpy(p, &f, 4); } }

static void build_frames(Out** tail, RConn* conn, int kind, int sid, uint32_t* seq, const Rec* r, int n, int f64)
{
    int vs = f64 ? 8 : 4, rs = 6 + (kind == 1 ? vs : 3 * vs);
    int per = (R_MAX_FRAME - 18) / rs, i = 0;
    while (i < n) {
        int cnt = n - i < per ? n - i : per, j, k = 0;
        int64_t t0 = r[i].ts;
        Out* o;
        for (j = i; j < i + cnt; j++) if (r[j].ts < t0) t0 = r[j].ts;
        o = (Out*)calloc(1, sizeof(Out));
        if (!o) return;
        o->data = (uint8_t*)malloc(18 + (size_t)cnt * rs);
        if (!o->data) { free(o); return; }
        o->data[0] = 1; o->data[1] = (uint8_t)kind; put16(o->data + 2, (uint32_t)sid);
        put32(o->data + 4, (*seq)++);
        put32(o->data + 8, (uint32_t)((uint64_t)t0 >> 32)); put32(o->data + 12, (uint32_t)((uint64_t)t0 & 0xFFFFFFFFu));
        for (j = i; j < i + cnt; j++) {
            uint8_t* p = o->data + 18 + (size_t)k * rs;
            int64_t dt = r[j].ts - t0;
            if (dt < 0 || dt > 0xFFFFFFFFLL) continue;
            put16(p, (uint32_t)r[j].vid); put32(p + 2, (uint32_t)dt);
            putval(p + 6, r[j].a, f64);
            if (kind == 2) { putval(p + 6 + vs, r[j].b, f64); putval(p + 6 + 2 * vs, r[j].c, f64); }
            k++;
        }
        put16(o->data + 16, (uint32_t)k);
        o->len = 18 + k * rs;
        rc_ref(conn); o->conn = conn;
        (*tail)->next = o; *tail = o;
        i += cnt;
    }
}

static int bucketize(int vid, const int64_t* ts, const double* v, int n, int64_t bu, Rec* out, int max)   /* minmax：a=min b=max c=last */
{
    int i, m = 0; int64_t cb = 0;
    for (i = 0; i < n; i++) {
        int64_t b = ts[i] - (ts[i] % bu);
        if (m == 0 || b != cb) {
            if (m >= max) break;
            cb = b; out[m].vid = vid; out[m].ts = b; out[m].a = out[m].b = out[m].c = v[i]; m++;
        } else {
            Rec* r = &out[m - 1];
            if (v[i] < r->a) r->a = v[i];
            if (v[i] > r->b) r->b = v[i];
            r->c = v[i];
        }
    }
    return m;
}

static void flush_out(Out* head)
{
    Out* o = head->next;
    while (o) {
        Out* nx = o->next;
        os_ws_send_bin(o->conn->ws, o->data, (uint32_t)o->len);
        rc_unref(o->conn);
        free(o->data); free(o);
        o = nx;
    }
    head->next = NULL;
}

/* ---------------- 推流线程：采样 + 定时推送 + 心跳/会话回收 ---------------- */
static DWORD WINAPI pump_thread(LPVOID p)
{
    char out[1024];
    (void)p;
    timeBeginPeriod(1);
    while (InterlockedCompareExchange(&g_running, 1, 1) == 1) {
        int ids[R_MAX_TRK], nid = 0, i;
        int64_t t;
        Out head; Out* tail = &head;
        head.next = NULL;
        Sleep(1);
        /* 1) 采样：读被订阅变量的最新样本（宿主采集线程按 watched 采样，ts_us 为 Unix 微秒） */
        EnterCriticalSection(&g_cs);
        for (i = 0; i < R_MAX_TRK; i++) if (g_trk[i].used && g_trk[i].refs > 0) ids[nid++] = g_trk[i].id;
        LeaveCriticalSection(&g_cs);
        for (i = 0; i < nid; i++) {
            char prm[48]; double hs = 0, v = 0, ts = 0;
            _snprintf(prm, sizeof(prm), "{\"id\":%d}", ids[i]);
            if (host("vars.read", prm, out, sizeof(out)) != OS_ERR_OK) continue;
            if (js_getnum(out, "has_sample", &hs) || hs < 1) continue;
            if (js_getnum(out, "value", &v) || js_getnum(out, "ts_us", &ts)) continue;
            EnterCriticalSection(&g_cs);
            {
                Trk* tk = trk_find(ids[i]);
                if (tk && (int64_t)ts > tk->last_ts) trk_push(tk, (int64_t)ts, v);
            }
            LeaveCriticalSection(&g_cs);
        }
        /* 2) 推流 + 心跳 + 回收 */
        t = now_us();
        EnterCriticalSection(&g_cs);
        for (i = 0; i < R_MAX_TRK; i++) {
            Trk* tk = &g_trk[i];
            if (tk->used && tk->refs == 0 && t - tk->idle_since > 120000000LL) { free(tk->ts); free(tk->v); memset(tk, 0, sizeof(*tk)); }
        }
        for (i = 0; i < R_MAX_SESS; i++) {
            Sess* s = &g_sess[i]; int k;
            if (!s->used) continue;
            if (!s->conn) {
                if (g_control == i && t - s->detached_at > R_CONTROL_GRACE_US) {
                    g_control = -1;
                    rlog2(OS_LOG_INFO, "remote: 会话 %s（%s）断线超过 15s，自动释放控制权", s->sid, s->client);
                }
                if (t - s->detached_at > R_SESSION_KEEP_US) {
                    sess_release_subs(s);
                    if (g_control == i) g_control = -1;
                    memset(s, 0, sizeof(*s));
                }
                continue;
            }
            if (t - s->last_rx > R_HB_TIMEOUT_US) {
                rlog2(OS_LOG_WARN, "remote: 会话 %s（%s）心跳超时，断开连接（会话保留 120s）", s->sid, s->client);
                os_ws_send_close(s->conn->ws, 4000, "heartbeat timeout");
                os_ws_shutdown(s->conn->ws);
                s->last_rx = t;
                continue;
            }
            for (k = 0; k < R_MAX_SUBS; k++) {
                Sub* sb = &s->subs[k]; int j, nr = 0;
                if (!sb->used || !sb->ready || t < sb->next_us) continue;
                sb->next_us = t + (int64_t)(1e6 / sb->rate_hz);
                for (j = 0; j < sb->nvars; j++) {
                    Trk* tk = trk_find(sb->vars[j]);
                    if (!tk || !tk->count) continue;
                    if (sb->mode == 0) {
                        int last = (tk->head - 1 + R_RING) % R_RING;
                        s_rec[nr].vid = tk->id; s_rec[nr].ts = tk->ts[last]; s_rec[nr].a = tk->v[last]; nr++;
                    } else if (sb->mode == 2) {
                        int n = trk_range(tk, sb->last_v[j], tk->last_ts, s_ts, s_v, S_TMP - nr), q;
                        for (q = 0; q < n; q++) { s_rec[nr].vid = tk->id; s_rec[nr].ts = s_ts[q]; s_rec[nr].a = s_v[q]; nr++; }
                        if (n) sb->last_v[j] = s_ts[n - 1];
                    } else {
                        /* 只发已完整的桶：上界取最新样本所在桶起点之前 */
                        int64_t bu = (int64_t)(1e6 / sb->rate_hz), upto;
                        int n;
                        if (bu < 1000) bu = 1000;
                        upto = tk->last_ts - (tk->last_ts % bu) - 1;
                        if (upto <= sb->last_v[j]) continue;
                        n = trk_range(tk, sb->last_v[j], upto, s_ts, s_v, S_TMP - nr);
                        nr += bucketize(tk->id, s_ts, s_v, n, bu, s_rec + nr, S_TMP - nr);
                        sb->last_v[j] = upto;
                    }
                }
                if (nr > 0) build_frames(&tail, s->conn, sb->kind, sb->id, &s->seq, s_rec, nr, sb->f64);
            }
        }
        LeaveCriticalSection(&g_cs);
        flush_out(&head);   /* 锁外发送；连接由引用计数保活 */
    }
    timeEndPeriod(1);
    return 0;
}

/* ---------------- 接探针一端的确认弹窗 ---------------- */
typedef int (WINAPI *MsgBoxTimeoutW)(HWND, LPCWSTR, LPCWSTR, UINT, WORD, DWORD);

static int confirm_elevate(const char* client, const char* sid, const char* role)
{
    wchar_t text[512];
    MsgBoxTimeoutW fn;
    int r;
    if (g_auto_elevate) {
        rlog2(OS_LOG_WARN, "remote: [测试自动确认] 会话 %s 申请 %s → 已批准", sid, role);
        return 1;
    }
    _snwprintf(text, 512,
        L"远程客户端请求提升权限：\n\n  客户端：%hs\n  会话：%hs\n  申请角色：%hs（可写变量）\n\n是否允许？（%d 秒无操作视为拒绝）",
        client, sid, role, R_ELEVATE_TIMEOUT_MS / 1000);
    fn = (MsgBoxTimeoutW)GetProcAddress(GetModuleHandleW(L"user32.dll"), "MessageBoxTimeoutW");
    if (fn) r = fn(NULL, text, L"OpenScope 远程调试", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL | MB_DEFBUTTON2, 0, R_ELEVATE_TIMEOUT_MS);
    else r = MessageBoxW(NULL, text, L"OpenScope 远程调试", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST | MB_SYSTEMMODAL | MB_DEFBUTTON2);
    rlog2(OS_LOG_INFO, "remote: 会话 %s 权限提升 → %s", sid, r == IDYES ? "已允许" : "已拒绝");
    return r == IDYES;
}

/* ---------------- 方法 ---------------- */
static int role_of(const char* s) { return !strcmp(s, "viewer") ? ROLE_VIEWER : !strcmp(s, "operator") ? ROLE_OPERATOR : !strcmp(s, "admin") ? ROLE_ADMIN : -1; }
static const char* role_name(int r) { return r == ROLE_ADMIN ? "admin" : r == ROLE_OPERATOR ? "operator" : "viewer"; }

static int is_connected(void)
{
    char st[4096]; double c = 0;
    if (host("status", "{}", st, sizeof(st)) == OS_ERR_OK) js_getnum(st, "connected", &c);
    return c > 0;
}

static void device_json(char* out, int cap)
{
    _snprintf(out, cap, "{\"device_id\":\"%s\",\"name\":\"OpenScope\",\"connected\":%d}", g_device_id, is_connected());
}

static void caps_json(Sess* s, char* out, int cap)
{
    _snprintf(out, cap, "{\"max_frame\":%d,\"max_request\":%d,\"modes\":[\"latest\",\"minmax\",\"raw\"],\"flash\":false,\"heartbeat_s\":%d%s}",
              R_MAX_FRAME, R_RECV_CAP - 1, R_HB_INTERVAL_S, strcmp(s->client, "mp") == 0 ? ",\"max_rate_hz\":20,\"max_vars\":4" : "");
}

static void meta_json(Sub* sb, char* out, int cap)   /* 持 g_cs */
{
    static const char* modes[] = { "latest", "minmax", "raw" };
    int n, j;
    n = _snprintf(out, cap, "{\"jsonrpc\":\"2.0\",\"method\":\"stream.meta\",\"params\":{\"stream_id\":%d,\"mode\":\"%s\",\"rate_hz\":%g,\"kind\":%d,\"vars\":[",
                  sb->id, modes[sb->mode], sb->rate_hz, sb->kind);
    for (j = 0; j < sb->nvars && n < cap - 400; j++) {
        VarInfo* v = var_by_id(sb->vars[j]); char esc[300];
        js_esc(v ? v->name : "?", esc, sizeof(esc));
        n += _snprintf(out + n, cap - n, "%s{\"var_id\":%d,\"name\":\"%s\",\"type\":\"%s\",\"value_type\":\"%s\"}",
                       j ? "," : "", sb->vars[j], esc, v ? v->type : "other", sb->f64 ? "f64" : "f32");
    }
    _snprintf(out + n, cap - n, "]}}");
}

static void kinds_json(int acc, char* out, int cap)
{
    _snprintf(out, cap, "%s%s%s", (acc & 2) ? "1" : "", (acc & 6) == 6 ? "," : "", (acc & 4) ? "2" : "");
}

static void m_hello(Ctx* x, const char* prm)
{
    char tok[64], cl[16] = "web", rs[40] = "", res[2048], caps[256], ctl[256], dev[256], streams[256] = "", ks[8];
    const char* ak = js_get(prm, "accept_kinds");
    int ti = -1, i, acc = 0, si = -1;
    Sess* s = NULL;
    if (js_getstr(prm, "token", tok, sizeof(tok)) != 0) { reply_err(x, E_UNAUTH, "unpaired or token invalid", NULL); return; }
    js_getstr(prm, "client", cl, sizeof(cl));
    if (strcmp(cl, "web") && strcmp(cl, "mp") && strcmp(cl, "os")) strcpy(cl, "web");
    if (ak && *ak == '[') {
        const char* p = js_ws(ak + 1); double d;
        while (p && *p && *p != ']') { if (js_num(p, &d) == 0 && (d == 1 || d == 2)) acc |= 1 << (int)d; p = js_skip(p); p = js_ws(p); if (p && *p == ',') p = js_ws(p + 1); }
    } else acc = 6;
    if (!acc) { reply_err(x, E_PARAMS, "accept_kinds must include 1 or 2", NULL); return; }
    kinds_json(acc, ks, sizeof(ks));
    js_getstr(prm, "resume_session_id", rs, sizeof(rs));
    device_json(dev, sizeof(dev));
    EnterCriticalSection(&g_cs);
    vars_refresh(0);
    for (i = 0; i < R_MAX_TOKENS; i++) if (g_tok[i].used && strcmp(g_tok[i].tok, tok) == 0) { ti = i; break; }
    if (ti < 0) { LeaveCriticalSection(&g_cs); reply_err(x, E_UNAUTH, "unpaired or token invalid", NULL); return; }
    if (x->sess >= 0) {   /* 同一连接重复 hello：脱离旧会话 */
        Sess* o = &g_sess[x->sess];
        if (o->conn == x->conn) { o->conn = NULL; o->detached_at = now_us(); rc_unref(x->conn); }
        x->sess = -1;
    }
    if (rs[0]) {
        RConn* old; int k, n = 0;
        for (i = 0; i < R_MAX_SESS; i++) if (g_sess[i].used && strcmp(g_sess[i].sid, rs) == 0) { s = &g_sess[i]; si = i; break; }
        if (!s || s->tok != ti) { LeaveCriticalSection(&g_cs); reply_err(x, E_EXPIRED, "session expired; re-subscribe", NULL); return; }
        old = s->conn;
        if (old) { os_ws_shutdown(old->ws); rc_unref(old); }
        s->accept = acc;
        /* 先在新连接上补发全部 meta，再挂接推流（仍持锁，推流线程此间不会发帧） */
        for (k = 0; k < R_MAX_SUBS; k++) if (s->subs[k].used) {
            char meta[8192];
            meta_json(&s->subs[k], meta, sizeof(meta));
            send_text(x->conn, meta);
            n += _snprintf(streams + n, sizeof(streams) - n, "%s%d", n ? "," : "", s->subs[k].id);
            s->subs[k].ready = 1; s->subs[k].next_us = 0;
        }
        rc_ref(x->conn);
        s->conn = x->conn; s->detached_at = 0; s->last_rx = now_us();
        x->sess = si;
        caps_json(s, caps, sizeof(caps)); control_json(si, ctl, sizeof(ctl));
        _snprintf(res, sizeof(res), "{\"session_id\":\"%s\",\"device\":%s,\"role\":\"%s\",\"accept_kinds\":[%s],\"resumed\":true,\"caps\":%s,\"control\":%s,\"streams\":[%s]}",
                  s->sid, dev, role_name(s->role), ks, caps, ctl, streams);
        LeaveCriticalSection(&g_cs);
        rlog2(OS_LOG_INFO, "remote: 会话 %s（%s）已恢复", rs, cl);
        reply_raw(x, res);
        return;
    }
    for (i = 0; i < R_MAX_SESS; i++) if (!g_sess[i].used) { s = &g_sess[i]; si = i; break; }
    if (!s) {   /* 回收最老的已脱离会话 */
        int64_t oldest = INT64_MAX;
        for (i = 0; i < R_MAX_SESS; i++) if (!g_sess[i].conn && g_control != i && g_sess[i].detached_at < oldest) { oldest = g_sess[i].detached_at; si = i; }
        if (si < 0) { LeaveCriticalSection(&g_cs); reply_err(x, E_TOOMANY, "too many sessions", NULL); return; }
        s = &g_sess[si];
        sess_release_subs(s);
    }
    memset(s, 0, sizeof(*s));
    s->used = 1; rand_hex(s->sid, 8); s->tok = ti; s->role = g_tok[ti].role;
    strcpy(s->client, cl); s->accept = acc;
    rc_ref(x->conn); s->conn = x->conn; s->last_rx = now_us();
    x->sess = si;
    caps_json(s, caps, sizeof(caps)); control_json(si, ctl, sizeof(ctl));
    _snprintf(res, sizeof(res), "{\"session_id\":\"%s\",\"device\":%s,\"role\":\"%s\",\"accept_kinds\":[%s],\"resumed\":false,\"caps\":%s,\"control\":%s}",
              s->sid, dev, role_name(s->role), ks, caps, ctl);
    strcpy(rs, s->sid);
    LeaveCriticalSection(&g_cs);
    rlog2(OS_LOG_INFO, "remote: 会话 %s（%s）建立", rs, cl);
    reply_raw(x, res);
}

static void m_pair(Ctx* x, const char* prm)
{
    char code[16] = "", cl[16] = "web", res[256];
    int i, slot = -1;
    js_getstr(prm, "code", code, sizeof(code));
    js_getstr(prm, "client", cl, sizeof(cl));
    if (strcmp(cl, "web") && strcmp(cl, "mp") && strcmp(cl, "os")) strcpy(cl, "web");
    EnterCriticalSection(&g_cs);
    if (!g_fixed_code[0] && now_us() > g_code_exp) { new_pair_code(); LeaveCriticalSection(&g_cs); reply_err(x, E_UNAUTH, "pairing code expired", NULL); return; }
    if (strcmp(code, g_code) != 0) { LeaveCriticalSection(&g_cs); Sleep(200); reply_err(x, E_UNAUTH, "invalid pairing code", NULL); return; }
    for (i = 0; i < R_MAX_TOKENS; i++) if (!g_tok[i].used) { slot = i; break; }
    if (slot < 0) {   /* 满：顶掉一个没有在用会话的 token */
        int k;
        for (i = 0; i < R_MAX_TOKENS && slot < 0; i++) {
            int inuse = 0;
            for (k = 0; k < R_MAX_SESS; k++) if (g_sess[k].used && g_sess[k].tok == i) inuse = 1;
            if (!inuse) slot = i;
        }
        if (slot < 0) { LeaveCriticalSection(&g_cs); reply_err(x, E_TOOMANY, "too many paired clients", NULL); return; }
    }
    memset(&g_tok[slot], 0, sizeof(Token));
    g_tok[slot].used = 1; g_tok[slot].role = ROLE_VIEWER; strcpy(g_tok[slot].client, cl);
    rand_hex(g_tok[slot].tok, 20);
    _snprintf(res, sizeof(res), "{\"token\":\"%s\",\"role\":\"viewer\",\"device_id\":\"%s\"}", g_tok[slot].tok, g_device_id);
    if (!g_fixed_code[0]) new_pair_code();
    LeaveCriticalSection(&g_cs);
    rlog2(OS_LOG_INFO, "remote: 新客户端已配对（%s，%s）", cl, "viewer");
    reply_raw(x, res);
}

static void m_elevate(Ctx* x, Sess* s, const char* prm)
{
    char role[16] = "", res[64], sid[20], cl[8];
    int r;
    if (js_getstr(prm, "role", role, sizeof(role)) != 0 || (r = role_of(role)) < 0) { reply_err(x, E_PARAMS, "role must be viewer|operator|admin", NULL); return; }
    if (r == ROLE_ADMIN) { reply_err(x, E_FORBIDDEN, "admin not grantable remotely", "{\"os_err\":-6}"); return; }
    EnterCriticalSection(&g_cs); strcpy(sid, s->sid); strcpy(cl, s->client); LeaveCriticalSection(&g_cs);
    if (r > s->role && !confirm_elevate(cl, sid, role)) { reply_err(x, E_FORBIDDEN, "elevation denied at probe side", "{\"os_err\":-6}"); return; }
    EnterCriticalSection(&g_cs);
    if (s->used && strcmp(s->sid, sid) == 0) { s->role = r; g_tok[s->tok].role = r; }
    LeaveCriticalSection(&g_cs);
    _snprintf(res, sizeof(res), "{\"role\":\"%s\"}", role);
    reply_raw(x, res);
}

static void m_vars_list(Ctx* x, const char* prm)
{
    char needle[128] = "", lw[128], esc[300];
    double d; int off = 0, lim = 200, i, total = 0, emitted = 0, n;
    char* out; char* res;
    js_getstr(prm, "needle", needle, sizeof(needle));
    if (js_getnum(prm, "offset", &d) == 0) off = (int)d;
    if (js_getnum(prm, "limit", &d) == 0) lim = (int)d;
    if (off < 0 || lim < 0) { reply_err(x, E_PARAMS, "offset/limit invalid", "{\"os_err\":-2}"); return; }
    if (lim > 2000) lim = 2000;
    for (i = 0; needle[i]; i++) lw[i] = (char)tolower((unsigned char)needle[i]);
    lw[i] = 0;
    out = (char*)malloc(R_RESULT_CAP);
    res = (char*)malloc(R_RESULT_CAP + 64);
    if (!out || !res) { free(out); free(res); reply_err(x, E_SRVFAIL, "oom", NULL); return; }
    n = 0; out[0] = 0;
    EnterCriticalSection(&g_cs);
    vars_refresh(0);
    for (i = 0; i < g_nvars; i++) {
        VarInfo* v = &g_vars[i];
        if (lw[0]) {
            char nm[256]; int k;
            for (k = 0; v->name[k] && k < 255; k++) nm[k] = (char)tolower((unsigned char)v->name[k]);
            nm[k] = 0;
            if (!strstr(nm, lw)) continue;
        }
        total++;
        if (total <= off || emitted >= lim || n > R_RESULT_CAP - 600) continue;
        js_esc(v->name, esc, sizeof(esc));
        n += _snprintf(out + n, R_RESULT_CAP - n, "%s{\"id\":%d,\"name\":\"%s\",\"addr\":\"%s\",\"size\":%d,\"type\":\"%s\",\"writable\":%d}",
                       emitted ? "," : "", v->id, esc, v->addr, v->size, v->type, v->writable);
        emitted++;
    }
    LeaveCriticalSection(&g_cs);
    _snprintf(res, R_RESULT_CAP + 64, "{\"total\":%d,\"vars\":[%s]}", total, out);
    reply_raw(x, res);
    free(out); free(res);
}

static int decode_mem(const char* hex, const VarInfo* v, double* out)   /* 按类型解码小端内存 */
{
    uint8_t b[8] = {0}; int i, n = v->size;
    if (n <= 0 || n > 8 || (int)strlen(hex) < 2 * n || strstr(v->type, ":bf") || strchr(v->type, '*')) return -1;
    for (i = 0; i < n; i++) { unsigned xv = 0; sscanf(hex + 2 * i, "%2x", &xv); b[i] = (uint8_t)xv; }
    if (!strcmp(v->type, "double") && n == 8) { double dv; memcpy(&dv, b, 8); *out = dv; return 0; }
    if (!strcmp(v->type, "float") && n == 4) { float f; memcpy(&f, b, 4); *out = f; return 0; }
    if (!strncmp(v->type, "float", 5) || !strncmp(v->type, "double", 6)) return -1;
    {
        uint64_t u = 0; for (i = n - 1; i >= 0; i--) u = (u << 8) | b[i];
        if (!strncmp(v->type, "int", 3)) { int sh = 64 - 8 * n; *out = (double)(((int64_t)(u << sh)) >> sh); }
        else *out = (double)u;
    }
    return 0;
}

static void m_vars_read(Ctx* x, const char* prm)
{
    VarInfo vv; VarInfo* v; char out[4096], res[700], esc[300], prm2[96]; double d;
    EnterCriticalSection(&g_cs); vars_refresh(0); v = var_from_params(prm); if (v) vv = *v; LeaveCriticalSection(&g_cs);
    if (!v) { reply_err(x, E_PARAMS, "id or name required/unknown", "{\"os_err\":-2}"); return; }
    js_esc(vv.name, esc, sizeof(esc));
    /* 优先直读目标内存（写值后立即可读回）；失败退回采集样本 */
    _snprintf(prm2, sizeof(prm2), "{\"address\":\"%s\",\"size\":%d}", vv.addr, vv.size);
    if (vv.size > 0 && vv.size <= 8 && host("mem.read", prm2, out, sizeof(out)) == OS_ERR_OK) {
        char hex[64];
        if (js_getstr(out, "data", hex, sizeof(hex)) == 0 && decode_mem(hex, &vv, &d) == 0) {
            _snprintf(res, sizeof(res), "{\"id\":%d,\"name\":\"%s\",\"has_sample\":1,\"value\":%.17g,\"ts_us\":%lld,\"source\":\"mem\"}", vv.id, esc, d, (long long)now_us());
            reply_raw(x, res); return;
        }
    }
    _snprintf(prm2, sizeof(prm2), "{\"id\":%d}", vv.id);
    if (host("vars.read", prm2, out, sizeof(out)) == OS_ERR_OK && js_getnum(out, "has_sample", &d) == 0 && d > 0) {
        double val = 0, ts = 0;
        js_getnum(out, "value", &val); js_getnum(out, "ts_us", &ts);
        _snprintf(res, sizeof(res), "{\"id\":%d,\"name\":\"%s\",\"has_sample\":1,\"value\":%.17g,\"ts_us\":%lld,\"source\":\"acq\"}", vv.id, esc, val, (long long)ts);
    } else
        _snprintf(res, sizeof(res), "{\"id\":%d,\"name\":\"%s\",\"has_sample\":0}", vv.id, esc);
    reply_raw(x, res);
}

static void m_vars_write(Ctx* x, Sess* s, const char* prm)
{
    VarInfo vv; VarInfo* v; char rid[64] = "", out[1024], res[320], msg[200], prm2[128], ridj[80] = "null", hd[160];
    double val; int i, have_rid = 0, me = (int)(s - g_sess);
    if (js_getstr(prm, "req_id", rid, sizeof(rid)) == 0) have_rid = 1;
    else if (js_getnum(prm, "req_id", &val) == 0) { _snprintf(rid, sizeof(rid), "%.0f", val); have_rid = 1; }
    EnterCriticalSection(&g_cs);
    if (s->role < ROLE_OPERATOR) { LeaveCriticalSection(&g_cs); reply_err(x, E_FORBIDDEN, "permission denied: need operator", "{\"need\":\"operator\"}"); return; }
    if (g_control != me) { holder_json(hd, sizeof(hd)); LeaveCriticalSection(&g_cs); reply_err(x, E_LOCKED, g_control >= 0 ? "control locked" : "control not acquired", hd); return; }
    if (have_rid) for (i = 0; i < R_REQ_CACHE; i++) if (s->rc[i].rid[0] && strcmp(s->rc[i].rid, rid) == 0) {
        char dup[320];
        _snprintf(dup, sizeof(dup), "%s,\"duplicate\":true}", s->rc[i].res);
        LeaveCriticalSection(&g_cs); reply_raw(x, dup); return;
    }
    vars_refresh(0); v = var_from_params(prm); if (v) vv = *v;
    LeaveCriticalSection(&g_cs);
    if (!v) { reply_err(x, E_PARAMS, "id or name required/unknown", "{\"os_err\":-2}"); return; }
    if (!vv.writable) { reply_err(x, E_PARAMS, "variable not writable", "{\"os_err\":-2}"); return; }
    if (js_getnum(prm, "value", &val) != 0 || !isfinite(val)) { reply_err(x, E_PARAMS, "value must be number", "{\"os_err\":-2}"); return; }
    if (!strcmp(vv.type, "float")) val = (double)(float)val;
    else if (strcmp(vv.type, "double")) val = (double)(long long)val;   /* 整型/布尔/枚举截断 */
    _snprintf(prm2, sizeof(prm2), "{\"id\":%d,\"value\":%.17g}", vv.id, val);
    if (host("vars.write", prm2, out, sizeof(out)) != OS_ERR_OK) {
        double code = -1; char data[48];
        host_msg(out, msg, sizeof(msg));
        js_getnum(out, "error", &code);
        _snprintf(data, sizeof(data), "{\"os_err\":%d}", (int)code);
        reply_err(x, ((int)code == OS_ERR_NOT_CONNECTED || !is_connected()) ? E_OFFLINE : E_SRVFAIL, msg, data);
        return;
    }
    if (have_rid) { char e[80]; js_esc(rid, e, sizeof(e)); _snprintf(ridj, sizeof(ridj), "\"%s\"", e); }
    _snprintf(res, sizeof(res), "{\"id\":%d,\"value\":%.17g,\"req_id\":%s,\"duplicate\":false}", vv.id, val, ridj);
    if (have_rid) {
        EnterCriticalSection(&g_cs);
        _snprintf(s->rc[s->rc_next].rid, sizeof(s->rc[0].rid), "%s", rid);
        _snprintf(s->rc[s->rc_next].res, sizeof(s->rc[0].res), "{\"id\":%d,\"value\":%.17g,\"req_id\":%s", vv.id, val, ridj);
        s->rc_next = (s->rc_next + 1) % R_REQ_CACHE;
        LeaveCriticalSection(&g_cs);
    }
    if (g_fw && g_fw->log) g_fw->log(OS_LOG_INFO, "remote: 写 %s = %.9g（%s）", vv.name, val, s->client);
    reply_raw(x, res);
}

static int ensure_acq(void)
{
    char out[512]; double st = 0;
    if (host("acq.state", "{}", out, sizeof(out)) == OS_ERR_OK && js_getnum(out, "acq_state", &st) == 0 && st == 1) return 0;
    return host("acq.start", "{}", out, sizeof(out)) == OS_ERR_OK ? 0 : -1;
}

static void m_subscribe(Ctx* x, Sess* s, const char* prm)
{
    char mode[16] = "", vt[8] = "f32", res[1024], meta[8192];
    int vids[R_SUB_VARS], nv, i, kind, m, mp = strcmp(s->client, "mp") == 0, k = -1;
    double rate;
    Sub* sb;
    EnterCriticalSection(&g_cs);
    vars_refresh(0);
    nv = parse_var_list(js_get(prm, "vars"), vids, R_SUB_VARS);
    LeaveCriticalSection(&g_cs);
    if (nv < 0) { reply_err(x, E_PARAMS, "unknown var", "{\"os_err\":-2}"); return; }
    if (nv == 0) { reply_err(x, E_PARAMS, "vars required", "{\"os_err\":-2}"); return; }
    if (js_getstr(prm, "mode", mode, sizeof(mode)) != 0) strcpy(mode, mp ? "minmax" : "latest");
    m = !strcmp(mode, "latest") ? 0 : !strcmp(mode, "minmax") ? 1 : !strcmp(mode, "raw") ? 2 : -1;
    if (m < 0) { reply_err(x, E_PARAMS, "mode must be latest|minmax|raw", NULL); return; }
    if (js_getnum(prm, "rate_hz", &rate) != 0) rate = mp ? 10 : 30;
    if (rate <= 0 || rate > 200) { reply_err(x, E_PARAMS, "rate_hz out of range (0,200]", NULL); return; }
    if (mp) { if (rate > 20) rate = 20; if (nv > 4) nv = 4; }
    js_getstr(prm, "value_type", vt, sizeof(vt));
    if (strcmp(vt, "f32") && strcmp(vt, "f64")) { reply_err(x, E_PARAMS, "value_type must be f32|f64", NULL); return; }
    kind = m == 1 ? 2 : 1;
    if (!(s->accept & (1 << kind))) { reply_err(x, E_PARAMS, "mode needs a kind not in accept_kinds", NULL); return; }
    if (!is_connected()) { reply_err(x, E_OFFLINE, "device offline (probe not connected)", "{\"os_err\":-4}"); return; }
    /* 勾选观测 + 确保采集运行（宿主采集线程每周期按 watched 取样，无需重启采集） */
    for (i = 0; i < nv; i++) { char p2[64], o[512]; _snprintf(p2, sizeof(p2), "{\"id\":%d,\"on\":1}", vids[i]); host("vars.watch", p2, o, sizeof(o)); }
    if (ensure_acq() != 0) { reply_err(x, E_OFFLINE, "acquisition start failed", "{\"os_err\":-4}"); return; }
    EnterCriticalSection(&g_cs);
    for (i = 0; i < R_MAX_SUBS; i++) if (!s->subs[i].used) { k = i; break; }
    if (k < 0) { LeaveCriticalSection(&g_cs); reply_err(x, E_PARAMS, "too many streams (max 8)", NULL); return; }
    for (i = 0; i < nv; i++) if (!trk_add(vids[i])) {
        int j; for (j = 0; j < i; j++) trk_release(vids[j]);
        LeaveCriticalSection(&g_cs); reply_err(x, E_PARAMS, "too many tracked variables (max 16)", NULL); return;
    }
    sb = &s->subs[k];
    memset(sb, 0, sizeof(*sb));
    sb->used = 1; sb->id = g_next_stream; g_next_stream = g_next_stream % 65535 + 1;
    sb->mode = m; sb->kind = kind; sb->f64 = !strcmp(vt, "f64"); sb->rate_hz = rate; sb->nvars = nv;
    memcpy(sb->vars, vids, sizeof(int) * (size_t)nv);
    for (i = 0; i < nv; i++) { Trk* tk = trk_find(vids[i]); sb->last_v[i] = tk ? tk->last_ts : 0; }
    meta_json(sb, meta, sizeof(meta));
    {
        int n = _snprintf(res, sizeof(res), "{\"stream_id\":%d,\"mode\":\"%s\",\"rate_hz\":%g,\"vars\":[", sb->id, mode, rate);
        for (i = 0; i < nv; i++) n += _snprintf(res + n, sizeof(res) - n, "%s%d", i ? "," : "", vids[i]);
        _snprintf(res + n, sizeof(res) - n, "],\"value_type\":\"%s\",\"kind\":%d}", vt, kind);
    }
    /* 持锁：先 meta 再 result，最后置 ready，推流线程之后才会发该流的帧 */
    send_text(x->conn, meta);
    reply_raw(x, res);
    sb->ready = 1;
    LeaveCriticalSection(&g_cs);
}

static void m_unsubscribe(Ctx* x, Sess* s, const char* prm)
{
    double d; int all = js_getnum(prm, "stream_id", &d) != 0, k, j, n, first = 1;
    char res[256];
    EnterCriticalSection(&g_cs);
    for (k = 0; k < R_MAX_SUBS; k++) {
        Sub* sb = &s->subs[k];
        if (!sb->used || (!all && sb->id != (int)d)) continue;
        for (j = 0; j < sb->nvars; j++) trk_release(sb->vars[j]);
        memset(sb, 0, sizeof(*sb));
    }
    n = _snprintf(res, sizeof(res), "{\"streams\":[");
    for (k = 0; k < R_MAX_SUBS; k++) if (s->subs[k].used) { n += _snprintf(res + n, sizeof(res) - n, "%s%d", first ? "" : ",", s->subs[k].id); first = 0; }
    _snprintf(res + n, sizeof(res) - n, "]}");
    LeaveCriticalSection(&g_cs);
    reply_raw(x, res);
}

static void m_history(Ctx* x, Sess* s, const char* prm)
{
    char mode[16] = "raw", vt[8] = "f32", res[8192];
    double d; int64_t t1 = now_us(), t0; int mp = 1000, kind, nv, i, nr = 0, n, f64;
    int vids[R_SUB_VARS];
    int64_t* ts; double* vv; Rec* rec;
    Out head; Out* tail = &head; uint32_t seq0;
    head.next = NULL;
    js_getstr(prm, "mode", mode, sizeof(mode));
    if (strcmp(mode, "raw") && strcmp(mode, "minmax")) { reply_err(x, E_PARAMS, "mode must be raw|minmax", NULL); return; }
    js_getstr(prm, "value_type", vt, sizeof(vt));
    if (strcmp(vt, "f32") && strcmp(vt, "f64")) { reply_err(x, E_PARAMS, "value_type must be f32|f64", NULL); return; }
    f64 = !strcmp(vt, "f64");
    if (js_getnum(prm, "t1", &d) == 0 && d > 0) t1 = (int64_t)d;
    t0 = t1 - 5000000;
    if (js_getnum(prm, "t0", &d) == 0 && d > 0) t0 = (int64_t)d;
    if (t0 >= t1) { reply_err(x, E_PARAMS, "t0 must be < t1", NULL); return; }
    if (js_getnum(prm, "max_points", &d) == 0 && d > 0) mp = (int)d;
    if (mp > 20000) mp = 20000;
    kind = !strcmp(mode, "minmax") ? 2 : 1;
    if (!(s->accept & (1 << kind))) { reply_err(x, E_PARAMS, "kind not in accept_kinds", NULL); return; }
    EnterCriticalSection(&g_cs);
    vars_refresh(0);
    nv = parse_var_list(js_get(prm, "vars"), vids, R_SUB_VARS);
    LeaveCriticalSection(&g_cs);
    if (nv < 0) { reply_err(x, E_PARAMS, "unknown var", "{\"os_err\":-2}"); return; }
    if (nv == 0) { reply_err(x, E_PARAMS, "vars required", "{\"os_err\":-2}"); return; }
    ts = (int64_t*)malloc(sizeof(int64_t) * R_RING); vv = (double*)malloc(sizeof(double) * R_RING);
    rec = (Rec*)malloc(sizeof(Rec) * (size_t)(mp * 2 + 16) * (size_t)nv);
    if (!ts || !vv || !rec) { free(ts); free(vv); free(rec); reply_err(x, E_SRVFAIL, "oom", NULL); return; }
    EnterCriticalSection(&g_cs);
    for (i = 0; i < nv; i++) {
        Trk* tk = trk_find(vids[i]); int c, q;
        if (!tk) continue;     /* 未被订阅过的变量没有历史 */
        c = trk_range(tk, t0 - 1, t1, ts, vv, R_RING);
        if (kind == 2) {
            int64_t bu = (t1 - t0) / mp; if (bu < 1000) bu = 1000;
            nr += bucketize(vids[i], ts, vv, c, bu, rec + nr, mp * 2 + 16);
        } else {
            int step = c / mp; if (step < 1) step = 1;
            for (q = 0; q < c && nr < (mp * 2 + 16) * nv; q += step) { rec[nr].vid = vids[i]; rec[nr].ts = ts[q]; rec[nr].a = vv[q]; nr++; }
        }
    }
    seq0 = s->seq;
    build_frames(&tail, x->conn, kind, 0, &s->seq, rec, nr, f64);
    n = _snprintf(res, sizeof(res), "{\"frames\":%u,\"records\":%d,\"kind\":%d,\"stream_id\":0,\"t0\":%lld,\"t1\":%lld,\"value_type\":\"%s\",\"vars\":[",
                  (unsigned)(s->seq - seq0), nr, kind, (long long)t0, (long long)t1, vt);
    for (i = 0; i < nv && n < (int)sizeof(res) - 400; i++) {
        VarInfo* v = var_by_id(vids[i]); char esc[300];
        js_esc(v ? v->name : "?", esc, sizeof(esc));
        n += _snprintf(res + n, sizeof(res) - n, "%s{\"var_id\":%d,\"name\":\"%s\",\"type\":\"%s\",\"value_type\":\"%s\"}", i ? "," : "", vids[i], esc, v ? v->type : "other", vt);
    }
    _snprintf(res + n, sizeof(res) - n, "]}");
    LeaveCriticalSection(&g_cs);
    free(ts); free(vv); free(rec);
    flush_out(&head);   /* 帧先于 result */
    reply_raw(x, res);
}

static void m_control(Ctx* x, Sess* s, int acquire)
{
    char res[320], hd[160]; int me = (int)(s - g_sess);
    EnterCriticalSection(&g_cs);
    if (acquire) {
        if (s->role < ROLE_OPERATOR) { LeaveCriticalSection(&g_cs); reply_err(x, E_FORBIDDEN, "permission denied: need operator", "{\"need\":\"operator\"}"); return; }
        if (g_control >= 0 && g_control != me && g_sess[g_control].used) { holder_json(hd, sizeof(hd)); LeaveCriticalSection(&g_cs); reply_err(x, E_LOCKED, "control locked", hd); return; }
        if (g_control != me) g_control_since = now_us();
        g_control = me;
        control_json(me, res, sizeof(res));
    } else {
        size_t L;
        if (g_control == me) g_control = -1;
        control_json(me, res, sizeof(res));
        L = strlen(res);
        _snprintf(res + L - 1, sizeof(res) - L + 1, ",\"released\":true}");
    }
    LeaveCriticalSection(&g_cs);
    reply_raw(x, res);
}

static void m_status(Ctx* x, Sess* s)
{
    char res[1024], ctl[256], dev[256]; int n = 0, i;
    device_json(dev, sizeof(dev));
    EnterCriticalSection(&g_cs);
    for (i = 0; i < R_MAX_SESS; i++) if (g_sess[i].used) n++;
    control_json((int)(s - g_sess), ctl, sizeof(ctl));
    _snprintf(res, sizeof(res), "{\"device\":%s,\"role\":\"%s\",\"control\":%s,\"sessions\":%d,\"orp\":1}", dev, role_name(s->role), ctl, n);
    LeaveCriticalSection(&g_cs);
    reply_raw(x, res);
}

static void dispatch(Ctx* x, const char* method, const char* prm)
{
    Sess* s;
    if (!strcmp(method, "ping")) { char r[64]; _snprintf(r, sizeof(r), "{\"pong\":true,\"t_us\":%lld}", (long long)now_us()); reply_raw(x, r); return; }
    if (!strcmp(method, "pair.claim")) { m_pair(x, prm); return; }
    if (!strcmp(method, "session.hello")) { m_hello(x, prm); return; }
    if (x->sess < 0 || !g_sess[x->sess].used || g_sess[x->sess].conn != x->conn) { x->sess = -1; reply_err(x, E_UNAUTH, "session.hello with valid token required", NULL); return; }
    s = &g_sess[x->sess];
    if (!strcmp(method, "session.bye")) {
        EnterCriticalSection(&g_cs);
        sess_release_subs(s);
        if (g_control == x->sess) g_control = -1;
        if (s->conn) rc_unref(s->conn);
        memset(s, 0, sizeof(*s));
        x->sess = -1;
        LeaveCriticalSection(&g_cs);
        reply_raw(x, "{}");
        return;
    }
    if (!strcmp(method, "status")) { m_status(x, s); return; }
    if (!strcmp(method, "auth.elevate")) { m_elevate(x, s, prm); return; }
    if (!strcmp(method, "control.acquire")) { m_control(x, s, 1); return; }
    if (!strcmp(method, "control.release")) { m_control(x, s, 0); return; }
    if (!strcmp(method, "vars.list")) { m_vars_list(x, prm); return; }
    if (!strcmp(method, "vars.read")) { m_vars_read(x, prm); return; }
    if (!strcmp(method, "vars.write")) { m_vars_write(x, s, prm); return; }
    if (!strcmp(method, "stream.subscribe")) { m_subscribe(x, s, prm); return; }
    if (!strcmp(method, "stream.unsubscribe")) { m_unsubscribe(x, s, prm); return; }
    if (!strcmp(method, "history.fetch")) { m_history(x, s, prm); return; }
    if (!strncmp(method, "flash.", 6) || !strcmp(method, "mem.write")) { reply_err(x, E_FORBIDDEN, "remote flashing/memory write disabled", "{\"os_err\":-6}"); return; }
    reply_err(x, E_NOMETHOD, "unknown method", NULL);
}

/* 请求 id 原文（数字或带引号字符串） */
static void extract_rid(const char* req, char* out, int cap)
{
    const char* v = js_get(req, "id");
    out[0] = 0;
    if (!v || *v == 'n') return;
    if (*v == '"') {
        const char* e = js_skip(v);
        if (e && e - v < cap) { memcpy(out, v, (size_t)(e - v)); out[e - v] = 0; }
    } else {
        int i = 0;
        while ((v[i] == '-' || (v[i] >= '0' && v[i] <= '9')) && i < cap - 1) { out[i] = v[i]; i++; }
        out[i] = 0;
    }
}

/* 超大请求：在已收到的头部里找顶层 "id"（简单扫描，找不到回 null） */
static void extract_rid_head(const char* head, int len, char* out, int cap)
{
    const char* p = head; const char* end = head + len;
    out[0] = 0;
    while (p + 4 < end) {
        if (p[0] == '"' && p[1] == 'i' && p[2] == 'd' && p[3] == '"') {
            const char* q = p + 4;
            while (q < end && (*q == ' ' || *q == ':' || *q == '\t')) q++;
            if (q < end && *q == '"') {
                const char* e = q + 1;
                while (e < end && *e != '"' && e - q < 70) { if (*e == '\\') e++; e++; }
                if (e < end && *e == '"' && e - q + 2 < cap) { memcpy(out, q, (size_t)(e - q + 1)); out[e - q + 1] = 0; }
            } else {
                int i = 0;
                while (q < end && (*q == '-' || (*q >= '0' && *q <= '9')) && i < cap - 1) out[i++] = *q++;
                out[i] = 0;
            }
            return;
        }
        p++;
    }
}

/* ---------------- 线程 ---------------- */
typedef struct RAccept { int sock; int over_limit; } RAccept;

static DWORD WINAPI client_thread(LPVOID p)
{
    RAccept acc = *(RAccept*)p;
    Ctx x; uint8_t* buf; OS_WSConn* ws; int slot = -1, i;
    free(p);
    ws = os_ws_server_upgrade_proto(acc.sock, R_HANDSHAKE_MS, "orp.v1");
    if (!ws) { InterlockedDecrement(&g_clients); return 0; }
    memset(&x, 0, sizeof(x)); x.sess = -1;
    x.conn = (RConn*)calloc(1, sizeof(RConn));
    if (!x.conn) { os_ws_close(ws); InterlockedDecrement(&g_clients); return 0; }
    x.conn->ws = ws; x.conn->refs = 1;
    if (acc.over_limit) {
        char m[160];
        _snprintf(m, sizeof(m), "{\"jsonrpc\":\"2.0\",\"id\":null,\"error\":{\"code\":-32001,\"message\":\"too many clients (max %d)\"}}", R_MAX_CLIENTS);
        os_ws_send_text(ws, m);
        os_ws_send_close(ws, 1013, "too many clients");
        rc_unref(x.conn);
        InterlockedDecrement(&g_clients);
        return 0;
    }
    EnterCriticalSection(&g_cs);
    for (i = 0; i < R_MAX_CLIENTS + R_MAX_PENDING; i++) if (!g_live[i]) { g_live[i] = x.conn; slot = i; rc_ref(x.conn); break; }
    LeaveCriticalSection(&g_cs);
    buf = (uint8_t*)malloc(R_RECV_CAP);
    while (buf && InterlockedCompareExchange(&g_running, 1, 1) == 1) {
        int op = 0, head = 0, n = os_ws_recv_capped_head(ws, buf, R_RECV_CAP - 1, &op, R_DRAIN_MAX, &head);
        if (n == OS_WS_TOO_BIG) {
            char m[300], rid[80];
            extract_rid_head((const char*)buf, head < 4096 ? head : 4096, rid, sizeof(rid));
            _snprintf(m, sizeof(m), "{\"jsonrpc\":\"2.0\",\"id\":%s,\"error\":{\"code\":-32600,\"message\":\"request too large (max %d bytes)\"}}", rid[0] ? rid : "null", R_RECV_CAP - 1);
            os_ws_send_text(ws, m);
            continue;
        }
        if (n <= 0) break;
        EnterCriticalSection(&g_cs);
        if (x.sess >= 0 && g_sess[x.sess].conn == x.conn) g_sess[x.sess].last_rx = now_us();
        LeaveCriticalSection(&g_cs);
        if (op != OS_WS_OP_TEXT) { x.rid[0] = 0; reply_err(&x, E_INVREQ, "binary requests not supported", NULL); continue; }
        buf[n] = 0;
        {
            const char* req = js_ws((const char*)buf);
            char method[64];
            const char* prm;
            x.rid[0] = 0;
            if (!req || *req != '{' || !js_skip(req)) { reply_err(&x, E_PARSE, "parse error", NULL); continue; }
            extract_rid(req, x.rid, sizeof(x.rid));
            if (js_getstr(req, "method", method, sizeof(method)) != 0) { reply_err(&x, E_INVREQ, "method required", NULL); continue; }
            prm = js_get(req, "params");
            if (prm && *prm == 'n') prm = NULL;
            if (prm && *prm != '{') { reply_err(&x, E_PARAMS, "params must be object", NULL); continue; }
            dispatch(&x, method, prm ? prm : "{}");
        }
    }
    free(buf);
    EnterCriticalSection(&g_cs);
    if (x.sess >= 0 && g_sess[x.sess].conn == x.conn) {
        g_sess[x.sess].conn = NULL; g_sess[x.sess].detached_at = now_us();
        rc_unref(x.conn);
    }
    if (slot >= 0 && g_live[slot] == x.conn) { g_live[slot] = NULL; rc_unref(x.conn); }
    LeaveCriticalSection(&g_cs);
    os_ws_shutdown(ws);
    InterlockedDecrement(&g_clients);   /* 连接关闭即释放名额（会话按保留期另行保留，不占名额） */
    rc_unref(x.conn);
    return 0;
}

static DWORD WINAPI listen_thread(LPVOID p)
{
    (void)p;
    while (InterlockedCompareExchange(&g_running, 1, 1) == 1) {
        int s = os_ws_accept_socket(g_listen);
        HANDLE th; RAccept* acc; LONG n;
        if (s < 0) { if (InterlockedCompareExchange(&g_running, 1, 1) != 1) break; Sleep(50); continue; }
        n = InterlockedIncrement(&g_clients);
        if (n > R_MAX_CLIENTS + R_MAX_PENDING) { InterlockedDecrement(&g_clients); closesocket((SOCKET)s); continue; }
        acc = (RAccept*)malloc(sizeof(RAccept));
        if (!acc) { InterlockedDecrement(&g_clients); closesocket((SOCKET)s); continue; }
        acc->sock = s; acc->over_limit = (n > R_MAX_CLIENTS);
        th = CreateThread(NULL, 0, client_thread, acc, 0, NULL);
        if (th) CloseHandle(th);
        else { free(acc); InterlockedDecrement(&g_clients); closesocket((SOCKET)s); }
    }
    return 0;
}

static int r_start(void)
{
    s_ts = (int64_t*)malloc(sizeof(int64_t) * S_TMP); s_v = (double*)malloc(sizeof(double) * S_TMP); s_rec = (Rec*)malloc(sizeof(Rec) * S_TMP);
    if (!s_ts || !s_v || !s_rec) return OS_ERR_FAIL;
    g_listen = os_ws_listen(g_host, g_port);
    if (g_listen < 0) {
        if (g_fw && g_fw->log) g_fw->log(OS_LOG_ERROR, "remote: 监听 %s:%d 失败（端口被占用？）", g_host, g_port);
        return OS_ERR_FAIL;
    }
    InterlockedExchange(&g_clients, 0);
    InterlockedExchange(&g_running, 1);
    g_thread = CreateThread(NULL, 0, listen_thread, NULL, 0, NULL);
    g_pump = CreateThread(NULL, 0, pump_thread, NULL, 0, NULL);
    if (!g_thread || !g_pump) { InterlockedExchange(&g_running, 0); os_ws_close_listen(g_listen); g_listen = -1; return OS_ERR_FAIL; }
    if (g_fw && g_fw->log) {
        g_fw->log(OS_LOG_INFO, "remote: ORP v1 服务端监听 ws://%s:%d（device_id=%s）", g_host, g_port, g_device_id);
        if (!strcmp(g_host, "0.0.0.0")) g_fw->log(OS_LOG_WARN, "remote: 局域网监听，必须配对后才能访问");
        if (g_auto_elevate) g_fw->log(OS_LOG_WARN, "remote: 已开启【仅测试】权限提升自动确认");
    }
    new_pair_code();
    return OS_ERR_OK;
}

static void r_stop(void)
{
    int spin, i;
    InterlockedExchange(&g_running, 0);
    if (g_listen >= 0) { os_ws_close_listen(g_listen); g_listen = -1; }
    EnterCriticalSection(&g_cs);
    for (i = 0; i < R_MAX_CLIENTS + R_MAX_PENDING; i++) if (g_live[i]) os_ws_shutdown(g_live[i]->ws);
    LeaveCriticalSection(&g_cs);
    if (g_thread) { WaitForSingleObject(g_thread, 3000); CloseHandle(g_thread); g_thread = NULL; }
    if (g_pump) { WaitForSingleObject(g_pump, 3000); CloseHandle(g_pump); g_pump = NULL; }
    for (spin = 0; spin < 500 && InterlockedCompareExchange(&g_clients, 0, 0) != 0; spin++) Sleep(10);
    EnterCriticalSection(&g_cs);
    for (i = 0; i < R_MAX_SESS; i++) if (g_sess[i].used && g_sess[i].conn) { rc_unref(g_sess[i].conn); g_sess[i].conn = NULL; }
    LeaveCriticalSection(&g_cs);
}

/* 命令行开关（模块自行解析，无需改主程序）：
 *   --remote-port=N  --remote-host=127.0.0.1|0.0.0.0  --remote-off  --remote-auto-elevate（仅测试）
 * 转为同名环境变量后按环境变量逻辑处理；命令行优先于已有环境变量。 */
static void apply_cmdline_flags(void)
{
    int argc = 0, i;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return;
    for (i = 1; i < argc; i++) {
        const wchar_t* a = argv[i];
        char v[64];
        if (!wcsncmp(a, L"--remote-port=", 14) || !wcsncmp(a, L"--remote-host=", 14)) {
            WideCharToMultiByte(CP_UTF8, 0, a + 14, -1, v, sizeof(v), NULL, NULL);
            SetEnvironmentVariableA(a[9] == L'p' ? "OPENSCOPE_REMOTE_PORT" : "OPENSCOPE_REMOTE_HOST", v);
        } else if (!wcscmp(a, L"--remote-off")) SetEnvironmentVariableA("OPENSCOPE_REMOTE_DISABLE", "1");
        else if (!wcscmp(a, L"--remote-auto-elevate")) SetEnvironmentVariableA("OPENSCOPE_REMOTE_AUTO_ELEVATE", "1");
    }
    LocalFree(argv);
}

static int r_init(const OS_Framework* fw, void** out_ctx)
{
    char env[64], d[8];
    char* q;
    g_fw = fw;
    *out_ctx = (void*)1;
    if (!fw || fw->api_version < 5 || !fw->ai_call) return OS_ERR_FAIL;
    InitializeCriticalSection(&g_cs); g_cs_ready = 1;
    apply_cmdline_flags();
    if (GetEnvironmentVariableA("OPENSCOPE_REMOTE_DISABLE", env, sizeof(env)) > 0 && env[0] == '1') {
        if (fw->log) fw->log(OS_LOG_INFO, "remote: 远程调试服务已禁用（OPENSCOPE_REMOTE_DISABLE=1）");
        return OS_ERR_OK;
    }
    if (GetEnvironmentVariableA("OPENSCOPE_REMOTE_PORT", env, sizeof(env)) > 0) { int p = atoi(env); if (p > 0 && p < 65536) g_port = p; }
    if (GetEnvironmentVariableA("OPENSCOPE_REMOTE_HOST", env, sizeof(env)) > 0) {
        if (!strcmp(env, "0.0.0.0") || !strcmp(env, "127.0.0.1")) strcpy(g_host, env);
        else if (fw->log) fw->log(OS_LOG_WARN, "remote: OPENSCOPE_REMOTE_HOST 仅支持 127.0.0.1 / 0.0.0.0，已忽略");
    }
    if (GetEnvironmentVariableA("OPENSCOPE_REMOTE_AUTO_ELEVATE", env, sizeof(env)) > 0 && env[0] == '1') {
        if (!strcmp(g_host, "127.0.0.1")) g_auto_elevate = 1;
        else if (fw->log) fw->log(OS_LOG_WARN, "remote: 局域网监听时不允许自动确认，已忽略 OPENSCOPE_REMOTE_AUTO_ELEVATE");
    }
    if (GetEnvironmentVariableA("OPENSCOPE_REMOTE_PAIR_CODE", env, sizeof(env)) == 6) {
        int ok = 1, i;
        for (i = 0; i < 6; i++) if (env[i] < '0' || env[i] > '9') ok = 0;
        if (ok) { memcpy(g_fixed_code, env, 7); if (fw->log) fw->log(OS_LOG_WARN, "remote: 使用【仅测试】固定配对码"); }
    }
    rand_hex(d, 3);
    _snprintf(g_device_id, sizeof(g_device_id), "OS-%s", d);
    for (q = g_device_id; *q; q++) *q = (char)toupper((unsigned char)*q);
    if (r_start() != OS_ERR_OK) {
        /* 端口被占用等：不影响主程序其他功能，模块仍加载但不提供服务 */
        return OS_ERR_OK;
    }
    return OS_ERR_OK;
}

static void r_deinit(void* ctx)
{
    int i;
    (void)ctx;
    if (!g_cs_ready) return;
    r_stop();
    for (i = 0; i < R_MAX_TRK; i++) { free(g_trk[i].ts); free(g_trk[i].v); }
    memset(g_trk, 0, sizeof(g_trk));
    memset(g_sess, 0, sizeof(g_sess));
    free(g_vars); g_vars = NULL; g_nvars = g_vars_cap = 0;
    free(s_ts); free(s_v); free(s_rec); s_ts = NULL; s_v = NULL; s_rec = NULL;
    DeleteCriticalSection(&g_cs); g_cs_ready = 0;
    g_fw = NULL;
}

static int r_command(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx; (void)in;
    if (cmd == OS_CMD_AI_DESCRIBE && out) { ((char*)out)[0] = 0; return 0; }
    if (cmd == OS_CMD_GET_INFO && out) {
        OS_DriverInfo* info = (OS_DriverInfo*)out;
        memset(info, 0, sizeof(*info));
        _snprintf(info->name, sizeof(info->name), "remote");
        _snprintf(info->version, sizeof(info->version), "1.0");
        info->connected = g_running ? 1 : 0;
        return OS_ERR_OK;
    }
    return OS_ERR_FAIL;
}

static const OS_Module g_mod = {
    OS_API_VERSION,
    OS_CAP_AI,
    "remote",
    "1.0.0",
    "远程调试服务端 ORP v1（WebSocket，默认 127.0.0.1:18766，配对码+token）",
    NULL,
    r_init,
    r_deinit,
    r_command,
    NULL, NULL, NULL, NULL, NULL, NULL
};

OS_MODULE_EXPORT const OS_Module* os_module_get(void)
{
    return &g_mod;
}
