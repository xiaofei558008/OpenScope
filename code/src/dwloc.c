/* DWARF 位置表达式 / 位置列表 / CFA —— 见 dwloc.h */
#include "dwloc.h"
#include <string.h>
#include <stdio.h>

static uint64_t rd_n(const uint8_t* p, int n, int le)
{
    uint64_t v = 0;
    int i;
    if (le) for (i = n - 1; i >= 0; i--) v = (v << 8) | p[i];
    else    for (i = 0; i < n; i++) v = (v << 8) | p[i];
    return v;
}

static int uleb(const uint8_t** pp, const uint8_t* end, uint64_t* out)
{
    uint64_t v = 0;
    int shift = 0;
    const uint8_t* p = *pp;
    while (p < end) {
        uint8_t b = *p++;
        if (shift < 64) v |= (uint64_t)(b & 0x7f) << shift;
        shift += 7;
        if (!(b & 0x80)) { *pp = p; *out = v; return 0; }
    }
    return -1;
}

static int sleb(const uint8_t** pp, const uint8_t* end, int64_t* out)
{
    int64_t v = 0;
    int shift = 0;
    uint8_t b = 0;
    const uint8_t* p = *pp;
    while (p < end) {
        b = *p++;
        if (shift < 64) v |= (int64_t)((uint64_t)(b & 0x7f) << shift);
        shift += 7;
        if (!(b & 0x80)) {
            if (shift < 64 && (b & 0x40)) v |= -((int64_t)1 << shift);
            *pp = p; *out = v; return 0;
        }
    }
    return -1;
}

int dwloc_reg_value(const DwLocCtx* c, int reg, uint64_t* out, int* size_bytes)
{
    if (!c || reg < 0) return -1;
    if (c->arch == 2 && reg >= 32 && reg <= 39) {
        int lo = (reg - 32) * 2, hi = lo + 1;
        if (!c->reg_valid[lo] || !c->reg_valid[hi]) return -1;
        *out = (c->reg[lo] & 0xffffffffu) | ((c->reg[hi] & 0xffffffffu) << 32);
        if (size_bytes) *size_bytes = 8;
        return 0;
    }
    if (reg >= DWLOC_MAX_REGS || !c->reg_valid[reg]) return -1;
    *out = c->reg[reg];
    if (size_bytes) *size_bytes = c->addr_size ? c->addr_size : 4;
    return 0;
}

#define STK 64
#define FAIL(msg) do { snprintf(out->err, sizeof(out->err), "%s", msg); out->kind = DWLOC_ERROR; return; } while (0)
#define NEED(n) do { if (sp < (n)) FAIL("stack underflow"); } while (0)
#define PUSH(v) do { uint64_t pv_ = (uint64_t)(v); if (sp >= STK) FAIL("stack overflow"); st[sp] = pv_; sp++; } while (0)

static void piece_put(DwLocResult* out, int* off, const uint8_t* src, int nbytes)
{
    int n = nbytes;
    if (*off + n > (int)sizeof(out->bytes)) n = (int)sizeof(out->bytes) - *off;
    if (n > 0) {
        if (src) memcpy(out->bytes + *off, src, (size_t)n);
        else { memset(out->bytes + *off, 0, (size_t)n); out->partial = 1; }
    }
    *off += nbytes;
}

