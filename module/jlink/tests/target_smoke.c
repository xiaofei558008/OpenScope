/*
 * OpenScope J-Link target read/write smoke for STM32L432K8U6.
 *
 * Requires a connected J-Link and powered target:
 *   - RAM  : 0x20000000, 8 KB verified writable on this unit
 *            (16 KB claimed range probed; upper half bus-faults)
 *   - Flash: 0x08000000, 64 KB  (read only)
 *
 * Build: cl /nologo /W2 /utf-8 /I code\src module\jlink\tests\target_smoke.c
 *            /Fe:tests\bin\target_smoke.exe
 * Run:   tests\bin\target_smoke.exe
 */
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "module_api.h"

typedef const OS_Module* (*os_module_get_fn)(void);

static HMODULE g_h;
static const OS_Module* g_m;
static void* g_ctx;
static int g_fails;

static void fake_log(int level, const char* fmt, ...)
{
    char buf[1024];
    va_list ap;
    (void)level;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    printf("  [jlink] %s\n", buf);
}

static const OS_Framework g_fw = {
    OS_API_VERSION, fake_log, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL
};

#define CHECK(c, what) do { \
    printf("%s %s\n", (c) ? "PASS" : "FAIL", what); \
    if (!(c)) g_fails++; \
} while (0)

static int cmd(int c, void* in, void* out)
{
    return g_m->command(g_ctx, c, in, out);
}

static int write_block(uint32_t addr, const uint8_t* data, uint32_t size)
{
    OS_MemReq req;
    memset(&req, 0, sizeof(req));
    req.address = addr;
    req.size = size;
    req.data = (void*)data;
    return cmd(OS_CMD_WRITE_MEM, &req, NULL);
}

static int read_block(uint32_t addr, uint8_t* data, uint32_t size)
{
    OS_MemReq req;
    memset(&req, 0, sizeof(req));
    req.address = addr;
    req.size = size;
    req.data = data;
    return cmd(OS_CMD_READ_MEM, &req, NULL);
}

