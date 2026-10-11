
/* ================= FUN_140009620 @ 140009620  size=115  callers=0 callees=3 ================= */
/* CALLEES: FUN_140001f60, FUN_1400068b0, FUN_1400079a0 */

undefined8 FUN_140009620(longlong param_1,longlong param_2,longlong param_3)

{
  int iVar1;
  undefined8 uVar2;
  
  if (((param_2 != 0) && (param_1 != 0)) && (param_3 == 0x2e)) {
    iVar1 = FUN_1400079a0(param_2,0x3c);
    if (iVar1 == 0) {
      iVar1 = FUN_1400079a0(param_2,0x34);
      if (iVar1 == 0) {
        uVar2 = FUN_1400068b0(param_2);
        FUN_140001f60(param_1 + 0x378,uVar2);
        return 1;
      }
    }
  }
  return 0;
}


/* ================= FUN_1400096a0 @ 1400096a0  size=135  callers=0 callees=4 ================= */
/* CALLEES: FUN_140001f60, FUN_1400068b0, FUN_1400079a0, thunk_FUN_140009120 */

undefined8
FUN_1400096a0(longlong param_1,longlong param_2,longlong param_3,undefined8 param_4,
             undefined8 param_5,longlong param_6)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  
  if ((((param_2 != 0) && (param_1 != 0)) && (param_3 == 0x34)) &&
     ((iVar2 = FUN_1400079a0(param_2,0x3c), iVar2 == 0 &&
      (iVar2 = FUN_1400079a0(param_2,0x34), iVar2 == 0)))) {
    if ((param_6 != 0) && (cVar1 = thunk_FUN_140009120(param_2), cVar1 == '\0')) {
      return 0;
    }
    uVar3 = FUN_1400068b0(param_2);
    FUN_140001f60(param_1 + 0x260,uVar3);
    return 1;
  }
  return 0;
}


/* ================= FUN_140009730 @ 140009730  size=146  callers=0 callees=4 ================= */
/* CALLEES: FUN_140004df0, FUN_1400068b0, FUN_140006900, FUN_1400097d0 */

undefined8
FUN_140009730(longlong param_1,longlong param_2,longlong param_3,undefined1 param_4,longlong param_5
             )

{
  longlong lVar1;
  undefined8 uVar2;
  undefined1 local_18 [16];
  
  if ((((param_5 != 0) && (param_2 != 0)) && (param_1 != 0)) && (param_3 == 0x1c)) {
    lVar1 = FUN_140004df0(param_2,0x34,local_18);
    if (lVar1 == 0) {
      uVar2 = FUN_1400068b0(param_2);
      uVar2 = FUN_140006900(param_5,param_4,uVar2);
      FUN_1400097d0(param_1,uVar2);
      return 1;
    }
  }
  return 0;
}


/* ================= FUN_1400097d0 @ 1400097d0  size=120  callers=1 callees=5 ================= */
/* CALLERS: FUN_140009730 */
/* CALLEES: FUN_1400027c0, FUN_140002840, FUN_140006920, FUN_14000a770, FUN_140019ae0 */

undefined8 FUN_1400097d0(undefined8 param_1,undefined8 param_2,undefined8 param_3)

{
  longlong lVar1;
  longlong lVar2;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0xffffffff;
  }
  lVar2 = *(longlong *)(lVar1 + 0x30);
  if (lVar2 == 0) {
    lVar2 = FUN_140019ae0(0x20);
    *(longlong *)(lVar1 + 0x30) = lVar2;
    if (lVar2 == 0) {
      FUN_14000a770(0x48);
      return 0xffffffff;
    }
  }
  if (*(longlong *)(lVar2 + 8) == 0) {
    FUN_140002840(lVar2,0);
    lVar2 = *(longlong *)(lVar1 + 0x30);
  }
  FUN_1400027c0(lVar2,param_3);
  return 0;
}


/* ================= FUN_140009850 @ 140009850  size=115  callers=0 callees=3 ================= */
/* CALLEES: FUN_140001f60, FUN_1400068b0, FUN_1400079a0 */

undefined8 FUN_140009850(longlong param_1,longlong param_2,longlong param_3)

{
  int iVar1;
  undefined8 uVar2;
  
  if (((param_2 != 0) && (param_1 != 0)) && (param_3 == 0x1d)) {
    iVar1 = FUN_1400079a0(param_2,0x3c);
    if (iVar1 == 0) {
      iVar1 = FUN_1400079a0(param_2,0x34);
      if (iVar1 == 0) {
        uVar2 = FUN_1400068b0(param_2);
        FUN_140001f60(param_1 + 0x3b0,uVar2);
        return 1;
      }
    }
  }
  return 0;
}


/* ================= FUN_1400098d0 @ 1400098d0  size=147  callers=0 callees=4 ================= */
/* CALLEES: FUN_1400068b0, FUN_140006900, FUN_1400079a0, FUN_140009970 */

undefined8
FUN_1400098d0(longlong param_1,longlong param_2,longlong param_3,undefined1 param_4,
             undefined8 param_5,longlong param_6)

{
  int iVar1;
  undefined8 uVar2;
  
  if ((((param_2 != 0) && (param_1 != 0)) && (param_6 != 0)) &&
     ((param_3 == 0x34 || (param_3 == 5)))) {
    iVar1 = FUN_1400079a0(param_2,0x3c);
    if (iVar1 == 0) {
      uVar2 = FUN_1400068b0(param_2);
      uVar2 = FUN_140006900(param_6,param_4,uVar2);
      FUN_140009970(param_1,uVar2);
      return 1;
    }
  }
  return 0;
}


/* ================= FUN_140009970 @ 140009970  size=144  callers=1 callees=5 ================= */
/* CALLERS: FUN_1400098d0 */
/* CALLEES: FUN_1400027c0, FUN_140002840, FUN_140006920, FUN_14000a770, FUN_140019ae0 */

undefined8 FUN_140009970(longlong param_1,undefined8 param_2,undefined8 param_3)

{
  longlong lVar1;
  longlong lVar2;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0xffffffff;
  }
  lVar2 = *(longlong *)(lVar1 + 0x20);
  if (lVar2 == 0) {
    lVar2 = FUN_140019ae0(0x20);
    *(longlong *)(lVar1 + 0x20) = lVar2;
    if (lVar2 == 0) {
      FUN_14000a770(0x48);
      return 0xffffffff;
    }
  }
  if (*(longlong *)(lVar2 + 8) == 0) {
    FUN_140002840(lVar2,0);
    lVar2 = *(longlong *)(lVar1 + 0x20);
  }
  FUN_1400027c0(lVar2,param_3);
  *(int *)(param_1 + 0xc0) = *(int *)(param_1 + 0xc0) + 1;
  return 0;
}


/* ================= FUN_140009a00 @ 140009a00  size=226  callers=0 callees=8 ================= */
/* CALLEES: FUN_1400054c0, FUN_1400068b0, FUN_140006900, FUN_140007070, FUN_1400079a0, FUN_140009af0, FUN_140009d00, FUN_14000a2d0 */

bool FUN_140009a00(longlong param_1,longlong param_2,undefined8 param_3,undefined1 param_4,
                  longlong param_5)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined8 uVar4;
  undefined1 uVar5;
  
  if (((param_5 != 0) && (param_2 != 0)) && (param_1 != 0)) {
    uVar5 = param_4;
    cVar1 = FUN_14000a2d0(param_3);
    if (cVar1 != '\0') {
      iVar2 = FUN_140007070(param_1,param_5,uVar5);
      cVar1 = FUN_140009d00((longlong)iVar2);
      if (cVar1 != '\0') {
        iVar2 = FUN_1400079a0(param_2,0x34);
        if (iVar2 == 0) {
          lVar3 = FUN_1400054c0(param_2);
          if (lVar3 == 0) {
            lVar3 = param_2;
          }
          uVar4 = FUN_1400068b0(lVar3);
          uVar4 = FUN_140006900(param_5,param_4,uVar4);
          FUN_140009af0(param_1,uVar4);
          return lVar3 == param_2;
        }
      }
    }
  }
  return false;
}


/* ================= FUN_140009af0 @ 140009af0  size=144  callers=1 callees=5 ================= */
/* CALLERS: FUN_140009a00 */
/* CALLEES: FUN_1400027c0, FUN_140002840, FUN_140006920, FUN_14000a770, FUN_140019ae0 */

undefined8 FUN_140009af0(longlong param_1,undefined8 param_2,undefined8 param_3)

{
  longlong lVar1;
  longlong lVar2;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0xffffffff;
  }
  lVar2 = *(longlong *)(lVar1 + 0x28);
  if (lVar2 == 0) {
    lVar2 = FUN_140019ae0(0x20);
    *(longlong *)(lVar1 + 0x28) = lVar2;
    if (lVar2 == 0) {
      FUN_14000a770(0x48);
      return 0xffffffff;
    }
  }
  if (*(longlong *)(lVar2 + 8) == 0) {
    FUN_140002840(lVar2,0);
    lVar2 = *(longlong *)(lVar1 + 0x28);
  }
  FUN_1400027c0(lVar2,param_3);
  *(int *)(param_1 + 0xbc) = *(int *)(param_1 + 0xbc) + 1;
  return 0;
}


/* ================= FUN_140009b80 @ 140009b80  size=70  callers=0 callees=2 ================= */
/* CALLEES: FUN_140001f60, FUN_1400068b0 */

undefined8 FUN_140009b80(longlong param_1,longlong param_2,longlong param_3)

{
  undefined8 uVar1;
  
  if ((((param_2 != 0) && (param_1 != 0)) && (*(longlong *)(param_1 + 0x78) != 0)) &&
     (param_3 == 0x39)) {
    uVar1 = FUN_1400068b0(param_2,param_2,0x39,param_1);
    FUN_140001f60(param_1 + 0x298,uVar1);
    return 1;
  }
  return 0;
}


/* ================= FUN_140009bd0 @ 140009bd0  size=107  callers=0 callees=3 ================= */
/* CALLEES: FUN_140001f60, FUN_140004df0, FUN_1400068b0 */

undefined8 FUN_140009bd0(longlong param_1,longlong param_2,longlong param_3)

{
  byte bVar1;
  undefined8 uVar2;
  byte local_res8 [8];
  
  if (((param_2 != 0) && (param_1 != 0)) && (param_3 == 0x39)) {
    bVar1 = FUN_140004df0(param_2,0x54,local_res8);
    if ((local_res8[0] & bVar1) != 0) {
      uVar2 = FUN_1400068b0(param_2);
      FUN_140001f60(param_1 + 0x2d0,uVar2);
      return 1;
    }
  }
  return 0;
}


/* ================= FUN_140009c40 @ 140009c40  size=155  callers=0 callees=5 ================= */
/* CALLEES: FUN_140001f60, FUN_140004df0, FUN_1400068b0, FUN_1400079a0, FUN_140009d00 */

undefined8 FUN_140009c40(longlong param_1,longlong param_2,longlong param_3)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined8 uVar4;
  undefined1 local_res10 [24];
  
  if ((param_2 != 0) && (param_1 != 0)) {
    if ((param_3 == 0x16) || (param_3 == 0x28)) {
LAB_140009ca7:
      uVar4 = FUN_1400068b0(param_2);
      FUN_140001f60(param_1 + 0x308,uVar4);
      return 1;
    }
    cVar1 = FUN_140009d00(param_3);
    if (cVar1 != '\0') {
      iVar2 = FUN_1400079a0(param_2,0x3c);
      if (iVar2 == 0) {
        iVar2 = FUN_1400079a0(param_2,0x34);
        if (iVar2 == 0) {
          lVar3 = FUN_140004df0(param_2,0xb,local_res10);
          if (lVar3 != 0) goto LAB_140009ca7;
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_140009ce0 @ 140009ce0  size=20  callers=10 callees=1 ================= */
/* CALLERS: FUN_140005ab0, FUN_140005be0, FUN_14000c0f0, FUN_14000c1c0, FUN_14000c340, FUN_14000c520, FUN_14000cb60, FUN_14000ccf0, FUN_14000cf50, FUN_14000d0c0 */
/* CALLEES: FUN_140005310 */

bool FUN_140009ce0(void)

{
  int iVar1;
  
  iVar1 = FUN_140005310();
  return iVar1 == 1;
}


/* ================= FUN_140009d00 @ 140009d00  size=23  callers=3 callees=0 ================= */
/* CALLERS: FUN_140009a00, FUN_140009c40, FUN_14000dc30 */

ulonglong FUN_140009d00(ulonglong param_1)

{
  ulonglong in_RAX;
  
  if ((param_1 < 0x18) && (in_RAX = 0x880014, (0x880014UL >> (param_1 & 0x3f) & 1) != 0)) {
    return 0x880001;
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_140009d20 @ 140009d20  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000dc30 */

ulonglong FUN_140009d20(ulonglong param_1)

{
  ulonglong in_RAX;
  
  if ((param_1 < 0x35) &&
     (in_RAX = 0x10008000002020, (0x10008000002020U >> (param_1 & 0x3f) & 1) != 0)) {
    return 0x10008000002001;
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_140009d40 @ 140009d40  size=7  callers=2 callees=0 ================= */
/* CALLERS: FUN_140003d10, FUN_1400074d0 */

bool FUN_140009d40(int param_1)

{
  return param_1 == 1;
}


/* ================= FUN_140009d50 @ 140009d50  size=33  callers=1 callees=2 ================= */
/* CALLERS: FUN_140005380 */
/* CALLEES: FUN_140006fb0, FUN_140009d80 */

undefined1 FUN_140009d50(longlong param_1)

{
  undefined8 uVar1;
  undefined1 uVar2;
  
  if (param_1 == 0) {
    return 0;
  }
  uVar1 = FUN_140006fb0();
  uVar2 = FUN_140009d80(uVar1);
  return uVar2;
}


/* ================= FUN_140009d80 @ 140009d80  size=28  callers=2 callees=0 ================= */
/* CALLERS: FUN_140009d50, FUN_14000dc30 */

ulonglong FUN_140009d80(ulonglong param_1)

{
  ulonglong in_RAX;
  
  if ((param_1 < 0x3c) &&
     (in_RAX = 0x800401080a98016, (0x800401080a98016U >> (param_1 & 0x3f) & 1) != 0)) {
    return 0x800401080a98001;
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_140009da0 @ 140009da0  size=91  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400054c0 */
/* CALLEES: FUN_140006560, FUN_140006580, FUN_1400068b0, FUN_1400090c0 */

bool FUN_140009da0(longlong param_1)

{
  undefined8 uVar1;
  char cVar2;
  int iVar3;
  longlong lVar4;
  undefined8 local_res8 [4];
  
  if (param_1 != 0) {
    iVar3 = FUN_140006580(param_1,local_res8);
    if (iVar3 != 0) {
      cVar2 = FUN_1400090c0();
      if (cVar2 != '\0') {
        return true;
      }
      uVar1 = FUN_1400068b0(param_1);
      lVar4 = FUN_140006560(local_res8[0],uVar1);
      return lVar4 != 0;
    }
  }
  return false;
}


/* ================= FUN_140009e00 @ 140009e00  size=39  callers=1 callees=2 ================= */
/* CALLERS: FUN_140007320 */
/* CALLEES: FUN_140008c60, FUN_14000a670 */

bool FUN_140009e00(undefined8 param_1)

{
  int iVar1;
  
  iVar1 = FUN_14000a670(param_1,param_1);
  if (iVar1 == 0) {
    return false;
  }
  iVar1 = FUN_140008c60(param_1);
  return iVar1 == 0;
}


/* ================= FUN_140009e30 @ 140009e30  size=40  callers=1 callees=2 ================= */
/* CALLERS: FUN_140007450 */
/* CALLEES: FUN_140008c60, FUN_14000a670 */

bool FUN_140009e30(undefined8 param_1)

{
  int iVar1;
  
  iVar1 = FUN_14000a670(param_1,param_1);
  if (iVar1 == 0) {
    return false;
  }
  iVar1 = FUN_140008c60(param_1);
  return iVar1 == 1;
}


/* ================= FUN_140009e60 @ 140009e60  size=303  callers=1 callees=2 ================= */
/* CALLERS: FUN_140009120 */
/* CALLEES: FUN_14000a3a0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_140009e60(undefined8 *param_1)

{
  char cVar1;
  uint uVar2;
  ulonglong uVar3;
  undefined8 uVar4;
  undefined1 auStack_128 [32];
  undefined8 local_108;
  undefined8 uStack_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined8 local_e8;
  undefined8 uStack_e0;
  undefined4 local_d8;
  undefined4 uStack_d4;
  undefined4 uStack_d0;
  undefined4 uStack_cc;
  undefined4 local_c8;
  undefined4 uStack_c4;
  undefined4 uStack_c0;
  undefined4 uStack_bc;
  undefined8 local_b8;
  undefined8 auStack_b0 [4];
  undefined8 uStack_90;
  undefined8 local_88;
  undefined8 uStack_80;
  undefined8 local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 local_38;
  undefined8 uStack_30;
  undefined8 local_28;
  undefined8 uStack_20;
  undefined8 local_18;
  ulonglong local_10;
  
  local_10 = DAT_140096fe8 ^ (ulonglong)auStack_128;
  uVar4 = *param_1;
  auStack_b0[0] = param_1[1];
  auStack_b0[1] = param_1[2];
  auStack_b0[2] = param_1[3];
  auStack_b0[3] = param_1[4];
  uStack_90 = param_1[5];
  local_88 = param_1[6];
  uStack_80 = param_1[7];
  local_78 = param_1[8];
  uStack_70 = param_1[9];
  local_68 = param_1[10];
  uStack_60 = param_1[0xb];
  local_58 = param_1[0xc];
  uStack_50 = param_1[0xd];
  local_48 = param_1[0xe];
  uStack_40 = param_1[0xf];
  local_38 = param_1[0x10];
  uStack_30 = param_1[0x11];
  local_28 = param_1[0x12];
  uStack_20 = param_1[0x13];
  local_18 = param_1[0x14];
  if ((int)uVar4 == 0) {
    return 0;
  }
  uVar3 = 0;
  local_b8 = uVar4;
  if ((int)uVar4 != 0) {
    do {
      local_108 = auStack_b0[uVar3 * 10];
      uStack_100 = auStack_b0[uVar3 * 10 + 1];
      local_f8 = auStack_b0[uVar3 * 10 + 2];
      uStack_f0 = auStack_b0[uVar3 * 10 + 3];
      local_e8 = auStack_b0[uVar3 * 10 + 4];
      uStack_e0 = auStack_b0[uVar3 * 10 + 5];
      local_d8 = *(undefined4 *)(&uStack_80 + uVar3 * 10);
      uStack_d4 = *(undefined4 *)((longlong)&uStack_80 + uVar3 * 0x50 + 4);
      uStack_d0 = *(undefined4 *)(&local_78 + uVar3 * 10);
      uStack_cc = *(undefined4 *)((longlong)&local_78 + uVar3 * 0x50 + 4);
      local_c8 = *(undefined4 *)(&uStack_70 + uVar3 * 10);
      uStack_c4 = *(undefined4 *)((longlong)&uStack_70 + uVar3 * 0x50 + 4);
      uStack_c0 = *(undefined4 *)(&local_68 + uVar3 * 10);
      uStack_bc = *(undefined4 *)((longlong)&local_68 + uVar3 * 0x50 + 4);
      cVar1 = FUN_14000a3a0(&local_108);
      if (cVar1 == '\0') {
        return 0;
      }
      uVar2 = (int)uVar3 + 1;
      uVar3 = (ulonglong)uVar2;
    } while (uVar2 < (uint)uVar4);
  }
  return 1;
}


/* ================= FUN_140009f90 @ 140009f90  size=16  callers=5 callees=0 ================= */
/* CALLERS: FUN_140003e50, FUN_1400077b0, FUN_140007f40, FUN_1400088e0, FUN_14000db80 */

undefined1 FUN_140009f90(int param_1)

{
  if ((param_1 != 2) && (param_1 != 0xd)) {
    return 0;
  }
  return 1;
}


/* ================= FUN_140009fa0 @ 140009fa0  size=18  callers=2 callees=0 ================= */
/* CALLERS: FUN_14000a4b0, FUN_14000dc30 */

undefined1 FUN_140009fa0(longlong param_1)

{
  if ((param_1 != 0x2e) && (param_1 != 0x1d)) {
    return 0;
  }
  return 1;
}


/* ================= FUN_140009fc0 @ 140009fc0  size=6  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400074d0 */

bool FUN_140009fc0(int param_1)

{
  return param_1 == 0;
}


/* ================= FUN_140009fd0 @ 140009fd0  size=54  callers=2 callees=2 ================= */
/* CALLERS: FUN_1400070e0, FUN_14000a4b0 */
/* CALLEES: FUN_140004920, FUN_14000a630 */

undefined1 FUN_140009fd0(longlong param_1)

{
  longlong lVar1;
  char cVar2;
  
  if (param_1 != 0) {
    cVar2 = FUN_14000a630();
    if (cVar2 != '\0') {
      lVar1 = FUN_140004920();
      if (lVar1 != 0) {
        return 1;
      }
    }
  }
  return 0;
}


/* ================= FUN_14000a010 @ 14000a010  size=442  callers=1 callees=3 ================= */
/* CALLERS: FUN_14000a1d0 */
/* CALLEES: FUN_14000a420, FUN_14000a490, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

ulonglong FUN_14000a010(undefined8 *param_1)

{
  ulonglong uVar1;
  uint uVar2;
  longlong lVar3;
  int iVar4;
  ulonglong uVar5;
  byte bVar6;
  ulonglong uVar7;
  undefined8 uVar8;
  undefined1 auStack_178 [32];
  undefined8 local_158;
  undefined8 uStack_150;
  undefined8 local_148;
  undefined8 uStack_140;
  undefined8 local_138;
  undefined8 uStack_130;
  undefined4 local_128;
  undefined4 uStack_124;
  undefined4 uStack_120;
  undefined4 uStack_11c;
  undefined4 local_118;
  undefined4 uStack_114;
  undefined4 uStack_110;
  undefined4 uStack_10c;
  undefined8 local_108;
  undefined8 uStack_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined8 local_e8;
  undefined8 uStack_e0;
  undefined4 local_d8;
  undefined4 uStack_d4;
  undefined4 uStack_d0;
  undefined4 uStack_cc;
  undefined4 local_c8;
  undefined4 uStack_c4;
  undefined4 uStack_c0;
  undefined4 uStack_bc;
  undefined8 local_b8;
  undefined8 auStack_b0 [4];
  undefined8 uStack_90;
  undefined8 local_88;
  undefined8 uStack_80;
  undefined8 local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 local_38;
  undefined8 uStack_30;
  undefined8 local_28;
  undefined8 uStack_20;
  ulonglong local_18;
  ulonglong local_10;
  
  local_10 = DAT_140096fe8 ^ (ulonglong)auStack_178;
  local_b8 = *param_1;
  uVar1 = param_1[0x14];
  uVar8 = 0;
  uVar7 = 0;
  auStack_b0[0] = param_1[1];
  uVar5 = 0;
  auStack_b0[1] = param_1[2];
  auStack_b0[2] = param_1[3];
  auStack_b0[3] = param_1[4];
  uStack_90 = param_1[5];
  local_88 = param_1[6];
  uStack_80 = param_1[7];
  local_78 = param_1[8];
  uStack_70 = param_1[9];
  local_68 = param_1[10];
  uStack_60 = param_1[0xb];
  local_58 = param_1[0xc];
  uStack_50 = param_1[0xd];
  local_48 = param_1[0xe];
  uStack_40 = param_1[0xf];
  local_38 = param_1[0x10];
  uStack_30 = param_1[0x11];
  local_28 = param_1[0x12];
  uStack_20 = param_1[0x13];
  local_18 = uVar1;
  uVar2 = (uint)local_b8;
  if (uVar2 != 0) {
    do {
      if (((char)uVar8 != '\0') && ((char)uVar7 != '\0')) {
        return 1;
      }
      lVar3 = uVar5 * 10;
      local_158 = auStack_b0[uVar5 * 10];
      uStack_150 = auStack_b0[uVar5 * 10 + 1];
      local_148 = auStack_b0[uVar5 * 10 + 2];
      uStack_140 = auStack_b0[uVar5 * 10 + 3];
      local_138 = auStack_b0[uVar5 * 10 + 4];
      uStack_130 = auStack_b0[uVar5 * 10 + 5];
      local_128 = *(undefined4 *)(&uStack_80 + uVar5 * 10);
      uStack_124 = *(undefined4 *)((longlong)&uStack_80 + uVar5 * 0x50 + 4);
      uStack_120 = *(undefined4 *)(&local_78 + uVar5 * 10);
      uStack_11c = *(undefined4 *)((longlong)&local_78 + uVar5 * 0x50 + 4);
      local_118 = *(undefined4 *)(&uStack_70 + uVar5 * 10);
      uStack_114 = *(undefined4 *)((longlong)&uStack_70 + uVar5 * 0x50 + 4);
      uStack_110 = *(undefined4 *)(&local_68 + uVar5 * 10);
      uStack_10c = *(undefined4 *)((longlong)&local_68 + uVar5 * 0x50 + 4);
      uVar1 = FUN_14000a490(&local_158);
      iVar4 = (int)uVar5;
      bVar6 = (byte)uVar7;
      if ((char)uVar1 == '\0') {
        local_108 = auStack_b0[lVar3];
        uStack_100 = auStack_b0[lVar3 + 1];
        local_f8 = auStack_b0[lVar3 + 2];
        uStack_f0 = auStack_b0[lVar3 + 3];
        local_e8 = auStack_b0[lVar3 + 4];
        uStack_e0 = auStack_b0[lVar3 + 5];
        local_d8 = *(undefined4 *)(&uStack_80 + lVar3);
        uStack_d4 = *(undefined4 *)((longlong)&uStack_80 + lVar3 * 8 + 4);
        uStack_d0 = *(undefined4 *)(&local_78 + lVar3);
        uStack_cc = *(undefined4 *)((longlong)&local_78 + lVar3 * 8 + 4);
        local_c8 = *(undefined4 *)(&uStack_70 + lVar3);
        uStack_c4 = *(undefined4 *)((longlong)&uStack_70 + lVar3 * 8 + 4);
        uStack_c0 = *(undefined4 *)(&local_68 + lVar3);
        uStack_bc = *(undefined4 *)((longlong)&local_68 + lVar3 * 8 + 4);
        uVar1 = FUN_14000a420(&local_108);
        uVar7 = (ulonglong)bVar6;
        if ((char)uVar1 != '\0') {
          uVar7 = 1;
        }
      }
      else {
        uVar8 = 1;
      }
      uVar5 = (ulonglong)(iVar4 + 1U);
    } while (iVar4 + 1U < uVar2);
  }
  return uVar1 & 0xffffffffffffff00;
}


/* ================= FUN_14000a1d0 @ 14000a1d0  size=254  callers=1 callees=2 ================= */
/* CALLERS: FUN_140009120 */
/* CALLEES: FUN_14000a010, FUN_14000a420 */

undefined8 FUN_14000a1d0(int *param_1)

{
  char cVar1;
  undefined8 uVar2;
  undefined8 local_108;
  undefined8 uStack_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined8 local_e8;
  undefined8 uStack_e0;
  int local_d8;
  int iStack_d4;
  int iStack_d0;
  int iStack_cc;
  int local_c8;
  int iStack_c4;
  int iStack_c0;
  int iStack_bc;
  undefined8 local_b8;
  undefined8 uStack_b0;
  undefined8 local_a8;
  undefined8 uStack_a0;
  undefined8 local_98;
  undefined8 uStack_90;
  undefined8 local_88;
  undefined8 uStack_80;
  undefined8 local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  undefined8 uStack_40;
  int local_38;
  int iStack_34;
  int iStack_30;
  int iStack_2c;
  int local_28;
  int iStack_24;
  int iStack_20;
  int iStack_1c;
  undefined8 local_18;
  
  if (*param_1 != 0) {
    local_108 = *(undefined8 *)(param_1 + 2);
    uStack_100 = *(undefined8 *)(param_1 + 4);
    local_f8 = *(undefined8 *)(param_1 + 6);
    uStack_f0 = *(undefined8 *)(param_1 + 8);
    local_e8 = *(undefined8 *)(param_1 + 10);
    uStack_e0 = *(undefined8 *)(param_1 + 0xc);
    local_d8 = param_1[0xe];
    iStack_d4 = param_1[0xf];
    iStack_d0 = param_1[0x10];
    iStack_cc = param_1[0x11];
    local_c8 = param_1[0x12];
    iStack_c4 = param_1[0x13];
    iStack_c0 = param_1[0x14];
    iStack_bc = param_1[0x15];
    if (*param_1 == 1) {
      uVar2 = FUN_14000a420(&local_108);
      return uVar2;
    }
    cVar1 = FUN_14000a420(&local_108);
    if (cVar1 != '\0') {
      local_b8 = *(undefined8 *)param_1;
      uStack_b0 = *(undefined8 *)(param_1 + 2);
      local_18 = *(undefined8 *)(param_1 + 0x28);
      local_a8 = *(undefined8 *)(param_1 + 4);
      uStack_a0 = *(undefined8 *)(param_1 + 6);
      local_98 = *(undefined8 *)(param_1 + 8);
      uStack_90 = *(undefined8 *)(param_1 + 10);
      local_88 = *(undefined8 *)(param_1 + 0xc);
      uStack_80 = *(undefined8 *)(param_1 + 0xe);
      local_78 = *(undefined8 *)(param_1 + 0x10);
      uStack_70 = *(undefined8 *)(param_1 + 0x12);
      local_68 = *(undefined8 *)(param_1 + 0x14);
      uStack_60 = *(undefined8 *)(param_1 + 0x16);
      local_58 = *(undefined8 *)(param_1 + 0x18);
      uStack_50 = *(undefined8 *)(param_1 + 0x1a);
      local_38 = param_1[0x20];
      iStack_34 = param_1[0x21];
      iStack_30 = param_1[0x22];
      iStack_2c = param_1[0x23];
      local_48 = *(undefined8 *)(param_1 + 0x1c);
      uStack_40 = *(undefined8 *)(param_1 + 0x1e);
      local_28 = param_1[0x24];
      iStack_24 = param_1[0x25];
      iStack_20 = param_1[0x26];
      iStack_1c = param_1[0x27];
      cVar1 = FUN_14000a010(&local_b8);
      if (cVar1 == '\0') {
        return 1;
      }
    }
  }
  return 0;
}


/* ================= FUN_14000a2d0 @ 14000a2d0  size=28  callers=3 callees=0 ================= */
/* CALLERS: FUN_140006b10, FUN_140009a00, FUN_14000a4b0 */

ulonglong FUN_14000a2d0(ulonglong param_1)

{
  ulonglong in_RAX;
  
  if ((param_1 < 0x2f) && (in_RAX = 0x410000002000, (0x410000002000U >> (param_1 & 0x3f) & 1) != 0))
  {
    return 0x410000002001;
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_14000a2f0 @ 14000a2f0  size=20  callers=32 callees=1 ================= */
/* CALLERS: FUN_1400028e0, FUN_140002900, FUN_140002970, FUN_1400029a0, FUN_1400029c0, FUN_140002a20, FUN_140002a50, FUN_140003a70, FUN_140003d10, FUN_140003e50, FUN_140004070, FUN_140004fe0, FUN_140005960, FUN_140006730, FUN_140006d30, FUN_140007070, FUN_140007630, FUN_140007670, FUN_140007920, FUN_140008750, FUN_140008f40, FUN_140009400, FUN_1400094f0, FUN_14000aaf0, FUN_14000bf80, FUN_14000c2c0, FUN_14000d7a0, FUN_14000d8c0, FUN_14000d930, FUN_14000d9a0, FUN_14000dd10, FUN_14000de50 */
/* CALLEES: FUN_1400078f0 */

bool FUN_14000a2f0(void)

{
  int iVar1;
  
  iVar1 = FUN_1400078f0();
  return iVar1 != -1;
}


/* ================= FUN_14000a310 @ 14000a310  size=131  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400065c0 */
/* CALLEES: FUN_14000e430 */

undefined8 FUN_14000a310(char *param_1)

{
  char cVar1;
  longlong lVar2;
  longlong lVar3;
  
  if (param_1 != (char *)0x0) {
    lVar3 = -1;
    lVar2 = -1;
    do {
      lVar2 = lVar2 + 1;
    } while (param_1[lVar2] != '\0');
    if ((lVar2 != 0) && (*param_1 == '.')) {
      return 1;
    }
    cVar1 = FUN_14000e430();
    if (cVar1 == ' ') {
      lVar2 = -1;
      do {
        lVar2 = lVar2 + 1;
      } while (param_1[lVar2] != '\0');
      if ((lVar2 == 0) || (*param_1 != '\\')) {
        do {
          lVar3 = lVar3 + 1;
        } while (param_1[lVar3] != '\0');
        if (lVar3 == 0) {
          return 1;
        }
        if (*param_1 != '/') {
          return 1;
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_14000a3a0 @ 14000a3a0  size=117  callers=1 callees=1 ================= */
/* CALLERS: FUN_140009e60 */
/* CALLEES: FUN_14000a490 */

bool FUN_14000a3a0(int *param_1)

{
  int iVar1;
  char cVar2;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 local_38;
  undefined8 uStack_30;
  int local_28;
  int iStack_24;
  int iStack_20;
  int iStack_1c;
  int local_18;
  int iStack_14;
  int iStack_10;
  int iStack_c;
  
  local_58 = *(undefined8 *)param_1;
  uStack_50 = *(undefined8 *)(param_1 + 2);
  local_48 = *(undefined8 *)(param_1 + 4);
  uStack_40 = *(undefined8 *)(param_1 + 6);
  local_38 = *(undefined8 *)(param_1 + 8);
  uStack_30 = *(undefined8 *)(param_1 + 10);
  local_28 = param_1[0xc];
  iStack_24 = param_1[0xd];
  iStack_20 = param_1[0xe];
  iStack_1c = param_1[0xf];
  local_18 = param_1[0x10];
  iStack_14 = param_1[0x11];
  iStack_10 = param_1[0x12];
  iStack_c = param_1[0x13];
  cVar2 = FUN_14000a490(&local_58);
  if (cVar2 != '\0') {
    return 1 < param_1[6] - 0x40aU;
  }
  iVar1 = *param_1;
  if (((iVar1 != 1) && (iVar1 != 2)) && (iVar1 != 3)) {
    return false;
  }
  return true;
}


/* ================= FUN_14000a420 @ 14000a420  size=37  callers=2 callees=0 ================= */
/* CALLERS: FUN_14000a010, FUN_14000a1d0 */

ulonglong FUN_14000a420(int *param_1)

{
  uint uVar1;
  ulonglong uVar2;
  
  uVar1 = *param_1 - 2;
  uVar2 = (ulonglong)uVar1;
  if (uVar1 < 0xf) {
    uVar2 = (ulonglong)(int)uVar1;
    switch(uVar1) {
    default:
      return CONCAT71((int7)(int3)(uVar1 >> 8),1);
    case 2:
    case 3:
    case 5:
    case 6:
    case 7:
    case 0xb:
    case 0xc:
      break;
    }
  }
  return uVar2 & 0xffffffffffffff00;
}


/* ================= FUN_14000a490 @ 14000a490  size=18  callers=2 callees=0 ================= */
/* CALLERS: FUN_14000a010, FUN_14000a3a0 */

undefined1 FUN_14000a490(int *param_1)

{
  if ((*param_1 != 4) && (*param_1 != 5)) {
    return 0;
  }
  return 1;
}


/* ================= FUN_14000a4b0 @ 14000a4b0  size=194  callers=1 callees=8 ================= */
/* CALLERS: FUN_140002fb0 */
/* CALLEES: FUN_140006fb0, FUN_1400079a0, FUN_1400090c0, FUN_140009fa0, FUN_140009fd0, FUN_14000a2d0, FUN_14000a630, thunk_FUN_140009120 */

ulonglong FUN_14000a4b0(longlong param_1)

{
  char cVar1;
  uint uVar2;
  ulonglong in_RAX;
  undefined8 uVar3;
  ulonglong uVar4;
  
  if (param_1 != 0) {
    in_RAX = FUN_14000a630();
    if ((char)in_RAX != '\0') {
      uVar4 = 0;
      uVar3 = FUN_140006fb0();
      cVar1 = FUN_140009fd0(param_1);
      if (cVar1 != '\0') {
        uVar4 = thunk_FUN_140009120(param_1);
        return uVar4;
      }
      cVar1 = FUN_140009fa0(uVar3);
      if (cVar1 != '\0') {
        cVar1 = FUN_1400090c0(param_1,100);
        return (ulonglong)(cVar1 == '\0');
      }
      cVar1 = FUN_14000a2d0();
      if (cVar1 != '\0') {
        uVar2 = FUN_1400079a0(param_1,0x3f);
        uVar4 = (ulonglong)uVar2;
      }
      return uVar4 & 0xff;
    }
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_14000a590 @ 14000a590  size=124  callers=1 callees=1 ================= */
/* CALLERS: FUN_140004970 */
/* CALLEES: FUN_140007070 */

undefined4
FUN_14000a590(undefined8 param_1,undefined8 param_2,undefined1 param_3,uint param_4,int *param_5)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  
  if (param_4 == 0) {
LAB_14000a5f1:
    uVar2 = 1;
  }
  else {
    uVar3 = 0;
    if (param_4 != 0) {
      do {
        iVar1 = FUN_140007070(param_1,param_2,param_3);
        if (iVar1 == *param_5) goto LAB_14000a5f1;
        uVar3 = uVar3 + 1;
        param_5 = param_5 + 1;
      } while (uVar3 < param_4);
    }
    uVar2 = 0;
  }
  return uVar2;
}


/* ================= FUN_14000a610 @ 14000a610  size=28  callers=18 callees=0 ================= */
/* CALLERS: FUN_140002fb0, FUN_140003cc0, FUN_1400043a0, FUN_140004920, FUN_140005430, FUN_1400054c0, FUN_1400058b0, FUN_140005be0, FUN_140005e90, FUN_140006a40, FUN_140006fb0, FUN_140008090, FUN_1400080f0, FUN_14000b630, FUN_14000b830, FUN_14000dd10, FUN_14000ddf0, FUN_14000e0d0 */

ulonglong FUN_14000a610(longlong param_1)

{
  longlong lVar1;
  ulonglong in_RAX;
  
  if ((param_1 != 0) && (lVar1 = *(longlong *)(param_1 + 0x50), in_RAX = 0, lVar1 != 0)) {
    return CONCAT71((int7)((ulonglong)lVar1 >> 8),*(int *)(lVar1 + 0x88) == 0);
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_14000a630 @ 14000a630  size=7  callers=32 callees=0 ================= */
/* CALLERS: FUN_140002fb0, FUN_1400041b0, FUN_140004ce0, FUN_140004d70, FUN_140004df0, FUN_140004f60, FUN_140005380, FUN_140005770, FUN_140005a20, FUN_140005ab0, FUN_140005be0, FUN_140005dc0, FUN_140005e30, FUN_140005ef0, FUN_140006210, FUN_1400064c0, FUN_140006580, FUN_1400065c0, FUN_140006c00, FUN_140006d80, FUN_140006e30, FUN_140006ef0, FUN_140007240, FUN_1400073c0, FUN_1400079a0, FUN_140008850, FUN_140008d40, FUN_1400090c0, FUN_140009120, FUN_140009fd0, FUN_14000a4b0, FUN_14000b830 */

bool FUN_14000a630(longlong param_1)

{
  return param_1 != 0;
}


/* ================= FUN_14000a640 @ 14000a640  size=45  callers=2 callees=0 ================= */
/* CALLERS: FUN_140005df0, FUN_14000dd10 */

ulonglong FUN_14000a640(longlong param_1,ulonglong param_2)

{
  longlong *plVar1;
  
  plVar1 = &DAT_140097c00;
  while ((*plVar1 != param_1 || (*(ulonglong *)(param_1 + 0xa8) <= param_2))) {
    plVar1 = plVar1 + 1;
    if (0x140097c0f < (longlong)plVar1) {
      return (ulonglong)plVar1 & 0xffffffffffffff00;
    }
  }
  return CONCAT71((int7)((ulonglong)plVar1 >> 8),1);
}


/* ================= FUN_14000a670 @ 14000a670  size=27  callers=13 callees=0 ================= */
/* CALLERS: FUN_1400072b0, FUN_140007320, FUN_140007800, FUN_1400088e0, FUN_140008a20, FUN_140008a60, FUN_140008ad0, FUN_140008b40, FUN_140008b90, FUN_140008c60, FUN_140008cb0, FUN_140009e00, FUN_140009e30 */

undefined8 FUN_14000a670(ulonglong param_1)

{
  if ((param_1 != 0) && (param_1 >> 0x34 == 0xb23)) {
    return 1;
  }
  return 0;
}


/* ================= FUN_14000a690 @ 14000a690  size=196  callers=0 callees=0 ================= */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14000a690(int *param_1,undefined *param_2)

{
  longlong *plVar1;
  uint uVar2;
  int local_48 [3];
  undefined8 local_3c;
  undefined8 uStack_34;
  undefined8 uStack_2c;
  undefined8 uStack_24;
  ulonglong local_1c;
  undefined4 local_14;
  undefined8 local_10;
  
  if (DAT_140098c38 != (code *)0x0) {
    local_48[0] = *param_1;
    uVar2 = 0;
    local_3c = 0;
    uStack_34 = 0;
    uStack_2c = 0;
    uStack_24 = 0;
    local_1c = 0;
    local_14 = 0;
    if (local_48[0] == 0) {
      local_48[0] = param_1[1];
    }
    local_10 = *(undefined8 *)(param_1 + 2);
    local_48[2] = param_1[4];
    local_48[1] = 0x44;
    if (param_2 == &DAT_140098c10) {
      plVar1 = &DAT_140098c18;
      local_1c = _DAT_140098c30 >> 0x20;
      uStack_34 = CONCAT44((undefined4)DAT_140098c20,DAT_140098c18._4_4_);
      uStack_2c = CONCAT44(_DAT_140098c28,DAT_140098c20._4_4_);
      uStack_24 = CONCAT44(_DAT_140098c30,DAT_140098c28_4);
      do {
        if (*plVar1 == 0) break;
        uVar2 = uVar2 + 1;
        plVar1 = plVar1 + 1;
      } while (uVar2 < 4);
      local_3c = CONCAT44((int)DAT_140098c18,uVar2);
    }
    (*DAT_140098c38)(local_48);
    _DAT_140098c10 = 0;
    DAT_140098c18 = 0;
    _DAT_140098c30 = 0;
    DAT_140098c20 = 0;
    _DAT_140098c28 = 0;
  }
  return;
}


/* ================= _guard_check_icall @ 14000a760  size=3  callers=13 callees=0 ================= */
/* CALLERS: FUN_140003a70, FUN_14000aaf0, FUN_14000e080, FUN_14003e9a0, FUN_14003f1a0, FUN_14003f1e0, FUN_14003f280, FUN_14003f830, FUN_140041780, FUN_1400423e0, FUN_140059370, FUN_14005a3f0, FUN_14005a530 */

void _guard_check_icall(void)

{
  return;
}


/* ================= FUN_14000a770 @ 14000a770  size=89  callers=12 callees=1 ================= */
/* CALLERS: FUN_140009400, FUN_1400094f0, FUN_1400097d0, FUN_140009970, FUN_140009af0, FUN_14000a870, FUN_14000aaf0, FUN_14000bf80, FUN_14000c2c0, FUN_14000ca30, FUN_14000cea0, FUN_14000dfc0 */
/* CALLEES: FUN_140007c50 */

void FUN_14000a770(undefined4 param_1)

{
  undefined4 local_48;
  undefined4 local_44;
  undefined8 local_40;
  undefined8 local_38;
  undefined8 uStack_30;
  undefined8 local_28;
  undefined8 uStack_20;
  longlong local_18;
  undefined8 local_10;
  
  local_18 = FUN_140007c50();
  if ((local_18 != 0) && (DAT_140098c38 != (code *)0x0)) {
    local_44 = 0x44;
    local_40 = 0;
    local_10 = 0;
    local_38 = 0;
    uStack_30 = 0;
    local_28 = 0;
    uStack_20 = 0;
    local_48 = param_1;
    (*DAT_140098c38)(&local_48);
  }
  return;
}


/* ================= FUN_14000a7d0 @ 14000a7d0  size=10  callers=6 callees=1 ================= */
/* CALLERS: FUN_140004970, FUN_14000aaf0, FUN_14000bb50, FUN_14000c2c0, FUN_14000c8c0, FUN_14000d240 */
/* CALLEES: FUN_14000a7e0 */

void FUN_14000a7d0(undefined8 param_1)

{
  FUN_14000a7e0(param_1,0,0);
  return;
}


/* ================= FUN_14000a7e0 @ 14000a7e0  size=140  callers=3 callees=1 ================= */
/* CALLERS: FUN_140004070, FUN_14000a7d0, FUN_14000aaf0 */
/* CALLEES: FUN_140007c50 */

void FUN_14000a7e0(undefined4 param_1,longlong param_2,longlong param_3)

{
  bool bVar1;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  uint uStack_3c;
  longlong local_38 [5];
  undefined8 local_10;
  
  local_38[4] = FUN_140007c50();
  if ((local_38[4] != 0) && (DAT_140098c38 != (code *)0x0)) {
    local_44 = 0x42;
    local_38[0] = 0;
    local_38[1] = 0;
    local_38[2] = 0;
    local_38[3] = 0;
    local_10 = 0;
    bVar1 = param_2 != 0;
    if (bVar1) {
      local_38[0] = param_2;
    }
    uStack_3c = (uint)bVar1;
    local_40 = 0;
    if (param_3 != 0) {
      local_38[(int)(uint)bVar1] = param_3;
      uStack_3c = uStack_3c + 1;
    }
    local_48 = param_1;
    (*DAT_140098c38)(&local_48);
  }
  return;
}


/* ================= FUN_14000a870 @ 14000a870  size=437  callers=0 callees=18 ================= */
/* CALLEES: FUN_140002f80, FUN_140003a70, FUN_1400078f0, FUN_14000a770, FUN_14000aaf0, FUN_14000bf80, FUN_14000c2c0, FUN_14000ca30, FUN_14000cea0, FUN_140019ae0, clock, fclose, fopen, fread, fseek, ftell, thunk_FUN_14000e6b0, thunk_FUN_14000ecd0 */

undefined1 * FUN_14000a870(char *param_1,undefined8 param_2,undefined8 param_3)

{
  int iVar1;
  long lVar2;
  clock_t cVar3;
  FILE *_File;
  undefined1 *puVar4;
  void *_DstBuf;
  size_t sVar5;
  undefined4 uVar6;
  size_t _Count;
  
  DAT_140097bf4 = 1;
  iVar1 = FUN_1400078f0(0,param_2,param_3,param_1);
  if (iVar1 == -1) {
    FUN_14000a770(0x45);
    DAT_140097bf4 = 0;
    return (undefined1 *)0x0;
  }
  _File = fopen(param_1,"rb");
  if (_File == (FILE *)0x0) {
    FUN_14000a770(0x46);
    DAT_140097bf4 = 0;
    return (undefined1 *)0x0;
  }
  fseek(_File,0,2);
  lVar2 = ftell(_File);
  _Count = (size_t)lVar2;
  fseek(_File,0,0);
  if ((longlong)_Count < 1) {
    FUN_14000a770(0x47);
    DAT_140097bf4 = 0;
    return (undefined1 *)0x0;
  }
  puVar4 = (undefined1 *)FUN_140019ae0(0x498);
  if (puVar4 == (undefined1 *)0x0) {
    FUN_14000a770(0x48);
    DAT_140097bf4 = 0;
    return (undefined1 *)0x0;
  }
  *(size_t *)(puVar4 + 0x28) = _Count;
  _DstBuf = (void *)FUN_140019ae0(lVar2);
  *(void **)(puVar4 + 0x38) = _DstBuf;
  if (_DstBuf == (void *)0x0) {
    uVar6 = 0x48;
  }
  else {
    sVar5 = fread(_DstBuf,1,_Count,_File);
    if (sVar5 == _Count) {
      fclose(_File);
      FUN_140002f80(puVar4);
      *puVar4 = 0;
      cVar3 = clock();
      *(longlong *)(puVar4 + 0x30) = (longlong)cVar3;
      DAT_14286d3c8 = puVar4;
      iVar1 = FUN_14000c2c0(puVar4);
      if ((((-1 < iVar1) && (iVar1 = FUN_14000cea0(puVar4), -1 < iVar1)) &&
          (iVar1 = FUN_14000ca30(puVar4), -1 < iVar1)) &&
         (iVar1 = FUN_14000bf80(puVar4), -1 < iVar1)) {
        if ((char)param_2 == '\0') {
          DAT_140097bf4 = 0;
          *puVar4 = 1;
          return puVar4;
        }
        FUN_14000aaf0(puVar4);
        DAT_140097bf4 = 0;
        *puVar4 = 1;
        thunk_FUN_14000e6b0();
        thunk_FUN_14000ecd0();
        return puVar4;
      }
      goto LAB_14000a940;
    }
    uVar6 = 0x49;
  }
  FUN_14000a770(uVar6);
LAB_14000a940:
  FUN_140003a70(puVar4);
  DAT_140097bf4 = 0;
  return (undefined1 *)0x0;
}


/* ================= FUN_14000aa30 @ 14000aa30  size=180  callers=1 callees=3 ================= */
/* CALLERS: FUN_14000aaf0 */
/* CALLEES: FUN_140004c50, FUN_140008290, FUN_140018a20 */

undefined8 FUN_14000aa30(longlong param_1)

{
  int iVar1;
  undefined8 uVar2;
  longlong local_res8;
  undefined4 local_res10 [2];
  
  if (param_1 != 0) {
    uVar2 = FUN_140008290(0x10);
    iVar1 = FUN_140004c50(param_1,uVar2);
    if (iVar1 != -1) {
      *(undefined1 *)(param_1 + 0x178) = 4;
    }
    iVar1 = FUN_140018a20(*(undefined8 *)(param_1 + 0x78),param_1 + 0x468,&local_res8,
                          param_1 + 0x460,local_res10,0);
    if (iVar1 == 0) {
      if (0 < local_res8) {
        *(undefined4 *)(param_1 + 0x94) = *(undefined4 *)(**(longlong **)(param_1 + 0x468) + 0x48);
      }
      *(undefined4 *)(param_1 + 0xb4) = local_res10[0];
      *(int *)(param_1 + 0xb8) = (int)local_res8;
      return 0;
    }
  }
  return 0xffffffff;
}


/* ================= FUN_14000aaf0 @ 14000aaf0  size=2874  callers=1 callees=28 ================= */
/* CALLERS: FUN_14000a870 */
/* CALLEES: FUN_1400019c0, FUN_140001f60, FUN_140002250, FUN_1400023a0, FUN_140004070, FUN_140004c50, FUN_140005310, FUN_140005350, FUN_1400068b0, FUN_140008290, FUN_140008620, FUN_14000a2f0, FUN_14000a770, FUN_14000a7d0, FUN_14000a7e0, FUN_14000aa30, FUN_14000b630, FUN_14000c6f0, FUN_14000c7e0, FUN_14000d6d0, FUN_14000d8c0, FUN_14000d930, FUN_14000dd10, FUN_140018bd0, __security_check_cookie, _guard_check_icall, clock, snprintf */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_14000aaf0(longlong param_1)

{
  longlong *plVar1;
  ulonglong uVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  clock_t cVar7;
  clock_t cVar8;
  clock_t cVar9;
  clock_t cVar10;
  undefined8 uVar11;
  longlong lVar12;
  longlong lVar13;
  undefined *puVar14;
  int iVar15;
  int iVar16;
  undefined1 auStack_78 [32];
  longlong *local_58;
  undefined8 local_50;
  int local_48;
  undefined1 local_40 [16];
  ulonglong local_30;
  
  local_30 = DAT_140096fe8 ^ (ulonglong)auStack_78;
  lVar12 = param_1;
  cVar3 = FUN_14000a2f0();
  if (cVar3 == '\0') {
    return -1;
  }
  iVar16 = 0;
  plVar1 = (longlong *)(lVar12 + 0x78);
  local_50 = 0;
  _DAT_140098c10 = 0;
  DAT_140098c18 = 0;
  _DAT_140098c30 = 0;
  DAT_140098c20 = 0;
  _DAT_140098c28 = 0;
  local_58 = plVar1;
  iVar4 = FUN_140018bd0(0,1,FUN_14000a690,&DAT_140098c10);
  if (iVar4 == 0) {
    local_48 = 0;
    *(longlong *)(*plVar1 + 0x338) = param_1;
    *(undefined4 *)(param_1 + 0x90) = 1;
    *(undefined4 *)(param_1 + 0xb0) = 0;
    *(undefined8 *)(param_1 + 0xbc) = 0;
    *(undefined1 *)(param_1 + 0x178) = 1;
    *(undefined8 *)(param_1 + 200) = 0xffffffffffffffff;
    *(undefined8 *)(param_1 + 0x88) = 0;
    uVar11 = FUN_140008290(0);
    uVar5 = FUN_140004c50(param_1,uVar11);
    lVar12 = FUN_140008620(param_1,uVar5);
    *(longlong *)(param_1 + 0x98) = lVar12;
    uVar11 = FUN_140008290(1);
    uVar5 = FUN_140004c50(param_1,uVar11);
    lVar13 = FUN_140008620(param_1,uVar5);
    *(longlong *)(param_1 + 0xa0) = lVar13;
    local_50 = 0;
    *(longlong *)(param_1 + 0xa8) = lVar13 + lVar12;
    local_58 = (longlong *)0x0;
    FUN_140002250(param_1 + 0x180,1000000,8);
    local_50 = 0;
    local_58 = (longlong *)0x0;
    FUN_140002250(param_1 + 0x1b8,1000000,8);
    iVar4 = FUN_1400023a0(param_1 + 0x180,*(undefined8 *)(param_1 + 0xa8));
    if ((iVar4 < 0) ||
       (iVar4 = FUN_1400023a0(param_1 + 0x1b8,*(undefined8 *)(param_1 + 0xa8)), iVar4 < 0)) {
      FUN_14000a770(0x48);
      iVar4 = -1;
    }
    else {
      uVar2 = *(ulonglong *)(param_1 + 0xa8);
      *(ulonglong *)(param_1 + 400) = uVar2;
      *(ulonglong *)(param_1 + 0x1c8) = uVar2;
      uVar11 = FUN_1400019c0(uVar2 / 10,1);
      *(undefined8 *)(param_1 + 0x458) = uVar11;
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 0x1f0,10000,8,0);
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 0x228,10000,8,0);
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 0x260,1000000,8,0);
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 0x378,1000000,8,0);
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 0x3b0,1000000,8,0);
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 0x298,100000,8,0);
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 0x2d0,10000,8,0);
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 0x308,1000000,8,0);
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 0x340,1000000,8,0);
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 1000,1000000,8,0);
      local_50 = 0;
      local_58 = (longlong *)0x0;
      FUN_140002250(param_1 + 0x420,1000000,8,0);
      iVar6 = FUN_14000d930(*plVar1);
      iVar4 = local_48;
      iVar15 = iVar16;
      while (iVar6 == 0) {
        local_58 = (longlong *)0x0;
        lVar12 = FUN_140004070(*plVar1,0,0,0);
        if (lVar12 == 0) {
          if (*(longlong *)(*plVar1 + 0x148) == 0) {
            local_40[0] = 0;
          }
          else {
            snprintf(local_40,0x10);
          }
          FUN_14000a7e0(0x33);
        }
        else {
          FUN_14000dd10(param_1,lVar12);
          FUN_140001f60(param_1 + 0x228,*(undefined8 *)(lVar12 + 0x20));
          cVar7 = clock();
          iVar4 = FUN_14000b630(param_1);
          cVar8 = clock();
          iVar15 = iVar15 + (cVar8 - cVar7);
          if (iVar4 < 0) {
            if (*(longlong *)(*plVar1 + 0x140) == 0) {
              local_40[0] = 0;
            }
            else {
              snprintf(local_40,0x10);
            }
            FUN_14000a7e0(0x33);
          }
        }
        iVar6 = FUN_14000d930(*plVar1);
        iVar4 = iVar15;
      }
      local_48 = iVar4;
      iVar4 = FUN_14000d8c0(*plVar1);
      iVar6 = iVar16;
      while (iVar4 == 0) {
        local_58 = (longlong *)0x0;
        lVar12 = FUN_140004070(*plVar1,0,1,0);
        if (lVar12 == 0) {
          if (*(longlong *)(*plVar1 + 0x140) == 0) {
            local_40[0] = 0;
          }
          else {
            snprintf(local_40,0x10,"0x%llX");
          }
          FUN_14000a7e0(0x34);
        }
        else {
          FUN_14000dd10(param_1,lVar12);
          uVar11 = FUN_1400068b0(lVar12);
          FUN_140001f60(param_1 + 0x340,uVar11);
          FUN_140001f60(param_1 + 0x1f0,*(undefined8 *)(lVar12 + 0x20));
          cVar7 = clock();
          iVar4 = FUN_14000b630(param_1,lVar12);
          cVar8 = clock();
          iVar6 = iVar6 + (cVar8 - cVar7);
          if (iVar4 < 0) {
            if (*(longlong *)(*plVar1 + 0x140) == 0) {
              local_40[0] = 0;
            }
            else {
              snprintf(local_40,0x10,"0x%llX");
            }
            FUN_14000a7e0(0x34,local_40);
          }
          cVar7 = clock();
          iVar4 = FUN_14000d6d0(param_1);
          cVar8 = clock();
          iVar16 = iVar16 + (cVar8 - cVar7);
          if (iVar4 < 0) {
            FUN_14000a7d0(0x37);
          }
        }
        iVar4 = FUN_14000d8c0(*plVar1);
        iVar15 = local_48;
      }
      uVar11 = FUN_14000c7e0(param_1);
      *(undefined8 *)(param_1 + 0x88) = uVar11;
      cVar7 = clock();
      iVar4 = FUN_14000c6f0(param_1);
      cVar8 = clock();
      if (iVar4 < 0) {
        FUN_14000a7d0(0x38);
      }
      cVar9 = clock();
      iVar4 = FUN_14000aa30(param_1);
      cVar10 = clock();
      if (iVar4 < 0) {
        FUN_14000a7d0(0x39);
      }
      snprintf(&DAT_140097c10,0x800,"parsed debug types section in ms: %ld",iVar15);
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"parsed debug info section in ms: %ld",iVar6);
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"parsed source line info in ms: %ld",iVar16);
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"parsed macro info in ms: %ld",cVar8 - cVar7);
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"parsed call frame info in ms: %ld",cVar10 - cVar9);
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"File Size: %llu",*(undefined8 *)(param_1 + 0x28));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"debug_info size: %llu",*(undefined8 *)(param_1 + 0x98));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"debug_types size: %llu",*(undefined8 *)(param_1 + 0xa0));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"DWARF Version: %d",*(undefined2 *)(param_1 + 0x80));
      _guard_check_icall(&DAT_140097c10);
      uVar5 = FUN_140005310(param_1);
      uVar11 = FUN_140005350(uVar5);
      snprintf(&DAT_140097c10,0x800,"Data Encoding: %s",uVar11);
      _guard_check_icall(&DAT_140097c10);
      puVar14 = &DAT_14005f6f0;
      if (*(int *)(param_1 + 0x90) == 2) {
        puVar14 = &DAT_14005f6ec;
      }
      snprintf(&DAT_140097c10,0x800,"Source Language: %s",puVar14);
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8u ELF symbols",*(undefined4 *)(param_1 + 4));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8u DWARF symbols",*(undefined4 *)(param_1 + 0xb0));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8lld Funcs",*(undefined8 *)(param_1 + 0x388));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8lld InlineFuncs",*(undefined8 *)(param_1 + 0x3c0));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8lld Global variables",*(undefined8 *)(param_1 + 0x270));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8lld Types",*(undefined8 *)(param_1 + 0x318));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8u Members",*(undefined4 *)(param_1 + 0xbc));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8u Local variables",*(undefined4 *)(param_1 + 0xc0));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8lld Compilation units",*(undefined8 *)(param_1 + 0x350));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8lld SrcLines",*(undefined8 *)(param_1 + 0x3f8));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8lld Namespaces",*(undefined8 *)(param_1 + 0x2a8));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8lld Namespace extensions",*(undefined8 *)(param_1 + 0x2e0));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8lld Macros",*(undefined8 *)(param_1 + 0x430));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8u FDEs",*(undefined4 *)(param_1 + 0xb4));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"%8u CIEs",*(undefined4 *)(param_1 + 0xb8));
      _guard_check_icall(&DAT_140097c10);
      snprintf(&DAT_140097c10,0x800,"Total init time: %ld ms",
               (cVar10 - cVar9) + (cVar8 - cVar7) + iVar16 + iVar6 + iVar15);
      _guard_check_icall(&DAT_140097c10);
    }
  }
  else {
    FUN_14000a770(0x4a);
    iVar4 = -1;
  }
  return iVar4;
}


/* ================= FUN_14000b630 @ 14000b630  size=504  callers=1 callees=8 ================= */
/* CALLERS: FUN_14000aaf0 */
/* CALLEES: FUN_140005260, FUN_1400079e0, FUN_1400079f0, FUN_140007b60, FUN_140007b70, FUN_14000a610, FUN_14000e0d0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14000b630(longlong param_1,longlong param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  longlong lVar6;
  undefined8 uVar7;
  uint uVar8;
  uint uVar9;
  undefined1 auStack_c8 [32];
  undefined8 local_a8;
  code *local_98;
  code *local_90;
  code *local_88;
  code *local_80;
  code *local_78;
  code *local_70;
  code *local_68;
  code *local_60;
  code *local_58;
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_c8;
  if ((param_1 == 0) || (param_2 == 0)) {
    uVar7 = 0xffffffff;
  }
  else {
    iVar2 = FUN_1400079f0();
    iVar3 = FUN_140007b70(param_1);
    cVar1 = FUN_14000a610(param_2);
    if (cVar1 == '\0') {
      local_80 = FUN_140009c40;
      local_98 = FUN_1400096a0;
      local_68 = FUN_140009a00;
      local_90 = FUN_140009620;
      local_58 = FUN_140009730;
      local_88 = FUN_140009850;
      local_78 = FUN_140009b80;
      local_70 = FUN_140009bd0;
      local_60 = FUN_1400098d0;
      local_a8 = 9;
    }
    else {
      local_98 = FUN_140009c40;
      local_a8 = 3;
      local_90 = FUN_140009a00;
      local_88 = FUN_140009730;
    }
    FUN_14000e0d0(param_1,param_2,0,&local_98);
    iVar4 = FUN_1400079f0(param_1);
    iVar5 = FUN_140007b70(param_1);
    uVar8 = 0;
    uVar9 = 0;
    if (iVar4 != iVar2) {
      do {
        lVar6 = FUN_1400079e0(param_1,uVar9 + iVar2);
        if (lVar6 != 0) {
          *(undefined4 *)(*(longlong *)(lVar6 + 0x50) + 8) =
               *(undefined4 *)(*(longlong *)(param_2 + 0x50) + 8);
        }
        uVar9 = uVar9 + 1;
      } while (uVar9 < (uint)(iVar4 - iVar2));
    }
    if (iVar5 != iVar3) {
      do {
        lVar6 = FUN_140007b60(param_1,uVar8 + iVar3);
        if (lVar6 != 0) {
          *(undefined4 *)(*(longlong *)(lVar6 + 0x50) + 8) =
               *(undefined4 *)(*(longlong *)(param_2 + 0x50) + 8);
        }
        uVar8 = uVar8 + 1;
      } while (uVar8 < (uint)(iVar5 - iVar3));
    }
    iVar2 = FUN_140005260(param_2);
    if (iVar2 == 2) {
      *(undefined4 *)(param_1 + 0x90) = 2;
    }
    uVar7 = 0;
  }
  return uVar7;
}


/* ================= FUN_14000b830 @ 14000b830  size=798  callers=2 callees=11 ================= */
/* CALLERS: FUN_140002c00, FUN_140006e30 */
/* CALLEES: FUN_140003fc0, FUN_1400044f0, FUN_140004600, FUN_140004df0, FUN_140005380, FUN_140006ef0, FUN_140006fb0, FUN_140008850, FUN_14000a610, FUN_14000a630, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

longlong * FUN_14000b830(longlong *param_1,longlong param_2)

{
  char cVar1;
  uint uVar2;
  longlong lVar3;
  longlong lVar4;
  ulonglong uVar5;
  undefined8 uVar6;
  ulonglong uVar7;
  ulonglong *puVar8;
  ulonglong uVar9;
  ulonglong uVar10;
  int iVar11;
  undefined4 extraout_XMM0_Da;
  undefined4 extraout_XMM0_Da_00;
  undefined4 uVar12;
  undefined4 extraout_XMM0_Db;
  undefined4 uVar13;
  undefined4 extraout_XMM0_Dc;
  undefined4 uVar14;
  undefined4 extraout_XMM0_Dd;
  undefined4 uVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  undefined4 uVar19;
  undefined4 uVar20;
  undefined4 uVar21;
  undefined4 uVar22;
  undefined4 uVar23;
  undefined4 uVar24;
  undefined4 uVar25;
  undefined4 uVar26;
  undefined4 uVar27;
  undefined4 uVar28;
  undefined4 uVar29;
  undefined4 uVar30;
  undefined4 uVar31;
  undefined4 uVar32;
  undefined4 uVar33;
  undefined4 uVar34;
  undefined4 uVar35;
  undefined1 auStack_148 [32];
  char local_128;
  undefined4 local_120;
  undefined4 uStack_11c;
  undefined4 uStack_118;
  undefined4 uStack_114;
  longlong local_110;
  longlong lStack_108;
  longlong local_100;
  longlong lStack_f8;
  longlong local_f0;
  longlong lStack_e8;
  longlong local_e0;
  longlong lStack_d8;
  longlong local_d0;
  longlong lStack_c8;
  undefined8 local_b8;
  undefined8 uStack_b0;
  undefined8 local_a8;
  undefined8 uStack_a0;
  undefined8 local_98;
  undefined4 uStack_90;
  undefined4 uStack_8c;
  undefined4 local_88;
  undefined4 uStack_84;
  undefined4 uStack_80;
  undefined4 uStack_7c;
  undefined4 local_78;
  undefined4 uStack_74;
  undefined4 uStack_70;
  undefined4 uStack_6c;
  undefined4 local_68;
  undefined4 uStack_64;
  undefined4 uStack_60;
  undefined4 uStack_5c;
  undefined4 local_58;
  undefined4 uStack_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_148;
  uVar12 = 0;
  uVar13 = 0;
  uVar14 = 0;
  uVar15 = 0;
  uVar16 = 0;
  uVar17 = 0;
  uVar18 = 0;
  uVar19 = 0;
  uVar20 = 0;
  uVar21 = 0;
  uVar22 = 0;
  uVar23 = 0;
  uVar24 = 0;
  uVar25 = 0;
  uVar26 = 0;
  uVar27 = 0;
  uVar28 = 0;
  uVar29 = 0;
  uVar30 = 0;
  uVar31 = 0;
  uVar32 = 0;
  uVar33 = 0;
  uVar34 = 0;
  uVar35 = 0;
  local_b8 = 0;
  uStack_b0 = 0;
  local_a8 = 0;
  uStack_a0 = 0;
  local_98._0_4_ = 0;
  local_98._4_4_ = 0;
  uStack_90 = 0;
  uStack_8c = 0;
  local_88 = 0;
  uStack_84 = 0;
  uStack_80 = 0;
  uStack_7c = 0;
  local_78 = 0;
  uStack_74 = 0;
  uStack_70 = 0;
  uStack_6c = 0;
  local_68 = 0;
  uStack_64 = 0;
  uStack_60 = 0;
  uStack_5c = 0;
  local_58 = 0;
  uStack_54 = 0;
  uStack_50 = 0;
  uStack_4c = 0;
  local_120 = 0;
  uStack_11c = 0;
  uStack_118 = 0;
  uStack_114 = 0;
  local_110 = 0;
  lStack_108 = 0;
  local_100 = 0;
  lStack_f8 = 0;
  local_f0 = 0;
  lStack_e8 = 0;
  local_e0 = 0;
  lStack_d8 = 0;
  local_d0 = 0;
  lStack_c8 = 0;
  if ((param_2 != 0) &&
     (cVar1 = FUN_14000a630(param_2), uVar12 = extraout_XMM0_Da, uVar13 = extraout_XMM0_Db,
     uVar14 = extraout_XMM0_Dc, uVar15 = extraout_XMM0_Dd, cVar1 != '\0')) {
    lVar3 = FUN_140006fb0();
    if (lVar3 == 1) {
      lVar3 = FUN_140003fc0(param_2,0);
      uVar9 = local_a8;
      uVar10 = uStack_a0;
      if (lVar3 != 0) {
        do {
          if (9 < uVar10) break;
          lVar4 = FUN_140006fb0(lVar3);
          if (lVar4 == 0x21) {
            uVar2 = FUN_140008850();
            uVar5 = (ulonglong)uVar2;
            (&local_98)[uVar10] = uVar5;
            uVar10 = uStack_a0 + 1;
            uStack_a0 = uVar10;
            if (local_a8 == 0) {
              uVar9 = (ulonglong)uVar2;
              local_a8 = uVar5;
            }
            else {
              uVar9 = local_a8 * uVar5;
              local_a8 = uVar9;
            }
          }
          else {
            lVar4 = FUN_140006fb0(lVar3);
            if (((lVar4 == 0x45) && (uVar5 = FUN_140004df0(lVar3,0x71), local_128 != '\0')) &&
               (uVar5 < 10)) {
              uVar2 = FUN_140008850(lVar3);
              iVar11 = 0;
              uVar10 = uVar5;
              uStack_a0 = uVar5;
              if (uVar5 != 0) {
                uVar7 = (ulonglong)uVar2;
                puVar8 = &local_98;
                do {
                  *puVar8 = uVar7;
                  uVar9 = uVar7;
                  if (local_a8 != 0) {
                    uVar9 = local_a8 * uVar7;
                  }
                  iVar11 = iVar11 + 1;
                  puVar8 = puVar8 + 1;
                  local_a8 = uVar9;
                } while ((ulonglong)(longlong)iVar11 < uVar5);
              }
            }
          }
          cVar1 = FUN_14000a610(lVar3);
          lVar3 = FUN_1400044f0(extraout_XMM0_Da_00,1,cVar1 == '\0');
        } while (lVar3 != 0);
        local_d0 = CONCAT44(uStack_54,local_58);
        lStack_c8 = CONCAT44(uStack_4c,uStack_50);
        local_e0 = CONCAT44(uStack_64,local_68);
        lStack_d8 = CONCAT44(uStack_5c,uStack_60);
        local_f0 = CONCAT44(uStack_74,local_78);
        lStack_e8 = CONCAT44(uStack_6c,uStack_70);
        local_100 = CONCAT44(uStack_84,local_88);
        lStack_f8 = CONCAT44(uStack_7c,uStack_80);
        local_110 = CONCAT44(local_98._4_4_,(undefined4)local_98);
        lStack_108 = CONCAT44(uStack_8c,uStack_90);
        local_120 = (undefined4)local_a8;
        uStack_11c = local_a8._4_4_;
        uStack_118 = (undefined4)uStack_a0;
        uStack_114 = uStack_a0._4_4_;
      }
      FUN_140004600(lVar3);
      uVar6 = FUN_140005380(param_2,0);
      lVar3 = FUN_140006ef0(uVar6);
      *param_1 = lVar3 * uVar9;
      param_1[1] = lVar3;
      param_1[2] = CONCAT44(uStack_11c,local_120);
      param_1[3] = CONCAT44(uStack_114,uStack_118);
      param_1[4] = local_110;
      param_1[5] = lStack_108;
      param_1[6] = local_100;
      param_1[7] = lStack_f8;
      param_1[8] = local_f0;
      param_1[9] = lStack_e8;
      param_1[10] = local_e0;
      param_1[0xb] = lStack_d8;
      param_1[0xc] = local_d0;
      param_1[0xd] = lStack_c8;
      return param_1;
    }
    uVar32 = 0;
    uVar33 = 0;
    uVar34 = 0;
    uVar35 = 0;
    uVar28 = 0;
    uVar29 = 0;
    uVar30 = 0;
    uVar31 = 0;
    uVar24 = 0;
    uVar25 = 0;
    uVar26 = 0;
    uVar27 = 0;
    uVar20 = 0;
    uVar21 = 0;
    uVar22 = 0;
    uVar23 = 0;
    uVar16 = 0;
    uVar17 = 0;
    uVar18 = 0;
    uVar19 = 0;
    uVar12 = 0;
    uVar13 = 0;
    uVar14 = 0;
    uVar15 = 0;
  }
  *(undefined4 *)param_1 = 0;
  *(undefined4 *)((longlong)param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 1) = 0;
  *(undefined4 *)((longlong)param_1 + 0xc) = 0;
  *(undefined4 *)(param_1 + 2) = uVar12;
  *(undefined4 *)((longlong)param_1 + 0x14) = uVar13;
  *(undefined4 *)(param_1 + 3) = uVar14;
  *(undefined4 *)((longlong)param_1 + 0x1c) = uVar15;
  *(undefined4 *)(param_1 + 4) = uVar16;
  *(undefined4 *)((longlong)param_1 + 0x24) = uVar17;
  *(undefined4 *)(param_1 + 5) = uVar18;
  *(undefined4 *)((longlong)param_1 + 0x2c) = uVar19;
  *(undefined4 *)(param_1 + 6) = uVar20;
  *(undefined4 *)((longlong)param_1 + 0x34) = uVar21;
  *(undefined4 *)(param_1 + 7) = uVar22;
  *(undefined4 *)((longlong)param_1 + 0x3c) = uVar23;
  *(undefined4 *)(param_1 + 8) = uVar24;
  *(undefined4 *)((longlong)param_1 + 0x44) = uVar25;
  *(undefined4 *)(param_1 + 9) = uVar26;
  *(undefined4 *)((longlong)param_1 + 0x4c) = uVar27;
  *(undefined4 *)(param_1 + 10) = uVar28;
  *(undefined4 *)((longlong)param_1 + 0x54) = uVar29;
  *(undefined4 *)(param_1 + 0xb) = uVar30;
  *(undefined4 *)((longlong)param_1 + 0x5c) = uVar31;
  *(undefined4 *)(param_1 + 0xc) = uVar32;
  *(undefined4 *)((longlong)param_1 + 100) = uVar33;
  *(undefined4 *)(param_1 + 0xd) = uVar34;
  *(undefined4 *)((longlong)param_1 + 0x6c) = uVar35;
  return param_1;
}


/* ================= FUN_14000bb50 @ 14000bb50  size=705  callers=1 callees=5 ================= */
/* CALLERS: FUN_14000be20 */
/* CALLEES: FUN_140003c80, FUN_140004890, FUN_14000a7d0, FUN_14000d240, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8
FUN_14000bb50(undefined8 param_1,undefined8 param_2,undefined8 param_3,ulonglong param_4,
             longlong param_5,undefined8 *param_6)

{
  uint uVar1;
  longlong lVar2;
  int iVar3;
  ulonglong uVar4;
  undefined8 uVar5;
  uint uVar6;
  char *pcVar7;
  ushort uVar8;
  undefined1 auStack_1c8 [32];
  char *local_1a8;
  undefined8 *local_1a0;
  ushort local_198;
  undefined8 local_190;
  undefined8 local_188;
  undefined8 local_178;
  undefined8 uStack_170;
  undefined8 local_168;
  undefined8 uStack_160;
  undefined8 local_158;
  undefined8 uStack_150;
  undefined8 local_148;
  undefined8 uStack_140;
  undefined8 local_138;
  undefined8 uStack_130;
  uint local_118;
  undefined4 uStack_114;
  undefined4 uStack_110;
  undefined4 uStack_10c;
  undefined8 local_108;
  undefined8 auStack_100 [8];
  undefined8 uStack_c0;
  undefined8 local_b8;
  undefined8 uStack_b0;
  undefined8 local_a8;
  undefined8 uStack_a0;
  undefined8 local_98;
  undefined8 uStack_90;
  undefined8 local_88;
  undefined8 uStack_80;
  undefined4 local_78;
  undefined4 uStack_74;
  undefined4 uStack_70;
  undefined4 uStack_6c;
  undefined8 local_68;
  ulonglong local_58;
  
  local_58 = DAT_140096fe8 ^ (ulonglong)auStack_1c8;
  uVar1 = *(uint *)(param_5 + 0x10);
  uStack_114 = *(undefined4 *)(param_5 + 0x14);
  uStack_110 = *(undefined4 *)(param_5 + 0x18);
  uStack_10c = *(undefined4 *)(param_5 + 0x1c);
  lVar2 = *(longlong *)(param_5 + 0x18);
  uVar8 = 0;
  local_108 = 0;
  auStack_100[0] = 0;
  local_198 = (ushort)param_4;
  auStack_100[1] = 0;
  auStack_100[2] = 0;
  auStack_100[3] = 0;
  auStack_100[4] = 0;
  auStack_100[5] = 0;
  auStack_100[6] = 0;
  auStack_100[7] = 0;
  uStack_c0 = 0;
  local_b8 = 0;
  uStack_b0 = 0;
  local_a8 = 0;
  uStack_a0 = 0;
  local_98 = 0;
  uStack_90 = 0;
  local_88 = 0;
  uStack_80 = 0;
  local_78 = 0;
  uStack_74 = 0;
  uStack_70 = 0;
  uStack_6c = 0;
  local_68 = 0;
  local_108._0_4_ = 0;
  uVar6 = (uint)local_108;
  local_190 = param_2;
  local_188 = param_3;
  local_118 = uVar1;
  if ((ushort)uVar1 == 0) {
LAB_14000bd29:
    if (uVar6 == 0) {
      uVar5 = 0xffffffff;
    }
    else {
      FUN_140004890(&local_108);
      FUN_140003c80((ulonglong)(uVar1 & 0xffff) * 0x20 + -0x20 + lVar2,&local_108);
      if (param_6 != (undefined8 *)0x0) {
        *param_6 = local_108;
        param_6[1] = auStack_100[0];
        param_6[2] = auStack_100[1];
        param_6[3] = auStack_100[2];
        param_6[4] = auStack_100[3];
        param_6[5] = auStack_100[4];
        param_6[6] = auStack_100[5];
        param_6[7] = auStack_100[6];
        param_6[8] = auStack_100[7];
        param_6[9] = uStack_c0;
        param_6[10] = local_b8;
        param_6[0xb] = uStack_b0;
        param_6[0xc] = local_a8;
        param_6[0xd] = uStack_a0;
        param_6[0xe] = local_98;
        param_6[0xf] = uStack_90;
        param_6[0x10] = local_88;
        param_6[0x11] = uStack_80;
        param_6[0x12] = CONCAT44(uStack_74,local_78);
        param_6[0x13] = CONCAT44(uStack_6c,uStack_70);
        param_6[0x14] = local_68;
      }
      uVar5 = 0;
    }
    return uVar5;
  }
  do {
    local_1a0 = &local_178;
    pcVar7 = (char *)((ulonglong)uVar8 * 0x20 + lVar2);
    local_178 = 0;
    uStack_170 = 0;
    local_168 = 0;
    uStack_160 = 0;
    local_158 = 0;
    uStack_150 = 0;
    local_148 = 0;
    uStack_140 = 0;
    local_138 = 0;
    uStack_130 = 0;
    local_1a8 = pcVar7;
    iVar3 = FUN_14000d240(param_1,local_190,local_188,param_4);
    if (iVar3 == 0) {
      if (1 < uVar6) {
        FUN_14000a7d0(0x30);
        goto LAB_14000bd29;
      }
      uVar4 = (ulonglong)uVar6;
      auStack_100[uVar4 * 10] = local_178;
      auStack_100[uVar4 * 10 + 1] = uStack_170;
      auStack_100[uVar4 * 10 + 2] = local_168;
      auStack_100[uVar4 * 10 + 3] = uStack_160;
      auStack_100[uVar4 * 10 + 4] = local_158;
      auStack_100[uVar4 * 10 + 5] = uStack_150;
      auStack_100[uVar4 * 10 + 6] = local_148;
      auStack_100[uVar4 * 10 + 7] = uStack_140;
      *(undefined4 *)(&uStack_c0 + uVar4 * 10) = (undefined4)local_138;
      *(undefined4 *)((longlong)&uStack_c0 + uVar4 * 0x50 + 4) = local_138._4_4_;
      *(undefined4 *)(&local_b8 + uVar4 * 10) = (undefined4)uStack_130;
      *(undefined4 *)((longlong)&local_b8 + uVar4 * 0x50 + 4) = uStack_130._4_4_;
      uVar6 = (uint)local_108 + 1;
      local_108 = CONCAT44(local_108._4_4_,uVar6);
    }
    else if (*pcVar7 == -0x6d) {
      if (uVar6 != 0) {
        auStack_100[(ulonglong)(uVar6 - 1) * 10 + 2] = *(undefined8 *)(pcVar7 + 8);
        uVar6 = (uint)local_108;
      }
    }
    else {
      uVar5 = 0x31;
      if (*pcVar7 != -99) {
        uVar5 = 0x32;
      }
      FUN_14000a7d0(uVar5);
    }
    uVar8 = uVar8 + 1;
    if ((ushort)uVar1 <= uVar8) goto LAB_14000bd29;
    param_4 = (ulonglong)local_198;
  } while( true );
}


/* ================= FUN_14000be20 @ 14000be20  size=349  callers=2 callees=2 ================= */
/* CALLERS: FUN_140006210, FUN_14000d240 */
/* CALLEES: FUN_14000bb50, FUN_14000df70 */

undefined8
FUN_14000be20(longlong param_1,undefined8 param_2,longlong param_3,longlong param_4,
             undefined8 param_5,undefined8 param_6,undefined2 param_7,undefined8 *param_8)

{
  undefined4 *puVar1;
  char cVar2;
  undefined8 uVar3;
  longlong lVar4;
  undefined4 extraout_XMM0_Da;
  undefined4 extraout_XMM0_Db;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 local_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  
  if ((((param_8 != (undefined8 *)0x0) && (param_4 != 0)) && (0 < param_3)) && (param_1 != 0)) {
    lVar4 = 0;
    *param_8 = 0;
    param_8[1] = 0;
    param_8[2] = 0;
    param_8[3] = 0;
    param_8[4] = 0;
    param_8[5] = 0;
    param_8[6] = 0;
    param_8[7] = 0;
    param_8[8] = 0;
    param_8[9] = 0;
    param_8[10] = 0;
    param_8[0xb] = 0;
    param_8[0xc] = 0;
    param_8[0xd] = 0;
    param_8[0xe] = 0;
    param_8[0xf] = 0;
    param_8[0x10] = 0;
    param_8[0x11] = 0;
    param_8[0x12] = 0;
    param_8[0x13] = 0;
    param_8[0x14] = 0;
    do {
      puVar1 = *(undefined4 **)(param_4 + lVar4 * 8);
      if (puVar1 != (undefined4 *)0x0) {
        local_28 = *puVar1;
        uStack_24 = puVar1[1];
        uStack_20 = puVar1[2];
        uStack_1c = puVar1[3];
        uVar5 = puVar1[4];
        uVar6 = puVar1[5];
        uVar7 = puVar1[6];
        uVar8 = puVar1[7];
        local_18 = uVar5;
        uStack_14 = uVar6;
        uStack_10 = uVar7;
        uStack_c = uVar8;
        cVar2 = FUN_14000df70(&local_28);
        if (cVar2 == '\x01') {
          if (param_3 <= lVar4) {
            return 0xffffffff;
          }
          local_28 = extraout_XMM0_Da;
          uStack_24 = extraout_XMM0_Db;
          local_18 = uVar5;
          uStack_14 = uVar6;
          uStack_10 = uVar7;
          uStack_c = uVar8;
          uVar3 = FUN_14000bb50(param_1,param_2,param_5,param_7,&local_28,param_8);
          return uVar3;
        }
      }
      lVar4 = lVar4 + 1;
    } while (lVar4 < param_3);
  }
  return 0xffffffff;
}


/* ================= FUN_14000bf80 @ 14000bf80  size=362  callers=1 callees=12 ================= */
/* CALLERS: FUN_14000a870 */
/* CALLEES: FUN_140007860, FUN_140008400, FUN_140008460, FUN_1400084c0, FUN_140008620, FUN_140008650, FUN_14000a2f0, FUN_14000a770, FUN_14000c0f0, FUN_14000c1c0, FUN_14000d7a0, FUN_140019ae0 */

undefined8 FUN_14000bf80(longlong param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  ulonglong uVar5;
  ulonglong uVar6;
  longlong lVar7;
  undefined8 uVar8;
  uint uVar9;
  
  lVar7 = param_1;
  cVar1 = FUN_14000a2f0();
  if (cVar1 != '\0') {
    iVar3 = 0;
    uVar9 = 0;
    *(undefined4 *)(lVar7 + 4) = 0;
    iVar2 = FUN_140008400();
    if (iVar2 != 0) {
      do {
        iVar3 = FUN_140008650(param_1,uVar9);
        if (((iVar3 == 2) || (iVar3 == 0xb)) && (uVar5 = FUN_1400084c0(param_1,uVar9), uVar5 != 0))
        {
          uVar6 = FUN_140008620(param_1);
          *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + (int)(uVar6 / uVar5);
        }
        uVar9 = uVar9 + 1;
        uVar4 = FUN_140008400(param_1);
      } while (uVar9 < uVar4);
      iVar3 = *(int *)(param_1 + 4);
    }
    lVar7 = FUN_140019ae0(iVar3 * 0x18);
    *(longlong *)(param_1 + 0x58) = lVar7;
    if (lVar7 == 0) {
      FUN_14000a770(0x48);
      uVar8 = 0xffffffff;
    }
    else {
      uVar9 = 0;
      iVar3 = FUN_140008400(param_1);
      if (iVar3 != 0) {
        do {
          iVar3 = FUN_140008650(param_1,uVar9);
          if ((iVar3 == 2) || (iVar3 == 0xb)) {
            lVar7 = FUN_1400084c0(param_1,uVar9);
            if (lVar7 != 0) {
              FUN_140008620(param_1,uVar9);
            }
            FUN_140008460(param_1,uVar9);
            iVar3 = FUN_140007860(param_1);
            if (iVar3 == 2) {
              FUN_14000c1c0();
            }
            else {
              FUN_14000c0f0();
            }
          }
          uVar9 = uVar9 + 1;
          uVar4 = FUN_140008400(param_1);
        } while (uVar9 < uVar4);
      }
      uVar8 = FUN_14000d7a0(param_1);
    }
    return uVar8;
  }
  return 0xffffffff;
}


/* ================= FUN_14000c0f0 @ 14000c0f0  size=202  callers=1 callees=3 ================= */
/* CALLERS: FUN_14000bf80 */
/* CALLEES: FUN_140009ce0, FUN_14000df20, FUN_14000df30 */

undefined8 FUN_14000c0f0(undefined8 param_1,undefined4 *param_2,ulonglong param_3,longlong param_4)

{
  uint uVar1;
  undefined4 uVar2;
  char cVar3;
  undefined4 uVar4;
  uint uVar5;
  ulonglong uVar6;
  undefined1 *puVar7;
  ulonglong uVar8;
  undefined1 uStack_c;
  undefined1 uStack_b;
  undefined2 uStack_a;
  
  cVar3 = FUN_140009ce0();
  if ((int)param_3 != 0) {
    puVar7 = (undefined1 *)(param_4 + 5);
    param_3 = param_3 & 0xffffffff;
    do {
      uVar4 = *param_2;
      uVar5 = param_2[1];
      uVar1 = param_2[2];
      uVar2 = param_2[3];
      uStack_a = (undefined2)((uint)uVar2 >> 0x10);
      if (cVar3 == '\0') {
        uVar8 = (ulonglong)uVar1;
        uVar6 = (ulonglong)uVar5;
      }
      else {
        uVar4 = FUN_14000df30(uVar4);
        uVar5 = FUN_14000df30(uVar5);
        uVar6 = (ulonglong)uVar5;
        uVar5 = FUN_14000df30(uVar1);
        uVar8 = (ulonglong)uVar5;
        uStack_a = FUN_14000df20(uStack_a);
      }
      uStack_c = (undefined1)uVar2;
      param_2 = param_2 + 4;
      puVar7[-1] = uStack_c;
      uStack_b = (undefined1)((uint)uVar2 >> 8);
      *puVar7 = uStack_b;
      *(ulonglong *)(puVar7 + 0xb) = uVar8 & 0xffffffff;
      *(ulonglong *)(puVar7 + 3) = uVar6 & 0xffffffff;
      *(undefined4 *)(puVar7 + -5) = uVar4;
      *(undefined2 *)(puVar7 + 1) = uStack_a;
      puVar7 = puVar7 + 0x18;
      param_3 = param_3 - 1;
    } while (param_3 != 0);
  }
  return 0;
}


/* ================= FUN_14000c1c0 @ 14000c1c0  size=242  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000bf80 */
/* CALLEES: FUN_140009ce0, FUN_14000df20, FUN_14000df30, FUN_14000df40 */

undefined8 FUN_14000c1c0(undefined8 param_1,longlong param_2,ulonglong param_3,longlong param_4)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  char cVar4;
  undefined4 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined1 *puVar8;
  undefined1 uStack_34;
  undefined1 uStack_33;
  undefined2 uStack_32;
  
  cVar4 = FUN_140009ce0();
  if ((int)param_3 != 0) {
    puVar8 = (undefined1 *)(param_4 + 5);
    param_3 = param_3 & 0xffffffff;
    do {
      uVar7 = *(undefined8 *)(puVar8 + (param_2 - param_4) + 0xb);
      puVar1 = (undefined4 *)(puVar8 + (param_2 - param_4) + -5);
      uVar5 = *puVar1;
      uVar2 = puVar1[1];
      uVar6 = *(undefined8 *)(puVar1 + 2);
      uVar3 = *(undefined8 *)(puVar1 + 2);
      uStack_32 = (undefined2)((uint)uVar2 >> 0x10);
      if (cVar4 != '\0') {
        uVar5 = FUN_14000df30(uVar5);
        uVar6 = FUN_14000df40(uVar3);
        uVar7 = FUN_14000df40(uVar7);
        uStack_32 = FUN_14000df20(uStack_32);
      }
      uStack_34 = (undefined1)uVar2;
      puVar8[-1] = uStack_34;
      uStack_33 = (undefined1)((uint)uVar2 >> 8);
      *puVar8 = uStack_33;
      *(undefined4 *)(puVar8 + -5) = uVar5;
      *(undefined2 *)(puVar8 + 1) = uStack_32;
      *(undefined8 *)(puVar8 + 0xb) = uVar7;
      *(undefined8 *)(puVar8 + 3) = uVar6;
      puVar8 = puVar8 + 0x18;
      param_3 = param_3 - 1;
    } while (param_3 != 0);
  }
  return 0;
}


/* ================= FUN_14000c2c0 @ 14000c2c0  size=119  callers=1 callees=7 ================= */
/* CALLERS: FUN_14000a870 */
/* CALLEES: FUN_14000a2f0, FUN_14000a770, FUN_14000a7d0, FUN_14000c340, FUN_14000c520, FUN_14000dfc0, FUN_140019ae0 */

undefined8 FUN_14000c2c0(longlong param_1)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined8 uVar4;
  
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    FUN_14000a7d0(0x2b);
  }
  else {
    lVar3 = FUN_140019ae0(0x40);
    *(longlong *)(param_1 + 0x40) = lVar3;
    if (lVar3 == 0) {
      FUN_14000a770(0x48);
      return 0xffffffff;
    }
    if (*(char *)(*(longlong *)(param_1 + 0x38) + 4) == '\x02') {
      iVar2 = FUN_14000c520();
    }
    else {
      iVar2 = FUN_14000c340(param_1);
    }
    if (-1 < iVar2) {
      uVar4 = FUN_14000dfc0(param_1);
      return uVar4;
    }
  }
  return 0xffffffff;
}


/* ================= FUN_14000c340 @ 14000c340  size=473  callers=1 callees=3 ================= */
/* CALLERS: FUN_14000c2c0 */
/* CALLEES: FUN_140009ce0, FUN_14000df20, FUN_14000df30 */

undefined8 FUN_14000c340(longlong param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  ulonglong *puVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  ulonglong uVar12;
  ulonglong uVar13;
  ulonglong uVar14;
  char cVar15;
  undefined2 uVar16;
  undefined4 uVar17;
  uint uVar18;
  uint uVar19;
  longlong lVar20;
  ulonglong uVar21;
  undefined2 local_38;
  undefined2 uStack_20;
  undefined2 uStack_1e;
  undefined2 uStack_1c;
  undefined2 uStack_1a;
  undefined2 local_18;
  undefined2 uStack_16;
  
  puVar1 = *(undefined4 **)(param_1 + 0x38);
  uVar3 = (undefined4)*(ulonglong *)(puVar1 + 4);
  uVar4 = puVar1[5];
  uVar21 = *(ulonglong *)(puVar1 + 4);
  puVar5 = (ulonglong *)(puVar1 + 6);
  uVar14 = *puVar5;
  uVar13 = *puVar5;
  uVar12 = *puVar5;
  uVar19 = puVar1[8];
  uVar6 = puVar1[9];
  uVar7 = puVar1[10];
  uVar8 = puVar1[0xb];
  uVar9 = puVar1[1];
  uVar10 = puVar1[2];
  uVar11 = puVar1[3];
  uVar17 = puVar1[0xc];
  puVar2 = *(undefined4 **)(param_1 + 0x40);
  local_38 = (undefined2)uVar3;
  *puVar2 = *puVar1;
  puVar2[1] = uVar9;
  puVar2[2] = uVar10;
  puVar2[3] = uVar11;
  lVar20 = *(longlong *)(param_1 + 0x40);
  cVar15 = FUN_140009ce0();
  uStack_20 = (undefined2)uVar7;
  uStack_1e = (undefined2)((uint)uVar7 >> 0x10);
  uStack_1c = (undefined2)uVar8;
  uStack_1a = (undefined2)((uint)uVar8 >> 0x10);
  local_18 = (undefined2)uVar17;
  uStack_16 = (undefined2)((uint)uVar17 >> 0x10);
  if (cVar15 != '\0') {
    uVar16 = FUN_14000df20(local_38);
    *(undefined2 *)(lVar20 + 0x10) = uVar16;
    uVar16 = FUN_14000df20(uVar21 >> 0x10);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x12) = uVar16;
    uVar17 = FUN_14000df30(uVar21 >> 0x20);
    *(undefined4 *)(*(longlong *)(param_1 + 0x40) + 0x14) = uVar17;
    uVar18 = FUN_14000df30((uint)uVar14);
    *(ulonglong *)(*(longlong *)(param_1 + 0x40) + 0x18) = (ulonglong)uVar18;
    uVar18 = FUN_14000df30(uVar12 >> 0x20);
    *(ulonglong *)(*(longlong *)(param_1 + 0x40) + 0x20) = (ulonglong)uVar18;
    uVar19 = FUN_14000df30(uVar19);
    *(ulonglong *)(*(longlong *)(param_1 + 0x40) + 0x28) = (ulonglong)uVar19;
    uVar17 = FUN_14000df30(uVar6);
    *(undefined4 *)(*(longlong *)(param_1 + 0x40) + 0x30) = uVar17;
    uVar16 = FUN_14000df20(uStack_20);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x34) = uVar16;
    uVar16 = FUN_14000df20(uStack_1e);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x36) = uVar16;
    uVar16 = FUN_14000df20(uStack_1c);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x38) = uVar16;
    uVar16 = FUN_14000df20(uStack_1a);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3a) = uVar16;
    uVar16 = FUN_14000df20(local_18);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3c) = uVar16;
    uVar16 = FUN_14000df20(uStack_16);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3e) = uVar16;
    return 0;
  }
  *(undefined2 *)(lVar20 + 0x10) = local_38;
  *(short *)(*(longlong *)(param_1 + 0x40) + 0x12) = (short)((uint)uVar3 >> 0x10);
  *(undefined4 *)(*(longlong *)(param_1 + 0x40) + 0x14) = uVar4;
  *(ulonglong *)(*(longlong *)(param_1 + 0x40) + 0x18) = (ulonglong)(uint)uVar14;
  *(ulonglong *)(*(longlong *)(param_1 + 0x40) + 0x20) = uVar13 >> 0x20;
  *(ulonglong *)(*(longlong *)(param_1 + 0x40) + 0x28) = (ulonglong)uVar19;
  *(undefined4 *)(*(longlong *)(param_1 + 0x40) + 0x30) = uVar6;
  *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x34) = uStack_20;
  *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x36) = uStack_1e;
  *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x38) = uStack_1c;
  *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3a) = uStack_1a;
  *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3c) = local_18;
  *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3e) = uStack_16;
  return 0;
}


/* ================= FUN_14000c520 @ 14000c520  size=459  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000c2c0 */
/* CALLEES: FUN_140009ce0, FUN_14000df20, FUN_14000df30, FUN_14000df40 */

undefined8 FUN_14000c520(longlong param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  char cVar13;
  undefined2 uVar14;
  undefined4 uVar15;
  undefined8 uVar16;
  ulonglong uVar17;
  longlong lVar18;
  undefined2 uStack_14;
  undefined2 uStack_12;
  undefined2 uStack_10;
  undefined2 uStack_e;
  undefined2 uStack_c;
  undefined2 uStack_a;
  
  puVar1 = *(undefined4 **)(param_1 + 0x38);
  uVar17 = *(ulonglong *)(puVar1 + 4);
  uVar16 = *(undefined8 *)(puVar1 + 6);
  uVar11 = *(undefined8 *)(puVar1 + 8);
  uVar9 = *(undefined8 *)(puVar1 + 8);
  uVar12 = *(undefined8 *)(puVar1 + 10);
  uVar10 = *(undefined8 *)(puVar1 + 10);
  uVar15 = puVar1[1];
  uVar3 = puVar1[2];
  uVar4 = puVar1[3];
  uVar5 = puVar1[0xc];
  uVar6 = puVar1[0xd];
  uVar7 = puVar1[0xe];
  uVar8 = puVar1[0xf];
  puVar2 = *(undefined4 **)(param_1 + 0x40);
  *puVar2 = *puVar1;
  puVar2[1] = uVar15;
  puVar2[2] = uVar3;
  puVar2[3] = uVar4;
  lVar18 = *(longlong *)(param_1 + 0x40);
  cVar13 = FUN_140009ce0();
  uStack_14 = (undefined2)uVar6;
  uStack_12 = (undefined2)((uint)uVar6 >> 0x10);
  uStack_10 = (undefined2)uVar7;
  uStack_e = (undefined2)((uint)uVar7 >> 0x10);
  uStack_c = (undefined2)uVar8;
  uStack_a = (undefined2)((uint)uVar8 >> 0x10);
  if (cVar13 == '\0') {
    *(short *)(lVar18 + 0x10) = (short)uVar17;
    *(short *)(*(longlong *)(param_1 + 0x40) + 0x12) = (short)(uVar17 >> 0x10);
    *(int *)(*(longlong *)(param_1 + 0x40) + 0x14) = (int)(uVar17 >> 0x20);
    *(undefined8 *)(*(longlong *)(param_1 + 0x40) + 0x18) = uVar16;
    *(undefined8 *)(*(longlong *)(param_1 + 0x40) + 0x20) = uVar11;
    *(undefined8 *)(*(longlong *)(param_1 + 0x40) + 0x28) = uVar12;
    *(undefined4 *)(*(longlong *)(param_1 + 0x40) + 0x30) = uVar5;
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x34) = uStack_14;
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x36) = uStack_12;
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x38) = uStack_10;
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3a) = uStack_e;
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3c) = uStack_c;
  }
  else {
    uVar14 = FUN_14000df20(uVar17 & 0xffff);
    *(undefined2 *)(lVar18 + 0x10) = uVar14;
    uVar14 = FUN_14000df20(uVar17 >> 0x10);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x12) = uVar14;
    uVar15 = FUN_14000df30(uVar17 >> 0x20);
    *(undefined4 *)(*(longlong *)(param_1 + 0x40) + 0x14) = uVar15;
    uVar16 = FUN_14000df40(uVar16);
    *(undefined8 *)(*(longlong *)(param_1 + 0x40) + 0x18) = uVar16;
    uVar16 = FUN_14000df40(uVar9);
    *(undefined8 *)(*(longlong *)(param_1 + 0x40) + 0x20) = uVar16;
    uVar16 = FUN_14000df40(uVar10);
    *(undefined8 *)(*(longlong *)(param_1 + 0x40) + 0x28) = uVar16;
    uVar15 = FUN_14000df30(uVar5);
    *(undefined4 *)(*(longlong *)(param_1 + 0x40) + 0x30) = uVar15;
    uVar14 = FUN_14000df20(uStack_14);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x34) = uVar14;
    uVar14 = FUN_14000df20(uStack_12);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x36) = uVar14;
    uVar14 = FUN_14000df20(uStack_10);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x38) = uVar14;
    uVar14 = FUN_14000df20(uStack_e);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3a) = uVar14;
    uVar14 = FUN_14000df20(uStack_c);
    *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3c) = uVar14;
    uStack_a = FUN_14000df20(uStack_a);
  }
  *(undefined2 *)(*(longlong *)(param_1 + 0x40) + 0x3e) = uStack_a;
  return 0;
}


/* ================= FUN_14000c6f0 @ 14000c6f0  size=237  callers=1 callees=3 ================= */
/* CALLERS: FUN_14000aaf0 */
/* CALLEES: FUN_140001ef0, FUN_140003c10, FUN_140019550 */

undefined4 FUN_14000c6f0(longlong param_1)

{
  int iVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  longlong lVar4;
  longlong lVar5;
  longlong local_res8;
  longlong local_res10;
  longlong local_res18;
  
  if (param_1 == 0) {
    return 0xffffffff;
  }
  local_res8 = 0;
  iVar1 = FUN_140019550(*(undefined8 *)(param_1 + 0x78),0,0,&local_res8,&local_res10,0);
  uVar3 = 0;
  lVar4 = local_res8;
  while (iVar1 == 0) {
    uVar2 = 0;
    local_res8 = lVar4;
    if (0 < lVar4) {
      do {
        lVar5 = local_res10 + uVar2 * 0x20;
        iVar1 = FUN_140003c10(*(undefined1 *)(lVar5 + 4));
        if (iVar1 == 0) {
          local_res18 = lVar5;
          FUN_140001ef0(param_1 + 0x420,&local_res18);
          lVar4 = local_res8;
        }
        uVar2 = (ulonglong)((int)uVar2 + 1);
      } while ((longlong)uVar2 < lVar4);
    }
    uVar3 = (ulonglong)((int)uVar3 + 1);
    iVar1 = FUN_140019550(*(undefined8 *)(param_1 + 0x78),uVar3,0,&local_res8,&local_res10,0);
    lVar4 = local_res8;
  }
  return 0;
}


/* ================= FUN_14000c7e0 @ 14000c7e0  size=217  callers=1 callees=3 ================= */
/* CALLERS: FUN_14000aaf0 */
/* CALLEES: FUN_1400079e0, FUN_1400079f0, FUN_1400090c0 */

longlong FUN_14000c7e0(undefined8 param_1)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  uint uVar7;
  
  uVar7 = 0;
  iVar2 = FUN_1400079f0();
  if (iVar2 != 0) {
    do {
      lVar4 = FUN_1400079e0(param_1,uVar7);
      cVar1 = FUN_1400090c0(lVar4,0x6a);
      if (cVar1 != '\0') {
        return lVar4;
      }
      uVar7 = uVar7 + 1;
      uVar3 = FUN_1400079f0(param_1);
    } while (uVar7 < uVar3);
  }
  uVar7 = 0;
  iVar2 = FUN_1400079f0(param_1);
  if (iVar2 != 0) {
    do {
      lVar4 = FUN_1400079e0(param_1,uVar7);
      if ((lVar4 != 0) && (*(longlong *)(lVar4 + 0x58) != 0)) {
        lVar5 = 0;
        while (lVar6 = lVar5 + 1,
              *(char *)(*(longlong *)(lVar4 + 0x58) + lVar5) == (&DAT_14005f5b4)[lVar5]) {
          lVar5 = lVar6;
          if (lVar6 == 5) {
            return lVar4;
          }
        }
      }
      uVar7 = uVar7 + 1;
      uVar3 = FUN_1400079f0(param_1);
    } while (uVar7 < uVar3);
  }
  return 0;
}


/* ================= FUN_14000c8c0 @ 14000c8c0  size=366  callers=1 callees=1 ================= */
/* CALLERS: FUN_140004fe0 */
/* CALLEES: FUN_14000a7d0 */

undefined8 *
FUN_14000c8c0(undefined8 *param_1,char param_2,char param_3,ulonglong param_4,longlong param_5,
             undefined8 param_6)

{
  ulonglong uVar1;
  
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  param_1[0xf] = 0;
  *(undefined4 *)(param_1 + 0x10) = 0;
  *(undefined4 *)((longlong)param_1 + 0x84) = 0;
  *(undefined4 *)(param_1 + 0x11) = 0;
  *(undefined4 *)((longlong)param_1 + 0x8c) = 0;
  *(undefined4 *)(param_1 + 0x12) = 0;
  *(undefined4 *)((longlong)param_1 + 0x94) = 0;
  *(undefined4 *)(param_1 + 0x13) = 0;
  *(undefined4 *)((longlong)param_1 + 0x9c) = 0;
  param_1[0x14] = 0;
  if (param_5 == 0x40b) {
    *(undefined4 *)param_1 = 1;
    *(int *)(param_1 + 4) = (int)param_4;
    *(undefined4 *)(param_1 + 1) = 4;
    return param_1;
  }
  if (param_2 == '\0') {
    if (param_3 == '\0') {
      *(undefined4 *)param_1 = 1;
      *(int *)(param_1 + 4) = (int)param_5;
      *(undefined4 *)(param_1 + 1) = 4;
      return param_1;
    }
    uVar1 = (ulonglong)DAT_140096000;
    *(undefined4 *)param_1 = 1;
    param_1[5] = param_6;
    if (param_4 == uVar1) {
      *(undefined4 *)(param_1 + 1) = 5;
      *(int *)(param_1 + 4) = (int)param_5;
      return param_1;
    }
    *(undefined4 *)(param_1 + 1) = 0xc;
    return param_1;
  }
  if (param_2 == '\x01') {
    param_1[5] = param_6;
    *(undefined4 *)param_1 = 1;
    *(undefined4 *)(param_1 + 1) = 9;
    return param_1;
  }
  if (param_2 != '\x02') {
    if (param_2 != '\x03') {
      FUN_14000a7d0(0x3d);
      return param_1;
    }
    FUN_14000a7d0(0x3c);
    return param_1;
  }
  FUN_14000a7d0(0x3b);
  return param_1;
}


/* ================= FUN_14000ca30 @ 14000ca30  size=291  callers=1 callees=9 ================= */
/* CALLERS: FUN_14000a870 */
/* CALLEES: FUN_140004c50, FUN_140007860, FUN_1400078a0, FUN_140008290, FUN_140008400, FUN_14000a770, FUN_14000cb60, FUN_14000ccf0, FUN_140019ae0 */

ulonglong FUN_14000ca30(longlong param_1)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  longlong lVar4;
  ulonglong uVar5;
  undefined8 uVar6;
  
  lVar4 = FUN_1400078a0();
  if (lVar4 == 0) {
    return 0xffffffff;
  }
  uVar5 = FUN_140008400();
  if ((int)uVar5 == 0) {
    return uVar5;
  }
  lVar4 = FUN_140019ae0((int)uVar5 << 6);
  *(longlong *)(param_1 + 0x48) = lVar4;
  if (lVar4 == 0) {
    FUN_14000a770(0x48);
    uVar5 = 0xffffffff;
  }
  else {
    iVar1 = FUN_140007860(param_1);
    if (iVar1 == 2) {
      uVar2 = FUN_14000ccf0();
    }
    else {
      uVar2 = FUN_14000cb60();
    }
    uVar5 = (ulonglong)uVar2;
    uVar6 = FUN_140008290(0xd);
    uVar3 = FUN_140004c50(param_1,uVar6);
    *(undefined4 *)(param_1 + 0x14) = uVar3;
    uVar6 = FUN_140008290(0xc);
    uVar3 = FUN_140004c50(param_1,uVar6);
    *(undefined4 *)(param_1 + 0x18) = uVar3;
    uVar6 = FUN_140008290(0xe);
    uVar3 = FUN_140004c50(param_1,uVar6);
    *(undefined4 *)(param_1 + 0x1c) = uVar3;
    uVar6 = FUN_140008290(0);
    uVar3 = FUN_140004c50(param_1,uVar6);
    *(undefined4 *)(param_1 + 0x20) = uVar3;
    uVar6 = FUN_140008290(1);
    uVar3 = FUN_140004c50(param_1,uVar6);
    *(undefined4 *)(param_1 + 0x24) = uVar3;
  }
  return uVar5;
}


/* ================= FUN_14000cb60 @ 14000cb60  size=395  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000ca30 */
/* CALLEES: FUN_140009ce0, FUN_14000df30, __security_check_cookie, memcpy */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14000cb60(longlong param_1,longlong param_2,uint param_3,uint param_4)

{
  longlong lVar1;
  char cVar2;
  undefined4 uVar3;
  uint uVar4;
  ulonglong uVar5;
  ulonglong uVar6;
  ulonglong uVar7;
  ulonglong uVar8;
  ulonglong uVar9;
  ulonglong uVar10;
  ulonglong uVar11;
  undefined1 auStack_88 [32];
  undefined4 local_68;
  undefined4 local_64;
  uint local_60;
  uint local_5c;
  uint local_58;
  uint local_54;
  undefined4 local_50;
  undefined4 local_4c;
  uint local_48;
  uint local_44;
  ulonglong local_40;
  
  local_40 = DAT_140096fe8 ^ (ulonglong)auStack_88;
  uVar6 = 0;
  uVar7 = uVar6;
  if (param_3 != 0) {
    do {
      lVar1 = *(longlong *)(param_1 + 0x48);
      memcpy(&local_68,
             (void *)((ulonglong)(param_4 * (int)uVar7) + *(longlong *)(param_1 + 0x38) + param_2),
             (ulonglong)param_4);
      cVar2 = FUN_140009ce0(param_1);
      if (cVar2 == '\0') {
        uVar5 = (ulonglong)local_44;
        uVar8 = (ulonglong)local_48;
        uVar9 = (ulonglong)local_54;
        uVar10 = (ulonglong)local_58;
        uVar11 = (ulonglong)local_5c;
        *(undefined4 *)(uVar6 + lVar1) = local_68;
        *(undefined4 *)(uVar6 + 4 + lVar1) = local_64;
        *(undefined4 *)(uVar6 + 0x28 + lVar1) = local_50;
        *(undefined4 *)(uVar6 + 0x2c + lVar1) = local_4c;
        uVar4 = local_60;
      }
      else {
        uVar3 = FUN_14000df30(local_68);
        *(undefined4 *)(uVar6 + lVar1) = uVar3;
        uVar3 = FUN_14000df30(local_64);
        *(undefined4 *)(uVar6 + 4 + lVar1) = uVar3;
        uVar3 = FUN_14000df30(local_50);
        *(undefined4 *)(uVar6 + 0x28 + lVar1) = uVar3;
        uVar3 = FUN_14000df30(local_4c);
        *(undefined4 *)(uVar6 + 0x2c + lVar1) = uVar3;
        uVar4 = FUN_14000df30(local_44);
        uVar5 = (ulonglong)uVar4;
        uVar4 = FUN_14000df30(local_48);
        uVar8 = (ulonglong)uVar4;
        uVar4 = FUN_14000df30(local_54);
        uVar9 = (ulonglong)uVar4;
        uVar4 = FUN_14000df30(local_58);
        uVar10 = (ulonglong)uVar4;
        uVar4 = FUN_14000df30(local_5c);
        uVar11 = (ulonglong)uVar4;
        uVar4 = FUN_14000df30(local_60);
      }
      *(ulonglong *)(uVar6 + 8 + lVar1) = (ulonglong)uVar4;
      uVar4 = (int)uVar7 + 1;
      *(ulonglong *)(uVar6 + 0x10 + lVar1) = uVar11;
      *(ulonglong *)(uVar6 + 0x18 + lVar1) = uVar10;
      *(ulonglong *)(uVar6 + 0x20 + lVar1) = uVar9;
      *(ulonglong *)(uVar6 + 0x30 + lVar1) = uVar8;
      *(ulonglong *)(uVar6 + 0x38 + lVar1) = uVar5;
      uVar6 = uVar6 + 0x40;
      uVar7 = (ulonglong)uVar4;
    } while (uVar4 < param_3);
  }
  return 0;
}


/* ================= FUN_14000ccf0 @ 14000ccf0  size=419  callers=1 callees=5 ================= */
/* CALLERS: FUN_14000ca30 */
/* CALLEES: FUN_140009ce0, FUN_14000df30, FUN_14000df40, __security_check_cookie, memcpy */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14000ccf0(longlong param_1,longlong param_2,uint param_3,uint param_4)

{
  longlong lVar1;
  char cVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  ulonglong uVar5;
  uint uVar6;
  ulonglong uVar7;
  undefined1 auStack_a8 [32];
  undefined4 local_88;
  undefined4 local_84;
  undefined8 local_80;
  undefined8 local_78;
  undefined8 local_70;
  undefined8 local_68;
  undefined4 local_60;
  undefined4 local_5c;
  undefined8 local_58;
  undefined8 local_50;
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_a8;
  uVar5 = 0;
  uVar7 = uVar5;
  if (param_3 != 0) {
    do {
      lVar1 = *(longlong *)(param_1 + 0x48);
      memcpy(&local_88,
             (void *)((ulonglong)(param_4 * (int)uVar7) + *(longlong *)(param_1 + 0x38) + param_2),
             (ulonglong)param_4);
      cVar2 = FUN_140009ce0(param_1);
      if (cVar2 == '\0') {
        *(undefined4 *)(uVar5 + lVar1) = local_88;
        *(undefined4 *)(uVar5 + 4 + lVar1) = local_84;
        *(undefined8 *)(uVar5 + 8 + lVar1) = local_80;
        *(undefined8 *)(uVar5 + 0x10 + lVar1) = local_78;
        *(undefined8 *)(uVar5 + 0x18 + lVar1) = local_70;
        *(undefined8 *)(uVar5 + 0x20 + lVar1) = local_68;
        *(undefined4 *)(uVar5 + 0x28 + lVar1) = local_60;
        *(undefined4 *)(uVar5 + 0x2c + lVar1) = local_5c;
        *(undefined8 *)(uVar5 + 0x30 + lVar1) = local_58;
        uVar4 = local_50;
      }
      else {
        uVar3 = FUN_14000df30(local_88);
        *(undefined4 *)(uVar5 + lVar1) = uVar3;
        uVar3 = FUN_14000df30(local_84);
        *(undefined4 *)(uVar5 + 4 + lVar1) = uVar3;
        uVar4 = FUN_14000df40(local_80);
        *(undefined8 *)(uVar5 + 8 + lVar1) = uVar4;
        uVar4 = FUN_14000df40(local_78);
        *(undefined8 *)(uVar5 + 0x10 + lVar1) = uVar4;
        uVar4 = FUN_14000df40(local_70);
        *(undefined8 *)(uVar5 + 0x18 + lVar1) = uVar4;
        uVar4 = FUN_14000df40(local_68);
        *(undefined8 *)(uVar5 + 0x20 + lVar1) = uVar4;
        uVar3 = FUN_14000df30(local_60);
        *(undefined4 *)(uVar5 + 0x28 + lVar1) = uVar3;
        uVar3 = FUN_14000df30(local_5c);
        *(undefined4 *)(uVar5 + 0x2c + lVar1) = uVar3;
        uVar4 = FUN_14000df40(local_58);
        *(undefined8 *)(uVar5 + 0x30 + lVar1) = uVar4;
        uVar4 = FUN_14000df40(local_50);
      }
      *(undefined8 *)(uVar5 + 0x38 + lVar1) = uVar4;
      uVar6 = (int)uVar7 + 1;
      uVar5 = uVar5 + 0x40;
      uVar7 = (ulonglong)uVar6;
    } while (uVar6 < param_3);
  }
  return 0;
}


/* ================= FUN_14000cea0 @ 14000cea0  size=163  callers=1 callees=7 ================= */
/* CALLERS: FUN_14000a870 */
/* CALLEES: FUN_140007860, FUN_1400078a0, FUN_140008670, FUN_14000a770, FUN_14000cf50, FUN_14000d0c0, FUN_140019ae0 */

undefined8 FUN_14000cea0(longlong param_1)

{
  int iVar1;
  longlong lVar2;
  undefined8 uVar3;
  
  lVar2 = FUN_1400078a0();
  if (lVar2 == 0) {
    return 0xffffffff;
  }
  uVar3 = FUN_140008670();
  if ((int)uVar3 == 0) {
    return uVar3;
  }
  lVar2 = FUN_140019ae0((uint)*(ushort *)(lVar2 + 0x38) * 0x38);
  *(longlong *)(param_1 + 0x50) = lVar2;
  if (lVar2 == 0) {
    FUN_14000a770(0x48);
    uVar3 = 0xffffffff;
  }
  else {
    iVar1 = FUN_140007860(param_1);
    if (iVar1 == 2) {
      uVar3 = FUN_14000d0c0();
    }
    else {
      uVar3 = FUN_14000cf50();
    }
  }
  return uVar3;
}


/* ================= FUN_14000cf50 @ 14000cf50  size=353  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000cea0 */
/* CALLEES: FUN_140009ce0, FUN_14000df30, __security_check_cookie, memcpy */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14000cf50(longlong param_1,longlong param_2,uint param_3,uint param_4)

{
  longlong lVar1;
  char cVar2;
  undefined4 uVar3;
  uint uVar4;
  ulonglong uVar5;
  ulonglong uVar6;
  ulonglong uVar7;
  ulonglong uVar8;
  ulonglong uVar9;
  ulonglong uVar10;
  ulonglong uVar11;
  undefined1 auStack_88 [32];
  undefined4 local_68;
  uint local_64;
  uint local_60;
  uint local_5c;
  uint local_58;
  uint local_54;
  undefined4 local_50;
  uint local_4c;
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_88;
  uVar6 = 0;
  uVar7 = uVar6;
  if (param_3 != 0) {
    do {
      lVar1 = *(longlong *)(param_1 + 0x50);
      memcpy(&local_68,
             (void *)((ulonglong)(param_4 * (int)uVar7) + *(longlong *)(param_1 + 0x38) + param_2),
             (ulonglong)param_4);
      cVar2 = FUN_140009ce0(param_1);
      if (cVar2 == '\0') {
        uVar5 = (ulonglong)local_4c;
        uVar8 = (ulonglong)local_54;
        uVar9 = (ulonglong)local_58;
        uVar10 = (ulonglong)local_5c;
        uVar11 = (ulonglong)local_60;
        *(undefined4 *)(uVar6 + lVar1) = local_68;
        *(undefined4 *)(uVar6 + 4 + lVar1) = local_50;
        uVar4 = local_64;
      }
      else {
        uVar3 = FUN_14000df30(local_68);
        *(undefined4 *)(uVar6 + lVar1) = uVar3;
        uVar3 = FUN_14000df30(local_50);
        *(undefined4 *)(uVar6 + 4 + lVar1) = uVar3;
        uVar4 = FUN_14000df30(local_4c);
        uVar5 = (ulonglong)uVar4;
        uVar4 = FUN_14000df30(local_54);
        uVar8 = (ulonglong)uVar4;
        uVar4 = FUN_14000df30(local_58);
        uVar9 = (ulonglong)uVar4;
        uVar4 = FUN_14000df30(local_5c);
        uVar10 = (ulonglong)uVar4;
        uVar4 = FUN_14000df30(local_60);
        uVar11 = (ulonglong)uVar4;
        uVar4 = FUN_14000df30(local_64);
      }
      *(ulonglong *)(uVar6 + 8 + lVar1) = (ulonglong)uVar4;
      uVar4 = (int)uVar7 + 1;
      *(ulonglong *)(uVar6 + 0x10 + lVar1) = uVar11;
      *(ulonglong *)(uVar6 + 0x18 + lVar1) = uVar10;
      *(ulonglong *)(uVar6 + 0x20 + lVar1) = uVar9;
      *(ulonglong *)(uVar6 + 0x28 + lVar1) = uVar8;
      *(ulonglong *)(uVar6 + 0x30 + lVar1) = uVar5;
      uVar6 = uVar6 + 0x38;
      uVar7 = (ulonglong)uVar4;
    } while (uVar4 < param_3);
  }
  return 0;
}


/* ================= FUN_14000d0c0 @ 14000d0c0  size=370  callers=1 callees=5 ================= */
/* CALLERS: FUN_14000cea0 */
/* CALLEES: FUN_140009ce0, FUN_14000df30, FUN_14000df40, __security_check_cookie, memcpy */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14000d0c0(longlong param_1,longlong param_2,uint param_3,uint param_4)

{
  longlong lVar1;
  char cVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  ulonglong uVar5;
  uint uVar6;
  ulonglong uVar7;
  undefined1 auStack_98 [32];
  undefined4 local_78;
  undefined4 local_74;
  undefined8 local_70;
  undefined8 local_68;
  undefined8 local_60;
  undefined8 local_58;
  undefined8 local_50;
  undefined8 local_48;
  ulonglong local_40;
  
  local_40 = DAT_140096fe8 ^ (ulonglong)auStack_98;
  uVar5 = 0;
  uVar7 = uVar5;
  if (param_3 != 0) {
    do {
      lVar1 = *(longlong *)(param_1 + 0x50);
      memcpy(&local_78,
             (void *)((ulonglong)(param_4 * (int)uVar7) + *(longlong *)(param_1 + 0x38) + param_2),
             (ulonglong)param_4);
      cVar2 = FUN_140009ce0(param_1);
      if (cVar2 == '\0') {
        *(undefined4 *)(uVar5 + lVar1) = local_78;
        *(undefined4 *)(uVar5 + 4 + lVar1) = local_74;
        *(undefined8 *)(uVar5 + 8 + lVar1) = local_70;
        *(undefined8 *)(uVar5 + 0x10 + lVar1) = local_68;
        *(undefined8 *)(uVar5 + 0x18 + lVar1) = local_60;
        *(undefined8 *)(uVar5 + 0x20 + lVar1) = local_58;
        *(undefined8 *)(uVar5 + 0x28 + lVar1) = local_50;
        uVar4 = local_48;
      }
      else {
        uVar3 = FUN_14000df30(local_78);
        *(undefined4 *)(uVar5 + lVar1) = uVar3;
        uVar3 = FUN_14000df30(local_74);
        *(undefined4 *)(uVar5 + 4 + lVar1) = uVar3;
        uVar4 = FUN_14000df40(local_70);
        *(undefined8 *)(uVar5 + 8 + lVar1) = uVar4;
        uVar4 = FUN_14000df40(local_68);
        *(undefined8 *)(uVar5 + 0x10 + lVar1) = uVar4;
        uVar4 = FUN_14000df40(local_60);
        *(undefined8 *)(uVar5 + 0x18 + lVar1) = uVar4;
        uVar4 = FUN_14000df40(local_58);
        *(undefined8 *)(uVar5 + 0x20 + lVar1) = uVar4;
        uVar4 = FUN_14000df40(local_50);
        *(undefined8 *)(uVar5 + 0x28 + lVar1) = uVar4;
        uVar4 = FUN_14000df40(local_48);
      }
      *(undefined8 *)(uVar5 + 0x30 + lVar1) = uVar4;
      uVar6 = (int)uVar7 + 1;
      uVar5 = uVar5 + 0x38;
      uVar7 = (ulonglong)uVar6;
    } while (uVar6 < param_3);
  }
  return 0;
}


/* ================= FUN_14000d240 @ 14000d240  size=1154  callers=1 callees=7 ================= */
/* CALLERS: FUN_14000bb50 */
/* CALLEES: FUN_140004fe0, FUN_14000a7d0, FUN_14000be20, FUN_1400191a0, __security_check_cookie, free, memcpy */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8
FUN_14000d240(longlong param_1,undefined8 param_2,undefined8 param_3,short param_4,byte *param_5,
             undefined8 *param_6)

{
  byte bVar1;
  void *pvVar2;
  int iVar3;
  longlong lVar4;
  ulonglong uVar5;
  undefined1 auStack_248 [32];
  void **local_228;
  undefined1 *local_220;
  undefined8 local_218;
  undefined1 *local_210;
  void *local_208;
  undefined1 local_200 [8];
  undefined1 local_1f8 [176];
  undefined8 local_148;
  undefined8 uStack_140;
  size_t local_138;
  ulonglong uStack_130;
  undefined8 local_128;
  undefined8 uStack_120;
  undefined8 local_118;
  undefined8 uStack_110;
  ulonglong local_108;
  undefined8 uStack_100;
  undefined1 local_f8 [8];
  undefined4 uStack_f0;
  undefined8 local_e8;
  ulonglong local_d8;
  undefined8 uStack_d0;
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_248;
  if ((param_6 == (undefined8 *)0x0) || (param_1 == 0)) {
    return 0xffffffff;
  }
  bVar1 = *param_5;
  local_148 = 0;
  uStack_140 = 0;
  local_138 = 0;
  uStack_130 = 0;
  local_128 = 0;
  uStack_120 = 0;
  local_118 = 0;
  uStack_110 = 0;
  local_108 = 0;
  uStack_100 = 0;
  if (bVar1 == 3) {
    uStack_140 = *(undefined8 *)(param_5 + 8);
  }
  else {
    if ((bVar1 != 0xa1) && (bVar1 != 0xfb)) {
      if (((byte)(bVar1 - 8) < 8) || ((byte)(bVar1 - 0x10) < 2)) {
        uStack_120 = *(undefined8 *)(param_5 + 8);
        local_148 = 1;
      }
      else if ((bVar1 == 0xa2) || (bVar1 == 0xfc)) {
        uStack_120 = *(undefined8 *)(param_5 + 0x10);
        local_148 = 1;
      }
      else if ((byte)(bVar1 + 0xb0) < 0x20) {
        local_148 = 4;
        uStack_130 = (ulonglong)(bVar1 - 0x50);
      }
      else if ((byte)(bVar1 + 0x90) < 0x20) {
        uStack_140 = 0;
        if (param_4 == 0x40) {
          local_148 = 5;
          uStack_130 = (ulonglong)(bVar1 - 0x70);
          local_128 = *(undefined8 *)(param_5 + 8);
        }
        else {
          local_108 = (ulonglong)bVar1 - 0x70;
          local_148 = 6;
          local_118 = 5;
          uStack_110 = 0;
          uStack_100 = *(undefined8 *)(param_5 + 8);
        }
      }
      else if ((bVar1 == 0x90) || (bVar1 == 0xa5)) {
        uStack_130 = (ulonglong)*(uint *)(param_5 + 8);
        local_148 = 4;
      }
      else if (bVar1 == 0x92) {
        uStack_140 = 0;
        if (param_4 == 0x40) {
          uStack_130 = (ulonglong)*(uint *)(param_5 + 8);
          local_128 = *(undefined8 *)(param_5 + 0x10);
          local_148 = 5;
        }
        else {
          uStack_100 = *(undefined8 *)(param_5 + 0x10);
          local_108 = *(ulonglong *)(param_5 + 8);
          local_148 = 6;
          local_118 = 5;
          uStack_110 = 0;
        }
      }
      else if (bVar1 == 0x91) {
        local_128 = *(undefined8 *)(param_5 + 8);
        local_148 = 0xb;
      }
      else if (bVar1 == 0x9c) {
        lVar4 = FUN_140004fe0(local_1f8,*(undefined8 *)(param_1 + 0x338));
        local_148 = *(undefined8 *)(lVar4 + 8);
        uStack_140 = *(undefined8 *)(lVar4 + 0x10);
        local_138 = *(size_t *)(lVar4 + 0x18);
        uStack_130 = *(ulonglong *)(lVar4 + 0x20);
        local_128 = *(undefined8 *)(lVar4 + 0x28);
        uStack_120 = *(undefined8 *)(lVar4 + 0x30);
        local_118 = *(undefined8 *)(lVar4 + 0x38);
        uStack_110 = *(undefined8 *)(lVar4 + 0x40);
        local_108 = *(ulonglong *)(lVar4 + 0x48);
        uStack_100 = *(undefined8 *)(lVar4 + 0x50);
      }
      else if (bVar1 == 0x23) {
        local_128 = *(undefined8 *)(param_5 + 8);
        local_148 = 0xe;
      }
      else if (bVar1 == 0x9e) {
        uVar5 = *(ulonglong *)(param_5 + 8);
        pvVar2 = *(void **)(param_5 + 0x10);
        if (8 < uVar5) {
          FUN_14000a7d0(0x2e);
          uVar5 = 8;
        }
        local_148 = CONCAT44(local_148._4_4_,1);
        uStack_120 = 0;
        local_138 = uVar5;
        memcpy(&uStack_120,pvVar2,uVar5);
      }
      else if (bVar1 == 0xa4) {
        uVar5 = *(ulonglong *)(param_5 + 8);
        pvVar2 = *(void **)(param_5 + 0x10);
        if (8 < uVar5) {
          FUN_14000a7d0(0x2e);
          uVar5 = 8;
        }
        local_148 = CONCAT44(local_148._4_4_,1);
        uStack_120 = 0;
        local_138 = uVar5;
        memcpy(&uStack_120,pvVar2,uVar5);
      }
      else {
        if ((bVar1 != 0xa3) && (bVar1 != 0xf3)) {
          *param_6 = 0;
          param_6[1] = 0;
          param_6[2] = 0;
          param_6[3] = 0;
          param_6[4] = 0;
          param_6[5] = 0;
          param_6[6] = 0;
          param_6[7] = 0;
          param_6[8] = 0;
          param_6[9] = 0;
          return 0xffffffff;
        }
        local_220 = local_200;
        local_218 = 0;
        local_228 = &local_208;
        iVar3 = FUN_1400191a0(param_1,param_2,*(undefined8 *)(param_5 + 0x10),
                              *(undefined8 *)(param_5 + 8));
        if ((iVar3 == 0) && (local_208 != (void *)0x0)) {
          local_210 = local_f8;
          local_218 = CONCAT62(local_218._2_6_,param_4);
          local_220 = (undefined1 *)0x0;
          local_228 = (void **)param_3;
          iVar3 = FUN_14000be20(param_1,param_2,1,&local_208);
          if (iVar3 == 0) {
            local_118 = CONCAT44(local_118._4_4_,uStack_f0);
            uStack_110 = local_e8;
            uStack_100 = uStack_d0;
            local_108 = local_d8 & 0xffffffff;
            local_148 = CONCAT44(local_148._4_4_,0xf);
          }
          else {
            FUN_14000a7d0(0x2f);
          }
          free(*(void **)((longlong)local_208 + 0x18));
          *(undefined8 *)((longlong)local_208 + 0x18) = 0;
          free(local_208);
        }
      }
      goto LAB_14000d66a;
    }
    uStack_140 = *(undefined8 *)(param_5 + 0x10);
  }
  local_148 = 2;
LAB_14000d66a:
  *param_6 = local_148;
  param_6[1] = uStack_140;
  param_6[2] = local_138;
  param_6[3] = uStack_130;
  param_6[4] = local_128;
  param_6[5] = uStack_120;
  param_6[6] = local_118;
  param_6[7] = uStack_110;
  param_6[8] = local_108;
  param_6[9] = uStack_100;
  return 0;
}


/* ================= FUN_14000d6d0 @ 14000d6d0  size=201  callers=1 callees=3 ================= */
/* CALLERS: FUN_14000aaf0 */
/* CALLEES: FUN_140001ef0, FUN_1400090c0, FUN_140018f60 */

undefined8 FUN_14000d6d0(longlong param_1,longlong param_2)

{
  longlong *plVar1;
  char cVar2;
  int iVar3;
  ulonglong uVar4;
  ulonglong uVar5;
  ulonglong local_res8;
  longlong local_res18;
  
  if ((param_1 != 0) && (param_2 != 0)) {
    cVar2 = FUN_1400090c0(param_2,0x10);
    if (cVar2 != '\0') {
      uVar5 = 0;
      local_res8 = 0;
      iVar3 = FUN_140018f60(param_2,&local_res18,&local_res8,0);
      if (0 < iVar3) {
        return 0xffffffff;
      }
      uVar4 = local_res8;
      if (0 < (longlong)local_res8) {
        do {
          plVar1 = *(longlong **)(local_res18 + uVar5 * 8);
          if (plVar1 != (longlong *)0x0) {
            if (*(longlong *)(param_2 + 0x50) != 0) {
              *plVar1 = (longlong)*(int *)(*(longlong *)(param_2 + 0x50) + 8);
            }
            FUN_140001ef0(param_1 + 1000,local_res18 + uVar5 * 8);
            uVar4 = local_res8;
          }
          uVar5 = uVar5 + 1;
        } while (uVar5 < uVar4);
      }
    }
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_14000d7a0 @ 14000d7a0  size=74  callers=1 callees=6 ================= */
/* CALLERS: FUN_14000bf80 */
/* CALLEES: FUN_140003d10, FUN_140003e50, FUN_140009400, FUN_1400094f0, FUN_14000a2f0, FUN_14000dca0 */

undefined8 FUN_14000d7a0(undefined8 param_1)

{
  char cVar1;
  
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    return 0xffffffff;
  }
  FUN_14000dca0();
  FUN_140003d10(param_1);
  FUN_140003e50(param_1);
  FUN_1400094f0(param_1);
  FUN_140009400(param_1);
  return 0;
}


/* ================= FUN_14000d7f0 @ 14000d7f0  size=45  callers=1 callees=0 ================= */
/* CALLERS: FUN_140003a70 */

void FUN_14000d7f0(longlong param_1)

{
  if (DAT_140097c00 == param_1) {
    DAT_140097c00 = 0;
  }
  if (DAT_140097c08 == param_1) {
    DAT_140097c08 = 0;
  }
  return;
}


/* ================= FUN_14000d820 @ 14000d820  size=160  callers=2 callees=3 ================= */
/* CALLERS: FUN_140004070, FUN_1400044f0 */
/* CALLEES: FUN_140002200, FUN_140002210, FUN_140005960 */

void FUN_14000d820(longlong param_1,longlong param_2,char param_3)

{
  undefined8 uVar1;
  longlong lVar2;
  longlong lVar3;
  longlong local_res10;
  
  lVar2 = *(longlong *)(param_1 + 0x338);
  lVar3 = 0x1f0;
  if (param_3 == '\0') {
    lVar3 = 0x228;
  }
  local_res10 = param_2;
  uVar1 = FUN_140002200(lVar3 + lVar2,&local_res10);
  lVar3 = FUN_140002210(lVar2 + 0x1f0,uVar1);
  if (lVar3 != local_res10) {
    lVar2 = FUN_140005960(lVar2,lVar3,param_3);
    if (lVar2 != 0) {
      if (param_3 != '\0') {
        *(undefined8 *)(param_1 + 0x140) = *(undefined8 *)(lVar2 + 0x50);
        return;
      }
      *(undefined8 *)(param_1 + 0x148) = *(undefined8 *)(lVar2 + 0x50);
    }
  }
  return;
}


/* ================= FUN_14000d8c0 @ 14000d8c0  size=112  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000aaf0 */
/* CALLEES: FUN_14000a2f0, FUN_140015d70 */

undefined8 FUN_14000d8c0(longlong param_1)

{
  longlong lVar1;
  char cVar2;
  int iVar3;
  ushort local_res8 [16];
  
  if (param_1 != 0) {
    lVar1 = *(longlong *)(param_1 + 0x338);
    cVar2 = FUN_14000a2f0(lVar1);
    if (cVar2 != '\0') {
      iVar3 = FUN_140015d70(param_1,0,local_res8,0,0,0,0);
      if (iVar3 == 0) {
        if (*(ushort *)(lVar1 + 0x80) < local_res8[0]) {
          *(ushort *)(lVar1 + 0x80) = local_res8[0];
        }
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_14000d930 @ 14000d930  size=105  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000aaf0 */
/* CALLEES: FUN_14000a2f0, FUN_140015e30 */

undefined8 FUN_14000d930(longlong param_1)

{
  char cVar1;
  undefined8 uVar2;
  
  if (param_1 != 0) {
    cVar1 = FUN_14000a2f0(*(undefined8 *)(param_1 + 0x338));
    if (cVar1 != '\0') {
      uVar2 = FUN_140015e30(param_1,0,0,0,0,0,0,0,0,0,0,0);
      if ((int)uVar2 == 0) {
        return uVar2;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_14000d9a0 @ 14000d9a0  size=179  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400080f0 */
/* CALLEES: FUN_14000a2f0, FUN_140015e30, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14000d9a0(longlong param_1,longlong param_2)

{
  longlong lVar1;
  char cVar2;
  undefined1 auStack_88 [32];
  undefined8 local_68;
  undefined8 local_60;
  undefined8 local_58;
  undefined8 local_50;
  longlong *local_48;
  undefined8 local_40;
  undefined8 local_38;
  undefined8 local_30;
  longlong local_28;
  ulonglong local_20;
  
  local_20 = DAT_140096fe8 ^ (ulonglong)auStack_88;
  if ((param_1 != 0) && (cVar2 = FUN_14000a2f0(*(undefined8 *)(param_1 + 0x338)), cVar2 != '\0')) {
    lVar1 = *(longlong *)(param_1 + 0x148);
    do {
      local_30 = 0;
      local_48 = &local_28;
      local_38 = 0;
      local_40 = 0;
      local_50 = 0;
      local_58 = 0;
      local_60 = 0;
      local_68 = 0;
      FUN_140015e30(param_1,0,0,0);
      if (param_2 == local_28) {
        return 0;
      }
    } while (*(longlong *)(param_1 + 0x148) != lVar1);
  }
  return 0xffffffff;
}


/* ================= FUN_14000da60 @ 14000da60  size=41  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000de50 */
/* CALLEES: FUN_140006920 */

undefined8 FUN_14000da60(undefined8 param_1,undefined8 param_2,undefined4 param_3)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0xffffffff;
  }
  *(undefined4 *)(lVar1 + 0xc) = param_3;
  return 0;
}


/* ================= FUN_14000da90 @ 14000da90  size=41  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000de50 */
/* CALLEES: FUN_140006920 */

undefined8 FUN_14000da90(undefined8 param_1,undefined8 param_2,undefined4 param_3)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0xffffffff;
  }
  *(undefined4 *)(lVar1 + 0x10) = param_3;
  return 0;
}


/* ================= FUN_14000dac0 @ 14000dac0  size=42  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000e0d0 */
/* CALLEES: FUN_140006920 */

undefined8 FUN_14000dac0(undefined8 param_1,undefined8 param_2,undefined1 param_3)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0xffffffff;
  }
  *(undefined1 *)(lVar1 + 0x14) = param_3;
  return 0;
}


/* ================= FUN_14000daf0 @ 14000daf0  size=41  callers=3 callees=1 ================= */
/* CALLERS: FUN_140003fc0, FUN_1400044f0, FUN_14000e0d0 */
/* CALLEES: FUN_140006920 */

undefined8 FUN_14000daf0(undefined8 param_1,undefined8 param_2,undefined4 param_3)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0xffffffff;
  }
  *(undefined4 *)(lVar1 + 8) = param_3;
  return 0;
}


/* ================= FUN_14000db20 @ 14000db20  size=44  callers=2 callees=1 ================= */
/* CALLERS: FUN_140006fb0, FUN_14000ded0 */
/* CALLEES: FUN_140006920 */

undefined4 FUN_14000db20(undefined8 param_1,undefined8 param_2,ulonglong param_3)

{
  ulonglong *puVar1;
  
  puVar1 = (ulonglong *)FUN_140006920();
  if (puVar1 == (ulonglong *)0x0) {
    return 0xffffffff;
  }
  *puVar1 = param_3 & 0xffffffff;
  return 0;
}


/* ================= FUN_14000db50 @ 14000db50  size=42  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400070e0 */
/* CALLEES: FUN_140006920 */

undefined8 FUN_14000db50(undefined8 param_1,undefined8 param_2,undefined8 param_3)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0xffffffff;
  }
  *(undefined8 *)(lVar1 + 0x18) = param_3;
  return 0;
}


/* ================= FUN_14000db80 @ 14000db80  size=167  callers=1 callees=5 ================= */
/* CALLERS: FUN_1400094f0 */
/* CALLEES: FUN_140007690, FUN_140007750, FUN_140007780, FUN_140008d20, FUN_140009f90 */

undefined8 FUN_14000db80(undefined8 param_1,uint param_2,uint param_3)

{
  char cVar1;
  undefined4 uVar2;
  longlong lVar3;
  char *pcVar4;
  longlong lVar5;
  ulonglong uVar6;
  ulonglong uVar7;
  
  lVar3 = FUN_140008d20();
  uVar2 = FUN_140007780(param_1,param_2);
  cVar1 = FUN_140009f90(uVar2);
  if ((cVar1 != '\0') && (lVar3 != -1)) {
    pcVar4 = (char *)FUN_140007690(param_1,param_2);
    if (pcVar4 != (char *)0x0) {
      if ((*pcVar4 != '?') && (*pcVar4 != '$')) {
        lVar5 = FUN_140008d20(param_1,param_3);
        if (lVar3 != lVar5) {
          return 1;
        }
        uVar6 = FUN_140007750(param_1,param_2);
        uVar7 = FUN_140007750(param_1,param_3);
        if ((uVar7 < uVar6) || (param_2 < param_3)) {
          return 2;
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_14000dc30 @ 14000dc30  size=97  callers=1 callees=6 ================= */
/* CALLERS: FUN_14000e0d0 */
/* CALLEES: FUN_140005e30, FUN_140006fb0, FUN_140009d00, FUN_140009d20, FUN_140009d80, FUN_140009fa0 */

undefined1 FUN_14000dc30(undefined8 param_1,longlong param_2)

{
  undefined8 uVar1;
  longlong lVar2;
  char cVar3;
  
  if ((param_2 == 0) || (*(longlong *)(param_2 + 0x58) == 0)) {
    return 0;
  }
  uVar1 = FUN_140006fb0(param_2);
  cVar3 = FUN_140009d80(uVar1);
  if ((cVar3 == '\0') &&
     ((cVar3 = FUN_140009d20(), cVar3 == '\0' && (cVar3 = FUN_140009fa0(), cVar3 == '\0')))) {
    return 0;
  }
  cVar3 = FUN_140009d00();
  if ((cVar3 != '\0') && (lVar2 = FUN_140005e30(param_2), lVar2 == 0)) {
    return 0;
  }
  return 1;
}


/* ================= FUN_14000dca0 @ 14000dca0  size=112  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000d7a0 */
/* CALLEES: FUN_140019ae0, qsort */

undefined8 FUN_14000dca0(longlong param_1)

{
  uint uVar1;
  undefined8 uVar2;
  uint uVar3;
  ulonglong uVar4;
  
  uVar2 = FUN_140019ae0(*(int *)(param_1 + 4) << 3);
  *(undefined8 *)(param_1 + 0x60) = uVar2;
  uVar4 = 0;
  uVar1 = 0;
  if (*(int *)(param_1 + 4) != 0) {
    do {
      *(ulonglong *)(*(longlong *)(param_1 + 0x60) + uVar4 * 8) =
           *(longlong *)(param_1 + 0x58) + uVar4 * 0x18;
      uVar3 = (int)uVar4 + 1;
      uVar4 = (ulonglong)uVar3;
      uVar1 = *(uint *)(param_1 + 4);
    } while (uVar3 < uVar1);
  }
  qsort(*(void **)(param_1 + 0x60),(ulonglong)uVar1,8,FUN_140003bc0);
  return 0;
}


/* ================= FUN_14000dd10 @ 14000dd10  size=214  callers=3 callees=8 ================= */
/* CALLERS: FUN_140005960, FUN_14000aaf0, FUN_14000e0d0 */
/* CALLEES: FUN_140002050, FUN_1400023e0, FUN_140005910, FUN_14000a2f0, FUN_14000a610, FUN_14000a640, FUN_14000ddf0, FUN_14000ded0 */

undefined8 FUN_14000dd10(longlong param_1,longlong param_2)

{
  char cVar1;
  undefined8 uVar2;
  longlong local_res10;
  
  if ((param_2 != 0) && (local_res10 = param_2, cVar1 = FUN_14000a2f0(), cVar1 != '\0')) {
    cVar1 = FUN_14000a610(param_2);
    uVar2 = FUN_140005910(param_1,*(undefined8 *)(param_2 + 0x20),cVar1 == '\0');
    cVar1 = FUN_14000a640();
    if (cVar1 != '\0') {
      FUN_140002050(param_1 + 0x180);
      FUN_1400023e0(param_1 + 0x180,uVar2,&local_res10);
      *(int *)(param_1 + 0xb0) = *(int *)(param_1 + 0xb0) + 1;
      FUN_14000ded0(local_res10);
      FUN_14000ddf0(local_res10);
      return 1;
    }
  }
  return 0;
}


/* ================= FUN_14000ddf0 @ 14000ddf0  size=90  callers=1 callees=5 ================= */
/* CALLERS: FUN_14000dd10 */
/* CALLEES: FUN_140004df0, FUN_140006580, FUN_140006d70, FUN_14000a610, FUN_14000de50 */

void FUN_14000ddf0(longlong param_1)

{
  undefined8 uVar1;
  int iVar2;
  char local_res8;
  undefined8 local_res10 [3];
  
  if (((param_1 != 0) && (iVar2 = FUN_140006580(param_1,local_res10), iVar2 != 0)) &&
     (uVar1 = FUN_140004df0(), local_res8 != '\0')) {
    FUN_140006d70(param_1,uVar1);
    FUN_14000a610();
    FUN_14000de50(local_res10[0]);
  }
  return;
}


/* ================= FUN_14000de50 @ 14000de50  size=115  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000ddf0 */
/* CALLEES: FUN_140006900, FUN_14000a2f0, FUN_14000da60, FUN_14000da90 */

void FUN_14000de50(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined1 param_4)

{
  undefined8 uVar1;
  char cVar2;
  undefined1 uVar3;
  
  uVar3 = param_4;
  cVar2 = FUN_14000a2f0();
  if (cVar2 != '\0') {
    uVar1 = FUN_140006900(param_2,uVar3);
    FUN_14000da90(param_1,uVar1,param_3);
    uVar1 = FUN_140006900(param_3,param_4);
    FUN_14000da60(param_1,uVar1,param_2);
  }
  return;
}


/* ================= FUN_14000ded0 @ 14000ded0  size=77  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000dd10 */
/* CALLEES: FUN_140006580, FUN_1400068b0, FUN_14000db20, FUN_140016840 */

void FUN_14000ded0(longlong param_1)

{
  undefined8 uVar1;
  int iVar2;
  undefined8 local_res10 [3];
  
  if (((param_1 != 0) && (iVar2 = FUN_140006580(param_1,local_res10), iVar2 != 0)) &&
     (iVar2 = FUN_140016840(), iVar2 == 0)) {
    uVar1 = FUN_1400068b0(param_1);
    FUN_14000db20(local_res10[0],uVar1);
  }
  return;
}


/* ================= FUN_14000df20 @ 14000df20  size=8  callers=4 callees=0 ================= */
/* CALLERS: FUN_14000c0f0, FUN_14000c1c0, FUN_14000c340, FUN_14000c520 */

ushort FUN_14000df20(ushort param_1)

{
  return param_1 << 8 | param_1 >> 8;
}


/* ================= FUN_14000df30 @ 14000df30  size=5  callers=9 callees=0 ================= */
/* CALLERS: FUN_14000c0f0, FUN_14000c1c0, FUN_14000c340, FUN_14000c520, FUN_14000cb60, FUN_14000ccf0, FUN_14000cf50, FUN_14000d0c0, FUN_14000df40 */

uint FUN_14000df30(uint param_1)

{
  return param_1 >> 0x18 | (param_1 & 0xff0000) >> 8 | (param_1 & 0xff00) << 8 | param_1 << 0x18;
}


/* ================= FUN_14000df40 @ 14000df40  size=40  callers=4 callees=1 ================= */
/* CALLERS: FUN_14000c1c0, FUN_14000c520, FUN_14000ccf0, FUN_14000d0c0 */
/* CALLEES: FUN_14000df30 */

ulonglong FUN_14000df40(ulonglong param_1)

{
  uint uVar1;
  ulonglong uVar2;
  
  uVar1 = FUN_14000df30(param_1 >> 0x20,param_1);
  uVar2 = (ulonglong)uVar1;
  uVar1 = FUN_14000df30(param_1 & 0xffffffff);
  return (ulonglong)uVar1 << 0x20 | uVar2;
}


/* ================= FUN_14000df70 @ 14000df70  size=79  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000be20 */

ulonglong FUN_14000df70(longlong *param_1,ulonglong param_2,ulonglong param_3)

{
  ulonglong in_RAX;
  ulonglong uVar1;
  
  if ((short)param_1[2] == 0) goto LAB_14000dfaf;
  in_RAX = param_2 - 1;
  if (in_RAX < 0xfffffffffffffffe) {
    in_RAX = ~param_3;
    if (in_RAX < (ulonglong)param_1[1]) {
      uVar1 = 0xffffffffffffffff;
    }
    else {
      uVar1 = param_1[1] + param_3;
    }
    if (uVar1 == *param_1 + param_3) {
      if (param_2 == uVar1) goto LAB_14000dfbc;
    }
    else if ((*param_1 + param_3 <= param_2) && (param_2 < uVar1)) goto LAB_14000dfbc;
LAB_14000dfaf:
    return in_RAX & 0xffffffffffffff00;
  }
LAB_14000dfbc:
  return CONCAT71((int7)(in_RAX >> 8),1);
}


/* ================= FUN_14000dfc0 @ 14000dfc0  size=185  callers=1 callees=7 ================= */
/* CALLERS: FUN_14000c2c0 */
/* CALLEES: FUN_140005310, FUN_140007860, FUN_1400078d0, FUN_140007950, FUN_140007980, FUN_14000a770, thunk_FUN_1400078a0 */

undefined8 FUN_14000dfc0(void)

{
  char cVar1;
  int iVar2;
  int *piVar3;
  
  piVar3 = (int *)thunk_FUN_1400078a0();
  if (*piVar3 != 0x464c457f) {
    FUN_14000a770(0x3f);
    return 0xffffffff;
  }
  iVar2 = FUN_140007950();
  if (iVar2 != 2) {
    FUN_14000a770(0x40);
    return 0xffffffff;
  }
  iVar2 = FUN_140007860();
  if (iVar2 == 0) {
    FUN_14000a770(0x41);
    return 0xffffffff;
  }
  iVar2 = FUN_140005310();
  if (iVar2 == 2) {
    FUN_14000a770(0x42);
    return 0xffffffff;
  }
  cVar1 = FUN_1400078d0();
  if (cVar1 != '\x01') {
    FUN_14000a770(0x43);
    return 0xffffffff;
  }
  iVar2 = FUN_140007980();
  if (iVar2 == 0) {
    FUN_14000a770(0x44);
    return 0xffffffff;
  }
  return 0;
}


/* ================= FUN_14000e080 @ 14000e080  size=70  callers=3 callees=2 ================= */
/* CALLERS: FUN_140003d10, FUN_140003e50, FUN_140006ef0 */
/* CALLEES: _guard_check_icall, snprintf */

void FUN_14000e080(ulonglong *param_1,undefined8 param_2)

{
  if (10000000 < *param_1) {
    snprintf(&DAT_140097c10,0x800,
             "Symbol at address %llX: did not pass allocation size sanity check",param_2);
    _guard_check_icall(&DAT_140097c10);
    *param_1 = 0;
  }
  return;
}


/* ================= FUN_14000e0d0 @ 14000e0d0  size=846  callers=1 callees=15 ================= */
/* CALLERS: FUN_14000b630 */
/* CALLEES: FUN_140001c60, FUN_1400043a0, FUN_140004600, FUN_1400068b0, FUN_1400069e0, FUN_140006d70, FUN_140006fb0, FUN_140007b80, FUN_140007e00, FUN_14000a610, FUN_14000dac0, FUN_14000daf0, FUN_14000dc30, FUN_14000dd10, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8
FUN_14000e0d0(longlong param_1,longlong param_2,undefined8 param_3,longlong param_4,
             ulonglong param_5)

{
  undefined8 uVar1;
  char cVar2;
  byte bVar3;
  int iVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  ulonglong uVar7;
  ulonglong uVar8;
  longlong lVar9;
  ulonglong uVar10;
  uint uVar11;
  ulonglong uVar12;
  ulonglong uVar13;
  byte bVar14;
  uint uVar15;
  ulonglong uVar16;
  undefined1 auStack_4c8 [32];
  ulonglong local_4a8;
  undefined8 local_4a0;
  ulonglong local_498;
  byte local_488;
  undefined1 local_487;
  uint local_484;
  uint local_480 [2];
  undefined8 local_478;
  undefined8 local_470;
  longlong local_468;
  undefined8 local_460;
  ulonglong local_458;
  longlong local_450;
  ulonglong local_448 [64];
  ulonglong local_248 [64];
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_4c8;
  if ((((param_1 != 0) && (param_2 != 0)) && (param_4 != 0)) && (param_5 != 0)) {
    uVar10 = 0;
    local_480[0] = 0;
    bVar14 = 0;
    lVar9 = param_2;
    uVar13 = uVar10;
    local_468 = param_4;
    local_450 = param_2;
    cVar2 = FUN_14000a610(param_2);
    local_487 = cVar2 == '\0';
    if (*(longlong *)(lVar9 + 0x50) != 0) {
      local_458 = (ulonglong)*(int *)(*(longlong *)(lVar9 + 0x50) + 0x60);
      uVar12 = uVar10;
      uVar16 = uVar10;
      uVar7 = uVar10;
      uVar8 = uVar10;
      local_448[0] = uVar13;
      local_248[0] = uVar13;
      do {
        iVar4 = (int)uVar10;
        if (iVar4 < (int)uVar12) {
          uVar12 = uVar10;
        }
        uVar11 = (uint)uVar12;
        local_484 = uVar11;
        if ((int)uVar16 < iVar4) {
          uVar5 = FUN_1400068b0(param_2);
          FUN_14000daf0(param_1,uVar5,uVar8);
          if (uVar11 < 0x40) {
            local_448[uVar12] = uVar8;
            local_248[uVar12] = uVar7;
            local_484 = uVar11 + 1;
            uVar12 = (ulonglong)local_484;
          }
        }
        uVar16 = (ulonglong)iVar4;
        if (param_2 == 0) {
LAB_14000e329:
          uVar7 = FUN_140006fb0(param_2);
          uVar8 = FUN_140006d70(param_2);
          if (param_2 != 0) goto LAB_14000e347;
LAB_14000e364:
          lVar9 = 0;
        }
        else {
          local_478 = FUN_140006fb0(param_2);
          if ((int)uVar12 == 0) {
            uVar13 = 0;
          }
          else {
            uVar13 = local_448[(int)uVar12 - 1];
          }
          local_470 = FUN_140007b80(uVar12,local_448,local_248);
          uVar5 = FUN_1400068b0(param_2);
          cVar2 = FUN_1400069e0(param_1,uVar5);
          uVar6 = FUN_1400068b0(param_2);
          uVar1 = local_470;
          uVar5 = local_478;
          bVar14 = 0;
          local_460 = uVar6;
          if (cVar2 == '\0') {
            uVar7 = 0;
            if (param_5 != 0) {
              bVar14 = 0;
              do {
                local_4a0 = uVar1;
                local_4a8 = uVar13;
                local_498 = uVar16;
                bVar3 = (**(code **)(local_468 + uVar7 * 8))(param_1,param_2,uVar5,local_487);
                uVar7 = (ulonglong)((int)uVar7 + 1);
                bVar14 = bVar14 | bVar3;
              } while (uVar7 < param_5);
              uVar12 = (ulonglong)local_484;
              uVar16 = uVar10;
              local_488 = bVar14;
            }
            uVar6 = local_460;
            FUN_14000dac0(param_1,local_460,1);
          }
          if (uVar13 != 0) {
            FUN_14000daf0(param_1,uVar6,uVar13);
          }
          cVar2 = FUN_14000dc30(param_1,param_2,uVar6);
          if (cVar2 == '\0') goto LAB_14000e329;
          uVar5 = FUN_140007e00(param_2,0xff);
          FUN_140001c60(*(undefined8 *)(param_1 + 0x458),uVar6,uVar5);
          uVar7 = FUN_140006fb0(param_2);
          uVar8 = FUN_140006d70(param_2);
LAB_14000e347:
          if (local_458 <= *(ulonglong *)(param_2 + 0x28)) goto LAB_14000e364;
          lVar9 = FUN_1400043a0(param_2,local_480);
        }
        uVar15 = (uint)uVar16;
        uVar11 = *(uint *)(param_1 + 0x490);
        if (*(uint *)(param_1 + 0x490) < uVar15) {
          uVar11 = uVar15;
        }
        *(uint *)(param_1 + 0x490) = uVar11;
        if (param_2 != local_450) {
          if (bVar14 == 0) {
            *(longlong *)(param_1 + 0x478) =
                 *(longlong *)(param_1 + 0x478) +
                 *(longlong *)(*(longlong *)(param_2 + 0x38) + 0x28);
            *(longlong *)(param_1 + 0x488) = *(longlong *)(param_1 + 0x488) + 1;
          }
          else {
            uVar11 = *(uint *)(param_1 + 0x494);
            if (*(uint *)(param_1 + 0x494) < uVar15) {
              uVar11 = uVar15;
            }
            *(uint *)(param_1 + 0x494) = uVar11;
            *(longlong *)(param_1 + 0x470) =
                 *(longlong *)(param_1 + 0x470) +
                 *(longlong *)(*(longlong *)(param_2 + 0x38) + 0x28);
            *(longlong *)(param_1 + 0x480) = *(longlong *)(param_1 + 0x480) + 1;
            iVar4 = FUN_14000dd10(param_1,param_2);
            if (iVar4 != 0) goto LAB_14000e3dd;
          }
          FUN_140004600(param_2);
        }
LAB_14000e3dd:
        uVar10 = (ulonglong)local_480[0];
        param_2 = lVar9;
        if (lVar9 == 0) {
          return 0;
        }
      } while( true );
    }
  }
  return 0xffffffff;
}


/* ================= FUN_14000e420 @ 14000e420  size=8  callers=5 callees=0 ================= */
/* CALLERS: FUN_1400151c0, FUN_140042f90, FUN_140047950, FUN_140059abc, snprintf */

undefined * FUN_14000e420(void)

{
  return &DAT_14286d3c0;
}


/* ================= FUN_14000e430 @ 14000e430  size=101  callers=2 callees=1 ================= */
/* CALLERS: FUN_14000a310, FUN_14001f8f0 */
/* CALLEES: strchr */

byte FUN_14000e430(char *param_1)

{
  char *pcVar1;
  char *pcVar2;
  uint uVar3;
  
  if (((param_1 != (char *)0x0) && (pcVar1 = strchr(param_1,0x3a), pcVar1 != (char *)0x0)) &&
     (pcVar1 != param_1)) {
    uVar3 = (byte)pcVar1[-1] - 0x41;
    if (((byte)uVar3 < 0x3a) && ((0x3ffffff43ffffffU >> ((ulonglong)uVar3 & 0x3f) & 1) != 0)) {
      pcVar2 = pcVar1 + -2;
      while( true ) {
        if (pcVar2 < param_1) {
          return pcVar1[-1];
        }
        if (*pcVar2 != ' ') break;
        pcVar2 = pcVar2 + -1;
      }
    }
  }
  return 0x20;
}


/* ================= FUN_14000e4a0 @ 14000e4a0  size=84  callers=1 callees=1 ================= */
/* CALLERS: FUN_140002fb0 */
/* CALLEES: strncat */

ulonglong FUN_14000e4a0(char *param_1,char *param_2,int param_3)

{
  ulonglong _Count;
  longlong lVar1;
  
  if (((param_1 != (char *)0x0) && (param_2 != (char *)0x0)) && (0 < param_3)) {
    lVar1 = -1;
    do {
      lVar1 = lVar1 + 1;
    } while (param_1[lVar1] != '\0');
    _Count = (param_3 - lVar1) - 1;
    if (_Count != 0) {
      strncat(param_1,param_2,_Count);
    }
    return _Count & 0xffffffff;
  }
  return 0;
}


/* ================= FUN_14000e500 @ 14000e500  size=97  callers=4 callees=1 ================= */
/* CALLERS: FUN_140002fb0, FUN_1400065c0, FUN_140006c00, FUN_1400070e0 */
/* CALLEES: memcpy */

ulonglong FUN_14000e500(void *param_1,void *param_2,int param_3)

{
  ulonglong _Size;
  
  if (((param_1 != (void *)0x0) && (param_2 != (void *)0x0)) && (0 < param_3)) {
    _Size = 0xffffffffffffffff;
    do {
      _Size = _Size + 1;
    } while (*(char *)((longlong)param_2 + _Size) != '\0');
    if ((ulonglong)(longlong)param_3 <= _Size) {
      _Size = (longlong)param_3 - 1;
    }
    if (_Size != 0) {
      memcpy(param_1,param_2,_Size);
    }
    *(undefined1 *)(_Size + (longlong)param_1) = 0;
    return _Size & 0xffffffff;
  }
  return 0;
}


/* ================= snprintf @ 14000e570  size=92  callers=10 callees=2 ================= */
/* CALLERS: FUN_140002fb0, FUN_140004070, FUN_1400065c0, FUN_14000aaf0, FUN_14000e080, FUN_14001baf0, FUN_14001bb20, FUN_1400397c0, FUN_140048540, FUN_140056f60 */
/* CALLEES: FUN_14000e420, __stdio_common_vsprintf */

/* Library Function - Single Match
    snprintf
   
   Library: Visual Studio 2019 Release */

int snprintf(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  int iVar1;
  ulonglong *puVar2;
  undefined8 local_res20;
  
  local_res20 = param_4;
  puVar2 = (ulonglong *)FUN_14000e420();
  iVar1 = __stdio_common_vsprintf(*puVar2 | 2,param_1,param_2,param_3,0,&local_res20);
  if (iVar1 < 0) {
    iVar1 = -1;
  }
  return iVar1;
}


/* ================= FUN_14000e5e0 @ 14000e5e0  size=18  callers=1 callees=1 ================= */
/* CALLERS: FUN_14001b820 */
/* CALLEES: FUN_140002970 */

void FUN_14000e5e0(void)

{
  undefined8 uVar1;
  
  if (DAT_1400994f0 == 0) {
    FUN_140002970();
    return;
  }
  uVar1 = FUN_140002970();
  FUN_14000e6c0(uVar1,1,0);
  return;
}


/* ================= FUN_14000e600 @ 14000e600  size=18  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000f0c0 */
/* CALLEES: thunk_FUN_140008b40 */

void FUN_14000e600(void)

{
  undefined8 uVar1;
  
  if (DAT_1400994f0 == 0) {
    thunk_FUN_140008b40();
    return;
  }
  uVar1 = thunk_FUN_140008b40();
  FUN_14000e6c0(uVar1,1,0);
  return;
}


/* ================= FUN_14000e630 @ 14000e630  size=34  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000e660 */
/* CALLEES: free */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_14000e630(void)

{
  free(DAT_1400994f8);
  _DAT_1400994f0 = 0;
  DAT_1400994f8 = (void *)0x0;
  return 0;
}


/* ================= FUN_14000e660 @ 14000e660  size=16  callers=0 callees=1 ================= */
/* CALLEES: FUN_14000e630 */

undefined8 FUN_14000e660(void)

{
  FUN_14000e630();
  return 0;
}


/* ================= FUN_14000e670 @ 14000e670  size=26  callers=0 callees=2 ================= */
/* CALLEES: FUN_140002970, FUN_14000e6c0 */

void FUN_14000e670(void)

{
  undefined8 uVar1;
  
  uVar1 = FUN_140002970();
  FUN_14000e6c0(uVar1,1,0);
  return;
}


/* ================= FUN_14000e690 @ 14000e690  size=26  callers=0 callees=2 ================= */
/* CALLEES: FUN_14000e6c0, thunk_FUN_140008b40 */

void FUN_14000e690(void)

{
  undefined8 uVar1;
  
  uVar1 = thunk_FUN_140008b40();
  FUN_14000e6c0(uVar1,1,0);
  return;
}


/* ================= FUN_14000e6b0 @ 14000e6b0  size=16  callers=0 callees=1 ================= */
/* CALLEES: FUN_14000e8d0 */

undefined8 FUN_14000e6b0(void)

{
  FUN_14000e8d0();
  return 0;
}


/* ================= FUN_14000e6c0 @ 14000e6c0  size=149  callers=2 callees=0 ================= */
/* CALLERS: FUN_14000e670, FUN_14000e690 */

ulonglong FUN_14000e6c0(ulonglong param_1,byte param_2,ulonglong param_3)

{
  longlong lVar1;
  longlong *plVar2;
  uint uVar3;
  
  if (param_1 == 0xffffffffffffffff) {
    return 0xffffffffffffffff;
  }
  uVar3 = 0;
  if (DAT_1400994f0 != 0) {
    plVar2 = (longlong *)(DAT_1400994f8 + 0x10);
    while ((param_2 + param_1 <= (ulonglong)plVar2[-2] ||
           ((ulonglong)plVar2[-1] <= param_1 - (param_3 & 0xff)))) {
LAB_14000e740:
      uVar3 = uVar3 + 1;
      plVar2 = plVar2 + 0xc;
      if (DAT_1400994f0 <= uVar3) {
        return param_1;
      }
    }
    lVar1 = *plVar2;
    if (lVar1 < 1) {
      if (param_1 < (ulonglong)-lVar1) goto LAB_14000e740;
    }
    else if (-lVar1 - 1U < param_1) goto LAB_14000e740;
    param_1 = param_1 + lVar1;
  }
  return param_1;
}


/* ================= FUN_14000e760 @ 14000e760  size=333  callers=1 callees=8 ================= */
/* CALLERS: FUN_14000e8b0 */
/* CALLEES: FUN_140002970, FUN_140002a50, FUN_140002f00, _strtoui64, strstr, thunk_FUN_140008b90, thunk_FUN_14000ea00, thunk_FUN_14000eaa0 */

undefined8 FUN_14000e760(char *param_1,ulonglong *param_2,ulonglong *param_3)

{
  int iVar1;
  char *pcVar2;
  ulonglong uVar3;
  longlong lVar4;
  undefined8 uVar5;
  ulonglong local_18;
  ulonglong local_10;
  
  if (((param_1 != (char *)0x0) && (param_2 != (ulonglong *)0x0)) && (param_3 != (ulonglong *)0x0))
  {
    if ((*param_1 == '*') && (param_1[1] == '\0')) {
      *param_2 = 0;
      *param_3 = 0xfffffffffffffffe;
      return 0;
    }
    pcVar2 = strstr(param_1,"-");
    if (pcVar2 == (char *)0x0) {
      iVar1 = thunk_FUN_14000ea00(param_1);
      if (iVar1 != -1) {
        uVar3 = FUN_140002970(DAT_14286d3c8,iVar1);
        *param_2 = uVar3;
        lVar4 = FUN_140002a50(DAT_14286d3c8,iVar1);
        *param_3 = lVar4 + *param_2;
        return 0;
      }
      iVar1 = thunk_FUN_14000eaa0(param_1);
      if (iVar1 != -1) {
        uVar5 = FUN_140002f00(DAT_14286d3c8,iVar1);
        iVar1 = thunk_FUN_140008b90(uVar5,1,&local_18);
        if ((local_18 != 0xffffffffffffffff) && (iVar1 != 0)) {
          *param_2 = local_18;
          *param_3 = local_10;
          return 0;
        }
      }
    }
    else {
      *pcVar2 = '\0';
      uVar3 = _strtoui64(param_1,(char **)0x0,0);
      *param_2 = uVar3;
      uVar3 = _strtoui64(pcVar2 + 1,(char **)0x0,0);
      *param_3 = uVar3;
      if ((uVar3 != 0) || (*param_2 != 0)) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_14000e8b0 @ 14000e8b0  size=25  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000e8d0 */
/* CALLEES: FUN_14000e760 */

undefined8 FUN_14000e8b0(longlong param_1)

{
  undefined8 uVar1;
  
  if (*(char *)(param_1 + 0x18) == '\0') {
    return 0;
  }
  uVar1 = FUN_14000e760(param_1 + 0x18,param_1,param_1 + 8);
  return uVar1;
}


/* ================= FUN_14000e8d0 @ 14000e8d0  size=54  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000e6b0 */
/* CALLEES: FUN_14000e8b0 */

undefined8 FUN_14000e8d0(void)

{
  uint uVar1;
  ulonglong uVar2;
  
  uVar2 = 0;
  if (DAT_1400994f0 != 0) {
    do {
      FUN_14000e8b0(uVar2 * 0x60 + DAT_1400994f8);
      uVar1 = (int)uVar2 + 1;
      uVar2 = (ulonglong)uVar1;
    } while (uVar1 < DAT_1400994f0);
  }
  return 0;
}


/* ================= FUN_14000e990 @ 14000e990  size=111  callers=0 callees=1 ================= */
/* CALLEES: free */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14000e990(void)

{
  DAT_140099500 = 0;
  DAT_140099504 = 0;
  DAT_140099508 = 0;
  free(DAT_140099518);
  free(DAT_140099520);
  free(DAT_140099528);
  DAT_140099518 = (void *)0x0;
  DAT_140099520 = (void *)0x0;
  _DAT_140099530 = 0;
  _DAT_140099538 = 0;
  _DAT_140099540 = 0;
  DAT_140099528 = (void *)0x0;
  return;
}


/* ================= FUN_14000ea00 @ 14000ea00  size=148  callers=0 callees=2 ================= */
/* CALLEES: FUN_1400029a0, FUN_140002a20 */

uint FUN_14000ea00(longlong param_1)

{
  char cVar1;
  char cVar2;
  uint uVar3;
  char *pcVar4;
  uint uVar5;
  longlong lVar6;
  
  uVar3 = FUN_1400029a0(DAT_14286d3c8);
  uVar5 = 0;
  if (uVar3 != 0) {
    do {
      pcVar4 = (char *)FUN_140002a20(DAT_14286d3c8,uVar5);
      if (pcVar4 != (char *)0x0) {
        lVar6 = param_1 - (longlong)pcVar4;
        do {
          cVar1 = *pcVar4;
          cVar2 = pcVar4[lVar6];
          if (cVar1 != cVar2) break;
          pcVar4 = pcVar4 + 1;
        } while (cVar2 != '\0');
        if (cVar1 == cVar2) {
          return uVar5;
        }
      }
      uVar5 = uVar5 + 1;
    } while (uVar5 < uVar3);
  }
  return 0xffffffff;
}


/* ================= FUN_14000eaa0 @ 14000eaa0  size=149  callers=0 callees=3 ================= */
/* CALLEES: FUN_140002f00, FUN_140002f30, thunk_FUN_14000eb40 */

uint FUN_14000eaa0(longlong param_1)

{
  char cVar1;
  char cVar2;
  uint uVar3;
  undefined8 uVar4;
  char *pcVar5;
  uint uVar6;
  longlong lVar7;
  
  uVar3 = FUN_140002f30(DAT_14286d3c8);
  uVar6 = 0;
  if (uVar3 != 0) {
    do {
      uVar4 = FUN_140002f00(DAT_14286d3c8,uVar6);
      pcVar5 = (char *)thunk_FUN_14000eb40(uVar4);
      if (pcVar5 != (char *)0x0) {
        lVar7 = param_1 - (longlong)pcVar5;
        do {
          cVar1 = *pcVar5;
          cVar2 = pcVar5[lVar7];
          if (cVar1 != cVar2) break;
          pcVar5 = pcVar5 + 1;
        } while (cVar2 != '\0');
        if (cVar1 == cVar2) {
          return uVar6;
        }
      }
      uVar6 = uVar6 + 1;
    } while (uVar6 < uVar3);
  }
  return 0xffffffff;
}


/* ================= FUN_14000eb40 @ 14000eb40  size=114  callers=0 callees=4 ================= */
/* CALLEES: FUN_14000fc20, free, strncpy, thunk_FUN_140008a60 */

undefined * FUN_14000eb40(void)

{
  undefined *puVar1;
  char *_Source;
  
  puVar1 = (undefined *)thunk_FUN_140008a60();
  if (puVar1 != (undefined *)0x0) {
    _Source = (char *)FUN_14000fc20(puVar1,2);
    if (_Source != (char *)0x0) {
      strncpy(&DAT_140099550,_Source,0x800);
      DAT_140099d48._7_1_ = 0;
      free(_Source);
      return &DAT_140099550;
    }
  }
  return puVar1;
}


/* ================= FUN_14000ebc0 @ 14000ebc0  size=266  callers=0 callees=6 ================= */
/* CALLEES: FUN_140002970, FUN_1400029a0, FUN_1400029f0, FUN_140002a50, FUN_140002ad0, FUN_140002bc0 */

longlong FUN_14000ebc0(undefined4 param_1)

{
  uint uVar1;
  ulonglong uVar2;
  longlong lVar3;
  ulonglong uVar4;
  longlong lVar5;
  ulonglong uVar6;
  uint uVar7;
  
  uVar1 = FUN_1400029a0(DAT_14286d3c8);
  uVar2 = FUN_140002bc0(DAT_14286d3c8,param_1);
  lVar3 = FUN_140002ad0(DAT_14286d3c8,param_1);
  uVar6 = 0xffffffffffffffff;
  if ((uVar2 != 0xffffffffffffffff) && (lVar3 != 0)) {
    uVar7 = 0;
    if (uVar1 != 0) {
      do {
        uVar4 = FUN_1400029f0(DAT_14286d3c8,uVar7);
        if ((uVar4 & 2) != 0) {
          lVar5 = FUN_140002a50(DAT_14286d3c8,uVar7);
          uVar4 = FUN_140002970(DAT_14286d3c8,uVar7);
          if ((((uVar4 != 0xffffffffffffffff) && (lVar5 != 0)) && (uVar2 <= uVar4)) &&
             ((uVar4 + lVar5 <= lVar3 + uVar2 && (uVar4 < uVar6)))) {
            uVar6 = uVar4;
          }
        }
        uVar7 = uVar7 + 1;
      } while (uVar7 < uVar1);
    }
    if ((uVar2 <= uVar6) && (uVar6 <= lVar3 + uVar2)) {
      return uVar6 - uVar2;
    }
  }
  return 0;
}


/* ================= FUN_14000ecd0 @ 14000ecd0  size=31  callers=0 callees=4 ================= */
/* CALLEES: FUN_14000ecf0, FUN_14000ed20, FUN_14000ef20, FUN_14000f0c0 */

undefined8 FUN_14000ecd0(void)

{
  FUN_14000ecf0();
  FUN_14000ef20();
  FUN_14000ed20();
  FUN_14000f0c0();
  return 0;
}


/* ================= FUN_14000ecf0 @ 14000ecf0  size=35  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000ecd0 */
/* CALLEES: FUN_1400028e0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14000ecf0(void)

{
  int iVar1;
  
  iVar1 = FUN_1400028e0(DAT_14286d3c8);
  _DAT_140099514 = (uint)(iVar1 == 1);
  return;
}


/* ================= FUN_14000ed20 @ 14000ed20  size=498  callers=1 callees=7 ================= */
/* CALLERS: FUN_14000ecd0 */
/* CALLEES: FUN_140002970, FUN_1400029a0, FUN_140002a50, FUN_14000f180, FUN_14000f1b0, calloc, qsort */

undefined8 FUN_14000ed20(void)

{
  char cVar1;
  uint uVar2;
  undefined8 uVar3;
  uint uVar4;
  void *pvVar5;
  ulonglong uVar6;
  
  uVar2 = FUN_1400029a0(DAT_14286d3c8);
  DAT_140099504 = 0;
  DAT_140099508 = 0;
  if (uVar2 == 0) {
    return 0;
  }
  uVar4 = 0;
  if (uVar2 != 0) {
    do {
      cVar1 = FUN_14000f180(uVar4);
      if (cVar1 == '\0') {
        cVar1 = FUN_14000f1b0(uVar4);
        if (cVar1 != '\0') {
          DAT_140099508 = DAT_140099508 + 1;
        }
      }
      else {
        DAT_140099504 = DAT_140099504 + 1;
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < uVar2);
  }
  DAT_140099520 = calloc((ulonglong)DAT_140099504,0x18);
  DAT_140099528 = calloc((ulonglong)DAT_140099508,0x18);
  if ((DAT_140099520 == (void *)0x0) || (DAT_140099528 == (void *)0x0)) {
    return 0xffffffff;
  }
  DAT_140099504 = 0;
  uVar4 = 0;
  DAT_140099508 = 0;
  if (uVar2 != 0) {
    do {
      cVar1 = FUN_14000f180(uVar4);
      if (cVar1 == '\0') {
        cVar1 = FUN_14000f1b0(uVar4);
        if (cVar1 != '\0') {
          uVar3 = FUN_140002970(DAT_14286d3c8,uVar4);
          pvVar5 = DAT_140099528;
          uVar6 = (ulonglong)DAT_140099508;
          *(undefined8 *)((longlong)DAT_140099528 + uVar6 * 0x18) = uVar3;
          pvVar5 = (void *)((longlong)pvVar5 + uVar6 * 0x18);
          uVar3 = FUN_140002a50(DAT_14286d3c8,uVar4);
          DAT_140099508 = DAT_140099508 + 1;
          goto LAB_14000ee88;
        }
      }
      else {
        uVar3 = FUN_140002970(DAT_14286d3c8,uVar4);
        pvVar5 = DAT_140099520;
        uVar6 = (ulonglong)DAT_140099504;
        *(undefined8 *)((longlong)DAT_140099520 + uVar6 * 0x18) = uVar3;
        pvVar5 = (void *)((longlong)pvVar5 + uVar6 * 0x18);
        uVar3 = FUN_140002a50(DAT_14286d3c8,uVar4);
        DAT_140099504 = DAT_140099504 + 1;
LAB_14000ee88:
        *(undefined8 *)((longlong)pvVar5 + 8) = uVar3;
        *(uint *)((longlong)pvVar5 + 0x10) = uVar4;
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < uVar2);
  }
  qsort(DAT_140099520,(ulonglong)DAT_140099504,0x18,(_PtFuncCompare *)&LAB_14000e970);
  qsort(DAT_140099528,(ulonglong)DAT_140099508,0x18,(_PtFuncCompare *)&LAB_14000e970);
  return 0;
}


/* ================= FUN_14000ef20 @ 14000ef20  size=415  callers=1 callees=11 ================= */
/* CALLERS: FUN_14000ecd0 */
/* CALLEES: FUN_140002a80, FUN_140002aa0, FUN_140002ad0, FUN_140002b00, FUN_140002b30, FUN_140002b60, FUN_140002b90, FUN_140002bc0, calloc, free, thunk_FUN_14000ebc0 */

void FUN_14000ef20(void)

{
  void *pvVar1;
  int iVar2;
  undefined4 uVar3;
  longlong lVar4;
  longlong lVar5;
  ulonglong uVar6;
  longlong lVar7;
  longlong lVar8;
  ulonglong uVar9;
  uint uVar10;
  ulonglong uVar11;
  
  DAT_140099500 = FUN_140002a80(DAT_14286d3c8);
  pvVar1 = DAT_140099518;
  if (DAT_140099500 != 0) {
    DAT_140099518 = calloc((ulonglong)DAT_140099500,0x30);
    free(pvVar1);
    if ((DAT_140099518 != (void *)0x0) && (uVar11 = 0, DAT_140099500 != 0)) {
      do {
        lVar4 = FUN_140002bc0(DAT_14286d3c8,uVar11);
        lVar5 = FUN_140002b60(DAT_14286d3c8,uVar11);
        uVar6 = FUN_140002ad0(DAT_14286d3c8,uVar11);
        lVar7 = FUN_140002b30(DAT_14286d3c8,uVar11);
        iVar2 = FUN_140002b90(DAT_14286d3c8,uVar11);
        uVar3 = FUN_140002b00(DAT_14286d3c8,uVar11);
        lVar8 = FUN_140002aa0(DAT_14286d3c8,uVar11);
        if ((lVar4 != -1) && (lVar5 != -1)) {
          if ((iVar2 == 1) && (uVar9 = thunk_FUN_14000ebc0(uVar11), uVar9 <= uVar6)) {
            lVar8 = lVar8 + uVar9;
            lVar5 = lVar5 + uVar9;
            lVar4 = lVar4 + uVar9;
            uVar6 = uVar6 - uVar9;
            lVar7 = lVar7 - uVar9;
          }
          pvVar1 = DAT_140099518;
          *(longlong *)((longlong)DAT_140099518 + uVar11 * 0x30) = lVar4;
          *(longlong *)((longlong)pvVar1 + uVar11 * 0x30 + 8) = lVar5;
          *(ulonglong *)((longlong)pvVar1 + uVar11 * 0x30 + 0x10) = uVar6;
          *(longlong *)((longlong)pvVar1 + uVar11 * 0x30 + 0x18) = lVar7;
          *(undefined4 *)((longlong)pvVar1 + uVar11 * 0x30 + 0x20) = uVar3;
          *(int *)((longlong)pvVar1 + uVar11 * 0x30 + 0x24) = iVar2;
          *(longlong *)((longlong)pvVar1 + uVar11 * 0x30 + 0x28) = lVar8;
        }
        uVar10 = (int)uVar11 + 1;
        uVar11 = (ulonglong)uVar10;
      } while (uVar10 < DAT_140099500);
    }
  }
  return;
}


/* ================= FUN_14000f0c0 @ 14000f0c0  size=182  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000ecd0 */
/* CALLEES: FUN_140002f00, FUN_140002f30, FUN_14000e600, thunk_FUN_140008cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14000f0c0(void)

{
  uint uVar1;
  undefined8 uVar2;
  ulonglong uVar3;
  longlong lVar4;
  ulonglong uVar5;
  uint uVar6;
  ulonglong uVar8;
  ulonglong uVar9;
  ulonglong uVar7;
  
  uVar1 = FUN_140002f30(DAT_14286d3c8);
  uVar7 = 0;
  uVar9 = 0xffffffffffffffff;
  uVar8 = uVar7;
  uVar5 = uVar9;
  if (uVar1 != 0) {
    do {
      uVar2 = FUN_140002f00(DAT_14286d3c8,uVar7);
      uVar3 = FUN_14000e600(uVar2);
      lVar4 = thunk_FUN_140008cb0(uVar2);
      uVar5 = uVar9;
      if (uVar3 != 0xffffffffffffffff) {
        uVar5 = uVar3;
        if (uVar9 < uVar3) {
          uVar5 = uVar9;
        }
        if (uVar8 <= lVar4 + uVar3) {
          uVar8 = lVar4 + uVar3;
        }
      }
      uVar6 = (int)uVar7 + 1;
      uVar7 = (ulonglong)uVar6;
      uVar9 = uVar5;
    } while (uVar6 < uVar1);
  }
  _DAT_140099540 = uVar1;
  _DAT_140099530 = uVar5;
  _DAT_140099538 = uVar8;
  return;
}


/* ================= FUN_14000f180 @ 14000f180  size=41  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000ed20 */
/* CALLEES: FUN_1400029f0 */

undefined1 FUN_14000f180(int param_1)

{
  byte bVar1;
  
  if (param_1 != -1) {
    bVar1 = FUN_1400029f0(DAT_14286d3c8,param_1);
    if ((bVar1 & 4) != 0) {
      return 1;
    }
  }
  return 0;
}


/* ================= FUN_14000f1b0 @ 14000f1b0  size=41  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000ed20 */
/* CALLEES: FUN_1400029f0 */

undefined1 FUN_14000f1b0(int param_1)

{
  byte bVar1;
  
  if (param_1 != -1) {
    bVar1 = FUN_1400029f0(DAT_14286d3c8,param_1);
    if ((bVar1 & 2) != 0) {
      return 1;
    }
  }
  return 0;
}


/* ================= FUN_14000f1e0 @ 14000f1e0  size=41  callers=1 callees=0 ================= */
/* CALLERS: FUN_140011a80 */

undefined8 FUN_14000f1e0(undefined8 *param_1,int param_2,longlong param_3)

{
  if (((param_1 != (undefined8 *)0x0) && (param_3 != 0)) && (param_2 - 1U < 5)) {
    *param_1 = 7;
    *(int *)(param_1 + 1) = param_2;
    param_1[2] = param_3;
    return 1;
  }
  return 0;
}


/* ================= FUN_14000f210 @ 14000f210  size=41  callers=1 callees=0 ================= */
/* CALLERS: FUN_140011b20 */

undefined8 FUN_14000f210(undefined8 *param_1,int param_2,longlong param_3)

{
  if (((param_1 != (undefined8 *)0x0) && (param_3 != 0)) && (param_2 - 1U < 5)) {
    *param_1 = 8;
    *(int *)(param_1 + 1) = param_2;
    param_1[2] = param_3;
    return 1;
  }
  return 0;
}


/* ================= FUN_14000f240 @ 14000f240  size=37  callers=1 callees=0 ================= */
/* CALLERS: FUN_140011b80 */

undefined8 FUN_14000f240(undefined8 *param_1,int param_2,longlong param_3)

{
  if (((param_1 != (undefined8 *)0x0) && (-1 < param_2)) && (param_3 != 0)) {
    *param_1 = 0x33;
    *(int *)(param_1 + 1) = param_2;
    param_1[2] = param_3;
    return 1;
  }
  return 0;
}


/* ================= FUN_14000f270 @ 14000f270  size=37  callers=1 callees=0 ================= */
/* CALLERS: FUN_140011bd0 */

undefined8 FUN_14000f270(undefined8 *param_1,longlong param_2,int param_3)

{
  if (((param_1 != (undefined8 *)0x0) && (param_2 != 0)) && (param_3 != 0)) {
    param_1[1] = param_2;
    *param_1 = 0;
    *(int *)(param_1 + 2) = param_3;
    return 1;
  }
  return 0;
}


/* ================= FUN_14000f2a0 @ 14000f2a0  size=54  callers=1 callees=0 ================= */
/* CALLERS: FUN_140010610 */

void FUN_14000f2a0(longlong param_1,undefined4 param_2,longlong param_3,longlong *param_4)

{
  *param_4 = param_1;
  param_4[1] = param_1 + param_3;
  *(int *)((longlong)param_4 + 0x2c) = (int)param_3 * 2;
  *(undefined4 *)(param_4 + 5) = 0;
  *(undefined4 *)(param_4 + 7) = 0;
  param_4[8] = 0;
  param_4[9] = 0;
  param_4[10] = 0;
  *(undefined4 *)(param_4 + 2) = param_2;
  param_4[3] = param_1;
  *(int *)((longlong)param_4 + 0x3c) = (int)param_3;
  return;
}


/* ================= FUN_14000f2e0 @ 14000f2e0  size=165  callers=2 callees=2 ================= */
/* CALLERS: FUN_140010610, FUN_140010aa0 */
/* CALLEES: FUN_140010060, FUN_140010960 */

undefined8 FUN_14000f2e0(longlong param_1,int param_2)

{
  char *pcVar1;
  undefined8 uVar2;
  char cVar3;
  
  pcVar1 = *(char **)(param_1 + 0x18);
  cVar3 = *pcVar1;
  if (cVar3 == '_') {
    pcVar1 = pcVar1 + 1;
    *(char **)(param_1 + 0x18) = pcVar1;
    cVar3 = *pcVar1;
  }
  else if (param_2 != 0) {
    return 0;
  }
  if (cVar3 != 'Z') {
    return 0;
  }
  *(char **)(param_1 + 0x18) = pcVar1 + 1;
  uVar2 = FUN_140010960(param_1);
  if ((param_2 != 0) && ((*(byte *)(param_1 + 0x10) & 1) != 0)) {
    pcVar1 = *(char **)(param_1 + 0x18);
    cVar3 = *pcVar1;
    while ((cVar3 == '.' &&
           (((cVar3 = pcVar1[1], (byte)(cVar3 + 0x9fU) < 0x1a || (cVar3 == '_')) ||
            ((byte)(cVar3 - 0x30U) < 10))))) {
      uVar2 = FUN_140010060(param_1);
      pcVar1 = *(char **)(param_1 + 0x18);
      cVar3 = *pcVar1;
    }
  }
  return uVar2;
}


/* ================= FUN_14000f390 @ 14000f390  size=429  callers=1 callees=7 ================= */
/* CALLERS: FUN_140010610 */
/* CALLEES: FUN_140012520, FUN_140013ce0, FUN_140013e80, FUN_140014440, __chkstk, __security_check_cookie, malloc */

/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

bool FUN_14000f390(undefined4 param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  longlong lVar1;
  int iVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  undefined1 *puVar5;
  undefined1 *puVar6;
  undefined1 auStack_1c8 [32];
  undefined4 local_1a8 [4];
  undefined4 local_198 [80];
  undefined4 *local_58;
  int local_4c;
  undefined4 *local_48;
  int local_3c;
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)local_1a8;
  FUN_140013e80(local_1a8,param_3,param_4,param_2);
  uVar3 = (longlong)local_4c * 0x10 + 0x10;
  uVar4 = 0;
  if ((ulonglong)((longlong)local_4c * 0x10) < uVar3) {
    uVar4 = uVar3;
  }
  if (uVar4 == 0) {
    local_58 = (undefined4 *)0x0;
    puVar5 = auStack_1c8;
  }
  else if (uVar4 < 0x401) {
    uVar3 = uVar4 + 0xf;
    if (uVar3 <= uVar4) {
      uVar3 = 0xffffffffffffff0;
    }
    lVar1 = -(uVar3 & 0xfffffffffffffff0);
    local_58 = (undefined4 *)0x0;
    puVar5 = auStack_1c8 + lVar1;
    if ((undefined4 *)((longlong)local_1a8 + lVar1) != (undefined4 *)0x0) {
      *(undefined4 *)((longlong)local_1a8 + lVar1) = 0xcccc;
      local_58 = (undefined4 *)((longlong)local_198 + lVar1);
      puVar5 = auStack_1c8 + lVar1;
    }
  }
  else {
    local_58 = malloc(uVar4);
    puVar5 = auStack_1c8;
    if (local_58 != (undefined4 *)0x0) {
      *local_58 = 0xdddd;
      local_58 = local_58 + 4;
      puVar5 = auStack_1c8;
    }
  }
  uVar3 = (longlong)local_3c * 0x10 + 0x10;
  uVar4 = 0;
  if ((ulonglong)((longlong)local_3c * 0x10) < uVar3) {
    uVar4 = uVar3;
  }
  puVar6 = puVar5;
  if (uVar4 == 0) {
    local_48 = (undefined4 *)0x0;
  }
  else if (uVar4 < 0x401) {
    uVar3 = uVar4 + 0xf;
    if (uVar3 <= uVar4) {
      uVar3 = 0xffffffffffffff0;
    }
    *(undefined8 *)(puVar5 + -8) = 0x14000f4a5;
    lVar1 = -(uVar3 & 0xfffffffffffffff0);
    local_48 = (undefined4 *)0x0;
    puVar6 = puVar5 + lVar1;
    if ((undefined4 *)(puVar5 + lVar1 + 0x20) != (undefined4 *)0x0) {
      *(undefined4 *)(puVar5 + lVar1 + 0x20) = 0xcccc;
      local_48 = (undefined4 *)(puVar5 + lVar1 + 0x30);
      puVar6 = puVar5 + lVar1;
    }
  }
  else {
    *(undefined8 *)(puVar5 + -8) = 0x14000f4ce;
    local_48 = malloc(uVar4);
    if (local_48 != (undefined4 *)0x0) {
      *local_48 = 0xdddd;
      local_48 = local_48 + 4;
    }
  }
  *(undefined8 *)(puVar6 + -8) = 0x14000f4fb;
  FUN_140012520(local_1a8,param_1,param_2);
  *(undefined8 *)(puVar6 + -8) = 0x14000f504;
  FUN_140013ce0(local_1a8);
  *(undefined8 *)(puVar6 + -8) = 0x14000f50d;
  iVar2 = FUN_140014440(local_1a8);
  *(undefined8 *)(puVar6 + -8) = 0x14000f523;
  return iVar2 == 0;
}


/* ================= FUN_14000f540 @ 14000f540  size=1501  callers=14 callees=21 ================= */
/* CALLERS: FUN_14000f540, FUN_14000fe60, FUN_14000ff40, FUN_140010280, FUN_140010610, FUN_140010aa0, FUN_140010be0, FUN_140012070, FUN_1400121d0, FUN_1400122a0, FUN_140012310, FUN_1400145e0, FUN_140014be0, FUN_140014f40 */
/* CALLEES: FUN_14000f540, FUN_14000fcc0, FUN_14000fe60, FUN_14000ff20, FUN_140010030, FUN_1400103a0, FUN_140010bb0, FUN_140011320, FUN_140011960, FUN_1400119b0, FUN_140011b50, FUN_140011bd0, FUN_140011fc0, FUN_1400122a0, FUN_1400145b0, FUN_140014a60, FUN_140014c60, FUN_140014d50, FUN_140014f40, FUN_140015140, thunk_FUN_140011dd0 */

int * FUN_14000f540(longlong param_1)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  undefined8 *puVar4;
  int *piVar5;
  undefined8 uVar6;
  undefined **ppuVar7;
  int *local_res10;
  undefined1 local_28 [32];
  
  iVar3 = FUN_140015140();
  if (iVar3 != 0) {
    puVar4 = (undefined8 *)FUN_1400103a0(param_1,&local_res10,0);
    if (puVar4 != (undefined8 *)0x0) {
      if (**(char **)(param_1 + 0x18) == 'F') {
        piVar5 = (int *)FUN_140011320();
      }
      else {
        piVar5 = (int *)FUN_14000f540(param_1);
      }
      *puVar4 = piVar5;
      if (piVar5 != (int *)0x0) {
        if (*piVar5 - 0x1fU < 2) {
          uVar6 = *(undefined8 *)(piVar5 + 2);
          *(int **)(piVar5 + 2) = local_res10;
          local_res10 = (int *)*puVar4;
          *puVar4 = uVar6;
        }
        iVar3 = FUN_14000fcc0(param_1,local_res10);
        if (iVar3 != 0) {
          return local_res10;
        }
      }
    }
    goto switchD_14000f614_caseD_3a;
  }
  pcVar2 = *(char **)(param_1 + 0x18);
  switch((int)*pcVar2) {
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
  case 0x4e:
  case 0x5a:
    local_res10 = (int *)thunk_FUN_140011dd0(param_1);
    break;
  default:
    goto switchD_14000f614_caseD_3a;
  case 0x41:
    local_res10 = (int *)FUN_14000fe60(param_1);
    break;
  case 0x43:
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    uVar6 = FUN_14000f540(param_1);
    local_res10 = (int *)FUN_1400119b0(param_1,0x25,uVar6);
    break;
  case 0x44:
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    cVar1 = pcVar2[1];
    if (cVar1 != '\0') {
      *(char **)(param_1 + 0x18) = pcVar2 + 2;
    }
    switch(cVar1) {
    case 'F':
      local_res10 = (int *)FUN_140011b50(param_1);
      *local_res10 = 0x2c;
      if ((byte)(**(char **)(param_1 + 0x18) - 0x30U) < 10) {
        *(undefined2 *)(local_res10 + 4) = 1;
        FUN_140011fc0(param_1);
      }
      else {
        *(undefined2 *)(local_res10 + 4) = 0;
      }
      uVar6 = FUN_14000f540(param_1);
      piVar5 = local_res10;
      *(undefined8 *)(local_res10 + 2) = uVar6;
      if (*(longlong *)(local_res10 + 2) != 0) {
        FUN_140011fc0(param_1);
        cVar1 = **(char **)(param_1 + 0x18);
        if (cVar1 == '\0') {
          *(undefined2 *)((longlong)piVar5 + 0x12) = 0;
          return local_res10;
        }
        *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
        *(ushort *)((longlong)piVar5 + 0x12) = (ushort)(cVar1 == 's');
        return local_res10;
      }
    default:
      goto switchD_14000f614_caseD_3a;
    case 'T':
    case 't':
      uVar6 = FUN_140010bb0(param_1);
      local_res10 = (int *)FUN_1400119b0(param_1,0x43,uVar6);
      if (local_res10 != (int *)0x0) {
        cVar1 = **(char **)(param_1 + 0x18);
        if ((cVar1 == '\0') ||
           (*(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1, cVar1 != 'E')) {
          local_res10 = (int *)0x0;
        }
      }
      goto LAB_14000fac9;
    case 'a':
      piVar5 = (int *)FUN_140011bd0(param_1,&DAT_140060a28,4);
      return piVar5;
    case 'c':
      piVar5 = (int *)FUN_140011bd0(param_1,"decltype(auto)",0xe);
      return piVar5;
    case 'd':
      ppuVar7 = &PTR_s_decimal64_140060220;
      break;
    case 'e':
      ppuVar7 = &PTR_s_decimal128_140060240;
      break;
    case 'f':
      ppuVar7 = &PTR_s_decimal32_140060200;
      break;
    case 'h':
      ppuVar7 = &PTR_DAT_140060260;
      break;
    case 'i':
      ppuVar7 = &PTR_s_char32_t_1400602a0;
      break;
    case 'n':
      ppuVar7 = &PTR_s_decltype_nullptr__1400602c0;
      break;
    case 'p':
      uVar6 = FUN_14000f540(param_1);
      local_res10 = (int *)FUN_1400119b0(param_1,0x4b,uVar6);
      goto LAB_14000fac9;
    case 's':
      ppuVar7 = &PTR_s_char16_t_140060280;
      break;
    case 'v':
      local_res10 = (int *)FUN_140014f40(param_1);
      goto LAB_14000fac9;
    }
    piVar5 = (int *)FUN_140011960(param_1,ppuVar7);
    *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + *(int *)(*(longlong *)(piVar5 + 2) + 8);
    return piVar5;
  case 0x46:
    local_res10 = (int *)FUN_140011320(param_1);
    break;
  case 0x47:
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    uVar6 = FUN_14000f540(param_1);
    local_res10 = (int *)FUN_1400119b0(param_1,0x26,uVar6);
    break;
  case 0x4d:
    local_res10 = (int *)FUN_1400122a0(param_1);
    break;
  case 0x4f:
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    uVar6 = FUN_14000f540(param_1);
    local_res10 = (int *)FUN_1400119b0(param_1,0x24,uVar6);
    break;
  case 0x50:
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    uVar6 = FUN_14000f540(param_1);
    local_res10 = (int *)FUN_1400119b0(param_1,0x22,uVar6);
    break;
  case 0x52:
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    uVar6 = FUN_14000f540(param_1);
    local_res10 = (int *)FUN_1400119b0(param_1,0x23,uVar6);
    break;
  case 0x53:
    if (((byte)(pcVar2[1] - 0x30U) < 0x30) &&
       ((0x87fffffe03ffU >> ((ulonglong)(byte)(pcVar2[1] - 0x30U) & 0x3f) & 1) != 0)) {
      local_res10 = (int *)FUN_140014a60(param_1,0);
      if (**(char **)(param_1 + 0x18) != 'I') {
        return local_res10;
      }
      uVar6 = FUN_140014c60(param_1);
      local_res10 = (int *)FUN_1400119b0(param_1,4,local_res10,uVar6);
    }
    else {
      local_res10 = (int *)thunk_FUN_140011dd0(param_1);
      if ((local_res10 != (int *)0x0) && (*local_res10 == 0x18)) {
        return local_res10;
      }
    }
    break;
  case 0x54:
    local_res10 = (int *)FUN_140014d50(param_1);
    if (**(char **)(param_1 + 0x18) == 'I') {
      if (*(int *)(param_1 + 0x50) == 0) {
        iVar3 = FUN_14000fcc0(param_1,local_res10);
        if (iVar3 == 0) goto switchD_14000f614_caseD_3a;
        uVar6 = FUN_140014c60();
        local_res10 = (int *)FUN_1400119b0(param_1,4,local_res10,uVar6);
      }
      else {
        FUN_140010030(param_1,local_28);
        FUN_140014c60();
        if (**(char **)(param_1 + 0x18) == 'I') {
          iVar3 = FUN_14000fcc0(param_1,local_res10);
          if (iVar3 == 0) goto switchD_14000f614_caseD_3a;
          local_res10 = (int *)FUN_1400119b0();
        }
        else {
          FUN_14000ff20(param_1,local_28);
        }
      }
    }
    break;
  case 0x55:
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    local_res10 = (int *)FUN_1400145b0(param_1);
    if (**(char **)(param_1 + 0x18) == 'I') {
      uVar6 = FUN_140014c60(param_1);
      local_res10 = (int *)FUN_1400119b0(param_1,4,local_res10,uVar6);
    }
    uVar6 = FUN_14000f540(param_1);
    local_res10 = (int *)FUN_1400119b0(param_1,0x21,uVar6,local_res10);
    break;
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
  case 0x6c:
  case 0x6d:
  case 0x6e:
  case 0x6f:
  case 0x73:
  case 0x74:
  case 0x76:
  case 0x77:
  case 0x78:
  case 0x79:
  case 0x7a:
    piVar5 = (int *)FUN_140011960(param_1,&PTR_s_signed_char_14005fec0 +
                                          (longlong)(*pcVar2 + -0x61) * 4);
    *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + *(int *)(*(longlong *)(piVar5 + 2) + 8);
    *(longlong *)(param_1 + 0x18) = *(longlong *)(param_1 + 0x18) + 1;
    return piVar5;
  case 0x75:
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    uVar6 = FUN_1400145b0(param_1);
    local_res10 = (int *)FUN_1400119b0(param_1,0x28,uVar6);
  }
LAB_14000fac9:
  iVar3 = FUN_14000fcc0(param_1,local_res10);
  if (iVar3 == 0) {
switchD_14000f614_caseD_3a:
    local_res10 = (int *)0x0;
  }
  return local_res10;
}


/* ================= FUN_14000fc20 @ 14000fc20  size=19  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000eb40 */
/* CALLEES: FUN_140010580 */

void FUN_14000fc20(undefined8 param_1,undefined8 param_2)

{
  undefined1 local_res18 [16];
  
  FUN_140010580(param_1,param_2,local_res18);
  return;
}


/* ================= FUN_14000fc40 @ 14000fc40  size=123  callers=2 callees=2 ================= */
/* CALLERS: FUN_140014a60, FUN_140014e00 */
/* CALLEES: FUN_1400119b0, FUN_1400145b0 */

undefined8 FUN_14000fc40(longlong param_1,undefined8 param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  
  uVar1 = *(undefined8 *)(param_1 + 0x40);
  if (**(char **)(param_1 + 0x18) == 'B') {
    do {
      *(longlong *)(param_1 + 0x18) = *(longlong *)(param_1 + 0x18) + 1;
      uVar2 = FUN_1400145b0(param_1);
      param_2 = FUN_1400119b0(param_1,0x4c,param_2,uVar2);
    } while (**(char **)(param_1 + 0x18) == 'B');
    *(undefined8 *)(param_1 + 0x40) = uVar1;
    return param_2;
  }
  *(undefined8 *)(param_1 + 0x40) = uVar1;
  return param_2;
}


/* ================= FUN_14000fcc0 @ 14000fcc0  size=37  callers=6 callees=0 ================= */
/* CALLERS: FUN_14000f540, FUN_140011760, FUN_140011dd0, FUN_140012310, FUN_140014a60, FUN_140014d90 */

undefined8 FUN_14000fcc0(longlong param_1,longlong param_2)

{
  if ((param_2 != 0) && (*(int *)(param_1 + 0x38) < *(int *)(param_1 + 0x3c))) {
    *(longlong *)(*(longlong *)(param_1 + 0x30) + (longlong)*(int *)(param_1 + 0x38) * 8) = param_2;
    *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + 1;
    return 1;
  }
  return 0;
}


/* ================= FUN_14000fcf0 @ 14000fcf0  size=77  callers=3 callees=1 ================= */
/* CALLERS: FUN_14000fde0, FUN_1400125b0, FUN_140013cc0 */
/* CALLEES: FUN_14000fd40 */

void FUN_14000fcf0(undefined8 param_1,longlong param_2,ulonglong param_3)

{
  ulonglong uVar1;
  
  if (param_3 != 0) {
    uVar1 = 0;
    do {
      FUN_14000fd40(param_1,*(undefined1 *)(uVar1 + param_2));
      uVar1 = uVar1 + 1;
    } while (uVar1 < param_3);
  }
  return;
}


/* ================= FUN_14000fd40 @ 14000fd40  size=72  callers=10 callees=1 ================= */
/* CALLERS: FUN_14000fcf0, FUN_140011c70, FUN_140012440, FUN_1400125b0, FUN_140013bb0, FUN_140013d20, FUN_140013f30, FUN_140013ff0, FUN_1400142a0, FUN_140014450 */
/* CALLEES: FUN_140013ce0 */

void FUN_14000fd40(longlong param_1,undefined1 param_2)

{
  longlong lVar1;
  
  lVar1 = *(longlong *)(param_1 + 0x100);
  if (lVar1 == 0xff) {
    FUN_140013ce0();
    lVar1 = *(longlong *)(param_1 + 0x100);
  }
  *(undefined1 *)(lVar1 + param_1) = param_2;
  *(longlong *)(param_1 + 0x100) = *(longlong *)(param_1 + 0x100) + 1;
  *(undefined1 *)(param_1 + 0x108) = param_2;
  return;
}


/* ================= FUN_14000fd90 @ 14000fd90  size=76  callers=2 callees=3 ================= */
/* CALLERS: FUN_1400125b0, FUN_1400142a0 */
/* CALLEES: FUN_14000fde0, FUN_1400151c0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14000fd90(undefined8 param_1,undefined4 param_2)

{
  undefined1 auStack_58 [32];
  undefined1 local_38 [32];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_58;
  FUN_1400151c0(local_38,&DAT_140060a50,param_2);
  FUN_14000fde0(param_1,local_38);
  return;
}


/* ================= FUN_14000fde0 @ 14000fde0  size=31  callers=6 callees=1 ================= */
/* CALLERS: FUN_14000fd90, FUN_140011c70, FUN_140012440, FUN_1400125b0, FUN_140013ff0, FUN_1400142a0 */
/* CALLEES: FUN_14000fcf0 */

void FUN_14000fde0(undefined8 param_1,longlong param_2)

{
  longlong lVar1;
  
  lVar1 = -1;
  do {
    lVar1 = lVar1 + 1;
  } while (*(char *)(param_2 + lVar1) != '\0');
  FUN_14000fcf0();
  return;
}


/* ================= FUN_14000fe00 @ 14000fe00  size=85  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400125b0 */
/* CALLEES: FUN_140011220, FUN_1400121b0 */

int FUN_14000fe00(undefined8 param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  undefined8 uVar3;
  int iVar4;
  
  iVar4 = 0;
  for (; ((param_2 != (int *)0x0 && (*param_2 == 0x2f)) &&
         (piVar1 = *(int **)(param_2 + 2), piVar1 != (int *)0x0)); param_2 = *(int **)(param_2 + 4))
  {
    if (*piVar1 == 0x4b) {
      uVar3 = FUN_140011220(param_1,*(undefined8 *)(piVar1 + 2));
      iVar2 = FUN_1400121b0(uVar3);
    }
    else {
      iVar2 = 1;
    }
    iVar4 = iVar4 + iVar2;
  }
  return iVar4;
}


/* ================= FUN_14000fe60 @ 14000fe60  size=188  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000f540 */
/* CALLEES: FUN_14000f540, FUN_140010bb0, FUN_1400119b0, FUN_140011bd0 */

undefined8 FUN_14000fe60(longlong param_1)

{
  char *pcVar1;
  char *pcVar2;
  longlong lVar3;
  undefined8 uVar4;
  
  if (**(char **)(param_1 + 0x18) == 'A') {
    pcVar1 = *(char **)(param_1 + 0x18) + 1;
    *(char **)(param_1 + 0x18) = pcVar1;
    if (*pcVar1 == '_') {
      lVar3 = 0;
    }
    else {
      pcVar2 = pcVar1;
      if ((byte)(*pcVar1 - 0x30U) < 10) {
        do {
          pcVar2 = pcVar2 + 1;
          *(char **)(param_1 + 0x18) = pcVar2;
          if (*pcVar2 < '0') break;
        } while (*pcVar2 < ':');
        lVar3 = FUN_140011bd0(param_1,pcVar1,(int)pcVar2 - (int)pcVar1);
      }
      else {
        lVar3 = FUN_140010bb0(param_1);
      }
      if (lVar3 == 0) {
        return 0;
      }
    }
    if (**(char **)(param_1 + 0x18) == '_') {
      *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
      uVar4 = FUN_14000f540(param_1);
      uVar4 = FUN_1400119b0(param_1,0x2a,lVar3,uVar4);
      return uVar4;
    }
  }
  return 0;
}


/* ================= FUN_14000ff20 @ 14000ff20  size=26  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000f540 */

void FUN_14000ff20(longlong param_1,undefined8 *param_2)

{
  *(undefined8 *)(param_1 + 0x18) = *param_2;
  *(undefined4 *)(param_1 + 0x28) = *(undefined4 *)(param_2 + 1);
  *(undefined4 *)(param_1 + 0x38) = *(undefined4 *)((longlong)param_2 + 0xc);
  *(undefined4 *)(param_1 + 0x48) = *(undefined4 *)(param_2 + 2);
  return;
}


/* ================= FUN_14000ff40 @ 14000ff40  size=114  callers=2 callees=3 ================= */
/* CALLERS: FUN_140010960, FUN_140011320 */
/* CALLEES: FUN_14000f540, FUN_1400119b0, FUN_1400121d0 */

void FUN_14000ff40(longlong param_1,int param_2)

{
  longlong lVar1;
  longlong lVar2;
  
  if (**(char **)(param_1 + 0x18) == 'J') {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
  }
  else if (param_2 == 0) {
    lVar1 = 0;
    goto LAB_14000ff7d;
  }
  lVar1 = FUN_14000f540();
  if (lVar1 == 0) {
    return;
  }
LAB_14000ff7d:
  lVar2 = FUN_1400121d0(param_1);
  if (lVar2 == 0) {
    return;
  }
  FUN_1400119b0(param_1,0x29,lVar1,lVar2);
  return;
}


/* ================= FUN_14000ffc0 @ 14000ffc0  size=110  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400145e0 */
/* CALLEES: FUN_140011fc0 */

undefined8 FUN_14000ffc0(longlong param_1,int param_2)

{
  char cVar1;
  
  if (param_2 == 0) {
    cVar1 = **(char **)(param_1 + 0x18);
    param_2 = (int)cVar1;
    if (cVar1 == '\0') {
      return 0;
    }
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
  }
  if (param_2 != 0x68) {
    if (param_2 != 0x76) {
      return 0;
    }
    FUN_140011fc0(param_1);
    if (**(char **)(param_1 + 0x18) != '_') {
      return 0;
    }
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
  }
  FUN_140011fc0(param_1);
  if (**(char **)(param_1 + 0x18) != '_') {
    return 0;
  }
  *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
  return 1;
}


/* ================= FUN_140010030 @ 140010030  size=26  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000f540 */

void FUN_140010030(longlong param_1,undefined8 *param_2)

{
  *param_2 = *(undefined8 *)(param_1 + 0x18);
  *(undefined4 *)(param_2 + 1) = *(undefined4 *)(param_1 + 0x28);
  *(undefined4 *)((longlong)param_2 + 0xc) = *(undefined4 *)(param_1 + 0x38);
  *(undefined4 *)(param_2 + 2) = *(undefined4 *)(param_1 + 0x48);
  return;
}


/* ================= FUN_140010060 @ 140010060  size=200  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000f2e0 */
/* CALLEES: FUN_1400119b0, FUN_140011bd0 */

void FUN_140010060(longlong param_1,undefined8 param_2)

{
  byte *pbVar1;
  char cVar2;
  byte bVar3;
  char *pcVar4;
  char *pcVar5;
  undefined8 uVar6;
  
  pcVar4 = *(char **)(param_1 + 0x18);
  pcVar5 = pcVar4;
  if ((*pcVar4 == '.') && (((byte)(pcVar4[1] + 0x9fU) < 0x1a || (pcVar4[1] == '_')))) {
    for (pcVar5 = pcVar4 + 2; ((byte)(*pcVar5 + 0x9fU) < 0x1a || (*pcVar5 == '_'));
        pcVar5 = pcVar5 + 1) {
    }
  }
  cVar2 = *pcVar5;
  while ((cVar2 == '.' && ((byte)(pcVar5[1] - 0x30U) < 10))) {
    pbVar1 = (byte *)(pcVar5 + 2);
    pcVar5 = pcVar5 + 2;
    bVar3 = *pbVar1;
    while (('/' < (char)bVar3 && (bVar3 < 0x3a))) {
      pbVar1 = (byte *)(pcVar5 + 1);
      pcVar5 = pcVar5 + 1;
      bVar3 = *pbVar1;
    }
    cVar2 = *pcVar5;
  }
  *(char **)(param_1 + 0x18) = pcVar5;
  uVar6 = FUN_140011bd0(param_1,pcVar4,(int)pcVar5 - (int)pcVar4);
  FUN_1400119b0(param_1,0x4e,param_2,uVar6);
  return;
}


/* ================= FUN_140010130 @ 140010130  size=76  callers=5 callees=1 ================= */
/* CALLERS: FUN_140010be0, FUN_140011760, FUN_140011810, FUN_140014d50, FUN_140014d90 */
/* CALLEES: FUN_140011fc0 */

int FUN_140010130(longlong param_1)

{
  int iVar1;
  
  if (**(char **)(param_1 + 0x18) == '_') {
    iVar1 = 0;
  }
  else {
    if (**(char **)(param_1 + 0x18) == 'n') {
      return -1;
    }
    iVar1 = FUN_140011fc0(param_1);
    iVar1 = iVar1 + 1;
    if (iVar1 < 0) {
      return -1;
    }
  }
  if (**(char **)(param_1 + 0x18) != '_') {
    return -1;
  }
  *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
  return iVar1;
}


/* ================= FUN_140010180 @ 140010180  size=145  callers=2 callees=1 ================= */
/* CALLERS: FUN_140010180, FUN_140013e80 */
/* CALLEES: FUN_140010180 */

void FUN_140010180(int *param_1,int *param_2,undefined4 *param_3)

{
  if (param_3 != (undefined4 *)0x0) {
    do {
      switch(*param_3) {
      case 1:
      case 2:
      case 3:
      case 9:
      case 10:
      case 0xb:
      case 0xc:
      case 0xd:
      case 0xe:
      case 0xf:
      case 0x10:
      case 0x11:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
      case 0x16:
      case 0x17:
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
      case 0x25:
      case 0x26:
      case 0x28:
      case 0x29:
      case 0x2a:
      case 0x2b:
      case 0x2d:
      case 0x2e:
      case 0x2f:
      case 0x30:
      case 0x31:
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
      case 0x43:
      case 0x49:
      case 0x4a:
      case 0x4b:
      case 0x4c:
      case 0x4d:
      case 0x4e:
      case 0x4f:
      case 0x50:
        break;
      case 4:
        *param_1 = *param_1 + 1;
        break;
      default:
        goto switchD_1400101cd_caseD_5;
      case 7:
      case 8:
      case 0x33:
        goto switchD_1400101cd_caseD_7;
      case 0x23:
      case 0x24:
        if (**(int **)(param_3 + 2) == 5) {
          *param_2 = *param_2 + 1;
        }
        break;
      case 0x2c:
      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47:
        param_3 = *(undefined4 **)(param_3 + 2);
        goto LAB_1400101f7;
      }
      FUN_140010180(param_1,param_2,*(undefined8 *)(param_3 + 2));
switchD_1400101cd_caseD_7:
      param_3 = *(undefined4 **)(param_3 + 4);
LAB_1400101f7:
    } while (param_3 != (undefined4 *)0x0);
switchD_1400101cd_caseD_5:
  }
  return;
}


/* ================= FUN_140010280 @ 140010280  size=286  callers=1 callees=3 ================= */
/* CALLERS: FUN_140014e00 */
/* CALLEES: FUN_14000f540, FUN_140011a80, FUN_140011b20 */

undefined8 FUN_140010280(longlong param_1)

{
  char *pcVar1;
  char cVar2;
  char cVar3;
  int *piVar4;
  undefined8 uVar5;
  char *pcVar6;
  undefined4 uVar7;
  
  piVar4 = *(int **)(param_1 + 0x40);
  if ((piVar4 != (int *)0x0) && ((*piVar4 == 0 || (*piVar4 == 0x18)))) {
    *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + piVar4[4];
  }
  pcVar6 = *(char **)(param_1 + 0x18);
  if (*pcVar6 != 'C') {
    if (*pcVar6 != 'D') {
      return 0;
    }
    cVar2 = pcVar6[1];
    if (cVar2 == '0') {
      uVar7 = 1;
    }
    else if (cVar2 == '1') {
      uVar7 = 2;
    }
    else if (cVar2 == '2') {
      uVar7 = 3;
    }
    else if (cVar2 == '4') {
      uVar7 = 4;
    }
    else {
      if (cVar2 != '5') {
        return 0;
      }
      uVar7 = 5;
    }
    *(char **)(param_1 + 0x18) = pcVar6 + 2;
    uVar5 = FUN_140011b20(param_1,uVar7);
    return uVar5;
  }
  pcVar1 = pcVar6 + 1;
  cVar2 = *pcVar1;
  uVar7 = 1;
  if (cVar2 == 'I') {
    *(char **)(param_1 + 0x18) = pcVar1;
    pcVar6 = pcVar1;
  }
  cVar3 = pcVar6[1];
  if (cVar3 != '1') {
    if (cVar3 == '2') {
      uVar7 = 2;
    }
    else if (cVar3 == '3') {
      uVar7 = 3;
    }
    else if (cVar3 == '4') {
      uVar7 = 4;
    }
    else {
      if (cVar3 != '5') {
        return 0;
      }
      uVar7 = 5;
    }
  }
  *(char **)(param_1 + 0x18) = pcVar6 + 2;
  if (cVar2 == 'I') {
    FUN_14000f540(param_1);
  }
  uVar5 = FUN_140011a80(param_1,uVar7,*(undefined8 *)(param_1 + 0x40));
  return uVar5;
}


/* ================= FUN_1400103a0 @ 1400103a0  size=465  callers=2 callees=4 ================= */
/* CALLERS: FUN_14000f540, FUN_140011f10 */
/* CALLEES: FUN_140010bb0, FUN_1400119b0, FUN_1400121d0, FUN_140015140 */

longlong * FUN_1400103a0(longlong param_1,longlong *param_2,int param_3)

{
  longlong *plVar1;
  byte bVar2;
  int *piVar3;
  int iVar4;
  longlong lVar5;
  undefined4 uVar7;
  char cVar8;
  ulonglong uVar9;
  byte *pbVar10;
  longlong lVar6;
  
  pbVar10 = *(byte **)(param_1 + 0x18);
  uVar9 = (ulonglong)*pbVar10;
  iVar4 = FUN_140015140();
  cVar8 = (char)uVar9;
  plVar1 = param_2;
  do {
    if (iVar4 == 0) {
      if ((param_3 == 0) && (cVar8 == 'F')) {
        for (; param_2 != plVar1; param_2 = (longlong *)(*param_2 + 8)) {
          piVar3 = (int *)*param_2;
          iVar4 = *piVar3;
          if (iVar4 == 0x19) {
            *piVar3 = 0x1c;
          }
          else if (iVar4 == 0x1a) {
            *piVar3 = 0x1d;
          }
          else if (iVar4 == 0x1b) {
            *piVar3 = 0x1e;
          }
        }
      }
      return plVar1;
    }
    lVar6 = 0;
    lVar5 = 0;
    *(byte **)(param_1 + 0x18) = pbVar10 + 1;
    cVar8 = (char)uVar9;
    if (cVar8 == 'r') {
      uVar7 = 0x19;
      if (param_3 != 0) {
        uVar7 = 0x1c;
      }
      *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + 9;
      lVar5 = lVar6;
    }
    else if (cVar8 == 'V') {
      uVar7 = 0x1a;
      if (param_3 != 0) {
        uVar7 = 0x1d;
      }
      *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + 9;
      lVar5 = lVar6;
    }
    else if (cVar8 == 'K') {
      uVar7 = 0x1b;
      if (param_3 != 0) {
        uVar7 = 0x1e;
      }
      *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + 6;
      lVar5 = lVar6;
    }
    else {
      bVar2 = pbVar10[1];
      if (bVar2 == 0) {
        return (longlong *)0x0;
      }
      *(byte **)(param_1 + 0x18) = pbVar10 + 2;
      if (bVar2 == 0x78) {
        *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + 0x11;
        uVar7 = 0x4d;
      }
      else if ((bVar2 + 0xb1 & 0xdf) == 0) {
        *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + 9;
        uVar7 = 0x4f;
        if (bVar2 == 0x4f) {
          lVar5 = FUN_140010bb0(param_1);
          if (lVar5 == 0) {
            return (longlong *)0x0;
          }
          if (**(char **)(param_1 + 0x18) != 'E') {
            return (longlong *)0x0;
          }
          *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
        }
      }
      else {
        if (bVar2 != 0x77) {
          return (longlong *)0x0;
        }
        *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + 6;
        uVar7 = 0x50;
        lVar5 = FUN_1400121d0(param_1);
        if (lVar5 == 0) {
          return (longlong *)0x0;
        }
        if (**(char **)(param_1 + 0x18) != 'E') {
          return (longlong *)0x0;
        }
        *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
      }
    }
    lVar5 = FUN_1400119b0(param_1,uVar7,0,lVar5);
    *plVar1 = lVar5;
    if (lVar5 == 0) {
      return (longlong *)0x0;
    }
    pbVar10 = *(byte **)(param_1 + 0x18);
    plVar1 = (longlong *)(lVar5 + 8);
    uVar9 = (ulonglong)*pbVar10;
    iVar4 = FUN_140015140(param_1);
    cVar8 = (char)uVar9;
  } while( true );
}


/* ================= FUN_140010580 @ 140010580  size=140  callers=1 callees=3 ================= */
/* CALLERS: FUN_14000fc20 */
/* CALLEES: FUN_140010610, FUN_140011490, free */

void * FUN_140010580(undefined8 param_1,undefined4 param_2,undefined8 *param_3)

{
  int iVar1;
  void *local_28 [2];
  undefined8 local_18;
  int local_10;
  
  FUN_140011490(local_28,0);
  iVar1 = FUN_140010610(param_1,param_2,&LAB_140011470,local_28);
  if (iVar1 == 0) {
    free(local_28[0]);
    *param_3 = 0;
    return (void *)0x0;
  }
  if (local_10 != 0) {
    local_18 = 1;
  }
  *param_3 = local_18;
  return local_28[0];
}


/* ================= FUN_140010610 @ 140010610  size=712  callers=1 callees=11 ================= */
/* CALLERS: FUN_140010580 */
/* CALLEES: FUN_14000f2a0, FUN_14000f2e0, FUN_14000f390, FUN_14000f540, FUN_1400119b0, FUN_140011ae0, __chkstk, __security_check_cookie, abort, malloc, strncmp */

/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

ulonglong FUN_140010610(char *param_1,uint param_2,undefined8 param_3,undefined8 param_4)

{
  int iVar1;
  uint uVar2;
  ulonglong uVar3;
  undefined8 uVar4;
  ulonglong uVar5;
  ulonglong uVar6;
  longlong lVar7;
  undefined1 *puVar8;
  undefined1 *puVar9;
  char cVar10;
  longlong lVar11;
  char cVar12;
  undefined1 auStack_b8 [32];
  undefined4 local_98 [4];
  undefined4 local_88 [2];
  char *local_80;
  undefined4 *local_78;
  uint local_6c;
  undefined4 *local_68;
  int local_5c;
  ulonglong local_38;
  
  puVar9 = auStack_b8;
  local_38 = DAT_140096fe8 ^ (ulonglong)local_98;
  uVar5 = 0;
  cVar10 = '\0';
  if ((*param_1 == '_') && (param_1[1] == 'Z')) {
    cVar10 = '\x01';
LAB_1400106c7:
    lVar7 = -1;
    lVar11 = -1;
    do {
      lVar11 = lVar11 + 1;
    } while (param_1[lVar11] != '\0');
    FUN_14000f2a0(param_1,param_2);
    if (((param_2 >> 0x12 & 1) != 0) || (local_6c < 0x801)) {
      uVar3 = (longlong)(int)local_6c * 0x18 + 0x10;
      uVar6 = uVar5;
      if ((ulonglong)((longlong)(int)local_6c * 0x18) < uVar3) {
        uVar6 = uVar3;
      }
      if (uVar6 == 0) {
        local_78 = (undefined4 *)0x0;
        puVar8 = auStack_b8;
      }
      else if (uVar6 < 0x401) {
        uVar3 = uVar6 + 0xf;
        if (uVar3 <= uVar6) {
          uVar3 = 0xffffffffffffff0;
        }
        lVar11 = -(uVar3 & 0xfffffffffffffff0);
        local_78 = (undefined4 *)0x0;
        puVar8 = auStack_b8 + lVar11;
        if ((undefined4 *)((longlong)local_98 + lVar11) != (undefined4 *)0x0) {
          *(undefined4 *)((longlong)local_98 + lVar11) = 0xcccc;
          local_78 = (undefined4 *)((longlong)local_88 + lVar11);
          puVar8 = auStack_b8 + lVar11;
        }
      }
      else {
        local_78 = malloc(uVar6);
        puVar8 = auStack_b8;
        if (local_78 != (undefined4 *)0x0) {
          *local_78 = 0xdddd;
          local_78 = local_78 + 4;
          puVar8 = auStack_b8;
        }
      }
      uVar3 = (longlong)local_5c * 8 + 0x10;
      uVar6 = uVar5;
      if ((ulonglong)((longlong)local_5c * 8) < uVar3) {
        uVar6 = uVar3;
      }
      puVar9 = puVar8;
      if (uVar6 == 0) {
        local_68 = (undefined4 *)0x0;
      }
      else if (uVar6 < 0x401) {
        uVar3 = uVar6 + 0xf;
        if (uVar3 <= uVar6) {
          uVar3 = 0xffffffffffffff0;
        }
        *(undefined8 *)(puVar8 + -8) = 0x1400107ba;
        lVar11 = -(uVar3 & 0xfffffffffffffff0);
        local_68 = (undefined4 *)0x0;
        puVar9 = puVar8 + lVar11;
        if ((undefined4 *)(puVar8 + lVar11 + 0x20) != (undefined4 *)0x0) {
          *(undefined4 *)(puVar8 + lVar11 + 0x20) = 0xcccc;
          local_68 = (undefined4 *)(puVar8 + lVar11 + 0x30);
          puVar9 = puVar8 + lVar11;
        }
      }
      else {
        *(undefined8 *)(puVar8 + -8) = 0x1400107e0;
        local_68 = malloc(uVar6);
        if (local_68 != (undefined4 *)0x0) {
          *local_68 = 0xdddd;
          local_68 = local_68 + 4;
        }
      }
      if (cVar10 == '\0') {
        *(undefined8 *)(puVar9 + -8) = 0x14001087b;
        lVar11 = FUN_14000f540(local_98);
      }
      else if (cVar10 == '\x01') {
        *(undefined8 *)(puVar9 + -8) = 0x140010870;
        lVar11 = FUN_14000f2e0(local_98,1);
      }
      else {
        if ((cVar10 != '\x02') && (cVar10 != '\x03')) {
                    /* WARNING: Subroutine does not return */
          *(undefined **)(puVar9 + -8) = &UNK_140010814;
          abort();
        }
        local_80 = local_80 + 0xb;
        *(undefined8 *)(puVar9 + -8) = 0x14001082a;
        uVar4 = FUN_140011ae0(local_98);
        cVar12 = '\0';
        *(undefined8 *)(puVar9 + -8) = 0x140010845;
        lVar11 = FUN_1400119b0(local_98,(cVar10 != '\x02') + 'D',uVar4);
        do {
          lVar7 = lVar7 + 1;
        } while (local_80[lVar7] != cVar12);
        local_80 = local_80 + lVar7;
      }
      if ((((param_2 & 1) == 0) || (*local_80 == '\0')) && (lVar11 != 0)) {
        *(undefined8 *)(puVar9 + -8) = 0x1400108a0;
        uVar2 = FUN_14000f390(param_2,lVar11,param_3,param_4);
        uVar5 = (ulonglong)uVar2;
      }
      goto LAB_1400108aa;
    }
  }
  else {
    iVar1 = strncmp(param_1,"_GLOBAL_",8);
    if ((((iVar1 == 0) &&
         (((byte)(param_1[8] - 0x24U) < 0x3c &&
          ((0x800000000000401U >> ((ulonglong)(byte)(param_1[8] - 0x24U) & 0x3f) & 1) != 0)))) &&
        ((cVar12 = param_1[9], cVar12 == 'D' || (cVar12 == 'I')))) && (param_1[10] == '_')) {
      cVar10 = (cVar12 != 'I') + '\x02';
      goto LAB_1400106c7;
    }
    if ((param_2 & 0x10) != 0) goto LAB_1400106c7;
  }
  uVar5 = 0;
LAB_1400108aa:
  *(undefined8 *)(puVar9 + -8) = 0x1400108b6;
  return uVar5;
}


/* ================= FUN_1400108e0 @ 1400108e0  size=115  callers=2 callees=1 ================= */
/* CALLERS: FUN_140011810, FUN_140014e00 */
/* CALLEES: FUN_140011fc0 */

undefined8 FUN_1400108e0(longlong param_1)

{
  char *pcVar1;
  int iVar2;
  uint uVar3;
  
  pcVar1 = *(char **)(param_1 + 0x18);
  uVar3 = 1;
  if (*pcVar1 == '_') {
    *(char **)(param_1 + 0x18) = pcVar1 + 1;
    if (pcVar1[1] == '_') {
      uVar3 = 2;
      *(char **)(param_1 + 0x18) = pcVar1 + 2;
    }
    iVar2 = FUN_140011fc0();
    if (iVar2 < 0) {
      return 0;
    }
    if ((1 < uVar3) && (9 < iVar2)) {
      if (**(char **)(param_1 + 0x18) != '_') {
        return 0;
      }
      *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    }
  }
  return 1;
}


/* ================= FUN_140010960 @ 140010960  size=318  callers=4 callees=6 ================= */
/* CALLERS: FUN_14000f2e0, FUN_140011810, FUN_140011ae0, FUN_1400145e0 */
/* CALLEES: FUN_14000ff40, FUN_1400119b0, FUN_140011dd0, FUN_1400145e0, FUN_140014fc0, FUN_1400150d0 */

int * FUN_140010960(longlong param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int *piVar5;
  undefined4 *puVar6;
  
  if ((**(char **)(param_1 + 0x18) == 'G') || (**(char **)(param_1 + 0x18) == 'T')) {
    piVar4 = (int *)FUN_1400145e0(param_1);
  }
  else {
    piVar4 = (int *)FUN_140011dd0(param_1);
    if (piVar4 != (int *)0x0) {
      if ((param_2 == 0) || ((*(byte *)(param_1 + 0x10) & 1) != 0)) {
        if ((**(char **)(param_1 + 0x18) != '\0') && (**(char **)(param_1 + 0x18) != 'E')) {
          uVar3 = FUN_140014fc0(piVar4);
          piVar5 = (int *)FUN_14000ff40(param_1,uVar3);
          if (piVar5 == (int *)0x0) {
            return (int *)0x0;
          }
          if (((param_2 == 0) && (*piVar4 == 2)) && (*piVar5 == 0x29)) {
            piVar5[2] = 0;
            piVar5[3] = 0;
          }
          piVar4 = (int *)FUN_1400119b0(param_1,3,piVar4,piVar5);
        }
      }
      else {
        iVar2 = *piVar4;
        iVar1 = FUN_1400150d0(iVar2);
        while (iVar1 != 0) {
          piVar4 = *(int **)(piVar4 + 2);
          iVar2 = *piVar4;
          iVar1 = FUN_1400150d0(iVar2);
        }
        if (iVar2 == 2) {
          puVar6 = *(undefined4 **)(piVar4 + 4);
          iVar2 = FUN_1400150d0(*puVar6);
          if (iVar2 != 0) {
            do {
              puVar6 = *(undefined4 **)(puVar6 + 2);
              *(undefined4 **)(piVar4 + 4) = puVar6;
              iVar2 = FUN_1400150d0(*puVar6);
            } while (iVar2 != 0);
            return piVar4;
          }
        }
      }
    }
  }
  return piVar4;
}


/* ================= FUN_140010aa0 @ 140010aa0  size=260  callers=2 callees=4 ================= */
/* CALLERS: FUN_140010be0, FUN_140014be0 */
/* CALLEES: FUN_14000f2e0, FUN_14000f540, FUN_1400119b0, FUN_140011bd0 */

undefined8 FUN_140010aa0(longlong param_1)

{
  char *pcVar1;
  char *pcVar2;
  int *piVar3;
  char *pcVar4;
  undefined8 uVar5;
  char cVar6;
  undefined8 uVar7;
  
  if (**(char **)(param_1 + 0x18) == 'L') {
    pcVar2 = *(char **)(param_1 + 0x18) + 1;
    *(char **)(param_1 + 0x18) = pcVar2;
    cVar6 = *pcVar2;
    if ((cVar6 == '_') || (cVar6 == 'Z')) {
      uVar7 = FUN_14000f2e0(param_1,0);
    }
    else {
      piVar3 = (int *)FUN_14000f540(param_1);
      if (piVar3 == (int *)0x0) {
        return 0;
      }
      if ((*piVar3 == 0x27) && (*(int *)(*(longlong *)(piVar3 + 2) + 0x1c) != 0)) {
        *(int *)(param_1 + 0x48) =
             *(int *)(param_1 + 0x48) - *(int *)(*(longlong *)(piVar3 + 2) + 8);
      }
      pcVar2 = *(char **)(param_1 + 0x18);
      uVar7 = 0x3d;
      cVar6 = *pcVar2;
      if (cVar6 == 'n') {
        pcVar2 = pcVar2 + 1;
        uVar7 = 0x3e;
        *(char **)(param_1 + 0x18) = pcVar2;
        cVar6 = *pcVar2;
      }
      pcVar4 = (char *)((ulonglong)pcVar2 & 0xffffffff);
      pcVar1 = pcVar2;
      while (cVar6 != 'E') {
        if (cVar6 == '\0') {
          return 0;
        }
        pcVar4 = pcVar1 + 1;
        *(char **)(param_1 + 0x18) = pcVar4;
        pcVar1 = pcVar4;
        cVar6 = *pcVar4;
      }
      uVar5 = FUN_140011bd0(param_1,pcVar2,(int)pcVar4 - (int)pcVar2);
      uVar7 = FUN_1400119b0(param_1,uVar7,piVar3,uVar5);
    }
    if (**(char **)(param_1 + 0x18) == 'E') {
      *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
      return uVar7;
    }
  }
  return 0;
}


/* ================= FUN_140010bb0 @ 140010bb0  size=42  callers=6 callees=1 ================= */
/* CALLERS: FUN_14000f540, FUN_14000fe60, FUN_1400103a0, FUN_140011150, FUN_140014be0, FUN_140014f40 */
/* CALLEES: FUN_140010be0 */

void FUN_140010bb0(longlong param_1)

{
  undefined4 uVar1;
  
  uVar1 = *(undefined4 *)(param_1 + 0x4c);
  *(undefined4 *)(param_1 + 0x4c) = 1;
  FUN_140010be0();
  *(undefined4 *)(param_1 + 0x4c) = uVar1;
  return;
}


/* ================= FUN_140010be0 @ 140010be0  size=1389  callers=2 callees=13 ================= */
/* CALLERS: FUN_140010bb0, FUN_140010be0 */
/* CALLEES: FUN_14000f540, FUN_140010130, FUN_140010aa0, FUN_140010be0, FUN_140011150, FUN_1400119b0, FUN_140011bb0, FUN_140012070, FUN_140014c60, FUN_140014c80, FUN_140014d50, FUN_140014e00, FUN_140015190 */

longlong FUN_140010be0(longlong param_1)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  int *piVar7;
  int iVar8;
  uint7 uVar9;
  char *pcVar10;
  char *pcVar11;
  
  pcVar10 = *(char **)(param_1 + 0x18);
  cVar1 = *pcVar10;
  if (cVar1 == 'L') {
    lVar3 = FUN_140010aa0(param_1);
    return lVar3;
  }
  if (cVar1 == 'T') {
    lVar3 = FUN_140014d50(param_1);
    return lVar3;
  }
  if (cVar1 == 's') {
    if (pcVar10[1] == 'r') {
      *(char **)(param_1 + 0x18) = pcVar10 + 2;
      uVar4 = FUN_14000f540(param_1);
      uVar5 = FUN_140014e00(param_1);
      if (**(char **)(param_1 + 0x18) == 'I') {
        uVar6 = FUN_140014c60(param_1);
        uVar5 = FUN_1400119b0(param_1,4,uVar5,uVar6);
        lVar3 = FUN_1400119b0(param_1,1,uVar4,uVar5);
        return lVar3;
      }
      lVar3 = FUN_1400119b0(param_1,1,uVar4,uVar5);
      return lVar3;
    }
    if (pcVar10[1] == 'p') {
      *(char **)(param_1 + 0x18) = pcVar10 + 2;
      uVar4 = FUN_140010be0(param_1);
      lVar3 = FUN_1400119b0(param_1,0x4b,uVar4);
      return lVar3;
    }
  }
  else if (cVar1 == 'f') {
    if (pcVar10[1] == 'p') {
      *(char **)(param_1 + 0x18) = pcVar10 + 2;
      if (pcVar10[2] == 'T') {
        *(char **)(param_1 + 0x18) = pcVar10 + 3;
        lVar3 = FUN_140011bb0(param_1,0);
        return lVar3;
      }
      iVar2 = FUN_140010130(param_1);
      if ((iVar2 + 1U & 0x7fffffff) == 0) {
        return 0;
      }
      lVar3 = FUN_140011bb0(param_1,iVar2 + 1);
      return lVar3;
    }
  }
  else {
    if ((byte)(cVar1 - 0x30U) < 10) {
LAB_140010d51:
      lVar3 = FUN_140014e00(param_1);
      if (lVar3 == 0) {
        return 0;
      }
      if (**(char **)(param_1 + 0x18) == 'I') {
        uVar4 = FUN_140014c60(param_1);
        lVar3 = FUN_1400119b0(param_1,4,lVar3,uVar4);
        return lVar3;
      }
      return lVar3;
    }
    if (cVar1 == 'o') {
      if (pcVar10[1] == 'n') {
        *(char **)(param_1 + 0x18) = pcVar10 + 2;
        goto LAB_140010d51;
      }
    }
    else if (((cVar1 == 'i') || (cVar1 == 't')) && (pcVar10[1] == 'l')) {
      pcVar10 = pcVar10 + 2;
      uVar4 = 0;
      *(char **)(param_1 + 0x18) = pcVar10;
      if (cVar1 == 't') {
        uVar4 = FUN_14000f540(param_1);
        pcVar10 = *(char **)(param_1 + 0x18);
      }
      if (pcVar10[1] == '\0') {
        return 0;
      }
      uVar5 = FUN_140011150(param_1,0x45);
      lVar3 = FUN_1400119b0(param_1,0x31,uVar4,uVar5);
      return lVar3;
    }
  }
  pcVar10 = (char *)0x0;
  piVar7 = (int *)FUN_140012070(param_1);
  if (piVar7 == (int *)0x0) {
    return 0;
  }
  pcVar11 = pcVar10;
  if (*piVar7 == 0x32) {
    pcVar11 = (char *)**(undefined8 **)(piVar7 + 2);
    *(int *)(param_1 + 0x48) =
         *(int *)(param_1 + 0x48) + *(int *)(*(undefined8 **)(piVar7 + 2) + 2) + -2;
    if (((*pcVar11 == 's') && (pcVar11[1] == 't')) && (pcVar11[2] == '\0')) {
      uVar4 = FUN_14000f540(param_1);
      goto LAB_140011121;
    }
  }
  iVar2 = *piVar7;
  uVar9 = (uint7)(uint3)((uint)iVar2 >> 8);
  if (iVar2 == 0x32) {
    iVar8 = *(int *)(*(longlong *)(piVar7 + 2) + 0x14);
LAB_140010e72:
    if (iVar8 == 0) {
      lVar3 = FUN_1400119b0(param_1,0x36,piVar7);
      return lVar3;
    }
    if (iVar8 != 1) {
      if (iVar8 == 2) {
        if (pcVar11 != (char *)0x0) {
          iVar2 = FUN_140015190(piVar7);
          if (iVar2 == 0) {
            if (*pcVar11 == 'f') {
              uVar4 = FUN_140012070();
            }
            else {
              uVar4 = FUN_140010be0(param_1);
            }
          }
          else {
            uVar4 = FUN_14000f540();
          }
          if (((*pcVar11 == 'c') && (pcVar11[1] == 'l')) && (pcVar11[2] == '\0')) {
            uVar5 = FUN_140011150(param_1,0x45);
          }
          else if ((((*pcVar11 == 'd') && (pcVar11[1] == 't')) && (pcVar11[2] == '\0')) ||
                  (((*pcVar11 == 'p' && (pcVar11[1] == 't')) && (pcVar11[2] == '\0')))) {
            uVar5 = FUN_140014e00(param_1);
            if (**(char **)(param_1 + 0x18) == 'I') {
              uVar6 = FUN_140014c60(param_1);
              uVar5 = FUN_1400119b0(param_1,4,uVar5,uVar6);
            }
          }
          else {
            uVar5 = FUN_140010be0(param_1);
          }
          uVar4 = FUN_1400119b0(param_1,0x39,uVar4,uVar5);
          lVar3 = FUN_1400119b0(param_1,0x38,piVar7,uVar4);
          return lVar3;
        }
      }
      else {
        if (iVar8 != 3) {
          return 0;
        }
        if (pcVar11 == (char *)0x0) {
          return 0;
        }
        if (((*pcVar11 == 'q') && (pcVar11[1] == 'u')) && (pcVar11[2] == '\0')) {
          uVar4 = FUN_140010be0(param_1);
        }
        else {
          if (*pcVar11 != 'f') {
            if (*pcVar11 != 'n') {
              return 0;
            }
            if ((pcVar11[1] != 'w') && (pcVar11[1] != 'a')) {
              return 0;
            }
            uVar4 = FUN_140011150(param_1,CONCAT71(uVar9,0x5f));
            uVar5 = FUN_14000f540(param_1);
            pcVar11 = *(char **)(param_1 + 0x18);
            cVar1 = *pcVar11;
            if (cVar1 == 'E') {
              *(char **)(param_1 + 0x18) = pcVar11 + 1;
            }
            else if (cVar1 == 'p') {
              if (pcVar11[1] != 'i') {
                return 0;
              }
              *(char **)(param_1 + 0x18) = pcVar11 + 2;
              pcVar10 = (char *)FUN_140011150(param_1,0x45);
            }
            else {
              if (cVar1 != 'i') {
                return 0;
              }
              if (pcVar11[1] != 'l') {
                return 0;
              }
              pcVar10 = (char *)FUN_140010be0(param_1);
            }
            goto LAB_140010f6d;
          }
          uVar4 = FUN_140012070(param_1);
        }
        uVar5 = FUN_140010be0(param_1);
        pcVar10 = (char *)FUN_140010be0(param_1);
        if (pcVar10 != (char *)0x0) {
LAB_140010f6d:
          uVar5 = FUN_1400119b0(param_1,0x3c,uVar5,pcVar10);
          uVar4 = FUN_1400119b0(param_1,0x3b,uVar4,uVar5);
          lVar3 = FUN_1400119b0(param_1,0x3a,piVar7,uVar4);
          return lVar3;
        }
      }
      return 0;
    }
  }
  else {
    if (iVar2 == 0x33) {
      iVar8 = piVar7[2];
      goto LAB_140010e72;
    }
    if (iVar2 != 0x34) {
      return 0;
    }
  }
  if ((pcVar11 != (char *)0x0) &&
     (((cVar1 = *pcVar11, cVar1 == 'p' || (cVar1 == 'm')) && (pcVar11[1] == cVar1)))) {
    if (**(char **)(param_1 + 0x18) == '_') {
      *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
      iVar2 = *piVar7;
      pcVar10 = (char *)0x0;
    }
    else {
      pcVar10 = (char *)0x1;
    }
  }
  if ((iVar2 == 0x34) && (**(char **)(param_1 + 0x18) == '_')) {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    uVar4 = FUN_140011150(param_1,CONCAT71(uVar9,0x45));
  }
  else if (((pcVar11 == (char *)0x0) || ((*pcVar11 != 's' || (pcVar11[1] != 'P')))) ||
          (pcVar11[2] != '\0')) {
    uVar4 = FUN_140010be0(param_1);
  }
  else {
    uVar4 = FUN_140014c80(param_1);
  }
  if ((int)pcVar10 != 0) {
    uVar4 = FUN_1400119b0(param_1,0x39,uVar4,uVar4);
  }
LAB_140011121:
  lVar3 = FUN_1400119b0(param_1,0x37,piVar7,uVar4);
  return lVar3;
}


/* ================= FUN_140011150 @ 140011150  size=194  callers=1 callees=2 ================= */
/* CALLERS: FUN_140010be0 */
/* CALLEES: FUN_140010bb0, FUN_1400119b0 */

undefined8 FUN_140011150(longlong param_1,char param_2)

{
  longlong *plVar1;
  undefined8 uVar2;
  longlong lVar3;
  longlong local_res8;
  
  plVar1 = &local_res8;
  local_res8 = 0;
  if (**(char **)(param_1 + 0x18) == param_2) {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    uVar2 = FUN_1400119b0(param_1,0x2e,0);
    return uVar2;
  }
  lVar3 = FUN_140010bb0();
  while( true ) {
    if (lVar3 == 0) {
      return 0;
    }
    lVar3 = FUN_1400119b0(param_1,0x2e,lVar3);
    *plVar1 = lVar3;
    if (lVar3 == 0) break;
    plVar1 = (longlong *)(lVar3 + 0x10);
    if (**(char **)(param_1 + 0x18) == param_2) {
      *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
      return local_res8;
    }
    lVar3 = FUN_140010bb0(param_1);
  }
  return 0;
}


/* ================= FUN_140011220 @ 140011220  size=145  callers=3 callees=2 ================= */
/* CALLERS: FUN_14000fe00, FUN_140011220, FUN_1400125b0 */
/* CALLEES: FUN_140011220, FUN_140011940 */

int * FUN_140011220(undefined8 param_1,undefined4 *param_2)

{
  int *piVar1;
  
  if (param_2 != (undefined4 *)0x0) {
    do {
      switch(*param_2) {
      case 0:
      case 6:
      case 0x18:
      case 0x27:
      case 0x2c:
      case 0x32:
      case 0x41:
      case 0x42:
      case 0x46:
      case 0x47:
      case 0x48:
      case 0x4b:
      case 0x4c:
        goto LAB_1400112a1;
      default:
        piVar1 = (int *)FUN_140011220(param_1,*(undefined8 *)(param_2 + 2));
        if (piVar1 != (int *)0x0) {
          return piVar1;
        }
      case 7:
      case 8:
      case 0x33:
        param_2 = *(undefined4 **)(param_2 + 4);
        if (param_2 == (undefined4 *)0x0) {
          return (int *)0x0;
        }
        break;
      case 5:
        piVar1 = (int *)FUN_140011940(param_1,param_2);
        if (piVar1 == (int *)0x0) {
          return (int *)0x0;
        }
        if (*piVar1 != 0x2f) {
          return (int *)0x0;
        }
        return piVar1;
      }
    } while( true );
  }
LAB_1400112a1:
  return (int *)0x0;
}


/* ================= FUN_140011320 @ 140011320  size=136  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000f540 */
/* CALLEES: FUN_14000ff40, FUN_1400144d0 */

undefined8 FUN_140011320(longlong param_1)

{
  char *pcVar1;
  undefined8 uVar2;
  
  uVar2 = 0;
  if ((*(uint *)(param_1 + 0x10) & 0x40000) == 0) {
    if (0x800 < *(uint *)(param_1 + 0x54)) {
      return 0;
    }
    *(uint *)(param_1 + 0x54) = *(uint *)(param_1 + 0x54) + 1;
  }
  pcVar1 = *(char **)(param_1 + 0x18);
  if (*pcVar1 == 'F') {
    *(char **)(param_1 + 0x18) = pcVar1 + 1;
    if (pcVar1[1] == 'Y') {
      *(char **)(param_1 + 0x18) = pcVar1 + 2;
    }
    uVar2 = FUN_14000ff40(param_1,1);
    uVar2 = FUN_1400144d0(param_1,uVar2);
    if (**(char **)(param_1 + 0x18) == 'E') {
      *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    }
    else {
      uVar2 = 0;
    }
  }
  if ((*(uint *)(param_1 + 0x10) & 0x40000) == 0) {
    *(int *)(param_1 + 0x54) = *(int *)(param_1 + 0x54) + -1;
  }
  return uVar2;
}


/* ================= FUN_1400113b0 @ 1400113b0  size=66  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400125b0 */

longlong * FUN_1400113b0(longlong param_1,longlong param_2)

{
  longlong *plVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  
  uVar2 = 0;
  if (0 < *(int *)(param_1 + 0x158)) {
    plVar1 = *(longlong **)(param_1 + 0x150);
    uVar3 = uVar2;
    do {
      if (*plVar1 == param_2) {
        return *(longlong **)(param_1 + 0x150) + (longlong)(int)uVar3 * 2;
      }
      uVar3 = (ulonglong)((int)uVar3 + 1);
      uVar2 = uVar2 + 1;
      plVar1 = plVar1 + 2;
    } while ((longlong)uVar2 < (longlong)*(int *)(param_1 + 0x158));
  }
  return (longlong *)0x0;
}


/* ================= FUN_140011400 @ 140011400  size=103  callers=0 callees=2 ================= */
/* CALLEES: FUN_1400114b0, memcpy */

void FUN_140011400(longlong *param_1,void *param_2,size_t param_3)

{
  if ((ulonglong)param_1[2] < param_1[1] + 1 + param_3) {
    FUN_1400114b0();
  }
  if ((int)param_1[3] == 0) {
    memcpy((void *)(*param_1 + param_1[1]),param_2,param_3);
    *(undefined1 *)(*param_1 + param_3 + param_1[1]) = 0;
    param_1[1] = param_1[1] + param_3;
  }
  return;
}


/* ================= FUN_140011490 @ 140011490  size=26  callers=1 callees=0 ================= */
/* CALLERS: FUN_140010580 */

void FUN_140011490(undefined8 *param_1,ulonglong param_2)

{
  ulonglong _NewSize;
  void *pvVar1;
  
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  *(undefined4 *)(param_1 + 3) = 0;
  if (param_2 == 0) {
    return;
  }
  if (*(int *)(param_1 + 3) == 0) {
    _NewSize = 2;
    if (param_1[2] != 0) {
      _NewSize = param_1[2];
    }
    for (; _NewSize < param_2; _NewSize = _NewSize * 2) {
    }
    pvVar1 = realloc((void *)*param_1,_NewSize);
    if (pvVar1 == (void *)0x0) {
      free((void *)*param_1);
      param_1[1] = 0;
      param_1[2] = 0;
      *param_1 = 0;
      *(undefined4 *)(param_1 + 3) = 1;
      return;
    }
    param_1[2] = _NewSize;
    *param_1 = pvVar1;
  }
  return;
}


/* ================= FUN_1400114b0 @ 1400114b0  size=131  callers=1 callees=2 ================= */
/* CALLERS: FUN_140011400 */
/* CALLEES: free, realloc */

void FUN_1400114b0(undefined8 *param_1,ulonglong param_2)

{
  ulonglong _NewSize;
  void *pvVar1;
  
  if (*(int *)(param_1 + 3) == 0) {
    _NewSize = 2;
    if (param_1[2] != 0) {
      _NewSize = param_1[2];
    }
    for (; _NewSize < param_2; _NewSize = _NewSize * 2) {
    }
    pvVar1 = realloc((void *)*param_1,_NewSize);
    if (pvVar1 == (void *)0x0) {
      free((void *)*param_1);
      param_1[1] = 0;
      param_1[2] = 0;
      *param_1 = 0;
      *(undefined4 *)(param_1 + 3) = 1;
      return;
    }
    param_1[2] = _NewSize;
    *param_1 = pvVar1;
  }
  return;
}


/* ================= FUN_140011540 @ 140011540  size=130  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400145b0 */
/* CALLEES: FUN_140011bd0 */

undefined8 FUN_140011540(longlong param_1,int param_2)

{
  char *pcVar1;
  byte bVar2;
  undefined8 uVar3;
  char *pcVar4;
  longlong lVar5;
  
  lVar5 = (longlong)param_2;
  pcVar4 = *(char **)(param_1 + 0x18);
  if (*(longlong *)(param_1 + 8) - (longlong)pcVar4 < lVar5) {
    return 0;
  }
  pcVar1 = (char *)(lVar5 + (longlong)pcVar4);
  *(char **)(param_1 + 0x18) = pcVar1;
  if (((*(byte *)(param_1 + 0x10) & 4) != 0) && (*pcVar1 == '$')) {
    *(char **)(param_1 + 0x18) = pcVar1 + 1;
  }
  if ((((9 < param_2) && (*(longlong *)pcVar4 == 0x5f4c41424f4c475f)) &&
      (bVar2 = (char)*(longlong *)((longlong)pcVar4 + 8) - 0x24, bVar2 < 0x3c)) &&
     (((0x800000000000401U >> ((ulonglong)bVar2 & 0x3f) & 1) != 0 &&
      (*(char *)((longlong)pcVar4 + 9) == 'N')))) {
    pcVar4 = "(anonymous namespace)";
    lVar5 = 0x15;
    *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + (0x16 - param_2);
  }
  uVar3 = FUN_140011bd0(param_1,pcVar4,lVar5);
  return uVar3;
}


/* ================= FUN_1400115d0 @ 1400115d0  size=53  callers=2 callees=0 ================= */
/* CALLERS: FUN_140011940, FUN_1400125b0 */

int * FUN_1400115d0(int *param_1,int param_2)

{
  if (param_2 < 0) {
    return param_1;
  }
  for (; param_1 != (int *)0x0; param_1 = *(int **)(param_1 + 4)) {
    if (*param_1 != 0x2f) {
      return (int *)0x0;
    }
    if (param_2 < 1) break;
    param_2 = param_2 + -1;
  }
  if ((param_2 == 0) && (param_1 != (int *)0x0)) {
    return *(int **)(param_1 + 2);
  }
  return (int *)0x0;
}


/* ================= FUN_140011610 @ 140011610  size=325  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400145e0 */
/* CALLEES: FUN_140011990, FUN_1400119b0, FUN_140011bd0, FUN_140011fc0 */

undefined8 FUN_140011610(longlong param_1)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined8 uVar4;
  int iVar5;
  char *pcVar6;
  longlong lVar7;
  
  iVar2 = FUN_140011fc0();
  if (1 < iVar2) {
    cVar1 = **(char **)(param_1 + 0x18);
    if (cVar1 != '\0') {
      pcVar6 = *(char **)(param_1 + 0x18) + 1;
      *(char **)(param_1 + 0x18) = pcVar6;
      if (cVar1 == '_') {
        iVar2 = iVar2 + -1;
        lVar7 = 0;
        while (*pcVar6 != '\0') {
          if (*pcVar6 == '$') {
            cVar1 = pcVar6[1];
            if (cVar1 == '$') {
              uVar4 = 0x24;
            }
            else if (cVar1 == 'S') {
              uVar4 = 0x2f;
            }
            else {
              if (cVar1 != '_') {
                return 0;
              }
              uVar4 = 0x2e;
            }
            lVar3 = FUN_140011990(param_1,uVar4);
            *(longlong *)(param_1 + 0x18) = *(longlong *)(param_1 + 0x18) + 2;
            iVar2 = iVar2 + -2;
          }
          else {
            iVar5 = 0;
            if (0 < iVar2) {
              lVar3 = 0;
              do {
                if ((pcVar6[lVar3] == '\0') || (pcVar6[lVar3] == '$')) break;
                iVar5 = iVar5 + 1;
                lVar3 = lVar3 + 1;
              } while (lVar3 < iVar2);
            }
            lVar3 = FUN_140011bd0(param_1,pcVar6,iVar5);
            iVar2 = iVar2 - iVar5;
            *(longlong *)(param_1 + 0x18) = *(longlong *)(param_1 + 0x18) + (longlong)iVar5;
          }
          pcVar6 = *(char **)(param_1 + 0x18);
          if (lVar3 == 0) {
            return 0;
          }
          if ((lVar7 != 0) && (lVar3 = FUN_1400119b0(param_1,0x40,lVar7), lVar3 == 0)) {
            return 0;
          }
          lVar7 = lVar3;
          if (iVar2 < 1) {
            uVar4 = FUN_1400119b0(param_1,0x3f,lVar3);
            return uVar4;
          }
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_140011760 @ 140011760  size=154  callers=1 callees=4 ================= */
/* CALLERS: FUN_140014e00 */
/* CALLEES: FUN_14000fcc0, FUN_140010130, FUN_140011b50, FUN_1400121d0 */

undefined4 * FUN_140011760(longlong param_1)

{
  char *pcVar1;
  int iVar2;
  longlong lVar3;
  undefined4 *puVar4;
  
  pcVar1 = *(char **)(param_1 + 0x18);
  if ((*pcVar1 == 'U') && (*(char **)(param_1 + 0x18) = pcVar1 + 1, pcVar1[1] == 'l')) {
    *(char **)(param_1 + 0x18) = pcVar1 + 2;
    lVar3 = FUN_1400121d0();
    if ((lVar3 != 0) && (**(char **)(param_1 + 0x18) == 'E')) {
      *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
      iVar2 = FUN_140010130(param_1);
      if (-1 < iVar2) {
        puVar4 = (undefined4 *)FUN_140011b50(param_1);
        if (puVar4 != (undefined4 *)0x0) {
          *puVar4 = 0x46;
          *(longlong *)(puVar4 + 2) = lVar3;
          puVar4[4] = iVar2;
        }
        iVar2 = FUN_14000fcc0(param_1,puVar4);
        if (iVar2 != 0) {
          return puVar4;
        }
      }
    }
  }
  return (undefined4 *)0x0;
}


/* ================= FUN_140011800 @ 140011800  size=8  callers=4 callees=0 ================= */
/* CALLERS: FUN_1400125b0, FUN_140013bb0, FUN_140013d20, FUN_140013ff0 */

undefined1 FUN_140011800(longlong param_1)

{
  return *(undefined1 *)(param_1 + 0x108);
}


/* ================= FUN_140011810 @ 140011810  size=295  callers=1 callees=7 ================= */
/* CALLERS: FUN_140011dd0 */
/* CALLEES: FUN_140010130, FUN_1400108e0, FUN_140010960, FUN_1400119b0, FUN_140011ab0, FUN_140011bd0, FUN_140011dd0 */

undefined8 FUN_140011810(longlong param_1)

{
  char cVar1;
  char *pcVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  undefined8 uVar8;
  
  if (**(char **)(param_1 + 0x18) == 'Z') {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    piVar6 = (int *)FUN_140010960(param_1,0);
    if ((piVar6 != (int *)0x0) && (pcVar2 = *(char **)(param_1 + 0x18), *pcVar2 == 'E')) {
      *(char **)(param_1 + 0x18) = pcVar2 + 1;
      cVar1 = pcVar2[1];
      if (cVar1 == 's') {
        *(char **)(param_1 + 0x18) = pcVar2 + 2;
        iVar4 = FUN_1400108e0(param_1);
        if (iVar4 != 0) {
          piVar7 = (int *)FUN_140011bd0(param_1,"string literal",0xe);
LAB_1400118f4:
          if ((*piVar6 == 3) && (piVar3 = *(int **)(piVar6 + 4), *piVar3 == 0x29)) {
            piVar3[2] = 0;
            piVar3[3] = 0;
          }
          uVar8 = FUN_1400119b0(param_1,2,piVar6,piVar7);
          return uVar8;
        }
      }
      else {
        iVar4 = -1;
        if (cVar1 == 'd') {
          *(char **)(param_1 + 0x18) = pcVar2 + 2;
          iVar4 = FUN_140010130(param_1);
          if (iVar4 < 0) {
            return 0;
          }
        }
        piVar7 = (int *)FUN_140011dd0(param_1);
        if (((piVar7 == (int *)0x0) || ((*piVar7 - 0x46U & 0xfffffffd) == 0)) ||
           (iVar5 = FUN_1400108e0(param_1), iVar5 != 0)) {
          if (-1 < iVar4) {
            piVar7 = (int *)FUN_140011ab0(param_1,iVar4,piVar7);
          }
          goto LAB_1400118f4;
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_140011940 @ 140011940  size=32  callers=2 callees=1 ================= */
/* CALLERS: FUN_140011220, FUN_1400125b0 */
/* CALLEES: FUN_1400115d0 */

void FUN_140011940(longlong param_1,longlong param_2)

{
  if (*(longlong *)(param_1 + 0x120) != 0) {
    FUN_1400115d0(*(undefined8 *)(*(longlong *)(*(longlong *)(param_1 + 0x120) + 8) + 0x10),
                  *(undefined4 *)(param_2 + 8));
    return;
  }
  *(undefined4 *)(param_1 + 0x130) = 1;
  return;
}


/* ================= FUN_140011960 @ 140011960  size=44  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000f540 */
/* CALLEES: FUN_140011b50 */

undefined4 * FUN_140011960(undefined8 param_1,longlong param_2)

{
  undefined4 *puVar1;
  
  if (param_2 == 0) {
    return (undefined4 *)0x0;
  }
  puVar1 = (undefined4 *)FUN_140011b50();
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = 0x27;
    *(longlong *)(puVar1 + 2) = param_2;
  }
  return puVar1;
}


/* ================= FUN_140011990 @ 140011990  size=32  callers=1 callees=1 ================= */
/* CALLERS: FUN_140011610 */
/* CALLEES: FUN_140011b50 */

void FUN_140011990(undefined8 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)FUN_140011b50();
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = 0x41;
    puVar1[2] = param_2;
  }
  return;
}


/* ================= FUN_1400119b0 @ 1400119b0  size=99  callers=23 callees=1 ================= */
/* CALLERS: FUN_14000f540, FUN_14000fc40, FUN_14000fe60, FUN_14000ff40, FUN_140010060, FUN_1400103a0, FUN_140010610, FUN_140010960, FUN_140010aa0, FUN_140010be0, FUN_140011150, FUN_140011610, FUN_140011810, FUN_140011dd0, FUN_140012070, FUN_1400121d0, FUN_1400122a0, FUN_140012310, FUN_1400144d0, FUN_1400145e0, FUN_140014c80, FUN_140014e00, FUN_140014f40 */
/* CALLEES: FUN_140011b50 */

undefined4 * FUN_1400119b0(undefined8 param_1,undefined4 param_2,longlong param_3,longlong param_4)

{
  longlong lVar1;
  undefined4 *puVar2;
  
  lVar1 = param_4;
  switch(param_2) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 0xb:
  case 0x21:
  case 0x2b:
  case 0x2d:
  case 0x37:
  case 0x38:
  case 0x39:
  case 0x3a:
  case 0x3b:
  case 0x3d:
  case 0x3e:
  case 0x40:
  case 0x4c:
  case 0x4e:
    if (param_3 == 0) {
      return (undefined4 *)0x0;
    }
    break;
  default:
    goto LAB_1400119ea;
  case 9:
  case 10:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x22:
  case 0x23:
  case 0x24:
  case 0x25:
  case 0x26:
  case 0x28:
  case 0x30:
  case 0x34:
  case 0x35:
  case 0x36:
  case 0x3c:
  case 0x3f:
  case 0x43:
  case 0x44:
  case 0x45:
  case 0x49:
  case 0x4a:
  case 0x4b:
    lVar1 = param_3;
    break;
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
  case 0x20:
  case 0x29:
  case 0x2e:
  case 0x2f:
  case 0x4d:
  case 0x4f:
  case 0x50:
    goto switchD_1400119db_caseD_19;
  case 0x2a:
  case 0x31:
  }
  if (lVar1 != 0) {
switchD_1400119db_caseD_19:
    puVar2 = (undefined4 *)FUN_140011b50();
    if (puVar2 != (undefined4 *)0x0) {
      *puVar2 = param_2;
      *(longlong *)(puVar2 + 2) = param_3;
      *(longlong *)(puVar2 + 4) = param_4;
      return puVar2;
    }
    return (undefined4 *)0x0;
  }
LAB_1400119ea:
  return (undefined4 *)0x0;
}


/* ================= FUN_140011a80 @ 140011a80  size=45  callers=1 callees=2 ================= */
/* CALLERS: FUN_140010280 */
/* CALLEES: FUN_14000f1e0, FUN_140011b50 */

undefined8 FUN_140011a80(undefined8 param_1,ulonglong param_2)

{
  int iVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  uVar2 = FUN_140011b50();
  iVar1 = FUN_14000f1e0(uVar2,param_2 & 0xffffffff);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  return uVar2;
}


/* ================= FUN_140011ab0 @ 140011ab0  size=36  callers=1 callees=1 ================= */
/* CALLERS: FUN_140011810 */
/* CALLEES: FUN_140011b50 */

void FUN_140011ab0(undefined8 param_1,undefined4 param_2,undefined8 param_3)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)FUN_140011b50();
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = 0x47;
    puVar1[4] = param_2;
    *(undefined8 *)(puVar1 + 2) = param_3;
  }
  return;
}


/* ================= FUN_140011ae0 @ 140011ae0  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_140010610 */
/* CALLEES: FUN_140010960, FUN_140011bd0 */

void FUN_140011ae0(longlong param_1,longlong param_2)

{
  char *pcVar1;
  longlong lVar2;
  
  pcVar1 = *(char **)(param_1 + 0x18);
  if ((*pcVar1 == '_') && (pcVar1[1] == 'Z')) {
    *(char **)(param_1 + 0x18) = pcVar1 + 2;
    FUN_140010960(param_1,0);
    return;
  }
  lVar2 = -1;
  do {
    lVar2 = lVar2 + 1;
  } while (*(char *)(param_2 + lVar2) != '\0');
  FUN_140011bd0();
  return;
}


/* ================= FUN_140011b20 @ 140011b20  size=45  callers=1 callees=2 ================= */
/* CALLERS: FUN_140010280 */
/* CALLEES: FUN_14000f210, FUN_140011b50 */

undefined8 FUN_140011b20(undefined8 param_1,ulonglong param_2)

{
  int iVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  uVar2 = FUN_140011b50();
  iVar1 = FUN_14000f210(uVar2,param_2 & 0xffffffff);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  return uVar2;
}


/* ================= FUN_140011b50 @ 140011b50  size=39  callers=16 callees=0 ================= */
/* CALLERS: FUN_14000f540, FUN_140011760, FUN_140011960, FUN_140011990, FUN_1400119b0, FUN_140011a80, FUN_140011ab0, FUN_140011b20, FUN_140011b80, FUN_140011bb0, FUN_140011bd0, FUN_140011c00, FUN_140011c20, FUN_140011c50, FUN_140012040, FUN_140014d90 */

longlong FUN_140011b50(longlong param_1)

{
  int iVar1;
  longlong lVar2;
  
  iVar1 = *(int *)(param_1 + 0x28);
  if (*(int *)(param_1 + 0x2c) <= iVar1) {
    return 0;
  }
  lVar2 = *(longlong *)(param_1 + 0x20);
  *(undefined4 *)(lVar2 + 4 + (longlong)iVar1 * 0x18) = 0;
  *(int *)(param_1 + 0x28) = *(int *)(param_1 + 0x28) + 1;
  return lVar2 + (longlong)iVar1 * 0x18;
}


/* ================= FUN_140011b80 @ 140011b80  size=45  callers=1 callees=2 ================= */
/* CALLERS: FUN_140012070 */
/* CALLEES: FUN_14000f240, FUN_140011b50 */

undefined8 FUN_140011b80(undefined8 param_1,ulonglong param_2)

{
  int iVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  uVar2 = FUN_140011b50();
  iVar1 = FUN_14000f240(uVar2,param_2 & 0xffffffff);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  return uVar2;
}


/* ================= FUN_140011bb0 @ 140011bb0  size=32  callers=1 callees=1 ================= */
/* CALLERS: FUN_140010be0 */
/* CALLEES: FUN_140011b50 */

void FUN_140011bb0(undefined8 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)FUN_140011b50();
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = 6;
    puVar1[2] = param_2;
  }
  return;
}


/* ================= FUN_140011bd0 @ 140011bd0  size=45  callers=9 callees=2 ================= */
/* CALLERS: FUN_14000f540, FUN_14000fe60, FUN_140010060, FUN_140010aa0, FUN_140011540, FUN_140011610, FUN_140011810, FUN_140011ae0, FUN_140011dd0 */
/* CALLEES: FUN_14000f270, FUN_140011b50 */

undefined8 FUN_140011bd0(undefined8 param_1,undefined8 param_2)

{
  int iVar1;
  undefined8 uVar2;
  
  uVar2 = FUN_140011b50();
  iVar1 = FUN_14000f270(uVar2,param_2);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  return uVar2;
}


/* ================= FUN_140011c00 @ 140011c00  size=32  callers=1 callees=1 ================= */
/* CALLERS: FUN_140012070 */
/* CALLEES: FUN_140011b50 */

void FUN_140011c00(undefined8 param_1,undefined8 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)FUN_140011b50();
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = 0x32;
    *(undefined8 *)(puVar1 + 2) = param_2;
  }
  return;
}


/* ================= FUN_140011c20 @ 140011c20  size=36  callers=1 callees=1 ================= */
/* CALLERS: FUN_140014a60 */
/* CALLEES: FUN_140011b50 */

void FUN_140011c20(undefined8 param_1,undefined8 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)FUN_140011b50();
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = 0x18;
    *(undefined8 *)(puVar1 + 2) = param_2;
    puVar1[4] = param_3;
  }
  return;
}


/* ================= FUN_140011c50 @ 140011c50  size=32  callers=1 callees=1 ================= */
/* CALLERS: FUN_140014d50 */
/* CALLEES: FUN_140011b50 */

void FUN_140011c50(undefined8 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)FUN_140011b50();
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = 5;
    puVar1[2] = param_2;
  }
  return;
}


/* ================= FUN_140011c70 @ 140011c70  size=338  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400125b0 */
/* CALLEES: FUN_14000fd40, FUN_14000fde0, FUN_140013cc0, FUN_140014450 */

undefined8 FUN_140011c70(longlong param_1,undefined4 param_2,longlong param_3)

{
  char cVar1;
  undefined4 uVar2;
  char *pcVar3;
  undefined8 uVar4;
  int *piVar5;
  int *piVar6;
  
  pcVar3 = (char *)**(undefined8 **)(*(longlong *)(param_3 + 8) + 8);
  if (*pcVar3 != 'f') {
    return 0;
  }
  piVar6 = (int *)0x0;
  piVar5 = *(int **)(*(longlong *)(param_3 + 0x10) + 0x10);
  uVar4 = *(undefined8 *)(*(longlong *)(param_3 + 0x10) + 8);
  if (*piVar5 == 0x3c) {
    piVar6 = *(int **)(piVar5 + 4);
    piVar5 = *(int **)(piVar5 + 2);
  }
  uVar2 = *(undefined4 *)(param_1 + 0x13c);
  *(undefined4 *)(param_1 + 0x13c) = 0xffffffff;
  cVar1 = pcVar3[1];
  if ((cVar1 == 'L') || (cVar1 == 'R')) {
    FUN_14000fd40(param_1,0x28);
    FUN_140014450(param_1,param_2,piVar5);
    FUN_140013cc0(param_1,param_2,uVar4);
    FUN_14000fde0(param_1,&DAT_140060544);
    FUN_140013cc0(param_1,param_2,uVar4);
  }
  else {
    if (cVar1 != 'l') {
      if (cVar1 == 'r') {
        FUN_14000fd40(param_1,0x28);
        FUN_140014450(param_1,param_2,piVar5);
        FUN_140013cc0(param_1,param_2,uVar4);
        FUN_14000fde0(param_1,&DAT_140060a5c);
      }
      goto LAB_140011d9b;
    }
    FUN_14000fde0(param_1,&DAT_140060a54);
    FUN_140013cc0(param_1,param_2,uVar4);
    piVar6 = piVar5;
  }
  FUN_140014450(param_1,param_2,piVar6);
  FUN_14000fd40(param_1,0x29);
LAB_140011d9b:
  *(undefined4 *)(param_1 + 0x13c) = uVar2;
  return 1;
}


/* ================= FUN_140011dd0 @ 140011dd0  size=317  callers=3 callees=8 ================= */
/* CALLERS: FUN_140010960, FUN_140011810, FUN_1400145e0 */
/* CALLEES: FUN_14000fcc0, FUN_140011810, FUN_1400119b0, FUN_140011bd0, FUN_140011f10, FUN_140014a60, FUN_140014c60, FUN_140014e00 */

undefined8 FUN_140011dd0(longlong param_1)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  
  pcVar2 = *(char **)(param_1 + 0x18);
  cVar1 = *pcVar2;
  if (cVar1 == 'L') {
LAB_140011edd:
    uVar4 = FUN_140014e00(param_1);
    if (**(char **)(param_1 + 0x18) != 'I') {
      return uVar4;
    }
    iVar3 = FUN_14000fcc0(param_1,uVar4);
  }
  else {
    if (cVar1 == 'N') {
      uVar4 = FUN_140011f10(param_1);
      return uVar4;
    }
    if (cVar1 != 'S') {
      if (cVar1 == 'U') {
        uVar4 = FUN_140014e00(param_1);
        return uVar4;
      }
      if (cVar1 == 'Z') {
        uVar4 = FUN_140011810(param_1);
        return uVar4;
      }
      goto LAB_140011edd;
    }
    cVar1 = pcVar2[1];
    if (cVar1 == 't') {
      *(char **)(param_1 + 0x18) = pcVar2 + 2;
      uVar4 = FUN_140014e00(param_1);
      uVar5 = FUN_140011bd0(param_1,&DAT_140060468,3);
      uVar4 = FUN_1400119b0(param_1,1,uVar5,uVar4);
      *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + 3;
    }
    else {
      uVar4 = FUN_140014a60(param_1,0);
    }
    if (**(char **)(param_1 + 0x18) != 'I') {
      return uVar4;
    }
    if (cVar1 != 't') goto LAB_140011ea2;
    iVar3 = FUN_14000fcc0(param_1,uVar4);
  }
  if (iVar3 == 0) {
    return 0;
  }
LAB_140011ea2:
  uVar5 = FUN_140014c60();
  uVar4 = FUN_1400119b0(param_1,4,uVar4,uVar5);
  return uVar4;
}


/* ================= FUN_140011f10 @ 140011f10  size=166  callers=1 callees=3 ================= */
/* CALLERS: FUN_140011dd0 */
/* CALLEES: FUN_1400103a0, FUN_140012310, FUN_1400144d0 */

longlong FUN_140011f10(longlong param_1)

{
  longlong *plVar1;
  longlong lVar2;
  longlong lVar3;
  longlong local_res8;
  
  if (**(char **)(param_1 + 0x18) == 'N') {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    plVar1 = (longlong *)FUN_1400103a0(param_1,&local_res8,1);
    if (plVar1 != (longlong *)0x0) {
      lVar2 = FUN_1400144d0(param_1,0);
      lVar3 = FUN_140012310(param_1);
      *plVar1 = lVar3;
      if (lVar3 != 0) {
        if (lVar2 != 0) {
          *(longlong *)(lVar2 + 8) = local_res8;
          local_res8 = lVar2;
        }
        if (**(char **)(param_1 + 0x18) == 'E') {
          *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
          return local_res8;
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_140011fc0 @ 140011fc0  size=123  callers=8 callees=0 ================= */
/* CALLERS: FUN_14000f540, FUN_14000ffc0, FUN_140010130, FUN_1400108e0, FUN_140011610, FUN_140012040, FUN_1400145b0, FUN_1400145e0 */

int FUN_140011fc0(longlong param_1)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  
  iVar4 = 0;
  bVar2 = **(byte **)(param_1 + 0x18);
  bVar5 = bVar2 == 0x6e;
  if (bVar5) {
    pbVar1 = *(byte **)(param_1 + 0x18) + 1;
    *(byte **)(param_1 + 0x18) = pbVar1;
    bVar2 = *pbVar1;
  }
  while (('/' < (char)bVar2 && (bVar2 < 0x3a))) {
    if ((int)(-0x7fffffd1 - (uint)bVar2) / 10 < iVar4) {
      return -1;
    }
    *(longlong *)(param_1 + 0x18) = *(longlong *)(param_1 + 0x18) + 1;
    iVar4 = (uint)bVar2 + iVar4 * 10 + -0x30;
    bVar2 = **(byte **)(param_1 + 0x18);
  }
  iVar3 = -iVar4;
  if (!bVar5) {
    iVar3 = iVar4;
  }
  return iVar3;
}


/* ================= FUN_140012040 @ 140012040  size=48  callers=2 callees=2 ================= */
/* CALLERS: FUN_1400145e0, FUN_140014f40 */
/* CALLEES: FUN_140011b50, FUN_140011fc0 */

undefined4 * FUN_140012040(undefined8 param_1,undefined8 param_2)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)FUN_140011b50(param_1,param_2,param_1);
  if (puVar2 != (undefined4 *)0x0) {
    *puVar2 = 0x42;
    uVar1 = FUN_140011fc0(param_1);
    puVar2[2] = uVar1;
  }
  return puVar2;
}


/* ================= FUN_140012070 @ 140012070  size=311  callers=2 callees=5 ================= */
/* CALLERS: FUN_140010be0, FUN_140014e00 */
/* CALLEES: FUN_14000f540, FUN_1400119b0, FUN_140011b80, FUN_140011c00, FUN_1400145b0 */

undefined8 FUN_140012070(longlong param_1)

{
  char cVar1;
  undefined4 uVar2;
  char *pcVar3;
  int iVar4;
  undefined8 uVar5;
  char cVar6;
  int iVar7;
  int iVar8;
  
  pcVar3 = *(char **)(param_1 + 0x18);
  cVar1 = *pcVar3;
  if (cVar1 == '\0') {
    cVar6 = '\0';
  }
  else {
    *(char **)(param_1 + 0x18) = pcVar3 + 1;
    cVar6 = pcVar3[1];
    if (cVar6 != '\0') {
      *(char **)(param_1 + 0x18) = pcVar3 + 2;
    }
  }
  if (cVar1 == 'v') {
    if ((byte)(cVar6 - 0x30U) < 10) {
      uVar5 = FUN_1400145b0(param_1);
      uVar5 = FUN_140011b80(param_1,cVar6 + -0x30,uVar5);
      return uVar5;
    }
  }
  else if ((cVar1 == 'c') && (cVar6 == 'v')) {
    uVar2 = *(undefined4 *)(param_1 + 0x50);
    *(uint *)(param_1 + 0x50) = (uint)(*(int *)(param_1 + 0x4c) == 0);
    uVar5 = FUN_14000f540(param_1);
    uVar5 = FUN_1400119b0(param_1,(*(int *)(param_1 + 0x50) != 0) + '4',uVar5,0);
    *(undefined4 *)(param_1 + 0x50) = uVar2;
    return uVar5;
  }
  iVar7 = 0;
  iVar8 = 0x43;
  do {
    iVar4 = (iVar8 - iVar7) / 2 + iVar7;
    pcVar3 = (&PTR_DAT_14005f860)[(longlong)iVar4 * 3];
    if (cVar1 == *pcVar3) {
      if (cVar6 == pcVar3[1]) {
        uVar5 = FUN_140011c00(param_1,&PTR_DAT_14005f860 + (longlong)iVar4 * 3);
        return uVar5;
      }
      if (pcVar3[1] <= cVar6) {
LAB_140012188:
        iVar7 = iVar4 + 1;
        iVar4 = iVar8;
      }
    }
    else if (*pcVar3 <= cVar1) goto LAB_140012188;
    iVar8 = iVar4;
    if (iVar7 == iVar8) {
      return 0;
    }
  } while( true );
}


/* ================= FUN_1400121b0 @ 1400121b0  size=31  callers=2 callees=0 ================= */
/* CALLERS: FUN_14000fe00, FUN_1400125b0 */

int FUN_1400121b0(int *param_1)

{
  int iVar1;
  
  iVar1 = 0;
  for (; ((param_1 != (int *)0x0 && (*param_1 == 0x2f)) && (*(longlong *)(param_1 + 2) != 0));
      param_1 = *(int **)(param_1 + 4)) {
    iVar1 = iVar1 + 1;
  }
  return iVar1;
}


/* ================= FUN_1400121d0 @ 1400121d0  size=198  callers=3 callees=2 ================= */
/* CALLERS: FUN_14000ff40, FUN_1400103a0, FUN_140011760 */
/* CALLEES: FUN_14000f540, FUN_1400119b0 */

longlong FUN_1400121d0(longlong param_1)

{
  char cVar1;
  longlong lVar2;
  char *pcVar3;
  longlong *plVar4;
  longlong local_res8;
  
  pcVar3 = *(char **)(param_1 + 0x18);
  plVar4 = &local_res8;
  local_res8 = 0;
  cVar1 = *pcVar3;
  if (cVar1 != '\0') {
    do {
      if (((cVar1 == 'E') || (cVar1 == '.')) ||
         (((cVar1 == 'R' || (cVar1 == 'O')) && (pcVar3[1] == 'E')))) break;
      lVar2 = FUN_14000f540(param_1);
      if (lVar2 == 0) {
        return 0;
      }
      lVar2 = FUN_1400119b0(param_1,0x2e,lVar2);
      *plVar4 = lVar2;
      if (lVar2 == 0) {
        return 0;
      }
      pcVar3 = *(char **)(param_1 + 0x18);
      plVar4 = (longlong *)(lVar2 + 0x10);
      cVar1 = *pcVar3;
    } while (cVar1 != '\0');
    if (local_res8 != 0) {
      if (*(longlong *)(local_res8 + 0x10) != 0) {
        return local_res8;
      }
      if (**(int **)(local_res8 + 8) == 0x27) {
        lVar2 = *(longlong *)(*(int **)(local_res8 + 8) + 2);
        if (*(int *)(lVar2 + 0x1c) == 9) {
          *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) - *(int *)(lVar2 + 8);
          *(undefined8 *)(local_res8 + 8) = 0;
          return local_res8;
        }
        return local_res8;
      }
      return local_res8;
    }
  }
  return 0;
}


/* ================= FUN_1400122a0 @ 1400122a0  size=97  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000f540 */
/* CALLEES: FUN_14000f540, FUN_1400119b0 */

undefined8 FUN_1400122a0(longlong param_1)

{
  longlong lVar1;
  longlong lVar2;
  undefined8 uVar3;
  
  if (**(char **)(param_1 + 0x18) == 'M') {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    lVar1 = FUN_14000f540();
    if (lVar1 != 0) {
      lVar2 = FUN_14000f540(param_1);
      if (lVar2 != 0) {
        uVar3 = FUN_1400119b0(param_1,0x2b,lVar1,lVar2);
        return uVar3;
      }
    }
  }
  return 0;
}


/* ================= FUN_140012310 @ 140012310  size=301  callers=1 callees=7 ================= */
/* CALLERS: FUN_140011f10 */
/* CALLEES: FUN_14000f540, FUN_14000fcc0, FUN_1400119b0, FUN_140014a60, FUN_140014c60, FUN_140014d50, FUN_140014e00 */

longlong FUN_140012310(longlong param_1)

{
  byte bVar1;
  byte *pbVar2;
  int iVar3;
  longlong lVar4;
  undefined8 uVar5;
  longlong lVar6;
  
  pbVar2 = *(byte **)(param_1 + 0x18);
  lVar6 = 0;
  bVar1 = *pbVar2;
  do {
    if (bVar1 == 0) {
      return 0;
    }
    if (bVar1 == 0x44) {
      if ((pbVar2[1] + 0xac & 0xdf) == 0) {
        lVar4 = FUN_14000f540();
        goto LAB_1400123d2;
      }
LAB_1400123cd:
      lVar4 = FUN_140014e00(param_1);
LAB_1400123d2:
      uVar5 = 1;
      if (lVar6 != 0) {
LAB_1400123dc:
        lVar4 = FUN_1400119b0(param_1,uVar5,lVar6,lVar4);
      }
      lVar6 = lVar4;
      if (((bVar1 != 0x53) && (**(char **)(param_1 + 0x18) != 'E')) &&
         (iVar3 = FUN_14000fcc0(param_1,lVar4), iVar3 == 0)) {
        return 0;
      }
    }
    else {
      if ((((byte)(bVar1 - 0x30) < 0x26) &&
          ((0x20100803ffU >> ((ulonglong)(bVar1 - 0x30) & 0x3f) & 1) != 0)) ||
         ((byte)(bVar1 + 0x9f) < 0x1a)) goto LAB_1400123cd;
      if (bVar1 == 0x53) {
        lVar4 = FUN_140014a60(param_1,1);
        goto LAB_1400123d2;
      }
      if (bVar1 == 0x49) {
        if (lVar6 == 0) {
          return 0;
        }
        uVar5 = 4;
        lVar4 = FUN_140014c60(param_1);
        goto LAB_1400123dc;
      }
      if (bVar1 == 0x54) {
        lVar4 = FUN_140014d50(param_1);
        goto LAB_1400123d2;
      }
      if (bVar1 == 0x45) {
        return lVar6;
      }
      if (bVar1 != 0x4d) {
        return 0;
      }
      if (lVar6 == 0) {
        return 0;
      }
      *(longlong *)(param_1 + 0x18) = *(longlong *)(param_1 + 0x18) + 1;
    }
    pbVar2 = *(byte **)(param_1 + 0x18);
    bVar1 = *pbVar2;
  } while( true );
}


/* ================= FUN_140012440 @ 140012440  size=198  callers=2 callees=4 ================= */
/* CALLERS: FUN_1400125b0, FUN_1400142a0 */
/* CALLEES: FUN_14000fd40, FUN_14000fde0, FUN_140012520, FUN_1400142a0 */

void FUN_140012440(undefined8 param_1,ulonglong param_2,longlong param_3,longlong *param_4)

{
  longlong *plVar1;
  
  plVar1 = param_4;
  if (param_4 != (longlong *)0x0) {
    do {
      if ((int)plVar1[2] == 0) {
        if (*(int *)plVar1[1] != 0x2a) {
          FUN_14000fde0(param_1,&DAT_140060de4);
          FUN_1400142a0(param_1,param_2 & 0xffffffff,param_4,0);
          FUN_14000fd40(param_1,0x29);
          goto LAB_1400124c1;
        }
        FUN_1400142a0(param_1,param_2,param_4,0);
        goto LAB_1400124cb;
      }
      plVar1 = (longlong *)*plVar1;
    } while (plVar1 != (longlong *)0x0);
    FUN_1400142a0(param_1,param_2,param_4,0);
  }
LAB_1400124c1:
  FUN_14000fd40(param_1,0x20);
LAB_1400124cb:
  FUN_14000fd40(param_1,0x5b);
  if (*(longlong *)(param_3 + 8) != 0) {
    FUN_140012520(param_1,param_2 & 0xffffffff);
  }
  FUN_14000fd40(param_1,0x5d);
  return;
}


/* ================= FUN_140012510 @ 140012510  size=9  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400125b0 */
/* CALLEES: FUN_140012520 */

void FUN_140012510(undefined8 param_1,undefined8 param_2,longlong param_3)

{
  FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 8));
  return;
}


/* ================= FUN_140012520 @ 140012520  size=135  callers=10 callees=2 ================= */
/* CALLERS: FUN_14000f390, FUN_140012440, FUN_140012510, FUN_1400125b0, FUN_140013bb0, FUN_140013cc0, FUN_140013d20, FUN_140013ff0, FUN_1400142a0, FUN_140014450 */
/* CALLEES: FUN_1400125b0, FUN_140013cb0 */

void FUN_140012520(longlong param_1,undefined8 param_2,longlong param_3)

{
  longlong local_18;
  undefined8 local_10;
  
  if (((param_3 != 0) && (*(int *)(param_3 + 4) < 2)) && (*(int *)(param_1 + 0x134) < 0x401)) {
    *(int *)(param_3 + 4) = *(int *)(param_3 + 4) + 1;
    local_10 = *(undefined8 *)(param_1 + 0x148);
    *(int *)(param_1 + 0x134) = *(int *)(param_1 + 0x134) + 1;
    *(longlong **)(param_1 + 0x148) = &local_18;
    local_18 = param_3;
    FUN_1400125b0();
    *(undefined8 *)(param_1 + 0x148) = local_10;
    *(int *)(param_3 + 4) = *(int *)(param_3 + 4) + -1;
    *(int *)(param_1 + 0x134) = *(int *)(param_1 + 0x134) + -1;
    return;
  }
  FUN_140013cb0();
  return;
}


/* ================= FUN_1400125b0 @ 1400125b0  size=5266  callers=1 callees=29 ================= */
/* CALLERS: FUN_140012520 */
/* CALLEES: FUN_14000fcf0, FUN_14000fd40, FUN_14000fd90, FUN_14000fde0, FUN_14000fe00, FUN_140011220, FUN_1400113b0, FUN_1400115d0, FUN_140011800, FUN_140011940, FUN_140011c70, FUN_1400121b0, FUN_140012440, FUN_140012510, FUN_140012520, FUN_140013bb0, FUN_140013cb0, FUN_140013cc0, FUN_140013ce0, FUN_140013d20, FUN_140013f30, FUN_140013ff0, FUN_140014440, FUN_140014450, FUN_140014520, FUN_1400150d0, FUN_140015190, __security_check_cookie, strncmp */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_1400125b0(longlong param_1,uint param_2,int *param_3)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  longlong lVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  char cVar8;
  int iVar9;
  int iVar10;
  int *piVar11;
  int *piVar12;
  undefined8 uVar13;
  longlong lVar14;
  undefined8 *puVar15;
  uint uVar16;
  char *pcVar17;
  undefined8 *puVar18;
  ulonglong uVar19;
  longlong lVar20;
  undefined8 uVar21;
  ulonglong uVar22;
  ulonglong uVar23;
  ulonglong uVar24;
  ulonglong uVar25;
  ulonglong uVar26;
  undefined4 extraout_XMM0_Da;
  undefined4 extraout_XMM0_Da_00;
  undefined4 extraout_XMM0_Da_01;
  undefined4 uVar27;
  undefined1 auStack_108 [32];
  undefined8 local_e8;
  int *piStack_e0;
  int local_d8;
  undefined8 local_d0;
  undefined8 local_c8;
  undefined8 local_c0;
  int local_b8 [2];
  undefined8 local_b0;
  undefined8 local_a8;
  int aiStack_a0 [2];
  undefined8 local_98;
  undefined8 uStack_90;
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_108;
  uVar19 = 0;
  pcVar17 = (char *)0x0;
  iVar10 = 0;
  uVar26 = 0;
  uVar24 = 0;
  uVar22 = 0;
  if (param_3 == (int *)0x0) goto switchD_140012623_caseD_34;
  lVar14 = param_1;
  piVar11 = param_3;
  iVar9 = FUN_140014440();
  if (iVar9 != 0) {
    return;
  }
  iVar9 = *piVar11;
  piVar11 = param_3;
  uVar23 = uVar19;
  uVar25 = uVar19;
  uVar27 = extraout_XMM0_Da;
  switch(iVar9) {
  case 0:
    iVar10 = param_3[4];
    uVar13 = *(undefined8 *)(param_3 + 2);
    if ((param_2 & 4) != 0) {
      FUN_140013f30();
      return;
    }
    goto LAB_140012cd4;
  case 1:
  case 2:
    FUN_140012520(extraout_XMM0_Da,param_2,*(undefined8 *)(param_3 + 2));
    if ((param_2 & 4) == 0) {
      FUN_14000fde0(param_1,&DAT_140060560);
    }
    else {
      FUN_14000fd40(param_1,0x2e);
    }
    piVar11 = *(int **)(param_3 + 4);
    if (*piVar11 == 0x47) {
      FUN_14000fde0(param_1,"{default arg#");
      FUN_14000fd90(param_1,piVar11[4] + 1);
      FUN_14000fde0(param_1,&DAT_140060a80);
      piVar11 = *(int **)(piVar11 + 2);
    }
    FUN_140012520(param_1,param_2,piVar11);
    break;
  case 3:
    uVar13 = *(undefined8 *)(lVar14 + 0x128);
    *(undefined8 *)(lVar14 + 0x128) = 0;
    piVar11 = *(int **)(param_3 + 2);
    local_e8 = 0;
    piStack_e0 = (int *)0x0;
    while( true ) {
      if ((piVar11 == (int *)0x0) || (3 < (uint)uVar19)) goto switchD_140012623_caseD_34;
      uVar1 = *(undefined8 *)(param_1 + 0x128);
      uVar21 = *(undefined8 *)(param_1 + 0x120);
      uVar22 = (ulonglong)((uint)uVar19 + 1);
      *(undefined8 **)(param_1 + 0x128) = &local_c8 + uVar19 * 4;
      (&local_c0)[uVar19 * 4] = piVar11;
      (&local_c8)[uVar19 * 4] = uVar1;
      local_b8[uVar19 * 8] = 0;
      (&local_b0)[uVar19 * 4] = uVar21;
      iVar10 = *piVar11;
      iVar9 = FUN_1400150d0(iVar10);
      if (iVar9 == 0) break;
      piVar11 = *(int **)(piVar11 + 2);
      uVar19 = uVar22;
    }
    if (iVar10 != 2) goto LAB_140012828;
    piVar11 = *(int **)(piVar11 + 4);
    if (*piVar11 == 0x47) {
      piVar11 = *(int **)(piVar11 + 2);
    }
    if (piVar11 != (int *)0x0) {
      iVar10 = *piVar11;
      iVar9 = FUN_1400150d0(iVar10);
      goto joined_r0x0001400127c4;
    }
    goto switchD_140012623_caseD_34;
  case 4:
    uVar13 = *(undefined8 *)(lVar14 + 0x170);
    uVar1 = *(undefined8 *)(lVar14 + 0x128);
    *(int **)(lVar14 + 0x170) = param_3;
    *(undefined8 *)(lVar14 + 0x128) = 0;
    piVar11 = *(int **)(param_3 + 2);
    if ((((param_2 & 4) == 0) || (*piVar11 != 0)) ||
       ((piVar11[4] != 6 || (iVar10 = strncmp(*(char **)(piVar11 + 2),"JArray",6), iVar10 != 0)))) {
      FUN_140012520(param_1,param_2,piVar11);
      cVar8 = FUN_140011800(param_1);
      if (cVar8 == '<') {
        FUN_14000fd40(extraout_XMM0_Da_00,0x20);
      }
      FUN_14000fd40(param_1,0x3c);
      FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
      cVar8 = FUN_140011800(param_1);
      if (cVar8 == '>') {
        FUN_14000fd40(extraout_XMM0_Da_01,0x20);
      }
      FUN_14000fd40(param_1,0x3e);
      *(undefined8 *)(param_1 + 0x128) = uVar1;
      *(undefined8 *)(param_1 + 0x170) = uVar13;
    }
    else {
      FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
      FUN_14000fde0(param_1,&DAT_140060570);
      *(undefined8 *)(param_1 + 0x128) = uVar1;
      *(undefined8 *)(param_1 + 0x170) = uVar13;
    }
    break;
  case 5:
    if (*(int *)(lVar14 + 0x138) != 0) {
      FUN_14000fcf0(extraout_XMM0_Da,"auto:",5);
      FUN_14000fd90(param_1,param_3[2] + 1);
      return;
    }
    piVar11 = (int *)FUN_140011940(extraout_XMM0_Da,param_3);
    if ((piVar11 != (int *)0x0) &&
       ((*piVar11 != 0x2f ||
        (piVar11 = (int *)FUN_1400115d0(piVar11,*(undefined4 *)(param_1 + 0x13c),piVar11),
        piVar11 != (int *)0x0)))) {
      puVar18 = *(undefined8 **)(param_1 + 0x120);
      *(undefined8 *)(param_1 + 0x120) = *puVar18;
      FUN_140012520(param_1,param_2,piVar11);
      *(undefined8 **)(param_1 + 0x120) = puVar18;
      return;
    }
    goto switchD_140012623_caseD_34;
  case 6:
    iVar10 = param_3[2];
    if (iVar10 == 0) {
      FUN_14000fde0(extraout_XMM0_Da,&DAT_140060cdc);
    }
    else {
      FUN_14000fde0(extraout_XMM0_Da,"{parm#");
      FUN_14000fd90(param_1,iVar10);
      FUN_14000fd40(param_1,0x7d);
    }
    break;
  case 7:
    goto switchD_140012623_caseD_7;
  case 8:
    FUN_14000fd40(extraout_XMM0_Da,0x14000007e);
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
    break;
  case 9:
    FUN_14000fde0(extraout_XMM0_Da,"vtable for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 10:
    FUN_14000fde0(extraout_XMM0_Da,"VTT for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0xb:
    FUN_14000fde0(extraout_XMM0_Da,"construction vtable for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    FUN_14000fde0(param_1,&DAT_140060af4);
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
    break;
  case 0xc:
    FUN_14000fde0(extraout_XMM0_Da,"typeinfo for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0xd:
    FUN_14000fde0(extraout_XMM0_Da,"typeinfo name for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0xe:
    FUN_14000fde0(extraout_XMM0_Da,"typeinfo fn for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0xf:
    FUN_14000fde0(extraout_XMM0_Da,"non-virtual thunk to ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x10:
    FUN_14000fde0(extraout_XMM0_Da,"virtual thunk to ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x11:
    FUN_14000fde0(extraout_XMM0_Da,"covariant return thunk to ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x12:
    FUN_14000fde0(extraout_XMM0_Da,"java Class for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x13:
    FUN_14000fde0(extraout_XMM0_Da,"guard variable for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x14:
    FUN_14000fde0(extraout_XMM0_Da,"TLS init function for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x15:
    FUN_14000fde0(extraout_XMM0_Da,"TLS wrapper function for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x16:
    FUN_14000fde0(extraout_XMM0_Da,"reference temporary #");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
    FUN_14000fde0(param_1," for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x17:
    FUN_14000fde0(extraout_XMM0_Da,"hidden alias for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x18:
    iVar10 = param_3[4];
    uVar13 = *(undefined8 *)(param_3 + 2);
LAB_140012cd4:
    FUN_14000fcf0(extraout_XMM0_Da,uVar13,(longlong)iVar10);
    break;
  case 0x19:
  case 0x1a:
  case 0x1b:
    for (puVar18 = *(undefined8 **)(lVar14 + 0x128); uVar23 = uVar22, uVar25 = uVar24,
        uVar19 = uVar26, puVar18 != (undefined8 *)0x0; puVar18 = (undefined8 *)*puVar18) {
      if (*(int *)(puVar18 + 2) == 0) {
        if (2 < *(int *)puVar18[1] - 0x19U) break;
        if (*(int *)puVar18[1] == iVar9) goto LAB_140012a30;
      }
    }
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
  case 0x20:
  case 0x21:
  case 0x22:
  case 0x25:
  case 0x26:
  case 0x4d:
  case 0x4f:
  case 0x50:
    goto switchD_140012623_caseD_1c;
  case 0x23:
  case 0x24:
    piVar12 = *(int **)(param_3 + 2);
    if ((*(int *)(lVar14 + 0x138) == 0) && (uVar23 = uVar22, uVar25 = uVar24, *piVar12 == 5)) {
      lVar14 = FUN_1400113b0(extraout_XMM0_Da,piVar12);
      if (lVar14 == 0) {
        FUN_140014520(param_1,piVar12);
        iVar10 = FUN_140014440();
        uVar25 = uVar19;
        if (iVar10 != 0) {
          return;
        }
      }
      else {
        for (puVar18 = *(undefined8 **)(param_1 + 0x148); puVar18 != (undefined8 *)0x0;
            puVar18 = (undefined8 *)puVar18[1]) {
          uVar19 = uVar22;
          if (((int *)*puVar18 == piVar12) ||
             (((int *)*puVar18 == param_3 && (puVar18 != *(undefined8 **)(param_1 + 0x148)))))
          goto LAB_140012da3;
        }
        uVar19 = 1;
        uVar25 = *(ulonglong *)(param_1 + 0x120);
        *(undefined8 *)(param_1 + 0x120) = *(undefined8 *)(lVar14 + 8);
      }
LAB_140012da3:
      piVar12 = (int *)FUN_140011940(param_1,piVar12);
      if ((piVar12 == (int *)0x0) ||
         ((uVar23 = uVar19, *piVar12 == 0x2f &&
          (piVar12 = (int *)FUN_1400115d0(piVar12,*(undefined4 *)(param_1 + 0x13c)),
          piVar12 == (int *)0x0)))) {
        if ((int)uVar19 != 0) {
          *(ulonglong *)(param_1 + 0x120) = uVar25;
        }
        goto switchD_140012623_caseD_34;
      }
    }
    iVar10 = *piVar12;
    piVar11 = piVar12;
    uVar19 = uVar26;
    if (((iVar10 != 0x23) && (iVar10 != *param_3)) && (piVar11 = param_3, iVar10 == 0x24)) {
      uVar19 = *(ulonglong *)(piVar12 + 2);
    }
    goto switchD_140012623_caseD_1c;
  case 0x27:
    puVar18 = *(undefined8 **)(param_3 + 2);
    if ((param_2 & 4) == 0) {
      FUN_14000fcf0(param_1,*puVar18,(longlong)*(int *)(puVar18 + 1));
    }
    else {
      FUN_14000fcf0(param_1,puVar18[2],(longlong)*(int *)(puVar18 + 3));
    }
    break;
  case 0x28:
    FUN_140012520(extraout_XMM0_Da,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x29:
    uVar16 = param_2 & 0xffffff9f;
    if ((param_2 & 0x20) != 0) {
      FUN_140013d20(extraout_XMM0_Da,uVar16,param_3,*(undefined8 *)(lVar14 + 0x128));
    }
    if (*(longlong *)(param_3 + 2) == 0) {
      if ((param_2 & 0x20) != 0) {
        return;
      }
    }
    else {
      if ((param_2 & 0x20) != 0) {
        FUN_140012520(param_1,uVar16);
        return;
      }
      if ((param_2 & 0x40) == 0) {
        local_e8 = *(undefined8 *)(param_1 + 0x128);
        *(undefined8 **)(param_1 + 0x128) = &local_e8;
        local_d0 = *(undefined8 *)(param_1 + 0x120);
        local_d8 = 0;
        piStack_e0 = param_3;
        FUN_140012520(param_1,uVar16,*(undefined8 *)(param_3 + 2));
        *(undefined8 *)(param_1 + 0x128) = local_e8;
        if (local_d8 != 0) {
          return;
        }
        FUN_14000fd40(param_1,0x20);
      }
    }
    FUN_140013d20(param_1,uVar16,param_3,*(undefined8 *)(param_1 + 0x128));
    break;
  case 0x2a:
    puVar18 = *(undefined8 **)(lVar14 + 0x128);
    *(undefined8 **)(lVar14 + 0x128) = &local_c8;
    local_b0 = *(undefined8 *)(lVar14 + 0x120);
    local_b8[0] = 0;
    uVar16 = 1;
    puVar2 = puVar18;
    while ((local_c8 = puVar18, local_c0 = param_3, puVar2 != (undefined8 *)0x0 &&
           (*(int *)puVar2[1] - 0x19U < 3))) {
      if (*(int *)(puVar2 + 2) == 0) {
        if (3 < uVar16) goto switchD_140012623_caseD_34;
        uVar27 = *(undefined4 *)((longlong)puVar2 + 4);
        uVar4 = *(undefined4 *)(puVar2 + 1);
        uVar5 = *(undefined4 *)((longlong)puVar2 + 0xc);
        uVar19 = (ulonglong)uVar16;
        puVar15 = &local_c8 + uVar19 * 4;
        uVar13 = *(undefined8 *)(param_1 + 0x128);
        uVar16 = uVar16 + 1;
        *(undefined4 *)puVar15 = *(undefined4 *)puVar2;
        *(undefined4 *)((longlong)&local_c8 + uVar19 * 0x20 + 4) = uVar27;
        *(undefined4 *)(&local_c0 + uVar19 * 4) = uVar4;
        *(undefined4 *)((longlong)&local_c0 + uVar19 * 0x20 + 4) = uVar5;
        uVar1 = puVar2[2];
        uVar21 = puVar2[3];
        *(undefined8 **)(param_1 + 0x128) = puVar15;
        *(undefined8 *)(local_b8 + uVar19 * 8) = uVar1;
        (&local_b0)[uVar19 * 4] = uVar21;
        *puVar15 = uVar13;
        *(undefined4 *)(puVar2 + 2) = 1;
      }
      puVar2 = (undefined8 *)*puVar2;
    }
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
    *(undefined8 **)(param_1 + 0x128) = puVar18;
    if (local_b8[0] == 0) {
      if (1 < uVar16) {
        puVar18 = &local_c0 + (ulonglong)uVar16 * 4;
        uVar19 = (ulonglong)(uVar16 - 1);
        do {
          puVar2 = puVar18 + -4;
          puVar18 = puVar18 + -4;
          FUN_140013ff0(param_1,param_2,*puVar2);
          uVar19 = uVar19 - 1;
        } while (uVar19 != 0);
        puVar18 = *(undefined8 **)(param_1 + 0x128);
      }
      FUN_140012440(param_1,param_2,param_3,puVar18);
    }
    break;
  case 0x2b:
  case 0x2d:
    local_e8 = *(undefined8 *)(lVar14 + 0x128);
    *(undefined8 **)(lVar14 + 0x128) = &local_e8;
    local_d0 = *(undefined8 *)(lVar14 + 0x120);
    local_d8 = 0;
    piStack_e0 = param_3;
    FUN_140012520(extraout_XMM0_Da,param_2,*(undefined8 *)(param_3 + 4));
    if (local_d8 == 0) {
      FUN_140013ff0(param_1,param_2,param_3);
    }
    *(undefined8 *)(param_1 + 0x128) = local_e8;
    break;
  case 0x2c:
    if (*(short *)((longlong)param_3 + 0x12) != 0) {
      FUN_14000fde0(extraout_XMM0_Da,"_Sat ");
    }
    if (*(undefined ***)(*(longlong *)(param_3 + 2) + 8) != &PTR_DAT_14005ffc0) {
      FUN_140012520(param_1,param_2);
      FUN_14000fd40(param_1,0x20);
    }
    pcVar17 = "_Accum";
    if ((short)param_3[4] == 0) {
      pcVar17 = "_Fract";
    }
    FUN_14000fde0(param_1,pcVar17);
    break;
  case 0x2e:
  case 0x2f:
    if (*(longlong *)(param_3 + 2) != 0) {
      FUN_140012520(extraout_XMM0_Da,param_2);
    }
    if (*(longlong *)(param_3 + 4) != 0) {
      if (0xfd < *(ulonglong *)(param_1 + 0x100)) {
        FUN_140013ce0(param_1);
      }
      FUN_14000fde0(param_1,&DAT_14005f588);
      lVar14 = *(longlong *)(param_1 + 0x100);
      iVar10 = *(int *)(param_1 + 0x140);
      FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
      if ((*(int *)(param_1 + 0x140) == iVar10) && (*(longlong *)(param_1 + 0x100) == lVar14)) {
        *(longlong *)(param_1 + 0x100) = *(longlong *)(param_1 + 0x100) + -2;
      }
    }
    break;
  case 0x30:
    FUN_14000fde0(extraout_XMM0_Da,"template parameter object for ");
LAB_140012a30:
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x31:
    uVar13 = *(undefined8 *)(param_3 + 4);
    if (*(longlong *)(param_3 + 2) != 0) {
      FUN_140012520(extraout_XMM0_Da,param_2);
    }
    FUN_14000fd40(param_1,0x7b);
    FUN_140012520(param_1,param_2,uVar13);
    FUN_14000fd40(param_1,0x7d);
    break;
  case 0x32:
    lVar14 = *(longlong *)(param_3 + 2);
    iVar10 = *(int *)(lVar14 + 0x10);
    FUN_14000fde0(extraout_XMM0_Da,"operator");
    pcVar17 = *(char **)(lVar14 + 8);
    if (('`' < *pcVar17) && (*pcVar17 < '{')) {
      FUN_14000fd40(param_1,CONCAT71((int7)((ulonglong)pcVar17 >> 8),0x20));
      pcVar17 = *(char **)(lVar14 + 8);
    }
    iVar9 = iVar10 + -1;
    if (pcVar17[(longlong)iVar10 + -1] != ' ') {
      iVar9 = iVar10;
    }
    FUN_14000fcf0(param_1,pcVar17,(longlong)iVar9);
    break;
  case 0x33:
    FUN_14000fde0(extraout_XMM0_Da,"operator ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
    break;
  default:
    goto switchD_140012623_caseD_34;
  case 0x35:
    FUN_14000fde0(extraout_XMM0_Da,"operator ");
    FUN_140013bb0(param_1,param_2,param_3);
    break;
  case 0x36:
    FUN_140013cc0(extraout_XMM0_Da,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x37:
    piVar11 = *(int **)(param_3 + 2);
    piVar12 = *(int **)(param_3 + 4);
    if (*piVar11 == 0x32) {
      pcVar17 = (char *)**(undefined8 **)(piVar11 + 2);
      if ((((*pcVar17 == 'a') && (pcVar17[1] == 'd')) &&
          ((pcVar17[2] == '\0' && ((*piVar12 == 3 && (**(int **)(piVar12 + 2) == 1)))))) &&
         (**(int **)(piVar12 + 4) == 0x29)) {
        piVar12 = *(int **)(piVar12 + 2);
      }
      if (*piVar12 == 0x39) {
        FUN_140014450(param_1,param_2,*(undefined8 *)(piVar12 + 2));
        FUN_140013cc0(param_1,param_2,piVar11);
        return;
      }
      if ((pcVar17 != (char *)0x0) && (*pcVar17 == 's')) {
        if ((pcVar17[1] == 'Z') && (pcVar17[2] == '\0')) {
          uVar13 = FUN_140011220(param_1,piVar12);
          uVar27 = FUN_1400121b0(uVar13);
          FUN_14000fd90(param_1,uVar27);
          return;
        }
        if (((*pcVar17 == 's') && (pcVar17[1] == 'P')) && (pcVar17[2] == '\0')) {
          uVar27 = FUN_14000fe00(param_1,piVar12);
          FUN_14000fd90(param_1,uVar27);
          return;
        }
      }
LAB_14001336e:
      FUN_140013cc0(param_1,param_2,piVar11);
      if (pcVar17 != (char *)0x0) {
        if (((*pcVar17 == 'g') && (pcVar17[1] == 's')) && (pcVar17[2] == '\0')) {
          FUN_140012520(param_1,param_2,piVar12);
          return;
        }
        if (((*pcVar17 == 's') && (pcVar17[1] == 't')) && (pcVar17[2] == '\0')) {
          FUN_14000fd40(param_1,0x28);
          FUN_140012520(param_1,param_2,piVar12);
          FUN_14000fd40(param_1,0x29);
          return;
        }
      }
    }
    else {
      if (*piVar11 != 0x34) goto LAB_14001336e;
      FUN_14000fd40(extraout_XMM0_Da,0x140000028);
      FUN_140012510(param_1,param_2,piVar11);
      FUN_14000fd40(param_1,0x29);
    }
    FUN_140014450(param_1,param_2,piVar12);
    break;
  case 0x38:
    if (**(int **)(param_3 + 4) == 0x39) {
      iVar10 = FUN_140015190(*(undefined8 *)(param_3 + 2));
      if (iVar10 == 0) {
        iVar10 = FUN_140011c70(param_1,param_2,param_3);
        if (iVar10 != 0) {
          return;
        }
        piVar11 = *(int **)(param_3 + 2);
        if (((*piVar11 == 0x32) &&
            (lVar14 = *(longlong *)(piVar11 + 2), *(int *)(lVar14 + 0x10) == 1)) &&
           (**(char **)(lVar14 + 8) == '>')) {
          FUN_14000fd40(param_1,CONCAT71((int7)((ulonglong)lVar14 >> 8),0x28));
          piVar11 = *(int **)(param_3 + 2);
        }
        pcVar17 = (char *)**(undefined8 **)(piVar11 + 2);
        if (((*pcVar17 == 'c') && (pcVar17[1] == 'l')) &&
           ((pcVar17[2] == '\0' &&
            (piVar11 = *(int **)(*(longlong *)(param_3 + 4) + 8), *piVar11 == 3)))) {
          if (**(int **)(piVar11 + 4) != 0x29) {
            FUN_140013cb0(param_1);
          }
          uVar13 = *(undefined8 *)(piVar11 + 2);
        }
        else {
          uVar13 = *(undefined8 *)(*(longlong *)(param_3 + 4) + 8);
        }
        FUN_140014450(param_1,param_2,uVar13);
        pcVar17 = (char *)**(undefined8 **)(*(longlong *)(param_3 + 2) + 8);
        if (((*pcVar17 == 'i') && (pcVar17[1] == 'x')) && (pcVar17[2] == '\0')) {
          FUN_14000fd40(param_1,0x5b);
          FUN_140012520(param_1,param_2,*(undefined8 *)(*(longlong *)(param_3 + 4) + 0x10));
          FUN_14000fd40(param_1,0x5d);
        }
        else {
          if (((*pcVar17 != 'c') || (pcVar17[1] != 'l')) || (pcVar17[2] != '\0')) {
            FUN_140013cc0(param_1,param_2);
          }
          FUN_140014450(param_1,param_2,*(undefined8 *)(*(longlong *)(param_3 + 4) + 0x10));
        }
        if (**(int **)(param_3 + 2) != 0x32) {
          return;
        }
        lVar14 = *(longlong *)(*(int **)(param_3 + 2) + 2);
        if (*(int *)(lVar14 + 0x10) != 1) {
          return;
        }
        if (**(char **)(lVar14 + 8) != '>') {
          return;
        }
        FUN_14000fd40(param_1,0x29);
        return;
      }
      FUN_140013cc0();
      FUN_14000fd40(param_1,0x3c);
      FUN_140012520(param_1,param_2,*(undefined8 *)(*(longlong *)(param_3 + 4) + 8));
      FUN_14000fde0(param_1,&DAT_140060c94);
      uVar13 = *(undefined8 *)(*(longlong *)(param_3 + 4) + 0x10);
      goto LAB_14001346b;
    }
    goto switchD_140012623_caseD_34;
  case 0x3a:
    if ((**(int **)(param_3 + 4) == 0x3b) && (**(int **)(*(int **)(param_3 + 4) + 4) == 0x3c)) {
      iVar10 = FUN_140011c70(extraout_XMM0_Da,param_2,param_3);
      if (iVar10 != 0) {
        return;
      }
      lVar14 = *(longlong *)(param_3 + 2);
      lVar20 = *(longlong *)(*(longlong *)(param_3 + 4) + 0x10);
      lVar3 = *(longlong *)(*(longlong *)(param_3 + 4) + 8);
      uVar13 = *(undefined8 *)(lVar20 + 8);
      lVar20 = *(longlong *)(lVar20 + 0x10);
      pcVar17 = (char *)**(undefined8 **)(lVar14 + 8);
      if (((*pcVar17 == 'q') && (pcVar17[1] == 'u')) && (pcVar17[2] == '\0')) {
        FUN_140014450(param_1,param_2,lVar3);
        FUN_140013cc0(param_1,param_2,lVar14);
        FUN_140014450(param_1,param_2,uVar13);
        FUN_14000fde0(param_1,&DAT_140060c98);
        FUN_140014450(param_1,param_2,lVar20);
        return;
      }
      FUN_14000fde0(param_1,&DAT_140060c9c);
      if (*(longlong *)(lVar3 + 8) != 0) {
        FUN_140014450(param_1,param_2,lVar3);
        FUN_14000fd40(param_1,0x20);
      }
      FUN_140012520(param_1,param_2,uVar13);
      if (lVar20 == 0) {
        return;
      }
      FUN_140014450(param_1,param_2,lVar20);
      return;
    }
switchD_140012623_caseD_34:
    FUN_140013cb0(param_1);
    break;
  case 0x3d:
  case 0x3e:
    if (**(int **)(param_3 + 2) == 0x27) {
      iVar10 = *(int *)(*(longlong *)(*(int **)(param_3 + 2) + 2) + 0x1c);
      switch(iVar10) {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
        piVar11 = *(int **)(param_3 + 4);
        if (*piVar11 == 0) {
          if (iVar9 == 0x3e) {
            FUN_14000fd40(param_1,0x14000002d);
            piVar11 = *(int **)(param_3 + 4);
          }
          FUN_140012520(param_1,param_2,piVar11);
          if (iVar10 == 2) {
            FUN_14000fd40(param_1,0x75);
            return;
          }
          if (iVar10 == 3) {
            FUN_14000fd40(param_1,0x6c);
            return;
          }
          if (iVar10 != 4) {
            if (iVar10 == 5) {
              FUN_14000fde0(param_1,&DAT_140060ca8);
              return;
            }
            if (iVar10 != 6) {
              return;
            }
            FUN_14000fde0(param_1,&DAT_140060cac);
            return;
          }
          FUN_14000fde0(param_1,&DAT_140060ca4);
          return;
        }
        break;
      case 7:
        piVar11 = *(int **)(param_3 + 4);
        if (((*piVar11 == 0) && (piVar11[4] == 1)) && (iVar9 == 0x3d)) {
          if (**(char **)(piVar11 + 2) == '0') {
            FUN_14000fde0(param_1,"false");
            return;
          }
          if (**(char **)(piVar11 + 2) == '1') {
            FUN_14000fde0(param_1,&DAT_140060cb8);
            return;
          }
        }
      }
    }
    FUN_14000fd40(param_1,0x140000028);
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    uVar27 = FUN_14000fd40(param_1,0x29);
    if (*param_3 == 0x3e) {
      uVar27 = FUN_14000fd40(param_1,0x2d);
    }
    if (iVar10 == 8) {
      FUN_14000fd40(param_1,0x5b);
      FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
      FUN_14000fd40(param_1,0x5d);
      return;
    }
switchD_140012623_caseD_7:
    FUN_140012520(uVar27,param_2,*(undefined8 *)(param_3 + 4));
    break;
  case 0x3f:
    FUN_14000fde0(extraout_XMM0_Da,"java resource ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x40:
    FUN_140012520(extraout_XMM0_Da,param_2,*(undefined8 *)(param_3 + 2));
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
    break;
  case 0x41:
    FUN_14000fd40(extraout_XMM0_Da,(char)param_3[2]);
    break;
  case 0x42:
    FUN_14000fd90(extraout_XMM0_Da,param_3[2]);
    break;
  case 0x43:
    FUN_14000fde0(extraout_XMM0_Da,"decltype (");
    uVar13 = *(undefined8 *)(param_3 + 2);
LAB_14001346b:
    FUN_140012520(param_1,param_2,uVar13);
    FUN_14000fd40(param_1,0x29);
    break;
  case 0x44:
    FUN_14000fde0(extraout_XMM0_Da,"global constructors keyed to ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x45:
    FUN_14000fde0(extraout_XMM0_Da,"global destructors keyed to ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x46:
    FUN_14000fde0(extraout_XMM0_Da,"{lambda(");
    *(int *)(param_1 + 0x138) = *(int *)(param_1 + 0x138) + 1;
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    *(int *)(param_1 + 0x138) = *(int *)(param_1 + 0x138) + -1;
    FUN_14000fde0(param_1,&DAT_140060d3c);
    FUN_14000fd90(param_1,param_3[4] + 1);
    FUN_14000fd40(param_1,0x7d);
    break;
  case 0x48:
    FUN_14000fde0(extraout_XMM0_Da,"{unnamed type#");
    FUN_14000fd90(param_1,param_3[2] + 1);
    FUN_14000fd40(param_1,0x7d);
    break;
  case 0x49:
    FUN_14000fde0(extraout_XMM0_Da,"transaction clone for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x4a:
    FUN_14000fde0(extraout_XMM0_Da,"non-transaction clone for ");
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 2));
    break;
  case 0x4b:
    lVar14 = FUN_140011220(extraout_XMM0_Da,*(undefined8 *)(param_3 + 2));
    if (lVar14 == 0) {
      FUN_140014450(param_1,param_2,*(undefined8 *)(param_3 + 2));
      FUN_14000fde0(param_1,&DAT_140060544);
    }
    else {
      iVar10 = FUN_1400121b0(lVar14);
      uVar13 = *(undefined8 *)(param_3 + 2);
      if (0 < iVar10) {
        do {
          iVar9 = (int)uVar19;
          *(int *)(param_1 + 0x13c) = iVar9;
          FUN_140012520(param_1,param_2,uVar13);
          if (iVar9 < iVar10 + -1) {
            FUN_14000fde0(param_1,&DAT_14005f588);
          }
          uVar19 = (ulonglong)(iVar9 + 1U);
        } while ((int)(iVar9 + 1U) < iVar10);
      }
    }
    break;
  case 0x4c:
    FUN_140012520(extraout_XMM0_Da,param_2,*(undefined8 *)(param_3 + 2));
    pcVar17 = "[abi:";
    goto LAB_140012654;
  case 0x4e:
    FUN_140012520(extraout_XMM0_Da,param_2,*(undefined8 *)(param_3 + 2));
    pcVar17 = " [clone ";
LAB_140012654:
    FUN_14000fde0(param_1,pcVar17);
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
    FUN_14000fd40(param_1,0x5d);
  }
  return;
joined_r0x0001400127c4:
  if (iVar9 == 0) {
LAB_140012828:
    if (iVar10 == 4) {
      *(undefined8 **)(param_1 + 0x120) = &local_e8;
      local_e8 = uVar21;
      piStack_e0 = piVar11;
    }
    FUN_140012520(param_1,param_2,*(undefined8 *)(param_3 + 4));
    if (*piVar11 == 4) {
      *(undefined8 *)(param_1 + 0x120) = local_e8;
    }
    if ((int)uVar22 != 0) {
      puVar18 = &local_c0 + uVar22 * 4;
      do {
        uVar16 = (int)uVar22 - 1;
        uVar22 = (ulonglong)uVar16;
        if (*(int *)(puVar18 + -3) == 0) {
          FUN_14000fd40(param_1,0x20);
          FUN_140013ff0(param_1,param_2,puVar18[-4]);
        }
        puVar18 = puVar18 + -4;
      } while (uVar16 != 0);
    }
    *(undefined8 *)(param_1 + 0x128) = uVar13;
    return;
  }
  uVar16 = (uint)uVar22;
  if (3 < uVar16) goto switchD_140012623_caseD_34;
  uVar19 = (ulonglong)(uVar16 - 1);
  lVar20 = uVar19 * 0x20;
  lVar14 = uVar22 * 0x20;
  puVar18 = &local_c8 + uVar22 * 4;
  *(undefined8 **)(param_1 + 0x128) = puVar18;
  uVar27 = *(undefined4 *)((longlong)&local_c8 + lVar20 + 4);
  uVar4 = *(undefined4 *)(&local_c0 + uVar19 * 4);
  uVar5 = *(undefined4 *)((longlong)&local_c0 + lVar20 + 4);
  iVar10 = local_b8[uVar19 * 8];
  iVar9 = local_b8[uVar19 * 8 + 1];
  uVar6 = *(undefined4 *)(&local_b0 + uVar19 * 4);
  uVar7 = *(undefined4 *)((longlong)&local_b0 + lVar20 + 4);
  (&local_c0)[uVar19 * 4] = piVar11;
  *(undefined4 *)puVar18 = *(undefined4 *)(&local_c8 + uVar19 * 4);
  *(undefined4 *)((longlong)&local_c8 + lVar14 + 4) = uVar27;
  *(undefined4 *)(&local_c0 + uVar22 * 4) = uVar4;
  *(undefined4 *)((longlong)&local_c0 + lVar14 + 4) = uVar5;
  local_b8[uVar19 * 8] = 0;
  local_b8[uVar22 * 8] = iVar10;
  local_b8[uVar22 * 8 + 1] = iVar9;
  *(undefined4 *)(&local_b0 + uVar22 * 4) = uVar6;
  *(undefined4 *)((longlong)&local_b0 + lVar14 + 4) = uVar7;
  *puVar18 = &local_c8 + uVar19 * 4;
  (&local_b0)[uVar19 * 4] = uVar21;
  piVar11 = *(int **)(piVar11 + 2);
  iVar10 = *piVar11;
  iVar9 = FUN_1400150d0(iVar10);
  uVar22 = (ulonglong)(uVar16 + 1);
  goto joined_r0x0001400127c4;
switchD_140012623_caseD_1c:
  local_e8 = *(undefined8 *)(param_1 + 0x128);
  *(undefined8 **)(param_1 + 0x128) = &local_e8;
  local_d0 = *(undefined8 *)(param_1 + 0x120);
  local_d8 = 0;
  if (uVar19 == 0) {
    uVar19 = *(ulonglong *)(piVar11 + 2);
  }
  piStack_e0 = piVar11;
  FUN_140012520(param_1,param_2,uVar19);
  if (local_d8 == 0) {
    FUN_140013ff0(param_1,param_2,piVar11);
  }
  *(undefined8 *)(param_1 + 0x128) = local_e8;
  if ((int)uVar23 == 0) {
    return;
  }
  *(ulonglong *)(param_1 + 0x120) = uVar25;
  return;
}


/* ================= FUN_140013bb0 @ 140013bb0  size=255  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400125b0 */
/* CALLEES: FUN_14000fd40, FUN_140011800, FUN_140012520 */

void FUN_140013bb0(longlong param_1,ulonglong param_2,longlong param_3)

{
  char cVar1;
  undefined4 extraout_XMM0_Da;
  undefined4 extraout_XMM0_Da_00;
  undefined8 local_18;
  longlong lStack_10;
  
  local_18 = 0;
  lStack_10 = 0;
  if (*(longlong *)(param_1 + 0x170) != 0) {
    local_18 = *(undefined8 *)(param_1 + 0x120);
    *(undefined8 **)(param_1 + 0x120) = &local_18;
    lStack_10 = *(longlong *)(param_1 + 0x170);
  }
  if (**(int **)(param_3 + 8) == 4) {
    FUN_140012520(param_1,param_2,*(undefined8 *)(*(int **)(param_3 + 8) + 2));
    if (*(longlong *)(param_1 + 0x170) != 0) {
      *(undefined8 *)(param_1 + 0x120) = local_18;
    }
    cVar1 = FUN_140011800(param_1);
    if (cVar1 == '<') {
      FUN_14000fd40(extraout_XMM0_Da,0x20);
    }
    FUN_14000fd40(param_1,0x3c);
    FUN_140012520(param_1,param_2 & 0xffffffff,*(undefined8 *)(*(longlong *)(param_3 + 8) + 0x10));
    cVar1 = FUN_140011800(param_1);
    if (cVar1 == '>') {
      FUN_14000fd40(extraout_XMM0_Da_00,0x20);
    }
    FUN_14000fd40(param_1,0x3e);
  }
  else {
    FUN_140012520(param_1);
    if (*(longlong *)(param_1 + 0x170) != 0) {
      *(undefined8 *)(param_1 + 0x120) = local_18;
      return;
    }
  }
  return;
}


/* ================= FUN_140013cb0 @ 140013cb0  size=11  callers=3 callees=0 ================= */
/* CALLERS: FUN_140012520, FUN_1400125b0, FUN_140014520 */

void FUN_140013cb0(longlong param_1)

{
  *(undefined4 *)(param_1 + 0x130) = 1;
  return;
}


/* ================= FUN_140013cc0 @ 140013cc0  size=28  callers=2 callees=2 ================= */
/* CALLERS: FUN_140011c70, FUN_1400125b0 */
/* CALLEES: FUN_14000fcf0, FUN_140012520 */

void FUN_140013cc0(undefined8 param_1,undefined8 param_2,int *param_3)

{
  if (*param_3 == 0x32) {
    FUN_14000fcf0(param_1,*(undefined8 *)(*(longlong *)(param_3 + 2) + 8),
                  (longlong)*(int *)(*(longlong *)(param_3 + 2) + 0x10));
    return;
  }
  FUN_140012520();
  return;
}


/* ================= FUN_140013ce0 @ 140013ce0  size=63  callers=3 callees=0 ================= */
/* CALLERS: FUN_14000f390, FUN_14000fd40, FUN_1400125b0 */

void FUN_140013ce0(longlong param_1)

{
  *(undefined1 *)(param_1 + *(longlong *)(param_1 + 0x100)) = 0;
  (**(code **)(param_1 + 0x110))
            (param_1,*(undefined8 *)(param_1 + 0x100),*(undefined8 *)(param_1 + 0x118));
  *(int *)(param_1 + 0x140) = *(int *)(param_1 + 0x140) + 1;
  *(undefined8 *)(param_1 + 0x100) = 0;
  return;
}


/* ================= FUN_140013d20 @ 140013d20  size=316  callers=2 callees=4 ================= */
/* CALLERS: FUN_1400125b0, FUN_1400142a0 */
/* CALLEES: FUN_14000fd40, FUN_140011800, FUN_140012520, FUN_1400142a0 */

void FUN_140013d20(longlong param_1,undefined4 param_2,longlong param_3,undefined8 *param_4)

{
  undefined8 *puVar1;
  char cVar2;
  undefined8 uVar3;
  
  puVar1 = param_4;
joined_r0x000140013d4c:
  if ((puVar1 == (undefined8 *)0x0) || (*(int *)(puVar1 + 2) != 0)) {
    uVar3 = *(undefined8 *)(param_1 + 0x128);
    *(undefined8 *)(param_1 + 0x128) = 0;
    FUN_1400142a0(param_1,param_2,param_4,0);
  }
  else {
    switch(*(undefined4 *)puVar1[1]) {
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x21:
    case 0x25:
    case 0x26:
    case 0x2b:
switchD_140013d7e_caseD_19:
      cVar2 = FUN_140011800(param_1);
      if (cVar2 != ' ') {
        FUN_14000fd40();
      }
      break;
    default:
      goto switchD_140013d7e_caseD_1c;
    case 0x22:
    case 0x23:
    case 0x24:
      cVar2 = FUN_140011800(param_1);
      if ((cVar2 != '(') && (cVar2 = FUN_140011800(), cVar2 != '*'))
      goto switchD_140013d7e_caseD_19;
    }
    FUN_14000fd40(param_1,0x28);
    uVar3 = *(undefined8 *)(param_1 + 0x128);
    *(undefined8 *)(param_1 + 0x128) = 0;
    FUN_1400142a0(param_1,param_2,param_4,0);
    FUN_14000fd40(param_1,0x29);
  }
  FUN_14000fd40(param_1,0x28);
  if (*(longlong *)(param_3 + 0x10) != 0) {
    FUN_140012520(param_1,param_2);
  }
  FUN_14000fd40(param_1,0x29);
  FUN_1400142a0(param_1,param_2,param_4,1);
  *(undefined8 *)(param_1 + 0x128) = uVar3;
  return;
switchD_140013d7e_caseD_1c:
  puVar1 = (undefined8 *)*puVar1;
  goto joined_r0x000140013d4c;
}


/* ================= FUN_140013e80 @ 140013e80  size=174  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000f390 */
/* CALLEES: FUN_140010180 */

void FUN_140013e80(longlong param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  *(undefined8 *)(param_1 + 0x110) = param_2;
  *(undefined8 *)(param_1 + 0x118) = param_3;
  *(undefined8 *)(param_1 + 0x100) = 0;
  *(undefined1 *)(param_1 + 0x108) = 0;
  *(undefined8 *)(param_1 + 0x120) = 0;
  *(undefined8 *)(param_1 + 0x128) = 0;
  *(undefined8 *)(param_1 + 0x13c) = 0;
  *(undefined8 *)(param_1 + 0x130) = 0;
  *(undefined4 *)(param_1 + 0x138) = 0;
  *(undefined8 *)(param_1 + 0x148) = 0;
  *(undefined8 *)(param_1 + 0x150) = 0;
  *(undefined8 *)(param_1 + 0x158) = 0;
  *(undefined8 *)(param_1 + 0x160) = 0;
  *(undefined8 *)(param_1 + 0x168) = 0;
  FUN_140010180(param_1 + 0x16c,param_1 + 0x15c,param_4);
  *(undefined8 *)(param_1 + 0x170) = 0;
  *(int *)(param_1 + 0x16c) = *(int *)(param_1 + 0x16c) * *(int *)(param_1 + 0x15c);
  return;
}


/* ================= FUN_140013f30 @ 140013f30  size=184  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400125b0 */
/* CALLEES: FUN_14000fd40 */

void FUN_140013f30(undefined8 param_1,char *param_2,int param_3)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  char *pcVar4;
  char *pcVar5;
  
  pcVar4 = param_2 + param_3;
  do {
    if (pcVar4 <= param_2) {
      return;
    }
    if ((((3 < (longlong)pcVar4 - (longlong)param_2) && (*param_2 == '_')) && (param_2[1] == '_'))
       && (param_2[2] == 'U')) {
      uVar3 = 0;
      for (pcVar5 = param_2 + 3; pcVar5 < pcVar4; pcVar5 = pcVar5 + 1) {
        cVar1 = *pcVar5;
        iVar2 = (int)cVar1;
        if ((byte)(cVar1 - 0x30U) < 10) {
          iVar2 = iVar2 + -0x30;
        }
        else if ((byte)(cVar1 + 0xbfU) < 6) {
          iVar2 = iVar2 + -0x37;
        }
        else {
          if (5 < (byte)(cVar1 + 0x9fU)) {
            if ((cVar1 == '_') && (uVar3 < 0x100)) goto LAB_140013fb6;
            break;
          }
          iVar2 = iVar2 + -0x57;
        }
        uVar3 = uVar3 * 0x10 + iVar2;
      }
    }
    pcVar5 = param_2;
LAB_140013fb6:
    FUN_14000fd40(param_1);
    param_2 = pcVar5 + 1;
  } while( true );
}


/* ================= FUN_140013ff0 @ 140013ff0  size=529  callers=2 callees=4 ================= */
/* CALLERS: FUN_1400125b0, FUN_1400142a0 */
/* CALLEES: FUN_14000fd40, FUN_14000fde0, FUN_140011800, FUN_140012520 */

void FUN_140013ff0(undefined8 param_1,ulonglong param_2,undefined4 *param_3)

{
  char cVar1;
  undefined8 uVar2;
  char *pcVar3;
  ulonglong uVar4;
  
  uVar4 = param_2 & 0xffffffff;
  switch(*param_3) {
  case 3:
    param_3 = *(undefined4 **)(param_3 + 2);
    break;
  case 0x19:
  case 0x1c:
    FUN_14000fde0(param_1," restrict");
    return;
  case 0x1a:
  case 0x1d:
    FUN_14000fde0(param_1," volatile");
    return;
  case 0x1b:
  case 0x1e:
    FUN_14000fde0(param_1," const");
    return;
  case 0x1f:
    FUN_14000fd40(param_1,0x20);
  case 0x23:
    uVar2 = 0x26;
    goto LAB_1400140dd;
  case 0x20:
    FUN_14000fd40(param_1,0x20);
  case 0x24:
    FUN_14000fde0(param_1,&DAT_140060484);
    return;
  case 0x21:
    FUN_14000fd40(param_1,0x20);
    param_3 = *(undefined4 **)(param_3 + 4);
    param_2 = uVar4;
    break;
  case 0x22:
    if ((param_2 & 4) != 0) {
      return;
    }
    uVar2 = 0x2a;
    goto LAB_1400140dd;
  case 0x25:
    FUN_14000fde0(param_1,"complex ");
    return;
  case 0x26:
    FUN_14000fde0(param_1,"imaginary ");
    return;
  case 0x2b:
    cVar1 = FUN_140011800(param_1);
    if (cVar1 != '(') {
      FUN_14000fd40();
    }
    FUN_140012520(param_1,uVar4,*(undefined8 *)(param_3 + 2));
    FUN_14000fde0(param_1,&DAT_140060dd4);
    return;
  case 0x2d:
    FUN_14000fde0(param_1," __vector(");
    uVar2 = *(undefined8 *)(param_3 + 2);
    goto LAB_1400140d1;
  case 0x4d:
    FUN_14000fde0(param_1," transaction_safe");
    return;
  case 0x4f:
    pcVar3 = " noexcept";
    goto LAB_1400140b4;
  case 0x50:
    pcVar3 = " throw";
LAB_1400140b4:
    FUN_14000fde0(param_1,pcVar3);
    if (*(longlong *)(param_3 + 4) == 0) {
      return;
    }
    FUN_14000fd40(param_1,0x28);
    uVar2 = *(undefined8 *)(param_3 + 4);
LAB_1400140d1:
    FUN_140012520(param_1,uVar4,uVar2);
    uVar2 = 0x29;
LAB_1400140dd:
    FUN_14000fd40(param_1,uVar2);
    return;
  }
  FUN_140012520(param_1,param_2,param_3);
  return;
}


/* ================= FUN_1400142a0 @ 1400142a0  size=403  callers=2 callees=9 ================= */
/* CALLERS: FUN_140012440, FUN_140013d20 */
/* CALLEES: FUN_14000fd40, FUN_14000fd90, FUN_14000fde0, FUN_140012440, FUN_140012520, FUN_140013d20, FUN_140013ff0, FUN_140014440, FUN_1400150d0 */

void FUN_1400142a0(longlong param_1,uint param_2,undefined8 *param_3,int param_4)

{
  undefined8 uVar1;
  undefined8 uVar2;
  int iVar3;
  int *piVar4;
  
  do {
    if (param_3 == (undefined8 *)0x0) {
      return;
    }
    iVar3 = FUN_140014440(param_1);
    if (iVar3 != 0) {
      return;
    }
    if ((*(int *)(param_3 + 2) == 0) &&
       ((param_4 != 0 || (iVar3 = FUN_1400150d0(*(undefined4 *)param_3[1]), iVar3 == 0)))) {
      *(undefined4 *)(param_3 + 2) = 1;
      uVar1 = *(undefined8 *)(param_1 + 0x120);
      *(undefined8 *)(param_1 + 0x120) = param_3[3];
      iVar3 = *(int *)param_3[1];
      if (iVar3 == 0x29) {
        FUN_140013d20(param_1,param_2,(int *)param_3[1],*param_3);
        goto LAB_140014411;
      }
      if (iVar3 == 0x2a) {
        FUN_140012440();
        goto LAB_140014411;
      }
      if (iVar3 == 2) {
        uVar2 = *(undefined8 *)(param_1 + 0x128);
        *(undefined8 *)(param_1 + 0x128) = 0;
        FUN_140012520(param_1,param_2,*(undefined8 *)(param_3[1] + 8));
        *(undefined8 *)(param_1 + 0x128) = uVar2;
        if ((param_2 & 4) == 0) {
          FUN_14000fde0(param_1,&DAT_140060560);
        }
        else {
          FUN_14000fd40(param_1,0x2e);
        }
        piVar4 = *(int **)(param_3[1] + 0x10);
        iVar3 = *piVar4;
        if (iVar3 == 0x47) {
          FUN_14000fde0(param_1,"{default arg#");
          FUN_14000fd90(param_1,piVar4[4] + 1);
          FUN_14000fde0(param_1,&DAT_140060a80);
          piVar4 = *(int **)(piVar4 + 2);
          iVar3 = *piVar4;
        }
        iVar3 = FUN_1400150d0(iVar3);
        while (iVar3 != 0) {
          piVar4 = *(int **)(piVar4 + 2);
          iVar3 = FUN_1400150d0(*piVar4);
        }
        FUN_140012520(param_1,param_2,piVar4);
LAB_140014411:
        *(undefined8 *)(param_1 + 0x120) = uVar1;
        return;
      }
      FUN_140013ff0();
      *(undefined8 *)(param_1 + 0x120) = uVar1;
    }
    param_3 = (undefined8 *)*param_3;
  } while( true );
}


/* ================= FUN_140014440 @ 140014440  size=12  callers=3 callees=0 ================= */
/* CALLERS: FUN_14000f390, FUN_1400125b0, FUN_1400142a0 */

bool FUN_140014440(longlong param_1)

{
  return *(int *)(param_1 + 0x130) != 0;
}


/* ================= FUN_140014450 @ 140014450  size=118  callers=2 callees=2 ================= */
/* CALLERS: FUN_140011c70, FUN_1400125b0 */
/* CALLEES: FUN_14000fd40, FUN_140012520 */

void FUN_140014450(undefined8 param_1,undefined4 param_2,uint *param_3)

{
  if ((*param_3 < 0x32) && ((0x2000000000043U >> ((longlong)(int)*param_3 & 0x3fU) & 1) != 0)) {
    FUN_140012520(param_1);
    return;
  }
  FUN_14000fd40(param_1,0x28);
  FUN_140012520(param_1,param_2,param_3);
  FUN_14000fd40(param_1,0x29);
  return;
}


/* ================= FUN_1400144d0 @ 1400144d0  size=73  callers=2 callees=1 ================= */
/* CALLERS: FUN_140011320, FUN_140011f10 */
/* CALLEES: FUN_1400119b0 */

undefined8 FUN_1400144d0(longlong param_1,undefined8 param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  cVar1 = **(char **)(param_1 + 0x18);
  if ((cVar1 != 'R') && (cVar1 != 'O')) {
    return param_2;
  }
  *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + (cVar1 != 'R') + 2;
  *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
  uVar2 = FUN_1400119b0(param_1,(cVar1 != 'R') + '\x1f',param_2,0);
  return uVar2;
}


/* ================= FUN_140014520 @ 140014520  size=136  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400125b0 */
/* CALLEES: FUN_140013cb0 */

void FUN_140014520(longlong param_1,undefined8 param_2)

{
  int iVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  
  iVar1 = *(int *)(param_1 + 0x158);
  if (iVar1 < *(int *)(param_1 + 0x15c)) {
    puVar4 = (undefined8 *)((longlong)iVar1 * 0x10 + *(longlong *)(param_1 + 0x150));
    *(int *)(param_1 + 0x158) = iVar1 + 1;
    *puVar4 = param_2;
    puVar2 = *(undefined8 **)(param_1 + 0x120);
    puVar4 = puVar4 + 1;
    while( true ) {
      if (puVar2 == (undefined8 *)0x0) {
        *puVar4 = 0;
        return;
      }
      iVar1 = *(int *)(param_1 + 0x168);
      if (*(int *)(param_1 + 0x16c) <= iVar1) break;
      puVar3 = (undefined8 *)((longlong)iVar1 * 0x10 + *(longlong *)(param_1 + 0x160));
      *(int *)(param_1 + 0x168) = iVar1 + 1;
      puVar3[1] = puVar2[1];
      *puVar4 = puVar3;
      puVar2 = (undefined8 *)*puVar2;
      puVar4 = puVar3;
    }
  }
  FUN_140013cb0();
  return;
}


/* ================= FUN_1400145b0 @ 1400145b0  size=46  callers=4 callees=2 ================= */
/* CALLERS: FUN_14000f540, FUN_14000fc40, FUN_140012070, FUN_140014e00 */
/* CALLEES: FUN_140011540, FUN_140011fc0 */

undefined8 FUN_1400145b0(longlong param_1)

{
  int iVar1;
  undefined8 uVar2;
  
  iVar1 = FUN_140011fc0();
  if (iVar1 < 1) {
    return 0;
  }
  uVar2 = FUN_140011540(param_1,iVar1);
  *(undefined8 *)(param_1 + 0x40) = uVar2;
  return uVar2;
}


/* ================= FUN_1400145e0 @ 1400145e0  size=1036  callers=1 callees=9 ================= */
/* CALLERS: FUN_140010960 */
/* CALLEES: FUN_14000f540, FUN_14000ffc0, FUN_140010960, FUN_140011610, FUN_1400119b0, FUN_140011dd0, FUN_140011fc0, FUN_140012040, FUN_140014be0 */

undefined8 FUN_1400145e0(longlong param_1)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  
  iVar3 = *(int *)(param_1 + 0x48);
  *(int *)(param_1 + 0x48) = iVar3 + 0x14;
  pcVar2 = *(char **)(param_1 + 0x18);
  if (*pcVar2 == 'T') {
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    cVar1 = pcVar2[1];
    if (cVar1 != '\0') {
      *(char **)(param_1 + 0x18) = pcVar2 + 2;
      switch(cVar1) {
      case 'A':
        uVar4 = FUN_140014be0(param_1);
        uVar4 = FUN_1400119b0(param_1,0x30,uVar4);
        return uVar4;
      case 'C':
        uVar4 = FUN_14000f540(param_1);
        iVar3 = FUN_140011fc0(param_1);
        if ((-1 < iVar3) && (**(char **)(param_1 + 0x18) == '_')) {
          *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
          uVar5 = FUN_14000f540(param_1);
          *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + 5;
          uVar4 = FUN_1400119b0(param_1,0xb,uVar5,uVar4);
          return uVar4;
        }
        break;
      case 'F':
        uVar4 = FUN_14000f540(param_1);
        uVar4 = FUN_1400119b0(param_1,0xe,uVar4);
        return uVar4;
      case 'H':
        uVar4 = FUN_140011dd0(param_1);
        uVar4 = FUN_1400119b0(param_1,0x14,uVar4);
        return uVar4;
      case 'I':
        uVar4 = FUN_14000f540(param_1);
        uVar4 = FUN_1400119b0(param_1,0xc,uVar4);
        return uVar4;
      case 'J':
        uVar4 = FUN_14000f540(param_1);
        uVar4 = FUN_1400119b0(param_1,0x12,uVar4);
        return uVar4;
      case 'S':
        uVar4 = FUN_14000f540(param_1);
        uVar4 = FUN_1400119b0(param_1,0xd,uVar4);
        return uVar4;
      case 'T':
        *(int *)(param_1 + 0x48) = iVar3 + 10;
        uVar4 = FUN_14000f540(param_1);
        uVar4 = FUN_1400119b0(param_1,10,uVar4);
        return uVar4;
      case 'V':
        *(int *)(param_1 + 0x48) = iVar3 + 0xf;
        uVar4 = FUN_14000f540(param_1);
        uVar4 = FUN_1400119b0(param_1,9,uVar4);
        return uVar4;
      case 'W':
        uVar4 = FUN_140011dd0(param_1);
        uVar4 = FUN_1400119b0(param_1,0x15,uVar4);
        return uVar4;
      case 'c':
        iVar3 = FUN_14000ffc0(param_1,0);
        if ((iVar3 != 0) && (iVar3 = FUN_14000ffc0(param_1,0), iVar3 != 0)) {
          uVar4 = FUN_140010960(param_1,0);
          uVar4 = FUN_1400119b0(param_1,0x11,uVar4);
          return uVar4;
        }
        break;
      case 'h':
        iVar3 = FUN_14000ffc0(param_1,0x68);
        if (iVar3 != 0) {
          uVar4 = FUN_140010960(param_1,0);
          uVar4 = FUN_1400119b0(param_1,0xf,uVar4);
          return uVar4;
        }
        break;
      case 'v':
        iVar3 = FUN_14000ffc0(param_1,0x76);
        if (iVar3 != 0) {
          uVar4 = FUN_140010960(param_1,0);
          uVar4 = FUN_1400119b0(param_1,0x10,uVar4);
          return uVar4;
        }
      }
    }
  }
  else if (*pcVar2 == 'G') {
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    cVar1 = pcVar2[1];
    if (cVar1 != '\0') {
      *(char **)(param_1 + 0x18) = pcVar2 + 2;
      if (cVar1 == 'A') {
        uVar4 = FUN_140010960(param_1,0);
        uVar4 = FUN_1400119b0(param_1,0x17,uVar4);
        return uVar4;
      }
      if (cVar1 == 'R') {
        uVar4 = FUN_140011dd0(param_1);
        uVar5 = FUN_140012040(param_1);
        uVar4 = FUN_1400119b0(param_1,0x16,uVar4,uVar5);
        return uVar4;
      }
      if (cVar1 == 'T') {
        cVar1 = pcVar2[2];
        if ((cVar1 != '\0') && (*(char **)(param_1 + 0x18) = pcVar2 + 3, cVar1 == 'n')) {
          uVar4 = FUN_140010960(param_1,0);
          uVar4 = FUN_1400119b0(param_1,0x4a,uVar4);
          return uVar4;
        }
        uVar4 = FUN_140010960(param_1,0);
        uVar4 = FUN_1400119b0(param_1,0x49,uVar4);
        return uVar4;
      }
      if (cVar1 == 'V') {
        uVar4 = FUN_140011dd0(param_1);
        uVar4 = FUN_1400119b0(param_1,0x13,uVar4);
        return uVar4;
      }
      if (cVar1 == 'r') {
        uVar4 = FUN_140011610(param_1);
        return uVar4;
      }
    }
  }
  return 0;
}


/* ================= FUN_140014a60 @ 140014a60  size=375  callers=3 callees=3 ================= */
/* CALLERS: FUN_14000f540, FUN_140011dd0, FUN_140012310 */
/* CALLEES: FUN_14000fc40, FUN_14000fcc0, FUN_140011c20 */

undefined8 FUN_140014a60(longlong param_1,int param_2)

{
  char *pcVar1;
  int iVar2;
  undefined8 uVar3;
  byte *pbVar4;
  ulonglong uVar5;
  byte bVar6;
  uint uVar7;
  uint uVar8;
  
  pcVar1 = *(char **)(param_1 + 0x18);
  if (*pcVar1 == 'S') {
    pbVar4 = (byte *)(pcVar1 + 1);
    *(byte **)(param_1 + 0x18) = pbVar4;
    bVar6 = *pbVar4;
    if (bVar6 != 0) {
      pbVar4 = (byte *)(pcVar1 + 2);
      *(byte **)(param_1 + 0x18) = pbVar4;
      if (((byte)(bVar6 - 0x30) < 0x30) &&
         ((0x87fffffe03ffU >> ((ulonglong)(bVar6 - 0x30) & 0x3f) & 1) != 0)) {
        uVar5 = 0;
        if (bVar6 != 0x5f) {
          do {
            uVar8 = (uint)uVar5;
            if ((byte)(bVar6 - 0x30) < 10) {
              uVar7 = (int)(char)bVar6 + (uVar8 * 9 + -0xc) * 4;
            }
            else {
              if (0x19 < (byte)(bVar6 + 0xbf)) {
                return 0;
              }
              uVar7 = uVar8 * 0x24 + -0x37 + (int)(char)bVar6;
            }
            uVar5 = (ulonglong)uVar7;
            if (uVar7 < uVar8) {
              return 0;
            }
            bVar6 = **(byte **)(param_1 + 0x18);
          } while ((bVar6 == 0) ||
                  (*(byte **)(param_1 + 0x18) = *(byte **)(param_1 + 0x18) + 1, bVar6 != 0x5f));
          uVar5 = (ulonglong)(uVar7 + 1);
        }
        if (*(uint *)(param_1 + 0x38) <= (uint)uVar5) {
          return 0;
        }
        return *(undefined8 *)(*(longlong *)(param_1 + 0x30) + uVar5 * 8);
      }
    }
    uVar8 = *(uint *)(param_1 + 0x10) >> 3 & 1;
    if (((uVar8 == 0) && (param_2 != 0)) && (*pbVar4 == 0x43 || *pbVar4 == 0x44)) {
      uVar8 = 1;
    }
    pbVar4 = &DAT_1400602e0;
    do {
      if (bVar6 == *pbVar4) {
        if (*(longlong *)(pbVar4 + 0x28) != 0) {
          uVar3 = FUN_140011c20(param_1,*(longlong *)(pbVar4 + 0x28),*(undefined4 *)(pbVar4 + 0x30))
          ;
          *(undefined8 *)(param_1 + 0x40) = uVar3;
        }
        if (uVar8 == 0) {
          uVar3 = *(undefined8 *)(pbVar4 + 8);
          iVar2 = *(int *)(pbVar4 + 0x10);
        }
        else {
          uVar3 = *(undefined8 *)(pbVar4 + 0x18);
          iVar2 = *(int *)(pbVar4 + 0x20);
        }
        *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x48) + iVar2;
        uVar3 = FUN_140011c20(param_1,uVar3);
        if (**(char **)(param_1 + 0x18) == 'B') {
          uVar3 = FUN_14000fc40(param_1);
          iVar2 = FUN_14000fcc0(param_1,uVar3);
          if (iVar2 == 0) {
            return 0;
          }
        }
        return uVar3;
      }
      pbVar4 = pbVar4 + 0x38;
    } while (pbVar4 < &DAT_140060468);
  }
  return 0;
}


/* ================= FUN_140014be0 @ 140014be0  size=121  callers=2 callees=4 ================= */
/* CALLERS: FUN_1400145e0, FUN_140014c80 */
/* CALLEES: FUN_14000f540, FUN_140010aa0, FUN_140010bb0, FUN_140014c60 */

undefined8 FUN_140014be0(longlong param_1)

{
  char cVar1;
  undefined8 uVar2;
  
  cVar1 = **(char **)(param_1 + 0x18);
  if ((cVar1 == 'I') || (cVar1 == 'J')) {
    uVar2 = FUN_140014c60(param_1);
    return uVar2;
  }
  if (cVar1 == 'L') {
    uVar2 = FUN_140010aa0(param_1);
    return uVar2;
  }
  if (cVar1 != 'X') {
    uVar2 = FUN_14000f540(param_1);
    return uVar2;
  }
  *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
  uVar2 = FUN_140010bb0(param_1);
  if (**(char **)(param_1 + 0x18) == 'E') {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    return uVar2;
  }
  return 0;
}


/* ================= FUN_140014c60 @ 140014c60  size=29  callers=5 callees=1 ================= */
/* CALLERS: FUN_14000f540, FUN_140010be0, FUN_140011dd0, FUN_140012310, FUN_140014be0 */
/* CALLEES: FUN_140014c80 */

undefined8 FUN_140014c60(longlong param_1)

{
  undefined8 uVar1;
  
  if (1 < (byte)(**(char **)(param_1 + 0x18) + 0xb7U)) {
    return 0;
  }
  *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
  uVar1 = FUN_140014c80();
  return uVar1;
}


/* ================= FUN_140014c80 @ 140014c80  size=198  callers=2 callees=2 ================= */
/* CALLERS: FUN_140010be0, FUN_140014c60 */
/* CALLEES: FUN_1400119b0, FUN_140014be0 */

undefined8 FUN_140014c80(longlong param_1)

{
  longlong *plVar1;
  undefined8 uVar2;
  longlong lVar3;
  longlong local_res8;
  
  uVar2 = *(undefined8 *)(param_1 + 0x40);
  if (**(char **)(param_1 + 0x18) == 'E') {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    uVar2 = FUN_1400119b0(param_1,0x2f,0);
    return uVar2;
  }
  plVar1 = &local_res8;
  local_res8 = 0;
  lVar3 = FUN_140014be0();
  while( true ) {
    if (lVar3 == 0) {
      return 0;
    }
    lVar3 = FUN_1400119b0(param_1,0x2f,lVar3);
    *plVar1 = lVar3;
    if (lVar3 == 0) break;
    plVar1 = (longlong *)(lVar3 + 0x10);
    if (**(char **)(param_1 + 0x18) == 'E') {
      *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
      *(undefined8 *)(param_1 + 0x40) = uVar2;
      return local_res8;
    }
    lVar3 = FUN_140014be0(param_1);
  }
  return 0;
}


/* ================= FUN_140014d50 @ 140014d50  size=57  callers=3 callees=2 ================= */
/* CALLERS: FUN_14000f540, FUN_140010be0, FUN_140012310 */
/* CALLEES: FUN_140010130, FUN_140011c50 */

undefined8 FUN_140014d50(longlong param_1)

{
  int iVar1;
  undefined8 uVar2;
  
  if (**(char **)(param_1 + 0x18) == 'T') {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    iVar1 = FUN_140010130();
    if (-1 < iVar1) {
      uVar2 = FUN_140011c50(param_1,iVar1);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_140014d90 @ 140014d90  size=104  callers=1 callees=3 ================= */
/* CALLERS: FUN_140014e00 */
/* CALLEES: FUN_14000fcc0, FUN_140010130, FUN_140011b50 */

undefined4 * FUN_140014d90(longlong param_1)

{
  char *pcVar1;
  int iVar2;
  undefined4 *puVar3;
  
  pcVar1 = *(char **)(param_1 + 0x18);
  if ((*pcVar1 == 'U') && (*(char **)(param_1 + 0x18) = pcVar1 + 1, pcVar1[1] == 't')) {
    *(char **)(param_1 + 0x18) = pcVar1 + 2;
    iVar2 = FUN_140010130();
    if (-1 < iVar2) {
      puVar3 = (undefined4 *)FUN_140011b50(param_1);
      if (puVar3 != (undefined4 *)0x0) {
        *puVar3 = 0x48;
        puVar3[2] = iVar2;
      }
      iVar2 = FUN_14000fcc0(param_1,puVar3);
      if (iVar2 != 0) {
        return puVar3;
      }
    }
  }
  return (undefined4 *)0x0;
}


/* ================= FUN_140014e00 @ 140014e00  size=319  callers=3 callees=8 ================= */
/* CALLERS: FUN_140010be0, FUN_140011dd0, FUN_140012310 */
/* CALLEES: FUN_14000fc40, FUN_140010280, FUN_1400108e0, FUN_140011760, FUN_1400119b0, FUN_140012070, FUN_1400145b0, FUN_140014d90 */

int * FUN_140014e00(longlong param_1)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  int *piVar4;
  undefined8 uVar5;
  
  pcVar2 = *(char **)(param_1 + 0x18);
  cVar1 = *pcVar2;
  if ((byte)(cVar1 - 0x30U) < 10) {
    piVar4 = (int *)FUN_1400145b0(param_1);
  }
  else if ((byte)(cVar1 + 0x9fU) < 0x1a) {
    if ((cVar1 == 'o') && (pcVar2[1] == 'n')) {
      *(char **)(param_1 + 0x18) = pcVar2 + 2;
    }
    piVar4 = (int *)FUN_140012070(param_1);
    if ((piVar4 != (int *)0x0) && (*piVar4 == 0x32)) {
      *(int *)(param_1 + 0x48) =
           *(int *)(param_1 + 0x48) + *(int *)(*(longlong *)(piVar4 + 2) + 0x10) + 7;
      pcVar2 = (char *)**(undefined8 **)(piVar4 + 2);
      if ((*pcVar2 == 'l') && ((pcVar2[1] == 'i' && (pcVar2[2] == '\0')))) {
        uVar5 = FUN_1400145b0(param_1);
        piVar4 = (int *)FUN_1400119b0(param_1,0x37,piVar4,uVar5);
      }
    }
  }
  else if ((byte)(cVar1 + 0xbdU) < 2) {
    piVar4 = (int *)FUN_140010280(param_1);
  }
  else if (cVar1 == 'L') {
    *(char **)(param_1 + 0x18) = pcVar2 + 1;
    piVar4 = (int *)FUN_1400145b0(param_1);
    if (piVar4 == (int *)0x0) {
      return (int *)0x0;
    }
    iVar3 = FUN_1400108e0(param_1);
    if (iVar3 == 0) {
      return (int *)0x0;
    }
  }
  else {
    if (cVar1 != 'U') {
      return (int *)0x0;
    }
    if (pcVar2[1] == 'l') {
      piVar4 = (int *)FUN_140011760(param_1);
    }
    else {
      if (pcVar2[1] != 't') {
        return (int *)0x0;
      }
      piVar4 = (int *)FUN_140014d90(param_1);
    }
  }
  if (**(char **)(param_1 + 0x18) == 'B') {
    piVar4 = (int *)FUN_14000fc40(param_1,piVar4);
  }
  return piVar4;
}


/* ================= FUN_140014f40 @ 140014f40  size=115  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000f540 */
/* CALLEES: FUN_14000f540, FUN_140010bb0, FUN_1400119b0, FUN_140012040 */

undefined8 FUN_140014f40(longlong param_1)

{
  longlong lVar1;
  undefined8 uVar2;
  
  if (**(char **)(param_1 + 0x18) == '_') {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    lVar1 = FUN_140010bb0();
  }
  else {
    lVar1 = FUN_140012040();
  }
  if ((lVar1 != 0) && (**(char **)(param_1 + 0x18) == '_')) {
    *(char **)(param_1 + 0x18) = *(char **)(param_1 + 0x18) + 1;
    uVar2 = FUN_14000f540(param_1);
    uVar2 = FUN_1400119b0(param_1,0x2d,lVar1,uVar2);
    return uVar2;
  }
  return 0;
}


/* ================= FUN_140014fc0 @ 140014fc0  size=98  callers=1 callees=1 ================= */
/* CALLERS: FUN_140010960 */
/* CALLEES: FUN_140015090 */

bool FUN_140014fc0(undefined4 *param_1)

{
  int iVar1;
  
  while (param_1 != (undefined4 *)0x0) {
    switch(*param_1) {
    case 2:
      param_1 = *(undefined4 **)(param_1 + 4);
      break;
    default:
      goto switchD_140014ff3_caseD_3;
    case 4:
      iVar1 = FUN_140015090(*(undefined8 *)(param_1 + 2));
      return iVar1 == 0;
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
    case 0x20:
    case 0x4d:
    case 0x4f:
    case 0x50:
      param_1 = *(undefined4 **)(param_1 + 2);
    }
  }
switchD_140014ff3_caseD_3:
  return false;
}


/* ================= FUN_140015090 @ 140015090  size=49  callers=1 callees=0 ================= */
/* CALLERS: FUN_140014fc0 */

undefined8 FUN_140015090(int *param_1)

{
  int iVar1;
  
  while( true ) {
    if (param_1 == (int *)0x0) {
      return 0;
    }
    iVar1 = *param_1;
    if ((iVar1 != 1) && (iVar1 != 2)) break;
    param_1 = *(int **)(param_1 + 4);
  }
  if ((iVar1 != 7) && ((iVar1 != 8 && (iVar1 != 0x35)))) {
    return 0;
  }
  return 1;
}


/* ================= FUN_1400150d0 @ 1400150d0  size=47  callers=3 callees=0 ================= */
/* CALLERS: FUN_140010960, FUN_1400125b0, FUN_1400142a0 */

undefined8 FUN_1400150d0(undefined4 param_1)

{
  switch(param_1) {
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
  case 0x20:
  case 0x4d:
  case 0x4f:
  case 0x50:
    return 1;
  default:
    return 0;
  }
}


/* ================= FUN_140015140 @ 140015140  size=70  callers=2 callees=0 ================= */
/* CALLERS: FUN_14000f540, FUN_1400103a0 */

undefined8 FUN_140015140(longlong param_1)

{
  byte bVar1;
  uint uVar2;
  
  bVar1 = **(byte **)(param_1 + 0x18);
  uVar2 = bVar1 - 0x4b;
  if (((0x27 < (byte)uVar2) || ((0x8000000801U >> ((ulonglong)uVar2 & 0x3f) & 1) == 0)) &&
     ((bVar1 != 0x44 ||
      ((bVar1 = (*(byte **)(param_1 + 0x18))[1] + 0xb1, 0x29 < bVar1 ||
       ((0x30100000001U >> ((ulonglong)bVar1 & 0x3f) & 1) == 0)))))) {
    return 0;
  }
  return 1;
}


/* ================= FUN_140015190 @ 140015190  size=40  callers=2 callees=0 ================= */
/* CALLERS: FUN_140010be0, FUN_1400125b0 */

undefined8 FUN_140015190(longlong param_1)

{
  char cVar1;
  
  if ((((char *)**(undefined8 **)(param_1 + 8))[1] == 'c') &&
     ((cVar1 = *(char *)**(undefined8 **)(param_1 + 8), (byte)(cVar1 + 0x8eU) < 2 ||
      ((byte)(cVar1 + 0x9dU) < 2)))) {
    return 1;
  }
  return 0;
}


/* ================= FUN_1400151c0 @ 1400151c0  size=94  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000fd90 */
/* CALLEES: FUN_14000e420, __stdio_common_vsprintf */

int FUN_1400151c0(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  int iVar1;
  ulonglong *puVar2;
  undefined8 local_res18;
  undefined8 local_res20;
  
  local_res18 = param_3;
  local_res20 = param_4;
  puVar2 = (ulonglong *)FUN_14000e420();
  iVar1 = __stdio_common_vsprintf(*puVar2 | 1,param_1,0xffffffffffffffff,param_2,0,&local_res18);
  if (iVar1 < 0) {
    iVar1 = -1;
  }
  return iVar1;
}


/* ================= FUN_140015220 @ 140015220  size=219  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400041b0 */
/* CALLEES: FUN_14001ad70, FUN_14001ba90, FUN_14001bb80, FUN_14001bd50 */

undefined8 FUN_140015220(longlong param_1,undefined2 param_2,longlong *param_3,undefined8 param_4)

{
  longlong lVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  
  if (param_1 == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = *(undefined8 *)(param_1 + 0x48);
    if (param_3 != (longlong *)0x0) {
      lVar1 = FUN_14001ad70(param_1,param_2);
      if (lVar1 == 0) {
        uVar2 = FUN_14001ba90(param_2);
        FUN_14001bd50(uVar3,param_4,0x20,0,"dwarf_attr_create",0x2c,uVar2,
                      *(undefined8 *)(param_1 + 0x58),0);
        return 0xffffffff;
      }
      *param_3 = lVar1;
      return 0;
    }
  }
  FUN_14001bb80(uVar3,param_4,0x1c,0,"dwarf_attr_create",0x27);
  return 1;
}


/* ================= FUN_140015300 @ 140015300  size=101  callers=2 callees=2 ================= */
/* CALLERS: FUN_1400090c0, FUN_140015c80 */
/* CALLEES: FUN_14001b1a0, FUN_14001bb80 */

undefined8 FUN_140015300(longlong param_1,undefined8 param_2,uint *param_3,undefined8 param_4)

{
  longlong lVar1;
  undefined8 uVar2;
  
  uVar2 = 0;
  if ((param_1 != 0) && (uVar2 = *(undefined8 *)(param_1 + 0x48), param_3 != (uint *)0x0)) {
    lVar1 = FUN_14001b1a0(param_1);
    *param_3 = (uint)(lVar1 != 0);
    return 0;
  }
  FUN_14001bb80(uVar2,param_4,0x1c,0,"dwarf_hasattr",0x3c);
  return 1;
}


/* ================= FUN_140015370 @ 140015370  size=103  callers=3 callees=2 ================= */
/* CALLERS: FUN_140004ce0, FUN_140004f60, FUN_140015c80 */
/* CALLEES: FUN_140015520, FUN_1400158f0 */

void FUN_140015370(undefined8 param_1,undefined2 param_2,undefined8 *param_3,undefined8 param_4)

{
  int iVar1;
  undefined8 local_18 [2];
  
  iVar1 = FUN_1400158f0();
  if (iVar1 != 0) {
    iVar1 = FUN_140015520(param_1,param_2,local_18,param_4,1);
    if (iVar1 == 0) {
      *param_3 = local_18[0];
    }
  }
  return;
}


/* ================= FUN_1400153e0 @ 1400153e0  size=312  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400079a0 */
/* CALLEES: FUN_14001ad70, FUN_14001ba90, FUN_14001bac0, FUN_14001bb80, FUN_14001bd50, free */

undefined8 FUN_1400153e0(longlong param_1,undefined2 param_2,uint *param_3,undefined8 param_4)

{
  int iVar1;
  void *_Memory;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  
  if (param_1 == 0) {
    uVar4 = 0;
  }
  else {
    uVar4 = *(undefined8 *)(param_1 + 0x48);
    if (param_3 != (uint *)0x0) {
      *param_3 = 0;
      _Memory = (void *)FUN_14001ad70(param_1,param_2);
      if (_Memory == (void *)0x0) {
        return 0xffffffff;
      }
      iVar1 = *(int *)((longlong)_Memory + 0x18);
      if (((iVar1 != 0xb) && (iVar1 != 0xc)) && (iVar1 != 0x19)) {
        uVar2 = FUN_14001bac0();
        uVar3 = FUN_14001ba90(param_2);
        FUN_14001bd50(uVar4,param_4,6,0,"dwarf_attrval_flag",0x37,uVar3,
                      *(undefined8 *)(param_1 + 0x58),uVar2);
        free(_Memory);
        return 1;
      }
      *param_3 = (uint)(*(longlong *)((longlong)_Memory + 0x20) != 0);
      free(_Memory);
      return 0;
    }
  }
  FUN_14001bb80(uVar4,param_4,0x1c,0,"dwarf_attrval_flag",0x26);
  return 1;
}


/* ================= FUN_140015520 @ 140015520  size=461  callers=1 callees=6 ================= */
/* CALLERS: FUN_140015370 */
/* CALLEES: FUN_14001ad70, FUN_14001ba90, FUN_14001bac0, FUN_14001bb80, FUN_14001bd50, free */

undefined4
FUN_140015520(longlong param_1,undefined2 param_2,longlong *param_3,undefined8 param_4,char param_5)

{
  void *_Memory;
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  
  if (param_1 == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = *(undefined8 *)(param_1 + 0x48);
    if (param_3 != (longlong *)0x0) {
      *param_3 = 0;
      _Memory = (void *)FUN_14001ad70(param_1,param_2);
      if (_Memory != (void *)0x0) {
        switch(*(undefined4 *)((longlong)_Memory + 0x18)) {
        case 5:
          *param_3 = (longlong)*(short *)((longlong)_Memory + 0x20);
          free(_Memory);
          return 0;
        case 6:
        case 0x17:
          *param_3 = (longlong)*(int *)((longlong)_Memory + 0x20);
          free(_Memory);
          return 0;
        case 7:
        case 0xd:
        case 0x1e:
          *param_3 = *(longlong *)((longlong)_Memory + 0x20);
          free(_Memory);
          return 0;
        case 0xb:
          *param_3 = (longlong)*(char *)((longlong)_Memory + 0x20);
          free(_Memory);
          return 0;
        }
        if (param_5 == '\0') {
          uVar1 = FUN_14001bac0(*(undefined4 *)((longlong)_Memory + 0x18));
          uVar2 = FUN_14001ba90(param_2);
          FUN_14001bd50(uVar3,param_4,6,0,"dwarf_attrval_signed",0x90,uVar2,
                        *(undefined8 *)(param_1 + 0x58),uVar1);
        }
        free(_Memory);
        return 1;
      }
      uVar1 = FUN_14001ba90(param_2);
      FUN_14001bd50(uVar3,param_4,0x20,0,"dwarf_attrval_signed",0x78,uVar1,
                    *(undefined8 *)(param_1 + 0x58),0);
      return 0xffffffff;
    }
  }
  FUN_14001bb80(uVar3,param_4,0x1c,0,"dwarf_attrval_signed",0x71);
  return 1;
}


/* ================= FUN_140015720 @ 140015720  size=403  callers=2 callees=6 ================= */
/* CALLERS: FUN_140004d70, FUN_140006c00 */
/* CALLEES: FUN_14001ad70, FUN_14001ba90, FUN_14001bac0, FUN_14001bb80, FUN_14001bd50, free */

undefined4 FUN_140015720(longlong param_1,undefined2 param_2,undefined8 *param_3,undefined8 param_4)

{
  void *_Memory;
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  
  if (param_1 == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = *(undefined8 *)(param_1 + 0x48);
    if (param_3 != (undefined8 *)0x0) {
      *param_3 = 0;
      _Memory = (void *)FUN_14001ad70(param_1,param_2);
      if (_Memory != (void *)0x0) {
        switch(*(undefined4 *)((longlong)_Memory + 0x18)) {
        case 8:
          *param_3 = *(undefined8 *)((longlong)_Memory + 0x20);
          free(_Memory);
          return 0;
        default:
          uVar1 = FUN_14001bac0(*(undefined4 *)((longlong)_Memory + 0x18));
          uVar2 = FUN_14001ba90(param_2);
          FUN_14001bd50(uVar3,param_4,6,0,"dwarf_attrval_string",0x61,uVar2,
                        *(undefined8 *)(param_1 + 0x58),uVar1);
          free(_Memory);
          return 1;
        case 0xe:
        case 0x1a:
        case 0x1d:
        case 0x1f:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
          *param_3 = *(undefined8 *)((longlong)_Memory + 0x28);
          free(_Memory);
          return 0;
        }
      }
      uVar1 = FUN_14001ba90(param_2);
      FUN_14001bd50(uVar3,param_4,0x20,0,"dwarf_attrval_string",0x4e,uVar1,
                    *(undefined8 *)(param_1 + 0x58),0);
      return 0xffffffff;
    }
  }
  FUN_14001bb80(uVar3,param_4,0x1c,0,"dwarf_attrval_string",0x47);
  return 1;
}


/* ================= FUN_1400158f0 @ 1400158f0  size=720  callers=1 callees=8 ================= */
/* CALLERS: FUN_140015370 */
/* CALLEES: FUN_140015fc0, FUN_14001ad70, FUN_14001b330, FUN_14001ba90, FUN_14001bac0, FUN_14001bb80, FUN_14001bd50, free */

undefined8
FUN_1400158f0(longlong param_1,short param_2,undefined8 *param_3,undefined8 param_4,char param_5)

{
  int iVar1;
  void *_Memory;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  longlong lVar5;
  undefined8 uVar6;
  undefined4 uVar7;
  
  if (param_1 == 0) {
    uVar6 = 0;
LAB_140015b90:
    FUN_14001bb80(uVar6,param_4,0x1c,0,"dwarf_attrval_unsigned",0xa4);
    return 1;
  }
  uVar6 = *(undefined8 *)(param_1 + 0x48);
  if (param_3 == (undefined8 *)0x0) goto LAB_140015b90;
  lVar5 = 0;
  *param_3 = 0;
  _Memory = (void *)FUN_14001ad70(param_1,param_2);
  if (_Memory != (void *)0x0) {
LAB_140015ab5:
    switch(*(undefined4 *)((longlong)_Memory + 0x18)) {
    case 1:
    case 5:
    case 6:
    case 7:
    case 0xb:
    case 0xf:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x17:
    case 0x1c:
    case 0x1e:
    case 0x24:
      uVar2 = *(undefined8 *)((longlong)_Memory + 0x20);
      break;
    default:
      if (lVar5 != 0) {
        FUN_140015fc0(uVar6,lVar5,8);
      }
      if (param_5 != '\0') goto LAB_140015b79;
      uVar3 = FUN_14001bac0(*(undefined4 *)((longlong)_Memory + 0x18));
      uVar4 = FUN_14001ba90(param_2);
      uVar2 = *(undefined8 *)(param_1 + 0x58);
      uVar7 = 0xf7;
      goto LAB_140015b5b;
    case 0x1b:
    case 0x22:
    case 0x23:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
      uVar2 = *(undefined8 *)((longlong)_Memory + 0x28);
    }
    *param_3 = uVar2;
    if (lVar5 != 0) {
      FUN_140015fc0(uVar6,lVar5,8);
    }
    free(_Memory);
    return 0;
  }
  if (param_2 != 0x49) {
    uVar2 = FUN_14001ba90(param_2);
    FUN_14001bd50(uVar6,param_4,0x20,0,"dwarf_attrval_unsigned",0xac,uVar2,
                  *(undefined8 *)(param_1 + 0x58),0);
    return 0xffffffff;
  }
  _Memory = (void *)FUN_14001ad70(param_1,0x31);
  if (_Memory == (void *)0x0) {
    return 1;
  }
  iVar1 = *(int *)((longlong)_Memory + 0x18);
  if ((((iVar1 == 0x11) || (iVar1 == 0x12)) || (iVar1 == 0x13)) ||
     ((iVar1 == 0x14 || (iVar1 == 0x15)))) {
    uVar2 = *(undefined8 *)((longlong)_Memory + 0x20);
    free(_Memory);
    lVar5 = FUN_14001b330(param_1,uVar2);
    if (lVar5 == 0) {
      FUN_14001bb80(uVar6,param_4,4,0,"dwarf_attrval_unsigned",0xc5);
      free((void *)0x0);
      return 0xffffffff;
    }
    _Memory = (void *)FUN_14001ad70(lVar5,0x49);
    if (_Memory == (void *)0x0) {
      FUN_140015fc0(uVar6,lVar5,8);
      free((void *)0x0);
      return 0xffffffff;
    }
    goto LAB_140015ab5;
  }
  if (param_5 != '\0') goto LAB_140015b79;
  uVar3 = FUN_14001bac0();
  uVar4 = FUN_14001ba90(0x49);
  uVar2 = *(undefined8 *)(param_1 + 0x58);
  uVar7 = 0xcd;
LAB_140015b5b:
  FUN_14001bd50(uVar6,param_4,6,0,"dwarf_attrval_unsigned",uVar7,uVar4,uVar2,uVar3);
LAB_140015b79:
  free(_Memory);
  return 1;
}


/* ================= FUN_140015c00 @ 140015c00  size=32  callers=4 callees=1 ================= */
/* CALLERS: FUN_1400250c0, FUN_140025ad0, FUN_140025b90, FUN_140025c90 */
/* CALLEES: FUN_140015c80 */

undefined8 FUN_140015c00(longlong param_1,longlong param_2,undefined8 param_3)

{
  undefined8 uVar1;
  
  if ((param_2 != 0) && (param_1 != 0)) {
    uVar1 = FUN_140015c80(param_1,0x73,param_2,param_3);
    return uVar1;
  }
  return 0x1c;
}


/* ================= FUN_140015c20 @ 140015c20  size=32  callers=1 callees=1 ================= */
/* CALLERS: FUN_140025ef0 */
/* CALLEES: FUN_140015c80 */

undefined8 FUN_140015c20(longlong param_1,longlong param_2,undefined8 param_3)

{
  undefined8 uVar1;
  
  if ((param_2 != 0) && (param_1 != 0)) {
    uVar1 = FUN_140015c80(param_1,0x8c,param_2,param_3);
    return uVar1;
  }
  return 0x1c;
}


/* ================= FUN_140015c40 @ 140015c40  size=32  callers=1 callees=1 ================= */
/* CALLERS: FUN_140026150 */
/* CALLEES: FUN_140015c80 */

undefined8 FUN_140015c40(longlong param_1,longlong param_2,undefined8 param_3)

{
  undefined8 uVar1;
  
  if ((param_2 != 0) && (param_1 != 0)) {
    uVar1 = FUN_140015c80(param_1,0x74,param_2,param_3);
    return uVar1;
  }
  return 0x1c;
}


/* ================= FUN_140015c60 @ 140015c60  size=32  callers=1 callees=1 ================= */
/* CALLERS: FUN_140026610 */
/* CALLEES: FUN_140015c80 */

undefined8 FUN_140015c60(longlong param_1,longlong param_2,undefined8 param_3)

{
  undefined8 uVar1;
  
  if ((param_2 != 0) && (param_1 != 0)) {
    uVar1 = FUN_140015c80(param_1,0x72,param_2,param_3);
    return uVar1;
  }
  return 0x1c;
}


/* ================= FUN_140015c80 @ 140015c80  size=200  callers=5 callees=4 ================= */
/* CALLERS: FUN_140015c00, FUN_140015c20, FUN_140015c40, FUN_140015c60, FUN_140015d50 */
/* CALLEES: FUN_140015300, FUN_140015370, FUN_14001b3f0, free */

undefined4
FUN_140015c80(longlong *param_1,undefined2 param_2,undefined8 *param_3,undefined8 param_4)

{
  int iVar1;
  void *_Memory;
  undefined4 uVar2;
  int local_res18 [4];
  undefined8 local_28 [2];
  
  if ((param_3 != (undefined8 *)0x0) && (param_1 != (longlong *)0x0)) {
    uVar2 = 0x1b;
    if (*param_1 != 0) {
      _Memory = (void *)FUN_14001b3f0(*param_1,param_1);
      if (_Memory != (void *)0x0) {
        iVar1 = FUN_140015300(_Memory,param_2,local_res18,param_4);
        if (iVar1 == 0) {
          if (local_res18[0] == 0) {
            uVar2 = 0x20;
          }
          else {
            iVar1 = FUN_140015370(_Memory,param_2,local_28,param_4);
            if (iVar1 == 0) {
              uVar2 = 0;
              *param_3 = local_28[0];
            }
          }
        }
        free(_Memory);
      }
      return uVar2;
    }
    return 0x1b;
  }
  return 0x1c;
}


/* ================= FUN_140015d50 @ 140015d50  size=32  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400233d0 */
/* CALLEES: FUN_140015c80 */

undefined8 FUN_140015d50(longlong param_1,longlong param_2,undefined8 param_3)

{
  undefined8 uVar1;
  
  if ((param_2 != 0) && (param_1 != 0)) {
    uVar1 = FUN_140015c80(param_1,0x11,param_2,param_3);
    return uVar1;
  }
  return 0x1c;
}


/* ================= FUN_140015d70 @ 140015d70  size=65  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000d8c0 */
/* CALLEES: FUN_140015dc0 */

void FUN_140015d70(void)

{
  FUN_140015dc0();
  return;
}


/* ================= FUN_140015dc0 @ 140015dc0  size=102  callers=1 callees=1 ================= */
/* CALLERS: FUN_140015d70 */
/* CALLEES: FUN_140015e30 */

void FUN_140015dc0(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4,
                  undefined8 param_5,undefined8 param_6,undefined8 param_7,undefined8 param_8,
                  undefined8 param_9)

{
  FUN_140015e30(param_1,1,param_2,param_3,param_4,param_5,param_6,param_7,0,0,param_8,param_9);
  return;
}


/* ================= FUN_140015e30 @ 140015e30  size=394  callers=3 callees=5 ================= */
/* CALLERS: FUN_14000d930, FUN_14000d9a0, FUN_140015dc0 */
/* CALLEES: FUN_14001bb80, FUN_14001e3c0, FUN_14001e460, FUN_14001e8e0, FUN_14001e980 */

undefined8
FUN_140015e30(longlong param_1,int param_2,ulonglong *param_3,undefined2 *param_4,
             undefined4 *param_5,ushort *param_6,undefined2 *param_7,undefined2 *param_8,
             undefined8 *param_9,undefined8 *param_10,longlong *param_11,undefined8 param_12)

{
  int iVar1;
  undefined8 uVar2;
  longlong lVar3;
  
  if (param_1 == 0) {
    FUN_14001bb80(0,param_12,0x1c,0,"dwarf_next_cu_header_c",0x29);
LAB_140015e7d:
    uVar2 = 1;
  }
  else {
    if (param_2 == 0) {
      if (*(longlong *)(param_1 + 0x148) == 0) {
        iVar1 = FUN_14001e460();
      }
      else {
        iVar1 = FUN_14001e980(param_1,param_12);
      }
    }
    else if (*(longlong *)(param_1 + 0x140) == 0) {
      iVar1 = FUN_14001e3c0();
    }
    else {
      iVar1 = FUN_14001e8e0();
    }
    if (iVar1 != 0x23) {
      if (iVar1 != 0) goto LAB_140015e7d;
      if (param_2 == 0) {
        lVar3 = *(longlong *)(param_1 + 0x148);
      }
      else {
        lVar3 = *(longlong *)(param_1 + 0x140);
      }
      if (lVar3 != 0) {
        if (param_3 != (ulonglong *)0x0) {
          *param_3 = (ulonglong)*(uint *)(lVar3 + 0xc);
        }
        if (param_4 != (undefined2 *)0x0) {
          *param_4 = *(undefined2 *)(lVar3 + 0x12);
        }
        if (param_5 != (undefined4 *)0x0) {
          *param_5 = *(undefined4 *)(lVar3 + 0x20);
        }
        if (param_6 != (ushort *)0x0) {
          *param_6 = (ushort)*(byte *)(lVar3 + 0x48);
        }
        if (param_7 != (undefined2 *)0x0) {
          if (*(short *)(lVar3 + 0x10) == 4) {
            *param_7 = 4;
          }
          else {
            *param_7 = 8;
          }
        }
        if (param_8 != (undefined2 *)0x0) {
          if (*(short *)(lVar3 + 0x10) == 4) {
            *param_8 = 0;
          }
          else {
            *param_8 = 4;
          }
        }
        if (param_11 != (longlong *)0x0) {
          *param_11 = (longlong)*(int *)(lVar3 + 0x60);
        }
        if (param_2 == 0) {
          if (param_9 != (undefined8 *)0x0) {
            *param_9 = *(undefined8 *)(lVar3 + 0x4a);
          }
          if (param_10 != (undefined8 *)0x0) {
            *param_10 = *(undefined8 *)(lVar3 + 0x58);
          }
        }
        return 0;
      }
    }
    uVar2 = 0xffffffff;
  }
  return uVar2;
}


/* ================= FUN_140015fc0 @ 140015fc0  size=364  callers=4 callees=1 ================= */
/* CALLERS: FUN_1400045e0, FUN_140004600, FUN_1400158f0, FUN_14001b4a0 */
/* CALLEES: free */

void FUN_140015fc0(undefined8 param_1,void *param_2,ulonglong param_3)

{
  longlong *plVar1;
  void *pvVar2;
  void *pvVar3;
  longlong lVar4;
  void *pvVar5;
  void *pvVar6;
  
  if ((0x15 < param_3) || ((0x341000UL >> (param_3 & 0x3f) & 1) == 0)) {
    if (param_3 == 0) {
      plVar1 = (longlong *)((longlong)param_2 + 0x30);
      pvVar5 = *(void **)((longlong)param_2 + 0x30);
      while (pvVar5 != (void *)0x0) {
        pvVar2 = (void *)*plVar1;
        pvVar3 = *(void **)((longlong)pvVar5 + 0x18);
        if (pvVar2 == pvVar5) {
          lVar4 = *(longlong *)((longlong)pvVar2 + 0x18);
          *plVar1 = lVar4;
          if (lVar4 == 0) {
            *(longlong **)((longlong)param_2 + 0x38) = plVar1;
          }
        }
        else {
          for (pvVar6 = *(void **)((longlong)pvVar2 + 0x18); pvVar6 != pvVar5;
              pvVar6 = *(void **)((longlong)pvVar6 + 0x18)) {
            pvVar2 = pvVar6;
          }
          lVar4 = *(longlong *)(*(longlong *)((longlong)pvVar2 + 0x18) + 0x18);
          *(longlong *)((longlong)pvVar2 + 0x18) = lVar4;
          if (lVar4 == 0) {
            *(longlong **)((longlong)param_2 + 0x38) = (longlong *)((longlong)pvVar2 + 0x18);
          }
        }
        free(pvVar5);
        pvVar5 = pvVar3;
      }
    }
    else if (param_3 != 3) {
      if (param_3 != 8) {
        return;
      }
      plVar1 = (longlong *)((longlong)param_2 + 0x70);
      pvVar5 = *(void **)((longlong)param_2 + 0x70);
      while (pvVar5 != (void *)0x0) {
        pvVar2 = (void *)*plVar1;
        pvVar3 = *(void **)((longlong)pvVar5 + 0x18);
        if (pvVar2 == pvVar5) {
          lVar4 = *(longlong *)((longlong)pvVar2 + 0x18);
          *plVar1 = lVar4;
          if (lVar4 == 0) {
            *(longlong **)((longlong)param_2 + 0x78) = plVar1;
          }
        }
        else {
          for (pvVar6 = *(void **)((longlong)pvVar2 + 0x18); pvVar6 != pvVar5;
              pvVar6 = *(void **)((longlong)pvVar6 + 0x18)) {
            pvVar2 = pvVar6;
          }
          lVar4 = *(longlong *)(*(longlong *)((longlong)pvVar2 + 0x18) + 0x18);
          *(longlong *)((longlong)pvVar2 + 0x18) = lVar4;
          if (lVar4 == 0) {
            *(longlong **)((longlong)param_2 + 0x78) = (longlong *)((longlong)pvVar2 + 0x18);
          }
        }
        free(pvVar5);
        pvVar5 = pvVar3;
      }
      if (*(void **)((longlong)param_2 + 0x68) != (void *)0x0) {
        free(*(void **)((longlong)param_2 + 0x68));
      }
      if (*(void **)((longlong)param_2 + 0x60) != (void *)0x0) {
        free(*(void **)((longlong)param_2 + 0x60));
      }
    }
  }
  free(param_2);
  return;
}


/* ================= FUN_140016130 @ 140016130  size=165  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400163c0 */
/* CALLEES: FUN_14001b4a0, FUN_14001bb80 */

undefined8
FUN_140016130(longlong param_1,longlong param_2,int param_3,longlong param_4,undefined8 param_5)

{
  undefined8 uVar1;
  
  if (((param_1 != 0) && (param_4 != 0)) && (param_2 != 0)) {
    if (*(int *)(param_2 + 0x88) == 0) {
      uVar1 = *(undefined8 *)(param_1 + 0x20);
    }
    else {
      uVar1 = *(undefined8 *)(param_1 + 0x10);
    }
    uVar1 = FUN_14001b4a0(param_1,uVar1,param_2,*(undefined1 *)(param_2 + 0x49),(longlong)param_3,
                          (longlong)*(int *)(param_2 + 0x60),param_4,0,param_5);
    return uVar1;
  }
  FUN_14001bb80(param_1,param_5,0x1c,0,"_dwarf_search_die_within_cu",0xb6);
  return 1;
}


/* ================= FUN_1400161e0 @ 1400161e0  size=35  callers=2 callees=1 ================= */
/* CALLERS: FUN_140003fc0, FUN_1400080f0 */
/* CALLEES: FUN_140016210 */

void FUN_1400161e0(longlong param_1,undefined8 param_2,undefined8 param_3)

{
  undefined4 uVar1;
  
  uVar1 = 1;
  if ((param_1 != 0) && (*(longlong *)(param_1 + 0x50) != 0)) {
    uVar1 = *(undefined4 *)(*(longlong *)(param_1 + 0x50) + 0x88);
  }
  FUN_140016210(param_1,param_2,uVar1,param_3);
  return;
}


/* ================= FUN_140016210 @ 140016210  size=345  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400161e0 */
/* CALLEES: FUN_14001b4a0, FUN_14001bb80 */

ulonglong FUN_140016210(longlong param_1,longlong param_2,int param_3,undefined8 param_4)

{
  longlong lVar1;
  int iVar2;
  undefined8 uVar3;
  longlong lVar4;
  
  if (param_1 == 0) {
    lVar4 = 0;
  }
  else {
    lVar4 = *(longlong *)(param_1 + 0x48);
    if (param_2 != 0) {
      if (*(longlong *)(param_1 + 0x38) == 0) {
        uVar3 = 0x2d;
      }
      else {
        if (*(char *)(*(longlong *)(param_1 + 0x38) + 0x10) == '\0') {
          return 0x23;
        }
        if ((lVar4 != 0) && (lVar1 = *(longlong *)(param_1 + 0x50), lVar1 != 0)) {
          if (param_3 == 0) {
            uVar3 = *(undefined8 *)(lVar4 + 0x20);
          }
          else {
            uVar3 = *(undefined8 *)(lVar4 + 0x10);
          }
          iVar2 = FUN_14001b4a0(lVar4,uVar3,lVar1,*(undefined1 *)(lVar1 + 0x49),
                                *(undefined8 *)(param_1 + 0x28),(longlong)*(int *)(lVar1 + 0x60),
                                param_2,0,param_4);
          if (iVar2 == 0x23) {
            FUN_14001bb80(lVar4,param_4,4,0,"dwarf_child_b",0x44);
          }
          else if (iVar2 != -1) {
            return (ulonglong)(iVar2 != 0);
          }
          return 0xffffffff;
        }
        uVar3 = 0x38;
      }
      goto LAB_140016336;
    }
  }
  uVar3 = 0x2a;
LAB_140016336:
  FUN_14001bb80(lVar4,param_4,0x1c,0,"dwarf_child_b",uVar3);
  return 1;
}


/* ================= FUN_140016370 @ 140016370  size=79  callers=1 callees=1 ================= */
/* CALLERS: FUN_140006e30 */
/* CALLEES: FUN_14001bb80 */

undefined8 FUN_140016370(longlong param_1,undefined2 *param_2,undefined8 param_3)

{
  if ((param_1 != 0) && (param_2 != (undefined2 *)0x0)) {
    *param_2 = *(undefined2 *)(param_1 + 0xf4);
    return 0;
  }
  FUN_14001bb80(param_1,param_3,0x1c,0,"dwarf_get_address_size",0x1a7);
  return 1;
}


/* ================= FUN_1400163c0 @ 1400163c0  size=576  callers=2 callees=5 ================= */
/* CALLERS: FUN_140004070, FUN_140016600 */
/* CALLEES: FUN_140016130, FUN_14001bb20, FUN_14001bb80, FUN_14001bd50, FUN_14001e500 */

undefined8
FUN_1400163c0(longlong param_1,int param_2,undefined8 param_3,longlong param_4,undefined8 param_5)

{
  int iVar1;
  undefined8 uVar2;
  longlong lVar3;
  undefined4 uVar4;
  
  if ((param_1 == 0) || (param_4 == 0)) {
    FUN_14001bb80(param_1,param_5,0x1c,0,"dwarf_offdie_b",200);
    return 1;
  }
  if ((int)param_3 == 0) {
    lVar3 = *(longlong *)(param_1 + 0x148);
  }
  else {
    lVar3 = *(longlong *)(param_1 + 0x140);
  }
  if (((lVar3 == 0) || (param_2 <= *(int *)(lVar3 + 8))) || (*(int *)(lVar3 + 0x60) <= param_2)) {
    iVar1 = FUN_14001e500(param_1,1,param_3,param_5);
    if (iVar1 != 0) {
      return 1;
    }
    if ((int)param_3 == 0) {
      for (lVar3 = *(longlong *)(param_1 + 0x130); lVar3 != 0; lVar3 = *(longlong *)(lVar3 + 0x90))
      {
        if ((*(int *)(lVar3 + 8) <= param_2) && (param_2 <= *(int *)(lVar3 + 0x60))) {
          iVar1 = FUN_140016130(param_1,lVar3,param_2,param_4,param_5);
          if (iVar1 != 0x23) goto joined_r0x0001400165bd;
          uVar2 = FUN_14001bb20();
          uVar4 = 0xf7;
          goto LAB_1400164ca;
        }
      }
    }
    else {
      for (lVar3 = *(longlong *)(param_1 + 0x120); lVar3 != 0; lVar3 = *(longlong *)(lVar3 + 0x90))
      {
        if ((*(int *)(lVar3 + 8) <= param_2) && (param_2 <= *(int *)(lVar3 + 0x60))) {
          iVar1 = FUN_140016130(param_1,lVar3,param_2,param_4,param_5);
          if (iVar1 != 0x23) goto joined_r0x0001400165bd;
          uVar2 = FUN_14001bb20();
          uVar4 = 0xe8;
          goto LAB_1400164ca;
        }
      }
    }
    uVar2 = FUN_14001bb20();
    uVar4 = 0x102;
  }
  else {
    iVar1 = FUN_140016130(param_1,lVar3,param_2,param_4,param_5);
    if (iVar1 != 0x23) {
joined_r0x0001400165bd:
      if (iVar1 == -1) {
        return 0xffffffff;
      }
      if (iVar1 != 0) {
        return 1;
      }
      return 0;
    }
    uVar2 = FUN_14001bb20();
    uVar4 = 0xd2;
  }
LAB_1400164ca:
  FUN_14001bd50(param_1,param_5,0x21,0,"dwarf_offdie_b",uVar4,uVar2,0,0);
  return 0xffffffff;
}


/* ================= FUN_140016600 @ 140016600  size=563  callers=3 callees=5 ================= */
/* CALLERS: FUN_140004070, FUN_1400044f0, FUN_1400080f0 */
/* CALLEES: FUN_1400163c0, FUN_14001ad70, FUN_14001b4a0, FUN_14001bb80, free */

ulonglong FUN_140016600(longlong param_1,longlong param_2,longlong param_3,int param_4,
                       undefined8 param_5)

{
  int iVar1;
  ulonglong uVar2;
  void *_Memory;
  undefined8 uVar3;
  longlong lVar4;
  longlong lVar5;
  undefined4 uVar6;
  
  if ((param_1 != 0) && (param_3 != 0)) {
    if (param_4 == 0) {
      lVar5 = *(longlong *)(param_1 + 0x148);
    }
    else {
      lVar5 = *(longlong *)(param_1 + 0x140);
    }
    if (lVar5 == 0) {
      FUN_14001bb80(param_1,param_5,0x24,0,"dwarf_siblingof_b",0x67);
      uVar2 = 1;
    }
    else if (param_2 == 0) {
      uVar2 = FUN_1400163c0(param_1,*(undefined4 *)(lVar5 + 0x68),param_4,param_3,param_5);
    }
    else if (param_4 == *(int *)(*(longlong *)(param_2 + 0x50) + 0x88)) {
      uVar6 = 0;
      if (*(char *)(*(longlong *)(param_2 + 0x38) + 0x10) == '\0') {
        lVar4 = *(longlong *)(param_2 + 0x28);
      }
      else {
        _Memory = (void *)FUN_14001ad70(param_2,1);
        if (_Memory == (void *)0x0) {
          lVar4 = *(longlong *)(param_2 + 0x28);
          uVar6 = 1;
        }
        else if (*(int *)((longlong)_Memory + 0x18) == 0x10) {
          lVar4 = *(longlong *)((longlong)_Memory + 0x20);
          free(_Memory);
        }
        else {
          lVar4 = (longlong)*(int *)(lVar5 + 8) + *(longlong *)((longlong)_Memory + 0x20);
          free(_Memory);
        }
      }
      if (param_4 == 0) {
        uVar3 = *(undefined8 *)(param_1 + 0x20);
      }
      else {
        uVar3 = *(undefined8 *)(param_1 + 0x10);
      }
      iVar1 = FUN_14001b4a0(*(undefined8 *)(param_2 + 0x48),uVar3,lVar5,
                            *(undefined1 *)(lVar5 + 0x49),lVar4,(longlong)*(int *)(lVar5 + 0x60),
                            param_3,uVar6,param_5);
      if (iVar1 == 0x23) {
        FUN_14001bb80(param_1,param_5,4,0,"dwarf_siblingof_b",0x9c);
      }
      else if (iVar1 != -1) {
        return (ulonglong)(iVar1 != 0);
      }
      uVar2 = 0xffffffff;
    }
    else {
      FUN_14001bb80(param_1,param_5,0x1c,0,"dwarf_siblingof_b",0x75);
      uVar2 = 1;
    }
    return uVar2;
  }
  FUN_14001bb80(param_1,param_5,0x1c,0,"dwarf_siblingof_b",0x61);
  return 1;
}

