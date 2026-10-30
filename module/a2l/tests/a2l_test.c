/* B4 A2L 解析单测：tests/sample.a2l + 错误输入 */
#include "../a2l_parse.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int g_fail, g_pass;
#define EXPECT(c, what) do { if (c) g_pass++; else { g_fail++; printf("FAIL %s (line %d)\n", what, __LINE__); } } while (0)
#define NEAR(a, b) (fabs((a) - (b)) < 1e-6)

static void test_xcp(const A2lFile* f)
{
    const A2lXcp* x = &f->xcp;
    const A2lXcpTransport* can = NULL;
    int i;
    EXPECT(x->present, "IF_DATA XCP");
    EXPECT(x->proto.present && x->proto.version == 0x0103, "PROTOCOL_LAYER 版本");
    EXPECT(x->proto.t[1] == 2000 && x->proto.t[7] == 2000, "T1/T7");
    EXPECT(x->proto.max_cto == 64 && x->proto.max_dto == 64, "MAX_CTO/DTO");
    EXPECT(!x->proto.byte_order_msb_first && x->proto.addr_gran == 1, "字节序/粒度");
    EXPECT(!strcmp(x->proto.seed_key_dll, "SeedKey_OpenScope.dll"), "Seed&Key DLL");
    EXPECT(x->proto.noptional_cmd == 4 && !strcmp(x->proto.optional_cmd[2], "SET_CAL_PAGE"), "OPTIONAL_CMD");
    EXPECT(x->proto.master_block && x->proto.slave_block, "COMMUNICATION_MODE_SUPPORTED");
    EXPECT(x->daq.present && x->daq.dynamic && x->daq.max_daq == 8 && x->daq.max_event == 2, "DAQ 动态 8/2");
    EXPECT(x->daq.max_odt_entry_size == 7 && x->daq.prescaler && !x->daq.resume, "DAQ 选项");
    EXPECT(!strcmp(x->daq.id_field, "IDENTIFICATION_FIELD_TYPE_ABSOLUTE"), "ID 字段类型");
    EXPECT(x->daq.ts_present && x->daq.ts_size == 4 && x->daq.ts_fixed && NEAR(x->daq.ts_tick_s, 1e-6), "时间戳 4B 1us");
    EXPECT(x->daq.nev == 2 && !strcmp(x->daq.ev[1].name, "10ms task") && x->daq.ev[1].channel == 1, "EVENT");
    EXPECT(NEAR(x->daq.ev[0].cycle_s, 0.001) && NEAR(x->daq.ev[1].cycle_s, 0.010), "事件周期");
    EXPECT(x->pag_present && x->pag_max_segments == 1 && x->pag_freeze, "PAG");
    EXPECT(x->ntl == 3, "3 种传输");
    EXPECT(x->tl[0].kind == A2L_TL_TCP && x->tl[0].port == 5555 && !strcmp(x->tl[0].host, "127.0.0.1"), "XCP_ON_TCP_IP");
    EXPECT(x->tl[1].kind == A2L_TL_UDP && x->tl[1].port == 5556 && !strcmp(x->tl[1].host, "localhost"), "XCP_ON_UDP_IP");
    for (i = 0; i < x->ntl; i++) if (x->tl[i].kind == A2L_TL_CAN) can = &x->tl[i];
    EXPECT(can && can->can_id_master == 0x200 && can->can_id_slave == 0x300 && can->has_broadcast &&
           can->can_id_broadcast == 0x100 && can->baudrate == 500000, "XCP_ON_CAN ID/波特率");
    EXPECT(can && can->proto.present && can->proto.max_cto == 8 && can->proto.byte_order_msb_first &&
           can->proto.t[7] == 32, "CAN 覆盖 PROTOCOL_LAYER");
    EXPECT(!x->tl[0].proto.present, "TCP 用公共参数");
}

