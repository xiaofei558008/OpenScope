/* XCP over UART（Win32）。
 * 帧格式（主机与 MCU 约定，便于任意 UART）：
 *   [u16 LE payload_len][payload...]
 * payload 即 XCP CTO/DTO（含 PID）。
 * 波特率默认 115200；opts="COMn" 或 "COMn:115200"。
 */
#include "xcp_transport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct SerCtx {
    HANDLE h;
    char   port[32];
    int    baud;
    CRITICAL_SECTION cs;
} SerCtx;

static int parse_opts(const char* opts, char* port, int port_cap, int* baud)
{
    const char* colon;
    if (!opts || !opts[0]) return -1;
    colon = strchr(opts, ':');
    if (colon) {
        size_t n = (size_t)(colon - opts);
        if (n >= (size_t)port_cap) n = (size_t)port_cap - 1;
        memcpy(port, opts, n);
        port[n] = 0;
        *baud = atoi(colon + 1);
        if (*baud <= 0) *baud = 115200;
    } else {
        _snprintf(port, port_cap, "%s", opts);
        *baud = 115200;
    }
    return 0;
}

static int ser_scan(char names[][64], int cap)
{
    int n = 0, i;
    for (i = 1; i <= 32 && n < cap; i++) {
        char path[32];
        HANDLE h;
        _snprintf(path, sizeof(path), "\\\\.\\COM%d", i);
        h = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                        OPEN_EXISTING, 0, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            _snprintf(names[n], 64, "COM%d", i);
            CloseHandle(h);
            n++;
        }
    }
    return n;
}

static int ser_apply_dcb(HANDLE h, int baud)
{
    DCB dcb;
    COMMTIMEOUTS to;
    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(h, &dcb)) return -1;
    dcb.BaudRate = (DWORD)baud;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary = TRUE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    if (!SetCommState(h, &dcb)) return -1;
    memset(&to, 0, sizeof(to));
    to.ReadIntervalTimeout = 20;
    to.ReadTotalTimeoutConstant = 200;
    to.ReadTotalTimeoutMultiplier = 2;
    to.WriteTotalTimeoutConstant = 200;
    to.WriteTotalTimeoutMultiplier = 2;
    SetCommTimeouts(h, &to);
    return 0;
}

static int ser_open(void** out, int index, const char* opts)
{
    SerCtx* s;
    char names[32][64];
    char port[32], path[40];
    int baud = 115200, found, i;
    HANDLE h;
    if (!out) return -1;
    *out = NULL;
    if (opts && opts[0]) {
        if (parse_opts(opts, port, sizeof(port), &baud) != 0) return -1;
    } else {
        found = ser_scan(names, 32);
        if (index < 0 || index >= found) return -1;
        _snprintf(port, sizeof(port), "%s", names[index]);
    }
    /* 规范化 COM 名 */
    if (_strnicmp(port, "COM", 3) != 0) return -1;
    i = atoi(port + 3);
    if (i <= 0) return -1;
    _snprintf(path, sizeof(path), "\\\\.\\COM%d", i);
    h = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                    OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) return -1;
    if (ser_apply_dcb(h, baud) != 0) {
        CloseHandle(h);
        return -1;
    }
    PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR);
    s = (SerCtx*)calloc(1, sizeof(SerCtx));
    if (!s) { CloseHandle(h); return -1; }
    s->h = h;
    s->baud = baud;
    _snprintf(s->port, sizeof(s->port), "COM%d", i);
    InitializeCriticalSection(&s->cs);
    *out = s;
    return 0;
}

static void ser_close(void* ctx)
{
    SerCtx* s = (SerCtx*)ctx;
    if (!s) return;
    if (s->h != INVALID_HANDLE_VALUE) CloseHandle(s->h);
    DeleteCriticalSection(&s->cs);
    free(s);
}

static int ser_write_all(HANDLE h, const uint8_t* p, int n)
{
    int off = 0;
    while (off < n) {
        DWORD wr = 0;
        if (!WriteFile(h, p + off, (DWORD)(n - off), &wr, NULL) || wr == 0)
            return -1;
        off += (int)wr;
    }
    return 0;
}

static int ser_read_exact(HANDLE h, uint8_t* p, int n, int timeout_ms)
{
    int off = 0;
    DWORD t0 = GetTickCount();
    while (off < n) {
        DWORD rd = 0;
        if ((int)(GetTickCount() - t0) > timeout_ms) return -1;
        if (!ReadFile(h, p + off, (DWORD)(n - off), &rd, NULL))
            return -1;
        if (rd == 0) {
            Sleep(1);
            continue;
        }
        off += (int)rd;
    }
    return 0;
}

/* 收到的帧大于调用方缓冲：读走丢弃，保持后续帧边界不撕裂 */
static void ser_discard(HANDLE h, int n, int timeout_ms)
{
    uint8_t tmp[64];
    DWORD t0 = GetTickCount();
    while (n > 0) {
        int chunk = n > (int)sizeof(tmp) ? (int)sizeof(tmp) : n;
        if (ser_read_exact(h, tmp, chunk, timeout_ms) != 0) return;
        n -= chunk;
        if ((int)(GetTickCount() - t0) > timeout_ms * 2) return;
    }
}

static int ser_send(void* ctx, const uint8_t* data, int len)
{
    SerCtx* s = (SerCtx*)ctx;
    uint8_t hdr[2];
    int rc;
    if (!s || !data || len <= 0 || len > 0xFFFF) return -1;
    hdr[0] = (uint8_t)(len & 0xFF);
    hdr[1] = (uint8_t)((len >> 8) & 0xFF);
    EnterCriticalSection(&s->cs);
    rc = ser_write_all(s->h, hdr, 2);
    if (rc == 0) rc = ser_write_all(s->h, data, len);
    LeaveCriticalSection(&s->cs);
    return rc == 0 ? len : -1;
}

static int ser_recv(void* ctx, uint8_t* data, int cap, int timeout_ms)
{
    SerCtx* s = (SerCtx*)ctx;
    uint8_t hdr[2];
    int len, rc;
    if (!s || !data || cap <= 0) return -1;
    if (timeout_ms <= 0) timeout_ms = 500;
    EnterCriticalSection(&s->cs);
    rc = ser_read_exact(s->h, hdr, 2, timeout_ms);
    if (rc != 0) { LeaveCriticalSection(&s->cs); return -1; }
    len = (int)hdr[0] | ((int)hdr[1] << 8);
    if (len <= 0) { LeaveCriticalSection(&s->cs); return -1; }
    if (len > cap) {
        ser_discard(s->h, len, timeout_ms);
        LeaveCriticalSection(&s->cs);
        return -1;
    }
    rc = ser_read_exact(s->h, data, len, timeout_ms);
    LeaveCriticalSection(&s->cs);
    return rc == 0 ? len : -1;
}

static void ser_bind(void* ctx, XcpIo* io)
{
    if (!io) return;
    io->ctx = ctx;
    io->send = ser_send;
    io->recv = ser_recv;
}

static const XcpTransport g_serial = {
    "serial",
    "串口",
    ser_scan,
    ser_open,
    ser_close,
    ser_bind
};

void xcp_serial_register(void)
{
    xcp_transport_register(&g_serial);
}
