/*
 * Bug4 驱动侧回归测试：真实 das.c + 假 mcdxdas（fake_mcd.c）。
 *  A 基线：连接、断点、寄存器、CPU 状态都正常。
 *  B 慢 I/O：采集线程大块读每次在锁内卡 300ms → UI 命令必须 ≤ MAX_MS 返回（OK 或 OS_ERR_AGAIN）。
 *  C 卡住的重连：读失败触发后台重连，open_server 卡 N 秒 → 期间 UI 命令 ≤ MAX_MS 返回 AGAIN，
 *    采集线程也不被卡住；N 秒后自动恢复，断点重新布置。
 *  D 重连中用户断开：DISCONNECT ≤ MAX_MS 返回；重连线程收尾，不泄漏会话。
 *  E 卸载：deinit 有界返回。
 *  F 连接中取消：CONNECT 在工作线程里（宿主异步连接），open_server 卡 1.5s；期间 DISCONNECT ≤ MAX_MS 返回，
 *    CONNECT 在下一个取消点返回 OS_ERR_CANCELED，IS_CONNECTED=0，会话不泄漏。
 *  G 卸载交接（风险1）：CONNECT 卡在 open_server 6s 时 deinit → 5s 有界返回（不阻塞宿主）；
 *    卡住的线程返回后自行收尾（关会话、exit_、FreeLibrary），之后可以重新 init。
 * 全程：MCD 调用并发度必须 = 1（MCD 全局锁），不得使用已关闭的 core（stale_use=0）。
 * 环境变量：TEST_STUCK_MS（默认 3000）、TEST_MAX_MS（默认 60）。
 * 时延上限：默认 60ms（50ms 锁等待 + 10ms 余量），所有平台相同。驱动用 QPC 截止 + SwitchToThread/
 * Sleep(0) + 末段自旋等锁，正常机器上超时误差为微秒级。仅当 _WIN32 下开测前的“精度自检”发现
 * 本机让出 CPU 的误差 > 10ms（CPU 被占满 / 虚拟机调度抖动，精确等待无法保证）时，才放宽到 100ms，
 * 并打印 WARN。显式设置 TEST_MAX_MS 时以环境变量为准，不做自检。
 */
#include "pre.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "module_api.h"
#include "fake_mcd.h"

const OS_Module* os_module_get(void);

static const OS_Module* M;
static int g_fail;
static long g_max_ms = 60;
static volatile LONG g_stop_acq, g_acq_calls, g_acq_again, g_acq_max_ms;
static CRITICAL_SECTION g_log_cs;

static ULONGLONG now_ms(void) { LARGE_INTEGER f, c; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&c); return (ULONGLONG)(c.QuadPart / (f.QuadPart / 1000)); }
static void fw_log(int lv, const char* fmt, ...)
{
    va_list ap;
    if (!getenv("TEST_VERBOSE")) return;
    EnterCriticalSection(&g_log_cs);
    va_start(ap, fmt); printf("    [drv%d] ", lv); vprintf(fmt, ap); printf("\n"); va_end(ap);
    LeaveCriticalSection(&g_log_cs);
}
#define CHECK(c, ...) do { if (!(c)) { g_fail++; printf("  FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } } while (0)

#include "precision.h"   /* 第五轮：精度自检改为真实 50ms 等锁 ×10 */

static int cmd(int c, void* in, void* out) { return M->command(NULL, c, in, out); }

typedef struct { int ok, again, other, n, max_k; long max_ms; } Stat;
static void ui_round(Stat* st)
{
    OS_CpuState cs; OS_BpInfo items[OS_BP_MAX]; OS_BpList bl; OS_RegSet rs; OS_BpReq br; OS_MemReq mr;
    uint8_t buf[8]; int rc[5], k; ULONGLONG t0, dt;
    memset(&bl, 0, sizeof(bl)); bl.items = items; bl.capacity = OS_BP_MAX;
    memset(&br, 0, sizeof(br)); br.address = 0x80000100u;
    memset(&mr, 0, sizeof(mr)); mr.address = 0x70000000u; mr.size = 4; mr.data = buf;
    for (k = 0; k < 5; k++) {
        t0 = now_ms();
        switch (k) {
        case 0: rc[k] = cmd(OS_CMD_GET_CPU_STATE, NULL, &cs); break;
        case 1: rc[k] = cmd(OS_CMD_BP_LIST, NULL, &bl); break;
        case 2: rc[k] = cmd(OS_CMD_BP_SET, &br, NULL); break;
        case 3: rc[k] = cmd(OS_CMD_REG_READ, NULL, &rs); break;
        default: rc[k] = cmd(OS_CMD_READ_MEM, &mr, NULL); if (rc[k] > 0) rc[k] = OS_ERR_OK; break;
        }
        dt = now_ms() - t0;
        if ((long)dt > st->max_ms) { st->max_ms = (long)dt; st->max_k = k; }
        st->n++;
        if (rc[k] == OS_ERR_OK) st->ok++; else if (rc[k] == OS_ERR_AGAIN) st->again++; else st->other++;
    }
}

