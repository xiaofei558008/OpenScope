/*
 * OpenScope ELF/.out dump tool: prints parsed global variables with full
 * type trees, then vartree-expanded leaves.
 *
 * Build: cl /nologo /W2 /utf-8 /I code\src tests\dump_elf.c ^
 *            code\src\elf.c code\src\vartree.c code\src\util.c ^
 *            /Fe:tests\bin\dump_elf.exe /link user32.lib
 * Run:   tests\bin\dump_elf.exe <file.elf|file.out>
 */
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "app.h"
#include "elf.h"
#include "vartree.h"

OS_App g_app;

static const char* kind_name(OS_TypeKind k)
{
    switch (k) {
    case OS_TYPE_VOID: return "void";
    case OS_TYPE_INT: return "int";
    case OS_TYPE_UINT: return "uint";
    case OS_TYPE_FLOAT: return "float";
    case OS_TYPE_BOOL: return "bool";
    case OS_TYPE_STRUCT: return "struct";
    case OS_TYPE_UNION: return "union";
    case OS_TYPE_ARRAY: return "array";
    case OS_TYPE_ENUM: return "enum";
    case OS_TYPE_PTR: return "ptr";
    case OS_TYPE_STRING: return "string";
    default: return "other";
    }
}

static void dump_type(const OS_Type* t, int indent)
{
    int i;
    char pad[64];
    if (!t) { printf("%*s(null)\n", indent, ""); return; }
    for (i = 0; i < indent && i < 60; i++) pad[i] = ' ';
    pad[i] = 0;
    printf("%s%s %s size=%u%s%s", pad, kind_name(t->kind), t->name ? t->name : "?",
           t->size, t->is_signed ? " signed" : "",
           t->is_bitfield ? " bitfield" : "");
    if (t->kind == OS_TYPE_ARRAY) printf(" count=%d", t->array_count);
    if (t->kind == OS_TYPE_ENUM && t->child_count == 0) printf(" enum");
    printf("\n");
    for (i = 0; i < t->child_count; i++) {
        if (t->children[i]) {
            printf("%s  [%d] ", pad, i);
            if (t->kind == OS_TYPE_STRUCT || t->kind == OS_TYPE_UNION) {
                printf("off=%lld ", (long long)t->children[i]->member_offset);
                if (t->children[i]->is_bitfield)
                    printf("bit=%u/%u ", t->children[i]->bit_offset,
                           t->children[i]->bit_size);
            } else if (t->kind == OS_TYPE_ENUM) {
                printf("val=%lld ", (long long)t->children[i]->enum_value);
            }
            dump_type(t->children[i], indent + 4);
        }
    }
}

int main(int argc, char** argv)
{
    char err[256] = "";
    OS_ElfFile* elf;
    int i;

    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc < 2) {
        printf("usage: dump_elf <file>\n");
        return 2;
    }
    memset(&g_app, 0, sizeof(g_app));

    elf = os_elf_open(argv[1], err, sizeof(err));
    if (!elf) {
        printf("FAIL open %s: %s\n", argv[1], err);
        return 1;
    }
    printf("ELF: %s machine=%u bits=%d entry=0x%llX vars=%d\n",
           argv[1], os_elf_machine(elf), os_elf_bits(elf),
           (unsigned long long)os_elf_entry(elf), os_elf_var_count(elf));

    for (i = 0; i < os_elf_var_count(elf); i++) {
        const OS_Variable* v = os_elf_var_at(elf, i);
        printf("--- var[%d] %s addr=0x%llX size=%llu debug=%d\n",
               i, v->name, (unsigned long long)v->address,
               (unsigned long long)v->symbol_size, v->has_debug);
        dump_type(v->type, 4);
    }

    g_app.elf = elf;
    os_vartree_build();
    printf("=== vartree leaves: %d ===\n", g_app.leaf_count);
    for (i = 0; i < g_app.leaf_count; i++) {
        printf("  %-40s addr=0x%llX size=%u kind=%s\n",
               g_app.leaves[i].name, (unsigned long long)g_app.leaves[i].address,
               g_app.leaves[i].size, kind_name(g_app.leaves[i].kind));
    }
    os_elf_close(elf);
    g_app.elf = NULL;
    return 0;
}
