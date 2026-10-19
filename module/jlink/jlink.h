#ifndef OS_JLINK_MODULE_H
#define OS_JLINK_MODULE_H

#include <windows.h>
#include <stdint.h>
#include "module_api.h"

/* JLINKARM_EMU_GetList host filter (SEGGER JLinkHost enum) */
#define OS_JLINK_HOST_USB        1
#define OS_JLINK_HOST_IP         2
#define OS_JLINK_HOST_USB_OR_IP  3

/* JLink_x64.dll 动态绑定：不在编译期链接 SEGGER SDK（AD-7）。 */

typedef struct OS_JLinkApi {
    HMODULE h;
    /* 基础 */
    int  (*open)(void* pParam);
    void (*close)(void);
    int  (*is_connected)(void);
    int  (*exec_cmd)(const char* cmd, char* result, int result_size);
    /* 连接/控制 */
    int  (*connect)(void);
    int  (*tif_select)(int tif);
    int  (*halt)(void);
    int  (*go)(void);
    int  (*reset)(void);
    int  (*step)(void);          /* 可选：单步 */
    int  (*reset_no_halt)(void); /* 可选：复位后继续运行 */
    int  (*is_halted)(void);      /* 可选：1=halted，0=running */
    uint32_t (*read_reg)(int reg_index); /* 可选：Cortex-M R0..R15(PC=15) */
    /* 可选硬件/软件断点：SetBPEx(addr, type) / ClrBPEx(handle) */
    int  (*set_bp_ex)(uint32_t addr, uint32_t type);
    int  (*clr_bp_ex)(int handle);
    /* GetHWStatus：VTarget(mV) + 复位脚等；结构按 SEGGER 布局前 8 字节够用 */
    int  (*get_hw_status)(void* pStat);
    int  (*set_speed)(int khz);
    /* 内存 */
    int  (*read_mem)(uint32_t addr, uint32_t size, uint8_t* data);
    int  (*write_mem)(uint32_t addr, uint32_t size, const uint8_t* data);
    /* 信息 */
    int  (*get_dll_version)(void);
    int  (*get_hw_version)(void);
    void (*get_fw_string)(char* buf, int bufsize);
    int  (*get_emu_caps)(void);
    /* 仿真器枚举 */
    uint32_t (*emu_get_num_devices)(void);
    int  (*emu_get_list)(int host, void* infos, int count);
    int  (*emu_select_by_index)(int idx);
    int  (*emu_select_by_usbsn)(uint32_t sn);
    /* 烧录（可选导出：JLINK_DownloadFile / JLINK_EraseChip） */
    int  (*download_file)(const char* path, uint32_t addr);
    int  (*erase_chip)(void);
    /* void(*)(action, prog, percent)；可选 */
    void (*set_flash_prog_progress_cb)(void* cb);
} OS_JLinkApi;

/* J-Link HW 状态前缀（与 SEGGER JLINKARM_HW_STATUS 对齐） */
typedef struct OS_JLinkHwStatus {
    uint16_t VTarget; /* 目标供电 mV */
    uint8_t  tck;
    uint8_t  tdi;
    uint8_t  tdo;
    uint8_t  tms;
    uint8_t  tres;    /* 0=RESET 脚拉低（复位中） */
    uint8_t  trst;
} OS_JLinkHwStatus;

/* 枚举信息结构（对齐 SEGGER JLINKARM_EMU_CONNECT_INFO） */
typedef struct OS_JLinkEmuInfo {
    uint32_t SerialNumber;
    uint8_t  Connection;
    uint8_t  sUSBAddr[7];
    char     sIPAddr[16];
    uint32_t Time;
    uint64_t Time_us;
    uint32_t HWVersion;
    uint8_t  abMACAddr[6];
    char     acProduct[32];
    char     acNickname[32];
    char     acFWString[112];
    uint8_t  IsDHCPAssignedIP;
    uint8_t  IsDHCPAssignedIPIsValid;
    uint8_t  NumIPConnections;
    uint8_t  NumIPConnectionsIsValid;
    uint8_t  aPadding[34];
} OS_JLinkEmuInfo;
typedef char os_jlink_emu_info_size_check[(sizeof(OS_JLinkEmuInfo) == 264) ? 1 : -1];

int os_jlink_bind(OS_JLinkApi* api, char* err, int err_len);
void os_jlink_unbind(OS_JLinkApi* api);

#endif
