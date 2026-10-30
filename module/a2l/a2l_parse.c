/* A2L 解析实现，见 a2l_parse.h。 */
#include "a2l_parse.h"

#include <ctype.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================ 词法 ============================ */

typedef struct Lex {
    const char* p;
    const char* end;
    int         line;
} Lex;

/* 内存不足不再 abort：全部分配走 xrealloc，失败返回 NULL，由调用方报错并释放已建结构。
 * 测试钩子：a2l_debug_fail_alloc_after(n) 让第 n+1 次分配起失败（-1 关闭；非线程安全，仅测试用）。 */
static long g_fail_after = -1;

void a2l_debug_fail_alloc_after(long n) { g_fail_after = n; }

static void* xrealloc(void* p, size_t n)
{
    if (g_fail_after == 0) return NULL;
    if (g_fail_after > 0) g_fail_after--;
    return realloc(p, n ? n : 1);
}

static void* xcalloc(size_t n, size_t sz)
{
    void* q = xrealloc(NULL, n * sz);
    if (q) memset(q, 0, n * sz);
    return q;
}

static char* xstrndup(const char* s, size_t n)
{
    char* d = (char*)xrealloc(NULL, n + 1);
    if (!d) return NULL;
    memcpy(d, s, n);
    d[n] = 0;
    return d;
}

/* 取下一个记号；返回 0 = 结束，-1 = 错误（未闭合字符串/注释），-2 = 内存不足 */
static int lex_next(Lex* L, A2lTok* t)
{
    const char* p = L->p;
    for (;;) {
        while (p < L->end && isspace((unsigned char)*p)) { if (*p == '\n') L->line++; p++; }
        if (p + 1 < L->end && p[0] == '/' && p[1] == '*') {
            p += 2;
            while (p + 1 < L->end && !(p[0] == '*' && p[1] == '/')) { if (*p == '\n') L->line++; p++; }
            if (p + 1 >= L->end) { L->p = p; return -1; }
            p += 2;
            continue;
        }
        if (p + 1 < L->end && p[0] == '/' && p[1] == '/') {
            while (p < L->end && *p != '\n') p++;
            continue;
        }
        break;
    }
    if (p >= L->end) { L->p = p; return 0; }
    t->line = L->line;
    if (*p == '"') {
        /* 字符串："" 与 \" 都视为转义的引号 */
        char* buf = NULL;
        size_t n = 0, cap = 0;
        p++;
        for (;;) {
            char ch;
            if (p >= L->end) { free(buf); L->p = p; return -1; }
            ch = *p++;
            if (ch == '\\' && p < L->end) { ch = *p++; if (ch == 'n') ch = '\n'; else if (ch == 't') ch = '\t'; }
            else if (ch == '"') {
                if (p < L->end && *p == '"') p++;
                else break;
            } else if (ch == '\n') L->line++;
            if (n + 2 > cap) {
                char* nb;
                cap = cap ? cap * 2 : 32;
                nb = (char*)xrealloc(buf, cap);
                if (!nb) { free(buf); L->p = p; return -2; }
                buf = nb;
            }
            buf[n++] = ch;
        }
        if (!buf && (buf = (char*)xrealloc(NULL, 1)) == NULL) { L->p = p; return -2; }
        buf[n] = 0;
        t->s = buf;
        t->quoted = 1;
    } else {
        const char* s = p;
        while (p < L->end && !isspace((unsigned char)*p) && *p != '"' &&
               !(p + 1 < L->end && p[0] == '/' && p[1] == '*' && p != s))
            p++;
        t->s = xstrndup(s, (size_t)(p - s));
        t->quoted = 0;
        if (!t->s) { L->p = p; return -2; }
    }
    L->p = p;
    return 1;
}

/* ============================ 块树 ============================ */

static A2lNode* node_new(const char* kw, int line)
{
    A2lNode* n = (A2lNode*)xcalloc(1, sizeof(A2lNode));
    if (!n) return NULL;
    n->kw = xstrndup(kw, strlen(kw));
    if (!n->kw) { free(n); return NULL; }
    n->line = line;
    return n;
}

static void node_free(A2lNode* n)
{
    int i;
    if (!n) return;
    for (i = 0; i < n->ntok; i++) free(n->tok[i].s);
    for (i = 0; i < n->nkid; i++) node_free(n->kid[i]);
    free(n->tok);
    free(n->kid);
    free(n->kw);
    free(n);
}

/* 容量 4,8,16…：仅在 n==0 或 n 为 ≥4 的 2 的幂时扩容（修复 n=1 时缩到 2 的多余 realloc） */
static int need_grow(int n) { return n == 0 || (n >= 4 && (n & (n - 1)) == 0); }

static int node_add_tok(A2lNode* n, A2lTok t)
{
    if (need_grow(n->ntok)) {
        A2lTok* q = (A2lTok*)xrealloc(n->tok, sizeof(A2lTok) * (size_t)(n->ntok ? n->ntok * 2 : 4));
        if (!q) return -1;
        n->tok = q;
    }
    n->tok[n->ntok++] = t;
    return 0;
}

static int node_add_kid(A2lNode* n, A2lNode* k)
{
    if (need_grow(n->nkid)) {
        A2lNode** q = (A2lNode**)xrealloc(n->kid, sizeof(A2lNode*) * (size_t)(n->nkid ? n->nkid * 2 : 4));
        if (!q) return -1;
        n->kid = q;
    }
    n->kid[n->nkid++] = k;
    return 0;
}

#define OOM_MSG "内存不足"

