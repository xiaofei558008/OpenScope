#ifndef A2L_PARSE_H
#define A2L_PARSE_H

/* ASAP2 / A2L 解析（B4，平台无关纯 C99，无 Win32 依赖）。
 * 覆盖：IF_DATA XCP（PROTOCOL_LAYER / DAQ+EVENT+TIMESTAMP / PAG / 各传输层及其覆盖参数 /
 *       SEED_AND_KEY_EXTERNAL_FUNCTION）、COMPU_METHOD（IDENTICAL/LINEAR/RAT_FUNC/FORM/
 *       TAB_INTP/TAB_NOINTP/TAB_VERB）+ COMPU_TAB/COMPU_VTAB/COMPU_VTAB_RANGE、GROUP/FUNCTION、
 *       MEASUREMENT、CHARACTERISTIC（VALUE/CURVE/MAP/VAL_BLK…）+ AXIS_DESCR、AXIS_PTS、
 *       RECORD_LAYOUT（主要字段）、MOD_PAR（EPK/ADDR_EPK/MEMORY_SEGMENT+XCP SEGMENT）、MOD_COMMON。
 * 先解析成通用块树（A2lNode），再抽取到结构体；未识别的块保留在树中可自行遍历。
 * 只解析第一个 MODULE。 */

#include <stddef.h>
#include <stdint.h>

#define A2L_NAME 128
#define A2L_TEXT 256

/* ---------- 通用块树 ---------- */
typedef struct A2lTok {
    char* s;
    int   quoted;
    int   line;
} A2lTok;

typedef struct A2lNode {
    char*            kw;      /* /begin 之后的关键字；根节点为 "" */
    A2lTok*          tok;     /* 块内非子块的所有记号（按顺序） */
    int              ntok;
    struct A2lNode** kid;
    int              nkid;
    int              line;
} A2lNode;

/* ---------- 抽取结果 ---------- */
typedef enum {
    A2L_CM_UNKNOWN = 0, A2L_CM_IDENTICAL, A2L_CM_LINEAR, A2L_CM_RAT_FUNC, A2L_CM_FORM,
    A2L_CM_TAB_INTP, A2L_CM_TAB_NOINTP, A2L_CM_TAB_VERB
} A2lCompuType;

typedef struct A2lTabEntry {
    double in, in_max;   /* VTAB_RANGE 用 in..in_max；其他 in_max = in */
    double out;
    char   text[A2L_NAME];
} A2lTabEntry;

typedef struct A2lCompuTab {
    char         name[A2L_NAME];
    int          kind;          /* A2lCompuType：TAB_INTP / TAB_NOINTP / TAB_VERB */
    int          is_range;      /* COMPU_VTAB_RANGE */
    A2lTabEntry* e;
    int          n;
    int          has_default_num;
    double       default_num;
    char         default_text[A2L_NAME];
} A2lCompuTab;

typedef struct A2lCompu {
    char         name[A2L_NAME];
    char         longid[A2L_TEXT];
    A2lCompuType type;
    char         format[32];
    char         unit[64];
    double       coeffs[6];     /* RAT_FUNC a..f；LINEAR 用 [0]=a [1]=b */
    char         tab_ref[A2L_NAME];
    char         formula[A2L_TEXT];
    char         formula_inv[A2L_TEXT];
    int          tab;           /* 解析后索引，-1 = 无 */
} A2lCompu;

typedef struct A2lMeasurement {
    char     name[A2L_NAME];
    char     longid[A2L_TEXT];
    char     datatype[16];
    char     conversion[A2L_NAME];
    double   lower, upper;
    uint32_t address;
    uint8_t  addr_ext;
    int      has_address;
    uint32_t bit_mask;          /* 0 = 无 */
    int      array_size;        /* ARRAY_SIZE / MATRIX_DIM 乘积，默认 1 */
    char     unit[64];          /* PHYS_UNIT */
} A2lMeasurement;

typedef enum { A2L_AX_UNKNOWN = 0, A2L_AX_STD, A2L_AX_FIX, A2L_AX_COM, A2L_AX_RES, A2L_AX_CURVE } A2lAxisAttr;

typedef struct A2lAxisDescr {
    A2lAxisAttr attr;
    char        input[A2L_NAME];
    char        conversion[A2L_NAME];
    int         max_points;
    double      lower, upper;
    char        axis_pts_ref[A2L_NAME];   /* COM_AXIS / RES_AXIS */
    char        curve_axis_ref[A2L_NAME]; /* CURVE_AXIS */
    int         fix_kind;                 /* 0 无，1 FIX_AXIS_PAR，2 FIX_AXIS_PAR_DIST，3 FIX_AXIS_PAR_LIST */
    double      fix_offset, fix_shift_or_dist;
    int         fix_count;
    double*     fix_list;
    int         axis_pts;                 /* 解析后 AXIS_PTS 索引，-1 */
} A2lAxisDescr;