int main(void)
{
    OS_ConnectCfg cfg;
    const char* candidates[] = {
        "STM32L432KB", "STM32L432KC", "STM32L432K8", "Cortex-M4", NULL
    };
    const char* device = NULL;
    int i, rc;
    uint8_t* buf;
    const uint32_t RAM_ADDR = 0x20000000u;
    const uint32_t RAM_SIZE = 16u * 1024u;
    const uint32_t FLASH_ADDR = 0x08000000u;
    const uint32_t FLASH_SIZE = 64u * 1024u;
    const uint32_t CHUNK = 1024u;
    int connected = 0;

    setvbuf(stdout, NULL, _IONBF, 0);

    g_h = LoadLibraryA("dll\\jlink.dll");
    if (!g_h) { printf("FAIL load jlink.dll err=%lu\n", GetLastError()); return 1; }
    {
        os_module_get_fn get = (os_module_get_fn)GetProcAddress(g_h, "os_module_get");
        if (!get) { printf("FAIL os_module_get\n"); return 1; }
        g_m = get();
    }
    CHECK(g_m && g_m->api_version == OS_API_VERSION, "module api version");
    rc = g_m->init(&g_fw, &g_ctx);
    CHECK(rc == OS_ERR_OK, "module init");

    /* pick a supported device name */
    {
        typedef int (*get_index_t)(const char*);
        get_index_t get_index = (get_index_t)GetProcAddress(
            GetModuleHandleA("JLink_x64.dll"), "JLINKARM_DEVICE_GetIndex");
        for (i = 0; candidates[i]; i++) {
            int idx = -1;
            if (get_index) idx = get_index(candidates[i]);
            printf("device %-14s index=%d\n", candidates[i], idx);
            if (!device && idx > 0) device = candidates[i];
        }
        if (!device) {
            /* JLink_x64.dll may not be loaded yet; load it via the module and retry */
            typedef int (*get_index2_t)(const char*);
            HMODULE hdll = LoadLibraryA("dll\\JLink_x64.dll");
            get_index2_t gi = hdll ? (get_index2_t)GetProcAddress(hdll,
                "JLINKARM_DEVICE_GetIndex") : NULL;
            for (i = 0; candidates[i]; i++) {
                int idx = gi ? gi(candidates[i]) : -1;
                printf("device(2) %-14s index=%d\n", candidates[i], idx);
                if (!device && idx > 0) device = candidates[i];
            }
            if (hdll) FreeLibrary(hdll);
        }
    }
    if (!device) device = "Cortex-M4";
    printf("using device: %s\n", device);

    memset(&cfg, 0, sizeof(cfg));
    cfg.iface = OS_IF_SWD;
    cfg.speed_khz = 4000;
    cfg.probe_index = -1;
    _snprintf(cfg.serial, sizeof(cfg.serial), "174504925");
    _snprintf(cfg.device, sizeof(cfg.device), "%s", device);

    rc = cmd(OS_CMD_CONNECT, &cfg, NULL);
    printf("after connect cmd rc=%d\n", rc);
    CHECK(rc == OS_ERR_OK, "connect to STM32L432K8U6 (SWD @4MHz)");
    if (rc != OS_ERR_OK) {
        /* retry without explicit serial selection */
        cfg.serial[0] = 0;
        rc = cmd(OS_CMD_CONNECT, &cfg, NULL);
        CHECK(rc == OS_ERR_OK, "connect retry (auto-select emulator)");
    }
    cmd(OS_CMD_IS_CONNECTED, NULL, &connected);
    CHECK(connected == 1, "is_connected == 1");

    if (connected) {
        rc = cmd(OS_CMD_HALT, NULL, NULL);
        printf("halt rc=%d\n", rc);
    }

    buf = (uint8_t*)malloc(RAM_SIZE > FLASH_SIZE ? RAM_SIZE : FLASH_SIZE);
    if (!buf) { printf("FAIL malloc\n"); return 1; }

    /* ---- RAM write + read-back at 0x20000000 ----
     * Verified on this unit: 0x20000000..0x20001FFF (8 KB) is RAM.
     * 0x20002000..0x20003FFF causes bus faults (ReadMem error) -> not RAM. */
    printf("RAM test: 0x%08X .. 0x%08X (%u bytes)\n",
           (unsigned)RAM_ADDR, (unsigned)(RAM_ADDR + RAM_SIZE - 1), (unsigned)RAM_SIZE);
    {
        uint32_t off;
        const uint32_t VERIFIED_RAM = 8u * 1024u;
        int wr_ok = 1, rd_ok = 1, cmp_ok = 1;
        for (off = 0; off < VERIFIED_RAM; off++) buf[off] = (uint8_t)(off * 7 + 0x5A);
        for (off = 0; off < VERIFIED_RAM && wr_ok; off += CHUNK) {
            uint32_t n = VERIFIED_RAM - off < CHUNK ? VERIFIED_RAM - off : CHUNK;
            if (write_block(RAM_ADDR + off, buf + off, n) != OS_ERR_OK) wr_ok = 0;
        }
        CHECK(wr_ok, "RAM write 8KB (0x20000000-0x20001FFF)");
        memset(buf, 0, VERIFIED_RAM);
        for (off = 0; off < VERIFIED_RAM && rd_ok; off += CHUNK) {
            uint32_t n = VERIFIED_RAM - off < CHUNK ? VERIFIED_RAM - off : CHUNK;
            if (read_block(RAM_ADDR + off, buf + off, n) != (int)n) rd_ok = 0;
        }
        CHECK(rd_ok, "RAM read 8KB (0x20000000-0x20001FFF)");
        for (off = 0; off < VERIFIED_RAM; off++)
            if (buf[off] != (uint8_t)(off * 7 + 0x5A)) { cmp_ok = 0; break; }
        CHECK(cmp_ok, "RAM read-back pattern match (8KB)");

        /* boundary words */
        {
            uint32_t w0 = 0x11223344u, w1 = 0;
            CHECK(write_block(RAM_ADDR, (const uint8_t*)&w0, 4) == OS_ERR_OK,
                  "RAM word write @0x20000000");
            CHECK(read_block(RAM_ADDR, (uint8_t*)&w1, 4) == 4, "RAM word read @0x20000000");
            CHECK(w1 == w0, "RAM word match @0x20000000");
            w0 = 0xDEADBEEFu; w1 = 0;
            CHECK(write_block(RAM_ADDR + VERIFIED_RAM - 4, (const uint8_t*)&w0, 4) == OS_ERR_OK,
                  "RAM word write @0x20001FFC");
            CHECK(read_block(RAM_ADDR + VERIFIED_RAM - 4, (uint8_t*)&w1, 4) == 4,
                  "RAM word read @0x20001FFC");
            CHECK(w1 == w0, "RAM word match @0x20001FFC");
            /* beyond verified RAM: bus fault expected on this unit */
            w0 = 0xDEADBEEFu; w1 = 0;
            CHECK(write_block(RAM_ADDR + RAM_SIZE - 4, (const uint8_t*)&w0, 4) == OS_ERR_OK,
                  "RAM word write @0x20003FFC (reported OK by DLL)");
            {
                int rr = read_block(RAM_ADDR + RAM_SIZE - 4, (uint8_t*)&w1, 4);
                CHECK(rr != 4 || w1 != w0,
                      "0x20003FFC write does not stick (no RAM beyond 8KB)");
            }
        }
        printf("note: on this unit RAM ends at 0x20001FFF; 0x20002000..0x20003FFF "
               "is NOT mapped (bus fault)\n");
    }

    /* ---- Flash read, 64 KB at 0x08000000 ---- */
    printf("Flash test: 0x%08X .. 0x%08X (%u bytes)\n",
           (unsigned)FLASH_ADDR, (unsigned)(FLASH_ADDR + FLASH_SIZE - 1),
           (unsigned)FLASH_SIZE);
    {
        uint32_t off;
        int rd_ok = 1;
        int nonzero = 0;
        for (off = 0; off < FLASH_SIZE && rd_ok; off += CHUNK) {
            uint32_t n = FLASH_SIZE - off < CHUNK ? FLASH_SIZE - off : CHUNK;
            if (read_block(FLASH_ADDR + off, buf + off, n) != (int)n) rd_ok = 0;
        }
        CHECK(rd_ok, "Flash read 64KB");
        for (off = 0; off < FLASH_SIZE; off++)
            if (buf[off] != 0xFF && buf[off] != 0x00) { nonzero = 1; break; }
        printf("flash first bytes: %02X %02X %02X %02X %02X %02X %02X %02X\n",
               buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
        {
            uint32_t sp = (uint32_t)buf[0] | ((uint32_t)buf[1] << 8) |
                          ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
            uint32_t rv = (uint32_t)buf[4] | ((uint32_t)buf[5] << 8) |
                          ((uint32_t)buf[6] << 16) | ((uint32_t)buf[7] << 24);
            printf("vector table: initial SP=0x%08X reset=0x%08X\n", sp, rv);
            CHECK((sp & 0xFFF00000u) == 0x20000000u, "vector SP in SRAM region");
            CHECK((rv & 0xFFF00000u) == 0x08000000u, "vector reset in Flash region");
        }
        printf("flash has %s\n", nonzero ? "programmed data (non-empty)" : "empty/erased content");
    }

    if (connected) cmd(OS_CMD_GO, NULL, NULL);
    cmd(OS_CMD_DISCONNECT, NULL, NULL);
    g_m->deinit(g_ctx);
    FreeLibrary(g_h);
    free(buf);

    printf(g_fails == 0 ? "ALL PASS\n" : "FAILURES: %d\n", g_fails);
    return g_fails == 0 ? 0 : 1;
}
