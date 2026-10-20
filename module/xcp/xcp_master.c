#include "xcp_master.h"

#include <string.h>

void xcp_master_init(XcpMaster* m, const XcpIo* io)
{
    memset(m, 0, sizeof(*m));
    if (io) m->io = *io;
    m->max_cto = 8;
    m->max_dto = 8;
}

static int xfer(XcpMaster* m, const uint8_t* cmd, int n, uint8_t* rsp, int cap)
{
    int rn;
    m->last_err = 0;
    if (!m->io.send || !m->io.recv) return -1;
    if (m->io.send(m->io.ctx, cmd, n) != n) return -1;
    rn = m->io.recv(m->io.ctx, rsp, cap, 1000);
    if (rn < 1) return -1;
    if (rsp[0] == XCP_PID_ERR) {
        m->last_err = rn >= 2 ? rsp[1] : XCP_ERR_GENERIC;
        return -2;
    }
    if (rsp[0] != XCP_PID_RES) return -1;
    return 0;
}

static int xfer_ok(XcpMaster* m, const uint8_t* cmd, int n)
{
    uint8_t rsp[XCP_MAX_CTO];
    return xfer(m, cmd, n, rsp, (int)sizeof(rsp));
}

int xcp_master_connect(XcpMaster* m)
{
    uint8_t cmd[2] = { XCP_CMD_CONNECT, 0x00 };
    uint8_t rsp[XCP_MAX_CTO];
    int rc = xfer(m, cmd, 2, rsp, (int)sizeof(rsp));
    if (rc != 0) return rc;
    if (rsp[0] != XCP_PID_RES) return -1;
    m->resource = rsp[1];
    m->max_cto = rsp[3] >= 8 ? rsp[3] : 8;
    m->max_dto = xcp_rd16(rsp + 4);
    if (m->max_dto < 8) m->max_dto = 8;
    /* 主机收发缓冲区上限：从机可能声明 255，但本实现每帧缓冲为 XCP_MAX_CTO/DTO。
     * 超限请求会让响应装不下并撕裂串口流（表现为大块 SHORT_UPLOAD 失败）。 */
    if (m->max_cto > XCP_MAX_CTO) m->max_cto = XCP_MAX_CTO;
    if (m->max_dto > XCP_MAX_DTO) m->max_dto = XCP_MAX_DTO;
    m->connected = 1;
    return 0;
}

int xcp_master_disconnect(XcpMaster* m)
{
    uint8_t cmd = XCP_CMD_DISCONNECT;
    int rc = xfer_ok(m, &cmd, 1);
    m->connected = 0;
    return rc;
}

int xcp_master_upload(XcpMaster* m, uint32_t addr, void* dst, int n)
{
    uint8_t* out = (uint8_t*)dst;
    int got = 0;
    if (!m->connected || n < 0 || (n > 0 && !dst)) return -1;
    while (got < n) {
        int chunk = n - got;
        uint8_t cmd[8];
        uint8_t rsp[XCP_MAX_CTO];
        int rc;
        if (chunk > m->max_cto - 1) chunk = m->max_cto - 1;
        if (chunk > m->max_dto - 1) chunk = m->max_dto - 1;
        cmd[0] = XCP_CMD_SHORT_UPLOAD;
        cmd[1] = (uint8_t)chunk;
        cmd[2] = 0;
        cmd[3] = 0;
        xcp_wr32(cmd + 4, addr + (uint32_t)got);
        rc = xfer(m, cmd, 8, rsp, (int)sizeof(rsp));
        if (rc != 0) return rc;
        if (rsp[0] != XCP_PID_RES) return -1;
        memcpy(out + got, rsp + 1, (size_t)chunk);
        got += chunk;
    }
    return 0;
}

static int download_piece(XcpMaster* m, uint32_t addr, const uint8_t* in, int n);

