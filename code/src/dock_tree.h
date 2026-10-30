/*
 * dock_tree —— Terminator 式平铺窗口管理的纯逻辑部分（无 Win32 依赖，可单测）
 *
 * 一棵分割树：叶子 = 一个窗格（含 1..N 个 tab，tab 元素为调用方的面板 id），
 * 内部节点 = 二分分割（水平排列 L|R 或垂直排列 T/B，ratio = 第一子所占比例）。
 * 提供：插入（按落点 左/右/上/下/中=tab）、移除（空窗格自动塌缩）、布局计算、
 * 分隔条命中/拖拽（含最小尺寸）、落点区域命中、均分、序列化/反序列化。
 */
#ifndef DOCK_TREE_H
#define DOCK_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

#define DT_MAX_NODES 128
#define DT_MAX_TABS  32

typedef struct { int l, t, r, b; } DtRect;

enum { DT_LEAF = 1, DT_SPLIT = 2 };
/* 落点区域 */
enum { DT_ZONE_NONE = 0, DT_ZONE_LEFT, DT_ZONE_RIGHT, DT_ZONE_TOP, DT_ZONE_BOTTOM, DT_ZONE_CENTER };

typedef struct {
    int    type;            /* 0=空闲 DT_LEAF DT_SPLIT */
    int    parent;          /* -1 = 根 */
    /* split */
    int    vert;            /* 0 = 左右排列（竖分隔条），1 = 上下排列（横分隔条） */
    double ratio;           /* 第一子占比 (0,1) */
    int    a, b;            /* 子节点 */
    /* leaf */
    int    tabs[DT_MAX_TABS];
    int    ntabs;
    int    active;          /* 当前 tab 下标 */
    /* 布局结果 */
    DtRect rc;              /* 节点整体矩形 */
    DtRect split_rc;        /* split：分隔条矩形 */
} DtNode;

typedef struct {
    DtNode n[DT_MAX_NODES];
    int    root;            /* -1 = 空树 */
    int    gap;             /* 分隔条宽度（像素） */
    int    hit_pad;         /* UI P0: extra splitter hit area per side (px) */
    int    min_size;        /* 窗格最小宽/高 */
} DtTree;

void dt_init(DtTree* t, int gap, int min_size);
void dt_set_hit_pad(DtTree* t, int pad);
int  dt_empty(const DtTree* t);
int  dt_find_leaf(const DtTree* t, int id);          /* 含 id 的叶子，-1 无 */
int  dt_first_leaf(const DtTree* t);
int  dt_panel_count(const DtTree* t);
/* 列出全部面板 id（深度优先、tab 顺序），返回个数 */
int  dt_list(const DtTree* t, int* ids, int max);

/* 插入：树空 → 成为根叶子；leaf<0 → 根旁（右侧）；zone=CENTER → 成为 leaf 的新 tab 并激活。
 * 返回新面板所在叶子，失败 -1。 */
int  dt_insert(DtTree* t, int leaf, int id, int zone);
/* 移除面板；叶子空则塌缩（兄弟顶替父节点）。返回 0 成功，-1 不存在 */
int  dt_remove(DtTree* t, int id);
/* 激活 tab */
int  dt_activate(DtTree* t, int id);
int  dt_active_id(const DtTree* t, int leaf);

/* 布局：计算各节点 rc / split_rc（ratio 按最小尺寸夹紧但不改写） */
void dt_layout(DtTree* t, DtRect area);
/* 命中：分隔条 → split 节点；叶子 → leaf 节点 */
int  dt_hit_splitter(const DtTree* t, int x, int y);
int  dt_hit_leaf(const DtTree* t, int x, int y);
/* 拖分隔条到坐标 (x,y)：按最小尺寸夹紧后写 ratio。返回 1 有变化 */
int  dt_drag_splitter(DtTree* t, int split, int x, int y);
/* 均分：split 及其同方向连续子分割，使各窗格等大（node<0 = 整树） */
void dt_equalize(DtTree* t, int node);
/* 落点区域：r 内 (x,y) → DT_ZONE_*；边缘 edge_frac（如 0.25）以内为四边，其余为中心 */
int  dt_zone_at(DtRect r, int x, int y, double edge_frac);
/* 落点高亮矩形（左右上下 = 半区，中心 = 整个） */
DtRect dt_zone_rect(DtRect r, int zone);

/* 序列化："S<h|v><ratio>(<a>)(<b>)" / "L<id>,<id>..@<active>"。
 * map_out(id) → 写出的编号（<0 跳过该面板）；返回长度，buf 不足返回 -1 */
int  dt_serialize(const DtTree* t, char* buf, int cap, int (*map_out)(int id, void* u), void* u);
/* 反序列化：map_in(编号) → 面板 id（<0 = 该面板已不存在，丢弃）。返回 0 成功，-1 格式错 */
int  dt_parse(DtTree* t, const char* s, int (*map_in)(int num, void* u), void* u);

/* 结构自检（单测用）：父子指针一致、无空叶子、ratio 合法。返回 0 正常 */
int  dt_check(const DtTree* t);

#ifdef __cplusplus
}
#endif
#endif
