
/* ================= FUN_14003dc80 @ 14003dc80  size=38  callers=1 callees=0 ================= */
/* CALLERS: FUN_140041cd0 */

undefined8 FUN_14003dc80(undefined8 param_1,undefined8 param_2,int param_3)

{
  undefined8 uVar1;
  
  if (param_3 != 1) {
    if (param_3 == 2) {
      DAT_14285da1e = 1;
    }
    return 0;
  }
  uVar1 = FUN_14004ea70();
  return uVar1;
}


/* ================= FUN_14003dcb0 @ 14003dcb0  size=210  callers=0 callees=8 ================= */
/* CALLEES: FUN_140027140, FUN_140027290, FUN_1400272c0, FUN_14003f110, FUN_14003f430, FUN_14004e9c0, FUN_14004ec40, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14003dcb0(undefined8 param_1)

{
  int iVar1;
  undefined8 uVar2;
  undefined1 auStack_608 [32];
  undefined4 local_5e8 [4];
  undefined1 local_5d8 [1296];
  undefined1 local_c8 [132];
  undefined4 local_44;
  undefined4 local_2c;
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_608;
  if (DAT_14285da1e == '\0') {
    FUN_14003f430("-USB should only be used in combination with -openprj.");
    uVar2 = 0xffffffff;
  }
  else {
    FUN_14004e9c0(local_5d8);
    iVar1 = FUN_140027290(param_1);
    if (iVar1 == 0) {
      FUN_140027140(local_c8,param_1,0x80);
      local_44 = 0;
    }
    else {
      iVar1 = FUN_1400272c0(param_1,local_5e8);
      if (iVar1 < 0) {
        FUN_14003f110("Error while parsing USB S/N");
        return 0xffffffff;
      }
      local_44 = local_5e8[0];
    }
    local_2c = 1;
    FUN_14004ec40(local_5d8);
    uVar2 = 0;
  }
  return uVar2;
}


/* ================= FUN_14003dd90 @ 14003dd90  size=138  callers=0 callees=5 ================= */
/* CALLEES: FUN_140027140, FUN_14003f430, FUN_14004e9c0, FUN_14004ec40, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14003dd90(undefined8 param_1)

{
  undefined8 uVar1;
  undefined1 auStack_5f8 [32];
  undefined1 local_5d8 [1168];
  undefined1 local_148 [284];
  undefined4 local_2c;
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_5f8;
  if (DAT_14285da1e == '\0') {
    FUN_14003f430("-IP should only be used in combination with -openprj.");
    uVar1 = 0xffffffff;
  }
  else {
    FUN_14004e9c0(local_5d8);
    FUN_140027140(local_148,param_1,0x80);
    local_2c = 2;
    FUN_14004ec40(local_5d8);
    uVar1 = 0;
  }
  return uVar1;
}


/* ================= FUN_14003de20 @ 14003de20  size=54  callers=0 callees=2 ================= */
/* CALLEES: FUN_1400276b0, FUN_140028380 */

undefined8 FUN_14003de20(undefined8 param_1)

{
  FUN_1400276b0(param_1,0x2c,0x20);
  FUN_140028380(&DAT_14285d210,0x200,"SetRTTSearchRanges %s",param_1);
  return 0;
}


/* ================= FUN_14003de60 @ 14003de60  size=161  callers=1 callees=5 ================= */
/* CALLERS: FUN_14004ded0 */
/* CALLEES: FUN_14003e050, FUN_14003fa70, FUN_140040980, FUN_140042a60, memset */

void FUN_14003de60(undefined4 param_1,undefined8 param_2)

{
  memset(&DAT_14085d1f0,0,0x2000000);
  FUN_14003e050();
  FUN_140040980();
  if ((DAT_14285d9c4 != '\0') && (DAT_14285d9c5 == '\0')) {
    DAT_14285d9c5 = 1;
    FUN_140042a60();
    DAT_140096dd0 = 0;
    FUN_14003fa70(param_1,param_2);
    DAT_140096dd0 = 1;
    DAT_14285d9c5 = 0;
    FUN_140042a60();
    return;
  }
  FUN_140042a60();
  DAT_140096dd0 = 0;
  FUN_14003fa70(param_1,param_2);
  DAT_140096dd0 = 1;
  return;
}


/* ================= FUN_14003df10 @ 14003df10  size=139  callers=3 callees=1 ================= */
/* CALLERS: FUN_14003dfa0, FUN_140045d50, FUN_140051590 */
/* CALLEES: FUN_140043d10 */

void FUN_14003df10(undefined8 param_1)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  longlong lVar3;
  undefined8 *puVar4;
  
  if (DAT_14286d3b8 == (undefined8 *)0x0) {
    DAT_14286d3b8 = (undefined8 *)FUN_140043d10(0x10);
    DAT_14286d3b8[1] = 0;
    puVar4 = DAT_14286d3b8;
  }
  else {
    puVar2 = (undefined8 *)DAT_14286d3b8[1];
    puVar4 = DAT_14286d3b8;
    while (puVar1 = puVar2, puVar1 != (undefined8 *)0x0) {
      puVar4 = puVar1;
      puVar2 = (undefined8 *)puVar1[1];
    }
    lVar3 = FUN_140043d10(0x10);
    puVar4[1] = lVar3;
    *(undefined8 *)(lVar3 + 8) = 0;
    puVar4 = (undefined8 *)puVar4[1];
  }
  DAT_14085d1e8 = DAT_14085d1e8 + 1;
  *puVar4 = param_1;
  DAT_14285db70 = 1;
  DAT_14285d414 = 1;
  return;
}


/* ================= FUN_14003dfa0 @ 14003dfa0  size=172  callers=1 callees=6 ================= */
/* CALLERS: FUN_14003fc90 */
/* CALLEES: FUN_140027140, FUN_1400277b0, FUN_14003df10, FUN_140040ab0, FUN_140043d10, memset */

void FUN_14003dfa0(undefined4 param_1,undefined4 param_2,undefined1 param_3,undefined8 param_4)

{
  int iVar1;
  longlong lVar2;
  void *_Dst;
  uint uVar3;
  
  iVar1 = FUN_1400277b0(param_4);
  uVar3 = iVar1 + 1;
  lVar2 = FUN_140040ab0(0);
  FUN_140027140(lVar2 + 4,param_4,0x100);
  _Dst = (void *)FUN_140043d10(uVar3);
  *(void **)(lVar2 + 0x108) = _Dst;
  memset(_Dst,0,(ulonglong)uVar3);
  FUN_140027140(*(undefined8 *)(lVar2 + 0x108),param_4,uVar3);
  *(undefined4 *)(lVar2 + 0x31c) = param_1;
  *(undefined4 *)(lVar2 + 0x314) = param_2;
  *(undefined1 *)(lVar2 + 0x330) = param_3;
  FUN_14003df10(lVar2);
  return;
}


/* ================= FUN_14003e050 @ 14003e050  size=89  callers=2 callees=2 ================= */
/* CALLERS: FUN_14003d5c0, FUN_14003de60 */
/* CALLEES: FUN_140043d10, memset */

void FUN_14003e050(void)

{
  longlong *plVar1;
  undefined8 uVar2;
  longlong lVar3;
  
  for (plVar1 = DAT_14286d3b8; plVar1 != (longlong *)0x0; plVar1 = (longlong *)plVar1[1]) {
    lVar3 = *plVar1;
    if (*(longlong *)(lVar3 + 0x3d0) == 0) {
      uVar2 = FUN_140043d10(0x4000000);
      *(undefined8 *)(*plVar1 + 0x3d0) = uVar2;
      lVar3 = *plVar1;
    }
    memset(*(void **)(lVar3 + 0x3d0),0,0x4000000);
  }
  return;
}


/* ================= FUN_14003e0b0 @ 14003e0b0  size=84  callers=1 callees=3 ================= */
/* CALLERS: FUN_14004d7e0 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e0b0(void)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x28,"APP: <CheckELFModifications> Job");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  *(undefined4 *)(puVar1 + 3) = 0xc;
  *puVar1 = &LAB_140042ee0;
  puVar1[1] = puVar1;
  puVar1[2] = 0;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e110 @ 14003e110  size=147  callers=1 callees=4 ================= */
/* CALLERS: FUN_14002ba50 */
/* CALLEES: FUN_140027140, FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e110(longlong param_1)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x128,"APP: <CreateProject> Job");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  if (param_1 == 0) {
    *(undefined1 *)(puVar1 + 4) = 0;
  }
  else {
    FUN_140027140(puVar1 + 4,param_1,0x104);
  }
  *(undefined4 *)(puVar1 + 3) = 2;
  puVar1[1] = puVar1;
  puVar1[2] = DAT_14286d388;
  *puVar1 = &LAB_140042ee0;
  *(undefined4 *)((longlong)puVar1 + 0x1c) = 0;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e1b0 @ 14003e1b0  size=84  callers=1 callees=3 ================= */
/* CALLERS: FUN_14004db40 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e1b0(void)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x28,"APP: <Exit>");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  *(undefined4 *)(puVar1 + 3) = 1;
  *puVar1 = &LAB_140042eb0;
  puVar1[1] = puVar1;
  puVar1[2] = 0;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e210 @ 14003e210  size=194  callers=1 callees=3 ================= */
/* CALLERS: FUN_14004cf60 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e210(undefined8 *param_1)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined8 uVar6;
  undefined8 *puVar7;
  undefined8 *puVar8;
  longlong lVar9;
  
  puVar7 = (undefined8 *)FUN_140043d20(0x128,"APP: <ImportRaw>");
  if (puVar7 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  lVar9 = 2;
  puVar8 = puVar7 + 4;
  do {
    puVar1 = puVar8 + 0x10;
    uVar6 = param_1[1];
    puVar2 = param_1 + 0x10;
    *puVar8 = *param_1;
    puVar8[1] = uVar6;
    uVar6 = param_1[3];
    puVar8[2] = param_1[2];
    puVar8[3] = uVar6;
    uVar6 = param_1[5];
    puVar8[4] = param_1[4];
    puVar8[5] = uVar6;
    uVar6 = param_1[7];
    puVar8[6] = param_1[6];
    puVar8[7] = uVar6;
    uVar6 = param_1[9];
    puVar8[8] = param_1[8];
    puVar8[9] = uVar6;
    uVar6 = param_1[0xb];
    puVar8[10] = param_1[10];
    puVar8[0xb] = uVar6;
    uVar3 = *(undefined4 *)((longlong)param_1 + 100);
    uVar4 = *(undefined4 *)(param_1 + 0xd);
    uVar5 = *(undefined4 *)((longlong)param_1 + 0x6c);
    *(undefined4 *)(puVar8 + 0xc) = *(undefined4 *)(param_1 + 0xc);
    *(undefined4 *)((longlong)puVar8 + 100) = uVar3;
    *(undefined4 *)(puVar8 + 0xd) = uVar4;
    *(undefined4 *)((longlong)puVar8 + 0x6c) = uVar5;
    uVar3 = *(undefined4 *)((longlong)param_1 + 0x74);
    uVar4 = *(undefined4 *)(param_1 + 0xf);
    uVar5 = *(undefined4 *)((longlong)param_1 + 0x7c);
    *(undefined4 *)(puVar8 + 0xe) = *(undefined4 *)(param_1 + 0xe);
    *(undefined4 *)((longlong)puVar8 + 0x74) = uVar3;
    *(undefined4 *)(puVar8 + 0xf) = uVar4;
    *(undefined4 *)((longlong)puVar8 + 0x7c) = uVar5;
    lVar9 = lVar9 + -1;
    puVar8 = puVar1;
    param_1 = puVar2;
  } while (lVar9 != 0);
  *(undefined4 *)puVar1 = *(undefined4 *)puVar2;
  *puVar7 = &LAB_140042ee0;
  *(undefined4 *)(puVar7 + 3) = 0xb;
  puVar7[1] = puVar7;
  puVar7[2] = 0;
  FUN_140039100(puVar7);
  return;
}


/* ================= FUN_14003e2e0 @ 14003e2e0  size=147  callers=1 callees=4 ================= */
/* CALLERS: FUN_14004c6a0 */
/* CALLEES: FUN_140039100, FUN_140039300, FUN_14003f430, FUN_140043d20 */

void FUN_14003e2e0(undefined8 param_1,undefined8 param_2,undefined8 param_3)

{
  undefined8 *puVar1;
  
  FUN_140039300(&PTR_FUN_140073ef0,0x10);
  puVar1 = (undefined8 *)FUN_140043d20(0x30,"APP: <Init> Job");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  puVar1[5] = param_2;
  *puVar1 = &LAB_140042ee0;
  puVar1[4] = param_3;
  *(undefined4 *)(puVar1 + 3) = 0;
  puVar1[2] = param_1;
  puVar1[1] = puVar1;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e380 @ 14003e380  size=146  callers=1 callees=4 ================= */
/* CALLERS: FUN_14002ba50 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20, memcpy */

void FUN_14003e380(void *param_1)

{
  undefined8 *puVar1;
  
  if (param_1 != (void *)0x0) {
    puVar1 = (undefined8 *)FUN_140043d20(0x608,"APP: <InitProjSettings>");
    if (puVar1 == (undefined8 *)0x0) {
      FUN_14003f430("Insufficient memory!");
      return;
    }
    memcpy(puVar1 + 4,param_1,0x5e8);
    *(undefined4 *)(puVar1 + 3) = 3;
    puVar1[1] = puVar1;
    puVar1[2] = DAT_14286d388;
    *puVar1 = FUN_140042ef0;
    *(undefined4 *)((longlong)puVar1 + 0x1c) = 0;
    FUN_140039100(puVar1);
  }
  return;
}


/* ================= FUN_14003e420 @ 14003e420  size=181  callers=3 callees=4 ================= */
/* CALLERS: FUN_14002a1b0, FUN_140057820, FUN_1400578b0 */
/* CALLEES: FUN_140027140, FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e420(undefined8 param_1,longlong param_2,undefined8 param_3,undefined8 param_4)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x138,"APP: <OpenProjFile> Job");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
  }
  else {
    if (param_2 == 0) {
      *(undefined1 *)(puVar1 + 4) = 0;
    }
    else {
      FUN_140027140(puVar1 + 4,param_2,0x104);
    }
    puVar1[0x26] = param_3;
    puVar1[0x25] = param_4;
    *puVar1 = &LAB_140042ee0;
    puVar1[3] = 10;
    puVar1[2] = param_1;
    puVar1[1] = puVar1;
    FUN_140039100(puVar1);
  }
  return;
}


/* ================= FUN_14003e4e0 @ 14003e4e0  size=223  callers=4 callees=3 ================= */
/* CALLERS: FUN_14003d9b0, FUN_14004d6c0, FUN_14004d9c0, FUN_14004eda0 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e4e0(undefined8 *param_1,undefined1 param_2)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined8 uVar6;
  undefined8 *puVar7;
  undefined8 *puVar8;
  longlong lVar9;
  
  puVar7 = (undefined8 *)FUN_140043d20(0x128,"APP: <ParseELFFile>");
  if (puVar7 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  lVar9 = 2;
  puVar8 = puVar7 + 4;
  do {
    puVar1 = puVar8 + 0x10;
    uVar6 = param_1[1];
    puVar2 = param_1 + 0x10;
    *puVar8 = *param_1;
    puVar8[1] = uVar6;
    uVar6 = param_1[3];
    puVar8[2] = param_1[2];
    puVar8[3] = uVar6;
    uVar6 = param_1[5];
    puVar8[4] = param_1[4];
    puVar8[5] = uVar6;
    uVar6 = param_1[7];
    puVar8[6] = param_1[6];
    puVar8[7] = uVar6;
    uVar6 = param_1[9];
    puVar8[8] = param_1[8];
    puVar8[9] = uVar6;
    uVar6 = param_1[0xb];
    puVar8[10] = param_1[10];
    puVar8[0xb] = uVar6;
    uVar3 = *(undefined4 *)((longlong)param_1 + 100);
    uVar4 = *(undefined4 *)(param_1 + 0xd);
    uVar5 = *(undefined4 *)((longlong)param_1 + 0x6c);
    *(undefined4 *)(puVar8 + 0xc) = *(undefined4 *)(param_1 + 0xc);
    *(undefined4 *)((longlong)puVar8 + 100) = uVar3;
    *(undefined4 *)(puVar8 + 0xd) = uVar4;
    *(undefined4 *)((longlong)puVar8 + 0x6c) = uVar5;
    uVar3 = *(undefined4 *)((longlong)param_1 + 0x74);
    uVar4 = *(undefined4 *)(param_1 + 0xf);
    uVar5 = *(undefined4 *)((longlong)param_1 + 0x7c);
    *(undefined4 *)(puVar8 + 0xe) = *(undefined4 *)(param_1 + 0xe);
    *(undefined4 *)((longlong)puVar8 + 0x74) = uVar3;
    *(undefined4 *)(puVar8 + 0xf) = uVar4;
    *(undefined4 *)((longlong)puVar8 + 0x7c) = uVar5;
    lVar9 = lVar9 + -1;
    puVar8 = puVar1;
    param_1 = puVar2;
  } while (lVar9 != 0);
  *(undefined4 *)puVar1 = *(undefined4 *)puVar2;
  *(undefined1 *)((longlong)puVar7 + 0x124) = param_2;
  *puVar7 = FUN_140042f30;
  *(undefined4 *)(puVar7 + 3) = 4;
  puVar7[1] = puVar7;
  puVar7[2] = 0;
  FUN_140039100(puVar7);
  return;
}


/* ================= FUN_14003e5c0 @ 14003e5c0  size=84  callers=1 callees=3 ================= */
/* CALLERS: FUN_140056130 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e5c0(void)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x28,"APP: <ReadPause>");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  *(undefined4 *)(puVar1 + 3) = 6;
  *puVar1 = &LAB_140042ee0;
  puVar1[1] = puVar1;
  puVar1[2] = 0;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e620 @ 14003e620  size=84  callers=1 callees=3 ================= */
/* CALLERS: FUN_140056130 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e620(void)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x28,"APP: <ReadStart>");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  *(undefined4 *)(puVar1 + 3) = 5;
  *puVar1 = FUN_140042f60;
  puVar1[1] = puVar1;
  puVar1[2] = 0;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e680 @ 14003e680  size=84  callers=2 callees=3 ================= */
/* CALLERS: FUN_140041710, FUN_140055f90 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e680(void)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x28,"APP: <ReadStop>");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  *(undefined4 *)(puVar1 + 3) = 7;
  *puVar1 = &LAB_140042ee0;
  puVar1[1] = puVar1;
  puVar1[2] = 0;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e6e0 @ 14003e6e0  size=84  callers=1 callees=3 ================= */
/* CALLERS: thunk_FUN_14003e6e0 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e6e0(void)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x28,"APP: <SaveProject>");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  *(undefined4 *)(puVar1 + 3) = 8;
  *puVar1 = &LAB_140042ee0;
  puVar1[1] = puVar1;
  puVar1[2] = 0;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e740 @ 14003e740  size=84  callers=1 callees=3 ================= */
/* CALLERS: thunk_FUN_14003e740 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e740(void)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x28,"APP: <SaveProjectAs>");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  *(undefined4 *)(puVar1 + 3) = 9;
  *puVar1 = &LAB_140042ee0;
  puVar1[1] = puVar1;
  puVar1[2] = 0;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e7a0 @ 14003e7a0  size=93  callers=1 callees=3 ================= */
/* CALLERS: FUN_140057a00 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20 */

void FUN_14003e7a0(undefined4 param_1)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x28,"APP: <SetAppStartAction> Job");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  *(undefined4 *)(puVar1 + 4) = param_1;
  *puVar1 = &LAB_140042ee0;
  *(undefined4 *)(puVar1 + 3) = 0xe;
  puVar1[1] = puVar1;
  puVar1[2] = 0;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e800 @ 14003e800  size=126  callers=1 callees=4 ================= */
/* CALLERS: FUN_14002ba50 */
/* CALLEES: FUN_140039100, FUN_14003f430, FUN_140043d20, memcpy */

void FUN_14003e800(void *param_1)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140043d20(0x608,"APP: <UpdateProjSettings> Job");
  if (puVar1 == (undefined8 *)0x0) {
    FUN_14003f430("Insufficient memory!");
    return;
  }
  memcpy(puVar1 + 4,param_1,0x5e8);
  *(undefined4 *)(puVar1 + 3) = 0xd;
  *puVar1 = &LAB_140042ee0;
  puVar1[1] = puVar1;
  puVar1[2] = 0;
  FUN_140039100(puVar1);
  return;
}


/* ================= FUN_14003e880 @ 14003e880  size=73  callers=6 callees=1 ================= */
/* CALLERS: FUN_14003cec0, FUN_14003d210, FUN_14003d9b0, FUN_14003db70, FUN_14003ea40, FUN_140051590 */
/* CALLEES: FUN_140043e60 */

void FUN_14003e880(void)

{
  longlong lVar1;
  longlong lVar2;
  
  lVar2 = DAT_14286d3b8;
  while (lVar2 != 0) {
    lVar1 = *(longlong *)(lVar2 + 8);
    FUN_140043e60(lVar2);
    lVar2 = lVar1;
  }
  DAT_14285db70 = 1;
  DAT_14085d1e8 = 0;
  DAT_14286d3b8 = 0;
  DAT_14285d414 = 1;
  return;
}


/* ================= FUN_14003e8e0 @ 14003e8e0  size=169  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400541b0 */

void FUN_14003e8e0(longlong param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = *(uint *)(param_1 + 0x358);
  if (1 < uVar2) {
    while ((param_2 < uVar2 && (1 < uVar2))) {
      iVar1 = 0;
      for (; 9 < uVar2; uVar2 = uVar2 / 10) {
        iVar1 = iVar1 + 1;
      }
      if ((uVar2 == 1) || (uVar2 == 2)) {
        *(uint *)(param_1 + 0x358) = *(uint *)(param_1 + 0x358) >> 1;
        uVar2 = *(uint *)(param_1 + 0x358);
      }
      else if (uVar2 == 5) {
        uVar2 = (*(uint *)(param_1 + 0x358) / 5) * 2;
        *(uint *)(param_1 + 0x358) = uVar2;
      }
      else {
        *(undefined4 *)(param_1 + 0x358) = 5;
        uVar2 = 5;
        if (iVar1 != 0) {
          do {
            uVar2 = uVar2 * 10;
            iVar1 = iVar1 + -1;
          } while (iVar1 != 0);
          *(uint *)(param_1 + 0x358) = uVar2;
        }
      }
    }
  }
  return;
}


/* ================= FUN_14003e990 @ 14003e990  size=7  callers=5 callees=1 ================= */
/* CALLERS: FUN_14003cc50, FUN_14003db70, FUN_140042ef0, FUN_140042f30, FUN_140042f60 */
/* CALLEES: FUN_14003e9a0 */

void FUN_14003e990(void)

{
  FUN_14003e9a0(0);
  return;
}


/* ================= FUN_14003e9a0 @ 14003e9a0  size=155  callers=3 callees=4 ================= */
/* CALLERS: FUN_14003e990, FUN_14004eb40, FUN_14004eda0 */
/* CALLEES: FUN_14003f430, FUN_14003f4f0, FUN_140041570, _guard_check_icall */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14003e9a0(char *param_1)

{
  char cVar1;
  
  if ((param_1 != (char *)0x0) && (*param_1 != '\0')) {
    cVar1 = FUN_140041570(DAT_14285eb8c * 2 + -1);
    if ((cVar1 != '\0') && (DAT_14285eb84 == 0)) {
      _guard_check_icall(DAT_14286d388,param_1);
    }
  }
  _DAT_14285eb80 = _DAT_14285eb80 + -1;
  DAT_14285eb8c = DAT_14285eb8c + -1;
  if ((DAT_14285eb8c == 0) && (DAT_14285db80 != '\0')) {
    if (DAT_14285eb84 != 0) {
      FUN_14003f430();
      return;
    }
    FUN_14003f4f0(&DAT_14285db80,&DAT_14006cc0c,0,0,0x400,0xffffffff);
  }
  return;
}


/* ================= FUN_14003ea40 @ 14003ea40  size=30  callers=3 callees=2 ================= */
/* CALLERS: FUN_14003fc90, FUN_14004d9c0, FUN_14004e8f0 */
/* CALLEES: FUN_14003e880, FUN_140040970 */

void FUN_14003ea40(void)

{
  FUN_14003e880();
  FUN_140040970();
  DAT_14286d3b0 = 0;
  return;
}


/* ================= FUN_14003ea60 @ 14003ea60  size=21  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002a0c0 */
/* CALLEES: FUN_140041470 */

char * FUN_14003ea60(void)

{
  FUN_140041470();
  return "J-Scope";
}


/* ================= FUN_14003ea80 @ 14003ea80  size=21  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004c6a0 */
/* CALLEES: FUN_140041470 */

undefined * FUN_14003ea80(void)

{
  FUN_140041470();
  return &DAT_14285dab0;
}


/* ================= FUN_14003eaa0 @ 14003eaa0  size=39  callers=4 callees=0 ================= */
/* CALLERS: FUN_140042ac0, FUN_14004a1b0, FUN_140053420, FUN_140056f60 */

uint FUN_14003eaa0(uint param_1,longlong param_2)

{
  if (param_2 != 0) {
    return (1 << ((byte)*(undefined4 *)(param_2 + 0x338) & 0x1f)) - 1U &
           param_1 >> ((byte)*(undefined4 *)(param_2 + 0x33c) & 0x1f);
  }
  return 0;
}


/* ================= FUN_14003ead0 @ 14003ead0  size=48  callers=1 callees=0 ================= */
/* CALLERS: FUN_14004fd40 */

double FUN_14003ead0(void)

{
  if (DAT_14285da7c < DAT_14285da78) {
    return 100.0;
  }
  return (double)DAT_14285da7c * 100.0 * 1.1920928955078125e-07;
}


/* ================= FUN_14003eb00 @ 14003eb00  size=7  callers=1 callees=0 ================= */
/* CALLERS: FUN_140050030 */

undefined4 FUN_14003eb00(void)

{
  return DAT_14285d1f8;
}


/* ================= FUN_14003eb10 @ 14003eb10  size=7  callers=1 callees=0 ================= */
/* CALLERS: FUN_14004ff40 */

undefined4 FUN_14003eb10(void)

{
  return DAT_14285daa8;
}


/* ================= FUN_14003eb20 @ 14003eb20  size=230  callers=2 callees=3 ================= */
/* CALLERS: FUN_14004f590, FUN_14004f6c0 */
/* CALLEES: FUN_140028380, GetFileAttributesA, GetLastError */

undefined8 FUN_14003eb20(LPCSTR param_1,undefined8 param_2,undefined8 param_3,undefined4 param_4)

{
  DWORD DVar1;
  char *pcVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  
  uVar4 = 0;
  uVar3 = 0;
  DVar1 = GetLastError();
  if (DVar1 == 2) {
    pcVar2 = "File not found!";
  }
  else if (DVar1 == 3) {
    pcVar2 = "Path not found!";
  }
  else {
    uVar3 = uVar4;
    if (DVar1 == 5) {
      DVar1 = GetFileAttributesA(param_1);
      if ((DVar1 == 0xffffffff) || ((DVar1 & 1) == 0)) {
        pcVar2 = "Access is denied!";
      }
      else {
        pcVar2 = "File has read only attribute!";
        uVar3 = 1;
      }
    }
    else if (DVar1 == 0xf) {
      pcVar2 = "Invalid drive!";
    }
    else {
      if (DVar1 != 0x7b) {
        FUN_140028380(param_3,param_4,"Could not %s file [%s]!",param_2,param_1);
        return 0;
      }
      pcVar2 = "The filename syntax is incorrect!";
    }
  }
  FUN_140028380(param_3,param_4,"Could not %s file [%s]!\n%s",param_2,param_1,pcVar2);
  return uVar3;
}


/* ================= FUN_14003ec10 @ 14003ec10  size=8  callers=4 callees=0 ================= */
/* CALLERS: FUN_140053000, FUN_140053920, FUN_140054060, FUN_140054250 */

undefined1 FUN_14003ec10(void)

{
  return DAT_14085d180;
}


/* ================= FUN_14003ec20 @ 14003ec20  size=95  callers=1 callees=2 ================= */
/* CALLERS: FUN_14004e150 */
/* CALLEES: FUN_14004e460, FUN_14004e490 */

uint FUN_14003ec20(undefined4 param_1,uint param_2,undefined8 *param_3)

{
  uint uVar1;
  undefined8 uVar2;
  uint uVar3;
  
  uVar1 = FUN_14004e490();
  if (param_2 < uVar1) {
    uVar1 = param_2;
  }
  if ((param_3 != (undefined8 *)0x0) && (uVar3 = 0, uVar1 != 0)) {
    do {
      uVar2 = FUN_14004e460(param_1,uVar3);
      uVar3 = uVar3 + 1;
      *param_3 = uVar2;
      param_3 = param_3 + 1;
    } while (uVar3 < uVar1);
  }
  return uVar1;
}


/* ================= FUN_14003ec80 @ 14003ec80  size=7  callers=3 callees=0 ================= */
/* CALLERS: FUN_140049710, FUN_140049a10, FUN_14004a6e0 */

undefined4 FUN_14003ec80(void)

{
  return DAT_14285da84;
}


/* ================= FUN_14003ec90 @ 14003ec90  size=21  callers=0 callees=1 ================= */
/* CALLEES: FUN_140041470 */

undefined * FUN_14003ec90(void)

{
  FUN_140041470();
  return &DAT_14285daf0;
}


/* ================= FUN_14003ecb0 @ 14003ecb0  size=7  callers=3 callees=0 ================= */
/* CALLERS: FUN_140047390, FUN_140047f50, FUN_14004ad90 */

undefined4 FUN_14003ecb0(void)

{
  return DAT_14285da80;
}


/* ================= FUN_14003ecc0 @ 14003ecc0  size=21  callers=0 callees=1 ================= */
/* CALLEES: FUN_140041470 */

undefined * FUN_14003ecc0(void)

{
  FUN_140041470();
  return &DAT_14285db30;
}


/* ================= FUN_14003ece0 @ 14003ece0  size=58  callers=5 callees=0 ================= */
/* CALLERS: FUN_1400476d0, FUN_14004b640, FUN_14004b8b0, FUN_14004bc80, FUN_140053420 */

longlong FUN_14003ece0(longlong param_1,uint param_2)

{
  uint uVar1;
  
  if (0x800000 < param_2) {
    return 0;
  }
  if (DAT_14285da7c < DAT_14285da78) {
    uVar1 = DAT_14285da78 + param_2;
    param_2 = uVar1 - 0x800000;
    if (uVar1 < 0x800000) {
      param_2 = uVar1;
    }
  }
  return *(longlong *)(param_1 + 0x3d0) + (ulonglong)param_2 * 8;
}


/* ================= FUN_14003ed20 @ 14003ed20  size=21  callers=4 callees=0 ================= */
/* CALLERS: FUN_1400476d0, FUN_140049710, FUN_14004ad90, FUN_14004bc80 */

uint FUN_14003ed20(void)

{
  uint uVar1;
  
  uVar1 = DAT_14285da7c;
  if (DAT_14285da7c < DAT_14285da78) {
    uVar1 = 0x800000;
  }
  return uVar1;
}


/* ================= FUN_14003ed40 @ 14003ed40  size=8  callers=1 callees=0 ================= */
/* CALLERS: FUN_140049950 */

undefined1 FUN_14003ed40(void)

{
  return DAT_14085d182;
}


/* ================= FUN_14003ed50 @ 14003ed50  size=7  callers=5 callees=0 ================= */
/* CALLERS: FUN_140029da0, FUN_140046e80, FUN_14004fd40, FUN_140053000, FUN_140056ef0 */

undefined4 FUN_14003ed50(void)

{
  return DAT_14085d1e8;
}


/* ================= FUN_14003ed60 @ 14003ed60  size=7  callers=2 callees=0 ================= */
/* CALLERS: FUN_14004bc30, FUN_14004ff40 */

undefined4 FUN_14003ed60(void)

{
  return DAT_140096dd8;
}


/* ================= FUN_14003ed70 @ 14003ed70  size=82  callers=6 callees=0 ================= */
/* CALLERS: FUN_1400476d0, FUN_140049f40, FUN_140049fe0, FUN_14004a060, FUN_14004b640, FUN_14004bc80 */

undefined4 FUN_14003ed70(uint param_1)

{
  uint uVar1;
  
  if (0x800000 < param_1) {
    return 0;
  }
  if (DAT_14285da7c < DAT_14285da78) {
    param_1 = param_1 + DAT_14285da78;
    uVar1 = param_1 - 0x800000;
    if (param_1 < 0x800000) {
      uVar1 = param_1;
    }
    return *(undefined4 *)(&DAT_14085d1f0 + (ulonglong)uVar1 * 4);
  }
  uVar1 = DAT_14285da7c;
  if ((param_1 <= DAT_14285da7c) && (uVar1 = param_1, param_1 < DAT_14285da78)) {
    uVar1 = DAT_14285da78;
  }
  return *(undefined4 *)(&DAT_14085d1f0 + (ulonglong)uVar1 * 4);
}


/* ================= FUN_14003edd0 @ 14003edd0  size=8  callers=4 callees=0 ================= */
/* CALLERS: FUN_140047390, FUN_140047f50, FUN_140049c50, FUN_14004ad90 */

