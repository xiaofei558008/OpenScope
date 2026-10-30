#ifndef XCP_PLAT_H
#define XCP_PLAT_H
/* 可移植层：互斥/条件变量/线程/单调时钟/原子/套接字。
 * C99，MSVC 与 gcc/mingw 均可编译；不使用 VLA。 */
#include <stdint.h>

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  include <windows.h>
typedef SOCKET xcp_sock_t;
#  define XCP_INVALID_SOCK INVALID_SOCKET
typedef struct { SRWLOCK l; } xcp_mutex_t;
typedef struct { CONDITION_VARIABLE c; } xcp_cond_t;
typedef struct { HANDLE h; unsigned id; } xcp_thread_t;
typedef unsigned long xcp_tid_t;
#else
#  include <pthread.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  include <arpa/inet.h>
#  include <unistd.h>
typedef int xcp_sock_t;
#  define XCP_INVALID_SOCK (-1)
typedef struct { pthread_mutex_t m; } xcp_mutex_t;
typedef struct { pthread_cond_t c; } xcp_cond_t;
typedef struct { pthread_t t; int valid; } xcp_thread_t;
typedef pthread_t xcp_tid_t;
#endif

/* ---- 原子（32 位，顺序一致） ---- */
typedef struct { volatile long v; } xcp_atomic_t;
long xcp_atomic_load(const xcp_atomic_t* a);
void xcp_atomic_store(xcp_atomic_t* a, long v);
long xcp_atomic_add(xcp_atomic_t* a, long d); /* 返回新值 */

/* ---- 锁 / 条件变量 ---- */
void xcp_mutex_init(xcp_mutex_t* m);
void xcp_mutex_destroy(xcp_mutex_t* m);
void xcp_mutex_lock(xcp_mutex_t* m);
void xcp_mutex_unlock(xcp_mutex_t* m);
void xcp_cond_init(xcp_cond_t* c);
void xcp_cond_destroy(xcp_cond_t* c);
/* 等待至多 ms 毫秒；返回 0 被唤醒（可能虚假唤醒），1 超时 */
int  xcp_cond_wait_ms(xcp_cond_t* c, xcp_mutex_t* m, int ms);
void xcp_cond_broadcast(xcp_cond_t* c);

/* ---- 线程 ---- */
typedef void (*xcp_thread_fn)(void* arg);
int  xcp_thread_start(xcp_thread_t* t, xcp_thread_fn fn, void* arg);
void xcp_thread_join(xcp_thread_t* t);
int  xcp_thread_is_self(const xcp_thread_t* t);

/* ---- 时间 ---- */
uint64_t xcp_now_ms(void);   /* 单调 */
void     xcp_sleep_ms(int ms);

/* ---- 套接字 ---- */
int  xcp_net_init(void);     /* Windows 下 WSAStartup（引用计数），POSIX 空操作 */
void xcp_net_cleanup(void);
void xcp_sock_close(xcp_sock_t s);
void xcp_sock_shutdown(xcp_sock_t s);
/* 等可读：1 可读，0 超时，<0 错误 */
int  xcp_sock_wait_readable(xcp_sock_t s, int ms);

#endif
