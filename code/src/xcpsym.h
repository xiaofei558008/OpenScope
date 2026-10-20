/*
 * XCP 符号注册表：合并「宿主已加载 ELF 的叶变量」与「手工加载的 .map 符号」，
 * 供 XCP 标定页和 AI 接口共用（按名字查询地址 / 类型 / 读写值）。
 */
#ifndef OS_XCPSYM_H
#define OS_XCPSYM_H

#include <stdint.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OS_XcpSym {
    char     name[256];
    uint64_t address;
    uint32_t size;       /* 字节数（位域为存储单元大小） */
    int      kind;       /* OS_TypeKind（来自 ELF；MAP 符号为 OS_TYPE_UINT） */
    int      is_signed;
    int      is_float;
    int      is_bitfield;
    uint8_t  bit_offset;
    uint8_t  bit_size;
    int      leaf_id;    /* >=0：ELF 叶 id；-1：MAP 符号 */
    int      from_map;
} OS_XcpSym;

/* 载入 .map（替换已加载的 map）。成功返回符号数，失败 <0。 */
int  os_xcpsym_load_map(const wchar_t* path_w, char* err, int errcap);
int  os_xcpsym_clear_map(void);
/** 当前 map 文件路径（UTF-8），无则空串。 */
void os_xcpsym_map_path(char* out, int cap);

/* ELF 重载后调用：使缓存失效（下次访问重建）。 */
void os_xcpsym_invalidate(void);

int  os_xcpsym_count(void);
int  os_xcpsym_get(int index, OS_XcpSym* out);
int  os_xcpsym_find(const char* name, OS_XcpSym* out);
/** 模糊搜索（不区分大小写的子串），返回写入 out 的条数。 */
int  os_xcpsym_search(const char* needle, int offset, int limit,
                      OS_XcpSym* out, int cap);

/** 把内存字节格式化为可读值（整数十进制 / 浮点 / 位域提取）。 */
int  os_xcpsym_format_value(const OS_XcpSym* s, const uint8_t* data, int n,
                            char* out, int cap);
/** 把文本值（十进制、0x 十六进制、浮点）编码为小端字节；返回字节数。 */
int  os_xcpsym_encode_value(const OS_XcpSym* s, const char* text,
                            uint8_t* out, int cap, char* err, int errcap);
/** 位域掩码（未移位的存储单元内掩码）。 */
uint64_t os_xcpsym_bit_mask(const OS_XcpSym* s);

#ifdef __cplusplus
}
#endif

#endif