undefined * FUN_14003edd0(void)

{
  return &DAT_14085d1f0;
}


/* ================= FUN_14003ede0 @ 14003ede0  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_140052110 */

undefined8 FUN_14003ede0(void)

{
  if (DAT_14285db70 != 0) {
    DAT_14285db70 = 0;
    return 1;
  }
  return 0;
}


/* ================= FUN_14003ee00 @ 14003ee00  size=64  callers=2 callees=0 ================= */
/* CALLERS: FUN_14004c020, FUN_140053f20 */

void FUN_14003ee00(longlong param_1)

{
  longlong *plVar1;
  
  plVar1 = DAT_14286d3b8;
  if ((param_1 == 0) || (*(char *)(param_1 + 0x380) != '\x01')) {
    for (; plVar1 != (longlong *)0x0; plVar1 = (longlong *)plVar1[1]) {
      *(undefined1 *)(*plVar1 + 0x380) = 0;
    }
    if (param_1 != 0) {
      *(undefined1 *)(param_1 + 0x380) = 1;
    }
  }
  return;
}


/* ================= FUN_14003ee40 @ 14003ee40  size=148  callers=2 callees=0 ================= */
/* CALLERS: FUN_14004b830, FUN_140054200 */

void FUN_14003ee40(longlong param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = *(uint *)(param_1 + 0x358);
  if (uVar2 < 2000000000) {
    while ((uVar2 < param_2 && (uVar2 < 2000000000))) {
      iVar1 = 0;
      for (; 9 < uVar2; uVar2 = uVar2 / 10) {
        iVar1 = iVar1 + 1;
      }
      if (uVar2 == 1) {
LAB_14003eebe:
        uVar2 = *(int *)(param_1 + 0x358) * 2;
        goto LAB_14003eec7;
      }
      if (uVar2 == 2) {
        uVar2 = (*(uint *)(param_1 + 0x358) >> 1) * 5;
        goto LAB_14003eec7;
      }
      if (uVar2 == 5) goto LAB_14003eebe;
      *(undefined4 *)(param_1 + 0x358) = 10;
      uVar2 = 10;
      if (iVar1 != 0) {
        do {
          uVar2 = uVar2 * 10;
          iVar1 = iVar1 + -1;
        } while (iVar1 != 0);
LAB_14003eec7:
        *(uint *)(param_1 + 0x358) = uVar2;
      }
    }
  }
  return;
}


/* ================= FUN_14003eee0 @ 14003eee0  size=8  callers=2 callees=0 ================= */
/* CALLERS: FUN_14003d5c0, FUN_140055bb0 */

undefined1 FUN_14003eee0(void)

{
  return DAT_14085d183;
}


/* ================= FUN_14003eef0 @ 14003eef0  size=8  callers=2 callees=0 ================= */
/* CALLERS: FUN_14003ce50, FUN_14004e8f0 */

undefined1 FUN_14003eef0(void)

{
  return DAT_14285da8c;
}


/* ================= FUN_14003ef00 @ 14003ef00  size=8  callers=2 callees=0 ================= */
/* CALLERS: FUN_14003fa70, FUN_140055bb0 */

undefined1 FUN_14003ef00(void)

{
  return DAT_140096dd0;
}


/* ================= FUN_14003ef10 @ 14003ef10  size=8  callers=11 callees=0 ================= */
/* CALLERS: FUN_140029da0, FUN_14003ce50, FUN_1400482d0, FUN_14004d7e0, FUN_14004d870, FUN_14004e8f0, FUN_140053920, FUN_140054060, FUN_140054250, FUN_140055dc0, FUN_140056ad0 */

undefined1 FUN_14003ef10(void)

{
  return DAT_14085d181;
}


/* ================= FUN_14003ef20 @ 14003ef20  size=58  callers=3 callees=2 ================= */
/* CALLERS: FUN_14003ce50, FUN_14003d7d0, FUN_14004e8f0 */
/* CALLEES: FUN_14004e9c0, JLINKARM_Close */

void FUN_14003ef20(void)

{
  FUN_14004e9c0(&DAT_14285d420);
  if (DAT_14285da8c != '\0') {
    if (DAT_14285d9d8 == '\0') {
      JLINKARM_Close();
    }
    DAT_14085d183 = 0;
    DAT_14285da8c = '\0';
  }
  return;
}


/* ================= FUN_14003ef60 @ 14003ef60  size=294  callers=3 callees=8 ================= */
/* CALLERS: FUN_14003d5c0, FUN_14003db70, FUN_140042ef0 */
/* CALLEES: FUN_14003f3e0, FUN_140041400, FUN_1400418f0, FUN_1400419f0, FUN_140041f30, FUN_140042100, FUN_140043d20, FUN_14004e9c0 */

int FUN_14003ef60(void)

{
  int iVar1;
  
  FUN_14004e9c0(&DAT_14285d420);
  iVar1 = FUN_1400418f0(&DAT_14285d420);
  if (iVar1 < 0) {
    return -1;
  }
  DAT_14285da8c = 1;
  iVar1 = FUN_1400419f0(&DAT_14285d420);
  if (iVar1 < 0) {
    return -1;
  }
  if (DAT_14285d9c4 != '\0') {
    DAT_14285d1fc = 0x2000;
    if (0x2000 < DAT_14085d1b0) {
      DAT_14285d1fc = DAT_14085d1b0;
    }
    DAT_14285d200 = FUN_140043d20(DAT_14285d1fc,"RTT read buffer allocation");
    if (DAT_14285d200 == 0) {
      iVar1 = -1;
    }
    if (DAT_14285d9c4 != '\0') {
      if (DAT_14285d9c6 == '\0') {
        if ((DAT_14285da88 == 0) && (iVar1 = FUN_140042100(&DAT_14285d420), iVar1 < 0)) {
          return -1;
        }
        FUN_14004e9c0(&DAT_14285d420);
        if (DAT_14285d9c5 == '\0') {
          FUN_140041400();
        }
      }
      if (DAT_14285d9c4 != '\0') goto LAB_14003f052;
    }
  }
  iVar1 = FUN_140041f30(&DAT_14285d420);
LAB_14003f052:
  if (-1 < iVar1) {
    FUN_14003f3e0(1);
    DAT_14285da8d = 1;
    return 0;
  }
  return iVar1;
}


/* ================= FUN_14003f090 @ 14003f090  size=7  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004e8f0 */
/* CALLEES: FUN_14003d7d0 */

void FUN_14003f090(void)

{
  FUN_14003d7d0(0);
  return;
}


/* ================= FUN_14003f0a0 @ 14003f0a0  size=97  callers=1 callees=1 ================= */
/* CALLERS: FUN_140054060 */
/* CALLEES: FUN_140043e60 */

void FUN_14003f0a0(longlong param_1)

{
  longlong lVar1;
  longlong *plVar2;
  longlong *plVar3;
  longlong *plVar4;
  
  lVar1 = *DAT_14286d3b8;
  plVar2 = (longlong *)0x0;
  plVar3 = DAT_14286d3b8;
  while (lVar1 != param_1) {
    lVar1 = *(longlong *)plVar3[1];
    plVar2 = plVar3;
    plVar3 = (longlong *)plVar3[1];
  }
  *(undefined8 *)(param_1 + 0x368) = 0;
  *(undefined1 *)(param_1 + 0x380) = 0;
  plVar4 = (longlong *)plVar3[1];
  if (plVar2 != (longlong *)0x0) {
    plVar2[1] = plVar3[1];
    plVar4 = DAT_14286d3b8;
  }
  DAT_14286d3b8 = plVar4;
  DAT_14085d1e8 = DAT_14085d1e8 + -1;
  DAT_14285db70 = 1;
  DAT_14285d414 = 1;
  FUN_140043e60();
  return;
}


/* ================= FUN_14003f110 @ 14003f110  size=7  callers=10 callees=1 ================= */
/* CALLERS: FUN_14003d210, FUN_14003d5c0, FUN_14003dcb0, FUN_14003f120, FUN_1400418f0, FUN_1400419f0, FUN_140041f30, FUN_140042100, FUN_140042630, FUN_140042f60 */
/* CALLEES: FUN_1400423e0 */

void FUN_14003f110(undefined8 param_1)

{
  FUN_1400423e0(param_1,0);
  return;
}


/* ================= FUN_14003f120 @ 14003f120  size=115  callers=3 callees=3 ================= */
/* CALLERS: FUN_14003cc50, FUN_1400418f0, FUN_1400419f0 */
/* CALLEES: FUN_1400283b0, FUN_14003f110, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003f120(longlong param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  undefined8 local_res10;
  undefined8 local_res18;
  undefined8 local_res20;
  undefined1 auStack_448 [32];
  undefined8 *local_428 [2];
  undefined1 local_418 [1024];
  ulonglong local_18;
  
  if (param_1 != 0) {
    local_18 = DAT_140096fe8 ^ (ulonglong)auStack_448;
    local_428[0] = &local_res10;
    local_res10 = param_2;
    local_res18 = param_3;
    local_res20 = param_4;
    FUN_1400283b0(local_418,0x400,param_1,local_428);
    FUN_14003f110(local_418);
  }
  return;
}


/* ================= FUN_14003f1a0 @ 14003f1a0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140041cd0 */
/* CALLEES: FUN_140041570, _guard_check_icall */

void FUN_14003f1a0(undefined8 param_1)

{
  char cVar1;
  
  cVar1 = FUN_140041570(DAT_14285eb8c * 2);
  if (cVar1 != '\0') {
    _guard_check_icall(DAT_14286d388,param_1);
    return;
  }
  return;
}


/* ================= FUN_14003f1e0 @ 14003f1e0  size=149  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003cc50 */
/* CALLEES: FUN_1400283b0, FUN_140041570, __security_check_cookie, _guard_check_icall */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003f1e0(longlong param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  char cVar1;
  undefined8 local_res10;
  undefined8 local_res18;
  undefined8 local_res20;
  undefined1 auStack_448 [32];
  undefined8 *local_428 [2];
  undefined1 local_418 [1024];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_448;
  local_res10 = param_2;
  local_res18 = param_3;
  local_res20 = param_4;
  cVar1 = FUN_140041570(DAT_14285eb8c * 2);
  if ((cVar1 != '\0') && (param_1 != 0)) {
    local_428[0] = &local_res10;
    FUN_1400283b0(local_418,0x400,param_1,local_428);
    _guard_check_icall(DAT_14286d388,local_418);
  }
  return;
}


/* ================= FUN_14003f280 @ 14003f280  size=153  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400419f0 */
/* CALLEES: FUN_1400277b0, FUN_140028380, FUN_14003f640, FUN_1400591dc, __security_check_cookie, _guard_check_icall */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003f280(longlong param_1)

{
  int iVar1;
  uint uVar2;
  undefined1 auStack_838 [32];
  char local_818 [2048];
  ulonglong local_18;
  
  if (param_1 != 0) {
    local_18 = DAT_140096fe8 ^ (ulonglong)auStack_838;
    FUN_140028380(local_818,0x800,"WARNING: %s",param_1);
    iVar1 = FUN_1400277b0(local_818);
    uVar2 = iVar1 - 1;
    if (local_818[uVar2] == '\n') {
      if (0x7ff < uVar2) {
                    /* WARNING: Subroutine does not return */
        FUN_1400591dc();
      }
      local_818[uVar2] = '\0';
    }
    _guard_check_icall(DAT_14286d388,local_818);
    FUN_14003f640(param_1);
  }
  return;
}


/* ================= FUN_14003f320 @ 14003f320  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004e8f0 */
/* CALLEES: FUN_14004df20 */

void FUN_14003f320(void)

{
  FUN_14004df20(DAT_14286d388);
  return;
}


/* ================= FUN_14003f330 @ 14003f330  size=95  callers=2 callees=0 ================= */
/* CALLERS: FUN_14003fa30, FUN_14004e8f0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14003f330(void)

{
  DAT_14085d180 = 0;
  DAT_14285d20c = 0;
  DAT_14285da78 = 0;
  DAT_14285da80 = 0;
  DAT_14285da84 = 0;
  DAT_14285da88 = 0;
  DAT_14085d181 = 0;
  DAT_140096dd0 = 1;
  DAT_14085d182 = 1;
  DAT_14285da8d = 0;
  DAT_14285da8e = 0;
  DAT_140096dd4 = 1;
  _DAT_14285d410 = 1;
  return;
}


/* ================= FUN_14003f390 @ 14003f390  size=71  callers=2 callees=2 ================= */
/* CALLERS: FUN_1400427b0, FUN_140055dc0 */
/* CALLEES: FUN_14004e9c0, JLINKARM_Reset */

undefined8 FUN_14003f390(void)

{
  undefined8 uVar1;
  
  FUN_14004e9c0(&DAT_14285d420);
  if ((DAT_14085d183 != '\0') && (DAT_14285d9d8 == '\0')) {
    uVar1 = JLINKARM_Reset();
    DAT_14285d414 = 1;
    DAT_14285d20c = 0;
    return uVar1;
  }
  return 0xffffffff;
}


/* ================= FUN_14003f3e0 @ 14003f3e0  size=7  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ef60 */

void FUN_14003f3e0(undefined1 param_1)

{
  DAT_14085d183 = param_1;
  return;
}


/* ================= FUN_14003f3f0 @ 14003f3f0  size=7  callers=1 callees=0 ================= */
/* CALLERS: FUN_14004c1b0 */

void FUN_14003f3f0(undefined4 param_1)

{
  DAT_14285daa8 = param_1;
  return;
}


/* ================= FUN_14003f400 @ 14003f400  size=7  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400419f0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14003f400(undefined4 param_1)

{
  _DAT_14285da90 = param_1;
  return;
}


/* ================= FUN_14003f410 @ 14003f410  size=7  callers=2 callees=0 ================= */
/* CALLERS: FUN_140055e50, FUN_140055e90 */

void FUN_14003f410(undefined4 param_1)

{
  DAT_14285d208 = param_1;
  return;
}


/* ================= FUN_14003f420 @ 14003f420  size=7  callers=2 callees=0 ================= */
/* CALLERS: FUN_140054790, FUN_140056070 */

void FUN_14003f420(undefined4 param_1)

{
  DAT_140096dd8 = param_1;
  return;
}


/* ================= FUN_14003f430 @ 14003f430  size=90  callers=30 callees=2 ================= */
/* CALLERS: FUN_14002a410, FUN_14003dcb0, FUN_14003dd90, FUN_14003e0b0, FUN_14003e110, FUN_14003e1b0, FUN_14003e210, FUN_14003e2e0, FUN_14003e380, FUN_14003e420, FUN_14003e4e0, FUN_14003e5c0, FUN_14003e620, FUN_14003e680, FUN_14003e6e0, FUN_14003e740, FUN_14003e7a0, FUN_14003e800, FUN_14003e9a0, FUN_14003f760, FUN_1400455e0, FUN_140046e80, FUN_140047020, FUN_1400471d0, FUN_140047390, FUN_1400476d0, FUN_14004efc0, FUN_14004f0c0, FUN_14004f590, FUN_14004f6c0 */
/* CALLEES: FUN_14003f580, thunk_FUN_14003ec90 */

void FUN_14003f430(undefined8 param_1)

{
  undefined8 uVar1;
  
  uVar1 = thunk_FUN_14003ec90();
  FUN_14003f580(uVar1,"Error",param_1,&DAT_1400746bc,0,0,0,0,0x300,0xffffffff,0xffffffff);
  return;
}


/* ================= FUN_14003f490 @ 14003f490  size=90  callers=1 callees=2 ================= */
/* CALLERS: FUN_140041400 */
/* CALLEES: FUN_14003f580, thunk_FUN_14003ec90 */

void FUN_14003f490(undefined8 param_1)

{
  undefined8 uVar1;
  
  uVar1 = thunk_FUN_14003ec90();
  FUN_14003f580(uVar1,"Information",param_1,&DAT_1400746bc,0,0,0,0,0x400,0xffffffff,0xffffffff);
  return;
}


/* ================= FUN_14003f4f0 @ 14003f4f0  size=132  callers=4 callees=2 ================= */
/* CALLERS: FUN_14003d9b0, FUN_14003e9a0, FUN_1400476d0, FUN_14004e720 */
/* CALLEES: FUN_14003f580, thunk_FUN_14003ec90 */

void FUN_14003f4f0(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined4 param_4,
                  undefined4 param_5,undefined4 param_6)

{
  undefined8 uVar1;
  
  uVar1 = thunk_FUN_14003ec90();
  FUN_14003f580(uVar1,0,param_1,param_2,param_3,param_4,0,0,param_5,param_6,0xffffffff);
  return;
}


/* ================= FUN_14003f580 @ 14003f580  size=192  callers=5 callees=3 ================= */
/* CALLERS: FUN_14003f430, FUN_14003f490, FUN_14003f4f0, FUN_14003f640, FUN_1400427b0 */
/* CALLEES: FUN_140028380, FUN_14004df50, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003f580(undefined1 *param_1,longlong param_2,undefined8 param_3,undefined8 param_4,
                  undefined8 param_5,undefined4 param_6,undefined8 param_7,undefined8 param_8,
                  undefined4 param_9,undefined4 param_10,undefined4 param_11)

{
  undefined1 auStack_188 [32];
  longlong local_168;
  undefined4 local_160;
  undefined8 local_158;
  undefined8 local_150;
  undefined4 local_148;
  undefined4 local_140;
  undefined4 local_138;
  undefined1 local_128 [256];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_188;
  if (param_2 != 0) {
    local_168 = param_2;
    FUN_140028380(local_128,0x100,"%s %s",param_1);
    param_1 = local_128;
  }
  local_138 = param_11;
  local_140 = param_10;
  local_148 = param_9;
  local_150 = 0;
  local_158 = 0;
  local_160 = param_6;
  local_168 = param_5;
  FUN_14004df50(DAT_14286d388,param_1,param_3,param_4);
  return;
}


/* ================= FUN_14003f640 @ 14003f640  size=90  callers=6 callees=2 ================= */
/* CALLERS: FUN_14002a1f0, FUN_14002ba50, FUN_14003f280, FUN_140041cd0, FUN_1400455e0, FUN_140046e80 */
/* CALLEES: FUN_14003f580, thunk_FUN_14003ec90 */

void FUN_14003f640(undefined8 param_1)

{
  undefined8 uVar1;
  
  uVar1 = thunk_FUN_14003ec90();
  FUN_14003f580(uVar1,"Warning",param_1,&DAT_1400746bc,0,0,0,0,0x300,0xffffffff,0xffffffff);
  return;
}


/* ================= FUN_14003f6a0 @ 14003f6a0  size=45  callers=7 callees=1 ================= */
/* CALLERS: FUN_14003f760, FUN_140040bd0, FUN_140050e30, FUN_140050f60, FUN_1400511f0, FUN_1400513a0, FUN_140051490 */
/* CALLEES: FUN_140043e60 */

void FUN_14003f6a0(longlong *param_1)

{
  if (param_1 != (longlong *)0x0) {
    if (*param_1 != 0) {
      FUN_140043e60();
    }
    *param_1 = 0;
    param_1[2] = 0;
    param_1[1] = 0;
  }
  return;
}


/* ================= FUN_14003f6d0 @ 14003f6d0  size=69  callers=7 callees=1 ================= */
/* CALLERS: FUN_140040bd0, FUN_140050e30, FUN_140050f60, FUN_140051130, FUN_1400511f0, FUN_1400513a0, FUN_140051490 */
/* CALLEES: FUN_140043d10 */

undefined8 * FUN_14003f6d0(undefined8 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined8 uVar1;
  
  uVar1 = FUN_140043d10(param_3);
  *param_1 = uVar1;
  *(undefined4 *)((longlong)param_1 + 0x14) = param_3;
  *(undefined4 *)((longlong)param_1 + 0xc) = param_3;
  *(undefined4 *)(param_1 + 1) = 0;
  *(undefined4 *)(param_1 + 2) = param_2;
  return param_1;
}


/* ================= FUN_14003f720 @ 14003f720  size=59  callers=7 callees=1 ================= */
/* CALLERS: FUN_140040bd0, FUN_140050e30, FUN_140050f60, FUN_140051130, FUN_1400511f0, FUN_1400513a0, FUN_140051490 */
/* CALLEES: memcpy */

void FUN_14003f720(longlong *param_1,void *param_2)

{
  int iVar1;
  
  if ((((param_1 != (longlong *)0x0) && (*param_1 != 0)) && (param_2 != (void *)0x0)) &&
     ((int)param_1[1] != 0)) {
    iVar1 = (int)param_1[1] + -1;
    *(int *)(param_1 + 1) = iVar1;
    memcpy(param_2,(void *)((ulonglong)(iVar1 * *(uint *)(param_1 + 2)) + *param_1),
           (ulonglong)*(uint *)(param_1 + 2));
    return;
  }
  return;
}


/* ================= FUN_14003f760 @ 14003f760  size=195  callers=7 callees=4 ================= */
/* CALLERS: FUN_140040bd0, FUN_140050e30, FUN_140050f60, FUN_140051130, FUN_1400511f0, FUN_1400513a0, FUN_140051490 */
/* CALLEES: FUN_14003f430, FUN_14003f6a0, FUN_140043fd0, memcpy */

void FUN_14003f760(longlong *param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  uint uVar3;
  longlong lVar4;
  uint uVar5;
  longlong local_res10;
  
  if ((((param_1 != (longlong *)0x0) && (lVar2 = *param_1, lVar2 != 0)) && (param_2 != 0)) &&
     ((int)param_1[1] != -1)) {
    iVar1 = (int)param_1[1] + 1;
    uVar3 = *(uint *)(param_1 + 2);
    *(int *)(param_1 + 1) = iVar1;
    uVar5 = iVar1 * uVar3;
    lVar4 = lVar2;
    local_res10 = param_2;
    if (*(uint *)((longlong)param_1 + 0xc) < uVar5) {
      lVar4 = FUN_140043fd0(lVar2,*(int *)((longlong)param_1 + 0x14) +
                                  *(uint *)((longlong)param_1 + 0xc));
      *param_1 = lVar4;
      if (lVar4 == 0) {
        FUN_14003f430("Insufficient memory!");
        *param_1 = lVar2;
        FUN_14003f6a0(param_1);
        return;
      }
      *(int *)((longlong)param_1 + 0xc) =
           *(int *)((longlong)param_1 + 0xc) + *(int *)((longlong)param_1 + 0x14);
      uVar3 = *(uint *)(param_1 + 2);
    }
    memcpy((void *)(((ulonglong)uVar5 - (ulonglong)uVar3) + lVar4),&local_res10,(ulonglong)uVar3);
  }
  return;
}


/* ================= FUN_14003f830 @ 14003f830  size=96  callers=7 callees=3 ================= */
/* CALLERS: FUN_14003cc50, FUN_14003cec0, FUN_14003d210, FUN_14003d5c0, FUN_14003db70, FUN_14004e8f0, FUN_14004eda0 */
/* CALLEES: FUN_140041570, FUN_140043cc0, _guard_check_icall */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14003f830(undefined8 param_1)

{
  int iVar1;
  char cVar2;
  
  cVar2 = FUN_140041570(DAT_14285eb8c * 2);
  if (cVar2 != '\0') {
    _guard_check_icall(DAT_14286d388,param_1);
  }
  _DAT_14285eb80 = _DAT_14285eb80 + 1;
  iVar1 = DAT_14285eb8c + 1;
  if (DAT_14285eb8c == 0) {
    DAT_14285db80 = 0;
    DAT_14285eb84 = DAT_14285eb8c;
    DAT_14285eb8c = DAT_14285eb8c + 1;
    _DAT_14285eb88 = FUN_140043cc0();
    iVar1 = DAT_14285eb8c;
  }
  DAT_14285eb8c = iVar1;
  return;
}


/* ================= FUN_14003f890 @ 14003f890  size=54  callers=2 callees=2 ================= */
/* CALLERS: FUN_1400427b0, FUN_140055ed0 */
/* CALLEES: FUN_14004e9c0, JLINKARM_Go */

void FUN_14003f890(void)

{
  FUN_14004e9c0(&DAT_14285d420);
  if ((DAT_14085d183 != '\0') && (DAT_14285d9d8 == '\0')) {
    JLINKARM_Go();
    DAT_14285d20c = 1;
  }
  return;
}


/* ================= FUN_14003f8d0 @ 14003f8d0  size=67  callers=1 callees=2 ================= */
/* CALLERS: FUN_140055ed0 */
/* CALLEES: FUN_14004e9c0, JLINKARM_Halt */

int FUN_14003f8d0(void)

{
  char cVar1;
  
  FUN_14004e9c0(&DAT_14285d420);
  if ((DAT_14085d183 != '\0') && (DAT_14285d9d8 == '\0')) {
    cVar1 = JLINKARM_Halt();
    DAT_14285d20c = 0;
    return (int)cVar1;
  }
  return -1;
}


/* ================= FUN_14003f920 @ 14003f920  size=7  callers=2 callees=0 ================= */
/* CALLERS: FUN_140055bb0, FUN_140055ed0 */

undefined4 FUN_14003f920(void)

{
  return DAT_14285d20c;
}


/* ================= FUN_14003f930 @ 14003f930  size=225  callers=4 callees=8 ================= */
/* CALLERS: FUN_14003cc50, FUN_14004ea70, FUN_14004eef0, thunk_FUN_14003f930 */
/* CALLEES: FUN_140027140, FUN_140027700, FUN_14004e2c0, FUN_14004e990, FUN_14004e9a0, FUN_14004ea40, __security_check_cookie, thunk_FUN_14003ea80 */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003f930(void)

{
  int iVar1;
  undefined8 uVar2;
  longlong lVar3;
  undefined1 auStack_1a8 [32];
  undefined1 local_188 [368];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_1a8;
  uVar2 = thunk_FUN_14003ea80();
  FUN_140027140(local_188,uVar2,0x168);
  iVar1 = FUN_14004ea40();
  if ((iVar1 != 0) && (lVar3 = FUN_14004e9a0(), lVar3 != 0)) {
    FUN_140027700(local_188,&DAT_140075054,0x168);
    FUN_140027700(local_188,lVar3,0x168);
    iVar1 = FUN_14004e990();
    if (0 < iVar1) {
      FUN_140027700(local_188,&DAT_14007505c,0x168);
    }
    FUN_140027700(local_188,&DAT_14005f580,0x168);
  }
  FUN_14004e2c0(DAT_14286d388,local_188);
  return;
}


/* ================= FUN_14003fa20 @ 14003fa20  size=12  callers=2 callees=1 ================= */
/* CALLERS: FUN_140048d10, FUN_14004d9c0 */
/* CALLEES: FUN_14004e120 */

void FUN_14003fa20(void)

{
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_14003fa30 @ 14003fa30  size=49  callers=2 callees=4 ================= */
/* CALLERS: FUN_14003cec0, FUN_14004ec80 */
/* CALLEES: FUN_14003f330, FUN_14004df20, FUN_14004e9c0, FUN_14004ea20 */

void FUN_14003fa30(void)

{
  FUN_14004e9c0(&DAT_14285d420);
  FUN_14004ea20(&DAT_14285d9e0);
  FUN_14003f330();
  FUN_14004df20(DAT_14286d388);
  return;
}


/* ================= FUN_14003fa70 @ 14003fa70  size=452  callers=3 callees=7 ================= */
/* CALLERS: FUN_14003de60, FUN_140040fa0, FUN_140042260 */
/* CALLEES: FUN_14003ef00, FUN_14003feb0, FUN_14003ff40, FUN_140041540, FUN_1400415a0, FUN_140042ac0, FUN_14004ea20 */

void FUN_14003fa70(uint param_1,longlong param_2)

{
  longlong lVar1;
  longlong *plVar2;
  char cVar3;
  int iVar4;
  longlong lVar5;
  uint uVar6;
  uint uVar7;
  ulonglong uVar8;
  
  FUN_14004ea20(&DAT_14285d9e0);
  DAT_14085d1ec = param_1 / DAT_14085d184;
  if (param_1 != 0) {
    uVar8 = (ulonglong)DAT_14285da80;
    uVar6 = DAT_14085d184;
    do {
      FUN_14003ff40(param_2);
      uVar7 = (uint)uVar8;
      DAT_14285da84 = *(undefined4 *)(&DAT_14085d1f0 + uVar8 * 4);
      cVar3 = FUN_14003ef00();
      plVar2 = DAT_14286d3b8;
      if (cVar3 == '\0') {
        for (; plVar2 != (longlong *)0x0; plVar2 = (longlong *)plVar2[1]) {
          lVar1 = *plVar2;
          FUN_14003feb0(lVar1,param_2);
          lVar5 = *(longlong *)(lVar1 + 0x3d0);
          iVar4 = FUN_140041540();
          if (iVar4 == 0) {
            iVar4 = FUN_1400415a0(&DAT_14285d9e0,lVar1,lVar5 + (ulonglong)((int)uVar8 - 1) * 8,
                                  lVar5 + uVar8 * 8);
            uVar8 = (ulonglong)DAT_14285da80;
            if (iVar4 == 0) {
              if (lVar1 == DAT_14285d9e0) {
                *(ulonglong *)(lVar1 + 0x3c8) = *(longlong *)(lVar1 + 0x3d0) + uVar8 * 8;
              }
LAB_14003fbb4:
              *(undefined1 *)(lVar1 + 0x388) = 0;
            }
            else {
              if (iVar4 == 1) {
                uVar8 = 0;
                DAT_14285da80 = 0;
                DAT_14285da78 = 0;
              }
              else if ((iVar4 != 2) && (iVar4 != 3)) goto LAB_14003fbb4;
              *(ulonglong *)(lVar1 + 0x3c8) = *(longlong *)(lVar1 + 0x3d0) + uVar8 * 8;
              FUN_140042ac0(lVar1);
              *(undefined1 *)(lVar1 + 0x388) = 1;
            }
          }
          else {
            *(longlong *)(lVar1 + 0x3c8) = lVar5;
          }
          uVar7 = (uint)uVar8;
          uVar6 = DAT_14085d184;
        }
      }
      param_1 = param_1 - uVar6;
      param_2 = param_2 + (ulonglong)uVar6;
      if (uVar7 < DAT_14285da78) {
        DAT_14285da78 = DAT_14285da78 + 1;
      }
      DAT_14285da80 = uVar7 + 1;
      uVar8 = (ulonglong)DAT_14285da80;
      if (0x7fffff < DAT_14285da80) {
        DAT_14285da80 = 0;
        DAT_14285da78 = 1;
        uVar8 = 0;
      }
      DAT_14285da7c = uVar7;
    } while (param_1 != 0);
  }
  return;
}


/* ================= FUN_14003fc40 @ 14003fc40  size=65  callers=3 callees=2 ================= */
/* CALLERS: FUN_14003d210, FUN_140040320, FUN_1400404c0 */
/* CALLEES: FUN_14003fee0, FUN_140040ab0 */

undefined8 FUN_14003fc40(longlong param_1)

{
  undefined8 uVar1;
  
  if (param_1 == 0) {
    return 0;
  }
  if (*(longlong *)(param_1 + 0x3d8) != 0) {
    uVar1 = FUN_14003fee0();
    return uVar1;
  }
  uVar1 = FUN_140040ab0(param_1);
  *(undefined8 *)(param_1 + 0x3d8) = uVar1;
  return uVar1;
}


/* ================= FUN_14003fc90 @ 14003fc90  size=544  callers=1 callees=7 ================= */
/* CALLERS: FUN_140042100 */
/* CALLEES: FUN_140027140, FUN_140027860, FUN_140028380, FUN_14003dfa0, FUN_14003ea40, FUN_14004ec40, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003fc90(longlong param_1)

{
  longlong lVar1;
  char cVar2;
  char *pcVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined1 auStack_178 [32];
  int local_158;
  char local_148 [32];
  undefined1 local_128 [256];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_178;
  iVar4 = 0;
  FUN_140027140(local_148,&DAT_14085d190,0x20);
  if (local_148[0] != '_') {
    pcVar3 = local_148;
    do {
      if (local_148[0] == '\0') break;
      local_148[0] = pcVar3[1];
      pcVar3 = pcVar3 + 1;
      iVar4 = iVar4 + 1;
    } while (local_148[0] != '_');
  }
  FUN_14003ea40();
  iVar6 = 0;
  cVar2 = local_148[(longlong)iVar4 + 1];
  lVar1 = (longlong)iVar4 + 1;
  do {
    if ((cVar2 == '\0') || (local_148[lVar1 + 1] == '\0')) {
      return;
    }
    cVar2 = FUN_140027860(local_148[lVar1]);
    iVar4 = (int)local_148[lVar1 + 1];
    iVar7 = iVar4 + -0x30;
    if (cVar2 == 't') {
      *(undefined1 *)(param_1 + 0x5a5) = 1;
      FUN_14004ec40(param_1);
    }
    else {
      if (cVar2 == 'b') {
        iVar5 = 0;
        if (iVar7 == 1) {
          iVar5 = 5;
        }
        pcVar3 = "%d Byte bool RTT Data %d";
      }
      else if (cVar2 == 'f') {
        if (iVar7 == 4) {
          pcVar3 = "%d Byte float RTT Data %d";
          iVar5 = 1;
        }
        else {
          pcVar3 = "%d Byte float RTT Data %d";
          iVar5 = 2;
          if (iVar7 != 8) {
            iVar5 = 0;
          }
        }
      }
      else {
        iVar5 = 0;
        if (cVar2 == 'u') {
          if (iVar4 == 0x31) {
            iVar5 = 6;
            pcVar3 = "%d Byte unsigned RTT Data %d";
          }
          else if (iVar4 == 0x32) {
            iVar5 = 7;
            pcVar3 = "%d Byte unsigned RTT Data %d";
          }
          else if (iVar7 == 4) {
            iVar5 = 8;
            pcVar3 = "%d Byte unsigned RTT Data %d";
          }
          else if (iVar7 == 8) {
            iVar5 = 9;
            pcVar3 = "%d Byte unsigned RTT Data %d";
          }
          else {
            pcVar3 = "%d Byte unsigned RTT Data %d";
          }
        }
        else {
          if (cVar2 != 'i') goto LAB_14003fe70;
          if (iVar4 == 0x31) {
            iVar5 = 10;
          }
          else if (iVar4 == 0x32) {
            iVar5 = 0xb;
          }
          else if (iVar7 == 4) {
            iVar5 = 0xc;
          }
          else if (iVar7 == 8) {
            iVar5 = 0xd;
          }
          pcVar3 = "%d Byte signed RTT Data %d";
        }
      }
      local_158 = iVar6;
      FUN_140028380(local_128,0x100,pcVar3,iVar7);
      if (iVar5 != 0) {
        FUN_14003dfa0(iVar7,iVar5,0,local_128);
        iVar6 = iVar6 + 1;
      }
    }
LAB_14003fe70:
    cVar2 = local_148[lVar1 + 2];
    lVar1 = lVar1 + 2;
  } while( true );
}


/* ================= FUN_14003feb0 @ 14003feb0  size=40  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003fa70 */
/* CALLEES: memcpy */

void FUN_14003feb0(longlong param_1,longlong param_2)

{
  memcpy((void *)(*(longlong *)(param_1 + 0x3d0) + (ulonglong)DAT_14285da80 * 8),
         (void *)(param_2 + (ulonglong)*(uint *)(param_1 + 0x38c)),
         (ulonglong)*(uint *)(param_1 + 0x31c));
  return;
}


/* ================= FUN_14003fee0 @ 14003fee0  size=88  callers=2 callees=1 ================= */
/* CALLERS: FUN_14003d210, FUN_14003fc40 */
/* CALLEES: FUN_140040ab0 */

undefined8 FUN_14003fee0(longlong param_1)

{
  longlong lVar1;
  longlong lVar2;
  undefined8 uVar3;
  
  if (param_1 == 0) {
    return 0;
  }
  lVar2 = *(longlong *)(param_1 + 0x3e0);
  while (lVar1 = lVar2, lVar1 != 0) {
    param_1 = lVar1;
    lVar2 = *(longlong *)(lVar1 + 0x3e0);
  }
  uVar3 = FUN_140040ab0(*(undefined8 *)(param_1 + 1000));
  *(undefined8 *)(param_1 + 0x3e0) = uVar3;
  return uVar3;
}


/* ================= FUN_14003ff40 @ 14003ff40  size=130  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003fa70 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14003ff40(undefined4 *param_1)

{
  if (DAT_14285d9c4 == '\0') {
    *(undefined4 *)(&DAT_14085d1f0 + (ulonglong)DAT_14285da80 * 4) = *param_1;
    return;
  }
  if (DAT_14285d9c5 != '\0') {
    *(undefined4 *)(&DAT_14085d1f0 + (ulonglong)DAT_14285da80 * 4) = *param_1;
    return;
  }
  if (DAT_14285da80 != 0) {
    *(int *)(&DAT_14085d1f0 + (ulonglong)DAT_14285da80 * 4) =
         *(int *)(&DAT_14085d1f0 + (ulonglong)DAT_14285da7c * 4) + 100;
    return;
  }
  if (DAT_14285da78 < DAT_14285da7c) {
    _DAT_14085d1f0 = *(int *)(&DAT_14085d1f0 + (ulonglong)DAT_14285da7c * 4) + 100;
    return;
  }
  _DAT_14085d1f0 = 0;
  return;
}


/* ================= FUN_14003ffd0 @ 14003ffd0  size=122  callers=1 callees=1 ================= */
/* CALLERS: FUN_140041870 */
/* CALLEES: FUN_140043d10 */

void FUN_14003ffd0(undefined8 param_1)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  longlong lVar4;
  
  if (DAT_14285da08 == (undefined8 *)0x0) {
    puVar3 = (undefined8 *)FUN_140043d10(0x10);
    DAT_14285da08 = puVar3;
    puVar3[1] = 0;
  }
  else {
    puVar2 = (undefined8 *)DAT_14285da08[1];
    puVar3 = DAT_14285da08;
    while (puVar1 = puVar2, puVar1 != (undefined8 *)0x0) {
      puVar3 = puVar1;
      puVar2 = (undefined8 *)puVar1[1];
    }
    lVar4 = FUN_140043d10(0x10);
    puVar3[1] = lVar4;
    *(undefined8 *)(lVar4 + 8) = 0;
    puVar3 = (undefined8 *)puVar3[1];
  }
  DAT_14285d9dc = DAT_14285d9dc + 1;
  *puVar3 = param_1;
  return;
}


