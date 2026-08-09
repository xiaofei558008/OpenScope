/*
 * Bug9 回归：非4000速度读值全为 0。
 * 根因：mod_read 用 r>=0 判定成功，而 JLINKARM_ReadMem 失败返回正值(rc=1)，
 *       零缓冲被当成成功样本推给 UI（rc==size 但数据为 0）。
 * 修复后严格 r==0 判定 + 连接丢失自动重连。
 *
 * 断言（每个速度）：
 *   A) 至少 1 次成功读（rc==size）——速度不能全部失败；
 *   B) 一致性：所有 rc==size 的读值 == 首个成功读值（RAM 静态不变）。
 *      任何 rc==size 但值不同（尤其=0）即"屏蔽失败"= 复现 Bug9。
 *
 * 用法: bug9_smoke <device> <speed_khz> [speed_khz ...]
 *   0=全 PASS；非 0=FAIL
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

/* 边缘目标/物理 J-Link 偶发 connect 失败（DLL 状态未稳），重试 3 次再判失败 */
static int try_connect(const char* devname, int speed)
{
    OS_ConnectCfg cfg;
    OS_ScanReq sr;
    OS_DeviceInfo di[8];
    int i, rc;
    memset(&sr, 0, sizeof(sr)); sr.items = di; sr.capacity = 8;
    rc = cmd(OS_CMD_SCAN, &sr, NULL);
    if (rc != OS_ERR_OK || sr.count <= 0) { printf("  FAIL scan rc=%d count=%d\n", rc, sr.count); return -1; }
    memset(&cfg, 0, sizeof(cfg));
    cfg.iface = OS_IF_SWD; cfg.speed_khz = speed; cfg.probe_index = 0;
    if (strcmp(devname, "-") != 0)
        _snprintf(cfg.device, sizeof(cfg.device), "%s", devname);  /* "-" = 空设备名，自动识别核心 */
    for (i = 0; i < 3; i++) {
        rc = cmd(OS_CMD_CONNECT, &cfg, NULL);
        if (rc == OS_ERR_OK) return 0;
        cmd(OS_CMD_DISCONNECT, NULL, NULL);
        Sleep(300);
    }
    printf("  FAIL connect after 3 tries rc=%d\n", rc);
    return -1;
}

static int run_speed(const char* devname, int speed)
{
    OS_MemReq mr;
    int i, rc, nreads = 40, succ = 0, first_set = 0;
    unsigned int first = 0;
    uint8_t buf[4];

    if (try_connect(devname, speed) != 0) return 1;

    for (i = 0; i < nreads; i++) {
        unsigned int val;
        memset(buf, 0, sizeof(buf));
        mr.address = 0x20000000; mr.size = 4; mr.data = buf;
        rc = cmd(OS_CMD_READ_MEM, &mr, NULL);
        memcpy(&val, buf, 4);
        if (rc == 4) {
            succ++;
            if (!first_set) { first = val; first_set = 1; }
            else if (val != first) {
                printf("  FAIL masked-zero inconsistency: rc=4 val=%08X != first=%08X\n", val, first);
                cmd(OS_CMD_DISCONNECT, NULL, NULL);
                return 1;
            }
        }
        Sleep(40);  /* 给连接丢失自动重连（500ms 节流）留出窗口 */
    }
    printf("  speed=%d: reads=%d succ=%d first=%08X %s\n",
           speed, nreads, succ, first, (first_set ? "OK" : "<- FAIL"));
    cmd(OS_CMD_DISCONNECT, NULL, NULL);
    if (!first_set) { printf("  FAIL no successful read at speed %d\n", speed); return 1; }
    return 0;
}

int main(int argc, char** argv)
{
    int i, total_fail = 0;
    if (argc < 3) { printf("usage: bug9_smoke <device> <speed_khz> [speed_khz ...]\n"); return 2; }
    setvbuf(stdout, NULL, _IONBF, 0);
    g_h = LoadLibraryW(L"dll/jlink.dll");
    if (!g_h) { printf("FAIL load err=%lu\n", GetLastError()); return 1; }
    g_m = ((os_module_get_fn)GetProcAddress(g_h, "os_module_get"))();
    if (!g_m || g_m->init(&g_fw, &g_ctx) != OS_ERR_OK) { printf("FAIL init\n"); return 1; }
    for (i = 2; i < argc; i++) {
        printf("[speed %d]\n", atoi(argv[i]));
        total_fail += run_speed(argv[1], atoi(argv[i]));
    }
    g_m->deinit(g_ctx);
    FreeLibrary(g_h);
    printf(total_fail ? "BUG9 SMOKE FAIL\n" : "BUG9 SMOKE ALL PASS\n");
    return total_fail ? 1 : 0;
}
