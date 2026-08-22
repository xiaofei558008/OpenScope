/*
 * OpenScope 网络远程操作模块（network.dll）
 *
 * 能力：OS_CAP_NET（服务型模块，非驱动/非窗口）。
 * 首版（测试交付物）落地传输无关内核：netproto（varint/帧/分块）+ netcodec（时序样本
 * 无损编解码）。WebSocket 传输层（mongoose 或自研 RFC6455）在后续 story 接入。
 */
#include "module_api.h"
#include "netproto.h"
#include "netcodec.h"
#include <stdio.h>
#include <string.h>

static const OS_Framework* g_fw;
static OS_DriverInfo g_info;

static int mod_command(void* ctx, int cmd, void* in, void* out)
{
    (void)ctx; (void)in;
    switch (cmd) {
    case OS_CMD_GET_INFO:
        if (out) memcpy(out, &g_info, sizeof(g_info));
        return OS_ERR_OK;
    case OS_CMD_ELF_RELOADED:
        if (g_fw) g_fw->log(OS_LOG_INFO, "network 模块: ELF 已重载");
        return OS_ERR_OK;
    default:
        return OS_ERR_FAIL;
    }
}

static int mod_init(const OS_Framework* fw, void** out_ctx)
{
    (void)out_ctx;
    g_fw = fw;
    memset(&g_info, 0, sizeof(g_info));
    _snprintf(g_info.name, sizeof(g_info.name), "%s", "network");
    _snprintf(g_info.version, sizeof(g_info.version), "%s", "0.1.0");
    _snprintf(g_info.dll_version, sizeof(g_info.dll_version), "%s", "netcore-1");
    _snprintf(g_info.emulator, sizeof(g_info.emulator), "%s", "未连接");
    if (fw) fw->log(OS_LOG_INFO, "network 模块: 已初始化（协议/编解码/分块内核就绪）");
    return OS_ERR_OK;
}

static void mod_deinit(void* ctx)
{
    (void)ctx;
}

static const OS_Module g_module = {
    OS_API_VERSION,
    OS_CAP_NET,
    "network",
    "0.1.0",
    "网络远程操作模块：WebSocket 传输 + 协议/编解码/分块内核",
    NULL,
    mod_init,
    mod_deinit,
    mod_command,
    NULL, NULL, NULL, NULL, NULL, NULL
};

const OS_Module* os_module_get(void)
{
    return &g_module;
}
