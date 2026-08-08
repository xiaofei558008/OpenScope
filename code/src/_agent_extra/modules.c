/*
 * OpenScope module manager: loads dll\*.dll modules, provides the
 * OS_Framework callbacks to modules, and dispatches driver commands.
 */
#include "app.h"

#include <string.h>

static const OS_Variable* fw_find_variable(const char* name)
{
    int idx;
    if (!g_app.elf || !name) return NULL;
    idx = os_elf_find_var(g_app.elf, name);
    return idx >= 0 ? os_elf_var_at(g_app.elf, idx) : NULL;
}

static int fw_leaf_count(void)
{
    return g_app.leaf_count;
}

static const char* fw_leaf_name(int id)
{
    if (id < 0 || id >= g_app.leaf_count) return NULL;
    return g_app.leaves[id].path;
}

static const OS_Sample* fw_leaf_sample(int id)
{
    if (id < 0 || id >= g_app.leaf_count) return NULL;
    return g_app.leaves[id].last.size ? &g_app.leaves[id].last : NULL;
}

static void fw_post_msg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (g_app.hMain) PostMessageW(g_app.hMain, msg, wParam, lParam);
}

static int fw_pick_variable(HWND parent, char* out, int out_len)
{
    return ui_pick_variable(parent, out, out_len);
}

static int fw_write_leaf(int id, double value, char* err, int err_len)
{
    return os_write_leaf(id, value, err, err_len);
}

static void fw_on_elf_reloaded(void)
{
    /* 通知各模块（由 os_notify_modules_reload 统一触发）；此处供模块主动调用 */
    os_notify_modules_reload();
}

static int fw_leaf_find(const char* needle, int* ids, int max_ids)
{
    int i, n = 0;
    size_t nn;
    size_t nh;
    if (!needle || max_ids <= 0) return 0;
    nn = strlen(needle);
    if (nn == 0) return 0;
    for (i = 0; i < g_app.leaf_count && n < max_ids; ++i) {
        const char* p = g_app.leaves[i].path;
        size_t j;
        nh = strlen(p);
        if (nn > nh) continue;
        for (j = 0; j + nn <= nh; ++j) {
            size_t k;
            int eq = 1;
            for (k = 0; k < nn; ++k) {
                char a = p[j + k], b = needle[k];
                if (a >= 'A' && a <= 'Z') a += 32;
                if (b >= 'A' && b <= 'Z') b += 32;
                if (a != b) { eq = 0; break; }
            }
            if (eq) { ids[n++] = i; break; }
        }
    }
    return n;
}

static OS_Framework g_fw = {
    OS_API_VERSION,
    os_log,
    fw_post_msg,
    fw_find_variable,
    fw_leaf_count,
    fw_leaf_name,
    fw_leaf_sample,
    fw_pick_variable,
    fw_write_leaf,
    fw_on_elf_reloaded,
    fw_leaf_find
};

/* 模块路径：可执行文件在 bin\Release，模块在 ..\..\dll */
static void module_dir(char* out, int cap)
{
    char exe[MAX_PATH];
    char* slash;
    GetModuleFileNameA(NULL, exe, MAX_PATH);
    slash = strrchr(exe, '\\');
    if (slash) *slash = 0;
    _snprintf(out, cap, "%s\\..\\..\\dll", exe);
}

void os_load_modules(void)
{
    char dir[MAX_PATH], pat[MAX_PATH];
    WIN32_FIND_DATAA fd;
    HANDLE hFind;

    module_dir(dir, sizeof(dir));
    _snprintf(pat, sizeof(pat), "%s\\*.dll", dir);
    hFind = FindFirstFileA(pat, &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        os_log(OS_LOG_WARN, "未找到模块目录: %s", dir);
        return;
    }
    do {
        char path[MAX_PATH];
        HMODULE h;
        os_module_get_fn getfn;
        const OS_Module* m;
        void* ctx = NULL;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        if (_stricmp(fd.cFileName, "JLink_x64.dll") == 0) continue; /* 依赖库而非模块 */
        if (g_app.mod_count >= OS_MAX_MODULES) break;
        _snprintf(path, sizeof(path), "%s\\%s", dir, fd.cFileName);
        h = LoadLibraryA(path);
        if (!h) {
            os_log(OS_LOG_WARN, "加载模块失败: %s (err=%lu)", fd.cFileName, GetLastError());
            continue;
        }
        getfn = (os_module_get_fn)GetProcAddress(h, OS_MODULE_EXPORT_NAME);
        if (!getfn) {
            os_log(OS_LOG_WARN, "%s 不是 OpenScope 模块（无 os_module_get）", fd.cFileName);
            FreeLibrary(h);
            continue;
        }
        m = getfn();
        if (!m || m->api_version != OS_API_VERSION) {
            os_log(OS_LOG_WARN, "%s 模块 API 版本不匹配", fd.cFileName);
            FreeLibrary(h);
            continue;
        }
        if (m->init && m->init(&g_fw, &ctx) != 0) {
            os_log(OS_LOG_ERROR, "%s 模块初始化失败", fd.cFileName);
            FreeLibrary(h);
            continue;
        }
        g_app.mod_handles[g_app.mod_count] = h;
        g_app.mods[g_app.mod_count] = m;
        g_app.mod_ctx[g_app.mod_count] = ctx;
        if ((m->capabilities & OS_CAP_DRIVER) && g_app.driver_idx < 0)
            g_app.driver_idx = g_app.mod_count;
        os_log(OS_LOG_INFO, "已加载模块: %s v%s - %s",
               m->name, m->version, m->description[0] ? m->description : "");
        ++g_app.mod_count;
    } while (FindNextFileA(hFind, &fd));
    FindClose(hFind);
    if (g_app.driver_idx < 0)
        os_log(OS_LOG_WARN, "未找到驱动模块（OS_CAP_DRIVER）");
}

