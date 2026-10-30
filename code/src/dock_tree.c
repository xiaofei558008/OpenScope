/* dock_tree —— 见 dock_tree.h（纯 C，无平台依赖） */
#include "dock_tree.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void dt_init(DtTree* t, int gap, int min_size)
{
    memset(t, 0, sizeof(*t));
    t->root = -1;
    t->gap = gap > 0 ? gap : 5;
    t->min_size = min_size > 0 ? min_size : 60;
}

void dt_set_hit_pad(DtTree* t, int pad) { t->hit_pad = pad > 0 ? pad : 0; }

int dt_empty(const DtTree* t) { return t->root < 0; }

static int node_alloc(DtTree* t, int type)
{
    int i;
    for (i = 0; i < DT_MAX_NODES; i++) {
        if (t->n[i].type == 0) {
            memset(&t->n[i], 0, sizeof(DtNode));
            t->n[i].type = type;
            t->n[i].parent = -1;
            t->n[i].a = t->n[i].b = -1;
            t->n[i].ratio = 0.5;
            return i;
        }
    }
    return -1;
}

int dt_find_leaf(const DtTree* t, int id)
{
    int i, k;
    for (i = 0; i < DT_MAX_NODES; i++) {
        if (t->n[i].type != DT_LEAF) continue;
        for (k = 0; k < t->n[i].ntabs; k++) if (t->n[i].tabs[k] == id) return i;
    }
    return -1;
}

int dt_first_leaf(const DtTree* t)
{
    int n = t->root;
    while (n >= 0 && t->n[n].type == DT_SPLIT) n = t->n[n].a;
    return n;
}

static int list_rec(const DtTree* t, int n, int* ids, int max, int cnt)
{
    int k;
    if (n < 0) return cnt;
    if (t->n[n].type == DT_LEAF) {
        for (k = 0; k < t->n[n].ntabs; k++) { if (cnt < max) ids[cnt] = t->n[n].tabs[k]; cnt++; }
        return cnt;
    }
    cnt = list_rec(t, t->n[n].a, ids, max, cnt);
    return list_rec(t, t->n[n].b, ids, max, cnt);
}

int dt_list(const DtTree* t, int* ids, int max) { return list_rec(t, t->root, ids, max, 0); }

int dt_panel_count(const DtTree* t)
{
    int i, c = 0;
    for (i = 0; i < DT_MAX_NODES; i++) if (t->n[i].type == DT_LEAF) c += t->n[i].ntabs;
    return c;
}

static void replace_child(DtTree* t, int parent, int old, int nw)
{
    if (parent < 0) { t->root = nw; }
    else if (t->n[parent].a == old) t->n[parent].a = nw;
    else t->n[parent].b = nw;
    if (nw >= 0) t->n[nw].parent = parent;
}

int dt_insert(DtTree* t, int leaf, int id, int zone)
{
    int nl, sp, parent;
    if (dt_find_leaf(t, id) >= 0) return -1;
    if (t->root < 0) {
        nl = node_alloc(t, DT_LEAF);
        if (nl < 0) return -1;
        t->n[nl].tabs[0] = id; t->n[nl].ntabs = 1; t->n[nl].active = 0;
        t->root = nl;
        return nl;
    }
    if (leaf < 0) { leaf = t->root; if (zone == DT_ZONE_CENTER) leaf = dt_first_leaf(t); }
    if (leaf < 0 || leaf >= DT_MAX_NODES || t->n[leaf].type == 0) return -1;
    if (zone == DT_ZONE_CENTER) {
        DtNode* L = &t->n[leaf];
        if (L->type != DT_LEAF || L->ntabs >= DT_MAX_TABS) return -1;
        L->tabs[L->ntabs] = id;
        L->active = L->ntabs++;
        return leaf;
    }
    if (zone < DT_ZONE_LEFT || zone > DT_ZONE_BOTTOM) return -1;
    nl = node_alloc(t, DT_LEAF);
    if (nl < 0) return -1;
    sp = node_alloc(t, DT_SPLIT);
    if (sp < 0) { t->n[nl].type = 0; return -1; }
    t->n[nl].tabs[0] = id; t->n[nl].ntabs = 1;
    parent = t->n[leaf].parent;
    t->n[sp].vert = (zone == DT_ZONE_TOP || zone == DT_ZONE_BOTTOM);
    t->n[sp].ratio = 0.5;
    if (zone == DT_ZONE_LEFT || zone == DT_ZONE_TOP) { t->n[sp].a = nl; t->n[sp].b = leaf; }
    else { t->n[sp].a = leaf; t->n[sp].b = nl; }
    replace_child(t, parent, leaf, sp);
    t->n[leaf].parent = sp;
    t->n[nl].parent = sp;
    return nl;
}

