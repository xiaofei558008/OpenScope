/*
 * OpenScope ELF/DWARF variable parser.
 *
 * Supports ELF32/ELF64 (little/big endian) with:
 *  - symbol table fallback (.symtab) for global objects;
 *  - DWARF v2-v5 .debug_info/.debug_abbrev/.debug_str/
 *    .debug_str_offsets/.debug_addr to recover variable types
 *    (base types, structs, unions, arrays, enums, pointers, typedefs)
 *    and absolute addresses (DW_OP_addr / DW_OP_addrx / GNU_addr_index).
 */

#include "elf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Allocation tracking (all memory is freed by os_elf_close)          */
/* ------------------------------------------------------------------ */

typedef struct OS_Alloc {
    void* p;
    struct OS_Alloc* next;
} OS_Alloc;

typedef struct DW_Unit DW_Unit;

struct OS_ElfFile {
    const uint8_t* data;
    size_t data_len;
    int is64;
    int le;                 /* 1 = little endian */
    uint16_t machine;
    uint32_t flags;
    uint64_t entry;

    /* section table */
    const uint8_t* shdr;
    uint64_t shoff;
    uint16_t shentsize;
    int shnum;
    uint32_t shstrndx;

    /* sections of interest */
    const uint8_t* shstr;   uint64_t shstr_size;
    const uint8_t* symtab;  uint64_t symtab_size;  uint64_t sym_entsize;
    const uint8_t* strtab;  uint64_t strtab_size;
    const uint8_t* dbg_info;          uint64_t dbg_info_size;
    const uint8_t* dbg_abbrev;        uint64_t dbg_abbrev_size;
    const uint8_t* dbg_str;           uint64_t dbg_str_size;
    const uint8_t* dbg_str_offsets;   uint64_t dbg_str_offsets_size;
    const uint8_t* dbg_addr;          uint64_t dbg_addr_size;

    OS_Variable* vars;
    int var_count;
    int var_cap;

    /* all parsed DWARF units (kept during parse, freed afterwards) */
    DW_Unit* units;
    int n_units, cap_units;
    uint64_t* die_glob_off;   /* sorted absolute DIE offsets */
    int* die_glob_unit;       /* owning unit index */
    int* die_glob_idx;        /* local die index in unit */
    int die_glob_count, die_glob_cap;

    OS_Alloc* allocs;
};

static void* os_alloc(OS_ElfFile* f, size_t n)
{
    OS_Alloc* a = (OS_Alloc*)calloc(1, sizeof(OS_Alloc));
    if (!a) return NULL;
    a->p = calloc(1, n ? n : 1);
    if (!a->p) { free(a); return NULL; }
    a->next = f->allocs;
    f->allocs = a;
    return a->p;
}

static char* os_strdup(OS_ElfFile* f, const char* s)
{
    size_t n = strlen(s) + 1;
    char* p = (char*)os_alloc(f, n);
    if (p) memcpy(p, s, n);
    return p;
}

/* ------------------------------------------------------------------ */
/* Byte readers                                                        */
/* ------------------------------------------------------------------ */

static uint16_t rd_u16(const uint8_t* p, int le)
{
    return le ? (uint16_t)(p[0] | (p[1] << 8))
              : (uint16_t)((p[0] << 8) | p[1]);
}

