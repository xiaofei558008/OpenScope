/**
 * ASAP2 / A2L 导出（面向 CANape / INCA 的精简但合法子集）。
 *
 * 数据来源：当前已加载 ELF/out 的 DWARF 叶变量表（绝对地址与类型），
 * 不解析 .map；地址与 XCP SHORT_UPLOAD / DOWNLOAD 使用的绝对地址一致。
 *
 * 输出内容：
 *   - 基础类型 MEASUREMENT（只读测量）
 *   - calibration_mode=1 时，RAM 中的标量输出为 CHARACTERISTIC（可标定写值）
 *   - 枚举输出 COMPU_VTAB，位域输出 BIT_MASK
 *   - 一维字节数组输出 MATRIX_DIM
 */
#include "a2l_export.h"
#include "app.h"
#include "util.h"
#include "version.h"

#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define A2L_ID_CAP      32   /* 含结尾 0；ASAP2 标识符上限 31 字符 */
#define A2L_HASH_CAP    32768

typedef struct A2LIds {
    char* names; /* A2L_HASH_CAP * A2L_ID_CAP */
} A2LIds;

typedef struct A2LType {
    const char* dt;   /* MEASUREMENT/CHARACTERISTIC 数据类型 */
    const char* rl;   /* DEPOSIT record layout */
    const char* lo;
    const char* hi;
} A2LType;

static uint32_t a2l_hash(const char* s)
{
    uint32_t h = 2166136261u;
    while (s && *s) {
        h ^= (uint8_t)*s++;
        h *= 16777619u;
    }
    return h;
}

static int a2l_ids_init(A2LIds* ids)
{
    ids->names = (char*)calloc(A2L_HASH_CAP, A2L_ID_CAP);
    return ids->names ? 0 : -1;
}

static void a2l_ids_free(A2LIds* ids)
{
    free(ids->names);
    ids->names = NULL;
}

static int a2l_id_has(const A2LIds* ids, const char* name)
{
    uint32_t h = a2l_hash(name) & (A2L_HASH_CAP - 1);
    int i;
    for (i = 0; i < A2L_HASH_CAP; i++) {
        const char* slot = ids->names + (size_t)h * A2L_ID_CAP;
        if (!slot[0]) return 0;
        if (strcmp(slot, name) == 0) return 1;
        h = (h + 1) & (A2L_HASH_CAP - 1);
    }
    return 0;
}

static void a2l_id_put(A2LIds* ids, const char* name)
{
    uint32_t h = a2l_hash(name) & (A2L_HASH_CAP - 1);
    int i;
    for (i = 0; i < A2L_HASH_CAP; i++) {
        char* slot = ids->names + (size_t)h * A2L_ID_CAP;
        if (!slot[0]) {
            _snprintf(slot, A2L_ID_CAP, "%s", name);
            return;
        }
        if (strcmp(slot, name) == 0) return;
        h = (h + 1) & (A2L_HASH_CAP - 1);
    }
}

/* 生成合法 A2L 标识符（字母/数字/下划线，首字符非数字，<=31） */
static void a2l_sanitize(const char* in, char* out, int cap)
{
    int i, j = 0, last_us = 0;
    if (!out || cap < 2) return;
    for (i = 0; in && in[i] && j < cap - 1; i++) {
        unsigned char c = (unsigned char)in[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_') {
            out[j++] = (char)c;
            last_us = (c == '_');
        } else if ((c == '.' || c == '[' || c == ']' || c == ':' ||
                    c == ' ' || c == '-' || c == '/') && !last_us) {
            out[j++] = '_';
            last_us = 1;
        }
    }
    while (j > 0 && out[j - 1] == '_') j--;
    out[j] = 0;
    if (j == 0) _snprintf(out, cap, "V");
    if (out[0] >= '0' && out[0] <= '9') {
        memmove(out + 1, out, strlen(out) + 1);
        out[0] = '_';
    }
}