void os_unload_modules(void)
{
    int i;
    for (i = 0; i < g_app.mod_count; ++i) {
        if (g_app.mods[i]->deinit) g_app.mods[i]->deinit(g_app.mod_ctx[i]);
        if (g_app.mod_handles[i]) FreeLibrary(g_app.mod_handles[i]);
    }
    g_app.mod_count = 0;
    g_app.driver_idx = -1;
}

void os_dispatch_samples(const OS_Sample* s, int n)
{
    int i;
    if (!s || n <= 0) return;
    for (i = 0; i < g_app.mod_count; ++i) {
        if (g_app.mods[i]->on_samples)
            g_app.mods[i]->on_samples(g_app.mod_ctx[i], s, n);
    }
}

void os_notify_modules_reload(void)
{
    int i;
    for (i = 0; i < g_app.mod_count; ++i)
        if (g_app.mods[i]->on_reload)
            g_app.mods[i]->on_reload(g_app.mod_ctx[i]);
}

int os_create_window_for_type(const char* type)
{
    int i, j;
    for (i = 0; i < g_app.mod_count; ++i) {
        const OS_WindowType* wt = g_app.mods[i]->window_types;
        if (!wt) continue;
        for (j = 0; wt[j].type; ++j) {
            if (strcmp(wt[j].type, type) == 0) {
                if (g_app.mods[i]->create_window) {
                    HWND hw = g_app.mods[i]->create_window(g_app.mod_ctx[i], type,
                                                          g_app.hMain, 0, 0, 0, 0, wt[j].display_name);
                    return hw ? 0 : -1;
                }
                return -1;
            }
        }
    }
    return -1;
}

void os_build_window_menu(HMENU menu)
{
    int i, j;
    while (GetMenuItemCount(menu) > 0)
        DeleteMenu(menu, 0, MF_BYPOSITION);
    for (i = 0; i < g_app.mod_count; ++i) {
        const OS_WindowType* wt = g_app.mods[i]->window_types;
        if (!wt) continue;
        for (j = 0; wt[j].type; ++j) {
            char buf[128];
            _snprintf(buf, sizeof(buf), "%s", wt[j].display_name);
            AppendMenuA(menu, MF_STRING, 0x4000 + (UINT)i * 16 + (UINT)j, buf);
        }
    }
}

/* ----------------------- driver dispatch -------------------------- */

static const OS_Module* driver(void)
{
    if (g_app.driver_idx >= 0 && g_app.driver_idx < g_app.mod_count)
        return g_app.mods[g_app.driver_idx];
    return NULL;
}

static void* driver_ctx(void)
{
    if (g_app.driver_idx >= 0 && g_app.driver_idx < g_app.mod_count)
        return g_app.mod_ctx[g_app.driver_idx];
    return NULL;
}

int os_driver_scan(OS_ScanReq* req)
{
    const OS_Module* m = driver();
    if (!m || !m->command || !req) return OS_ERR_FAIL;
    return m->command(driver_ctx(), OS_CMD_SCAN, req, NULL);
}

int os_driver_connect(OS_ConnectCfg* cfg)
{
    const OS_Module* m = driver();
    int err = OS_ERR_FAIL;
    if (!m || !m->command || !cfg) return OS_ERR_FAIL;
    m->command(driver_ctx(), OS_CMD_CONNECT, cfg, &err);
    g_app.connected = (err == OS_ERR_OK);
    if (g_app.connected) {
        memset(&g_app.dinfo, 0, sizeof(g_app.dinfo));
        os_driver_get_info(&g_app.dinfo);
    }
    return err;
}

void os_driver_disconnect(void)
{
    const OS_Module* m = driver();
    if (m && m->command) m->command(driver_ctx(), OS_CMD_DISCONNECT, NULL, NULL);
    g_app.connected = 0;
    memset(&g_app.dinfo, 0, sizeof(g_app.dinfo));
}

int os_driver_is_connected(void)
{
    const OS_Module* m = driver();
    int v = 0;
    if (m && m->command) m->command(driver_ctx(), OS_CMD_IS_CONNECTED, NULL, &v);
    return v;
}

int os_driver_read(uint64_t addr, uint32_t size, void* data)
{
    const OS_Module* m = driver();
    OS_MemReq req;
    if (!m || !m->command || !data) return OS_ERR_FAIL;
    req.address = addr;
    req.size = size;
    req.data = data;
    return m->command(driver_ctx(), OS_CMD_READ_MEM, &req, NULL);
}

int os_driver_write(uint64_t addr, uint32_t size, const void* data)
{
    const OS_Module* m = driver();
    OS_MemReq req;
    if (!m || !m->command || !data) return OS_ERR_FAIL;
    req.address = addr;
    req.size = size;
    req.data = (void*)data;
    return m->command(driver_ctx(), OS_CMD_WRITE_MEM, &req, NULL);
}

int os_driver_get_info(OS_DriverInfo* info)
{
    const OS_Module* m = driver();
    if (!m || !m->command || !info) return OS_ERR_FAIL;
    return m->command(driver_ctx(), OS_CMD_GET_INFO, NULL, info);
}

int os_driver_halt(void)
{
    const OS_Module* m = driver();
    if (!m || !m->command) return OS_ERR_FAIL;
    return m->command(driver_ctx(), OS_CMD_HALT, NULL, NULL);
}

int os_driver_go(void)
{
    const OS_Module* m = driver();
    if (!m || !m->command) return OS_ERR_FAIL;
    return m->command(driver_ctx(), OS_CMD_GO, NULL, NULL);
}