void dwloc_eval(const uint8_t* expr, size_t len, const DwLocCtx* c, DwLocResult* out)
{
    uint64_t st[STK];
    int sp = 0;
    const uint8_t* p = expr;
    const uint8_t* end = expr + len;
    int as = (c && c->addr_size) ? c->addr_size : 4;
    int le = c ? c->le : 1;
    uint64_t amask = as >= 8 ? ~(uint64_t)0 : (((uint64_t)1 << (as * 8)) - 1);
    /* 当前“位置”状态：0 = 栈顶为地址，1 = 寄存器 reg_loc，2 = 隐式值（stack_value），3 = implicit_value 字节 */
    int loc_state = 0, reg_loc = -1;
    int pieces = 0, pc_off = 0;
    memset(out, 0, sizeof(*out));
    out->kind = DWLOC_NONE;
    if (!expr || len == 0) return;
    while (p < end) {
        uint8_t op = *p++;
        uint64_t u = 0;
        int64_t s = 0;
        if (op >= 0x30 && op <= 0x4f) { PUSH(op - 0x30); continue; }          /* lit0..31 */
        if (op >= 0x50 && op <= 0x6f) { loc_state = 1; reg_loc = op - 0x50; continue; } /* reg0..31 */
        if (op >= 0x70 && op <= 0x8f) {                                        /* breg0..31 */
            uint64_t rv;
            if (sleb(&p, end, &s)) FAIL("bad sleb");
            if (dwloc_reg_value(c, op - 0x70, &rv, NULL)) FAIL("reg unavailable");
            PUSH((rv + (uint64_t)s) & amask);
            continue;
        }
        switch (op) {
        case 0x03: /* addr */
            if (p + as > end) FAIL("trunc");
            PUSH(rd_n(p, as, le)); p += as; break;
        case 0x06: { /* deref */
            uint8_t b[8] = {0};
            NEED(1);
            if (!c || !c->read_mem || c->read_mem(c->user, st[sp - 1], b, (uint32_t)as)) FAIL("deref read failed");
            st[sp - 1] = rd_n(b, as, 1);
            break;
        }
        case 0x08: if (p + 1 > end) FAIL("trunc"); PUSH(p[0]); p += 1; break;
        case 0x09: if (p + 1 > end) FAIL("trunc"); PUSH((int64_t)(int8_t)p[0]); p += 1; break;
        case 0x0a: if (p + 2 > end) FAIL("trunc"); PUSH(rd_n(p, 2, le)); p += 2; break;
        case 0x0b: if (p + 2 > end) FAIL("trunc"); PUSH((int64_t)(int16_t)rd_n(p, 2, le)); p += 2; break;
        case 0x0c: if (p + 4 > end) FAIL("trunc"); PUSH(rd_n(p, 4, le)); p += 4; break;
        case 0x0d: if (p + 4 > end) FAIL("trunc"); PUSH((int64_t)(int32_t)rd_n(p, 4, le)); p += 4; break;
        case 0x0e: case 0x0f: if (p + 8 > end) FAIL("trunc"); PUSH(rd_n(p, 8, le)); p += 8; break;
        case 0x10: if (uleb(&p, end, &u)) FAIL("bad uleb"); PUSH(u); break;
        case 0x11: if (sleb(&p, end, &s)) FAIL("bad sleb"); PUSH(s); break;
        case 0x12: NEED(1); PUSH(st[sp - 1]); break;                       /* dup */
        case 0x13: NEED(1); sp--; break;                                     /* drop */
        case 0x14: NEED(2); PUSH(st[sp - 2]); break;                       /* over */
        case 0x15: { uint8_t i; if (p >= end) FAIL("trunc"); i = *p++; if (sp <= i) FAIL("pick"); PUSH(st[sp - 1 - i]); break; }
        case 0x16: { uint64_t t; NEED(2); t = st[sp - 1]; st[sp - 1] = st[sp - 2]; st[sp - 2] = t; break; } /* swap */
        case 0x17: { uint64_t t; NEED(3); t = st[sp - 1]; st[sp - 1] = st[sp - 2]; st[sp - 2] = st[sp - 3]; st[sp - 3] = t; break; } /* rot */
        case 0x19: NEED(1); if ((int64_t)st[sp - 1] < 0) st[sp - 1] = (uint64_t)(-(int64_t)st[sp - 1]); break; /* abs */
        case 0x1a: NEED(2); st[sp - 2] &= st[sp - 1]; sp--; break;
        case 0x1b: NEED(2); if (!st[sp - 1]) FAIL("div0"); st[sp - 2] = (uint64_t)((int64_t)st[sp - 2] / (int64_t)st[sp - 1]); sp--; break;
        case 0x1c: NEED(2); st[sp - 2] -= st[sp - 1]; sp--; break;          /* minus */
        case 0x1d: NEED(2); if (!st[sp - 1]) FAIL("mod0"); st[sp - 2] %= st[sp - 1]; sp--; break;
        case 0x1e: NEED(2); st[sp - 2] *= st[sp - 1]; sp--; break;
        case 0x1f: NEED(1); st[sp - 1] = (uint64_t)(-(int64_t)st[sp - 1]); break;
        case 0x20: NEED(1); st[sp - 1] = ~st[sp - 1]; break;
        case 0x21: NEED(2); st[sp - 2] |= st[sp - 1]; sp--; break;
        case 0x22: NEED(2); st[sp - 2] += st[sp - 1]; sp--; break;          /* plus */
        case 0x23: NEED(1); if (uleb(&p, end, &u)) FAIL("bad uleb"); st[sp - 1] += u; break; /* plus_uconst */
        case 0x24: NEED(2); st[sp - 2] = st[sp - 1] >= 64 ? 0 : st[sp - 2] << st[sp - 1]; sp--; break;
        case 0x25: NEED(2); st[sp - 2] = st[sp - 1] >= 64 ? 0 : st[sp - 2] >> st[sp - 1]; sp--; break;
        case 0x26: NEED(2); st[sp - 2] = (uint64_t)((int64_t)st[sp - 2] >> (st[sp - 1] >= 63 ? 63 : st[sp - 1])); sp--; break;
        case 0x27: NEED(2); st[sp - 2] ^= st[sp - 1]; sp--; break;
        case 0x96: break;                                                    /* nop */
        case 0x90: /* regx */
            if (uleb(&p, end, &u)) FAIL("bad uleb");
            loc_state = 1; reg_loc = (int)u; break;
        case 0x91: /* fbreg */
            if (sleb(&p, end, &s)) FAIL("bad sleb");
            if (!c || !c->have_frame_base) FAIL("no frame base");
            PUSH((c->frame_base + (uint64_t)s) & amask); break;
        case 0x92: { /* bregx */
            uint64_t rv;
            if (uleb(&p, end, &u) || sleb(&p, end, &s)) FAIL("bad leb");
            if (dwloc_reg_value(c, (int)u, &rv, NULL)) FAIL("reg unavailable");
            PUSH((rv + (uint64_t)s) & amask); break;
        }
        case 0x94: { /* deref_size */
            uint8_t n, b[8] = {0};
            if (p >= end) FAIL("trunc");
            n = *p++;
            NEED(1);
            if (n == 0 || n > 8) FAIL("deref_size");
            if (!c || !c->read_mem || c->read_mem(c->user, st[sp - 1], b, n)) FAIL("deref read failed");
            st[sp - 1] = rd_n(b, n, 1);
            break;
        }
        case 0x9c: /* call_frame_cfa */
            if (!c || !c->have_cfa) FAIL("no CFA");
            PUSH(c->cfa); break;
        case 0x9e: { /* implicit_value */
            if (uleb(&p, end, &u) || p + u > end) FAIL("bad implicit_value");
            out->len = (int)(u > sizeof(out->bytes) ? sizeof(out->bytes) : u);
            memcpy(out->bytes, p, (size_t)out->len);
            p += u;
            loc_state = 3;
            break;
        }
        case 0x9f: loc_state = 2; break;                                     /* stack_value */
        case 0x93: case 0x9d: { /* piece / bit_piece：仅支持字节对齐 */
            uint64_t sz = 0, boff = 0;
            int nbytes;
            if (uleb(&p, end, &sz)) FAIL("bad uleb");
            if (op == 0x9d) { if (uleb(&p, end, &boff)) FAIL("bad uleb"); if (sz % 8 || boff % 8) FAIL("bit_piece unaligned"); sz /= 8; boff /= 8; }
            nbytes = (int)sz;
            pieces++;
            if (loc_state == 1) {
                uint64_t rv; int rs = 0; uint8_t b[8];
                int i;
                if (dwloc_reg_value(c, reg_loc, &rv, &rs) == 0) {
                    rv >>= (boff * 8);
                    for (i = 0; i < 8; i++) b[i] = (uint8_t)(rv >> (8 * i));
                    piece_put(out, &pc_off, b, nbytes > 8 ? 8 : nbytes);
                    if (nbytes > 8) piece_put(out, &pc_off, NULL, nbytes - 8);
                } else piece_put(out, &pc_off, NULL, nbytes);
            } else if (loc_state == 2) {
                uint64_t v; uint8_t b[8]; int i;
                if (sp < 1) FAIL("stack underflow");
                v = st[--sp] >> (boff * 8);
                for (i = 0; i < 8; i++) b[i] = (uint8_t)(v >> (8 * i));
                piece_put(out, &pc_off, b, nbytes > 8 ? 8 : nbytes);
            } else if (sp > 0) {
                uint8_t b[16];
                uint64_t a = st[--sp] + boff;
                int n = nbytes > 16 ? 16 : nbytes;
                if (c && c->read_mem && c->read_mem(c->user, a, b, (uint32_t)n) == 0) piece_put(out, &pc_off, b, n);
                else piece_put(out, &pc_off, NULL, n);
            } else {
                piece_put(out, &pc_off, NULL, nbytes); /* 空片 = 优化掉 */
            }
            loc_state = 0; reg_loc = -1;
            break;
        }
        case 0xf3: case 0xa3: /* GNU_entry_value / entry_value：调用方值，帧 0 无法可靠求得 */
            FAIL("entry_value");
        default: {
            char m[32];
            snprintf(m, sizeof(m), "unsupported op 0x%02x", op);
            FAIL(m);
        }
        }
    }
    if (pieces) {
        out->kind = DWLOC_PIECES;
        out->len = pc_off > (int)sizeof(out->bytes) ? (int)sizeof(out->bytes) : pc_off;
        return;
    }
    if (loc_state == 1) { out->kind = DWLOC_REG; out->reg = reg_loc; return; }
    if (loc_state == 3) { out->kind = DWLOC_VALUE; return; }
    if (loc_state == 2) {
        uint64_t v; int i;
        if (sp < 1) FAIL("stack underflow");
        v = st[sp - 1];
        for (i = 0; i < 8; i++) out->bytes[i] = (uint8_t)(v >> (8 * i));
        out->len = 8; out->kind = DWLOC_VALUE; return;
    }
    if (sp < 1) FAIL("empty");
    out->kind = DWLOC_MEM;
    out->addr = st[sp - 1];
}

