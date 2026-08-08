/*
 * OpenScope real-world .out regression test (IAR DWARF).
 *
 * Verifies a real IAR .out parses all global variables with DWARF types
 * and expands structs/unions/arrays down to atomic leaves.  The fixture is
 * the user's project file tests/linix_stm32l031_v1.2.out (18 globals,
 * 511 atomic leaves); tests/enc.out is kept as a fallback.
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

    elf = os_elf_open("tests/linix_stm32l031_v1.2.out", err, sizeof(err));
    if (!elf)
        elf = os_elf_open("tests/enc.out", err, sizeof(err));
    CHECK(elf != NULL, "os_elf_open loads real IAR .out");
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
    idx = var_index(elf, "SystemCoreClock");
    CHECK(idx >= 0, "SystemCoreClock present");
    if (idx >= 0) {
        const OS_Variable* v = os_elf_var_at(elf, idx);
        CHECK(v->address == 0x20000000u && v->symbol_size == 4,
              "SystemCoreClock @0x20000000 size 4");
    }
    idx = var_index(elf, "vofa");
    CHECK(idx >= 0, "vofa present");
    if (idx >= 0) {
        const OS_Variable* v = os_elf_var_at(elf, idx);
        CHECK(v->address == 0x20000004u && v->symbol_size == 400,
              "vofa @0x20000004 size 400");
        CHECK(v->type && v->type->kind == OS_TYPE_STRUCT,
              "vofa struct type resolved");
    }
    idx = var_index(elf, "AbsEnc");
    CHECK(idx >= 0, "AbsEnc present");
    if (idx >= 0) {
        const OS_Variable* v = os_elf_var_at(elf, idx);
        CHECK(v->address == 0x20000258u && v->symbol_size == 180,
              "AbsEnc @0x20000258 size 180");
        CHECK(v->type && v->type->kind == OS_TYPE_STRUCT,
              "AbsEnc struct type resolved");
    }
    idx = var_index(elf, "MT6835");
    CHECK(idx >= 0, "MT6835 present");
    if (idx >= 0) {
        const OS_Variable* v = os_elf_var_at(elf, idx);
        CHECK(v->address == 0x2000030Cu && v->symbol_size == 64,
              "MT6835 @0x2000030C size 64");
    }

    /* struct expansion to atomic leaves */
    g_app.elf = elf;
    os_vartree_build();
    CHECK(g_app.leaf_count >= 300, ">= 300 atomic leaves");
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
        l = leaf_by_name("AbsEnc.anon.FRawAng");
        CHECK(l && l->address == 0x20000258u && l->size == 4,
              "bitfield leaf AbsEnc.anon.FRawAng @0x20000258");
        l = leaf_by_name("AbsEnc.Param.anon.AngleBit");
        CHECK(l && l->address == 0x2000025Cu && l->size == 4,
              "nested leaf AbsEnc.Param.anon.AngleBit @0x2000025C");
        l = leaf_by_name("SystemCoreClock");
        CHECK(l && l->address == 0x20000000u && l->size == 4,
              "leaf SystemCoreClock @0x20000000");
        l = leaf_by_name("MT6835.EEPROM.reg0x09.anon.zero_posi_11_4");
        CHECK(l && l->address == 0x2000031Du && l->size == 1,
              "deep leaf MT6835.EEPROM.reg0x09.anon.zero_posi_11_4 @0x2000031D");
    }

    os_elf_close(elf);
    g_app.elf = NULL;

    printf(g_fails == 0 ? "ALL PASS\n" : "FAILURES: %d\n", g_fails);
    return g_fails == 0 ? 0 : 1;
}