/* 生成唯一标识符：冲突/过长时截断并追加 _<id> */
static void a2l_unique(A2LIds* ids, const char* base, int id, char* out, int cap)
{
    char tmp[A2L_ID_CAP];
    a2l_sanitize(base, tmp, sizeof(tmp));
    if (strlen(tmp) > (size_t)(cap - 1)) tmp[cap - 1] = 0;
    if (!a2l_id_has(ids, tmp)) {
        _snprintf(out, cap, "%s", tmp);
        a2l_id_put(ids, out);
        return;
    }
    {
        char suffix[24];
        int n = 0;
        _snprintf(suffix, sizeof(suffix), "_%d", id);
        n = (int)strlen(suffix);
        if (n > cap - 2) n = cap - 2;
        if ((int)strlen(tmp) > cap - 1 - n) tmp[cap - 1 - n] = 0;
        _snprintf(out, cap, "%s%s", tmp, suffix);
    }
    if (a2l_id_has(ids, out)) {
        int k = 1;
        do {
            char suffix[32];
            int n;
            _snprintf(suffix, sizeof(suffix), "_%d_%d", id, k++);
            n = (int)strlen(suffix);
            if (n > cap - 2) n = cap - 2;
            if ((int)strlen(tmp) > cap - 1 - n) tmp[cap - 1 - n] = 0;
            _snprintf(out, cap, "%s%s", tmp, suffix);
        } while (a2l_id_has(ids, out) && k < 1000);
    }
    a2l_id_put(ids, out);
}

static void a2l_escape(const char* in, char* out, int cap)
{
    int i, j = 0;
    if (!out || cap < 1) return;
    if (!in) { out[0] = 0; return; }
    for (i = 0; in[i] && j < cap - 2; i++) {
        unsigned char c = (unsigned char)in[i];
        if (c == '"' || c == '\\') {
            out[j++] = '\\';
            out[j++] = (char)c;
        } else if (c < 0x20) {
            out[j++] = ' ';
        } else {
            out[j++] = (char)c;
        }
    }
    out[j] = 0;
}

static int a2l_ram_addr(uint64_t a)
{
    return (a >= 0x10000000ULL && a < 0x10010000ULL) ||  /* CCM */
           (a >= 0x20000000ULL && a < 0x20200000ULL) ||  /* SRAM */
           (a >= 0x24000000ULL && a < 0x24200000ULL) ||  /* H7 AXI SRAM */
           (a >= 0x30000000ULL && a < 0x30080000ULL) ||  /* H7 SRAM1-4 */
           (a >= 0x38000000ULL && a < 0x38100000ULL);    /* H7 SRAM4 */
}

