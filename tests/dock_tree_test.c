/* dock_tree 单元测试（纯 C；MSVC / gcc 均可编译） */
#include "dock_tree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int g_fail, g_n;
#define CHECK(c, m) do { g_n++; if (!(c)) { g_fail++; printf("FAIL: %s (line %d)\n", m, __LINE__); } } while (0)

static int map_plus100(int id, void* u) { (void)u; return id + 100; }
static int map_minus100(int n, void* u) { (void)u; return n - 100; }
static int map_skip3(int id, void* u) { (void)u; return id == 3 ? -1 : id; }

static DtRect R(int l, int t, int r, int b) { DtRect x; x.l = l; x.t = t; x.r = r; x.b = b; return x; }

int main(void)
{
    DtTree t, u;
    char buf[1024], buf2[1024];
    int l0, l1, l2, sp, ids[16], n;

    dt_init(&t, 6, 50);
    CHECK(dt_empty(&t) && dt_check(&t) == 0, "empty tree");
    l0 = dt_insert(&t, -1, 0, DT_ZONE_CENTER);
    CHECK(l0 >= 0 && t.root == l0 && dt_check(&t) == 0, "first insert becomes root");
    l1 = dt_insert(&t, l0, 1, DT_ZONE_RIGHT);
    CHECK(l1 >= 0 && t.n[t.root].type == DT_SPLIT && !t.n[t.root].vert, "split right → horizontal split");
    CHECK(t.n[t.root].a == l0 && t.n[t.root].b == l1, "right: new pane is second child");
    l2 = dt_insert(&t, l1, 2, DT_ZONE_TOP);
    CHECK(t.n[t.n[l2].parent].vert == 1 && t.n[t.n[l2].parent].a == l2, "split top → vertical split, new pane first");
    CHECK(dt_insert(&t, l2, 3, DT_ZONE_CENTER) == l2 && t.n[l2].ntabs == 2 && t.n[l2].active == 1, "center → tab, activated");
    CHECK(dt_insert(&t, l2, 3, DT_ZONE_LEFT) == -1, "duplicate id rejected");
    CHECK(dt_check(&t) == 0 && dt_panel_count(&t) == 4, "structure ok, 4 panels");
    n = dt_list(&t, ids, 16);
    CHECK(n == 4 && ids[0] == 0 && ids[1] == 2 && ids[2] == 3 && ids[3] == 1, "list order (DFS, tab order)");

    /* 布局 */
    dt_layout(&t, R(0, 0, 806, 600));
    CHECK(t.n[l0].rc.l == 0 && t.n[l0].rc.r == 400 && t.n[l1].rc.l == 406 && t.n[l1].rc.r == 806, "layout halves + gap");
    CHECK(t.n[l2].rc.t == 0 && t.n[l2].rc.b == 297 && t.n[l1].rc.t == 303, "nested vertical layout");
    sp = dt_hit_splitter(&t, 402, 100);
    CHECK(sp == t.root, "hit root splitter");
    CHECK(dt_hit_splitter(&t, 600, 299) == t.n[l2].parent, "hit nested splitter");
    CHECK(dt_hit_leaf(&t, 100, 100) == l0 && dt_hit_leaf(&t, 600, 500) == l1, "hit leaves");
    CHECK(dt_hit_leaf(&t, 402, 100) == -1, "gap is not a leaf");

    /* 拖分隔条 + 最小尺寸 */
    CHECK(dt_drag_splitter(&t, sp, 203, 10) == 1, "drag splitter");
    CHECK(t.n[l0].rc.r == 200, "left pane resized to 200");
    dt_drag_splitter(&t, sp, 5, 10);
    CHECK(t.n[l0].rc.r - t.n[l0].rc.l == 50, "min size clamps left");
    dt_drag_splitter(&t, sp, 800, 10);
    CHECK(t.n[l1].rc.r - t.n[l1].rc.l == 50, "min size clamps right");
    dt_layout(&t, R(0, 0, 80, 600));
    CHECK(t.n[l0].rc.r >= t.n[l0].rc.l && t.n[l1].rc.r <= 80, "too small area stays sane");

    /* 均分 */
    dt_insert(&t, l0, 4, DT_ZONE_RIGHT);   /* root: (l0 | 4) | l1-col */
    dt_equalize(&t, -1);
    dt_layout(&t, R(0, 0, 912, 600));
    {
        int a = dt_find_leaf(&t, 0), b = dt_find_leaf(&t, 4), c = dt_find_leaf(&t, 1);
        int wa = t.n[a].rc.r - t.n[a].rc.l, wb = t.n[b].rc.r - t.n[b].rc.l, wc = t.n[c].rc.r - t.n[c].rc.l;
        CHECK(abs(wa - wb) <= 6 && abs(wb - wc) <= 6 && abs(wa - wc) <= 6, "equalize 3 columns (within one gap)");
    }

    /* 落点 */
    {
        DtRect r = R(100, 100, 300, 300), z;
        CHECK(dt_zone_at(r, 110, 200, 0.25) == DT_ZONE_LEFT, "zone left");
        CHECK(dt_zone_at(r, 290, 200, 0.25) == DT_ZONE_RIGHT, "zone right");
        CHECK(dt_zone_at(r, 200, 105, 0.25) == DT_ZONE_TOP, "zone top");
        CHECK(dt_zone_at(r, 200, 295, 0.25) == DT_ZONE_BOTTOM, "zone bottom");
        CHECK(dt_zone_at(r, 200, 200, 0.25) == DT_ZONE_CENTER, "zone center");
        CHECK(dt_zone_at(r, 50, 50, 0.25) == DT_ZONE_NONE, "zone outside");
        z = dt_zone_rect(r, DT_ZONE_RIGHT);
        CHECK(z.l == 200 && z.r == 300 && z.t == 100, "zone rect right half");
    }

    /* 序列化往返 */
    n = dt_serialize(&t, buf, sizeof(buf), NULL, NULL);
    CHECK(n > 0, "serialize");
    printf("serialized: %s\n", buf);
    dt_init(&u, 6, 50);
    CHECK(dt_parse(&u, buf, NULL, NULL) == 0 && dt_check(&u) == 0, "parse back");
    dt_serialize(&u, buf2, sizeof(buf2), NULL, NULL);
    CHECK(strcmp(buf, buf2) == 0, "roundtrip identical");
    /* 映射 */
    dt_serialize(&t, buf, sizeof(buf), map_plus100, NULL);
    dt_parse(&u, buf, map_minus100, NULL);
    dt_serialize(&u, buf2, sizeof(buf2), NULL, NULL);
    dt_serialize(&t, buf, sizeof(buf), NULL, NULL);
    CHECK(strcmp(buf, buf2) == 0, "roundtrip through id mapping");
    /* 跳过面板 → 无空叶子 */
    dt_serialize(&t, buf, sizeof(buf), map_skip3, NULL);
    CHECK(strstr(buf, "3") == NULL || strstr(buf, "L3") == NULL, "skipped id not written");
    dt_parse(&u, buf, NULL, NULL);
    CHECK(dt_check(&u) == 0 && dt_find_leaf(&u, 3) < 0 && dt_panel_count(&u) == 4, "parse w/o skipped panel");
    /* 恢复时面板不存在 → 塌缩 */
    dt_serialize(&t, buf, sizeof(buf), NULL, NULL);
    dt_parse(&u, buf, map_skip3, NULL);
    dt_parse(&u, "Sh0.5(L7@0)(Sv0.3(L8@0)(L9@0))", map_skip3, NULL);
    CHECK(dt_check(&u) == 0 && dt_panel_count(&u) == 3, "parse literal");
    CHECK(dt_parse(&u, "Sh0.5(L7@0)(Sv0.3(Lx@0)(L9@0))", NULL, NULL) == -1 && dt_empty(&u), "bad format rejected");
    CHECK(dt_parse(&u, "Sh0.5(L7", NULL, NULL) == -1, "truncated rejected");
    CHECK(dt_parse(&u, "E", NULL, NULL) == 0 && dt_empty(&u), "empty marker");

    /* 移除与塌缩 */
    CHECK(dt_remove(&t, 3) == 0 && dt_check(&t) == 0, "remove tab");
    CHECK(dt_remove(&t, 2) == 0 && dt_check(&t) == 0, "remove → collapse leaf");
    CHECK(dt_remove(&t, 99) == -1, "remove missing");
    dt_remove(&t, 0); dt_remove(&t, 4);
    CHECK(dt_check(&t) == 0 && t.root == dt_find_leaf(&t, 1) && t.n[t.root].parent == -1, "single remaining pane is root");
    dt_remove(&t, 1);
    CHECK(dt_empty(&t) && dt_check(&t) == 0, "all removed → empty, no leaked nodes");

    /* 压力：随机插入/移除保持结构合法 */
    {
        int i, live[64] = {0}, ok = 1;
        srand(1234);
        dt_init(&t, 5, 40);
        for (i = 0; i < 5000; i++) {
            int id = rand() % 64;
            if (live[id]) { if (dt_remove(&t, id) != 0) ok = 0; live[id] = 0; }
            else {
                int leaf = dt_empty(&t) ? -1 : dt_find_leaf(&t, ids[0]);
                int lst[64], c = dt_list(&t, lst, 64);
                if (c > 0) leaf = dt_find_leaf(&t, lst[rand() % c]);
                if (dt_insert(&t, leaf, id, 1 + rand() % 5) >= 0) live[id] = 1;
            }
            if (dt_check(&t) != 0) { ok = 0; break; }
            if ((i % 97) == 0) {
                dt_serialize(&t, buf, sizeof(buf), NULL, NULL);
                if (dt_parse(&u, buf, NULL, NULL) != 0 || dt_panel_count(&u) != dt_panel_count(&t)) { ok = 0; break; }
                dt_layout(&t, R(0, 0, 1200, 800));
            }
        }
        CHECK(ok, "random insert/remove/serialize 5000 ops keep invariants");
    }
    if (g_fail) { printf("dock_tree_test: %d/%d FAILED\n", g_fail, g_n); return 1; }
    printf("dock_tree_test: %d checks ALL PASS\n", g_n);
    return 0;
}