static A2lNode* build_tree(const char* text, size_t len, char* err, int errcap)
{
    Lex L;
    A2lTok t;
    A2lNode* stack[64];
    int sp = 0, r;
    L.p = text; L.end = text + len; L.line = 1;
    stack[0] = node_new("", 0);
    if (!stack[0]) { snprintf(err, (size_t)errcap, OOM_MSG); return NULL; }
    while ((r = lex_next(&L, &t)) > 0) {
        if (!t.quoted && strcmp(t.s, "/begin") == 0) {
            A2lTok k;
            int r2;
            free(t.s);
            r2 = lex_next(&L, &k);
            if (r2 == -2) { r = -2; break; }
            if (r2 <= 0 || k.quoted) {
                if (r2 > 0) free(k.s);   /* /begin "X"：带引号的记号也要释放 */
                snprintf(err, (size_t)errcap, "line %d: /begin 后缺关键字", L.line);
                goto fail;
            }
            if (sp + 1 >= 64) { snprintf(err, (size_t)errcap, "line %d: 嵌套过深", L.line); free(k.s); goto fail; }
            {
                A2lNode* n = node_new(k.s, k.line);
                free(k.s);
                if (!n) { r = -2; break; }
                if (node_add_kid(stack[sp], n) != 0) { node_free(n); r = -2; break; }
                stack[++sp] = n;
            }
        } else if (!t.quoted && strcmp(t.s, "/end") == 0) {
            A2lTok k;
            int r2;
            free(t.s);
            r2 = lex_next(&L, &k);
            if (r2 == -2) { r = -2; break; }
            if (r2 <= 0) { snprintf(err, (size_t)errcap, "line %d: /end 后缺关键字", L.line); goto fail; }
            if (sp == 0 || strcmp(k.s, stack[sp]->kw) != 0) {
                snprintf(err, (size_t)errcap, "line %d: /end %s 与 /begin %s (line %d) 不匹配", k.line, k.s,
                         sp ? stack[sp]->kw : "(none)", sp ? stack[sp]->line : 0);
                free(k.s);
                goto fail;
            }
            free(k.s);
            sp--;
        } else if (node_add_tok(stack[sp], t) != 0) {
            free(t.s);
            r = -2;
            break;
        }
    }
    if (r == -2) { snprintf(err, (size_t)errcap, OOM_MSG); goto fail; }
    if (r < 0) { snprintf(err, (size_t)errcap, "line %d: 未闭合的字符串或注释", L.line); goto fail; }
    if (sp != 0) { snprintf(err, (size_t)errcap, "文件结束时 /begin %s (line %d) 未闭合", stack[sp]->kw, stack[sp]->line); goto fail; }
    return stack[0];
fail:
    node_free(stack[0]);
    return NULL;
}

const A2lNode* a2l_node_child(const A2lNode* n, const char* kw, int idx)
{
    int i;
    if (!n) return NULL;
    for (i = 0; i < n->nkid; i++)
        if (strcmp(n->kid[i]->kw, kw) == 0 && idx-- == 0) return n->kid[i];
    return NULL;
}

/* ============================ 工具 ============================ */

static const char* tk(const A2lNode* n, int i) { return (n && i >= 0 && i < n->ntok) ? n->tok[i].s : ""; }
static double tnum(const A2lNode* n, int i) { return strtod(tk(n, i), NULL); }
static uint32_t tu32(const A2lNode* n, int i) { return (uint32_t)strtoul(tk(n, i), NULL, 0); }
static long tint(const A2lNode* n, int i) { return strtol(tk(n, i), NULL, 0); }

/* 未加引号的关键字位置（从 start 开始找），-1 = 无 */
static int kwi(const A2lNode* n, int start, const char* kw)
{
    int i;
    for (i = start; i < n->ntok; i++)
        if (!n->tok[i].quoted && strcmp(n->tok[i].s, kw) == 0) return i;
    return -1;
}

static void cpy(char* d, size_t cap, const char* s)
{
    size_t n = strlen(s);
    if (n >= cap) n = cap - 1;
    memcpy(d, s, n);
    d[n] = 0;
}
#define CPY(d, s) cpy((d), sizeof(d), (s))

/* 抽取阶段内存不足：跳回 a2l_parse_text，由其 a2l_free 已建结构（各数组在扩容成功后才计数，始终一致） */
typedef struct Ctx { jmp_buf jb; } Ctx;

static void* need(A2lFile* f, void* p)
{
    if (!p) longjmp(((Ctx*)f->priv)->jb, 1);
    return p;
}

static void str_push(A2lFile* f, char*** arr, int* n, const char* s)
{
    char* d;
    *arr = (char**)need(f, xrealloc(*arr, sizeof(char*) * (size_t)(*n + 1)));
    d = (char*)need(f, xstrndup(s, strlen(s)));
    (*arr)[(*n)++] = d;
}

static void* grow_impl(A2lFile* f, void** arr, int* n, size_t sz)
{
    char* q = (char*)need(f, xrealloc(*arr, sz * (size_t)(*n + 1)));
    *arr = q;
    memset(q + sz * (size_t)*n, 0, sz);
    return q + sz * (size_t)(*n)++;
}

static void str_free(char** a, int n)
{
    int i;
    for (i = 0; i < n; i++) free(a[i]);
    free(a);
}

#define GROW(arr, n) grow_impl(f, (void**)&(arr), &(n), sizeof(*(arr)))

int a2l_datatype_size(const char* dt)
{
    if (!strcmp(dt, "UBYTE") || !strcmp(dt, "SBYTE")) return 1;
    if (!strcmp(dt, "UWORD") || !strcmp(dt, "SWORD") || !strcmp(dt, "FLOAT16_IEEE")) return 2;
    if (!strcmp(dt, "ULONG") || !strcmp(dt, "SLONG") || !strcmp(dt, "FLOAT32_IEEE")) return 4;
    if (!strcmp(dt, "A_UINT64") || !strcmp(dt, "A_INT64") || !strcmp(dt, "FLOAT64_IEEE")) return 8;
    return 0;
}

/* ============================ 抽取：XCP ============================ */

static void parse_proto(A2lFile* f, const A2lNode* n, A2lXcpProto* p)
{
    int i;
    p->present = 1;
    p->version = (uint16_t)tu32(n, 0);
    for (i = 1; i <= 7; i++) p->t[i] = (uint16_t)tu32(n, i);
    p->max_cto = (int)tint(n, 8);
    p->max_dto = (int)tint(n, 9);
    p->byte_order_msb_first = strcmp(tk(n, 10), "BYTE_ORDER_MSB_FIRST") == 0;
    p->addr_gran = !strcmp(tk(n, 11), "ADDRESS_GRANULARITY_DWORD") ? 4 :
                   !strcmp(tk(n, 11), "ADDRESS_GRANULARITY_WORD") ? 2 : 1;
    for (i = 12; i < n->ntok; i++) {
        if (n->tok[i].quoted) continue;
        if (!strcmp(n->tok[i].s, "OPTIONAL_CMD") && i + 1 < n->ntok)
            str_push(f, &p->optional_cmd, &p->noptional_cmd, n->tok[++i].s);
        else if (!strcmp(n->tok[i].s, "SEED_AND_KEY_EXTERNAL_FUNCTION") && i + 1 < n->ntok)
            CPY(p->seed_key_dll, n->tok[++i].s);
        else if (!strcmp(n->tok[i].s, "COMMUNICATION_MODE_SUPPORTED")) {
            int j;
            for (j = i + 1; j < n->ntok && !n->tok[j].quoted; j++) {
                const char* s = n->tok[j].s;
                if (!strcmp(s, "SLAVE")) p->slave_block = 1;
                else if (!strcmp(s, "MASTER")) p->master_block = 1;
                else if (!strcmp(s, "INTERLEAVED")) p->interleaved = 1;
                else if (!strcmp(s, "BLOCK") || isdigit((unsigned char)s[0])) {}
                else break;
            }
            i = j - 1;
        }
    }
}

