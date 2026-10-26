/*
 * 风险4：jlink.c / stlink.c 的驱动侧回归测试（真实驱动源码 + 假厂商 DLL）。
 * 同一份测试，编译时 -DPROBE_JLINK 或 -DPROBE_STLINK 选择驱动与假 DLL（fake_jlink.c / fake_stlink.c）。
 *  A 基线：连接；CPU 状态、断点、寄存器、小块读、HALT/GO、GET_INFO 全部 OK。
 *  B 慢 I/O：采集线程大块读在 DLL 里每次 300ms → UI 命令（含运行控制）≤ MAX_MS 返回（OK 或 AGAIN）。
 *  C 卡住的重连：目标掉线触发后台重连，连接调用卡 N 秒 → 期间 UI ≤ MAX_MS 且只见 AGAIN，采集不被卡住；之后自动恢复。
 *  D 重连中用户断开：≤ MAX_MS 返回，会话不泄漏。
 *  F 连接中取消：工作线程里 CONNECT 卡 1.5s，期间 DISCONNECT ≤ MAX_MS；CONNECT 返回 CANCELED，会话不泄漏。
 *  G 卸载交接：CONNECT 卡 6s 时 deinit → 有界返回；卡住的线程返回后自行收尾（关会话、FreeLibrary），可重新 init。
 * 全程：厂商 DLL 内并发调用数必须 = 1（风险2：所有 DLL 调用都在会话锁/独占区内），不得在无会话/已卸载时调用。
 * 环境变量：TEST_STUCK_MS（默认 3000）、TEST_MAX_MS（默认 60）、TEST_VERBOSE。
 */
#include "pre_probe.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "module_api.h"
#include "fake_probe.h"

#if defined(PROBE_JLINK)
#define DRV_NAME "jlink"
#elif defined(PROBE_STLINK)
#define DRV_NAME "stlink"
#else
#error "define PROBE_JLINK or PROBE_STLINK"
#endif

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

static int cmd(int c, void* in, void* out) { return M->command(NULL, c, in, out); }

typedef struct { int ok, again, other, n, max_k; long max_ms; } Stat;
#define UI_CMDS 8
static void ui_round(Stat* st)
{
    OS_CpuState cs; OS_BpInfo items[OS_BP_MAX]; OS_BpList bl; OS_RegSet rs; OS_BpReq br; OS_MemReq mr; OS_DriverInfo di;
    uint8_t buf[8]; int rc, k; ULONGLONG t0, dt;
    memset(&bl, 0, sizeof(bl)); bl.items = items; bl.capacity = OS_BP_MAX;
    memset(&br, 0, sizeof(br)); br.address = 0x08000100u;
    memset(&mr, 0, sizeof(mr)); mr.address = 0x20000000u; mr.size = 4; mr.data = buf;
    for (k = 0; k < UI_CMDS; k++) {
        t0 = now_ms();
        switch (k) {
        case 0: rc = cmd(OS_CMD_GET_CPU_STATE, NULL, &cs); break;
        case 1: rc = cmd(OS_CMD_BP_LIST, NULL, &bl); break;
        case 2: rc = cmd(OS_CMD_BP_SET, &br, NULL); break;
        case 3: rc = cmd(OS_CMD_REG_READ, NULL, &rs); break;
        case 4: rc = cmd(OS_CMD_READ_MEM, &mr, NULL); if (rc > 0) rc = OS_ERR_OK; break;
        case 5: rc = cmd(OS_CMD_HALT, NULL, NULL); break;
        case 6: rc = cmd(OS_CMD_GO, NULL, NULL); break;
        default: rc = cmd(OS_CMD_GET_INFO, NULL, &di); break;
        }
        dt = now_ms() - t0;
        if ((long)dt > st->max_ms) { st->max_ms = (long)dt; st->max_k = k; }
        st->n++;
        if (rc == OS_ERR_OK) st->ok++; else if (rc == OS_ERR_AGAIN) st->again++;
        else { st->other++; if (getenv("TEST_VERBOSE")) printf("    ui cmd#%d rc=%d\n", k, rc); }
    }
}

