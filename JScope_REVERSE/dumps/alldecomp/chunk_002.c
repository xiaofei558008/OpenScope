
/* ================= FUN_140016840 @ 140016840  size=89  callers=2 callees=1 ================= */
/* CALLERS: FUN_140006fb0, FUN_14000ded0 */
/* CALLEES: FUN_14001bb80 */

undefined8 FUN_140016840(longlong param_1,undefined2 *param_2,undefined8 param_3)

{
  undefined8 uVar1;
  
  if (param_1 == 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = *(undefined8 *)(param_1 + 0x48);
    if (param_2 != (undefined2 *)0x0) {
      *param_2 = *(undefined2 *)(*(longlong *)(param_1 + 0x38) + 8);
      return 0;
    }
  }
  FUN_14001bb80(uVar1,param_3,0x1c,0,"dwarf_tag",0x112);
  return 1;
}


/* ================= FUN_1400168a0 @ 1400168a0  size=1713  callers=1 callees=0 ================= */
/* CALLERS: FUN_14001ba90 */

undefined8 FUN_1400168a0(uint param_1,undefined8 *param_2)

{
  if (param_2 != (undefined8 *)0x0) {
    if (param_1 < 0x2001) {
      if (param_1 == 0x2000) {
        *param_2 = "DW_AT_lo_user";
        return 0;
      }
      switch(param_1) {
      case 1:
        *param_2 = "DW_AT_sibling";
        return 0;
      case 2:
        *param_2 = "DW_AT_location";
        return 0;
      case 3:
        *param_2 = "DW_AT_name";
        return 0;
      case 9:
        *param_2 = "DW_AT_ordering";
        return 0;
      case 10:
        *param_2 = "DW_AT_subscr_data";
        return 0;
      case 0xb:
        *param_2 = "DW_AT_byte_size";
        return 0;
      case 0xc:
        *param_2 = "DW_AT_bit_offset";
        return 0;
      case 0xd:
        *param_2 = "DW_AT_bit_size";
        return 0;
      case 0xf:
        *param_2 = "DW_AT_element_list";
        return 0;
      case 0x10:
        *param_2 = "DW_AT_stmt_list";
        return 0;
      case 0x11:
        *param_2 = "DW_AT_low_pc";
        return 0;
      case 0x12:
        *param_2 = "DW_AT_high_pc";
        return 0;
      case 0x13:
        *param_2 = "DW_AT_language";
        return 0;
      case 0x14:
        *param_2 = "DW_AT_member";
        return 0;
      case 0x15:
        *param_2 = "DW_AT_discr";
        return 0;
      case 0x16:
        *param_2 = "DW_AT_discr_value";
        return 0;
      case 0x17:
        *param_2 = "DW_AT_visibility";
        return 0;
      case 0x18:
        *param_2 = "DW_AT_import";
        return 0;
      case 0x19:
        *param_2 = "DW_AT_string_length";
        return 0;
      case 0x1a:
        *param_2 = "DW_AT_common_reference";
        return 0;
      case 0x1b:
        *param_2 = "DW_AT_comp_dir";
        return 0;
      case 0x1c:
        *param_2 = "DW_AT_const_value";
        return 0;
      case 0x1d:
        *param_2 = "DW_AT_containing_type";
        return 0;
      case 0x1e:
        *param_2 = "DW_AT_default_value";
        return 0;
      case 0x20:
        *param_2 = "DW_AT_inline";
        return 0;
      case 0x21:
        *param_2 = "DW_AT_is_optional";
        return 0;
      case 0x22:
        *param_2 = "DW_AT_lower_bound";
        return 0;
      case 0x25:
        *param_2 = "DW_AT_producer";
        return 0;
      case 0x27:
        *param_2 = "DW_AT_prototyped";
        return 0;
      case 0x2a:
        *param_2 = "DW_AT_return_addr";
        return 0;
      case 0x2c:
        *param_2 = "DW_AT_start_scope";
        return 0;
      case 0x2e:
        *param_2 = "DW_AT_bit_stride";
        return 0;
      case 0x2f:
        *param_2 = "DW_AT_upper_bound";
        return 0;
      case 0x31:
        *param_2 = "DW_AT_abstract_origin";
        return 0;
      case 0x32:
        *param_2 = "DW_AT_accessibility";
        return 0;
      case 0x33:
        *param_2 = "DW_AT_address_class";
        return 0;
      case 0x34:
        *param_2 = "DW_AT_artificial";
        return 0;
      case 0x35:
        *param_2 = "DW_AT_base_types";
        return 0;
      case 0x36:
        *param_2 = "DW_AT_calling_convention";
        return 0;
      case 0x37:
        *param_2 = "DW_AT_count";
        return 0;
      case 0x38:
        *param_2 = "DW_AT_data_member_location";
        return 0;
      case 0x39:
        *param_2 = "DW_AT_decl_column";
        return 0;
      case 0x3a:
        *param_2 = "DW_AT_decl_file";
        return 0;
      case 0x3b:
        *param_2 = "DW_AT_decl_line";
        return 0;
      case 0x3c:
        *param_2 = "DW_AT_declaration";
        return 0;
      case 0x3d:
        *param_2 = "DW_AT_discr_list";
        return 0;
      case 0x3e:
        *param_2 = "DW_AT_encoding";
        return 0;
      case 0x3f:
        *param_2 = "DW_AT_external";
        return 0;
      case 0x40:
        *param_2 = "DW_AT_frame_base";
        return 0;
      case 0x41:
        *param_2 = "DW_AT_friend";
        return 0;
      case 0x42:
        *param_2 = "DW_AT_identifier_case";
        return 0;
      case 0x43:
        *param_2 = "DW_AT_macro_info";
        return 0;
      case 0x44:
        *param_2 = "DW_AT_namelist_item";
        return 0;
      case 0x45:
        *param_2 = "DW_AT_priority";
        return 0;
      case 0x46:
        *param_2 = "DW_AT_segment";
        return 0;
      case 0x47:
        *param_2 = "DW_AT_specification";
        return 0;
      case 0x48:
        *param_2 = "DW_AT_static_link";
        return 0;
      case 0x49:
        *param_2 = "DW_AT_type";
        return 0;
      case 0x4a:
        *param_2 = "DW_AT_use_location";
        return 0;
      case 0x4b:
        *param_2 = "DW_AT_variable_parameter";
        return 0;
      case 0x4c:
        *param_2 = "DW_AT_virtuality";
        return 0;
      case 0x4d:
        *param_2 = "DW_AT_vtable_elem_location";
        return 0;
      case 0x4e:
        *param_2 = "DW_AT_allocated";
        return 0;
      case 0x4f:
        *param_2 = "DW_AT_associated";
        return 0;
      case 0x50:
        *param_2 = "DW_AT_data_location";
        return 0;
      case 0x51:
        *param_2 = "DW_AT_byte_stride";
        return 0;
      case 0x52:
        *param_2 = "DW_AT_entry_pc";
        return 0;
      case 0x53:
        *param_2 = "DW_AT_use_UTF8";
        return 0;
      case 0x54:
        *param_2 = "DW_AT_extension";
        return 0;
      case 0x55:
        *param_2 = "DW_AT_ranges";
        return 0;
      case 0x56:
        *param_2 = "DW_AT_trampoline";
        return 0;
      case 0x57:
        *param_2 = "DW_AT_call_column";
        return 0;
      case 0x58:
        *param_2 = "DW_AT_call_file";
        return 0;
      case 0x59:
        *param_2 = "DW_AT_call_line";
        return 0;
      case 0x5a:
        *param_2 = "DW_AT_description";
        return 0;
      case 0x5b:
        *param_2 = "DW_AT_binary_scale";
        return 0;
      case 0x5c:
        *param_2 = "DW_AT_decimal_scale";
        return 0;
      case 0x5d:
        *param_2 = "DW_AT_small";
        return 0;
      case 0x5e:
        *param_2 = "DW_AT_decimal_sign";
        return 0;
      case 0x5f:
        *param_2 = "DW_AT_digit_count";
        return 0;
      case 0x60:
        *param_2 = "DW_AT_picture_string";
        return 0;
      case 0x61:
        *param_2 = "DW_AT_mutable";
        return 0;
      case 0x62:
        *param_2 = "DW_AT_threads_scaled";
        return 0;
      case 99:
        *param_2 = "DW_AT_explicit";
        return 0;
      case 100:
        *param_2 = "DW_AT_object_pointer";
        return 0;
      case 0x65:
        *param_2 = "DW_AT_endianity";
        return 0;
      case 0x66:
        *param_2 = "DW_AT_elemental";
        return 0;
      case 0x67:
        *param_2 = "DW_AT_pure";
        return 0;
      case 0x68:
        *param_2 = "DW_AT_recursive";
        return 0;
      case 0x69:
        *param_2 = "DW_AT_signature";
        return 0;
      case 0x6a:
        *param_2 = "DW_AT_main_subprogram";
        return 0;
      case 0x6b:
        *param_2 = "DW_AT_data_bit_offset";
        return 0;
      case 0x6c:
        *param_2 = "DW_AT_const_expr";
        return 0;
      case 0x6d:
        *param_2 = "DW_AT_enum_class";
        return 0;
      case 0x6e:
        *param_2 = "DW_AT_linkage_name";
        return 0;
      case 0x6f:
        *param_2 = "DW_AT_string_length_bit_size";
        return 0;
      case 0x70:
        *param_2 = "DW_AT_string_length_byte_size";
        return 0;
      case 0x71:
        *param_2 = "DW_AT_rank";
        return 0;
      case 0x72:
        *param_2 = "DW_AT_str_offsets_base";
        return 0;
      case 0x73:
        *param_2 = "DW_AT_addr_base";
        return 0;
      case 0x74:
        *param_2 = "DW_AT_rnglists_base";
        return 0;
      case 0x76:
        *param_2 = "DW_AT_dwo_name";
        return 0;
      case 0x77:
        *param_2 = "DW_AT_reference";
        return 0;
      case 0x78:
        *param_2 = "DW_AT_rvalue_reference";
        return 0;
      case 0x79:
        *param_2 = "DW_AT_macros";
        return 0;
      case 0x7a:
        *param_2 = "DW_AT_call_all_calls";
        return 0;
      case 0x7b:
        *param_2 = "DW_AT_call_all_source_calls";
        return 0;
      case 0x7c:
        *param_2 = "DW_AT_call_all_tail_calls";
        return 0;
      case 0x7d:
        *param_2 = "DW_AT_call_return_pc";
        return 0;
      case 0x7e:
        *param_2 = "DW_AT_call_value";
        return 0;
      case 0x7f:
        *param_2 = "DW_AT_call_origin";
        return 0;
      case 0x80:
        *param_2 = "DW_AT_call_parameter";
        return 0;
      case 0x81:
        *param_2 = "DW_AT_call_pc";
        return 0;
      case 0x82:
        *param_2 = "DW_AT_call_tail_call";
        return 0;
      case 0x83:
        *param_2 = "DW_AT_call_target";
        return 0;
      case 0x84:
        *param_2 = "DW_AT_call_target_clobbered";
        return 0;
      case 0x85:
        *param_2 = "DW_AT_call_data_location";
        return 0;
      case 0x86:
        *param_2 = "DW_AT_call_data_value";
        return 0;
      case 0x87:
        *param_2 = "DW_AT_noreturn";
        return 0;
      case 0x88:
        *param_2 = "DW_AT_alignment";
        return 0;
      case 0x89:
        *param_2 = "DW_AT_export_symbols";
        return 0;
      case 0x8a:
        *param_2 = "DW_AT_deleted";
        return 0;
      case 0x8b:
        *param_2 = "DW_AT_defaulted";
        return 0;
      case 0x8c:
        *param_2 = "DW_AT_loclists_base";
        return 0;
      }
    }
    else if (param_1 == 0x3fff) {
      *param_2 = "DW_AT_hi_user";
      return 0;
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140017190 @ 140017190  size=593  callers=1 callees=0 ================= */
/* CALLERS: FUN_14001bac0 */

undefined8 FUN_140017190(undefined4 param_1,undefined8 *param_2)

{
  if (param_2 != (undefined8 *)0x0) {
    switch(param_1) {
    case 1:
      *param_2 = "DW_FORM_addr";
      return 0;
    case 3:
      *param_2 = "DW_FORM_block2";
      return 0;
    case 4:
      *param_2 = "DW_FORM_block4";
      return 0;
    case 5:
      *param_2 = "DW_FORM_data2";
      return 0;
    case 6:
      *param_2 = "DW_FORM_data4";
      return 0;
    case 7:
      *param_2 = "DW_FORM_data8";
      return 0;
    case 8:
      *param_2 = "DW_FORM_string";
      return 0;
    case 9:
      *param_2 = "DW_FORM_block";
      return 0;
    case 10:
      *param_2 = "DW_FORM_block1";
      return 0;
    case 0xb:
      *param_2 = "DW_FORM_data1";
      return 0;
    case 0xc:
      *param_2 = "DW_FORM_flag";
      return 0;
    case 0xd:
      *param_2 = "DW_FORM_sdata";
      return 0;
    case 0xe:
      *param_2 = "DW_FORM_strp";
      return 0;
    case 0xf:
    case 0x1f:
      *param_2 = "DW_FORM_udata";
      return 0;
    case 0x10:
      *param_2 = "DW_FORM_ref_addr";
      return 0;
    case 0x11:
      *param_2 = "DW_FORM_ref1";
      return 0;
    case 0x12:
      *param_2 = "DW_FORM_ref2";
      return 0;
    case 0x13:
      *param_2 = "DW_FORM_ref4";
      return 0;
    case 0x14:
      *param_2 = "DW_FORM_ref8";
      return 0;
    case 0x15:
      *param_2 = "DW_FORM_ref_udata";
      return 0;
    case 0x16:
      *param_2 = "DW_FORM_indirect";
      return 0;
    case 0x17:
      *param_2 = "DW_FORM_sec_offset";
      return 0;
    case 0x18:
      *param_2 = "DW_FORM_exprloc";
      return 0;
    case 0x19:
      *param_2 = "DW_FORM_flag_present";
      return 0;
    case 0x1a:
      *param_2 = "DW_FORM_strx";
      return 0;
    case 0x1b:
      *param_2 = "DW_FORM_addrx";
      return 0;
    case 0x1c:
      *param_2 = "DW_FORM_ref_sup4";
      return 0;
    case 0x1d:
      *param_2 = "DW_FORM_strp_sup";
      return 0;
    case 0x1e:
      *param_2 = "DW_FORM_data16";
      return 0;
    case 0x20:
      *param_2 = "DW_FORM_ref_sig8";
      return 0;
    case 0x21:
      *param_2 = "DW_FORM_implicit_const";
      return 0;
    case 0x22:
      *param_2 = "DW_FORM_loclistx";
      return 0;
    case 0x23:
      *param_2 = "DW_FORM_rnglistx";
      return 0;
    case 0x24:
      *param_2 = "DW_FORM_ref_sup8";
      return 0;
    case 0x25:
      *param_2 = "DW_FORM_strx1";
      return 0;
    case 0x26:
      *param_2 = "DW_FORM_strx2";
      return 0;
    case 0x27:
      *param_2 = "DW_FORM_strx3";
      return 0;
    case 0x28:
      *param_2 = "DW_FORM_strx4";
      return 0;
    case 0x29:
      *param_2 = "DW_FORM_addrx1";
      return 0;
    case 0x2a:
      *param_2 = "DW_FORM_addrx2";
      return 0;
    case 0x2b:
      *param_2 = "DW_FORM_addrx3";
      return 0;
    case 0x2c:
      *param_2 = "DW_FORM_addrx4";
      return 0;
    }
  }
  return 0xffffffff;
}


/* ================= FUN_1400174a0 @ 1400174a0  size=2333  callers=1 callees=0 ================= */
/* CALLERS: FUN_14001bb50 */

undefined8 FUN_1400174a0(undefined4 param_1,undefined8 *param_2)

{
  if (param_2 != (undefined8 *)0x0) {
    switch(param_1) {
    case 3:
      *param_2 = "DW_OP_addr";
      return 0;
    case 6:
      *param_2 = "DW_OP_deref";
      return 0;
    case 8:
      *param_2 = "DW_OP_const1u";
      return 0;
    case 9:
      *param_2 = "DW_OP_const1s";
      return 0;
    case 10:
      *param_2 = "DW_OP_const2u";
      return 0;
    case 0xb:
      *param_2 = "DW_OP_const2s";
      return 0;
    case 0xc:
      *param_2 = "DW_OP_const4u";
      return 0;
    case 0xd:
      *param_2 = "DW_OP_const4s";
      return 0;
    case 0xe:
      *param_2 = "DW_OP_const8u";
      return 0;
    case 0xf:
      *param_2 = "DW_OP_const8s";
      return 0;
    case 0x10:
      *param_2 = "DW_OP_constu";
      return 0;
    case 0x11:
      *param_2 = "DW_OP_consts";
      return 0;
    case 0x12:
      *param_2 = "DW_OP_dup";
      return 0;
    case 0x13:
      *param_2 = "DW_OP_drop";
      return 0;
    case 0x14:
      *param_2 = "DW_OP_over";
      return 0;
    case 0x15:
      *param_2 = "DW_OP_pick";
      return 0;
    case 0x16:
      *param_2 = "DW_OP_swap";
      return 0;
    case 0x17:
      *param_2 = "DW_OP_rot";
      return 0;
    case 0x18:
      *param_2 = "DW_OP_xderef";
      return 0;
    case 0x19:
      *param_2 = "DW_OP_abs";
      return 0;
    case 0x1a:
      *param_2 = "DW_OP_and";
      return 0;
    case 0x1b:
      *param_2 = "DW_OP_div";
      return 0;
    case 0x1c:
      *param_2 = "DW_OP_minus";
      return 0;
    case 0x1d:
      *param_2 = "DW_OP_mod";
      return 0;
    case 0x1e:
      *param_2 = "DW_OP_mul";
      return 0;
    case 0x1f:
      *param_2 = "DW_OP_neg";
      return 0;
    case 0x20:
      *param_2 = "DW_OP_not";
      return 0;
    case 0x21:
      *param_2 = "DW_OP_or";
      return 0;
    case 0x22:
      *param_2 = "DW_OP_plus";
      return 0;
    case 0x23:
      *param_2 = "DW_OP_plus_uconst";
      return 0;
    case 0x24:
      *param_2 = "DW_OP_shl";
      return 0;
    case 0x25:
      *param_2 = "DW_OP_shr";
      return 0;
    case 0x26:
      *param_2 = "DW_OP_shra";
      return 0;
    case 0x27:
      *param_2 = "DW_OP_xor";
      return 0;
    case 0x28:
      *param_2 = "DW_OP_bra";
      return 0;
    case 0x29:
      *param_2 = "DW_OP_eq";
      return 0;
    case 0x2a:
      *param_2 = "DW_OP_ge";
      return 0;
    case 0x2b:
      *param_2 = "DW_OP_gt";
      return 0;
    case 0x2c:
      *param_2 = "DW_OP_le";
      return 0;
    case 0x2d:
      *param_2 = "DW_OP_lt";
      return 0;
    case 0x2e:
      *param_2 = "DW_OP_ne";
      return 0;
    case 0x2f:
      *param_2 = "DW_OP_skip";
      return 0;
    case 0x30:
      *param_2 = "DW_OP_lit0";
      return 0;
    case 0x31:
      *param_2 = "DW_OP_lit1";
      return 0;
    case 0x32:
      *param_2 = "DW_OP_lit2";
      return 0;
    case 0x33:
      *param_2 = "DW_OP_lit3";
      return 0;
    case 0x34:
      *param_2 = "DW_OP_lit4";
      return 0;
    case 0x35:
      *param_2 = "DW_OP_lit5";
      return 0;
    case 0x36:
      *param_2 = "DW_OP_lit6";
      return 0;
    case 0x37:
      *param_2 = "DW_OP_lit7";
      return 0;
    case 0x38:
      *param_2 = "DW_OP_lit8";
      return 0;
    case 0x39:
      *param_2 = "DW_OP_lit9";
      return 0;
    case 0x3a:
      *param_2 = "DW_OP_lit10";
      return 0;
    case 0x3b:
      *param_2 = "DW_OP_lit11";
      return 0;
    case 0x3c:
      *param_2 = "DW_OP_lit12";
      return 0;
    case 0x3d:
      *param_2 = "DW_OP_lit13";
      return 0;
    case 0x3e:
      *param_2 = "DW_OP_lit14";
      return 0;
    case 0x3f:
      *param_2 = "DW_OP_lit15";
      return 0;
    case 0x40:
      *param_2 = "DW_OP_lit16";
      return 0;
    case 0x41:
      *param_2 = "DW_OP_lit17";
      return 0;
    case 0x42:
      *param_2 = "DW_OP_lit18";
      return 0;
    case 0x43:
      *param_2 = "DW_OP_lit19";
      return 0;
    case 0x44:
      *param_2 = "DW_OP_lit20";
      return 0;
    case 0x45:
      *param_2 = "DW_OP_lit21";
      return 0;
    case 0x46:
      *param_2 = "DW_OP_lit22";
      return 0;
    case 0x47:
      *param_2 = "DW_OP_lit23";
      return 0;
    case 0x48:
      *param_2 = "DW_OP_lit24";
      return 0;
    case 0x49:
      *param_2 = "DW_OP_lit25";
      return 0;
    case 0x4a:
      *param_2 = "DW_OP_lit26";
      return 0;
    case 0x4b:
      *param_2 = "DW_OP_lit27";
      return 0;
    case 0x4c:
      *param_2 = "DW_OP_lit28";
      return 0;
    case 0x4d:
      *param_2 = "DW_OP_lit29";
      return 0;
    case 0x4e:
      *param_2 = "DW_OP_lit30";
      return 0;
    case 0x4f:
      *param_2 = "DW_OP_lit31";
      return 0;
    case 0x50:
      *param_2 = "DW_OP_reg0";
      return 0;
    case 0x51:
      *param_2 = "DW_OP_reg1";
      return 0;
    case 0x52:
      *param_2 = "DW_OP_reg2";
      return 0;
    case 0x53:
      *param_2 = "DW_OP_reg3";
      return 0;
    case 0x54:
      *param_2 = "DW_OP_reg4";
      return 0;
    case 0x55:
      *param_2 = "DW_OP_reg5";
      return 0;
    case 0x56:
      *param_2 = "DW_OP_reg6";
      return 0;
    case 0x57:
      *param_2 = "DW_OP_reg7";
      return 0;
    case 0x58:
      *param_2 = "DW_OP_reg8";
      return 0;
    case 0x59:
      *param_2 = "DW_OP_reg9";
      return 0;
    case 0x5a:
      *param_2 = "DW_OP_reg10";
      return 0;
    case 0x5b:
      *param_2 = "DW_OP_reg11";
      return 0;
    case 0x5c:
      *param_2 = "DW_OP_reg12";
      return 0;
    case 0x5d:
      *param_2 = "DW_OP_reg13";
      return 0;
    case 0x5e:
      *param_2 = "DW_OP_reg14";
      return 0;
    case 0x5f:
      *param_2 = "DW_OP_reg15";
      return 0;
    case 0x60:
      *param_2 = "DW_OP_reg16";
      return 0;
    case 0x61:
      *param_2 = "DW_OP_reg17";
      return 0;
    case 0x62:
      *param_2 = "DW_OP_reg18";
      return 0;
    case 99:
      *param_2 = "DW_OP_reg19";
      return 0;
    case 100:
      *param_2 = "DW_OP_reg20";
      return 0;
    case 0x65:
      *param_2 = "DW_OP_reg21";
      return 0;
    case 0x66:
      *param_2 = "DW_OP_reg22";
      return 0;
    case 0x67:
      *param_2 = "DW_OP_reg23";
      return 0;
    case 0x68:
      *param_2 = "DW_OP_reg24";
      return 0;
    case 0x69:
      *param_2 = "DW_OP_reg25";
      return 0;
    case 0x6a:
      *param_2 = "DW_OP_reg26";
      return 0;
    case 0x6b:
      *param_2 = "DW_OP_reg27";
      return 0;
    case 0x6c:
      *param_2 = "DW_OP_reg28";
      return 0;
    case 0x6d:
      *param_2 = "DW_OP_reg29";
      return 0;
    case 0x6e:
      *param_2 = "DW_OP_reg30";
      return 0;
    case 0x6f:
      *param_2 = "DW_OP_reg31";
      return 0;
    case 0x70:
      *param_2 = "DW_OP_breg0";
      return 0;
    case 0x71:
      *param_2 = "DW_OP_breg1";
      return 0;
    case 0x72:
      *param_2 = "DW_OP_breg2";
      return 0;
    case 0x73:
      *param_2 = "DW_OP_breg3";
      return 0;
    case 0x74:
      *param_2 = "DW_OP_breg4";
      return 0;
    case 0x75:
      *param_2 = "DW_OP_breg5";
      return 0;
    case 0x76:
      *param_2 = "DW_OP_breg6";
      return 0;
    case 0x77:
      *param_2 = "DW_OP_breg7";
      return 0;
    case 0x78:
      *param_2 = "DW_OP_breg8";
      return 0;
    case 0x79:
      *param_2 = "DW_OP_breg9";
      return 0;
    case 0x7a:
      *param_2 = "DW_OP_breg10";
      return 0;
    case 0x7b:
      *param_2 = "DW_OP_breg11";
      return 0;
    case 0x7c:
      *param_2 = "DW_OP_breg12";
      return 0;
    case 0x7d:
      *param_2 = "DW_OP_breg13";
      return 0;
    case 0x7e:
      *param_2 = "DW_OP_breg14";
      return 0;
    case 0x7f:
      *param_2 = "DW_OP_breg15";
      return 0;
    case 0x80:
      *param_2 = "DW_OP_breg16";
      return 0;
    case 0x81:
      *param_2 = "DW_OP_breg17";
      return 0;
    case 0x82:
      *param_2 = "DW_OP_breg18";
      return 0;
    case 0x83:
      *param_2 = "DW_OP_breg19";
      return 0;
    case 0x84:
      *param_2 = "DW_OP_breg20";
      return 0;
    case 0x85:
      *param_2 = "DW_OP_breg21";
      return 0;
    case 0x86:
      *param_2 = "DW_OP_breg22";
      return 0;
    case 0x87:
      *param_2 = "DW_OP_breg23";
      return 0;
    case 0x88:
      *param_2 = "DW_OP_breg24";
      return 0;
    case 0x89:
      *param_2 = "DW_OP_breg25";
      return 0;
    case 0x8a:
      *param_2 = "DW_OP_breg26";
      return 0;
    case 0x8b:
      *param_2 = "DW_OP_breg27";
      return 0;
    case 0x8c:
      *param_2 = "DW_OP_breg28";
      return 0;
    case 0x8d:
      *param_2 = "DW_OP_breg29";
      return 0;
    case 0x8e:
      *param_2 = "DW_OP_breg30";
      return 0;
    case 0x8f:
      *param_2 = "DW_OP_breg31";
      return 0;
    case 0x90:
      *param_2 = "DW_OP_regx";
      return 0;
    case 0x91:
      *param_2 = "DW_OP_fbreg";
      return 0;
    case 0x92:
      *param_2 = "DW_OP_bregx";
      return 0;
    case 0x93:
      *param_2 = "DW_OP_piece";
      return 0;
    case 0x94:
      *param_2 = "DW_OP_deref_size";
      return 0;
    case 0x95:
      *param_2 = "DW_OP_xderef_size";
      return 0;
    case 0x96:
      *param_2 = "DW_OP_nop";
      return 0;
    case 0x97:
      *param_2 = "DW_OP_push_object_address";
      return 0;
    case 0x98:
      *param_2 = "DW_OP_call2";
      return 0;
    case 0x99:
      *param_2 = "DW_OP_call4";
      return 0;
    case 0x9a:
      *param_2 = "DW_OP_call_ref";
      return 0;
    case 0x9b:
      *param_2 = "DW_OP_form_tls_address";
      return 0;
    case 0x9c:
      *param_2 = "DW_OP_call_frame_cfa";
      return 0;
    case 0x9d:
      *param_2 = "DW_OP_bit_piece";
      return 0;
    case 0x9e:
      *param_2 = "DW_OP_implicit_value";
      return 0;
    case 0x9f:
      *param_2 = "DW_OP_stack_value";
      return 0;
    case 0xa0:
      *param_2 = "DW_OP_implicit_pointer";
      return 0;
    case 0xa1:
      *param_2 = "DW_OP_addrx";
      return 0;
    case 0xa2:
      *param_2 = "DW_OP_constx";
      return 0;
    case 0xa3:
      *param_2 = "DW_OP_entry_value";
      return 0;
    case 0xa4:
      *param_2 = "DW_OP_const_type";
      return 0;
    case 0xa5:
      *param_2 = "DW_OP_GNU_regval_type";
      return 0;
    case 0xa6:
      *param_2 = "DW_OP_deref_type";
      return 0;
    case 0xa7:
      *param_2 = "DW_OP_xderef_type";
      return 0;
    case 0xa8:
      *param_2 = "DW_OP_convert";
      return 0;
    case 0xa9:
      *param_2 = "DW_OP_reinterpret";
      return 0;
    case 0xe0:
      *param_2 = "DW_OP_GNU_push_tls_address";
      return 0;
    case 0xf2:
      *param_2 = "DW_OP_GNU_implicit_pointer";
      return 0;
    case 0xf3:
      *param_2 = "DW_OP_GNU_entry_value";
      return 0;
    case 0xf4:
      *param_2 = "DW_OP_GNU_const_type";
      return 0;
    case 0xf5:
      *param_2 = "DW_OP_regval_type";
      return 0;
    case 0xf6:
      *param_2 = "DW_OP_GNU_deref_type";
      return 0;
    case 0xf7:
      *param_2 = "DW_OP_GNU_convert";
      return 0;
    case 0xf9:
      *param_2 = "DW_OP_GNU_reinterpret";
      return 0;
    case 0xfa:
      *param_2 = "DW_OP_GNU_parameter_ref";
      return 0;
    case 0xfb:
      *param_2 = "DW_OP_GNU_addrx";
      return 0;
    case 0xfc:
      *param_2 = "DW_OP_GNU_constx";
      return 0;
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140018180 @ 140018180  size=49  callers=1 callees=4 ================= */
/* CALLERS: FUN_140003a70 */
/* CALLEES: FUN_140019c50, FUN_14001b920, FUN_14001f420, free */

undefined8 FUN_140018180(void *param_1)

{
  if (param_1 != (void *)0x0) {
    FUN_14001f420();
    FUN_14001b920(param_1);
    free(param_1);
    FUN_140019c50();
  }
  return 0;
}


/* ================= FUN_1400181c0 @ 1400181c0  size=173  callers=2 callees=1 ================= */
/* CALLERS: FUN_140004df0, FUN_1400055d0 */
/* CALLEES: FUN_14001bb80 */

undefined8 FUN_1400181c0(longlong *param_1,longlong *param_2,undefined8 param_3)

{
  int iVar1;
  undefined8 uVar2;
  
  uVar2 = 0;
  if ((param_1 == (longlong *)0x0) ||
     (uVar2 = *(undefined8 *)(*param_1 + 0x48), param_2 == (longlong *)0x0)) {
    FUN_14001bb80(uVar2,param_3,0x1c,0,"dwarf_formaddr",0xbb);
    return 1;
  }
  iVar1 = (int)param_1[3];
  if (iVar1 == 1) {
    *param_2 = param_1[4];
    return 0;
  }
  if ((iVar1 != 0x1b) && (3 < iVar1 - 0x29U)) {
    FUN_14001bb80(uVar2,param_3,0x1c,0,"dwarf_formaddr",0xc9);
    return 1;
  }
  *param_2 = param_1[5];
  return 0;
}


/* ================= FUN_140018270 @ 140018270  size=117  callers=1 callees=1 ================= */
/* CALLERS: FUN_140006210 */
/* CALLEES: FUN_14001bb80 */

undefined8 FUN_140018270(longlong *param_1,longlong *param_2,longlong *param_3,undefined8 param_4)

{
  undefined8 uVar1;
  undefined4 uVar2;
  
  if (param_1 == (longlong *)0x0) {
    uVar1 = 0;
  }
  else {
    uVar1 = *(undefined8 *)(*param_1 + 0x48);
    if ((param_2 != (longlong *)0x0) && (param_3 != (longlong *)0x0)) {
      if ((int)param_1[3] == 0x18) {
        *param_2 = param_1[4];
        *param_3 = param_1[5];
        return 0;
      }
      uVar2 = 0x185;
      goto LAB_1400182bd;
    }
  }
  uVar2 = 0x180;
LAB_1400182bd:
  FUN_14001bb80(uVar1,param_4,0x1c,0,"dwarf_formexprloc",uVar2);
  return 1;
}


/* ================= FUN_1400182f0 @ 1400182f0  size=153  callers=1 callees=1 ================= */
/* CALLERS: FUN_140004df0 */
/* CALLEES: FUN_14001bb80 */

undefined8 FUN_1400182f0(longlong *param_1,uint *param_2,undefined8 param_3)

{
  undefined8 uVar1;
  
  uVar1 = 0;
  if ((param_1 == (longlong *)0x0) ||
     (uVar1 = *(undefined8 *)(*param_1 + 0x48), param_2 == (uint *)0x0)) {
    FUN_14001bb80(uVar1,param_3,0x1c,0,"dwarf_formflag",0xd7);
    return 1;
  }
  if (((int)param_1[3] != 0xc) && ((int)param_1[3] != 0x19)) {
    FUN_14001bb80(uVar1,param_3,0x1c,0,"dwarf_formflag",0xe1);
    return 1;
  }
  *param_2 = (uint)(param_1[4] != 0);
  return 0;
}


/* ================= FUN_140018390 @ 140018390  size=252  callers=1 callees=1 ================= */
/* CALLERS: FUN_140004df0 */
/* CALLEES: FUN_14001bb80 */

undefined8 FUN_140018390(longlong *param_1,longlong *param_2,undefined8 param_3)

{
  undefined8 uVar1;
  
  if (param_1 == (longlong *)0x0) {
    uVar1 = 0;
  }
  else {
    uVar1 = *(undefined8 *)(*param_1 + 0x48);
    if (param_2 != (longlong *)0x0) {
      switch((int)param_1[3]) {
      case 5:
        *param_2 = (longlong)(short)param_1[4];
        return 0;
      case 6:
      case 0x17:
        *param_2 = (longlong)(int)param_1[4];
        return 0;
      case 7:
      case 0xd:
      case 0x1e:
        *param_2 = param_1[4];
        return 0;
      default:
        FUN_14001bb80(uVar1,param_3,0x1c,0,"dwarf_formsdata",0x140);
        return 1;
      case 0xb:
        *param_2 = (longlong)(char)param_1[4];
        return 0;
      }
    }
  }
  FUN_14001bb80(uVar1,param_3,0x1c,0,"dwarf_formsdata",0x124);
  return 1;
}


/* ================= FUN_1400184c0 @ 1400184c0  size=117  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400080f0 */
/* CALLEES: FUN_14001bb80, memcpy */

undefined8 FUN_1400184c0(longlong *param_1,void *param_2,undefined8 param_3)

{
  undefined8 uVar1;
  undefined4 uVar2;
  
  if (param_1 == (longlong *)0x0) {
    uVar1 = 0;
  }
  else {
    uVar1 = *(undefined8 *)(*param_1 + 0x48);
    if (param_2 != (void *)0x0) {
      if ((int)param_1[3] == 0x20) {
        memcpy(param_2,(void *)param_1[5],param_1[4]);
        return 0;
      }
      uVar2 = 0x16f;
      goto LAB_14001850d;
    }
  }
  uVar2 = 0x16a;
LAB_14001850d:
  FUN_14001bb80(uVar1,param_3,0x1c,0,"dwarf_formsig8",uVar2);
  return 1;
}


/* ================= FUN_140018540 @ 140018540  size=214  callers=2 callees=1 ================= */
/* CALLERS: FUN_140004df0, FUN_1400055d0 */
/* CALLEES: FUN_14001bb80 */

undefined8 FUN_140018540(longlong *param_1,longlong *param_2,undefined8 param_3)

{
  undefined8 uVar1;
  
  if (param_1 == (longlong *)0x0) {
    uVar1 = 0;
  }
  else {
    uVar1 = *(undefined8 *)(*param_1 + 0x48);
    if (param_2 != (longlong *)0x0) {
      switch((int)param_1[3]) {
      case 5:
      case 6:
      case 7:
      case 0xb:
      case 0xf:
      case 0x17:
      case 0x1c:
      case 0x1e:
      case 0x24:
        *param_2 = param_1[4];
        return 0;
      default:
        FUN_14001bb80(uVar1,param_3,0x1c,0,"dwarf_formudata",0x115);
        return 1;
      case 0x1b:
      case 0x22:
      case 0x23:
      case 0x29:
      case 0x2a:
      case 0x2b:
      case 0x2c:
        *param_2 = param_1[5];
        return 0;
      }
    }
  }
  FUN_14001bb80(uVar1,param_3,0x1c,0,"dwarf_formudata",0xf0);
  return 1;
}


/* ================= FUN_140018650 @ 140018650  size=243  callers=1 callees=1 ================= */
/* CALLERS: FUN_140004df0 */
/* CALLEES: FUN_14001bb80 */

undefined8 FUN_140018650(longlong *param_1,int *param_2,undefined8 param_3)

{
  undefined8 uVar1;
  
  if (param_1 == (longlong *)0x0) {
    uVar1 = 0;
  }
  else {
    uVar1 = *(undefined8 *)(*param_1 + 0x48);
    if (param_2 != (int *)0x0) {
      switch((int)param_1[3]) {
      case 0x10:
      case 0x17:
      case 0x1c:
      case 0x24:
        *param_2 = (int)param_1[4];
        return 0;
      case 0x11:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
        *param_2 = *(int *)(*(longlong *)(*param_1 + 0x50) + 8) + (int)param_1[4];
        return 0;
      default:
        FUN_14001bb80(uVar1,param_3,0x1c,0,"dwarf_global_formref",0xad);
        return 1;
      case 0x1b:
      case 0x22:
      case 0x23:
      case 0x29:
      case 0x2a:
      case 0x2b:
      case 0x2c:
        *param_2 = (int)param_1[5];
        return 0;
      }
    }
  }
  FUN_14001bb80(uVar1,param_3,0x1c,0,"dwarf_global_formref",0x88);
  return 1;
}


/* ================= FUN_140018780 @ 140018780  size=339  callers=1 callees=3 ================= */
/* CALLERS: FUN_140004fe0 */
/* CALLEES: FUN_14001bb20, FUN_14001bb80, FUN_14001bd50 */

undefined8
FUN_140018780(longlong *param_1,ulonglong param_2,longlong *param_3,undefined8 *param_4,
             longlong *param_5,undefined8 param_6)

{
  longlong lVar1;
  ulonglong uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  int iVar5;
  undefined4 uVar6;
  
  if (param_1 == (longlong *)0x0) {
    uVar4 = 0;
  }
  else {
    uVar4 = *(undefined8 *)*param_1;
    if (((param_3 != (longlong *)0x0) && (param_4 != (undefined8 *)0x0)) &&
       (param_5 != (longlong *)0x0)) {
      lVar1 = ((undefined8 *)*param_1)[2];
      if (lVar1 != 0) {
        uVar2 = *(ulonglong *)(lVar1 + 0x38);
        iVar5 = 0;
        if (uVar2 != 0) {
          do {
            lVar1 = *param_1;
            if (((lVar1 != 0) && (*(ulonglong *)(lVar1 + 0x38) <= param_2)) &&
               (param_2 < *(ulonglong *)(lVar1 + 0x38) + *(longlong *)(lVar1 + 0x40))) {
              *param_3 = lVar1;
              *param_4 = *(undefined8 *)(lVar1 + 0x38);
              *param_5 = *(longlong *)(lVar1 + 0x40) + -1 + *(longlong *)(lVar1 + 0x38);
              return 0;
            }
            iVar5 = iVar5 + 1;
            param_1 = param_1 + 1;
          } while ((ulonglong)(longlong)iVar5 < uVar2);
        }
        uVar3 = FUN_14001bb20(param_2);
        FUN_14001bd50(uVar4,param_6,0x22,0,"dwarf_get_fde_at_pc",0xa2,uVar3,0,0);
        return 0xffffffff;
      }
      uVar6 = 0x94;
      goto LAB_140018895;
    }
  }
  uVar6 = 0x8b;
LAB_140018895:
  FUN_14001bb80(uVar4,param_6,0x1c,0,"dwarf_get_fde_at_pc",uVar6);
  return 1;
}


/* ================= FUN_1400188e0 @ 1400188e0  size=318  callers=1 callees=2 ================= */
/* CALLERS: FUN_140004fe0 */
/* CALLEES: FUN_14001bb80, FUN_14001ca80 */

undefined8
FUN_1400188e0(undefined8 *param_1,ulonglong param_2,byte *param_3,ulonglong *param_4,
             ulonglong *param_5,undefined8 *param_6,undefined8 *param_7,undefined8 *param_8,
             undefined8 param_9)

{
  int iVar1;
  undefined8 uVar2;
  byte *local_res8;
  undefined4 uVar3;
  undefined8 local_28 [2];
  
  if (param_1 == (undefined8 *)0x0) {
    uVar2 = 0;
  }
  else {
    uVar2 = *param_1;
    if (((((param_3 != (byte *)0x0) && (param_4 != (ulonglong *)0x0)) &&
         (param_5 != (ulonglong *)0x0)) &&
        ((param_6 != (undefined8 *)0x0 && (param_7 != (undefined8 *)0x0)))) &&
       (param_8 != (undefined8 *)0x0)) {
      if (((ulonglong)param_1[7] <= param_2) && (param_2 < (ulonglong)(param_1[7] + param_1[8]))) {
        iVar1 = FUN_14001ca80(param_1,param_2,&local_res8,local_28,param_9);
        if (iVar1 != 0) {
          return 1;
        }
        *param_3 = local_res8[1];
        *param_4 = (ulonglong)*local_res8;
        *param_5 = (ulonglong)*(ushort *)(local_res8 + 2);
        *param_6 = *(undefined8 *)(local_res8 + 8);
        *param_7 = *(undefined8 *)(local_res8 + 0x10);
        *param_8 = local_28[0];
        return 0;
      }
      uVar3 = 0x199;
      goto LAB_1400189dd;
    }
  }
  uVar3 = 0x194;
LAB_1400189dd:
  FUN_14001bb80(uVar2,param_9,0x1c,0,"dwarf_get_fde_info_for_cfa_reg3",uVar3);
  return 1;
}


/* ================= FUN_140018a20 @ 140018a20  size=422  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000aa30 */
/* CALLEES: FUN_14001bb80, FUN_14001bd50, FUN_14001cc70, FUN_14001e170 */

undefined8
FUN_140018a20(longlong param_1,longlong *param_2,undefined8 *param_3,undefined8 *param_4,
             undefined8 *param_5,undefined8 param_6)

{
  int iVar1;
  undefined8 uVar2;
  longlong lVar3;
  undefined4 uVar4;
  
  if ((((param_1 == 0) || (param_2 == (longlong *)0x0)) || (param_3 == (undefined8 *)0x0)) ||
     ((param_4 == (undefined8 *)0x0 || (param_5 == (undefined8 *)0x0)))) {
    FUN_14001bb80(param_1,param_6,0x1c,0,"dwarf_get_fde_list",0x22);
    return 1;
  }
  if ((*(longlong *)(param_1 + 0x228) == 0) && (iVar1 = FUN_14001cc70(param_1,param_6), iVar1 != 0))
  {
LAB_140018ab2:
    uVar2 = 1;
  }
  else {
    lVar3 = *(longlong *)(param_1 + 400);
    if (lVar3 == 0) {
      iVar1 = FUN_14001e170(param_1,param_6);
      if (iVar1 != 0) goto LAB_140018ab2;
      lVar3 = *(longlong *)(param_1 + 400);
      if (lVar3 != 0) goto LAB_140018b05;
      uVar2 = 0x1d;
      uVar4 = 0x2f;
    }
    else {
LAB_140018b05:
      if ((*(longlong *)(lVar3 + 0x20) != 0) && (*(longlong *)(lVar3 + 0x30) != 0)) {
        *param_2 = *(longlong *)(lVar3 + 0x20);
        *param_3 = *(undefined8 *)(*(longlong *)(param_1 + 400) + 0x28);
        *param_4 = *(undefined8 *)(*(longlong *)(param_1 + 400) + 0x30);
        *param_5 = *(undefined8 *)(*(longlong *)(param_1 + 400) + 0x38);
        return 0;
      }
      uVar2 = 3;
      uVar4 = 0x34;
    }
    FUN_14001bd50(param_1,param_6,uVar2,0,"dwarf_get_fde_list",uVar4,".debug_frame",0,0);
    uVar2 = 0xffffffff;
  }
  return uVar2;
}


/* ================= FUN_140018bd0 @ 140018bd0  size=278  callers=1 callees=6 ================= */
/* CALLERS: FUN_14000aaf0 */
/* CALLEES: FUN_14001b920, FUN_14001b960, FUN_14001bb80, FUN_14001ea70, FUN_14001f440, free */

undefined4
FUN_140018bd0(longlong param_1,int param_2,undefined8 param_3,undefined8 param_4,undefined8 *param_5
             ,undefined8 param_6)

{
  int iVar1;
  void *local_res8;
  undefined4 uVar2;
  
  if ((param_1 == 0) || (param_5 == (undefined8 *)0x0)) {
    uVar2 = 0x24;
  }
  else {
    if (param_2 == 1) {
      iVar1 = FUN_14001ea70(&local_res8,1,param_6);
      if (iVar1 == 0) {
        iVar1 = FUN_14001b960(local_res8,param_1,param_6);
        if (iVar1 == 0) {
          iVar1 = FUN_14001f440(local_res8,1,param_3,param_4,param_6);
          if (iVar1 == 0) {
            *param_5 = local_res8;
            return 0;
          }
          FUN_14001b920(local_res8);
          free(local_res8);
          if (iVar1 == 7) {
            return 0xffffffff;
          }
        }
        else {
          free(local_res8);
        }
      }
      return 1;
    }
    uVar2 = 0x29;
  }
  FUN_14001bb80(0,param_6,0x1c,0,"dwarf_elf_init",uVar2);
  return 1;
}


/* ================= FUN_140018cf0 @ 140018cf0  size=624  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400064c0 */
/* CALLEES: FUN_14001ad70, FUN_14001bb80, FUN_14001bd50, FUN_1400202a0, calloc, free */

undefined8 FUN_140018cf0(longlong param_1,longlong *param_2,size_t *param_3,undefined8 param_4)

{
  undefined8 uVar1;
  longlong lVar2;
  size_t _Count;
  int iVar3;
  void *pvVar4;
  longlong lVar5;
  undefined8 *puVar6;
  undefined8 uVar7;
  uint uVar8;
  ulonglong uVar9;
  ulonglong uVar10;
  
  if (param_1 == 0) {
    uVar7 = 0;
  }
  else {
    uVar7 = *(undefined8 *)(param_1 + 0x48);
    if ((param_2 != (longlong *)0x0) && (param_3 != (size_t *)0x0)) {
      pvVar4 = (void *)FUN_14001ad70(param_1,0x10);
      if (pvVar4 == (void *)0x0) {
        FUN_14001bd50(uVar7,param_4,0x20,0,"dwarf_srcfiles",0x76,"DW_AT_stmt_list",
                      *(undefined8 *)(param_1 + 0x58),0);
        return 0xffffffff;
      }
      uVar1 = *(undefined8 *)((longlong)pvVar4 + 0x20);
      free(pvVar4);
      lVar2 = *(longlong *)(param_1 + 0x50);
      if (lVar2 == 0) {
        FUN_14001bd50(uVar7,param_4,0x20,0,"dwarf_srcfiles",0x7f,"compilation unit",
                      *(undefined8 *)(param_1 + 0x58),0);
      }
      else if ((*(longlong *)(lVar2 + 0x78) != 0) ||
              (iVar3 = FUN_1400202a0(param_1,uVar1), iVar3 == 0)) {
        lVar2 = *(longlong *)(lVar2 + 0x78);
        if (lVar2 == 0) {
          FUN_14001bd50(uVar7,param_4,0x20,0,"dwarf_srcfiles",0x88,"source files",
                        *(undefined8 *)(param_1 + 0x58),0);
        }
        else {
          _Count = *(size_t *)(lVar2 + 0x48);
          *param_3 = _Count;
          if (_Count != 0) {
            lVar5 = *(longlong *)(lVar2 + 0x40);
            if (lVar5 == 0) {
              pvVar4 = calloc(_Count,8);
              *(void **)(lVar2 + 0x40) = pvVar4;
              if (pvVar4 == (void *)0x0) {
                FUN_14001bb80(uVar7,param_4,1,0,"dwarf_srcfiles",0x9a);
                return 1;
              }
              puVar6 = *(undefined8 **)(lVar2 + 0x80);
              uVar9 = 0;
              uVar10 = uVar9;
              if (0 < (longlong)*param_3) {
                do {
                  if (puVar6 == (undefined8 *)0x0) break;
                  if (puVar6[1] == 0) {
                    *(undefined8 *)(uVar10 + *(longlong *)(lVar2 + 0x40)) = *puVar6;
                  }
                  else {
                    *(undefined8 *)(uVar10 + *(longlong *)(lVar2 + 0x40)) = puVar6[1];
                  }
                  puVar6 = (undefined8 *)puVar6[5];
                  uVar8 = (int)uVar9 + 1;
                  uVar9 = (ulonglong)uVar8;
                  uVar10 = uVar10 + 8;
                } while ((longlong)(int)uVar8 < (longlong)*param_3);
              }
              lVar5 = *(longlong *)(lVar2 + 0x40);
            }
            *param_2 = lVar5;
            return 0;
          }
        }
        return 0xffffffff;
      }
      return 1;
    }
  }
  FUN_14001bb80(uVar7,param_4,0x1c,0,"dwarf_srcfiles",0x71);
  return 1;
}


/* ================= FUN_140018f60 @ 140018f60  size=573  callers=1 callees=6 ================= */
/* CALLERS: FUN_14000d6d0 */
/* CALLEES: FUN_14001ad70, FUN_14001bb80, FUN_14001bd50, FUN_1400202a0, calloc, free */

undefined4 FUN_140018f60(longlong param_1,longlong *param_2,size_t *param_3,undefined8 param_4)

{
  longlong lVar1;
  size_t _Count;
  int iVar2;
  void *pvVar3;
  longlong lVar4;
  uint uVar5;
  ulonglong uVar6;
  undefined8 uVar7;
  ulonglong uVar8;
  undefined4 uVar9;
  char *pcVar10;
  undefined8 uVar11;
  
  if (param_1 == 0) {
    uVar7 = 0;
LAB_14001916d:
    FUN_14001bb80(uVar7,param_4,0x1c,0,"dwarf_srclines",0x2b);
    return 1;
  }
  uVar7 = *(undefined8 *)(param_1 + 0x48);
  if ((param_2 == (longlong *)0x0) || (param_3 == (size_t *)0x0)) goto LAB_14001916d;
  lVar1 = *(longlong *)(param_1 + 0x50);
  if (lVar1 == 0) {
    FUN_14001bd50(uVar7,param_4,0x20,0,"dwarf_srclines",0x31,"compilation unit",
                  *(undefined8 *)(param_1 + 0x58),0);
    return 1;
  }
  if (*(longlong *)(lVar1 + 0x78) == 0) {
    pvVar3 = (void *)FUN_14001ad70();
    if (pvVar3 == (void *)0x0) {
      uVar11 = *(undefined8 *)(param_1 + 0x58);
      pcVar10 = "DW_AT_stmt_list";
      uVar9 = 0x3a;
      goto LAB_140019084;
    }
    uVar11 = *(undefined8 *)((longlong)pvVar3 + 0x20);
    free(pvVar3);
    iVar2 = FUN_1400202a0(param_1,uVar11,param_4);
    if (iVar2 != 0) {
      return 1;
    }
  }
  lVar1 = *(longlong *)(lVar1 + 0x78);
  if (lVar1 != 0) {
    _Count = *(size_t *)(lVar1 + 0x98);
    *param_3 = _Count;
    if (_Count == 0) {
      return 0xffffffff;
    }
    lVar4 = *(longlong *)(lVar1 + 0x90);
    if (lVar4 == 0) {
      pvVar3 = calloc(_Count,0x50);
      *(void **)(lVar1 + 0x90) = pvVar3;
      if (pvVar3 == (void *)0x0) {
        FUN_14001bb80(uVar7,param_4,1,0,"dwarf_srclines",0x57);
        return 1;
      }
      lVar4 = *(longlong *)(lVar1 + 0xa0);
      uVar6 = 0;
      uVar8 = uVar6;
      if (0 < (longlong)*param_3) {
        do {
          if (lVar4 == 0) break;
          uVar5 = (int)uVar6 + 1;
          uVar6 = (ulonglong)uVar5;
          *(longlong *)(uVar8 + *(longlong *)(lVar1 + 0x90)) = lVar4;
          lVar4 = *(longlong *)(lVar4 + 0x48);
          uVar8 = uVar8 + 8;
        } while ((longlong)(int)uVar5 < (longlong)*param_3);
      }
      lVar4 = *(longlong *)(lVar1 + 0x90);
    }
    *param_2 = lVar4;
    return 0;
  }
  uVar11 = *(undefined8 *)(param_1 + 0x58);
  pcVar10 = "source lines";
  uVar9 = 0x45;
LAB_140019084:
  FUN_14001bd50(uVar7,param_4,0x20,0,"dwarf_srclines",uVar9,pcVar10,uVar11,0);
  return 0xffffffff;
}


/* ================= FUN_1400191a0 @ 1400191a0  size=175  callers=2 callees=2 ================= */
/* CALLERS: FUN_140006210, FUN_14000d240 */
/* CALLEES: FUN_14001bb80, FUN_1400228f0 */

undefined8
FUN_1400191a0(longlong param_1,undefined8 param_2,longlong param_3,longlong param_4,
             undefined8 *param_5,undefined8 *param_6,undefined8 param_7)

{
  int iVar1;
  undefined8 local_res8;
  
  if ((((param_1 == 0) || (param_3 == 0)) || (param_4 == 0)) ||
     ((param_5 == (undefined8 *)0x0 || (param_6 == (undefined8 *)0x0)))) {
    FUN_14001bb80(param_1,param_7,0x1c,0,"dwarf_loclist_from_expr",0x117);
  }
  else {
    iVar1 = FUN_1400228f0(param_1,param_2,&local_res8,param_3,param_4,
                          *(undefined1 *)(param_1 + 0xf4),param_7);
    if (iVar1 == 0) {
      *param_5 = local_res8;
      *param_6 = 1;
      return 0;
    }
  }
  return 1;
}


/* ================= FUN_140019250 @ 140019250  size=628  callers=1 callees=7 ================= */
/* CALLERS: FUN_140006210 */
/* CALLEES: FUN_14001ba90, FUN_14001bac0, FUN_14001bb80, FUN_14001bd50, FUN_140022210, FUN_140023020, FUN_1400238d0 */

undefined8 FUN_140019250(longlong *param_1,longlong *param_2,longlong *param_3,undefined8 param_4)

{
  longlong lVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  longlong *local_res8;
  undefined4 uVar6;
  
  if (param_1 == (longlong *)0x0) {
    uVar5 = 0;
  }
  else {
    lVar1 = *param_1;
    uVar5 = *(undefined8 *)(lVar1 + 0x48);
    if ((param_2 != (longlong *)0x0) && (param_3 != (longlong *)0x0)) {
      switch(*(undefined4 *)((longlong)param_1 + 0x14)) {
      case 2:
      case 0x19:
      case 0x2a:
      case 0x38:
      case 0x40:
      case 0x46:
      case 0x48:
      case 0x4a:
      case 0x4d:
        goto switchD_1400192ba_caseD_2;
      default:
        uVar4 = FUN_14001ba90(*(undefined4 *)((longlong)param_1 + 0x14));
        FUN_14001bd50(uVar5,param_4,0x1c,0,"dwarf_loclist_n",0x76,uVar4,0,0);
        return 1;
      }
    }
  }
  FUN_14001bb80(uVar5,param_4,0x1c,0,"dwarf_loclist_n",0x27);
  return 1;
switchD_1400192ba_caseD_2:
  switch((int)param_1[3]) {
  case 3:
  case 4:
  case 9:
  case 10:
    if ((param_1[8] == 0) && (iVar2 = FUN_140022210(lVar1,param_1,param_4), iVar2 != 0)) {
      return 1;
    }
    *param_2 = (longlong)(param_1 + 8);
    *param_3 = 1;
    return 0;
  case 5:
  case 0xb:
    goto LAB_1400194ae;
  case 6:
  case 7:
    iVar2 = FUN_140023020(uVar5,*(undefined8 *)(lVar1 + 0x50),param_1[4],&local_res8,param_4);
    if (iVar2 != 0x23) {
LAB_140019330:
      if (iVar2 != 0) {
        return 1;
      }
      *param_2 = *local_res8;
      *param_3 = (longlong)(int)local_res8[1];
      return 0;
    }
    uVar6 = 0x39;
    break;
  default:
    uVar4 = FUN_14001bac0((int)param_1[3]);
    uVar3 = FUN_14001ba90(*(undefined4 *)((longlong)param_1 + 0x14));
    FUN_14001bd50(uVar5,param_4,6,0,"dwarf_loclist_n",0x71,uVar3,"loclist",uVar4);
    return 0xffffffff;
  case 0x17:
    if (*(ushort *)(*(longlong *)(lVar1 + 0x50) + 0x12) < 5) {
      iVar2 = FUN_140023020();
    }
    else {
      iVar2 = FUN_1400238d0(uVar5,*(longlong *)(lVar1 + 0x50),param_1[4],&local_res8,param_4);
    }
    if (iVar2 != 0x23) goto LAB_140019330;
    uVar6 = 0x49;
    break;
  case 0x22:
    iVar2 = FUN_1400238d0(uVar5,*(undefined8 *)(lVar1 + 0x50),param_1[5],&local_res8,param_4);
    if (iVar2 != 0x23) goto LAB_140019330;
    uVar6 = 0x55;
  }
  FUN_14001bb80(uVar5,param_4,5,0,"dwarf_loclist_n",uVar6);
LAB_1400194ae:
  return 0xffffffff;
}


/* ================= FUN_140019550 @ 140019550  size=185  callers=1 callees=3 ================= */
/* CALLERS: FUN_14000c6f0 */
/* CALLEES: FUN_14001bb80, FUN_140023c90, FUN_140023f20 */

undefined8
FUN_140019550(longlong param_1,undefined4 param_2,undefined8 param_3,longlong param_4,
             longlong param_5,undefined8 param_6)

{
  int iVar1;
  undefined8 uVar2;
  
  if (((param_1 == 0) || (param_4 == 0)) || (param_5 == 0)) {
    FUN_14001bb80(param_1,param_6,0x1c,0,"dwarf_get_macro_details",0x33);
    uVar2 = 1;
  }
  else if ((*(int *)(param_1 + 0x104) == 0) && (iVar1 = FUN_140023f20(param_1,param_6), iVar1 != 0))
  {
    uVar2 = 1;
  }
  else {
    uVar2 = FUN_140023c90(param_1,param_2,param_3,param_4,param_5,param_6);
  }
  return uVar2;
}


/* ================= FUN_140019610 @ 140019610  size=309  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001f580 */
/* CALLEES: free */

void FUN_140019610(longlong param_1)

{
  longlong *plVar1;
  longlong *plVar2;
  void *pvVar3;
  void *pvVar4;
  longlong lVar5;
  void *pvVar6;
  void *pvVar7;
  void *pvVar8;
  void *_Memory;
  
  plVar2 = (longlong *)(param_1 + 0x268);
  _Memory = (void *)*plVar2;
  while (_Memory != (void *)0x0) {
    pvVar3 = (void *)*plVar2;
    pvVar4 = *(void **)((longlong)_Memory + 0x30);
    if (pvVar3 == _Memory) {
      lVar5 = *(longlong *)((longlong)pvVar3 + 0x30);
      *plVar2 = lVar5;
      if (lVar5 == 0) {
        *(longlong **)(param_1 + 0x270) = plVar2;
      }
    }
    else {
      for (pvVar6 = *(void **)((longlong)pvVar3 + 0x30); pvVar6 != _Memory;
          pvVar6 = *(void **)((longlong)pvVar6 + 0x30)) {
        pvVar3 = pvVar6;
      }
      lVar5 = *(longlong *)(*(longlong *)((longlong)pvVar3 + 0x30) + 0x30);
      *(longlong *)((longlong)pvVar3 + 0x30) = lVar5;
      if (lVar5 == 0) {
        *(longlong **)(param_1 + 0x270) = (longlong *)((longlong)pvVar3 + 0x30);
      }
    }
    plVar1 = (longlong *)((longlong)_Memory + 0x20);
    pvVar3 = *(void **)((longlong)_Memory + 0x20);
    while (pvVar3 != (void *)0x0) {
      pvVar6 = (void *)*plVar1;
      pvVar7 = *(void **)((longlong)pvVar3 + 0x28);
      pvVar8 = *(void **)((longlong)pvVar6 + 0x28);
      if (pvVar6 == pvVar3) {
        *plVar1 = (longlong)pvVar8;
        if (pvVar8 == (void *)0x0) {
          *(longlong **)((longlong)_Memory + 0x28) = plVar1;
        }
      }
      else {
        for (; pvVar8 != pvVar3; pvVar8 = *(void **)((longlong)pvVar8 + 0x28)) {
          pvVar6 = pvVar8;
        }
        lVar5 = *(longlong *)(*(longlong *)((longlong)pvVar6 + 0x28) + 0x28);
        *(longlong *)((longlong)pvVar6 + 0x28) = lVar5;
        if (lVar5 == 0) {
          *(longlong **)((longlong)_Memory + 0x28) = (longlong *)((longlong)pvVar6 + 0x28);
        }
      }
      free(pvVar3);
      pvVar3 = pvVar7;
    }
    if (*(void **)((longlong)_Memory + 8) != (void *)0x0) {
      free(*(void **)((longlong)_Memory + 8));
    }
    free(_Memory);
    _Memory = pvVar4;
  }
  return;
}


/* ================= FUN_140019750 @ 140019750  size=278  callers=1 callees=3 ================= */
/* CALLERS: FUN_140019870 */
/* CALLEES: FUN_14001bb80, FUN_140024c00, FUN_140024e60 */

undefined4
FUN_140019750(undefined8 param_1,longlong param_2,int param_3,undefined8 *param_4,
             undefined8 *param_5,longlong *param_6,undefined8 param_7)

{
  int iVar1;
  longlong local_res10;
  
  if (param_2 == 0) {
    FUN_14001bb80(param_1,param_7,0x1c,0,"_dwarf_get_ranges",0x23);
    return 1;
  }
  local_res10 = 0;
  iVar1 = FUN_140024e60(param_1,(longlong)param_3,&local_res10);
  if ((iVar1 != 0x23) ||
     (iVar1 = FUN_140024c00(param_1,param_2,(longlong)param_3,&local_res10,param_7), iVar1 == 0)) {
    if (local_res10 != 0) {
      if (param_4 != (undefined8 *)0x0) {
        *param_4 = *(undefined8 *)(local_res10 + 0x10);
      }
      if (param_5 != (undefined8 *)0x0) {
        *param_5 = *(undefined8 *)(local_res10 + 0x18);
      }
      if (param_6 != (longlong *)0x0) {
        *param_6 = (ulonglong)*(byte *)(param_2 + 0x48) * *(longlong *)(local_res10 + 0x18) * 2;
      }
      return 0;
    }
    FUN_14001bb80(param_1,param_7,0x1c,0,"_dwarf_get_ranges",0x30);
  }
  return 1;
}


/* ================= FUN_140019870 @ 140019870  size=217  callers=1 callees=3 ================= */
/* CALLERS: FUN_140005770 */
/* CALLEES: FUN_140019750, FUN_14001bb80, FUN_14001e500 */

undefined8
FUN_140019870(longlong param_1,longlong param_2,undefined4 param_3,longlong param_4,
             undefined8 param_5,undefined8 param_6,undefined8 param_7)

{
  int iVar1;
  undefined8 uVar2;
  
  if (((param_1 == 0) || (param_2 == 0)) || (param_4 == 0)) {
    FUN_14001bb80(param_1,param_7,0x1c,0,"dwarf_get_ranges",0x48);
    uVar2 = 1;
  }
  else if ((*(int *)(param_1 + 0xfc) == 0) &&
          (iVar1 = FUN_14001e500(param_1,1,1,param_7), iVar1 != 0)) {
    uVar2 = 1;
  }
  else {
    uVar2 = FUN_140019750(param_1,param_2,param_3,param_4,param_5,param_6,param_7);
  }
  return uVar2;
}


/* ================= FUN_140019950 @ 140019950  size=392  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400241f0 */
/* CALLEES: FUN_14001bb80, FUN_14001bd50, FUN_140026bb0 */

undefined8
FUN_140019950(longlong param_1,int param_2,longlong *param_3,longlong *param_4,undefined8 param_5)

{
  longlong lVar1;
  longlong lVar2;
  ulonglong uVar3;
  
  uVar3 = (ulonglong)param_2;
  if ((((param_1 == 0) || (param_2 < 0)) || (param_3 == (longlong *)0x0)) ||
     (param_4 == (longlong *)0x0)) {
    FUN_14001bb80(param_1,param_5,0x1c,0,"dwarf_get_str",0x24);
  }
  else {
    lVar1 = FUN_140026bb0(param_1,3);
    if (lVar1 == 0) {
      FUN_14001bd50(param_1,param_5,0x1d,0,"dwarf_get_str",0x2a,".debug_str",0,0);
      return 0xffffffffffffffff;
    }
    if (uVar3 <= *(ulonglong *)(lVar1 + 0x18)) {
      if (uVar3 == *(ulonglong *)(lVar1 + 0x18)) {
        FUN_14001bb80(param_1,param_5,0x23,0,"dwarf_get_str",0x34);
        return 0xffffffffffffffff;
      }
      lVar2 = uVar3 + *(longlong *)(lVar1 + 8);
      lVar1 = -1;
      *param_3 = lVar2;
      do {
        lVar1 = lVar1 + 1;
      } while (*(char *)(lVar2 + lVar1) != '\0');
      *param_4 = lVar1;
      return 0;
    }
    FUN_14001bd50(param_1,param_5,0x1c,0,"dwarf_get_str",0x2f,"offset",0,0);
  }
  return 1;
}


/* ================= FUN_140019ae0 @ 140019ae0  size=357  callers=19 callees=3 ================= */
/* CALLERS: FUN_140006920, FUN_1400070e0, FUN_140009400, FUN_1400094f0, FUN_1400097d0, FUN_140009970, FUN_140009af0, FUN_14000a870, FUN_14000bf80, FUN_14000c2c0, FUN_14000ca30, FUN_14000cea0, FUN_14000dca0, FUN_140019d60, FUN_14001a1d0, FUN_140022a40, FUN_1400230d0, FUN_140023f20, FUN_140024c00 */
/* CALLEES: FUN_14001bb80, calloc, realloc */

void * FUN_140019ae0(uint param_1)

{
  void *pvVar1;
  int iVar2;
  
  if (param_1 < 500000) {
    iVar2 = DAT_14009603c;
    if (1000000 < DAT_140096038 + param_1) {
      DAT_14009603c = DAT_14009603c + 1;
      DAT_140096038 = 0;
      pvVar1 = calloc(1,1000000);
      iVar2 = DAT_14009603c;
      (&DAT_140099d50)[DAT_14009603c] = pvVar1;
      if (pvVar1 == (void *)0x0) {
        FUN_14001bb80(0,0,1,0,"_dwarf_calloc_mem",0x66);
        return (void *)0x0;
      }
    }
    DAT_140096038 = DAT_140096038 + param_1;
    return (void *)((ulonglong)(DAT_140096038 - param_1) + (&DAT_140099d50)[iVar2]);
  }
  if ((int)DAT_14083af54 <= DAT_14083af50) {
    DAT_14083af54 = DAT_14083af54 * 2;
    if ((int)DAT_14083af54 < 10) {
      DAT_14083af54 = 10;
    }
    DAT_14083af58 = realloc(DAT_14083af58,(ulonglong)DAT_14083af54 << 3);
    if (DAT_14083af58 == (void *)0x0) {
      DAT_14083af54 = 0;
      FUN_14001bb80(0,0,1,0,"_dwarf_calloc_mem",0x55);
      return (void *)0x0;
    }
  }
  pvVar1 = calloc(1,(ulonglong)param_1);
  *(void **)((longlong)DAT_14083af58 + (longlong)DAT_14083af50 * 8) = pvVar1;
  DAT_14083af50 = DAT_14083af50 + 1;
  return pvVar1;
}


/* ================= FUN_140019c50 @ 140019c50  size=189  callers=1 callees=2 ================= */
/* CALLERS: FUN_140018180 */
/* CALLEES: free, memset */

void FUN_140019c50(void)

{
  ulonglong uVar1;
  ulonglong uVar2;
  uint uVar3;
  ulonglong uVar4;
  undefined8 *puVar5;
  
  uVar1 = 0;
  uVar2 = uVar1;
  uVar4 = uVar1;
  if (0 < DAT_14083af50) {
    do {
      free(*(void **)(uVar2 + (longlong)DAT_14083af58));
      uVar3 = (int)uVar4 + 1;
      uVar2 = uVar2 + 8;
      uVar4 = (ulonglong)uVar3;
    } while ((int)uVar3 < DAT_14083af50);
  }
  free(DAT_14083af58);
  DAT_14083af58 = (void *)0x0;
  DAT_14083af50 = 0;
  DAT_14083af54 = 0;
  if (-1 < DAT_14009603c) {
    puVar5 = &DAT_140099d50;
    do {
      free((void *)*puVar5);
      uVar3 = (int)uVar1 + 1;
      uVar1 = (ulonglong)uVar3;
      puVar5 = puVar5 + 1;
    } while ((int)uVar3 <= DAT_14009603c);
  }
  memset(&DAT_140099d50,0,8000000);
  DAT_140096038 = 1000000;
  DAT_14009603c = 0xffffffff;
  return;
}


/* ================= FUN_140019d10 @ 140019d10  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_140019d60 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140019d10(ulonglong param_1,undefined8 param_2)

{
  if (param_1 < DAT_14083af60) {
    _DAT_14083af68 = _DAT_14083af68 + 1;
    *(undefined8 *)(DAT_14083af70 + param_1 * 8) = param_2;
  }
  return;
}


/* ================= FUN_140019d30 @ 140019d30  size=37  callers=1 callees=0 ================= */
/* CALLERS: FUN_140019e60 */

void FUN_140019d30(ulonglong param_1,undefined8 *param_2)

{
  if ((DAT_14083af70 != 0) && (param_1 < DAT_14083af60)) {
    *param_2 = *(undefined8 *)(DAT_14083af70 + param_1 * 8);
    return;
  }
  *param_2 = 0;
  return;
}


/* ================= FUN_140019d60 @ 140019d60  size=196  callers=1 callees=3 ================= */
/* CALLERS: FUN_140019fb0 */
/* CALLEES: FUN_140019ae0, FUN_140019d10, FUN_14001bb80 */

undefined4
FUN_140019d60(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined1 param_4,
             undefined8 param_5,longlong *param_6,undefined8 param_7)

{
  undefined4 uVar1;
  undefined8 *puVar2;
  
  puVar2 = (undefined8 *)FUN_140019ae0();
  if (puVar2 == (undefined8 *)0x0) {
    FUN_14001bb80(param_1,param_7,1,0,"_dwarf_abbrev_add",0x66);
    uVar1 = 1;
  }
  else {
    *puVar2 = param_2;
    puVar2[1] = param_3;
    *(undefined1 *)(puVar2 + 2) = param_4;
    puVar2[3] = param_5;
    puVar2[6] = 0;
    puVar2[4] = 0;
    puVar2[5] = 0;
    puVar2[7] = puVar2 + 6;
    FUN_140019d10(param_2,puVar2);
    if (param_6 != (longlong *)0x0) {
      *param_6 = (longlong)puVar2;
    }
    uVar1 = 0;
  }
  return uVar1;
}


/* ================= FUN_140019e30 @ 140019e30  size=45  callers=2 callees=1 ================= */
/* CALLERS: FUN_14001e200, FUN_14001ea20 */
/* CALLEES: free */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140019e30(void)

{
  free(DAT_14083af70);
  DAT_14083af70 = (void *)0x0;
  DAT_14083af60 = 0;
  _DAT_14083af68 = 0;
  return;
}


/* ================= FUN_140019e60 @ 140019e60  size=332  callers=1 callees=5 ================= */
/* CALLERS: FUN_14001b4a0 */
/* CALLEES: FUN_140019d30, FUN_140019fb0, FUN_14001bb80, FUN_140026bb0, calloc */

undefined8 FUN_140019e60(longlong *param_1,longlong param_2,undefined8 *param_3,undefined8 param_4)

{
  longlong lVar1;
  longlong lVar2;
  ulonglong uVar3;
  undefined8 uVar4;
  longlong *local_res8;
  ulonglong local_28 [2];
  
  lVar2 = *param_1;
  if (lVar2 != 0) {
    if (param_3 != (undefined8 *)0x0) {
      if (param_2 != 0) {
        lVar1 = param_1[4];
        FUN_140019d30(param_2 + lVar1,&local_res8);
        if (local_res8 != (longlong *)0x0) {
          *param_3 = local_res8;
          return 0;
        }
        lVar2 = FUN_140026bb0(lVar2,4);
        local_28[0] = param_1[4];
        if (DAT_14083af70 == (void *)0x0) {
          DAT_14083af70 = calloc(1,*(longlong *)(lVar2 + 0x18) << 3);
          uVar3 = *(ulonglong *)(lVar2 + 0x18);
          DAT_14083af60 = uVar3;
        }
        else {
          uVar3 = *(ulonglong *)(lVar2 + 0x18);
        }
        if (local_28[0] < uVar3) {
          do {
            uVar4 = FUN_140019fb0(param_1,local_28,&local_res8,param_4);
            if ((int)uVar4 != 0) {
              return uVar4;
            }
          } while ((*local_res8 != 0) && (local_28[0] < *(ulonglong *)(lVar2 + 0x18)));
        }
        FUN_140019d30(param_2 + lVar1,&local_res8);
        if (local_res8 != (longlong *)0x0) {
          *local_res8 = *local_res8 - param_1[4];
          *param_3 = local_res8;
          return 0;
        }
      }
      return 0x23;
    }
    FUN_14001bb80(0,param_4,0x1c,0,"_dwarf_abbrev_find",0x110);
  }
  return 0x1c;
}


/* ================= FUN_140019fb0 @ 140019fb0  size=542  callers=1 callees=6 ================= */
/* CALLERS: FUN_140019e60 */
/* CALLEES: FUN_140019d60, FUN_14001a1d0, FUN_14001bb80, FUN_14001bd50, FUN_1400266f0, FUN_140026bb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_140019fb0(longlong *param_1,ulonglong *param_2,longlong *param_3,undefined8 param_4)

{
  longlong lVar1;
  ulonglong uVar2;
  longlong lVar3;
  ulonglong uVar4;
  undefined1 uVar5;
  short sVar6;
  longlong lVar7;
  undefined8 uVar8;
  longlong lVar9;
  undefined8 in_stack_ffffffffffffffa0;
  undefined4 uVar10;
  
  uVar10 = (undefined4)((ulonglong)in_stack_ffffffffffffffa0 >> 0x20);
  lVar1 = *param_1;
  if (lVar1 != 0) {
    if ((param_3 != (longlong *)0x0) && (param_2 != (ulonglong *)0x0)) {
      lVar7 = FUN_140026bb0(lVar1,4);
      if (lVar7 == 0) {
        FUN_14001bd50(lVar1,param_4,0x1d,0,"_dwarf_abbrev_parse",CONCAT44(uVar10,200),
                      ".debug_abbrev",0,0);
        uVar8 = 0x1c;
      }
      else {
        uVar2 = *param_2;
        if (uVar2 < *(ulonglong *)(lVar7 + 0x18)) {
          lVar9 = FUN_1400266f0(*(undefined8 *)(lVar7 + 8),param_2);
          if (lVar9 == 0) {
            *param_3 = (longlong)&DAT_14083af80;
            _DAT_14083afa0 = 1;
LAB_14001a090:
            uVar8 = 0;
          }
          else {
            lVar3 = param_1[4];
            uVar8 = FUN_1400266f0(*(undefined8 *)(lVar7 + 8),param_2);
            uVar5 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar7 + 8),param_2,1);
            uVar8 = FUN_140019d60(lVar1,lVar9 + lVar3,uVar8,uVar5,uVar2,param_3,param_4);
            if ((int)uVar8 == 0) {
              do {
                uVar4 = *param_2;
                sVar6 = FUN_1400266f0(*(undefined8 *)(lVar7 + 8),param_2);
                lVar9 = FUN_1400266f0(*(undefined8 *)(lVar7 + 8),param_2);
                if (sVar6 == 0) {
                  *(ulonglong *)(*param_3 + 0x20) = *param_2 - uVar2;
                  goto LAB_14001a090;
                }
                uVar8 = 0;
                if (lVar9 == 0x21) {
                  uVar8 = FUN_1400266f0(*(undefined8 *)(lVar7 + 8),param_2);
                }
                uVar8 = FUN_14001a1d0(lVar1,*param_3,sVar6,lVar9,uVar4,uVar8,0,param_4);
              } while ((int)uVar8 == 0);
            }
          }
        }
        else {
          uVar8 = 0x23;
        }
      }
      return uVar8;
    }
    FUN_14001bb80(lVar1,param_4,0x1c,0,"_dwarf_abbrev_parse",CONCAT44(uVar10,0xc2));
  }
  return 0x1c;
}


/* ================= FUN_14001a1d0 @ 14001a1d0  size=232  callers=1 callees=2 ================= */
/* CALLERS: FUN_140019fb0 */
/* CALLEES: FUN_140019ae0, FUN_14001bb80 */

undefined4
FUN_14001a1d0(undefined8 param_1,longlong param_2,undefined2 param_3,undefined4 param_4,
             undefined4 param_5,undefined8 param_6,longlong *param_7,undefined8 param_8)

{
  undefined4 uVar1;
  undefined2 *puVar2;
  
  if (param_2 == 0) {
    FUN_14001bb80(param_1,param_8,0x1c,0,"_dwarf_attrdef_add",0x8a);
    uVar1 = 0x1c;
  }
  else {
    puVar2 = (undefined2 *)FUN_140019ae0(0x20);
    if (puVar2 == (undefined2 *)0x0) {
      FUN_14001bb80(param_1,param_8,1,0,"_dwarf_attrdef_add",0x90);
      uVar1 = 1;
    }
    else {
      *puVar2 = param_3;
      *(undefined4 *)(puVar2 + 2) = param_4;
      *(undefined4 *)(puVar2 + 4) = param_5;
      *(undefined8 *)(puVar2 + 8) = param_6;
      *(undefined8 *)(puVar2 + 0xc) = 0;
      **(longlong **)(param_2 + 0x38) = (longlong)puVar2;
      *(longlong *)(param_2 + 0x28) = *(longlong *)(param_2 + 0x28) + 1;
      *(undefined2 **)(param_2 + 0x38) = puVar2 + 0xc;
      if (param_7 != (longlong *)0x0) {
        *param_7 = (longlong)puVar2;
      }
      uVar1 = 0;
    }
  }
  return uVar1;
}


/* ================= FUN_14001a2c0 @ 14001a2c0  size=349  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001eb00 */
/* CALLEES: free */

void FUN_14001a2c0(longlong param_1)

{
  longlong *plVar1;
  longlong *plVar2;
  void *pvVar3;
  void *pvVar4;
  void *pvVar5;
  void *pvVar6;
  void *pvVar7;
  longlong lVar8;
  void *_Memory;
  
  if (param_1 != 0) {
    plVar2 = (longlong *)(param_1 + 0x1a0);
    _Memory = (void *)*plVar2;
    while (_Memory != (void *)0x0) {
      plVar1 = (longlong *)((longlong)_Memory + 0x20);
      pvVar3 = *(void **)((longlong)_Memory + 0x30);
      pvVar7 = *(void **)((longlong)_Memory + 0x20);
      while (pvVar7 != (void *)0x0) {
        pvVar4 = (void *)*plVar1;
        pvVar5 = *(void **)((longlong)pvVar7 + 0x30);
        pvVar6 = *(void **)((longlong)pvVar4 + 0x30);
        if (pvVar4 == pvVar7) {
          *plVar1 = (longlong)pvVar6;
          if (pvVar6 == (void *)0x0) {
            *(longlong **)((longlong)_Memory + 0x28) = plVar1;
          }
        }
        else {
          for (; pvVar6 != pvVar7; pvVar6 = *(void **)((longlong)pvVar6 + 0x30)) {
            pvVar4 = pvVar6;
          }
          lVar8 = *(longlong *)(*(longlong *)((longlong)pvVar4 + 0x30) + 0x30);
          *(longlong *)((longlong)pvVar4 + 0x30) = lVar8;
          if (lVar8 == 0) {
            *(longlong **)((longlong)_Memory + 0x28) = (longlong *)((longlong)pvVar4 + 0x30);
          }
        }
        free(pvVar7);
        pvVar7 = pvVar5;
      }
      pvVar7 = (void *)*plVar2;
      if (pvVar7 == _Memory) {
        lVar8 = *(longlong *)((longlong)pvVar7 + 0x30);
        *plVar2 = lVar8;
        if (lVar8 == 0) {
          *(longlong **)(param_1 + 0x1a8) = plVar2;
        }
      }
      else {
        for (pvVar4 = *(void **)((longlong)pvVar7 + 0x30); pvVar4 != _Memory;
            pvVar4 = *(void **)((longlong)pvVar4 + 0x30)) {
          pvVar7 = pvVar4;
        }
        lVar8 = *(longlong *)(*(longlong *)((longlong)pvVar7 + 0x30) + 0x30);
        *(longlong *)((longlong)pvVar7 + 0x30) = lVar8;
        if (lVar8 == 0) {
          *(longlong **)(param_1 + 0x1a8) = (longlong *)((longlong)pvVar7 + 0x30);
        }
      }
      free(_Memory);
      _Memory = pvVar3;
    }
    if (*(void **)(param_1 + 0x1b0) != (void *)0x0) {
      free(*(void **)(param_1 + 0x1b0));
    }
    *(undefined8 *)(param_1 + 0x1b0) = 0;
    *(undefined8 *)(param_1 + 0x1b8) = 0;
  }
  return;
}


/* ================= FUN_14001a420 @ 14001a420  size=193  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001f580 */
/* CALLEES: free */

void FUN_14001a420(longlong param_1)

{
  longlong *plVar1;
  void *_Memory;
  void *pvVar2;
  void *pvVar3;
  void *pvVar4;
  longlong lVar5;
  void *_Memory_00;
  
  _Memory = *(void **)(param_1 + 0x280);
  if (_Memory != (void *)0x0) {
    plVar1 = (longlong *)((longlong)_Memory + 0x20);
    _Memory_00 = *(void **)((longlong)_Memory + 0x20);
    while (_Memory_00 != (void *)0x0) {
      pvVar2 = (void *)*plVar1;
      pvVar3 = *(void **)((longlong)_Memory_00 + 0x30);
      pvVar4 = *(void **)((longlong)pvVar2 + 0x30);
      if (pvVar2 == _Memory_00) {
        *plVar1 = (longlong)pvVar4;
        if (pvVar4 == (void *)0x0) {
          *(longlong **)((longlong)_Memory + 0x28) = plVar1;
        }
      }
      else {
        for (; pvVar4 != _Memory_00; pvVar4 = *(void **)((longlong)pvVar4 + 0x30)) {
          pvVar2 = pvVar4;
        }
        lVar5 = *(longlong *)(*(longlong *)((longlong)pvVar2 + 0x30) + 0x30);
        *(longlong *)((longlong)pvVar2 + 0x30) = lVar5;
        if (lVar5 == 0) {
          *(longlong **)((longlong)_Memory + 0x28) = (longlong *)((longlong)pvVar2 + 0x30);
        }
      }
      free(_Memory_00);
      _Memory_00 = pvVar3;
    }
    free(_Memory);
    *(undefined8 *)(param_1 + 0x280) = 0;
  }
  return;
}


/* ================= FUN_14001a4f0 @ 14001a4f0  size=231  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001ae10 */
/* CALLEES: FUN_14001a610, FUN_14001bb80 */

undefined8 FUN_14001a4f0(longlong param_1,undefined8 *param_2,undefined8 param_3,undefined8 param_4)

{
  short *psVar1;
  undefined8 uVar2;
  short *local_res8;
  
  if ((param_1 == 0) || (param_2 == (undefined8 *)0x0)) {
    FUN_14001bb80(0,param_4,0x1c,0,"_dwarf_attr_add",0x36);
    uVar2 = 0x1c;
  }
  else {
    uVar2 = FUN_14001a610(param_1,&local_res8,param_4);
    if ((int)uVar2 == 0) {
      uVar2 = param_2[1];
      psVar1 = local_res8 + 0xc;
      *(undefined8 *)local_res8 = *param_2;
      *(undefined8 *)(local_res8 + 4) = uVar2;
      uVar2 = param_2[3];
      *(undefined8 *)(local_res8 + 8) = param_2[2];
      *(undefined8 *)(local_res8 + 0xc) = uVar2;
      psVar1[0] = 0;
      psVar1[1] = 0;
      psVar1[2] = 0;
      psVar1[3] = 0;
      **(undefined8 **)(param_1 + 0x78) = local_res8;
      *(short **)(param_1 + 0x78) = psVar1;
      if (*local_res8 == 3) {
        switch(*(undefined4 *)(local_res8 + 2)) {
        case 8:
        case 0xe:
        case 0x1a:
        case 0x1d:
        case 0x1f:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
          *(undefined8 *)(param_1 + 0x58) = param_3;
        }
      }
      return 0;
    }
  }
  return uVar2;
}


/* ================= FUN_14001a610 @ 14001a610  size=192  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001a4f0 */
/* CALLEES: FUN_14001bb80, calloc */

undefined8 FUN_14001a610(longlong param_1,undefined8 *param_2,undefined8 param_3)

{
  void *pvVar1;
  
  if ((param_1 != 0) && (param_2 != (undefined8 *)0x0)) {
    pvVar1 = calloc(1,0x20);
    if (pvVar1 == (void *)0x0) {
      FUN_14001bb80(*(undefined8 *)(param_1 + 0x48),param_3,1,0,"_dwarf_attr_alloc",0x28);
      return 1;
    }
    *param_2 = pvVar1;
    return 0;
  }
  FUN_14001bb80(0,param_3,0x1c,0,"_dwarf_attr_alloc",0x23);
  return 0x1c;
}


/* ================= FUN_14001a6d0 @ 14001a6d0  size=1505  callers=1 callees=18 ================= */
/* CALLERS: FUN_14001ad70 */
/* CALLEES: FUN_14001ba90, FUN_14001bac0, FUN_14001bb80, FUN_14001bd50, FUN_140025ad0, FUN_140025b90, FUN_140025e00, FUN_140025ef0, FUN_140026150, FUN_1400261d0, FUN_140026240, FUN_1400262d0, FUN_140026310, FUN_1400263a0, FUN_140026450, FUN_1400266f0, calloc, free */

longlong *
FUN_14001a6d0(longlong param_1,longlong param_2,longlong param_3,uint param_4,longlong param_5,
             longlong param_6,ushort *param_7,ulonglong param_8,int param_9,undefined8 param_10)

{
  int iVar1;
  int iVar2;
  longlong *_Memory;
  longlong lVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  longlong local_res8;
  undefined8 in_stack_ffffffffffffff90;
  undefined4 uVar6;
  longlong local_48;
  undefined1 local_40 [8];
  
  if (param_1 == 0) {
LAB_14001ac6d:
    uVar4 = 0x102;
LAB_14001ac75:
    FUN_14001bb80(param_1,param_10,0x1c,0,"_dwarf_attr_create",uVar4);
    return (longlong *)0x0;
  }
LAB_14001a710:
  if ((((param_2 != 0) && (param_3 != 0)) && (param_5 != 0)) &&
     ((param_7 != (ushort *)0x0 && (param_6 != 0)))) {
    _Memory = calloc(1,0x68);
    uVar6 = (undefined4)((ulonglong)in_stack_ffffffffffffff90 >> 0x20);
    if (_Memory != (longlong *)0x0) {
      *_Memory = param_6;
      *(uint *)((longlong)_Memory + 0x14) = (uint)*param_7;
      iVar1 = (int)param_8;
      iVar2 = iVar1;
      if (param_9 == 0) {
        iVar2 = *(int *)(param_7 + 2);
      }
      *(int *)(_Memory + 3) = iVar2;
      *(int *)((longlong)_Memory + 0x1c) = param_9;
      _Memory[8] = 0;
      switch(param_8) {
      case 1:
        param_4 = (uint)*(byte *)(param_5 + 0x48);
        goto LAB_14001a943;
      default:
        uVar4 = FUN_14001bac0(param_8 & 0xffffffff);
        uVar5 = FUN_14001ba90(*(undefined4 *)((longlong)_Memory + 0x14));
        FUN_14001bd50(param_1,param_10,6,0,"_dwarf_attr_create",CONCAT44(uVar6,0x1d9),uVar5,
                      *(undefined8 *)(param_6 + 0x58),uVar4);
        free(_Memory);
        return (longlong *)0x0;
      case 3:
        uVar4 = 2;
        goto LAB_14001a801;
      case 4:
        uVar4 = 4;
        goto LAB_14001a801;
      case 5:
      case 0x12:
        uVar4 = 2;
        goto LAB_14001a857;
      case 6:
      case 0x13:
      case 0x1c:
        uVar4 = 4;
        goto LAB_14001a857;
      case 7:
      case 0x14:
      case 0x24:
        uVar4 = 8;
        goto LAB_14001a857;
      case 8:
        FUN_1400262d0(param_1,param_2,param_3,param_5,&local_res8,param_10);
        _Memory[4] = local_res8;
        return _Memory;
      case 9:
      case 0x18:
        lVar3 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
        _Memory[4] = lVar3;
        local_48 = FUN_140025e00(*(undefined8 *)(param_2 + 8),param_3,lVar3);
        goto LAB_14001ab9e;
      case 10:
        uVar4 = 1;
LAB_14001a801:
        lVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,uVar4);
        _Memory[4] = lVar3;
        lVar3 = FUN_140025e00(*(undefined8 *)(param_2 + 8),param_3,lVar3);
        _Memory[5] = lVar3;
        _Memory[6] = _Memory[4];
        _Memory[7] = _Memory[5];
        return _Memory;
      case 0xb:
      case 0xc:
      case 0x11:
        uVar4 = 1;
LAB_14001a857:
        lVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,uVar4);
        _Memory[4] = lVar3;
LAB_14001aba2:
        if (((param_8 != 9) && (param_8 != 10)) && ((param_8 != 3 && (param_8 != 4)))) {
          return _Memory;
        }
        _Memory[6] = _Memory[4];
        _Memory[7] = _Memory[5];
        return _Memory;
      case 0xd:
        lVar3 = FUN_1400261d0(*(undefined8 *)(param_2 + 8),param_3);
        _Memory[4] = lVar3;
        return _Memory;
      case 0xe:
        FUN_140026310(param_1,param_2,param_3,param_5,&local_res8,&local_48,param_10);
        break;
      case 0xf:
      case 0x15:
        lVar3 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
        _Memory[4] = lVar3;
        goto LAB_14001aba2;
      case 0x10:
        if (*(short *)(param_5 + 0x12) != 2) {
          lVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,param_4);
          _Memory[4] = lVar3;
          return _Memory;
        }
        lVar3 = (**(code **)(param_1 + 0x1f8))
                          (*(undefined8 *)(param_2 + 8),param_3,*(undefined1 *)(param_5 + 0x48));
        _Memory[4] = lVar3;
        return _Memory;
      case 0x16:
        goto switchD_14001a7a9_caseD_16;
      case 0x17:
LAB_14001a943:
        lVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,param_4);
        _Memory[4] = lVar3;
        return _Memory;
      case 0x19:
        _Memory[4] = 1;
        return _Memory;
      case 0x1a:
        FUN_1400263a0(param_1,param_2,param_3,param_5,&local_res8,0,&local_48,param_10);
        break;
      case 0x1b:
        FUN_140025ad0(param_1,param_2,param_3,param_5,&local_res8,local_40,&local_48,param_10);
        break;
      case 0x1d:
        lVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,param_4);
        _Memory[4] = lVar3;
        _Memory[5] = 0;
        return _Memory;
      case 0x1e:
        lVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,8);
        _Memory[4] = lVar3;
        lVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,8);
        _Memory[5] = lVar3;
        return _Memory;
      case 0x1f:
        FUN_140026240(param_1,param_2,param_3,param_5,&local_res8,&local_48,param_10);
        break;
      case 0x20:
        _Memory[4] = 8;
        lVar3 = FUN_140025e00(*(undefined8 *)(param_2 + 8),param_3,8);
        _Memory[5] = lVar3;
        return _Memory;
      case 0x21:
        _Memory[4] = *(longlong *)(param_7 + 8);
        return _Memory;
      case 0x22:
        FUN_140025ef0(param_1,param_2,param_3,param_5,&local_res8,&local_48,param_10);
        break;
      case 0x23:
        FUN_140026150(param_1,param_2,param_3,param_5,&local_res8,&local_48,param_10);
        break;
      case 0x25:
      case 0x26:
      case 0x27:
      case 0x28:
        FUN_140026450(param_1,param_2,param_3,param_5,iVar1 + -0x24,&local_res8,0,&local_48,param_10
                     );
        goto LAB_14001ab8f;
      case 0x29:
      case 0x2a:
      case 0x2b:
      case 0x2c:
        FUN_140025b90(param_1,param_2,param_3,param_5,iVar1 + -0x28,&local_res8,0,&local_48,param_10
                     );
LAB_14001ab8f:
        _Memory[4] = local_res8;
LAB_14001ab9e:
        _Memory[5] = local_48;
        goto LAB_14001aba2;
      }
      _Memory[4] = local_res8;
      _Memory[5] = local_48;
      return _Memory;
    }
    uVar4 = CONCAT44(uVar6,0x107);
    goto LAB_14001ac75;
  }
  goto LAB_14001ac6d;
switchD_14001a7a9_caseD_16:
  param_8 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
  param_9 = 1;
  goto LAB_14001a710;
}


/* ================= FUN_14001ad70 @ 14001ad70  size=146  callers=9 callees=2 ================= */
/* CALLERS: FUN_140015220, FUN_1400153e0, FUN_140015520, FUN_140015720, FUN_1400158f0, FUN_140016600, FUN_140018cf0, FUN_140018f60, FUN_1400202a0 */
/* CALLEES: FUN_14001a6d0, FUN_14001b1a0 */

undefined8 FUN_14001ad70(longlong param_1)

{
  longlong lVar1;
  longlong lVar2;
  longlong lVar3;
  undefined8 uVar4;
  longlong lVar5;
  ulonglong local_res18 [2];
  
  lVar3 = FUN_14001b1a0();
  if ((((lVar3 != 0) && (param_1 != 0)) && (lVar1 = *(longlong *)(param_1 + 0x50), lVar1 != 0)) &&
     (lVar2 = *(longlong *)(param_1 + 0x48), lVar2 != 0)) {
    if (*(int *)(lVar1 + 0x88) == 0) {
      lVar5 = *(longlong *)(lVar2 + 0x20);
    }
    else {
      lVar5 = *(longlong *)(lVar2 + 0x10);
    }
    if (lVar5 != 0) {
      local_res18[0] = (ulonglong)*(uint *)(lVar3 + 8);
      uVar4 = FUN_14001a6d0(lVar2,lVar5,local_res18,*(undefined1 *)(lVar1 + 0x49),lVar1,param_1,
                            lVar3,*(undefined4 *)(lVar3 + 4),0,0);
      return uVar4;
    }
  }
  return 0;
}


/* ================= FUN_14001ae10 @ 14001ae10  size=727  callers=1 callees=10 ================= */
/* CALLERS: FUN_14001b4a0 */
/* CALLEES: FUN_14001a4f0, FUN_14001ba90, FUN_14001bac0, FUN_14001bb80, FUN_14001bd50, FUN_1400261d0, FUN_140026240, FUN_1400262d0, FUN_140026310, FUN_1400266f0 */

undefined8
FUN_14001ae10(longlong param_1,longlong param_2,longlong *param_3,int param_4,longlong param_5,
             longlong param_6,undefined2 *param_7,undefined8 param_8,undefined8 param_9,
             undefined8 param_10)

{
  undefined2 uVar1;
  longlong lVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 local_res8;
  undefined8 in_stack_ffffffffffffff70;
  undefined4 uVar5;
  undefined8 local_68;
  undefined8 local_60;
  undefined2 local_58 [2];
  int local_54;
  undefined4 local_50;
  
  if (param_1 == 0) {
LAB_14001b09c:
    FUN_14001bb80(param_1,param_10,0x1c,0,"_dwarf_attr_init",99);
    return 0x1c;
  }
LAB_14001ae5e:
  uVar5 = (undefined4)((ulonglong)in_stack_ffffffffffffff70 >> 0x20);
  if ((((param_2 == 0) || (param_3 == (longlong *)0x0)) || (param_5 == 0)) ||
     ((param_7 == (undefined2 *)0x0 || (param_6 == 0)))) goto LAB_14001b09c;
  local_50 = (undefined4)*param_3;
  uVar1 = *param_7;
  local_54 = (int)param_8;
  local_58[0] = uVar1;
  switch(param_8) {
  case 1:
    goto switchD_14001aec6_caseD_1;
  default:
    uVar3 = FUN_14001bac0();
    uVar4 = FUN_14001ba90(uVar1);
    FUN_14001bd50(param_1,param_10,6,0,"_dwarf_attr_init",CONCAT44(uVar5,0xf2),uVar4,
                  *(undefined8 *)(param_6 + 0x58),uVar3);
    return 6;
  case 3:
    uVar3 = 2;
    goto LAB_14001aeff;
  case 4:
    uVar3 = 4;
    goto LAB_14001aeff;
  case 5:
  case 0x12:
  case 0x2a:
    *param_3 = *param_3 + 2;
    break;
  case 6:
  case 0x13:
  case 0x1c:
  case 0x2c:
    *param_3 = *param_3 + 4;
    break;
  case 7:
  case 0x14:
  case 0x20:
  case 0x24:
    *param_3 = *param_3 + 8;
    break;
  case 8:
    FUN_1400262d0(param_1,param_2,param_3,param_5,&local_res8,param_10);
    local_60 = local_res8;
    goto LAB_14001b02f;
  case 9:
  case 0x18:
    lVar2 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
    *param_3 = *param_3 + lVar2;
    break;
  case 10:
    uVar3 = 1;
LAB_14001aeff:
    lVar2 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,uVar3);
    *param_3 = *param_3 + lVar2;
    break;
  case 0xb:
  case 0xc:
  case 0x11:
  case 0x29:
    *param_3 = *param_3 + 1;
    break;
  case 0xd:
    FUN_1400261d0(*(undefined8 *)(param_2 + 8),param_3);
    break;
  case 0xe:
    FUN_140026310(param_1,param_2,param_3,param_5,0,&local_68,param_10);
    local_60 = local_68;
    goto LAB_14001b02f;
  case 0xf:
  case 0x15:
  case 0x1a:
  case 0x1b:
  case 0x22:
  case 0x23:
    FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
    break;
  case 0x10:
    if (*(short *)(param_5 + 0x12) != 2) goto switchD_14001aec6_caseD_17;
    goto switchD_14001aec6_caseD_1;
  case 0x16:
    goto switchD_14001aec6_caseD_16;
  case 0x17:
switchD_14001aec6_caseD_17:
    *param_3 = *param_3 + (longlong)param_4;
    break;
  case 0x19:
  case 0x21:
    break;
  case 0x1d:
    *param_3 = *param_3 + (ulonglong)*(byte *)(param_5 + 0x49);
    break;
  case 0x1e:
    *param_3 = *param_3 + 0x10;
    break;
  case 0x1f:
    FUN_140026240(param_1,param_2,param_3,param_5,0,&local_60,param_10);
    goto LAB_14001b02f;
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
    *param_3 = *param_3 + (longlong)(local_54 + -0x24);
    break;
  case 0x2b:
    *param_3 = *param_3 + 3;
  }
switchD_14001aec6_caseD_19:
  local_60 = 0;
LAB_14001b02f:
  uVar3 = FUN_14001a4f0(param_6,local_58,local_60,param_10);
  return uVar3;
switchD_14001aec6_caseD_16:
  param_8 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
  goto LAB_14001ae5e;
switchD_14001aec6_caseD_1:
  *param_3 = *param_3 + (ulonglong)*(byte *)(param_5 + 0x48);
  goto switchD_14001aec6_caseD_19;
}


/* ================= FUN_14001b1a0 @ 14001b1a0  size=32  callers=2 callees=0 ================= */
/* CALLERS: FUN_140015300, FUN_14001ad70 */

short * FUN_14001b1a0(longlong param_1,short param_2)

{
  short *psVar1;
  
  if (param_1 != 0) {
    for (psVar1 = *(short **)(param_1 + 0x70); (psVar1 != (short *)0x0 && (*psVar1 != param_2));
        psVar1 = *(short **)(psVar1 + 0xc)) {
    }
    return psVar1;
  }
  return (short *)0x0;
}


/* ================= FUN_14001b1c0 @ 14001b1c0  size=171  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001b270 */
/* CALLEES: FUN_14001bb80, calloc */

undefined8 FUN_14001b1c0(longlong param_1,undefined8 *param_2,undefined8 param_3)

{
  void *pvVar1;
  
  if ((param_2 != (undefined8 *)0x0) && (param_1 != 0)) {
    pvVar1 = calloc(1,0x88);
    if (pvVar1 == (void *)0x0) {
      FUN_14001bb80(param_1,param_3,1,0,"_dwarf_die_alloc",0x2d);
      return 1;
    }
    *(undefined8 **)((longlong)pvVar1 + 0x78) = (undefined8 *)((longlong)pvVar1 + 0x70);
    *(undefined8 *)((longlong)pvVar1 + 0x70) = 0;
    *param_2 = pvVar1;
    return 0;
  }
  return 0x1c;
}


/* ================= FUN_14001b270 @ 14001b270  size=190  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001b4a0 */
/* CALLEES: FUN_14001b1c0, FUN_14001bb80 */

undefined8
FUN_14001b270(longlong *param_1,ulonglong param_2,undefined4 param_3,undefined8 param_4,
             longlong *param_5,undefined8 param_6)

{
  undefined8 uVar1;
  longlong local_res8;
  undefined4 uVar2;
  
  if (param_1 == (longlong *)0x0) {
    uVar2 = 0x3f;
  }
  else {
    if (*param_1 != 0) {
      uVar1 = FUN_14001b1c0(*param_1,&local_res8,param_6);
      if ((int)uVar1 != 0) {
        return uVar1;
      }
      if (local_res8 != 0) {
        *(ulonglong *)(local_res8 + 0x20) = param_2 & 0xffffffff;
        *(undefined4 *)(local_res8 + 0x30) = param_3;
        *(undefined8 *)(local_res8 + 0x38) = param_4;
        *(longlong **)(local_res8 + 0x50) = param_1;
        *(longlong *)(local_res8 + 0x48) = *param_1;
      }
      if (param_5 != (longlong *)0x0) {
        *param_5 = local_res8;
      }
      return 0;
    }
    uVar2 = 0x45;
  }
  FUN_14001bb80(0,param_6,0x1c,0,"_dwarf_die_create",uVar2);
  return 0x1c;
}


/* ================= FUN_14001b330 @ 14001b330  size=182  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400158f0 */
/* CALLEES: FUN_14001b4a0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14001b330(longlong param_1,undefined8 param_2)

{
  longlong lVar1;
  longlong lVar2;
  int iVar3;
  undefined8 uVar4;
  undefined1 auStack_498 [32];
  undefined8 local_478;
  longlong local_470;
  undefined8 *local_468;
  undefined4 local_460;
  undefined1 *local_458;
  undefined8 local_448 [2];
  undefined1 local_438 [1056];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_498;
  if (param_1 != 0) {
    lVar1 = *(longlong *)(param_1 + 0x50);
    lVar2 = *(longlong *)(param_1 + 0x48);
    if ((lVar1 != 0) && (lVar2 != 0)) {
      if (*(int *)(lVar1 + 0x88) == 0) {
        uVar4 = *(undefined8 *)(lVar2 + 0x20);
      }
      else {
        uVar4 = *(undefined8 *)(lVar2 + 0x10);
      }
      local_470 = (longlong)*(int *)(lVar1 + 0x60);
      local_458 = local_438;
      local_468 = local_448;
      local_460 = 0;
      local_478 = param_2;
      iVar3 = FUN_14001b4a0(lVar2,uVar4,lVar1,*(undefined1 *)(lVar1 + 0x49));
      if (iVar3 == 0) {
        return local_448[0];
      }
    }
  }
  return 0;
}


/* ================= FUN_14001b3f0 @ 14001b3f0  size=175  callers=1 callees=2 ================= */
/* CALLERS: FUN_140015c80 */
/* CALLEES: FUN_14001b4a0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14001b3f0(longlong param_1,longlong param_2)

{
  int iVar1;
  undefined8 uVar2;
  undefined1 auStack_498 [32];
  undefined8 local_478;
  longlong local_470;
  undefined8 *local_468;
  undefined4 local_460;
  undefined1 *local_458;
  undefined8 local_448 [2];
  undefined1 local_438 [1056];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_498;
  if ((param_2 != 0) && (param_1 != 0)) {
    local_478 = *(undefined8 *)(param_2 + 0x68);
    if (*(int *)(param_2 + 0x88) == 0) {
      uVar2 = *(undefined8 *)(param_1 + 0x20);
    }
    else {
      uVar2 = *(undefined8 *)(param_1 + 0x10);
    }
    local_470 = (longlong)*(int *)(param_2 + 0x60);
    local_458 = local_438;
    local_468 = local_448;
    local_460 = 0;
    iVar1 = FUN_14001b4a0(param_1,uVar2,param_2,*(undefined1 *)(param_2 + 0x49));
    if (iVar1 == 0) {
      return local_448[0];
    }
  }
  return 0;
}


/* ================= FUN_14001b4a0 @ 14001b4a0  size=581  callers=5 callees=6 ================= */
/* CALLERS: FUN_140016130, FUN_140016210, FUN_140016600, FUN_14001b330, FUN_14001b3f0 */
/* CALLEES: FUN_140015fc0, FUN_140019e60, FUN_14001ae10, FUN_14001b270, FUN_14001bb80, FUN_1400266f0 */

undefined8
FUN_14001b4a0(longlong param_1,longlong param_2,longlong param_3,undefined4 param_4,
             ulonglong param_5,ulonglong param_6,longlong *param_7,int param_8,undefined8 param_9)

{
  longlong lVar1;
  int iVar2;
  undefined8 uVar3;
  longlong lVar4;
  undefined8 uVar5;
  ulonglong uVar6;
  int iVar7;
  longlong local_res18;
  undefined4 local_res20;
  longlong local_48 [2];
  
  uVar3 = param_9;
  iVar2 = param_8;
  local_res20 = param_4;
  if ((((param_3 == 0) || (param_2 == 0)) || (param_1 == 0)) || (param_7 == (longlong *)0x0)) {
    FUN_14001bb80(param_1,param_9,0x1c,0,"_dwarf_die_parse",0xac);
    return 0x1c;
  }
  iVar7 = 1;
  local_48[0] = 0;
  uVar6 = param_5;
  if (param_5 < param_6) {
    do {
      if (*(ulonglong *)(param_2 + 0x18) <= uVar6) {
        return 0x23;
      }
      lVar4 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),&param_5);
      if (lVar4 == 0) {
        if ((iVar7 == 0) || (iVar2 == 0)) {
          return 0xffffffff;
        }
        iVar7 = iVar7 + -1;
      }
      else {
        uVar5 = FUN_140019e60(param_3,lVar4,&local_res18,uVar3);
        if ((int)uVar5 != 0) {
          return uVar5;
        }
        uVar5 = FUN_14001b270(param_3,uVar6,lVar4,local_res18,local_48,uVar3);
        lVar4 = local_48[0];
        if ((int)uVar5 != 0) {
          return uVar5;
        }
        for (lVar1 = *(longlong *)(local_res18 + 0x30); lVar1 != 0;
            lVar1 = *(longlong *)(lVar1 + 0x18)) {
          uVar5 = FUN_14001ae10(param_1,param_2,&param_5,local_res20,param_3,lVar4,lVar1,
                                *(undefined4 *)(lVar1 + 4),0,uVar3);
          if ((int)uVar5 != 0) {
            return uVar5;
          }
        }
        *(ulonglong *)(lVar4 + 0x28) = (ulonglong)(uint)param_5;
        if ((iVar2 == 0) || (iVar7 < 1)) {
          *param_7 = lVar4;
          return 0;
        }
        FUN_140015fc0(param_1,lVar4,8);
        if (*(char *)(local_res18 + 0x10) == '\x01') {
          iVar7 = iVar7 + 1;
        }
      }
      uVar6 = CONCAT44(param_5._4_4_,(uint)param_5);
    } while (CONCAT44(param_5._4_4_,(uint)param_5) < param_6);
  }
  return 0x23;
}


/* ================= FUN_14001b6f0 @ 14001b6f0  size=197  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001f580 */
/* CALLEES: free */

void FUN_14001b6f0(longlong param_1)

{
  longlong *plVar1;
  longlong lVar2;
  void *pvVar3;
  void *pvVar4;
  void *pvVar5;
  longlong lVar6;
  longlong lVar7;
  void *_Memory;
  
  lVar2 = *(longlong *)(param_1 + 600);
  while (lVar7 = lVar2, lVar7 != 0) {
    plVar1 = (longlong *)(lVar7 + 0x70);
    lVar2 = *(longlong *)(lVar7 + 0x80);
    _Memory = *(void **)(lVar7 + 0x70);
    while (_Memory != (void *)0x0) {
      pvVar3 = (void *)*plVar1;
      pvVar4 = *(void **)((longlong)_Memory + 0x18);
      pvVar5 = *(void **)((longlong)pvVar3 + 0x18);
      if (pvVar3 == _Memory) {
        *plVar1 = (longlong)pvVar5;
        if (pvVar5 == (void *)0x0) {
          *(longlong **)(lVar7 + 0x78) = plVar1;
        }
      }
      else {
        for (; pvVar5 != _Memory; pvVar5 = *(void **)((longlong)pvVar5 + 0x18)) {
          pvVar3 = pvVar5;
        }
        lVar6 = *(longlong *)(*(longlong *)((longlong)pvVar3 + 0x18) + 0x18);
        *(longlong *)((longlong)pvVar3 + 0x18) = lVar6;
        if (lVar6 == 0) {
          *(longlong **)(lVar7 + 0x78) = (longlong *)((longlong)pvVar3 + 0x18);
        }
      }
      free(_Memory);
      _Memory = pvVar4;
    }
  }
  return;
}


/* ================= FUN_14001b7c0 @ 14001b7c0  size=24  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400028e0 */

bool FUN_14001b7c0(void)

{
  int iVar1;
  
  iVar1 = FUN_1400028e0();
  return iVar1 != 1;
}


/* ================= FUN_14001b7e0 @ 14001b7e0  size=33  callers=0 callees=1 ================= */
/* CALLEES: FUN_140002900 */

undefined8 FUN_14001b7e0(void)

{
  int iVar1;
  undefined8 uVar2;
  
  iVar1 = FUN_140002900();
  uVar2 = 4;
  if (iVar1 == 2) {
    uVar2 = 8;
  }
  return uVar2;
}


/* ================= FUN_14001b810 @ 14001b810  size=16  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400029a0 */

undefined4 FUN_14001b810(void)

{
  undefined4 uVar1;
  
  uVar1 = FUN_1400029a0();
  return uVar1;
}


/* ================= FUN_14001b820 @ 14001b820  size=137  callers=0 callees=4 ================= */
/* CALLEES: FUN_1400029a0, FUN_140002a20, FUN_140002a50, FUN_14000e5e0 */

undefined8 FUN_14001b820(undefined8 param_1,uint param_2,longlong *param_3,undefined4 *param_4)

{
  uint uVar1;
  undefined8 uVar2;
  longlong lVar3;
  
  param_2 = param_2 & 0xffff;
  uVar1 = FUN_1400029a0();
  if (param_2 < uVar1) {
    lVar3 = FUN_14000e5e0(param_1,param_2);
    *param_3 = lVar3;
    lVar3 = FUN_140002a50(param_1,param_2);
    param_3[1] = lVar3;
    lVar3 = FUN_140002a20(param_1,param_2);
    param_3[2] = lVar3;
    if (*param_3 == -1) {
      param_3[1] = 0;
    }
    uVar2 = 0;
  }
  else {
    if (param_4 != (undefined4 *)0x0) {
      *param_4 = 2;
    }
    uVar2 = 1;
  }
  return uVar2;
}


/* ================= FUN_14001b8b0 @ 14001b8b0  size=100  callers=0 callees=2 ================= */
/* CALLEES: FUN_1400029a0, FUN_1400029c0 */

undefined8 FUN_14001b8b0(undefined8 param_1,uint param_2,longlong *param_3,undefined4 *param_4)

{
  uint uVar1;
  longlong lVar2;
  
  uVar1 = FUN_1400029a0();
  if (((param_2 & 0xffff) < uVar1) && (lVar2 = FUN_1400029c0(param_1,param_2 & 0xffff), lVar2 != 0))
  {
    *param_3 = lVar2;
    return 0;
  }
  if (param_4 != (undefined4 *)0x0) {
    *param_4 = 2;
  }
  return 1;
}


/* ================= FUN_14001b920 @ 14001b920  size=63  callers=2 callees=1 ================= */
/* CALLERS: FUN_140018180, FUN_140018bd0 */
/* CALLEES: free */

void FUN_14001b920(undefined8 *param_1)

{
  void *_Memory;
  
  if ((param_1 != (undefined8 *)0x0) && (_Memory = (void *)*param_1, _Memory != (void *)0x0)) {
    free(*(void **)((longlong)_Memory + 8));
    free(_Memory);
    *param_1 = 0;
  }
  return;
}


/* ================= FUN_14001b960 @ 14001b960  size=291  callers=1 callees=3 ================= */
/* CALLERS: FUN_140018bd0 */
/* CALLEES: FUN_14001bb80, calloc, free */

undefined4 FUN_14001b960(undefined8 *param_1,undefined8 param_2,undefined8 param_3)

{
  undefined8 *_Memory;
  undefined8 *puVar1;
  undefined4 uVar2;
  
  if (param_1 == (undefined8 *)0x0) {
    FUN_14001bb80(0,param_3,0x1c,0,"_dwarf_elf_init",0x3a);
    return 0x1c;
  }
  _Memory = calloc(1,0x10);
  if (_Memory == (undefined8 *)0x0) {
    uVar2 = 0x3e;
  }
  else {
    puVar1 = calloc(1,0x30);
    if (puVar1 != (undefined8 *)0x0) {
      *puVar1 = FUN_14001b820;
      puVar1[1] = FUN_14001b7c0;
      puVar1[2] = FUN_14001b7e0;
      puVar1[3] = FUN_14001b7e0;
      puVar1[4] = FUN_14001b810;
      puVar1[5] = FUN_14001b8b0;
      _Memory[1] = puVar1;
      *_Memory = param_2;
      *param_1 = _Memory;
      return 0;
    }
    free(_Memory);
    uVar2 = 0x43;
  }
  FUN_14001bb80(param_1,param_3,1,0,"_dwarf_elf_init",uVar2);
  return 1;
}


/* ================= FUN_14001ba90 @ 14001ba90  size=47  callers=8 callees=2 ================= */
/* CALLERS: FUN_140015220, FUN_1400153e0, FUN_140015520, FUN_140015720, FUN_1400158f0, FUN_140019250, FUN_14001a6d0, FUN_14001ae10 */
/* CALLEES: FUN_1400168a0, FUN_14001bb20 */

undefined8 FUN_14001ba90(ulonglong param_1)

{
  int iVar1;
  undefined8 uVar2;
  undefined8 local_res10 [3];
  
  iVar1 = FUN_1400168a0(param_1,local_res10);
  if (iVar1 == 0) {
    return local_res10[0];
  }
  uVar2 = FUN_14001bb20(param_1 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_14001bac0 @ 14001bac0  size=47  callers=7 callees=2 ================= */
/* CALLERS: FUN_1400153e0, FUN_140015520, FUN_140015720, FUN_1400158f0, FUN_140019250, FUN_14001a6d0, FUN_14001ae10 */
/* CALLEES: FUN_140017190, FUN_14001bb20 */

undefined8 FUN_14001bac0(ulonglong param_1)

{
  int iVar1;
  undefined8 uVar2;
  undefined8 local_res10 [3];
  
  iVar1 = FUN_140017190(param_1,local_res10);
  if (iVar1 == 0) {
    return local_res10[0];
  }
  uVar2 = FUN_14001bb20(param_1 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_14001baf0 @ 14001baf0  size=43  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001e500 */
/* CALLEES: snprintf */

undefined * FUN_14001baf0(undefined8 param_1)

{
  snprintf(&DAT_14083afe0,0x40,"\'compilation unit @ 0x%llX\'",param_1);
  return &DAT_14083afe0;
}


/* ================= FUN_14001bb20 @ 14001bb20  size=43  callers=5 callees=1 ================= */
/* CALLERS: FUN_1400163c0, FUN_140018780, FUN_14001ba90, FUN_14001bac0, FUN_14001bb50 */
/* CALLEES: snprintf */

undefined * FUN_14001bb20(undefined8 param_1)

{
  snprintf(&DAT_14083afc0,0x20,"0x%llX",param_1);
  return &DAT_14083afc0;
}


/* ================= FUN_14001bb50 @ 14001bb50  size=47  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400222c0 */
/* CALLEES: FUN_1400174a0, FUN_14001bb20 */

undefined8 FUN_14001bb50(ulonglong param_1)

{
  int iVar1;
  undefined8 uVar2;
  undefined8 local_res10 [3];
  
  iVar1 = FUN_1400174a0(param_1,local_res10);
  if (iVar1 == 0) {
    return local_res10[0];
  }
  uVar2 = FUN_14001bb20(param_1 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_14001bb80 @ 14001bb80  size=459  callers=100 callees=0 ================= */
/* CALLERS: FUN_140015220, FUN_140015300, FUN_1400153e0, FUN_140015520, FUN_140015720, FUN_1400158f0, FUN_140015e30, FUN_140016130, FUN_140016210, FUN_140016370, FUN_1400163c0, FUN_140016600, FUN_140016840, FUN_1400181c0, FUN_140018270, FUN_1400182f0, FUN_140018390, FUN_1400184c0, FUN_140018540, FUN_140018650, FUN_140018780, FUN_1400188e0, FUN_140018a20, FUN_140018bd0, FUN_140018cf0, FUN_140018f60, FUN_1400191a0, FUN_140019250, FUN_140019550, FUN_140019750, FUN_140019870, FUN_140019950, FUN_140019ae0, FUN_140019d60, FUN_140019e60, FUN_140019fb0, FUN_14001a1d0, FUN_14001a4f0, FUN_14001a610, FUN_14001a6d0, FUN_14001ae10, FUN_14001b1c0, FUN_14001b270, FUN_14001b4a0, FUN_14001b960, FUN_14001c140, FUN_14001c200, FUN_14001c5c0, FUN_14001c9a0, FUN_14001ca80, FUN_14001cc70, FUN_14001cda0, FUN_14001d060, FUN_14001d290, FUN_14001d440, FUN_14001ddf0, FUN_14001e170, FUN_14001e200, FUN_14001e3c0, FUN_14001e460, FUN_14001e500, FUN_14001e8e0, FUN_14001e980, FUN_14001ea70, FUN_14001ebb0, FUN_14001f440, FUN_14001f610, FUN_14001fc00, FUN_14001fe20, FUN_1400202a0, FUN_140020940, FUN_140020a70, FUN_140021000, FUN_1400211b0, FUN_140021610, FUN_140021720, FUN_140021880, FUN_140021b20, FUN_140021c60, FUN_140021d80, FUN_140022210, FUN_1400222c0, FUN_140022790, FUN_1400228f0, FUN_140022a40, FUN_140022d70, FUN_1400230d0, FUN_1400233d0, FUN_140023f20, FUN_1400241f0, FUN_140024790, FUN_140024c00, FUN_140024f60, FUN_1400250c0, FUN_140025c90, FUN_1400264e0, FUN_140026770, FUN_140026930, FUN_140026ac0, FUN_140026f70 */

void FUN_14001bb80(longlong param_1,undefined8 *param_2,undefined4 param_3,undefined4 param_4,
                  undefined8 param_5,undefined4 param_6)

{
  undefined8 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 *puVar8;
  undefined8 *puVar9;
  undefined8 *puVar10;
  undefined8 *puVar11;
  undefined8 uVar12;
  longlong lVar13;
  code *pcVar14;
  undefined4 local_848;
  undefined4 uStack_844;
  undefined8 uStack_840;
  undefined4 local_838;
  undefined1 uStack_834;
  undefined8 local_428 [133];
  
  uStack_840 = param_5;
  local_838 = param_6;
  local_848 = param_3;
  uStack_844 = param_4;
  uStack_834 = 0;
  if (param_2 != (undefined8 *)0x0) {
    lVar13 = 8;
    puVar8 = (undefined8 *)&local_848;
    do {
      puVar11 = param_2;
      puVar10 = puVar8;
      uVar12 = puVar10[1];
      uVar1 = puVar10[2];
      uVar6 = puVar10[3];
      *puVar11 = *puVar10;
      puVar11[1] = uVar12;
      uVar12 = puVar10[4];
      uVar7 = puVar10[5];
      puVar11[2] = uVar1;
      puVar11[3] = uVar6;
      uVar1 = puVar10[6];
      uVar6 = puVar10[7];
      puVar11[4] = uVar12;
      puVar11[5] = uVar7;
      uVar12 = puVar10[8];
      uVar7 = puVar10[9];
      puVar11[6] = uVar1;
      puVar11[7] = uVar6;
      uVar1 = puVar10[10];
      uVar6 = puVar10[0xb];
      puVar11[8] = uVar12;
      puVar11[9] = uVar7;
      uVar12 = puVar10[0xc];
      uVar7 = puVar10[0xd];
      puVar11[10] = uVar1;
      puVar11[0xb] = uVar6;
      uVar1 = puVar10[0xe];
      uVar6 = puVar10[0xf];
      puVar11[0xc] = uVar12;
      puVar11[0xd] = uVar7;
      puVar11[0xe] = uVar1;
      puVar11[0xf] = uVar6;
      lVar13 = lVar13 + -1;
      puVar8 = puVar10 + 0x10;
      param_2 = puVar11 + 0x10;
    } while (lVar13 != 0);
    uVar1 = puVar10[0x11];
    uVar12 = puVar10[0x12];
    puVar11[0x10] = puVar10[0x10];
    puVar11[0x11] = uVar1;
    puVar11[0x12] = uVar12;
    return;
  }
  if ((param_1 == 0) || (pcVar14 = *(code **)(param_1 + 0x110), pcVar14 == (code *)0x0)) {
    if (DAT_140096020 == (code *)0x0) {
      return;
    }
    lVar13 = 8;
    puVar8 = (undefined8 *)&local_848;
    puVar10 = local_428;
    do {
      puVar11 = puVar10 + 0x10;
      uVar12 = puVar8[1];
      uVar1 = puVar8[2];
      uVar6 = puVar8[3];
      puVar9 = puVar8 + 0x10;
      *puVar10 = *puVar8;
      puVar10[1] = uVar12;
      uVar12 = puVar8[4];
      uVar7 = puVar8[5];
      puVar10[2] = uVar1;
      puVar10[3] = uVar6;
      uVar1 = puVar8[6];
      uVar6 = puVar8[7];
      puVar10[4] = uVar12;
      puVar10[5] = uVar7;
      uVar12 = puVar8[8];
      uVar7 = puVar8[9];
      puVar10[6] = uVar1;
      puVar10[7] = uVar6;
      uVar1 = puVar8[10];
      uVar6 = puVar8[0xb];
      puVar10[8] = uVar12;
      puVar10[9] = uVar7;
      uVar12 = puVar8[0xc];
      uVar7 = puVar8[0xd];
      puVar10[10] = uVar1;
      puVar10[0xb] = uVar6;
      uVar2 = *(undefined4 *)(puVar8 + 0xe);
      uVar3 = *(undefined4 *)((longlong)puVar8 + 0x74);
      uVar4 = *(undefined4 *)(puVar8 + 0xf);
      uVar5 = *(undefined4 *)((longlong)puVar8 + 0x7c);
      puVar10[0xc] = uVar12;
      puVar10[0xd] = uVar7;
      *(undefined4 *)(puVar10 + 0xe) = uVar2;
      *(undefined4 *)((longlong)puVar10 + 0x74) = uVar3;
      *(undefined4 *)(puVar10 + 0xf) = uVar4;
      *(undefined4 *)((longlong)puVar10 + 0x7c) = uVar5;
      lVar13 = lVar13 + -1;
      puVar8 = puVar9;
      puVar10 = puVar11;
      uVar12 = DAT_140096028;
      pcVar14 = DAT_140096020;
    } while (lVar13 != 0);
  }
  else {
    lVar13 = 8;
    puVar8 = (undefined8 *)&local_848;
    puVar10 = local_428;
    do {
      puVar11 = puVar10 + 0x10;
      uVar12 = puVar8[1];
      uVar1 = puVar8[2];
      uVar6 = puVar8[3];
      puVar9 = puVar8 + 0x10;
      *puVar10 = *puVar8;
      puVar10[1] = uVar12;
      uVar12 = puVar8[4];
      uVar7 = puVar8[5];
      puVar10[2] = uVar1;
      puVar10[3] = uVar6;
      uVar1 = puVar8[6];
      uVar6 = puVar8[7];
      puVar10[4] = uVar12;
      puVar10[5] = uVar7;
      uVar12 = puVar8[8];
      uVar7 = puVar8[9];
      puVar10[6] = uVar1;
      puVar10[7] = uVar6;
      uVar1 = puVar8[10];
      uVar6 = puVar8[0xb];
      puVar10[8] = uVar12;
      puVar10[9] = uVar7;
      uVar12 = puVar8[0xc];
      uVar7 = puVar8[0xd];
      puVar10[10] = uVar1;
      puVar10[0xb] = uVar6;
      uVar2 = *(undefined4 *)(puVar8 + 0xe);
      uVar3 = *(undefined4 *)((longlong)puVar8 + 0x74);
      uVar4 = *(undefined4 *)(puVar8 + 0xf);
      uVar5 = *(undefined4 *)((longlong)puVar8 + 0x7c);
      puVar10[0xc] = uVar12;
      puVar10[0xd] = uVar7;
      *(undefined4 *)(puVar10 + 0xe) = uVar2;
      *(undefined4 *)((longlong)puVar10 + 0x74) = uVar3;
      *(undefined4 *)(puVar10 + 0xf) = uVar4;
      *(undefined4 *)((longlong)puVar10 + 0x7c) = uVar5;
      lVar13 = lVar13 + -1;
      puVar8 = puVar9;
      puVar10 = puVar11;
    } while (lVar13 != 0);
    uVar12 = *(undefined8 *)(param_1 + 0x118);
  }
  uVar2 = *(undefined4 *)((longlong)puVar9 + 4);
  uVar3 = *(undefined4 *)(puVar9 + 1);
  uVar4 = *(undefined4 *)((longlong)puVar9 + 0xc);
  uVar1 = puVar9[2];
  *(undefined4 *)puVar11 = *(undefined4 *)puVar9;
  *(undefined4 *)((longlong)puVar11 + 4) = uVar2;
  *(undefined4 *)(puVar11 + 1) = uVar3;
  *(undefined4 *)((longlong)puVar11 + 0xc) = uVar4;
  puVar11[2] = uVar1;
  (*pcVar14)(local_428,uVar12);
  return;
}


/* ================= FUN_14001bd50 @ 14001bd50  size=571  callers=25 callees=0 ================= */
/* CALLERS: FUN_140015220, FUN_1400153e0, FUN_140015520, FUN_140015720, FUN_1400158f0, FUN_1400163c0, FUN_140018780, FUN_140018a20, FUN_140018cf0, FUN_140018f60, FUN_140019250, FUN_140019950, FUN_140019fb0, FUN_14001a6d0, FUN_14001ae10, FUN_14001e500, FUN_14001ebb0, FUN_1400222c0, FUN_140022a40, FUN_1400230d0, FUN_140024c00, FUN_140025c90, FUN_1400264e0, FUN_140026770, FUN_140026f70 */

void FUN_14001bd50(longlong param_1,undefined8 *param_2,undefined4 param_3,undefined4 param_4,
                  undefined8 param_5,undefined4 param_6,longlong param_7,longlong param_8,
                  longlong param_9)

{
  char *pcVar1;
  undefined8 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined8 *puVar10;
  undefined8 *puVar11;
  undefined8 *puVar12;
  undefined8 *puVar13;
  undefined8 uVar14;
  longlong lVar15;
  code *pcVar16;
  undefined4 local_848;
  undefined4 uStack_844;
  undefined8 uStack_840;
  undefined4 local_838;
  undefined1 uStack_834;
  undefined8 local_428 [133];
  
  uStack_840 = param_5;
  local_838 = param_6;
  local_848 = param_3;
  uStack_844 = param_4;
  uStack_834 = 0;
  if (param_2 != (undefined8 *)0x0) {
    lVar15 = 8;
    puVar10 = (undefined8 *)&local_848;
    do {
      puVar13 = param_2;
      puVar12 = puVar10;
      uVar14 = puVar12[1];
      uVar2 = puVar12[2];
      uVar7 = puVar12[3];
      *puVar13 = *puVar12;
      puVar13[1] = uVar14;
      uVar14 = puVar12[4];
      uVar8 = puVar12[5];
      puVar13[2] = uVar2;
      puVar13[3] = uVar7;
      uVar2 = puVar12[6];
      uVar7 = puVar12[7];
      puVar13[4] = uVar14;
      puVar13[5] = uVar8;
      uVar14 = puVar12[8];
      uVar8 = puVar12[9];
      puVar13[6] = uVar2;
      puVar13[7] = uVar7;
      uVar2 = puVar12[10];
      uVar7 = puVar12[0xb];
      puVar13[8] = uVar14;
      puVar13[9] = uVar8;
      uVar14 = puVar12[0xc];
      uVar8 = puVar12[0xd];
      puVar13[10] = uVar2;
      puVar13[0xb] = uVar7;
      uVar2 = puVar12[0xe];
      uVar7 = puVar12[0xf];
      puVar13[0xc] = uVar14;
      puVar13[0xd] = uVar8;
      puVar13[0xe] = uVar2;
      puVar13[0xf] = uVar7;
      lVar15 = lVar15 + -1;
      puVar10 = puVar12 + 0x10;
      param_2 = puVar13 + 0x10;
    } while (lVar15 != 0);
    uVar2 = puVar12[0x11];
    uVar14 = puVar12[0x12];
    puVar13[0x10] = puVar12[0x10];
    puVar13[0x11] = uVar2;
    puVar13[0x12] = uVar14;
    return;
  }
  if ((param_1 == 0) || (pcVar16 = *(code **)(param_1 + 0x110), pcVar16 == (code *)0x0)) {
    if (DAT_140096020 == (code *)0x0) {
      return;
    }
    lVar15 = 8;
    puVar10 = (undefined8 *)&local_848;
    puVar12 = local_428;
    do {
      puVar13 = puVar12 + 0x10;
      uVar14 = puVar10[1];
      uVar2 = puVar10[2];
      uVar7 = puVar10[3];
      puVar11 = puVar10 + 0x10;
      *puVar12 = *puVar10;
      puVar12[1] = uVar14;
      uVar14 = puVar10[4];
      uVar8 = puVar10[5];
      puVar12[2] = uVar2;
      puVar12[3] = uVar7;
      uVar2 = puVar10[6];
      uVar7 = puVar10[7];
      puVar12[4] = uVar14;
      puVar12[5] = uVar8;
      uVar14 = puVar10[8];
      uVar8 = puVar10[9];
      puVar12[6] = uVar2;
      puVar12[7] = uVar7;
      uVar2 = puVar10[10];
      uVar7 = puVar10[0xb];
      puVar12[8] = uVar14;
      puVar12[9] = uVar8;
      uVar14 = puVar10[0xc];
      uVar8 = puVar10[0xd];
      puVar12[10] = uVar2;
      puVar12[0xb] = uVar7;
      uVar3 = *(undefined4 *)(puVar10 + 0xe);
      uVar4 = *(undefined4 *)((longlong)puVar10 + 0x74);
      uVar5 = *(undefined4 *)(puVar10 + 0xf);
      uVar6 = *(undefined4 *)((longlong)puVar10 + 0x7c);
      puVar12[0xc] = uVar14;
      puVar12[0xd] = uVar8;
      *(undefined4 *)(puVar12 + 0xe) = uVar3;
      *(undefined4 *)((longlong)puVar12 + 0x74) = uVar4;
      *(undefined4 *)(puVar12 + 0xf) = uVar5;
      *(undefined4 *)((longlong)puVar12 + 0x7c) = uVar6;
      lVar15 = lVar15 + -1;
      puVar10 = puVar11;
      puVar12 = puVar13;
      uVar14 = DAT_140096028;
      pcVar16 = DAT_140096020;
    } while (lVar15 != 0);
  }
  else {
    pcVar1 = *(char **)(param_1 + 0x118);
    uVar14 = 0;
    if (pcVar1 != (char *)0x0) {
      if ('\0' < *pcVar1) {
        return;
      }
      *(longlong *)(pcVar1 + 8) = param_7;
      *(longlong *)(pcVar1 + 0x10) = param_8;
      *(longlong *)(pcVar1 + 0x18) = param_9;
      *pcVar1 = (param_7 != 0) + (param_8 != 0) + (param_9 != 0);
      uVar14 = *(undefined8 *)(param_1 + 0x118);
      pcVar16 = *(code **)(param_1 + 0x110);
    }
    lVar15 = 8;
    puVar10 = (undefined8 *)&local_848;
    puVar12 = local_428;
    do {
      puVar13 = puVar12 + 0x10;
      uVar2 = puVar10[1];
      uVar7 = puVar10[2];
      uVar8 = puVar10[3];
      puVar11 = puVar10 + 0x10;
      *puVar12 = *puVar10;
      puVar12[1] = uVar2;
      uVar2 = puVar10[4];
      uVar9 = puVar10[5];
      puVar12[2] = uVar7;
      puVar12[3] = uVar8;
      uVar7 = puVar10[6];
      uVar8 = puVar10[7];
      puVar12[4] = uVar2;
      puVar12[5] = uVar9;
      uVar2 = puVar10[8];
      uVar9 = puVar10[9];
      puVar12[6] = uVar7;
      puVar12[7] = uVar8;
      uVar7 = puVar10[10];
      uVar8 = puVar10[0xb];
      puVar12[8] = uVar2;
      puVar12[9] = uVar9;
      uVar2 = puVar10[0xc];
      uVar9 = puVar10[0xd];
      puVar12[10] = uVar7;
      puVar12[0xb] = uVar8;
      uVar3 = *(undefined4 *)(puVar10 + 0xe);
      uVar4 = *(undefined4 *)((longlong)puVar10 + 0x74);
      uVar5 = *(undefined4 *)(puVar10 + 0xf);
      uVar6 = *(undefined4 *)((longlong)puVar10 + 0x7c);
      puVar12[0xc] = uVar2;
      puVar12[0xd] = uVar9;
      *(undefined4 *)(puVar12 + 0xe) = uVar3;
      *(undefined4 *)((longlong)puVar12 + 0x74) = uVar4;
      *(undefined4 *)(puVar12 + 0xf) = uVar5;
      *(undefined4 *)((longlong)puVar12 + 0x7c) = uVar6;
      lVar15 = lVar15 + -1;
      puVar10 = puVar11;
      puVar12 = puVar13;
    } while (lVar15 != 0);
  }
  uVar3 = *(undefined4 *)((longlong)puVar11 + 4);
  uVar4 = *(undefined4 *)(puVar11 + 1);
  uVar5 = *(undefined4 *)((longlong)puVar11 + 0xc);
  uVar2 = puVar11[2];
  *(undefined4 *)puVar13 = *(undefined4 *)puVar11;
  *(undefined4 *)((longlong)puVar13 + 4) = uVar3;
  *(undefined4 *)(puVar13 + 1) = uVar4;
  *(undefined4 *)((longlong)puVar13 + 0xc) = uVar5;
  puVar13[2] = uVar2;
  (*pcVar16)(local_428,uVar14);
  return;
}


/* ================= FUN_14001bfa0 @ 14001bfa0  size=98  callers=0 callees=1 ================= */
/* CALLEES: FUN_14001c0a0 */

void FUN_14001bfa0(longlong param_1,undefined8 param_2,longlong param_3,undefined8 param_4,
                  longlong param_5)

{
  undefined8 uVar1;
  
  uVar1 = (**(code **)(param_1 + 0x1f8))
                    (*(undefined8 *)(param_3 + 8),param_4,*(undefined4 *)(param_1 + 0xf4));
  *(undefined8 *)(param_5 + 0x30) = uVar1;
  FUN_14001c0a0(param_1,param_2,param_3,param_4);
  return;
}


/* ================= FUN_14001c010 @ 14001c010  size=134  callers=1 callees=3 ================= */
/* CALLERS: FUN_14001c200 */
/* CALLEES: FUN_14001c0a0, FUN_14001cda0, FUN_1400266f0 */

void FUN_14001c010(undefined8 param_1,undefined8 param_2,longlong param_3,longlong *param_4,
                  longlong param_5,undefined8 param_6)

{
  longlong lVar1;
  
  FUN_14001c0a0();
  lVar1 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),param_4);
  *(longlong *)(param_5 + 0x50) = lVar1;
  *(longlong *)(param_5 + 0x58) = *param_4 + *(longlong *)(param_3 + 8);
  *param_4 = *param_4 + lVar1;
  *(undefined1 *)(param_5 + 0x60) = 0;
  FUN_14001cda0(param_1,param_5,param_6);
  return;
}


/* ================= FUN_14001c0a0 @ 14001c0a0  size=146  callers=3 callees=3 ================= */
/* CALLERS: FUN_14001bfa0, FUN_14001c010, FUN_14001c200 */
/* CALLEES: FUN_14001c140, FUN_1400261d0, FUN_1400266f0 */

undefined8
FUN_14001c0a0(longlong param_1,undefined8 param_2,longlong param_3,undefined8 param_4,
             longlong param_5)

{
  undefined8 uVar1;
  
  FUN_14001c140();
  uVar1 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),param_4);
  *(undefined8 *)(param_5 + 0x38) = uVar1;
  uVar1 = FUN_1400261d0(*(undefined8 *)(param_3 + 8),param_4);
  *(undefined8 *)(param_5 + 0x40) = uVar1;
  if (*(short *)(param_5 + 0x20) == 1) {
    uVar1 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),param_4,1);
  }
  else {
    uVar1 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),param_4);
  }
  *(undefined8 *)(param_5 + 0x48) = uVar1;
  return 0;
}


/* ================= FUN_14001c140 @ 14001c140  size=190  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001c0a0 */
/* CALLEES: FUN_14001bb80 */

undefined8
FUN_14001c140(undefined8 param_1,undefined8 param_2,longlong param_3,longlong *param_4,
             longlong param_5,undefined8 param_6)

{
  byte bVar1;
  longlong lVar2;
  undefined8 uVar3;
  undefined4 uVar4;
  
  if (*(ushort *)(param_5 + 0x20) < 4) {
    return 0xffffffff;
  }
  lVar2 = *param_4;
  if (lVar2 + 2U < *(ulonglong *)(param_3 + 0x18)) {
    bVar1 = *(byte *)(lVar2 + *(longlong *)(param_3 + 8));
    if (bVar1 == 0) {
      uVar4 = 0x4d;
      uVar3 = 0x10;
    }
    else if (bVar1 < 9) {
      *param_4 = lVar2 + 1;
      if (*(byte *)(lVar2 + 1 + *(longlong *)(param_3 + 8)) < 9) {
        *param_4 = lVar2 + 2;
        return 0;
      }
      uVar4 = 0x5a;
      uVar3 = 0x11;
    }
    else {
      uVar4 = 0x51;
      uVar3 = 0x10;
    }
  }
  else {
    uVar4 = 0x45;
    uVar3 = 0xf;
  }
  FUN_14001bb80(param_1,param_6,uVar3,0,"_ParseCFI_V4_Members",uVar4);
  return 1;
}


/* ================= FUN_14001c200 @ 14001c200  size=949  callers=2 callees=6 ================= */
/* CALLERS: FUN_14001c5c0, FUN_14001ddf0 */
/* CALLEES: FUN_14001bb80, FUN_14001c010, FUN_14001c0a0, FUN_14001ca50, calloc, strncmp */

undefined4
FUN_14001c200(longlong param_1,longlong param_2,longlong param_3,longlong *param_4,
             undefined8 *param_5,undefined8 param_6)

{
  char cVar1;
  short sVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  longlong *plVar6;
  ulonglong uVar7;
  longlong lVar8;
  longlong lVar9;
  size_t _MaxCount;
  undefined **ppuVar10;
  int local_res8;
  undefined4 uStackX_c;
  undefined8 in_stack_ffffffffffffffd0;
  
  uVar4 = (undefined4)((ulonglong)in_stack_ffffffffffffffd0 >> 0x20);
  if ((((param_1 == 0) || (param_3 == 0)) || (param_2 == 0)) || (param_4 == (longlong *)0x0)) {
    FUN_14001bb80(param_1,param_6,0x1c,0,"_dwarf_frame_add_cie",0x173);
    return 0x1c;
  }
  lVar8 = *param_4;
  plVar6 = param_4;
  iVar3 = FUN_14001ca50(param_2,lVar8,&local_res8);
  if (iVar3 != 0x23) {
    *plVar6 = *(longlong *)(CONCAT44(uStackX_c,local_res8) + 0x18) + 4 + lVar8;
    return 0;
  }
  plVar6 = calloc(1,0x80);
  if (plVar6 == (longlong *)0x0) {
    FUN_14001bb80(param_1,param_6,1,0,"_dwarf_frame_add_cie",0x180);
    return 1;
  }
  iVar3 = 0;
  plVar6[0xf] = 0;
  iVar5 = 4;
  local_res8 = 4;
  **(undefined8 **)(param_2 + 8) = plVar6;
  *(longlong **)(param_2 + 8) = plVar6 + 0xf;
  *plVar6 = param_1;
  plVar6[1] = *(longlong *)(param_2 + 0x28);
  plVar6[2] = *param_4;
  uVar7 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),param_4,4);
  if (uVar7 == 0xffffffff) {
    iVar5 = 8;
    local_res8 = 8;
    uVar7 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),param_4,8);
  }
  if ((ulonglong)(*(longlong *)(param_3 + 0x18) - *param_4) < uVar7) {
    FUN_14001bb80(param_1,param_6,0xf,0,"_dwarf_frame_add_cie",CONCAT44(uVar4,0x194));
    uVar4 = 0xf;
  }
  else {
    (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),param_4,iVar5);
    plVar6[3] = uVar7;
    sVar2 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),param_4,1);
    *(short *)(plVar6 + 4) = sVar2;
    if (((sVar2 - 1U & 0xfffc) == 0) && (sVar2 != 2)) {
      plVar6[5] = *param_4 + *(longlong *)(param_3 + 8);
      lVar8 = *(longlong *)(param_3 + 8);
      lVar9 = *param_4;
      do {
        cVar1 = *(char *)(lVar9 + lVar8);
        lVar9 = lVar9 + 1;
        *param_4 = lVar9;
      } while (cVar1 != '\0');
      if (((char *)plVar6[5] == (char *)0x0) || (*(char *)plVar6[5] == '\0')) {
        FUN_14001c0a0(param_1,param_2,param_3,param_4,plVar6,param_6);
        local_res8 = iVar5;
      }
      else {
        ppuVar10 = &PTR_DAT_140096040;
        do {
          _MaxCount = 0xffffffffffffffff;
          do {
            _MaxCount = _MaxCount + 1;
          } while ((*ppuVar10)[_MaxCount] != '\0');
          iVar5 = strncmp((char *)plVar6[5],*ppuVar10,_MaxCount);
          if (iVar5 == 0) {
            (*(code *)(&PTR_FUN_140096048)[(longlong)iVar3 * 2])
                      (param_1,param_2,param_3,param_4,plVar6,param_6);
            break;
          }
          if (iVar3 == 3) {
            FUN_14001bb80(param_1,param_6,0x13,0,"_dwarf_frame_add_cie",0x1be);
            break;
          }
          iVar3 = iVar3 + 1;
          ppuVar10 = ppuVar10 + 2;
        } while (iVar3 < 3);
      }
      plVar6[0xd] = *param_4 + *(longlong *)(param_3 + 8);
      if (local_res8 == 4) {
        lVar8 = (plVar6[2] - *param_4) + 4;
      }
      else {
        lVar8 = (plVar6[2] - *param_4) + 0xc;
      }
      plVar6[0xe] = lVar8 + uVar7;
      *param_4 = *param_4 + lVar8 + uVar7;
      if (param_5 != (undefined8 *)0x0) {
        *param_5 = plVar6;
      }
      *(longlong *)(param_2 + 0x28) = *(longlong *)(param_2 + 0x28) + 1;
      uVar4 = 0;
    }
    else {
      FUN_14001bb80(param_1,param_6,0x15,0,"_dwarf_frame_add_cie",CONCAT44(uVar4,0x1a3));
      uVar4 = 0x15;
    }
  }
  return uVar4;
}


/* ================= FUN_14001c5c0 @ 14001c5c0  size=985  callers=1 callees=6 ================= */
/* CALLERS: FUN_14001ddf0 */
/* CALLEES: FUN_14001bb80, FUN_14001c200, FUN_14001ca50, FUN_14001d060, FUN_1400266f0, calloc */

undefined8
FUN_14001c5c0(longlong param_1,longlong param_2,longlong param_3,longlong *param_4,int param_5,
             undefined8 param_6)

{
  int iVar1;
  longlong *plVar2;
  ulonglong uVar3;
  longlong lVar4;
  undefined8 uVar5;
  undefined4 uVar6;
  longlong lVar7;
  int local_res8;
  undefined8 in_stack_ffffffffffffffb0;
  undefined4 uVar8;
  longlong local_38;
  ulonglong local_30;
  
  uVar8 = (undefined4)((ulonglong)in_stack_ffffffffffffffb0 >> 0x20);
  if ((((param_1 == 0) || (param_2 == 0)) || (param_3 == 0)) || (param_4 == (longlong *)0x0)) {
    FUN_14001bb80(param_1,param_6,0x1c,0,"_dwarf_frame_add_fde",0x1ee);
    return 0x1c;
  }
  plVar2 = calloc(1,0x90);
  if (plVar2 == (longlong *)0x0) {
    FUN_14001bb80(param_1,param_6,1,0,"_dwarf_frame_add_fde",500);
    return 1;
  }
  plVar2[0x11] = 0;
  lVar7 = 4;
  uVar6 = 4;
  **(undefined8 **)(param_2 + 0x18) = plVar2;
  *(longlong **)(param_2 + 0x18) = plVar2 + 0x11;
  *plVar2 = param_1;
  plVar2[2] = param_2;
  plVar2[3] = *(longlong *)(param_3 + 8) + *param_4;
  plVar2[4] = *param_4;
  uVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),param_4,4);
  if (uVar3 == 0xffffffff) {
    uVar6 = 8;
    local_res8 = 8;
    uVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),param_4,8);
  }
  else {
    local_res8 = 4;
  }
  if ((ulonglong)(*(longlong *)(param_3 + 0x18) - *param_4) < uVar3) {
    FUN_14001bb80(param_1,param_6,0xf,0,"_dwarf_frame_add_fde",CONCAT44(uVar8,0x209));
    return 0xf;
  }
  plVar2[5] = uVar3;
  if (param_5 == 1) {
    lVar4 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),param_4,4);
    plVar2[6] = lVar4;
    local_30 = (*param_4 - lVar4) - 4;
    if (local_30 == plVar2[4]) {
      uVar5 = CONCAT44(uVar8,0x216);
    }
    else {
      if (local_30 < *(ulonglong *)(param_3 + 0x18)) goto LAB_14001c7e5;
      uVar5 = CONCAT44(uVar8,0x21d);
    }
  }
  else {
    local_30 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),param_4,uVar6);
    plVar2[6] = local_30;
    if (local_30 < *(ulonglong *)(param_3 + 0x18)) {
LAB_14001c7e5:
      iVar1 = FUN_14001ca50(param_2,local_30,&local_38);
      if ((iVar1 == 0x23) &&
         (uVar5 = FUN_14001c200(param_1,param_2,param_3,&local_30,&local_38,param_6),
         (int)uVar5 != 0)) {
        return uVar5;
      }
      lVar4 = local_38;
      plVar2[1] = local_38;
      if (param_5 == 1) {
        uVar5 = FUN_14001d060(param_1,&local_38,*(undefined8 *)(param_3 + 8),param_4,
                              *(undefined1 *)(local_38 + 0x60),
                              *(longlong *)(param_3 + 0x10) + *param_4,param_6);
        if ((int)uVar5 != 0) {
          return uVar5;
        }
        plVar2[7] = local_38;
        uVar5 = FUN_14001d060(param_1,&local_38,*(undefined8 *)(param_3 + 8),param_4,
                              *(undefined1 *)(lVar4 + 0x60),0,param_6);
        if ((int)uVar5 != 0) {
          return uVar5;
        }
        plVar2[8] = local_38;
        if (**(char **)(lVar4 + 0x28) == 'z') {
          lVar4 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),param_4);
          plVar2[9] = lVar4;
          plVar2[10] = *(longlong *)(param_3 + 8) + *param_4;
          *param_4 = *param_4 + lVar4;
        }
      }
      else {
        lVar4 = (**(code **)(param_1 + 0x1f8))
                          (*(undefined8 *)(param_3 + 8),param_4,*(undefined4 *)(param_1 + 0xf4));
        plVar2[7] = lVar4;
        lVar4 = (**(code **)(param_1 + 0x1f8))
                          (*(undefined8 *)(param_3 + 8),param_4,*(undefined4 *)(param_1 + 0xf4));
        plVar2[8] = lVar4;
      }
      plVar2[0xb] = *(longlong *)(param_3 + 8) + *param_4;
      if (local_res8 != 4) {
        lVar7 = 0xc;
      }
      lVar7 = (lVar7 - *param_4) + plVar2[4] + uVar3;
      plVar2[0xc] = lVar7;
      *param_4 = *param_4 + lVar7;
      *(longlong *)(param_2 + 0x38) = *(longlong *)(param_2 + 0x38) + 1;
      return 0;
    }
    uVar5 = CONCAT44(uVar8,0x22a);
  }
  FUN_14001bb80(param_1,param_6,0x12,0,"_dwarf_frame_add_fde",uVar5);
  return 0x12;
}


/* ================= FUN_14001c9a0 @ 14001c9a0  size=168  callers=1 callees=3 ================= */
/* CALLERS: FUN_14001eb00 */
/* CALLEES: FUN_14001bb80, FUN_14001dca0, free */

void FUN_14001c9a0(longlong param_1)

{
  void *_Memory;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,0,0x1c,0,"_dwarf_frame_cleanup",0x5cc);
    return;
  }
  _Memory = *(void **)(param_1 + 0x228);
  if (_Memory != (void *)0x0) {
    free(*(void **)((longlong)_Memory + 0x20));
    free(_Memory);
    *(undefined8 *)(param_1 + 0x228) = 0;
  }
  if (*(longlong *)(param_1 + 400) != 0) {
    FUN_14001dca0();
    *(undefined8 *)(param_1 + 400) = 0;
  }
  if (*(longlong *)(param_1 + 0x198) != 0) {
    FUN_14001dca0();
    *(undefined8 *)(param_1 + 0x198) = 0;
  }
  return;
}


/* ================= FUN_14001ca50 @ 14001ca50  size=40  callers=2 callees=0 ================= */
/* CALLERS: FUN_14001c200, FUN_14001c5c0 */

undefined8 FUN_14001ca50(longlong *param_1,longlong param_2,longlong *param_3)

{
  longlong lVar1;
  
  lVar1 = *param_1;
  while( true ) {
    if (lVar1 == 0) {
      return 0x23;
    }
    if (*(longlong *)(lVar1 + 0x10) == param_2) break;
    lVar1 = *(longlong *)(lVar1 + 0x78);
  }
  if (param_3 != (longlong *)0x0) {
    *param_3 = lVar1;
  }
  return 0;
}


/* ================= FUN_14001ca80 @ 14001ca80  size=485  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400188e0 */
/* CALLEES: FUN_14001bb80, FUN_14001d440, memset */

undefined8
FUN_14001ca80(longlong *param_1,ulonglong param_2,undefined8 *param_3,undefined8 *param_4,
             undefined8 param_5)

{
  longlong lVar1;
  undefined8 *puVar2;
  longlong lVar3;
  undefined8 uVar4;
  ulonglong uVar5;
  uint uVar6;
  ulonglong uVar7;
  undefined8 local_res8;
  
  if (param_1 == (longlong *)0x0) {
    FUN_14001bb80(0,param_5,0x1c,0,"_dwarf_frame_get_internal_table",0x598);
    return 0x1c;
  }
  lVar1 = *param_1;
  if ((lVar1 != 0) && (puVar2 = *(undefined8 **)(lVar1 + 0x228), puVar2 != (undefined8 *)0x0)) {
    *puVar2 = 0;
    puVar2[1] = 0;
    puVar2[2] = 0;
    memset((void *)puVar2[4],0,(ulonglong)*(ushort *)(puVar2 + 3) * 0x18);
    uVar5 = 0;
    uVar7 = uVar5;
    if (*(short *)(puVar2 + 3) != 0) {
      do {
        uVar6 = (int)uVar7 + 1;
        *(undefined2 *)(uVar5 + 2 + puVar2[4]) = *(undefined2 *)(lVar1 + 0x21a);
        uVar5 = uVar5 + 0x18;
        uVar7 = (ulonglong)uVar6;
      } while ((int)uVar6 < (int)(uint)*(ushort *)(puVar2 + 3));
    }
    lVar3 = param_1[1];
    if (lVar3 != 0) {
      uVar4 = FUN_14001d440(lVar1,puVar2,*(undefined8 *)(lVar3 + 0x68),*(undefined8 *)(lVar3 + 0x70)
                            ,*(undefined8 *)(lVar3 + 0x38),*(undefined8 *)(lVar3 + 0x40),0,
                            0xffffffffffffffff,&local_res8,1,param_5);
      if ((int)uVar4 != 0) {
        return uVar4;
      }
      if (((ulonglong)param_1[7] <= param_2) &&
         (uVar4 = FUN_14001d440(lVar1,puVar2,param_1[0xb],param_1[0xc],*(undefined8 *)(lVar3 + 0x38)
                                ,*(undefined8 *)(lVar3 + 0x40),param_1[7],param_2,&local_res8,0,
                                param_5), (int)uVar4 != 0)) {
        return uVar4;
      }
      if (param_3 != (undefined8 *)0x0) {
        *param_3 = puVar2;
      }
      if (param_4 != (undefined8 *)0x0) {
        *param_4 = local_res8;
      }
      return 0;
    }
  }
  return 0x1b;
}


/* ================= FUN_14001cc70 @ 14001cc70  size=253  callers=1 callees=3 ================= */
/* CALLERS: FUN_140018a20 */
/* CALLEES: FUN_14001bb80, calloc, free */

undefined4 FUN_14001cc70(longlong param_1,undefined8 param_2)

{
  ushort uVar1;
  void *_Memory;
  void *pvVar2;
  undefined4 uVar3;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_2,0x1c,0,"_dwarf_frame_interal_table_init",0x60f);
    return 0x1c;
  }
  if (*(longlong *)(param_1 + 0x228) != 0) {
    return 0;
  }
  _Memory = calloc(1,0x28);
  if (_Memory == (void *)0x0) {
    uVar3 = 0x617;
  }
  else {
    uVar1 = *(ushort *)(param_1 + 0x218);
    *(ushort *)((longlong)_Memory + 0x18) = uVar1;
    pvVar2 = calloc((ulonglong)uVar1,0x18);
    *(void **)((longlong)_Memory + 0x20) = pvVar2;
    if (pvVar2 != (void *)0x0) {
      *(void **)(param_1 + 0x228) = _Memory;
      return 0;
    }
    free(_Memory);
    uVar3 = 0x61e;
  }
  FUN_14001bb80(param_1,param_2,1,0,"_dwarf_frame_interal_table_init",uVar3);
  return 1;
}


/* ================= FUN_14001cd70 @ 14001cd70  size=38  callers=1 callees=0 ================= */
/* CALLERS: FUN_14001ebb0 */

void FUN_14001cd70(longlong param_1)

{
  if (param_1 != 0) {
    *(undefined4 *)(param_1 + 0x218) = 0x40b0203;
    *(undefined2 *)(param_1 + 0x220) = 0x40a;
    *(undefined4 *)(param_1 + 0x21c) = 0x40b059c;
  }
  return;
}


/* ================= FUN_14001cda0 @ 14001cda0  size=287  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001c010 */
/* CALLEES: FUN_14001bb80, FUN_14001d060 */

undefined8 FUN_14001cda0(longlong param_1,longlong param_2,undefined8 param_3)

{
  char cVar1;
  undefined8 uVar2;
  undefined1 *puVar3;
  char *pcVar4;
  longlong local_res8;
  undefined1 local_res20 [8];
  
  if ((param_1 == 0) || (param_2 == 0)) {
    FUN_14001bb80(param_1,param_3,0x1c,0,"_dwarf_frame_parse_lsb_cie_augment",0x13b);
    return 0x1c;
  }
  puVar3 = *(undefined1 **)(param_2 + 0x58);
  pcVar4 = (char *)(*(longlong *)(param_2 + 0x28) + 1);
  cVar1 = *pcVar4;
  do {
    if (cVar1 == '\0') {
      return 0;
    }
    if (cVar1 == 'L') {
LAB_14001cdfa:
      puVar3 = puVar3 + 1;
    }
    else {
      if (cVar1 != 'P') {
        if (cVar1 != 'R') {
          FUN_14001bb80(param_1,param_3,0x13,0,"_dwarf_frame_parse_lsb_cie_augment",0x15c);
          return 0x13;
        }
        *(undefined1 *)(param_2 + 0x60) = *puVar3;
        goto LAB_14001cdfa;
      }
      local_res8 = 0;
      uVar2 = FUN_14001d060(param_1,local_res20,puVar3 + 1,&local_res8,*puVar3,0,param_3);
      if ((int)uVar2 != 0) {
        return uVar2;
      }
      puVar3 = puVar3 + 1 + local_res8;
    }
    cVar1 = pcVar4[1];
    pcVar4 = pcVar4 + 1;
  } while( true );
}


/* ================= FUN_14001cec0 @ 14001cec0  size=403  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001f580 */
/* CALLEES: free */

void FUN_14001cec0(longlong param_1)

{
  longlong *plVar1;
  longlong *plVar2;
  void *pvVar3;
  void *pvVar4;
  longlong lVar5;
  void *pvVar6;
  void *pvVar7;
  
  plVar1 = (longlong *)(param_1 + 0x298);
  pvVar6 = (void *)*plVar1;
  while (pvVar6 != (void *)0x0) {
    pvVar3 = (void *)*plVar1;
    pvVar4 = *(void **)((longlong)pvVar6 + 0x78);
    if (pvVar3 == pvVar6) {
      lVar5 = *(longlong *)((longlong)pvVar3 + 0x78);
      *plVar1 = lVar5;
      if (lVar5 == 0) {
        *(longlong **)(param_1 + 0x2a0) = plVar1;
      }
    }
    else {
      for (pvVar7 = *(void **)((longlong)pvVar3 + 0x78); pvVar7 != pvVar6;
          pvVar7 = *(void **)((longlong)pvVar7 + 0x78)) {
        pvVar3 = pvVar7;
      }
      lVar5 = *(longlong *)(*(longlong *)((longlong)pvVar3 + 0x78) + 0x78);
      *(longlong *)((longlong)pvVar3 + 0x78) = lVar5;
      if (lVar5 == 0) {
        *(longlong **)(param_1 + 0x2a0) = (longlong *)((longlong)pvVar3 + 0x78);
      }
    }
    if (*(void **)((longlong)pvVar6 + 0x28) != (void *)0x0) {
      free(*(void **)((longlong)pvVar6 + 0x28));
    }
    if (*(void **)((longlong)pvVar6 + 0x68) != (void *)0x0) {
      free(*(void **)((longlong)pvVar6 + 0x68));
    }
    free(pvVar6);
    pvVar6 = pvVar4;
  }
  plVar1 = (longlong *)(param_1 + 0x2a8);
  *(undefined8 *)(param_1 + 0x2b8) = 0;
  pvVar6 = (void *)*plVar1;
  while (pvVar6 != (void *)0x0) {
    pvVar3 = (void *)*plVar1;
    pvVar4 = *(void **)((longlong)pvVar6 + 0x88);
    if (pvVar3 == pvVar6) {
      lVar5 = *(longlong *)((longlong)pvVar3 + 0x88);
      *plVar1 = lVar5;
      if (lVar5 == 0) {
        *(longlong **)(param_1 + 0x2b0) = plVar1;
      }
    }
    else {
      for (pvVar7 = *(void **)((longlong)pvVar3 + 0x88); pvVar7 != pvVar6;
          pvVar7 = *(void **)((longlong)pvVar7 + 0x88)) {
        pvVar3 = pvVar7;
      }
      plVar2 = (longlong *)((longlong)pvVar3 + 0x88);
      lVar5 = *(longlong *)(*plVar2 + 0x88);
      *plVar2 = lVar5;
      if (lVar5 == 0) {
        *(longlong **)(param_1 + 0x2b0) = plVar2;
      }
    }
    if (*(void **)((longlong)pvVar6 + 0x58) != (void *)0x0) {
      free(*(void **)((longlong)pvVar6 + 0x58));
    }
    free(pvVar6);
    pvVar6 = pvVar4;
  }
  *(undefined8 *)(param_1 + 0x2c0) = 0;
  return;
}


/* ================= FUN_14001d060 @ 14001d060  size=449  callers=2 callees=3 ================= */
/* CALLERS: FUN_14001c5c0, FUN_14001cda0 */
/* CALLEES: FUN_14001bb80, FUN_1400261d0, FUN_1400266f0 */

undefined8
FUN_14001d060(longlong param_1,longlong *param_2,longlong param_3,longlong param_4,byte param_5,
             longlong param_6,undefined8 param_7)

{
  short sVar1;
  int iVar2;
  longlong lVar3;
  undefined4 uVar4;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_frame_read_lsb_encoded",0xe9);
    return 0x1c;
  }
  if (((param_2 == (longlong *)0x0) || (param_3 == 0)) || (param_4 == 0)) {
    FUN_14001bb80(param_1,param_7,0x1c,0,"_dwarf_frame_read_lsb_encoded",0xed);
    return 0x1c;
  }
  if (param_5 == 0xff) {
    return 0;
  }
  switch(param_5 & 0xf) {
  case 0:
    uVar4 = *(undefined4 *)(param_1 + 0xf4);
    break;
  case 1:
    lVar3 = FUN_1400266f0(param_3,param_4);
    goto LAB_14001d184;
  case 2:
    uVar4 = 2;
    break;
  case 3:
    uVar4 = 4;
    break;
  case 4:
  case 0xc:
    uVar4 = 8;
    break;
  default:
    FUN_14001bb80(param_1,param_7,0x13,0,"_dwarf_frame_read_lsb_encoded",0x113);
    return 0x13;
  case 9:
    lVar3 = FUN_1400261d0(param_3,param_4);
    goto LAB_14001d184;
  case 10:
    sVar1 = (**(code **)(param_1 + 0x1f8))(param_3,param_4,2);
    lVar3 = (longlong)sVar1;
    goto LAB_14001d184;
  case 0xb:
    iVar2 = (**(code **)(param_1 + 0x1f8))(param_3,param_4,4);
    lVar3 = (longlong)iVar2;
    goto LAB_14001d184;
  }
  lVar3 = (**(code **)(param_1 + 0x1f8))(param_3,param_4,uVar4);
LAB_14001d184:
  *param_2 = lVar3;
  if ((param_5 & 0xf0) == 0x10) {
    switch(param_5 & 0xf) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
      *param_2 = param_6 + lVar3;
    }
  }
  return 0;
}


/* ================= FUN_14001d290 @ 14001d290  size=426  callers=1 callees=3 ================= */
/* CALLERS: FUN_14001d440 */
/* CALLEES: FUN_14001bb80, calloc, free */

undefined8 FUN_14001d290(longlong param_1,longlong *param_2,undefined8 *param_3,undefined8 param_4)

{
  undefined8 *puVar1;
  longlong lVar2;
  void *pvVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
  longlong lVar6;
  int iVar7;
  longlong lVar8;
  undefined4 uVar9;
  
  if ((param_2 == (longlong *)0x0) || (param_3 == (undefined8 *)0x0)) {
    FUN_14001bb80(param_1,param_4,0x1c,0,"_dwarf_frame_regtable_copy",0x570);
    return 0x1c;
  }
  puVar5 = (undefined8 *)*param_2;
  if (puVar5 == (undefined8 *)0x0) {
    pvVar3 = calloc(1,0x28);
    *param_2 = (longlong)pvVar3;
    if (pvVar3 == (void *)0x0) {
      uVar9 = 0x576;
    }
    else {
      *(undefined2 *)((longlong)pvVar3 + 0x18) = *(undefined2 *)(param_3 + 3);
      pvVar3 = calloc((ulonglong)*(ushort *)(param_3 + 3),0x18);
      *(void **)(*param_2 + 0x20) = pvVar3;
      puVar5 = (undefined8 *)*param_2;
      if (puVar5[4] != 0) goto LAB_14001d350;
      free(puVar5);
      uVar9 = 0x57e;
    }
    FUN_14001bb80(param_1,param_4,1,0,"_dwarf_frame_regtable_copy",uVar9);
    uVar4 = 1;
  }
  else {
LAB_14001d350:
    uVar4 = param_3[1];
    iVar7 = 0;
    *puVar5 = *param_3;
    puVar5[1] = uVar4;
    puVar5[2] = param_3[2];
    lVar6 = *param_2;
    if (*(short *)(lVar6 + 0x18) != 0) {
      lVar8 = 0;
      do {
        if ((int)(uint)*(ushort *)(param_3 + 3) <= iVar7) break;
        iVar7 = iVar7 + 1;
        lVar6 = param_3[4];
        lVar2 = *(longlong *)(*param_2 + 0x20);
        puVar5 = (undefined8 *)(lVar6 + lVar8);
        uVar4 = puVar5[1];
        puVar1 = (undefined8 *)(lVar2 + lVar8);
        *puVar1 = *puVar5;
        puVar1[1] = uVar4;
        *(undefined8 *)(lVar2 + 0x10 + lVar8) = *(undefined8 *)(lVar6 + 0x10 + lVar8);
        lVar8 = lVar8 + 0x18;
        lVar6 = *param_2;
      } while (iVar7 < (int)(uint)*(ushort *)(lVar6 + 0x18));
    }
    if (iVar7 < (int)(uint)*(ushort *)(lVar6 + 0x18)) {
      lVar6 = (longlong)iVar7 * 0x18;
      do {
        lVar6 = lVar6 + 0x18;
        iVar7 = iVar7 + 1;
        *(undefined2 *)(*(longlong *)(*param_2 + 0x20) + -0x16 + lVar6) =
             *(undefined2 *)(param_1 + 0x220);
      } while (iVar7 < (int)(uint)*(ushort *)(*param_2 + 0x18));
    }
    uVar4 = 0;
  }
  return uVar4;
}


/* ================= FUN_14001d440 @ 14001d440  size=2054  callers=1 callees=5 ================= */
/* CALLERS: FUN_14001ca80 */
/* CALLEES: FUN_14001bb80, FUN_14001d290, FUN_140025a00, FUN_140025a70, free */

undefined8
FUN_14001d440(longlong param_1,undefined2 *param_2,byte *param_3,longlong param_4,longlong param_5,
             longlong param_6,ulonglong param_7,ulonglong param_8,ulonglong *param_9,char param_10,
             undefined8 param_11)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  longlong lVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  void *pvVar6;
  byte bVar7;
  undefined2 uVar8;
  longlong lVar9;
  longlong lVar10;
  ulonglong uVar11;
  undefined8 uVar12;
  byte bVar13;
  byte *local_res8;
  undefined2 *local_res10;
  undefined4 uVar14;
  void *local_58;
  void *local_50;
  byte *local_48;
  
  local_res10 = param_2;
  if ((((param_1 == 0) || (param_2 == (undefined2 *)0x0)) || (param_3 == (byte *)0x0)) ||
     (param_9 == (ulonglong *)0x0)) {
    FUN_14001bb80(param_1,param_11,0x1c,0,"_dwarf_frame_run_inst",0x304);
    return 0x1c;
  }
  local_50 = (void *)0x0;
  local_58 = (void *)0x0;
  *param_9 = param_7;
  FUN_14001d290(param_1,&local_58,param_2,param_11);
  local_48 = param_3 + param_4;
  uVar12 = 0;
  local_res8 = param_3;
  if (param_3 < local_48) {
    do {
      bVar7 = *local_res8;
      local_res8 = local_res8 + 1;
      if (bVar7 == 0) goto LAB_14001dac9;
      bVar13 = bVar7 & 0x3f;
      bVar7 = bVar7 & 0xc0;
      if (bVar7 != 0) {
        if (bVar7 == 0x40) {
          uVar11 = (ulonglong)bVar13;
          goto LAB_14001d5d2;
        }
        if (bVar7 == 0x80) {
          *param_9 = param_7;
          if ((ushort)param_2[0xc] <= (ushort)bVar13) {
            uVar14 = 0x344;
            goto LAB_14001dafb;
          }
          lVar10 = (ulonglong)bVar13 * 0x18;
          *(undefined1 *)(lVar10 + *(longlong *)(param_2 + 0x10)) = 1;
          *(undefined1 *)(*(longlong *)(param_2 + 0x10) + 1 + lVar10) = 0;
          *(undefined2 *)(*(longlong *)(param_2 + 0x10) + 2 + lVar10) =
               *(undefined2 *)(param_1 + 0x21c);
          lVar9 = FUN_140025a70(&local_res8);
          *(longlong *)(*(longlong *)(param_2 + 0x10) + 8 + lVar10) = lVar9 * param_6;
        }
        else {
          if (bVar7 != 0xc0) {
            uVar14 = 0x358;
            goto LAB_14001db96;
          }
          *param_9 = param_7;
          if ((ushort)param_2[0xc] <= (ushort)bVar13) {
            uVar14 = 0x350;
            goto LAB_14001dafb;
          }
          lVar10 = (ulonglong)bVar13 * 0x18;
          lVar9 = *(longlong *)((longlong)local_58 + 0x20);
          lVar3 = *(longlong *)(param_2 + 0x10);
          puVar1 = (undefined4 *)(lVar9 + lVar10);
          uVar14 = puVar1[1];
          uVar4 = puVar1[2];
          uVar5 = puVar1[3];
          puVar2 = (undefined4 *)(lVar3 + lVar10);
          *puVar2 = *puVar1;
          puVar2[1] = uVar14;
          puVar2[2] = uVar4;
          puVar2[3] = uVar5;
          *(undefined8 *)(lVar3 + 0x10 + lVar10) = *(undefined8 *)(lVar9 + 0x10 + lVar10);
        }
        goto LAB_14001dac9;
      }
      switch(bVar13) {
      case 1:
        param_7 = (**(code **)(param_1 + 0x210))(&local_res8,*(undefined4 *)(param_1 + 0xf4));
        goto joined_r0x00014001d5dc;
      case 2:
        uVar11 = (**(code **)(param_1 + 0x210))(&local_res8,1);
        goto LAB_14001d5d2;
      case 3:
        uVar11 = (**(code **)(param_1 + 0x210))(&local_res8,2);
        goto LAB_14001d5d2;
      case 4:
        uVar11 = (**(code **)(param_1 + 0x210))(&local_res8,4);
LAB_14001d5d2:
        param_7 = param_7 + uVar11 * param_5;
joined_r0x00014001d5dc:
        uVar12 = 0;
        if (param_8 < param_7) goto LAB_14001dbba;
        break;
      case 5:
        *param_9 = param_7;
        uVar11 = FUN_140025a70(&local_res8);
        lVar10 = FUN_140025a70(&local_res8);
        if ((ushort)param_2[0xc] <= uVar11) {
          uVar14 = 0x385;
          goto LAB_14001dafb;
        }
        lVar9 = uVar11 * 0x18;
        *(undefined1 *)(lVar9 + *(longlong *)(param_2 + 0x10)) = 1;
        *(undefined1 *)(*(longlong *)(param_2 + 0x10) + 1 + lVar9) = 0;
LAB_14001d6b0:
        *(undefined2 *)(*(longlong *)(param_2 + 0x10) + 2 + lVar9) =
             *(undefined2 *)(param_1 + 0x21c);
        *(longlong *)(*(longlong *)(param_2 + 0x10) + 8 + lVar9) = lVar10 * param_6;
        break;
      case 6:
        *param_9 = param_7;
        uVar11 = FUN_140025a70(&local_res8);
        if ((ushort)param_2[0xc] <= uVar11) {
          uVar14 = 0x392;
          goto LAB_14001dafb;
        }
        lVar10 = uVar11 * 0x18;
        lVar9 = *(longlong *)((longlong)local_58 + 0x20);
        lVar3 = *(longlong *)(param_2 + 0x10);
        puVar1 = (undefined4 *)(lVar9 + lVar10);
        uVar14 = puVar1[1];
        uVar4 = puVar1[2];
        uVar5 = puVar1[3];
        puVar2 = (undefined4 *)(lVar3 + lVar10);
        *puVar2 = *puVar1;
        puVar2[1] = uVar14;
        puVar2[2] = uVar4;
        puVar2[3] = uVar5;
        *(undefined8 *)(lVar3 + 0x10 + lVar10) = *(undefined8 *)(lVar9 + 0x10 + lVar10);
        break;
      case 7:
        *param_9 = param_7;
        uVar11 = FUN_140025a70(&local_res8);
        if ((ushort)param_2[0xc] <= uVar11) {
          uVar14 = 0x39c;
          goto LAB_14001dafb;
        }
        lVar10 = uVar11 * 0x18;
        *(undefined1 *)(lVar10 + *(longlong *)(param_2 + 0x10)) = 0;
        if (param_10 == '\0') {
          *(undefined2 *)(*(longlong *)(param_2 + 0x10) + 2 + lVar10) =
               *(undefined2 *)(param_1 + 0x220);
        }
        else {
          *(undefined2 *)(*(longlong *)(param_2 + 0x10) + 2 + lVar10) =
               *(undefined2 *)(param_1 + 0x21e);
        }
        break;
      case 8:
        uVar11 = FUN_140025a70(&local_res8);
        if ((ushort)param_2[0xc] <= uVar11) {
          uVar14 = 0x3b3;
          goto LAB_14001dafb;
        }
        *(undefined1 *)(uVar11 * 0x18 + *(longlong *)(param_2 + 0x10)) = 0;
        *(undefined2 *)(*(longlong *)(param_2 + 0x10) + 2 + uVar11 * 0x18) =
             *(undefined2 *)(param_1 + 0x21e);
        break;
      case 9:
        *param_9 = param_7;
        uVar11 = FUN_140025a70(&local_res8);
        uVar8 = FUN_140025a70(&local_res8);
        if ((ushort)param_2[0xc] <= uVar11) {
          uVar14 = 0x3be;
          goto LAB_14001dafb;
        }
        *(undefined1 *)(uVar11 * 0x18 + *(longlong *)(param_2 + 0x10)) = 0;
        *(undefined2 *)(*(longlong *)(param_2 + 0x10) + 2 + uVar11 * 0x18) = uVar8;
        break;
      case 10:
        FUN_14001d290(param_1,&local_50,param_2,param_11);
        break;
      case 0xb:
        *param_9 = param_7;
        FUN_14001d290(param_1,&local_res10,local_50,param_11);
        param_2 = local_res10;
        break;
      case 0xc:
        *param_9 = param_7;
        uVar8 = FUN_140025a70(&local_res8);
        uVar12 = FUN_140025a70(&local_res8);
        *param_2 = 1;
        param_2[1] = uVar8;
        *(undefined8 *)(param_2 + 4) = uVar12;
        break;
      case 0xd:
        *param_9 = param_7;
        uVar8 = FUN_140025a70(&local_res8);
        param_2[1] = uVar8;
        break;
      case 0xe:
        *param_9 = param_7;
        uVar12 = FUN_140025a70(&local_res8);
        *param_2 = 1;
        *(undefined8 *)(param_2 + 4) = uVar12;
        break;
      case 0xf:
        *param_9 = param_7;
        *param_2 = 0x200;
        uVar12 = FUN_140025a70(&local_res8);
        *(undefined8 *)(param_2 + 4) = uVar12;
        *(byte **)(param_2 + 8) = local_res8;
        local_res8 = local_res8 + *(longlong *)(param_2 + 4);
        break;
      case 0x10:
        *param_9 = param_7;
        uVar11 = FUN_140025a70(&local_res8);
        if ((ushort)param_2[0xc] <= uVar11) {
          uVar14 = 0x408;
          goto LAB_14001dafb;
        }
        lVar10 = uVar11 * 0x18;
        *(undefined1 *)(lVar10 + *(longlong *)(param_2 + 0x10)) = 0;
        *(undefined1 *)(*(longlong *)(param_2 + 0x10) + 1 + lVar10) = 2;
LAB_14001da8b:
        uVar12 = FUN_140025a70(&local_res8);
        *(undefined8 *)(*(longlong *)(param_2 + 0x10) + 8 + lVar10) = uVar12;
        *(byte **)(*(longlong *)(param_2 + 0x10) + 0x10 + lVar10) = local_res8;
        local_res8 = local_res8 + *(longlong *)(*(longlong *)(param_2 + 0x10) + 8 + lVar10);
        break;
      case 0x11:
        *param_9 = param_7;
        uVar11 = FUN_140025a70(&local_res8);
        lVar10 = FUN_140025a00(&local_res8);
        if ((ushort)param_2[0xc] <= uVar11) {
          uVar14 = 0x417;
          goto LAB_14001dafb;
        }
        lVar9 = uVar11 * 0x18;
        *(undefined1 *)(lVar9 + *(longlong *)(param_2 + 0x10)) = 1;
        *(undefined1 *)(*(longlong *)(param_2 + 0x10) + 1 + lVar9) = 0;
        *(undefined2 *)(*(longlong *)(param_2 + 0x10) + 2 + lVar9) =
             *(undefined2 *)(param_1 + 0x21c);
        *(longlong *)(*(longlong *)(param_2 + 0x10) + 8 + lVar9) = lVar10 * param_6;
        break;
      case 0x12:
        *param_9 = param_7;
        uVar8 = FUN_140025a70(&local_res8);
        lVar10 = FUN_140025a00(&local_res8);
        *param_2 = 1;
        param_2[1] = uVar8;
        *(longlong *)(param_2 + 4) = lVar10 * param_6;
        break;
      case 0x13:
        *param_9 = param_7;
        lVar10 = FUN_140025a00(&local_res8);
        *param_2 = 1;
        *(longlong *)(param_2 + 4) = lVar10 * param_6;
        break;
      case 0x14:
        *param_9 = param_7;
        uVar11 = FUN_140025a70(&local_res8);
        lVar10 = FUN_140025a70(&local_res8);
        if (uVar11 < (ushort)param_2[0xc]) {
          lVar9 = uVar11 * 0x18;
          *(undefined1 *)(lVar9 + *(longlong *)(param_2 + 0x10)) = 1;
          *(undefined1 *)(*(longlong *)(param_2 + 0x10) + 1 + lVar9) = 1;
          goto LAB_14001d6b0;
        }
        uVar14 = 0x43c;
        goto LAB_14001dafb;
      case 0x15:
        *param_9 = param_7;
        uVar11 = FUN_140025a70(&local_res8);
        lVar10 = FUN_140025a00(&local_res8);
        if (uVar11 < (ushort)param_2[0xc]) {
          lVar9 = uVar11 * 0x18;
          *(undefined1 *)(lVar9 + *(longlong *)(param_2 + 0x10)) = 1;
          *(undefined1 *)(*(longlong *)(param_2 + 0x10) + 1 + lVar9) = 1;
          goto LAB_14001d6b0;
        }
        uVar14 = 0x44a;
        goto LAB_14001dafb;
      case 0x16:
        *param_9 = param_7;
        uVar11 = FUN_140025a70(&local_res8);
        if (uVar11 < (ushort)param_2[0xc]) {
          lVar10 = uVar11 * 0x18;
          *(undefined1 *)(lVar10 + *(longlong *)(param_2 + 0x10)) = 0;
          *(undefined1 *)(*(longlong *)(param_2 + 0x10) + 1 + lVar10) = 3;
          goto LAB_14001da8b;
        }
        uVar14 = 0x457;
LAB_14001dafb:
        FUN_14001bb80(param_1,param_11,0x1c,0,"_dwarf_frame_run_inst",uVar14);
        uVar12 = 0x1c;
        goto LAB_14001dbba;
      default:
        uVar14 = 0x466;
LAB_14001db96:
        FUN_14001bb80(param_1,param_11,0x14,0,"_dwarf_frame_run_inst",uVar14);
        uVar12 = 0x14;
        goto LAB_14001dbba;
      }
LAB_14001dac9:
      uVar12 = 0;
    } while (local_res8 < local_48);
  }
LAB_14001dbba:
  pvVar6 = local_58;
  free(*(void **)((longlong)local_58 + 0x20));
  free(pvVar6);
  pvVar6 = local_50;
  if (local_50 != (void *)0x0) {
    free(*(void **)((longlong)local_50 + 0x20));
    free(pvVar6);
  }
  return uVar12;
}


/* ================= FUN_14001dca0 @ 14001dca0  size=324  callers=2 callees=1 ================= */
/* CALLERS: FUN_14001c9a0, FUN_14001ddf0 */
/* CALLEES: free */

void FUN_14001dca0(longlong *param_1)

{
  longlong *plVar1;
  longlong *plVar2;
  void *pvVar3;
  void *pvVar4;
  longlong lVar5;
  void *pvVar6;
  void *pvVar7;
  
  if (param_1 != (longlong *)0x0) {
    pvVar6 = (void *)*param_1;
    while (pvVar6 != (void *)0x0) {
      pvVar3 = (void *)*param_1;
      pvVar4 = *(void **)((longlong)pvVar6 + 0x78);
      if (pvVar3 == pvVar6) {
        lVar5 = *(longlong *)((longlong)pvVar3 + 0x78);
        *param_1 = lVar5;
        if (lVar5 == 0) {
          param_1[1] = (longlong)param_1;
        }
      }
      else {
        for (pvVar7 = *(void **)((longlong)pvVar3 + 0x78); pvVar7 != pvVar6;
            pvVar7 = *(void **)((longlong)pvVar7 + 0x78)) {
          pvVar3 = pvVar7;
        }
        lVar5 = *(longlong *)(*(longlong *)((longlong)pvVar3 + 0x78) + 0x78);
        *(longlong *)((longlong)pvVar3 + 0x78) = lVar5;
        if (lVar5 == 0) {
          param_1[1] = (longlong)pvVar3 + 0x78;
        }
      }
      free(pvVar6);
      pvVar6 = pvVar4;
    }
    plVar1 = param_1 + 2;
    pvVar6 = (void *)param_1[2];
    while (pvVar6 != (void *)0x0) {
      pvVar3 = (void *)*plVar1;
      pvVar4 = *(void **)((longlong)pvVar6 + 0x88);
      if (pvVar3 == pvVar6) {
        lVar5 = *(longlong *)((longlong)pvVar3 + 0x88);
        *plVar1 = lVar5;
        if (lVar5 == 0) {
          param_1[3] = (longlong)plVar1;
        }
      }
      else {
        for (pvVar7 = *(void **)((longlong)pvVar3 + 0x88); pvVar7 != pvVar6;
            pvVar7 = *(void **)((longlong)pvVar7 + 0x88)) {
          pvVar3 = pvVar7;
        }
        plVar2 = (longlong *)((longlong)pvVar3 + 0x88);
        lVar5 = *(longlong *)(*plVar2 + 0x88);
        *plVar2 = lVar5;
        if (lVar5 == 0) {
          param_1[3] = (longlong)plVar2;
        }
      }
      free(pvVar6);
      pvVar6 = pvVar4;
    }
    if ((void *)param_1[4] != (void *)0x0) {
      free((void *)param_1[4]);
    }
    if ((void *)param_1[6] != (void *)0x0) {
      free((void *)param_1[6]);
    }
    free(param_1);
  }
  return;
}


/* ================= FUN_14001ddf0 @ 14001ddf0  size=894  callers=1 callees=5 ================= */
/* CALLERS: FUN_14001e170 */
/* CALLEES: FUN_14001bb80, FUN_14001c200, FUN_14001c5c0, FUN_14001dca0, calloc */

int FUN_14001ddf0(longlong param_1,undefined8 *param_2,longlong param_3,int param_4,
                 undefined8 param_5)

{
  longlong *plVar1;
  longlong lVar2;
  int iVar3;
  longlong *plVar4;
  ulonglong uVar5;
  longlong lVar6;
  void *pvVar7;
  ulonglong local_res8;
  undefined8 *local_res10;
  longlong in_stack_ffffffffffffffa8;
  uint uVar8;
  undefined8 in_stack_ffffffffffffffb0;
  undefined4 uVar10;
  undefined8 uVar9;
  ulonglong local_48 [2];
  
  local_res10 = param_2;
  if (((param_1 == 0) || (param_2 == (undefined8 *)0x0)) || (param_3 == 0)) {
    FUN_14001bb80(param_1,param_5,0x1c,0,"_dwarf_frame_section_init",0x29b);
    return 0x1c;
  }
  plVar4 = calloc(1,0x40);
  if (plVar4 == (longlong *)0x0) {
    FUN_14001bb80(param_1,param_5,1,0,"_dwarf_frame_section_init",0x2a2);
    return 1;
  }
  *plVar4 = 0;
  plVar4[1] = (longlong)plVar4;
  plVar1 = plVar4 + 2;
  plVar4[3] = (longlong)plVar1;
  *plVar1 = 0;
  local_48[0] = 0;
  if (*(longlong *)(param_3 + 0x18) != 0) {
    do {
      uVar8 = (uint)((ulonglong)in_stack_ffffffffffffffa8 >> 0x20);
      uVar10 = (undefined4)((ulonglong)in_stack_ffffffffffffffb0 >> 0x20);
      local_res8 = local_48[0];
      uVar5 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),local_48,4);
      if (uVar5 == 0xffffffff) {
        iVar3 = 8;
        uVar5 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),local_48,8);
      }
      else {
        iVar3 = 4;
      }
      if (*(longlong *)(param_3 + 0x18) - local_48[0] < uVar5) {
LAB_14001e02f:
        FUN_14001bb80(param_1,param_5,0xf,0,"_dwarf_frame_section_init",CONCAT44(uVar10,0x2b3));
        return 0xf;
      }
      if (uVar5 == 0) {
        param_2 = local_res10;
        if (param_4 == 0) goto LAB_14001e02f;
        break;
      }
      lVar6 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),local_48,iVar3);
      in_stack_ffffffffffffffb0 = param_5;
      if (param_4 == 0) {
        if (iVar3 == 4) {
          if (lVar6 == 0xffffffff) {
LAB_14001e005:
            in_stack_ffffffffffffffa8 = 0;
            iVar3 = FUN_14001c200(param_1,plVar4,param_3,&local_res8,0,param_5);
            goto LAB_14001df9b;
          }
        }
        else if ((iVar3 == 8) && (lVar6 == -1)) goto LAB_14001e005;
        in_stack_ffffffffffffffa8 = (ulonglong)uVar8 << 0x20;
LAB_14001df96:
        iVar3 = FUN_14001c5c0(param_1,plVar4,param_3,&local_res8,in_stack_ffffffffffffffa8,param_5);
      }
      else {
        if (lVar6 != 0) {
          in_stack_ffffffffffffffa8 = CONCAT44(uVar8,1);
          goto LAB_14001df96;
        }
        in_stack_ffffffffffffffa8 = 0;
        iVar3 = FUN_14001c200();
      }
LAB_14001df9b:
      if (iVar3 != 0) goto LAB_14001e0ed;
      local_48[0] = local_res8;
      param_2 = local_res10;
    } while (local_res8 < *(ulonglong *)(param_3 + 0x18));
  }
  if (plVar4[5] == 0) {
LAB_14001e0a6:
    if (plVar4[7] != 0) {
      pvVar7 = calloc(plVar4[7],8);
      plVar4[6] = (longlong)pvVar7;
      if (pvVar7 == (void *)0x0) {
        uVar9 = 0x2e5;
        goto LAB_14001e0cb;
      }
      lVar2 = 0;
      for (lVar6 = *plVar1; lVar6 != 0; lVar6 = *(longlong *)(lVar6 + 0x88)) {
        *(longlong *)(lVar2 + plVar4[6]) = lVar6;
        lVar2 = lVar2 + 8;
      }
    }
    *param_2 = plVar4;
    iVar3 = 0;
  }
  else {
    pvVar7 = calloc(plVar4[5],8);
    plVar4[4] = (longlong)pvVar7;
    if (pvVar7 != (void *)0x0) {
      lVar2 = 0;
      for (lVar6 = *plVar4; lVar6 != 0; lVar6 = *(longlong *)(lVar6 + 0x78)) {
        *(longlong *)(lVar2 + plVar4[4]) = lVar6;
        lVar2 = lVar2 + 8;
      }
      goto LAB_14001e0a6;
    }
    uVar9 = 0x2d7;
LAB_14001e0cb:
    iVar3 = 1;
    FUN_14001bb80(param_1,param_5,1,0,"_dwarf_frame_section_init",uVar9);
LAB_14001e0ed:
    FUN_14001dca0(plVar4);
  }
  return iVar3;
}


/* ================= FUN_14001e170 @ 14001e170  size=130  callers=1 callees=3 ================= */
/* CALLERS: FUN_140018a20 */
/* CALLEES: FUN_14001bb80, FUN_14001ddf0, FUN_140026bb0 */

undefined8 FUN_14001e170(longlong param_1,undefined8 param_2)

{
  longlong lVar1;
  undefined8 uVar2;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_2,0x1c,0,"_dwarf_frame_section_load",0x5e6);
    return 0x1c;
  }
  lVar1 = FUN_140026bb0(param_1,5);
  if (lVar1 != 0) {
    uVar2 = FUN_14001ddf0(param_1,param_1 + 400,lVar1,0,param_2);
    return uVar2;
  }
  return 0;
}


/* ================= FUN_14001e200 @ 14001e200  size=437  callers=1 callees=4 ================= */
/* CALLERS: FUN_14001eb00 */
/* CALLEES: FUN_140019e30, FUN_14001bb80, FUN_1400200d0, free */

void FUN_14001e200(longlong param_1)

{
  longlong *plVar1;
  longlong *plVar2;
  void *pvVar3;
  void *pvVar4;
  longlong lVar5;
  void *pvVar6;
  void *pvVar7;
  
  if (param_1 != 0) {
    plVar1 = (longlong *)(param_1 + 0x120);
    pvVar6 = (void *)*plVar1;
    while (pvVar6 != (void *)0x0) {
      pvVar3 = (void *)*plVar1;
      pvVar4 = *(void **)((longlong)pvVar6 + 0x90);
      if (pvVar3 == pvVar6) {
        lVar5 = *(longlong *)((longlong)pvVar3 + 0x90);
        *plVar1 = lVar5;
        if (lVar5 == 0) {
          *(longlong **)(param_1 + 0x128) = plVar1;
        }
      }
      else {
        for (pvVar7 = *(void **)((longlong)pvVar3 + 0x90); pvVar7 != pvVar6;
            pvVar7 = *(void **)((longlong)pvVar7 + 0x90)) {
          pvVar3 = pvVar7;
        }
        plVar2 = (longlong *)((longlong)pvVar3 + 0x90);
        lVar5 = *(longlong *)(*plVar2 + 0x90);
        *plVar2 = lVar5;
        if (lVar5 == 0) {
          *(longlong **)(param_1 + 0x128) = plVar2;
        }
      }
      if (*(longlong *)((longlong)pvVar6 + 0x78) != 0) {
        FUN_1400200d0();
        *(undefined8 *)((longlong)pvVar6 + 0x78) = 0;
      }
      free(pvVar6);
      pvVar6 = pvVar4;
    }
    plVar1 = (longlong *)(param_1 + 0x130);
    pvVar6 = (void *)*plVar1;
    while (pvVar6 != (void *)0x0) {
      pvVar3 = (void *)*plVar1;
      pvVar4 = *(void **)((longlong)pvVar6 + 0x90);
      if (pvVar3 == pvVar6) {
        lVar5 = *(longlong *)((longlong)pvVar3 + 0x90);
        *plVar1 = lVar5;
        if (lVar5 == 0) {
          *(longlong **)(param_1 + 0x138) = plVar1;
        }
      }
      else {
        for (pvVar7 = *(void **)((longlong)pvVar3 + 0x90); pvVar7 != pvVar6;
            pvVar7 = *(void **)((longlong)pvVar7 + 0x90)) {
          pvVar3 = pvVar7;
        }
        plVar2 = (longlong *)((longlong)pvVar3 + 0x90);
        lVar5 = *(longlong *)(*plVar2 + 0x90);
        *plVar2 = lVar5;
        if (lVar5 == 0) {
          *(longlong **)(param_1 + 0x138) = plVar2;
        }
      }
      FUN_140019e30(pvVar6);
      free(pvVar6);
      pvVar6 = pvVar4;
    }
    FUN_140019e30();
    return;
  }
  FUN_14001bb80(0,0,0x1c,0,"_dwarf_info_cleanup",0x174);
  return;
}


/* ================= FUN_14001e3c0 @ 14001e3c0  size=149  callers=1 callees=2 ================= */
/* CALLERS: FUN_140015e30 */
/* CALLEES: FUN_14001bb80, FUN_14001e500 */

undefined8 FUN_14001e3c0(longlong param_1,undefined8 param_2)

{
  undefined8 uVar1;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_2,0x1c,0,"_dwarf_info_first_cu",0x25);
    return 0x1c;
  }
  if (*(longlong *)(param_1 + 0x120) != 0) {
    *(longlong *)(param_1 + 0x140) = *(longlong *)(param_1 + 0x120);
    return 0;
  }
  if (*(int *)(param_1 + 0xfc) != 0) {
    return 0x23;
  }
  *(undefined4 *)(param_1 + 0x18) = 0;
  uVar1 = FUN_14001e500(param_1,0,1,param_2);
  if ((int)uVar1 == 0) {
    *(undefined8 *)(param_1 + 0x140) = *(undefined8 *)(param_1 + 0x120);
    uVar1 = 0;
  }
  return uVar1;
}


/* ================= FUN_14001e460 @ 14001e460  size=148  callers=1 callees=2 ================= */
/* CALLERS: FUN_140015e30 */
/* CALLEES: FUN_14001bb80, FUN_14001e500 */

undefined8 FUN_14001e460(longlong param_1,undefined8 param_2)

{
  undefined8 uVar1;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_2,0x1c,0,"_dwarf_info_first_tu",0x45);
    return 0x1c;
  }
  if (*(longlong *)(param_1 + 0x130) != 0) {
    *(longlong *)(param_1 + 0x148) = *(longlong *)(param_1 + 0x130);
    return 0;
  }
  if (*(int *)(param_1 + 0x100) != 0) {
    return 0x23;
  }
  *(undefined4 *)(param_1 + 0x28) = 0;
  uVar1 = FUN_14001e500(param_1,0,0,param_2);
  if ((int)uVar1 == 0) {
    *(undefined8 *)(param_1 + 0x148) = *(undefined8 *)(param_1 + 0x130);
    uVar1 = 0;
  }
  return uVar1;
}


/* ================= FUN_14001e500 @ 14001e500  size=988  callers=6 callees=5 ================= */
/* CALLERS: FUN_1400163c0, FUN_140019870, FUN_14001e3c0, FUN_14001e460, FUN_14001e8e0, FUN_14001e980 */
/* CALLEES: FUN_14001baf0, FUN_14001bb80, FUN_14001bd50, calloc, free */

undefined4 FUN_14001e500(longlong param_1,int param_2,int param_3,undefined8 param_4)

{
  longlong *plVar1;
  int iVar2;
  undefined1 uVar3;
  undefined2 uVar4;
  ushort uVar5;
  undefined4 uVar6;
  longlong *_Memory;
  ulonglong uVar7;
  longlong lVar8;
  undefined8 uVar9;
  longlong lVar10;
  undefined4 uVar11;
  ulonglong uVar12;
  char cVar13;
  ulonglong local_res8;
  int local_res10;
  
  local_res10 = param_2;
  if (param_1 == 0) {
    FUN_14001bb80(0,param_4,0x1c,0,"_dwarf_info_load",0xad);
    return 0x1c;
  }
  uVar6 = 0;
  if (param_3 == 0) {
    if (*(int *)(param_1 + 0x100) != 0) {
      return 0;
    }
    iVar2 = *(int *)(param_1 + 0x28);
    lVar10 = *(longlong *)(param_1 + 0x20);
  }
  else {
    if (*(int *)(param_1 + 0xfc) != 0) {
      return 0;
    }
    iVar2 = *(int *)(param_1 + 0x18);
    lVar10 = *(longlong *)(param_1 + 0x10);
  }
  local_res8 = (ulonglong)iVar2;
  if (lVar10 == 0) {
    uVar6 = 0x23;
  }
  else {
    if (local_res8 < *(ulonglong *)(lVar10 + 0x18)) {
      while( true ) {
        _Memory = calloc(1,0x98);
        if (_Memory == (longlong *)0x0) {
          FUN_14001bb80(param_1,param_4,1,0,"_dwarf_info_load",0xcf);
          return 1;
        }
        *_Memory = param_1;
        *(int *)(_Memory + 0x11) = param_3;
        cVar13 = '\x04';
        *(undefined4 *)(_Memory + 1) = (undefined4)local_res8;
        uVar7 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res8,4);
        if (uVar7 == 0xffffffff) {
          uVar7 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res8,8);
          cVar13 = '\b';
        }
        if (*(longlong *)(lVar10 + 0x18) - local_res8 < uVar7) {
          free(_Memory);
          FUN_14001bb80(param_1,param_4,8,0,"_dwarf_info_load",0xea);
          return 8;
        }
        *(char *)((longlong)_Memory + 0x49) = cVar13;
        uVar4 = 0xc;
        uVar12 = local_res8 + uVar7;
        *(int *)((longlong)_Memory + 0xc) = (int)uVar7;
        uVar11 = (undefined4)uVar12;
        *(undefined4 *)(_Memory + 0xc) = uVar11;
        if (cVar13 == '\x04') {
          uVar4 = 4;
        }
        *(undefined2 *)(_Memory + 2) = uVar4;
        uVar5 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res8,2);
        *(ushort *)((longlong)_Memory + 0x12) = uVar5;
        if (3 < (ushort)(uVar5 - 2)) break;
        if (uVar5 < 5) {
          lVar8 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res8,cVar13);
          _Memory[4] = lVar8;
          _Memory[5] = lVar8;
          uVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res8,1);
          *(undefined1 *)(_Memory + 9) = uVar3;
          if (param_3 == 0) {
LAB_14001e71c:
            *(undefined8 *)((longlong)_Memory + 0x4a) =
                 *(undefined8 *)(local_res8 + *(longlong *)(lVar10 + 8));
            local_res8 = local_res8 + 8;
            lVar8 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res8,cVar13);
            _Memory[0xb] = lVar8;
          }
        }
        else {
          uVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res8,1);
          *(undefined1 *)((longlong)_Memory + 0x14) = uVar3;
          uVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res8,1);
          *(undefined1 *)(_Memory + 9) = uVar3;
          lVar8 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res8,cVar13);
          _Memory[4] = lVar8;
          _Memory[5] = lVar8;
          if (*(char *)((longlong)_Memory + 0x14) == '\x02') goto LAB_14001e71c;
          if ((byte)(*(char *)((longlong)_Memory + 0x14) - 4U) < 3) {
            lVar8 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res8,8);
            _Memory[3] = lVar8;
          }
        }
        plVar1 = _Memory + 0x12;
        *plVar1 = 0;
        if (param_3 == 0) {
          **(undefined8 **)(param_1 + 0x138) = _Memory;
          *(longlong **)(param_1 + 0x138) = plVar1;
          _Memory[0xd] = local_res8;
          *(undefined4 *)(param_1 + 0x28) = uVar11;
        }
        else {
          **(undefined8 **)(param_1 + 0x128) = _Memory;
          *(longlong **)(param_1 + 0x128) = plVar1;
          _Memory[0xd] = local_res8;
          *(undefined4 *)(param_1 + 0x18) = uVar11;
        }
        if ((local_res10 == 0) || (local_res8 = uVar12, *(ulonglong *)(lVar10 + 0x18) <= uVar12))
        goto LAB_14001e7d1;
      }
      uVar9 = FUN_14001baf0((longlong)(int)_Memory[1]);
      FUN_14001bd50(param_1,param_4,9,0,"_dwarf_info_load",0x101,uVar9,0,0);
      uVar6 = 9;
    }
LAB_14001e7d1:
    if (param_3 == 0) {
      if (*(ulonglong *)(lVar10 + 0x18) <= (ulonglong)(longlong)*(int *)(param_1 + 0x28)) {
        *(undefined4 *)(param_1 + 0x100) = 1;
      }
    }
    else if (*(ulonglong *)(lVar10 + 0x18) <= (ulonglong)(longlong)*(int *)(param_1 + 0x18)) {
      *(undefined4 *)(param_1 + 0xfc) = 1;
    }
  }
  return uVar6;
}


/* ================= FUN_14001e8e0 @ 14001e8e0  size=148  callers=1 callees=2 ================= */
/* CALLERS: FUN_140015e30 */
/* CALLEES: FUN_14001bb80, FUN_14001e500 */

undefined8 FUN_14001e8e0(longlong param_1,undefined8 param_2)

{
  undefined8 uVar1;
  longlong lVar2;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_2,0x1c,0,"_dwarf_info_next_cu",0x65);
    return 0x1c;
  }
  lVar2 = *(longlong *)(*(longlong *)(param_1 + 0x140) + 0x90);
  if (lVar2 == 0) {
    if (*(int *)(param_1 + 0xfc) != 0) {
      *(undefined8 *)(param_1 + 0x140) = 0;
      return 0x23;
    }
    uVar1 = FUN_14001e500(param_1,0,1,param_2);
    if ((int)uVar1 != 0) {
      return uVar1;
    }
    lVar2 = *(longlong *)(*(longlong *)(param_1 + 0x140) + 0x90);
  }
  *(longlong *)(param_1 + 0x140) = lVar2;
  return 0;
}


/* ================= FUN_14001e980 @ 14001e980  size=147  callers=1 callees=2 ================= */
/* CALLERS: FUN_140015e30 */
/* CALLEES: FUN_14001bb80, FUN_14001e500 */

undefined8 FUN_14001e980(longlong param_1,undefined8 param_2)

{
  undefined8 uVar1;
  longlong lVar2;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_2,0x1c,0,"_dwarf_info_next_tu",0x85);
    return 0x1c;
  }
  lVar2 = *(longlong *)(*(longlong *)(param_1 + 0x148) + 0x90);
  if (lVar2 == 0) {
    if (*(int *)(param_1 + 0x100) != 0) {
      *(undefined8 *)(param_1 + 0x148) = 0;
      return 0x23;
    }
    uVar1 = FUN_14001e500(param_1,0,0,param_2);
    if ((int)uVar1 != 0) {
      return uVar1;
    }
    lVar2 = *(longlong *)(*(longlong *)(param_1 + 0x148) + 0x90);
  }
  *(longlong *)(param_1 + 0x148) = lVar2;
  return 0;
}


/* ================= FUN_14001ea20 @ 14001ea20  size=75  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001f580 */
/* CALLEES: FUN_140019e30 */

void FUN_14001ea20(longlong param_1)

{
  longlong *plVar1;
  void *_Memory;
  longlong lVar2;
  
  if (param_1 != 0) {
    plVar1 = (longlong *)(param_1 + 0x120);
    _Memory = (void *)*plVar1;
    lVar2 = *(longlong *)((longlong)_Memory + 0x90);
    *plVar1 = lVar2;
    if (lVar2 == 0) {
      *(longlong **)(param_1 + 0x128) = plVar1;
    }
    FUN_140019e30();
                    /* WARNING: Could not recover jumptable at 0x00014001ea5f. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    free(_Memory);
    return;
  }
  return;
}


/* ================= FUN_14001ea70 @ 14001ea70  size=129  callers=1 callees=2 ================= */
/* CALLERS: FUN_140018bd0 */
/* CALLEES: FUN_14001bb80, calloc */

undefined8 FUN_14001ea70(undefined8 *param_1,undefined4 param_2,undefined8 param_3)

{
  void *pvVar1;
  
  pvVar1 = calloc(0x340,1);
  if (pvVar1 == (void *)0x0) {
    FUN_14001bb80(0,param_3,1,0,"_dwarf_alloc",0x187);
    return 1;
  }
  *(undefined4 *)((longlong)pvVar1 + 0xf0) = param_2;
  *param_1 = pvVar1;
  return 0;
}


/* ================= FUN_14001eb00 @ 14001eb00  size=166  callers=0 callees=10 ================= */
/* CALLEES: FUN_14001a2c0, FUN_14001c9a0, FUN_14001e200, FUN_140022ff0, FUN_1400238a0, FUN_140023ea0, FUN_140024950, FUN_140024e30, FUN_140026f40, free */

void FUN_14001eb00(longlong param_1)

{
  if (param_1 != 0) {
    FUN_14001e200();
    FUN_140022ff0(param_1);
    FUN_1400238a0(param_1);
    FUN_140024e30(param_1);
    FUN_14001c9a0(param_1);
    FUN_14001a2c0(param_1);
    FUN_140023ea0(param_1);
    FUN_140026f40(param_1);
    FUN_140024950(param_1 + 0x160);
    FUN_140024950(param_1 + 0x168);
    FUN_140024950(param_1 + 0x170);
    FUN_140024950(param_1 + 0x178);
    FUN_140024950(param_1 + 0x180);
    FUN_140024950(param_1 + 0x188);
    free(*(void **)(param_1 + 8));
  }
  return;
}


/* ================= FUN_14001ebb0 @ 14001ebb0  size=2154  callers=1 callees=8 ================= */
/* CALLERS: FUN_14001f440 */
/* CALLEES: FUN_14001bb80, FUN_14001bd50, FUN_14001cd70, FUN_140026bb0, calloc, memset, strcmp, strncmp */

undefined4 FUN_14001ebb0(longlong *param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  char *_Str1;
  byte bVar4;
  int iVar5;
  undefined1 *puVar6;
  ulonglong _Count;
  void *pvVar7;
  longlong lVar8;
  code *pcVar9;
  undefined1 *puVar10;
  ulonglong uVar11;
  undefined4 local_res8 [2];
  longlong local_res18;
  undefined4 uVar12;
  undefined8 local_58;
  undefined8 local_50;
  char *local_48;
  
  if (param_1 == (longlong *)0x0) {
    uVar12 = 0x28;
  }
  else {
    puVar1 = (undefined8 *)*param_1;
    if (puVar1 != (undefined8 *)0x0) {
      puVar2 = (undefined8 *)puVar1[1];
      if (puVar2 == (undefined8 *)0x0) {
        FUN_14001bb80(param_1,param_2,0x1c,0,"_dwarf_consumer_init",0x30);
        return 0x1c;
      }
      uVar3 = *puVar1;
      iVar5 = (*(code *)puVar2[1])(uVar3);
      if (iVar5 == 0) {
        puVar6 = &LAB_1400258f0;
        pcVar9 = FUN_140026a20;
        puVar10 = &LAB_140026030;
      }
      else {
        puVar6 = &LAB_140025840;
        pcVar9 = FUN_140026890;
        puVar10 = &LAB_140025f70;
      }
      param_1[0x3f] = (longlong)puVar10;
      param_1[0x40] = (longlong)pcVar9;
      param_1[0x42] = (longlong)puVar6;
      bVar4 = (*(code *)puVar2[3])(uVar3);
      *(uint *)((longlong)param_1 + 0xf4) = (uint)bVar4;
      bVar4 = (*(code *)puVar2[2])(uVar3);
      *(uint *)(param_1 + 0x1f) = (uint)bVar4;
      _Count = (*(code *)puVar2[4])(uVar3);
      if (_Count == 0) {
        FUN_14001bb80(param_1,param_2,7,0,"_dwarf_consumer_init",0x47);
        uVar12 = 7;
      }
      else {
        param_1[6] = _Count;
        pvVar7 = calloc(_Count,0x20);
        param_1[1] = (longlong)pvVar7;
        if (pvVar7 == (void *)0x0) {
          FUN_14001bb80(param_1,param_2,1,0,"_dwarf_consumer_init",0x4f);
          uVar12 = 1;
        }
        else {
          memset(param_1 + 7,0xff,0xb8);
          uVar11 = 0;
          if (_Count != 0) {
            local_res18 = 0;
            do {
              iVar5 = (*(code *)*puVar2)(uVar3,uVar11 & 0xffff,&local_58,local_res8);
              if (iVar5 != 0) {
                uVar12 = 0x5a;
LAB_14001f37a:
                FUN_14001bb80(param_1,param_2,local_res8[0],0,"_dwarf_consumer_init",uVar12);
                return local_res8[0];
              }
              *(undefined8 *)(local_res18 + 0x10 + param_1[1]) = local_58;
              *(undefined8 *)(local_res18 + 0x18 + param_1[1]) = local_50;
              *(char **)(local_res18 + param_1[1]) = local_48;
              iVar5 = (*(code *)puVar2[5])
                                (uVar3,uVar11 & 0xffff,param_1[1] + 8 + local_res18,local_res8);
              if (iVar5 != 0) {
                uVar12 = 100;
                goto LAB_14001f37a;
              }
              iVar5 = strncmp(local_48,".debug",6);
              _Str1 = local_48;
              if (iVar5 == 0) {
                iVar5 = strcmp(local_48,".debug_info");
                if (iVar5 == 0) {
                  param_1[7] = uVar11;
                }
                else {
                  iVar5 = strcmp(_Str1,".debug_types");
                  if (iVar5 == 0) {
                    param_1[8] = uVar11;
                  }
                  else {
                    iVar5 = strcmp(_Str1,".debug_loc");
                    if (iVar5 == 0) {
                      param_1[9] = uVar11;
                    }
                    else {
                      iVar5 = strcmp(_Str1,".debug_str");
                      if (iVar5 == 0) {
                        param_1[10] = uVar11;
                      }
                      else {
                        iVar5 = strcmp(_Str1,".debug_abbrev");
                        if (iVar5 == 0) {
                          param_1[0xb] = uVar11;
                        }
                        else {
                          iVar5 = strcmp(_Str1,".debug_frame");
                          if (iVar5 == 0) {
                            param_1[0xc] = uVar11;
                          }
                          else {
                            iVar5 = strcmp(_Str1,".debug_line");
                            if (iVar5 == 0) {
                              param_1[0xd] = uVar11;
                            }
                            else {
                              iVar5 = strcmp(_Str1,".debug_macinfo");
                              if (iVar5 == 0) {
                                param_1[0xe] = uVar11;
                              }
                              else {
                                iVar5 = strcmp(_Str1,".debug_macro");
                                if (iVar5 == 0) {
                                  param_1[0xf] = uVar11;
                                }
                                else {
                                  iVar5 = strcmp(_Str1,".debug_ranges");
                                  if (iVar5 == 0) {
                                    param_1[0x10] = uVar11;
                                  }
                                  else {
                                    iVar5 = strcmp(_Str1,".debug_aranges");
                                    if (iVar5 == 0) {
                                      param_1[0x11] = uVar11;
                                    }
                                    else {
                                      iVar5 = strcmp(_Str1,".debug_loclists");
                                      if (iVar5 == 0) {
                                        param_1[0x18] = uVar11;
                                      }
                                      else {
                                        iVar5 = strcmp(_Str1,".debug_rnglists");
                                        if (iVar5 == 0) {
                                          param_1[0x19] = uVar11;
                                        }
                                        else {
                                          iVar5 = strcmp(_Str1,".debug_line_str");
                                          if (iVar5 == 0) {
                                            param_1[0x1a] = uVar11;
                                          }
                                          else {
                                            iVar5 = strcmp(_Str1,".debug_names");
                                            if (iVar5 == 0) {
                                              param_1[0x1b] = uVar11;
                                            }
                                            else {
                                              iVar5 = strcmp(_Str1,".debug_addr");
                                              if (iVar5 == 0) {
                                                param_1[0x1c] = uVar11;
                                              }
                                              else {
                                                iVar5 = strcmp(_Str1,".debug_str_offsets");
                                                if (iVar5 == 0) {
                                                  param_1[0x1d] = uVar11;
                                                }
                                                else {
                                                  iVar5 = strcmp(_Str1,".debug_abbrev.dwo");
                                                  if (iVar5 == 0) {
                                                    FUN_14001bd50(param_1,param_2,0x29,0,
                                                                  "_dwarf_consumer_init",0x8c,
                                                                  ".debug_abbrev.dwo",0,0);
                                                  }
                                                  else {
                                                    iVar5 = strcmp(_Str1,".debug_info.dwo");
                                                    if (iVar5 == 0) {
                                                      FUN_14001bd50(param_1,param_2,0x29,0,
                                                                    "_dwarf_consumer_init",0x8e,
                                                                    ".debug_info.dwo",0,0);
                                                    }
                                                    else {
                                                      iVar5 = strcmp(_Str1,".debug_line.dwo");
                                                      if (iVar5 == 0) {
                                                        FUN_14001bd50(param_1,param_2,0x29,0,
                                                                      "_dwarf_consumer_init",0x90,
                                                                      ".debug_line.dwo",0,0);
                                                      }
                                                      else {
                                                        iVar5 = strcmp(_Str1,".debug_loclists.dwo");
                                                        if (iVar5 == 0) {
                                                          FUN_14001bd50(param_1,param_2,0x29,0,
                                                                        "_dwarf_consumer_init",0x92,
                                                                        ".debug_loclists.dwo",0,0);
                                                        }
                                                        else {
                                                          iVar5 = strcmp(_Str1,".debug_macro.dwo");
                                                          if (iVar5 == 0) {
                                                            FUN_14001bd50(param_1,param_2,0x29,0,
                                                                          "_dwarf_consumer_init",
                                                                          0x94,".debug_macro.dwo",0,
                                                                          0);
                                                          }
                                                          else {
                                                            iVar5 = strcmp(_Str1,".debug_str.dwo");
                                                            if (iVar5 == 0) {
                                                              FUN_14001bd50(param_1,param_2,0x29,0,
                                                                            "_dwarf_consumer_init",
                                                                            0x96,".debug_str.dwo",0,
                                                                            0);
                                                            }
                                                            else {
                                                              iVar5 = strcmp(_Str1,
                                                  ".debug_str_offsets.dwo");
                                                  if (iVar5 == 0) {
                                                    FUN_14001bd50(param_1,param_2,0x29,0,
                                                                  "_dwarf_consumer_init",0x98,
                                                                  ".debug_str_offsets.dwo",0,0);
                                                  }
                                                  else {
                                                    iVar5 = strcmp(_Str1,".debug_cu_index");
                                                    if (iVar5 == 0) {
                                                      FUN_14001bd50(param_1,param_2,0x29,0,
                                                                    "_dwarf_consumer_init",0x9a,
                                                                    ".debug_cu_index",0,0);
                                                    }
                                                    else {
                                                      iVar5 = strcmp(_Str1,".debug_tu_index");
                                                      if (iVar5 == 0) {
                                                        FUN_14001bd50(param_1,param_2,0x29,0,
                                                                      "_dwarf_consumer_init",0x9c,
                                                                      ".debug_tu_index",0,0);
                                                      }
                                                      else {
                                                        FUN_14001bd50(param_1,param_2,0x2a,0,
                                                                      "_dwarf_consumer_init",0x9e,
                                                                      _Str1,0,0);
                                                      }
                                                    }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
              else {
                iVar5 = strcmp(local_48,".eh_frame");
                if (iVar5 == 0) {
                  param_1[0x12] = uVar11;
                }
              }
              local_res18 = local_res18 + 0x20;
              uVar11 = uVar11 + 1;
            } while (uVar11 < _Count);
          }
          lVar8 = FUN_140026bb0(param_1,4);
          if (lVar8 == 0) {
            uVar12 = 0xa7;
          }
          else {
            lVar8 = FUN_140026bb0(param_1,0);
            param_1[2] = lVar8;
            if (lVar8 != 0) {
              lVar8 = FUN_140026bb0(param_1,1);
              param_1[4] = lVar8;
              FUN_14001cd70(param_1);
              return 0;
            }
            uVar12 = 0xac;
          }
          FUN_14001bb80(param_1,param_2,7,0,"_dwarf_consumer_init",uVar12);
          uVar12 = 7;
        }
      }
      return uVar12;
    }
    uVar12 = 0x2c;
  }
  FUN_14001bb80(param_1,param_2,0x1c,0,"_dwarf_consumer_init",uVar12);
  return 0x1c;
}


/* ================= FUN_14001f420 @ 14001f420  size=25  callers=2 callees=0 ================= */
/* CALLERS: FUN_140018180, FUN_14001f440 */

void FUN_14001f420(longlong param_1)

{
  if (*(int *)(param_1 + 0xf0) == 1) {
    if (param_1 != 0) {
      FUN_14001e200();
      FUN_140022ff0(param_1);
      FUN_1400238a0(param_1);
      FUN_140024e30(param_1);
      FUN_14001c9a0(param_1);
      FUN_14001a2c0(param_1);
      FUN_140023ea0(param_1);
      FUN_140026f40(param_1);
      FUN_140024950(param_1 + 0x160);
      FUN_140024950(param_1 + 0x168);
      FUN_140024950(param_1 + 0x170);
      FUN_140024950(param_1 + 0x178);
      FUN_140024950(param_1 + 0x180);
      FUN_140024950(param_1 + 0x188);
      free(*(void **)(param_1 + 8));
    }
    return;
  }
  if (*(int *)(param_1 + 0xf0) != 2) {
    return;
  }
  FUN_14001ea20();
  FUN_14001b6f0(param_1);
  FUN_140019610(param_1);
  FUN_140021460(param_1);
  FUN_14001cec0(param_1);
  FUN_14001a420(param_1);
  FUN_140024ac0(param_1 + 0x2c8);
  FUN_140024ac0(param_1 + 0x2d0);
  FUN_140024ac0(param_1 + 0x2d8);
  FUN_140024ac0(param_1 + 0x2e0);
  FUN_140024ac0(param_1 + 0x2e8);
  FUN_140026c00(param_1);
  FUN_1400256c0(param_1);
  return;
}


/* ================= FUN_14001f440 @ 14001f440  size=315  callers=1 callees=5 ================= */
/* CALLERS: FUN_140018bd0 */
/* CALLEES: FUN_14001bb80, FUN_14001ebb0, FUN_14001f420, FUN_14001f610, FUN_140026f70 */

ulonglong FUN_14001f440(longlong param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4,
                       undefined8 param_5)

{
  uint uVar1;
  ulonglong uVar2;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_5,0x1c,0,"_dwarf_init",0x11d);
    return 0x1c;
  }
  *(undefined8 *)(param_1 + 0x110) = param_3;
  *(undefined8 **)(param_1 + 0x128) = (undefined8 *)(param_1 + 0x120);
  *(undefined8 *)(param_1 + 0x118) = param_4;
  *(undefined8 *)(param_1 + 0x120) = 0;
  *(undefined8 *)(param_1 + 0x130) = 0;
  *(undefined8 **)(param_1 + 0x138) = (undefined8 *)(param_1 + 0x130);
  *(undefined8 *)(param_1 + 0x1e8) = 0;
  *(undefined8 **)(param_1 + 0x1f0) = (undefined8 *)(param_1 + 0x1e8);
  *(undefined8 *)(param_1 + 0x1a0) = 0;
  *(undefined8 **)(param_1 + 0x1a8) = (undefined8 *)(param_1 + 0x1a0);
  *(undefined8 *)(param_1 + 0x1d8) = 0;
  *(undefined8 **)(param_1 + 0x1e0) = (undefined8 *)(param_1 + 0x1d8);
  *(undefined8 *)(param_1 + 0x150) = 0;
  *(undefined8 **)(param_1 + 0x158) = (undefined8 *)(param_1 + 0x150);
  if ((*(int *)(param_1 + 0xf0) == 1) || (*(int *)(param_1 + 0xf0) == 4)) {
    uVar1 = FUN_14001ebb0(param_1,param_5);
    if (uVar1 != 0) goto LAB_14001f545;
  }
  if (*(int *)(param_1 + 0xf0) == 2) {
    uVar1 = FUN_14001f610(param_1,param_2,param_5);
    if (uVar1 != 0) {
LAB_14001f545:
      FUN_14001f420(param_1);
      return (ulonglong)uVar1;
    }
  }
  uVar2 = FUN_140026f70(param_1,param_5);
  return uVar2;
}


/* ================= FUN_14001f580 @ 14001f580  size=135  callers=0 callees=9 ================= */
/* CALLEES: FUN_140019610, FUN_14001a420, FUN_14001b6f0, FUN_14001cec0, FUN_14001ea20, FUN_140021460, FUN_140024ac0, FUN_1400256c0, FUN_140026c00 */

void FUN_14001f580(longlong param_1)

{
  FUN_14001ea20();
  FUN_14001b6f0(param_1);
  FUN_140019610(param_1);
  FUN_140021460(param_1);
  FUN_14001cec0(param_1);
  FUN_14001a420(param_1);
  FUN_140024ac0(param_1 + 0x2c8);
  FUN_140024ac0(param_1 + 0x2d0);
  FUN_140024ac0(param_1 + 0x2d8);
  FUN_140024ac0(param_1 + 0x2e0);
  FUN_140024ac0(param_1 + 0x2e8);
  FUN_140026c00(param_1);
  FUN_1400256c0(param_1);
  return;
}


/* ================= FUN_14001f610 @ 14001f610  size=728  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001f440 */
/* CALLEES: FUN_14001bb80, calloc */

undefined4 FUN_14001f610(longlong param_1,ulonglong param_2,undefined8 param_3)

{
  code *pcVar1;
  ulonglong uVar2;
  void *pvVar3;
  ulonglong uVar4;
  undefined4 uVar5;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_3,0x1c,0,"_dwarf_producer_init",0xbc);
    return 0x1c;
  }
  *(undefined4 *)(param_1 + 0xf8) = 4;
  if (((uint)param_2 & 0x60000000) == 0x60000000) {
    uVar5 = 0xc3;
    goto LAB_14001f67c;
  }
  uVar2 = param_2 | 0x20000000;
  if ((param_2 & 0x60000000) != 0) {
    uVar2 = param_2;
  }
  uVar5 = 8;
  if ((uVar2 >> 0x1e & 1) == 0) {
    uVar5 = 4;
  }
  *(undefined4 *)(param_1 + 0xf4) = uVar5;
  if ((uVar2 & 0x81000000) == 0x81000000) {
    uVar5 = 0xd2;
    goto LAB_14001f67c;
  }
  *(ulonglong *)(param_1 + 0x238) = 2 - (ulonglong)((uVar2 & 0x1000000) != 0);
  if (((uint)uVar2 & 0x8100000) == 0x8100000) {
    uVar5 = 0xdd;
    goto LAB_14001f67c;
  }
  uVar4 = uVar2 | 0x8000000;
  if ((uVar2 & 0x8100000) != 0) {
    uVar4 = uVar2;
  }
  if ((uVar4 >> 0x1b & 1) == 0) {
    if ((uVar4 >> 0x14 & 1) != 0) {
      *(code **)(param_1 + 0x200) = FUN_140026890;
      pcVar1 = FUN_140026930;
      goto LAB_14001f762;
    }
  }
  else {
    *(code **)(param_1 + 0x200) = FUN_140026a20;
    pcVar1 = FUN_140026ac0;
LAB_14001f762:
    *(code **)(param_1 + 0x208) = pcVar1;
  }
  if (((uint)uVar4 & 0x6000000) != 0x6000000) {
    uVar2 = uVar4 | 0x2000000;
    if ((uVar4 & 0x6000000) != 0) {
      uVar2 = uVar4;
    }
    *(ulonglong *)(param_1 + 0x230) = uVar2;
    *(undefined8 *)(param_1 + 600) = 0;
    *(undefined8 **)(param_1 + 0x260) = (undefined8 *)(param_1 + 600);
    *(undefined8 *)(param_1 + 0x268) = 0;
    *(undefined8 **)(param_1 + 0x270) = (undefined8 *)(param_1 + 0x268);
    *(undefined8 *)(param_1 + 0x2f0) = 0;
    *(undefined8 **)(param_1 + 0x2f8) = (undefined8 *)(param_1 + 0x2f0);
    *(undefined8 *)(param_1 + 0x318) = 0;
    *(undefined8 **)(param_1 + 800) = (undefined8 *)(param_1 + 0x318);
    *(undefined8 *)(param_1 + 0x298) = 0;
    *(undefined8 **)(param_1 + 0x2a0) = (undefined8 *)(param_1 + 0x298);
    *(undefined8 *)(param_1 + 0x2a8) = 0;
    *(undefined8 **)(param_1 + 0x2b0) = (undefined8 *)(param_1 + 0x2a8);
    pvVar3 = calloc(1,0xb0);
    *(void **)(param_1 + 0x278) = pvVar3;
    if (pvVar3 == (void *)0x0) {
      uVar5 = 0x109;
    }
    else {
      *(undefined8 *)((longlong)pvVar3 + 0x80) = 0;
      *(longlong *)(*(longlong *)(param_1 + 0x278) + 0x88) = *(longlong *)(param_1 + 0x278) + 0x80;
      *(undefined8 *)(*(longlong *)(param_1 + 0x278) + 0xa0) = 0;
      *(longlong *)(*(longlong *)(param_1 + 0x278) + 0xa8) = *(longlong *)(param_1 + 0x278) + 0xa0;
      pvVar3 = calloc(1,0x38);
      *(void **)(param_1 + 0x280) = pvVar3;
      if (pvVar3 != (void *)0x0) {
        *(undefined8 *)((longlong)pvVar3 + 0x20) = 0;
        *(longlong *)(*(longlong *)(param_1 + 0x280) + 0x28) = *(longlong *)(param_1 + 0x280) + 0x20
        ;
        return 0;
      }
      uVar5 = 0x110;
    }
    FUN_14001bb80(param_1,param_3,1,0,"_dwarf_producer_init",uVar5);
    return 1;
  }
  uVar5 = 0xf6;
LAB_14001f67c:
  FUN_14001bb80(param_1,param_3,0x1c,0,"_dwarf_producer_init",uVar5);
  return 0x1c;
}


/* ================= FUN_14001f8f0 @ 14001f8f0  size=62  callers=2 callees=1 ================= */
/* CALLERS: FUN_14001fa50, FUN_14001fb30 */
/* CALLEES: FUN_14000e430 */

undefined8 FUN_14001f8f0(char *param_1,int param_2)

{
  char cVar1;
  
  if ((param_1 == (char *)0x0) || (param_2 < 1)) {
    return 0;
  }
  if (*param_1 != '.') {
    cVar1 = FUN_14000e430();
    if (cVar1 != ' ') {
      return 0;
    }
    if (*param_1 == '\\') {
      return 0;
    }
    if (*param_1 == '/') {
      return 0;
    }
  }
  return 1;
}


/* ================= FUN_14001f930 @ 14001f930  size=281  callers=2 callees=2 ================= */
/* CALLERS: FUN_14001fa50, FUN_14001fb30 */
/* CALLEES: calloc, strncpy */

undefined4
FUN_14001f930(char *param_1,int param_2,char *param_3,int param_4,char *param_5,int param_6,
             undefined8 *param_7)

{
  int iVar1;
  char *_Dest;
  char *_Dest_00;
  
  _Dest = (char *)0x0;
  *param_7 = 0;
  if ((param_3 != (char *)0x0) || (param_1 != (char *)0x0)) {
    iVar1 = param_6 + 1;
    if (param_3 != (char *)0x0) {
      iVar1 = param_6 + 3 + param_4;
    }
    if (param_1 != (char *)0x0) {
      iVar1 = iVar1 + 2 + param_2;
    }
    _Dest = calloc(1,(longlong)iVar1);
    if (_Dest == (char *)0x0) {
      return 1;
    }
    _Dest_00 = _Dest;
    if (param_3 != (char *)0x0) {
      strncpy(_Dest,param_3,(longlong)(param_4 + 1));
      _Dest_00 = _Dest + param_4;
    }
    if (param_1 != (char *)0x0) {
      if (_Dest_00 != _Dest) {
        *_Dest_00 = '/';
        _Dest_00 = _Dest_00 + 1;
      }
      strncpy(_Dest_00,param_1,(longlong)(param_2 + 1));
      _Dest_00 = _Dest_00 + param_2;
    }
    if (param_5 != (char *)0x0) {
      if (_Dest_00 != _Dest) {
        *_Dest_00 = '/';
        _Dest_00 = _Dest_00 + 1;
      }
      strncpy(_Dest_00,param_5,(longlong)(param_6 + 1));
      _Dest_00[param_6] = '\0';
    }
  }
  *param_7 = _Dest;
  return 0;
}


/* ================= FUN_14001fa50 @ 14001fa50  size=218  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001fc00 */
/* CALLEES: FUN_14001f8f0, FUN_14001f930 */

undefined8
FUN_14001fa50(undefined8 param_1,undefined8 param_2,longlong param_3,ulonglong param_4,
             undefined4 param_5,int param_6,undefined8 param_7,undefined4 param_8,
             undefined8 *param_9)

{
  int iVar1;
  undefined8 uVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  undefined4 uVar5;
  ulonglong uVar6;
  
  uVar3 = 0;
  *param_9 = 0;
  iVar1 = FUN_14001f8f0(param_7,param_8);
  if (iVar1 == 0) {
    return 0;
  }
  uVar4 = uVar3;
  uVar6 = uVar3;
  if (0 < param_6) {
    uVar4 = *(ulonglong *)(*(longlong *)(param_3 + 0x28) + -8 + (longlong)param_6 * 8);
    uVar6 = (ulonglong)*(ushort *)(*(longlong *)(param_3 + 0x38) + -2 + (longlong)param_6 * 2);
    iVar1 = FUN_14001f8f0(uVar4,uVar6);
    uVar5 = 0;
    if (iVar1 == 0) goto LAB_14001fade;
  }
  uVar3 = param_4;
  uVar5 = param_5;
LAB_14001fade:
  uVar2 = FUN_14001f930(uVar4,uVar6,uVar3,uVar5,param_7,param_8,param_9);
  return uVar2;
}


/* ================= FUN_14001fb30 @ 14001fb30  size=195  callers=1 callees=2 ================= */
/* CALLERS: FUN_140020a70 */
/* CALLEES: FUN_14001f8f0, FUN_14001f930 */

undefined8
FUN_14001fb30(undefined8 param_1,undefined8 param_2,longlong param_3,undefined8 param_4,
             undefined4 param_5,int param_6,undefined8 param_7,undefined4 param_8,
             undefined8 *param_9)

{
  undefined2 uVar1;
  undefined8 uVar2;
  int iVar3;
  undefined8 uVar4;
  undefined4 uVar5;
  
  uVar4 = 0;
  *param_9 = 0;
  iVar3 = FUN_14001f8f0(param_7,param_8);
  if (iVar3 != 0) {
    uVar2 = *(undefined8 *)(*(longlong *)(param_3 + 0x28) + (longlong)param_6 * 8);
    uVar1 = *(undefined2 *)(*(longlong *)(param_3 + 0x38) + (longlong)param_6 * 2);
    iVar3 = FUN_14001f8f0(uVar2,uVar1);
    uVar5 = 0;
    if (iVar3 != 0) {
      uVar4 = param_4;
      uVar5 = param_5;
    }
    uVar4 = FUN_14001f930(uVar2,uVar1,uVar4,uVar5,param_7,param_8,param_9);
  }
  return uVar4;
}


/* ================= FUN_14001fc00 @ 14001fc00  size=541  callers=2 callees=5 ================= */
/* CALLERS: FUN_140020940, FUN_140021d80 */
/* CALLEES: FUN_14001bb80, FUN_14001fa50, FUN_140025a70, calloc, free */

int FUN_14001fc00(longlong param_1,longlong *param_2,undefined8 param_3,undefined4 param_4,
                 undefined8 param_5,undefined8 param_6)

{
  int iVar1;
  longlong *_Memory;
  ulonglong uVar2;
  longlong lVar3;
  longlong local_res10;
  undefined8 in_stack_ffffffffffffffa8;
  undefined4 uVar5;
  longlong lVar4;
  
  uVar5 = (undefined4)((ulonglong)in_stack_ffffffffffffffa8 >> 0x20);
  if (param_2 != (longlong *)0x0) {
    local_res10 = *param_2;
    _Memory = calloc(1,0x30);
    if (_Memory == (longlong *)0x0) {
      FUN_14001bb80(param_6,param_5,1,0,"_dwarf_lineno_add_file",0x12d);
      iVar1 = 1;
    }
    else {
      _Memory[1] = 0;
      *_Memory = local_res10;
      lVar3 = -1;
      do {
        lVar4 = lVar3;
        lVar3 = lVar4 + 1;
      } while (*(char *)(local_res10 + lVar3) != '\0');
      local_res10 = local_res10 + ((int)lVar4 + 2);
      uVar2 = FUN_140025a70(&local_res10);
      _Memory[2] = uVar2;
      if (*(ulonglong *)(param_1 + 0x30) < uVar2) {
        free(_Memory);
        FUN_14001bb80(param_6,param_5,0xe,0,"_dwarf_lineno_add_file",0x13b);
        iVar1 = 0xe;
      }
      else {
        iVar1 = FUN_14001fa50(param_6,param_5,param_1,param_3,CONCAT44(uVar5,param_4),(int)uVar2,
                              *_Memory,(int)lVar3,_Memory + 1);
        if (iVar1 == 0) {
          lVar3 = FUN_140025a70(&local_res10);
          _Memory[3] = lVar3;
          lVar3 = FUN_140025a70(&local_res10);
          _Memory[4] = lVar3;
          _Memory[5] = 0;
          **(undefined8 **)(param_1 + 0x88) = _Memory;
          *(longlong *)(param_1 + 0x48) = *(longlong *)(param_1 + 0x48) + 1;
          *(longlong **)(param_1 + 0x88) = _Memory + 5;
          *param_2 = local_res10;
          iVar1 = 0;
        }
        else {
          free(_Memory);
        }
      }
    }
    return iVar1;
  }
  FUN_14001bb80(param_6,param_5,0x1c,0,"_dwarf_lineno_add_file",0x126);
  return 0x1c;
}


/* ================= FUN_14001fe20 @ 14001fe20  size=615  callers=1 callees=7 ================= */
/* CALLERS: FUN_1400211b0 */
/* CALLEES: FUN_14001bb80, FUN_140026240, FUN_1400262d0, FUN_140026310, FUN_1400263a0, FUN_140026450, FUN_140026d00 */

undefined4
FUN_14001fe20(longlong *param_1,undefined8 param_2,longlong param_3,undefined8 param_4,
             undefined8 param_5,int param_6,undefined8 param_7)

{
  int iVar1;
  longlong lVar2;
  longlong local_res8;
  undefined8 uVar3;
  
  lVar2 = *param_1;
  local_res8 = 0;
  if (lVar2 == 0) {
    FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_lineno_add_include_dir_name_dwarf5",0x272);
    return 0x1c;
  }
  switch(param_4) {
  case 8:
    iVar1 = FUN_1400262d0(lVar2,param_2,param_5,param_1,&local_res8,param_7);
    if (iVar1 != 0) {
      uVar3 = 0x27c;
LAB_14001fff1:
      FUN_14001bb80(lVar2,param_7,0x1c,0,"_dwarf_lineno_add_include_dir_name_dwarf5",uVar3);
      return 0x1c;
    }
    break;
  default:
    FUN_140026d00(lVar2,param_2,param_5,*(undefined1 *)(param_3 + 10),param_4);
    break;
  case 0xe:
    iVar1 = FUN_140026310(lVar2,param_2,param_5,param_1,0,&local_res8,param_7);
    if (iVar1 != 0) {
      uVar3 = 0x284;
      goto LAB_14001fff1;
    }
    break;
  case 0x1a:
    iVar1 = FUN_1400263a0(lVar2,param_2,param_5,param_1,0,0,&local_res8,param_7);
    if (iVar1 != 0) {
      uVar3 = 0x294;
      goto LAB_14001fff1;
    }
    break;
  case 0x1f:
    iVar1 = FUN_140026240(lVar2,param_2,param_5,param_1,0,&local_res8,param_7);
    if (iVar1 != 0) {
      uVar3 = 0x28c;
      goto LAB_14001fff1;
    }
    break;
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
    iVar1 = FUN_140026450(lVar2,param_2,param_5,param_1,(int)param_4 + -0x24,0,0,&local_res8,param_7
                         );
    if (iVar1 != 0) {
      uVar3 = 0x2a2;
      goto LAB_14001fff1;
    }
  }
  *(longlong *)(*(longlong *)(param_3 + 0x28) + (longlong)param_6 * 8) = local_res8;
  if (local_res8 != 0) {
    lVar2 = -1;
    do {
      lVar2 = lVar2 + 1;
    } while (*(char *)(local_res8 + lVar2) != '\0');
    *(short *)(*(longlong *)(param_3 + 0x38) + (longlong)param_6 * 2) = (short)lVar2;
  }
  return 0;
}


/* ================= FUN_1400200d0 @ 1400200d0  size=459  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001e200 */
/* CALLEES: free */

void FUN_1400200d0(void *param_1)

{
  longlong *plVar1;
  void *pvVar2;
  void *pvVar3;
  longlong lVar4;
  void *pvVar5;
  void *pvVar6;
  
  if (param_1 != (void *)0x0) {
    plVar1 = (longlong *)((longlong)param_1 + 0x80);
    pvVar6 = (void *)*plVar1;
    while (pvVar6 != (void *)0x0) {
      pvVar2 = (void *)*plVar1;
      pvVar3 = *(void **)((longlong)pvVar6 + 0x28);
      if (pvVar2 == pvVar6) {
        lVar4 = *(longlong *)((longlong)pvVar2 + 0x28);
        *plVar1 = lVar4;
        if (lVar4 == 0) {
          *(longlong **)((longlong)param_1 + 0x88) = plVar1;
        }
      }
      else {
        for (pvVar5 = *(void **)((longlong)pvVar2 + 0x28); pvVar5 != pvVar6;
            pvVar5 = *(void **)((longlong)pvVar5 + 0x28)) {
          pvVar2 = pvVar5;
        }
        lVar4 = *(longlong *)(*(longlong *)((longlong)pvVar2 + 0x28) + 0x28);
        *(longlong *)((longlong)pvVar2 + 0x28) = lVar4;
        if (lVar4 == 0) {
          *(longlong **)((longlong)param_1 + 0x88) = (longlong *)((longlong)pvVar2 + 0x28);
        }
      }
      if (*(void **)((longlong)pvVar6 + 8) != (void *)0x0) {
        free(*(void **)((longlong)pvVar6 + 8));
      }
      free(pvVar6);
      pvVar6 = pvVar3;
    }
    plVar1 = (longlong *)((longlong)param_1 + 0xa0);
    pvVar6 = (void *)*plVar1;
    while (pvVar6 != (void *)0x0) {
      pvVar2 = (void *)*plVar1;
      pvVar3 = *(void **)((longlong)pvVar6 + 0x48);
      if (pvVar2 == pvVar6) {
        lVar4 = *(longlong *)((longlong)pvVar2 + 0x48);
        *plVar1 = lVar4;
        if (lVar4 == 0) {
          *(longlong **)((longlong)param_1 + 0xa8) = plVar1;
        }
      }
      else {
        for (pvVar5 = *(void **)((longlong)pvVar2 + 0x48); pvVar5 != pvVar6;
            pvVar5 = *(void **)((longlong)pvVar5 + 0x48)) {
          pvVar2 = pvVar5;
        }
        lVar4 = *(longlong *)(*(longlong *)((longlong)pvVar2 + 0x48) + 0x48);
        *(longlong *)((longlong)pvVar2 + 0x48) = lVar4;
        if (lVar4 == 0) {
          *(longlong **)((longlong)param_1 + 0xa8) = (longlong *)((longlong)pvVar2 + 0x48);
        }
      }
      free(pvVar6);
      pvVar6 = pvVar3;
    }
    if (*(void **)((longlong)param_1 + 0x20) != (void *)0x0) {
      free(*(void **)((longlong)param_1 + 0x20));
    }
    if (*(void **)((longlong)param_1 + 0x28) != (void *)0x0) {
      free(*(void **)((longlong)param_1 + 0x28));
    }
    if (*(void **)((longlong)param_1 + 0x38) != (void *)0x0) {
      free(*(void **)((longlong)param_1 + 0x38));
    }
    if (*(void **)((longlong)param_1 + 0x90) != (void *)0x0) {
      free(*(void **)((longlong)param_1 + 0x90));
    }
    if (*(void **)((longlong)param_1 + 0x40) != (void *)0x0) {
      free(*(void **)((longlong)param_1 + 0x40));
    }
    if (*(void **)((longlong)param_1 + 0x58) != (void *)0x0) {
      free(*(void **)((longlong)param_1 + 0x58));
    }
    if (*(void **)((longlong)param_1 + 0x60) != (void *)0x0) {
      free(*(void **)((longlong)param_1 + 0x60));
    }
    if (*(void **)((longlong)param_1 + 0x70) != (void *)0x0) {
      free(*(void **)((longlong)param_1 + 0x70));
    }
    if (*(void **)((longlong)param_1 + 0x78) != (void *)0x0) {
      free(*(void **)((longlong)param_1 + 0x78));
    }
    free(param_1);
  }
  return;
}


/* ================= FUN_1400202a0 @ 1400202a0  size=1647  callers=2 callees=10 ================= */
/* CALLERS: FUN_140018cf0, FUN_140018f60 */
/* CALLEES: FUN_14001ad70, FUN_14001bb80, FUN_140020940, FUN_140020a70, FUN_140021000, FUN_1400211b0, FUN_140021d80, FUN_140026bb0, calloc, free */

int FUN_1400202a0(longlong param_1,longlong param_2,undefined8 param_3)

{
  longlong lVar1;
  void *pvVar2;
  void *pvVar3;
  void *pvVar4;
  longlong lVar5;
  longlong *plVar6;
  undefined1 uVar7;
  byte bVar8;
  ushort uVar9;
  longlong lVar10;
  void *pvVar11;
  ulonglong uVar12;
  ulonglong *_Memory;
  longlong lVar13;
  longlong lVar14;
  longlong lVar15;
  ulonglong *puVar16;
  longlong lVar17;
  int iVar18;
  undefined4 local_res8;
  longlong local_res10;
  undefined8 local_res18;
  longlong *local_res20;
  undefined4 uVar19;
  
  local_res10 = param_2;
  local_res18 = param_3;
  if (param_1 == 0) {
    uVar19 = 0x594;
LAB_1400202c7:
    FUN_14001bb80(0,param_3,0x1c,0,"_dwarf_lineno_init",uVar19);
    return 0x1c;
  }
  local_res20 = *(longlong **)(param_1 + 0x50);
  if (local_res20 == (longlong *)0x0) {
    uVar19 = 0x599;
    local_res20 = (longlong *)0x0;
    goto LAB_1400202c7;
  }
  lVar1 = *local_res20;
  if (lVar1 == 0) {
    FUN_14001bb80(0,param_3,0x1c,0,"_dwarf_lineno_init",0x59e);
    return 0x1c;
  }
  lVar10 = FUN_140026bb0(lVar1,6);
  if (lVar10 == 0) {
    return 0;
  }
  lVar15 = 0;
  pvVar11 = (void *)FUN_14001ad70(param_1,0x1b);
  lVar14 = 0;
  if (pvVar11 != (void *)0x0) {
    switch(*(undefined4 *)((longlong)pvVar11 + 0x18)) {
    case 8:
      lVar15 = *(longlong *)((longlong)pvVar11 + 0x20);
      lVar14 = 0;
      if (lVar15 != 0) {
        lVar14 = -1;
        do {
          lVar14 = lVar14 + 1;
        } while (*(char *)(lVar15 + lVar14) != '\0');
      }
      break;
    case 0xe:
    case 0x1a:
    case 0x1d:
    case 0x1f:
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
      lVar15 = *(longlong *)((longlong)pvVar11 + 0x28);
      lVar14 = 0;
      if (lVar15 != 0) {
        lVar14 = -1;
        do {
          lVar14 = lVar14 + 1;
        } while (*(char *)(lVar15 + lVar14) != '\0');
      }
    }
  }
  free(pvVar11);
  local_res8 = 4;
  uVar12 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,4);
  if (uVar12 == 0xffffffff) {
    local_res8 = 8;
    uVar12 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,8);
  }
  if ((ulonglong)(*(longlong *)(lVar10 + 0x18) - local_res10) < uVar12) {
    FUN_14001bb80(lVar1,param_3,0xc,0,"_dwarf_lineno_init",0x5d9);
    return 0xc;
  }
  iVar18 = 1;
  _Memory = calloc(1,0xb0);
  if (_Memory == (ulonglong *)0x0) {
    FUN_14001bb80(lVar1,param_3,1,0,"_dwarf_lineno_init",0x5df);
    return 1;
  }
  *_Memory = uVar12;
  lVar13 = local_res10 + uVar12;
  uVar9 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,2);
  *(ushort *)(_Memory + 1) = uVar9;
  if (4 < uVar9) {
    uVar7 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,1);
    *(undefined1 *)((longlong)_Memory + 10) = uVar7;
    uVar7 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,1);
    *(undefined1 *)((longlong)_Memory + 0xb) = uVar7;
  }
  uVar12 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,local_res8);
  lVar5 = local_res10;
  _Memory[2] = uVar12;
  uVar7 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,1);
  *(undefined1 *)(_Memory + 3) = uVar7;
  if ((ushort)_Memory[1] < 4) {
    uVar7 = 1;
  }
  else {
    uVar7 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,1);
  }
  *(undefined1 *)((longlong)_Memory + 0x1d) = uVar7;
  uVar7 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,1);
  *(undefined1 *)((longlong)_Memory + 0x19) = uVar7;
  uVar7 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,1);
  *(undefined1 *)((longlong)_Memory + 0x1a) = uVar7;
  uVar7 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,1);
  *(undefined1 *)((longlong)_Memory + 0x1b) = uVar7;
  bVar8 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,1);
  *(byte *)((longlong)_Memory + 0x1c) = bVar8;
  _Memory[0x14] = 0;
  _Memory[0x10] = 0;
  _Memory[0x15] = (ulonglong)(_Memory + 0x14);
  _Memory[0x11] = (ulonglong)(_Memory + 0x10);
  if ((int)_Memory[2] + -5 < (int)(bVar8 - 1)) {
    uVar19 = 0x61d;
LAB_14002064a:
    iVar18 = 0xc;
  }
  else {
    pvVar11 = calloc(1,(ulonglong)bVar8);
    _Memory[4] = (ulonglong)pvVar11;
    if (pvVar11 == (void *)0x0) {
      uVar19 = 0x626;
    }
    else {
      if (1 < *(byte *)((longlong)_Memory + 0x1c)) {
        lVar17 = 1;
        iVar18 = 1;
        do {
          uVar7 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar10 + 8),&local_res10,1);
          lVar17 = lVar17 + 1;
          iVar18 = iVar18 + 1;
          *(undefined1 *)((_Memory[4] - 1) + lVar17) = uVar7;
          param_3 = local_res18;
        } while (iVar18 < (int)(uint)*(byte *)((longlong)_Memory + 0x1c));
      }
      if ((ushort)_Memory[1] < 5) {
        iVar18 = FUN_140021000();
        if (iVar18 == 0) {
LAB_14002072f:
          if ((ushort)_Memory[1] < 5) {
            iVar18 = FUN_140020940();
            if (iVar18 == 0) {
LAB_14002078a:
              plVar6 = local_res20;
              if (local_res10 - lVar5 == _Memory[2]) {
                iVar18 = FUN_140021d80(local_res20,_Memory,*(longlong *)(lVar10 + 8) + local_res10,
                                       lVar13 + *(longlong *)(lVar10 + 8),lVar15,(int)lVar14,param_3
                                      );
                if (iVar18 == 0) {
                  plVar6[0xf] = (longlong)_Memory;
                  return 0;
                }
                goto LAB_1400207e8;
              }
              uVar19 = 0x656;
              goto LAB_14002064a;
            }
            uVar19 = 0x647;
          }
          else {
            iVar18 = FUN_140020a70(local_res20,lVar10,&local_res10,_Memory,lVar15,(int)lVar14,
                                   param_3);
            if (iVar18 == 0) goto LAB_14002078a;
            uVar19 = 0x64d;
          }
        }
        else {
          uVar19 = 0x637;
        }
      }
      else {
        iVar18 = FUN_1400211b0(local_res20,lVar10,&local_res10,_Memory,param_3);
        if (iVar18 == 0) goto LAB_14002072f;
        uVar19 = 0x63d;
      }
    }
  }
  FUN_14001bb80(lVar1,param_3,iVar18,0,"_dwarf_lineno_init",uVar19);
LAB_1400207e8:
  puVar16 = _Memory + 0x10;
  pvVar11 = (void *)*puVar16;
  while (pvVar11 != (void *)0x0) {
    pvVar2 = (void *)*puVar16;
    pvVar3 = *(void **)((longlong)pvVar11 + 0x28);
    pvVar4 = *(void **)((longlong)pvVar2 + 0x28);
    if (pvVar2 == pvVar11) {
      *puVar16 = (ulonglong)pvVar4;
      if (pvVar4 == (void *)0x0) {
        _Memory[0x11] = (ulonglong)puVar16;
      }
    }
    else {
      for (; pvVar4 != pvVar11; pvVar4 = *(void **)((longlong)pvVar4 + 0x28)) {
        pvVar2 = pvVar4;
      }
      lVar1 = *(longlong *)(*(longlong *)((longlong)pvVar2 + 0x28) + 0x28);
      *(longlong *)((longlong)pvVar2 + 0x28) = lVar1;
      if (lVar1 == 0) {
        _Memory[0x11] = (ulonglong)((longlong)pvVar2 + 0x28);
      }
    }
    if (*(void **)((longlong)pvVar11 + 8) != (void *)0x0) {
      free(*(void **)((longlong)pvVar11 + 8));
    }
    free(pvVar11);
    pvVar11 = pvVar3;
  }
  if ((void *)_Memory[4] != (void *)0x0) {
    free((void *)_Memory[4]);
  }
  if ((void *)_Memory[5] != (void *)0x0) {
    free((void *)_Memory[5]);
  }
  if ((void *)_Memory[7] != (void *)0x0) {
    free((void *)_Memory[7]);
  }
  if ((void *)_Memory[0xb] != (void *)0x0) {
    free((void *)_Memory[0xb]);
  }
  if ((void *)_Memory[0xc] != (void *)0x0) {
    free((void *)_Memory[0xc]);
  }
  if ((void *)_Memory[0xe] != (void *)0x0) {
    free((void *)_Memory[0xe]);
  }
  if ((void *)_Memory[0xf] != (void *)0x0) {
    free((void *)_Memory[0xf]);
  }
  free(_Memory);
  return iVar18;
}


/* ================= FUN_140020940 @ 140020940  size=289  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400202a0 */
/* CALLEES: FUN_14001bb80, FUN_14001fc00 */

undefined8
FUN_140020940(longlong *param_1,longlong param_2,longlong *param_3,undefined8 param_4,
             undefined8 param_5,undefined4 param_6,undefined8 param_7)

{
  char cVar1;
  longlong lVar2;
  longlong lVar3;
  undefined8 uVar4;
  char *local_res8;
  
  lVar2 = *param_1;
  if (lVar2 == 0) {
    FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_lineno_init_file_list",0x338);
    return 0x1c;
  }
  lVar3 = *(longlong *)(param_2 + 8);
  local_res8 = (char *)(*param_3 + lVar3);
  cVar1 = *local_res8;
  while( true ) {
    if (cVar1 == '\0') {
      *param_3 = (longlong)(local_res8 + (1 - lVar3));
      return 0;
    }
    uVar4 = FUN_14001fc00(param_4,&local_res8,param_5,param_6,param_7,lVar2);
    if ((int)uVar4 != 0) break;
    lVar3 = *(longlong *)(param_2 + 8);
    if ((longlong)*(int *)(param_2 + 0x18) < (longlong)local_res8 - lVar3) {
      FUN_14001bb80(lVar2,param_7,0xc,0,"_dwarf_lineno_init_file_list",0x346);
      return 0xc;
    }
    cVar1 = *local_res8;
  }
  return uVar4;
}


/* ================= FUN_140020a70 @ 140020a70  size=1424  callers=1 callees=11 ================= */
/* CALLERS: FUN_1400202a0 */
/* CALLEES: FUN_14001bb80, FUN_14001fb30, FUN_140021610, FUN_140021720, FUN_140021880, FUN_140021b20, FUN_140021c60, FUN_140025a70, FUN_140026d00, calloc, free */

int FUN_140020a70(longlong *param_1,longlong param_2,longlong *param_3,longlong param_4,
                 undefined8 param_5,undefined4 param_6,undefined8 param_7)

{
  bool bVar1;
  byte bVar2;
  int iVar3;
  void *pvVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  longlong *_Memory;
  ulonglong uVar7;
  longlong lVar8;
  longlong lVar9;
  size_t sVar10;
  size_t _Count;
  longlong local_88;
  int local_80;
  int local_7c;
  int local_78;
  ulonglong local_70;
  longlong local_68;
  longlong local_60;
  ulonglong local_58;
  longlong local_50;
  longlong local_48;
  ulonglong local_40;
  undefined1 local_38 [8];
  undefined1 local_30 [8];
  
  lVar8 = *param_1;
  local_68 = lVar8;
  if (lVar8 == 0) {
    FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_lineno_init_file_list_dwarf5",0x4d5);
    return 0x1c;
  }
  bVar2 = (**(code **)(lVar8 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,1);
  *(byte *)(param_4 + 0x68) = bVar2;
  pvVar4 = calloc((ulonglong)bVar2,8);
  *(void **)(param_4 + 0x70) = pvVar4;
  if (pvVar4 == (void *)0x0) {
    uVar5 = 0x4e4;
  }
  else {
    pvVar4 = calloc((ulonglong)bVar2,8);
    *(void **)(param_4 + 0x78) = pvVar4;
    if (pvVar4 != (void *)0x0) {
      lVar8 = *(longlong *)(param_2 + 8);
      uVar7 = (ulonglong)bVar2;
      local_88 = *param_3 + lVar8;
      lVar9 = 0;
      local_70 = uVar7;
      if (uVar7 != 0) {
        do {
          uVar5 = FUN_140025a70(&local_88);
          uVar6 = FUN_140025a70(&local_88);
          *(undefined8 *)(*(longlong *)(param_4 + 0x70) + lVar9 * 8) = uVar5;
          *(undefined8 *)(*(longlong *)(param_4 + 0x78) + lVar9 * 8) = uVar6;
          lVar9 = lVar9 + 1;
        } while (lVar9 < (longlong)uVar7);
        lVar8 = *(longlong *)(param_2 + 8);
      }
      *param_3 = local_88 - lVar8;
      local_88 = (local_88 - lVar8) + *(longlong *)(param_2 + 8);
      local_40 = FUN_140025a70(&local_88);
      *param_3 = local_88 - *(longlong *)(param_2 + 8);
      local_78 = 0;
      uVar7 = local_70;
      if (local_40 != 0) {
        do {
          sVar10 = 0;
          local_60 = 0;
          local_58 = 0;
          local_50 = 0;
          local_48 = 0;
          bVar1 = false;
          local_80 = 0;
          local_7c = 0;
          _Count = sVar10;
          if (uVar7 != 0) {
            do {
              uVar5 = *(undefined8 *)(*(longlong *)(param_4 + 0x78) + sVar10 * 8);
              lVar8 = *(longlong *)(*(longlong *)(param_4 + 0x70) + sVar10 * 8);
              if (lVar8 == 1) {
                iVar3 = FUN_140021880(param_1,param_2,param_4,uVar5,param_3,&local_60,param_7);
                if (iVar3 != 0) {
                  uVar5 = 0x520;
LAB_140020fdd:
                  FUN_14001bb80(0,param_7,iVar3,0,"_dwarf_lineno_init_file_list_dwarf5",uVar5);
                  return iVar3;
                }
                _Count = 1;
              }
              else if (lVar8 == 2) {
                iVar3 = FUN_140021610(param_1,param_2,param_4,uVar5,param_3,&local_58,param_7);
                if (iVar3 != 0) {
                  uVar5 = 0x529;
                  goto LAB_140020fdd;
                }
                bVar1 = true;
              }
              else if (lVar8 == 3) {
                iVar3 = FUN_140021720(param_1,param_2,param_4,uVar5,param_3,&local_50,param_7);
                if (iVar3 != 0) {
                  uVar5 = 0x531;
                  goto LAB_140020fdd;
                }
                local_80 = 1;
              }
              else if (lVar8 == 4) {
                iVar3 = FUN_140021b20(param_1,param_2,param_4,uVar5,param_3,&local_48,param_7);
                if (iVar3 != 0) {
                  uVar5 = 0x539;
                  goto LAB_140020fdd;
                }
                local_7c = 1;
              }
              else if (lVar8 == 5) {
                iVar3 = FUN_140021c60(param_1,param_2,param_4,uVar5,param_3,local_30,local_38,
                                      param_7);
                if (iVar3 != 0) {
                  uVar5 = 0x541;
                  goto LAB_140020fdd;
                }
              }
              else {
                FUN_140026d00(local_68,param_2,param_3,*(undefined1 *)(param_4 + 10),uVar5);
              }
              sVar10 = sVar10 + 1;
            } while ((longlong)sVar10 < (longlong)local_70);
            uVar7 = local_70;
            if (((int)_Count == 1) && (bVar1)) {
              _Memory = calloc(_Count,0x30);
              if (_Memory == (longlong *)0x0) {
                FUN_14001bb80(local_68,param_7,1,0,"_dwarf_lineno_init_file_list_dwarf5",0x55c);
                return 1;
              }
              uVar7 = 0;
              _Memory[1] = 0;
              *_Memory = local_60;
              _Memory[2] = local_58;
              if (local_80 == 1) {
                _Memory[3] = local_50;
              }
              if (local_7c == 1) {
                _Memory[4] = local_48;
              }
              if (*(ulonglong *)(param_4 + 0x30) < local_58) {
                free(_Memory);
                FUN_14001bb80(local_68,param_7,0xe,0,"_dwarf_lineno_init_file_list_dwarf5",0x570);
                return 0xe;
              }
              lVar8 = *_Memory;
              if (lVar8 != 0) {
                uVar7 = 0xffffffffffffffff;
                do {
                  uVar7 = uVar7 + 1;
                } while (*(char *)(lVar8 + uVar7) != '\0');
              }
              iVar3 = FUN_14001fb30(local_68,param_7,param_4,**(undefined8 **)(param_4 + 0x28),
                                    param_6,local_58 & 0xffffffff,lVar8,uVar7 & 0xffffffff,
                                    _Memory + 1);
              if (iVar3 != 0) {
                free(_Memory);
                return iVar3;
              }
              _Memory[5] = 0;
              **(undefined8 **)(param_4 + 0x88) = _Memory;
              *(longlong *)(param_4 + 0x48) = *(longlong *)(param_4 + 0x48) + 1;
              *(longlong **)(param_4 + 0x88) = _Memory + 5;
              uVar7 = local_70;
            }
          }
          local_78 = local_78 + 1;
        } while ((ulonglong)(longlong)local_78 < local_40);
      }
      return 0;
    }
    uVar5 = 0x4ea;
  }
  FUN_14001bb80(lVar8,param_7,1,0,"_dwarf_lineno_init_file_list_dwarf5",uVar5);
  return 1;
}


/* ================= FUN_140021000 @ 140021000  size=423  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400202a0 */
/* CALLEES: FUN_14001bb80, calloc */

undefined4
FUN_140021000(longlong *param_1,longlong param_2,longlong *param_3,longlong param_4,
             undefined8 param_5)

{
  short *psVar1;
  char cVar2;
  void *pvVar3;
  longlong lVar4;
  char *pcVar5;
  size_t sVar6;
  size_t sVar7;
  undefined4 uVar8;
  
  lVar4 = *param_1;
  if (lVar4 == 0) {
    FUN_14001bb80(0,param_5,0x1c,0,"_dwarf_lineno_init_include_directories",0x217);
    return 0x1c;
  }
  pcVar5 = (char *)(*(longlong *)(param_2 + 8) + *param_3);
  sVar6 = 0;
  cVar2 = *pcVar5;
  sVar7 = sVar6;
  while (cVar2 != '\0') {
    do {
      cVar2 = *pcVar5;
      pcVar5 = pcVar5 + 1;
    } while (cVar2 != '\0');
    sVar7 = sVar7 + 1;
    cVar2 = *pcVar5;
  }
  *(size_t *)(param_4 + 0x30) = sVar7;
  if ((longlong)*(int *)(param_2 + 0x18) < (longlong)pcVar5 - *(longlong *)(param_2 + 8)) {
    FUN_14001bb80(lVar4,param_5,0xc,0,"_dwarf_lineno_init_include_directories",0x230);
    uVar8 = 0xc;
  }
  else {
    pvVar3 = calloc(sVar7,8);
    *(void **)(param_4 + 0x28) = pvVar3;
    if (pvVar3 == (void *)0x0) {
      uVar8 = 0x239;
    }
    else {
      pvVar3 = calloc(sVar7,2);
      *(void **)(param_4 + 0x38) = pvVar3;
      if (pvVar3 != (void *)0x0) {
        lVar4 = *(longlong *)(param_2 + 8);
        pcVar5 = (char *)(*param_3 + lVar4);
        sVar7 = sVar6;
        if (*pcVar5 != '\0') {
          do {
            *(char **)(sVar7 + *(longlong *)(param_4 + 0x28)) = pcVar5;
            *(undefined2 *)(sVar6 + *(longlong *)(param_4 + 0x38)) = 0;
            cVar2 = *pcVar5;
            while (cVar2 != '\0') {
              pcVar5 = pcVar5 + 1;
              psVar1 = (short *)(*(longlong *)(param_4 + 0x38) + sVar6);
              *psVar1 = *psVar1 + 1;
              cVar2 = *pcVar5;
            }
            pcVar5 = pcVar5 + 1;
            sVar6 = sVar6 + 2;
            sVar7 = sVar7 + 8;
          } while (*pcVar5 != '\0');
          lVar4 = *(longlong *)(param_2 + 8);
        }
        *param_3 = (longlong)(pcVar5 + (1 - lVar4));
        return 0;
      }
      uVar8 = 0x23f;
    }
    FUN_14001bb80(lVar4,param_5,1,0,"_dwarf_lineno_init_include_directories",uVar8);
    uVar8 = 1;
  }
  return uVar8;
}


/* ================= FUN_1400211b0 @ 1400211b0  size=687  callers=1 callees=5 ================= */
/* CALLERS: FUN_1400202a0 */
/* CALLEES: FUN_14001bb80, FUN_14001fe20, FUN_140025a70, FUN_140026d00, calloc */

ulonglong FUN_1400211b0(longlong *param_1,longlong param_2,longlong *param_3,longlong param_4,
                       undefined8 param_5)

{
  longlong lVar1;
  byte bVar2;
  uint uVar3;
  void *pvVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  size_t _Count;
  ulonglong uVar7;
  int iVar8;
  longlong lVar9;
  ulonglong uVar10;
  undefined4 uVar11;
  longlong local_38;
  size_t local_30;
  
  lVar1 = *param_1;
  uVar3 = 0;
  if (lVar1 == 0) {
    FUN_14001bb80(0,param_5,0x1c,0,"_dwarf_lineno_init_include_directories_dwarf5",0x2d5);
    return 0x1c;
  }
  bVar2 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,1);
  *(byte *)(param_4 + 0x50) = bVar2;
  pvVar4 = calloc((ulonglong)bVar2,8);
  *(void **)(param_4 + 0x58) = pvVar4;
  if (pvVar4 == (void *)0x0) {
    uVar11 = 0x2e4;
  }
  else {
    pvVar4 = calloc((ulonglong)bVar2,8);
    *(void **)(param_4 + 0x60) = pvVar4;
    if (pvVar4 != (void *)0x0) {
      lVar9 = *(longlong *)(param_2 + 8);
      local_38 = *param_3 + lVar9;
      uVar10 = (ulonglong)bVar2;
      if (bVar2 != 0) {
        lVar9 = 0;
        do {
          uVar5 = FUN_140025a70(&local_38);
          uVar6 = FUN_140025a70(&local_38);
          *(undefined8 *)(*(longlong *)(param_4 + 0x58) + lVar9 * 8) = uVar5;
          *(undefined8 *)(*(longlong *)(param_4 + 0x60) + lVar9 * 8) = uVar6;
          lVar9 = lVar9 + 1;
        } while (lVar9 < (longlong)uVar10);
        lVar9 = *(longlong *)(param_2 + 8);
      }
      *param_3 = local_38 - lVar9;
      local_38 = (local_38 - lVar9) + *(longlong *)(param_2 + 8);
      _Count = FUN_140025a70(&local_38);
      *param_3 = local_38 - *(longlong *)(param_2 + 8);
      local_30 = _Count;
      pvVar4 = calloc(_Count,8);
      *(void **)(param_4 + 0x28) = pvVar4;
      if (pvVar4 == (void *)0x0) {
        uVar11 = 0x304;
      }
      else {
        pvVar4 = calloc(_Count,2);
        *(void **)(param_4 + 0x38) = pvVar4;
        if (pvVar4 != (void *)0x0) {
          iVar8 = 0;
          *(size_t *)(param_4 + 0x30) = _Count;
          if (_Count != 0) {
            do {
              lVar9 = 0;
              if (uVar10 != 0) {
                do {
                  uVar5 = *(undefined8 *)(*(longlong *)(param_4 + 0x60) + lVar9 * 8);
                  if (*(longlong *)(*(longlong *)(param_4 + 0x58) + lVar9 * 8) == 1) {
                    uVar7 = FUN_14001fe20(param_1,param_2,param_4,uVar5,param_3,iVar8,param_5);
                    uVar3 = (uint)uVar7;
                    if (uVar3 != 0) {
                      return uVar7;
                    }
                  }
                  else {
                    FUN_140026d00(lVar1,param_2,param_3,*(undefined1 *)(param_4 + 10),uVar5);
                  }
                  lVar9 = lVar9 + 1;
                  _Count = local_30;
                } while (lVar9 < (longlong)uVar10);
              }
              iVar8 = iVar8 + 1;
            } while ((ulonglong)(longlong)iVar8 < _Count);
          }
          return (ulonglong)uVar3;
        }
        uVar11 = 0x30a;
      }
      FUN_14001bb80(lVar1,param_5,1,0,"_dwarf_lineno_init_include_directories_dwarf5",uVar11);
      return 1;
    }
    uVar11 = 0x2ea;
  }
  FUN_14001bb80(lVar1,param_5,1,0,"_dwarf_lineno_init_include_directories_dwarf5",uVar11);
  return 1;
}


/* ================= FUN_140021460 @ 140021460  size=417  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001f580 */
/* CALLEES: free */

void FUN_140021460(longlong param_1)

{
  longlong *plVar1;
  void *_Memory;
  undefined8 *puVar2;
  undefined8 *puVar3;
  void *pvVar4;
  void *pvVar5;
  undefined8 *_Memory_00;
  undefined8 *puVar6;
  void *pvVar7;
  void *pvVar8;
  longlong lVar9;
  int iVar10;
  
  _Memory = *(void **)(param_1 + 0x278);
  if (_Memory != (void *)0x0) {
    plVar1 = (longlong *)((longlong)_Memory + 0x80);
    _Memory_00 = (undefined8 *)*plVar1;
    while (_Memory_00 != (undefined8 *)0x0) {
      puVar2 = (undefined8 *)*plVar1;
      puVar3 = (undefined8 *)_Memory_00[5];
      if (puVar2 == _Memory_00) {
        lVar9 = puVar2[5];
        *plVar1 = lVar9;
        if (lVar9 == 0) {
          *(longlong **)((longlong)_Memory + 0x88) = plVar1;
        }
      }
      else {
        for (puVar6 = (undefined8 *)puVar2[5]; puVar6 != _Memory_00;
            puVar6 = (undefined8 *)puVar6[5]) {
          puVar2 = puVar6;
        }
        lVar9 = *(longlong *)(puVar2[5] + 0x28);
        puVar2[5] = lVar9;
        if (lVar9 == 0) {
          *(undefined8 **)((longlong)_Memory + 0x88) = puVar2 + 5;
        }
      }
      if ((void *)*_Memory_00 != (void *)0x0) {
        free((void *)*_Memory_00);
      }
      free(_Memory_00);
      _Memory_00 = puVar3;
    }
    plVar1 = (longlong *)((longlong)_Memory + 0xa0);
    pvVar8 = (void *)*plVar1;
    while (pvVar8 != (void *)0x0) {
      pvVar4 = (void *)*plVar1;
      pvVar5 = *(void **)((longlong)pvVar8 + 0x48);
      if (pvVar4 == pvVar8) {
        lVar9 = *(longlong *)((longlong)pvVar4 + 0x48);
        *plVar1 = lVar9;
        if (lVar9 == 0) {
          *(longlong **)((longlong)_Memory + 0xa8) = plVar1;
        }
      }
      else {
        for (pvVar7 = *(void **)((longlong)pvVar4 + 0x48); pvVar7 != pvVar8;
            pvVar7 = *(void **)((longlong)pvVar7 + 0x48)) {
          pvVar4 = pvVar7;
        }
        lVar9 = *(longlong *)(*(longlong *)((longlong)pvVar4 + 0x48) + 0x48);
        *(longlong *)((longlong)pvVar4 + 0x48) = lVar9;
        if (lVar9 == 0) {
          *(longlong **)((longlong)_Memory + 0xa8) = (longlong *)((longlong)pvVar4 + 0x48);
        }
      }
      free(pvVar8);
      pvVar8 = pvVar5;
    }
    pvVar8 = *(void **)((longlong)_Memory + 0x28);
    if (pvVar8 != (void *)0x0) {
      iVar10 = 0;
      if (*(longlong *)((longlong)_Memory + 0x30) != 0) {
        lVar9 = 0;
        do {
          free(*(void **)(*(longlong *)((longlong)_Memory + 0x28) + lVar9));
          iVar10 = iVar10 + 1;
          lVar9 = lVar9 + 8;
        } while ((ulonglong)(longlong)iVar10 < *(ulonglong *)((longlong)_Memory + 0x30));
        pvVar8 = *(void **)((longlong)_Memory + 0x28);
      }
      free(pvVar8);
    }
    free(_Memory);
    *(undefined8 *)(param_1 + 0x278) = 0;
  }
  return;
}


/* ================= FUN_140021610 @ 140021610  size=267  callers=1 callees=3 ================= */
/* CALLERS: FUN_140020a70 */
/* CALLEES: FUN_14001bb80, FUN_140025a70, FUN_140026d00 */

undefined8
FUN_140021610(longlong *param_1,longlong param_2,longlong param_3,longlong param_4,longlong *param_5
             ,ulonglong *param_6,undefined8 param_7)

{
  longlong lVar1;
  ulonglong *puVar2;
  byte bVar3;
  ulonglong uVar4;
  undefined8 uVar5;
  undefined4 uVar6;
  
  puVar2 = param_6;
  if (param_6 == (ulonglong *)0x0) {
    uVar6 = 0x3cd;
  }
  else {
    lVar1 = *param_1;
    if (lVar1 != 0) {
      if (param_4 == 5) {
        uVar5 = 2;
      }
      else {
        if (param_4 != 0xb) {
          if (param_4 != 0xf) {
            FUN_140026d00(lVar1,param_2,param_5,*(undefined1 *)(param_3 + 10),param_4);
            return 0;
          }
          param_6 = (ulonglong *)(*(longlong *)(param_2 + 8) + *param_5);
          uVar4 = FUN_140025a70(&param_6);
          *puVar2 = uVar4;
          *param_5 = (longlong)param_6 - *(longlong *)(param_2 + 8);
          return 0;
        }
        uVar5 = 1;
      }
      bVar3 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_5,uVar5);
      *puVar2 = (ulonglong)bVar3;
      return 0;
    }
    uVar6 = 0x3d3;
  }
  FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_lineno_read_directory_index_dwarf5",uVar6);
  return 0x1c;
}


/* ================= FUN_140021720 @ 140021720  size=348  callers=1 callees=3 ================= */
/* CALLERS: FUN_140020a70 */
/* CALLEES: FUN_14001bb80, FUN_140025a70, FUN_140026d00 */

undefined4
FUN_140021720(longlong *param_1,longlong param_2,longlong param_3,longlong param_4,longlong *param_5
             ,ulonglong *param_6,undefined8 param_7)

{
  longlong lVar1;
  ulonglong *puVar2;
  ulonglong uVar3;
  undefined8 uVar4;
  
  puVar2 = param_6;
  if (param_6 == (ulonglong *)0x0) {
    FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_lineno_read_file_mod_time_dwarf5",0x40e);
    return 0x1c;
  }
  lVar1 = *param_1;
  if (lVar1 != 0) {
    if (param_4 == 6) {
      uVar4 = 4;
    }
    else {
      if (param_4 != 7) {
        if (param_4 == 9) {
          FUN_140026d00(lVar1,param_2,param_5,*(undefined1 *)(param_3 + 10),9);
          *puVar2 = 0;
        }
        else if (param_4 == 0xf) {
          param_6 = (ulonglong *)(*param_5 + *(longlong *)(param_2 + 8));
          uVar3 = FUN_140025a70(&param_6);
          *puVar2 = uVar3;
          *param_5 = (longlong)param_6 - *(longlong *)(param_2 + 8);
          return 0;
        }
        FUN_140026d00(lVar1,param_2,param_5,*(undefined1 *)(param_3 + 10),param_4);
        return 0;
      }
      uVar4 = 8;
    }
    uVar3 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_5,uVar4);
    *puVar2 = uVar3 & 0xff;
    return 0;
  }
  FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_lineno_read_file_mod_time_dwarf5",0x414);
  return 0x1c;
}


/* ================= FUN_140021880 @ 140021880  size=601  callers=1 callees=7 ================= */
/* CALLERS: FUN_140020a70 */
/* CALLEES: FUN_14001bb80, FUN_140026240, FUN_1400262d0, FUN_140026310, FUN_1400263a0, FUN_140026450, FUN_140026d00 */

undefined4
FUN_140021880(longlong *param_1,undefined8 param_2,longlong param_3,undefined8 param_4,
             undefined8 param_5,longlong param_6,undefined8 param_7)

{
  longlong lVar1;
  int iVar2;
  undefined8 uVar3;
  
  if (param_6 == 0) {
    FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_lineno_read_file_name_dwarf5",0x369);
    return 0x1c;
  }
  lVar1 = *param_1;
  if (lVar1 == 0) {
    FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_lineno_read_file_name_dwarf5",0x36f);
    return 0x1c;
  }
  switch(param_4) {
  case 8:
    iVar2 = FUN_1400262d0(lVar1,param_2,param_5,param_1,param_6,param_7);
    if (iVar2 != 0) {
      uVar3 = 0x37a;
      goto LAB_140021a7b;
    }
  case 0xe:
    iVar2 = FUN_140026310(lVar1,param_2,param_5,param_1,0,param_6,param_7);
    if (iVar2 != 0) {
      uVar3 = 0x381;
LAB_140021a7b:
      FUN_14001bb80(lVar1,param_7,0x1c,0,"_dwarf_lineno_read_file_name_dwarf5",uVar3);
      return 0x1c;
    }
    break;
  default:
    FUN_140026d00(lVar1,param_2,param_5,*(undefined1 *)(param_3 + 10),param_4);
    break;
  case 0x1a:
    iVar2 = FUN_1400263a0(lVar1,param_2,param_5,param_1,0,0,param_6,param_7);
    if (iVar2 != 0) {
      uVar3 = 0x394;
      goto LAB_140021a7b;
    }
    break;
  case 0x1f:
    iVar2 = FUN_140026240(lVar1,param_2,param_5,param_1,0,param_6,param_7);
    if (iVar2 != 0) {
      uVar3 = 0x389;
      goto LAB_140021a7b;
    }
    break;
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
    iVar2 = FUN_140026450(lVar1,param_2,param_5,param_1,(int)param_4 + -0x24,0,0,param_6,param_7);
    if (iVar2 != 0) {
      uVar3 = 0x3a5;
      goto LAB_140021a7b;
    }
  }
  return 0;
}


/* ================= FUN_140021b20 @ 140021b20  size=306  callers=1 callees=3 ================= */
/* CALLERS: FUN_140020a70 */
/* CALLEES: FUN_14001bb80, FUN_140025a70, FUN_140026d00 */

undefined8
FUN_140021b20(longlong *param_1,longlong param_2,longlong param_3,longlong param_4,longlong *param_5
             ,ulonglong *param_6,undefined8 param_7)

{
  longlong lVar1;
  ulonglong *puVar2;
  byte bVar3;
  ulonglong uVar4;
  undefined8 uVar5;
  undefined4 uVar6;
  
  puVar2 = param_6;
  if (param_6 == (ulonglong *)0x0) {
    uVar6 = 0x455;
  }
  else {
    lVar1 = *param_1;
    if (lVar1 != 0) {
      if (param_4 == 5) {
        uVar5 = 2;
      }
      else if (param_4 == 6) {
        uVar5 = 4;
      }
      else if (param_4 == 7) {
        uVar5 = 8;
      }
      else {
        if (param_4 != 0xb) {
          if (param_4 != 0xf) {
            FUN_140026d00(lVar1,param_2,param_5,*(undefined1 *)(param_3 + 10),param_4);
            return 0;
          }
          param_6 = (ulonglong *)(*param_5 + *(longlong *)(param_2 + 8));
          uVar4 = FUN_140025a70(&param_6);
          *puVar2 = uVar4;
          *param_5 = (longlong)param_6 - *(longlong *)(param_2 + 8);
          return 0;
        }
        uVar5 = 1;
      }
      bVar3 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_5,uVar5);
      *puVar2 = (ulonglong)bVar3;
      return 0;
    }
    uVar6 = 0x45b;
  }
  FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_lineno_read_file_size_dwarf5",uVar6);
  return 0x1c;
}


/* ================= FUN_140021c60 @ 140021c60  size=279  callers=1 callees=2 ================= */
/* CALLERS: FUN_140020a70 */
/* CALLEES: FUN_14001bb80, FUN_140026d00 */

undefined4
FUN_140021c60(longlong *param_1,longlong param_2,longlong param_3,longlong param_4,
             undefined8 param_5,ulonglong *param_6,ulonglong *param_7,undefined8 param_8)

{
  longlong lVar1;
  byte bVar2;
  undefined4 uVar3;
  
  if ((param_6 != (ulonglong *)0x0) && (param_7 != (ulonglong *)0x0)) {
    lVar1 = *param_1;
    if (lVar1 == 0) {
      FUN_14001bb80(0,param_8,0x1c,0,"_dwarf_lineno_read_md5_dwarf5",0x4a8);
      uVar3 = 0x1c;
    }
    else if (param_4 == 0x1e) {
      bVar2 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_5,8);
      *param_6 = (ulonglong)bVar2;
      bVar2 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_5,8);
      uVar3 = 0;
      *param_7 = (ulonglong)bVar2;
    }
    else {
      FUN_140026d00(lVar1,param_2,param_5,*(undefined1 *)(param_3 + 10),param_4);
      uVar3 = 0;
    }
    return uVar3;
  }
  FUN_14001bb80(0,param_8,0x1c,0,"_dwarf_lineno_read_md5_dwarf5",0x4a2);
  return 0x1c;
}


/* ================= FUN_140021d80 @ 140021d80  size=1120  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400202a0 */
/* CALLEES: FUN_14001bb80, FUN_14001fc00, FUN_140025a00, FUN_140025a70, calloc, free */

int FUN_140021d80(longlong *param_1,longlong param_2,byte *param_3,byte *param_4,undefined8 param_5,
                 undefined4 param_6,undefined8 param_7)

{
  longlong *plVar1;
  byte bVar2;
  longlong lVar3;
  void *pvVar4;
  void *pvVar5;
  void *pvVar6;
  int iVar7;
  uint uVar8;
  longlong lVar9;
  longlong lVar10;
  void *pvVar11;
  undefined8 *puVar12;
  longlong lVar13;
  undefined8 uVar14;
  undefined4 uVar15;
  byte *local_res18;
  byte *local_res20;
  undefined8 local_48;
  undefined8 local_40;
  
  local_res18 = param_3;
  local_res20 = param_4;
  if (param_1 == (longlong *)0x0) {
    FUN_14001bb80(0,param_7,0x1c,0,"_dwarf_lineno_run_program",0x15f);
    return 0x1c;
  }
  lVar3 = *param_1;
  if (lVar3 == 0) {
    return 0x1c;
  }
  lVar10 = 0;
  uVar8 = (uint)*(byte *)(param_2 + 0x19);
  uVar14 = 1;
  lVar13 = 1;
  uVar15 = 0;
  local_40 = 1;
  local_48 = 0;
  if (param_3 < param_4) {
    do {
      bVar2 = *local_res18;
      if (bVar2 == 0) {
        local_res18 = local_res18 + 1;
        lVar9 = FUN_140025a70(&local_res18);
        bVar2 = *local_res18;
        if (bVar2 == 1) {
          local_res18 = local_res18 + 1;
          lVar10 = 0;
          uVar8 = (uint)*(byte *)(param_2 + 0x19);
          local_40 = 1;
          lVar13 = 1;
          local_48 = 0;
          uVar15 = 0;
          uVar14 = 1;
        }
        else if (bVar2 == 2) {
          local_res18 = local_res18 + 1;
          lVar10 = (**(code **)(lVar3 + 0x210))(&local_res18,(char)param_1[9]);
        }
        else if (bVar2 == 3) {
          local_res18 = local_res18 + 1;
          iVar7 = FUN_14001fc00(param_2,&local_res18,param_5,param_6,param_7,lVar3);
          uVar14 = local_40;
          if (iVar7 != 0) goto LAB_14002215d;
        }
        else {
          if (bVar2 == 4) {
            local_res18 = local_res18 + 1;
            goto switchD_140021f3f_caseD_c;
          }
          local_res18 = local_res18 + lVar9;
        }
        goto switchD_140021f3f_caseD_a;
      }
      if (bVar2 < *(byte *)(param_2 + 0x1c)) {
        local_res18 = local_res18 + 1;
        switch((uint)bVar2) {
        case 1:
          pvVar11 = calloc(1,0x50);
          if (pvVar11 == (void *)0x0) {
            uVar14 = 0x1bd;
            goto LAB_140022139;
          }
          *(longlong *)((longlong)pvVar11 + 8) = param_2;
          puVar12 = (undefined8 *)((longlong)pvVar11 + 0x48);
          *(longlong *)((longlong)pvVar11 + 0x10) = lVar10;
          *(undefined8 *)((longlong)pvVar11 + 0x18) = 0;
          *(undefined8 *)((longlong)pvVar11 + 0x20) = uVar14;
          *(longlong *)((longlong)pvVar11 + 0x28) = lVar13;
          *(undefined4 *)((longlong)pvVar11 + 0x38) = uVar15;
          *(undefined8 *)((longlong)pvVar11 + 0x30) = local_48;
          *(uint *)((longlong)pvVar11 + 0x3c) = uVar8;
          *(undefined4 *)((longlong)pvVar11 + 0x40) = 0;
          *puVar12 = 0;
          **(undefined8 **)(param_2 + 0xa8) = pvVar11;
          *(longlong *)(param_2 + 0x98) = *(longlong *)(param_2 + 0x98) + 1;
          goto LAB_1400220e1;
        case 2:
          lVar9 = FUN_140025a70(&local_res18);
          lVar10 = lVar10 + lVar9 * (ulonglong)*(byte *)(param_2 + 0x18);
          break;
        case 3:
          lVar9 = FUN_140025a00(&local_res18);
          lVar13 = lVar13 + lVar9;
          break;
        case 4:
          local_40 = FUN_140025a70(&local_res18);
          uVar14 = local_40;
          break;
        case 5:
          local_48 = FUN_140025a70(&local_res18);
          break;
        case 6:
          uVar8 = (uint)(uVar8 == 0);
          break;
        case 7:
          uVar15 = 1;
          break;
        case 8:
          lVar10 = lVar10 + (int)(((int)(0xff - (uint)*(byte *)(param_2 + 0x1c)) /
                                  (int)(uint)*(byte *)(param_2 + 0x1b)) *
                                 (uint)*(byte *)(param_2 + 0x18));
          break;
        case 9:
          lVar9 = (**(code **)(lVar3 + 0x210))(&local_res18,2);
          lVar10 = lVar10 + lVar9;
          break;
        case 0xc:
switchD_140021f3f_caseD_c:
          FUN_140025a70(&local_res18);
        }
      }
      else {
        iVar7 = (uint)bVar2 - (uint)*(byte *)(param_2 + 0x1c);
        lVar13 = lVar13 + ((int)*(char *)(param_2 + 0x1a) +
                          iVar7 % (int)(uint)*(byte *)(param_2 + 0x1b));
        lVar10 = lVar10 + (int)((iVar7 / (int)(uint)*(byte *)(param_2 + 0x1b)) *
                               (uint)*(byte *)(param_2 + 0x18));
        pvVar11 = calloc(1,0x50);
        if (pvVar11 == (void *)0x0) {
          uVar14 = 0x1ec;
LAB_140022139:
          iVar7 = 1;
          FUN_14001bb80(lVar3,param_7,1,0,"_dwarf_lineno_run_program",uVar14);
LAB_14002215d:
          plVar1 = (longlong *)(param_2 + 0xa0);
          pvVar11 = (void *)*plVar1;
          while (pvVar11 != (void *)0x0) {
            pvVar4 = (void *)*plVar1;
            pvVar5 = *(void **)((longlong)pvVar11 + 0x48);
            pvVar6 = *(void **)((longlong)pvVar4 + 0x48);
            if (pvVar4 == pvVar11) {
              *plVar1 = (longlong)pvVar6;
              if (pvVar6 == (void *)0x0) {
                *(longlong **)(param_2 + 0xa8) = plVar1;
              }
            }
            else {
              for (; pvVar6 != pvVar11; pvVar6 = *(void **)((longlong)pvVar6 + 0x48)) {
                pvVar4 = pvVar6;
              }
              lVar3 = *(longlong *)(*(longlong *)((longlong)pvVar4 + 0x48) + 0x48);
              *(longlong *)((longlong)pvVar4 + 0x48) = lVar3;
              if (lVar3 == 0) {
                *(longlong **)(param_2 + 0xa8) = (longlong *)((longlong)pvVar4 + 0x48);
              }
            }
            free(pvVar11);
            pvVar11 = pvVar5;
          }
          return iVar7;
        }
        *(longlong *)((longlong)pvVar11 + 8) = param_2;
        puVar12 = (undefined8 *)((longlong)pvVar11 + 0x48);
        *(longlong *)((longlong)pvVar11 + 0x10) = lVar10;
        *(undefined8 *)((longlong)pvVar11 + 0x18) = 0;
        *(undefined8 *)((longlong)pvVar11 + 0x20) = uVar14;
        *(longlong *)((longlong)pvVar11 + 0x28) = lVar13;
        *(undefined4 *)((longlong)pvVar11 + 0x38) = uVar15;
        *(undefined8 *)((longlong)pvVar11 + 0x30) = local_48;
        *(uint *)((longlong)pvVar11 + 0x3c) = uVar8;
        *(undefined4 *)((longlong)pvVar11 + 0x40) = 0;
        *puVar12 = 0;
        **(undefined8 **)(param_2 + 0xa8) = pvVar11;
        *(longlong *)(param_2 + 0x98) = *(longlong *)(param_2 + 0x98) + 1;
        local_res18 = local_res18 + 1;
LAB_1400220e1:
        uVar15 = 0;
        *(undefined8 **)(param_2 + 0xa8) = puVar12;
      }
switchD_140021f3f_caseD_a:
    } while (local_res18 < local_res20);
  }
  return 0;
}


/* ================= FUN_140022210 @ 140022210  size=166  callers=1 callees=2 ================= */
/* CALLERS: FUN_140019250 */
/* CALLEES: FUN_14001bb80, FUN_1400228f0 */

undefined8 FUN_140022210(longlong param_1,longlong param_2,undefined8 param_3)

{
  longlong *plVar1;
  undefined8 uVar2;
  undefined4 uVar3;
  
  if (param_2 == 0) {
    uVar3 = 0x1f1;
  }
  else {
    if (*(longlong *)(param_2 + 0x20) == 0) {
      return 0;
    }
    if (param_1 == 0) {
      uVar3 = 0x1fe;
    }
    else {
      plVar1 = *(longlong **)(param_1 + 0x50);
      if (plVar1 == (longlong *)0x0) {
        uVar3 = 0x203;
      }
      else {
        if (*plVar1 != 0) {
          uVar2 = FUN_1400228f0(*plVar1,plVar1,param_2 + 0x40,*(undefined8 *)(param_2 + 0x28),
                                *(longlong *)(param_2 + 0x20),(char)plVar1[9],param_3);
          return uVar2;
        }
        uVar3 = 0x208;
      }
    }
  }
  FUN_14001bb80(0,param_3,0x1c,0,"_dwarf_loc_add",uVar3);
  return 0x1c;
}


/* ================= FUN_1400222c0 @ 1400222c0  size=897  callers=1 callees=6 ================= */
/* CALLERS: FUN_140022790 */
/* CALLEES: FUN_14001bb50, FUN_14001bb80, FUN_14001bd50, FUN_140025a00, FUN_140025a70, FUN_140025c90 */

int FUN_1400222c0(longlong param_1,undefined8 param_2,longlong param_3,undefined1 param_4,
                 byte *param_5,int param_6,undefined8 param_7,undefined8 param_8)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  ulonglong uVar4;
  byte *pbVar5;
  undefined8 uVar6;
  ulonglong uVar7;
  int iVar8;
  byte *pbVar9;
  longlong lVar10;
  byte *pbVar11;
  byte *pbVar12;
  longlong local_res8;
  byte *local_50;
  byte *local_48;
  byte *local_40;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,0,0x1c,0,"_dwarf_loc_fill_loc",0x2b);
    iVar8 = -1;
  }
  else {
    iVar8 = 0;
    pbVar9 = param_5 + param_6;
    local_40 = param_5;
    if (param_5 < pbVar9) {
      local_res8 = 0;
      lVar10 = 0;
      do {
        if ((int)param_5 == -1) {
          return iVar8;
        }
        uVar7 = 0;
        pbVar5 = (byte *)0x0;
        pbVar11 = (byte *)0x0;
        if ((((param_3 != 0) && (*(longlong *)(param_3 + 0x18) != 0)) && (-1 < local_res8)) &&
           (iVar8 < (int)(uint)*(ushort *)(param_3 + 0x10))) {
          *(byte *)(*(longlong *)(param_3 + 0x18) + lVar10) = *param_5;
          *(longlong *)(*(longlong *)(param_3 + 0x18) + 0x18 + lVar10) =
               (longlong)param_5 - (longlong)local_40;
        }
        bVar2 = *param_5;
        pbVar1 = param_5 + 1;
        uVar4 = uVar7;
        pbVar12 = pbVar5;
        switch(bVar2) {
        case 3:
          param_5 = pbVar1;
          uVar4 = (**(code **)(param_1 + 0x210))(&param_5,param_4);
          pbVar5 = pbVar11;
          pbVar1 = param_5;
          break;
        default:
          param_5 = pbVar1;
          uVar6 = FUN_14001bb50(bVar2);
          FUN_14001bd50(param_1,0,10,0,"_dwarf_loc_fill_loc",400,uVar6,0,0);
          return -1;
        case 6:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
        case 0x1a:
        case 0x1b:
        case 0x1c:
        case 0x1d:
        case 0x1e:
        case 0x1f:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
        case 0x3a:
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e:
        case 0x3f:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4a:
        case 0x4b:
        case 0x4c:
        case 0x4d:
        case 0x4e:
        case 0x4f:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5a:
        case 0x5b:
        case 0x5c:
        case 0x5d:
        case 0x5e:
        case 0x5f:
        case 0x60:
        case 0x61:
        case 0x62:
        case 99:
        case 100:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x96:
        case 0x97:
        case 0x9b:
        case 0x9c:
        case 0x9f:
        case 0xe0:
        case 0xf0:
          break;
        case 8:
        case 9:
        case 0x15:
        case 0x94:
        case 0x95:
          uVar4 = (ulonglong)*pbVar1;
          pbVar1 = param_5 + 2;
          break;
        case 10:
        case 0xb:
        case 0x28:
        case 0x2f:
        case 0x98:
          param_5 = pbVar1;
          uVar4 = (**(code **)(param_1 + 0x210))(&param_5,2);
          pbVar5 = pbVar11;
          pbVar1 = param_5;
          break;
        case 0xc:
        case 0xd:
        case 0x99:
        case 0xfa:
          param_5 = pbVar1;
          uVar4 = (**(code **)(param_1 + 0x210))(&param_5,4);
          pbVar1 = param_5;
          break;
        case 0xe:
        case 0xf:
          param_5 = pbVar1;
          uVar4 = (**(code **)(param_1 + 0x210))(&param_5,8);
          pbVar5 = pbVar11;
          pbVar1 = param_5;
          break;
        case 0x10:
        case 0x23:
        case 0x90:
        case 0x93:
        case 0xa8:
        case 0xa9:
        case 0xf7:
        case 0xf9:
          param_5 = pbVar1;
          uVar4 = FUN_140025a70(&param_5);
          pbVar1 = param_5;
          break;
        case 0x11:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7a:
        case 0x7b:
        case 0x7c:
        case 0x7d:
        case 0x7e:
        case 0x7f:
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8a:
        case 0x8b:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0x8f:
        case 0x91:
          param_5 = pbVar1;
          uVar4 = FUN_140025a00(&param_5);
          pbVar5 = pbVar11;
          pbVar1 = param_5;
          break;
        case 0x92:
          param_5 = pbVar1;
          uVar4 = FUN_140025a70(&param_5);
          goto LAB_140022597;
        case 0x9a:
          param_5 = pbVar1;
          uVar4 = (**(code **)(param_1 + 0x210))(&param_5,*(undefined4 *)(param_1 + 0xf8));
          pbVar5 = pbVar11;
          pbVar1 = param_5;
          break;
        case 0x9d:
        case 0xa5:
        case 0xf5:
          param_5 = pbVar1;
          uVar4 = FUN_140025a70(&param_5);
          pbVar5 = (byte *)FUN_140025a70(&param_5);
          pbVar1 = param_5;
          break;
        case 0x9e:
        case 0xa3:
        case 0xf3:
          param_5 = pbVar1;
          uVar4 = FUN_140025a70(&param_5);
          pbVar5 = param_5;
          pbVar1 = param_5 + uVar4;
          break;
        case 0xa0:
        case 0xf2:
          param_5 = pbVar1;
          uVar4 = (**(code **)(param_1 + 0x210))(&param_5,*(undefined4 *)(param_1 + 0xf8));
LAB_140022597:
          pbVar5 = (byte *)FUN_140025a00(&param_5);
          pbVar1 = param_5;
          break;
        case 0xa1:
        case 0xfb:
          param_5 = pbVar1;
          uVar4 = FUN_140025a70(&param_5);
          iVar3 = FUN_140025c90(param_1,param_2,uVar4,&local_50,param_8);
          if (iVar3 == 0) {
            uVar7 = uVar4;
            pbVar12 = local_50;
          }
LAB_1400224df:
          uVar4 = uVar7;
          pbVar5 = pbVar12;
          pbVar1 = param_5;
          break;
        case 0xa2:
        case 0xfc:
          param_5 = pbVar1;
          uVar4 = FUN_140025a70(&param_5);
          iVar3 = FUN_140025c90(param_1,param_2,uVar4,&local_48,param_8);
          pbVar5 = local_48;
          pbVar1 = param_5;
          if (iVar3 != 0) goto LAB_1400224df;
          break;
        case 0xa4:
        case 0xf4:
          param_5 = pbVar1;
          FUN_140025a70(&param_5);
          uVar4 = (**(code **)(param_1 + 0x210))(&param_5,1);
          pbVar5 = param_5;
          pbVar1 = param_5 + uVar4;
          break;
        case 0xa6:
        case 0xa7:
        case 0xf6:
          param_5 = pbVar1;
          uVar4 = (**(code **)(param_1 + 0x210))(&param_5,1);
          pbVar5 = (byte *)FUN_140025a70(&param_5);
          pbVar1 = param_5;
        }
        param_5 = pbVar1;
        if (((param_3 != 0) && (*(longlong *)(param_3 + 0x18) != 0)) &&
           ((-1 < local_res8 && (iVar8 < (int)(uint)*(ushort *)(param_3 + 0x10))))) {
          *(ulonglong *)(*(longlong *)(param_3 + 0x18) + 8 + lVar10) = uVar4;
          *(byte **)(*(longlong *)(param_3 + 0x18) + 0x10 + lVar10) = pbVar5;
        }
        local_res8 = local_res8 + 1;
        iVar8 = iVar8 + 1;
        lVar10 = lVar10 + 0x20;
      } while (param_5 < pbVar9);
    }
  }
  return iVar8;
}


/* ================= FUN_140022790 @ 140022790  size=343  callers=3 callees=3 ================= */
/* CALLERS: FUN_1400228f0, FUN_140022d70, FUN_140023b50 */
/* CALLEES: FUN_14001bb80, FUN_1400222c0, calloc */

undefined8
FUN_140022790(longlong param_1,undefined8 param_2,longlong param_3,longlong param_4,
             undefined4 param_5,undefined8 param_6,undefined1 param_7,undefined8 param_8)

{
  int iVar1;
  undefined8 uVar2;
  void *pvVar3;
  
  if (((param_1 != 0) && (param_3 != 0)) && (param_4 != 0)) {
    iVar1 = FUN_1400222c0(param_1,param_2,0,param_7,param_4,param_5,param_6,param_8);
    if (iVar1 < 0) {
      uVar2 = 10;
    }
    else {
      *(short *)(param_3 + 0x10) = (short)iVar1;
      if (0 < iVar1) {
        pvVar3 = calloc((longlong)iVar1,0x20);
        *(void **)(param_3 + 0x18) = pvVar3;
        if (pvVar3 == (void *)0x0) {
          FUN_14001bb80(param_1,param_8,1,0,"_dwarf_loc_fill_locdesc",0x1c5);
          return 1;
        }
        FUN_1400222c0(param_1,param_2,param_3,param_7,param_4,param_5,param_6,param_8);
      }
      uVar2 = 0;
    }
    return uVar2;
  }
  FUN_14001bb80(param_1,param_8,0x1c,0,"_dwarf_loc_fill_locdesc",0x1ad);
  return 0x1c;
}


/* ================= FUN_1400228f0 @ 1400228f0  size=237  callers=2 callees=4 ================= */
/* CALLERS: FUN_1400191a0, FUN_140022210 */
/* CALLEES: FUN_14001bb80, FUN_140022790, calloc, free */

int FUN_1400228f0(undefined8 param_1,undefined8 param_2,undefined8 *param_3,undefined8 param_4,
                 undefined8 param_5,undefined1 param_6,undefined8 param_7)

{
  int iVar1;
  undefined8 *_Memory;
  undefined8 in_stack_ffffffffffffffe0;
  undefined4 uVar2;
  
  uVar2 = (undefined4)((ulonglong)in_stack_ffffffffffffffe0 >> 0x20);
  _Memory = calloc(1,0x20);
  if (_Memory == (undefined8 *)0x0) {
    FUN_14001bb80(param_1,param_7,1,0,"_dwarf_loc_fill_locexpr",CONCAT44(uVar2,0x1d6));
    iVar1 = 1;
  }
  else {
    _Memory[1] = 0xffffffffffffffff;
    *_Memory = 0;
    _Memory[3] = 0;
    *(undefined2 *)(_Memory + 2) = 0;
    iVar1 = FUN_140022790(param_1,param_2,_Memory,param_4,param_5,0,param_6,param_7);
    if (iVar1 == 0) {
      if (param_3 != (undefined8 *)0x0) {
        *param_3 = _Memory;
      }
    }
    else {
      free(_Memory);
    }
  }
  return iVar1;
}


/* ================= FUN_1400229e0 @ 1400229e0  size=33  callers=1 callees=0 ================= */
/* CALLERS: FUN_140022a40 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1400229e0(ulonglong param_1,undefined8 param_2)

{
  if ((DAT_14083b030 != 0) && (param_1 < DAT_14083b020)) {
    _DAT_14083b028 = _DAT_14083b028 + 1;
    *(undefined8 *)(DAT_14083b030 + param_1 * 8) = param_2;
  }
  return;
}


/* ================= FUN_140022a10 @ 140022a10  size=37  callers=1 callees=0 ================= */
/* CALLERS: FUN_140023020 */

void FUN_140022a10(ulonglong param_1,undefined8 *param_2)

{
  if ((DAT_14083b030 != 0) && (param_1 < DAT_14083b020)) {
    *param_2 = *(undefined8 *)(DAT_14083b030 + param_1 * 8);
    return;
  }
  *param_2 = 0;
  return;
}


/* ================= FUN_140022a40 @ 140022a40  size=806  callers=1 callees=7 ================= */
/* CALLERS: FUN_140023020 */
/* CALLEES: FUN_140019ae0, FUN_14001bb80, FUN_14001bd50, FUN_1400229e0, FUN_140022d70, FUN_140026bb0, calloc */

int FUN_140022a40(longlong param_1,longlong param_2,ulonglong param_3,undefined8 *param_4,
                 undefined8 param_5)

{
  int iVar1;
  longlong lVar2;
  longlong *plVar3;
  longlong lVar4;
  uint uVar5;
  ulonglong uVar7;
  ulonglong local_res8;
  longlong local_res10;
  ulonglong local_res18;
  undefined8 in_stack_ffffffffffffffa0;
  undefined4 uVar9;
  undefined8 uVar8;
  ulonglong uVar6;
  
  uVar9 = (undefined4)((ulonglong)in_stack_ffffffffffffffa0 >> 0x20);
  local_res10 = param_2;
  local_res18 = param_3;
  if ((param_1 == 0) || (param_2 == 0)) {
    FUN_14001bb80(param_1,param_5,0x1c,0,"_dwarf_loclist_add",CONCAT44(uVar9,0xb9));
    return 0x1c;
  }
  lVar2 = FUN_140026bb0(param_1,2);
  if (lVar2 == 0) {
    FUN_14001bd50(param_1,param_5,0x1d,0,"_dwarf_loclist_add",CONCAT44(uVar9,0xc0),".debug_loc",0,0)
    ;
    iVar1 = 0x23;
  }
  else if (local_res18 < *(ulonglong *)(lVar2 + 0x18)) {
    plVar3 = (longlong *)FUN_140019ae0(0x30);
    if (plVar3 == (longlong *)0x0) {
      FUN_14001bb80(param_1,param_5,1,0,"_dwarf_loclist_add",CONCAT44(uVar9,0xcb));
      iVar1 = 1;
    }
    else {
      plVar3[2] = local_res18;
      uVar6 = 0;
      iVar1 = FUN_140022d70(param_1,param_2,lVar2,&local_res18,0,&local_res8,0,param_5);
      if (iVar1 == 0) {
        *(int *)(plVar3 + 1) = (int)local_res8;
        lVar4 = FUN_140019ae0((int)local_res8 * 8);
        *plVar3 = lVar4;
        if (lVar4 == 0) {
          uVar8 = 0xe0;
LAB_140022c00:
          iVar1 = 1;
          FUN_14001bb80(param_1,param_5,1,0,"_dwarf_loclist_add",uVar8);
        }
        else {
          uVar7 = uVar6;
          if (local_res8 != 0) {
            do {
              lVar4 = FUN_140019ae0(0x20);
              *(longlong *)(uVar7 + *plVar3) = lVar4;
              if (lVar4 == 0) {
                uVar8 = 0xea;
                goto LAB_140022c00;
              }
              uVar5 = (int)uVar6 + 1;
              uVar6 = (ulonglong)uVar5;
              uVar7 = uVar7 + 8;
            } while ((ulonglong)(longlong)(int)uVar5 < local_res8);
          }
          local_res18 = plVar3[2];
          iVar1 = FUN_140022d70(param_1,local_res10,lVar2,&local_res18,*plVar3,0,plVar3 + 3,param_5)
          ;
          if (iVar1 == 0) {
            if (DAT_14083b030 == (void *)0x0) {
              DAT_14083b030 = calloc(1,*(longlong *)(lVar2 + 0x18) << 3);
              DAT_14083b020 = *(undefined8 *)(lVar2 + 0x18);
            }
            FUN_1400229e0(plVar3[2],plVar3);
            if (param_4 != (undefined8 *)0x0) {
              *param_4 = plVar3;
            }
            iVar1 = 0;
          }
        }
      }
    }
  }
  else {
    FUN_14001bb80(param_1,param_5,0x1c,0,"_dwarf_loclist_add",CONCAT44(uVar9,0xc5));
    iVar1 = 0x23;
  }
  return iVar1;
}


/* ================= FUN_140022d70 @ 140022d70  size=637  callers=1 callees=2 ================= */
/* CALLERS: FUN_140022a40 */
/* CALLEES: FUN_14001bb80, FUN_140022790 */

undefined8
FUN_140022d70(longlong param_1,longlong param_2,longlong param_3,ulonglong *param_4,
             longlong *param_5,longlong *param_6,longlong *param_7,undefined8 param_8)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  undefined8 uVar4;
  int iVar5;
  longlong *plVar6;
  
  if ((((param_1 == 0) || (param_3 == 0)) || (param_2 == 0)) || (param_4 == (ulonglong *)0x0)) {
    FUN_14001bb80(param_1,param_8,0x1c,0,"_dwarf_loclist_add_locdesc",99);
    return 0x1c;
  }
  iVar5 = 0;
  iVar1 = 0;
  if (param_7 != (longlong *)0x0) {
    *param_7 = 0;
  }
  plVar6 = param_5;
  if (*param_4 < *(ulonglong *)(param_3 + 0x18)) {
    do {
      lVar2 = (**(code **)(param_1 + 0x1f8))
                        (*(undefined8 *)(param_3 + 8),param_4,*(undefined1 *)(param_2 + 0x48));
      lVar3 = (**(code **)(param_1 + 0x1f8))
                        (*(undefined8 *)(param_3 + 8),param_4,*(undefined1 *)(param_2 + 0x48));
      if (param_5 != (longlong *)0x0) {
        *(longlong *)*plVar6 = lVar2;
        *(longlong *)(*plVar6 + 8) = lVar3;
      }
      if (param_7 != (longlong *)0x0) {
        *param_7 = *param_7 + (ulonglong)*(byte *)(param_2 + 0x48) * 2;
      }
      if ((lVar2 == 0) && (lVar3 == 0)) {
        iVar1 = iVar5 + 1;
        break;
      }
      if (*(char *)(param_2 + 0x48) == '\x04') {
        if (lVar2 != 0xffffffff) goto LAB_140022e6c;
LAB_140022f4c:
        FUN_14001bb80(param_1,param_8,0,0,"_dwarf_loclist_add_locdesc",0x86);
      }
      else {
        if ((*(char *)(param_2 + 0x48) == '\b') && (lVar2 == -1)) goto LAB_140022f4c;
LAB_140022e6c:
        iVar1 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),param_4,2);
        lVar2 = (longlong)iVar1;
        if (*(ulonglong *)(param_3 + 0x18) < *param_4 + lVar2) {
          FUN_14001bb80(param_1,param_8,0xb,0,"_dwarf_loclist_add_locdesc",0x90);
          return 0xb;
        }
        if (param_7 != (longlong *)0x0) {
          *param_7 = *param_7 + lVar2;
        }
        if (((param_5 != (longlong *)0x0) && (0 < iVar1)) &&
           (uVar4 = FUN_140022790(param_1,param_2,*plVar6,*param_4 + *(longlong *)(param_3 + 8),
                                  lVar2,*(longlong *)(param_3 + 0x18) + *(longlong *)(param_3 + 8),
                                  *(undefined1 *)(param_2 + 0x48),param_8), (int)uVar4 != 0)) {
          return uVar4;
        }
        *param_4 = *param_4 + lVar2;
      }
      iVar1 = iVar5 + 1;
      plVar6 = plVar6 + 1;
      iVar5 = iVar1;
    } while (*param_4 < *(ulonglong *)(param_3 + 0x18));
  }
  if (param_6 != (longlong *)0x0) {
    *param_6 = (longlong)iVar1;
  }
  return 0;
}


/* ================= FUN_140022ff0 @ 140022ff0  size=45  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001eb00 */
/* CALLEES: free */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140022ff0(void)

{
  free(DAT_14083b030);
  DAT_14083b030 = (void *)0x0;
  DAT_14083b020 = 0;
  _DAT_14083b028 = 0;
  return;
}


/* ================= FUN_140023020 @ 140023020  size=67  callers=1 callees=2 ================= */
/* CALLERS: FUN_140019250 */
/* CALLEES: FUN_140022a10, FUN_140022a40 */

undefined8 FUN_140023020(undefined8 param_1,undefined8 param_2,undefined8 param_3,longlong *param_4)

{
  undefined8 uVar1;
  longlong local_18 [2];
  
  FUN_140022a10(param_3,local_18);
  if (local_18[0] == 0) {
    uVar1 = FUN_140022a40(param_1,param_2);
    return uVar1;
  }
  if (param_4 != (longlong *)0x0) {
    *param_4 = local_18[0];
  }
  return 0;
}


/* ================= FUN_140023070 @ 140023070  size=33  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400230d0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140023070(ulonglong param_1,undefined8 param_2)

{
  if ((DAT_14083b048 != 0) && (param_1 < DAT_14083b038)) {
    _DAT_14083b040 = _DAT_14083b040 + 1;
    *(undefined8 *)(DAT_14083b048 + param_1 * 8) = param_2;
  }
  return;
}


/* ================= FUN_1400230a0 @ 1400230a0  size=37  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400238d0 */

void FUN_1400230a0(ulonglong param_1,undefined8 *param_2)

{
  if ((DAT_14083b048 != 0) && (param_1 < DAT_14083b038)) {
    *param_2 = *(undefined8 *)(DAT_14083b048 + param_1 * 8);
    return;
  }
  *param_2 = 0;
  return;
}


/* ================= FUN_1400230d0 @ 1400230d0  size=759  callers=1 callees=7 ================= */
/* CALLERS: FUN_1400238d0 */
/* CALLEES: FUN_140019ae0, FUN_14001bb80, FUN_14001bd50, FUN_140023070, FUN_1400233d0, FUN_140026bb0, calloc */

int FUN_1400230d0(longlong param_1,longlong param_2,ulonglong param_3,undefined8 *param_4,
                 undefined8 param_5)

{
  int iVar1;
  longlong lVar2;
  longlong *plVar3;
  longlong lVar4;
  uint uVar5;
  ulonglong uVar7;
  ulonglong local_res8;
  longlong local_res10;
  undefined8 in_stack_ffffffffffffffa0;
  undefined4 uVar9;
  undefined8 uVar8;
  ulonglong uVar6;
  
  uVar9 = (undefined4)((ulonglong)in_stack_ffffffffffffffa0 >> 0x20);
  local_res10 = param_2;
  if ((param_1 == 0) || (param_2 == 0)) {
    FUN_14001bb80(param_1,param_5,0x1c,0,"_dwarf_loclists_add",CONCAT44(uVar9,0x22b));
    return 0x1c;
  }
  lVar2 = FUN_140026bb0(param_1,0x11);
  if (lVar2 == 0) {
    FUN_14001bd50(param_1,param_5,0x1d,0,"_dwarf_loclists_add",CONCAT44(uVar9,0x232),
                  ".debug_loclists",0,0);
    iVar1 = 0x23;
  }
  else if (param_3 < *(ulonglong *)(lVar2 + 0x18)) {
    plVar3 = (longlong *)FUN_140019ae0(0x30);
    if (plVar3 == (longlong *)0x0) {
      FUN_14001bb80(param_1,param_5,1,0,"_dwarf_loclists_add",CONCAT44(uVar9,0x23d));
      iVar1 = 1;
    }
    else {
      plVar3[2] = param_3;
      uVar6 = 0;
      iVar1 = FUN_1400233d0(param_1,param_2,lVar2,param_3,0,&local_res8,0,param_5);
      if (iVar1 == 0) {
        *(int *)(plVar3 + 1) = (int)local_res8;
        lVar4 = FUN_140019ae0((int)local_res8 * 8);
        *plVar3 = lVar4;
        if (lVar4 == 0) {
          uVar8 = 0x253;
LAB_140023278:
          iVar1 = 1;
          FUN_14001bb80(param_1,param_5,1,0,"_dwarf_loclists_add",uVar8);
        }
        else {
          uVar7 = uVar6;
          if (local_res8 != 0) {
            do {
              lVar4 = FUN_140019ae0(0x20);
              *(longlong *)(uVar7 + *plVar3) = lVar4;
              if (lVar4 == 0) {
                uVar8 = 0x25d;
                goto LAB_140023278;
              }
              uVar5 = (int)uVar6 + 1;
              uVar6 = (ulonglong)uVar5;
              uVar7 = uVar7 + 8;
            } while ((ulonglong)(longlong)(int)uVar5 < local_res8);
            lVar4 = *plVar3;
          }
          iVar1 = FUN_1400233d0(param_1,local_res10,lVar2,plVar3[2],lVar4,0,plVar3 + 3,param_5);
          if (iVar1 == 0) {
            if (DAT_14083b048 == (void *)0x0) {
              DAT_14083b048 = calloc(1,*(longlong *)(lVar2 + 0x18) << 3);
              DAT_14083b038 = *(undefined8 *)(lVar2 + 0x18);
            }
            FUN_140023070(plVar3[2],plVar3);
            if (param_4 != (undefined8 *)0x0) {
              *param_4 = plVar3;
            }
            iVar1 = 0;
          }
        }
      }
    }
  }
  else {
    FUN_14001bb80(param_1,param_5,0x1c,0,"_dwarf_loclists_add",CONCAT44(uVar9,0x237));
    iVar1 = 0x23;
  }
  return iVar1;
}


/* ================= FUN_1400233d0 @ 1400233d0  size=1193  callers=1 callees=7 ================= */
/* CALLERS: FUN_1400230d0 */
/* CALLEES: FUN_140015d50, FUN_14001bb80, FUN_140023a10, FUN_140023b50, FUN_140023c20, FUN_140025c90, FUN_1400266f0 */

undefined8
FUN_1400233d0(longlong param_1,longlong param_2,longlong param_3,longlong param_4,undefined8 param_5
             ,longlong *param_6,undefined8 *param_7,undefined8 param_8)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  longlong lVar4;
  undefined8 uVar5;
  longlong lVar6;
  int iVar7;
  char local_res8 [8];
  longlong local_res20;
  undefined4 uVar9;
  char *pcVar8;
  uint local_78;
  undefined4 uStack_74;
  longlong local_70;
  longlong local_68;
  longlong local_60;
  longlong local_58;
  longlong local_50;
  longlong local_48 [2];
  
  local_res20 = param_4;
  if (((param_1 == 0) || (param_3 == 0)) || (param_2 == 0)) {
    FUN_14001bb80(param_1,param_8,0x1c,0,"_dwarf_loclists_add_locdesc",0x11a);
    return 0x1c;
  }
  iVar7 = 0;
  pcVar8 = local_res8;
  uVar3 = FUN_140023a10(param_1,param_3,param_4,0,0,pcVar8,0,&local_78,&local_68);
  if ((int)uVar3 == 0) {
    if (CONCAT44(uStack_74,local_78) != 0) {
      local_res20 = (**(code **)(param_1 + 0x1f8))
                              (*(undefined8 *)(param_3 + 8),&local_res20,local_res8[0]);
      local_res20 = local_res20 + local_68;
    }
    if (param_7 != (undefined8 *)0x0) {
      *param_7 = 0;
    }
    lVar4 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),&local_res20,1);
    iVar2 = FUN_140015d50(param_2,&local_70,param_8);
    if (iVar2 != 0) {
      local_70 = 0;
    }
    local_78 = (uint)(iVar2 == 0);
    iVar2 = iVar7;
    while ((cVar1 = local_res8[0], uVar9 = (undefined4)((ulonglong)pcVar8 >> 0x20), lVar4 != 0 &&
           (iVar2 == 0))) {
      switch(lVar4) {
      case 0:
        break;
      case 1:
        uVar3 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
        uVar3 = FUN_140025c90(param_1,param_2,uVar3,&local_68,param_8);
        if ((int)uVar3 != 0) {
          return uVar3;
        }
        local_70 = local_68;
        break;
      case 2:
        uVar3 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
        local_60 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
        uVar3 = FUN_140025c90(param_1,param_2,uVar3,&local_50,param_8);
        if ((int)uVar3 != 0) {
          return uVar3;
        }
        uVar3 = FUN_140025c90(param_1,param_2,local_60,&local_58,param_8);
        lVar4 = local_50;
        lVar6 = local_58;
        if ((int)uVar3 != 0) {
          return uVar3;
        }
        goto LAB_1400235c4;
      case 3:
        uVar3 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
        local_60 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
        uVar3 = FUN_140025c90(param_1,param_2,uVar3,local_48,param_8);
        if ((int)uVar3 != 0) {
          return uVar3;
        }
        lVar4 = local_48[0];
        lVar6 = local_60 + local_48[0];
        goto LAB_1400235c4;
      case 4:
        lVar4 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
        lVar6 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
        if (0 < (int)local_78) {
          lVar4 = lVar4 + local_70;
          lVar6 = lVar6 + local_70;
          goto LAB_1400235c4;
        }
        uVar3 = CONCAT44(uVar9,0x1b2);
        FUN_14001bb80(param_1,param_8,0x27,0,"_dwarf_loclists_add_locdesc",uVar3);
        pcVar8 = (char *)CONCAT44((int)((ulonglong)uVar3 >> 0x20),iVar7);
        uVar3 = FUN_140023c20(param_1,param_2,param_3,&local_res20,param_5,pcVar8,0,0,param_8);
        goto LAB_1400235e4;
      case 5:
        lVar6 = -1;
        if (local_res8[0] == '\x04') {
          lVar6 = 0xffffffff;
        }
        lVar4 = 0;
        goto LAB_1400235c4;
      case 6:
        local_70 = (**(code **)(param_1 + 0x1f8))
                             (*(undefined8 *)(param_3 + 8),&local_res20,local_res8[0]);
        local_78 = 1;
        break;
      case 7:
        uVar3 = (**(code **)(param_1 + 0x1f8))
                          (*(undefined8 *)(param_3 + 8),&local_res20,local_res8[0]);
        uVar5 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),&local_res20,cVar1);
        pcVar8 = (char *)CONCAT44(uVar9,iVar7);
        uVar3 = FUN_140023b50(param_1,param_2,param_3,&local_res20,param_5,pcVar8,uVar3,uVar5,
                              param_8);
        if ((int)uVar3 != 0) {
          return uVar3;
        }
        iVar7 = iVar7 + 1;
        break;
      case 8:
        lVar4 = (**(code **)(param_1 + 0x1f8))
                          (*(undefined8 *)(param_3 + 8),&local_res20,local_res8[0]);
        lVar6 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
        lVar6 = lVar6 + lVar4;
LAB_1400235c4:
        pcVar8 = (char *)CONCAT44(uVar9,iVar7);
        uVar3 = FUN_140023b50(param_1,param_2,param_3,&local_res20,param_5,pcVar8,lVar4,lVar6,
                              param_8);
LAB_1400235e4:
        if ((int)uVar3 != 0) {
          return uVar3;
        }
        iVar7 = iVar7 + 1;
        break;
      default:
        iVar2 = 1;
      }
      lVar4 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),&local_res20,1);
    }
    if (param_6 != (longlong *)0x0) {
      *param_6 = (longlong)iVar7;
    }
    uVar3 = 0;
  }
  return uVar3;
}


/* ================= FUN_1400238a0 @ 1400238a0  size=45  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001eb00 */
/* CALLEES: free */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1400238a0(void)

{
  free(DAT_14083b048);
  DAT_14083b048 = (void *)0x0;
  DAT_14083b038 = 0;
  _DAT_14083b040 = 0;
  return;
}


/* ================= FUN_1400238d0 @ 1400238d0  size=115  callers=1 callees=2 ================= */
/* CALLERS: FUN_140019250 */
/* CALLEES: FUN_1400230a0, FUN_1400230d0 */

undefined8
FUN_1400238d0(undefined8 param_1,undefined8 param_2,undefined8 param_3,longlong *param_4,
             undefined8 param_5)

{
  undefined8 uVar1;
  longlong local_18 [2];
  
  FUN_1400230a0(param_3,local_18);
  if (local_18[0] == 0) {
    uVar1 = FUN_1400230d0(param_1,param_2,param_3,param_4,param_5);
  }
  else {
    if (param_4 != (longlong *)0x0) {
      *param_4 = local_18[0];
    }
    uVar1 = 0;
  }
  return uVar1;
}


/* ================= FUN_140023950 @ 140023950  size=187  callers=1 callees=1 ================= */
/* CALLERS: FUN_140023a10 */
/* CALLEES: FUN_140025e50 */

ulonglong FUN_140023950(undefined8 param_1,longlong param_2,ulonglong param_3,ulonglong *param_4)

{
  int iVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  ulonglong local_res10;
  longlong local_28 [2];
  
  uVar2 = 0;
  if ((param_2 == 0) || (*(ulonglong *)(param_2 + 0x18) <= param_3)) {
    uVar2 = 0x1c;
  }
  else {
    local_res10 = 0;
    uVar3 = uVar2;
    if (param_3 != 0) {
      do {
        if ((*(ulonglong *)(param_2 + 0x18) <= local_res10) || ((int)uVar2 != 0)) break;
        iVar1 = FUN_140025e50(param_1,param_2,&local_res10,local_28);
        if (iVar1 == 0) {
          local_res10 = local_28[0] + local_res10;
          if (local_res10 < param_3) {
            uVar3 = local_res10;
          }
        }
        else {
          uVar2 = 0x1a;
        }
      } while (local_res10 < param_3);
    }
    if (param_4 != (ulonglong *)0x0) {
      *param_4 = uVar3;
    }
  }
  return uVar2;
}


/* ================= FUN_140023a10 @ 140023a10  size=315  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400233d0 */
/* CALLEES: FUN_140023950, FUN_140025e50 */

undefined8
FUN_140023a10(longlong param_1,longlong param_2,undefined8 param_3,undefined8 *param_4,
             short *param_5,undefined1 *param_6,undefined1 *param_7,undefined8 *param_8,
             undefined8 *param_9)

{
  undefined1 uVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 local_28;
  undefined8 local_20;
  
  iVar4 = FUN_140023950();
  if (iVar4 == 0) {
    iVar4 = FUN_140025e50(param_1,param_2,&local_28,&local_20);
    uVar6 = 0;
    if (iVar4 != 0) {
      uVar6 = 0x1a;
    }
    sVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),&local_28,2);
    if (sVar3 != 5) {
      uVar6 = 0x1a;
    }
    uVar1 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),&local_28,1);
    uVar2 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),&local_28,1);
    uVar5 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),&local_28,4);
    if (param_4 != (undefined8 *)0x0) {
      *param_4 = local_20;
    }
    if (param_5 != (short *)0x0) {
      *param_5 = sVar3;
    }
    if (param_6 != (undefined1 *)0x0) {
      *param_6 = uVar1;
    }
    if (param_7 != (undefined1 *)0x0) {
      *param_7 = uVar2;
    }
    if (param_8 != (undefined8 *)0x0) {
      *param_8 = uVar5;
    }
    if (param_9 != (undefined8 *)0x0) {
      *param_9 = local_28;
    }
  }
  else {
    uVar6 = 0x1a;
  }
  return uVar6;
}


/* ================= FUN_140023b50 @ 140023b50  size=201  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400233d0 */
/* CALLEES: FUN_140022790, FUN_1400266f0 */

undefined8
FUN_140023b50(undefined8 param_1,longlong param_2,longlong param_3,longlong *param_4,
             longlong param_5,int param_6,undefined8 param_7,undefined8 param_8,undefined8 param_9)

{
  longlong lVar1;
  undefined8 uVar2;
  longlong lVar3;
  
  lVar1 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),param_4);
  if (param_5 != 0) {
    lVar3 = (longlong)param_6;
    **(undefined8 **)(param_5 + lVar3 * 8) = param_7;
    *(undefined8 *)(*(longlong *)(param_5 + lVar3 * 8) + 8) = param_8;
    if ((lVar1 != 0) &&
       (uVar2 = FUN_140022790(param_1,param_2,*(undefined8 *)(param_5 + lVar3 * 8),
                              *param_4 + *(longlong *)(param_3 + 8),lVar1,
                              *(longlong *)(param_3 + 0x18) + *(longlong *)(param_3 + 8),
                              *(undefined1 *)(param_2 + 0x48),param_9), (int)uVar2 != 0)) {
      return uVar2;
    }
  }
  *param_4 = *param_4 + lVar1;
  return 0;
}


/* ================= FUN_140023c20 @ 140023c20  size=32  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400233d0 */
/* CALLEES: FUN_1400266f0 */

undefined8 FUN_140023c20(undefined8 param_1,undefined8 param_2,longlong param_3,longlong *param_4)

{
  longlong lVar1;
  
  lVar1 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),param_4);
  *param_4 = *param_4 + lVar1;
  return 0;
}


/* ================= FUN_140023c40 @ 140023c40  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_140023f20 */

void FUN_140023c40(ulonglong param_1,undefined8 param_2)

{
  if (param_1 < DAT_14083b050) {
    DAT_14083b058 = DAT_14083b058 + 1;
    *(undefined8 *)(DAT_14083b060 + param_1 * 8) = param_2;
  }
  return;
}


/* ================= FUN_140023c60 @ 140023c60  size=37  callers=1 callees=0 ================= */
/* CALLERS: FUN_140023c90 */

void FUN_140023c60(ulonglong param_1,undefined8 *param_2)

{
  if ((DAT_14083b060 != 0) && (param_1 < DAT_14083b050)) {
    *param_2 = *(undefined8 *)(DAT_14083b060 + param_1 * 8);
    return;
  }
  *param_2 = 0;
  return;
}


/* ================= FUN_140023c90 @ 140023c90  size=84  callers=1 callees=1 ================= */
/* CALLERS: FUN_140019550 */
/* CALLEES: FUN_140023c60 */

undefined8
FUN_140023c90(undefined8 param_1,int param_2,ulonglong param_3,ulonglong *param_4,
             undefined8 *param_5)

{
  ulonglong uVar1;
  undefined8 *local_18 [3];
  
  FUN_140023c60((longlong)param_2,local_18);
  if (local_18[0] == (undefined8 *)0x0) {
    return 0xffffffff;
  }
  uVar1 = local_18[0][1];
  if ((param_3 != 0) && (param_3 < uVar1)) {
    uVar1 = param_3;
  }
  if (param_5 != (undefined8 *)0x0) {
    *param_5 = *local_18[0];
  }
  if (param_4 != (ulonglong *)0x0) {
    *param_4 = uVar1;
  }
  return 0;
}


/* ================= FUN_140023cf0 @ 140023cf0  size=120  callers=1 callees=0 ================= */
/* CALLERS: FUN_140023d70 */

undefined8 FUN_140023cf0(longlong *param_1,uint param_2,longlong param_3)

{
  byte *pbVar1;
  longlong lVar2;
  int iVar3;
  
  if ((param_3 != 0) && (param_1 != (longlong *)0x0)) {
    iVar3 = 0;
    if (*(byte *)(param_3 + 8) == 0) {
      return 0x23;
    }
    lVar2 = 0;
    pbVar1 = *(byte **)(param_3 + 0x10);
    do {
      if (*pbVar1 == param_2) {
        *param_1 = (longlong)(*(byte **)(param_3 + 0x10) + (longlong)iVar3 * 0x18);
        return 0;
      }
      iVar3 = iVar3 + 1;
      lVar2 = lVar2 + 1;
      pbVar1 = pbVar1 + 0x18;
    } while (lVar2 < (longlong)(ulonglong)*(byte *)(param_3 + 8));
    return 0x23;
  }
  return 0x1c;
}


/* ================= FUN_140023d70 @ 140023d70  size=296  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400241f0 */
/* CALLEES: FUN_140023cf0, FUN_140026d00 */

undefined8
FUN_140023d70(longlong param_1,longlong param_2,longlong param_3,int param_4,longlong param_5)

{
  ulonglong uVar1;
  int iVar2;
  ulonglong uVar3;
  longlong local_res8;
  
  if ((((param_1 != 0) && (4 < *(int *)(param_1 + 0x104))) && (param_2 != 0)) && (param_3 != 0)) {
    if ((param_5 != 0) && (param_4 - 0xe0U < 0x20)) {
      iVar2 = FUN_140023cf0(&local_res8,param_4,param_5);
      if (iVar2 != 0) {
        return 0;
      }
      uVar3 = 0;
      uVar1 = *(ulonglong *)(local_res8 + 8);
      if (uVar1 != 0) {
        do {
          FUN_140026d00(param_1,param_2,param_3,(*(byte *)(param_5 + 2) & 1) * '\x04' + '\x04',
                        *(undefined1 *)(*(longlong *)(local_res8 + 0x10) + uVar3));
          uVar3 = uVar3 + 1;
        } while (uVar3 < uVar1);
      }
      return 0;
    }
    return 0x1c;
  }
  return 0x1c;
}


/* ================= FUN_140023ea0 @ 140023ea0  size=125  callers=2 callees=1 ================= */
/* CALLERS: FUN_14001eb00, FUN_140023f20 */
/* CALLEES: free */

void FUN_140023ea0(longlong param_1)

{
  ulonglong uVar1;
  
  uVar1 = 0;
  if (DAT_14083b058 != 0) {
    do {
      free((void *)**(undefined8 **)((longlong)DAT_14083b060 + uVar1 * 8));
      uVar1 = uVar1 + 1;
    } while (uVar1 < DAT_14083b058);
  }
  free(DAT_14083b060);
  DAT_14083b060 = (void *)0x0;
  DAT_14083b050 = 0;
  DAT_14083b058 = 0;
  if (param_1 != 0) {
    *(undefined4 *)(param_1 + 0x104) = 0;
  }
  return;
}


/* ================= FUN_140023f20 @ 140023f20  size=524  callers=1 callees=8 ================= */
/* CALLERS: FUN_140019550 */
/* CALLEES: FUN_140019ae0, FUN_14001bb80, FUN_140023c40, FUN_140023ea0, FUN_1400241f0, FUN_140026bb0, calloc, free */

int FUN_140023f20(longlong param_1,undefined8 param_2)

{
  undefined8 uVar1;
  longlong lVar2;
  int iVar3;
  undefined4 uVar4;
  longlong lVar5;
  undefined8 *puVar6;
  void *local_res8;
  longlong local_res18;
  ulonglong local_res20;
  undefined8 in_stack_ffffffffffffffc0;
  
  uVar4 = (undefined4)((ulonglong)in_stack_ffffffffffffffc0 >> 0x20);
  if (param_1 == 0) {
    FUN_14001bb80(0,param_2,0x1c,0,"_dwarf_macinfo_init",0x262);
    return 0x1c;
  }
  *(undefined4 *)(param_1 + 0x104) = 4;
  lVar5 = FUN_140026bb0(param_1,7);
  if (lVar5 == 0) {
    lVar5 = FUN_140026bb0(param_1,8);
    if (lVar5 == 0) {
      FUN_14001bb80(param_1,param_2,0x1e,0,"_dwarf_macinfo_init",CONCAT44(uVar4,0x26c));
      return 0;
    }
    *(undefined4 *)(param_1 + 0x104) = 5;
  }
  if (DAT_14083b060 == (void *)0x0) {
    DAT_14083b060 = calloc(*(size_t *)(lVar5 + 0x18),8);
    DAT_14083b050 = *(undefined8 *)(lVar5 + 0x18);
  }
  local_res20 = 0;
  if (*(longlong *)(lVar5 + 0x18) != 0) {
    do {
      local_res8 = calloc(0x20,0x20);
      if (local_res8 == (void *)0x0) {
        uVar4 = 0x283;
LAB_1400240fb:
        FUN_14001bb80(param_1,param_2,1,0,"_dwarf_macinfo_init",uVar4);
        FUN_140023ea0(param_1);
        return 1;
      }
      local_res18 = 0;
      iVar3 = FUN_1400241f0(param_1,lVar5,&local_res20,&local_res8,0x20,&local_res18,param_2);
      lVar2 = local_res18;
      if ((iVar3 != 0) || (local_res18 == 0)) {
        free(local_res8);
        return iVar3;
      }
      puVar6 = (undefined8 *)FUN_140019ae0(0x10);
      if (puVar6 == (undefined8 *)0x0) {
        uVar4 = 0x291;
        goto LAB_1400240fb;
      }
      *puVar6 = local_res8;
      uVar1 = DAT_14083b058;
      puVar6[1] = lVar2;
      FUN_140023c40(uVar1,puVar6);
    } while (local_res20 < *(ulonglong *)(lVar5 + 0x18));
  }
  return 0;
}


/* ================= FUN_140024130 @ 140024130  size=179  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400241f0, FUN_140024790 */
/* CALLEES: free */

void FUN_140024130(undefined8 param_1,longlong param_2)

{
  byte bVar1;
  void *_Memory;
  void *pvVar2;
  void *_Memory_00;
  longlong lVar3;
  longlong lVar4;
  
  if ((param_2 != 0) && (_Memory_00 = *(void **)(param_2 + 0x10), _Memory_00 != (void *)0x0)) {
    bVar1 = *(byte *)(param_2 + 8);
    lVar3 = 0;
    pvVar2 = _Memory_00;
    lVar4 = lVar3;
    if ((ulonglong)bVar1 != 0) {
      do {
        _Memory = *(void **)(lVar3 + 0x10 + (longlong)pvVar2);
        if (_Memory != (void *)0x0) {
          free(_Memory);
          *(undefined8 *)(lVar3 + 0x10 + *(longlong *)(param_2 + 0x10)) = 0;
          *(undefined8 *)(lVar3 + 8 + *(longlong *)(param_2 + 0x10)) = 0;
          pvVar2 = *(void **)(param_2 + 0x10);
          _Memory_00 = pvVar2;
        }
        lVar4 = lVar4 + 1;
        lVar3 = lVar3 + 0x18;
      } while (lVar4 < (longlong)(ulonglong)bVar1);
    }
    free(_Memory_00);
    *(undefined8 *)(param_2 + 0x10) = 0;
    *(undefined1 *)(param_2 + 8) = 0;
  }
  return;
}


/* ================= FUN_1400241f0 @ 1400241f0  size=1232  callers=1 callees=9 ================= */
/* CALLERS: FUN_140023f20 */
/* CALLEES: FUN_140019950, FUN_14001bb80, FUN_140023d70, FUN_140024130, FUN_1400246c0, FUN_1400266f0, __chkstk, __security_check_cookie, realloc */

/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8
FUN_1400241f0(longlong param_1,longlong param_2,ulonglong *param_3,undefined8 *param_4,uint param_5,
             longlong *param_6,undefined8 param_7)

{
  char cVar1;
  longlong lVar2;
  int iVar3;
  undefined4 uVar4;
  undefined8 uVar5;
  void *_Memory;
  undefined8 uVar6;
  ulonglong uVar7;
  int iVar8;
  undefined8 *puVar9;
  longlong lVar10;
  void *pvStack_200b8;
  uint uStack_200b0;
  undefined8 uStack_200a8;
  undefined8 uStack_200a0;
  undefined8 uStack_20098;
  undefined8 *puStack_20090;
  undefined8 uStack_20088;
  undefined8 uStack_20080;
  longlong *plStack_20078;
  undefined8 auStack_20058 [16387];
  
  uStack_200b0 = param_5;
  plStack_20078 = param_6;
  puStack_20090 = param_4;
  if ((((param_1 == 0) || (param_2 == 0)) || (param_3 == (ulonglong *)0x0)) ||
     (param_4 == (undefined8 *)0x0)) {
    FUN_14001bb80(param_1,param_7,0x1c);
    uVar5 = 0x1c;
  }
  else {
    _Memory = (void *)*param_4;
    uStack_200a8 = 0;
    uStack_200a0 = 0;
    uStack_20098 = 0;
    if ((*(int *)(param_1 + 0x104) < 5) ||
       (uVar5 = FUN_1400246c0(param_1,param_2,param_3,&uStack_200a8), (int)uVar5 == 0)) {
      iVar3 = 0;
      if (*param_3 < *(ulonglong *)(param_2 + 0x18)) {
        lVar10 = 0;
        puVar9 = auStack_20058;
        pvStack_200b8 = _Memory;
        do {
          iVar8 = iVar3;
          if ((int)uStack_200b0 <= iVar8) {
            uStack_200b0 = uStack_200b0 * 2;
            _Memory = realloc(_Memory,(ulonglong)uStack_200b0 << 5);
            pvStack_200b8 = _Memory;
            if (_Memory == (void *)0x0) {
              FUN_140024130(param_1,&uStack_200a8);
              FUN_14001bb80(param_1,param_7,1);
              return 1;
            }
          }
          *(int *)(lVar10 + (longlong)_Memory) = (int)*param_3;
          iVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,1);
          uVar5 = *puVar9;
          *(char *)(lVar10 + 4 + (longlong)_Memory) = (char)iVar3;
          *(undefined8 *)(lVar10 + 0x10 + (longlong)_Memory) = uVar5;
          if (iVar3 == 0) break;
          if (iVar3 - 1U < 2) {
            uVar5 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
            lVar2 = *(longlong *)(param_2 + 8);
            *(undefined8 *)(lVar10 + 8 + (longlong)_Memory) = uVar5;
            *(ulonglong *)(lVar10 + 0x18 + (longlong)_Memory) = *param_3 + lVar2;
            uVar7 = *param_3;
            cVar1 = *(char *)(uVar7 + lVar2);
            while (cVar1 != '\0') {
              uVar7 = *param_3 + 1;
              *param_3 = uVar7;
              cVar1 = *(char *)(uVar7 + lVar2);
            }
LAB_14002457f:
            *param_3 = uVar7 + 1;
          }
          else if (iVar3 == 3) {
            uVar5 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
            puVar9 = puVar9 + 1;
            uVar6 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
            *puVar9 = uVar6;
            *(undefined8 *)(lVar10 + 8 + (longlong)pvStack_200b8) = uVar5;
            *(undefined8 *)(lVar10 + 0x10 + (longlong)pvStack_200b8) = uVar6;
            _Memory = pvStack_200b8;
          }
          else if (iVar3 == 4) {
            puVar9 = puVar9 + -1;
          }
          else if (((iVar3 == 7) || (iVar3 < 5)) || (9 < iVar3)) {
            if (iVar3 - 0xbU < 2) {
              uVar5 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
              uVar4 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
              *(undefined8 *)(lVar10 + 8 + (longlong)pvStack_200b8) = uVar5;
              iVar3 = FUN_140019950(param_1,uVar4,&uStack_20080);
              _Memory = pvStack_200b8;
              if (iVar3 == 0) {
                *(undefined8 *)(lVar10 + 0x18 + (longlong)pvStack_200b8) = uStack_20080;
              }
            }
            else if ((iVar3 == 7) || (iVar3 == 10)) {
              uVar4 = (**(code **)(param_1 + 0x1f8))
                                (*(undefined8 *)(param_2 + 8),param_3,
                                 (uStack_200a8._2_1_ & 1) * '\x04' + '\x04');
              *(undefined4 *)(lVar10 + (longlong)_Memory) = uVar4;
            }
            else {
              if (*(int *)(param_1 + 0x104) < 5) {
                if (iVar3 != 0xff) {
                  FUN_140024130(param_1,&uStack_200a8);
LAB_14002463a:
                  FUN_14001bb80(param_1,param_7,0x17);
                  return 0x17;
                }
                lVar2 = *(longlong *)(param_2 + 8);
                FUN_1400266f0(lVar2,param_3);
                uVar7 = *param_3;
                cVar1 = *(char *)(uVar7 + lVar2);
                while (_Memory = pvStack_200b8, cVar1 != '\0') {
                  uVar7 = *param_3 + 1;
                  *param_3 = uVar7;
                  cVar1 = *(char *)(uVar7 + lVar2);
                }
                goto LAB_14002457f;
              }
              if (0x1f < iVar3 - 0xe0U) {
                FUN_140024130(param_1,&uStack_200a8);
                goto LAB_14002463a;
              }
              FUN_140023d70(param_1,param_2,param_3);
            }
          }
          else {
            uVar5 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
            uVar4 = (**(code **)(param_1 + 0x1f8))
                              (*(undefined8 *)(param_2 + 8),param_3,
                               (uStack_200a8._2_1_ & 1) * '\x04' + '\x04');
            *(undefined8 *)(lVar10 + 8 + (longlong)pvStack_200b8) = uVar5;
            iVar3 = FUN_140019950(param_1,uVar4,&uStack_20088);
            _Memory = pvStack_200b8;
            if (iVar3 == 0) {
              *(undefined8 *)(lVar10 + 0x18 + (longlong)pvStack_200b8) = uStack_20088;
            }
          }
          lVar10 = lVar10 + 0x20;
          iVar3 = iVar8 + 1;
        } while (*param_3 < *(ulonglong *)(param_2 + 0x18));
        iVar3 = iVar8 + 1;
        param_4 = puStack_20090;
      }
      FUN_140024130(param_1,&uStack_200a8);
      if (plStack_20078 != (longlong *)0x0) {
        *plStack_20078 = (longlong)iVar3;
      }
      *param_4 = _Memory;
      uVar5 = 0;
    }
  }
  return uVar5;
}


/* ================= FUN_1400246c0 @ 1400246c0  size=198  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400241f0 */
/* CALLEES: FUN_140024790 */

undefined8
FUN_1400246c0(longlong param_1,longlong param_2,longlong param_3,undefined2 *param_4,
             undefined8 param_5)

{
  byte bVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  
  if (((param_4 == (undefined2 *)0x0) || (param_2 == 0)) || (param_3 == 0)) {
    uVar4 = 0x1b;
  }
  else {
    uVar2 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,2);
    *param_4 = uVar2;
    bVar1 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,1);
    *(byte *)(param_4 + 1) = bVar1;
    if ((bVar1 & 2) != 0) {
      uVar3 = (**(code **)(param_1 + 0x1f8))
                        (*(undefined8 *)(param_2 + 8),param_3,(bVar1 & 1) * '\x04' + '\x04');
      *(undefined4 *)(param_4 + 2) = uVar3;
      bVar1 = *(byte *)(param_4 + 1);
    }
    if ((bVar1 & 4) == 0) {
      uVar4 = 0;
    }
    else {
      uVar4 = FUN_140024790(param_1,param_2,param_3,param_4,param_5);
    }
  }
  return uVar4;
}


/* ================= FUN_140024790 @ 140024790  size=433  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400246c0 */
/* CALLEES: FUN_14001bb80, FUN_140024130, FUN_1400266f0, calloc */

undefined8
FUN_140024790(longlong param_1,longlong param_2,longlong *param_3,longlong param_4,
             undefined8 param_5)

{
  byte bVar1;
  undefined1 uVar2;
  void *pvVar3;
  ulonglong _Count;
  ulonglong uVar4;
  undefined8 uVar5;
  longlong lVar6;
  undefined8 *puVar7;
  undefined4 uVar8;
  
  bVar1 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,1);
  pvVar3 = calloc((ulonglong)bVar1,8);
  if (pvVar3 == (void *)0x0) {
    FUN_14001bb80(param_1,param_5,1,0,"_dwarf_macinfo_parse_opcode_operands_table",0x91);
    return 1;
  }
  *(byte *)(param_4 + 8) = bVar1;
  *(void **)(param_4 + 0x10) = pvVar3;
  if (bVar1 != 0) {
    lVar6 = 0;
    puVar7 = (undefined8 *)((longlong)pvVar3 + 0x10);
    do {
      uVar2 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,1);
      _Count = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
      if (*(ulonglong *)(param_2 + 0x18) < *param_3 + _Count) {
        uVar8 = 0xa9;
        uVar5 = 0x18;
LAB_14002490b:
        FUN_14001bb80(param_1,param_5,uVar5,0,"_dwarf_macinfo_parse_opcode_operands_table",uVar8);
        FUN_140024130(param_1,param_4);
        return 1;
      }
      pvVar3 = calloc(_Count,1);
      if (pvVar3 == (void *)0x0) {
        uVar8 = 0xb3;
        uVar5 = 1;
        goto LAB_14002490b;
      }
      *(undefined1 *)(puVar7 + -2) = uVar2;
      uVar4 = 0;
      puVar7[-1] = _Count;
      *puVar7 = pvVar3;
      if (_Count != 0) {
        do {
          uVar2 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,1);
          *(undefined1 *)(uVar4 + (longlong)pvVar3) = uVar2;
          uVar4 = uVar4 + 1;
        } while (uVar4 < _Count);
      }
      lVar6 = lVar6 + 1;
      puVar7 = puVar7 + 3;
    } while (lVar6 < (longlong)(ulonglong)bVar1);
  }
  return 0;
}


/* ================= FUN_140024950 @ 140024950  size=360  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001eb00 */
/* CALLEES: free */

void FUN_140024950(undefined8 *param_1)

{
  longlong *plVar1;
  longlong *_Memory;
  void *pvVar2;
  void *pvVar3;
  void *pvVar4;
  void *pvVar5;
  void *pvVar6;
  longlong lVar7;
  void *_Memory_00;
  
  if ((param_1 != (undefined8 *)0x0) && (_Memory = (longlong *)*param_1, _Memory != (longlong *)0x0)
     ) {
    _Memory_00 = (void *)*_Memory;
    while (_Memory_00 != (void *)0x0) {
      plVar1 = (longlong *)((longlong)_Memory_00 + 0x28);
      pvVar2 = *(void **)((longlong)_Memory_00 + 0x38);
      pvVar6 = *(void **)((longlong)_Memory_00 + 0x28);
      while (pvVar6 != (void *)0x0) {
        pvVar3 = (void *)*plVar1;
        pvVar4 = *(void **)((longlong)pvVar6 + 0x20);
        pvVar5 = *(void **)((longlong)pvVar3 + 0x20);
        if (pvVar3 == pvVar6) {
          *plVar1 = (longlong)pvVar5;
          if (pvVar5 == (void *)0x0) {
            *(longlong **)((longlong)_Memory_00 + 0x30) = plVar1;
          }
        }
        else {
          for (; pvVar5 != pvVar6; pvVar5 = *(void **)((longlong)pvVar5 + 0x20)) {
            pvVar3 = pvVar5;
          }
          lVar7 = *(longlong *)(*(longlong *)((longlong)pvVar3 + 0x20) + 0x20);
          *(longlong *)((longlong)pvVar3 + 0x20) = lVar7;
          if (lVar7 == 0) {
            *(longlong **)((longlong)_Memory_00 + 0x30) = (longlong *)((longlong)pvVar3 + 0x20);
          }
        }
        free(pvVar6);
        pvVar6 = pvVar4;
      }
      pvVar6 = (void *)*_Memory;
      if (pvVar6 == _Memory_00) {
        lVar7 = *(longlong *)((longlong)pvVar6 + 0x38);
        *_Memory = lVar7;
        if (lVar7 == 0) {
          _Memory[1] = (longlong)_Memory;
        }
      }
      else {
        for (pvVar3 = *(void **)((longlong)pvVar6 + 0x38); pvVar3 != _Memory_00;
            pvVar3 = *(void **)((longlong)pvVar3 + 0x38)) {
          pvVar6 = pvVar3;
        }
        lVar7 = *(longlong *)(*(longlong *)((longlong)pvVar6 + 0x38) + 0x38);
        *(longlong *)((longlong)pvVar6 + 0x38) = lVar7;
        if (lVar7 == 0) {
          _Memory[1] = (longlong)pvVar6 + 0x38;
        }
      }
      free(_Memory_00);
      _Memory_00 = pvVar2;
    }
    if ((void *)_Memory[2] != (void *)0x0) {
      free((void *)_Memory[2]);
    }
    free(_Memory);
    *param_1 = 0;
  }
  return;
}


/* ================= FUN_140024ac0 @ 140024ac0  size=210  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001f580 */
/* CALLEES: free */

void FUN_140024ac0(undefined8 *param_1)

{
  longlong *plVar1;
  void *_Memory;
  void *pvVar2;
  void *pvVar3;
  void *pvVar4;
  longlong lVar5;
  void *_Memory_00;
  
  if ((param_1 != (undefined8 *)0x0) && (_Memory = (void *)*param_1, _Memory != (void *)0x0)) {
    plVar1 = (longlong *)((longlong)_Memory + 0x28);
    _Memory_00 = *(void **)((longlong)_Memory + 0x28);
    while (_Memory_00 != (void *)0x0) {
      pvVar2 = (void *)*plVar1;
      pvVar3 = *(void **)((longlong)_Memory_00 + 0x20);
      pvVar4 = *(void **)((longlong)pvVar2 + 0x20);
      if (pvVar2 == _Memory_00) {
        *plVar1 = (longlong)pvVar4;
        if (pvVar4 == (void *)0x0) {
          *(longlong **)((longlong)_Memory + 0x30) = plVar1;
        }
      }
      else {
        for (; pvVar4 != _Memory_00; pvVar4 = *(void **)((longlong)pvVar4 + 0x20)) {
          pvVar2 = pvVar4;
        }
        lVar5 = *(longlong *)(*(longlong *)((longlong)pvVar2 + 0x20) + 0x20);
        *(longlong *)((longlong)pvVar2 + 0x20) = lVar5;
        if (lVar5 == 0) {
          *(longlong **)((longlong)_Memory + 0x30) = (longlong *)((longlong)pvVar2 + 0x20);
        }
      }
      if (*(void **)((longlong)_Memory_00 + 0x18) != (void *)0x0) {
        free(*(void **)((longlong)_Memory_00 + 0x18));
      }
      free(_Memory_00);
      _Memory_00 = pvVar3;
    }
    free(_Memory);
    *param_1 = 0;
  }
  return;
}


/* ================= FUN_140024ba0 @ 140024ba0  size=33  callers=1 callees=0 ================= */
/* CALLERS: FUN_140024c00 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140024ba0(ulonglong param_1,undefined8 param_2)

{
  if ((DAT_14083b078 != 0) && (param_1 < DAT_14083b068)) {
    _DAT_14083b070 = _DAT_14083b070 + 1;
    *(undefined8 *)(DAT_14083b078 + param_1 * 8) = param_2;
  }
  return;
}


/* ================= FUN_140024bd0 @ 140024bd0  size=37  callers=1 callees=0 ================= */
/* CALLERS: FUN_140024e60 */

void FUN_140024bd0(ulonglong param_1,undefined8 *param_2)

{
  if ((DAT_14083b078 != 0) && (param_1 < DAT_14083b068)) {
    *param_2 = *(undefined8 *)(DAT_14083b078 + param_1 * 8);
    return;
  }
  *param_2 = 0;
  return;
}


/* ================= FUN_140024c00 @ 140024c00  size=555  callers=1 callees=8 ================= */
/* CALLERS: FUN_140019750 */
/* CALLEES: FUN_140019ae0, FUN_14001bb80, FUN_14001bd50, FUN_140024ba0, FUN_140024f50, FUN_140026bb0, FUN_140026be0, calloc */

undefined8
FUN_140024c00(longlong param_1,longlong param_2,undefined8 param_3,longlong *param_4,
             undefined8 param_5)

{
  longlong lVar1;
  undefined8 uVar2;
  longlong lVar3;
  longlong lVar4;
  undefined4 uVar5;
  undefined8 local_res8;
  undefined8 in_stack_ffffffffffffffb0;
  undefined4 uVar7;
  undefined8 *puVar6;
  
  uVar7 = (undefined4)((ulonglong)in_stack_ffffffffffffffb0 >> 0x20);
  if (param_1 != 0) {
    uVar5 = 9;
    if (4 < *(ushort *)(param_2 + 0x12)) {
      uVar5 = 0x12;
    }
    lVar1 = FUN_140026bb0(param_1,uVar5);
    if (lVar1 == 0) {
      uVar2 = FUN_140026be0(uVar5);
      FUN_14001bd50(param_1,param_5,0x1d,0,"_dwarf_ranges_add",CONCAT44(uVar7,0x28c),uVar2,0,0);
      uVar2 = 0x23;
    }
    else {
      lVar3 = FUN_140019ae0(0x28);
      if (lVar3 == 0) {
        FUN_14001bb80(param_1,param_5,1,0,"_dwarf_ranges_add",CONCAT44(uVar7,0x294));
        uVar2 = 1;
      }
      else {
        *(undefined8 *)(lVar3 + 8) = param_3;
        puVar6 = &local_res8;
        uVar2 = FUN_140024f50(param_1,param_2,lVar1,param_3,0,puVar6,param_5);
        uVar7 = (undefined4)((ulonglong)puVar6 >> 0x20);
        if ((int)uVar2 == 0) {
          *(undefined8 *)(lVar3 + 0x18) = local_res8;
          lVar4 = FUN_140019ae0((int)local_res8 * 0x18);
          *(longlong *)(lVar3 + 0x10) = lVar4;
          if (lVar4 == 0) {
            FUN_14001bb80(param_1,param_5,1,0,"_dwarf_ranges_add",CONCAT44(uVar7,0x2a6));
            uVar2 = 1;
          }
          else {
            uVar2 = FUN_140024f50(param_1,param_2,lVar1,param_3,lVar4,0,param_5);
            if ((int)uVar2 == 0) {
              if (DAT_14083b078 == (void *)0x0) {
                DAT_14083b078 = calloc(1,*(longlong *)(lVar1 + 0x18) << 3);
                DAT_14083b068 = *(undefined8 *)(lVar1 + 0x18);
              }
              FUN_140024ba0(*(undefined8 *)(lVar3 + 8),lVar3);
              if (param_4 != (longlong *)0x0) {
                *param_4 = lVar3;
              }
              uVar2 = 0;
            }
          }
        }
      }
    }
    return uVar2;
  }
  FUN_14001bb80(0,param_5,0x1c,0,"_dwarf_ranges_add",CONCAT44(uVar7,0x27f));
  return 0x1c;
}


/* ================= FUN_140024e30 @ 140024e30  size=45  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001eb00 */
/* CALLEES: free */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140024e30(void)

{
  free(DAT_14083b078);
  DAT_14083b078 = (void *)0x0;
  DAT_14083b068 = 0;
  _DAT_14083b070 = 0;
  return;
}


/* ================= FUN_140024e60 @ 140024e60  size=39  callers=1 callees=1 ================= */
/* CALLERS: FUN_140019750 */
/* CALLEES: FUN_140024bd0 */

undefined8 FUN_140024e60(undefined8 param_1,undefined8 param_2,longlong *param_3)

{
  undefined8 uVar1;
  
  FUN_140024bd0(param_2,param_3);
  uVar1 = 0;
  if (*param_3 == 0) {
    uVar1 = 0x23;
  }
  return uVar1;
}


/* ================= FUN_140024e90 @ 140024e90  size=187  callers=1 callees=1 ================= */
/* CALLERS: FUN_140025580 */
/* CALLEES: FUN_140025e50 */

ulonglong FUN_140024e90(undefined8 param_1,longlong param_2,ulonglong param_3,ulonglong *param_4)

{
  int iVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  ulonglong local_res10;
  longlong local_28 [2];
  
  uVar2 = 0;
  if ((param_2 == 0) || (*(ulonglong *)(param_2 + 0x18) <= param_3)) {
    uVar2 = 0x1c;
  }
  else {
    local_res10 = 0;
    uVar3 = uVar2;
    if (param_3 != 0) {
      do {
        if ((*(ulonglong *)(param_2 + 0x18) <= local_res10) || ((int)uVar2 != 0)) break;
        iVar1 = FUN_140025e50(param_1,param_2,&local_res10,local_28);
        if (iVar1 == 0) {
          local_res10 = local_28[0] + local_res10;
          if (local_res10 < param_3) {
            uVar3 = local_res10;
          }
        }
        else {
          uVar2 = 0x19;
        }
      } while (local_res10 < param_3);
    }
    if (param_4 != (ulonglong *)0x0) {
      *param_4 = uVar3;
    }
  }
  return uVar2;
}


/* ================= FUN_140024f50 @ 140024f50  size=16  callers=1 callees=1 ================= */
/* CALLERS: FUN_140024c00 */
/* CALLEES: FUN_1400250c0 */

undefined8
FUN_140024f50(longlong param_1,longlong param_2,longlong param_3,ulonglong param_4,longlong param_5,
             longlong *param_6)

{
  undefined8 uVar1;
  longlong lVar2;
  longlong lVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  ulonglong uStackX_20;
  
  if (4 < *(ushort *)(param_2 + 0x12)) {
    uVar1 = FUN_1400250c0();
    return uVar1;
  }
  uStackX_20 = param_4;
  if (((param_1 == 0) || (param_3 == 0)) || (param_2 == 0)) {
    FUN_14001bb80(param_1,0,0x1c,0,"_dwarf_ranges_parse_dwarf4",0x52);
    return 0x1c;
  }
  iVar6 = 0;
  if (param_4 < *(ulonglong *)(param_3 + 0x18)) {
    piVar5 = (int *)(param_5 + 0x10);
    do {
      lVar2 = (**(code **)(param_1 + 0x1f8))
                        (*(undefined8 *)(param_3 + 8),&uStackX_20,*(undefined1 *)(param_2 + 0x48));
      lVar3 = (**(code **)(param_1 + 0x1f8))
                        (*(undefined8 *)(param_3 + 8),&uStackX_20,*(undefined1 *)(param_2 + 0x48));
      if (lVar2 == 0) {
        if (lVar3 != 0) goto LAB_140025017;
        iVar4 = 3;
      }
      else {
        if (lVar2 == 0xffffffff) {
          if (*(char *)(param_2 + 0x48) == '\x04') {
LAB_140025082:
            iVar4 = 2;
            goto LAB_140025019;
          }
        }
        else if ((lVar2 == -1) && (*(char *)(param_2 + 0x48) == '\b')) goto LAB_140025082;
LAB_140025017:
        iVar4 = 0;
      }
LAB_140025019:
      if (param_5 != 0) {
        *(longlong *)(piVar5 + -4) = lVar2;
        *(longlong *)(piVar5 + -2) = lVar3;
        *piVar5 = iVar4;
      }
      iVar6 = iVar6 + 1;
      piVar5 = piVar5 + 6;
    } while ((iVar4 != 3) && (uStackX_20 < *(ulonglong *)(param_3 + 0x18)));
  }
  if (param_6 != (longlong *)0x0) {
    *param_6 = (longlong)iVar6;
  }
  return 0;
}


/* ================= FUN_140024f60 @ 140024f60  size=345  callers=0 callees=1 ================= */
/* CALLEES: FUN_14001bb80 */

undefined8
FUN_140024f60(longlong param_1,longlong param_2,longlong param_3,ulonglong param_4,longlong param_5,
             longlong *param_6)

{
  longlong lVar1;
  longlong lVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  ulonglong local_res20;
  
  local_res20 = param_4;
  if (((param_1 == 0) || (param_3 == 0)) || (param_2 == 0)) {
    FUN_14001bb80(param_1,0,0x1c,0,"_dwarf_ranges_parse_dwarf4",0x52);
    return 0x1c;
  }
  iVar5 = 0;
  if (param_4 < *(ulonglong *)(param_3 + 0x18)) {
    piVar4 = (int *)(param_5 + 0x10);
    do {
      lVar1 = (**(code **)(param_1 + 0x1f8))
                        (*(undefined8 *)(param_3 + 8),&local_res20,*(undefined1 *)(param_2 + 0x48));
      lVar2 = (**(code **)(param_1 + 0x1f8))
                        (*(undefined8 *)(param_3 + 8),&local_res20,*(undefined1 *)(param_2 + 0x48));
      if (lVar1 == 0) {
        if (lVar2 != 0) goto LAB_140025017;
        iVar3 = 3;
      }
      else {
        if (lVar1 == 0xffffffff) {
          if (*(char *)(param_2 + 0x48) == '\x04') {
LAB_140025082:
            iVar3 = 2;
            goto LAB_140025019;
          }
        }
        else if ((lVar1 == -1) && (*(char *)(param_2 + 0x48) == '\b')) goto LAB_140025082;
LAB_140025017:
        iVar3 = 0;
      }
LAB_140025019:
      if (param_5 != 0) {
        *(longlong *)(piVar4 + -4) = lVar1;
        *(longlong *)(piVar4 + -2) = lVar2;
        *piVar4 = iVar3;
      }
      iVar5 = iVar5 + 1;
      piVar4 = piVar4 + 6;
    } while ((iVar3 != 3) && (local_res20 < *(ulonglong *)(param_3 + 0x18)));
  }
  if (param_6 != (longlong *)0x0) {
    *param_6 = (longlong)iVar5;
  }
  return 0;
}


/* ================= FUN_1400250c0 @ 1400250c0  size=1171  callers=1 callees=5 ================= */
/* CALLERS: FUN_140024f50 */
/* CALLEES: FUN_140015c00, FUN_14001bb80, FUN_140025580, FUN_140025c60, FUN_1400266f0 */

undefined8
FUN_1400250c0(longlong param_1,longlong param_2,longlong param_3,ulonglong param_4,longlong param_5,
             longlong *param_6,undefined8 param_7)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined8 uVar4;
  longlong lVar5;
  longlong lVar6;
  undefined4 *puVar7;
  int iVar8;
  byte local_res8 [8];
  longlong local_res10;
  ulonglong local_res20;
  longlong local_78;
  longlong local_70;
  longlong local_68;
  longlong local_60;
  undefined8 local_58;
  longlong local_50;
  longlong local_48;
  longlong local_40;
  
  local_res10 = param_2;
  local_res20 = param_4;
  if (((param_1 == 0) || (param_3 == 0)) || (param_2 == 0)) {
    FUN_14001bb80(param_1,0,0x1c,0,"_dwarf_ranges_parse_dwarf5",0x112);
    return 0x1c;
  }
  uVar4 = FUN_140025580(param_1,param_3,param_4,0,0,local_res8,0,&local_78,&local_70);
  if ((int)uVar4 == 0) {
    if (local_78 != 0) {
      lVar5 = (**(code **)(param_1 + 0x1f8))
                        (*(undefined8 *)(param_3 + 8),&local_res20,local_res8[0]);
      local_res20 = lVar5 + local_70;
    }
    iVar8 = 0;
    if (local_res20 < *(ulonglong *)(param_3 + 0x18)) {
      puVar7 = (undefined4 *)(param_5 + 0x10);
      iVar8 = 0;
      do {
        cVar2 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),&local_res20,1);
        bVar1 = local_res8[0];
        switch(cVar2) {
        case '\0':
          if (param_5 != 0) {
            lVar5 = 0;
            *puVar7 = 3;
            *(undefined8 *)(puVar7 + -4) = 0;
LAB_1400254bc:
            *(longlong *)(puVar7 + -2) = lVar5;
          }
          break;
        case '\x01':
          iVar3 = FUN_140015c00(param_2,&local_70,param_7);
          if (iVar3 != 0) {
            return 0x23;
          }
          lVar5 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
          iVar3 = FUN_140025c60(param_2,0x15,(ulonglong)*(byte *)(param_2 + 0x49) * lVar5 + local_70
                                ,&local_78,param_7);
          if (iVar3 != 0) {
            return 0x23;
          }
          if (param_5 != 0) {
            uVar4 = 0xffffffffffffffff;
            if (local_res8[0] == 4) {
              uVar4 = 0xffffffff;
            }
            *(undefined8 *)(puVar7 + -4) = uVar4;
            *puVar7 = 2;
            lVar5 = local_78;
            goto LAB_1400254bc;
          }
          break;
        case '\x02':
          iVar3 = FUN_140015c00(param_2,&local_68,param_7);
          if (iVar3 != 0) {
            return 0x23;
          }
          lVar5 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
          lVar6 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
          param_2 = local_res10;
          local_60 = (ulonglong)*(byte *)(local_res10 + 0x49) * lVar6 + local_68;
          iVar3 = FUN_140025c60(local_res10,0x15,
                                (ulonglong)*(byte *)(local_res10 + 0x49) * lVar5 + local_68,
                                &local_58,param_7);
          if (iVar3 != 0) {
            return 0x23;
          }
          iVar3 = FUN_140025c60(param_2,0x15,local_60,&local_50,param_7);
          if (iVar3 != 0) {
            return 0x23;
          }
          if (param_5 != 0) {
            *(undefined8 *)(puVar7 + -4) = local_58;
            lVar5 = local_50;
LAB_1400254b6:
            *puVar7 = 1;
            goto LAB_1400254bc;
          }
          break;
        case '\x03':
          iVar3 = FUN_140015c00(param_2,&local_48,param_7);
          if (iVar3 != 0) {
            return 0x23;
          }
          lVar5 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
          local_60 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
          param_2 = local_res10;
          iVar3 = FUN_140025c60(local_res10,0x15,(ulonglong)local_res8[0] * lVar5 + local_48,
                                &local_40,param_7);
          if (iVar3 != 0) {
            return 0x23;
          }
          if (param_5 != 0) {
            *(longlong *)(puVar7 + -4) = local_40;
            lVar5 = local_40 + local_60;
            goto LAB_1400254b6;
          }
          break;
        case '\x04':
          local_60 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
          uVar4 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
          if (param_5 != 0) {
            *(longlong *)(puVar7 + -4) = local_60;
            *(undefined8 *)(puVar7 + -2) = uVar4;
            *puVar7 = 0;
          }
          break;
        case '\x05':
          lVar5 = (**(code **)(param_1 + 0x1f8))
                            (*(undefined8 *)(param_3 + 8),&local_res20,local_res8[0]);
          if (param_5 != 0) {
            uVar4 = 0xffffffffffffffff;
            if (local_res8[0] == 4) {
              uVar4 = 0xffffffff;
            }
            *(undefined8 *)(puVar7 + -4) = uVar4;
            *puVar7 = 2;
            goto LAB_1400254bc;
          }
          break;
        case '\x06':
          local_60 = (**(code **)(param_1 + 0x1f8))
                               (*(undefined8 *)(param_3 + 8),&local_res20,local_res8[0]);
          uVar4 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_3 + 8),&local_res20,bVar1);
          param_2 = local_res10;
          if (param_5 != 0) {
            *(longlong *)(puVar7 + -4) = local_60;
            *(undefined8 *)(puVar7 + -2) = uVar4;
            *puVar7 = 1;
          }
          break;
        case '\a':
          local_60 = (**(code **)(param_1 + 0x1f8))
                               (*(undefined8 *)(param_3 + 8),&local_res20,local_res8[0]);
          lVar5 = FUN_1400266f0(*(undefined8 *)(param_3 + 8),&local_res20);
          if (param_5 != 0) {
            *(longlong *)(puVar7 + -4) = local_60;
            lVar5 = local_60 + lVar5;
            goto LAB_1400254b6;
          }
        }
        iVar8 = iVar8 + 1;
        puVar7 = puVar7 + 6;
      } while ((cVar2 != '\0') && (local_res20 < *(ulonglong *)(param_3 + 0x18)));
    }
    if (param_6 != (longlong *)0x0) {
      *param_6 = (longlong)iVar8;
    }
    uVar4 = 0;
  }
  return uVar4;
}


/* ================= FUN_140025580 @ 140025580  size=315  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400250c0 */
/* CALLEES: FUN_140024e90, FUN_140025e50 */

undefined8
FUN_140025580(longlong param_1,longlong param_2,undefined8 param_3,undefined8 *param_4,
             short *param_5,undefined1 *param_6,undefined1 *param_7,undefined8 *param_8,
             undefined8 *param_9)

{
  undefined1 uVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 local_28;
  undefined8 local_20;
  
  iVar4 = FUN_140024e90();
  if (iVar4 == 0) {
    iVar4 = FUN_140025e50(param_1,param_2,&local_28,&local_20);
    uVar6 = 0;
    if (iVar4 != 0) {
      uVar6 = 0x19;
    }
    sVar3 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),&local_28,2);
    if (sVar3 != 5) {
      uVar6 = 0x19;
    }
    uVar1 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),&local_28,1);
    uVar2 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),&local_28,1);
    uVar5 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),&local_28,4);
    if (param_4 != (undefined8 *)0x0) {
      *param_4 = local_20;
    }
    if (param_5 != (short *)0x0) {
      *param_5 = sVar3;
    }
    if (param_6 != (undefined1 *)0x0) {
      *param_6 = uVar1;
    }
    if (param_7 != (undefined1 *)0x0) {
      *param_7 = uVar2;
    }
    if (param_8 != (undefined8 *)0x0) {
      *param_8 = uVar5;
    }
    if (param_9 != (undefined8 *)0x0) {
      *param_9 = local_28;
    }
  }
  else {
    uVar6 = 0x19;
  }
  return uVar6;
}


/* ================= FUN_1400256c0 @ 1400256c0  size=381  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001f580 */
/* CALLEES: free */

void FUN_1400256c0(longlong param_1)

{
  longlong *plVar1;
  longlong *plVar2;
  longlong *plVar3;
  longlong lVar4;
  void *pvVar5;
  void *pvVar6;
  void *pvVar7;
  longlong *plVar8;
  void *_Memory;
  undefined8 *_Memory_00;
  longlong *_Memory_01;
  
  plVar1 = (longlong *)(param_1 + 0x318);
  _Memory_01 = (longlong *)*plVar1;
  if (_Memory_01 == (longlong *)0x0) {
    *(undefined8 *)(param_1 + 0x328) = 0;
    *(undefined8 *)(param_1 + 0x330) = 0;
    return;
  }
  do {
    plVar2 = (longlong *)*plVar1;
    plVar3 = (longlong *)_Memory_01[8];
    if (plVar2 == _Memory_01) {
      lVar4 = plVar2[8];
      *plVar1 = lVar4;
      if (lVar4 == 0) {
        *(longlong **)(param_1 + 800) = plVar1;
      }
    }
    else {
      for (plVar8 = (longlong *)plVar2[8]; plVar8 != _Memory_01; plVar8 = (longlong *)plVar8[8]) {
        plVar2 = plVar8;
      }
      lVar4 = *(longlong *)(plVar2[8] + 0x40);
      plVar2[8] = lVar4;
      if (lVar4 == 0) {
        *(longlong **)(param_1 + 800) = plVar2 + 8;
      }
    }
    plVar2 = _Memory_01 + 3;
    _Memory = (void *)_Memory_01[3];
    while (_Memory != (void *)0x0) {
      pvVar5 = (void *)*plVar2;
      pvVar6 = *(void **)((longlong)_Memory + 0x28);
      pvVar7 = *(void **)((longlong)pvVar5 + 0x28);
      if (pvVar5 == _Memory) {
        *plVar2 = (longlong)pvVar7;
        if (pvVar7 == (void *)0x0) {
          _Memory_01[4] = (longlong)plVar2;
        }
      }
      else {
        for (; pvVar7 != _Memory; pvVar7 = *(void **)((longlong)pvVar7 + 0x28)) {
          pvVar5 = pvVar7;
        }
        lVar4 = *(longlong *)(*(longlong *)((longlong)pvVar5 + 0x28) + 0x28);
        *(longlong *)((longlong)pvVar5 + 0x28) = lVar4;
        if (lVar4 == 0) {
          _Memory_01[4] = (longlong)pvVar5 + 0x28;
        }
      }
      free(_Memory);
      _Memory = pvVar6;
    }
    if (((*(uint *)(param_1 + 0x230) >> 0x1a & 1) != 0) &&
       (_Memory_00 = (undefined8 *)*_Memory_01, _Memory_00 != (undefined8 *)0x0)) {
      if ((void *)*_Memory_00 != (void *)0x0) {
        free((void *)*_Memory_00);
        _Memory_00 = (undefined8 *)*_Memory_01;
      }
      free(_Memory_00);
    }
    free(_Memory_01);
    _Memory_01 = plVar3;
  } while (plVar3 != (longlong *)0x0);
  *(undefined8 *)(param_1 + 0x328) = 0;
  *(undefined8 *)(param_1 + 0x330) = 0;
  return;
}


/* ================= FUN_140025a00 @ 140025a00  size=100  callers=4 callees=0 ================= */
/* CALLERS: FUN_14001d440, FUN_140021d80, FUN_1400222c0, FUN_140026d00 */

ulonglong FUN_140025a00(undefined8 *param_1)

{
  byte bVar1;
  byte bVar2;
  ulonglong uVar3;
  byte *pbVar4;
  ulonglong uVar5;
  ulonglong uVar6;
  
  uVar3 = 0;
  if ((param_1 != (undefined8 *)0x0) &&
     (pbVar4 = (byte *)*param_1, uVar5 = uVar3, uVar6 = uVar3, pbVar4 != (byte *)0x0)) {
    do {
      bVar1 = *pbVar4;
      pbVar4 = pbVar4 + 1;
      if ((int)uVar6 < 9) {
        bVar2 = (byte)uVar3;
        uVar3 = (ulonglong)((int)uVar3 + 7);
        uVar5 = uVar5 | (longlong)(int)((bVar1 & 0x7f) << (bVar2 & 0x1f));
        uVar6 = (ulonglong)((int)uVar6 + 1);
      }
    } while ((char)bVar1 < '\0');
    if (((int)uVar3 < 0x20) && ((bVar1 & 0x40) != 0)) {
      uVar5 = uVar5 | -1L << ((byte)uVar3 & 0x3f);
    }
    *param_1 = pbVar4;
    return uVar5;
  }
  return 0;
}


/* ================= FUN_140025a70 @ 140025a70  size=85  callers=10 callees=0 ================= */
/* CALLERS: FUN_14001d440, FUN_14001fc00, FUN_140020a70, FUN_1400211b0, FUN_140021610, FUN_140021720, FUN_140021b20, FUN_140021d80, FUN_1400222c0, FUN_140026d00 */

ulonglong FUN_140025a70(undefined8 *param_1)

{
  byte bVar1;
  ulonglong uVar2;
  byte *pbVar3;
  ulonglong uVar4;
  ulonglong uVar5;
  ulonglong uVar6;
  
  uVar2 = 0;
  if ((param_1 != (undefined8 *)0x0) &&
     (pbVar3 = (byte *)*param_1, uVar4 = uVar2, uVar5 = uVar2, pbVar3 != (byte *)0x0)) {
    do {
      bVar1 = *pbVar3;
      pbVar3 = pbVar3 + 1;
      uVar6 = uVar5;
      if ((int)uVar4 < 9) {
        uVar6 = (ulonglong)((int)uVar5 + 7);
        uVar2 = uVar2 | (longlong)(int)((bVar1 & 0x7f) << ((byte)uVar5 & 0x1f));
        uVar4 = (ulonglong)((int)uVar4 + 1);
      }
      uVar5 = uVar6;
    } while ((char)bVar1 < '\0');
    *param_1 = pbVar3;
  }
  return uVar2;
}


/* ================= FUN_140025ad0 @ 140025ad0  size=188  callers=1 callees=3 ================= */
/* CALLERS: FUN_14001a6d0 */
/* CALLEES: FUN_140015c00, FUN_140025c60, FUN_1400266f0 */

int FUN_140025ad0(undefined8 param_1,longlong param_2,undefined8 param_3,longlong param_4,
                 longlong *param_5,longlong *param_6,longlong *param_7,undefined8 param_8)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong local_res10;
  longlong local_28 [2];
  
  lVar3 = 0;
  local_res10 = 0;
  lVar2 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
  iVar1 = FUN_140015c00(param_4,local_28,param_8);
  lVar4 = lVar3;
  if (iVar1 == 0) {
    lVar3 = (ulonglong)*(byte *)(param_4 + 0x49) * lVar2 + local_28[0];
    iVar1 = FUN_140025c60(param_4,0x15,lVar3,&local_res10,param_8);
    lVar4 = local_res10;
  }
  if (param_5 != (longlong *)0x0) {
    *param_5 = lVar2;
  }
  if (param_6 != (longlong *)0x0) {
    *param_6 = lVar3;
  }
  if (param_7 != (longlong *)0x0) {
    *param_7 = lVar4;
  }
  return iVar1;
}


/* ================= FUN_140025b90 @ 140025b90  size=201  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001a6d0 */
/* CALLEES: FUN_140015c00, FUN_140025c60 */

int FUN_140025b90(longlong param_1,longlong param_2,undefined8 param_3,longlong param_4,
                 undefined4 param_5,longlong *param_6,longlong *param_7,longlong *param_8,
                 undefined8 param_9)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong local_res8;
  longlong local_res10;
  
  lVar3 = 0;
  local_res8 = 0;
  lVar2 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,param_5);
  iVar1 = FUN_140015c00(param_4,&local_res10,param_9);
  lVar4 = lVar3;
  if (iVar1 == 0) {
    lVar3 = (ulonglong)*(byte *)(param_4 + 0x49) * lVar2 + local_res10;
    iVar1 = FUN_140025c60(param_4,0x15,lVar3,&local_res8,param_9);
    lVar4 = local_res8;
  }
  if (param_6 != (longlong *)0x0) {
    *param_6 = lVar2;
  }
  if (param_7 != (longlong *)0x0) {
    *param_7 = lVar3;
  }
  if (param_8 != (longlong *)0x0) {
    *param_8 = lVar4;
  }
  return iVar1;
}


/* ================= FUN_140025c60 @ 140025c60  size=37  callers=3 callees=1 ================= */
/* CALLERS: FUN_1400250c0, FUN_140025ad0, FUN_140025b90 */
/* CALLEES: FUN_140026770 */

void FUN_140025c60(void)

{
  FUN_140026770();
  return;
}


/* ================= FUN_140025c90 @ 140025c90  size=362  callers=2 callees=4 ================= */
/* CALLERS: FUN_1400222c0, FUN_1400233d0 */
/* CALLEES: FUN_140015c00, FUN_14001bb80, FUN_14001bd50, FUN_140026bb0 */

undefined8
FUN_140025c90(longlong param_1,longlong param_2,longlong param_3,undefined8 *param_4,
             undefined8 param_5)

{
  int iVar1;
  longlong lVar2;
  undefined8 uVar3;
  longlong local_res8;
  ulonglong local_28 [2];
  
  if ((param_1 != 0) && (param_2 != 0)) {
    lVar2 = FUN_140026bb0(param_1,0x15);
    if (lVar2 == 0) {
      FUN_14001bd50(param_1,param_5,0x1d,0,"_dwarf_read_addridx_dot_debug_addr",0x2d9,".debug_addr",
                    0,0);
    }
    else {
      iVar1 = FUN_140015c00(param_2,&local_res8,param_5);
      if (iVar1 == 0) {
        local_28[0] = (ulonglong)*(byte *)(param_2 + 0x49) * param_3 + local_res8;
        if (local_28[0] < *(ulonglong *)(lVar2 + 0x18)) {
          uVar3 = (**(code **)(param_1 + 0x1f8))
                            (*(undefined8 *)(lVar2 + 8),local_28,*(undefined1 *)(param_2 + 0x48));
          *param_4 = uVar3;
          return 0;
        }
        FUN_14001bb80(param_1,param_5,0x1c,0,"_dwarf_read_addridx_dot_debug_addr",0x2ee);
      }
    }
    return 0x23;
  }
  FUN_14001bb80(param_1,param_5,0x1c,0,"_dwarf_read_addridx_dot_debug_addr",0x2ce);
  return 0x1c;
}


/* ================= FUN_140025e00 @ 140025e00  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_14001a6d0 */

longlong FUN_140025e00(longlong param_1,longlong *param_2,longlong param_3)

{
  longlong lVar1;
  
  if ((param_1 != 0) && (param_2 != (longlong *)0x0)) {
    lVar1 = *param_2;
    *param_2 = lVar1 + param_3;
    return lVar1 + param_1;
  }
  return 0;
}


/* ================= FUN_140025e20 @ 140025e20  size=37  callers=1 callees=1 ================= */
/* CALLERS: FUN_140026610 */
/* CALLEES: FUN_140026770 */

void FUN_140025e20(void)

{
  FUN_140026770();
  return;
}


/* ================= FUN_140025e50 @ 140025e50  size=154  callers=4 callees=0 ================= */
/* CALLERS: FUN_140023950, FUN_140023a10, FUN_140024e90, FUN_140025580 */

undefined4 FUN_140025e50(longlong param_1,longlong param_2,longlong param_3,ulonglong *param_4)

{
  ulonglong uVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if (param_3 == 0) {
    uVar2 = 0x1c;
  }
  else {
    uVar1 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,4);
    if (uVar1 == 0xffffffff) {
      uVar1 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,8);
      uVar2 = 0;
    }
    else if (0xffffffef < uVar1) {
      uVar2 = 0x19;
    }
    if (param_4 != (ulonglong *)0x0) {
      *param_4 = uVar1;
    }
  }
  return uVar2;
}


/* ================= FUN_140025ef0 @ 140025ef0  size=116  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001a6d0 */
/* CALLEES: FUN_140015c20, FUN_1400266f0 */

int FUN_140025ef0(undefined8 param_1,longlong param_2,undefined8 param_3,longlong param_4,
                 longlong *param_5,longlong *param_6,undefined8 param_7)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  longlong local_res10;
  
  lVar3 = 0;
  lVar2 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
  iVar1 = FUN_140015c20(param_4,&local_res10,param_7);
  if (iVar1 == 0) {
    lVar3 = (ulonglong)*(byte *)(param_4 + 0x49) * lVar2 + local_res10;
  }
  if (param_5 != (longlong *)0x0) {
    *param_5 = lVar2;
  }
  if (param_6 != (longlong *)0x0) {
    *param_6 = lVar3;
  }
  return iVar1;
}


/* ================= FUN_140026150 @ 140026150  size=116  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001a6d0 */
/* CALLEES: FUN_140015c40, FUN_1400266f0 */

int FUN_140026150(undefined8 param_1,longlong param_2,undefined8 param_3,longlong param_4,
                 longlong *param_5,longlong *param_6,undefined8 param_7)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  longlong local_res10;
  
  lVar3 = 0;
  lVar2 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
  iVar1 = FUN_140015c40(param_4,&local_res10,param_7);
  if (iVar1 == 0) {
    lVar3 = (ulonglong)*(byte *)(param_4 + 0x49) * lVar2 + local_res10;
  }
  if (param_5 != (longlong *)0x0) {
    *param_5 = lVar2;
  }
  if (param_6 != (longlong *)0x0) {
    *param_6 = lVar3;
  }
  return iVar1;
}


/* ================= FUN_1400261d0 @ 1400261d0  size=101  callers=4 callees=0 ================= */
/* CALLERS: FUN_14001a6d0, FUN_14001ae10, FUN_14001c0a0, FUN_14001d060 */

ulonglong FUN_1400261d0(longlong param_1,longlong *param_2)

{
  byte bVar1;
  byte bVar2;
  ulonglong uVar3;
  byte *pbVar4;
  ulonglong uVar5;
  ulonglong uVar6;
  
  uVar3 = 0;
  if ((param_1 != 0) && (param_2 != (longlong *)0x0)) {
    pbVar4 = (byte *)(*param_2 + param_1);
    uVar5 = uVar3;
    uVar6 = uVar3;
    do {
      bVar1 = *pbVar4;
      pbVar4 = pbVar4 + 1;
      *param_2 = *param_2 + 1;
      if ((int)uVar6 < 9) {
        bVar2 = (byte)uVar3;
        uVar3 = (ulonglong)((int)uVar3 + 7);
        uVar5 = uVar5 | (longlong)(int)((bVar1 & 0x7f) << (bVar2 & 0x1f));
        uVar6 = (ulonglong)((int)uVar6 + 1);
      }
    } while ((char)bVar1 < '\0');
    if (((int)uVar3 < 0x20) && ((bVar1 & 0x40) != 0)) {
      uVar5 = uVar5 | -1L << ((byte)uVar3 & 0x3f);
    }
    return uVar5;
  }
  return 0;
}


/* ================= FUN_140026240 @ 140026240  size=144  callers=4 callees=1 ================= */
/* CALLERS: FUN_14001a6d0, FUN_14001ae10, FUN_14001fe20, FUN_140021880 */
/* CALLEES: FUN_1400264e0 */

void FUN_140026240(longlong param_1,longlong param_2,undefined8 param_3,longlong param_4,
                  undefined8 *param_5,undefined8 *param_6,undefined8 param_7)

{
  undefined8 uVar1;
  undefined8 local_res8;
  
  local_res8 = 0;
  uVar1 = (**(code **)(param_1 + 0x1f8))
                    (*(undefined8 *)(param_2 + 8),param_3,*(undefined1 *)(param_4 + 0x49));
  FUN_1400264e0(param_1,param_4,0x13,uVar1,&local_res8,param_7);
  if (param_5 != (undefined8 *)0x0) {
    *param_5 = uVar1;
  }
  if (param_6 != (undefined8 *)0x0) {
    *param_6 = local_res8;
  }
  return;
}


/* ================= FUN_1400262d0 @ 1400262d0  size=53  callers=4 callees=0 ================= */
/* CALLERS: FUN_14001a6d0, FUN_14001ae10, FUN_14001fe20, FUN_140021880 */

undefined8
FUN_1400262d0(undefined8 param_1,longlong param_2,longlong *param_3,undefined8 param_4,
             undefined8 *param_5)

{
  char cVar1;
  char *pcVar2;
  
  pcVar2 = (char *)(*param_3 + *(longlong *)(param_2 + 8));
  if (param_5 != (undefined8 *)0x0) {
    *param_5 = pcVar2;
  }
  cVar1 = *pcVar2;
  while (cVar1 != '\0') {
    pcVar2 = pcVar2 + 1;
    cVar1 = *pcVar2;
  }
  *param_3 = (longlong)(pcVar2 + (1 - *(longlong *)(param_2 + 8)));
  return 0;
}


/* ================= FUN_140026310 @ 140026310  size=144  callers=4 callees=1 ================= */
/* CALLERS: FUN_14001a6d0, FUN_14001ae10, FUN_14001fe20, FUN_140021880 */
/* CALLEES: FUN_1400264e0 */

void FUN_140026310(longlong param_1,longlong param_2,undefined8 param_3,longlong param_4,
                  undefined8 *param_5,undefined8 *param_6,undefined8 param_7)

{
  undefined8 uVar1;
  undefined8 local_res8;
  
  local_res8 = 0;
  uVar1 = (**(code **)(param_1 + 0x1f8))
                    (*(undefined8 *)(param_2 + 8),param_3,*(undefined1 *)(param_4 + 0x49));
  FUN_1400264e0(param_1,param_4,3,uVar1,&local_res8,param_7);
  if (param_5 != (undefined8 *)0x0) {
    *param_5 = uVar1;
  }
  if (param_6 != (undefined8 *)0x0) {
    *param_6 = local_res8;
  }
  return;
}


/* ================= FUN_1400263a0 @ 1400263a0  size=167  callers=3 callees=2 ================= */
/* CALLERS: FUN_14001a6d0, FUN_14001fe20, FUN_140021880 */
/* CALLEES: FUN_140026610, FUN_1400266f0 */

undefined4
FUN_1400263a0(undefined8 param_1,longlong param_2,undefined8 param_3,undefined8 param_4,
             undefined8 *param_5,undefined8 *param_6,undefined8 *param_7,undefined8 param_8)

{
  undefined4 uVar1;
  undefined8 uVar2;
  undefined8 local_res10;
  undefined8 local_18 [2];
  
  local_18[0] = 0;
  local_res10 = 0;
  uVar2 = FUN_1400266f0(*(undefined8 *)(param_2 + 8),param_3);
  uVar1 = FUN_140026610(uVar2,param_1,param_4,&local_res10,local_18,param_8);
  if (param_5 != (undefined8 *)0x0) {
    *param_5 = uVar2;
  }
  if (param_6 != (undefined8 *)0x0) {
    *param_6 = local_res10;
  }
  if (param_7 != (undefined8 *)0x0) {
    *param_7 = local_18[0];
  }
  return uVar1;
}


/* ================= FUN_140026450 @ 140026450  size=144  callers=3 callees=1 ================= */
/* CALLERS: FUN_14001a6d0, FUN_14001fe20, FUN_140021880 */
/* CALLEES: FUN_140026610 */

void FUN_140026450(longlong param_1,longlong param_2,undefined8 param_3,undefined8 param_4,
                  undefined4 param_5,undefined8 *param_6,undefined8 param_7,undefined8 *param_8,
                  undefined8 param_9)

{
  undefined8 uVar1;
  undefined8 local_res8;
  
  local_res8 = 0;
  uVar1 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,param_5);
  FUN_140026610(uVar1,param_1,param_4,0,&local_res8,param_9);
  if (param_6 != (undefined8 *)0x0) {
    *param_6 = uVar1;
  }
  if (param_8 != (undefined8 *)0x0) {
    *param_8 = local_res8;
  }
  return;
}


/* ================= FUN_1400264e0 @ 1400264e0  size=294  callers=3 callees=4 ================= */
/* CALLERS: FUN_140026240, FUN_140026310, FUN_140026610 */
/* CALLEES: FUN_14001bb80, FUN_14001bd50, FUN_140026bb0, FUN_140026be0 */

undefined4
FUN_1400264e0(longlong param_1,undefined8 param_2,undefined4 param_3,ulonglong param_4,
             longlong *param_5,undefined8 param_6)

{
  longlong lVar1;
  undefined8 uVar2;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_6,0x1c,0,"_dwarf_read_string_from_section",0x30a);
    return 0x1c;
  }
  lVar1 = FUN_140026bb0(param_1,param_3);
  if (lVar1 == 0) {
    uVar2 = FUN_140026be0();
    FUN_14001bd50(param_1,param_6,0x1d,0,"_dwarf_read_string_from_section",0x312,uVar2,0,0);
    return 0x23;
  }
  if (*(ulonglong *)(lVar1 + 0x18) <= param_4) {
    FUN_14001bb80(param_1,param_6,0x1c,0,"_dwarf_read_string_from_section",0x31a);
    return 0x1c;
  }
  *param_5 = *(longlong *)(lVar1 + 8) + param_4;
  return 0;
}


/* ================= FUN_140026610 @ 140026610  size=209  callers=2 callees=3 ================= */
/* CALLERS: FUN_1400263a0, FUN_140026450 */
/* CALLEES: FUN_140015c60, FUN_140025e20, FUN_1400264e0 */

void FUN_140026610(longlong param_1,undefined8 param_2,longlong param_3,undefined8 *param_4,
                  undefined8 *param_5,undefined8 param_6)

{
  int iVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 local_res20;
  undefined8 local_38;
  longlong local_30;
  
  uVar3 = 0;
  local_38 = 0;
  local_res20 = 0;
  iVar1 = FUN_140015c60(param_3,&local_30,param_6);
  uVar2 = uVar3;
  if (iVar1 == 0) {
    iVar1 = FUN_140025e20(param_3,0x16,(ulonglong)*(byte *)(param_3 + 0x49) * param_1 + local_30,
                          &local_res20,param_6);
    uVar2 = local_res20;
    if (iVar1 == 0) {
      FUN_1400264e0(param_2,param_3,3,local_res20,&local_38,param_6);
      uVar3 = local_38;
    }
  }
  if (param_4 != (undefined8 *)0x0) {
    *param_4 = uVar2;
  }
  if (param_5 != (undefined8 *)0x0) {
    *param_5 = uVar3;
  }
  return;
}


/* ================= FUN_1400266f0 @ 1400266f0  size=119  callers=18 callees=0 ================= */
/* CALLERS: FUN_140019fb0, FUN_14001a6d0, FUN_14001ae10, FUN_14001b4a0, FUN_14001c010, FUN_14001c0a0, FUN_14001c5c0, FUN_14001d060, FUN_1400233d0, FUN_140023b50, FUN_140023c20, FUN_1400241f0, FUN_140024790, FUN_1400250c0, FUN_140025ad0, FUN_140025ef0, FUN_140026150, FUN_1400263a0 */

ulonglong FUN_1400266f0(longlong param_1,longlong *param_2)

{
  byte bVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  byte *pbVar5;
  ulonglong uVar6;
  
  uVar2 = 0;
  if ((param_1 != 0) && (param_2 != (longlong *)0x0)) {
    pbVar5 = (byte *)(*param_2 + param_1);
    uVar3 = uVar2;
    uVar6 = uVar2;
    do {
      bVar1 = *pbVar5;
      pbVar5 = pbVar5 + 1;
      *param_2 = *param_2 + 1;
      uVar4 = uVar3;
      if ((int)uVar6 < 9) {
        uVar4 = (ulonglong)((int)uVar3 + 7);
        uVar2 = uVar2 | (longlong)(int)((bVar1 & 0x7f) << ((byte)uVar3 & 0x1f));
        uVar6 = (ulonglong)((int)uVar6 + 1);
      }
      uVar3 = uVar4;
    } while ((char)bVar1 < '\0');
    return uVar2;
  }
  return 0;
}


/* ================= FUN_140026770 @ 140026770  size=286  callers=2 callees=4 ================= */
/* CALLERS: FUN_140025c60, FUN_140025e20 */
/* CALLEES: FUN_14001bb80, FUN_14001bd50, FUN_140026bb0, FUN_140026be0 */

undefined4
FUN_140026770(longlong *param_1,undefined8 param_2,ulonglong param_3,undefined4 param_4,
             undefined8 *param_5,undefined8 param_6)

{
  longlong lVar1;
  undefined4 uVar2;
  longlong lVar3;
  undefined8 uVar4;
  ulonglong local_res8;
  
  lVar1 = *param_1;
  if (lVar1 == 0) {
    FUN_14001bb80(0,param_6,0x1c,0,"_dwarf_read_value_from_section",0x253);
    uVar2 = 0x1c;
  }
  else {
    lVar3 = FUN_140026bb0(lVar1);
    if (lVar3 == 0) {
      uVar4 = FUN_140026be0();
      FUN_14001bd50(lVar1,param_6,0x1d,0,"_dwarf_read_value_from_section",0x25c,uVar4,0,0);
      uVar2 = 0x23;
    }
    else if (param_3 < *(ulonglong *)(lVar3 + 0x18)) {
      local_res8 = param_3;
      uVar4 = (**(code **)(lVar1 + 0x1f8))(*(undefined8 *)(lVar3 + 8),&local_res8,param_4);
      *param_5 = uVar4;
      uVar2 = 0;
    }
    else {
      FUN_14001bb80(lVar1,param_6,0x1c,0,"_dwarf_read_value_from_section",0x264);
      uVar2 = 0x1c;
    }
  }
  return uVar2;
}


/* ================= FUN_140026890 @ 140026890  size=158  callers=1 callees=0 ================= */
/* CALLERS: FUN_140026930 */

void FUN_140026890(longlong param_1,longlong *param_2,undefined8 param_3,int param_4)

{
  undefined1 *puVar1;
  
  if (param_1 == 0) {
    return;
  }
  if (param_2 == (longlong *)0x0) {
    return;
  }
  puVar1 = (undefined1 *)(param_1 + *param_2);
  if (param_4 == 8) {
    puVar1[7] = (char)((ulonglong)param_3 >> 0x38);
    puVar1[6] = (char)((ulonglong)param_3 >> 0x30);
    puVar1[5] = (char)((ulonglong)param_3 >> 0x28);
    puVar1[4] = (char)((ulonglong)param_3 >> 0x20);
LAB_140026906:
    puVar1[3] = (char)((ulonglong)param_3 >> 0x18);
    puVar1[2] = (char)((ulonglong)param_3 >> 0x10);
  }
  else {
    if (((param_4 - 2U & 0xfffffffd) != 0) && (param_4 != 1)) {
      return;
    }
    if (3 < param_4) goto LAB_140026906;
    if (param_4 < 2) {
      if (0 < param_4) {
        *puVar1 = (char)param_3;
        *param_2 = *param_2 + (longlong)param_4;
        return;
      }
      goto LAB_140026927;
    }
  }
  *puVar1 = (char)param_3;
  puVar1[1] = (char)((ulonglong)param_3 >> 8);
LAB_140026927:
  *param_2 = *param_2 + (longlong)param_4;
  return;
}


/* ================= FUN_140026930 @ 140026930  size=235  callers=0 callees=3 ================= */
/* CALLEES: FUN_14001bb80, FUN_140026890, realloc */

undefined8
FUN_140026930(undefined8 *param_1,ulonglong *param_2,longlong *param_3,undefined8 param_4,
             int param_5,undefined8 param_6)

{
  void *pvVar1;
  undefined8 uVar2;
  ulonglong uVar3;
  
  if (((param_1 == (undefined8 *)0x0) || (param_2 == (ulonglong *)0x0)) ||
     (param_3 == (longlong *)0x0)) {
    uVar2 = 0x1c;
  }
  else {
    uVar3 = *param_2;
    if (uVar3 < (ulonglong)(*param_3 + (longlong)param_5)) {
      do {
        *param_2 = uVar3 * 2;
        pvVar1 = realloc((void *)*param_1,uVar3 * 2);
        *param_1 = pvVar1;
        if (pvVar1 == (void *)0x0) {
          FUN_14001bb80(0,param_6,1,0,"_dwarf_write_lsb_alloc",0xda);
          return 1;
        }
        uVar3 = *param_2;
      } while (uVar3 < (ulonglong)(*param_3 + (longlong)param_5));
    }
    FUN_140026890(*param_1,param_3,param_4,param_5);
    uVar2 = 0;
  }
  return uVar2;
}


/* ================= FUN_140026a20 @ 140026a20  size=152  callers=1 callees=0 ================= */
/* CALLERS: FUN_140026ac0 */

void FUN_140026a20(longlong param_1,longlong *param_2,ulonglong param_3,int param_4)

{
  undefined1 *puVar1;
  
  if (param_1 == 0) {
    return;
  }
  if (param_2 == (longlong *)0x0) {
    return;
  }
  puVar1 = (undefined1 *)(param_1 + *param_2);
  if (param_4 == 8) {
    puVar1[7] = (char)param_3;
    puVar1[6] = (char)(param_3 >> 8);
    puVar1[5] = (char)(param_3 >> 0x10);
    puVar1[4] = (char)(param_3 >> 0x18);
    param_3 = param_3 >> 0x20;
LAB_140026a94:
    puVar1[3] = (char)param_3;
    puVar1[2] = (char)(param_3 >> 8);
    param_3 = param_3 >> 0x10;
  }
  else {
    if (((param_4 - 2U & 0xfffffffd) != 0) && (param_4 != 1)) {
      return;
    }
    if (3 < param_4) goto LAB_140026a94;
    if (param_4 < 2) {
      if (0 < param_4) {
        *puVar1 = (char)param_3;
        *param_2 = *param_2 + (longlong)param_4;
        return;
      }
      goto LAB_140026ab1;
    }
  }
  puVar1[1] = (char)param_3;
  *puVar1 = (char)(param_3 >> 8);
LAB_140026ab1:
  *param_2 = *param_2 + (longlong)param_4;
  return;
}


/* ================= FUN_140026ac0 @ 140026ac0  size=235  callers=0 callees=3 ================= */
/* CALLEES: FUN_14001bb80, FUN_140026a20, realloc */

undefined8
FUN_140026ac0(undefined8 *param_1,ulonglong *param_2,longlong *param_3,undefined8 param_4,
             int param_5,undefined8 param_6)

{
  void *pvVar1;
  undefined8 uVar2;
  ulonglong uVar3;
  
  if (((param_1 == (undefined8 *)0x0) || (param_2 == (ulonglong *)0x0)) ||
     (param_3 == (longlong *)0x0)) {
    uVar2 = 0x1c;
  }
  else {
    uVar3 = *param_2;
    if (uVar3 < (ulonglong)(*param_3 + (longlong)param_5)) {
      do {
        *param_2 = uVar3 * 2;
        pvVar1 = realloc((void *)*param_1,uVar3 * 2);
        *param_1 = pvVar1;
        if (pvVar1 == (void *)0x0) {
          FUN_14001bb80(0,param_6,1,0,"_dwarf_write_msb_alloc",0x112);
          return 1;
        }
        uVar3 = *param_2;
      } while (uVar3 < (ulonglong)(*param_3 + (longlong)param_5));
    }
    FUN_140026a20(*param_1,param_3,param_4,param_5);
    uVar2 = 0;
  }
  return uVar2;
}


/* ================= FUN_140026bb0 @ 140026bb0  size=44  callers=14 callees=0 ================= */
/* CALLERS: FUN_140019950, FUN_140019e60, FUN_140019fb0, FUN_14001e170, FUN_14001ebb0, FUN_1400202a0, FUN_140022a40, FUN_1400230d0, FUN_140023f20, FUN_140024c00, FUN_140025c90, FUN_1400264e0, FUN_140026770, FUN_140026f70 */

longlong FUN_140026bb0(longlong param_1,uint param_2)

{
  longlong lVar1;
  
  if ((((param_1 != 0) && (param_2 < 0x17)) &&
      (lVar1 = *(longlong *)(param_1 + 0x38 + (longlong)(int)param_2 * 8), -1 < lVar1)) &&
     (lVar1 < *(int *)(param_1 + 0x30))) {
    return lVar1 * 0x20 + *(longlong *)(param_1 + 8);
  }
  return 0;
}


/* ================= FUN_140026be0 @ 140026be0  size=23  callers=3 callees=0 ================= */
/* CALLERS: FUN_140024c00, FUN_1400264e0, FUN_140026770 */

undefined * FUN_140026be0(int param_1)

{
  if (param_1 < 0x17) {
    return (&PTR_s__debug_info_140096080)[param_1];
  }
  return (undefined *)0x0;
}


/* ================= FUN_140026c00 @ 140026c00  size=254  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001f580 */
/* CALLEES: free */

void FUN_140026c00(longlong param_1)

{
  longlong *plVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  longlong lVar4;
  undefined8 *puVar5;
  undefined8 *_Memory;
  
  plVar1 = (longlong *)(param_1 + 0x2f0);
  _Memory = (undefined8 *)*plVar1;
  if (_Memory != (undefined8 *)0x0) {
    do {
      puVar2 = (undefined8 *)*plVar1;
      puVar3 = (undefined8 *)_Memory[6];
      if (puVar2 == _Memory) {
        lVar4 = puVar2[6];
        *plVar1 = lVar4;
        if (lVar4 == 0) {
          *(longlong **)(param_1 + 0x2f8) = plVar1;
        }
      }
      else {
        for (puVar5 = (undefined8 *)puVar2[6]; puVar5 != _Memory; puVar5 = (undefined8 *)puVar5[6])
        {
          puVar2 = puVar5;
        }
        lVar4 = *(longlong *)(puVar2[6] + 0x30);
        puVar2[6] = lVar4;
        if (lVar4 == 0) {
          *(undefined8 **)(param_1 + 0x2f8) = puVar2 + 6;
        }
      }
      if ((void *)*_Memory != (void *)0x0) {
        free((void *)*_Memory);
      }
      if ((void *)_Memory[1] != (void *)0x0) {
        free((void *)_Memory[1]);
      }
      free(_Memory);
      _Memory = puVar3;
    } while (puVar3 != (undefined8 *)0x0);
    *(undefined8 *)(param_1 + 0x300) = 0;
    *(undefined8 *)(param_1 + 0x308) = 0;
    return;
  }
  *(undefined8 *)(param_1 + 0x300) = 0;
  *(undefined8 *)(param_1 + 0x308) = 0;
  return;
}


/* ================= FUN_140026d00 @ 140026d00  size=457  callers=10 callees=3 ================= */
/* CALLERS: FUN_14001fe20, FUN_140020a70, FUN_1400211b0, FUN_140021610, FUN_140021720, FUN_140021880, FUN_140021b20, FUN_140021c60, FUN_140023d70, FUN_140026d00 */
/* CALLEES: FUN_140025a00, FUN_140025a70, FUN_140026d00 */

void FUN_140026d00(longlong param_1,longlong param_2,longlong *param_3,int param_4,longlong param_5)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined8 uVar4;
  char *pcVar5;
  
  switch(param_5) {
  default:
    *param_3 = *param_3 + (longlong)param_4;
    break;
  case 3:
    uVar4 = 2;
    goto LAB_140026dfd;
  case 4:
    uVar4 = 4;
    goto LAB_140026dfd;
  case 5:
  case 0x12:
  case 0x26:
  case 0x2a:
    *param_3 = *param_3 + 2;
    break;
  case 6:
  case 0x13:
  case 0x1c:
  case 0x28:
  case 0x2c:
    *param_3 = *param_3 + 4;
    break;
  case 7:
  case 0x14:
  case 0x20:
  case 0x24:
    *param_3 = *param_3 + 8;
    break;
  case 8:
    pcVar5 = (char *)(*param_3 + *(longlong *)(param_2 + 8));
    cVar1 = *pcVar5;
    while (cVar1 != '\0') {
      pcVar5 = pcVar5 + 1;
      cVar1 = *pcVar5;
    }
    *param_3 = (longlong)(pcVar5 + (1 - *(longlong *)(param_2 + 8)));
    break;
  case 9:
  case 0x18:
    param_5 = *(longlong *)(param_2 + 8) + *param_3;
    lVar3 = FUN_140025a70(&param_5);
    *param_3 = (lVar3 - *(longlong *)(param_2 + 8)) + param_5;
    break;
  case 10:
    uVar4 = 1;
LAB_140026dfd:
    iVar2 = (**(code **)(param_1 + 0x1f8))(*(undefined8 *)(param_2 + 8),param_3,uVar4);
    *param_3 = *param_3 + (longlong)iVar2;
    break;
  case 0xb:
  case 0xc:
  case 0x11:
  case 0x25:
  case 0x29:
    *param_3 = *param_3 + 1;
    break;
  case 0xd:
    param_5 = *(longlong *)(param_2 + 8) + *param_3;
    FUN_140025a00(&param_5);
    *param_3 = param_5 - *(longlong *)(param_2 + 8);
    break;
  case 0xf:
  case 0x15:
  case 0x1a:
  case 0x1b:
  case 0x22:
  case 0x23:
    param_5 = *(longlong *)(param_2 + 8) + *param_3;
    FUN_140025a70(&param_5);
    *param_3 = param_5 - *(longlong *)(param_2 + 8);
    break;
  case 0x16:
    param_5 = *(longlong *)(param_2 + 8) + *param_3;
    uVar4 = FUN_140025a70(&param_5);
    *param_3 = param_5 - *(longlong *)(param_2 + 8);
    FUN_140026d00(param_1,param_2,param_3,param_4,uVar4);
    break;
  case 0x19:
    break;
  case 0x1e:
    *param_3 = *param_3 + 0x10;
    break;
  case 0x21:
    param_5 = *(longlong *)(param_2 + 8) + *param_3;
    FUN_140025a70(&param_5);
    FUN_140025a00(&param_5);
    *param_3 = param_5 - *(longlong *)(param_2 + 8);
    break;
  case 0x27:
  case 0x2b:
    *param_3 = *param_3 + 3;
  }
  return;
}


/* ================= FUN_140026f40 @ 140026f40  size=36  callers=1 callees=0 ================= */
/* CALLERS: FUN_14001eb00 */

void FUN_140026f40(longlong param_1)

{
  if ((param_1 != 0) && ((*(int *)(param_1 + 0xf0) - 2U & 0xfffffffd) == 0)) {
                    /* WARNING: Could not recover jumptable at 0x000140026f5c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    free(*(void **)(param_1 + 0x1c0));
    return;
  }
  return;
}


/* ================= FUN_140026f70 @ 140026f70  size=461  callers=1 callees=5 ================= */
/* CALLERS: FUN_14001f440 */
/* CALLEES: FUN_14001bb80, FUN_14001bd50, FUN_140026bb0, calloc, memcpy */

undefined4 FUN_140026f70(longlong param_1,undefined8 param_2)

{
  undefined8 uVar1;
  undefined1 *puVar2;
  longlong lVar3;
  void *_Dst;
  undefined4 uVar4;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_2,0x1c,0,"_dwarf_strtab_init",0x4d);
    return 0x1c;
  }
  if ((*(int *)(param_1 + 0xf0) == 1) || (*(int *)(param_1 + 0xf0) == 4)) {
    lVar3 = FUN_140026bb0(param_1,3);
    if (lVar3 == 0) {
      *(undefined8 *)(param_1 + 0x1c0) = 0;
      *(undefined8 *)(param_1 + 0x1d0) = 0;
      *(undefined8 *)(param_1 + 0x1c8) = 0;
      FUN_14001bd50(param_1,param_2,0x1d,0,"_dwarf_strtab_init",0x56,".debug_str",0,0);
      return 0;
    }
    uVar1 = *(undefined8 *)(lVar3 + 0x18);
    *(undefined8 *)(param_1 + 0x1d0) = uVar1;
    *(undefined8 *)(param_1 + 0x1c8) = uVar1;
    if (*(int *)(param_1 + 0xf0) != 4) {
      *(undefined8 *)(param_1 + 0x1c0) = *(undefined8 *)(lVar3 + 8);
      return 0;
    }
    _Dst = calloc(1,*(size_t *)(lVar3 + 0x18));
    *(void **)(param_1 + 0x1c0) = _Dst;
    if (_Dst != (void *)0x0) {
      memcpy(_Dst,*(void **)(lVar3 + 8),*(size_t *)(lVar3 + 0x18));
      return 0;
    }
    uVar4 = 0x5e;
  }
  else {
    *(undefined8 *)(param_1 + 0x1c8) = 0x400;
    *(undefined8 *)(param_1 + 0x1d0) = 0;
    puVar2 = calloc(1,0x400);
    *(undefined1 **)(param_1 + 0x1c0) = puVar2;
    if (puVar2 != (undefined1 *)0x0) {
      *puVar2 = 0;
      return 0;
    }
    uVar4 = 0x6c;
  }
  FUN_14001bb80(param_1,param_2,1,0,"_dwarf_strtab_init",uVar4);
  return 1;
}


/* ================= FUN_140027140 @ 140027140  size=103  callers=45 callees=1 ================= */
/* CALLERS: FUN_1400273a0, FUN_140027700, FUN_1400277d0, FUN_140027a00, FUN_14002b940, FUN_14002bb30, FUN_14002bc30, FUN_14002c250, FUN_14002f8f0, FUN_140033060, FUN_140033540, FUN_140033e50, FUN_140034d30, FUN_1400359e0, FUN_140036ec0, FUN_140037720, FUN_140039600, FUN_140039a10, FUN_140039c60, FUN_140039e50, FUN_14003a0e0, FUN_14003d210, FUN_14003dcb0, FUN_14003dd90, FUN_14003dfa0, FUN_14003e110, FUN_14003e420, FUN_14003f930, FUN_14003fc90, FUN_140040320, FUN_1400404c0, FUN_140041780, FUN_140043210, FUN_140043430, FUN_1400434e0, FUN_140043590, FUN_140043ad0, FUN_140044920, FUN_140047390, FUN_1400476d0, FUN_14004e2c0, FUN_14004e310, FUN_14004e4b0, FUN_14004ec20, FUN_140050030 */
/* CALLEES: memcpy */

ulonglong FUN_140027140(void *param_1,void *param_2,int param_3)

{
  ulonglong uVar1;
  ulonglong uVar2;
  size_t _Size;
  
  uVar1 = 0xffffffffffffffff;
  if (((param_1 != (void *)0x0) && (param_2 != (void *)0x0)) && (param_3 != 0)) {
    do {
      uVar2 = uVar1;
      uVar1 = uVar2 + 1;
    } while (*(char *)((longlong)param_2 + uVar1) != '\0');
    _Size = (size_t)(param_3 - 1U);
    if ((uint)uVar1 < param_3 - 1U) {
      _Size = uVar1 & 0xffffffff;
    }
    if ((int)_Size != 0) {
      memcpy(param_1,param_2,_Size);
    }
    *(undefined1 *)(_Size + (longlong)param_1) = 0;
    uVar1 = (ulonglong)((int)uVar2 + 2);
  }
  return uVar1;
}


/* ================= FUN_1400271b0 @ 1400271b0  size=8  callers=1 callees=0 ================= */
/* CALLERS: FUN_14002ce80 */

char * FUN_1400271b0(void)

{
  return "SEGGER Microcontroller GmbH";
}


/* ================= FUN_1400271c0 @ 1400271c0  size=8  callers=1 callees=0 ================= */
/* CALLERS: FUN_14002ce80 */

undefined * FUN_1400271c0(void)

{
  return &DAT_140063e4f;
}


/* ================= FUN_1400271d0 @ 1400271d0  size=17  callers=2 callees=0 ================= */
/* CALLERS: FUN_1400273a0, FUN_140033540 */

ulonglong FUN_1400271d0(longlong param_1,longlong param_2)

{
  ulonglong uVar1;
  
  uVar1 = (ulonglong)(uint)((int)param_2 - (int)param_1);
  if (-1 < param_1 - param_2) {
    uVar1 = param_1 - param_2 & 0xffffffff;
  }
  return uVar1;
}


/* ================= FUN_1400271f0 @ 1400271f0  size=156  callers=1 callees=0 ================= */
/* CALLERS: FUN_140041780 */

void FUN_1400271f0(char *param_1,uint param_2)

{
  char cVar1;
  
  if (param_1 != (char *)0x0) {
    *(uint *)(param_1 + 4) = param_2 / 10000;
    *(uint *)(param_1 + 8) = (param_2 / 100) % 100;
    param_2 = param_2 % 100;
    if (0x1a < param_2) {
      *param_1 = 'z';
      param_1[3] = '\0';
      cVar1 = (char)((ulonglong)(param_2 - 0x1a) / 10);
      param_1[1] = cVar1 + '0';
      param_1[2] = (char)(param_2 - 0x1a) + cVar1 * -10 + '0';
      return;
    }
    if (param_2 != 0) {
      param_1[1] = '\0';
      *param_1 = (char)param_2 + '`';
      return;
    }
    *param_1 = '\0';
  }
  return;
}


/* ================= FUN_140027290 @ 140027290  size=33  callers=2 callees=0 ================= */
/* CALLERS: FUN_14002c250, FUN_14003dcb0 */

undefined8 FUN_140027290(char *param_1)

{
  char cVar1;
  
  cVar1 = *param_1;
  while( true ) {
    if (cVar1 == '\0') {
      return 1;
    }
    if (9 < (byte)(cVar1 - 0x30U)) break;
    cVar1 = param_1[1];
    param_1 = param_1 + 1;
  }
  return 0;
}


/* ================= FUN_1400272c0 @ 1400272c0  size=169  callers=5 callees=2 ================= */
/* CALLERS: FUN_14002c250, FUN_140037720, FUN_140039ca0, FUN_140039e90, FUN_14003dcb0 */
/* CALLEES: FUN_140027880, FUN_1400278e0 */

undefined8 FUN_1400272c0(byte *param_1,undefined4 *param_2)

{
  byte *pbVar1;
  byte bVar2;
  char cVar3;
  int iVar4;
  byte *pbVar5;
  ulonglong uVar6;
  ulonglong uVar7;
  
  bVar2 = *param_1;
  uVar7 = (ulonglong)bVar2;
  uVar6 = 0;
  if ((bVar2 == 0x30) && ((param_1[1] + 0xa8 & 0xdf) == 0)) {
    pbVar5 = param_1 + 2;
    iVar4 = FUN_140027880(param_1[2]);
    if (-1 < iVar4) {
      do {
        pbVar1 = pbVar5 + 1;
        pbVar5 = pbVar5 + 1;
        uVar6 = (ulonglong)(uint)((int)uVar6 * 0x10 + iVar4);
        iVar4 = FUN_140027880(*pbVar1);
      } while (-1 < iVar4);
      *param_2 = (int)uVar6;
      return 0;
    }
  }
  else {
    cVar3 = FUN_1400278e0(bVar2);
    if (cVar3 != '\0') {
      do {
        cVar3 = (char)uVar7;
        uVar7 = (ulonglong)param_1[1];
        param_1 = param_1 + 1;
        uVar6 = (ulonglong)(uint)((int)cVar3 + ((int)uVar6 * 5 + -0x18) * 2);
        cVar3 = FUN_1400278e0(uVar7);
      } while (cVar3 != '\0');
      *param_2 = (int)uVar6;
      return 0;
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140027370 @ 140027370  size=43  callers=1 callees=1 ================= */
/* CALLERS: FUN_140043ad0 */
/* CALLEES: FUN_1400278f0 */

void FUN_140027370(undefined8 param_1,undefined4 *param_2)

{
  undefined8 local_res10 [3];
  
  local_res10[0] = 0;
  FUN_1400278f0(param_1,local_res10,0);
  *param_2 = (undefined4)local_res10[0];
  return;
}


/* ================= FUN_1400273a0 @ 1400273a0  size=293  callers=1 callees=5 ================= */
/* CALLERS: FUN_14002fad0 */
/* CALLEES: FUN_140027140, FUN_1400271d0, FUN_1400274d0, FUN_14002c8f0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

int FUN_1400273a0(undefined8 *param_1,uint param_2,char *param_3,int param_4)

{
  uint uVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  undefined *puVar6;
  char *pcVar7;
  uint uVar8;
  undefined1 auStack_b8 [32];
  undefined1 *local_98;
  undefined1 local_88 [64];
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_b8;
  iVar5 = 0;
  uVar8 = 0;
  *param_3 = '\0';
  pcVar7 = param_3;
  if (param_2 != 0) {
    do {
      iVar3 = 0;
      for (piVar2 = (int *)*param_1; (piVar2 != (int *)0x0 && (*piVar2 != 0)); piVar2 = piVar2 + 3)
      {
        iVar3 = iVar3 + *piVar2 * piVar2[1];
      }
      iVar5 = iVar5 + iVar3;
      FUN_1400274d0(iVar3,local_88,0x40);
      iVar3 = FUN_1400271d0(pcVar7,param_3);
      puVar6 = &DAT_14005f306;
      uVar4 = param_4 - iVar3;
      if (uVar8 != 0) {
        puVar6 = &DAT_140063e80;
      }
      local_98 = local_88;
      uVar1 = FUN_14002c8f0(pcVar7,uVar4,&DAT_1400631dc,puVar6);
      if (uVar1 < uVar4) {
        uVar4 = uVar1;
      }
      param_1 = param_1 + 1;
      uVar8 = uVar8 + 1;
      pcVar7 = pcVar7 + uVar4;
    } while (uVar8 < param_2);
    if (*param_3 != '\0') {
      return iVar5;
    }
  }
  FUN_140027140(param_3,&DAT_14005f85c,param_4);
  return 0;
}


/* ================= FUN_1400274d0 @ 1400274d0  size=467  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400273a0 */
/* CALLEES: FUN_140027700, FUN_14002c8f0 */

void FUN_1400274d0(ulonglong param_1,undefined1 *param_2,undefined4 param_3)

{
  char *pcVar1;
  ulonglong uVar2;
  
  *param_2 = 0;
  if (param_1 == 0) {
    uVar2 = 0;
    pcVar1 = "%d Bytes";
  }
  else if ((param_1 & 0x3fffffff) == 0) {
    pcVar1 = "%d GB";
    uVar2 = param_1 >> 0x1e & 0xffffffff;
  }
  else {
    if (((param_1 & 0x1fffffff) == 0) && (0x3fffffff < param_1)) {
      FUN_14002c8f0(param_2,param_3,"%lld.%lld GB",param_1 >> 0x1e,
                    (param_1 * 10 >> 0x1e) + (param_1 >> 0x1e) * -10);
      return;
    }
    if ((param_1 & 0xfffff) == 0) {
      pcVar1 = "%d MB";
      uVar2 = param_1 >> 0x14 & 0xffffffff;
    }
    else {
      if (((param_1 & 0x7ffff) == 0) && (0xfffff < param_1)) {
        FUN_14002c8f0(param_2,param_3,"%lld.%lld MB",param_1 >> 0x14,
                      (param_1 * 10 >> 0x14) + (param_1 >> 0x14) * -10);
        return;
      }
      if ((param_1 & 0x3ff) != 0) {
        if (((param_1 & 0x1ff) == 0) && (0x3ff < param_1)) {
          FUN_14002c8f0(param_2,param_3,"%lld.%lld KB",param_1 >> 10,
                        (param_1 * 10 >> 10) + (param_1 >> 10) * -10);
          return;
        }
        FUN_14002c8f0(param_2,param_3,"%llu Byte",param_1);
        if (param_1 == 1) {
          return;
        }
        FUN_140027700(param_2,&DAT_140063e34,param_3);
        return;
      }
      pcVar1 = "%d KB";
      uVar2 = param_1 >> 10 & 0xffffffff;
    }
  }
  FUN_14002c8f0(param_2,param_3,pcVar1,uVar2);
  return;
}


/* ================= FUN_1400276b0 @ 1400276b0  size=26  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003de20 */

void FUN_1400276b0(char *param_1,char param_2,char param_3)

{
  char *pcVar1;
  char cVar2;
  
  cVar2 = *param_1;
  while (cVar2 != '\0') {
    if (cVar2 == param_2) {
      *param_1 = param_3;
    }
    pcVar1 = param_1 + 1;
    param_1 = param_1 + 1;
    cVar2 = *pcVar1;
  }
  return;
}


/* ================= FUN_1400276d0 @ 1400276d0  size=44  callers=1 callees=1 ================= */
/* CALLERS: FUN_140033540 */
/* CALLEES: FUN_1400279f0 */

void FUN_1400276d0(char *param_1)

{
  char cVar1;
  
  cVar1 = *param_1;
  while (cVar1 != '\0') {
    cVar1 = FUN_1400279f0((int)cVar1);
    *param_1 = cVar1;
    param_1 = param_1 + 1;
    cVar1 = *param_1;
  }
  return;
}


/* ================= FUN_140027700 @ 140027700  size=90  callers=16 callees=2 ================= */
/* CALLERS: FUN_1400274d0, FUN_140027a00, FUN_14002be90, FUN_140034d30, FUN_1400368e0, FUN_14003f930, FUN_140040320, FUN_140041780, FUN_1400423e0, FUN_140043210, FUN_140043430, FUN_1400476d0, FUN_140048540, FUN_140048680, FUN_14004fd40, FUN_140050030 */
/* CALLEES: FUN_140027140, FUN_1400277b0 */

void FUN_140027700(longlong param_1,undefined8 param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = FUN_1400277b0();
  iVar2 = FUN_1400277b0(param_2);
  iVar3 = (param_3 - iVar1) + -1;
  if (iVar2 < iVar3) {
    iVar3 = iVar2;
  }
  FUN_140027140(param_1 + iVar1,param_2,iVar3 + 1);
  return;
}


/* ================= FUN_140027760 @ 140027760  size=73  callers=13 callees=0 ================= */
/* CALLERS: FUN_140039af0, FUN_140039b60, FUN_14003aca0, FUN_14003d210, FUN_140040050, FUN_1400419f0, FUN_140041cd0, FUN_140042100, FUN_1400429c0, FUN_140043ad0, FUN_14004d9c0, FUN_14004e310, FUN_1400512f0 */

int FUN_140027760(char *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  
  cVar1 = (&DAT_140063ca0)[*param_2];
  cVar2 = (&DAT_140063ca0)[*param_1];
  while( true ) {
    if (cVar2 == '\0') break;
    param_1 = param_1 + 1;
    param_2 = param_2 + 1;
    if ((int)cVar2 != (int)cVar1) break;
    cVar2 = (&DAT_140063ca0)[*param_1];
    cVar1 = (&DAT_140063ca0)[*param_2];
  }
  return (int)cVar2 - (int)cVar1;
}


/* ================= FUN_1400277b0 @ 1400277b0  size=30  callers=36 callees=0 ================= */
/* CALLERS: FUN_140027700, FUN_1400297e0, FUN_140033e50, FUN_1400359e0, FUN_1400368e0, FUN_1400377e0, FUN_140039600, FUN_140039a10, FUN_140039b60, FUN_14003a000, FUN_14003a0e0, FUN_14003a240, FUN_14003a710, FUN_14003a7d0, FUN_14003dfa0, FUN_14003f280, FUN_140040cd0, FUN_140041470, FUN_140041cd0, FUN_1400423a0, FUN_1400423e0, FUN_1400434e0, FUN_140043590, FUN_1400440e0, FUN_140046110, FUN_14004e610, FUN_14004efc0, FUN_14004f0c0, FUN_14004f2a0, FUN_14004f330, FUN_14004f3a0, FUN_14004f470, FUN_14004f510, FUN_14004f6c0, FUN_140051020, FUN_140056ad0 */

int FUN_1400277b0(char *param_1)

{
  char cVar1;
  int iVar2;
  
  iVar2 = 0;
  cVar1 = *param_1;
  while (cVar1 != '\0') {
    param_1 = param_1 + 1;
    iVar2 = iVar2 + 1;
    cVar1 = *param_1;
  }
  return iVar2;
}


/* ================= FUN_1400277d0 @ 1400277d0  size=18  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400359e0 */
/* CALLEES: FUN_140027140 */

void FUN_1400277d0(undefined8 param_1,undefined8 param_2,int param_3,uint param_4)

{
  if (param_3 + 1U < param_4) {
    param_4 = param_3 + 1U;
  }
  FUN_140027140(param_1,param_2,param_4);
  return;
}


/* ================= FUN_1400277f0 @ 1400277f0  size=102  callers=8 callees=1 ================= */
/* CALLERS: FUN_14003a240, FUN_14003a710, FUN_14003a7d0, FUN_140041cd0, FUN_14004f3a0, FUN_14004f470, FUN_140051020, FUN_140056ad0 */
/* CALLEES: FUN_1400279f0 */

int FUN_1400277f0(longlong param_1,char *param_2,longlong param_3)

{
  char *pcVar1;
  char cVar2;
  char cVar3;
  ulonglong uVar4;
  
  if (param_3 != 0) {
    param_1 = param_1 - (longlong)param_2;
    do {
      pcVar1 = param_2 + param_1;
      param_3 = param_3 + -1;
      uVar4 = (ulonglong)(uint)(int)*param_2;
      param_2 = param_2 + 1;
      cVar2 = FUN_1400279f0((int)*pcVar1);
      cVar3 = FUN_1400279f0(uVar4 & 0xffffffff);
      if (cVar2 != cVar3) {
        return (int)cVar2 - (int)cVar3;
      }
    } while ((cVar2 != '\0') && (param_3 != 0));
  }
  return 0;
}


/* ================= FUN_140027860 @ 140027860  size=22  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003fc90 */

char FUN_140027860(char param_1)

{
  char cVar1;
  
  cVar1 = param_1 + ' ';
  if (0x19 < (byte)(param_1 + 0xbfU)) {
    cVar1 = param_1;
  }
  return cVar1;
}


/* ================= FUN_140027880 @ 140027880  size=48  callers=2 callees=0 ================= */
/* CALLERS: FUN_1400272c0, FUN_1400278f0 */

int FUN_140027880(char param_1)

{
  if ((byte)(param_1 - 0x30U) < 10) {
    return param_1 + -0x30;
  }
  if ((byte)(param_1 + 0x9fU) < 6) {
    return param_1 + -0x57;
  }
  if ((byte)(param_1 + 0xbfU) < 6) {
    return param_1 + -0x37;
  }
  return -1;
}


/* ================= FUN_1400278b0 @ 1400278b0  size=40  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400278f0 */

void FUN_1400278b0(undefined8 *param_1)

{
  byte *pbVar1;
  
  pbVar1 = (byte *)*param_1;
  while ((*pbVar1 < 0x21 && ((0x100002600U >> ((longlong)(char)*pbVar1 & 0x3fU) & 1) != 0))) {
    pbVar1 = pbVar1 + 1;
  }
  *param_1 = pbVar1;
  return;
}


/* ================= FUN_1400278e0 @ 1400278e0  size=10  callers=2 callees=0 ================= */
/* CALLERS: FUN_1400272c0, FUN_1400278f0 */

bool FUN_1400278e0(char param_1)

{
  return (byte)(param_1 - 0x30U) < 10;
}


/* ================= FUN_1400278f0 @ 1400278f0  size=236  callers=1 callees=3 ================= */
/* CALLERS: FUN_140027370 */
/* CALLEES: FUN_140027880, FUN_1400278b0, FUN_1400278e0 */

undefined8 FUN_1400278f0(undefined8 *param_1,ulonglong *param_2,int param_3)

{
  byte *pbVar1;
  byte bVar2;
  char cVar3;
  uint uVar4;
  ulonglong uVar5;
  ulonglong uVar6;
  int iVar7;
  byte *pbVar8;
  ulonglong uVar9;
  byte *local_res8;
  
  local_res8 = (byte *)*param_1;
  uVar9 = 0;
  FUN_1400278b0(&local_res8);
  uVar5 = uVar9 & 0xffffffff;
  pbVar8 = local_res8;
  if (*local_res8 == 0x2d) {
    pbVar8 = local_res8 + 1;
    uVar5 = (ulonglong)((int)uVar9 + 1);
  }
  bVar2 = *pbVar8;
  uVar6 = (ulonglong)bVar2;
  if ((bVar2 == 0x30) && ((pbVar8[1] + 0xa8 & 0xdf) == 0)) {
    pbVar8 = pbVar8 + 2;
  }
  else if (param_3 == 0) {
    cVar3 = FUN_1400278e0(bVar2);
    if (cVar3 == '\0') {
      return 0xffffffff;
    }
    do {
      cVar3 = (char)uVar6;
      pbVar8 = pbVar8 + 1;
      uVar6 = (ulonglong)*pbVar8;
      uVar9 = (longlong)(cVar3 + -0x30) + uVar9 * 10;
      cVar3 = FUN_1400278e0(uVar6);
      iVar7 = (int)uVar5;
    } while (cVar3 != '\0');
    goto LAB_1400279c4;
  }
  uVar4 = FUN_140027880(*pbVar8);
  if ((int)uVar4 < 0) {
    return 0xffffffff;
  }
  do {
    pbVar1 = pbVar8 + 1;
    pbVar8 = pbVar8 + 1;
    uVar9 = uVar9 * 0x10 + (ulonglong)uVar4;
    uVar4 = FUN_140027880(*pbVar1);
    iVar7 = (int)uVar5;
  } while (-1 < (int)uVar4);
LAB_1400279c4:
  *param_1 = pbVar8;
  uVar5 = -uVar9;
  if (iVar7 == 0) {
    uVar5 = uVar9;
  }
  *param_2 = uVar5;
  return 0;
}


/* ================= FUN_1400279f0 @ 1400279f0  size=13  callers=2 callees=0 ================= */
/* CALLERS: FUN_1400276d0, FUN_1400277f0 */

int FUN_1400279f0(int param_1)

{
  int iVar1;
  
  iVar1 = param_1 + 0x20;
  if (0x19 < param_1 - 0x41U) {
    iVar1 = param_1;
  }
  return iVar1;
}


/* ================= FUN_140027a00 @ 140027a00  size=2156  callers=1 callees=4 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: FUN_140027140, FUN_140027700, FUN_14002c8f0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140027a00(uint param_1,undefined8 param_2,undefined4 param_3)

{
  undefined8 *puVar1;
  undefined *puVar2;
  char *pcVar3;
  uint uVar4;
  uint uVar5;
  undefined8 uVar6;
  uint uVar7;
  undefined1 auStack_1c8 [32];
  undefined8 *local_1a8;
  undefined8 *local_1a0;
  undefined8 local_198;
  undefined8 local_190;
  undefined8 local_188;
  undefined8 uStack_180;
  undefined8 local_178;
  undefined8 uStack_170;
  undefined8 local_168;
  undefined8 uStack_160;
  undefined8 local_158;
  undefined8 uStack_150;
  undefined1 local_148 [256];
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_1c8;
  local_198 = 0;
  local_190 = 0;
  uVar4 = param_1 >> 4 & 0xf;
  uVar5 = param_1 >> 0x10 & 0xff;
  uVar7 = param_1 >> 8 & 0xff;
  local_178 = 0;
  uStack_170 = 0;
  local_168 = 0;
  uStack_160 = 0;
  local_188 = 0;
  uStack_180 = 0;
  local_158 = 0;
  uStack_150 = 0;
  switch(param_1 >> 0x18) {
  case 0:
    pcVar3 = "None";
    break;
  case 1:
    pcVar3 = "Cortex-M1";
    goto LAB_140027ab2;
  default:
    pcVar3 = "Unspecified";
    break;
  case 3:
    pcVar3 = "Cortex-M3";
    goto LAB_140027ab2;
  case 4:
    pcVar3 = "Simulator";
    break;
  case 6:
    if ((uVar5 == 0) || (pcVar3 = "Cortex-M23", uVar5 != 1)) {
      pcVar3 = "Cortex-M0";
    }
    goto LAB_140027ab2;
  case 7:
    FUN_140027140(&local_178,&DAT_140063f14,0x20);
    if (uVar5 == 0) {
switchD_140027dc6_caseD_0:
      puVar2 = &DAT_140063f1c;
      goto LAB_140027b5b;
    }
    goto switchD_140027dc6_caseD_1;
  case 8:
    if (param_1 == 0x80800ff) {
      pcVar3 = "Cortex-A7";
      uVar6 = 0x10;
      puVar1 = &local_188;
    }
    else if (param_1 == 0x80900ff) {
      pcVar3 = "Cortex-A9";
      uVar6 = 0x10;
      puVar1 = &local_188;
    }
    else if (param_1 == 0x80a00ff) {
      pcVar3 = "Cortex-A12";
      uVar6 = 0x10;
      puVar1 = &local_188;
    }
    else if (param_1 == 0x80b00ff) {
      pcVar3 = "Cortex-A15";
      uVar6 = 0x10;
      puVar1 = &local_188;
    }
    else {
      pcVar3 = "Cortex-A8";
      uVar6 = 0x10;
      if (param_1 == 0x80c00ff) {
        pcVar3 = "Cortex-A17";
      }
      puVar1 = &local_188;
    }
    goto LAB_1400281b4;
  case 9:
    FUN_140027140(&local_178,&DAT_140064024,0x20);
    switch(uVar5) {
    case 0:
      goto switchD_140027dc6_caseD_0;
    default:
      goto switchD_140027dc6_caseD_1;
    case 0x20:
      puVar2 = &DAT_14006402c;
      break;
    case 0x22:
      puVar2 = &DAT_140064030;
      break;
    case 0x26:
      puVar2 = &DAT_140064034;
      break;
    case 0x46:
      puVar2 = &DAT_14006403c;
      break;
    case 0x66:
      puVar2 = &DAT_140064040;
      break;
    case 0x68:
      puVar2 = &DAT_140064044;
    }
LAB_140027b5b:
    FUN_140027140(&local_188,puVar2,0x10);
switchD_140027dc6_caseD_1:
    if (uVar7 == 1) {
      FUN_140027140(&local_158,&DAT_140063f24,0x10);
    }
    if (uVar4 != 0xf) {
      FUN_14002c8f0(&local_198,8,"Rev.%d",uVar4);
    }
    goto LAB_1400281b9;
  case 10:
    pcVar3 = "ARM10";
    break;
  case 0xb:
    if (param_1 < 0xb760300) {
      if (param_1 == 0xb7602ff) {
        pcVar3 = "ARM1176J";
      }
      else if (param_1 < 0xb360800) {
        if (param_1 == 0xb3607ff) {
          pcVar3 = "ARM11JF-S";
        }
        else if (param_1 == 0xb3602ff) {
          pcVar3 = "ARM1136J";
        }
        else if (param_1 == 0xb3603ff) {
          pcVar3 = "ARM1136J-S";
        }
        else {
          if (param_1 != 0xb3606ff) goto LAB_140027ed4;
          pcVar3 = "ARM1136JF";
        }
      }
      else if (param_1 == 0xb36ffff) {
        pcVar3 = "ARM1136";
      }
      else if (param_1 == 0xb56ffff) {
        pcVar3 = "ARM1156";
      }
      else {
LAB_140027ed4:
        pcVar3 = "ARM11";
      }
    }
    else if (param_1 == 0xb7603ff) {
      pcVar3 = "ARM1176J-S";
    }
    else if (param_1 == 0xb7606ff) {
      pcVar3 = "ARM1176JF";
    }
    else if (param_1 == 0xb7607ff) {
      pcVar3 = "ARM1176JF-S";
    }
    else {
      if (param_1 != 0xb76ffff) goto LAB_140027ed4;
      pcVar3 = "ARM1176";
    }
    break;
  case 0xc:
    if (uVar5 == 0) {
LAB_140027f47:
      pcVar3 = "Cortex-R4";
    }
    else if (uVar5 == 1) {
      pcVar3 = "Cortex-R5";
    }
    else if (uVar5 == 2) {
      pcVar3 = "Cortex-R8";
    }
    else {
      if (uVar5 != 3) goto LAB_140027f47;
      pcVar3 = "Cortex-R7";
    }
    break;
  case 0xd:
    pcVar3 = "RX";
    break;
  case 0xe:
    if (param_1 < 0xe040100) {
      if (param_1 == 0xe0400ff) {
        pcVar3 = "STAR";
      }
      else if (param_1 == 0xe0000ff) {
        pcVar3 = "Cortex-M4";
      }
      else if (param_1 == 0xe0100ff) {
        pcVar3 = "Cortex-M7";
      }
      else if (param_1 == 0xe0200ff) {
        pcVar3 = "Cortex-M33";
      }
      else {
        if (param_1 != 0xe0300ff) goto LAB_140027fdd;
        pcVar3 = "Cortex-M55";
      }
    }
    else if (param_1 == 0xe0500ff) {
      pcVar3 = "Cortex-M85";
    }
    else if (param_1 == 0xe0600ff) {
      pcVar3 = "Cortex-M52";
    }
    else if (param_1 == 0xe0700ff) {
      pcVar3 = "STAR-MC3";
    }
    else {
LAB_140027fdd:
      pcVar3 = "Unknown Cortex-M4 like";
    }
    goto LAB_140027ab2;
  case 0xf:
    pcVar3 = "Cortex-A5";
LAB_140027ab2:
    FUN_140027140(&local_178,pcVar3,0x20);
    if (uVar4 != 0xf) {
      FUN_14002c8f0(&local_198,8,&DAT_140063ecc,uVar4);
    }
    if ((param_1 & 0xf) != 0xf) {
      FUN_14002c8f0(&local_190,8,&DAT_140063ed0,param_1 & 0xf);
    }
    goto LAB_1400281b9;
  case 0x10:
    if ((uVar7 == 0xff) || (pcVar3 = "PowerPC (Nexus 2+)", (param_1 >> 8 & 1) == 0)) {
      pcVar3 = "PowerPC (Nexus 1)";
    }
    break;
  case 0x11:
    if (uVar5 == 0) {
      pcVar3 = "MIPS M4K";
    }
    else if (uVar5 == 1) {
      pcVar3 = "MIPS MicroAptiv";
    }
    else if (uVar5 == 2) {
      pcVar3 = "MIPS M14K";
    }
    else {
      pcVar3 = "MIPS";
    }
    break;
  case 0x12:
    pcVar3 = "8051";
    break;
  case 0x13:
    pcVar3 = "RISC-V";
    break;
  case 0x14:
    if (param_1 < 0x14050000) {
      if (param_1 == 0x1404ffff) {
        pcVar3 = "Cortex-R52";
        uVar6 = 0x10;
        puVar1 = &local_188;
      }
      else if (param_1 == 0x1400ffff) {
LAB_140027d03:
        pcVar3 = "Cortex-A53";
        uVar6 = 0x10;
        puVar1 = &local_188;
      }
      else if (param_1 == 0x1401ffff) {
        pcVar3 = "Cortex-A57";
        uVar6 = 0x10;
        puVar1 = &local_188;
      }
      else if (param_1 == 0x1402ffff) {
        pcVar3 = "Cortex-A72";
        uVar6 = 0x10;
        puVar1 = &local_188;
      }
      else {
        if (param_1 != 0x1403ffff) goto LAB_140027d03;
        pcVar3 = "Cortex-A35";
        uVar6 = 0x10;
        puVar1 = &local_188;
      }
    }
    else if (param_1 == 0x1405ffff) {
      pcVar3 = "Cortex-A55";
      uVar6 = 0x10;
      puVar1 = &local_188;
    }
    else if (param_1 == 0x1406ffff) {
      pcVar3 = "Cortex-A32";
      uVar6 = 0x10;
      puVar1 = &local_188;
    }
    else if (param_1 == 0x1407ffff) {
      pcVar3 = "Cortex-A76";
      uVar6 = 0x10;
      puVar1 = &local_188;
    }
    else {
      if (param_1 != 0x1408ffff) goto LAB_140027d03;
      pcVar3 = "Cortex-A520";
      uVar6 = 0x10;
      puVar1 = &local_188;
    }
    goto LAB_1400281b4;
  case 0x15:
    uVar6 = 0x10;
    pcVar3 = "BT5511";
    puVar1 = &local_188;
    goto LAB_1400281b4;
  case 0x16:
    FUN_140027140(&local_178,&DAT_14005f306,0x20);
    switch(param_1) {
    case 0x16040000:
      pcVar3 = "Xtensa LX";
      uVar6 = 0x10;
      puVar1 = &local_188;
      break;
    case 0x16040001:
      pcVar3 = "HiFi 1";
      uVar6 = 0x10;
      puVar1 = &local_188;
      break;
    case 0x16040002:
      pcVar3 = "HiFi 3";
      uVar6 = 0x10;
      puVar1 = &local_188;
      break;
    case 0x16040003:
      pcVar3 = "HiFi 3z";
      uVar6 = 0x10;
      puVar1 = &local_188;
      break;
    case 0x16040004:
      pcVar3 = "HiFi 4";
      uVar6 = 0x10;
      puVar1 = &local_188;
      break;
    case 0x16040005:
      pcVar3 = "HiFi 5";
      uVar6 = 0x10;
      puVar1 = &local_188;
      break;
    case 0x16040006:
      pcVar3 = "HiFi 1s";
      uVar6 = 0x10;
      puVar1 = &local_188;
      break;
    case 0x16040007:
      pcVar3 = "HiFi 5s";
      uVar6 = 0x10;
      puVar1 = &local_188;
      break;
    case 0x16040008:
      pcVar3 = "Fusion F1";
      uVar6 = 0x10;
      puVar1 = &local_188;
      break;
    default:
      pcVar3 = "Unknown type";
      uVar6 = 0x10;
      puVar1 = &local_188;
    }
    goto LAB_1400281b4;
  }
  uVar6 = 0x20;
  puVar1 = &local_178;
LAB_1400281b4:
  FUN_140027140(puVar1,pcVar3,uVar6);
LAB_1400281b9:
  local_1a0 = &local_158;
  local_1a8 = &local_188;
  FUN_14002c8f0(local_148,0x100,"%s%s%s",&local_178);
  if (((char)local_198 != '\0') || ((char)local_190 != '\0')) {
    FUN_140027700(local_148,&DAT_14005f570,0x100);
    FUN_140027700(local_148,&local_198,0x100);
    FUN_140027700(local_148,&local_190,0x100);
  }
  FUN_140027140(param_2,local_148,param_3);
  return;
}


/* ================= FUN_140028380 @ 140028380  size=39  callers=38 callees=1 ================= */
/* CALLERS: FUN_14002a1f0, FUN_14002be90, FUN_140039850, FUN_1400398d0, FUN_140039960, FUN_14003cc50, FUN_14003d9b0, FUN_14003de20, FUN_14003eb20, FUN_14003f280, FUN_14003f580, FUN_14003fc90, FUN_140040320, FUN_140041400, FUN_140041470, FUN_140041780, FUN_1400419f0, FUN_140041cd0, FUN_140041f30, FUN_1400423e0, FUN_140046e80, FUN_140047020, FUN_1400476d0, FUN_14004e4b0, FUN_14004e610, FUN_14004eda0, FUN_14004f0c0, FUN_14004f1e0, FUN_14004f2a0, FUN_14004f590, FUN_14004f6c0, FUN_14004fd40, FUN_140050030, FUN_1400504d0, FUN_140050d00, FUN_140053680, FUN_140054790, FUN_140057220 */
/* CALLEES: FUN_1400283b0 */

void FUN_140028380(void)

{
  FUN_1400283b0();
  return;
}


/* ================= FUN_1400283b0 @ 1400283b0  size=740  callers=3 callees=5 ================= */
/* CALLERS: FUN_140028380, FUN_14003f120, FUN_14003f1e0 */
/* CALLEES: FUN_1400286e0, FUN_140028720, FUN_140028760, FUN_140028860, FUN_140028970 */

void FUN_1400283b0(longlong param_1,undefined4 param_2,byte *param_3,longlong *param_4)

{
  char cVar1;
  ulonglong *puVar2;
  byte bVar3;
  undefined4 uVar4;
  ulonglong uVar5;
  byte bVar6;
  int iVar7;
  byte *pbVar8;
  int iVar9;
  int iVar10;
  char *pcVar11;
  int in_stack_ffffffffffffffc8;
  uint3 uVar12;
  longlong local_28;
  undefined4 local_20;
  int local_1c;
  
  local_1c = 0;
  bVar6 = *param_3;
  local_28 = param_1;
  local_20 = param_2;
  do {
    if (bVar6 == 0) {
      *(undefined1 *)(local_1c + param_1) = 0;
      return;
    }
    pbVar8 = param_3 + 1;
    if (bVar6 != 0x25) {
LAB_140028403:
      FUN_140028970(&local_28,bVar6);
      goto switchD_1400284c5_caseD_59;
    }
    bVar3 = *pbVar8;
    pbVar8 = param_3 + 2;
    if (bVar3 == 0x25) goto LAB_140028403;
    iVar10 = 0;
    while (('/' < (char)bVar3 && (bVar3 < 0x3a))) {
      iVar10 = (uint)bVar3 + (iVar10 * 5 + -0x18) * 2;
      bVar3 = *pbVar8;
      pbVar8 = pbVar8 + 1;
    }
    iVar9 = 0;
    if (bVar3 == 0x2e) {
      bVar3 = *pbVar8;
      while ((pbVar8 = pbVar8 + 1, '/' < (char)bVar3 && (bVar3 < 0x3a))) {
        iVar9 = (uint)bVar3 + (iVar9 * 5 + -0x18) * 2;
        bVar3 = *pbVar8;
      }
    }
    iVar7 = 0;
    while (bVar3 == 0x6c) {
      iVar7 = iVar7 + 1;
      bVar3 = *pbVar8;
      pbVar8 = pbVar8 + 1;
    }
    uVar12 = (uint3)((uint)in_stack_ffffffffffffffc8 >> 8);
    switch(bVar3) {
    case 0x58:
      *param_4 = *param_4 + 8;
      if (iVar7 < 2) {
        uVar5 = (ulonglong)*(uint *)(*param_4 + -8);
      }
      else {
        uVar5 = *(ulonglong *)(*param_4 + -8);
        if (0xffffffff < uVar5) {
          in_stack_ffffffffffffffc8 = CONCAT31(uVar12,1);
          goto LAB_1400285b9;
        }
      }
      uVar4 = (undefined4)uVar5;
      in_stack_ffffffffffffffc8 = CONCAT31(uVar12,1);
      goto LAB_1400285d8;
    case 99:
      *param_4 = *param_4 + 8;
      bVar6 = *(byte *)(*param_4 + -8);
      goto LAB_140028403;
    case 100:
    case 0x69:
      puVar2 = (ulonglong *)*param_4;
      *param_4 = (longlong)(puVar2 + 1);
      if (iVar7 < 2) {
        uVar5 = (ulonglong)(uint)*puVar2;
      }
      else {
        uVar5 = *puVar2;
        if (0x7fffffff < (longlong)((uVar5 ^ (longlong)uVar5 >> 0x3f) - ((longlong)uVar5 >> 0x3f)))
        {
          FUN_140028720(&local_28,uVar5,10,iVar10,iVar9);
          in_stack_ffffffffffffffc8 = iVar9;
          break;
        }
      }
      FUN_1400286e0(&local_28,uVar5 & 0xffffffff,10,iVar10,iVar9);
      in_stack_ffffffffffffffc8 = iVar9;
      break;
    case 0x70:
      *param_4 = *param_4 + 8;
      in_stack_ffffffffffffffc8 = (uint)uVar12 << 8;
      uVar5 = *(ulonglong *)(*param_4 + -8);
      if (uVar5 < 0x100000000) {
        FUN_140028760(&local_28,uVar5 & 0xffffffff,0x10,8,in_stack_ffffffffffffffc8,iVar9);
      }
      else {
LAB_1400285b9:
        FUN_140028860(&local_28,uVar5,0x10,iVar10,in_stack_ffffffffffffffc8,iVar9);
      }
      break;
    case 0x73:
      *param_4 = *param_4 + 8;
      pcVar11 = *(char **)(*param_4 + -8);
      cVar1 = *pcVar11;
      while (cVar1 != '\0') {
        pcVar11 = pcVar11 + 1;
        FUN_140028970(&local_28);
        cVar1 = *pcVar11;
      }
      break;
    case 0x75:
      *param_4 = *param_4 + 8;
      if (iVar7 < 2) {
        uVar5 = (ulonglong)*(uint *)(*param_4 + -8);
      }
      else {
        uVar5 = *(ulonglong *)(*param_4 + -8);
        if (0xffffffff < uVar5) {
          in_stack_ffffffffffffffc8 = (uint)uVar12 << 8;
          FUN_140028860(&local_28,uVar5,10,iVar10,in_stack_ffffffffffffffc8,iVar9);
          break;
        }
      }
      in_stack_ffffffffffffffc8 = (uint)uVar12 << 8;
      FUN_140028760(&local_28,uVar5 & 0xffffffff,10,iVar10,in_stack_ffffffffffffffc8,iVar9);
      break;
    case 0x78:
      *param_4 = *param_4 + 8;
      if (iVar7 < 2) {
        uVar5 = (ulonglong)*(uint *)(*param_4 + -8);
      }
      else {
        uVar5 = *(ulonglong *)(*param_4 + -8);
        if (0xffffffff < uVar5) {
          in_stack_ffffffffffffffc8 = (uint)uVar12 << 8;
          goto LAB_1400285b9;
        }
      }
      uVar4 = (undefined4)uVar5;
      in_stack_ffffffffffffffc8 = (uint)uVar12 << 8;
LAB_1400285d8:
      FUN_140028760(&local_28,uVar4,0x10,iVar10,in_stack_ffffffffffffffc8,iVar9);
    }
switchD_1400284c5_caseD_59:
    bVar6 = *pbVar8;
    param_3 = pbVar8;
  } while( true );
}


/* ================= FUN_1400286e0 @ 1400286e0  size=62  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400283b0 */
/* CALLEES: FUN_140028760, FUN_140028970 */

void FUN_1400286e0(undefined8 param_1,ulonglong param_2,undefined4 param_3,ulonglong param_4,
                  undefined4 param_5)

{
  ulonglong uVar1;
  
  param_4 = param_4 & 0xffffffff;
  uVar1 = param_2 & 0xffffffff;
  if ((int)param_2 < 0) {
    uVar1 = (ulonglong)(uint)-(int)param_2;
    FUN_140028970(param_1,0x2d);
  }
  FUN_140028760(param_1,uVar1 & 0xffffffff,param_3,param_4 & 0xffffffff,0,param_5);
  return;
}


/* ================= FUN_140028720 @ 140028720  size=63  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400283b0 */
/* CALLEES: FUN_140028860, FUN_140028970 */

void FUN_140028720(undefined8 param_1,longlong param_2,undefined4 param_3,ulonglong param_4,
                  undefined4 param_5)

{
  param_4 = param_4 & 0xffffffff;
  if (param_2 < 0) {
    param_2 = -param_2;
    FUN_140028970(param_1,0x2d);
  }
  FUN_140028860(param_1,param_2,param_3,param_4 & 0xffffffff,0,param_5);
  return;
}


/* ================= FUN_140028760 @ 140028760  size=247  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400283b0, FUN_1400286e0 */
/* CALLEES: FUN_140028970 */

void FUN_140028760(undefined8 param_1,uint param_2,uint param_3,int param_4,undefined8 param_5,
                  int param_6)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  ulonglong uVar4;
  ulonglong uVar5;
  
  iVar3 = 1;
  uVar4 = 1;
  uVar2 = param_2;
  while ((param_3 <= uVar2 && (uVar1 = (uint)uVar4 * param_3, (uint)uVar4 <= uVar1))) {
    iVar3 = iVar3 + 1;
    uVar2 = param_2 / uVar1;
    uVar4 = (ulonglong)uVar1;
  }
  uVar5 = 0;
  if (0 < param_6 - iVar3) {
    uVar5 = (ulonglong)(uint)(param_6 - iVar3);
  }
  for (iVar3 = (param_4 - (int)uVar5) - iVar3; 0 < iVar3; iVar3 = iVar3 + -1) {
    FUN_140028970(param_1);
  }
  uVar2 = (uint)uVar5;
  while (0 < (int)uVar2) {
    iVar3 = (int)uVar5;
    FUN_140028970(param_1);
    uVar2 = iVar3 - 1;
    uVar5 = (ulonglong)uVar2;
  }
  do {
    FUN_140028970(param_1);
    uVar4 = (uVar4 & 0xffffffff) / (ulonglong)param_3;
  } while ((int)uVar4 != 0);
  return;
}


/* ================= FUN_140028860 @ 140028860  size=266  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400283b0, FUN_140028720 */
/* CALLEES: FUN_140028970 */

void FUN_140028860(undefined8 param_1,ulonglong param_2,uint param_3,int param_4,undefined8 param_5,
                  int param_6)

{
  ulonglong uVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  int iVar4;
  ulonglong uVar5;
  uint uVar6;
  
  uVar5 = (ulonglong)param_3;
  iVar4 = 1;
  uVar3 = 1;
  uVar2 = param_2;
  while ((uVar5 <= uVar2 && (uVar1 = uVar5 * uVar3, uVar3 <= uVar1))) {
    iVar4 = iVar4 + 1;
    uVar2 = param_2 / uVar1;
    uVar3 = uVar1;
  }
  uVar2 = 0;
  if (0 < param_6 - iVar4) {
    uVar2 = (ulonglong)(uint)(param_6 - iVar4);
  }
  for (iVar4 = (param_4 - (int)uVar2) - iVar4; 0 < iVar4; iVar4 = iVar4 + -1) {
    FUN_140028970(param_1);
  }
  uVar6 = (uint)uVar2;
  while (0 < (int)uVar6) {
    iVar4 = (int)uVar2;
    FUN_140028970(param_1);
    uVar6 = iVar4 - 1;
    uVar2 = (ulonglong)uVar6;
  }
  do {
    FUN_140028970(param_1);
    uVar3 = uVar3 / uVar5;
  } while (uVar3 != 0);
  return;
}


/* ================= FUN_140028970 @ 140028970  size=29  callers=5 callees=0 ================= */
/* CALLERS: FUN_1400283b0, FUN_1400286e0, FUN_140028720, FUN_140028760, FUN_140028860 */

void FUN_140028970(longlong *param_1,undefined1 param_2)

{
  int iVar1;
  
  iVar1 = *(int *)((longlong)param_1 + 0xc) + 1;
  if (iVar1 < (int)param_1[1]) {
    *(undefined1 *)((longlong)*(int *)((longlong)param_1 + 0xc) + *param_1) = param_2;
    *(int *)((longlong)param_1 + 0xc) = iVar1;
  }
  return;
}


/* ================= JLINKARM_Close @ 140028990  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003ef20 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_Close(void)

{
  if (DAT_14085b3f8 == (code *)0x0) {
    DAT_14085b3f8 = (code *)FUN_1400293b0("JLINKARM_Close");
    if (DAT_14085b3f8 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x0001400289bc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b3f8)();
  return;
}


/* ================= JLINKARM_ConfigJTAG @ 1400289d0  size=67  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400419f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_ConfigJTAG(undefined4 param_1,undefined4 param_2)

{
  if ((DAT_14085b468 == (code *)0x0) &&
     (DAT_14085b468 = (code *)FUN_1400293b0("JLINKARM_ConfigJTAG"), DAT_14085b468 == (code *)0x0)) {
    return;
  }
  (*DAT_14085b468)(param_1,param_2);
  return;
}


/* ================= JLINKARM_Connect @ 140028a20  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400419f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_Connect(void)

{
  if (DAT_14085b470 == (code *)0x0) {
    DAT_14085b470 = (code *)FUN_1400293b0("JLINKARM_Connect");
    if (DAT_14085b470 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028a51. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b470)();
  return;
}


/* ================= JLINKARM_DEVICE_GetInfo @ 140028a60  size=80  callers=2 callees=1 ================= */
/* CALLERS: FUN_14002bc30, FUN_1400419f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_DEVICE_GetInfo(undefined4 param_1,undefined8 param_2)

{
  if (DAT_14085ba40 == (code *)0x0) {
    DAT_14085ba40 = (code *)FUN_1400293b0("JLINKARM_DEVICE_GetInfo");
    if (DAT_14085ba40 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028aad. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085ba40)(param_1,param_2);
  return;
}


/* ================= JLINKARM_DEVICE_SelectDialog @ 140028ab0  size=101  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400419f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_DEVICE_SelectDialog(undefined8 param_1,undefined4 param_2,undefined8 param_3)

{
  if (DAT_14085ba48 == (code *)0x0) {
    DAT_14085ba48 = (code *)FUN_1400293b0("JLINKARM_DEVICE_SelectDialog");
    if (DAT_14085ba48 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028b12. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085ba48)(param_1,param_2,param_3);
  return;
}


/* ================= JLINKARM_EMU_HasCapEx @ 140028b20  size=60  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400419f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_EMU_HasCapEx(undefined4 param_1)

{
  if (DAT_14085bab8 == (code *)0x0) {
    DAT_14085bab8 = (code *)FUN_1400293b0("JLINKARM_EMU_HasCapEx");
    if (DAT_14085bab8 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028b59. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085bab8)(param_1);
  return;
}


/* ================= JLINKARM_EMU_SelectByUSBSN @ 140028b60  size=60  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400418f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_EMU_SelectByUSBSN(undefined4 param_1)

{
  if (DAT_14085bad8 == (code *)0x0) {
    DAT_14085bad8 = (code *)FUN_1400293b0("JLINKARM_EMU_SelectByUSBSN");
    if (DAT_14085bad8 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028b99. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085bad8)(param_1);
  return;
}


/* ================= JLINKARM_ExecCommand @ 140028ba0  size=103  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400419f0, FUN_140041cd0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_ExecCommand(undefined8 param_1,undefined8 param_2,undefined4 param_3)

{
  if (DAT_14085b4d0 == (code *)0x0) {
    DAT_14085b4d0 = (code *)FUN_1400293b0("JLINKARM_ExecCommand");
    if (DAT_14085b4d0 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028c04. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b4d0)(param_1,param_2,param_3);
  return;
}


/* ================= JLINKARM_GetCompileDateTime @ 140028c10  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_140041780 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_GetCompileDateTime(void)

{
  if (DAT_14085b500 == (code *)0x0) {
    DAT_14085b500 = (code *)FUN_1400293b0("JLINKARM_GetCompileDateTime");
    if (DAT_14085b500 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028c41. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b500)();
  return;
}


/* ================= JLINKARM_GetDLLVersion @ 140028c50  size=52  callers=2 callees=1 ================= */
/* CALLERS: FUN_140041780, FUN_1400419f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_GetDLLVersion(void)

{
  if (DAT_14085b520 == (code *)0x0) {
    DAT_14085b520 = (code *)FUN_1400293b0("JLINKARM_GetDLLVersion");
    if (DAT_14085b520 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028c81. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b520)();
  return;
}


/* ================= JLINKARM_Go @ 140028c90  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003f890 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_Go(void)

{
  if (DAT_14085b630 == (code *)0x0) {
    DAT_14085b630 = (code *)FUN_1400293b0("JLINKARM_Go");
    if (DAT_14085b630 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028cbc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b630)();
  return;
}


/* ================= JLINKARM_Halt @ 140028cd0  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003f8d0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_Halt(void)

{
  if (DAT_14085b658 == (code *)0x0) {
    DAT_14085b658 = (code *)FUN_1400293b0("JLINKARM_Halt");
    if (DAT_14085b658 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028d01. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b658)();
  return;
}


/* ================= JLINKARM_IsHalted @ 140028d10  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003d5c0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_IsHalted(void)

{
  if (DAT_14085b670 == (code *)0x0) {
    DAT_14085b670 = (code *)FUN_1400293b0("JLINKARM_IsHalted");
    if (DAT_14085b670 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028d41. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b670)();
  return;
}


/* ================= JLINKARM_OpenEx @ 140028d50  size=82  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400418f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_OpenEx(undefined8 param_1,undefined8 param_2)

{
  if (DAT_14085b6b0 == (code *)0x0) {
    DAT_14085b6b0 = (code *)FUN_1400293b0("JLINKARM_OpenEx");
    if (DAT_14085b6b0 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028d9f. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b6b0)(param_1,param_2);
  return;
}


/* ================= JLINKARM_Reset @ 140028db0  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003f390 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_Reset(void)

{
  if (DAT_14085b778 == (code *)0x0) {
    DAT_14085b778 = (code *)FUN_1400293b0("JLINKARM_Reset");
    if (DAT_14085b778 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028de1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b778)();
  return;
}


/* ================= JLINKARM_SelectIP @ 140028df0  size=80  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400418f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_SelectIP(undefined8 param_1,undefined4 param_2)

{
  if (DAT_14085b7a8 == (code *)0x0) {
    DAT_14085b7a8 = (code *)FUN_1400293b0("JLINKARM_SelectIP");
    if (DAT_14085b7a8 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028e3d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b7a8)(param_1,param_2);
  return;
}


/* ================= JLINKARM_SetSpeed @ 140028e40  size=60  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400419f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_SetSpeed(undefined4 param_1)

{
  if (DAT_14085b840 == (code *)0x0) {
    DAT_14085b840 = (code *)FUN_1400293b0("JLINKARM_SetSpeed");
    if (DAT_14085b840 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028e73. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085b840)(param_1);
  return;
}


/* ================= JLINKARM_TIF_Select @ 140028e80  size=60  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400419f0 */
/* CALLEES: FUN_1400293b0 */

void JLINKARM_TIF_Select(undefined4 param_1)

{
  if (DAT_14085bdc8 == (code *)0x0) {
    DAT_14085bdc8 = (code *)FUN_1400293b0("JLINKARM_TIF_Select");
    if (DAT_14085bdc8 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028eb9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085bdc8)(param_1);
  return;
}


/* ================= FUN_140028ec0 @ 140028ec0  size=19  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400418f0 */
/* CALLEES: FUN_140029490 */

int FUN_140028ec0(void)

{
  char cVar1;
  
  cVar1 = FUN_140029490(0);
  return (int)cVar1;
}


/* ================= JLINK_EMU_SelectByUSBNickname @ 140028ef0  size=62  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400418f0 */
/* CALLEES: FUN_1400293b0 */

void JLINK_EMU_SelectByUSBNickname(undefined8 param_1)

{
  if (DAT_14085c2e0 == (code *)0x0) {
    DAT_14085c2e0 = (code *)FUN_1400293b0("JLINK_EMU_SelectByUSBNickname");
    if (DAT_14085c2e0 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028f2b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085c2e0)(param_1);
  return;
}


/* ================= JLINK_ExecCommand @ 140028f30  size=103  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400419f0 */
/* CALLEES: FUN_1400293b0 */

void JLINK_ExecCommand(undefined8 param_1,undefined8 param_2,undefined4 param_3)

{
  if (DAT_14085c138 == (code *)0x0) {
    DAT_14085c138 = (code *)FUN_1400293b0("JLINK_ExecCommand");
    if (DAT_14085c138 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028f94. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085c138)(param_1,param_2,param_3);
  return;
}


/* ================= JLINK_Go @ 140028fa0  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_140041f30 */
/* CALLEES: FUN_1400293b0 */

void JLINK_Go(void)

{
  if (DAT_14085bf58 == (code *)0x0) {
    DAT_14085bf58 = (code *)FUN_1400293b0("JLINK_Go");
    if (DAT_14085bf58 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140028fcc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085bf58)();
  return;
}


/* ================= JLINK_HSS_GetCaps @ 140028fe0  size=62  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400419f0 */
/* CALLEES: FUN_1400293b0 */

void JLINK_HSS_GetCaps(undefined8 param_1)

{
  if (DAT_14085beb8 == (code *)0x0) {
    DAT_14085beb8 = (code *)FUN_1400293b0("JLINK_HSS_GetCaps");
    if (DAT_14085beb8 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x00014002901b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085beb8)(param_1);
  return;
}


/* ================= JLINK_HSS_Read @ 140029020  size=80  callers=1 callees=1 ================= */
/* CALLERS: FUN_140040fa0 */
/* CALLEES: FUN_1400293b0 */

void JLINK_HSS_Read(undefined8 param_1,undefined4 param_2)

{
  if (DAT_14085bed0 == (code *)0x0) {
    DAT_14085bed0 = (code *)FUN_1400293b0("JLINK_HSS_Read");
    if (DAT_14085bed0 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x00014002906d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085bed0)(param_1,param_2);
  return;
}


/* ================= JLINK_HSS_Start @ 140029070  size=101  callers=1 callees=1 ================= */
/* CALLERS: FUN_140041f30 */
/* CALLEES: FUN_1400293b0 */

void JLINK_HSS_Start(undefined8 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  if ((DAT_14085bec0 == (code *)0x0) &&
     (DAT_14085bec0 = (code *)FUN_1400293b0("JLINK_HSS_Start"), DAT_14085bec0 == (code *)0x0)) {
    return;
  }
  (*DAT_14085bec0)(param_1,param_2,param_3,param_4);
  return;
}


/* ================= JLINK_HSS_Stop @ 1400290e0  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003d7d0 */
/* CALLEES: FUN_1400293b0 */

void JLINK_HSS_Stop(void)

{
  if (DAT_14085bec8 == (code *)0x0) {
    DAT_14085bec8 = (code *)FUN_1400293b0("JLINK_HSS_Stop");
    if (DAT_14085bec8 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140029111. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085bec8)();
  return;
}


/* ================= JLINK_IsHalted @ 140029120  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_140041f30 */
/* CALLEES: FUN_1400293b0 */

void JLINK_IsHalted(void)

{
  if (DAT_14085bf90 == (code *)0x0) {
    DAT_14085bf90 = (code *)FUN_1400293b0("JLINK_IsHalted");
    if (DAT_14085bf90 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140029151. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085bf90)();
  return;
}


/* ================= JLINK_RTTERMINAL_Control @ 140029160  size=80  callers=5 callees=1 ================= */
/* CALLERS: FUN_14003d5c0, FUN_14003d7d0, FUN_140042100, FUN_1400424f0, FUN_1400427b0 */
/* CALLEES: FUN_1400293b0 */

void JLINK_RTTERMINAL_Control(undefined4 param_1,undefined8 param_2)

{
  if (DAT_14085bce8 == (code *)0x0) {
    DAT_14085bce8 = (code *)FUN_1400293b0("JLINK_RTTERMINAL_Control");
    if (DAT_14085bce8 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x0001400291ad. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085bce8)(param_1,param_2);
  return;
}


/* ================= JLINK_RTTERMINAL_Read @ 1400291b0  size=101  callers=2 callees=1 ================= */
/* CALLERS: FUN_140040ed0, FUN_140042260 */
/* CALLEES: FUN_1400293b0 */

void JLINK_RTTERMINAL_Read(undefined4 param_1,undefined8 param_2,undefined4 param_3)

{
  if (DAT_14085bcd8 == (code *)0x0) {
    DAT_14085bcd8 = (code *)FUN_1400293b0("JLINK_RTTERMINAL_Read");
    if (DAT_14085bcd8 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140029212. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085bcd8)(param_1,param_2,param_3);
  return;
}


/* ================= JLINK_RTTERMINAL_Write @ 140029220  size=101  callers=1 callees=1 ================= */
/* CALLERS: FUN_140042630 */
/* CALLEES: FUN_1400293b0 */

void JLINK_RTTERMINAL_Write(undefined4 param_1,undefined8 param_2,undefined4 param_3)

{
  if (DAT_14085bce0 == (code *)0x0) {
    DAT_14085bce0 = (code *)FUN_1400293b0("JLINK_RTTERMINAL_Write");
    if (DAT_14085bce0 == (code *)0x0) {
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140029282. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_14085bce0)(param_1,param_2,param_3);
  return;
}

