/*
 * OpenScope J-Link speed read smoke — reproduces Bug 9.
 * 验证不同时钟速度下连接+读取是否正常（Bug 9：非4000速度读值为0）。
 * 通过模块 dll/jlink.dll 完整连接路径。设备名由 argv[1] 指定（默认 Cortex-M4）。
 *
 * Build: cl /nologo /W2 /utf-8 /I code\src tests\speed_smoke.c /Fe:tests\bin\speed_smoke.exe
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "module_api.h"

typedef const OS_Module* (*os_module_get_fn)(void);

static HMODULE g_h;
static const OS_Module* g_m;
static void* g_ctx;
static int g_fails;

static void fake_log(int level, const char* fmt, ...) { (void)level; (void)fmt; }

static const OS_Framework g_fw = {
    OS_API_VERSION, fake_log, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL
};

#define CHECK(c, what) do { \
    printf("%s %s\n", (c) ? "PASS" : "FAIL", what); \
    if (!(c)) g_fails++; \
} while (0)

static int cmd(int c, void* in, void* out) { return g_m->command(g_ctx, c, in, out); }

int main(int argc, char** argv)
{
    OS_ConnectCfg cfg;
    OS_ScanReq sr;
    OS_DeviceInfo di[8];
    int rc, i;
    const char* devname = (argc > 1) ? argv[1] : "Cortex-M4";
    static const int speeds[] = { 1000, 2000, 4000, 5000, 8000, 10000 };
    setvbuf(stdout, NULL, _IONBF, 0);   /* 逐行输出，避免挂起时缓冲丢失 */
    g_h = LoadLibraryW(L"dll/jlink.dll");
    if (!g_h) { printf("FAIL load dll/jlink.dll err=%lu\n", GetLastError()); return 1; }
    os_module_get_fn get = (os_module_get_fn)GetProcAddress(g_h, "os_module_get");
    if (!get) { printf("FAIL no os_module_get\n"); return 1; }
    g_m = get();
    rc = g_m->init(&g_fw, &g_ctx);
    CHECK(rc == OS_ERR_OK, "模块初始化");

    memset(&sr, 0, sizeof(sr)); sr.items = di; sr.capacity = 8;
    rc = cmd(OS_CMD_SCAN, &sr, NULL);
    CHECK(rc == OS_ERR_OK && sr.count > 0, "扫描到 J-Link 设备");
    if (sr.count <= 0) { printf("no device, exit\n"); return 1; }
    printf("设备: %s (serial %s)\n", di[0].name, di[0].serial);
    printf("设备名: %s\n", devname);

    for (i = 0; i < (int)(sizeof(speeds)/sizeof(speeds[0])); i++) {
        OS_MemReq mr;
        uint8_t buf[8];
        unsigned int val;
        int c = 0;
        memset(&cfg, 0, sizeof(cfg));
        cfg.iface = OS_IF_SWD;
        cfg.speed_khz = speeds[i];
        cfg.probe_index = 0;
        _snprintf(cfg.device, sizeof(cfg.device), "%s", devname);
        rc = cmd(OS_CMD_CONNECT, &cfg, NULL);
        g_m->command(g_ctx, OS_CMD_IS_CONNECTED, NULL, &c);
        memset(buf, 0, sizeof(buf));
        mr.address = 0x20000000; mr.size = 4; mr.data = buf;
        rc = cmd(OS_CMD_READ_MEM, &mr, NULL);
        memcpy(&val, buf, 4);
        printf("speed=%d kHz connect_rc=%d isconn=%d read=%s val=0x%08X\n",
               speeds[i], rc, c, (rc == 4 ? "ok" : "fail"), val);
        cmd(OS_CMD_DISCONNECT, NULL, NULL);
        Sleep(300);
    }
    g_m->deinit(g_ctx);
    FreeLibrary(g_h);
    printf("%s\n", g_fails ? "FAILURES" : "ALL PASS");
    return g_fails;
}