static uint32_t rd_u32(const uint8_t* p, int le)
{
    if (le) return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
                   ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static uint64_t rd_u64(const uint8_t* p, int le)
{
    uint64_t v = 0;
    int i;
    if (le) {
        for (i = 7; i >= 0; --i) v = (v << 8) | p[i];
    } else {
        for (i = 0; i < 8; ++i) v = (v << 8) | p[i];
    }
    return v;
}

static uint64_t rd_addr(const uint8_t* p, int le, int size)
{
    uint64_t v = 0;
    int i;
    if (le) {
        for (i = size - 1; i >= 0; --i) v = (v << 8) | p[i];
    } else {
        for (i = 0; i < size; ++i) v = (v << 8) | p[i];
    }
    return v;
}

static uint64_t rd_uleb(const uint8_t* p, const uint8_t* end, const uint8_t** out)
{
    uint64_t r = 0;
    int shift = 0;
    while (p < end && shift < 64) {
        uint8_t b = *p++;
        r |= (uint64_t)(b & 0x7f) << shift;
        if (!(b & 0x80)) break;
        shift += 7;
    }
    if (out) *out = p;
    return r;
}

static int64_t rd_sleb(const uint8_t* p, const uint8_t* end, const uint8_t** out)
{
    uint64_t r = 0;
    int shift = 0;
    uint8_t b;
    do {
        if (p >= end) break;
        b = *p++;
        r |= (uint64_t)(b & 0x7f) << shift;
        shift += 7;
    } while (b & 0x80);
    if (shift < 64 && (b & 0x40)) r |= (~(uint64_t)0) << shift;
    if (out) *out = p;
    return (int64_t)r;
}

static const char* rd_cstr(const uint8_t* p, const uint8_t* end)
{
    const uint8_t* q = p;
    while (q < end && *q) ++q;
    return (const char*)p;
}

/* ------------------------------------------------------------------ */
/* ELF header/section parsing                                          */
/* ------------------------------------------------------------------ */

#define SHN_UNDEF 0
#define SHT_SYMTAB 2
#define STT_OBJECT 1
#define STB_GLOBAL 1
#define STB_WEAK   2

static int elf_parse_header(OS_ElfFile* f, char* errbuf, int errbuf_len)
{
    const uint8_t* d = f->data;
    if (f->data_len < 16 || memcmp(d, "\177ELF", 4) != 0) {
        snprintf(errbuf, errbuf_len, "not an ELF file");
        return -1;
    }
    if (d[4] != 1 && d[4] != 2) {
        snprintf(errbuf, errbuf_len, "unsupported ELF class %d", d[4]);
        return -1;
    }
    f->is64 = (d[4] == 2);
    f->le = (d[5] == 1);
    if (d[6] != 1) {
        snprintf(errbuf, errbuf_len, "unsupported ELF version");
        return -1;
    }
    if (f->is64) {
        if (f->data_len < 64) { snprintf(errbuf, errbuf_len, "truncated ELF64 header"); return -1; }
        f->machine = rd_u16(d + 18, f->le);
        f->flags   = rd_u32(d + 48, f->le);
        f->entry   = rd_u64(d + 24, f->le);
        f->shoff   = rd_u64(d + 40, f->le);
        f->shentsize = rd_u16(d + 58, f->le);
        f->shnum    = rd_u16(d + 60, f->le);
        f->shstrndx = rd_u16(d + 62, f->le);
    } else {
        if (f->data_len < 52) { snprintf(errbuf, errbuf_len, "truncated ELF32 header"); return -1; }
        f->machine = rd_u16(d + 18, f->le);
        f->flags   = rd_u32(d + 36, f->le);
        f->entry   = rd_u32(d + 24, f->le);
        f->shoff   = rd_u32(d + 32, f->le);
        f->shentsize = rd_u16(d + 46, f->le);
        f->shnum    = rd_u16(d + 48, f->le);
        f->shstrndx = rd_u16(d + 50, f->le);
    }
    if (f->shnum == 0) f->shnum = 0x10000; /* extended count in sh_size of section 0 */
    return 0;
}

static const uint8_t* elf_section(OS_ElfFile* f, int idx, uint64_t* out_size)
{
    const uint8_t* sh;
    uint64_t off, size;
    if (idx < 0 || idx >= f->shnum) return NULL;
    sh = f->shdr + (uint64_t)idx * f->shentsize;
    if (f->is64) {
        off  = rd_u64(sh + 24, f->le);
        size = rd_u64(sh + 32, f->le);
    } else {
        off  = rd_u32(sh + 16, f->le);
        size = rd_u32(sh + 20, f->le);
    }
    if (off > f->data_len || size > f->data_len - off) return NULL;
    if (out_size) *out_size = size;
    return f->data + off;
}

static const char* elf_section_name(OS_ElfFile* f, int idx)
{
    const uint8_t* sh;
    uint32_t name_off;
    if (idx < 0 || idx >= f->shnum) return NULL;
    sh = f->shdr + (uint64_t)idx * f->shentsize;
    name_off = rd_u32(sh, f->le);
    if (name_off >= f->shstr_size) return NULL;
    return (const char*)(f->shstr + name_off);
}

static const uint8_t* elf_find_section(OS_ElfFile* f, const char* want, uint64_t* out_size, int* out_idx)
{
    int i;
    for (i = 0; i < f->shnum; ++i) {
        const char* nm = elf_section_name(f, i);
        if (nm && strcmp(nm, want) == 0) {
            if (out_idx) *out_idx = i;
            return elf_section(f, i, out_size);
        }
    }
    return NULL;
}

static int elf_parse_sections(OS_ElfFile* f, char* errbuf, int errbuf_len)
{
    uint64_t shstr_size = 0;
    if (f->shoff >= f->data_len) { snprintf(errbuf, errbuf_len, "bad section table offset"); return -1; }
    if ((uint64_t)f->shnum * f->shentsize > f->data_len - f->shoff) {
        snprintf(errbuf, errbuf_len, "bad section table size");
        return -1;
    }
    f->shdr = f->data + f->shoff;
    if (f->shstrndx == 0xffff) {
        /* extended shstrndx in sh_link of section 0 */
        if (f->shentsize < 40) { snprintf(errbuf, errbuf_len, "bad section table"); return -1; }
        f->shstrndx = (uint32_t)rd_u32(f->shdr + 24, f->le);
    }
    f->shstr = elf_section(f, (int)f->shstrndx, &f->shstr_size);
    if (!f->shstr) { snprintf(errbuf, errbuf_len, "missing .shstrtab"); return -1; }
    (void)shstr_size;

    f->symtab = elf_find_section(f, ".symtab", &f->symtab_size, NULL);
    if (f->symtab) {
        const uint8_t* sh = NULL;
        int idx = 0;
        elf_find_section(f, ".symtab", &f->symtab_size, &idx);
        sh = f->shdr + (uint64_t)idx * f->shentsize;
        f->sym_entsize = f->is64 ? rd_u64(sh + 56, f->le) : rd_u32(sh + 36, f->le);
        if (!f->sym_entsize) f->sym_entsize = f->is64 ? 24 : 16;
        f->strtab = elf_section(f, f->is64 ? rd_u32(sh + 40, f->le)
                                           : rd_u32(sh + 24, f->le),
                                &f->strtab_size);
    }
    elf_find_section(f, ".debug_info", &f->dbg_info_size, NULL);
    f->dbg_info = elf_find_section(f, ".debug_info", &f->dbg_info_size, NULL);
    elf_find_section(f, ".debug_abbrev", &f->dbg_abbrev_size, NULL);
    f->dbg_abbrev = elf_find_section(f, ".debug_abbrev", &f->dbg_abbrev_size, NULL);
    elf_find_section(f, ".debug_str", &f->dbg_str_size, NULL);
    f->dbg_str = elf_find_section(f, ".debug_str", &f->dbg_str_size, NULL);
    elf_find_section(f, ".debug_str_offsets", &f->dbg_str_offsets_size, NULL);
    f->dbg_str_offsets = elf_find_section(f, ".debug_str_offsets", &f->dbg_str_offsets_size, NULL);
    elf_find_section(f, ".debug_addr", &f->dbg_addr_size, NULL);
    f->dbg_addr = elf_find_section(f, ".debug_addr", &f->dbg_addr_size, NULL);
    return 0;
}

/* ------------------------------------------------------------------ */
/* DWARF                                                               */
/* ------------------------------------------------------------------ */

#define DW_TAG_array_type         0x01
#define DW_TAG_class_type         0x02
#define DW_TAG_enumeration_type   0x04
#define DW_TAG_member             0x0d
#define DW_TAG_pointer_type       0x0f
#define DW_TAG_reference_type     0x10
#define DW_TAG_compile_unit       0x11
#define DW_TAG_structure_type     0x13
#define DW_TAG_subroutine_type    0x15
#define DW_TAG_typedef            0x16
#define DW_TAG_union_type         0x17
#define DW_TAG_string_type        0x18
#define DW_TAG_ptr_to_member_type 0x1f
#define DW_TAG_subrange_type      0x21
#define DW_TAG_base_type          0x24
#define DW_TAG_const_type         0x26
#define DW_TAG_enumerator         0x28
#define DW_TAG_subprogram         0x2e
#define DW_TAG_variable           0x34
#define DW_TAG_volatile_type      0x35
#define DW_TAG_restrict_type      0x37
#define DW_TAG_unspecified_type   0x3e
#define DW_TAG_rvalue_reference_type 0x42

#define DW_AT_location             0x02
#define DW_AT_name                 0x03
#define DW_AT_byte_size            0x0b
#define DW_AT_bit_offset           0x0c
#define DW_AT_bit_size             0x0d
#define DW_AT_low_pc               0x11
#define DW_AT_const_value          0x1c
#define DW_AT_lower_bound          0x22
#define DW_AT_upper_bound          0x2f
#define DW_AT_abstract_origin      0x31
#define DW_AT_artificial           0x34
#define DW_AT_count                0x37
#define DW_AT_data_member_location 0x38
#define DW_AT_declaration          0x3c
#define DW_AT_encoding             0x3e
#define DW_AT_external             0x3f
#define DW_AT_specification        0x47
#define DW_AT_type                 0x49
#define DW_AT_data_bit_offset      0x6b
#define DW_AT_linkage_name         0x6e
#define DW_AT_str_offsets_base     0x72
#define DW_AT_addr_base            0x73
#define DW_AT_MIPS_linkage_name    0x2007

#define DW_FORM_addr         0x01
#define DW_FORM_block2       0x03
#define DW_FORM_block4       0x04
#define DW_FORM_data2        0x05
#define DW_FORM_data4        0x06
#define DW_FORM_data8        0x07
#define DW_FORM_string       0x08
#define DW_FORM_block        0x09
#define DW_FORM_block1       0x0a
#define DW_FORM_data1        0x0b
#define DW_FORM_flag         0x0c
#define DW_FORM_sdata        0x0d
#define DW_FORM_strp         0x0e
#define DW_FORM_udata        0x0f
#define DW_FORM_ref_addr     0x10
#define DW_FORM_ref1         0x11
#define DW_FORM_ref2         0x12
#define DW_FORM_ref4         0x13
#define DW_FORM_ref8         0x14
#define DW_FORM_ref_udata    0x15
#define DW_FORM_indirect     0x16
#define DW_FORM_sec_offset   0x17
#define DW_FORM_exprloc      0x18
#define DW_FORM_flag_present 0x19
#define DW_FORM_strx         0x1a
#define DW_FORM_addrx        0x1b
#define DW_FORM_data16       0x1e
#define DW_FORM_line_strp    0x1f
#define DW_FORM_ref_sig8     0x20
#define DW_FORM_implicit_const 0x21
#define DW_FORM_loclistx     0x22
#define DW_FORM_rnglistx     0x23
#define DW_FORM_ref_sup4     0x1c
#define DW_FORM_ref_sup8     0x1d
#define DW_FORM_strx1        0x25
#define DW_FORM_strx2        0x26
#define DW_FORM_strx3        0x27
#define DW_FORM_strx4        0x28
#define DW_FORM_addrx1       0x29
#define DW_FORM_addrx2       0x2a
#define DW_FORM_addrx3       0x2b
#define DW_FORM_addrx4       0x2c

#define DW_ATE_boolean       0x02
#define DW_ATE_float         0x04
#define DW_ATE_signed        0x05
#define DW_ATE_signed_char   0x06
#define DW_ATE_unsigned      0x07
#define DW_ATE_unsigned_char 0x08
#define DW_ATE_signed_fixed  0x0d
#define DW_ATE_unsigned_fixed 0x0e
#define DW_ATE_UTF           0x10

#define DW_OP_addr           0x03
#define DW_OP_constu         0x10
#define DW_OP_plus_uconst    0x23
#define DW_OP_stack_value    0x9f
#define DW_OP_addrx          0xa1
#define DW_OP_GNU_addr_index 0xfb

typedef struct DW_Attr {
    uint64_t name;
    uint8_t  kind;     /* 0 none, 1 const, 2 uleb, 3 sleb, 4 string, 5 strp, 6 strx, 7 addrx, 8 expr */
    uint64_t v;        /* const / offset / index / expr length */
    const uint8_t* p;  /* expr data / string pointer */
} DW_Attr;

typedef struct DW_Die {
    uint64_t off;
    uint64_t tag;
    int      has_children;
    int      parent;
    int      n_child;
    DW_Attr* attrs;
    int      nattrs;
    int      capattrs;
} DW_Die;

typedef struct DW_Abbrev {
    uint64_t code;
    uint64_t tag;
    int      has_children;
    uint64_t* anames;
    uint64_t* aforms;
    int      nattrs;
    int64_t*  implicit_vals; /* for DW_FORM_implicit_const */
} DW_Abbrev;

typedef struct DW_Unit {
    OS_ElfFile* f;
    uint64_t unit_off;    /* offset of unit_length */
    uint64_t start;       /* first DIE offset */
    uint64_t end;
    uint16_t version;
    uint8_t  unit_type;   /* DWARF5: DW_UT_* */
    uint8_t  addr_size;
    uint64_t abbrev_off;  /* section offset */
    uint64_t str_offsets_base;
    uint64_t addr_base;
    DW_Die* dies;
    int      ndies, capdies;
    uint64_t* die_offs;   /* sorted offsets for lookup */
    int*      die_index;
    OS_Type** type_map;   /* by die index */
} DW_Unit;

/* ------------------------ abbrev table ---------------------------- */

static int abbrev_parse(DW_Unit* u, DW_Abbrev** out, int* out_n)
{
    OS_ElfFile* f = u->f;
    const uint8_t* p = f->dbg_abbrev + u->abbrev_off;
    const uint8_t* end = f->dbg_abbrev + f->dbg_abbrev_size;
    DW_Abbrev* items = NULL;
    int n = 0, cap = 0;

    while (p < end) {
        uint64_t code = rd_uleb(p, end, &p);
        if (code == 0) break;
        if (n == cap) {
            cap = cap ? cap * 2 : 64;
            items = (DW_Abbrev*)realloc(items, cap * sizeof(DW_Abbrev));
            if (!items) return -1;
            memset(items + n, 0, (cap - n) * sizeof(DW_Abbrev));
        }
        items[n].code = code;
        items[n].tag = rd_uleb(p, end, &p);
        items[n].has_children = (p < end && *p == 1) ? 1 : 0;
        ++p;
        {
            int capa = 0;
            while (p < end) {
                uint64_t an = rd_uleb(p, end, &p);
                uint64_t af = rd_uleb(p, end, &p);
                if (an == 0 && af == 0) break;
                if (items[n].nattrs == capa) {
                    int nc = capa ? capa * 2 : 8;
                    uint64_t* na = (uint64_t*)realloc(items[n].anames, nc * sizeof(uint64_t));
                    uint64_t* nf = (uint64_t*)realloc(items[n].aforms, nc * sizeof(uint64_t));
                    int64_t*  nv = (int64_t*)realloc(items[n].implicit_vals, nc * sizeof(int64_t));
                    if (!na || !nf || !nv) return -1;
                    items[n].anames = na;
                    items[n].aforms = nf;
                    items[n].implicit_vals = nv;
                    capa = nc;
                }
                items[n].anames[items[n].nattrs] = an;
                items[n].aforms[items[n].nattrs] = af;
                items[n].implicit_vals[items[n].nattrs] = 0;
                if (af == DW_FORM_implicit_const) {
                    items[n].implicit_vals[items[n].nattrs] = rd_sleb(p, end, &p);
                }
                items[n].nattrs++;
            }
        }
        ++n;
    }
    *out = items;
    *out_n = n;
    return 0;
}

static const DW_Abbrev* abbrev_find(DW_Abbrev* items, int n, uint64_t code)
{
    int i;
    for (i = 0; i < n; ++i)
        if (items[i].code == code) return &items[i];
    return NULL;
}

/* ------------------------ DIE record helpers ---------------------- */

static int die_add_attr(DW_Die* d, uint64_t name, uint8_t kind, uint64_t v, const uint8_t* p)
{
    if (d->nattrs == d->capattrs) {
        int nc = d->capattrs ? d->capattrs * 2 : 16;
        DW_Attr* na = (DW_Attr*)realloc(d->attrs, nc * sizeof(DW_Attr));
        if (!na) return -1;
        d->attrs = na;
        d->capattrs = nc;
    }
    d->attrs[d->nattrs].name = name;
    d->attrs[d->nattrs].kind = kind;
    d->attrs[d->nattrs].v = v;
    d->attrs[d->nattrs].p = p;
    ++d->nattrs;
    return 0;
}

static DW_Die* unit_die(DW_Unit* u, int idx)
{
    return &u->dies[idx];
}

static DW_Die* die_glob_find(OS_ElfFile* f, uint64_t off, DW_Unit** out_u, int* out_idx)
{
    int lo = 0, hi = f->die_glob_count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (f->die_glob_off[mid] == off) {
            int ui = f->die_glob_unit[mid];
            int di = f->die_glob_idx[mid];
            if (out_u) *out_u = &f->units[ui];
            if (out_idx) *out_idx = di;
            return &f->units[ui].dies[di];
        }
        if (f->die_glob_off[mid] < off) lo = mid + 1;
        else hi = mid - 1;
    }
    return NULL;
}

