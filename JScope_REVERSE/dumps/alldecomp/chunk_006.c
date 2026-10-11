
/* ================= FUN_140047390 @ 140047390  size=781  callers=1 callees=16 ================= */
/* CALLERS: thunk_FUN_140047390 */
/* CALLEES: FUN_140027140, FUN_1400368e0, FUN_14003ecb0, FUN_14003edd0, FUN_14003f430, FUN_140043d10, FUN_140043e60, FUN_140047950, FUN_14004e9c0, FUN_14004ec40, FUN_14004f820, FUN_14004f9b0, __security_check_cookie, fclose, fopen_s, fputs */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

longlong FUN_140047390(void)

{
  double dVar1;
  longlong *plVar2;
  int iVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  longlong lVar7;
  uint uVar8;
  ulonglong uVar9;
  longlong lVar10;
  undefined1 auStack_188 [32];
  char *local_168;
  char *local_160;
  undefined8 local_158;
  FILE *local_148 [2];
  char local_138 [256];
  ulonglong local_38;
  
  local_38 = DAT_140096fe8 ^ (ulonglong)auStack_188;
  lVar10 = 0;
  lVar4 = FUN_140043d10(0x5bc);
  FUN_14004e9c0(lVar4);
  FUN_14004f820(local_138,0x100);
  local_158 = 0;
  local_160 = "CSV (*.csv)";
  local_168 = local_138;
  FUN_1400368e0(DAT_14286d388,local_138,0x100,"Export Data");
  if (local_138[0] == '\0') {
    return 0;
  }
  FUN_14004f9b0(local_138);
  FUN_140027140(lVar4 + 0x184,local_138,0x104);
  FUN_14004ec40(lVar4);
  fopen_s(local_148,local_138,"w");
  if (local_148[0] == (FILE *)0x0) {
    FUN_14003f430("File could not be opened!");
  }
  else if (DAT_14286d3b8 != (longlong *)0x0) {
    fputs("Timestamp",local_148[0]);
    for (plVar2 = DAT_14286d3b8; plVar2 != (longlong *)0x0; plVar2 = (longlong *)plVar2[1]) {
      fputs(";",local_148[0]);
      fputs(*(char **)(*plVar2 + 0x108),local_148[0]);
    }
    fputs("\n",local_148[0]);
    iVar3 = FUN_14003ecb0();
    if (iVar3 - 1U != 0) {
      uVar9 = (ulonglong)(iVar3 - 1U);
      lVar6 = lVar10;
      lVar7 = lVar10;
      do {
        plVar2 = DAT_14286d3b8;
        lVar5 = FUN_14003edd0();
        FUN_140047950(local_148[0],&DAT_14005f57c,*(undefined4 *)(lVar6 + lVar5));
        for (; plVar2 != (longlong *)0x0; plVar2 = (longlong *)plVar2[1]) {
          lVar5 = *plVar2;
          if (lVar5 == 0) goto switchD_14004758a_caseD_3;
          switch(*(undefined4 *)(lVar5 + 0x314)) {
          case 1:
            dVar1 = (double)*(float *)(*(longlong *)(lVar5 + 0x3d0) + lVar7);
            goto LAB_14004759b;
          case 2:
            dVar1 = *(double *)(*(longlong *)(lVar5 + 0x3d0) + lVar7);
LAB_14004759b:
            FUN_140047950(local_148[0],&DAT_140075a18,dVar1);
          default:
            goto switchD_14004758a_caseD_3;
          case 5:
          case 6:
            uVar8 = (uint)*(byte *)(*(longlong *)(lVar5 + 0x3d0) + lVar7);
            break;
          case 7:
            uVar8 = (uint)*(ushort *)(*(longlong *)(lVar5 + 0x3d0) + lVar7);
            break;
          case 8:
          case 0xc:
            uVar8 = *(uint *)(*(longlong *)(lVar5 + 0x3d0) + lVar7);
            break;
          case 10:
            uVar8 = (uint)*(char *)(*(longlong *)(lVar5 + 0x3d0) + lVar7);
            break;
          case 0xb:
            uVar8 = (uint)*(short *)(*(longlong *)(lVar5 + 0x3d0) + lVar7);
          }
          FUN_140047950(local_148[0],&DAT_140075a1c,uVar8);
switchD_14004758a_caseD_3:
        }
        fputs("\n",local_148[0]);
        lVar6 = lVar6 + 4;
        lVar7 = lVar7 + 8;
        uVar9 = uVar9 - 1;
      } while (uVar9 != 0);
    }
    fclose(local_148[0]);
    goto LAB_140047679;
  }
  lVar10 = 0xffffffff;
LAB_140047679:
  FUN_140043e60(lVar4);
  return lVar10;
}


/* ================= FUN_1400476d0 @ 1400476d0  size=633  callers=1 callees=20 ================= */
/* CALLERS: thunk_FUN_1400476d0 */
/* CALLEES: FUN_140027140, FUN_140027700, FUN_140028380, FUN_1400368e0, FUN_14003ece0, FUN_14003ed20, FUN_14003ed70, FUN_14003f430, FUN_14003f4f0, FUN_140043d10, FUN_140043e60, FUN_14004e9c0, FUN_14004ec40, FUN_14004f820, FUN_14004f9b0, __security_check_cookie, fclose, fopen_s, fwrite, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

uint FUN_1400476d0(void)

{
  longlong lVar1;
  longlong *plVar2;
  ulonglong uVar3;
  uint uVar4;
  longlong lVar5;
  void *_Str;
  uint uVar6;
  uint uVar7;
  undefined1 auStack_578 [32];
  undefined8 local_558;
  undefined8 local_550;
  undefined8 local_548;
  FILE *local_538;
  undefined4 local_530 [2];
  char local_528 [256];
  undefined1 local_428 [1024];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_578;
  uVar7 = 0;
  lVar5 = FUN_140043d10(0x5bc);
  FUN_14004e9c0(lVar5);
  FUN_14004f820(local_528,0x100);
  local_548 = 0;
  local_550 = "JRAW (*.jraw)";
  local_558 = local_528;
  FUN_1400368e0(DAT_14286d388,local_528,0x100,"Export Data");
  if (local_528[0] == '\0') {
    uVar7 = 0;
  }
  else {
    FUN_14004f9b0(local_528);
    FUN_140027140(lVar5 + 0x288,local_528,0x104);
    FUN_14004ec40(lVar5);
    fopen_s(&local_538,local_528,"wb");
    if (local_538 == (FILE *)0x0) {
      FUN_14003f430("File could not be opened!");
      uVar7 = 0xffffffff;
    }
    else {
      uVar4 = FUN_14003ed20();
      uVar6 = uVar7;
      if (uVar4 != 0) {
        do {
          plVar2 = DAT_14286d3b8;
          local_530[0] = FUN_14003ed70(uVar6);
          fwrite(local_530,4,1,local_538);
          for (; plVar2 != (longlong *)0x0; plVar2 = (longlong *)plVar2[1]) {
            lVar1 = *plVar2;
            _Str = (void *)FUN_14003ece0(lVar1,uVar6);
            fwrite(_Str,(ulonglong)*(uint *)(lVar1 + 0x31c),1,local_538);
          }
          uVar6 = uVar6 + 1;
        } while (uVar6 < uVar4);
      }
      fclose(local_538);
      memset(local_428,0,0x400);
      plVar2 = DAT_14286d3b8;
      FUN_140028380(local_428,0x400,"Raw data exported in the order:\nTimestamp");
      for (; plVar2 != (longlong *)0x0; plVar2 = (longlong *)plVar2[1]) {
        FUN_140027700(local_428,&DAT_14006e2bc,0x400);
        FUN_140027700(local_428,*plVar2 + 4,0x400);
      }
      uVar3 = (ulonglong)local_550 >> 0x20;
      local_550 = (char *)CONCAT44((int)uVar3,0xffffffff);
      uVar3 = (ulonglong)local_558 >> 0x20;
      local_558 = (char *)CONCAT44((int)uVar3,0x400);
      FUN_14003f4f0(local_428,&DAT_14006cc0c,0,0);
    }
    FUN_140043e60(lVar5);
  }
  return uVar7;
}


/* ================= FUN_140047950 @ 140047950  size=69  callers=1 callees=2 ================= */
/* CALLERS: FUN_140047390 */
/* CALLEES: FUN_14000e420, __stdio_common_vfprintf */

void FUN_140047950(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  undefined8 *puVar1;
  undefined8 local_res18;
  undefined8 local_res20;
  
  local_res18 = param_3;
  local_res20 = param_4;
  puVar1 = (undefined8 *)FUN_14000e420();
  __stdio_common_vfprintf(*puVar1,param_1,param_2,0,&local_res18);
  return;
}


/* ================= FUN_1400479a0 @ 1400479a0  size=1193  callers=1 callees=30 ================= */
/* CALLERS: FUN_14004c6a0 */
/* CALLEES: FUN_1400487b0, FUN_140048e60, FUN_140058d94, QBrush, QColor, QHBoxLayout, QPalette, QScrollBar, QSpacerItem, QVBoxLayout, QWidget, __security_check_cookie, addSpacerItem, addWidget, connect, resize, setAlignment, setAutoFillBackground, setBrush, setContentsMargins, setFixedHeight, setLayout, setMaximum, setMinimum, setPalette, setSpacing, setVisible, sizeHint, ~QBrush, ~QPalette */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

QWidget * FUN_1400479a0(QWidget *param_1)

{
  QScrollBar *pQVar1;
  QScrollBar *this;
  QWidget *pQVar2;
  QHBoxLayout *this_00;
  QVBoxLayout *this_01;
  QBrush *pQVar3;
  QSpacerItem *this_02;
  QSpacerItem *pQVar4;
  longlong lVar5;
  QSpacerItem *pQVar6;
  QWidget *pQVar7;
  QHBoxLayout *pQVar8;
  QVBoxLayout *this_03;
  undefined1 auStackY_98 [32];
  undefined8 local_68;
  QPalette local_60 [16];
  QWidget *local_50;
  QWidget *local_48 [2];
  ulonglong local_38;
  
  local_38 = DAT_140096fe8 ^ (ulonglong)auStackY_98;
  pQVar6 = (QSpacerItem *)0x0;
  local_68 = (QScrollBar *)((ulonglong)local_68 & 0xffffffff00000000);
  local_50 = param_1;
  QWidget::QWidget();
  *(undefined ***)param_1 = GraphContainer::vftable;
  *(undefined ***)(param_1 + 0x10) = GraphContainer::vftable;
  QPalette::QPalette(local_60);
  *(undefined8 *)(param_1 + 0x28) = 0;
  *(undefined8 *)(param_1 + 0x30) = 0;
  *(undefined8 *)(param_1 + 0x38) = 0;
  *(undefined8 *)(param_1 + 0x40) = 0;
  *(undefined8 *)(param_1 + 0x48) = 0;
  *(undefined8 *)(param_1 + 0x50) = 0;
  *(undefined8 *)(param_1 + 0x58) = 0;
  *(undefined8 *)(param_1 + 0x60) = 0;
  *(undefined8 *)(param_1 + 0x68) = 0;
  *(undefined8 *)(param_1 + 0x70) = 0;
  *(undefined8 *)(param_1 + 0x78) = 0;
  *(undefined8 *)(param_1 + 0x80) = 0;
  *(undefined8 *)(param_1 + 0x88) = 0;
  *(undefined8 *)(param_1 + 0x90) = 0;
  DAT_14286d3a8 = (QObject *)param_1;
  local_68 = (QScrollBar *)FUN_140058d94(0x180);
  pQVar1 = (QScrollBar *)pQVar6;
  if (local_68 != (QScrollBar *)0x0) {
    pQVar1 = (QScrollBar *)FUN_140048e60(local_68,param_1);
  }
  *(QScrollBar **)(param_1 + 0x38) = pQVar1;
  this = (QScrollBar *)FUN_140058d94(0x28);
  pQVar1 = (QScrollBar *)pQVar6;
  local_68 = this;
  if (this != (QScrollBar *)0x0) {
    QScrollBar::QScrollBar(this,1,(QWidget *)0x0);
    *(undefined ***)this = QScrollBar::vftable;
    *(undefined ***)(this + 0x10) = QScrollBar::vftable;
    pQVar1 = this;
  }
  *(QScrollBar **)(param_1 + 0x40) = pQVar1;
  pQVar2 = (QWidget *)FUN_140058d94(0x28);
  pQVar7 = (QWidget *)pQVar6;
  local_48[0] = pQVar2;
  if (pQVar2 != (QWidget *)0x0) {
    local_68 = (QScrollBar *)((ulonglong)local_68 & 0xffffffff00000000);
    QWidget::QWidget(pQVar2,param_1,&local_68);
    *(undefined ***)pQVar2 = QWidget::vftable;
    *(undefined ***)(pQVar2 + 0x10) = QWidget::vftable;
    pQVar7 = pQVar2;
  }
  *(QWidget **)(param_1 + 0x30) = pQVar7;
  this_00 = (QHBoxLayout *)FUN_140058d94(0x20);
  pQVar8 = (QHBoxLayout *)pQVar6;
  local_48[0] = (QWidget *)this_00;
  if (this_00 != (QHBoxLayout *)0x0) {
    QHBoxLayout::QHBoxLayout(this_00,param_1);
    *(undefined ***)this_00 = QHBoxLayout::vftable;
    *(undefined ***)(this_00 + 0x10) = QHBoxLayout::vftable;
    pQVar8 = this_00;
  }
  *(QHBoxLayout **)(param_1 + 0x48) = pQVar8;
  this_01 = (QVBoxLayout *)FUN_140058d94(0x20);
  this_03 = (QVBoxLayout *)pQVar6;
  local_48[0] = (QWidget *)this_01;
  if (this_01 != (QVBoxLayout *)0x0) {
    QVBoxLayout::QVBoxLayout(this_01);
    *(undefined ***)this_01 = QVBoxLayout::vftable;
    *(undefined ***)(this_01 + 0x10) = QVBoxLayout::vftable;
    this_03 = this_01;
  }
  FUN_1400487b0(param_1);
  QWidget::setAutoFillBackground(*(QWidget **)(param_1 + 0x30),true);
  QColor::QColor((QColor *)local_48,5);
  pQVar3 = (QBrush *)QBrush::QBrush((QBrush *)&local_68,(QColor *)local_48,1);
  QPalette::setBrush(local_60,5,10,pQVar3);
  QBrush::~QBrush((QBrush *)&local_68);
  QWidget::setPalette(*(QWidget **)(param_1 + 0x30),local_60);
  QWidget::setLayout(*(QWidget **)(param_1 + 0x30),*(QLayout **)(param_1 + 0x48));
  QLayout::setContentsMargins(*(QLayout **)(param_1 + 0x48),0x28,2,0,2);
  local_68._0_4_ = 1;
  QLayoutItem::setAlignment((QLayoutItem *)(*(longlong *)(param_1 + 0x48) + 0x10),&local_68);
  local_68._0_4_ = 0;
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x48),*(undefined8 *)(param_1 + 0x68),0,&local_68);
  local_68._0_4_ = 0;
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x48),*(undefined8 *)(param_1 + 0x70),0,&local_68);
  local_68 = (QScrollBar *)((ulonglong)local_68._4_4_ << 0x20);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x48),*(undefined8 *)(param_1 + 0x78),0,&local_68);
  this_02 = (QSpacerItem *)FUN_140058d94(0x30);
  pQVar4 = pQVar6;
  local_48[0] = (QWidget *)this_02;
  if (this_02 != (QSpacerItem *)0x0) {
    QSpacerItem::QSpacerItem(this_02,0x19,0x19,1,1);
    *(undefined ***)this_02 = QSpacerItem::vftable;
    pQVar4 = this_02;
  }
  QBoxLayout::addSpacerItem(*(QBoxLayout **)(param_1 + 0x48),pQVar4);
  local_68 = (QScrollBar *)((ulonglong)local_68 & 0xffffffff00000000);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x48),*(undefined8 *)(param_1 + 0x60),0,&local_68);
  local_68 = (QScrollBar *)((ulonglong)local_68 & 0xffffffff00000000);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x48),*(undefined8 *)(param_1 + 0x50),0,&local_68);
  pQVar4 = (QSpacerItem *)FUN_140058d94(0x30);
  local_48[0] = (QWidget *)pQVar4;
  if (pQVar4 != (QSpacerItem *)0x0) {
    QSpacerItem::QSpacerItem(pQVar4,0x19,0x19,1,1);
    *(undefined ***)pQVar4 = QSpacerItem::vftable;
    pQVar6 = pQVar4;
  }
  QBoxLayout::addSpacerItem(*(QBoxLayout **)(param_1 + 0x48),pQVar6);
  local_68 = (QScrollBar *)((ulonglong)local_68 & 0xffffffff00000000);
  QBoxLayout::addWidget(*(QBoxLayout **)(param_1 + 0x48));
  QLayout::setContentsMargins((QLayout *)this_03,0,0,0,0);
  QBoxLayout::setSpacing((QBoxLayout *)this_03,0);
  local_68 = (QScrollBar *)((ulonglong)local_68 & 0xffffffff00000000);
  QBoxLayout::addWidget((QBoxLayout *)this_03,*(undefined8 *)(param_1 + 0x30),0,&local_68);
  local_68 = (QScrollBar *)((ulonglong)local_68 & 0xffffffff00000000);
  QBoxLayout::addWidget((QBoxLayout *)this_03,*(undefined8 *)(param_1 + 0x38),0,&local_68);
  local_68 = (QScrollBar *)((ulonglong)local_68 & 0xffffffff00000000);
  QBoxLayout::addWidget((QBoxLayout *)this_03,*(undefined8 *)(param_1 + 0x40),0,&local_68);
  QAbstractSlider::setMinimum(*(QAbstractSlider **)(param_1 + 0x40),0);
  QAbstractSlider::setMaximum(*(QAbstractSlider **)(param_1 + 0x40),100);
  (**(code **)(**(longlong **)(param_1 + 0x40) + 0x58))(*(longlong **)(param_1 + 0x40),0);
  QObject::connect(*(QObject **)(param_1 + 0x40),"2valueChanged(int)",(QObject *)param_1,
                   "1_OnScrollValueChanged(int)",0);
  QWidget::setLayout(param_1,(QLayout *)this_03);
  local_68 = (QScrollBar *)0x32000000320;
  QWidget::resize(param_1,(QSize *)&local_68);
  lVar5 = (**(code **)(**(longlong **)(param_1 + 0x30) + 0x60))
                    (*(longlong **)(param_1 + 0x30),local_48);
  QWidget::setFixedHeight(*(QWidget **)(param_1 + 0x30),*(int *)(lVar5 + 4));
  QPalette::~QPalette(local_60);
  return param_1;
}


/* ================= FUN_140047e50 @ 140047e50  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_140047e90 */

void FUN_140047e50(QWidget *param_1)

{
  *(undefined ***)param_1 = GraphContainer::vftable;
  *(undefined ***)(param_1 + 0x10) = GraphContainer::vftable;
                    /* WARNING: Could not recover jumptable at 0x000140047e65. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::~QWidget(param_1);
  return;
}


/* ================= FUN_140047e6c @ 140047e6c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140047e90 */

void FUN_140047e6c(longlong param_1)

{
  FUN_140047e90(param_1 + -0x10);
  return;
}


/* ================= FUN_140047e78 @ 140047e78  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140047ed0 */

void FUN_140047e78(longlong param_1)

{
  FUN_140047ed0(param_1 + -0x10);
  return;
}


/* ================= FUN_140047e84 @ 140047e84  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140047f10 */

void FUN_140047e84(longlong param_1)

{
  FUN_140047f10(param_1 + -0x10);
  return;
}


/* ================= FUN_140047e90 @ 140047e90  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_140047e6c */
/* CALLEES: FUN_140047e50, free */

void * FUN_140047e90(void *param_1,ulonglong param_2)

{
  FUN_140047e50();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140047ed0 @ 140047ed0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140047e78 */
/* CALLEES: free, ~QColorDialog */

QColorDialog * FUN_140047ed0(QColorDialog *param_1,uint param_2)

{
  QColorDialog::~QColorDialog(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140047f10 @ 140047f10  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140047e84 */
/* CALLEES: free, ~QScrollBar */

QScrollBar * FUN_140047f10(QScrollBar *param_1,uint param_2)

{
  QScrollBar::~QScrollBar(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140047f50 @ 140047f50  size=136  callers=1 callees=7 ================= */
/* CALLERS: FUN_1400482d0 */
/* CALLEES: FUN_14003ecb0, FUN_14003edd0, FUN_140049900, FUN_140049940, setMaximum, setMinimum, setSingleStep */

void FUN_140047f50(longlong param_1)

{
  int iVar1;
  int iVar2;
  longlong lVar3;
  
  FUN_140049940(*(undefined8 *)(param_1 + 0x38));
  iVar2 = 0;
  if ((DAT_14286d3b8 != (longlong *)0x0) && (*DAT_14286d3b8 != 0)) {
    iVar1 = FUN_14003ecb0();
    if (iVar1 != 0) {
      lVar3 = FUN_14003edd0();
      iVar2 = FUN_14003ecb0();
      iVar2 = *(int *)(lVar3 + (ulonglong)(iVar2 - 1) * 4);
    }
  }
  QAbstractSlider::setMinimum(*(QAbstractSlider **)(param_1 + 0x40),0);
  QAbstractSlider::setMaximum(*(QAbstractSlider **)(param_1 + 0x40),iVar2);
  iVar2 = FUN_140049900(*(undefined8 *)(param_1 + 0x38));
  QAbstractSlider::setSingleStep(*(QAbstractSlider **)(param_1 + 0x40),iVar2);
                    /* WARNING: Could not recover jumptable at 0x000140047fd4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(**(longlong **)(param_1 + 0x40) + 0x58))(*(longlong **)(param_1 + 0x40),1);
  return;
}


/* ================= FUN_140047fe0 @ 140047fe0  size=80  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400482d0 */
/* CALLEES: FUN_140048320, FUN_140049950 */

void FUN_140047fe0(longlong param_1)

{
  (**(code **)(**(longlong **)(param_1 + 0x40) + 0x58))(*(longlong **)(param_1 + 0x40),0);
  FUN_140049950(*(undefined8 *)(param_1 + 0x38));
  if (((DAT_14286d3b8 != (longlong *)0x0) && (*DAT_14286d3b8 != 0)) &&
     (*(char *)(*DAT_14286d3b8 + 0x388) == '\0')) {
    FUN_140048320(param_1,0);
    return;
  }
  return;
}


/* ================= FUN_140048030 @ 140048030  size=200  callers=3 callees=3 ================= */
/* CALLERS: FUN_14004a410, FUN_14004a4b0, FUN_14004bae0 */
/* CALLEES: FUN_140049910, hide, show */

void FUN_140048030(longlong param_1)

{
  char cVar1;
  char cVar2;
  
  cVar1 = FUN_140049910(*(undefined8 *)(param_1 + 0x38),1);
  cVar2 = FUN_140049910(*(undefined8 *)(param_1 + 0x38),2);
  if (cVar1 == '\0') {
    QWidget::hide(*(QWidget **)(param_1 + 0x70));
  }
  else {
    QWidget::show(*(QWidget **)(param_1 + 0x70));
  }
  if (cVar2 == '\0') {
    QWidget::hide(*(QWidget **)(param_1 + 0x78));
  }
  else {
    QWidget::show(*(QWidget **)(param_1 + 0x78));
  }
  if ((cVar1 != '\0') && (cVar2 != '\0')) {
    QWidget::show(*(QWidget **)(param_1 + 0x60));
    QWidget::show(*(QWidget **)(param_1 + 0x50));
                    /* WARNING: Could not recover jumptable at 0x0001400480c3. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    QWidget::show(*(QWidget **)(param_1 + 0x58));
    return;
  }
  QWidget::hide(*(QWidget **)(param_1 + 0x60));
  QWidget::hide(*(QWidget **)(param_1 + 0x50));
                    /* WARNING: Could not recover jumptable at 0x0001400480f1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::hide(*(QWidget **)(param_1 + 0x58));
  return;
}


/* ================= FUN_140048100 @ 140048100  size=456  callers=3 callees=8 ================= */
/* CALLERS: FUN_1400482d0, FUN_14004bdc0, FUN_14004c470 */
/* CALLEES: FUN_140048540, FUN_140048680, FUN_1400498d0, FUN_140049900, __security_check_cookie, fromAscii_helper, setText, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140048100(longlong param_1)

{
  undefined8 uVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  longlong lVar4;
  longlong lVar5;
  undefined1 auStack_158 [32];
  Data *local_138 [2];
  char local_128 [256];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_158;
  uVar2 = FUN_140049900(*(undefined8 *)(param_1 + 0x38));
  uVar1 = *(undefined8 *)(param_1 + 0x38);
  uVar3 = FUN_1400498d0(uVar1,0);
  FUN_140048680(local_128,uVar3,uVar2);
  local_138[0] = QString::fromAscii_helper(local_128,-1);
  QAbstractButton::setText(*(QAbstractButton **)(param_1 + 0x68),(QString *)local_138);
  QString::~QString((QString *)local_138);
  lVar4 = FUN_1400498d0(uVar1,1);
  FUN_140048680(local_128,lVar4,uVar2);
  local_138[0] = QString::fromAscii_helper(local_128,-1);
  QAbstractButton::setText(*(QAbstractButton **)(param_1 + 0x70),(QString *)local_138);
  QString::~QString((QString *)local_138);
  lVar5 = FUN_1400498d0(uVar1,2);
  FUN_140048680(local_128,lVar5,uVar2);
  local_138[0] = QString::fromAscii_helper(local_128,-1);
  QAbstractButton::setText(*(QAbstractButton **)(param_1 + 0x78),(QString *)local_138);
  QString::~QString((QString *)local_138);
  FUN_140048540(local_128,lVar4 - lVar5);
  local_138[0] = QString::fromAscii_helper(local_128,-1);
  QLabel::setText(*(QLabel **)(param_1 + 0x58),(QString *)local_138);
  QString::~QString((QString *)local_138);
  FUN_140048680(local_128,lVar4 - lVar5,uVar2);
  local_138[0] = QString::fromAscii_helper(local_128,-1);
  QLabel::setText(*(QLabel **)(param_1 + 0x50),(QString *)local_138);
  QString::~QString((QString *)local_138);
  return;
}


/* ================= FUN_1400482d0 @ 1400482d0  size=76  callers=3 callees=7 ================= */
/* CALLERS: FUN_14004d830, FUN_14004d870, FUN_14004d900 */
/* CALLEES: FUN_14003ef10, FUN_140047f50, FUN_140047fe0, FUN_140048100, FUN_140048320, FUN_1400497f0, FUN_140049c20 */

void FUN_1400482d0(longlong param_1)

{
  char cVar1;
  double dVar2;
  
  cVar1 = FUN_14003ef10();
  if (cVar1 == '\0') {
    FUN_140047f50(param_1);
  }
  else {
    FUN_140047fe0();
  }
  FUN_140048100(param_1);
  FUN_140049c20(*(undefined8 *)(param_1 + 0x38));
  dVar2 = (double)FUN_1400497f0(*(undefined8 *)(param_1 + 0x38));
  FUN_140048320(param_1,(int)dVar2);
  return;
}


/* ================= FUN_140048320 @ 140048320  size=46  callers=4 callees=2 ================= */
/* CALLERS: FUN_140047fe0, FUN_1400482d0, FUN_140049a10, FUN_14004bdc0 */
/* CALLEES: FUN_140049900, setValue */

void FUN_140048320(longlong param_1,int param_2)

{
  int iVar1;
  
  QAbstractSlider::setValue(*(QAbstractSlider **)(param_1 + 0x40),param_2);
  iVar1 = FUN_140049900(*(undefined8 *)(param_1 + 0x38));
                    /* WARNING: Could not recover jumptable at 0x000140048347. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QAbstractSlider::setSingleStep(*(QAbstractSlider **)(param_1 + 0x40),iVar1);
  return;
}


/* ================= FUN_140048350 @ 140048350  size=311  callers=1 callees=18 ================= */
/* CALLERS: FUN_1400487b0 */
/* CALLEES: FUN_140048a70, FUN_140058d94, QBrush, QPalette, QPushButton, fromAscii_helper, hide, operator=, palette, setAutoFillBackground, setBrush, setFlat, setMaximumHeight, setPalette, setText, ~QBrush, ~QPalette, ~QString */

QWidget * FUN_140048350(QColor *param_1,char *param_2,int param_3)

{
  QPushButton *pQVar1;
  QPalette *pQVar2;
  QBrush *pQVar3;
  QPushButton *local_38;
  QPalette local_30 [24];
  
  QPalette::QPalette(local_30);
  pQVar1 = (QPushButton *)FUN_140058d94(0x28);
  local_38 = pQVar1;
  if (pQVar1 == (QPushButton *)0x0) {
    pQVar1 = (QPushButton *)0x0;
  }
  else {
    QPushButton::QPushButton(pQVar1,(QWidget *)0x0);
    *(undefined ***)pQVar1 = QPushButton::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QPushButton::vftable;
  }
  QWidget::setAutoFillBackground((QWidget *)pQVar1,true);
  QPushButton::setFlat(pQVar1,true);
  local_38 = (QPushButton *)QString::fromAscii_helper(param_2,-1);
  QAbstractButton::setText((QAbstractButton *)pQVar1,(QString *)&local_38);
  QString::~QString((QString *)&local_38);
  QWidget::setMaximumHeight((QWidget *)pQVar1,param_3);
  FUN_140048a70(pQVar1,param_1);
  pQVar2 = QWidget::palette((QWidget *)pQVar1);
  QPalette::operator=(local_30,pQVar2);
  pQVar3 = (QBrush *)QBrush::QBrush((QBrush *)&local_38,param_1,1);
  QPalette::setBrush(local_30,5,1,pQVar3);
  QBrush::~QBrush((QBrush *)&local_38);
  QWidget::setPalette((QWidget *)pQVar1,local_30);
  QWidget::hide((QWidget *)pQVar1);
  QPalette::~QPalette(local_30);
  return (QWidget *)pQVar1;
}


/* ================= FUN_140048490 @ 140048490  size=169  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400487b0 */
/* CALLEES: FUN_140058d94, QLabel, fromAscii_helper, hide, setMaximumHeight, ~QString */

QWidget * FUN_140048490(void)

{
  QWidget *this;
  QLabel *pQVar1;
  undefined4 local_res10 [2];
  Data *local_res18;
  QLabel *local_res20;
  
  pQVar1 = (QLabel *)FUN_140058d94(0x28);
  local_res20 = pQVar1;
  if (pQVar1 == (QLabel *)0x0) {
    this = (QWidget *)0x0;
  }
  else {
    local_res18 = QString::fromAscii_helper("",-1);
    local_res10[0] = 0;
    QLabel::QLabel(pQVar1,&local_res18,0,local_res10);
    *(undefined ***)pQVar1 = QLabel::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QLabel::vftable;
    QString::~QString((QString *)&local_res18);
    this = (QWidget *)pQVar1;
  }
  QWidget::setMaximumHeight(this,0x14);
  QWidget::hide(this);
  return this;
}


/* ================= FUN_140048540 @ 140048540  size=314  callers=1 callees=4 ================= */
/* CALLERS: FUN_140048100 */
/* CALLEES: FUN_140027700, __security_check_cookie, memset, snprintf */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140048540(void *param_1,longlong param_2)

{
  double dVar1;
  longlong lVar2;
  char *pcVar3;
  undefined1 auStack_158 [32];
  undefined1 local_138 [256];
  ulonglong local_38;
  
  local_38 = DAT_140096fe8 ^ (ulonglong)auStack_158;
  memset(param_1,0,0x100);
  lVar2 = -param_2;
  if (-1 < param_2) {
    lVar2 = param_2;
  }
  if (lVar2 == 0) {
    dVar1 = 0.0;
  }
  else {
    dVar1 = 1.0 / ((double)lVar2 / 1000000.0);
  }
  pcVar3 = "f  %.4f Hz";
  if (0.09 < dVar1) {
    pcVar3 = "f  %.2f Hz";
  }
  snprintf(local_138,0x100,pcVar3,SUB84(dVar1,0));
  if (1000.0 <= dVar1) {
    dVar1 = dVar1 / 1000.0;
    snprintf(local_138,0x100,"f  %.2f KHz",SUB84(dVar1,0));
    if (1000.0 <= dVar1) {
      snprintf(local_138,0x100,"f  %.2f MHz",SUB84(dVar1 / 1000.0,0));
    }
  }
  FUN_140027700(param_1,local_138,0x100);
  return;
}


/* ================= FUN_140048680 @ 140048680  size=295  callers=1 callees=4 ================= */
/* CALLERS: FUN_140048100 */
/* CALLEES: FUN_140027700, FUN_14002c8f0, __security_check_cookie, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140048680(void *param_1,longlong param_2,int param_3)

{
  int iVar1;
  longlong lVar2;
  undefined1 auStack_148 [32];
  int local_128;
  int local_120;
  undefined1 local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_148;
  memset(param_1,0,0x100);
  lVar2 = -param_2;
  if (-1 < param_2) {
    lVar2 = param_2;
  }
  iVar1 = (int)(lVar2 / 1000);
  local_128 = iVar1 + (int)((lVar2 / 1000) / 1000) * -1000;
  if (param_3 < 1000000) {
    local_120 = (int)lVar2 + iVar1 * -1000;
    FUN_14002c8f0(local_118,0x100,"%0d%03d.%03d ms",lVar2 / 1000000);
  }
  else {
    FUN_14002c8f0(local_118,0x100,"%0d.%03d s",lVar2 / 1000000);
  }
  FUN_140027700(param_1,local_118,0x100);
  return;
}


/* ================= FUN_1400487b0 @ 1400487b0  size=666  callers=1 callees=20 ================= */
/* CALLERS: FUN_1400479a0 */
/* CALLEES: FUN_14002a290, FUN_140048350, FUN_140048490, FUN_140058d94, QFontMetrics, QLabel, QPixmap, connect, contentsMargins, fromAscii_helper, height, hide, operator=, scaledToHeight, setPixmap, setRgb, show, ~QFontMetrics, ~QPixmap, ~QString */

void FUN_1400487b0(QObject *param_1)

{
  ulonglong uVar1;
  ulonglong uVar2;
  int iVar3;
  undefined8 uVar4;
  QLabel *pQVar5;
  undefined8 *puVar6;
  QFontMetrics *this;
  QPixmap *pQVar7;
  QWidget *this_00;
  undefined4 local_res8 [2];
  undefined4 local_res10 [2];
  Data *local_res18;
  QLabel *local_res20;
  undefined8 local_78;
  undefined8 uStack_70;
  QPixmap local_60 [24];
  QPixmap local_48 [32];
  
  this_00 = (QWidget *)0x0;
  local_res8[0] = 0;
  QPixmap::QPixmap(local_60);
  uVar4 = FUN_140048490();
  *(undefined8 *)(param_1 + 0x50) = uVar4;
  uVar4 = FUN_140048490();
  *(undefined8 *)(param_1 + 0x58) = uVar4;
  pQVar5 = (QLabel *)FUN_140058d94(0x28);
  local_res20 = pQVar5;
  if (pQVar5 != (QLabel *)0x0) {
    local_res18 = QString::fromAscii_helper("",-1);
    local_res8[0] = 1;
    local_res10[0] = 0;
    QLabel::QLabel(pQVar5,&local_res18,0,local_res10);
    *(undefined ***)pQVar5 = QLabel::vftable;
    *(undefined ***)(pQVar5 + 0x10) = QLabel::vftable;
    this_00 = (QWidget *)pQVar5;
  }
  *(QWidget **)(param_1 + 0x60) = this_00;
  if (pQVar5 != (QLabel *)0x0) {
    QString::~QString((QString *)&local_res18);
    this_00 = *(QWidget **)(param_1 + 0x60);
  }
  puVar6 = (undefined8 *)QWidget::contentsMargins(this_00);
  local_78 = *puVar6;
  uStack_70 = puVar6[1];
  this = (QFontMetrics *)
         QFontMetrics::QFontMetrics
                   ((QFontMetrics *)local_res8,
                    (QFont *)(*(longlong *)(*(longlong *)(param_1 + 0x60) + 0x20) + 0x38));
  iVar3 = QFontMetrics::height(this);
  QFontMetrics::~QFontMetrics((QFontMetrics *)local_res8);
  uVar1 = (ulonglong)uStack_70 >> 0x20;
  uVar2 = (ulonglong)local_78 >> 0x20;
  pQVar7 = (QPixmap *)FUN_14002a290(local_48,0x20);
  pQVar7 = (QPixmap *)
           QPixmap::scaledToHeight(pQVar7,(int)&local_78,(iVar3 - (int)uVar2) - (int)uVar1);
  QPixmap::operator=(local_60,pQVar7);
  QPixmap::~QPixmap((QPixmap *)&local_78);
  QPixmap::~QPixmap(local_48);
  QLabel::setPixmap(*(QLabel **)(param_1 + 0x60),local_60);
  QWidget::hide(*(QWidget **)(param_1 + 0x60));
  QColor::setRgb((QColor *)&local_78,0xff,0xa5,0,0xff);
  uVar4 = FUN_140048350(&local_78,&DAT_14005f306,iVar3);
  *(undefined8 *)(param_1 + 0x68) = uVar4;
  QColor::setRgb((QColor *)&local_78,0xa5,0xff,0xa5,0xff);
  uVar4 = FUN_140048350(&local_78,&DAT_14005f306,iVar3);
  *(undefined8 *)(param_1 + 0x70) = uVar4;
  QColor::setRgb((QColor *)&local_78,0xa5,0xa5,0xff,0xff);
  uVar4 = FUN_140048350(&local_78,&DAT_14005f306,iVar3);
  *(undefined8 *)(param_1 + 0x78) = uVar4;
  QObject::connect(*(QObject **)(param_1 + 0x68),"2clicked()",param_1,"1_SlotOnColorBtnClicked()",0)
  ;
  QObject::connect(*(QObject **)(param_1 + 0x70),"2clicked()",param_1,"1_SlotOnColorBtnClicked()",0)
  ;
  QObject::connect(*(QObject **)(param_1 + 0x78),"2clicked()",param_1,"1_SlotOnColorBtnClicked()",0)
  ;
  QWidget::show(*(QWidget **)(param_1 + 0x68));
  QPixmap::~QPixmap(local_60);
  return;
}


/* ================= FUN_140048a50 @ 140048a50  size=17  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b7f0 */
/* CALLEES: FUN_1400499e0 */

void FUN_140048a50(longlong param_1,int param_2)

{
  FUN_1400499e0(*(undefined8 *)(param_1 + 0x38),(double)param_2);
  return;
}


/* ================= FUN_140048a70 @ 140048a70  size=314  callers=2 callees=8 ================= */
/* CALLERS: FUN_140048350, FUN_140048bb0 */
/* CALLEES: QChar, __security_check_cookie, arg, dark, fromAscii_helper, name, setStyleSheet, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140048a70(QWidget *param_1,QColor *param_2)

{
  int iVar1;
  int iVar2;
  QString *pQVar3;
  undefined1 auStack_88 [32];
  QChar *local_68;
  QChar local_58 [2];
  QChar local_56 [6];
  Data *local_50;
  QString local_48 [8];
  QString local_40 [8];
  QString local_38 [8];
  QString local_30 [8];
  QColor local_28 [16];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_88;
  QChar::QChar(local_56,0x20);
  QChar::QChar(local_58,0x20);
  QColor::dark(param_2,(int)local_28);
  iVar1 = QColor::name(local_28);
  iVar2 = QColor::name(param_2);
  local_50 = QString::fromAscii_helper
                       ("QPushButton { background-color: %1;padding-left:     5px; padding-right:    5px; padding-top:      2px; padding-bottom:   2px; border:           2px;} QPushButton:pressed { background-color: %2;}"
                        ,-1);
  local_68 = local_58;
  pQVar3 = (QString *)QString::arg((QString *)&local_50,local_40,iVar2,(QChar *)0x0);
  local_68 = local_56;
  pQVar3 = (QString *)QString::arg(pQVar3,local_48,iVar1,(QChar *)0x0);
  QWidget::setStyleSheet(param_1,pQVar3);
  QString::~QString(local_48);
  QString::~QString(local_40);
  QString::~QString((QString *)&local_50);
  QString::~QString(local_38);
  QString::~QString(local_30);
  return;
}


/* ================= FUN_140048bb0 @ 140048bb0  size=341  callers=1 callees=16 ================= */
/* CALLERS: FUN_140048d10 */
/* CALLEES: FUN_140048a70, FUN_140058d94, QBrush, QColorDialog, QPalette, __security_check_cookie, color, exec, invalidate, operator=, palette, selectedColor, setBrush, setPalette, ~QBrush, ~QPalette */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140048bb0(QWidget *param_1,QWidget *param_2)

{
  int iVar1;
  QColorDialog *pQVar2;
  QPalette *pQVar3;
  QColor *pQVar4;
  QBrush *pQVar5;
  undefined1 auStack_68 [32];
  QPalette local_48 [16];
  QColorDialog *local_38 [2];
  QColor local_28 [16];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_68;
  QPalette::QPalette(local_48);
  QColor::invalidate(local_28);
  pQVar2 = (QColorDialog *)FUN_140058d94(0x28);
  local_38[0] = pQVar2;
  if (pQVar2 == (QColorDialog *)0x0) {
    pQVar2 = (QColorDialog *)0x0;
  }
  else {
    pQVar3 = QWidget::palette(param_2);
    pQVar4 = QPalette::color(pQVar3,1);
    QColorDialog::QColorDialog(pQVar2,pQVar4,param_1);
    *(undefined ***)pQVar2 = QColorDialog::vftable;
    *(undefined ***)(pQVar2 + 0x10) = QColorDialog::vftable;
  }
  iVar1 = QDialog::exec((QDialog *)pQVar2);
  if (iVar1 != 0) {
    pQVar4 = (QColor *)QColorDialog::selectedColor(pQVar2);
    QColor::operator=(local_28,pQVar4);
    pQVar3 = QWidget::palette(param_2);
    QPalette::operator=(local_48,pQVar3);
    pQVar5 = (QBrush *)QBrush::QBrush((QBrush *)local_38,local_28,1);
    QPalette::setBrush(local_48,5,1,pQVar5);
    QBrush::~QBrush((QBrush *)local_38);
    QWidget::setPalette(param_2,local_48);
    FUN_140048a70(param_2,local_28);
  }
  QPalette::~QPalette(local_48);
  return;
}


/* ================= FUN_140048d10 @ 140048d10  size=323  callers=1 callees=9 ================= */
/* CALLERS: FUN_14003b7f0 */
/* CALLEES: FUN_14003fa20, FUN_140048bb0, FUN_140049990, __security_check_cookie, color, invalidate, operator=, palette, sender */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140048d10(QObject *param_1)

{
  QObject *pQVar1;
  QPalette *pQVar2;
  QColor *pQVar3;
  undefined1 auStack_98 [32];
  undefined4 local_78;
  undefined8 local_74;
  undefined2 local_6c;
  undefined4 local_68;
  undefined8 local_64;
  undefined2 local_5c;
  undefined4 local_58;
  undefined8 local_54;
  undefined2 local_4c;
  undefined4 local_48;
  undefined8 local_44;
  undefined2 local_3c;
  undefined4 local_38;
  undefined8 local_34;
  undefined2 local_2c;
  undefined4 local_28;
  undefined8 local_24;
  undefined2 local_1c;
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_98;
  QColor::invalidate((QColor *)&local_28);
  QColor::invalidate((QColor *)&local_38);
  QColor::invalidate((QColor *)&local_48);
  pQVar1 = QObject::sender(param_1);
  FUN_140048bb0(param_1,pQVar1);
  pQVar2 = QWidget::palette(*(QWidget **)(param_1 + 0x68));
  pQVar3 = QPalette::color(pQVar2,1);
  QColor::operator=((QColor *)&local_28,pQVar3);
  pQVar2 = QWidget::palette(*(QWidget **)(param_1 + 0x70));
  pQVar3 = QPalette::color(pQVar2,1);
  QColor::operator=((QColor *)&local_38,pQVar3);
  pQVar2 = QWidget::palette(*(QWidget **)(param_1 + 0x78));
  pQVar3 = QPalette::color(pQVar2,1);
  QColor::operator=((QColor *)&local_48,pQVar3);
  local_78 = local_48;
  local_6c = local_3c;
  local_68 = local_38;
  local_74 = local_44;
  local_5c = local_2c;
  local_58 = local_28;
  local_64 = local_34;
  local_4c = local_1c;
  local_54 = local_24;
  FUN_140049990(*(undefined8 *)(param_1 + 0x38),&local_58,&local_68,&local_78);
  FUN_14003fa20();
  return;
}


/* ================= FUN_140048e60 @ 140048e60  size=2076  callers=1 callees=23 ================= */
/* CALLERS: FUN_1400479a0 */
/* CALLEES: FUN_140043990, FUN_140055470, FUN_140058d94, QAction, QKeySequence, QMenu, QShortcut, QWidget, __security_check_cookie, addAction, connect, hide, invalidate, memset, operator=, setAttribute, setFocusPolicy, setKey, setMinimumSize, setRgb, setShortcut, show, ~QKeySequence */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

QWidget * FUN_140048e60(QWidget *param_1,undefined8 param_2)

{
  undefined8 uVar1;
  QMenu *this;
  QShortcut *pQVar2;
  QKeySequence *pQVar3;
  QAction *pQVar4;
  QAction *pQVar5;
  QMenu *pQVar6;
  QShortcut *pQVar7;
  QAction *pQVar8;
  undefined1 auStackY_78 [32];
  undefined4 local_48 [2];
  QWidget *local_40;
  QMenu *local_38 [2];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStackY_78;
  pQVar8 = (QAction *)0x0;
  local_48[0] = 0;
  local_40 = param_1;
  QWidget::QWidget(param_1,param_2,local_48);
  *(undefined ***)param_1 = GraphWidget::vftable;
  *(undefined ***)(param_1 + 0x10) = GraphWidget::vftable;
  QColor::invalidate((QColor *)(param_1 + 0x10c));
  QColor::invalidate((QColor *)(param_1 + 0x134));
  QColor::invalidate((QColor *)(param_1 + 0x15c));
  memset(param_1 + 0x28,0,0x158);
  DAT_14286d3a0 = (QObject *)param_1;
  QWidget::hide(param_1);
  QWidget::setMinimumSize(param_1,800,400);
  *(undefined2 *)(param_1 + 0x29) = 0;
  param_1[0x2b] = (QWidget)0x1;
  *(undefined8 *)(param_1 + 0x80) = 0x412e848000000000;
  *(undefined4 *)(param_1 + 0x88) = 100;
  *(undefined8 *)(param_1 + 0x40) = 0;
  uVar1 = FUN_140043990();
  *(undefined8 *)(param_1 + 0x48) = uVar1;
  param_1[0x2d] = (QWidget)0x0;
  *(int *)(param_1 + 0x50) =
       ((*(int *)(*(longlong *)(param_1 + 0x20) + 0x1c) -
        *(int *)(*(longlong *)(param_1 + 0x20) + 0x14)) + 1) / 2;
  *(undefined4 *)(param_1 + 0x54) = 0;
  this = (QMenu *)FUN_140058d94(0x28);
  pQVar6 = (QMenu *)pQVar8;
  local_38[0] = this;
  if (this != (QMenu *)0x0) {
    QMenu::QMenu(this,(QWidget *)0x0);
    *(undefined ***)this = QMenu::vftable;
    *(undefined ***)(this + 0x10) = QMenu::vftable;
    pQVar6 = this;
  }
  *(QMenu **)(param_1 + 0xe0) = pQVar6;
  QWidget::setAttribute(param_1,2,true);
  QColor::setRgb((QColor *)local_38,0xff,0xa5,0,0xff);
  QColor::operator=((QColor *)(param_1 + 0x10c),(QColor *)local_38);
  QColor::setRgb((QColor *)local_38,0xa5,0xff,0xa5,0xff);
  QColor::operator=((QColor *)(param_1 + 0x134),(QColor *)local_38);
  QColor::setRgb((QColor *)local_38,0xa5,0xa5,0xff,0xff);
  QColor::operator=((QColor *)(param_1 + 0x15c),(QColor *)local_38);
  *(undefined4 *)(param_1 + 0x108) = 0;
  *(undefined4 *)(param_1 + 0x130) = 1;
  *(undefined4 *)(param_1 + 0x158) = 2;
  *(undefined2 *)(param_1 + 0x150) = 0;
  *(undefined2 *)(param_1 + 0x178) = 0;
  param_1[0x128] = (QWidget)0x1;
  *(undefined4 *)(param_1 + 0x11c) = 0;
  *(undefined8 *)(param_1 + 0x120) = 0;
  FUN_140055470(DAT_14286d390,param_1[0x2b]);
  pQVar2 = (QShortcut *)FUN_140058d94(0x10);
  pQVar7 = (QShortcut *)pQVar8;
  local_38[0] = (QMenu *)pQVar2;
  if (pQVar2 != (QShortcut *)0x0) {
    QShortcut::QShortcut(pQVar2,param_1);
    *(undefined ***)pQVar2 = QShortcut::vftable;
    pQVar7 = pQVar2;
  }
  *(QShortcut **)(param_1 + 0x90) = pQVar7;
  pQVar2 = (QShortcut *)FUN_140058d94(0x10);
  pQVar7 = (QShortcut *)pQVar8;
  local_38[0] = (QMenu *)pQVar2;
  if (pQVar2 != (QShortcut *)0x0) {
    QShortcut::QShortcut(pQVar2,param_1);
    *(undefined ***)pQVar2 = QShortcut::vftable;
    pQVar7 = pQVar2;
  }
  *(QShortcut **)(param_1 + 0x98) = pQVar7;
  pQVar2 = (QShortcut *)FUN_140058d94(0x10);
  pQVar7 = (QShortcut *)pQVar8;
  local_38[0] = (QMenu *)pQVar2;
  if (pQVar2 != (QShortcut *)0x0) {
    QShortcut::QShortcut(pQVar2,param_1);
    *(undefined ***)pQVar2 = QShortcut::vftable;
    pQVar7 = pQVar2;
  }
  *(QShortcut **)(param_1 + 0xa0) = pQVar7;
  pQVar2 = (QShortcut *)FUN_140058d94(0x10);
  pQVar7 = (QShortcut *)pQVar8;
  local_38[0] = (QMenu *)pQVar2;
  if (pQVar2 != (QShortcut *)0x0) {
    QShortcut::QShortcut(pQVar2,param_1);
    *(undefined ***)pQVar2 = QShortcut::vftable;
    pQVar7 = pQVar2;
  }
  *(QShortcut **)(param_1 + 0xa8) = pQVar7;
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)local_38,0x1000014,0,0,0);
  QShortcut::setKey(*(QShortcut **)(param_1 + 0x90),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)local_38);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)local_38,0x1000012,0,0,0);
  QShortcut::setKey(*(QShortcut **)(param_1 + 0x98),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)local_38);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)local_38,0x1000010,0,0,0);
  QShortcut::setKey(*(QShortcut **)(param_1 + 0xa0),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)local_38);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)local_38,0x1000011,0,0,0);
  QShortcut::setKey(*(QShortcut **)(param_1 + 0xa8),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)local_38);
  QObject::connect(*(QObject **)(param_1 + 0x90),"2activated()",(QObject *)param_1,
                   "1_OnTriggeredMoveGraphToRight(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x98),"2activated()",(QObject *)param_1,
                   "1_OnTriggeredMoveGraphToLeft(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0xa0),"2activated()",(QObject *)param_1,
                   "1_OnTriggeredMoveGraphToStart(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0xa8),"2activated()",(QObject *)param_1,
                   "1_OnTriggeredMoveGraphToEnd(void)",0);
  pQVar2 = (QShortcut *)FUN_140058d94(0x10);
  pQVar7 = (QShortcut *)pQVar8;
  local_38[0] = (QMenu *)pQVar2;
  if (pQVar2 != (QShortcut *)0x0) {
    QShortcut::QShortcut(pQVar2,param_1);
    *(undefined ***)pQVar2 = QShortcut::vftable;
    pQVar7 = pQVar2;
  }
  *(QShortcut **)(param_1 + 0xb0) = pQVar7;
  pQVar2 = (QShortcut *)FUN_140058d94(0x10);
  pQVar7 = (QShortcut *)pQVar8;
  local_38[0] = (QMenu *)pQVar2;
  if (pQVar2 != (QShortcut *)0x0) {
    QShortcut::QShortcut(pQVar2,param_1);
    *(undefined ***)pQVar2 = QShortcut::vftable;
    pQVar7 = pQVar2;
  }
  *(QShortcut **)(param_1 + 0xb8) = pQVar7;
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)local_38,0x4000031,0,0,0);
  QShortcut::setKey(*(QShortcut **)(param_1 + 0xb0),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)local_38);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)local_38,0x4000032,0,0,0);
  QShortcut::setKey(*(QShortcut **)(param_1 + 0xb8),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)local_38);
  QObject::connect(*(QObject **)(param_1 + 0xb0),"2activated()",(QObject *)param_1,
                   "1_SlotOnShowX1(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0xb8),"2activated()",(QObject *)param_1,
                   "1_SlotOnShowX2(void)",0);
  pQVar4 = (QAction *)FUN_140058d94(0x10);
  pQVar5 = pQVar8;
  local_38[0] = (QMenu *)pQVar4;
  if (pQVar4 != (QAction *)0x0) {
    QAction::QAction(pQVar4,(QObject *)param_1);
    *(undefined ***)pQVar4 = QAction::vftable;
    pQVar5 = pQVar4;
  }
  *(QAction **)(param_1 + 0xc0) = pQVar5;
  pQVar4 = (QAction *)FUN_140058d94(0x10);
  pQVar5 = pQVar8;
  local_38[0] = (QMenu *)pQVar4;
  if (pQVar4 != (QAction *)0x0) {
    QAction::QAction(pQVar4,(QObject *)param_1);
    *(undefined ***)pQVar4 = QAction::vftable;
    pQVar5 = pQVar4;
  }
  *(QAction **)(param_1 + 200) = pQVar5;
  pQVar4 = (QAction *)FUN_140058d94(0x10);
  pQVar5 = pQVar8;
  local_38[0] = (QMenu *)pQVar4;
  if (pQVar4 != (QAction *)0x0) {
    QAction::QAction(pQVar4,(QObject *)param_1);
    *(undefined ***)pQVar4 = QAction::vftable;
    pQVar5 = pQVar4;
  }
  *(QAction **)(param_1 + 0xd0) = pQVar5;
  pQVar5 = (QAction *)FUN_140058d94(0x10);
  local_38[0] = (QMenu *)pQVar5;
  if (pQVar5 != (QAction *)0x0) {
    QAction::QAction(pQVar5,(QObject *)param_1);
    *(undefined ***)pQVar5 = QAction::vftable;
    pQVar8 = pQVar5;
  }
  *(QAction **)(param_1 + 0xd8) = pQVar8;
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)local_38,0x5000014,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0xc0),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)local_38);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)local_38,0x9000014,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 200),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)local_38);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)local_38,0x5000012,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0xd0),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)local_38);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)local_38,0x9000012,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0xd8),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)local_38);
  QObject::connect(*(QObject **)(param_1 + 0xc0),"2triggered()",(QObject *)param_1,
                   "1_SlotOnMoveRightX1(void)",0);
  QObject::connect(*(QObject **)(param_1 + 200),"2triggered()",(QObject *)param_1,
                   "1_SlotOnMoveRightX2(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0xd0),"2triggered()",(QObject *)param_1,
                   "1_SlotOnMoveLeftX1(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0xd8),"2triggered()",(QObject *)param_1,
                   "1_SlotOnMoveLeftX2(void)",0);
  QWidget::addAction(param_1,*(QAction **)(param_1 + 0xc0));
  QWidget::addAction(param_1,*(QAction **)(param_1 + 200));
  QWidget::addAction(param_1,*(QAction **)(param_1 + 0xd0));
  QWidget::addAction(param_1,*(QAction **)(param_1 + 0xd8));
  QObject::connect((QObject *)param_1,"2UpdateRequest(void)",(QObject *)param_1,
                   "1_UpdateScreen(void)",0);
  QWidget::setFocusPolicy(param_1,0xb);
  QWidget::show(param_1);
  return param_1;
}


/* ================= FUN_140049680 @ 140049680  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400496b0 */

void FUN_140049680(QWidget *param_1)

{
  *(undefined ***)param_1 = GraphWidget::vftable;
  *(undefined ***)(param_1 + 0x10) = GraphWidget::vftable;
                    /* WARNING: Could not recover jumptable at 0x000140049695. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::~QWidget(param_1);
  return;
}


/* ================= FUN_14004969c @ 14004969c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400496b0 */

void FUN_14004969c(longlong param_1)

{
  FUN_1400496b0(param_1 + -0x10);
  return;
}


/* ================= FUN_1400496b0 @ 1400496b0  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14004969c */
/* CALLEES: FUN_140049680, free */

void * FUN_1400496b0(void *param_1,ulonglong param_2)

{
  FUN_140049680();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_1400496f0 @ 1400496f0  size=25  callers=3 callees=1 ================= */
/* CALLERS: FUN_140049830, FUN_140049f40, FUN_14004a7c0 */
/* CALLEES: FUN_140049800 */

double FUN_1400496f0(longlong param_1)

{
  double dVar1;
  
  dVar1 = (double)FUN_140049800();
  return dVar1 + *(double *)(param_1 + 0x40);
}


/* ================= FUN_140049710 @ 140049710  size=168  callers=2 callees=5 ================= */
/* CALLERS: FUN_14004b8b0, FUN_140053420 */
/* CALLEES: FUN_14003ec80, FUN_14003ed20, FUN_140049f40, FUN_140049fe0, FUN_14004a060 */

undefined8 FUN_140049710(longlong param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined8 uVar5;
  longlong lVar6;
  
  if (param_2 != 0) {
    if (param_2 == 1) {
      lVar6 = 0x148;
      goto LAB_140049743;
    }
    if (param_2 == 2) {
      lVar6 = 0x170;
      goto LAB_140049743;
    }
  }
  lVar6 = 0x120;
LAB_140049743:
  iVar1 = FUN_14003ed20();
  iVar1 = iVar1 + -2;
  if (-1 < iVar1) {
    uVar2 = FUN_14003ec80();
    if (*(ulonglong *)(lVar6 + param_1) <= (ulonglong)uVar2) {
      uVar3 = FUN_14004a060(param_1,0,iVar1);
      uVar4 = FUN_140049f40(param_1,0,iVar1);
      uVar5 = FUN_140049fe0(param_1,uVar3,uVar4,*(undefined4 *)(lVar6 + param_1));
      return uVar5;
    }
  }
  return 0xffffffff;
}


/* ================= FUN_1400497c0 @ 1400497c0  size=38  callers=2 callees=0 ================= */
/* CALLERS: FUN_140051790, FUN_140054250 */

longlong FUN_1400497c0(longlong param_1,int param_2)

{
  if (param_2 != 0) {
    if (param_2 == 1) {
      return param_1 + 0x134;
    }
    if (param_2 == 2) {
      return param_1 + 0x15c;
    }
  }
  return param_1 + 0x10c;
}


/* ================= FUN_1400497f0 @ 1400497f0  size=6  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400482d0 */

undefined8 FUN_1400497f0(longlong param_1)

{
  return *(undefined8 *)(param_1 + 0x40);
}


/* ================= FUN_140049800 @ 140049800  size=46  callers=6 callees=0 ================= */
/* CALLERS: FUN_1400496f0, FUN_140049a10, FUN_14004a6e0, FUN_14004a750, FUN_14004a7c0, FUN_14004b450 */

double FUN_140049800(longlong param_1)

{
  return (double)(((*(int *)(*(longlong *)(param_1 + 0x20) + 0x1c) -
                   *(int *)(*(longlong *)(param_1 + 0x20) + 0x14)) + 1) / 100) *
         *(double *)(param_1 + 0x80);
}


/* ================= FUN_140049830 @ 140049830  size=155  callers=5 callees=1 ================= */
/* CALLERS: FUN_140049a10, FUN_14004bae0, FUN_14004bdc0, FUN_14004c1b0, FUN_14004c470 */
/* CALLEES: FUN_1400496f0 */

longlong FUN_140049830(longlong param_1,int param_2)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  double dVar4;
  double dVar5;
  
  lVar2 = *(longlong *)(param_1 + 0x20);
  iVar1 = *(int *)(param_1 + 0x74);
  dVar5 = *(double *)(param_1 + 0x40);
  dVar4 = (double)FUN_1400496f0();
  lVar3 = 0;
  dVar5 = (dVar4 - dVar5) *
          ((double)(param_2 - iVar1) /
          (double)(((*(int *)(lVar2 + 0x1c) + iVar1 * -2) - *(int *)(lVar2 + 0x14)) + 1)) + dVar5;
  if ((9.223372036854776e+18 <= dVar5) &&
     (dVar5 = dVar5 - 9.223372036854776e+18, dVar5 < 9.223372036854776e+18)) {
    lVar3 = -0x8000000000000000;
  }
  return (longlong)dVar5 + lVar3;
}


/* ================= FUN_1400498d0 @ 1400498d0  size=41  callers=1 callees=0 ================= */
/* CALLERS: FUN_140048100 */

undefined8 FUN_1400498d0(longlong param_1,int param_2)

{
  if (param_2 == 0) {
    return *(undefined8 *)(param_1 + 0x120);
  }
  if (param_2 != 1) {
    if (param_2 != 2) {
      return 0;
    }
    return *(undefined8 *)(param_1 + 0x170);
  }
  return *(undefined8 *)(param_1 + 0x148);
}


/* ================= FUN_140049900 @ 140049900  size=9  callers=3 callees=0 ================= */
/* CALLERS: FUN_140047f50, FUN_140048100, FUN_140048320 */

int FUN_140049900(longlong param_1)

{
  return (int)*(double *)(param_1 + 0x80);
}


/* ================= FUN_140049910 @ 140049910  size=41  callers=2 callees=0 ================= */
/* CALLERS: FUN_140048030, FUN_140054250 */

ulonglong FUN_140049910(longlong param_1,int param_2)

{
  ulonglong in_RAX;
  
  if (param_2 == 0) {
    return (ulonglong)*(byte *)(param_1 + 0x128);
  }
  if (param_2 != 1) {
    if (param_2 != 2) {
      return in_RAX & 0xffffffffffffff00;
    }
    return (ulonglong)*(byte *)(param_1 + 0x178);
  }
  return (ulonglong)*(byte *)(param_1 + 0x150);
}


/* ================= FUN_140049940 @ 140049940  size=5  callers=1 callees=0 ================= */
/* CALLERS: FUN_140047f50 */

void FUN_140049940(longlong param_1)

{
  *(undefined1 *)(param_1 + 0x2d) = 1;
  return;
}


/* ================= FUN_140049950 @ 140049950  size=43  callers=1 callees=2 ================= */
/* CALLERS: FUN_140047fe0 */
/* CALLEES: FUN_14003b990, FUN_14003ed40 */

void FUN_140049950(longlong param_1)

{
  int iVar1;
  
  iVar1 = FUN_14003ed40();
  if (iVar1 != 0) {
    *(undefined8 *)(param_1 + 0x40) = 0;
  }
  *(undefined1 *)(param_1 + 0x2d) = 0;
  FUN_14003b990(param_1);
  return;
}


/* ================= FUN_140049980 @ 140049980  size=4  callers=1 callees=0 ================= */
/* CALLERS: FUN_140056050 */

void FUN_140049980(longlong param_1,undefined1 param_2)

{
  *(undefined1 *)(param_1 + 0x2b) = param_2;
  return;
}


/* ================= FUN_140049990 @ 140049990  size=70  callers=1 callees=1 ================= */
/* CALLERS: FUN_140048d10 */
/* CALLEES: operator= */

void FUN_140049990(longlong param_1,QColor *param_2,QColor *param_3,QColor *param_4)

{
  QColor::operator=((QColor *)(param_1 + 0x10c),param_2);
  QColor::operator=((QColor *)(param_1 + 0x134),param_3);
                    /* WARNING: Could not recover jumptable at 0x0001400499cf. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QColor::operator=((QColor *)(param_1 + 0x15c),param_4);
  return;
}


/* ================= FUN_1400499e0 @ 1400499e0  size=45  callers=1 callees=2 ================= */
/* CALLERS: FUN_140048a50 */
/* CALLEES: FUN_14003b990, FUN_140049c50 */

void FUN_1400499e0(longlong param_1,double param_2)

{
  double dVar1;
  
  dVar1 = *(double *)(param_1 + 0x40);
  *(double *)(param_1 + 0x40) = param_2;
  FUN_140049c50(dVar1,(int)(param_2 - dVar1));
  FUN_14003b990(param_1);
  return;
}


/* ================= FUN_140049a10 @ 140049a10  size=515  callers=1 callees=7 ================= */
/* CALLERS: FUN_1400560d0 */
/* CALLEES: FUN_14003b990, FUN_14003ec80, FUN_140048320, FUN_140049800, FUN_140049830, FUN_140049c50, FUN_140055470 */

void FUN_140049a10(longlong param_1,uint param_2)

{
  double dVar1;
  uint uVar2;
  ulonglong uVar3;
  undefined8 uVar4;
  double dVar5;
  double dVar6;
  
  dVar6 = *(double *)(param_1 + 0x80);
  dVar1 = *(double *)(param_1 + 0x40);
  uVar3 = FUN_140049830(param_1,*(undefined4 *)(param_1 + 0x11c));
  *(double *)(param_1 + 0x80) = (double)param_2;
  if ((double)((longlong)dVar6 & 0xffffffff) <= (double)param_2) {
    uVar3 = FUN_140049830(param_1,*(undefined4 *)(param_1 + 0x11c));
    *(double *)(param_1 + 0x40) = dVar1 - ((double)uVar3 - dVar1) * 0.5;
    uVar2 = FUN_14003ec80();
    dVar6 = *(double *)(param_1 + 0x40);
    dVar5 = (double)FUN_140049800(param_1);
    if ((double)uVar2 - dVar5 <= dVar6) {
      uVar2 = FUN_14003ec80();
      dVar6 = (double)FUN_140049800(param_1);
      dVar6 = (double)uVar2 - dVar6;
      *(double *)(param_1 + 0x40) = dVar6;
    }
    if (dVar6 < 0.0) {
      *(undefined8 *)(param_1 + 0x40) = 0;
    }
    uVar4 = 0xffffffff;
  }
  else {
    *(double *)(param_1 + 0x40) = ((double)uVar3 - dVar1) * 0.5 + dVar1;
    uVar2 = FUN_14003ec80();
    dVar6 = *(double *)(param_1 + 0x40);
    dVar5 = (double)FUN_140049800(param_1);
    if ((double)uVar2 - dVar5 <= dVar6) {
      uVar2 = FUN_14003ec80();
      dVar6 = (double)FUN_140049800(param_1);
      dVar6 = (double)uVar2 - dVar6;
      *(double *)(param_1 + 0x40) = dVar6;
    }
    if (dVar6 < 0.0) {
      *(undefined8 *)(param_1 + 0x40) = 0;
    }
    uVar4 = 1;
  }
  FUN_140049c50(param_1,uVar4);
  FUN_14003b990(param_1);
  dVar6 = *(double *)(param_1 + 0x40);
  if (dVar1 != dVar6) {
    FUN_140055470(DAT_14286d390,0);
    dVar6 = *(double *)(param_1 + 0x40);
  }
  FUN_140048320(DAT_14286d3a8,(int)dVar6);
  return;
}


/* ================= FUN_140049c20 @ 140049c20  size=43  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400482d0 */
/* CALLEES: FUN_14003b990 */

void FUN_140049c20(longlong param_1)

{
  if (((DAT_14286d3b8 != (longlong *)0x0) && (*DAT_14286d3b8 != 0)) &&
     (*(char *)(*DAT_14286d3b8 + 0x388) == '\0')) {
    *(undefined8 *)(param_1 + 0x40) = 0;
    *(undefined4 *)(param_1 + 0x54) = 0;
  }
  FUN_14003b990();
  return;
}


/* ================= FUN_140049c50 @ 140049c50  size=369  callers=4 callees=1 ================= */
/* CALLERS: FUN_1400499e0, FUN_140049a10, FUN_14004ad90, FUN_14004bdc0 */
/* CALLEES: FUN_14003edd0 */

void FUN_140049c50(longlong param_1,int param_2)

{
  double dVar1;
  int iVar2;
  double dVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  int iVar7;
  longlong lVar8;
  longlong lVar9;
  uint uVar10;
  
  if (param_2 != 0) {
    iVar7 = *(int *)(param_1 + 0x54);
    dVar6 = 1.0;
    dVar5 = 0.0;
    dVar4 = -1.0;
    lVar8 = FUN_14003edd0();
    uVar10 = 0;
    if (*(int *)(param_1 + 0x54) < 0) {
      *(undefined4 *)(param_1 + 0x54) = 0;
    }
    do {
      if ((dVar5 == dVar6) && (dVar5 == dVar4)) goto LAB_140049d6e;
      iVar2 = *(int *)(param_1 + 0x54);
      lVar9 = (longlong)iVar2;
      dVar1 = *(double *)(param_1 + 0x40);
      dVar3 = dVar1 - (double)*(uint *)(lVar8 + lVar9 * 4);
      dVar5 = ABS(dVar3);
      if (param_2 < 1) {
        if (param_2 < 0) {
          if (iVar2 == 0) goto LAB_140049d6e;
          if (0 < iVar2) {
            dVar1 = dVar1 - (double)*(uint *)(lVar8 + -4 + lVar9 * 4);
            dVar4 = ABS(dVar1);
            if (ABS(dVar3) < ABS(dVar1) || ABS(dVar3) == ABS(dVar1)) goto LAB_140049d6e;
            *(int *)(param_1 + 0x54) = iVar2 + -1;
            if (iVar2 + -1 < 0) {
              *(undefined4 *)(param_1 + 0x54) = 0;
            }
          }
        }
      }
      else {
        if (iVar2 == 0x7fffff) goto LAB_140049d6e;
        if (iVar2 < 0x800000) {
          dVar1 = dVar1 - (double)*(uint *)(lVar8 + 4 + lVar9 * 4);
          dVar6 = ABS(dVar1);
          if (ABS(dVar3) < ABS(dVar1) || ABS(dVar3) == ABS(dVar1)) goto LAB_140049d6e;
          *(int *)(param_1 + 0x54) = iVar2 + 1;
        }
      }
      uVar10 = uVar10 + 1;
    } while (uVar10 < 0x800001);
    *(undefined8 *)(param_1 + 0x40) = 0;
    *(undefined4 *)(param_1 + 0x54) = 0;
LAB_140049d6e:
    iVar2 = *(int *)(param_1 + 0x54);
    if (iVar7 == iVar2) {
      if (param_2 < 1) {
        iVar7 = iVar2 + -1;
        if (iVar2 + -1 < 0) {
          iVar7 = 0;
        }
        *(int *)(param_1 + 0x54) = iVar7;
      }
      else {
        *(int *)(param_1 + 0x54) = iVar2 + 1;
        if (0x7fffff < iVar2 + 1) {
          *(undefined4 *)(param_1 + 0x54) = 0x7fffff;
          return;
        }
      }
    }
  }
  return;
}


/* ================= FUN_140049dd0 @ 140049dd0  size=52  callers=0 callees=1 ================= */
/* CALLEES: drawLines */

void FUN_140049dd0(QPainter *param_1,undefined8 param_2,undefined8 param_3)

{
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = (undefined4)param_2;
  local_10 = (undefined4)param_3;
  local_14 = (undefined4)((ulonglong)param_2 >> 0x20);
  local_c = (undefined4)((ulonglong)param_3 >> 0x20);
  QPainter::drawLines(param_1,(QLine *)&local_18,1);
  return;
}


/* ================= FUN_140049e10 @ 140049e10  size=128  callers=0 callees=2 ================= */
/* CALLEES: drawLines, drawPoints */

void FUN_140049e10(QPainter *param_1,undefined8 param_2,undefined8 param_3)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uStackX_1c;
  int local_res20;
  undefined4 local_res24;
  int local_28;
  undefined4 local_24;
  int local_20;
  undefined4 local_1c;
  
  uVar1 = (undefined4)((ulonglong)param_2 >> 0x20);
  iVar3 = (int)param_2;
  local_res20 = iVar3;
  local_res24 = uVar1;
  QPainter::drawPoints(param_1,(QPoint *)&local_res20,1);
  iVar2 = (int)param_3 - iVar3;
  local_28 = (int)(iVar2 + (iVar2 >> 0x1f & 3U)) >> 2;
  local_20 = (int)param_3 - local_28;
  local_28 = local_28 + iVar3;
  uStackX_1c = (undefined4)((ulonglong)param_3 >> 0x20);
  local_1c = uStackX_1c;
  local_24 = uVar1;
  QPainter::drawLines(param_1,(QLine *)&local_28,1);
  return;
}


/* ================= FUN_140049e90 @ 140049e90  size=38  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004b640 */
/* CALLEES: drawPoints */

void FUN_140049e90(QPainter *param_1,undefined8 param_2)

{
  undefined4 local_res10;
  undefined4 local_res14;
  
  local_res10 = (undefined4)param_2;
  local_res14 = (undefined4)((ulonglong)param_2 >> 0x20);
  QPainter::drawPoints(param_1,(QPoint *)&local_res10,1);
  return;
}


/* ================= FUN_140049ec0 @ 140049ec0  size=113  callers=0 callees=1 ================= */
/* CALLEES: drawLines */

void FUN_140049ec0(QPainter *param_1,undefined8 param_2,undefined8 param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uStackX_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_18 = (undefined4)param_2;
  uVar1 = (undefined4)((ulonglong)param_2 >> 0x20);
  uVar2 = (undefined4)param_3;
  local_14 = uVar1;
  local_10 = uVar2;
  local_c = uVar1;
  QPainter::drawLines(param_1,(QLine *)&local_18,1);
  uStackX_1c = (undefined4)((ulonglong)param_3 >> 0x20);
  local_c = uStackX_1c;
  local_18 = uVar2;
  local_14 = uVar1;
  local_10 = uVar2;
  QPainter::drawLines(param_1,(QLine *)&local_18,1);
  return;
}


/* ================= FUN_140049f40 @ 140049f40  size=158  callers=2 callees=2 ================= */
/* CALLERS: FUN_140049710, FUN_14004ad90 */
/* CALLEES: FUN_14003ed70, FUN_1400496f0 */

int FUN_140049f40(undefined8 param_1,int param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  double dVar3;
  
  while ((((iVar2 = param_2, uVar1 = FUN_14003ed70(param_3), uVar1 == 0 ||
           (dVar3 = (double)FUN_1400496f0(param_1), dVar3 < (double)uVar1)) && (iVar2 != param_3))
         && (iVar2 != param_3 + -1))) {
    param_2 = iVar2 + (param_3 - iVar2) / 2;
    uVar1 = FUN_14003ed70(param_2);
    dVar3 = (double)FUN_1400496f0(param_1);
    if (dVar3 <= (double)uVar1) {
      param_3 = param_2;
      param_2 = iVar2;
    }
  }
  return param_3;
}


/* ================= FUN_140049fe0 @ 140049fe0  size=114  callers=2 callees=1 ================= */
/* CALLERS: FUN_140049710, FUN_14004bc80 */
/* CALLEES: FUN_14003ed70 */

int FUN_140049fe0(undefined8 param_1,int param_2,int param_3,uint param_4)

{
  uint uVar1;
  int iVar2;
  
  if (param_2 == param_3) {
    return param_3;
  }
  do {
    if (param_2 == param_3 + -1) {
      return param_3;
    }
    iVar2 = param_2 + (param_3 - param_2) / 2;
    uVar1 = FUN_14003ed70(iVar2);
    if (param_4 <= uVar1) {
      param_3 = iVar2;
      iVar2 = param_2;
    }
    param_2 = iVar2;
  } while (param_2 != param_3);
  return param_3;
}


/* ================= FUN_14004a060 @ 14004a060  size=129  callers=2 callees=1 ================= */
/* CALLERS: FUN_140049710, FUN_14004ad90 */
/* CALLEES: FUN_14003ed70 */

int FUN_14004a060(longlong param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  if (param_2 == param_3) {
    return param_3;
  }
  do {
    if (param_2 == param_3 + -1) {
      return param_3;
    }
    iVar2 = param_3 - param_2;
    if (iVar2 < 0) {
      iVar2 = iVar2 + 1;
    }
    iVar2 = (iVar2 >> 1) + param_2;
    uVar3 = FUN_14003ed70(iVar2);
    iVar1 = iVar2;
    if ((double)uVar3 < *(double *)(param_1 + 0x40)) {
      iVar1 = param_3;
      param_2 = iVar2;
    }
    param_3 = iVar1;
  } while (param_2 != param_3);
  return param_3;
}


/* ================= FUN_14004a0f0 @ 14004a0f0  size=180  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004b640 */
/* CALLEES: FUN_14004a1b0 */

void FUN_14004a0f0(longlong param_1,int *param_2,longlong param_3,undefined8 param_4,uint param_5,
                  uint param_6)

{
  double dVar1;
  double dVar2;
  double dVar3;
  
  dVar3 = (double)FUN_14004a1b0(*(undefined4 *)(param_3 + 0x314),param_4);
  dVar1 = *(double *)(param_1 + 0x40);
  dVar2 = *(double *)(param_1 + 0x80);
  param_2[1] = (int)((double)(*(int *)(param_1 + 0x6c) -
                             (int)((dVar3 / (double)*(uint *)(param_3 + 0x358)) * 100.0)) -
                    *(double *)(param_3 + 0x350) * 100.0);
  *param_2 = (int)(((double)param_5 - dVar1) / (dVar2 / 100.0) + (double)param_6);
  return;
}


/* ================= FUN_14004a1b0 @ 14004a1b0  size=195  callers=2 callees=1 ================= */
/* CALLERS: FUN_14004a0f0, FUN_14004b8b0 */
/* CALLEES: FUN_14003eaa0 */

double FUN_14004a1b0(undefined4 param_1,double *param_2,undefined8 param_3)

{
  uint uVar1;
  
  switch(param_1) {
  case 1:
    return (double)*(float *)param_2;
  case 2:
    return *param_2;
  default:
    return 0.0;
  case 5:
  case 6:
    return (double)*(byte *)param_2;
  case 7:
    return (double)*(ushort *)param_2;
  case 8:
    return (double)(uint)*(float *)param_2;
  case 10:
    return (double)(int)*(char *)param_2;
  case 0xb:
    return (double)(int)*(short *)param_2;
  case 0xc:
    return (double)(int)*(float *)param_2;
  case 0xe:
    uVar1 = FUN_14003eaa0(*(float *)param_2,param_3);
    return (double)uVar1;
  }
}


/* ================= FUN_14004a2b0 @ 14004a2b0  size=34  callers=3 callees=0 ================= */
/* CALLERS: FUN_14004af10, FUN_14004bdc0, FUN_14004c020 */

int FUN_14004a2b0(longlong param_1,longlong param_2)

{
  return (int)((double)*(int *)(param_1 + 0x6c) - *(double *)(param_2 + 0x350) * 100.0);
}


/* ================= FUN_14004a2e0 @ 14004a2e0  size=244  callers=1 callees=0 ================= */
/* CALLERS: FUN_14004bc80 */

void FUN_14004a2e0(double *param_1,double *param_2,double *param_3,undefined4 param_4)

{
  double dVar1;
  double dVar2;
  byte bVar3;
  byte bVar4;
  char cVar5;
  char cVar6;
  ushort uVar7;
  ushort uVar8;
  short sVar9;
  short sVar10;
  float fVar11;
  float fVar12;
  
  *(undefined1 *)((longlong)param_1 + 0xc) = 1;
  switch(param_4) {
  default:
    *(undefined1 *)((longlong)param_1 + 0xc) = 0;
    return;
  case 1:
    fVar11 = *(float *)param_2;
    fVar12 = *(float *)param_3;
    *(undefined4 *)(param_1 + 1) = 2;
    *param_1 = (double)(fVar11 - fVar12);
    return;
  case 2:
    dVar1 = *param_2;
    dVar2 = *param_3;
    *(undefined4 *)(param_1 + 1) = 2;
    *param_1 = dVar1 - dVar2;
    return;
  case 6:
    bVar3 = *(byte *)param_2;
    bVar4 = *(byte *)param_3;
    *(undefined4 *)(param_1 + 1) = 0xd;
    *param_1 = (double)((ulonglong)bVar3 - (ulonglong)bVar4);
    return;
  case 7:
    uVar7 = *(ushort *)param_2;
    uVar8 = *(ushort *)param_3;
    *(undefined4 *)(param_1 + 1) = 0xd;
    *param_1 = (double)((ulonglong)uVar7 - (ulonglong)uVar8);
    return;
  case 8:
    fVar11 = *(float *)param_2;
    fVar12 = *(float *)param_3;
    *(undefined4 *)(param_1 + 1) = 0xd;
    *param_1 = (double)((ulonglong)(uint)fVar11 - (ulonglong)(uint)fVar12);
    return;
  case 10:
    cVar5 = *(char *)param_3;
    cVar6 = *(char *)param_2;
    *(undefined4 *)(param_1 + 1) = 0xd;
    *param_1 = (double)(longlong)((int)cVar6 - (int)cVar5);
    return;
  case 0xb:
    sVar9 = *(short *)param_3;
    sVar10 = *(short *)param_2;
    *(undefined4 *)(param_1 + 1) = 0xd;
    *param_1 = (double)(longlong)((int)sVar10 - (int)sVar9);
    return;
  case 0xc:
    *param_1 = (double)(longlong)((int)*(float *)param_2 - (int)*(float *)param_3);
    *(undefined4 *)(param_1 + 1) = 0xd;
    return;
  case 0xe:
    *param_1 = (double)(ulonglong)(uint)((int)*(float *)param_2 - (int)*(float *)param_3);
    *(undefined4 *)(param_1 + 1) = 0xd;
    return;
  }
}


/* ================= FUN_14004a410 @ 14004a410  size=156  callers=2 callees=2 ================= */
/* CALLERS: FUN_14004bbd0, FUN_14004bbe0 */
/* CALLEES: FUN_140048030, FUN_14004e120 */

void FUN_14004a410(longlong *param_1,longlong param_2)

{
  longlong lVar1;
  longlong lVar2;
  double dVar3;
  
  if (*(char *)(param_2 + 0x20) != '\0') {
    lVar1 = 0;
    dVar3 = (double)param_1[0x10] / 10.0;
    if ((9.223372036854776e+18 <= dVar3) &&
       (dVar3 = dVar3 - 9.223372036854776e+18, dVar3 < 9.223372036854776e+18)) {
      lVar1 = -0x8000000000000000;
    }
    lVar2 = *(longlong *)(param_2 + 0x18) - ((longlong)dVar3 + lVar1);
    if (*(ulonglong *)(param_2 + 0x18) <= (ulonglong)((longlong)dVar3 + lVar1)) {
      lVar2 = 0;
    }
    *(longlong *)(param_2 + 0x18) = lVar2;
    FUN_140048030(DAT_14286d3a8);
    (**(code **)(*param_1 + 0xd0))(param_1,0);
    FUN_14004e120(DAT_14286d388);
    return;
  }
  return;
}


/* ================= FUN_14004a4b0 @ 14004a4b0  size=161  callers=2 callees=2 ================= */
/* CALLERS: FUN_14004bbf0, FUN_14004bc00 */
/* CALLEES: FUN_140048030, FUN_14004e120 */

void FUN_14004a4b0(longlong *param_1,longlong param_2)

{
  longlong lVar1;
  longlong lVar2;
  double dVar3;
  
  if (*(char *)(param_2 + 0x20) != '\0') {
    lVar1 = 0;
    dVar3 = (double)param_1[0x10] / 10.0;
    if ((9.223372036854776e+18 <= dVar3) &&
       (dVar3 = dVar3 - 9.223372036854776e+18, dVar3 < 9.223372036854776e+18)) {
      lVar1 = -0x8000000000000000;
    }
    lVar2 = *(ulonglong *)(param_2 + 0x18) + (longlong)dVar3 + lVar1;
    if ((ulonglong)~((longlong)dVar3 + lVar1) <= *(ulonglong *)(param_2 + 0x18)) {
      lVar2 = -1;
    }
    *(longlong *)(param_2 + 0x18) = lVar2;
    FUN_140048030(DAT_14286d3a8);
    (**(code **)(*param_1 + 0xd0))(param_1,0);
    FUN_14004e120(DAT_14286d388);
    return;
  }
  return;
}


/* ================= FUN_14004a560 @ 14004a560  size=371  callers=2 callees=8 ================= */
/* CALLERS: FUN_14003b8e0, FUN_14004c020 */
/* CALLEES: FUN_140036150, FUN_140036210, exec, fromAscii_helper, mapToGlobal, setEnabled, setStyleSheet, ~QString */

void FUN_14004a560(QWidget *param_1,undefined8 param_2)

{
  undefined8 uVar1;
  QPoint *pQVar2;
  Data *local_res8;
  undefined8 local_res10;
  
  local_res10 = param_2;
  uVar1 = FUN_140036150(param_1,0,0);
  *(undefined8 *)(param_1 + 0xe0) = uVar1;
  local_res8 = QString::fromAscii_helper
                         ("QMenu::indicator     { width: 0; height: 0;       }QMenu::item:selected { background-color: #90C8F6; }QMenu::item:selected { color:            #000000; }"
                          ,-1);
  QWidget::setStyleSheet(*(QWidget **)(param_1 + 0xe0),(QString *)&local_res8);
  QString::~QString((QString *)&local_res8);
  uVar1 = FUN_140036210(param_1,*(undefined8 *)(param_1 + 0xe0),"Set measurement X1",0,param_1,
                        "1_SlotOnShowX1(void)");
  *(undefined8 *)(param_1 + 0xe8) = uVar1;
  uVar1 = FUN_140036210(param_1,*(undefined8 *)(param_1 + 0xe0),"Set measurement X2",0,param_1,
                        "1_SlotOnShowX2(void)");
  *(undefined8 *)(param_1 + 0xf0) = uVar1;
  uVar1 = FUN_140036210(param_1,*(undefined8 *)(param_1 + 0xe0),"Clear measurement X1",0,param_1,
                        "1_SlotOnHideX1(void)");
  *(undefined8 *)(param_1 + 0xf8) = uVar1;
  uVar1 = FUN_140036210(param_1,*(undefined8 *)(param_1 + 0xe0),"Clear measurement X2",0,param_1,
                        "1_SlotOnHideX2(void)");
  *(undefined8 *)(param_1 + 0x100) = uVar1;
  if (param_1[0x150] == (QWidget)0x0) {
    QAction::setEnabled(*(QAction **)(param_1 + 0xf8),false);
  }
  if (param_1[0x178] == (QWidget)0x0) {
    QAction::setEnabled(*(QAction **)(param_1 + 0x100),false);
  }
  pQVar2 = (QPoint *)QWidget::mapToGlobal(param_1,(QPoint *)&local_res8);
  QMenu::exec(*(QMenu **)(param_1 + 0xe0),pQVar2,(QAction *)0x0);
  return;
}


/* ================= FUN_14004a6e0 @ 14004a6e0  size=99  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14003ec80, FUN_140049800, FUN_14004e120, FUN_140055470 */

void FUN_14004a6e0(longlong param_1)

{
  double dVar1;
  uint uVar2;
  
  FUN_140055470(DAT_14286d390,0);
  uVar2 = FUN_14003ec80();
  dVar1 = (double)FUN_140049800(param_1);
  *(double *)(param_1 + 0x40) = (double)uVar2 - dVar1;
  if ((double)uVar2 - dVar1 < 0.0) {
    *(undefined8 *)(param_1 + 0x40) = 0;
  }
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_14004a750 @ 14004a750  size=99  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_140049800, FUN_14004e120, FUN_140055470 */

void FUN_14004a750(longlong param_1)

{
  double dVar1;
  
  FUN_140055470(DAT_14286d390,0);
  dVar1 = (double)FUN_140049800(param_1);
  dVar1 = *(double *)(param_1 + 0x40) - (double)(int)(dVar1 / 10.0);
  *(double *)(param_1 + 0x40) = dVar1;
  if (dVar1 < 0.0) {
    *(undefined8 *)(param_1 + 0x40) = 0;
  }
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_14004a7c0 @ 14004a7c0  size=115  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_1400496f0, FUN_140049800, FUN_14004e120, FUN_140055470 */

void FUN_14004a7c0(longlong param_1)

{
  double dVar1;
  undefined8 uVar2;
  double dVar3;
  
  FUN_140055470(DAT_14286d390,0);
  dVar1 = (double)FUN_140049800(param_1);
  dVar3 = (double)(int)(dVar1 / 10.0) + *(double *)(param_1 + 0x40);
  *(double *)(param_1 + 0x40) = dVar3;
  dVar1 = (double)FUN_1400496f0(param_1);
  if (dVar1 < dVar3) {
    uVar2 = FUN_1400496f0(param_1);
    *(undefined8 *)(param_1 + 0x40) = uVar2;
  }
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_14004a840 @ 14004a840  size=48  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14004e120, FUN_140055470 */

void FUN_14004a840(longlong param_1)

{
  FUN_140055470(DAT_14286d390,0);
  *(undefined8 *)(param_1 + 0x40) = 0;
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_14004a870 @ 14004a870  size=1312  callers=1 callees=4 ================= */
/* CALLERS: FUN_14004c1b0 */
/* CALLEES: __security_check_cookie, drawLines, setPen, setRgb */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14004a870(longlong param_1,QPainter *param_2)

{
  longlong lVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined1 auStackY_78 [32];
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStackY_78;
  QColor::setRgb((QColor *)&local_38,0xc0,0xc0,0xc0,0xff);
  QPainter::setPen(param_2,(QColor *)&local_38);
  local_44 = *(undefined4 *)(param_1 + 0x6c);
  local_48 = *(int *)(param_1 + 0x74);
  local_40 = ((*(int *)(*(longlong *)(param_1 + 0x20) + 0x1c) - local_48) -
             *(int *)(*(longlong *)(param_1 + 0x20) + 0x14)) + 1;
  local_3c = local_44;
  QPainter::drawLines(param_2,(QLine *)&local_48,1);
  QColor::setRgb((QColor *)&local_38,0xc0,0xc0,0xc0,0xff);
  QPainter::setPen(param_2,(QColor *)&local_38);
  local_48 = *(int *)(param_1 + 0x70);
  local_44 = *(int *)(param_1 + 0x78);
  local_3c = ((*(int *)(*(longlong *)(param_1 + 0x20) + 0x20) -
              *(int *)(*(longlong *)(param_1 + 0x20) + 0x18)) - local_44) + 1;
  local_40 = local_48;
  QPainter::drawLines(param_2,(QLine *)&local_48,1);
  QColor::setRgb((QColor *)&local_38,0xc0,0xc0,0xc0,0xff);
  QPainter::setPen(param_2,(QColor *)&local_38);
  iVar3 = 0;
  iVar2 = 0;
  iVar4 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x1c);
  iVar5 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x14);
  if (0 < ((iVar4 - ((iVar4 - iVar5) + 1) % 100) - iVar5) + 0x15) {
    do {
      lVar1 = *(longlong *)(param_1 + 0x20);
      iVar5 = (*(int *)(lVar1 + 0x1c) - *(int *)(lVar1 + 0x14)) + 1;
      local_48 = (iVar5 % 100) / 2 + iVar2;
      iVar4 = ((*(int *)(lVar1 + 0x20) - *(int *)(lVar1 + 0x18)) + 1) % 100;
      if (iVar4 < 0) {
        iVar4 = iVar4 + 1;
      }
      local_44 = (iVar4 >> 1) + (*(int *)(param_1 + 100) / 2) * 100;
      local_40 = (iVar5 % 100) / 2 + iVar2;
      local_3c = local_44 + 5;
      QPainter::drawLines(param_2,(QLine *)&local_48,1);
      lVar1 = *(longlong *)(param_1 + 0x20);
      iVar5 = (*(int *)(lVar1 + 0x1c) - *(int *)(lVar1 + 0x14)) + 1;
      local_38 = (iVar5 % 100) / 2 + iVar2;
      iVar4 = ((*(int *)(lVar1 + 0x20) - *(int *)(lVar1 + 0x18)) + 1) % 100;
      if (iVar4 < 0) {
        iVar4 = iVar4 + 1;
      }
      local_34 = (iVar4 >> 1) + (*(int *)(param_1 + 100) / 2) * 100;
      local_30 = (iVar5 % 100) / 2 + iVar2;
      local_2c = local_34 + -5;
      QPainter::drawLines(param_2,(QLine *)&local_38,1);
      iVar2 = iVar2 + 0x14;
      iVar4 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x1c);
      iVar5 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x14);
    } while (iVar2 < ((iVar4 - ((iVar4 - iVar5) + 1) % 100) - iVar5) + 0x15);
  }
  QColor::setRgb((QColor *)&local_38,0xc0,0xc0,0xc0,0xff);
  QPainter::setPen(param_2,(QColor *)&local_38);
  iVar4 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x20);
  iVar5 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x18);
  if (0 < ((iVar4 - ((iVar4 - iVar5) + 1) % 100) - iVar5) + 0x15) {
    do {
      lVar1 = *(longlong *)(param_1 + 0x20);
      iVar5 = (*(int *)(lVar1 + 0x1c) - *(int *)(lVar1 + 0x14)) + 1;
      iVar2 = (*(int *)(param_1 + 0x68) / 2) * 100;
      local_38 = (iVar5 % 100) / 2 + iVar2;
      iVar4 = ((*(int *)(lVar1 + 0x20) - *(int *)(lVar1 + 0x18)) + 1) % 100;
      if (iVar4 < 0) {
        iVar4 = iVar4 + 1;
      }
      local_34 = (iVar4 >> 1) + iVar3;
      iVar5 = iVar5 % 100;
      if (iVar5 < 0) {
        iVar5 = iVar5 + 1;
      }
      local_30 = iVar2 + 5 + (iVar5 >> 1);
      local_2c = local_34;
      QPainter::drawLines(param_2,(QLine *)&local_38,1);
      lVar1 = *(longlong *)(param_1 + 0x20);
      iVar4 = (*(int *)(lVar1 + 0x1c) - *(int *)(lVar1 + 0x14)) + 1;
      iVar2 = (*(int *)(param_1 + 0x68) / 2) * 100;
      local_48 = (iVar4 % 100) / 2 + iVar2;
      iVar5 = ((*(int *)(lVar1 + 0x20) - *(int *)(lVar1 + 0x18)) + 1) % 100;
      if (iVar5 < 0) {
        iVar5 = iVar5 + 1;
      }
      local_44 = (iVar5 >> 1) + iVar3;
      iVar4 = iVar4 % 100;
      if (iVar4 < 0) {
        iVar4 = iVar4 + 1;
      }
      local_40 = iVar2 + -5 + (iVar4 >> 1);
      local_3c = local_44;
      QPainter::drawLines(param_2,(QLine *)&local_48,1);
      iVar3 = iVar3 + 0x14;
      iVar4 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x20);
      iVar5 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x18);
    } while (iVar3 < ((iVar4 - ((iVar4 - iVar5) + 1) % 100) - iVar5) + 0x15);
  }
  return;
}


/* ================= FUN_14004ad90 @ 14004ad90  size=376  callers=1 callees=7 ================= */
/* CALLERS: FUN_14004c1b0 */
/* CALLEES: FUN_14003ecb0, FUN_14003ed20, FUN_14003edd0, FUN_140049c50, FUN_140049f40, FUN_14004a060, FUN_14004b640 */

void FUN_14004ad90(longlong param_1,undefined8 param_2)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  longlong lVar7;
  longlong *plVar8;
  double dVar9;
  double dVar10;
  
  plVar8 = DAT_14286d3b8;
  lVar5 = 0;
  lVar4 = FUN_14003edd0();
  if (plVar8 != (longlong *)0x0) {
    iVar1 = FUN_14003ed20();
    iVar3 = 0;
    if (-1 < iVar1 + -2) {
      iVar3 = iVar1 + -2;
    }
    uVar2 = FUN_14004a060(param_1,0,iVar3);
    iVar3 = FUN_140049f40(param_1,0,iVar3);
    lVar6 = lVar5;
    if (iVar3 != 0) {
      iVar3 = iVar3 + -1;
      lVar6 = 0;
    }
    do {
      lVar5 = *plVar8;
      lVar7 = lVar6;
      if (((*(char *)(lVar5 + 0x35e) != '\0') && (*(char *)(lVar5 + 0x388) != '\0')) &&
         (lVar7 = lVar5, *(char *)(lVar5 + 0x380) == '\0')) {
        FUN_14004b640(param_1,param_2,lVar5,uVar2,iVar3);
        lVar7 = lVar6;
      }
      plVar8 = (longlong *)plVar8[1];
      lVar6 = lVar7;
    } while (plVar8 != (longlong *)0x0);
    if (lVar7 != 0) {
      FUN_14004b640(param_1,param_2,lVar7,uVar2,iVar3);
    }
  }
  iVar3 = FUN_14003ecb0();
  if (((iVar3 != 0) && (lVar5 != 0)) &&
     ((*(longlong *)(lVar5 + 0x3c8) != 0 && (*(char *)(param_1 + 0x2b) != '\0')))) {
    dVar9 = (double)*(uint *)(lVar4 + (longlong)(iVar3 + -1) * 4);
    dVar10 = (double)(((*(int *)(*(longlong *)(param_1 + 0x20) + 0x1c) -
                       *(int *)(*(longlong *)(param_1 + 0x20) + 0x14)) + 1) / 100) *
             *(double *)(param_1 + 0x80);
    if (dVar10 < dVar9) {
      *(double *)(param_1 + 0x40) = dVar9 - dVar10;
      FUN_140049c50(param_1,1);
    }
  }
  return;
}


/* ================= FUN_14004af10 @ 14004af10  size=890  callers=1 callees=15 ================= */
/* CALLERS: FUN_14004c1b0 */
/* CALLEES: FUN_14004a2b0, FUN_140058d94, QBrush, QPainterPath, brush, drawPath, fillPath, free, lineTo, moveTo, setBrush, setPen, setRenderHint, ~QBrush, ~QPainterPath */

void FUN_14004af10(longlong param_1,QPainter *param_2)

{
  longlong lVar1;
  QColor *pQVar2;
  int iVar3;
  QPainterPath *this;
  QBrush *pQVar4;
  int iVar5;
  longlong *plVar6;
  int iVar7;
  int local_res18;
  QBrush local_res20 [8];
  double local_138 [2];
  double local_128 [2];
  double local_118 [2];
  double local_108 [2];
  double local_f8;
  double local_f0;
  double local_e8;
  double local_e0;
  double local_d8;
  double local_d0;
  double local_c8;
  double local_c0;
  undefined8 local_b8;
  double local_b0;
  undefined8 local_a8;
  double local_a0;
  undefined8 local_98;
  double local_90;
  undefined8 local_88;
  double local_80;
  QPainterPath *local_78;
  
  if (DAT_14286d3b8 != (longlong *)0x0) {
    iVar7 = 5;
    local_res18 = 5;
    plVar6 = DAT_14286d3b8;
    do {
      lVar1 = *plVar6;
      pQVar2 = *(QColor **)(lVar1 + 0x378);
      if (pQVar2 == (QColor *)0x0) {
        return;
      }
      QPainter::setRenderHint(param_2,1,true);
      local_78 = (QPainterPath *)FUN_140058d94(8);
      if (local_78 == (QPainterPath *)0x0) {
        this = (QPainterPath *)0x0;
      }
      else {
        this = (QPainterPath *)QPainterPath::QPainterPath(local_78);
      }
      iVar3 = FUN_14004a2b0(param_1,lVar1);
      if (iVar3 < *(int *)(param_1 + 0x78)) {
        local_138[1] = 5.0;
        local_138[0] = (double)(iVar7 + -5);
        QPainterPath::moveTo(this,(QPointF *)local_138);
        local_128[0] = (double)(iVar7 + 5);
        local_128[1] = 5.0;
        QPainterPath::lineTo(this,(QPointF *)local_128);
        local_118[0] = (double)iVar7;
        local_118[1] = 0.0;
        QPainterPath::lineTo(this,(QPointF *)local_118);
        local_108[1] = 5.0;
        local_108[0] = (double)(iVar7 + -5);
        QPainterPath::lineTo(this,(QPointF *)local_108);
        iVar7 = iVar7 + 10;
      }
      else {
        iVar5 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x20) -
                *(int *)(*(longlong *)(param_1 + 0x20) + 0x18);
        if ((iVar5 - *(int *)(param_1 + 0x78)) + 1 < iVar3) {
          local_f0 = (double)(iVar5 + -4);
          local_f8 = (double)(local_res18 + -5);
          QPainterPath::moveTo(this,(QPointF *)&local_f8);
          local_e0 = (double)((*(int *)(*(longlong *)(param_1 + 0x20) + 0x20) -
                              *(int *)(*(longlong *)(param_1 + 0x20) + 0x18)) + -4);
          local_e8 = (double)(local_res18 + 5);
          QPainterPath::lineTo(this,(QPointF *)&local_e8);
          local_d0 = (double)((*(int *)(*(longlong *)(param_1 + 0x20) + 0x20) -
                              *(int *)(*(longlong *)(param_1 + 0x20) + 0x18)) + 1);
          local_d8 = (double)local_res18;
          QPainterPath::lineTo(this,(QPointF *)&local_d8);
          local_c0 = (double)((*(int *)(*(longlong *)(param_1 + 0x20) + 0x20) -
                              *(int *)(*(longlong *)(param_1 + 0x20) + 0x18)) + -4);
          local_c8 = (double)(local_res18 + -5);
          QPainterPath::lineTo(this,(QPointF *)&local_c8);
          local_res18 = local_res18 + 10;
        }
        else {
          local_b8 = 0;
          local_b0 = (double)(iVar3 + 5);
          QPainterPath::moveTo(this,(QPointF *)&local_b8);
          local_a8 = 0;
          local_a0 = (double)(iVar3 + -5);
          QPainterPath::lineTo(this,(QPointF *)&local_a8);
          local_98 = 0x4014000000000000;
          local_90 = (double)iVar3;
          QPainterPath::lineTo(this,(QPointF *)&local_98);
          local_88 = 0;
          local_80 = (double)(iVar3 + 5);
          QPainterPath::lineTo(this,(QPointF *)&local_88);
        }
      }
      QPainter::setPen(param_2,pQVar2);
      QBrush::QBrush(local_res20,pQVar2,1);
      QPainter::setBrush(param_2,local_res20);
      QBrush::~QBrush(local_res20);
      QPainter::drawPath(param_2,this);
      pQVar4 = QPainter::brush(param_2);
      QPainter::fillPath(param_2,this,pQVar4);
      QPainter::setRenderHint(param_2,1,false);
      if (this != (QPainterPath *)0x0) {
        QPainterPath::~QPainterPath(this);
        free(this);
      }
      plVar6 = (longlong *)plVar6[1];
    } while (plVar6 != (longlong *)0x0);
  }
  return;
}


/* ================= FUN_14004b290 @ 14004b290  size=437  callers=1 callees=7 ================= */
/* CALLERS: FUN_14004c1b0 */
/* CALLEES: QBrush, __security_check_cookie, drawLines, fillRect, setPen, setRgb, ~QBrush */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14004b290(longlong param_1,QPainter *param_2)

{
  longlong lVar1;
  QBrush *pQVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined1 auStackY_98 [32];
  undefined8 local_68;
  undefined8 uStack_60;
  double local_58;
  double local_50;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  ulonglong local_38;
  
  local_38 = DAT_140096fe8 ^ (ulonglong)auStackY_98;
  lVar1 = *(longlong *)(param_1 + 0x20);
  iVar7 = (*(int *)(lVar1 + 0x20) - *(int *)(lVar1 + 0x18)) + 1;
  iVar6 = (*(int *)(lVar1 + 0x1c) - *(int *)(lVar1 + 0x14)) + 1;
  QColor::setRgb((QColor *)&local_48,0,0,0,0xff);
  QPainter::setPen(param_2,(QColor *)&local_48);
  pQVar2 = (QBrush *)QBrush::QBrush((QBrush *)&local_48,1);
  local_68 = 0;
  uStack_60 = 0;
  local_58 = (double)iVar6;
  local_50 = (double)iVar7;
  QPainter::fillRect(param_2,(QRectF *)&local_68,pQVar2);
  QBrush::~QBrush((QBrush *)&local_48);
  QColor::setRgb((QColor *)&local_48,0xff,0xff,0xff,0x3c);
  QPainter::setPen(param_2,(QColor *)&local_48);
  iVar5 = 0;
  iVar3 = 0;
  if (0 < *(int *)(param_1 + 0x68) + 1) {
    iVar4 = 0;
    do {
      local_48 = *(int *)(param_1 + 0x74) + iVar4;
      local_44 = *(int *)(param_1 + 0x78);
      local_3c = iVar7 - local_44;
      local_40 = local_48;
      QPainter::drawLines(param_2,(QLine *)&local_48,1);
      iVar3 = iVar3 + 1;
      iVar4 = iVar4 + 100;
    } while (iVar3 < *(int *)(param_1 + 0x68) + 1);
  }
  if (0 < *(int *)(param_1 + 100) + 1) {
    iVar3 = 0;
    do {
      local_48 = *(int *)(param_1 + 0x74);
      local_44 = *(int *)(param_1 + 0x78) + iVar3;
      local_40 = iVar6 - local_48;
      local_3c = local_44;
      QPainter::drawLines(param_2,(QLine *)&local_48,1);
      iVar5 = iVar5 + 1;
      iVar3 = iVar3 + 100;
    } while (iVar5 < *(int *)(param_1 + 100) + 1);
  }
  return;
}


/* ================= FUN_14004b450 @ 14004b450  size=486  callers=1 callees=12 ================= */
/* CALLERS: FUN_14004c1b0 */
/* CALLEES: FUN_140049800, QBrush, QPainterPath, drawLines, drawPath, fillPath, lineTo, moveTo, setPen, setRenderHint, ~QBrush, ~QPainterPath */

void FUN_14004b450(longlong param_1,QPainter *param_2,longlong param_3)

{
  double dVar1;
  double dVar2;
  QPainterPath local_res18 [8];
  QBrush local_res20 [8];
  undefined8 local_38;
  undefined8 local_30;
  
  QPainterPath::QPainterPath(local_res18);
  if (*(char *)(param_3 + 0x20) != '\0') {
    dVar1 = *(double *)(param_1 + 0x40);
    dVar2 = (double)FUN_140049800(param_1);
    if ((dVar1 <= (double)*(uint *)(param_3 + 0x18)) &&
       ((double)*(uint *)(param_3 + 0x18) <= dVar2 + dVar1)) {
      QPainter::setPen(param_2,(QColor *)(param_3 + 4));
      local_38 = (double)CONCAT44(*(int *)(param_1 + 0x78),*(undefined4 *)(param_3 + 0x14));
      local_30 = (double)CONCAT44(((*(int *)(*(longlong *)(param_1 + 0x20) + 0x20) -
                                   *(int *)(*(longlong *)(param_1 + 0x20) + 0x18)) -
                                  *(int *)(param_1 + 0x78)) + 1,*(undefined4 *)(param_3 + 0x14));
      QPainter::drawLines(param_2,(QLine *)&local_38,1);
      QPainter::setRenderHint(param_2,1,true);
      local_30 = (double)*(int *)(param_1 + 0x78);
      local_38 = (double)(*(int *)(param_3 + 0x14) + -5);
      QPainterPath::moveTo(local_res18,(QPointF *)&local_38);
      local_30 = (double)*(int *)(param_1 + 0x78);
      local_38 = (double)(*(int *)(param_3 + 0x14) + 5);
      QPainterPath::lineTo(local_res18,(QPointF *)&local_38);
      local_30 = (double)(*(int *)(param_1 + 0x78) + 5);
      local_38 = (double)*(int *)(param_3 + 0x14);
      QPainterPath::lineTo(local_res18,(QPointF *)&local_38);
      local_30 = (double)*(int *)(param_1 + 0x78);
      local_38 = (double)(*(int *)(param_3 + 0x14) + -5);
      QPainterPath::lineTo(local_res18,(QPointF *)&local_38);
      QPainter::drawPath(param_2,local_res18);
      QBrush::QBrush(local_res20,(QColor *)(param_3 + 4),1);
      QPainter::fillPath(param_2,local_res18,local_res20);
      QBrush::~QBrush(local_res20);
      QPainter::setRenderHint(param_2,1,false);
    }
  }
  QPainterPath::~QPainterPath(local_res18);
  return;
}


/* ================= FUN_14004b640 @ 14004b640  size=496  callers=1 callees=12 ================= */
/* CALLERS: FUN_14004ad90 */
/* CALLEES: FUN_14003ece0, FUN_14003ed70, FUN_140049e90, FUN_14004a0f0, FUN_14004b830, QColor, QPen, __security_check_cookie, setColor, setPen, setWidth, ~QPen */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14004b640(longlong param_1,QPainter *param_2,longlong param_3,uint param_4,uint param_5)

{
  undefined8 uVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  undefined8 uVar5;
  code *pcVar6;
  undefined1 auStack_b8 [32];
  undefined4 local_98;
  int local_90;
  char local_88;
  undefined8 local_80;
  QPen local_78 [8];
  undefined8 local_70;
  QPainter *local_68;
  QColor local_60 [16];
  ulonglong local_50;
  
  local_50 = DAT_140096fe8 ^ (ulonglong)auStack_b8;
  local_68 = param_2;
  QColor::QColor(local_60,1);
  QPen::QPen(local_78,local_60);
  if (*(char *)(param_3 + 0x35c) == '\0') {
    FUN_14004b830(param_1,param_3);
  }
  iVar3 = 4;
  if (*(char *)(param_3 + 0x380) != '\x01') {
    iVar3 = 2;
  }
  QPen::setWidth(local_78,iVar3);
  QPen::setColor(local_78,*(QColor **)(param_3 + 0x378));
  QPainter::setPen(param_2,local_78);
  iVar3 = *(int *)(param_3 + 0x348);
  if (iVar3 == 0) {
    pcVar6 = FUN_140049dd0;
    goto LAB_14004b71f;
  }
  if (iVar3 != 1) {
    if (iVar3 == 2) {
      pcVar6 = FUN_140049e10;
      goto LAB_14004b71f;
    }
    if (iVar3 == 3) {
      pcVar6 = FUN_140049ec0;
      goto LAB_14004b71f;
    }
  }
  pcVar6 = FUN_140049e90;
LAB_14004b71f:
  local_70 = 0;
  local_80 = 0;
  iVar3 = (((*(int *)(*(longlong *)(param_1 + 0x20) + 0x1c) -
            *(int *)(*(longlong *)(param_1 + 0x20) + 0x14)) + 1) % 100) / 2;
  local_88 = '\x01';
  uVar1 = 0;
  cVar2 = local_88;
  do {
    local_88 = cVar2;
    if (param_5 <= param_4) {
      QPen::~QPen(local_78);
      return;
    }
    uVar5 = FUN_14003ece0(param_3,param_4);
    uVar4 = FUN_14003ed70(param_4);
    local_98 = (undefined4)(longlong)(double)uVar4;
    local_90 = iVar3;
    if (cVar2 == '\0') {
      local_70 = uVar1;
      FUN_14004a0f0(param_1,&local_80,param_3,uVar5);
      uVar5 = local_80;
      if ((int)uVar1 == (int)local_80) {
        if (local_70._4_4_ == local_80._4_4_) goto LAB_14004b7f8;
      }
      (*pcVar6)(local_68,uVar1,local_80);
    }
    else {
      FUN_14004a0f0(param_1,&local_80,param_3,uVar5);
      local_88 = '\0';
      uVar5 = local_80;
    }
LAB_14004b7f8:
    param_4 = param_4 + 1;
    uVar1 = uVar5;
    cVar2 = local_88;
  } while( true );
}


/* ================= FUN_14004b830 @ 14004b830  size=125  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004b640 */
/* CALLEES: FUN_14003ee40 */

undefined8 FUN_14004b830(longlong param_1,longlong param_2)

{
  ulonglong uVar1;
  double dVar2;
  
  dVar2 = *(double *)(param_2 + 0x398);
  if (dVar2 <= ABS(*(double *)(param_2 + 0x3a0))) {
    dVar2 = ABS(*(double *)(param_2 + 0x3a0));
  }
  uVar1 = (ulonglong)
          ((dVar2 + dVar2) /
          (double)(uint)(((*(int *)(*(longlong *)(param_1 + 0x20) + 0x20) -
                          *(int *)(*(longlong *)(param_1 + 0x20) + 0x18)) + 1) / 100));
  if (*(uint *)(param_2 + 0x358) < (uint)uVar1) {
    FUN_14003ee40(param_2,uVar1 & 0xffffffff);
    return 1;
  }
  return 0;
}


/* ================= FUN_14004b8b0 @ 14004b8b0  size=557  callers=2 callees=16 ================= */
/* CALLERS: FUN_14004bdc0, FUN_14004c1b0 */
/* CALLEES: FUN_14003ece0, FUN_140049710, FUN_14004a1b0, FUN_140058d94, QString, append, contains, hideText, isActiveWindow, mapFrom, mapFromGlobal, number, pos, rect, showText, ~QString */

void FUN_14004b8b0(QWidget *param_1)

{
  longlong lVar1;
  longlong *plVar2;
  bool bVar3;
  int iVar4;
  undefined8 *puVar5;
  QString *this;
  longlong lVar6;
  QString *extraout_RAX;
  QPoint *pQVar7;
  char *pcVar8;
  longlong lVar9;
  double dVar10;
  undefined8 local_res10;
  QString *local_res18;
  QPoint local_res20 [8];
  undefined8 local_58;
  undefined8 uStack_50;
  
  bVar3 = QWidget::isActiveWindow(DAT_14286d388);
  if ((bVar3) && (bVar3 = QWidget::isActiveWindow(*(QWidget **)(param_1 + 0xe0)), !bVar3)) {
    puVar5 = (undefined8 *)QWidget::rect(param_1);
    local_58 = *puVar5;
    uStack_50 = puVar5[1];
    QCursor::pos();
    puVar5 = (undefined8 *)QWidget::mapFromGlobal(param_1,local_res20);
    local_res10 = *puVar5;
    bVar3 = QRect::contains((QRect *)&local_58,(QPoint *)&local_res10,false);
    if (bVar3) {
      iVar4 = FUN_140049710(param_1,0);
      if (iVar4 == -1) {
        QToolTip::hideText();
        return;
      }
      local_res18 = (QString *)FUN_140058d94(8);
      if (local_res18 == (QString *)0x0) {
        this = (QString *)0x0;
        plVar2 = DAT_14286d3b8;
      }
      else {
        this = (QString *)QString::QString(local_res18);
        plVar2 = DAT_14286d3b8;
      }
      for (; plVar2 != (longlong *)0x0; plVar2 = (longlong *)plVar2[1]) {
        lVar1 = *plVar2;
        if (*(char *)(lVar1 + 0x388) != '\0') {
          QString::append(this,*(char **)(lVar1 + 0x108));
          QString::append(this,": ");
          lVar6 = FUN_14003ece0(lVar1,iVar4);
          if (lVar6 == 0) {
            pcVar8 = "N/A";
LAB_14004ba63:
            QString::append(this,pcVar8);
          }
          else {
            lVar9 = lVar1;
            dVar10 = (double)FUN_14004a1b0(*(undefined4 *)(lVar1 + 0x314),lVar6,lVar1);
            if (*(int *)(lVar1 + 0x314) == 5) {
              if (dVar10 == 0.0) {
                pcVar8 = "false";
              }
              else {
                pcVar8 = "true";
              }
              goto LAB_14004ba63;
            }
            QString::number(dVar10,(char)lVar6,(int)CONCAT71((int7)((ulonglong)lVar9 >> 8),0x67));
            QString::append(this,extraout_RAX);
            QString::~QString((QString *)&local_res18);
          }
          if (plVar2[1] != 0) {
            QString::append(this,"\n");
          }
        }
      }
      if (0 < *(int *)(*(longlong *)this + 8)) {
        QCursor::pos();
        pQVar7 = (QPoint *)QWidget::mapFrom(param_1,(QWidget *)local_res20,(QPoint *)param_1);
        QToolTip::showText(pQVar7,this,param_1);
      }
    }
  }
  return;
}


/* ================= FUN_14004bae0 @ 14004bae0  size=206  callers=4 callees=3 ================= */
/* CALLERS: FUN_14004bbb0, FUN_14004bbc0, FUN_14004bc10, FUN_14004bc20 */
/* CALLEES: FUN_140048030, FUN_140049830, FUN_14004e120 */

void FUN_14004bae0(longlong param_1,longlong param_2,char param_3)

{
  ulonglong uVar1;
  
  *(char *)(param_2 + 0x20) = param_3;
  if (param_3 != '\0') {
    uVar1 = FUN_140049830(param_1,*(undefined4 *)(param_1 + 0x11c));
    *(ulonglong *)(param_2 + 0x18) = uVar1;
    *(int *)(param_2 + 0x14) =
         (int)(((double)uVar1 - *(double *)(param_1 + 0x40)) / (*(double *)(param_1 + 0x80) / 100.0)
              + (double)((((*(int *)(*(longlong *)(param_1 + 0x20) + 0x1c) -
                           *(int *)(*(longlong *)(param_1 + 0x20) + 0x14)) + 1) % 100) / 2));
  }
  FUN_140048030(DAT_14286d3a8);
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_14004bbb0 @ 14004bbb0  size=15  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14004bae0 */

void FUN_14004bbb0(longlong param_1)

{
  FUN_14004bae0(param_1,param_1 + 0x130,0);
  return;
}


/* ================= FUN_14004bbc0 @ 14004bbc0  size=15  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14004bae0 */

void FUN_14004bbc0(longlong param_1)

{
  FUN_14004bae0(param_1,param_1 + 0x158,0);
  return;
}


/* ================= FUN_14004bbd0 @ 14004bbd0  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14004a410 */

void FUN_14004bbd0(longlong param_1)

{
  FUN_14004a410(param_1,param_1 + 0x130);
  return;
}


/* ================= FUN_14004bbe0 @ 14004bbe0  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14004a410 */

void FUN_14004bbe0(longlong param_1)

{
  FUN_14004a410(param_1,param_1 + 0x158);
  return;
}


/* ================= FUN_14004bbf0 @ 14004bbf0  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14004a4b0 */

void FUN_14004bbf0(longlong param_1)

{
  FUN_14004a4b0(param_1,param_1 + 0x130);
  return;
}


/* ================= FUN_14004bc00 @ 14004bc00  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14004a4b0 */

void FUN_14004bc00(longlong param_1)

{
  FUN_14004a4b0(param_1,param_1 + 0x158);
  return;
}


/* ================= FUN_14004bc10 @ 14004bc10  size=15  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14004bae0 */

void FUN_14004bc10(longlong param_1)

{
  FUN_14004bae0(param_1,param_1 + 0x130,1);
  return;
}


/* ================= FUN_14004bc20 @ 14004bc20  size=15  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14004bae0 */

void FUN_14004bc20(longlong param_1)

{
  FUN_14004bae0(param_1,param_1 + 0x158,1);
  return;
}


/* ================= FUN_14004bc30 @ 14004bc30  size=76  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003b8e0 */
/* CALLEES: FUN_14003ed60, FUN_140043970, FUN_140043990, update */

void FUN_14004bc30(QWidget *param_1)

{
  uint uVar1;
  ulonglong uVar2;
  undefined8 uVar3;
  
  uVar1 = FUN_14003ed60();
  uVar2 = FUN_140043970(*(undefined8 *)(param_1 + 0x48),1000000 % (ulonglong)uVar1);
  if (1000000 / (ulonglong)uVar1 <= uVar2) {
    uVar3 = FUN_140043990();
    *(undefined8 *)(param_1 + 0x48) = uVar3;
    QWidget::update(param_1);
  }
  return;
}


/* ================= FUN_14004bc80 @ 14004bc80  size=320  callers=1 callees=5 ================= */
/* CALLERS: FUN_14004c1b0 */
/* CALLEES: FUN_14003ece0, FUN_14003ed20, FUN_14003ed70, FUN_140049fe0, FUN_14004a2e0 */

void FUN_14004bc80(longlong param_1)

{
  longlong lVar1;
  longlong *plVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  
  plVar2 = DAT_14286d3b8;
  do {
    if (plVar2 == (longlong *)0x0) {
      return;
    }
    lVar1 = *plVar2;
    if ((((*(char *)(param_1 + 0x150) == '\0') || (*(char *)(param_1 + 0x178) == '\0')) ||
        (*(char *)(lVar1 + 0x35e) == '\0')) || (*(char *)(lVar1 + 0x388) == '\0')) {
LAB_14004bd8c:
      *(undefined1 *)(lVar1 + 0x3c4) = 0;
    }
    else {
      uVar3 = FUN_14003ed20();
      uVar4 = FUN_14003ed70(uVar3);
      if ((((ulonglong)uVar4 < *(ulonglong *)(param_1 + 0x148)) ||
          ((ulonglong)uVar4 < *(ulonglong *)(param_1 + 0x170))) ||
         (iVar5 = FUN_140049fe0(param_1,0,uVar3,*(ulonglong *)(param_1 + 0x148) & 0xffffffff),
         iVar5 < 0)) goto LAB_14004bd8c;
      uVar7 = FUN_14003ece0(lVar1);
      iVar6 = FUN_140049fe0(param_1,0,uVar3,*(undefined4 *)(param_1 + 0x170));
      if (iVar6 < 0) goto LAB_14004bd8c;
      uVar8 = FUN_14003ece0(lVar1,iVar6);
      uVar9 = uVar7;
      if (iVar6 < iVar5) {
        uVar9 = uVar8;
        uVar8 = uVar7;
      }
      FUN_14004a2e0(lVar1 + 0x3b8,uVar8,uVar9,*(undefined4 *)(lVar1 + 0x314));
    }
    plVar2 = (longlong *)plVar2[1];
  } while( true );
}


/* ================= FUN_14004bdc0 @ 14004bdc0  size=604  callers=0 callees=9 ================= */
/* CALLEES: FUN_140048100, FUN_140048320, FUN_140049830, FUN_140049c50, FUN_14004a2b0, FUN_14004b8b0, FUN_14004e120, FUN_14004ea40, FUN_140055470 */

void FUN_14004bdc0(longlong param_1,longlong param_2)

{
  longlong lVar1;
  undefined8 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  int iVar8;
  int iVar9;
  double dVar10;
  
  iVar3 = FUN_14004ea40();
  uVar2 = DAT_14286d3a8;
  uVar7 = DAT_14286d390;
  if (iVar3 == 0) {
    return;
  }
  iVar3 = *(int *)(param_2 + 0x20);
  iVar5 = *(int *)(param_2 + 0x24);
  if (*(char *)(param_1 + 0x151) != '\0') {
    *(undefined2 *)(param_1 + 0x29) = 0;
    *(undefined1 *)(param_1 + 0x128) = 0;
    uVar6 = FUN_140049830(param_1,*(undefined4 *)(param_1 + 0x11c));
    *(undefined8 *)(param_1 + 0x148) = uVar6;
  }
  if (*(char *)(param_1 + 0x179) == '\0') {
    if (*(char *)(param_1 + 0x29) != '\0') {
      lVar1 = *(longlong *)(param_1 + 0x30);
      iVar3 = *(int *)(param_1 + 0x38);
      iVar9 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x20) -
              *(int *)(*(longlong *)(param_1 + 0x20) + 0x18);
      iVar8 = (iVar9 - *(int *)(param_1 + 0x78)) + 1;
      iVar4 = FUN_14004a2b0(param_1,lVar1);
      if ((iVar4 < *(int *)(param_1 + 0x78)) && (*(int *)(param_1 + 0x78) < iVar5)) {
        dVar10 = (double)((iVar9 + 1) / 200);
      }
      else if ((iVar8 < iVar4) && (iVar5 < iVar8)) {
        iVar3 = (int)((ulonglong)((longlong)(iVar8 - *(int *)(param_1 + 0x6c)) * -0x51eb851f) >>
                     0x20);
        dVar10 = (double)((iVar3 >> 5) - (iVar3 >> 0x1f));
      }
      else {
        dVar10 = (double)(iVar3 - iVar5) / 100.0 + *(double *)(lVar1 + 0x350);
      }
      *(double *)(lVar1 + 0x350) = dVar10;
      *(int *)(param_1 + 0x38) = iVar5;
      goto LAB_14004bfa6;
    }
  }
  else {
    *(undefined2 *)(param_1 + 0x29) = 0;
    *(undefined1 *)(param_1 + 0x128) = 0;
    uVar6 = FUN_140049830(param_1,*(undefined4 *)(param_1 + 0x11c));
    *(undefined8 *)(param_1 + 0x170) = uVar6;
  }
  if (*(char *)(param_1 + 0x2a) == '\0') {
    *(int *)(param_1 + 0x11c) = iVar3;
  }
  else {
    FUN_140055470(uVar7,0);
    *(int *)(param_1 + 0x11c) = iVar3;
    dVar10 = (*(double *)(param_1 + 0x80) / 100.0) * (double)(*(int *)(param_1 + 0x3c) - iVar3) +
             *(double *)(param_1 + 0x40);
    *(double *)(param_1 + 0x40) = dVar10;
    if (dVar10 < 0.0) {
      *(undefined8 *)(param_1 + 0x40) = 0;
    }
    FUN_140049c50(param_1);
    dVar10 = *(double *)(param_1 + 0x40);
    if (dVar10 < 0.0) {
      *(undefined8 *)(param_1 + 0x40) = 0;
      dVar10 = 0.0;
    }
    FUN_140048320(uVar2,(int)dVar10);
    *(int *)(param_1 + 0x3c) = iVar3;
  }
LAB_14004bfa6:
  iVar3 = *(int *)(param_1 + 0x11c);
  iVar5 = *(int *)(param_1 + 0x74);
  if (iVar3 < iVar5) {
    *(int *)(param_1 + 0x11c) = iVar5;
    iVar3 = iVar5;
  }
  else {
    iVar5 = ((*(int *)(*(longlong *)(param_1 + 0x20) + 0x1c) - iVar5) -
            *(int *)(*(longlong *)(param_1 + 0x20) + 0x14)) + 1;
    if (iVar5 < iVar3) {
      *(int *)(param_1 + 0x11c) = iVar5;
      iVar3 = iVar5;
    }
  }
  uVar7 = FUN_140049830(param_1,iVar3);
  *(undefined8 *)(param_1 + 0x120) = uVar7;
  FUN_14004b8b0(param_1);
  FUN_140048100(uVar2);
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_14004c020 @ 14004c020  size=344  callers=0 callees=4 ================= */
/* CALLEES: FUN_14003ee00, FUN_14004a2b0, FUN_14004a560, FUN_14004e120 */

void FUN_14004c020(longlong param_1,longlong param_2)

{
  int iVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 *puVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  
  FUN_14003ee00(0);
  iVar1 = *(int *)(param_2 + 0x20);
  iVar2 = *(int *)(param_2 + 0x24);
  if (*(int *)(param_2 + 0x30) == 2) {
    FUN_14004a560(param_1,*(undefined8 *)(param_2 + 0x20));
    return;
  }
  iVar5 = 5;
  iVar11 = 5;
  for (puVar4 = DAT_14286d3b8; puVar4 != (undefined8 *)0x0; puVar4 = (undefined8 *)puVar4[1]) {
    uVar3 = *puVar4;
    iVar6 = FUN_14004a2b0(param_1,uVar3);
    iVar12 = iVar5;
    if (iVar6 < *(int *)(param_1 + 0x78)) {
      iVar6 = 0;
      iVar10 = iVar11 + 10;
    }
    else {
      iVar8 = *(int *)(*(longlong *)(param_1 + 0x20) + 0x20) -
              *(int *)(*(longlong *)(param_1 + 0x20) + 0x18);
      iVar10 = iVar11;
      if ((iVar8 - *(int *)(param_1 + 0x78)) + 1 < iVar6) {
        iVar6 = iVar8 + 1;
        iVar12 = iVar5 + 10;
        iVar11 = iVar5;
      }
      else {
        iVar11 = 0;
      }
    }
    uVar7 = iVar6 - iVar2;
    uVar9 = (int)uVar7 >> 0x1f;
    if (((int)((uVar7 ^ uVar9) - uVar9) < 6) &&
       (uVar7 = iVar11 - iVar1 >> 0x1f, (int)((iVar11 - iVar1 ^ uVar7) - uVar7) < 6)) {
      *(undefined1 *)(param_1 + 0x29) = 1;
      *(undefined8 *)(param_1 + 0x30) = uVar3;
      FUN_14003ee00(uVar3);
    }
    iVar5 = iVar12;
    iVar11 = iVar10;
  }
  if ((iVar1 + -10 < *(int *)(param_1 + 0x16c)) && (*(int *)(param_1 + 0x16c) < iVar1 + 10)) {
    *(undefined1 *)(param_1 + 0x179) = 1;
  }
  else if ((iVar1 + -10 < *(int *)(param_1 + 0x144)) && (*(int *)(param_1 + 0x144) < iVar1 + 10)) {
    *(undefined1 *)(param_1 + 0x151) = 1;
  }
  *(undefined1 *)(param_1 + 0x2a) = 1;
  *(int *)(param_1 + 0x3c) = iVar1;
  *(int *)(param_1 + 0x38) = iVar2;
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_14004c180 @ 14004c180  size=39  callers=0 callees=1 ================= */
/* CALLEES: FUN_14004e120 */

void FUN_14004c180(longlong param_1)

{
  *(undefined2 *)(param_1 + 0x29) = 0;
  *(undefined1 *)(param_1 + 0x151) = 0;
  *(undefined1 *)(param_1 + 0x179) = 0;
  *(undefined1 *)(param_1 + 0x128) = 1;
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_14004c1b0 @ 14004c1b0  size=692  callers=0 callees=13 ================= */
/* CALLEES: FUN_14003f3f0, FUN_140043970, FUN_140043990, FUN_140049830, FUN_14004a870, FUN_14004ad90, FUN_14004af10, FUN_14004b290, FUN_14004b450, FUN_14004b8b0, FUN_14004bc80, QPainter, ~QPainter */

void FUN_14004c1b0(longlong param_1)

{
  longlong lVar1;
  undefined1 auVar2 [16];
  undefined1 auVar3 [16];
  int iVar4;
  int iVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  ulonglong uVar8;
  QPaintDevice *pQVar9;
  int iVar10;
  int iVar11;
  double dVar12;
  QPainter local_res8 [8];
  
  uVar6 = FUN_140043990();
  pQVar9 = (QPaintDevice *)(param_1 + 0x10);
  if (param_1 == 0) {
    pQVar9 = (QPaintDevice *)0x0;
  }
  QPainter::QPainter(local_res8,pQVar9);
  lVar1 = *(longlong *)(param_1 + 0x20);
  iVar11 = (*(int *)(lVar1 + 0x1c) - *(int *)(lVar1 + 0x14)) + 1;
  iVar10 = (*(int *)(lVar1 + 0x20) - *(int *)(lVar1 + 0x18)) + 1;
  iVar4 = iVar10 / 100;
  *(int *)(param_1 + 100) = iVar4;
  iVar5 = iVar11 / 100;
  *(int *)(param_1 + 0x68) = iVar5;
  iVar11 = (iVar11 % 100) / 2;
  *(int *)(param_1 + 0x74) = iVar11;
  iVar10 = (iVar10 % 100) / 2;
  *(int *)(param_1 + 0x78) = iVar10;
  *(int *)(param_1 + 0x6c) = (iVar4 / 2) * 100 + iVar10;
  *(int *)(param_1 + 0x70) = (iVar5 / 2) * 100 + iVar11;
  dVar12 = *(double *)(param_1 + 0x80) / 100.0;
  *(int *)(param_1 + 0x144) =
       (int)((double)((((*(int *)(lVar1 + 0x1c) - *(int *)(lVar1 + 0x14)) + 1) % 100) / 2) +
            ((double)*(ulonglong *)(param_1 + 0x148) - *(double *)(param_1 + 0x40)) / dVar12);
  *(int *)(param_1 + 0x16c) =
       (int)((double)((((*(int *)(lVar1 + 0x1c) - *(int *)(lVar1 + 0x14)) + 1) % 100) / 2) +
            ((double)*(ulonglong *)(param_1 + 0x170) - *(double *)(param_1 + 0x40)) / dVar12);
  FUN_14004b290(param_1,local_res8);
  FUN_14004a870(param_1,local_res8);
  FUN_14004bc80(param_1);
  uVar7 = FUN_140043990();
  FUN_14004ad90(param_1,local_res8);
  FUN_140043970(uVar7);
  uVar7 = FUN_140049830(param_1,*(undefined4 *)(param_1 + 0x11c));
  *(undefined8 *)(param_1 + 0x120) = uVar7;
  FUN_14004b450(param_1,local_res8,param_1 + 0x108);
  FUN_14004b450(param_1,local_res8,param_1 + 0x130);
  FUN_14004b450(param_1,local_res8,param_1 + 0x158);
  FUN_14004af10(param_1,local_res8);
  uVar8 = FUN_140043970(uVar6);
  if (uVar8 != 0) {
    auVar2._8_8_ = 0;
    auVar2._0_8_ = uVar8;
    auVar3 = ZEXT816(0) << 0x40 | ZEXT816(1000000);
    FUN_14003f3f0(SUB164(auVar3 / auVar2,0),SUB168(auVar3 % auVar2,0));
  }
  FUN_14004b8b0(param_1);
  QPainter::~QPainter(local_res8);
  return;
}


/* ================= FUN_14004c470 @ 14004c470  size=97  callers=0 callees=6 ================= */
/* CALLEES: FUN_140048100, FUN_140049830, mapFromGlobal, pos, thunk_FUN_140055fc0, thunk_FUN_140056000 */

void FUN_14004c470(QWidget *param_1,longlong param_2)

{
  undefined4 *puVar1;
  undefined8 uVar2;
  QPoint local_res10 [24];
  
  if (*(int *)(param_2 + 0x30) < 1) {
    thunk_FUN_140056000(DAT_14286d390);
  }
  else {
    thunk_FUN_140055fc0();
  }
  FUN_140048100(DAT_14286d3a8);
  QCursor::pos();
  puVar1 = (undefined4 *)QWidget::mapFromGlobal(param_1,local_res10);
  uVar2 = FUN_140049830(param_1,*puVar1);
  *(undefined8 *)(param_1 + 0x120) = uVar2;
  return;
}


/* ================= FUN_14004c4e0 @ 14004c4e0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_14004c520 */
/* CALLEES: free, ~QApplication */

QApplication * FUN_14004c4e0(QApplication *param_1,uint param_2)

{
  QApplication::~QApplication(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14004c520 @ 14004c520  size=261  callers=1 callees=8 ================= */
/* CALLERS: FUN_14004c670 */
/* CALLEES: FUN_140036d80, FUN_14004c4e0, FUN_14004c6a0, FUN_140058d94, QApplication, __acrt_iob_func, exec, setvbuf */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_14004c520(int param_1,char **param_2)

{
  FILE *_File;
  QApplication *this;
  longlong lVar1;
  QApplication *pQVar2;
  QApplication *pQVar3;
  int local_res8 [2];
  
  local_res8[0] = param_1;
  _File = (FILE *)__acrt_iob_func(1);
  setvbuf(_File,(char *)0x0,4,0);
  this = (QApplication *)FUN_140058d94(0x10);
  pQVar2 = (QApplication *)0x0;
  pQVar3 = pQVar2;
  if (this != (QApplication *)0x0) {
    QApplication::QApplication(this,local_res8,param_2,0x1040807);
    *(undefined ***)this = QApplication::vftable;
    pQVar3 = this;
  }
  FUN_140036d80();
  DAT_14286d3b8 = 0;
  DAT_14286d3b0 = 0;
  DAT_14286d3a0 = 0;
  _DAT_14286d398 = 0;
  DAT_14286d388 = 0;
  DAT_14286d380 = 0;
  lVar1 = FUN_140058d94(0x348);
  if (lVar1 != 0) {
    pQVar2 = (QApplication *)FUN_14004c6a0(lVar1,local_res8[0],param_2);
  }
  QApplication::exec();
  if (pQVar2 != (QApplication *)0x0) {
    (**(code **)(*(longlong *)pQVar2 + 0x18))(pQVar2,1);
  }
  if (pQVar3 != (QApplication *)0x0) {
    (**(code **)(*(longlong *)pQVar3 + 0x18))(pQVar3,1);
  }
  return 0;
}


/* ================= FUN_14004c670 @ 14004c670  size=36  callers=1 callees=3 ================= */
/* CALLERS: FUN_140059454 */
/* CALLEES: FUN_14004c520, __p___argc, __p___argv */

void FUN_14004c670(void)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  undefined4 *puVar3;
  
  puVar2 = (undefined8 *)__p___argv();
  uVar1 = *puVar2;
  puVar3 = (undefined4 *)__p___argc();
  FUN_14004c520(*puVar3,uVar1);
  return;
}


/* ================= FUN_14004c6a0 @ 14004c6a0  size=1864  callers=1 callees=41 ================= */
/* CALLERS: FUN_14004c520 */
/* CALLEES: FUN_140029ad0, FUN_14002a340, FUN_14003e2e0, FUN_14003ea80, FUN_140043110, FUN_140043950, FUN_1400479a0, FUN_14004fbe0, FUN_140051790, FUN_140054790, FUN_1400561b0, FUN_140058d94, QBoxLayout, QDesktopWidget, QMainWindow, QSplitter, QTimer, QWidget, addToolBar, addWidget, connect, fromAscii_helper, memset, move, screenGeometry, screenNumber, setCentralWidget, setContentsMargins, setGeometry, setInterval, setLayout, setMenuBar, setMinimumSize, setMovable, setOrientation, setWindowTitle, show, sizeHint, start, ~QDesktopWidget, ~QString */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

QWidget * FUN_14004c6a0(QWidget *param_1,undefined4 param_2,undefined8 param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char *pcVar6;
  undefined8 uVar7;
  QBoxLayout *this;
  QWidget *pQVar8;
  QMenuBar *pQVar9;
  QToolBar *this_00;
  QSplitter *this_01;
  QWidget *pQVar10;
  int *piVar11;
  longlong lVar12;
  QTimer *this_02;
  QBoxLayout *this_03;
  QSplitter *this_04;
  QTimer *this_05;
  undefined4 local_res10 [2];
  undefined8 local_res18;
  undefined8 local_78;
  undefined8 uStack_70;
  QDesktopWidget local_68 [48];
  
  this_05 = (QTimer *)0x0;
  local_res10[0] = 0;
  QMainWindow::QMainWindow((QMainWindow *)param_1,0,local_res10);
  *(undefined ***)param_1 = MainWindow::vftable;
  *(undefined ***)(param_1 + 0x10) = MainWindow::vftable;
  QDesktopWidget::QDesktopWidget(local_68);
  DAT_14286d388 = (QObject *)param_1;
  pcVar6 = (char *)FUN_14003ea80();
  local_res18 = (QTimer *)QString::fromAscii_helper(pcVar6,-1);
  QWidget::setWindowTitle(param_1,(QString *)&local_res18);
  QString::~QString((QString *)&local_res18);
  *(undefined8 *)(param_1 + 0x2d8) = 0;
  *(undefined8 *)(param_1 + 0x2e0) = 0;
  *(undefined8 *)(param_1 + 0x2e8) = 0;
  *(undefined8 *)(param_1 + 0x2f0) = 0;
  *(undefined8 *)(param_1 + 0x2f8) = 0;
  *(undefined8 *)(param_1 + 0x300) = 0;
  *(undefined8 *)(param_1 + 0x308) = 0;
  *(undefined8 *)(param_1 + 0x310) = 0;
  *(undefined8 *)(param_1 + 0x318) = 0;
  *(undefined8 *)(param_1 + 800) = 0;
  *(undefined8 *)(param_1 + 0x328) = 0;
  *(undefined8 *)(param_1 + 0x330) = 0;
  *(undefined8 *)(param_1 + 0x338) = 0;
  *(undefined8 *)(param_1 + 0x340) = 0;
  memset(param_1 + 0x28,0,0x2b0);
  uVar7 = FUN_140043950();
  *(undefined8 *)(param_1 + 0x1b0) = uVar7;
  uVar7 = FUN_140043110(0);
  *(undefined8 *)(param_1 + 0x318) = uVar7;
  this = (QBoxLayout *)FUN_140058d94(0x20);
  this_03 = (QBoxLayout *)this_05;
  local_res18 = (QTimer *)this;
  if (this != (QBoxLayout *)0x0) {
    QBoxLayout::QBoxLayout(this,2,(QWidget *)0x0);
    *(undefined ***)this = QBoxLayout::vftable;
    *(undefined ***)(this + 0x10) = QBoxLayout::vftable;
    this_03 = this;
  }
  *(QBoxLayout **)(param_1 + 0x308) = this_03;
  QLayout::setContentsMargins((QLayout *)this_03,0,0,0,0);
  pQVar8 = (QWidget *)FUN_140058d94(0x28);
  pQVar10 = (QWidget *)this_05;
  local_res18 = (QTimer *)pQVar8;
  if (pQVar8 != (QWidget *)0x0) {
    local_res10[0] = 0;
    QWidget::QWidget(pQVar8,param_1,local_res10);
    *(undefined ***)pQVar8 = QWidget::vftable;
    *(undefined ***)(pQVar8 + 0x10) = QWidget::vftable;
    pQVar10 = pQVar8;
  }
  *(QWidget **)(param_1 + 0x310) = pQVar10;
  QWidget::setLayout(pQVar10,*(QLayout **)(param_1 + 0x308));
  QMainWindow::setCentralWidget((QMainWindow *)param_1,*(QWidget **)(param_1 + 0x310));
  local_78 = 0;
  uStack_70 = 0x3dd000003e7;
  QWidget::setGeometry(param_1,(QRect *)&local_78);
  QWidget::setMinimumSize(param_1,800,800);
  FUN_14002a340();
  local_res18 = (QTimer *)FUN_140058d94(0x30);
  pQVar9 = (QMenuBar *)this_05;
  if (local_res18 != (QTimer *)0x0) {
    pQVar9 = (QMenuBar *)FUN_140029ad0(local_res18,param_1);
  }
  *(QMenuBar **)(param_1 + 0x2e0) = pQVar9;
  QMainWindow::setMenuBar((QMainWindow *)param_1,pQVar9);
  local_res18 = (QTimer *)FUN_140058d94(0xc0);
  this_00 = (QToolBar *)this_05;
  if (local_res18 != (QTimer *)0x0) {
    this_00 = (QToolBar *)FUN_140054790(local_res18);
  }
  *(QToolBar **)(param_1 + 0x2e8) = this_00;
  QToolBar::setMovable(this_00,false);
  QMainWindow::addToolBar((QMainWindow *)param_1,*(QToolBar **)(param_1 + 0x2e8));
  DAT_14286d390 = *(undefined8 *)(param_1 + 0x2e8);
  local_res18 = (QTimer *)FUN_140058d94(0x98);
  pQVar9 = (QMenuBar *)this_05;
  if (local_res18 != (QTimer *)0x0) {
    pQVar9 = (QMenuBar *)FUN_1400561b0(local_res18,param_1);
  }
  *(QMenuBar **)(param_1 + 0x2f0) = pQVar9;
  local_res10[0] = 0;
  QBoxLayout::addWidget(*(QBoxLayout **)(param_1 + 0x308),pQVar9);
  this_01 = (QSplitter *)FUN_140058d94(0x28);
  this_04 = (QSplitter *)this_05;
  local_res18 = (QTimer *)this_01;
  if (this_01 != (QSplitter *)0x0) {
    QSplitter::QSplitter(this_01,param_1);
    *(undefined ***)this_01 = QSplitter::vftable;
    *(undefined ***)(this_01 + 0x10) = QSplitter::vftable;
    this_04 = this_01;
  }
  *(QSplitter **)(param_1 + 0x2d8) = this_04;
  QSplitter::setOrientation(this_04,2);
  QWidget::setContentsMargins(*(QWidget **)(param_1 + 0x2d8),0,0,0,0);
  local_res10[0] = 0;
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x308),*(undefined8 *)(param_1 + 0x2d8),0,local_res10);
  local_res18 = (QTimer *)FUN_140058d94(0x98);
  pQVar10 = (QWidget *)this_05;
  if (local_res18 != (QTimer *)0x0) {
    pQVar10 = (QWidget *)FUN_1400479a0(local_res18,param_1);
  }
  *(QWidget **)(param_1 + 0x2f8) = pQVar10;
  QSplitter::addWidget(*(QSplitter **)(param_1 + 0x2d8),pQVar10);
  local_res18 = (QTimer *)FUN_140058d94(0xb8);
  pQVar9 = (QMenuBar *)this_05;
  if (local_res18 != (QTimer *)0x0) {
    pQVar9 = (QMenuBar *)FUN_140051790(local_res18,param_1);
  }
  *(QMenuBar **)(param_1 + 0x300) = pQVar9;
  _DAT_14286d398 = pQVar9;
  QSplitter::addWidget(*(QSplitter **)(param_1 + 0x2d8),*(QWidget **)(param_1 + 0x300));
  local_res18 = (QTimer *)FUN_140058d94(0x48);
  pQVar9 = (QMenuBar *)this_05;
  if (local_res18 != (QTimer *)0x0) {
    pQVar9 = (QMenuBar *)FUN_14004fbe0(local_res18,param_1);
  }
  *(QMenuBar **)(param_1 + 800) = pQVar9;
  local_res10[0] = 0;
  QBoxLayout::addWidget(*(QBoxLayout **)(param_1 + 0x308),pQVar9,0,local_res10);
  iVar5 = QDesktopWidget::screenNumber(local_68,param_1);
  if (-1 < iVar5) {
    piVar11 = (int *)QDesktopWidget::screenGeometry(local_68,(int)&local_78);
    iVar1 = *piVar11;
    iVar2 = piVar11[1];
    iVar3 = piVar11[2];
    iVar4 = piVar11[3];
    piVar11 = (int *)QWidget::sizeHint(param_1);
    iVar5 = *piVar11;
    lVar12 = QWidget::sizeHint(param_1);
    local_res18 = (QTimer *)
                  CONCAT44(((iVar4 - iVar2) + 1) / 2 - *(int *)(lVar12 + 4) / 2,
                           ((iVar3 - iVar1) + 1) / 2 - iVar5 / 2);
    QWidget::move(param_1,(QPoint *)&local_res18);
  }
  this_02 = (QTimer *)FUN_140058d94(0x20);
  local_res18 = this_02;
  if (this_02 != (QTimer *)0x0) {
    QTimer::QTimer(this_02,(QObject *)param_1);
    *(undefined ***)this_02 = QTimer::vftable;
    this_05 = this_02;
  }
  *(QTimer **)(param_1 + 0x328) = this_05;
  QTimer::setInterval(this_05,1000);
  QObject::connect(*(QObject **)(param_1 + 0x328),"2timeout()",(QObject *)param_1,
                   "1_SlotOnTimeout()",0);
  QTimer::start(*(QTimer **)(param_1 + 0x328));
  QWidget::show(param_1);
  QObject::connect((QObject *)param_1,"2_SignalClose(void)",(QObject *)param_1,"1_SlotOnClose(void)"
                   ,2);
  QObject::connect((QObject *)param_1,"2_SignalNewProjectDialog(void)",(QObject *)param_1,
                   "1_SlotOnNewProjectDialog(void)",2);
  QObject::connect((QObject *)param_1,"2_SignalOptionsDialog(void)",(QObject *)param_1,
                   "1_SlotOptionsDialog(void)",2);
  QObject::connect((QObject *)param_1,"2_SignalShowSymbolSelection(void)",(QObject *)param_1,
                   "1_SlotOnShowSymbolSelection(void)",2);
  QObject::connect((QObject *)param_1,"2_SignalShowCustMsgBox(void)",(QObject *)param_1,
                   "1_SlotOnShowCustMsgBox(void)",2);
  QObject::connect((QObject *)param_1,"2_SignalUpdateProgBar(void)",(QObject *)param_1,
                   "1_SlotOnUpdateProgBar(void)",2);
  QObject::connect((QObject *)param_1,"2_SignalUpdateGUI()",(QObject *)param_1,"1_SlotOnUpdateGUI()"
                   ,0);
  QObject::connect((QObject *)param_1,"2_SignalUpdateData()",(QObject *)param_1,
                   "1_SlotOnUpdateData()",0);
  QObject::connect((QObject *)param_1,"2_SignalUpdateSymbolSelection()",(QObject *)param_1,
                   "1_SlotOnUpdateSymbolSelection()",0);
  QObject::connect((QObject *)param_1,"2_SignalGetSaveFileName()",(QObject *)param_1,
                   "1_SlotOnGetSaveFileName()",0);
  QObject::connect((QObject *)param_1,"2_SignalAssignColors()",(QObject *)param_1,
                   "1_SlotOnAssignColors()",0);
  QObject::connect((QObject *)param_1,"2_SignalReset()",(QObject *)param_1,"1_SlotOnReset()",0);
  QObject::connect((QObject *)param_1,"2_SignalFinAsyncAppInit(void)",(QObject *)param_1,
                   "1_SlotOnFinAsyncAppInit(void)",2);
  QObject::connect((QObject *)param_1,"2_SignalShowFileDialogOpen(void)",(QObject *)param_1,
                   "1_SlotOnShowFileDialogOpen(void)",2);
  QObject::connect((QObject *)param_1,"2_SignalShowFileDialogSave(void)",(QObject *)param_1,
                   "1_SlotOnShowFileDialogSave(void)",2);
  *(undefined4 *)(param_1 + 0x338) = param_2;
  *(undefined8 *)(param_1 + 0x330) = param_3;
  FUN_14003e2e0(param_1,param_1 + 0x330,&LAB_14004db30);
  QDesktopWidget::~QDesktopWidget(local_68);
  return param_1;
}


/* ================= FUN_14004cdf0 @ 14004cdf0  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_14004ce30 */

void FUN_14004cdf0(QMainWindow *param_1)

{
  *(undefined ***)param_1 = MainWindow::vftable;
  *(undefined ***)(param_1 + 0x10) = MainWindow::vftable;
                    /* WARNING: Could not recover jumptable at 0x00014004ce05. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMainWindow::~QMainWindow(param_1);
  return;
}


/* ================= FUN_14004ce0c @ 14004ce0c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14004ce30 */

void FUN_14004ce0c(longlong param_1)

{
  FUN_14004ce30(param_1 + -0x10);
  return;
}


/* ================= FUN_14004ce18 @ 14004ce18  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14004ce70 */

void FUN_14004ce18(longlong param_1)

{
  FUN_14004ce70(param_1 + -0x10);
  return;
}


/* ================= FUN_14004ce24 @ 14004ce24  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14004ceb0 */

void FUN_14004ce24(longlong param_1)

{
  FUN_14004ceb0(param_1 + -0x10);
  return;
}


/* ================= FUN_14004ce30 @ 14004ce30  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14004ce0c */
/* CALLEES: FUN_14004cdf0, free */

void * FUN_14004ce30(void *param_1,ulonglong param_2)

{
  FUN_14004cdf0();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14004ce70 @ 14004ce70  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_14004ce18 */
/* CALLEES: free, ~QBoxLayout */

QBoxLayout * FUN_14004ce70(QBoxLayout *param_1,uint param_2)

{
  QBoxLayout::~QBoxLayout(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14004ceb0 @ 14004ceb0  size=53  callers=2 callees=2 ================= */
/* CALLERS: FUN_14004ce24, FUN_14004d170 */
/* CALLEES: free, ~QProgressDialog */

QProgressDialog * FUN_14004ceb0(QProgressDialog *param_1,uint param_2)

{
  QProgressDialog::~QProgressDialog(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14004cf00 @ 14004cf00  size=38  callers=1 callees=2 ================= */
/* CALLERS: FUN_140056130 */
/* CALLEES: FUN_140056950, FUN_140056da0 */

void FUN_14004cf00(longlong param_1)

{
  FUN_140056950(*(undefined8 *)(param_1 + 0x2f0));
  FUN_140056da0(*(undefined8 *)(param_1 + 0x2f0));
  return;
}


/* ================= FUN_14004cf30 @ 14004cf30  size=12  callers=2 callees=1 ================= */
/* CALLERS: FUN_140055f90, FUN_140056130 */
/* CALLEES: FUN_140056a00 */

void FUN_14004cf30(longlong param_1)

{
  FUN_140056a00(*(undefined8 *)(param_1 + 0x2f0));
  return;
}


/* ================= FUN_14004cf50 @ 14004cf50  size=14  callers=1 callees=0 ================= */
/* CALLERS: FUN_14002a160 */

void FUN_14004cf50(longlong param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014004cf5a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(**(longlong **)(param_1 + 0x2f8) + 0x58))();
  return;
}


/* ================= FUN_14004cf60 @ 14004cf60  size=181  callers=1 callees=6 ================= */
/* CALLERS: FUN_14002a190 */
/* CALLEES: FUN_140036720, FUN_14003e210, FUN_14004f820, FUN_14004f9b0, __security_check_cookie, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14004cf60(void)

{
  int iVar1;
  undefined1 auStack_278 [32];
  undefined1 *local_258;
  char *local_250;
  undefined8 local_248;
  undefined1 local_238 [272];
  undefined1 local_128 [272];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_278;
  memset(local_238,0,0x104);
  FUN_14004f820(local_238,0x104);
  local_248 = 0;
  local_250 = "JRAW (*.jraw)";
  local_258 = local_238;
  iVar1 = FUN_140036720(DAT_14286d388,local_128,0x104,"Open JRAW file");
  FUN_14004f9b0(local_128);
  if (-1 < iVar1) {
    FUN_14003e210(local_128);
  }
  return;
}


/* ================= FUN_14004d020 @ 14004d020  size=17  callers=28 callees=1 ================= */
/* CALLERS: FUN_14004d290, FUN_14004d460, FUN_14004d540, FUN_14004d5a0, FUN_14004d5e0, FUN_14004d650, FUN_14004d6c0, FUN_14004d7e0, FUN_14004d830, FUN_14004d870, FUN_14004d900, FUN_14004d930, FUN_14004d990, FUN_14004d9c0, FUN_14004dc70, FUN_14004dcb0, FUN_14004dd70, FUN_14004de70, FUN_14004dea0, FUN_14004ded0, FUN_14004df20, FUN_14004df50, FUN_14004e0b0, FUN_14004e0f0, FUN_14004e120, FUN_14004e150, FUN_14004e1f0, FUN_14004e2c0 */
/* CALLEES: FUN_1400443e0 */

void FUN_14004d020(longlong param_1)

{
  FUN_1400443e0(*(undefined8 *)(param_1 + 0x318),0xffffffff);
  return;
}


/* ================= FUN_14004d0f0 @ 14004d0f0  size=71  callers=2 callees=2 ================= */
/* CALLERS: FUN_14004d5a0, FUN_14004df50 */
/* CALLEES: FUN_140034630, FUN_140058d94 */

void FUN_14004d0f0(undefined8 param_1,undefined8 param_2)

{
  longlong lVar1;
  QDialog *this;
  
  lVar1 = FUN_140058d94(0x60);
  this = (QDialog *)0x0;
  if (lVar1 != 0) {
    this = (QDialog *)FUN_140034630(lVar1,param_1,param_2);
  }
                    /* WARNING: Could not recover jumptable at 0x00014004d130. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::exec(this);
  return;
}


/* ================= FUN_14004d140 @ 14004d140  size=14  callers=1 callees=0 ================= */
/* CALLERS: FUN_14002a260 */

void FUN_14004d140(longlong param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014004d14a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(**(longlong **)(param_1 + 0x300) + 0x58))();
  return;
}


/* ================= FUN_14004d150 @ 14004d150  size=12  callers=28 callees=1 ================= */
/* CALLERS: FUN_14004d290, FUN_14004d460, FUN_14004d540, FUN_14004d5a0, FUN_14004d5e0, FUN_14004d650, FUN_14004d6c0, FUN_14004d7e0, FUN_14004d830, FUN_14004d870, FUN_14004d900, FUN_14004d930, FUN_14004d990, FUN_14004d9c0, FUN_14004dc70, FUN_14004dcb0, FUN_14004dd70, FUN_14004de70, FUN_14004dea0, FUN_14004ded0, FUN_14004df20, FUN_14004df50, FUN_14004e0b0, FUN_14004e0f0, FUN_14004e120, FUN_14004e150, FUN_14004e1f0, FUN_14004e2c0 */
/* CALLEES: FUN_140044370 */

void FUN_14004d150(longlong param_1)

{
  FUN_140044370(*(undefined8 *)(param_1 + 0x318));
  return;
}


/* ================= FUN_14004d160 @ 14004d160  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004d380 */
/* CALLEES: FUN_140029da0 */

void FUN_14004d160(longlong param_1)

{
  FUN_140029da0(*(undefined8 *)(param_1 + 0x2e0));
  return;
}


/* ================= FUN_14004d170 @ 14004d170  size=283  callers=2 callees=13 ================= */
/* CALLERS: FUN_14004d930, FUN_1400507f0 */
/* CALLEES: FUN_140036f30, FUN_14004ceb0, FUN_140058d94, QProgressDialog, fromAscii_helper, setCancelButton, setLabelText, setMaximum, setMinimum, setValue, setWindowModality, show, ~QString */

QProgressDialog *
FUN_14004d170(undefined8 param_1,QProgressDialog *param_2,char *param_3,int param_4,int param_5)

{
  undefined4 local_res10 [2];
  QProgressDialog *local_18 [2];
  
  if (param_2 == (QProgressDialog *)0x0) {
    param_2 = (QProgressDialog *)FUN_140058d94(0x28);
    local_18[0] = param_2;
    if (param_2 == (QProgressDialog *)0x0) {
      param_2 = (QProgressDialog *)0x0;
    }
    else {
      local_res10[0] = 0;
      QProgressDialog::QProgressDialog(param_2,param_1,local_res10);
      *(undefined ***)param_2 = QProgressDialog::vftable;
      *(undefined ***)(param_2 + 0x10) = QProgressDialog::vftable;
    }
    QProgressDialog::setMinimum(param_2,0);
    QProgressDialog::setMaximum(param_2,param_4);
    local_18[0] = (QProgressDialog *)QString::fromAscii_helper(param_3,-1);
    QProgressDialog::setLabelText(param_2,(QString *)local_18);
    QString::~QString((QString *)local_18);
    QWidget::setWindowModality((QWidget *)param_2,2);
    QProgressDialog::setCancelButton(param_2,(QPushButton *)0x0);
    FUN_140036f30(param_2);
  }
  if (param_5 < 0) {
    if (param_2 != (QProgressDialog *)0x0) {
      (**(code **)(*(longlong *)param_2 + 0x18))(param_2,1);
    }
  }
  else {
    QProgressDialog::setValue(param_2,param_5);
    QWidget::show((QWidget *)param_2);
  }
  return param_2;
}


/* ================= FUN_14004d290 @ 14004d290  size=238  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_14004d020, FUN_14004d150, FUN_140058d94 */

void FUN_14004d290(undefined8 param_1)

{
  undefined4 *puVar1;
  ulonglong uVar2;
  longlong *plVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  
  FUN_14004d020();
  if ((DAT_14286d3b8 != (longlong *)0x0) && (*DAT_14286d3b8 != 0)) {
    puVar4 = (undefined4 *)0x0;
    puVar1 = (undefined4 *)(ulonglong)DAT_14285ef30;
    plVar3 = DAT_14286d3b8;
    if (*(longlong *)(*DAT_14286d3b8 + 0x378) == 0) {
      puVar1 = puVar4;
    }
    do {
      if (5 < (byte)puVar1) {
        puVar1 = puVar4;
      }
      DAT_14285ef30 = (byte)puVar1;
      if (*(longlong *)(*plVar3 + 0x378) == 0) {
        puVar1 = (undefined4 *)FUN_140058d94(0x10);
        puVar5 = puVar4;
        if (puVar1 != (undefined4 *)0x0) {
          uVar2 = (ulonglong)DAT_14285ef30;
          *puVar1 = *(undefined4 *)(&DAT_14285eed0 + uVar2 * 0x10);
          *(undefined8 *)(puVar1 + 1) = *(undefined8 *)(&DAT_14285eed4 + uVar2 * 0x10);
          *(undefined2 *)(puVar1 + 3) = *(undefined2 *)(&DAT_14285eedc + uVar2 * 0x10);
          puVar5 = puVar1;
        }
        DAT_14285ef30 = DAT_14285ef30 + 1;
        puVar1 = (undefined4 *)(ulonglong)DAT_14285ef30;
        *(undefined4 **)(*plVar3 + 0x378) = puVar5;
      }
      plVar3 = (longlong *)plVar3[1];
    } while (plVar3 != (longlong *)0x0);
  }
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d380 @ 14004d380  size=213  callers=1 callees=5 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_14002a410, FUN_14004d160, FUN_140057220, FUN_140058d94, exec */

void FUN_14004d380(QWidget *param_1)

{
  int iVar1;
  longlong lVar2;
  QDialog *this;
  
  if (param_1[0x342] != (QWidget)0x0) {
                    /* WARNING: Could not recover jumptable at 0x00014004d3a0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    QWidget::close(param_1);
    return;
  }
  if (param_1[0x341] == (QWidget)0x0) {
    if (param_1[0x340] == (QWidget)0x0) goto LAB_14004d442;
    lVar2 = FUN_140058d94(0x148);
    if (lVar2 == 0) {
      this = (QDialog *)0x0;
    }
    else {
      this = (QDialog *)FUN_14002a410(lVar2,param_1,0);
    }
    QDialog::exec(this);
  }
  else {
    lVar2 = FUN_140058d94(0x68);
    if (lVar2 == 0) {
      this = (QDialog *)0x0;
    }
    else {
      this = (QDialog *)FUN_140057220(lVar2,param_1,param_1 + 0x230,10);
    }
    iVar1 = QDialog::exec(this);
    *(int *)(param_1 + 0x33c) = iVar1;
  }
  if (this != (QDialog *)0x0) {
    (**(code **)(*(longlong *)this + 0x18))(this,1);
  }
LAB_14004d442:
  FUN_14004d160(param_1);
  return;
}


/* ================= FUN_14004d460 @ 14004d460  size=115  callers=1 callees=5 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_1400368e0, FUN_1400443b0, FUN_14004d020, FUN_14004d150, FUN_14004fab0 */

void FUN_14004d460(longlong param_1)

{
  undefined4 uVar1;
  
  FUN_14004d020();
  uVar1 = FUN_1400368e0(param_1,*(undefined8 *)(param_1 + 0x218),*(undefined4 *)(param_1 + 0x228),
                        *(undefined8 *)(param_1 + 0x200),*(undefined8 *)(param_1 + 0x218),
                        *(undefined8 *)(param_1 + 0x208),*(undefined8 *)(param_1 + 0x210));
  *(undefined4 *)(param_1 + 0x22c) = uVar1;
  FUN_14004fab0(*(undefined8 *)(param_1 + 0x218));
  FUN_1400443b0(*(undefined8 *)(param_1 + 0x220));
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d4e0 @ 14004d4e0  size=90  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_14002a410, FUN_140058d94, exec */

void FUN_14004d4e0(undefined8 param_1)

{
  longlong lVar1;
  QDialog *this;
  
  lVar1 = FUN_140058d94(0x148);
  if (lVar1 == 0) {
    this = (QDialog *)0x0;
  }
  else {
    this = (QDialog *)FUN_14002a410(lVar1,param_1,0);
  }
  QDialog::exec(this);
  if (this != (QDialog *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x00014004d530. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*(longlong *)this + 0x18))(this,1);
    return;
  }
  return;
}


/* ================= FUN_14004d540 @ 14004d540  size=85  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_14004d020, FUN_14004d150, FUN_1400520f0, FUN_140056ab0 */

void FUN_14004d540(longlong param_1)

{
  FUN_14004d020();
  if (DAT_14286d380 != (longlong *)0x0) {
    (**(code **)(*DAT_14286d380 + 0x18))(DAT_14286d380,1);
  }
  DAT_14286d380 = (longlong *)0x0;
  FUN_140056ab0(*(undefined8 *)(param_1 + 0x2f0));
  FUN_1400520f0(*(undefined8 *)(param_1 + 0x300));
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d5a0 @ 14004d5a0  size=54  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_1400443b0, FUN_14004d020, FUN_14004d0f0, FUN_14004d150 */

void FUN_14004d5a0(longlong param_1)

{
  FUN_14004d020();
  FUN_14004d0f0(param_1,*(undefined8 *)(param_1 + 0x1d0));
  FUN_1400443b0(*(undefined8 *)(param_1 + 0x1d8));
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d5e0 @ 14004d5e0  size=103  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_140036720, FUN_1400443b0, FUN_14004d020, FUN_14004d150 */

void FUN_14004d5e0(longlong param_1)

{
  undefined4 uVar1;
  
  FUN_14004d020();
  uVar1 = FUN_140036720(param_1,*(undefined8 *)(param_1 + 0x218),*(undefined4 *)(param_1 + 0x228),
                        *(undefined8 *)(param_1 + 0x200),*(undefined8 *)(param_1 + 0x218),
                        *(undefined8 *)(param_1 + 0x208),*(undefined8 *)(param_1 + 0x210));
  *(undefined4 *)(param_1 + 0x22c) = uVar1;
  FUN_1400443b0(*(undefined8 *)(param_1 + 0x220));
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d650 @ 14004d650  size=103  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_1400368e0, FUN_1400443b0, FUN_14004d020, FUN_14004d150 */

void FUN_14004d650(longlong param_1)

{
  undefined4 uVar1;
  
  FUN_14004d020();
  uVar1 = FUN_1400368e0(param_1,*(undefined8 *)(param_1 + 0x218),*(undefined4 *)(param_1 + 0x228),
                        *(undefined8 *)(param_1 + 0x200),*(undefined8 *)(param_1 + 0x218),
                        *(undefined8 *)(param_1 + 0x208),*(undefined8 *)(param_1 + 0x210));
  *(undefined4 *)(param_1 + 0x22c) = uVar1;
  FUN_1400443b0(*(undefined8 *)(param_1 + 0x220));
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d6c0 @ 14004d6c0  size=274  callers=1 callees=11 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_14003e4e0, FUN_140043d20, FUN_140043e60, FUN_14004d020, FUN_14004d150, FUN_14004e9c0, FUN_140050140, FUN_140058d94, Sleep, exec, processEvents */

void FUN_14004d6c0(undefined8 param_1)

{
  longlong lVar1;
  QDialog *this;
  ulonglong local_res10;
  
  lVar1 = FUN_140043d20(0x5bc,"Project settings");
  FUN_14004e9c0(lVar1);
  if ((*(char *)(lVar1 + 0x5a4) == '\0') || (*(char *)(lVar1 + 0x5a6) != '\0')) {
    DAT_14286d379 = '\0';
    if (DAT_14286d3b0 == 0) {
      DAT_14286d379 = '\x01';
      FUN_14003e4e0(lVar1 + 0x80,0);
    }
    this = (QDialog *)0x0;
    while (DAT_14286d379 != '\0') {
      local_res10 = local_res10 & 0xffffffff00000000;
      QCoreApplication::processEvents(&local_res10);
      Sleep(10);
    }
    if (DAT_14286d3b0 != 0) {
      local_res10 = FUN_140058d94(0x60);
      if (local_res10 != 0) {
        this = (QDialog *)FUN_140050140(local_res10,param_1);
      }
      FUN_14004d020(param_1);
      QDialog::exec(this);
      if (this != (QDialog *)0x0) {
        (**(code **)(*(longlong *)this + 0x18))(this,1);
      }
      FUN_14004d150(param_1);
    }
  }
  FUN_140043e60(lVar1);
  return;
}


/* ================= FUN_14004d7e0 @ 14004d7e0  size=70  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_14003e0b0, FUN_14003ef10, FUN_14004d020, FUN_14004d150 */

void FUN_14004d7e0(undefined8 param_1)

{
  char cVar1;
  
  FUN_14004d020();
  cVar1 = FUN_14003ef10();
  if (((cVar1 == '\0') && (DAT_14286d379 == '\0')) && (DAT_14286d378 == '\0')) {
    DAT_14286d378 = '\x01';
    FUN_14003e0b0();
  }
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d830 @ 14004d830  size=51  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_1400482d0, FUN_14004d020, FUN_14004d150, FUN_140052110 */

void FUN_14004d830(longlong param_1)

{
  FUN_14004d020();
  FUN_1400482d0(*(undefined8 *)(param_1 + 0x2f8));
  FUN_140052110(*(undefined8 *)(param_1 + 0x300));
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d870 @ 14004d870  size=143  callers=1 callees=12 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_140029da0, FUN_14003ef10, FUN_1400482d0, FUN_14004d020, FUN_14004d150, FUN_14004ea40, FUN_14004fd10, FUN_140052110, FUN_140055bb0, FUN_140056950, FUN_140056a00, FUN_140056ad0 */

void FUN_14004d870(longlong param_1)

{
  char cVar1;
  int iVar2;
  
  FUN_14004d020();
  FUN_140029da0(*(undefined8 *)(param_1 + 0x2e0));
  FUN_140056ad0(*(undefined8 *)(param_1 + 0x2f0));
  iVar2 = FUN_14004ea40();
  if (iVar2 != 0) {
    cVar1 = FUN_14003ef10();
    if (cVar1 == '\0') {
      FUN_140056a00(*(undefined8 *)(param_1 + 0x2f0));
      goto LAB_14004d8c2;
    }
  }
  FUN_140056950(*(undefined8 *)(param_1 + 0x2f0));
LAB_14004d8c2:
  FUN_140055bb0(*(undefined8 *)(param_1 + 0x2e8));
  FUN_140052110(*(undefined8 *)(param_1 + 0x300));
  FUN_1400482d0(*(undefined8 *)(param_1 + 0x2f8));
  FUN_14004fd10(*(undefined8 *)(param_1 + 800));
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d900 @ 14004d900  size=39  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_1400482d0, FUN_14004d020, FUN_14004d150 */

void FUN_14004d900(longlong param_1)

{
  FUN_14004d020();
  FUN_1400482d0(*(undefined8 *)(param_1 + 0x2f8));
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d930 @ 14004d930  size=91  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_1400443b0, FUN_14004d020, FUN_14004d150, FUN_14004d170 */

void FUN_14004d930(longlong param_1)

{
  undefined8 uVar1;
  
  FUN_14004d020();
  uVar1 = FUN_14004d170(param_1,**(undefined8 **)(param_1 + 0x1e0),*(undefined8 *)(param_1 + 0x1e8),
                        *(undefined4 *)(param_1 + 0x1f0),*(undefined4 *)(param_1 + 500));
  **(undefined8 **)(param_1 + 0x1e0) = uVar1;
  FUN_1400443b0(*(undefined8 *)(param_1 + 0x1f8));
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d990 @ 14004d990  size=39  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_14004d020, FUN_14004d150, FUN_140052110 */

void FUN_14004d990(longlong param_1)

{
  FUN_14004d020();
  FUN_140052110(*(undefined8 *)(param_1 + 0x300));
  FUN_14004d150(param_1);
  return;
}


/* ================= FUN_14004d9c0 @ 14004d9c0  size=356  callers=1 callees=16 ================= */
/* CALLERS: FUN_14003ba80 */
/* CALLEES: FUN_140027760, FUN_14002a410, FUN_14003e4e0, FUN_14003ea40, FUN_14003fa20, FUN_140043d20, FUN_140043e60, FUN_14004d020, FUN_14004d150, FUN_14004e0b0, FUN_14004e9e0, FUN_140058d94, Sleep, exec, memset, processEvents */

void FUN_14004d9c0(undefined8 param_1)

{
  int iVar1;
  void *_Dst;
  QDialog *this;
  void *_Dst_00;
  ulonglong local_res10;
  
  _Dst = (void *)FUN_140043d20(0x5e8);
  memset(_Dst,0,0x5e8);
  FUN_14004e9e0(_Dst);
  local_res10 = FUN_140058d94(0x148);
  this = (QDialog *)0x0;
  if (local_res10 != 0) {
    this = (QDialog *)FUN_14002a410(local_res10,param_1,1);
  }
  FUN_14004d020(param_1);
  iVar1 = QDialog::exec(this);
  if (this != (QDialog *)0x0) {
    (**(code **)(*(longlong *)this + 0x18))(this,1);
  }
  FUN_14004d150(param_1);
  if (iVar1 == 1) {
    _Dst_00 = (void *)FUN_140043d20(0x5e8);
    memset(_Dst_00,0,0x5e8);
    FUN_14004e9e0(_Dst_00);
    iVar1 = FUN_140027760((longlong)_Dst + 0x88,(longlong)_Dst_00 + 0x88);
    if (iVar1 != 0) {
      FUN_14003ea40();
      DAT_14286d379 = '\x01';
      FUN_14003e4e0((longlong)_Dst_00 + 0x88,0);
      while (DAT_14286d379 != '\0') {
        local_res10 = local_res10 & 0xffffffff00000000;
        QCoreApplication::processEvents(&local_res10);
        Sleep(10);
      }
      FUN_14004e0b0(param_1);
    }
    FUN_140043e60(_Dst_00);
  }
  FUN_140043e60(_Dst);
  FUN_14003fa20();
  return;
}


/* ================= FUN_14004db40 @ 14004db40  size=301  callers=0 callees=9 ================= */
/* CALLEES: FUN_140039100, FUN_14003e1b0, FUN_140042ff0, FUN_140043000, FUN_1400430f0, FUN_1400443d0, Sleep, exit, processEvents */

void FUN_14004db40(longlong param_1,longlong param_2)

{
  int iVar1;
  undefined8 uVar2;
  undefined4 local_res18 [2];
  undefined8 local_28;
  undefined8 local_20;
  longlong local_18;
  undefined4 local_10;
  undefined4 local_c;
  
  DAT_14286d37c = 1;
  FUN_14003e1b0();
  do {
    while( true ) {
      if (DAT_14286d37c == 0) {
        *(ushort *)(param_2 + 0x12) = *(ushort *)(param_2 + 0x12) & 0xfffb;
        return;
      }
      if (DAT_14286d37c != 1) break;
      local_res18[0] = 0;
      QCoreApplication::processEvents(local_res18);
      Sleep(10);
    }
  } while (DAT_14286d37c != 2);
  local_28 = 0;
  local_c = 0;
  uVar2 = FUN_1400430f0(0,0);
  local_10 = 0xf;
  local_20 = uVar2;
  local_18 = param_1;
  FUN_140039100(&local_28);
  iVar1 = FUN_1400443d0(uVar2,10);
  while (iVar1 != 0) {
    local_res18[0] = 0;
    QCoreApplication::processEvents(local_res18);
    iVar1 = FUN_1400443d0(uVar2,10);
  }
  FUN_140042ff0(uVar2);
  FUN_140043000(param_1 + 0x318);
  *(ushort *)(param_2 + 0x12) = *(ushort *)(param_2 + 0x12) | 4;
  QCoreApplication::exit(0);
  return;
}


/* ================= FUN_14004dc70 @ 14004dc70  size=40  callers=2 callees=3 ================= */
/* CALLERS: FUN_140042100, FUN_140051590 */
/* CALLEES: FUN_14004d020, FUN_14004d150, thunk_FUN_14003bbe0 */

void FUN_14004dc70(longlong param_1)

{
  if (param_1 != 0) {
    FUN_14004d020();
    thunk_FUN_14003bbe0(param_1);
    FUN_14004d150(param_1);
  }
  return;
}


/* ================= FUN_14004dca0 @ 14004dca0  size=15  callers=2 callees=1 ================= */
/* CALLERS: FUN_14002a130, FUN_14003cc50 */
/* CALLEES: FUN_14003bc00 */

void FUN_14004dca0(longlong param_1)

{
  if (param_1 == 0) {
    return;
  }
  FUN_14003bc00();
  return;
}


/* ================= FUN_14004dcb0 @ 14004dcb0  size=186  callers=1 callees=6 ================= */
/* CALLERS: FUN_14004ea70 */
/* CALLEES: FUN_140042ff0, FUN_1400430f0, FUN_1400443d0, FUN_14004d020, FUN_14004d150, thunk_FUN_14003bce0 */

undefined4
FUN_14004dcb0(longlong param_1,longlong param_2,undefined4 param_3,undefined8 param_4,
             undefined8 param_5,undefined8 param_6)

{
  undefined4 uVar1;
  undefined8 uVar2;
  
  if ((param_1 == 0) || (param_2 == 0)) {
    uVar1 = 0xffffffff;
  }
  else {
    FUN_14004d020();
    uVar2 = FUN_1400430f0(0,0);
    *(undefined8 *)(param_1 + 0x220) = uVar2;
    *(undefined8 *)(param_1 + 0x208) = param_5;
    *(undefined8 *)(param_1 + 0x210) = param_6;
    *(longlong *)(param_1 + 0x218) = param_2;
    *(undefined4 *)(param_1 + 0x228) = param_3;
    *(undefined8 *)(param_1 + 0x200) = param_4;
    FUN_14004d150(param_1);
    thunk_FUN_14003bce0(param_1);
    FUN_1400443d0(*(undefined8 *)(param_1 + 0x220),0xffffffff);
    FUN_140042ff0(*(undefined8 *)(param_1 + 0x220));
    uVar1 = *(undefined4 *)(param_1 + 0x22c);
  }
  return uVar1;
}


/* ================= FUN_14004dd70 @ 14004dd70  size=232  callers=1 callees=6 ================= */
/* CALLERS: FUN_14004ebc0 */
/* CALLEES: FUN_140042ff0, FUN_1400430f0, FUN_1400443d0, FUN_14004d020, FUN_14004d150, thunk_FUN_14003bd00 */

undefined4
FUN_14004dd70(longlong param_1,longlong param_2,undefined4 param_3,longlong param_4,longlong param_5
             ,undefined8 param_6)

{
  undefined4 uVar1;
  undefined8 uVar2;
  
  if (((param_1 == 0) || (param_2 == 0)) || (param_4 == 0)) {
    uVar1 = 0xffffffff;
  }
  else if (param_5 == 0) {
    uVar1 = 0xffffffff;
  }
  else {
    FUN_14004d020();
    uVar2 = FUN_1400430f0(0,0);
    *(undefined8 *)(param_1 + 0x220) = uVar2;
    *(undefined8 *)(param_1 + 0x210) = param_6;
    *(longlong *)(param_1 + 0x218) = param_2;
    *(undefined4 *)(param_1 + 0x228) = param_3;
    *(longlong *)(param_1 + 0x200) = param_4;
    *(longlong *)(param_1 + 0x208) = param_5;
    FUN_14004d150(param_1);
    thunk_FUN_14003bd00(param_1);
    FUN_1400443d0(*(undefined8 *)(param_1 + 0x220),0xffffffff);
    FUN_140042ff0(*(undefined8 *)(param_1 + 0x220));
    uVar1 = *(undefined4 *)(param_1 + 0x22c);
  }
  return uVar1;
}


/* ================= FUN_14004de60 @ 14004de60  size=10  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003d210 */

void FUN_14004de60(longlong param_1)

{
  if (param_1 == 0) {
    return;
  }
  FUN_1400443e0(*(undefined8 *)(param_1 + 0x318),0xffffffff);
  return;
}


/* ================= FUN_14004de70 @ 14004de70  size=40  callers=1 callees=3 ================= */
/* CALLERS: FUN_14002a1a0 */
/* CALLEES: FUN_14004d020, FUN_14004d150, thunk_FUN_14003bc60 */

void FUN_14004de70(longlong param_1)

{
  if (param_1 != 0) {
    FUN_14004d020();
    thunk_FUN_14003bc60(param_1);
    FUN_14004d150(param_1);
  }
  return;
}


/* ================= FUN_14004dea0 @ 14004dea0  size=40  callers=1 callees=3 ================= */
/* CALLERS: FUN_14002a1c0 */
/* CALLEES: FUN_14004d020, FUN_14004d150, thunk_FUN_14003bc80 */

void FUN_14004dea0(longlong param_1)

{
  if (param_1 != 0) {
    FUN_14004d020();
    thunk_FUN_14003bc80(param_1);
    FUN_14004d150(param_1);
  }
  return;
}


/* ================= FUN_14004ded0 @ 14004ded0  size=67  callers=1 callees=3 ================= */
/* CALLERS: FUN_140046e80 */
/* CALLEES: FUN_14003de60, FUN_14004d020, FUN_14004d150 */

void FUN_14004ded0(longlong param_1,undefined4 param_2,undefined8 param_3)

{
  if (param_1 != 0) {
    FUN_14004d020();
    FUN_14003de60(param_2,param_3);
    FUN_14004d150(param_1);
  }
  return;
}


/* ================= FUN_14004df20 @ 14004df20  size=40  callers=3 callees=3 ================= */
/* CALLERS: FUN_14003d210, FUN_14003f320, FUN_14003fa30 */
/* CALLEES: FUN_14004d020, FUN_14004d150, thunk_FUN_14003bca0 */

void FUN_14004df20(longlong param_1)

{
  if (param_1 != 0) {
    FUN_14004d020();
    thunk_FUN_14003bca0(param_1);
    FUN_14004d150(param_1);
  }
  return;
}


/* ================= FUN_14004df50 @ 14004df50  size=339  callers=1 callees=9 ================= */
/* CALLERS: FUN_14003f580 */
/* CALLEES: FUN_140042ff0, FUN_1400430f0, FUN_140043950, FUN_1400443d0, FUN_14004d020, FUN_14004d0f0, FUN_14004d150, __security_check_cookie, thunk_FUN_14003bcc0 */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined4
FUN_14004df50(longlong param_1,longlong param_2,longlong param_3,longlong param_4,undefined8 param_5
             ,undefined4 param_6,undefined8 param_7,undefined4 *param_8,undefined4 param_9,
             undefined4 param_10,undefined4 param_11)

{
  longlong lVar1;
  undefined8 uVar2;
  undefined1 auStack_98 [32];
  longlong local_78;
  longlong local_70;
  longlong local_68;
  undefined8 local_60;
  undefined8 local_58;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined8 local_40;
  undefined8 local_38;
  undefined8 uStack_30;
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_98;
  if ((((param_1 == 0) || (param_3 == 0)) || (param_2 == 0)) || (param_4 == 0)) {
    param_10 = 0xffffffff;
  }
  else {
    local_50 = 0;
    local_40 = 0;
    local_38 = 0;
    uStack_30 = 0;
    local_60 = param_5;
    local_4c = param_6;
    local_58 = param_7;
    local_48 = param_9;
    local_44 = param_11;
    if (param_8 != (undefined4 *)0x0) {
      local_50 = *param_8;
    }
    local_78 = param_2;
    local_70 = param_3;
    local_68 = param_4;
    lVar1 = FUN_140043950();
    if (lVar1 == *(longlong *)(param_1 + 0x1b0)) {
      FUN_14004d0f0(param_1,&local_78);
    }
    else {
      FUN_14004d020(param_1);
      uVar2 = FUN_1400430f0(0,0);
      *(undefined8 *)(param_1 + 0x1d8) = uVar2;
      *(longlong **)(param_1 + 0x1d0) = &local_78;
      FUN_14004d150(param_1);
      thunk_FUN_14003bcc0(param_1);
      FUN_1400443d0(*(undefined8 *)(param_1 + 0x1d8),0xffffffff);
      FUN_140042ff0(*(undefined8 *)(param_1 + 0x1d8));
    }
    if (param_8 != (undefined4 *)0x0) {
      *param_8 = (undefined4)local_38;
    }
    if ((int)uStack_30 == 0) {
      param_10 = local_38._4_4_;
    }
  }
  return param_10;
}


/* ================= FUN_14004e0b0 @ 14004e0b0  size=40  callers=3 callees=3 ================= */
/* CALLERS: FUN_14003cec0, FUN_14004d9c0, FUN_140053750 */
/* CALLEES: FUN_14004d020, FUN_14004d150, thunk_FUN_14003bd20 */

void FUN_14004e0b0(longlong param_1)

{
  if (param_1 != 0) {
    FUN_14004d020();
    thunk_FUN_14003bd20(param_1);
    FUN_14004d150(param_1);
  }
  return;
}


/* ================= FUN_14004e0e0 @ 14004e0e0  size=10  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003d210 */

void FUN_14004e0e0(longlong param_1)

{
  if (param_1 == 0) {
    return;
  }
  FUN_140044370(*(undefined8 *)(param_1 + 0x318));
  return;
}


/* ================= FUN_14004e0f0 @ 14004e0f0  size=40  callers=5 callees=3 ================= */
/* CALLERS: FUN_14003d5c0, FUN_140054130, FUN_140054170, FUN_1400541b0, FUN_140054200 */
/* CALLEES: FUN_14004d020, FUN_14004d150, thunk_FUN_14003bd40 */

void FUN_14004e0f0(longlong param_1)

{
  if (param_1 != 0) {
    FUN_14004d020();
    thunk_FUN_14003bd40(param_1);
    FUN_14004d150(param_1);
  }
  return;
}


/* ================= FUN_14004e120 @ 14004e120  size=40  callers=26 callees=3 ================= */
/* CALLERS: FUN_14003d5c0, FUN_14003d7d0, FUN_14003fa20, FUN_140040980, FUN_140040fa0, FUN_140042100, FUN_140042260, FUN_140042f30, FUN_140047020, FUN_14004a410, FUN_14004a4b0, FUN_14004a6e0, FUN_14004a750, FUN_14004a7c0, FUN_14004a840, FUN_14004bae0, FUN_14004bdc0, FUN_14004c020, FUN_14004c180, FUN_140051590, FUN_140053c80, FUN_140053f20, FUN_140054060, FUN_1400540f0, FUN_140056070, FUN_1400560d0 */
/* CALLEES: FUN_14004d020, FUN_14004d150, thunk_FUN_14003bd60 */

void FUN_14004e120(longlong param_1)

{
  if (param_1 != 0) {
    FUN_14004d020();
    thunk_FUN_14003bd60(param_1);
    FUN_14004d150(param_1);
  }
  return;
}


/* ================= FUN_14004e150 @ 14004e150  size=145  callers=2 callees=4 ================= */
/* CALLERS: FUN_14003cc50, FUN_14004f950 */
/* CALLEES: FUN_14003ec20, FUN_14004d020, FUN_14004d150, thunk_FUN_14003bd60 */

void FUN_14004e150(longlong param_1)

{
  if (param_1 != 0) {
    FUN_14004d020();
    *(undefined8 *)(param_1 + 0x230) = 0;
    *(undefined8 *)(param_1 + 0x238) = 0;
    *(undefined8 *)(param_1 + 0x240) = 0;
    *(undefined8 *)(param_1 + 0x248) = 0;
    *(undefined8 *)(param_1 + 0x250) = 0;
    *(undefined8 *)(param_1 + 600) = 0;
    *(undefined8 *)(param_1 + 0x260) = 0;
    *(undefined8 *)(param_1 + 0x268) = 0;
    *(undefined8 *)(param_1 + 0x270) = 0;
    *(undefined8 *)(param_1 + 0x278) = 0;
    *(undefined8 *)(param_1 + 0x280) = 0;
    *(undefined8 *)(param_1 + 0x288) = 0;
    *(undefined8 *)(param_1 + 0x290) = 0;
    *(undefined8 *)(param_1 + 0x298) = 0;
    *(undefined8 *)(param_1 + 0x2a0) = 0;
    *(undefined8 *)(param_1 + 0x2a8) = 0;
    *(undefined8 *)(param_1 + 0x2b0) = 0;
    *(undefined8 *)(param_1 + 0x2b8) = 0;
    *(undefined8 *)(param_1 + 0x2c0) = 0;
    *(undefined8 *)(param_1 + 0x2c8) = 0;
    FUN_14003ec20(1);
    FUN_14003ec20(0,10,(undefined8 *)(param_1 + 0x280));
    FUN_14004d150(param_1);
    thunk_FUN_14003bd60(param_1);
  }
  return;
}


/* ================= FUN_14004e1f0 @ 14004e1f0  size=193  callers=2 callees=6 ================= */
/* CALLERS: FUN_14003d210, FUN_1400424f0 */
/* CALLEES: FUN_140042ff0, FUN_1400430f0, FUN_1400443d0, FUN_14004d020, FUN_14004d150, thunk_FUN_14003bda0 */

undefined8
FUN_14004e1f0(longlong param_1,undefined8 param_2,undefined8 param_3,undefined4 param_4,
             undefined4 param_5)

{
  longlong lVar1;
  undefined8 local_res10;
  
  if (param_1 != 0) {
    local_res10 = param_2;
    lVar1 = FUN_1400430f0(0,0);
    if (lVar1 != -1) {
      FUN_14004d020(param_1);
      *(undefined4 *)(param_1 + 500) = param_5;
      *(undefined8 **)(param_1 + 0x1e0) = &local_res10;
      *(undefined8 *)(param_1 + 0x1e8) = param_3;
      *(undefined4 *)(param_1 + 0x1f0) = param_4;
      *(longlong *)(param_1 + 0x1f8) = lVar1;
      FUN_14004d150(param_1);
      thunk_FUN_14003bda0(param_1);
      FUN_1400443d0(lVar1,0xffffffff);
      FUN_140042ff0(lVar1);
    }
    return local_res10;
  }
  return param_2;
}


/* ================= FUN_14004e2c0 @ 14004e2c0  size=76  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003f930 */
/* CALLEES: FUN_140027140, FUN_14004d020, FUN_14004d150, thunk_FUN_14003bd60 */

void FUN_14004e2c0(longlong param_1,longlong param_2)

{
  if ((param_1 != 0) && (param_2 != 0)) {
    FUN_14004d020();
    FUN_140027140(param_1 + 0x2c,param_2,0x184);
    FUN_14004d150(param_1);
    thunk_FUN_14003bd60(param_1);
  }
  return;
}


/* ================= FUN_14004e310 @ 14004e310  size=326  callers=1 callees=3 ================= */
/* CALLERS: FUN_14004f950 */
/* CALLEES: FUN_140027140, FUN_140027760, FUN_14004e610 */

void FUN_14004e310(int param_1,undefined8 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  longlong lVar4;
  undefined *puVar5;
  undefined *puVar6;
  undefined *puVar7;
  int iVar8;
  
  lVar4 = (longlong)param_1 * 0x1044;
  puVar5 = &DAT_14285f920 + lVar4;
  iVar8 = *(int *)(&DAT_142860960 + lVar4);
  iVar3 = 0;
  if (0 < iVar8) {
    do {
      iVar1 = FUN_140027760(param_2,puVar5 + (longlong)iVar3 * 0x104);
      iVar8 = *(int *)(&DAT_142860960 + lVar4);
      iVar2 = iVar3 + 1;
      if (iVar1 == 0) {
        if (iVar2 < iVar8) {
          do {
            FUN_140027140(puVar5 + (longlong)iVar3 * 0x104,puVar5 + (longlong)iVar2 * 0x104,0x104);
            iVar8 = *(int *)(&DAT_142860960 + lVar4);
            iVar2 = iVar2 + 1;
            iVar3 = iVar3 + 1;
          } while (iVar2 < iVar8);
        }
        iVar8 = iVar8 + -1;
        *(int *)(&DAT_142860960 + lVar4) = iVar8;
        break;
      }
      iVar3 = iVar2;
    } while (iVar2 < iVar8);
  }
  if (iVar8 < *(int *)(&DAT_14285f908 + (longlong)param_1 * 4)) {
    iVar8 = iVar8 + 1;
    *(int *)(&DAT_142860960 + lVar4) = iVar8;
  }
  iVar3 = iVar8 + -1;
  if (0 < iVar3) {
    puVar7 = puVar5 + (longlong)iVar3 * 0x104;
    puVar6 = puVar5 + (longlong)(iVar8 + -2) * 0x104;
    do {
      FUN_140027140(puVar7,puVar6,0x104);
      iVar3 = iVar3 + -1;
      puVar6 = puVar6 + -0x104;
      puVar7 = puVar7 + -0x104;
    } while (0 < iVar3);
  }
  FUN_140027140(puVar5,param_2,0x104);
  FUN_14004e610(param_1);
  return;
}


/* ================= FUN_14004e460 @ 14004e460  size=34  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ec20 */

undefined * FUN_14004e460(int param_1,int param_2)

{
  return &DAT_14285f920 + (longlong)param_1 * 0x1044 + (longlong)param_2 * 0x104;
}


/* ================= FUN_14004e490 @ 14004e490  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ec20 */

undefined4 FUN_14004e490(int param_1)

{
  return *(undefined4 *)(&DAT_142860960 + (longlong)param_1 * 0x1044);
}


/* ================= FUN_14004e4b0 @ 14004e4b0  size=339  callers=1 callees=6 ================= */
/* CALLERS: FUN_14003cc50 */
/* CALLEES: FUN_140027140, FUN_140028380, FUN_140044260, FUN_140044270, FUN_1400442a0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

ulonglong FUN_14004e4b0(int param_1,undefined8 param_2,int param_3)

{
  int *piVar1;
  int iVar2;
  longlong lVar3;
  ulonglong uVar4;
  longlong lVar5;
  uint uVar6;
  int iVar7;
  undefined1 auStack_198 [32];
  undefined4 *local_178;
  undefined4 local_168;
  undefined1 local_164 [4];
  undefined1 local_160 [40];
  undefined1 local_138 [272];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_198;
  lVar3 = (longlong)param_1;
  lVar5 = lVar3 * 0x1044;
  local_178 = (undefined4 *)CONCAT44(local_178._4_4_,param_1);
  uVar6 = 0;
  piVar1 = (int *)(&DAT_14285f908 + lVar3 * 4);
  *piVar1 = param_3;
  FUN_140028380(&DAT_14285ef40 + lVar3 * 500,500,"%sRecent File List %d\\",param_2);
  lVar3 = FUN_140044270(0x80000001,&DAT_14285ef40 + lVar3 * 500);
  if (lVar3 == -1) {
    uVar4 = 0xffffffffffffffff;
  }
  else {
    iVar7 = 0;
    if (0 < *piVar1) {
      do {
        iVar7 = iVar7 + 1;
        FUN_140028380(local_160,0x20,"File%i",iVar7);
        local_178 = &local_168;
        local_168 = 0x104;
        iVar2 = FUN_1400442a0(lVar3,local_160,local_164,local_138);
        if (iVar2 != 0) {
          uVar6 = 0xfffffffe;
          goto LAB_14004e5d1;
        }
        FUN_140027140(&DAT_14285f920 + (longlong)*(int *)(&DAT_142860960 + lVar5) * 0x104 + lVar5,
                      local_138,0x104);
        *(int *)(&DAT_142860960 + lVar5) = *(int *)(&DAT_142860960 + lVar5) + 1;
      } while (iVar7 < *piVar1);
      uVar6 = 0;
    }
LAB_14004e5d1:
    FUN_140044260(lVar3);
    uVar4 = (ulonglong)uVar6;
  }
  return uVar4;
}


/* ================= FUN_14004e610 @ 14004e610  size=272  callers=1 callees=6 ================= */
/* CALLERS: FUN_14004e310 */
/* CALLEES: FUN_1400277b0, FUN_140028380, FUN_140044260, FUN_140044270, FUN_140044330, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

ulonglong FUN_14004e610(int param_1)

{
  int iVar1;
  int iVar2;
  longlong lVar3;
  ulonglong uVar4;
  longlong lVar5;
  uint uVar6;
  undefined1 auStack_68 [32];
  undefined4 local_48;
  undefined1 local_38 [24];
  ulonglong local_20;
  
  local_20 = DAT_140096fe8 ^ (ulonglong)auStack_68;
  uVar6 = 0;
  lVar5 = (longlong)param_1 * 0x1044;
  lVar3 = FUN_140044270(0x80000001,&DAT_14285ef40 + (longlong)param_1 * 500);
  if (lVar3 == -1) {
    uVar4 = 0xffffffffffffffff;
  }
  else {
    iVar2 = 0;
    if (0 < *(int *)(&DAT_142860960 + lVar5)) {
      do {
        iVar1 = iVar2 + 1;
        FUN_140028380(local_38,0x14,"File%i",iVar1);
        local_48 = FUN_1400277b0(&DAT_14285f920 + (longlong)iVar2 * 0x104 + lVar5);
        iVar2 = FUN_140044330(lVar3,local_38,2,&DAT_14285f920 + (longlong)iVar2 * 0x104 + lVar5);
        if (iVar2 != 0) {
          uVar6 = 0xfffffffe;
        }
        iVar2 = iVar1;
      } while (iVar1 < *(int *)(&DAT_142860960 + lVar5));
    }
    FUN_140044260(lVar3);
    uVar4 = (ulonglong)uVar6;
  }
  return uVar4;
}


/* ================= FUN_14004e720 @ 14004e720  size=91  callers=1 callees=3 ================= */
/* CALLERS: FUN_14004e8f0 */
/* CALLEES: FUN_14003f4f0, FUN_14004eb40, FUN_14004ebc0 */

ulonglong FUN_14004e720(void)

{
  ulonglong uVar1;
  
  uVar1 = FUN_14003f4f0("The J-Scope project has been modified.\nDo you want to save your changes?",
                        &DAT_140074740,&DAT_14007473c,0,0x200,0xffffffff);
  if ((int)uVar1 == 0) {
    if (DAT_142865070 == (char)uVar1) {
      FUN_14004ebc0();
      return uVar1 & 0xffffffff;
    }
    FUN_14004eb40();
    uVar1 = uVar1 & 0xffffffff;
  }
  return uVar1;
}


/* ================= FUN_14004e780 @ 14004e780  size=94  callers=1 callees=2 ================= */
/* CALLERS: FUN_14004ec40 */
/* CALLEES: FUN_14004ed70, memcmp */

bool FUN_14004e780(void *param_1)

{
  int iVar1;
  
  iVar1 = FUN_14004ed70();
  if ((iVar1 != 0) && (param_1 != (void *)0x0)) {
    iVar1 = memcmp(&DAT_142864a88,param_1,0x5bc);
    return iVar1 != 0;
  }
  return false;
}


/* ================= FUN_14004e7e0 @ 14004e7e0  size=65  callers=2 callees=1 ================= */
/* CALLERS: FUN_14004ec80, FUN_14004ecc0 */
/* CALLEES: memcmp */

bool FUN_14004e7e0(void *param_1)

{
  int iVar1;
  
  if (param_1 != (void *)0x0) {
    iVar1 = memcmp(&DAT_142864a80,param_1,0x5e8);
    return iVar1 != 0;
  }
  return false;
}


/* ================= FUN_14004e830 @ 14004e830  size=107  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004ed00 */
/* CALLEES: FUN_14004ed70 */

bool FUN_14004e830(longlong *param_1)

{
  int iVar1;
  longlong lVar2;
  
  iVar1 = FUN_14004ed70();
  if ((iVar1 != 0) && (param_1 != (longlong *)0x0)) {
    lVar2 = DAT_142865050 - *param_1;
    if ((lVar2 == 0) && (lVar2 = DAT_142865058 - param_1[1], lVar2 == 0)) {
      lVar2 = DAT_142865060 - param_1[2];
    }
    return lVar2 != 0;
  }
  return false;
}


/* ================= FUN_14004e8a0 @ 14004e8a0  size=78  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004ed40 */
/* CALLEES: FUN_14004ed70 */

bool FUN_14004e8a0(longlong *param_1)

{
  int iVar1;
  
  iVar1 = FUN_14004ed70();
  if ((iVar1 != 0) && (param_1 != (longlong *)0x0)) {
    return DAT_142865044 != *param_1;
  }
  return false;
}


/* ================= FUN_14004e8f0 @ 14004e8f0  size=111  callers=2 callees=12 ================= */
/* CALLERS: FUN_14004e960, FUN_14004eda0 */
/* CALLEES: FUN_14003ea40, FUN_14003eef0, FUN_14003ef10, FUN_14003ef20, FUN_14003f090, FUN_14003f320, FUN_14003f330, FUN_14003f830, FUN_14004e720, FUN_14004e990, FUN_14004ed70, FUN_14004eef0 */

undefined8 FUN_14004e8f0(void)

{
  char cVar1;
  int iVar2;
  
  cVar1 = FUN_14003ef10();
  if (cVar1 != '\0') {
    FUN_14003f090();
  }
  cVar1 = FUN_14003eef0();
  if (cVar1 != '\0') {
    FUN_14003ef20();
  }
  iVar2 = FUN_14004ed70();
  if (iVar2 != 0) {
    iVar2 = FUN_14004e990();
    if (0 < iVar2) {
      iVar2 = FUN_14004e720();
      if (iVar2 == 2) {
        return 0xffffffff;
      }
    }
    FUN_14003ea40();
    FUN_14003f830("Close project");
    FUN_14004eef0(0);
    FUN_14003f330();
    FUN_14003f320();
  }
  return 0;
}


/* ================= FUN_14004e960 @ 14004e960  size=33  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003ce50 */
/* CALLEES: FUN_14004e8f0, FUN_14004ea40 */

ulonglong FUN_14004e960(void)

{
  uint uVar1;
  ulonglong uVar2;
  
  uVar2 = FUN_14004ea40();
  if ((int)uVar2 != 0) {
    uVar1 = FUN_14004e8f0();
    uVar2 = (ulonglong)uVar1;
    if (uVar1 != 0) {
      uVar2 = 0xffffffff;
    }
  }
  return uVar2;
}


/* ================= FUN_14004e990 @ 14004e990  size=7  callers=2 callees=0 ================= */
/* CALLERS: FUN_14003f930, FUN_14004e8f0 */

undefined4 FUN_14004e990(void)

{
  return DAT_142865068;
}


/* ================= FUN_14004e9a0 @ 14004e9a0  size=20  callers=2 callees=0 ================= */
/* CALLERS: FUN_14003f930, FUN_14004eb40 */

undefined1 * FUN_14004e9a0(void)

{
  undefined1 *puVar1;
  
  puVar1 = &DAT_142865070;
  if (DAT_14286506c == '\0') {
    puVar1 = (undefined1 *)0x0;
  }
  return puVar1;
}


/* ================= FUN_14004e9c0 @ 14004e9c0  size=24  callers=22 callees=1 ================= */
/* CALLERS: FUN_14003d5c0, FUN_14003d7d0, FUN_14003d9b0, FUN_14003dcb0, FUN_14003dd90, FUN_14003ef20, FUN_14003ef60, FUN_14003f390, FUN_14003f890, FUN_14003f8d0, FUN_14003fa30, FUN_140041710, FUN_140047390, FUN_1400476d0, FUN_14004d6c0, FUN_140053000, FUN_140053920, FUN_140054060, FUN_140054250, FUN_140055bb0, FUN_140055dc0, FUN_140056130 */
/* CALLEES: memcpy */

void FUN_14004e9c0(void *param_1)

{
  if (param_1 != (void *)0x0) {
    memcpy(param_1,&DAT_142864a88,0x5bc);
    return;
  }
  return;
}


/* ================= FUN_14004e9e0 @ 14004e9e0  size=61  callers=4 callees=3 ================= */
/* CALLERS: FUN_14002a410, FUN_14003db70, FUN_1400471d0, FUN_14004d9c0 */
/* CALLEES: FUN_14004ed70, memcpy, memset */

void FUN_14004e9e0(void *param_1)

{
  int iVar1;
  
  if (param_1 != (void *)0x0) {
    iVar1 = FUN_14004ed70();
    if (iVar1 != 0) {
      memcpy(param_1,&DAT_142864a80,0x5e8);
      return;
    }
    memset(param_1,0,0x5e8);
  }
  return;
}


/* ================= FUN_14004ea20 @ 14004ea20  size=29  callers=5 callees=0 ================= */
/* CALLERS: FUN_14003d5c0, FUN_14003fa30, FUN_14003fa70, FUN_140056ad0, FUN_140056e40 */

void FUN_14004ea20(undefined8 *param_1)

{
  undefined8 uVar1;
  
  uVar1 = DAT_142865058;
  if (param_1 != (undefined8 *)0x0) {
    *param_1 = DAT_142865050;
    param_1[1] = uVar1;
    param_1[2] = DAT_142865060;
  }
  return;
}


/* ================= FUN_14004ea40 @ 14004ea40  size=8  callers=10 callees=0 ================= */
/* CALLERS: FUN_140029da0, FUN_14003d9b0, FUN_14003f930, FUN_14004bdc0, FUN_14004d870, FUN_14004e960, FUN_14004ed70, FUN_14004eda0, FUN_140053000, FUN_140055bb0 */

int FUN_14004ea40(void)

{
  return (int)DAT_14286506c;
}


/* ================= FUN_14004ea50 @ 14004ea50  size=19  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003cc50 */
/* CALLEES: FUN_14004eda0 */

int FUN_14004ea50(undefined8 param_1)

{
  char cVar1;
  
  cVar1 = FUN_14004eda0(param_1,1);
  return (int)cVar1;
}


/* ================= FUN_14004ea70 @ 14004ea70  size=198  callers=1 callees=7 ================= */
/* CALLERS: thunk_FUN_14004ea70 */
/* CALLEES: FUN_14003f930, FUN_14004dcb0, FUN_14004eda0, FUN_14004f910, FUN_14004fab0, __security_check_cookie, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

int FUN_14004ea70(char *param_1)

{
  char cVar1;
  int iVar2;
  undefined1 auStack_158 [32];
  char *local_138;
  char *local_130;
  char local_128 [272];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_158;
  memset(local_128,0,0x104);
  if ((param_1 == (char *)0x0) || (*param_1 == '\0')) {
    FUN_14004f910(local_128,0x104);
    local_130 = "J-Scope project files (*.jscope)";
    local_138 = "All files (*.*);;J-Scope project files (*.jscope)";
    iVar2 = FUN_14004dcb0(DAT_14286d388,local_128,0x104,"Open project");
    if (iVar2 < 0) {
      cVar1 = -1;
      goto LAB_14004eb15;
    }
    FUN_14004fab0(local_128);
    param_1 = local_128;
  }
  cVar1 = FUN_14004eda0(param_1,0);
LAB_14004eb15:
  FUN_14003f930();
  return (int)cVar1;
}


/* ================= FUN_14004eb40 @ 14004eb40  size=118  callers=2 callees=6 ================= */
/* CALLERS: FUN_14004e720, FUN_14004ebc0 */
/* CALLEES: FUN_14003e9a0, FUN_1400471d0, FUN_14004e9a0, FUN_14004ed70, FUN_14004eef0, FUN_14004f950 */

undefined8 FUN_14004eb40(void)

{
  int iVar1;
  undefined8 uVar2;
  
  iVar1 = FUN_14004ed70();
  if (iVar1 == 0) {
    return 0;
  }
  uVar2 = FUN_14004e9a0();
  iVar1 = FUN_1400471d0(uVar2);
  if (iVar1 == 0) {
    FUN_14003e9a0("Project saved successfully");
    FUN_14004f950(uVar2);
    DAT_14286506d = 1;
    FUN_14004eef0(0);
    return 0;
  }
  FUN_14003e9a0("Failed to save project file");
  return 0xffffffff;
}


/* ================= FUN_14004ebc0 @ 14004ebc0  size=92  callers=1 callees=3 ================= */
/* CALLERS: FUN_14004e720 */
/* CALLEES: FUN_14004dd70, FUN_14004eb40, FUN_14004ed70 */

undefined8 FUN_14004ebc0(void)

{
  int iVar1;
  
  iVar1 = FUN_14004ed70();
  if (iVar1 != 0) {
    iVar1 = FUN_14004dd70(DAT_14286d388,&DAT_142865070,0x104,"Save project as",
                          "J-Scope project files (*.jscope)",0);
    if (iVar1 < 0) {
      return 0xffffffff;
    }
    FUN_14004eb40();
  }
  return 0;
}


/* ================= FUN_14004ec20 @ 14004ec20  size=21  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004eda0 */
/* CALLEES: FUN_140027140 */

void FUN_14004ec20(undefined8 param_1)

{
  FUN_140027140(&DAT_142865070,param_1,0x104);
  return;
}


/* ================= FUN_14004ec40 @ 14004ec40  size=55  callers=8 callees=3 ================= */
/* CALLERS: FUN_14003cec0, FUN_14003dcb0, FUN_14003dd90, FUN_14003fc90, FUN_1400419f0, FUN_140042100, FUN_140047390, FUN_1400476d0 */
/* CALLEES: FUN_14004e780, memcpy, thunk_FUN_14003f930 */

void FUN_14004ec40(void *param_1)

{
  char cVar1;
  
  cVar1 = FUN_14004e780();
  if (cVar1 != '\0') {
    memcpy(&DAT_142864a88,param_1,0x5bc);
    thunk_FUN_14003f930();
    return;
  }
  return;
}


/* ================= FUN_14004ec80 @ 14004ec80  size=60  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003db70 */
/* CALLEES: FUN_14003fa30, FUN_14004e7e0, memcpy, thunk_FUN_14003f930 */

void FUN_14004ec80(void *param_1)

{
  char cVar1;
  
  cVar1 = FUN_14004e7e0();
  if (cVar1 != '\0') {
    memcpy(&DAT_142864a80,param_1,0x5e8);
    FUN_14003fa30();
    thunk_FUN_14003f930();
    return;
  }
  return;
}


/* ================= FUN_14004ecc0 @ 14004ecc0  size=50  callers=1 callees=2 ================= */
/* CALLERS: FUN_140047020 */
/* CALLEES: FUN_14004e7e0, memcpy */

void FUN_14004ecc0(void *param_1)

{
  char cVar1;
  
  cVar1 = FUN_14004e7e0();
  if (cVar1 != '\0') {
    memcpy(&DAT_142864a80,param_1,0x5e8);
    return;
  }
  return;
}


/* ================= FUN_14004ed00 @ 14004ed00  size=57  callers=11 callees=2 ================= */
/* CALLERS: FUN_14003cec0, FUN_14003d5c0, FUN_140041040, FUN_1400410b0, FUN_140041110, FUN_140041190, FUN_140041200, FUN_140041280, FUN_140041300, FUN_140041380, FUN_140056da0 */
/* CALLEES: FUN_14004e830, thunk_FUN_14003f930 */

void FUN_14004ed00(undefined4 *param_1)

{
  char cVar1;
  
  cVar1 = FUN_14004e830();
  if (cVar1 != '\0') {
    DAT_142865050._0_4_ = *param_1;
    DAT_142865050._4_4_ = param_1[1];
    DAT_142865058._0_4_ = param_1[2];
    DAT_142865058._4_4_ = param_1[3];
    DAT_142865060 = *(undefined8 *)(param_1 + 4);
    thunk_FUN_14003f930();
    return;
  }
  return;
}


/* ================= FUN_14004ed40 @ 14004ed40  size=44  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003cec0 */
/* CALLEES: FUN_14004e8a0, thunk_FUN_14003f930 */

void FUN_14004ed40(undefined8 *param_1)

{
  char cVar1;
  
  cVar1 = FUN_14004e8a0();
  if (cVar1 != '\0') {
    DAT_142865044 = *param_1;
    thunk_FUN_14003f930();
    return;
  }
  return;
}


/* ================= FUN_14004ed70 @ 14004ed70  size=23  callers=7 callees=1 ================= */
/* CALLERS: FUN_14004e780, FUN_14004e830, FUN_14004e8a0, FUN_14004e8f0, FUN_14004e9e0, FUN_14004eb40, FUN_14004ebc0 */
/* CALLEES: FUN_14004ea40 */

bool FUN_14004ed70(void)

{
  int iVar1;
  
  iVar1 = FUN_14004ea40();
  return iVar1 != 0;
}


/* ================= FUN_14004eda0 @ 14004eda0  size=335  callers=2 callees=11 ================= */
/* CALLERS: FUN_14004ea50, FUN_14004ea70 */
/* CALLEES: FUN_140028380, FUN_14003e4e0, FUN_14003e9a0, FUN_14003f830, FUN_140047020, FUN_14004e8f0, FUN_14004ea40, FUN_14004ec20, FUN_14004eef0, FUN_14004f950, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14004eda0(longlong param_1,char param_2)

{
  int iVar1;
  undefined8 uVar2;
  char *pcVar3;
  undefined1 auStack_188 [32];
  longlong local_168;
  undefined1 local_158 [304];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_188;
  pcVar3 = "Opening";
  if (param_2 != '\0') {
    pcVar3 = "Creating new";
  }
  iVar1 = FUN_14004ea40();
  if ((iVar1 != 0) && (iVar1 = FUN_14004e8f0(), iVar1 != 0)) {
    return 0xff;
  }
  if (param_1 == 0) {
    FUN_140028380(local_158,0x124,"%s project ...",pcVar3);
  }
  else {
    local_168 = param_1;
    FUN_140028380(local_158,0x124,"%s project file [%s] ...",pcVar3);
  }
  FUN_14003f830(local_158);
  FUN_14004ec20(param_1);
  FUN_14004eef0(0);
  DAT_14286506d = param_1 != 0;
  if (param_1 != 0) {
    FUN_14004f950(param_1);
  }
  if (param_2 == '\0') {
    iVar1 = FUN_140047020(param_1);
    if (iVar1 < 0) {
      FUN_14003e9a0("Failed");
      return 0xfe;
    }
    if (DAT_142864b08 != '\0') {
      FUN_14003e4e0(&DAT_142864b08,0);
    }
    if (iVar1 != 0) goto LAB_14004eea9;
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  FUN_14004eef0(uVar2);
LAB_14004eea9:
  DAT_14286506c = 1;
  pcVar3 = "Project opened successfully";
  if (param_2 != '\0') {
    pcVar3 = "New project created successfully";
  }
  FUN_14003e9a0(pcVar3);
  return 0;
}


/* ================= FUN_14004eef0 @ 14004eef0  size=11  callers=3 callees=1 ================= */
/* CALLERS: FUN_14004e8f0, FUN_14004eb40, FUN_14004eda0 */
/* CALLEES: FUN_14003f930 */

void FUN_14004eef0(undefined4 param_1)

{
  DAT_142865068 = param_1;
  FUN_14003f930();
  return;
}


/* ================= FUN_14004ef00 @ 14004ef00  size=185  callers=1 callees=10 ================= */
/* CALLERS: FUN_14004f940 */
/* CALLEES: FUN_140043110, FUN_140043430, FUN_140043720, FUN_140043d10, FUN_140043e60, FUN_1400440e0, FUN_14004f590, FUN_14004f6a0, FUN_14004f7e0, atexit */

undefined8 FUN_14004ef00(char *param_1)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  
  if ((param_1 == (char *)0x0) || (*param_1 == '\0')) {
    return 0xffffffff;
  }
  atexit(FUN_14004f300);
  DAT_14286c7c0 = FUN_140043110(0);
  FUN_14004f6a0();
  uVar3 = FUN_140043d10(0x104);
  FUN_140043430(uVar3,0x104);
  FUN_1400440e0(uVar3,param_1,&DAT_142865180,0x104);
  FUN_140043e60(uVar3);
  cVar1 = FUN_140043720(&DAT_142865180);
  if ((cVar1 != '\0') && (iVar2 = FUN_14004f590(), iVar2 < 0)) {
    return 0xffffffff;
  }
  DAT_142865288 = 1;
  FUN_14004f7e0();
  return 0;
}


/* ================= FUN_14004efc0 @ 14004efc0  size=251  callers=1 callees=10 ================= */
/* CALLERS: FUN_14004f8c0 */
/* CALLEES: FUN_1400277b0, FUN_14003f430, FUN_140044840, FUN_1400448c0, FUN_1400448e0, FUN_14004f2f0, FUN_14004f470, FUN_14004f6a0, FUN_14004f7e0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14004efc0(undefined8 param_1,undefined8 param_2,undefined8 param_3)

{
  int iVar1;
  undefined8 uVar2;
  undefined1 auStack_168 [32];
  undefined4 local_148;
  undefined4 local_140;
  longlong local_138 [2];
  char local_128 [256];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_168;
  uVar2 = 0xffffffff;
  iVar1 = FUN_14004f2f0();
  if (iVar1 == 0) {
    local_128[0] = '\0';
    FUN_14004f6a0();
    local_138[0] = FUN_14004f470(param_1,param_2);
    if (local_138[0] != 0) {
      iVar1 = FUN_1400277b0(param_2);
      local_138[0] = local_138[0] + iVar1;
      FUN_1400448c0(local_138);
      FUN_140044840(local_128,0x100,local_138,0x3d);
      FUN_1400448c0(local_138);
      local_140 = 0x7fffffff;
      local_148 = 0;
      FUN_1400448e0(local_128,0x100,local_138,param_3);
      if (local_128[0] == '\0') {
        uVar2 = 0;
      }
      else {
        FUN_14003f430(local_128);
      }
    }
    FUN_14004f7e0();
  }
  return uVar2;
}


/* ================= FUN_14004f0c0 @ 14004f0c0  size=281  callers=4 callees=11 ================= */
/* CALLERS: FUN_14004f7f0, FUN_14004f820, FUN_14004f890, FUN_14004f910 */
/* CALLEES: FUN_1400277b0, FUN_140028380, FUN_14003f430, FUN_140044840, FUN_1400448c0, FUN_140044a20, FUN_14004f2f0, FUN_14004f470, FUN_14004f6a0, FUN_14004f7e0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14004f0c0(undefined8 param_1,longlong param_2,undefined1 *param_3,undefined4 param_4)

{
  int iVar1;
  undefined8 uVar2;
  undefined1 auStack_168 [32];
  longlong local_148 [2];
  char local_138 [256];
  ulonglong local_38;
  
  local_38 = DAT_140096fe8 ^ (ulonglong)auStack_168;
  uVar2 = 0xffffffff;
  iVar1 = FUN_14004f2f0();
  if (iVar1 == 0) {
    local_138[0] = '\0';
    FUN_14004f6a0();
    *param_3 = 0;
    local_148[0] = FUN_14004f470(param_1,param_2);
    if (local_148[0] != 0) {
      iVar1 = FUN_1400277b0(param_2);
      local_148[0] = local_148[0] + iVar1;
      FUN_1400448c0(local_148);
      FUN_140044840(local_138,0x100,local_148,0x3d);
      FUN_1400448c0(local_148);
      FUN_140044a20(local_138,local_148,param_3,param_4);
      if (local_138[0] == '\0') {
        uVar2 = 0;
      }
      else if (param_2 == 0) {
        FUN_14003f430("Error while parsing settings file. Syntax error.");
      }
      else {
        FUN_140028380(local_138,0x100,
                      "Error while parsing subkey \"%s\" in settings file. Syntax error.",param_2);
        FUN_14003f430(local_138);
      }
    }
    FUN_14004f7e0();
  }
  return uVar2;
}


/* ================= FUN_14004f1e0 @ 14004f1e0  size=181  callers=4 callees=10 ================= */
/* CALLERS: FUN_14004f950, FUN_14004f9b0, FUN_14004fa30, FUN_14004fab0 */
/* CALLEES: FUN_140028380, FUN_14004f2f0, FUN_14004f330, FUN_14004f3a0, FUN_14004f470, FUN_14004f510, FUN_14004f6a0, FUN_14004f6c0, FUN_14004f7e0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined4 FUN_14004f1e0(undefined8 param_1,undefined8 param_2,undefined8 param_3)

{
  int iVar1;
  undefined4 uVar2;
  longlong lVar3;
  undefined8 uVar4;
  undefined1 auStack_828 [32];
  undefined8 local_808;
  undefined1 local_7f8 [2000];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_828;
  iVar1 = FUN_14004f2f0();
  if (iVar1 == 0) {
    FUN_14004f6a0();
    lVar3 = FUN_14004f470(param_1,param_2);
    if (lVar3 != 0) {
      FUN_14004f330(param_1,param_2);
    }
    uVar4 = FUN_14004f3a0(param_1);
    local_808 = param_3;
    FUN_140028380(local_7f8,2000,"%s=\"%s\"\r\n",param_2);
    FUN_14004f510(uVar4,local_7f8);
    uVar2 = FUN_14004f6c0();
    FUN_14004f7e0();
  }
  else {
    uVar2 = 0xffffffff;
  }
  return uVar2;
}


/* ================= FUN_14004f2a0 @ 14004f2a0  size=70  callers=1 callees=2 ================= */
/* CALLERS: FUN_14004f3a0 */
/* CALLEES: FUN_1400277b0, FUN_140028380 */

undefined1 * FUN_14004f2a0(undefined8 param_1)

{
  FUN_140028380(&DAT_142865290 + DAT_142865284,30000 - DAT_142865284,"[%s]\r\n",param_1);
  DAT_142865284 = FUN_1400277b0(&DAT_142865290);
  return &DAT_142865290 + DAT_142865284;
}


/* ================= FUN_14004f2f0 @ 14004f2f0  size=13  callers=3 callees=0 ================= */
/* CALLERS: FUN_14004efc0, FUN_14004f0c0, FUN_14004f1e0 */

int FUN_14004f2f0(void)

{
  return -(uint)(DAT_142865288 != 1);
}


/* ================= FUN_14004f300 @ 14004f300  size=42  callers=0 callees=1 ================= */
/* CALLEES: FUN_140043000 */

void FUN_14004f300(void)

{
  if (DAT_14286c7c0 != 0) {
    FUN_140043000(&DAT_14286c7c0);
    DAT_14286c7c0 = 0;
  }
  return;
}


/* ================= FUN_14004f330 @ 14004f330  size=104  callers=1 callees=4 ================= */
/* CALLERS: FUN_14004f1e0 */
/* CALLEES: FUN_1400277b0, FUN_140044880, FUN_14004f470, memmove */

void FUN_14004f330(void)

{
  void *_Dst;
  void *local_res18 [2];
  
  _Dst = (void *)FUN_14004f470();
  if (_Dst != (void *)0x0) {
    local_res18[0] = _Dst;
    FUN_140044880(local_res18);
    if (_Dst < local_res18[0]) {
      memmove(_Dst,local_res18[0],
              (ulonglong)(uint)((int)local_res18[0] + DAT_142865284 + -0x4286528f));
      DAT_142865284 = FUN_1400277b0(&DAT_142865290);
    }
  }
  return;
}


/* ================= FUN_14004f3a0 @ 14004f3a0  size=201  callers=2 callees=4 ================= */
/* CALLERS: FUN_14004f1e0, FUN_14004f470 */
/* CALLEES: FUN_1400277b0, FUN_1400277f0, FUN_140044880, FUN_14004f2a0 */

char * FUN_14004f3a0(longlong param_1)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  char *pcVar4;
  char *local_res8;
  
  pcVar4 = &DAT_142865290;
  local_res8 = &DAT_142865290;
  if (param_1 != 0) {
    uVar2 = FUN_1400277b0();
    cVar1 = *local_res8;
    while (cVar1 != '\0') {
      if (cVar1 == '[') {
        local_res8 = local_res8 + 1;
        iVar3 = FUN_1400277f0(local_res8,param_1,(ulonglong)uVar2);
        if ((iVar3 == 0) && (local_res8 = local_res8 + uVar2, *local_res8 == ']')) {
          return local_res8 + 3;
        }
      }
      else {
        FUN_140044880(&local_res8);
      }
      cVar1 = *local_res8;
    }
    if (uVar2 != 0) {
      pcVar4 = (char *)FUN_14004f2a0(param_1);
      return pcVar4;
    }
    pcVar4 = (char *)0x0;
  }
  return pcVar4;
}


/* ================= FUN_14004f470 @ 14004f470  size=159  callers=4 callees=5 ================= */
/* CALLERS: FUN_14004efc0, FUN_14004f0c0, FUN_14004f1e0, FUN_14004f330 */
/* CALLEES: FUN_1400277b0, FUN_1400277f0, FUN_140044880, FUN_1400448c0, FUN_14004f3a0 */

char * FUN_14004f470(undefined8 param_1,undefined8 param_2)

{
  int iVar1;
  int iVar2;
  char cVar3;
  char *local_res18 [2];
  
  local_res18[0] = (char *)FUN_14004f3a0();
  iVar1 = FUN_1400277b0(param_2);
  cVar3 = *local_res18[0];
  if (cVar3 == '\0') {
    return (char *)0x0;
  }
  while( true ) {
    if (cVar3 == '[') {
      return (char *)0x0;
    }
    FUN_1400448c0(local_res18);
    iVar2 = FUN_1400277f0(local_res18[0],param_2,(longlong)iVar1);
    if ((iVar2 == 0) && ((local_res18[0][iVar1] == ' ' || (local_res18[0][iVar1] == '=')))) break;
    FUN_140044880(local_res18);
    cVar3 = *local_res18[0];
    if (cVar3 == '\0') {
      return (char *)0x0;
    }
  }
  return local_res18[0];
}


/* ================= FUN_14004f510 @ 14004f510  size=116  callers=1 callees=3 ================= */
/* CALLERS: FUN_14004f1e0 */
/* CALLEES: FUN_1400277b0, memcpy, memmove */

void FUN_14004f510(void *param_1,void *param_2)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = FUN_1400277b0(param_2);
  iVar2 = FUN_1400277b0(&DAT_142865290);
  memmove((void *)((ulonglong)uVar1 + (longlong)param_1),param_1,
          (ulonglong)(uint)((0x42865290 - (int)param_1) + iVar2));
  memcpy(param_1,param_2,(ulonglong)uVar1);
  DAT_142865284 = FUN_1400277b0(&DAT_142865290);
  return;
}


/* ================= FUN_14004f590 @ 14004f590  size=271  callers=1 callees=8 ================= */
/* CALLERS: FUN_14004ef00 */
/* CALLEES: CloseHandle, FUN_140028380, FUN_14003eb20, FUN_14003f430, FUN_140043710, FUN_140043760, FUN_1400438b0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14004f590(void)

{
  uint uVar1;
  HANDLE hObject;
  undefined8 uVar2;
  undefined1 auStack_248 [32];
  undefined4 local_228;
  char local_218 [512];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_248;
  local_218[0] = '\0';
  hObject = (HANDLE)FUN_140043760(&DAT_142865180,5,0x28,5);
  if (hObject == (HANDLE)0xffffffffffffffff) {
    FUN_14003eb20(&DAT_142865180,&DAT_140077278,local_218,0x200);
  }
  else {
    DAT_142865284 = FUN_140043710(hObject);
    if (DAT_142865284 < 0x7531) {
      uVar1 = FUN_1400438b0(hObject,&DAT_142865290,DAT_142865284);
      if (uVar1 != DAT_142865284) {
        FUN_140028380(local_218,0x200,"Could not open file [%s]\nFailed to read data!",
                      &DAT_142865180);
      }
    }
    else {
      local_228 = 30000;
      FUN_140028380(local_218,0x200,
                    "Could not open file [%s]\nFile too large - maximum file size (%d bytes) exceeded!"
                    ,&DAT_142865180);
    }
    CloseHandle(hObject);
  }
  if (local_218[0] == '\0') {
    uVar2 = 0;
  }
  else {
    FUN_14003f430(local_218);
    uVar2 = 0xffffffff;
  }
  return uVar2;
}


/* ================= FUN_14004f6a0 @ 14004f6a0  size=17  callers=4 callees=1 ================= */
/* CALLERS: FUN_14004ef00, FUN_14004efc0, FUN_14004f0c0, FUN_14004f1e0 */
/* CALLEES: FUN_1400443e0 */

void FUN_14004f6a0(void)

{
  FUN_1400443e0(DAT_14286c7c0,0xffffffff);
  return;
}


/* ================= FUN_14004f6c0 @ 14004f6c0  size=273  callers=1 callees=8 ================= */
/* CALLERS: FUN_14004f1e0 */
/* CALLEES: CloseHandle, FUN_1400277b0, FUN_140028380, FUN_14003eb20, FUN_14003f430, FUN_140043760, FUN_140043920, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14004f6c0(void)

{
  int iVar1;
  int iVar2;
  HANDLE hObject;
  undefined1 auStack_238 [32];
  char local_218 [512];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_238;
  local_218[0] = '\0';
  hObject = (HANDLE)FUN_140043760(&DAT_142865180,0x32,0x28,5);
  if (hObject == (HANDLE)0xffffffffffffffff) {
    FUN_14003eb20(&DAT_142865180,&DAT_140077308,local_218,0x200);
  }
  else {
    iVar1 = FUN_1400277b0(&DAT_142865290);
    iVar2 = FUN_140043920(hObject,&DAT_142865290,iVar1);
    if (iVar2 != iVar1) {
      FUN_140028380(local_218,0x200,"Could not save file [%s]\nFailed to write data!",&DAT_142865180
                   );
    }
    CloseHandle(hObject);
  }
  if (local_218[0] != '\0') {
    FUN_14003f430(local_218);
    return 0xffffffff;
  }
  return 0;
}


/* ================= FUN_14004f7e0 @ 14004f7e0  size=12  callers=4 callees=1 ================= */
/* CALLERS: FUN_14004ef00, FUN_14004efc0, FUN_14004f0c0, FUN_14004f1e0 */
/* CALLEES: FUN_140044370 */

void FUN_14004f7e0(void)

{
  FUN_140044370(DAT_14286c7c0);
  return;
}


/* ================= FUN_14004f7f0 @ 14004f7f0  size=35  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003cc50 */
/* CALLEES: FUN_14004f0c0 */

void FUN_14004f7f0(longlong param_1,int param_2)

{
  if ((param_1 != 0) && (param_2 != 0)) {
    FUN_14004f0c0("PROJECT","Current",param_1,param_2);
    return;
  }
  return;
}


/* ================= FUN_14004f820 @ 14004f820  size=105  callers=4 callees=2 ================= */
/* CALLERS: FUN_14002bb30, FUN_140047390, FUN_1400476d0, FUN_14004cf60 */
/* CALLEES: FUN_140043a00, FUN_14004f0c0 */

ulonglong FUN_14004f820(char *param_1,int param_2)

{
  ulonglong uVar1;
  
  if ((param_1 != (char *)0x0) && (param_2 != 0)) {
    uVar1 = FUN_14004f0c0(&DAT_1400773fc,"DataFiles",param_1,param_2);
    if (*param_1 == '\0') {
      FUN_140043a00(param_1,param_2);
      uVar1 = uVar1 & 0xffffffff;
    }
    return uVar1;
  }
  return 0xffffffff;
}


/* ================= FUN_14004f890 @ 14004f890  size=35  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002b940 */
/* CALLEES: FUN_14004f0c0 */

void FUN_14004f890(longlong param_1,int param_2)

{
  if ((param_1 != 0) && (param_2 != 0)) {
    FUN_14004f0c0(&DAT_1400773fc,"ELFFiles",param_1,param_2);
    return;
  }
  return;
}


/* ================= FUN_14004f8c0 @ 14004f8c0  size=76  callers=1 callees=1 ================= */
/* CALLERS: FUN_140041570 */
/* CALLEES: FUN_14004efc0 */

int FUN_14004f8c0(void)

{
  int iVar1;
  int local_res8 [8];
  
  iVar1 = FUN_14004efc0(&DAT_140077434,"LogLevel",local_res8);
  if (iVar1 != 0) {
    return 3;
  }
  if (local_res8[0] < 1) {
    return 1;
  }
  if (9 < local_res8[0]) {
    local_res8[0] = 9;
  }
  return local_res8[0];
}


/* ================= FUN_14004f910 @ 14004f910  size=35  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004ea70 */
/* CALLEES: FUN_14004f0c0 */

void FUN_14004f910(longlong param_1,int param_2)

{
  if ((param_1 != 0) && (param_2 != 0)) {
    FUN_14004f0c0(&DAT_1400773fc,"ProjectFiles",param_1,param_2);
    return;
  }
  return;
}


/* ================= FUN_14004f940 @ 14004f940  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003cc50 */
/* CALLEES: FUN_14004ef00 */

void FUN_14004f940(void)

{
  FUN_14004ef00("JScopeSettings.ini");
  return;
}


/* ================= FUN_14004f950 @ 14004f950  size=92  callers=2 callees=3 ================= */
/* CALLERS: FUN_14004eb40, FUN_14004eda0 */
/* CALLEES: FUN_14004e150, FUN_14004e310, FUN_14004f1e0 */

ulonglong FUN_14004f950(longlong param_1)

{
  ulonglong uVar1;
  
  if (param_1 == 0) {
    return 0xffffffff;
  }
  uVar1 = FUN_14004f1e0("PROJECT","Current",param_1);
  if ((int)uVar1 == 0) {
    FUN_14004e310(1,param_1);
    FUN_14004e150(DAT_14286d388);
    uVar1 = uVar1 & 0xffffffff;
  }
  return uVar1;
}


/* ================= FUN_14004f9b0 @ 14004f9b0  size=128  callers=4 callees=3 ================= */
/* CALLERS: FUN_14002bb30, FUN_140047390, FUN_1400476d0, FUN_14004cf60 */
/* CALLEES: FUN_14004f1e0, FUN_14004fb30, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14004f9b0(char *param_1)

{
  undefined8 uVar1;
  undefined1 auStack_148 [32];
  undefined1 local_128 [272];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_148;
  if ((param_1 != (char *)0x0) && (*param_1 != '\0')) {
    FUN_14004fb30(param_1,local_128,0x104);
    uVar1 = FUN_14004f1e0(&DAT_1400773fc,"DataFiles",local_128);
    return uVar1;
  }
  return 0xffffffff;
}


/* ================= FUN_14004fa30 @ 14004fa30  size=128  callers=1 callees=3 ================= */
/* CALLERS: FUN_14002b940 */
/* CALLEES: FUN_14004f1e0, FUN_14004fb30, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14004fa30(char *param_1)

{
  undefined8 uVar1;
  undefined1 auStack_148 [32];
  undefined1 local_128 [272];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_148;
  if ((param_1 != (char *)0x0) && (*param_1 != '\0')) {
    FUN_14004fb30(param_1,local_128,0x104);
    uVar1 = FUN_14004f1e0(&DAT_1400773fc,"ELFFiles",local_128);
    return uVar1;
  }
  return 0xffffffff;
}


/* ================= FUN_14004fab0 @ 14004fab0  size=128  callers=2 callees=3 ================= */
/* CALLERS: FUN_14004d460, FUN_14004ea70 */
/* CALLEES: FUN_14004f1e0, FUN_14004fb30, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_14004fab0(char *param_1)

{
  undefined8 uVar1;
  undefined1 auStack_148 [32];
  undefined1 local_128 [272];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_148;
  if ((param_1 != (char *)0x0) && (*param_1 != '\0')) {
    FUN_14004fb30(param_1,local_128,0x104);
    uVar1 = FUN_14004f1e0(&DAT_1400773fc,"ProjectFiles",local_128);
    return uVar1;
  }
  return 0xffffffff;
}


/* ================= FUN_14004fb30 @ 14004fb30  size=173  callers=3 callees=3 ================= */
/* CALLERS: FUN_14004f9b0, FUN_14004fa30, FUN_14004fab0 */
/* CALLEES: FUN_1400444d0, FUN_1400444f0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14004fb30(undefined8 param_1,undefined8 param_2,undefined4 param_3)

{
  undefined1 auStack_378 [32];
  undefined8 local_358;
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
  local_350 = local_118;
  local_348 = 0x100;
  local_358 = CONCAT44(local_358._4_4_,0x100);
  FUN_1400444f0(param_1,local_328,3,local_318);
  local_350 = (undefined1 *)0x0;
  local_358 = 0;
  FUN_1400444d0(param_2,param_3,local_328,local_318);
  return;
}


/* ================= FUN_14004fbe0 @ 14004fbe0  size=177  callers=1 callees=4 ================= */
/* CALLERS: FUN_14004c6a0 */
/* CALLEES: FUN_140035ff0, QStatusBar, addPermanentWidget, setSizeGripEnabled */

QStatusBar * FUN_14004fbe0(QStatusBar *param_1,QWidget *param_2)

{
  undefined8 uVar1;
  QWidget *pQVar2;
  
  QStatusBar::QStatusBar(param_1,param_2);
  *(undefined ***)param_1 = StatusBar::vftable;
  *(undefined ***)(param_1 + 0x10) = StatusBar::vftable;
  *(undefined8 *)(param_1 + 0x28) = 0;
  *(undefined8 *)(param_1 + 0x30) = 0;
  *(undefined8 *)(param_1 + 0x38) = 0;
  *(undefined8 *)(param_1 + 0x40) = 0;
  uVar1 = FUN_140035ff0(param_1,&DAT_14005f306);
  *(undefined8 *)(param_1 + 0x30) = uVar1;
  uVar1 = FUN_140035ff0(param_1,&DAT_14005f306);
  *(undefined8 *)(param_1 + 0x28) = uVar1;
  pQVar2 = (QWidget *)FUN_140035ff0(param_1,&DAT_14005f306);
  *(QWidget **)(param_1 + 0x38) = pQVar2;
  QStatusBar::addPermanentWidget(param_1,pQVar2,0);
  QStatusBar::addPermanentWidget(param_1,*(QWidget **)(param_1 + 0x30),0);
  QStatusBar::addPermanentWidget(param_1,*(QWidget **)(param_1 + 0x28),0);
  QStatusBar::setSizeGripEnabled(param_1,true);
  return param_1;
}


/* ================= FUN_14004fca0 @ 14004fca0  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_14004fcd0 */

void FUN_14004fca0(QStatusBar *param_1)

{
  *(undefined ***)param_1 = StatusBar::vftable;
  *(undefined ***)(param_1 + 0x10) = StatusBar::vftable;
                    /* WARNING: Could not recover jumptable at 0x00014004fcb5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QStatusBar::~QStatusBar(param_1);
  return;
}


/* ================= FUN_14004fcbc @ 14004fcbc  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_14004fcd0 */

void FUN_14004fcbc(longlong param_1)

{
  FUN_14004fcd0(param_1 + -0x10);
  return;
}


/* ================= FUN_14004fcd0 @ 14004fcd0  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14004fcbc */
/* CALLEES: FUN_14004fca0, free */

void * FUN_14004fcd0(void *param_1,ulonglong param_2)

{
  FUN_14004fca0();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_14004fd10 @ 14004fd10  size=35  callers=1 callees=3 ================= */
/* CALLERS: FUN_14004d870 */
/* CALLEES: FUN_14004fd40, FUN_14004ff40, FUN_140050030 */

void FUN_14004fd10(undefined8 param_1)

{
  FUN_14004ff40();
  FUN_14004fd40(param_1);
  FUN_140050030(param_1);
  return;
}


/* ================= FUN_14004fd40 @ 14004fd40  size=497  callers=1 callees=8 ================= */
/* CALLERS: FUN_14004fd10 */
/* CALLEES: FUN_140027700, FUN_140028380, FUN_14003ead0, FUN_14003ed50, __security_check_cookie, fromAscii_helper, setText, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14004fd40(longlong param_1)

{
  double dVar1;
  int iVar2;
  int iVar3;
  undefined *puVar4;
  undefined *puVar5;
  undefined1 auStack_198 [32];
  Data *local_178 [2];
  undefined1 local_168 [64];
  char local_128 [256];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_198;
  local_128[0] = '\0';
  iVar3 = FUN_14003ed50();
  iVar3 = iVar3 * 0x4000000;
  dVar1 = (double)FUN_14003ead0();
  iVar2 = (int)((iVar3 >> 0x1f & 0x3ffU) + iVar3) >> 10;
  if (iVar3 < 0x401) {
    iVar2 = iVar3;
  }
  puVar4 = &DAT_14007763c;
  if (iVar3 < 0x401) {
    puVar4 = &DAT_140077634;
  }
  puVar5 = &DAT_140077640;
  if (iVar2 < 0x401) {
    puVar5 = puVar4;
  }
  FUN_140027700(local_128,"Buffer: ",0x100);
  iVar3 = (int)((iVar2 >> 0x1f & 0x3ffU) + iVar2) >> 10;
  if (iVar2 < 0x401) {
    iVar3 = iVar2;
  }
  FUN_140028380(local_168,0x40,&DAT_140077644,iVar3);
  FUN_140027700(local_128,local_168,0x100);
  FUN_140027700(local_128,puVar5,0x100);
  FUN_140027700(local_128,&DAT_140060de4,0x100);
  FUN_140028380(local_168,0x40,&DAT_140060a50,0x800000);
  FUN_140027700(local_128,local_168,0x100);
  FUN_140027700(local_128," samples), ",0x100);
  FUN_140028380(local_168,0x40,&DAT_140060a50,(int)dVar1);
  FUN_140027700(local_128,local_168,0x100);
  FUN_140027700(local_128,&DAT_140077654,0x100);
  local_178[0] = QString::fromAscii_helper(local_128,-1);
  QLabel::setText(*(QLabel **)(param_1 + 0x30),(QString *)local_178);
  QString::~QString((QString *)local_178);
  return;
}


/* ================= FUN_14004ff40 @ 14004ff40  size=237  callers=1 callees=8 ================= */
/* CALLERS: FUN_14004fd10 */
/* CALLEES: FUN_14003eb10, FUN_14003ed60, QString, append, fromAscii, number, setText, ~QString */

void FUN_14004ff40(longlong param_1)

{
  uint uVar1;
  uint uVar2;
  QString *pQVar3;
  QString local_res10 [8];
  QString local_res18 [8];
  QString local_res20 [8];
  QString local_28 [24];
  
  uVar1 = FUN_14003ed60();
  uVar2 = FUN_14003eb10();
  if (uVar1 < uVar2) {
    uVar2 = uVar1;
  }
  pQVar3 = (QString *)QString::number((uint)local_28,uVar2);
  QString::fromAscii((char *)local_res18,0x40077620);
  QString::append(local_res18,pQVar3);
  QString::QString(local_res10,local_res18);
  pQVar3 = (QString *)QString::fromAscii((char *)local_res20,0x400718b4);
  QString::append(local_res10,pQVar3);
  QString::~QString(local_res20);
  QLabel::setText(*(QLabel **)(param_1 + 0x28),local_res10);
  QString::~QString(local_res10);
  QString::~QString(local_res18);
  QString::~QString(local_28);
  return;
}


/* ================= FUN_140050030 @ 140050030  size=257  callers=1 callees=8 ================= */
/* CALLERS: FUN_14004fd10 */
/* CALLEES: FUN_140027140, FUN_140027700, FUN_140028380, FUN_14003eb00, __security_check_cookie, fromAscii_helper, setText, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140050030(longlong param_1)

{
  uint uVar1;
  char *pcVar2;
  undefined1 auStack_198 [32];
  Data *local_178;
  undefined1 local_170 [24];
  undefined1 local_158 [64];
  char local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_198;
  local_118[0] = '\0';
  uVar1 = FUN_14003eb00();
  if (uVar1 < 1000) {
    pcVar2 = "Samples/s";
  }
  else {
    uVar1 = uVar1 / 1000;
    pcVar2 = "kSamples/s";
  }
  FUN_140027140(local_170,pcVar2,0x10);
  FUN_140028380(local_158,0x40,&DAT_140077644,uVar1);
  FUN_140027700(local_118,local_158,0x100);
  FUN_140027700(local_118,local_170,0x100);
  local_178 = QString::fromAscii_helper(local_118,-1);
  QLabel::setText(*(QLabel **)(param_1 + 0x38),(QString *)&local_178);
  QString::~QString((QString *)&local_178);
  return;
}


/* ================= FUN_140050140 @ 140050140  size=253  callers=1 callees=11 ================= */
/* CALLERS: FUN_14004d6c0 */
/* CALLEES: FUN_140036f30, FUN_140050340, FUN_1400503f0, FUN_1400507f0, FUN_140058d94, QDialog, QGridLayout, fromAscii_helper, setLayout, setWindowTitle, ~QString */

QWidget * FUN_140050140(QWidget *param_1,undefined8 param_2)

{
  QGridLayout *pQVar1;
  undefined4 local_res10 [2];
  Data *local_res18;
  
  local_res10[0] = 0;
  QDialog::QDialog((QDialog *)param_1,param_2,local_res10);
  *(undefined ***)param_1 = SymbolSelection::vftable;
  *(undefined ***)(param_1 + 0x10) = SymbolSelection::vftable;
  *(undefined8 *)(param_1 + 0x28) = 0;
  *(undefined8 *)(param_1 + 0x30) = 0;
  *(undefined8 *)(param_1 + 0x38) = 0;
  *(undefined8 *)(param_1 + 0x40) = 0;
  *(undefined8 *)(param_1 + 0x48) = 0;
  *(undefined8 *)(param_1 + 0x50) = 0;
  *(undefined8 *)(param_1 + 0x58) = param_2;
  local_res18 = QString::fromAscii_helper("Select Symbols",-1);
  QWidget::setWindowTitle(param_1,(QString *)&local_res18);
  QString::~QString((QString *)&local_res18);
  FUN_140036f30(param_1);
  pQVar1 = (QGridLayout *)FUN_140058d94(0x20);
  local_res18 = (Data *)pQVar1;
  if (pQVar1 == (QGridLayout *)0x0) {
    pQVar1 = (QGridLayout *)0x0;
  }
  else {
    QGridLayout::QGridLayout(pQVar1,param_1);
    *(undefined ***)pQVar1 = QGridLayout::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QGridLayout::vftable;
  }
  *(QGridLayout **)(param_1 + 0x28) = pQVar1;
  QWidget::setLayout(param_1,(QLayout *)pQVar1);
  FUN_140050340(param_1);
  FUN_1400507f0(param_1);
  FUN_1400503f0(param_1);
  return param_1;
}


/* ================= FUN_140050240 @ 140050240  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_140050300 */

void FUN_140050240(QDialog *param_1)

{
  *(undefined ***)param_1 = SymbolSelection::vftable;
  *(undefined ***)(param_1 + 0x10) = SymbolSelection::vftable;
                    /* WARNING: Could not recover jumptable at 0x000140050255. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::~QDialog(param_1);
  return;
}


/* ================= FUN_14005025c @ 14005025c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140050280 */

void FUN_14005025c(longlong param_1)

{
  FUN_140050280(param_1 + -0x10);
  return;
}


/* ================= FUN_140050268 @ 140050268  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140050300 */

void FUN_140050268(longlong param_1)

{
  FUN_140050300(param_1 + -0x10);
  return;
}


/* ================= FUN_140050280 @ 140050280  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_14005025c */
/* CALLEES: free, ~QTreeWidget */

QTreeWidget * FUN_140050280(QTreeWidget *param_1,uint param_2)

{
  QTreeWidget::~QTreeWidget(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_1400502c0 @ 1400502c0  size=53  callers=0 callees=2 ================= */
/* CALLEES: free, ~QTreeWidgetItem */

QTreeWidgetItem * FUN_1400502c0(QTreeWidgetItem *param_1,uint param_2)

{
  QTreeWidgetItem::~QTreeWidgetItem(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140050300 @ 140050300  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_140050268 */
/* CALLEES: FUN_140050240, free */

void * FUN_140050300(void *param_1,ulonglong param_2)

{
  FUN_140050240();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140050340 @ 140050340  size=167  callers=1 callees=6 ================= */
/* CALLERS: FUN_140050140 */
/* CALLEES: FUN_1400360a0, addWidget, connect, fromAscii_helper, setPlaceholderText, ~QString */

void FUN_140050340(QObject *param_1)

{
  QObject *pQVar1;
  Data *local_res8 [4];
  
  pQVar1 = (QObject *)FUN_1400360a0(param_1,0,0);
  *(QObject **)(param_1 + 0x38) = pQVar1;
  QObject::connect(pQVar1,"2textChanged(const QString&)",param_1,
                   "1_SlotOnTextChangedLineEditSearch(const QString&)",0);
  local_res8[0] = QString::fromAscii_helper("Filter symbols",-1);
  QLineEdit::setPlaceholderText(*(QLineEdit **)(param_1 + 0x38),(QString *)local_res8);
  QString::~QString((QString *)local_res8);
  local_res8[0] = (Data *)((ulonglong)local_res8[0] & 0xffffffff00000000);
  QGridLayout::addWidget
            (*(QGridLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x38),0,0,1,5,local_res8);
  return;
}


/* ================= FUN_1400503f0 @ 1400503f0  size=210  callers=1 callees=3 ================= */
/* CALLERS: FUN_140050140 */
/* CALLEES: FUN_140035c20, addWidget, setAutoDefault */

void FUN_1400503f0(longlong param_1)

{
  undefined8 uVar1;
  undefined4 local_res8 [8];
  
  uVar1 = FUN_140035c20(param_1,&DAT_14006cc0c,param_1,"1_SlotOnReleaseBtnOK(void)");
  *(undefined8 *)(param_1 + 0x40) = uVar1;
  uVar1 = FUN_140035c20(param_1,"Cancel",param_1,"1_SlotOnReleaseBtnCancel(void)");
  *(undefined8 *)(param_1 + 0x48) = uVar1;
  QPushButton::setAutoDefault(*(QPushButton **)(param_1 + 0x40),false);
  QPushButton::setAutoDefault(*(QPushButton **)(param_1 + 0x48),false);
  local_res8[0] = 0;
  QGridLayout::addWidget
            (*(QGridLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x40),2,1,1,1,local_res8);
  local_res8[0] = 0;
  QGridLayout::addWidget
            (*(QGridLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x48),2,3,1,1,local_res8);
  return;
}


/* ================= FUN_1400504d0 @ 1400504d0  size=655  callers=2 callees=17 ================= */
/* CALLERS: FUN_1400507f0, FUN_140050c30 */
/* CALLEES: FUN_140028380, FUN_140050760, FUN_1400512f0, FUN_140058d94, QColor, QTreeWidgetItem, QVariant, __security_check_cookie, fromAscii_helper, setBackgroundColor, setCheckState, setData, setDisabled, setItemExpanded, setText, ~QString, ~QVariant */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

QTreeWidgetItem * FUN_1400504d0(undefined8 param_1,longlong param_2)

{
  int iVar1;
  QTreeWidgetItem *pQVar2;
  CheckState CVar3;
  undefined1 auStack_58 [32];
  QTreeWidgetItem *local_38 [2];
  char local_28 [16];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_58;
  if (param_2 == 0) {
LAB_140050740:
    pQVar2 = (QTreeWidgetItem *)0x0;
  }
  else {
    pQVar2 = (QTreeWidgetItem *)FUN_140058d94(0x40);
    local_38[0] = pQVar2;
    if (pQVar2 == (QTreeWidgetItem *)0x0) {
      pQVar2 = (QTreeWidgetItem *)0x0;
    }
    else {
      QTreeWidgetItem::QTreeWidgetItem(pQVar2,0);
      *(undefined ***)pQVar2 = QTreeWidgetItem::vftable;
    }
    if (*(QTreeWidget **)(pQVar2 + 0x18) != (QTreeWidget *)0x0) {
      QTreeWidget::setItemExpanded(*(QTreeWidget **)(pQVar2 + 0x18),pQVar2,false);
    }
    if (*(char *)(param_2 + 4) == '\0') {
      if (*(char *)(param_2 + 0x210) == '\0') goto LAB_140050740;
      local_38[0] = (QTreeWidgetItem *)QString::fromAscii_helper((char *)(param_2 + 0x210),-1);
      QTreeWidgetItem::setText(pQVar2,0,(QString *)local_38);
      QString::~QString((QString *)local_38);
      QColor::QColor((QColor *)local_38,6);
      QTreeWidgetItem::setBackgroundColor(pQVar2,0,(QColor *)local_38);
    }
    else {
      local_38[0] = (QTreeWidgetItem *)QString::fromAscii_helper((char *)(param_2 + 4),-1);
      QTreeWidgetItem::setText(pQVar2,0,(QString *)local_38);
      QString::~QString((QString *)local_38);
    }
    if (*(char *)(param_2 + 0x110) != '\0') {
      local_38[0] = (QTreeWidgetItem *)QString::fromAscii_helper((char *)(param_2 + 0x110),-1);
      QTreeWidgetItem::setText(pQVar2,1,(QString *)local_38);
      QString::~QString((QString *)local_38);
    }
    if (*(uint *)(param_2 + 0x31c) != 0) {
      QVariant::QVariant((QVariant *)local_38,*(uint *)(param_2 + 0x31c));
      (**(code **)(*(longlong *)pQVar2 + 0x18))(pQVar2,2,0,local_38);
      QVariant::~QVariant((QVariant *)local_38);
    }
    if (*(int *)(param_2 + 0x318) != 0) {
      FUN_140028380(local_28,0x10,&DAT_140071900);
      local_38[0] = (QTreeWidgetItem *)QString::fromAscii_helper(local_28,-1);
      QTreeWidgetItem::setText(pQVar2,3,(QString *)local_38);
      QString::~QString((QString *)local_38);
    }
    if (*(char *)(param_2 + 0x35f) != '\0') {
      iVar1 = FUN_1400512f0(param_1,param_2);
      if (iVar1 == 0) {
        CVar3 = 0;
      }
      else {
        FUN_140050760(param_1,param_2);
        CVar3 = 2;
      }
      QTreeWidgetItem::setCheckState(pQVar2,4,CVar3);
    }
    iVar1 = *(int *)(param_2 + 0x314);
    if ((((iVar1 == 3) || (iVar1 == 4)) || (iVar1 == 9)) || (iVar1 == 0xd)) {
      QTreeWidgetItem::setDisabled(pQVar2,true);
      local_38[0] = (QTreeWidgetItem *)QString::fromAscii_helper("Symbol type is not supported",-1);
      QTreeWidgetItem::setText(pQVar2,4,(QString *)local_38);
      QString::~QString((QString *)local_38);
    }
    *(QTreeWidgetItem **)(param_2 + 0x360) = pQVar2;
  }
  return pQVar2;
}


/* ================= FUN_140050760 @ 140050760  size=137  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400504d0, FUN_140051690 */
/* CALLEES: FUN_140043d10 */

void FUN_140050760(longlong param_1,undefined8 param_2)

{
  longlong lVar1;
  longlong lVar2;
  
  lVar1 = *(longlong *)(param_1 + 0x50);
  if (lVar1 != 0) {
    for (lVar2 = *(longlong *)(lVar1 + 8); lVar2 != 0; lVar2 = *(longlong *)(lVar2 + 8)) {
      lVar1 = lVar2;
    }
    lVar2 = FUN_140043d10(0x10);
    *(longlong *)(lVar1 + 8) = lVar2;
    *(undefined8 *)(lVar2 + 8) = 0;
    **(undefined8 **)(lVar1 + 8) = param_2;
    return;
  }
  lVar1 = FUN_140043d10(0x10);
  *(longlong *)(param_1 + 0x50) = lVar1;
  *(undefined8 *)(lVar1 + 8) = 0;
  **(undefined8 **)(param_1 + 0x50) = param_2;
  return;
}


/* ================= FUN_1400507f0 @ 1400507f0  size=1085  callers=1 callees=27 ================= */
/* CALLERS: FUN_140050140 */
/* CALLEES: FUN_140030840, FUN_14004d170, FUN_1400504d0, FUN_140050c30, FUN_1400510a0, FUN_140051740, FUN_140058d94, QTreeWidget, addChildren, addWidget, columnCount, columnWidth, connect, fromAscii_helper, header, invisibleRootItem, qFree, resize, resizeColumnToContents, setColumnCount, setColumnWidth, setEditTriggers, setFirstItemColumnSpanned, setHeaderLabels, setItemExpanded, setSortingEnabled, ~QString */

void FUN_1400507f0(QWidget *param_1)

{
  QString *pQVar1;
  code *pcVar2;
  int iVar3;
  int iVar4;
  QTreeWidget *this;
  undefined8 uVar5;
  QTreeWidgetItem *pQVar6;
  QHeaderView *pQVar7;
  QString *this_00;
  longlong lVar8;
  longlong lVar9;
  uint uVar10;
  undefined **ppuVar11;
  uint uVar13;
  int local_res8;
  QTreeWidget *local_res10;
  undefined4 local_res18 [2];
  code *local_res20;
  code *local_68;
  Data *local_60;
  int local_58;
  int local_54;
  longlong local_50 [2];
  QTreeWidget *pQVar12;
  QTreeWidget *pQVar14;
  
  local_68 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  local_res20 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  local_res8 = 0x3c;
  this = (QTreeWidget *)FUN_140058d94(0x28);
  pQVar14 = (QTreeWidget *)0x0;
  pQVar12 = pQVar14;
  local_res10 = this;
  if (this != (QTreeWidget *)0x0) {
    QTreeWidget::QTreeWidget(this,param_1);
    *(undefined ***)this = QTreeWidget::vftable;
    *(undefined ***)(this + 0x10) = QTreeWidget::vftable;
    pQVar12 = this;
  }
  *(QTreeWidget **)(param_1 + 0x30) = pQVar12;
  ppuVar11 = &PTR_DAT_140077690;
  lVar8 = 5;
  do {
    local_60 = QString::fromAscii_helper(*ppuVar11,-1);
    FUN_140030840(&local_68,&local_60);
    QString::~QString((QString *)&local_60);
    ppuVar11 = ppuVar11 + 2;
    lVar8 = lVar8 + -1;
  } while (lVar8 != 0);
  QTreeWidget::setColumnCount(*(QTreeWidget **)(param_1 + 0x30),5);
  QTreeWidget::setHeaderLabels(*(QTreeWidget **)(param_1 + 0x30),(QStringList *)&local_68);
  QTreeWidget::setSortingEnabled(*(QTreeWidget **)(param_1 + 0x30),true);
  uVar10 = 0;
  local_res10 = (QTreeWidget *)((ulonglong)local_res10 & 0xffffffff00000000);
  QAbstractItemView::setEditTriggers(*(QAbstractItemView **)(param_1 + 0x30),&local_res10);
  lVar9 = DAT_14286d3b0;
  pQVar12 = pQVar14;
  for (lVar8 = DAT_14286d3b0; lVar8 != 0; lVar8 = *(longlong *)(lVar8 + 0x3e0)) {
    uVar10 = (int)pQVar12 + 1;
    pQVar12 = (QTreeWidget *)(ulonglong)uVar10;
  }
  uVar5 = FUN_14004d170(*(undefined8 *)(param_1 + 0x58),0,"Preparing Symbols ...",uVar10,0);
  if (uVar10 != 0) {
    do {
      FUN_14004d170(*(undefined8 *)(param_1 + 0x58),uVar5,0,uVar10,(int)pQVar14);
      lVar8 = FUN_1400504d0(param_1,lVar9);
      local_50[0] = lVar8;
      if (lVar8 != 0) {
        FUN_140051740(&local_res20,local_50);
      }
      FUN_140050c30(param_1,lVar8);
      lVar9 = *(longlong *)(lVar9 + 0x3e0);
      uVar13 = (int)pQVar14 + 1;
      pQVar14 = (QTreeWidget *)(ulonglong)uVar13;
    } while (uVar13 < uVar10);
  }
  uVar13 = 0;
  if (*(int *)(local_res20 + 0xc) != *(int *)(local_res20 + 8)) {
    pQVar6 = QTreeWidget::invisibleRootItem(*(QTreeWidget **)(param_1 + 0x30));
    QTreeWidgetItem::addChildren(pQVar6,(QList<QTreeWidgetItem*___ptr64> *)&local_res20);
  }
  FUN_14004d170(*(undefined8 *)(param_1 + 0x58),uVar5,0,uVar10,0xffffffff);
  lVar8 = DAT_14286d3b0;
  uVar5 = FUN_14004d170(*(undefined8 *)(param_1 + 0x58),0,"Finishing Symbols ...",uVar10,0);
  if (uVar10 != 0) {
    do {
      FUN_14004d170(*(undefined8 *)(param_1 + 0x58),uVar5,0,uVar10,uVar13);
      pQVar6 = *(QTreeWidgetItem **)(lVar8 + 0x360);
      if (((*(char *)(lVar8 + 4) == '\0') && (*(char *)(lVar8 + 0x210) != '\0')) &&
         (*(QTreeWidget **)(pQVar6 + 0x18) != (QTreeWidget *)0x0)) {
        QTreeWidget::setFirstItemColumnSpanned(*(QTreeWidget **)(pQVar6 + 0x18),pQVar6,true);
        if (*(QTreeWidget **)(pQVar6 + 0x18) != (QTreeWidget *)0x0) {
          QTreeWidget::setItemExpanded(*(QTreeWidget **)(pQVar6 + 0x18),pQVar6,true);
        }
      }
      FUN_1400510a0(param_1,*(undefined8 *)(lVar8 + 0x3d8));
      lVar8 = *(longlong *)(lVar8 + 0x3e0);
      uVar13 = uVar13 + 1;
    } while (uVar13 < uVar10);
  }
  FUN_14004d170(*(undefined8 *)(param_1 + 0x58),uVar5,0,uVar10,0xffffffff);
  uVar10 = 0;
  iVar3 = QTreeWidget::columnCount(*(QTreeWidget **)(param_1 + 0x30));
  if (iVar3 != 0) {
    local_res8 = 0x3c;
    do {
      QTreeView::resizeColumnToContents(*(QTreeView **)(param_1 + 0x30),uVar10);
      iVar3 = *(int *)(&DAT_140077698 + (ulonglong)uVar10 * 0x10);
      iVar4 = QTreeView::columnWidth(*(QTreeView **)(param_1 + 0x30),uVar10);
      if (iVar3 < iVar4) {
        QTreeView::setColumnWidth(*(QTreeView **)(param_1 + 0x30),uVar10,iVar3);
      }
      iVar3 = QTreeView::columnWidth(*(QTreeView **)(param_1 + 0x30),uVar10);
      local_res8 = local_res8 + iVar3;
      uVar10 = uVar10 + 1;
      uVar13 = QTreeWidget::columnCount(*(QTreeWidget **)(param_1 + 0x30));
    } while (uVar10 < uVar13);
  }
  QObject::connect(*(QObject **)(param_1 + 0x30),"2itemChanged(QTreeWidgetItem*, int)",
                   (QObject *)param_1,"1_SlotOnTreeItemChanged(QTreeWidgetItem*, int)",0);
  QObject::connect(*(QObject **)(param_1 + 0x30),"2itemDoubleClicked(QTreeWidgetItem*, int)",
                   (QObject *)param_1,"1_SlotOnTreeWidgetItemDoubleClicked(QTreeWidgetItem*, int)",0
                  );
  local_res18[0] = 0;
  QGridLayout::addWidget
            (*(QGridLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x30),1,0,1,5,local_res18);
  pQVar7 = QTreeView::header(*(QTreeView **)(param_1 + 0x30));
  local_54 = ((*(int *)(*(longlong *)(pQVar7 + 0x20) + 0x20) -
              *(int *)(*(longlong *)(pQVar7 + 0x20) + 0x18)) + 1) * 0x14;
  local_58 = local_res8;
  QWidget::resize(param_1,(QSize *)&local_58);
  LOCK();
  iVar3 = *(int *)local_res20;
  *(int *)local_res20 = *(int *)local_res20 + -1;
  UNLOCK();
  if (iVar3 == 1) {
    qFree(local_res20);
  }
  pcVar2 = local_68;
  LOCK();
  iVar3 = *(int *)local_68;
  *(int *)local_68 = *(int *)local_68 + -1;
  UNLOCK();
  if (iVar3 == 1) {
    pQVar1 = (QString *)(local_68 + ((longlong)*(int *)(local_68 + 8) + 3) * 8);
    this_00 = (QString *)(local_68 + ((longlong)*(int *)(local_68 + 0xc) + 3) * 8);
    while (this_00 != pQVar1) {
      this_00 = this_00 + -8;
      QString::~QString(this_00);
    }
    qFree(pcVar2);
  }
  return;
}


/* ================= FUN_140050c30 @ 140050c30  size=193  callers=2 callees=5 ================= */
/* CALLERS: FUN_1400507f0, FUN_140050c30 */
/* CALLEES: FUN_1400504d0, FUN_140050c30, FUN_140051740, addChildren, qFree */

void FUN_140050c30(undefined8 param_1,QTreeWidgetItem *param_2,longlong param_3)

{
  int iVar1;
  longlong lVar2;
  code *local_res10;
  longlong local_res18 [2];
  
  local_res10 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  for (; param_3 != 0; param_3 = *(longlong *)(param_3 + 0x3e0)) {
    lVar2 = FUN_1400504d0(param_1,param_3);
    local_res18[0] = lVar2;
    if (lVar2 != 0) {
      FUN_140051740(&local_res10,local_res18);
      FUN_140050c30(param_1,lVar2,*(undefined8 *)(param_3 + 0x3d8));
    }
  }
  if ((param_2 != (QTreeWidgetItem *)0x0) &&
     (*(int *)(local_res10 + 0xc) != *(int *)(local_res10 + 8))) {
    QTreeWidgetItem::addChildren(param_2,(QList<QTreeWidgetItem*___ptr64> *)&local_res10);
  }
  LOCK();
  iVar1 = *(int *)local_res10;
  *(int *)local_res10 = *(int *)local_res10 + -1;
  UNLOCK();
  if (iVar1 == 1) {
    qFree(local_res10);
  }
  return;
}


/* ================= FUN_140050d00 @ 140050d00  size=304  callers=1 callees=5 ================= */
/* CALLERS: FUN_1400513a0 */
/* CALLEES: FUN_140028380, FUN_140036ec0, FUN_140051020, __security_check_cookie, text */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_140050d00(undefined8 param_1,longlong param_2,undefined8 param_3)

{
  longlong lVar1;
  undefined8 uVar2;
  undefined1 auStack_118 [32];
  undefined1 local_f8 [16];
  undefined1 local_e8 [208];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_118;
  lVar1 = FUN_140051020(param_1,param_2 + 0x210);
  if ((lVar1 == 0) && (lVar1 = FUN_140051020(param_1,param_2 + 4,param_3), lVar1 == 0)) {
    if (*(QTreeWidgetItem **)(param_2 + 0x360) == (QTreeWidgetItem *)0x0) {
      return 0;
    }
    uVar2 = QTreeWidgetItem::text(*(QTreeWidgetItem **)(param_2 + 0x360),(int)local_f8);
    FUN_140036ec0(local_e8,uVar2,200);
    lVar1 = FUN_140051020(param_1,local_e8,param_3);
    if (lVar1 == 0) {
      FUN_140028380(local_e8,200,&DAT_140077db0,*(undefined4 *)(param_2 + 0x31c));
      lVar1 = FUN_140051020(param_1,local_e8,param_3);
      if (lVar1 == 0) {
        FUN_140028380(local_e8,200,&DAT_140071900,*(undefined4 *)(param_2 + 0x318));
        lVar1 = FUN_140051020(param_1,local_e8,param_3);
        if (lVar1 == 0) {
          return 0;
        }
      }
    }
  }
  return 1;
}


/* ================= FUN_140050e30 @ 140050e30  size=293  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400513a0 */
/* CALLEES: FUN_14003f6a0, FUN_14003f6d0, FUN_14003f720, FUN_14003f760, setItemExpanded, setItemHidden */

void FUN_140050e30(undefined8 param_1,longlong param_2)

{
  QTreeWidgetItem *pQVar1;
  undefined4 *puVar2;
  longlong local_res10 [3];
  undefined4 local_38;
  undefined4 uStack_34;
  int iStack_30;
  undefined4 uStack_2c;
  undefined8 local_28;
  undefined1 local_20 [24];
  
  if (param_2 != 0) {
    local_res10[0] = param_2;
    puVar2 = (undefined4 *)FUN_14003f6d0(local_20,8,800);
    local_38 = *puVar2;
    uStack_34 = puVar2[1];
    iStack_30 = puVar2[2];
    uStack_2c = puVar2[3];
    local_28 = *(undefined8 *)(puVar2 + 4);
    FUN_14003f760(&local_38,local_res10[0]);
    while( true ) {
      while( true ) {
        if ((local_res10[0] == 0) || (*(char *)(local_res10[0] + 4) == '\0'))
        goto joined_r0x000140050ee9;
        if (*(longlong *)(local_res10[0] + 1000) == 0) break;
        FUN_14003f760(&local_38);
        local_res10[0] = *(longlong *)(local_res10[0] + 1000);
      }
      if (*(longlong *)(local_res10[0] + 0x3e0) == 0) break;
      FUN_14003f760(&local_38);
      local_res10[0] = *(longlong *)(local_res10[0] + 0x3e0);
    }
    local_res10[0] = 0;
joined_r0x000140050ee9:
    while (iStack_30 != 0) {
      FUN_14003f720(&local_38,local_res10);
      pQVar1 = *(QTreeWidgetItem **)(local_res10[0] + 0x360);
      if ((pQVar1 != (QTreeWidgetItem *)0x0) &&
         (*(QTreeWidget **)(pQVar1 + 0x18) != (QTreeWidget *)0x0)) {
        QTreeWidget::setItemExpanded(*(QTreeWidget **)(pQVar1 + 0x18),pQVar1,true);
        if (*(QTreeWidget **)(pQVar1 + 0x18) != (QTreeWidget *)0x0) {
          QTreeWidget::setItemHidden(*(QTreeWidget **)(pQVar1 + 0x18),pQVar1,false);
        }
      }
    }
    FUN_14003f6a0(&local_38);
  }
  return;
}


/* ================= FUN_140050f60 @ 140050f60  size=179  callers=1 callees=5 ================= */
/* CALLERS: FUN_1400515f0 */
/* CALLEES: FUN_14003f6a0, FUN_14003f6d0, FUN_14003f720, FUN_14003f760, setItemExpanded */

void FUN_140050f60(undefined8 param_1,longlong param_2)

{
  QTreeWidgetItem *pQVar1;
  undefined4 *puVar2;
  longlong local_res10 [3];
  undefined4 local_38;
  undefined4 uStack_34;
  int iStack_30;
  undefined4 uStack_2c;
  undefined8 local_28;
  undefined1 local_20 [32];
  
  if (param_2 != 0) {
    local_res10[0] = param_2;
    puVar2 = (undefined4 *)FUN_14003f6d0(local_20,8,800);
    local_38 = *puVar2;
    uStack_34 = puVar2[1];
    iStack_30 = puVar2[2];
    uStack_2c = puVar2[3];
    local_28 = *(undefined8 *)(puVar2 + 4);
    FUN_14003f760(&local_38,local_res10[0]);
    while (iStack_30 != 0) {
      FUN_14003f720(&local_38,local_res10);
      pQVar1 = *(QTreeWidgetItem **)(local_res10[0] + 0x360);
      if ((pQVar1 != (QTreeWidgetItem *)0x0) &&
         (*(QTreeWidget **)(pQVar1 + 0x18) != (QTreeWidget *)0x0)) {
        QTreeWidget::setItemExpanded(*(QTreeWidget **)(pQVar1 + 0x18),pQVar1,true);
      }
      if (*(longlong *)(local_res10[0] + 0x3e0) != 0) {
        FUN_14003f760(&local_38);
      }
    }
    FUN_14003f6a0(&local_38);
  }
  return;
}


/* ================= FUN_140051020 @ 140051020  size=116  callers=1 callees=2 ================= */
/* CALLERS: FUN_140050d00 */
/* CALLEES: FUN_1400277b0, FUN_1400277f0 */

char * FUN_140051020(undefined8 param_1,char *param_2,longlong param_3)

{
  int iVar1;
  int iVar2;
  
  if (((param_2 != (char *)0x0) && (param_3 != 0)) &&
     (iVar1 = FUN_1400277b0(param_3), *param_2 != '\0')) {
    do {
      iVar2 = FUN_1400277f0(param_2,param_3,(longlong)iVar1);
      if (iVar2 == 0) {
        if (*param_2 == '\0') {
          return (char *)0x0;
        }
        return param_2;
      }
      param_2 = param_2 + 1;
    } while (*param_2 != '\0');
  }
  return (char *)0x0;
}


/* ================= FUN_1400510a0 @ 1400510a0  size=144  callers=2 callees=3 ================= */
/* CALLERS: FUN_1400507f0, FUN_1400510a0 */
/* CALLEES: FUN_1400510a0, setFirstItemColumnSpanned, setItemExpanded */

void FUN_1400510a0(undefined8 param_1,longlong param_2)

{
  QTreeWidgetItem *pQVar1;
  
  for (; param_2 != 0; param_2 = *(longlong *)(param_2 + 0x3e0)) {
    pQVar1 = *(QTreeWidgetItem **)(param_2 + 0x360);
    if ((((pQVar1 != (QTreeWidgetItem *)0x0) && (*(char *)(param_2 + 4) == '\0')) &&
        (*(char *)(param_2 + 0x210) != '\0')) &&
       (*(QTreeWidget **)(pQVar1 + 0x18) != (QTreeWidget *)0x0)) {
      QTreeWidget::setFirstItemColumnSpanned(*(QTreeWidget **)(pQVar1 + 0x18),pQVar1,true);
      if (*(QTreeWidget **)(pQVar1 + 0x18) != (QTreeWidget *)0x0) {
        QTreeWidget::setItemExpanded(*(QTreeWidget **)(pQVar1 + 0x18),pQVar1,true);
      }
    }
    FUN_1400510a0(param_1,*(undefined8 *)(param_2 + 0x3d8));
  }
  return;
}


/* ================= FUN_140051130 @ 140051130  size=189  callers=1 callees=3 ================= */
/* CALLERS: FUN_140051690 */
/* CALLEES: FUN_14003f6d0, FUN_14003f720, FUN_14003f760 */

longlong FUN_140051130(undefined8 param_1,longlong param_2,longlong param_3)

{
  undefined4 *puVar1;
  longlong local_res18 [2];
  undefined4 local_38;
  undefined4 uStack_34;
  int iStack_30;
  undefined4 uStack_2c;
  undefined8 local_28;
  undefined1 local_20 [24];
  
  if (param_3 != 0) {
    local_res18[0] = param_3;
    puVar1 = (undefined4 *)FUN_14003f6d0(local_20,8,800);
    local_38 = *puVar1;
    uStack_34 = puVar1[1];
    iStack_30 = puVar1[2];
    uStack_2c = puVar1[3];
    local_28 = *(undefined8 *)(puVar1 + 4);
    FUN_14003f760(&local_38,local_res18[0]);
    while (iStack_30 != 0) {
      FUN_14003f720(&local_38,local_res18);
      if (*(longlong *)(local_res18[0] + 0x360) == param_2) {
        return local_res18[0];
      }
      if (*(longlong *)(local_res18[0] + 0x3e0) != 0) {
        FUN_14003f760(&local_38);
      }
      if (*(longlong *)(local_res18[0] + 0x3d8) != 0) {
        FUN_14003f760(&local_38);
      }
    }
  }
  return 0;
}


/* ================= FUN_1400511f0 @ 1400511f0  size=255  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400515f0 */
/* CALLEES: FUN_14003f6a0, FUN_14003f6d0, FUN_14003f720, FUN_14003f760, setItemExpanded, setItemHidden */

void FUN_1400511f0(undefined8 param_1,longlong param_2)

{
  QTreeWidgetItem *pQVar1;
  undefined4 *puVar2;
  longlong local_res10 [3];
  undefined4 local_38;
  undefined4 uStack_34;
  int iStack_30;
  undefined4 uStack_2c;
  undefined8 local_28;
  undefined1 local_20 [24];
  
  if (param_2 != 0) {
    local_res10[0] = param_2;
    puVar2 = (undefined4 *)FUN_14003f6d0(local_20,8,800);
    local_38 = *puVar2;
    uStack_34 = puVar2[1];
    iStack_30 = puVar2[2];
    uStack_2c = puVar2[3];
    local_28 = *(undefined8 *)(puVar2 + 4);
    FUN_14003f760(&local_38,local_res10[0]);
    while (iStack_30 != 0) {
      FUN_14003f720(&local_38,local_res10);
      pQVar1 = *(QTreeWidgetItem **)(local_res10[0] + 0x360);
      if ((pQVar1 != (QTreeWidgetItem *)0x0) &&
         (*(QTreeWidget **)(pQVar1 + 0x18) != (QTreeWidget *)0x0)) {
        QTreeWidget::setItemHidden(*(QTreeWidget **)(pQVar1 + 0x18),pQVar1,true);
        if (*(QTreeWidget **)(pQVar1 + 0x18) != (QTreeWidget *)0x0) {
          QTreeWidget::setItemExpanded(*(QTreeWidget **)(pQVar1 + 0x18),pQVar1,false);
        }
      }
      if (*(longlong *)(local_res10[0] + 0x3d8) != 0) {
        FUN_14003f760(&local_38);
      }
      if (*(longlong *)(local_res10[0] + 0x3e0) != 0) {
        FUN_14003f760(&local_38);
      }
    }
    FUN_14003f6a0(&local_38);
  }
  return;
}


/* ================= FUN_1400512f0 @ 1400512f0  size=110  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400504d0 */
/* CALLEES: FUN_140027760 */

undefined8 FUN_1400512f0(undefined8 param_1,longlong param_2)

{
  longlong *plVar1;
  int iVar2;
  
  plVar1 = DAT_14286d3b8;
  while( true ) {
    if (plVar1 == (longlong *)0x0) {
      return 0;
    }
    if ((*(int *)(param_2 + 0x318) == *(int *)(*plVar1 + 0x318)) &&
       (iVar2 = FUN_140027760(*(undefined8 *)(param_2 + 0x108),*(undefined8 *)(*plVar1 + 0x108)),
       iVar2 == 0)) break;
    plVar1 = (longlong *)plVar1[1];
  }
  return 1;
}


/* ================= FUN_140051360 @ 140051360  size=55  callers=1 callees=1 ================= */
/* CALLERS: FUN_140051690 */
/* CALLEES: FUN_140043e60 */

void FUN_140051360(longlong param_1,longlong param_2)

{
  longlong lVar1;
  longlong *plVar2;
  longlong *plVar3;
  longlong *plVar4;
  
  lVar1 = **(longlong **)(param_1 + 0x50);
  plVar4 = *(longlong **)(param_1 + 0x50);
  plVar3 = (longlong *)0x0;
  while (plVar2 = plVar4, lVar1 != param_2) {
    plVar4 = (longlong *)plVar2[1];
    plVar3 = plVar2;
    lVar1 = *(longlong *)plVar2[1];
  }
  if (plVar3 == (longlong *)0x0) {
    *(longlong *)(param_1 + 0x50) = plVar2[1];
    FUN_140043e60();
    return;
  }
  plVar3[1] = plVar2[1];
  FUN_140043e60();
  return;
}


/* ================= FUN_1400513a0 @ 1400513a0  size=226  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400515f0 */
/* CALLEES: FUN_14003f6a0, FUN_14003f6d0, FUN_14003f720, FUN_14003f760, FUN_140050d00, FUN_140050e30 */

void FUN_1400513a0(undefined8 param_1,longlong param_2,undefined8 param_3)

{
  char cVar1;
  undefined4 *puVar2;
  longlong local_res10 [3];
  undefined4 local_38;
  undefined4 uStack_34;
  int iStack_30;
  undefined4 uStack_2c;
  undefined8 local_28;
  undefined1 local_20 [24];
  
  if (param_2 != 0) {
    local_res10[0] = param_2;
    puVar2 = (undefined4 *)FUN_14003f6d0(local_20,8,800);
    local_38 = *puVar2;
    uStack_34 = puVar2[1];
    iStack_30 = puVar2[2];
    uStack_2c = puVar2[3];
    local_28 = *(undefined8 *)(puVar2 + 4);
    FUN_14003f760(&local_38,local_res10[0]);
    while (iStack_30 != 0) {
      FUN_14003f720(&local_38,local_res10);
      cVar1 = FUN_140050d00(param_1,local_res10[0],param_3);
      if (cVar1 != '\0') {
        FUN_140050e30(param_1,local_res10[0]);
      }
      if (*(longlong *)(local_res10[0] + 0x3e0) != 0) {
        FUN_14003f760(&local_38);
      }
      if (*(longlong *)(local_res10[0] + 0x3d8) != 0) {
        FUN_14003f760(&local_38);
      }
    }
    FUN_14003f6a0(&local_38);
  }
  return;
}


/* ================= FUN_140051490 @ 140051490  size=255  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400515f0 */
/* CALLEES: FUN_14003f6a0, FUN_14003f6d0, FUN_14003f720, FUN_14003f760, setItemExpanded, setItemHidden */

void FUN_140051490(undefined8 param_1,longlong param_2)

{
  QTreeWidgetItem *pQVar1;
  undefined4 *puVar2;
  longlong local_res10 [3];
  undefined4 local_38;
  undefined4 uStack_34;
  int iStack_30;
  undefined4 uStack_2c;
  undefined8 local_28;
  undefined1 local_20 [24];
  
  if (param_2 != 0) {
    local_res10[0] = param_2;
    puVar2 = (undefined4 *)FUN_14003f6d0(local_20,8);
    local_38 = *puVar2;
    uStack_34 = puVar2[1];
    iStack_30 = puVar2[2];
    uStack_2c = puVar2[3];
    local_28 = *(undefined8 *)(puVar2 + 4);
    FUN_14003f760(&local_38,local_res10[0]);
    while (iStack_30 != 0) {
      FUN_14003f720(&local_38,local_res10);
      pQVar1 = *(QTreeWidgetItem **)(local_res10[0] + 0x360);
      if ((pQVar1 != (QTreeWidgetItem *)0x0) &&
         (*(QTreeWidget **)(pQVar1 + 0x18) != (QTreeWidget *)0x0)) {
        QTreeWidget::setItemHidden(*(QTreeWidget **)(pQVar1 + 0x18),pQVar1,false);
        if (*(QTreeWidget **)(pQVar1 + 0x18) != (QTreeWidget *)0x0) {
          QTreeWidget::setItemExpanded(*(QTreeWidget **)(pQVar1 + 0x18),pQVar1,false);
        }
      }
      if (*(longlong *)(local_res10[0] + 0x3e0) != 0) {
        FUN_14003f760(&local_38);
      }
      if (*(longlong *)(local_res10[0] + 0x3d8) != 0) {
        FUN_14003f760(&local_38);
      }
    }
    FUN_14003f6a0(&local_38);
  }
  return;
}


/* ================= FUN_140051590 @ 140051590  size=96  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003c240 */
/* CALLEES: FUN_14003df10, FUN_14003e880, FUN_14004dc70, FUN_14004e120 */

void FUN_140051590(longlong *param_1)

{
  undefined8 *puVar1;
  
  FUN_14003e880();
  for (puVar1 = (undefined8 *)param_1[10]; puVar1 != (undefined8 *)0x0;
      puVar1 = (undefined8 *)puVar1[1]) {
    FUN_14003df10(*puVar1);
  }
  FUN_14004dc70(DAT_14286d388);
  FUN_14004e120(DAT_14286d388);
                    /* WARNING: Could not recover jumptable at 0x0001400515e9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_1 + 0x198))(param_1);
  return;
}


/* ================= FUN_1400515f0 @ 1400515f0  size=149  callers=1 callees=7 ================= */
/* CALLERS: FUN_14003c240 */
/* CALLEES: FUN_140036ec0, FUN_140050f60, FUN_1400511f0, FUN_1400513a0, FUN_140051490, QString, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_1400515f0(undefined8 param_1,QString *param_2)

{
  undefined8 uVar1;
  undefined1 auStack_148 [32];
  QString local_128 [16];
  char local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_148;
  uVar1 = QString::QString(local_128,param_2);
  FUN_140036ec0(local_118,uVar1,0x100);
  if (local_118[0] == '\0') {
    FUN_140051490();
    FUN_140050f60(param_1,DAT_14286d3b0);
  }
  else {
    FUN_1400511f0(param_1,DAT_14286d3b0);
    FUN_1400513a0(param_1,DAT_14286d3b0,local_118);
  }
  return;
}


/* ================= FUN_140051690 @ 140051690  size=141  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003c240 */
/* CALLEES: FUN_140050760, FUN_140051130, FUN_140051360, checkState */

void FUN_140051690(undefined8 param_1,QTreeWidgetItem *param_2)

{
  CheckState CVar1;
  longlong lVar2;
  
  lVar2 = FUN_140051130(param_1,param_2,DAT_14286d3b0);
  if ((lVar2 != 0) && (*(longlong *)(lVar2 + 0x328) == 0)) {
    CVar1 = QTreeWidgetItem::checkState(param_2,4);
    if (CVar1 == 2) {
      FUN_140050760(param_1,lVar2);
      return;
    }
    CVar1 = QTreeWidgetItem::checkState(param_2,4);
    if (CVar1 == 0) {
      FUN_140051360(param_1,lVar2);
    }
  }
  return;
}


/* ================= FUN_140051720 @ 140051720  size=18  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003c240 */

void FUN_140051720(longlong param_1,QTreeWidgetItem *param_2,int param_3)

{
  if (param_3 == 4) {
                    /* WARNING: Could not recover jumptable at 0x00014005172a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    QTreeWidget::editItem(*(QTreeWidget **)(param_1 + 0x30),param_2,4);
    return;
  }
  return;
}


/* ================= FUN_140051740 @ 140051740  size=78  callers=2 callees=2 ================= */
/* CALLERS: FUN_1400507f0, FUN_140050c30 */
/* CALLEES: FUN_140030fe0, append */

void FUN_140051740(QListData *param_1,undefined8 *param_2)

{
  void *pvVar1;
  undefined8 *puVar2;
  void **ppvVar3;
  
  if (**(int **)param_1 != 1) {
    puVar2 = (undefined8 *)FUN_140030fe0(param_1,0x7fffffff,1);
    *puVar2 = *param_2;
    return;
  }
  pvVar1 = (void *)*param_2;
  ppvVar3 = QListData::append(param_1);
  *ppvVar3 = pvVar1;
  return;
}


/* ================= FUN_140051790 @ 140051790  size=2273  callers=1 callees=53 ================= */
/* CALLERS: FUN_14004c6a0 */
/* CALLEES: FUN_14002a290, FUN_140036410, FUN_1400497c0, FUN_1400532c0, FUN_140058d94, QAction, QFontMetrics, QGridLayout, QIcon, QKeySequence, QPixmap, QTableWidgetItem, QWidget, addAction, addWidget, connect, contentsMargins, font, fromAscii_helper, height, hide, horizontalHeader, itemDelegate, operator=, qFree, resize, resizeColumnsToContents, scaledToHeight, setAttribute, setColumnWidth, setContentsMargins, setContextMenuPolicy, setDefaultAlignment, setEditTriggers, setFocusPolicy, setHorizontalHeaderItem, setIcon, setLayout, setMinimumSectionSize, setParent, setRowCount, setShortcut, setSortIndicatorShown, setSortingEnabled, setStretchLastSection, setText, show, sizeHint, ~QFont, ~QFontMetrics, ~QKeySequence, ~QPixmap, ~QString */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

QWidget * FUN_140051790(QWidget *param_1,QWidget *param_2)

{
  QString *pQVar1;
  code *pcVar2;
  int iVar3;
  QGridLayout *this;
  QAbstractItemView *pQVar4;
  QTableWidgetItem *pQVar5;
  undefined8 *puVar6;
  QFont *pQVar7;
  QFontMetrics *this_00;
  QPixmap *pQVar8;
  QIcon *pQVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  QHeaderView *pQVar12;
  QAction *pQVar13;
  QAction *pQVar14;
  QKeySequence *pQVar15;
  QAbstractItemDelegate *pQVar16;
  QSize *pQVar17;
  uint uVar18;
  TransformationMode TVar19;
  QGridLayout *this_01;
  QTableWidgetItem *pQVar20;
  QAction *pQVar21;
  int *piVar22;
  QString *this_02;
  QGridLayout *local_res20;
  QTableWidgetItem *local_118;
  undefined4 local_110;
  undefined4 local_10c;
  undefined8 local_108;
  undefined8 uStack_100;
  Data *local_f8;
  code *local_f0;
  QFontMetrics local_e8 [8];
  QKeySequence local_e0 [8];
  QKeySequence local_d8 [8];
  QKeySequence local_d0 [8];
  QKeySequence local_c8 [8];
  QKeySequence local_c0 [8];
  QPixmap local_b8 [32];
  QFont local_98 [16];
  QPixmap local_88 [24];
  QPixmap local_70 [40];
  undefined8 local_48;
  undefined8 uStack_40;
  
  pQVar21 = (QAction *)0x0;
  QWidget::QWidget(param_1,0);
  *(undefined ***)param_1 = TableWidget::vftable;
  *(undefined ***)(param_1 + 0x10) = TableWidget::vftable;
  local_f0 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  QPixmap::QPixmap(local_b8);
  *(undefined8 *)(param_1 + 0x28) = 0;
  *(undefined8 *)(param_1 + 0x30) = 0;
  *(undefined8 *)(param_1 + 0x38) = 0;
  *(undefined8 *)(param_1 + 0x40) = 0;
  *(undefined8 *)(param_1 + 0x48) = 0;
  *(undefined8 *)(param_1 + 0x50) = 0;
  *(undefined8 *)(param_1 + 0x58) = 0;
  *(undefined8 *)(param_1 + 0x60) = 0;
  *(undefined8 *)(param_1 + 0x68) = 0;
  *(undefined8 *)(param_1 + 0x70) = 0;
  *(undefined8 *)(param_1 + 0x78) = 0;
  *(undefined8 *)(param_1 + 0x80) = 0;
  *(undefined8 *)(param_1 + 0x88) = 0;
  *(undefined8 *)(param_1 + 0x90) = 0;
  *(undefined8 *)(param_1 + 0x98) = 0;
  *(undefined8 *)(param_1 + 0xa0) = 0;
  *(undefined8 *)(param_1 + 0xa8) = 0;
  *(undefined8 *)(param_1 + 0xb0) = 0;
  _DAT_14286d398 = param_1;
  QWidget::setParent(param_1,param_2);
  QWidget::hide(param_1);
  this = (QGridLayout *)FUN_140058d94(0x20);
  this_01 = (QGridLayout *)pQVar21;
  local_res20 = this;
  if (this != (QGridLayout *)0x0) {
    QGridLayout::QGridLayout(this,param_1);
    *(undefined ***)this = QGridLayout::vftable;
    *(undefined ***)(this + 0x10) = QGridLayout::vftable;
    this_01 = this;
  }
  QLayout::setContentsMargins((QLayout *)this_01,0,0,0,0);
  pQVar4 = (QAbstractItemView *)FUN_140036410(param_1,&PTR_DAT_140096df0,0x11);
  *(QAbstractItemView **)(param_1 + 0x28) = pQVar4;
  local_res20 = (QGridLayout *)CONCAT44(local_res20._4_4_,2);
  QAbstractItemView::setEditTriggers(pQVar4,&local_res20);
  local_110 = 0;
  QGridLayout::addWidget(this_01,*(undefined8 *)(param_1 + 0x28),0,0,&local_110);
  QWidget::setLayout(param_1,(QLayout *)this_01);
  QTableWidget::setRowCount(*(QTableWidget **)(param_1 + 0x28),1);
  piVar22 = &DAT_140096f00;
  pQVar9 = (QIcon *)pQVar21;
  do {
    QTableView::setColumnWidth(*(QTableView **)(param_1 + 0x28),(int)pQVar9,*piVar22);
    uVar18 = (int)pQVar9 + 1;
    pQVar9 = (QIcon *)(ulonglong)uVar18;
    piVar22 = piVar22 + 1;
  } while (uVar18 < 0xe);
  pQVar5 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar20 = (QTableWidgetItem *)pQVar21;
  local_118 = pQVar5;
  if (pQVar5 != (QTableWidgetItem *)0x0) {
    QTableWidgetItem::QTableWidgetItem(pQVar5,0);
    *(undefined ***)pQVar5 = QTableWidgetItem::vftable;
    pQVar20 = pQVar5;
  }
  local_f8 = QString::fromAscii_helper("Y",-1);
  QTableWidgetItem::setText(pQVar20,(QString *)&local_f8);
  QString::~QString((QString *)&local_f8);
  puVar6 = (undefined8 *)QWidget::contentsMargins(*(QWidget **)(param_1 + 0x28));
  local_108 = (QAction *)*puVar6;
  uStack_100 = puVar6[1];
  local_48 = local_108;
  uStack_40 = uStack_100;
  pQVar7 = (QFont *)QTableWidgetItem::font(pQVar20);
  this_00 = (QFontMetrics *)QFontMetrics::QFontMetrics(local_e8,pQVar7);
  iVar3 = QFontMetrics::height(this_00);
  TVar19 = (iVar3 - local_108._4_4_) - uStack_100._4_4_;
  QFontMetrics::~QFontMetrics(local_e8);
  QFont::~QFont(local_98);
  pQVar8 = (QPixmap *)FUN_14002a290(local_70,0x20);
  pQVar8 = (QPixmap *)QPixmap::scaledToHeight(pQVar8,(int)local_88,TVar19);
  QPixmap::operator=(local_b8,pQVar8);
  QPixmap::~QPixmap(local_88);
  QPixmap::~QPixmap(local_70);
  local_118 = (QTableWidgetItem *)FUN_140058d94(8);
  pQVar9 = (QIcon *)pQVar21;
  if (local_118 != (QTableWidgetItem *)0x0) {
    pQVar9 = (QIcon *)QIcon::QIcon((QIcon *)local_118,local_b8);
  }
  QTableWidgetItem::setIcon(pQVar20,pQVar9);
  QTableWidget::setHorizontalHeaderItem(*(QTableWidget **)(param_1 + 0x28),10,pQVar20);
  pQVar5 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar20 = (QTableWidgetItem *)pQVar21;
  local_118 = pQVar5;
  if (pQVar5 != (QTableWidgetItem *)0x0) {
    QTableWidgetItem::QTableWidgetItem(pQVar5,0);
    *(undefined ***)pQVar5 = QTableWidgetItem::vftable;
    pQVar20 = pQVar5;
  }
  *(QTableWidgetItem **)(param_1 + 0xa8) = pQVar20;
  pQVar5 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar20 = (QTableWidgetItem *)pQVar21;
  local_118 = pQVar5;
  if (pQVar5 != (QTableWidgetItem *)0x0) {
    QTableWidgetItem::QTableWidgetItem(pQVar5,0);
    *(undefined ***)pQVar5 = QTableWidgetItem::vftable;
    pQVar20 = pQVar5;
  }
  *(QTableWidgetItem **)(param_1 + 0xb0) = pQVar20;
  uVar11 = DAT_14286d3a0;
  uVar10 = FUN_1400497c0(DAT_14286d3a0,1);
  FUN_1400532c0(param_1,*(undefined8 *)(param_1 + 0xa8),uVar10);
  uVar11 = FUN_1400497c0(uVar11,2);
  FUN_1400532c0(param_1,*(undefined8 *)(param_1 + 0xb0),uVar11);
  QTableWidget::setHorizontalHeaderItem
            (*(QTableWidget **)(param_1 + 0x28),8,*(QTableWidgetItem **)(param_1 + 0xa8));
  QTableWidget::setHorizontalHeaderItem
            (*(QTableWidget **)(param_1 + 0x28),9,*(QTableWidgetItem **)(param_1 + 0xb0));
  QTableWidget::setSortingEnabled(*(QTableWidget **)(param_1 + 0x28),true);
  QWidget::setContentsMargins(*(QWidget **)(param_1 + 0x28),0,0,0,0);
  pQVar12 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x28));
  QHeaderView::setMinimumSectionSize(pQVar12,10);
  pQVar12 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x28));
  QHeaderView::setSortIndicatorShown(pQVar12,false);
  pQVar12 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x28));
  QWidget::setContentsMargins((QWidget *)pQVar12,0,0,0,0);
  local_10c = 1;
  pQVar12 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x28));
  QHeaderView::setDefaultAlignment(pQVar12,&local_10c);
  pQVar12 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x28));
  QHeaderView::setStretchLastSection(pQVar12,true);
  local_118 = (QTableWidgetItem *)CONCAT44(local_118._4_4_,0x85);
  pQVar12 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x28));
  QHeaderView::setDefaultAlignment(pQVar12,&local_118);
  pQVar13 = (QAction *)FUN_140058d94(0x10);
  pQVar14 = pQVar21;
  local_108 = pQVar13;
  if (pQVar13 != (QAction *)0x0) {
    QAction::QAction(pQVar13,(QObject *)param_1);
    *(undefined ***)pQVar13 = QAction::vftable;
    pQVar14 = pQVar13;
  }
  *(QAction **)(param_1 + 0x40) = pQVar14;
  pQVar13 = (QAction *)FUN_140058d94(0x10);
  pQVar14 = pQVar21;
  local_108 = pQVar13;
  if (pQVar13 != (QAction *)0x0) {
    QAction::QAction(pQVar13,(QObject *)param_1);
    *(undefined ***)pQVar13 = QAction::vftable;
    pQVar14 = pQVar13;
  }
  *(QAction **)(param_1 + 0x48) = pQVar14;
  pQVar13 = (QAction *)FUN_140058d94(0x10);
  pQVar14 = pQVar21;
  local_108 = pQVar13;
  if (pQVar13 != (QAction *)0x0) {
    QAction::QAction(pQVar13,(QObject *)param_1);
    *(undefined ***)pQVar13 = QAction::vftable;
    pQVar14 = pQVar13;
  }
  *(QAction **)(param_1 + 0x50) = pQVar14;
  pQVar13 = (QAction *)FUN_140058d94(0x10);
  pQVar14 = pQVar21;
  local_108 = pQVar13;
  if (pQVar13 != (QAction *)0x0) {
    QAction::QAction(pQVar13,(QObject *)param_1);
    *(undefined ***)pQVar13 = QAction::vftable;
    pQVar14 = pQVar13;
  }
  *(QAction **)(param_1 + 0x58) = pQVar14;
  pQVar14 = (QAction *)FUN_140058d94(0x10);
  local_108 = pQVar14;
  if (pQVar14 != (QAction *)0x0) {
    QAction::QAction(pQVar14,(QObject *)param_1);
    *(undefined ***)pQVar14 = QAction::vftable;
    pQVar21 = pQVar14;
  }
  *(QAction **)(param_1 + 0x60) = pQVar21;
  pQVar15 = (QKeySequence *)QKeySequence::QKeySequence(local_e0,0x400002b,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0x40),pQVar15);
  QKeySequence::~QKeySequence(local_e0);
  pQVar15 = (QKeySequence *)QKeySequence::QKeySequence(local_d8,0x400002d,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0x48),pQVar15);
  QKeySequence::~QKeySequence(local_d8);
  pQVar15 = (QKeySequence *)QKeySequence::QKeySequence(local_d0,0x2b,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0x50),pQVar15);
  QKeySequence::~QKeySequence(local_d0);
  pQVar15 = (QKeySequence *)QKeySequence::QKeySequence(local_c8,0x2d,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0x58),pQVar15);
  QKeySequence::~QKeySequence(local_c8);
  pQVar15 = (QKeySequence *)QKeySequence::QKeySequence(local_c0,0x1000007,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0x60),pQVar15);
  QKeySequence::~QKeySequence(local_c0);
  QObject::connect(*(QObject **)(param_1 + 0x40),"2triggered()",(QObject *)param_1,
                   "1_SlotOnReleasedYOffsetUp(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x48),"2triggered()",(QObject *)param_1,
                   "1_SlotOnReleasedYOffsetDown(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x50),"2triggered()",(QObject *)param_1,
                   "1_SlotOnReleasedYResUp(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x58),"2triggered()",(QObject *)param_1,
                   "1_SlotOnReleasedYResDown(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x60),"2triggered()",(QObject *)param_1,
                   "1_SlotOnReleasedRemoveSymbol(void)",0);
  QWidget::addAction(param_1,*(QAction **)(param_1 + 0x40));
  QWidget::addAction(param_1,*(QAction **)(param_1 + 0x48));
  QWidget::addAction(param_1,*(QAction **)(param_1 + 0x50));
  QWidget::addAction(param_1,*(QAction **)(param_1 + 0x58));
  QWidget::addAction(param_1,*(QAction **)(param_1 + 0x60));
  QWidget::setContextMenuPolicy(param_1,3);
  QWidget::setFocusPolicy(param_1,0);
  QObject::connect(*(QObject **)(param_1 + 0x28),"2itemChanged(QTableWidgetItem*)",
                   (QObject *)param_1,"1_SlotOnItemChanged(QTableWidgetItem*)",0);
  QObject::connect(*(QObject **)(param_1 + 0x28),"2itemClicked(QTableWidgetItem*)",
                   (QObject *)param_1,"1_SlotOnItemClicked(QTableWidgetItem*)",0);
  QObject::connect(*(QObject **)(param_1 + 0x28),"2itemDoubleClicked(QTableWidgetItem*)",
                   (QObject *)param_1,"1_SlotOnItemDblClicked(QTableWidgetItem*)",0);
  QObject::connect((QObject *)param_1,"2customContextMenuRequested(const QPoint &)",
                   (QObject *)param_1,"1_SlotOnContextMenuRequested(const QPoint &)",0);
  pQVar16 = QAbstractItemView::itemDelegate(*(QAbstractItemView **)(param_1 + 0x28));
  QObject::connect((QObject *)pQVar16,"2closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)",
                   (QObject *)param_1,
                   "1_SlotOnItemEditFinished(QWidget*, QAbstractItemDelegate::EndEditHint)",0);
  QWidget::setAttribute(*(QWidget **)(param_1 + 0x28),2,true);
  QTableView::resizeColumnsToContents(*(QTableView **)(param_1 + 0x28));
  pQVar17 = (QSize *)QWidget::sizeHint(param_1);
  QWidget::resize(param_1,pQVar17);
  QWidget::show(param_1);
  QPixmap::~QPixmap(local_b8);
  pcVar2 = local_f0;
  LOCK();
  iVar3 = *(int *)local_f0;
  *(int *)local_f0 = *(int *)local_f0 + -1;
  UNLOCK();
  if (iVar3 == 1) {
    pQVar1 = (QString *)(local_f0 + ((longlong)*(int *)(local_f0 + 8) + 3) * 8);
    this_02 = (QString *)(local_f0 + ((longlong)*(int *)(local_f0 + 0xc) + 3) * 8);
    while (this_02 != pQVar1) {
      this_02 = this_02 + -8;
      QString::~QString(this_02);
    }
    qFree(pcVar2);
  }
  return param_1;
}


/* ================= FUN_140052080 @ 140052080  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400520b0 */

void FUN_140052080(QWidget *param_1)

{
  *(undefined ***)param_1 = TableWidget::vftable;
  *(undefined ***)(param_1 + 0x10) = TableWidget::vftable;
                    /* WARNING: Could not recover jumptable at 0x000140052095. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::~QWidget(param_1);
  return;
}


/* ================= FUN_14005209c @ 14005209c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400520b0 */

void FUN_14005209c(longlong param_1)

{
  FUN_1400520b0(param_1 + -0x10);
  return;
}


/* ================= FUN_1400520b0 @ 1400520b0  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14005209c */
/* CALLEES: FUN_140052080, free */

void * FUN_1400520b0(void *param_1,ulonglong param_2)

{
  FUN_140052080();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_1400520f0 @ 1400520f0  size=27  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004d540 */
/* CALLEES: FUN_140053290 */

void FUN_1400520f0(longlong param_1)

{
  FUN_140053290();
  *(undefined1 *)(param_1 + 0xa0) = 0;
  return;
}


/* ================= FUN_140052110 @ 140052110  size=172  callers=3 callees=7 ================= */
/* CALLERS: FUN_14004d830, FUN_14004d870, FUN_14004d990 */
/* CALLEES: FUN_14003ede0, FUN_140053000, FUN_140054250, horizontalHeader, sortByColumn, sortIndicatorOrder, sortIndicatorSection */

void FUN_140052110(longlong param_1)

{
  int iVar1;
  SortOrder SVar2;
  int iVar3;
  QHeaderView *pQVar4;
  
  pQVar4 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x28));
  iVar1 = QHeaderView::sortIndicatorSection(pQVar4);
  pQVar4 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x28));
  SVar2 = QHeaderView::sortIndicatorOrder(pQVar4);
  QTableView::sortByColumn(*(QTableView **)(param_1 + 0x28),1,0);
  iVar3 = FUN_14003ede0();
  if (iVar3 != 0) {
    *(undefined1 *)(param_1 + 0xa0) = 0;
  }
  if (DAT_14286d3b8 == 0) {
    FUN_140053000(param_1);
  }
  else if (*(char *)(param_1 + 0xa0) == '\0') {
    FUN_140053000(param_1);
    *(undefined1 *)(param_1 + 0xa0) = 1;
  }
  else {
    FUN_140054250();
  }
                    /* WARNING: Could not recover jumptable at 0x0001400521b5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QTableView::sortByColumn(*(QTableView **)(param_1 + 0x28),iVar1,SVar2);
  return;
}


/* ================= FUN_1400521c0 @ 1400521c0  size=3294  callers=1 callees=43 ================= */
/* CALLERS: FUN_140053000 */
/* CALLEES: FUN_140053680, FUN_140058d94, QBrush, QComboBox, QHBoxLayout, QIcon, QPalette, QPushButton, QTableWidgetItem, QVariant, QWidget, addWidget, blockSignals, connect, count, flags, fromAscii_helper, horizontalHeader, insertItem, item, number, operator=, palette, resizeColumnsToContents, rowHeight, setAutoFillBackground, setBrush, setCellWidget, setCheckState, setContentsMargins, setCurrentIndex, setFixedSize, setFlags, setFlat, setItem, setPalette, setStretchLastSection, update, ~QBrush, ~QIcon, ~QPalette, ~QString, ~QVariant */

void FUN_1400521c0(QObject *param_1,longlong param_2,int param_3)

{
  int iVar1;
  QTableWidgetItem *pQVar2;
  QPushButton *pQVar3;
  QWidget *pQVar4;
  QHBoxLayout *pQVar5;
  QPalette *pQVar6;
  QBrush *pQVar7;
  QComboBox *this;
  QIcon *pQVar8;
  QTableWidgetItem *pQVar9;
  undefined4 *puVar10;
  QString *pQVar11;
  QTableWidgetItem *pQVar12;
  QHeaderView *this_00;
  char cVar13;
  uint uVar14;
  undefined8 uVar15;
  QComboBox *this_01;
  bool bVar16;
  double extraout_XMM0_Qa;
  undefined8 local_res8;
  QWidget *local_res10;
  Data *local_res20;
  QPalette local_58 [16];
  QVariant local_48 [16];
  
  local_res8 = (QTableWidgetItem *)((ulonglong)local_res8 & 0xffffffff00000000);
  QPalette::QPalette(local_58);
  pQVar2 = (QTableWidgetItem *)FUN_140058d94(0x30);
  local_res8 = pQVar2;
  if (pQVar2 == (QTableWidgetItem *)0x0) {
    pQVar2 = (QTableWidgetItem *)0x0;
  }
  else {
    QTableWidgetItem::QTableWidgetItem(pQVar2,0);
    *(undefined ***)pQVar2 = QTableWidgetItem::vftable;
  }
  local_res8 = (QTableWidgetItem *)CONCAT44(local_res8._4_4_,0x30);
  QTableWidgetItem::setFlags(pQVar2);
  QTableWidgetItem::setCheckState(pQVar2,-(uint)(*(char *)(param_2 + 0x35e) != '\0') & 2);
  QObject::blockSignals(*(QObject **)(param_1 + 0x28),true);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,0,pQVar2);
  QObject::blockSignals(*(QObject **)(param_1 + 0x28),false);
  pQVar3 = (QPushButton *)FUN_140058d94(0x28);
  local_res8 = (QTableWidgetItem *)pQVar3;
  if (pQVar3 == (QPushButton *)0x0) {
    pQVar3 = (QPushButton *)0x0;
  }
  else {
    QPushButton::QPushButton(pQVar3,(QWidget *)0x0);
    *(undefined ***)pQVar3 = QPushButton::vftable;
    *(undefined ***)(pQVar3 + 0x10) = QPushButton::vftable;
  }
  pQVar4 = (QWidget *)FUN_140058d94(0x28);
  local_res10 = pQVar4;
  if (pQVar4 == (QWidget *)0x0) {
    pQVar4 = (QWidget *)0x0;
  }
  else {
    local_res8 = (QTableWidgetItem *)((ulonglong)local_res8 & 0xffffffff00000000);
    QWidget::QWidget(pQVar4,0,&local_res8);
    *(undefined ***)pQVar4 = QWidget::vftable;
    *(undefined ***)(pQVar4 + 0x10) = QWidget::vftable;
  }
  pQVar5 = (QHBoxLayout *)FUN_140058d94(0x20);
  local_res10 = (QWidget *)pQVar5;
  if (pQVar5 == (QHBoxLayout *)0x0) {
    pQVar5 = (QHBoxLayout *)0x0;
  }
  else {
    QHBoxLayout::QHBoxLayout(pQVar5,pQVar4);
    *(undefined ***)pQVar5 = QHBoxLayout::vftable;
    *(undefined ***)(pQVar5 + 0x10) = QHBoxLayout::vftable;
  }
  local_res10 = (QWidget *)((ulonglong)local_res10 & 0xffffffff00000000);
  QBoxLayout::addWidget((QBoxLayout *)pQVar5);
  QLayout::setContentsMargins((QLayout *)pQVar5,0,0,0,0);
  iVar1 = QTableView::rowHeight(*(QTableView **)(param_1 + 0x28),0);
  QWidget::setFixedSize((QWidget *)pQVar3,iVar1 + -2,iVar1 + -2);
  pQVar6 = QWidget::palette((QWidget *)pQVar3);
  QPalette::operator=(local_58,pQVar6);
  if (*(QColor **)(param_2 + 0x378) != (QColor *)0x0) {
    pQVar7 = (QBrush *)QBrush::QBrush((QBrush *)&local_res10,*(QColor **)(param_2 + 0x378),1);
    QPalette::setBrush(local_58,5,1,pQVar7);
    QBrush::~QBrush((QBrush *)&local_res10);
  }
  QWidget::setAutoFillBackground((QWidget *)pQVar3,true);
  QWidget::setPalette((QWidget *)pQVar3,local_58);
  QPushButton::setFlat(pQVar3,true);
  QWidget::update((QWidget *)pQVar3);
  *(QPushButton **)(param_2 + 0x370) = pQVar3;
  pQVar2 = (QTableWidgetItem *)0x0;
  QObject::connect((QObject *)pQVar3,"2clicked()",param_1,"1_SlotOnColorBtnClicked()",0);
  QTableWidget::setCellWidget(*(QTableWidget **)(param_1 + 0x28),param_3,1,pQVar4);
  this = (QComboBox *)FUN_140058d94(0x28);
  this_01 = (QComboBox *)pQVar2;
  local_res8 = (QTableWidgetItem *)this;
  if (this != (QComboBox *)0x0) {
    QComboBox::QComboBox(this,(QWidget *)0x0);
    *(undefined ***)this = QComboBox::vftable;
    *(undefined ***)(this + 0x10) = QComboBox::vftable;
    this_01 = this;
  }
  QVariant::QVariant(local_48,0);
  local_res8 = (QTableWidgetItem *)QString::fromAscii_helper("Line",-1);
  iVar1 = QComboBox::count(this_01);
  pQVar8 = (QIcon *)QIcon::QIcon((QIcon *)&local_res10);
  QComboBox::insertItem(this_01,iVar1,pQVar8,(QString *)&local_res8,local_48);
  QIcon::~QIcon((QIcon *)&local_res10);
  QString::~QString((QString *)&local_res8);
  QVariant::~QVariant(local_48);
  QVariant::QVariant(local_48,1);
  local_res8 = (QTableWidgetItem *)QString::fromAscii_helper("Points",-1);
  iVar1 = QComboBox::count(this_01);
  pQVar8 = (QIcon *)QIcon::QIcon((QIcon *)&local_res10);
  QComboBox::insertItem(this_01,iVar1,pQVar8,(QString *)&local_res8,local_48);
  QIcon::~QIcon((QIcon *)&local_res10);
  QString::~QString((QString *)&local_res8);
  QVariant::~QVariant(local_48);
  QVariant::QVariant(local_48,2);
  local_res8 = (QTableWidgetItem *)QString::fromAscii_helper("Line & Points",-1);
  iVar1 = QComboBox::count(this_01);
  pQVar8 = (QIcon *)QIcon::QIcon((QIcon *)&local_res10);
  QComboBox::insertItem(this_01,iVar1,pQVar8,(QString *)&local_res8,local_48);
  QIcon::~QIcon((QIcon *)&local_res10);
  QString::~QString((QString *)&local_res8);
  QVariant::~QVariant(local_48);
  QVariant::QVariant(local_48,3);
  local_res8 = (QTableWidgetItem *)QString::fromAscii_helper("Rectangle",-1);
  iVar1 = QComboBox::count(this_01);
  pQVar8 = (QIcon *)QIcon::QIcon((QIcon *)&local_res10);
  QComboBox::insertItem(this_01,iVar1,pQVar8,(QString *)&local_res8,local_48);
  QIcon::~QIcon((QIcon *)&local_res10);
  QString::~QString((QString *)&local_res8);
  QVariant::~QVariant(local_48);
  QComboBox::setCurrentIndex(this_01,*(int *)(param_2 + 0x348));
  QObject::connect((QObject *)this_01,"2currentIndexChanged(int)",param_1,
                   "1_SlotOnDrawStyleChanged()",0);
  QTableWidget::setCellWidget(*(QTableWidget **)(param_1 + 0x28),param_3,2,(QWidget *)this_01);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  bVar16 = pQVar9 != (QTableWidgetItem *)0x0;
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (bVar16) {
    local_res20 = QString::fromAscii_helper(*(char **)(param_2 + 0x108),-1);
    local_res8 = (QTableWidgetItem *)CONCAT44(local_res8._4_4_,1);
    QTableWidgetItem::QTableWidgetItem(pQVar9,(QString *)&local_res20,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
  }
  uVar14 = (uint)bVar16;
  if (bVar16) {
    uVar14 = 0;
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,3,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    pQVar11 = (QString *)QString::number((uint)&local_res20,*(int *)(param_2 + 0x318));
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 2);
    QTableWidgetItem::QTableWidgetItem(pQVar9,pQVar11,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = uVar14 | 2;
  }
  if ((uVar14 & 2) != 0) {
    uVar14 = uVar14 & 0xfffffffd;
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,4,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    pQVar11 = (QString *)QString::number((uint)&local_res20,*(int *)(param_2 + 0x31c));
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 4);
    QTableWidgetItem::QTableWidgetItem(pQVar9,pQVar11,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = uVar14 | 4;
  }
  if ((uVar14 & 4) != 0) {
    uVar14 = uVar14 & 0xfffffffb;
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,5,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    pQVar11 = (QString *)QString::number((int)&local_res20,0);
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 8);
    QTableWidgetItem::QTableWidgetItem(pQVar9,pQVar11,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = uVar14 | 8;
  }
  if ((uVar14 & 8) != 0) {
    uVar14 = uVar14 & 0xfffffff7;
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,7,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    local_res20 = QString::fromAscii_helper("N/A",-1);
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 0x10);
    QTableWidgetItem::QTableWidgetItem(pQVar9,(QString *)&local_res20,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = uVar14 | 0x10;
  }
  if ((uVar14 & 0x10) != 0) {
    uVar14 = uVar14 & 0xffffffef;
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,8,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    local_res20 = QString::fromAscii_helper("N/A",-1);
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 0x20);
    QTableWidgetItem::QTableWidgetItem(pQVar9,(QString *)&local_res20,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = uVar14 | 0x20;
  }
  if ((uVar14 & 0x20) != 0) {
    uVar14 = uVar14 & 0xffffffdf;
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,9,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    local_res20 = QString::fromAscii_helper((char *)(param_2 + 0x110),-1);
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 0x40);
    QTableWidgetItem::QTableWidgetItem(pQVar9,(QString *)&local_res20,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = uVar14 | 0x40;
  }
  if ((uVar14 & 0x40) != 0) {
    uVar14 = uVar14 & 0xffffffbf;
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,6,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    local_res20 = QString::fromAscii_helper("N/A",-1);
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 0x80);
    QTableWidgetItem::QTableWidgetItem(pQVar9,(QString *)&local_res20,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = uVar14 | 0x80;
  }
  if ((char)uVar14 < '\0') {
    uVar14 = uVar14 & 0xffffff7f;
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,0xb,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    local_res20 = QString::fromAscii_helper("N/A",-1);
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 0x100);
    QTableWidgetItem::QTableWidgetItem(pQVar9,(QString *)&local_res20,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = uVar14 | 0x100;
  }
  if (uVar14 >> 8 != 0) {
    uVar14 = uVar14 & 0xfffffeff;
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,0xc,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    local_res20 = QString::fromAscii_helper("N/A",-1);
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 0x200);
    QTableWidgetItem::QTableWidgetItem(pQVar9,(QString *)&local_res20,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = uVar14 | 0x200;
  }
  if (uVar14 >> 9 != 0) {
    uVar14 = uVar14 & 0xfffffdff;
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,0xd,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res8 = pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    QTableWidgetItem::QTableWidgetItem(pQVar9,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
  }
  FUN_140053680(param_1,pQVar12,*(undefined4 *)(param_2 + 0x358));
  uVar15 = 0;
  cVar13 = (char)param_3;
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,0xe,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res20 = (Data *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    pQVar11 = (QString *)
              QString::number(extraout_XMM0_Qa,cVar13,
                              (int)CONCAT71((int7)((ulonglong)uVar15 >> 8),0x67));
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 0x400);
    QTableWidgetItem::QTableWidgetItem(pQVar9,pQVar11,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = uVar14 | 0x400;
  }
  if (uVar14 >> 10 != 0) {
    uVar14 = uVar14 & 0xfffffbff;
    QString::~QString((QString *)&local_res10);
  }
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,0xf,pQVar12);
  pQVar9 = (QTableWidgetItem *)FUN_140058d94(0x30);
  pQVar12 = pQVar2;
  local_res10 = (QWidget *)pQVar9;
  if (pQVar9 != (QTableWidgetItem *)0x0) {
    local_res20 = QString::fromAscii_helper("N/A",-1);
    local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,uVar14) | 0x800);
    QTableWidgetItem::QTableWidgetItem(pQVar9,(QString *)&local_res20,0);
    *(undefined ***)pQVar9 = QTableWidgetItem::vftable;
    pQVar12 = pQVar9;
    uVar14 = 0x800;
  }
  if (uVar14 >> 0xb != 0) {
    QString::~QString((QString *)&local_res20);
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar12);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar12,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,10,pQVar12);
  pQVar12 = (QTableWidgetItem *)FUN_140058d94(0x30);
  local_res8 = pQVar12;
  if (pQVar12 != (QTableWidgetItem *)0x0) {
    QTableWidgetItem::QTableWidgetItem(pQVar12,0);
    *(undefined ***)pQVar12 = QTableWidgetItem::vftable;
    pQVar2 = pQVar12;
  }
  puVar10 = (undefined4 *)QTableWidgetItem::flags(pQVar2);
  local_res8 = (QTableWidgetItem *)(CONCAT44(local_res8._4_4_,*puVar10) & 0xfffffffffffffffd);
  QTableWidgetItem::setFlags(pQVar2,&local_res8);
  QTableWidget::setItem(*(QTableWidget **)(param_1 + 0x28),param_3,0x10,pQVar2);
  pQVar2 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),param_3,3);
  *(QTableWidgetItem **)(param_2 + 0x368) = pQVar2;
  QTableView::resizeColumnsToContents(*(QTableView **)(param_1 + 0x28));
  this_00 = QTableView::horizontalHeader(*(QTableView **)(param_1 + 0x28));
  QHeaderView::setStretchLastSection(this_00,true);
  QPalette::~QPalette(local_58);
  return;
}


/* ================= FUN_140052ea0 @ 140052ea0  size=202  callers=9 callees=2 ================= */
/* CALLERS: FUN_140053920, FUN_140053f20, FUN_140054030, FUN_140054060, FUN_1400540f0, FUN_140054130, FUN_140054170, FUN_1400541b0, FUN_140054200 */
/* CALLEES: qFree, selectedItems */

longlong FUN_140052ea0(longlong param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  longlong lVar4;
  longlong *plVar5;
  longlong *plVar6;
  int *local_res10;
  int *local_res18;
  
  plVar6 = DAT_14286d3b8;
  if (DAT_14286d3b8 != (longlong *)0x0) {
    plVar5 = (longlong *)QTableWidget::selectedItems(*(QTableWidget **)(param_1 + 0x28));
    iVar2 = *(int *)(*plVar5 + 0xc);
    iVar3 = *(int *)(*plVar5 + 8);
    LOCK();
    iVar1 = *local_res10;
    *local_res10 = *local_res10 + -1;
    UNLOCK();
    if (iVar1 == 1) {
      qFree(local_res10);
    }
    if (iVar2 != iVar3) {
      plVar5 = (longlong *)QTableWidget::selectedItems(*(QTableWidget **)(param_1 + 0x28));
      lVar4 = *(longlong *)(*plVar5 + 0x18 + (longlong)*(int *)(*plVar5 + 8) * 8);
      LOCK();
      iVar1 = *local_res18;
      *local_res18 = *local_res18 + -1;
      UNLOCK();
      if (iVar1 == 1) {
        qFree(local_res18);
      }
      do {
        if (*(longlong *)(*plVar6 + 0x368) == lVar4) {
          return *plVar6;
        }
        plVar6 = (longlong *)plVar6[1];
      } while (plVar6 != (longlong *)0x0);
    }
  }
  return 0;
}


/* ================= FUN_140052f70 @ 140052f70  size=93  callers=2 callees=1 ================= */
/* CALLERS: FUN_140053c80, FUN_140053f50 */
/* CALLEES: FUN_140052fd0 */

undefined8 FUN_140052f70(int param_1)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  int iVar3;
  
  puVar2 = DAT_14286d3b8;
  while( true ) {
    if (puVar2 == (undefined8 *)0x0) {
      return 0;
    }
    uVar1 = *puVar2;
    iVar3 = FUN_140052fd0(uVar1);
    if (iVar3 == param_1) break;
    puVar2 = (undefined8 *)puVar2[1];
  }
  return uVar1;
}


/* ================= FUN_140052fd0 @ 140052fd0  size=34  callers=3 callees=0 ================= */
/* CALLERS: FUN_140052f70, FUN_140053c10, FUN_140054250 */

int FUN_140052fd0(longlong param_1)

{
  QTableWidgetItem *pQVar1;
  int iVar2;
  
  pQVar1 = *(QTableWidgetItem **)(param_1 + 0x368);
  if ((pQVar1 != (QTableWidgetItem *)0x0) &&
     (*(QTableWidget **)(pQVar1 + 0x18) != (QTableWidget *)0x0)) {
                    /* WARNING: Could not recover jumptable at 0x000140052fe5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    iVar2 = QTableWidget::row(*(QTableWidget **)(pQVar1 + 0x18),pQVar1);
    return iVar2;
  }
  return -1;
}


/* ================= FUN_140053000 @ 140053000  size=651  callers=1 callees=32 ================= */
/* CALLERS: FUN_140052110 */
/* CALLEES: FUN_14003ec10, FUN_14003ed50, FUN_140043d10, FUN_140043e60, FUN_14004e9c0, FUN_14004ea40, FUN_1400521c0, FUN_140053290, FUN_140058d94, QBrush, QColor, QPalette, QPushButton, __security_check_cookie, connect, fromAscii_helper, operator=, palette, rowHeight, setAutoFillBackground, setBrush, setCellWidget, setDisabled, setFixedSize, setFlat, setPalette, setRowCount, setStyleSheet, update, ~QBrush, ~QPalette, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140053000(QObject *param_1)

{
  undefined8 *puVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  longlong lVar5;
  QPushButton *this;
  QPalette *pQVar6;
  QBrush *pQVar7;
  QPushButton *pQVar8;
  bool bVar9;
  undefined1 auStackY_88 [32];
  Data *local_58;
  undefined4 local_50;
  QPalette local_48 [16];
  QPushButton *local_38 [2];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStackY_88;
  local_50 = 0;
  QPalette::QPalette(local_48);
  FUN_140053290(param_1);
  iVar3 = FUN_14003ed50();
  QTableWidget::setRowCount(*(QTableWidget **)(param_1 + 0x28),iVar3 + 1);
  iVar3 = 0;
  for (puVar1 = DAT_14286d3b8; puVar1 != (undefined8 *)0x0; puVar1 = (undefined8 *)puVar1[1]) {
    FUN_1400521c0(param_1,*puVar1);
    iVar3 = iVar3 + 1;
  }
  lVar5 = FUN_140043d10(0x5bc);
  FUN_14004e9c0(lVar5);
  if ((*(char *)(lVar5 + 0x5a4) == '\0') || (*(char *)(lVar5 + 0x5a6) == '\x01')) {
    this = (QPushButton *)FUN_140058d94(0x28);
    local_38[0] = this;
    if (this == (QPushButton *)0x0) {
      pQVar8 = (QPushButton *)0x0;
    }
    else {
      local_58 = QString::fromAscii_helper("+",-1);
      local_50 = 1;
      QPushButton::QPushButton(this,(QString *)&local_58,(QWidget *)0x0);
      *(undefined ***)this = QPushButton::vftable;
      *(undefined ***)(this + 0x10) = QPushButton::vftable;
      pQVar8 = this;
    }
    *(QPushButton **)(param_1 + 0x30) = pQVar8;
    if (this != (QPushButton *)0x0) {
      QString::~QString((QString *)&local_58);
    }
    local_58 = QString::fromAscii_helper("font-weight: bold;",-1);
    QWidget::setStyleSheet(*(QWidget **)(param_1 + 0x30),(QString *)&local_58);
    QString::~QString((QString *)&local_58);
    QPushButton::setFlat(*(QPushButton **)(param_1 + 0x30),true);
    pQVar6 = QWidget::palette(*(QWidget **)(param_1 + 0x30));
    QPalette::operator=(local_48,pQVar6);
    QColor::QColor((QColor *)local_38,6);
    pQVar7 = (QBrush *)QBrush::QBrush((QBrush *)&local_58,(QColor *)local_38,1);
    QPalette::setBrush(local_48,5,1,pQVar7);
    QBrush::~QBrush((QBrush *)&local_58);
    QWidget::setAutoFillBackground(*(QWidget **)(param_1 + 0x30),true);
    QWidget::setPalette(*(QWidget **)(param_1 + 0x30),local_48);
    QWidget::update(*(QWidget **)(param_1 + 0x30));
    iVar4 = QTableView::rowHeight(*(QTableView **)(param_1 + 0x28),0);
    QWidget::setFixedSize(*(QWidget **)(param_1 + 0x30),iVar4 + -2,iVar4 + -2);
    QObject::connect(*(QObject **)(param_1 + 0x30),"2clicked()",param_1,"1_SlotOnAddBtnClicked()",0)
    ;
    QTableWidget::setCellWidget
              (*(QTableWidget **)(param_1 + 0x28),iVar3,0,*(QWidget **)(param_1 + 0x30));
    iVar3 = FUN_14004ea40();
    if (iVar3 == 0) {
      bVar9 = true;
    }
    else {
      cVar2 = FUN_14003ec10();
      bVar9 = cVar2 != '\0';
    }
    QWidget::setDisabled(*(QWidget **)(param_1 + 0x30),bVar9);
  }
  FUN_140043e60(lVar5);
  QPalette::~QPalette(local_48);
  return;
}


/* ================= FUN_140053290 @ 140053290  size=40  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400520f0, FUN_140053000 */
/* CALLEES: clearContents */

void FUN_140053290(longlong param_1)

{
  QTableWidget::clearContents(*(QTableWidget **)(param_1 + 0x28));
                    /* WARNING: Could not recover jumptable at 0x0001400532b1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QTableWidget::setRowCount(*(QTableWidget **)(param_1 + 0x28),1);
  return;
}