static int a2l_type_for(const OS_Leaf* L, A2LType* t)
{
    if (!L || !t) return -1;
    if (L->is_bitfield) {
        if (L->size == 1) {
            t->dt = L->is_signed ? "SBYTE" : "UBYTE";
            t->rl = L->is_signed ? "RL_SBYTE" : "RL_UBYTE";
            t->lo = L->is_signed ? "-128" : "0";
            t->hi = L->is_signed ? "127" : "255";
        } else if (L->size == 2) {
            t->dt = L->is_signed ? "SWORD" : "UWORD";
            t->rl = L->is_signed ? "RL_SWORD" : "RL_UWORD";
            t->lo = L->is_signed ? "-32768" : "0";
            t->hi = L->is_signed ? "32767" : "65535";
        } else {
            t->dt = L->is_signed ? "SLONG" : "ULONG";
            t->rl = L->is_signed ? "RL_SLONG" : "RL_ULONG";
            t->lo = L->is_signed ? "-2147483648" : "0";
            t->hi = L->is_signed ? "2147483647" : "4294967295";
        }
        return 0;
    }
    if (L->kind == OS_TYPE_FLOAT && L->size == 4) {
        t->dt = "FLOAT32_IEEE"; t->rl = "RL_FLOAT32";
        t->lo = "-3.4028235E+38"; t->hi = "3.4028235E+38";
        return 0;
    }
    if (L->kind == OS_TYPE_FLOAT && L->size == 8) {
        t->dt = "FLOAT64_IEEE"; t->rl = "RL_FLOAT64";
        t->lo = "-1.7976931348623157E+308"; t->hi = "1.7976931348623157E+308";
        return 0;
    }
    if (L->size == 1) {
        t->dt = L->is_signed ? "SBYTE" : "UBYTE";
        t->rl = L->is_signed ? "RL_SBYTE" : "RL_UBYTE";
        t->lo = L->is_signed ? "-128" : "0";
        t->hi = L->is_signed ? "127" : "255";
        return 0;
    }
    if (L->size == 2) {
        t->dt = L->is_signed ? "SWORD" : "UWORD";
        t->rl = L->is_signed ? "RL_SWORD" : "RL_UWORD";
        t->lo = L->is_signed ? "-32768" : "0";
        t->hi = L->is_signed ? "32767" : "65535";
        return 0;
    }
    if (L->size == 4) {
        t->dt = L->is_signed ? "SLONG" : "ULONG";
        t->rl = L->is_signed ? "RL_SLONG" : "RL_ULONG";
        t->lo = L->is_signed ? "-2147483648" : "0";
        t->hi = L->is_signed ? "2147483647" : "4294967295";
        return 0;
    }
    if (L->size == 8) {
        t->dt = L->is_signed ? "A_INT64" : "A_UINT64";
        t->rl = L->is_signed ? "RL_A_INT64" : "RL_A_UINT64";
        t->lo = L->is_signed ? "-9223372036854775807" : "0";
        t->hi = L->is_signed ? "9223372036854775807" : "18446744073709551615";
        return 0;
    }
    return -1;
}

static uint64_t a2l_bitmask(const OS_Leaf* L)
{
    uint64_t mask;
    if (!L || !L->is_bitfield || L->bit_size == 0 || L->bit_size >= 64)
        return 0;
    mask = ((uint64_t)1 << L->bit_size) - 1;
    return mask << L->bit_offset;
}

static void a2l_emit_record_layouts(FILE* f)
{
    static const char* rl[][2] = {
        {"RL_SBYTE", "SBYTE"}, {"RL_UBYTE", "UBYTE"},
        {"RL_SWORD", "SWORD"}, {"RL_UWORD", "UWORD"},
        {"RL_SLONG", "SLONG"}, {"RL_ULONG", "ULONG"},
        {"RL_A_INT64", "A_INT64"}, {"RL_A_UINT64", "A_UINT64"},
        {"RL_FLOAT32", "FLOAT32_IEEE"}, {"RL_FLOAT64", "FLOAT64_IEEE"}
    };
    int i;
    for (i = 0; i < (int)(sizeof(rl) / sizeof(rl[0])); i++) {
        fprintf(f,
            "    /begin RECORD_LAYOUT %s\n"
            "      FNC_VALUES 1 %s COLUMN_DIR DIRECT\n"
            "    /end RECORD_LAYOUT\n", rl[i][0], rl[i][1]);
    }
}

static void a2l_emit_compu_vtab(FILE* f, const char* conv, const char* long_id,
                                const OS_Leaf* L)
{
    int i;
    fprintf(f,
        "    /begin COMPU_VTAB %s \"%s\"\n"
        "      TAB_VERB\n"
        "      %d\n", conv, long_id, L->enum_count);
    for (i = 0; i < L->enum_count; i++) {
        char en[128];
        a2l_escape(L->enums[i].name, en, sizeof(en));
        fprintf(f, "      %lld \"%s\"\n", (long long)L->enums[i].value, en);
    }
    fprintf(f, "    /end COMPU_VTAB\n");
}

static void a2l_emit_measurement(FILE* f, const char* id, const char* long_id,
                                 const A2LType* t, uint64_t addr, uint32_t array_dim,
                                 const char* conv, uint64_t bitmask)
{
    fprintf(f,
        "    /begin MEASUREMENT %s \"%s\"\n"
        "      %s %s 0 0 %s %s\n"
        "      ECU_ADDRESS 0x%08X\n",
        id, long_id, t->dt, conv, t->lo, t->hi, (unsigned)(uint32_t)addr);
    if (array_dim > 0) fprintf(f, "      MATRIX_DIM %u\n", (unsigned)array_dim);
    if (bitmask) fprintf(f, "      BIT_MASK 0x%08X\n", (unsigned)(uint32_t)bitmask);
    fprintf(f, "    /end MEASUREMENT\n");
}

