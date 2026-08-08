/* J-Link 模块冒烟测试（不参与模块 DLL 编译，vcxproj 只编译 module/jlink/*.c） */
#include <windows.h>
#include <stdio.h>
#include "module_api.h"

typedef const OS_Module* (*os_module_get_fn)(void);

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    HMODULE h;
    os_module_get_fn get;
    const OS_Module* m;
    void* ctx = NULL;
    OS_DriverInfo info;
    OS_DeviceInfo items[16];
    OS_ScanReq req;
    int rc, conn = -1;

    h = LoadLibraryA("dll\\jlink.dll");
    if (!h) { printf("FAIL load jlink.dll err=%lu\n", GetLastError()); return 1; }
    get = (os_module_get_fn)GetProcAddress(h, "os_module_get");
    if (!get) { printf("FAIL os_module_get\n"); return 1; }
    m = get();
    if (!m || m->api_version != OS_API_VERSION) { printf("FAIL api\n"); return 1; }
    printf("module: %s v%s cap=0x%X\n", m->name, m->version, m->capabilities);

    rc = m->init(NULL, &ctx);
    printf("init rc=%d\n", rc);

    rc = m->command(ctx, OS_CMD_GET_INFO, NULL, &info);
    printf("get_info rc=%d dll=%s hw=%d fw=%d emu=%s connected=%d\n",
           rc, info.dll_version, info.hw_version, info.fw_version, info.emulator, info.connected);

    memset(&req, 0, sizeof(req));
    req.items = items;
    req.capacity = 16;
    rc = m->command(ctx, OS_CMD_SCAN, &req, NULL);
    printf("scan rc=%d count=%d\n", rc, req.count);
    for (int i = 0; i < req.count; i++)
        printf("  dev[%d] sn=%s name=%s\n", items[i].index, items[i].serial, items[i].name);

    rc = m->command(ctx, OS_CMD_IS_CONNECTED, NULL, &conn);
    printf("is_connected rc=%d value=%d\n", rc, conn);

    m->deinit(ctx);
    FreeLibrary(h);
    printf("PASS\n");
    return 0;
}