static void test_compu(const A2lFile* f)
{
    int ok;
    const A2lCompu* c;
    EXPECT(f->ncompu == 7 && f->ntab == 3, "COMPU 数量");
    c = a2l_find_compu(f, "CM_rpm");
    EXPECT(c && c->type == A2L_CM_LINEAR && !strcmp(c->unit, "rpm"), "LINEAR");
    EXPECT(NEAR(a2l_to_phys(f, c, 4000, &ok), 900) && ok, "LINEAR raw→phys");
    EXPECT(NEAR(a2l_to_raw(f, c, 900, &ok), 4000) && ok, "LINEAR phys→raw");
    c = a2l_find_compu(f, "CM_degC");
    EXPECT(c && c->type == A2L_CM_RAT_FUNC, "RAT_FUNC");
    EXPECT(NEAR(a2l_to_phys(f, c, 650, &ok), 25) && ok, "RAT_FUNC raw→phys");
    EXPECT(NEAR(a2l_to_raw(f, c, 25, &ok), 650) && ok, "RAT_FUNC phys→raw");
    c = a2l_find_compu(f, "CM_form");
    EXPECT(c && c->type == A2L_CM_FORM && NEAR(a2l_to_phys(f, c, 512, &ok), 3.0) && ok, "FORM");
    EXPECT(NEAR(a2l_to_raw(f, c, 3.0, &ok), 512) && ok, "FORMULA_INV");
    c = a2l_find_compu(f, "CM_tab");
    EXPECT(c && c->type == A2L_CM_TAB_INTP && NEAR(a2l_to_phys(f, c, 150, &ok), 225.0), "TAB_INTP 插值");
    EXPECT(NEAR(a2l_to_phys(f, c, 999, &ok), 300.0), "TAB_INTP 越界钳位");
    EXPECT(NEAR(a2l_to_raw(f, c, 125, &ok), 50), "TAB_INTP 反算");
    c = a2l_find_compu(f, "CM_gear");
    EXPECT(c && c->type == A2L_CM_TAB_VERB && !strcmp(a2l_to_text(f, c, 2), "Second"), "TAB_VERB");
    EXPECT(!strcmp(a2l_to_text(f, c, 9), "Invalid"), "TAB_VERB 默认值");
    c = a2l_find_compu(f, "CM_state");
    EXPECT(c && !strcmp(a2l_to_text(f, c, 42), "High") && !strcmp(a2l_to_text(f, c, 200), "Out"), "VTAB_RANGE");
    EXPECT(NEAR(a2l_to_phys_by_name(f, "NO_COMPU_METHOD", 7, &ok), 7) && ok, "NO_COMPU_METHOD");
    a2l_to_phys_by_name(f, "missing", 7, &ok);
    EXPECT(!ok, "未知 COMPU 报错");
    EXPECT(NEAR(a2l_eval_formula("2^3+sqrt(16)*-1", 0, &ok), 4) && ok, "公式求值");
    a2l_eval_formula("X1 +* 2", 1, &ok);
    EXPECT(!ok, "非法公式");
}

static void test_objects(const A2lFile* f)
{
    const A2lMeasurement* m;
    const A2lCharacteristic* c;
    const A2lAxisPts* ap;
    const A2lRecordLayout* rl;
    EXPECT(!strcmp(f->project, "OpenScopeDemo") && !strcmp(f->module, "DemoEcu"), "PROJECT/MODULE");
    EXPECT(!strcmp(f->epk, "OpenScope-XCP") && f->has_addr_epk && f->addr_epk == 0x20000F00, "EPK");
    EXPECT(f->nmeas == 4, "MEASUREMENT 数量");
    m = a2l_find_meas(f, "gear");
    EXPECT(m && m->address == 0x2000000A && m->bit_mask == 0x0F && !strcmp(m->conversion, "CM_gear"), "MEASUREMENT 地址/掩码");
    m = a2l_find_meas(f, "sine");
    EXPECT(m && !strcmp(m->datatype, "FLOAT32_IEEE") && a2l_datatype_size(m->datatype) == 4 && !strcmp(m->unit, "-"), "MEASUREMENT 类型/单位");
    c = a2l_find_chr(f, "idle_rpm");
    EXPECT(c && !strcmp(c->type, "VALUE") && c->address == 0x20000100 && c->upper == 2000 && c->naxis == 0, "VALUE");
    c = a2l_find_chr(f, "ign_curve");
    EXPECT(c && !strcmp(c->type, "CURVE") && c->naxis == 1 && c->axis[0].attr == A2L_AX_STD &&
           c->axis[0].max_points == 8 && !strcmp(c->axis[0].input, "rpm") && !strcmp(c->axis[0].conversion, "CM_rpm"), "CURVE STD_AXIS");
    rl = a2l_find_record_layout(f, c ? c->deposit : "");
    EXPECT(rl && !strcmp(rl->fnc_type, "SWORD") && !strcmp(rl->axis_x_type, "UWORD") && !strcmp(rl->no_axis_x_type, "UBYTE") &&
           rl->fnc_pos == 3 && !strcmp(rl->fnc_index_mode, "COLUMN_DIR"), "RECORD_LAYOUT");
    c = a2l_find_chr(f, "fuel_map");
    EXPECT(c && !strcmp(c->type, "MAP") && c->naxis == 2, "MAP 两轴");
    EXPECT(c && c->axis[0].attr == A2L_AX_COM && !strcmp(c->axis[0].axis_pts_ref, "ax_rpm") && c->axis[0].axis_pts == 0, "COM_AXIS → AXIS_PTS");
    EXPECT(c && c->axis[1].attr == A2L_AX_FIX && c->axis[1].fix_kind == 2 && c->axis[1].fix_count == 4 &&
           NEAR(c->axis[1].fix_shift_or_dist, 100), "FIX_AXIS_PAR_DIST");
    c = a2l_find_chr(f, "lut");
    EXPECT(c && c->axis[0].fix_kind == 3 && c->axis[0].fix_count == 3 && NEAR(c->axis[0].fix_list[2], 50), "FIX_AXIS_PAR_LIST");
    c = a2l_find_chr(f, "name_str");
    EXPECT(c && c->number == 16 && c->read_only, "ASCII NUMBER/READ_ONLY");
    ap = a2l_find_axis_pts(f, "ax_rpm");
    EXPECT(ap && ap->address == 0x20000180 && ap->max_points == 6 && !strcmp(ap->conversion, "CM_rpm"), "AXIS_PTS");
}

