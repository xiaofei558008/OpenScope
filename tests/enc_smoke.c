/*
 * OpenScope real-world .out regression test (IAR, multi-CU DWARF4).
 *
 * Verifies that tests/enc.out parses all global variables with DWARF
 * types and expands structs/unions/arrays down to atomic leaves.
 */
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "app.h"
#include "elf.h"
#include "vartree.h"

OS_App g_app;

static int g_fails;

#define CHECK(c, what) do { \
    printf("%s %s\n", (c) ? "PASS" : "FAIL", what); \
    if (!(c)) g_fails++; \
} while (0)

static const OS_Leaf* leaf_by_name(const char* name)
{
    int i;
    for (i = 0; i < g_app.leaf_count; i++) {
        if (strcmp(g_app.leaves[i].name, name) == 0) return &g_app.leaves[i];
    }
    return NULL;
}

static int var_index(OS_ElfFile* elf, const char* name)
{
    int i;
    for (i = 0; i < os_elf_var_count(elf); i++) {
        const OS_Variable* v = os_elf_var_at(elf, i);
        if (v && v->name && strcmp(v->name, name) == 0) return i;
    }
    return -1;
}

int main(void)
{
    char err[256] = "";
    OS_ElfFile* elf;
    int idx, i;

    setvbuf(stdout, NULL, _IONBF, 0);
    memset(&g_app, 0, sizeof(g_app));

    elf = os_elf_open("tests/enc.out", err, sizeof(err));
    CHECK(elf != NULL, "os_elf_open loads tests/enc.out");
    if (!elf) {
        printf("  err: %s\n", err);
        return 1;
    }
    CHECK(os_elf_bits(elf) == 32 && os_elf_machine(elf) == 40,
          "ELF32 ARM");
    CHECK(os_elf_var_count(elf) >= 16, ">= 16 global variables parsed");

    /* every variable must carry a DWARF type now */
    {
        int typed = 0;
        for (i = 0; i < os_elf_var_count(elf); i++) {
            const OS_Variable* v = os_elf_var_at(elf, i);
            if (v && v->type && v->has_debug) typed++;
        }
        CHECK(typed == os_elf_var_count(elf),
              "all globals have DWARF types");
    }

    /* key globals with expected addresses/sizes */
    idx = var_index(elf, "ADC_Temper");
    CHECK(idx >= 0, "ADC_Temper present");
    if (idx >= 0) {
        const OS_Variable* v = os_elf_var_at(elf, idx);
        CHECK(v->address == 0x20000934u && v->symbol_size == 12,
              "ADC_Temper @0x20000934 size 12");
        CHECK(v->type && v->type->kind == OS_TYPE_STRUCT &&
              v->type->child_count == 4, "ADC_Temper struct with 4 members");
    }
    idx = var_index(elf, "hall_chk");
    CHECK(idx >= 0, "hall_chk present");
    if (idx >= 0) {
        const OS_Variable* v = os_elf_var_at(elf, idx);
        CHECK(v->address == 0x20000918u && v->symbol_size == 28,
              "hall_chk @0x20000918 size 28");
    }
    idx = var_index(elf, "MT6835");
    CHECK(idx >= 0, "MT6835 present");
    if (idx >= 0) {
        const OS_Variable* v = os_elf_var_at(elf, idx);
        CHECK(v->address == 0x20000840u, "MT6835 @0x20000840");
        CHECK(v->type && v->type->kind == OS_TYPE_STRUCT,
              "MT6835 struct type resolved");
    }
    idx = var_index(elf, "ta_enc");
    CHECK(idx >= 0, "ta_enc present");
    if (idx >= 0) {
        const OS_Variable* v = os_elf_var_at(elf, idx);
        CHECK(v->address == 0x200008B8u && v->symbol_size == 96,
              "ta_enc @0x200008B8 size 96");
    }

    /* struct expansion to atomic leaves */
    g_app.elf = elf;
    os_vartree_build();
    CHECK(g_app.leaf_count >= 285, ">= 285 atomic leaves");
    {
        int structs = 0, zero = 0;
        for (i = 0; i < g_app.leaf_count; i++) {
            if (g_app.leaves[i].kind == OS_TYPE_STRUCT ||
                g_app.leaves[i].kind == OS_TYPE_UNION) structs++;
            if (g_app.leaves[i].size == 0) zero++;
        }
        CHECK(structs == 0, "no struct/union leaves remain");
        CHECK(zero == 0, "no zero-size leaves");
    }
    {
        const OS_Leaf* l;
        l = leaf_by_name("MT6835.AbsEnc.AngleBit");
        CHECK(l && l->address == 0x20000840u && l->size == 4,
              "nested leaf MT6835.AbsEnc.AngleBit @0x20000840");
        l = leaf_by_name("ta_enc.index_tx");
        CHECK(l && l->address == 0x200008B8u, "leaf ta_enc.index_tx");
        l = leaf_by_name("modbus_rtu_slave.addr");
        CHECK(l && l->address == 0x20000608u, "leaf modbus_rtu_slave.addr");
        l = leaf_by_name("hall_chk.error");
        CHECK(l && l->address == 0x20000918u, "leaf hall_chk.error");
        l = leaf_by_name("ADC_Temper.MCU_Temper");
        CHECK(l && l->address == 0x2000093Cu, "leaf ADC_Temper.MCU_Temper");
        l = leaf_by_name("aCRCh");
        CHECK(l && l->address == 0x20000000u && l->size == 256,
              "char array aCRCh kept as single leaf (256B)");
    }

    os_elf_close(elf);
    g_app.elf = NULL;

    printf(g_fails == 0 ? "ALL PASS\n" : "FAILURES: %d\n", g_fails);
    return g_fails == 0 ? 0 : 1;
}
