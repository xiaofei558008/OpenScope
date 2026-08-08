#include "app.h"
#include "module_mgr.h"
#include <string.h>

static void dll_dir(wchar_t* out, int outlen)
{
    wchar_t exe[MAX_PATH];
    wchar_t* slash;
    GetModuleFileNameW(NULL, exe, MAX_PATH);
    slash = wcsrchr(exe, L'\\');
    if (slash) *slash = 0;
    _snwprintf(out, outlen, L"%s\\..\\..\\dll", exe);
}

static int name_has_jlink(const char* name)
{
    return (strstr(name, "jlink") != NULL) || (strstr(name, "JLink") != NULL);
}

int os_modmgr_load(void)
{
    wchar_t dir[MAX_PATH], pattern[MAX_PATH], path[MAX_PATH];
    WIN32_FIND_DATAW fd;
    HANDLE h;
    int loaded = 0;

    dll_dir(dir, MAX_PATH);
    _snwprintf(pattern, MAX_PATH, L"%s\\*.dll", dir);
    h = FindFirstFileW(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        os_log(OS_LOG_WARN, "模块目录不存在: %ls", dir);
        return 0;
    }
    do {
        HMODULE hmod;
        os_module_get_fn get;
        const OS_Module* m;
        void* ctx = NULL;
        int i;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        _snwprintf(path, MAX_PATH, L"%s\\%s", dir, fd.cFileName);
        hmod = LoadLibraryW(path);
        if (!hmod) {
            os_log(OS_LOG_WARN, "加载模块失败: %ls (err=%lu)", fd.cFileName, GetLastError());
            continue;
        }
        get = (os_module_get_fn)GetProcAddress(hmod, OS_MODULE_EXPORT_NAME);
        if (!get) {
            FreeLibrary(hmod);
            continue;
        }
        m = get();
        if (!m || m->api_version != OS_API_VERSION) {
            os_log(OS_LOG_WARN, "模块 %ls API 版本不匹配，已跳过", fd.cFileName);
            FreeLibrary(hmod);
            continue;
        }
        if (m->init && m->init(&g_app.fw, &ctx) != OS_ERR_OK) {
            os_log(OS_LOG_ERROR, "模块 %s 初始化失败，已跳过", m->name);
            FreeLibrary(hmod);
            continue;
        }
        if (g_app.mod_count >= OS_MAX_MODULES) {
            os_log(OS_LOG_WARN, "模块数量超限，%s 未加载", m->name);
            if (m->deinit) m->deinit(ctx);
            FreeLibrary(hmod);
            break;
        }
        i = g_app.mod_count;
        memcpy(&g_app.mods[i], m, sizeof(OS_Module));
        g_app.mod_ctx[i] = ctx;
        g_app.mod_count++;
        loaded++;
        os_log(OS_LOG_INFO, "已加载模块: %s v%s - %s", m->name, m->version, m->description);
    } while (FindNextFileW(h, &fd));
    FindClose(h);

    /* 选择驱动模块：优先 JLink */
    g_app.driver = NULL;
    g_app.driver_ctx = NULL;
    {
        int i;
        for (i = 0; i < g_app.mod_count; i++) {
            OS_Module* m = &g_app.mods[i];
            if ((m->capabilities & OS_CAP_DRIVER) && name_has_jlink(m->name)) {
                g_app.driver = m;
                g_app.driver_ctx = g_app.mod_ctx[i];
                break;
            }
        }
        if (!g_app.driver) {
            for (i = 0; i < g_app.mod_count; i++) {
                OS_Module* m = &g_app.mods[i];
                if (m->capabilities & OS_CAP_DRIVER) {
                    g_app.driver = m;
                    g_app.driver_ctx = g_app.mod_ctx[i];
                    break;
                }
            }
        }
    }
    /* 窗口模块列表 */
    g_app.winmod_count = 0;
    {
        int i;
        for (i = 0; i < g_app.mod_count; i++) {
            OS_Module* m = &g_app.mods[i];
            if (m->capabilities & OS_CAP_WINDOW) {
                g_app.winmods[g_app.winmod_count] = m;
                g_app.winmod_ctx[g_app.winmod_count] = g_app.mod_ctx[i];
                g_app.winmod_count++;
            }
        }
    }
    if (g_app.driver)
        os_log(OS_LOG_INFO, "驱动模块: %s", g_app.driver->name);
    else
        os_log(OS_LOG_WARN, "未找到驱动模块（J-Link 等）");
    return loaded;
}

void os_modmgr_shutdown(void)
{
    int i;
    if (g_app.driver && g_app.driver->command) {
        g_app.driver->command(g_app.driver_ctx, OS_CMD_DISCONNECT, NULL, NULL);
    }
    for (i = g_app.mod_count - 1; i >= 0; i--) {
        if (g_app.mods[i].deinit) g_app.mods[i].deinit(g_app.mod_ctx[i]);
    }
    g_app.mod_count = 0;
    g_app.driver = NULL;
    g_app.winmod_count = 0;
}
