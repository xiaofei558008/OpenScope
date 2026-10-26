/*
 * 局部变量窗口与求值引擎 —— 见 locals.h
 */
#include "app.h"
#include "locals.h"
#include "elf.h"
#include "dwloc.h"
#include "util.h"
#include "theme.h"
#include "mainwin.h"
#include <commctrl.h>
#include <stdio.h>
#include <string.h>

#define LOC_MAX_SYMS     128
#define LOC_MAX_ROWS     1024
#define LOC_MAX_BYTES    4096
#define LOC_ARRAY_SHOW   64
#define LOC_MAX_DEPTH    6

/* ------------------------------------------------------------------ */
/* 引擎                                                                */
/* ------------------------------------------------------------------ */

static int s_read_busy;   /* Bug4：本次快照中有读返回 OS_ERR_AGAIN（仅 UI 线程） */

static int drv_read(void* user, uint64_t addr, void* buf, uint32_t size)
{
    OS_MemReq r;
    (void)user;
    if (!g_app.driver || !g_app.driver->command || size == 0) return -1;
    memset(&r, 0, sizeof(r));
    r.address = addr;
    r.size = size;
    r.data = buf;
    {
        int rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_READ_MEM, &r, NULL);
        if (rc == OS_ERR_AGAIN) s_read_busy = 1;   /* Bug4 */
        return rc < 0 ? -1 : 0;
    }
}

static OS_LocRow* row_add(OS_LocSnap* s)
{
    OS_LocRow* r;
    if (s->nrows >= LOC_MAX_ROWS) return NULL;
    if (s->nrows == s->caprows) {
        int nc = s->caprows ? s->caprows * 2 : 64;
        OS_LocRow* n = (OS_LocRow*)realloc(s->rows, (size_t)nc * sizeof(OS_LocRow));
        if (!n) return NULL;
        s->rows = n;
        s->caprows = nc;
    }
    r = &s->rows[s->nrows++];
    memset(r, 0, sizeof(*r));
    return r;
}

static void fmt_scalar(const OS_Type* t, const uint8_t* raw, int avail, char* out, int cap)
{
    OS_EnumVal ev[32];
    int nev = 0, i;
    if (!t) { snprintf(out, cap, "<无类型>"); return; }
    if ((int)t->size > avail || t->size == 0) {
        if (t->is_bitfield && avail > 0) { /* 位域：存储单元可能大于 size */ }
        else { snprintf(out, cap, "<不可读>"); return; }
    }
    if (t->kind == OS_TYPE_ENUM) {
        for (i = 0; i < t->child_count && nev < 32; i++) {
            const OS_Type* c = t->children[i];
            if (!c) continue;
            snprintf(ev[nev].name, sizeof(ev[nev].name), "%s", c->name ? c->name : "?");
            ev[nev].value = c->enum_value;
            nev++;
        }
    }
    os_format_raw(out, cap, raw, (int)t->size, t->kind, t->is_signed, t->is_ptr,
                  t->is_bitfield, t->bit_offset, t->bit_size, NULL, ev, nev);
    if (t->kind == OS_TYPE_INT || t->kind == OS_TYPE_UINT) {
        /* 附十六进制，便于寄存器值对照 */
        size_t l = strlen(out);
        uint64_t v = 0;
        int k;
        if (!t->is_bitfield && !t->is_ptr && t->size <= 8 && (int)l + 20 < cap) {
            for (k = (int)t->size - 1; k >= 0; k--) v = (v << 8) | raw[k];
            if (v > 9) snprintf(out + l, cap - (int)l, " (0x%llX)", (unsigned long long)v);
        }
    }
}

static const char* type_name(const OS_Type* t)
{
    if (!t) return "?";
    if (t->base) t = t->base;
    if (t->name && t->name[0]) return t->name;
    switch (t->kind) {
    case OS_TYPE_STRUCT: return "struct";
    case OS_TYPE_UNION:  return "union";
    case OS_TYPE_ARRAY:  return "array";
    case OS_TYPE_PTR:    return "pointer";
    default:             return "?";
    }
}