static double unit_seconds(int code)
{
    /* XCP TIME_UNIT / 时间戳单位：0=1ns 1=10ns … 9=1s */
    static const double u[] = { 1e-9, 1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1, 1.0 };
    return (code >= 0 && code <= 9) ? u[code] : 0.0;
}

static double unit_kw_seconds(const char* s)
{
    static const char* k[] = { "UNIT_1NS", "UNIT_10NS", "UNIT_100NS", "UNIT_1US", "UNIT_10US",
                               "UNIT_100US", "UNIT_1MS", "UNIT_10MS", "UNIT_100MS", "UNIT_1S" };
    int i;
    for (i = 0; i < 10; i++) if (!strcmp(s, k[i])) return unit_seconds(i);
    return 0.0;
}

static void parse_daq(A2lFile* f, const A2lNode* n, A2lXcpDaq* d)
{
    int i;
    const A2lNode* ts;
    d->present = 1;
    d->dynamic = strcmp(tk(n, 0), "DYNAMIC") == 0;
    d->max_daq = (int)tint(n, 1);
    d->max_event = (int)tint(n, 2);
    d->min_daq = (int)tint(n, 3);
    CPY(d->optimisation, tk(n, 4));
    CPY(d->addr_ext, tk(n, 5));
    CPY(d->id_field, tk(n, 6));
    CPY(d->granularity, tk(n, 7));
    d->max_odt_entry_size = (int)tint(n, 8);
    CPY(d->overload, tk(n, 9));
    d->prescaler = kwi(n, 10, "PRESCALER_SUPPORTED") >= 0;
    d->resume = kwi(n, 10, "RESUME_SUPPORTED") >= 0;
    if ((ts = a2l_node_child(n, "TIMESTAMP_SUPPORTED", 0)) != NULL) {
        d->ts_present = 1;
        d->ts_ticks = (int)tint(ts, 0);
        d->ts_size = !strcmp(tk(ts, 1), "SIZE_DWORD") ? 4 : !strcmp(tk(ts, 1), "SIZE_WORD") ? 2 :
                     !strcmp(tk(ts, 1), "SIZE_BYTE") ? 1 : 0;
        CPY(d->ts_unit, tk(ts, 2));
        d->ts_tick_s = d->ts_ticks * unit_kw_seconds(d->ts_unit);
        d->ts_fixed = kwi(ts, 3, "TIMESTAMP_FIXED") >= 0;
    }
    for (i = 0; i < n->nkid; i++) {
        const A2lNode* e = n->kid[i];
        A2lXcpEvent* ev;
        if (strcmp(e->kw, "EVENT")) continue;
        ev = GROW(d->ev, d->nev);
        CPY(ev->name, tk(e, 0));
        CPY(ev->short_name, tk(e, 1));
        ev->channel = (int)tint(e, 2);
        CPY(ev->direction, tk(e, 3));
        ev->max_daq_list = (int)tint(e, 4);
        ev->time_cycle = (int)tint(e, 5);
        ev->time_unit = (int)tint(e, 6);
        ev->priority = (int)tint(e, 7);
        ev->cycle_s = ev->time_cycle * unit_seconds(ev->time_unit);
    }
}

static void parse_common(A2lFile* f, const A2lNode* n, A2lXcpProto* p, A2lXcpDaq* d)
{
    const A2lNode* c;
    if ((c = a2l_node_child(n, "PROTOCOL_LAYER", 0)) != NULL) parse_proto(f, c, p);
    if ((c = a2l_node_child(n, "DAQ", 0)) != NULL) parse_daq(f, c, d);
}

static void parse_transport(A2lFile* f, const A2lNode* n, A2lXcp* x)
{
    static const struct { const char* kw; A2lTlKind k; } map[] = {
        { "XCP_ON_TCP_IP", A2L_TL_TCP }, { "XCP_ON_UDP_IP", A2L_TL_UDP }, { "XCP_ON_CAN", A2L_TL_CAN },
        { "XCP_ON_USB", A2L_TL_USB }, { "XCP_ON_SxI", A2L_TL_SXI }, { "XCP_ON_FLX", A2L_TL_FLX },
    };
    unsigned i;
    int j;
    A2lXcpTransport* t;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); i++)
        if (!strcmp(n->kw, map[i].kw)) break;
    if (i == sizeof(map) / sizeof(map[0])) return;
    t = GROW(x->tl, x->ntl);
    t->kind = map[i].k;
    CPY(t->name, n->kw);
    t->version = (uint16_t)tu32(n, 0);
    if (t->kind == A2L_TL_TCP || t->kind == A2L_TL_UDP) {
        t->port = (int)tint(n, 1);
        if ((j = kwi(n, 2, "ADDRESS")) >= 0 || (j = kwi(n, 2, "HOST_NAME")) >= 0) CPY(t->host, tk(n, j + 1));
    } else if (t->kind == A2L_TL_CAN) {
        if ((j = kwi(n, 1, "CAN_ID_MASTER")) >= 0) t->can_id_master = tu32(n, j + 1);
        if ((j = kwi(n, 1, "CAN_ID_SLAVE")) >= 0) t->can_id_slave = tu32(n, j + 1);
        if ((j = kwi(n, 1, "CAN_ID_BROADCAST")) >= 0) { t->can_id_broadcast = tu32(n, j + 1); t->has_broadcast = 1; }
        if ((j = kwi(n, 1, "BAUDRATE")) >= 0) t->baudrate = tu32(n, j + 1);
        if ((j = kwi(n, 1, "MAX_DLC")) >= 0) t->max_dlc = (int)tint(n, j + 1);
        t->can_fd = a2l_node_child(n, "CAN_FD", 0) != NULL;
        if (t->can_fd && (j = kwi(a2l_node_child(n, "CAN_FD", 0), 0, "MAX_DLC")) >= 0)
            t->max_dlc = (int)tint(a2l_node_child(n, "CAN_FD", 0), j + 1);
    } else if (t->kind == A2L_TL_SXI) {
        t->baudrate = tu32(n, 1);
    }
    parse_common(f, n, &t->proto, &t->daq);
}