/* ------------------------ DIE walker ------------------------------ */

static int parse_dies(DW_Unit* u, uint64_t off, int parent, DW_Abbrev* abbrevs, int nabbrevs,
                      uint64_t* next_off);

static int parse_one_die(DW_Unit* u, uint64_t off, int parent, DW_Abbrev* abbrevs, int nabbrevs,
                         uint64_t* next_off)
{
    OS_ElfFile* f = u->f;
    const uint8_t* p = f->dbg_info + off;
    const uint8_t* end = f->dbg_info + u->end;
    const uint8_t* after_code;
    uint64_t code, tag;
    const DW_Abbrev* ab;
    DW_Die* d;
    int idx;
    int i;

    if (p >= end) return -1;
    code = rd_uleb(p, end, &after_code);
    if (code == 0) {
        *next_off = off + (uint64_t)(after_code - p);
        return 0;
    }
    ab = abbrev_find(abbrevs, nabbrevs, code);
    if (!ab) return -1;
    tag = ab->tag;

    if (u->ndies == u->capdies) {
        int nc = u->capdies ? u->capdies * 2 : 256;
        DW_Die* nd = (DW_Die*)realloc(u->dies, nc * sizeof(DW_Die));
        uint64_t* no = (uint64_t*)realloc(u->die_offs, nc * sizeof(uint64_t));
        int* ni = (int*)realloc(u->die_index, nc * sizeof(int));
        if (!nd || !no || !ni) return -1;
        u->dies = nd;
        u->die_offs = no;
        u->die_index = ni;
        u->capdies = nc;
    }
    idx = u->ndies++;
    d = &u->dies[idx];
    memset(d, 0, sizeof(*d));
    d->off = off;
    d->tag = tag;
    d->has_children = ab->has_children;
    d->parent = parent;
    u->die_offs[idx] = off;
    u->die_index[idx] = idx;

    p = after_code;
    for (i = 0; i < ab->nattrs; ++i) {
        uint64_t an = ab->anames[i];
        uint64_t af = ab->aforms[i];
        if (af == DW_FORM_indirect) {
            af = rd_uleb(p, end, &p);
        }
        if (af == DW_FORM_implicit_const) {
            die_add_attr(d, an, 3, (uint64_t)ab->implicit_vals[i], NULL);
            continue;
        }
        switch (af) {
        case DW_FORM_addr:
            if (p + u->addr_size > end) return -1;
            die_add_attr(d, an, 9, rd_addr(p, f->le, (int)u->addr_size), NULL);
            p += u->addr_size;
            break;
        case DW_FORM_data1:
        case DW_FORM_flag:
            if (p >= end) return -1;
            die_add_attr(d, an, 1, *p, NULL);
            ++p;
            break;
        case DW_FORM_ref1:
            if (p >= end) return -1;
            die_add_attr(d, an, 1, u->unit_off + *p, NULL);
            ++p;
            break;
        case DW_FORM_data2:
            if (p + 2 > end) return -1;
            die_add_attr(d, an, 1, rd_u16(p, f->le), NULL);
            p += 2;
            break;
        case DW_FORM_ref2:
            if (p + 2 > end) return -1;
            die_add_attr(d, an, 1, u->unit_off + rd_u16(p, f->le), NULL);
            p += 2;
            break;
        case DW_FORM_data4:
        case DW_FORM_sec_offset:
        case DW_FORM_strp:
            if (p + 4 > end) return -1;
            die_add_attr(d, an, (af == DW_FORM_strp) ? 5 : 1, rd_u32(p, f->le), NULL);
            p += 4;
            break;
        case DW_FORM_ref4:
            if (p + 4 > end) return -1;
            die_add_attr(d, an, 1, u->unit_off + rd_u32(p, f->le), NULL);
            p += 4;
            break;
        case DW_FORM_data8:
            if (p + 8 > end) return -1;
            die_add_attr(d, an, 1, rd_u64(p, f->le), NULL);
            p += 8;
            break;
        case DW_FORM_ref8:
            if (p + 8 > end) return -1;
            die_add_attr(d, an, 1, u->unit_off + rd_u64(p, f->le), NULL);
            p += 8;
            break;
        case DW_FORM_ref_sig8:
            if (p + 8 > end) return -1;
            die_add_attr(d, an, 1, rd_u64(p, f->le), NULL);
            p += 8;
            break;
        case DW_FORM_data16:
            if (p + 16 > end) return -1;
            p += 16;
            break;
        case DW_FORM_ref_sup4:
            if (p + 4 > end) return -1;
            p += 4;
            break;
        case DW_FORM_ref_sup8:
            if (p + 8 > end) return -1;
            p += 8;
            break;
        case DW_FORM_sdata:
            die_add_attr(d, an, 3, (uint64_t)rd_sleb(p, end, &p), NULL);
            break;
        case DW_FORM_udata:
            die_add_attr(d, an, 2, rd_uleb(p, end, &p), NULL);
            break;
        case DW_FORM_ref_udata:
            die_add_attr(d, an, 1, u->unit_off + rd_uleb(p, end, &p), NULL);
            break;
        case DW_FORM_string:
            if (p >= end) return -1;
            die_add_attr(d, an, 4, 0, p);
            p += strlen((const char*)p) + 1;
            break;
        case DW_FORM_strx:
            die_add_attr(d, an, 6, rd_uleb(p, end, &p), NULL);
            break;
        case DW_FORM_strx1:
            if (p >= end) return -1;
            die_add_attr(d, an, 6, *p, NULL); ++p;
            break;
        case DW_FORM_strx2:
            if (p + 2 > end) return -1;
            die_add_attr(d, an, 6, rd_u16(p, f->le), NULL); p += 2;
            break;
        case DW_FORM_strx3:
            if (p + 3 > end) return -1;
            die_add_attr(d, an, 6, (uint64_t)p[0] | ((uint64_t)p[1] << 8) | ((uint64_t)p[2] << 16), NULL);
            p += 3;
            break;
        case DW_FORM_strx4:
            if (p + 4 > end) return -1;
            die_add_attr(d, an, 6, rd_u32(p, f->le), NULL); p += 4;
            break;
        case DW_FORM_addrx:
            die_add_attr(d, an, 7, rd_uleb(p, end, &p), NULL);
            break;
        case DW_FORM_addrx1:
            if (p >= end) return -1;
            die_add_attr(d, an, 7, *p, NULL); ++p;
            break;
        case DW_FORM_addrx2:
            if (p + 2 > end) return -1;
            die_add_attr(d, an, 7, rd_u16(p, f->le), NULL); p += 2;
            break;
        case DW_FORM_addrx3:
            if (p + 3 > end) return -1;
            die_add_attr(d, an, 7, (uint64_t)p[0] | ((uint64_t)p[1] << 8) | ((uint64_t)p[2] << 16), NULL);
            p += 3;
            break;
        case DW_FORM_addrx4:
            if (p + 4 > end) return -1;
            die_add_attr(d, an, 7, rd_u32(p, f->le), NULL); p += 4;
            break;
        case DW_FORM_exprloc: {
            uint64_t len = rd_uleb(p, end, &p);
            if (p + len > end) return -1;
            die_add_attr(d, an, 8, len, p);
            p += len;
            break;
        }
        case DW_FORM_block1:
            if (p >= end) return -1;
            p += 1 + *p;
            break;
        case DW_FORM_block2:
            if (p + 2 > end) return -1;
            p += 2 + rd_u16(p, f->le);
            break;
        case DW_FORM_block4:
            if (p + 4 > end) return -1;
            p += 4 + rd_u32(p, f->le);
            break;
        case DW_FORM_block:
        case DW_FORM_loclistx:
        case DW_FORM_rnglistx:
            rd_uleb(p, end, &p);
            break;
        case DW_FORM_flag_present:
            die_add_attr(d, an, 1, 1, NULL);
            break;
        case DW_FORM_ref_addr:
            if (u->version <= 2) {
                if (p + u->addr_size > end) return -1;
                die_add_attr(d, an, 1, rd_addr(p, f->le, (int)u->addr_size), NULL);
                p += u->addr_size;
            } else {
                if (p + 4 > end) return -1;
                die_add_attr(d, an, 1, rd_u32(p, f->le), NULL);
                p += 4;
            }
            break;
        case DW_FORM_line_strp:
            if (p + 4 > end) return -1;
            p += 4;
            break;
        default:
            return -1;
        }
    }

    if (d->has_children) {
        uint64_t child = 0;
        if (parse_dies(u, (uint64_t)(p - f->dbg_info), idx, abbrevs, nabbrevs, &child) != 0)
            return -1;
        d->n_child = u->ndies - idx - 1;
        *next_off = child;
    } else {
        *next_off = (uint64_t)(p - f->dbg_info);
    }
    return 0;
}

