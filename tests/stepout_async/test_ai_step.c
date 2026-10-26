/* tests/stepout_async/test_ai_step.c：AI 的 step_out 异步化（第五轮 任务4）
 * 真实 code/src/ai_step.c + cmd_async.c + jlink.c / stlink.c + 假厂商 DLL。计时一律 QPC。 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "module_api.h"
#include "cmd_async.h"
#include "ai_step.h"
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
static volatile LONG g_pins, g_unpins;
BOOL pin_GetModuleHandleExW(DWORD fl, LPCWSTR a, HMODULE* out) { (void)fl; (void)a; InterlockedIncrement(&g_pins); *out = (HMODULE)(intptr_t)0x7777; return TRUE; }
BOOL pin_FreeLibrary(HMODULE h) { (void)h; InterlockedIncrement(&g_unpins); return TRUE; }
BOOL pin_PostMessageW(HWND h, UINT msg, WPARAM wp, LPARAM lp) { (void)h; (void)msg; (void)wp; (void)lp; return TRUE; }
static LONG L(volatile LONG* p) { return InterlockedCompareExchange(p, 0, 0); }

/* 等工作线程真正进入 StepOut（假 DLL 收到 Go） */
static void wait_go(LONG rc0) { double t0 = now_ms(); while (L(&g_fk_run_ctl) == rc0 && now_ms() - t0 < 1000) Sleep(1); }

typedef struct { int r, rc; double ms; } WaitRes;
static volatile LONG g_w_done;
static WaitRes g_w;
static DWORD WINAPI waiter(LPVOID p)
{
    double t0 = now_ms();
    (void)p;
    g_w.rc = 12345;
    g_w.r = os_ai_step_out(M, NULL, 0, 3000, &g_w.rc);
    g_w.ms = now_ms() - t0;
    InterlockedExchange(&g_w_done, 1);
    return 0;
}

