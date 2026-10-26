/* tests/conn_async：code/src/conn_async.c 的桩测试（第三轮 任务4）
 * 假驱动：CONNECT 可配置耗时；DISCONNECT 清 want，CONNECT 每 5ms 检查 want（取消点）→ -6；
 * 可配置"忽略取消"（模拟厂商调用里卡住）。假 GetModuleHandleExW/FreeLibrary 计钉住次数，
 * 假 PostMessageW 把完成消息放进队列（模拟主线程消息循环）。 */
#include "pre_conn.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "conn_async.h"

static int g_fail;
#define CHECK(c, ...) do { if (!(c)) { printf("  FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); g_fail++; } } while (0)

/* ---- 假驱动 ---- */
static volatile LONG g_delay_ms, g_ignore_cancel, g_connect_rc, g_want, g_conn, g_open, g_close, g_in_connect;
static volatile LONG g_pins, g_unpins, g_posts;
static int L(volatile LONG* p) { return (int)InterlockedCompareExchange(p, 0, 0); }

static int fake_cmd(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx; (void)in;
    switch (cmd) {
    case OS_CMD_CONNECT: {
        int waited = 0, d = L(&g_delay_ms);
        InterlockedExchange(&g_want, 1);
        InterlockedExchange(&g_in_connect, 1);
        while (waited < d) {
            Sleep(5); waited += 5;
            if (!L(&g_ignore_cancel) && !L(&g_want)) { InterlockedExchange(&g_in_connect, 0); return OS_ERR_CANCELED; }
        }
        InterlockedExchange(&g_in_connect, 0);
        if (L(&g_connect_rc) != OS_ERR_OK) return L(&g_connect_rc);
        InterlockedIncrement(&g_open); InterlockedExchange(&g_conn, 1);
        return OS_ERR_OK;
    }
    case OS_CMD_DISCONNECT:
        InterlockedExchange(&g_want, 0);
        if (InterlockedExchange(&g_conn, 0)) InterlockedIncrement(&g_close);
        return OS_ERR_OK;
    case OS_CMD_IS_CONNECTED: *(int*)out = L(&g_conn) && L(&g_want); return OS_ERR_OK;
    case OS_CMD_GET_INFO: snprintf(((OS_DriverInfo*)out)->emulator, sizeof(((OS_DriverInfo*)out)->emulator), "FAKE"); return OS_ERR_OK;
    }
    return OS_ERR_UNSUPPORTED;
}
static OS_Module g_mod;

BOOL fake_GetModuleHandleExW(DWORD fl, LPCWSTR addr, HMODULE* out)
{ (void)fl; (void)addr; InterlockedIncrement(&g_pins); *out = (HMODULE)&g_mod; return TRUE; }
BOOL fake_FreeLibrary(HMODULE h) { (void)h; InterlockedIncrement(&g_unpins); return TRUE; }
#ifndef _WIN32
/* Linux shim 声明了这些，conn_async 不用 */
HMODULE LoadLibraryExA(const char* p, HANDLE f, DWORD fl) { (void)p; (void)f; (void)fl; return NULL; }
HMODULE LoadLibraryA(const char* p) { (void)p; return NULL; }
FARPROC GetProcAddress(HMODULE h, const char* n) { (void)h; (void)n; return NULL; }
#endif

/* ---- 假消息队列 ---- */
static CRITICAL_SECTION g_qcs;
static OS_ConnJob* g_q[16];
static volatile LONG g_qn;
#define FAKE_HWND ((HWND)(intptr_t)0x1234)
#define WM_DONE (0x8000 + 61)
BOOL fake_PostMessageW(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    (void)wp;
    if (h != FAKE_HWND || msg != WM_DONE) return FALSE;
    EnterCriticalSection(&g_qcs);
    if (g_qn < 16) g_q[g_qn] = (OS_ConnJob*)lp;
    InterlockedIncrement(&g_qn);
    LeaveCriticalSection(&g_qcs);
    InterlockedIncrement(&g_posts);
    return TRUE;
}
static OS_ConnJob* wait_msg(DWORD ms)  /* 模拟主线程 GetMessage */
{
    LARGE_INTEGER f, a, b; OS_ConnJob* j = NULL;
    QueryPerformanceFrequency(&f); QueryPerformanceCounter(&a);
    for (;;) {
        EnterCriticalSection(&g_qcs);
        if (g_qn > 0) { j = g_q[0]; memmove(g_q, g_q + 1, sizeof(g_q[0]) * 15); g_qn--; }
        LeaveCriticalSection(&g_qcs);
        if (j) return j;
        QueryPerformanceCounter(&b);
        if ((DWORD)((b.QuadPart - a.QuadPart) * 1000 / f.QuadPart) >= ms) return NULL;
        Sleep(1);
    }
}
static double now_ms(void) { LARGE_INTEGER f, c; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&c); return (double)c.QuadPart * 1000.0 / (double)f.QuadPart; }
static void reset(int delay, int ignore, int rc)
{
    InterlockedExchange(&g_delay_ms, delay); InterlockedExchange(&g_ignore_cancel, ignore);
    InterlockedExchange(&g_connect_rc, rc); InterlockedExchange(&g_want, 0); InterlockedExchange(&g_conn, 0);
}