/* ================= FUN_140040050 @ 140040050  size=709  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_140027760, FUN_1400423a0 */

void FUN_140040050(longlong param_1)

{
  int iVar1;
  undefined8 uVar2;
  
  if (*(longlong *)(param_1 + 0x3d8) != 0) {
    *(undefined4 *)(param_1 + 0x314) = 0;
    return;
  }
  *(undefined1 *)(param_1 + 0x35f) = 1;
  if (*(int *)(param_1 + 0x314) == 0xe) {
    return;
  }
  uVar2 = FUN_1400423a0(param_1 + 0x110);
  if ((*(int *)(param_1 + 0x31c) == 1) &&
     ((iVar1 = FUN_140027760(uVar2,&DAT_1400606d4), iVar1 == 0 ||
      (iVar1 = FUN_140027760(uVar2,"_Bool"), iVar1 == 0)))) {
    *(undefined8 *)(param_1 + 0x3b0) = 0x3ff0000000000000;
    *(undefined4 *)(param_1 + 0x314) = 5;
    *(undefined1 *)(param_1 + 0x330) = 0;
    return;
  }
  if ((*(int *)(param_1 + 0x31c) == 4) && (iVar1 = FUN_140027760(uVar2,"float"), iVar1 == 0)) {
    *(undefined8 *)(param_1 + 0x3b0) = 0x47efffffe0000000;
    *(undefined4 *)(param_1 + 0x314) = 1;
    *(undefined1 *)(param_1 + 0x330) = 1;
    return;
  }
  if ((*(int *)(param_1 + 0x31c) == 8) && (iVar1 = FUN_140027760(uVar2,"double"), iVar1 == 0)) {
    *(undefined8 *)(param_1 + 0x3b0) = 0x7fefffffffffffff;
    *(undefined4 *)(param_1 + 0x314) = 2;
    *(undefined1 *)(param_1 + 0x330) = 1;
    return;
  }
  iVar1 = *(int *)(param_1 + 0x31c);
  if (*(char *)(param_1 + 0x330) == '\0') {
    if (iVar1 == 1) {
      *(undefined8 *)(param_1 + 0x3b0) = 0x406fe00000000000;
      *(undefined4 *)(param_1 + 0x314) = 6;
      return;
    }
    if (iVar1 == 2) {
      *(undefined8 *)(param_1 + 0x3b0) = 0x40efffe000000000;
      *(undefined4 *)(param_1 + 0x314) = 7;
      return;
    }
    if (iVar1 == 4) {
      *(undefined8 *)(param_1 + 0x3b0) = 0x41efffffffe00000;
      *(undefined4 *)(param_1 + 0x314) = 8;
      return;
    }
    if (iVar1 == 8) {
      *(undefined8 *)(param_1 + 0x3b0) = 0x43f0000000000000;
      *(undefined4 *)(param_1 + 0x314) = 9;
      return;
    }
  }
  else {
    if (iVar1 == 1) {
      *(undefined8 *)(param_1 + 0x3b0) = 0x405fc00000000000;
      *(undefined4 *)(param_1 + 0x314) = 10;
      return;
    }
    if (iVar1 == 2) {
      *(undefined8 *)(param_1 + 0x3b0) = 0x40dfffc000000000;
      *(undefined4 *)(param_1 + 0x314) = 0xb;
      return;
    }
    if (iVar1 == 4) {
      *(undefined8 *)(param_1 + 0x3b0) = 0x41dfffffffc00000;
      *(undefined4 *)(param_1 + 0x314) = 0xc;
      return;
    }
    if (iVar1 == 8) {
      *(undefined8 *)(param_1 + 0x3b0) = 0x43e0000000000000;
      *(undefined4 *)(param_1 + 0x314) = 0xd;
      return;
    }
  }
  *(undefined4 *)(param_1 + 0x314) = 0;
  return;
}


/* ================= FUN_140040320 @ 140040320  size=411  callers=2 callees=9 ================= */
/* CALLERS: FUN_140040320, FUN_1400404c0 */
/* CALLEES: FUN_140027140, FUN_140027700, FUN_140028380, FUN_14003fc40, FUN_140040320, FUN_1400404c0, FUN_140040e90, __security_check_cookie, thunk_FUN_140008cb0 */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140040320(uint *param_1,uint param_2,undefined8 param_3,longlong param_4,longlong param_5,
                  undefined8 param_6)

{
  int iVar1;
  longlong lVar2;
  uint uVar3;
  uint uVar4;
  undefined1 auStack_288 [32];
  longlong local_268;
  undefined8 local_260;
  undefined1 local_258 [256];
  undefined1 local_158 [256];
  ulonglong local_58;
  
  if ((param_4 != 0) && (local_58 = DAT_140096fe8 ^ (ulonglong)auStack_288, param_2 < 0x21)) {
    uVar4 = 0;
    while( true ) {
      uVar3 = 30000;
      if (*param_1 < 30000) {
        uVar3 = *param_1;
      }
      if (uVar3 <= uVar4) break;
      FUN_140027140(local_258,param_5 + 4,0x100);
      FUN_140028380(local_158,0x100,&DAT_140074518,uVar4);
      FUN_140027700(local_258,local_158,0x100);
      lVar2 = FUN_14003fc40(param_5);
      FUN_140027140(lVar2 + 4,local_258,0x100);
      if (*(longlong *)(param_1 + 2) == 0) {
        iVar1 = thunk_FUN_140008cb0(param_4);
        local_260 = param_6;
        local_268 = CONCAT44(local_268._4_4_,param_2 + 1);
        *(uint *)(lVar2 + 0x318) = iVar1 * uVar4 + *(int *)(param_5 + 0x318);
        FUN_1400404c0(param_3,param_4,lVar2,param_5);
        uVar4 = uVar4 + 1;
      }
      else {
        *(undefined1 *)(lVar2 + 0x334) = 1;
        iVar1 = FUN_140040e90(*(undefined8 *)(param_1 + 2),param_4);
        local_260 = param_6;
        *(uint *)(lVar2 + 0x318) = iVar1 * uVar4 + *(int *)(param_5 + 0x318);
        local_268 = lVar2;
        FUN_140040320(*(undefined8 *)(param_1 + 2),param_2 + 1,param_3,param_4);
        uVar4 = uVar4 + 1;
      }
    }
  }
  return;
}


/* ================= FUN_1400404c0 @ 1400404c0  size=1195  callers=3 callees=25 ================= */
/* CALLERS: FUN_14003d210, FUN_140040320, FUN_1400404c0 */
/* CALLEES: FUN_140002c00, FUN_140002cb0, FUN_140002cd0, FUN_140002cf0, FUN_140002d20, FUN_140002d40, FUN_140002d60, FUN_140002d80, FUN_140002da0, FUN_140002e30, FUN_140002e50, FUN_140002e80, FUN_140002eb0, FUN_140027140, FUN_14003fc40, FUN_140040050, FUN_140040320, FUN_1400404c0, FUN_140040cd0, FUN_140041870, FUN_140043d20, __security_check_cookie, thunk_FUN_140008a60, thunk_FUN_140008ad0, thunk_FUN_140008cb0 */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_1400404c0(undefined8 param_1,longlong param_2,int *param_3,uint *param_4,uint param_5,
                  longlong param_6)

{
  longlong *plVar1;
  ulonglong uVar2;
  undefined1 uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 *puVar9;
  longlong lVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  undefined4 *puVar13;
  uint uVar14;
  ulonglong uVar15;
  undefined4 *puVar16;
  undefined1 auStack_1f8 [32];
  int *local_1d8;
  longlong local_1d0;
  undefined8 local_1c0;
  longlong local_1b8;
  undefined8 uStack_1b0;
  undefined8 local_1a8;
  undefined8 uStack_1a0;
  undefined8 local_198;
  undefined8 uStack_190;
  undefined8 local_188;
  undefined8 uStack_180;
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
  undefined8 local_128;
  undefined8 uStack_120;
  undefined4 local_118;
  undefined4 uStack_114;
  undefined4 uStack_110;
  undefined4 uStack_10c;
  undefined8 local_108;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined8 local_e8;
  ulonglong uStack_e0;
  longlong local_d8 [4];
  undefined8 local_b8;
  undefined8 uStack_b0;
  undefined4 local_a8;
  undefined4 uStack_a4;
  undefined4 uStack_a0;
  undefined4 uStack_9c;
  undefined4 local_98;
  undefined4 uStack_94;
  undefined4 uStack_90;
  undefined4 uStack_8c;
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_1f8;
  *(uint **)(param_3 + 0xfa) = param_4;
  if (param_2 == 0) {
    return;
  }
  if (0x20 < param_5) {
    return;
  }
  local_1c0 = param_1;
  local_1b8 = param_2;
  uVar7 = FUN_140002d20(param_2,1);
  iVar4 = thunk_FUN_140008ad0(uVar7);
  if (iVar4 == 0x2e) {
    return;
  }
  if (param_6 == 0) {
    uVar8 = thunk_FUN_140008a60(param_2);
    FUN_140027140(param_3 + 1,uVar8,0x100);
    puVar9 = (undefined8 *)FUN_140002da0(&local_f8,param_2,0xffffffffffffffff);
    local_1a8 = *puVar9;
    uStack_1a0 = puVar9[1];
    local_198 = puVar9[2];
    uStack_190 = puVar9[3];
    local_188 = puVar9[4];
    uStack_180 = puVar9[5];
    local_178 = puVar9[6];
    uStack_170 = puVar9[7];
    local_168 = puVar9[8];
    uStack_160 = puVar9[9];
    local_158 = puVar9[10];
    uStack_150 = puVar9[0xb];
    local_148 = puVar9[0xc];
    uStack_140 = puVar9[0xd];
    local_128 = puVar9[0x10];
    uStack_120 = puVar9[0x11];
    local_138 = puVar9[0xe];
    uStack_130 = puVar9[0xf];
    local_118 = *(undefined4 *)(puVar9 + 0x12);
    uStack_114 = *(undefined4 *)((longlong)puVar9 + 0x94);
    uStack_110 = *(undefined4 *)(puVar9 + 0x13);
    uStack_10c = *(undefined4 *)((longlong)puVar9 + 0x9c);
    local_108 = puVar9[0x14];
    param_3[0xc6] = (int)local_198;
    if ((iVar4 == 1) && ((char)param_3[1] == '\0')) {
      return;
    }
  }
  uVar8 = FUN_140002d40(param_2);
  FUN_140027140(param_3 + 0x44,uVar8,0x100);
  uVar8 = FUN_140002cf0(param_2);
  FUN_140027140(param_3 + 0x84,uVar8,0x104);
  iVar5 = thunk_FUN_140008cb0(param_2);
  param_3[199] = iVar5;
  *param_3 = iVar4;
  uVar3 = FUN_140002eb0(param_2);
  *(undefined1 *)(param_3 + 0xcc) = uVar3;
  if (*(longlong *)(param_3 + 0xfa) != 0) {
    if ((*param_4 < 0x18) && ((0x880004U >> (*param_4 & 0x1f) & 1) != 0)) {
      iVar5 = FUN_140002e80(param_2);
      param_3[0xc6] = iVar5 + *(int *)(*(longlong *)(param_3 + 0xfa) + 0x318);
      lVar10 = FUN_140002cd0(param_2);
      if (lVar10 != 0) {
        param_3[199] = param_4[199];
        param_3[0xc5] = 0xe;
        iVar5 = FUN_140002cd0(param_2);
        param_3[0xce] = iVar5;
        iVar5 = FUN_140002cb0(param_2);
        param_3[0xcf] = iVar5;
      }
    }
    if (((*param_3 == 2) && (*param_4 == 2)) && ((char)param_3[1] == '\0')) {
      FUN_140027140(param_3 + 1,"<Inherited>",0x100);
    }
  }
  if ((iVar4 - 0x13U & 0xfffffffb) == 0) {
    if ((char)param_3[1] == '\0') {
      FUN_140027140(param_3 + 1,"<Anonymous>",0x100);
    }
    if ((iVar4 - 0x13U & 0xfffffffb) != 0) goto LAB_140040743;
LAB_14004074d:
    uVar6 = FUN_140002e50(uVar7);
    uVar8 = local_1c0;
    uVar14 = 0;
    if (uVar6 != 0) {
      do {
        uVar11 = FUN_140002e30(uVar7,uVar14);
        uVar12 = FUN_14003fc40(param_3);
        local_1d0 = 0;
        local_1d8 = (int *)CONCAT44(local_1d8._4_4_,param_5 + 1);
        FUN_1400404c0(uVar8,uVar11,uVar12,param_3);
        uVar14 = uVar14 + 1;
        param_2 = local_1b8;
      } while (uVar14 < uVar6);
    }
    if (iVar4 == 2) {
      uVar6 = FUN_140002d80(uVar7);
      uVar14 = 0;
      if (uVar6 != 0) {
        do {
          uVar8 = FUN_140002d60(uVar7,uVar14);
          uVar11 = FUN_14003fc40(param_3);
          local_1d0 = 0;
          local_1d8 = (int *)CONCAT44(local_1d8._4_4_,param_5 + 1);
          FUN_1400404c0(local_1c0,uVar8,uVar11,param_3);
          uVar14 = uVar14 + 1;
        } while (uVar14 < uVar6);
      }
      goto LAB_140040908;
    }
  }
  else {
LAB_140040743:
    if (iVar4 == 2) goto LAB_14004074d;
  }
  uVar6 = 1;
  if (iVar4 == 1) {
    *(undefined1 *)(param_3 + 0xcd) = 1;
    puVar9 = (undefined8 *)FUN_140002c00(&local_1a8,uVar7);
    local_f8 = *puVar9;
    uStack_f0 = puVar9[1];
    local_e8 = puVar9[2];
    uVar2 = puVar9[3];
    local_b8 = puVar9[8];
    uStack_b0 = puVar9[9];
    local_1b8 = puVar9[4];
    uStack_1b0 = puVar9[5];
    local_d8[2] = puVar9[6];
    local_d8[3] = puVar9[7];
    local_98 = *(undefined4 *)(puVar9 + 0xc);
    uStack_94 = *(undefined4 *)((longlong)puVar9 + 100);
    uStack_90 = *(undefined4 *)(puVar9 + 0xd);
    uStack_8c = *(undefined4 *)((longlong)puVar9 + 0x6c);
    local_a8 = *(undefined4 *)(puVar9 + 10);
    uStack_a4 = *(undefined4 *)((longlong)puVar9 + 0x54);
    uStack_a0 = *(undefined4 *)(puVar9 + 0xb);
    uStack_9c = *(undefined4 *)((longlong)puVar9 + 0x5c);
    param_3[200] = (int)uVar2;
    uStack_e0 = uVar2;
    local_d8[0] = local_1b8;
    local_d8[1] = uStack_1b0;
    puVar13 = (undefined4 *)FUN_140043d20(0x10,"Node dimension size");
    *puVar13 = (undefined4)local_1b8;
    if (1 < uVar2) {
      uVar15 = 1;
      puVar16 = puVar13;
      do {
        puVar13 = (undefined4 *)FUN_140043d20(0x10,"Dimension size sibling");
        *(undefined4 **)(puVar16 + 2) = puVar13;
        uVar6 = uVar6 + 1;
        plVar1 = local_d8 + uVar15;
        uVar15 = (ulonglong)uVar6;
        *puVar13 = (int)*plVar1;
        puVar16 = puVar13;
      } while (uVar15 < uVar2);
    }
    *(undefined8 *)(puVar13 + 2) = 0;
    uVar7 = FUN_140002d20(uVar7,0);
    local_1d8 = param_3;
    local_1d0 = param_2;
    FUN_140040320(puVar13,0,local_1c0,uVar7);
  }
LAB_140040908:
  FUN_140040050(param_3);
  uVar7 = FUN_140040cd0(param_3);
  *(undefined8 *)(param_3 + 0x42) = uVar7;
  FUN_140041870(param_3);
  return;
}


/* ================= FUN_140040970 @ 140040970  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003ea40 */
/* CALLEES: FUN_140040bd0 */

void FUN_140040970(void)

{
  FUN_140040bd0(DAT_14286d3b0);
  return;
}


/* ================= FUN_140040980 @ 140040980  size=134  callers=4 callees=2 ================= */
/* CALLERS: FUN_14003cec0, FUN_14003d5c0, FUN_14003db70, FUN_14003de60 */
/* CALLEES: FUN_14004e120, memset */

void FUN_140040980(void)

{
  longlong lVar1;
  longlong *plVar2;
  
  for (plVar2 = DAT_14286d3b8; plVar2 != (longlong *)0x0; plVar2 = (longlong *)plVar2[1]) {
    lVar1 = *plVar2;
    if (*(void **)(lVar1 + 0x3d0) != (void *)0x0) {
      memset(*(void **)(lVar1 + 0x3d0),0,0x4000000);
    }
    *(undefined8 *)(lVar1 + 0x3c8) = 0;
    *(undefined1 *)(lVar1 + 0x388) = 0;
  }
  DAT_14285da80 = 0;
  DAT_14285da84 = 0;
  DAT_14285da78 = 0;
  DAT_14285da7c = 0;
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_140040a10 @ 140040a10  size=158  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003d210 */
/* CALLEES: FUN_1400429c0 */

void FUN_140040a10(void)

{
  longlong lVar1;
  longlong *plVar2;
  longlong lVar3;
  longlong *plVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined8 uVar8;
  int iVar9;
  
  plVar4 = DAT_14286d3b8;
  do {
    if (plVar4 == (longlong *)0x0) {
      return;
    }
    lVar1 = *plVar4;
    for (plVar2 = DAT_14285da08; plVar2 != (longlong *)0x0; plVar2 = (longlong *)plVar2[1]) {
      iVar9 = FUN_1400429c0(lVar1,*plVar2);
      if (iVar9 != 0) {
        uVar8 = *(undefined8 *)(lVar1 + 0x350);
        lVar3 = *plVar2;
        *(undefined8 *)(lVar3 + 0x348) = *(undefined8 *)(lVar1 + 0x348);
        *(undefined8 *)(lVar3 + 0x350) = uVar8;
        uVar8 = *(undefined8 *)(lVar1 + 0x360);
        *(undefined8 *)(lVar3 + 0x358) = *(undefined8 *)(lVar1 + 0x358);
        *(undefined8 *)(lVar3 + 0x360) = uVar8;
        uVar5 = *(undefined4 *)(lVar1 + 0x36c);
        uVar6 = *(undefined4 *)(lVar1 + 0x370);
        uVar7 = *(undefined4 *)(lVar1 + 0x374);
        *(undefined4 *)(lVar3 + 0x368) = *(undefined4 *)(lVar1 + 0x368);
        *(undefined4 *)(lVar3 + 0x36c) = uVar5;
        *(undefined4 *)(lVar3 + 0x370) = uVar6;
        *(undefined4 *)(lVar3 + 0x374) = uVar7;
        uVar8 = *(undefined8 *)(lVar1 + 0x380);
        *(undefined8 *)(lVar3 + 0x378) = *(undefined8 *)(lVar1 + 0x378);
        *(undefined8 *)(lVar3 + 0x380) = uVar8;
        break;
      }
    }
    plVar4 = (longlong *)plVar4[1];
  } while( true );
}


/* ================= FUN_140040ab0 @ 140040ab0  size=286  callers=4 callees=1 ================= */
/* CALLERS: FUN_14003d210, FUN_14003dfa0, FUN_14003fc40, FUN_14003fee0 */
/* CALLEES: FUN_140043d20 */

void FUN_140040ab0(void)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)FUN_140043d20(0x3f0);
  if (puVar1 != (undefined4 *)0x0) {
    *(undefined8 *)(puVar1 + 0xee) = 0;
    *(undefined8 *)(puVar1 + 0xf0) = 0;
    *(undefined8 *)(puVar1 + 0xe6) = 0xffefffffffffffff;
    *(undefined8 *)(puVar1 + 0xe8) = 0x7fefffffffffffff;
    *puVar1 = 0;
    *(undefined1 *)(puVar1 + 1) = 0;
    *(undefined1 *)(puVar1 + 0x44) = 0;
    *(undefined1 *)(puVar1 + 0x84) = 0;
    *(undefined8 *)(puVar1 + 0xc6) = 0;
    puVar1[200] = 0;
    *(undefined8 *)(puVar1 + 0xca) = 0;
    puVar1[0xcc] = 0;
    *(undefined1 *)(puVar1 + 0xcd) = 0;
    *(undefined8 *)(puVar1 + 0xce) = 0;
    *(undefined8 *)(puVar1 + 0x42) = 0;
    *(undefined1 *)(puVar1 + 0xd0) = 0;
    puVar1[0xd7] = 0x10000;
    *(undefined8 *)(puVar1 + 0xd8) = 0;
    *(undefined8 *)(puVar1 + 0xda) = 0;
    *(undefined8 *)(puVar1 + 0xde) = 0;
    *(undefined8 *)(puVar1 + 0xdc) = 0;
    puVar1[0xd2] = 0;
    *(undefined8 *)(puVar1 + 0xd4) = 0;
    puVar1[0xd6] = 1;
    *(undefined2 *)(puVar1 + 0xe0) = 0;
    *(undefined8 *)(puVar1 + 0xec) = 0;
    *(undefined1 *)(puVar1 + 0xe2) = 0;
    *(undefined8 *)(puVar1 + 0xe4) = 0;
    *(undefined8 *)(puVar1 + 0xea) = 0;
    puVar1[0xe3] = 0;
    *(undefined8 *)(puVar1 + 0xf2) = 0;
    *(undefined8 *)(puVar1 + 0xf4) = 0;
    *(undefined8 *)(puVar1 + 0xf6) = 0;
    *(undefined8 *)(puVar1 + 0xf8) = 0;
    *(undefined8 *)(puVar1 + 0xfa) = 0;
  }
  return;
}


/* ================= FUN_140040bd0 @ 140040bd0  size=245  callers=2 callees=5 ================= */
/* CALLERS: FUN_14003d210, FUN_140040970 */
/* CALLEES: FUN_14003f6a0, FUN_14003f6d0, FUN_14003f720, FUN_14003f760, FUN_140043e60 */

void FUN_140040bd0(longlong param_1)

{
  longlong lVar1;
  longlong lVar2;
  longlong lVar3;
  undefined4 *puVar4;
  longlong local_res8;
  undefined4 local_38;
  undefined4 uStack_34;
  int iStack_30;
  undefined4 uStack_2c;
  undefined8 local_28;
  undefined1 local_20 [24];
  
  if (param_1 != 0) {
    local_res8 = param_1;
    puVar4 = (undefined4 *)FUN_14003f6d0(local_20,8,800);
    local_38 = *puVar4;
    uStack_34 = puVar4[1];
    iStack_30 = puVar4[2];
    uStack_2c = puVar4[3];
    local_28 = *(undefined8 *)(puVar4 + 4);
    FUN_14003f760(&local_38,param_1);
    while (iStack_30 != 0) {
      FUN_14003f720(&local_38,&local_res8);
      lVar3 = local_res8;
      if (*(longlong *)(local_res8 + 0x3e0) != 0) {
        FUN_14003f760(&local_38);
      }
      if (*(longlong *)(lVar3 + 0x3d8) != 0) {
        FUN_14003f760(&local_38);
      }
      if (*(int *)(lVar3 + 800) != 0) {
        lVar2 = *(longlong *)(lVar3 + 0x328);
        while (lVar2 != 0) {
          lVar1 = *(longlong *)(lVar2 + 8);
          FUN_140043e60(lVar2);
          lVar2 = lVar1;
        }
      }
      FUN_140043e60(lVar3);
    }
    FUN_14003f6a0(&local_38);
  }
  return;
}


/* ================= FUN_140040cd0 @ 140040cd0  size=301  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_1400277b0, FUN_140043d10, memcpy, memset */

void * FUN_140040cd0(longlong param_1)

{
  longlong lVar1;
  int iVar2;
  void *_Dst;
  undefined1 *_Dst_00;
  uint uVar3;
  
  if (param_1 != 0) {
    iVar2 = FUN_1400277b0(param_1 + 4);
    uVar3 = iVar2 + 1;
    for (lVar1 = *(longlong *)(param_1 + 1000); lVar1 != 0; lVar1 = *(longlong *)(lVar1 + 1000)) {
      if ((*(char *)(lVar1 + 0x334) == '\0') && (*(char *)(lVar1 + 0x340) == '\0')) {
        iVar2 = FUN_1400277b0(lVar1 + 4);
        uVar3 = uVar3 + 1 + iVar2;
      }
    }
    _Dst = (void *)FUN_140043d10(uVar3);
    memset(_Dst,0,(ulonglong)uVar3);
    iVar2 = FUN_1400277b0(param_1 + 4);
    _Dst_00 = (undefined1 *)((ulonglong)((uVar3 - iVar2) - 1) + (longlong)_Dst);
    iVar2 = FUN_1400277b0(param_1 + 4);
    memcpy(_Dst_00,(void *)(param_1 + 4),(longlong)(iVar2 + 1));
    for (lVar1 = *(longlong *)(param_1 + 1000); lVar1 != 0; lVar1 = *(longlong *)(lVar1 + 1000)) {
      if ((*(char *)(lVar1 + 0x334) == '\0') && (*(char *)(lVar1 + 0x340) == '\0')) {
        _Dst_00[-1] = 0x2e;
        iVar2 = FUN_1400277b0(lVar1 + 4);
        _Dst_00 = _Dst_00 + -1 + -(longlong)iVar2;
        iVar2 = FUN_1400277b0(lVar1 + 4);
        memcpy(_Dst_00,(void *)(lVar1 + 4),(longlong)iVar2);
      }
    }
    return _Dst;
  }
  return (void *)0x0;
}


/* ================= FUN_140040e10 @ 140040e10  size=16  callers=3 callees=1 ================= */
/* CALLERS: FUN_140042ef0, FUN_140042f30, FUN_140042f60 */
/* CALLEES: FUN_140043e60 */

void FUN_140040e10(longlong param_1)

{
  if (*(longlong *)(param_1 + 8) != 0) {
    param_1 = *(longlong *)(param_1 + 8);
  }
  FUN_140043e60(param_1);
  return;
}


/* ================= FUN_140040e20 @ 140040e20  size=99  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003cc50 */
/* CALLEES: FUN_140044260, FUN_140044270, FUN_1400442a0, thunk_FUN_14003ecc0 */

undefined4 FUN_140040e20(void)

{
  undefined8 uVar1;
  longlong lVar2;
  undefined4 local_res8 [2];
  undefined4 local_res10 [6];
  
  local_res8[0] = 0;
  local_res10[0] = 4;
  uVar1 = thunk_FUN_14003ecc0();
  lVar2 = FUN_140044270(0x80000001,uVar1);
  if (lVar2 != 0) {
    FUN_1400442a0(lVar2,"DontShowAgainStartJScope",0,local_res8,local_res10);
    FUN_140044260(lVar2);
  }
  return local_res8[0];
}


/* ================= FUN_140040e90 @ 140040e90  size=53  callers=1 callees=1 ================= */
/* CALLERS: FUN_140040320 */
/* CALLEES: thunk_FUN_140008cb0 */

int FUN_140040e90(int *param_1,undefined8 param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = thunk_FUN_140008cb0(param_2);
  iVar2 = 1;
  do {
    iVar2 = iVar2 * *param_1;
    param_1 = *(int **)(param_1 + 2);
  } while (param_1 != (int *)0x0);
  return iVar2 * iVar1;
}


/* ================= FUN_140040ed0 @ 140040ed0  size=202  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400427b0 */
/* CALLEES: FUN_14002c8f0, JLINK_RTTERMINAL_Read, Sleep, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined4 FUN_140040ed0(longlong param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined1 auStack_148 [32];
  char local_128 [256];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_148;
  if (param_1 == 0) {
    uVar1 = 0xfffffffd;
  }
  else {
    uVar3 = 0;
    iVar4 = 0;
    do {
      uVar3 = uVar3 + 1;
      iVar2 = JLINK_RTTERMINAL_Read(DAT_14085d188,local_128,0x100);
      iVar4 = iVar4 + iVar2;
      if (iVar4 == 0x100) {
        if (local_128[0] != '\0') {
          FUN_14002c8f0(param_1,param_2,"Target Error: %s",local_128);
          return 0xffffffff;
        }
        return 0;
      }
      Sleep(1);
    } while (uVar3 < 0x65);
    uVar1 = 0xfffffffe;
  }
  return uVar1;
}


/* ================= FUN_140040fa0 @ 140040fa0  size=145  callers=1 callees=5 ================= */
/* CALLERS: FUN_140041710 */
/* CALLEES: FUN_14003fa70, FUN_140043d10, FUN_140043e60, FUN_14004e120, JLINK_HSS_Read */

