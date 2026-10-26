#include "app.h"
#include "module_mgr.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "target_alias.h"

/* 日志来源前缀：每个模块拿到一份独立的 OS_Framework 副本，其 log 指向按槽位区分的
 * 跳板函数，统一输出 "<模块名>: <消息>"（如 "das: 已连接 ..."）。若消息已自带
 * "<模块名>:" 前缀（remote/network/aiapi 历史写法）则不重复添加。 */
static OS_Framework s_mod_fw[OS_MAX_MODULES];
static char s_mod_src[OS_MAX_MODULES][32];
/* 目标别名（Tom：实时日志统一 "<目标别名> <模块>: 消息"，如 "tc3x das: ..."）。
 * 只对“数据源”模块（探针驱动 OS_CAP_DRIVER、XCP）设置；aiapi/network/remote 等没有别名，仍是 "<模块>: 消息"。
 * 自动值（由芯片/驱动推导）不覆盖用户值。多目标（docs/multi_target_design.md M2/M3）时 alias = Target.id。
 * 日志可来自任意线程 → SRW 锁保护读写。 */
static char s_mod_alias[OS_MAX_MODULES][OS_ALIAS_MAX];
static int  s_mod_alias_user[OS_MAX_MODULES];
static SRWLOCK s_alias_lock = SRWLOCK_INIT;

static void modlog_v(int slot, int level, const char* fmt, va_list ap)
{
    char buf[1024], line[1100], alias[OS_ALIAS_MAX];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    buf[sizeof(buf) - 1] = 0;
    AcquireSRWLockShared(&s_alias_lock);
    memcpy(alias, s_mod_alias[slot], sizeof(alias));
    ReleaseSRWLockShared(&s_alias_lock);
    alias[sizeof(alias) - 1] = 0;
    os_log_format_src(alias, s_mod_src[slot], buf, line, (int)sizeof(line));
    os_log(level, "%s", line);
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
    AcquireSRWLockExclusive(&s_alias_lock);
    s_mod_alias[slot][0] = 0;                  /* 加载时还不知能力位：见 alias_init_targets() */
    s_mod_alias_user[slot] = 0;
    ReleaseSRWLockExclusive(&s_alias_lock);
    return &s_mod_fw[slot];
}

static int slot_of_driver(int drv_idx)
{
    int i;
    if (drv_idx < 0 || drv_idx >= g_app.driver_count) return -1;
    for (i = 0; i < g_app.mod_count; i++)
        if (&g_app.mods[i] == g_app.drivers[drv_idx]) return i;
    return -1;
}

/* 同一别名被其它槽占用时加后缀 _1、_2…（多目标同型号芯片） */
static void alias_unique(int slot, char* a, int cap)
{
    char base[OS_ALIAS_MAX];
    int k, i, clash;
    _snprintf(base, sizeof(base) - 1, "%s", a); base[sizeof(base) - 1] = 0;
    for (k = 1; k < 100; k++) {
        clash = 0;
        for (i = 0; i < g_app.mod_count; i++)
            if (i != slot && s_mod_alias[i][0] && strcmp(s_mod_alias[i], a) == 0) { clash = 1; break; }
        if (!clash) return;
        base[OS_ALIAS_MAX - 4] = 0;
        _snprintf(a, cap - 1, "%s_%d", base, k); a[cap - 1] = 0;
    }
}

static void alias_set_slot(int slot, const char* alias, int user)
{
    char a[OS_ALIAS_MAX], old[OS_ALIAS_MAX];
    int changed;
    if (slot < 0 || slot >= g_app.mod_count) return;
    if (!user && s_mod_alias_user[slot]) return;           /* 用户值优先 */
    if (os_alias_sanitize(alias, a, (int)sizeof(a)) == 0) os_alias_default(s_mod_src[slot], a, (int)sizeof(a));
    AcquireSRWLockExclusive(&s_alias_lock);
    alias_unique(slot, a, (int)sizeof(a));
    memcpy(old, s_mod_alias[slot], sizeof(old));
    changed = strcmp(old, a) != 0;
    memcpy(s_mod_alias[slot], a, sizeof(a));
    if (user) s_mod_alias_user[slot] = (alias && alias[0]) ? 1 : 0;
    ReleaseSRWLockExclusive(&s_alias_lock);
    if (changed && old[0])
        os_log(OS_LOG_INFO, "目标别名: %s %s → %s%s", old, s_mod_src[slot], a, user ? "（用户设置）" : "（自动）");
}

/* 加载完成后：探针驱动与 XCP 是数据源，给默认别名 "<驱动>0" */
static void alias_init_targets(void)
{
    int i;
    for (i = 0; i < g_app.mod_count; i++)
        if (g_app.mods[i].capabilities & (OS_CAP_DRIVER | OS_CAP_XCP)) {
            char a[OS_ALIAS_MAX];
            os_alias_default(g_app.mods[i].name, a, (int)sizeof(a));
            alias_set_slot(i, a, 0);
        }
}

int os_modmgr_driver_alias(int drv_idx, char* out, int cap)
{
    int slot = slot_of_driver(drv_idx);
    if (!out || cap <= 0) return -1;
    out[0] = 0;
    if (slot < 0) return -1;
    AcquireSRWLockShared(&s_alias_lock);
    _snprintf(out, cap - 1, "%s", s_mod_alias[slot]); out[cap - 1] = 0;
    ReleaseSRWLockShared(&s_alias_lock);
    return s_mod_alias_user[slot];
}

int os_modmgr_set_driver_alias(int drv_idx, const char* alias, int user)
{
    int slot = slot_of_driver(drv_idx);
    if (slot < 0) return OS_ERR_INVALID_ARG;
    if (user && alias && alias[0]) {
        char a[OS_ALIAS_MAX];
        if (os_alias_sanitize(alias, a, (int)sizeof(a)) == 0) return OS_ERR_INVALID_ARG;
    }
    if (user && (!alias || !alias[0])) {                    /* 清除用户值 → 回到驱动默认，下次连接再自动推导 */
        s_mod_alias_user[slot] = 0;
        alias_set_slot(slot, NULL, 0);
        return OS_ERR_OK;
    }
    alias_set_slot(slot, alias, user);
    return OS_ERR_OK;
}

void os_modmgr_auto_alias_pre(int drv_idx, const char* device)
{
    /* 连接前：只有能从器件名推导出芯片时才改（驱动在 CONNECT 里打的“已连接”行就带上新别名）；
     * 推不出（如 DAS 不看器件名、"Cortex-M4"）则保留现值，连接成功后再按 GET_INFO 推导 */
    char a[OS_ALIAS_MAX];
    int slot = slot_of_driver(drv_idx);
    if (slot < 0 || !device || !device[0]) return;
    if (os_alias_from_chip(device, s_mod_src[slot], a, (int)sizeof(a)))
        alias_set_slot(slot, a, 0);
}

void os_modmgr_auto_alias(int drv_idx, const char* device, const char* info_text)
{
    char t[512], a[OS_ALIAS_MAX];
    int slot = slot_of_driver(drv_idx);
    if (slot < 0) return;
    _snprintf(t, sizeof(t) - 1, "%s %s", device ? device : "", info_text ? info_text : "");
    t[sizeof(t) - 1] = 0;
    os_alias_from_chip(t, s_mod_src[slot], a, (int)sizeof(a));
    alias_set_slot(slot, a, 0);
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
    alias_init_targets();
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
