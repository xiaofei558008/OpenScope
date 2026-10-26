/* 日志来源前缀 / 目标别名单元测试（code/src/target_alias.c；纯函数，无 Windows 依赖）
 * gcc:  ./build.sh      MSVC: build_msvc.bat */
#include <stdio.h>
#include <string.h>
#include "target_alias.h"

static int g_fail, g_n;
static void eq(const char* got, const char* want, const char* what)
{
    g_n++;
    if (strcmp(got, want) != 0) { g_fail++; printf("FAIL %s: got \"%s\" want \"%s\"\n", what, got, want); }
}

int main(void)
{
    char a[OS_ALIAS_MAX], line[256];
    /* 芯片推导 */
    os_alias_from_chip("TC375", "das", a, sizeof(a));                           eq(a, "tc3x", "TC375");
    os_alias_from_chip(" DAS JDS TC375 Lite Kit / TC37x / core0", "das", a, sizeof(a)); eq(a, "tc3x", "das emulator text");
    os_alias_from_chip("tc277", "das", a, sizeof(a));                           eq(a, "tc2x", "tc277");
    os_alias_from_chip("STM32G474RE", "stlink", a, sizeof(a));                  eq(a, "stm32g4", "STM32G474RE");
    os_alias_from_chip("STM32F407VG", "jlink", a, sizeof(a));                   eq(a, "stm32f4", "STM32F407VG");
    os_alias_from_chip("GD32F303CC", "jlink", a, sizeof(a));                    eq(a, "gd32f3", "GD32F303");
    os_alias_from_chip("nRF52840_xxAA", "jlink", a, sizeof(a));                 eq(a, "nrf52", "nRF52840");
    os_alias_from_chip("S32K144", "jlink", a, sizeof(a));                       eq(a, "s32k1", "S32K144");
    os_alias_from_chip("Cortex-M4", "jlink", a, sizeof(a));                     eq(a, "jlink0", "generic core -> driver");
    os_alias_from_chip("", "stlink", a, sizeof(a));                             eq(a, "stlink0", "empty -> driver");
    os_alias_from_chip(NULL, NULL, a, sizeof(a));                               eq(a, "dev0", "null");
    os_alias_from_chip("ATC375", "das", a, sizeof(a));                          eq(a, "das0", "word start only");
    /* 规范化 */
    os_alias_sanitize("My Board-1!", a, sizeof(a));                             eq(a, "myboard1", "sanitize");
    os_alias_default("JLink", a, sizeof(a));                                    eq(a, "jlink0", "default lower");
    /* 去历史前缀 + 组装 */
    os_log_format_src("tc3x", "das", "das 模块: 已连接 X", line, sizeof(line)); eq(line, "tc3x das: 已连接 X", "strip 'das 模块:'");
    os_log_format_src("tc3x", "das", "das 模块：已重连", line, sizeof(line));   eq(line, "tc3x das: 已重连", "strip full-width colon");
    os_log_format_src("jlink0", "jlink", "jlink: J-Link 扫描: 发现 1 个设备", line, sizeof(line)); eq(line, "jlink0 jlink: J-Link 扫描: 发现 1 个设备", "strip 'jlink:'");
    os_log_format_src("stlink0", "stlink", "stlink 模块: LoadersPath=C:\\x", line, sizeof(line)); eq(line, "stlink0 stlink: LoadersPath=C:\\x", "stlink");
    os_log_format_src("tc3x", "das", "已绑定 mcdxdas.dll", line, sizeof(line)); eq(line, "tc3x das: 已绑定 mcdxdas.dll", "no legacy prefix");
    os_log_format_src("tc3x", "das", "dasXYZ: keep", line, sizeof(line));       eq(line, "tc3x das: dasXYZ: keep", "not a prefix (dasXYZ)");
    os_log_format_src("tc3x", "das", "das server busy", line, sizeof(line));    eq(line, "tc3x das: das server busy", "not a prefix (das + text)");
    os_log_format_src("", "aiapi", "aiapi: 监听 127.0.0.1", line, sizeof(line)); eq(line, "aiapi: 监听 127.0.0.1", "non-target module");
    os_log_format_src("", "network", "已初始化", line, sizeof(line));          eq(line, "network: 已初始化", "non-target add module");
    os_log_format_src("x", "", "msg", line, sizeof(line));                      eq(line, "msg", "no module");
    os_log_format_src("tc3x", "das", "das 模块: 很长很长", line, 12);           g_n++; if (strlen(line) > 11) { g_fail++; printf("FAIL truncation\n"); }
    printf("%d checks, %d failed\nRESULT: %s\n", g_n, g_fail, g_fail ? "FAIL" : "PASS");
    return g_fail ? 1 : 0;
}