static int parse_dies(DW_Unit* u, uint64_t off, int parent, DW_Abbrev* abbrevs, int nabbrevs,
                      uint64_t* next_off)
{
    uint64_t cur = off;
    while (cur < u->end) {
        uint64_t after = 0;
        const uint8_t* cp = u->f->dbg_info + cur;
        if (cp >= u->f->dbg_info + u->end) break;
        if (*cp == 0) { /* null DIE terminates the children list */
            cur += 1;
            break;
        }
        if (parse_one_die(u, cur, parent, abbrevs, nabbrevs, &after) != 0)
            return -1;
        if (after <= cur) return -1;
        cur = after;
    }
    *next_off = cur;
    return 0;
}

/* ------------------------ unit parsing ---------------------------- */

static int parse_unit(OS_ElfFile* f, uint64_t off, DW_Unit* u)
{
    const uint8_t* p = f->dbg_info + off;
    const uint8_t* end = f->dbg_info + f->dbg_info_size;
    uint64_t len, start;
    DW_Abbrev* abbrevs = NULL;
    int nabbrevs = 0;
    int i;

    memset(u, 0, sizeof(*u));
    u->f = f;
    u->unit_off = off;
    if (p + 4 > end) return -1;
    len = rd_u32(p, f->le);
    if (len == 0xffffffff) return -1; /* DWARF64 not supported */
    if (len == 0 || p + 4 + len > end) return -1;
    u->end = off + 4 + len;
    p += 4;
    u->version = rd_u16(p, f->le);
    p += 2;
    if (u->version >= 5) {
        u->unit_type = *p++;
        u->addr_size = *p++;
        u->abbrev_off = rd_u32(p, f->le);
        p += 4;
    } else {
        u->addr_size = *(p + 4);
        u->abbrev_off = rd_u32(p, f->le);
        p += 5;
    }
    if (u->addr_size == 0 || u->addr_size > 8) return -1;
    if (u->abbrev_off >= f->dbg_abbrev_size) return -1;
    if (abbrev_parse(u, &abbrevs, &nabbrevs) != 0 || nabbrevs == 0) {
        if (abbrevs) free(abbrevs);
        return -1;
    }
    u->start = (uint64_t)(p - f->dbg_info);
    {
        uint64_t dummy = 0;
        if (parse_dies(u, u->start, -1, abbrevs, nabbrevs, &dummy) != 0) {
            for (i = 0; i < nabbrevs; ++i) {
                free(abbrevs[i].anames);
                free(abbrevs[i].aforms);
                free(abbrevs[i].implicit_vals);
            }
            free(abbrevs);
            return -1;
        }
    }
    for (i = 0; i < nabbrevs; ++i) {
        free(abbrevs[i].anames);
        free(abbrevs[i].aforms);
        free(abbrevs[i].implicit_vals);
    }
    free(abbrevs);

    /* CU attributes: str_offsets_base / addr_base */
    if (u->ndies > 0) {
        DW_Die* cu = &u->dies[0];
        for (i = 0; i < cu->nattrs; ++i) {
            DW_Attr* a = &cu->attrs[i];
            if (a->name == DW_AT_str_offsets_base && a->kind == 1)
                u->str_offsets_base = a->v;
            else if (a->name == DW_AT_addr_base && a->kind == 1)
                u->addr_base = a->v;
        }
    }
    if (u->version >= 5 && u->str_offsets_base == 0) u->str_offsets_base = 8;
    u->type_map = (OS_Type**)calloc(u->ndies ? u->ndies : 1, sizeof(OS_Type*));
    if (!u->type_map) return -1;
    start = u->unit_off;
    (void)start;
    return 0;
}