int FUN_140040fa0(void)

{
  int iVar1;
  longlong lVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = 0;
  iVar4 = 0x200000 - (int)(0x200000 % (ulonglong)(uint)(DAT_14085d184 * DAT_14085d1e8));
  lVar2 = FUN_140043d10(iVar4);
  if (lVar2 == 0) {
    iVar1 = -1;
  }
  else {
    iVar1 = iVar3;
    if (((DAT_14085d180 != '\0') && (iVar1 = JLINK_HSS_Read(lVar2,iVar4), -1 < iVar1)) &&
       (FUN_14003fa70(iVar1,lVar2), iVar1 = iVar3, DAT_14285d20c != 0)) {
      FUN_14004e120(DAT_14286d388);
    }
  }
  FUN_140043e60(lVar2);
  return iVar1;
}


/* ================= FUN_140041040 @ 140041040  size=106  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400415a0 */
/* CALLEES: FUN_14004ed00 */

undefined8 FUN_140041040(longlong param_1,float *param_2,float *param_3)

{
  if ((((*(char *)(param_1 + 0x12) == '\0') || (*(double *)(param_1 + 8) <= (double)*param_2)) ||
      ((double)*param_3 < *(double *)(param_1 + 8))) &&
     (((*(char *)(param_1 + 0x13) == '\0' || ((double)*param_2 <= *(double *)(param_1 + 8))) ||
      (*(double *)(param_1 + 8) < (double)*param_3)))) {
    return 0;
  }
  *(undefined1 *)(param_1 + 0x14) = 1;
  FUN_14004ed00();
  return 1;
}


/* ================= FUN_1400410b0 @ 1400410b0  size=86  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400415a0 */
/* CALLEES: FUN_14004ed00 */

undefined8 FUN_1400410b0(longlong param_1,double *param_2,double *param_3)

{
  double dVar1;
  
  if (((*(char *)(param_1 + 0x12) != '\0') &&
      (dVar1 = *(double *)(param_1 + 8), *param_2 <= dVar1 && dVar1 != *param_2)) &&
     (dVar1 <= *param_3)) {
LAB_1400410ec:
    *(undefined1 *)(param_1 + 0x14) = 1;
    FUN_14004ed00();
    return 1;
  }
  if (*(char *)(param_1 + 0x13) != '\0') {
    if ((*(double *)(param_1 + 8) < *param_2) && (*param_3 <= *(double *)(param_1 + 8)))
    goto LAB_1400410ec;
  }
  return 0;
}


/* ================= FUN_140041110 @ 140041110  size=122  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400415a0 */
/* CALLEES: FUN_14004ed00 */

undefined8 FUN_140041110(longlong param_1,short *param_2,short *param_3)

{
  if ((((*(char *)(param_1 + 0x12) == '\0') || (*(double *)(param_1 + 8) <= (double)(int)*param_2))
      || ((double)(int)*param_3 < *(double *)(param_1 + 8))) &&
     (((*(char *)(param_1 + 0x13) == '\0' || ((double)(int)*param_2 <= *(double *)(param_1 + 8))) ||
      (*(double *)(param_1 + 8) < (double)(int)*param_3)))) {
    return 0;
  }
  *(undefined1 *)(param_1 + 0x14) = 1;
  FUN_14004ed00();
  return 1;
}


/* ================= FUN_140041190 @ 140041190  size=110  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400415a0 */
/* CALLEES: FUN_14004ed00 */

undefined8 FUN_140041190(longlong param_1,int *param_2,int *param_3)

{
  if ((((*(char *)(param_1 + 0x12) == '\0') || (*(double *)(param_1 + 8) <= (double)*param_2)) ||
      ((double)*param_3 < *(double *)(param_1 + 8))) &&
     (((*(char *)(param_1 + 0x13) == '\0' || ((double)*param_2 <= *(double *)(param_1 + 8))) ||
      (*(double *)(param_1 + 8) < (double)*param_3)))) {
    return 0;
  }
  *(undefined1 *)(param_1 + 0x14) = 1;
  FUN_14004ed00();
  return 1;
}


/* ================= FUN_140041200 @ 140041200  size=122  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400415a0 */
/* CALLEES: FUN_14004ed00 */

undefined8 FUN_140041200(longlong param_1,char *param_2,char *param_3)

{
  if ((((*(char *)(param_1 + 0x12) == '\0') || (*(double *)(param_1 + 8) <= (double)(int)*param_2))
      || ((double)(int)*param_3 < *(double *)(param_1 + 8))) &&
     (((*(char *)(param_1 + 0x13) == '\0' || ((double)(int)*param_2 <= *(double *)(param_1 + 8))) ||
      (*(double *)(param_1 + 8) < (double)(int)*param_3)))) {
    return 0;
  }
  *(undefined1 *)(param_1 + 0x14) = 1;
  FUN_14004ed00();
  return 1;
}


/* ================= FUN_140041280 @ 140041280  size=122  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400415a0 */
/* CALLEES: FUN_14004ed00 */

undefined8 FUN_140041280(longlong param_1,ushort *param_2,ushort *param_3)

{
  if ((((*(char *)(param_1 + 0x12) == '\0') || (*(double *)(param_1 + 8) <= (double)*param_2)) ||
      ((double)*param_3 < *(double *)(param_1 + 8))) &&
     (((*(char *)(param_1 + 0x13) == '\0' || ((double)*param_2 <= *(double *)(param_1 + 8))) ||
      (*(double *)(param_1 + 8) < (double)*param_3)))) {
    return 0;
  }
  *(undefined1 *)(param_1 + 0x14) = 1;
  FUN_14004ed00();
  return 1;
}


/* ================= FUN_140041300 @ 140041300  size=118  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400415a0 */
/* CALLEES: FUN_14004ed00 */

undefined8 FUN_140041300(longlong param_1,uint *param_2,uint *param_3)

{
  if ((((*(char *)(param_1 + 0x12) == '\0') || (*(double *)(param_1 + 8) <= (double)*param_2)) ||
      ((double)*param_3 < *(double *)(param_1 + 8))) &&
     (((*(char *)(param_1 + 0x13) == '\0' || ((double)*param_2 <= *(double *)(param_1 + 8))) ||
      (*(double *)(param_1 + 8) < (double)*param_3)))) {
    return 0;
  }
  *(undefined1 *)(param_1 + 0x14) = 1;
  FUN_14004ed00();
  return 1;
}


/* ================= FUN_140041380 @ 140041380  size=122  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400415a0 */
/* CALLEES: FUN_14004ed00 */

undefined8 FUN_140041380(longlong param_1,byte *param_2,byte *param_3)

{
  if ((((*(char *)(param_1 + 0x12) == '\0') || (*(double *)(param_1 + 8) <= (double)*param_2)) ||
      ((double)*param_3 < *(double *)(param_1 + 8))) &&
     (((*(char *)(param_1 + 0x13) == '\0' || ((double)*param_2 <= *(double *)(param_1 + 8))) ||
      (*(double *)(param_1 + 8) < (double)*param_3)))) {
    return 0;
  }
  *(undefined1 *)(param_1 + 0x14) = 1;
  FUN_14004ed00();
  return 1;
}


/* ================= FUN_140041400 @ 140041400  size=97  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003ef60 */
/* CALLEES: FUN_140028380, FUN_14003f490, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140041400(void)

{
  undefined1 auStack_838 [32];
  undefined1 local_818 [2048];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_838;
  if (DAT_14285da8e == '\0') {
    FUN_140028380(local_818,0x800,
                  "RTT channel is set up to provide no timestamp!\nWithout timestamp J-Scope will assume that data points are uniformly distributed with a gap of 100 [us] each.\nTo set up the application for timestamps, please see the SEGGER wiki:\nhttps://kb.segger.com/UM08028_J-Scope#RTT:_Instrumenting_an_application_to_use_it_with_J-Scope"
                 );
    FUN_14003f490(local_818);
    DAT_14285da8e = '\x01';
  }
  return;
}


/* ================= FUN_140041470 @ 140041470  size=199  callers=4 callees=2 ================= */
/* CALLERS: FUN_14003ea60, FUN_14003ea80, FUN_14003ec90, FUN_14003ecc0 */
/* CALLEES: FUN_1400277b0, FUN_140028380 */

void FUN_140041470(void)

{
  if (DAT_14285da98 == '\0') {
    FUN_140028380(&DAT_14285da98,0x10,"V%d.%.2d",9,0x42);
    FUN_1400277b0(&DAT_14285da98);
    FUN_140028380(&DAT_14285dab0,0x40,"SEGGER %s %s%s","J-Scope",&DAT_14285da98,&DAT_14005f306);
    FUN_140028380(&DAT_14285daf0,0x40,"%s %s","J-Scope",&DAT_14285da98);
    FUN_140028380(&DAT_14285db30,0x40,"Software\\SEGGER\\%s","J-Scope");
    return;
  }
  return;
}


/* ================= FUN_140041540 @ 140041540  size=33  callers=2 callees=0 ================= */
/* CALLERS: FUN_14003fa70, FUN_140042260 */

undefined8 FUN_140041540(void)

{
  undefined8 uVar1;
  
  uVar1 = 0;
  if (((DAT_14285da80 == 0) && (DAT_14285da78 == 0)) && (uVar1 = 0, DAT_14285da7c == 0)) {
    uVar1 = 1;
  }
  return uVar1;
}


/* ================= FUN_140041570 @ 140041570  size=34  callers=5 callees=1 ================= */
/* CALLERS: FUN_14003e9a0, FUN_14003f1a0, FUN_14003f1e0, FUN_14003f830, FUN_1400423e0 */
/* CALLEES: FUN_14004f8c0 */

undefined8 FUN_140041570(int param_1)

{
  int iVar1;
  undefined8 uVar2;
  
  iVar1 = FUN_14004f8c0();
  uVar2 = 0;
  if ((param_1 <= iVar1) || (iVar1 == 9)) {
    uVar2 = 1;
  }
  return uVar2;
}


/* ================= FUN_1400415a0 @ 1400415a0  size=306  callers=1 callees=8 ================= */
/* CALLERS: FUN_14003fa70 */
/* CALLEES: FUN_140041040, FUN_1400410b0, FUN_140041110, FUN_140041190, FUN_140041200, FUN_140041280, FUN_140041300, FUN_140041380 */

ulonglong FUN_1400415a0(longlong *param_1,longlong param_2,longlong param_3,longlong param_4)

{
  uint uVar1;
  ulonglong uVar2;
  
  uVar2 = 0;
  if ((param_3 != 0) && (param_4 != 0)) {
    if ((char)param_1[2] == '\0') {
      return 2;
    }
    if (*(char *)((longlong)param_1 + 0x14) == '\x01') {
      return 3;
    }
    if (*param_1 == param_2) {
      uVar2 = 0;
      switch(*(undefined4 *)(param_2 + 0x314)) {
      case 1:
        uVar2 = FUN_140041040(param_1,param_3,param_4);
        return uVar2;
      case 2:
        uVar2 = FUN_1400410b0(param_1,param_3,param_4);
        return uVar2;
      case 6:
        uVar2 = FUN_140041380(param_1,param_3,param_4);
        return uVar2;
      case 7:
        uVar2 = FUN_140041280(param_1,param_3,param_4);
        return uVar2;
      case 8:
        uVar2 = FUN_140041300(param_1,param_3,param_4);
        return uVar2;
      case 10:
        uVar2 = FUN_140041200(param_1,param_3,param_4);
        return uVar2;
      case 0xb:
        uVar2 = FUN_140041110(param_1,param_3,param_4);
        return uVar2;
      case 0xc:
        uVar1 = FUN_140041190(param_1,param_3,param_4);
        uVar2 = (ulonglong)uVar1;
      }
    }
  }
  return uVar2;
}


/* ================= FUN_140041710 @ 140041710  size=101  callers=0 callees=6 ================= */
/* CALLEES: FUN_14003e680, FUN_140040fa0, FUN_140042260, FUN_140043970, FUN_140043990, FUN_14004e9c0 */

undefined8 FUN_140041710(void)

{
  int iVar1;
  ulonglong uVar2;
  
  FUN_14004e9c0(&DAT_14285d420);
  if (DAT_14285d9c4 == '\0') {
    iVar1 = FUN_140040fa0();
  }
  else {
    iVar1 = FUN_140042260();
  }
  if (iVar1 < 0) {
    FUN_14003e680();
  }
  uVar2 = FUN_140043970(DAT_14285d1f0);
  DAT_14285d1f8 = (undefined4)((uint)(DAT_14085d1ec * 10000000) / uVar2);
  DAT_14285d1f0 = FUN_140043990();
  return 0;
}


/* ================= FUN_140041780 @ 140041780  size=226  callers=1 callees=10 ================= */
/* CALLERS: FUN_14003cc50 */
/* CALLEES: FUN_140027140, FUN_1400271f0, FUN_140027700, FUN_140028380, JLINKARM_GetCompileDateTime, JLINKARM_GetDLLVersion, __security_check_cookie, _guard_check_icall, thunk_FUN_14003ea60, thunk_FUN_14003ec90 */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140041780(void)

{
  undefined4 uVar1;
  undefined8 uVar2;
  undefined1 auStack_e8 [32];
  undefined4 local_c8;
  undefined1 *local_c0;
  undefined8 local_b8;
  undefined1 local_a8 [4];
  undefined4 local_a4;
  undefined4 local_a0;
  undefined1 local_98 [128];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_e8;
  uVar2 = thunk_FUN_14003ec90();
  FUN_140027140(local_98,uVar2,0x80);
  FUN_140027700(local_98,&DAT_140060de4,0x80);
  uVar2 = thunk_FUN_14003ea60();
  FUN_140027700(local_98,uVar2,0x80);
  _guard_check_icall(DAT_14286d388,local_98);
  uVar1 = JLINKARM_GetDLLVersion();
  FUN_1400271f0(local_a8,uVar1);
  local_b8 = JLINKARM_GetCompileDateTime();
  local_c0 = local_a8;
  local_c8 = local_a0;
  FUN_140028380(local_98,0x80,"JLinkARM.dll V%d.%.2d%s (DLL compiled %s)",local_a4);
  _guard_check_icall(DAT_14286d388,local_98);
  return;
}


/* ================= FUN_140041870 @ 140041870  size=86  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_14003ffd0, FUN_1400429c0 */

void FUN_140041870(undefined8 param_1)

{
  int iVar1;
  undefined8 *puVar2;
  
  puVar2 = DAT_14286d3b8;
  if (DAT_14286d3b8 != (undefined8 *)0x0) {
    while( true ) {
      iVar1 = FUN_1400429c0(*puVar2,param_1);
      if (iVar1 != 0) break;
      puVar2 = (undefined8 *)puVar2[1];
      if (puVar2 == (undefined8 *)0x0) {
        return;
      }
    }
    FUN_14003ffd0(param_1);
  }
  return;
}


/* ================= FUN_1400418d0 @ 1400418d0  size=17  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003db70 */

undefined8 FUN_1400418d0(int param_1,int param_2)

{
  if ((param_1 != 0) && (param_2 == 0)) {
    return 0;
  }
  return 1;
}


/* ================= FUN_1400418f0 @ 1400418f0  size=251  callers=1 callees=7 ================= */
/* CALLERS: FUN_14003ef60 */
/* CALLEES: FUN_140028ec0, FUN_14003f110, FUN_14003f120, JLINKARM_EMU_SelectByUSBSN, JLINKARM_OpenEx, JLINKARM_SelectIP, JLINK_EMU_SelectByUSBNickname */

undefined8 FUN_1400418f0(longlong param_1)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  
  iVar2 = FUN_140028ec0();
  if (iVar2 < 0) {
    FUN_14003f110("Failed to load J-Link library.");
    return 0xffffffff;
  }
  if (*(int *)(param_1 + 0x5ac) == 2) {
    cVar1 = JLINKARM_SelectIP(param_1 + 0x490,0);
    if (cVar1 != '\0') {
      FUN_14003f120("Failed to select J-Link via IP (\'%s\').",param_1 + 0x490);
      return 0xffffffff;
    }
  }
  else if (*(int *)(param_1 + 0x594) == 0) {
    if (*(char *)(param_1 + 0x510) != '\0') {
      iVar2 = JLINK_EMU_SelectByUSBNickname(param_1 + 0x510);
      if (iVar2 < 0) {
        FUN_14003f120("Failed to select J-Link with nickname (\'%s\') via USB.",param_1 + 0x510);
        return 0xffffffff;
      }
    }
  }
  else {
    iVar2 = JLINKARM_EMU_SelectByUSBSN();
    if (iVar2 < 0) {
      FUN_14003f120("Failed to select J-Link with serial number (\'%d\') via USB.",
                    *(undefined4 *)(param_1 + 0x594));
      return 0xffffffff;
    }
  }
  lVar3 = JLINKARM_OpenEx(0,0);
  if (lVar3 != 0) {
    FUN_14003f120("Failed to connect to J-Link: %s",lVar3);
    return 0xffffffff;
  }
  return 0;
}


/* ================= FUN_1400419f0 @ 1400419f0  size=725  callers=1 callees=21 ================= */
/* CALLERS: FUN_14003ef60 */
/* CALLEES: FUN_140027760, FUN_140028380, FUN_14003f110, FUN_14003f120, FUN_14003f280, FUN_14003f400, FUN_14004ec40, JLINKARM_ConfigJTAG, JLINKARM_Connect, JLINKARM_DEVICE_GetInfo, JLINKARM_DEVICE_SelectDialog, JLINKARM_EMU_HasCapEx, JLINKARM_ExecCommand, JLINKARM_GetDLLVersion, JLINKARM_SetSpeed, JLINKARM_TIF_Select, JLINK_ExecCommand, JLINK_HSS_GetCaps, __chkstk, __security_check_cookie, memset */

/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_1400419f0(char *param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  undefined8 uVar6;
  undefined1 auStack_11d8 [32];
  undefined4 local_11b8;
  undefined1 local_11b4 [4];
  undefined8 *local_11b0;
  undefined1 local_838 [4];
  uint local_834;
  char local_818 [1024];
  undefined1 local_418 [1024];
  ulonglong local_18;
  undefined8 uStack_10;
  
  uStack_10 = 0x1400419fc;
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_11d8;
  memset(local_818,0,0x400);
  if (((*(int *)(param_1 + 0x59c) == 0) || (*(int *)(param_1 + 0x59c) == 7)) &&
     (-1 < *(int *)(param_1 + 0x5b0))) {
    JLINKARM_ConfigJTAG(*(undefined4 *)(param_1 + 0x5b4));
  }
  JLINKARM_ExecCommand("SetSkipInitECCRAMOnConnect = 1",0,0);
  if ((*param_1 == '\0') || (iVar4 = FUN_140027760(param_1), iVar4 == 0)) {
    iVar4 = JLINKARM_DEVICE_SelectDialog(DAT_14286d388,0,0);
    if (iVar4 == -1) {
      FUN_14003f280("User cancelled device selection.");
      return 0xffffffff;
    }
    memset(local_11b4,0,0x974);
    local_11b8 = 0x978;
    JLINKARM_DEVICE_GetInfo(iVar4,&local_11b8);
    uVar6 = local_11b0[1];
    *(undefined8 *)param_1 = *local_11b0;
    *(undefined8 *)(param_1 + 8) = uVar6;
    uVar6 = local_11b0[3];
    *(undefined8 *)(param_1 + 0x10) = local_11b0[2];
    *(undefined8 *)(param_1 + 0x18) = uVar6;
    uVar6 = local_11b0[5];
    *(undefined8 *)(param_1 + 0x20) = local_11b0[4];
    *(undefined8 *)(param_1 + 0x28) = uVar6;
    uVar6 = local_11b0[7];
    *(undefined8 *)(param_1 + 0x30) = local_11b0[6];
    *(undefined8 *)(param_1 + 0x38) = uVar6;
    uVar6 = local_11b0[9];
    *(undefined8 *)(param_1 + 0x40) = local_11b0[8];
    *(undefined8 *)(param_1 + 0x48) = uVar6;
    uVar6 = local_11b0[0xb];
    *(undefined8 *)(param_1 + 0x50) = local_11b0[10];
    *(undefined8 *)(param_1 + 0x58) = uVar6;
    uVar1 = *(undefined4 *)((longlong)local_11b0 + 100);
    uVar2 = *(undefined4 *)(local_11b0 + 0xd);
    uVar3 = *(undefined4 *)((longlong)local_11b0 + 0x6c);
    *(undefined4 *)(param_1 + 0x60) = *(undefined4 *)(local_11b0 + 0xc);
    *(undefined4 *)(param_1 + 100) = uVar1;
    *(undefined4 *)(param_1 + 0x68) = uVar2;
    *(undefined4 *)(param_1 + 0x6c) = uVar3;
    uVar1 = *(undefined4 *)((longlong)local_11b0 + 0x74);
    uVar2 = *(undefined4 *)(local_11b0 + 0xf);
    uVar3 = *(undefined4 *)((longlong)local_11b0 + 0x7c);
    *(undefined4 *)(param_1 + 0x70) = *(undefined4 *)(local_11b0 + 0xe);
    *(undefined4 *)(param_1 + 0x74) = uVar1;
    *(undefined4 *)(param_1 + 0x78) = uVar2;
    *(undefined4 *)(param_1 + 0x7c) = uVar3;
    FUN_14004ec40(param_1);
  }
  FUN_140028380(local_418,0x400,"device = %s",param_1);
  JLINKARM_ExecCommand(local_418,local_818,0x400);
  if (local_818[0] != '\0') {
    FUN_14003f120("Failed to select target device \'%s\'.",param_1);
    return 0xffffffff;
  }
  if (param_1[0x38c] != '\0') {
    FUN_140028380(local_418,0x400,"ScriptFile = %s",param_1 + 0x38c);
    JLINKARM_ExecCommand(local_418,local_818,0x400);
    if (local_818[0] != '\0') {
      FUN_14003f120("Failed to select script file \'%s\'.",param_1 + 0x38c);
      return 0xffffffff;
    }
  }
  JLINKARM_TIF_Select(*(undefined4 *)(param_1 + 0x59c));
  JLINKARM_SetSpeed(*(undefined4 *)(param_1 + 0x5a0));
  if ((DAT_14285d210 != '\0') && (iVar4 = JLINK_ExecCommand(&DAT_14285d210,0,0), iVar4 != 0)) {
    FUN_14003f110("Wrong syntax for \"-RTTSearchRange\".\n");
    return 0xffffffff;
  }
  iVar4 = JLINKARM_Connect();
  if (iVar4 < 0) {
    FUN_14003f110("Failed to establish connection to target.");
    return 0xffffffff;
  }
  if (param_1[0x5a4] == '\0') {
    iVar4 = JLINKARM_EMU_HasCapEx(0x35);
    if (iVar4 == 0) {
      uVar6 = 20000;
    }
    else {
      uVar5 = JLINKARM_GetDLLVersion();
      if (0xc79b < uVar5) {
        iVar4 = JLINK_HSS_GetCaps(local_838);
        if (iVar4 != 0) {
          return 0;
        }
        _DAT_14285da90 = (int)(1000000 / (ulonglong)local_834);
        return 0;
      }
      uVar6 = 10;
    }
    FUN_14003f400(uVar6);
  }
  return 0;
}


/* ================= FUN_140041cd0 @ 140041cd0  size=601  callers=1 callees=10 ================= */
/* CALLERS: FUN_14003cc50 */
/* CALLEES: FUN_140027760, FUN_1400277b0, FUN_1400277f0, FUN_140028380, FUN_14003dc80, FUN_14003f1a0, FUN_14003f640, FUN_1400434e0, JLINKARM_ExecCommand, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined4 FUN_140041cd0(uint param_1,longlong param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  undefined **ppuVar3;
  ulonglong uVar4;
  ulonglong uVar5;
  char *pcVar6;
  char *pcVar7;
  uint uVar8;
  int iVar9;
  undefined4 uVar10;
  undefined1 auStack_478 [32];
  undefined4 local_458;
  uint local_454;
  undefined1 local_448 [1024];
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_478;
  DAT_14285da1f = 1;
  local_458 = 0;
  local_454 = param_1;
  JLINKARM_ExecCommand("SuppressGUI = 1",0,0);
  uVar4 = 1;
  uVar10 = 0;
  if (1 < param_1) {
    do {
      ppuVar3 = &PTR_s__openprj_140074040;
      uVar5 = (ulonglong)((int)uVar4 + 1);
      uVar8 = 0;
      pcVar6 = *(char **)(param_2 + uVar4 * 8);
      iVar9 = 1;
      do {
        if (*ppuVar3 == (undefined *)0x0) break;
        if ((*pcVar6 - 0x2dU & 0xfd) != 0) {
          FUN_1400434e0(pcVar6,local_448,0x400);
          iVar1 = FUN_140027760(local_448,".jscope");
          if (iVar1 == 0) {
            iVar9 = FUN_14003dc80(pcVar6,0,param_3);
            if (iVar9 == 0) goto LAB_140041db6;
            goto LAB_140041e9b;
          }
          break;
        }
        if ((*(uint *)(ppuVar3 + 1) & param_3) == 0) {
          uVar5 = (ulonglong)((int)uVar5 + 1);
          iVar9 = 0;
        }
        else {
          iVar1 = FUN_1400277b0();
          iVar2 = FUN_1400277f0(pcVar6,*ppuVar3,(longlong)iVar1);
          if (iVar2 == 0) {
            pcVar7 = *(char **)(param_2 + uVar5 * 8);
            if ((pcVar7 == (char *)0x0) || ((*pcVar7 - 0x2dU & 0xfd) == 0)) {
              pcVar7 = pcVar6 + iVar1;
            }
            else {
              uVar5 = (ulonglong)((int)uVar5 + 1);
            }
            iVar1 = (*(code *)ppuVar3[2])(pcVar7,iVar1,param_3);
            if (iVar1 == 0) goto LAB_140041db6;
            pcVar6 = pcVar7;
            if ((iVar1 < 0) || (DAT_14285da1c != '\0')) {
              DAT_14285da1f = 0;
              goto LAB_140041e9b;
            }
            if (iVar1 == 1) {
              return 1;
            }
            break;
          }
        }
        uVar8 = uVar8 + 1;
        ppuVar3 = ppuVar3 + 4;
      } while (uVar8 < 4);
      if (iVar9 == 1) {
        FUN_140028380(local_448,0x400,"CMDLINE: Unknown command line option \"%s\" detected!",pcVar6
                     );
        FUN_14003f1a0(local_448);
        FUN_14003f640(local_448);
        DAT_14285da1c = '\x01';
        local_458 = 0xffffffff;
      }
      else {
        if (iVar9 == 2) {
LAB_140041e9b:
          FUN_140028380(local_448,0x400,"CMDLINE: Failed to execute command line option \"%s\"!",
                        pcVar6);
          FUN_14003f1a0(local_448);
          FUN_14003f640(local_448);
          DAT_14285da1c = 1;
          return 0xffffffff;
        }
        if (iVar9 == 3) {
          return 1;
        }
      }
LAB_140041db6:
      uVar4 = uVar5;
      uVar10 = local_458;
    } while ((uint)uVar5 < local_454);
  }
  JLINKARM_ExecCommand("SuppressGUI = 0",0,0);
  DAT_14285da1f = 0;
  return uVar10;
}


/* ================= FUN_140041f30 @ 140041f30  size=457  callers=1 callees=8 ================= */
/* CALLERS: FUN_14003ef60 */
/* CALLEES: FUN_140028380, FUN_14003f110, FUN_140043d10, JLINK_Go, JLINK_HSS_Start, JLINK_IsHalted, __security_check_cookie, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

int FUN_140041f30(longlong param_1)

{
  longlong lVar1;
  longlong *plVar2;
  char cVar3;
  int iVar4;
  undefined4 *_Dst;
  uint uVar5;
  undefined4 *puVar6;
  char *pcVar7;
  undefined1 auStack_b8 [32];
  undefined1 local_98 [128];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_b8;
  DAT_14085d181 = 0;
  uVar5 = (DAT_14085d1e8 + 1) * 0x10;
  DAT_14085d180 = 0;
  _Dst = (undefined4 *)FUN_140043d10(uVar5);
  DAT_14285da70 = _Dst;
  memset(_Dst,0,(ulonglong)uVar5);
  DAT_14285da80 = 0;
  DAT_14085d184 = 4;
  puVar6 = _Dst;
  iVar4 = DAT_14085d1e8;
  for (plVar2 = DAT_14286d3b8; DAT_14085d1e8 = iVar4, plVar2 != (longlong *)0x0;
      plVar2 = (longlong *)plVar2[1]) {
    lVar1 = *plVar2;
    *(int *)(lVar1 + 0x38c) = DAT_14085d184;
    DAT_14085d184 = DAT_14085d184 + *(int *)(lVar1 + 0x31c);
    *puVar6 = *(undefined4 *)(lVar1 + 0x318);
    puVar6[1] = *(undefined4 *)(lVar1 + 0x31c);
    *(undefined8 *)(lVar1 + 0x3d0) = 0;
    puVar6 = puVar6 + 4;
    *(undefined8 *)(lVar1 + 0x3c8) = 0;
    *(undefined8 *)(lVar1 + 0x390) = 0;
    *(undefined8 *)(lVar1 + 0x3a8) = 0;
    *(undefined8 *)(lVar1 + 0x398) = 0xffefffffffffffff;
    *(undefined8 *)(lVar1 + 0x3a0) = 0x7fefffffffffffff;
    *(undefined1 *)(lVar1 + 0x388) = 0;
    iVar4 = DAT_14085d1e8;
  }
  puVar6[1] = 0;
  iVar4 = JLINK_HSS_Start(_Dst,iVar4,*(undefined4 *)(param_1 + 0x5a8),1);
  if (iVar4 < 0) {
    if (iVar4 == -4) {
      pcVar7 = "Target hardware does not support HSS";
    }
    else if (iVar4 == -3) {
      pcVar7 = "Too many variables specified";
    }
    else {
      if (iVar4 != -2) {
        FUN_14003f110("Failed to inititalize HSS.");
        return iVar4;
      }
      pcVar7 = "Failed to allocate memory on debug probe";
    }
    FUN_140028380(local_98,0x80,"Failed to initialize HSS: %s.",pcVar7);
    FUN_14003f110(local_98);
  }
  else {
    DAT_14085d180 = 1;
    cVar3 = JLINK_IsHalted();
    if (cVar3 != '\0') {
      JLINK_Go();
    }
    iVar4 = 0;
  }
  return iVar4;
}


/* ================= FUN_140042100 @ 140042100  size=337  callers=2 callees=9 ================= */
/* CALLERS: FUN_14003ef60, FUN_1400427b0 */
/* CALLEES: FUN_140027760, FUN_14003f110, FUN_14003fc90, FUN_1400424f0, FUN_140042a60, FUN_14004dc70, FUN_14004e120, FUN_14004ec40, JLINK_RTTERMINAL_Control */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_140042100(longlong param_1)

{
  int iVar1;
  undefined8 uVar2;
  
  *(undefined1 *)(param_1 + 0x5a5) = 0;
  DAT_14285da88 = 0;
  iVar1 = JLINK_RTTERMINAL_Control(0,0);
  if (iVar1 < 0) {
LAB_14004216d:
    uVar2 = 0xffffffff;
  }
  else {
    _DAT_14085d18c = 0;
    uVar2 = 1;
    if (*(char *)(param_1 + 0x5a6) == '\0') {
      iVar1 = FUN_1400424f0("JScope_",7,&DAT_14085d188);
      if (iVar1 < 0) {
        FUN_14003f110(
                     "Failed to start RTT Sampling.\n\nNo RTT Up-Channel named \"JScope_%Format%\" found"
                     );
        goto LAB_14004216d;
      }
      FUN_14003fc90(param_1);
      if (DAT_14085d1e8 == 0) {
        FUN_14003f110(
                     "Failed to start RTT Sampling.\n\nThe first RTT Channel named JScope_... found did not contain a valid VarFormat"
                     );
        uVar2 = 0xffffffff;
        goto LAB_1400421bb;
      }
      FUN_14004dc70(DAT_14286d388);
      FUN_14004e120(DAT_14286d388);
    }
    else {
      iVar1 = FUN_1400424f0("JScope_Data",0xb,&DAT_14085d188);
      if (iVar1 < 0) {
        FUN_14003f110(
                     "Failed to start RTT Sampling.\n\nNo RTT Up-Channel named \"JScope_Data\" found"
                     );
        uVar2 = 0xffffffff;
        goto LAB_1400421bb;
      }
      iVar1 = FUN_140027760(&DAT_14085d190,"JScope_Data_t");
      if (iVar1 == 0) {
        *(undefined1 *)(param_1 + 0x5a5) = 1;
      }
      _DAT_14085d1bc = 1;
      iVar1 = FUN_1400424f0("JScope_Config",0xe,&DAT_14085d1b8);
      if (iVar1 < 0) {
        FUN_14003f110(
                     "Failed to start RTT Sampling.\n\nNo RTT Down-Channel named \"JScope_Config\" found"
                     );
        uVar2 = 0xffffffff;
        goto LAB_1400421bb;
      }
    }
    FUN_140042a60();
    DAT_14285da88 = 1;
  }
LAB_1400421bb:
  FUN_14004ec40(param_1);
  return uVar2;
}


