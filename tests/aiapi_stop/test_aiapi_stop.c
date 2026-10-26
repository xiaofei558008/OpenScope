/* tests/aiapi_stop：aiapi.dll 停止耗时（第四轮 任务3）
 * 真实 module/aiapi/aiapi.c + module/network/netws.c，假框架 ai_call（方法 "slow" 按 params.ms 睡眠）。
 * 场景：0/1/2/3 个空闲客户端；半开连接（未握手）+ 空闲客户端；3 个请求中（400ms）；1 个请求中（5s，超期限）。
 * 计时一律用 QPC 测实际经过时间（Windows Sleep 粒度 15.6ms，不数调用次数）。 */
#include "pre_ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "module_api.h"
#include "netws.h"

#ifndef _WIN32
#include <signal.h>
#endif

static int g_fail;
#define CHECK(c, ...) do { if (!(c)) { printf("  FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); g_fail++; } } while (0)
static volatile LONG g_pins, g_unpins, g_in_call, g_calls_done;
static LONG L(volatile LONG* p) { return InterlockedCompareExchange(p, 0, 0); }

BOOL fake_GetModuleHandleExW(DWORD fl, LPCWSTR addr, HMODULE* out)
{ (void)fl; (void)addr; InterlockedIncrement(&g_pins); *out = (HMODULE)(intptr_t)0x5151; return TRUE; }
BOOL fake_FreeLibrary(HMODULE h) { (void)h; InterlockedIncrement(&g_unpins); return TRUE; }
#ifndef _WIN32
HMODULE LoadLibraryExA(const char* p, HANDLE f, DWORD fl) { (void)p; (void)f; (void)fl; return NULL; }
HMODULE LoadLibraryA(const char* p) { (void)p; return NULL; }
FARPROC GetProcAddress(HMODULE h, const char* n) { (void)h; (void)n; return NULL; }
#endif

static double now_ms(void) { LARGE_INTEGER f, c; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&c); return (double)c.QuadPart * 1000.0 / (double)f.QuadPart; }

static void fw_log(int level, const char* fmt, ...) { (void)level; (void)fmt; }
static int fake_ai_call(const char* method, const char* params, char* out, int cap)
{
    int ms = 0;
    const char* p;
    InterlockedIncrement(&g_in_call);
    if (strcmp(method, "slow") == 0 && params && (p = strstr(params, "\"ms\"")) != NULL && (p = strchr(p, ':')) != NULL)
        ms = atoi(p + 1);
    if (ms > 0) {   /* 按 QPC 睡够 ms（不依赖 Sleep 粒度） */
        double t0 = now_ms();
        while (now_ms() - t0 < ms) Sleep(5);
    }
    _snprintf(out, cap, "{\"ok\":true}");
    InterlockedDecrement(&g_in_call);
    InterlockedIncrement(&g_calls_done);
    return 0;
}

static OS_Framework g_fw;
static const OS_Module* M;
static void* g_ctx;
static int g_port;

static int pick_port(void)
{
    struct sockaddr_in a; int len = sizeof(a), port;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&a, 0, sizeof(a)); a.sin_family = AF_INET; a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    bind(s, (struct sockaddr*)&a, sizeof(a));
    getsockname(s, (struct sockaddr*)&a, (void*)&len);
    port = ntohs(a.sin_port);
    closesocket(s);
    return port;
}

static int start(void)
{
    char e[32];
    _snprintf(e, sizeof(e), "%d", g_port);
#ifdef _WIN32
    SetEnvironmentVariableA("OPENSCOPE_AI_PORT", e);
#else
    setenv("OPENSCOPE_AI_PORT", e, 1);
#endif
    return M->init(&g_fw, &g_ctx);
}

static SOCKET raw_connect(void)
{
    struct sockaddr_in a;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&a, 0, sizeof(a)); a.sin_family = AF_INET; a.sin_port = htons((u_short)g_port);
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(s, (struct sockaddr*)&a, sizeof(a)) != 0) { closesocket(s); return INVALID_SOCKET; }
    return s;
}

