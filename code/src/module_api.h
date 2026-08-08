/*
 * OpenScope Module API v1
 *
 * 模块接口定义：主框架与功能模块(dll)之间的 C ABI 契约。
 * 每个模块 dll 只需导出 os_module_get()，返回模块描述与回调表。
 * 编译模块时需定义 OPENSCOPE_MODULE_BUILD。
 */
#ifndef OPENSCOPE_MODULE_API_H
#define OPENSCOPE_MODULE_API_H

#include <windows.h>
#include <stdint.h>

#define OS_API_VERSION 2

/* 模块能力标志 */
#define OS_CAP_DRIVER 0x0001 /* 提供 MCU 访问（连接/读写） */
#define OS_CAP_WINDOW 0x0002 /* 提供自定义窗口类型 */

/* 日志级别 */
#define OS_LOG_INFO  0
#define OS_LOG_WARN  1
#define OS_LOG_ERROR 2
#define OS_LOG_DEBUG 3

/* 通用错误码 */
#define OS_ERR_OK             0
#define OS_ERR_FAIL          -1
#define OS_ERR_INVALID_ARG   -2
#define OS_ERR_NO_DEVICE     -3
#define OS_ERR_NOT_CONNECTED -4
#define OS_ERR_TIMEOUT       -5
#define OS_ERR_CANCELED      -6
#define OS_ERR_BUSY          -7

/* OS_Module::command 命令号 */
enum {
    OS_CMD_CONFIGURE    = 1, /* in: HWND parent，打开配置对话框 */
    OS_CMD_CONNECT      = 2, /* in: OS_ConnectCfg*，out: int* 错误码 */
    OS_CMD_DISCONNECT   = 3, /* 断开连接 */
    OS_CMD_IS_CONNECTED = 4, /* out: int* (1/0) */
    OS_CMD_SCAN         = 5, /* in: OS_ScanReq*，枚举仿真器 */
    OS_CMD_READ_MEM     = 6, /* in: OS_MemReq*，返回字节数或负错误码 */
    OS_CMD_WRITE_MEM    = 7, /* in: OS_MemReq*, return 0=OK or negative error */
    OS_CMD_GET_INFO     = 8, /* out: OS_DriverInfo* */
    OS_CMD_HALT         = 9,
    OS_CMD_GO           = 10,
    OS_CMD_RESET        = 11,
    OS_CMD_ELF_RELOADED = 12 /* ELF 重新加载后由框架广播，模块应重解析窗口变量 */
};

/* 仿真口类型 */
#define OS_IF_SWD  0
#define OS_IF_JTAG 1

typedef struct OS_DeviceInfo {
    int  index;
    char serial[64];
    char name[128];
} OS_DeviceInfo;

typedef struct OS_ScanReq {
    OS_DeviceInfo* items;
    int capacity;
    int count;
} OS_ScanReq;

typedef struct OS_ConnectCfg {
    int  iface;               /* OS_IF_SWD / OS_IF_JTAG（勿用 interface：Windows SDK 宏） */
    int  speed_khz;           /* 0 = 自动 */
    char device[128];         /* 目标器件型号，空 = 默认 */
    int  probe_index;         /* -1 = 自动 */
    char serial[64];          /* 仿真器序列号，空 = 自动 */
    int  connect_under_reset; /* 0/1 */
} OS_ConnectCfg;

typedef struct OS_DriverInfo {
    char name[64];
    char version[32];
    char dll_version[32];
    char emulator[128];
    int  hw_version;
    int  fw_version;
    int  connected;
} OS_DriverInfo;

typedef struct OS_MemReq {
    uint64_t address;
    uint32_t size;
    void*    data;
} OS_MemReq;