static void parse_if_data_xcp(A2lFile* f, const A2lNode* n, A2lXcp* x)
{
    int i;
    const A2lNode* c;
    x->present = 1;
    parse_common(f, n, &x->proto, &x->daq);
    if ((c = a2l_node_child(n, "PAG", 0)) != NULL) {
        x->pag_present = 1;
        x->pag_max_segments = (int)tint(c, 0);
        x->pag_freeze = kwi(c, 1, "FREEZE_SUPPORTED") >= 0;
    }
    /* SEED_AND_KEY_EXTERNAL_FUNCTION 也可能出现在 XCP 层（Common_Parameters） */
    if ((i = kwi(n, 0, "SEED_AND_KEY_EXTERNAL_FUNCTION")) >= 0 && !x->proto.seed_key_dll[0])
        CPY(x->proto.seed_key_dll, tk(n, i + 1));
    for (i = 0; i < n->nkid; i++) parse_transport(f, n->kid[i], x);
}

/* ============================ 抽取：其他 ============================ */

static A2lCompuType compu_type(const char* s)
{
    if (!strcmp(s, "IDENTICAL")) return A2L_CM_IDENTICAL;
    if (!strcmp(s, "LINEAR")) return A2L_CM_LINEAR;
    if (!strcmp(s, "RAT_FUNC")) return A2L_CM_RAT_FUNC;
    if (!strcmp(s, "FORM")) return A2L_CM_FORM;
    if (!strcmp(s, "TAB_INTP")) return A2L_CM_TAB_INTP;
    if (!strcmp(s, "TAB_NOINTP")) return A2L_CM_TAB_NOINTP;
    if (!strcmp(s, "TAB_VERB")) return A2L_CM_TAB_VERB;
    return A2L_CM_UNKNOWN;
}

static void parse_compu(A2lFile* f, const A2lNode* n)
{
    A2lCompu* c = GROW(f->compu, f->ncompu);
    int j, k;
    const A2lNode* fm;
    CPY(c->name, tk(n, 0));
    CPY(c->longid, tk(n, 1));
    c->type = compu_type(tk(n, 2));
    CPY(c->format, tk(n, 3));
    CPY(c->unit, tk(n, 4));
    c->tab = -1;
    if ((j = kwi(n, 5, "COEFFS")) >= 0)
        for (k = 0; k < 6; k++) c->coeffs[k] = tnum(n, j + 1 + k);
    if ((j = kwi(n, 5, "COEFFS_LINEAR")) >= 0) { c->coeffs[0] = tnum(n, j + 1); c->coeffs[1] = tnum(n, j + 2); }
    if ((j = kwi(n, 5, "COMPU_TAB_REF")) >= 0) CPY(c->tab_ref, tk(n, j + 1));
    if ((fm = a2l_node_child(n, "FORMULA", 0)) != NULL) {
        CPY(c->formula, tk(fm, 0));
        if ((j = kwi(fm, 1, "FORMULA_INV")) >= 0) CPY(c->formula_inv, tk(fm, j + 1));
    }
}

static void parse_tab(A2lFile* f, const A2lNode* n, int is_vtab, int is_range)
{
    A2lCompuTab* t = GROW(f->tab, f->ntab);
    int cnt, i, pos, j;
    CPY(t->name, tk(n, 0));
    t->is_range = is_range;
    if (is_range) { t->kind = A2L_CM_TAB_VERB; cnt = (int)tint(n, 2); pos = 3; }
    else { t->kind = is_vtab ? A2L_CM_TAB_VERB : compu_type(tk(n, 2)); cnt = (int)tint(n, 3); pos = 4; }
    if (cnt < 0 || pos + cnt * (is_range ? 3 : 2) > n->ntok) cnt = 0;
    t->e = (A2lTabEntry*)need(f, xcalloc((size_t)(cnt ? cnt : 1), sizeof(A2lTabEntry)));
    t->n = cnt;
    for (i = 0; i < cnt; i++) {
        A2lTabEntry* e = &t->e[i];
        e->in = tnum(n, pos++);
        e->in_max = is_range ? tnum(n, pos++) : e->in;
        if (t->kind == A2L_CM_TAB_VERB) CPY(e->text, tk(n, pos++));
        else e->out = tnum(n, pos++);
    }
    if ((j = kwi(n, pos, "DEFAULT_VALUE")) >= 0) CPY(t->default_text, tk(n, j + 1));
    if ((j = kwi(n, pos, "DEFAULT_VALUE_NUMERIC")) >= 0) { t->has_default_num = 1; t->default_num = tnum(n, j + 1); }
}

static void read_addr_opts(const A2lNode* n, int start, uint32_t* bitmask, uint8_t* ext, int* number)
{
    int j;
    if ((j = kwi(n, start, "BIT_MASK")) >= 0) *bitmask = tu32(n, j + 1);
    if ((j = kwi(n, start, "ECU_ADDRESS_EXTENSION")) >= 0) *ext = (uint8_t)tu32(n, j + 1);
    if (number) {
        if ((j = kwi(n, start, "NUMBER")) >= 0) *number = (int)tint(n, j + 1);
        if ((j = kwi(n, start, "ARRAY_SIZE")) >= 0) *number = (int)tint(n, j + 1);
        if ((j = kwi(n, start, "MATRIX_DIM")) >= 0) {
            int k, prod = 1;
            for (k = j + 1; k < n->ntok && k <= j + 3 && isdigit((unsigned char)n->tok[k].s[0]); k++)
                prod *= (int)tint(n, k) ? (int)tint(n, k) : 1;
            *number = prod;
        }
    }
}