static int is_aggregate(const OS_Type* t)
{
    if (t && t->base) t = t->base;
    return t && (t->kind == OS_TYPE_STRUCT || t->kind == OS_TYPE_UNION || t->kind == OS_TYPE_ARRAY);
}

/* 递归输出一个值（raw 指向该对象起点，avail 为可用字节） */
static void emit_value(OS_LocSnap* s, const char* name, const char* key, const OS_Type* t,
                       const uint8_t* raw, int avail, const char* where, int level, int is_param,
                       int (*is_exp)(const char*, void*), void* u, int expand_depth)
{
    OS_LocRow* r = row_add(s);
    int exp;
    if (!r) return;
    snprintf(r->name, sizeof(r->name), "%s", name);
    snprintf(r->key, sizeof(r->key), "%s", key);
    snprintf(r->type, sizeof(r->type), "%s", type_name(t));
    snprintf(r->where, sizeof(r->where), "%s", where ? where : "");
    r->level = level;
    r->is_param = is_param;
    if (!raw) { snprintf(r->value, sizeof(r->value), "%s", where ? "<不可读>" : ""); return; }
    if (!is_aggregate(t)) { fmt_scalar(t, raw, avail, r->value, sizeof(r->value)); return; }
    r->expandable = 1;
    exp = is_exp ? is_exp(key, u) : (level < expand_depth);
    if (level >= LOC_MAX_DEPTH) exp = 0;
    r->expanded = exp;
    if ((t->base ? t->base : t)->kind == OS_TYPE_ARRAY) {
        const OS_Type* at = t->base ? t->base : t;
        const OS_Type* et = at->child_count > 0 ? at->children[0] : NULL;
        int n = at->array_count, i, es = et ? (int)et->size : 0;
        snprintf(r->value, sizeof(r->value), "[%d]", n);
        if (!exp || !et || es <= 0) return;
        for (i = 0; i < n && i < LOC_ARRAY_SHOW; i++) {
            char nm[32], k2[192];
            int off = i * es;
            snprintf(nm, sizeof(nm), "[%d]", i);
            snprintf(k2, sizeof(k2), "%s[%d]", key, i);
            emit_value(s, nm, k2, et, off + es <= avail ? raw + off : NULL, avail - off, where ? "" : NULL,
                       level + 1, 0, is_exp, u, expand_depth);
        }
        if (n > LOC_ARRAY_SHOW) {
            OS_LocRow* m = row_add(s);
            if (m) { snprintf(m->name, sizeof(m->name), "..."); snprintf(m->value, sizeof(m->value), "仅显示前 %d 个", LOC_ARRAY_SHOW); m->level = level + 1; }
        }
        return;
    }
    snprintf(r->value, sizeof(r->value), "{...}");
    if (!exp) return;
    {
        int i;
        for (i = 0; i < t->child_count; i++) {
            const OS_Type* m = t->children[i];
            char k2[192];
            int off;
            if (!m) continue;
            off = (int)m->member_offset;
            snprintf(k2, sizeof(k2), "%s.%s", key, m->name ? m->name : "?");
            emit_value(s, m->name ? m->name : "?", k2, m,
                       (off >= 0 && off + (int)(m->size ? m->size : 1) <= avail) ? raw + off : NULL,
                       avail - off, "", level + 1, 0, is_exp, u, expand_depth);
        }
    }
}