int main(void)
{
    OS_Framework fw; void* ctx = NULL; OS_ConnectCfg cfg; double t0, dt; int r, rc, c = 0; LONG rc0, bp0; HANDLE th;
    setvbuf(stdout, NULL, _IONBF, 0);
    memset(&fw, 0, sizeof(fw)); fw.api_version = OS_API_VERSION; fw.log = fw_log;
    printf("== AI step_out 异步（驱动 %s）\n", DRV_NAME);
    M = os_module_get();
    CHECK(M && M->init(&fw, &ctx) == OS_ERR_OK, "init");
    memset(&cfg, 0, sizeof(cfg)); cfg.probe_index = -1; cfg.speed_khz = 4000;
    CHECK(cmd(OS_CMD_CONNECT, &cfg, NULL) == OS_ERR_OK, "connect");
    cmd(OS_CMD_IS_CONNECTED, NULL, &c); CHECK(c == 1, "connected");
    bp0 = L(&g_fk_bp_live);

    printf("[0] 未启动过：status = idle\n");
    CHECK(os_ai_step_status(&rc) == -1, "status before any step");

    printf("[1] 兼容模式（step_out {}）：200ms 后命中，完成后回复 OK\n");
    FK_SET(g_fk_run_ms, 200);
    t0 = now_ms(); rc = 999;
    r = os_ai_step_out(M, NULL, 0, 3000, &rc); dt = now_ms() - t0;
    printf("    r=%d rc=%d 耗时 %.1fms\n", r, rc, dt);
    CHECK(r == 1 && rc == OS_ERR_OK, "legacy r=%d rc=%d", r, rc);
    CHECK(dt >= 180 && dt < 600, "legacy duration %.1f", dt);
    CHECK(L(&g_fk_bp_live) == bp0, "temp bp leaked");

    printf("[2] 异步模式（{\"async\":1}）：立即 pending，轮询 status 得到 OK\n");
    FK_SET(g_fk_run_ms, 150);
    rc0 = L(&g_fk_run_ctl);
    t0 = now_ms();
    r = os_ai_step_out(M, NULL, 0, 0, &rc); dt = now_ms() - t0;
    printf("    start r=%d 耗时 %.1fms\n", r, dt);
    CHECK(r == 0 && dt < 20, "async start r=%d %.1fms", r, dt);
    wait_go(rc0);
    CHECK(os_ai_step_busy(), "busy while running");
    CHECK(os_ai_step_status(&rc) == 0, "status pending");
    printf("[3] 进行中再来一次 step_out → AGAIN（多个 AI 客户端互斥）\n");
    r = os_ai_step_out(M, NULL, 0, 3000, &rc);
    CHECK(r == OS_ERR_AGAIN, "second step_out r=%d", r);
    t0 = now_ms();
    while ((r = os_ai_step_status(&rc)) == 0 && now_ms() - t0 < 2000) Sleep(5);
    printf("    status r=%d rc=%d（%.1fms 后）\n", r, rc, now_ms() - t0);
    CHECK(r == 1 && rc == OS_ERR_OK, "async result r=%d rc=%d", r, rc);
    CHECK(!os_ai_step_busy(), "busy after done");

    printf("[4] 异步 + 目标一直跑：另一客户端 halt 取消 → status=canceled\n");
    FK_SET(g_fk_run_ms, -1);
    rc0 = L(&g_fk_run_ctl);
    CHECK(os_ai_step_out(M, NULL, 0, 0, &rc) == 0, "start");
    wait_go(rc0); Sleep(50);
    t0 = now_ms();
    cmd(OS_CMD_HALT, NULL, NULL);
    while ((r = os_ai_step_status(&rc)) == 0 && now_ms() - t0 < 2000) Sleep(1);
    dt = now_ms() - t0;
    printf("    HALT 后 %.1fms 完成，rc=%d\n", dt, rc);
    CHECK(r == 1 && rc == OS_ERR_CANCELED, "halt cancel r=%d rc=%d", r, rc);
    CHECK(dt < 150, "cancel slow %.1f", dt);
    CHECK(L(&g_fk_bp_live) == bp0, "temp bp leaked after cancel");

    printf("[5] 兼容模式等待中，别的线程先 status 收割 → 等待者仍拿到自己的结果\n");
    FK_SET(g_fk_run_ms, 150);
    InterlockedExchange(&g_w_done, 0);
    rc0 = L(&g_fk_run_ctl);
    th = CreateThread(NULL, 0, waiter, NULL, 0, NULL);
    wait_go(rc0);
    t0 = now_ms();
    while (os_ai_step_status(&rc) == 0 && now_ms() - t0 < 2000) Sleep(1);   /* 抢先收割 */
    WaitForSingleObject(th, INFINITE); CloseHandle(th);
    printf("    等待者 r=%d rc=%d 耗时 %.1fms\n", g_w.r, g_w.rc, g_w.ms);
    CHECK(g_w.r == 1 && g_w.rc == OS_ERR_OK, "waiter r=%d rc=%d", g_w.r, g_w.rc);

    printf("[6] UI 的 StepOut 正在进行，AI step_out → 驱动报忙（不并发）\n");
    {
        OS_CmdJob* uj; FK_SET(g_fk_run_ms, 300);
        rc0 = L(&g_fk_run_ctl);
        uj = os_cmd_start(M, NULL, OS_CMD_STEP_OUT, 0, NULL, 0);
        wait_go(rc0);
        r = os_ai_step_out(M, NULL, 0, 3000, &rc);
        printf("    AI r=%d rc=%d\n", r, rc);
        CHECK(r == 1 && rc != OS_ERR_OK, "AI step_out during UI step should fail busy (r=%d rc=%d)", r, rc);
        CHECK(os_cmd_finish(uj, 3000) == 1, "ui job");
    }

    printf("[7] 退出：兼容模式等待中（目标一直跑）调用 os_ai_step_abort → 等待立即结束、HALT、线程汇合\n");
    FK_SET(g_fk_run_ms, -1);
    InterlockedExchange(&g_w_done, 0);
    rc0 = L(&g_fk_run_ctl);
    th = CreateThread(NULL, 0, waiter, NULL, 0, NULL);
    wait_go(rc0); Sleep(50);
    t0 = now_ms();
    r = os_ai_step_abort(3000);
    dt = now_ms() - t0;
    WaitForSingleObject(th, INFINITE); CloseHandle(th);
    printf("    abort r=%d 耗时 %.1fms；等待者 r=%d（0=pending）\n", r, dt, g_w.r);
    CHECK(r == 1 && dt < 150, "abort r=%d %.1fms", r, dt);
    CHECK(g_w.r == 0, "waiter should report pending after abort (r=%d)", g_w.r);
    CHECK(os_ai_step_out(M, NULL, 0, 3000, &rc) == OS_ERR_CANCELED, "step_out after abort must be refused");
    CHECK(L(&g_fk_bp_live) == bp0, "temp bp leaked after abort");

    cmd(OS_CMD_DISCONNECT, NULL, NULL);
    M->deinit(ctx);
    printf("钉住 %ld / 放开 %ld\n", (long)L(&g_pins), (long)L(&g_unpins));
    CHECK(L(&g_pins) == L(&g_unpins), "pin leak");
    printf(g_fail ? "RESULT: FAIL (%d)\n" : "RESULT: PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
