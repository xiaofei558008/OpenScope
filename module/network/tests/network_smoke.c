/* network.dll 模块冒烟测试：加载 + os_module_get + init + GET_INFO + deinit。 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
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
    int rc;

    h = LoadLibraryA("dll\\network.dll");
    if (!h) { printf("FAIL load network.dll err=%lu\n", GetLastError()); return 1; }
    get = (os_module_get_fn)GetProcAddress(h, "os_module_get");
    if (!get) { printf("FAIL os_module_get\n"); return 1; }
    m = get();
    if (!m || m->api_version != OS_API_VERSION) { printf("FAIL api\n"); return 1; }
    printf("module: %s v%s cap=0x%X\n", m->name, m->version, m->capabilities);
    if (!(m->capabilities & OS_CAP_NET)) { printf("FAIL cap NET\n"); return 1; }

    rc = m->init(NULL, &ctx);
    printf("init rc=%d\n", rc);
    memset(&info, 0, sizeof(info));
    rc = m->command(ctx, OS_CMD_GET_INFO, NULL, &info);
    printf("get_info rc=%d name=%s version=%s dll=%s\n", rc, info.name, info.version, info.dll_version);
    if (strcmp(info.name, "network") != 0) { printf("FAIL name\n"); return 1; }

    m->deinit(ctx);
    FreeLibrary(h);
    printf("PASS\n");
    return 0;
}