static void a2l_emit_characteristic(FILE* f, const char* id, const char* long_id,
                                    const A2LType* t, uint64_t addr, uint32_t array_dim,
                                    const char* conv, uint64_t bitmask)
{
    const char* type = array_dim > 0 ? "VAL_BLK" : "VALUE";
    fprintf(f,
        "    /begin CHARACTERISTIC %s \"%s\"\n"
        "      %s 0x%08X %s 0 %s %s %s\n",
        id, long_id, type, (unsigned)(uint32_t)addr, t->rl, conv, t->lo, t->hi);
    if (array_dim > 0) fprintf(f, "      MATRIX_DIM %u\n", (unsigned)array_dim);
    if (bitmask) fprintf(f, "      BIT_MASK 0x%08X\n", (unsigned)(uint32_t)bitmask);
    fprintf(f, "    /end CHARACTERISTIC\n");
}

int os_a2l_export(const wchar_t* path_w, const char* module_name, int calibration_mode)
{
    FILE* f;
    char path8[MAX_PATH], mod[64];
    A2LIds ids;
    int i, n_meas = 0, n_char = 0, n_skip = 0;

    if (!path_w || !path_w[0]) return -1;
    if (g_app.leaf_count <= 0) {
        os_log(OS_LOG_WARN, "A2L 导出: 无叶变量（请先加载 ELF/out）");
        return -2;
    }
    if (a2l_ids_init(&ids) != 0) return -3;
    /* 保留固定标识符，避免与变量名冲突 */
    {
        static const char* fixed[] = {
            "CM_IDENT", "RL_SBYTE", "RL_UBYTE", "RL_SWORD", "RL_UWORD",
            "RL_SLONG", "RL_ULONG", "RL_A_INT64", "RL_A_UINT64",
            "RL_FLOAT32", "RL_FLOAT64", "OpenScope"
        };
        int k;
        for (k = 0; k < (int)(sizeof(fixed) / sizeof(fixed[0])); k++)
            a2l_id_put(&ids, fixed[k]);
    }
    a2l_sanitize((module_name && module_name[0]) ? module_name : "ECU", mod, sizeof(mod));

    os_wide_to_utf8_buf(path_w, path8, sizeof(path8));
    f = fopen(path8, "wb");
    if (!f) {
        os_log(OS_LOG_ERROR, "A2L 导出: 无法写入 %s", path8);
        a2l_ids_free(&ids);
        return -4;
    }

    fprintf(f, "ASAP2_VERSION 1 71\n");
    fprintf(f, "/* Generated by OpenScope %s; addresses from loaded ELF/DWARF. */\n",
            OS_VERSION_STR);
    fprintf(f, "/begin PROJECT OpenScope \"OpenScope exported A2L\"\n");
    fprintf(f,
        "  /begin HEADER \"OpenScope A2L export\"\n"
        "    VERSION \"%s\"\n"
        "    PROJECT_NO 1\n"
        "  /end HEADER\n", OS_VERSION_STR);
    fprintf(f, "  /begin MODULE %s \"ECU variables\"\n", mod);
    fprintf(f,
        "    /begin MOD_COMMON \"\"\n"
        "      BYTE_ORDER MSB_LAST\n"
        "      ALIGNMENT_BYTE 1\n"
        "      ALIGNMENT_WORD 1\n"
        "      ALIGNMENT_LONG 1\n"
        "      ALIGNMENT_FLOAT32_IEEE 1\n"
        "      ALIGNMENT_FLOAT64_IEEE 1\n"
        "      ALIGNMENT_INT64 1\n"
        "    /end MOD_COMMON\n");
    fprintf(f,
        "    /begin MOD_PAR \"OpenScope\"\n"
        "      EPK \"OpenScope %s\"\n"
        "    /end MOD_PAR\n", OS_VERSION_STR);

    fprintf(f,
        "    /begin COMPU_METHOD CM_IDENT \"identity\"\n"
        "      IDENTICAL \"%%6.3\" \"\"\n"
        "    /end COMPU_METHOD\n");
    a2l_emit_record_layouts(f);

    for (i = 0; i < g_app.leaf_count; i++) {
        const OS_Leaf* L = &g_app.leaves[i];
        A2LType t;
        char id[A2L_ID_CAP], long_id[512], conv[64];
        uint32_t array_dim = 0;
        uint64_t bitmask = 0;
        int is_ram, is_char;

        if (!L->name[0] || L->size == 0) continue;
        if (a2l_type_for(L, &t) != 0) {
            /* 数组/字符串：按原始字节导出为一维 UBYTE */
            if ((L->kind == OS_TYPE_STRING || L->kind == OS_TYPE_OTHER) &&
                L->size > 8 && L->size <= 4096) {
                t.dt = "UBYTE"; t.rl = "RL_UBYTE"; t.lo = "0"; t.hi = "255";
                array_dim = L->size;
            } else {
                n_skip++;
                continue;
            }
        }
        a2l_unique(&ids, L->name, L->id, id, sizeof(id));
        a2l_escape(L->name, long_id, sizeof(long_id));
        _snprintf(conv, sizeof(conv), "CM_IDENT");

        if (L->enum_count > 0 && !L->is_bitfield) {
            char vt[64], vtbase[320];
            _snprintf(vtbase, sizeof(vtbase), "VT_%s", L->name);
            a2l_unique(&ids, vtbase, L->id + 100000, vt, sizeof(vt));
            a2l_emit_compu_vtab(f, vt, long_id, L);
            _snprintf(conv, sizeof(conv), "%s", vt);
        }
        if (L->is_bitfield) bitmask = a2l_bitmask(L);

        is_ram = a2l_ram_addr(L->address);
        is_char = calibration_mode && is_ram;

        if (is_char) {
            a2l_emit_characteristic(f, id, long_id, &t, L->address, array_dim,
                                    conv, bitmask);
            n_char++;
        } else {
            a2l_emit_measurement(f, id, long_id, &t, L->address, array_dim,
                                 conv, bitmask);
            n_meas++;
        }
    }

    fprintf(f, "  /end MODULE\n");
    fprintf(f, "/end PROJECT\n");
    fclose(f);
    a2l_ids_free(&ids);

    os_log(OS_LOG_INFO,
           "A2L 已导出: %d MEASUREMENT + %d CHARACTERISTIC（跳过 %d）→ %s",
           n_meas, n_char, n_skip, path8);
    return 0;
}

int os_a2l_export_dialog(HWND owner, int calibration_mode)
{
    OPENFILENAMEW ofn;
    wchar_t path[MAX_PATH];
    path[0] = 0;

    if (g_app.elf_path[0]) {
        wchar_t* slash = wcsrchr(g_app.elf_path, L'\\');
        wchar_t* dot;
        size_t n = slash ? (size_t)(slash - g_app.elf_path + 1) : 0;
        if (n < MAX_PATH) {
            wcsncpy(path, g_app.elf_path, n);
            path[n] = 0;
            wcsncat(path, slash ? slash + 1 : g_app.elf_path, MAX_PATH - n - 1);
            dot = wcsrchr(path, L'.');
            if (dot) *dot = 0;
            wcsncat(path, L".a2l", MAX_PATH - wcslen(path) - 1);
        }
    }
    if (!path[0]) wcscpy(path, L"openscope.a2l");

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner ? owner : g_app.hMain;
    ofn.lpstrFilter = L"ASAP2 A2L\0*.a2l\0所有文件\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"a2l";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    if (!GetSaveFileNameW(&ofn)) return 1;
    return os_a2l_export(path, "ECU", calibration_mode);
}
