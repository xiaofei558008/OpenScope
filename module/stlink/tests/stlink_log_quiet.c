/* 回归测试：ST-Link 采集时不得刷屏（readMemory 每次打印 ~10 条日志会卡死 UI）。
 * 用计数框架验证：connect + 连续 300 次 read 期间，模块回传日志条数应为 0（无错误/警告）。 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "module_api.h"

typedef const OS_Module* (*os_module_get_fn)(void);

static int g_log_count;

static void counting_log(int level, const char* fmt, ...)
{
    (void)level; (void)fmt;
    g_log_count++;
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    HMODULE h;
    os_module_get_fn get;
    const OS_Module* m;
    void* ctx = NULL;
    OS_Framework fw;
    OS_ConnectCfg cfg;
    OS_MemReq req;
    unsigned char buf[64];
    int rc, i, ok = 0;

    h = LoadLibraryA("dll\\stlink.dll");
    if (!h) { printf("FAIL load err=%lu\n", GetLastError()); return 1; }
    get = (os_module_get_fn)GetProcAddress(h, "os_module_get");
    m = get();

    memset(&fw, 0, sizeof(fw));
    fw.log = counting_log;
    rc = m->init(&fw, &ctx);
    printf("init rc=%d (log_count=%d)\n", rc, g_log_count);

    memset(&cfg, 0, sizeof(cfg));
    cfg.iface = OS_IF_SWD; cfg.speed_khz = 4000; cfg.probe_index = -1;
    rc = m->command(ctx, OS_CMD_CONNECT, &cfg, NULL);
    printf("connect rc=%d (log_count=%d)\n", rc, g_log_count);
    if (rc == OS_ERR_NO_DEVICE) {
        printf("SKIP: 未发现可用 ST-Link\n");
        m->deinit(ctx); FreeLibrary(h); return 0;
    }
    if (rc != OS_ERR_OK) { m->deinit(ctx); FreeLibrary(h); return 2; }

    memset(&req, 0, sizeof(req));
    req.address = 0x10000000; req.size = 64; req.data = buf;
    {
        int before = g_log_count;
        for (i = 0; i < 300; i++) {
            rc = m->command(ctx, OS_CMD_READ_MEM, &req, NULL);
            if (rc == 64) ok++;
        }
        printf("read %d/300 OK, 读取期间新增日志=%d\n", ok, g_log_count - before);
        if (ok == 300 && (g_log_count - before) == 0) {
            printf("PASS（读取无刷屏）\n");
        } else {
            printf("FAIL\n");
            m->command(ctx, OS_CMD_DISCONNECT, NULL, NULL);
            m->deinit(ctx); FreeLibrary(h);
            return 3;
        }
    }

    m->command(ctx, OS_CMD_DISCONNECT, NULL, NULL);
    m->deinit(ctx);
    FreeLibrary(h);
    printf("PASS\n");
    return 0;
}
