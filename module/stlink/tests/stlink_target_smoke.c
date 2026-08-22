/* ST-Link 目标实机测试（Nucleo-G431RB）：连接 + 读 + 写/回读/恢复。
 * 用法: stlink_target_smoke [speed_khz] [addr] [size]
 * 写测试为"非破坏"：读原值 → 写测试图案 → 回读校验 → 恢复原值。 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "module_api.h"

typedef const OS_Module* (*os_module_get_fn)(void);

int main(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    HMODULE h;
    os_module_get_fn get;
    const OS_Module* m;
    void* ctx = NULL;
    OS_ConnectCfg cfg;
    OS_DriverInfo info;
    OS_MemReq req;
    unsigned char buf[256], orig[256], pat[256];
    int speed = argc > 1 ? atoi(argv[1]) : 4000;
    unsigned int addr = argc > 2 ? (unsigned int)strtoul(argv[2], NULL, 0) : 0x10000000u; /* CCM SRAM，默认固件不占用 */
    unsigned int size = argc > 3 ? (unsigned int)strtoul(argv[3], NULL, 0) : 8u;
    int rc, conn = 0, i, rd_ok = 0, wr_ok = 0;
    if (size > sizeof(buf)) size = sizeof(buf);

    h = LoadLibraryA("dll\\stlink.dll");
    if (!h) { printf("FAIL load stlink.dll err=%lu\n", GetLastError()); return 1; }
    get = (os_module_get_fn)GetProcAddress(h, "os_module_get");
    m = get();
    if (!m || m->api_version != OS_API_VERSION) { printf("FAIL api\n"); return 1; }

    rc = m->init(NULL, &ctx);
    printf("init rc=%d\n", rc);

    memset(&cfg, 0, sizeof(cfg));
    cfg.iface = OS_IF_SWD;
    cfg.speed_khz = speed;
    cfg.probe_index = -1;
    rc = m->command(ctx, OS_CMD_CONNECT, &cfg, NULL);
    printf("connect rc=%d speed=%d\n", rc, speed);
    if (rc == OS_ERR_NO_DEVICE) {
        printf("SKIP: 未发现可用 ST-Link（未连接或被占用，如 OpenScope.exe 已连接）\n");
        m->deinit(ctx); FreeLibrary(h); return 0;
    }
    if (rc != OS_ERR_OK) { m->deinit(ctx); FreeLibrary(h); return 2; }

    m->command(ctx, OS_CMD_IS_CONNECTED, NULL, &conn);
    memset(&info, 0, sizeof(info));
    m->command(ctx, OS_CMD_GET_INFO, NULL, &info);
    printf("connected=%d emulator=%s\n", conn, info.emulator);

    /* 读 200 次校验稳定 */
    memset(&req, 0, sizeof(req));
    req.address = addr; req.size = size; req.data = buf;
    for (i = 0; i < 200; i++) {
        memset(buf, 0, size);
        rc = m->command(ctx, OS_CMD_READ_MEM, &req, NULL);
        if (rc == (int)size) rd_ok++;
    }
    printf("read: %d/200 OK\n", rd_ok);
    memcpy(orig, buf, size);
    printf("orig   :");
    for (i = 0; i < (int)size; i++) printf(" %02X", orig[i]);
    printf("\n");

    /* 写测试图案 → 回读校验 → 恢复原值（非破坏） */
    for (i = 0; i < (int)size; i++) pat[i] = (unsigned char)(0xA0 + (i & 0x0F));
    req.address = addr; req.size = size;
    req.data = pat;
    rc = m->command(ctx, OS_CMD_WRITE_MEM, &req, NULL);
    printf("write rc=%d (期望 %d)\n", rc, OS_ERR_OK);
    memset(buf, 0, size);
    req.data = buf;
    rc = m->command(ctx, OS_CMD_READ_MEM, &req, NULL);
    if (rc == (int)size && memcmp(buf, pat, size) == 0) wr_ok = 1;
    printf("pat    :");
    for (i = 0; i < (int)size; i++) printf(" %02X", pat[i]);
    printf("\nreadback:");
    for (i = 0; i < (int)size; i++) printf(" %02X", buf[i]);
    printf("\nwrite-verify: rc=%d match=%s\n", rc, wr_ok ? "YES" : "NO");
    /* 恢复原值 */
    req.data = orig;
    m->command(ctx, OS_CMD_WRITE_MEM, &req, NULL);
    memset(buf, 0, size);
    req.data = buf;
    m->command(ctx, OS_CMD_READ_MEM, &req, NULL);
    printf("after-restore:");
    for (i = 0; i < (int)size; i++) printf(" %02X", buf[i]);
    printf("\nrestore match=%s\n", memcmp(buf, orig, size) == 0 ? "YES" : "NO");

    m->command(ctx, OS_CMD_DISCONNECT, NULL, NULL);
    m->deinit(ctx);
    FreeLibrary(h);
    {
        int pass = (rd_ok == 200) && (wr_ok == 1) && (memcmp(buf, orig, size) == 0);
        printf(pass ? "PASS\n" : "FAIL\n");
        return pass ? 0 : 3;
    }
}