static void reg_ctx_init(DwLocCtx* c, const OS_RegSet* rs)
{
    int i;
    memset(c, 0, sizeof(*c));
    c->arch = rs->arch == OS_ARCH_TRICORE ? 2 : 1;
    c->addr_size = 4;
    c->le = 1;
    c->read_mem = drv_read;
    for (i = 0; i < OS_REG_MAX && i < DWLOC_MAX_REGS; i++) {
        if (rs->arch == OS_ARCH_TRICORE && i >= 32 && i < 40) continue; /* E 对由 D 合成 */
        if (rs->arch == OS_ARCH_ARM_CM && i >= 16) continue;             /* 16+ 非 DWARF 核心寄存器（xPSR/FPU） */
        c->reg[i] = rs->value[i];
        c->reg_valid[i] = rs->valid[i];
    }
}

static const char* reg_name(const OS_RegSet* rs, int reg, char* buf, int cap)
{
    if (rs->arch == OS_ARCH_TRICORE && reg >= 32 && reg <= 39) { snprintf(buf, cap, "E%d", (reg - 32) * 2); return buf; }
    if (reg >= 0 && reg < OS_REG_MAX && rs->name[reg][0]) return rs->name[reg];
    snprintf(buf, cap, "r%d", reg);
    return buf;
}