/* ------------------------ attribute resolution -------------------- */

static const char* unit_str(DW_Unit* u, DW_Attr* a)
{
    OS_ElfFile* f = u->f;
    switch (a->kind) {
    case 4: return (const char*)a->p;
    case 5:
        if (a->v < f->dbg_str_size) return (const char*)(f->dbg_str + a->v);
        break;
    case 6: {
        uint64_t idx = a->v;
        uint64_t base = u->str_offsets_base;
        uint64_t off;
        if (u->version >= 5 && f->dbg_str_offsets && base < f->dbg_str_offsets_size) {
            uint64_t pos = base + idx * 4;
            if (pos + 4 <= f->dbg_str_offsets_size) {
                off = rd_u32(f->dbg_str_offsets + pos, f->le);
                if (off < f->dbg_str_size) return (const char*)(f->dbg_str + off);
            }
        }
        break;
    }
    default:
        break;
    }
    return NULL;
}

static uint64_t unit_addr(DW_Unit* u, DW_Attr* a)
{
    OS_ElfFile* f = u->f;
    uint64_t idx;
    switch (a->kind) {
    case 7: /* addrx index into .debug_addr */
        idx = a->v;
        {
            uint64_t pos = u->addr_base + idx * u->addr_size;
            if (pos + u->addr_size <= f->dbg_addr_size)
                return rd_addr(f->dbg_addr + pos, f->le, (int)u->addr_size);
        }
        return 0;
    case 9: /* raw absolute address */
        return a->v;
    default:
        return 0;
    }
}

static uint64_t expr_addr(DW_Unit* u, const uint8_t* p, uint64_t len)
{
    OS_ElfFile* f = u->f;
    const uint8_t* end = p + len;
    while (p < end) {
        uint8_t op = *p++;
        switch (op) {
        case DW_OP_addr:
            if (p + u->addr_size <= end) return rd_addr(p, f->le, (int)u->addr_size);
            return 0;
        case DW_OP_addrx:
        case DW_OP_GNU_addr_index: {
            const uint8_t* q = NULL;
            uint64_t idx = rd_uleb(p, end, &q);
            uint64_t pos = u->addr_base + idx * u->addr_size;
            if (pos + u->addr_size <= f->dbg_addr_size)
                return rd_addr(f->dbg_addr + pos, f->le, (int)u->addr_size);
            return 0;
        }
        default:
            return 0;
        }
    }
    return 0;
}

static uint64_t const_u64(DW_Attr* a)
{
    if (a->kind == 1 || a->kind == 2 || a->kind == 3 || a->kind == 7) return a->v;
    return 0;
}

/* ------------------------ type building --------------------------- */

static OS_Type* build_type(DW_Unit* u, int die_idx, int depth);

static OS_Type* type_new(DW_Unit* u, OS_TypeKind kind, const char* name)
{
    OS_ElfFile* f = u->f;
    OS_Type* t = (OS_Type*)os_alloc(f, sizeof(OS_Type));
    if (!t) return NULL;
    t->kind = kind;
    t->name = name ? os_strdup(f, name) : NULL;
    return t;
}

static void type_set_child(DW_Unit* u, OS_Type* t, OS_Type* child)
{
    if (!child) return;
    t->children = (OS_Type**)os_alloc(u->f, sizeof(OS_Type*));
    if (!t->children) return;
    t->children[0] = child;
    t->child_count = 1;
}

static OS_Type* shallow_clone(DW_Unit* u, OS_Type* src, const char* name, int64_t off,
                              int bit_size, int bit_offset, int has_bit)
{
    OS_Type* t;
    if (!src) return NULL;
    t = type_new(u, src->kind, name);
    if (!t) return NULL;
    t->size = src->size;
    t->is_signed = src->is_signed;
    t->is_ptr = src->is_ptr;
    t->member_offset = off;
    t->children = src->children;
    t->child_count = src->child_count;
    if (has_bit) {
        t->is_bitfield = 1;
        t->bit_offset = (uint8_t)bit_offset;
        t->bit_size = (uint8_t)bit_size;
    }
    return t;
}

