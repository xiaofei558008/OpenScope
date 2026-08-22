#ifndef OS_NETSESSION_H
#define OS_NETSESSION_H
/* 远程协议会话的载荷序列化（长度前缀 + varint），纯 C、可单测。 */
#include <stdint.h>

#define OS_NET_NAME_MAX 256

typedef struct OS_NetVar {
    char     name[OS_NET_NAME_MAX];
    uint64_t addr;
    uint32_t size;
} OS_NetVar;

int os_net_encode_varlist(const OS_NetVar* v, int n, uint8_t* out, int cap);
int os_net_decode_varlist(const uint8_t* in, int len, OS_NetVar* v, int max);

int os_net_encode_names(const char* const* names, int n, uint8_t* out, int cap);
int os_net_decode_names(const uint8_t* in, int len, char names[][OS_NET_NAME_MAX], int max);

int os_net_encode_write(const char* name, const char* text, uint8_t* out, int cap);
int os_net_decode_write(const uint8_t* in, int len, char* name, int name_cap, char* text, int text_cap);

int os_net_encode_ack(int code, const char* msg, uint8_t* out, int cap);
int os_net_decode_ack(const uint8_t* in, int len, int* code, char* msg, int msg_cap);

#endif