int os_locals_snapshot(OS_LocSnap* s, int (*is_exp)(const char*, void*), void* u, int expand_depth)
{
    OS_LocalSym syms[LOC_MAX_SYMS];
    const uint8_t* fb = NULL;
    uint32_t fbl = 0;
    DwLocCtx c;
    int n, i, rc, pc_reg;
    uint64_t lo = 0, hi = 0;
    memset(s, 0, sizeof(*s));
    if (!g_app.driver || !g_app.connected) { s->status = OS_LOCS_NOT_CONNECTED; snprintf(s->msg, sizeof(s->msg), "仿真器未连接"); return s->status; }
    if (!g_app.elf) { s->status = OS_LOCS_NO_ELF; snprintf(s->msg, sizeof(s->msg), "未加载带调试信息的 ELF"); return s->status; }
    s_read_busy = 0;
    rc = g_app.driver->command(g_app.driver_ctx, OS_CMD_REG_READ, NULL, &s->regs);
    if (rc == OS_ERR_AGAIN) { s->status = OS_LOCS_BUSY; snprintf(s->msg, sizeof(s->msg), "驱动忙/重连中，稍后自动刷新…"); return s->status; }
    if (rc == OS_ERR_BUSY) { s->status = OS_LOCS_RUNNING; snprintf(s->msg, sizeof(s->msg), "CPU 运行中（停住后显示）"); return s->status; }
    if (rc != OS_ERR_OK) {
        s->status = (rc == OS_ERR_UNSUPPORTED || rc == OS_ERR_FAIL) ? OS_LOCS_UNSUPPORTED : OS_LOCS_REG_FAIL;
        snprintf(s->msg, sizeof(s->msg), "读寄存器失败（驱动 %s rc=%d）", g_app.driver->name, rc);
        return s->status;
    }
    pc_reg = s->regs.arch == OS_ARCH_TRICORE ? 40 : 15;
    s->pc = s->regs.value[pc_reg] & ~1u;
    os_elf_line_of_addr(g_app.elf, s->pc, s->file, sizeof(s->file), &s->line);
    if (!os_elf_func_at(g_app.elf, s->pc, s->func, sizeof(s->func), &lo, &hi)) {
        s->status = OS_LOCS_NO_FUNC;
        snprintf(s->msg, sizeof(s->msg), "PC 0x%08X 不在任何带调试信息的函数内", s->pc);
        return s->status;
    }
    reg_ctx_init(&c, &s->regs);
    {
        int cr = 0;
        int64_t co = 0;
        uint64_t rv;
        if (os_elf_cfa_at(g_app.elf, s->pc, &cr, &co) && dwloc_reg_value(&c, cr, &rv, NULL) == 0) {
            c.have_cfa = 1;
            c.cfa = (rv + (uint64_t)co) & 0xffffffffu;
            s->have_cfa = 1;
            s->cfa = c.cfa;
        }
    }
    n = os_elf_locals_at(g_app.elf, s->pc, syms, LOC_MAX_SYMS, &fb, &fbl);
    if (fb && fbl) {
        DwLocResult r;
        dwloc_eval(fb, fbl, &c, &r);
        if (r.kind == DWLOC_MEM) { c.have_frame_base = 1; c.frame_base = r.addr; }      /* breg/call_frame_cfa → 地址 */
        else if (r.kind == DWLOC_REG) { uint64_t v; if (dwloc_reg_value(&c, r.reg, &v, NULL) == 0) { c.have_frame_base = 1; c.frame_base = v; } }
        s->have_fb = c.have_frame_base;
        s->frame_base = c.frame_base;
    }
    for (i = 0; i < n; i++) {
        const OS_LocalSym* L = &syms[i];
        uint8_t buf[LOC_MAX_BYTES];
        char where[48] = "";
        int size = L->type && L->type->size ? (int)L->type->size : 4;
        int have = 0;
        if (size > LOC_MAX_BYTES) size = LOC_MAX_BYTES;
        memset(buf, 0, sizeof(buf));
        if (L->loc_kind == OS_LOC_OPTIMIZED_OUT || L->loc_kind == OS_LOC_NOT_AT_PC) {
            OS_LocRow* r = row_add(s);
            if (!r) break;
            snprintf(r->name, sizeof(r->name), "%s", L->name);
            snprintf(r->key, sizeof(r->key), "%s", L->name);
            snprintf(r->type, sizeof(r->type), "%s", type_name(L->type));
            snprintf(r->value, sizeof(r->value), "%s",
                     L->loc_kind == OS_LOC_NOT_AT_PC ? "<当前 PC 不可用>" : "<已被优化>");
            r->is_param = L->is_param;
            continue;
        }
        if (L->loc_kind == OS_LOC_CONST) {
            int k;
            for (k = 0; k < 8; k++) buf[k] = (uint8_t)((uint64_t)L->const_val >> (8 * k));
            snprintf(where, sizeof(where), "常量");
            have = 1;
        } else if (L->loc_kind == OS_LOC_CONST_BYTES) {
            memcpy(buf, L->expr, L->expr_len < sizeof(buf) ? L->expr_len : sizeof(buf));
            snprintf(where, sizeof(where), "常量");
            have = 1;
        } else {
            DwLocResult r;
            char rn[16];
            dwloc_eval(L->expr, L->expr_len, &c, &r);
            switch (r.kind) {
            case DWLOC_MEM:
                snprintf(where, sizeof(where), "0x%08llX", (unsigned long long)r.addr);
                have = drv_read(NULL, r.addr, buf, (uint32_t)size) == 0;
                break;
            case DWLOC_REG:
                snprintf(where, sizeof(where), "%s", reg_name(&s->regs, r.reg, rn, sizeof(rn)));
                have = dwloc_materialize(&c, &r, buf, (uint32_t)(size > 8 ? 8 : size)) == 0;
                break;
            case DWLOC_VALUE:
                snprintf(where, sizeof(where), "隐式值");
                memcpy(buf, r.bytes, (size_t)r.len);
                have = 1;
                break;
            case DWLOC_PIECES:
                snprintf(where, sizeof(where), r.partial ? "多片(部分)" : "多片");
                memcpy(buf, r.bytes, (size_t)r.len);
                have = 1;
                break;
            case DWLOC_NONE:
                snprintf(where, sizeof(where), "");
                break;
            default:
                snprintf(where, sizeof(where), "?");
                {
                    OS_LocRow* rr = row_add(s);
                    if (!rr) break;
                    snprintf(rr->name, sizeof(rr->name), "%s", L->name);
                    snprintf(rr->key, sizeof(rr->key), "%s", L->name);
                    snprintf(rr->type, sizeof(rr->type), "%s", type_name(L->type));
                    snprintf(rr->value, sizeof(rr->value), "<不可用: %s>", r.err);
                    rr->is_param = L->is_param;
                }
                continue;
            }
        }
        emit_value(s, L->name, L->name, L->type, have ? buf : NULL, size, where, 0, L->is_param,
                   is_exp, u, expand_depth);
    }
    s->status = OS_LOCS_OK;
    snprintf(s->msg, sizeof(s->msg), "%s @ 0x%08X%s", s->func, s->pc, n < 0 ? "" : "");
    return s->status;
}