static OS_Type* build_type(DW_Unit* u, int die_idx, int depth)
{
    OS_ElfFile* f = u->f;
    DW_Die* d;
    OS_Type* t;
    int i;
    uint64_t byte_size = 0, encoding = 0, type_ref = 0;
    const char* name = NULL;
    int have_size = 0, have_enc = 0, have_type = 0;
    int is_ptr = 0;
    OS_TypeKind kind = OS_TYPE_OTHER;
    const char* tagname = NULL;
    int is_signed = 0;

    if (die_idx < 0 || die_idx >= u->ndies || depth > 40) return NULL;
    if (u->type_map[die_idx]) return u->type_map[die_idx];
    d = &u->dies[die_idx];

    for (i = 0; i < d->nattrs; ++i) {
        DW_Attr* a = &d->attrs[i];
        if (a->name == DW_AT_name) name = unit_str(u, a);
        else if (a->name == DW_AT_byte_size) { byte_size = const_u64(a); have_size = 1; }
        else if (a->name == DW_AT_encoding) { encoding = const_u64(a); have_enc = 1; }
        else if (a->name == DW_AT_type) { type_ref = const_u64(a); have_type = 1; }
    }

    switch (d->tag) {
    case DW_TAG_base_type:
        if (have_enc) {
            switch (encoding) {
            case DW_ATE_boolean: kind = OS_TYPE_BOOL; break;
            case DW_ATE_float:   kind = OS_TYPE_FLOAT; break;
            case DW_ATE_signed: case DW_ATE_signed_char:
            case DW_ATE_signed_fixed: kind = OS_TYPE_INT; is_signed = 1; break;
            case DW_ATE_unsigned: case DW_ATE_unsigned_char:
            case DW_ATE_unsigned_fixed: kind = OS_TYPE_UINT; break;
            case DW_ATE_UTF: kind = OS_TYPE_INT; break;
            default: kind = OS_TYPE_OTHER; break;
            }
        } else {
            kind = OS_TYPE_OTHER;
        }
        t = type_new(u, kind, name ? name : "int");
        if (t) t->size = have_size ? (uint32_t)byte_size : 4;
        t->is_signed = is_signed;
        break;
    case DW_TAG_typedef: {
        const char* own = name;
        if (have_type) {
            DW_Unit* tu = NULL;
            int tidx = -1;
            OS_Type* child = NULL;
            if (die_glob_find(f, type_ref, &tu, &tidx))
                child = build_type(tu, tidx, depth + 1);
            if (child) {
                if (child->kind == OS_TYPE_INT || child->kind == OS_TYPE_UINT ||
                    child->kind == OS_TYPE_FLOAT || child->kind == OS_TYPE_BOOL ||
                    child->kind == OS_TYPE_ENUM || child->kind == OS_TYPE_PTR) {
                    t = type_new(u, child->kind, own ? own : child->name);
                    if (!t) return NULL;
                    t->size = child->size;
                    t->is_signed = child->is_signed;
                    t->is_ptr = child->is_ptr;
                    type_set_child(u, t, child);
                } else {
                    t = child;
                    if (own && !t->name) t->name = os_strdup(f, own);
                }
            } else {
                t = type_new(u, OS_TYPE_OTHER, own ? own : "?");
                if (t) t->size = have_size ? (uint32_t)byte_size : 0;
            }
        } else {
            t = type_new(u, OS_TYPE_OTHER, own ? own : "?");
            if (t) t->size = have_size ? (uint32_t)byte_size : 0;
        }
        break;
    }
    case DW_TAG_const_type:
    case DW_TAG_volatile_type:
    case DW_TAG_restrict_type:
        if (have_type) {
            DW_Unit* tu = NULL;
            int tidx = -1;
            t = NULL;
            if (die_glob_find(f, type_ref, &tu, &tidx))
                t = build_type(tu, tidx, depth + 1);
            if (!t) { t = type_new(u, OS_TYPE_OTHER, name ? name : "?"); }
        } else {
            t = type_new(u, OS_TYPE_OTHER, name ? name : "?");
        }
        break;
    case DW_TAG_pointer_type:
    case DW_TAG_reference_type:
    case DW_TAG_rvalue_reference_type:
    case DW_TAG_ptr_to_member_type:
        t = type_new(u, OS_TYPE_PTR, name ? name : "void*");
        if (!t) return NULL;
        t->size = have_size ? (uint32_t)byte_size : (uint32_t)(f->is64 ? 8 : 4);
        t->is_ptr = 1;
        t->is_signed = 0;
        if (have_type) {
            DW_Unit* tu = NULL;
            int tidx = -1;
            OS_Type* child = NULL;
            if (die_glob_find(f, type_ref, &tu, &tidx))
                child = build_type(tu, tidx, depth + 1);
            if (child) type_set_child(u, t, child);
        }
        break;
    case DW_TAG_structure_type:
    case DW_TAG_class_type:
    case DW_TAG_union_type: {
        char buf[128];
        int is_union = (d->tag == DW_TAG_union_type);
        const char* prefix = is_union ? "union" : "struct";
        if (name) snprintf(buf, sizeof(buf), "%s %s", prefix, name);
        else snprintf(buf, sizeof(buf), "%s", prefix);
        t = type_new(u, is_union ? OS_TYPE_UNION : OS_TYPE_STRUCT, buf);
        if (!t) return NULL;
        t->size = have_size ? (uint32_t)byte_size : 0;
        /* collect member children */
        {
            int n = 0;
            int j;
            for (j = 0; j < d->n_child; ++j) {
                int cidx = die_idx + 1 + j;
                DW_Die* c = cidx < u->ndies ? &u->dies[cidx] : NULL;
                if (!c || c->parent != die_idx) continue;
                if (c->tag == DW_TAG_member) ++n;
            }
            if (n > 0) {
                t->children = (OS_Type**)os_alloc(f, (size_t)n * sizeof(OS_Type*));
                if (t->children) {
                    int k = 0;
                    for (j = 0; j < d->n_child; ++j) {
                        int cidx = die_idx + 1 + j;
                        DW_Die* c = cidx < u->ndies ? &u->dies[cidx] : NULL;
                        uint64_t mtype = 0, moff = 0;
                        int have_mtype = 0, have_moff = 0, is_art = 0;
                        uint64_t bsz = 0, boff = 0;
                        int have_bsz = 0, have_boff = 0;
                        const char* mname = NULL;
                        OS_Type* mt;
                        int mi;
                        if (!c || c->parent != die_idx || c->tag != DW_TAG_member) continue;
                        for (mi = 0; mi < c->nattrs; ++mi) {
                            DW_Attr* a = &c->attrs[mi];
                            if (a->name == DW_AT_name) mname = unit_str(u, a);
                            else if (a->name == DW_AT_type) { mtype = const_u64(a); have_mtype = 1; }
                            else if (a->name == DW_AT_data_member_location) {
                                if (a->kind == 8) {
                                    moff = expr_addr(u, a->p, a->v);
                                    if (a->v >= 1 && a->p[0] == DW_OP_constu) {
                                        const uint8_t* q = NULL;
                                        moff = rd_uleb(a->p + 1, a->p + a->v, &q);
                                    } else if (a->v >= 2 && a->p[0] == DW_OP_plus_uconst) {
                                        const uint8_t* q = NULL;
                                        moff = rd_uleb(a->p + 1, a->p + a->v, &q);
                                    }
                                    have_moff = 1;
                                } else {
                                    moff = const_u64(a);
                                    have_moff = 1;
                                }
                            }
                            else if (a->name == DW_AT_artificial) is_art = const_u64(a) ? 1 : 0;
                            else if (a->name == DW_AT_bit_size) { bsz = const_u64(a); have_bsz = 1; }
                            else if (a->name == DW_AT_bit_offset || a->name == DW_AT_data_bit_offset) {
                                boff = const_u64(a); have_boff = 1;
                            }
                        }
                        if (is_art && !mname) continue;
                        mt = NULL;
                        if (have_mtype) {
                            DW_Unit* tu = NULL;
                            int tidx = -1;
                            OS_Type* child = NULL;
                            if (die_glob_find(f, mtype, &tu, &tidx))
                                child = build_type(tu, tidx, depth + 1);
                            mt = shallow_clone(u, child, mname, (int64_t)moff,
                                               have_bsz ? (int)bsz : 0,
                                               have_boff ? (int)boff : 0,
                                               have_bsz);
                            if (!mt && mname) {
                                mt = type_new(u, OS_TYPE_OTHER, mname);
                                if (mt) mt->member_offset = (int64_t)moff;
                            }
                        } else {
                            mt = type_new(u, OS_TYPE_OTHER, mname ? mname : "?");
                            if (mt) mt->member_offset = (int64_t)moff;
                        }
                        if (mt) t->children[k++] = mt;
                    }
                    t->child_count = k;
                }
            }
        }
        break;
    }
    case DW_TAG_array_type: {
        uint64_t count = 0;
        OS_Type* elem = NULL;
        int have_count = 0;
        if (have_type) {
            DW_Unit* tu = NULL;
            int tidx = -1;
            elem = NULL;
            if (die_glob_find(f, type_ref, &tu, &tidx))
                elem = build_type(tu, tidx, depth + 1);
        }
        for (i = 0; i < d->n_child; ++i) {
            int cidx = die_idx + 1 + i;
            DW_Die* c = cidx < u->ndies ? &u->dies[cidx] : NULL;
            uint64_t ub = 0, lb = 0, cnt = 0;
            int have_ub = 0, have_lb = 0, have_cnt = 0;
            int j;
            if (!c || c->parent != die_idx || c->tag != DW_TAG_subrange_type) continue;
            for (j = 0; j < c->nattrs; ++j) {
                DW_Attr* a = &c->attrs[j];
                if (a->name == DW_AT_upper_bound) { ub = const_u64(a); have_ub = 1; }
                else if (a->name == DW_AT_lower_bound) { lb = const_u64(a); have_lb = 1; }
                else if (a->name == DW_AT_count) { cnt = const_u64(a); have_cnt = 1; }
            }
            if (have_cnt) {
                count = count ? count * (cnt ? cnt : 1) : cnt;
                have_count = 1;
            } else if (have_ub) {
                uint64_t n = ub + 1 - (have_lb ? lb : 0);
                count = count ? count * (n ? n : 1) : n;
                have_count = 1;
            }
        }
        if (!have_count && have_size && elem && elem->size)
            count = byte_size / elem->size;
        t = type_new(u, OS_TYPE_ARRAY, name ? name : "");
        if (!t) return NULL;
        t->array_count = (int)count;
        t->size = have_size ? (uint32_t)byte_size : (uint32_t)(count * (elem ? elem->size : 1));
        if (elem) type_set_child(u, t, elem);
        break;
    }
    case DW_TAG_enumeration_type: {
        int n_enum = 0;
        int j;
        for (j = 0; j < d->n_child; ++j) {
            int cidx = die_idx + 1 + j;
            DW_Die* c = cidx < u->ndies ? &u->dies[cidx] : NULL;
            if (c && c->parent == die_idx && c->tag == DW_TAG_enumerator) ++n_enum;
        }
        t = type_new(u, OS_TYPE_ENUM, name ? name : "enum");
        if (!t) return NULL;
        t->size = have_size ? (uint32_t)byte_size : 4;
        if (n_enum > 0) {
            int k = 0;
            t->children = (OS_Type**)os_alloc(f, (size_t)n_enum * sizeof(OS_Type*));
            if (t->children) {
                for (j = 0; j < d->n_child; ++j) {
                    int cidx = die_idx + 1 + j;
                    DW_Die* c = cidx < u->ndies ? &u->dies[cidx] : NULL;
                    const char* ename = NULL;
                    int64_t eval = 0;
                    int have_val = 0;
                    int mi;
                    if (!c || c->parent != die_idx || c->tag != DW_TAG_enumerator) continue;
                    for (mi = 0; mi < c->nattrs; ++mi) {
                        DW_Attr* a = &c->attrs[mi];
                        if (a->name == DW_AT_name) ename = unit_str(u, a);
                        else if (a->name == DW_AT_const_value) { eval = (int64_t)const_u64(a); have_val = 1; }
                    }
                    {
                        OS_Type* et = type_new(u, OS_TYPE_ENUM, ename ? ename : "?");
                        if (et) {
                            et->enum_value = eval;
                            if (have_val) et->size = 4;
                            t->children[k++] = et;
                        }
                    }
                }
                t->child_count = k;
            }
        }
        break;
    }
    case DW_TAG_string_type:
        t = type_new(u, OS_TYPE_STRING, name ? name : "char*");
        if (t) t->size = have_size ? (uint32_t)byte_size : 1;
        break;
    case DW_TAG_subroutine_type:
    case DW_TAG_unspecified_type:
        t = type_new(u, OS_TYPE_OTHER, name ? name : "?");
        if (t) t->size = have_size ? (uint32_t)byte_size : 0;
        break;
    default:
        t = type_new(u, OS_TYPE_OTHER, name ? name : "?");
        if (t) t->size = have_size ? (uint32_t)byte_size : 0;
        break;
    }
    (void)tagname;
    (void)is_ptr;
    (void)is_signed;
    u->type_map[die_idx] = t;
    return t;
}

