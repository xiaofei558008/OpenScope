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
    int  (*exec_cmd)(const char* cmd, char* result);
    /* 连接/控制 */
    int  (*connect)(void);
    int  (*halt)(void);
    int  (*go)(void);
    int  (*reset)(void);
    int  (*set_speed)(int khz);
    /* 内存 */
    int  (*read_mem)(uint32_t addr, uint32_t size, uint8_t* data);
    int  (*write_mem)(uint32_t addr, uint32_t size, const uint8_t* data);
    /* 信息 */
    int  (*get_dll_version)(void);
    int  (*get_hw_version)(void);
    char* (*get_fw_string)(void);
    int  (*get_emu_caps)(void);
    /* 仿真器枚举 */
    uint32_t (*emu_get_num_devices)(void);
    int  (*emu_get_list)(int host, void* infos, int count);
    int  (*emu_select_by_index)(int idx);
    int  (*emu_select_by_usbsn)(uint32_t sn);
} OS_JLinkApi;

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

/* 配置对话框（OS_CMD_CONFIGURE 实现） */
int os_jlink_show_config_dialog(HWND parent);
/* 供对话框使用的内部访问器 */
OS_ConnectCfg* os_jlink_cfg(void);
int  os_jlink_scan_devices(OS_DeviceInfo* items, int cap);
int  os_jlink_connect_now(char* err, int errlen);
int  os_jlink_disconnect_now(void);

#endif