/* 采集样本：框架在轮询线程中生成，分发给窗口模块与绘图/记录器 */
typedef struct OS_Sample {
    int64_t  ts_us;   /* Unix 纪元（1970-01-01）微秒 */
    int      var_id;  /* 框架叶子变量 ID */
    uint64_t address;
    uint8_t  raw[8];
    int      size;
    double   value;   /* 数值解释 */
    char     text[64];
    int      written; /* 1 = 由用户写入回读产生的样本 */
} OS_Sample;

struct OS_Variable; /* elf.h 中定义 */

/* 框架提供给模块的回调 */
typedef struct OS_Framework {
    int  api_version;
    void (*log)(int level, const char* fmt, ...);
    void (*post_msg)(UINT msg, WPARAM wParam, LPARAM lParam);
    const struct OS_Variable* (*find_variable)(const char* name);
    int  (*leaf_count)(void);
    const char* (*leaf_name)(int id);
    const OS_Sample* (*leaf_sample)(int id); /* 最新样本，可能为 NULL */

    /* ---- v1 扩展回调 ---- */
    /* 弹出“添加变量”模糊搜索对话框，选择后写入 out（叶变量完整路径），
     * 返回 1 成功 / 0 取消。模块可反复调用。 */
    int  (*pick_variable)(HWND parent, char* out, int out_len);
    /* 写入叶变量（按当前值类型解释输入文本），err 可传 NULL。 */
    int  (*write_leaf)(int id, double value, char* err, int err_len);
    /* ELF 已重新加载并刷新叶变量表后通知模块，模块应更新窗口中的变量 id
     * （按名称重新解析，找不到的置为 -1）。 */
    void (*on_elf_reloaded)(void);
    /* 按子串模糊搜索叶变量，返回匹配数，ids 最多 max_ids 个。 */
    int  (*leaf_find)(const char* needle, int* ids, int max_ids);
} OS_Framework;

typedef struct OS_WindowType {
    const char* type;         /* 类型标识，如 "scope.bar" */
    const char* display_name; /* 显示名，如 "仪表窗口" */
} OS_WindowType;

/* 框架定义：窗口关闭请求（模块窗口向主窗口投递以关闭自身） */
#define OS_WM_WIN_CLOSED (WM_APP + 5)

/* 模块导出结构 */
typedef struct OS_Module {
    int      api_version;
    uint32_t capabilities;
    char     name[64];
    char     version[32];
    char     description[256];
    const OS_WindowType* window_types; /* 以 {NULL,NULL} 结尾；可为 NULL */

    int  (*init)(const OS_Framework* fw, void** out_ctx);
    void (*deinit)(void* ctx);
    int  (*command)(void* ctx, int cmd, void* in, void* out);
    HWND (*create_window)(void* ctx, const char* type, HWND parent,
                          int x, int y, int w, int h, const char* title);
    void (*destroy_window)(void* ctx, HWND hwnd);
    void (*on_samples)(void* ctx, const OS_Sample* samples, int count);
    /* ELF 重新加载并刷新叶变量表后由框架调用；模块应把窗口中记录的
     * 变量名重新解析为新 id（找不到的置为 -1）。可留空。 */
    void (*on_reload)(void* ctx);
    /* v2：框架向指定窗口添加变量（树右键入口）；hwnd 为 create_window 返回的句柄，
     * leaf_id 为框架叶变量编号，返回 OS_ERR_OK 成功。可留空
     * （只有 api_version >= 2 的模块才会被调用）。 */
    int  (*win_add_var)(void* ctx, HWND hwnd, int leaf_id);
} OS_Module;

#ifdef OPENSCOPE_MODULE_BUILD
#define OS_MODULE_EXPORT __declspec(dllexport)
#else
#define OS_MODULE_EXPORT
#endif

#define OS_MODULE_EXPORT_NAME "os_module_get"
typedef const OS_Module* (*os_module_get_fn)(void);

#ifdef __cplusplus
extern "C" {
#endif
OS_MODULE_EXPORT const OS_Module* os_module_get(void);
#ifdef __cplusplus
}
#endif

#endif /* OPENSCOPE_MODULE_API_H */