static DWORD WINAPI acq_thread(LPVOID p)
{
    static uint8_t big[4096];
    int size = (int)(intptr_t)p;
    while (!InterlockedCompareExchange(&g_stop_acq, 0, 0)) {
        OS_MemReq mr; ULONGLONG t0 = now_ms(); LONG dt; int rc;
        memset(&mr, 0, sizeof(mr)); mr.address = 0x70000000u; mr.size = (uint32_t)size; mr.data = big;
        rc = cmd(OS_CMD_READ_MEM, &mr, NULL);
        dt = (LONG)(now_ms() - t0);
        InterlockedIncrement(&g_acq_calls);
        if (rc == OS_ERR_AGAIN) InterlockedIncrement(&g_acq_again);
        if (dt > InterlockedCompareExchange(&g_acq_max_ms, 0, 0)) InterlockedExchange(&g_acq_max_ms, dt);
        if (rc < 0) Sleep(5);
    }
    return 0;
}

static volatile LONG g_conn_rc = 12345;
static DWORD WINAPI connect_thread(LPVOID p)
{
    OS_ConnectCfg cfg;
    (void)p;
    memset(&cfg, 0, sizeof(cfg));
    InterlockedExchange(&g_conn_rc, cmd(OS_CMD_CONNECT, &cfg, NULL));
    return 0;
}

static void run_ui_for(long ms, Stat* st)
{
    ULONGLONG end = now_ms() + (ULONGLONG)ms;
    memset(st, 0, sizeof(*st));
    while (now_ms() < end) { ui_round(st); Sleep(10); }
}

static int wait_cpu_ok(long ms)
{
    ULONGLONG end = now_ms() + (ULONGLONG)ms;
    OS_CpuState cs;
    while (now_ms() < end) { if (cmd(OS_CMD_GET_CPU_STATE, NULL, &cs) == OS_ERR_OK) return 1; Sleep(20); }
    return 0;
}