/* ================= FUN_140042260 @ 140042260  size=313  callers=1 callees=5 ================= */
/* CALLERS: FUN_140041710 */
/* CALLEES: FUN_14003fa70, FUN_140041540, FUN_14004e120, JLINK_RTTERMINAL_Read, memcpy */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

size_t FUN_140042260(void)

{
  uint uVar1;
  void *pvVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  size_t sVar6;
  size_t _Size;
  
  sVar6 = 0;
  if (DAT_14085d183 != '\0') {
    iVar3 = FUN_140041540();
    pvVar2 = DAT_14285d200;
    if (iVar3 == 0) {
      _Size = (size_t)DAT_14285da24;
    }
    else {
      DAT_14285da24 = 0;
      _DAT_14285da30 = 0;
      uRam000000014285da38 = 0;
      _DAT_14285da40 = 0;
      uRam000000014285da48 = 0;
      _DAT_14285da50 = 0;
      uRam000000014285da58 = 0;
      _DAT_14285da60 = 0;
      uRam000000014285da68 = 0;
      _Size = sVar6;
    }
    uVar4 = DAT_14285d1fc;
    if (0x200000 < DAT_14285d1fc) {
      uVar4 = 0x200000;
    }
    memcpy(DAT_14285d200,&DAT_14285da30,_Size);
    iVar3 = JLINK_RTTERMINAL_Read(DAT_14085d188,_Size + (longlong)pvVar2,uVar4 - (int)_Size);
    pvVar2 = DAT_14285d200;
    if (iVar3 < 0) {
      sVar6 = 0xffffffff;
    }
    else {
      uVar4 = iVar3 + (int)_Size;
      if (0 < (int)uVar4) {
        uVar5 = uVar4 % DAT_14085d184;
        uVar1 = uVar4 - uVar5;
        if (uVar5 == 0) {
          uVar1 = uVar4;
        }
        FUN_14003fa70(uVar1,DAT_14285d200);
        if (uVar5 == 0) {
          DAT_14285da24 = 0;
        }
        else {
          if (0x40 < uVar5) {
            uVar5 = 0x40;
          }
          memcpy(&DAT_14285da30,(void *)((longlong)pvVar2 + (longlong)(int)uVar1),(ulonglong)uVar5);
          DAT_14285da24 = uVar5;
        }
      }
    }
  }
  FUN_14004e120(DAT_14286d388);
  return sVar6;
}


/* ================= FUN_1400423a0 @ 1400423a0  size=64  callers=1 callees=1 ================= */
/* CALLERS: FUN_140040050 */
/* CALLEES: FUN_1400277b0 */

char * FUN_1400423a0(longlong param_1)

{
  int iVar1;
  char *pcVar2;
  char *pcVar3;
  uint uVar4;
  ulonglong uVar5;
  
  iVar1 = FUN_1400277b0();
  uVar5 = (ulonglong)iVar1;
  pcVar3 = (char *)(param_1 + uVar5);
  if (iVar1 != 0) {
    while( true ) {
      uVar4 = (int)uVar5 - 1;
      uVar5 = (ulonglong)uVar4;
      pcVar2 = pcVar3 + -1;
      if (*pcVar2 == ' ') break;
      pcVar3 = pcVar2;
      if (uVar4 == 0) {
        return pcVar2;
      }
    }
  }
  return pcVar3;
}


/* ================= FUN_1400423e0 @ 1400423e0  size=271  callers=1 callees=7 ================= */
/* CALLERS: FUN_14003f110 */
/* CALLEES: FUN_140027700, FUN_1400277b0, FUN_140028380, FUN_140041570, FUN_1400591dc, __security_check_cookie, _guard_check_icall */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_1400423e0(longlong param_1,int param_2)

{
  char cVar1;
  int iVar2;
  ulonglong uVar3;
  undefined1 auStack_838 [32];
  char local_818 [2048];
  ulonglong local_18;
  
  if (param_1 != 0) {
    local_18 = DAT_140096fe8 ^ (ulonglong)auStack_838;
    cVar1 = FUN_140041570(DAT_14285eb8c * 2 + -1);
    if ((cVar1 != '\0') || (param_2 != 0)) {
      FUN_140028380(local_818,0x800,"ERROR: %s",param_1);
      iVar2 = FUN_1400277b0(local_818);
      uVar3 = (ulonglong)(iVar2 + -1);
      if (local_818[uVar3] == '\n') {
        if (0x7ff < uVar3) {
                    /* WARNING: Subroutine does not return */
          FUN_1400591dc();
        }
        local_818[uVar3] = '\0';
      }
      _guard_check_icall(DAT_14286d388,local_818);
    }
    if (DAT_14285db80 != '\0') {
      iVar2 = FUN_1400277b0(&DAT_14285db80);
      if ((&DAT_14285db80)[iVar2 + -1] != '\n') {
        FUN_140027700(&DAT_14285db80,&DAT_14006e2bc,0x1000);
      }
    }
    FUN_140027700(&DAT_14285db80,param_1,0x1000);
    DAT_14285eb84 = DAT_14285eb84 + 1;
  }
  return;
}


/* ================= FUN_1400424f0 @ 1400424f0  size=308  callers=1 callees=5 ================= */
/* CALLERS: FUN_140042100 */
/* CALLEES: FUN_140042f90, FUN_14004e1f0, JLINK_RTTERMINAL_Control, Sleep, strncmp */

undefined4 FUN_1400424f0(char *param_1,ulonglong param_2,uint *param_3)

{
  uint uVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  int iVar4;
  uint uVar5;
  
  FUN_140042f90("Waiting until RTT Control Block is found...");
  uVar5 = 0;
  uVar3 = FUN_14004e1f0(DAT_14286d388,0,"Searching for RTT control block ...",2000,0);
  iVar4 = 0;
  do {
    Sleep(5);
    iVar4 = iVar4 + 1;
    uVar1 = JLINK_RTTERMINAL_Control(3,param_3 + 1);
    if (iVar4 == 2000) {
      FUN_14004e1f0(DAT_14286d388,uVar3,0,2000,0xffffffff);
      goto LAB_140042602;
    }
    FUN_14004e1f0(DAT_14286d388,uVar3,0,2000,iVar4);
  } while (uVar1 == 0xfffffffe);
  if ((int)uVar1 < 0) {
LAB_140042602:
    uVar2 = 0xffffffff;
  }
  else {
    FUN_14004e1f0(DAT_14286d388,uVar3,0,2000,0xffffffff);
    uVar2 = 0;
    if (uVar1 != 0) {
      do {
        *param_3 = uVar5;
        JLINK_RTTERMINAL_Control(2,param_3);
        iVar4 = strncmp((char *)(param_3 + 2),param_1,param_2 & 0xffffffff);
        if (iVar4 == 0) {
          return 0;
        }
        uVar5 = uVar5 + 1;
        uVar2 = 0xffffffff;
      } while (uVar5 < uVar1);
    }
  }
  return uVar2;
}


/* ================= FUN_140042630 @ 140042630  size=278  callers=1 callees=5 ================= */
/* CALLERS: FUN_1400427b0 */
/* CALLEES: FUN_14003f110, FUN_140043d10, FUN_140043e60, JLINK_RTTERMINAL_Write, Sleep */

ulonglong FUN_140042630(void)

{
  longlong *plVar1;
  ulonglong *puVar2;
  int iVar3;
  ulonglong *puVar4;
  ulonglong uVar5;
  ulonglong uVar6;
  uint uVar7;
  ulonglong uVar8;
  uint uVar9;
  
  uVar7 = (DAT_14085d1e8 + 1) * 0x10;
  puVar4 = (ulonglong *)FUN_140043d10(uVar7);
  if (puVar4 == (ulonglong *)0x0) {
    FUN_14003f110("Insufficient memory!");
    return 0xffffffff;
  }
  uVar6 = 0;
  uVar9 = 0;
  puVar2 = puVar4;
  uVar5 = uVar6;
  for (plVar1 = DAT_14286d3b8; plVar1 != (longlong *)0x0; plVar1 = (longlong *)plVar1[1]) {
    uVar9 = (int)uVar5 + 1;
    uVar5 = (ulonglong)uVar9;
    *(undefined4 *)(puVar2 + 1) = *(undefined4 *)(*plVar1 + 0x31c);
    *puVar2 = (ulonglong)*(uint *)(*plVar1 + 0x318);
    puVar2 = puVar2 + 2;
  }
  *(undefined4 *)(puVar4 + (longlong)(int)uVar9 * 2 + 1) = 0;
  puVar4[(longlong)(int)uVar9 * 2] = 0;
  uVar5 = uVar6;
  uVar8 = uVar6;
  do {
    uVar9 = (int)uVar8 + 1;
    uVar8 = (ulonglong)uVar9;
    if (100 < uVar9) {
      uVar6 = 0xffffffff;
      goto LAB_140042721;
    }
    iVar3 = JLINK_RTTERMINAL_Write(DAT_14085d1b8,uVar5 + (longlong)puVar4,uVar7 - (int)uVar5);
    if (iVar3 < 1) {
      Sleep(1);
    }
    else {
      uVar5 = (ulonglong)(uint)((int)uVar5 + iVar3);
    }
  } while ((uint)uVar5 < uVar7);
  DAT_14285d414 = 0;
LAB_140042721:
  FUN_140043e60(puVar4);
  return uVar6;
}


/* ================= FUN_140042750 @ 140042750  size=94  callers=0 callees=4 ================= */
/* CALLEES: FUN_140044260, FUN_140044270, FUN_140044330, thunk_FUN_14003ecc0 */

undefined4 FUN_140042750(undefined4 param_1)

{
  undefined8 uVar1;
  longlong lVar2;
  undefined4 local_res8 [8];
  
  local_res8[0] = param_1;
  uVar1 = thunk_FUN_14003ecc0();
  lVar2 = FUN_140044270(0x80000001,uVar1);
  if (lVar2 == 0) {
    return 0xffffffff;
  }
  FUN_140044330(lVar2,"DontShowAgainStartJScope",4,local_res8,4);
  FUN_140044260(lVar2);
  return 0;
}


/* ================= FUN_1400427b0 @ 1400427b0  size=528  callers=1 callees=9 ================= */
/* CALLERS: FUN_14003d5c0 */
/* CALLEES: FUN_14002c8f0, FUN_14003f390, FUN_14003f580, FUN_14003f890, FUN_140040ed0, FUN_140042100, FUN_140042630, JLINK_RTTERMINAL_Control, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

ulonglong FUN_1400427b0(void)

{
  int iVar1;
  ulonglong uVar2;
  int iVar3;
  char *pcVar4;
  undefined1 auStack_378 [32];
  undefined *local_358;
  undefined4 local_350;
  undefined8 local_348;
  undefined8 local_340;
  undefined4 local_338;
  int local_330;
  int local_328;
  char local_318 [256];
  undefined1 local_218 [512];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_378;
  iVar3 = 0;
  while( true ) {
    if ((DAT_14285d415 == '\x01') && (iVar1 = FUN_140040ed0(local_318,0x100), iVar1 == -1)) {
      local_338 = 0x300;
      local_340 = 0;
      local_348 = 0;
      local_350 = 0;
      local_358 = (undefined *)0x0;
      local_330 = iVar1;
      local_328 = iVar1;
      FUN_14003f580("Got pending Error from Target.",&DAT_14005f306,local_318,&DAT_1400746bc);
    }
    JLINK_RTTERMINAL_Control(1);
    if (DAT_14285d208 != 1) {
      FUN_14003f390();
    }
    FUN_14003f890();
    JLINK_RTTERMINAL_Control(0,0);
    uVar2 = FUN_140042100(&DAT_14285d420);
    if ((int)uVar2 < 0) {
      return uVar2;
    }
    FUN_140042630();
    uVar2 = FUN_140040ed0(local_318,0x100);
    iVar1 = (int)uVar2;
    if (iVar1 == -2) {
      pcVar4 = "Timeout. No response from target!";
      DAT_14285d415 = '\x01';
    }
    else {
      DAT_14285d415 = '\0';
      if (iVar1 == -1) {
        pcVar4 = local_318;
      }
      else {
        if (iVar1 == 0) {
          DAT_14285d415 = 0;
          return uVar2;
        }
        pcVar4 = "Unspecified Error!";
      }
    }
    if (iVar3 == 1) break;
    iVar3 = iVar3 + 1;
    FUN_14002c8f0(local_218,0x200,"%s\nRetry?",pcVar4);
    local_328 = -1;
    local_330 = -1;
    local_338 = 0x300;
    local_340 = 0;
    local_348 = 0;
    local_350 = 0;
    local_358 = &DAT_14007473c;
    iVar1 = FUN_14003f580("Error.",&DAT_14005f306,local_218,&DAT_140074740);
    if (iVar1 != 0) {
      return 0xffffffff;
    }
  }
  local_328 = 0xffffffff;
  local_330 = 0xffffffff;
  local_338 = 0x300;
  local_340 = 0;
  local_348 = 0;
  local_350 = 0;
  local_358 = (undefined *)0x0;
  FUN_14003f580("Error.",&DAT_14005f306,pcVar4,&DAT_1400746bc);
  return uVar2 & 0xffffffff;
}


/* ================= FUN_1400429c0 @ 1400429c0  size=153  callers=2 callees=1 ================= */
/* CALLERS: FUN_140040a10, FUN_140041870 */
/* CALLEES: FUN_140027760 */

bool FUN_1400429c0(longlong param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = FUN_140027760(param_1 + 0x210,param_2 + 0x210);
  if (((iVar1 == 0) && (iVar1 = FUN_140027760(param_1 + 4,param_2 + 4), iVar1 == 0)) &&
     (iVar1 = FUN_140027760(*(undefined8 *)(param_1 + 0x108),*(undefined8 *)(param_2 + 0x108)),
     iVar1 == 0)) {
    iVar1 = FUN_140027760(param_1 + 0x110,param_2 + 0x110);
    return iVar1 == 0;
  }
  return false;
}


/* ================= FUN_140042a60 @ 140042a60  size=82  callers=3 callees=0 ================= */
/* CALLERS: FUN_14003d5c0, FUN_14003de60, FUN_140042100 */

void FUN_140042a60(void)

{
  longlong *plVar1;
  
  plVar1 = DAT_14286d3b8;
  if (DAT_14285d9c4 == '\0') {
    DAT_14085d184 = 4;
  }
  else {
    DAT_14085d184 = -(uint)(DAT_14285d9c5 != '\0') & 4;
  }
  for (; plVar1 != (longlong *)0x0; plVar1 = (longlong *)plVar1[1]) {
    *(uint *)(*plVar1 + 0x38c) = DAT_14085d184;
    DAT_14085d184 = DAT_14085d184 + *(int *)(*plVar1 + 0x31c);
  }
  return;
}


/* ================= FUN_140042ac0 @ 140042ac0  size=950  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003fa70 */
/* CALLEES: FUN_14003eaa0 */

void FUN_140042ac0(longlong param_1,double *param_2)

{
  byte bVar1;
  ushort uVar2;
  float fVar3;
  uint uVar4;
  char cVar5;
  short sVar6;
  double dVar7;
  
  switch(*(undefined4 *)(param_1 + 0x314)) {
  case 1:
    fVar3 = *(float *)param_2;
    dVar7 = (double)fVar3;
    if (*(double *)(param_1 + 0x398) <= dVar7 && dVar7 != *(double *)(param_1 + 0x398)) {
      *(double *)(param_1 + 0x398) = dVar7;
      fVar3 = *(float *)param_2;
    }
    if ((double)fVar3 < *(double *)(param_1 + 0x3a0)) {
      *(double *)(param_1 + 0x3a0) = (double)fVar3;
    }
    if (DAT_14285da80 != 0) {
      dVar7 = (double)*(float *)param_2;
LAB_140042b40:
      *(double *)(param_1 + 0x3a8) =
           ((double)(DAT_14285da80 - 1) * *(double *)(param_1 + 0x3a8) + dVar7) /
           (double)DAT_14285da80;
      return;
    }
    break;
  case 2:
    dVar7 = *param_2;
    if (*(double *)(param_1 + 0x398) <= dVar7 && dVar7 != *(double *)(param_1 + 0x398)) {
      *(double *)(param_1 + 0x398) = dVar7;
      dVar7 = *param_2;
    }
    if (dVar7 < *(double *)(param_1 + 0x3a0)) {
      *(double *)(param_1 + 0x3a0) = dVar7;
    }
    if (DAT_14285da80 == 0) {
      return;
    }
    dVar7 = (double)(DAT_14285da80 - 1) * *(double *)(param_1 + 0x3a8) + *param_2;
    goto LAB_140042e5c;
  case 5:
  case 6:
    bVar1 = *(byte *)param_2;
    dVar7 = (double)bVar1;
    if (*(double *)(param_1 + 0x398) <= dVar7 && dVar7 != *(double *)(param_1 + 0x398)) {
      *(double *)(param_1 + 0x398) = dVar7;
      bVar1 = *(byte *)param_2;
    }
    if ((double)bVar1 < *(double *)(param_1 + 0x3a0)) {
      *(double *)(param_1 + 0x3a0) = (double)bVar1;
    }
    if (DAT_14285da80 != 0) {
      dVar7 = (double)*(byte *)param_2;
      goto LAB_140042b40;
    }
    break;
  case 7:
    uVar2 = *(ushort *)param_2;
    dVar7 = (double)uVar2;
    if (*(double *)(param_1 + 0x398) <= dVar7 && dVar7 != *(double *)(param_1 + 0x398)) {
      *(double *)(param_1 + 0x398) = dVar7;
      uVar2 = *(ushort *)param_2;
    }
    if ((double)uVar2 < *(double *)(param_1 + 0x3a0)) {
      *(double *)(param_1 + 0x3a0) = (double)uVar2;
    }
    if (DAT_14285da80 != 0) {
      dVar7 = (double)*(ushort *)param_2;
      goto LAB_140042b40;
    }
    break;
  case 8:
    fVar3 = *(float *)param_2;
    dVar7 = (double)(uint)fVar3;
    if (*(double *)(param_1 + 0x398) <= dVar7 && dVar7 != *(double *)(param_1 + 0x398)) {
      *(double *)(param_1 + 0x398) = dVar7;
      fVar3 = *(float *)param_2;
    }
    if ((double)(uint)fVar3 < *(double *)(param_1 + 0x3a0)) {
      *(double *)(param_1 + 0x3a0) = (double)(uint)fVar3;
    }
    if (DAT_14285da80 != 0) {
      dVar7 = (double)(uint)*(float *)param_2;
      goto LAB_140042b40;
    }
    break;
  case 10:
    cVar5 = *(char *)param_2;
    dVar7 = (double)(int)cVar5;
    if (*(double *)(param_1 + 0x398) <= dVar7 && dVar7 != *(double *)(param_1 + 0x398)) {
      *(double *)(param_1 + 0x398) = dVar7;
      cVar5 = *(char *)param_2;
    }
    if ((double)(int)cVar5 < *(double *)(param_1 + 0x3a0)) {
      *(double *)(param_1 + 0x3a0) = (double)(int)cVar5;
    }
    if (DAT_14285da80 != 0) {
      dVar7 = (double)(int)*(char *)param_2;
      goto LAB_140042b40;
    }
    break;
  case 0xb:
    sVar6 = *(short *)param_2;
    dVar7 = (double)(int)sVar6;
    if (*(double *)(param_1 + 0x398) <= dVar7 && dVar7 != *(double *)(param_1 + 0x398)) {
      *(double *)(param_1 + 0x398) = dVar7;
      sVar6 = *(short *)param_2;
    }
    if ((double)(int)sVar6 < *(double *)(param_1 + 0x3a0)) {
      *(double *)(param_1 + 0x3a0) = (double)(int)sVar6;
    }
    if (DAT_14285da80 != 0) {
      dVar7 = (double)(int)*(short *)param_2;
      goto LAB_140042b40;
    }
    break;
  case 0xc:
    fVar3 = *(float *)param_2;
    dVar7 = (double)(int)fVar3;
    if (*(double *)(param_1 + 0x398) <= dVar7 && dVar7 != *(double *)(param_1 + 0x398)) {
      *(double *)(param_1 + 0x398) = dVar7;
      fVar3 = *(float *)param_2;
    }
    if ((double)(int)fVar3 < *(double *)(param_1 + 0x3a0)) {
      *(double *)(param_1 + 0x3a0) = (double)(int)fVar3;
    }
    if (DAT_14285da80 != 0) {
      dVar7 = (double)(int)*(float *)param_2;
      goto LAB_140042b40;
    }
    break;
  case 0xe:
    uVar4 = FUN_14003eaa0(*(float *)param_2,param_1);
    dVar7 = (double)uVar4;
    if (*(double *)(param_1 + 0x398) <= dVar7 && dVar7 != *(double *)(param_1 + 0x398)) {
      *(double *)(param_1 + 0x398) = dVar7;
    }
    if (dVar7 < *(double *)(param_1 + 0x3a0)) {
      *(double *)(param_1 + 0x3a0) = dVar7;
    }
    if (DAT_14285da80 == 0) {
      return;
    }
    dVar7 = (double)(DAT_14285da80 - 1) * *(double *)(param_1 + 0x3a8) + dVar7;
LAB_140042e5c:
    *(double *)(param_1 + 0x3a8) = dVar7 / (double)DAT_14285da80;
  }
  return;
}


/* ================= FUN_140042ef0 @ 140042ef0  size=50  callers=0 callees=3 ================= */
/* CALLEES: FUN_14003e990, FUN_14003ef60, FUN_140040e10 */

void FUN_140042ef0(undefined8 param_1)

{
  if ((DAT_14285d9c4 != '\0') && (DAT_14285d9c6 == '\0')) {
    FUN_14003ef60();
  }
  FUN_140040e10(param_1);
  FUN_14003e990();
  return;
}


/* ================= FUN_140042f30 @ 140042f30  size=37  callers=0 callees=3 ================= */
/* CALLEES: FUN_14003e990, FUN_140040e10, FUN_14004e120 */

void FUN_140042f30(void)

{
  FUN_140040e10();
  FUN_14003e990();
  DAT_14286d379 = 0;
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_140042f60 @ 140042f60  size=40  callers=0 callees=3 ================= */
/* CALLEES: FUN_14003e990, FUN_14003f110, FUN_140040e10 */

void FUN_140042f60(longlong param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x1c);
  FUN_140040e10();
  if (iVar1 < 0) {
    FUN_14003f110("Failed to start data sampling.");
  }
  FUN_14003e990();
  return;
}


/* ================= FUN_140042f90 @ 140042f90  size=85  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400424f0 */
/* CALLEES: FUN_14000e420, __acrt_iob_func, __stdio_common_vfprintf */

void FUN_140042f90(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  undefined8 local_res10;
  undefined8 local_res18;
  undefined8 local_res20;
  
  local_res10 = param_2;
  local_res18 = param_3;
  local_res20 = param_4;
  uVar1 = __acrt_iob_func(1);
  puVar2 = (undefined8 *)FUN_14000e420();
  __stdio_common_vfprintf(*puVar2,uVar1,param_1,0,&local_res10);
  return;
}


/* ================= FUN_140042ff0 @ 140042ff0  size=13  callers=6 callees=0 ================= */
/* CALLERS: FUN_140039430, FUN_14004db40, FUN_14004dcb0, FUN_14004dd70, FUN_14004df50, FUN_14004e1f0 */

void FUN_140042ff0(HANDLE param_1)

{
  if (param_1 != (HANDLE)0x0) {
                    /* WARNING: Could not recover jumptable at 0x000140042ff5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    CloseHandle(param_1);
    return;
  }
  return;
}


/* ================= FUN_140043000 @ 140043000  size=73  callers=3 callees=3 ================= */
/* CALLERS: FUN_140039430, FUN_14004db40, FUN_14004f300 */
/* CALLEES: CloseHandle, DeleteCriticalSection, FUN_140043e60 */

void FUN_140043000(undefined8 *param_1)

{
  int *piVar1;
  
  piVar1 = (int *)*param_1;
  if (piVar1 != (int *)0x0) {
    if (*piVar1 == 0) {
      CloseHandle(((LPCRITICAL_SECTION)(piVar1 + 2))->DebugInfo);
    }
    else {
      DeleteCriticalSection((LPCRITICAL_SECTION)(piVar1 + 2));
    }
    FUN_140043e60(piVar1);
    *param_1 = 0;
  }
  return;
}


/* ================= FUN_140043050 @ 140043050  size=150  callers=1 callees=1 ================= */
/* CALLERS: FUN_140043970 */
/* CALLEES: QueryPerformanceFrequency */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

longlong FUN_140043050(longlong param_1)

{
  longlong lVar1;
  float fVar2;
  
  if (DAT_14285ec3c == 0) {
    DAT_14285ec3c = 1;
    QueryPerformanceFrequency((LARGE_INTEGER *)&DAT_14285eda0);
    _DAT_14285edac = (float)_DAT_14285eda0;
    DAT_14285eda8 = _DAT_14285edac / 1e+06;
  }
  lVar1 = 0;
  fVar2 = (float)param_1 / DAT_14285eda8;
  if ((9.223372e+18 <= fVar2) && (fVar2 = fVar2 - 9.223372e+18, fVar2 < 9.223372e+18)) {
    lVar1 = -0x8000000000000000;
  }
  return (longlong)fVar2 + lVar1;
}


/* ================= FUN_1400430f0 @ 1400430f0  size=17  callers=6 callees=0 ================= */
/* CALLERS: FUN_140039370, FUN_14004db40, FUN_14004dcb0, FUN_14004dd70, FUN_14004df50, FUN_14004e1f0 */

void FUN_1400430f0(undefined4 param_1,undefined4 param_2)

{
                    /* WARNING: Could not recover jumptable at 0x0001400430fa. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  CreateEventA(0,param_1,param_2,0);
  return;
}


/* ================= FUN_140043110 @ 140043110  size=120  callers=3 callees=3 ================= */
/* CALLERS: FUN_140039400, FUN_14004c6a0, FUN_14004ef00 */
/* CALLEES: CreateMutexA, FUN_140043d20, InitializeCriticalSectionAndSpinCount */

undefined4 * FUN_140043110(LPCSTR param_1)

{
  undefined4 *puVar1;
  HANDLE pvVar2;
  
  puVar1 = (undefined4 *)FUN_140043d20(0x30,"Mutex info");
  if (param_1 != (LPCSTR)0x0) {
    pvVar2 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,param_1);
    *(HANDLE *)(puVar1 + 2) = pvVar2;
    *puVar1 = 0;
    return puVar1;
  }
  InitializeCriticalSectionAndSpinCount((LPCRITICAL_SECTION)(puVar1 + 2),0);
  *puVar1 = 1;
  return puVar1;
}


/* ================= FUN_140043190 @ 140043190  size=116  callers=1 callees=2 ================= */
/* CALLERS: FUN_140039370 */
/* CALLEES: CreateThread, FUN_140044750 */

HANDLE FUN_140043190(LPTHREAD_START_ROUTINE param_1,LPVOID param_2,ulonglong *param_3,
                    longlong param_4,uint param_5)

{
  HANDLE pvVar1;
  
  pvVar1 = CreateThread((LPSECURITY_ATTRIBUTES)0x0,0,param_1,param_2,(param_5 & 1) << 2,&param_5);
  if (pvVar1 != (HANDLE)0x0) {
    if (param_4 != 0) {
      FUN_140044750(pvVar1,param_4);
    }
    if (param_3 != (ulonglong *)0x0) {
      *param_3 = (ulonglong)param_5;
    }
  }
  return pvVar1;
}


/* ================= FUN_140043210 @ 140043210  size=541  callers=2 callees=10 ================= */
/* CALLERS: FUN_140043210, FUN_140043430 */
/* CALLEES: FUN_140027140, FUN_140027700, FUN_140043210, FUN_1400444f0, FUN_1400591dc, __security_check_cookie, _errno, _mkdir, _stat64i32, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

int FUN_140043210(char *param_1)

{
  longlong lVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  longlong lVar5;
  int iVar7;
  undefined1 auStack_3d8 [32];
  undefined4 local_3b8;
  char *local_3b0;
  undefined4 local_3a8;
  undefined8 local_3a0;
  undefined4 local_398;
  char local_388 [8];
  undefined1 local_380 [6];
  ushort local_37a;
  char local_348 [272];
  char local_238 [272];
  char local_128 [272];
  ulonglong local_18;
  longlong lVar6;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_3d8;
  iVar7 = 0;
  DAT_14285ec28 = DAT_14285ec28 + 1;
  if ((param_1 == (char *)0x0) || (0x80 < DAT_14285ec28)) {
LAB_14004324c:
    iVar7 = -1;
    goto LAB_1400433e2;
  }
  local_398 = 0;
  local_3b0 = local_128;
  local_3a0 = 0;
  local_3a8 = 0x104;
  local_3b8 = 0x104;
  FUN_1400444f0(param_1,local_388,3,local_238);
  if (local_238[0] == '\0') goto LAB_1400433e2;
  iVar2 = _stat64i32(param_1,local_380);
  if (iVar2 == 0) {
    iVar7 = -(uint)((local_37a & 0xf000) != 0x4000);
    goto LAB_1400433e2;
  }
  piVar4 = _errno();
  if (*piVar4 != 2) goto LAB_14004324c;
  local_398 = 0;
  local_3b0 = local_128;
  local_3a0 = 0;
  local_3a8 = 0x104;
  local_3b8 = 0x104;
  FUN_1400444f0(param_1,local_388,3,local_238);
  memset(local_348,0,0x104);
  if (local_388[0] == '\0') {
    FUN_140027140(local_348,local_238,0x104);
  }
  else {
    FUN_140027140(local_348,local_388,0x104);
    FUN_140027700(local_348,local_238,0x104);
  }
  if (local_348[0] == '\0') {
    if (local_128[0] != '\0') goto LAB_140043418;
  }
  else {
    lVar6 = -1;
    do {
      lVar5 = lVar6 + 1;
      lVar1 = lVar6 + 1;
      lVar6 = lVar5;
    } while (local_348[lVar1] != '\0');
    uVar3 = (int)lVar5 - 1;
    if ((local_348[uVar3] == '\\') || (local_348[uVar3] == '/')) {
      if (0x103 < (ulonglong)uVar3) {
                    /* WARNING: Subroutine does not return */
        FUN_1400591dc();
      }
      local_348[uVar3] = '\0';
    }
    iVar2 = FUN_140043210(local_348);
    if (-1 < iVar2) {
LAB_140043418:
      iVar2 = _mkdir(param_1);
      if (iVar2 != 0) {
        iVar7 = -1;
      }
      goto LAB_1400433e2;
    }
  }
  iVar7 = -1;
LAB_1400433e2:
  DAT_14285ec28 = DAT_14285ec28 + -1;
  return iVar7;
}


/* ================= FUN_140043430 @ 140043430  size=167  callers=1 callees=5 ================= */
/* CALLERS: FUN_14004ef00 */
/* CALLEES: FUN_140027140, FUN_140027700, FUN_140043210, SHGetSpecialFolderPathA, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_140043430(longlong param_1,int param_2)

{
  BOOL BVar1;
  int iVar2;
  undefined1 auStack_148 [32];
  CHAR local_128 [272];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_148;
  if (((param_1 != 0) && (param_2 != 0)) &&
     (BVar1 = SHGetSpecialFolderPathA((HWND)0x0,local_128,0x1a,1), BVar1 != 0)) {
    FUN_140027700(local_128,"\\SEGGER",0x104);
    iVar2 = FUN_140043210(local_128);
    if (-1 < iVar2) {
      FUN_140027140(param_1,local_128,param_2);
      return 0;
    }
  }
  return 0xffffffff;
}


/* ================= FUN_1400434e0 @ 1400434e0  size=176  callers=2 callees=4 ================= */
/* CALLERS: FUN_140041cd0, FUN_140057220 */
/* CALLEES: FUN_140027140, FUN_1400277b0, FUN_1400444f0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_1400434e0(undefined8 param_1,undefined1 *param_2,uint param_3)

{
  uint uVar1;
  undefined1 auStack_378 [32];
  undefined4 local_358;
  undefined1 *local_350;
  undefined4 local_348;
  undefined1 *local_340;
  undefined4 local_338;
  undefined1 local_328 [16];
  undefined1 local_318 [256];
  undefined1 local_218 [256];
  undefined1 local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_378;
  local_338 = 0x100;
  local_340 = local_318;
  local_350 = local_218;
  local_348 = 0x100;
  local_358 = 0x100;
  FUN_1400444f0(param_1,local_328,3,local_118);
  uVar1 = FUN_1400277b0(local_318);
  if (param_3 < uVar1) {
    *param_2 = 0;
  }
  else {
    FUN_140027140(param_2,local_318,param_3);
  }
  return;
}


/* ================= FUN_140043590 @ 140043590  size=176  callers=1 callees=4 ================= */
/* CALLERS: FUN_140057220 */
/* CALLEES: FUN_140027140, FUN_1400277b0, FUN_1400444f0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140043590(undefined8 param_1,undefined1 *param_2,uint param_3)

{
  uint uVar1;
  undefined1 auStack_378 [32];
  undefined4 local_358;
  undefined1 *local_350;
  undefined4 local_348;
  undefined1 *local_340;
  undefined4 local_338;
  undefined1 local_328 [16];
  undefined1 local_318 [256];
  undefined1 local_218 [256];
  undefined1 local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_378;
  local_338 = 0x100;
  local_340 = local_218;
  local_350 = local_318;
  local_348 = 0x100;
  local_358 = 0x100;
  FUN_1400444f0(param_1,local_328,3,local_118);
  uVar1 = FUN_1400277b0(local_318);
  if (param_3 < uVar1) {
    *param_2 = 0;
  }
  else {
    FUN_140027140(param_2,local_318,param_3);
  }
  return;
}


/* ================= FUN_140043660 @ 140043660  size=162  callers=2 callees=4 ================= */
/* CALLERS: FUN_14003d210, FUN_14003d9b0 */
/* CALLEES: FUN_140044800, FileTimeToSystemTime, GetFileTime, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_140043660(HANDLE param_1,longlong param_2,longlong param_3)

{
  BOOL BVar1;
  undefined8 uVar2;
  undefined1 auStack_58 [32];
  _FILETIME local_38;
  _FILETIME local_30;
  _SYSTEMTIME local_28;
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_58;
  BVar1 = GetFileTime(param_1,(LPFILETIME)0x0,&local_38,&local_30);
  if (BVar1 == 0) {
LAB_140043695:
    uVar2 = 0xffffffff;
  }
  else {
    if (param_2 != 0) {
      BVar1 = FileTimeToSystemTime(&local_38,&local_28);
      if (BVar1 == 0) goto LAB_140043695;
      FUN_140044800(&local_28,param_2);
    }
    if (param_3 != 0) {
      BVar1 = FileTimeToSystemTime(&local_30,&local_28);
      if (BVar1 == 0) goto LAB_140043695;
      FUN_140044800(&local_28,param_3);
    }
    uVar2 = 0;
  }
  return uVar2;
}


/* ================= FUN_140043710 @ 140043710  size=9  callers=2 callees=0 ================= */
/* CALLERS: FUN_140039f10, FUN_14004f590 */

