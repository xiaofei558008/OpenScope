#ifndef OS_WORKSPACE_H
#define OS_WORKSPACE_H

#include <windows.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_MAX_PROJECTS 8

typedef enum {
    OS_PROJ_PROBE = 0, /* J-Link / ST-Link（drivers[] 下标） */
    OS_PROJ_XCP   = 1
} OS_ProjKind;

struct OS_ElfFile;

typedef struct OS_Project {
    int         id;                 /* 稳定 id，写入 leaf/sample */
    char        name[64];
    OS_ProjKind kind;
    int         driver_index;       /* kind==PROBE 时：g_app.drivers 下标，-1=未选 */
    int         connected;          /* UI/状态镜像 */
    wchar_t     elf_path[MAX_PATH];
    wchar_t     a2l_path[MAX_PATH];
    char        xcp_transport[16];
    char        xcp_port[64];
    int         xcp_baud;
    /* B2: 每 Project 连接配置（工具栏读写活动 Project） */
    int         iface;              /* 0=SWD 1=JTAG */
    int         speed_khz;
    int         probe_index;        /* 仿真器下拉下标 */
    char        device[128];
    struct OS_ElfFile* elf;         /* 运行时持有；不写 JSON */
} OS_Project;

typedef struct OS_Workspace {
    char        name[128];
    wchar_t     path[MAX_PATH];     /* 持久化文件路径 */
    OS_Project  projects[OS_MAX_PROJECTS];
    int         project_count;
    int         active;             /* 当前 UI 操作的 project 下标 */
} OS_Workspace;

void os_ws_init(void);
OS_Workspace* os_ws(void);
OS_Project*   os_ws_active(void);
OS_Project*   os_ws_project(int id);          /* 按 id 查找 */
OS_Project*   os_ws_project_at(int index);
int           os_ws_active_id(void);

/* 增删改 */
int  os_ws_add_project(const char* name, OS_ProjKind kind, int driver_index);
int  os_ws_remove_project(int index);
void os_ws_set_active(int index);

/* 连接解析：返回用于读内存的模块；out_ctx 可空 */
struct OS_Module;
int os_ws_resolve_mod(const OS_Project* pr, struct OS_Module** out_mod, void** out_ctx);

struct OS_Leaf;
/* 显示名：有 Project 时 "Name::var"，否则仅变量名 */
void os_ws_leaf_label(const struct OS_Leaf* L, char* out, int cap);

/* 持久化 workspace.json */
int  os_ws_save(const wchar_t* path_w);
int  os_ws_load(const wchar_t* path_w);
int  os_ws_save_dialog(HWND owner);
int  os_ws_load_dialog(HWND owner);

#ifdef __cplusplus
}
#endif

#endif
