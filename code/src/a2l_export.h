#ifndef OS_A2L_EXPORT_H
#define OS_A2L_EXPORT_H

#include <windows.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 把当前 ELF 叶变量表导出为 ASAP2 (.a2l)，供 CANape / INCA 使用。
 *  path_w：输出路径；module_name：MODULE 名（可空=ECU）。
 *  calibration_mode：1=RAM 中的标量导出为 CHARACTERISTIC（可写标定），
 *                    0=全部导出为 MEASUREMENT（只读测量）。
 *  返回 0 成功。 */
int os_a2l_export(const wchar_t* path_w, const char* module_name, int calibration_mode);

/** 弹出另存为对话框并导出；成功返回 0。calibration_mode 同上。 */
int os_a2l_export_dialog(HWND owner, int calibration_mode);

#ifdef __cplusplus
}
#endif

#endif
