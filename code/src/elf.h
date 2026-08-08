/*
 * OpenScope ELF/DWARF 变量解析接口
 *
 * 解析 ELF32/ELF64 文件：读取符号表得到全局变量地址，
 * 结合 DWARF 调试信息还原变量类型（基础类型/结构体/联合体/数组/枚举/指针等）。
 */
#ifndef OPENSCOPE_ELF_H
#define OPENSCOPE_ELF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum OS_TypeKind {
    OS_TYPE_VOID = 0,
    OS_TYPE_INT,
    OS_TYPE_UINT,
    OS_TYPE_FLOAT,
    OS_TYPE_BOOL,
    OS_TYPE_STRUCT,
    OS_TYPE_UNION,
    OS_TYPE_ARRAY,
    OS_TYPE_ENUM,
    OS_TYPE_PTR,
    OS_TYPE_STRING,
    OS_TYPE_OTHER
} OS_TypeKind;

/* 类型节点。子节点（结构体成员/数组元素/枚举值）通过 children 组织。 */
typedef struct OS_Type {
    char*         name;          /* 如 "uint32_t"、"struct Foo"、"int[4]"；malloc 分配 */
    OS_TypeKind   kind;
    uint32_t      size;          /* 字节数，未知为 0 */
    int           is_signed;
    int           is_ptr;
    int           is_bitfield;
    uint8_t       bit_offset;    /* 位域：存储单元内 LSB 起始位 */
    uint8_t       bit_size;      /* 位域位宽 */
    int64_t       member_offset; /* 结构体成员相对首地址的字节偏移；位域为存储字节偏移 */
    int64_t       enum_value;    /* 枚举子节点取值 */
    int           array_count;   /* 数组元素个数 */
    int           child_count;
    struct OS_Type** children;   /* malloc 数组，可为 NULL */
} OS_Type;

/* 顶层全局变量 */
typedef struct OS_Variable {
    char*       name;
    uint64_t    address;     /* 绝对地址 */
    uint64_t    symbol_size; /* 符号表大小 */
    OS_Type*    type;        /* 可能为 NULL（仅有符号信息） */
    int         has_debug;   /* 1 = 类型来自 DWARF */
} OS_Variable;

typedef struct OS_ElfFile OS_ElfFile;

/* 打开 ELF 文件；失败返回 NULL 并填充 errbuf。 */
OS_ElfFile* os_elf_open(const char* path, char* errbuf, int errbuf_len);
void        os_elf_close(OS_ElfFile* f);

uint16_t    os_elf_machine(OS_ElfFile* f);   /* EM_* 值 */
uint32_t    os_elf_flags(OS_ElfFile* f);     /* e_flags */
int         os_elf_bits(OS_ElfFile* f);      /* 32 或 64 */
const char* os_elf_arch_name(OS_ElfFile* f); /* 静态字符串，如 "ARM Cortex-M" */
uint64_t    os_elf_entry(OS_ElfFile* f);

int  os_elf_var_count(OS_ElfFile* f);
const OS_Variable* os_elf_var_at(OS_ElfFile* f, int idx);
int  os_elf_find_var(OS_ElfFile* f, const char* name);  /* 精确匹配，返回索引或 -1 */
/* 模糊搜索（子串匹配，不区分大小写），返回匹配数，结果写入 out（最多 max 个索引）。 */
int  os_elf_find_vars(OS_ElfFile* f, const char* needle, int max, int* out);

#ifdef __cplusplus
}
#endif

#endif /* OPENSCOPE_ELF_H */