typedef struct A2lCharacteristic {
    char         name[A2L_NAME];
    char         longid[A2L_TEXT];
    char         type[16];                /* VALUE / CURVE / MAP / VAL_BLK / ASCII / CUBOID ... */
    uint32_t     address;
    uint8_t      addr_ext;
    char         deposit[A2L_NAME];       /* RECORD_LAYOUT 名 */
    double       max_diff;
    char         conversion[A2L_NAME];
    double       lower, upper;
    uint32_t     bit_mask;
    int          number;                  /* VAL_BLK/ASCII 的 NUMBER 或 MATRIX_DIM 乘积 */
    A2lAxisDescr axis[3];
    int          naxis;
    int          read_only;
} A2lCharacteristic;

typedef struct A2lAxisPts {
    char     name[A2L_NAME];
    char     longid[A2L_TEXT];
    uint32_t address;
    char     input[A2L_NAME];
    char     deposit[A2L_NAME];
    double   max_diff;
    char     conversion[A2L_NAME];
    int      max_points;
    double   lower, upper;
} A2lAxisPts;

typedef struct A2lRecordLayout {
    char name[A2L_NAME];
    char fnc_type[16];        /* FNC_VALUES 数据类型 */
    char fnc_index_mode[16];  /* ROW_DIR / COLUMN_DIR / ... */
    char axis_x_type[16], axis_y_type[16];
    char no_axis_x_type[16], no_axis_y_type[16];
    int  fnc_pos, axis_x_pos, axis_y_pos, no_axis_x_pos, no_axis_y_pos;
} A2lRecordLayout;

typedef struct A2lGroup {
    char   name[A2L_NAME];
    char   longid[A2L_TEXT];
    int    is_function;   /* 来自 FUNCTION 块 */
    int    root;
    char** chars;  int nchars;   /* REF_CHARACTERISTIC / DEF_CHARACTERISTIC */
    char** meas;   int nmeas;    /* REF_MEASUREMENT / LOC_/IN_/OUT_MEASUREMENT */
    char** subs;   int nsubs;    /* SUB_GROUP / SUB_FUNCTION */
} A2lGroup;

typedef struct A2lMemSegment {
    char     name[A2L_NAME];
    char     longid[A2L_TEXT];
    char     prg_type[32];    /* CODE / DATA / OFFLINE_DATA / VARIABLES / SERAM / CALIBRATION_VARIABLES … */
    char     mem_type[16];    /* RAM / EEPROM / FLASH / ROM / EPROM / REGISTER … */
    char     attribute[16];   /* INTERN / EXTERN */
    uint32_t address, size;
    int      xcp_segment;     /* IF_DATA XCP SEGMENT 段号，-1 = 无 */
    int      xcp_pages;
    uint8_t  xcp_addr_ext;
} A2lMemSegment;

typedef struct A2lXcpProto {
    int      present;
    uint16_t version;
    uint16_t t[8];            /* t[1]..t[7] ms */
    int      max_cto, max_dto;
    int      byte_order_msb_first;   /* 1 = Motorola */
    int      addr_gran;              /* 1/2/4 */
    char     seed_key_dll[A2L_TEXT];
    char**   optional_cmd; int noptional_cmd;
    int      master_block, slave_block, interleaved;
} A2lXcpProto;

typedef struct A2lXcpEvent {
    char     name[A2L_NAME];
    char     short_name[A2L_NAME];
    int      channel;
    char     direction[16];   /* DAQ / STIM / DAQ_STIM */
    int      max_daq_list;
    int      time_cycle, time_unit;
    int      priority;
    double   cycle_s;         /* time_cycle × unit，0 = 非周期 */
} A2lXcpEvent;

typedef struct A2lXcpDaq {
    int          present;
    int          dynamic;
    int          max_daq, max_event, min_daq;
    char         optimisation[48];
    char         addr_ext[32];
    char         id_field[64];
    char         granularity[48];
    int          max_odt_entry_size;
    char         overload[48];
    int          prescaler, resume;
    int          ts_present, ts_ticks, ts_size, ts_fixed;
    char         ts_unit[16];
    double       ts_tick_s;   /* 每 tick 秒数 = ticks × unit */
    A2lXcpEvent* ev; int nev;
} A2lXcpDaq;