static void parse_meas(A2lFile* f, const A2lNode* n)
{
    A2lMeasurement* m = GROW(f->meas, f->nmeas);
    int j;
    CPY(m->name, tk(n, 0));
    CPY(m->longid, tk(n, 1));
    CPY(m->datatype, tk(n, 2));
    CPY(m->conversion, tk(n, 3));
    m->lower = tnum(n, 6);
    m->upper = tnum(n, 7);
    m->array_size = 1;
    if ((j = kwi(n, 8, "ECU_ADDRESS")) >= 0) { m->address = tu32(n, j + 1); m->has_address = 1; }
    if ((j = kwi(n, 8, "PHYS_UNIT")) >= 0) CPY(m->unit, tk(n, j + 1));
    read_addr_opts(n, 8, &m->bit_mask, &m->addr_ext, &m->array_size);
}

static A2lAxisAttr axis_attr(const char* s)
{
    if (!strcmp(s, "STD_AXIS")) return A2L_AX_STD;
    if (!strcmp(s, "FIX_AXIS")) return A2L_AX_FIX;
    if (!strcmp(s, "COM_AXIS")) return A2L_AX_COM;
    if (!strcmp(s, "RES_AXIS")) return A2L_AX_RES;
    if (!strcmp(s, "CURVE_AXIS")) return A2L_AX_CURVE;
    return A2L_AX_UNKNOWN;
}

static void parse_axis_descr(A2lFile* f, const A2lNode* n, A2lAxisDescr* a)
{
    int j, k;
    const A2lNode* fl;
    a->attr = axis_attr(tk(n, 0));
    CPY(a->input, tk(n, 1));
    CPY(a->conversion, tk(n, 2));
    a->max_points = (int)tint(n, 3);
    a->lower = tnum(n, 4);
    a->upper = tnum(n, 5);
    a->axis_pts = -1;
    if ((j = kwi(n, 6, "AXIS_PTS_REF")) >= 0) CPY(a->axis_pts_ref, tk(n, j + 1));
    if ((j = kwi(n, 6, "CURVE_AXIS_REF")) >= 0) CPY(a->curve_axis_ref, tk(n, j + 1));
    if ((j = kwi(n, 6, "FIX_AXIS_PAR")) >= 0) {
        a->fix_kind = 1; a->fix_offset = tnum(n, j + 1); a->fix_shift_or_dist = tnum(n, j + 2); a->fix_count = (int)tint(n, j + 3);
    } else if ((j = kwi(n, 6, "FIX_AXIS_PAR_DIST")) >= 0) {
        a->fix_kind = 2; a->fix_offset = tnum(n, j + 1); a->fix_shift_or_dist = tnum(n, j + 2); a->fix_count = (int)tint(n, j + 3);
    } else if ((fl = a2l_node_child(n, "FIX_AXIS_PAR_LIST", 0)) != NULL) {
        a->fix_kind = 3;
        a->fix_list = (double*)need(f, xcalloc((size_t)(fl->ntok ? fl->ntok : 1), sizeof(double)));
        a->fix_count = fl->ntok;
        for (k = 0; k < fl->ntok; k++) a->fix_list[k] = tnum(fl, k);
    }
}

static void parse_chr(A2lFile* f, const A2lNode* n)
{
    A2lCharacteristic* c = GROW(f->chr, f->nchr);
    int i;
    CPY(c->name, tk(n, 0));
    CPY(c->longid, tk(n, 1));
    CPY(c->type, tk(n, 2));
    c->address = tu32(n, 3);
    CPY(c->deposit, tk(n, 4));
    c->max_diff = tnum(n, 5);
    CPY(c->conversion, tk(n, 6));
    c->lower = tnum(n, 7);
    c->upper = tnum(n, 8);
    c->number = 1;
    c->read_only = kwi(n, 9, "READ_ONLY") >= 0;
    read_addr_opts(n, 9, &c->bit_mask, &c->addr_ext, &c->number);
    for (i = 0; i < n->nkid && c->naxis < 3; i++)
        if (!strcmp(n->kid[i]->kw, "AXIS_DESCR")) { A2lAxisDescr* ad = &c->axis[c->naxis++]; parse_axis_descr(f, n->kid[i], ad); }
}

static void parse_axpts(A2lFile* f, const A2lNode* n)
{
    A2lAxisPts* a = GROW(f->axpts, f->naxpts);
    CPY(a->name, tk(n, 0));
    CPY(a->longid, tk(n, 1));
    a->address = tu32(n, 2);
    CPY(a->input, tk(n, 3));
    CPY(a->deposit, tk(n, 4));
    a->max_diff = tnum(n, 5);
    CPY(a->conversion, tk(n, 6));
    a->max_points = (int)tint(n, 7);
    a->lower = tnum(n, 8);
    a->upper = tnum(n, 9);
}

static void parse_rl(A2lFile* f, const A2lNode* n)
{
    A2lRecordLayout* r = GROW(f->rl, f->nrl);
    int j;
    CPY(r->name, tk(n, 0));
    if ((j = kwi(n, 1, "FNC_VALUES")) >= 0) { r->fnc_pos = (int)tint(n, j + 1); CPY(r->fnc_type, tk(n, j + 2)); CPY(r->fnc_index_mode, tk(n, j + 3)); }
    if ((j = kwi(n, 1, "AXIS_PTS_X")) >= 0) { r->axis_x_pos = (int)tint(n, j + 1); CPY(r->axis_x_type, tk(n, j + 2)); }
    if ((j = kwi(n, 1, "AXIS_PTS_Y")) >= 0) { r->axis_y_pos = (int)tint(n, j + 1); CPY(r->axis_y_type, tk(n, j + 2)); }
    if ((j = kwi(n, 1, "NO_AXIS_PTS_X")) >= 0) { r->no_axis_x_pos = (int)tint(n, j + 1); CPY(r->no_axis_x_type, tk(n, j + 2)); }
    if ((j = kwi(n, 1, "NO_AXIS_PTS_Y")) >= 0) { r->no_axis_y_pos = (int)tint(n, j + 1); CPY(r->no_axis_y_type, tk(n, j + 2)); }
}

static void push_refs(A2lFile* f, const A2lNode* n, const char* kw, char*** arr, int* cnt)
{
    int i, k;
    for (i = 0; i < n->nkid; i++)
        if (!strcmp(n->kid[i]->kw, kw))
            for (k = 0; k < n->kid[i]->ntok; k++) str_push(f, arr, cnt, n->kid[i]->tok[k].s);
}