static DWORD WINAPI acq_thread(LPVOID p)
{
    static uint8_t big[4096];
    int size = (int)(intptr_t)p;
    while (!InterlockedCompareExchange(&g_stop_acq, 0, 0)) {
        OS_MemReq mr; ULONGLONG t0 = now_ms(); LONG dt; int rc;
        memset(&mr, 0, sizeof(mr)); mr.address = 0x20000000u; mr.size = (uint32_t)size; mr.data = big;
        rc = cmd(OS_CMD_READ_MEM, &mr, NULL);
        dt = (LONG)(now_ms() - t0);
        InterlockedIncrement(&g_acq_calls);
        if (rc == OS_ERR_AGAIN) InterlockedIncrement(&g_acq_again);
        if (dt > InterlockedCompareExchange(&g_acq_max_ms, 0, 0)) InterlockedExchange(&g_acq_max_ms, dt);
        if (rc < 0) Sleep(5);
    }
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

static volatile LONG g_conn_rc = 12345;
static DWORD WINAPI connect_thread(LPVOID p)
{
    OS_ConnectCfg cfg;
    (void)p;
    memset(&cfg, 0, sizeof(cfg)); cfg.probe_index = -1; cfg.speed_khz = 4000;
    InterlockedExchange(&g_conn_rc, cmd(OS_CMD_CONNECT, &cfg, NULL));
    return 0;
}

#define SESS_BALANCED() (FK_GET(g_fk_open) == FK_GET(g_fk_close))

int main(void)
{
    OS_Framework fw; void* ctx = NULL; OS_ConnectCfg cfg; Stat st; HANDLE th;
    long stuck = getenv("TEST_STUCK_MS") ? atol(getenv("TEST_STUCK_MS")) : 3000;
    int rc, c; ULONGLONG t0; LONG run0;
    setvbuf(stdout, NULL, _IONBF, 0);
    if (getenv("TEST_MAX_MS")) g_max_ms = atol(getenv("TEST_MAX_MS"));
    InitializeCriticalSection(&g_log_cs);
    memset(&fw, 0, sizeof(fw)); fw.api_version = OS_API_VERSION; fw.log = fw_log;
    printf("== 驱动 %s\n", DRV_NAME);
    M = os_module_get();
    CHECK(M && M->init(&fw, &ctx) == OS_ERR_OK, "init");
    memset(&cfg, 0, sizeof(cfg)); cfg.probe_index = -1; cfg.speed_khz = 4000;

    printf("[A] 基线\n");
    rc = cmd(OS_CMD_CONNECT, &cfg, NULL); CHECK(rc == OS_ERR_OK, "connect rc=%d", rc);
    cmd(OS_CMD_IS_CONNECTED, NULL, &c); CHECK(c == 1, "IS_CONNECTED after connect");
    run_ui_for(200, &st);
    printf("    ui: n=%d ok=%d again=%d other=%d max=%ldms\n", st.n, st.ok, st.again, st.other, st.max_ms);
    CHECK(st.ok == st.n, "baseline all OK");

    printf("[B] 采集线程大块读在 DLL 内每次 300ms\n");
    FK_SET(g_fk_read_delay_ms, 300);
    run0 = FK_GET(g_fk_run_ctl);
    InterlockedExchange(&g_stop_acq, 0);
    th = CreateThread(NULL, 0, acq_thread, (LPVOID)(intptr_t)4096, 0, NULL);
    Sleep(50);
    run_ui_for(1500, &st);
    InterlockedExchange(&g_stop_acq, 1); WaitForSingleObject(th, INFINITE); CloseHandle(th);
    FK_SET(g_fk_read_delay_ms, 0);
    printf("    ui: n=%d ok=%d again=%d other=%d max=%ldms（命令#%d） (上限 %ldms)；运行控制实际执行 %ld 次\n",
           st.n, st.ok, st.again, st.other, st.max_ms, st.max_k, g_max_ms, (long)(FK_GET(g_fk_run_ctl) - run0));
    CHECK(st.max_ms <= g_max_ms, "UI latency %ld > %ld under slow I/O", st.max_ms, g_max_ms);
    CHECK(st.again > 0, "expected some AGAIN while acq holds lock");
    CHECK(st.other == 0, "no hard errors (other=%d)", st.other);
    CHECK(FK_GET(g_fk_max_inside) == 1, "DLL called concurrently (max inside=%ld)", (long)FK_GET(g_fk_max_inside));

    printf("[C] 卡住的重连（连接调用卡 %ldms）\n", stuck);
    FK_SET(g_fk_open_delay_ms, stuck);
    FK_SET(g_fk_fail, 1);
    InterlockedExchange(&g_stop_acq, 0); InterlockedExchange(&g_acq_max_ms, 0); InterlockedExchange(&g_acq_again, 0); InterlockedExchange(&g_acq_calls, 0);
    th = CreateThread(NULL, 0, acq_thread, (LPVOID)(intptr_t)64, 0, NULL);
    Sleep(150);                    /* 采集读失败 → 触发后台重连 */
    FK_SET(g_fk_fail, 0);          /* 目标“回来了”，但连接调用还要卡 stuck ms */
    run_ui_for(stuck - 400, &st);
    printf("    ui: n=%d ok=%d again=%d other=%d max=%ldms; acq: calls=%ld again=%ld max=%ldms\n",
           st.n, st.ok, st.again, st.other, st.max_ms, (long)FK_GET(g_acq_calls), (long)FK_GET(g_acq_again), (long)FK_GET(g_acq_max_ms));
    CHECK(st.max_ms <= g_max_ms, "UI latency %ld > %ld during stuck reconnect", st.max_ms, g_max_ms);
    CHECK(st.again > 0 && st.ok == 0 && st.other == 0, "UI should see only AGAIN while reconnecting (ok=%d again=%d other=%d)", st.ok, st.again, st.other);
    CHECK(FK_GET(g_acq_max_ms) <= g_max_ms, "acq thread blocked %ldms", (long)FK_GET(g_acq_max_ms));
    t0 = now_ms();
    CHECK(wait_cpu_ok(stuck + 3000), "no recovery after stuck reconnect");
    printf("    恢复：%llums 内 CPU 状态 OK\n", (unsigned long long)(now_ms() - t0));
    InterlockedExchange(&g_stop_acq, 1); WaitForSingleObject(th, INFINITE); CloseHandle(th);
    FK_SET(g_fk_open_delay_ms, 0);
    run_ui_for(200, &st);
    CHECK(st.ok == st.n, "after recovery all OK (ok=%d n=%d other=%d)", st.ok, st.n, st.other);

    printf("[D] 重连中用户断开\n");
    FK_SET(g_fk_open_delay_ms, 1500);
    FK_SET(g_fk_fail, 1);
    { uint8_t b[4]; OS_MemReq mr; memset(&mr, 0, sizeof(mr)); mr.address = 0x20000000u; mr.size = 4; mr.data = b;
      Sleep(600);  /* 越过 500ms 重连节流 */
      rc = cmd(OS_CMD_READ_MEM, &mr, NULL); CHECK(rc == OS_ERR_AGAIN || rc == OS_ERR_TIMEOUT, "read during fail rc=%d", rc); }
    FK_SET(g_fk_fail, 0);
    Sleep(100);
    t0 = now_ms();
    rc = cmd(OS_CMD_DISCONNECT, NULL, NULL);
    printf("    disconnect rc=%d 耗时 %llums\n", rc, (unsigned long long)(now_ms() - t0));
    CHECK(rc == OS_ERR_OK && (long)(now_ms() - t0) <= g_max_ms, "disconnect must not wait for reconnect");
    cmd(OS_CMD_IS_CONNECTED, NULL, &c);
    CHECK(c == 0, "IS_CONNECTED must be 0 right after user disconnect");
    Sleep(2000);
    cmd(OS_CMD_IS_CONNECTED, NULL, &c);
    CHECK(c == 0, "still connected after user disconnect");
    printf("    sessions: open=%ld close=%ld\n", (long)FK_GET(g_fk_open), (long)FK_GET(g_fk_close));
    CHECK(SESS_BALANCED(), "session leak");

    printf("[F] 连接中取消（工作线程里 CONNECT，连接调用卡 1500ms）\n");
    FK_SET(g_fk_open_delay_ms, 1500);
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
    printf("    connect rc=%ld（期望 %d = CANCELED）\n", (long)FK_GET(g_conn_rc), OS_ERR_CANCELED);
    CHECK(FK_GET(g_conn_rc) == OS_ERR_CANCELED, "connect should be canceled");
    cmd(OS_CMD_IS_CONNECTED, NULL, &c);
    CHECK(c == 0, "connected after cancel");
    CHECK(SESS_BALANCED(), "session leak after cancel (open=%ld close=%ld)", (long)FK_GET(g_fk_open), (long)FK_GET(g_fk_close));
    FK_SET(g_fk_open_delay_ms, 0);
    rc = cmd(OS_CMD_CONNECT, &cfg, NULL); CHECK(rc == OS_ERR_OK, "connect after cancel rc=%d", rc);
    cmd(OS_CMD_DISCONNECT, NULL, NULL);

    printf("[H] 第五轮：采集读占锁（300ms）时断开 → Close 延后到锁释放，不泄漏会话、不清空断点就断开\n");
    {
        OS_BpReq br; LONG cl0, fr0, bp0; ULONGLONG td;
        rc = cmd(OS_CMD_CONNECT, &cfg, NULL); CHECK(rc == OS_ERR_OK, "H connect rc=%d", rc);
        bp0 = FK_GET(g_fk_bp_live);
        memset(&br, 0, sizeof(br)); br.address = 0x08000200u;
        rc = cmd(OS_CMD_BP_SET, &br, NULL); CHECK(rc == OS_ERR_OK, "H bp set rc=%d", rc);
        CHECK(FK_GET(g_fk_bp_live) == bp0 + 1, "H bp not live (%ld)", (long)FK_GET(g_fk_bp_live));
        FK_SET(g_fk_read_delay_ms, 300);
        InterlockedExchange(&g_stop_acq, 0);
        th = CreateThread(NULL, 0, acq_thread, (LPVOID)(intptr_t)4096, 0, NULL);
        Sleep(60);                                   /* 读已在 DLL 内持锁 */
        cl0 = FK_GET(g_fk_close); fr0 = FK_GET(g_fk_free);
        t0 = now_ms();
        rc = cmd(OS_CMD_DISCONNECT, NULL, NULL);
        td = now_ms() - t0;
        CHECK(rc == OS_ERR_OK && (long)td <= g_max_ms, "H disconnect rc=%d took %llums", rc, (unsigned long long)td);
        CHECK(FK_GET(g_fk_close) == cl0, "H Close ran concurrently with in-flight read");
        while (FK_GET(g_fk_close) == cl0 && now_ms() - t0 < 2000) Sleep(1);
        printf("    断开 %llums 返回；Close 在 %llums 后执行（读 300ms），FreeLibrary 变化 %ld，残留断点 %ld\n",
               (unsigned long long)td, (unsigned long long)(now_ms() - t0), (long)(FK_GET(g_fk_free) - fr0),
               (long)(FK_GET(g_fk_bp_live) - bp0));
        CHECK(FK_GET(g_fk_close) == cl0 + 1, "H deferred Close did not run");
        CHECK(FK_GET(g_fk_free) == fr0, "H session ended by FreeLibrary instead of Close");
        CHECK(FK_GET(g_fk_bp_live) == bp0, "H breakpoint left in target after deferred close");
        CHECK((long)(now_ms() - t0) < 1000, "H deferred close too late");
        InterlockedExchange(&g_stop_acq, 1); WaitForSingleObject(th, INFINITE); CloseHandle(th);
        FK_SET(g_fk_read_delay_ms, 0);
        /* 延迟期间立刻重新连接：由连接路径（独占）先执行 Close，再开新会话 */
        rc = cmd(OS_CMD_CONNECT, &cfg, NULL); CHECK(rc == OS_ERR_OK, "H reconnect rc=%d", rc);
        FK_SET(g_fk_read_delay_ms, 300);
        InterlockedExchange(&g_stop_acq, 0);
        th = CreateThread(NULL, 0, acq_thread, (LPVOID)(intptr_t)4096, 0, NULL);
        Sleep(60);
        cl0 = FK_GET(g_fk_close);
        cmd(OS_CMD_DISCONNECT, NULL, NULL);
        InterlockedExchange(&g_stop_acq, 1);
        rc = cmd(OS_CMD_CONNECT, &cfg, NULL);       /* 等读返回（独占排空）→ 先关旧会话 */
        CHECK(rc == OS_ERR_OK, "H connect during deferred close rc=%d", rc);
        CHECK(FK_GET(g_fk_close) == cl0 + 1, "H old session not closed before new open (close %ld)", (long)(FK_GET(g_fk_close) - cl0));
        WaitForSingleObject(th, INFINITE); CloseHandle(th);
        FK_SET(g_fk_read_delay_ms, 0);
        cmd(OS_CMD_DISCONNECT, NULL, NULL);
        CHECK(SESS_BALANCED(), "H session leak (open=%ld close=%ld)", (long)FK_GET(g_fk_open), (long)FK_GET(g_fk_close));
    }

    printf("[G] 卸载交接：CONNECT 卡 6000ms 时 deinit\n");
    {
        LONG fl0;
        FK_SET(g_fk_open_delay_ms, 6000);
        th = CreateThread(NULL, 0, connect_thread, NULL, 0, NULL);
        Sleep(100);                       /* jlink 连接前会整库重载一次：之后才开始计数 */
        fl0 = FK_GET(g_fk_free);
        t0 = now_ms();
        M->deinit(ctx);
        printf("    deinit 耗时 %llums（有界）\n", (unsigned long long)(now_ms() - t0));
        CHECK((long)(now_ms() - t0) <= 5500, "deinit not bounded");
        CHECK(FK_GET(g_fk_free) == fl0, "FreeLibrary while a thread is inside the DLL");
        WaitForSingleObject(th, INFINITE); CloseHandle(th);
        printf("    卡住的线程返回：connect rc=%ld，FreeLibrary %ld 次（期望 1，由该线程收尾）\n",
               (long)FK_GET(g_conn_rc), (long)(FK_GET(g_fk_free) - fl0));
        CHECK(FK_GET(g_fk_free) - fl0 == 1, "deferred cleanup did not run");
        CHECK(SESS_BALANCED(), "session leak after deferred cleanup (open=%ld close=%ld)", (long)FK_GET(g_fk_open), (long)FK_GET(g_fk_close));
        FK_SET(g_fk_open_delay_ms, 0);
        CHECK(M->init(&fw, &ctx) == OS_ERR_OK, "re-init after deferred cleanup");
        rc = cmd(OS_CMD_CONNECT, &cfg, NULL); CHECK(rc == OS_ERR_OK, "connect after re-init rc=%d", rc);
        M->deinit(ctx);
    }

    printf("DLL 内最大并发=%ld（须为 1） 无会话/已卸载调用=%ld  会话 open=%ld close=%ld  Load=%ld Free=%ld\n",
           (long)FK_GET(g_fk_max_inside), (long)FK_GET(g_fk_stale), (long)FK_GET(g_fk_open), (long)FK_GET(g_fk_close),
           (long)FK_GET(g_fk_load), (long)FK_GET(g_fk_free));
    CHECK(FK_GET(g_fk_max_inside) == 1, "vendor DLL calls not serialized");
    CHECK(FK_GET(g_fk_stale) == 0, "DLL called without session / after unload");
    CHECK(SESS_BALANCED(), "session leak at end");
    CHECK(FK_GET(g_fk_load) == FK_GET(g_fk_free), "DLL load/free unbalanced");
    printf(g_fail ? "RESULT: FAIL (%d)\n" : "RESULT: PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
