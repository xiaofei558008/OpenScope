/*
 * OpenScope DWARF 位置表达式求值 / 位置列表选择 / CFA（.debug_frame）查找
 *
 * 纯逻辑模块（无 Windows 依赖），供 elf.c 与局部变量窗口使用，可在 box 上单测。
 * 寄存器编号为 DWARF 编号：
 *   ARM:     0..15 = R0..R15
 *   TriCore: 0..15 = D0..D15，16..31 = A0..A15，32..39 = E0,E2..E14（64 位 D 寄存器对），40 = PC
 */
#ifndef OS_DWLOC_H
#define OS_DWLOC_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DWLOC_MAX_REGS 64

typedef enum {
    DWLOC_NONE = 0,     /* 无位置（optimized out / 当前 PC 不可用） */
    DWLOC_MEM,          /* 值在内存 addr */
    DWLOC_REG,          /* 值在寄存器 reg（整个变量） */
    DWLOC_VALUE,        /* 隐式值（stack_value / implicit_value），见 bytes */
    DWLOC_PIECES,       /* 多片组合（寄存器/内存片拼接），见 bytes + valid_len */
    DWLOC_ERROR         /* 不支持的操作码或读寄存器/内存失败 */
} DwLocKind;

typedef struct {
    int      arch;                         /* 1=ARM 2=TriCore（影响 32..39 寄存器对） */
    int      addr_size;                    /* 4 */
    int      le;                           /* 1=小端 */
    uint64_t reg[DWLOC_MAX_REGS];
    uint8_t  reg_valid[DWLOC_MAX_REGS];
    int      have_frame_base;
    uint64_t frame_base;
    int      have_cfa;
    uint64_t cfa;
    /* 读目标内存（deref / 拼片时用）；返回 0 成功 */
    int    (*read_mem)(void* user, uint64_t addr, void* buf, uint32_t size);
    void*    user;
} DwLocCtx;

typedef struct {
    DwLocKind kind;
    uint64_t  addr;        /* MEM */
    int       reg;         /* REG */
    uint8_t   bytes[16];   /* VALUE / PIECES：小端字节 */
    int       len;         /* VALUE/PIECES 有效字节数 */
    int       partial;     /* PIECES 中有片不可用 */
    char      err[64];
} DwLocResult;

/* 读寄存器值（含 TriCore E 对：返回 64 位）；不可用返回 -1 */
int dwloc_reg_value(const DwLocCtx* c, int reg, uint64_t* out, int* size_bytes);

/* 求值一个位置表达式。 */
void dwloc_eval(const uint8_t* expr, size_t len, const DwLocCtx* c, DwLocResult* out);

/* 若结果为 REG：把寄存器值按 size 物化为字节（便于统一显示）。成功返回 0。 */
int dwloc_materialize(const DwLocCtx* c, const DwLocResult* r, uint8_t* buf, uint32_t size);

/* DWARF 2..4 .debug_loc：在 off 处的列表里找覆盖 pc 的表达式。
 * cu_base = CU 的 DW_AT_low_pc（无则 0）。命中返回 1 并写 expr/len；不覆盖返回 0；损坏返回 -1。 */
int dwloc_loclist_v4(const uint8_t* sec, size_t sec_size, uint64_t off, int addr_size, int le,
                     uint64_t cu_base, uint64_t pc, const uint8_t** expr, size_t* len);

/* DWARF 5 .debug_loclists：off 为列表起始（已加 loclists_base / 已解析 loclistx）。
 * debug_addr/addr_base 用于 *x 形式（可为 NULL）。 */
int dwloc_loclist_v5(const uint8_t* sec, size_t sec_size, uint64_t off, int addr_size, int le,
                     uint64_t cu_base, const uint8_t* debug_addr, size_t debug_addr_size,
                     uint64_t addr_base, uint64_t pc, const uint8_t** expr, size_t* len);

/* 范围列表：v4 .debug_ranges / v5 .debug_rnglists；pc 在任一区间内返回 1 */
int dwloc_ranges_v4_contains(const uint8_t* sec, size_t sec_size, uint64_t off, int addr_size, int le,
                             uint64_t cu_base, uint64_t pc);
int dwloc_ranges_v5_contains(const uint8_t* sec, size_t sec_size, uint64_t off, int addr_size, int le,
                             uint64_t cu_base, const uint8_t* debug_addr, size_t debug_addr_size,
                             uint64_t addr_base, uint64_t pc);

/* .debug_frame：求 pc 处 CFA 规则（寄存器 + 偏移）。成功返回 1。 */
int dwloc_cfa_rule(const uint8_t* frame, size_t frame_size, int addr_size, int le, uint64_t pc,
                   int* cfa_reg, int64_t* cfa_off);

#ifdef __cplusplus
}
#endif
#endif
