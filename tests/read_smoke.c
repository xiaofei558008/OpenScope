/*
 * OpenScope J-Link read-poll smoke — 模拟 poll_thread 连续读取。
 * 用法: read_smoke <device> <speed_khz> [reads]
 * 连接一次，连续读取 reads 次（默认 100），统计非零结果。
 * Bug 9：非4000速度读值为0 —— 若某速度下非零命中明显偏低/为0，复现。
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "module_api.h"

typedef const OS_Module* (*os_module_get_fn)(void);

static HMODULE g_h;
static const OS_Module* g_m;
static void* g_ctx;

static void fake_log(int level, const char* fmt, ...) { (void)level; (void)fmt; }
static const OS_Framework g_fw = { OS_API_VERSION, fake_log, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

static int cmd(int c, void* in, void* out) { return g_m->command(g_ctx, c, in, out); }

int main(int argc, char** argv)
{
    OS_ConnectCfg cfg;
    OS_ScanReq sr;
    OS_DeviceInfo di[8];
    OS_MemReq mr;
    int rc, i, nreads = 100, nonzero = 0, failreads = 0;
    unsigned long long total = 0;
    if (argc < 3) { printf("usage: read_smoke <device> <speed_khz> [reads]\n"); return 2; }
    const char* devname = argv[1];
    int speed = atoi(argv[2]);
    if (argc > 3) nreads = atoi(argv[3]);
    setvbuf(stdout, NULL, _IONBF, 0);
    g_h = LoadLibraryW(L"dll/jlink.dll");
    if (!g_h) { printf("FAIL load err=%lu\n", GetLastError()); return 1; }
    g_m = ((os_module_get_fn)GetProcAddress(g_h, "os_module_get"))();
    rc = g_m->init(&g_fw, &g_ctx);
    if (rc != OS_ERR_OK) { printf("FAIL init rc=%d\n", rc); return 1; }

    memset(&sr, 0, sizeof(sr)); sr.items = di; sr.capacity = 8;
    rc = cmd(OS_CMD_SCAN, &sr, NULL);
    if (rc != OS_ERR_OK || sr.count <= 0) { printf("FAIL scan rc=%d count=%d\n", rc, sr.count); return 1; }

    memset(&cfg, 0, sizeof(cfg));
    cfg.iface = OS_IF_SWD; cfg.speed_khz = speed; cfg.probe_index = 0;
    if (strcmp(devname, "-") != 0)
        _snprintf(cfg.device, sizeof(cfg.device), "%s", devname);  /* "-" = 空设备名，让 J-Link 自动识别核心 */
    printf("connect device=%s speed=%d kHz...\n", devname, speed);
    rc = cmd(OS_CMD_CONNECT, &cfg, NULL);
    printf("connect rc=%d\n", rc);

    for (i = 0; i < nreads; i++) {
        uint8_t buf[4];
        unsigned int val;
        memset(buf, 0, sizeof(buf));
        mr.address = 0x20000000; mr.size = 4; mr.data = buf;
        rc = cmd(OS_CMD_READ_MEM, &mr, NULL);
        memcpy(&val, buf, 4);
        total += val;
        if (rc != 4) failreads++;
        else if (val != 0) nonzero++;
        Sleep(5);
    }
    printf("device=%s speed=%d: reads=%d nonzero=%d failread=%d last=%llX\n",
           devname, speed, nreads, nonzero, failreads, total & 0xFFFFFFFF);
    cmd(OS_CMD_DISCONNECT, NULL, NULL);
    g_m->deinit(g_ctx);
    FreeLibrary(g_h);
    return 0;
}
