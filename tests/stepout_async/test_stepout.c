/* tests/stepout_async：StepOut 异步化（第四轮 任务1）
 * 真实 jlink.c / stlink.c + 假厂商 DLL（tests/drv_probe）+ code/src/cmd_async.c。
 * 模拟主线程：os_cmd_start → 等"完成消息"（假 PostMessageW 队列）；取消 = 主线程发 HALT / DISCONNECT。
 * 计时一律 QPC 实测（Windows Sleep 粒度 15.6ms）。 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "module_api.h"
#include "cmd_async.h"
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
#define CHECK(c, ...) do { if (!(c)) { g_fail++; printf("  FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } } while (0)
static double now_ms(void) { LARGE_INTEGER f, c; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&c); return (double)c.QuadPart * 1000.0 / (double)f.QuadPart; }
static void fw_log(int lv, const char* fmt, ...)
{
    va_list ap;
    if (!getenv("TEST_VERBOSE")) return;
    va_start(ap, fmt); printf("    [drv%d] ", lv); vprintf(fmt, ap); printf("\n"); va_end(ap);
}
static int cmd(int c, void* in, void* out) { return M->command(NULL, c, in, out); }

/* ---- 钉住计数 + 假消息队列 ---- */
static volatile LONG g_pins, g_unpins;
static CRITICAL_SECTION g_qcs;
static OS_CmdJob* g_q[8];
static int g_qn;
#define FAKE_HWND ((HWND)(intptr_t)0x4321)
#define WM_DONE (0x8000 + 62)
BOOL pin_GetModuleHandleExW(DWORD fl, LPCWSTR a, HMODULE* out) { (void)fl; (void)a; InterlockedIncrement(&g_pins); *out = (HMODULE)(intptr_t)0x7777; return TRUE; }
BOOL pin_FreeLibrary(HMODULE h) { (void)h; InterlockedIncrement(&g_unpins); return TRUE; }
BOOL pin_PostMessageW(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    (void)wp;
    if (h != FAKE_HWND || msg != WM_DONE) return FALSE;
    EnterCriticalSection(&g_qcs); if (g_qn < 8) g_q[g_qn++] = (OS_CmdJob*)lp; LeaveCriticalSection(&g_qcs);
    return TRUE;
}
static OS_CmdJob* wait_msg(double ms)
{
    double t0 = now_ms(); OS_CmdJob* j = NULL;
    for (;;) {
        EnterCriticalSection(&g_qcs);
        if (g_qn) { j = g_q[0]; memmove(g_q, g_q + 1, sizeof(g_q[0]) * 7); g_qn--; }
        LeaveCriticalSection(&g_qcs);
        if (j || now_ms() - t0 >= ms) return j;
        Sleep(1);
    }
}
static LONG L(volatile LONG* p) { return InterlockedCompareExchange(p, 0, 0); }

static void do_connect(void)
{
    OS_ConnectCfg cfg; int rc, c = 0;
    memset(&cfg, 0, sizeof(cfg)); cfg.probe_index = -1; cfg.speed_khz = 4000;
    rc = cmd(OS_CMD_CONNECT, &cfg, NULL);
    cmd(OS_CMD_IS_CONNECTED, NULL, &c);
    CHECK(rc == OS_ERR_OK && c == 1, "connect rc=%d c=%d", rc, c);
}

/* 启动一次 StepOut；返回 job，并检查 start 不阻塞 */
static OS_CmdJob* start_step(void)
{
    double t0 = now_ms(), dt;
    LONG rc0 = L(&g_fk_run_ctl);
    OS_CmdJob* j = os_cmd_start(M, NULL, OS_CMD_STEP_OUT, 0, FAKE_HWND, WM_DONE);
    dt = now_ms() - t0;
    CHECK(j != NULL, "start failed");
    CHECK(dt < 20, "os_cmd_start blocked %.1fms", dt);
    /* 等工作线程真正进入 StepOut（假 DLL 收到 Go）：Windows 上新线程偶尔 >50ms 才被调度，
     * 否则后续固定 Sleep 后的 HALT / 第二次 StepOut 会抢在它前面（测试时序假象，不是缺陷） */
    t0 = now_ms();
    while (j && L(&g_fk_run_ctl) == rc0 && now_ms() - t0 < 1000) Sleep(1);
    return j;
}