int dt_remove(DtTree* t, int id)
{
    int leaf = dt_find_leaf(t, id), k;
    DtNode* L;
    if (leaf < 0) return -1;
    L = &t->n[leaf];
    for (k = 0; k < L->ntabs; k++) if (L->tabs[k] == id) break;
    memmove(&L->tabs[k], &L->tabs[k + 1], (size_t)(L->ntabs - k - 1) * sizeof(int));
    L->ntabs--;
    if (L->active >= L->ntabs) L->active = L->ntabs - 1;
    else if (L->active > k) L->active--;
    if (L->active < 0) L->active = 0;
    if (L->ntabs > 0) return 0;
    /* 塌缩 */
    {
        int parent = L->parent;
        L->type = 0;
        if (parent < 0) { t->root = -1; return 0; }
        {
            int sib = t->n[parent].a == leaf ? t->n[parent].b : t->n[parent].a;
            int gp = t->n[parent].parent;
            t->n[parent].type = 0;
            replace_child(t, gp, parent, sib);
        }
    }
    return 0;
}

int dt_activate(DtTree* t, int id)
{
    int leaf = dt_find_leaf(t, id), k;
    if (leaf < 0) return -1;
    for (k = 0; k < t->n[leaf].ntabs; k++) if (t->n[leaf].tabs[k] == id) t->n[leaf].active = k;
    return leaf;
}

int dt_active_id(const DtTree* t, int leaf)
{
    if (leaf < 0 || leaf >= DT_MAX_NODES || t->n[leaf].type != DT_LEAF || t->n[leaf].ntabs <= 0) return -1;
    return t->n[leaf].tabs[t->n[leaf].active];
}

/* 子树在某方向上的最小尺寸 */
static int min_extent(const DtTree* t, int n, int vert)
{
    const DtNode* N;
    if (n < 0) return 0;
    N = &t->n[n];
    if (N->type == DT_LEAF) return t->min_size;
    if (N->vert == vert)
        return min_extent(t, N->a, vert) + t->gap + min_extent(t, N->b, vert);
    {
        int ea = min_extent(t, N->a, vert), eb = min_extent(t, N->b, vert);
        return ea > eb ? ea : eb;
    }
}

static int clamp_first(const DtTree* t, int n, int total, int first)
{
    const DtNode* N = &t->n[n];
    int mina = min_extent(t, N->a, N->vert), minb = min_extent(t, N->b, N->vert);
    int avail = total - t->gap;
    if (avail <= 0) return 0;
    if (mina + minb > avail) {           /* 空间不够：按最小尺寸比例分 */
        double s = (double)mina / (double)(mina + minb);
        return (int)(avail * s + 0.5);
    }
    if (first < mina) first = mina;
    if (first > avail - minb) first = avail - minb;
    return first;
}

