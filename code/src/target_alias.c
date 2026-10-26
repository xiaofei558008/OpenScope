/* target_alias.c — 见 target_alias.h */
#include "target_alias.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static char lc(char c) { return (c >= 'A' && c <= 'Z') ? (char)(c - 'A' + 'a') : c; }
static int is_al(char c) { c = lc(c); return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_'; }
static int is_dig(char c) { return c >= '0' && c <= '9'; }
static int is_up(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }

int os_alias_sanitize(const char* in, char* out, int cap)
{
    int n = 0;
    if (!out || cap <= 0) return 0;
    if (in) for (; *in && n < cap - 1 && n < OS_ALIAS_MAX - 1; in++)
        if (is_al(*in)) out[n++] = lc(*in);
    out[n] = 0;
    return n;
}

void os_alias_default(const char* drv, char* out, int cap)
{
    char b[OS_ALIAS_MAX];
    if (!out || cap <= 0) return;
    if (os_alias_sanitize(drv, b, (int)sizeof(b) - 1) == 0) memcpy(b, "dev", 4);
    b[OS_ALIAS_MAX - 2] = 0;
    snprintf(out, (size_t)cap, "%s0", b);
}

/* 不区分大小写比较前缀 */
static int ipfx(const char* s, const char* p)
{
    for (; *p; s++, p++) if (lc(*s) != lc(*p)) return 0;
    return 1;
}

int os_alias_from_chip(const char* t, const char* drv, char* out, int cap)
{
    const char* s;
    if (!out || cap <= 0) return 0;
    out[0] = 0;
    for (s = t; s && *s; s++) {
        if (s != t && is_al(s[-1])) continue;            /* 只在词首匹配 */
        /* Infineon AURIX：TC3xx / TC2xx / TC4xx → tc3x */
        if (ipfx(s, "tc") && is_dig(s[2])) { snprintf(out, (size_t)cap, "tc%cx", s[2]); return 1; }
        /* STM32：STM32G474RE → stm32g4 */
        if (ipfx(s, "stm32") && is_up(s[5]) && is_dig(s[6])) {
            snprintf(out, (size_t)cap, "stm32%c%c", lc(s[5]), s[6]); return 1;
        }
        /* GD32 同 STM32 风格 */
        if (ipfx(s, "gd32") && is_up(s[4]) && is_dig(s[5])) {
            snprintf(out, (size_t)cap, "gd32%c%c", lc(s[4]), s[5]); return 1;
        }
        /* NXP S32K：S32K144 → s32k1 */
        if (ipfx(s, "s32k") && is_dig(s[4])) { snprintf(out, (size_t)cap, "s32k%c", s[4]); return 1; }
        /* Nordic：nRF52840 → nrf52 */
        if (ipfx(s, "nrf") && is_dig(s[3]) && is_dig(s[4])) { snprintf(out, (size_t)cap, "nrf%c%c", s[3], s[4]); return 1; }
        /* Renesas RH850 */
        if (ipfx(s, "rh850")) { snprintf(out, (size_t)cap, "rh850"); return 1; }
    }
    os_alias_default(drv, out, cap);
    return 0;
}

const char* os_log_strip_src(const char* msg, const char* mod)
{
    size_t n;
    const char* p;
    static const char kMod[] = "\xE6\xA8\xA1\xE5\x9D\x97";   /* "模块" UTF-8 */
    static const char kColon[] = "\xEF\xBC\x9A";             /* "：" UTF-8 */
    if (!msg || !mod || !*mod) return msg;
    n = strlen(mod);
    if (strncmp(msg, mod, n) != 0) return msg;
    p = msg + n;
    if (*p == ' ') p++;
    if (strncmp(p, kMod, sizeof(kMod) - 1) == 0) p += sizeof(kMod) - 1;
    if (*p == ':') p++;
    else if (strncmp(p, kColon, sizeof(kColon) - 1) == 0) p += sizeof(kColon) - 1;
    else return msg;                                           /* "das xxx" 不是前缀 */
    while (*p == ' ') p++;
    return p;
}

void os_log_format_src(const char* alias, const char* mod, const char* msg, char* out, int cap)
{
    const char* body;
    if (!out || cap <= 0) return;
    if (!msg) msg = "";
    body = os_log_strip_src(msg, mod);
    if (mod && *mod && alias && *alias) snprintf(out, (size_t)cap, "%s %s: %s", alias, mod, body);
    else if (mod && *mod)               snprintf(out, (size_t)cap, "%s: %s", mod, body);
    else                                snprintf(out, (size_t)cap, "%s", body);
    out[cap - 1] = 0;
}