int xcp_master_download(XcpMaster* m, uint32_t addr, const void* src, int n)
{
    const uint8_t* in = (const uint8_t*)src;
    int off = 0;
    if (!m->connected || n <= 0 || !src) return -1;
    while (off < n) {
        int piece = n - off;
        int rc;
        /* 每段单独 SET_MTA + DOWNLOAD，长度不超过单帧 CTO 容量。
         * 不用 DOWNLOAD_NEXT 块模式：部分精简从机（MCU 示例）只实现 DOWNLOAD。 */
        if (piece > m->max_cto - 2) piece = m->max_cto - 2;
        if (piece > 255) piece = 255;
        if (piece <= 0) return -1;
        rc = download_piece(m, addr + (uint32_t)off, in + off, piece);
        if (rc != 0) return rc;
        off += piece;
    }
    return 0;
}

static int download_piece(XcpMaster* m, uint32_t addr, const uint8_t* in, int n)
{
    uint8_t mta[8];
    int fit, off, rc;
    mta[0] = XCP_CMD_SET_MTA;
    mta[1] = 0;
    mta[2] = 0;
    mta[3] = 0;
    xcp_wr32(mta + 4, addr);
    rc = xfer_ok(m, mta, 8);
    if (rc != 0) return rc;
    fit = m->max_cto - 2;
    if (fit < 1) return -1;
    if (n <= fit) {
        uint8_t cmd[XCP_MAX_CTO];
        cmd[0] = XCP_CMD_DOWNLOAD;
        cmd[1] = (uint8_t)n;
        memcpy(cmd + 2, in, (size_t)n);
        return xfer_ok(m, cmd, 2 + n);
    }
    {
        uint8_t cmd[XCP_MAX_CTO];
        cmd[0] = XCP_CMD_DOWNLOAD;
        cmd[1] = (uint8_t)n;
        memcpy(cmd + 2, in, (size_t)fit);
        if (m->io.send(m->io.ctx, cmd, 2 + fit) != 2 + fit) return -1;
        off = fit;
        while (off < n) {
            int chunk = n - off;
            int remain = n - off;
            if (chunk > fit) chunk = fit;
            cmd[0] = XCP_CMD_DOWNLOAD_NEXT;
            cmd[1] = (uint8_t)remain;
            memcpy(cmd + 2, in + off, (size_t)chunk);
            if (m->io.send(m->io.ctx, cmd, 2 + chunk) != 2 + chunk) return -1;
            off += chunk;
        }
        {
            uint8_t rsp[XCP_MAX_CTO];
            int rn = m->io.recv(m->io.ctx, rsp, (int)sizeof(rsp), 1000);
            if (rn < 1 || rsp[0] != XCP_PID_RES) {
                if (rn >= 2 && rsp[0] == XCP_PID_ERR) {
                    m->last_err = rsp[1];
                    return rsp[1];
                }
                return -1;
            }
        }
    }
    return 0;
}

int xcp_master_checksum(XcpMaster* m, uint32_t addr, uint32_t size, uint32_t* sum)
{
    uint8_t mta[8], cmd[8], rsp[XCP_MAX_CTO];
    int rc;
    mta[0] = XCP_CMD_SET_MTA;
    mta[1] = mta[2] = mta[3] = 0;
    xcp_wr32(mta + 4, addr);
    rc = xfer_ok(m, mta, 8);
    if (rc != 0) return rc;
    cmd[0] = XCP_CMD_BUILD_CHECKSUM;
    cmd[1] = cmd[2] = cmd[3] = 0;
    xcp_wr32(cmd + 4, size);
    rc = xfer(m, cmd, 8, rsp, (int)sizeof(rsp));
    if (rc != 0) return rc;
    if (rsp[1] != XCP_CHECKSUM_ADD_14) return -1;
    if (sum) *sum = xcp_rd32(rsp + 4);
    return 0;
}