static void layout_rec(DtTree* t, int n, DtRect r)
{
    DtNode* N;
    if (n < 0) return;
    N = &t->n[n];
    N->rc = r;
    if (N->type != DT_SPLIT) return;
    {
        int total = N->vert ? (r.b - r.t) : (r.r - r.l);
        int first = clamp_first(t, n, total, (int)((total - t->gap) * N->ratio + 0.5));
        DtRect ra = r, rb = r, sr = r;
        if (N->vert) {
            ra.b = r.t + first; sr.t = ra.b; sr.b = sr.t + t->gap; rb.t = sr.b;
        } else {
            ra.r = r.l + first; sr.l = ra.r; sr.r = sr.l + t->gap; rb.l = sr.r;
        }
        N->split_rc = sr;
        layout_rec(t, N->a, ra);
        layout_rec(t, N->b, rb);
    }
}

void dt_layout(DtTree* t, DtRect area) { layout_rec(t, t->root, area); }

static int in_rect(DtRect r, int x, int y) { return x >= r.l && x < r.r && y >= r.t && y < r.b; }

int dt_hit_splitter(const DtTree* t, int x, int y)
{
    int i;
    for (i = 0; i < DT_MAX_NODES; i++)
        if (t->n[i].type == DT_SPLIT && in_rect(t->n[i].split_rc, x, y)) return i;
    if (t->hit_pad > 0) { /* UI P0: thin visual gap, wider drag hit area */
        for (i = 0; i < DT_MAX_NODES; i++) {
            DtRect r;
            if (t->n[i].type != DT_SPLIT) continue;
            r = t->n[i].split_rc;
            if (t->n[i].vert) { r.t -= t->hit_pad; r.b += t->hit_pad; }
            else { r.l -= t->hit_pad; r.r += t->hit_pad; }
            if (in_rect(r, x, y)) return i;
        }
    }
    return -1;
}

int dt_hit_leaf(const DtTree* t, int x, int y)
{
    int i;
    for (i = 0; i < DT_MAX_NODES; i++)
        if (t->n[i].type == DT_LEAF && in_rect(t->n[i].rc, x, y)) return i;
    return -1;
}

int dt_drag_splitter(DtTree* t, int split, int x, int y)
{
    DtNode* N;
    int total, first;
    double nr;
    if (split < 0 || split >= DT_MAX_NODES || t->n[split].type != DT_SPLIT) return 0;
    N = &t->n[split];
    total = N->vert ? (N->rc.b - N->rc.t) : (N->rc.r - N->rc.l);
    if (total - t->gap <= 0) return 0;
    first = N->vert ? (y - N->rc.t - t->gap / 2) : (x - N->rc.l - t->gap / 2);
    first = clamp_first(t, split, total, first);
    nr = (double)first / (double)(total - t->gap);
    if (nr < 0.01) nr = 0.01;
    if (nr > 0.99) nr = 0.99;
    if (nr == N->ratio) return 0;
    N->ratio = nr;
    layout_rec(t, split, N->rc);
    return 1;
}

/* 同方向连续分割上的窗格数 */
static int run_count(const DtTree* t, int n, int vert)
{
    if (n < 0) return 0;
    if (t->n[n].type == DT_SPLIT && t->n[n].vert == vert)
        return run_count(t, t->n[n].a, vert) + run_count(t, t->n[n].b, vert);
    return 1;
}

static void equalize_rec(DtTree* t, int n)
{
    DtNode* N;
    if (n < 0 || t->n[n].type != DT_SPLIT) return;
    N = &t->n[n];
    {
        int ca = run_count(t, N->a, N->vert), cb = run_count(t, N->b, N->vert);
        N->ratio = (double)ca / (double)(ca + cb);
    }
    equalize_rec(t, N->a);
    equalize_rec(t, N->b);
}

void dt_equalize(DtTree* t, int node) { equalize_rec(t, node < 0 ? t->root : node); }

int dt_zone_at(DtRect r, int x, int y, double edge_frac)
{
    double w = r.r - r.l, h = r.b - r.t, fx, fy, d[4];
    int best = 0, i;
    if (!in_rect(r, x, y) || w <= 0 || h <= 0) return DT_ZONE_NONE;
    fx = (x - r.l) / w; fy = (y - r.t) / h;
    d[0] = fx; d[1] = 1 - fx; d[2] = fy; d[3] = 1 - fy;
    for (i = 1; i < 4; i++) if (d[i] < d[best]) best = i;
    if (d[best] > edge_frac) return DT_ZONE_CENTER;
    return DT_ZONE_LEFT + best;
}

