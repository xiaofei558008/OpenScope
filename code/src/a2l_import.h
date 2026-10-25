#ifndef OS_A2L_IMPORT_H
#define OS_A2L_IMPORT_H

#include <windows.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 加载 ASAP2 (.a2l) 精简子集：CHARACTERISTIC / MEASUREMENT + ECU_ADDRESS。
 *  写入当前 Project 的叶表（g_app.leaves，带 project_id / from_a2l）。
 *  返回新增叶数，失败 <0。 */
int os_a2l_import(const wchar_t* path_w, int project_id);

/** 弹出打开对话框并导入；成功返回新增数，取消 0，失败 <0。 */
int os_a2l_import_dialog(HWND owner, int project_id);

#ifdef __cplusplus
}
#endif

#endif
