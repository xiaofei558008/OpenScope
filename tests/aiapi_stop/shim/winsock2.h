/* 最小 Winsock shim（Linux/gcc 跑 tests/aiapi_stop 用；Windows 上用真正的 winsock2.h） */
#ifndef OS_TEST_WINSOCK2_SHIM_H
#define OS_TEST_WINSOCK2_SHIM_H
#include <windows.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>

typedef int SOCKET;
typedef unsigned short u_short;
typedef struct { int dummy; } WSADATA;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define SD_BOTH SHUT_RDWR
#define MAKEWORD(a, b) ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define _strnicmp strncasecmp
static inline int WSAStartup(WORD v, WSADATA* d) { (void)v; (void)d; return 0; }
static inline int closesocket(SOCKET s) { return close(s); }
/* Windows 的 SO_RCVTIMEO/SO_SNDTIMEO 取 DWORD 毫秒；POSIX 取 timeval */
static inline int shim_setsockopt(SOCKET s, int lvl, int opt, const char* v, int len)
{
    if (lvl == SOL_SOCKET && (opt == SO_RCVTIMEO || opt == SO_SNDTIMEO) && len == (int)sizeof(DWORD)) {
        DWORD ms = *(const DWORD*)v; struct timeval tv;
        tv.tv_sec = ms / 1000; tv.tv_usec = (ms % 1000) * 1000;
        return setsockopt(s, lvl, opt, &tv, sizeof(tv));
    }
    return setsockopt(s, lvl, opt, v, (socklen_t)len);
}
#define setsockopt shim_setsockopt
#endif