DtRect dt_zone_rect(DtRect r, int zone)
{
    DtRect o = r;
    switch (zone) {
    case DT_ZONE_LEFT:   o.r = r.l + (r.r - r.l) / 2; break;
    case DT_ZONE_RIGHT:  o.l = r.l + (r.r - r.l) / 2; break;
    case DT_ZONE_TOP:    o.b = r.t + (r.b - r.t) / 2; break;
    case DT_ZONE_BOTTOM: o.t = r.t + (r.b - r.t) / 2; break;
    default: break;
    }
    return o;
}

/* ---------- 序列化 ---------- */
typedef struct { char* p; int cap, len, err; } SBuf;
static void sput(SBuf* s, const char* fmt, ...)
{
    va_list ap;
    int n;
    if (s->err) return;
    va_start(ap, fmt);
    n = vsnprintf(s->p + s->len, (size_t)(s->cap - s->len), fmt, ap);
    va_end(ap);
    if (n < 0 || s->len + n >= s->cap) { s->err = 1; return; }
    s->len += n;
}

/* 节点是否含有可写出面板 */
static int has_out(const DtTree* t, int n, int (*m)(int, void*), void* u)
{
    int k;
    if (n < 0) return 0;
    if (t->n[n].type == DT_LEAF) {
        for (k = 0; k < t->n[n].ntabs; k++) if (!m || m(t->n[n].tabs[k], u) >= 0) return 1;
        return 0;
    }
    return has_out(t, t->n[n].a, m, u) || has_out(t, t->n[n].b, m, u);
}

static void ser_rec(const DtTree* t, int n, SBuf* s, int (*m)(int, void*), void* u)
{
    const DtNode* N = &t->n[n];
    if (N->type == DT_LEAF) {
        int k, first = 1, act = 0, w = 0;
        sput(s, "L");
        for (k = 0; k < N->ntabs; k++) {
            int o = m ? m(N->tabs[k], u) : N->tabs[k];
            if (o < 0) continue;
            if (k == N->active) act = w;
            sput(s, first ? "%d" : ",%d", o);
            first = 0; w++;
        }
        sput(s, "@%d", act);
        return;
    }
    {
        int ha = has_out(t, N->a, m, u), hb = has_out(t, N->b, m, u);
        if (ha && hb) {
            sput(s, "S%c%.4f(", N->vert ? 'v' : 'h', N->ratio);
            ser_rec(t, N->a, s, m, u);
            sput(s, ")(");
            ser_rec(t, N->b, s, m, u);
            sput(s, ")");
        } else if (ha) ser_rec(t, N->a, s, m, u);
        else if (hb) ser_rec(t, N->b, s, m, u);
    }
}

int dt_serialize(const DtTree* t, char* buf, int cap, int (*map_out)(int, void*), void* u)
{
    SBuf s;
    if (!buf || cap <= 1) return -1;
    s.p = buf; s.cap = cap; s.len = 0; s.err = 0;
    buf[0] = 0;
    if (t->root >= 0 && has_out(t, t->root, map_out, u)) ser_rec(t, t->root, &s, map_out, u);
    else sput(&s, "E");
    return s.err ? -1 : s.len;
}

typedef struct { const char* p; int (*m)(int, void*); void* u; int err; } PState;