/* ------------------------ variables ------------------------------- */

static void unit_add_variable(DW_Unit* u, DW_Die* d)
{
    OS_ElfFile* f = u->f;
    const char* name = NULL;
    uint64_t type_ref = 0, addr = 0;
    int have_type = 0, have_addr = 0, is_decl = 0, is_art = 0;
    int i;
    OS_Variable* v;

    for (i = 0; i < d->nattrs; ++i) {
        DW_Attr* a = &d->attrs[i];
        if (a->name == DW_AT_name) name = unit_str(u, a);
        else if (a->name == DW_AT_type) { type_ref = const_u64(a); have_type = 1; }
        else if (a->name == DW_AT_location) {
            if (a->kind == 8) {
                addr = expr_addr(u, a->p, a->v);
                have_addr = addr != 0;
            } else if (a->kind == 7 || a->kind == 9) {
                addr = unit_addr(u, a);
                have_addr = 1;
            }
        }
        else if (a->name == DW_AT_declaration) is_decl = const_u64(a) ? 1 : 0;
        else if (a->name == DW_AT_artificial) is_art = const_u64(a) ? 1 : 0;
        else if (a->name == DW_AT_specification || a->name == DW_AT_abstract_origin) {
            if (!name) {
                uint64_t ref = const_u64(a);
                DW_Unit* tu = NULL;
                int tidx = -1;
                DW_Die* rd = die_glob_find(f, ref, &tu, &tidx);
                if (rd) {
                    int j;
                    for (j = 0; j < rd->nattrs; ++j)
                        if (rd->attrs[j].name == DW_AT_name)
                            name = unit_str(tu, &rd->attrs[j]);
                }
            }
        }
    }
    if (is_decl || is_art || !name || !name[0]) return;
    if (!have_addr) return;

    /* duplicate check (across CUs, keep first with debug type) */
    for (i = 0; i < f->var_count; ++i) {
        if (f->vars[i].name && strcmp(f->vars[i].name, name) == 0) {
            if (f->vars[i].type == NULL && have_type) {
                DW_Unit* tu = NULL;
                int tidx = -1;
                f->vars[i].type = NULL;
                if (die_glob_find(f, type_ref, &tu, &tidx))
                    f->vars[i].type = build_type(tu, tidx, 0);
                f->vars[i].has_debug = 1;
            }
            return;
        }
    }
    if (f->var_count == f->var_cap) {
        int nc = f->var_cap ? f->var_cap * 2 : 256;
        OS_Variable* nv = (OS_Variable*)os_alloc(f, (size_t)nc * sizeof(OS_Variable));
        if (!nv) return;
        if (f->vars) memcpy(nv, f->vars, (size_t)f->var_count * sizeof(OS_Variable));
        f->vars = nv;
        f->var_cap = nc;
    }
    v = &f->vars[f->var_count];
    memset(v, 0, sizeof(*v));
    v->name = os_strdup(f, name);
    v->address = addr;
    if (have_type) {
        DW_Unit* tu = NULL;
        int tidx = -1;
        v->type = NULL;
        if (die_glob_find(f, type_ref, &tu, &tidx))
            v->type = build_type(tu, tidx, 0);
    }
    if (v->type && v->type->size)
        v->symbol_size = v->type->size;
    v->has_debug = 1;
    ++f->var_count;
}

static void parse_dwarf(OS_ElfFile* f)
{
    uint64_t off = 0;
    int ui, i;
    while (off + 4 <= f->dbg_info_size) {
        DW_Unit u;
        uint64_t len;
        if (parse_unit(f, off, &u) != 0) {
            /* skip malformed unit, keep parsing the rest */
            for (i = 0; i < u.ndies; i++) free(u.dies[i].attrs);
            free(u.dies);
            free(u.die_offs);
            free(u.die_index);
            free(u.type_map);
        } else {
            if (f->n_units == f->cap_units) {
                int nc = f->cap_units ? f->cap_units * 2 : 32;
                DW_Unit* nu = (DW_Unit*)realloc(f->units, (size_t)nc * sizeof(DW_Unit));
                if (!nu) break;
                f->units = nu;
                f->cap_units = nc;
            }
            f->units[f->n_units++] = u;
        }
        len = rd_u32(f->dbg_info + off, f->le);
        if (len == 0 || len == 0xffffffff) break;
        off += 4 + len;
    }

    /* global DIE index (dies appended in increasing absolute offset order) */
    for (ui = 0; ui < f->n_units; ui++) {
        DW_Unit* u = &f->units[ui];
        for (i = 0; i < u->ndies; i++) {
            if (f->die_glob_count == f->die_glob_cap) {
                int nc = f->die_glob_cap ? f->die_glob_cap * 2 : 4096;
                uint64_t* no = (uint64_t*)realloc(f->die_glob_off,
                                                  (size_t)nc * sizeof(uint64_t));
                int* nu2 = (int*)realloc(f->die_glob_unit, (size_t)nc * sizeof(int));
                int* ni = (int*)realloc(f->die_glob_idx, (size_t)nc * sizeof(int));
                if (!no || !nu2 || !ni) {
                    if (no) f->die_glob_off = no;
                    if (nu2) f->die_glob_unit = nu2;
                    if (ni) f->die_glob_idx = ni;
                    goto done;
                }
                f->die_glob_off = no;
                f->die_glob_unit = nu2;
                f->die_glob_idx = ni;
                f->die_glob_cap = nc;
            }
            f->die_glob_off[f->die_glob_count] = u->dies[i].off;
            f->die_glob_unit[f->die_glob_count] = ui;
            f->die_glob_idx[f->die_glob_count] = i;
            f->die_glob_count++;
        }
    }

    /* collect global variables */
    for (ui = 0; ui < f->n_units; ui++) {
        DW_Unit* u = &f->units[ui];
        for (i = 0; i < u->ndies; i++) {
            if (u->dies[i].tag == DW_TAG_variable)
                unit_add_variable(u, &u->dies[i]);
        }
    }

done:
    for (ui = 0; ui < f->n_units; ui++) {
        DW_Unit* u = &f->units[ui];
        for (i = 0; i < u->ndies; i++) free(u->dies[i].attrs);
        free(u->dies);
        free(u->die_offs);
        free(u->die_index);
        free(u->type_map);
    }
    free(f->units);
    free(f->die_glob_off);
    free(f->die_glob_unit);
    free(f->die_glob_idx);
    f->units = NULL;
    f->die_glob_off = NULL;
    f->die_glob_unit = NULL;
    f->die_glob_idx = NULL;
    f->n_units = f->cap_units = 0;
    f->die_glob_count = f->die_glob_cap = 0;
}

