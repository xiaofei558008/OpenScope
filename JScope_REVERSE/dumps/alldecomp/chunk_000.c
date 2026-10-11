
/* ================= FUN_140001000 @ 140001000  size=643  callers=0 callees=2 ================= */
/* CALLEES: QKeySequence, atexit */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140001000(void)

{
  QKeySequence::QKeySequence((QKeySequence *)&DAT_1400961a8,0x400004e,0,0,0);
  _DAT_1400961b0 = "1_SlotOnActionNewProjectDialog (void)";
  _DAT_1400961b8 = "Create a new Project";
  DAT_1400961c0 = 0;
  _DAT_1400961c8 = "&Open Project";
  QKeySequence::QKeySequence((QKeySequence *)&DAT_1400961d0,0x400004f,0,0,0);
  _DAT_1400961d8 = "1_SlotOnActionOpenProjectDialog(void)";
  _DAT_1400961e0 = "Open an existing Project";
  DAT_1400961e8 = 0;
  _DAT_1400961f0 = "&Save Project";
  QKeySequence::QKeySequence((QKeySequence *)&DAT_1400961f8,0x4000053,0,0,0);
  _DAT_140096200 = "1_SlotOnActionSaveProject (void)";
  _DAT_140096208 = "Save the current Project";
  DAT_140096210 = 0;
  _DAT_140096218 = "Save Project &as ...";
  QKeySequence::QKeySequence((QKeySequence *)&DAT_140096220,0x6000053,0,0,0);
  _DAT_140096228 = "1_SlotOnActionSaveProjectAs (void)";
  _DAT_140096230 = "Save the current Project as ...";
  DAT_140096238 = 0;
  _DAT_140096240 = "Export &Data ...";
  QKeySequence::QKeySequence((QKeySequence *)&DAT_140096248,0x4000044,0,0,0);
  _DAT_140096250 = "1_SlotOnActionExportData (void)";
  _DAT_140096258 = "Save data to csv";
  DAT_140096260 = 0;
  _DAT_140096268 = "Export &Raw ...";
  QKeySequence::QKeySequence((QKeySequence *)&DAT_140096270,0x4000052,0,0,0);
  _DAT_140096278 = "1_SlotOnActionExportRaw (void)";
  _DAT_140096280 = "Save raw data to file";
  DAT_140096288 = 0;
  _DAT_140096290 = "Impor&t Raw";
  QKeySequence::QKeySequence((QKeySequence *)&DAT_140096298,0x4000054,0,0,0);
  _DAT_1400962a0 = "1_SlotOnActionImportRaw (void)";
  _DAT_1400962a8 = "Load data from file";
  DAT_1400962b0 = 0;
  _DAT_1400962b8 = "E&xit";
  QKeySequence::QKeySequence((QKeySequence *)&DAT_1400962c0,0x8000058,0,0,0);
  _DAT_1400962c8 = "1_SlotOnActionExit (void)";
  _DAT_1400962d0 = "Quit this app";
  _DAT_1400962d8 = 0;
  atexit((_func_5014 *)&LAB_14005bab0);
  return;
}


/* ================= FUN_140001290 @ 140001290  size=172  callers=0 callees=2 ================= */
/* CALLEES: QKeySequence, atexit */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140001290(void)

{
  QKeySequence::QKeySequence((QKeySequence *)&DAT_140096368,0x1000030,0,0,0);
  _DAT_140096370 = "1_SlotOnActionUserGuide (void)";
  _DAT_140096378 = "Open the manual";
  _DAT_140096380 = 0;
  _DAT_140096388 = "&About";
  QKeySequence::QKeySequence((QKeySequence *)&DAT_140096390,0x100003b,0,0,0);
  _DAT_140096398 = "1_SlotOnActionAbout (void)";
  _DAT_1400963a0 = "Show about dialog";
  _DAT_1400963a8 = 0;
  atexit((_func_5014 *)&LAB_14005bad0);
  return;
}


/* ================= FUN_140001340 @ 140001340  size=89  callers=0 callees=2 ================= */
/* CALLEES: QKeySequence, atexit */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140001340(void)

{
  QKeySequence::QKeySequence((QKeySequence *)&DAT_140096338,0x400002c,0,0,0);
  _DAT_140096340 = "1_SlotOnActionProjectSettings(void)";
  _DAT_140096348 = "Show Project Settings";
  DAT_140096350 = 0;
  atexit((_func_5014 *)&LAB_14005baf0);
  return;
}


/* ================= FUN_1400013a0 @ 1400013a0  size=166  callers=0 callees=2 ================= */
/* CALLEES: QKeySequence, atexit */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1400013a0(void)

{
  QKeySequence::QKeySequence((QKeySequence *)&DAT_1400962e8,0,0,0,0);
  _DAT_1400962f0 = "1_SlotOnActionWatchWindow (void)";
  _DAT_1400962f8 = "Show watch window";
  DAT_140096300 = 0;
  _DAT_140096308 = "Graph window";
  QKeySequence::QKeySequence((QKeySequence *)&DAT_140096310,0,0,0,0);
  _DAT_140096318 = "1_SlotOnActionGraphWindow (void)";
  _DAT_140096320 = "Show Graph window";
  DAT_140096328 = 0;
  atexit((_func_5014 *)&LAB_14005bb10);
  return;
}


/* ================= FUN_140001450 @ 140001450  size=42  callers=0 callees=1 ================= */
/* CALLEES: setRgb */

void FUN_140001450(void)

{
  QColor::setRgb((QColor *)&DAT_14085cb20,0xff,0xa5,0,0x7f);
  return;
}


/* ================= FUN_140001480 @ 140001480  size=352  callers=0 callees=2 ================= */
/* CALLEES: invalidate, setRgb */

void FUN_140001480(void)

{
  QColor::setRgb((QColor *)&DAT_14085c960,0,0,0xff,0xff);
  QColor::setRgb((QColor *)&DAT_14085c970,0xff,0xff,0xff,0xff);
  QColor::setRgb((QColor *)&DAT_14085c980,0x80,0x80,0x80,0xff);
  QColor::setRgb((QColor *)&DAT_14085c990,0xff,0xff,0xff,0xff);
  QColor::setRgb((QColor *)&DAT_14085c9a0,0,0,0xff,0xff);
  QColor::setRgb((QColor *)&DAT_14085c9b0,0xff,0xff,0xff,0xff);
  QColor::setRgb((QColor *)&DAT_14085c9c0,0xff,0xff,0xff,0xff);
  QColor::setRgb((QColor *)&DAT_14085c9d0,0,0,0,0xff);
  QColor::invalidate((QColor *)&DAT_14085c9e0);
  QColor::invalidate((QColor *)&DAT_14085c9f0);
  QColor::setRgb((QColor *)&DAT_14085ca00,0x80,0x80,0x80,0xff);
  QColor::setRgb((QColor *)&DAT_14085ca10,0xff,0xff,0xff,0xff);
  return;
}


/* ================= FUN_1400015e0 @ 1400015e0  size=520  callers=0 callees=1 ================= */
/* CALLEES: setRgb */

void FUN_1400015e0(void)

{
  QColor::setRgb((QColor *)&DAT_14085ca20,0,0,0,0xff);
  QColor::setRgb((QColor *)&DAT_14085ca30,0x88,0,0,0xff);
  QColor::setRgb((QColor *)&DAT_14085ca40,0,0x88,0,0xff);
  QColor::setRgb((QColor *)&DAT_14085ca50,0x88,0x88,0,0xff);
  QColor::setRgb((QColor *)&DAT_14085ca60,0,0,0x88,0xff);
  QColor::setRgb((QColor *)&DAT_14085ca70,0x88,0,0x88,0xff);
  QColor::setRgb((QColor *)&DAT_14085ca80,0,0x88,0x88,0xff);
  QColor::setRgb((QColor *)&DAT_14085ca90,0xcc,0xcc,0xcc,0xff);
  QColor::setRgb((QColor *)&DAT_14085caa0,0x88,0x88,0x88,0xff);
  QColor::setRgb((QColor *)&DAT_14085cab0,0xff,0,0,0xff);
  QColor::setRgb((QColor *)&DAT_14085cac0,0,0xff,0,0xff);
  QColor::setRgb((QColor *)&DAT_14085cad0,0xff,0xff,0,0xff);
  QColor::setRgb((QColor *)&DAT_14085cae0,0,0,0xff,0xff);
  QColor::setRgb((QColor *)&DAT_14085caf0,0xff,0,0xff,0xff);
  QColor::setRgb((QColor *)&DAT_14085cb00,0,0xff,0xff,0xff);
  QColor::setRgb((QColor *)&DAT_14085cb10,0xff,0xff,0xff,0xff);
  return;
}


/* ================= FUN_140001940 @ 140001940  size=117  callers=0 callees=1 ================= */
/* CALLEES: QColor */

void FUN_140001940(void)

