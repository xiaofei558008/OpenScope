/* 单元测试用 Workspace 桩：满足 vartree 对 os_ws_* 的链接依赖 */
#include "app.h"
#include "workspace.h"
#include <string.h>

static OS_Workspace g_ws_stub;

void os_ws_init(void)
{
    memset(&g_ws_stub, 0, sizeof(g_ws_stub));
    g_ws_stub.project_count = 1;
    g_ws_stub.projects[0].id = 1;
    _snprintf(g_ws_stub.projects[0].name, sizeof(g_ws_stub.projects[0].name), "Test");
}

OS_Workspace* os_ws(void)
{
    if (g_ws_stub.project_count <= 0) os_ws_init();
    return &g_ws_stub;
}

OS_Project* os_ws_active(void) { return os_ws_project_at(os_ws()->active); }
int os_ws_active_id(void)
{
    OS_Project* p = os_ws_active();
    return p ? p->id : 0;
}
OS_Project* os_ws_project_at(int index)
{
    if (index < 0 || index >= os_ws()->project_count) return NULL;
    return &os_ws()->projects[index];
}
OS_Project* os_ws_project(int id)
{
    int i;
    for (i = 0; i < os_ws()->project_count; i++)
        if (os_ws()->projects[i].id == id) return &os_ws()->projects[i];
    return NULL;
}
int os_ws_add_project(const char* name, OS_ProjKind kind, int driver_index)
{
    (void)name; (void)kind; (void)driver_index;
    return -1;
}
int os_ws_remove_project(int index) { (void)index; return -1; }
void os_ws_set_active(int index) { if (index >= 0) os_ws()->active = index; }
int os_ws_resolve_mod(const OS_Project* pr, OS_Module** out_mod, void** out_ctx)
{
    (void)pr;
    if (out_mod) *out_mod = NULL;
    if (out_ctx) *out_ctx = NULL;
    return -1;
}
void os_ws_leaf_label(const OS_Leaf* L, char* out, int cap)
{
    if (!out || cap < 1) return;
    out[0] = 0;
    if (L) _snprintf(out, cap, "%s", L->name);
}
const char* os_ws_dev_kind_name(int dev_kind)
{
    switch (dev_kind) {
    case OS_DEVK_JLINK:  return "J-Link";
    case OS_DEVK_STLINK: return "ST-Link";
    case OS_DEVK_DAS:    return "miniWiggler";
    case OS_DEVK_XCP:    return "XCP";
    case OS_DEVK_NET:    return "网络";
    default:             return "设备";
    }
}
int os_ws_save(const wchar_t* path_w) { (void)path_w; return -1; }
int os_ws_load(const wchar_t* path_w) { (void)path_w; return -1; }
int os_ws_save_dialog(HWND owner) { (void)owner; return -1; }
int os_ws_load_dialog(HWND owner) { (void)owner; return -1; }