static void test_groups_segments(const A2lFile* f)
{
    const A2lGroup* g = a2l_find_group(f, "Engine");
    const A2lMemSegment* s;
    EXPECT(g && g->root && g->nchars == 3 && g->nmeas == 2 && g->nsubs == 1 && !strcmp(g->subs[0], "Diag"), "GROUP Engine");
    g = a2l_find_group(f, "Diag");
    EXPECT(g && !g->root && g->nmeas == 2, "GROUP Diag");
    g = a2l_find_group(f, "Ctrl");
    EXPECT(g && g->is_function && g->nchars == 1 && g->nmeas == 2, "FUNCTION");
    EXPECT(f->nseg == 2, "MEMORY_SEGMENT 数量");
    s = a2l_find_segment(f, 0x20000110, 32);
    EXPECT(s && !strcmp(s->name, "Cal") && !strcmp(s->mem_type, "FLASH") && !strcmp(s->prg_type, "DATA") &&
           s->size == 0x400 && s->xcp_segment == 0 && s->xcp_pages == 2, "MEMORY_SEGMENT + XCP SEGMENT");
    EXPECT(a2l_find_segment(f, 0x200004F0, 0x20) == NULL, "跨段写入被拒（越界）");
    EXPECT(a2l_find_segment(f, 0x30000000, 1) == NULL, "段外地址");
    EXPECT(a2l_node_child(f->root, "PROJECT", 0) != NULL, "通用树可遍历");
}

static void test_errors(void)
{
    char err[256];
    A2lFile* f;
    const char* bad1 = "/begin PROJECT P \"\" /begin MODULE M \"\" /end PROJECT";
    const char* bad2 = "/begin PROJECT P \"unterminated /end PROJECT";
    const char* ok3 = "/begin PROJECT P \"q \"\"x\"\" // not comment\" // comment\n /begin MODULE M \"m\" /* c */ /end MODULE /end PROJECT";
    f = a2l_parse_text(bad1, strlen(bad1), err, sizeof err);
    EXPECT(!f && strstr(err, "不匹配"), "/begin /end 不匹配报错");
    f = a2l_parse_text(bad2, strlen(bad2), err, sizeof err);
    EXPECT(!f && strstr(err, "未闭合"), "未闭合字符串报错");
    f = a2l_parse_text(ok3, strlen(ok3), err, sizeof err);
    EXPECT(f && !strcmp(f->module, "M") && f->nmeas == 0 && !f->xcp.present, "注释/转义引号/空模块");
    a2l_free(f);
    f = a2l_parse_text("ASAP2_VERSION 1 71", 18, err, sizeof err);
    EXPECT(!f && strstr(err, "MODULE"), "缺 MODULE 报错");
}

int main(int argc, char** argv)
{
    char err[256];
    A2lFile* f = a2l_parse_file(argc > 1 ? argv[1] : "tests/sample.a2l", err, sizeof err);
    if (!f) { printf("FAIL parse: %s\n", err); return 1; }
    test_xcp(f);
    test_compu(f);
    test_objects(f);
    test_groups_segments(f);
    a2l_free(f);
    test_errors();
    printf("a2l_test: %d pass, %d fail\n", g_pass, g_fail);
    return g_fail ? 1 : 0;
}
