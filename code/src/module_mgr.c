#include "app.h"
#include "module_mgr.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* 日志来源前缀：每个模块拿到一份独立的 OS_Framework 副本，其 log 指向按槽位区分的
 * 跳板函数，统一输出 "<模块名>: <消息>"（如 "das: 已连接 ..."）。若消息已自带
 * "<模块名>:" 前缀（remote/network/aiapi 历史写法）则不重复添加。 */
static OS_Framework s_mod_fw[OS_MAX_MODULES];
static char s_mod_src[OS_MAX_MODULES][32];

static void modlog_v(int slot, int level, const char* fmt, va_list ap)
{
    char buf[1024];
    const char* src = s_mod_src[slot];
    size_t n = strlen(src);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    buf[sizeof(buf) - 1] = 0;
    if (n == 0 || (strncmp(buf, src, n) == 0 && (buf[n] == ':' || buf[n] == ' ')))
        os_log(level, "%s", buf);
    else
        os_log(level, "%s: %s", src, buf);
}
#define MODLOG_TRAMP(i) static void modlog_##i(int lv, const char* fmt, ...) \
    { va_list ap; va_start(ap, fmt); modlog_v(i, lv, fmt, ap); va_end(ap); }
MODLOG_TRAMP(0)  MODLOG_TRAMP(1)  MODLOG_TRAMP(2)  MODLOG_TRAMP(3)
MODLOG_TRAMP(4)  MODLOG_TRAMP(5)  MODLOG_TRAMP(6)  MODLOG_TRAMP(7)
MODLOG_TRAMP(8)  MODLOG_TRAMP(9)  MODLOG_TRAMP(10) MODLOG_TRAMP(11)
MODLOG_TRAMP(12) MODLOG_TRAMP(13) MODLOG_TRAMP(14) MODLOG_TRAMP(15)
MODLOG_TRAMP(16) MODLOG_TRAMP(17) MODLOG_TRAMP(18) MODLOG_TRAMP(19)
MODLOG_TRAMP(20) MODLOG_TRAMP(21) MODLOG_TRAMP(22) MODLOG_TRAMP(23)
MODLOG_TRAMP(24) MODLOG_TRAMP(25) MODLOG_TRAMP(26) MODLOG_TRAMP(27)
MODLOG_TRAMP(28) MODLOG_TRAMP(29) MODLOG_TRAMP(30) MODLOG_TRAMP(31)
typedef void (*modlog_fn)(int, const char*, ...);
static const modlog_fn s_modlog[OS_MAX_MODULES] = {
    modlog_0, modlog_1, modlog_2, modlog_3, modlog_4, modlog_5, modlog_6, modlog_7,
    modlog_8, modlog_9, modlog_10, modlog_11, modlog_12, modlog_13, modlog_14, modlog_15,
    modlog_16, modlog_17, modlog_18, modlog_19, modlog_20, modlog_21, modlog_22, modlog_23,
    modlog_24, modlog_25, modlog_26, modlog_27, modlog_28, modlog_29, modlog_30, modlog_31 };

static const OS_Framework* mod_fw_for_slot(int slot, const char* name)
{
    s_mod_fw[slot] = g_app.fw;
    s_mod_fw[slot].log = s_modlog[slot];
    _snprintf(s_mod_src[slot], sizeof(s_mod_src[slot]) - 1, "%s", name ? name : "");
    s_mod_src[slot][sizeof(s_mod_src[slot]) - 1] = 0;
    return &s_mod_fw[slot];
}