static int parse_rec(DtTree* t, PState* ps, int parent)
{
    const char* p = ps->p;
    if (*p == 'L') {
        int nl = node_alloc(t, DT_LEAF), act = 0;
        if (nl < 0) { ps->err = 1; return -1; }
        t->n[nl].parent = parent;
        p++;
        while (*p && *p != '@') {
            char* e;
            long num = strtol(p, &e, 10);
            int id;
            if (e == p) { ps->err = 1; return -1; }
            p = e;
            id = ps->m ? ps->m((int)num, ps->u) : (int)num;
            if (id >= 0 && dt_find_leaf(t, id) < 0 && t->n[nl].ntabs < DT_MAX_TABS)
                t->n[nl].tabs[t->n[nl].ntabs++] = id;
            if (*p == ',') p++;
        }
        if (*p == '@') { char* e; act = (int)strtol(p + 1, &e, 10); p = e; }
        if (act >= t->n[nl].ntabs) act = t->n[nl].ntabs - 1;
        t->n[nl].active = act < 0 ? 0 : act;
        ps->p = p;
        if (t->n[nl].ntabs == 0) { t->n[nl].type = 0; return -2; } /* 空叶子：丢弃 */
        return nl;
    }
    if (*p == 'S') {
        int sp, a, b;
        char* e;
        double r;
        if (p[1] != 'h' && p[1] != 'v') { ps->err = 1; return -1; }
        sp = node_alloc(t, DT_SPLIT);
        if (sp < 0) { ps->err = 1; return -1; }
        t->n[sp].vert = p[1] == 'v';
        r = strtod(p + 2, &e);
        if (e == p + 2 || *e != '(') { ps->err = 1; return -1; }
        t->n[sp].ratio = (r > 0.01 && r < 0.99) ? r : 0.5;
        t->n[sp].parent = parent;
        ps->p = e + 1;
        a = parse_rec(t, ps, sp);
        if (ps->err || *ps->p != ')' || ps->p[1] != '(') { ps->err = 1; return -1; }
        ps->p += 2;
        b = parse_rec(t, ps, sp);
        if (ps->err || *ps->p != ')') { ps->err = 1; return -1; }
        ps->p++;
        if (a >= 0 && b >= 0) { t->n[sp].a = a; t->n[sp].b = b; return sp; }
        t->n[sp].type = 0;
        if (a >= 0) { t->n[a].parent = parent; return a; }
        if (b >= 0) { t->n[b].parent = parent; return b; }
        return -2;
    }
    ps->err = 1;
    return -1;
}

int dt_parse(DtTree* t, const char* s, int (*map_in)(int, void*), void* u)
{
    PState ps;
    int gap = t->gap, ms = t->min_size, hp = t->hit_pad, r;
    dt_init(t, gap, ms); t->hit_pad = hp;
    if (!s || !*s || *s == 'E') return 0;
    ps.p = s; ps.m = map_in; ps.u = u; ps.err = 0;
    r = parse_rec(t, &ps, -1);
    if (ps.err) { dt_init(t, gap, ms); t->hit_pad = hp; return -1; }
    t->root = r >= 0 ? r : -1;
    return 0;
}

static int check_rec(const DtTree* t, int n, int parent, int* seen)
{
    const DtNode* N;
    if (n < 0 || n >= DT_MAX_NODES) return -1;
    N = &t->n[n];
    if (seen[n]) return -1;
    seen[n] = 1;
    if (N->parent != parent) return -1;
    if (N->type == DT_LEAF) return (N->ntabs > 0 && N->active >= 0 && N->active < N->ntabs) ? 0 : -1;
    if (N->type != DT_SPLIT || !(N->ratio > 0 && N->ratio < 1)) return -1;
    if (check_rec(t, N->a, n, seen) || check_rec(t, N->b, n, seen)) return -1;
    return 0;
}

int dt_check(const DtTree* t)
{
    int seen[DT_MAX_NODES], i;
    memset(seen, 0, sizeof(seen));
    if (t->root < 0) {
        for (i = 0; i < DT_MAX_NODES; i++) if (t->n[i].type) return -1;
        return 0;
    }
    if (check_rec(t, t->root, -1, seen)) return -1;
    for (i = 0; i < DT_MAX_NODES; i++) if (t->n[i].type && !seen[i]) return -1; /* 泄漏节点 */
    return 0;
}