int xcp_master_unlock_cal(XcpMaster* m)
{
    uint8_t cmd[8], rsp[XCP_MAX_CTO];
    int rc, i;
    cmd[0] = XCP_CMD_GET_SEED;
    cmd[1] = 0;
    cmd[2] = XCP_RES_CAL_PAG;
    rc = xfer(m, cmd, 3, rsp, (int)sizeof(rsp));
    if (rc != 0) return rc;
    if (rsp[1] == 0) return 0;
    if (rsp[1] != 4) return -1;
    cmd[0] = XCP_CMD_UNLOCK;
    cmd[1] = 4;
    for (i = 0; i < 4; i++) cmd[2 + i] = (uint8_t)(rsp[2 + i] ^ 0xFF);
    return xfer_ok(m, cmd, 6);
}

int xcp_master_daq_free(XcpMaster* m)
{
    uint8_t cmd = XCP_CMD_FREE_DAQ;
    return xfer_ok(m, &cmd, 1);
}

int xcp_master_daq_alloc(XcpMaster* m, int n_daq, int n_odt, int n_entry)
{
    uint8_t cmd[8];
    int rc, d, o;
    rc = xcp_master_daq_free(m);
    if (rc != 0) return rc;
    cmd[0] = XCP_CMD_ALLOC_DAQ;
    cmd[1] = 0;
    xcp_wr16(cmd + 2, (uint16_t)n_daq);
    rc = xfer_ok(m, cmd, 4);
    if (rc != 0) return rc;
    for (d = 0; d < n_daq; d++) {
        cmd[0] = XCP_CMD_ALLOC_ODT;
        cmd[1] = 0;
        xcp_wr16(cmd + 2, (uint16_t)d);
        cmd[4] = (uint8_t)n_odt;
        rc = xfer_ok(m, cmd, 5);
        if (rc != 0) return rc;
        for (o = 0; o < n_odt; o++) {
            cmd[0] = XCP_CMD_ALLOC_ODT_ENTRY;
            cmd[1] = 0;
            xcp_wr16(cmd + 2, (uint16_t)d);
            cmd[4] = (uint8_t)o;
            cmd[5] = (uint8_t)n_entry;
            rc = xfer_ok(m, cmd, 6);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int xcp_master_daq_entry(XcpMaster* m, uint16_t daq, uint8_t odt, uint8_t entry,
                         uint32_t addr, uint8_t size)
{
    uint8_t cmd[8];
    int rc;
    cmd[0] = XCP_CMD_SET_DAQ_PTR;
    cmd[1] = 0;
    xcp_wr16(cmd + 2, daq);
    cmd[4] = odt;
    cmd[5] = entry;
    rc = xfer_ok(m, cmd, 6);
    if (rc != 0) return rc;
    cmd[0] = XCP_CMD_WRITE_DAQ;
    cmd[1] = 0xFF;
    cmd[2] = size;
    cmd[3] = 0;
    xcp_wr32(cmd + 4, addr);
    return xfer_ok(m, cmd, 8);
}

int xcp_master_daq_start(XcpMaster* m, uint16_t daq, uint16_t event)
{
    uint8_t cmd[8];
    int rc;
    cmd[0] = XCP_CMD_SET_DAQ_LIST_MODE;
    cmd[1] = XCP_DAQ_FLAG_TIMESTAMP;
    xcp_wr16(cmd + 2, daq);
    xcp_wr16(cmd + 4, event);
    cmd[6] = 1;
    cmd[7] = 0;
    rc = xfer_ok(m, cmd, 8);
    if (rc != 0) return rc;
    cmd[0] = XCP_CMD_START_STOP_DAQ_LIST;
    cmd[1] = XCP_DAQ_START_START;
    xcp_wr16(cmd + 2, daq);
    return xfer_ok(m, cmd, 4);
}

int xcp_master_recv_dto(XcpMaster* m, uint8_t* dst, int cap, int timeout_ms)
{
    if (!m->io.recv) return -1;
    return m->io.recv(m->io.ctx, dst, cap, timeout_ms);
}