static void dll_dir(wchar_t* out, int outlen)
{
    wchar_t exe[MAX_PATH];
    wchar_t test[MAX_PATH];
    wchar_t* slash;
    GetModuleFileNameW(NULL, exe, MAX_PATH);
    slash = wcsrchr(exe, L'\\');
    if (slash) *slash = 0;
    /* 安装布局：exe 同目录下 dll\；开发布局：bin\Release\..\..\dll */
    _snwprintf(test, outlen, L"%s\\dll", exe);
    if (GetFileAttributesW(test) != INVALID_FILE_ATTRIBUTES)
        _snwprintf(out, outlen, L"%s", test);
    else
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
        if (g_app.mod_count >= OS_MAX_MODULES) {
            os_log(OS_LOG_WARN, "模块数量超限，%s 未加载", m->name);
            FreeLibrary(hmod);
            break;
        }
        if (m->init && m->init(mod_fw_for_slot(g_app.mod_count, m->name), &ctx) != OS_ERR_OK) {
            os_log(OS_LOG_ERROR, "模块 %s 初始化失败，已跳过", m->name);
            FreeLibrary(hmod);
            continue;
        }
        i = g_app.mod_count;
        memcpy(&g_app.mods[i], m, sizeof(OS_Module));
        g_app.mod_ctx[i] = ctx;
        g_app.mod_count++;
        loaded++;
        os_log(OS_LOG_INFO, "已加载模块: %s v%s - %s", m->name, m->version, m->description);
    } while (FindNextFileW(h, &fd));
    FindClose(h);

    /* 探针驱动：仅 OS_CAP_DRIVER 且非 XCP（J-Link/ST-Link 一组互斥） */
    g_app.driver = NULL;
    g_app.driver_ctx = NULL;
    g_app.driver_count = 0;
    g_app.xcpmod = NULL;
    g_app.xcpmod_ctx = NULL;
    g_app.xcp_connected = 0;
    {
        int i, jlink_idx = -1;
        for (i = 0; i < g_app.mod_count; i++) {
            OS_Module* m = &g_app.mods[i];
            if (m->capabilities & OS_CAP_XCP) {
                if (!g_app.xcpmod) {
                    g_app.xcpmod = m;
                    g_app.xcpmod_ctx = g_app.mod_ctx[i];
                }
                continue;
            }
            if (m->capabilities & OS_CAP_DRIVER) {
                if (jlink_idx < 0 && name_has_jlink(m->name)) jlink_idx = g_app.driver_count;
                g_app.drivers[g_app.driver_count] = m;
                g_app.driver_ctxs[g_app.driver_count] = g_app.mod_ctx[i];
                g_app.driver_count++;
            }
        }
        if (g_app.driver_count > 0) {
            int sel = (jlink_idx >= 0) ? jlink_idx : 0;
            g_app.driver = g_app.drivers[sel];
            g_app.driver_ctx = g_app.driver_ctxs[sel];
        }
    }
    if (g_app.xcpmod)
        os_log(OS_LOG_INFO, "XCP 模块: %s（与探针/网络并行）", g_app.xcpmod->name);
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
    /* 网络模块（OS_CAP_NET，需求 14） */
    g_app.netmod = NULL;
    g_app.netmod_ctx = NULL;
    {
        int i;
        for (i = 0; i < g_app.mod_count; i++) {
            if (g_app.mods[i].capabilities & OS_CAP_NET) {
                g_app.netmod = &g_app.mods[i];
                g_app.netmod_ctx = g_app.mod_ctx[i];
                break;
            }
        }
    }
    if (g_app.netmod)
        os_log(OS_LOG_INFO, "网络模块: %s", g_app.netmod->name);
    if (g_app.driver)
        os_log(OS_LOG_INFO, "驱动模块: %s", g_app.driver->name);
    else
        os_log(OS_LOG_WARN, "未找到驱动模块（J-Link 等）");
    return loaded;
}

int os_modmgr_driver_count(void)
{
    return g_app.driver_count;
}

const char* os_modmgr_driver_name(int idx)
{
    if (idx < 0 || idx >= g_app.driver_count) return NULL;
    return g_app.drivers[idx]->name;
}

int os_modmgr_driver_index(void)
{
    int i;
    for (i = 0; i < g_app.driver_count; i++)
        if (g_app.drivers[i] == g_app.driver) return i;
    return -1;
}

int os_modmgr_select_driver(int idx)
{
    int c = 0;
    if (idx < 0 || idx >= g_app.driver_count) return OS_ERR_INVALID_ARG;
    /* Workspace 多 MCU：切换 UI 当前驱动时不断开其它驱动，允许 J-Link/ST-Link 并行连接 */
    g_app.driver = g_app.drivers[idx];
    g_app.driver_ctx = g_app.driver_ctxs[idx];
    if (g_app.driver && g_app.driver->command)
        g_app.driver->command(g_app.driver_ctx, OS_CMD_IS_CONNECTED, NULL, &c);
    g_app.connected = c;
    return OS_ERR_OK;
}

void os_modmgr_shutdown(void)
{
    int i;
    int done[OS_MAX_MODULES];
    memset(done, 0, sizeof(done));

    /* 1) 先停 AI：其 WebSocket 客户端线程可能仍在调 xcp/driver */
    for (i = 0; i < g_app.mod_count; i++) {
        if (g_app.mods[i].capabilities & OS_CAP_AI) {
            if (g_app.mods[i].deinit) g_app.mods[i].deinit(g_app.mod_ctx[i]);
            done[i] = 1;
        }
    }

    /* 2) 断开 XCP 并清宿主指针，避免 deinit 后悬空调用 */
    if (g_app.xcpmod && g_app.xcpmod->command)
        g_app.xcpmod->command(g_app.xcpmod_ctx, OS_CMD_DISCONNECT, NULL, NULL);
    g_app.xcpmod = NULL;
    g_app.xcpmod_ctx = NULL;
    g_app.xcp_connected = 0;

    /* 断开全部探针驱动（Workspace 多 MCU 可能并行连接） */
    for (i = 0; i < g_app.driver_count; i++) {
        if (g_app.drivers[i] && g_app.drivers[i]->command)
            g_app.drivers[i]->command(g_app.driver_ctxs[i], OS_CMD_DISCONNECT, NULL, NULL);
    }
    g_app.connected = 0;

    /* 3) 其余模块逆序卸载 */
    for (i = g_app.mod_count - 1; i >= 0; i--) {
        if (!done[i] && g_app.mods[i].deinit)
            g_app.mods[i].deinit(g_app.mod_ctx[i]);
    }
    g_app.mod_count = 0;
    g_app.driver = NULL;
    g_app.driver_ctx = NULL;
    g_app.netmod = NULL;
    g_app.netmod_ctx = NULL;
    g_app.winmod_count = 0;
}