int dwloc_materialize(const DwLocCtx* c, const DwLocResult* r, uint8_t* buf, uint32_t size)
{
    uint64_t v;
    int rs = 0;
    uint32_t i;
    if (!r || !buf) return -1;
    memset(buf, 0, size);
    switch (r->kind) {
    case DWLOC_REG:
        if (dwloc_reg_value(c, r->reg, &v, &rs)) return -1;
        for (i = 0; i < size && i < 8; i++) buf[i] = (uint8_t)(v >> (8 * i));
        if (size > 8) return -1; /* 跨寄存器的大对象需 piece */
        return 0;
    case DWLOC_VALUE:
    case DWLOC_PIECES:
        memcpy(buf, r->bytes, size < (uint32_t)r->len ? size : (uint32_t)r->len);
        return (size <= (uint32_t)r->len) ? 0 : -1;
    case DWLOC_MEM:
        if (!c || !c->read_mem) return -1;
        return c->read_mem(c->user, r->addr, buf, size) ? -1 : 0;
    default:
        return -1;
    }
}

/* ---------------- 位置列表 ---------------- */

int dwloc_loclist_v4(const uint8_t* sec, size_t sec_size, uint64_t off, int addr_size, int le,
                     uint64_t cu_base, uint64_t pc, const uint8_t** expr, size_t* len)
{
    const uint8_t* p;
    const uint8_t* end = sec + sec_size;
    uint64_t base = cu_base;
    uint64_t maxa = addr_size >= 8 ? ~(uint64_t)0 : (((uint64_t)1 << (addr_size * 8)) - 1);
    if (!sec || off >= sec_size) return -1;
    p = sec + off;
    while (p + 2 * addr_size <= end) {
        uint64_t b = rd_n(p, addr_size, le), e = rd_n(p + addr_size, addr_size, le);
        uint32_t n;
        p += 2 * addr_size;
        if (b == 0 && e == 0) return 0;               /* end of list */
        if (b == maxa) { base = e; continue; }        /* base address selection */
        if (p + 2 > end) return -1;
        n = (uint32_t)rd_n(p, 2, le);
        p += 2;
        if (p + n > end) return -1;
        if (pc >= base + b && pc < base + e) { *expr = p; *len = n; return 1; }
        p += n;
    }
    return -1;
}