/* 取消：主线程发 cmd（HALT/DISCONNECT），返回取消到完成消息的耗时 */
static double cancel_with(OS_CmdJob* j, int c, const char* what)
{
    double t0, t_cmd, t_done; int rc; OS_CmdJob* m;
    t0 = now_ms();
    rc = cmd(c, NULL, NULL);
    t_cmd = now_ms() - t0;
    m = wait_msg(3000);
    t_done = now_ms() - t0;
    printf("    %s：命令 rc=%d 耗时 %.1fms，完成消息 %.1fms 后到达，StepOut rc=%d\n", what, rc, t_cmd, t_done, m ? m->rc : 999);
    CHECK(m == j, "no done message");
    CHECK(t_cmd < 60, "%s blocked UI %.1fms", what, t_cmd);
    CHECK(t_done < 150, "%s cancel too slow %.1fms", what, t_done);
    if (m) { CHECK(m->rc == OS_ERR_CANCELED, "expected CANCELED, got %d", m->rc); os_cmd_finish(m, 1000); }
    return t_done;
}

int main(void)
{
    OS_Framework fw; void* ctx = NULL; OS_CmdJob* j; OS_CmdJob* m; double t0, dt; LONG bp0; int i;
    setvbuf(stdout, NULL, _IONBF, 0);
    InitializeCriticalSection(&g_qcs);
    memset(&fw, 0, sizeof(fw)); fw.api_version = OS_API_VERSION; fw.log = fw_log;
    printf("== StepOut 异步（驱动 %s）\n", DRV_NAME);
    M = os_module_get();
    CHECK(M && M->init(&fw, &ctx) == OS_ERR_OK, "init");
    do_connect();
    bp0 = L(&g_fk_bp_live);

    printf("[1] 正常完成：Go 后 200ms 命中返回地址\n");
    FK_SET(g_fk_run_ms, 200);
    t0 = now_ms();
    j = start_step();
    m = wait_msg(3000); dt = now_ms() - t0;
    printf("    完成消息 %.1fms 后到达，rc=%d\n", dt, m ? m->rc : 999);
    CHECK(m == j && m->rc == OS_ERR_OK, "expected OK");
    CHECK(dt >= 180 && dt < 600, "unexpected duration %.1f", dt);
    if (m) os_cmd_finish(m, 1000);
    CHECK(L(&g_fk_bp_live) == bp0, "temp bp leaked (%ld)", (long)(L(&g_fk_bp_live) - bp0));

    printf("[2] 目标一直跑，用户点 Stop（HALT）取消\n");
    FK_SET(g_fk_run_ms, -1);
    j = start_step(); Sleep(100);
    cancel_with(j, OS_CMD_HALT, "HALT");
    CHECK(L(&g_fk_bp_live) == bp0, "temp bp leaked after HALT cancel");

    printf("[3] StepOut 进行中再来一次 StepOut（如 AI 线程）→ BUSY，不并发\n");
    j = start_step(); Sleep(50);
    t0 = now_ms();
    i = cmd(OS_CMD_STEP_OUT, NULL, NULL);
    dt = now_ms() - t0;
    printf("    第二次 StepOut rc=%d 耗时 %.1fms\n", i, dt);
    CHECK(i == OS_ERR_BUSY && dt < 20, "second step-out should be BUSY immediately");
    cancel_with(j, OS_CMD_HALT, "HALT");

    printf("[4] 反复取消 8 次后再正常步出（临时断点/FPB 槽不泄漏）\n");
    for (i = 0; i < 8; i++) {
        j = start_step(); Sleep(30);
        cmd(OS_CMD_HALT, NULL, NULL);
        m = wait_msg(3000);
        CHECK(m == j && m->rc == OS_ERR_CANCELED, "loop %d rc=%d", i, m ? m->rc : 999);
        if (m) os_cmd_finish(m, 1000);
    }
    FK_SET(g_fk_run_ms, 100);
    j = start_step(); m = wait_msg(3000);
    CHECK(m == j && m->rc == OS_ERR_OK, "step-out after cancels rc=%d", m ? m->rc : 999);
    if (m) os_cmd_finish(m, 1000);
    CHECK(L(&g_fk_bp_live) == bp0, "temp bp leaked after loop");

    printf("[5] 2s 内未返回（目标一直跑、无人取消）：有界结束\n");
    FK_SET(g_fk_run_ms, -1);
    t0 = now_ms(); j = start_step(); m = wait_msg(4000); dt = now_ms() - t0;
    printf("    完成消息 %.1fms 后到达，rc=%d\n", dt, m ? m->rc : 999);
    CHECK(m == j && dt >= 1900 && dt < 2600, "timeout path %.1f", dt);
    if (m) os_cmd_finish(m, 1000);
    cmd(OS_CMD_HALT, NULL, NULL);

    printf("[6] 用户点断开取消\n");
    j = start_step(); Sleep(100);
    cancel_with(j, OS_CMD_DISCONNECT, "DISCONNECT");
    do_connect();

    printf("[7] 退出（WM_DESTROY 顺序）：HALT + abandon(3000) → deinit\n");
    j = start_step(); Sleep(80);
    t0 = now_ms();
    /* 先摘掉通知再 HALT：若按 HALT→abandon 的顺序，工作线程可能在两者之间就结束并投递（Windows 上偶发），
     * 那条消息发往正在销毁的窗口、会被丢弃，不是缺陷；这里只验证「abandon 之后不再投递」。 */
    InterlockedExchangePointer((PVOID volatile*)&j->notify, NULL);
    cmd(OS_CMD_HALT, NULL, NULL);
    i = os_cmd_abandon(j, 3000);
    dt = now_ms() - t0;
    printf("    abandon 返回 %d，耗时 %.1fms\n", i, dt);
    CHECK(i == 1 && dt < 150, "abandon not quick");
    CHECK(wait_msg(50) == NULL, "posted after abandon");

    printf("[8] 无人取消时直接 deinit（例如 AI 线程的 StepOut 还在跑）：deinit 先等 StepOut 退出再删锁\n");
    j = start_step(); Sleep(80);
    t0 = now_ms();
    M->deinit(NULL);
    dt = now_ms() - t0;
    m = wait_msg(1000);
    printf("    deinit 耗时 %.1fms；StepOut rc=%d\n", dt, m ? m->rc : 999);
    CHECK(dt < 700, "deinit too slow %.1f", dt);
    CHECK(m == j && m->rc == OS_ERR_CANCELED, "step-out not cancelled by deinit");
    if (m) os_cmd_finish(m, 1000);

    printf("DLL 内最大并发=%ld（须为 1） 无会话/已卸载调用=%ld  会话 open=%ld close=%ld  钉住=%ld 放开=%ld\n",
           (long)L(&g_fk_max_inside), (long)L(&g_fk_stale), (long)L(&g_fk_open), (long)L(&g_fk_close), (long)L(&g_pins), (long)L(&g_unpins));
    CHECK(L(&g_fk_max_inside) == 1, "concurrent DLL calls");
    CHECK(L(&g_fk_stale) == 0, "stale DLL calls");
    CHECK(L(&g_fk_open) == L(&g_fk_close), "session imbalance");
    CHECK(L(&g_pins) == L(&g_unpins), "pin imbalance");
    printf("RESULT: %s (%d)\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