{
  QColor::QColor((QColor *)&DAT_14285eed0,7);
  QColor::QColor((QColor *)&DAT_14285eee0,9);
  QColor::QColor((QColor *)&DAT_14285eef0,8);
  QColor::QColor((QColor *)&DAT_14285ef00,0xc);
  QColor::QColor((QColor *)&DAT_14285ef10,10);
                    /* WARNING: Could not recover jumptable at 0x0001400019ae. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QColor::QColor((QColor *)&DAT_14285ef20,0xb);
  return;
}


/* ================= FUN_1400019c0 @ 1400019c0  size=144  callers=2 callees=1 ================= */
/* CALLERS: FUN_140001db0, FUN_14000aaf0 */
/* CALLEES: calloc */

undefined8 * FUN_1400019c0(ulonglong param_1,longlong param_2,undefined1 param_3)

{
  undefined8 *puVar1;
  void *pvVar2;
  longlong lVar3;
  
  puVar1 = calloc(1,0x28);
  if (param_1 == 0) {
    param_1 = 0x3fd;
  }
  else if (param_1 < 4) {
    param_1 = 4;
  }
  pvVar2 = calloc(0x18,param_1);
  puVar1[1] = param_1;
  *puVar1 = pvVar2;
  lVar3 = 10;
  if (param_2 != 0) {
    lVar3 = param_2;
  }
  *(undefined1 *)(puVar1 + 4) = param_3;
  puVar1[2] = lVar3;
  puVar1[3] = 0;
  return puVar1;
}


/* ================= FUN_140001a50 @ 140001a50  size=107  callers=1 callees=1 ================= */
/* CALLERS: FUN_140003a70 */
/* CALLEES: free */

void FUN_140001a50(longlong *param_1)

{
  ulonglong uVar1;
  ulonglong uVar2;
  
  if (param_1 != (longlong *)0x0) {
    uVar1 = 0;
    uVar2 = uVar1;
    if (param_1[1] != 0) {
      do {
        free(*(void **)(uVar2 + *param_1));
        free(*(void **)(uVar2 + 8 + *param_1));
        uVar1 = uVar1 + 1;
        uVar2 = uVar2 + 0x18;
      } while (uVar1 < (ulonglong)param_1[1]);
    }
    free((void *)*param_1);
    free(param_1);
  }
  return;
}


/* ================= FUN_140001ac0 @ 140001ac0  size=164  callers=1 callees=0 ================= */
/* CALLERS: FUN_140001db0 */

longlong FUN_140001ac0(ulonglong param_1)

{
  longlong lVar1;
  double dVar2;
  
  if (param_1 < 100) {
    return 0x61;
  }
  if (param_1 < 1000) {
    return 0x3e5;
  }
  if (param_1 < 10000) {
    return 0x26ef;
  }
  if (param_1 < 100000) {
    return 0x18697;
  }
  if (param_1 < 1000000) {
    return 0xf422f;
  }
  dVar2 = (double)param_1 * 1.33333;
  lVar1 = 0;
  if ((9.223372036854776e+18 <= dVar2) &&
     (dVar2 = dVar2 - 9.223372036854776e+18, dVar2 < 9.223372036854776e+18)) {
    lVar1 = -0x8000000000000000;
  }
  return (longlong)dVar2 + lVar1;
}


/* ================= FUN_140001b70 @ 140001b70  size=227  callers=2 callees=0 ================= */
/* CALLERS: FUN_140004aa0, FUN_140004af0 */

undefined8 FUN_140001b70(longlong *param_1,ulonglong param_2)

{
  undefined8 *puVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  ulonglong *puVar4;
  ulonglong uVar5;
  ulonglong uVar6;
  ulonglong uVar7;
  
  if ((param_1 != (longlong *)0x0) && (param_1[3] != 0)) {
    uVar6 = 0;
    uVar2 = param_1[1];
    if (uVar2 == 0) {
      return 0;
    }
    uVar7 = param_2;
    while( true ) {
      uVar7 = uVar7 % uVar2;
      uVar5 = 0;
      uVar3 = *(ulonglong *)(*param_1 + 0x10 + uVar7 * 0x18);
      puVar1 = (undefined8 *)(*param_1 + uVar7 * 0x18);
      if (uVar3 != 0) {
        puVar4 = (ulonglong *)*puVar1;
        do {
          if (*puVar4 == param_2) {
            return *(undefined8 *)(puVar1[1] + uVar5 * 8);
          }
          uVar5 = uVar5 + 1;
          puVar4 = puVar4 + 1;
        } while (uVar5 < uVar3);
      }
      if (uVar3 < (ulonglong)param_1[2]) break;
      uVar7 = param_2 % (uVar2 - 2) + 1 + uVar7;
      uVar6 = uVar6 + 1;
      if (uVar2 <= uVar6) {
        return 0;
      }
    }
    return 0;
  }
  return 0;
}


/* ================= FUN_140001c60 @ 140001c60  size=332  callers=2 callees=2 ================= */
/* CALLERS: FUN_140001db0, FUN_14000e0d0 */
/* CALLEES: FUN_140001db0, realloc */

void FUN_140001c60(longlong *param_1,undefined8 param_2,ulonglong param_3)

{
  size_t _NewSize;
  longlong lVar1;
  longlong *plVar2;
  ulonglong uVar3;
  longlong lVar4;
  ulonglong uVar5;
  ulonglong *puVar6;
  void *pvVar7;
  ulonglong uVar8;
  ulonglong uVar9;
  ulonglong uVar10;
  
  if (param_1 == (longlong *)0x0) {
    return;
  }
  if ((ulonglong)param_1[3] <= (ulonglong)(param_1[1] * param_1[2]) &&
      param_1[1] * param_1[2] - param_1[3] != 0) goto LAB_140001ca0;
  do {
    do {
      FUN_140001db0(param_1);
LAB_140001ca0:
      uVar3 = param_1[1];
      uVar8 = 0;
    } while (uVar3 == 0);
    lVar4 = *param_1;
    uVar10 = param_3;
    do {
      uVar10 = uVar10 % uVar3;
      uVar5 = *(ulonglong *)(lVar4 + 0x10 + uVar10 * 0x18);
      if (((char)param_1[4] == '\0') && (uVar9 = 0, uVar5 != 0)) {
        puVar6 = *(ulonglong **)(lVar4 + uVar10 * 0x18);
        do {
          if (*puVar6 == param_3) {
            *(undefined8 *)(*(longlong *)(lVar4 + 8 + uVar10 * 0x18) + uVar9 * 8) = param_2;
            return;
          }
          uVar9 = uVar9 + 1;
          puVar6 = puVar6 + 1;
        } while (uVar9 < uVar5);
      }
      if (uVar5 < (ulonglong)param_1[2]) {
        lVar1 = uVar10 * 0x18;
        _NewSize = uVar5 * 8 + 8;
        pvVar7 = realloc(*(void **)(lVar1 + lVar4),_NewSize);
        *(void **)(lVar1 + *param_1) = pvVar7;
        pvVar7 = realloc(*(void **)(*param_1 + 8 + lVar1),_NewSize);
        *(void **)(*param_1 + 8 + lVar1) = pvVar7;
        *(ulonglong *)(*(longlong *)(*param_1 + lVar1) + uVar5 * 8) = param_3;
        *(undefined8 *)(*(longlong *)(*param_1 + 8 + lVar1) + uVar5 * 8) = param_2;
        plVar2 = (longlong *)(*param_1 + 0x10 + lVar1);
        *plVar2 = *plVar2 + 1;
        param_1[3] = param_1[3] + 1;
        return;
      }
      uVar10 = param_3 % (uVar3 - 2) + 1 + uVar10;
      uVar8 = uVar8 + 1;
    } while (uVar8 < uVar3);
  } while( true );
}


/* ================= FUN_140001db0 @ 140001db0  size=307  callers=1 callees=4 ================= */
/* CALLERS: FUN_140001c60 */
/* CALLEES: FUN_1400019c0, FUN_140001ac0, FUN_140001c60, free */

void FUN_140001db0(longlong *param_1)

{
  ulonglong uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  longlong *_Memory;
  longlong lVar5;
  ulonglong uVar6;
  ulonglong uVar7;
  longlong lVar8;
  
  if (param_1 != (longlong *)0x0) {
    lVar8 = param_1[1];
    uVar4 = FUN_140001ac0(lVar8 * 2);
    _Memory = (longlong *)FUN_1400019c0(uVar4,param_1[2],(char)param_1[4]);
    if (lVar8 != 0) {
      uVar7 = 0;
      do {
        lVar5 = *param_1;
        uVar1 = *(ulonglong *)(uVar7 + 0x10 + lVar5);
        uVar6 = 0;
        if (uVar1 != 0) {
          do {
            if (*(longlong *)(*(longlong *)(uVar7 + lVar5) + uVar6 * 8) != -0x1c00c00de00f061) {
              FUN_140001c60(_Memory,*(undefined8 *)(*(longlong *)(uVar7 + lVar5 + 8) + uVar6 * 8));
              lVar5 = *param_1;
            }
            uVar6 = uVar6 + 1;
          } while (uVar6 < uVar1);
        }
        free(*(void **)(uVar7 + 8 + lVar5));
        free(*(void **)(uVar7 + *param_1));
        *(undefined8 *)(uVar7 + 8 + *param_1) = 0;
        *(undefined8 *)(uVar7 + *param_1) = 0;
        uVar7 = uVar7 + 0x18;
        lVar8 = lVar8 + -1;
      } while (lVar8 != 0);
    }
    free((void *)*param_1);
    lVar8 = _Memory[1];
    *param_1 = *_Memory;
    param_1[1] = lVar8;
    uVar2 = *(undefined4 *)((longlong)_Memory + 0x14);
    lVar8 = _Memory[3];
    uVar3 = *(undefined4 *)((longlong)_Memory + 0x1c);
    *(int *)(param_1 + 2) = (int)_Memory[2];
    *(undefined4 *)((longlong)param_1 + 0x14) = uVar2;
    *(int *)(param_1 + 3) = (int)lVar8;
    *(undefined4 *)((longlong)param_1 + 0x1c) = uVar3;
    param_1[4] = _Memory[4];
    free(_Memory);
  }
  return;
}


/* ================= FUN_140001ef0 @ 140001ef0  size=110  callers=4 callees=2 ================= */
/* CALLERS: FUN_140009400, FUN_1400094f0, FUN_14000c6f0, FUN_14000d6d0 */
/* CALLEES: FUN_1400025b0, memcpy */

undefined8 FUN_140001ef0(longlong *param_1,void *param_2)

{
  int iVar1;
  ulonglong _Size;
  
  iVar1 = FUN_1400025b0();
  if (iVar1 != 0) {
    return 0xffffffff;
  }
  _Size = (ulonglong)*(uint *)((longlong)param_1 + 0x24);
  memcpy((void *)(_Size * ((ulonglong)param_1[2] % (*(uint *)(param_1 + 4) / _Size)) +
                 *(longlong *)
                  (*param_1 + ((ulonglong)param_1[2] / (*(uint *)(param_1 + 4) / _Size)) * 8)),
         param_2,_Size);
  param_1[2] = param_1[2] + 1;
  return 0;
}


/* ================= FUN_140001f60 @ 140001f60  size=99  callers=7 callees=2 ================= */
/* CALLERS: FUN_140009620, FUN_1400096a0, FUN_140009850, FUN_140009b80, FUN_140009bd0, FUN_140009c40, FUN_14000aaf0 */
/* CALLEES: FUN_1400025b0, memcpy */

undefined8 FUN_140001f60(longlong *param_1,undefined8 param_2)

{
  int iVar1;
  ulonglong _Size;
  undefined8 local_res10 [3];
  
  local_res10[0] = param_2;
  iVar1 = FUN_1400025b0();
  if (iVar1 != 0) {
    return 0xffffffff;
  }
  _Size = (ulonglong)*(uint *)((longlong)param_1 + 0x24);
  memcpy((void *)(_Size * ((ulonglong)param_1[2] % (*(uint *)(param_1 + 4) / _Size)) +
                 *(longlong *)
                  (*param_1 + ((ulonglong)param_1[2] / (*(uint *)(param_1 + 4) / _Size)) * 8)),
         local_res10,_Size);
  param_1[2] = param_1[2] + 1;
  return 0;
}


/* ================= FUN_140001fd0 @ 140001fd0  size=103  callers=0 callees=1 ================= */
/* CALLEES: free */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140001fd0(longlong *param_1)

{
  ulonglong uVar1;
  uint uVar2;
  
  if (param_1 != (longlong *)0x0) {
    uVar1 = 0;
    if (*(int *)((longlong)param_1 + 0x1c) != 0) {
      do {
        free(*(void **)(*param_1 + uVar1 * 8));
        uVar2 = (int)uVar1 + 1;
        uVar1 = (ulonglong)uVar2;
        _DAT_140097bd8 = _DAT_140097bd8 + (ulonglong)*(uint *)(param_1 + 4);
      } while (uVar2 < *(uint *)((longlong)param_1 + 0x1c));
    }
    free((void *)*param_1);
    *param_1 = 0;
    *(undefined4 *)((longlong)param_1 + 0x1c) = 0;
    param_1[2] = 0;
    param_1[1] = 0;
  }
  return;
}


/* ================= FUN_140002040 @ 140002040  size=5  callers=2 callees=0 ================= */
/* CALLERS: FUN_140004b40, FUN_140008f40 */

undefined8 FUN_140002040(longlong param_1)

{
  return *(undefined8 *)(param_1 + 0x10);
}


/* ================= FUN_140002050 @ 140002050  size=340  callers=5 callees=1 ================= */
/* CALLERS: FUN_140005df0, FUN_140006920, FUN_140009400, FUN_1400094f0, FUN_14000dd10 */
/* CALLEES: memcpy */

longlong FUN_140002050(longlong *param_1,longlong param_2,longlong param_3,longlong param_4)

{
  ulonglong uVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  ulonglong uVar5;
  longlong lVar6;
  void *_Src;
  size_t _Size;
  longlong lVar7;
  ulonglong uVar8;
  
  if (param_3 != 0) {
    uVar5 = (ulonglong)*(uint *)((longlong)param_1 + 0x24);
    lVar7 = 0;
    uVar1 = *(uint *)(param_1 + 4) / uVar5;
    uVar2 = param_2 / (longlong)uVar1;
    lVar6 = param_2 % (longlong)uVar1;
    uVar3 = param_3 + -1 + param_2;
    uVar4 = uVar3 / uVar1;
    if (((longlong)uVar2 < (longlong)(ulonglong)*(uint *)((longlong)param_1 + 0x1c)) &&
       ((longlong)uVar4 < (longlong)(ulonglong)*(uint *)((longlong)param_1 + 0x1c))) {
      uVar8 = uVar2;
      if ((longlong)uVar2 <= (longlong)uVar4) {
        do {
          if (uVar8 == uVar2) {
            if (uVar8 == uVar4) {
              _Size = (ulonglong)*(uint *)((longlong)param_1 + 0x24) * param_3;
              _Src = (void *)((ulonglong)*(uint *)((longlong)param_1 + 0x24) * lVar6 +
                             *(longlong *)(*param_1 + uVar8 * 8));
            }
            else {
              uVar5 = (ulonglong)*(uint *)((longlong)param_1 + 0x24);
              _Size = (*(uint *)(param_1 + 4) / uVar5 - lVar6) * uVar5;
              _Src = (void *)(uVar5 * lVar6 + *(longlong *)(*param_1 + uVar8 * 8));
            }
          }
          else {
            if (uVar8 == uVar4) {
              _Size = (uVar3 % uVar1 + 1) * (ulonglong)*(uint *)((longlong)param_1 + 0x24);
            }
            else {
              _Size = (size_t)(*(uint *)(param_1 + 4) -
                              *(uint *)(param_1 + 4) % *(uint *)((longlong)param_1 + 0x24));
            }
            _Src = *(void **)(*param_1 + uVar8 * 8);
          }
          memcpy((void *)(param_4 + lVar7),_Src,_Size);
          lVar7 = lVar7 + _Size;
          uVar8 = uVar8 + 1;
        } while ((longlong)uVar8 <= (longlong)uVar4);
        uVar5 = (ulonglong)*(uint *)((longlong)param_1 + 0x24);
      }
      return lVar7 / (longlong)uVar5;
    }
  }
  return 0;
}


/* ================= FUN_1400021c0 @ 1400021c0  size=55  callers=2 callees=0 ================= */
/* CALLERS: FUN_1400025e0, FUN_140006920 */

longlong FUN_1400021c0(longlong *param_1,ulonglong param_2)

{
  ulonglong uVar1;
  
  if ((-1 < (longlong)param_2) && ((longlong)param_2 < param_1[2])) {
    uVar1 = (ulonglong)*(uint *)(param_1 + 4) / (ulonglong)*(uint *)((longlong)param_1 + 0x24);
    return (ulonglong)*(uint *)((longlong)param_1 + 0x24) * (param_2 % uVar1) +
           *(longlong *)(*param_1 + (param_2 / uVar1) * 8);
  }
  return 0;
}


/* ================= FUN_140002200 @ 140002200  size=9  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000d820 */
/* CALLEES: FUN_1400025e0 */

void FUN_140002200(longlong param_1,undefined8 param_2)

{
  FUN_1400025e0(param_1,param_2,*(undefined8 *)(param_1 + 0x28));
  return;
}


/* ================= FUN_140002210 @ 140002210  size=62  callers=3 callees=0 ================= */
/* CALLERS: FUN_140004b40, FUN_140006730, FUN_14000d820 */

undefined8 FUN_140002210(longlong *param_1,ulonglong param_2)

{
  ulonglong uVar1;
  
  if ((-1 < (longlong)param_2) && ((longlong)param_2 < param_1[2])) {
    uVar1 = (ulonglong)*(uint *)(param_1 + 4) / (ulonglong)*(uint *)((longlong)param_1 + 0x24);
    return *(undefined8 *)
            ((ulonglong)*(uint *)((longlong)param_1 + 0x24) * (param_2 % uVar1) +
            *(longlong *)(*param_1 + (param_2 / uVar1) * 8));
  }
  return 0;
}


/* ================= FUN_140002250 @ 140002250  size=105  callers=3 callees=1 ================= */
/* CALLERS: FUN_140009400, FUN_1400094f0, FUN_14000aaf0 */
/* CALLEES: FUN_140002780 */

void FUN_140002250(undefined8 *param_1,undefined4 param_2,int param_3,undefined1 param_4,
                  longlong param_5,undefined8 param_6)

{
  undefined8 uVar1;
  int iVar2;
  
  iVar2 = 1;
  if (param_3 != 0) {
    iVar2 = param_3;
  }
  param_1[1] = 0;
  param_1[2] = 0;
  *(undefined4 *)((longlong)param_1 + 0x1c) = 0;
  *param_1 = 0;
  *(undefined4 *)(param_1 + 4) = param_2;
  *(int *)((longlong)param_1 + 0x24) = iVar2;
  *(undefined1 *)(param_1 + 3) = param_4;
  if (param_5 != 0) {
    param_1[5] = param_5;
    param_1[6] = param_6;
    return;
  }
  uVar1 = FUN_140002780(iVar2,param_4);
  param_1[5] = uVar1;
  param_1[6] = &DAT_140097be0;
  return;
}


/* ================= FUN_1400022c0 @ 1400022c0  size=11  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400094f0 */
/* CALLEES: FUN_1400022d0 */

void FUN_1400022c0(undefined8 param_1,undefined8 param_2)

{
  FUN_1400022d0(param_1,param_2,1);
  return;
}


/* ================= FUN_1400022d0 @ 1400022d0  size=199  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400022c0 */
/* CALLEES: memcpy */

undefined8 FUN_1400022d0(longlong *param_1,ulonglong param_2,longlong param_3)

{
  uint uVar1;
  ulonglong uVar2;
  longlong lVar3;
  
  if ((-1 < (longlong)param_2) && (lVar3 = param_1[2], (longlong)(param_2 + param_3) <= lVar3)) {
    if (param_2 < (ulonglong)(lVar3 - param_3)) {
      do {
        uVar1 = *(uint *)((longlong)param_1 + 0x24);
        uVar2 = (ulonglong)*(uint *)(param_1 + 4) / (ulonglong)uVar1;
        memcpy((void *)((ulonglong)uVar1 * (param_2 % uVar2) +
                       *(longlong *)(*param_1 + (param_2 / uVar2) * 8)),
               (void *)((ulonglong)uVar1 * ((param_2 + param_3) % uVar2) +
                       *(longlong *)(*param_1 + ((param_2 + param_3) / uVar2) * 8)),(ulonglong)uVar1
              );
        lVar3 = param_1[2];
        param_2 = param_2 + 1;
      } while (param_2 < (ulonglong)(lVar3 - param_3));
    }
    param_1[2] = lVar3 - param_3;
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_1400023a0 @ 1400023a0  size=55  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400025b0, FUN_14000aaf0 */
/* CALLEES: FUN_140002440 */

undefined8 FUN_1400023a0(longlong param_1,ulonglong param_2)

{
  ulonglong uVar1;
  undefined8 uVar2;
  int iVar3;
  
  uVar1 = (ulonglong)*(uint *)(param_1 + 0x20) / (ulonglong)*(uint *)(param_1 + 0x24);
  iVar3 = ((uint)(param_2 % uVar1 != 0) - *(int *)(param_1 + 0x1c)) + (int)(param_2 / uVar1);
  if (iVar3 < 1) {
    return 0;
  }
  uVar2 = FUN_140002440(param_1,iVar3);
  return uVar2;
}


/* ================= FUN_1400023e0 @ 1400023e0  size=83  callers=2 callees=1 ================= */
/* CALLERS: FUN_140006920, FUN_14000dd10 */
/* CALLEES: memcpy */

undefined8 FUN_1400023e0(longlong *param_1,ulonglong param_2,void *param_3)

{
  ulonglong _Size;
  
  if ((-1 < (longlong)param_2) && ((longlong)param_2 < param_1[2])) {
    _Size = (ulonglong)*(uint *)((longlong)param_1 + 0x24);
    memcpy((void *)(_Size * (param_2 % (*(uint *)(param_1 + 4) / _Size)) +
                   *(longlong *)(*param_1 + (param_2 / (*(uint *)(param_1 + 4) / _Size)) * 8)),
           param_3,_Size);
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_140002440 @ 140002440  size=203  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400023a0 */
/* CALLEES: calloc, realloc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_140002440(longlong *param_1,uint param_2)

{
  void *pvVar1;
  size_t _Size;
  uint uVar2;
  
  if (param_2 == 0) {
    return 0;
  }
  if (*(int *)((longlong)param_1 + 0x24) != 0) {
    pvVar1 = realloc((void *)*param_1,(ulonglong)(*(int *)((longlong)param_1 + 0x1c) + param_2) << 3
                    );
    uVar2 = 0;
    *param_1 = (longlong)pvVar1;
    if (pvVar1 != (void *)0x0) {
      if (param_2 != 0) {
        _Size = (size_t)*(uint *)(param_1 + 4);
        do {
          pvVar1 = calloc(1,_Size);
          *(void **)(*param_1 + (ulonglong)(*(int *)((longlong)param_1 + 0x1c) + uVar2) * 8) =
               pvVar1;
          _Size = (size_t)*(uint *)(param_1 + 4);
          _DAT_140097bd0 = _DAT_140097bd0 + _Size;
          if (*(longlong *)(*param_1 + (ulonglong)(*(int *)((longlong)param_1 + 0x1c) + uVar2) * 8)
              == 0) {
            return 0xffffffff;
          }
          uVar2 = uVar2 + 1;
        } while (uVar2 < param_2);
      }
      *(int *)((longlong)param_1 + 0x1c) = *(int *)((longlong)param_1 + 0x1c) + param_2;
      param_1[1] = (ulonglong)
                   ((*(uint *)(param_1 + 4) / *(uint *)((longlong)param_1 + 0x24)) *
                   *(int *)((longlong)param_1 + 0x1c));
      return 0;
    }
    *(undefined4 *)((longlong)param_1 + 0x1c) = 0;
    param_1[1] = 0;
  }
  return 0xffffffff;
}


/* ================= FUN_1400025b0 @ 1400025b0  size=36  callers=2 callees=1 ================= */
/* CALLERS: FUN_140001ef0, FUN_140001f60 */
/* CALLEES: FUN_1400023a0 */

undefined8 FUN_1400025b0(longlong param_1,ulonglong param_2)

{
  int iVar1;
  
  if (*(ulonglong *)(param_1 + 8) < param_2) {
    iVar1 = FUN_1400023a0();
    if (iVar1 != 0) {
      return 0xffffffff;
    }
  }
  return 0;
}


/* ================= FUN_1400025e0 @ 1400025e0  size=405  callers=1 callees=2 ================= */
/* CALLERS: FUN_140002200 */
/* CALLEES: FUN_1400021c0, bsearch */

longlong FUN_1400025e0(longlong *param_1,void *param_2,_PtFuncCompare *param_3)

{
  longlong lVar1;
  int iVar2;
  void *pvVar3;
  ulonglong uVar4;
  longlong lVar5;
  int iVar6;
  uint uVar7;
  ulonglong uVar8;
  ulonglong uVar9;
  
  if (((param_3 != (_PtFuncCompare *)0x0) && (param_1[6] != 0)) &&
     (*(int *)((longlong)param_1 + 0x1c) != 0)) {
    uVar8 = 0;
    if (*(int *)((longlong)param_1 + 0x1c) != 1) {
      do {
        uVar4 = (ulonglong)*(uint *)(param_1 + 4) / (ulonglong)*(uint *)((longlong)param_1 + 0x24);
        iVar6 = (int)uVar8;
        uVar9 = (ulonglong)(uint)((int)uVar4 * iVar6);
        iVar2 = (*param_3)(param_2,(void *)((ulonglong)
                                            (uint)((int)(uVar9 % uVar4) *
                                                  *(int *)((longlong)param_1 + 0x24)) +
                                           *(longlong *)(*param_1 + (uVar9 / uVar4) * 8)));
        if (-1 < iVar2) {
          uVar4 = (ulonglong)*(uint *)(param_1 + 4) / (ulonglong)*(uint *)((longlong)param_1 + 0x24)
          ;
          uVar9 = (ulonglong)(uint)((iVar6 + 1) * (int)uVar4);
          iVar2 = (*param_3)(param_2,(void *)((ulonglong)
                                              (uint)((int)(uVar9 % uVar4) *
                                                    *(int *)((longlong)param_1 + 0x24)) +
                                             *(longlong *)(*param_1 + (uVar9 / uVar4) * 8)));
          if (iVar2 < 0) break;
        }
        uVar8 = (ulonglong)(iVar6 + 1U);
      } while (iVar6 + 1U < *(int *)((longlong)param_1 + 0x1c) - 1U);
    }
    uVar7 = (uint)uVar8;
    if (uVar7 < *(uint *)((longlong)param_1 + 0x1c)) {
      *(undefined8 *)param_1[6] = 0;
      uVar9 = (ulonglong)*(uint *)(param_1 + 4) / (ulonglong)*(uint *)((longlong)param_1 + 0x24);
      uVar4 = param_1[2] - (ulonglong)((int)uVar9 * uVar7);
      if (uVar4 <= uVar9) {
        uVar9 = uVar4;
      }
      pvVar3 = bsearch(param_2,*(void **)(uVar8 * 8 + *param_1),uVar9,
                       (ulonglong)*(uint *)((longlong)param_1 + 0x24),param_3);
      if ((pvVar3 != (void *)0x0) || (pvVar3 = *(void **)param_1[6], pvVar3 != (void *)0x0)) {
        lVar5 = ((longlong)pvVar3 - *(longlong *)(uVar8 * 8 + *param_1)) /
                (longlong)(ulonglong)*(uint *)((longlong)param_1 + 0x24) +
                (ulonglong)((*(uint *)(param_1 + 4) / *(uint *)((longlong)param_1 + 0x24)) * uVar7);
        if (lVar5 < 0) {
          return 0;
        }
        lVar1 = param_1[2];
        if (lVar1 <= lVar5) {
          return lVar1 + -1;
        }
        if (lVar1 + -1 <= lVar5) {
          return lVar5;
        }
        pvVar3 = (void *)FUN_1400021c0(param_1,lVar5 + 1);
        iVar2 = (*param_3)(pvVar3,param_2);
        if (iVar2 != 0) {
          return lVar5;
        }
        return lVar5 + 1;
      }
    }
  }
  return -1;
}


/* ================= FUN_140002780 @ 140002780  size=59  callers=1 callees=0 ================= */
/* CALLERS: FUN_140002250 */

undefined1 * FUN_140002780(int param_1,char param_2)

{
  if (param_2 == '\0') {
    if (param_1 == 4) {
      return &LAB_140002570;
    }
    if (param_1 == 8) {
      return &LAB_140002590;
    }
  }
  else {
    if (param_1 == 4) {
      return &LAB_140002510;
    }
    if (param_1 == 8) {
      return &LAB_140002540;
    }
  }
  return (undefined1 *)0x0;
}


/* ================= FUN_1400027c0 @ 1400027c0  size=86  callers=3 callees=1 ================= */
/* CALLERS: FUN_1400097d0, FUN_140009970, FUN_140009af0 */
/* CALLEES: FUN_140002880 */

void FUN_1400027c0(longlong *param_1,undefined8 param_2)

{
  ulonglong uVar1;
  ulonglong uVar2;
  bool bVar3;
  bool bVar4;
  
  if (param_1 != (longlong *)0x0) {
    uVar1 = param_1[1];
    uVar2 = param_1[2];
    bVar3 = uVar1 < uVar2;
    bVar4 = false;
    if (uVar1 == uVar2) {
      FUN_140002880(param_1,uVar1 * 2);
      uVar2 = param_1[2];
      bVar3 = (ulonglong)param_1[1] < uVar2;
      bVar4 = param_1[1] == uVar2;
    }
    if ((!bVar3 && !bVar4) && (*param_1 != 0)) {
      *(undefined8 *)(*param_1 + uVar2 * 8) = param_2;
      param_1[2] = param_1[2] + 1;
    }
  }
  return;
}


/* ================= FUN_140002820 @ 140002820  size=27  callers=2 callees=0 ================= */
/* CALLERS: FUN_140007b10, FUN_140007c00 */

undefined8 FUN_140002820(longlong *param_1,ulonglong param_2)

{
  if (((param_1 != (longlong *)0x0) && (param_2 < (ulonglong)param_1[2])) && (*param_1 != 0)) {
    return *(undefined8 *)(*param_1 + param_2 * 8);
  }
  return 0;
}


/* ================= FUN_140002840 @ 140002840  size=55  callers=3 callees=1 ================= */
/* CALLERS: FUN_1400097d0, FUN_140009970, FUN_140009af0 */
/* CALLEES: calloc */

void FUN_140002840(undefined8 *param_1,undefined1 param_2)

{
  void *pvVar1;
  
  if (param_1 != (undefined8 *)0x0) {
    *(undefined1 *)(param_1 + 3) = param_2;
    param_1[1] = 4;
    param_1[2] = 0;
    pvVar1 = calloc(4,8);
    *param_1 = pvVar1;
  }
  return;
}


/* ================= FUN_140002880 @ 140002880  size=61  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400027c0 */
/* CALLEES: realloc */

void FUN_140002880(undefined8 *param_1,longlong param_2)

{
  void *pvVar1;
  
  if ((param_1 != (undefined8 *)0x0) &&
     (pvVar1 = realloc((void *)*param_1,param_2 * 8), pvVar1 != (void *)0x0)) {
    *param_1 = pvVar1;
    param_1[1] = param_2;
  }
  return;
}


/* ================= FUN_1400028c0 @ 1400028c0  size=27  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003d210 */
/* CALLEES: FUN_140003a30, FUN_140003a70 */

void FUN_1400028c0(void)

{
  char cVar1;
  
  cVar1 = FUN_140003a30();
  if (cVar1 != '\0') {
    FUN_140003a70();
    return;
  }
  return;
}


/* ================= FUN_1400028e0 @ 1400028e0  size=32  callers=2 callees=2 ================= */
/* CALLERS: FUN_14000ecf0, FUN_14001b7c0 */
/* CALLEES: FUN_140005310, FUN_14000a2f0 */

undefined8 FUN_1400028e0(void)

{
  char cVar1;
  undefined8 uVar2;
  
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    return 2;
  }
  uVar2 = FUN_140005310();
  return uVar2;
}


/* ================= FUN_140002900 @ 140002900  size=29  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001b7e0 */
/* CALLEES: FUN_140007860, FUN_14000a2f0 */

undefined8 FUN_140002900(void)

{
  char cVar1;
  undefined8 uVar2;
  
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140007860();
  return uVar2;
}


/* ================= FUN_140002920 @ 140002920  size=42  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003d210 */
/* CALLEES: FUN_140003a30, FUN_140007420 */

undefined8 FUN_140002920(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140007420(param_1,&LAB_140007a00,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002950 @ 140002950  size=29  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003d210 */
/* CALLEES: FUN_140003a30, FUN_140007a10 */

undefined8 FUN_140002950(void)

{
  char cVar1;
  undefined8 uVar2;
  
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140007a10();
  return uVar2;
}


/* ================= FUN_140002970 @ 140002970  size=35  callers=5 callees=2 ================= */
/* CALLERS: FUN_14000e5e0, FUN_14000e670, FUN_14000e760, FUN_14000ebc0, FUN_14000ed20 */
/* CALLEES: FUN_1400083d0, FUN_14000a2f0 */

undefined8 FUN_140002970(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_1400083d0(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_1400029a0 @ 1400029a0  size=29  callers=6 callees=2 ================= */
/* CALLERS: FUN_14000ea00, FUN_14000ebc0, FUN_14000ed20, FUN_14001b810, FUN_14001b820, FUN_14001b8b0 */
/* CALLEES: FUN_140008400, FUN_14000a2f0 */

undefined8 FUN_1400029a0(void)

{
  char cVar1;
  undefined8 uVar2;
  
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140008400();
  return uVar2;
}


/* ================= FUN_1400029c0 @ 1400029c0  size=35  callers=1 callees=2 ================= */
/* CALLERS: FUN_14001b8b0 */
/* CALLEES: FUN_140008460, FUN_14000a2f0 */

undefined8 FUN_1400029c0(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140008460(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_1400029f0 @ 1400029f0  size=35  callers=3 callees=2 ================= */
/* CALLERS: FUN_14000ebc0, FUN_14000f180, FUN_14000f1b0 */
/* CALLEES: FUN_140003a30, FUN_140008500 */

undefined8 FUN_1400029f0(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140008500(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002a20 @ 140002a20  size=35  callers=2 callees=2 ================= */
/* CALLERS: FUN_14000ea00, FUN_14001b820 */
/* CALLEES: FUN_140008580, FUN_14000a2f0 */

undefined8 FUN_140002a20(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140008580(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002a50 @ 140002a50  size=35  callers=4 callees=2 ================= */
/* CALLERS: FUN_14000e760, FUN_14000ebc0, FUN_14000ed20, FUN_14001b820 */
/* CALLEES: FUN_140008620, FUN_14000a2f0 */

undefined8 FUN_140002a50(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140008620(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002a80 @ 140002a80  size=29  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000ef20 */
/* CALLEES: FUN_140003a30, FUN_140008670 */

undefined8 FUN_140002a80(void)

{
  char cVar1;
  undefined8 uVar2;
  
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140008670();
  return uVar2;
}


/* ================= FUN_140002aa0 @ 140002aa0  size=35  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000ef20 */
/* CALLEES: FUN_140003a30, FUN_140008690 */

undefined8 FUN_140002aa0(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140008690(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002ad0 @ 140002ad0  size=35  callers=2 callees=2 ================= */
/* CALLERS: FUN_14000ebc0, FUN_14000ef20 */
/* CALLEES: FUN_140003a30, FUN_140008710 */

undefined8 FUN_140002ad0(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140008710(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002b00 @ 140002b00  size=35  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000ef20 */
/* CALLEES: FUN_140003a30, FUN_140008730 */

undefined8 FUN_140002b00(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140008730(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002b30 @ 140002b30  size=35  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000ef20 */
/* CALLEES: FUN_140003a30, FUN_140008780 */

undefined8 FUN_140002b30(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140008780(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002b60 @ 140002b60  size=40  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000ef20 */
/* CALLEES: FUN_140003a30, FUN_1400087a0 */

undefined8 FUN_140002b60(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0xffffffffffffffff;
  }
  uVar2 = FUN_1400087a0(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002b90 @ 140002b90  size=35  callers=1 callees=2 ================= */
/* CALLERS: FUN_14000ef20 */
/* CALLEES: FUN_140003a30, FUN_1400087d0 */

undefined8 FUN_140002b90(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_1400087d0(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002bc0 @ 140002bc0  size=40  callers=2 callees=2 ================= */
/* CALLERS: FUN_14000ebc0, FUN_14000ef20 */
/* CALLEES: FUN_140003a30, FUN_1400087f0 */

undefined8 FUN_140002bc0(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0xffffffffffffffff;
  }
  uVar2 = FUN_1400087f0(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002c00 @ 140002c00  size=174  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_140005560, FUN_14000b830 */

undefined8 * FUN_140002c00(undefined8 *param_1,undefined8 param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  longlong lVar5;
  undefined8 *puVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  undefined1 local_78 [112];
  
  lVar5 = FUN_140005560(param_2);
  if (lVar5 == 0) {
    *param_1 = 0;
    param_1[1] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
    param_1[4] = 0;
    param_1[5] = 0;
    param_1[6] = 0;
    param_1[7] = 0;
    uVar11 = 0;
    uVar12 = 0;
    uVar13 = 0;
    uVar14 = 0;
    param_1[8] = 0;
    param_1[9] = 0;
    uVar7 = 0;
    uVar8 = 0;
    uVar9 = 0;
    uVar10 = 0;
  }
  else {
    puVar6 = (undefined8 *)FUN_14000b830(local_78,lVar5);
    uVar1 = puVar6[1];
    uVar2 = puVar6[2];
    uVar3 = puVar6[3];
    *param_1 = *puVar6;
    param_1[1] = uVar1;
    uVar1 = puVar6[4];
    uVar4 = puVar6[5];
    param_1[2] = uVar2;
    param_1[3] = uVar3;
    uVar2 = puVar6[6];
    uVar3 = puVar6[7];
    param_1[4] = uVar1;
    param_1[5] = uVar4;
    uVar1 = puVar6[8];
    uVar4 = puVar6[9];
    param_1[6] = uVar2;
    param_1[7] = uVar3;
    uVar11 = *(undefined4 *)(puVar6 + 10);
    uVar12 = *(undefined4 *)((longlong)puVar6 + 0x54);
    uVar13 = *(undefined4 *)(puVar6 + 0xb);
    uVar14 = *(undefined4 *)((longlong)puVar6 + 0x5c);
    param_1[8] = uVar1;
    param_1[9] = uVar4;
    uVar7 = *(undefined4 *)(puVar6 + 0xc);
    uVar8 = *(undefined4 *)((longlong)puVar6 + 100);
    uVar9 = *(undefined4 *)(puVar6 + 0xd);
    uVar10 = *(undefined4 *)((longlong)puVar6 + 0x6c);
  }
  *(undefined4 *)(param_1 + 10) = uVar11;
  *(undefined4 *)((longlong)param_1 + 0x54) = uVar12;
  *(undefined4 *)(param_1 + 0xb) = uVar13;
  *(undefined4 *)((longlong)param_1 + 0x5c) = uVar14;
  *(undefined4 *)(param_1 + 0xc) = uVar7;
  *(undefined4 *)((longlong)param_1 + 100) = uVar8;
  *(undefined4 *)(param_1 + 0xd) = uVar9;
  *(undefined4 *)((longlong)param_1 + 0x6c) = uVar10;
  return param_1;
}


/* ================= FUN_140002cb0 @ 140002cb0  size=31  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_140005560, FUN_140005ab0 */

void FUN_140002cb0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140005560();
  if (lVar1 == 0) {
    return;
  }
  FUN_140005ab0(lVar1);
  return;
}


/* ================= FUN_140002cd0 @ 140002cd0  size=31  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_140005560, FUN_140005dc0 */

void FUN_140002cd0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140005560();
  if (lVar1 == 0) {
    return;
  }
  FUN_140005dc0(lVar1);
  return;
}


/* ================= FUN_140002cf0 @ 140002cf0  size=43  callers=2 callees=2 ================= */
/* CALLERS: FUN_14003d210, FUN_1400404c0 */
/* CALLEES: FUN_140005560, FUN_1400065c0 */

undefined * FUN_140002cf0(void)

{
  longlong lVar1;
  undefined *puVar2;
  
  lVar1 = FUN_140005560();
  if (lVar1 == 0) {
    return &DAT_14005f306;
  }
  puVar2 = (undefined *)FUN_1400065c0(lVar1,0x58);
  return puVar2;
}


/* ================= FUN_140002d20 @ 140002d20  size=18  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_1400072b0 */

void FUN_140002d20(undefined8 param_1,undefined4 param_2)

{
  FUN_1400072b0(FUN_140005380,param_1,param_2);
  return;
}


/* ================= FUN_140002d40 @ 140002d40  size=31  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_140005560, FUN_1400070e0 */

void FUN_140002d40(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140005560();
  if (lVar1 == 0) {
    return;
  }
  FUN_1400070e0(lVar1);
  return;
}


/* ================= FUN_140002d60 @ 140002d60  size=18  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_1400072b0 */

void FUN_140002d60(undefined8 param_1,undefined4 param_2)

{
  FUN_1400072b0(FUN_140007a60,param_1,param_2);
  return;
}


/* ================= FUN_140002d80 @ 140002d80  size=31  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_140005560, FUN_140007aa0 */

void FUN_140002d80(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140005560();
  if (lVar1 == 0) {
    return;
  }
  FUN_140007aa0(lVar1);
  return;
}


/* ================= FUN_140002da0 @ 140002da0  size=138  callers=2 callees=1 ================= */
/* CALLERS: FUN_14003d210, FUN_1400404c0 */
/* CALLEES: FUN_1400088e0 */

undefined8 * FUN_140002da0(undefined8 *param_1)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 *puVar6;
  undefined1 local_b8 [176];
  
  puVar6 = (undefined8 *)FUN_1400088e0(local_b8);
  uVar1 = puVar6[1];
  uVar2 = puVar6[2];
  uVar3 = puVar6[3];
  *param_1 = *puVar6;
  param_1[1] = uVar1;
  uVar1 = puVar6[4];
  uVar4 = puVar6[5];
  param_1[2] = uVar2;
  param_1[3] = uVar3;
  uVar2 = puVar6[6];
  uVar3 = puVar6[7];
  param_1[4] = uVar1;
  param_1[5] = uVar4;
  uVar1 = puVar6[8];
  uVar4 = puVar6[9];
  param_1[6] = uVar2;
  param_1[7] = uVar3;
  uVar2 = puVar6[10];
  uVar3 = puVar6[0xb];
  param_1[8] = uVar1;
  param_1[9] = uVar4;
  uVar1 = puVar6[0xc];
  uVar4 = puVar6[0xd];
  param_1[10] = uVar2;
  param_1[0xb] = uVar3;
  uVar2 = puVar6[0x10];
  uVar3 = puVar6[0x11];
  param_1[0xc] = uVar1;
  param_1[0xd] = uVar4;
  uVar1 = puVar6[0xf];
  param_1[0xe] = puVar6[0xe];
  param_1[0xf] = uVar1;
  uVar4 = puVar6[0x12];
  uVar5 = puVar6[0x13];
  uVar1 = puVar6[0x14];
  param_1[0x10] = uVar2;
  param_1[0x11] = uVar3;
  param_1[0x12] = uVar4;
  param_1[0x13] = uVar5;
  param_1[0x14] = uVar1;
  return param_1;
}


/* ================= FUN_140002e30 @ 140002e30  size=18  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_1400072b0 */

void FUN_140002e30(undefined8 param_1,undefined4 param_2)

{
  FUN_1400072b0(FUN_140006a00,param_1,param_2);
  return;
}


/* ================= FUN_140002e50 @ 140002e50  size=33  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_140005560, FUN_140006a40 */

void FUN_140002e50(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140005560();
  if (lVar1 == 0) {
    return;
  }
  FUN_140006a40(lVar1,0);
  return;
}


/* ================= FUN_140002e80 @ 140002e80  size=31  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_140005560, FUN_140006b10 */

void FUN_140002e80(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140005560();
  if (lVar1 == 0) {
    return;
  }
  FUN_140006b10(lVar1);
  return;
}


/* ================= FUN_140002eb0 @ 140002eb0  size=31  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400404c0 */
/* CALLEES: FUN_140005560, FUN_140006d80 */

void FUN_140002eb0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140005560();
  if (lVar1 == 0) {
    return;
  }
  FUN_140006d80(lVar1);
  return;
}


/* ================= FUN_140002f00 @ 140002f00  size=35  callers=3 callees=2 ================= */
/* CALLERS: FUN_14000e760, FUN_14000eaa0, FUN_14000f0c0 */
/* CALLEES: FUN_140003a30, FUN_1400075e0 */

undefined8 FUN_140002f00(undefined8 param_1,ulonglong param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  param_2 = param_2 & 0xffffffff;
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_1400075e0(param_1,param_2 & 0xffffffff);
  return uVar2;
}


/* ================= FUN_140002f30 @ 140002f30  size=29  callers=2 callees=2 ================= */
/* CALLERS: FUN_14000eaa0, FUN_14000f0c0 */
/* CALLEES: FUN_140003a30, FUN_140007670 */

undefined8 FUN_140002f30(void)

{
  char cVar1;
  undefined8 uVar2;
  
  cVar1 = FUN_140003a30();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140007670();
  return uVar2;
}


/* ================= FUN_140002f50 @ 140002f50  size=7  callers=1 callees=0 ================= */
/* CALLERS: FUN_140005ef0 */

bool FUN_140002f50(int *param_1)

{
  return *param_1 != 0;
}


/* ================= FUN_140002f60 @ 140002f60  size=25  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003d210 */

undefined1 * FUN_140002f60(char *param_1,undefined8 param_2,undefined8 param_3)

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
  
  if ((DAT_140097bf4 != '\0') || (DAT_140097bf5 != '\0')) {
    return (undefined1 *)0x0;
  }
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


/* ================= FUN_140002f80 @ 140002f80  size=45  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000a870 */

void FUN_140002f80(undefined8 param_1)

{
  longlong *plVar1;
  int iVar2;
  
  iVar2 = 0;
  plVar1 = &DAT_140097c00;
  do {
    if (*plVar1 == 0) {
      (&DAT_140097c00)[iVar2] = param_1;
      return;
    }
    iVar2 = iVar2 + 1;
    plVar1 = plVar1 + 1;
  } while ((longlong)plVar1 < 0x140097c10);
  return;
}


/* ================= FUN_140002fb0 @ 140002fb0  size=2578  callers=2 callees=18 ================= */
/* CALLERS: FUN_140002fb0, FUN_1400070e0 */
/* CALLEES: FUN_140002fb0, FUN_140003fc0, FUN_1400044f0, FUN_140004df0, FUN_140006c00, FUN_140006fb0, FUN_140007240, FUN_1400080f0, FUN_140008850, FUN_140008dc0, FUN_14000a4b0, FUN_14000a610, FUN_14000a630, FUN_14000e4a0, FUN_14000e500, __security_check_cookie, memset, snprintf */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_140002fb0(longlong param_1,char *param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  undefined8 uVar4;
  longlong lVar5;
  ulonglong uVar6;
  longlong lVar7;
  longlong lVar8;
  char *pcVar9;
  longlong lVar10;
  int iVar11;
  int iVar12;
  undefined1 auStack_e98 [32];
  char local_e78;
  undefined1 local_e77 [3];
  int local_e74;
  undefined4 local_e70;
  undefined4 local_e6c;
  longlong local_e68;
  undefined1 local_e58 [512];
  char local_c58 [512];
  char local_a58 [512];
  char local_858 [512];
  undefined1 local_658 [512];
  undefined1 local_458 [512];
  char local_258 [512];
  ulonglong local_58;
  
  local_58 = DAT_140096fe8 ^ (ulonglong)auStack_e98;
  local_e70 = param_4;
  local_e6c = param_3;
  local_e68 = param_1;
  memset(local_c58,0,0x200);
  memset(local_658,0,0x200);
  memset(local_458,0,0x200);
  memset(local_a58,0,0x200);
  memset(local_258,0,0x200);
  memset(local_e58,0,0x200);
  memset(local_858,0,0x200);
  if (((param_1 == 0) || (param_2 == (char *)0x0)) ||
     (cVar1 = FUN_14000a630(param_1), cVar1 == '\0')) {
    uVar4 = 0xffffffff;
  }
  else {
    iVar11 = 0;
    FUN_14000e500(param_2,&DAT_14005f306,param_3);
    FUN_14000e500(local_c58,&DAT_14005f306,0x200);
    FUN_14000e500(local_658,&DAT_14005f306,0x200);
    FUN_14000e500(local_458,&DAT_14005f306,0x200);
    FUN_14000e500(local_a58,&DAT_14005f306,0x200);
    FUN_14000e500(local_e58,&DAT_14005f306,0x200);
    FUN_14000e500(local_858,&DAT_14005f306,0x200);
    do {
      lVar5 = -1;
      if (0x18 < iVar11) break;
      iVar11 = iVar11 + 1;
      local_e74 = iVar11;
      uVar4 = FUN_140006fb0(param_1);
      switch(uVar4) {
      case 1:
        FUN_14000e500(local_e58,&DAT_14005f574,0x200);
        iVar11 = 0;
        lVar5 = FUN_140003fc0(param_1,0);
        FUN_14000e500(local_c58,&DAT_14005f306,0x200);
        while (lVar5 != 0) {
          lVar7 = FUN_140006fb0(lVar5);
          if (lVar7 == 0x21) {
            if (0 < iVar11) {
              FUN_14000e4a0(local_c58,&DAT_14005f578,0x200);
            }
            uVar2 = FUN_140008850(lVar5);
            snprintf(local_658,0x200,&DAT_14005f57c,uVar2);
            FUN_14000e4a0(local_c58,local_658,0x200);
            iVar11 = iVar11 + 1;
          }
          else {
            lVar7 = FUN_140006fb0(lVar5);
            if ((lVar7 == 0x45) && (uVar6 = FUN_140004df0(lVar5,0x71,&local_e78), local_e78 != '\0')
               ) {
              uVar2 = FUN_140008850(lVar5);
              iVar12 = 0;
              if (uVar6 != 0) {
                do {
                  if (0 < iVar12) {
                    FUN_14000e4a0(local_c58,&DAT_14005f578,0x200);
                  }
                  snprintf(local_658,0x200,&DAT_14005f57c,uVar2);
                  FUN_14000e4a0(local_c58,local_658,0x200);
                  iVar12 = iVar12 + 1;
                } while ((ulonglong)(longlong)iVar12 < uVar6);
              }
            }
          }
          FUN_14000a610(lVar5);
          lVar5 = FUN_1400044f0();
          param_3 = local_e6c;
          param_4 = local_e70;
        }
        FUN_14000e4a0(local_e58,local_c58,0x200);
        FUN_14000e4a0(local_e58,&DAT_14005f580,0x200);
        FUN_14000e4a0(local_e58,param_2,0x200);
        FUN_14000e500(param_2,local_e58,param_3);
        iVar11 = local_e74;
        break;
      case 2:
      case 4:
      case 0x13:
      case 0x17:
        lVar10 = FUN_140006c00(param_1,0);
        lVar7 = lVar5;
        if (lVar10 == 0) {
          do {
            lVar8 = lVar7 + 1;
            lVar10 = lVar7 + 1;
            lVar7 = lVar8;
          } while (local_858[lVar10] != '\0');
          if (lVar8 != 0) {
            FUN_14000e4a0(local_858,param_2,0x200);
            pcVar9 = local_858;
            goto LAB_14000356a;
          }
        }
        else {
          uVar4 = FUN_140006c00(param_1,0);
          FUN_14000e500(local_c58,uVar4,0x200);
          FUN_14000e4a0(local_c58,param_2,0x200);
          pcVar9 = local_c58;
LAB_14000356a:
          FUN_14000e500(param_2,pcVar9,param_3);
        }
        lVar7 = FUN_140008dc0(param_1);
        if (lVar7 != 0) {
          uVar4 = FUN_140008dc0(param_1);
          FUN_14000e500(local_e58,uVar4,0x200);
          do {
            lVar5 = lVar5 + 1;
          } while (param_2[lVar5] != '\0');
          if (lVar5 != 0) {
            FUN_14000e4a0(local_e58,&DAT_14005f570,0x200);
          }
          FUN_14000e4a0(local_e58,param_2,0x200);
          FUN_14000e500(param_2,local_e58,param_3);
        }
        lVar5 = FUN_140006fb0(param_1);
        if (lVar5 == 4) {
          param_1 = 0;
        }
        break;
      case 0xf:
      case 0x10:
        lVar5 = FUN_140008dc0(param_1);
        if (lVar5 != 0) {
          uVar4 = FUN_140008dc0(param_1);
          FUN_14000e500(local_e58,uVar4,0x200);
          FUN_14000e4a0(local_e58,param_2,0x200);
          FUN_14000e500(param_2,local_e58,param_3);
        }
        break;
      case 0x15:
      case 0x1d:
      case 0x2e:
        FUN_14000e500(local_e58,&DAT_14005f584,0x200);
        FUN_14000e500(local_c58,&DAT_14005f306,0x200);
        lVar5 = FUN_140003fc0(param_1,0);
        iVar11 = 0;
        while (lVar5 != 0) {
          FUN_14000e500(local_458,&DAT_14005f306,0x200);
          lVar7 = FUN_140006fb0(lVar5);
          iVar12 = iVar11;
          if (((lVar7 == 5) && (lVar7 = FUN_140004df0(lVar5,0x34,local_e77), lVar7 == 0)) &&
             (iVar3 = FUN_140002fb0(lVar5,local_458,0x200,param_4), iVar3 == 0)) {
            iVar12 = iVar11 + 1;
            if (0 < iVar11) {
              FUN_14000e4a0(local_c58,&DAT_14005f588,0x200);
            }
            FUN_14000e4a0(local_c58,local_458,0x200);
          }
          FUN_14000a610(lVar5);
          lVar5 = FUN_1400044f0();
          iVar11 = iVar12;
        }
        FUN_14000e4a0(local_e58,local_c58,0x200);
        FUN_14000e4a0(local_e58,&DAT_14005f58c,0x200);
        FUN_14000e4a0(local_e58,param_2,0x200);
        FUN_14000e500(param_2,local_e58,param_3);
        lVar5 = FUN_1400080f0(param_1,0x49);
        iVar11 = local_e74;
        if (lVar5 == 0) {
          FUN_14000e500(local_e58,&DAT_14005f590,0x200);
          FUN_14000e4a0(local_e58,param_2,0x200);
          FUN_14000e500(param_2,local_e58,param_3);
          iVar11 = local_e74;
        }
        break;
      case 0x16:
        lVar5 = FUN_140006c00(param_1,0);
        if (lVar5 == 0) {
          FUN_14000e500(local_858,&DAT_14005f306,0x200);
        }
        else {
          uVar4 = FUN_140006c00(param_1,0);
          FUN_14000e500(local_858,uVar4,0x200);
        }
        break;
      case 0x24:
      case 0x3b:
        lVar5 = FUN_140006c00(param_1,0);
        if (lVar5 != 0) {
          uVar4 = FUN_140006c00(param_1,0);
          FUN_14000e500(local_c58,uVar4,0x200);
          FUN_14000e4a0(local_c58,param_2,0x200);
          FUN_14000e500(param_2,local_c58,param_3);
        }
        break;
      case 0x26:
      case 0x35:
        lVar5 = FUN_140008dc0(param_1);
        if (lVar5 != 0) {
          uVar4 = FUN_140008dc0(param_1);
          FUN_14000e500(local_e58,uVar4,0x200);
          FUN_14000e4a0(local_e58,&DAT_14005f570,0x200);
          FUN_14000e500(local_a58,local_e58,0x200);
        }
        break;
      case 0x37:
        lVar5 = FUN_140008dc0(param_1);
        if (lVar5 != 0) {
          uVar4 = FUN_140008dc0(param_1);
          FUN_14000e500(local_e58,uVar4,0x200);
          FUN_14000e500(local_258,local_e58,0x200);
        }
      }
      param_1 = FUN_1400080f0(param_1);
    } while (param_1 != 0);
    lVar7 = -1;
    lVar5 = lVar7;
    if (*param_2 == '*') {
      FUN_14000e4a0(local_a58,&DAT_14005f590,0x200);
    }
    do {
      lVar8 = lVar5 + 1;
      lVar10 = lVar5 + 1;
      lVar5 = lVar8;
    } while (local_a58[lVar10] != '\0');
    if (lVar8 != 0) {
      FUN_14000e4a0(local_a58,param_2,0x200);
      FUN_14000e500(param_2,local_a58,param_3);
    }
    lVar5 = local_e68;
    cVar1 = FUN_14000a4b0(local_e68);
    if (cVar1 != '\0') {
      FUN_14000e500(local_a58,"static",0x200);
      FUN_14000e4a0(local_a58,&DAT_14005f570,0x200);
      FUN_14000e4a0(local_a58,param_2,0x200);
      FUN_14000e500(param_2,local_a58,param_3);
    }
    iVar11 = FUN_140007240(lVar5);
    if (iVar11 != 0) {
      FUN_14000e500(local_a58,"virtual",0x200);
      FUN_14000e4a0(local_a58,&DAT_14005f570,0x200);
      FUN_14000e4a0(local_a58,param_2,0x200);
      FUN_14000e500(param_2,local_a58,param_3);
    }
    do {
      lVar10 = lVar7 + 1;
      lVar5 = lVar7 + 1;
      lVar7 = lVar10;
    } while (local_258[lVar5] != '\0');
    if (lVar10 != 0) {
      FUN_14000e4a0(param_2,&DAT_14005f570,param_3);
      FUN_14000e4a0(param_2,local_258,param_3);
    }
    uVar4 = 0;
  }
  return uVar4;
}


/* ================= FUN_140003a30 @ 140003a30  size=52  callers=16 callees=1 ================= */
/* CALLERS: FUN_1400028c0, FUN_140002920, FUN_140002950, FUN_1400029f0, FUN_140002a80, FUN_140002aa0, FUN_140002ad0, FUN_140002b00, FUN_140002b30, FUN_140002b60, FUN_140002b90, FUN_140002bc0, FUN_140002f00, FUN_140002f30, FUN_140007320, FUN_140007450 */
/* CALLEES: FUN_1400078f0 */

ulonglong FUN_140003a30(byte *param_1)

{
  ulonglong in_RAX;
  
  if (((param_1 != (byte *)0x0) && (DAT_140097bf4 == '\0')) && (DAT_140097bf5 == '\0')) {
    in_RAX = FUN_1400078f0();
    if ((int)in_RAX != -1) {
      return (ulonglong)*param_1;
    }
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_140003a70 @ 140003a70  size=323  callers=2 callees=10 ================= */
/* CALLERS: FUN_1400028c0, FUN_14000a870 */
/* CALLEES: FUN_140001a50, FUN_140004640, FUN_1400047a0, FUN_14000a2f0, FUN_14000d7f0, FUN_140018180, _guard_check_icall, thunk_FUN_140001fd0, thunk_FUN_14000e660, thunk_FUN_14000e990 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140003a70(longlong param_1)

{
  char cVar1;
  
  if ((param_1 != 0) && (cVar1 = FUN_14000a2f0(), cVar1 != '\0')) {
    DAT_140097bf5 = 1;
    thunk_FUN_14000e990();
    thunk_FUN_14000e660();
    FUN_140004640(param_1,param_1 + 0x180,*(undefined8 *)(param_1 + 0xa8));
    FUN_1400047a0(param_1,param_1 + 0x1b8,*(undefined8 *)(param_1 + 0xa8));
    FUN_140001a50(*(undefined8 *)(param_1 + 0x458));
    _guard_check_icall(*(undefined8 *)(param_1 + 0x78),*(undefined8 *)(param_1 + 0x468),
                       *(undefined4 *)(param_1 + 0xb8),*(undefined8 *)(param_1 + 0x460),
                       *(undefined4 *)(param_1 + 0xb4));
    thunk_FUN_140001fd0(param_1 + 1000);
    thunk_FUN_140001fd0(param_1 + 0x260);
    thunk_FUN_140001fd0(param_1 + 0x378);
    thunk_FUN_140001fd0(param_1 + 0x3b0);
    thunk_FUN_140001fd0(param_1 + 0x308);
    thunk_FUN_140001fd0(param_1 + 0x298);
    thunk_FUN_140001fd0(param_1 + 0x2d0);
    thunk_FUN_140001fd0(param_1 + 0x340);
    thunk_FUN_140001fd0(param_1 + 0x420);
    thunk_FUN_140001fd0(param_1 + 0x1f0);
    thunk_FUN_140001fd0(param_1 + 0x228);
    FUN_140018180(*(undefined8 *)(param_1 + 0x78),0);
    FUN_14000d7f0();
    DAT_14286d3c8 = 0;
    _DAT_140097bf8 = 0;
    DAT_140097bf5 = 0;
  }
  return;
}


/* ================= FUN_140003bc0 @ 140003bc0  size=77  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400077b0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_140003bc0(undefined8 *param_1,undefined8 *param_2)

{
  undefined8 *puVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  
  if (param_1 == (undefined8 *)0x0) {
    return 0xffffffff;
  }
  puVar1 = _DAT_140097bf8;
  if (param_2 != (undefined8 *)0x0) {
    uVar2 = FUN_1400077b0(*param_1);
    uVar3 = FUN_1400077b0(*param_2);
    if (uVar2 == uVar3) {
      return 0;
    }
    puVar1 = param_2;
    if (uVar2 < uVar3) {
      return 0xffffffff;
    }
  }
  _DAT_140097bf8 = puVar1;
  return 1;
}


/* ================= FUN_140003c10 @ 140003c10  size=59  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000c6f0 */

undefined8 FUN_140003c10(undefined4 param_1)

{
  switch(param_1) {
  case 1:
  case 5:
  case 8:
  case 0xb:
    return 0;
  case 2:
  case 6:
  case 9:
  case 0xc:
    return 1;
  case 3:
    return 2;
  case 4:
    return 3;
  case 7:
  case 10:
    return 4;
  default:
    return 5;
  }
}


/* ================= FUN_140003c80 @ 140003c80  size=57  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000bb50 */

void FUN_140003c80(char *param_1,int *param_2)

{
  if ((((param_1 != (char *)0x0) && (*param_1 == -0x61)) && (param_2 != (int *)0x0)) &&
     (*param_2 == 1)) {
    param_2[0xe] = param_2[2];
    *(undefined8 *)(param_2 + 0x10) = *(undefined8 *)(param_2 + 4);
    *(undefined8 *)(param_2 + 0x14) = *(undefined8 *)(param_2 + 10);
    *(ulonglong *)(param_2 + 0x12) = (ulonglong)(uint)param_2[8];
    param_2[2] = 0x10;
  }
  return;
}


/* ================= FUN_140003cc0 @ 140003cc0  size=76  callers=1 callees=3 ================= */
/* CALLERS: FUN_140006210 */
/* CALLEES: FUN_140004070, FUN_140006580, FUN_14000a610 */

undefined8 FUN_140003cc0(longlong param_1)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  longlong local_res8 [4];
  
  if (param_1 != 0) {
    iVar2 = FUN_140006580(param_1,local_res8);
    if (iVar2 != 0) {
      cVar1 = FUN_14000a610();
      uVar3 = FUN_140004070(*(undefined8 *)(local_res8[0] + 0x78),*(undefined8 *)(param_1 + 0x20),
                            cVar1 == '\0',1,0);
      return uVar3;
    }
  }
  return 0;
}


/* ================= FUN_140003d10 @ 140003d10  size=315  callers=1 callees=11 ================= */
/* CALLERS: FUN_14000d7a0 */
/* CALLEES: FUN_140007630, FUN_1400076d0, FUN_140007780, FUN_1400077b0, FUN_140007ff0, FUN_1400083d0, FUN_140008620, FUN_140008ef0, FUN_140009d40, FUN_14000a2f0, FUN_14000e080 */

undefined8 FUN_140003d10(longlong param_1)

{
  uint uVar1;
  char cVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  longlong lVar5;
  ulonglong uVar6;
  longlong lVar7;
  longlong lVar8;
  ulonglong uVar9;
  uint uVar10;
  longlong local_res10;
  
  lVar5 = param_1;
  cVar2 = FUN_14000a2f0();
  if (cVar2 != '\0') {
    uVar10 = 0;
    uVar1 = *(uint *)(lVar5 + 4);
    if (uVar1 != 0) {
      do {
        uVar3 = FUN_140008ef0(param_1,uVar10);
        lVar5 = FUN_140007630(param_1,uVar3);
        if ((lVar5 != 0) && (*(longlong *)(lVar5 + 0x10) == 0)) {
          uVar4 = FUN_140007780(param_1,uVar3);
          cVar2 = FUN_140009d40(uVar4);
          if (cVar2 != '\0') {
            uVar3 = FUN_1400076d0(param_1,uVar3);
            uVar6 = FUN_1400077b0(lVar5);
            lVar7 = FUN_1400083d0(param_1,uVar3);
            lVar8 = FUN_140008620(param_1,uVar3);
            if ((lVar7 != -1) && (lVar8 != 0)) {
              uVar9 = FUN_140007ff0(param_1,uVar10,uVar6,lVar7,lVar8);
              local_res10 = 0;
              if (uVar6 < uVar9) {
                local_res10 = uVar9 - uVar6;
              }
              FUN_14000e080(&local_res10,uVar6);
              *(longlong *)(lVar5 + 0x10) = local_res10;
            }
          }
        }
        uVar10 = uVar10 + 1;
      } while (uVar10 < uVar1);
    }
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_140003e50 @ 140003e50  size=354  callers=1 callees=12 ================= */
/* CALLERS: FUN_14000d7a0 */
/* CALLEES: FUN_140007630, FUN_1400076d0, FUN_140007780, FUN_1400077b0, FUN_140007ea0, FUN_140007f40, FUN_1400083d0, FUN_140008620, FUN_140008ef0, FUN_140009f90, FUN_14000a2f0, FUN_14000e080 */

undefined8 FUN_140003e50(longlong param_1)

{
  uint uVar1;
  char cVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  longlong lVar6;
  longlong lVar7;
  longlong lVar8;
  longlong lVar9;
  ulonglong uVar10;
  undefined8 uVar11;
  uint uVar12;
  longlong local_res10;
  
  lVar6 = param_1;
  cVar2 = FUN_14000a2f0();
  if (cVar2 == '\0') {
LAB_140003f90:
    uVar11 = 0xffffffff;
  }
  else {
    uVar1 = *(uint *)(lVar6 + 4);
    uVar12 = 0;
    if (uVar1 != 0) {
      do {
        uVar3 = FUN_140008ef0(param_1,uVar12);
        lVar6 = FUN_140007630(param_1,uVar3);
        if ((lVar6 != 0) && (*(longlong *)(lVar6 + 0x10) == 0)) {
          uVar4 = FUN_140007780(param_1,uVar3);
          cVar2 = FUN_140009f90(uVar4);
          if (cVar2 != '\0') {
            iVar5 = FUN_1400076d0(param_1,uVar3);
            lVar7 = FUN_1400083d0(param_1,iVar5);
            lVar8 = FUN_140008620(param_1,iVar5);
            lVar9 = FUN_1400077b0(lVar6);
            if (lVar9 - 1U < 0xfffffffffffffffe) {
              uVar10 = FUN_140007f40(param_1,lVar9,uVar12);
              if (((uVar10 == 0xffffffffffffffff) &&
                  ((iVar5 != 0xfff1 ||
                   (uVar10 = FUN_140007ea0(param_1,lVar9,uVar12), uVar10 == 0xffffffffffffffff))))
                 && ((lVar8 == 0 || (uVar10 = lVar8 + lVar7, uVar10 == 0xffffffffffffffff))))
              goto LAB_140003f90;
              if ((lVar8 != 0) && ((ulonglong)(lVar8 + lVar7) < uVar10)) {
                uVar10 = lVar8 + lVar7;
              }
              local_res10 = uVar10 - lVar9;
              FUN_14000e080(&local_res10,lVar9);
              *(longlong *)(lVar6 + 0x10) = local_res10;
            }
          }
        }
        uVar12 = uVar12 + 1;
      } while (uVar12 < uVar1);
    }
    uVar11 = 0;
  }
  return uVar11;
}


/* ================= FUN_140003fc0 @ 140003fc0  size=166  callers=2 callees=5 ================= */
/* CALLERS: FUN_140002fb0, FUN_14000b830 */
/* CALLEES: FUN_140004600, FUN_140006580, FUN_1400068b0, FUN_14000daf0, FUN_1400161e0 */

/* WARNING: Removing unreachable block (ram,0x000140004011) */

undefined8 FUN_140003fc0(longlong param_1,char param_2)

{
  int iVar1;
  undefined1 local_res18 [8];
  
  if ((param_1 != 0) && (iVar1 = FUN_140006580(param_1,local_res18), iVar1 != 0)) {
    FUN_1400161e0();
    if (param_2 != '\0') {
      FUN_140004600(param_1);
    }
    return 0;
  }
  return 0;
}


/* ================= FUN_140004070 @ 140004070  size=319  callers=4 callees=7 ================= */
/* CALLERS: FUN_140003cc0, FUN_1400043a0, FUN_140005960, FUN_14000aaf0 */
/* CALLEES: FUN_14000a2f0, FUN_14000a7e0, FUN_14000d820, FUN_1400163c0, FUN_140016600, __security_check_cookie, snprintf */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8
FUN_140004070(longlong param_1,ulonglong param_2,char param_3,char param_4,undefined1 *param_5)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  ulonglong uVar4;
  char *pcVar5;
  bool bVar6;
  undefined1 auStack_78 [32];
  undefined8 local_58;
  undefined8 local_48;
  undefined1 local_40 [16];
  ulonglong local_30;
  
  local_30 = DAT_140096fe8 ^ (ulonglong)auStack_78;
  if (param_5 != (undefined1 *)0x0) {
    *param_5 = 0;
  }
  uVar4 = 0;
  local_48 = 0;
  if ((param_1 != 0) && (cVar1 = FUN_14000a2f0(*(undefined8 *)(param_1 + 0x338)), cVar1 != '\0')) {
    if (param_3 == '\0') {
      lVar3 = *(longlong *)(param_1 + 0x148);
    }
    else {
      lVar3 = *(longlong *)(param_1 + 0x140);
    }
    uVar4 = param_2;
    if (lVar3 != 0) {
      uVar4 = (longlong)*(int *)(lVar3 + 8) + param_2;
    }
    if (param_4 != '\0') {
      FUN_14000d820(param_1,param_2,param_3);
    }
    local_58 = 0;
    if (param_2 == 0) {
      iVar2 = FUN_140016600(param_1,0,&local_48,param_3);
      bVar6 = iVar2 == 0;
    }
    else {
      iVar2 = FUN_1400163c0(param_1,param_2 & 0xffffffff,param_3,&local_48);
      if (iVar2 == 0) {
        return local_48;
      }
      if (param_5 != (undefined1 *)0x0) {
        *param_5 = iVar2 == -1;
      }
      bVar6 = iVar2 == -1;
    }
    if (bVar6) {
      return local_48;
    }
  }
  snprintf(local_40,0x10,"0x%llX",uVar4);
  pcVar5 = "debug.types";
  if (param_3 != '\0') {
    pcVar5 = "debug.info";
  }
  FUN_14000a7e0(0x3e,local_40,pcVar5);
  return 0;
}


/* ================= FUN_1400041b0 @ 1400041b0  size=130  callers=1 callees=3 ================= */
/* CALLERS: FUN_140004240 */
/* CALLEES: FUN_1400090c0, FUN_14000a630, FUN_140015220 */

undefined8 FUN_1400041b0(longlong param_1,undefined2 param_2,undefined1 *param_3)

{
  char cVar1;
  int iVar2;
  undefined8 local_res8;
  
  if (param_3 != (undefined1 *)0x0) {
    *param_3 = 0;
  }
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      cVar1 = FUN_1400090c0();
      if (cVar1 != '\0') {
        local_res8 = 0;
        iVar2 = FUN_140015220(param_1,param_2,&local_res8,0);
        if (iVar2 == 0) {
          *param_3 = 1;
          return local_res8;
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_140004240 @ 140004240  size=340  callers=5 callees=9 ================= */
/* CALLERS: FUN_140004240, FUN_140004df0, FUN_1400055d0, FUN_140006210, FUN_1400080f0 */
/* CALLEES: FUN_1400041b0, FUN_140004240, FUN_140004bf0, FUN_140005430, FUN_1400054c0, FUN_140006580, FUN_140006fb0, FUN_140007e90, FUN_1400080f0 */

undefined8 FUN_140004240(longlong param_1,ushort param_2,char *param_3,int param_4)

{
  int iVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  longlong lVar4;
  int iVar5;
  undefined8 local_res18 [2];
  
  if ((((param_3 != (char *)0x0) && (*param_3 = '\0', param_1 != 0)) &&
      (iVar5 = param_4, iVar1 = FUN_140006580(param_1,local_res18), iVar1 != 0)) && (iVar5 < 6)) {
    uVar3 = FUN_1400041b0();
    if (*param_3 != '\0') {
      return uVar3;
    }
    lVar4 = FUN_140006fb0(param_1);
    if (((ushort)(param_2 - 0x31) < 0x24) &&
       ((0x800400801U >> ((ulonglong)(param_2 - 0x31) & 0x3f) & 1) != 0)) {
      return uVar3;
    }
    if (lVar4 == 0x39) {
      uVar2 = FUN_140004bf0();
      lVar4 = FUN_140007e90(local_res18[0],uVar2);
      if ((lVar4 != 0) && (lVar4 != param_1)) {
        uVar3 = FUN_1400041b0(lVar4,param_2,param_3);
        return uVar3;
      }
    }
    else {
      lVar4 = FUN_140005430(param_1);
      if (((lVar4 != 0) && (lVar4 != param_1)) &&
         (uVar3 = FUN_1400041b0(lVar4,param_2,param_3), *param_3 != '\0')) {
        return uVar3;
      }
      lVar4 = FUN_1400080f0(param_1,0x31);
      if (((lVar4 != 0) && (lVar4 != param_1)) &&
         (uVar3 = FUN_140004240(lVar4,param_2,param_3,param_4 + 1), *param_3 != '\0')) {
        return uVar3;
      }
      lVar4 = FUN_1400054c0(param_1);
      if (((lVar4 != 0) && (lVar4 != param_1)) &&
         (uVar3 = FUN_1400041b0(lVar4,param_2,param_3), *param_3 != '\0')) {
        return uVar3;
      }
    }
  }
  return 0;
}


/* ================= FUN_1400043a0 @ 1400043a0  size=331  callers=1 callees=5 ================= */
/* CALLERS: FUN_14000e0d0 */
/* CALLEES: FUN_140004070, FUN_140004df0, FUN_1400090c0, FUN_140009100, FUN_14000a610 */

undefined8 FUN_1400043a0(longlong param_1,int *param_2)

{
  char cVar1;
  char cVar2;
  ulonglong uVar3;
  undefined8 uVar4;
  int *piVar5;
  int iVar6;
  bool bVar7;
  char local_res8 [8];
  
  if (param_1 == 0) {
    return 0;
  }
  uVar3 = *(ulonglong *)(param_1 + 0x28);
  piVar5 = param_2;
  cVar1 = FUN_14000a610();
  bVar7 = cVar1 == '\0';
  cVar1 = FUN_140009100();
  iVar6 = 0;
  if (param_2 != (int *)0x0) {
    iVar6 = *piVar5;
  }
  if ((((cVar1 == '\0') || (local_res8[0] = '\0', iVar6 < 0x41)) ||
      (cVar2 = FUN_1400090c0(), cVar2 == '\0')) ||
     (uVar3 = FUN_140004df0(param_1,1,local_res8), local_res8[0] == '\0')) {
    uVar4 = FUN_140004070(*(undefined8 *)(param_1 + 0x48),uVar3,bVar7,0,local_res8);
    if (cVar1 != '\0') {
      iVar6 = iVar6 + 1;
    }
  }
  else {
    uVar4 = FUN_140004070(*(undefined8 *)(param_1 + 0x48),uVar3,bVar7,0,local_res8);
  }
  cVar1 = FUN_140009100(uVar4);
  if (cVar1 == '\0') {
    while ((local_res8[0] != '\0' && (0 < iVar6))) {
      uVar3 = uVar3 + 1;
      iVar6 = iVar6 + -1;
      if ((*(longlong *)(param_1 + 0x50) != 0) &&
         ((ulonglong)(longlong)*(int *)(*(longlong *)(param_1 + 0x50) + 0x60) <= uVar3)) break;
      uVar4 = FUN_140004070(*(undefined8 *)(param_1 + 0x48),uVar3,bVar7,0,local_res8);
    }
  }
  if (param_2 != (int *)0x0) {
    *param_2 = iVar6;
  }
  return uVar4;
}


/* ================= FUN_1400044f0 @ 1400044f0  size=235  callers=2 callees=7 ================= */
/* CALLERS: FUN_140002fb0, FUN_14000b830 */
/* CALLEES: FUN_140004600, FUN_140006580, FUN_1400068b0, FUN_140006d10, FUN_14000d820, FUN_14000daf0, FUN_140016600 */

longlong FUN_1400044f0(longlong param_1,char param_2,undefined1 param_3)

{
  int iVar1;
  undefined8 uVar2;
  longlong lVar3;
  longlong local_res8;
  undefined8 local_res20;
  
  if (param_1 != 0) {
    lVar3 = param_1;
    iVar1 = FUN_140006580(param_1,&local_res20);
    if (iVar1 != 0) {
      local_res8 = 0;
      FUN_14000d820(*(undefined8 *)(lVar3 + 0x48),*(undefined8 *)(lVar3 + 0x20),param_3);
      iVar1 = FUN_140016600(*(undefined8 *)(param_1 + 0x48),param_1,&local_res8,param_3,0);
      if (iVar1 == 0) {
        if (local_res8 != 0) {
          uVar2 = FUN_1400068b0(param_1);
          lVar3 = FUN_140006d10(local_res20,uVar2);
          if (lVar3 != 0) {
            uVar2 = FUN_1400068b0(local_res8);
            FUN_14000daf0(local_res20,uVar2);
          }
        }
      }
      else {
        local_res8 = 0;
      }
      if (param_2 != '\0') {
        FUN_140004600(param_1);
      }
      return local_res8;
    }
  }
  return 0;
}


/* ================= FUN_1400045e0 @ 1400045e0  size=22  callers=4 callees=1 ================= */
/* CALLERS: FUN_140004df0, FUN_1400055d0, FUN_140006210, FUN_1400080f0 */
/* CALLEES: FUN_140015fc0 */

void FUN_1400045e0(longlong param_1,longlong param_2)

{
  if ((param_1 != 0) && (param_2 != 0)) {
    FUN_140015fc0(param_1,param_2,3);
    return;
  }
  return;
}


/* ================= FUN_140004600 @ 140004600  size=55  callers=7 callees=2 ================= */
/* CALLERS: FUN_140003fc0, FUN_1400044f0, FUN_140005960, FUN_140006210, FUN_1400080f0, FUN_14000b830, FUN_14000e0d0 */
/* CALLEES: FUN_140006580, FUN_140015fc0 */

void FUN_140004600(longlong param_1)

{
  int iVar1;
  longlong local_res8 [4];
  
  if (param_1 != 0) {
    iVar1 = FUN_140006580(param_1,local_res8);
    if (iVar1 != 0) {
      FUN_140015fc0(*(undefined8 *)(local_res8[0] + 0x78),param_1,8);
      return;
    }
  }
  return;
}


/* ================= FUN_140004640 @ 140004640  size=352  callers=1 callees=1 ================= */
/* CALLERS: FUN_140003a70 */
/* CALLEES: free */

void FUN_140004640(undefined8 param_1,longlong *param_2,longlong param_3)

{
  longlong *plVar1;
  uint uVar2;
  uint uVar3;
  void *_Memory;
  void *pvVar4;
  void *pvVar5;
  void *pvVar6;
  longlong lVar7;
  void *_Memory_00;
  ulonglong uVar8;
  undefined8 *puVar9;
  longlong lVar10;
  
  if (param_2 != (longlong *)0x0) {
    uVar2 = *(uint *)((longlong)param_2 + 0x24);
    if ((uVar2 != 0) && ((undefined8 *)*param_2 != (undefined8 *)0x0)) {
      uVar3 = *(uint *)(param_2 + 4);
      uVar8 = 0;
      puVar9 = *(undefined8 **)*param_2;
      if (param_3 != 0) {
        lVar10 = 0;
        do {
          if (uVar8 == (ulonglong)uVar3 / (ulonglong)uVar2) {
            lVar10 = lVar10 + 8;
            uVar8 = 0;
            puVar9 = *(undefined8 **)(lVar10 + *param_2);
          }
          _Memory = (void *)*puVar9;
          if (_Memory != (void *)0x0) {
            plVar1 = (longlong *)((longlong)_Memory + 0x70);
            _Memory_00 = *(void **)((longlong)_Memory + 0x70);
            while (_Memory_00 != (void *)0x0) {
              pvVar4 = (void *)*plVar1;
              pvVar5 = *(void **)((longlong)_Memory_00 + 0x18);
              pvVar6 = *(void **)((longlong)pvVar4 + 0x18);
              if (pvVar4 == _Memory_00) {
                *plVar1 = (longlong)pvVar6;
                if (pvVar6 == (void *)0x0) {
                  *(longlong **)((longlong)_Memory + 0x78) = plVar1;
                }
              }
              else {
                for (; pvVar6 != _Memory_00; pvVar6 = *(void **)((longlong)pvVar6 + 0x18)) {
                  pvVar4 = pvVar6;
                }
                lVar7 = *(longlong *)(*(longlong *)((longlong)pvVar4 + 0x18) + 0x18);
                *(longlong *)((longlong)pvVar4 + 0x18) = lVar7;
                if (lVar7 == 0) {
                  *(longlong **)((longlong)_Memory + 0x78) = (longlong *)((longlong)pvVar4 + 0x18);
                }
              }
              free(_Memory_00);
              _Memory_00 = pvVar5;
            }
            if (*(void **)((longlong)_Memory + 0x68) != (void *)0x0) {
              free(*(void **)((longlong)_Memory + 0x68));
            }
            if (*(void **)((longlong)_Memory + 0x60) != (void *)0x0) {
              free(*(void **)((longlong)_Memory + 0x60));
            }
            free(_Memory);
          }
          uVar8 = uVar8 + 1;
          puVar9 = (undefined8 *)((longlong)puVar9 + (ulonglong)uVar2);
          param_3 = param_3 + -1;
        } while (param_3 != 0);
      }
    }
  }
  return;
}


/* ================= FUN_1400047a0 @ 1400047a0  size=232  callers=1 callees=1 ================= */
/* CALLERS: FUN_140003a70 */
/* CALLEES: free */

void FUN_1400047a0(undefined8 param_1,longlong *param_2,longlong param_3)

{
  uint uVar1;
  uint uVar2;
  longlong lVar3;
  longlong *plVar4;
  ulonglong uVar5;
  longlong lVar6;
  
  if (((param_2 != (longlong *)0x0) && (uVar1 = *(uint *)((longlong)param_2 + 0x24), uVar1 != 0)) &&
     ((undefined8 *)*param_2 != (undefined8 *)0x0)) {
    uVar2 = *(uint *)(param_2 + 4);
    plVar4 = *(longlong **)*param_2;
    uVar5 = 0;
    if (param_3 != 0) {
      lVar6 = 0;
      do {
        if (uVar5 == (ulonglong)uVar2 / (ulonglong)uVar1) {
          lVar6 = lVar6 + 8;
          uVar5 = 0;
          plVar4 = *(longlong **)(lVar6 + *param_2);
        }
        lVar3 = *plVar4;
        if (lVar3 != 0) {
          if (*(undefined8 **)(lVar3 + 0x20) != (undefined8 *)0x0) {
            free((void *)**(undefined8 **)(lVar3 + 0x20));
          }
          if (*(undefined8 **)(lVar3 + 0x28) != (undefined8 *)0x0) {
            free((void *)**(undefined8 **)(lVar3 + 0x28));
          }
          if (*(undefined8 **)(lVar3 + 0x30) != (undefined8 *)0x0) {
            free((void *)**(undefined8 **)(lVar3 + 0x30));
          }
        }
        uVar5 = uVar5 + 1;
        plVar4 = (longlong *)((longlong)plVar4 + (ulonglong)uVar1);
        param_3 = param_3 + -1;
      } while (param_3 != 0);
    }
  }
  return;
}


/* ================= FUN_140004890 @ 140004890  size=80  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000bb50 */

undefined8 FUN_140004890(int *param_1)

{
  longlong lVar1;
  
  if ((*param_1 != 2) || (param_1[0x16] != 0xe)) {
    return 0xffffffff;
  }
  lVar1 = *(longlong *)(param_1 + 0x1e);
  if (param_1[2] == 2) {
    *(longlong *)(param_1 + 4) = *(longlong *)(param_1 + 4) + lVar1;
    *param_1 = 1;
    return 0;
  }
  if (param_1[2] != 4) {
    *(longlong *)(param_1 + 10) = *(longlong *)(param_1 + 10) + lVar1;
    *param_1 = 1;
    return 0;
  }
  *(longlong *)(param_1 + 10) = lVar1;
  param_1[2] = 5;
  *param_1 = 1;
  return 0;
}


/* ================= FUN_1400048e0 @ 1400048e0  size=49  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400058b0 */
/* CALLEES: FUN_140004920, FUN_140006580 */

undefined8 FUN_1400048e0(undefined8 param_1,undefined4 param_2)

{
  int iVar1;
  undefined8 uVar2;
  undefined4 local_res10 [2];
  undefined1 local_res18 [16];
  
  local_res10[0] = param_2;
  iVar1 = FUN_140006580(param_1,local_res18);
  if (iVar1 == 0) {
    return 0;
  }
  uVar2 = FUN_140004920(param_1,1,local_res10);
  return uVar2;
}


/* ================= FUN_140004920 @ 140004920  size=78  callers=2 callees=4 ================= */
/* CALLERS: FUN_1400048e0, FUN_140009fd0 */
/* CALLEES: FUN_140004970, FUN_140006580, FUN_140006d70, FUN_14000a610 */

undefined8 FUN_140004920(undefined8 param_1)

{
  int iVar1;
  undefined8 uVar2;
  undefined8 local_res20;
  
  iVar1 = FUN_140006580(param_1,&local_res20);
  if (iVar1 == 0) {
    return 0;
  }
  FUN_14000a610();
  uVar2 = FUN_140006d70();
  uVar2 = FUN_140004970(local_res20,uVar2);
  return uVar2;
}


/* ================= FUN_140004970 @ 140004970  size=303  callers=2 callees=8 ================= */
/* CALLERS: FUN_140004920, FUN_140004970 */
/* CALLEES: FUN_140004970, FUN_140005590, FUN_140005960, FUN_140006540, FUN_140006900, FUN_140006d30, FUN_14000a590, FUN_14000a7d0 */

longlong FUN_140004970(longlong param_1,longlong param_2,undefined1 param_3,int param_4,
                      undefined4 param_5,undefined8 param_6)

{
  char cVar1;
  undefined8 uVar2;
  longlong lVar3;
  undefined4 uVar5;
  undefined8 uVar4;
  
  if ((param_2 != 0) && (param_1 != 0)) {
    if (param_4 < 1000) {
      uVar2 = param_6;
      cVar1 = FUN_14000a590();
      uVar5 = (undefined4)((ulonglong)uVar2 >> 0x20);
      if (cVar1 != '\0') {
        return param_2;
      }
      uVar2 = FUN_140006d30(param_1,param_2,param_3);
      param_4 = param_4 + 1;
      uVar4 = CONCAT44(uVar5,param_5);
      lVar3 = FUN_140004970(param_1,uVar2,param_3,param_4,uVar4,param_6);
      uVar5 = (undefined4)((ulonglong)uVar4 >> 0x20);
      if (lVar3 != 0) {
        return lVar3;
      }
      uVar2 = FUN_140006900(param_2,param_3);
      uVar2 = FUN_140006540(param_1,uVar2);
      uVar4 = CONCAT44(uVar5,param_5);
      lVar3 = FUN_140004970(param_1,uVar2,param_3,param_4,uVar4,param_6);
      uVar5 = (undefined4)((ulonglong)uVar4 >> 0x20);
      if (lVar3 != 0) {
        return lVar3;
      }
      uVar2 = FUN_140005960(param_1,param_2,param_3);
      uVar2 = FUN_140005590(uVar2);
      lVar3 = FUN_140004970(param_1,uVar2,param_3,param_4,CONCAT44(uVar5,param_5),param_6);
      if (lVar3 != 0) {
        return lVar3;
      }
    }
    else {
      FUN_14000a7d0(0x2c);
    }
  }
  return 0;
}


/* ================= FUN_140004aa0 @ 140004aa0  size=73  callers=1 callees=3 ================= */
/* CALLERS: FUN_140005430 */
/* CALLEES: FUN_140001b70, FUN_140005df0, FUN_140007e00 */

undefined8 FUN_140004aa0(longlong param_1,longlong param_2)

{
  undefined8 uVar1;
  longlong lVar2;
  
  if (*(longlong *)(param_2 + 0x58) != 0) {
    uVar1 = FUN_140007e00(param_2,1);
    lVar2 = FUN_140001b70(*(undefined8 *)(param_1 + 0x458),uVar1);
    if (lVar2 != 0) {
      uVar1 = FUN_140005df0(param_1,lVar2);
      return uVar1;
    }
  }
  return 0;
}


/* ================= FUN_140004af0 @ 140004af0  size=73  callers=2 callees=3 ================= */
/* CALLERS: FUN_140005380, FUN_1400054c0 */
/* CALLEES: FUN_140001b70, FUN_140005df0, FUN_140007e00 */

undefined8 FUN_140004af0(longlong param_1,longlong param_2)

{
  undefined8 uVar1;
  longlong lVar2;
  
  if (*(longlong *)(param_2 + 0x58) != 0) {
    uVar1 = FUN_140007e00(param_2,0);
    lVar2 = FUN_140001b70(*(undefined8 *)(param_1 + 0x458),uVar1);
    if (lVar2 != 0) {
      uVar1 = FUN_140005df0(param_1,lVar2);
      return uVar1;
    }
  }
  return 0;
}


/* ================= FUN_140004b40 @ 140004b40  size=166  callers=1 callees=4 ================= */
/* CALLERS: FUN_140004bf0 */
/* CALLEES: FUN_140002040, FUN_140002210, FUN_140004df0, FUN_140005df0 */

ulonglong FUN_140004b40(longlong param_1,longlong param_2,longlong param_3,undefined2 param_4)

{
  ulonglong uVar1;
  undefined8 uVar2;
  longlong lVar3;
  ulonglong uVar4;
  char local_res10 [8];
  
  if ((param_2 != 0) && (param_1 != 0)) {
    uVar1 = FUN_140002040(param_2);
    uVar4 = 0;
    if (uVar1 != 0) {
      do {
        uVar2 = FUN_140002210(param_2,uVar4);
        lVar3 = FUN_140005df0(param_1,uVar2);
        if (((lVar3 != 0) &&
            (lVar3 = FUN_140004df0(lVar3,param_4,local_res10), local_res10[0] != '\0')) &&
           (lVar3 == param_3)) {
          return uVar4;
        }
        uVar4 = uVar4 + 1;
      } while (uVar4 < uVar1);
    }
  }
  return 0xffffffffffffffff;
}


/* ================= FUN_140004bf0 @ 140004bf0  size=82  callers=1 callees=3 ================= */
/* CALLERS: FUN_140004240 */
/* CALLEES: FUN_140004b40, FUN_140006580, FUN_140006fb0 */

undefined8 FUN_140004bf0(longlong param_1)

{
  int iVar1;
  longlong lVar2;
  undefined8 uVar3;
  longlong local_res8 [4];
  
  if (param_1 != 0) {
    iVar1 = FUN_140006580(param_1,local_res8);
    if (iVar1 != 0) {
      lVar2 = FUN_140006fb0();
      if (lVar2 == 0x39) {
        uVar3 = FUN_140004b40(local_res8[0],local_res8[0] + 0x2d0,*(undefined8 *)(param_1 + 0x20),
                              0x54);
        return uVar3;
      }
    }
  }
  return 0xffffffffffffffff;
}


/* ================= FUN_140004c50 @ 140004c50  size=139  callers=3 callees=2 ================= */
/* CALLERS: FUN_14000aa30, FUN_14000aaf0, FUN_14000ca30 */
/* CALLEES: FUN_140008400, FUN_140008580 */

uint FUN_140004c50(undefined8 param_1,longlong param_2)

{
  char cVar1;
  char cVar2;
  uint uVar3;
  char *pcVar4;
  uint uVar5;
  longlong lVar6;
  
  if (param_2 != 0) {
    uVar5 = 0;
    uVar3 = FUN_140008400();
    if (uVar3 != 0) {
      do {
        pcVar4 = (char *)FUN_140008580(param_1,uVar5);
        if (pcVar4 != (char *)0x0) {
          lVar6 = param_2 - (longlong)pcVar4;
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
  }
  return 0xffffffff;
}


/* ================= FUN_140004ce0 @ 140004ce0  size=135  callers=1 callees=3 ================= */
/* CALLERS: FUN_140006d80 */
/* CALLEES: FUN_1400090c0, FUN_14000a630, FUN_140015370 */

undefined8 FUN_140004ce0(longlong param_1,undefined2 param_2,undefined1 *param_3)

{
  char cVar1;
  undefined8 local_res8;
  
  local_res8 = 0;
  if (param_3 != (undefined1 *)0x0) {
    *param_3 = 0;
  }
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      cVar1 = FUN_1400090c0();
      if (cVar1 != '\0') {
        cVar1 = FUN_140015370(param_1,param_2,&local_res8,0);
        if (param_3 != (undefined1 *)0x0) {
          *param_3 = cVar1 == '\0';
        }
        return local_res8;
      }
    }
  }
  return 0;
}


/* ================= FUN_140004d70 @ 140004d70  size=117  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400065c0 */
/* CALLEES: FUN_1400090c0, FUN_14000a630, FUN_140015720 */

undefined8 FUN_140004d70(longlong param_1,undefined2 param_2,undefined8 *param_3)

{
  char cVar1;
  int iVar2;
  
  *param_3 = &DAT_14005f5a8;
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      cVar1 = FUN_1400090c0();
      if (cVar1 != '\0') {
        iVar2 = FUN_140015720(param_1,param_2,param_3,0);
        if (iVar2 != 0) {
          return 0xffffffff;
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_140004df0 @ 140004df0  size=361  callers=20 callees=13 ================= */
/* CALLERS: FUN_140002fb0, FUN_1400043a0, FUN_140004b40, FUN_140005260, FUN_140005590, FUN_140005a20, FUN_140005ab0, FUN_140005be0, FUN_140005dc0, FUN_140005e30, FUN_140006210, FUN_1400065c0, FUN_140006a40, FUN_140007240, FUN_1400080f0, FUN_140009730, FUN_140009bd0, FUN_140009c40, FUN_14000b830, FUN_14000ddf0 */
/* CALLEES: FUN_140004240, FUN_1400045e0, FUN_140009050, FUN_1400092b0, FUN_1400092d0, FUN_140009340, FUN_140009390, FUN_14000a630, FUN_1400181c0, FUN_1400182f0, FUN_140018390, FUN_140018540, FUN_140018650 */

/* WARNING: Removing unreachable block (ram,0x000140004e4a) */
/* WARNING: Removing unreachable block (ram,0x000140004e53) */
/* WARNING: Removing unreachable block (ram,0x000140004e5f) */
/* WARNING: Removing unreachable block (ram,0x000140004e73) */
/* WARNING: Removing unreachable block (ram,0x000140004e7f) */
/* WARNING: Removing unreachable block (ram,0x000140004e93) */
/* WARNING: Removing unreachable block (ram,0x000140004e9a) */
/* WARNING: Removing unreachable block (ram,0x000140004ea6) */
/* WARNING: Removing unreachable block (ram,0x000140004eba) */
/* WARNING: Removing unreachable block (ram,0x000140004ec8) */
/* WARNING: Removing unreachable block (ram,0x000140004ed4) */
/* WARNING: Removing unreachable block (ram,0x000140004ee5) */
/* WARNING: Removing unreachable block (ram,0x000140004ef3) */
/* WARNING: Removing unreachable block (ram,0x000140004eff) */
/* WARNING: Removing unreachable block (ram,0x000140004f13) */

undefined8 FUN_140004df0(longlong param_1,undefined8 param_2,undefined1 *param_3)

{
  char cVar1;
  longlong lVar2;
  
  if (param_3 != (undefined1 *)0x0) {
    *param_3 = 0;
  }
  if ((param_1 != 0) && (cVar1 = FUN_14000a630(), cVar1 != '\0')) {
    lVar2 = FUN_140004240();
    if (param_3 != (undefined1 *)0x0) {
      *param_3 = 0;
    }
    if (lVar2 != 0) {
      FUN_1400045e0(*(undefined8 *)(param_1 + 0x48),lVar2);
    }
    return 0;
  }
  return 0;
}


/* ================= FUN_140004f60 @ 140004f60  size=126  callers=2 callees=3 ================= */
/* CALLERS: FUN_140005770, FUN_140008850 */
/* CALLEES: FUN_1400090c0, FUN_14000a630, FUN_140015370 */

undefined8 FUN_140004f60(longlong param_1,undefined2 param_2,undefined1 *param_3)

{
  char cVar1;
  int iVar2;
  undefined8 local_res8;
  
  if (param_3 != (undefined1 *)0x0) {
    *param_3 = 0;
  }
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      cVar1 = FUN_1400090c0();
      if (cVar1 != '\0') {
        iVar2 = FUN_140015370(param_1,param_2,&local_res8,0);
        if (param_3 != (undefined1 *)0x0) {
          *param_3 = iVar2 == 0;
        }
        return local_res8;
      }
    }
  }
  return 0;
}


/* ================= FUN_140004fe0 @ 140004fe0  size=627  callers=1 callees=4 ================= */
/* CALLERS: FUN_14000d240 */
/* CALLEES: FUN_14000a2f0, FUN_14000c8c0, FUN_140018780, FUN_1400188e0 */

undefined8 * FUN_140004fe0(undefined8 *param_1,longlong param_2,longlong param_3)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  char cVar5;
  int iVar6;
  undefined8 *puVar7;
  undefined8 uVar8;
  longlong lVar9;
  undefined1 local_res8 [8];
  undefined8 local_res20;
  undefined8 local_e8;
  undefined8 local_e0;
  undefined1 local_d8 [8];
  undefined1 local_d0 [8];
  undefined1 local_c8 [8];
  undefined1 local_c0 [8];
  undefined1 local_b8 [8];
  undefined1 local_b0 [168];
  
  lVar9 = param_2;
  cVar5 = FUN_14000a2f0();
  if ((cVar5 == '\0') || (param_3 == -1)) {
LAB_1400051cc:
    puVar7 = (undefined8 *)&DAT_14005ec20;
  }
  else {
    if (*(longlong *)(lVar9 + 200) != param_3) {
      iVar6 = FUN_140018780(*(undefined8 *)(param_2 + 0x460),param_3,&local_res20,local_c8,local_d0,
                            0);
      if (iVar6 == 0) {
        iVar6 = FUN_1400188e0(local_res20,param_3,local_res8,local_d8,&local_e0,&local_e8,local_b8,
                              local_c0,0);
        if (iVar6 == 0) {
          *(longlong *)(param_2 + 200) = param_3;
          puVar7 = (undefined8 *)
                   FUN_14000c8c0(local_b0,local_res8[0],local_d8[0],DAT_140096000,local_e0,local_e8)
          ;
          uVar8 = puVar7[1];
          *(undefined8 *)(param_2 + 0xd0) = *puVar7;
          *(undefined8 *)(param_2 + 0xd8) = uVar8;
          uVar8 = puVar7[3];
          *(undefined8 *)(param_2 + 0xe0) = puVar7[2];
          *(undefined8 *)(param_2 + 0xe8) = uVar8;
          uVar8 = puVar7[5];
          *(undefined8 *)(param_2 + 0xf0) = puVar7[4];
          *(undefined8 *)(param_2 + 0xf8) = uVar8;
          uVar8 = puVar7[7];
          *(undefined8 *)(param_2 + 0x100) = puVar7[6];
          *(undefined8 *)(param_2 + 0x108) = uVar8;
          uVar8 = puVar7[9];
          *(undefined8 *)(param_2 + 0x110) = puVar7[8];
          *(undefined8 *)(param_2 + 0x118) = uVar8;
          uVar8 = puVar7[0xb];
          *(undefined8 *)(param_2 + 0x120) = puVar7[10];
          *(undefined8 *)(param_2 + 0x128) = uVar8;
          uVar8 = puVar7[0xd];
          *(undefined8 *)(param_2 + 0x130) = puVar7[0xc];
          *(undefined8 *)(param_2 + 0x138) = uVar8;
          uVar8 = puVar7[0xf];
          *(undefined8 *)(param_2 + 0x140) = puVar7[0xe];
          *(undefined8 *)(param_2 + 0x148) = uVar8;
          uVar8 = puVar7[0x11];
          *(undefined8 *)(param_2 + 0x150) = puVar7[0x10];
          *(undefined8 *)(param_2 + 0x158) = uVar8;
          uVar8 = puVar7[0x13];
          *(undefined8 *)(param_2 + 0x160) = puVar7[0x12];
          *(undefined8 *)(param_2 + 0x168) = uVar8;
          *(undefined8 *)(param_2 + 0x170) = puVar7[0x14];
          uVar1 = puVar7[1];
          uVar8 = puVar7[0x14];
          uVar2 = puVar7[2];
          uVar3 = puVar7[3];
          *param_1 = *puVar7;
          param_1[1] = uVar1;
          uVar1 = puVar7[4];
          uVar4 = puVar7[5];
          param_1[2] = uVar2;
          param_1[3] = uVar3;
          uVar2 = puVar7[6];
          uVar3 = puVar7[7];
          param_1[4] = uVar1;
          param_1[5] = uVar4;
          uVar1 = puVar7[8];
          uVar4 = puVar7[9];
          param_1[6] = uVar2;
          param_1[7] = uVar3;
          uVar2 = puVar7[10];
          uVar3 = puVar7[0xb];
          param_1[8] = uVar1;
          param_1[9] = uVar4;
          uVar1 = puVar7[0xc];
          uVar4 = puVar7[0xd];
          param_1[10] = uVar2;
          param_1[0xb] = uVar3;
          uVar2 = puVar7[0xe];
          uVar3 = puVar7[0xf];
          param_1[0xc] = uVar1;
          param_1[0xd] = uVar4;
          uVar1 = puVar7[0x10];
          uVar4 = puVar7[0x11];
          param_1[0xe] = uVar2;
          param_1[0xf] = uVar3;
          uVar2 = puVar7[0x12];
          uVar3 = puVar7[0x13];
          param_1[0x10] = uVar1;
          param_1[0x11] = uVar4;
          param_1[0x12] = uVar2;
          param_1[0x13] = uVar3;
          goto LAB_140005234;
        }
      }
      goto LAB_1400051cc;
    }
    puVar7 = (undefined8 *)(lVar9 + 0xd0);
  }
  uVar8 = puVar7[1];
  uVar1 = puVar7[2];
  uVar2 = puVar7[3];
  *param_1 = *puVar7;
  param_1[1] = uVar8;
  uVar8 = puVar7[4];
  uVar3 = puVar7[5];
  param_1[2] = uVar1;
  param_1[3] = uVar2;
  uVar1 = puVar7[6];
  uVar2 = puVar7[7];
  param_1[4] = uVar8;
  param_1[5] = uVar3;
  uVar8 = puVar7[8];
  uVar3 = puVar7[9];
  param_1[6] = uVar1;
  param_1[7] = uVar2;
  uVar1 = puVar7[10];
  uVar2 = puVar7[0xb];
  param_1[8] = uVar8;
  param_1[9] = uVar3;
  uVar8 = puVar7[0xc];
  uVar3 = puVar7[0xd];
  param_1[10] = uVar1;
  param_1[0xb] = uVar2;
  uVar1 = puVar7[0x10];
  uVar2 = puVar7[0x11];
  param_1[0xc] = uVar8;
  param_1[0xd] = uVar3;
  uVar8 = puVar7[0xf];
  param_1[0xe] = puVar7[0xe];
  param_1[0xf] = uVar8;
  uVar3 = puVar7[0x12];
  uVar4 = puVar7[0x13];
  uVar8 = puVar7[0x14];
  param_1[0x10] = uVar1;
  param_1[0x11] = uVar2;
  param_1[0x12] = uVar3;
  param_1[0x13] = uVar4;
LAB_140005234:
  param_1[0x14] = uVar8;
  return param_1;
}


/* ================= FUN_140005260 @ 140005260  size=166  callers=1 callees=3 ================= */
/* CALLERS: FUN_14000b630 */
/* CALLEES: FUN_140004df0, FUN_140006580, strstr */

undefined8 FUN_140005260(longlong param_1)

{
  int iVar1;
  ulonglong uVar2;
  char *pcVar3;
  char local_res8 [32];
  
  if ((param_1 != 0) && (iVar1 = FUN_140006580(param_1,local_res8), iVar1 != 0)) {
    uVar2 = FUN_140004df0();
    if (local_res8[0] == '\0') {
      if (*(char **)(param_1 + 0x58) != (char *)0x0) {
        pcVar3 = strstr(*(char **)(param_1 + 0x58),".cpp");
        if (pcVar3 != (char *)0x0) {
          return 2;
        }
        pcVar3 = strstr(*(char **)(param_1 + 0x58),".c");
        if (pcVar3 != (char *)0x0) {
          return 1;
        }
      }
    }
    else {
      if ((uVar2 < 0x1e) && ((0x20001006UL >> (uVar2 & 0x3f) & 1) != 0)) {
        return 1;
      }
      if ((uVar2 < 0x22) && ((0x206000010U >> (uVar2 & 0x3f) & 1) != 0)) {
        return 2;
      }
    }
  }
  return 0;
}


/* ================= FUN_140005310 @ 140005310  size=52  callers=4 callees=1 ================= */
/* CALLERS: FUN_1400028e0, FUN_140009ce0, FUN_14000aaf0, FUN_14000dfc0 */
/* CALLEES: FUN_1400078a0 */

undefined8 FUN_140005310(void)

{
  char cVar1;
  longlong lVar2;
  
  lVar2 = FUN_1400078a0();
  if ((lVar2 != 0) && (cVar1 = *(char *)(lVar2 + 5), cVar1 != '\0')) {
    if (cVar1 == '\x01') {
      return 0;
    }
    if (cVar1 == '\x02') {
      return 1;
    }
  }
  return 2;
}


/* ================= FUN_140005350 @ 140005350  size=33  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000aaf0 */

char * FUN_140005350(int param_1)

{
  if (param_1 == 0) {
    return "Little Endian";
  }
  if (param_1 != 1) {
    return "Unknown data encoding";
  }
  return "Big Endian";
}


/* ================= FUN_140005380 @ 140005380  size=163  callers=1 callees=7 ================= */
/* CALLERS: FUN_14000b830 */
/* CALLEES: FUN_140004af0, FUN_1400054c0, FUN_140006580, FUN_140006e30, FUN_1400080f0, FUN_140009d50, FUN_14000a630 */

longlong FUN_140005380(longlong param_1,int param_2)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined8 local_res8 [4];
  
  if (param_1 == 0) {
    return 0;
  }
  cVar1 = FUN_14000a630();
  if (cVar1 == '\0') {
    return 0;
  }
  if (param_2 == 0) goto LAB_1400053b0;
  cVar1 = FUN_140009d50();
  while (cVar1 == '\0') {
LAB_1400053b0:
    param_1 = FUN_1400080f0(param_1,0x49);
    if (param_1 == 0) break;
    cVar1 = FUN_140009d50(param_1);
  }
  lVar3 = FUN_1400054c0(param_1);
  if (lVar3 != 0) {
    param_1 = lVar3;
  }
  lVar3 = FUN_140006e30(param_1);
  if (((lVar3 == 0) && (iVar2 = FUN_140006580(param_1,local_res8), iVar2 != 0)) &&
     (lVar3 = FUN_140004af0(local_res8[0],param_1), lVar3 != 0)) {
    param_1 = lVar3;
  }
  return param_1;
}


/* ================= FUN_140005430 @ 140005430  size=141  callers=1 callees=6 ================= */
/* CALLERS: FUN_140004240 */
/* CALLEES: FUN_140004aa0, FUN_140005960, FUN_140006540, FUN_140006580, FUN_1400068b0, FUN_14000a610 */

longlong FUN_140005430(longlong param_1)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  longlong lVar4;
  undefined8 local_res8;
  
  if (param_1 == 0) {
    return 0;
  }
  iVar2 = FUN_140006580(param_1,&local_res8);
  if (iVar2 == 0) {
    return 0;
  }
  cVar1 = FUN_14000a610();
  uVar3 = FUN_1400068b0();
  lVar4 = FUN_140006540(local_res8,uVar3);
  if (lVar4 == *(longlong *)(param_1 + 0x20)) {
    return param_1;
  }
  if ((lVar4 != 0) && (lVar4 = FUN_140005960(local_res8,lVar4,cVar1 == '\0'), lVar4 != 0)) {
    return lVar4;
  }
  lVar4 = FUN_140004aa0(local_res8,param_1);
  return lVar4;
}


/* ================= FUN_1400054c0 @ 1400054c0  size=157  callers=5 callees=7 ================= */
/* CALLERS: FUN_140004240, FUN_140005380, FUN_140006a40, FUN_140006c00, FUN_140009a00 */
/* CALLEES: FUN_140004af0, FUN_140005960, FUN_140006560, FUN_140006580, FUN_1400068b0, FUN_140009da0, FUN_14000a610 */

longlong FUN_1400054c0(longlong param_1)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  longlong lVar4;
  undefined8 local_res8;
  
  if (param_1 == 0) {
    return 0;
  }
  iVar2 = FUN_140006580(param_1,&local_res8);
  if (iVar2 == 0) {
    return 0;
  }
  cVar1 = FUN_140009da0();
  if (cVar1 == '\0') {
    return 0;
  }
  cVar1 = FUN_14000a610(param_1);
  uVar3 = FUN_1400068b0();
  lVar4 = FUN_140006560(local_res8,uVar3);
  if (lVar4 != *(longlong *)(param_1 + 0x20)) {
    if ((lVar4 != 0) && (lVar4 = FUN_140005960(local_res8,lVar4,cVar1 == '\0'), lVar4 != 0)) {
      return lVar4;
    }
    lVar4 = FUN_140004af0(local_res8,param_1);
    return lVar4;
  }
  return param_1;
}


/* ================= FUN_140005560 @ 140005560  size=40  callers=9 callees=1 ================= */
/* CALLERS: FUN_140002c00, FUN_140002cb0, FUN_140002cd0, FUN_140002cf0, FUN_140002d40, FUN_140002d80, FUN_140002e50, FUN_140002e80, FUN_140002eb0 */
/* CALLEES: FUN_140007320 */

undefined8 FUN_140005560(undefined8 param_1)

{
  int iVar1;
  undefined8 local_res10;
  undefined1 local_res18 [16];
  
  iVar1 = FUN_140007320(param_1,local_res18,&local_res10);
  if (iVar1 == 0) {
    local_res10 = 0;
  }
  return local_res10;
}


/* ================= FUN_140005590 @ 140005590  size=49  callers=1 callees=1 ================= */
/* CALLERS: FUN_140004970 */
/* CALLEES: FUN_140004df0 */

undefined8 FUN_140005590(longlong param_1)

{
  undefined8 uVar1;
  undefined8 uVar2;
  char local_res8 [32];
  
  if (param_1 == 0) {
    return 0;
  }
  uVar1 = FUN_140004df0(param_1,0x31,local_res8);
  uVar2 = 0;
  if (local_res8[0] != '\0') {
    uVar2 = uVar1;
  }
  return uVar2;
}


/* ================= FUN_1400055d0 @ 1400055d0  size=342  callers=1 callees=6 ================= */
/* CALLERS: DDX_FieldText */
/* CALLEES: FUN_140004240, FUN_1400045e0, FUN_140009050, FUN_140009390, FUN_1400181c0, FUN_140018540 */

undefined8 FUN_1400055d0(longlong param_1,int param_2,ulonglong *param_3)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  ulonglong uVar4;
  ulonglong uVar5;
  char local_res20 [8];
  ulonglong local_28 [2];
  
  uVar3 = FUN_140004240(param_1,0x11,local_res20,0);
  if (local_res20[0] != '\0') {
    cVar1 = FUN_140009050(uVar3);
    if (((cVar1 == '\0') ||
        (iVar2 = FUN_1400181c0(uVar3,local_28,0), uVar4 = local_28[0], iVar2 != 0)) &&
       ((cVar1 = FUN_140009390(uVar3), cVar1 == '\0' ||
        (iVar2 = FUN_140018540(uVar3,local_28,0), uVar4 = local_28[0], iVar2 != 0)))) {
      uVar4 = 0xffffffffffffffff;
    }
    FUN_1400045e0(*(undefined8 *)(param_1 + 0x48),uVar3);
    uVar3 = FUN_140004240(param_1,0x12,local_res20,0);
    if (local_res20[0] != '\0') {
      cVar1 = FUN_140009050(uVar3);
      if ((cVar1 == '\0') ||
         (iVar2 = FUN_1400181c0(uVar3,local_28,0), uVar5 = local_28[0], iVar2 != 0)) {
        cVar1 = FUN_140009390(uVar3);
        if ((cVar1 == '\0') || (iVar2 = FUN_140018540(uVar3,local_28,0), iVar2 != 0)) {
          uVar5 = 0;
        }
        else {
          uVar5 = local_28[0] + uVar4;
        }
      }
      FUN_1400045e0(*(undefined8 *)(param_1 + 0x48),uVar3);
      if (uVar4 < uVar5) {
        if ((param_3 != (ulonglong *)0x0) && (param_2 != 0)) {
          *param_3 = uVar4;
          param_3[1] = uVar5;
        }
        return 1;
      }
    }
  }
  return 0;
}


/* ================= DDX_FieldText @ 140005730  size=61  callers=2 callees=2 ================= */
/* CALLERS: FUN_140006ef0, FUN_140008b90 */
/* CALLEES: FUN_1400055d0, FUN_140005770 */

/* Library Function - Multiple Matches With Same Base Name
    void __cdecl DDX_FieldText(class CDataExchange * __ptr64,int,unsigned char & __ptr64,class
   CRecordset * __ptr64)
    void __cdecl DDX_FieldText(class CDataExchange * __ptr64,int,int & __ptr64,class CRecordset *
   __ptr64)
    void __cdecl DDX_FieldText(class CDataExchange * __ptr64,int,unsigned int & __ptr64,class
   CRecordset * __ptr64)
    void __cdecl DDX_FieldText(class CDataExchange * __ptr64,int,long & __ptr64,class CRecordset *
   __ptr64)
     5 names - too many to list
   
   Libraries: Visual Studio 2008 Release, Visual Studio 2010 Release, Visual Studio 2012 Release,
   Visual Studio 2015 Release */

void DDX_FieldText(undefined8 param_1,undefined4 param_2,undefined8 param_3)

{
  int iVar1;
  
  iVar1 = FUN_1400055d0();
  if (iVar1 == 0) {
    FUN_140005770(param_1,param_2,param_3);
  }
  return;
}


/* ================= FUN_140005770 @ 140005770  size=306  callers=1 callees=4 ================= */
/* CALLERS: DDX_FieldText */
/* CALLEES: FUN_140004f60, FUN_140005a20, FUN_14000a630, FUN_140019870 */

/* WARNING: Removing unreachable block (ram,0x0001400057c4) */
/* WARNING: Removing unreachable block (ram,0x0001400057fd) */
/* WARNING: Removing unreachable block (ram,0x000140005814) */
/* WARNING: Removing unreachable block (ram,0x000140005818) */
/* WARNING: Removing unreachable block (ram,0x00014000581d) */
/* WARNING: Removing unreachable block (ram,0x000140005826) */
/* WARNING: Removing unreachable block (ram,0x000140005833) */
/* WARNING: Removing unreachable block (ram,0x000140005846) */
/* WARNING: Removing unreachable block (ram,0x00014000584b) */
/* WARNING: Removing unreachable block (ram,0x000140005837) */
/* WARNING: Removing unreachable block (ram,0x000140005852) */
/* WARNING: Removing unreachable block (ram,0x000140005857) */
/* WARNING: Removing unreachable block (ram,0x00014000585c) */
/* WARNING: Removing unreachable block (ram,0x000140005861) */
/* WARNING: Removing unreachable block (ram,0x000140005866) */
/* WARNING: Removing unreachable block (ram,0x00014000586a) */
/* WARNING: Removing unreachable block (ram,0x000140005878) */
/* WARNING: Removing unreachable block (ram,0x00014000582e) */
/* WARNING: Removing unreachable block (ram,0x00014000587a) */
/* WARNING: Removing unreachable block (ram,0x000140005884) */

undefined8 FUN_140005770(longlong param_1)

{
  char cVar1;
  
  if ((param_1 != 0) && (cVar1 = FUN_14000a630(), cVar1 != '\0')) {
    FUN_140004f60();
  }
  return 0;
}


/* ================= FUN_1400058b0 @ 1400058b0  size=81  callers=1 callees=4 ================= */
/* CALLERS: FUN_140005e90 */
/* CALLEES: FUN_1400048e0, FUN_140005960, FUN_140006580, FUN_14000a610 */

undefined8 FUN_1400058b0(longlong param_1)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined8 uVar4;
  undefined8 local_res8 [4];
  
  if (param_1 != 0) {
    lVar3 = FUN_1400048e0();
    if (lVar3 != 0) {
      iVar2 = FUN_140006580(param_1,local_res8);
      if (iVar2 != 0) {
        cVar1 = FUN_14000a610();
        uVar4 = FUN_140005960(local_res8[0],lVar3,cVar1 == '\0');
        return uVar4;
      }
    }
  }
  return 0;
}


/* ================= FUN_140005910 @ 140005910  size=20  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000dd10 */

longlong FUN_140005910(longlong param_1,longlong param_2,char param_3)

{
  if (param_3 != '\0') {
    return param_2;
  }
  return *(longlong *)(param_1 + 0x98) + param_2;
}


/* ================= FUN_140005930 @ 140005930  size=40  callers=1 callees=0 ================= */
/* CALLERS: FUN_140005df0 */

ulonglong FUN_140005930(longlong param_1,ulonglong param_2)

{
  if ((param_2 >> 0x30 & 1) != 0) {
    return param_2 & 0xffffffffffff;
  }
  return *(longlong *)(param_1 + 0x98) + (param_2 & 0xffffffffffff);
}


/* ================= FUN_140005960 @ 140005960  size=184  callers=9 callees=6 ================= */
/* CALLERS: FUN_140004970, FUN_140005430, FUN_1400054c0, FUN_1400058b0, FUN_140005be0, FUN_140005e90, FUN_140008090, FUN_1400080f0, FUN_14000d820 */
/* CALLEES: FUN_140004070, FUN_140004600, FUN_140005df0, FUN_140006900, FUN_14000a2f0, FUN_14000dd10 */

longlong FUN_140005960(longlong param_1,longlong param_2,undefined1 param_3)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  longlong lVar4;
  longlong lVar5;
  
  cVar1 = FUN_14000a2f0();
  if ((cVar1 == '\0') || (param_2 == 0)) {
    return 0;
  }
  uVar3 = FUN_140006900(param_2,param_3);
  lVar4 = FUN_140005df0(param_1,uVar3);
  if ((lVar4 == 0) &&
     (lVar5 = FUN_140004070(*(undefined8 *)(param_1 + 0x78),param_2,param_3,1,0), lVar4 = 0,
     lVar5 != 0)) {
    if ((*(longlong *)(lVar5 + 0x20) == param_2) &&
       (iVar2 = FUN_14000dd10(param_1,lVar5), iVar2 != 0)) {
      return lVar5;
    }
    FUN_140004600(lVar5);
    lVar4 = 0;
  }
  return lVar4;
}


/* ================= FUN_140005a20 @ 140005a20  size=135  callers=3 callees=5 ================= */
/* CALLERS: FUN_140005770, FUN_140005a20, FUN_140006210 */
/* CALLEES: FUN_140004df0, FUN_140005a20, FUN_140005e90, FUN_140006fb0, FUN_14000a630 */

undefined8 FUN_140005a20(longlong param_1)

{
  char cVar1;
  longlong lVar2;
  undefined8 uVar3;
  char local_res8 [8];
  
  uVar3 = 0xffffffffffffffff;
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      lVar2 = FUN_140006fb0();
      if (lVar2 == 0x11) {
        uVar3 = FUN_140004df0(param_1,0x11,local_res8);
        if (local_res8[0] == '\0') {
          return 0xffffffffffffffff;
        }
      }
      else if (lVar2 != 0xd) {
        lVar2 = FUN_140005e90(param_1);
        if (lVar2 != 0) {
          uVar3 = FUN_140005a20(lVar2);
        }
      }
    }
  }
  return uVar3;
}


/* ================= FUN_140005ab0 @ 140005ab0  size=299  callers=1 callees=9 ================= */
/* CALLERS: FUN_140002cb0 */
/* CALLEES: FUN_140004df0, FUN_140005be0, FUN_140005dc0, FUN_140005e30, FUN_140006580, FUN_140006e30, FUN_140008090, FUN_140009ce0, FUN_14000a630 */

longlong FUN_140005ab0(longlong param_1)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  longlong lVar4;
  undefined8 uVar5;
  longlong lVar6;
  longlong lVar7;
  char local_res10 [8];
  undefined8 local_res18;
  
  if ((DAT_140097bf0 & 1) != 0) {
    lVar3 = FUN_140005be0();
    return lVar3;
  }
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      iVar2 = FUN_140006580();
      if (iVar2 != 0) {
        lVar3 = FUN_140004df0();
        if (local_res10[0] == '\0') {
          lVar3 = FUN_140004df0(param_1,0xc,local_res10);
          if (local_res10[0] == '\0') {
            return 0;
          }
        }
        else {
          cVar1 = FUN_140009ce0(local_res18);
          if (cVar1 == '\0') {
            return lVar3;
          }
        }
        lVar4 = FUN_140005e30(param_1);
        if (lVar4 == 0) {
          uVar5 = FUN_140008090(param_1);
          lVar4 = FUN_140006e30(uVar5);
        }
        lVar6 = FUN_140005dc0(param_1);
        if (lVar6 != 0) {
          lVar7 = (0x20 - lVar6) - lVar3;
          if ((ulonglong)(lVar6 + lVar3) <= (ulonglong)(lVar4 * 8)) {
            lVar7 = (lVar4 * 8 - lVar6) - lVar3;
          }
          return lVar7;
        }
        return lVar3;
      }
    }
  }
  return 0;
}


/* ================= FUN_140005be0 @ 140005be0  size=465  callers=1 callees=10 ================= */
/* CALLERS: FUN_140005ab0 */
/* CALLEES: FUN_140004df0, FUN_140005960, FUN_140005dc0, FUN_140005e30, FUN_140006580, FUN_140006e30, FUN_140008090, FUN_140009ce0, FUN_14000a610, FUN_14000a630 */

longlong FUN_140005be0(longlong param_1)

{
  char cVar1;
  char cVar2;
  int iVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  ulonglong uVar7;
  longlong lVar8;
  bool bVar9;
  char local_res8 [8];
  undefined8 local_res10;
  
  if (((param_1 != 0) && (cVar1 = FUN_14000a630(), cVar1 != '\0')) &&
     (iVar3 = FUN_140006580(), iVar3 != 0)) {
    lVar4 = FUN_140004df0();
    if (local_res8[0] == '\0') {
      lVar4 = FUN_140004df0(param_1,0xc,local_res8);
      cVar1 = local_res8[0];
      if (local_res8[0] == '\0') {
        return 0;
      }
      cVar2 = FUN_140009ce0(local_res10);
      uVar7 = (ulonglong)(cVar2 == '\0');
    }
    else {
      uVar7 = 0;
      cVar1 = local_res8[0];
    }
    lVar8 = 1;
    lVar5 = FUN_140008090(param_1);
    if (lVar5 != 0) {
      lVar6 = lVar5;
      cVar2 = FUN_14000a610();
      lVar6 = FUN_140005960(local_res10,*(undefined8 *)(lVar6 + 0x28),cVar2 == '\0');
      if (lVar6 != 0) {
        uVar7 = FUN_140004df0(lVar6,0x6b,local_res8);
        lVar8 = FUN_140005dc0(lVar6);
        cVar1 = local_res8[0];
        if (local_res8[0] == '\0') {
          uVar7 = FUN_140004df0(lVar6,0xc,local_res8);
          cVar1 = local_res8[0];
        }
      }
    }
    cVar2 = FUN_140009ce0(local_res10);
    if (cVar2 == '\0') {
      bVar9 = uVar7 == 0;
    }
    else {
      bVar9 = uVar7 != 0;
    }
    if (bVar9) {
      lVar6 = FUN_140005e30(param_1);
      if (lVar6 == 0) {
        lVar6 = FUN_140006e30(lVar5);
      }
      lVar5 = lVar4 + ((lVar6 * 8 - lVar8) - uVar7);
    }
    else {
      lVar8 = FUN_140005e30(param_1);
      if (lVar8 == 0) {
        lVar8 = FUN_140006e30(lVar5);
      }
      lVar6 = FUN_140005dc0(param_1);
      lVar5 = lVar4;
      if (lVar6 != 0) {
        lVar5 = (0x20 - lVar6) - lVar4;
        if ((ulonglong)(lVar6 + lVar4) <= (ulonglong)(lVar8 * 8)) {
          lVar5 = (lVar8 * 8 - lVar6) - lVar4;
        }
      }
    }
    if (cVar1 != '\0') {
      return lVar5;
    }
  }
  return 0;
}


/* ================= FUN_140005dc0 @ 140005dc0  size=47  callers=3 callees=2 ================= */
/* CALLERS: FUN_140002cd0, FUN_140005ab0, FUN_140005be0 */
/* CALLEES: FUN_140004df0, FUN_14000a630 */

undefined8 FUN_140005dc0(longlong param_1)

{
  char cVar1;
  undefined8 uVar2;
  char local_res8;
  
  if (((param_1 != 0) && (cVar1 = FUN_14000a630(), cVar1 != '\0')) &&
     (uVar2 = FUN_140004df0(), local_res8 != '\0')) {
    return uVar2;
  }
  return 0;
}


/* ================= FUN_140005df0 @ 140005df0  size=61  callers=8 callees=3 ================= */
/* CALLERS: FUN_140004aa0, FUN_140004af0, FUN_140004b40, FUN_140005960, FUN_140006730, FUN_140007320, FUN_140007b10, FUN_140007c00 */
/* CALLEES: FUN_140002050, FUN_140005930, FUN_14000a640 */

undefined8 FUN_140005df0(longlong param_1)

{
  char cVar1;
  undefined8 local_res18;
  
  FUN_140005930();
  cVar1 = FUN_14000a640();
  if (cVar1 == '\0') {
    return 0;
  }
  FUN_140002050(param_1 + 0x180);
  return local_res18;
}


/* ================= FUN_140005e30 @ 140005e30  size=88  callers=4 callees=2 ================= */
/* CALLERS: FUN_140005ab0, FUN_140005be0, FUN_140006e30, FUN_14000dc30 */
/* CALLEES: FUN_140004df0, FUN_14000a630 */

ulonglong FUN_140005e30(longlong param_1)

{
  char cVar1;
  ulonglong uVar2;
  char local_res8 [32];
  
  if ((param_1 != 0) && (cVar1 = FUN_14000a630(), cVar1 != '\0')) {
    uVar2 = FUN_140004df0();
    if (local_res8[0] != '\0') {
      return uVar2;
    }
    uVar2 = FUN_140004df0(param_1,0xd,local_res8);
    if (local_res8[0] != '\0') {
      return uVar2 >> 3;
    }
  }
  return 0;
}


/* ================= FUN_140005e90 @ 140005e90  size=95  callers=2 callees=4 ================= */
/* CALLERS: FUN_140005a20, FUN_1400065c0 */
/* CALLEES: FUN_1400058b0, FUN_140005960, FUN_140006580, FUN_14000a610 */

void FUN_140005e90(longlong param_1)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined8 local_res8 [4];
  
  lVar3 = *(longlong *)(param_1 + 0x50);
  local_res8[0] = 0;
  if (lVar3 != 0) {
    iVar2 = FUN_140006580(param_1,local_res8);
    if (iVar2 != 0) {
      cVar1 = FUN_14000a610();
      lVar3 = FUN_140005960(local_res8[0],*(undefined8 *)(lVar3 + 0x68),cVar1 == '\0');
      if (lVar3 != 0) {
        return;
      }
    }
  }
  FUN_1400058b0(param_1,0x11);
  return;
}


/* ================= FUN_140005ef0 @ 140005ef0  size=789  callers=2 callees=4 ================= */
/* CALLERS: FUN_140006b10, FUN_1400088e0 */
/* CALLEES: FUN_140002f50, FUN_140006210, FUN_140006fb0, FUN_14000a630 */

undefined8 * FUN_140005ef0(undefined8 *param_1,longlong param_2,undefined8 param_3)

{
  char cVar1;
  undefined2 uVar2;
  longlong lVar3;
  undefined8 *puVar4;
  short sVar5;
  undefined8 local_218;
  undefined8 uStack_210;
  undefined8 local_208;
  undefined8 uStack_200;
  undefined8 local_1f8;
  undefined8 uStack_1f0;
  undefined8 local_1e8;
  undefined8 uStack_1e0;
  undefined8 local_1d8;
  undefined8 uStack_1d0;
  undefined8 local_1c8;
  undefined8 uStack_1c0;
  undefined8 local_1b8;
  undefined8 uStack_1b0;
  undefined8 local_1a8;
  undefined8 uStack_1a0;
  undefined8 local_198;
  undefined8 uStack_190;
  undefined8 local_188;
  undefined8 uStack_180;
  undefined8 local_178;
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
  undefined8 local_118;
  undefined8 uStack_110;
  undefined8 local_108;
  undefined8 uStack_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined4 local_e8;
  undefined4 uStack_e4;
  undefined4 uStack_e0;
  undefined4 uStack_dc;
  undefined4 local_d8;
  undefined4 uStack_d4;
  undefined4 uStack_d0;
  undefined4 uStack_cc;
  undefined8 local_c8;
  undefined1 local_b8 [176];
  
  if (param_2 != 0) {
    cVar1 = FUN_14000a630(param_2);
    if (cVar1 != '\0') {
      sVar5 = 0x3fff;
      lVar3 = FUN_140006fb0();
      if (lVar3 == 0xd) {
        sVar5 = 2;
LAB_140005f6e:
        uVar2 = 0x38;
      }
      else {
        if (lVar3 == 0x1c) goto LAB_140005f6e;
        if ((lVar3 == 0x1d) || (lVar3 == 0x2e)) {
          uVar2 = 0x40;
          sVar5 = 0x3fff;
        }
        else {
          uVar2 = 2;
        }
      }
      puVar4 = (undefined8 *)FUN_140006210(local_b8,param_2,param_3,uVar2);
      local_218 = *puVar4;
      uStack_210 = puVar4[1];
      local_208 = puVar4[2];
      uStack_200 = puVar4[3];
      local_1f8 = puVar4[4];
      uStack_1f0 = puVar4[5];
      local_1e8 = puVar4[6];
      uStack_1e0 = puVar4[7];
      local_1d8 = puVar4[8];
      uStack_1d0 = puVar4[9];
      local_1c8 = puVar4[10];
      uStack_1c0 = puVar4[0xb];
      local_1b8 = puVar4[0xc];
      uStack_1b0 = puVar4[0xd];
      local_198 = puVar4[0x10];
      uStack_190 = puVar4[0x11];
      local_1a8 = puVar4[0xe];
      uStack_1a0 = puVar4[0xf];
      local_188 = puVar4[0x12];
      uStack_180 = puVar4[0x13];
      local_178 = puVar4[0x14];
      local_158 = puVar4[2];
      uStack_150 = puVar4[3];
      local_168 = *puVar4;
      uStack_160 = puVar4[1];
      local_c8 = puVar4[0x14];
      local_148 = puVar4[4];
      uStack_140 = puVar4[5];
      local_138 = puVar4[6];
      uStack_130 = puVar4[7];
      local_128 = puVar4[8];
      uStack_120 = puVar4[9];
      local_118 = puVar4[10];
      uStack_110 = puVar4[0xb];
      local_108 = puVar4[0xc];
      uStack_100 = puVar4[0xd];
      local_f8 = puVar4[0xe];
      uStack_f0 = puVar4[0xf];
      local_e8 = *(undefined4 *)(puVar4 + 0x10);
      uStack_e4 = *(undefined4 *)((longlong)puVar4 + 0x84);
      uStack_e0 = *(undefined4 *)(puVar4 + 0x11);
      uStack_dc = *(undefined4 *)((longlong)puVar4 + 0x8c);
      local_d8 = *(undefined4 *)(puVar4 + 0x12);
      uStack_d4 = *(undefined4 *)((longlong)puVar4 + 0x94);
      uStack_d0 = *(undefined4 *)(puVar4 + 0x13);
      uStack_cc = *(undefined4 *)((longlong)puVar4 + 0x9c);
      cVar1 = FUN_140002f50(&local_168);
      if ((cVar1 == '\0') && (sVar5 != 0x3fff)) {
        puVar4 = (undefined8 *)FUN_140006210(local_b8,param_2,param_3,sVar5);
        local_218 = *puVar4;
        uStack_210 = puVar4[1];
        local_208 = puVar4[2];
        uStack_200 = puVar4[3];
        local_1f8 = puVar4[4];
        uStack_1f0 = puVar4[5];
        local_1e8 = puVar4[6];
        uStack_1e0 = puVar4[7];
        local_1d8 = puVar4[8];
        uStack_1d0 = puVar4[9];
        local_1c8 = puVar4[10];
        uStack_1c0 = puVar4[0xb];
        local_1b8 = puVar4[0xc];
        uStack_1b0 = puVar4[0xd];
        local_1a8 = puVar4[0xe];
        uStack_1a0 = puVar4[0xf];
        local_198 = puVar4[0x10];
        uStack_190 = puVar4[0x11];
        local_188 = puVar4[0x12];
        uStack_180 = puVar4[0x13];
        local_178 = puVar4[0x14];
      }
      puVar4 = &local_218;
      *param_1 = local_218;
      param_1[1] = uStack_210;
      param_1[2] = local_208;
      param_1[3] = uStack_200;
      param_1[4] = local_1f8;
      param_1[5] = uStack_1f0;
      param_1[6] = local_1e8;
      param_1[7] = uStack_1e0;
      param_1[8] = local_1d8;
      param_1[9] = uStack_1d0;
      param_1[10] = local_1c8;
      param_1[0xb] = uStack_1c0;
      param_1[0xc] = local_1b8;
      param_1[0xd] = uStack_1b0;
      param_1[0xe] = local_1a8;
      param_1[0xf] = uStack_1a0;
      param_1[0x10] = local_198;
      param_1[0x11] = uStack_190;
      param_1[0x12] = local_188;
      param_1[0x13] = uStack_180;
      goto LAB_1400061df;
    }
  }
  puVar4 = (undefined8 *)&DAT_14005ec20;
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
  param_1[0x10] = 0;
  param_1[0x11] = 0;
  param_1[0x12] = 0;
  param_1[0x13] = 0;
LAB_1400061df:
  param_1[0x14] = puVar4[0x14];
  return param_1;
}


/* ================= FUN_140006210 @ 140006210  size=683  callers=2 callees=15 ================= */
/* CALLERS: FUN_140005ef0, FUN_140009120 */
/* CALLEES: FUN_140003cc0, FUN_140004240, FUN_1400045e0, FUN_140004600, FUN_140004df0, FUN_140005a20, FUN_140006fb0, FUN_140008090, FUN_140009290, FUN_14000a630, FUN_14000be20, FUN_140018270, FUN_1400191a0, FUN_140019250, free */

undefined8 * FUN_140006210(undefined8 *param_1,longlong param_2,undefined8 param_3,short param_4)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  longlong lVar6;
  void **ppvVar7;
  undefined4 extraout_XMM0_Da;
  char local_res8 [8];
  char local_res10 [8];
  void *local_68;
  undefined8 local_60;
  undefined8 local_58;
  undefined8 local_50;
  void **local_48 [2];
  
  local_res10[0] = '\0';
  local_res8[0] = '\0';
  local_60 = 0;
  local_58 = 0;
  local_50 = 0;
  local_48[0] = (void **)0x0;
  local_68 = (void *)0x0;
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
  if (param_2 == 0) {
    return param_1;
  }
  cVar1 = FUN_14000a630(param_2);
  if (cVar1 == '\0') {
    return param_1;
  }
  lVar3 = FUN_140003cc0();
  if (lVar3 == 0) {
    return param_1;
  }
  uVar4 = FUN_140004240(lVar3,param_4,local_res8,0);
  if (local_res8[0] == '\0') {
LAB_1400063e2:
    if (param_4 == 0x38) {
      uVar5 = FUN_140008090(param_2);
      lVar6 = FUN_140006fb0(uVar5);
      if (lVar6 == 0x17) {
        *(undefined4 *)param_1 = 1;
        param_1[2] = 0;
        *(undefined4 *)(param_1 + 4) = 0;
        *(undefined4 *)(param_1 + 1) = 0xe;
        param_1[5] = 0;
      }
      if (local_res8[0] != '\0') goto LAB_14000641d;
    }
  }
  else {
    cVar1 = FUN_140009290(uVar4);
    if ((cVar1 == '\0') ||
       (iVar2 = FUN_140018270(extraout_XMM0_Da,&local_58,&local_50,0), iVar2 != 0)) {
      iVar2 = FUN_140019250(uVar4,local_48,&local_60,0);
      if (iVar2 != 0) goto LAB_1400063d4;
LAB_140006377:
      lVar6 = FUN_140005a20(param_2);
      if (lVar6 == -1) {
        lVar6 = 0;
      }
      if (local_68 == (void *)0x0) {
        uVar5 = local_60;
        ppvVar7 = local_48[0];
        if (local_48[0] == (void **)0x0) goto LAB_140006487;
      }
      else {
        uVar5 = 1;
        ppvVar7 = &local_68;
      }
      iVar2 = FUN_14000be20(*(undefined8 *)(param_2 + 0x48),*(undefined8 *)(param_2 + 0x50),uVar5,
                            ppvVar7,param_3,lVar6,param_4,param_1);
      if (iVar2 == -1) goto LAB_1400063d4;
    }
    else {
      iVar2 = FUN_1400191a0(*(undefined8 *)(param_2 + 0x48),*(undefined8 *)(param_2 + 0x50),local_50
                            ,local_58,&local_68,&local_60,0);
      if (iVar2 == 0) goto LAB_140006377;
LAB_1400063d4:
      if (local_res8[0] == '\0') goto LAB_1400063e2;
      if (param_4 != 0x38) goto LAB_14000645e;
LAB_14000641d:
      lVar6 = FUN_140004df0(param_2,0x38,local_res10);
      if ((local_res10[0] != '\0') ||
         (FUN_140004df0(param_2,0x6b,local_res10), lVar6 = 0, local_res10[0] != '\0')) {
        *(undefined4 *)param_1 = 1;
        *(undefined4 *)(param_1 + 1) = 0xe;
        param_1[5] = lVar6;
      }
    }
  }
LAB_14000645e:
  if (local_68 != (void *)0x0) {
    free(*(void **)((longlong)local_68 + 0x18));
    *(undefined8 *)((longlong)local_68 + 0x18) = 0;
    free(local_68);
    local_68 = (void *)0x0;
  }
LAB_140006487:
  if (local_res8[0] != '\0') {
    FUN_1400045e0(*(undefined8 *)(param_2 + 0x48),uVar4);
  }
  FUN_140004600(lVar3);
  return param_1;
}


/* ================= FUN_1400064c0 @ 1400064c0  size=126  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400065c0 */
/* CALLEES: FUN_1400090c0, FUN_14000a630, FUN_140018cf0 */

undefined8 FUN_1400064c0(longlong param_1,undefined4 *param_2)

{
  char cVar1;
  int iVar2;
  longlong local_res8;
  undefined8 local_res10;
  
  if (param_2 != (undefined4 *)0x0) {
    *param_2 = 0;
  }
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      cVar1 = FUN_1400090c0();
      if (cVar1 != '\0') {
        iVar2 = FUN_140018cf0(param_1,&local_res10,&local_res8,0);
        if ((iVar2 == 0) && (-1 < local_res8)) {
          if (param_2 != (undefined4 *)0x0) {
            *param_2 = (int)local_res8;
          }
          return local_res10;
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_140006540 @ 140006540  size=28  callers=2 callees=1 ================= */
/* CALLERS: FUN_140004970, FUN_140005430 */
/* CALLEES: FUN_140006920 */

longlong FUN_140006540(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0;
  }
  return (longlong)*(int *)(lVar1 + 0xc);
}


/* ================= FUN_140006560 @ 140006560  size=28  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400054c0, FUN_140009da0 */
/* CALLEES: FUN_140006920 */

longlong FUN_140006560(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0;
  }
  return (longlong)*(int *)(lVar1 + 0x10);
}


/* ================= FUN_140006580 @ 140006580  size=61  callers=28 callees=1 ================= */
/* CALLERS: FUN_140003cc0, FUN_140003fc0, FUN_140004240, FUN_1400044f0, FUN_140004600, FUN_1400048e0, FUN_140004920, FUN_140004bf0, FUN_140005260, FUN_140005380, FUN_140005430, FUN_1400054c0, FUN_1400058b0, FUN_140005ab0, FUN_140005be0, FUN_140005e90, FUN_140006a00, FUN_140006a40, FUN_140006b10, FUN_140006fb0, FUN_1400070e0, FUN_140007a60, FUN_140007aa0, FUN_140008090, FUN_1400080f0, FUN_140009da0, FUN_14000ddf0, FUN_14000ded0 */
/* CALLEES: FUN_14000a630 */

undefined8 FUN_140006580(longlong param_1,undefined8 *param_2)

{
  char cVar1;
  
  if (param_2 != (undefined8 *)0x0) {
    cVar1 = FUN_14000a630();
    if ((cVar1 != '\0') && (*(longlong *)(param_1 + 0x48) != 0)) {
      *param_2 = *(undefined8 *)(*(longlong *)(param_1 + 0x48) + 0x338);
      return 1;
    }
    *param_2 = 0;
  }
  return 0;
}


/* ================= FUN_1400065c0 @ 1400065c0  size=357  callers=1 callees=9 ================= */
/* CALLERS: FUN_140002cf0 */
/* CALLEES: FUN_140004d70, FUN_140004df0, FUN_140005e90, FUN_1400064c0, FUN_140006c00, FUN_14000a310, FUN_14000a630, FUN_14000e500, snprintf */

undefined1 * FUN_1400065c0(longlong param_1,undefined2 param_2)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  longlong lVar4;
  longlong lVar5;
  undefined8 uVar6;
  char local_res8;
  undefined7 uStackX_9;
  uint local_res18 [4];
  
  if (param_1 == 0) {
    return &DAT_14005f306;
  }
  cVar1 = FUN_14000a630();
  if (cVar1 == '\0') {
    return &DAT_14005f306;
  }
  local_res18[0] = 0;
  lVar4 = FUN_140005e90();
  uVar2 = FUN_140004df0(param_1,param_2,&local_res8);
  if (local_res8 == '\0') {
    uVar2 = 0xffffffff;
  }
  else if (*(ushort *)(*(longlong *)(lVar4 + 0x50) + 0x12) < 5) {
    uVar2 = uVar2 - 1;
  }
  lVar5 = FUN_1400064c0(lVar4,local_res18);
  DAT_140098410 = '\0';
  DAT_140097c10 = 0;
  if (uVar2 < local_res18[0]) {
    FUN_14000e500(&DAT_140098410,*(undefined8 *)(lVar5 + (ulonglong)uVar2 * 8),0x7ff);
    if (DAT_140098410 == '\0') {
      return &DAT_14005f306;
    }
  }
  else {
    lVar5 = FUN_140006c00(lVar4,0);
    if (lVar5 == 0) {
      return &DAT_14005f306;
    }
    uVar6 = FUN_140006c00(lVar4,0);
    FUN_14000e500(&DAT_140098410,uVar6,0x7ff);
    if (DAT_140098410 == '\0') {
      return &DAT_14005f306;
    }
    iVar3 = FUN_14000a310(&DAT_140098410);
    if (((iVar3 != 0) && (iVar3 = FUN_140004d70(lVar4,0x1b,&local_res8), iVar3 == 0)) &&
       (iVar3 = FUN_14000a310(CONCAT71(uStackX_9,local_res8)), iVar3 == 0)) {
      snprintf(&DAT_140097c10,0x7ff,"%s/%s",CONCAT71(uStackX_9,local_res8),&DAT_140098410);
      return &DAT_140097c10;
    }
  }
  return &DAT_140098410;
}


/* ================= FUN_140006730 @ 140006730  size=307  callers=3 callees=3 ================= */
/* CALLERS: FUN_1400079e0, FUN_140007b60, FUN_140007e90 */
/* CALLEES: FUN_140002210, FUN_140005df0, FUN_14000a2f0 */

undefined8 FUN_140006730(longlong param_1,undefined8 param_2,uint param_3)

{
  char cVar1;
  undefined8 uVar2;
  ulonglong uVar3;
  
  uVar3 = (ulonglong)param_3;
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    return 0;
  }
  switch(param_2) {
  case 0x10:
    uVar2 = FUN_140002210(param_1 + 0x308,uVar3);
    uVar2 = FUN_140005df0(param_1,uVar2);
    return uVar2;
  case 0x11:
    uVar2 = FUN_140002210(param_1 + 0x340,uVar3);
    uVar2 = FUN_140005df0(param_1,uVar2);
    return uVar2;
  default:
    uVar2 = FUN_140005df0(param_1,0);
    return uVar2;
  case 0x1d:
    uVar2 = FUN_140002210(param_1 + 0x3b0,uVar3);
    uVar2 = FUN_140005df0(param_1,uVar2);
    return uVar2;
  case 0x1e:
    uVar2 = FUN_140002210(param_1 + 0x2d0,uVar3);
    uVar2 = FUN_140005df0(param_1,uVar2);
    return uVar2;
  case 0x2e:
    uVar2 = FUN_140002210(param_1 + 0x378,uVar3);
    uVar2 = FUN_140005df0(param_1,uVar2);
    return uVar2;
  case 0x34:
    uVar2 = FUN_140002210(param_1 + 0x260,uVar3);
    uVar2 = FUN_140005df0(param_1,uVar2);
    return uVar2;
  case 0x39:
    uVar2 = FUN_140002210(param_1 + 0x298,uVar3);
    uVar2 = FUN_140005df0(param_1,uVar2);
    return uVar2;
  }
}


/* ================= FUN_1400068b0 @ 1400068b0  size=76  callers=23 callees=0 ================= */
/* CALLERS: FUN_140003fc0, FUN_1400044f0, FUN_140005430, FUN_1400054c0, FUN_140006a00, FUN_140006fb0, FUN_1400070e0, FUN_1400073c0, FUN_140007a60, FUN_140007aa0, FUN_140009620, FUN_1400096a0, FUN_140009730, FUN_140009850, FUN_1400098d0, FUN_140009a00, FUN_140009b80, FUN_140009bd0, FUN_140009c40, FUN_140009da0, FUN_14000aaf0, FUN_14000ded0, FUN_14000e0d0 */

ulonglong FUN_1400068b0(longlong param_1)

{
  if (param_1 == 0) {
    return 0;
  }
  if ((*(longlong *)(param_1 + 0x50) != 0) && (*(int *)(*(longlong *)(param_1 + 0x50) + 0x88) == 0))
  {
    return *(ulonglong *)(param_1 + 0x20) & 0xffffffffffff;
  }
  return *(ulonglong *)(param_1 + 0x20) & 0xffffffffffff | 0x1000000000000;
}


/* ================= FUN_140006900 @ 140006900  size=27  callers=9 callees=0 ================= */
/* CALLERS: FUN_140004970, FUN_140005960, FUN_140006a40, FUN_140006d30, FUN_140007070, FUN_140009730, FUN_1400098d0, FUN_140009a00, FUN_14000de50 */

ulonglong FUN_140006900(ulonglong param_1,uint param_2)

{
  return (ulonglong)(param_2 & 1) << 0x30 | param_1 & 0xffffffffffff;
}


/* ================= FUN_140006920 @ 140006920  size=183  callers=19 callees=5 ================= */
/* CALLERS: FUN_140006540, FUN_140006560, FUN_1400069e0, FUN_140006d10, FUN_1400070c0, FUN_140007210, FUN_140007ae0, FUN_140007b10, FUN_140007bd0, FUN_140007c00, FUN_1400097d0, FUN_140009970, FUN_140009af0, FUN_14000da60, FUN_14000da90, FUN_14000dac0, FUN_14000daf0, FUN_14000db20, FUN_14000db50 */
/* CALLEES: FUN_140002050, FUN_1400021c0, FUN_1400023e0, FUN_1400078f0, FUN_140019ae0 */

longlong FUN_140006920(longlong param_1,ulonglong param_2)

{
  int iVar1;
  longlong *plVar2;
  ulonglong uVar3;
  longlong local_res18 [2];
  
  iVar1 = FUN_1400078f0();
  if (iVar1 == -1) {
    return 0;
  }
  uVar3 = param_2 & 0xffffffffffff;
  if ((param_2 >> 0x30 & 1) == 0) {
    uVar3 = uVar3 + *(longlong *)(param_1 + 0x98);
  }
  if (uVar3 < *(ulonglong *)(param_1 + 0xa8)) {
    param_1 = param_1 + 0x1b8;
    plVar2 = (longlong *)FUN_1400021c0(param_1,uVar3);
    if ((plVar2 != (longlong *)0x0) && (local_res18[0] = *plVar2, local_res18[0] != 0)) {
      return local_res18[0];
    }
    local_res18[0] = FUN_140019ae0(0x38);
    FUN_1400023e0(param_1,uVar3,local_res18);
    FUN_140002050(param_1,uVar3,1,local_res18);
    return local_res18[0];
  }
  return 0;
}


/* ================= FUN_1400069e0 @ 1400069e0  size=28  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000e0d0 */
/* CALLEES: FUN_140006920 */

undefined1 FUN_1400069e0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0;
  }
  return *(undefined1 *)(lVar1 + 0x14);
}


/* ================= FUN_140006a00 @ 140006a00  size=55  callers=0 callees=3 ================= */
/* CALLEES: FUN_140006580, FUN_1400068b0, FUN_140007c00 */

undefined8 FUN_140006a00(longlong param_1,undefined4 param_2)

{
  int iVar1;
  undefined8 uVar2;
  undefined8 local_res8 [4];
  
  if (param_1 != 0) {
    iVar1 = FUN_140006580(param_1,local_res8,param_2);
    if (iVar1 != 0) {
      uVar2 = FUN_1400068b0();
      uVar2 = FUN_140007c00(local_res8[0],uVar2);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_140006a40 @ 140006a40  size=194  callers=2 callees=8 ================= */
/* CALLERS: FUN_140002e50, FUN_140006a40 */
/* CALLEES: FUN_140004df0, FUN_1400054c0, FUN_140006580, FUN_140006900, FUN_140006a40, FUN_140006d70, FUN_140007bd0, FUN_14000a610 */

undefined8 FUN_140006a40(longlong param_1,int param_2)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  undefined1 local_res8 [8];
  undefined8 local_res18;
  
  if (param_1 != 0) {
    iVar2 = FUN_140006580(param_1,&local_res18);
    if ((iVar2 != 0) && (param_2 < 0x15)) {
      cVar1 = FUN_14000a610();
      uVar3 = FUN_140006d70();
      uVar3 = FUN_140006900(uVar3,cVar1 == '\0');
      uVar3 = FUN_140007bd0(local_res18,uVar3);
      if ((int)uVar3 == 0) {
        uVar3 = FUN_1400054c0(param_1);
        uVar3 = FUN_140006a40(uVar3,param_2 + 1);
        if ((int)uVar3 == 0) {
          uVar3 = FUN_140004df0(param_1,0x47,local_res8);
          uVar3 = FUN_140006900(uVar3,cVar1 == '\0');
          uVar3 = FUN_140007bd0(local_res18,uVar3);
        }
      }
      return uVar3;
    }
  }
  return 0;
}


/* ================= FUN_140006b10 @ 140006b10  size=239  callers=1 callees=4 ================= */
/* CALLERS: FUN_140002e80 */
/* CALLEES: FUN_140005ef0, FUN_140006580, FUN_140006fb0, FUN_14000a2d0 */

longlong FUN_140006b10(longlong param_1)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 *puVar4;
  longlong lVar5;
  undefined1 local_res8 [32];
  int local_168;
  int iStack_160;
  undefined1 local_b8 [176];
  
  if (param_1 != 0) {
    iVar2 = FUN_140006580(param_1,local_res8);
    if (iVar2 != 0) {
      uVar3 = FUN_140006fb0();
      cVar1 = FUN_14000a2d0(uVar3);
      if (cVar1 != '\0') {
        puVar4 = (undefined8 *)FUN_140005ef0(local_b8,param_1,0);
        local_168 = (int)*puVar4;
        if ((local_168 == 1) && (iStack_160 = (int)puVar4[1], iStack_160 == 0xe)) {
          lVar5 = 0;
          if (0 < (longlong)puVar4[5]) {
            lVar5 = puVar4[5];
          }
          return lVar5;
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_140006c00 @ 140006c00  size=269  callers=4 callees=6 ================= */
/* CALLERS: FUN_140002fb0, FUN_1400065c0, FUN_140006c00, FUN_140008a60 */
/* CALLEES: FUN_1400054c0, FUN_140006c00, FUN_1400080f0, FUN_14000a630, FUN_14000e500, FUN_140015720 */

undefined * FUN_140006c00(longlong param_1,int param_2)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined *puVar4;
  undefined8 local_res8;
  
  if (((param_1 == 0) || (lVar3 = param_1, iVar2 = param_2, cVar1 = FUN_14000a630(), cVar1 == '\0'))
     || (4 < iVar2)) {
    return (undefined *)0x0;
  }
  lVar3 = *(longlong *)(lVar3 + 0x58);
  if (lVar3 == 0) {
    iVar2 = FUN_140015720();
    if (iVar2 == 0) {
      *(undefined8 *)(param_1 + 0x58) = local_res8;
    }
    lVar3 = *(longlong *)(param_1 + 0x58);
    if (lVar3 == 0) {
      lVar3 = FUN_1400080f0(param_1,0x31);
      if ((lVar3 != 0) && (*(longlong *)(lVar3 + 0x20) == *(longlong *)(param_1 + 0x20))) {
        return (undefined *)0x0;
      }
      puVar4 = (undefined *)FUN_140006c00(lVar3,param_2);
      if (puVar4 != (undefined *)0x0) {
        return puVar4;
      }
      lVar3 = FUN_1400080f0(param_1,0x47);
      if ((lVar3 != 0) && (*(longlong *)(lVar3 + 0x20) == *(longlong *)(param_1 + 0x20))) {
        return (undefined *)0x0;
      }
      puVar4 = (undefined *)FUN_140006c00(lVar3,param_2 + 1);
      if (puVar4 != (undefined *)0x0) {
        return puVar4;
      }
      lVar3 = FUN_1400054c0(param_1);
      if ((lVar3 != 0) && (*(longlong *)(lVar3 + 0x20) == *(longlong *)(param_1 + 0x20))) {
        return (undefined *)0x0;
      }
      puVar4 = (undefined *)FUN_140006c00(lVar3,param_2 + 2);
      if (puVar4 == (undefined *)0x0) {
        return (undefined *)0x0;
      }
      return puVar4;
    }
  }
  FUN_14000e500(&DAT_140098c90,lVar3,0x800);
  return &DAT_140098c90;
}


/* ================= FUN_140006d10 @ 140006d10  size=28  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400044f0, FUN_140006d30 */
/* CALLEES: FUN_140006920 */

longlong FUN_140006d10(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return 0;
  }
  return (longlong)*(int *)(lVar1 + 8);
}


/* ================= FUN_140006d30 @ 140006d30  size=57  callers=2 callees=3 ================= */
/* CALLERS: FUN_140004970, FUN_140008090 */
/* CALLEES: FUN_140006900, FUN_140006d10, FUN_14000a2f0 */

undefined8 FUN_140006d30(undefined8 param_1,undefined8 param_2,byte param_3)

{
  char cVar1;
  undefined8 uVar2;
  ulonglong uVar3;
  
  uVar3 = (ulonglong)param_3;
  cVar1 = FUN_14000a2f0();
  if (cVar1 == '\0') {
    return 0;
  }
  uVar2 = FUN_140006900(param_2,uVar3 & 0xff);
  uVar2 = FUN_140006d10(param_1,uVar2);
  return uVar2;
}


/* ================= FUN_140006d70 @ 140006d70  size=13  callers=6 callees=0 ================= */
/* CALLERS: FUN_140004920, FUN_140006a40, FUN_140006fb0, FUN_1400080f0, FUN_14000ddf0, FUN_14000e0d0 */

undefined8 FUN_140006d70(longlong param_1)

{
  if (param_1 == 0) {
    return 0;
  }
  return *(undefined8 *)(param_1 + 0x20);
}


/* ================= FUN_140006d80 @ 140006d80  size=127  callers=1 callees=3 ================= */
/* CALLERS: FUN_140002eb0 */
/* CALLEES: FUN_140004ce0, FUN_140008d40, FUN_14000a630 */

undefined8 FUN_140006d80(longlong param_1)

{
  char cVar1;
  longlong lVar2;
  undefined8 uVar3;
  char local_res8 [32];
  
  if ((((param_1 != 0) && (cVar1 = FUN_14000a630(), cVar1 != '\0')) &&
      (lVar2 = FUN_140008d40(), lVar2 == 0)) &&
     ((lVar2 = FUN_140008d40(param_1,0x24), lVar2 != 0 &&
      (uVar3 = FUN_140004ce0(lVar2,0x3e,local_res8), local_res8[0] != '\0')))) {
    switch(uVar3) {
    case 5:
    case 6:
    case 0xd:
      return 1;
    }
  }
  return 0;
}


/* ================= FUN_140006e30 @ 140006e30  size=184  callers=4 callees=5 ================= */
/* CALLERS: FUN_140005380, FUN_140005ab0, FUN_140005be0, FUN_140006ef0 */
/* CALLEES: FUN_140005e30, FUN_140006fb0, FUN_14000a630, FUN_14000b830, FUN_140016370 */

ulonglong FUN_140006e30(longlong param_1)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  ulonglong *puVar4;
  ulonglong uVar5;
  ushort local_res8 [16];
  undefined1 local_78 [112];
  
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      lVar3 = FUN_140006fb0();
      if (lVar3 != 0xf) {
        lVar3 = FUN_140006fb0(param_1);
        if (lVar3 != 0x10) {
          lVar3 = FUN_140006fb0(param_1);
          if (lVar3 == 1) {
            puVar4 = (ulonglong *)FUN_14000b830(local_78,param_1);
            return *puVar4;
          }
          uVar5 = FUN_140005e30(param_1);
          return uVar5;
        }
      }
      iVar2 = FUN_140016370(*(undefined8 *)(param_1 + 0x48),local_res8,0);
      if (iVar2 == 0) {
        return (ulonglong)local_res8[0];
      }
      return 4;
    }
  }
  return 0;
}


/* ================= FUN_140006ef0 @ 140006ef0  size=187  callers=2 callees=6 ================= */
/* CALLERS: FUN_140008cb0, FUN_14000b830 */
/* CALLEES: DDX_FieldText, FUN_140006e30, FUN_140006fb0, FUN_1400080f0, FUN_14000a630, FUN_14000e080 */

longlong FUN_140006ef0(longlong param_1)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  longlong lVar4;
  longlong local_res8;
  longlong local_18;
  longlong local_10;
  
  if ((param_1 != 0) && (lVar4 = param_1, cVar1 = FUN_14000a630(), cVar1 != '\0')) {
    lVar3 = 0;
    do {
      if (lVar3 != 0) {
        return lVar3;
      }
      lVar3 = FUN_140006e30(lVar4);
      lVar4 = FUN_1400080f0(lVar4,0x49);
    } while (lVar4 != 0);
    if ((lVar3 == 0) && (iVar2 = DDX_FieldText(param_1,1,&local_18), iVar2 != 0)) {
      local_res8 = local_10 - local_18;
      lVar4 = FUN_140006fb0(param_1);
      lVar3 = local_10 - local_18;
      if (lVar4 != 0x11) {
        FUN_14000e080(&local_res8,local_18);
        lVar3 = local_res8;
      }
    }
    return lVar3;
  }
  return 0;
}


/* ================= FUN_140006fb0 @ 140006fb0  size=192  callers=18 callees=7 ================= */
/* CALLERS: FUN_140002fb0, FUN_140004240, FUN_140004bf0, FUN_140005a20, FUN_140005ef0, FUN_140006210, FUN_140006b10, FUN_140006e30, FUN_140006ef0, FUN_140007e00, FUN_140008ad0, FUN_140008d40, FUN_140008dc0, FUN_140009d50, FUN_14000a4b0, FUN_14000b830, FUN_14000dc30, FUN_14000e0d0 */
/* CALLEES: FUN_140006580, FUN_1400068b0, FUN_140006d70, FUN_140007070, FUN_14000a610, FUN_14000db20, FUN_140016840 */

short FUN_140006fb0(longlong param_1)

{
  int iVar1;
  undefined8 uVar2;
  short local_res8 [4];
  undefined8 local_res10 [3];
  
  if (param_1 != 0) {
    iVar1 = FUN_140006580(param_1,local_res10);
    if (iVar1 != 0) {
      iVar1 = FUN_140016840();
      if (iVar1 == 0) {
        return local_res8[0];
      }
      FUN_14000a610(param_1);
      uVar2 = FUN_140006d70();
      local_res8[0] = FUN_140007070(local_res10[0],uVar2);
      if (local_res8[0] == 0) {
        iVar1 = FUN_140016840(param_1,local_res8,0);
        if (iVar1 != 0) {
          return -1;
        }
        uVar2 = FUN_1400068b0(param_1);
        FUN_14000db20(local_res10[0],uVar2);
      }
      return local_res8[0];
    }
  }
  return -1;
}


/* ================= FUN_140007070 @ 140007070  size=65  callers=3 callees=3 ================= */
/* CALLERS: FUN_140006fb0, FUN_140009a00, FUN_14000a590 */
/* CALLEES: FUN_140006900, FUN_1400070c0, FUN_14000a2f0 */

undefined8 FUN_140007070(longlong param_1,undefined8 param_2,byte param_3)

{
  char cVar1;
  undefined8 uVar2;
  ulonglong uVar3;
  
  uVar3 = (ulonglong)param_3;
  if (param_1 != 0) {
    cVar1 = FUN_14000a2f0();
    if (cVar1 != '\0') {
      uVar2 = FUN_140006900(param_2,uVar3 & 0xff);
      uVar2 = FUN_1400070c0(param_1,uVar2);
      return uVar2;
    }
  }
  return 0xffff;
}


/* ================= FUN_1400070c0 @ 1400070c0  size=27  callers=1 callees=1 ================= */
/* CALLERS: FUN_140007070 */
/* CALLEES: FUN_140006920 */

undefined8 FUN_1400070c0(void)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)FUN_140006920();
  if (puVar1 == (undefined8 *)0x0) {
    return 0;
  }
  return *puVar1;
}


/* ================= FUN_1400070e0 @ 1400070e0  size=302  callers=1 callees=10 ================= */
/* CALLERS: FUN_140002d40 */
/* CALLEES: FUN_140002fb0, FUN_140006580, FUN_1400068b0, FUN_140007210, FUN_140009fd0, FUN_14000db50, FUN_14000e500, FUN_140019ae0, __security_check_cookie, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

longlong FUN_1400070e0(longlong param_1)

{
  longlong lVar1;
  undefined1 uVar2;
  int iVar3;
  undefined8 uVar4;
  longlong lVar5;
  longlong lVar6;
  undefined1 auStack_248 [32];
  undefined8 local_228 [2];
  char local_218 [512];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_248;
  memset(local_218,0,0x200);
  if ((param_1 == 0) || (iVar3 = FUN_140006580(param_1,local_228), iVar3 == 0)) {
    lVar5 = 0;
  }
  else {
    uVar4 = FUN_1400068b0();
    lVar5 = FUN_140007210(local_228[0],uVar4);
    if (lVar5 == 0) {
      uVar2 = FUN_140009fd0(param_1);
      FUN_140002fb0(param_1,local_218,0x200,uVar2);
      lVar1 = -1;
      do {
        lVar6 = lVar1;
        lVar1 = lVar6 + 1;
      } while (local_218[lVar6 + 1] != '\0');
      iVar3 = (int)(lVar6 + 1);
      if (iVar3 < 0x200) {
        lVar5 = FUN_140019ae0((int)lVar6 + 2);
        FUN_14000e500(lVar5,local_218,(int)lVar6 + 2);
        *(undefined1 *)(iVar3 + lVar5) = 0;
        FUN_14000db50(local_228[0],uVar4,lVar5);
      }
    }
  }
  return lVar5;
}


/* ================= FUN_140007210 @ 140007210  size=35  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400070e0 */
/* CALLEES: FUN_140006920 */

undefined * FUN_140007210(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if (lVar1 == 0) {
    return &DAT_14005f306;
  }
  return *(undefined **)(lVar1 + 0x18);
}


/* ================= FUN_140007240 @ 140007240  size=102  callers=1 callees=2 ================= */
/* CALLERS: FUN_140002fb0 */
/* CALLEES: FUN_140004df0, FUN_14000a630 */

undefined4 FUN_140007240(longlong param_1)

{
  char cVar1;
  longlong lVar2;
  undefined4 uVar3;
  undefined1 local_res8;
  
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      lVar2 = FUN_140004df0();
      uVar3 = 0;
      if ((local_res8 != '\0') && (uVar3 = 0, lVar2 != 0)) {
        if (lVar2 == 1) {
          uVar3 = 1;
        }
        else if (lVar2 == 2) {
          return 2;
        }
      }
      return uVar3;
    }
  }
  return 0;
}


/* ================= FUN_1400072b0 @ 1400072b0  size=98  callers=3 callees=3 ================= */
/* CALLERS: FUN_140002d20, FUN_140002d60, FUN_140002e30 */
/* CALLEES: FUN_140007320, FUN_1400073c0, FUN_14000a670 */

undefined8 FUN_1400072b0(code *param_1,undefined8 param_2,ulonglong param_3)

{
  int iVar1;
  undefined8 uVar2;
  undefined8 local_res20;
  undefined8 local_18 [2];
  
  iVar1 = FUN_14000a670(param_2,param_2,param_3,param_2);
  if (iVar1 != 0) {
    iVar1 = FUN_140007320(param_2,local_18,&local_res20);
    if (iVar1 != 0) {
      uVar2 = (*param_1)(local_res20,param_3 & 0xffffffff);
      uVar2 = FUN_1400073c0(local_18[0],uVar2);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_140007320 @ 140007320  size=147  callers=7 callees=6 ================= */
/* CALLERS: FUN_140005560, FUN_1400072b0, FUN_1400088e0, FUN_140008a60, FUN_140008ad0, FUN_140008b90, FUN_140008cb0 */
/* CALLEES: FUN_140003a30, FUN_140005df0, FUN_140007800, FUN_140008a20, FUN_140009e00, FUN_14000a670 */

undefined8 FUN_140007320(undefined8 param_1,longlong *param_2,longlong *param_3)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined1 local_res10 [24];
  
  if ((param_2 != (longlong *)0x0) && (param_3 != (longlong *)0x0)) {
    iVar2 = FUN_14000a670();
    if (iVar2 != 0) {
      cVar1 = FUN_140009e00(param_1);
      if (cVar1 != '\0') {
        iVar2 = FUN_140007800(param_1,param_2);
        if (iVar2 != 0) {
          iVar2 = FUN_140008a20(param_1,local_res10);
          if (iVar2 != 0) {
            cVar1 = FUN_140003a30(*param_2);
            if (cVar1 != '\0') {
              lVar3 = FUN_140005df0();
              *param_3 = lVar3;
              if ((*param_2 != 0) && (lVar3 != 0)) {
                return 1;
              }
            }
          }
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_1400073c0 @ 1400073c0  size=93  callers=2 callees=3 ================= */
/* CALLERS: FUN_1400072b0, FUN_140007420 */
/* CALLEES: FUN_1400068b0, FUN_1400078f0, FUN_14000a630 */

ulonglong FUN_1400073c0(undefined8 param_1,longlong param_2)

{
  char cVar1;
  uint uVar2;
  ulonglong uVar3;
  
  uVar2 = FUN_1400078f0();
  if ((param_2 != 0) && (uVar2 != 0xffffffff)) {
    cVar1 = FUN_14000a630(param_2);
    if (cVar1 != '\0') {
      uVar3 = FUN_1400068b0();
      return uVar3 & 0x1ffffffffffff | (ulonglong)(uVar2 & 3) << 0x31 | 0xb230000000000000;
    }
  }
  return 0;
}


/* ================= FUN_140007420 @ 140007420  size=46  callers=1 callees=1 ================= */
/* CALLERS: FUN_140002920 */
/* CALLEES: FUN_1400073c0 */

undefined8 FUN_140007420(longlong param_1,code *param_2,undefined4 param_3)

{
  undefined8 uVar1;
  
  if (param_1 == 0) {
    return 0;
  }
  uVar1 = (*param_2)(param_1,param_3);
  uVar1 = FUN_1400073c0(param_1,uVar1);
  return uVar1;
}


/* ================= FUN_140007450 @ 140007450  size=115  callers=6 callees=5 ================= */
/* CALLERS: FUN_1400088e0, FUN_140008a60, FUN_140008ad0, FUN_140008b40, FUN_140008b90, FUN_140008cb0 */
/* CALLEES: FUN_140003a30, FUN_140007670, FUN_140007800, FUN_140008a20, FUN_140009e30 */

bool FUN_140007450(undefined8 param_1,undefined8 *param_2,uint *param_3)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint local_res18 [4];
  
  if ((param_3 != (uint *)0x0) && (param_2 != (undefined8 *)0x0)) {
    cVar1 = FUN_140009e30();
    if (cVar1 != '\0') {
      iVar2 = FUN_140007800(param_1,param_2);
      if (iVar2 != 0) {
        iVar2 = FUN_140008a20(param_1,local_res18);
        if (iVar2 != 0) {
          cVar1 = FUN_140003a30(*param_2);
          if (cVar1 != '\0') {
            uVar3 = FUN_140007670();
            *param_3 = local_res18[0];
            return local_res18[0] < uVar3;
          }
        }
      }
    }
  }
  return false;
}


/* ================= FUN_1400074d0 @ 1400074d0  size=258  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400088e0 */
/* CALLEES: FUN_140007780, FUN_140008d20, FUN_140009d40, FUN_140009fc0 */

undefined8 * FUN_1400074d0(undefined8 *param_1,undefined8 param_2,undefined4 param_3)

{
  char cVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  
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
  uVar2 = FUN_140007780(param_2,param_3);
  cVar1 = FUN_140009fc0(uVar2);
  if (cVar1 != '\0') {
    *(undefined4 *)param_1 = 1;
    *(undefined4 *)(param_1 + 1) = 1;
    uVar3 = FUN_140008d20(param_2,param_3);
    param_1[6] = uVar3;
    return param_1;
  }
  cVar1 = FUN_140009d40();
  if (cVar1 != '\0') {
    *(undefined4 *)param_1 = 1;
    *(undefined4 *)(param_1 + 1) = 2;
    uVar3 = FUN_140008d20(param_2,param_3);
    param_1[2] = uVar3;
  }
  return param_1;
}


/* ================= FUN_1400075e0 @ 1400075e0  size=68  callers=1 callees=2 ================= */
/* CALLERS: FUN_140002f00 */
/* CALLEES: FUN_140007670, FUN_1400078f0 */

ulonglong FUN_1400075e0(undefined8 param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  ulonglong uVar3;
  
  uVar3 = (ulonglong)param_2;
  uVar1 = FUN_1400078f0();
  if (uVar1 != 0xffffffff) {
    uVar2 = FUN_140007670();
    if ((uint)uVar3 < uVar2) {
      return (ulonglong)(uVar1 & 3) << 0x31 | uVar3 | 0xb238000000000000;
    }
  }
  return 0;
}


/* ================= FUN_140007630 @ 140007630  size=53  callers=13 callees=2 ================= */
/* CALLERS: FUN_140003d10, FUN_140003e50, FUN_140007690, FUN_1400076d0, FUN_140007750, FUN_140007780, FUN_140007ea0, FUN_140007f40, FUN_140007ff0, FUN_140008c90, FUN_140008d20, FUN_140009400, FUN_1400094f0 */
/* CALLEES: FUN_140007670, FUN_14000a2f0 */

longlong FUN_140007630(longlong param_1,uint param_2)

{
  char cVar1;
  uint uVar2;
  ulonglong uVar3;
  
  uVar3 = (ulonglong)param_2;
  cVar1 = FUN_14000a2f0();
  if (cVar1 != '\0') {
    uVar2 = FUN_140007670();
    if ((uint)uVar3 < uVar2) {
      return *(longlong *)(param_1 + 0x58) + uVar3 * 0x18;
    }
  }
  return 0;
}


/* ================= FUN_140007670 @ 140007670  size=28  callers=4 callees=1 ================= */
/* CALLERS: FUN_140002f30, FUN_140007450, FUN_1400075e0, FUN_140007630 */
/* CALLEES: FUN_14000a2f0 */

undefined4 FUN_140007670(longlong param_1)

{
  char cVar1;
  
  cVar1 = FUN_14000a2f0();
  if (cVar1 != '\0') {
    return *(undefined4 *)(param_1 + 4);
  }
  return 0;
}


/* ================= FUN_140007690 @ 140007690  size=53  callers=2 callees=2 ================= */
/* CALLERS: FUN_140008a60, FUN_14000db80 */
/* CALLEES: FUN_140007630, FUN_140008820 */

undefined8 FUN_140007690(longlong param_1)

{
  undefined4 *puVar1;
  undefined8 uVar2;
  
  if (param_1 != 0) {
    puVar1 = (undefined4 *)FUN_140007630();
    if ((puVar1 != (undefined4 *)0x0) && (*(int *)(param_1 + 0x14) != -1)) {
      uVar2 = FUN_140008820(param_1,*(int *)(param_1 + 0x14),*puVar1);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_1400076d0 @ 1400076d0  size=120  callers=3 callees=3 ================= */
/* CALLERS: FUN_140003d10, FUN_140003e50, FUN_140007ea0 */
/* CALLEES: FUN_140007630, FUN_140008460, FUN_140008620 */

uint FUN_1400076d0(longlong param_1,int param_2)

{
  uint uVar1;
  longlong lVar2;
  ulonglong uVar3;
  
  lVar2 = FUN_140007630();
  if ((lVar2 == 0) || (uVar1 = (uint)*(ushort *)(lVar2 + 6), *(ushort *)(lVar2 + 6) == 0xffff)) {
    lVar2 = FUN_140008460(param_1,*(undefined4 *)(param_1 + 0x1c));
    uVar3 = FUN_140008620(param_1,*(undefined4 *)(param_1 + 0x1c));
    if ((lVar2 == 0) ||
       ((uVar3 < param_2 * 4 + 4 ||
        (uVar1 = *(uint *)((ulonglong)(uint)(param_2 * 4) + lVar2), uVar1 == 0xffff)))) {
      uVar1 = 0xffffffff;
    }
  }
  return uVar1;
}


/* ================= FUN_140007750 @ 140007750  size=45  callers=3 callees=1 ================= */
/* CALLERS: FUN_140008b90, FUN_140008cb0, FUN_14000db80 */
/* CALLEES: FUN_140007630 */

ulonglong FUN_140007750(void)

{
  longlong lVar1;
  ulonglong uVar2;
  
  lVar1 = FUN_140007630();
  if (lVar1 == 0) {
    return 0;
  }
  uVar2 = ~*(ulonglong *)(lVar1 + 8);
  if (*(ulonglong *)(lVar1 + 0x10) <= ~*(ulonglong *)(lVar1 + 8)) {
    uVar2 = *(ulonglong *)(lVar1 + 0x10);
  }
  return uVar2;
}


/* ================= FUN_140007780 @ 140007780  size=48  callers=8 callees=2 ================= */
/* CALLERS: FUN_140003d10, FUN_140003e50, FUN_1400074d0, FUN_140007f40, FUN_1400088e0, FUN_140008ad0, FUN_140009400, FUN_14000db80 */
/* CALLEES: FUN_140007630, FUN_140008c90 */

uint FUN_140007780(undefined8 param_1,ulonglong param_2)

{
  uint uVar1;
  longlong lVar2;
  
  param_2 = param_2 & 0xffffffff;
  lVar2 = FUN_140007630();
  if (lVar2 != 0) {
    uVar1 = FUN_140008c90(param_1,param_2 & 0xffffffff);
    return uVar1 & 0xf;
  }
  return 0;
}


/* ================= FUN_1400077b0 @ 1400077b0  size=69  callers=7 callees=1 ================= */
/* CALLERS: FUN_140003bc0, FUN_140003d10, FUN_140003e50, FUN_140007ea0, FUN_140007f40, FUN_140007ff0, FUN_140008d20 */
/* CALLEES: FUN_140009f90 */

ulonglong FUN_1400077b0(longlong param_1)

{
  char cVar1;
  ulonglong uVar2;
  
  if (param_1 != 0) {
    uVar2 = *(ulonglong *)(param_1 + 8);
    cVar1 = FUN_140009f90(*(byte *)(param_1 + 4) & 0xf);
    if ((cVar1 != '\0') && (uVar2 != 0xffffffffffffffff)) {
      uVar2 = uVar2 - uVar2 % (ulonglong)DAT_140096008;
    }
    return uVar2;
  }
  return 0xffffffffffffffff;
}


/* ================= FUN_140007800 @ 140007800  size=82  callers=2 callees=1 ================= */
/* CALLERS: FUN_140007320, FUN_140007450 */
/* CALLEES: FUN_14000a670 */

ulonglong FUN_140007800(ulonglong param_1,longlong *param_2)

{
  longlong lVar1;
  int iVar2;
  undefined8 uVar3;
  
  if (param_2 != (longlong *)0x0) {
    uVar3 = 0;
    *param_2 = 0;
    iVar2 = FUN_14000a670();
    if ((iVar2 != 0) && (((byte)(param_1 >> 0x31) & 3) < 2)) {
      lVar1 = (&DAT_140097c00)[(uint)(param_1 >> 0x31) & 3];
      *param_2 = lVar1;
      return CONCAT71((int7)((ulonglong)uVar3 >> 8),lVar1 != 0) & 0xffffffff;
    }
  }
  return 0;
}


/* ================= FUN_140007860 @ 140007860  size=28  callers=7 callees=1 ================= */
/* CALLERS: FUN_140002900, FUN_140008400, FUN_1400085c0, FUN_14000bf80, FUN_14000ca30, FUN_14000cea0, FUN_14000dfc0 */
/* CALLEES: FUN_1400078a0 */

undefined1 FUN_140007860(void)

{
  longlong lVar1;
  
  lVar1 = FUN_1400078a0();
  if (lVar1 != 0) {
    return *(undefined1 *)(lVar1 + 4);
  }
  return 0;
}


/* ================= FUN_140007880 @ 140007880  size=28  callers=2 callees=1 ================= */
/* CALLERS: FUN_140008460, FUN_140008690 */
/* CALLEES: FUN_1400078a0 */

undefined8 FUN_140007880(longlong param_1)

{
  longlong lVar1;
  
  lVar1 = FUN_1400078a0();
  if (lVar1 != 0) {
    return *(undefined8 *)(param_1 + 0x38);
  }
  return 0;
}


/* ================= FUN_1400078a0 @ 1400078a0  size=41  callers=11 callees=0 ================= */
/* CALLERS: FUN_140005310, FUN_140007860, FUN_140007880, FUN_1400078d0, FUN_140007950, FUN_140007980, FUN_140008400, FUN_1400085c0, FUN_140008670, FUN_14000ca30, FUN_14000cea0 */

undefined8 FUN_1400078a0(longlong param_1)

{
  longlong *plVar1;
  
  if (param_1 != 0) {
    plVar1 = &DAT_140097c00;
    do {
      if (*plVar1 == param_1) {
        return *(undefined8 *)(param_1 + 0x40);
      }
      plVar1 = plVar1 + 1;
    } while ((longlong)plVar1 < 0x140097c10);
  }
  return 0;
}


/* ================= FUN_1400078d0 @ 1400078d0  size=28  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000dfc0 */
/* CALLEES: FUN_1400078a0 */

undefined1 FUN_1400078d0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_1400078a0();
  if (lVar1 != 0) {
    return *(undefined1 *)(lVar1 + 6);
  }
  return 0;
}


/* ================= FUN_1400078f0 @ 1400078f0  size=41  callers=6 callees=0 ================= */
/* CALLERS: FUN_140003a30, FUN_140006920, FUN_1400073c0, FUN_1400075e0, FUN_14000a2f0, FUN_14000a870 */

int FUN_1400078f0(longlong param_1)

{
  longlong *plVar1;
  int iVar2;
  
  iVar2 = 0;
  plVar1 = &DAT_140097c00;
  do {
    if (*plVar1 == param_1) {
      return iVar2;
    }
    iVar2 = iVar2 + 1;
    plVar1 = plVar1 + 1;
  } while ((longlong)plVar1 < 0x140097c10);
  return -1;
}


/* ================= FUN_140007920 @ 140007920  size=34  callers=1 callees=1 ================= */
/* CALLERS: FUN_140008690 */
/* CALLEES: FUN_14000a2f0 */

undefined8 FUN_140007920(longlong param_1)

{
  char cVar1;
  
  if (param_1 != 0) {
    cVar1 = FUN_14000a2f0();
    if (cVar1 != '\0') {
      return *(undefined8 *)(param_1 + 0x28);
    }
  }
  return 0;
}


/* ================= FUN_140007950 @ 140007950  size=33  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000dfc0 */
/* CALLEES: FUN_1400078a0 */

ulonglong FUN_140007950(void)

{
  longlong lVar1;
  
  lVar1 = FUN_1400078a0();
  if (lVar1 != 0) {
    return (ulonglong)*(ushort *)(lVar1 + 0x10);
  }
  return 0x10000;
}


/* ================= FUN_140007980 @ 140007980  size=27  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000dfc0 */
/* CALLEES: FUN_1400078a0 */

undefined4 FUN_140007980(void)

{
  longlong lVar1;
  
  lVar1 = FUN_1400078a0();
  if (lVar1 != 0) {
    return *(undefined4 *)(lVar1 + 0x14);
  }
  return 0;
}


/* ================= FUN_1400079a0 @ 1400079a0  size=51  callers=8 callees=2 ================= */
/* CALLERS: FUN_140007e00, FUN_140009620, FUN_1400096a0, FUN_140009850, FUN_1400098d0, FUN_140009a00, FUN_140009c40, FUN_14000a4b0 */
/* CALLEES: FUN_14000a630, FUN_1400153e0 */

undefined4 FUN_1400079a0(longlong param_1)

{
  char cVar1;
  int iVar2;
  undefined4 local_res8;
  
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      iVar2 = FUN_1400153e0();
      if (iVar2 == 0) {
        return local_res8;
      }
    }
  }
  return 0;
}


/* ================= FUN_1400079e0 @ 1400079e0  size=13  callers=2 callees=1 ================= */
/* CALLERS: FUN_14000b630, FUN_14000c7e0 */
/* CALLEES: FUN_140006730 */

void FUN_1400079e0(undefined8 param_1,undefined4 param_2)

{
  FUN_140006730(param_1,0x2e,param_2);
  return;
}


/* ================= FUN_1400079f0 @ 1400079f0  size=10  callers=2 callees=1 ================= */
/* CALLERS: FUN_14000b630, FUN_14000c7e0 */
/* CALLEES: FUN_140008f40 */

void FUN_1400079f0(undefined8 param_1)

{
  FUN_140008f40(param_1,0x2e);
  return;
}


/* ================= FUN_140007a10 @ 140007a10  size=10  callers=1 callees=1 ================= */
/* CALLERS: FUN_140002950 */
/* CALLEES: FUN_140008f40 */

void FUN_140007a10(undefined8 param_1)

{
  FUN_140008f40(param_1,0x34);
  return;
}


/* ================= FUN_140007a20 @ 140007a20  size=50  callers=1 callees=0 ================= */
/* CALLERS: FUN_140007e00 */

int FUN_140007a20(char *param_1)

{
  char cVar1;
  int iVar2;
  
  iVar2 = 0x1505;
  if (param_1 == (char *)0x0) {
    return 0;
  }
  cVar1 = *param_1;
  while (cVar1 != '\0') {
    iVar2 = iVar2 * 0x21 + (int)cVar1;
    cVar1 = *param_1;
    param_1 = param_1 + 1;
  }
  return iVar2;
}


/* ================= FUN_140007a60 @ 140007a60  size=55  callers=0 callees=3 ================= */
/* CALLEES: FUN_140006580, FUN_1400068b0, FUN_140007b10 */

undefined8 FUN_140007a60(longlong param_1,undefined4 param_2)

{
  int iVar1;
  undefined8 uVar2;
  undefined8 local_res8 [4];
  
  if (param_1 != 0) {
    iVar1 = FUN_140006580(param_1,local_res8,param_2);
    if (iVar1 != 0) {
      uVar2 = FUN_1400068b0();
      uVar2 = FUN_140007b10(local_res8[0],uVar2);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_140007aa0 @ 140007aa0  size=52  callers=1 callees=3 ================= */
/* CALLERS: FUN_140002d80 */
/* CALLEES: FUN_140006580, FUN_1400068b0, FUN_140007ae0 */

undefined8 FUN_140007aa0(longlong param_1)

{
  int iVar1;
  undefined8 uVar2;
  undefined8 local_res8 [4];
  
  if (param_1 != 0) {
    iVar1 = FUN_140006580(param_1,local_res8);
    if (iVar1 != 0) {
      uVar2 = FUN_1400068b0();
      uVar2 = FUN_140007ae0(local_res8[0],uVar2);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_140007ae0 @ 140007ae0  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_140007aa0 */
/* CALLEES: FUN_140006920 */

undefined4 FUN_140007ae0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if ((lVar1 != 0) && (*(longlong *)(lVar1 + 0x30) != 0)) {
    return *(undefined4 *)(*(longlong *)(lVar1 + 0x30) + 0x10);
  }
  return 0;
}


/* ================= FUN_140007b10 @ 140007b10  size=76  callers=1 callees=3 ================= */
/* CALLERS: FUN_140007a60 */
/* CALLEES: FUN_140002820, FUN_140005df0, FUN_140006920 */

undefined8 FUN_140007b10(undefined8 param_1,undefined8 param_2,undefined4 param_3)

{
  longlong lVar1;
  undefined8 uVar2;
  
  lVar1 = FUN_140006920();
  if ((lVar1 != 0) && (*(longlong *)(lVar1 + 0x30) != 0)) {
    uVar2 = FUN_140002820(*(longlong *)(lVar1 + 0x30),param_3);
    uVar2 = FUN_140005df0(param_1,uVar2);
    return uVar2;
  }
  return 0;
}


/* ================= FUN_140007b60 @ 140007b60  size=13  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000b630 */
/* CALLEES: FUN_140006730 */

void FUN_140007b60(undefined8 param_1,undefined4 param_2)

{
  FUN_140006730(param_1,0x1d,param_2);
  return;
}


/* ================= FUN_140007b70 @ 140007b70  size=10  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000b630 */
/* CALLEES: FUN_140008f40 */

void FUN_140007b70(undefined8 param_1)

{
  FUN_140008f40(param_1,0x1d);
  return;
}


/* ================= FUN_140007b80 @ 140007b80  size=50  callers=1 callees=0 ================= */
/* CALLERS: FUN_14000e0d0 */

undefined8 FUN_140007b80(int param_1,longlong param_2,longlong param_3)

{
  ulonglong uVar1;
  longlong lVar2;
  
  lVar2 = (longlong)(param_1 + -1);
  while( true ) {
    if (lVar2 < 0) {
      return 0;
    }
    uVar1 = *(ulonglong *)(param_3 + lVar2 * 8);
    if ((uVar1 < 0x2f) && ((0x400020200000U >> (uVar1 & 0x3f) & 1) != 0)) break;
    lVar2 = lVar2 + -1;
  }
  return *(undefined8 *)(param_2 + lVar2 * 8);
}


/* ================= FUN_140007bd0 @ 140007bd0  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_140006a40 */
/* CALLEES: FUN_140006920 */

undefined4 FUN_140007bd0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140006920();
  if ((lVar1 != 0) && (*(longlong *)(lVar1 + 0x28) != 0)) {
    return *(undefined4 *)(*(longlong *)(lVar1 + 0x28) + 0x10);
  }
  return 0;
}


/* ================= FUN_140007c00 @ 140007c00  size=76  callers=1 callees=3 ================= */
/* CALLERS: FUN_140006a00 */
/* CALLEES: FUN_140002820, FUN_140005df0, FUN_140006920 */

undefined8 FUN_140007c00(undefined8 param_1,undefined8 param_2,undefined4 param_3)

{
  longlong lVar1;
  undefined8 uVar2;
  
  lVar1 = FUN_140006920();
  if ((lVar1 != 0) && (*(longlong *)(lVar1 + 0x28) != 0)) {
    uVar2 = FUN_140002820(*(longlong *)(lVar1 + 0x28),param_3);
    uVar2 = FUN_140005df0(param_1,uVar2);
    return uVar2;
  }
  return 0;
}


/* ================= FUN_140007c50 @ 140007c50  size=298  callers=2 callees=0 ================= */
/* CALLERS: FUN_14000a770, FUN_14000a7e0 */

char * FUN_140007c50(undefined4 param_1)

{
  switch(param_1) {
  case 0x2b:
    return "Invalid ELF file handle";
  case 0x2c:
    return "Recursion limit exceeded in ELF parser";
  case 0x2d:
    return "Symbol constant value data exceeds allocation size limit";
  case 0x2e:
    return "DW_OP_implicit_value with data size greater than 8 bytes is not supported";
  case 0x2f:
    return "DW_OP_entry_value opcode encountered that does not specify an entry location";
  case 0x30:
    return "symbol location description contains more parts than can be stored";
  case 0x31:
    return "byte-unaligned symbol data pieces not yet supported";
  case 0x32:
    return "failed to parse location description element";
  case 0x33:
    return "could not parse debug info for type unit";
  case 0x34:
    return "could not parse debug info for compilation unit";
  case 0x35:
    return 
    "compilation unit with DWARF5 debug information detected - DWARF5 is not yet fully supported";
  case 0x36:
    return "could not parse source file info for compilation unit";
  case 0x37:
    return "could not parse source line info for compilation unit";
  case 0x38:
    return "could not parse macro information";
  case 0x39:
    return "could not parse call frame info for compilation unit";
  case 0x3a:
    return "could not initialize symbol name index";
  case 0x3b:
    return "evaluation of opcode \'DW_EXPR_EXPRESSION\' not yet implemented";
  case 0x3c:
    return "evaluation of opcode \'DW_EXPR_VAL_EXPRESSION\' not yet implemented";
  case 0x3d:
    return "unknown cfi register table value type";
  case 0x3e:
    return 
    "a debug information entry (DIE) at a specific data section offset could not be parsed/generated"
    ;
  case 0x3f:
    return "file not tagged with ELF magic number";
  case 0x40:
    return "not an executable ELF file";
  case 0x41:
    return "file class is ELF_CLASS_NONE";
  case 0x42:
    return "data encoding is ELF_DATA_NONE";
  case 0x43:
    return "header version number is not EV_CURRENT";
  case 0x44:
    return "unsupported file version number";
  case 0x45:
    return "a maximum number of ELF files are already open";
  case 0x46:
    return "file could not be opened for reading";
  case 0x47:
    return "unexpected file size";
  case 0x48:
    return "parser is out of memory";
  case 0x49:
    return "error reading file data";
  case 0x4a:
    return "failed to initialize libdwarf library";
  default:
    return "";
  }
}


/* ================= FUN_140007e00 @ 140007e00  size=131  callers=3 callees=3 ================= */
/* CALLERS: FUN_140004aa0, FUN_140004af0, FUN_14000e0d0 */
/* CALLEES: FUN_140006fb0, FUN_1400079a0, FUN_140007a20 */

longlong FUN_140007e00(longlong param_1,char param_2)

{
  uint uVar1;
  longlong lVar2;
  
  if ((param_1 != 0) && (lVar2 = *(longlong *)(param_1 + 0x58), lVar2 != 0)) {
    if (param_2 < '\0') {
      param_2 = FUN_1400079a0(param_1,0x3c);
      lVar2 = *(longlong *)(param_1 + 0x58);
    }
    uVar1 = FUN_140007a20(lVar2);
    lVar2 = FUN_140006fb0(param_1);
    return (lVar2 + ((ulonglong)uVar1 + 0x2c9) * 0x1f) * 0x1f + (ulonglong)(param_2 != '\0');
  }
  return 0;
}


/* ================= FUN_140007e90 @ 140007e90  size=13  callers=1 callees=1 ================= */
/* CALLERS: FUN_140004240 */
/* CALLEES: FUN_140006730 */

void FUN_140007e90(undefined8 param_1,undefined4 param_2)

{
  FUN_140006730(param_1,0x1e,param_2);
  return;
}


/* ================= FUN_140007ea0 @ 140007ea0  size=155  callers=1 callees=4 ================= */
/* CALLERS: FUN_140003e50 */
/* CALLEES: FUN_140007630, FUN_1400076d0, FUN_1400077b0, FUN_140008ef0 */

undefined8 FUN_140007ea0(longlong param_1,ulonglong param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  longlong lVar5;
  ulonglong uVar6;
  undefined8 uVar7;
  
  uVar1 = *(uint *)(param_1 + 4);
  uVar2 = param_3;
  do {
    uVar2 = uVar2 + 1;
    if (uVar1 <= uVar2) {
      return 0xffffffffffffffff;
    }
    uVar3 = FUN_140008ef0(param_1,uVar2);
    lVar5 = FUN_140007630(param_1,uVar3);
  } while (((lVar5 == 0) || (iVar4 = FUN_1400076d0(param_1,param_3), iVar4 != 0xfff1)) ||
          (uVar6 = FUN_1400077b0(lVar5), uVar6 <= param_2));
  uVar7 = FUN_1400077b0(lVar5);
  return uVar7;
}


/* ================= FUN_140007f40 @ 140007f40  size=162  callers=1 callees=5 ================= */
/* CALLERS: FUN_140003e50 */
/* CALLEES: FUN_140007630, FUN_140007780, FUN_1400077b0, FUN_140008ef0, FUN_140009f90 */

undefined8 FUN_140007f40(longlong param_1,ulonglong param_2,uint param_3)

{
  uint uVar1;
  char cVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  longlong lVar5;
  ulonglong uVar6;
  undefined8 uVar7;
  
  uVar1 = *(uint *)(param_1 + 4);
  do {
    param_3 = param_3 + 1;
    if (uVar1 <= param_3) {
      return 0xffffffffffffffff;
    }
    uVar3 = FUN_140008ef0(param_1,param_3);
    uVar4 = FUN_140007780(param_1,uVar3);
    cVar2 = FUN_140009f90(uVar4);
  } while (((cVar2 == '\0') || (lVar5 = FUN_140007630(param_1,uVar3), lVar5 == 0)) ||
          (uVar6 = FUN_1400077b0(lVar5), uVar6 <= param_2));
  uVar7 = FUN_1400077b0(lVar5);
  return uVar7;
}


/* ================= FUN_140007ff0 @ 140007ff0  size=157  callers=1 callees=3 ================= */
/* CALLERS: FUN_140003d10 */
/* CALLEES: FUN_140007630, FUN_1400077b0, FUN_140008ef0 */

ulonglong FUN_140007ff0(undefined8 param_1,uint param_2,ulonglong param_3,longlong param_4,
                       longlong param_5)

{
  uint uVar1;
  undefined4 uVar2;
  longlong lVar3;
  ulonglong uVar4;
  
  uVar1 = param_2 + 10;
  do {
    param_2 = param_2 + 1;
    if (uVar1 <= param_2) goto LAB_14000805f;
    uVar2 = FUN_140008ef0(param_1,param_2);
    lVar3 = FUN_140007630(param_1,uVar2);
  } while ((lVar3 == 0) || (uVar4 = FUN_1400077b0(lVar3), uVar4 <= param_3));
  param_3 = FUN_1400077b0(lVar3);
LAB_14000805f:
  if ((param_5 != 0) && ((ulonglong)(param_4 + param_5) < param_3)) {
    param_3 = param_4 + param_5;
  }
  return param_3;
}


/* ================= FUN_140008090 @ 140008090  size=90  callers=3 callees=4 ================= */
/* CALLERS: FUN_140005ab0, FUN_140005be0, FUN_140006210 */
/* CALLEES: FUN_140005960, FUN_140006580, FUN_140006d30, FUN_14000a610 */

undefined8 FUN_140008090(longlong param_1)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 local_res8 [4];
  
  if (param_1 != 0) {
    iVar2 = FUN_140006580(param_1,local_res8);
    if (iVar2 != 0) {
      cVar1 = FUN_14000a610();
      uVar3 = FUN_140006d30(local_res8[0],*(undefined8 *)(param_1 + 0x20),cVar1 == '\0');
      uVar3 = FUN_140005960(local_res8[0],uVar3,cVar1 == '\0');
      return uVar3;
    }
  }
  return 0;
}


/* ================= FUN_1400080f0 @ 1400080f0  size=408  callers=6 callees=14 ================= */
/* CALLERS: FUN_140002fb0, FUN_140004240, FUN_140005380, FUN_140006c00, FUN_140006ef0, FUN_140008d40 */
/* CALLEES: FUN_140004240, FUN_1400045e0, FUN_140004600, FUN_140004df0, FUN_140005960, FUN_140006580, FUN_140006d70, FUN_140009330, FUN_14000a610, FUN_14000d9a0, FUN_1400161e0, FUN_140016600, FUN_1400184c0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

longlong FUN_1400080f0(longlong param_1)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  longlong lVar4;
  bool bVar5;
  undefined1 auStack_78 [32];
  undefined8 local_58;
  char local_48;
  longlong local_40;
  undefined8 local_38;
  undefined8 local_30;
  undefined8 local_28;
  ulonglong local_20;
  
  local_20 = DAT_140096fe8 ^ (ulonglong)auStack_78;
  if (param_1 == 0) {
    return 0;
  }
  iVar2 = FUN_140006580(param_1,&local_30);
  if (iVar2 == 0) {
    return 0;
  }
  local_40 = 0;
  local_38 = 0;
  uVar3 = FUN_140004240();
  if (local_48 == '\0') {
    return 0;
  }
  if ((*(ushort *)(*(longlong *)(param_1 + 0x50) + 0x12) < 5) &&
     (cVar1 = FUN_140009330(uVar3), cVar1 == '\x01')) {
    iVar2 = FUN_1400184c0();
    if ((iVar2 == 0) &&
       (iVar2 = FUN_14000d9a0(*(undefined8 *)(param_1 + 0x48),local_28), iVar2 == 0)) {
      local_58 = 0;
      iVar2 = FUN_140016600(*(undefined8 *)(param_1 + 0x48),0,&local_38,0);
      if (iVar2 == 0) {
        iVar2 = FUN_1400161e0(local_38,&local_40,0);
        if (iVar2 != 0) {
          local_40 = 0;
        }
      }
      else {
        local_38 = 0;
      }
    }
    FUN_140004600(local_38);
    local_38 = 0;
    if (local_40 == 0) goto LAB_140008246;
    lVar4 = FUN_140006d70();
    FUN_140004600();
    local_40 = 0;
    if (lVar4 == 0) goto LAB_140008246;
    bVar5 = false;
  }
  else {
    cVar1 = FUN_14000a610(param_1);
    bVar5 = cVar1 == '\0';
    lVar4 = FUN_140004df0();
    if ((local_48 == '\0') || (lVar4 == 0)) goto LAB_140008246;
  }
  local_40 = FUN_140005960(local_30,lVar4,bVar5);
LAB_140008246:
  FUN_1400045e0(*(undefined8 *)(param_1 + 0x48),uVar3);
  if (local_40 == 0) {
    return 0;
  }
  if (*(longlong *)(local_40 + 0x20) == *(longlong *)(param_1 + 0x20)) {
    return 0;
  }
  return local_40;
}


/* ================= FUN_140008290 @ 140008290  size=223  callers=3 callees=0 ================= */
/* CALLERS: FUN_14000aa30, FUN_14000aaf0, FUN_14000ca30 */

char * FUN_140008290(undefined4 param_1)

{
  switch(param_1) {
  case 0:
    return ".debug_info";
  case 1:
    return ".debug_types";
  case 2:
    return ".debug_loc";
  case 3:
    return ".debug_str";
  case 4:
    return ".debug_abbrev";
  case 5:
    return ".debug_frame";
  case 6:
    return ".debug_line";
  case 7:
    return ".debug_macinfo";
  case 8:
    return ".debug_macro";
  case 9:
    return ".debug_ranges";
  case 10:
    return ".debug_aranges";
  case 0xb:
    return ".eh_frame";
  case 0xc:
    return ".symtab";
  case 0xd:
    return ".strtab";
  case 0xe:
    return ".symtab_shndx";
  case 0xf:
    return ".note";
  case 0x10:
    return ".note.iar";
  case 0x11:
    return ".debug_loclists";
  case 0x12:
    return ".debug_rnglists";
  case 0x13:
    return ".debug_line_str";
  case 0x14:
    return ".debug_names";
  case 0x15:
    return ".debug_addr";
  case 0x16:
    return ".debug_str_offsets";
  default:
    return "";
  }
}


/* ================= FUN_1400083d0 @ 1400083d0  size=35  callers=3 callees=1 ================= */
/* CALLERS: FUN_140002970, FUN_140003d10, FUN_140003e50 */
/* CALLEES: FUN_140008520 */

undefined8 FUN_1400083d0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008520();
  if (lVar1 != 0) {
    return *(undefined8 *)(lVar1 + 0x10);
  }
  return 0xffffffffffffffff;
}


/* ================= FUN_140008400 @ 140008400  size=85  callers=5 callees=2 ================= */
/* CALLERS: FUN_1400029a0, FUN_140004c50, FUN_140008520, FUN_14000bf80, FUN_14000ca30 */
/* CALLEES: FUN_140007860, FUN_1400078a0 */

uint FUN_140008400(longlong param_1)

{
  int iVar1;
  longlong lVar2;
  
  lVar2 = FUN_1400078a0();
  if (lVar2 != 0) {
    if (*(ushort *)(lVar2 + 0x3c) != 0) {
      return (uint)*(ushort *)(lVar2 + 0x3c);
    }
    iVar1 = FUN_140007860();
    if (iVar1 == 2) {
      return *(uint *)(*(longlong *)(lVar2 + 0x28) + 0x20 + *(longlong *)(param_1 + 0x38));
    }
    if (iVar1 == 1) {
      return *(uint *)(*(longlong *)(lVar2 + 0x28) + 0x14 + *(longlong *)(param_1 + 0x38));
    }
  }
  return 0;
}


/* ================= FUN_140008460 @ 140008460  size=92  callers=4 callees=3 ================= */
/* CALLERS: FUN_1400029c0, FUN_1400076d0, FUN_140008820, FUN_14000bf80 */
/* CALLEES: FUN_140007880, FUN_1400084e0, FUN_140008520 */

longlong FUN_140008460(undefined8 param_1,undefined4 param_2)

{
  longlong lVar1;
  longlong lVar2;
  
  lVar1 = FUN_140007880();
  if (lVar1 != 0) {
    lVar2 = FUN_140008520();
    if (lVar2 != 0) {
      lVar2 = FUN_1400084e0(param_1,param_2);
      return lVar2 + lVar1;
    }
  }
  return 0;
}


/* ================= FUN_1400084c0 @ 1400084c0  size=28  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000bf80 */
/* CALLEES: FUN_140008520 */

undefined8 FUN_1400084c0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008520();
  if (lVar1 != 0) {
    return *(undefined8 *)(lVar1 + 0x38);
  }
  return 0;
}


/* ================= FUN_1400084e0 @ 1400084e0  size=28  callers=1 callees=1 ================= */
/* CALLERS: FUN_140008460 */
/* CALLEES: FUN_140008520 */

undefined8 FUN_1400084e0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008520();
  if (lVar1 != 0) {
    return *(undefined8 *)(lVar1 + 0x18);
  }
  return 0;
}


/* ================= FUN_140008500 @ 140008500  size=28  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400029f0 */
/* CALLEES: FUN_140008520 */

undefined8 FUN_140008500(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008520();
  if (lVar1 != 0) {
    return *(undefined8 *)(lVar1 + 8);
  }
  return 0;
}


/* ================= FUN_140008520 @ 140008520  size=93  callers=8 callees=1 ================= */
/* CALLERS: FUN_1400083d0, FUN_140008460, FUN_1400084c0, FUN_1400084e0, FUN_140008500, FUN_140008580, FUN_140008620, FUN_140008650 */
/* CALLEES: FUN_140008400 */

longlong FUN_140008520(longlong param_1,uint param_2)

{
  uint uVar1;
  longlong *plVar2;
  
  plVar2 = &DAT_140097c00;
  do {
    if (*plVar2 == param_1) {
      uVar1 = FUN_140008400(param_1);
      if (param_2 < uVar1) {
        return (ulonglong)param_2 * 0x40 + *(longlong *)(param_1 + 0x48);
      }
    }
    plVar2 = plVar2 + 1;
  } while ((longlong)plVar2 < 0x140097c10);
  return 0;
}


/* ================= FUN_140008580 @ 140008580  size=61  callers=2 callees=3 ================= */
/* CALLERS: FUN_140002a20, FUN_140004c50 */
/* CALLEES: FUN_140008520, FUN_1400085c0, FUN_140008820 */

undefined8 FUN_140008580(undefined8 param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined8 uVar3;
  
  puVar2 = (undefined4 *)FUN_140008520();
  if (puVar2 != (undefined4 *)0x0) {
    iVar1 = FUN_1400085c0(param_1);
    if (iVar1 != -1) {
      uVar3 = FUN_140008820(param_1,iVar1,*puVar2);
      return uVar3;
    }
  }
  return 0;
}


/* ================= FUN_1400085c0 @ 1400085c0  size=87  callers=1 callees=2 ================= */
/* CALLERS: FUN_140008580 */
/* CALLEES: FUN_140007860, FUN_1400078a0 */

uint FUN_1400085c0(longlong param_1)

{
  int iVar1;
  longlong lVar2;
  
  lVar2 = FUN_1400078a0();
  if (lVar2 != 0) {
    if (*(ushort *)(lVar2 + 0x3e) != 0xffff) {
      return (uint)*(ushort *)(lVar2 + 0x3e);
    }
    iVar1 = FUN_140007860();
    if (iVar1 == 2) {
      return *(uint *)(*(longlong *)(param_1 + 0x38) + 0x28 + *(longlong *)(lVar2 + 0x28));
    }
    if (iVar1 == 1) {
      return *(uint *)(*(longlong *)(param_1 + 0x38) + 0x18 + *(longlong *)(lVar2 + 0x28));
    }
  }
  return 0xffffffff;
}


/* ================= FUN_140008620 @ 140008620  size=45  callers=6 callees=1 ================= */
/* CALLERS: FUN_140002a50, FUN_140003d10, FUN_140003e50, FUN_1400076d0, FUN_14000aaf0, FUN_14000bf80 */
/* CALLEES: FUN_140008520 */

ulonglong FUN_140008620(void)

{
  longlong lVar1;
  ulonglong uVar2;
  
  lVar1 = FUN_140008520();
  if (lVar1 != 0) {
    uVar2 = ~*(ulonglong *)(lVar1 + 0x10);
    if (*(ulonglong *)(lVar1 + 0x20) <= ~*(ulonglong *)(lVar1 + 0x10)) {
      uVar2 = *(ulonglong *)(lVar1 + 0x20);
    }
    return uVar2;
  }
  return 0;
}


/* ================= FUN_140008650 @ 140008650  size=27  callers=1 callees=1 ================= */
/* CALLERS: FUN_14000bf80 */
/* CALLEES: FUN_140008520 */

undefined4 FUN_140008650(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008520();
  if (lVar1 != 0) {
    return *(undefined4 *)(lVar1 + 4);
  }
  return 0;
}


/* ================= FUN_140008670 @ 140008670  size=28  callers=3 callees=1 ================= */
/* CALLERS: FUN_140002a80, FUN_140008750, FUN_14000cea0 */
/* CALLEES: FUN_1400078a0 */

undefined2 FUN_140008670(void)

{
  longlong lVar1;
  
  lVar1 = FUN_1400078a0();
  if (lVar1 != 0) {
    return *(undefined2 *)(lVar1 + 0x38);
  }
  return 0;
}


/* ================= FUN_140008690 @ 140008690  size=94  callers=1 callees=5 ================= */
/* CALLERS: FUN_140002aa0 */
/* CALLEES: FUN_140007880, FUN_140007920, FUN_1400086f0, FUN_140008710, FUN_140008750 */

longlong FUN_140008690(void)

{
  longlong lVar1;
  longlong lVar2;
  longlong lVar3;
  ulonglong uVar4;
  
  lVar1 = FUN_140007880();
  if (lVar1 != 0) {
    lVar2 = FUN_140008750();
    if (lVar2 != 0) {
      lVar2 = FUN_1400086f0();
      lVar3 = FUN_140008710();
      if (lVar3 != 0) {
        uVar4 = FUN_140007920();
        if ((ulonglong)(lVar3 + lVar2) <= uVar4) {
          return lVar1 + lVar2;
        }
      }
    }
  }
  return 0;
}


/* ================= FUN_1400086f0 @ 1400086f0  size=28  callers=1 callees=1 ================= */
/* CALLERS: FUN_140008690 */
/* CALLEES: FUN_140008750 */

undefined8 FUN_1400086f0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008750();
  if (lVar1 != 0) {
    return *(undefined8 *)(lVar1 + 8);
  }
  return 0;
}


/* ================= FUN_140008710 @ 140008710  size=28  callers=2 callees=1 ================= */
/* CALLERS: FUN_140002ad0, FUN_140008690 */
/* CALLEES: FUN_140008750 */

undefined8 FUN_140008710(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008750();
  if (lVar1 != 0) {
    return *(undefined8 *)(lVar1 + 0x20);
  }
  return 0;
}


/* ================= FUN_140008730 @ 140008730  size=27  callers=1 callees=1 ================= */
/* CALLERS: FUN_140002b00 */
/* CALLEES: FUN_140008750 */

undefined4 FUN_140008730(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008750();
  if (lVar1 != 0) {
    return *(undefined4 *)(lVar1 + 4);
  }
  return 0;
}


/* ================= FUN_140008750 @ 140008750  size=46  callers=8 callees=2 ================= */
/* CALLERS: FUN_140008690, FUN_1400086f0, FUN_140008710, FUN_140008730, FUN_140008780, FUN_1400087a0, FUN_1400087d0, FUN_1400087f0 */
/* CALLEES: FUN_140008670, FUN_14000a2f0 */

longlong FUN_140008750(longlong param_1,uint param_2)

{
  char cVar1;
  uint uVar2;
  ulonglong uVar3;
  
  uVar3 = (ulonglong)param_2;
  cVar1 = FUN_14000a2f0();
  if (cVar1 != '\0') {
    uVar2 = FUN_140008670();
    if ((uint)uVar3 < uVar2) {
      return uVar3 * 0x38 + *(longlong *)(param_1 + 0x50);
    }
  }
  return 0;
}


/* ================= FUN_140008780 @ 140008780  size=28  callers=1 callees=1 ================= */
/* CALLERS: FUN_140002b30 */
/* CALLEES: FUN_140008750 */

undefined8 FUN_140008780(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008750();
  if (lVar1 != 0) {
    return *(undefined8 *)(lVar1 + 0x28);
  }
  return 0;
}


/* ================= FUN_1400087a0 @ 1400087a0  size=35  callers=1 callees=1 ================= */
/* CALLERS: FUN_140002b60 */
/* CALLEES: FUN_140008750 */

undefined8 FUN_1400087a0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008750();
  if (lVar1 != 0) {
    return *(undefined8 *)(lVar1 + 0x18);
  }
  return 0xffffffffffffffff;
}


/* ================= FUN_1400087d0 @ 1400087d0  size=26  callers=1 callees=1 ================= */
/* CALLERS: FUN_140002b90 */
/* CALLEES: FUN_140008750 */

undefined4 FUN_1400087d0(void)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)FUN_140008750();
  if (puVar1 != (undefined4 *)0x0) {
    return *puVar1;
  }
  return 0;
}


/* ================= FUN_1400087f0 @ 1400087f0  size=35  callers=1 callees=1 ================= */
/* CALLERS: FUN_140002bc0 */
/* CALLEES: FUN_140008750 */

undefined8 FUN_1400087f0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140008750();
  if (lVar1 != 0) {
    return *(undefined8 *)(lVar1 + 0x10);
  }
  return 0xffffffffffffffff;
}


/* ================= FUN_140008820 @ 140008820  size=34  callers=2 callees=1 ================= */
/* CALLERS: FUN_140007690, FUN_140008580 */
/* CALLEES: FUN_140008460 */

longlong FUN_140008820(undefined8 param_1,undefined8 param_2,longlong param_3)

{
  longlong lVar1;
  
  lVar1 = FUN_140008460();
  if (lVar1 != 0) {
    return lVar1 + param_3;
  }
  return 0;
}


/* ================= FUN_140008850 @ 140008850  size=140  callers=2 callees=2 ================= */
/* CALLERS: FUN_140002fb0, FUN_14000b830 */
/* CALLEES: FUN_140004f60, FUN_14000a630 */

ulonglong FUN_140008850(longlong param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  ulonglong uVar4;
  char local_res8 [8];
  
  if ((param_1 != 0) && (cVar1 = FUN_14000a630(), cVar1 != '\0')) {
    uVar4 = FUN_140004f60();
    if (local_res8[0] != '\0') {
      return uVar4;
    }
    iVar2 = FUN_140004f60(param_1,0x22,local_res8);
    if (local_res8[0] == '\0') {
      iVar2 = 0;
    }
    iVar3 = FUN_140004f60(param_1,0x2f,local_res8);
    if (local_res8[0] == '\0') {
      iVar3 = 0;
    }
    return (ulonglong)((iVar3 - iVar2) + 1);
  }
  return 0;
}


/* ================= FUN_1400088e0 @ 1400088e0  size=311  callers=1 callees=7 ================= */
/* CALLERS: FUN_140002da0 */
/* CALLEES: FUN_140005ef0, FUN_140007320, FUN_140007450, FUN_1400074d0, FUN_140007780, FUN_140009f90, FUN_14000a670 */

undefined8 * FUN_1400088e0(undefined8 *param_1,undefined8 param_2,undefined8 param_3)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  char cVar6;
  int iVar7;
  undefined4 uVar8;
  undefined8 *puVar9;
  undefined4 local_res8;
  undefined4 uStackX_c;
  undefined8 local_res20;
  undefined1 local_b8 [176];
  
  iVar7 = FUN_14000a670(param_2);
  if (iVar7 != 0) {
    iVar7 = FUN_140007320(param_2,&local_res20,&local_res8);
    if (iVar7 != 0) {
      puVar9 = (undefined8 *)FUN_140005ef0(local_b8,CONCAT44(uStackX_c,local_res8),param_3);
      goto LAB_140008997;
    }
    iVar7 = FUN_140007450(param_2,&local_res20,&local_res8);
    if (iVar7 != 0) {
      uVar8 = FUN_140007780(local_res20,local_res8);
      cVar6 = FUN_140009f90(uVar8);
      if (cVar6 == '\0') {
        puVar9 = (undefined8 *)FUN_1400074d0(local_b8,local_res20,local_res8);
        goto LAB_140008997;
      }
    }
  }
  puVar9 = (undefined8 *)&DAT_14005ec20;
LAB_140008997:
  uVar1 = puVar9[1];
  uVar2 = puVar9[2];
  uVar3 = puVar9[3];
  *param_1 = *puVar9;
  param_1[1] = uVar1;
  uVar1 = puVar9[4];
  uVar4 = puVar9[5];
  param_1[2] = uVar2;
  param_1[3] = uVar3;
  uVar2 = puVar9[6];
  uVar3 = puVar9[7];
  param_1[4] = uVar1;
  param_1[5] = uVar4;
  uVar1 = puVar9[8];
  uVar4 = puVar9[9];
  param_1[6] = uVar2;
  param_1[7] = uVar3;
  uVar2 = puVar9[10];
  uVar3 = puVar9[0xb];
  param_1[8] = uVar1;
  param_1[9] = uVar4;
  uVar1 = puVar9[0xc];
  uVar4 = puVar9[0xd];
  param_1[10] = uVar2;
  param_1[0xb] = uVar3;
  uVar2 = puVar9[0x10];
  uVar3 = puVar9[0x11];
  param_1[0xc] = uVar1;
  param_1[0xd] = uVar4;
  uVar1 = puVar9[0xf];
  param_1[0xe] = puVar9[0xe];
  param_1[0xf] = uVar1;
  uVar4 = puVar9[0x12];
  uVar5 = puVar9[0x13];
  uVar1 = puVar9[0x14];
  param_1[0x10] = uVar2;
  param_1[0x11] = uVar3;
  param_1[0x12] = uVar4;
  param_1[0x13] = uVar5;
  param_1[0x14] = uVar1;
  return param_1;
}


/* ================= FUN_140008a20 @ 140008a20  size=54  callers=2 callees=1 ================= */
/* CALLERS: FUN_140007320, FUN_140007450 */
/* CALLEES: FUN_14000a670 */

undefined8 FUN_140008a20(ulonglong param_1,ulonglong *param_2)

{
  int iVar1;
  
  if (param_2 != (ulonglong *)0x0) {
    iVar1 = FUN_14000a670();
    if (iVar1 != 0) {
      *param_2 = param_1 & 0x1ffffffffffff;
      return 1;
    }
  }
  return 0;
}


/* ================= FUN_140008a60 @ 140008a60  size=106  callers=0 callees=5 ================= */
/* CALLEES: FUN_140006c00, FUN_140007320, FUN_140007450, FUN_140007690, FUN_14000a670 */

undefined8 FUN_140008a60(undefined8 param_1)

{
  int iVar1;
  undefined8 uVar2;
  undefined4 local_res10;
  undefined4 uStackX_14;
  undefined8 local_res18 [2];
  
  iVar1 = FUN_14000a670();
  if (iVar1 != 0) {
    iVar1 = FUN_140007320(param_1,local_res18,&local_res10);
    if (iVar1 != 0) {
      uVar2 = FUN_140006c00(CONCAT44(uStackX_14,local_res10),0);
      return uVar2;
    }
    iVar1 = FUN_140007450(param_1,local_res18,&local_res10);
    if (iVar1 != 0) {
      uVar2 = FUN_140007690(local_res18[0],local_res10);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_140008ad0 @ 140008ad0  size=110  callers=0 callees=5 ================= */
/* CALLEES: FUN_140006fb0, FUN_140007320, FUN_140007450, FUN_140007780, FUN_14000a670 */

longlong FUN_140008ad0(undefined8 param_1)

{
  int iVar1;
  longlong lVar2;
  undefined4 local_res10;
  undefined4 uStackX_14;
  undefined8 local_res18 [2];
  
  iVar1 = FUN_14000a670();
  if (iVar1 != 0) {
    iVar1 = FUN_140007320(param_1,local_res18,&local_res10);
    if (iVar1 != 0) {
      lVar2 = FUN_140006fb0(CONCAT44(uStackX_14,local_res10));
      return lVar2;
    }
    iVar1 = FUN_140007450(param_1,local_res18,&local_res10);
    if (iVar1 != 0) {
      iVar1 = FUN_140007780(local_res18[0],local_res10);
      return (longlong)iVar1;
    }
  }
  return 0xffff;
}


/* ================= FUN_140008b40 @ 140008b40  size=68  callers=0 callees=3 ================= */
/* CALLEES: FUN_140007450, FUN_140008d20, FUN_14000a670 */

undefined8 FUN_140008b40(undefined8 param_1)

{
  int iVar1;
  undefined8 uVar2;
  undefined4 local_res10 [2];
  undefined8 local_res18 [2];
  
  iVar1 = FUN_14000a670();
  if (iVar1 != 0) {
    iVar1 = FUN_140007450(param_1,local_res18,local_res10);
    if (iVar1 != 0) {
      uVar2 = FUN_140008d20(local_res18[0],local_res10[0]);
      return uVar2;
    }
  }
  return 0xffffffffffffffff;
}


/* ================= FUN_140008b90 @ 140008b90  size=195  callers=0 callees=6 ================= */
/* CALLEES: DDX_FieldText, FUN_140007320, FUN_140007450, FUN_140007750, FUN_140008d20, FUN_14000a670 */

undefined8 FUN_140008b90(undefined8 param_1,undefined4 param_2,longlong *param_3)

{
  int iVar1;
  undefined8 uVar2;
  longlong lVar3;
  longlong lVar4;
  undefined4 local_res20;
  undefined4 uStackX_24;
  undefined8 local_18 [2];
  
  iVar1 = FUN_14000a670();
  if (iVar1 != 0) {
    iVar1 = FUN_140007320(param_1,local_18,&local_res20);
    if (iVar1 != 0) {
      uVar2 = DDX_FieldText(CONCAT44(uStackX_24,local_res20),param_2,param_3);
      return uVar2;
    }
    iVar1 = FUN_140007450(param_1,local_18,&local_res20);
    if (iVar1 != 0) {
      if (param_3 != (longlong *)0x0) {
        lVar3 = FUN_140008d20(local_18[0],local_res20);
        lVar4 = FUN_140007750(local_18[0],local_res20);
        *param_3 = lVar3;
        param_3[1] = lVar4 + lVar3;
      }
      return 1;
    }
  }
  return 0;
}


/* ================= FUN_140008c60 @ 140008c60  size=35  callers=2 callees=1 ================= */
/* CALLERS: FUN_140009e00, FUN_140009e30 */
/* CALLEES: FUN_14000a670 */

ulonglong FUN_140008c60(undefined8 param_1)

{
  ulonglong uVar1;
  
  uVar1 = FUN_14000a670(param_1,param_1);
  if ((int)uVar1 == 0) {
    return uVar1;
  }
  return (ulonglong)((uint)((ulonglong)param_1 >> 0x33) & 1);
}


/* ================= FUN_140008c90 @ 140008c90  size=28  callers=1 callees=1 ================= */
/* CALLERS: FUN_140007780 */
/* CALLEES: FUN_140007630 */

undefined1 FUN_140008c90(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140007630();
  if (lVar1 != 0) {
    return *(undefined1 *)(lVar1 + 4);
  }
  return 0;
}


/* ================= FUN_140008cb0 @ 140008cb0  size=104  callers=0 callees=5 ================= */
/* CALLEES: FUN_140006ef0, FUN_140007320, FUN_140007450, FUN_140007750, FUN_14000a670 */

undefined8 FUN_140008cb0(undefined8 param_1)

{
  int iVar1;
  undefined8 uVar2;
  undefined4 local_res10;
  undefined4 uStackX_14;
  undefined8 local_res18 [2];
  
  iVar1 = FUN_14000a670();
  if (iVar1 != 0) {
    iVar1 = FUN_140007320(param_1,local_res18,&local_res10);
    if (iVar1 != 0) {
      uVar2 = FUN_140006ef0(CONCAT44(uStackX_14,local_res10));
      return uVar2;
    }
    iVar1 = FUN_140007450(param_1,local_res18,&local_res10);
    if (iVar1 != 0) {
      uVar2 = FUN_140007750(local_res18[0],local_res10);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_140008d20 @ 140008d20  size=21  callers=4 callees=2 ================= */
/* CALLERS: FUN_1400074d0, FUN_140008b40, FUN_140008b90, FUN_14000db80 */
/* CALLEES: FUN_140007630, FUN_1400077b0 */

void FUN_140008d20(void)

{
  undefined8 uVar1;
  
  uVar1 = FUN_140007630();
  FUN_1400077b0(uVar1);
  return;
}


/* ================= FUN_140008d40 @ 140008d40  size=127  callers=1 callees=3 ================= */
/* CALLERS: FUN_140006d80 */
/* CALLEES: FUN_140006fb0, FUN_1400080f0, FUN_14000a630 */

longlong FUN_140008d40(longlong param_1,ulonglong param_2)

{
  char cVar1;
  ulonglong uVar2;
  longlong lVar3;
  
  if ((param_1 != 0) && (cVar1 = FUN_14000a630(), cVar1 != '\0')) {
    uVar2 = FUN_140006fb0();
    if (uVar2 == (param_2 & 0xffffffff)) {
      return param_1;
    }
    for (lVar3 = FUN_1400080f0(param_1,0x49); lVar3 != 0; lVar3 = FUN_1400080f0(lVar3,0x49)) {
      uVar2 = FUN_140006fb0(lVar3);
      if (uVar2 == (param_2 & 0xffffffff)) {
        return lVar3;
      }
    }
  }
  return 0;
}


/* ================= FUN_140008dc0 @ 140008dc0  size=177  callers=1 callees=1 ================= */
/* CALLERS: FUN_140002fb0 */
/* CALLEES: FUN_140006fb0 */

char * FUN_140008dc0(void)

{
  undefined8 uVar1;
  
  uVar1 = FUN_140006fb0();
  switch(uVar1) {
  case 2:
    return "class";
  default:
    return (char *)0x0;
  case 4:
    return "enum";
  case 0xf:
    return "*";
  case 0x10:
    return "&";
  case 0x13:
    return "struct";
  case 0x17:
    return "union";
  case 0x26:
    return "const";
  case 0x35:
    return "volatile";
  case 0x37:
    return "restrict";
  case 0x47:
    return "_Atomic";
  }
}


/* ================= FUN_140008ef0 @ 140008ef0  size=66  callers=6 callees=0 ================= */
/* CALLERS: FUN_140003d10, FUN_140003e50, FUN_140007ea0, FUN_140007f40, FUN_140007ff0, FUN_1400094f0 */

ulonglong FUN_140008ef0(longlong param_1,uint param_2)

{
  ulonglong uVar1;
  
  if ((param_2 < *(uint *)(param_1 + 4)) &&
     (uVar1 = (*(longlong *)(*(longlong *)(param_1 + 0x60) + (ulonglong)param_2 * 8) -
              *(longlong *)(param_1 + 0x58)) / 0x18, (uint)uVar1 < *(uint *)(param_1 + 4))) {
    return uVar1 & 0xffffffff;
  }
  return 0xffffffff;
}


/* ================= FUN_140008f40 @ 140008f40  size=182  callers=3 callees=2 ================= */
/* CALLERS: FUN_1400079f0, FUN_140007a10, FUN_140007b70 */
/* CALLEES: FUN_140002040, FUN_14000a2f0 */

undefined8 FUN_140008f40(longlong param_1,undefined8 param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  cVar1 = FUN_14000a2f0();
  if (cVar1 != '\0') {
    switch(param_2) {
    case 0x10:
      uVar2 = FUN_140002040(param_1 + 0x308);
      return uVar2;
    case 0x11:
      uVar2 = FUN_140002040(param_1 + 0x340);
      return uVar2;
    case 0x1d:
      uVar2 = FUN_140002040(param_1 + 0x3b0);
      return uVar2;
    case 0x1e:
      uVar2 = FUN_140002040(param_1 + 0x2d0);
      return uVar2;
    case 0x2e:
      uVar2 = FUN_140002040(param_1 + 0x378);
      return uVar2;
    case 0x34:
      uVar2 = FUN_140002040(param_1 + 0x260);
      return uVar2;
    case 0x39:
      uVar2 = FUN_140002040(param_1 + 0x298);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_140009050 @ 140009050  size=48  callers=2 callees=0 ================= */
/* CALLERS: FUN_140004df0, FUN_1400055d0 */

ulonglong FUN_140009050(longlong param_1)

{
  ulonglong in_RAX;
  
  if (param_1 != 0) {
    in_RAX = 0;
    switch(*(int *)(param_1 + 0x18) + -1) {
    case 0:
    case 0x1a:
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
      return 1;
    }
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_1400090c0 @ 1400090c0  size=52  callers=10 callees=2 ================= */
/* CALLERS: FUN_1400041b0, FUN_1400043a0, FUN_140004ce0, FUN_140004d70, FUN_140004f60, FUN_1400064c0, FUN_140009da0, FUN_14000a4b0, FUN_14000c7e0, FUN_14000d6d0 */
/* CALLEES: FUN_14000a630, FUN_140015300 */

ulonglong FUN_1400090c0(longlong param_1)

{
  ulonglong in_RAX;
  undefined1 local_res8;
  
  if (param_1 != 0) {
    in_RAX = FUN_14000a630();
    if ((char)in_RAX != '\0') {
      in_RAX = FUN_140015300();
      if ((int)in_RAX == 0) {
        return (ulonglong)local_res8;
      }
    }
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_140009100 @ 140009100  size=22  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400043a0 */

ulonglong FUN_140009100(longlong param_1)

{
  ulonglong in_RAX;
  
  if ((param_1 != 0) && (in_RAX = 0, *(longlong *)(param_1 + 0x38) != 0)) {
    return (ulonglong)*(byte *)(*(longlong *)(param_1 + 0x38) + 0x10);
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_140009120 @ 140009120  size=361  callers=0 callees=4 ================= */
/* CALLEES: FUN_140006210, FUN_140009e60, FUN_14000a1d0, FUN_14000a630 */

undefined8 FUN_140009120(longlong param_1)

{
  char cVar1;
  undefined8 *puVar2;
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
  undefined8 local_118;
  undefined8 uStack_110;
  undefined8 local_108;
  undefined8 uStack_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined4 local_e8;
  undefined4 uStack_e4;
  undefined4 uStack_e0;
  undefined4 uStack_dc;
  undefined4 local_d8;
  undefined4 uStack_d4;
  undefined4 uStack_d0;
  undefined4 uStack_cc;
  undefined8 local_c8;
  undefined1 local_b8 [176];
  
  if (param_1 != 0) {
    cVar1 = FUN_14000a630();
    if (cVar1 != '\0') {
      puVar2 = (undefined8 *)FUN_140006210(local_b8,param_1,0,2);
      local_168 = *puVar2;
      uStack_160 = puVar2[1];
      local_158 = puVar2[2];
      uStack_150 = puVar2[3];
      local_148 = puVar2[4];
      uStack_140 = puVar2[5];
      local_138 = puVar2[6];
      uStack_130 = puVar2[7];
      local_128 = puVar2[8];
      uStack_120 = puVar2[9];
      local_118 = puVar2[10];
      uStack_110 = puVar2[0xb];
      local_108 = puVar2[0xc];
      uStack_100 = puVar2[0xd];
      local_e8 = *(undefined4 *)(puVar2 + 0x10);
      uStack_e4 = *(undefined4 *)((longlong)puVar2 + 0x84);
      uStack_e0 = *(undefined4 *)(puVar2 + 0x11);
      uStack_dc = *(undefined4 *)((longlong)puVar2 + 0x8c);
      local_f8 = puVar2[0xe];
      uStack_f0 = puVar2[0xf];
      local_d8 = *(undefined4 *)(puVar2 + 0x12);
      uStack_d4 = *(undefined4 *)((longlong)puVar2 + 0x94);
      uStack_d0 = *(undefined4 *)(puVar2 + 0x13);
      uStack_cc = *(undefined4 *)((longlong)puVar2 + 0x9c);
      local_c8 = puVar2[0x14];
      cVar1 = FUN_14000a1d0(&local_168);
      if (cVar1 != '\0') {
        local_168 = *puVar2;
        uStack_160 = puVar2[1];
        local_c8 = puVar2[0x14];
        local_158 = puVar2[2];
        uStack_150 = puVar2[3];
        local_148 = puVar2[4];
        uStack_140 = puVar2[5];
        local_138 = puVar2[6];
        uStack_130 = puVar2[7];
        local_128 = puVar2[8];
        uStack_120 = puVar2[9];
        local_118 = puVar2[10];
        uStack_110 = puVar2[0xb];
        local_108 = puVar2[0xc];
        uStack_100 = puVar2[0xd];
        local_f8 = puVar2[0xe];
        uStack_f0 = puVar2[0xf];
        local_e8 = *(undefined4 *)(puVar2 + 0x10);
        uStack_e4 = *(undefined4 *)((longlong)puVar2 + 0x84);
        uStack_e0 = *(undefined4 *)(puVar2 + 0x11);
        uStack_dc = *(undefined4 *)((longlong)puVar2 + 0x8c);
        local_d8 = *(undefined4 *)(puVar2 + 0x12);
        uStack_d4 = *(undefined4 *)((longlong)puVar2 + 0x94);
        uStack_d0 = *(undefined4 *)(puVar2 + 0x13);
        uStack_cc = *(undefined4 *)((longlong)puVar2 + 0x9c);
        cVar1 = FUN_140009e60(&local_168);
        if (cVar1 != '\0') {
          return 1;
        }
      }
      return 0;
    }
  }
  return 0;
}


/* ================= FUN_140009290 @ 140009290  size=17  callers=1 callees=0 ================= */
/* CALLERS: FUN_140006210 */

undefined1 FUN_140009290(longlong param_1)

{
  if ((param_1 != 0) && (*(int *)(param_1 + 0x18) == 0x18)) {
    return 1;
  }
  return 0;
}


/* ================= FUN_1400092b0 @ 1400092b0  size=24  callers=1 callees=0 ================= */
/* CALLERS: FUN_140004df0 */

ulonglong FUN_1400092b0(longlong param_1)

{
  uint uVar1;
  ulonglong in_RAX;
  
  if (param_1 != 0) {
    uVar1 = *(uint *)(param_1 + 0x18);
    in_RAX = (ulonglong)uVar1;
    if ((uVar1 == 0xc) || (uVar1 == 0x19)) {
      return CONCAT71((uint7)(uint3)(uVar1 >> 8),1);
    }
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_1400092d0 @ 1400092d0  size=49  callers=1 callees=0 ================= */
/* CALLERS: FUN_140004df0 */

ulonglong FUN_1400092d0(longlong param_1)

{
  ulonglong in_RAX;
  
  if (param_1 != 0) {
    in_RAX = 0;
    switch(*(int *)(param_1 + 0x18) + -0x10) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 0xb:
    case 0xc:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
      return 1;
    }
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_140009330 @ 140009330  size=16  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400080f0 */

bool FUN_140009330(longlong param_1)

{
  if (param_1 == 0) {
    return false;
  }
  return *(int *)(param_1 + 0x18) == 0x20;
}


/* ================= FUN_140009340 @ 140009340  size=49  callers=1 callees=0 ================= */
/* CALLERS: FUN_140004df0 */

ulonglong FUN_140009340(longlong param_1)

{
  ulonglong in_RAX;
  
  if (param_1 != 0) {
    in_RAX = 0;
    switch(*(int *)(param_1 + 0x18) + -5) {
    case 0:
    case 1:
    case 2:
    case 6:
    case 8:
    case 0x12:
      return 1;
    }
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_140009390 @ 140009390  size=49  callers=2 callees=0 ================= */
/* CALLERS: FUN_140004df0, FUN_1400055d0 */

ulonglong FUN_140009390(longlong param_1)

{
  ulonglong in_RAX;
  
  if (param_1 != 0) {
    in_RAX = 0;
    switch(*(int *)(param_1 + 0x18) + -5) {
    case 0:
    case 1:
    case 2:
    case 6:
    case 10:
    case 0x12:
    case 0x16:
    case 0x17:
    case 0x1d:
    case 0x1e:
    case 0x1f:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
      return 1;
    }
  }
  return in_RAX & 0xffffffffffffff00;
}


/* ================= FUN_140009400 @ 140009400  size=231  callers=1 callees=9 ================= */
/* CALLERS: FUN_14000d7a0 */
/* CALLEES: FUN_140001ef0, FUN_140002050, FUN_140002250, FUN_140007630, FUN_140007780, FUN_14000a2f0, FUN_14000a770, FUN_140019ae0, thunk_FUN_140001fd0 */

undefined8 FUN_140009400(longlong param_1)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  uint uVar4;
  ulonglong uVar5;
  longlong local_res10 [3];
  undefined1 local_48 [64];
  
  lVar3 = param_1;
  cVar1 = FUN_14000a2f0();
  if (cVar1 != '\0') {
    uVar5 = 0;
    *(undefined4 *)(lVar3 + 0x10) = 0;
    FUN_140002250(local_48,1000000,8,0,0,0);
    if (*(int *)(param_1 + 4) != 0) {
      do {
        iVar2 = FUN_140007780(param_1,uVar5);
        if (iVar2 == 4) {
          *(int *)(param_1 + 0x10) = *(int *)(param_1 + 0x10) + 1;
          local_res10[0] = FUN_140007630(param_1,uVar5);
          *(ulonglong *)(local_res10[0] + 8) = uVar5;
          FUN_140001ef0(local_48,local_res10);
        }
        uVar4 = (int)uVar5 + 1;
        uVar5 = (ulonglong)uVar4;
      } while (uVar4 < *(uint *)(param_1 + 4));
    }
    lVar3 = FUN_140019ae0(*(int *)(param_1 + 0x10) << 3);
    *(longlong *)(param_1 + 0x70) = lVar3;
    if (lVar3 != 0) {
      FUN_140002050(local_48,0,*(undefined4 *)(param_1 + 0x10),lVar3);
      thunk_FUN_140001fd0(local_48);
      return 0;
    }
    FUN_14000a770(0x48);
  }
  return 0xffffffff;
}


/* ================= FUN_1400094f0 @ 1400094f0  size=298  callers=1 callees=11 ================= */
/* CALLERS: FUN_14000d7a0 */
/* CALLEES: FUN_140001ef0, FUN_140002050, FUN_140002250, FUN_1400022c0, FUN_140007630, FUN_140008ef0, FUN_14000a2f0, FUN_14000a770, FUN_14000db80, FUN_140019ae0, thunk_FUN_140001fd0 */

undefined8 FUN_1400094f0(longlong param_1)

{
  char cVar1;
  undefined4 uVar2;
  longlong lVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined8 local_res10;
  undefined1 local_48 [16];
  longlong local_38;
  
  lVar3 = param_1;
  cVar1 = FUN_14000a2f0();
  if (cVar1 != '\0') {
    uVar5 = 0;
    uVar4 = 0xffffffff;
    *(undefined4 *)(lVar3 + 0xc) = 0;
    FUN_140002250(local_48,1000000,8,0,0,0);
    if (*(int *)(param_1 + 4) != 0) {
      do {
        uVar2 = FUN_140008ef0(param_1,uVar5);
        cVar1 = FUN_14000db80(param_1,uVar2,uVar4);
        if (cVar1 != '\0') {
          if (cVar1 == '\x02') {
            *(int *)(param_1 + 0xc) = *(int *)(param_1 + 0xc) + -1;
            FUN_1400022c0(local_48,local_38 + -1);
          }
          *(int *)(param_1 + 0xc) = *(int *)(param_1 + 0xc) + 1;
          local_res10 = FUN_140007630(param_1,uVar2);
          FUN_140001ef0(local_48,&local_res10);
          uVar4 = uVar2;
        }
        uVar5 = uVar5 + 1;
      } while (uVar5 < *(uint *)(param_1 + 4));
    }
    lVar3 = FUN_140019ae0(*(int *)(param_1 + 0xc) << 3);
    *(longlong *)(param_1 + 0x68) = lVar3;
    if (lVar3 != 0) {
      FUN_140002050(local_48,0,*(undefined4 *)(param_1 + 0xc),lVar3);
      thunk_FUN_140001fd0(local_48);
      return 0;
    }
    FUN_14000a770(0x48);
  }
  return 0xffffffff;
}

