
/* ================= FUN_140029290 @ 140029290  size=33  callers=2 callees=0 ================= */
/* CALLERS: FUN_1400297e0, FUN_140029930 */

void FUN_140029290(longlong param_1,undefined1 *param_2,int param_3)

{
  if (param_3 != 0) {
    param_1 = param_1 - (longlong)param_2;
    do {
      param_2[param_1] = *param_2;
      param_2 = param_2 + 1;
      param_3 = param_3 + -1;
    } while (param_3 != 0);
  }
  return;
}


/* ================= FUN_1400292c0 @ 1400292c0  size=36  callers=2 callees=0 ================= */
/* CALLERS: FUN_1400293b0, FUN_140029490 */

int FUN_1400292c0(longlong param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  
  iVar2 = 0;
  param_1 = param_1 - (longlong)param_2;
  do {
    cVar1 = *param_2;
    iVar2 = iVar2 + 1;
    param_2[param_1] = cVar1;
    param_2 = param_2 + 1;
  } while (cVar1 != '\0');
  return iVar2;
}


/* ================= FUN_1400292f0 @ 1400292f0  size=188  callers=1 callees=4 ================= */
/* CALLERS: FUN_1400296f0 */
/* CALLEES: FUN_140029920, RegCloseKey, RegOpenKeyA, RegQueryValueExA */

undefined8 FUN_1400292f0(HKEY param_1,LPBYTE param_2,DWORD param_3)

{
  LSTATUS LVar1;
  DWORD local_res20 [2];
  HKEY local_18 [2];
  
  LVar1 = RegOpenKeyA(param_1,"Software\\SEGGER\\J-Link",local_18);
  if ((LVar1 != 0) &&
     (LVar1 = RegOpenKeyA(param_1,"Software\\SEGGER\\Flasher",local_18), LVar1 != 0)) {
    return 0xffffffff;
  }
  local_res20[0] = param_3;
  FUN_140029920(param_2,0,param_3);
  RegQueryValueExA(local_18[0],"InstallPath",(LPDWORD)0x0,(LPDWORD)0x0,param_2,local_res20);
  if (*param_2 == '\0') {
    RegCloseKey(local_18[0]);
    return 0xffffffff;
  }
  RegCloseKey(local_18[0]);
  return 0;
}


/* ================= FUN_1400293b0 @ 1400293b0  size=214  callers=29 callees=5 ================= */
/* CALLERS: JLINKARM_Close, JLINKARM_ConfigJTAG, JLINKARM_Connect, JLINKARM_DEVICE_GetInfo, JLINKARM_DEVICE_SelectDialog, JLINKARM_EMU_HasCapEx, JLINKARM_EMU_SelectByUSBSN, JLINKARM_ExecCommand, JLINKARM_GetCompileDateTime, JLINKARM_GetDLLVersion, JLINKARM_Go, JLINKARM_Halt, JLINKARM_IsHalted, JLINKARM_OpenEx, JLINKARM_Reset, JLINKARM_SelectIP, JLINKARM_SetSpeed, JLINKARM_TIF_Select, JLINK_EMU_SelectByUSBNickname, JLINK_ExecCommand, JLINK_Go, JLINK_HSS_GetCaps, JLINK_HSS_Read, JLINK_HSS_Start, JLINK_HSS_Stop, JLINK_IsHalted, JLINK_RTTERMINAL_Control, JLINK_RTTERMINAL_Read, JLINK_RTTERMINAL_Write */
/* CALLEES: FUN_1400292c0, FUN_140029490, GetProcAddress, MessageBoxA, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

FARPROC FUN_1400293b0(LPCSTR param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  FARPROC pFVar4;
  undefined1 auStack_238 [32];
  CHAR local_218 [512];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_238;
  cVar1 = FUN_140029490(1);
  if (cVar1 < '\0') {
    pFVar4 = (FARPROC)0x0;
  }
  else {
    pFVar4 = GetProcAddress(DAT_14085b3a8,param_1);
    if (pFVar4 == (FARPROC)0x0) {
      iVar2 = FUN_1400292c0(local_218,"DLL function ");
      iVar2 = iVar2 + -1;
      iVar3 = FUN_1400292c0(local_218 + iVar2,param_1);
      FUN_1400292c0(local_218 + (iVar3 + iVar2 + -1),"(...) not found !");
      MessageBoxA((HWND)0x0,local_218,"J-Link Error",0x42030);
    }
  }
  return pFVar4;
}


/* ================= FUN_140029490 @ 140029490  size=599  callers=2 callees=10 ================= */
/* CALLERS: FUN_140028ec0, FUN_1400293b0 */
/* CALLEES: FUN_1400292c0, FUN_1400296f0, FUN_1400297e0, FUN_140029930, GetModuleFileNameA, LoadLibraryA, MessageBoxA, VirtualQuery, __security_check_cookie, exit */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_140029490(int param_1)

{
  int iVar1;
  int iVar2;
  undefined8 uVar3;
  SIZE_T SVar4;
  undefined1 auStack_698 [32];
  undefined1 *local_678;
  undefined1 local_668 [8];
  _MEMORY_BASIC_INFORMATION local_660;
  CHAR local_628 [272];
  undefined1 local_518 [256];
  undefined1 local_418 [256];
  undefined1 local_318 [256];
  CHAR local_218 [512];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_698;
  if (DAT_14085b3a8 == (HMODULE)0x0) {
    local_628[0] = '\0';
    SVar4 = VirtualQuery(&LAB_140028ee0,&local_660,0x30);
    if ((int)SVar4 != 0) {
      GetModuleFileNameA(local_660.AllocationBase,local_628,0x104);
    }
    if (local_628[0] != '\0') {
      local_678 = local_418;
      FUN_140029930(local_628,local_668,local_518,local_318);
      local_678 = (undefined1 *)0x0;
      FUN_1400297e0(local_628,local_668,local_518,"JLink_x64.dll");
      DAT_14085b3a8 = LoadLibraryA(local_628);
    }
    if (DAT_14085b3a8 == (HMODULE)0x0) {
      local_628[0] = '\0';
      GetModuleFileNameA(DAT_14085b3a0,local_628,0x104);
      if (local_628[0] != '\0') {
        local_678 = local_418;
        FUN_140029930(local_628,local_668,local_518,local_318);
        local_678 = (undefined1 *)0x0;
        FUN_1400297e0(local_628,local_668,local_518,"JLink_x64.dll");
        DAT_14085b3a8 = LoadLibraryA(local_628);
      }
      if (DAT_14085b3a8 == (HMODULE)0x0) {
        FUN_1400296f0(0xffffffff80000001);
        if (DAT_14085b3a8 == (HMODULE)0x0) {
          FUN_1400296f0(0xffffffff80000002);
          if (DAT_14085b3a8 == (HMODULE)0x0) {
            if (param_1 != 0) {
              iVar1 = FUN_1400292c0(local_218,"Target DLL ");
              iVar1 = iVar1 + -1;
              iVar2 = FUN_1400292c0(local_218 + iVar1,local_628);
              FUN_1400292c0(local_218 + (iVar2 + iVar1 + -1)," not found !");
              MessageBoxA((HWND)0x0,local_218,"J-Link Error",0x42030);
                    /* WARNING: Subroutine does not return */
              exit(1);
            }
            uVar3 = 0xff;
          }
          else {
            uVar3 = 4;
          }
        }
        else {
          uVar3 = 3;
        }
      }
      else {
        uVar3 = 2;
      }
    }
    else {
      uVar3 = 1;
    }
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}


/* ================= FUN_1400296f0 @ 1400296f0  size=234  callers=1 callees=5 ================= */
/* CALLERS: FUN_140029490 */
/* CALLEES: FUN_1400292f0, FUN_1400297e0, FUN_140029930, LoadLibraryA, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_1400296f0(undefined8 param_1)

{
  int iVar1;
  undefined1 auStack_468 [32];
  HMODULE local_448;
  undefined1 local_438 [16];
  CHAR local_428 [272];
  undefined1 local_318 [256];
  undefined1 local_218 [256];
  undefined1 local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_468;
  iVar1 = FUN_1400292f0(param_1,local_428,0x104);
  if (iVar1 == 0) {
    local_448 = (HMODULE)local_218;
    FUN_140029930(local_428,local_438,local_318,local_118);
    local_448 = (HMODULE)0x0;
    FUN_1400297e0(local_428,local_438,local_318,"JLink_x64.dll");
    DAT_14085b3a8 = LoadLibraryA(local_428);
    if (DAT_14085b3a8 == (HMODULE)0x0) {
      local_448 = DAT_14085b3a8;
      FUN_1400297e0(local_428,local_438,local_318,"bin_x64\\JLink_x64.dll");
      DAT_14085b3a8 = LoadLibraryA(local_428);
    }
  }
  return 0;
}


/* ================= FUN_1400297e0 @ 1400297e0  size=320  callers=2 callees=2 ================= */
/* CALLERS: FUN_140029490, FUN_1400296f0 */
/* CALLEES: FUN_1400277b0, FUN_140029290 */

void FUN_1400297e0(undefined1 *param_1,longlong param_2,char *param_3,longlong param_4,char *param_5
                  )

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  int iVar4;
  ulonglong uVar5;
  
  uVar5 = 0;
  iVar4 = 0;
  if (param_2 != 0) {
    iVar1 = FUN_1400277b0(param_2);
    if ((iVar1 != 0) && (*(char *)((longlong)iVar1 + -1 + param_2) != ':')) {
      *param_1 = 0x3a;
      uVar5 = 1;
    }
    iVar4 = (int)uVar5;
    FUN_140029290(param_1 + (uVar5 & 0xffffffff),param_2,iVar1);
    iVar4 = iVar4 + iVar1;
  }
  if (param_3 != (char *)0x0) {
    iVar1 = FUN_1400277b0(param_3);
    lVar3 = (longlong)iVar1;
    if (iVar1 != 0) {
      if ((*param_3 != '\\') && (*param_3 != '/')) {
        lVar2 = (longlong)iVar4;
        iVar4 = iVar4 + 1;
        param_1[lVar2] = 0x5c;
      }
      FUN_140029290(param_1 + iVar4,param_3,iVar1);
      iVar4 = iVar4 + (int)lVar3;
      if ((param_3[lVar3 + -1] == '\\') || (param_3[lVar3 + -1] == '/')) goto LAB_1400298a2;
    }
  }
  lVar3 = (longlong)iVar4;
  iVar4 = iVar4 + 1;
  param_1[lVar3] = 0x5c;
LAB_1400298a2:
  if (param_4 != 0) {
    iVar1 = FUN_1400277b0(param_4);
    FUN_140029290(param_1 + iVar4,param_4,iVar1);
    iVar4 = iVar4 + iVar1;
  }
  if (param_5 != (char *)0x0) {
    iVar1 = FUN_1400277b0(param_5);
    if (*param_5 != '.') {
      lVar3 = (longlong)iVar4;
      iVar4 = iVar4 + 1;
      param_1[lVar3] = 0x2e;
    }
    FUN_140029290(param_1 + iVar4,param_5,iVar1);
    param_1[iVar1 + iVar4] = 0;
    return;
  }
  param_1[iVar4] = 0;
  return;
}


/* ================= FUN_140029920 @ 140029920  size=14  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400292f0 */
/* CALLEES: memset */

void FUN_140029920(void *param_1,int param_2,uint param_3)

{
  if (param_3 != 0) {
    memset(param_1,param_2,(ulonglong)param_3);
    return;
  }
  return;
}


/* ================= FUN_140029930 @ 140029930  size=405  callers=2 callees=1 ================= */
/* CALLERS: FUN_140029490, FUN_1400296f0 */
/* CALLEES: FUN_140029290 */

void FUN_140029930(char *param_1,undefined1 *param_2,undefined1 *param_3,undefined1 *param_4,
                  undefined1 *param_5)

{
  char *pcVar1;
  char cVar2;
  longlong lVar3;
  char *pcVar4;
  int iVar5;
  int iVar6;
  
  if (param_2 != (undefined1 *)0x0) {
    *param_2 = 0;
  }
  if (param_3 != (undefined1 *)0x0) {
    *param_3 = 0;
  }
  if (param_4 != (undefined1 *)0x0) {
    *param_4 = 0;
  }
  if (param_5 != (undefined1 *)0x0) {
    *param_5 = 0;
  }
  iVar6 = 0;
  pcVar4 = param_1;
  if (*param_1 != '\0') {
    do {
      iVar6 = iVar6 + 1;
      pcVar4 = pcVar4 + 1;
    } while (*pcVar4 != '\0');
    if (iVar6 != 0) {
      iVar5 = 0;
      lVar3 = (longlong)iVar6;
      if (iVar6 != 0) {
        do {
          cVar2 = param_1[lVar3 + -1];
          if (cVar2 == '.') {
            iVar5 = iVar5 + 1;
            break;
          }
          if ((cVar2 == '\\') || (cVar2 == '/')) {
            iVar5 = 0;
            break;
          }
          iVar5 = iVar5 + 1;
          lVar3 = lVar3 + -1;
        } while (lVar3 != 0);
      }
      iVar6 = iVar6 - iVar5;
      lVar3 = (longlong)iVar6;
      if (param_5 != (undefined1 *)0x0) {
        FUN_140029290(param_5,param_1 + lVar3,iVar5);
        param_5[iVar5] = 0;
      }
      if (iVar5 != 0) {
        iVar5 = 0;
        for (; ((lVar3 != 0 && (param_1[lVar3 + -1] != '\\')) && (param_1[lVar3 + -1] != '/'));
            lVar3 = lVar3 + -1) {
          iVar5 = iVar5 + 1;
        }
        iVar6 = iVar6 - iVar5;
        if (param_4 != (undefined1 *)0x0) {
          FUN_140029290(param_4,param_1 + iVar6,iVar5);
          param_4[iVar5] = 0;
        }
      }
      iVar5 = 0;
      cVar2 = *param_1;
      pcVar4 = param_1;
      while (cVar2 != ':') {
        if ((cVar2 == '\\') || (cVar2 == '/')) {
          iVar5 = 0;
          goto LAB_140029a72;
        }
        if (cVar2 == '\0') goto LAB_140029a72;
        pcVar1 = pcVar4 + 1;
        pcVar4 = pcVar4 + 1;
        iVar5 = iVar5 + 1;
        cVar2 = *pcVar1;
      }
      iVar5 = iVar5 + 1;
LAB_140029a72:
      lVar3 = (longlong)iVar5;
      if (param_2 != (undefined1 *)0x0) {
        FUN_140029290(param_2,param_1,iVar5);
        param_2[lVar3] = 0;
      }
      iVar6 = iVar6 - iVar5;
      if (param_3 != (undefined1 *)0x0) {
        FUN_140029290(param_3,param_1 + lVar3,iVar6);
        param_3[iVar6] = 0;
      }
    }
  }
  return;
}


/* ================= FUN_140029ad0 @ 140029ad0  size=281  callers=1 callees=8 ================= */
/* CALLERS: FUN_14004c6a0 */
/* CALLEES: FUN_140029f00, FUN_140029fa0, QMenuBar, addMenu, hide, setCheckable, setChecked, show */

QMenuBar * FUN_140029ad0(QMenuBar *param_1,QWidget *param_2)

{
  QMenu *pQVar1;
  undefined8 *puVar2;
  longlong lVar3;
  int iVar4;
  
  QMenuBar::QMenuBar(param_1,param_2);
  *(undefined ***)param_1 = MenuBar::vftable;
  *(undefined ***)(param_1 + 0x10) = MenuBar::vftable;
  *(undefined8 *)(param_1 + 0x28) = 0;
  QWidget::hide((QWidget *)param_1);
  *(QWidget **)(param_1 + 0x28) = param_2;
  puVar2 = &DAT_1400963c8;
  lVar3 = 4;
  do {
    pQVar1 = (QMenu *)FUN_140029f00(param_1,puVar2[-3]);
    *puVar2 = pQVar1;
    QMenuBar::addMenu(param_1,pQVar1);
    iVar4 = 0;
    if (0 < *(int *)(puVar2 + -1)) {
      do {
        FUN_140029fa0(param_1,*puVar2,puVar2[-2] + (longlong)iVar4 * 0x28);
        iVar4 = iVar4 + 1;
      } while (iVar4 < *(int *)(puVar2 + -1));
    }
    puVar2 = puVar2 + 4;
    lVar3 = lVar3 + -1;
  } while (lVar3 != 0);
  QAction::setCheckable(DAT_140096300,true);
  QAction::setChecked(DAT_140096300,true);
  QAction::setCheckable(DAT_140096328,true);
  QAction::setChecked(DAT_140096328,true);
  QWidget::show((QWidget *)param_1);
  return param_1;
}


/* ================= FUN_140029bf0 @ 140029bf0  size=181  callers=1 callees=0 ================= */
/* CALLERS: FUN_140029ce0 */

void FUN_140029bf0(QMenuBar *param_1)

{
  longlong *plVar1;
  int iVar2;
  longlong lVar3;
  longlong lVar4;
  int *piVar5;
  
  *(undefined ***)param_1 = MenuBar::vftable;
  piVar5 = &DAT_1400963c0;
  lVar3 = 4;
  *(undefined ***)(param_1 + 0x10) = MenuBar::vftable;
  do {
    iVar2 = 0;
    if (0 < *piVar5) {
      lVar4 = 0;
      do {
        plVar1 = *(longlong **)(*(longlong *)(piVar5 + -2) + 0x20 + lVar4);
        if (plVar1 != (longlong *)0x0) {
          (**(code **)(*plVar1 + 0x18))(plVar1,1);
        }
        iVar2 = iVar2 + 1;
        lVar4 = lVar4 + 0x28;
      } while (iVar2 < *piVar5);
    }
    plVar1 = *(longlong **)(piVar5 + 2);
    if (plVar1 != (longlong *)0x0) {
      (**(code **)(*plVar1 + 0x18))(plVar1,1);
    }
    piVar5 = piVar5 + 8;
    lVar3 = lVar3 + -1;
  } while (lVar3 != 0);
                    /* WARNING: Could not recover jumptable at 0x000140029c9e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMenuBar::~QMenuBar(param_1);
  return;
}


/* ================= FUN_140029cbc @ 140029cbc  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140029ce0 */

void FUN_140029cbc(longlong param_1)

{
  FUN_140029ce0(param_1 + -0x10);
  return;
}


/* ================= FUN_140029cc8 @ 140029cc8  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140029d60 */

void FUN_140029cc8(longlong param_1)

{
  FUN_140029d60(param_1 + -0x10);
  return;
}


/* ================= FUN_140029ce0 @ 140029ce0  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_140029cbc */
/* CALLEES: FUN_140029bf0, free */

void * FUN_140029ce0(void *param_1,ulonglong param_2)

{
  FUN_140029bf0();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140029d20 @ 140029d20  size=53  callers=0 callees=2 ================= */
/* CALLEES: free, ~QAction */

QAction * FUN_140029d20(QAction *param_1,uint param_2)

{
  QAction::~QAction(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140029d60 @ 140029d60  size=53  callers=2 callees=2 ================= */
/* CALLERS: FUN_140029cc8, FUN_140038090 */
/* CALLEES: free, ~QMenu */

QMenu * FUN_140029d60(QMenu *param_1,uint param_2)

{
  QMenu::~QMenu(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140029da0 @ 140029da0  size=336  callers=2 callees=4 ================= */
/* CALLERS: FUN_14004d160, FUN_14004d870 */
/* CALLEES: FUN_14003ed50, FUN_14003ef10, FUN_14004ea40, setEnabled */

void FUN_140029da0(void)

{
  char cVar1;
  int iVar2;
  undefined8 *puVar3;
  longlong lVar4;
  
  cVar1 = FUN_14003ef10();
  puVar3 = &DAT_1400961c0;
  lVar4 = 8;
  if (cVar1 != '\0') {
    do {
      QAction::setEnabled((QAction *)*puVar3,false);
      puVar3 = puVar3 + 5;
      lVar4 = lVar4 + -1;
    } while (lVar4 != 0);
                    /* WARNING: Could not recover jumptable at 0x000140029de8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    QAction::setEnabled(DAT_140096350,false);
    return;
  }
  do {
    QAction::setEnabled((QAction *)*puVar3,true);
    puVar3 = puVar3 + 5;
    lVar4 = lVar4 + -1;
  } while (lVar4 != 0);
  iVar2 = FUN_14004ea40();
  if (iVar2 == 0) {
    QAction::setEnabled(DAT_140096350,false);
    QAction::setEnabled(DAT_140096210,false);
    QAction::setEnabled(DAT_140096238,false);
    QAction::setEnabled(DAT_140096260,false);
    QAction::setEnabled(DAT_140096288,false);
  }
  else {
    QAction::setEnabled(DAT_140096350,true);
    QAction::setEnabled(DAT_140096210,true);
    QAction::setEnabled(DAT_140096238,true);
    QAction::setEnabled(DAT_140096260,true);
    QAction::setEnabled(DAT_140096288,true);
  }
  QAction::setEnabled(DAT_1400962b0,iVar2 != 0);
  iVar2 = FUN_14003ed50();
  if (iVar2 == 0) {
    QAction::setEnabled(DAT_140096260,false);
    QAction::setEnabled(DAT_140096288,false);
    QAction::setEnabled(DAT_1400962b0,false);
  }
  return;
}


/* ================= FUN_140029f00 @ 140029f00  size=145  callers=1 callees=5 ================= */
/* CALLERS: FUN_140029ad0 */
/* CALLEES: FUN_140058d94, QMenu, fromAscii_helper, setTitle, ~QString */

QMenu * FUN_140029f00(QWidget *param_1,char *param_2)

{
  QMenu *pQVar1;
  QMenu *local_res18 [2];
  
  pQVar1 = (QMenu *)FUN_140058d94(0x28);
  local_res18[0] = pQVar1;
  if (pQVar1 == (QMenu *)0x0) {
    pQVar1 = (QMenu *)0x0;
  }
  else {
    QMenu::QMenu(pQVar1,param_1);
    *(undefined ***)pQVar1 = QMenu::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QMenu::vftable;
  }
  local_res18[0] = (QMenu *)QString::fromAscii_helper(param_2,-1);
  QMenu::setTitle(pQVar1,(QString *)local_res18);
  QString::~QString((QString *)local_res18);
  return pQVar1;
}


/* ================= FUN_140029fa0 @ 140029fa0  size=278  callers=1 callees=10 ================= */
/* CALLERS: FUN_140029ad0 */
/* CALLEES: FUN_140058d94, QAction, addAction, connect, fromAscii_helper, operator_int, setShortcut, setStatusTip, setText, ~QString */

undefined8 FUN_140029fa0(QObject *param_1,QWidget *param_2,undefined8 *param_3)

{
  int iVar1;
  QAction *pQVar2;
  QAction *local_res18;
  
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  local_res18 = pQVar2;
  if (pQVar2 == (QAction *)0x0) {
    pQVar2 = (QAction *)0x0;
  }
  else {
    QAction::QAction(pQVar2,param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
  }
  param_3[4] = pQVar2;
  local_res18 = (QAction *)QString::fromAscii_helper((char *)*param_3,-1);
  QAction::setText((QAction *)param_3[4],(QString *)&local_res18);
  QString::~QString((QString *)&local_res18);
  iVar1 = QKeySequence::operator_int((QKeySequence *)(param_3 + 1));
  if (iVar1 != 0) {
    QAction::setShortcut((QAction *)param_3[4],(QKeySequence *)(param_3 + 1));
  }
  if ((char *)param_3[2] != (char *)0x0) {
    QObject::connect((QObject *)param_3[4],"2triggered(void)",param_1,(char *)param_3[2],0);
  }
  QWidget::addAction(param_2,(QAction *)param_3[4]);
  local_res18 = (QAction *)QString::fromAscii_helper((char *)param_3[3],-1);
  QAction::setStatusTip((QAction *)param_3[4],(QString *)&local_res18);
  QString::~QString((QString *)&local_res18);
  return param_3[4];
}


/* ================= FUN_14002a0c0 @ 14002a0c0  size=101  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003beb0 */
/* CALLEES: FUN_14002ce80, FUN_14003ea60, FUN_140058d94, exec */

void FUN_14002a0c0(undefined8 param_1)

{
  longlong lVar1;
  undefined8 uVar2;
  QDialog *this;
  
  lVar1 = FUN_140058d94(0x30);
  if (lVar1 == 0) {
    this = (QDialog *)0x0;
  }
  else {
    uVar2 = FUN_14003ea60();
    this = (QDialog *)FUN_14002ce80(lVar1,param_1,uVar2);
  }
  QDialog::exec(this);
  if (this != (QDialog *)0x0) {
    (**(code **)(*(longlong *)this + 0x18))(this,1);
  }
  return;
}


/* ================= FUN_14002a130 @ 14002a130  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003beb0 */
/* CALLEES: FUN_14004dca0 */

void FUN_14002a130(void)

{
  FUN_14004dca0(DAT_14286d388);
  return;
}


/* ================= FUN_14002a160 @ 14002a160  size=41  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003beb0 */
/* CALLEES: FUN_14004cf50, isChecked */

void FUN_14002a160(longlong param_1)

{
  bool bVar1;
  
  bVar1 = QAction::isChecked(DAT_140096328);
  FUN_14004cf50(*(undefined8 *)(param_1 + 0x28),bVar1);
  return;
}


/* ================= FUN_14002a190 @ 14002a190  size=9  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003beb0 */
/* CALLEES: FUN_14004cf60 */

void FUN_14002a190(longlong param_1)

{
  FUN_14004cf60(*(undefined8 *)(param_1 + 0x28));
  return;
}


/* ================= FUN_14002a1a0 @ 14002a1a0  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003beb0 */
/* CALLEES: FUN_14004de70 */

void FUN_14002a1a0(void)

{
  FUN_14004de70(DAT_14286d388);
  return;
}


/* ================= FUN_14002a1b0 @ 14002a1b0  size=13  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003beb0 */
/* CALLEES: FUN_14003e420 */

void FUN_14002a1b0(undefined8 param_1)

{
  FUN_14003e420(param_1,0,0,0);
  return;
}


/* ================= FUN_14002a1c0 @ 14002a1c0  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003beb0 */
/* CALLEES: FUN_14004dea0 */

void FUN_14002a1c0(void)

{
  FUN_14004dea0(DAT_14286d388);
  return;
}


/* ================= FUN_14002a1f0 @ 14002a1f0  size=102  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003beb0 */
/* CALLEES: FUN_140028380, FUN_140036e60, FUN_14003f640, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14002a1f0(void)

{
  int iVar1;
  undefined1 auStack_1c8 [32];
  undefined1 local_1a8 [400];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_1c8;
  iVar1 = FUN_140036e60("https://kb.segger.com/UM08028_J-Scope");
  if (iVar1 < 0) {
    FUN_140028380(local_1a8,0x184,"Failed to open User Guide at:\n\"%s\"",local_1a8);
    FUN_14003f640(local_1a8);
  }
  return;
}


/* ================= FUN_14002a260 @ 14002a260  size=41  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003beb0 */
/* CALLEES: FUN_14004d140, isChecked */

void FUN_14002a260(longlong param_1)

{
  bool bVar1;
  
  bVar1 = QAction::isChecked(DAT_140096300);
  FUN_14004d140(*(undefined8 *)(param_1 + 0x28),bVar1);
  return;
}


/* ================= FUN_14002a290 @ 14002a290  size=142  callers=2 callees=4 ================= */
/* CALLERS: FUN_1400487b0, FUN_140051790 */
/* CALLEES: QPixmap, loadFromData, operator=, ~QPixmap */

QPixmap * FUN_14002a290(QPixmap *param_1,uint param_2)

{
  QPixmap *pQVar1;
  undefined4 local_res10 [2];
  QPixmap local_20 [24];
  
  QPixmap::QPixmap(param_1);
  pQVar1 = (QPixmap *)QPixmap::QPixmap(local_20);
  QPixmap::operator=(param_1,pQVar1);
  QPixmap::~QPixmap(local_20);
  local_res10[0] = 0;
  QPixmap::loadFromData
            (param_1,(&PTR_PNG_140096430)[(ulonglong)param_2 * 3],
             (&DAT_140096440)[(ulonglong)param_2 * 6],0,local_res10);
  return param_1;
}


/* ================= FUN_14002a320 @ 14002a320  size=18  callers=11 callees=0 ================= */
/* CALLERS: FUN_140055480, FUN_1400555d0, FUN_140055730, FUN_140055800, FUN_140055960, FUN_140055ac0, FUN_140055bb0, FUN_140055dc0, FUN_140055ed0, FUN_140056950, FUN_140056a00 */

undefined8 FUN_14002a320(ulonglong param_1)

{
  return (&DAT_140096438)[(param_1 & 0xffffffff) * 3];
}


/* ================= FUN_14002a340 @ 14002a340  size=207  callers=1 callees=6 ================= */
/* CALLERS: FUN_14004c6a0 */
/* CALLEES: FUN_140058d94, QIcon, QPixmap, loadFromData, operator=, ~QPixmap */

undefined4 FUN_14002a340(void)

{
  QPixmap *pQVar1;
  undefined *puVar2;
  undefined **ppuVar3;
  longlong lVar4;
  QIcon *local_res8;
  QPixmap local_38 [24];
  QPixmap local_20 [24];
  
  QPixmap::QPixmap(local_38);
  ppuVar3 = &PTR_PNG_140096430;
  lVar4 = 0x48;
  do {
    pQVar1 = (QPixmap *)QPixmap::QPixmap(local_20);
    QPixmap::operator=(local_38,pQVar1);
    QPixmap::~QPixmap(local_20);
    local_res8 = (QIcon *)((ulonglong)local_res8 & 0xffffffff00000000);
    QPixmap::loadFromData(local_38,*ppuVar3,*(undefined4 *)(ppuVar3 + 2),0,&local_res8);
    local_res8 = (QIcon *)FUN_140058d94(8);
    puVar2 = (undefined *)0x0;
    if (local_res8 != (QIcon *)0x0) {
      puVar2 = (undefined *)QIcon::QIcon(local_res8,local_38);
    }
    ppuVar3[1] = puVar2;
    ppuVar3 = ppuVar3 + 3;
    lVar4 = lVar4 + -1;
  } while (lVar4 != 0);
  QPixmap::~QPixmap(local_38);
  return 0x48;
}


/* ================= FUN_14002a410 @ 14002a410  size=4584  callers=4 callees=46 ================= */
/* CALLERS: FUN_14004d380, FUN_14004d4e0, FUN_14004d9c0, FUN_1400578b0 */
/* CALLEES: FUN_14002c650, FUN_140035c20, FUN_140035d30, FUN_140035e00, FUN_140035e90, FUN_140035f30, FUN_140035ff0, FUN_1400360a0, FUN_140036330, FUN_140036e00, FUN_140036f30, FUN_1400373a0, FUN_140037670, FUN_14003f430, FUN_140043d20, FUN_14004e9e0, FUN_140058d94, QButtonGroup, QDialog, QHBoxLayout, QRegExp, QRegExpValidator, QWidget, addButton, addLayout, addWidget, connect, fromAscii_helper, hide, memset, number, resize, setChecked, setCurrentIndex, setExclusive, setFixedSize, setLayout, setRegExp, setSizeConstraint, setSpacing, setText, setValidator, setWindowTitle, show, ~QRegExp, ~QString */

QWidget * FUN_14002a410(QWidget *param_1,undefined8 param_2,QWidget param_3)

{
  int iVar1;
  void *_Dst;
  QLayout *pQVar2;
  QGridLayout *pQVar3;
  undefined8 uVar4;
  QWidget *pQVar5;
  QSize *pQVar6;
  longlong *plVar7;
  longlong lVar8;
  QWidget *pQVar9;
  QButtonGroup *this;
  QHBoxLayout *pQVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  QRegExpValidator *this_00;
  QString *pQVar13;
  QAbstractButton *pQVar14;
  QButtonGroup *this_01;
  QHBoxLayout *pQVar15;
  QRegExpValidator *this_02;
  undefined4 local_res18 [2];
  QRegExp local_res20 [8];
  Data *local_58;
  Data *local_50;
  QButtonGroup *local_48 [2];
  
  this_02 = (QRegExpValidator *)0x0;
  local_res18[0] = 0;
  QDialog::QDialog((QDialog *)param_1,param_2,local_res18);
  *(undefined ***)param_1 = NewProjectDialog::vftable;
  *(undefined ***)(param_1 + 0x10) = NewProjectDialog::vftable;
  local_58 = QString::fromAscii_helper("^[0-9]*$",-1);
  QRegExp::QRegExp(local_res20,(QString *)&local_58,1,0);
  QString::~QString((QString *)&local_58);
  QWidget::hide(param_1);
  memset(param_1 + 0x28,0,0x120);
  param_1[0x140] = param_3;
  if (param_3 == (QWidget)0x0) {
    local_50 = QString::fromAscii_helper("Create new project",-1);
    QWidget::setWindowTitle(param_1,(QString *)&local_50);
  }
  else {
    local_50 = QString::fromAscii_helper("Project settings",-1);
    QWidget::setWindowTitle(param_1,(QString *)&local_50);
  }
  QString::~QString((QString *)&local_50);
  _Dst = (void *)FUN_140043d20(0x5e8);
  *(void **)(param_1 + 0x138) = _Dst;
  if (_Dst == (void *)0x0) {
    FUN_14003f430("Insufficient memory!");
  }
  else {
    memset(_Dst,0,0x5e8);
    if (param_3 != (QWidget)0x0) {
      FUN_14004e9e0(*(undefined8 *)(param_1 + 0x138));
    }
    pQVar2 = (QLayout *)FUN_140035e90(0);
    *(QLayout **)(param_1 + 0x130) = pQVar2;
    QWidget::setLayout(param_1,pQVar2);
    QLayout::setSizeConstraint(*(QLayout **)(param_1 + 0x130),3);
    FUN_140036f30(param_1);
    pQVar3 = (QGridLayout *)FUN_140035e90(0);
    uVar4 = FUN_140035f30(param_1,"Connection to J-Link",pQVar3);
    *(undefined8 *)(param_1 + 0x28) = uVar4;
    uVar4 = FUN_140036330(param_1,&DAT_14006c788,1,param_1,"1_SlotOnToggleRadioUSB (bool)");
    *(undefined8 *)(param_1 + 0x68) = uVar4;
    uVar4 = FUN_140036330(param_1,"TCP/IP",0,param_1,"1_SlotOnToggleRadioTCPIP(bool)");
    *(undefined8 *)(param_1 + 0x90) = uVar4;
    uVar4 = FUN_140036330(param_1,"Existing Session",0,param_1,"1_SlotOnToggleRadioExSes(bool)");
    *(undefined8 *)(param_1 + 0xa0) = uVar4;
    uVar4 = FUN_140035d30(param_1,"SN / Nickname","2stateChanged(int)",param_1,
                          "1_SlotOnToggleCheckUSB(int)");
    *(undefined8 *)(param_1 + 0x70) = uVar4;
    uVar4 = FUN_1400360a0(param_1,0,0);
    *(undefined8 *)(param_1 + 0x78) = uVar4;
    uVar4 = FUN_1400360a0(param_1,0,0);
    *(undefined8 *)(param_1 + 0x98) = uVar4;
    pQVar5 = (QWidget *)FUN_140058d94(0x28);
    pQVar9 = (QWidget *)this_02;
    local_50 = (Data *)pQVar5;
    if (pQVar5 != (QWidget *)0x0) {
      local_res18[0] = 0;
      QWidget::QWidget(pQVar5,param_1,local_res18);
      *(undefined ***)pQVar5 = QWidget::vftable;
      *(undefined ***)(pQVar5 + 0x10) = QWidget::vftable;
      pQVar9 = pQVar5;
    }
    *(QWidget **)(param_1 + 0x88) = pQVar9;
    pQVar5 = (QWidget *)FUN_140058d94(0x28);
    pQVar9 = (QWidget *)this_02;
    local_50 = (Data *)pQVar5;
    if (pQVar5 != (QWidget *)0x0) {
      local_res18[0] = 0;
      QWidget::QWidget(pQVar5,param_1,local_res18);
      *(undefined ***)pQVar5 = QWidget::vftable;
      *(undefined ***)(pQVar5 + 0x10) = QWidget::vftable;
      pQVar9 = pQVar5;
    }
    *(QWidget **)(param_1 + 0x80) = pQVar9;
    QWidget::hide(*(QWidget **)(param_1 + 0x78));
    QWidget::show(*(QWidget **)(param_1 + 0x88));
    QWidget::hide(*(QWidget **)(param_1 + 0x80));
    QWidget::hide(*(QWidget **)(param_1 + 0x98));
    pQVar6 = (QSize *)(**(code **)(**(longlong **)(param_1 + 0x70) + 0x60))
                                (*(longlong **)(param_1 + 0x70),&local_50);
    QWidget::setFixedSize(*(QWidget **)(param_1 + 0x70),pQVar6);
    pQVar6 = (QSize *)(**(code **)(**(longlong **)(param_1 + 0x70) + 0x60))
                                (*(longlong **)(param_1 + 0x70),&local_50);
    QWidget::setFixedSize(*(QWidget **)(param_1 + 0x80),pQVar6);
    pQVar6 = (QSize *)(**(code **)(**(longlong **)(param_1 + 0x78) + 0x60))
                                (*(longlong **)(param_1 + 0x78),&local_50);
    QWidget::setFixedSize(*(QWidget **)(param_1 + 0x78),pQVar6);
    pQVar6 = (QSize *)(**(code **)(**(longlong **)(param_1 + 0x78) + 0x60))
                                (*(longlong **)(param_1 + 0x78),&local_50);
    QWidget::setFixedSize(*(QWidget **)(param_1 + 0x88),pQVar6);
    pQVar6 = (QSize *)(**(code **)(**(longlong **)(param_1 + 0x98) + 0x60))
                                (*(longlong **)(param_1 + 0x98),&local_50);
    QWidget::setFixedSize(*(QWidget **)(param_1 + 0x98),pQVar6);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x68),0,0,1,1,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x90),1,0,1,2,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0xa0),2,0,1,2,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x70),0,1,1,1,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x80),0,1,1,1,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x78),0,2,1,1,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x88),0,2,1,1,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x98),1,2,1,1,local_res18);
    QWidget::hide(*(QWidget **)(param_1 + 0xa0));
    pQVar3 = (QGridLayout *)FUN_140035e90(0);
    uVar4 = FUN_140035f30(param_1,"Specify Target Device",pQVar3);
    *(undefined8 *)(param_1 + 0x30) = uVar4;
    uVar4 = FUN_1400360a0(param_1,0,0);
    *(undefined8 *)(param_1 + 0xa8) = uVar4;
    plVar7 = (longlong *)
             FUN_140035c20(param_1,&DAT_140060544,param_1,"1_SlotOnReleaseBtnTarget(void)");
    *(longlong **)(param_1 + 0xb0) = plVar7;
    lVar8 = (**(code **)(*plVar7 + 0x60))(plVar7,&local_50);
    iVar1 = *(int *)(lVar8 + 4);
    lVar8 = (**(code **)(**(longlong **)(param_1 + 0xb0) + 0x60))
                      (*(longlong **)(param_1 + 0xb0),local_48);
    QWidget::setFixedSize(*(QWidget **)(param_1 + 0xb0),*(int *)(lVar8 + 4),iVar1);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0xa8),0,0,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0xb0),0,1,local_res18);
    pQVar3 = (QGridLayout *)FUN_140035e90(0);
    uVar4 = FUN_140035f30(param_1,"Script file (optional)",pQVar3);
    *(undefined8 *)(param_1 + 0x38) = uVar4;
    uVar4 = FUN_1400360a0(param_1,0,0);
    *(undefined8 *)(param_1 + 0xb8) = uVar4;
    plVar7 = (longlong *)
             FUN_140035c20(param_1,&DAT_140060544,param_1,"1_SlotOnReleaseBtnScript(void)");
    *(longlong **)(param_1 + 0xc0) = plVar7;
    lVar8 = (**(code **)(*plVar7 + 0x60))(plVar7,local_48);
    iVar1 = *(int *)(lVar8 + 4);
    lVar8 = (**(code **)(**(longlong **)(param_1 + 0xc0) + 0x60))
                      (*(longlong **)(param_1 + 0xc0),&local_50);
    QWidget::setFixedSize(*(QWidget **)(param_1 + 0xc0),*(int *)(lVar8 + 4),iVar1);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0xb8),0,0,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0xc0),0,1,local_res18);
    pQVar3 = (QGridLayout *)FUN_140035e90(0);
    uVar4 = FUN_140035f30(param_1,"Target Interface & Speed",pQVar3);
    *(undefined8 *)(param_1 + 0x40) = uVar4;
    uVar4 = FUN_140035e00(param_1,&PTR_DAT_140096af0,4);
    *(undefined8 *)(param_1 + 200) = uVar4;
    local_48[0] = (QButtonGroup *)FUN_140058d94(0x48);
    pQVar9 = (QWidget *)this_02;
    if (local_48[0] != (QButtonGroup *)0x0) {
      pQVar9 = (QWidget *)FUN_1400373a0(local_48[0],param_1,"1_SlotOnChangedCmbBxSpeed(void)");
    }
    *(QWidget **)(param_1 + 0xd0) = pQVar9;
    uVar4 = FUN_140035ff0(param_1,&DAT_14006c650);
    *(undefined8 *)(param_1 + 0xd8) = uVar4;
    QObject::connect(*(QObject **)(param_1 + 200),"2currentIndexChanged(int)",(QObject *)param_1,
                     "1_SlotOnChangedCmbBxInterface(int)",0);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 200),0,0,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0xd0),0,1,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0xd8),0,2,local_res18);
    pQVar3 = (QGridLayout *)FUN_140035e90(0);
    uVar4 = FUN_140035f30(param_1,"Sampling Source",pQVar3);
    *(undefined8 *)(param_1 + 0x48) = uVar4;
    QGridLayout::setSpacing(pQVar3,0);
    this = (QButtonGroup *)FUN_140058d94(0x10);
    this_01 = (QButtonGroup *)this_02;
    local_48[0] = this;
    if (this != (QButtonGroup *)0x0) {
      QButtonGroup::QButtonGroup(this,(QObject *)param_1);
      *(undefined ***)this = QButtonGroup::vftable;
      this_01 = this;
    }
    *(QButtonGroup **)(param_1 + 0xe0) = this_01;
    QButtonGroup::setExclusive(this_01,true);
    QObject::connect(*(QObject **)(param_1 + 0xe0),"2buttonClicked(int)",(QObject *)param_1,
                     "1_SlotOnBtnGroupSamplingModeClicked(int)",0);
    uVar4 = FUN_140036330(param_1,"HSS (asynchronous)",1,param_1,0);
    *(undefined8 *)(param_1 + 0xe8) = uVar4;
    uVar4 = FUN_140035ff0(param_1,
                          "<a href=\"https://kb.segger.com/UM08028_J-Scope#Asynchronous_Mode_(HSS)\">?</a>"
                         );
    FUN_140036e00(uVar4,1,1);
    pQVar10 = (QHBoxLayout *)FUN_140058d94(0x20);
    pQVar15 = (QHBoxLayout *)this_02;
    local_48[0] = (QButtonGroup *)pQVar10;
    if (pQVar10 != (QHBoxLayout *)0x0) {
      QHBoxLayout::QHBoxLayout(pQVar10);
      *(undefined ***)pQVar10 = QHBoxLayout::vftable;
      *(undefined ***)(pQVar10 + 0x10) = QHBoxLayout::vftable;
      pQVar15 = pQVar10;
    }
    local_res18[0] = 0;
    QBoxLayout::addWidget((QBoxLayout *)pQVar15,*(undefined8 *)(param_1 + 0xe8),0,local_res18);
    local_res18[0] = 0;
    QBoxLayout::addWidget((QBoxLayout *)pQVar15,uVar4,0,local_res18);
    local_res18[0] = 1;
    QGridLayout::addLayout(pQVar3,pQVar15,0,0,local_res18);
    QButtonGroup::addButton
              (*(QButtonGroup **)(param_1 + 0xe0),*(QAbstractButton **)(param_1 + 0xe8));
    uVar4 = FUN_140036330(param_1,"RTT (synchronus)",0,param_1,0);
    *(undefined8 *)(param_1 + 0xf0) = uVar4;
    uVar4 = FUN_140035ff0(param_1,
                          "<a href=\"https://kb.segger.com/UM08028_J-Scope#Synchronous_Mode_(RTT)\">?</a>"
                         );
    FUN_140036e00(uVar4,1,1);
    pQVar10 = (QHBoxLayout *)FUN_140058d94(0x20);
    pQVar15 = (QHBoxLayout *)this_02;
    local_48[0] = (QButtonGroup *)pQVar10;
    if (pQVar10 != (QHBoxLayout *)0x0) {
      QHBoxLayout::QHBoxLayout(pQVar10);
      *(undefined ***)pQVar10 = QHBoxLayout::vftable;
      *(undefined ***)(pQVar10 + 0x10) = QHBoxLayout::vftable;
      pQVar15 = pQVar10;
    }
    local_res18[0] = 0;
    QBoxLayout::addWidget((QBoxLayout *)pQVar15,*(undefined8 *)(param_1 + 0xf0),0,local_res18);
    local_res18[0] = 0;
    QBoxLayout::addWidget((QBoxLayout *)pQVar15,uVar4,0,local_res18);
    local_res18[0] = 1;
    QGridLayout::addLayout(pQVar3,pQVar15,1,0,local_res18);
    QButtonGroup::addButton
              (*(QButtonGroup **)(param_1 + 0xe0),*(QAbstractButton **)(param_1 + 0xf0));
    uVar4 = FUN_140036330(param_1,"RTT-Plus (synchronus)",0,param_1,0);
    *(undefined8 *)(param_1 + 0xf8) = uVar4;
    uVar4 = FUN_140035ff0(param_1,
                          "<a href=\"https://kb.segger.com/UM08028_J-Scope#RTT_Plus_(Synchronous)\">?</a>"
                         );
    FUN_140036e00(uVar4,1,1);
    pQVar10 = (QHBoxLayout *)FUN_140058d94(0x20);
    pQVar15 = (QHBoxLayout *)this_02;
    local_48[0] = (QButtonGroup *)pQVar10;
    if (pQVar10 != (QHBoxLayout *)0x0) {
      QHBoxLayout::QHBoxLayout(pQVar10);
      *(undefined ***)pQVar10 = QHBoxLayout::vftable;
      *(undefined ***)(pQVar10 + 0x10) = QHBoxLayout::vftable;
      pQVar15 = pQVar10;
    }
    local_res18[0] = 0;
    QBoxLayout::addWidget((QBoxLayout *)pQVar15,*(undefined8 *)(param_1 + 0xf8),0,local_res18);
    local_res18[0] = 0;
    QBoxLayout::addWidget((QBoxLayout *)pQVar15,uVar4,0,local_res18);
    local_res18[0] = 1;
    QGridLayout::addLayout(pQVar3,pQVar15,2,0,local_res18);
    QButtonGroup::addButton
              (*(QButtonGroup **)(param_1 + 0xe0),*(QAbstractButton **)(param_1 + 0xf8));
    pQVar3 = (QGridLayout *)FUN_140035e90(0);
    uVar4 = FUN_140035f30(param_1,"Sampling Rate",pQVar3);
    *(undefined8 *)(param_1 + 0x50) = uVar4;
    uVar4 = FUN_140035ff0(param_1,"Sample every ");
    uVar11 = FUN_140035ff0(param_1,"us, sampling rate: 10 kHz");
    *(undefined8 *)(param_1 + 0x108) = uVar11;
    uVar11 = FUN_140035ff0(param_1,
                           "Set 0 to sample as fast as possible. <a href=\"https://kb.segger.com/UM08028_J-Scope\">?</a>"
                          );
    FUN_140036e00(uVar11,1,1);
    uVar12 = FUN_1400360a0(param_1,&DAT_14006cb6c,0);
    *(undefined8 *)(param_1 + 0x100) = uVar12;
    this_00 = (QRegExpValidator *)FUN_140058d94(0x18);
    local_48[0] = (QButtonGroup *)this_00;
    if (this_00 != (QRegExpValidator *)0x0) {
      QRegExpValidator::QRegExpValidator(this_00,(QObject *)param_1);
      *(undefined ***)this_00 = QRegExpValidator::vftable;
      this_02 = this_00;
    }
    QRegExpValidator::setRegExp(this_02,local_res20);
    QLineEdit::setValidator(*(QLineEdit **)(param_1 + 0x100),(QValidator *)this_02);
    pQVar6 = (QSize *)(**(code **)(**(longlong **)(param_1 + 0x100) + 0x60))
                                (*(longlong **)(param_1 + 0x100),local_48);
    QWidget::resize(*(QWidget **)(param_1 + 0x100),pQVar6);
    QObject::connect(*(QObject **)(param_1 + 0x100),"2textChanged(const QString&)",
                     (QObject *)param_1,"1_SlotOnTextChangedLineSampRate(const QString&)",0);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,uVar4,0,0,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x100),0,1,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x108),0,2,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,uVar11,1,0,1,3,local_res18);
    pQVar3 = (QGridLayout *)FUN_140035e90(0);
    uVar4 = FUN_140035f30(param_1,"ELF File",pQVar3);
    *(undefined8 *)(param_1 + 0x58) = uVar4;
    uVar4 = FUN_1400360a0(param_1,&DAT_14005f306,0);
    *(undefined8 *)(param_1 + 0x110) = uVar4;
    plVar7 = (longlong *)
             FUN_140035c20(param_1,&DAT_140060544,param_1,"1_SlotOnReleaseBtnElfFile(void)");
    *(longlong **)(param_1 + 0x118) = plVar7;
    lVar8 = (**(code **)(*plVar7 + 0x60))(plVar7,local_48);
    iVar1 = *(int *)(lVar8 + 4);
    lVar8 = (**(code **)(**(longlong **)(param_1 + 0x118) + 0x60))
                      (*(longlong **)(param_1 + 0x118),&local_50);
    QWidget::setFixedSize(*(QWidget **)(param_1 + 0x118),*(int *)(lVar8 + 4),iVar1);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x110),0,0,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x118),0,1,local_res18);
    pQVar3 = (QGridLayout *)FUN_140035e90(0);
    uVar4 = FUN_140035f30(param_1,0,pQVar3);
    *(undefined8 *)(param_1 + 0x60) = uVar4;
    uVar4 = FUN_140035c20(param_1,&DAT_14006cc0c,param_1,"1_SlotOnReleaseBtnOK(void)");
    *(undefined8 *)(param_1 + 0x120) = uVar4;
    uVar4 = FUN_140035c20(param_1,"Cancel",param_1,"1_SlotOnReleaseBtnCancel(void)");
    *(undefined8 *)(param_1 + 0x128) = uVar4;
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x120),0,0,local_res18);
    local_res18[0] = 0;
    QGridLayout::addWidget(pQVar3,*(undefined8 *)(param_1 + 0x128),0,1,local_res18);
    QLayout::addWidget(*(QLayout **)(param_1 + 0x130),*(QWidget **)(param_1 + 0x28));
    QLayout::addWidget(*(QLayout **)(param_1 + 0x130),*(QWidget **)(param_1 + 0x30));
    QLayout::addWidget(*(QLayout **)(param_1 + 0x130),*(QWidget **)(param_1 + 0x38));
    QLayout::addWidget(*(QLayout **)(param_1 + 0x130),*(QWidget **)(param_1 + 0x40));
    QLayout::addWidget(*(QLayout **)(param_1 + 0x130),*(QWidget **)(param_1 + 0x48));
    QLayout::addWidget(*(QLayout **)(param_1 + 0x130),*(QWidget **)(param_1 + 0x50));
    QLayout::addWidget(*(QLayout **)(param_1 + 0x130),*(QWidget **)(param_1 + 0x58));
    QLayout::addWidget(*(QLayout **)(param_1 + 0x130),*(QWidget **)(param_1 + 0x60));
    iVar1 = 4000;
    if (*(int *)(*(longlong *)(param_1 + 0x138) + 0x5a8) != 0) {
      iVar1 = *(int *)(*(longlong *)(param_1 + 0x138) + 0x5a8);
    }
    FUN_140037670(*(undefined8 *)(param_1 + 0xd0),iVar1);
    iVar1 = FUN_14002c650(&PTR_DAT_140096af0,4,
                          *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x5a4));
    QComboBox::setCurrentIndex(*(QComboBox **)(param_1 + 200),iVar1);
    lVar8 = *(longlong *)(param_1 + 0x138);
    if (*(char *)(lVar8 + 8) != '\0') {
      local_50 = QString::fromAscii_helper((char *)(lVar8 + 8),-1);
      QLineEdit::setText(*(QLineEdit **)(param_1 + 0xa8),(QString *)&local_50);
      QString::~QString((QString *)&local_50);
      lVar8 = *(longlong *)(param_1 + 0x138);
    }
    if (*(char *)(lVar8 + 0x88) != '\0') {
      local_50 = QString::fromAscii_helper((char *)(lVar8 + 0x88),-1);
      QLineEdit::setText(*(QLineEdit **)(param_1 + 0x110),(QString *)&local_50);
      QString::~QString((QString *)&local_50);
      lVar8 = *(longlong *)(param_1 + 0x138);
    }
    if (*(char *)(lVar8 + 0x498) != '\0') {
      local_50 = QString::fromAscii_helper((char *)(lVar8 + 0x498),-1);
      QLineEdit::setText(*(QLineEdit **)(param_1 + 0x98),(QString *)&local_50);
      QString::~QString((QString *)&local_50);
      lVar8 = *(longlong *)(param_1 + 0x138);
    }
    if (*(char *)(lVar8 + 0x394) != '\0') {
      local_50 = QString::fromAscii_helper((char *)(lVar8 + 0x394),-1);
      QLineEdit::setText(*(QLineEdit **)(param_1 + 0xb8),(QString *)&local_50);
      QString::~QString((QString *)&local_50);
      lVar8 = *(longlong *)(param_1 + 0x138);
    }
    pQVar14 = *(QAbstractButton **)(param_1 + 0x70);
    if (*(int *)(lVar8 + 0x59c) == 0) {
      if (*(char *)(lVar8 + 0x518) == '\0') {
        QAbstractButton::setChecked(pQVar14,false);
        local_50 = QString::fromAscii_helper("",-1);
        QLineEdit::setText(*(QLineEdit **)(param_1 + 0x78),(QString *)&local_50);
      }
      else {
        QAbstractButton::setChecked(pQVar14,true);
        local_50 = QString::fromAscii_helper((char *)(*(longlong *)(param_1 + 0x138) + 0x518),-1);
        QLineEdit::setText(*(QLineEdit **)(param_1 + 0x78),(QString *)&local_50);
      }
    }
    else {
      QAbstractButton::setChecked(pQVar14,true);
      pQVar13 = (QString *)
                QString::number((int)&local_50,*(int *)(*(longlong *)(param_1 + 0x138) + 0x59c));
      QLineEdit::setText(*(QLineEdit **)(param_1 + 0x78),pQVar13);
    }
    QString::~QString((QString *)&local_50);
    if (*(char *)(*(longlong *)(param_1 + 0x138) + 0x5ac) == '\0') {
      pQVar14 = *(QAbstractButton **)(param_1 + 0xe8);
    }
    else if (*(char *)(*(longlong *)(param_1 + 0x138) + 0x5ae) == '\0') {
      pQVar14 = *(QAbstractButton **)(param_1 + 0xf0);
    }
    else {
      pQVar14 = *(QAbstractButton **)(param_1 + 0xf8);
    }
    QAbstractButton::setChecked(pQVar14,true);
    lVar8 = *(longlong *)(param_1 + 0x138);
    if (*(int *)(lVar8 + 0x5a8) != 0) {
      FUN_140037670(*(undefined8 *)(param_1 + 0xd0));
      lVar8 = *(longlong *)(param_1 + 0x138);
    }
    if (*(int *)(lVar8 + 0x5b0) != 0) {
      pQVar13 = (QString *)QString::number((uint)&local_50,*(int *)(lVar8 + 0x5b0));
      QLineEdit::setText(*(QLineEdit **)(param_1 + 0x100),pQVar13);
      QString::~QString((QString *)&local_50);
      lVar8 = *(longlong *)(param_1 + 0x138);
    }
    if ((*(int *)(lVar8 + 0x5b4) == 1) || (*(int *)(lVar8 + 0x5b4) != 2)) {
      pQVar14 = *(QAbstractButton **)(param_1 + 0x68);
    }
    else {
      pQVar14 = *(QAbstractButton **)(param_1 + 0x90);
    }
    QAbstractButton::setChecked(pQVar14,true);
    QWidget::show(param_1);
  }
  QRegExp::~QRegExp(local_res20);
  return param_1;
}


/* ================= FUN_14002b610 @ 14002b610  size=57  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002b670 */
/* CALLEES: FUN_140043e60 */

void FUN_14002b610(QDialog *param_1)

{
  *(undefined ***)param_1 = NewProjectDialog::vftable;
  *(undefined ***)(param_1 + 0x10) = NewProjectDialog::vftable;
  FUN_140043e60(*(undefined8 *)(param_1 + 0x138));
                    /* WARNING: Could not recover jumptable at 0x00014002b642. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::~QDialog(param_1);
  return;
}


/* ================= FUN_14002b64c @ 14002b64c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002b670 */

void FUN_14002b64c(longlong param_1)

{
  FUN_14002b670(param_1 + -0x10);
  return;
}


/* ================= FUN_14002b658 @ 14002b658  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002b6f0 */

void FUN_14002b658(longlong param_1)

{
  FUN_14002b6f0(param_1 + -0x10);
  return;
}


/* ================= FUN_14002b664 @ 14002b664  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002b770 */

void FUN_14002b664(longlong param_1)

{
  FUN_14002b770(param_1 + -0x10);
  return;
}


/* ================= FUN_14002b670 @ 14002b670  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002b64c */
/* CALLEES: FUN_14002b610, free */

void * FUN_14002b670(void *param_1,ulonglong param_2)

{
  FUN_14002b610();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002b6b0 @ 14002b6b0  size=53  callers=0 callees=2 ================= */
/* CALLEES: free, ~QButtonGroup */

QButtonGroup * FUN_14002b6b0(QButtonGroup *param_1,uint param_2)

{
  QButtonGroup::~QButtonGroup(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002b6f0 @ 14002b6f0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002b658 */
/* CALLEES: free, ~QHBoxLayout */

QHBoxLayout * FUN_14002b6f0(QHBoxLayout *param_1,uint param_2)

{
  QHBoxLayout::~QHBoxLayout(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002b730 @ 14002b730  size=53  callers=0 callees=2 ================= */
/* CALLEES: free, ~QRegExpValidator */

QRegExpValidator * FUN_14002b730(QRegExpValidator *param_1,uint param_2)

{
  QRegExpValidator::~QRegExpValidator(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002b770 @ 14002b770  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002b664 */
/* CALLEES: free, ~QWidget */

QWidget * FUN_14002b770(QWidget *param_1,uint param_2)

{
  QWidget::~QWidget(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002b7b0 @ 14002b7b0  size=17  callers=2 callees=0 ================= */
/* CALLERS: FUN_14002b8d0, FUN_14002c250 */

undefined4 FUN_14002b7b0(longlong param_1,uint param_2,uint param_3)

{
  ulonglong uVar1;
  
  uVar1 = 0;
  if (param_3 < param_2) {
    uVar1 = (ulonglong)param_3;
  }
  return *(undefined4 *)(param_1 + 8 + uVar1 * 0x10);
}


/* ================= FUN_14002b7d0 @ 14002b7d0  size=242  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: hide, id, show */

void FUN_14002b7d0(longlong param_1,int param_2)

{
  int iVar1;
  
  iVar1 = QButtonGroup::id(*(QButtonGroup **)(param_1 + 0xe0),*(QAbstractButton **)(param_1 + 0xe8))
  ;
  if (param_2 == iVar1) {
    *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ac) = 0;
    *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ae) = 0;
    QWidget::show(*(QWidget **)(param_1 + 0x58));
    QWidget::show(*(QWidget **)(param_1 + 0x50));
  }
  iVar1 = QButtonGroup::id(*(QButtonGroup **)(param_1 + 0xe0),*(QAbstractButton **)(param_1 + 0xf0))
  ;
  if (param_2 == iVar1) {
    *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ac) = 1;
    *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ae) = 0;
    QWidget::hide(*(QWidget **)(param_1 + 0x58));
    QWidget::hide(*(QWidget **)(param_1 + 0x50));
  }
  iVar1 = QButtonGroup::id(*(QButtonGroup **)(param_1 + 0xe0),*(QAbstractButton **)(param_1 + 0xf8))
  ;
  if (param_2 == iVar1) {
    *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ac) = 1;
    *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ae) = 1;
    QWidget::show(*(QWidget **)(param_1 + 0x58));
    QWidget::hide(*(QWidget **)(param_1 + 0x50));
  }
  return;
}


/* ================= FUN_14002b8d0 @ 14002b8d0  size=45  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: FUN_14002b7b0 */

void FUN_14002b8d0(longlong param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  uVar1 = FUN_14002b7b0(&PTR_DAT_140096af0,4,param_2);
  *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x5a4) = uVar1;
  return;
}


/* ================= FUN_14002b900 @ 14002b900  size=40  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: FUN_1400375c0 */

void FUN_14002b900(longlong param_1)

{
  undefined4 uVar1;
  
  uVar1 = FUN_1400375c0(*(undefined8 *)(param_1 + 0xd0));
  *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x5a8) = uVar1;
  return;
}


/* ================= FUN_14002b930 @ 14002b930  size=10  callers=2 callees=0 ================= */
/* CALLERS: FUN_14003c020, FUN_14003c240 */

void FUN_14002b930(longlong *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014002b933. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_1 + 0x1a0))();
  return;
}


/* ================= FUN_14002b940 @ 14002b940  size=263  callers=1 callees=9 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: FUN_140027140, FUN_140036720, FUN_14004f890, FUN_14004fa30, __security_check_cookie, fromAscii_helper, memset, setText, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14002b940(longlong param_1)

{
  undefined1 auStack_288 [32];
  undefined1 *local_268;
  char *local_260;
  undefined8 local_258;
  Data *local_248 [2];
  char local_238 [272];
  undefined1 local_128 [272];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_288;
  memset(local_238,0,0x104);
  memset(local_128,0,0x104);
  FUN_14004f890(local_128,0x104);
  local_258 = 0;
  local_260 = "Output files (*.elf *.out *.obj *.axf);;All files (*.*)";
  local_268 = local_128;
  FUN_140036720(param_1,local_238,0x104,"Open output file");
  FUN_14004fa30(local_238);
  local_248[0] = QString::fromAscii_helper(local_238,-1);
  QLineEdit::setText(*(QLineEdit **)(param_1 + 0x110),(QString *)local_248);
  QString::~QString((QString *)local_248);
  FUN_140027140(*(longlong *)(param_1 + 0x138) + 0x88,local_238,0x104);
  return;
}


/* ================= FUN_14002ba50 @ 14002ba50  size=220  callers=1 callees=8 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: FUN_14002c250, FUN_14003e110, FUN_14003e380, FUN_14003e800, FUN_14003f640, isChecked, text, ~QString */

void FUN_14002ba50(longlong *param_1)

{
  int iVar1;
  bool bVar2;
  longlong *plVar3;
  QString local_res8 [8];
  
  bVar2 = QAbstractButton::isChecked((QAbstractButton *)param_1[0x1e]);
  if (bVar2) {
    plVar3 = (longlong *)QLineEdit::text((QLineEdit *)param_1[0x15]);
    iVar1 = *(int *)(*plVar3 + 8);
    QString::~QString(local_res8);
    if (iVar1 == 0) {
      FUN_14003f640("In RTT mode specifying the target device is mandatory.");
      return;
    }
  }
  else {
    plVar3 = (longlong *)QLineEdit::text((QLineEdit *)param_1[0x22]);
    iVar1 = *(int *)(*plVar3 + 8);
    QString::~QString(local_res8);
    if (iVar1 == 0) {
      FUN_14003f640("No ELF file selected.");
      return;
    }
  }
  FUN_14002c250(param_1);
  if ((char)param_1[0x28] == '\0') {
    FUN_14003e110(0);
    FUN_14003e380(param_1[0x27]);
  }
  else {
    FUN_14003e800(param_1[0x27]);
  }
                    /* WARNING: Could not recover jumptable at 0x00014002bb25. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_1 + 0x198))(param_1);
  return;
}


/* ================= FUN_14002bb30 @ 14002bb30  size=245  callers=1 callees=9 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: FUN_140027140, FUN_140036720, FUN_14004f820, FUN_14004f9b0, __security_check_cookie, fromAscii_helper, memset, setText, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14002bb30(longlong param_1)

{
  undefined1 auStack_288 [32];
  undefined1 *local_268;
  char *local_260;
  undefined8 local_258;
  Data *local_248 [2];
  char local_238 [272];
  undefined1 local_128 [272];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_288;
  memset(local_128,0,0x104);
  FUN_14004f820(local_128,0x104);
  local_258 = 0;
  local_260 = "J-Link script files (*.JLinkScript *.pex *.pc);;All files (*.*)";
  local_268 = local_128;
  FUN_140036720(param_1,local_238,0x104,"Open script file");
  FUN_14004f9b0(local_238);
  local_248[0] = QString::fromAscii_helper(local_238,-1);
  QLineEdit::setText(*(QLineEdit **)(param_1 + 0xb8),(QString *)local_248);
  QString::~QString((QString *)local_248);
  FUN_140027140(*(longlong *)(param_1 + 0x138) + 0x394,local_238,0x104);
  return;
}


/* ================= FUN_14002bc30 @ 14002bc30  size=600  callers=1 callees=18 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: FUN_140027140, FUN_14002d5c0, FUN_14002e950, FUN_14002f8f0, FUN_140043d20, FUN_140043e60, FUN_140058d94, JLINKARM_DEVICE_GetInfo, QString, __security_check_cookie, exec, fromAscii_helper, memset, operator=, setText, toAscii, ~QByteArray, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14002bc30(longlong param_1)

{
  uint uVar1;
  void *_Dst;
  longlong lVar2;
  QDialog *this;
  char *pcVar3;
  longlong *plVar4;
  uint uVar5;
  QDialog *pQVar6;
  void *pvVar7;
  undefined1 auStack_a38 [32];
  Data **local_a18;
  Data *local_a08;
  undefined4 local_a00;
  char local_9f8;
  undefined4 local_9f4;
  undefined4 local_9f0;
  undefined4 local_9ec;
  longlong local_9e8;
  longlong local_9e0;
  undefined1 local_9d8 [16];
  QString local_9c8 [16];
  undefined4 local_9b8;
  undefined1 local_9b4 [2428];
  ulonglong local_38;
  
  local_38 = DAT_140096fe8 ^ (ulonglong)auStack_a38;
  this = (QDialog *)0x0;
  local_a00 = 0;
  QString::QString((QString *)&local_9e8);
  uVar1 = JLINKARM_DEVICE_GetInfo(0xffffffff);
  _Dst = (void *)FUN_140043d20(uVar1 * 0x218);
  memset(_Dst,0,(ulonglong)(uVar1 * 0x218));
  memset(local_9b4,0,0x974);
  local_9b8 = 0x978;
  pQVar6 = this;
  pvVar7 = _Dst;
  if (uVar1 != 0) {
    do {
      JLINKARM_DEVICE_GetInfo(pQVar6,&local_9b8);
      FUN_14002f8f0(pvVar7,&local_9b8);
      uVar5 = (int)pQVar6 + 1;
      pQVar6 = (QDialog *)(ulonglong)uVar5;
      pvVar7 = (void *)((longlong)pvVar7 + 0x218);
    } while (uVar5 < uVar1);
  }
  lVar2 = FUN_140058d94(0xe0);
  local_9e0 = lVar2;
  if (lVar2 != 0) {
    local_a08 = QString::fromAscii_helper((char *)(*(longlong *)(param_1 + 0x138) + 8),-1);
    local_a00 = 1;
    local_a18 = &local_a08;
    this = (QDialog *)FUN_14002d5c0(lVar2,param_1,_Dst,uVar1);
    QString::~QString((QString *)&local_a08);
  }
  QDialog::exec(this);
  pcVar3 = (char *)FUN_14002e950(this,local_9d8);
  local_9f8 = *pcVar3;
  local_9f4 = *(undefined4 *)(pcVar3 + 4);
  local_9f0 = *(undefined4 *)(pcVar3 + 8);
  local_9ec = *(undefined4 *)(pcVar3 + 0xc);
  QString::operator=((QString *)&local_9e8,(QString *)(pcVar3 + 0x10));
  QString::~QString(local_9c8);
  if (this != (QDialog *)0x0) {
    (**(code **)(*(longlong *)this + 0x18))(this,1);
  }
  FUN_140043e60(_Dst);
  if ((local_9f8 != '\0') && (*(int *)(local_9e8 + 8) != 0)) {
    plVar4 = (longlong *)QString::toAscii((QString *)&local_9e8);
    FUN_140027140(*(longlong *)(param_1 + 0x138) + 8,*(undefined8 *)(*plVar4 + 0x10),0x80);
    QByteArray::~QByteArray((QByteArray *)&local_a08);
    local_a08 = QString::fromAscii_helper((char *)(*(longlong *)(param_1 + 0x138) + 8),-1);
    QLineEdit::setText(*(QLineEdit **)(param_1 + 0xa8),(QString *)&local_a08);
    QString::~QString((QString *)&local_a08);
  }
  QString::~QString((QString *)&local_9e8);
  return;
}


/* ================= FUN_14002be90 @ 14002be90  size=414  callers=1 callees=9 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: FUN_140027700, FUN_140028380, FUN_140036ec0, QString, __security_check_cookie, fromAscii_helper, setText, strtod, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14002be90(longlong param_1,QString *param_2)

{
  double dVar1;
  undefined8 uVar2;
  undefined *puVar3;
  int iVar4;
  undefined1 auStack_108 [32];
  Data *local_e8 [2];
  char local_d8 [64];
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
  undefined8 local_38;
  undefined8 uStack_30;
  undefined8 local_28;
  undefined8 uStack_20;
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_108;
  local_d8[0] = '\0';
  local_d8[1] = '\0';
  local_d8[2] = '\0';
  local_d8[3] = '\0';
  local_d8[4] = '\0';
  local_d8[5] = '\0';
  local_d8[6] = '\0';
  local_d8[7] = '\0';
  local_d8[8] = '\0';
  local_d8[9] = '\0';
  local_d8[10] = '\0';
  local_d8[0xb] = '\0';
  local_d8[0xc] = '\0';
  local_d8[0xd] = '\0';
  local_d8[0xe] = '\0';
  local_d8[0xf] = '\0';
  local_d8[0x10] = '\0';
  local_d8[0x11] = '\0';
  local_d8[0x12] = '\0';
  local_d8[0x13] = '\0';
  local_d8[0x14] = '\0';
  local_d8[0x15] = '\0';
  local_d8[0x16] = '\0';
  local_d8[0x17] = '\0';
  local_d8[0x18] = '\0';
  local_d8[0x19] = '\0';
  local_d8[0x1a] = '\0';
  local_d8[0x1b] = '\0';
  local_d8[0x1c] = '\0';
  local_d8[0x1d] = '\0';
  local_d8[0x1e] = '\0';
  local_d8[0x1f] = '\0';
  local_d8[0x20] = '\0';
  local_d8[0x21] = '\0';
  local_d8[0x22] = '\0';
  local_d8[0x23] = '\0';
  local_d8[0x24] = '\0';
  local_d8[0x25] = '\0';
  local_d8[0x26] = '\0';
  local_d8[0x27] = '\0';
  local_d8[0x28] = '\0';
  local_d8[0x29] = '\0';
  local_d8[0x2a] = '\0';
  local_d8[0x2b] = '\0';
  local_d8[0x2c] = '\0';
  local_d8[0x2d] = '\0';
  local_d8[0x2e] = '\0';
  local_d8[0x2f] = '\0';
  local_d8[0x30] = '\0';
  local_d8[0x31] = '\0';
  local_d8[0x32] = '\0';
  local_d8[0x33] = '\0';
  local_d8[0x34] = '\0';
  local_d8[0x35] = '\0';
  local_d8[0x36] = '\0';
  local_d8[0x37] = '\0';
  local_d8[0x38] = '\0';
  local_d8[0x39] = '\0';
  local_d8[0x3a] = '\0';
  local_d8[0x3b] = '\0';
  local_d8[0x3c] = '\0';
  local_d8[0x3d] = '\0';
  local_d8[0x3e] = '\0';
  local_d8[0x3f] = '\0';
  local_58 = 0;
  uStack_50 = 0;
  local_48 = 0;
  uStack_40 = 0;
  local_38 = 0;
  uStack_30 = 0;
  local_28 = 0;
  uStack_20 = 0;
  local_98 = 0;
  uStack_90 = 0;
  local_88 = 0;
  uStack_80 = 0;
  local_78 = 0;
  uStack_70 = 0;
  local_68 = 0;
  uStack_60 = 0;
  uVar2 = QString::QString((QString *)local_e8,param_2);
  FUN_140036ec0(&local_98,uVar2,0x40);
  dVar1 = strtod((char *)&local_98,(char **)0x0);
  *(int *)(*(longlong *)(param_1 + 0x138) + 0x5b0) = (int)(longlong)dVar1;
  if (dVar1 == 0.0) {
    iVar4 = 0;
  }
  else {
    iVar4 = (int)(1000000.0 / dVar1);
    if (999 < iVar4) {
      iVar4 = iVar4 / 1000;
      puVar3 = &DAT_14006c650;
      goto LAB_14002bf49;
    }
  }
  puVar3 = &DAT_14006c64c;
LAB_14002bf49:
  FUN_140028380(&local_58,0x40,&DAT_140060a50,iVar4);
  FUN_140027700(local_d8,"us, sampling rate: ",0x40);
  FUN_140027700(local_d8,&local_58,0x40);
  FUN_140027700(local_d8,puVar3,0x40);
  local_e8[0] = QString::fromAscii_helper(local_d8,-1);
  QLabel::setText(*(QLabel **)(param_1 + 0x108),(QString *)local_e8);
  QString::~QString((QString *)local_e8);
  return;
}


/* ================= FUN_14002c030 @ 14002c030  size=82  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: hide, show */

void FUN_14002c030(longlong param_1,int param_2)

{
  if (param_2 == 2) {
    QWidget::hide(*(QWidget **)(param_1 + 0x88));
                    /* WARNING: Could not recover jumptable at 0x00014002c054. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    QWidget::show(*(QWidget **)(param_1 + 0x78));
    return;
  }
  if (param_2 == 0) {
    QWidget::show(*(QWidget **)(param_1 + 0x88));
                    /* WARNING: Could not recover jumptable at 0x00014002c075. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    QWidget::hide(*(QWidget **)(param_1 + 0x78));
    return;
  }
  return;
}


/* ================= FUN_14002c090 @ 14002c090  size=184  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: hide, setChecked, setDisabled, show */

void FUN_14002c090(longlong param_1,char param_2)

{
  if (param_2 != '\0') {
    QWidget::hide(*(QWidget **)(param_1 + 0x30));
    QWidget::hide(*(QWidget **)(param_1 + 0x38));
    QWidget::hide(*(QWidget **)(param_1 + 0x40));
    QAbstractButton::setChecked(*(QAbstractButton **)(param_1 + 0xe8),true);
    QWidget::setDisabled(*(QWidget **)(param_1 + 0xf0),true);
    QWidget::setDisabled(*(QWidget **)(param_1 + 0xf8),true);
    *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5c0) = 1;
    return;
  }
  QWidget::show(*(QWidget **)(param_1 + 0x30));
  QWidget::show(*(QWidget **)(param_1 + 0x38));
  QWidget::show(*(QWidget **)(param_1 + 0x40));
  QWidget::setDisabled(*(QWidget **)(param_1 + 0xf0),false);
  QWidget::setDisabled(*(QWidget **)(param_1 + 0xf8),false);
  *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5c0) = 0;
  return;
}


/* ================= FUN_14002c150 @ 14002c150  size=86  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: show */

void FUN_14002c150(longlong param_1,char param_2)

{
  if (param_2 != '\0') {
    *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x5b4) = 2;
    QWidget::show(*(QWidget **)(param_1 + 0x98));
                    /* WARNING: Could not recover jumptable at 0x00014002c18c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    QWidget::setFocus(*(QWidget **)(param_1 + 0x98),7);
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x00014002c19f. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::hide(*(QWidget **)(param_1 + 0x98));
  return;
}


/* ================= FUN_14002c1b0 @ 14002c1b0  size=148  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c020 */
/* CALLEES: hide, isChecked, show */

void FUN_14002c1b0(longlong param_1,char param_2)

{
  bool bVar1;
  QWidget *this;
  
  if (param_2 == '\0') {
    QWidget::show(*(QWidget **)(param_1 + 0x80));
    QWidget::hide(*(QWidget **)(param_1 + 0x70));
    this = *(QWidget **)(param_1 + 0x78);
  }
  else {
    *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x5b4) = 1;
    QWidget::show(*(QWidget **)(param_1 + 0x70));
    QWidget::hide(*(QWidget **)(param_1 + 0x80));
    bVar1 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0x70));
    this = *(QWidget **)(param_1 + 0x78);
    if (bVar1) {
      QWidget::show(this);
                    /* WARNING: Could not recover jumptable at 0x00014002c209. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      QWidget::hide(*(QWidget **)(param_1 + 0x88));
      return;
    }
  }
  QWidget::hide(this);
                    /* WARNING: Could not recover jumptable at 0x00014002c23d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::show(*(QWidget **)(param_1 + 0x88));
  return;
}


/* ================= FUN_14002c250 @ 14002c250  size=1017  callers=1 callees=14 ================= */
/* CALLERS: FUN_14002ba50 */
/* CALLEES: FUN_140027140, FUN_140027290, FUN_1400272c0, FUN_14002b7b0, FUN_140036ec0, FUN_1400375c0, __security_check_cookie, currentIndex, isChecked, text, toAscii, toInt, ~QByteArray, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14002c250(longlong param_1)

{
  longlong lVar1;
  bool bVar2;
  int iVar3;
  undefined4 uVar4;
  QString *pQVar5;
  longlong *plVar6;
  undefined8 uVar7;
  bool bVar8;
  undefined1 auStack_68 [32];
  QString local_48 [8];
  QByteArray local_40 [8];
  undefined4 local_38 [2];
  undefined1 local_30 [32];
  ulonglong local_10;
  
  local_10 = DAT_140096fe8 ^ (ulonglong)auStack_68;
  bVar8 = false;
  local_38[0] = 0;
  pQVar5 = (QString *)QLineEdit::text(*(QLineEdit **)(param_1 + 0xa8));
  plVar6 = (longlong *)QString::toAscii(pQVar5);
  FUN_140027140(*(longlong *)(param_1 + 0x138) + 8,*(undefined8 *)(*plVar6 + 0x10),0x80);
  QByteArray::~QByteArray(local_40);
  QString::~QString(local_48);
  pQVar5 = (QString *)QLineEdit::text(*(QLineEdit **)(param_1 + 0x110));
  plVar6 = (longlong *)QString::toAscii(pQVar5);
  FUN_140027140(*(longlong *)(param_1 + 0x138) + 0x88,*(undefined8 *)(*plVar6 + 0x10),0x104);
  QByteArray::~QByteArray((QByteArray *)local_48);
  QString::~QString((QString *)local_40);
  pQVar5 = (QString *)QLineEdit::text(*(QLineEdit **)(param_1 + 0xb8));
  plVar6 = (longlong *)QString::toAscii(pQVar5);
  FUN_140027140(*(longlong *)(param_1 + 0x138) + 0x394,*(undefined8 *)(*plVar6 + 0x10),0x104);
  QByteArray::~QByteArray((QByteArray *)local_48);
  QString::~QString((QString *)local_40);
  pQVar5 = (QString *)QLineEdit::text(*(QLineEdit **)(param_1 + 0x98));
  plVar6 = (longlong *)QString::toAscii(pQVar5);
  FUN_140027140(*(longlong *)(param_1 + 0x138) + 0x498,*(undefined8 *)(*plVar6 + 0x10),0x80);
  QByteArray::~QByteArray((QByteArray *)local_48);
  QString::~QString((QString *)local_40);
  bVar2 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0x70));
  if (bVar2) {
    plVar6 = (longlong *)QLineEdit::text(*(QLineEdit **)(param_1 + 0x78));
    bVar8 = true;
    if (*(int *)(*plVar6 + 8) == 0) goto LAB_14002c3fa;
    bVar2 = true;
  }
  else {
LAB_14002c3fa:
    bVar2 = false;
  }
  if (bVar8) {
    QString::~QString(local_48);
  }
  if (bVar2) {
    uVar7 = QLineEdit::text(*(QLineEdit **)(param_1 + 0x78));
    FUN_140036ec0(local_30,uVar7,0x20);
    iVar3 = FUN_140027290(local_30);
    if (iVar3 != 0) {
      FUN_1400272c0(local_30,local_38);
      *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x59c) = local_38[0];
      goto LAB_14002c49f;
    }
    FUN_140027140(*(longlong *)(param_1 + 0x138) + 0x518,local_30,0x80);
    *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x59c) = 0;
  }
  else {
    *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x59c) = 0;
LAB_14002c49f:
    lVar1 = *(longlong *)(param_1 + 0x138);
    *(undefined8 *)(lVar1 + 0x518) = 0;
    *(undefined8 *)(lVar1 + 0x520) = 0;
    *(undefined8 *)(lVar1 + 0x528) = 0;
    *(undefined8 *)(lVar1 + 0x530) = 0;
    *(undefined8 *)(lVar1 + 0x538) = 0;
    *(undefined8 *)(lVar1 + 0x540) = 0;
    *(undefined8 *)(lVar1 + 0x548) = 0;
    *(undefined8 *)(lVar1 + 0x550) = 0;
    *(undefined8 *)(lVar1 + 0x558) = 0;
    *(undefined8 *)(lVar1 + 0x560) = 0;
    *(undefined8 *)(lVar1 + 0x568) = 0;
    *(undefined8 *)(lVar1 + 0x570) = 0;
    *(undefined8 *)(lVar1 + 0x578) = 0;
    *(undefined8 *)(lVar1 + 0x580) = 0;
    *(undefined8 *)(lVar1 + 0x588) = 0;
    *(undefined8 *)(lVar1 + 0x590) = 0;
  }
  *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5c0) = 0;
  bVar2 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0x90));
  if (bVar2) {
    *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x5b4) = 2;
  }
  else {
    bVar2 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0xa0));
    if (bVar2) {
      *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5c0) = 1;
    }
    else {
      *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x5b4) = 1;
    }
  }
  bVar2 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0xf0));
  if (bVar2) {
    *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ac) = 1;
  }
  else {
    bVar2 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0xf8));
    if (bVar2) {
      *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ac) = 1;
      *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ae) = 1;
      goto LAB_14002c5a8;
    }
    bVar2 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0xe8));
    if (!bVar2) goto LAB_14002c5a8;
    *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ac) = 0;
  }
  *(undefined1 *)(*(longlong *)(param_1 + 0x138) + 0x5ae) = 0;
LAB_14002c5a8:
  iVar3 = QComboBox::currentIndex(*(QComboBox **)(param_1 + 200));
  uVar4 = FUN_14002b7b0(&PTR_DAT_140096af0,4,iVar3);
  *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x5a4) = uVar4;
  uVar4 = FUN_1400375c0(*(undefined8 *)(param_1 + 0xd0));
  *(undefined4 *)(*(longlong *)(param_1 + 0x138) + 0x5a8) = uVar4;
  pQVar5 = (QString *)QLineEdit::text(*(QLineEdit **)(param_1 + 0x100));
  iVar3 = QString::toInt(pQVar5,(bool *)0x0,10);
  *(int *)(*(longlong *)(param_1 + 0x138) + 0x5b0) = iVar3;
  QString::~QString(local_48);
  return;
}


/* ================= FUN_14002c650 @ 14002c650  size=34  callers=1 callees=0 ================= */
/* CALLERS: FUN_14002a410 */

uint FUN_14002c650(longlong param_1,uint param_2,int param_3)

{
  uint uVar1;
  int *piVar2;
  
  uVar1 = 0;
  if (param_2 != 0) {
    piVar2 = (int *)(param_1 + 8);
    do {
      if (*piVar2 == param_3) {
        return uVar1;
      }
      uVar1 = uVar1 + 1;
      piVar2 = piVar2 + 4;
    } while (uVar1 < param_2);
  }
  return 0;
}


/* ================= FUN_14002c680 @ 14002c680  size=65  callers=2 callees=0 ================= */
/* CALLERS: FUN_14002c6d0, FUN_14002c9b0 */

void FUN_14002c680(longlong *param_1,longlong param_2,undefined1 param_3)

{
  int iVar1;
  int iVar2;
  
  iVar1 = *(int *)((longlong)param_1 + 0xc);
  iVar2 = iVar1 + 1;
  if (iVar2 < (int)param_1[1]) {
    *(undefined1 *)((longlong)iVar1 + *param_1) = param_3;
    *(int *)((longlong)param_1 + 0xc) = iVar2;
    if (((param_2 != 0) && (*(code **)(param_2 + 0x10) != (code *)0x0)) &&
       (iVar1 + 2 == (int)param_1[1])) {
                    /* WARNING: Could not recover jumptable at 0x00014002c6b9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)(param_2 + 0x10))(param_2);
      return;
    }
  }
  else {
    *(int *)((longlong)param_1 + 0xc) = iVar2;
  }
  return;
}


/* ================= FUN_14002c6d0 @ 14002c6d0  size=450  callers=2 callees=1 ================= */
/* CALLERS: FUN_14002c8a0, FUN_14002c9b0 */
/* CALLEES: FUN_14002c680 */

int FUN_14002c6d0(undefined8 param_1,undefined8 param_2,uint param_3,uint param_4,byte param_5,
                 int param_6,int param_7)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  ulonglong uVar7;
  int iVar8;
  
  iVar1 = param_7;
  iVar8 = 0;
  uVar3 = (uint)(0xffffffff / (ulonglong)param_4);
  iVar6 = param_7;
  uVar4 = 1;
  if (1 < uVar3) {
    do {
      uVar2 = uVar4 * param_4;
      iVar6 = param_7;
      if (param_3 < uVar2) break;
      param_6 = param_6 + -1;
      param_7 = param_7 + -1;
      iVar6 = param_7;
      uVar4 = uVar2;
    } while (uVar2 < uVar3);
  }
  param_7._0_1_ = '\0';
  if ((param_5 & 0x20) == 0) {
    if ((param_5 & 2) == 0) {
      if ((param_5 & 4) == 0) goto LAB_14002c776;
      param_7._0_1_ = ' ';
    }
    else {
      param_7._0_1_ = '+';
    }
  }
  else {
    param_7._0_1_ = '-';
  }
  param_6 = param_6 + -1;
LAB_14002c776:
  iVar5 = param_6;
  if (((param_5 & 0x10) != 0) && (iVar1 == -1)) {
    iVar5 = -1;
    iVar6 = param_6;
  }
  if ((param_5 & 1) == 0) {
    for (; (1 < iVar5 && (iVar6 < iVar5)); iVar5 = iVar5 + -1) {
      FUN_14002c680(param_1,param_2,param_5 & 0x10 | 0x20);
      iVar8 = iVar8 + 1;
    }
  }
  if ((char)param_7 != '\0') {
    FUN_14002c680(param_1,param_2,(char)param_7);
    iVar8 = iVar8 + 1;
  }
  if (1 < iVar6) {
    uVar3 = iVar6 - 1;
    iVar5 = iVar5 - uVar3;
    uVar7 = (ulonglong)uVar3;
    iVar8 = iVar8 + uVar3;
    do {
      FUN_14002c680(param_1,param_2,0x30);
      uVar7 = uVar7 - 1;
    } while (uVar7 != 0);
  }
  do {
    uVar3 = param_3 / uVar4;
    param_3 = param_3 - uVar3 * uVar4;
    uVar4 = uVar4 / param_4;
    FUN_14002c680(param_1,param_2,"0123456789ABCDEF(null)"[(int)uVar3]);
    iVar8 = iVar8 + 1;
  } while (uVar4 != 0);
  if (((param_5 & 1) != 0) && (1 < iVar5)) {
    uVar7 = (ulonglong)(iVar5 - 1U);
    iVar8 = iVar8 + (iVar5 - 1U);
    do {
      FUN_14002c680(param_1,param_2,0x20);
      uVar7 = uVar7 - 1;
    } while (uVar7 != 0);
  }
  return iVar8;
}


/* ================= FUN_14002c8a0 @ 14002c8a0  size=75  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002c9b0 */
/* CALLEES: FUN_14002c6d0 */

void FUN_14002c8a0(undefined8 param_1,undefined8 param_2,int param_3,undefined8 param_4,byte param_5
                  ,undefined4 param_6,undefined4 param_7)

{
  if (param_3 < 0) {
    FUN_14002c6d0(param_1,param_2,-param_3,param_4,param_5 | 0x20,param_6,param_7);
    return;
  }
  FUN_14002c6d0();
  return;
}


/* ================= FUN_14002c8f0 @ 14002c8f0  size=49  callers=17 callees=1 ================= */
/* CALLERS: FUN_1400273a0, FUN_1400274d0, FUN_140027a00, FUN_14002ce80, FUN_140034fd0, FUN_1400351b0, FUN_1400352e0, FUN_1400377e0, FUN_140040ed0, FUN_1400427b0, FUN_140043d20, FUN_140043fd0, FUN_140044520, FUN_140044840, FUN_140044920, FUN_140048680, FUN_140056f60 */
/* CALLEES: FUN_14002c9b0 */

void FUN_14002c8f0(undefined8 param_1,undefined4 param_2,undefined8 param_3,undefined8 param_4)

{
  undefined8 local_res20;
  undefined8 local_18;
  undefined4 local_10;
  undefined4 local_c;
  
  local_c = 0;
  local_res20 = param_4;
  local_18 = param_1;
  local_10 = param_2;
  FUN_14002c9b0(&local_18,0,param_3,&local_res20);
  return;
}


/* ================= FUN_14002c930 @ 14002c930  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400341e0 */
/* CALLEES: FUN_14002c9b0 */

void FUN_14002c930(undefined8 param_1,undefined4 param_2)

{
  undefined8 local_18;
  undefined4 local_10;
  undefined4 local_c;
  
  local_c = 0;
  local_18 = param_1;
  local_10 = param_2;
  FUN_14002c9b0(&local_18,0);
  return;
}


/* ================= FUN_14002c960 @ 14002c960  size=72  callers=1 callees=0 ================= */
/* CALLERS: FUN_14002c9b0 */

void FUN_14002c960(longlong *param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = (int)param_1[1] + -1;
  if (*(int *)((longlong)param_1 + 0xc) + 1 <= (int)param_1[1]) {
    iVar1 = *(int *)((longlong)param_1 + 0xc);
  }
  *(undefined1 *)((longlong)iVar1 + *param_1) = 0;
  if ((((param_2 != 0) && (*(code **)(param_2 + 0x10) != (code *)0x0)) && (iVar1 != 0)) &&
     (iVar1 + 1 < (int)param_1[1])) {
                    /* WARNING: Could not recover jumptable at 0x00014002c9a4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(param_2 + 0x10))(param_2);
    return;
  }
  return;
}


/* ================= FUN_14002c9b0 @ 14002c9b0  size=1101  callers=2 callees=4 ================= */
/* CALLERS: FUN_14002c8f0, FUN_14002c930 */
/* CALLEES: FUN_14002c680, FUN_14002c6d0, FUN_14002c8a0, FUN_14002c960 */

undefined4 FUN_14002c9b0(longlong param_1,longlong param_2,char *param_3,uint *param_4)

{
  undefined8 *puVar1;
  bool bVar2;
  bool bVar3;
  int iVar4;
  char cVar5;
  char *pcVar6;
  ulonglong uVar7;
  uint uVar8;
  char *pcVar9;
  uint uVar10;
  undefined8 uVar11;
  uint *puVar12;
  uint uVar13;
  ulonglong uVar14;
  uint *local_res8;
  
  local_res8 = param_4;
  if (((param_2 != 0) && (*(code **)(param_2 + 0x10) != (code *)0x0)) &&
     (*(int *)(param_1 + 0xc) + 1 == *(int *)(param_1 + 8))) {
    (**(code **)(param_2 + 0x10))(param_2);
  }
  cVar5 = *param_3;
  puVar12 = local_res8;
  do {
    if (cVar5 == '\0') {
      FUN_14002c960(param_1,param_2);
      return *(undefined4 *)(param_1 + 0xc);
    }
    param_3 = param_3 + 1;
    if (cVar5 != '%') {
      FUN_14002c680(param_1,param_2);
      puVar12 = local_res8;
      goto LAB_14002cd12;
    }
    bVar2 = false;
    do {
      while( true ) {
        pcVar9 = param_3;
        cVar5 = *pcVar9;
        pcVar6 = pcVar9 + 1;
        param_3 = pcVar6;
        if (cVar5 != '-') break;
        bVar2 = true;
      }
    } while (((cVar5 == '+') || (cVar5 == ' ')) || ((cVar5 == '#' || (cVar5 == '0'))));
    if (cVar5 == '*') {
      cVar5 = *pcVar6;
      local_res8 = puVar12 + 2;
      pcVar6 = pcVar9 + 2;
      uVar10 = *puVar12;
      bVar3 = true;
      if (-1 < (int)uVar10) {
        bVar3 = bVar2;
      }
      bVar2 = bVar3;
      uVar10 = (uVar10 ^ (int)uVar10 >> 0x1f) - ((int)uVar10 >> 0x1f);
      puVar12 = local_res8;
    }
    else if ((byte)(cVar5 - 0x30U) < 10) {
      uVar10 = 0;
      do {
        iVar4 = (int)cVar5;
        cVar5 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        uVar10 = iVar4 + (uVar10 * 5 + -0x18) * 2;
        if (cVar5 < '0') break;
      } while (cVar5 < ':');
    }
    else {
      uVar10 = 0xffffffff;
    }
    if (cVar5 == '.') {
      cVar5 = *pcVar6;
      uVar8 = 0;
      param_3 = pcVar6 + 1;
      if ((byte)(cVar5 - 0x30U) < 10) {
        do {
          iVar4 = (int)cVar5;
          cVar5 = *param_3;
          param_3 = param_3 + 1;
          uVar8 = iVar4 + (uVar8 * 5 + -0x18) * 2;
          if (cVar5 < '0') break;
        } while (cVar5 < ':');
      }
      else if (cVar5 == '*') {
        cVar5 = *param_3;
        local_res8 = puVar12 + 2;
        uVar8 = *puVar12;
        param_3 = pcVar6 + 2;
        puVar12 = local_res8;
      }
    }
    else {
      uVar8 = 0xffffffff;
      param_3 = pcVar6;
    }
    while ((cVar5 + 0x98U & 0xfb) == 0) {
      cVar5 = *param_3;
      param_3 = param_3 + 1;
    }
    switch(cVar5) {
    case '%':
      FUN_14002c680(param_1,param_2);
      puVar12 = local_res8;
      break;
    case 'X':
    case 'x':
      uVar11 = 0x10;
      goto LAB_14002cc1a;
    case '_':
      cVar5 = *param_3;
      param_3 = param_3 + 1;
    default:
      for (puVar1 = DAT_14085c8c0; puVar1 != (undefined8 *)0x0; puVar1 = (undefined8 *)*puVar1) {
        if (*(char *)(puVar1 + 2) == cVar5) {
          (*(code *)puVar1[1])(param_1,param_2,&PTR_FUN_14006cc40,&local_res8,bVar2,uVar10,uVar8);
          puVar12 = local_res8;
          break;
        }
      }
      break;
    case 'c':
      local_res8 = puVar12 + 2;
      FUN_14002c680(param_1,param_2);
      puVar12 = local_res8;
      break;
    case 'd':
      local_res8 = puVar12 + 2;
      FUN_14002c8a0(param_1,param_2,*puVar12,10,bVar2,uVar10,uVar8);
      puVar12 = local_res8;
      break;
    case 'o':
      uVar11 = 8;
      goto LAB_14002cc1a;
    case 'p':
      uVar8 = 8;
      uVar11 = 0x10;
      uVar10 = 0xffffffff;
      goto LAB_14002cc1a;
    case 's':
      local_res8 = puVar12 + 2;
      pcVar6 = "(null)";
      if (*(char **)puVar12 != (char *)0x0) {
        pcVar6 = *(char **)puVar12;
      }
      if ((int)uVar8 < 0) {
        cVar5 = *pcVar6;
        pcVar9 = pcVar6;
        while (cVar5 != '\0') {
          pcVar9 = pcVar9 + 1;
          cVar5 = *pcVar9;
        }
        uVar8 = (int)pcVar9 - (int)pcVar6;
      }
      uVar13 = uVar8;
      if (-1 < (int)uVar10) {
        uVar13 = uVar10;
      }
      uVar7 = 0;
      if (0 < (int)uVar8) {
        do {
          if (*pcVar6 == '\0') break;
          uVar10 = (int)uVar7 + 1;
          uVar7 = (ulonglong)uVar10;
          pcVar6 = pcVar6 + 1;
        } while ((int)uVar10 < (int)uVar8);
      }
      uVar10 = (uint)uVar7;
      if (bVar2) {
        uVar14 = uVar7;
        if (0 < (int)uVar10) {
          do {
            FUN_14002c680(param_1,param_2);
            uVar14 = uVar14 - 1;
          } while (uVar14 != 0);
        }
        while (puVar12 = local_res8, (int)uVar10 < (int)uVar13) {
          FUN_14002c680(param_1,param_2);
          uVar10 = (int)uVar7 + 1;
          uVar7 = (ulonglong)uVar10;
        }
      }
      else {
        if (0 < (int)(uVar13 - uVar10)) {
          uVar14 = (ulonglong)(uVar13 - uVar10);
          do {
            FUN_14002c680(param_1,param_2);
            uVar14 = uVar14 - 1;
          } while (uVar14 != 0);
        }
        puVar12 = local_res8;
        if (0 < (int)uVar10) {
          do {
            FUN_14002c680(param_1,param_2);
            uVar7 = uVar7 - 1;
            puVar12 = local_res8;
          } while (uVar7 != 0);
        }
      }
      break;
    case 'u':
      uVar11 = 10;
LAB_14002cc1a:
      local_res8 = puVar12 + 2;
      FUN_14002c6d0(param_1,param_2,*puVar12,uVar11,bVar2,uVar10,uVar8);
      puVar12 = local_res8;
    }
LAB_14002cd12:
    cVar5 = *param_3;
  } while( true );
}


/* ================= FUN_14002ce80 @ 14002ce80  size=1227  callers=1 callees=30 ================= */
/* CALLERS: FUN_14002a0c0 */
/* CALLEES: FUN_1400271b0, FUN_1400271c0, FUN_14002c8f0, FUN_1400359e0, FUN_140035cf0, FUN_140035e90, FUN_140035ff0, FUN_140036e00, FUN_140058d94, QDialog, QPixmap, QWidget, __security_check_cookie, addWidget, connect, fromAscii_helper, hide, loadFromData, minimumSize, setContentsMargins, setFixedSize, setLayout, setModal, setPixmap, setText, setWindowFlags, setWindowTitle, show, windowFlags, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

QWidget * FUN_14002ce80(QWidget *param_1,undefined8 param_2,undefined8 param_3)

{
  QLabel *this;
  undefined8 uVar1;
  QLabel *this_00;
  QLabel *this_01;
  QPixmap *this_02;
  QLayout *this_03;
  QLayout *pQVar2;
  QWidget *pQVar3;
  uint *puVar4;
  undefined4 *puVar5;
  QSize *pQVar6;
  QPixmap *pQVar7;
  QWidget *this_04;
  undefined1 auStackY_5d8 [32];
  undefined8 local_598;
  undefined4 local_590;
  undefined4 local_58c;
  QWidget *local_588;
  undefined1 local_578 [304];
  char local_448 [1024];
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStackY_5d8;
  this_04 = (QWidget *)0x0;
  local_58c = 0;
  local_588 = param_1;
  QDialog::QDialog((QDialog *)param_1,param_2,&local_58c);
  *(undefined ***)param_1 = AboutDialog::vftable;
  *(undefined ***)(param_1 + 0x10) = AboutDialog::vftable;
  *(undefined8 *)(param_1 + 0x28) = 0;
  QWidget::hide(param_1);
  QDialog::setModal((QDialog *)param_1,true);
  local_598 = (QWidget *)QString::fromAscii_helper("About",-1);
  QWidget::setWindowTitle(param_1,(QString *)&local_598);
  QString::~QString((QString *)&local_598);
  FUN_14002c8f0(local_578,300,"%s\nCompiled: %s %s\nhttps://www.segger.com",param_3);
  this = (QLabel *)FUN_140035ff0(param_1,0);
  FUN_140036e00(this,1,1);
  FUN_1400359e0(local_578,local_448,0x400);
  local_598 = (QWidget *)QString::fromAscii_helper(local_448,-1);
  QLabel::setText(this,(QString *)&local_598);
  QString::~QString((QString *)&local_598);
  FUN_1400271c0();
  uVar1 = FUN_1400271b0();
  FUN_14002c8f0(local_578,300,
                "Copyright (C) %s 1998 - %s\n\nThe GUI of this software uses Qt4 which is under LGPLv21\nAdd. 3rd party licensing and source code of Qt4:\nhttps://www.segger.com/qt-usage/"
                ,uVar1);
  this_00 = (QLabel *)FUN_140035ff0(param_1,0);
  FUN_140036e00(this_00,1,1);
  FUN_1400359e0(local_578,local_448,0x400);
  local_598 = (QWidget *)QString::fromAscii_helper(local_448,-1);
  QLabel::setText(this_00,(QString *)&local_598);
  QString::~QString((QString *)&local_598);
  this_01 = (QLabel *)FUN_140035ff0(param_1,0);
  this_02 = (QPixmap *)FUN_140058d94(0x18);
  pQVar7 = (QPixmap *)this_04;
  local_598 = (QWidget *)this_02;
  if (this_02 != (QPixmap *)0x0) {
    QPixmap::QPixmap(this_02);
    *(undefined ***)this_02 = QPixmap::vftable;
    pQVar7 = this_02;
  }
  local_590 = 0;
  QPixmap::loadFromData(pQVar7,&PNG_14006cca0,0x3a2,&DAT_14006d358);
  QLabel::setPixmap(this_01,pQVar7);
  uVar1 = FUN_140035cf0(param_1,&DAT_14006d378,param_1,"1_Slot_OnBtnOK(void)");
  *(undefined8 *)(param_1 + 0x28) = uVar1;
  QObject::connect((QObject *)param_1,"2_Sig_CloseDlg(void)",(QObject *)param_1,
                   "1_Slot_OnBtnOK(void)",2);
  this_03 = (QLayout *)FUN_140035e90(0);
  QLayout::setContentsMargins(this_03,0,0,0,0);
  local_590 = 0x20;
  QGridLayout::addWidget((QGridLayout *)this_03,this_01,0,0);
  local_590 = 1;
  QGridLayout::addWidget((QGridLayout *)this_03,this,0,1);
  pQVar2 = (QLayout *)FUN_140035e90(0);
  pQVar3 = (QWidget *)FUN_140058d94(0x28);
  local_598 = pQVar3;
  if (pQVar3 != (QWidget *)0x0) {
    local_590 = 0;
    QWidget::QWidget(pQVar3,param_1,&local_590);
    *(undefined ***)pQVar3 = QWidget::vftable;
    *(undefined ***)(pQVar3 + 0x10) = QWidget::vftable;
    this_04 = pQVar3;
  }
  QWidget::setLayout(this_04,this_03);
  local_598 = (QWidget *)((ulonglong)local_598 & 0xffffffff00000000);
  QGridLayout::addWidget((QGridLayout *)pQVar2,this_04,0,0);
  local_598._0_4_ = 1;
  QGridLayout::addWidget((QGridLayout *)pQVar2,this_00,1);
  local_598._0_4_ = 2;
  QGridLayout::addWidget((QGridLayout *)pQVar2,*(undefined8 *)(param_1 + 0x28),2);
  QWidget::setLayout(param_1,pQVar2);
  puVar4 = (uint *)QWidget::windowFlags(param_1);
  local_598._0_4_ = *puVar4 & 0xfffe3fff;
  QWidget::setWindowFlags(param_1,&local_598);
  puVar4 = (uint *)QWidget::windowFlags(param_1);
  *puVar4 = *puVar4 | 0x800;
  local_598._0_4_ = *puVar4;
  QWidget::setWindowFlags(param_1,&local_598);
  puVar5 = (undefined4 *)QWidget::windowFlags(param_1);
  local_598 = (QWidget *)(CONCAT44(local_598._4_4_,*puVar5) | 0x40000);
  QWidget::setWindowFlags(param_1,&local_598);
  pQVar6 = (QSize *)QWidget::minimumSize(param_1);
  QWidget::setFixedSize(param_1,pQVar6);
  QWidget::show(param_1);
  return param_1;
}


/* ================= FUN_14002d350 @ 14002d350  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_14002d380 */

void FUN_14002d350(QDialog *param_1)

{
  *(undefined ***)param_1 = AboutDialog::vftable;
  *(undefined ***)(param_1 + 0x10) = AboutDialog::vftable;
                    /* WARNING: Could not recover jumptable at 0x00014002d365. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::~QDialog(param_1);
  return;
}


/* ================= FUN_14002d36c @ 14002d36c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002d380 */

void FUN_14002d36c(longlong param_1)

{
  FUN_14002d380(param_1 + -0x10);
  return;
}


/* ================= FUN_14002d380 @ 14002d380  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002d36c */
/* CALLEES: FUN_14002d350, free */

void * FUN_14002d380(void *param_1,ulonglong param_2)

{
  FUN_14002d350();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002d3c0 @ 14002d3c0  size=53  callers=0 callees=2 ================= */
/* CALLEES: free, ~QPixmap */

QPixmap * FUN_14002d3c0(QPixmap *param_1,uint param_2)

{
  QPixmap::~QPixmap(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002d410 @ 14002d410  size=104  callers=0 callees=1 ================= */
/* CALLEES: QString */

longlong FUN_14002d410(longlong param_1,undefined8 param_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x10) = *param_3;
  *(undefined4 *)(param_1 + 0x14) = param_3[1];
  *(undefined4 *)(param_1 + 0x18) = param_3[2];
  *(undefined4 *)(param_1 + 0x1c) = param_3[3];
  *(undefined4 *)(param_1 + 0x20) = param_3[4];
  QString::QString((QString *)(param_1 + 0x28),(QString *)(param_3 + 6));
  *(undefined8 *)(param_1 + 0x30) = *(undefined8 *)(param_3 + 8);
  return param_1;
}


/* ================= FUN_14002d480 @ 14002d480  size=215  callers=1 callees=5 ================= */
/* CALLERS: FUN_14002fe40 */
/* CALLEES: FUN_14002dfd0, QAbstractItemModel, fromAscii_helper, setObjectName, ~QString */

QAbstractItemModel * FUN_14002d480(QAbstractItemModel *param_1,QObject *param_2,undefined8 param_3)

{
  Data *local_res10;
  
  QAbstractItemModel::QAbstractItemModel(param_1,param_2);
  *(undefined ***)param_1 = CDeviceDataModel::vftable;
  *(code **)(param_1 + 0x18) = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  *(code **)(param_1 + 0x20) = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  *(code **)(param_1 + 0x28) = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  *(code **)(param_1 + 0x30) = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  *(QObject **)(param_1 + 0x38) = param_2;
  *(undefined8 *)(param_1 + 0x10) = param_3;
  local_res10 = QString::fromAscii_helper("DeviceDataModel",-1);
  QObject::setObjectName((QObject *)param_1,(QString *)&local_res10);
  QString::~QString((QString *)&local_res10);
  FUN_14002dfd0(param_1);
  return param_1;
}


/* ================= FUN_14002d560 @ 14002d560  size=82  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002fe40 */
/* CALLEES: QSortFilterProxyModel */

QSortFilterProxyModel * FUN_14002d560(QSortFilterProxyModel *param_1,QObject *param_2)

{
  QSortFilterProxyModel::QSortFilterProxyModel(param_1,param_2);
  *(undefined ***)param_1 = CDeviceDataProxyModel::vftable;
  *(code **)(param_1 + 0x18) = shared_null_exref;
  LOCK();
  *(int *)(shared_null_exref + 0x68) = *(int *)(shared_null_exref + 0x68) + 1;
  UNLOCK();
  *(QObject **)(param_1 + 0x10) = param_2;
  *(undefined8 *)(param_1 + 0x24) = 0;
  param_1[0x20] = (QSortFilterProxyModel)0x0;
  return param_1;
}


/* ================= FUN_14002d5c0 @ 14002d5c0  size=253  callers=1 callees=6 ================= */
/* CALLERS: FUN_14002bc30 */
/* CALLEES: FUN_14002ea10, FUN_14002fcb0, FUN_14002fcd0, FUN_140036f30, QDialog, QString */

QDialog * FUN_14002d5c0(QDialog *param_1,undefined8 param_2,undefined8 param_3,undefined4 param_4,
                       longlong *param_5)

{
  QDialog *local_18 [2];
  
  local_18[0] = (QDialog *)((ulonglong)local_18[0] & 0xffffffff00000000);
  QDialog::QDialog(param_1,param_2,local_18);
  *(undefined ***)param_1 = CDeviceSelectionDialog::vftable;
  *(undefined ***)(param_1 + 0x10) = CDeviceSelectionDialog::vftable;
  *(code **)(param_1 + 0x28) = shared_null_exref;
  LOCK();
  *(int *)(shared_null_exref + 0x10) = *(int *)(shared_null_exref + 0x10) + 1;
  UNLOCK();
  *(code **)(param_1 + 0x30) = shared_null_exref;
  LOCK();
  *(int *)(shared_null_exref + 0x10) = *(int *)(shared_null_exref + 0x10) + 1;
  UNLOCK();
  *(code **)(param_1 + 0x38) = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  *(code **)(param_1 + 0x40) = shared_null_exref;
  LOCK();
  *(int *)(shared_null_exref + 0x68) = *(int *)(shared_null_exref + 0x68) + 1;
  UNLOCK();
  local_18[0] = param_1 + 200;
  QString::QString((QString *)(param_1 + 0xd8));
  FUN_140036f30(param_1);
  FUN_14002ea10(param_1,param_3,param_4);
  if (*(int *)(*param_5 + 8) == 0) {
    FUN_14002fcd0(param_1,0);
  }
  else {
    FUN_14002fcb0();
  }
  return param_1;
}


/* ================= FUN_14002d6c0 @ 14002d6c0  size=68  callers=1 callees=2 ================= */
/* CALLERS: FUN_140032220 */
/* CALLEES: QTableView, setAttribute */

QTableView * FUN_14002d6c0(QTableView *param_1,QWidget *param_2)

{
  QTableView::QTableView(param_1,param_2);
  *(undefined ***)param_1 = CTableView::vftable;
  *(undefined ***)(param_1 + 0x10) = CTableView::vftable;
  QWidget::setAttribute((QWidget *)param_1,2,true);
  return param_1;
}


/* ================= FUN_14002d710 @ 14002d710  size=44  callers=0 callees=1 ================= */
/* CALLEES: free_helper */

void FUN_14002d710(longlong *param_1)

{
  int *piVar1;
  int iVar2;
  
  LOCK();
  piVar1 = (int *)(*param_1 + 0x10);
  iVar2 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar2 == 1) {
    QHashData::free_helper((QHashData *)*param_1,(_func_void_Node_ptr *)&LAB_140030c40);
  }
  return;
}


/* ================= FUN_14002d740 @ 14002d740  size=44  callers=0 callees=1 ================= */
/* CALLEES: free_helper */

void FUN_14002d740(longlong *param_1)

{
  int *piVar1;
  int iVar2;
  
  LOCK();
  piVar1 = (int *)(*param_1 + 0x10);
  iVar2 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar2 == 1) {
    QHashData::free_helper((QHashData *)*param_1,(_func_void_Node_ptr *)&LAB_14002b600);
  }
  return;
}


/* ================= FUN_14002d770 @ 14002d770  size=36  callers=1 callees=1 ================= */
/* CALLERS: FUN_14005a340 */
/* CALLEES: qFree */

void FUN_14002d770(undefined8 *param_1)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = (int *)*param_1;
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    qFree((void *)*param_1);
  }
  return;
}


/* ================= FUN_14002d7a0 @ 14002d7a0  size=108  callers=0 callees=2 ================= */
/* CALLEES: qFree, ~QIcon */

void FUN_14002d7a0(undefined8 *param_1)

{
  int iVar1;
  int *piVar2;
  void *pvVar3;
  QIcon *this;
  
  piVar2 = (int *)*param_1;
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    pvVar3 = (void *)*param_1;
    iVar1 = *(int *)((longlong)pvVar3 + 8);
    this = (QIcon *)((longlong)pvVar3 + ((longlong)*(int *)((longlong)pvVar3 + 0xc) + 3) * 8);
    while (this != (QIcon *)((longlong)pvVar3 + ((longlong)iVar1 + 3) * 8)) {
      this = this + -8;
      QIcon::~QIcon(this);
    }
    qFree(pvVar3);
  }
  return;
}


/* ================= FUN_14002d810 @ 14002d810  size=108  callers=4 callees=2 ================= */
/* CALLERS: FUN_14002da40, FUN_14005a260, FUN_14005a390, FUN_14005a8c0 */
/* CALLEES: qFree, ~QString */

void FUN_14002d810(undefined8 *param_1)

{
  int iVar1;
  int *piVar2;
  void *pvVar3;
  QString *this;
  
  piVar2 = (int *)*param_1;
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    pvVar3 = (void *)*param_1;
    iVar1 = *(int *)((longlong)pvVar3 + 8);
    this = (QString *)((longlong)pvVar3 + ((longlong)*(int *)((longlong)pvVar3 + 0xc) + 3) * 8);
    while (this != (QString *)((longlong)pvVar3 + ((longlong)iVar1 + 3) * 8)) {
      this = this + -8;
      QString::~QString(this);
    }
    qFree(pvVar3);
  }
  return;
}


/* ================= FUN_14002d880 @ 14002d880  size=35  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400313f0 */

void FUN_14002d880(undefined8 *param_1)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = (int *)*param_1;
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    FUN_1400313f0(param_1,*param_1);
  }
  return;
}


/* ================= FUN_14002d8b0 @ 14002d8b0  size=96  callers=2 callees=2 ================= */
/* CALLERS: FUN_14002da10, FUN_14002da40 */
/* CALLEES: continueFreeData, ~QString */

void FUN_14002d8b0(longlong *param_1)

{
  int *piVar1;
  int iVar2;
  QMapData *this;
  QMapData *pQVar3;
  QMapData *pQVar4;
  
  if (*param_1 != 0) {
    LOCK();
    piVar1 = (int *)(*param_1 + 0x68);
    iVar2 = *piVar1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (iVar2 == 1) {
      this = (QMapData *)*param_1;
      pQVar4 = *(QMapData **)(this + 8);
      while (pQVar4 != this) {
        pQVar3 = *(QMapData **)(pQVar4 + 8);
        QString::~QString((QString *)(pQVar4 + -8));
        pQVar4 = pQVar3;
      }
      QMapData::continueFreeData(this,0x10);
    }
  }
  return;
}


/* ================= FUN_14002d910 @ 14002d910  size=41  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400314d0 */

void FUN_14002d910(longlong *param_1)

{
  int *piVar1;
  int iVar2;
  
  if (*param_1 != 0) {
    LOCK();
    piVar1 = (int *)(*param_1 + 0x68);
    iVar2 = *piVar1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (iVar2 == 1) {
      FUN_1400314d0(param_1,*param_1);
    }
  }
  return;
}


/* ================= FUN_14002d940 @ 14002d940  size=44  callers=1 callees=1 ================= */
/* CALLERS: FUN_14005a230 */
/* CALLEES: free_helper */

void FUN_14002d940(longlong *param_1)

{
  int *piVar1;
  int iVar2;
  
  LOCK();
  piVar1 = (int *)(*param_1 + 0x10);
  iVar2 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar2 == 1) {
    QHashData::free_helper((QHashData *)*param_1,(_func_void_Node_ptr *)&LAB_14002b600);
  }
  return;
}


/* ================= FUN_14002d970 @ 14002d970  size=150  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002dc80 */
/* CALLEES: qFree */

void FUN_14002d970(QAbstractItemModel *param_1)

{
  int iVar1;
  int *piVar2;
  
  *(undefined ***)param_1 = CDeviceDataModel::vftable;
  piVar2 = *(int **)(param_1 + 0x30);
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    qFree(*(void **)(param_1 + 0x30));
  }
  piVar2 = *(int **)(param_1 + 0x28);
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    qFree(*(void **)(param_1 + 0x28));
  }
  piVar2 = *(int **)(param_1 + 0x20);
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    qFree(*(void **)(param_1 + 0x20));
  }
  piVar2 = *(int **)(param_1 + 0x18);
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    qFree(*(void **)(param_1 + 0x18));
  }
                    /* WARNING: Could not recover jumptable at 0x00014002d9ff. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QAbstractItemModel::~QAbstractItemModel(param_1);
  return;
}


/* ================= FUN_14002da10 @ 14002da10  size=43  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002dcc0 */
/* CALLEES: FUN_14002d8b0 */

void FUN_14002da10(QSortFilterProxyModel *param_1)

{
  *(undefined ***)param_1 = CDeviceDataProxyModel::vftable;
  FUN_14002d8b0(param_1 + 0x18);
                    /* WARNING: Could not recover jumptable at 0x00014002da34. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QSortFilterProxyModel::~QSortFilterProxyModel(param_1);
  return;
}


/* ================= FUN_14002da40 @ 14002da40  size=157  callers=1 callees=4 ================= */
/* CALLERS: FUN_14002dd00 */
/* CALLEES: FUN_14002d810, FUN_14002d8b0, free_helper, ~QString */

void FUN_14002da40(QDialog *param_1)

{
  int *piVar1;
  int iVar2;
  
  *(undefined ***)param_1 = CDeviceSelectionDialog::vftable;
  *(undefined ***)(param_1 + 0x10) = CDeviceSelectionDialog::vftable;
  QString::~QString((QString *)(param_1 + 0xd8));
  FUN_14002d8b0(param_1 + 0x40);
  FUN_14002d810(param_1 + 0x38);
  LOCK();
  piVar1 = (int *)(*(longlong *)(param_1 + 0x30) + 0x10);
  iVar2 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar2 == 1) {
    QHashData::free_helper(*(QHashData **)(param_1 + 0x30),(_func_void_Node_ptr *)&LAB_14002b600);
  }
  LOCK();
  piVar1 = (int *)(*(longlong *)(param_1 + 0x28) + 0x10);
  iVar2 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar2 == 1) {
    QHashData::free_helper(*(QHashData **)(param_1 + 0x28),(_func_void_Node_ptr *)&LAB_140030c40);
  }
                    /* WARNING: Could not recover jumptable at 0x00014002dad6. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::~QDialog(param_1);
  return;
}


/* ================= FUN_14002dae0 @ 14002dae0  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_14002dd40 */

void FUN_14002dae0(QTableView *param_1)

{
  *(undefined ***)param_1 = CTableView::vftable;
  *(undefined ***)(param_1 + 0x10) = CTableView::vftable;
                    /* WARNING: Could not recover jumptable at 0x00014002daf5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QTableView::~QTableView(param_1);
  return;
}


/* ================= FUN_14002db20 @ 14002db20  size=85  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002f800 */
/* CALLEES: FUN_140030cd0, qFree */

undefined8 * FUN_14002db20(undefined8 *param_1,undefined8 *param_2)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  
  piVar2 = (int *)*param_2;
  if ((int *)*param_1 != piVar2) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    piVar3 = (int *)*param_1;
    LOCK();
    iVar1 = *piVar3;
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (iVar1 == 1) {
      qFree((void *)*param_1);
    }
    *param_1 = piVar2;
    if ((*(byte *)(piVar2 + 4) & 1) == 0) {
      FUN_140030cd0(param_1,piVar2[1]);
    }
  }
  return param_1;
}


/* ================= FUN_14002db80 @ 14002db80  size=177  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: detach_helper2, free_helper */

undefined8 * FUN_14002db80(undefined8 *param_1,undefined8 *param_2)

{
  QHashData *pQVar1;
  int iVar2;
  QHashData *pQVar3;
  QHashData *pQVar4;
  
  pQVar3 = (QHashData *)*param_1;
  LOCK();
  *(int *)(pQVar3 + 0x10) = *(int *)(pQVar3 + 0x10) + 1;
  UNLOCK();
  pQVar4 = pQVar3;
  if ((((byte)pQVar3[0x24] & 1) == 0) && (*(int *)(pQVar3 + 0x10) != 1)) {
    pQVar4 = QHashData::detach_helper2
                       (pQVar3,(_func_void_Node_ptr_void_ptr *)&LAB_1400311f0,
                        (_func_void_Node_ptr *)&LAB_14002b600,0x18,8);
    LOCK();
    pQVar1 = pQVar3 + 0x10;
    iVar2 = *(int *)pQVar1;
    *(int *)pQVar1 = *(int *)pQVar1 + -1;
    UNLOCK();
    if (iVar2 == 1) {
      QHashData::free_helper(pQVar3,(_func_void_Node_ptr *)&LAB_14002b600);
    }
  }
  pQVar1 = (QHashData *)*param_1;
  *param_1 = *param_2;
  *param_2 = pQVar4;
  LOCK();
  pQVar3 = pQVar1 + 0x10;
  iVar2 = *(int *)pQVar3;
  *(int *)pQVar3 = *(int *)pQVar3 + -1;
  UNLOCK();
  if (iVar2 == 1) {
    QHashData::free_helper(pQVar1,(_func_void_Node_ptr *)&LAB_14002b600);
  }
  return param_1;
}


/* ================= FUN_14002dc34 @ 14002dc34  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002dd00 */

void FUN_14002dc34(longlong param_1)

{
  FUN_14002dd00(param_1 + -0x10);
  return;
}


/* ================= FUN_14002dc40 @ 14002dc40  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002dd40 */

void FUN_14002dc40(longlong param_1)

{
  FUN_14002dd40(param_1 + -0x10);
  return;
}


/* ================= FUN_14002dc4c @ 14002dc4c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002dd80 */

void FUN_14002dc4c(longlong param_1)

{
  FUN_14002dd80(param_1 + -0x10);
  return;
}


/* ================= FUN_14002dc58 @ 14002dc58  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002ddc0 */

void FUN_14002dc58(longlong param_1)

{
  FUN_14002ddc0(param_1 + -0x10);
  return;
}


/* ================= FUN_14002dc64 @ 14002dc64  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002de00 */

void FUN_14002dc64(longlong param_1)

{
  FUN_14002de00(param_1 + -0x10);
  return;
}


/* ================= FUN_14002dc70 @ 14002dc70  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002dec0 */

void FUN_14002dc70(longlong param_1)

{
  FUN_14002dec0(param_1 + -0x10);
  return;
}


/* ================= FUN_14002dc80 @ 14002dc80  size=52  callers=0 callees=2 ================= */
/* CALLEES: FUN_14002d970, free */

void * FUN_14002dc80(void *param_1,ulonglong param_2)

{
  FUN_14002d970();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002dcc0 @ 14002dcc0  size=52  callers=0 callees=2 ================= */
/* CALLEES: FUN_14002da10, free */

void * FUN_14002dcc0(void *param_1,ulonglong param_2)

{
  FUN_14002da10();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002dd00 @ 14002dd00  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002dc34 */
/* CALLEES: FUN_14002da40, free */

void * FUN_14002dd00(void *param_1,ulonglong param_2)

{
  FUN_14002da40();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002dd40 @ 14002dd40  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002dc40 */
/* CALLEES: FUN_14002dae0, free */

void * FUN_14002dd40(void *param_1,ulonglong param_2)

{
  FUN_14002dae0();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002dd80 @ 14002dd80  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002dc4c */
/* CALLEES: free, ~QFrame */

QFrame * FUN_14002dd80(QFrame *param_1,uint param_2)

{
  QFrame::~QFrame(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002ddc0 @ 14002ddc0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002dc58 */
/* CALLEES: free, ~QGridLayout */

QGridLayout * FUN_14002ddc0(QGridLayout *param_1,uint param_2)

{
  QGridLayout::~QGridLayout(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002de00 @ 14002de00  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002dc64 */
/* CALLEES: free, ~QPushButton */

QPushButton * FUN_14002de00(QPushButton *param_1,uint param_2)

{
  QPushButton::~QPushButton(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002de40 @ 14002de40  size=53  callers=0 callees=2 ================= */
/* CALLEES: free, ~QSpacerItem */

QSpacerItem * FUN_14002de40(QSpacerItem *param_1,uint param_2)

{
  QSpacerItem::~QSpacerItem(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002de80 @ 14002de80  size=53  callers=2 callees=2 ================= */
/* CALLERS: FUN_14005a6f0, FUN_14005a790 */
/* CALLEES: free, ~QString */

QString * FUN_14002de80(QString *param_1,uint param_2)

{
  QString::~QString(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002dec0 @ 14002dec0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002dc70 */
/* CALLEES: free, ~QVBoxLayout */

QVBoxLayout * FUN_14002dec0(QVBoxLayout *param_1,uint param_2)

{
  QVBoxLayout::~QVBoxLayout(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14002df00 @ 14002df00  size=196  callers=1 callees=3 ================= */
/* CALLERS: FUN_140032b20 */
/* CALLEES: parentWidget, setGeometry, translate */

void FUN_14002df00(QWidget *param_1)

{
  longlong lVar1;
  QWidget *pQVar2;
  int iVar3;
  int iVar4;
  int local_res8;
  int local_resc;
  int local_18 [3];
  int local_c;
  
  lVar1 = *(longlong *)(param_1 + 0x20);
  iVar4 = *(int *)(lVar1 + 0x1c) - *(int *)(lVar1 + 0x14);
  iVar3 = *(int *)(lVar1 + 0x20) - *(int *)(lVar1 + 0x18);
  pQVar2 = QWidget::parentWidget(param_1);
  lVar1 = *(longlong *)(pQVar2 + 0x20);
  local_18[0] = 0;
  local_18[1] = 0;
  local_res8 = (*(int *)(lVar1 + 0x14) + *(int *)(lVar1 + 0x1c)) / 2;
  local_resc = (*(int *)(lVar1 + 0x18) + *(int *)(lVar1 + 0x20)) / 2;
  local_18[2] = iVar4;
  local_c = iVar3;
  QRect::translate((QRect *)local_18,(QPoint *)&local_res8);
  iVar3 = -((iVar3 + 1) / 2);
  local_c = local_c + iVar3;
  iVar4 = -((iVar4 + 1) / 2);
  local_18[1] = local_18[1] + iVar3;
  local_18[0] = local_18[0] + iVar4;
  local_18[2] = local_18[2] + iVar4;
  QWidget::setGeometry(param_1,(QRect *)local_18);
  return;
}


/* ================= FUN_14002dfd0 @ 14002dfd0  size=342  callers=2 callees=6 ================= */
/* CALLERS: FUN_14002d480, FUN_14002e130 */
/* CALLEES: FUN_1400308c0, beginResetModel, continueFreeData, free_helper, qFree, ~QString */

void FUN_14002dfd0(QAbstractItemModel *param_1)

{
  QHashData *pQVar1;
  QMapData *pQVar2;
  int iVar3;
  int *piVar4;
  longlong lVar5;
  QHashData *pQVar6;
  QMapData *this;
  QMapData *pQVar7;
  code *pcVar8;
  
  QAbstractItemModel::beginResetModel(param_1);
  pcVar8 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  piVar4 = *(int **)(param_1 + 0x18);
  *(code **)(param_1 + 0x18) = pcVar8;
  LOCK();
  iVar3 = *piVar4;
  *piVar4 = *piVar4 + -1;
  UNLOCK();
  if (iVar3 == 1) {
    qFree(piVar4);
  }
  pcVar8 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  piVar4 = *(int **)(param_1 + 0x20);
  *(code **)(param_1 + 0x20) = pcVar8;
  LOCK();
  iVar3 = *piVar4;
  *piVar4 = *piVar4 + -1;
  UNLOCK();
  if (iVar3 == 1) {
    qFree(piVar4);
  }
  pcVar8 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  piVar4 = *(int **)(param_1 + 0x28);
  *(code **)(param_1 + 0x28) = pcVar8;
  LOCK();
  iVar3 = *piVar4;
  *piVar4 = *piVar4 + -1;
  UNLOCK();
  if (iVar3 == 1) {
    qFree(piVar4);
  }
  pcVar8 = shared_null_exref;
  lVar5 = *(longlong *)(param_1 + 0x10);
  LOCK();
  *(int *)(shared_null_exref + 0x10) = *(int *)(shared_null_exref + 0x10) + 1;
  UNLOCK();
  pQVar6 = *(QHashData **)(lVar5 + 0x28);
  *(code **)(lVar5 + 0x28) = pcVar8;
  LOCK();
  pQVar1 = pQVar6 + 0x10;
  iVar3 = *(int *)pQVar1;
  *(int *)pQVar1 = *(int *)pQVar1 + -1;
  UNLOCK();
  if (iVar3 == 1) {
    QHashData::free_helper(pQVar6,(_func_void_Node_ptr *)&LAB_140030c40);
  }
  pcVar8 = shared_null_exref;
  lVar5 = *(longlong *)(param_1 + 0x10);
  LOCK();
  *(int *)(shared_null_exref + 0x10) = *(int *)(shared_null_exref + 0x10) + 1;
  UNLOCK();
  pQVar6 = *(QHashData **)(lVar5 + 0x30);
  *(code **)(lVar5 + 0x30) = pcVar8;
  LOCK();
  pQVar1 = pQVar6 + 0x10;
  iVar3 = *(int *)pQVar1;
  *(int *)pQVar1 = *(int *)pQVar1 + -1;
  UNLOCK();
  if (iVar3 == 1) {
    QHashData::free_helper(pQVar6,(_func_void_Node_ptr *)&LAB_14002b600);
  }
  pcVar8 = shared_null_exref;
  lVar5 = *(longlong *)(param_1 + 0x10);
  LOCK();
  *(int *)(shared_null_exref + 0x68) = *(int *)(shared_null_exref + 0x68) + 1;
  UNLOCK();
  this = *(QMapData **)(lVar5 + 0x40);
  *(code **)(lVar5 + 0x40) = pcVar8;
  if (this != (QMapData *)0x0) {
    LOCK();
    pQVar2 = this + 0x68;
    iVar3 = *(int *)pQVar2;
    *(int *)pQVar2 = *(int *)pQVar2 + -1;
    UNLOCK();
    if (iVar3 == 1) {
      pQVar2 = *(QMapData **)(this + 8);
      while (pQVar2 != this) {
        pQVar7 = *(QMapData **)(pQVar2 + 8);
        QString::~QString((QString *)(pQVar2 + -8));
        pQVar2 = pQVar7;
      }
      QMapData::continueFreeData(this,0x10);
    }
  }
  FUN_1400308c0(*(longlong *)(param_1 + 0x10) + 0x38);
                    /* WARNING: Could not recover jumptable at 0x00014002e11f. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QAbstractItemModel::endResetModel(param_1);
  return;
}


/* ================= FUN_14002e130 @ 14002e130  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: FUN_14002dfd0 */

void FUN_14002e130(longlong param_1)

{
  FUN_14002dfd0(*(undefined8 *)(param_1 + 0xc0));
  return;
}


/* ================= FUN_14002e140 @ 14002e140  size=380  callers=1 callees=8 ================= */
/* CALLERS: FUN_140031210 */
/* CALLEES: compare, contains, endsWith, fromAscii_helper, left, right, startsWith, ~QString */

bool FUN_14002e140(undefined8 param_1,QString *param_2,QString *param_3,char param_4)

{
  bool bVar1;
  int iVar2;
  QString *pQVar3;
  char *pcVar4;
  char *pcVar5;
  QString local_res20 [8];
  Data *local_18 [2];
  
  if (param_4 != '\0') {
    iVar2 = QString::compare(param_2,param_3,0);
    return iVar2 == 0;
  }
  local_18[0] = QString::fromAscii_helper("*",-1);
  bVar1 = QString::startsWith(param_3,(QString *)local_18,1);
  QString::~QString((QString *)local_18);
  if (bVar1) {
    pQVar3 = (QString *)QString::right(param_3,(int)local_18);
    bVar1 = QString::endsWith(param_2,pQVar3,0);
    QString::~QString((QString *)local_18);
    return bVar1;
  }
  local_18[0] = QString::fromAscii_helper("*",-1);
  bVar1 = QString::endsWith(param_3,(QString *)local_18,1);
  QString::~QString((QString *)local_18);
  if (bVar1) {
    pQVar3 = (QString *)QString::left(param_3,(int)local_18);
    bVar1 = QString::startsWith(param_2,pQVar3,0);
    QString::~QString((QString *)local_18);
    return bVar1;
  }
  pcVar4 = (char *)QString::contains(param_2,local_res20,(CaseSensitivity)param_3);
  pcVar5 = (char *)0x0;
  if (*pcVar4 != '\0') {
    pcVar5 = pcVar4;
  }
  return pcVar5 != (char *)0x0;
}


/* ================= FUN_14002e2c0 @ 14002e2c0  size=123  callers=1 callees=1 ================= */
/* CALLERS: FUN_140030a10 */
/* CALLEES: QString */

QString * FUN_14002e2c0(longlong param_1,QString *param_2,uint param_3)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  
  puVar2 = *(undefined8 **)(*(longlong *)(param_1 + 0x10) + 0x30);
  if ((*(int *)((longlong)puVar2 + 0x14) != 0) && (*(uint *)(puVar2 + 4) != 0)) {
    puVar1 = (undefined8 *)(puVar2[1] + ((ulonglong)param_3 % (ulonglong)*(uint *)(puVar2 + 4)) * 8)
    ;
    for (puVar3 = (undefined8 *)*puVar1; (puVar3 != puVar2 && (param_3 != *(uint *)(puVar3 + 1)));
        puVar3 = (undefined8 *)*puVar3) {
      puVar1 = puVar3;
    }
    if ((undefined8 *)*puVar1 != puVar2) {
      QString::QString(param_2,(QString *)((undefined8 *)*puVar1 + 2));
      return param_2;
    }
  }
  QString::QString(param_2);
  return param_2;
}


/* ================= FUN_14002e340 @ 14002e340  size=143  callers=2 callees=2 ================= */
/* CALLERS: FUN_14002e3d0, FUN_140030a10 */
/* CALLEES: FUN_140032d60, ~QString */

undefined4 FUN_14002e340(longlong param_1,uint param_2)

{
  undefined4 uVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  longlong lVar5;
  uint local_res10 [6];
  undefined1 local_38 [24];
  QString local_20 [24];
  
  puVar2 = *(undefined8 **)(*(longlong *)(param_1 + 0x10) + 0x28);
  puVar4 = puVar2;
  if (*(uint *)(puVar2 + 4) != 0) {
    puVar4 = (undefined8 *)(puVar2[1] + ((ulonglong)param_2 % (ulonglong)*(uint *)(puVar2 + 4)) * 8)
    ;
    for (puVar3 = (undefined8 *)*puVar4; (puVar3 != puVar2 && (param_2 != *(uint *)(puVar3 + 1)));
        puVar3 = (undefined8 *)*puVar3) {
      puVar4 = puVar3;
    }
    puVar4 = (undefined8 *)*puVar4;
  }
  if (puVar4 == puVar2) {
    return 0xffffffff;
  }
  local_res10[0] = param_2;
  lVar5 = FUN_140032d60(*(longlong *)(param_1 + 0x10) + 0x28,local_38,local_res10);
  uVar1 = *(undefined4 *)(lVar5 + 8);
  QString::~QString(local_20);
  return uVar1;
}


/* ================= FUN_14002e3d0 @ 14002e3d0  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_140030760 */
/* CALLEES: FUN_14002e340 */

void FUN_14002e3d0(longlong param_1)

{
  FUN_14002e340(*(undefined8 *)(param_1 + 0xc0));
  return;
}


/* ================= FUN_14002e3e0 @ 14002e3e0  size=157  callers=1 callees=3 ================= */
/* CALLERS: FUN_140030a10 */
/* CALLEES: FUN_140032d60, QString, ~QString */

QString * FUN_14002e3e0(longlong param_1,QString *param_2,uint param_3)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  longlong lVar4;
  uint local_res18 [4];
  undefined1 local_38 [24];
  QString local_20 [24];
  
  puVar1 = *(undefined8 **)(*(longlong *)(param_1 + 0x10) + 0x28);
  puVar3 = puVar1;
  if (*(uint *)(puVar1 + 4) != 0) {
    puVar3 = (undefined8 *)(puVar1[1] + ((ulonglong)param_3 % (ulonglong)*(uint *)(puVar1 + 4)) * 8)
    ;
    for (puVar2 = (undefined8 *)*puVar3; (puVar2 != puVar1 && (param_3 != *(uint *)(puVar2 + 1)));
        puVar2 = (undefined8 *)*puVar2) {
      puVar3 = puVar2;
    }
    puVar3 = (undefined8 *)*puVar3;
  }
  local_res18[0] = param_3;
  if (puVar3 == puVar1) {
    QString::QString(param_2);
    return param_2;
  }
  lVar4 = FUN_140032d60(*(longlong *)(param_1 + 0x10) + 0x28,local_38,local_res18);
  QString::QString(param_2,(QString *)(lVar4 + 0x18));
  QString::~QString(local_20);
  return param_2;
}


/* ================= FUN_14002e480 @ 14002e480  size=140  callers=1 callees=2 ================= */
/* CALLERS: FUN_140030a10 */
/* CALLEES: FUN_140032d60, ~QString */

undefined4 FUN_14002e480(longlong param_1,uint param_2)

{
  undefined4 uVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  longlong lVar5;
  uint local_res10 [6];
  undefined1 local_38 [24];
  QString local_20 [24];
  
  puVar2 = *(undefined8 **)(*(longlong *)(param_1 + 0x10) + 0x28);
  puVar4 = puVar2;
  if (*(uint *)(puVar2 + 4) != 0) {
    puVar4 = (undefined8 *)(puVar2[1] + ((ulonglong)param_2 % (ulonglong)*(uint *)(puVar2 + 4)) * 8)
    ;
    for (puVar3 = (undefined8 *)*puVar4; (puVar3 != puVar2 && (param_2 != *(uint *)(puVar3 + 1)));
        puVar3 = (undefined8 *)*puVar3) {
      puVar4 = puVar3;
    }
    puVar4 = (undefined8 *)*puVar4;
  }
  if (puVar4 == puVar2) {
    return 0;
  }
  local_res10[0] = param_2;
  lVar5 = FUN_140032d60(*(longlong *)(param_1 + 0x10) + 0x28,local_38,local_res10);
  uVar1 = *(undefined4 *)(lVar5 + 0x10);
  QString::~QString(local_20);
  return uVar1;
}


/* ================= FUN_14002e510 @ 14002e510  size=28  callers=2 callees=0 ================= */
/* CALLERS: FUN_14002e9a0, FUN_140030a10 */

int FUN_14002e510(undefined8 param_1,int *param_2)

{
  if (((-1 < *param_2) && (-1 < param_2[1])) && (*(longlong *)(param_2 + 4) != 0)) {
    return param_2[2];
  }
  return -1;
}


/* ================= FUN_14002e530 @ 14002e530  size=264  callers=1 callees=3 ================= */
/* CALLERS: FUN_14002e640 */
/* CALLEES: QString, operator==, ~QString */

int FUN_14002e530(longlong param_1,QString *param_2)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  bool bVar4;
  longlong lVar5;
  ulonglong uVar6;
  uint uVar7;
  ulonglong uVar8;
  longlong lVar9;
  longlong lVar10;
  QString local_res8 [8];
  
  uVar6 = 0;
  lVar9 = *(longlong *)(param_1 + 0x20);
  uVar8 = uVar6;
  if (0 < *(int *)(lVar9 + 0xc) - *(int *)(lVar9 + 8)) {
    do {
      iVar1 = *(int *)(lVar9 + 0x18 + ((longlong)*(int *)(lVar9 + 8) + uVar6) * 8);
      lVar9 = *(longlong *)(*(longlong *)(param_1 + 0x10) + 0x40);
      if ((*(int *)(lVar9 + 0x70) == 0) ||
         (lVar10 = (longlong)*(int *)(lVar9 + 0x6c), lVar5 = lVar9, lVar10 < 0)) {
LAB_14002e5d3:
        QString::QString(local_res8);
      }
      else {
        do {
          lVar3 = *(longlong *)(lVar5 + 8 + lVar10 * 8);
          while ((lVar2 = lVar3, lVar2 != lVar9 && (*(int *)(lVar2 + -0x10) < iVar1))) {
            lVar5 = lVar2;
            lVar3 = *(longlong *)(lVar2 + 8 + lVar10 * 8);
          }
          lVar10 = lVar10 + -1;
        } while (-1 < lVar10);
        if ((lVar2 == lVar9) || (iVar1 < *(int *)(lVar2 + -0x10))) goto LAB_14002e5d3;
        QString::QString(local_res8,(QString *)(lVar2 + -8));
      }
      bVar4 = QString::operator==(param_2,local_res8);
      if (bVar4) {
        QString::~QString(local_res8);
        return iVar1;
      }
      QString::~QString(local_res8);
      uVar7 = (int)uVar8 + 1;
      uVar6 = uVar6 + 1;
      lVar9 = *(longlong *)(param_1 + 0x20);
      uVar8 = (ulonglong)uVar7;
    } while ((int)uVar7 < *(int *)(lVar9 + 0xc) - *(int *)(lVar9 + 8));
  }
  return -1;
}


/* ================= FUN_14002e640 @ 14002e640  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002fcb0 */
/* CALLEES: FUN_14002e530 */

void FUN_14002e640(longlong param_1)

{
  FUN_14002e530(*(undefined8 *)(param_1 + 0xc0));
  return;
}


/* ================= FUN_14002e650 @ 14002e650  size=228  callers=1 callees=3 ================= */
/* CALLERS: FUN_140030a10 */
/* CALLEES: FUN_140032d60, QString, ~QString */

QString * FUN_14002e650(longlong param_1,QString *param_2,uint param_3)

{
  int iVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  longlong lVar5;
  uint local_res18 [4];
  undefined1 local_38 [24];
  QString local_20 [24];
  
  puVar2 = *(undefined8 **)(*(longlong *)(param_1 + 0x10) + 0x28);
  puVar4 = puVar2;
  if (*(uint *)(puVar2 + 4) != 0) {
    puVar4 = (undefined8 *)(puVar2[1] + ((ulonglong)param_3 % (ulonglong)*(uint *)(puVar2 + 4)) * 8)
    ;
    for (puVar3 = (undefined8 *)*puVar4; (puVar3 != puVar2 && (param_3 != *(uint *)(puVar3 + 1)));
        puVar3 = (undefined8 *)*puVar3) {
      puVar4 = puVar3;
    }
    puVar4 = (undefined8 *)*puVar4;
  }
  local_res18[0] = param_3;
  if (puVar4 == puVar2) {
    QString::QString(param_2);
    return param_2;
  }
  lVar5 = FUN_140032d60(*(longlong *)(param_1 + 0x10) + 0x28,local_38,local_res18);
  iVar1 = *(int *)(lVar5 + 4);
  if (-1 < iVar1) {
    lVar5 = *(longlong *)(*(longlong *)(param_1 + 0x10) + 0x38);
    if (iVar1 < *(int *)(lVar5 + 0xc) - *(int *)(lVar5 + 8)) {
      QString::QString(param_2,(QString *)
                               (lVar5 + ((longlong)*(int *)(lVar5 + 8) + (longlong)iVar1) * 8 + 0x18
                               ));
      goto LAB_14002e71b;
    }
  }
  QString::QString(param_2);
LAB_14002e71b:
  QString::~QString(local_20);
  return param_2;
}


/* ================= FUN_14002e740 @ 14002e740  size=185  callers=2 callees=1 ================= */
/* CALLERS: FUN_14002fb50, FUN_14002fcd0 */
/* CALLEES: ~QModelIndex */

undefined8 * FUN_14002e740(longlong *param_1,undefined8 *param_2,int param_3)

{
  int iVar1;
  longlong lVar2;
  int *piVar3;
  ulonglong uVar4;
  undefined8 local_28;
  undefined8 local_20;
  undefined8 uStack_18;
  
  lVar2 = param_1[4];
  iVar1 = *(int *)(lVar2 + 8);
  if (0 < *(int *)(lVar2 + 0xc) - iVar1) {
    for (piVar3 = (int *)(lVar2 + 0x18 + (longlong)iVar1 * 8);
        piVar3 != (int *)(lVar2 + 0x18 + (longlong)*(int *)(lVar2 + 0xc) * 8); piVar3 = piVar3 + 2)
    {
      if (*piVar3 == param_3) {
        uVar4 = (longlong)piVar3 + (-lVar2 - ((longlong)iVar1 * 8 + 0x18)) >> 3;
        if ((int)uVar4 != -1) {
          local_28 = 0xffffffffffffffff;
          local_20 = 0;
          uStack_18 = 0;
          (**(code **)(*param_1 + 0x58))(param_1,param_2,uVar4 & 0xffffffff,0,&local_28);
          QModelIndex::~QModelIndex((QModelIndex *)&local_28);
          return param_2;
        }
        break;
      }
    }
  }
  *param_2 = 0xffffffffffffffff;
  param_2[1] = 0;
  param_2[2] = 0;
  return param_2;
}


/* ================= FUN_14002e800 @ 14002e800  size=142  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002e890 */
/* CALLEES: FUN_140032d60, ~QString */

undefined4 FUN_14002e800(longlong param_1,uint param_2)

{
  undefined4 uVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  undefined4 *puVar5;
  uint local_res10 [6];
  undefined1 local_38 [24];
  QString local_20 [24];
  
  puVar2 = *(undefined8 **)(*(longlong *)(param_1 + 0x10) + 0x28);
  puVar4 = puVar2;
  if (*(uint *)(puVar2 + 4) != 0) {
    puVar4 = (undefined8 *)(puVar2[1] + ((ulonglong)param_2 % (ulonglong)*(uint *)(puVar2 + 4)) * 8)
    ;
    for (puVar3 = (undefined8 *)*puVar4; (puVar3 != puVar2 && (param_2 != *(uint *)(puVar3 + 1)));
        puVar3 = (undefined8 *)*puVar3) {
      puVar4 = puVar3;
    }
    puVar4 = (undefined8 *)*puVar4;
  }
  if (puVar4 == puVar2) {
    return 0xffffffff;
  }
  local_res10[0] = param_2;
  puVar5 = (undefined4 *)FUN_140032d60(*(longlong *)(param_1 + 0x10) + 0x28,local_38,local_res10);
  uVar1 = *puVar5;
  QString::~QString(local_20);
  return uVar1;
}


/* ================= FUN_14002e890 @ 14002e890  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_140030760 */
/* CALLEES: FUN_14002e800 */

void FUN_14002e890(longlong param_1)

{
  FUN_14002e800(*(undefined8 *)(param_1 + 0xc0));
  return;
}


/* ================= FUN_14002e8a0 @ 14002e8a0  size=129  callers=2 callees=2 ================= */
/* CALLERS: FUN_14002e930, FUN_140030a10 */
/* CALLEES: FUN_140032e30, QString */

QString * FUN_14002e8a0(longlong param_1,QString *param_2,uint param_3)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  uint local_res18 [4];
  
  puVar1 = *(undefined8 **)(*(longlong *)(param_1 + 0x10) + 0x28);
  puVar3 = puVar1;
  if (*(uint *)(puVar1 + 4) != 0) {
    puVar3 = (undefined8 *)(puVar1[1] + ((ulonglong)param_3 % (ulonglong)*(uint *)(puVar1 + 4)) * 8)
    ;
    for (puVar2 = (undefined8 *)*puVar3; (puVar2 != puVar1 && (param_3 != *(uint *)(puVar2 + 1)));
        puVar2 = (undefined8 *)*puVar2) {
      puVar3 = puVar2;
    }
    puVar3 = (undefined8 *)*puVar3;
  }
  local_res18[0] = param_3;
  if (puVar3 == puVar1) {
    QString::QString(param_2);
    return param_2;
  }
  FUN_140032e30(*(longlong *)(param_1 + 0x10) + 0x40,param_2,local_res18);
  return param_2;
}


/* ================= FUN_14002e930 @ 14002e930  size=30  callers=1 callees=1 ================= */
/* CALLERS: FUN_140030760 */
/* CALLEES: FUN_14002e8a0 */

undefined8 FUN_14002e930(longlong param_1,undefined8 param_2)

{
  FUN_14002e8a0(*(undefined8 *)(param_1 + 0xc0));
  return param_2;
}


/* ================= FUN_14002e950 @ 14002e950  size=71  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002bc30 */
/* CALLEES: QString */

undefined1 * FUN_14002e950(longlong param_1,undefined1 *param_2)

{
  *param_2 = *(undefined1 *)(param_1 + 200);
  *(undefined4 *)(param_2 + 4) = *(undefined4 *)(param_1 + 0xcc);
  *(undefined4 *)(param_2 + 8) = *(undefined4 *)(param_1 + 0xd0);
  *(undefined4 *)(param_2 + 0xc) = *(undefined4 *)(param_1 + 0xd4);
  QString::QString((QString *)(param_2 + 0x10),(QString *)(param_1 + 0xd8));
  return param_2;
}


/* ================= FUN_14002e9a0 @ 14002e9a0  size=101  callers=3 callees=3 ================= */
/* CALLERS: FUN_14002f980, FUN_140030760, FUN_140032b20 */
/* CALLEES: FUN_14002e510, currentIndex, ~QModelIndex */

undefined4 FUN_14002e9a0(longlong param_1)

{
  undefined4 uVar1;
  undefined8 uVar2;
  QModelIndex local_38 [24];
  QModelIndex local_20 [24];
  
  uVar2 = QAbstractItemView::currentIndex(*(QAbstractItemView **)(param_1 + 0x88));
  uVar2 = (**(code **)(**(longlong **)(param_1 + 0xb8) + 0x140))
                    (*(longlong **)(param_1 + 0xb8),local_38,uVar2);
  uVar1 = FUN_14002e510(*(undefined8 *)(param_1 + 0xc0),uVar2);
  QModelIndex::~QModelIndex(local_38);
  QModelIndex::~QModelIndex(local_20);
  return uVar1;
}


/* ================= FUN_14002ea10 @ 14002ea10  size=3553  callers=1 callees=32 ================= */
/* CALLERS: FUN_14002d5c0 */
/* CALLEES: FUN_140027a00, FUN_14002db80, FUN_14002e130, FUN_14002fca0, FUN_14002fdb0, FUN_14002fe40, FUN_140030270, FUN_1400307f0, FUN_140030840, FUN_140030c50, FUN_140030d50, FUN_140030e00, FUN_1400316b0, FUN_140031ee0, FUN_140032eb0, FUN_140038440, QString, __security_check_cookie, allocateNode, detach_helper2, free, free_helper, fromAscii_helper, nextNode, operator=, operator==, qFree, realloc, rehash, ~QIcon, ~QString, ~QVariant */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14002ea10(longlong param_1,char *param_2,uint param_3)

{
  int *piVar1;
  QIcon *pQVar2;
  code *pcVar3;
  uint uVar4;
  QVariant *pQVar5;
  undefined8 *puVar6;
  longlong lVar7;
  code *pcVar8;
  Node *pNVar9;
  char *pcVar10;
  longlong lVar11;
  longlong lVar12;
  bool bVar13;
  QString *pQVar14;
  longlong lVar15;
  QHashData *pQVar16;
  undefined8 *puVar17;
  undefined8 *puVar18;
  undefined8 uVar19;
  Node *pNVar20;
  QHashData *pQVar21;
  QHashData *pQVar22;
  longlong *plVar23;
  uint uVar24;
  int iVar25;
  ulonglong uVar26;
  QHashData *pQVar27;
  QIcon *pQVar28;
  code *pcVar29;
  QString *pQVar30;
  QHashData *pQVar31;
  longlong lVar32;
  longlong lVar33;
  ulonglong uVar34;
  int iVar35;
  undefined1 auStackY_278 [32];
  code *local_248;
  code *local_240;
  undefined1 local_238 [8];
  code *local_230;
  QHashData *local_228;
  code *local_220;
  Node *local_218;
  undefined4 local_210;
  uint local_20c;
  longlong *local_208;
  code *local_200;
  undefined4 local_1f8;
  int local_1f4;
  undefined4 local_1f0;
  undefined4 local_1ec;
  undefined4 local_1e8;
  QString local_1e0 [8];
  undefined8 local_1d8;
  int local_1d0 [2];
  Data *local_1c8;
  Data *local_1c0;
  char *local_1b8;
  code *local_1b0;
  code *local_1a8;
  code *local_1a0;
  code *local_198;
  int local_190;
  undefined4 local_18c;
  undefined4 uStack_188;
  Data *local_180;
  Data *local_178;
  longlong local_170;
  int *local_168;
  Data *local_160;
  QString local_158 [8];
  longlong *local_150;
  QString local_148 [8];
  QString local_140 [8];
  undefined8 *local_138;
  undefined1 local_130 [8];
  longlong local_128 [12];
  char local_c8 [128];
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStackY_278;
  uVar34 = 0;
  local_210 = 0;
  local_20c = param_3;
  local_1b8 = param_2;
  local_170 = param_1;
  QString::QString(local_1e0);
  local_1d8 = 0;
  local_200 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  local_218 = (Node *)shared_null_exref;
  LOCK();
  *(int *)(shared_null_exref + 0x10) = *(int *)(shared_null_exref + 0x10) + 1;
  UNLOCK();
  local_248 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  *(undefined1 *)(param_1 + 200) = 0;
  *(undefined8 *)(param_1 + 0xcc) = 0xffffffffffffffff;
  *(undefined4 *)(param_1 + 0xd4) = 0xffffffff;
  pQVar14 = (QString *)QString::QString(local_158);
  QString::operator=((QString *)(param_1 + 0xd8),pQVar14);
  QString::~QString(local_158);
  *(undefined8 *)(param_1 + 0x48) = 0;
  FUN_14002fe40(param_1);
  FUN_14002fdb0(param_1);
  FUN_14002e130(param_1);
  uVar26 = uVar34;
  if (param_3 != 0) {
    do {
      local_190 = (int)uVar26;
      FUN_1400307f0(&local_200,&local_190);
      uVar24 = (int)uVar26 + 1;
      uVar26 = (ulonglong)uVar24;
    } while (uVar24 < param_3);
  }
  lVar11 = local_170;
  lVar33 = param_1;
  if (param_2 == (char *)0x0) goto LAB_14002f715;
  if (param_3 != 0) {
    local_150 = (longlong *)(param_1 + 0x40);
    local_208 = (longlong *)(param_1 + 0x38);
    do {
      plVar23 = local_150;
      local_1c8 = QString::fromAscii_helper(param_2,-1);
      iVar25 = (int)uVar34;
      local_1d0[0] = iVar25;
      if (*(int *)(*plVar23 + 0x68) != 1) {
        FUN_140030e00(plVar23);
      }
      lVar33 = *plVar23;
      lVar32 = (longlong)*(int *)(lVar33 + 0x6c);
      lVar15 = lVar33;
      if (lVar32 < 0) {
LAB_14002ebd9:
        FUN_140031ee0(plVar23,lVar33,local_128,local_1d0);
      }
      else {
        do {
          lVar12 = *(longlong *)(lVar15 + 8 + lVar32 * 8);
          while ((lVar7 = lVar12, lVar7 != lVar33 && (*(int *)(lVar7 + -0x10) < iVar25))) {
            lVar15 = lVar7;
            lVar12 = *(longlong *)(lVar7 + 8 + lVar32 * 8);
          }
          local_128[lVar32] = lVar15;
          lVar32 = lVar32 + -1;
        } while (-1 < lVar32);
        if ((lVar7 == lVar33) || (iVar25 < *(int *)(lVar7 + -0x10))) goto LAB_14002ebd9;
        QString::operator=((QString *)(lVar7 + -8),(QString *)&local_1c8);
      }
      QString::~QString((QString *)&local_1c8);
      local_1f8 = *(undefined4 *)(param_2 + 0x214);
      local_1f0 = *(undefined4 *)(param_2 + 0x200);
      local_160 = QString::fromAscii_helper(param_2 + 0x100,-1);
      QString::operator=(local_1e0,(QString *)&local_160);
      QString::~QString((QString *)&local_160);
      local_1e8 = *(undefined4 *)(param_2 + 0x208);
      uStack_188 = *(undefined4 *)(param_2 + 0x210);
      local_18c = *(undefined4 *)(param_2 + 0x20c);
      local_1d8 = *(undefined8 *)(param_2 + 0x20c);
      local_180 = QString::fromAscii_helper(param_2 + 0x80,-1);
      lVar33 = *local_208;
      iVar35 = *(int *)(lVar33 + 0xc);
      if (0 < iVar35 - *(int *)(lVar33 + 8)) {
        for (pQVar14 = (QString *)(lVar33 + 0x18 + (longlong)*(int *)(lVar33 + 8) * 8);
            pQVar14 != (QString *)(lVar33 + 0x18 + (longlong)iVar35 * 8); pQVar14 = pQVar14 + 8) {
          bVar13 = QString::operator==(pQVar14,(QString *)&local_180);
          if (bVar13) {
            iVar35 = (int)((longlong)
                           (pQVar14 +
                           (-*local_208 - ((longlong)*(int *)(*local_208 + 8) * 8 + 0x18))) >> 3);
            goto LAB_14002ecd2;
          }
        }
      }
      iVar35 = -1;
LAB_14002ecd2:
      plVar23 = local_208;
      QString::~QString((QString *)&local_180);
      if (iVar35 == -1) {
        local_178 = QString::fromAscii_helper(param_2 + 0x80,-1);
        FUN_140030840(plVar23);
        QString::~QString((QString *)&local_178);
        lVar33 = *plVar23;
        iVar35 = (*(int *)(lVar33 + 0xc) - *(int *)(lVar33 + 8)) + -1;
      }
      local_1f4 = iVar35;
      pQVar16 = *(QHashData **)(param_1 + 0x28);
      if (*(int *)(*(longlong *)(param_1 + 0x28) + 0x10) != 1) {
        pQVar16 = QHashData::detach_helper2
                            (pQVar16,(_func_void_Node_ptr_void_ptr *)&LAB_1400311d0,
                             (_func_void_Node_ptr *)&LAB_140030c40,0x38,8);
        LOCK();
        piVar1 = (int *)(*(longlong *)(param_1 + 0x28) + 0x10);
        iVar35 = *piVar1;
        *piVar1 = *piVar1 + -1;
        UNLOCK();
        if (iVar35 == 1) {
          QHashData::free_helper
                    (*(QHashData **)(param_1 + 0x28),(_func_void_Node_ptr *)&LAB_140030c40);
        }
        *(QHashData **)(param_1 + 0x28) = pQVar16;
      }
      uVar24 = *(uint *)(pQVar16 + 0x20);
      if (uVar24 == 0) {
        pQVar27 = (QHashData *)(param_1 + 0x28);
        pQVar31 = pQVar16;
      }
      else {
        pQVar27 = (QHashData *)(*(longlong *)(pQVar16 + 8) + (uVar34 % (ulonglong)uVar24) * 8);
        pQVar31 = *(QHashData **)pQVar27;
        if (*(QHashData **)pQVar27 != pQVar16) {
          do {
            pQVar21 = pQVar31;
            if (iVar25 == *(int *)(pQVar21 + 8)) break;
            pQVar16 = *(QHashData **)(param_1 + 0x28);
            pQVar31 = *(QHashData **)pQVar21;
            pQVar27 = pQVar21;
          } while (*(QHashData **)pQVar21 != pQVar16);
        }
        pQVar31 = *(QHashData **)pQVar27;
      }
      if (pQVar31 == pQVar16) {
        if ((int)uVar24 <= *(int *)(pQVar16 + 0x14)) {
          QHashData::rehash(pQVar16,*(short *)(pQVar16 + 0x1e) + 1);
          pQVar16 = *(QHashData **)(param_1 + 0x28);
          if (*(uint *)(pQVar16 + 0x20) == 0) {
            pQVar27 = (QHashData *)(param_1 + 0x28);
          }
          else {
            pQVar27 = (QHashData *)
                      (*(longlong *)(pQVar16 + 8) +
                      (uVar34 % (ulonglong)*(uint *)(pQVar16 + 0x20)) * 8);
            for (pQVar31 = *(QHashData **)pQVar27;
                (pQVar31 != pQVar16 && (iVar25 != *(int *)(pQVar31 + 8)));
                pQVar31 = *(QHashData **)pQVar31) {
              pQVar27 = pQVar31;
            }
          }
        }
        puVar17 = QHashData::allocateNode(pQVar16,8);
        local_138 = puVar17 + 2;
        *(undefined4 *)local_138 = local_1f8;
        *(int *)((longlong)puVar17 + 0x14) = local_1f4;
        *(undefined4 *)(puVar17 + 3) = local_1f0;
        *(undefined4 *)((longlong)puVar17 + 0x1c) = local_1ec;
        *(undefined4 *)(puVar17 + 4) = local_1e8;
        QString::QString((QString *)(puVar17 + 5),local_1e0);
        puVar17[6] = local_1d8;
        *(int *)(puVar17 + 1) = iVar25;
        *puVar17 = *(undefined8 *)pQVar27;
        *(undefined8 **)pQVar27 = puVar17;
        piVar1 = (int *)(*(longlong *)(param_1 + 0x28) + 0x14);
        *piVar1 = *piVar1 + 1;
      }
      else {
        *(undefined4 *)(pQVar31 + 0x10) = local_1f8;
        *(int *)(pQVar31 + 0x14) = local_1f4;
        *(undefined4 *)(pQVar31 + 0x18) = local_1f0;
        *(undefined4 *)(pQVar31 + 0x1c) = local_1ec;
        *(undefined4 *)(pQVar31 + 0x20) = local_1e8;
        QString::operator=((QString *)(pQVar31 + 0x28),local_1e0);
        *(undefined8 *)(pQVar31 + 0x30) = local_1d8;
      }
      pcVar10 = local_1b8;
      uVar24 = *(uint *)(local_1b8 + 0x200);
      puVar17 = *(undefined8 **)(lVar11 + 0x30);
      puVar18 = puVar17;
      if (*(uint *)(puVar17 + 4) != 0) {
        puVar18 = (undefined8 *)
                  (puVar17[1] + ((ulonglong)uVar24 % (ulonglong)*(uint *)(puVar17 + 4)) * 8);
        for (puVar6 = (undefined8 *)*puVar18;
            (puVar6 != puVar17 && (uVar24 != *(uint *)(puVar6 + 1))); puVar6 = (undefined8 *)*puVar6
            ) {
          puVar18 = puVar6;
        }
        puVar18 = (undefined8 *)*puVar18;
      }
      if (puVar18 == puVar17) {
        FUN_140027a00((ulonglong)uVar24,local_c8,0x80);
        uVar24 = *(uint *)(pcVar10 + 0x200);
        local_1c0 = QString::fromAscii_helper(local_c8,-1);
        pQVar16 = *(QHashData **)(lVar11 + 0x30);
        if (*(int *)(*(longlong *)(lVar11 + 0x30) + 0x10) != 1) {
          pQVar16 = QHashData::detach_helper2
                              (pQVar16,(_func_void_Node_ptr_void_ptr *)&LAB_1400311f0,
                               (_func_void_Node_ptr *)&LAB_14002b600,0x18,8);
          LOCK();
          piVar1 = (int *)(*(longlong *)(lVar11 + 0x30) + 0x10);
          iVar35 = *piVar1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (iVar35 == 1) {
            QHashData::free_helper
                      (*(QHashData **)(lVar11 + 0x30),(_func_void_Node_ptr *)&LAB_14002b600);
          }
          *(QHashData **)(lVar11 + 0x30) = pQVar16;
        }
        uVar4 = *(uint *)(pQVar16 + 0x20);
        pQVar27 = pQVar16;
        if (uVar4 == 0) {
          pQVar31 = (QHashData *)(lVar11 + 0x30);
        }
        else {
          pQVar31 = (QHashData *)
                    (*(longlong *)(pQVar16 + 8) + ((ulonglong)uVar24 % (ulonglong)uVar4) * 8);
          pQVar21 = *(QHashData **)pQVar31;
          if (*(QHashData **)pQVar31 != pQVar16) {
            do {
              pQVar22 = pQVar21;
              pQVar27 = pQVar16;
              if (uVar24 == *(uint *)(pQVar22 + 8)) break;
              pQVar16 = *(QHashData **)(lVar11 + 0x30);
              pQVar21 = *(QHashData **)pQVar22;
              pQVar27 = pQVar16;
              pQVar31 = pQVar22;
            } while (*(QHashData **)pQVar22 != pQVar16);
          }
          pQVar16 = *(QHashData **)pQVar31;
        }
        if (pQVar16 == pQVar27) {
          if ((int)uVar4 <= *(int *)(pQVar27 + 0x14)) {
            QHashData::rehash(pQVar27,*(short *)(pQVar27 + 0x1e) + 1);
            pQVar27 = *(QHashData **)(lVar11 + 0x30);
            if (*(uint *)(pQVar27 + 0x20) == 0) {
              pQVar31 = (QHashData *)(lVar11 + 0x30);
            }
            else {
              pQVar31 = (QHashData *)
                        (*(longlong *)(pQVar27 + 8) +
                        ((ulonglong)uVar24 % (ulonglong)*(uint *)(pQVar27 + 0x20)) * 8);
              for (pQVar16 = *(QHashData **)pQVar31;
                  (pQVar16 != pQVar27 && (uVar24 != *(uint *)(pQVar16 + 8)));
                  pQVar16 = *(QHashData **)pQVar16) {
                pQVar31 = pQVar16;
              }
            }
          }
          puVar17 = QHashData::allocateNode(pQVar27,8);
          QString::QString((QString *)(puVar17 + 2),(QString *)&local_1c0);
          *(uint *)(puVar17 + 1) = uVar24;
          *puVar17 = *(undefined8 *)pQVar31;
          *(undefined8 **)pQVar31 = puVar17;
          piVar1 = (int *)(*(longlong *)(lVar11 + 0x30) + 0x14);
          *piVar1 = *piVar1 + 1;
        }
        else {
          QString::operator=((QString *)(pQVar16 + 0x10),(QString *)&local_1c0);
        }
        QString::~QString((QString *)&local_1c0);
      }
      uVar34 = (ulonglong)(iVar25 + 1U);
      param_2 = local_1b8 + 0x218;
      lVar33 = local_170;
      local_1b8 = param_2;
    } while (iVar25 + 1U < local_20c);
  }
  iVar35 = 0;
  local_1a8 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  local_1b0 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  uVar19 = QString::QString(local_148);
  FUN_140038440(*(undefined8 *)(lVar33 + 0x48),0,lVar33 + 0x38,uVar19);
  QString::~QString(local_148);
  pcVar8 = local_1b0;
  LOCK();
  iVar25 = *(int *)local_1b0;
  *(int *)local_1b0 = *(int *)local_1b0 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    pQVar2 = (QIcon *)(local_1b0 + ((longlong)*(int *)(local_1b0 + 8) + 3) * 8);
    pQVar28 = (QIcon *)(local_1b0 + ((longlong)*(int *)(local_1b0 + 0xc) + 3) * 8);
    while (pQVar28 != pQVar2) {
      pQVar28 = pQVar28 + -8;
      QIcon::~QIcon(pQVar28);
    }
    qFree(pcVar8);
  }
  pcVar8 = local_1a8;
  LOCK();
  iVar25 = *(int *)local_1a8;
  *(int *)local_1a8 = *(int *)local_1a8 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    pcVar3 = local_1a8 + ((longlong)*(int *)(local_1a8 + 8) + 3) * 8;
    pcVar29 = local_1a8 + ((longlong)*(int *)(local_1a8 + 0xc) + 3) * 8;
    while (pcVar29 != pcVar3) {
      pcVar29 = pcVar29 + -8;
      pQVar5 = *(QVariant **)pcVar29;
      if (pQVar5 != (QVariant *)0x0) {
        QVariant::~QVariant(pQVar5);
        free(pQVar5);
      }
    }
    qFree(pcVar8);
  }
  puVar17 = (undefined8 *)FUN_140032eb0(lVar33 + 0x30,&local_168);
  local_230 = (code *)*puVar17;
  LOCK();
  *(int *)local_230 = *(int *)local_230 + 1;
  UNLOCK();
  if (((byte)local_230[0x10] & 1) == 0) {
    FUN_140030d50(&local_230,*(int *)(local_230 + 4));
  }
  pcVar3 = local_230;
  pcVar8 = local_248;
  if (local_248 != local_230) {
    LOCK();
    *(int *)local_230 = *(int *)local_230 + 1;
    UNLOCK();
    LOCK();
    iVar25 = *(int *)local_248;
    *(int *)local_248 = *(int *)local_248 + -1;
    UNLOCK();
    if (iVar25 == 1) {
      pQVar14 = (QString *)(local_248 + ((longlong)*(int *)(local_248 + 8) + 3) * 8);
      pQVar30 = (QString *)(local_248 + ((longlong)*(int *)(local_248 + 0xc) + 3) * 8);
      while (pQVar30 != pQVar14) {
        pQVar30 = pQVar30 + -8;
        QString::~QString(pQVar30);
      }
      qFree(pcVar8);
    }
    local_248 = pcVar3;
    if (((byte)pcVar3[0x10] & 1) == 0) {
      FUN_140030d50(&local_248,*(int *)(pcVar3 + 4));
    }
  }
  pcVar8 = local_230;
  LOCK();
  iVar25 = *(int *)local_230;
  *(int *)local_230 = *(int *)local_230 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    pQVar14 = (QString *)(local_230 + ((longlong)*(int *)(local_230 + 8) + 3) * 8);
    pQVar30 = (QString *)(local_230 + ((longlong)*(int *)(local_230 + 0xc) + 3) * 8);
    while (pQVar30 != pQVar14) {
      pQVar30 = pQVar30 + -8;
      QString::~QString(pQVar30);
    }
    qFree(pcVar8);
  }
  LOCK();
  iVar25 = *local_168;
  *local_168 = *local_168 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    iVar25 = local_168[2];
    pQVar14 = (QString *)(local_168 + ((longlong)local_168[3] + 3) * 2);
    while (pQVar14 != (QString *)(local_168 + ((longlong)iVar25 + 3) * 2)) {
      pQVar14 = pQVar14 + -8;
      QString::~QString(pQVar14);
    }
    qFree(local_168);
  }
  local_228 = (QHashData *)shared_null_exref;
  LOCK();
  *(int *)(shared_null_exref + 0x10) = *(int *)(shared_null_exref + 0x10) + 1;
  UNLOCK();
  local_210 = 1;
  iVar25 = *(int *)(local_248 + 0xc) - *(int *)(local_248 + 8);
  FUN_140030c50(&local_228);
  if (iVar25 < 1) {
    iVar25 = 1;
  }
  QHashData::rehash(local_228,-iVar25);
  iVar25 = *(int *)(local_248 + 8);
  if (0 < *(int *)(local_248 + 0xc) - iVar25) {
    do {
      FUN_1400316b0(&local_228,local_130,local_248 + ((longlong)iVar25 + (longlong)iVar35 + 3) * 8,
                    local_238);
      iVar35 = iVar35 + 1;
      iVar25 = *(int *)(local_248 + 8);
    } while (iVar35 < *(int *)(local_248 + 0xc) - iVar25);
  }
  FUN_14002db80(&local_218,&local_228);
  LOCK();
  pQVar16 = local_228 + 0x10;
  iVar25 = *(int *)pQVar16;
  *(int *)pQVar16 = *(int *)pQVar16 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    QHashData::free_helper(local_228,(_func_void_Node_ptr *)&LAB_14002b600);
  }
  pcVar8 = shared_null_exref;
  local_240 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  local_210 = 2;
  if (*(int *)(pcVar8 + 4) < *(int *)(local_218 + 0x14)) {
    if (*(int *)pcVar8 == 1) {
      QListData::realloc((QListData *)&local_240,*(int *)(local_218 + 0x14));
    }
    else {
      FUN_140030d50();
    }
  }
  pNVar9 = local_218;
  plVar23 = *(longlong **)(local_218 + 8);
  iVar25 = *(int *)(local_218 + 0x20);
  while (iVar25 != 0) {
    iVar25 = iVar25 + -1;
    pNVar20 = (Node *)*plVar23;
    if (pNVar20 != local_218) goto LAB_14002f480;
    plVar23 = plVar23 + 1;
  }
LAB_14002f4a4:
  LOCK();
  *(int *)local_240 = *(int *)local_240 + 1;
  UNLOCK();
  local_220 = local_240;
  if (((byte)local_240[0x10] & 1) == 0) {
    FUN_140030d50(&local_220,*(int *)(local_240 + 4));
  }
  pcVar3 = local_220;
  pcVar8 = local_248;
  if (local_248 != local_220) {
    LOCK();
    *(int *)local_220 = *(int *)local_220 + 1;
    UNLOCK();
    LOCK();
    iVar25 = *(int *)local_248;
    *(int *)local_248 = *(int *)local_248 + -1;
    UNLOCK();
    if (iVar25 == 1) {
      pQVar14 = (QString *)(local_248 + ((longlong)*(int *)(local_248 + 8) + 3) * 8);
      pQVar30 = (QString *)(local_248 + ((longlong)*(int *)(local_248 + 0xc) + 3) * 8);
      while (pQVar30 != pQVar14) {
        pQVar30 = pQVar30 + -8;
        QString::~QString(pQVar30);
      }
      qFree(pcVar8);
    }
    local_248 = pcVar3;
    if (((byte)pcVar3[0x10] & 1) == 0) {
      FUN_140030d50(&local_248,*(int *)(pcVar3 + 4));
    }
  }
  pcVar8 = local_220;
  LOCK();
  iVar25 = *(int *)local_220;
  *(int *)local_220 = *(int *)local_220 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    pQVar14 = (QString *)(local_220 + ((longlong)*(int *)(local_220 + 8) + 3) * 8);
    pQVar30 = (QString *)(local_220 + ((longlong)*(int *)(local_220 + 0xc) + 3) * 8);
    while (pQVar30 != pQVar14) {
      pQVar30 = pQVar30 + -8;
      QString::~QString(pQVar30);
    }
    qFree(pcVar8);
  }
  pcVar8 = local_240;
  LOCK();
  iVar25 = *(int *)local_240;
  *(int *)local_240 = *(int *)local_240 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    pQVar14 = (QString *)(local_240 + ((longlong)*(int *)(local_240 + 8) + 3) * 8);
    pQVar30 = (QString *)(local_240 + ((longlong)*(int *)(local_240 + 0xc) + 3) * 8);
    while (pQVar30 != pQVar14) {
      pQVar30 = pQVar30 + -8;
      QString::~QString(pQVar30);
    }
    qFree(pcVar8);
  }
  FUN_140030270(&local_248,0);
  local_198 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  local_1a0 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  uVar19 = QString::QString(local_140);
  FUN_140038440(*(undefined8 *)(lVar33 + 0x48),2,&local_248,uVar19);
  QString::~QString(local_140);
  pcVar8 = local_1a0;
  LOCK();
  iVar25 = *(int *)local_1a0;
  *(int *)local_1a0 = *(int *)local_1a0 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    pQVar2 = (QIcon *)(local_1a0 + ((longlong)*(int *)(local_1a0 + 8) + 3) * 8);
    pQVar28 = (QIcon *)(local_1a0 + ((longlong)*(int *)(local_1a0 + 0xc) + 3) * 8);
    while (pQVar28 != pQVar2) {
      pQVar28 = pQVar28 + -8;
      QIcon::~QIcon(pQVar28);
    }
    qFree(pcVar8);
  }
  pcVar8 = local_198;
  LOCK();
  iVar25 = *(int *)local_198;
  *(int *)local_198 = *(int *)local_198 + -1;
  UNLOCK();
  param_3 = local_20c;
  if (iVar25 == 1) {
    pcVar3 = local_198 + ((longlong)*(int *)(local_198 + 8) + 3) * 8;
    pcVar29 = local_198 + ((longlong)*(int *)(local_198 + 0xc) + 3) * 8;
    while (pcVar29 != pcVar3) {
      pcVar29 = pcVar29 + -8;
      pQVar5 = *(QVariant **)pcVar29;
      if (pQVar5 != (QVariant *)0x0) {
        QVariant::~QVariant(pQVar5);
        free(pQVar5);
      }
    }
    qFree(pcVar8);
    param_3 = local_20c;
  }
LAB_14002f715:
  if (param_3 != 0) {
    FUN_14002fca0(lVar33,&local_200);
  }
  pcVar8 = local_248;
  LOCK();
  iVar25 = *(int *)local_248;
  *(int *)local_248 = *(int *)local_248 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    pQVar14 = (QString *)(local_248 + ((longlong)*(int *)(local_248 + 8) + 3) * 8);
    pQVar30 = (QString *)(local_248 + ((longlong)*(int *)(local_248 + 0xc) + 3) * 8);
    while (pQVar30 != pQVar14) {
      pQVar30 = pQVar30 + -8;
      QString::~QString(pQVar30);
    }
    qFree(pcVar8);
  }
  LOCK();
  pQVar16 = (QHashData *)(local_218 + 0x10);
  iVar25 = *(int *)pQVar16;
  *(int *)pQVar16 = *(int *)pQVar16 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    QHashData::free_helper((QHashData *)local_218,(_func_void_Node_ptr *)&LAB_14002b600);
  }
  LOCK();
  iVar25 = *(int *)local_200;
  *(int *)local_200 = *(int *)local_200 + -1;
  UNLOCK();
  if (iVar25 == 1) {
    qFree(local_200);
  }
  QString::~QString(local_1e0);
  return;
LAB_14002f480:
  do {
    FUN_140030840(&local_240,pNVar20 + 0x10);
    pNVar20 = QHashData::nextNode(pNVar20);
  } while (pNVar20 != pNVar9);
  goto LAB_14002f4a4;
}


/* ================= FUN_14002f800 @ 14002f800  size=229  callers=1 callees=4 ================= */
/* CALLERS: FUN_14002fc20 */
/* CALLEES: FUN_14002db20, FUN_1400307f0, FUN_140030cd0, qFree */

void FUN_14002f800(longlong param_1)

{
  int iVar1;
  int *piVar2;
  code *pcVar3;
  ulonglong uVar4;
  uint uVar5;
  ulonglong uVar6;
  undefined4 local_res8 [2];
  
  pcVar3 = shared_null_exref;
  if (*(int *)(*(longlong *)(param_1 + 0x28) + 0xc) == *(int *)(*(longlong *)(param_1 + 0x28) + 8))
  {
    if (*(int *)(*(longlong *)(param_1 + 0x30) + 0xc) == *(int *)(*(longlong *)(param_1 + 0x30) + 8)
       ) {
      FUN_14002db20(param_1 + 0x20,param_1 + 0x18);
    }
  }
  else {
    LOCK();
    *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
    UNLOCK();
    piVar2 = *(int **)(param_1 + 0x20);
    *(code **)(param_1 + 0x20) = pcVar3;
    LOCK();
    iVar1 = *piVar2;
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (iVar1 == 1) {
      qFree(piVar2);
    }
    uVar4 = 0;
    uVar6 = uVar4;
    if (0 < *(int *)(*(longlong *)(param_1 + 0x18) + 0xc) -
            *(int *)(*(longlong *)(param_1 + 0x18) + 8)) {
      do {
        if (**(int **)(param_1 + 0x18) != 1) {
          FUN_140030cd0(param_1 + 0x18,*(undefined4 *)(*(longlong *)(param_1 + 0x18) + 4));
        }
        local_res8[0] =
             *(undefined4 *)
              (*(longlong *)(param_1 + 0x18) + 0x18 +
              ((longlong)*(int *)(*(longlong *)(param_1 + 0x18) + 8) + uVar4) * 8);
        FUN_1400307f0(param_1 + 0x20,local_res8);
        uVar5 = (int)uVar6 + 1;
        uVar4 = uVar4 + 1;
        uVar6 = (ulonglong)uVar5;
      } while ((int)uVar5 <
               *(int *)(*(longlong *)(param_1 + 0x18) + 0xc) -
               *(int *)(*(longlong *)(param_1 + 0x18) + 8));
    }
  }
  return;
}


/* ================= FUN_14002f8f0 @ 14002f8f0  size=140  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002bc30 */
/* CALLEES: FUN_140027140, FUN_14002fad0 */

void FUN_14002f8f0(longlong param_1,longlong param_2)

{
  FUN_140027140(param_1,*(undefined8 *)(param_2 + 8),0x80);
  FUN_140027140(param_1 + 0x80,*(undefined8 *)(param_2 + 0x28),0x80);
  *(undefined4 *)(param_1 + 0x214) = 0;
  *(undefined4 *)(param_1 + 0x200) = *(undefined4 *)(param_2 + 0x230);
  *(undefined4 *)(param_1 + 0x20c) = *(undefined4 *)(param_2 + 0x18);
  *(undefined4 *)(param_1 + 0x210) = *(undefined4 *)(param_2 + 0x24);
  *(undefined4 *)(param_1 + 0x204) = *(undefined4 *)(param_2 + 0x14);
  FUN_14002fad0(param_2,param_1 + 0x100,0x100,param_1 + 0x208);
  return;
}


/* ================= FUN_14002f980 @ 14002f980  size=62  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003b350 */
/* CALLEES: FUN_14002e9a0, FUN_14002fb50, invalidate */

void FUN_14002f980(QSortFilterProxyModel *param_1)

{
  undefined4 uVar1;
  
  QSortFilterProxyModel::invalidate(param_1);
  uVar1 = FUN_14002e9a0(*(undefined8 *)(param_1 + 0x10));
  FUN_14002fb50(*(undefined8 *)(param_1 + 0x10),uVar1);
  FUN_14002fb50(*(undefined8 *)(param_1 + 0x10),uVar1);
  return;
}


/* ================= FUN_14002f9c0 @ 14002f9c0  size=186  callers=1 callees=9 ================= */
/* CALLERS: FUN_14003b350 */
/* CALLEES: FUN_140037fc0, FUN_140038030, QString, currentText, findText, invalidate, operator=, setCurrentIndex, ~QString */

void FUN_14002f9c0(QSortFilterProxyModel *param_1)

{
  char cVar1;
  int iVar2;
  QComboBox *this;
  QString *pQVar3;
  undefined4 local_res8 [2];
  QString local_res10 [8];
  
  QString::QString(local_res10);
  cVar1 = FUN_140038030(*(undefined8 *)(*(longlong *)(param_1 + 0x10) + 0x48),0);
  if (cVar1 != '\0') {
    this = (QComboBox *)FUN_140037fc0(*(undefined8 *)(*(longlong *)(param_1 + 0x10) + 0x48),0);
    if (this != (QComboBox *)0x0) {
      pQVar3 = (QString *)QComboBox::currentText(this);
      QString::operator=(local_res10,pQVar3);
      QString::~QString((QString *)local_res8);
      local_res8[0] = 8;
      iVar2 = QComboBox::findText(this,local_res10,local_res8);
      if (-1 < iVar2) {
        QComboBox::setCurrentIndex(this,iVar2);
        param_1[0x20] = (QSortFilterProxyModel)0x1;
        QSortFilterProxyModel::invalidate(param_1);
      }
    }
  }
  QString::~QString(local_res10);
  return;
}


/* ================= FUN_14002fa80 @ 14002fa80  size=53  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b350 */
/* CALLEES: FUN_140038030 */

void FUN_14002fa80(QSortFilterProxyModel *param_1)

{
  char cVar1;
  
  cVar1 = FUN_140038030(*(undefined8 *)(*(longlong *)(param_1 + 0x10) + 0x48),0);
  if (cVar1 != '\0') {
    param_1[0x20] = (QSortFilterProxyModel)0x0;
                    /* WARNING: Could not recover jumptable at 0x00014002faa8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    QSortFilterProxyModel::invalidate(param_1);
    return;
  }
  return;
}


/* ================= FUN_14002fad0 @ 14002fad0  size=118  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002f8f0 */
/* CALLEES: FUN_1400273a0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14002fad0(longlong param_1,undefined8 param_2,undefined4 param_3,undefined4 *param_4)

{
  longlong lVar1;
  undefined4 uVar2;
  longlong *plVar3;
  ulonglong uVar4;
  undefined1 auStack_138 [32];
  longlong local_118 [32];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_138;
  plVar3 = (longlong *)(param_1 + 0x238);
  uVar4 = 0;
  lVar1 = *plVar3;
  while (lVar1 != 0) {
    if (plVar3[3] != 0) {
      local_118[uVar4] = plVar3[3];
      uVar4 = (ulonglong)((int)uVar4 + 1);
    }
    plVar3 = plVar3 + 5;
    lVar1 = *plVar3;
  }
  uVar2 = FUN_1400273a0(local_118,uVar4,param_2,param_3);
  if (param_4 != (undefined4 *)0x0) {
    *param_4 = uVar2;
  }
  return;
}


/* ================= FUN_14002fb50 @ 14002fb50  size=205  callers=3 callees=2 ================= */
/* CALLERS: FUN_14002f980, FUN_14002fcd0, FUN_140032b20 */
/* CALLEES: FUN_14002e740, ~QModelIndex */

void FUN_14002fb50(longlong param_1,undefined4 param_2)

{
  undefined8 *puVar1;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 uStack_48;
  undefined8 local_40;
  undefined8 uStack_38;
  undefined8 uStack_30;
  QModelIndex local_28 [32];
  
  local_40 = 0xffffffffffffffff;
  uStack_38 = 0;
  uStack_30 = 0;
  local_58 = 0xffffffffffffffff;
  uStack_50 = 0;
  uStack_48 = 0;
  puVar1 = (undefined8 *)FUN_14002e740(*(undefined8 *)(param_1 + 0xc0),local_28,param_2);
  local_40 = *puVar1;
  uStack_38 = puVar1[1];
  uStack_30 = puVar1[2];
  QModelIndex::~QModelIndex(local_28);
  puVar1 = (undefined8 *)
           (**(code **)(**(longlong **)(param_1 + 0xb8) + 0x148))
                     (*(longlong **)(param_1 + 0xb8),local_28,&local_40);
  local_58 = *puVar1;
  uStack_50 = puVar1[1];
  uStack_48 = puVar1[2];
  QModelIndex::~QModelIndex(local_28);
  (**(code **)(**(longlong **)(param_1 + 0x88) + 0x1c0))(*(longlong **)(param_1 + 0x88),&local_58,3)
  ;
  QModelIndex::~QModelIndex((QModelIndex *)&local_58);
  QModelIndex::~QModelIndex((QModelIndex *)&local_40);
  return;
}


/* ================= FUN_14002fc20 @ 14002fc20  size=123  callers=1 callees=4 ================= */
/* CALLERS: FUN_14002fca0 */
/* CALLEES: FUN_14002f800, FUN_140030cd0, beginResetModel, qFree */

void FUN_14002fc20(QAbstractItemModel *param_1,undefined8 *param_2)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  
  QAbstractItemModel::beginResetModel(param_1);
  piVar2 = (int *)*param_2;
  if (*(int **)(param_1 + 0x18) != piVar2) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    piVar3 = *(int **)(param_1 + 0x18);
    LOCK();
    iVar1 = *piVar3;
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (iVar1 == 1) {
      qFree(*(void **)(param_1 + 0x18));
    }
    *(int **)(param_1 + 0x18) = piVar2;
    if ((*(byte *)(piVar2 + 4) & 1) == 0) {
      FUN_140030cd0(param_1 + 0x18,piVar2[1]);
    }
  }
  FUN_14002f800(param_1);
                    /* WARNING: Could not recover jumptable at 0x00014002fc94. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QAbstractItemModel::endResetModel(param_1);
  return;
}


/* ================= FUN_14002fca0 @ 14002fca0  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: FUN_14002fc20 */

void FUN_14002fca0(longlong param_1)

{
  FUN_14002fc20(*(undefined8 *)(param_1 + 0xc0));
  return;
}


/* ================= FUN_14002fcb0 @ 14002fcb0  size=29  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002d5c0 */
/* CALLEES: FUN_14002e640, FUN_14002fcd0 */

void FUN_14002fcb0(undefined8 param_1)

{
  undefined4 uVar1;
  
  uVar1 = FUN_14002e640();
  FUN_14002fcd0(param_1,uVar1);
  return;
}


/* ================= FUN_14002fcd0 @ 14002fcd0  size=220  callers=2 callees=4 ================= */
/* CALLERS: FUN_14002d5c0, FUN_14002fcb0 */
/* CALLEES: FUN_14002e740, FUN_14002fb50, setCurrentIndex, ~QModelIndex */

void FUN_14002fcd0(longlong param_1,undefined4 param_2)

{
  undefined8 *puVar1;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 uStack_48;
  undefined8 local_40;
  undefined8 uStack_38;
  undefined8 uStack_30;
  QModelIndex local_28 [32];
  
  local_40 = 0xffffffffffffffff;
  uStack_38 = 0;
  uStack_30 = 0;
  local_58 = 0xffffffffffffffff;
  uStack_50 = 0;
  uStack_48 = 0;
  puVar1 = (undefined8 *)FUN_14002e740(*(undefined8 *)(param_1 + 0xc0),local_28,param_2);
  local_40 = *puVar1;
  uStack_38 = puVar1[1];
  uStack_30 = puVar1[2];
  QModelIndex::~QModelIndex(local_28);
  puVar1 = (undefined8 *)
           (**(code **)(**(longlong **)(param_1 + 0xb8) + 0x148))
                     (*(longlong **)(param_1 + 0xb8),local_28,&local_40);
  local_58 = *puVar1;
  uStack_50 = puVar1[1];
  uStack_48 = puVar1[2];
  QModelIndex::~QModelIndex(local_28);
  QAbstractItemView::setCurrentIndex
            (*(QAbstractItemView **)(param_1 + 0x88),(QModelIndex *)&local_58);
  FUN_14002fb50(param_1,param_2);
  QModelIndex::~QModelIndex((QModelIndex *)&local_58);
  QModelIndex::~QModelIndex((QModelIndex *)&local_40);
  return;
}


/* ================= FUN_14002fdb0 @ 14002fdb0  size=129  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: connect */

void FUN_14002fdb0(QObject *param_1)

{
  QObject::connect(*(QObject **)(param_1 + 0xa8),"2pressed()",param_1,"1accept()",0);
  QObject::connect(*(QObject **)(param_1 + 0xb0),"2pressed()",param_1,"1reject()",0);
  QObject::connect(*(QObject **)(param_1 + 0x88),"2activated(const QModelIndex&)",param_1,
                   "1OnTableCellActivated()",0);
  return;
}


/* ================= FUN_14002fe40 @ 14002fe40  size=618  callers=1 callees=21 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: FUN_14002d480, FUN_14002d560, FUN_1400300b0, FUN_1400321b0, FUN_140032220, FUN_140037a90, FUN_1400388c0, FUN_140058d94, connect, horizontalHeader, setAutoDefault, setClickable, setDefault, setEditTriggers, setHorizontalHeader, setMovable, setSelectionBehavior, setSortIndicator, setSortIndicatorShown, setSortingEnabled, setStretchLastSection */

void FUN_14002fe40(longlong param_1)

{
  undefined8 uVar1;
  longlong *plVar2;
  QHeaderView *pQVar3;
  longlong local_res8 [4];
  
  FUN_140032220(param_1 + 0x50,param_1);
  FUN_1400300b0(param_1);
  local_res8[0] = FUN_140058d94(0x40);
  uVar1 = 0;
  if (local_res8[0] != 0) {
    uVar1 = FUN_14002d480(local_res8[0],*(undefined8 *)(param_1 + 0x88),param_1);
  }
  *(undefined8 *)(param_1 + 0xc0) = uVar1;
  local_res8[0] = FUN_140058d94();
  if (local_res8[0] == 0) {
    plVar2 = (longlong *)0x0;
  }
  else {
    plVar2 = (longlong *)FUN_14002d560(local_res8[0],param_1);
  }
  *(longlong **)(param_1 + 0xb8) = plVar2;
  (**(code **)(*plVar2 + 0x138))(plVar2,*(undefined8 *)(param_1 + 0xc0));
  (**(code **)(**(longlong **)(param_1 + 0x88) + 0x1a0))
            (*(longlong **)(param_1 + 0x88),*(undefined8 *)(param_1 + 0xb8));
  QTableView::setSortingEnabled(*(QTableView **)(param_1 + 0x88),true);
  QAbstractItemView::setSelectionBehavior(*(QAbstractItemView **)(param_1 + 0x88),1);
  local_res8[0] = CONCAT44(local_res8[0]._4_4_,0x1f);
  QAbstractItemView::setEditTriggers(*(QAbstractItemView **)(param_1 + 0x88),local_res8);
  local_res8[0] = FUN_140058d94(0x58);
  uVar1 = 0;
  if (local_res8[0] != 0) {
    uVar1 = FUN_140037a90(local_res8[0],0xffffffff,1,*(undefined8 *)(param_1 + 0x88));
  }
  *(undefined8 *)(param_1 + 0x48) = uVar1;
  FUN_1400388c0(uVar1,5);
  QObject::connect(*(QObject **)(param_1 + 0x48),"2SignalFilterChanged()",
                   *(QObject **)(param_1 + 0xb8),"1OnColFilterChanged()",0);
  QObject::connect(*(QObject **)(param_1 + 0x48),"2SignalSelectionChanged()",
                   *(QObject **)(param_1 + 0xb8),"1OnColSelectionChanged()",0);
  QObject::connect(*(QObject **)(param_1 + 0x48),"2SignalTextChanged()",
                   *(QObject **)(param_1 + 0xb8),"1OnColTextChanged()",0);
  QTableView::setHorizontalHeader
            (*(QTableView **)(param_1 + 0x88),*(QHeaderView **)(param_1 + 0x48));
  pQVar3 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x88));
  QHeaderView::setMovable(pQVar3,true);
  pQVar3 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x88));
  QHeaderView::setClickable(pQVar3,true);
  pQVar3 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x88));
  QHeaderView::setSortIndicatorShown(pQVar3,true);
  pQVar3 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x88));
  QHeaderView::setSortIndicator(pQVar3,0,0);
  pQVar3 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x88));
  QHeaderView::setStretchLastSection(pQVar3,true);
  QPushButton::setDefault(*(QPushButton **)(param_1 + 0xa8),false);
  QPushButton::setDefault(*(QPushButton **)(param_1 + 0xb0),false);
  QPushButton::setAutoDefault(*(QPushButton **)(param_1 + 0xa8),false);
  QPushButton::setAutoDefault(*(QPushButton **)(param_1 + 0xb0),false);
  FUN_1400321b0(param_1,*(longlong *)(param_1 + 0x20) + 0x38);
  return;
}


/* ================= FUN_1400300b0 @ 1400300b0  size=440  callers=1 callees=12 ================= */
/* CALLERS: FUN_14002fe40 */
/* CALLEES: FUN_140030cd0, QFont, QString, font, fromAscii_helper, operator=, qFree, qt_qFindChildren_helper, setFont, setStyleSheet, ~QFont, ~QString */

void FUN_1400300b0(QWidget *param_1)

{
  int iVar1;
  code *pcVar2;
  QFont *pQVar3;
  QString *pQVar4;
  ulonglong uVar5;
  uint uVar6;
  ulonglong uVar7;
  code *local_res10;
  code *local_res18;
  Data *local_res20;
  QString local_58 [8];
  QFont local_50 [16];
  QFont local_40 [24];
  
  uVar5 = 0;
  local_res10 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  QFont::QFont(local_50);
  if (param_1 != (QWidget *)0x0) {
    pQVar3 = (QFont *)QApplication::font();
    QFont::operator=(local_50,pQVar3);
    QFont::~QFont(local_40);
    QWidget::setFont(param_1,local_50);
    pQVar4 = (QString *)QString::QString(local_58);
    local_res18 = shared_null_exref;
    LOCK();
    *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
    UNLOCK();
    qt_qFindChildren_helper
              ((QObject *)param_1,pQVar4,(QRegExp *)0x0,(QMetaObject *)staticMetaObject_exref,
               (QList<void*___ptr64> *)&local_res18);
    pcVar2 = local_res10;
    local_res10 = local_res18;
    local_res18 = pcVar2;
    LOCK();
    iVar1 = *(int *)pcVar2;
    *(int *)pcVar2 = *(int *)pcVar2 + -1;
    UNLOCK();
    if (iVar1 == 1) {
      qFree(pcVar2);
    }
    QString::~QString(local_58);
    uVar7 = uVar5;
    if (0 < *(int *)(local_res10 + 0xc) - *(int *)(local_res10 + 8)) {
      do {
        if (*(int *)local_res10 != 1) {
          FUN_140030cd0(&local_res10,*(int *)(local_res10 + 4));
        }
        if (*(longlong *)(local_res10 + ((longlong)*(int *)(local_res10 + 8) + uVar5) * 8 + 0x18) !=
            0) {
          if (*(int *)local_res10 != 1) {
            FUN_140030cd0(&local_res10,*(int *)(local_res10 + 4));
          }
          QWidget::setFont(*(QWidget **)
                            (local_res10 + ((longlong)*(int *)(local_res10 + 8) + uVar5) * 8 + 0x18)
                           ,local_50);
        }
        uVar6 = (int)uVar7 + 1;
        uVar5 = uVar5 + 1;
        uVar7 = (ulonglong)uVar6;
      } while ((int)uVar6 < *(int *)(local_res10 + 0xc) - *(int *)(local_res10 + 8));
    }
    local_res20 = QString::fromAscii_helper("gridline-color: gray;",-1);
    QWidget::setStyleSheet(param_1,(QString *)&local_res20);
    QString::~QString((QString *)&local_res20);
  }
  QFont::~QFont(local_50);
  LOCK();
  iVar1 = *(int *)local_res10;
  *(int *)local_res10 = *(int *)local_res10 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    qFree(local_res10);
  }
  return;
}


/* ================= FUN_140030270 @ 140030270  size=1260  callers=1 callees=12 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: FUN_140030840, FUN_140030d50, FUN_140030f10, FUN_140031f40, __security_check_cookie, continueFreeData, operator<, operator=, qFree, realloc, toLower, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140030270(longlong *param_1,char param_2)

{
  int iVar1;
  int *piVar2;
  void *pvVar3;
  QMapData *pQVar4;
  QMapData *pQVar5;
  code *pcVar6;
  QMapData *pQVar7;
  bool bVar8;
  int iVar9;
  QString *pQVar10;
  QString *pQVar11;
  QMapData *pQVar12;
  longlong lVar13;
  undefined1 auStack_158 [32];
  QString *local_138;
  char local_128;
  QMapData *local_120;
  code *local_118;
  code *local_110;
  undefined4 local_108;
  QString local_100 [8];
  longlong local_f8 [12];
  longlong local_98 [12];
  ulonglong local_38;
  
  local_38 = DAT_140096fe8 ^ (ulonglong)auStack_158;
  iVar9 = 0;
  local_118 = (code *)((ulonglong)local_118 & 0xffffffff00000000);
  local_108 = 0;
  local_120 = (QMapData *)shared_null_exref;
  LOCK();
  *(int *)(shared_null_exref + 0x68) = *(int *)(shared_null_exref + 0x68) + 1;
  UNLOCK();
  local_128 = param_2;
  if (0 < *(int *)(*param_1 + 0xc) - *(int *)(*param_1 + 8)) {
    do {
      if (local_128 == '\0') {
        if (*(int *)*param_1 != 1) {
          FUN_140030d50(param_1,*(undefined4 *)(*param_1 + 4));
        }
        pQVar10 = (QString *)
                  QString::toLower((QString *)
                                   (*param_1 +
                                    ((longlong)*(int *)(*param_1 + 8) + (longlong)iVar9) * 8 + 0x18)
                                  );
        if (*(int *)*param_1 != 1) {
          FUN_140030d50(param_1,*(undefined4 *)(*param_1 + 4));
        }
        pQVar11 = (QString *)
                  (*param_1 + ((longlong)*(int *)(*param_1 + 8) + 3 + (longlong)iVar9) * 8);
        if (*(int *)(local_120 + 0x68) != 1) {
          FUN_140030f10(&local_120);
        }
        pQVar4 = local_120;
        lVar13 = (longlong)*(int *)(local_120 + 0x6c);
        pQVar12 = local_120;
        if (lVar13 < 0) {
LAB_1400304de:
          local_138 = pQVar11;
          FUN_140031f40(&local_120,pQVar4,local_98,pQVar10);
        }
        else {
          do {
            pQVar7 = *(QMapData **)(pQVar12 + lVar13 * 8 + 8);
            while ((pQVar5 = pQVar7, pQVar5 != pQVar4 &&
                   (bVar8 = QString::operator<((QString *)(pQVar5 + -0x10),pQVar10), bVar8))) {
              pQVar7 = *(QMapData **)(pQVar5 + lVar13 * 8 + 8);
              pQVar12 = pQVar5;
            }
            local_98[lVar13] = (longlong)pQVar12;
            lVar13 = lVar13 + -1;
          } while (-1 < lVar13);
          if ((pQVar5 == pQVar4) ||
             (bVar8 = QString::operator<(pQVar10,(QString *)(pQVar5 + -0x10)), bVar8))
          goto LAB_1400304de;
          QString::operator=((QString *)(pQVar5 + -8),pQVar11);
        }
        QString::~QString(local_100);
      }
      else {
        if (*(int *)*param_1 != 1) {
          FUN_140030d50(param_1,*(undefined4 *)(*param_1 + 4));
        }
        piVar2 = (int *)*param_1;
        iVar1 = piVar2[2];
        if (*piVar2 != 1) {
          FUN_140030d50(param_1,*(undefined4 *)(*param_1 + 4));
        }
        pQVar10 = (QString *)
                  (*param_1 + ((longlong)*(int *)(*param_1 + 8) + 3 + (longlong)iVar9) * 8);
        if (*(int *)(local_120 + 0x68) != 1) {
          FUN_140030f10(&local_120);
        }
        pQVar4 = local_120;
        lVar13 = (longlong)*(int *)(local_120 + 0x6c);
        pQVar12 = local_120;
        if (-1 < lVar13) {
          do {
            pQVar7 = *(QMapData **)(pQVar12 + lVar13 * 8 + 8);
            while ((pQVar5 = pQVar7, pQVar5 != pQVar4 &&
                   (bVar8 = QString::operator<((QString *)(pQVar5 + -0x10),pQVar10), bVar8))) {
              pQVar7 = *(QMapData **)(pQVar5 + lVar13 * 8 + 8);
              pQVar12 = pQVar5;
            }
            local_f8[lVar13] = (longlong)pQVar12;
            lVar13 = lVar13 + -1;
          } while (-1 < lVar13);
          if ((pQVar5 != pQVar4) &&
             (bVar8 = QString::operator<(pQVar10,(QString *)(pQVar5 + -0x10)), !bVar8)) {
            QString::operator=((QString *)(pQVar5 + -8),
                               (QString *)(piVar2 + ((longlong)iVar1 + 3 + (longlong)iVar9) * 2));
            goto LAB_140030503;
          }
        }
        local_138 = (QString *)(piVar2 + ((longlong)iVar1 + 3 + (longlong)iVar9) * 2);
        FUN_140031f40(&local_120,pQVar4,local_f8,pQVar10);
      }
LAB_140030503:
      iVar9 = (int)local_118 + 1;
      local_118 = (code *)CONCAT44(local_118._4_4_,iVar9);
    } while (iVar9 < *(int *)(*param_1 + 0xc) - *(int *)(*param_1 + 8));
  }
  pcVar6 = shared_null_exref;
  local_118 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  local_108 = 1;
  if (*(int *)(pcVar6 + 4) < *(int *)(local_120 + 0x70)) {
    if (*(int *)pcVar6 == 1) {
      QListData::realloc((QListData *)&local_118,*(int *)(local_120 + 0x70));
    }
    else {
      FUN_140030d50();
    }
  }
  pQVar4 = local_120;
  for (pQVar12 = *(QMapData **)(local_120 + 8); pQVar12 != pQVar4;
      pQVar12 = *(QMapData **)(pQVar12 + 8)) {
    FUN_140030840(&local_118,pQVar12 + -8);
  }
  LOCK();
  *(int *)local_118 = *(int *)local_118 + 1;
  UNLOCK();
  local_110 = local_118;
  if (((byte)local_118[0x10] & 1) == 0) {
    FUN_140030d50(&local_110,*(int *)(local_118 + 4));
  }
  pcVar6 = local_110;
  if ((code *)*param_1 != local_110) {
    LOCK();
    *(int *)local_110 = *(int *)local_110 + 1;
    UNLOCK();
    piVar2 = (int *)*param_1;
    LOCK();
    iVar9 = *piVar2;
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (iVar9 == 1) {
      pvVar3 = (void *)*param_1;
      iVar9 = *(int *)((longlong)pvVar3 + 8);
      pQVar10 = (QString *)
                ((longlong)pvVar3 + (longlong)*(int *)((longlong)pvVar3 + 0xc) * 8 + 0x18);
      while (pQVar10 != (QString *)((longlong)iVar9 * 8 + 0x18 + (longlong)pvVar3)) {
        pQVar10 = pQVar10 + -8;
        QString::~QString(pQVar10);
      }
      qFree(pvVar3);
    }
    *param_1 = (longlong)pcVar6;
    if (((byte)pcVar6[0x10] & 1) == 0) {
      FUN_140030d50(param_1,*(int *)(pcVar6 + 4));
    }
  }
  pcVar6 = local_110;
  LOCK();
  iVar9 = *(int *)local_110;
  *(int *)local_110 = *(int *)local_110 + -1;
  UNLOCK();
  if (iVar9 == 1) {
    pQVar10 = (QString *)(local_110 + ((longlong)*(int *)(local_110 + 8) + 3) * 8);
    pQVar11 = (QString *)(local_110 + ((longlong)*(int *)(local_110 + 0xc) + 3) * 8);
    while (pQVar11 != pQVar10) {
      pQVar11 = pQVar11 + -8;
      QString::~QString(pQVar11);
    }
    qFree(pcVar6);
  }
  pcVar6 = local_118;
  LOCK();
  iVar9 = *(int *)local_118;
  *(int *)local_118 = *(int *)local_118 + -1;
  UNLOCK();
  if (iVar9 == 1) {
    pQVar10 = (QString *)(local_118 + ((longlong)*(int *)(local_118 + 8) + 3) * 8);
    pQVar11 = (QString *)(local_118 + ((longlong)*(int *)(local_118 + 0xc) + 3) * 8);
    while (pQVar11 != pQVar10) {
      pQVar11 = pQVar11 + -8;
      QString::~QString(pQVar11);
    }
    qFree(pcVar6);
  }
  pQVar7 = local_120;
  LOCK();
  pQVar12 = local_120 + 0x68;
  iVar9 = *(int *)pQVar12;
  *(int *)pQVar12 = *(int *)pQVar12 + -1;
  UNLOCK();
  if (iVar9 == 1) {
    pQVar12 = *(QMapData **)(pQVar4 + 8);
    while (pQVar12 != pQVar7) {
      pQVar4 = *(QMapData **)(pQVar12 + 8);
      QString::~QString((QString *)(pQVar12 + -0x10));
      QString::~QString((QString *)(pQVar12 + -8));
      pQVar12 = pQVar4;
    }
    QMapData::continueFreeData(pQVar7,0x10);
  }
  return;
}


/* ================= FUN_140030760 @ 140030760  size=131  callers=0 callees=6 ================= */
/* CALLEES: FUN_14002e3d0, FUN_14002e890, FUN_14002e930, FUN_14002e9a0, operator=, ~QString */

void FUN_140030760(QDialog *param_1)

{
  undefined4 uVar1;
  QString *pQVar2;
  QString local_res8 [32];
  
  param_1[200] = (QDialog)0x1;
  uVar1 = FUN_14002e9a0();
  *(undefined4 *)(param_1 + 0xd4) = uVar1;
  uVar1 = FUN_14002e890(param_1,uVar1);
  *(undefined4 *)(param_1 + 0xcc) = uVar1;
  uVar1 = FUN_14002e3d0(param_1,*(undefined4 *)(param_1 + 0xd4));
  *(undefined4 *)(param_1 + 0xd0) = uVar1;
  uVar1 = FUN_14002e9a0(param_1);
  pQVar2 = (QString *)FUN_14002e930(param_1,local_res8,uVar1);
  QString::operator=((QString *)(param_1 + 0xd8),pQVar2);
  QString::~QString(local_res8);
                    /* WARNING: Could not recover jumptable at 0x0001400307dc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::accept(param_1);
  return;
}


/* ================= FUN_1400307f0 @ 1400307f0  size=79  callers=2 callees=2 ================= */
/* CALLERS: FUN_14002ea10, FUN_14002f800 */
/* CALLEES: FUN_140030fe0, append */

void FUN_1400307f0(QListData *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  void **ppvVar3;
  undefined4 uStackX_1c;
  
  if (**(int **)param_1 != 1) {
    puVar2 = (undefined4 *)FUN_140030fe0(param_1,0x7fffffff,1);
    *puVar2 = *param_2;
    return;
  }
  uVar1 = *param_2;
  ppvVar3 = QListData::append(param_1);
  *ppvVar3 = (void *)CONCAT44(uStackX_1c,uVar1);
  return;
}


/* ================= FUN_140030840 @ 140030840  size=115  callers=4 callees=3 ================= */
/* CALLERS: FUN_14002ea10, FUN_140030270, FUN_140032eb0, FUN_1400507f0 */
/* CALLEES: FUN_1400310c0, QString, append */

void FUN_140030840(QListData *param_1,QString *param_2)

{
  QString *this;
  void **ppvVar1;
  void *local_res18 [2];
  
  if (**(int **)param_1 != 1) {
    this = (QString *)FUN_1400310c0(param_1,0x7fffffff,1);
    QString::QString(this,param_2);
    return;
  }
  QString::QString((QString *)local_res18,param_2);
  ppvVar1 = QListData::append(param_1);
  *ppvVar1 = local_res18[0];
  return;
}


/* ================= FUN_1400308c0 @ 1400308c0  size=124  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002dfd0 */
/* CALLEES: qFree, ~QString */

void FUN_1400308c0(undefined8 *param_1)

{
  int iVar1;
  int *piVar2;
  code *pcVar3;
  QString *this;
  
  pcVar3 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  piVar2 = (int *)*param_1;
  *param_1 = pcVar3;
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    iVar1 = piVar2[2];
    this = (QString *)(piVar2 + ((longlong)piVar2[3] + 3) * 2);
    while (this != (QString *)(piVar2 + ((longlong)iVar1 + 3) * 2)) {
      this = this + -8;
      QString::~QString(this);
    }
    qFree(piVar2);
  }
  return;
}


/* ================= FUN_140030940 @ 140030940  size=188  callers=0 callees=5 ================= */
/* CALLEES: closeEditor, edit, hasFocus, model, ~QModelIndex */

void FUN_140030940(QAbstractItemView *param_1,QWidget *param_2,EndEditHint param_3)

{
  bool bVar1;
  QAbstractItemModel *pQVar2;
  QModelIndex *pQVar3;
  undefined8 local_38;
  undefined8 local_30;
  undefined8 uStack_28;
  QModelIndex local_20 [24];
  
  bVar1 = QWidget::hasFocus(param_2);
  if (param_3 != 3) {
    QAbstractItemView::closeEditor(param_1,param_2,param_3);
  }
  if (bVar1) {
    pQVar2 = QAbstractItemView::model(param_1);
    local_38 = 0xffffffffffffffff;
    local_30 = 0;
    uStack_28 = 0;
    pQVar3 = (QModelIndex *)(**(code **)(*(longlong *)pQVar2 + 0x58))(pQVar2,local_20,0,1,&local_38)
    ;
    QAbstractItemView::edit(param_1,pQVar3);
    QModelIndex::~QModelIndex(local_20);
    QModelIndex::~QModelIndex((QModelIndex *)&local_38);
  }
  return;
}


/* ================= FUN_140030a00 @ 140030a00  size=6  callers=0 callees=0 ================= */

undefined8 FUN_140030a00(void)

{
  return 5;
}


/* ================= FUN_140030a10 @ 140030a10  size=547  callers=0 callees=10 ================= */
/* CALLEES: FUN_14002e2c0, FUN_14002e340, FUN_14002e3e0, FUN_14002e480, FUN_14002e510, FUN_14002e650, FUN_14002e8a0, QVariant, ~QModelIndex, ~QString */

QVariant * FUN_140030a10(longlong *param_1,QVariant *param_2,int *param_3,undefined8 param_4)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  QString *pQVar6;
  int iVar7;
  bool bVar8;
  QString local_res18 [8];
  undefined8 local_30;
  undefined8 local_28;
  undefined8 uStack_20;
  
  bVar8 = false;
  iVar7 = *param_3;
  if (-1 < iVar7) {
    local_30 = 0xffffffffffffffff;
    local_28 = 0;
    uStack_20 = 0;
    bVar8 = true;
    iVar2 = (**(code **)(*param_1 + 0x68))(0,&local_30,param_3,param_4,1);
    if (iVar7 < iVar2) {
      bVar1 = false;
      goto LAB_140030a75;
    }
  }
  bVar1 = true;
LAB_140030a75:
  if (bVar8) {
    QModelIndex::~QModelIndex((QModelIndex *)&local_30);
  }
  if ((((!bVar1) && (-1 < *param_3)) && (-1 < param_3[1])) && (*(longlong *)(param_3 + 4) != 0)) {
    uVar3 = FUN_14002e510(param_1,param_3);
    uVar4 = FUN_14002e340(param_1,uVar3);
    iVar7 = (int)param_4;
    if (iVar7 == 0) {
      iVar7 = param_3[1];
      if (iVar7 == 0) {
        pQVar6 = (QString *)FUN_14002e650(param_1,local_res18,uVar3);
        QVariant::QVariant(param_2,pQVar6);
        QString::~QString(local_res18);
        return param_2;
      }
      if (iVar7 == 1) {
        pQVar6 = (QString *)FUN_14002e8a0(param_1,local_res18,uVar3);
        QVariant::QVariant(param_2,pQVar6);
        QString::~QString(local_res18);
        return param_2;
      }
      if (iVar7 == 2) {
        pQVar6 = (QString *)FUN_14002e2c0(param_1,local_res18,uVar4);
        QVariant::QVariant(param_2,pQVar6);
        QString::~QString(local_res18);
        return param_2;
      }
      if (iVar7 == 3) {
        QVariant::QVariant(param_2,1);
        return param_2;
      }
      if (iVar7 == 4) {
        pQVar6 = (QString *)FUN_14002e3e0(param_1,local_res18,uVar3);
        QVariant::QVariant(param_2,pQVar6);
        QString::~QString(local_res18);
        return param_2;
      }
    }
    else {
      if (iVar7 == 3) {
        (**(code **)(*param_1 + 0x80))(param_1,param_2,param_3,0);
        return param_2;
      }
      if (iVar7 == 7) {
        QVariant::QVariant(param_2,0x81);
        return param_2;
      }
      if ((iVar7 == 0x20) && (param_3[1] == 4)) {
        uVar5 = FUN_14002e480(param_1,uVar3);
        QVariant::QVariant(param_2,uVar5);
        return param_2;
      }
    }
  }
  QVariant::QVariant(param_2);
  return param_2;
}


/* ================= FUN_140030c50 @ 140030c50  size=114  callers=2 callees=2 ================= */
/* CALLERS: FUN_14002ea10, FUN_1400316b0 */
/* CALLEES: detach_helper2, free_helper */

void FUN_140030c50(longlong *param_1)

{
  int *piVar1;
  int iVar2;
  QHashData *pQVar3;
  
  if (*(int *)(*param_1 + 0x10) != 1) {
    pQVar3 = QHashData::detach_helper2
                       ((QHashData *)*param_1,(_func_void_Node_ptr_void_ptr *)&LAB_1400311f0,
                        (_func_void_Node_ptr *)&LAB_14002b600,0x18,8);
    LOCK();
    piVar1 = (int *)(*param_1 + 0x10);
    iVar2 = *piVar1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (iVar2 == 1) {
      QHashData::free_helper((QHashData *)*param_1,(_func_void_Node_ptr *)&LAB_14002b600);
    }
    *param_1 = (longlong)pQVar3;
  }
  return;
}


/* ================= FUN_140030cd0 @ 140030cd0  size=127  callers=4 callees=3 ================= */
/* CALLERS: FUN_14002db20, FUN_14002f800, FUN_14002fc20, FUN_1400300b0 */
/* CALLEES: FUN_140031e50, detach, qFree */

void FUN_140030cd0(QListData *param_1,int param_2)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  Data *pDVar4;
  
  lVar2 = *(longlong *)param_1;
  iVar1 = *(int *)(lVar2 + 8);
  pDVar4 = QListData::detach(param_1,param_2);
  lVar3 = *(longlong *)param_1;
  FUN_140031e50(param_1,lVar3 + (longlong)*(int *)(lVar3 + 8) * 8 + 0x18,
                lVar3 + ((longlong)*(int *)(lVar3 + 0xc) + 3) * 8,lVar2 + (longlong)iVar1 * 8 + 0x18
               );
  LOCK();
  iVar1 = *(int *)pDVar4;
  *(int *)pDVar4 = *(int *)pDVar4 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    qFree(pDVar4);
  }
  return;
}


/* ================= FUN_140030d50 @ 140030d50  size=174  callers=3 callees=4 ================= */
/* CALLERS: FUN_14002ea10, FUN_140030270, FUN_140032eb0 */
/* CALLEES: FUN_140031e80, detach, qFree, ~QString */

void FUN_140030d50(QListData *param_1,int param_2)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  Data *pDVar4;
  Data *this;
  
  lVar2 = *(longlong *)param_1;
  iVar1 = *(int *)(lVar2 + 8);
  pDVar4 = QListData::detach(param_1,param_2);
  lVar3 = *(longlong *)param_1;
  FUN_140031e80(param_1,lVar3 + (longlong)*(int *)(lVar3 + 8) * 8 + 0x18,
                lVar3 + ((longlong)*(int *)(lVar3 + 0xc) + 3) * 8,lVar2 + (longlong)iVar1 * 8 + 0x18
               );
  LOCK();
  iVar1 = *(int *)pDVar4;
  *(int *)pDVar4 = *(int *)pDVar4 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    iVar1 = *(int *)(pDVar4 + 8);
    this = pDVar4 + ((longlong)*(int *)(pDVar4 + 0xc) + 3) * 8;
    while (this != pDVar4 + ((longlong)iVar1 + 3) * 8) {
      this = this + -8;
      QString::~QString((QString *)this);
    }
    qFree(pDVar4);
  }
  return;
}


/* ================= FUN_140030e00 @ 140030e00  size=257  callers=1 callees=5 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: FUN_140031ee0, __security_check_cookie, continueFreeData, createData, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140030e00(longlong *param_1)

{
  int *piVar1;
  int iVar2;
  QMapData *this;
  QMapData *pQVar3;
  QMapData *pQVar4;
  QMapData *pQVar5;
  longlong lVar6;
  undefined1 auStack_b8 [32];
  longlong local_98;
  QMapData *local_88;
  longlong *local_80;
  QMapData *local_78 [12];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_b8;
  local_80 = param_1;
  pQVar5 = QMapData::createData(8);
  lVar6 = *param_1;
  local_88 = pQVar5;
  if (*(int *)(lVar6 + 0x70) != 0) {
    *(uint *)(pQVar5 + 0x78) = *(uint *)(pQVar5 + 0x78) | 1;
    local_78[0] = pQVar5;
    for (lVar6 = *(longlong *)(*param_1 + 8); lVar6 != *param_1; lVar6 = *(longlong *)(lVar6 + 8)) {
      local_98 = lVar6 + -8;
      FUN_140031ee0(param_1,pQVar5,local_78);
    }
    *(uint *)(pQVar5 + 0x78) = *(uint *)(pQVar5 + 0x78) & 0xfffffffe;
    lVar6 = *param_1;
  }
  LOCK();
  piVar1 = (int *)(lVar6 + 0x68);
  iVar2 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar2 == 1) {
    this = (QMapData *)*param_1;
    pQVar4 = *(QMapData **)(this + 8);
    while (pQVar4 != this) {
      pQVar3 = *(QMapData **)(pQVar4 + 8);
      QString::~QString((QString *)(pQVar4 + -8));
      pQVar4 = pQVar3;
    }
    QMapData::continueFreeData(this,0x10);
  }
  *param_1 = (longlong)pQVar5;
  return;
}


/* ================= FUN_140030f10 @ 140030f10  size=202  callers=1 callees=4 ================= */
/* CALLERS: FUN_140030270 */
/* CALLEES: FUN_1400314d0, FUN_140031f40, __security_check_cookie, createData */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140030f10(longlong *param_1)

{
  int *piVar1;
  int iVar2;
  QMapData *pQVar3;
  longlong lVar4;
  undefined1 auStack_b8 [32];
  longlong local_98;
  QMapData *local_88;
  longlong *local_80;
  QMapData *local_78 [12];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_b8;
  local_80 = param_1;
  pQVar3 = QMapData::createData(8);
  lVar4 = *param_1;
  local_88 = pQVar3;
  if (*(int *)(lVar4 + 0x70) != 0) {
    *(uint *)(pQVar3 + 0x78) = *(uint *)(pQVar3 + 0x78) | 1;
    local_78[0] = pQVar3;
    for (lVar4 = *(longlong *)(*param_1 + 8); lVar4 != *param_1; lVar4 = *(longlong *)(lVar4 + 8)) {
      local_98 = lVar4 + -8;
      FUN_140031f40(param_1,pQVar3,local_78);
    }
    *(uint *)(pQVar3 + 0x78) = *(uint *)(pQVar3 + 0x78) & 0xfffffffe;
    lVar4 = *param_1;
  }
  LOCK();
  piVar1 = (int *)(lVar4 + 0x68);
  iVar2 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar2 == 1) {
    FUN_1400314d0(param_1,*param_1);
  }
  *param_1 = (longlong)pQVar3;
  return;
}


/* ================= FUN_140030fe0 @ 140030fe0  size=212  callers=2 callees=3 ================= */
/* CALLERS: FUN_1400307f0, FUN_140051740 */
/* CALLEES: FUN_140031e50, detach_grow, qFree */

longlong FUN_140030fe0(QListData *param_1,int param_2,int param_3)

{
  longlong lVar1;
  int iVar2;
  Data *pDVar3;
  longlong lVar4;
  int local_res10 [2];
  
  lVar1 = *(longlong *)param_1 + 0x18 + (longlong)*(int *)(*(longlong *)param_1 + 8) * 8;
  local_res10[0] = param_2;
  pDVar3 = QListData::detach_grow(param_1,local_res10,param_3);
  lVar4 = *(longlong *)param_1 + (longlong)*(int *)(*(longlong *)param_1 + 8) * 8 + 0x18;
  FUN_140031e50(param_1,lVar4,lVar4 + (longlong)local_res10[0] * 8,lVar1);
  lVar4 = *(longlong *)param_1;
  FUN_140031e50(param_1,lVar4 + 0x18 +
                        ((longlong)*(int *)(lVar4 + 8) + (longlong)local_res10[0] +
                        (longlong)param_3) * 8,lVar4 + ((longlong)*(int *)(lVar4 + 0xc) + 3) * 8,
                lVar1 + (longlong)local_res10[0] * 8);
  LOCK();
  iVar2 = *(int *)pDVar3;
  *(int *)pDVar3 = *(int *)pDVar3 + -1;
  UNLOCK();
  if (iVar2 == 1) {
    qFree(pDVar3);
  }
  return *(longlong *)param_1 +
         ((longlong)local_res10[0] + 3 + (longlong)*(int *)(*(longlong *)param_1 + 8)) * 8;
}


/* ================= FUN_1400310c0 @ 1400310c0  size=259  callers=1 callees=4 ================= */
/* CALLERS: FUN_140030840 */
/* CALLEES: FUN_140031e80, detach_grow, qFree, ~QString */

longlong FUN_1400310c0(QListData *param_1,int param_2,int param_3)

{
  longlong lVar1;
  int iVar2;
  Data *pDVar3;
  longlong lVar4;
  Data *this;
  int local_res10 [2];
  
  lVar1 = *(longlong *)param_1 + 0x18 + (longlong)*(int *)(*(longlong *)param_1 + 8) * 8;
  local_res10[0] = param_2;
  pDVar3 = QListData::detach_grow(param_1,local_res10,param_3);
  lVar4 = *(longlong *)param_1 + (longlong)*(int *)(*(longlong *)param_1 + 8) * 8 + 0x18;
  FUN_140031e80(param_1,lVar4,lVar4 + (longlong)local_res10[0] * 8,lVar1);
  lVar4 = *(longlong *)param_1;
  FUN_140031e80(param_1,lVar4 + 0x18 +
                        ((longlong)*(int *)(lVar4 + 8) + (longlong)local_res10[0] +
                        (longlong)param_3) * 8,lVar4 + ((longlong)*(int *)(lVar4 + 0xc) + 3) * 8,
                lVar1 + (longlong)local_res10[0] * 8);
  LOCK();
  iVar2 = *(int *)pDVar3;
  *(int *)pDVar3 = *(int *)pDVar3 + -1;
  UNLOCK();
  if (iVar2 == 1) {
    iVar2 = *(int *)(pDVar3 + 8);
    this = pDVar3 + ((longlong)*(int *)(pDVar3 + 0xc) + 3) * 8;
    while (this != pDVar3 + ((longlong)iVar2 + 3) * 8) {
      this = this + -8;
      QString::~QString((QString *)this);
    }
    qFree(pDVar3);
  }
  return *(longlong *)param_1 +
         ((longlong)local_res10[0] + 3 + (longlong)*(int *)(*(longlong *)param_1 + 8)) * 8;
}


/* ================= FUN_140031210 @ 140031210  size=462  callers=0 callees=10 ================= */
/* CALLEES: FUN_14002e140, FUN_140037f20, QString, QVariant, operator=, sourceModel, toString, ~QModelIndex, ~QString, ~QVariant */

undefined8 FUN_140031210(QAbstractProxyModel *param_1,undefined4 param_2,undefined8 param_3)

{
  char cVar1;
  QVariant *pQVar2;
  QString *pQVar3;
  QAbstractItemModel *pQVar4;
  undefined8 *puVar5;
  int iVar6;
  QAbstractProxyModel QVar7;
  undefined8 uVar8;
  QString local_res20 [8];
  QString local_a8 [8];
  QVariant local_a0 [8];
  uint local_98;
  undefined8 local_90;
  undefined8 uStack_88;
  undefined8 uStack_80;
  QString local_78 [8];
  QString local_70 [8];
  QVariant local_68 [16];
  QVariant local_58 [16];
  QModelIndex local_48 [32];
  
  QVariant::QVariant(local_a0);
  QString::QString(local_a8);
  QString::QString(local_res20);
  local_90 = 0xffffffffffffffff;
  uStack_88 = 0;
  uStack_80 = 0;
  uVar8 = 1;
  iVar6 = 0;
  do {
    pQVar2 = (QVariant *)
             FUN_140037f20(*(undefined8 *)(*(longlong *)(param_1 + 0x10) + 0x48),local_68,iVar6);
    QVariant::operator=(local_a0,pQVar2);
    QVariant::~QVariant(local_68);
    if ((local_98 & 0x3fffffff) != 0) {
      pQVar3 = (QString *)QVariant::toString(local_a0);
      QString::operator=(local_res20,pQVar3);
      QString::~QString(local_78);
      QVar7 = (QAbstractProxyModel)0x0;
      if (iVar6 == 0) {
        QVar7 = param_1[0x20];
      }
      pQVar4 = QAbstractProxyModel::sourceModel(param_1);
      puVar5 = (undefined8 *)
               (**(code **)(*(longlong *)pQVar4 + 0x58))(pQVar4,local_48,param_2,iVar6,param_3);
      local_90 = *puVar5;
      uStack_88 = puVar5[1];
      uStack_80 = puVar5[2];
      QModelIndex::~QModelIndex(local_48);
      pQVar4 = QAbstractProxyModel::sourceModel(param_1);
      pQVar2 = (QVariant *)(**(code **)(*(longlong *)pQVar4 + 0x80))(pQVar4,local_58,&local_90,0);
      pQVar3 = (QString *)QVariant::toString(pQVar2);
      QString::operator=(local_a8,pQVar3);
      QString::~QString(local_70);
      QVariant::~QVariant(local_58);
      cVar1 = FUN_14002e140(param_1,local_a8,local_res20,QVar7);
      if (cVar1 == '\0') {
        uVar8 = 0;
        goto LAB_140031393;
      }
    }
    iVar6 = iVar6 + 1;
    if (4 < iVar6) {
LAB_140031393:
      QModelIndex::~QModelIndex((QModelIndex *)&local_90);
      QString::~QString(local_res20);
      QString::~QString(local_a8);
      QVariant::~QVariant(local_a0);
      return uVar8;
    }
  } while( true );
}


/* ================= FUN_1400313e0 @ 1400313e0  size=10  callers=0 callees=0 ================= */

undefined4 * FUN_1400313e0(undefined8 param_1,undefined4 *param_2)

{
  *param_2 = 0x21;
  return param_2;
}


/* ================= FUN_1400313f0 @ 1400313f0  size=134  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002d880 */
/* CALLEES: free, ~QVariant */

void FUN_1400313f0(undefined8 param_1,void *param_2)

{
  int iVar1;
  QVariant *this;
  longlong lVar2;
  
  iVar1 = *(int *)((longlong)param_2 + 8);
  lVar2 = (longlong)param_2 + (longlong)*(int *)((longlong)param_2 + 0xc) * 8 + 0x18;
  while (lVar2 != (longlong)iVar1 * 8 + 0x18 + (longlong)param_2) {
    this = *(QVariant **)(lVar2 + -8);
    lVar2 = lVar2 + -8;
    if (this != (QVariant *)0x0) {
      QVariant::~QVariant(this);
      free(this);
    }
  }
                    /* WARNING: Could not recover jumptable at 0x00014003146f. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  qFree(param_2);
  return;
}


/* ================= FUN_140031480 @ 140031480  size=79  callers=1 callees=1 ================= */
/* CALLERS: FUN_14005a4d0 */
/* CALLEES: ~QString */

void FUN_140031480(undefined8 param_1,QMapData *param_2)

{
  QMapData *pQVar1;
  QMapData *pQVar2;
  
  pQVar2 = *(QMapData **)(param_2 + 8);
  while (pQVar2 != param_2) {
    pQVar1 = *(QMapData **)(pQVar2 + 8);
    QString::~QString((QString *)(pQVar2 + -8));
    pQVar2 = pQVar1;
  }
                    /* WARNING: Could not recover jumptable at 0x0001400314c8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMapData::continueFreeData(param_2,0x10);
  return;
}


/* ================= FUN_1400314d0 @ 1400314d0  size=94  callers=3 callees=1 ================= */
/* CALLERS: FUN_14002d910, FUN_140030f10, FUN_14005a500 */
/* CALLEES: ~QString */

void FUN_1400314d0(undefined8 param_1,QMapData *param_2)

{
  QMapData *pQVar1;
  QMapData *pQVar2;
  
  pQVar2 = *(QMapData **)(param_2 + 8);
  while (pQVar2 != param_2) {
    pQVar1 = *(QMapData **)(pQVar2 + 8);
    QString::~QString((QString *)(pQVar2 + -0x10));
    QString::~QString((QString *)(pQVar2 + -8));
    pQVar2 = pQVar1;
  }
                    /* WARNING: Could not recover jumptable at 0x000140031527. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMapData::continueFreeData(param_2,0x10);
  return;
}


/* ================= FUN_140031530 @ 140031530  size=254  callers=0 callees=1 ================= */
/* CALLEES: QVariant */

QVariant * FUN_140031530(undefined8 param_1,QVariant *param_2,int param_3,int param_4,int param_5)

{
  if (param_5 == 0) {
    if (param_4 == 1) {
      if (param_3 == 0) {
        QVariant::QVariant(param_2,"Manufacturer");
        return param_2;
      }
      if (param_3 == 1) {
        QVariant::QVariant(param_2,"Device");
        return param_2;
      }
      if (param_3 == 2) {
        QVariant::QVariant(param_2,"Core");
        return param_2;
      }
      if (param_3 == 3) {
        QVariant::QVariant(param_2,"NumCores");
        return param_2;
      }
      if (param_3 == 4) {
        QVariant::QVariant(param_2,"Flash Size");
        return param_2;
      }
    }
  }
  else {
    if (param_5 == 7) {
      QVariant::QVariant(param_2,0x81);
      return param_2;
    }
    if (param_5 == 0xe) {
      QVariant::QVariant(param_2,0);
      return param_2;
    }
  }
  QVariant::QVariant(param_2);
  return param_2;
}


/* ================= FUN_140031630 @ 140031630  size=125  callers=0 callees=1 ================= */
/* CALLEES: createIndex */

undefined8 *
FUN_140031630(QAbstractItemModel *param_1,undefined8 *param_2,int param_3,int param_4,int *param_5)

{
  if ((((*param_5 < 0) || (param_5[1] < 0)) || (*(longlong *)(param_5 + 4) == 0)) &&
     ((-1 < param_3 &&
      (param_3 <=
       *(int *)(*(longlong *)(param_1 + 0x20) + 0xc) - *(int *)(*(longlong *)(param_1 + 0x20) + 8)))
     )) {
    QAbstractItemModel::createIndex(param_1,(int)param_2,param_3,param_4);
    return param_2;
  }
  *param_2 = 0xffffffffffffffff;
  param_2[1] = 0;
  param_2[2] = 0;
  return param_2;
}


/* ================= FUN_1400316b0 @ 1400316b0  size=339  callers=1 callees=6 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: FUN_140030c50, QString, allocateNode, operator==, qHash, rehash */

longlong * FUN_1400316b0(longlong *param_1,longlong *param_2,QString *param_3)

{
  longlong lVar1;
  ulonglong uVar2;
  bool bVar3;
  uint uVar4;
  QHashData *pQVar5;
  longlong *plVar6;
  longlong *plVar7;
  QHashData *this;
  
  FUN_140030c50();
  uVar4 = qHash(param_3);
  this = (QHashData *)*param_1;
  plVar7 = param_1;
  if (*(uint *)(this + 0x20) != 0) {
    uVar2 = (ulonglong)uVar4 % (ulonglong)*(uint *)(this + 0x20);
    plVar7 = (longlong *)(*(longlong *)(this + 8) + uVar2 * 8);
    pQVar5 = *(QHashData **)(*(longlong *)(this + 8) + uVar2 * 8);
    if (pQVar5 != this) {
      do {
        if ((uVar4 == *(uint *)(pQVar5 + 8)) &&
           (bVar3 = QString::operator==(param_3,(QString *)(pQVar5 + 0x10)), bVar3)) {
          this = (QHashData *)*param_1;
          break;
        }
        plVar7 = (longlong *)*plVar7;
        this = (QHashData *)*param_1;
        pQVar5 = (QHashData *)*plVar7;
      } while (pQVar5 != this);
    }
    if ((QHashData *)*plVar7 != this) {
      *param_2 = *plVar7;
      return param_2;
    }
  }
  if (*(int *)(this + 0x20) <= *(int *)(this + 0x14)) {
    QHashData::rehash(this,*(short *)(this + 0x1e) + 1);
    uVar4 = qHash(param_3);
    this = (QHashData *)*param_1;
    plVar7 = param_1;
    if (*(uint *)(this + 0x20) != 0) {
      uVar2 = (ulonglong)uVar4 % (ulonglong)*(uint *)(this + 0x20);
      plVar7 = (longlong *)(*(longlong *)(this + 8) + uVar2 * 8);
      pQVar5 = *(QHashData **)(*(longlong *)(this + 8) + uVar2 * 8);
      if (pQVar5 != this) {
        do {
          if ((uVar4 == *(uint *)(pQVar5 + 8)) &&
             (bVar3 = QString::operator==(param_3,(QString *)(pQVar5 + 0x10)), bVar3)) {
            this = (QHashData *)*param_1;
            break;
          }
          plVar7 = (longlong *)*plVar7;
          this = (QHashData *)*param_1;
          pQVar5 = (QHashData *)*plVar7;
        } while (pQVar5 != this);
      }
    }
  }
  plVar6 = QHashData::allocateNode(this,8);
  QString::QString((QString *)(plVar6 + 2),param_3);
  *(uint *)(plVar6 + 1) = uVar4;
  *plVar6 = *plVar7;
  *plVar7 = (longlong)plVar6;
  lVar1 = *param_1;
  *param_2 = (longlong)plVar6;
  *(int *)(lVar1 + 0x14) = *(int *)(lVar1 + 0x14) + 1;
  return param_2;
}


/* ================= FUN_140031810 @ 140031810  size=154  callers=0 callees=3 ================= */
/* CALLEES: cast, focusWidget, instance */

void FUN_140031810(QDialog *param_1,QKeyEvent *param_2)

{
  QWidget *pQVar1;
  QObject *pQVar2;
  
  QCoreApplication::instance();
  pQVar1 = QApplication::focusWidget();
  if (pQVar1 != (QWidget *)0x0) {
    pQVar2 = QMetaObject::cast((QMetaObject *)staticMetaObject_exref,(QObject *)pQVar1);
    if (pQVar2 == (QObject *)0x0) {
      pQVar2 = QMetaObject::cast((QMetaObject *)staticMetaObject_exref,(QObject *)pQVar1);
      if (pQVar2 == (QObject *)0x0) goto LAB_14003188e;
    }
    if (*(int *)(param_2 + 0x28) == 0x1000000) {
                    /* WARNING: Could not recover jumptable at 0x000140031887. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      QWidget::setFocus(*(QWidget **)(param_1 + 0x88),7);
      return;
    }
  }
LAB_14003188e:
                    /* WARNING: Could not recover jumptable at 0x0001400318a3. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::keyPressEvent(param_1,param_2);
  return;
}


/* ================= FUN_1400318b0 @ 1400318b0  size=1001  callers=0 callees=15 ================= */
/* CALLEES: FUN_140037f20, QString, QVariant, compare, data, operator!=, operator=, operator==, sourceModel, toInt, toString, toUInt, ~QModelIndex, ~QString, ~QVariant */

undefined1 FUN_1400318b0(QAbstractProxyModel *param_1,QModelIndex *param_2,QModelIndex *param_3)

{
  int iVar1;
  bool bVar2;
  undefined1 uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  QVariant *pQVar7;
  QString *pQVar8;
  QAbstractItemModel *pQVar9;
  QAbstractItemModel *pQVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  int iVar13;
  QString local_res8 [8];
  QString local_res10 [16];
  QString local_res20 [8];
  undefined8 local_b8;
  undefined8 local_b0;
  undefined8 uStack_a8;
  QString local_a0 [8];
  QVariant local_98 [8];
  uint local_90;
  undefined8 local_88;
  undefined8 local_80;
  undefined8 uStack_78;
  QModelIndex local_70 [24];
  QModelIndex local_58 [32];
  
  QString::QString(local_res10);
  QString::QString(local_res8);
  QVariant::QVariant(local_98);
  QString::QString(local_res20);
  iVar1 = *(int *)(param_2 + 4);
  pQVar7 = (QVariant *)QModelIndex::data(param_2,(int)&local_b8);
  pQVar8 = (QString *)QVariant::toString(pQVar7);
  QString::operator=(local_res10,pQVar8);
  QString::~QString(local_a0);
  QVariant::~QVariant((QVariant *)&local_b8);
  pQVar7 = (QVariant *)QModelIndex::data(param_3,(int)&local_b8);
  pQVar8 = (QString *)QVariant::toString(pQVar7);
  QString::operator=(local_res8,pQVar8);
  QString::~QString(local_a0);
  QVariant::~QVariant((QVariant *)&local_b8);
  if (iVar1 == 0) {
    pQVar7 = (QVariant *)
             FUN_140037f20(*(undefined8 *)(*(longlong *)(param_1 + 0x10) + 0x48),&local_b8);
    QVariant::operator=(local_98,pQVar7);
    QVariant::~QVariant((QVariant *)&local_b8);
    if ((local_90 & 0x3fffffff) != 0) {
      pQVar8 = (QString *)QVariant::toString(local_98);
      QString::operator=(local_res20,pQVar8);
      QString::~QString(local_a0);
      iVar13 = QString::compare(local_res10,local_res20,0);
      if (iVar13 == 0) {
        iVar13 = -1;
        goto LAB_140031b71;
      }
      iVar13 = QString::compare(local_res8,local_res20,0);
      if (iVar13 == 0) {
        iVar13 = 1;
        goto LAB_140031b71;
      }
    }
    bVar2 = QString::operator==(local_res10,"Unspecified");
    if (bVar2) {
      bVar2 = QString::operator!=(local_res8,"Unspecified");
      if (bVar2) {
        iVar13 = -1;
        goto LAB_140031b71;
      }
    }
    bVar2 = QString::operator==(local_res8,"Unspecified");
    if (bVar2) {
      bVar2 = QString::operator!=(local_res10,"Unspecified");
      if (bVar2) {
        iVar13 = 1;
        goto LAB_140031b71;
      }
    }
  }
  else if ((iVar1 != 1) && (iVar1 != 2)) {
    if (iVar1 == 3) {
      pQVar7 = (QVariant *)QModelIndex::data(param_2,(int)&local_b8);
      iVar13 = QVariant::toInt(pQVar7,(bool *)0x0);
      QVariant::~QVariant((QVariant *)&local_b8);
      pQVar7 = (QVariant *)QModelIndex::data(param_3,(int)&local_b8);
      iVar6 = QVariant::toInt(pQVar7,(bool *)0x0);
      QVariant::~QVariant((QVariant *)&local_b8);
      iVar13 = iVar13 - iVar6;
      goto LAB_140031b71;
    }
    if (iVar1 == 4) {
      pQVar7 = (QVariant *)QModelIndex::data(param_2,(int)&local_b8);
      uVar4 = QVariant::toUInt(pQVar7,(bool *)0x0);
      QVariant::~QVariant((QVariant *)&local_b8);
      pQVar7 = (QVariant *)QModelIndex::data(param_3,(int)&local_b8);
      uVar5 = QVariant::toUInt(pQVar7,(bool *)0x0);
      QVariant::~QVariant((QVariant *)&local_b8);
      iVar13 = uVar4 - uVar5;
      goto LAB_140031b71;
    }
  }
  iVar13 = QString::compare(local_res10,local_res8,0);
LAB_140031b71:
  if ((iVar1 == *(int *)(param_1 + 0x2c)) && (*(int *)(param_1 + 0x30) == 1)) {
    iVar13 = -iVar13;
  }
  if (iVar13 < 0) {
    uVar3 = 1;
  }
  else if ((iVar13 < 1) && (iVar1 != *(int *)(param_1 + 0x2c))) {
    pQVar9 = QAbstractProxyModel::sourceModel(param_1);
    pQVar10 = QAbstractProxyModel::sourceModel(param_1);
    local_88 = 0xffffffffffffffff;
    local_80 = 0;
    uStack_78 = 0;
    uVar11 = (**(code **)(*(longlong *)pQVar9 + 0x58))
                       (pQVar9,local_58,*(undefined4 *)param_3,*(undefined4 *)(param_1 + 0x2c),
                        &local_88);
    local_b8 = 0xffffffffffffffff;
    local_b0 = 0;
    uStack_a8 = 0;
    uVar12 = (**(code **)(*(longlong *)pQVar10 + 0x58))
                       (pQVar10,local_70,*(undefined4 *)param_2,*(undefined4 *)(param_1 + 0x2c),
                        &local_b8);
    uVar3 = (**(code **)(*(longlong *)param_1 + 0x170))(param_1,uVar12,uVar11);
    QModelIndex::~QModelIndex(local_70);
    QModelIndex::~QModelIndex((QModelIndex *)&local_b8);
    QModelIndex::~QModelIndex(local_58);
    QModelIndex::~QModelIndex((QModelIndex *)&local_88);
  }
  else {
    uVar3 = 0;
  }
  QString::~QString(local_res20);
  QVariant::~QVariant(local_98);
  QString::~QString(local_res8);
  QString::~QString(local_res10);
  return uVar3;
}


/* ================= FUN_140031ca0 @ 140031ca0  size=426  callers=0 callees=11 ================= */
/* CALLEES: QCursor, cast, cursor, indexWidget, model, mouseMoveEvent, setCursor, shape, viewport, ~QCursor, ~QModelIndex */

void FUN_140031ca0(QAbstractItemView *param_1,QMouseEvent *param_2)

{
  CursorShape CVar1;
  QAbstractItemModel *pQVar2;
  undefined8 *puVar3;
  QWidget *pQVar4;
  QObject *pQVar5;
  byte *pbVar6;
  QCursor *pQVar7;
  QCursor local_res18 [16];
  undefined8 local_38;
  undefined8 uStack_30;
  undefined8 uStack_28;
  QModelIndex local_20 [24];
  
  local_38 = 0xffffffffffffffff;
  uStack_30 = 0;
  uStack_28 = 0;
  pQVar2 = QAbstractItemView::model(param_1);
  if (pQVar2 != (QAbstractItemModel *)0x0) {
    puVar3 = (undefined8 *)
             (**(code **)(*(longlong *)param_1 + 0x1c8))(param_1,local_20,param_2 + 0x20);
    local_38 = *puVar3;
    uStack_30 = puVar3[1];
    uStack_28 = puVar3[2];
    QModelIndex::~QModelIndex(local_20);
    pQVar4 = QAbstractItemView::indexWidget(param_1,(QModelIndex *)&local_38);
    if (pQVar4 == (QWidget *)0x0) {
      pQVar5 = (QObject *)0x0;
    }
    else {
      pQVar5 = QMetaObject::cast((QMetaObject *)staticMetaObject_exref,(QObject *)pQVar4);
    }
    pQVar2 = QAbstractItemView::model(param_1);
    pbVar6 = (byte *)(**(code **)(*(longlong *)pQVar2 + 0x100))(pQVar2,local_res18,&local_38);
    if (((*pbVar6 & 2) == 0) || (pQVar5 != (QObject *)0x0)) {
      pQVar4 = QAbstractScrollArea::viewport((QAbstractScrollArea *)param_1);
      pQVar7 = (QCursor *)QWidget::cursor(pQVar4);
      CVar1 = QCursor::shape(pQVar7);
      QCursor::~QCursor(local_res18);
      if (CVar1 == 0) goto LAB_140031e22;
      QCursor::QCursor(local_res18,0);
      pQVar4 = QAbstractScrollArea::viewport((QAbstractScrollArea *)param_1);
      QWidget::setCursor(pQVar4,local_res18);
    }
    else {
      pQVar4 = QAbstractScrollArea::viewport((QAbstractScrollArea *)param_1);
      pQVar7 = (QCursor *)QWidget::cursor(pQVar4);
      CVar1 = QCursor::shape(pQVar7);
      QCursor::~QCursor(local_res18);
      if (CVar1 == 4) goto LAB_140031e22;
      QCursor::QCursor(local_res18,4);
      pQVar4 = QAbstractScrollArea::viewport((QAbstractScrollArea *)param_1);
      QWidget::setCursor(pQVar4,local_res18);
    }
    QCursor::~QCursor(local_res18);
  }
LAB_140031e22:
  QAbstractItemView::mouseMoveEvent(param_1,param_2);
  QModelIndex::~QModelIndex((QModelIndex *)&local_38);
  return;
}


/* ================= FUN_140031e50 @ 140031e50  size=40  callers=2 callees=1 ================= */
/* CALLERS: FUN_140030cd0, FUN_140030fe0 */
/* CALLEES: memcpy */

void FUN_140031e50(undefined8 param_1,void *param_2,longlong param_3,void *param_4)

{
  longlong lVar1;
  
  if ((param_4 != param_2) && (lVar1 = param_3 - (longlong)param_2 >> 3, 0 < lVar1)) {
    memcpy(param_2,param_4,lVar1 * 8);
    return;
  }
  return;
}


/* ================= FUN_140031e80 @ 140031e80  size=82  callers=2 callees=1 ================= */
/* CALLERS: FUN_140030d50, FUN_1400310c0 */
/* CALLEES: QString */

void FUN_140031e80(undefined8 param_1,QString *param_2,QString *param_3,QString *param_4)

{
  for (; param_2 != param_3; param_2 = param_2 + 8) {
    QString::QString(param_2,param_4);
    param_4 = param_4 + 8;
  }
  return;
}


/* ================= FUN_140031ee0 @ 140031ee0  size=91  callers=2 callees=2 ================= */
/* CALLERS: FUN_14002ea10, FUN_140030e00 */
/* CALLEES: QString, node_create */

Node * FUN_140031ee0(undefined8 param_1,QMapData *param_2,Node **param_3,undefined4 *param_4,
                    QString *param_5)

{
  Node *pNVar1;
  
  pNVar1 = QMapData::node_create(param_2,param_3,0x10,8);
  *(undefined4 *)(pNVar1 + -0x10) = *param_4;
  QString::QString((QString *)(pNVar1 + -8),param_5);
  return pNVar1;
}


/* ================= FUN_140031f40 @ 140031f40  size=118  callers=2 callees=2 ================= */
/* CALLERS: FUN_140030270, FUN_140030f10 */
/* CALLEES: QString, node_create */

Node * FUN_140031f40(undefined8 param_1,QMapData *param_2,Node **param_3,QString *param_4,
                    QString *param_5)

{
  Node *pNVar1;
  
  pNVar1 = QMapData::node_create(param_2,param_3,0x10,8);
  QString::QString((QString *)(pNVar1 + -0x10),param_4);
  QString::QString((QString *)(pNVar1 + -8),param_5);
  return pNVar1;
}


/* ================= FUN_140031fc0 @ 140031fc0  size=61  callers=1 callees=1 ================= */
/* CALLERS: FUN_14005a5c0 */
/* CALLEES: ~QString */

void FUN_140031fc0(undefined8 param_1,QString *param_2,QString *param_3)

{
  while (param_3 != param_2) {
    param_3 = param_3 + -8;
    QString::~QString(param_3);
  }
  return;
}


/* ================= FUN_140032000 @ 140032000  size=10  callers=1 callees=0 ================= */
/* CALLERS: FUN_14005a420 */

void FUN_140032000(undefined8 param_1,QString *param_2)

{
                    /* WARNING: Could not recover jumptable at 0x000140032003. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QString::~QString(param_2);
  return;
}


/* ================= FUN_140032010 @ 140032010  size=21  callers=0 callees=0 ================= */

undefined8 * FUN_140032010(undefined8 param_1,undefined8 *param_2)

{
  *param_2 = 0xffffffffffffffff;
  param_2[1] = 0;
  param_2[2] = 0;
  return param_2;
}


/* ================= FUN_140032030 @ 140032030  size=6  callers=2 callees=0 ================= */
/* CALLERS: FUN_14005a750, FUN_14005a7c0 */

undefined8 FUN_140032030(void)

{
  return 0x10;
}


/* ================= FUN_140032040 @ 140032040  size=92  callers=0 callees=3 ================= */
/* CALLEES: QString, operator=, ~QString */

void FUN_140032040(QDialog *param_1)

{
  QString *pQVar1;
  QString local_res8 [32];
  
  param_1[200] = (QDialog)0x0;
  *(undefined8 *)(param_1 + 0xcc) = 0xffffffffffffffff;
  *(undefined4 *)(param_1 + 0xd4) = 0xffffffff;
  pQVar1 = (QString *)QString::QString(local_res8);
  QString::operator=((QString *)(param_1 + 0xd8),pQVar1);
  QString::~QString(local_res8);
                    /* WARNING: Could not recover jumptable at 0x000140032095. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::reject(param_1);
  return;
}


/* ================= FUN_1400320a0 @ 1400320a0  size=212  callers=1 callees=4 ================= */
/* CALLERS: FUN_140032220 */
/* CALLEES: setText, setWindowTitle, translate, ~QString */

void FUN_1400320a0(longlong param_1,QWidget *param_2)

{
  QString *pQVar1;
  QString local_res8 [8];
  
  pQVar1 = (QString *)
           QCoreApplication::translate
                     ((char *)local_res8,"DeviceSelectionDialog","Target Device Settings",0);
  QWidget::setWindowTitle(param_2,pQVar1);
  QString::~QString(local_res8);
  pQVar1 = (QString *)QCoreApplication::translate((char *)local_res8,"DeviceSelectionDialog","OK",0)
  ;
  QAbstractButton::setText(*(QAbstractButton **)(param_1 + 0x58),pQVar1);
  QString::~QString(local_res8);
  pQVar1 = (QString *)
           QCoreApplication::translate((char *)local_res8,"DeviceSelectionDialog","Cancel",0);
  QAbstractButton::setText(*(QAbstractButton **)(param_1 + 0x60),pQVar1);
  QString::~QString(local_res8);
  return;
}


/* ================= FUN_140032180 @ 140032180  size=34  callers=0 callees=0 ================= */

int FUN_140032180(longlong param_1,int *param_2)

{
  if (((-1 < *param_2) && (-1 < param_2[1])) && (*(longlong *)(param_2 + 4) != 0)) {
    return 0;
  }
  return *(int *)(*(longlong *)(param_1 + 0x20) + 0xc) - *(int *)(*(longlong *)(param_1 + 0x20) + 8)
  ;
}


/* ================= FUN_1400321b0 @ 1400321b0  size=108  callers=1 callees=5 ================= */
/* CALLERS: FUN_14002fe40 */
/* CALLEES: QFontMetrics, height, setDefaultSectionSize, verticalHeader, ~QFontMetrics */

void FUN_1400321b0(QWidget *param_1,QFont *param_2)

{
  int iVar1;
  QFontMetrics *this;
  QHeaderView *this_00;
  QFontMetrics local_res8 [8];
  
  this = (QFontMetrics *)QFontMetrics::QFontMetrics(local_res8,param_2);
  iVar1 = QFontMetrics::height(this);
  QFontMetrics::~QFontMetrics(local_res8);
  this_00 = QTableView::verticalHeader(*(QTableView **)(param_1 + 0x88));
  QHeaderView::setDefaultSectionSize(this_00,iVar1 + 4);
                    /* WARNING: Could not recover jumptable at 0x000140032215. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::setFont(param_1,param_2);
  return;
}


/* ================= FUN_140032220 @ 140032220  size=2299  callers=1 callees=39 ================= */
/* CALLERS: FUN_14002fe40 */
/* CALLEES: FUN_14002d6c0, FUN_1400320a0, FUN_140058d94, QFrame, QGridLayout, QHBoxLayout, QIcon, QPushButton, QSpacerItem, QVBoxLayout, QWidget, addFile, addItem, addWidget, connectSlotsByName, fromUtf8, horizontalHeader, objectName, setAutoFillBackground, setContentsMargins, setContextMenuPolicy, setFrameShadow, setFrameShape, setHorizontalScrollMode, setObjectName, setSelectionBehavior, setSelectionMode, setSizePolicy, setSortingEnabled, setSpacing, setStretchLastSection, setTabKeyNavigation, setVerticalScrollMode, setWindowIcon, sizePolicy, sortByColumn, verticalHeader, ~QIcon, ~QString */

void FUN_140032220(undefined8 *param_1,QObject *param_2)

{
  int iVar1;
  longlong *plVar2;
  QString *pQVar3;
  QGridLayout *this;
  QFrame *pQVar4;
  uint *puVar5;
  QHBoxLayout *pQVar6;
  QSpacerItem *pQVar7;
  QVBoxLayout *this_00;
  QFrame *pQVar8;
  QHeaderView *pQVar9;
  QWidget *pQVar10;
  QPushButton *this_01;
  QPushButton *pQVar11;
  QGridLayout *this_02;
  QVBoxLayout *this_03;
  QWidget *pQVar12;
  QHBoxLayout *pQVar13;
  QSpacerItem *pQVar14;
  QPushButton *pQVar15;
  QGridLayout *local_res8;
  QIcon local_res18 [8];
  QFrame *local_res20;
  
  plVar2 = (longlong *)QObject::objectName(param_2);
  iVar1 = *(int *)(*plVar2 + 8);
  QString::~QString((QString *)&local_res8);
  if (iVar1 == 0) {
    pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e328);
    QObject::setObjectName(param_2,pQVar3);
    QString::~QString((QString *)&local_res8);
  }
  QIcon::QIcon(local_res18);
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res20,0x4006e340);
  local_res8 = (QGridLayout *)0xffffffffffffffff;
  QIcon::addFile(local_res18,pQVar3,(QSize *)&local_res8,0,1);
  QString::~QString((QString *)&local_res20);
  QWidget::setWindowIcon((QWidget *)param_2,local_res18);
  this = (QGridLayout *)FUN_140058d94(0x20);
  pQVar15 = (QPushButton *)0x0;
  this_02 = (QGridLayout *)pQVar15;
  local_res8 = this;
  if (this != (QGridLayout *)0x0) {
    QGridLayout::QGridLayout(this,(QWidget *)param_2);
    *(undefined ***)this = QGridLayout::vftable;
    *(undefined ***)(this + 0x10) = QGridLayout::vftable;
    this_02 = this;
  }
  *param_1 = this_02;
  QGridLayout::setSpacing(this_02,7);
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e368);
  QObject::setObjectName((QObject *)*param_1,pQVar3);
  QString::~QString((QString *)&local_res8);
  QLayout::setContentsMargins((QLayout *)*param_1,7,2,7,9);
  pQVar4 = (QFrame *)FUN_140058d94(0x28);
  pQVar8 = (QFrame *)pQVar15;
  local_res20 = pQVar4;
  if (pQVar4 != (QFrame *)0x0) {
    local_res8 = (QGridLayout *)((ulonglong)local_res8 & 0xffffffff00000000);
    QFrame::QFrame(pQVar4,param_2,&local_res8);
    *(undefined ***)pQVar4 = QFrame::vftable;
    *(undefined ***)(pQVar4 + 0x10) = QFrame::vftable;
    pQVar8 = pQVar4;
  }
  param_1[1] = pQVar8;
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e378);
  QObject::setObjectName((QObject *)param_1[1],pQVar3);
  QString::~QString((QString *)&local_res8);
  QFrame::setFrameShape((QFrame *)param_1[1],4);
  QFrame::setFrameShadow((QFrame *)param_1[1],0x30);
  local_res8 = (QGridLayout *)((ulonglong)local_res8 & 0xffffffff00000000);
  QGridLayout::addWidget((QGridLayout *)*param_1,param_1[1],3,0,1,2,&local_res8);
  pQVar4 = (QFrame *)FUN_140058d94(0x28);
  pQVar8 = (QFrame *)pQVar15;
  local_res20 = pQVar4;
  if (pQVar4 != (QFrame *)0x0) {
    local_res8 = (QGridLayout *)((ulonglong)local_res8 & 0xffffffff00000000);
    QFrame::QFrame(pQVar4,param_2,&local_res8);
    *(undefined ***)pQVar4 = QFrame::vftable;
    *(undefined ***)(pQVar4 + 0x10) = QFrame::vftable;
    pQVar8 = pQVar4;
  }
  param_1[2] = pQVar8;
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e388);
  QObject::setObjectName((QObject *)param_1[2],pQVar3);
  QString::~QString((QString *)&local_res8);
  puVar5 = (uint *)QWidget::sizePolicy((QWidget *)param_1[2]);
  QWidget::setSizePolicy((QWidget *)param_1[2],(*puVar5 >> 8 & 1) * 0x100 + 0x57);
  pQVar6 = (QHBoxLayout *)FUN_140058d94(0x20);
  pQVar13 = (QHBoxLayout *)pQVar15;
  local_res8 = (QGridLayout *)pQVar6;
  if (pQVar6 != (QHBoxLayout *)0x0) {
    QHBoxLayout::QHBoxLayout(pQVar6,(QWidget *)param_1[2]);
    *(undefined ***)pQVar6 = QHBoxLayout::vftable;
    *(undefined ***)(pQVar6 + 0x10) = QHBoxLayout::vftable;
    pQVar13 = pQVar6;
  }
  param_1[3] = pQVar13;
  QBoxLayout::setSpacing((QBoxLayout *)pQVar13,7);
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e398);
  QObject::setObjectName((QObject *)param_1[3],pQVar3);
  QString::~QString((QString *)&local_res8);
  QLayout::setContentsMargins((QLayout *)param_1[3],0,5,0,0);
  pQVar7 = (QSpacerItem *)FUN_140058d94(0x30);
  pQVar14 = (QSpacerItem *)pQVar15;
  local_res8 = (QGridLayout *)pQVar7;
  if (pQVar7 != (QSpacerItem *)0x0) {
    QSpacerItem::QSpacerItem(pQVar7,0x28,0x14,7,1);
    *(undefined ***)pQVar7 = QSpacerItem::vftable;
    pQVar14 = pQVar7;
  }
  param_1[4] = pQVar14;
  (**(code **)(*(longlong *)param_1[3] + 0x58))((longlong *)param_1[3],pQVar14);
  pQVar4 = (QFrame *)FUN_140058d94(0x28);
  pQVar8 = (QFrame *)pQVar15;
  local_res20 = pQVar4;
  if (pQVar4 != (QFrame *)0x0) {
    local_res8 = (QGridLayout *)((ulonglong)local_res8 & 0xffffffff00000000);
    QFrame::QFrame(pQVar4,param_2,&local_res8);
    *(undefined ***)pQVar4 = QFrame::vftable;
    *(undefined ***)(pQVar4 + 0x10) = QFrame::vftable;
    pQVar8 = pQVar4;
  }
  param_1[5] = pQVar8;
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e3b0);
  QObject::setObjectName((QObject *)param_1[5],pQVar3);
  QString::~QString((QString *)&local_res8);
  QFrame::setFrameShape((QFrame *)param_1[5],0);
  QFrame::setFrameShadow((QFrame *)param_1[5],0x20);
  this_00 = (QVBoxLayout *)FUN_140058d94(0x20);
  this_03 = (QVBoxLayout *)pQVar15;
  local_res8 = (QGridLayout *)this_00;
  if (this_00 != (QVBoxLayout *)0x0) {
    QVBoxLayout::QVBoxLayout(this_00,(QWidget *)param_1[5]);
    *(undefined ***)this_00 = QVBoxLayout::vftable;
    *(undefined ***)(this_00 + 0x10) = QVBoxLayout::vftable;
    this_03 = this_00;
  }
  param_1[6] = this_03;
  QBoxLayout::setSpacing((QBoxLayout *)this_03,0);
  QLayout::setContentsMargins((QLayout *)param_1[6],0,0,0,0);
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e3c0);
  QObject::setObjectName((QObject *)param_1[6],pQVar3);
  QString::~QString((QString *)&local_res8);
  local_res8 = (QGridLayout *)FUN_140058d94(0x28);
  pQVar8 = (QFrame *)pQVar15;
  if (local_res8 != (QGridLayout *)0x0) {
    pQVar8 = (QFrame *)FUN_14002d6c0(local_res8,param_1[5]);
  }
  param_1[7] = pQVar8;
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e3d8);
  QObject::setObjectName((QObject *)param_1[7],pQVar3);
  QString::~QString((QString *)&local_res8);
  puVar5 = (uint *)QWidget::sizePolicy((QWidget *)param_1[7]);
  QWidget::setSizePolicy((QWidget *)param_1[7],(*puVar5 >> 8 & 1) * 0x100 + 0x77);
  QWidget::setContextMenuPolicy((QWidget *)param_1[7],3);
  QWidget::setAutoFillBackground((QWidget *)param_1[7],true);
  QFrame::setFrameShape((QFrame *)param_1[7],1);
  QAbstractItemView::setTabKeyNavigation((QAbstractItemView *)param_1[7],false);
  QAbstractItemView::setSelectionMode((QAbstractItemView *)param_1[7],1);
  QAbstractItemView::setSelectionBehavior((QAbstractItemView *)param_1[7],1);
  QAbstractItemView::setVerticalScrollMode((QAbstractItemView *)param_1[7],1);
  QAbstractItemView::setHorizontalScrollMode((QAbstractItemView *)param_1[7],1);
  QTableView::setSortingEnabled((QTableView *)param_1[7],true);
  QTableView::sortByColumn((QTableView *)param_1[7],0,0);
  pQVar9 = QTableView::horizontalHeader((QTableView *)param_1[7]);
  QHeaderView::setStretchLastSection(pQVar9,true);
  pQVar9 = QTableView::verticalHeader((QTableView *)param_1[7]);
  (**(code **)(*(longlong *)pQVar9 + 0x58))(pQVar9,0);
  local_res8 = (QGridLayout *)((ulonglong)local_res8 & 0xffffffff00000000);
  QBoxLayout::addWidget((QBoxLayout *)param_1[6],param_1[7],0);
  local_res8 = (QGridLayout *)((ulonglong)local_res8 & 0xffffffff00000000);
  QGridLayout::addWidget((QGridLayout *)*param_1,param_1[5],1,0,1,2,&local_res8);
  pQVar10 = (QWidget *)FUN_140058d94(0x28);
  pQVar12 = (QWidget *)pQVar15;
  local_res20 = (QFrame *)pQVar10;
  if (pQVar10 != (QWidget *)0x0) {
    local_res8 = (QGridLayout *)((ulonglong)local_res8 & 0xffffffff00000000);
    QWidget::QWidget(pQVar10,param_2,&local_res8);
    *(undefined ***)pQVar10 = QWidget::vftable;
    *(undefined ***)(pQVar10 + 0x10) = QWidget::vftable;
    pQVar12 = pQVar10;
  }
  param_1[8] = pQVar12;
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e3e4);
  QObject::setObjectName((QObject *)param_1[8],pQVar3);
  QString::~QString((QString *)&local_res8);
  pQVar6 = (QHBoxLayout *)FUN_140058d94(0x20);
  pQVar13 = (QHBoxLayout *)pQVar15;
  local_res8 = (QGridLayout *)pQVar6;
  if (pQVar6 != (QHBoxLayout *)0x0) {
    QHBoxLayout::QHBoxLayout(pQVar6,(QWidget *)param_1[8]);
    *(undefined ***)pQVar6 = QHBoxLayout::vftable;
    *(undefined ***)(pQVar6 + 0x10) = QHBoxLayout::vftable;
    pQVar13 = pQVar6;
  }
  param_1[9] = pQVar13;
  QLayout::setContentsMargins((QLayout *)pQVar13,0,0,0,0);
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e3f0);
  QObject::setObjectName((QObject *)param_1[9],pQVar3);
  QString::~QString((QString *)&local_res8);
  pQVar7 = (QSpacerItem *)FUN_140058d94(0x30);
  pQVar14 = (QSpacerItem *)pQVar15;
  local_res8 = (QGridLayout *)pQVar7;
  if (pQVar7 != (QSpacerItem *)0x0) {
    QSpacerItem::QSpacerItem(pQVar7,0x28,0x14,7,1);
    *(undefined ***)pQVar7 = QSpacerItem::vftable;
    pQVar14 = pQVar7;
  }
  param_1[10] = pQVar14;
  (**(code **)(*(longlong *)param_1[9] + 0x58))((longlong *)param_1[9],pQVar14);
  this_01 = (QPushButton *)FUN_140058d94(0x28);
  pQVar11 = pQVar15;
  local_res8 = (QGridLayout *)this_01;
  if (this_01 != (QPushButton *)0x0) {
    QPushButton::QPushButton(this_01,(QWidget *)param_1[8]);
    *(undefined ***)this_01 = QPushButton::vftable;
    *(undefined ***)(this_01 + 0x10) = QPushButton::vftable;
    pQVar11 = this_01;
  }
  param_1[0xb] = pQVar11;
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e408);
  QObject::setObjectName((QObject *)param_1[0xb],pQVar3);
  QString::~QString((QString *)&local_res8);
  local_res8 = (QGridLayout *)((ulonglong)local_res8 & 0xffffffff00000000);
  QBoxLayout::addWidget((QBoxLayout *)param_1[9],param_1[0xb],0,&local_res8);
  pQVar11 = (QPushButton *)FUN_140058d94(0x28);
  local_res8 = (QGridLayout *)pQVar11;
  if (pQVar11 != (QPushButton *)0x0) {
    QPushButton::QPushButton(pQVar11,(QWidget *)param_1[8]);
    *(undefined ***)pQVar11 = QPushButton::vftable;
    *(undefined ***)(pQVar11 + 0x10) = QPushButton::vftable;
    pQVar15 = pQVar11;
  }
  param_1[0xc] = pQVar15;
  pQVar3 = (QString *)QString::fromUtf8((char *)&local_res8,0x4006e418);
  QObject::setObjectName((QObject *)param_1[0xc],pQVar3);
  QString::~QString((QString *)&local_res8);
  local_res8 = (QGridLayout *)((ulonglong)local_res8 & 0xffffffff00000000);
  QBoxLayout::addWidget((QBoxLayout *)param_1[9],param_1[0xc],0,&local_res8);
  local_res8 = (QGridLayout *)((ulonglong)local_res8 & 0xffffffff00000000);
  QGridLayout::addWidget((QGridLayout *)*param_1,param_1[8],4,0,1,2,&local_res8);
  FUN_1400320a0(param_1,param_2);
  QMetaObject::connectSlotsByName(param_2);
  QIcon::~QIcon(local_res18);
  return;
}


/* ================= FUN_140032b20 @ 140032b20  size=256  callers=0 callees=9 ================= */
/* CALLEES: FUN_14002df00, FUN_14002e9a0, FUN_14002fb50, FUN_140037ed0, horizontalHeader, resize, resizeColumnsToContents, resizeSection, sectionSize */

void FUN_140032b20(QWidget *param_1)

{
  int iVar1;
  undefined4 uVar2;
  QHeaderView *pQVar3;
  QSize *pQVar4;
  undefined1 local_res8 [8];
  
  QTableView::resizeColumnsToContents(*(QTableView **)(param_1 + 0x88));
  pQVar3 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x88));
  iVar1 = QHeaderView::sectionSize(pQVar3,1);
  pQVar3 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x88));
  QHeaderView::resizeSection(pQVar3,1,(int)((double)iVar1 * 1.5));
  pQVar3 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x88));
  iVar1 = QHeaderView::sectionSize(pQVar3,2);
  pQVar3 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x88));
  QHeaderView::resizeSection(pQVar3,2,(int)((double)iVar1 * 1.5));
  FUN_140037ed0(*(undefined8 *)(param_1 + 0x48),1);
  pQVar4 = (QSize *)(**(code **)(*(longlong *)param_1 + 0x60))(param_1,local_res8);
  QWidget::resize(param_1,pQVar4);
  FUN_14002df00(param_1);
  uVar2 = FUN_14002e9a0(param_1);
  FUN_14002fb50(param_1,uVar2);
  return;
}


/* ================= FUN_140032c20 @ 140032c20  size=288  callers=0 callees=6 ================= */
/* CALLEES: columnWidth, horizontalHeader, model, rowHeight, verticalScrollBar, ~QModelIndex */

int * FUN_140032c20(longlong param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  longlong lVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  QHeaderView *pQVar9;
  QAbstractItemModel *pQVar10;
  QScrollBar *pQVar11;
  int iVar12;
  int iVar13;
  undefined8 local_38;
  undefined8 local_30;
  undefined8 uStack_28;
  
  iVar6 = QTableView::rowHeight(*(QTableView **)(param_1 + 0x88),0);
  pQVar9 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x88));
  lVar5 = *(longlong *)(*(longlong *)(param_1 + 0xa8) + 0x20);
  iVar1 = *(int *)(lVar5 + 0x20);
  iVar2 = *(int *)(lVar5 + 0x18);
  iVar3 = *(int *)(*(longlong *)(pQVar9 + 0x20) + 0x18);
  iVar4 = *(int *)(*(longlong *)(pQVar9 + 0x20) + 0x20);
  iVar13 = 0;
  pQVar10 = QAbstractItemView::model(*(QAbstractItemView **)(param_1 + 0x88));
  local_38 = 0xffffffffffffffff;
  local_30 = 0;
  uStack_28 = 0;
  iVar7 = (**(code **)(*(longlong *)pQVar10 + 0x70))(pQVar10,&local_38);
  QModelIndex::~QModelIndex((QModelIndex *)&local_38);
  iVar12 = 0;
  if (0 < iVar7) {
    do {
      iVar8 = QTableView::columnWidth(*(QTableView **)(param_1 + 0x88),iVar12);
      iVar13 = iVar13 + iVar8;
      iVar12 = iVar12 + 1;
    } while (iVar12 < iVar7);
  }
  pQVar11 = QAbstractScrollArea::verticalScrollBar(*(QAbstractScrollArea **)(param_1 + 0x88));
  iVar13 = (*(int *)(*(longlong *)(pQVar11 + 0x20) + 0x1c) -
           *(int *)(*(longlong *)(pQVar11 + 0x20) + 0x14)) + 1 + iVar13;
  if (iVar13 < 0) {
    iVar13 = 0;
  }
  *param_2 = iVar13;
  param_2[1] = ((iVar1 - iVar2) - iVar3) + iVar4 + iVar6 * 0x14 + 2;
  return param_2;
}


/* ================= FUN_140032d40 @ 140032d40  size=30  callers=0 callees=0 ================= */

void FUN_140032d40(QSortFilterProxyModel *param_1,int param_2,SortOrder param_3)

{
  if (param_2 != *(int *)(param_1 + 0x24)) {
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x24);
    *(undefined4 *)(param_1 + 0x30) = *(undefined4 *)(param_1 + 0x28);
  }
  *(int *)(param_1 + 0x24) = param_2;
  *(SortOrder *)(param_1 + 0x28) = param_3;
                    /* WARNING: Could not recover jumptable at 0x000140032d57. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QSortFilterProxyModel::sort(param_1,param_2,param_3);
  return;
}


/* ================= FUN_140032d60 @ 140032d60  size=203  callers=5 callees=1 ================= */
/* CALLERS: FUN_14002e340, FUN_14002e3e0, FUN_14002e480, FUN_14002e650, FUN_14002e800 */
/* CALLEES: QString */

undefined8 * FUN_140032d60(longlong *param_1,undefined8 *param_2,uint *param_3)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  ulonglong uVar3;
  undefined8 *puVar4;
  undefined8 *puVar5;
  
  puVar1 = (undefined8 *)*param_1;
  if ((*(int *)((longlong)puVar1 + 0x14) != 0) && (*(uint *)(puVar1 + 4) != 0)) {
    uVar3 = (ulonglong)*param_3 % (ulonglong)*(uint *)(puVar1 + 4);
    puVar5 = *(undefined8 **)(puVar1[1] + uVar3 * 8);
    puVar2 = (undefined8 *)(puVar1[1] + uVar3 * 8);
    while ((puVar4 = puVar5, puVar4 != puVar1 && (*param_3 != *(uint *)(puVar4 + 1)))) {
      puVar2 = puVar4;
      puVar5 = (undefined8 *)*puVar4;
    }
    puVar2 = (undefined8 *)*puVar2;
    if (puVar2 != puVar1) {
      *(undefined4 *)param_2 = *(undefined4 *)(puVar2 + 2);
      *(undefined4 *)((longlong)param_2 + 4) = *(undefined4 *)((longlong)puVar2 + 0x14);
      *(undefined4 *)(param_2 + 1) = *(undefined4 *)(puVar2 + 3);
      *(undefined4 *)((longlong)param_2 + 0xc) = *(undefined4 *)((longlong)puVar2 + 0x1c);
      *(undefined4 *)(param_2 + 2) = *(undefined4 *)(puVar2 + 4);
      QString::QString((QString *)(param_2 + 3),(QString *)(puVar2 + 5));
      param_2[4] = puVar2[6];
      return param_2;
    }
  }
  *param_2 = 0;
  param_2[1] = 0;
  param_2[2] = 0;
  param_2[3] = 0;
  param_2[4] = 0;
  QString::QString((QString *)(param_2 + 3));
  param_2[4] = 0;
  return param_2;
}


/* ================= FUN_140032e30 @ 140032e30  size=126  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002e8a0 */
/* CALLEES: QString */

QString * FUN_140032e30(longlong *param_1,QString *param_2,int *param_3)

{
  longlong lVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  
  lVar1 = *param_1;
  if ((*(int *)(lVar1 + 0x70) != 0) &&
     (lVar5 = (longlong)*(int *)(lVar1 + 0x6c), lVar2 = lVar1, -1 < lVar5)) {
    do {
      lVar4 = *(longlong *)(lVar2 + 8 + lVar5 * 8);
      if (lVar4 != lVar1) {
        lVar3 = lVar4;
        do {
          lVar4 = lVar3;
          if (*param_3 <= *(int *)(lVar3 + -0x10)) break;
          lVar4 = *(longlong *)(lVar3 + 8 + lVar5 * 8);
          lVar2 = lVar3;
          lVar3 = lVar4;
        } while (lVar4 != lVar1);
      }
      lVar5 = lVar5 + -1;
    } while (-1 < lVar5);
    if ((lVar4 != lVar1) && (*(int *)(lVar4 + -0x10) <= *param_3)) {
      QString::QString(param_2,(QString *)(lVar4 + -8));
      return param_2;
    }
  }
  QString::QString(param_2);
  return param_2;
}


/* ================= FUN_140032eb0 @ 140032eb0  size=198  callers=1 callees=4 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: FUN_140030840, FUN_140030d50, nextNode, realloc */

QListData *
FUN_140032eb0(longlong *param_1,QListData *param_2,undefined8 param_3,undefined8 param_4)

{
  Node *pNVar1;
  int iVar2;
  Node *pNVar3;
  longlong *plVar4;
  undefined4 uVar5;
  
  *(code **)param_2 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  uVar5 = 1;
  if ((*(int **)param_2)[1] < *(int *)(*param_1 + 0x14)) {
    if (**(int **)param_2 == 1) {
      QListData::realloc(param_2,*(int *)(*param_1 + 0x14));
    }
    else {
      FUN_140030d50();
    }
  }
  pNVar1 = (Node *)*param_1;
  plVar4 = *(longlong **)(pNVar1 + 8);
  iVar2 = *(int *)(pNVar1 + 0x20);
  if (iVar2 != 0) {
    while( true ) {
      iVar2 = iVar2 + -1;
      pNVar3 = (Node *)*plVar4;
      if (pNVar3 != pNVar1) break;
      plVar4 = plVar4 + 1;
      if (iVar2 == 0) {
        return param_2;
      }
    }
    do {
      FUN_140030840(param_2,pNVar3 + 0x10,param_3,param_4,uVar5);
      pNVar3 = QHashData::nextNode(pNVar3);
    } while (pNVar3 != (Node *)*param_1);
  }
  return param_2;
}


/* ================= FUN_140032f90 @ 140032f90  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140057220 */
/* CALLEES: FUN_1400331f0, QComboBox */

QComboBox * FUN_140032f90(QComboBox *param_1,QWidget *param_2)

{
  QComboBox::QComboBox(param_1,param_2);
  *(undefined ***)param_1 = CustComboBox::vftable;
  *(undefined ***)(param_1 + 0x10) = CustComboBox::vftable;
  FUN_1400331f0(param_1);
  return param_1;
}


/* ================= FUN_140032fd0 @ 140032fd0  size=59  callers=1 callees=1 ================= */
/* CALLERS: FUN_140033020 */
/* CALLEES: FUN_140043e60 */

void FUN_140032fd0(QComboBox *param_1)

{
  *(undefined ***)param_1 = CustComboBox::vftable;
  *(undefined ***)(param_1 + 0x10) = CustComboBox::vftable;
  if (*(longlong *)(param_1 + 0x30) != 0) {
    FUN_140043e60();
  }
                    /* WARNING: Could not recover jumptable at 0x000140033004. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QComboBox::~QComboBox(param_1);
  return;
}


/* ================= FUN_14003300c @ 14003300c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140033020 */

void FUN_14003300c(longlong param_1)

{
  FUN_140033020(param_1 + -0x10);
  return;
}


/* ================= FUN_140033020 @ 140033020  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003300c */
/* CALLEES: FUN_140032fd0, free */

void * FUN_140033020(void *param_1,ulonglong param_2)

{
  FUN_140032fd0();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140033060 @ 140033060  size=322  callers=1 callees=14 ================= */
/* CALLERS: FUN_140057220 */
/* CALLEES: FUN_140027140, FUN_140033230, FUN_140035970, QIcon, QString, QVariant, blockSignals, count, insertItem, memset, operator=, ~QIcon, ~QString, ~QVariant */

undefined4 FUN_140033060(QObject *param_1,undefined8 param_2,undefined8 param_3)

{
  bool bVar1;
  int iVar2;
  QString *pQVar3;
  QVariant *pQVar4;
  QIcon *pQVar5;
  void *_Dst;
  undefined4 uVar6;
  QString local_res20 [8];
  QString local_38 [8];
  QVariant local_30 [24];
  
  QString::QString(local_res20);
  uVar6 = 0;
  iVar2 = FUN_140033230(param_1);
  if (iVar2 < 0) {
    uVar6 = 0xffffffff;
  }
  else {
    _Dst = (void *)((ulonglong)*(uint *)(param_1 + 0x38) * 0x150 + *(longlong *)(param_1 + 0x30));
    memset(_Dst,0,0x150);
    FUN_140027140(_Dst,param_2,0x144);
    *(undefined8 *)((longlong)_Dst + 0x148) = param_3;
    *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + 1;
    bVar1 = QObject::blockSignals(param_1,true);
    pQVar3 = (QString *)FUN_140035970(local_38,_Dst);
    QString::operator=(local_res20,pQVar3);
    QString::~QString(local_38);
    pQVar4 = (QVariant *)QVariant::QVariant(local_30);
    iVar2 = QComboBox::count((QComboBox *)param_1);
    pQVar5 = (QIcon *)QIcon::QIcon((QIcon *)local_38);
    QComboBox::insertItem((QComboBox *)param_1,iVar2,pQVar5,local_res20,pQVar4);
    QIcon::~QIcon((QIcon *)local_38);
    QVariant::~QVariant(local_30);
    QObject::blockSignals(param_1,bVar1);
  }
  QString::~QString(local_res20);
  return uVar6;
}


/* ================= FUN_1400331b0 @ 1400331b0  size=55  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400578b0 */
/* CALLEES: currentIndex */

longlong FUN_1400331b0(QComboBox *param_1)

{
  int iVar1;
  
  iVar1 = QComboBox::currentIndex(param_1);
  if (iVar1 < 0) {
    return 0;
  }
  return *(longlong *)(param_1 + 0x30) + 0x148 + (longlong)iVar1 * 0x150;
}


/* ================= FUN_1400331f0 @ 1400331f0  size=55  callers=1 callees=1 ================= */
/* CALLERS: FUN_140032f90 */
/* CALLEES: FUN_140043d10 */

void FUN_1400331f0(longlong param_1)

{
  undefined8 uVar1;
  
  *(undefined8 *)(param_1 + 0x30) = 0;
  *(undefined8 *)(param_1 + 0x38) = 0;
  *(undefined1 *)(param_1 + 0x28) = 0;
  uVar1 = FUN_140043d10(0x1500);
  *(undefined8 *)(param_1 + 0x30) = uVar1;
  *(undefined4 *)(param_1 + 0x3c) = 0x10;
  *(undefined4 *)(param_1 + 0x38) = 0;
  return;
}


/* ================= FUN_140033230 @ 140033230  size=191  callers=1 callees=3 ================= */
/* CALLERS: FUN_140033060 */
/* CALLEES: FUN_140043d10, FUN_140043e60, memcpy */

undefined8 FUN_140033230(longlong param_1,uint param_2)

{
  void *_Src;
  uint uVar1;
  void *_Dst;
  uint uVar2;
  
  if (param_2 <= (uint)(*(int *)(param_1 + 0x3c) - *(int *)(param_1 + 0x38))) {
    return 0;
  }
  uVar1 = param_2;
  if (param_2 < 0x10) {
    uVar1 = 0x10;
  }
  uVar1 = uVar1 + *(int *)(param_1 + 0x3c);
  uVar2 = 0x80;
  if (uVar1 < 0x80) {
    uVar2 = uVar1;
  }
  if (uVar2 - *(int *)(param_1 + 0x38) < param_2) {
    return 0xffffffff;
  }
  _Src = *(void **)(param_1 + 0x30);
  _Dst = (void *)FUN_140043d10(uVar2 * 0x150);
  *(void **)(param_1 + 0x30) = _Dst;
  if (_Dst == (void *)0x0) {
    return 0xfffffffe;
  }
  memcpy(_Dst,_Src,(ulonglong)(uint)(*(int *)(param_1 + 0x38) * 0x150));
  FUN_140043e60(_Src);
  *(uint *)(param_1 + 0x3c) = uVar2;
  return 0;
}


/* ================= FUN_1400332f0 @ 1400332f0  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140033300 */

void FUN_1400332f0(longlong param_1)

{
  FUN_140033300(param_1 + -0x10);
  return;
}


/* ================= FUN_140033300 @ 140033300  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400332f0 */
/* CALLEES: free, ~QToolButton */

QToolButton * FUN_140033300(QToolButton *param_1,uint param_2)

{
  QToolButton::~QToolButton(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140033350 @ 140033350  size=46  callers=0 callees=1 ================= */
/* CALLEES: isFloating */

void FUN_140033350(longlong param_1)

{
  bool bVar1;
  
  bVar1 = QDockWidget::isFloating(*(QDockWidget **)(param_1 + 0x1c0));
                    /* WARNING: Could not recover jumptable at 0x000140033377. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDockWidget::setFloating(*(QDockWidget **)(param_1 + 0x1c0),!bVar1);
  return;
}


/* ================= FUN_140033390 @ 140033390  size=105  callers=0 callees=3 ================= */
/* CALLEES: fromAscii_helper, setStyleSheet, ~QString */

void FUN_140033390(QWidget *param_1,ulonglong param_2)

{
  char *pcVar1;
  Data *local_res8;
  
  (**(code **)(**(longlong **)(param_1 + 0x1b8) + 0x58))
            (*(longlong **)(param_1 + 0x1b8),param_2 ^ 1);
  pcVar1 = "QToolButton {padding: -1px;} QToolBar {border: none; rgb(255, 255, 255);}";
  if ((char)param_2 == '\0') {
    pcVar1 = 
    "QToolButton {padding: -1px;} QToolBar {border: none; background-color: rgb(200, 200, 200);}";
  }
  local_res8 = QString::fromAscii_helper(pcVar1,-1);
  QWidget::setStyleSheet(param_1,(QString *)&local_res8);
  QString::~QString((QString *)&local_res8);
  return;
}


/* ================= FUN_1400333fc @ 1400333fc  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140033450 */

void FUN_1400333fc(longlong param_1)

{
  FUN_140033450(param_1 + -0x10);
  return;
}


/* ================= FUN_140033410 @ 140033410  size=53  callers=0 callees=2 ================= */
/* CALLEES: free, ~QShortcut */

QShortcut * FUN_140033410(QShortcut *param_1,uint param_2)

{
  QShortcut::~QShortcut(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140033450 @ 140033450  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400333fc */
/* CALLEES: free, ~QToolBar */

QToolBar * FUN_140033450(QToolBar *param_1,uint param_2)

{
  QToolBar::~QToolBar(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140033490 @ 140033490  size=22  callers=1 callees=1 ================= */
/* CALLERS: FUN_140034390 */
/* CALLEES: thunk_FUN_1400342f0 */

void FUN_140033490(longlong param_1)

{
  if (*(char *)(param_1 + 0x1f1) != '\0') {
    thunk_FUN_1400342f0(*(undefined8 *)(param_1 + 0x1a8));
    return;
  }
  return;
}


/* ================= FUN_1400334b0 @ 1400334b0  size=96  callers=1 callees=4 ================= */
/* CALLERS: FUN_140033510 */
/* CALLEES: FUN_140033b60, FUN_140033db0, FUN_140034170, setMinimumSize */

void FUN_1400334b0(QWidget *param_1)

{
  FUN_140033b60();
  FUN_140034170(param_1);
  *(undefined4 *)(param_1 + 0x6c) = 0;
  *(undefined4 *)(param_1 + 0x60) = *(undefined4 *)(param_1 + 100);
  param_1[0x72] = (QWidget)0x0;
  param_1[0x70] = (QWidget)0x1;
  (**(code **)(param_1 + 0x148))(param_1 + *(int *)(param_1 + 0x150));
  QWidget::setMinimumSize(param_1,0,0);
  FUN_140033db0(param_1);
                    /* WARNING: Could not recover jumptable at 0x000140033509. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::update(param_1);
  return;
}


/* ================= FUN_140033510 @ 140033510  size=31  callers=0 callees=2 ================= */
/* CALLEES: FUN_1400334b0, FUN_1400343d0 */

void FUN_140033510(longlong param_1)

{
  FUN_1400334b0(*(undefined8 *)(param_1 + 0x70));
  FUN_1400343d0(param_1);
  return;
}


/* ================= FUN_140033530 @ 140033530  size=11  callers=3 callees=0 ================= */
/* CALLERS: FUN_140033e50, FUN_140034510, FUN_140034570 */

void FUN_140033530(QWidget *param_1)

{
  param_1[0x50] = (QWidget)0x0;
                    /* WARNING: Could not recover jumptable at 0x000140033534. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::update(param_1);
  return;
}


/* ================= FUN_140033540 @ 140033540  size=781  callers=1 callees=11 ================= */
/* CALLERS: FUN_140033e50 */
/* CALLEES: FUN_140027140, FUN_1400271d0, FUN_1400276d0, FUN_1400340e0, FUN_1400341e0, FUN_140034260, FUN_140034270, FUN_140043d20, FUN_140043e60, __security_check_cookie, memcpy */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined4
FUN_140033540(longlong param_1,char *param_2,uint param_3,int param_4,uint *param_5,int param_6)

{
  uint uVar1;
  int iVar2;
  undefined1 *_Dst;
  longlong lVar3;
  uint uVar4;
  uint uVar5;
  undefined4 uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  bool bVar11;
  undefined1 auStack_298 [32];
  uint local_278;
  uint local_274;
  uint local_270;
  int local_26c;
  int local_268;
  uint local_264;
  longlong local_260;
  code *local_258;
  code *local_250;
  undefined1 local_248 [256];
  undefined1 local_148 [256];
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_298;
  if (*param_2 == '\0') {
    uVar6 = 0xffffffff;
  }
  else {
    local_274 = param_3;
    local_26c = param_4;
    local_260 = param_1;
    uVar1 = (**(code **)(param_1 + 200))(*(int *)(param_1 + 0xd0) + param_1);
    uVar4 = uVar1;
    if ((param_6 != 0) && (param_4 == 0)) {
      uVar4 = *(int *)(param_1 + 0x60) + *param_5;
      if (uVar1 <= uVar4) {
        uVar4 = uVar4 - uVar1;
      }
      if (param_5[1] ==
          *(int *)(*(longlong *)(*(longlong *)(param_1 + 0x58) + (ulonglong)uVar4 * 8) + 0x14) - 1U)
      {
        uVar4 = *param_5 + 1;
        param_5[1] = 0;
        *param_5 = uVar4;
        if (uVar1 <= uVar4) {
          *param_5 = 0;
        }
      }
      else {
        param_5[1] = param_5[1] + 1;
      }
      uVar4 = uVar1 + 1;
    }
    uVar8 = 0x100;
    local_278 = 0x100;
    FUN_140027140(local_248,param_2,0x100);
    FUN_1400276d0(local_248);
    bVar11 = param_4 != 0;
    local_258 = (code *)&LAB_1400340d0;
    if (bVar11) {
      local_258 = FUN_1400340e0;
    }
    local_250 = FUN_140034270;
    _Dst = local_148;
    if (bVar11) {
      local_250 = FUN_140034260;
    }
    local_268 = 1;
    local_264 = uVar4;
    if (bVar11) {
      local_268 = -1;
      local_264 = 0;
    }
    uVar10 = 0;
    if (bVar11) {
      uVar10 = uVar4;
    }
    uVar4 = *param_5 + *(int *)(param_1 + 0x60);
    if (uVar1 <= uVar4) {
      uVar4 = uVar4 - uVar1;
    }
    local_270 = uVar10;
    iVar2 = (*local_250)(uVar10,local_264);
    if (iVar2 == 0) {
      uVar6 = 0xffffffff;
    }
    else {
      uVar4 = uVar4 + uVar10;
      do {
        uVar7 = uVar4;
        if (uVar1 <= uVar4) {
          uVar7 = uVar4 - uVar1;
        }
        lVar3 = *(longlong *)(*(longlong *)(local_260 + 0x58) + (ulonglong)uVar7 * 8);
        uVar9 = *(uint *)(lVar3 + 0x14);
        if (local_274 <= uVar9) {
          if (uVar8 <= uVar9) {
            if (_Dst != local_148) {
              FUN_140043e60(_Dst);
              uVar9 = *(uint *)(lVar3 + 0x14);
            }
            uVar8 = uVar9 * 2;
            local_278 = uVar8;
            _Dst = (undefined1 *)FUN_140043d20(uVar8,"CustTextView.cpp: FindNextString() tmp buf");
            if (_Dst == (undefined1 *)0x0) {
              FUN_1400341e0(local_260,"Insufficient memory");
              return 0xffffffff;
            }
          }
          memcpy(_Dst,*(void **)(lVar3 + 8),(ulonglong)uVar8);
          _Dst[*(uint *)(lVar3 + 0x14)] = 0;
          uVar9 = *(uint *)(lVar3 + 0x14);
          uVar5 = 0;
          if (uVar10 == local_270) {
            uVar5 = param_5[1];
            uVar8 = local_278;
            if (uVar9 <= uVar5) goto LAB_140033784;
            if (local_26c == 0) {
              uVar9 = uVar9 - uVar5;
            }
            else {
              _Dst[uVar5] = 0;
              uVar9 = param_5[1];
              uVar5 = 0;
            }
          }
          FUN_1400276d0(_Dst + uVar5);
          lVar3 = (*local_258)(_Dst + uVar5,uVar9,local_248,local_274);
          uVar8 = local_278;
          if (lVar3 != 0) {
            uVar4 = uVar7 - *(uint *)(local_260 + 0x60);
            if (uVar7 < *(uint *)(local_260 + 0x60)) {
              uVar4 = uVar4 + uVar1;
            }
            *param_5 = uVar4;
            uVar4 = FUN_1400271d0(lVar3,_Dst);
            param_5[1] = uVar4;
            uVar6 = 0;
            goto LAB_1400337a4;
          }
        }
LAB_140033784:
        uVar10 = uVar10 + local_268;
        uVar4 = uVar4 + local_268;
        iVar2 = (*local_250)(uVar10,local_264);
      } while (iVar2 != 0);
      uVar6 = 0xffffffff;
LAB_1400337a4:
      if ((_Dst != (undefined1 *)0x0) && (_Dst != local_148)) {
        FUN_140043e60(_Dst);
      }
    }
  }
  return uVar6;
}


/* ================= FUN_140033850 @ 140033850  size=44  callers=1 callees=3 ================= */
/* CALLERS: FUN_140034390 */
/* CALLEES: FUN_140033b60, FUN_140033db0, FUN_140034150 */

void FUN_140033850(longlong param_1)

{
  FUN_140033b60();
  if (*(char *)(param_1 + 0x1f0) != '\0') {
    FUN_140034150(param_1);
  }
  FUN_140033db0(param_1);
  return;
}


/* ================= FUN_140033880 @ 140033880  size=10  callers=1 callees=0 ================= */
/* CALLERS: FUN_140033e50 */

longlong FUN_140033880(longlong param_1)

{
  return (longlong)*(double *)(param_1 + 0x198);
}


/* ================= FUN_140033890 @ 140033890  size=597  callers=2 callees=4 ================= */
/* CALLERS: FUN_140033b90, FUN_1400343e0 */
/* CALLEES: FUN_140033b40, FUN_140033b60, FUN_140033db0, memcpy */

uint FUN_140033890(longlong param_1,undefined2 *param_2,int param_3)

{
  longlong lVar1;
  longlong lVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  void *_Src;
  uint uVar8;
  uint uVar9;
  ulonglong uVar10;
  uint uVar11;
  uint local_48;
  int *local_40;
  int *local_38 [2];
  
  uVar11 = 0;
  FUN_140033b60();
  iVar4 = FUN_140033b40(param_1);
  if ((iVar4 == 0) ||
     (uVar5 = (**(code **)(param_1 + 0xe8))(*(int *)(param_1 + 0xf0) + param_1,local_38,&local_40),
     uVar5 == 0)) goto LAB_140033acf;
  uVar11 = *(uint *)(param_1 + 0x68);
  uVar9 = *local_38[0] + *(int *)(param_1 + 0x60);
  local_48 = *local_40 + *(int *)(param_1 + 0x60);
  uVar3 = uVar9 - uVar11;
  if (uVar9 < uVar11) {
    uVar3 = uVar9;
  }
  if (uVar11 <= local_48) {
    local_48 = local_48 - uVar11;
  }
  uVar9 = local_38[0][1];
  if (uVar5 == 1) {
    uVar11 = local_40[1] - uVar9;
  }
  else {
    iVar4 = (*(int *)(*(longlong *)(*(longlong *)(param_1 + 0x58) + (ulonglong)uVar3 * 8) + 0x14) -
            uVar9) + 2;
    if (1 < uVar5 - 1) {
      uVar10 = (ulonglong)(uVar5 - 2);
      uVar11 = uVar3;
      do {
        uVar7 = uVar11 + 1;
        uVar11 = uVar7 - *(uint *)(param_1 + 0x68);
        if (uVar7 < *(uint *)(param_1 + 0x68)) {
          uVar11 = uVar7;
        }
        iVar4 = iVar4 + *(int *)(*(longlong *)
                                  (*(longlong *)(param_1 + 0x58) + (ulonglong)uVar11 * 8) + 0x14) +
                        2;
        uVar10 = uVar10 - 1;
      } while (uVar10 != 0);
    }
    uVar11 = iVar4 + local_40[1];
  }
  if ((param_2 == (undefined2 *)0x0) || (param_3 == 0)) goto LAB_140033acf;
  uVar7 = uVar11;
  if (param_3 - 1U <= uVar11) {
    uVar7 = param_3 - 1U;
  }
  lVar1 = *(longlong *)(*(longlong *)(param_1 + 0x58) + (ulonglong)uVar3 * 8);
  lVar2 = *(longlong *)(lVar1 + 8);
  if (uVar5 == 1) {
    _Src = (void *)(lVar2 + (ulonglong)uVar9);
LAB_140033aaa:
    memcpy(param_2,_Src,(ulonglong)uVar7);
    param_2 = (undefined2 *)((longlong)param_2 + (ulonglong)uVar7);
  }
  else {
    uVar6 = *(int *)(lVar1 + 0x14) - uVar9;
    uVar8 = uVar7;
    if (uVar6 < uVar7) {
      uVar8 = uVar6;
    }
    memcpy(param_2,(void *)(lVar2 + (ulonglong)uVar9),(ulonglong)uVar8);
    param_2 = (undefined2 *)((longlong)param_2 + (ulonglong)uVar8);
    uVar8 = uVar7 - uVar8;
    if (1 < uVar8) {
      *param_2 = 0xa0d;
      while( true ) {
        uVar5 = uVar5 - 1;
        param_2 = param_2 + 1;
        uVar8 = uVar8 - 2;
        if (uVar5 < 2) break;
        uVar9 = uVar3 + 1;
        uVar3 = uVar9 - *(int *)(param_1 + 0x68);
        if (uVar9 < *(uint *)(param_1 + 0x68)) {
          uVar3 = uVar9;
        }
        lVar1 = *(longlong *)(*(longlong *)(param_1 + 0x58) + (ulonglong)uVar3 * 8);
        uVar9 = uVar8;
        if (*(uint *)(lVar1 + 0x14) < uVar8) {
          uVar9 = *(uint *)(lVar1 + 0x14);
        }
        memcpy(param_2,*(void **)(lVar1 + 8),(ulonglong)uVar9);
        param_2 = (undefined2 *)((longlong)param_2 + (ulonglong)uVar9);
        uVar8 = uVar8 - uVar9;
        if (uVar8 < 2) break;
        *param_2 = 0xa0d;
      }
      uVar7 = local_40[1];
      if (uVar8 < (uint)local_40[1]) {
        uVar7 = uVar8;
      }
      _Src = *(void **)(*(longlong *)(*(longlong *)(param_1 + 0x58) + (ulonglong)local_48 * 8) + 8);
      goto LAB_140033aaa;
    }
  }
  *(undefined1 *)param_2 = 0;
LAB_140033acf:
  FUN_140033db0(param_1);
  return uVar11;
}


/* ================= FUN_140033af0 @ 140033af0  size=69  callers=1 callees=0 ================= */
/* CALLERS: FUN_140033e50 */

void FUN_140033af0(longlong param_1,uint *param_2,int *param_3,int *param_4)

{
  double dVar1;
  uint uVar2;
  
  uVar2 = *param_2;
  dVar1 = *(double *)(param_1 + 0x198);
  *param_3 = (int)((double)param_2[1] * *(double *)(param_1 + 0x188) + 5.0);
  *param_4 = (int)((double)uVar2 * dVar1 + 5.0);
  return;
}


/* ================= FUN_140033b40 @ 140033b40  size=27  callers=3 callees=0 ================= */
/* CALLERS: FUN_140033890, FUN_140034280, FUN_1400343e0 */

undefined8 FUN_140033b40(longlong param_1)

{
  undefined8 uVar1;
  
  uVar1 = 1;
  if ((*(int *)(param_1 + 0x30) == *(int *)(param_1 + 0x38)) &&
     (uVar1 = 1, *(int *)(param_1 + 0x34) == *(int *)(param_1 + 0x3c))) {
    uVar1 = 0;
  }
  return uVar1;
}


/* ================= FUN_140033b60 @ 140033b60  size=17  callers=8 callees=1 ================= */
/* CALLERS: FUN_1400334b0, FUN_140033850, FUN_140033890, FUN_140033b80, FUN_140033b90, FUN_140033cc0, FUN_140033d50, FUN_140033dd0 */
/* CALLEES: FUN_1400443e0 */

void FUN_140033b60(longlong param_1)

{
  FUN_1400443e0(*(undefined8 *)(param_1 + 0x1d0),0xffffffff);
  return;
}


/* ================= FUN_140033b80 @ 140033b80  size=9  callers=2 callees=1 ================= */
/* CALLERS: FUN_140033e50, FUN_1400343e0 */
/* CALLEES: FUN_140033b60 */

void FUN_140033b80(longlong param_1)

{
  FUN_140033b60(*(undefined8 *)(param_1 + 0x70));
  return;
}


/* ================= FUN_140033b90 @ 140033b90  size=300  callers=0 callees=12 ================= */
/* CALLEES: FUN_140033890, FUN_140033b60, FUN_140033db0, FUN_1400341e0, FUN_140035970, FUN_140043d20, FUN_140043e60, __security_check_cookie, clipboard, fromAscii_helper, setText, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140033b90(longlong param_1)

{
  uint uVar1;
  char *pcVar2;
  QClipboard *this;
  QString *pQVar3;
  undefined1 auStack_88 [32];
  Data *local_68 [2];
  char local_58 [64];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_88;
  FUN_140033b60();
  uVar1 = FUN_140033890(param_1,local_58);
  if (uVar1 < 0x40) {
    pcVar2 = local_58;
  }
  else {
    pcVar2 = (char *)FUN_140043d20(uVar1 + 1,"CustTextView.cpp: Tmp. copy buffer");
    if (pcVar2 == (char *)0x0) {
      FUN_1400341e0(param_1,"Insufficient memory");
      FUN_140033db0(param_1);
      return;
    }
    FUN_140033890(param_1,pcVar2);
  }
  this = QApplication::clipboard();
  if (*(char *)(param_1 + 499) == '\0') {
    local_68[0] = QString::fromAscii_helper(pcVar2,-1);
    QClipboard::setText(this,(QString *)local_68,0);
  }
  else {
    pQVar3 = (QString *)FUN_140035970(local_68,pcVar2);
    QClipboard::setText(this,pQVar3,0);
  }
  QString::~QString((QString *)local_68);
  FUN_140033db0(param_1);
  if ((pcVar2 != (char *)0x0) && (pcVar2 != local_58)) {
    FUN_140043e60(pcVar2);
  }
  return;
}


/* ================= FUN_140033cc0 @ 140033cc0  size=93  callers=0 callees=3 ================= */
/* CALLEES: FUN_140033b60, FUN_140033db0, FUN_1400341c0 */

void FUN_140033cc0(QWidget *param_1)

{
  int iVar1;
  
  FUN_140033b60();
  *(undefined8 *)(param_1 + 0x30) = 0;
  iVar1 = (**(code **)(param_1 + 200))(param_1 + *(int *)(param_1 + 0xd0));
  if (iVar1 != 0) {
    *(int *)(param_1 + 0x38) = iVar1 + -1;
    iVar1 = FUN_1400341c0(param_1);
    *(undefined4 *)(param_1 + 0x3c) =
         *(undefined4 *)(*(longlong *)(*(longlong *)(param_1 + 0x58) + (longlong)iVar1 * 8) + 0x14);
    FUN_140033db0(param_1);
  }
                    /* WARNING: Could not recover jumptable at 0x000140033d16. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::update(param_1);
  return;
}


/* ================= FUN_140033d30 @ 140033d30  size=22  callers=1 callees=0 ================= */
/* CALLERS: FUN_140033e50 */

void FUN_140033d30(QWidget *param_1,undefined8 *param_2,undefined4 param_3)

{
  param_1[0x50] = (QWidget)0x1;
  *(undefined8 *)(param_1 + 0x44) = *param_2;
  *(undefined4 *)(param_1 + 0x4c) = param_3;
                    /* WARNING: Could not recover jumptable at 0x000140033d3f. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::update(param_1);
  return;
}


/* ================= FUN_140033d50 @ 140033d50  size=85  callers=2 callees=2 ================= */
/* CALLERS: FUN_140033e50, FUN_140034570 */
/* CALLEES: FUN_140033b60, FUN_140033db0 */

void FUN_140033d50(QWidget *param_1,undefined8 *param_2,int param_3)

{
  undefined8 uVar1;
  
  FUN_140033b60();
  *(undefined8 *)(param_1 + 0x30) = *param_2;
  uVar1 = *param_2;
  *(undefined8 *)(param_1 + 0x38) = uVar1;
  *(int *)(param_1 + 0x3c) = (int)((ulonglong)uVar1 >> 0x20) + param_3;
  FUN_140033db0(param_1);
                    /* WARNING: Could not recover jumptable at 0x000140033d9e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::update(param_1);
  return;
}


/* ================= FUN_140033db0 @ 140033db0  size=12  callers=8 callees=1 ================= */
/* CALLERS: FUN_1400334b0, FUN_140033850, FUN_140033890, FUN_140033b90, FUN_140033cc0, FUN_140033d50, FUN_140033dc0, FUN_140033dd0 */
/* CALLEES: FUN_140044370 */

void FUN_140033db0(longlong param_1)

{
  FUN_140044370(*(undefined8 *)(param_1 + 0x1d0));
  return;
}


/* ================= FUN_140033dc0 @ 140033dc0  size=9  callers=2 callees=1 ================= */
/* CALLERS: FUN_140033e50, FUN_1400343e0 */
/* CALLEES: FUN_140033db0 */

void FUN_140033dc0(longlong param_1)

{
  FUN_140033db0(*(undefined8 *)(param_1 + 0x70));
  return;
}


/* ================= FUN_140033dd0 @ 140033dd0  size=125  callers=1 callees=3 ================= */
/* CALLERS: FUN_140034390 */
/* CALLEES: FUN_140033b60, FUN_140033db0, setMinimumSize */

void FUN_140033dd0(QWidget *param_1)

{
  double dVar1;
  uint uVar2;
  uint uVar3;
  
  FUN_140033b60();
  uVar2 = *(uint *)(param_1 + 0x6c);
  dVar1 = *(double *)(param_1 + 0x188);
  uVar3 = (**(code **)(param_1 + 200))(param_1 + *(int *)(param_1 + 0xd0));
  QWidget::setMinimumSize
            (param_1,(int)((double)uVar2 * dVar1 + 10.0),
             (int)((double)uVar3 * *(double *)(param_1 + 0x198) + 10.0));
  FUN_140033db0(param_1);
  return;
}


/* ================= FUN_140033e50 @ 140033e50  size=630  callers=0 callees=25 ================= */
/* CALLEES: FUN_140027140, FUN_1400277b0, FUN_140033530, FUN_140033540, FUN_140033880, FUN_140033af0, FUN_140033b80, FUN_140033d30, FUN_140033d50, FUN_140033dc0, QBrush, QPalette, __security_check_cookie, ensureVisible, operator=, palette, setBrush, setPalette, setRgb, text, toAscii, ~QBrush, ~QByteArray, ~QPalette, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140033e50(QScrollArea *param_1,undefined4 param_2)

{
  QScrollArea *pQVar1;
  uint uVar2;
  undefined8 uVar3;
  undefined4 uVar4;
  int iVar5;
  QString *this;
  longlong *plVar6;
  QPalette *pQVar7;
  QBrush *pQVar8;
  undefined1 auStackY_198 [32];
  int local_168;
  int local_164;
  QString local_160 [8];
  QPalette local_158 [16];
  QByteArray local_148 [16];
  undefined1 local_138 [256];
  ulonglong local_38;
  
  local_38 = DAT_140096fe8 ^ (ulonglong)auStackY_198;
  QPalette::QPalette(local_158);
  local_164 = 0;
  local_168 = 0;
  uVar3 = *(undefined8 *)(param_1 + 0x70);
  FUN_140033b80(param_1);
  this = (QString *)QLineEdit::text(*(QLineEdit **)(param_1 + 0x50));
  plVar6 = (longlong *)QString::toAscii(this);
  FUN_140027140(local_138,*(undefined8 *)(*plVar6 + 0x10),0x100);
  QByteArray::~QByteArray(local_148);
  QString::~QString(local_160);
  uVar4 = FUN_1400277b0(local_138);
  pQVar1 = param_1 + 0x40;
  iVar5 = FUN_140033540(uVar3,local_138,uVar4,param_2);
  if (iVar5 < 0) {
    FUN_140033dc0(param_1);
    pQVar7 = QWidget::palette(*(QWidget **)(param_1 + 0x48));
    QPalette::operator=(local_158,pQVar7);
    QColor::setRgb((QColor *)local_148,0xf4,0xd0,0x3f,0xff);
    pQVar8 = (QBrush *)QBrush::QBrush((QBrush *)local_160,(QColor *)local_148,1);
    QPalette::setBrush(local_158,5,10,pQVar8);
    QBrush::~QBrush((QBrush *)local_160);
    QWidget::setPalette(*(QWidget **)(param_1 + 0x48),local_158);
    FUN_140033530(uVar3);
    param_1[0x58] = (QScrollArea)0x0;
    *(undefined8 *)pQVar1 = 0;
  }
  else {
    FUN_140033af0(uVar3,pQVar1,&local_164,&local_168);
    uVar2 = *(uint *)(*(longlong *)(*(longlong *)(param_1 + 0x48) + 0x20) + 8);
    FUN_140033d50(uVar3,pQVar1,uVar4);
    if ((uVar2 >> 0xf & 1) != 0) {
      FUN_140033d30(uVar3,pQVar1,uVar4);
    }
    param_1[0x58] = (QScrollArea)0x1;
    FUN_140033dc0(param_1);
    pQVar7 = QWidget::palette(*(QWidget **)(param_1 + 0x48));
    QPalette::operator=(local_158,pQVar7);
    pQVar8 = (QBrush *)QBrush::QBrush((QBrush *)local_160,(QColor *)(param_1 + 0x30),1);
    QPalette::setBrush(local_158,5,10,pQVar8);
    QBrush::~QBrush((QBrush *)local_160);
    QWidget::setPalette(*(QWidget **)(param_1 + 0x48),local_158);
    iVar5 = FUN_140033880(uVar3);
    QScrollArea::ensureVisible(param_1,local_164,local_168,5,iVar5 + 5);
  }
  QPalette::~QPalette(local_158);
  return;
}


/* ================= FUN_1400340e0 @ 1400340e0  size=102  callers=1 callees=1 ================= */
/* CALLERS: FUN_140033540 */
/* CALLEES: memcmp */

void * FUN_1400340e0(void *param_1,uint param_2,void *param_3,uint param_4)

{
  int iVar1;
  void *_Buf1;
  
  _Buf1 = (void *)(((ulonglong)param_2 - (ulonglong)param_4) + (longlong)param_1);
  while( true ) {
    if (_Buf1 < param_1) {
      return (void *)0x0;
    }
    iVar1 = memcmp(_Buf1,param_3,(ulonglong)param_4);
    if (iVar1 == 0) break;
    _Buf1 = (void *)((longlong)_Buf1 + -1);
  }
  return _Buf1;
}


/* ================= FUN_140034150 @ 140034150  size=27  callers=1 callees=1 ================= */
/* CALLERS: FUN_140033850 */
/* CALLEES: FUN_14003b6b0 */

void FUN_140034150(longlong param_1)

{
  FUN_14003b6b0();
  *(undefined1 *)(param_1 + 0x1f0) = 0;
  return;
}


/* ================= FUN_140034170 @ 140034170  size=67  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400334b0 */
/* CALLEES: FUN_1400341c0 */

void FUN_140034170(longlong param_1)

{
  code *pcVar1;
  longlong lVar2;
  int iVar3;
  
  pcVar1 = *(code **)(param_1 + 0x1c0);
  if ((pcVar1 != (code *)0x0) && (*(char *)(param_1 + 0x70) == '\0')) {
    iVar3 = FUN_1400341c0();
    lVar2 = *(longlong *)(*(longlong *)(param_1 + 0x58) + (longlong)iVar3 * 8);
    (*pcVar1)(*(undefined8 *)(lVar2 + 8),*(undefined4 *)(lVar2 + 0x14));
  }
  return;
}


/* ================= FUN_1400341c0 @ 1400341c0  size=25  callers=2 callees=0 ================= */
/* CALLERS: FUN_140033cc0, FUN_140034170 */

int FUN_1400341c0(longlong param_1)

{
  if (*(int *)(param_1 + 100) != 0) {
    return *(int *)(param_1 + 100) + -1;
  }
  if (*(char *)(param_1 + 0x72) != '\0') {
    return *(int *)(param_1 + 0x68) + -1;
  }
  return 0;
}


/* ================= FUN_1400341e0 @ 1400341e0  size=116  callers=2 callees=2 ================= */
/* CALLERS: FUN_140033540, FUN_140033b90 */
/* CALLEES: FUN_14002c930, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_1400341e0(longlong param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  undefined8 local_res18;
  undefined8 local_res20;
  undefined1 auStack_238 [32];
  undefined1 local_218 [512];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_238;
  if (*(longlong *)(param_1 + 0x1b0) != 0) {
    local_res18 = param_3;
    local_res20 = param_4;
    FUN_14002c930(local_218,0x200,param_2,&local_res18);
    (**(code **)(param_1 + 0x1b0))(param_1 + 0x78,local_218);
  }
  return;
}


/* ================= FUN_140034260 @ 140034260  size=8  callers=1 callees=0 ================= */
/* CALLERS: FUN_140033540 */

bool FUN_140034260(int param_1,int param_2)

{
  return param_2 <= param_1;
}


/* ================= FUN_140034270 @ 140034270  size=8  callers=1 callees=0 ================= */
/* CALLERS: FUN_140033540 */

bool FUN_140034270(int param_1,int param_2)

{
  return param_1 < param_2;
}


/* ================= FUN_140034280 @ 140034280  size=94  callers=0 callees=4 ================= */
/* CALLEES: FUN_140033b40, exec, mapToGlobal, setEnabled */

void FUN_140034280(QWidget *param_1)

{
  int iVar1;
  undefined8 *puVar2;
  undefined8 local_res8;
  QPoint local_res18 [16];
  
  iVar1 = FUN_140033b40(*(undefined8 *)(param_1 + 0x70));
  QAction::setEnabled(*(QAction **)(param_1 + 0x60),iVar1 != 0);
  puVar2 = (undefined8 *)QWidget::mapToGlobal(param_1,local_res18);
  local_res8 = *puVar2;
  QMenu::exec(*(QMenu **)(param_1 + 0x68),(QPoint *)&local_res8,(QAction *)0x0);
  return;
}


/* ================= FUN_1400342f0 @ 1400342f0  size=61  callers=1 callees=2 ================= */
/* CALLERS: thunk_FUN_1400342f0 */
/* CALLEES: maximum, verticalScrollBar */

void FUN_1400342f0(QAbstractScrollArea *param_1)

{
  int iVar1;
  QScrollBar *pQVar2;
  
  pQVar2 = QAbstractScrollArea::verticalScrollBar(param_1);
  iVar1 = QAbstractSlider::maximum((QAbstractSlider *)pQVar2);
  pQVar2 = QAbstractScrollArea::verticalScrollBar(param_1);
                    /* WARNING: Could not recover jumptable at 0x000140034326. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QAbstractSlider::setValue((QAbstractSlider *)pQVar2,iVar1);
  return;
}


/* ================= FUN_140034330 @ 140034330  size=61  callers=0 callees=2 ================= */
/* CALLEES: minimum, verticalScrollBar */

void FUN_140034330(QAbstractScrollArea *param_1)

{
  int iVar1;
  QScrollBar *pQVar2;
  
  pQVar2 = QAbstractScrollArea::verticalScrollBar(param_1);
  iVar1 = QAbstractSlider::minimum((QAbstractSlider *)pQVar2);
  pQVar2 = QAbstractScrollArea::verticalScrollBar(param_1);
                    /* WARNING: Could not recover jumptable at 0x000140034366. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QAbstractSlider::setValue((QAbstractSlider *)pQVar2,iVar1);
  return;
}


/* ================= FUN_140034390 @ 140034390  size=52  callers=0 callees=3 ================= */
/* CALLEES: FUN_140033490, FUN_140033850, FUN_140033dd0 */

void FUN_140034390(longlong param_1)

{
  FUN_140033dd0(*(undefined8 *)(param_1 + 0x70));
  FUN_140033490(*(undefined8 *)(param_1 + 0x70));
  FUN_140033850(*(undefined8 *)(param_1 + 0x70));
                    /* WARNING: Could not recover jumptable at 0x0001400343bd. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::update(*(QWidget **)(param_1 + 0x70));
  return;
}


/* ================= FUN_1400343d0 @ 1400343d0  size=11  callers=1 callees=0 ================= */
/* CALLERS: FUN_140033510 */

void FUN_1400343d0(longlong param_1)

{
  *(undefined1 *)(param_1 + 0x58) = 0;
  *(undefined8 *)(param_1 + 0x40) = 0;
  return;
}


/* ================= FUN_1400343e0 @ 1400343e0  size=298  callers=0 callees=14 ================= */
/* CALLEES: FUN_140033890, FUN_140033b40, FUN_140033b80, FUN_140033dc0, FUN_1400345e0, FUN_140043d20, FUN_140043e60, __security_check_cookie, fromAscii_helper, selectAll, setFocus, setText, show, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_1400343e0(longlong param_1)

{
  undefined8 uVar1;
  int iVar2;
  uint uVar3;
  char *pcVar4;
  uint uVar5;
  undefined1 auStack_88 [32];
  Data *local_68 [2];
  char local_58 [64];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_88;
  uVar1 = *(undefined8 *)(param_1 + 0x70);
  FUN_140033b80();
  iVar2 = FUN_140033b40(uVar1);
  if (iVar2 != 0) {
    uVar3 = FUN_140033890(uVar1,local_58,0x40);
    if (uVar3 < 0x40) {
      pcVar4 = local_58;
    }
    else {
      uVar5 = 0x100;
      if (uVar3 + 1 < 0x100) {
        uVar5 = uVar3 + 1;
      }
      pcVar4 = (char *)FUN_140043d20(uVar5,"CustTextView.cpp: Tmp. \'find\' buffer");
      FUN_140033890(uVar1,pcVar4,uVar5);
      if (pcVar4 == (char *)0x0) goto LAB_1400344b1;
    }
    local_68[0] = QString::fromAscii_helper(pcVar4,-1);
    QLineEdit::setText(*(QLineEdit **)(param_1 + 0x50),(QString *)local_68);
    QString::~QString((QString *)local_68);
    if (pcVar4 != local_58) {
      FUN_140043e60(pcVar4);
    }
  }
LAB_1400344b1:
  FUN_140033dc0(param_1);
  QWidget::show(*(QWidget **)(param_1 + 0x48));
  QWidget::setFocus(*(QWidget **)(param_1 + 0x50),4);
  QLineEdit::selectAll(*(QLineEdit **)(param_1 + 0x50));
  FUN_1400345e0(param_1);
  return;
}


/* ================= FUN_140034510 @ 140034510  size=33  callers=0 callees=2 ================= */
/* CALLEES: FUN_140033530, hide */

void FUN_140034510(longlong param_1)

{
  QWidget::hide(*(QWidget **)(param_1 + 0x48));
  FUN_140033530(*(undefined8 *)(param_1 + 0x70));
  return;
}


/* ================= FUN_140034570 @ 140034570  size=102  callers=0 callees=3 ================= */
/* CALLEES: FUN_140033530, FUN_140033d50, hide */

void FUN_140034570(QWidget *param_1)

{
  QWidget *this;
  undefined8 local_res8 [4];
  
  FUN_140033530(*(undefined8 *)(param_1 + 0x70));
  this = *(QWidget **)(param_1 + 0x48);
  if ((*(uint *)(*(longlong *)(this + 0x20) + 8) >> 0xf & 1) == 0) {
    local_res8[0] = 0;
    FUN_140033d50(*(undefined8 *)(param_1 + 0x70),local_res8,0);
    *(undefined8 *)(param_1 + 0x40) = 0;
    this = *(QWidget **)(param_1 + 0x48);
    param_1[0x58] = (QWidget)0x0;
  }
  QWidget::hide(this);
                    /* WARNING: Could not recover jumptable at 0x0001400345cf. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::setFocus(param_1,7);
  return;
}


/* ================= FUN_1400345e0 @ 1400345e0  size=75  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400343e0 */
/* CALLEES: move, viewport */

void FUN_1400345e0(QAbstractScrollArea *param_1)

{
  longlong lVar1;
  QWidget *pQVar2;
  int local_res8 [8];
  
  pQVar2 = QAbstractScrollArea::viewport(param_1);
  local_res8[1] = 5;
  lVar1 = *(longlong *)(*(QWidget **)(param_1 + 0x48) + 0x20);
  local_res8[0] =
       ((*(int *)(*(longlong *)(pQVar2 + 0x20) + 0x1c) - *(int *)(lVar1 + 0x1c)) -
       *(int *)(*(longlong *)(pQVar2 + 0x20) + 0x14)) + *(int *)(lVar1 + 0x14) + -5;
  QWidget::move(*(QWidget **)(param_1 + 0x48),(QPoint *)local_res8);
  return;
}


/* ================= FUN_140034630 @ 140034630  size=1547  callers=1 callees=47 ================= */
/* CALLERS: FUN_14004d0f0 */
/* CALLEES: FUN_1400359e0, FUN_140035cf0, FUN_140035d30, FUN_140035e90, FUN_140035ef0, FUN_140035ff0, FUN_140036640, FUN_1400366c0, FUN_140036bd0, FUN_140036c40, FUN_140036e00, FUN_140036f30, FUN_140043d20, FUN_140058d94, QDialog, QIcon, QKeySequence, QPixmap, QShortcut, QTimer, __security_check_cookie, addWidget, connect, fromAscii_helper, hide, operator=, setAttribute, setChecked, setContentsMargins, setFixedSize, setInterval, setLayout, setMinimumWidth, setModal, setPixmap, setText, setTextInteractionFlags, setWindowTitle, show, sizeHint, standardIcon, start, style, ~QIcon, ~QKeySequence, ~QPixmap, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

QWidget * FUN_140034630(QWidget *param_1,undefined8 param_2,undefined8 *param_3)

{
  int iVar1;
  QStyle *this;
  QIcon *pQVar2;
  QLabel *pQVar3;
  undefined8 uVar4;
  longlong lVar5;
  QAbstractButton *this_00;
  QGridLayout *pQVar6;
  QWidget *this_01;
  QLayout *pQVar7;
  QTimer *this_02;
  QSize *pQVar8;
  QShortcut *this_03;
  QKeySequence *pQVar9;
  char *pcVar10;
  uint uVar11;
  QTimer *this_04;
  QShortcut *pQVar12;
  undefined1 auStackY_d8 [32];
  Data *local_98;
  Data *local_90;
  QTimer *local_88;
  QIcon local_80 [8];
  QPixmap local_78 [24];
  QWidget *local_60;
  QShortcut *local_58;
  undefined8 local_50;
  undefined8 uStack_48;
  ulonglong local_40;
  
  local_40 = DAT_140096fe8 ^ (ulonglong)auStackY_d8;
  pQVar12 = (QShortcut *)0x0;
  local_98 = (Data *)((ulonglong)local_98 & 0xffffffff00000000);
  local_60 = param_1;
  QDialog::QDialog();
  *(undefined ***)param_1 = DIALOG_CustMsgBox::vftable;
  *(undefined ***)(param_1 + 0x10) = DIALOG_CustMsgBox::vftable;
  QIcon::QIcon(local_80);
  QPixmap::QPixmap(local_78);
  QWidget::hide(param_1);
  *(undefined8 *)(param_1 + 0x28) = 0;
  *(undefined8 *)(param_1 + 0x30) = 0;
  *(undefined8 *)(param_1 + 0x38) = 0;
  *(undefined8 *)(param_1 + 0x40) = 0;
  *(undefined8 *)(param_1 + 0x48) = 0;
  *(undefined8 *)(param_1 + 0x50) = 0;
  *(undefined8 *)(param_1 + 0x58) = 0;
  param_3[8] = 0;
  *(undefined4 *)(param_3 + 9) = 0;
  iVar1 = *(int *)(param_3 + 6);
  if (iVar1 == 0x100) {
    uVar11 = 0xb;
  }
  else if (iVar1 == 0x200) {
    uVar11 = 0xc;
  }
  else if (iVar1 == 0x300) {
    uVar11 = 10;
  }
  else {
    pQVar3 = (QLabel *)pQVar12;
    if (iVar1 != 0x400) goto LAB_140034795;
    uVar11 = 9;
  }
  this = QWidget::style(param_1);
  pQVar2 = (QIcon *)QStyle::standardIcon
                              (this,(StandardPixmap)&local_90,(QStyleOption *)(ulonglong)uVar11,
                               (QWidget *)0x0);
  QIcon::operator=(local_80,pQVar2);
  QIcon::~QIcon((QIcon *)&local_90);
  pQVar3 = (QLabel *)FUN_140035ff0(param_1,0);
  iVar1 = FUN_140036c40(local_80,local_78);
  if (-1 < iVar1) {
    QLabel::setPixmap(pQVar3,local_78);
    pQVar8 = (QSize *)(**(code **)(*(longlong *)pQVar3 + 0x60))(pQVar3,&local_88);
    QWidget::setFixedSize((QWidget *)pQVar3,pQVar8);
  }
LAB_140034795:
  uVar4 = FUN_140035ff0(param_1,0);
  *(undefined8 *)(param_1 + 0x58) = uVar4;
  lVar5 = FUN_140043d20(0x8000,"Temp. MsgBox string");
  *(longlong *)(param_1 + 0x40) = lVar5;
  if (lVar5 == 0) {
    pcVar10 = "<Insufficient memory for message box>";
  }
  else {
    FUN_1400359e0(param_3[1],lVar5,0x8000);
    FUN_140036e00(*(undefined8 *)(param_1 + 0x58),1);
    pcVar10 = *(char **)(param_1 + 0x40);
  }
  local_98 = QString::fromAscii_helper(pcVar10,-1);
  QLabel::setText(*(QLabel **)(param_1 + 0x58),(QString *)&local_98);
  QString::~QString((QString *)&local_98);
  local_98 = (Data *)CONCAT44(local_98._4_4_,(*(char *)(param_3 + 7) != '\0') + 0xc);
  QLabel::setTextInteractionFlags(*(QLabel **)(param_1 + 0x58),&local_98);
  if (param_3[4] != 0) {
    this_00 = (QAbstractButton *)FUN_140035d30(param_1);
    *(QAbstractButton **)(param_1 + 0x28) = this_00;
    QAbstractButton::setChecked(this_00,*(int *)(param_3 + 5) != 0);
  }
  *(undefined8 *)(param_1 + 0x48) = 0;
  *(undefined8 *)(param_1 + 0x50) = 0;
  if (param_3[2] != 0) {
    uVar4 = FUN_140035cf0(param_1);
    *(undefined8 *)(param_1 + 0x48) = uVar4;
  }
  if (param_3[3] != 0) {
    uVar4 = FUN_140035cf0(param_1);
    *(undefined8 *)(param_1 + 0x50) = uVar4;
  }
  local_50 = 0;
  uStack_48 = 0;
  pQVar6 = (QGridLayout *)FUN_140035ef0(0,&local_50);
  if (*(longlong *)(param_1 + 0x48) != 0) {
    local_98 = (Data *)((ulonglong)local_98 & 0xffffffff00000000);
    QGridLayout::addWidget(pQVar6);
  }
  if (*(longlong *)(param_1 + 0x50) != 0) {
    local_98 = (Data *)((ulonglong)local_98 & 0xffffffff00000000);
    QGridLayout::addWidget(pQVar6);
  }
  this_01 = (QWidget *)FUN_140036640(param_1);
  QWidget::setContentsMargins(this_01,0,0,0,0);
  pQVar7 = (QLayout *)FUN_140035e90(0);
  if (pQVar3 != (QLabel *)0x0) {
    local_98 = (Data *)((ulonglong)local_98 & 0xffffffff00000000);
    QGridLayout::addWidget((QGridLayout *)pQVar7,pQVar3,0,0);
  }
  local_98 = (Data *)((ulonglong)local_98 & 0xffffffff00000000);
  QGridLayout::addWidget((QGridLayout *)pQVar7,*(undefined8 *)(param_1 + 0x58),0,1);
  if (*(longlong *)(param_1 + 0x28) != 0) {
    local_98 = (Data *)((ulonglong)local_98 & 0xffffffff00000000);
    QGridLayout::addWidget((QGridLayout *)pQVar7,*(longlong *)(param_1 + 0x28),1);
  }
  local_98 = (Data *)CONCAT44(local_98._4_4_,2);
  QGridLayout::addWidget((QGridLayout *)pQVar7,this_01,2);
  QWidget::setLayout(param_1,pQVar7);
  FUN_1400366c0(param_1,1);
  QDialog::setModal((QDialog *)param_1,true);
  *(undefined8 **)(param_1 + 0x38) = param_3;
  if (0 < *(int *)((longlong)param_3 + 0x34)) {
    this_02 = (QTimer *)FUN_140058d94(0x20);
    this_04 = (QTimer *)pQVar12;
    local_88 = this_02;
    if (this_02 != (QTimer *)0x0) {
      QTimer::QTimer(this_02,(QObject *)param_1);
      *(undefined ***)this_02 = QTimer::vftable;
      this_04 = this_02;
    }
    *(QTimer **)(param_1 + 0x30) = this_04;
    QTimer::setInterval(this_04,10);
    QObject::connect(*(QObject **)(param_1 + 0x30),"2timeout(void)",(QObject *)param_1,
                     "1_SlotOnTimer(void)",0);
    QTimer::start(*(QTimer **)(param_1 + 0x30));
  }
  QWidget::setAttribute(param_1,0x37,true);
  pQVar8 = (QSize *)QDialog::sizeHint((QDialog *)param_1);
  QWidget::setFixedSize(param_1,pQVar8);
  local_98 = QString::fromAscii_helper((char *)*param_3,-1);
  QWidget::setWindowTitle(param_1,(QString *)&local_98);
  QString::~QString((QString *)&local_98);
  iVar1 = FUN_140036bd0(param_1,*param_3);
  QWidget::setMinimumWidth(param_1,iVar1 + 0x96);
  *(undefined4 *)((longlong)param_3 + 0x44) = *(undefined4 *)((longlong)param_3 + 0x2c);
  QObject::connect((QObject *)param_1,"2accepted(void)",(QObject *)param_1,"1accept(void)",0);
  QObject::connect((QObject *)param_1,"2rejected(void)",(QObject *)param_1,"1reject(void)",0);
  this_03 = (QShortcut *)FUN_140058d94(0x10);
  pQVar3 = (QLabel *)pQVar12;
  local_58 = this_03;
  if (this_03 != (QShortcut *)0x0) {
    local_90 = QString::fromAscii_helper("Ctrl+C",-1);
    local_98._0_4_ = 1;
    pQVar9 = (QKeySequence *)
             QKeySequence::QKeySequence((QKeySequence *)&local_88,(QString *)&local_90);
    local_98 = (Data *)CONCAT44(local_98._4_4_,3);
    QShortcut::QShortcut(this_03,pQVar9,param_1,(char *)0x0,(char *)0x0,1);
    *(undefined ***)this_03 = QShortcut::vftable;
    pQVar12 = this_03;
    pQVar3 = (QLabel *)0x3;
  }
  if (((ulonglong)pQVar3 & 2) != 0) {
    pQVar3 = (QLabel *)(ulonglong)((uint)pQVar3 & 0xfffffffd);
    QKeySequence::~QKeySequence((QKeySequence *)&local_88);
  }
  if (((ulonglong)pQVar3 & 1) != 0) {
    QString::~QString((QString *)&local_90);
  }
  QObject::connect((QObject *)pQVar12,"2activated(void)",(QObject *)param_1,"1_SlotOnCopy(void)",0);
  FUN_140036f30(param_1);
  QWidget::show(param_1);
  QPixmap::~QPixmap(local_78);
  QIcon::~QIcon(local_80);
  return param_1;
}


/* ================= FUN_140034c40 @ 140034c40  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_140034c70 */

void FUN_140034c40(QDialog *param_1)

{
  *(undefined ***)param_1 = DIALOG_CustMsgBox::vftable;
  *(undefined ***)(param_1 + 0x10) = DIALOG_CustMsgBox::vftable;
                    /* WARNING: Could not recover jumptable at 0x000140034c55. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::~QDialog(param_1);
  return;
}


/* ================= FUN_140034c5c @ 140034c5c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140034c70 */

void FUN_140034c5c(longlong param_1)

{
  FUN_140034c70(param_1 + -0x10);
  return;
}


/* ================= FUN_140034c70 @ 140034c70  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_140034c5c */
/* CALLEES: FUN_140034c40, free */

void * FUN_140034c70(void *param_1,ulonglong param_2)

{
  FUN_140034c40();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140034cb0 @ 140034cb0  size=53  callers=0 callees=2 ================= */
/* CALLEES: free, ~QTimer */

QTimer * FUN_140034cb0(QTimer *param_1,uint param_2)

{
  QTimer::~QTimer(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140034cf0 @ 140034cf0  size=18  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003b6f0 */

void FUN_140034cf0(QWidget *param_1)

{
  *(undefined4 *)(*(longlong *)(param_1 + 0x38) + 0x44) = 0;
                    /* WARNING: Could not recover jumptable at 0x000140034cfb. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::close(param_1);
  return;
}


/* ================= FUN_140034d10 @ 140034d10  size=18  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003b6f0 */

void FUN_140034d10(QWidget *param_1)

{
  *(undefined4 *)(*(longlong *)(param_1 + 0x38) + 0x44) = 1;
                    /* WARNING: Could not recover jumptable at 0x000140034d1b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::close(param_1);
  return;
}


/* ================= FUN_140034d30 @ 140034d30  size=663  callers=1 callees=15 ================= */
/* CALLERS: FUN_14003b6f0 */
/* CALLEES: FUN_140027140, FUN_140027700, QTextDocument, __security_check_cookie, clipboard, fromAscii_helper, setHtml, setText, text, toAscii, toPlainText, windowTitle, ~QByteArray, ~QString, ~QTextDocument */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140034d30(QWidget *param_1)

{
  QString *pQVar1;
  longlong *plVar2;
  QClipboard *this;
  undefined1 auStack_858 [32];
  Data *local_838;
  QString local_830 [8];
  QTextDocument local_828 [16];
  char local_818 [2048];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_858;
  QTextDocument::QTextDocument(local_828,(QObject *)0x0);
  FUN_140027140(local_818,"---------------------------\r\n",0x800);
  pQVar1 = (QString *)QWidget::windowTitle(param_1);
  plVar2 = (longlong *)QString::toAscii(pQVar1);
  FUN_140027700(local_818,*(undefined8 *)(*plVar2 + 0x10),0x800);
  QByteArray::~QByteArray((QByteArray *)&local_838);
  QString::~QString(local_830);
  FUN_140027700(local_818,"\r\n---------------------------\r\n",0x800);
  pQVar1 = (QString *)QLabel::text(*(QLabel **)(param_1 + 0x58));
  QTextDocument::setHtml(local_828,pQVar1);
  QString::~QString(local_830);
  pQVar1 = (QString *)QTextDocument::toPlainText(local_828);
  plVar2 = (longlong *)QString::toAscii(pQVar1);
  FUN_140027700(local_818,*(undefined8 *)(*plVar2 + 0x10),0x800);
  QByteArray::~QByteArray((QByteArray *)local_830);
  QString::~QString((QString *)&local_838);
  FUN_140027700(local_818,"\r\n---------------------------\r\n");
  if (*(QAbstractButton **)(param_1 + 0x48) != (QAbstractButton *)0x0) {
    pQVar1 = (QString *)QAbstractButton::text(*(QAbstractButton **)(param_1 + 0x48));
    plVar2 = (longlong *)QString::toAscii(pQVar1);
    FUN_140027700(local_818,*(undefined8 *)(*plVar2 + 0x10),0x800);
    QByteArray::~QByteArray((QByteArray *)local_830);
    QString::~QString((QString *)&local_838);
    FUN_140027700(local_818,&DAT_14006fac0);
  }
  if (*(QAbstractButton **)(param_1 + 0x50) != (QAbstractButton *)0x0) {
    pQVar1 = (QString *)QAbstractButton::text(*(QAbstractButton **)(param_1 + 0x50));
    plVar2 = (longlong *)QString::toAscii(pQVar1);
    FUN_140027700(local_818,*(undefined8 *)(*plVar2 + 0x10),0x800);
    QByteArray::~QByteArray((QByteArray *)local_830);
    QString::~QString((QString *)&local_838);
    FUN_140027700(local_818,&DAT_14006fac0);
  }
  if ((*(longlong *)(param_1 + 0x48) != 0) || (*(longlong *)(param_1 + 0x50) != 0)) {
    FUN_140027700(local_818,"\r\n---------------------------\r\n");
  }
  local_838 = QString::fromAscii_helper(local_818,-1);
  this = QApplication::clipboard();
  QClipboard::setText(this,(QString *)&local_838,0);
  QString::~QString((QString *)&local_838);
  QTextDocument::~QTextDocument(local_828);
  return;
}


/* ================= FUN_140034fd0 @ 140034fd0  size=272  callers=1 callees=8 ================= */
/* CALLERS: FUN_14003b6f0 */
/* CALLEES: FUN_14002c8f0, FUN_140036bd0, __security_check_cookie, close, fromAscii_helper, setMinimumWidth, setWindowTitle, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140034fd0(QWidget *param_1)

{
  int iVar1;
  undefined8 *puVar2;
  int iVar3;
  undefined1 auStack_158 [32];
  int local_138;
  Data *local_128 [2];
  char local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_158;
  puVar2 = *(undefined8 **)(param_1 + 0x38);
  iVar1 = *(int *)((longlong)puVar2 + 0x34);
  if (iVar1 < 0xb) {
    FUN_14002c8f0(local_118,0x100,"%s (closing now)",*puVar2);
    *(undefined4 *)(*(longlong *)(param_1 + 0x38) + 0x34) = 0;
  }
  else {
    *(int *)((longlong)puVar2 + 0x34) = iVar1 + -10;
    local_138 = *(int *)((longlong)*(undefined8 **)(param_1 + 0x38) + 0x34) / 1000 + 1;
    FUN_14002c8f0(local_118,0x100,"%s (closing in %d seconds)",**(undefined8 **)(param_1 + 0x38));
  }
  iVar3 = FUN_140036bd0(param_1,local_118);
  QWidget::setMinimumWidth(param_1,iVar3 + 0x96);
  local_128[0] = QString::fromAscii_helper(local_118,-1);
  QWidget::setWindowTitle(param_1,(QString *)local_128);
  QString::~QString((QString *)local_128);
  if (iVar1 < 0xb) {
    QWidget::close(param_1);
  }
  return;
}


/* ================= FUN_1400350e0 @ 1400350e0  size=115  callers=0 callees=3 ================= */
/* CALLEES: FUN_140043e60, isChecked, stop */

void FUN_1400350e0(longlong param_1,longlong param_2)

{
  bool bVar1;
  
  if ((*(byte *)(param_2 + 0x12) >> 1 & 1) != 0) {
    *(undefined4 *)(*(longlong *)(param_1 + 0x38) + 0x48) = 1;
  }
  if (*(QAbstractButton **)(param_1 + 0x28) != (QAbstractButton *)0x0) {
    bVar1 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0x28));
    *(uint *)(*(longlong *)(param_1 + 0x38) + 0x40) = (uint)bVar1;
  }
  if (*(QTimer **)(param_1 + 0x30) != (QTimer *)0x0) {
    QTimer::stop(*(QTimer **)(param_1 + 0x30));
  }
  if (*(longlong *)(param_1 + 0x40) != 0) {
    FUN_140043e60();
    *(undefined8 *)(param_1 + 0x40) = 0;
  }
  *(ushort *)(param_2 + 0x12) = *(ushort *)(param_2 + 0x12) | 4;
  return;
}


/* ================= FUN_140035160 @ 140035160  size=18  callers=0 callees=0 ================= */

void FUN_140035160(QWidget *param_1)

{
  *(undefined4 *)(*(longlong *)(param_1 + 0x38) + 0x48) = 1;
                    /* WARNING: Could not recover jumptable at 0x00014003516b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::close(param_1);
  return;
}


/* ================= FUN_140035180 @ 140035180  size=38  callers=4 callees=1 ================= */
/* CALLERS: FUN_14003a000, FUN_14003afd0, FUN_14003b070, FUN_14003b0c0 */
/* CALLEES: FUN_1400353f0 */

void FUN_140035180(longlong param_1,undefined8 param_2,undefined4 param_3)

{
  if (*(int *)(param_1 + 0x14) != 0) {
    FUN_1400353f0(param_1,*(int *)(param_1 + 0x10) + 1,param_2,param_3);
    return;
  }
  FUN_1400353f0(param_1,0,param_2,param_3);
  return;
}


/* ================= FUN_1400351b0 @ 1400351b0  size=292  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400353f0 */
/* CALLEES: FUN_14002c8f0, FUN_140043d10, FUN_140043e60, __security_check_cookie, memcpy, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_1400351b0(undefined8 *param_1,uint param_2)

{
  uint uVar1;
  undefined8 uVar2;
  void *_Dst;
  uint uVar3;
  undefined1 auStack_138 [32];
  undefined1 local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_138;
  if (param_2 < *(uint *)((longlong)param_1 + 0xc)) {
    uVar2 = 0;
  }
  else {
    uVar1 = *(uint *)(param_1 + 1);
    if (param_2 < uVar1) {
      uVar3 = *(uint *)((longlong)param_1 + 0xc) * 2;
      if (uVar3 <= param_2 + 1) {
        uVar3 = param_2 + 1;
      }
      if (uVar3 <= uVar1) {
        uVar1 = uVar3;
      }
      _Dst = (void *)FUN_140043d10(uVar1);
      if (_Dst == (void *)0x0) {
        if (param_1[4] != 0) {
          FUN_14002c8f0(local_118,0x100,"Cannot allocate %d bytes of memory for dynamic buffer",
                        uVar1);
          (*(code *)param_1[4])(local_118);
        }
        uVar2 = 0xfffffffe;
      }
      else {
        memcpy(_Dst,(void *)*param_1,(ulonglong)*(uint *)((longlong)param_1 + 0xc));
        FUN_140043e60(*param_1);
        if (-1 < (int)*(uint *)((longlong)param_1 + 0x1c)) {
          memset((void *)((ulonglong)*(uint *)((longlong)param_1 + 0xc) + (longlong)_Dst),
                 *(uint *)((longlong)param_1 + 0x1c) & 0xff,
                 (ulonglong)(uVar1 - *(uint *)((longlong)param_1 + 0xc)));
        }
        *(uint *)((longlong)param_1 + 0xc) = uVar1;
        uVar2 = 0;
        *param_1 = _Dst;
      }
    }
    else {
      if (param_1[4] != 0) {
        FUN_14002c8f0(local_118,0x100,
                      "Debugger accesses memory in dynamic buffer at offset 0x%08X which is out of array bounds."
                      ,param_2);
        (*(code *)param_1[4])(local_118);
      }
      uVar2 = 0xfffffffd;
    }
  }
  return uVar2;
}


/* ================= FUN_1400352e0 @ 1400352e0  size=199  callers=1 callees=5 ================= */
/* CALLERS: FUN_14003a000 */
/* CALLEES: FUN_14002c8f0, FUN_140035470, FUN_140043d20, __security_check_cookie, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8
FUN_1400352e0(longlong *param_1,longlong param_2,undefined4 param_3,uint param_4,uint param_5)

{
  longlong lVar1;
  undefined8 uVar2;
  undefined1 auStack_138 [32];
  undefined1 local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_138;
  param_1[4] = param_2;
  lVar1 = FUN_140043d20(param_4,"DYNBUFFER: dynamic buffer");
  *param_1 = lVar1;
  if (lVar1 == 0) {
    if (param_1[4] != 0) {
      FUN_14002c8f0(local_118,0x100,"Cannot allocate %d bytes of memory for dynamic buffer",
                    (ulonglong)param_4);
      (*(code *)param_1[4])(local_118);
    }
    uVar2 = 0xfffffffe;
  }
  else {
    *(uint *)((longlong)param_1 + 0x1c) = param_5;
    *(uint *)((longlong)param_1 + 0xc) = param_4;
    *(undefined4 *)(param_1 + 1) = param_3;
    FUN_140035470(param_1);
    if (-1 < (int)param_5) {
      memset((void *)*param_1,param_5 & 0xff,(ulonglong)param_4);
    }
    uVar2 = 0;
  }
  return uVar2;
}