typedef enum { A2L_TL_TCP = 1, A2L_TL_UDP, A2L_TL_CAN, A2L_TL_USB, A2L_TL_SXI, A2L_TL_FLX } A2lTlKind;

typedef struct A2lXcpTransport {
    A2lTlKind   kind;
    char        name[32];     /* XCP_ON_TCP_IP … */
    uint16_t    version;
    int         port;
    char        host[A2L_TEXT];          /* ADDRESS / HOST_NAME */
    uint32_t    can_id_master, can_id_slave, can_id_broadcast;
    int         has_broadcast;
    uint32_t    baudrate;
    int         can_fd, max_dlc;
    A2lXcpProto proto;        /* 覆盖参数（present=0 用公共的） */
    A2lXcpDaq   daq;
} A2lXcpTransport;

typedef struct A2lXcp {
    int              present;
    A2lXcpProto      proto;
    A2lXcpDaq        daq;
    int              pag_present, pag_max_segments, pag_freeze;
    A2lXcpTransport* tl; int ntl;
} A2lXcp;

typedef struct A2lFile {
    A2lNode*           root;
    char               project[A2L_NAME];
    char               module[A2L_NAME];
    char               epk[A2L_TEXT];
    uint32_t           addr_epk;
    int                has_addr_epk;
    int                byte_order_msb_first;   /* MOD_COMMON BYTE_ORDER */
    A2lXcp             xcp;
    A2lCompu*          compu;   int ncompu;
    A2lCompuTab*       tab;     int ntab;
    A2lMeasurement*    meas;    int nmeas;
    A2lCharacteristic* chr;     int nchr;
    A2lAxisPts*        axpts;   int naxpts;
    A2lRecordLayout*   rl;      int nrl;
    A2lGroup*          grp;     int ngrp;
    A2lMemSegment*     seg;     int nseg;
    char               err[256];
    void*              priv;     /* 内部：解析期上下文 */
} A2lFile;

/* 解析文本（不支持 /include，见 README）。失败（语法错误或内存不足）返回 NULL，errbuf 写原因，不会 abort。 */
A2lFile* a2l_parse_text(const char* text, size_t len, char* errbuf, int errcap);
A2lFile* a2l_parse_file(const char* path, char* errbuf, int errcap);
void     a2l_free(A2lFile* f);

/* 查找 */
const A2lCompu*          a2l_find_compu(const A2lFile* f, const char* name);
const A2lMeasurement*    a2l_find_meas(const A2lFile* f, const char* name);
const A2lCharacteristic* a2l_find_chr(const A2lFile* f, const char* name);
const A2lAxisPts*        a2l_find_axis_pts(const A2lFile* f, const char* name);
const A2lRecordLayout*   a2l_find_record_layout(const A2lFile* f, const char* name);
const A2lGroup*          a2l_find_group(const A2lFile* f, const char* name);
/* 地址所在 MEMORY_SEGMENT（标定写入范围校验用），NULL = 不在任何段 */
const A2lMemSegment*     a2l_find_segment(const A2lFile* f, uint32_t addr, uint32_t len);

/* 单位换算。compu 为 NULL 或 "NO_COMPU_METHOD" 视为 IDENTICAL。ok 可为 NULL。 */
double      a2l_to_phys(const A2lFile* f, const A2lCompu* c, double raw, int* ok);
double      a2l_to_raw(const A2lFile* f, const A2lCompu* c, double phys, int* ok);
/* TAB_VERB 文本；非 TAB_VERB 或无匹配返回默认值/NULL */
const char* a2l_to_text(const A2lFile* f, const A2lCompu* c, double raw);
/* 按名换算 */
double      a2l_to_phys_by_name(const A2lFile* f, const char* compu, double raw, int* ok);

/* ASAP2 数据类型字节数（UBYTE..A_FLOAT64_IEEE），未知返回 0 */
int         a2l_datatype_size(const char* dt);

/* 简单 FORMULA 求值（+ - * / ^ 括号，sin cos tan exp log log10 sqrt abs，X1/X 变量）。ok 可为 NULL */
double      a2l_eval_formula(const char* expr, double x, int* ok);

/* 树：按关键字取子块（第 idx 个），NULL 未找到 */
const A2lNode* a2l_node_child(const A2lNode* n, const char* kw, int idx);

/* 测试钩子：第 n+1 次内部分配起返回失败（-1 关闭）。全局、非线程安全，仅供单测模拟 OOM。 */
void a2l_debug_fail_alloc_after(long n);

#endif
