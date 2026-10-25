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
    const struct OS_Type* base;  /* 结构体成员节点：指向成员的原始类型（类型名/array_count），否则 NULL */
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

/* ---- DWARF .debug_line：源码行 ↔ 地址 ---- */
int         os_elf_line_count(OS_ElfFile* f);
int         os_elf_line_file_count(OS_ElfFile* f);
const char* os_elf_line_file_at(OS_ElfFile* f, int idx);
/* 按文件路径/文件名 + 行号查指令地址；成功返回 1 并写 *out_addr */
int         os_elf_addr_of_line(OS_ElfFile* f, const char* path_or_name, int line,
                                uint32_t* out_addr);
/* 按地址查源文件与行号；成功返回 1 */
int         os_elf_line_of_addr(OS_ElfFile* f, uint32_t addr,
                                char* file_out, int file_cap, int* line_out);

/* ---- 函数 / 局部变量（DWARF subprogram/lexical_block/formal_parameter/variable）---- */
enum {
    OS_LOC_OPTIMIZED_OUT = 0, /* 无 DW_AT_location，或空表达式 */
    OS_LOC_EXPR          = 1, /* expr/expr_len 为当前 PC 下的位置表达式 */
    OS_LOC_NOT_AT_PC     = 2, /* 位置列表在当前 PC 不覆盖 */
    OS_LOC_CONST         = 3, /* DW_AT_const_value 整数（const_val） */
    OS_LOC_CONST_BYTES   = 4  /* DW_AT_const_value 块（expr/expr_len 为原始字节） */
};
typedef struct OS_LocalSym {
    const char*    name;
    const OS_Type* type;     /* 可能为 NULL */
    int            is_param;
    int            depth;    /* 作用域嵌套深度：0=函数级 */
    int            loc_kind; /* OS_LOC_* */
    const uint8_t* expr;
    uint32_t       expr_len;
    int64_t        const_val;
} OS_LocalSym;

/* 按 PC 找函数；成功返回 1 */
int os_elf_func_at(OS_ElfFile* f, uint64_t pc, char* name, int cap, uint64_t* lo, uint64_t* hi);
/* 当前 PC 作用域内的参数与局部变量（参数在前）；无函数返回 -1。fb_* 为当前 PC 的 frame_base 表达式 */
int os_elf_locals_at(OS_ElfFile* f, uint64_t pc, OS_LocalSym* out, int max,
                     const uint8_t** fb_expr, uint32_t* fb_len);
/* .debug_frame：pc 处 CFA = reg + off；成功返回 1 */
int os_elf_cfa_at(OS_ElfFile* f, uint64_t pc, int* reg, int64_t* off);
int os_elf_func_count(OS_ElfFile* f);

#ifdef __cplusplus
}
#endif

#endif /* OPENSCOPE_ELF_H */
