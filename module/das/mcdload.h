/* mcdxdas.dll 动态绑定（DAS64 MCD API），不链接导入库：DAS 未安装时 OpenScope 仍可启动，das 模块仅报告不可用 */
#ifndef OS_MCDLOAD_H
#define OS_MCDLOAD_H
#include <windows.h>
#include "mcd/mcd_api.h"

typedef struct McdApi {
    HMODULE h;
    mcd_return_et (*initialize)(const mcd_api_version_st*, mcd_impl_version_info_st*);
    void          (*exit_)(void);
    mcd_return_et (*qry_servers)(const mcd_char_t*, mcd_bool_t, uint32_t, uint32_t*, mcd_server_info_st*);
    mcd_return_et (*open_server)(const mcd_char_t*, const mcd_char_t*, mcd_server_st**);
    mcd_return_et (*close_server)(const mcd_server_st*);
    mcd_return_et (*qry_systems)(uint32_t, uint32_t*, mcd_core_con_info_st*);
    mcd_return_et (*qry_devices)(const mcd_core_con_info_st*, uint32_t, uint32_t*, mcd_core_con_info_st*);
    mcd_return_et (*qry_cores)(const mcd_core_con_info_st*, uint32_t, uint32_t*, mcd_core_con_info_st*);
    mcd_return_et (*open_core)(const mcd_core_con_info_st*, mcd_core_st**);
    mcd_return_et (*close_core)(const mcd_core_st*);
    void          (*qry_error_info)(const mcd_core_st*, mcd_error_info_st*);
    mcd_return_et (*execute_txlist)(const mcd_core_st*, mcd_txlist_st*);
    mcd_return_et (*run)(const mcd_core_st*, mcd_bool_t);
    mcd_return_et (*stop)(const mcd_core_st*, mcd_bool_t);
    mcd_return_et (*step)(const mcd_core_st*, mcd_bool_t, mcd_core_step_type_et, uint32_t);
    mcd_return_et (*qry_state)(const mcd_core_st*, mcd_core_state_st*);
    mcd_return_et (*qry_rst_classes)(const mcd_core_st*, uint32_t*);
    mcd_return_et (*rst)(const mcd_core_st*, uint32_t, mcd_bool_t);
    mcd_return_et (*qry_reg_groups)(const mcd_core_st*, uint32_t, uint32_t*, mcd_register_group_st*);
    mcd_return_et (*qry_reg_map)(const mcd_core_st*, uint32_t, uint32_t, uint32_t*, mcd_register_info_st*);
    mcd_return_et (*qry_mem_spaces)(const mcd_core_st*, uint32_t, uint32_t*, mcd_memspace_st*);
    mcd_return_et (*qry_server_config)(const mcd_server_st*, uint32_t*, mcd_char_t*);
} McdApi;

/* dir 为空则依次尝试 OPENSCOPE_DAS_DIR、C:\Program Files\DAS64\clients */
static int mcd_bind(McdApi* a, char* err, int errcap)
{
    /* 非 static：每次调用重建，避免保存指向本栈帧 envdir 的悬空指针（deinit 后再绑定） */
    const char* dirs[4];
    char envdir[MAX_PATH], path[MAX_PATH + 32];
    DWORD el;
    int i;
    memset(a, 0, sizeof(*a));
    path[0] = 0;
    el = GetEnvironmentVariableA("OPENSCOPE_DAS_DIR", envdir, sizeof(envdir));
    dirs[0] = (el > 0 && el < sizeof(envdir)) ? envdir : NULL;
    dirs[1] = "C:\\Program Files\\DAS64\\clients";
    dirs[2] = "C:\\Program Files (x86)\\DAS64\\clients";
    dirs[3] = NULL;
    for (i = 0; i < 3 && !a->h; i++) {
        if (!dirs[i]) continue;
        _snprintf(path, sizeof(path) - 1, "%s\\mcdxdas.dll", dirs[i]); path[sizeof(path) - 1] = 0;
        /* 依赖 DLL（das_api.dll 等）在同目录 */
        a->h = LoadLibraryExA(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    }
    if (!a->h) { { _snprintf(err, errcap - 1, "未找到 mcdxdas.dll（请安装 DAS64，或设 OPENSCOPE_DAS_DIR）"); err[errcap - 1] = 0; } return -1; }
#define B(f, n) if (!(*(FARPROC*)&a->f = GetProcAddress(a->h, n))) { { _snprintf(err, errcap - 1, "mcdxdas.dll 缺少导出 %s", n); err[errcap - 1] = 0; } FreeLibrary(a->h); a->h = NULL; return -1; }
    B(initialize, "mcd_initialize_f") B(exit_, "mcd_exit_f") B(qry_servers, "mcd_qry_servers_f")
    B(open_server, "mcd_open_server_f") B(close_server, "mcd_close_server_f") B(qry_systems, "mcd_qry_systems_f")
    B(qry_devices, "mcd_qry_devices_f") B(qry_cores, "mcd_qry_cores_f") B(open_core, "mcd_open_core_f")
    B(close_core, "mcd_close_core_f") B(qry_error_info, "mcd_qry_error_info_f") B(execute_txlist, "mcd_execute_txlist_f")
    B(run, "mcd_run_f") B(stop, "mcd_stop_f") B(step, "mcd_step_f") B(qry_state, "mcd_qry_state_f")
    B(qry_rst_classes, "mcd_qry_rst_classes_f") B(rst, "mcd_rst_f") B(qry_reg_groups, "mcd_qry_reg_groups_f")
    B(qry_reg_map, "mcd_qry_reg_map_f") B(qry_mem_spaces, "mcd_qry_mem_spaces_f") B(qry_server_config, "mcd_qry_server_config_f")
#undef B
    { _snprintf(err, errcap - 1, "%s", path); err[errcap - 1] = 0; }
    return 0;
}
#endif