static void parse_group(A2lFile* f, const A2lNode* n, int is_function)
{
    A2lGroup* g = GROW(f->grp, f->ngrp);
    CPY(g->name, tk(n, 0));
    CPY(g->longid, tk(n, 1));
    g->is_function = is_function;
    g->root = kwi(n, 2, "ROOT") >= 0;
    push_refs(f, n, "REF_CHARACTERISTIC", &g->chars, &g->nchars);
    push_refs(f, n, "DEF_CHARACTERISTIC", &g->chars, &g->nchars);
    push_refs(f, n, "REF_MEASUREMENT", &g->meas, &g->nmeas);
    push_refs(f, n, "IN_MEASUREMENT", &g->meas, &g->nmeas);
    push_refs(f, n, "OUT_MEASUREMENT", &g->meas, &g->nmeas);
    push_refs(f, n, "LOC_MEASUREMENT", &g->meas, &g->nmeas);
    push_refs(f, n, "SUB_GROUP", &g->subs, &g->nsubs);
    push_refs(f, n, "SUB_FUNCTION", &g->subs, &g->nsubs);
}

static void parse_segment(A2lFile* f, const A2lNode* n)
{
    A2lMemSegment* s = GROW(f->seg, f->nseg);
    int i;
    CPY(s->name, tk(n, 0));
    CPY(s->longid, tk(n, 1));
    CPY(s->prg_type, tk(n, 2));
    CPY(s->mem_type, tk(n, 3));
    CPY(s->attribute, tk(n, 4));
    s->address = tu32(n, 5);
    s->size = tu32(n, 6);
    s->xcp_segment = -1;
    for (i = 0; i < n->nkid; i++) {
        const A2lNode* d = n->kid[i];
        const A2lNode* sg;
        if (strcmp(d->kw, "IF_DATA") || (strcmp(tk(d, 0), "XCP") && strcmp(tk(d, 0), "XCPplus"))) continue;
        if ((sg = a2l_node_child(d, "SEGMENT", 0)) != NULL) {
            s->xcp_segment = (int)tint(sg, 0);
            s->xcp_pages = (int)tint(sg, 1);
            s->xcp_addr_ext = (uint8_t)tint(sg, 2);
        }
    }
}

static void parse_module(A2lFile* f, const A2lNode* m)
{
    int i, j;
    CPY(f->module, tk(m, 0));
    for (i = 0; i < m->nkid; i++) {
        const A2lNode* n = m->kid[i];
        const char* kw = n->kw;
        if (!strcmp(kw, "COMPU_METHOD")) parse_compu(f, n);
        else if (!strcmp(kw, "COMPU_TAB")) parse_tab(f, n, 0, 0);
        else if (!strcmp(kw, "COMPU_VTAB")) parse_tab(f, n, 1, 0);
        else if (!strcmp(kw, "COMPU_VTAB_RANGE")) parse_tab(f, n, 1, 1);
        else if (!strcmp(kw, "MEASUREMENT")) parse_meas(f, n);
        else if (!strcmp(kw, "CHARACTERISTIC")) parse_chr(f, n);
        else if (!strcmp(kw, "AXIS_PTS")) parse_axpts(f, n);
        else if (!strcmp(kw, "RECORD_LAYOUT")) parse_rl(f, n);
        else if (!strcmp(kw, "GROUP")) parse_group(f, n, 0);
        else if (!strcmp(kw, "FUNCTION")) parse_group(f, n, 1);
        else if (!strcmp(kw, "IF_DATA") && (!strcmp(tk(n, 0), "XCP") || !strcmp(tk(n, 0), "XCPplus")))
            parse_if_data_xcp(f, n, &f->xcp);
        else if (!strcmp(kw, "MOD_COMMON")) {
            if ((j = kwi(n, 1, "BYTE_ORDER")) >= 0) f->byte_order_msb_first = !strcmp(tk(n, j + 1), "MSB_FIRST");
        } else if (!strcmp(kw, "MOD_PAR")) {
            int k;
            if ((j = kwi(n, 1, "EPK")) >= 0) CPY(f->epk, tk(n, j + 1));
            if ((j = kwi(n, 1, "ADDR_EPK")) >= 0) { f->addr_epk = tu32(n, j + 1); f->has_addr_epk = 1; }
            for (k = 0; k < n->nkid; k++)
                if (!strcmp(n->kid[k]->kw, "MEMORY_SEGMENT")) parse_segment(f, n->kid[k]);
        }
    }
    /* 解析引用 */
    for (i = 0; i < f->ncompu; i++) {
        A2lCompu* c = &f->compu[i];
        if (!c->tab_ref[0]) continue;
        for (j = 0; j < f->ntab; j++)
            if (!strcmp(f->tab[j].name, c->tab_ref)) { c->tab = j; break; }
    }
    for (i = 0; i < f->nchr; i++) {
        int a;
        for (a = 0; a < f->chr[i].naxis; a++) {
            A2lAxisDescr* ad = &f->chr[i].axis[a];
            if (!ad->axis_pts_ref[0]) continue;
            for (j = 0; j < f->naxpts; j++)
                if (!strcmp(f->axpts[j].name, ad->axis_pts_ref)) { ad->axis_pts = j; break; }
        }
    }
}

A2lFile* a2l_parse_text(const char* text, size_t len, char* errbuf, int errcap)
{
    char err[256] = "";
    A2lNode* root = build_tree(text, len, err, (int)sizeof(err));
    A2lFile* f;
    const A2lNode* proj;
    const A2lNode* mod;
    if (!root) {
        if (errbuf && errcap > 0) snprintf(errbuf, (size_t)errcap, "%s", err);
        return NULL;
    }
    proj = a2l_node_child(root, "PROJECT", 0);
    mod = proj ? a2l_node_child(proj, "MODULE", 0) : a2l_node_child(root, "MODULE", 0);
    if (!mod) {
        if (errbuf && errcap > 0) snprintf(errbuf, (size_t)errcap, "未找到 /begin MODULE");
        node_free(root);
        return NULL;
    }
    f = (A2lFile*)xcalloc(1, sizeof(A2lFile));
    if (!f) {
        if (errbuf && errcap > 0) snprintf(errbuf, (size_t)errcap, OOM_MSG);
        node_free(root);
        return NULL;
    }
    f->root = root;
    {
        Ctx ctx;
        f->priv = &ctx;
        if (setjmp(ctx.jb) != 0) {
            if (errbuf && errcap > 0) snprintf(errbuf, (size_t)errcap, OOM_MSG);
            a2l_free(f);
            return NULL;
        }
        if (proj) CPY(f->project, tk(proj, 0));
        parse_module(f, mod);
        f->priv = NULL;
    }
    return f;
}