static int addrx(const uint8_t* da, size_t das, uint64_t base, uint64_t idx, int as, int le, uint64_t* out)
{
    uint64_t pos = base + idx * (uint64_t)as;
    if (!da || pos + (uint64_t)as > das) return -1;
    *out = rd_n(da + pos, as, le);
    return 0;
}

int dwloc_loclist_v5(const uint8_t* sec, size_t sec_size, uint64_t off, int addr_size, int le,
                     uint64_t cu_base, const uint8_t* debug_addr, size_t debug_addr_size,
                     uint64_t addr_base, uint64_t pc, const uint8_t** expr, size_t* len)
{
    const uint8_t* p;
    const uint8_t* end = sec + sec_size;
    uint64_t base = cu_base;
    const uint8_t* dflt = NULL;
    size_t dflt_len = 0;
    if (!sec || off >= sec_size) return -1;
    p = sec + off;
    while (p < end) {
        uint8_t k = *p++;
        uint64_t a = 0, b = 0, n = 0;
        int have = 0;
        switch (k) {
        case 0x00: /* end_of_list */
            if (dflt) { *expr = dflt; *len = dflt_len; return 1; }
            return 0;
        case 0x01: /* base_addressx */
            if (uleb(&p, end, &a) || addrx(debug_addr, debug_addr_size, addr_base, a, addr_size, le, &base)) return -1;
            continue;
        case 0x02: /* startx_endx */
            if (uleb(&p, end, &a) || uleb(&p, end, &b)) return -1;
            if (addrx(debug_addr, debug_addr_size, addr_base, a, addr_size, le, &a) ||
                addrx(debug_addr, debug_addr_size, addr_base, b, addr_size, le, &b)) return -1;
            have = 1; break;
        case 0x03: /* startx_length */
            if (uleb(&p, end, &a) || uleb(&p, end, &b)) return -1;
            if (addrx(debug_addr, debug_addr_size, addr_base, a, addr_size, le, &a)) return -1;
            b += a; have = 1; break;
        case 0x04: /* offset_pair */
            if (uleb(&p, end, &a) || uleb(&p, end, &b)) return -1;
            a += base; b += base; have = 1; break;
        case 0x05: /* default_location */
            if (uleb(&p, end, &n) || p + n > end) return -1;
            dflt = p; dflt_len = (size_t)n; p += n;
            continue;
        case 0x06: /* base_address */
            if (p + addr_size > end) return -1;
            base = rd_n(p, addr_size, le); p += addr_size; continue;
        case 0x07: /* start_end */
            if (p + 2 * addr_size > end) return -1;
            a = rd_n(p, addr_size, le); b = rd_n(p + addr_size, addr_size, le); p += 2 * addr_size;
            have = 1; break;
        case 0x08: /* start_length */
            if (p + addr_size > end) return -1;
            a = rd_n(p, addr_size, le); p += addr_size;
            if (uleb(&p, end, &b)) return -1;
            b += a; have = 1; break;
        default:
            return -1;
        }
        if (have) {
            if (uleb(&p, end, &n) || p + n > end) return -1;
            if (pc >= a && pc < b) { *expr = p; *len = (size_t)n; return 1; }
            p += n;
        }
    }
    return -1;
}