int main(void)
{
    OS_Framework fw; void* ctx = NULL; OS_ConnectCfg cfg; Stat st; HANDLE th;
    long stuck = getenv("TEST_STUCK_MS") ? atol(getenv("TEST_STUCK_MS")) : 3000;
    int rc, c; ULONGLONG t0; LONG trig0;
    setvbuf(stdout, NULL, _IONBF, 0);
    prec_pick_limit(&g_max_ms);
    InitializeCriticalSection(&g_log_cs);
    memset(&fw, 0, sizeof(fw)); fw.api_version = OS_API_VERSION; fw.log = fw_log;
    M = os_module_get();
    CHECK(M && M->init(&fw, &ctx) == OS_ERR_OK, "init");
    memset(&cfg, 0, sizeof(cfg));
    printf("[A] 基线\n");
    rc = cmd(OS_CMD_CONNECT, &cfg, NULL); CHECK(rc == OS_ERR_OK, "connect rc=%d", rc);
    run_ui_for(200, &st);
    printf("    ui: n=%d ok=%d again=%d other=%d max=%ldms\n", st.n, st.ok, st.again, st.other, st.max_ms);
    CHECK(st.ok == st.n, "baseline all OK");

    printf("[B] 采集线程锁内慢 I/O（每次 300ms）\n");
    FAKE_SET(g_fake_tx_delay_ms, 300);
    InterlockedExchange(&g_stop_acq, 0);
    th = CreateThread(NULL, 0, acq_thread, (LPVOID)(intptr_t)4096, 0, NULL);
    Sleep(50);
    run_ui_for(1500, &st);
    InterlockedExchange(&g_stop_acq, 1); WaitForSingleObject(th, INFINITE); CloseHandle(th);
    FAKE_SET(g_fake_tx_delay_ms, 0);
    printf("    ui: n=%d ok=%d again=%d other=%d max=%ldms（命令#%d） (上限 %ldms)\n", st.n, st.ok, st.again, st.other, st.max_ms, st.max_k, g_max_ms);
    CHECK(st.max_ms <= g_max_ms, "UI latency %ld > %ld under slow I/O", st.max_ms, g_max_ms);
    CHECK(st.again > 0, "expected some AGAIN while acq holds lock");
    CHECK(st.other == 0, "no hard errors (other=%d)", st.other);

    printf("[C] 卡住的重连（open_server 卡 %ldms）\n", stuck);
    trig0 = FAKE_GET(g_fake_trig_create);
    FAKE_SET(g_fake_open_delay_ms, stuck);
    FAKE_SET(g_fake_fail_tx, 1);
    InterlockedExchange(&g_stop_acq, 0); InterlockedExchange(&g_acq_max_ms, 0); InterlockedExchange(&g_acq_again, 0);
    th = CreateThread(NULL, 0, acq_thread, (LPVOID)(intptr_t)64, 0, NULL);
    Sleep(100);                       /* 采集读失败 → 触发后台重连 */
    FAKE_SET(g_fake_fail_tx, 0);      /* 目标“回来了”，但 open_server 还要卡 stuck ms */
    run_ui_for(stuck - 300, &st);
    printf("    ui: n=%d ok=%d again=%d other=%d max=%ldms; acq: calls=%ld again=%ld max=%ldms\n",
           st.n, st.ok, st.again, st.other, st.max_ms, (long)FAKE_GET(g_acq_calls), (long)FAKE_GET(g_acq_again), (long)FAKE_GET(g_acq_max_ms));
    CHECK(st.max_ms <= g_max_ms, "UI latency %ld > %ld during stuck reconnect", st.max_ms, g_max_ms);
    CHECK(st.again > 0 && st.ok == 0, "UI should see only AGAIN while reconnecting (ok=%d again=%d)", st.ok, st.again);
    CHECK(FAKE_GET(g_acq_max_ms) <= g_max_ms, "acq thread blocked %ldms", (long)FAKE_GET(g_acq_max_ms));
    t0 = now_ms();
    CHECK(wait_cpu_ok(stuck + 3000), "no recovery after stuck reconnect");
    printf("    恢复：重连结束后 %llums 内 CPU 状态 OK\n", (unsigned long long)(now_ms() - t0));
    InterlockedExchange(&g_stop_acq, 1); WaitForSingleObject(th, INFINITE); CloseHandle(th);
    CHECK(FAKE_GET(g_fake_trig_create) > trig0, "breakpoint not re-armed after reconnect");
    run_ui_for(200, &st);
    CHECK(st.ok == st.n, "after recovery all OK (ok=%d n=%d)", st.ok, st.n);

    printf("[D] 重连中用户断开\n");
    FAKE_SET(g_fake_open_delay_ms, 1500);
    FAKE_SET(g_fake_fail_tx, 1);
    { uint8_t b[4]; OS_MemReq mr; memset(&mr, 0, sizeof(mr)); mr.address = 0x70000000u; mr.size = 4; mr.data = b;
      Sleep(1100);  /* 越过 1s 重连节流 */
      rc = cmd(OS_CMD_READ_MEM, &mr, NULL); CHECK(rc == OS_ERR_AGAIN || rc == OS_ERR_TIMEOUT, "read during fail rc=%d", rc); }
    FAKE_SET(g_fake_fail_tx, 0);
    Sleep(50);
    t0 = now_ms();
    rc = cmd(OS_CMD_DISCONNECT, NULL, NULL);
    printf("    disconnect rc=%d 耗时 %llums\n", rc, (unsigned long long)(now_ms() - t0));
    CHECK(rc == OS_ERR_OK && (long)(now_ms() - t0) <= g_max_ms, "disconnect must not wait for reconnect");
    Sleep(2000);
    cmd(OS_CMD_IS_CONNECTED, NULL, &c);
    CHECK(c == 0, "still connected after user disconnect");
    printf("    sessions: open_srv=%ld close_srv=%ld open_core=%ld close_core=%ld\n",
           (long)FAKE_GET(g_fake_open_srv), (long)FAKE_GET(g_fake_close_srv), (long)FAKE_GET(g_fake_open_core), (long)FAKE_GET(g_fake_close_core));
    CHECK(FAKE_GET(g_fake_open_srv) == FAKE_GET(g_fake_close_srv) && FAKE_GET(g_fake_open_core) == FAKE_GET(g_fake_close_core),
          "session leak");

    printf("[E] 卸载\n");
    t0 = now_ms();
    M->deinit(ctx);
    printf("    deinit 耗时 %llums\n", (unsigned long long)(now_ms() - t0));

    printf("[F] 连接中取消（宿主工作线程里 CONNECT，open_server 卡 1500ms）\n");
    CHECK(M->init(&fw, &ctx) == OS_ERR_OK, "re-init");
    FAKE_SET(g_fake_open_delay_ms, 1500);
    InterlockedExchange(&g_conn_rc, 12345);
    th = CreateThread(NULL, 0, connect_thread, NULL, 0, NULL);
    Sleep(100);
    run_ui_for(200, &st);
    CHECK(st.max_ms <= g_max_ms && st.ok == 0, "UI during connect: max=%ld ok=%d", st.max_ms, st.ok);
    t0 = now_ms();
    rc = cmd(OS_CMD_DISCONNECT, NULL, NULL);
    printf("    连接中 disconnect rc=%d 耗时 %llums\n", rc, (unsigned long long)(now_ms() - t0));
    CHECK(rc == OS_ERR_OK && (long)(now_ms() - t0) <= g_max_ms, "disconnect during connect must not wait");
    WaitForSingleObject(th, INFINITE); CloseHandle(th);
    printf("    connect rc=%ld（期望 %d = CANCELED）\n", (long)FAKE_GET(g_conn_rc), OS_ERR_CANCELED);
    CHECK(FAKE_GET(g_conn_rc) == OS_ERR_CANCELED, "connect should be canceled");
    cmd(OS_CMD_IS_CONNECTED, NULL, &c);
    CHECK(c == 0, "connected after cancel");
    CHECK(FAKE_GET(g_fake_open_srv) == FAKE_GET(g_fake_close_srv) && FAKE_GET(g_fake_open_core) == FAKE_GET(g_fake_close_core),
          "session leak after cancel");
    FAKE_SET(g_fake_open_delay_ms, 0);
    rc = cmd(OS_CMD_CONNECT, &cfg, NULL); CHECK(rc == OS_ERR_OK, "connect after cancel rc=%d", rc);
    cmd(OS_CMD_DISCONNECT, NULL, NULL);

    printf("[G] 卸载交接：CONNECT 卡在 open_server 6000ms 时 deinit\n");
    {
        LONG fl0 = FAKE_GET(g_fake_free_lib);
        FAKE_SET(g_fake_open_delay_ms, 6000);
        th = CreateThread(NULL, 0, connect_thread, NULL, 0, NULL);
        Sleep(100);
        t0 = now_ms();
        M->deinit(ctx);
        printf("    deinit 耗时 %llums（有界，不等卡住的线程）\n", (unsigned long long)(now_ms() - t0));
        CHECK((long)(now_ms() - t0) <= 5500, "deinit not bounded");
        CHECK(FAKE_GET(g_fake_free_lib) == fl0, "FreeLibrary while a thread is inside the DLL");
        WaitForSingleObject(th, INFINITE); CloseHandle(th);
        printf("    卡住的线程返回：connect rc=%ld，FreeLibrary %ld 次（期望 1，由该线程收尾）\n",
               (long)FAKE_GET(g_conn_rc), (long)(FAKE_GET(g_fake_free_lib) - fl0));
        CHECK(FAKE_GET(g_fake_free_lib) - fl0 == 1, "deferred cleanup did not run");
        CHECK(FAKE_GET(g_fake_open_srv) == FAKE_GET(g_fake_close_srv) && FAKE_GET(g_fake_open_core) == FAKE_GET(g_fake_close_core),
              "session leak after deferred cleanup");
        FAKE_SET(g_fake_open_delay_ms, 0);
        CHECK(M->init(&fw, &ctx) == OS_ERR_OK, "re-init after deferred cleanup");
        rc = cmd(OS_CMD_CONNECT, &cfg, NULL); CHECK(rc == OS_ERR_OK, "connect after re-init rc=%d", rc);
        M->deinit(ctx);
    }

    printf("MCD 最大并发=%ld（须为 1） stale_core_use=%ld\n", (long)FAKE_GET(g_fake_max_inside), (long)FAKE_GET(g_fake_stale_use));
    CHECK(FAKE_GET(g_fake_max_inside) == 1, "MCD calls not serialized");
    CHECK(FAKE_GET(g_fake_stale_use) == 0, "used a closed core handle");
    printf(g_fail ? "RESULT: FAIL (%d)\n" : "RESULT: PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