A2lFile* a2l_parse_file(const char* path, char* errbuf, int errcap)
{
    FILE* fp = fopen(path, "rb");
    long n;
    char* buf;
    A2lFile* f;
    if (!fp) { if (errbuf) snprintf(errbuf, (size_t)errcap, "无法打开 %s", path); return NULL; }
    fseek(fp, 0, SEEK_END);
    n = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (n < 0) { fclose(fp); return NULL; }
    buf = (char*)xrealloc(NULL, (size_t)n + 1);
    if (!buf) { fclose(fp); if (errbuf) snprintf(errbuf, (size_t)errcap, OOM_MSG); return NULL; }
    if (fread(buf, 1, (size_t)n, fp) != (size_t)n) { fclose(fp); free(buf); if (errbuf) snprintf(errbuf, (size_t)errcap, "读取失败"); return NULL; }
    fclose(fp);
    buf[n] = 0;
    /* 跳过 UTF-8 BOM */
    if (n >= 3 && (unsigned char)buf[0] == 0xEF && (unsigned char)buf[1] == 0xBB && (unsigned char)buf[2] == 0xBF)
        f = a2l_parse_text(buf + 3, (size_t)n - 3, errbuf, errcap);
    else
        f = a2l_parse_text(buf, (size_t)n, errbuf, errcap);
    free(buf);
    return f;
}

static void free_proto(A2lXcpProto* p) { str_free(p->optional_cmd, p->noptional_cmd); }

void a2l_free(A2lFile* f)
{
    int i, a;
    if (!f) return;
    node_free(f->root);
    for (i = 0; i < f->ntab; i++) free(f->tab[i].e);
    for (i = 0; i < f->nchr; i++)
        for (a = 0; a < f->chr[i].naxis; a++) free(f->chr[i].axis[a].fix_list);
    for (i = 0; i < f->ngrp; i++) {
        str_free(f->grp[i].chars, f->grp[i].nchars);
        str_free(f->grp[i].meas, f->grp[i].nmeas);
        str_free(f->grp[i].subs, f->grp[i].nsubs);
    }
    free_proto(&f->xcp.proto);
    free(f->xcp.daq.ev);
    for (i = 0; i < f->xcp.ntl; i++) { free_proto(&f->xcp.tl[i].proto); free(f->xcp.tl[i].daq.ev); }
    free(f->xcp.tl);
    free(f->compu); free(f->tab); free(f->meas); free(f->chr); free(f->axpts);
    free(f->rl); free(f->grp); free(f->seg);
    free(f);
}

/* ============================ 查找 ============================ */

#define FIND(arr, cnt) do { int i_; if (!f || !name) return NULL; \
    for (i_ = 0; i_ < f->cnt; i_++) { if (!strcmp(f->arr[i_].name, name)) return &f->arr[i_]; } \
    return NULL; } while (0)

const A2lCompu*          a2l_find_compu(const A2lFile* f, const char* name) { FIND(compu, ncompu); }
const A2lMeasurement*    a2l_find_meas(const A2lFile* f, const char* name) { FIND(meas, nmeas); }
const A2lCharacteristic* a2l_find_chr(const A2lFile* f, const char* name) { FIND(chr, nchr); }
const A2lAxisPts*        a2l_find_axis_pts(const A2lFile* f, const char* name) { FIND(axpts, naxpts); }
const A2lRecordLayout*   a2l_find_record_layout(const A2lFile* f, const char* name) { FIND(rl, nrl); }
const A2lGroup*          a2l_find_group(const A2lFile* f, const char* name) { FIND(grp, ngrp); }

const A2lMemSegment* a2l_find_segment(const A2lFile* f, uint32_t addr, uint32_t len)
{
    int i;
    if (!f) return NULL;
    for (i = 0; i < f->nseg; i++) {
        const A2lMemSegment* s = &f->seg[i];
        if (addr >= s->address && (uint64_t)addr + len <= (uint64_t)s->address + s->size) return s;
    }
    return NULL;
}

/* ============================ 公式 ============================ */

typedef struct Ex { const char* p; double x; int ok; } Ex;
static double ex_add(Ex* e);

static void ex_ws(Ex* e) { while (isspace((unsigned char)*e->p)) e->p++; }

static double ex_atom(Ex* e)
{
    double v;
    ex_ws(e);
    if (*e->p == '(') {
        e->p++;
        v = ex_add(e);
        ex_ws(e);
        if (*e->p == ')') e->p++; else e->ok = 0;
        return v;
    }
    if (*e->p == '-') { e->p++; return -ex_atom(e); }
    if (*e->p == '+') { e->p++; return ex_atom(e); }
    if (isdigit((unsigned char)*e->p) || *e->p == '.') {
        char* end;
        v = strtod(e->p, &end);
        e->p = end;
        return v;
    }
    if (isalpha((unsigned char)*e->p) || *e->p == '_') {
        char id[16];
        int n = 0;
        while ((isalnum((unsigned char)*e->p) || *e->p == '_') && n < 15) id[n++] = *e->p++;
        id[n] = 0;
        if (!strcmp(id, "X") || !strcmp(id, "X1") || !strcmp(id, "x") || !strcmp(id, "x1")) return e->x;
        ex_ws(e);
        if (*e->p == '(') {
            double a = ex_atom(e);
            if (!strcmp(id, "sin")) return sin(a);
            if (!strcmp(id, "cos")) return cos(a);
            if (!strcmp(id, "tan")) return tan(a);
            if (!strcmp(id, "exp")) return exp(a);
            if (!strcmp(id, "log")) return log(a);
            if (!strcmp(id, "log10")) return log10(a);
            if (!strcmp(id, "sqrt")) return sqrt(a);
            if (!strcmp(id, "abs") || !strcmp(id, "fabs")) return fabs(a);
        }
    }
    e->ok = 0;
    return 0;
}

static double ex_pow(Ex* e)
{
    double b = ex_atom(e);
    ex_ws(e);
    if (*e->p == '^') { e->p++; return pow(b, ex_pow(e)); }
    return b;
}