int dwloc_ranges_v4_contains(const uint8_t* sec, size_t sec_size, uint64_t off, int addr_size, int le,
                             uint64_t cu_base, uint64_t pc)
{
    const uint8_t* p;
    const uint8_t* end = sec + sec_size;
    uint64_t base = cu_base;
    uint64_t maxa = addr_size >= 8 ? ~(uint64_t)0 : (((uint64_t)1 << (addr_size * 8)) - 1);
    if (!sec || off >= sec_size) return 0;
    p = sec + off;
    while (p + 2 * addr_size <= end) {
        uint64_t b = rd_n(p, addr_size, le), e = rd_n(p + addr_size, addr_size, le);
        p += 2 * addr_size;
        if (b == 0 && e == 0) return 0;
        if (b == maxa) { base = e; continue; }
        if (pc >= base + b && pc < base + e) return 1;
    }
    return 0;
}

int dwloc_ranges_v5_contains(const uint8_t* sec, size_t sec_size, uint64_t off, int addr_size, int le,
                             uint64_t cu_base, const uint8_t* debug_addr, size_t debug_addr_size,
                             uint64_t addr_base, uint64_t pc)
{
    const uint8_t* p;
    const uint8_t* end = sec + sec_size;
    uint64_t base = cu_base;
    if (!sec || off >= sec_size) return 0;
    p = sec + off;
    while (p < end) {
        uint8_t k = *p++;
        uint64_t a = 0, b = 0;
        switch (k) {
        case 0x00: return 0;
        case 0x01: if (uleb(&p, end, &a) || addrx(debug_addr, debug_addr_size, addr_base, a, addr_size, le, &base)) return 0; continue;
        case 0x02: if (uleb(&p, end, &a) || uleb(&p, end, &b)) return 0;
                   if (addrx(debug_addr, debug_addr_size, addr_base, a, addr_size, le, &a) ||
                       addrx(debug_addr, debug_addr_size, addr_base, b, addr_size, le, &b)) return 0;
                   break;
        case 0x03: if (uleb(&p, end, &a) || uleb(&p, end, &b)) return 0;
                   if (addrx(debug_addr, debug_addr_size, addr_base, a, addr_size, le, &a)) return 0;
                   b += a; break;
        case 0x04: if (uleb(&p, end, &a) || uleb(&p, end, &b)) return 0; a += base; b += base; break;
        case 0x05: if (p + addr_size > end) return 0; base = rd_n(p, addr_size, le); p += addr_size; continue;
        case 0x06: if (p + 2 * addr_size > end) return 0; a = rd_n(p, addr_size, le); b = rd_n(p + addr_size, addr_size, le); p += 2 * addr_size; break;
        case 0x07: if (p + addr_size > end) return 0; a = rd_n(p, addr_size, le); p += addr_size; if (uleb(&p, end, &b)) return 0; b += a; break;
        default: return 0;
        }
        if (pc >= a && pc < b) return 1;
    }
    return 0;
}