void FUN_140043710(undefined8 param_1)

{
                    /* WARNING: Could not recover jumptable at 0x000140043712. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  GetFileSize(param_1,0);
  return;
}


/* ================= FUN_140043720 @ 140043720  size=47  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400440e0, FUN_14004ef00 */
/* CALLEES: GetFileAttributesA */

bool FUN_140043720(LPCSTR param_1)

{
  DWORD DVar1;
  
  DVar1 = GetFileAttributesA(param_1);
  if (DVar1 != 0xffffffff) {
    return (DVar1 & 0x10) == 0;
  }
  return false;
}


/* ================= FUN_140043750 @ 140043750  size=11  callers=4 callees=1 ================= */
/* CALLERS: FUN_140039ec0, FUN_14003d210, FUN_14003d9b0, FUN_1400471d0 */
/* CALLEES: FUN_140043760 */

void FUN_140043750(undefined8 param_1,undefined8 param_2)

{
  FUN_140043760(param_1,param_2,0,0);
  return;
}


/* ================= FUN_140043760 @ 140043760  size=331  callers=3 callees=3 ================= */
/* CALLERS: FUN_140043750, FUN_14004f590, FUN_14004f6c0 */
/* CALLEES: CreateFileA, SetFilePointer, Sleep */

HANDLE FUN_140043760(LPCSTR param_1,uint param_2,int param_3,DWORD param_4)

{
  DWORD DVar1;
  HANDLE hFile;
  DWORD dwCreationDisposition;
  uint dwShareMode;
  DWORD dwDesiredAccess;
  uint dwFlagsAndAttributes;
  
  dwDesiredAccess = (param_2 & 2 | param_2 * 4) << 0x1d;
  dwShareMode = (param_2 >> 4 & 0x10 | param_2 & 0xc) >> 2;
  dwFlagsAndAttributes = ((param_2 & 0x200) << 6 | param_2 & 0x400) << 0xf | 0x80;
  DVar1 = 2;
  if ((param_2 & 0x10) == 0) {
    dwCreationDisposition = 3;
    DVar1 = 5;
  }
  else {
    if ((char)param_2 < '\0') {
      dwCreationDisposition = 1;
      goto LAB_140043803;
    }
    dwCreationDisposition = 4;
  }
  if ((param_2 & 0x20) != 0) {
    dwCreationDisposition = DVar1;
  }
LAB_140043803:
  hFile = CreateFileA(param_1,dwDesiredAccess,dwShareMode,(LPSECURITY_ATTRIBUTES)0x0,
                      dwCreationDisposition,dwFlagsAndAttributes,(HANDLE)0x0);
  while( true ) {
    if (hFile != (HANDLE)0xffffffffffffffff) {
      if ((param_2 & 0x40) != 0) {
        SetFilePointer(hFile,0,(PLONG)0x0,2);
      }
      return hFile;
    }
    if (param_3 == 0) break;
    param_3 = param_3 + -1;
    if (param_4 != 0) {
      Sleep(param_4);
    }
    hFile = CreateFileA(param_1,dwDesiredAccess,dwShareMode,(LPSECURITY_ATTRIBUTES)0x0,
                        dwCreationDisposition,dwFlagsAndAttributes,(HANDLE)0x0);
  }
  return (HANDLE)0xffffffffffffffff;
}


/* ================= FUN_1400438b0 @ 1400438b0  size=45  callers=2 callees=1 ================= */
/* CALLERS: FUN_140039f10, FUN_14004f590 */
/* CALLEES: ReadFile */

DWORD FUN_1400438b0(HANDLE param_1,LPVOID param_2,DWORD param_3)

{
  BOOL BVar1;
  DWORD local_res20 [2];
  
  BVar1 = ReadFile(param_1,param_2,param_3,local_res20,(LPOVERLAPPED)0x0);
  if (BVar1 == 0) {
    local_res20[0] = 0xffffffff;
  }
  return local_res20[0];
}


/* ================= FUN_1400438e0 @ 1400438e0  size=60  callers=1 callees=0 ================= */
/* CALLERS: FUN_140039f10 */

void FUN_1400438e0(HANDLE param_1,LONG param_2,int param_3)

{
  if (param_3 != 0) {
    if (param_3 == 1) {
                    /* WARNING: Could not recover jumptable at 0x000140043908. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      SetFilePointer(param_1,param_2,(PLONG)0x0,1);
      return;
    }
    if (param_3 == 2) {
                    /* WARNING: Could not recover jumptable at 0x0001400438f8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      SetFilePointer(param_1,param_2,(PLONG)0x0,2);
      return;
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140043915. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  SetFilePointer(param_1,param_2,(PLONG)0x0,0);
  return;
}


/* ================= FUN_140043920 @ 140043920  size=45  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400471d0, FUN_14004f6c0 */
/* CALLEES: WriteFile */

DWORD FUN_140043920(HANDLE param_1,LPCVOID param_2,DWORD param_3)

{
  BOOL BVar1;
  DWORD local_res20 [2];
  
  BVar1 = WriteFile(param_1,param_2,param_3,local_res20,(LPOVERLAPPED)0x0);
  if (BVar1 == 0) {
    local_res20[0] = 0xffffffff;
  }
  return local_res20[0];
}


/* ================= FUN_140043950 @ 140043950  size=17  callers=2 callees=1 ================= */
/* CALLERS: FUN_14004c6a0, FUN_14004df50 */
/* CALLEES: GetCurrentThreadId */

DWORD FUN_140043950(void)

{
  DWORD DVar1;
  
  DVar1 = GetCurrentThreadId();
  return DVar1;
}


/* ================= FUN_140043970 @ 140043970  size=30  callers=5 callees=2 ================= */
/* CALLERS: FUN_140039140, FUN_140041710, FUN_1400443e0, FUN_14004bc30, FUN_14004c1b0 */
/* CALLEES: FUN_140043050, FUN_140043990 */

void FUN_140043970(longlong param_1)

{
  longlong lVar1;
  
  lVar1 = FUN_140043990();
  FUN_140043050(lVar1 - param_1);
  return;
}


/* ================= FUN_140043990 @ 140043990  size=49  callers=8 callees=1 ================= */
/* CALLERS: FUN_140039140, FUN_14003d5c0, FUN_140041710, FUN_140043970, FUN_1400443e0, FUN_140048e60, FUN_14004bc30, FUN_14004c1b0 */
/* CALLEES: QueryPerformanceCounter */

longlong FUN_140043990(void)

{
  LARGE_INTEGER local_res8 [4];
  
  QueryPerformanceCounter(local_res8);
  if (DAT_14285edb0.QuadPart == 0) {
    DAT_14285edb0 = local_res8[0];
  }
  return local_res8[0].QuadPart - (longlong)DAT_14285edb0;
}


/* ================= FUN_1400439e0 @ 1400439e0  size=30  callers=1 callees=0 ================= */
/* CALLERS: FUN_140043a00 */

void FUN_1400439e0(LPSTR param_1,DWORD param_2)

{
  if ((param_1 != (LPSTR)0x0) && (param_2 != 0)) {
                    /* WARNING: Could not recover jumptable at 0x0001400439f6. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    GetModuleFileNameA(DAT_14285ec20,param_1,param_2);
    return;
  }
  return;
}


/* ================= FUN_140043a00 @ 140043a00  size=198  callers=2 callees=4 ================= */
/* CALLERS: FUN_14004f820, FUN_1400578b0 */
/* CALLEES: FUN_1400439e0, FUN_1400444d0, FUN_1400444f0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140043a00(undefined8 param_1,undefined4 param_2)

{
  undefined1 auStack_488 [32];
  undefined8 local_468;
  undefined1 *local_460;
  undefined4 local_458;
  undefined1 *local_450;
  undefined4 local_448;
  undefined1 local_438 [16];
  undefined1 local_428 [256];
  undefined1 local_328 [272];
  undefined1 local_218 [256];
  undefined1 local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_488;
  FUN_1400439e0(local_328,0x104);
  local_448 = 0x100;
  local_450 = local_218;
  local_460 = local_118;
  local_458 = 0x100;
  local_468 = CONCAT44(local_468._4_4_,0x100);
  FUN_1400444f0(local_328,local_438,3,local_428);
  local_460 = (undefined1 *)0x0;
  local_468 = 0;
  FUN_1400444d0(param_1,param_2,local_438,local_428);
  return;
}


/* ================= FUN_140043ad0 @ 140043ad0  size=493  callers=1 callees=7 ================= */
/* CALLERS: FUN_140037210 */
/* CALLEES: FUN_140027140, FUN_140027370, FUN_140027760, RegCloseKey, RegOpenKeyExA, RegQueryValueExA, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined4 FUN_140043ad0(void)

{
  LSTATUS LVar1;
  int iVar2;
  undefined *puVar3;
  uint uVar4;
  ulonglong uVar5;
  undefined4 uVar6;
  undefined **ppuVar7;
  undefined1 auStackY_c8 [32];
  DWORD local_98;
  uint local_94;
  HKEY local_90;
  BYTE local_88 [8];
  BYTE *local_80;
  BYTE local_78 [64];
  BYTE local_38 [32];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStackY_c8;
  uVar5 = 0;
  local_78[0] = '\0';
  local_90 = (HKEY)0x0;
  uVar6 = 0x1000;
  LVar1 = RegOpenKeyExA((HKEY)0xffffffff80000002,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",0
                        ,0x20019,&local_90);
  if (LVar1 != 0) goto LAB_140043c87;
  local_98 = 0x40;
  LVar1 = RegQueryValueExA(local_90,"CurrentVersion",(LPDWORD)0x0,(LPDWORD)0x0,local_78,&local_98);
  if (LVar1 != 0) goto LAB_140043c87;
  iVar2 = FUN_140027760(local_78,&DAT_1400752b0);
  if (iVar2 == 0) {
    local_98 = 4;
    LVar1 = RegQueryValueExA(local_90,"CurrentMajorVersionNumber",(LPDWORD)0x0,(LPDWORD)0x0,local_88
                             ,&local_98);
    if (LVar1 == 0) {
      local_98 = 0x20;
      LVar1 = RegQueryValueExA(local_90,"CurrentBuildNumber",(LPDWORD)0x0,(LPDWORD)0x0,local_38,
                               &local_98);
      if (LVar1 == 0) {
        local_80 = local_38;
        FUN_140027370(&local_80,&local_94);
        puVar3 = &DAT_1400752a0;
        if (local_94 < 22000) goto LAB_140043c27;
      }
      else {
        local_94 = 0;
LAB_140043c27:
        puVar3 = &DAT_1400752a8;
      }
      FUN_140027140(local_78,puVar3,0x40);
    }
  }
  ppuVar7 = &PTR_DAT_1400751f0;
  do {
    iVar2 = FUN_140027760(local_78,*ppuVar7);
    if (iVar2 == 0) {
      uVar6 = (&DAT_1400751f8)[uVar5 * 4];
      break;
    }
    uVar4 = (int)uVar5 + 1;
    uVar5 = (ulonglong)uVar4;
    ppuVar7 = ppuVar7 + 2;
  } while (uVar4 < 9);
LAB_140043c87:
  if (local_90 != (HKEY)0x0) {
    RegCloseKey(local_90);
  }
  return uVar6;
}


/* ================= FUN_140043cc0 @ 140043cc0  size=72  callers=2 callees=1 ================= */
/* CALLERS: FUN_140039430, FUN_14003f830 */
/* CALLEES: FUN_1400446e0 */

int FUN_140043cc0(void)

{
  int iVar1;
  
  if (DAT_14285ed30 == 0) {
    FUN_1400446e0();
  }
  if (DAT_14285ed48 == (code *)0x0) {
    iVar1 = 0;
  }
  else {
    iVar1 = (*DAT_14285ed48)();
  }
  if (DAT_14285edb8 == 0) {
    DAT_14285edb8 = iVar1;
  }
  return iVar1 - DAT_14285edb8;
}


/* ================= FUN_140043d10 @ 140043d10  size=7  callers=32 callees=1 ================= */
/* CALLERS: FUN_1400331f0, FUN_140033230, FUN_1400351b0, FUN_140039300, FUN_140039b60, FUN_140039f10, FUN_14003d7d0, FUN_14003d9b0, FUN_14003df10, FUN_14003dfa0, FUN_14003e050, FUN_14003f6d0, FUN_14003ffd0, FUN_140040cd0, FUN_140040fa0, FUN_140041f30, FUN_140042630, FUN_140043fd0, FUN_140044d50, FUN_140046e80, FUN_140047020, FUN_1400471d0, FUN_140047390, FUN_1400476d0, FUN_14004ef00, FUN_140050760, FUN_140053000, FUN_140053920, FUN_140054060, FUN_140054250, FUN_1400561b0, FUN_140056e40 */
/* CALLEES: FUN_140043d20 */

void FUN_140043d10(undefined8 param_1)

{
  FUN_140043d20(param_1,0);
  return;
}


/* ================= FUN_140043d20 @ 140043d20  size=310  callers=33 callees=4 ================= */
/* CALLERS: FUN_14002a410, FUN_14002bc30, FUN_140033540, FUN_140033b90, FUN_1400343e0, FUN_140034630, FUN_1400352e0, FUN_140035540, FUN_14003d5c0, FUN_14003e0b0, FUN_14003e110, FUN_14003e1b0, FUN_14003e210, FUN_14003e2e0, FUN_14003e380, FUN_14003e420, FUN_14003e4e0, FUN_14003e5c0, FUN_14003e620, FUN_14003e680, FUN_14003e6e0, FUN_14003e740, FUN_14003e7a0, FUN_14003e800, FUN_14003ef60, FUN_1400404c0, FUN_140040ab0, FUN_140043110, FUN_140043d10, FUN_140043fd0, FUN_14004d6c0, FUN_14004d9c0, FUN_1400578b0 */
/* CALLEES: FUN_14002c8f0, MessageBoxA, __security_check_cookie, malloc */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint * FUN_140043d20(uint param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  undefined1 auStack_78 [32];
  CHAR local_58 [64];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_78;
  uVar3 = param_1;
  if (DAT_14285eb9c != 0) {
    uVar3 = param_1 + 0x40;
  }
  puVar4 = malloc((ulonglong)uVar3);
  if (puVar4 != (uint *)0x0) {
    if (DAT_14285eb9c == 0) {
      return puVar4;
    }
    puVar1 = (undefined8 *)(((ulonglong)uVar3 - 0x20) + (longlong)puVar4);
    *puVar1 = 0xcececececececece;
    puVar1[1] = 0xcececececececece;
    puVar1 = (undefined8 *)(((ulonglong)uVar3 - 0x10) + (longlong)puVar4);
    *puVar1 = 0xcececececececece;
    puVar1[1] = 0xcececececececece;
    puVar4[4] = 0;
    puVar4[5] = 0;
    puVar4[6] = 0;
    puVar4[7] = 0;
    *puVar4 = param_1;
    puVar4[1] = 0xdeadbeef;
    *(undefined8 *)(puVar4 + 2) = param_2;
    do {
      iVar2 = DAT_14285ec2c;
      LOCK();
      DAT_14285ec2c = 1;
      UNLOCK();
    } while (iVar2 != 0);
    _DAT_14285eb90 = _DAT_14285eb90 + 1;
    _DAT_14285eb94 = _DAT_14285eb94 + param_1;
    *(uint **)(puVar4 + 4) = DAT_14285ec30;
    if (DAT_14285ec30 != (uint *)0x0) {
      *(uint **)(DAT_14285ec30 + 6) = puVar4;
    }
    LOCK();
    DAT_14285ec2c = 0;
    UNLOCK();
    DAT_14285ec30 = puVar4;
    if (puVar4 + 8 != (uint *)0x0) {
      DAT_14285ec2c = 0;
      return puVar4 + 8;
    }
  }
  FUN_14002c8f0(local_58,0x40,"Failed to allocate 0x%08X bytes of memory!",param_1);
  MessageBoxA((HWND)0x0,local_58,"Error",0x42010);
  return (uint *)0x0;
}


/* ================= FUN_140043e60 @ 140043e60  size=359  callers=40 callees=7 ================= */
/* CALLERS: FUN_14002b610, FUN_14002bc30, FUN_140032fd0, FUN_140033230, FUN_140033540, FUN_140033b90, FUN_1400343e0, FUN_1400350e0, FUN_1400351b0, FUN_1400353b0, FUN_1400354d0, FUN_1400374d0, FUN_140039300, FUN_140039430, FUN_140039b60, FUN_140039f10, FUN_14003d7d0, FUN_14003d9b0, FUN_14003e880, FUN_14003f0a0, FUN_14003f6a0, FUN_140040bd0, FUN_140040e10, FUN_140040fa0, FUN_140042630, FUN_140043000, FUN_140043fd0, FUN_140047020, FUN_1400471d0, FUN_140047390, FUN_1400476d0, FUN_14004d6c0, FUN_14004d9c0, FUN_14004ef00, FUN_140051360, FUN_140053000, FUN_140053920, FUN_140054060, FUN_140054250, FUN_140056e40 */
/* CALLEES: CloseHandle, FUN_140044520, FUN_140044610, UnmapViewOfFile, free, memmove, memset */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140043e60(uint *param_1)

{
  uint *puVar1;
  int iVar2;
  uint *puVar3;
  int iVar4;
  
  if (param_1 != (uint *)0x0) {
    puVar3 = param_1;
    iVar2 = FUN_140044610();
    if (-1 < iVar2) {
      UnmapViewOfFile((LPCVOID)(&DAT_14285eba8)[(longlong)iVar2 * 2]);
      CloseHandle(*(HANDLE *)(&DAT_14285eba0 + (longlong)iVar2 * 0x10));
      iVar4 = DAT_14285eb98 + -1;
      DAT_14285eb98 = iVar4;
      if (iVar2 < iVar4) {
        memmove(&DAT_14285eba0 + (longlong)iVar2 * 0x10,
                &DAT_14285eba0 + (longlong)(iVar2 + 1) * 0x10,(longlong)(iVar4 - iVar2) << 4);
      }
      (&DAT_14285eba8)[(longlong)iVar4 * 2] = 0;
      *(undefined8 *)(&DAT_14285eba0 + (longlong)iVar4 * 0x10) = 0;
      return;
    }
    if (DAT_14285eb9c != 0) {
      puVar1 = puVar3 + -8;
      iVar2 = FUN_140044520(puVar1);
      if (-1 < iVar2) {
        memset(param_1,0xce,(ulonglong)*puVar1);
        do {
          iVar2 = DAT_14285ec2c;
          LOCK();
          DAT_14285ec2c = 1;
          UNLOCK();
        } while (iVar2 != 0);
        _DAT_14285eb90 = _DAT_14285eb90 + -1;
        _DAT_14285eb94 = _DAT_14285eb94 - *puVar1;
        if (*(longlong *)(puVar3 + -4) != 0) {
          *(undefined8 *)(*(longlong *)(puVar3 + -4) + 0x18) = *(undefined8 *)(puVar3 + -2);
        }
        if (*(longlong *)(puVar3 + -2) != 0) {
          *(undefined8 *)(*(longlong *)(puVar3 + -2) + 0x10) = *(undefined8 *)(puVar3 + -4);
        }
        if (puVar1 == DAT_14285ec30) {
          DAT_14285ec30 = *(uint **)(puVar3 + -4);
        }
        LOCK();
        DAT_14285ec2c = 0;
        UNLOCK();
        param_1 = puVar1;
      }
    }
    free(param_1);
  }
  return;
}


/* ================= FUN_140043fd0 @ 140043fd0  size=268  callers=1 callees=10 ================= */
/* CALLERS: FUN_14003f760 */
/* CALLEES: FUN_14002c8f0, FUN_140043d10, FUN_140043d20, FUN_140043e60, FUN_140044520, FUN_140044610, MessageBoxA, __security_check_cookie, memcpy, realloc */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void * FUN_140043fd0(void *param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  void *pvVar3;
  ulonglong _NewSize;
  undefined1 auStack_88 [32];
  CHAR local_68 [64];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_88;
  _NewSize = (ulonglong)param_2;
  if (DAT_14285eb9c == 0) {
    pvVar3 = realloc(param_1,_NewSize);
  }
  else {
    pvVar3 = param_1;
    iVar1 = FUN_140044610();
    if (iVar1 < 0) {
      if (param_1 == (void *)0x0) {
        pvVar3 = (void *)FUN_140043d10(_NewSize);
      }
      else {
        FUN_140044520((longlong)pvVar3 + -0x20);
        pvVar3 = (void *)FUN_140043d20(_NewSize,*(undefined8 *)((longlong)param_1 + -0x18));
        if (pvVar3 != (void *)0x0) {
          uVar2 = *(uint *)((longlong)param_1 + -0x20);
          if (param_2 < *(uint *)((longlong)param_1 + -0x20)) {
            uVar2 = param_2;
          }
          memcpy(pvVar3,param_1,(ulonglong)uVar2);
        }
        FUN_140043e60(param_1);
      }
      if (pvVar3 == (void *)0x0) {
        FUN_14002c8f0(local_68,0x40,"Failed to reallocate 0x%08X bytes of memory!",_NewSize);
        MessageBoxA((HWND)0x0,local_68,"Error",0x42010);
      }
    }
    else {
      MessageBoxA((HWND)0x0,"SYS_MEM_Realloc() can not be used for shared memory !"," Debug Error",
                  0x42030);
      pvVar3 = (void *)0x0;
    }
  }
  return pvVar3;
}


/* ================= FUN_1400440e0 @ 1400440e0  size=382  callers=1 callees=3 ================= */
/* CALLERS: FUN_14004ef00 */
/* CALLEES: FUN_1400277b0, FUN_140043720, memcpy */

void FUN_1400440e0(char *param_1,char *param_2,char *param_3,int param_4)

{
  bool bVar1;
  bool bVar2;
  char cVar3;
  uint uVar4;
  char *pcVar5;
  uint uVar6;
  int iVar7;
  ulonglong uVar8;
  
  *param_3 = '\0';
  bVar2 = false;
  uVar4 = FUN_1400277b0(param_2);
  if (((1 < uVar4) && (*param_2 == '\\')) && (param_2[1] == '\\')) {
LAB_14004423f:
    uVar6 = param_4 - 1U;
    if (uVar4 < param_4 - 1U) {
      uVar6 = uVar4;
    }
    memcpy(param_3,param_2,(ulonglong)uVar6);
    param_3[uVar6] = '\0';
    return;
  }
  uVar6 = 0;
  pcVar5 = param_2;
  if (uVar4 != 0) {
    do {
      if (*pcVar5 == ':') goto LAB_14004423f;
      uVar6 = uVar6 + 1;
      pcVar5 = pcVar5 + 1;
    } while (uVar6 < uVar4);
  }
  uVar6 = FUN_1400277b0(param_1);
  uVar8 = (ulonglong)uVar6;
  cVar3 = FUN_140043720(param_1);
  if (cVar3 != '\0') {
    bVar1 = false;
    if (uVar6 == 0) goto LAB_1400441a1;
    pcVar5 = param_1 + (uVar8 - 1);
    do {
      if ((*pcVar5 == '\\') || (*pcVar5 == '/')) break;
      pcVar5 = pcVar5 + -1;
      uVar6 = (int)uVar8 - 1;
      uVar8 = (ulonglong)uVar6;
    } while (uVar6 != 0);
  }
  bVar1 = false;
  if (((int)uVar8 != 0) && ((param_1[uVar8 - 1] == '\\' || (param_1[uVar8 - 1] == '/')))) {
    bVar1 = true;
  }
LAB_1400441a1:
  if ((*param_2 == '\\') || (*param_2 == '/')) {
    bVar2 = true;
  }
  uVar6 = (uint)uVar8;
  while ((iVar7 = param_4 + -1, uVar6 != 0 && (iVar7 != 0))) {
    cVar3 = *param_1;
    param_1 = param_1 + 1;
    if (cVar3 == '/') {
      cVar3 = '\\';
    }
    *param_3 = cVar3;
    param_3 = param_3 + 1;
    uVar6 = (int)uVar8 - 1;
    uVar8 = (ulonglong)uVar6;
    param_4 = iVar7;
  }
  if (((!bVar1) && (!bVar2)) && (iVar7 != 0)) {
    *param_3 = '\\';
    param_3 = param_3 + 1;
    iVar7 = param_4 + -2;
  }
  for (; (uVar4 != 0 && (iVar7 != 0)); iVar7 = iVar7 + -1) {
    cVar3 = *param_2;
    param_2 = param_2 + 1;
    if (cVar3 == '/') {
      cVar3 = '\\';
    }
    *param_3 = cVar3;
    param_3 = param_3 + 1;
    uVar4 = uVar4 - 1;
  }
  *param_3 = '\0';
  return;
}


/* ================= FUN_140044260 @ 140044260  size=13  callers=4 callees=0 ================= */
/* CALLERS: FUN_140040e20, FUN_140042750, FUN_14004e4b0, FUN_14004e610 */

void FUN_140044260(HKEY param_1)

{
  if (param_1 != (HKEY)0x0) {
                    /* WARNING: Could not recover jumptable at 0x000140044265. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    RegCloseKey(param_1);
    return;
  }
  return;
}


/* ================= FUN_140044270 @ 140044270  size=33  callers=4 callees=1 ================= */
/* CALLERS: FUN_140040e20, FUN_140042750, FUN_14004e4b0, FUN_14004e610 */
/* CALLEES: RegCreateKeyA */

HKEY FUN_140044270(HKEY param_1,LPCSTR param_2)

{
  LSTATUS LVar1;
  HKEY pHVar2;
  HKEY local_res18 [2];
  
  LVar1 = RegCreateKeyA(param_1,param_2,local_res18);
  pHVar2 = (HKEY)0x0;
  if (LVar1 == 0) {
    pHVar2 = local_res18[0];
  }
  return pHVar2;
}


/* ================= FUN_1400442a0 @ 1400442a0  size=132  callers=2 callees=1 ================= */
/* CALLERS: FUN_140040e20, FUN_14004e4b0 */
/* CALLEES: RegQueryValueExA */

LSTATUS FUN_1400442a0(HKEY param_1,LPCSTR param_2,DWORD *param_3,LPBYTE param_4,DWORD *param_5)

{
  LSTATUS LVar1;
  DWORD local_res8 [2];
  DWORD local_18 [4];
  
  if (param_1 != (HKEY)0x0) {
    if (param_4 == (LPBYTE)0x0) {
      LVar1 = RegQueryValueExA(param_1,param_2,(LPDWORD)0x0,local_18,(LPBYTE)0x0,(LPDWORD)0x0);
    }
    else {
      local_res8[0] = *param_5;
      LVar1 = RegQueryValueExA(param_1,param_2,(LPDWORD)0x0,local_18,param_4,local_res8);
      *param_5 = local_res8[0];
    }
    if (param_3 != (DWORD *)0x0) {
      *param_3 = local_18[0];
    }
    return LVar1;
  }
  return 1;
}


/* ================= FUN_140044330 @ 140044330  size=49  callers=2 callees=1 ================= */
/* CALLERS: FUN_140042750, FUN_14004e610 */
/* CALLEES: RegSetValueExA */

LSTATUS FUN_140044330(HKEY param_1,LPCSTR param_2,DWORD param_3,BYTE *param_4,DWORD param_5)

{
  LSTATUS LVar1;
  
  if (param_1 != (HKEY)0x0) {
    LVar1 = RegSetValueExA(param_1,param_2,0,param_3,param_4,param_5);
    return LVar1;
  }
  return 1;
}


/* ================= FUN_140044370 @ 140044370  size=58  callers=4 callees=2 ================= */
/* CALLERS: FUN_140033db0, FUN_1400395f0, FUN_14004d150, FUN_14004f7e0 */
/* CALLEES: LeaveCriticalSection, ReleaseMutex */

bool FUN_140044370(int *param_1)

{
  BOOL BVar1;
  
  if (param_1 == (int *)0x0) {
    return true;
  }
  if (*param_1 != 0) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(param_1 + 2));
    return false;
  }
  BVar1 = ReleaseMutex(*(HANDLE *)(param_1 + 2));
  return (char)BVar1 == '\0';
}


/* ================= FUN_1400443b0 @ 1400443b0  size=13  callers=7 callees=0 ================= */
/* CALLERS: FUN_140039140, FUN_140039430, FUN_14004d460, FUN_14004d5a0, FUN_14004d5e0, FUN_14004d650, FUN_14004d930 */

void FUN_1400443b0(longlong param_1)

{
  if (param_1 != 0) {
                    /* WARNING: Could not recover jumptable at 0x0001400443b5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    SetEvent();
    return;
  }
  return;
}


/* ================= FUN_1400443d0 @ 1400443d0  size=15  callers=6 callees=0 ================= */
/* CALLERS: FUN_140039430, FUN_14004db40, FUN_14004dcb0, FUN_14004dd70, FUN_14004df50, FUN_14004e1f0 */

DWORD FUN_1400443d0(HANDLE param_1,DWORD param_2)

{
  DWORD DVar1;
  
  if (param_1 != (HANDLE)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0001400443d5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    DVar1 = WaitForSingleObject(param_1,param_2);
    return DVar1;
  }
  return 0;
}


/* ================= FUN_1400443e0 @ 1400443e0  size=235  callers=4 callees=6 ================= */
/* CALLERS: FUN_140033b60, FUN_140039400, FUN_14004d020, FUN_14004f6a0 */
/* CALLEES: EnterCriticalSection, FUN_140043970, FUN_140043990, Sleep, TryEnterCriticalSection, WaitForSingleObject */

DWORD FUN_1400443e0(int *param_1,DWORD param_2)

{
  LPCRITICAL_SECTION lpCriticalSection;
  BOOL BVar1;
  int iVar2;
  DWORD DVar3;
  undefined8 uVar4;
  ulonglong uVar5;
  
  if (param_1 == (int *)0x0) {
    return 0;
  }
  lpCriticalSection = (LPCRITICAL_SECTION)(param_1 + 2);
  if (*param_1 != 0) {
    if (param_2 == 0xffffffff) {
      EnterCriticalSection(lpCriticalSection);
    }
    else {
      BVar1 = TryEnterCriticalSection(lpCriticalSection);
      if (BVar1 == 0) {
        uVar4 = FUN_140043990();
        iVar2 = TryEnterCriticalSection(lpCriticalSection);
        while( true ) {
          if (iVar2 != 0) {
            return 0;
          }
          uVar5 = FUN_140043970(uVar4);
          if (param_2 * 1000 < uVar5) break;
          if (10000 < uVar5) {
            Sleep(1);
          }
          iVar2 = TryEnterCriticalSection(lpCriticalSection);
        }
        return 0x102;
      }
    }
    return 0;
  }
  DVar3 = WaitForSingleObject(lpCriticalSection->DebugInfo,param_2);
  if (DVar3 == 0x80) {
    DVar3 = 0;
  }
  return DVar3;
}


/* ================= FUN_1400444d0 @ 1400444d0  size=30  callers=2 callees=0 ================= */
/* CALLERS: FUN_140043a00, FUN_14004fb30 */

void FUN_1400444d0(undefined8 param_1,undefined4 param_2,undefined8 param_3,char *param_4)

{
  if (((param_4 != (char *)0x0) && (*param_4 == '\\')) && (param_4[1] == '\0')) {
    param_4 = (char *)0x0;
  }
                    /* WARNING: Could not recover jumptable at 0x0001400444e7. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  _makepath_s(param_1,param_2,param_3,param_4);
  return;
}


/* ================= FUN_1400444f0 @ 1400444f0  size=39  callers=7 callees=0 ================= */
/* CALLERS: FUN_1400368e0, FUN_140043210, FUN_1400434e0, FUN_140043590, FUN_140043a00, FUN_14004fb30, FUN_140057220 */

void FUN_1400444f0(undefined8 param_1,undefined8 param_2,undefined4 param_3)

{
                    /* WARNING: Could not recover jumptable at 0x000140044510. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  _splitpath_s(param_1,param_2,param_3);
  return;
}


/* ================= FUN_140044520 @ 140044520  size=229  callers=2 callees=3 ================= */
/* CALLERS: FUN_140043e60, FUN_140043fd0 */
/* CALLEES: FUN_14002c8f0, MessageBoxA, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_140044520(uint *param_1)

{
  int iVar1;
  undefined1 auStack_238 [32];
  CHAR local_218 [512];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_238;
  if (param_1 != (uint *)0x0) {
    if (param_1[1] != 0xdeadbeef) {
      MessageBoxA((HWND)0x0,"Wrong alloc signature in memory block !"," Debug Error",0x42030);
      return 0xffffffff;
    }
    iVar1 = 0;
    do {
      iVar1 = iVar1 + 1;
      if (*(char *)((ulonglong)*param_1 + 0x20 + (longlong)param_1) != -0x32) {
        FUN_14002c8f0(local_218,0x200,"Buffer overflow in memory block. %d bytes overflowed.");
        MessageBoxA((HWND)0x0,local_218," Debug Error",0x42030);
        return 0xffffffff;
      }
    } while (iVar1 < 0x20);
  }
  return 0;
}


/* ================= FUN_140044610 @ 140044610  size=43  callers=2 callees=0 ================= */
/* CALLERS: FUN_140043e60, FUN_140043fd0 */

int FUN_140044610(longlong param_1)

{
  int iVar1;
  longlong *plVar2;
  
  iVar1 = 0;
  if (0 < DAT_14285eb98) {
    plVar2 = &DAT_14285eba8;
    do {
      if (param_1 == *plVar2) {
        return iVar1;
      }
      iVar1 = iVar1 + 1;
      plVar2 = plVar2 + 2;
    } while (iVar1 < DAT_14285eb98);
  }
  return -1;
}


/* ================= FUN_140044640 @ 140044640  size=148  callers=1 callees=2 ================= */
/* CALLERS: FUN_140044750 */
/* CALLEES: GetProcAddress, LoadLibraryA */

void FUN_140044640(undefined8 *param_1)

{
  HMODULE hModule;
  FARPROC pFVar1;
  
  hModule = LoadLibraryA("Kernel32.dll");
  *param_1 = hModule;
  if (hModule != (HMODULE)0x0) {
    pFVar1 = GetProcAddress(hModule,"GetThreadDescription");
    param_1[2] = pFVar1;
    pFVar1 = GetProcAddress(hModule,"SetThreadDescription");
    param_1[1] = pFVar1;
    pFVar1 = GetProcAddress(hModule,"CreateToolhelp32Snapshot");
    param_1[3] = pFVar1;
    pFVar1 = GetProcAddress(hModule,"Process32First");
    param_1[4] = pFVar1;
    pFVar1 = GetProcAddress(hModule,"Process32Next");
    param_1[5] = pFVar1;
  }
  return;
}


/* ================= FUN_1400446e0 @ 1400446e0  size=108  callers=1 callees=2 ================= */
/* CALLERS: FUN_140043cc0 */
/* CALLEES: GetProcAddress, LoadLibraryA */

void FUN_1400446e0(undefined8 *param_1)

{
  HMODULE hModule;
  FARPROC pFVar1;
  
  hModule = LoadLibraryA("Winmm.dll");
  *param_1 = hModule;
  if (hModule != (HMODULE)0x0) {
    pFVar1 = GetProcAddress(hModule,"timeBeginPeriod");
    param_1[1] = pFVar1;
    pFVar1 = GetProcAddress(hModule,"timeEndPeriod");
    param_1[2] = pFVar1;
    pFVar1 = GetProcAddress(hModule,"timeGetTime");
    param_1[3] = pFVar1;
  }
  return;
}


/* ================= FUN_140044750 @ 140044750  size=167  callers=1 callees=3 ================= */
/* CALLERS: FUN_140043190 */
/* CALLEES: FUN_140044640, __security_check_cookie, mbstowcs_s */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

int FUN_140044750(undefined8 param_1,char *param_2)

{
  int iVar1;
  undefined1 auStackY_158 [32];
  size_t local_128 [2];
  wchar_t local_118 [128];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStackY_158;
  if (DAT_14285ed70 == 0) {
    FUN_140044640(&DAT_14285ed70);
  }
  if (DAT_14285ed78 == (code *)0x0) {
    iVar1 = -1;
  }
  else {
    mbstowcs_s(local_128,local_118,0x40,param_2,0x40);
    iVar1 = (*DAT_14285ed78)(param_1,local_118);
    iVar1 = (-1 < iVar1) - 1;
  }
  return iVar1;
}


/* ================= FUN_140044800 @ 140044800  size=55  callers=1 callees=0 ================= */
/* CALLERS: FUN_140043660 */

void FUN_140044800(undefined2 *param_1,undefined2 *param_2)

{
  param_2[2] = param_1[3];
  param_2[3] = param_1[4];
  param_2[6] = param_1[7];
  param_2[4] = param_1[5];
  param_2[1] = param_1[1];
  param_2[5] = param_1[6];
  *param_2 = *param_1;
  return;
}


/* ================= FUN_140044840 @ 140044840  size=58  callers=2 callees=1 ================= */
/* CALLERS: FUN_14004efc0, FUN_14004f0c0 */
/* CALLEES: FUN_14002c8f0 */

void FUN_140044840(char *param_1,undefined8 param_2,undefined8 *param_3,char param_4)

{
  char cVar1;
  
  if (*param_1 == '\0') {
    cVar1 = *(char *)*param_3;
    if (cVar1 != param_4) {
      FUN_14002c8f0(param_1,param_2,"Expected \'%c\', got \'%c\'",(int)param_4,(int)cVar1);
      return;
    }
    *param_3 = (char *)*param_3 + 1;
  }
  return;
}


/* ================= FUN_140044880 @ 140044880  size=62  callers=3 callees=0 ================= */
/* CALLERS: FUN_14004f330, FUN_14004f3a0, FUN_14004f470 */

void FUN_140044880(undefined8 *param_1)

{
  char *pcVar1;
  char *pcVar2;
  char cVar3;
  
  pcVar2 = (char *)*param_1;
  cVar3 = *pcVar2;
  pcVar1 = pcVar2;
  if (cVar3 != '\n') {
    do {
      pcVar2 = pcVar1;
      if ((cVar3 == '\r') || (cVar3 == '\0')) goto LAB_1400448a8;
      cVar3 = pcVar2[1];
      pcVar1 = pcVar2 + 1;
    } while (cVar3 != '\n');
    pcVar2 = pcVar2 + 2;
  }
LAB_1400448a8:
  for (; (*pcVar2 == '\n' || (*pcVar2 == '\r')); pcVar2 = pcVar2 + 1) {
  }
  *param_1 = pcVar2;
  return;
}


/* ================= FUN_1400448c0 @ 1400448c0  size=25  callers=5 callees=0 ================= */
/* CALLERS: FUN_140044920, FUN_140044a20, FUN_14004efc0, FUN_14004f0c0, FUN_14004f470 */

void FUN_1400448c0(undefined8 *param_1)

{
  char *pcVar1;
  
  for (pcVar1 = (char *)*param_1; (*pcVar1 == ' ' || (*pcVar1 == '\t')); pcVar1 = pcVar1 + 1) {
  }
  *param_1 = pcVar1;
  return;
}


/* ================= FUN_1400448e0 @ 1400448e0  size=54  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004efc0 */
/* CALLEES: FUN_140044920 */

void FUN_1400448e0(void)

{
  undefined4 uVar1;
  undefined4 *in_R9;
  
  uVar1 = *in_R9;
  FUN_140044920();
  *in_R9 = uVar1;
  return;
}


/* ================= FUN_140044920 @ 140044920  size=251  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400448e0 */
/* CALLEES: FUN_140027140, FUN_14002c8f0, FUN_1400448c0, FUN_140044ab0 */

void FUN_140044920(char *param_1,undefined4 param_2,undefined8 *param_3,int *param_4,int param_5,
                  int param_6)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  byte *pbVar5;
  char cVar6;
  ulonglong uVar7;
  
  iVar3 = 0;
  iVar4 = 1;
  if (*param_1 != '\0') {
    return;
  }
  FUN_1400448c0(param_3);
  pbVar5 = (byte *)*param_3;
  bVar1 = *pbVar5;
  uVar7 = (ulonglong)bVar1;
  if (bVar1 != 0x2b) {
    if (bVar1 != 0x2d) {
      iVar2 = FUN_140044ab0(uVar7);
      if (iVar2 == 0) {
        FUN_140027140(param_1,"Expecting a number",param_2);
        return;
      }
      goto LAB_14004496c;
    }
    iVar4 = -1;
  }
  pbVar5 = pbVar5 + 1;
  uVar7 = (ulonglong)*pbVar5;
LAB_14004496c:
  iVar2 = FUN_140044ab0(uVar7 & 0xff);
  while (iVar2 != 0) {
    cVar6 = (char)uVar7;
    uVar7 = (ulonglong)pbVar5[1];
    pbVar5 = pbVar5 + 1;
    iVar3 = (int)cVar6 + (iVar3 * 5 + -0x18) * 2;
    iVar2 = FUN_140044ab0(uVar7);
  }
  if (iVar3 < param_5) {
    FUN_14002c8f0(param_1,param_2,"Expected a value >= %ld");
  }
  else {
    if (param_6 < iVar3) {
      iVar3 = param_6;
    }
    *param_4 = iVar4 * iVar3;
    *param_3 = pbVar5;
  }
  return;
}


/* ================= FUN_140044a20 @ 140044a20  size=138  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004f0c0 */
/* CALLEES: FUN_1400448c0 */

void FUN_140044a20(char *param_1,undefined8 *param_2,char *param_3,int param_4)

{
  char cVar1;
  char *pcVar2;
  char *pcVar3;
  
  if ((param_4 != 0) && (*param_1 == '\0')) {
    FUN_1400448c0(param_2);
    pcVar2 = (char *)*param_2;
    if (*pcVar2 == '\"') {
      while( true ) {
        param_4 = param_4 + -1;
        pcVar3 = pcVar2 + 1;
        if (((param_4 == 0) || (cVar1 = *pcVar3, cVar1 == '\0')) || (cVar1 == '\"')) break;
        *param_3 = cVar1;
        param_3 = param_3 + 1;
        pcVar2 = pcVar3;
      }
      *param_3 = '\0';
      if (*pcVar3 == '\"') {
        *param_2 = pcVar2 + 2;
        return;
      }
    }
    *param_1 = '\x01';
  }
  return;
}


/* ================= FUN_140044ab0 @ 140044ab0  size=12  callers=1 callees=0 ================= */
/* CALLERS: FUN_140044920 */

bool FUN_140044ab0(char param_1)

{
  return (byte)(param_1 - 0x30U) < 10;
}


/* ================= FUN_140044ac0 @ 140044ac0  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400460b0 */

int FUN_140044ac0(undefined8 param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = FUN_1400460b0(param_1,"Address",param_2 + 0x318);
  return (-1 < iVar1) - 1;
}


/* ================= FUN_140044af0 @ 140044af0  size=78  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400460b0 */

undefined8 FUN_140044af0(longlong param_1,longlong param_2)

{
  int iVar1;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    iVar1 = FUN_1400460b0(param_1,"DrawStyle",(undefined4 *)(param_2 + 0x348));
    if (iVar1 < 0) {
      *(undefined4 *)(param_2 + 0x348) = 0;
    }
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_140044b40 @ 140044b40  size=100  callers=1 callees=2 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_140039af0, FUN_140039c60 */

undefined8 FUN_140044b40(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,&DAT_1400755c4);
    if (lVar2 != 0) {
      iVar1 = FUN_140039c60(DAT_14285eec0,lVar2,param_2 + 0x210,0x104);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140044bb0 @ 140044bb0  size=203  callers=1 callees=3 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400460b0, FUN_140058d94, setRgb */

undefined8 FUN_140044bb0(undefined8 param_1,longlong param_2)

{
  int iVar1;
  QColor *this;
  int local_res18 [2];
  int local_res20 [2];
  int local_18 [2];
  QColor *local_10;
  
  local_18[0] = 0;
  local_res20[0] = 0;
  local_res18[0] = 0;
  iVar1 = FUN_1400460b0(param_1,"GraphColorRed",local_18);
  if (-1 < iVar1) {
    iVar1 = FUN_1400460b0(param_1,"GraphColorGreen",local_res20);
    if (-1 < iVar1) {
      iVar1 = FUN_1400460b0(param_1,"GraphColorBlue",local_res18);
      if (-1 < iVar1) {
        this = (QColor *)FUN_140058d94(0x10);
        if (this == (QColor *)0x0) {
          this = (QColor *)0x0;
        }
        else {
          local_10 = this;
          QColor::setRgb(this,local_18[0],local_res20[0],local_res18[0],0xff);
        }
        *(QColor **)(param_2 + 0x378) = this;
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140044c80 @ 140044c80  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_140045250 */

int FUN_140044c80(undefined8 param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = FUN_140045250(param_1,"IsAutoConfig",param_2 + 0x35d);
  return (-1 < iVar1) - 1;
}


/* ================= FUN_140044cb0 @ 140044cb0  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400452b0 */

int FUN_140044cb0(undefined8 param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = FUN_1400452b0(param_1,"MaxValue",param_2 + 0x3b0);
  return (-1 < iVar1) - 1;
}


/* ================= FUN_140044ce0 @ 140044ce0  size=97  callers=1 callees=2 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_140039af0, FUN_140039c60 */

undefined8 FUN_140044ce0(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,&DAT_1400756d0);
    if (lVar2 != 0) {
      iVar1 = FUN_140039c60(DAT_14285eec0,lVar2,param_2 + 4,0x100);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140044d50 @ 140044d50  size=208  callers=1 callees=4 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_140039af0, FUN_140039c60, FUN_140043d10, memset */

undefined8 FUN_140044d50(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  void *_Dst;
  undefined1 local_res18 [16];
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    local_res18[0] = 0;
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,"NameWithParents");
    if (lVar2 == 0) {
      *(longlong *)(param_2 + 0x108) = param_2 + 4;
      return 0;
    }
    iVar1 = FUN_140039c60(DAT_14285eec0,lVar2,local_res18,1);
    if (-1 < iVar1) {
      iVar1 = iVar1 + 1;
      _Dst = (void *)FUN_140043d10(iVar1);
      *(void **)(param_2 + 0x108) = _Dst;
      memset(_Dst,0,(longlong)iVar1);
      iVar1 = FUN_140039c60(DAT_14285eec0,lVar2,*(undefined8 *)(param_2 + 0x108),iVar1);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140044e20 @ 140044e20  size=78  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400460b0 */

undefined8 FUN_140044e20(longlong param_1,longlong param_2)

{
  int iVar1;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    iVar1 = FUN_1400460b0(param_1,"Offset",(undefined4 *)(param_2 + 0x38c));
    if (iVar1 < 0) {
      *(undefined4 *)(param_2 + 0x38c) = 0;
    }
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_140044e70 @ 140044e70  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_140045250 */

int FUN_140044e70(undefined8 param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = FUN_140045250(param_1,"ShowInGraph",param_2 + 0x35e);
  return (-1 < iVar1) - 1;
}


/* ================= FUN_140044ea0 @ 140044ea0  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400460b0 */

int FUN_140044ea0(undefined8 param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = FUN_1400460b0(param_1,&DAT_1400756b4,param_2 + 0x31c);
  return (-1 < iVar1) - 1;
}


/* ================= FUN_140044ed0 @ 140044ed0  size=100  callers=1 callees=2 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_140039af0, FUN_140039c60 */

undefined8 FUN_140044ed0(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,&DAT_1400756bc);
    if (lVar2 != 0) {
      iVar1 = FUN_140039c60(DAT_14285eec0,lVar2,param_2 + 0x110,0x100);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140044f40 @ 140044f40  size=447  callers=1 callees=3 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_140045250, FUN_1400460b0, strcmp */

undefined8 FUN_140044f40(undefined8 param_1,longlong param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  undefined4 uVar7;
  undefined4 local_res18 [2];
  
  iVar1 = FUN_1400460b0(param_1,"TypeID",local_res18);
  if (iVar1 == 0) {
    *(undefined4 *)(param_2 + 0x314) = local_res18[0];
    return 0;
  }
  iVar1 = FUN_140045250(param_1,"IsSigned",param_2 + 0x330);
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = *(int *)(param_2 + 0x31c);
  lVar5 = 0;
  lVar6 = lVar5;
  if (iVar1 == 1) {
    do {
      lVar4 = lVar6 + 1;
      if (*(char *)(param_2 + 0x110 + lVar6) != (&DAT_140075698)[lVar6]) goto LAB_140044ff6;
      lVar6 = lVar4;
    } while (lVar4 != 8);
    *(undefined4 *)(param_2 + 0x314) = 5;
  }
  else {
LAB_140044ff6:
    if (iVar1 == 4) {
      do {
        lVar6 = lVar5 + 1;
        if (*(char *)(param_2 + 0x110 + lVar5) != (&DAT_1400756a0)[lVar5]) goto LAB_140045061;
        lVar5 = lVar6;
      } while (lVar6 != 8);
      *(undefined4 *)(param_2 + 0x314) = 1;
    }
    else if ((iVar1 == 8) && (iVar2 = strcmp((char *)(param_2 + 0x110),"*double*"), iVar2 == 0)) {
      *(undefined4 *)(param_2 + 0x314) = 2;
    }
    else {
LAB_140045061:
      if (*(char *)(param_2 + 0x330) == '\0') {
        if (iVar1 == 1) {
          *(undefined4 *)(param_2 + 0x314) = 6;
          return 0;
        }
        if (iVar1 == 2) {
          *(undefined4 *)(param_2 + 0x314) = 7;
          return 0;
        }
        if (iVar1 == 4) {
          *(undefined4 *)(param_2 + 0x314) = 8;
          return 0;
        }
        uVar3 = 9;
      }
      else {
        if (iVar1 == 1) {
          *(undefined4 *)(param_2 + 0x314) = 10;
          return 0;
        }
        if (iVar1 == 2) {
          *(undefined4 *)(param_2 + 0x314) = 0xb;
          return 0;
        }
        if (iVar1 == 4) {
          *(undefined4 *)(param_2 + 0x314) = 0xc;
          return 0;
        }
        uVar3 = 0xd;
      }
      uVar7 = 0;
      if (iVar1 == 8) {
        uVar7 = uVar3;
      }
      *(undefined4 *)(param_2 + 0x314) = uVar7;
    }
  }
  return 0;
}


/* ================= FUN_140045100 @ 140045100  size=79  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400452b0 */

undefined8 FUN_140045100(longlong param_1,longlong param_2)

{
  int iVar1;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    iVar1 = FUN_1400452b0(param_1,"YAvgSampled",(undefined8 *)(param_2 + 0x3a8));
    if (iVar1 < 0) {
      *(undefined8 *)(param_2 + 0x3a8) = 0;
    }
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_140045150 @ 140045150  size=79  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400452b0 */

undefined8 FUN_140045150(longlong param_1,longlong param_2)

{
  int iVar1;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    iVar1 = FUN_1400452b0(param_1,"YMaxSampled",(undefined8 *)(param_2 + 0x398));
    if (iVar1 < 0) {
      *(undefined8 *)(param_2 + 0x398) = 0;
    }
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_1400451a0 @ 1400451a0  size=79  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400452b0 */

undefined8 FUN_1400451a0(longlong param_1,longlong param_2)

{
  int iVar1;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    iVar1 = FUN_1400452b0(param_1,"YMinSampled",(undefined8 *)(param_2 + 0x3a0));
    if (iVar1 < 0) {
      *(undefined8 *)(param_2 + 0x3a0) = 0;
    }
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_1400451f0 @ 1400451f0  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400452b0 */

int FUN_1400451f0(undefined8 param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = FUN_1400452b0(param_1,"YOffset",param_2 + 0x350);
  return (-1 < iVar1) - 1;
}


/* ================= FUN_140045220 @ 140045220  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045db0 */
/* CALLEES: FUN_1400460b0 */

int FUN_140045220(undefined8 param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = FUN_1400460b0(param_1,"YResolution",param_2 + 0x358);
  return (-1 < iVar1) - 1;
}


/* ================= FUN_140045250 @ 140045250  size=96  callers=6 callees=2 ================= */
/* CALLERS: FUN_140044c80, FUN_140044e70, FUN_140044f40, FUN_140045670, FUN_140045ee0, FUN_140045ff0 */
/* CALLEES: FUN_140039af0, FUN_140039ca0 */

undefined8 FUN_140045250(longlong param_1,longlong param_2,longlong param_3)

{
  int iVar1;
  longlong lVar2;
  
  if ((((DAT_14285eec0 != 0) && (param_1 != 0)) && (param_2 != 0)) && (param_3 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,param_2);
    if (lVar2 != 0) {
      iVar1 = FUN_140039ca0(DAT_14285eec0,lVar2,param_3);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_1400452b0 @ 1400452b0  size=96  callers=6 callees=2 ================= */
/* CALLERS: FUN_140044cb0, FUN_140045100, FUN_140045150, FUN_1400451a0, FUN_1400451f0, FUN_140045ee0 */
/* CALLEES: FUN_140039af0, FUN_140039cf0 */

undefined8 FUN_1400452b0(longlong param_1,longlong param_2,longlong param_3)

{
  int iVar1;
  longlong lVar2;
  
  if ((((DAT_14285eec0 != 0) && (param_1 != 0)) && (param_2 != 0)) && (param_3 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,param_2);
    if (lVar2 != 0) {
      iVar1 = FUN_140039cf0(DAT_14285eec0,lVar2,param_3);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045310 @ 140045310  size=96  callers=1 callees=2 ================= */
/* CALLERS: FUN_140045670 */
/* CALLEES: FUN_140039af0, FUN_140039e00 */

undefined8 FUN_140045310(longlong param_1,longlong param_2,longlong param_3)

{
  int iVar1;
  longlong lVar2;
  
  if ((((DAT_14285eec0 != 0) && (param_1 != 0)) && (param_2 != 0)) && (param_3 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,param_2);
    if (lVar2 != 0) {
      iVar1 = FUN_140039e00(DAT_14285eec0,lVar2,param_3);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045370 @ 140045370  size=392  callers=1 callees=8 ================= */
/* CALLERS: FUN_140045a60 */
/* CALLEES: FUN_140039d40, FUN_140045500, FUN_140045570, FUN_140045670, FUN_140045b80, FUN_140045bf0, FUN_140045c50, FUN_140045ee0 */

undefined8 FUN_140045370(undefined8 param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if (((DAT_14285eec0 != 0) && (lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"JLink"), lVar2 != 0))
     && (iVar1 = FUN_140045670(lVar2,param_2), -1 < iVar1)) {
    lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"ScriptFile");
    if ((lVar2 != 0) && (iVar1 = FUN_140045c50(lVar2,param_2), iVar1 < 0)) {
      return 0xffffffff;
    }
    lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"ElfFile");
    if (lVar2 != 0) {
      iVar1 = FUN_140045570(lVar2,param_2);
      if (-1 < iVar1) {
        lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"DataFile");
        if ((lVar2 != 0) && (iVar1 = FUN_140045500(lVar2,param_2), iVar1 < 0)) {
          return 1;
        }
        lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"RawFile");
        if ((lVar2 != 0) && (iVar1 = FUN_140045b80(lVar2,param_2), iVar1 < 0)) {
          return 1;
        }
        lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"Scope");
        if (lVar2 == 0) {
          return 0xffffffff;
        }
        iVar1 = FUN_140045bf0(lVar2,param_2);
        if (-1 < iVar1) {
          lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"Trigger");
          if ((lVar2 != 0) && (iVar1 = FUN_140045ee0(lVar2,param_2 + 0x5d0), iVar1 < 0)) {
            return 1;
          }
          return 0;
        }
      }
      return 1;
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045500 @ 140045500  size=100  callers=1 callees=2 ================= */
/* CALLERS: FUN_140045370 */
/* CALLEES: FUN_140039af0, FUN_140039c60 */

undefined8 FUN_140045500(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,&DAT_1400756d0);
    if (lVar2 != 0) {
      iVar1 = FUN_140039c60(DAT_14285eec0,lVar2,param_2 + 0x18c,0x104);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045570 @ 140045570  size=100  callers=1 callees=2 ================= */
/* CALLERS: FUN_140045370 */
/* CALLEES: FUN_140039af0, FUN_140039c60 */

undefined8 FUN_140045570(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,&DAT_1400756d0);
    if (lVar2 != 0) {
      iVar1 = FUN_140039c60(DAT_14285eec0,lVar2,param_2 + 0x88,0x104);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_1400455e0 @ 1400455e0  size=142  callers=1 callees=3 ================= */
/* CALLERS: FUN_140047020 */
/* CALLEES: FUN_14003f430, FUN_14003f640, FUN_1400460b0 */

void FUN_1400455e0(longlong param_1)

{
  int iVar1;
  uint local_res8 [2];
  uint local_res10 [6];
  
  if (param_1 != 0) {
    local_res8[0] = 0;
    local_res10[0] = 0;
    iVar1 = FUN_1400460b0(param_1,"major",local_res8);
    if (iVar1 < 0) {
      FUN_14003f430(
                   "Project file major version can not be determined!\nIt might not be compatible with this version of J-Scope."
                   );
      return;
    }
    iVar1 = FUN_1400460b0(param_1,"minor",local_res10);
    if (iVar1 < 0) {
      FUN_14003f430(
                   "Project file minor version can not be determined!\nIt might not be compatible with this version of J-Scope."
                   );
      return;
    }
    if ((1 < local_res8[0]) || ((local_res8[0] == 1 && (1 < local_res10[0])))) {
      FUN_14003f640(
                   "The project has a newer file version and might not be compatible with this version of J-Scope."
                   );
    }
  }
  return;
}


/* ================= FUN_140045670 @ 140045670  size=999  callers=1 callees=7 ================= */
/* CALLERS: FUN_140045370 */
/* CALLEES: FUN_140039af0, FUN_140039c60, FUN_140039d40, FUN_140045250, FUN_140045310, FUN_1400460b0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Type propagation algorithm not settling */

undefined8 FUN_140045670(longlong param_1,longlong param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  longlong lVar7;
  longlong lVar8;
  undefined1 auStack_68 [32];
  int local_48 [2];
  char local_40 [24];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_68;
  if (DAT_14285eec0 == 0) {
    return 0xffffffff;
  }
  if (param_1 == 0) {
    return 0xffffffff;
  }
  lVar4 = FUN_140039d40(DAT_14285eec0,param_1,"Options");
  if (lVar4 == 0) {
    return 0xffffffff;
  }
  iVar2 = FUN_140045250(lVar4,"IsRTTSession",param_2 + 0x5ac);
  if (iVar2 < 0) {
    *(undefined1 *)(param_2 + 0x5ac) = 0;
  }
  iVar2 = FUN_140045250(lVar4,"IsRTTWithTimeStamp",param_2 + 0x5ad);
  if (iVar2 < 0) {
    *(undefined1 *)(param_2 + 0x5ad) = 0;
  }
  iVar2 = FUN_140045250(lVar4,"IsRTTPlus",param_2 + 0x5ae);
  if (iVar2 < 0) {
    *(undefined1 *)(param_2 + 0x5ae) = 0;
  }
  iVar2 = FUN_1400460b0(lVar4,"SamplingPeriod",param_2 + 0x5b0);
  if (iVar2 < 0) {
    *(undefined4 *)(param_2 + 0x5b0) = 1000;
  }
  lVar6 = 0;
  local_48[0] = 0;
  iVar2 = FUN_1400460b0(lVar4,"Connection",local_48);
  if (iVar2 < 0) {
    return 0xffffffff;
  }
  if ((local_48[0] == 1) || (uVar3 = 2, local_48[0] != 2)) {
    uVar3 = 1;
  }
  *(undefined4 *)(param_2 + 0x5b4) = uVar3;
  iVar2 = FUN_140045310(lVar4,"JTAG_IRPre",param_2 + 0x5bc);
  if (iVar2 < 0) {
    *(undefined4 *)(param_2 + 0x5bc) = 0;
  }
  iVar2 = FUN_140045310(lVar4,"JTAG_DRPre",param_2 + 0x5b8);
  if (iVar2 < 0) {
    *(undefined4 *)(param_2 + 0x5b8) = 0;
  }
  iVar2 = *(int *)(param_2 + 0x5b4);
  if (iVar2 == 1) {
    iVar2 = FUN_1400460b0(lVar4,"SerialNo",param_2 + 0x59c);
    if (iVar2 < 0) {
      *(undefined4 *)(param_2 + 0x59c) = 0;
    }
    lVar5 = FUN_140039af0(DAT_14285eec0,lVar4,"Nickname");
    if (lVar5 != 0) {
      iVar2 = FUN_140039c60(DAT_14285eec0,lVar5,param_2 + 0x518,0x80);
      goto joined_r0x0001400458a3;
    }
  }
  else if (iVar2 == 2) {
    lVar5 = FUN_140039af0(DAT_14285eec0,lVar4,&DAT_140075794);
    if (lVar5 == 0) {
      return 0xffffffff;
    }
    iVar2 = FUN_140039c60(DAT_14285eec0,lVar5,param_2 + 0x498,0x80);
joined_r0x0001400458a3:
    if (iVar2 < 0) {
      return 0xffffffff;
    }
  }
  else {
    if (iVar2 != 4) {
      return 0xffffffff;
    }
    *(undefined1 *)(param_2 + 0x5c0) = 1;
  }
  lVar5 = FUN_140039af0(DAT_14285eec0,lVar4,"TargetDevice");
  if (lVar5 == 0) {
    if (*(char *)(param_2 + 0x5c0) == '\0') {
      return 0xffffffff;
    }
  }
  else {
    iVar2 = FUN_140039c60(DAT_14285eec0,lVar5,param_2 + 8,0x80);
    if (iVar2 < 0) {
      return 0xffffffff;
    }
  }
  lVar5 = FUN_140039af0(DAT_14285eec0,lVar4,"TargetInterface");
  if (lVar5 == 0) {
    cVar1 = *(char *)(param_2 + 0x5c0);
joined_r0x000140045a03:
    if (cVar1 == '\0') {
      return 0xffffffff;
    }
  }
  else {
    iVar2 = FUN_140039c60(DAT_14285eec0,lVar5,local_40,0x14);
    lVar5 = lVar6;
    if (iVar2 < 0) {
      return 0xffffffff;
    }
    do {
      lVar7 = lVar5 + 1;
      lVar8 = lVar6;
      if (local_40[lVar5] != (&DAT_14006c010)[lVar5]) goto LAB_140045970;
      lVar5 = lVar7;
    } while (lVar7 != 5);
    *(undefined4 *)(param_2 + 0x5a4) = 0;
  }
  goto LAB_140045a09;
  while (lVar8 = lVar7, lVar7 != 6) {
LAB_140045970:
    lVar7 = lVar8 + 1;
    lVar5 = lVar6;
    if (local_40[lVar8] != (&DAT_14006c024)[lVar8]) goto LAB_1400459a0;
  }
  *(undefined4 *)(param_2 + 0x5a4) = 7;
  goto LAB_140045a09;
  while (lVar5 = lVar8, lVar8 != 4) {
LAB_1400459a0:
    lVar8 = lVar5 + 1;
    if (local_40[lVar5] != (&DAT_14006c018)[lVar5]) goto LAB_1400459d0;
  }
  *(undefined4 *)(param_2 + 0x5a4) = 1;
  goto LAB_140045a09;
  while (lVar6 = lVar5, lVar5 != 5) {
LAB_1400459d0:
    lVar5 = lVar6 + 1;
    if (local_40[lVar6] != (&DAT_14006c01c)[lVar6]) {
      cVar1 = *(char *)(param_2 + 0x5c0);
      goto joined_r0x000140045a03;
    }
  }
  *(undefined4 *)(param_2 + 0x5a4) = 3;
LAB_140045a09:
  lVar6 = FUN_140039af0(DAT_14285eec0,lVar4,"InterfaceSpeed");
  if (lVar6 == 0) {
    if (*(char *)(param_2 + 0x5c0) != '\0') {
      return 0;
    }
  }
  else {
    iVar2 = FUN_140045310(lVar4,"InterfaceSpeed",param_2 + 0x5a8);
    if (-1 < iVar2) {
      return 0;
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045a60 @ 140045a60  size=276  callers=1 callees=4 ================= */
/* CALLERS: FUN_140047020 */
/* CALLEES: FUN_140039d40, FUN_140045370, FUN_140045cc0, FUN_140045ff0 */

undefined8 FUN_140045a60(undefined8 param_1,longlong param_2)

{
  char cVar1;
  char cVar2;
  longlong *plVar3;
  int iVar4;
  longlong lVar5;
  char *pcVar6;
  longlong lVar7;
  
  if ((((DAT_14285eec0 == 0) ||
       (lVar5 = FUN_140039d40(DAT_14285eec0,param_1,"Configuration"), lVar5 == 0)) ||
      (iVar4 = FUN_140045370(lVar5,param_2), iVar4 < 0)) ||
     (((lVar5 = FUN_140039d40(DAT_14285eec0,param_1,"Windows"), lVar5 == 0 ||
       (iVar4 = FUN_140045ff0(lVar5,param_2), iVar4 < 0)) ||
      ((lVar5 = FUN_140039d40(DAT_14285eec0,param_1,"SelectedSymbols"), plVar3 = DAT_14286d3b8,
       lVar5 != 0 && (iVar4 = FUN_140045cc0(lVar5), plVar3 = DAT_14286d3b8, iVar4 < 0)))))) {
    return 0xffffffff;
  }
  do {
    if (plVar3 == (longlong *)0x0) {
      return 0;
    }
    lVar5 = *plVar3;
    if (lVar5 != 0) {
      pcVar6 = (char *)(lVar5 + 4);
      lVar7 = (longlong)&DAT_14285edc0 - (longlong)pcVar6;
      do {
        cVar1 = *pcVar6;
        cVar2 = pcVar6[lVar7];
        if (cVar1 != cVar2) break;
        pcVar6 = pcVar6 + 1;
      } while (cVar2 != '\0');
      if (cVar1 == cVar2) {
        *(longlong *)(param_2 + 0x5d0) = lVar5;
      }
    }
    plVar3 = (longlong *)plVar3[1];
  } while( true );
}


/* ================= FUN_140045b80 @ 140045b80  size=100  callers=1 callees=2 ================= */
/* CALLERS: FUN_140045370 */
/* CALLEES: FUN_140039af0, FUN_140039c60 */

undefined8 FUN_140045b80(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,&DAT_1400756d0);
    if (lVar2 != 0) {
      iVar1 = FUN_140039c60(DAT_14285eec0,lVar2,param_2 + 0x290,0x104);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045bf0 @ 140045bf0  size=86  callers=1 callees=2 ================= */
/* CALLERS: FUN_140045370 */
/* CALLEES: FUN_140039d40, FUN_1400460b0 */

undefined8 FUN_140045bf0(undefined8 param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if (DAT_14285eec0 != 0) {
    lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"Options");
    if (lVar2 != 0) {
      iVar1 = FUN_1400460b0(lVar2,"TimeResolutionUs",param_2 + 0x5c8);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045c50 @ 140045c50  size=100  callers=1 callees=2 ================= */
/* CALLERS: FUN_140045370 */
/* CALLEES: FUN_140039af0, FUN_140039c60 */

undefined8 FUN_140045c50(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,&DAT_1400756d0);
    if (lVar2 != 0) {
      iVar1 = FUN_140039c60(DAT_14285eec0,lVar2,param_2 + 0x394,0x104);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045cc0 @ 140045cc0  size=135  callers=1 callees=5 ================= */
/* CALLERS: FUN_140045a60 */
/* CALLEES: FUN_140039b60, FUN_140039d40, FUN_140039e40, FUN_140045d50, thunk_FUN_140040ab0 */

undefined8 FUN_140045cc0(longlong param_1)

{
  int iVar1;
  longlong lVar2;
  undefined8 uVar3;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"Symbol");
    while( true ) {
      if (lVar2 == 0) {
        return 0;
      }
      uVar3 = thunk_FUN_140040ab0(0);
      iVar1 = FUN_140045d50(lVar2,uVar3);
      if (iVar1 < 0) break;
      uVar3 = FUN_140039e40(DAT_14285eec0,lVar2);
      lVar2 = FUN_140039b60(DAT_14285eec0,uVar3,"Symbol");
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045d50 @ 140045d50  size=96  callers=1 callees=3 ================= */
/* CALLERS: FUN_140045cc0 */
/* CALLEES: FUN_140039d40, FUN_14003df10, FUN_140045db0 */

undefined8 FUN_140045d50(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if (((DAT_14285eec0 != 0) && (param_1 != 0)) && (param_2 != 0)) {
    lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"Options");
    if (lVar2 != 0) {
      iVar1 = FUN_140045db0(lVar2,param_2);
      if (-1 < iVar1) {
        FUN_14003df10(param_2);
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045db0 @ 140045db0  size=299  callers=1 callees=18 ================= */
/* CALLERS: FUN_140045d50 */
/* CALLEES: FUN_140044ac0, FUN_140044af0, FUN_140044b40, FUN_140044bb0, FUN_140044c80, FUN_140044cb0, FUN_140044ce0, FUN_140044d50, FUN_140044e20, FUN_140044e70, FUN_140044ea0, FUN_140044ed0, FUN_140044f40, FUN_140045100, FUN_140045150, FUN_1400451a0, FUN_1400451f0, FUN_140045220 */

uint FUN_140045db0(longlong param_1,longlong param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  uint uVar17;
  uint uVar18;
  
  if ((param_1 != 0) && (param_2 != 0)) {
    uVar1 = FUN_140044ce0();
    uVar2 = FUN_140044ac0(param_1,param_2);
    uVar3 = FUN_140044ed0(param_1,param_2);
    uVar4 = FUN_140044ea0(param_1,param_2);
    uVar5 = FUN_140044f40(param_1,param_2);
    uVar6 = FUN_140044bb0(param_1,param_2);
    uVar7 = FUN_1400451a0(param_1,param_2);
    uVar8 = FUN_140045150(param_1,param_2);
    uVar9 = FUN_140045100(param_1,param_2);
    uVar10 = FUN_140044e20(param_1,param_2);
    uVar11 = FUN_140044e70(param_1,param_2);
    uVar12 = FUN_1400451f0(param_1,param_2);
    uVar13 = FUN_140044c80(param_1,param_2);
    uVar14 = FUN_140045220(param_1,param_2);
    uVar15 = FUN_140044cb0(param_1,param_2);
    uVar16 = FUN_140044b40(param_1,param_2);
    uVar17 = FUN_140044af0(param_1,param_2);
    uVar18 = FUN_140044d50(param_1,param_2);
    return uVar18 | uVar1 | uVar2 | uVar3 | uVar4 | uVar5 | uVar6 | uVar7 | uVar8 | uVar9 | uVar10 |
                    uVar11 | uVar12 | uVar13 | uVar14 | uVar15 | uVar16 | uVar17;
  }
  return 0xffffffff;
}


/* ================= FUN_140045ee0 @ 140045ee0  size=267  callers=1 callees=5 ================= */
/* CALLERS: FUN_140045370 */
/* CALLEES: FUN_140039af0, FUN_140039c60, FUN_140039d40, FUN_140045250, FUN_1400452b0 */

undefined8 FUN_140045ee0(undefined8 param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  
  if (DAT_14285eec0 != 0) {
    lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"Options");
    if (lVar2 != 0) {
      iVar1 = FUN_1400452b0(lVar2,"Level",param_2 + 8);
      if (-1 < iVar1) {
        lVar3 = FUN_140039af0(DAT_14285eec0,lVar2,"Symbol");
        if (lVar3 != 0) {
          iVar1 = FUN_140039c60(DAT_14285eec0,lVar3,&DAT_14285edc0,0x100);
          if (-1 < iVar1) {
            iVar1 = FUN_140045250(lVar2,&DAT_1400757d4,param_2 + 0x10);
            if (-1 < iVar1) {
              iVar1 = FUN_140045250(lVar2,&DAT_1400757d8,param_2 + 0x11);
              if (-1 < iVar1) {
                iVar1 = FUN_140045250(lVar2,&DAT_1400757e0,param_2 + 0x12);
                if (-1 < iVar1) {
                  iVar1 = FUN_140045250(lVar2,&DAT_1400757e8,param_2 + 0x13);
                  if (-1 < iVar1) {
                    return 0;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140045ff0 @ 140045ff0  size=183  callers=1 callees=2 ================= */
/* CALLERS: FUN_140045a60 */
/* CALLEES: FUN_140039d40, FUN_140045250 */

undefined4 FUN_140045ff0(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if (DAT_14285eec0 != 0) {
    if (param_1 != 0) {
      lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"Graph");
      if ((lVar2 != 0) && (iVar1 = FUN_140045250(lVar2,&DAT_1400756f0,param_2 + 0x5c4), iVar1 < 0))
      {
        *(undefined1 *)(param_2 + 0x5c4) = 0;
      }
      lVar2 = FUN_140039d40(DAT_14285eec0,param_1,"Watch");
      if ((lVar2 != 0) && (iVar1 = FUN_140045250(lVar2,&DAT_1400756f0,param_2 + 0x5c5), iVar1 < 0))
      {
        *(undefined1 *)(param_2 + 0x5c5) = 0;
      }
    }
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_1400460b0 @ 1400460b0  size=96  callers=10 callees=2 ================= */
/* CALLERS: FUN_140044ac0, FUN_140044af0, FUN_140044bb0, FUN_140044e20, FUN_140044ea0, FUN_140044f40, FUN_140045220, FUN_1400455e0, FUN_140045670, FUN_140045bf0 */
/* CALLEES: FUN_140039af0, FUN_140039e90 */

undefined8 FUN_1400460b0(longlong param_1,longlong param_2,longlong param_3)

{
  int iVar1;
  longlong lVar2;
  
  if ((((DAT_14285eec0 != 0) && (param_1 != 0)) && (param_2 != 0)) && (param_3 != 0)) {
    lVar2 = FUN_140039af0(DAT_14285eec0,param_1,param_2);
    if (lVar2 != 0) {
      iVar1 = FUN_140039e90(DAT_14285eec0,lVar2,param_3);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140046110 @ 140046110  size=292  callers=1 callees=9 ================= */
/* CALLERS: FUN_140046650 */
/* CALLEES: FUN_1400277b0, FUN_1400397a0, FUN_140046240, FUN_1400462b0, FUN_1400463b0, FUN_1400466e0, FUN_140046750, FUN_1400467d0, FUN_140046c50 */

undefined8 FUN_140046110(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((((DAT_14285eec0 != 0) && (param_1 != 0)) &&
      (lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"Configuration"), lVar2 != 0)) &&
     (iVar1 = FUN_1400463b0(lVar2,param_2 + 8), -1 < iVar1)) {
    iVar1 = FUN_1400277b0(param_2 + 0x394);
    if ((0 < iVar1) && (iVar1 = FUN_1400467d0(lVar2,param_2 + 8), iVar1 < 0)) {
      return 0xffffffff;
    }
    iVar1 = FUN_1400462b0(lVar2,param_2 + 8);
    if (-1 < iVar1) {
      iVar1 = FUN_1400277b0(param_2 + 0x18c);
      if ((0 < iVar1) && (iVar1 = FUN_140046240(lVar2,param_2 + 8), iVar1 < 0)) {
        return 0xffffffff;
      }
      iVar1 = FUN_1400277b0(param_2 + 0x290);
      if ((0 < iVar1) && (iVar1 = FUN_1400466e0(lVar2,param_2 + 8), iVar1 < 0)) {
        return 0xffffffff;
      }
      iVar1 = FUN_140046750(lVar2,param_2 + 0x5c4);
      if (-1 < iVar1) {
        if ((*(longlong *)(param_2 + 0x5d0) != 0) && (iVar1 = FUN_140046c50(lVar2), iVar1 < 0)) {
          return 0xffffffff;
        }
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140046240 @ 140046240  size=101  callers=1 callees=2 ================= */
/* CALLERS: FUN_140046110 */
/* CALLEES: FUN_140039600, FUN_1400397a0 */

undefined8 FUN_140046240(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"DataFile");
    if (lVar2 != 0) {
      iVar1 = FUN_140039600(DAT_14285eec0,lVar2,&DAT_1400756d0,param_2 + 0x184);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_1400462b0 @ 1400462b0  size=101  callers=1 callees=2 ================= */
/* CALLERS: FUN_140046110 */
/* CALLEES: FUN_140039600, FUN_1400397a0 */

undefined8 FUN_1400462b0(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"ElfFile");
    if (lVar2 != 0) {
      iVar1 = FUN_140039600(DAT_14285eec0,lVar2,&DAT_1400756d0,param_2 + 0x80);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140046320 @ 140046320  size=132  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400471d0 */
/* CALLEES: FUN_1400397a0, FUN_1400398d0 */

undefined8 FUN_140046320(longlong param_1)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"FileVersion");
    if (lVar2 != 0) {
      iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,"major",1);
      if (-1 < iVar1) {
        iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,"minor",1);
        if (-1 < iVar1) {
          return 0;
        }
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_1400463b0 @ 1400463b0  size=667  callers=1 callees=5 ================= */
/* CALLERS: FUN_140046110 */
/* CALLEES: FUN_140039600, FUN_140039760, FUN_1400397a0, FUN_140039850, FUN_1400398d0 */

undefined8 FUN_1400463b0(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  undefined4 uVar3;
  undefined1 *puVar4;
  
  if (DAT_14285eec0 == 0) {
    return 0xffffffff;
  }
  if (param_1 == 0) {
    return 0xffffffff;
  }
  lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"JLink");
  if (lVar2 == 0) {
    return 0xffffffff;
  }
  lVar2 = FUN_1400397a0(DAT_14285eec0,lVar2,"Options");
  if (lVar2 == 0) {
    return 0xffffffff;
  }
  uVar3 = 4;
  if (*(char *)(param_2 + 0x5b8) == '\0') {
    uVar3 = *(undefined4 *)(param_2 + 0x5ac);
  }
  iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,"Connection",uVar3);
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_140039760(DAT_14285eec0,lVar2,"IsRTTSession",*(undefined1 *)(param_2 + 0x5a4));
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_140039760(DAT_14285eec0,lVar2,"IsRTTWithTimeStamp",*(undefined1 *)(param_2 + 0x5a5));
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_140039760(DAT_14285eec0,lVar2,"IsRTTPlus",*(undefined1 *)(param_2 + 0x5a6));
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,"SamplingPeriod",*(undefined4 *)(param_2 + 0x5a8));
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,"SerialNo",*(undefined4 *)(param_2 + 0x594));
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_140039850(DAT_14285eec0,lVar2,"JTAG_IRPre",*(undefined4 *)(param_2 + 0x5b4));
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_140039850(DAT_14285eec0,lVar2,"JTAG_DRPre",*(undefined4 *)(param_2 + 0x5b0));
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_140039600(DAT_14285eec0,lVar2,&DAT_140075794,param_2 + 0x490);
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_140039600(DAT_14285eec0,lVar2,"TargetDevice",param_2);
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_140039600(DAT_14285eec0,lVar2,"Nickname",param_2 + 0x510);
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_140039850(DAT_14285eec0,lVar2,"InterfaceSpeed",*(undefined4 *)(param_2 + 0x5a0));
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = *(int *)(param_2 + 0x59c);
  if (iVar1 == 0) {
    puVar4 = &DAT_14006c010;
    goto LAB_140046614;
  }
  if (iVar1 != 1) {
    if (iVar1 == 3) {
      puVar4 = &DAT_14006c01c;
      goto LAB_140046614;
    }
    if (iVar1 == 7) {
      puVar4 = &DAT_14006c024;
      goto LAB_140046614;
    }
  }
  puVar4 = &DAT_14006c018;
LAB_140046614:
  iVar1 = FUN_140039600(DAT_14285eec0,lVar2,"TargetInterface",puVar4);
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  return 0;
}


/* ================= FUN_140046650 @ 140046650  size=136  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400471d0 */
/* CALLEES: FUN_1400397a0, FUN_140046110, FUN_140046840, FUN_140046da0 */

undefined8 FUN_140046650(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if (((DAT_14285eec0 != 0) && (param_1 != 0)) && (param_2 != 0)) {
    lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"Project");
    if (lVar2 != 0) {
      iVar1 = FUN_140046110(lVar2,param_2);
      if (-1 < iVar1) {
        iVar1 = FUN_140046da0(lVar2,param_2 + 0x5c4);
        if (-1 < iVar1) {
          iVar1 = FUN_140046840(lVar2);
          if (-1 < iVar1) {
            return 0;
          }
        }
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_1400466e0 @ 1400466e0  size=101  callers=1 callees=2 ================= */
/* CALLERS: FUN_140046110 */
/* CALLEES: FUN_140039600, FUN_1400397a0 */

undefined8 FUN_1400466e0(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"RawFile");
    if (lVar2 != 0) {
      iVar1 = FUN_140039600(DAT_14285eec0,lVar2,&DAT_1400756d0,param_2 + 0x288);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140046750 @ 140046750  size=125  callers=1 callees=2 ================= */
/* CALLERS: FUN_140046110 */
/* CALLEES: FUN_1400397a0, FUN_1400398d0 */

undefined8 FUN_140046750(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"Scope");
    if (lVar2 != 0) {
      lVar2 = FUN_1400397a0(DAT_14285eec0,lVar2,"Options");
      if (lVar2 != 0) {
        iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,"TimeResolutionUs",*(undefined4 *)(param_2 + 4));
        if (-1 < iVar1) {
          return 0;
        }
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_1400467d0 @ 1400467d0  size=101  callers=1 callees=2 ================= */
/* CALLERS: FUN_140046110 */
/* CALLEES: FUN_140039600, FUN_1400397a0 */

undefined8 FUN_1400467d0(longlong param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"ScriptFile");
    if (lVar2 != 0) {
      iVar1 = FUN_140039600(DAT_14285eec0,lVar2,&DAT_1400756d0,param_2 + 0x38c);
      if (-1 < iVar1) {
        return 0;
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140046840 @ 140046840  size=123  callers=1 callees=2 ================= */
/* CALLERS: FUN_140046650 */
/* CALLEES: FUN_1400397a0, FUN_1400468c0 */

undefined8 FUN_140046840(longlong param_1)

{
  longlong *plVar1;
  int iVar2;
  longlong lVar3;
  
  if (((DAT_14285eec0 != 0) && (param_1 != 0)) &&
     (lVar3 = FUN_1400397a0(DAT_14285eec0,param_1,"SelectedSymbols"), plVar1 = DAT_14286d3b8,
     lVar3 != 0)) {
    while( true ) {
      if (plVar1 == (longlong *)0x0) {
        return 0;
      }
      if ((*plVar1 != 0) && (iVar2 = FUN_1400468c0(lVar3), iVar2 < 0)) break;
      plVar1 = (longlong *)plVar1[1];
    }
  }
  return 0xffffffff;
}


/* ================= FUN_1400468c0 @ 1400468c0  size=911  callers=1 callees=11 ================= */
/* CALLERS: FUN_140046840 */
/* CALLEES: FUN_140039600, FUN_140039760, FUN_1400397a0, FUN_1400397b0, FUN_1400397c0, FUN_140039850, FUN_1400398d0, FUN_140039960, blue, green, red */

undefined8 FUN_1400468c0(longlong param_1,longlong param_2)

{
  QColor *this;
  int iVar1;
  longlong lVar2;
  
  if (((DAT_14285eec0 != 0) && (param_1 != 0)) && (param_2 != 0)) {
    lVar2 = FUN_1400397b0(DAT_14285eec0,param_1,"Symbol");
    if (lVar2 != 0) {
      lVar2 = FUN_1400397a0(DAT_14285eec0,lVar2,"Options");
      if ((lVar2 != 0) && (this = *(QColor **)(param_2 + 0x378), this != (QColor *)0x0)) {
        iVar1 = FUN_140039600(DAT_14285eec0,lVar2,&DAT_1400756d0,param_2 + 4);
        if (-1 < iVar1) {
          iVar1 = FUN_140039960(DAT_14285eec0,lVar2,"Address",*(undefined4 *)(param_2 + 0x318));
          if (-1 < iVar1) {
            iVar1 = FUN_140039600(DAT_14285eec0,lVar2,&DAT_1400756bc,param_2 + 0x110);
            if (-1 < iVar1) {
              iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,"TypeID",*(undefined4 *)(param_2 + 0x314));
              if (-1 < iVar1) {
                iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,&DAT_1400756b4,
                                      *(undefined4 *)(param_2 + 0x31c));
                if (-1 < iVar1) {
                  iVar1 = QColor::red(this);
                  iVar1 = FUN_140039850(DAT_14285eec0,lVar2,"GraphColorRed",iVar1);
                  if (-1 < iVar1) {
                    iVar1 = QColor::green(this);
                    iVar1 = FUN_140039850(DAT_14285eec0,lVar2,"GraphColorGreen",iVar1);
                    if (-1 < iVar1) {
                      iVar1 = QColor::blue(this);
                      iVar1 = FUN_140039850(DAT_14285eec0,lVar2,"GraphColorBlue",iVar1);
                      if (-1 < iVar1) {
                        iVar1 = FUN_1400397c0(DAT_14285eec0,lVar2,"YMinSampled",
                                              *(undefined8 *)(param_2 + 0x3a0));
                        if (-1 < iVar1) {
                          iVar1 = FUN_1400397c0(DAT_14285eec0,lVar2,"YMaxSampled",
                                                *(undefined8 *)(param_2 + 0x398));
                          if (-1 < iVar1) {
                            iVar1 = FUN_1400397c0(DAT_14285eec0,lVar2,"YAvgSampled",
                                                  *(undefined8 *)(param_2 + 0x3a8));
                            if (-1 < iVar1) {
                              iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,"Offset",
                                                    *(undefined4 *)(param_2 + 0x38c));
                              if (-1 < iVar1) {
                                iVar1 = FUN_140039760(DAT_14285eec0,lVar2,"ShowInGraph",
                                                      *(undefined1 *)(param_2 + 0x35e));
                                if (-1 < iVar1) {
                                  iVar1 = FUN_1400397c0(DAT_14285eec0,lVar2,"YOffset",
                                                        *(undefined8 *)(param_2 + 0x350));
                                  if (-1 < iVar1) {
                                    iVar1 = FUN_140039760(DAT_14285eec0,lVar2,"IsAutoConfig",
                                                          *(undefined1 *)(param_2 + 0x35d));
                                    if (-1 < iVar1) {
                                      iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,"YResolution",
                                                            *(undefined4 *)(param_2 + 0x358));
                                      if (-1 < iVar1) {
                                        iVar1 = FUN_1400397c0(DAT_14285eec0,lVar2,"MaxValue",
                                                              *(undefined8 *)(param_2 + 0x3b0));
                                        if (-1 < iVar1) {
                                          iVar1 = FUN_140039600(DAT_14285eec0,lVar2,&DAT_1400755c4,
                                                                param_2 + 0x210);
                                          if (-1 < iVar1) {
                                            iVar1 = FUN_1400398d0(DAT_14285eec0,lVar2,"DrawStyle",
                                                                  *(undefined4 *)(param_2 + 0x348));
                                            if (-1 < iVar1) {
                                              iVar1 = FUN_140039600(DAT_14285eec0,lVar2,
                                                                    "NameWithParents",
                                                                    *(undefined8 *)(param_2 + 0x108)
                                                                   );
                                              if (-1 < iVar1) {
                                                return 0;
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
  return 0xffffffff;
}


/* ================= FUN_140046c50 @ 140046c50  size=324  callers=1 callees=4 ================= */
/* CALLERS: FUN_140046110 */
/* CALLEES: FUN_140039600, FUN_140039760, FUN_1400397a0, FUN_1400397c0 */

undefined8 FUN_140046c50(longlong param_1,longlong *param_2)

{
  int iVar1;
  longlong lVar2;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"Trigger");
    if (lVar2 != 0) {
      lVar2 = FUN_1400397a0(DAT_14285eec0,lVar2,"Options");
      if (lVar2 != 0) {
        iVar1 = FUN_1400397c0(DAT_14285eec0,lVar2,"Level",param_2[1]);
        if (-1 < iVar1) {
          iVar1 = FUN_140039600(DAT_14285eec0,lVar2,"Symbol",*param_2 + 4);
          if (-1 < iVar1) {
            iVar1 = FUN_140039760(DAT_14285eec0,lVar2,&DAT_1400757d4,(char)param_2[2]);
            if (-1 < iVar1) {
              iVar1 = FUN_140039760(DAT_14285eec0,lVar2,&DAT_1400757d8,
                                    *(undefined1 *)((longlong)param_2 + 0x11));
              if (-1 < iVar1) {
                iVar1 = FUN_140039760(DAT_14285eec0,lVar2,&DAT_1400757e0,
                                      *(undefined1 *)((longlong)param_2 + 0x12));
                if (-1 < iVar1) {
                  iVar1 = FUN_140039760(DAT_14285eec0,lVar2,&DAT_1400757e8,
                                        *(undefined1 *)((longlong)param_2 + 0x13));
                  if (-1 < iVar1) {
                    return 0;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140046da0 @ 140046da0  size=212  callers=1 callees=2 ================= */
/* CALLERS: FUN_140046650 */
/* CALLEES: FUN_140039760, FUN_1400397a0 */

undefined8 FUN_140046da0(longlong param_1,undefined1 *param_2)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  
  if ((DAT_14285eec0 != 0) && (param_1 != 0)) {
    lVar2 = FUN_1400397a0(DAT_14285eec0,param_1,"Windows");
    if (lVar2 != 0) {
      lVar3 = FUN_1400397a0(DAT_14285eec0,lVar2,"Graph");
      if (lVar3 != 0) {
        iVar1 = FUN_140039760(DAT_14285eec0,lVar3,&DAT_1400756f0,*param_2);
        if (-1 < iVar1) {
          lVar2 = FUN_1400397a0(DAT_14285eec0,lVar2,"Watch");
          if (lVar2 != 0) {
            iVar1 = FUN_140039760(DAT_14285eec0,lVar2,&DAT_1400756f0,param_2[1]);
            if (-1 < iVar1) {
              return 0;
            }
          }
        }
      }
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140046e80 @ 140046e80  size=405  callers=0 callees=13 ================= */
/* CALLEES: FUN_140028380, FUN_14003ed50, FUN_14003f430, FUN_14003f640, FUN_140043d10, FUN_14004ded0, __security_check_cookie, fclose, fopen_s, fread, fseek, ftell, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_140046e80(char *param_1)

{
  longlong *plVar1;
  uint uVar2;
  int iVar3;
  undefined8 uVar4;
  void *_Dst;
  uint uVar5;
  undefined1 auStack_458 [32];
  uint local_438;
  FILE *local_428 [2];
  undefined1 local_418 [1024];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_458;
  uVar5 = 4;
  for (plVar1 = DAT_14286d3b8; plVar1 != (longlong *)0x0; plVar1 = (longlong *)plVar1[1]) {
    uVar5 = uVar5 + *(int *)(*plVar1 + 0x31c);
  }
  fopen_s(local_428,param_1,"rb");
  if (local_428[0] == (FILE *)0x0) {
    FUN_14003f430("File could not be opened!");
    uVar4 = 0xffffffff;
  }
  else {
    fseek(local_428[0],0,2);
    uVar2 = ftell(local_428[0]);
    fseek(local_428[0],0,0);
    if (uVar2 % uVar5 != 0) {
      memset(local_418,0,0x400);
      local_438 = uVar5;
      FUN_140028380(local_418,0x400,
                    "File size does not align with selected symbols!\nFile size: %d Bytes\nSize of selected symbols + timestamp: %d Bytes"
                    ,uVar2);
      FUN_14003f640(local_418);
    }
    iVar3 = FUN_14003ed50();
    if (iVar3 * 0x4000000 + 0x2000000U < uVar2) {
      FUN_14003f640("File size exceeds symbol buffers!\n Excess data will be discarded.");
    }
    _Dst = (void *)FUN_140043d10(uVar2);
    memset(_Dst,0,(ulonglong)uVar2);
    fread(_Dst,1,(ulonglong)uVar2,local_428[0]);
    FUN_14004ded0(DAT_14286d388,uVar2 - uVar2 % uVar5,_Dst);
    fclose(local_428[0]);
    uVar4 = 0;
  }
  return uVar4;
}


/* ================= FUN_140047020 @ 140047020  size=420  callers=1 callees=14 ================= */
/* CALLERS: FUN_14004eda0 */
/* CALLEES: FUN_140028380, FUN_1400399f0, FUN_140039d40, FUN_140039db0, FUN_140039eb0, FUN_14003f430, FUN_140043d10, FUN_140043e60, FUN_1400455e0, FUN_140045a60, FUN_14004e120, FUN_14004ecc0, __security_check_cookie, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined4 FUN_140047020(undefined8 param_1)

{
  int iVar1;
  void *_Dst;
  longlong lVar2;
  longlong lVar3;
  undefined4 uVar4;
  undefined1 auStack_238 [32];
  undefined1 local_218 [512];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_238;
  if (DAT_14285eec0 != 0) {
    iVar1 = FUN_1400399f0();
    if (iVar1 < 0) {
      FUN_14003f430("Failed to close project file.");
    }
    DAT_14285eec0 = 0;
  }
  _Dst = (void *)FUN_140043d10(0x5e8);
  memset(_Dst,0,0x5e8);
  DAT_14285eec0 = FUN_140039eb0(param_1);
  if (DAT_14285eec0 == 0) {
    uVar4 = 0xffffffff;
    memset(local_218,0,0x200);
    FUN_140028380(local_218,0x200,"Failed to open project file %s.",param_1);
    FUN_14003f430(local_218);
  }
  else {
    lVar2 = FUN_140039db0(DAT_14285eec0,"JScopeProject");
    if (lVar2 != 0) {
      lVar3 = FUN_140039d40(DAT_14285eec0,lVar2,"FileVersion");
      if (lVar3 != 0) {
        FUN_1400455e0(lVar3);
        lVar2 = FUN_140039d40(DAT_14285eec0,lVar2,"Project");
        if (lVar2 != 0) {
          uVar4 = FUN_140045a60(lVar2,_Dst);
          goto LAB_14004714f;
        }
      }
    }
    uVar4 = 0xffffffff;
  }
LAB_14004714f:
  FUN_14004ecc0(_Dst);
  FUN_140043e60(_Dst);
  FUN_14004e120(DAT_14286d388);
  iVar1 = FUN_1400399f0(DAT_14285eec0);
  if (iVar1 < 0) {
    FUN_14003f430("Failed to close project file.");
  }
  DAT_14285eec0 = 0;
  return uVar4;
}


/* ================= FUN_1400471d0 @ 1400471d0  size=433  callers=1 callees=14 ================= */
/* CALLERS: FUN_14004eb40 */
/* CALLEES: CloseHandle, FUN_1400399f0, FUN_140039a10, FUN_140039d90, FUN_14003a000, FUN_14003f430, FUN_140043750, FUN_140043920, FUN_140043d10, FUN_140043e60, FUN_140046320, FUN_140046650, FUN_14004e9e0, memset */

undefined8 FUN_1400471d0(undefined8 param_1)

{
  int iVar1;
  longlong lVar2;
  void *_Dst;
  HANDLE hObject;
  char *pcVar3;
  ulonglong uVar4;
  uint uVar5;
  
  if (DAT_14285eec0 != 0) {
    iVar1 = FUN_1400399f0();
    if (iVar1 < 0) {
      FUN_14003f430("Failed to close project file.");
    }
    DAT_14285eec0 = 0;
  }
  DAT_14285eec0 = FUN_140039a10("JScopeProject");
  if ((DAT_14285eec0 == 0) || (lVar2 = FUN_140039d90(DAT_14285eec0), lVar2 == 0)) {
    return 0xffffffff;
  }
  _Dst = (void *)FUN_140043d10(0x5e8);
  memset(_Dst,0,0x5e8);
  FUN_14004e9e0(_Dst);
  iVar1 = FUN_140046320(lVar2);
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  iVar1 = FUN_140046650(lVar2,_Dst);
  if (iVar1 < 0) {
    return 0xffffffff;
  }
  hObject = (HANDLE)FUN_140043750(param_1,0x12);
  if (hObject == (HANDLE)0xffffffffffffffff) {
    return 0xffffffff;
  }
  uVar5 = 0x10000;
  do {
    lVar2 = FUN_140043d10(uVar5);
    iVar1 = FUN_14003a000(DAT_14285eec0,lVar2,uVar5,0);
    uVar4 = (ulonglong)iVar1;
    if (iVar1 < 0) {
      FUN_140043e60(lVar2);
      lVar2 = 0;
      if (0x7fffffff < uVar5) goto LAB_140047316;
      uVar5 = uVar5 * 2;
    }
    else {
      *(undefined1 *)(uVar4 + lVar2) = 0;
      uVar4 = (ulonglong)(iVar1 + 1);
    }
    iVar1 = (int)uVar4;
  } while (iVar1 == -2);
  if (iVar1 < 0) {
LAB_140047316:
    if (iVar1 == -2) goto LAB_140047327;
    pcVar3 = "Failed to save XML.";
  }
  else {
    iVar1 = FUN_140043920(hObject,lVar2,uVar4 & 0xffffffff);
    if (-1 < iVar1) goto LAB_140047327;
    pcVar3 = "Failed to write XML to project file.";
  }
  FUN_14003f430(pcVar3);
LAB_140047327:
  CloseHandle(hObject);
  FUN_140043e60(_Dst);
  iVar1 = FUN_1400399f0(DAT_14285eec0);
  if (iVar1 < 0) {
    FUN_14003f430("Failed to close project file.");
  }
  DAT_14285eec0 = 0;
  return 0;
}

