/* tests/conn_async/test_conn_race：AI 线程连接 vs UI 连接（第四轮 任务2）
 * code/src/conn_async.c 的 claim/active/connect_sync。UI 侧按 mainwin 的做法（claim → os_conn_start →
 * set_active → 完成消息 → set_active(NULL) → settle → release），AI 侧调 os_conn_connect_sync。
 * 假驱动统计 CONNECT 并发（必须 ≤1）、会话平衡；DISCONNECT 清 want = 取消点。计时 QPC 实测。 */
#include "pre_conn.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "conn_async.h"

static int g_fail;
#define CHECK(c, ...) do { if (!(c)) { printf("  FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); g_fail++; } } while (0)
static double now_ms(void) { LARGE_INTEGER f, c; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&c); return (double)c.QuadPart * 1000.0 / (double)f.QuadPart; }
static LONG L(volatile LONG* p) { return InterlockedCompareExchange(p, 0, 0); }

/* ---- 假驱动 ---- */
static volatile LONG g_delay_ms, g_again_n, g_want, g_conn, g_open, g_close, g_inside, g_max_inside, g_connects;
static int fake_cmd(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx; (void)in;
    switch (cmd) {
    case OS_CMD_CONNECT: {
        double t0 = now_ms(); LONG n = InterlockedIncrement(&g_inside), m;
        while ((m = L(&g_max_inside)) < n && InterlockedCompareExchange(&g_max_inside, n, m) != m) {}
        InterlockedIncrement(&g_connects);
        if (L(&g_again_n) > 0) { InterlockedDecrement(&g_again_n); InterlockedDecrement(&g_inside); return OS_ERR_AGAIN; }
        InterlockedExchange(&g_want, 1);
        while (now_ms() - t0 < L(&g_delay_ms)) {
            Sleep(2);
            if (!L(&g_want)) { InterlockedDecrement(&g_inside); return OS_ERR_CANCELED; }
        }
        InterlockedIncrement(&g_open); InterlockedExchange(&g_conn, 1);
        InterlockedDecrement(&g_inside);
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

static volatile LONG g_pins, g_unpins;
BOOL fake_GetModuleHandleExW(DWORD fl, LPCWSTR a, HMODULE* out) { (void)fl; (void)a; InterlockedIncrement(&g_pins); *out = (HMODULE)&g_mod; return TRUE; }
BOOL fake_FreeLibrary(HMODULE h) { (void)h; InterlockedIncrement(&g_unpins); return TRUE; }
#ifndef _WIN32
HMODULE LoadLibraryExA(const char* p, HANDLE f, DWORD fl) { (void)p; (void)f; (void)fl; return NULL; }
HMODULE LoadLibraryA(const char* p) { (void)p; return NULL; }
FARPROC GetProcAddress(HMODULE h, const char* n) { (void)h; (void)n; return NULL; }
#endif

/* ---- 假 UI 消息队列 ---- */
static CRITICAL_SECTION g_qcs;
static OS_ConnJob* g_q[64];
static int g_qn;
#define FAKE_HWND ((HWND)(intptr_t)0x1234)
#define WM_DONE (0x8000 + 61)
BOOL fake_PostMessageW(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    (void)wp;
    if (h != FAKE_HWND || msg != WM_DONE) return FALSE;
    EnterCriticalSection(&g_qcs); if (g_qn < 64) g_q[g_qn++] = (OS_ConnJob*)lp; LeaveCriticalSection(&g_qcs);
    return TRUE;
}
static OS_ConnJob* wait_msg(double ms)
{
    double t0 = now_ms(); OS_ConnJob* j = NULL;
    for (;;) {
        EnterCriticalSection(&g_qcs);
        if (g_qn) { j = g_q[0]; memmove(g_q, g_q + 1, sizeof(g_q[0]) * 63); g_qn--; }
        LeaveCriticalSection(&g_qcs);
        if (j || now_ms() - t0 >= ms) return j;
        Sleep(1);
    }
}

static OS_ConnectCfg g_cfg;
/* mainwin 的 UI 连接：claim 失败 = 忽略点击（返回 NULL） */
static OS_ConnJob* ui_connect(void)
{
    OS_ConnJob* j;
    if (!os_conn_claim(OS_CONN_OWNER_UI)) return NULL;
    j = os_conn_start(&g_mod, NULL, &g_cfg, 0, 0, FAKE_HWND, WM_DONE);
    os_conn_set_active(j);
    if (!j) os_conn_release(OS_CONN_OWNER_UI);
    return j;
}
static int ui_done(OS_ConnJob* j)   /* on_connect_done */
{
    int rc;
    os_conn_set_active(NULL);
    rc = os_conn_settle(j);
    os_conn_release(OS_CONN_OWNER_UI);
    os_conn_finish(j, 2000);
    return rc;
}
static void ui_disconnect(void)     /* cmd_disconnect 的相关部分 */
{
    os_conn_cancel_active();
    fake_cmd(NULL, OS_CMD_DISCONNECT, NULL, NULL);
}

/* AI 线程：method_connect 的核心 */
typedef struct { OS_ConnResult res; double ms; } AiRun;
static DWORD WINAPI ai_thread(LPVOID p)
{
    AiRun* r = (AiRun*)p; double t0 = now_ms();
    os_conn_connect_sync(OS_CONN_OWNER_AI, &g_mod, NULL, &g_cfg, 0, 0, 15000, &r->res);
    r->ms = now_ms() - t0;
    return 0;
}

/* 同时起跑：两个线程等同一个起跑信号 */
static volatile LONG g_go;
typedef struct { int ui_ok; OS_ConnJob* job; } UiRace;
static DWORD WINAPI ui_race_thread(LPVOID p)
{
    UiRace* u = (UiRace*)p;
    while (!L(&g_go)) {}
    u->job = ui_connect();
    u->ui_ok = u->job != NULL;
    return 0;
}
static DWORD WINAPI ai_race_thread(LPVOID p)
{
    while (!L(&g_go)) {}
    return ai_thread(p);
}

int main(void)
{
    AiRun ar; HANDLE th; OS_ConnJob* j; OS_ConnJob* m; double t0, dt; int i, ui_wins = 0, ai_wins = 0;
    setvbuf(stdout, NULL, _IONBF, 0);
    InitializeCriticalSection(&g_qcs);
    g_mod.command = fake_cmd;
    memset(&g_cfg, 0, sizeof(g_cfg));

    printf("[1] AI 连接在途（300ms）时 UI 点连接 → 立即被忽略；AI 完成后 UI 可连\n");
    InterlockedExchange(&g_delay_ms, 300);
    memset(&ar, 0, sizeof(ar));
    th = CreateThread(NULL, 0, ai_thread, &ar, 0, NULL);
    Sleep(50);
    t0 = now_ms(); j = ui_connect(); dt = now_ms() - t0;
    printf("    UI claim 结果=%s，耗时 %.2fms；owner=%d\n", j ? "拿到" : "忽略", dt, os_conn_owner());
    CHECK(j == NULL && dt < 5 && os_conn_owner() == OS_CONN_OWNER_AI, "UI must be refused immediately");
    WaitForSingleObject(th, INFINITE); CloseHandle(th);
    printf("    AI 回复 rc=%d connected=%d emulator=%s 用时 %.0fms\n", ar.res.rc, ar.res.connected, ar.res.info.emulator, ar.ms);
    CHECK(ar.res.rc == OS_ERR_OK && ar.res.connected == 1 && strcmp(ar.res.info.emulator, "FAKE") == 0, "AI connect result");
    CHECK(os_conn_owner() == OS_CONN_OWNER_NONE, "slot not released");
    fake_cmd(NULL, OS_CMD_DISCONNECT, NULL, NULL);
    j = ui_connect(); CHECK(j != NULL, "UI connect after AI done");
    m = wait_msg(2000); CHECK(m == j && ui_done(m) == OS_ERR_OK, "UI connect ok");
    fake_cmd(NULL, OS_CMD_DISCONNECT, NULL, NULL);

    printf("[2] UI 连接在途时 AI connect → 立即回 AGAIN（busy_owner=UI），不等待\n");
    j = ui_connect(); Sleep(30);
    memset(&ar, 0, sizeof(ar));
    t0 = now_ms(); ai_thread(&ar); dt = now_ms() - t0;
    printf("    AI rc=%d busy_owner=%d 耗时 %.2fms\n", ar.res.rc, ar.res.busy_owner, dt);
    CHECK(ar.res.rc == OS_ERR_AGAIN && ar.res.busy_owner == OS_CONN_OWNER_UI && dt < 5, "AI must get AGAIN immediately");
    m = wait_msg(2000); CHECK(m == j && ui_done(m) == OS_ERR_OK, "UI connect ok");
    fake_cmd(NULL, OS_CMD_DISCONNECT, NULL, NULL);

    printf("[3] UI 点断开取消 AI 的连接（CONNECT 2000ms）\n");
    InterlockedExchange(&g_delay_ms, 2000);
    memset(&ar, 0, sizeof(ar));
    th = CreateThread(NULL, 0, ai_thread, &ar, 0, NULL);
    Sleep(100);
    t0 = now_ms(); ui_disconnect();
    WaitForSingleObject(th, INFINITE); CloseHandle(th); dt = now_ms() - t0;
    printf("    AI 回复 rc=%d，断开后 %.1fms 返回\n", ar.res.rc, dt);
    CHECK(ar.res.rc == OS_ERR_CANCELED && dt < 100, "AI connect not cancelled quickly");
    CHECK(L(&g_conn) == 0 && os_conn_owner() == 0, "session/slot left");

    printf("[4] AI disconnect 取消 UI 的连接\n");
    j = ui_connect(); Sleep(100);
    t0 = now_ms(); ui_disconnect();   /* method_disconnect：os_conn_cancel_active + DISCONNECT */
    m = wait_msg(2000); dt = now_ms() - t0;
    CHECK(m == j, "no UI done msg");
    if (m) { int rc = ui_done(m); printf("    UI 连接 rc=%d，%.1fms 后完成\n", rc, dt); CHECK(rc == OS_ERR_CANCELED && dt < 100, "UI connect not cancelled"); }

    printf("[5] 驱动后台重连：AI 收到 3 次 AGAIN 后连上；期间 UI 仍被拒（同一次占用）\n");
    InterlockedExchange(&g_delay_ms, 50); InterlockedExchange(&g_again_n, 3);
    memset(&ar, 0, sizeof(ar));
    th = CreateThread(NULL, 0, ai_thread, &ar, 0, NULL);
    Sleep(300);
    CHECK(ui_connect() == NULL, "UI must be refused during AI AGAIN retries");
    WaitForSingleObject(th, INFINITE); CloseHandle(th);
    printf("    AI rc=%d 用时 %.0fms（3×250ms 重试）\n", ar.res.rc, ar.ms);
    CHECK(ar.res.rc == OS_ERR_OK && ar.ms >= 700, "AGAIN retry path");
    fake_cmd(NULL, OS_CMD_DISCONNECT, NULL, NULL);

    printf("[6] AGAIN 重试等待中被 UI 断开 → CANCELED，不再重试\n");
    InterlockedExchange(&g_again_n, 1000);
    memset(&ar, 0, sizeof(ar));
    th = CreateThread(NULL, 0, ai_thread, &ar, 0, NULL);
    Sleep(400);
    t0 = now_ms(); ui_disconnect();
    WaitForSingleObject(th, INFINITE); CloseHandle(th); dt = now_ms() - t0;
    printf("    AI rc=%d，断开后 %.1fms 返回\n", ar.res.rc, dt);
    CHECK(ar.res.rc == OS_ERR_CANCELED && dt < 100, "AGAIN wait not cancelled");
    InterlockedExchange(&g_again_n, 0);

    printf("[7] UI 与 AI 同时起跑 200 轮：每轮恰好一方拿到，CONNECT 从不并发\n");
    InterlockedExchange(&g_delay_ms, 3);
    for (i = 0; i < 200; i++) {
        UiRace u; HANDLE t1, t2;
        memset(&u, 0, sizeof(u)); memset(&ar, 0, sizeof(ar));
        InterlockedExchange(&g_go, 0);
        t1 = CreateThread(NULL, 0, ui_race_thread, &u, 0, NULL);
        t2 = CreateThread(NULL, 0, ai_race_thread, &ar, 0, NULL);
        InterlockedExchange(&g_go, 1);
        WaitForSingleObject(t1, INFINITE); CloseHandle(t1);
        WaitForSingleObject(t2, INFINITE); CloseHandle(t2);
        if (u.ui_ok) { ui_wins++; m = wait_msg(2000); CHECK(m == u.job && ui_done(m) == OS_ERR_OK, "race ui done"); }
        if (ar.res.rc == OS_ERR_OK) ai_wins++;
        CHECK(!(u.ui_ok && ar.res.rc == OS_ERR_OK), "both won round %d", i);
        CHECK(u.ui_ok || ar.res.rc == OS_ERR_OK, "nobody won round %d (ai rc=%d)", i, ar.res.rc);
        fake_cmd(NULL, OS_CMD_DISCONNECT, NULL, NULL);
    }
    printf("    UI 赢 %d 轮，AI 赢 %d 轮；CONNECT 最大并发=%ld\n", ui_wins, ai_wins, (long)L(&g_max_inside));
    CHECK(L(&g_max_inside) == 1, "CONNECT ran concurrently");

    printf("会话 open=%ld close=%ld  钉住=%ld 放开=%ld  owner=%d\n", (long)L(&g_open), (long)L(&g_close), (long)L(&g_pins), (long)L(&g_unpins), os_conn_owner());
    CHECK(L(&g_open) == L(&g_close), "session imbalance");
    CHECK(L(&g_pins) == L(&g_unpins), "pin imbalance");
    CHECK(os_conn_owner() == 0, "slot leaked");
    printf("RESULT: %s (%d)\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