/* ---------------- CFA（.debug_frame） ---------------- */

typedef struct { int reg; int64_t off; int valid; } CfaState;

/* 执行 CFA 指令直到 loc > pc。仅跟踪 CFA 规则；其它寄存器规则只解析跳过。 */
static int cfa_run(const uint8_t* p, const uint8_t* end, int addr_size, int le, uint64_t caf, int64_t daf,
                   uint64_t* loc, uint64_t pc, CfaState* cs, const CfaState* init)
{
    CfaState stack[8];
    int ssp = 0;
    while (p < end) {
        uint8_t op = *p++;
        uint8_t hi = op & 0xc0, lo6 = op & 0x3f;
        uint64_t u = 0, u2 = 0;
        int64_t s = 0;
        if (hi == 0x40) { *loc += lo6 * caf; if (*loc > pc) return 1; continue; }   /* advance_loc */
        if (hi == 0x80) { if (uleb(&p, end, &u)) return -1; continue; }            /* offset */
        if (hi == 0xc0) continue;                                                   /* restore */
        switch (op) {
        case 0x00: break;                                                           /* nop */
        case 0x01: if (p + addr_size > end) return -1; *loc = rd_n(p, addr_size, le); p += addr_size; if (*loc > pc) return 1; break;
        case 0x02: if (p + 1 > end) return -1; *loc += p[0] * caf; p += 1; if (*loc > pc) return 1; break;
        case 0x03: if (p + 2 > end) return -1; *loc += rd_n(p, 2, le) * caf; p += 2; if (*loc > pc) return 1; break;
        case 0x04: if (p + 4 > end) return -1; *loc += rd_n(p, 4, le) * caf; p += 4; if (*loc > pc) return 1; break;
        case 0x05: if (uleb(&p, end, &u) || uleb(&p, end, &u2)) return -1; break;   /* offset_extended */
        case 0x06: case 0x07: case 0x08: if (uleb(&p, end, &u)) return -1; break;  /* restore_ext/undefined/same_value */
        case 0x09: if (uleb(&p, end, &u) || uleb(&p, end, &u2)) return -1; break;   /* register */
        case 0x0a: if (ssp < 8) stack[ssp++] = *cs; break;                          /* remember_state */
        case 0x0b: if (ssp > 0) *cs = stack[--ssp]; else if (init) *cs = *init; break; /* restore_state */
        case 0x0c: if (uleb(&p, end, &u) || uleb(&p, end, &u2)) return -1; cs->reg = (int)u; cs->off = (int64_t)u2; cs->valid = 1; break;
        case 0x0d: if (uleb(&p, end, &u)) return -1; cs->reg = (int)u; cs->valid = 1; break;
        case 0x0e: if (uleb(&p, end, &u)) return -1; cs->off = (int64_t)u; break;
        case 0x0f: if (uleb(&p, end, &u) || p + u > end) return -1; p += u; cs->valid = 0; break; /* def_cfa_expression */
        case 0x10: if (uleb(&p, end, &u) || uleb(&p, end, &u2) || p + u2 > end) return -1; p += u2; break; /* expression */
        case 0x11: if (uleb(&p, end, &u) || sleb(&p, end, &s)) return -1; break;     /* offset_extended_sf */
        case 0x12: if (uleb(&p, end, &u) || sleb(&p, end, &s)) return -1; cs->reg = (int)u; cs->off = s * daf; cs->valid = 1; break;
        case 0x13: if (sleb(&p, end, &s)) return -1; cs->off = s * daf; break;
        case 0x14: if (uleb(&p, end, &u) || uleb(&p, end, &u2)) return -1; break;   /* val_offset */
        case 0x15: if (uleb(&p, end, &u) || sleb(&p, end, &s)) return -1; break;     /* val_offset_sf */
        case 0x16: if (uleb(&p, end, &u) || uleb(&p, end, &u2) || p + u2 > end) return -1; p += u2; break; /* val_expression */
        case 0x2e: if (uleb(&p, end, &u)) return -1; break;                          /* GNU_args_size */
        case 0x2f: if (uleb(&p, end, &u) || uleb(&p, end, &u2)) return -1; break;
        default: return -1;
        }
    }
    return 0;
}