/* ------------------------ symbol fallback ------------------------- */

static void parse_symtab(OS_ElfFile* f)
{
    const uint8_t* p = f->symtab;
    const uint8_t* end = f->symtab ? f->symtab + f->symtab_size : NULL;
    uint64_t es = f->sym_entsize ? f->sym_entsize : (f->is64 ? 24 : 16);
    while (p && p + es <= end) {
        const uint8_t* s = p;
        uint32_t st_name;
        uint8_t st_info;
        uint16_t st_shndx;
        uint64_t st_value, st_size;
        uint8_t type, bind;
        const char* nm;
        int i;
        if (f->is64) {
            st_name = rd_u32(s, f->le);
            st_info = s[4];
            st_shndx = rd_u16(s + 6, f->le);
            st_value = rd_u64(s + 8, f->le);
            st_size = rd_u64(s + 16, f->le);
        } else {
            st_name = rd_u32(s, f->le);
            st_value = rd_u32(s + 4, f->le);
            st_size = rd_u32(s + 8, f->le);
            st_info = s[12];
            st_shndx = rd_u16(s + 14, f->le);
        }
        type = st_info & 0xf;
        bind = st_info >> 4;
        p += es;
        if (type != STT_OBJECT) continue;
        if (bind != STB_GLOBAL && bind != STB_WEAK) continue;
        if (st_shndx == SHN_UNDEF) continue;
        if (!f->strtab || st_name >= f->strtab_size) continue;
        nm = (const char*)(f->strtab + st_name);
        if (!nm[0] || nm[0] == '$') continue;
        if (st_size == 0) continue;
        for (i = 0; i < f->var_count; ++i)
            if (f->vars[i].name && strcmp(f->vars[i].name, nm) == 0)
                break;
        if (i < f->var_count) {
            if (!f->vars[i].symbol_size) f->vars[i].symbol_size = st_size;
            continue;
        }
        if (f->var_count == f->var_cap) {
            int nc = f->var_cap ? f->var_cap * 2 : 256;
            OS_Variable* nv = (OS_Variable*)os_alloc(f, (size_t)nc * sizeof(OS_Variable));
            if (!nv) return;
            if (f->vars) memcpy(nv, f->vars, (size_t)f->var_count * sizeof(OS_Variable));
            f->vars = nv;
            f->var_cap = nc;
        }
        f->vars[f->var_count].name = os_strdup(f, nm);
        f->vars[f->var_count].address = st_value;
        f->vars[f->var_count].symbol_size = st_size;
        f->vars[f->var_count].type = NULL;
        f->vars[f->var_count].has_debug = 0;
        ++f->var_count;
    }
}

/* ------------------------ public API ------------------------------ */

OS_ElfFile* os_elf_open(const char* path, char* errbuf, int errbuf_len)
{
    FILE* fp;
    long sz;
    uint8_t* data;
    OS_ElfFile* f;

    if (errbuf_len > 0) errbuf[0] = 0;
    fp = fopen(path, "rb");
    if (!fp) {
        snprintf(errbuf, errbuf_len, "cannot open file: %s", path);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz <= 0) {
        fclose(fp);
        snprintf(errbuf, errbuf_len, "empty file");
        return NULL;
    }
    data = (uint8_t*)malloc((size_t)sz);
    if (!data) {
        fclose(fp);
        snprintf(errbuf, errbuf_len, "out of memory");
        return NULL;
    }
    if (fread(data, 1, (size_t)sz, fp) != (size_t)sz) {
        free(data);
        fclose(fp);
        snprintf(errbuf, errbuf_len, "read error");
        return NULL;
    }
    fclose(fp);

    f = (OS_ElfFile*)calloc(1, sizeof(OS_ElfFile));
    if (!f) { free(data); snprintf(errbuf, errbuf_len, "out of memory"); return NULL; }
    f->data = data;
    f->data_len = (size_t)sz;

    if (elf_parse_header(f, errbuf, errbuf_len) != 0 ||
        elf_parse_sections(f, errbuf, errbuf_len) != 0) {
        os_elf_close(f);
        return NULL;
    }
    if (f->dbg_info) parse_dwarf(f);
    if (f->symtab) parse_symtab(f);
    if (f->var_count == 0) {
        snprintf(errbuf, errbuf_len, "no global variables found (no debug info / symbols)");
        os_elf_close(f);
        return NULL;
    }
    return f;
}

void os_elf_close(OS_ElfFile* f)
{
    OS_Alloc* a;
    if (!f) return;
    free((void*)f->data);
    a = f->allocs;
    while (a) {
        OS_Alloc* next = a->next;
        free(a->p);
        free(a);
        a = next;
    }
    free(f);
}

uint16_t os_elf_machine(OS_ElfFile* f) { return f ? f->machine : 0; }
uint32_t os_elf_flags(OS_ElfFile* f)   { return f ? f->flags : 0; }
int      os_elf_bits(OS_ElfFile* f)    { return f ? (f->is64 ? 64 : 32) : 0; }
uint64_t os_elf_entry(OS_ElfFile* f)   { return f ? f->entry : 0; }

const char* os_elf_arch_name(OS_ElfFile* f)
{
    if (!f) return "unknown";
    switch (f->machine) {
    case 3:   return "x86";
    case 8:   return "MIPS";
    case 40:  return (f->flags & 0x100000) ? "ARM Cortex-M" : "ARM";
    case 62:  return "x86-64";
    case 183: return "AArch64";
    case 243: return "RISC-V";
    case 20:  return "PowerPC";
    case 21:  return "PowerPC64";
    case 2:   return "SPARC";
    default:  return "unknown";
    }
}

int os_elf_var_count(OS_ElfFile* f)
{
    return f ? f->var_count : 0;
}

const OS_Variable* os_elf_var_at(OS_ElfFile* f, int idx)
{
    if (!f || idx < 0 || idx >= f->var_count) return NULL;
    return &f->vars[idx];
}

int os_elf_find_var(OS_ElfFile* f, const char* name)
{
    int i;
    if (!f || !name) return -1;
    for (i = 0; i < f->var_count; ++i)
        if (f->vars[i].name && strcmp(f->vars[i].name, name) == 0)
            return i;
    return -1;
}

static int ci_substr(const char* hay, const char* needle)
{
    size_t nh = strlen(hay), nn = strlen(needle);
    size_t i, j;
    if (nn == 0) return 1;
    if (nn > nh) return 0;
    for (i = 0; i + nn <= nh; ++i) {
        for (j = 0; j < nn; ++j) {
            char a = hay[i + j], b = needle[j];
            if (a >= 'A' && a <= 'Z') a += 32;
            if (b >= 'A' && b <= 'Z') b += 32;
            if (a != b) break;
        }
        if (j == nn) return 1;
    }
    return 0;
}

int os_elf_find_vars(OS_ElfFile* f, const char* needle, int max, int* out)
{
    int i, n = 0;
    if (!f || !needle || max <= 0) return 0;
    for (i = 0; i < f->var_count && n < max; ++i)
        if (f->vars[i].name && ci_substr(f->vars[i].name, needle))
            out[n++] = i;
    return n;
}
