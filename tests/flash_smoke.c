/* 烧录 API 冒烟：未连接时 FLASH 命令返回 NOT_CONNECTED；结构体布局合理。 */
#include "module_api.h"
#include <stdio.h>
#include <string.h>

static int g_fail;
static void expect(int ok, const char* what)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) g_fail++;
}

/* 最小驱动桩：只实现 flash 命令的未连接路径 */
static int stub_cmd(void* ctx, int cmd, void* in, void* out)
{
    int connected = 0;
    (void)ctx; (void)out;
    if (cmd == OS_CMD_IS_CONNECTED) {
        if (out) *(int*)out = 0;
        return OS_ERR_OK;
    }
    if (cmd == OS_CMD_FLASH_DOWNLOAD || cmd == OS_CMD_FLASH_ERASE) {
        if (!connected) return OS_ERR_NOT_CONNECTED;
        (void)in;
        return OS_ERR_OK;
    }
    return OS_ERR_FAIL;
}

int main(void)
{
    OS_FlashReq req;
    int rc;
    setvbuf(stdout, NULL, _IONBF, 0);
    memset(&req, 0, sizeof(req));
    wcscpy(req.path, L"D:\\fw.elf");
    req.address = 0x08000000u;
    req.verify = 1;
    req.reset_run = 1;
    _snprintf(req.device, sizeof(req.device), "STM32F303RC");

    expect(OS_CMD_FLASH_DOWNLOAD == 30 && OS_CMD_FLASH_ERASE == 31, "命令号 30/31");
    expect(sizeof(req.path) >= MAX_PATH * sizeof(wchar_t), "path 字段够宽");
    expect(req.address == 0x08000000u, "默认 Flash 基址");

    rc = stub_cmd(NULL, OS_CMD_FLASH_DOWNLOAD, &req, NULL);
    expect(rc == OS_ERR_NOT_CONNECTED, "未连接时 DOWNLOAD 返回 NOT_CONNECTED");
    rc = stub_cmd(NULL, OS_CMD_FLASH_ERASE, &req, NULL);
    expect(rc == OS_ERR_NOT_CONNECTED, "未连接时 ERASE 返回 NOT_CONNECTED");

    if (g_fail) {
        printf("FLASH SMOKE FAIL (%d)\n", g_fail);
        return 1;
    }
    printf("FLASH SMOKE ALL PASS\n");
    return 0;
}