int dwloc_cfa_rule(const uint8_t* frame, size_t frame_size, int addr_size, int le, uint64_t pc,
                   int* cfa_reg, int64_t* cfa_off)
{
    const uint8_t* p = frame;
    const uint8_t* end = frame + frame_size;
    if (!frame || !frame_size) return 0;
    while (p + 4 <= end) {
        uint64_t len = rd_n(p, 4, le);
        const uint8_t* ent = p + 4;
        const uint8_t* ent_end;
        uint64_t cie_id;
        if (len == 0) { p += 4; continue; }
        if (len == 0xffffffffu || ent + len > end) return 0;
        ent_end = ent + len;
        cie_id = rd_n(ent, 4, le);
        if (cie_id != 0xffffffffu) {
            /* FDE */
            const uint8_t* q = ent + 4;
            uint64_t ploc, prange;
            if (q + 2 * addr_size > ent_end) { p = ent_end; continue; }
            ploc = rd_n(q, addr_size, le); prange = rd_n(q + addr_size, addr_size, le);
            q += 2 * addr_size;
            if (pc >= ploc && pc < ploc + prange && cie_id + 4 <= frame_size) {
                const uint8_t* cie = frame + cie_id;
                uint64_t clen = rd_n(cie, 4, le);
                const uint8_t* c = cie + 8;
                const uint8_t* cend = cie + 4 + clen;
                uint8_t ver;
                uint64_t caf = 1, ra = 0, loc;
                int64_t daf = 1;
                CfaState cs, init;
                if (cend > end || c >= cend) return 0;
                ver = *c++;
                while (c < cend && *c) c++;          /* augmentation string */
                if (c >= cend) return 0;
                if (cie[8 + 1] != 0) return 0;       /* 不支持增强（.eh_frame 风格） */
                c++;
                if (ver >= 4) c += 2;                /* address_size, segment_size */
                if (uleb(&c, cend, &caf) || sleb(&c, cend, &daf)) return 0;
                if (ver == 1) c++; else if (uleb(&c, cend, &ra)) return 0;
                memset(&cs, 0, sizeof(cs));
                loc = ploc;
                if (cfa_run(c, cend, addr_size, le, caf, daf, &loc, ~(uint64_t)0, &cs, NULL) < 0) return 0;
                init = cs;
                loc = ploc;
                if (cfa_run(q, ent_end, addr_size, le, caf, daf, &loc, pc, &cs, &init) < 0) return 0;
                if (!cs.valid) return 0;
                *cfa_reg = cs.reg; *cfa_off = cs.off;
                return 1;
            }
        }
        p = ent_end;
    }
    return 0;
}