void os_locals_free(OS_LocSnap* s)
{
    if (!s) return;
    free(s->rows);
    s->rows = NULL;
    s->nrows = s->caprows = 0;
}

static int jesc(const char* in, char* out, int cap)
{
    int o = 0;
    for (; in && *in && o < cap - 7; in++) {
        unsigned char ch = (unsigned char)*in;
        if (ch == '"' || ch == '\\') { out[o++] = '\\'; out[o++] = (char)ch; }
        else if (ch < 0x20) o += snprintf(out + o, cap - o, "\\u%04x", ch);
        else out[o++] = (char)ch;
    }
    out[o] = 0;
    return o;
}

int os_locals_to_json(const OS_LocSnap* s, char* out, int cap)
{
    int o = 0, i;
    char a[400], b[400], c[200], d[100], fn[300], fl[600];
    if (!out || cap < 64) return 0;
    jesc(s->func, fn, sizeof(fn));
    jesc(s->file, fl, sizeof(fl));
    jesc(s->msg, a, sizeof(a));
    o += snprintf(out + o, cap - o,
                  "{\"ok\":%s,\"status\":%d,\"message\":\"%s\",\"pc\":%u,\"pc_hex\":\"0x%08X\",\"function\":\"%s\","
                  "\"file\":\"%s\",\"line\":%d,\"frame\":0,\"frame_base\":%s%llu,\"cfa\":%s%llu,\"regs\":{",
                  s->status == OS_LOCS_OK ? "true" : "false", s->status, a, s->pc, s->pc, fn, fl, s->line,
                  s->have_fb ? "" : "null,\"_fb\":", (unsigned long long)s->frame_base,
                  s->have_cfa ? "" : "null,\"_cfa\":", (unsigned long long)s->cfa);
    {
        int first = 1;
        for (i = 0; i < s->regs.count && i < OS_REG_MAX && o < cap - 64; i++) {
            if (!s->regs.valid[i] || !s->regs.name[i][0]) continue;
            o += snprintf(out + o, cap - o, "%s\"%s\":\"0x%08X\"", first ? "" : ",", s->regs.name[i], s->regs.value[i]);
            first = 0;
        }
    }
    o += snprintf(out + o, cap - o, "},\"locals\":[");
    for (i = 0; i < s->nrows && o < cap - 900; i++) {
        const OS_LocRow* r = &s->rows[i];
        jesc(r->name, a, sizeof(a));
        jesc(r->value, b, sizeof(b));
        jesc(r->type, c, sizeof(c));
        jesc(r->where, d, sizeof(d));
        o += snprintf(out + o, cap - o,
                      "%s{\"name\":\"%s\",\"value\":\"%s\",\"type\":\"%s\",\"where\":\"%s\",\"level\":%d,\"param\":%d,\"expandable\":%d}",
                      i ? "," : "", a, b, c, d, r->level, r->is_param, r->expandable);
    }
    if (o > cap - 4) o = cap - 4;
    o += snprintf(out + o, cap - o, "]}");
    return o;
}

/* ------------------------------------------------------------------ */
/* 窗口                                                                */
/* ------------------------------------------------------------------ */

static const wchar_t* g_loc_class = L"OSLocalsWin";

typedef struct LocWin {
    HWND hwnd, list, hdr;
    OS_LocSnap snap;
    char  (*exp_keys)[192];
    int   nexp, capexp;
    int   stale;          /* 1 = 运行中（置灰） */
    uint32_t last_pc;
    int   have_snap;
} LocWin;

