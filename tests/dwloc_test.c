/* dwloc 单元测试：DWARF 位置表达式 / 位置列表 / 范围列表 / .debug_frame CFA */
#include "dwloc.h"
#include <stdio.h>
#include <string.h>

static int g_fail, g_n;
#define CHECK(c, msg) do { g_n++; if (!(c)) { g_fail++; printf("FAIL: %s (line %d)\n", msg, __LINE__); } } while (0)

static uint8_t g_mem[256];
static int rd(void* u, uint64_t a, void* b, uint32_t n)
{
    (void)u;
    if (a < 0x1000 || a + n > 0x1000 + sizeof(g_mem)) return -1;
    memcpy(b, g_mem + (a - 0x1000), n);
    return 0;
}

static void ctx_arm(DwLocCtx* c)
{
    memset(c, 0, sizeof(*c));
    c->arch = 1; c->addr_size = 4; c->le = 1; c->read_mem = rd;
    c->reg[0] = 0x11; c->reg_valid[0] = 1;
    c->reg[1] = 0x22; c->reg_valid[1] = 1;
    c->reg[13] = 0x1080; c->reg_valid[13] = 1;
}

int main(void)
{
    DwLocCtx c;
    DwLocResult r;
    uint8_t buf[16];
    uint64_t v;
    int sz;

    /* DW_OP_reg0 */
    { uint8_t e[] = {0x50}; ctx_arm(&c); dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_REG && r.reg == 0, "reg0");
      CHECK(dwloc_materialize(&c, &r, buf, 4) == 0 && buf[0] == 0x11, "materialize reg0"); }
    /* DW_OP_breg13 -4 → mem 0x107C */
    { uint8_t e[] = {0x7d, 0x7c}; ctx_arm(&c); dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_MEM && r.addr == 0x107C, "breg13-4"); }
    /* DW_OP_fbreg 8 with frame base */
    { uint8_t e[] = {0x91, 0x08}; ctx_arm(&c); c.have_frame_base = 1; c.frame_base = 0x1040;
      dwloc_eval(e, sizeof(e), &c, &r); CHECK(r.kind == DWLOC_MEM && r.addr == 0x1048, "fbreg8"); }
    /* fbreg 无 frame base → 错误 */
    { uint8_t e[] = {0x91, 0x08}; ctx_arm(&c); dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_ERROR, "fbreg w/o frame base"); }
    /* DW_OP_call_frame_cfa */
    { uint8_t e[] = {0x9c}; ctx_arm(&c); c.have_cfa = 1; c.cfa = 0x1090;
      dwloc_eval(e, sizeof(e), &c, &r); CHECK(r.kind == DWLOC_MEM && r.addr == 0x1090, "call_frame_cfa"); }
    /* DW_OP_addr 0x1010 */
    { uint8_t e[] = {0x03, 0x10, 0x10, 0, 0}; ctx_arm(&c); dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_MEM && r.addr == 0x1010, "addr"); }
    /* lit5 lit3 plus stack_value → 8 */
    { uint8_t e[] = {0x35, 0x33, 0x22, 0x9f}; ctx_arm(&c); dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_VALUE && r.bytes[0] == 8, "lit plus stack_value"); }
    /* breg0 0 ; deref : mem[0x11]→ out of range → 错误；改为 addr 0x1000 deref */
    { uint8_t e[] = {0x03, 0x00, 0x10, 0, 0, 0x06, 0x9f}; ctx_arm(&c);
      g_mem[0] = 0x78; g_mem[1] = 0x56; g_mem[2] = 0x34; g_mem[3] = 0x12;
      dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_VALUE && r.bytes[0] == 0x78 && r.bytes[3] == 0x12, "deref"); }
    /* pieces: reg0 piece2, reg1 piece2 */
    { uint8_t e[] = {0x50, 0x93, 0x02, 0x51, 0x93, 0x02}; ctx_arm(&c); dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_PIECES && r.len == 4 && r.bytes[0] == 0x11 && r.bytes[2] == 0x22, "pieces"); }
    /* implicit_value 2 bytes */
    { uint8_t e[] = {0x9e, 0x02, 0xAB, 0xCD}; ctx_arm(&c); dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_VALUE && r.len == 2 && r.bytes[1] == 0xCD, "implicit_value"); }
    /* 未知寄存器 → 错误 */
    { uint8_t e[] = {0x55}; ctx_arm(&c); dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_REG || r.kind == DWLOC_ERROR, "reg5 kind");
      CHECK(dwloc_materialize(&c, &r, buf, 4) != 0, "reg5 invalid → materialize fails"); }
    /* entry_value 不支持 */
    { uint8_t e[] = {0xa3, 0x01, 0x50, 0x9f}; ctx_arm(&c); dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_ERROR, "entry_value unsupported"); }
    /* TriCore: E2 = D3:D2 (reg 33) */
    memset(&c, 0, sizeof(c)); c.arch = 2; c.addr_size = 4; c.le = 1;
    c.reg[2] = 0x89ABCDEF; c.reg_valid[2] = 1; c.reg[3] = 0x01234567; c.reg_valid[3] = 1;
    CHECK(dwloc_reg_value(&c, 33, &v, &sz) == 0 && v == 0x0123456789ABCDEFull && sz == 8, "tricore E2");
    /* TriCore breg26 (A10/SP) +24 */
    { uint8_t e[] = {0x8a, 0x18}; c.reg[26] = 0x70019F00; c.reg_valid[26] = 1; dwloc_eval(e, sizeof(e), &c, &r);
      CHECK(r.kind == DWLOC_MEM && r.addr == 0x70019F18, "tricore breg26+24"); }

    /* .debug_loc v4: base-select(0x8000_0000) ; [0x10,0x20) reg0 ; [0x20,0x30) reg1 ; end */
    {
        uint8_t s[64]; int o = 0; const uint8_t* ex; size_t el;
#define W32(x) do { uint32_t _x = (x); memcpy(s + o, &_x, 4); o += 4; } while (0)
        W32(0xFFFFFFFF); W32(0x80000000);
        W32(0x10); W32(0x20); s[o++] = 1; s[o++] = 0; s[o++] = 0x50;
        W32(0x20); W32(0x30); s[o++] = 1; s[o++] = 0; s[o++] = 0x51;
        W32(0); W32(0);
        CHECK(dwloc_loclist_v4(s, o, 0, 4, 1, 0, 0x80000018, &ex, &el) == 1 && el == 1 && ex[0] == 0x50, "loclist v4 base-select hit1");
        CHECK(dwloc_loclist_v4(s, o, 0, 4, 1, 0, 0x80000020, &ex, &el) == 1 && ex[0] == 0x51, "loclist v4 hit2");
        CHECK(dwloc_loclist_v4(s, o, 0, 4, 1, 0, 0x80000030, &ex, &el) == 0, "loclist v4 miss");
        /* 同表也当作 ranges（v4 格式同构，表达式字节不同；单独造一个） */
        o = 0;
        W32(0x100); W32(0x200); W32(0x300); W32(0x380); W32(0); W32(0);
        CHECK(dwloc_ranges_v4_contains(s, o, 0, 4, 1, 0x08000000, 0x08000150) == 1, "ranges v4 hit");
        CHECK(dwloc_ranges_v4_contains(s, o, 0, 4, 1, 0x08000000, 0x08000250) == 0, "ranges v4 gap");
        CHECK(dwloc_ranges_v4_contains(s, o, 0, 4, 1, 0x08000000, 0x08000300) == 1, "ranges v4 hit2");
    }
    /* .debug_loclists v5: DW_LLE_offset_pair(4) 0x10 0x20 len1 reg2 ; DW_LLE_end_of_list(0) */
    {
        uint8_t s[] = {0x04, 0x10, 0x20, 0x01, 0x52, 0x00};
        const uint8_t* ex; size_t el;
        CHECK(dwloc_loclist_v5(s, sizeof(s), 0, 4, 1, 0x1000, NULL, 0, 0, 0x1015, &ex, &el) == 1 && ex[0] == 0x52, "loclist v5");
        CHECK(dwloc_loclist_v5(s, sizeof(s), 0, 4, 1, 0x1000, NULL, 0, 0, 0x1025, &ex, &el) == 0, "loclist v5 miss");
    }
    /* .debug_rnglists v5: DW_RLE_offset_pair(4) 0x0 0x40 ; end */
    {
        uint8_t s[] = {0x04, 0x00, 0x40, 0x00};
        CHECK(dwloc_ranges_v5_contains(s, sizeof(s), 0, 4, 1, 0x2000, NULL, 0, 0, 0x2010) == 1, "rnglists v5 hit");
        CHECK(dwloc_ranges_v5_contains(s, sizeof(s), 0, 4, 1, 0x2000, NULL, 0, 0, 0x2040) == 0, "rnglists v5 miss");
    }
    /* .debug_frame: CIE(def_cfa r13+0) + FDE [0x08000100,+0x40): advance_loc 2, def_cfa_offset 8 */
    {
        uint8_t s[128]; int o = 0, cie_len_at, fde_len_at;
        int cr; int64_t co;
        cie_len_at = o; W32(0); W32(0xFFFFFFFF); s[o++] = 1; s[o++] = 0; /* version, aug "" */
        s[o++] = 2; /* code align */ s[o++] = 0x7c; /* data align -4 */ s[o++] = 14; /* RA col */
        s[o++] = 0x0c; s[o++] = 13; s[o++] = 0; /* def_cfa r13, 0 */
        while ((o - cie_len_at - 4) % 4) s[o++] = 0;
        { uint32_t L = (uint32_t)(o - cie_len_at - 4); memcpy(s + cie_len_at, &L, 4); }
        fde_len_at = o; W32(0); W32((uint32_t)cie_len_at); W32(0x08000100); W32(0x40);
        s[o++] = 0x41; /* advance_loc 1*2 */ s[o++] = 0x0e; s[o++] = 8; /* def_cfa_offset 8 */
        while ((o - fde_len_at - 4) % 4) s[o++] = 0;
        { uint32_t L = (uint32_t)(o - fde_len_at - 4); memcpy(s + fde_len_at, &L, 4); }
        CHECK(dwloc_cfa_rule(s, o, 4, 1, 0x08000100, &cr, &co) == 1 && cr == 13 && co == 0, "cfa at entry");
        CHECK(dwloc_cfa_rule(s, o, 4, 1, 0x08000110, &cr, &co) == 1 && cr == 13 && co == 8, "cfa after push");
        CHECK(dwloc_cfa_rule(s, o, 4, 1, 0x08000200, &cr, &co) == 0, "cfa outside FDE");
    }
    if (g_fail) { printf("dwloc_test: %d/%d FAILED\n", g_fail, g_n); return 1; }
    printf("dwloc_test: %d checks ALL PASS\n", g_n);
    return 0;
}