/* 一轮：启动 → n 个空闲客户端 + half 个半开 + busy 个"请求中(busy_ms)" → 计时停止 */
static double run_case(const char* name, int idle, int half, int busy, int busy_ms, double limit_ms)
{
    OS_WSConn* c[8]; SOCKET h[4]; int nc = 0, nh = 0, i;
    double t0, dt; LONG calls0 = L(&g_calls_done);
    CHECK(start() == OS_ERR_OK, "%s: init", name);
    for (i = 0; i < idle + busy; i++) {
        c[nc] = os_ws_connect("127.0.0.1", g_port);
        CHECK(c[nc] != NULL, "%s: ws connect %d", name, i);
        if (c[nc]) nc++;
    }
    for (i = 0; i < half; i++) { h[nh] = raw_connect(); if (h[nh] != INVALID_SOCKET) nh++; }
    /* 先确认空闲连接都已握手进入 recv：发一个快请求并收到回复 */
    for (i = 0; i < nc; i++) {
        uint8_t buf[512]; int op = 0, n;
        os_ws_send_text(c[i], "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"ping\",\"params\":{}}");
        n = os_ws_recv(c[i], buf, sizeof(buf) - 1, &op);
        CHECK(n > 0, "%s: ping reply %d", name, i);
    }
    for (i = 0; i < busy; i++) {
        char req[160];
        _snprintf(req, sizeof(req), "{\"jsonrpc\":\"2.0\",\"id\":2,\"method\":\"slow\",\"params\":{\"ms\":%d}}", busy_ms);
        os_ws_send_text(c[idle + i], req);
    }
    if (busy) { double w = now_ms(); while (L(&g_in_call) < busy && now_ms() - w < 2000) Sleep(1); }
    Sleep(30);   /* 半开连接的握手线程进入 recv */
    t0 = now_ms();
    M->deinit(g_ctx);
    dt = now_ms() - t0;
    printf("  %-34s 停止 %7.1f ms（上限 %.0f）  钉住=%ld 放开=%ld\n", name, dt, limit_ms, (long)L(&g_pins), (long)L(&g_unpins));
    CHECK(dt <= limit_ms, "%s: stop took %.1f ms > %.0f", name, dt, limit_ms);
    for (i = 0; i < nc; i++) os_ws_close(c[i]);
    for (i = 0; i < nh; i++) closesocket(h[i]);
    (void)calls0;
    return dt;
}

int main(void)
{
    double dt;
#ifndef _WIN32
    signal(SIGPIPE, SIG_IGN);
#else
    { WSADATA wd; WSAStartup(MAKEWORD(2, 2), &wd); }
#endif
    setvbuf(stdout, NULL, _IONBF, 0);
    memset(&g_fw, 0, sizeof(g_fw));
    g_fw.api_version = OS_API_VERSION; g_fw.log = fw_log; g_fw.ai_call = fake_ai_call;
    M = os_module_get();
    g_port = pick_port();
    printf("aiapi 停止耗时（端口 %d）\n", g_port);

    run_case("0 个客户端", 0, 0, 0, 0, 300);
    run_case("1 个空闲客户端", 1, 0, 0, 0, 300);
    run_case("2 个空闲客户端", 2, 0, 0, 0, 300);
    run_case("3 个空闲客户端", 3, 0, 0, 0, 300);
    run_case("2 个半开(未握手) + 1 个空闲", 1, 2, 0, 0, 300);
    CHECK(L(&g_pins) == L(&g_unpins), "pin imbalance after idle cases");
    run_case("3 个请求中(400ms)", 0, 0, 3, 400, 400 + 300);
    CHECK(L(&g_pins) == L(&g_unpins), "pin imbalance after busy case");

    printf("[超期限] 1 个请求中(5000ms)：停止应在 2s 期限返回，线程保持钉住，结束后放开\n");
    dt = run_case("1 个请求中(5000ms)", 0, 0, 1, 5000, 2000 + 300);
    CHECK(dt >= 1900, "should wait until the 2s deadline (%.1f)", dt);
    CHECK(L(&g_pins) == L(&g_unpins) + 1, "straggler must keep the DLL pinned (pins=%ld unpins=%ld)", (long)L(&g_pins), (long)L(&g_unpins));
    CHECK(start() == OS_ERR_BUSY, "restart must refuse while straggler runs");
    { double w = now_ms(); while (L(&g_pins) != L(&g_unpins) && now_ms() - w < 6000) Sleep(10); }
    CHECK(L(&g_pins) == L(&g_unpins), "straggler did not release pin");
    printf("  straggler 结束后：钉住=%ld 放开=%ld\n", (long)L(&g_pins), (long)L(&g_unpins));
    run_case("straggler 结束后重启 + 1 个空闲", 1, 0, 0, 0, 300);

    printf("RESULT: %s (%d)\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
