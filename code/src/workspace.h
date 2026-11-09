#ifndef OS_WORKSPACE_H
#define OS_WORKSPACE_H

#include <windows.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_MAX_PROJECTS 16

typedef enum {
    OS_PROJ_PROBE = 0, /* J-Link / ST-Link / miniWiggler(DAS)（drivers[] 下标） */
    OS_PROJ_XCP   = 1,
    OS_PROJ_NET   = 2  /* 网络远程设备（network.dll） */
} OS_ProjKind;

/* 设备类型（左树设备节点）。与 OS_ProjKind 是"类别/具体型号"两层关系：
 * J-Link / ST-Link / miniWiggler 都属于 OS_PROJ_PROBE，XCP/网络 各自一类。 */
typedef enum {
    OS_DEVK_UNKNOWN = 0,
    OS_DEVK_JLINK   = 1,
    OS_DEVK_STLINK  = 2,
    OS_DEVK_DAS     = 3,
    OS_DEVK_XCP     = 4,
    OS_DEVK_NET     = 5
} OS_DeviceKind;

struct OS_ElfFile;

typedef struct OS_Project {
    int         id;                 /* 稳定 id，写入 leaf/sample */
    char        name[64];           /* 设备显示名（可重命名；同类设备按 ID 区分） */
    OS_ProjKind kind;
    int         dev_kind;           /* OS_DeviceKind；旧工程无此字段时按驱动名推导 */
    int         driver_index;       /* kind==PROBE 时：g_app.drivers 下标，-1=未选 */
    int         connected;          /* UI/状态镜像 */
    wchar_t     elf_path[MAX_PATH];
    wchar_t     a2l_path[MAX_PATH];
    wchar_t     map_path[MAX_PATH]; /* XCP 符号来源（GNU/Keil/IAR/TASKING .map） */
    char        xcp_transport[16];
    char        xcp_port[64];
    int         xcp_baud;
    /* B2: 每设备连接配置（设备配置对话框读写自身 Project） */
    int         iface;              /* 0=SWD 1=JTAG */
    int         speed_khz;
    int         probe_index;        /* 扫描列表下标（-1=按 serial 或自动） */
    int         connect_under_reset;/* 1=复位下连接（打断目标）；0=普通 */
    int         attach_default;     /* 1=设备配置页“连接”按 Attach（不复位/不停核） */
    char        serial[64];         /* 探针序列号 / DAS 服务器名（同类设备按此区分） */
    char        device[128];
    /* 网络设备 */
    char        net_ip[64];
    int         net_port;
    int         net_mode;           /* 0=客户端连接 1=服务端监听 */
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

/* ---- 设备列表（左树设备节点） ---- */
const char* os_ws_dev_kind_name(int dev_kind);     /* "J-Link"/"ST-Link"/"miniWiggler"/"XCP"/"网络" */
const char* os_ws_dev_kind_short(int dev_kind);    /* "JLink"/"STLink"/"DAS"/"XCP"/"NET" */
int  os_ws_dev_kind_from_module(const char* module_name); /* 驱动模块名 -> OS_DEVK_* */
int  os_ws_add_device(int dev_kind, const char* name);    /* 返回项目下标，失败 -1 */
int  os_ws_remove_device(int index);                       /* 断开由调用方负责 */
int  os_ws_device_index_by_id(int id);
int  os_ws_bind_device_driver(OS_Project* p);              /* 按 dev_kind 绑定驱动；0=已绑定 */
int  os_ws_first_driver_for_kind(int dev_kind);            /* g_app.drivers 下标，-1=无 */
int  os_ws_default_device_index(void);                     /* 首个 PROBE 设备下标 */
int  os_ws_save_auto(void);                                /* %LOCALAPPDATA%\OpenScope\workspace.json */
int  os_ws_load_auto(void);

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