static double ex_mul(Ex* e)
{
    double v = ex_pow(e);
    for (;;) {
        ex_ws(e);
        if (*e->p == '*') { e->p++; v *= ex_pow(e); }
        else if (*e->p == '/') { e->p++; v /= ex_pow(e); }
        else return v;
    }
}

static double ex_add(Ex* e)
{
    double v = ex_mul(e);
    for (;;) {
        ex_ws(e);
        if (*e->p == '+') { e->p++; v += ex_mul(e); }
        else if (*e->p == '-') { e->p++; v -= ex_mul(e); }
        else return v;
    }
}

double a2l_eval_formula(const char* expr, double x, int* ok)
{
    Ex e;
    double v;
    e.p = expr; e.x = x; e.ok = 1;
    v = ex_add(&e);
    ex_ws(&e);
    if (*e.p) e.ok = 0;
    if (ok) *ok = e.ok;
    return v;
}

/* ============================ 换算 ============================ */

static const A2lCompuTab* tab_of(const A2lFile* f, const A2lCompu* c)
{
    return (f && c && c->tab >= 0 && c->tab < f->ntab) ? &f->tab[c->tab] : NULL;
}

double a2l_to_phys(const A2lFile* f, const A2lCompu* c, double raw, int* ok)
{
    int dummy;
    if (!ok) ok = &dummy;
    *ok = 1;
    if (!c) return raw;
    switch (c->type) {
    case A2L_CM_IDENTICAL: return raw;
    case A2L_CM_LINEAR: return c->coeffs[0] * raw + c->coeffs[1];
    case A2L_CM_RAT_FUNC: {
        /* ASAP2：raw = (a·p² + b·p + c) / (d·p² + e·p + f)，求 p */
        const double* k = c->coeffs;
        double A = k[0] - k[3] * raw, B = k[1] - k[4] * raw, C = k[2] - k[5] * raw;
        if (fabs(A) < 1e-300) {
            if (fabs(B) < 1e-300) { *ok = 0; return 0; }
            return -C / B;
        } else {
            double disc = B * B - 4 * A * C;
            if (disc < 0) { *ok = 0; return 0; }
            return (-B + sqrt(disc)) / (2 * A);
        }
    }
    case A2L_CM_FORM: return a2l_eval_formula(c->formula, raw, ok);
    case A2L_CM_TAB_INTP:
    case A2L_CM_TAB_NOINTP: {
        const A2lCompuTab* t = tab_of(f, c);
        int i;
        if (!t || t->n == 0) { *ok = 0; return 0; }
        if (c->type == A2L_CM_TAB_NOINTP) {
            for (i = 0; i < t->n; i++) if (t->e[i].in == raw) return t->e[i].out;
            if (t->has_default_num) return t->default_num;
            /* 无精确匹配：取最近点 */
            {
                int best = 0;
                for (i = 1; i < t->n; i++) if (fabs(t->e[i].in - raw) < fabs(t->e[best].in - raw)) best = i;
                return t->e[best].out;
            }
        }
        if (raw <= t->e[0].in) return t->e[0].out;
        if (raw >= t->e[t->n - 1].in) return t->e[t->n - 1].out;
        for (i = 1; i < t->n; i++)
            if (raw <= t->e[i].in) {
                double x0 = t->e[i - 1].in, x1 = t->e[i].in;
                return t->e[i - 1].out + (t->e[i].out - t->e[i - 1].out) * (raw - x0) / (x1 - x0);
            }
        return t->e[t->n - 1].out;
    }
    case A2L_CM_TAB_VERB: return raw;  /* 物理值即原值，文本用 a2l_to_text */
    default: *ok = 0; return raw;
    }
}

double a2l_to_raw(const A2lFile* f, const A2lCompu* c, double phys, int* ok)
{
    int dummy;
    if (!ok) ok = &dummy;
    *ok = 1;
    if (!c) return phys;
    switch (c->type) {
    case A2L_CM_IDENTICAL: case A2L_CM_TAB_VERB: return phys;
    case A2L_CM_LINEAR:
        if (c->coeffs[0] == 0) { *ok = 0; return 0; }
        return (phys - c->coeffs[1]) / c->coeffs[0];
    case A2L_CM_RAT_FUNC: {
        const double* k = c->coeffs;
        double den = k[3] * phys * phys + k[4] * phys + k[5];
        if (den == 0) { *ok = 0; return 0; }
        return (k[0] * phys * phys + k[1] * phys + k[2]) / den;
    }
    case A2L_CM_FORM:
        if (!c->formula_inv[0]) { *ok = 0; return 0; }
        return a2l_eval_formula(c->formula_inv, phys, ok);
    case A2L_CM_TAB_INTP:
    case A2L_CM_TAB_NOINTP: {
        const A2lCompuTab* t = tab_of(f, c);
        int i, best = 0;
        if (!t || t->n == 0) { *ok = 0; return 0; }
        if (c->type == A2L_CM_TAB_INTP)
            for (i = 1; i < t->n; i++) {
                double y0 = t->e[i - 1].out, y1 = t->e[i].out;
                if ((phys - y0) * (phys - y1) <= 0 && y0 != y1)
                    return t->e[i - 1].in + (t->e[i].in - t->e[i - 1].in) * (phys - y0) / (y1 - y0);
            }
        for (i = 1; i < t->n; i++) if (fabs(t->e[i].out - phys) < fabs(t->e[best].out - phys)) best = i;
        return t->e[best].in;
    }
    default: *ok = 0; return phys;
    }
}

const char* a2l_to_text(const A2lFile* f, const A2lCompu* c, double raw)
{
    const A2lCompuTab* t;
    int i;
    if (!c || c->type != A2L_CM_TAB_VERB) return NULL;
    t = tab_of(f, c);
    if (!t) return NULL;
    for (i = 0; i < t->n; i++)
        if (t->is_range ? (raw >= t->e[i].in && raw <= t->e[i].in_max) : raw == t->e[i].in) return t->e[i].text;
    return t->default_text[0] ? t->default_text : NULL;
}

double a2l_to_phys_by_name(const A2lFile* f, const char* compu, double raw, int* ok)
{
    const A2lCompu* c = (compu && strcmp(compu, "NO_COMPU_METHOD")) ? a2l_find_compu(f, compu) : NULL;
    if (compu && strcmp(compu, "NO_COMPU_METHOD") && !c) { if (ok) *ok = 0; return raw; }
    return a2l_to_phys(f, c, raw, ok);
}