static LocWin* loc_from(HWND h)
{
    wchar_t cls[64];
    if (!h || !GetClassNameW(h, cls, 64) || wcscmp(cls, g_loc_class) != 0) return NULL;
    return (LocWin*)GetWindowLongPtrW(h, GWLP_USERDATA);
}

static int lw_is_exp(const char* key, void* u)
{
    LocWin* w = (LocWin*)u;
    int i;
    for (i = 0; i < w->nexp; i++) if (strcmp(w->exp_keys[i], key) == 0) return 1;
    return 0;
}

static void lw_toggle(LocWin* w, const char* key)
{
    int i;
    for (i = 0; i < w->nexp; i++) {
        if (strcmp(w->exp_keys[i], key) == 0) {
            memmove(w->exp_keys[i], w->exp_keys[i + 1], (size_t)(w->nexp - i - 1) * sizeof(w->exp_keys[0]));
            w->nexp--;
            return;
        }
    }
    if (w->nexp == w->capexp) {
        int nc = w->capexp ? w->capexp * 2 : 32;
        char (*n)[192] = (char (*)[192])realloc(w->exp_keys, (size_t)nc * sizeof(w->exp_keys[0]));
        if (!n) return;
        w->exp_keys = n;
        w->capexp = nc;
    }
    snprintf(w->exp_keys[w->nexp++], 192, "%s", key);
}