int main(void)
{
    OS_ConnectCfg cfg; OS_ConnJob* j; OS_ConnJob* m; double t0, dt;
    InitializeCriticalSection(&g_qcs);
    g_mod.command = fake_cmd;
    memset(&cfg, 0, sizeof(cfg)); snprintf(cfg.device, sizeof(cfg.device), "STM32F303");

    printf("[1] 正常连接（CONNECT 300ms）\n");
    reset(300, 0, OS_ERR_OK);
    t0 = now_ms();
    j = os_conn_start(&g_mod, NULL, &cfg, 0, 3, FAKE_HWND, WM_DONE);
    dt = now_ms() - t0;
    CHECK(j != NULL, "start failed");
    printf("    start 返回 %.1fms（UI 不阻塞）\n", dt);
    CHECK(dt < 20, "start blocked %.1fms", dt);
    m = wait_msg(3000);
    CHECK(m == j, "no/unknown done message");
    if (m) {
        CHECK(m->rc == OS_ERR_OK && m->connected == 1, "rc=%d conn=%d", m->rc, m->connected);
        CHECK(strcmp(m->info.emulator, "FAKE") == 0, "info not filled");
        CHECK(m->driver_index == 3 && strcmp(m->cfg.device, "STM32F303") == 0, "cfg/index not kept");
        CHECK(os_conn_finish(m, 1000) == 1, "finish timeout");
    }
    fake_cmd(NULL, OS_CMD_DISCONNECT, NULL, NULL);

    printf("[2] 连接中用户断开（CONNECT 2000ms，驱动响应取消）\n");
    reset(2000, 0, OS_ERR_OK);
    j = os_conn_start(&g_mod, NULL, &cfg, 1, 0, FAKE_HWND, WM_DONE);
    Sleep(100);
    t0 = now_ms();
    os_conn_cancel(j); fake_cmd(NULL, OS_CMD_DISCONNECT, NULL, NULL);   /* 即 cmd_disconnect 的做法 */
    m = wait_msg(3000); dt = now_ms() - t0;
    printf("    完成消息 %.1fms 后到达，rc=%d\n", dt, m ? m->rc : 999);
    CHECK(m == j && m->rc == OS_ERR_CANCELED && m->connected == 0, "expected CANCELED");
    CHECK(dt < 200, "cancel too slow %.1fms", dt);
    if (m) os_conn_finish(m, 1000);

    printf("[3] 驱动不响应取消、CONNECT 成功后才看到取消 → 工作线程补 DISCONNECT\n");
    reset(300, 1, OS_ERR_OK);
    j = os_conn_start(&g_mod, NULL, &cfg, 0, 0, FAKE_HWND, WM_DONE);
    Sleep(50);
    os_conn_cancel(j); fake_cmd(NULL, OS_CMD_DISCONNECT, NULL, NULL);
    m = wait_msg(3000);
    CHECK(m == j && m->rc == OS_ERR_CANCELED, "expected CANCELED rc=%d", m ? m->rc : 999);
    if (m) os_conn_finish(m, 1000);
    CHECK(L(&g_conn) == 0, "session left open after cancel");

    printf("[4] 启动即取消（线程还没进 CONNECT）\n");
    reset(0, 0, OS_ERR_OK);
    j = os_conn_start(&g_mod, NULL, &cfg, 0, 0, FAKE_HWND, WM_DONE);
    os_conn_cancel(j);
    m = wait_msg(3000);
    if (m) os_conn_settle(m);   /* 主线程完成处理的第一步 */
    CHECK(m == j && m->rc == OS_ERR_CANCELED && L(&g_conn) == 0, "expected CANCELED, no session");
    if (m) os_conn_finish(m, 1000);

    printf("[5] 驱动后台重连中：CONNECT 返回 AGAIN 原样带回（由 connect_again 重试）\n");
    reset(10, 0, OS_ERR_AGAIN);
    j = os_conn_start(&g_mod, NULL, &cfg, 0, 0, FAKE_HWND, WM_DONE);
    m = wait_msg(3000);
    CHECK(m == j && m->rc == OS_ERR_AGAIN && m->connected == 0, "expected AGAIN");
    if (m) os_conn_finish(m, 1000);

    printf("[6] 退出时 CONNECT 卡死（不响应取消，1500ms）：abandon 有界返回、不再投递、DLL 一直钉住到线程结束\n");
    reset(1500, 1, OS_ERR_OK);
    {
        LONG posts0 = L(&g_posts);
        int r;
        j = os_conn_start(&g_mod, NULL, &cfg, 0, 0, FAKE_HWND, WM_DONE);
        Sleep(50);
        t0 = now_ms();
        r = os_conn_abandon(j, 200);
        dt = now_ms() - t0;
        printf("    abandon 返回 %d，耗时 %.1fms；此时钉住=%d 放开=%d\n", r, dt, L(&g_pins), L(&g_unpins));
        CHECK(r == 0 && dt < 400, "abandon not bounded");
        CHECK(L(&g_pins) == L(&g_unpins) + 1, "DLL must stay pinned while worker is inside");
        Sleep(1700);
        CHECK(L(&g_posts) == posts0, "posted to destroyed window");
        CHECK(L(&g_pins) == L(&g_unpins), "pin not released after worker exit");
        CHECK(L(&g_conn) == 0, "late session not closed (cancelled job must DISCONNECT)");
        /* job 有意泄漏（真实退出路径）；测试里回收以免 ASan 报泄漏 */
        os_conn_finish(j, 1000);
    }

    printf("钉住=%d 放开=%d 会话 open=%d close=%d 投递=%d\n", L(&g_pins), L(&g_unpins), L(&g_open), L(&g_close), L(&g_posts));
    CHECK(L(&g_open) == L(&g_close), "session imbalance");
    CHECK(L(&g_pins) == L(&g_unpins), "pin imbalance");
    printf("RESULT: %s (%d)\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
