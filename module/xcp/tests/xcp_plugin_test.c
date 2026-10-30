/* OS_CAP_XCP 插件（xcp.c）回归：通过 os_module_get()->command 驱动，对假从机（TCP）。
 * 同一份测试分别链接“迁移前”（原 xcp.c + 原 xcp_master.c）和“迁移后”代码，
 * 输出 STEP 行由 tests/compare_plugin.sh 逐行比对。XCP_NEW 时追加只属于新实现的步骤。 */
#include "../xcp_plat.h"
#include "../../module_api.h"
#include "xcp_fake_slave.h"
#include "../xcp_proto.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

const OS_Module* os_module_get(void);
static int g_fail;
static void flog(int level, const char* fmt, ...) { (void)level; (void)fmt; }
#define STEP(name, rc, ok) do { int ok_ = (ok); printf("STEP %-28s rc=%-4d %s\n", name, (int)(rc), ok_ ? "ok" : "BAD"); if (!ok_) g_fail++; } while (0)

static int cmd(const OS_Module* m, int c, void* in, void* out) { return m->command(NULL, c, in, out); }
static int is_conn(const OS_Module* m) { int v = -1; cmd(m, OS_CMD_IS_CONNECTED, NULL, &v); return v; }

int main(void)
{
    const OS_Module* m = os_module_get();
    OS_Framework fw;
    FakeSlave* f;
    FakeCfg c;
    int port, rc, i;
    OS_ConnectCfg cc;
    OS_MemReq mr;
    OS_DriverInfo di;
    OS_DeviceInfo items[8];
    OS_ScanReq sr;
    static char desc[4096];
    uint8_t buf[2000], ref[2000];

    setvbuf(stdout, NULL, _IONBF, 0);
    memset(&fw, 0, sizeof(fw));
    fw.api_version = OS_API_VERSION;
    fw.log = flog;
    memset(&c, 0, sizeof(c));
    c.ag = 1; c.max_cto = 8; c.max_dto = 8; c.resource = XCP_RES_CAL_PAG | XCP_RES_DAQ;
    f = fake_start(&c, &port);

    STEP("module caps", 0, m->capabilities == OS_CAP_XCP && m->api_version == OS_API_VERSION && strcmp(m->name, "xcp") == 0);
    STEP("init", m->init(&fw, NULL), 1);
    rc = cmd(m, OS_CMD_AI_DESCRIBE, NULL, desc);
    STEP("ai_describe", rc, rc == 0 && strstr(desc, "xcp.read") != NULL);
    STEP("is_connected before", is_conn(m), is_conn(m) == 0);
    mr.address = 0x10; mr.size = 4; mr.data = buf;
    rc = cmd(m, OS_CMD_READ_MEM, &mr, NULL);
    STEP("read before connect", rc, rc == OS_ERR_NOT_CONNECTED);
    memset(&sr, 0, sizeof(sr)); sr.items = items; sr.capacity = 8;
    rc = cmd(m, OS_CMD_SCAN, &sr, NULL);
    STEP("scan", rc, rc == 0 && sr.count == 1 && strcmp(items[0].name, "tcp:127.0.0.1:5555") == 0);

    memset(&cc, 0, sizeof(cc));
    cc.probe_index = -1;
    snprintf(cc.device, sizeof(cc.device), "tcp");
    snprintf(cc.serial, sizeof(cc.serial), "127.0.0.1:%d", port);
    rc = cmd(m, OS_CMD_CONNECT, &cc, NULL);
    STEP("connect tcp", rc, rc == 0);
    memset(&di, 0, sizeof(di));
    cmd(m, OS_CMD_GET_INFO, NULL, &di);
    { char want[128]; snprintf(want, sizeof(want), "XCP/tcp 127.0.0.1:%d", port);
      STEP("get_info", di.connected, di.connected == 1 && strcmp(di.emulator, want) == 0 && strcmp(di.name, "xcp") == 0); }
    STEP("is_connected", is_conn(m), is_conn(m) == 1);
    rc = cmd(m, OS_CMD_CONNECT, &cc, NULL);
    STEP("connect again (no-op)", rc, rc == 0);

    mr.address = 0x10; mr.size = 4; mr.data = buf;
    rc = cmd(m, OS_CMD_READ_MEM, &mr, NULL);
    fake_get_mem(f, 0x10, ref, 4);
    STEP("read 4", rc, rc == 4 && memcmp(buf, ref, 4) == 0);
    mr.address = 0x100; mr.size = 300;
    rc = cmd(m, OS_CMD_READ_MEM, &mr, NULL);
    fake_get_mem(f, 0x100, ref, 300);
    STEP("read 300", rc, rc == 300 && memcmp(buf, ref, 300) == 0);
    for (i = 0; i < 1000; i++) buf[i] = (uint8_t)(i * 3 + 11);
    mr.address = 0x400; mr.size = 1000;
    rc = cmd(m, OS_CMD_WRITE_MEM, &mr, NULL);
    fake_get_mem(f, 0x400, ref, 1000);
    STEP("write 1000", rc, rc == 0 && memcmp(buf, ref, 1000) == 0);
    mr.size = 0;
    STEP("read size 0", cmd(m, OS_CMD_READ_MEM, &mr, NULL), cmd(m, OS_CMD_READ_MEM, &mr, NULL) == OS_ERR_INVALID_ARG);
    mr.size = 4097;
    STEP("read size 4097", cmd(m, OS_CMD_READ_MEM, &mr, NULL), cmd(m, OS_CMD_READ_MEM, &mr, NULL) == OS_ERR_INVALID_ARG);
    STEP("write size 4097", cmd(m, OS_CMD_WRITE_MEM, &mr, NULL), cmd(m, OS_CMD_WRITE_MEM, &mr, NULL) == OS_ERR_INVALID_ARG);
    mr.address = 0x10000; mr.size = 4;
    rc = cmd(m, OS_CMD_READ_MEM, &mr, NULL);
    STEP("read slave ERR (keeps link)", rc, rc == OS_ERR_TIMEOUT && is_conn(m) == 1);
    rc = cmd(m, OS_CMD_WRITE_MEM, &mr, NULL);
    STEP("write slave ERR (keeps link)", rc, rc == OS_ERR_TIMEOUT && is_conn(m) == 1);
    STEP("halt", cmd(m, OS_CMD_HALT, NULL, NULL), cmd(m, OS_CMD_HALT, NULL, NULL) == 0);
    STEP("unknown cmd", cmd(m, 999, NULL, NULL), cmd(m, 999, NULL, NULL) == OS_ERR_FAIL);

    /* 从机不应答 -> 超时 -> 丢链路 */
    fake_drop(f, XCP_CMD_SHORT_UPLOAD, 1000);
    mr.address = 0x10; mr.size = 4;
    { uint64_t t0 = xcp_now_ms(); rc = cmd(m, OS_CMD_READ_MEM, &mr, NULL);
      printf("INFO timeout read took %lu ms\n", (unsigned long)(xcp_now_ms() - t0)); }
    STEP("read timeout drops link", rc, rc == OS_ERR_TIMEOUT && is_conn(m) == 0);
    fake_drop(f, XCP_CMD_SHORT_UPLOAD, 0);
    xcp_sleep_ms(200);
    rc = cmd(m, OS_CMD_CONNECT, &cc, NULL);
    STEP("reconnect", rc, rc == 0 && is_conn(m) == 1);
    mr.address = 0x20; mr.size = 8;
    rc = cmd(m, OS_CMD_READ_MEM, &mr, NULL);
    STEP("read after reconnect", rc, rc == 8);

    /* 从机断开 TCP -> 下一次读返回 TIMEOUT 并丢链路 */
    fake_close_client(f);
    xcp_sleep_ms(300);
    rc = cmd(m, OS_CMD_READ_MEM, &mr, NULL);
    /* 旧实现：等满超时返回 TIMEOUT；新实现：RX 线程已发现断链，立即 NOT_CONNECTED（有意的行为差异） */
    printf("INFO read after peer close rc=%d\n", rc);
    STEP("read after peer close", 0, (rc == OS_ERR_TIMEOUT || rc == OS_ERR_NOT_CONNECTED) && is_conn(m) == 0);
    xcp_sleep_ms(200);
    rc = cmd(m, OS_CMD_CONNECT, &cc, NULL);
    STEP("reconnect 2", rc, rc == 0);
    rc = cmd(m, OS_CMD_DISCONNECT, NULL, NULL);
    STEP("disconnect", rc, rc == 0 && is_conn(m) == 0);
    rc = cmd(m, OS_CMD_HALT, NULL, NULL);
    STEP("halt disconnected", rc, rc == OS_ERR_NOT_CONNECTED);
    snprintf(cc.serial, sizeof(cc.serial), "127.0.0.1:1");
    rc = cmd(m, OS_CMD_CONNECT, &cc, NULL);
    STEP("connect refused", rc, rc == OS_ERR_NO_DEVICE);

#ifdef XCP_NEW
    {
        FakeSlave* fu; FakeCfg cu = c; int pu;
        cu.udp = 1; cu.max_cto = 255; cu.max_dto = 1400; cu.block = 1;
        fu = fake_start(&cu, &pu);
        snprintf(cc.device, sizeof(cc.device), "udp");
        snprintf(cc.serial, sizeof(cc.serial), "127.0.0.1:%d", pu);
        rc = cmd(m, OS_CMD_CONNECT, &cc, NULL);
        STEP("NEW connect udp", rc, rc == 0);
        cmd(m, OS_CMD_GET_INFO, NULL, &di);
        STEP("NEW get_info udp", 0, strncmp(di.emulator, "XCP/udp ", 8) == 0);
        mr.address = 0x100; mr.size = 2000; mr.data = buf;
        rc = cmd(m, OS_CMD_READ_MEM, &mr, NULL);
        fake_get_mem(fu, 0x100, ref, 2000);
        STEP("NEW read 2000 (max_cto 255)", rc, rc == 2000 && memcmp(buf, ref, 2000) == 0);
        for (i = 0; i < 2000; i++) buf[i] = (uint8_t)(i ^ 0x3C);
        rc = cmd(m, OS_CMD_WRITE_MEM, &mr, NULL);
        fake_get_mem(fu, 0x100, ref, 2000);
        STEP("NEW write 2000 block", rc, rc == 0 && memcmp(buf, ref, 2000) == 0 && fake_max_block_frames(fu) > 1);
        STEP("NEW short_upload frames", fake_count(fu, XCP_CMD_SHORT_UPLOAD), fake_count(fu, XCP_CMD_SHORT_UPLOAD) == 8); /* 2000/254 */
        /* 链路断：RX 线程检测，IS_CONNECTED 立即反映（旧实现要等下一次读写失败） */
        fake_stop(fu);
        rc = cmd(m, OS_CMD_DISCONNECT, NULL, NULL);
        STEP("NEW disconnect udp (slave gone)", rc, rc == 0 && is_conn(m) == 0);
        snprintf(cc.device, sizeof(cc.device), "tcp");
        snprintf(cc.serial, sizeof(cc.serial), "127.0.0.1:%d", port);
        rc = cmd(m, OS_CMD_CONNECT, &cc, NULL);
        fake_close_client(f);
        { uint64_t t0 = xcp_now_ms(); while (is_conn(m) == 1 && xcp_now_ms() - t0 < 2000) xcp_sleep_ms(10);
          STEP("NEW is_connected reflects peer close", rc, rc == 0 && is_conn(m) == 0); }
    }
#endif
    m->deinit(NULL);
    STEP("deinit", 0, 1);
    rc = 0;
    STEP("command after deinit", cmd(m, OS_CMD_IS_CONNECTED, NULL, &rc), rc == 0);
    fake_stop(f);
    printf("%s plugin: %d bad\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