static void lw_fill(LocWin* w)
{
    int i;
    wchar_t buf[256];
    SendMessageW(w->list, WM_SETREDRAW, FALSE, 0);
    ListView_DeleteAllItems(w->list);
    if (w->snap.status != OS_LOCS_OK || w->snap.nrows == 0) {
        LVITEMW it;
        memset(&it, 0, sizeof(it));
        it.mask = LVIF_TEXT;
        os_utf8_to_wide_buf(w->snap.status == OS_LOCS_OK ? "（无参数/局部变量）" : w->snap.msg, buf, 256);
        it.pszText = buf;
        ListView_InsertItem(w->list, &it);
    }
    for (i = 0; w->snap.status == OS_LOCS_OK && i < w->snap.nrows; i++) {
        const OS_LocRow* r = &w->snap.rows[i];
        LVITEMW it;
        char nm[160];
        snprintf(nm, sizeof(nm), "%s%s%s", r->expandable ? (r->expanded ? "\xE2\x96\xBE " : "\xE2\x96\xB8 ") : "  ",
                 r->is_param && r->level == 0 ? "\xE2\x80\xA2" : "", r->name);
        memset(&it, 0, sizeof(it));
        it.mask = LVIF_TEXT | LVIF_INDENT | LVIF_PARAM;
        it.iItem = i;
        it.iIndent = r->level;
        it.lParam = i;
        os_utf8_to_wide_buf(nm, buf, 256);
        it.pszText = buf;
        ListView_InsertItem(w->list, &it);
        os_utf8_to_wide_buf(r->value, buf, 256); ListView_SetItemText(w->list, i, 1, buf);
        os_utf8_to_wide_buf(r->type, buf, 256);  ListView_SetItemText(w->list, i, 2, buf);
        os_utf8_to_wide_buf(r->where, buf, 256); ListView_SetItemText(w->list, i, 3, buf);
    }
    SendMessageW(w->list, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(w->hwnd, NULL, TRUE);
}

static void lw_apply_colors(LocWin* w);
#define LOC_TIMER_RETRY 0x5B03   /* Bug4：驱动忙时的重试定时器 */

static void lw_refresh(LocWin* w)
{
    /* Bug4：先拍到临时快照；驱动忙则保留上次结果（置灰）并定时重试，不清空、不报错 */
    OS_LocSnap* t = (OS_LocSnap*)calloc(1, sizeof(OS_LocSnap));
    if (t) {
        os_locals_snapshot(t, lw_is_exp, w, 0);
        if (t->status == OS_LOCS_BUSY || s_read_busy) {
            s_read_busy = 0;
            SetTimer(w->hwnd, LOC_TIMER_RETRY, 300, NULL);
            if (w->have_snap) {
                os_locals_free(t); free(t);
                w->stale = 1;
                lw_apply_colors(w);
                InvalidateRect(w->list, NULL, TRUE);
                return;
            }
            t->status = OS_LOCS_BUSY;
            snprintf(t->msg, sizeof(t->msg), "驱动忙/重连中，稍后自动刷新…");
        }
        os_locals_free(&w->snap);
        w->snap = *t;   /* 浅拷贝转移所有权 */
        free(t);
    } else {
        os_locals_free(&w->snap);
        os_locals_snapshot(&w->snap, lw_is_exp, w, 0);
    }
    w->have_snap = 1;
    w->stale = (w->snap.status == OS_LOCS_RUNNING);
    w->last_pc = w->snap.pc;
    lw_fill(w);
}

static void lw_layout(LocWin* w)
{
    RECT rc;
    GetClientRect(w->hwnd, &rc);
    MoveWindow(w->list, 0, 24, rc.right, rc.bottom - 24 > 0 ? rc.bottom - 24 : 0, TRUE);
}

static void lw_apply_colors(LocWin* w)
{
    COLORREF bg = os_theme(TH_TREE_BG), fg = w->stale ? os_theme(TH_DIMTEXT) : os_theme(TH_TREE_TEXT);
    ListView_SetBkColor(w->list, bg);
    ListView_SetTextBkColor(w->list, bg);
    ListView_SetTextColor(w->list, fg);
}

static LRESULT CALLBACK loc_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    LocWin* w = (LocWin*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_TIMER:
        if (wp == LOC_TIMER_RETRY) {   /* Bug4 */
            KillTimer(hwnd, LOC_TIMER_RETRY);
            if (w && g_app.connected && g_app.driver) lw_refresh(w);
            return 0;
        }
        break;
    case WM_CREATE: {
        LVCOLUMNW col;
        w = (LocWin*)calloc(1, sizeof(LocWin));
        if (!w) return -1;
        w->hwnd = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)w);
        w->list = CreateWindowExW(0, WC_LISTVIEWW, L"",
                                  WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
                                  0, 24, 100, 100, hwnd, (HMENU)1, g_app.hInst, NULL);
        ListView_SetExtendedListViewStyle(w->list, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES);
        SendMessageW(w->list, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        memset(&col, 0, sizeof(col));
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.pszText = L"名称"; col.cx = 180; ListView_InsertColumn(w->list, 0, &col);
        col.pszText = L"值";   col.cx = 200; ListView_InsertColumn(w->list, 1, &col);
        col.pszText = L"类型"; col.cx = 120; ListView_InsertColumn(w->list, 2, &col);
        col.pszText = L"位置"; col.cx = 100; ListView_InsertColumn(w->list, 3, &col);
        lw_apply_colors(w);
        w->snap.status = OS_LOCS_RUNNING;
        snprintf(w->snap.msg, sizeof(w->snap.msg), "CPU 停住（断点/暂停）后显示当前函数的参数与局部变量");
        lw_fill(w);
        /* 打开时若已停住则立即刷新 */
        if (g_app.connected && g_app.driver) lw_refresh(w);
        return 0;
    }
    case WM_SIZE:
        if (w) lw_layout(w);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        RECT rc, top;
        wchar_t t[400];
        char line[400];
        GetClientRect(hwnd, &rc);
        top = rc; top.bottom = 24;
        FillRect(dc, &top, os_theme_brush(TH_PANEL));
        if (w) {
            if (w->snap.status == OS_LOCS_OK)
                snprintf(line, sizeof(line), "帧 #0  %s()  PC=0x%08X  %s:%d%s",
                         w->snap.func, w->snap.pc, w->snap.file[0] ? w->snap.file : "?", w->snap.line,
                         w->stale ? "   [运行中 — 显示的是上次停住时的值]" : "");
            else
                snprintf(line, sizeof(line), "%s", w->snap.msg);
            os_utf8_to_wide_buf(line, t, 400);
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, w->stale ? os_theme(TH_DIMTEXT) : os_theme(TH_TEXT));
            SelectObject(dc, GetStockObject(DEFAULT_GUI_FONT));
            top.left += 6;
            DrawTextW(dc, t, -1, &top, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_NOTIFY: {
        NMHDR* nh = (NMHDR*)lp;
        if (w && nh->hwndFrom == w->list && (nh->code == NM_DBLCLK || nh->code == NM_CLICK)) {
            NMITEMACTIVATE* ia = (NMITEMACTIVATE*)lp;
            if (ia->iItem >= 0 && ia->iItem < w->snap.nrows && w->snap.rows[ia->iItem].expandable &&
                (nh->code == NM_DBLCLK || ia->ptAction.x < 24 + 16 * w->snap.rows[ia->iItem].level)) {
                lw_toggle(w, w->snap.rows[ia->iItem].key);
                if (!w->stale) lw_refresh(w);
            }
        }
        break;
    }
    case WM_DESTROY:
        if (w) {
            os_locals_free(&w->snap);
            free(w->exp_keys);
            free(w);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void os_locals_register(void)
{
    WNDCLASSW wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = loc_proc;
    wc.hInstance = g_app.hInst ? g_app.hInst : GetModuleHandleW(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = g_loc_class;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClassW(&wc);
}

HWND os_locals_create(HWND parent, int x, int y, int w, int h, const wchar_t* title)
{
    return CreateWindowExW(WS_EX_CLIENTEDGE, g_loc_class, title ? title : L"局部变量",
                           WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                           x, y, w, h, parent, NULL, g_app.hInst ? g_app.hInst : GetModuleHandleW(NULL), NULL);
}

int os_locals_is(HWND hwnd) { return loc_from(hwnd) != NULL; }

int os_locals_window_count(void)
{
    int i, j, n = 0;
    for (i = 0; i < g_app.win_count; i++)
        for (j = 0; j < g_app.wins[i].group_count; j++)
            if (loc_from(g_app.wins[i].group[j])) n++;
    return n;
}

HWND os_locals_open(void)
{
    int i, j;
    for (i = 0; i < g_app.win_count; i++)
        for (j = 0; j < g_app.wins[i].group_count; j++)
            if (loc_from(g_app.wins[i].group[j])) {
                os_mainwin_select_tab(i);
                return g_app.wins[i].group[j];
            }
    return os_win_create_by_type("locals", L"局部变量");
}

void os_locals_on_cpu(int halted, uint32_t pc, int connected)
{
    int i, j;
    for (i = 0; i < g_app.win_count; i++) {
        for (j = 0; j < g_app.wins[i].group_count; j++) {
            LocWin* w = loc_from(g_app.wins[i].group[j]);
            if (!w) continue;
            if (!connected) {
                if (w->snap.status != OS_LOCS_NOT_CONNECTED) {
                    os_locals_free(&w->snap);
                    w->snap.status = OS_LOCS_NOT_CONNECTED;
                    snprintf(w->snap.msg, sizeof(w->snap.msg), "仿真器未连接");
                    w->stale = 1;
                    lw_apply_colors(w);
                    lw_fill(w);
                }
                continue;
            }
            if (halted == 1) {
                if (w->stale || !w->have_snap || (pc & ~1u) != w->last_pc) {
                    w->stale = 0;
                    lw_apply_colors(w);
                    lw_refresh(w);
                }
            } else if (halted == 0 && !w->stale) {
                w->stale = 1;           /* 运行中：置灰，不读目标 */
                lw_apply_colors(w);
                InvalidateRect(w->list, NULL, TRUE);
                InvalidateRect(w->hwnd, NULL, TRUE);
            }
        }
    }
}
