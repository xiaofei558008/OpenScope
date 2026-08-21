#ifndef OS_STLINK_MODULE_H
#define OS_STLINK_MODULE_H

#include <windows.h>
#include <stdint.h>
#include "module_api.h"

/* ST-Link 驱动模块：动态绑定 STM32CubeProgrammer 的 CubeProgrammer_API.dll。
 * 结构体布局与 ST 官方头 CubeProgrammer_API.h 严格一致（x64/MSVC 默认对齐）。
 * 所有导出为 extern "C"（无名字修饰），经 GetProcAddress 绑定，不链接 .lib。 */

/* debugPort */
#define OS_STLINK_PORT_JTAG 0
#define OS_STLINK_PORT_SWD  1

/* debugConnectMode */
#define OS_STLINK_MODE_NORMAL      0
#define OS_STLINK_MODE_HOTPLUG     1 /* 非侵入：不 halt/不复位目标（变量采集默认） */
#define OS_STLINK_MODE_UNDER_RESET 2
#define OS_STLINK_MODE_POWER_DOWN  3
#define OS_STLINK_MODE_PRE_RESET   4

/* debugResetMode */
#define OS_STLINK_RST_SOFTWARE 0
#define OS_STLINK_RST_HARDWARE 1
#define OS_STLINK_RST_CORE     2

/* frequencies（debugConnectParameters.freq） */
typedef struct OS_StLinkFreq {
    unsigned int jtagFreq[12];
    unsigned int jtagFreqNumber;
    unsigned int swdFreq[12];
    unsigned int swdFreqNumber;
} OS_StLinkFreq;
typedef char os_stlink_freq_size_check[(sizeof(OS_StLinkFreq) == 104) ? 1 : -1];

/* debugConnectParameters（connectStLink 按值传参，布局必须与 DLL 完全一致） */
/* 官方示例必调 setLoadersPath，否则 connectStLink 报 "Unable to list supported devices"。
 * 该路径需含 FlashLoader/ 与 ExternalLoader/ 子目录。 */
typedef struct OS_StLinkDcp {
    int  dbgPort;            /* debugPort */
    int  index;
    char serialNumber[33];
    char firmwareVersion[20];
    char targetVoltage[5];
    int  accessPortNumber;
    int  accessPort;
    int  connectionMode;     /* debugConnectMode */
    int  resetMode;          /* debugResetMode */
    int  isOldFirmware;
    OS_StLinkFreq freq;
    int  frequency;          /* kHz，从 freq.swdFreq[]/jtagFreq[] 档位中选择 */
    int  isBridge;
    int  shared;
    char board[100];
    int  DBG_Sleep;
} OS_StLinkDcp;
typedef char os_stlink_dcp_size_check[(sizeof(OS_StLinkDcp) == 308) ? 1 : -1];

/* generalInf（getDeviceGeneralInf 返回，逐字段读取） */
typedef struct OS_StLinkGenInf {
    unsigned short deviceId;
    int  flashSize;
    int  bootloaderVersion;
    char type[4];
    char cpu[20];
    char name[100];
    char series[100];
    char description[150];
    char revisionId[100];
    char board[100];
} OS_StLinkGenInf;

/* displayCallBacks（setDisplayCallbacks 按值传参） */
typedef struct OS_StLinkCallbacks {
    void (*initProgressBar)(void);
    void (*logMessage)(int msgType, const wchar_t* str);
    void (*loadBar)(int x, int n);
} OS_StLinkCallbacks;

/* CubeProgrammer_API.dll 函数指针表 */
typedef struct OS_StLinkApi {
    HMODULE h;
    void (*set_display_callbacks)(OS_StLinkCallbacks c);
    void (*set_verbosity_level)(int level);
    void (*set_loaders_path)(const char* path); /* 指向含 FlashLoader/ExternalLoader 的目录（如 api\lib） */
    int  (*get_stlink_list)(OS_StLinkDcp** list, int shared);
    void (*delete_interface_list)(void);
    int  (*connect_stlink)(OS_StLinkDcp params);
    int  (*check_device_connection)(void);
    void (*disconnect)(void);
    int  (*read_memory)(unsigned int addr, unsigned char** data, unsigned int size);
    int  (*write_memory)(unsigned int addr, char* data, unsigned int size);
    int  (*reset)(int rst_mode);
    int  (*execute)(unsigned int addr);
    OS_StLinkGenInf* (*get_device_general_inf)(void);
} OS_StLinkApi;

int  os_stlink_bind(OS_StLinkApi* api, char* err, int err_len);
void os_stlink_unbind(OS_StLinkApi* api);

#endif
