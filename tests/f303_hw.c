/* STM32F303RC + J-Link 真机检查。
 * 确认探针、核心、DBGMCU ID、SRAM 块读，以及停核后的写/读/还原。
 * 用法（在仓库根目录）: tests\bin\f303_hw.exe
 */
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "module_api.h"

typedef const OS_Module* (*os_module_get_fn)(void);

static int g_fail;

static void logfn(int level, const char* fmt, ...)
{
    va_list ap;
    (void)level;
    printf("    | ");
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

static const OS_Framework g_fw = {
    OS_API_VERSION, logfn, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

static void check(int ok, const char* what)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) g_fail++;
}

static int read_mem(const OS_Module* m, void* ctx, uint64_t addr, void* buf, uint32_t n)
{
    OS_MemReq mr;
    memset(buf, 0, n);
    mr.address = addr;
    mr.size = n;
    mr.data = buf;
    return m->command(ctx, OS_CMD_READ_MEM, &mr, NULL);
}

static int write_mem(const OS_Module* m, void* ctx, uint64_t addr, const void* buf, uint32_t n)
{
    OS_MemReq mr;
    mr.address = addr;
    mr.size = n;
    mr.data = (void*)buf;
    return m->command(ctx, OS_CMD_WRITE_MEM, &mr, NULL);
}

static int connect_at(const OS_Module* m, void* ctx, int speed)
{
    OS_ConnectCfg cfg;
    int i, rc;
    memset(&cfg, 0, sizeof(cfg));
    cfg.iface = OS_IF_SWD;
    cfg.speed_khz = speed;
    cfg.probe_index = 0;
    _snprintf(cfg.device, sizeof(cfg.device), "Cortex-M4");
    for (i = 0; i < 3; i++) {
        rc = m->command(ctx, OS_CMD_CONNECT, &cfg, NULL);
        if (rc == OS_ERR_OK) return 0;
        m->command(ctx, OS_CMD_DISCONNECT, NULL, NULL);
        Sleep(400);
    }
    printf("  connect %d kHz failed rc=%d\n", speed, rc);
    return -1;
}

/* F303xB/C DEV_ID = 0x422（RM0316 DBGMCU_IDCODE） */
static int dev_is_f303(unsigned idcode)
{
    return (idcode & 0xFFF) == 0x422;
}

int main(void)
{
    HMODULE h;
    const OS_Module* m;
    void* ctx = NULL;
    OS_ScanReq sr;
    OS_DeviceInfo di[8];
    OS_DriverInfo info;
    unsigned char buf[256];
    unsigned char blk[512];
    unsigned idcode = 0, cpuid = 0, sp = 0, reset = 0;
    unsigned orig = 0, back = 0, sentinel = 0xA5A55A5Au;
    int rc, i, succ, connected = 0;
    const uint64_t scratch = 0x20009FFC; /* F303RC 主 SRAM 40KB 的最后一个字 */

    setvbuf(stdout, NULL, _IONBF, 0);
    h = LoadLibraryW(L"dll/jlink.dll");
    if (!h) { printf("FAIL load jlink.dll err=%lu\n", GetLastError()); return 1; }
    m = ((os_module_get_fn)GetProcAddress(h, "os_module_get"))();
    if (!m || m->init(&g_fw, &ctx) != OS_ERR_OK) { printf("FAIL init\n"); return 1; }

    memset(&sr, 0, sizeof(sr));
    sr.items = di;
    sr.capacity = 8;
    rc = m->command(ctx, OS_CMD_SCAN, &sr, NULL);
    printf("scan rc=%d count=%d\n", rc, sr.count);
    for (i = 0; i < sr.count; i++)
        printf("  probe[%d] sn=%s name=%s\n", di[i].index, di[i].serial, di[i].name);
    check(rc == OS_ERR_OK && sr.count >= 1, "J-Link 扫描到至少 1 个探针");

    check(connect_at(m, ctx, 4000) == 0, "SWD 4000 kHz 连接 Cortex-M4");
    memset(&info, 0, sizeof(info));
    m->command(ctx, OS_CMD_GET_INFO, NULL, &info);
    printf("  emulator: %s\n", info.emulator);

    rc = read_mem(m, ctx, 0xE0042000, &idcode, 4);
    printf("  DBGMCU_IDCODE rc=%d value=%08X dev=%03X\n", rc, idcode, idcode & 0xFFF);
    check(rc == 4 && dev_is_f303(idcode), "目标是 STM32F303（DEV_ID 0x422）");

    rc = read_mem(m, ctx, 0xE000ED00, &cpuid, 4);
    printf("  CPUID rc=%d value=%08X\n", rc, cpuid);
    check(rc == 4 && cpuid == 0x410FC241, "CPUID 是 Cortex-M4");

    rc = read_mem(m, ctx, 0x08000000, buf, 8);
    memcpy(&sp, buf, 4);
    memcpy(&reset, buf + 4, 4);
    printf("  vector SP=%08X Reset=%08X\n", sp, reset);
    check(rc == 8 && (sp & 0xFF000000) == 0x20000000, "向量表 SP 落在 SRAM");
    check(rc == 8 && (reset & 0xFF000000) == 0x08000000, "向量表 Reset 落在 Flash");

    rc = read_mem(m, ctx, 0x20000000, blk, sizeof(blk));
    printf("  SRAM block 512B @0x20000000 rc=%d\n", rc);
    check(rc == (int)sizeof(blk), "SRAM 512 字节块读成功");

    /* 停核后改最后一个字再还原，避免把正在跑的程序写坏。 */
    rc = m->command(ctx, OS_CMD_HALT, NULL, NULL);
    printf("  halt rc=%d\n", rc);
    check(rc == 0, "Halt 目标");
    if (rc == 0) {
        int wr, rd1, rd2, rd3;
        rd1 = read_mem(m, ctx, scratch, &orig, 4);
        wr = write_mem(m, ctx, scratch, &sentinel, 4);
        rd2 = read_mem(m, ctx, scratch, &back, 4);
        printf("  scratch @0x%llX orig=%08X write_rc=%d readback=%08X\n",
               (unsigned long long)scratch, orig, wr, back);
        check(rd1 == 4 && wr == OS_ERR_OK && rd2 == 4 && back == sentinel,
              "SRAM 末字写入后读回一致");
        wr = write_mem(m, ctx, scratch, &orig, 4);
        rd3 = read_mem(m, ctx, scratch, &back, 4);
        check(wr == OS_ERR_OK && rd3 == 4 && back == orig, "SRAM 末字已还原");
        m->command(ctx, OS_CMD_GO, NULL, NULL);
    }

    /* 12 MHz：连续小读，统计成功率。旧问题是一开始就失败并拆掉会话。 */
    m->command(ctx, OS_CMD_DISCONNECT, NULL, NULL);
    check(connect_at(m, ctx, 12000) == 0, "SWD 12000 kHz 重新连接");
    succ = 0;
    for (i = 0; i < 200; i++) {
        unsigned v = 0;
        if (read_mem(m, ctx, 0x08000000, &v, 4) == 4 && v == sp) succ++;
    }
    printf("  12000 kHz flash reads %d/200\n", succ);
    check(succ >= 190, "12000 kHz 连续读 Flash 向量至少 190/200 成功");

    m->command(ctx, OS_CMD_IS_CONNECTED, NULL, &connected);
    check(connected == 1, "连续读之后会话仍连接");
    m->command(ctx, OS_CMD_DISCONNECT, NULL, NULL);
    m->deinit(ctx);
    FreeLibrary(h);

    printf(g_fail ? "F303 HW FAIL (%d)\n" : "F303 HW ALL PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
