
/* ================= FUN_1400353b0 @ 1400353b0  size=57  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003a000 */
/* CALLEES: FUN_140043e60 */

undefined8 FUN_1400353b0(longlong *param_1)

{
  if (*param_1 != 0) {
    FUN_140043e60();
    param_1[1] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
    param_1[4] = 0;
    *param_1 = 0;
    return 0;
  }
  return 0;
}


/* ================= FUN_1400353f0 @ 1400353f0  size=120  callers=1 callees=2 ================= */
/* CALLERS: FUN_140035180 */
/* CALLEES: FUN_1400351b0, memcpy */

undefined8 FUN_1400353f0(longlong *param_1,uint param_2,void *param_3,uint param_4)

{
  undefined8 uVar1;
  uint uVar2;
  
  uVar2 = (param_2 - 1) + param_4;
  uVar1 = FUN_1400351b0(param_1,uVar2);
  if ((int)uVar1 < 0) {
    *(int *)(param_1 + 3) = (int)uVar1;
  }
  else {
    memcpy((void *)((ulonglong)param_2 + *param_1),param_3,(ulonglong)param_4);
    if (*(uint *)(param_1 + 2) < uVar2) {
      *(uint *)(param_1 + 2) = uVar2;
    }
    *(undefined4 *)((longlong)param_1 + 0x14) = 1;
    uVar1 = 0;
  }
  return uVar1;
}


/* ================= FUN_140035470 @ 140035470  size=10  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400352e0 */

void FUN_140035470(longlong param_1)

{
  *(undefined8 *)(param_1 + 0x10) = 0;
  *(undefined4 *)(param_1 + 0x18) = 0;
  return;
}


/* ================= FUN_140035480 @ 140035480  size=77  callers=6 callees=2 ================= */
/* CALLERS: FUN_140039600, FUN_140039a10, FUN_140039f10, FUN_14003a0e0, FUN_14003aa40, FUN_14003aca0 */
/* CALLEES: FUN_140035540, FUN_1400355e0 */

longlong FUN_140035480(undefined8 param_1,ulonglong param_2)

{
  longlong *plVar1;
  longlong lVar2;
  ulonglong uVar3;
  
  uVar3 = param_2 & 0xffffffff;
  if ((uint)param_2 < 0x400000) {
    lVar2 = FUN_1400355e0();
    if (lVar2 != 0) {
      return lVar2;
    }
    plVar1 = (longlong *)FUN_140035540(param_1,0x400000,uVar3 & 0xffffffff);
  }
  else {
    plVar1 = (longlong *)FUN_140035540(param_1,param_2,param_2 & 0xffffffff);
  }
  if (plVar1 == (longlong *)0x0) {
    return 0;
  }
  return *plVar1;
}


/* ================= FUN_1400354d0 @ 1400354d0  size=43  callers=3 callees=1 ================= */
/* CALLERS: FUN_1400399f0, FUN_140039a10, FUN_140039f10 */
/* CALLEES: FUN_140043e60 */

void FUN_1400354d0(longlong param_1)

{
  while (param_1 != 0) {
    param_1 = *(longlong *)(param_1 + 8);
    FUN_140043e60();
  }
  return;
}


/* ================= FUN_140035500 @ 140035500  size=15  callers=2 callees=1 ================= */
/* CALLERS: FUN_140039a10, FUN_140039f10 */
/* CALLEES: FUN_140035540 */

void FUN_140035500(void)

{
  FUN_140035540(0,0x400000,0);
  return;
}


/* ================= FUN_140035510 @ 140035510  size=33  callers=1 callees=0 ================= */
/* CALLERS: FUN_140035540 */

void FUN_140035510(longlong param_1,undefined8 param_2)

{
  longlong lVar1;
  longlong lVar2;
  
  lVar2 = *(longlong *)(param_1 + 8);
  while (lVar1 = lVar2, lVar1 != 0) {
    param_1 = lVar1;
    lVar2 = *(longlong *)(lVar1 + 8);
  }
  *(undefined8 *)(param_1 + 8) = param_2;
  return;
}


/* ================= FUN_140035540 @ 140035540  size=156  callers=2 callees=4 ================= */
/* CALLERS: FUN_140035480, FUN_140035500 */
/* CALLEES: FUN_140035510, FUN_140035640, FUN_140043d20, memset */

longlong * FUN_140035540(longlong param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  longlong *_Dst;
  
  iVar1 = FUN_140035640(param_2);
  uVar2 = FUN_140035640(0x18);
  _Dst = (longlong *)FUN_140043d20(uVar2 + iVar1);
  if (_Dst != (longlong *)0x0) {
    memset(_Dst,0,(ulonglong)(uVar2 + iVar1));
    *(int *)((longlong)_Dst + 0x14) = iVar1;
    uVar3 = FUN_140035640(param_3);
    *(undefined4 *)(_Dst + 2) = uVar3;
    *_Dst = (longlong)_Dst + (ulonglong)uVar2;
    if (param_1 != 0) {
      FUN_140035510(param_1,_Dst);
    }
  }
  return _Dst;
}


/* ================= FUN_1400355e0 @ 1400355e0  size=82  callers=1 callees=1 ================= */
/* CALLERS: FUN_140035480 */
/* CALLEES: FUN_140035640 */

longlong FUN_1400355e0(longlong *param_1,undefined4 param_2)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = FUN_140035640(param_2);
  while( true ) {
    if (param_1 == (longlong *)0x0) {
      return 0;
    }
    uVar1 = *(uint *)(param_1 + 2);
    if (uVar2 <= *(int *)((longlong)param_1 + 0x14) - uVar1) break;
    param_1 = (longlong *)param_1[1];
  }
  *(uint *)(param_1 + 2) = uVar1 + uVar2;
  return (ulonglong)uVar1 + *param_1;
}


/* ================= FUN_140035640 @ 140035640  size=7  callers=2 callees=0 ================= */
/* CALLERS: FUN_140035540, FUN_1400355e0 */

uint FUN_140035640(int param_1)

{
  return param_1 + 7U & 0xfffffff8;
}


/* ================= FUN_140035648 @ 140035648  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400356a0 */

void FUN_140035648(longlong param_1)

{
  FUN_1400356a0(param_1 + -0x10);
  return;
}


/* ================= FUN_140035654 @ 140035654  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400356e0 */

void FUN_140035654(longlong param_1)

{
  FUN_1400356e0(param_1 + -0x10);
  return;
}


/* ================= FUN_140035660 @ 140035660  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140035720 */

void FUN_140035660(longlong param_1)

{
  FUN_140035720(param_1 + -0x10);
  return;
}


/* ================= FUN_14003566c @ 14003566c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140035760 */

void FUN_14003566c(longlong param_1)

{
  FUN_140035760(param_1 + -0x10);
  return;
}


/* ================= FUN_140035678 @ 140035678  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400357a0 */

void FUN_140035678(longlong param_1)

{
  FUN_1400357a0(param_1 + -0x10);
  return;
}


/* ================= FUN_140035684 @ 140035684  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400357e0 */

void FUN_140035684(longlong param_1)

{
  FUN_1400357e0(param_1 + -0x10);
  return;
}


/* ================= FUN_140035690 @ 140035690  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140035820 */

void FUN_140035690(longlong param_1)

{
  FUN_140035820(param_1 + -0x10);
  return;
}


/* ================= FUN_1400356a0 @ 1400356a0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140035648 */
/* CALLEES: free, ~QCheckBox */

QCheckBox * FUN_1400356a0(QCheckBox *param_1,uint param_2)

{
  QCheckBox::~QCheckBox(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_1400356e0 @ 1400356e0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140035654 */
/* CALLEES: free, ~QComboBox */

QComboBox * FUN_1400356e0(QComboBox *param_1,uint param_2)

{
  QComboBox::~QComboBox(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140035720 @ 140035720  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140035660 */
/* CALLEES: free, ~QGroupBox */

QGroupBox * FUN_140035720(QGroupBox *param_1,uint param_2)

{
  QGroupBox::~QGroupBox(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140035760 @ 140035760  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003566c */
/* CALLEES: free, ~QLabel */

QLabel * FUN_140035760(QLabel *param_1,uint param_2)

{
  QLabel::~QLabel(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_1400357a0 @ 1400357a0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140035678 */
/* CALLEES: free, ~QRadioButton */

QRadioButton * FUN_1400357a0(QRadioButton *param_1,uint param_2)

{
  QRadioButton::~QRadioButton(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_1400357e0 @ 1400357e0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140035684 */
/* CALLEES: free, ~QSplitter */

QSplitter * FUN_1400357e0(QSplitter *param_1,uint param_2)

{
  QSplitter::~QSplitter(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140035820 @ 140035820  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140035690 */
/* CALLEES: free, ~QTableWidget */

QTableWidget * FUN_140035820(QTableWidget *param_1,uint param_2)

{
  QTableWidget::~QTableWidget(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140035860 @ 140035860  size=53  callers=0 callees=2 ================= */
/* CALLEES: free, ~QTableWidgetItem */

QTableWidgetItem * FUN_140035860(QTableWidgetItem *param_1,uint param_2)

{
  QTableWidgetItem::~QTableWidgetItem(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_1400358a0 @ 1400358a0  size=194  callers=1 callees=8 ================= */
/* CALLERS: FUN_140035e00 */
/* CALLEES: QIcon, QVariant, count, fromAscii_helper, insertItem, ~QIcon, ~QString, ~QVariant */

void FUN_1400358a0(QComboBox *param_1,longlong *param_2,uint param_3)

{
  int iVar1;
  QIcon *pQVar2;
  ulonglong uVar3;
  Data *local_res10 [2];
  QIcon local_res20 [8];
  QVariant local_28 [16];
  
  if ((param_2 != (longlong *)0x0) && (0 < (int)param_3)) {
    uVar3 = (ulonglong)param_3;
    do {
      if (*param_2 != 0) {
        QVariant::QVariant(local_28,*(uint *)(param_2 + 1));
        local_res10[0] = QString::fromAscii_helper((char *)*param_2,-1);
        iVar1 = QComboBox::count(param_1);
        pQVar2 = (QIcon *)QIcon::QIcon(local_res20);
        QComboBox::insertItem(param_1,iVar1,pQVar2,(QString *)local_res10,local_28);
        QIcon::~QIcon(local_res20);
        QString::~QString((QString *)local_res10);
        QVariant::~QVariant(local_28);
        param_2 = param_2 + 2;
      }
      uVar3 = uVar3 - 1;
    } while (uVar3 != 0);
  }
  return;
}


/* ================= FUN_140035970 @ 140035970  size=102  callers=4 callees=4 ================= */
/* CALLERS: FUN_140033060, FUN_140033b90, FUN_140036720, FUN_1400368e0 */
/* CALLEES: QString, fromLocal8Bit, operator=, ~QString */

QString * FUN_140035970(QString *param_1,int param_2)

{
  QString *pQVar1;
  QString local_res18 [16];
  
  QString::QString(param_1);
  pQVar1 = (QString *)QString::fromLocal8Bit((char *)local_res18,param_2);
  QString::operator=(param_1,pQVar1);
  QString::~QString(local_res18);
  return param_1;
}


/* ================= FUN_1400359e0 @ 1400359e0  size=561  callers=2 callees=4 ================= */
/* CALLERS: FUN_14002ce80, FUN_140034630 */
/* CALLEES: FUN_140027140, FUN_1400277b0, FUN_1400277d0, FUN_140037020 */

bool FUN_1400359e0(char *param_1,char *param_2,uint param_3)

{
  char cVar1;
  bool bVar2;
  char cVar3;
  uint uVar4;
  ulonglong uVar5;
  char *pcVar6;
  char *pcVar7;
  uint uVar8;
  ulonglong uVar9;
  bool bVar10;
  char *local_58;
  char *local_50;
  undefined8 local_48;
  undefined8 local_40;
  undefined4 local_38;
  undefined4 local_34;
  
  uVar9 = (ulonglong)param_3;
  bVar2 = false;
  cVar3 = FUN_140037020(param_1,&local_58);
  bVar10 = cVar3 != '\0';
  pcVar7 = param_2;
  do {
    if ((cVar3 != '\0') && (param_1 == local_58)) {
      FUN_140027140(pcVar7,"<a href=\"",uVar9);
      uVar4 = FUN_1400277b0(pcVar7);
      uVar5 = (ulonglong)uVar4;
      if ((uint)uVar9 < uVar4) {
        uVar5 = uVar9;
      }
      uVar8 = (uint)uVar9 - (int)uVar5;
      pcVar7 = pcVar7 + uVar5;
      FUN_1400277d0(pcVar7,local_48,local_38,uVar8);
      uVar4 = FUN_1400277b0(pcVar7);
      if (uVar8 < uVar4) {
        uVar4 = uVar8;
      }
      uVar8 = uVar8 - uVar4;
      pcVar7 = pcVar7 + uVar4;
      FUN_140027140(pcVar7,&DAT_140070c5c,uVar8);
      uVar4 = FUN_1400277b0(pcVar7);
      if (uVar8 < uVar4) {
        uVar4 = uVar8;
      }
      uVar8 = uVar8 - uVar4;
      pcVar7 = pcVar7 + uVar4;
      FUN_1400277d0(pcVar7,local_40,local_34,uVar8);
      uVar4 = FUN_1400277b0(pcVar7);
      if (uVar8 < uVar4) {
        uVar4 = uVar8;
      }
      uVar8 = uVar8 - uVar4;
      pcVar7 = pcVar7 + uVar4;
      FUN_140027140(pcVar7,&DAT_140070c60,uVar8);
      uVar4 = FUN_1400277b0(pcVar7);
      param_1 = local_50;
      if (uVar8 < uVar4) {
        uVar4 = uVar8;
      }
      uVar9 = (ulonglong)(uVar8 - uVar4);
      pcVar7 = pcVar7 + uVar4;
      if (*local_50 == '\0') {
LAB_140035bbc:
        uVar4 = (uint)uVar9;
        if ((bVar2) && (0 < (int)uVar4)) {
          FUN_140027140(pcVar7,&DAT_14006fd4c,uVar9);
          uVar4 = uVar4 - 4;
        }
LAB_140035bda:
        if ((int)uVar4 < 1) {
          uVar4 = 1;
        }
        param_2[param_3 - uVar4] = '\0';
        return bVar10;
      }
      cVar3 = FUN_140037020(local_50,&local_58);
    }
    if ((*param_1 != '\n') && (bVar2)) {
      FUN_140027140(pcVar7,&DAT_14006fd4c,uVar9);
      pcVar7 = pcVar7 + 4;
      uVar4 = (int)uVar9 - 4;
      uVar9 = (ulonglong)uVar4;
      if ((int)uVar4 < 1) goto LAB_140035bda;
    }
    cVar1 = *param_1;
    if (cVar1 == '\r') {
      bVar2 = true;
    }
    else {
      bVar2 = false;
      uVar5 = 0;
      pcVar6 = ">";
      do {
        if (*pcVar6 == cVar1) {
          FUN_140027140(pcVar7,*(undefined8 *)(&UNK_14006fca8 + uVar5 * 0x18),uVar9);
          pcVar7 = pcVar7 + *(uint *)(&UNK_14006fcb0 + uVar5 * 0x18);
          uVar9 = (ulonglong)((int)uVar9 - *(uint *)(&UNK_14006fcb0 + uVar5 * 0x18));
          goto LAB_140035bab;
        }
        uVar4 = (int)uVar5 + 1;
        uVar5 = (ulonglong)uVar4;
        pcVar6 = pcVar6 + 0x18;
      } while (uVar4 < 6);
      *pcVar7 = cVar1;
      pcVar7 = pcVar7 + 1;
      uVar9 = (ulonglong)((int)uVar9 - 1);
    }
LAB_140035bab:
    param_1 = param_1 + 1;
    if ((*param_1 == '\0') || ((int)uVar9 < 1)) goto LAB_140035bbc;
  } while( true );
}


/* ================= FUN_140035c20 @ 140035c20  size=208  callers=3 callees=7 ================= */
/* CALLERS: FUN_14002a410, FUN_140035cf0, FUN_1400503f0 */
/* CALLEES: FUN_140058d94, QPushButton, connect, fromAscii_helper, setAutoDefault, setText, ~QString */

QAbstractButton * FUN_140035c20(QWidget *param_1,char *param_2,QObject *param_3,char *param_4)

{
  QPushButton *pQVar1;
  QPushButton *local_res10;
  
  pQVar1 = (QPushButton *)FUN_140058d94(0x28);
  local_res10 = pQVar1;
  if (pQVar1 == (QPushButton *)0x0) {
    pQVar1 = (QPushButton *)0x0;
  }
  else {
    QPushButton::QPushButton(pQVar1,param_1);
    *(undefined ***)pQVar1 = QPushButton::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QPushButton::vftable;
  }
  if (param_2 != (char *)0x0) {
    local_res10 = (QPushButton *)QString::fromAscii_helper(param_2,-1);
    QAbstractButton::setText((QAbstractButton *)pQVar1,(QString *)&local_res10);
    QString::~QString((QString *)&local_res10);
  }
  if (param_4 != (char *)0x0) {
    QObject::connect((QObject *)pQVar1,"2released(void)",param_3,param_4,0);
  }
  QPushButton::setAutoDefault(pQVar1,true);
  return (QAbstractButton *)pQVar1;
}


/* ================= FUN_140035cf0 @ 140035cf0  size=50  callers=3 callees=2 ================= */
/* CALLERS: FUN_14002ce80, FUN_140034630, FUN_140057220 */
/* CALLEES: FUN_140035c20, setFixedSize */

QWidget * FUN_140035cf0(void)

{
  QWidget *this;
  QSize *pQVar1;
  undefined1 local_18 [16];
  
  this = (QWidget *)FUN_140035c20();
  pQVar1 = (QSize *)(**(code **)(*(longlong *)this + 0x60))(this,local_18);
  QWidget::setFixedSize(this,pQVar1);
  return this;
}


/* ================= FUN_140035d30 @ 140035d30  size=195  callers=4 callees=6 ================= */
/* CALLERS: FUN_14002a410, FUN_140034630, FUN_140054790, FUN_140057220 */
/* CALLEES: FUN_140058d94, QCheckBox, connect, fromAscii_helper, setText, ~QString */

QAbstractButton *
FUN_140035d30(QWidget *param_1,char *param_2,char *param_3,QObject *param_4,char *param_5)

{
  QCheckBox *pQVar1;
  QCheckBox *local_res10;
  
  pQVar1 = (QCheckBox *)FUN_140058d94(0x28);
  local_res10 = pQVar1;
  if (pQVar1 == (QCheckBox *)0x0) {
    pQVar1 = (QCheckBox *)0x0;
  }
  else {
    QCheckBox::QCheckBox(pQVar1,param_1);
    *(undefined ***)pQVar1 = QCheckBox::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QCheckBox::vftable;
  }
  if (param_2 != (char *)0x0) {
    local_res10 = (QCheckBox *)QString::fromAscii_helper(param_2,-1);
    QAbstractButton::setText((QAbstractButton *)pQVar1,(QString *)&local_res10);
    QString::~QString((QString *)&local_res10);
  }
  if (param_5 != (char *)0x0) {
    QObject::connect((QObject *)pQVar1,param_3,param_4,param_5,0);
  }
  return (QAbstractButton *)pQVar1;
}


/* ================= FUN_140035e00 @ 140035e00  size=142  callers=2 callees=4 ================= */
/* CALLERS: FUN_14002a410, FUN_1400561b0 */
/* CALLEES: FUN_1400358a0, FUN_140058d94, QComboBox, setEditable */

QComboBox * FUN_140035e00(QWidget *param_1,undefined8 param_2,int param_3)

{
  QComboBox *this;
  
  this = (QComboBox *)FUN_140058d94(0x28);
  if (this == (QComboBox *)0x0) {
    this = (QComboBox *)0x0;
  }
  else {
    QComboBox::QComboBox(this,param_1);
    *(undefined ***)this = QComboBox::vftable;
    *(undefined ***)(this + 0x10) = QComboBox::vftable;
  }
  if (param_3 != 0) {
    FUN_1400358a0(this,param_2,param_3);
  }
  QComboBox::setEditable(this,false);
  return this;
}


/* ================= FUN_140035e90 @ 140035e90  size=96  callers=4 callees=2 ================= */
/* CALLERS: FUN_14002a410, FUN_14002ce80, FUN_140034630, FUN_140035ef0 */
/* CALLEES: FUN_140058d94, QGridLayout */

QGridLayout * FUN_140035e90(QWidget *param_1)

{
  QGridLayout *this;
  
  this = (QGridLayout *)FUN_140058d94(0x20);
  if (this != (QGridLayout *)0x0) {
    QGridLayout::QGridLayout(this,param_1);
    *(undefined ***)this = QGridLayout::vftable;
    *(undefined ***)(this + 0x10) = QGridLayout::vftable;
    return this;
  }
  return (QGridLayout *)0x0;
}


/* ================= FUN_140035ef0 @ 140035ef0  size=61  callers=1 callees=2 ================= */
/* CALLERS: FUN_140034630 */
/* CALLEES: FUN_140035e90, setContentsMargins */

QLayout * FUN_140035ef0(undefined8 param_1,int *param_2)

{
  QLayout *this;
  
  this = (QLayout *)FUN_140035e90();
  QLayout::setContentsMargins(this,*param_2,param_2[1],param_2[2],param_2[3]);
  return this;
}


/* ================= FUN_140035f30 @ 140035f30  size=180  callers=1 callees=6 ================= */
/* CALLERS: FUN_14002a410 */
/* CALLEES: FUN_140058d94, QGroupBox, fromAscii_helper, setLayout, setTitle, ~QString */

QGroupBox * FUN_140035f30(QWidget *param_1,char *param_2,QLayout *param_3)

{
  QGroupBox *pQVar1;
  QGroupBox *local_res10;
  
  pQVar1 = (QGroupBox *)FUN_140058d94(0x28);
  local_res10 = pQVar1;
  if (pQVar1 == (QGroupBox *)0x0) {
    pQVar1 = (QGroupBox *)0x0;
  }
  else {
    QGroupBox::QGroupBox(pQVar1,param_1);
    *(undefined ***)pQVar1 = QGroupBox::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QGroupBox::vftable;
  }
  if (param_2 != (char *)0x0) {
    local_res10 = (QGroupBox *)QString::fromAscii_helper(param_2,-1);
    QGroupBox::setTitle(pQVar1,(QString *)&local_res10);
    QString::~QString((QString *)&local_res10);
  }
  if (param_3 != (QLayout *)0x0) {
    QWidget::setLayout((QWidget *)pQVar1,param_3);
  }
  return pQVar1;
}


/* ================= FUN_140035ff0 @ 140035ff0  size=163  callers=5 callees=5 ================= */
/* CALLERS: FUN_14002a410, FUN_14002ce80, FUN_140034630, FUN_14004fbe0, FUN_140057220 */
/* CALLEES: FUN_140058d94, QLabel, fromAscii_helper, setText, ~QString */

QLabel * FUN_140035ff0(undefined8 param_1,char *param_2)

{
  QLabel *pQVar1;
  undefined4 local_res10 [2];
  QLabel *local_res18;
  
  pQVar1 = (QLabel *)FUN_140058d94(0x28);
  local_res18 = pQVar1;
  if (pQVar1 == (QLabel *)0x0) {
    pQVar1 = (QLabel *)0x0;
  }
  else {
    local_res10[0] = 0;
    QLabel::QLabel(pQVar1,param_1,local_res10);
    *(undefined ***)pQVar1 = QLabel::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QLabel::vftable;
  }
  if (param_2 != (char *)0x0) {
    local_res18 = (QLabel *)QString::fromAscii_helper(param_2,-1);
    QLabel::setText(pQVar1,(QString *)&local_res18);
    QString::~QString((QString *)&local_res18);
  }
  return pQVar1;
}


/* ================= FUN_1400360a0 @ 1400360a0  size=171  callers=3 callees=8 ================= */
/* CALLERS: FUN_14002a410, FUN_140050340, FUN_1400561b0 */
/* CALLEES: FUN_140036f80, FUN_140037210, FUN_140058d94, fromAscii_helper, setMinimumWidth, setReadOnly, setText, ~QString */

QLineEdit * FUN_1400360a0(undefined8 param_1,char *param_2,int param_3)

{
  QLineEdit *this;
  Data *local_res10;
  
  local_res10 = (Data *)FUN_140058d94(0x30);
  if (local_res10 == (Data *)0x0) {
    this = (QLineEdit *)0x0;
  }
  else {
    this = (QLineEdit *)FUN_140037210(local_res10,param_1);
  }
  if (param_2 != (char *)0x0) {
    local_res10 = QString::fromAscii_helper(param_2,-1);
    QLineEdit::setText(this,(QString *)&local_res10);
    QString::~QString((QString *)&local_res10);
  }
  if (param_3 == 1) {
    QLineEdit::setReadOnly(this,true);
    FUN_140036f80(this);
  }
  QWidget::setMinimumWidth((QWidget *)this,0x96);
  return this;
}


/* ================= FUN_140036150 @ 140036150  size=180  callers=2 callees=6 ================= */
/* CALLERS: FUN_14004a560, FUN_140053920 */
/* CALLEES: FUN_140058d94, QMenu, addMenu, fromAscii_helper, setTitle, ~QString */

QMenu * FUN_140036150(QWidget *param_1,char *param_2,QMenuBar *param_3)

{
  QMenu *pQVar1;
  QMenu *local_res10;
  
  pQVar1 = (QMenu *)FUN_140058d94(0x28);
  local_res10 = pQVar1;
  if (pQVar1 == (QMenu *)0x0) {
    pQVar1 = (QMenu *)0x0;
  }
  else {
    QMenu::QMenu(pQVar1,param_1);
    *(undefined ***)pQVar1 = QMenu::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QMenu::vftable;
  }
  if (param_2 != (char *)0x0) {
    local_res10 = (QMenu *)QString::fromAscii_helper(param_2,-1);
    QMenu::setTitle(pQVar1,(QString *)&local_res10);
    QString::~QString((QString *)&local_res10);
  }
  if (param_3 != (QMenuBar *)0x0) {
    QMenuBar::addMenu(param_3,pQVar1);
  }
  return pQVar1;
}


/* ================= FUN_140036210 @ 140036210  size=284  callers=2 callees=10 ================= */
/* CALLERS: FUN_14004a560, FUN_140053920 */
/* CALLEES: FUN_140058d94, QAction, QKeySequence, addAction, connect, fromAscii_helper, setShortcut, setText, ~QKeySequence, ~QString */

QAction * FUN_140036210(QObject *param_1,QWidget *param_2,char *param_3,char *param_4,
                       QObject *param_5,char *param_6)

{
  QAction *pQVar1;
  QKeySequence *pQVar2;
  QAction *local_res10;
  QKeySequence local_res18 [8];
  
  pQVar1 = (QAction *)FUN_140058d94(0x10);
  local_res10 = pQVar1;
  if (pQVar1 == (QAction *)0x0) {
    pQVar1 = (QAction *)0x0;
  }
  else {
    QAction::QAction(pQVar1,param_1);
    *(undefined ***)pQVar1 = QAction::vftable;
  }
  if (param_3 != (char *)0x0) {
    local_res10 = (QAction *)QString::fromAscii_helper(param_3,-1);
    QAction::setText(pQVar1,(QString *)&local_res10);
    QString::~QString((QString *)&local_res10);
  }
  if (param_4 != (char *)0x0) {
    local_res10 = (QAction *)QString::fromAscii_helper(param_4,-1);
    pQVar2 = (QKeySequence *)QKeySequence::QKeySequence(local_res18,(QString *)&local_res10);
    QAction::setShortcut(pQVar1,pQVar2);
    QKeySequence::~QKeySequence(local_res18);
    QString::~QString((QString *)&local_res10);
  }
  if (param_6 != (char *)0x0) {
    QObject::connect((QObject *)pQVar1,"2triggered(void)",param_5,param_6,0);
  }
  if (param_2 != (QWidget *)0x0) {
    QWidget::addAction(param_2,pQVar1);
  }
  return pQVar1;
}


/* ================= FUN_140036330 @ 140036330  size=213  callers=2 callees=7 ================= */
/* CALLERS: FUN_14002a410, FUN_140057220 */
/* CALLEES: FUN_140058d94, QRadioButton, connect, fromAscii_helper, setChecked, setText, ~QString */

QAbstractButton *
FUN_140036330(QWidget *param_1,char *param_2,bool param_3,QObject *param_4,char *param_5)

{
  QRadioButton *pQVar1;
  QRadioButton *local_res10;
  
  pQVar1 = (QRadioButton *)FUN_140058d94(0x28);
  local_res10 = pQVar1;
  if (pQVar1 == (QRadioButton *)0x0) {
    pQVar1 = (QRadioButton *)0x0;
  }
  else {
    QRadioButton::QRadioButton(pQVar1,param_1);
    *(undefined ***)pQVar1 = QRadioButton::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QRadioButton::vftable;
  }
  if (param_2 != (char *)0x0) {
    local_res10 = (QRadioButton *)QString::fromAscii_helper(param_2,-1);
    QAbstractButton::setText((QAbstractButton *)pQVar1,(QString *)&local_res10);
    QString::~QString((QString *)&local_res10);
  }
  QAbstractButton::setChecked((QAbstractButton *)pQVar1,param_3);
  if (param_5 != (char *)0x0) {
    QObject::connect((QObject *)pQVar1,"2toggled(bool)",param_4,param_5,0);
  }
  return (QAbstractButton *)pQVar1;
}


/* ================= FUN_140036410 @ 140036410  size=551  callers=1 callees=26 ================= */
/* CALLERS: FUN_140051790 */
/* CALLEES: FUN_140058d94, QTableWidget, QTableWidgetItem, font, fromAscii_helper, hide, horizontalHeader, horizontalHeaderItem, minimumSectionSize, resizeColumnsToContents, setColumnCount, setDefaultSectionSize, setEditTriggers, setFont, setHighlightSections, setHorizontalHeaderItem, setMinimumSectionSize, setMinimumWidth, setRowCount, setSelectionBehavior, setSelectionMode, setShowGrid, setTextAlignment, verticalHeader, ~QFont, ~QString */

QWidget * FUN_140036410(QWidget *param_1,undefined8 *param_2,uint param_3,int param_4)

{
  bool bVar1;
  int iVar2;
  QTableWidget *pQVar3;
  QTableWidgetItem *pQVar4;
  QHeaderView *pQVar5;
  QFont *pQVar6;
  uint uVar7;
  QTableWidget *local_res10;
  QTableWidgetItem *local_38 [2];
  
  bVar1 = false;
  pQVar3 = (QTableWidget *)FUN_140058d94(0x28);
  local_res10 = pQVar3;
  if (pQVar3 == (QTableWidget *)0x0) {
    pQVar3 = (QTableWidget *)0x0;
  }
  else {
    QTableWidget::QTableWidget(pQVar3,param_1);
    *(undefined ***)pQVar3 = QTableWidget::vftable;
    *(undefined ***)(pQVar3 + 0x10) = QTableWidget::vftable;
  }
  if (param_4 != 0) {
    QWidget::setMinimumWidth((QWidget *)pQVar3,param_4);
  }
  QTableWidget::setColumnCount(pQVar3,param_3);
  QTableWidget::setRowCount(pQVar3,0);
  local_res10 = (QTableWidget *)((ulonglong)local_res10 & 0xffffffff00000000);
  QAbstractItemView::setEditTriggers((QAbstractItemView *)pQVar3,&local_res10);
  QAbstractItemView::setSelectionBehavior((QAbstractItemView *)pQVar3,1);
  QAbstractItemView::setSelectionMode((QAbstractItemView *)pQVar3,1);
  if ((param_2 != (undefined8 *)0x0) && (uVar7 = 0, param_3 != 0)) {
    do {
      pQVar4 = (QTableWidgetItem *)FUN_140058d94(0x30);
      local_38[0] = pQVar4;
      if (pQVar4 == (QTableWidgetItem *)0x0) {
        pQVar4 = (QTableWidgetItem *)0x0;
      }
      else {
        local_res10 = (QTableWidget *)QString::fromAscii_helper((char *)*param_2,-1);
        bVar1 = true;
        QTableWidgetItem::QTableWidgetItem(pQVar4,(QString *)&local_res10,0);
        *(undefined ***)pQVar4 = QTableWidgetItem::vftable;
      }
      QTableWidget::setHorizontalHeaderItem(pQVar3,uVar7,pQVar4);
      if (bVar1) {
        bVar1 = false;
        QString::~QString((QString *)&local_res10);
      }
      iVar2 = *(int *)(param_2 + 1);
      if (iVar2 != 0) {
        pQVar4 = QTableWidget::horizontalHeaderItem(pQVar3,uVar7);
        QTableWidgetItem::setTextAlignment(pQVar4,iVar2);
      }
      uVar7 = uVar7 + 1;
      param_2 = param_2 + 2;
    } while (uVar7 < param_3);
  }
  pQVar5 = QTableView::horizontalHeader((QTableView *)pQVar3);
  QHeaderView::setMinimumSectionSize(pQVar5,1);
  pQVar5 = QTableView::horizontalHeader((QTableView *)pQVar3);
  QHeaderView::setHighlightSections(pQVar5,false);
  QTableView::resizeColumnsToContents((QTableView *)pQVar3);
  pQVar5 = QTableView::verticalHeader((QTableView *)pQVar3);
  iVar2 = QHeaderView::minimumSectionSize(pQVar5);
  pQVar5 = QTableView::verticalHeader((QTableView *)pQVar3);
  QHeaderView::setDefaultSectionSize(pQVar5,iVar2);
  pQVar5 = QTableView::verticalHeader((QTableView *)pQVar3);
  QWidget::hide((QWidget *)pQVar5);
  QTableView::setShowGrid((QTableView *)pQVar3,false);
  pQVar6 = (QFont *)QApplication::font();
  QWidget::setFont((QWidget *)pQVar3,pQVar6);
  QFont::~QFont((QFont *)local_38);
  return (QWidget *)pQVar3;
}


/* ================= FUN_140036640 @ 140036640  size=125  callers=1 callees=3 ================= */
/* CALLERS: FUN_140034630 */
/* CALLEES: FUN_140058d94, QWidget, setLayout */

QWidget * FUN_140036640(undefined8 param_1,QLayout *param_2)

{
  QWidget *pQVar1;
  undefined4 local_res18 [2];
  QWidget *local_res20;
  
  pQVar1 = (QWidget *)FUN_140058d94(0x28);
  local_res20 = pQVar1;
  if (pQVar1 == (QWidget *)0x0) {
    pQVar1 = (QWidget *)0x0;
  }
  else {
    local_res18[0] = 0;
    QWidget::QWidget(pQVar1,param_1,local_res18);
    *(undefined ***)pQVar1 = QWidget::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QWidget::vftable;
  }
  QWidget::setLayout(pQVar1,param_2);
  return pQVar1;
}


/* ================= FUN_1400366c0 @ 1400366c0  size=77  callers=1 callees=2 ================= */
/* CALLERS: FUN_140034630 */
/* CALLEES: setWindowFlags, windowFlags */

void FUN_1400366c0(QWidget *param_1,int param_2)

{
  uint *puVar1;
  uint local_res10 [6];
  
  puVar1 = (uint *)QWidget::windowFlags(param_1);
  local_res10[0] = *puVar1 | 0x40000;
  if (param_2 == 0) {
    local_res10[0] = *puVar1 & 0xfffbffff;
  }
  QWidget::setWindowFlags(param_1,local_res10);
  return;
}


/* ================= FUN_140036710 @ 140036710  size=14  callers=1 callees=0 ================= */
/* CALLERS: FUN_140036f30 */

uint * FUN_140036710(uint *param_1,uint *param_2)

{
  *param_2 = *param_2 & 0xfffeffff;
  *param_1 = *param_2;
  return param_1;
}


/* ================= FUN_140036720 @ 140036720  size=441  callers=6 callees=8 ================= */
/* CALLERS: FUN_14002b940, FUN_14002bb30, FUN_14004cf60, FUN_14004d5e0, FUN_140057820, FUN_1400578b0 */
/* CALLEES: FUN_140035970, FUN_140036ec0, QString, fromAscii_helper, getOpenFileName, operator=, toNativeSeparators, ~QString */

int FUN_140036720(undefined8 param_1,undefined8 param_2,undefined4 param_3,char *param_4,
                 undefined8 param_5,char *param_6,char *param_7)

{
  int iVar1;
  QString *pQVar2;
  undefined8 uVar3;
  Data *local_48;
  longlong local_40;
  QString local_38 [8];
  QString local_30 [8];
  Data *local_28;
  Data *local_20;
  QString local_18 [16];
  
  QString::QString((QString *)&local_40);
  QString::QString(local_30);
  QString::QString(local_38);
  pQVar2 = (QString *)FUN_140035970(&local_20,param_5);
  QString::operator=(local_38,pQVar2);
  QString::~QString((QString *)&local_20);
  local_48 = QString::fromAscii_helper(param_7,-1);
  QString::operator=(local_30,(QString *)&local_48);
  QString::~QString((QString *)&local_48);
  local_20 = QString::fromAscii_helper(param_6,-1);
  local_28 = QString::fromAscii_helper(param_4,-1);
  local_48 = (Data *)((ulonglong)local_48 & 0xffffffff00000000);
  pQVar2 = (QString *)
           QFileDialog::getOpenFileName
                     (local_18,param_1,&local_28,local_38,&local_20,local_30,&local_48);
  QString::operator=((QString *)&local_40,pQVar2);
  QString::~QString(local_18);
  QString::~QString((QString *)&local_28);
  QString::~QString((QString *)&local_20);
  pQVar2 = (QString *)QDir::toNativeSeparators(local_18);
  QString::operator=((QString *)&local_40,pQVar2);
  QString::~QString(local_18);
  uVar3 = QString::QString(local_18,(QString *)&local_40);
  FUN_140036ec0(param_2,uVar3,param_3);
  iVar1 = *(int *)(local_40 + 8);
  QString::~QString(local_38);
  QString::~QString(local_30);
  QString::~QString((QString *)&local_40);
  return (iVar1 != 0) - 1;
}


/* ================= FUN_1400368e0 @ 1400368e0  size=740  callers=4 callees=17 ================= */
/* CALLERS: FUN_140047390, FUN_1400476d0, FUN_14004d460, FUN_14004d650 */
/* CALLEES: FUN_140027700, FUN_1400277b0, FUN_140035970, FUN_140036ec0, FUN_1400444f0, FUN_1400591dc, QString, __security_check_cookie, fromAscii_helper, getSaveFileName, memcpy, operator=, strrchr, toAscii, toNativeSeparators, ~QByteArray, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

int FUN_1400368e0(undefined8 param_1,undefined8 param_2,undefined4 param_3,char *param_4,
                 undefined8 param_5,char *param_6,char *param_7)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  QString *pQVar4;
  undefined8 uVar5;
  longlong *plVar6;
  char *_Src;
  uint uVar7;
  char *pcVar8;
  undefined1 auStack_3e8 [32];
  Data **local_3c8;
  QString *local_3c0;
  Data **local_3b8;
  undefined1 *local_3b0;
  undefined4 local_3a8;
  QString local_398 [8];
  longlong local_390;
  Data *local_388;
  QString local_380 [8];
  Data *local_378;
  QString local_370 [8];
  Data *local_368;
  undefined1 local_360 [8];
  undefined1 local_358 [256];
  QString local_258 [256];
  undefined1 local_158 [256];
  ulonglong local_58;
  
  local_58 = DAT_140096fe8 ^ (ulonglong)auStack_3e8;
  QString::QString((QString *)&local_390);
  QString::QString(local_380);
  QString::QString(local_370);
  pQVar4 = (QString *)FUN_140035970(&local_378,param_5);
  QString::operator=(local_370,pQVar4);
  QString::~QString((QString *)&local_378);
  local_388 = QString::fromAscii_helper(param_7,-1);
  QString::operator=(local_380,(QString *)&local_388);
  QString::~QString((QString *)&local_388);
  local_378 = QString::fromAscii_helper(param_6,-1);
  local_368 = QString::fromAscii_helper(param_4,-1);
  local_388 = (Data *)((ulonglong)local_388 & 0xffffffff00000000);
  local_3b8 = &local_388;
  local_3c0 = local_380;
  local_3c8 = &local_378;
  pQVar4 = (QString *)QFileDialog::getSaveFileName(local_398,param_1,&local_368,local_370);
  QString::operator=((QString *)&local_390,pQVar4);
  QString::~QString(local_398);
  QString::~QString((QString *)&local_368);
  QString::~QString((QString *)&local_378);
  pQVar4 = (QString *)QDir::toNativeSeparators(local_398);
  QString::operator=((QString *)&local_390,pQVar4);
  QString::~QString(local_398);
  uVar5 = QString::QString(local_398,(QString *)&local_390);
  FUN_140036ec0(param_2,uVar5,param_3);
  iVar2 = *(int *)(local_390 + 8);
  if (iVar2 != 0) {
    local_3a8 = 0x100;
    local_3b0 = local_358;
    local_3b8 = (Data **)CONCAT44(local_3b8._4_4_,0x100);
    local_3c0 = local_258;
    local_3c8 = (Data **)CONCAT44(local_3c8._4_4_,0x100);
    FUN_1400444f0(param_2,local_360,3,local_158);
    uVar3 = FUN_1400277b0(local_358);
    if (uVar3 < 2) {
      plVar6 = (longlong *)QString::toAscii(local_380);
      _Src = strrchr(*(char **)(*plVar6 + 0x10),0x2e);
      QByteArray::~QByteArray((QByteArray *)local_398);
      if (_Src != (char *)0x0) {
        pcVar8 = _Src + 1;
        cVar1 = *pcVar8;
        while (cVar1 != ')') {
          pcVar8 = pcVar8 + 1;
          cVar1 = *pcVar8;
        }
        uVar7 = (int)pcVar8 - (int)_Src;
        uVar3 = 0xff;
        if (uVar7 < 0xff) {
          uVar3 = uVar7;
        }
        memcpy(local_358,_Src,(ulonglong)uVar3);
        if (0xff < (ulonglong)uVar3) {
                    /* WARNING: Subroutine does not return */
          FUN_1400591dc();
        }
        local_358[uVar3] = 0;
        FUN_140027700(param_2,local_358,param_3);
      }
    }
  }
  QString::~QString(local_370);
  QString::~QString(local_380);
  QString::~QString((QString *)&local_390);
  return (iVar2 != 0) - 1;
}


/* ================= FUN_140036bd0 @ 140036bd0  size=103  callers=2 callees=5 ================= */
/* CALLERS: FUN_140034630, FUN_140034fd0 */
/* CALLEES: QFontMetrics, fromAscii_helper, width, ~QFontMetrics, ~QString */

int FUN_140036bd0(longlong param_1,char *param_2)

{
  int iVar1;
  Data *local_res8 [2];
  QFontMetrics local_res18 [16];
  
  QFontMetrics::QFontMetrics(local_res18,(QFont *)(*(longlong *)(param_1 + 0x20) + 0x38));
  local_res8[0] = QString::fromAscii_helper(param_2,-1);
  iVar1 = QFontMetrics::width(local_res18,(QString *)local_res8,-1);
  QString::~QString((QString *)local_res8);
  QFontMetrics::~QFontMetrics(local_res18);
  return iVar1;
}


/* ================= FUN_140036c40 @ 140036c40  size=306  callers=1 callees=7 ================= */
/* CALLERS: FUN_140034630 */
/* CALLEES: QPixmap, copy, height, operator=, pixmap, width, ~QPixmap */

undefined4 FUN_140036c40(QIcon *param_1,QPixmap *param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  QPixmap *pQVar5;
  undefined4 uVar6;
  undefined8 local_68;
  undefined8 local_60;
  QPixmap local_58 [24];
  QSize local_40 [24];
  
  QPixmap::QPixmap(local_58);
  uVar6 = 0xffffffff;
  iVar4 = param_4;
  iVar3 = param_3;
  while( true ) {
    local_68 = CONCAT44(iVar4,iVar3);
    QIcon::pixmap(param_1,local_40,(Mode)&local_68,0);
    QPixmap::operator=(local_58,(QPixmap *)local_40);
    QPixmap::~QPixmap((QPixmap *)local_40);
    iVar1 = QPixmap::width(local_58);
    iVar2 = QPixmap::height(local_58);
    if ((iVar1 <= param_3) && (iVar2 <= param_4)) break;
    iVar3 = iVar3 / 2;
    iVar4 = iVar4 / 2;
    if ((iVar3 < 8) || (iVar4 < 8)) goto LAB_140036d46;
  }
  uVar6 = 0;
  local_68 = 0;
  local_60 = 0xffffffffffffffff;
  pQVar5 = (QPixmap *)QPixmap::copy(local_58,(QRect *)local_40);
  QPixmap::operator=(param_2,pQVar5);
  QPixmap::~QPixmap((QPixmap *)local_40);
LAB_140036d46:
  QPixmap::~QPixmap(local_58);
  return uVar6;
}


/* ================= FUN_140036d80 @ 140036d80  size=118  callers=1 callees=7 ================= */
/* CALLERS: FUN_14004c520 */
/* CALLEES: QFont, font, operator=, qInstallMsgHandler, setFont, setPointSize, ~QFont */

void FUN_140036d80(void)

{
  QFont *pQVar1;
  QFont local_28 [16];
  QFont local_18 [24];
  
  QFont::QFont(local_28);
  qInstallMsgHandler(_guard_check_icall);
  pQVar1 = (QFont *)QApplication::font();
  QFont::operator=(local_28,pQVar1);
  QFont::~QFont(local_18);
  QFont::setPointSize(local_28,8);
  QApplication::setFont(local_28,"QWidget");
  QFont::~QFont(local_28);
  return;
}


/* ================= FUN_140036e00 @ 140036e00  size=96  callers=3 callees=2 ================= */
/* CALLERS: FUN_14002a410, FUN_14002ce80, FUN_140034630 */
/* CALLEES: setTextFormat, setTextInteractionFlags */

void FUN_140036e00(QLabel *param_1,int param_2,int param_3)

{
  uint uVar1;
  uint local_res10 [2];
  
  QLabel::setTextFormat(param_1,1);
  uVar1 = -(uint)(param_2 != 0) & 4;
  local_res10[0] = uVar1 | 8;
  if (param_3 == 0) {
    local_res10[0] = uVar1;
  }
  QLabel::setTextInteractionFlags(param_1,local_res10);
                    /* WARNING: Could not recover jumptable at 0x000140036e59. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QLabel::setOpenExternalLinks(param_1,true);
  return;
}


/* ================= FUN_140036e60 @ 140036e60  size=88  callers=1 callees=5 ================= */
/* CALLERS: FUN_14002a1f0 */
/* CALLEES: QUrl, fromAscii_helper, openUrl, ~QString, ~QUrl */

int FUN_140036e60(char *param_1)

{
  bool bVar1;
  QUrl *pQVar2;
  Data *local_res10;
  QUrl local_res18 [16];
  
  local_res10 = QString::fromAscii_helper(param_1,-1);
  pQVar2 = (QUrl *)QUrl::QUrl(local_res18,(QString *)&local_res10);
  bVar1 = QDesktopServices::openUrl(pQVar2);
  QUrl::~QUrl(local_res18);
  QString::~QString((QString *)&local_res10);
  return bVar1 - 1;
}


/* ================= FUN_140036ec0 @ 140036ec0  size=98  callers=6 callees=3 ================= */
/* CALLERS: FUN_14002be90, FUN_14002c250, FUN_140036720, FUN_1400368e0, FUN_140050d00, FUN_1400515f0 */
/* CALLEES: FUN_140027140, toLocal8Bit, ~QByteArray */

void FUN_140036ec0(undefined8 param_1,QString *param_2,undefined4 param_3)

{
  longlong *plVar1;
  QByteArray local_res20 [8];
  
  plVar1 = (longlong *)QString::toLocal8Bit(param_2);
  FUN_140027140(param_1,*(undefined8 *)(*plVar1 + 0x10),param_3);
  QByteArray::~QByteArray(local_res20);
                    /* WARNING: Could not recover jumptable at 0x000140036f1b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QString::~QString(param_2);
  return;
}


/* ================= FUN_140036f30 @ 140036f30  size=67  callers=5 callees=3 ================= */
/* CALLERS: FUN_14002a410, FUN_14002d5c0, FUN_140034630, FUN_14004d170, FUN_140050140 */
/* CALLEES: FUN_140036710, setWindowFlags, windowFlags */

void FUN_140036f30(QWidget *param_1)

{
  undefined4 *puVar1;
  undefined4 local_res10 [2];
  undefined1 local_res18 [16];
  
  puVar1 = (undefined4 *)QWidget::windowFlags(param_1);
  local_res10[0] = *puVar1;
  puVar1 = (undefined4 *)FUN_140036710(local_res18,local_res10);
  local_res10[0] = *puVar1;
  QWidget::setWindowFlags(param_1,local_res10);
  return;
}


/* ================= FUN_140036f80 @ 140036f80  size=155  callers=1 callees=8 ================= */
/* CALLERS: FUN_1400360a0 */
/* CALLEES: QBrush, QColor, QPalette, __security_check_cookie, setBrush, setPalette, ~QBrush, ~QPalette */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140036f80(QWidget *param_1)

{
  QBrush *pQVar1;
  undefined1 auStack_58 [32];
  QBrush local_38 [8];
  QPalette local_30 [16];
  QColor local_20 [16];
  ulonglong local_10;
  
  local_10 = DAT_140096fe8 ^ (ulonglong)auStack_58;
  QPalette::QPalette(local_30);
  QColor::QColor(local_20,0x13);
  pQVar1 = (QBrush *)QBrush::QBrush(local_38,local_20,1);
  QPalette::setBrush(local_30,5,9,pQVar1);
  QBrush::~QBrush(local_38);
  QWidget::setPalette(param_1,local_30);
  QPalette::~QPalette(local_30);
  return;
}


/* ================= FUN_140037020 @ 140037020  size=343  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400359e0 */
/* CALLEES: FUN_140037180, FUN_1400371e0, strstr */

undefined8 FUN_140037020(char *param_1,undefined8 *param_2)

{
  int iVar1;
  char *_Str;
  char *pcVar2;
  char *pcVar3;
  undefined8 uVar4;
  char *pcVar5;
  int iVar6;
  
  _Str = (char *)FUN_140037180();
  pcVar2 = strstr(param_1,"[");
  pcVar3 = (char *)0x0;
  if (pcVar2 != (char *)0x0) {
    pcVar3 = strstr(pcVar2,"]");
  }
  if (_Str == (char *)0x0) {
    uVar4 = 0;
  }
  else {
    iVar6 = (int)_Str;
    if (pcVar2 != (char *)0x0) {
      while ((pcVar3 != (char *)0x0 && (pcVar2 < _Str))) {
        if (pcVar2 + 1 == _Str) {
          pcVar5 = strstr(_Str," ");
          *param_2 = pcVar2;
          param_2[1] = pcVar3 + 1;
          param_2[2] = _Str;
          if ((pcVar5 == (char *)0x0) || (pcVar3 < pcVar5)) {
            param_2[3] = _Str;
            iVar6 = (int)pcVar3 - iVar6;
            *(int *)(param_2 + 4) = iVar6;
            *(int *)((longlong)param_2 + 0x24) = iVar6;
          }
          else {
            param_2[3] = pcVar5 + 1;
            *(int *)(param_2 + 4) = (int)pcVar5 - iVar6;
            *(int *)((longlong)param_2 + 0x24) = ((int)pcVar3 - (int)pcVar5) + -1;
          }
          goto LAB_14003715f;
        }
        pcVar2 = strstr(pcVar2 + 1,"[");
        if (pcVar2 == (char *)0x0) break;
        if (pcVar3 < pcVar2) {
          pcVar3 = strstr(pcVar3 + 1,"]");
        }
      }
    }
    pcVar2 = _Str;
    iVar1 = FUN_1400371e0(*_Str);
    while (iVar1 != 1) {
      pcVar3 = pcVar2 + 1;
      pcVar2 = pcVar2 + 1;
      iVar1 = FUN_1400371e0(*pcVar3);
    }
    param_2[1] = pcVar2;
    iVar6 = (int)pcVar2 - iVar6;
    *(int *)(param_2 + 4) = iVar6;
    *(int *)((longlong)param_2 + 0x24) = iVar6;
    *param_2 = _Str;
    param_2[2] = _Str;
    param_2[3] = _Str;
LAB_14003715f:
    uVar4 = 1;
  }
  return uVar4;
}


/* ================= FUN_140037180 @ 140037180  size=89  callers=1 callees=1 ================= */
/* CALLERS: FUN_140037020 */
/* CALLEES: strstr */

char * FUN_140037180(char *param_1)

{
  char *pcVar1;
  char *pcVar2;
  
  pcVar1 = strstr(param_1,"http://");
  pcVar2 = strstr(param_1,"https://");
  if ((pcVar1 != (char *)0x0) && (pcVar2 != (char *)0x0)) {
    if (pcVar1 < pcVar2) {
      pcVar2 = pcVar1;
    }
    return pcVar2;
  }
  if (pcVar1 != (char *)0x0) {
    pcVar2 = pcVar1;
  }
  return pcVar2;
}


/* ================= FUN_1400371e0 @ 1400371e0  size=39  callers=1 callees=0 ================= */
/* CALLERS: FUN_140037020 */

undefined8 FUN_1400371e0(char param_1)

{
  char *pcVar1;
  uint uVar2;
  
  uVar2 = 0;
  pcVar1 = "";
  do {
    if (param_1 == *pcVar1) {
      return 1;
    }
    uVar2 = uVar2 + 1;
    pcVar1 = pcVar1 + 1;
  } while (uVar2 < 9);
  return 0;
}


/* ================= FUN_140037210 @ 140037210  size=83  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400360a0 */
/* CALLEES: FUN_140043ad0, QLineEdit */

QLineEdit * FUN_140037210(QLineEdit *param_1,QWidget *param_2)

{
  int iVar1;
  bool bVar2;
  
  QLineEdit::QLineEdit(param_1,param_2);
  *(undefined ***)param_1 = CustLineEdit::vftable;
  *(undefined ***)(param_1 + 0x10) = CustLineEdit::vftable;
  bVar2 = DAT_14085cf30 == '\0';
  *(undefined2 *)(param_1 + 0x28) = 0;
  if (bVar2) {
    iVar1 = FUN_140043ad0();
    DAT_14085cf31 = iVar1 == 0x1008;
    DAT_14085cf30 = '\x01';
  }
  return param_1;
}


/* ================= FUN_140037270 @ 140037270  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400372a0 */

void FUN_140037270(QLineEdit *param_1)

{
  *(undefined ***)param_1 = CustLineEdit::vftable;
  *(undefined ***)(param_1 + 0x10) = CustLineEdit::vftable;
                    /* WARNING: Could not recover jumptable at 0x000140037285. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QLineEdit::~QLineEdit(param_1);
  return;
}


/* ================= FUN_14003728c @ 14003728c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400372a0 */

void FUN_14003728c(longlong param_1)

{
  FUN_1400372a0(param_1 + -0x10);
  return;
}


/* ================= FUN_1400372a0 @ 1400372a0  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003728c */
/* CALLEES: FUN_140037270, free */

void * FUN_1400372a0(void *param_1,ulonglong param_2)

{
  FUN_140037270();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_1400372e0 @ 1400372e0  size=185  callers=0 callees=8 ================= */
/* CALLEES: QPalette, changeEvent, fromAscii_helper, isEnabled, setPalette, setStyleSheet, ~QPalette, ~QString */

void FUN_1400372e0(QWidget *param_1,QEvent *param_2)

{
  bool bVar1;
  Data *local_res18 [2];
  QPalette local_18 [16];
  
  QPalette::QPalette(local_18);
  if ((DAT_14085cf31 != '\0') && (*(short *)(param_2 + 0x10) == 0x62)) {
    bVar1 = QWidget::isEnabled(param_1);
    if (bVar1) {
      local_res18[0] = QString::fromAscii_helper((char *)0x0,-1);
      QWidget::setStyleSheet(param_1,(QString *)local_res18);
    }
    else {
      local_res18[0] =
           QString::fromAscii_helper
                     ("QLineEdit{border: 1px solid #CCCCCC; background-color: transparent;} QLineEdit:hover{border: 1px solid #CCCCCC; background-color: transparent;}"
                      ,-1);
      QWidget::setStyleSheet(param_1,(QString *)local_res18);
    }
    QString::~QString((QString *)local_res18);
    QWidget::setPalette(param_1,local_18);
  }
  QLineEdit::changeEvent((QLineEdit *)param_1,param_2);
  QPalette::~QPalette(local_18);
  return;
}


/* ================= FUN_1400373a0 @ 1400373a0  size=289  callers=1 callees=7 ================= */
/* CALLERS: FUN_14002a410 */
/* CALLEES: FUN_1400377e0, FUN_140058d94, QComboBox, QRegExpValidator, connect, lineEdit, setEditable */

QComboBox * FUN_1400373a0(QComboBox *param_1,QWidget *param_2,char *param_3)

{
  QRegExpValidator *this;
  QLineEdit *pQVar1;
  
  QComboBox::QComboBox(param_1,param_2);
  *(undefined ***)param_1 = TIFSpeedComboBox::vftable;
  *(undefined ***)(param_1 + 0x10) = TIFSpeedComboBox::vftable;
  *(undefined8 *)(param_1 + 0x28) = 0;
  *(undefined8 *)(param_1 + 0x30) = 0;
  *(undefined8 *)(param_1 + 0x38) = 0;
  *(undefined8 *)(param_1 + 0x40) = 0;
  QComboBox::setEditable(param_1,true);
  *(undefined **)(param_1 + 0x28) = &DAT_140070ed0;
  *(undefined4 *)(param_1 + 0x38) = 0x21;
  this = (QRegExpValidator *)FUN_140058d94(0x18);
  if (this == (QRegExpValidator *)0x0) {
    this = (QRegExpValidator *)0x0;
  }
  else {
    QRegExpValidator::QRegExpValidator(this,(QObject *)param_1);
    *(undefined ***)this = QRegExpValidator::vftable;
  }
  *(QRegExpValidator **)(param_1 + 0x40) = this;
  pQVar1 = QComboBox::lineEdit(param_1);
  QObject::connect((QObject *)pQVar1,"2editingFinished(void)",(QObject *)param_1,
                   "1_SlotOnCustSpeedEntered(void)",0);
  QObject::connect((QObject *)param_1,"2currentIndexChanged(int)",(QObject *)param_1,
                   "2SignalSpeedChanged(void)",0);
  if ((param_2 != (QWidget *)0x0) && (param_3 != (char *)0x0)) {
    QObject::connect((QObject *)param_1,"2SignalSpeedChanged(void)",(QObject *)param_2,param_3,0);
  }
  FUN_1400377e0(param_1);
  return param_1;
}


/* ================= FUN_1400374d0 @ 1400374d0  size=59  callers=1 callees=1 ================= */
/* CALLERS: FUN_140037520 */
/* CALLEES: FUN_140043e60 */

void FUN_1400374d0(QComboBox *param_1)

{
  *(undefined ***)param_1 = TIFSpeedComboBox::vftable;
  *(undefined ***)(param_1 + 0x10) = TIFSpeedComboBox::vftable;
  if (*(longlong *)(param_1 + 0x30) != 0) {
    FUN_140043e60();
  }
                    /* WARNING: Could not recover jumptable at 0x000140037504. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QComboBox::~QComboBox(param_1);
  return;
}


/* ================= FUN_14003750c @ 14003750c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140037520 */

void FUN_14003750c(longlong param_1)

{
  FUN_140037520(param_1 + -0x10);
  return;
}


/* ================= FUN_140037520 @ 140037520  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003750c */
/* CALLEES: FUN_1400374d0, free */

void * FUN_140037520(void *param_1,ulonglong param_2)

{
  FUN_1400374d0();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140037560 @ 140037560  size=86  callers=2 callees=1 ================= */
/* CALLERS: FUN_1400375c0, FUN_1400377e0 */
/* CALLEES: currentIndex */

int FUN_140037560(QComboBox *param_1)

{
  uint uVar1;
  
  uVar1 = QComboBox::currentIndex(param_1);
  if (-1 < (int)uVar1) {
    if ((uVar1 == 0) && (*(int *)(param_1 + 0x3c) != 0)) {
      return *(int *)(param_1 + 0x3c) * 1000;
    }
    if (((*(int *)(param_1 + 0x3c) == 0) || (uVar1 = uVar1 - 1, -1 < (int)uVar1)) &&
       ((int)uVar1 < *(int *)(param_1 + 0x38))) {
      return *(int *)(*(longlong *)(param_1 + 0x28) + (ulonglong)uVar1 * 4);
    }
  }
  return -1;
}


/* ================= FUN_1400375c0 @ 1400375c0  size=30  callers=2 callees=2 ================= */
/* CALLERS: FUN_14002b900, FUN_14002c250 */
/* CALLEES: FUN_140037560, FUN_140037700 */

void FUN_1400375c0(void)

{
  int iVar1;
  
  iVar1 = FUN_140037560();
  if (iVar1 != -1) {
    FUN_140037700(iVar1);
    return;
  }
  return;
}


/* ================= FUN_1400375e0 @ 1400375e0  size=136  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400377e0 */
/* CALLEES: FUN_140037700, FUN_1400377e0, setCurrentIndex */

void FUN_1400375e0(QComboBox *param_1,int param_2)

{
  undefined4 uVar1;
  uint uVar2;
  ulonglong uVar3;
  
  if ((param_2 != -1) && (*(int *)(param_1 + 0x3c) * 1000 != param_2)) {
    uVar3 = 0;
    if (*(uint *)(param_1 + 0x38) != 0) {
      do {
        uVar2 = (uint)uVar3 + 1;
        if (*(int *)(*(longlong *)(param_1 + 0x28) + uVar3 * 4) == param_2) {
          if (*(int *)(param_1 + 0x3c) == 0) {
            uVar2 = (uint)uVar3;
          }
                    /* WARNING: Could not recover jumptable at 0x000140037661. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          QComboBox::setCurrentIndex(param_1,uVar2);
          return;
        }
        uVar3 = (ulonglong)uVar2;
      } while (uVar2 < *(uint *)(param_1 + 0x38));
    }
    QComboBox::setCurrentIndex(param_1,0);
    uVar1 = FUN_140037700(param_2);
    *(undefined4 *)(param_1 + 0x3c) = uVar1;
    FUN_1400377e0(param_1);
  }
  return;
}


/* ================= FUN_140037670 @ 140037670  size=140  callers=2 callees=3 ================= */
/* CALLERS: FUN_14002a410, FUN_140037720 */
/* CALLEES: FUN_140037700, FUN_1400377e0, setCurrentIndex */

void FUN_140037670(QComboBox *param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  ulonglong uVar5;
  longlong lVar6;
  
  if ((param_2 != -1) && (*(int *)(param_1 + 0x3c) != param_2)) {
    uVar5 = (ulonglong)*(uint *)(param_1 + 0x38);
    uVar3 = 0;
    if (*(uint *)(param_1 + 0x38) != 0) {
      lVar6 = *(longlong *)(param_1 + 0x28);
      do {
        uVar4 = (ulonglong)((int)uVar3 + 1);
        iVar2 = FUN_140037700(*(undefined4 *)(lVar6 + uVar3 * 4));
        if (iVar2 == param_2) {
          uVar1 = (uint)uVar4;
          if (*(int *)(param_1 + 0x3c) == 0) {
            uVar1 = (uint)uVar3;
          }
                    /* WARNING: Could not recover jumptable at 0x0001400376f5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          QComboBox::setCurrentIndex(param_1,uVar1);
          return;
        }
        uVar3 = uVar4 & 0xffffffff;
      } while ((uint)uVar4 < (uint)uVar5);
    }
    QComboBox::setCurrentIndex(param_1,0);
    *(int *)(param_1 + 0x3c) = param_2;
    FUN_1400377e0(param_1);
  }
  return;
}


/* ================= FUN_140037700 @ 140037700  size=19  callers=5 callees=0 ================= */
/* CALLERS: FUN_1400375c0, FUN_1400375e0, FUN_140037670, FUN_140037720, FUN_1400377e0 */

ulonglong FUN_140037700(int param_1)

{
  return (ulonglong)(param_1 + 500) / 1000;
}


/* ================= FUN_140037720 @ 140037720  size=189  callers=1 callees=11 ================= */
/* CALLERS: FUN_14003c520 */
/* CALLEES: FUN_140027140, FUN_1400272c0, FUN_140037670, FUN_140037700, FUN_14003c540, __security_check_cookie, lineEdit, text, toAscii, ~QByteArray, ~QString */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140037720(QComboBox *param_1)

{
  int iVar1;
  QLineEdit *this;
  QString *this_00;
  longlong *plVar2;
  undefined1 auStack_58 [32];
  undefined4 local_38 [2];
  QByteArray local_30 [8];
  QString local_28 [8];
  char local_20 [16];
  ulonglong local_10;
  
  local_10 = DAT_140096fe8 ^ (ulonglong)auStack_58;
  this = QComboBox::lineEdit(param_1);
  this_00 = (QString *)QLineEdit::text(this);
  plVar2 = (longlong *)QString::toAscii(this_00);
  FUN_140027140(local_20,*(undefined8 *)(*plVar2 + 0x10),0x10);
  QByteArray::~QByteArray(local_30);
  QString::~QString(local_28);
  if ((local_20[0] == '\0') || (iVar1 = FUN_1400272c0(local_20,local_38), iVar1 < 0)) {
    local_38[0] = FUN_140037700(**(undefined4 **)(param_1 + 0x28));
  }
  FUN_140037670(param_1,local_38[0]);
  FUN_14003c540(param_1);
  return;
}


/* ================= FUN_1400377e0 @ 1400377e0  size=681  callers=3 callees=22 ================= */
/* CALLERS: FUN_1400373a0, FUN_1400375e0, FUN_140037670 */
/* CALLEES: FUN_1400277b0, FUN_14002c8f0, FUN_140037560, FUN_1400375e0, FUN_140037700, QIcon, QRegExp, QVariant, __security_check_cookie, blockSignals, clear, count, fromAscii_helper, insertItem, lineEdit, setPattern, setRegExp, setValidator, ~QIcon, ~QRegExp, ~QString, ~QVariant */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_1400377e0(QObject *param_1)

{
  bool bVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  QVariant *pQVar5;
  QIcon *pQVar6;
  QLineEdit *this;
  uint *puVar7;
  ulonglong uVar8;
  uint uVar9;
  uint uVar10;
  undefined1 auStackY_a8 [32];
  Data *local_78;
  QRegExp local_70 [8];
  QIcon local_68 [8];
  QVariant local_60 [16];
  char local_50 [32];
  ulonglong local_30;
  
  local_30 = DAT_140096fe8 ^ (ulonglong)auStackY_a8;
  QRegExp::QRegExp(local_70);
  uVar2 = FUN_140037560(param_1);
  bVar1 = QObject::blockSignals(param_1,true);
  QComboBox::clear((QComboBox *)param_1);
  if (*(int *)(param_1 + 0x3c) != 0) {
    FUN_14002c8f0(local_50,0x20,&DAT_140060a50);
    local_78 = QString::fromAscii_helper(local_50,-1);
    pQVar5 = (QVariant *)QVariant::QVariant(local_60);
    iVar3 = QComboBox::count((QComboBox *)param_1);
    pQVar6 = (QIcon *)QIcon::QIcon(local_68);
    QComboBox::insertItem((QComboBox *)param_1,iVar3,pQVar6,(QString *)&local_78,pQVar5);
    QIcon::~QIcon(local_68);
    QVariant::~QVariant(local_60);
    QString::~QString((QString *)&local_78);
  }
  uVar9 = 0;
  uVar10 = 0;
  if (*(int *)(param_1 + 0x38) != 0) {
    do {
      uVar4 = FUN_140037700(*(undefined4 *)(*(longlong *)(param_1 + 0x28) + (ulonglong)uVar10 * 4));
      FUN_14002c8f0(local_50,0x20,&DAT_140060a50,uVar4);
      local_78 = QString::fromAscii_helper(local_50,-1);
      pQVar5 = (QVariant *)QVariant::QVariant(local_60);
      iVar3 = QComboBox::count((QComboBox *)param_1);
      pQVar6 = (QIcon *)QIcon::QIcon(local_68);
      QComboBox::insertItem((QComboBox *)param_1,iVar3,pQVar6,(QString *)&local_78,pQVar5);
      QIcon::~QIcon(local_68);
      QVariant::~QVariant(local_60);
      QString::~QString((QString *)&local_78);
      uVar10 = uVar10 + 1;
    } while (uVar10 < *(uint *)(param_1 + 0x38));
  }
  QObject::blockSignals(param_1,bVar1);
  if (*(uint *)(param_1 + 0x38) != 0) {
    puVar7 = *(uint **)(param_1 + 0x28);
    uVar8 = (ulonglong)*(uint *)(param_1 + 0x38);
    do {
      uVar10 = *puVar7;
      if (*puVar7 < uVar9) {
        uVar10 = uVar9;
      }
      uVar9 = uVar10;
      puVar7 = puVar7 + 1;
      uVar8 = uVar8 - 1;
    } while (uVar8 != 0);
  }
  uVar4 = FUN_140037700(uVar9);
  FUN_14002c8f0(local_50,0x20,&DAT_140060a50,uVar4);
  uVar4 = FUN_1400277b0(local_50);
  FUN_14002c8f0(local_50,0x20,"[0-9]{0,%d}",uVar4);
  local_78 = QString::fromAscii_helper(local_50,-1);
  QRegExp::setPattern(local_70,(QString *)&local_78);
  QString::~QString((QString *)&local_78);
  this = QComboBox::lineEdit((QComboBox *)param_1);
  QRegExpValidator::setRegExp(*(QRegExpValidator **)(param_1 + 0x40),local_70);
  QLineEdit::setValidator(this,*(QValidator **)(param_1 + 0x40));
  FUN_1400375e0(param_1,uVar2);
  QRegExp::~QRegExp(local_70);
  return;
}


/* ================= FUN_140037a90 @ 140037a90  size=166  callers=1 callees=2 ================= */
/* CALLERS: FUN_14002fe40 */
/* CALLEES: FUN_140038ab0, QHeaderView */

QHeaderView * FUN_140037a90(QHeaderView *param_1,uint param_2,Orientation param_3,QWidget *param_4)

{
  QHeaderView::QHeaderView(param_1,param_3,param_4);
  *(undefined ***)param_1 = UtilFilterHeader::vftable;
  *(undefined ***)(param_1 + 0x10) = UtilFilterHeader::vftable;
  *(code **)(param_1 + 0x50) = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  *(QWidget **)(param_1 + 0x40) = param_4;
  param_1[0x28] = (QHeaderView)((byte)param_2 & 1);
  param_1[0x29] = (QHeaderView)((byte)(param_2 >> 1) & 1);
  param_1[0x2a] = (QHeaderView)((byte)(param_2 >> 2) & 1);
  param_1[0x2b] = (QHeaderView)((byte)(param_2 >> 3) & 1);
  param_1[0x2c] = (QHeaderView)((byte)(param_2 >> 4) & 1);
  FUN_140038ab0(param_1);
  return param_1;
}


/* ================= FUN_140037b40 @ 140037b40  size=44  callers=0 callees=1 ================= */
/* CALLEES: FUN_140038d80 */

void FUN_140037b40(longlong param_1)

{
  int iVar1;
  int *piVar2;
  undefined8 *puVar3;
  
  puVar3 = (undefined8 *)(param_1 + 0x28);
  piVar2 = (int *)*puVar3;
  if (piVar2 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar2;
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (iVar1 == 1) {
      FUN_140038d80(puVar3,*puVar3);
    }
  }
  return;
}


/* ================= FUN_140037b70 @ 140037b70  size=80  callers=1 callees=1 ================= */
/* CALLERS: FUN_140037c20 */
/* CALLEES: FUN_140038d80 */

void FUN_140037b70(QHeaderView *param_1)

{
  int iVar1;
  int *piVar2;
  QHeaderView *pQVar3;
  
  *(undefined ***)param_1 = UtilFilterHeader::vftable;
  *(undefined ***)(param_1 + 0x10) = UtilFilterHeader::vftable;
  pQVar3 = param_1 + 0x50;
  piVar2 = *(int **)pQVar3;
  if (piVar2 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar2;
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (iVar1 == 1) {
      FUN_140038d80(pQVar3,*(undefined8 *)pQVar3);
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140037bb9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QHeaderView::~QHeaderView(param_1);
  return;
}


/* ================= FUN_140037bc0 @ 140037bc0  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140037be0 */

void FUN_140037bc0(longlong param_1)

{
  FUN_140037be0(param_1 + -0x10);
  return;
}


/* ================= FUN_140037bcc @ 140037bcc  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140037c20 */

void FUN_140037bcc(longlong param_1)

{
  FUN_140037c20(param_1 + -0x10);
  return;
}


/* ================= FUN_140037be0 @ 140037be0  size=53  callers=1 callees=2 ================= */
/* CALLERS: FUN_140037bc0 */
/* CALLEES: free, ~QLineEdit */

QLineEdit * FUN_140037be0(QLineEdit *param_1,uint param_2)

{
  QLineEdit::~QLineEdit(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140037c20 @ 140037c20  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_140037bcc */
/* CALLEES: FUN_140037b70, free */

void * FUN_140037c20(void *param_1,ulonglong param_2)

{
  FUN_140037b70();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140037c60 @ 140037c60  size=623  callers=6 callees=11 ================= */
/* CALLERS: FUN_140038310, FUN_140038440, FUN_1400388c0, FUN_140038c20, FUN_140039070, FUN_14003c8d0 */
/* CALLEES: FUN_140038d70, logicalIndex, move, offset, resize, sectionPosition, sectionSize, sizeHint, stretchLastSection, viewport, visualIndex */

void FUN_140037c60(QAbstractScrollArea *param_1)

{
  undefined4 uVar1;
  QWidget *pQVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  QWidget *pQVar8;
  longlong lVar9;
  int iVar10;
  int local_res8;
  QWidget *local_res10;
  int local_res18;
  int local_res1c;
  int local_res20;
  undefined4 local_res24;
  longlong local_88;
  int local_80;
  int local_7c;
  int local_78;
  undefined4 local_74;
  undefined1 local_70 [16];
  undefined1 local_60 [32];
  
  iVar10 = 0;
  local_res10 = (QWidget *)0x0;
  pQVar8 = QAbstractScrollArea::viewport(param_1);
  local_res8 = (*(int *)(*(longlong *)(pQVar8 + 0x20) + 0x1c) -
               *(int *)(*(longlong *)(pQVar8 + 0x20) + 0x14)) + 1;
  if (0 < *(int *)(param_1 + 0x34)) {
    local_88 = 0;
    do {
      if (**(int **)(param_1 + 0x50) != 1) {
        FUN_140038d70(param_1 + 0x50);
      }
      pQVar8 = *(QWidget **)(*(longlong *)(param_1 + 0x50) + 0x18 + local_88);
      iVar4 = QHeaderView::visualIndex((QHeaderView *)param_1,iVar10);
      bVar3 = QHeaderView::stretchLastSection((QHeaderView *)param_1);
      if ((((!bVar3) || (pQVar2 = pQVar8, iVar4 != *(int *)(param_1 + 0x3c))) &&
          (pQVar2 = local_res10, -1 < iVar4)) && (pQVar8 != (QWidget *)0x0)) {
        lVar9 = (**(code **)(*(longlong *)pQVar8 + 0x60))(pQVar8,local_70);
        uVar1 = *(undefined4 *)(lVar9 + 4);
        iVar6 = QHeaderView::sectionSize((QHeaderView *)param_1,iVar10);
        lVar9 = QHeaderView::sizeHint((QHeaderView *)param_1);
        iVar4 = *(int *)(param_1 + 0x30);
        iVar5 = *(int *)(lVar9 + 4);
        iVar7 = QHeaderView::offset((QHeaderView *)param_1);
        local_res18 = QHeaderView::sectionPosition((QHeaderView *)param_1,iVar10);
        local_res18 = local_res18 + (1 - iVar7);
        local_res1c = iVar4 / 2 + iVar5;
        QWidget::move(pQVar8,(QPoint *)&local_res18);
        local_res20 = iVar6;
        local_res24 = uVar1;
        QWidget::resize(pQVar8,(QSize *)&local_res20);
        local_res8 = local_res8 - iVar6;
      }
      local_res10 = pQVar2;
      iVar10 = iVar10 + 1;
      local_88 = local_88 + 0x10;
    } while (iVar10 < *(int *)(param_1 + 0x34));
    if (local_res10 != (QWidget *)0x0) {
      iVar5 = QHeaderView::logicalIndex((QHeaderView *)param_1,*(int *)(param_1 + 0x3c));
      lVar9 = (**(code **)(*(longlong *)local_res10 + 0x60))(local_res10,local_60);
      uVar1 = *(undefined4 *)(lVar9 + 4);
      iVar6 = QHeaderView::sectionSize((QHeaderView *)param_1,iVar5);
      lVar9 = QHeaderView::sizeHint((QHeaderView *)param_1);
      iVar10 = *(int *)(param_1 + 0x30);
      iVar4 = *(int *)(lVar9 + 4);
      iVar7 = QHeaderView::offset((QHeaderView *)param_1);
      local_80 = QHeaderView::sectionPosition((QHeaderView *)param_1,iVar5);
      local_80 = local_80 + (1 - iVar7);
      local_7c = iVar10 / 2 + iVar4;
      QWidget::move(local_res10,(QPoint *)&local_80);
      local_78 = local_res8;
      if (local_res8 <= iVar6 + -1) {
        local_78 = iVar6 + -1;
      }
      local_74 = uVar1;
      QWidget::resize(local_res10,(QSize *)&local_78);
    }
  }
  return;
}


/* ================= FUN_140037ed0 @ 140037ed0  size=78  callers=1 callees=2 ================= */
/* CALLERS: FUN_140032b20 */
/* CALLEES: FUN_140038d70, setFocus */

void FUN_140037ed0(longlong param_1,int param_2)

{
  if (param_2 < *(int *)(param_1 + 0x34)) {
    if (**(int **)(param_1 + 0x50) != 1) {
      FUN_140038d70((longlong *)(param_1 + 0x50));
    }
    QWidget::setFocus(*(QWidget **)(*(longlong *)(param_1 + 0x50) + 0x18 + (longlong)param_2 * 0x10)
                      ,7);
  }
  return;
}


/* ================= FUN_140037f20 @ 140037f20  size=157  callers=2 callees=4 ================= */
/* CALLERS: FUN_140031210, FUN_1400318b0 */
/* CALLEES: QVariant, currentText, text, ~QString */

QVariant * FUN_140037f20(longlong param_1,QVariant *param_2,int param_3)

{
  int iVar1;
  longlong lVar2;
  QString *pQVar3;
  longlong lVar4;
  QString local_res8 [32];
  
  if (param_3 < *(int *)(param_1 + 0x34)) {
    lVar4 = (longlong)param_3;
    lVar2 = *(longlong *)(param_1 + 0x50);
    iVar1 = *(int *)(lVar2 + 0x10 + lVar4 * 0x10);
    if (iVar1 == 0) {
      pQVar3 = (QString *)QLineEdit::text(*(QLineEdit **)(lVar2 + 0x18 + lVar4 * 0x10));
      QVariant::QVariant(param_2,pQVar3);
      QString::~QString(local_res8);
      return param_2;
    }
    if (iVar1 == 1) {
      pQVar3 = (QString *)QComboBox::currentText(*(QComboBox **)(lVar2 + 0x18 + lVar4 * 0x10));
      QVariant::QVariant(param_2,pQVar3);
      QString::~QString(local_res8);
      return param_2;
    }
  }
  QVariant::QVariant(param_2);
  return param_2;
}


/* ================= FUN_140037fc0 @ 140037fc0  size=110  callers=1 callees=1 ================= */
/* CALLERS: FUN_14002f9c0 */
/* CALLEES: FUN_140038d70 */

undefined8 FUN_140037fc0(longlong param_1,int param_2)

{
  longlong *plVar1;
  
  if (*(int *)(param_1 + 0x34) <= param_2) {
    return 0;
  }
  plVar1 = (longlong *)(param_1 + 0x50);
  if (**(int **)(param_1 + 0x50) != 1) {
    FUN_140038d70(plVar1);
  }
  if (((int *)*plVar1)[(longlong)param_2 * 4 + 4] == 1) {
    if (*(int *)*plVar1 != 1) {
      FUN_140038d70(plVar1);
    }
    return *(undefined8 *)(*plVar1 + 0x18 + (longlong)param_2 * 0x10);
  }
  return 0;
}


/* ================= FUN_140038030 @ 140038030  size=81  callers=2 callees=1 ================= */
/* CALLERS: FUN_14002f9c0, FUN_14002fa80 */
/* CALLEES: FUN_140038d70 */

bool FUN_140038030(longlong param_1,int param_2)

{
  bool bVar1;
  
  if (param_2 < *(int *)(param_1 + 0x34)) {
    if (**(int **)(param_1 + 0x50) != 1) {
      FUN_140038d70((longlong *)(param_1 + 0x50));
    }
                    /* WARNING: Could not recover jumptable at 0x000140038072. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    bVar1 = QWidget::hasFocus(*(QWidget **)
                               (*(longlong *)(param_1 + 0x50) + 0x18 + (longlong)param_2 * 0x10));
    return bVar1;
  }
  return false;
}


/* ================= FUN_140038090 @ 140038090  size=615  callers=1 callees=21 ================= */
/* CALLERS: FUN_14003c8d0 */
/* CALLEES: FUN_140029d60, FUN_140058d94, QAction, QMenu, QVariant, addAction, addSeparator, connect, exec, isEmpty, isSectionHidden, mapToGlobal, model, setCheckable, setChecked, setData, setText, toString, ~QModelIndex, ~QString, ~QVariant */

void FUN_140038090(QAbstractItemView *param_1)

{
  bool bVar1;
  int iVar2;
  QMenu *this;
  QAbstractItemModel *pQVar3;
  QAction *this_00;
  QVariant *this_01;
  QString *pQVar4;
  QPoint *pQVar5;
  QAction *this_02;
  int iVar6;
  QAction *pQVar7;
  QMenu *this_03;
  QAction *pQVar8;
  QMenu *local_res18;
  QAction *local_res20;
  QVariant local_78 [16];
  QVariant local_68 [16];
  undefined8 local_58;
  undefined8 local_50;
  undefined8 uStack_48;
  
  this = (QMenu *)FUN_140058d94(0x28);
  pQVar8 = (QAction *)0x0;
  this_03 = (QMenu *)pQVar8;
  local_res18 = this;
  if (this != (QMenu *)0x0) {
    QMenu::QMenu(this,(QWidget *)param_1);
    *(undefined ***)this = QMenu::vftable;
    *(undefined ***)(this + 0x10) = QMenu::vftable;
    this_03 = this;
  }
  if (this_03 != (QMenu *)0x0) {
    if (param_1[0x2a] == (QAbstractItemView)0x1) {
      pQVar3 = QAbstractItemView::model(param_1);
      local_58 = 0xffffffffffffffff;
      local_50 = 0;
      uStack_48 = 0;
      iVar2 = (**(code **)(*(longlong *)pQVar3 + 0x70))(pQVar3,&local_58);
      QModelIndex::~QModelIndex((QModelIndex *)&local_58);
      pQVar7 = pQVar8;
      if (0 < iVar2) {
        do {
          this_00 = (QAction *)FUN_140058d94(0x10);
          this_02 = pQVar8;
          local_res20 = this_00;
          if (this_00 != (QAction *)0x0) {
            QAction::QAction(this_00,(QObject *)this_03);
            *(undefined ***)this_00 = QAction::vftable;
            this_02 = this_00;
          }
          iVar6 = (int)pQVar7;
          this_01 = (QVariant *)
                    (**(code **)(*(longlong *)pQVar3 + 0x90))(pQVar3,local_68,pQVar7,1,0);
          pQVar4 = (QString *)QVariant::toString(this_01);
          QAction::setText(this_02,pQVar4);
          QString::~QString((QString *)&local_res18);
          QVariant::~QVariant(local_68);
          QAction::setCheckable(this_02,true);
          bVar1 = QHeaderView::isSectionHidden((QHeaderView *)param_1,iVar6);
          QAction::setChecked(this_02,!bVar1);
          QVariant::QVariant(local_78,iVar6);
          QAction::setData(this_02,local_78);
          QVariant::~QVariant(local_78);
          QObject::connect((QObject *)this_02,"2toggled(bool)",(QObject *)param_1,
                           "1OnToggleColumn(bool)",0);
          QWidget::addAction((QWidget *)this_03,this_02);
          local_58 = 0xffffffffffffffff;
          local_50 = 0;
          uStack_48 = 0;
          iVar2 = (**(code **)(*(longlong *)pQVar3 + 0x70))(pQVar3,&local_58);
          QModelIndex::~QModelIndex((QModelIndex *)&local_58);
          pQVar7 = (QAction *)(ulonglong)(iVar6 + 1U);
        } while ((int)(iVar6 + 1U) < iVar2);
      }
    }
    if (param_1[0x29] == (QAbstractItemView)0x1) {
      QMenu::addSeparator(this_03);
      QWidget::addAction((QWidget *)this_03,*(QAction **)(param_1 + 0x48));
    }
    bVar1 = QMenu::isEmpty(this_03);
    if (!bVar1) {
      pQVar5 = (QPoint *)QWidget::mapToGlobal((QWidget *)param_1,(QPoint *)&local_res18);
      QMenu::exec(this_03,pQVar5,(QAction *)0x0);
    }
    (**(code **)(*(longlong *)this_03 + 0x18))(this_03,1);
  }
  return;
}


/* ================= FUN_140038310 @ 140038310  size=134  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c8d0 */
/* CALLEES: FUN_140037c60, isSectionHidden, logicalIndex */

void FUN_140038310(QHeaderView *param_1,undefined8 param_2,int param_3,int param_4)

{
  bool bVar1;
  int iVar2;
  
  if (*(int *)(param_1 + 0x3c) <= param_4) {
    *(int *)(param_1 + 0x3c) = param_4;
    FUN_140037c60();
    return;
  }
  if ((param_3 == *(int *)(param_1 + 0x3c)) &&
     (*(undefined4 *)(param_1 + 0x3c) = 0xffffffff, -1 < param_3)) {
    while( true ) {
      iVar2 = QHeaderView::logicalIndex(param_1,param_3);
      bVar1 = QHeaderView::isSectionHidden(param_1,iVar2);
      if (!bVar1) break;
      param_3 = param_3 + -1;
      if (param_3 < 0) {
        FUN_140037c60(param_1);
        return;
      }
    }
    *(int *)(param_1 + 0x3c) = param_3;
  }
  FUN_140037c60(param_1);
  return;
}


/* ================= FUN_1400383c0 @ 1400383c0  size=101  callers=1 callees=5 ================= */
/* CALLERS: FUN_14003c8d0 */
/* CALLEES: FUN_140038c20, data, sender, toInt, ~QVariant */

void FUN_1400383c0(QObject *param_1,undefined1 param_2)

{
  int iVar1;
  QObject *this;
  QVariant *this_00;
  QVariant local_18 [16];
  
  this = QObject::sender(param_1);
  if (this != (QObject *)0x0) {
    this_00 = (QVariant *)QAction::data((QAction *)this);
    iVar1 = QVariant::toInt(this_00,(bool *)0x0);
    QVariant::~QVariant(local_18);
    FUN_140038c20(param_1,iVar1,param_2);
  }
  return;
}


/* ================= FUN_140038440 @ 140038440  size=1145  callers=1 callees=27 ================= */
/* CALLERS: FUN_14002ea10 */
/* CALLEES: FUN_140037c60, FUN_140038d70, FUN_140058d94, QComboBox, QIcon, QVariant, clear, connect, count, findText, fromAscii_helper, hide, insertItem, insertItems, lineEdit, setAutoCompletion, setAutoFillBackground, setCurrentIndex, setEditable, setMinimumWidth, setTextElideMode, show, verticalScrollBar, view, ~QIcon, ~QString, ~QVariant */

void FUN_140038440(QObject *param_1,int param_2,QStringList *param_3,longlong *param_4,
                  longlong *param_5,longlong *param_6)

{
  longlong *plVar1;
  longlong lVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  QComboBox *pQVar6;
  QAbstractItemView *pQVar7;
  QLineEdit *pQVar8;
  QVariant *pQVar9;
  QIcon *pQVar10;
  QScrollBar *pQVar11;
  int *piVar12;
  int iVar13;
  QComboBox *pQVar14;
  longlong lVar15;
  QComboBox *this;
  QObject *local_res8;
  undefined4 local_res10 [2];
  Data *local_68;
  QIcon local_60 [8];
  QComboBox *local_58;
  QVariant local_50 [24];
  
  lVar15 = (longlong)param_2;
  if (param_2 < *(int *)(param_1 + 0x34)) {
    local_res8 = param_1;
    if (**(int **)(param_1 + 0x50) != 1) {
      FUN_140038d70(param_1 + 0x50);
    }
    pQVar14 = (QComboBox *)0x0;
    iVar3 = **(int **)(param_1 + 0x50);
    if ((*(int **)(param_1 + 0x50))[lVar15 * 4 + 4] == 1) {
      if (iVar3 != 1) {
        FUN_140038d70(param_1 + 0x50);
      }
      this = *(QComboBox **)(*(longlong *)(param_1 + 0x50) + 0x18 + lVar15 * 0x10);
      QComboBox::clear(this);
    }
    else {
      if (iVar3 != 1) {
        FUN_140038d70(param_1 + 0x50);
      }
      if (*(longlong *)(*(int **)(param_1 + 0x50) + lVar15 * 4 + 6) != 0) {
        if (**(int **)(param_1 + 0x50) != 1) {
          FUN_140038d70(param_1 + 0x50);
        }
        plVar1 = *(longlong **)(*(longlong *)(param_1 + 0x50) + 0x18 + lVar15 * 0x10);
        if (plVar1 != (longlong *)0x0) {
          (**(code **)(*plVar1 + 0x18))(plVar1,1);
        }
      }
      pQVar6 = (QComboBox *)FUN_140058d94(0x28);
      this = pQVar14;
      local_58 = pQVar6;
      if (pQVar6 != (QComboBox *)0x0) {
        QComboBox::QComboBox(pQVar6,*(QWidget **)(param_1 + 0x40));
        *(undefined ***)pQVar6 = QComboBox::vftable;
        *(undefined ***)(pQVar6 + 0x10) = QComboBox::vftable;
        this = pQVar6;
      }
      QComboBox::setEditable(this,true);
      QWidget::setAutoFillBackground((QWidget *)this,true);
      pQVar7 = QComboBox::view(this);
      QAbstractItemView::setTextElideMode(pQVar7,3);
      QComboBox::setAutoCompletion(this,false);
      pQVar8 = QComboBox::lineEdit(this);
      QObject::connect((QObject *)this,"2currentIndexChanged(const QString& )",param_1,
                       "1OnEditorChanged()",0);
      QObject::connect((QObject *)this,"2editTextChanged (const QString& )",param_1,
                       "1OnEditorChanged()",0);
      QObject::connect((QObject *)this,"2activated (const QString& )",param_1,
                       "1OnSelectionChanged()",0);
      QObject::connect((QObject *)this,"2editTextChanged (const QString& )",param_1,
                       "1OnTextChanged ()",0);
      QObject::connect((QObject *)pQVar8,"2returnPressed (void)",param_1,"1OnSelectionChanged()",0);
      if (**(int **)(param_1 + 0x50) != 1) {
        FUN_140038d70(param_1 + 0x50);
      }
      *(undefined4 *)(*(longlong *)(param_1 + 0x50) + 0x10 + lVar15 * 0x10) = 1;
      if (**(int **)(param_1 + 0x50) != 1) {
        FUN_140038d70(param_1 + 0x50);
      }
      *(QComboBox **)(*(longlong *)(param_1 + 0x50) + 0x18 + lVar15 * 0x10) = this;
      if (param_1[0x28] == (QObject)0x1) {
        if (**(int **)(param_1 + 0x50) != 1) {
          FUN_140038d70(param_1 + 0x50);
        }
        QWidget::show(*(QWidget **)(*(longlong *)(param_1 + 0x50) + 0x18 + lVar15 * 0x10));
      }
      else {
        if (**(int **)(param_1 + 0x50) != 1) {
          FUN_140038d70(param_1 + 0x50);
        }
        QWidget::hide(*(QWidget **)(*(longlong *)(param_1 + 0x50) + 0x18 + lVar15 * 0x10));
      }
    }
    if (*(int *)(*(longlong *)param_3 + 0xc) != *(int *)(*(longlong *)param_3 + 8)) {
      local_68 = QString::fromAscii_helper("",-1);
      pQVar9 = (QVariant *)QVariant::QVariant(local_50);
      iVar3 = QComboBox::count(this);
      pQVar10 = (QIcon *)QIcon::QIcon(local_60);
      QComboBox::insertItem(this,iVar3,pQVar10,(QString *)&local_68,pQVar9);
      QIcon::~QIcon(local_60);
      QVariant::~QVariant(local_50);
      QString::~QString((QString *)&local_68);
      if ((*(int *)(*param_5 + 0xc) == *(int *)(*param_5 + 8)) &&
         (*(int *)(*param_6 + 0xc) == *(int *)(*param_6 + 8))) {
        iVar3 = QComboBox::count(this);
        QComboBox::insertItems(this,iVar3,param_3);
      }
      else {
        pQVar6 = pQVar14;
        if (0 < *(int *)(*(longlong *)param_3 + 0xc) - *(int *)(*(longlong *)param_3 + 8)) {
          do {
            pQVar9 = *(QVariant **)
                      (*param_6 + 0x18 + (longlong)(pQVar6 + *(int *)(*param_6 + 8)) * 8);
            lVar15 = *(longlong *)param_3;
            iVar13 = (int)pQVar14;
            iVar3 = *(int *)(lVar15 + 8);
            lVar2 = *param_5;
            iVar5 = *(int *)(lVar2 + 8);
            iVar4 = QComboBox::count(this);
            QComboBox::insertItem
                      (this,iVar4,(QIcon *)(lVar2 + ((longlong)iVar5 + (longlong)iVar13) * 8 + 0x18)
                       ,(QString *)(lVar15 + ((longlong)iVar3 + (longlong)iVar13) * 8 + 0x18),pQVar9
                      );
            pQVar14 = (QComboBox *)(ulonglong)(iVar13 + 1U);
            param_1 = local_res8;
            pQVar6 = pQVar6 + 1;
          } while ((int)(iVar13 + 1U) <
                   *(int *)(*(longlong *)param_3 + 0xc) - *(int *)(*(longlong *)param_3 + 8));
        }
      }
    }
    pQVar7 = QComboBox::view(this);
    iVar5 = (**(code **)(*(longlong *)pQVar7 + 0x1d8))(pQVar7,0);
    pQVar7 = QComboBox::view(this);
    pQVar11 = QAbstractScrollArea::verticalScrollBar((QAbstractScrollArea *)pQVar7);
    piVar12 = (int *)(**(code **)(*(longlong *)pQVar11 + 0x60))(pQVar11,&local_res8);
    iVar3 = *piVar12;
    pQVar7 = QComboBox::view(this);
    QWidget::setMinimumWidth((QWidget *)pQVar7,iVar5 + iVar3 + 4);
    FUN_140037c60(param_1);
    if (*(int *)(*param_4 + 8) != 0) {
      local_res10[0] = 0x10;
      iVar3 = QComboBox::findText(this,param_4,local_res10);
      if (iVar3 != -1) {
        QComboBox::setCurrentIndex(this,iVar3);
      }
    }
  }
  return;
}


/* ================= FUN_1400388c0 @ 1400388c0  size=491  callers=1 callees=11 ================= */
/* CALLERS: FUN_14002fe40 */
/* CALLEES: FUN_140037c60, FUN_140038d70, FUN_140038fa0, FUN_140058d94, QLineEdit, connect, fromAscii_helper, hide, setPlaceholderText, show, ~QString */

void FUN_1400388c0(QObject *param_1,int param_2)

{
  longlong lVar1;
  QLineEdit *this;
  longlong lVar2;
  QLineEdit *this_00;
  Data *local_res8;
  int local_res10;
  QLineEdit *local_res18;
  
  if (param_2 != *(int *)(param_1 + 0x34)) {
    local_res10 = param_2;
    FUN_140038fa0(param_1 + 0x50,param_2);
    lVar1 = (longlong)*(int *)(param_1 + 0x34);
    if (lVar1 < param_2) {
      lVar2 = lVar1 << 4;
      lVar1 = param_2 - lVar1;
      do {
        this = (QLineEdit *)FUN_140058d94(0x28);
        this_00 = (QLineEdit *)0x0;
        local_res18 = this;
        if (this != (QLineEdit *)0x0) {
          QLineEdit::QLineEdit(this,*(QWidget **)(param_1 + 0x40));
          *(undefined ***)this = QLineEdit::vftable;
          *(undefined ***)(this + 0x10) = QLineEdit::vftable;
          this_00 = this;
        }
        local_res8 = QString::fromAscii_helper("Filter",-1);
        QLineEdit::setPlaceholderText(this_00,(QString *)&local_res8);
        QString::~QString((QString *)&local_res8);
        QObject::connect((QObject *)this_00,"2returnPressed()",param_1,"1OnEditorChanged()",0);
        QObject::connect((QObject *)this_00,"2editingFinished()",param_1,"1OnEditorChanged()",0);
        if (param_1[0x2c] != (QObject)0x0) {
          QObject::connect((QObject *)this_00,"2textChanged(const QString&)",param_1,
                           "1OnEditorChanged()",0);
        }
        if (**(int **)(param_1 + 0x50) != 1) {
          FUN_140038d70(param_1 + 0x50);
        }
        *(undefined4 *)(*(longlong *)(param_1 + 0x50) + 0x10 + lVar2) = 0;
        if (**(int **)(param_1 + 0x50) != 1) {
          FUN_140038d70(param_1 + 0x50);
        }
        *(QLineEdit **)(*(longlong *)(param_1 + 0x50) + 0x18 + lVar2) = this_00;
        if (param_1[0x28] == (QObject)0x1) {
          if (**(int **)(param_1 + 0x50) != 1) {
            FUN_140038d70(param_1 + 0x50);
          }
          QWidget::show(*(QWidget **)(*(longlong *)(param_1 + 0x50) + 0x18 + lVar2));
        }
        else {
          if (**(int **)(param_1 + 0x50) != 1) {
            FUN_140038d70(param_1 + 0x50);
          }
          QWidget::hide(*(QWidget **)(*(longlong *)(param_1 + 0x50) + 0x18 + lVar2));
        }
        lVar2 = lVar2 + 0x10;
        lVar1 = lVar1 + -1;
        param_2 = local_res10;
      } while (lVar1 != 0);
    }
    *(int *)(param_1 + 0x34) = param_2;
    *(int *)(param_1 + 0x3c) = param_2 + -1;
    *(int *)(param_1 + 0x38) = param_2;
    FUN_140037c60(param_1);
  }
  return;
}


/* ================= FUN_140038ab0 @ 140038ab0  size=353  callers=1 callees=10 ================= */
/* CALLERS: FUN_140037a90 */
/* CALLEES: FUN_140058d94, QAction, connect, fromAscii_helper, horizontalScrollBar, setCheckable, setChecked, setContextMenuPolicy, setText, ~QString */

void FUN_140038ab0(QObject *param_1)

{
  QAction *this;
  QScrollBar *pQVar1;
  QAction *pQVar2;
  QAction *local_res8;
  
  *(undefined8 *)(param_1 + 0x30) = 4;
  pQVar2 = (QAction *)0x0;
  this = (QAction *)FUN_140058d94(0x10);
  local_res8 = this;
  if (this != (QAction *)0x0) {
    QAction::QAction(this,param_1);
    *(undefined ***)this = QAction::vftable;
    pQVar2 = this;
  }
  *(QAction **)(param_1 + 0x48) = pQVar2;
  local_res8 = (QAction *)QString::fromAscii_helper("Filter",-1);
  QAction::setText(*(QAction **)(param_1 + 0x48),(QString *)&local_res8);
  QString::~QString((QString *)&local_res8);
  QAction::setCheckable(*(QAction **)(param_1 + 0x48),true);
  QAction::setChecked(*(QAction **)(param_1 + 0x48),(bool)param_1[0x28]);
  QObject::connect(*(QObject **)(param_1 + 0x48),"2toggled(bool)",param_1,"1OnToggleFilter(bool)",0)
  ;
  QObject::connect(param_1,"2sectionResized(int, int, int)",param_1,"1AdjustPositions()",0);
  QObject::connect(param_1,"2sectionMoved(int, int, int)",param_1,"1OnSectionMoved(int, int, int)",0
                  );
  pQVar1 = QAbstractScrollArea::horizontalScrollBar(*(QAbstractScrollArea **)(param_1 + 0x40));
  QObject::connect((QObject *)pQVar1,"2valueChanged(int)",param_1,"1AdjustPositions()",0);
  if (param_1[0x2b] != (QObject)0x0) {
    QWidget::setContextMenuPolicy((QWidget *)param_1,3);
  }
  QObject::connect(param_1,"2customContextMenuRequested(const QPoint&)",param_1,
                   "1OnContextMenu(const QPoint&)",0);
  return;
}


/* ================= FUN_140038c20 @ 140038c20  size=185  callers=1 callees=5 ================= */
/* CALLERS: FUN_1400383c0 */
/* CALLEES: FUN_140037c60, isSectionHidden, logicalIndex, setSectionHidden, visualIndex */

void FUN_140038c20(QHeaderView *param_1,int param_2,byte param_3)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  
  if (param_3 == 0) {
    if (*(int *)(param_1 + 0x38) == 1) {
      return;
    }
    iVar2 = -1;
  }
  else {
    iVar2 = 1;
  }
  *(int *)(param_1 + 0x38) = iVar2 + *(int *)(param_1 + 0x38);
  iVar2 = QHeaderView::visualIndex(param_1,param_2);
  QHeaderView::setSectionHidden(param_1,param_2,(bool)(param_3 ^ 1));
  if (param_3 == 0) {
    if (iVar2 == *(int *)(param_1 + 0x3c)) {
      *(undefined4 *)(param_1 + 0x3c) = 0xffffffff;
      for (; -1 < iVar2; iVar2 = iVar2 + -1) {
        iVar3 = QHeaderView::logicalIndex(param_1,iVar2);
        bVar1 = QHeaderView::isSectionHidden(param_1,iVar3);
        if (!bVar1) goto LAB_140038cb9;
      }
    }
  }
  else if (*(int *)(param_1 + 0x3c) < iVar2) {
LAB_140038cb9:
    *(int *)(param_1 + 0x3c) = iVar2;
  }
  FUN_140037c60(param_1);
  return;
}


/* ================= FUN_140038ce0 @ 140038ce0  size=130  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c8d0 */
/* CALLEES: FUN_140038d70, hide, show */

void FUN_140038ce0(QHeaderView *param_1,QHeaderView param_2)

{
  QWidget *this;
  int iVar1;
  longlong lVar2;
  
  iVar1 = 0;
  param_1[0x28] = param_2;
  if (0 < *(int *)(param_1 + 0x34)) {
    lVar2 = 0;
    do {
      if (**(int **)(param_1 + 0x50) != 1) {
        FUN_140038d70(param_1 + 0x50);
      }
      this = *(QWidget **)(*(longlong *)(param_1 + 0x50) + 0x18 + lVar2);
      if (this != (QWidget *)0x0) {
        if (param_1[0x28] == (QHeaderView)0x1) {
          QWidget::show(this);
        }
        else {
          QWidget::hide(this);
        }
      }
      iVar1 = iVar1 + 1;
      lVar2 = lVar2 + 0x10;
    } while (iVar1 < *(int *)(param_1 + 0x34));
  }
                    /* WARNING: Could not recover jumptable at 0x000140038d5b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QHeaderView::geometriesChanged(param_1);
  return;
}


/* ================= FUN_140038d70 @ 140038d70  size=15  callers=8 callees=1 ================= */
/* CALLERS: FUN_140037c60, FUN_140037ed0, FUN_140037fc0, FUN_140038030, FUN_140038440, FUN_1400388c0, FUN_140038ce0, FUN_140039070 */
/* CALLEES: FUN_140038df0 */

void FUN_140038d70(longlong *param_1)

{
  FUN_140038df0(param_1,*(undefined4 *)(*param_1 + 8),*(undefined4 *)(*param_1 + 4));
  return;
}


/* ================= FUN_140038d80 @ 140038d80  size=107  callers=4 callees=0 ================= */
/* CALLERS: FUN_140037b40, FUN_140037b70, FUN_140038df0, FUN_14005ad20 */

void FUN_140038d80(undefined8 param_1,QVectorData *param_2)

{
  longlong *plVar1;
  QVectorData *pQVar2;
  
  pQVar2 = param_2 + 0x10 + (longlong)*(int *)(param_2 + 8) * 0x10;
  while (pQVar2 != param_2 + 0x10) {
    plVar1 = *(longlong **)(pQVar2 + -8);
    pQVar2 = pQVar2 + -0x10;
    if (plVar1 != (longlong *)0x0) {
      (**(code **)(*plVar1 + 0x18))(plVar1,1);
    }
  }
                    /* WARNING: Could not recover jumptable at 0x000140038de4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QVectorData::free(param_2,8);
  return;
}


/* ================= FUN_140038df0 @ 140038df0  size=422  callers=2 callees=3 ================= */
/* CALLERS: FUN_140038d70, FUN_140038fa0 */
/* CALLEES: FUN_140038d80, allocate, qBadAlloc */

void FUN_140038df0(longlong *param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  longlong *plVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  QVectorData *pQVar7;
  QVectorData *pQVar8;
  int *piVar9;
  int *piVar10;
  QVectorData *pQVar11;
  undefined4 *puVar12;
  longlong *plVar13;
  int local_res10 [2];
  
  local_res10[0] = param_2;
  pQVar8 = (QVectorData *)*param_1;
  pQVar11 = pQVar8 + 8;
  if ((param_2 < *(int *)pQVar11) && (*(int *)pQVar8 == 1)) {
    iVar1 = *(int *)(*param_1 + 8);
    if (param_2 < iVar1) {
      plVar13 = (longlong *)((longlong)iVar1 * 0x10 + 0x18 + *param_1);
      do {
        plVar13 = plVar13 + -2;
        plVar3 = (longlong *)*plVar13;
        if (plVar3 != (longlong *)0x0) {
          (**(code **)(*plVar3 + 0x18))(plVar3,1);
        }
        *(int *)(*param_1 + 8) = *(int *)(*param_1 + 8) + -1;
      } while (param_2 < *(int *)(*param_1 + 8));
    }
  }
  if ((param_3 != ((int *)*param_1)[1]) || (param_2 = local_res10[0], *(int *)*param_1 != 1)) {
    pQVar8 = QVectorData::allocate((param_3 + 1) * 0x10,8);
    if (pQVar8 == (QVectorData *)0x0) {
      qBadAlloc();
      qBadAlloc();
    }
    pQVar11 = pQVar8 + 8;
    *(int *)pQVar11 = 0;
    *(undefined4 *)pQVar8 = 1;
    *(int *)(pQVar8 + 4) = param_3;
    *(uint *)(pQVar8 + 0xc) = *(uint *)(pQVar8 + 0xc) | 1;
    *(uint *)(pQVar8 + 0xc) = *(uint *)(*param_1 + 0xc) & 2 | *(uint *)(pQVar8 + 0xc) & 1;
  }
  iVar1 = *(int *)pQVar11;
  puVar12 = (undefined4 *)(*param_1 + 0x10 + (longlong)iVar1 * 0x10);
  piVar9 = (int *)(*param_1 + 8);
  piVar10 = local_res10;
  if (*piVar9 <= param_2) {
    piVar10 = piVar9;
  }
  iVar2 = *piVar10;
  pQVar7 = pQVar8 + (longlong)iVar1 * 0x10 + 0x10;
  while (iVar1 < iVar2) {
    uVar4 = puVar12[1];
    uVar5 = puVar12[2];
    uVar6 = puVar12[3];
    *(undefined4 *)pQVar7 = *puVar12;
    *(undefined4 *)(pQVar7 + 4) = uVar4;
    *(undefined4 *)(pQVar7 + 8) = uVar5;
    *(undefined4 *)(pQVar7 + 0xc) = uVar6;
    puVar12 = puVar12 + 4;
    *(int *)pQVar11 = *(int *)pQVar11 + 1;
    pQVar7 = pQVar7 + 0x10;
    iVar1 = *(int *)pQVar11;
  }
  while (iVar1 < param_2) {
    *(undefined4 *)pQVar7 = 0xffffffff;
    *(undefined8 *)(pQVar7 + 8) = 0;
    *(int *)pQVar11 = *(int *)pQVar11 + 1;
    pQVar7 = pQVar7 + 0x10;
    iVar1 = *(int *)pQVar11;
  }
  *(int *)pQVar11 = param_2;
  pQVar11 = (QVectorData *)*param_1;
  if (pQVar11 != pQVar8) {
    LOCK();
    iVar1 = *(int *)pQVar11;
    *(int *)pQVar11 = *(int *)pQVar11 + -1;
    UNLOCK();
    if (iVar1 == 1) {
      FUN_140038d80(param_1,*param_1);
    }
    *param_1 = (longlong)pQVar8;
  }
  return;
}


/* ================= FUN_140038fa0 @ 140038fa0  size=88  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400388c0 */
/* CALLEES: FUN_140038df0, grow */

void FUN_140038fa0(longlong *param_1,int param_2)

{
  longlong lVar1;
  int iVar2;
  
  lVar1 = *param_1;
  iVar2 = *(int *)(lVar1 + 4);
  if ((iVar2 < param_2) ||
     ((((*(byte *)(lVar1 + 0xc) & 2) == 0 && (param_2 < *(int *)(lVar1 + 8))) &&
      (param_2 < iVar2 >> 1)))) {
    iVar2 = QVectorData::grow(0x20,param_2,0x10,true);
  }
  FUN_140038df0(param_1,param_2,iVar2);
  return;
}


/* ================= FUN_140039000 @ 140039000  size=99  callers=0 callees=1 ================= */
/* CALLEES: sizeHint */

undefined8 * FUN_140039000(QHeaderView *param_1,undefined8 *param_2)

{
  QHeaderView QVar1;
  longlong *plVar2;
  undefined8 *puVar3;
  longlong lVar4;
  undefined1 local_res8 [8];
  
  *param_2 = 0xffffffffffffffff;
  puVar3 = (undefined8 *)QHeaderView::sizeHint(param_1);
  QVar1 = param_1[0x28];
  *param_2 = *puVar3;
  if (((QVar1 != (QHeaderView)0x0) && (0 < *(int *)(param_1 + 0x34))) &&
     (plVar2 = *(longlong **)(*(longlong *)(param_1 + 0x50) + 0x18), plVar2 != (longlong *)0x0)) {
    lVar4 = (**(code **)(*plVar2 + 0x60))(plVar2,local_res8);
    *(int *)((longlong)param_2 + 4) =
         *(int *)((longlong)param_2 + 4) + *(int *)(lVar4 + 4) + *(int *)(param_1 + 0x30);
  }
  return param_2;
}


/* ================= FUN_140039070 @ 140039070  size=134  callers=0 callees=4 ================= */
/* CALLEES: FUN_140037c60, FUN_140038d70, setViewportMargins, updateGeometries */

void FUN_140039070(QAbstractScrollArea *param_1)

{
  longlong lVar1;
  int iVar2;
  
  iVar2 = 0;
  if ((param_1[0x28] != (QAbstractScrollArea)0x0) && (iVar2 = 0, 0 < *(int *)(param_1 + 0x34))) {
    if (**(int **)(param_1 + 0x50) != 1) {
      FUN_140038d70(param_1 + 0x50);
    }
    if (*(longlong **)(*(longlong *)(param_1 + 0x50) + 0x18) != (longlong *)0x0) {
      lVar1 = (**(code **)(**(longlong **)(*(longlong *)(param_1 + 0x50) + 0x18) + 0x60))();
      iVar2 = *(int *)(lVar1 + 4) + *(int *)(param_1 + 0x30);
    }
  }
  QAbstractScrollArea::setViewportMargins(param_1,0,0,0,iVar2);
  QHeaderView::updateGeometries((QHeaderView *)param_1);
  FUN_140037c60(param_1);
  return;
}


/* ================= FUN_140039100 @ 140039100  size=49  callers=16 callees=1 ================= */
/* CALLERS: FUN_14003e0b0, FUN_14003e110, FUN_14003e1b0, FUN_14003e210, FUN_14003e2e0, FUN_14003e380, FUN_14003e420, FUN_14003e4e0, FUN_14003e5c0, FUN_14003e620, FUN_14003e680, FUN_14003e6e0, FUN_14003e740, FUN_14003e7a0, FUN_14003e800, FUN_14004db40 */
/* CALLEES: FUN_140039140 */

void FUN_140039100(undefined8 param_1)

{
  undefined8 local_28;
  undefined8 local_20;
  undefined8 uStack_18;
  undefined4 local_10;
  undefined4 local_c;
  
  local_c = 1;
  local_20 = 0;
  uStack_18 = 0;
  local_10 = 0xffffffff;
  local_28 = param_1;
  FUN_140039140(&local_28);
  return;
}


/* ================= FUN_140039140 @ 140039140  size=398  callers=1 callees=7 ================= */
/* CALLERS: FUN_140039100 */
/* CALLEES: FUN_140039370, FUN_140039400, FUN_1400395f0, FUN_140043970, FUN_140043990, FUN_1400443b0, Sleep */

undefined8 FUN_140039140(undefined8 *param_1)

{
  undefined1 auVar1 [16];
  int iVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  ulonglong uVar5;
  longlong lVar6;
  ulonglong uVar7;
  uint uVar8;
  uint uVar9;
  
  uVar8 = 1;
  if (1 < *(uint *)((longlong)param_1 + 0x1c)) {
    uVar8 = *(uint *)((longlong)param_1 + 0x1c);
  }
  if (DAT_14085cf40 == '\0') {
    FUN_140039400();
    FUN_140039370();
    uVar7 = 0;
    if (*(int *)(param_1 + 3) != -1) {
      uVar7 = (ulonglong)(uint)(*(int *)(param_1 + 3) * 1000);
    }
    uVar3 = FUN_140043990();
    iVar2 = *(int *)(&DAT_14085cf50 + (ulonglong)DAT_14085d158 * 0x10);
    while( true ) {
      if (iVar2 == 0) {
        uVar5 = (ulonglong)DAT_14085d158;
        uVar7 = (ulonglong)(DAT_14085d158 + 1);
        if (0x1f < DAT_14085d158 + 1) {
          uVar7 = 0;
        }
        *(undefined8 *)(&DAT_14085cf58 + uVar5 * 0x10) = *param_1;
        DAT_14085d158 = (uint)uVar7;
        *(undefined4 *)(&DAT_14085cf50 + uVar5 * 0x10) = 1;
        FUN_1400395f0();
        FUN_1400443b0(DAT_14085d168);
        return 0;
      }
      uVar4 = FUN_140043990();
      if (((code *)param_1[1] != (code *)0x0) &&
         (iVar2 = (*(code *)param_1[1])(param_1[2]), iVar2 < 0)) break;
      if ((*(int *)(param_1 + 3) != -1) && (uVar5 = FUN_140043970(uVar3), uVar7 < uVar5)) {
        FUN_1400395f0();
        return 0xfffffffe;
      }
      FUN_1400395f0();
      uVar5 = FUN_140043970(uVar4);
      auVar1._8_8_ = 0;
      auVar1._0_8_ = uVar5;
      lVar6 = SUB168(ZEXT816(0x624dd2f1a9fbe77) * auVar1,8);
      uVar9 = (uint)((uVar5 - lVar6 >> 1) + lVar6 >> 9);
      if (uVar9 < uVar8) {
        Sleep(uVar8 - uVar9);
      }
      FUN_140039400();
      iVar2 = *(int *)(&DAT_14085cf50 + (ulonglong)DAT_14085d158 * 0x10);
    }
    FUN_1400395f0();
    return 0xfffffffd;
  }
  return 0xffffffff;
}


/* ================= FUN_1400392d0 @ 1400392d0  size=29  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003d5c0 */

undefined8 FUN_1400392d0(longlong *param_1)

{
  undefined8 *puVar1;
  longlong *plVar2;
  
  *param_1 = (longlong)DAT_14085d150;
  plVar2 = param_1;
  if (DAT_14085d150 != (longlong *)0x0) {
    puVar1 = (undefined8 *)((longlong)DAT_14085d150 + 8);
    DAT_14085d150 = param_1;
    *puVar1 = param_1;
    plVar2 = DAT_14085d150;
  }
  DAT_14085d150 = plVar2;
  return 0;
}


/* ================= FUN_1400392f0 @ 1400392f0  size=16  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003d7d0 */
/* CALLEES: FUN_1400393d0 */

undefined8 FUN_1400392f0(void)

{
  FUN_1400393d0();
  return 0;
}


/* ================= FUN_140039300 @ 140039300  size=106  callers=1 callees=6 ================= */
/* CALLERS: FUN_14003e2e0 */
/* CALLEES: FUN_140039370, FUN_140039400, FUN_1400395f0, FUN_140043d10, FUN_140043e60, memcpy */

void FUN_140039300(void *param_1,int param_2)

{
  FUN_140039400();
  FUN_140039370();
  if (DAT_14085d170 != (void *)0x0) {
    FUN_140043e60();
  }
  DAT_14085d170 = (void *)FUN_140043d10(param_2 << 4);
  memcpy(DAT_14085d170,param_1,(ulonglong)(uint)(param_2 << 4));
  DAT_14085d15c = param_2;
  FUN_1400395f0();
  return;
}


/* ================= FUN_140039370 @ 140039370  size=86  callers=2 callees=2 ================= */
/* CALLERS: FUN_140039140, FUN_140039300 */
/* CALLEES: FUN_1400430f0, FUN_140043190 */

void FUN_140039370(void)

{
  undefined1 local_res8 [32];
  
  if (DAT_14085d160 == 0) {
    DAT_14085d160 = FUN_140043190(FUN_140039430,0,local_res8,"WORKER",0);
  }
  if (DAT_14085d168 == 0) {
    DAT_14085d168 = FUN_1400430f0(0,0);
  }
  return;
}


/* ================= FUN_1400393d0 @ 1400393d0  size=41  callers=2 callees=0 ================= */
/* CALLERS: FUN_1400392f0, FUN_140039430 */

void FUN_1400393d0(longlong *param_1)

{
  longlong lVar1;
  longlong lVar2;
  
  lVar2 = *param_1;
  lVar1 = lVar2;
  if ((longlong *)param_1[1] != (longlong *)0x0) {
    *(longlong *)param_1[1] = lVar2;
    lVar2 = *param_1;
    lVar1 = DAT_14085d150;
  }
  DAT_14085d150 = lVar1;
  if (lVar2 != 0) {
    *(longlong *)(lVar2 + 8) = param_1[1];
  }
  return;
}


/* ================= FUN_140039400 @ 140039400  size=47  callers=2 callees=2 ================= */
/* CALLERS: FUN_140039140, FUN_140039300 */
/* CALLEES: FUN_140043110, FUN_1400443e0 */

void FUN_140039400(void)

{
  if (DAT_14085cf48 == 0) {
    DAT_14085cf48 = FUN_140043110(0);
  }
  FUN_1400443e0(DAT_14085cf48,0xffffffff);
  return;
}


/* ================= FUN_140039430 @ 140039430  size=445  callers=0 callees=8 ================= */
/* CALLEES: FUN_1400393d0, FUN_140042ff0, FUN_140043000, FUN_140043cc0, FUN_140043e60, FUN_1400443b0, FUN_1400443d0, memset */

undefined8 FUN_140039430(void)

{
  uint uVar1;
  int iVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  ulonglong uVar5;
  uint uVar6;
  ulonglong uVar7;
  ulonglong uVar8;
  
  uVar8 = 0;
  uVar5 = uVar8;
  do {
    while( true ) {
      while (*(int *)(&DAT_14085cf50 + uVar5 * 0x10) != 0) {
        puVar3 = *(undefined8 **)(&DAT_14085cf58 + uVar5 * 0x10);
        uVar1 = *(uint *)(puVar3 + 3);
        if (uVar1 < DAT_14085d15c) {
          if (uVar1 == DAT_14085d15c - 1) {
            DAT_14085cf40 = 1;
            FUN_140042ff0(DAT_14085d160);
            FUN_140042ff0(DAT_14085d168);
            DAT_14085d160 = 0;
            DAT_14085d168 = 0;
            DAT_14085d158 = 0;
            if (DAT_14085cf48 != 0) {
              FUN_140043000(&DAT_14085cf48);
            }
            memset(&DAT_14085cf50,0,0x200);
            if (puVar3[1] != 0) {
              FUN_1400443b0();
            }
            return 0;
          }
          (**(code **)(DAT_14085d170 + (ulonglong)uVar1 * 0x10))(puVar3);
        }
        if ((code *)*puVar3 != (code *)0x0) {
          (*(code *)*puVar3)(puVar3);
        }
        uVar1 = (int)uVar5 + 1;
        *(undefined4 *)(&DAT_14085cf50 + uVar5 * 0x10) = 0;
        uVar5 = uVar8;
        if (uVar1 < 0x20) {
          uVar5 = (ulonglong)uVar1;
        }
      }
      puVar3 = DAT_14085d150;
      uVar7 = uVar8;
      if (DAT_14085d150 != (undefined8 *)0x0) break;
LAB_140039553:
      FUN_1400443d0(DAT_14085d168);
    }
    do {
      uVar1 = *(uint *)((longlong)puVar3 + 0x24);
      iVar2 = FUN_140043cc0();
      if ((int)(uVar1 - iVar2) < 1) {
        iVar2 = (*(code *)puVar3[2])(puVar3);
        if (iVar2 == 0) {
          iVar2 = FUN_140043cc0();
          uVar1 = *(int *)(puVar3 + 4) + iVar2;
          *(uint *)((longlong)puVar3 + 0x24) = uVar1;
          goto LAB_14003951d;
        }
        puVar4 = (undefined8 *)*puVar3;
        FUN_1400393d0(puVar3);
        if (puVar3[3] != 0) {
          FUN_140043e60();
        }
        FUN_140043e60(puVar3);
      }
      else {
LAB_14003951d:
        puVar4 = (undefined8 *)*puVar3;
        uVar6 = (uint)uVar7;
        if ((uVar6 != 0) && (uVar6 < uVar1)) {
          uVar1 = uVar6;
        }
        uVar7 = (ulonglong)uVar1;
      }
      puVar3 = puVar4;
    } while (puVar4 != (undefined8 *)0x0);
    if ((int)uVar7 == 0) goto LAB_140039553;
    FUN_140043cc0();
    FUN_1400443d0(DAT_14085d168);
  } while( true );
}


/* ================= FUN_1400395f0 @ 1400395f0  size=12  callers=2 callees=1 ================= */
/* CALLERS: FUN_140039140, FUN_140039300 */
/* CALLEES: FUN_140044370 */

void FUN_1400395f0(void)

{
  FUN_140044370(DAT_14085cf48);
  return;
}


/* ================= FUN_140039600 @ 140039600  size=345  callers=12 callees=5 ================= */
/* CALLERS: FUN_140039760, FUN_1400397c0, FUN_140039850, FUN_1400398d0, FUN_140039960, FUN_140046240, FUN_1400462b0, FUN_1400463b0, FUN_1400466e0, FUN_1400467d0, FUN_1400468c0, FUN_140046c50 */
/* CALLEES: FUN_140027140, FUN_1400277b0, FUN_140035480, FUN_140039af0, FUN_14003a930 */

undefined8 FUN_140039600(longlong param_1,longlong param_2,char *param_3,undefined *param_4)

{
  int iVar1;
  int iVar2;
  longlong lVar3;
  undefined8 *puVar4;
  undefined8 uVar5;
  undefined *puVar6;
  
  if (((param_2 != 0) && (param_3 != (char *)0x0)) && (*param_3 != '\0')) {
    puVar6 = &DAT_14005f306;
    if (param_4 != (undefined *)0x0) {
      puVar6 = param_4;
    }
    lVar3 = FUN_140039af0();
    if (lVar3 == 0) {
      iVar1 = FUN_1400277b0(param_3);
      iVar2 = FUN_1400277b0(puVar6);
      puVar4 = (undefined8 *)FUN_140035480(*(undefined8 *)(param_1 + 0x10),0x28);
      *puVar4 = 0;
      puVar4[1] = 0;
      puVar4[2] = 0;
      puVar4[3] = 0;
      puVar4[4] = 0;
      uVar5 = FUN_140035480(*(undefined8 *)(param_1 + 0x10),iVar1 + 1);
      *puVar4 = uVar5;
      uVar5 = FUN_140035480(*(undefined8 *)(param_1 + 0x10),iVar2 + 1);
      puVar4[1] = uVar5;
      FUN_140027140(*puVar4,param_3,iVar1 + 1);
      FUN_140027140(puVar4[1],puVar6,iVar2 + 1);
      *(int *)(puVar4 + 2) = iVar1;
      *(int *)((longlong)puVar4 + 0x14) = iVar2;
      if (*(longlong *)(param_2 + 0x20) == 0) {
        *(undefined8 **)(param_2 + 0x20) = puVar4;
      }
      else {
        lVar3 = FUN_14003a930(param_2);
        *(undefined8 **)(lVar3 + 0x20) = puVar4;
        puVar4[3] = lVar3;
      }
    }
    else {
      iVar1 = FUN_1400277b0(puVar6);
      uVar5 = FUN_140035480(*(undefined8 *)(param_1 + 0x10),iVar1 + 1);
      *(undefined8 *)(lVar3 + 8) = uVar5;
      FUN_140027140(uVar5,puVar6,iVar1 + 1);
      *(int *)(lVar3 + 0x14) = iVar1;
    }
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_140039760 @ 140039760  size=50  callers=4 callees=1 ================= */
/* CALLERS: FUN_1400463b0, FUN_1400468c0, FUN_140046c50, FUN_140046da0 */
/* CALLEES: FUN_140039600 */

undefined8 FUN_140039760(longlong param_1,longlong param_2,longlong param_3)

{
  undefined8 uVar1;
  
  if (((param_1 != 0) && (param_2 != 0)) && (param_3 != 0)) {
    uVar1 = FUN_140039600();
    return uVar1;
  }
  return 0xffffffff;
}


/* ================= FUN_1400397a0 @ 1400397a0  size=11  callers=13 callees=1 ================= */
/* CALLERS: FUN_140046110, FUN_140046240, FUN_1400462b0, FUN_140046320, FUN_1400463b0, FUN_140046650, FUN_1400466e0, FUN_140046750, FUN_1400467d0, FUN_140046840, FUN_1400468c0, FUN_140046c50, FUN_140046da0 */
/* CALLEES: FUN_14003a0e0 */

void FUN_1400397a0(void)

{
  FUN_14003a0e0();
  return;
}


/* ================= FUN_1400397b0 @ 1400397b0  size=8  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400468c0 */
/* CALLEES: FUN_14003a0e0 */

void FUN_1400397b0(void)

{
  FUN_14003a0e0();
  return;
}


/* ================= FUN_1400397c0 @ 1400397c0  size=134  callers=2 callees=3 ================= */
/* CALLERS: FUN_1400468c0, FUN_140046c50 */
/* CALLEES: FUN_140039600, __security_check_cookie, snprintf */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_1400397c0(longlong param_1,longlong param_2,longlong param_3)

{
  undefined8 uVar1;
  undefined1 auStack_68 [32];
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 local_38;
  undefined8 uStack_30;
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_68;
  if (((param_1 == 0) || (param_2 == 0)) || (param_3 == 0)) {
    uVar1 = 0xffffffff;
  }
  else {
    local_48 = 0;
    uStack_40 = 0;
    local_38 = 0;
    uStack_30 = 0;
    snprintf(&local_48,0x20,"%.17g");
    uVar1 = FUN_140039600(param_1,param_2,param_3,&local_48);
  }
  return uVar1;
}


/* ================= FUN_140039850 @ 140039850  size=125  callers=2 callees=3 ================= */
/* CALLERS: FUN_1400463b0, FUN_1400468c0 */
/* CALLEES: FUN_140028380, FUN_140039600, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_140039850(longlong param_1,longlong param_2,longlong param_3)

{
  undefined8 uVar1;
  undefined1 auStack_58 [32];
  undefined8 local_38;
  undefined4 local_30;
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_58;
  if (((param_1 == 0) || (param_2 == 0)) || (param_3 == 0)) {
    uVar1 = 0xffffffff;
  }
  else {
    local_38 = 0;
    local_30 = 0;
    FUN_140028380(&local_38,0xc,&DAT_140060a50);
    uVar1 = FUN_140039600(param_1,param_2,param_3,&local_38);
  }
  return uVar1;
}


/* ================= FUN_1400398d0 @ 1400398d0  size=130  callers=4 callees=3 ================= */
/* CALLERS: FUN_140046320, FUN_1400463b0, FUN_140046750, FUN_1400468c0 */
/* CALLEES: FUN_140028380, FUN_140039600, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_1400398d0(longlong param_1,longlong param_2,longlong param_3)

{
  undefined8 uVar1;
  undefined1 auStack_58 [32];
  undefined8 local_38;
  undefined2 local_30;
  undefined1 local_2e;
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_58;
  if (((param_1 == 0) || (param_2 == 0)) || (param_3 == 0)) {
    uVar1 = 0xffffffff;
  }
  else {
    local_38 = 0;
    local_30 = 0;
    local_2e = 0;
    FUN_140028380(&local_38,0xb,&DAT_14005f57c);
    uVar1 = FUN_140039600(param_1,param_2,param_3,&local_38);
  }
  return uVar1;
}


/* ================= FUN_140039960 @ 140039960  size=130  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400468c0 */
/* CALLEES: FUN_140028380, FUN_140039600, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined8 FUN_140039960(longlong param_1,longlong param_2,longlong param_3)

{
  undefined8 uVar1;
  undefined1 auStack_58 [32];
  undefined8 local_38;
  undefined2 local_30;
  undefined1 local_2e;
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_58;
  if (((param_1 == 0) || (param_2 == 0)) || (param_3 == 0)) {
    uVar1 = 0xffffffff;
  }
  else {
    local_38 = 0;
    local_30 = 0;
    local_2e = 0;
    FUN_140028380(&local_38,0xb,&DAT_140071900);
    uVar1 = FUN_140039600(param_1,param_2,param_3,&local_38);
  }
  return uVar1;
}


/* ================= FUN_1400399f0 @ 1400399f0  size=25  callers=2 callees=1 ================= */
/* CALLERS: FUN_140047020, FUN_1400471d0 */
/* CALLEES: FUN_1400354d0 */

undefined8 FUN_1400399f0(longlong param_1)

{
  if (param_1 != 0) {
    FUN_1400354d0(*(undefined8 *)(param_1 + 0x10));
  }
  return 0;
}


/* ================= FUN_140039a10 @ 140039a10  size=220  callers=1 callees=5 ================= */
/* CALLERS: FUN_1400471d0 */
/* CALLEES: FUN_140027140, FUN_1400277b0, FUN_140035480, FUN_1400354d0, FUN_140035500 */

undefined8 * FUN_140039a10(longlong param_1)

{
  int iVar1;
  longlong lVar2;
  longlong *plVar3;
  longlong lVar4;
  undefined8 *puVar5;
  
  lVar2 = FUN_140035500();
  if (((param_1 != 0) && (iVar1 = FUN_1400277b0(param_1), 0 < iVar1)) &&
     (plVar3 = (longlong *)FUN_140035480(lVar2,0x48), plVar3 != (longlong *)0x0)) {
    *plVar3 = 0;
    plVar3[1] = 0;
    plVar3[2] = 0;
    plVar3[3] = 0;
    plVar3[4] = 0;
    plVar3[5] = 0;
    plVar3[6] = 0;
    plVar3[7] = 0;
    plVar3[8] = 0;
    lVar4 = FUN_140035480(lVar2,iVar1 + 1);
    *plVar3 = lVar4;
    if (lVar4 != 0) {
      FUN_140027140(lVar4,param_1,iVar1 + 1);
      *(int *)(plVar3 + 1) = iVar1;
      puVar5 = (undefined8 *)FUN_140035480(lVar2,0x18);
      if (puVar5 != (undefined8 *)0x0) {
        puVar5[1] = 0;
        *puVar5 = plVar3;
        puVar5[2] = lVar2;
        return puVar5;
      }
    }
  }
  if (lVar2 != 0) {
    FUN_1400354d0(lVar2);
  }
  return (undefined8 *)0x0;
}


/* ================= FUN_140039af0 @ 140039af0  size=97  callers=15 callees=1 ================= */
/* CALLERS: FUN_140039600, FUN_140044b40, FUN_140044ce0, FUN_140044d50, FUN_140044ed0, FUN_140045250, FUN_1400452b0, FUN_140045310, FUN_140045500, FUN_140045570, FUN_140045670, FUN_140045b80, FUN_140045c50, FUN_140045ee0, FUN_1400460b0 */
/* CALLEES: FUN_140027760 */

longlong * FUN_140039af0(undefined8 param_1,longlong param_2,longlong param_3)

{
  longlong *plVar1;
  int iVar2;
  
  if ((param_2 != 0) && (param_3 != 0)) {
    plVar1 = *(longlong **)(param_2 + 0x20);
    while( true ) {
      if (plVar1 == (longlong *)0x0) {
        return (longlong *)0x0;
      }
      if ((*plVar1 != 0) && (iVar2 = FUN_140027760(param_3), iVar2 == 0)) break;
      plVar1 = (longlong *)plVar1[4];
    }
    return plVar1;
  }
  return (longlong *)0x0;
}


/* ================= FUN_140039b60 @ 140039b60  size=252  callers=4 callees=8 ================= */
/* CALLERS: FUN_140039d40, FUN_140039db0, FUN_14003a0e0, FUN_140045cc0 */
/* CALLEES: FUN_140027760, FUN_1400277b0, FUN_140039e40, FUN_140039e50, FUN_140043d10, FUN_140043e60, __security_check_cookie, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

longlong FUN_140039b60(undefined8 param_1,longlong param_2,undefined8 param_3)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  undefined1 *puVar4;
  longlong lVar5;
  uint uVar6;
  undefined1 auStack_168 [32];
  undefined1 local_148 [256];
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStack_168;
  memset(local_148,0,0x100);
  iVar2 = FUN_1400277b0(param_3);
  uVar6 = iVar2 + 1;
  if (uVar6 < 0x101) {
    puVar4 = local_148;
    bVar1 = false;
  }
  else {
    puVar4 = (undefined1 *)FUN_140043d10(uVar6);
    bVar1 = true;
  }
  do {
    iVar3 = FUN_140039e50(param_1,param_2,puVar4,uVar6);
    if (((-1 < iVar3) && (iVar3 == iVar2)) &&
       (iVar3 = FUN_140027760(puVar4,param_3), lVar5 = param_2, iVar3 == 0)) break;
    param_2 = FUN_140039e40(param_1,param_2);
    lVar5 = 0;
  } while (param_2 != 0);
  if (bVar1) {
    FUN_140043e60(puVar4);
  }
  return lVar5;
}


/* ================= FUN_140039c60 @ 140039c60  size=57  callers=10 callees=1 ================= */
/* CALLERS: FUN_140044b40, FUN_140044ce0, FUN_140044d50, FUN_140044ed0, FUN_140045500, FUN_140045570, FUN_140045670, FUN_140045b80, FUN_140045c50, FUN_140045ee0 */
/* CALLEES: FUN_140027140 */

undefined4 FUN_140039c60(undefined8 param_1,longlong param_2,longlong param_3,undefined4 param_4)

{
  if ((param_3 != 0) && (param_2 != 0)) {
    FUN_140027140(param_3,*(undefined8 *)(param_2 + 8),param_4);
    return *(undefined4 *)(param_2 + 0x14);
  }
  return 0xffffffff;
}


/* ================= FUN_140039ca0 @ 140039ca0  size=79  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045250 */
/* CALLEES: FUN_1400272c0 */

ulonglong FUN_140039ca0(undefined8 param_1,longlong param_2,longlong param_3)

{
  ulonglong uVar1;
  int local_res18 [4];
  
  if ((param_3 == 0) || (param_2 == 0)) {
    uVar1 = 0xffffffff;
  }
  else {
    local_res18[0] = 0;
    uVar1 = FUN_1400272c0(*(undefined8 *)(param_2 + 8),local_res18);
    if (-1 < (int)uVar1) {
      *(bool *)param_3 = local_res18[0] != 0;
      return uVar1 & 0xffffffff;
    }
  }
  return uVar1;
}


/* ================= FUN_140039cf0 @ 140039cf0  size=52  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400452b0 */
/* CALLEES: atof */

undefined8 FUN_140039cf0(undefined8 param_1,longlong param_2,double *param_3)

{
  double dVar1;
  
  if ((param_3 != (double *)0x0) && (param_2 != 0)) {
    dVar1 = atof(*(char **)(param_2 + 8));
    *param_3 = dVar1;
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_140039d30 @ 140039d30  size=13  callers=1 callees=0 ================= */
/* CALLERS: FUN_140039d40 */

undefined8 FUN_140039d30(undefined8 param_1,longlong param_2)

{
  if (param_2 != 0) {
    return *(undefined8 *)(param_2 + 0x40);
  }
  return 0;
}


/* ================= FUN_140039d40 @ 140039d40  size=73  callers=9 callees=2 ================= */
/* CALLERS: FUN_140045370, FUN_140045670, FUN_140045a60, FUN_140045bf0, FUN_140045cc0, FUN_140045d50, FUN_140045ee0, FUN_140045ff0, FUN_140047020 */
/* CALLEES: FUN_140039b60, FUN_140039d30 */

undefined8 FUN_140039d40(longlong param_1,longlong param_2,undefined8 param_3)

{
  longlong lVar1;
  undefined8 uVar2;
  
  if ((param_1 != 0) && (param_2 != 0)) {
    lVar1 = FUN_140039d30();
    if (lVar1 != 0) {
      uVar2 = FUN_140039b60(param_1,lVar1,param_3);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_140039d90 @ 140039d90  size=18  callers=2 callees=0 ================= */
/* CALLERS: FUN_140039db0, FUN_1400471d0 */

longlong FUN_140039d90(longlong *param_1)

{
  longlong lVar1;
  
  lVar1 = 0;
  if ((param_1 != (longlong *)0x0) && (*param_1 != 0)) {
    lVar1 = *param_1;
  }
  return lVar1;
}


/* ================= FUN_140039db0 @ 140039db0  size=68  callers=1 callees=2 ================= */
/* CALLERS: FUN_140047020 */
/* CALLEES: FUN_140039b60, FUN_140039d90 */

undefined8 FUN_140039db0(longlong param_1,undefined8 param_2)

{
  longlong lVar1;
  undefined8 uVar2;
  
  if (param_1 != 0) {
    lVar1 = FUN_140039d90();
    if (lVar1 != 0) {
      uVar2 = FUN_140039b60(param_1,lVar1,param_2);
      return uVar2;
    }
  }
  return 0;
}


/* ================= FUN_140039e00 @ 140039e00  size=50  callers=1 callees=1 ================= */
/* CALLERS: FUN_140045310 */
/* CALLEES: atoi */

undefined8 FUN_140039e00(undefined8 param_1,longlong param_2,int *param_3)

{
  int iVar1;
  
  if ((param_3 != (int *)0x0) && (param_2 != 0)) {
    iVar1 = atoi(*(char **)(param_2 + 8));
    *param_3 = iVar1;
    return 0;
  }
  return 0xffffffff;
}


/* ================= FUN_140039e40 @ 140039e40  size=13  callers=2 callees=0 ================= */
/* CALLERS: FUN_140039b60, FUN_140045cc0 */

undefined8 FUN_140039e40(undefined8 param_1,longlong param_2)

{
  if (param_2 != 0) {
    return *(undefined8 *)(param_2 + 0x38);
  }
  return 0;
}


/* ================= FUN_140039e50 @ 140039e50  size=56  callers=1 callees=1 ================= */
/* CALLERS: FUN_140039b60 */
/* CALLEES: FUN_140027140 */

undefined4 FUN_140039e50(undefined8 param_1,undefined8 *param_2,longlong param_3,undefined4 param_4)

{
  if ((param_3 != 0) && (param_2 != (undefined8 *)0x0)) {
    FUN_140027140(param_3,*param_2,param_4);
    return *(undefined4 *)(param_2 + 1);
  }
  return 0xffffffff;
}


/* ================= FUN_140039e90 @ 140039e90  size=31  callers=1 callees=1 ================= */
/* CALLERS: FUN_1400460b0 */
/* CALLEES: FUN_1400272c0 */

undefined8 FUN_140039e90(undefined8 param_1,longlong param_2,longlong param_3)

{
  undefined8 uVar1;
  
  if ((param_3 != 0) && (param_2 != 0)) {
    uVar1 = FUN_1400272c0(*(undefined8 *)(param_2 + 8),param_3);
    return uVar1;
  }
  return 0xffffffff;
}


/* ================= FUN_140039eb0 @ 140039eb0  size=10  callers=1 callees=1 ================= */
/* CALLERS: FUN_140047020 */
/* CALLEES: FUN_140039ec0 */

void FUN_140039eb0(undefined8 param_1)

{
  FUN_140039ec0(param_1,0,0);
  return;
}


/* ================= FUN_140039ec0 @ 140039ec0  size=71  callers=1 callees=3 ================= */
/* CALLERS: FUN_140039eb0 */
/* CALLEES: CloseHandle, FUN_140039f10, FUN_140043750 */

undefined8 FUN_140039ec0(undefined8 param_1)

{
  HANDLE hObject;
  undefined8 uVar1;
  
  hObject = (HANDLE)FUN_140043750(param_1,5);
  if (hObject != (HANDLE)0xffffffffffffffff) {
    uVar1 = FUN_140039f10(hObject);
    CloseHandle(hObject);
    return uVar1;
  }
  return 0;
}


/* ================= FUN_140039f10 @ 140039f10  size=232  callers=1 callees=9 ================= */
/* CALLERS: FUN_140039ec0 */
/* CALLEES: FUN_140035480, FUN_1400354d0, FUN_140035500, FUN_14003aca0, FUN_140043710, FUN_1400438b0, FUN_1400438e0, FUN_140043d10, FUN_140043e60 */

longlong * FUN_140039f10(undefined8 param_1)

{
  uint uVar1;
  int iVar2;
  longlong lVar3;
  longlong lVar4;
  longlong *plVar5;
  longlong local_res10;
  
  lVar3 = FUN_140035500();
  uVar1 = FUN_140043710(param_1);
  if ((uVar1 != 0) && (lVar4 = FUN_140043d10(uVar1 + 1), lVar4 != 0)) {
    FUN_1400438e0(param_1,0,0);
    FUN_1400438b0(param_1,lVar4,uVar1);
    *(undefined1 *)((ulonglong)uVar1 + lVar4) = 0;
    plVar5 = (longlong *)FUN_140035480(lVar3,0x18);
    if (plVar5 != (longlong *)0x0) {
      *plVar5 = 0;
      plVar5[1] = 0;
      plVar5[2] = lVar3;
      local_res10 = lVar4;
      iVar2 = FUN_14003aca0(plVar5,&local_res10,0,0);
      if ((iVar2 < 0) || (*plVar5 == 0)) {
        FUN_140043e60(lVar4);
        goto LAB_140039fd5;
      }
    }
    FUN_140043e60(lVar4);
    if (plVar5 != (longlong *)0x0) {
      return plVar5;
    }
  }
LAB_140039fd5:
  if (lVar3 != 0) {
    FUN_1400354d0(lVar3);
  }
  return (longlong *)0x0;
}


/* ================= FUN_14003a000 @ 14003a000  size=219  callers=1 callees=6 ================= */
/* CALLERS: FUN_1400471d0 */
/* CALLEES: FUN_1400277b0, FUN_140035180, FUN_1400352e0, FUN_1400353b0, FUN_14003b0c0, memcpy */

uint FUN_14003a000(undefined8 *param_1,void *param_2,uint param_3,uint param_4)

{
  uint uVar1;
  undefined4 uVar2;
  uint uVar3;
  uint uVar4;
  void *local_38;
  undefined8 uStack_30;
  undefined8 local_28;
  undefined8 uStack_20;
  undefined8 local_18;
  
  if (param_2 == (void *)0x0) {
    uVar4 = 0xffffffff;
  }
  else {
    local_18 = 0;
    local_38 = (void *)0x0;
    uStack_30 = 0;
    local_28 = 0;
    uStack_20 = 0;
    FUN_1400352e0(&local_38,0,0x100000,0x1000,0);
    if ((param_4 & 1) == 0) {
      uVar2 = FUN_1400277b0("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n");
      FUN_140035180(&local_38,"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n",
                    uVar2);
    }
    FUN_14003b0c0(*param_1,&local_38);
    uVar3 = (int)local_28 + 1;
    uVar1 = param_3;
    if (uVar3 < param_3) {
      uVar1 = uVar3;
    }
    uVar4 = 0xfffffffe;
    if (uVar3 <= param_3) {
      uVar4 = uVar3;
    }
    memcpy(param_2,local_38,(ulonglong)uVar1);
    FUN_1400353b0(&local_38);
  }
  return uVar4;
}


/* ================= FUN_14003a0e0 @ 14003a0e0  size=263  callers=2 callees=5 ================= */
/* CALLERS: FUN_1400397a0, FUN_1400397b0 */
/* CALLEES: FUN_140027140, FUN_1400277b0, FUN_140035480, FUN_140039b60, FUN_14003a970 */

undefined8 * FUN_14003a0e0(longlong param_1,longlong param_2,char *param_3,int param_4)

{
  int iVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  longlong lVar4;
  
  if (((param_2 == 0) || (param_3 == (char *)0x0)) || (*param_3 == '\0')) {
    return (undefined8 *)0x0;
  }
  if (((*(longlong *)(param_2 + 0x40) != 0) && (param_4 != 0)) &&
     (puVar2 = (undefined8 *)FUN_140039b60(), puVar2 != (undefined8 *)0x0)) {
    return puVar2;
  }
  iVar1 = FUN_1400277b0(param_3);
  puVar2 = (undefined8 *)FUN_140035480(*(undefined8 *)(param_1 + 0x10),0x48);
  *puVar2 = 0;
  puVar2[1] = 0;
  puVar2[2] = 0;
  puVar2[3] = 0;
  puVar2[4] = 0;
  puVar2[5] = 0;
  puVar2[6] = 0;
  puVar2[7] = 0;
  puVar2[8] = 0;
  uVar3 = FUN_140035480(*(undefined8 *)(param_1 + 0x10),iVar1 + 1);
  *puVar2 = uVar3;
  FUN_140027140(uVar3,param_3,iVar1 + 1);
  *(int *)(puVar2 + 1) = iVar1;
  puVar2[5] = param_2;
  if (*(longlong *)(param_2 + 0x40) == 0) {
    *(undefined8 **)(param_2 + 0x40) = puVar2;
    return puVar2;
  }
  lVar4 = FUN_14003a970(param_2);
  *(undefined8 **)(lVar4 + 0x38) = puVar2;
  puVar2[6] = lVar4;
  return puVar2;
}


/* ================= FUN_14003a1f0 @ 14003a1f0  size=65  callers=6 callees=1 ================= */
/* CALLERS: FUN_14003a240, FUN_14003a710, FUN_14003a9a0, FUN_14003aa40, FUN_14003ac20, FUN_14003aca0 */
/* CALLEES: FUN_14003a7d0 */

ulonglong FUN_14003a1f0(undefined8 param_1,undefined8 param_2)

{
  ulonglong uVar1;
  int iVar2;
  ulonglong uVar3;
  
  uVar3 = FUN_14003a7d0();
  iVar2 = (int)uVar3;
  uVar1 = uVar3;
  while (0 < iVar2) {
    iVar2 = FUN_14003a7d0(param_1,param_2);
    uVar1 = uVar3 & 0xffffffff;
  }
  return uVar1;
}


/* ================= FUN_14003a240 @ 14003a240  size=1218  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003aca0 */
/* CALLEES: FUN_1400277b0, FUN_1400277f0, FUN_14003a1f0 */

undefined8 FUN_14003a240(undefined8 param_1,longlong *param_2)

{
  longlong lVar1;
  int iVar2;
  int iVar3;
  undefined8 uVar4;
  longlong lVar5;
  longlong local_res10;
  
  local_res10 = *param_2;
  uVar4 = 0;
  FUN_14003a1f0(param_1,&local_res10);
  iVar2 = FUN_1400277b0("<?xml");
  lVar1 = local_res10;
  iVar3 = FUN_1400277f0(local_res10,"<?xml",(longlong)iVar2);
  if (iVar3 != 0) goto LAB_14003a6d4;
  local_res10 = lVar1 + iVar2;
  FUN_14003a1f0(param_1,&local_res10);
  iVar2 = FUN_1400277b0("version");
  lVar1 = local_res10;
  iVar3 = FUN_1400277f0(local_res10,"version",(longlong)iVar2);
  if (iVar3 == 0) {
    local_res10 = lVar1 + iVar2;
    FUN_14003a1f0(param_1,&local_res10);
    iVar2 = FUN_1400277b0(&DAT_14006047c);
    lVar1 = local_res10;
    iVar3 = FUN_1400277f0(local_res10,&DAT_14006047c,(longlong)iVar2);
    if (iVar3 == 0) {
      local_res10 = lVar1 + iVar2;
      FUN_14003a1f0(param_1,&local_res10);
      iVar2 = FUN_1400277b0(&DAT_14006e2d0);
      lVar1 = local_res10;
      iVar3 = FUN_1400277f0(local_res10,&DAT_14006e2d0,(longlong)iVar2);
      if (iVar3 == 0) {
        local_res10 = lVar1 + iVar2;
        FUN_14003a1f0(param_1,&local_res10);
        iVar2 = FUN_1400277b0(&DAT_140071880);
        lVar1 = local_res10;
        lVar5 = (longlong)iVar2;
        iVar2 = FUN_1400277f0(local_res10,&DAT_140071880,lVar5);
        if (iVar2 == 0) {
          local_res10 = lVar5 + lVar1;
          if ((byte)(*(char *)(lVar5 + lVar1) - 0x30U) < 10) {
            local_res10 = local_res10 + 1;
            FUN_14003a1f0(param_1,&local_res10);
            iVar2 = FUN_1400277b0(&DAT_14006e2d0);
            lVar1 = local_res10;
            iVar3 = FUN_1400277f0(local_res10,&DAT_14006e2d0,(longlong)iVar2);
            if (iVar3 == 0) {
              local_res10 = lVar1 + iVar2;
              FUN_14003a1f0(param_1,&local_res10);
              iVar2 = FUN_1400277b0("encoding");
              lVar1 = local_res10;
              iVar3 = FUN_1400277f0(local_res10,"encoding",(longlong)iVar2);
              if (iVar3 == 0) {
                local_res10 = lVar1 + iVar2;
                FUN_14003a1f0(param_1,&local_res10);
                iVar2 = FUN_1400277b0(&DAT_14006047c);
                lVar1 = local_res10;
                iVar3 = FUN_1400277f0(local_res10,&DAT_14006047c,(longlong)iVar2);
                if (iVar3 == 0) {
                  local_res10 = lVar1 + iVar2;
                  FUN_14003a1f0(param_1,&local_res10);
                  iVar2 = FUN_1400277b0(&DAT_14006e2d0);
                  lVar1 = local_res10;
                  iVar3 = FUN_1400277f0(local_res10,&DAT_14006e2d0,(longlong)iVar2);
                  if (iVar3 == 0) {
                    local_res10 = lVar1 + iVar2;
                    FUN_14003a1f0(param_1,&local_res10);
                    iVar2 = FUN_1400277b0("UTF-8");
                    lVar1 = local_res10;
                    iVar3 = FUN_1400277f0(local_res10,"UTF-8",(longlong)iVar2);
                    if (iVar3 != 0) {
                      uVar4 = 0xfffffff6;
                      goto LAB_14003a6d4;
                    }
                    local_res10 = lVar1 + iVar2;
                    FUN_14003a1f0(param_1,&local_res10);
                    iVar2 = FUN_1400277b0(&DAT_14006e2d0);
                    lVar1 = local_res10;
                    iVar3 = FUN_1400277f0(local_res10,&DAT_14006e2d0,(longlong)iVar2);
                    if (iVar3 == 0) {
                      local_res10 = lVar1 + iVar2;
                      goto LAB_14003a547;
                    }
                  }
                }
              }
              else {
LAB_14003a547:
                FUN_14003a1f0(param_1,&local_res10);
                iVar2 = FUN_1400277b0("standalone");
                lVar1 = local_res10;
                iVar3 = FUN_1400277f0(local_res10,"standalone",(longlong)iVar2);
                if (iVar3 == 0) {
                  local_res10 = lVar1 + iVar2;
                  FUN_14003a1f0(param_1,&local_res10);
                  iVar2 = FUN_1400277b0(&DAT_14006047c);
                  lVar1 = local_res10;
                  iVar3 = FUN_1400277f0(local_res10,&DAT_14006047c,(longlong)iVar2);
                  if (iVar3 == 0) {
                    local_res10 = lVar1 + iVar2;
                    FUN_14003a1f0(param_1,&local_res10);
                    iVar2 = FUN_1400277b0(&DAT_14006e2d0);
                    lVar1 = local_res10;
                    iVar3 = FUN_1400277f0(local_res10,&DAT_14006e2d0,(longlong)iVar2);
                    if (iVar3 == 0) {
                      local_res10 = lVar1 + iVar2;
                      FUN_14003a1f0(param_1,&local_res10);
                      iVar2 = FUN_1400277b0(&DAT_1400718ac);
                      lVar1 = local_res10;
                      iVar3 = FUN_1400277f0(local_res10,&DAT_1400718ac,(longlong)iVar2);
                      if (iVar3 != 0) {
                        uVar4 = 0xfffffff5;
                        goto LAB_14003a6d4;
                      }
                      local_res10 = lVar1 + iVar2;
                      FUN_14003a1f0(param_1,&local_res10);
                      iVar2 = FUN_1400277b0(&DAT_14006e2d0);
                      lVar1 = local_res10;
                      iVar3 = FUN_1400277f0(local_res10,&DAT_14006e2d0,(longlong)iVar2);
                      if (iVar3 == 0) {
                        local_res10 = lVar1 + iVar2;
                        goto LAB_14003a68d;
                      }
                    }
                  }
                }
                else {
LAB_14003a68d:
                  FUN_14003a1f0(param_1,&local_res10);
                  iVar2 = FUN_1400277b0(&DAT_1400718b0);
                  lVar1 = local_res10;
                  iVar3 = FUN_1400277f0(local_res10,&DAT_1400718b0,(longlong)iVar2);
                  if (iVar3 == 0) {
                    local_res10 = lVar1 + iVar2;
                    goto LAB_14003a6d4;
                  }
                }
              }
            }
            goto LAB_14003a2dc;
          }
        }
        uVar4 = 0xfffffff7;
        goto LAB_14003a6d4;
      }
    }
  }
LAB_14003a2dc:
  uVar4 = 0xfffffff4;
LAB_14003a6d4:
  FUN_14003a1f0(param_1,&local_res10);
  *param_2 = local_res10;
  return uVar4;
}


/* ================= FUN_14003a710 @ 14003a710  size=190  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003aca0 */
/* CALLEES: FUN_1400277b0, FUN_1400277f0, FUN_14003a1f0 */

undefined8 FUN_14003a710(undefined8 param_1,longlong *param_2)

{
  char *pcVar1;
  char *pcVar2;
  char cVar3;
  int iVar4;
  undefined8 uVar5;
  longlong lVar6;
  char *local_res10;
  
  local_res10 = (char *)*param_2;
  uVar5 = 0;
  FUN_14003a1f0(param_1,&local_res10);
  iVar4 = FUN_1400277b0("<!DOCTYPE");
  pcVar1 = local_res10;
  lVar6 = (longlong)iVar4;
  iVar4 = FUN_1400277f0(local_res10,"<!DOCTYPE",lVar6);
  if (iVar4 == 0) {
    pcVar2 = pcVar1 + lVar6;
    cVar3 = pcVar1[lVar6];
    while ((cVar3 != '\0' && (cVar3 != '>'))) {
      pcVar1 = pcVar2 + 1;
      pcVar2 = pcVar2 + 1;
      cVar3 = *pcVar1;
    }
    local_res10 = pcVar2 + 1;
    if (*pcVar2 != '>') {
      uVar5 = 0xfffffff3;
    }
  }
  FUN_14003a1f0(param_1,&local_res10);
  *param_2 = (longlong)local_res10;
  return uVar5;
}


/* ================= FUN_14003a7d0 @ 14003a7d0  size=294  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003a1f0 */
/* CALLEES: FUN_1400277b0, FUN_1400277f0, FUN_14003a900 */

undefined8 FUN_14003a7d0(longlong param_1,longlong *param_2)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  int iVar4;
  char *pcVar5;
  longlong lVar6;
  undefined8 uVar7;
  char *local_res10;
  
  local_res10 = (char *)*param_2;
  uVar7 = 1;
  FUN_14003a900(param_1,&local_res10);
  iVar3 = FUN_1400277b0(&DAT_140071850);
  pcVar5 = local_res10;
  iVar4 = FUN_1400277f0(local_res10,&DAT_140071850,(longlong)iVar3);
  if (iVar4 == 0) {
    pcVar5 = pcVar5 + iVar3;
    local_res10 = pcVar5;
    iVar3 = FUN_1400277b0(&DAT_140071858);
    cVar1 = *pcVar5;
    lVar6 = (longlong)iVar3;
    pcVar2 = local_res10;
    while ((cVar1 != '\0' &&
           ((cVar1 != '-' ||
            (iVar3 = FUN_1400277f0(pcVar5,&DAT_140071858,lVar6), pcVar2 = pcVar5, iVar3 != 0))))) {
      if (*pcVar5 == '\n') {
        *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 1;
      }
      cVar1 = pcVar5[1];
      pcVar5 = pcVar5 + 1;
      pcVar2 = pcVar5;
    }
    local_res10 = pcVar2;
    iVar3 = FUN_1400277f0(pcVar5,&DAT_140071858,lVar6);
    if (iVar3 == 0) {
      local_res10 = pcVar5 + lVar6;
    }
    else {
      uVar7 = 0xfffffff2;
    }
  }
  else {
    uVar7 = 0;
  }
  FUN_14003a900(param_1,&local_res10);
  *param_2 = (longlong)local_res10;
  return uVar7;
}


/* ================= FUN_14003a900 @ 14003a900  size=46  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003a7d0 */

void FUN_14003a900(longlong param_1,undefined8 *param_2)

{
  byte bVar1;
  byte *pbVar2;
  
  pbVar2 = (byte *)*param_2;
  do {
    bVar1 = *pbVar2;
    if ((0x20 < bVar1) || ((0x100002200U >> ((longlong)(char)bVar1 & 0x3fU) & 1) == 0)) {
      if (bVar1 != 10) {
        *param_2 = pbVar2;
        return;
      }
      *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 1;
    }
    pbVar2 = pbVar2 + 1;
  } while( true );
}


/* ================= FUN_14003a930 @ 14003a930  size=51  callers=1 callees=0 ================= */
/* CALLERS: FUN_140039600 */

longlong FUN_14003a930(longlong param_1)

{
  longlong lVar1;
  longlong lVar2;
  
  if (param_1 != 0) {
    lVar2 = *(longlong *)(param_1 + 0x20);
    if (lVar2 != 0) {
      for (lVar1 = *(longlong *)(lVar2 + 0x20); lVar1 != 0; lVar1 = *(longlong *)(lVar1 + 0x20)) {
        lVar2 = lVar1;
      }
    }
    return lVar2;
  }
  return 0;
}


/* ================= FUN_14003a970 @ 14003a970  size=48  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003a0e0 */

longlong FUN_14003a970(longlong param_1)

{
  longlong lVar1;
  longlong lVar2;
  
  lVar2 = 0;
  if (param_1 != 0) {
    lVar2 = *(longlong *)(param_1 + 0x40);
    if (lVar2 == 0) {
      return 0;
    }
    for (lVar1 = *(longlong *)(lVar2 + 0x38); lVar1 != 0; lVar1 = *(longlong *)(lVar1 + 0x38)) {
      lVar2 = lVar1;
    }
  }
  return lVar2;
}


/* ================= FUN_14003a9a0 @ 14003a9a0  size=147  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003aa40 */
/* CALLEES: FUN_14003a1f0 */

char * FUN_14003a9a0(undefined8 param_1,longlong *param_2,undefined8 *param_3)

{
  char *pcVar1;
  char cVar2;
  char *pcVar3;
  char *pcVar4;
  char *pcVar5;
  char *local_res10;
  
  local_res10 = (char *)*param_2;
  pcVar5 = (char *)0x0;
  FUN_14003a1f0(param_1,&local_res10);
  pcVar3 = pcVar5;
  if (*local_res10 == '\"') {
    pcVar3 = local_res10 + 1;
    cVar2 = local_res10[1];
    local_res10 = pcVar3;
    pcVar4 = pcVar5;
    while (cVar2 != '\0') {
      if (cVar2 == '\"') {
        if (*local_res10 == '\"') {
          local_res10 = local_res10 + 1;
          pcVar5 = pcVar4;
        }
        break;
      }
      pcVar1 = local_res10 + 1;
      local_res10 = local_res10 + 1;
      pcVar4 = (char *)(ulonglong)((int)pcVar4 + 1);
      cVar2 = *pcVar1;
    }
  }
  *param_3 = pcVar3;
  *param_2 = (longlong)local_res10;
  return pcVar5;
}


/* ================= FUN_14003aa40 @ 14003aa40  size=475  callers=1 callees=5 ================= */
/* CALLERS: FUN_14003aca0 */
/* CALLEES: FUN_140035480, FUN_14003a1f0, FUN_14003a9a0, FUN_14003ac20, memcpy */

longlong * FUN_14003aa40(longlong param_1,undefined8 *param_2)

{
  char cVar1;
  longlong *plVar2;
  longlong *plVar3;
  uint uVar4;
  uint uVar5;
  longlong *plVar6;
  longlong lVar7;
  int iVar8;
  longlong *plVar9;
  longlong *plVar10;
  char *local_res18;
  void *local_res20;
  void *local_48 [2];
  
  local_res18 = (char *)*param_2;
  plVar10 = (longlong *)0x0;
  FUN_14003a1f0(param_1,&local_res18);
  cVar1 = *local_res18;
  plVar2 = plVar10;
  plVar3 = plVar10;
  while( true ) {
    iVar8 = 0;
    if ((cVar1 == '>') || (iVar8 = 0, cVar1 == '/')) goto LAB_14003abfd;
    uVar4 = FUN_14003ac20(param_1,&local_res18,&local_res20);
    if ((uVar4 == 0) || (FUN_14003a1f0(param_1,&local_res18), *local_res18 != '=')) break;
    local_res18 = local_res18 + 1;
    FUN_14003a1f0(param_1,&local_res18);
    uVar5 = FUN_14003a9a0(param_1,&local_res18,local_48);
    plVar6 = (longlong *)FUN_140035480(*(undefined8 *)(param_1 + 0x10),0x28);
    if (plVar6 == (longlong *)0x0) break;
    *plVar6 = 0;
    plVar6[1] = 0;
    plVar6[2] = 0;
    plVar6[3] = 0;
    plVar6[4] = 0;
    lVar7 = FUN_140035480(*(undefined8 *)(param_1 + 0x10),uVar4 + 1);
    *plVar6 = lVar7;
    lVar7 = FUN_140035480(*(undefined8 *)(param_1 + 0x10),uVar5 + 1);
    plVar6[1] = lVar7;
    if (((void *)*plVar6 == (void *)0x0) || (lVar7 == 0)) break;
    *(uint *)((longlong)plVar6 + 0x14) = uVar5;
    *(uint *)(plVar6 + 2) = uVar4;
    memcpy((void *)*plVar6,local_res20,(ulonglong)uVar4);
    *(undefined1 *)((ulonglong)uVar4 + *plVar6) = 0;
    memcpy((void *)plVar6[1],local_48[0],(ulonglong)uVar5);
    *(undefined1 *)((ulonglong)uVar5 + plVar6[1]) = 0;
    plVar6[4] = 0;
    plVar9 = plVar6;
    if (plVar3 != (longlong *)0x0) {
      plVar2[4] = (longlong)plVar6;
      plVar6[3] = (longlong)plVar2;
      plVar9 = plVar3;
    }
    FUN_14003a1f0(param_1,&local_res18);
    cVar1 = *local_res18;
    plVar2 = plVar6;
    plVar3 = plVar9;
  }
  iVar8 = -1;
LAB_14003abfd:
  if (-1 < iVar8) {
    plVar10 = plVar3;
  }
  *param_2 = local_res18;
  return plVar10;
}


/* ================= FUN_14003ac20 @ 14003ac20  size=124  callers=2 callees=1 ================= */
/* CALLERS: FUN_14003aa40, FUN_14003aca0 */
/* CALLEES: FUN_14003a1f0 */

int FUN_14003ac20(undefined8 param_1,undefined8 *param_2,undefined8 *param_3)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  char *local_res10;
  
  local_res10 = (char *)*param_2;
  FUN_14003a1f0(param_1,&local_res10);
  pcVar3 = local_res10;
  for (iVar2 = 0;
      (((cVar1 = *pcVar3, (byte)(cVar1 + 0xbfU) < 0x1a || ((byte)(cVar1 + 0x9fU) < 0x1a)) ||
       ((byte)(cVar1 - 0x30U) < 10)) || ((cVar1 == '_' || ((cVar1 == '-' && (iVar2 != 0))))));
      iVar2 = iVar2 + 1) {
    pcVar3 = pcVar3 + 1;
  }
  *param_3 = local_res10;
  *param_2 = pcVar3;
  return iVar2;
}


/* ================= FUN_14003aca0 @ 14003aca0  size=813  callers=2 callees=11 ================= */
/* CALLERS: FUN_140039f10, FUN_14003aca0 */
/* CALLEES: FUN_140027760, FUN_140035480, FUN_14003a1f0, FUN_14003a240, FUN_14003a710, FUN_14003aa40, FUN_14003ac20, FUN_14003aca0, __security_check_cookie, memcpy, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

int FUN_14003aca0(longlong *param_1,undefined8 *param_2,undefined8 *param_3,int param_4)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  void *_Dst;
  undefined8 *puVar5;
  undefined8 uVar6;
  undefined1 auStack_1a8 [32];
  char *local_188;
  int local_180;
  int local_17c;
  undefined8 *local_178;
  void *local_170;
  undefined8 *local_168;
  undefined1 local_158 [256];
  ulonglong local_58;
  
  local_58 = DAT_140096fe8 ^ (ulonglong)auStack_1a8;
  local_17c = param_4;
  local_168 = param_2;
  memset(local_158,0,0x100);
  local_188 = (char *)*param_2;
  iVar3 = 0;
  local_180 = 0;
  local_178 = (undefined8 *)0x0;
  while( true ) {
    iVar4 = local_180;
    if (100 < param_4) {
      iVar3 = -8;
      goto LAB_14003af97;
    }
    do {
      iVar1 = FUN_14003a240(param_1,&local_188);
    } while (iVar1 != 0);
    do {
      iVar1 = FUN_14003a710(param_1,&local_188);
    } while (iVar1 != 0);
    if (*local_188 == '\0') goto LAB_14003af97;
    if (*local_188 != '<') {
      iVar3 = -2;
      goto LAB_14003af97;
    }
    local_188 = local_188 + 1;
    FUN_14003a1f0(param_1,&local_188);
    if (*local_188 == '/') {
      iVar4 = 1;
      local_188 = local_188 + 1;
      local_180 = 1;
    }
    uVar2 = FUN_14003ac20(param_1,&local_188,&local_170);
    if (0xfe < uVar2 - 1) {
      iVar3 = -3;
      goto LAB_14003af97;
    }
    memcpy(local_158,local_170,(ulonglong)uVar2);
    local_158[uVar2] = 0;
    if (iVar4 != 0) break;
    _Dst = (void *)FUN_140035480(param_1[2],uVar2 + 1);
    puVar5 = (undefined8 *)FUN_140035480(param_1[2],0x48);
    if ((_Dst == (void *)0x0) || (puVar5 == (undefined8 *)0x0)) {
      iVar3 = -1;
      goto LAB_14003af97;
    }
    memcpy(_Dst,local_158,(ulonglong)(uVar2 + 1));
    *(undefined8 *)((longlong)puVar5 + 0xc) = 0;
    *(undefined8 *)((longlong)puVar5 + 0x14) = 0;
    *(undefined8 *)((longlong)puVar5 + 0x1c) = 0;
    *(undefined8 *)((longlong)puVar5 + 0x24) = 0;
    *(undefined8 *)((longlong)puVar5 + 0x2c) = 0;
    *(undefined8 *)((longlong)puVar5 + 0x34) = 0;
    *(undefined8 *)((longlong)puVar5 + 0x3c) = 0;
    *(undefined4 *)((longlong)puVar5 + 0x44) = 0;
    *puVar5 = _Dst;
    *(uint *)(puVar5 + 1) = uVar2;
    uVar6 = FUN_14003aa40(param_1,&local_188);
    puVar5[4] = uVar6;
    if (*param_1 == 0) {
      *param_1 = (longlong)puVar5;
    }
    if (local_178 != (undefined8 *)0x0) {
      local_178[7] = puVar5;
    }
    puVar5[5] = param_3;
    if ((param_3 != (undefined8 *)0x0) && (param_3[8] == 0)) {
      param_3[8] = puVar5;
    }
    local_178 = puVar5;
    FUN_14003a1f0(param_1,&local_188);
    if (*local_188 == '/') {
      local_188 = local_188 + 1;
      FUN_14003a1f0(param_1,&local_188);
      if (*local_188 != '>') {
LAB_14003af73:
        iVar3 = -6;
LAB_14003af97:
        *local_168 = local_188;
        return iVar3;
      }
      local_188 = local_188 + 1;
    }
    else {
      FUN_14003a1f0(param_1,&local_188);
      if (*local_188 != '>') goto LAB_14003af73;
      local_188 = local_188 + 1;
      iVar3 = FUN_14003aca0(param_1,&local_188,puVar5,local_17c + 1);
    }
    if ((iVar3 < 0) || (param_4 = local_17c, *local_188 == '\0')) goto LAB_14003af97;
  }
  if (param_3 == (undefined8 *)0x0) {
    iVar3 = -4;
  }
  else {
    iVar4 = FUN_140027760(*param_3,local_158);
    if (iVar4 == 0) {
      FUN_14003a1f0(param_1,&local_188);
      if (*local_188 != '>') goto LAB_14003af73;
      local_188 = local_188 + 1;
    }
    else {
      iVar3 = -5;
    }
  }
  goto LAB_14003af97;
}


/* ================= FUN_14003afd0 @ 14003afd0  size=146  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b0c0 */
/* CALLEES: FUN_140035180 */

void FUN_14003afd0(undefined8 *param_1,undefined8 param_2)

{
  for (; param_1 != (undefined8 *)0x0; param_1 = (undefined8 *)param_1[4]) {
    FUN_140035180(param_2,&DAT_14005f570,1);
    FUN_140035180(param_2,*param_1,*(undefined4 *)(param_1 + 2));
    FUN_140035180(param_2,&DAT_1400718b8,2);
    FUN_140035180(param_2,param_1[1],*(undefined4 *)((longlong)param_1 + 0x14));
    FUN_140035180(param_2,&DAT_14006e2d0,1);
  }
  return;
}


/* ================= FUN_14003b070 @ 14003b070  size=70  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b0c0 */
/* CALLEES: FUN_140035180 */

void FUN_14003b070(uint param_1,undefined8 param_2)

{
  ulonglong uVar1;
  
  if (param_1 != 0) {
    uVar1 = (ulonglong)param_1;
    do {
      FUN_140035180(param_2,&DAT_1400718b4,2);
      uVar1 = uVar1 - 1;
    } while (uVar1 != 0);
  }
  return;
}


/* ================= FUN_14003b0c0 @ 14003b0c0  size=367  callers=2 callees=4 ================= */
/* CALLERS: FUN_14003a000, FUN_14003b0c0 */
/* CALLEES: FUN_140035180, FUN_14003afd0, FUN_14003b070, FUN_14003b0c0 */

void FUN_14003b0c0(undefined8 *param_1,undefined8 param_2)

{
  undefined *puVar1;
  undefined8 uVar2;
  
  do {
    if (param_1 == (undefined8 *)0x0) {
      return;
    }
    FUN_14003b070(DAT_14085d178,param_2);
    FUN_140035180(param_2,&DAT_1400605a0,1);
    FUN_140035180(param_2,*param_1,*(undefined4 *)(param_1 + 1));
    FUN_14003afd0(param_1[4],param_2);
    if (param_1[8] == 0) {
      if (param_1[2] != 0) {
        FUN_140035180(param_2,&DAT_140060568,1);
        FUN_140035180(param_2,param_1[2],*(undefined4 *)(param_1 + 3));
        goto LAB_14003b1ba;
      }
      puVar1 = &DAT_1400718bc;
      uVar2 = 2;
    }
    else {
      FUN_140035180(param_2,&DAT_140060568,1);
      if (param_1[2] != 0) {
        FUN_140035180(param_2,param_1[2],*(undefined4 *)(param_1 + 3));
      }
      FUN_140035180(param_2,&DAT_14006f760,2);
      DAT_14085d178 = DAT_14085d178 + 1;
      FUN_14003b0c0(param_1[8],param_2);
      DAT_14085d178 = DAT_14085d178 + -1;
      FUN_14003b070(DAT_14085d178,param_2);
LAB_14003b1ba:
      FUN_140035180(param_2,&DAT_14006e2b8,2);
      FUN_140035180(param_2,*param_1,*(undefined4 *)(param_1 + 1));
      puVar1 = &DAT_140060568;
      uVar2 = 1;
    }
    FUN_140035180(param_2,puVar1,uVar2);
    FUN_140035180(param_2,&DAT_14006f760,2);
    param_1 = (undefined8 *)param_1[7];
  } while( true );
}


/* ================= FUN_14003b230 @ 14003b230  size=46  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003b290 */
/* CALLEES: close */

void FUN_14003b230(QWidget *param_1,int param_2,int param_3)

{
  if (param_2 == 0) {
    if (param_3 == 0) {
                    /* WARNING: Could not recover jumptable at 0x00014003b25d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      QMetaObject::activate((QObject *)param_1,(QMetaObject *)&DAT_140096b30,0,(void **)0x0);
      return;
    }
    if (param_3 == 1) {
      QWidget::close(param_1);
      return;
    }
  }
  return;
}


/* ================= FUN_14003b270 @ 14003b270  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003b270(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096b30;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003b290 @ 14003b290  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003b230, qt_metacall */

int FUN_14003b290(QDialog *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QDialog::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 2) {
      FUN_14003b230(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -2;
  }
  return iVar1;
}


/* ================= FUN_14003b2f0 @ 14003b2f0  size=72  callers=0 callees=0 ================= */

QDialog * FUN_14003b2f0(QDialog *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QDialog *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QDialog *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"AboutDialog" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003b331. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QDialog::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003b340 @ 14003b340  size=24  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003b440 */

void FUN_14003b340(longlong *param_1,int param_2,int param_3)

{
  if ((param_2 == 0) && (param_3 == 0)) {
                    /* WARNING: Could not recover jumptable at 0x00014002fac3. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*param_1 + 0x198))();
    return;
  }
  return;
}


/* ================= FUN_14003b350 @ 14003b350  size=37  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003b3e0 */
/* CALLEES: FUN_14002f980, FUN_14002f9c0, FUN_14002fa80 */

void FUN_14003b350(undefined8 param_1,int param_2,int param_3)

{
  if (param_2 == 0) {
    if (param_3 == 0) {
      FUN_14002f980();
      return;
    }
    if (param_3 == 1) {
      FUN_14002f9c0();
      return;
    }
    if (param_3 == 2) {
      FUN_14002fa80();
      return;
    }
  }
  return;
}


/* ================= FUN_14003b380 @ 14003b380  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003b380(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096b90;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003b3a0 @ 14003b3a0  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003b3a0(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096b70;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003b3c0 @ 14003b3c0  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003b3c0(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096b50;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003b3e0 @ 14003b3e0  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003b350, qt_metacall */

int FUN_14003b3e0(QSortFilterProxyModel *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QSortFilterProxyModel::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 3) {
      FUN_14003b350(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -3;
  }
  return iVar1;
}


/* ================= FUN_14003b440 @ 14003b440  size=90  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003b340, qt_metacall */

int FUN_14003b440(QDialog *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QDialog::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 1) {
      FUN_14003b340(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -1;
  }
  return iVar1;
}


/* ================= FUN_14003b4b0 @ 14003b4b0  size=72  callers=0 callees=0 ================= */

QSortFilterProxyModel * FUN_14003b4b0(QSortFilterProxyModel *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QSortFilterProxyModel *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QSortFilterProxyModel *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"CDeviceDataProxyModel" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003b4f1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QSortFilterProxyModel::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003b500 @ 14003b500  size=72  callers=0 callees=0 ================= */

QDialog * FUN_14003b500(QDialog *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QDialog *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QDialog *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"CDeviceSelectionDialog" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003b541. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QDialog::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003b550 @ 14003b550  size=72  callers=0 callees=0 ================= */

QTableView * FUN_14003b550(QTableView *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QTableView *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QTableView *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"CTableView" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003b591. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QTableView::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003b6b0 @ 14003b6b0  size=20  callers=1 callees=0 ================= */
/* CALLERS: FUN_140034150 */

void FUN_14003b6b0(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003b6bd. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096bf0,0,(void **)0x0);
  return;
}


/* ================= FUN_14003b6f0 @ 14003b6f0  size=48  callers=1 callees=4 ================= */
/* CALLERS: FUN_14003b740 */
/* CALLEES: FUN_140034cf0, FUN_140034d10, FUN_140034d30, FUN_140034fd0 */

void FUN_14003b6f0(undefined8 param_1,int param_2,int param_3)

{
  if (param_2 == 0) {
    if (param_3 == 0) {
      FUN_140034cf0();
      return;
    }
    if (param_3 == 1) {
      FUN_140034d10();
      return;
    }
    if (param_3 == 2) {
      FUN_140034fd0();
      return;
    }
    if (param_3 == 3) {
      FUN_140034d30();
      return;
    }
  }
  return;
}


/* ================= FUN_14003b720 @ 14003b720  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003b720(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096c10;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003b740 @ 14003b740  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003b6f0, qt_metacall */

int FUN_14003b740(QDialog *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QDialog::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 4) {
      FUN_14003b6f0(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -4;
  }
  return iVar1;
}


/* ================= FUN_14003b7a0 @ 14003b7a0  size=72  callers=0 callees=0 ================= */

QDialog * FUN_14003b7a0(QDialog *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QDialog *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QDialog *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"DIALOG_CustMsgBox" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003b7e1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QDialog::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003b7f0 @ 14003b7f0  size=32  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003b830 */
/* CALLEES: FUN_140048a50, FUN_140048d10 */

void FUN_14003b7f0(undefined8 param_1,int param_2,int param_3,longlong param_4)

{
  if (param_2 == 0) {
    if (param_3 == 0) {
      FUN_140048a50(param_1,**(undefined4 **)(param_4 + 8));
      return;
    }
    if (param_3 == 1) {
      FUN_140048d10();
      return;
    }
  }
  return;
}


/* ================= FUN_14003b810 @ 14003b810  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003b810(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096c30;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003b830 @ 14003b830  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003b7f0, qt_metacall */

int FUN_14003b830(QWidget *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QWidget::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 2) {
      FUN_14003b7f0(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -2;
  }
  return iVar1;
}


/* ================= FUN_14003b890 @ 14003b890  size=72  callers=0 callees=0 ================= */

QWidget * FUN_14003b890(QWidget *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QWidget *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QWidget *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"GraphContainer" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003b8d1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QWidget::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003b8e0 @ 14003b8e0  size=116  callers=1 callees=15 ================= */
/* CALLERS: FUN_14003b9d0 */
/* CALLEES: FUN_14003b990, FUN_14004a560, FUN_14004a6e0, FUN_14004a750, FUN_14004a7c0, FUN_14004a840, FUN_14004bbb0, FUN_14004bbc0, FUN_14004bbd0, FUN_14004bbe0, FUN_14004bbf0, FUN_14004bc00, FUN_14004bc10, FUN_14004bc20, FUN_14004bc30 */

void FUN_14003b8e0(undefined8 param_1,int param_2,undefined4 param_3,longlong param_4)

{
  if (param_2 == 0) {
    switch(param_3) {
    case 0:
      FUN_14003b990();
      return;
    case 1:
      FUN_14004bc30();
      return;
    case 2:
      FUN_14004a7c0();
      return;
    case 3:
      FUN_14004a750();
      return;
    case 4:
      FUN_14004a840();
      return;
    case 5:
      FUN_14004a6e0();
      return;
    case 6:
      FUN_14004a560(param_1,**(undefined8 **)(param_4 + 8));
      return;
    case 7:
      FUN_14004bc10();
      return;
    case 8:
      FUN_14004bc20();
      return;
    case 9:
      FUN_14004bbb0();
      return;
    case 10:
      FUN_14004bbc0();
      return;
    case 0xb:
      FUN_14004bbf0();
      return;
    case 0xc:
      FUN_14004bc00();
      return;
    case 0xd:
      FUN_14004bbd0();
      return;
    case 0xe:
      FUN_14004bbe0();
      return;
    }
  }
  return;
}


/* ================= FUN_14003b990 @ 14003b990  size=20  callers=5 callees=0 ================= */
/* CALLERS: FUN_14003b8e0, FUN_140049950, FUN_1400499e0, FUN_140049a10, FUN_140049c20 */

void FUN_14003b990(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003b99d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c50,0,(void **)0x0);
  return;
}


/* ================= FUN_14003b9b0 @ 14003b9b0  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003b9b0(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096c50;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003b9d0 @ 14003b9d0  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003b8e0, qt_metacall */

int FUN_14003b9d0(QWidget *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QWidget::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 0xf) {
      FUN_14003b8e0(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -0xf;
  }
  return iVar1;
}


/* ================= FUN_14003ba30 @ 14003ba30  size=72  callers=0 callees=0 ================= */

QWidget * FUN_14003ba30(QWidget *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QWidget *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QWidget *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"GraphWidget" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003ba71. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QWidget::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003ba80 @ 14003ba80  size=207  callers=1 callees=33 ================= */
/* CALLERS: FUN_14003be00 */
/* CALLEES: FUN_14003bbe0, FUN_14003bc00, FUN_14003bc20, FUN_14003bc40, FUN_14003bc60, FUN_14003bc80, FUN_14003bca0, FUN_14003bcc0, FUN_14003bce0, FUN_14003bd00, FUN_14003bd20, FUN_14003bd40, FUN_14003bd60, FUN_14003bd80, FUN_14003bda0, FUN_14003bdc0, FUN_14004d290, FUN_14004d380, FUN_14004d460, FUN_14004d4e0, FUN_14004d540, FUN_14004d5a0, FUN_14004d5e0, FUN_14004d650, FUN_14004d6c0, FUN_14004d7e0, FUN_14004d830, FUN_14004d870, FUN_14004d900, FUN_14004d930, FUN_14004d990, FUN_14004d9c0, close */

void FUN_14003ba80(QWidget *param_1,int param_2,undefined4 param_3)

{
  if (param_2 == 0) {
    switch(param_3) {
    case 0:
      FUN_14003bc20();
      return;
    case 1:
      FUN_14003bd60();
      return;
    case 2:
      FUN_14003bd40();
      return;
    case 3:
      FUN_14003bd80();
      return;
    case 4:
      FUN_14003bdc0();
      return;
    case 5:
      FUN_14003bc60();
      return;
    case 6:
      FUN_14003bc80();
      return;
    case 7:
      FUN_14003bd20();
      return;
    case 8:
      FUN_14003bcc0();
      return;
    case 9:
      FUN_14003bda0();
      return;
    case 10:
      FUN_14003bc40();
      return;
    case 0xb:
      FUN_14003bbe0();
      return;
    case 0xc:
      FUN_14003bca0();
      return;
    case 0xd:
      FUN_14003bc00();
      return;
    case 0xe:
      FUN_14003bce0();
      return;
    case 0xf:
      FUN_14003bd00();
      return;
    case 0x10:
      FUN_14004d4e0();
      return;
    case 0x11:
      FUN_14004d9c0();
      return;
    case 0x12:
      FUN_14004d6c0();
      return;
    case 0x13:
      FUN_14004d5a0();
      return;
    case 0x14:
      FUN_14004d930();
      return;
    case 0x15:
      FUN_14004d870();
      return;
    case 0x16:
      FUN_14004d830();
      return;
    case 0x17:
      FUN_14004d900();
      return;
    case 0x18:
      FUN_14004d990();
      return;
    case 0x19:
      FUN_14004d460();
      return;
    case 0x1a:
      FUN_14004d290();
      return;
    case 0x1b:
      FUN_14004d7e0();
      return;
    case 0x1c:
      FUN_14004d540();
      return;
    case 0x1d:
      QWidget::close(param_1);
      return;
    case 0x1e:
      FUN_14004d380();
      return;
    case 0x1f:
      FUN_14004d5e0();
      return;
    case 0x20:
      FUN_14004d650();
      return;
    }
  }
  return;
}


/* ================= FUN_14003bbe0 @ 14003bbe0  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bbe0(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bbee. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,0xb,(void **)0x0);
  return;
}


/* ================= FUN_14003bc00 @ 14003bc00  size=21  callers=2 callees=0 ================= */
/* CALLERS: FUN_14003ba80, FUN_14004dca0 */

void FUN_14003bc00(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bc0e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,0xd,(void **)0x0);
  return;
}


/* ================= FUN_14003bc20 @ 14003bc20  size=20  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bc20(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bc2d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,0,(void **)0x0);
  return;
}


/* ================= FUN_14003bc40 @ 14003bc40  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bc40(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bc4e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,10,(void **)0x0);
  return;
}


/* ================= FUN_14003bc60 @ 14003bc60  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bc60(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bc6e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,5,(void **)0x0);
  return;
}


/* ================= FUN_14003bc80 @ 14003bc80  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bc80(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bc8e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,6,(void **)0x0);
  return;
}


/* ================= FUN_14003bca0 @ 14003bca0  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bca0(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bcae. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,0xc,(void **)0x0);
  return;
}


/* ================= FUN_14003bcc0 @ 14003bcc0  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bcc0(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bcce. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,8,(void **)0x0);
  return;
}


/* ================= FUN_14003bce0 @ 14003bce0  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bce0(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bcee. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,0xe,(void **)0x0);
  return;
}


/* ================= FUN_14003bd00 @ 14003bd00  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bd00(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bd0e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,0xf,(void **)0x0);
  return;
}


/* ================= FUN_14003bd20 @ 14003bd20  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bd20(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bd2e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,7,(void **)0x0);
  return;
}


/* ================= FUN_14003bd40 @ 14003bd40  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bd40(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bd4e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,2,(void **)0x0);
  return;
}


/* ================= FUN_14003bd60 @ 14003bd60  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bd60(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bd6e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,1,(void **)0x0);
  return;
}


/* ================= FUN_14003bd80 @ 14003bd80  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bd80(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bd8e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,3,(void **)0x0);
  return;
}


/* ================= FUN_14003bda0 @ 14003bda0  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bda0(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bdae. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,9,(void **)0x0);
  return;
}


/* ================= FUN_14003bdc0 @ 14003bdc0  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003ba80 */

void FUN_14003bdc0(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003bdce. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096c70,4,(void **)0x0);
  return;
}


/* ================= FUN_14003bde0 @ 14003bde0  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003bde0(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096c70;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003be00 @ 14003be00  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003ba80, qt_metacall */

int FUN_14003be00(QMainWindow *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QMainWindow::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 0x21) {
      FUN_14003ba80(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -0x21;
  }
  return iVar1;
}


/* ================= FUN_14003be60 @ 14003be60  size=72  callers=0 callees=0 ================= */

QMainWindow * FUN_14003be60(QMainWindow *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QMainWindow *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QMainWindow *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"MainWindow" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003bea1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QMainWindow::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003beb0 @ 14003beb0  size=99  callers=1 callees=13 ================= */
/* CALLERS: FUN_14003bf70 */
/* CALLEES: FUN_14002a0c0, FUN_14002a130, FUN_14002a160, FUN_14002a190, FUN_14002a1a0, FUN_14002a1b0, FUN_14002a1c0, FUN_14002a1f0, FUN_14002a260, thunk_FUN_14003e6e0, thunk_FUN_14003e740, thunk_FUN_140047390, thunk_FUN_1400476d0 */

void FUN_14003beb0(undefined8 param_1,int param_2,undefined4 param_3)

{
  if (param_2 == 0) {
    switch(param_3) {
    case 0:
      FUN_14002a1f0();
      return;
    case 1:
      FUN_14002a0c0();
      return;
    case 2:
      FUN_14002a130();
      return;
    case 3:
      FUN_14002a1a0();
      return;
    case 4:
      thunk_FUN_14003e6e0();
      return;
    case 5:
      thunk_FUN_14003e740();
      return;
    case 6:
      FUN_14002a1b0();
      return;
    case 7:
      FUN_14002a1c0();
      return;
    case 8:
      thunk_FUN_140047390();
      return;
    case 9:
      thunk_FUN_1400476d0();
      return;
    case 10:
      FUN_14002a190();
      return;
    case 0xb:
      FUN_14002a160();
      return;
    case 0xc:
      FUN_14002a260();
      return;
    }
  }
  return;
}


/* ================= FUN_14003bf50 @ 14003bf50  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003bf50(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096c90;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003bf70 @ 14003bf70  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003beb0, qt_metacall */

int FUN_14003bf70(QMenuBar *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QMenuBar::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 0xd) {
      FUN_14003beb0(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -0xd;
  }
  return iVar1;
}


/* ================= FUN_14003bfd0 @ 14003bfd0  size=72  callers=0 callees=0 ================= */

QMenuBar * FUN_14003bfd0(QMenuBar *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QMenuBar *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QMenuBar *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"MenuBar" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003c011. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QMenuBar::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003c020 @ 14003c020  size=150  callers=1 callees=13 ================= */
/* CALLERS: FUN_14003c110 */
/* CALLEES: FUN_14002b7d0, FUN_14002b8d0, FUN_14002b900, FUN_14002b930, FUN_14002b940, FUN_14002ba50, FUN_14002bb30, FUN_14002bc30, FUN_14002be90, FUN_14002c030, FUN_14002c090, FUN_14002c150, FUN_14002c1b0 */

void FUN_14003c020(undefined8 param_1,int param_2,undefined4 param_3,longlong param_4)

{
  if (param_2 == 0) {
    switch(param_3) {
    case 0:
      FUN_14002c1b0(param_1,**(undefined1 **)(param_4 + 8));
      return;
    case 1:
      FUN_14002c030(param_1,**(undefined4 **)(param_4 + 8));
      return;
    case 2:
      FUN_14002c150(param_1,**(undefined1 **)(param_4 + 8));
      return;
    case 3:
      FUN_14002c090(param_1,**(undefined1 **)(param_4 + 8));
      return;
    case 4:
      FUN_14002bc30();
      return;
    case 5:
      FUN_14002bb30();
      return;
    case 6:
      FUN_14002b8d0(param_1,**(undefined4 **)(param_4 + 8));
      return;
    case 7:
      FUN_14002b900();
      return;
    case 8:
      FUN_14002b7d0(param_1,**(undefined4 **)(param_4 + 8));
      return;
    case 9:
      FUN_14002be90(param_1,*(undefined8 *)(param_4 + 8));
      return;
    case 10:
      FUN_14002b940();
      return;
    case 0xb:
      FUN_14002ba50();
      return;
    case 0xc:
      FUN_14002b930();
      return;
    }
  }
  return;
}


/* ================= FUN_14003c0f0 @ 14003c0f0  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003c0f0(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096cb0;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003c110 @ 14003c110  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003c020, qt_metacall */

int FUN_14003c110(QDialog *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QDialog::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 0xd) {
      FUN_14003c020(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -0xd;
  }
  return iVar1;
}


/* ================= FUN_14003c170 @ 14003c170  size=72  callers=0 callees=0 ================= */

QDialog * FUN_14003c170(QDialog *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QDialog *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QDialog *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"NewProjectDialog" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003c1b1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QDialog::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003c1c0 @ 14003c1c0  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003c1c0(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096cd0;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003c1f0 @ 14003c1f0  size=72  callers=0 callees=0 ================= */

QStatusBar * FUN_14003c1f0(QStatusBar *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QStatusBar *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QStatusBar *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"StatusBar" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003c231. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QStatusBar::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003c240 @ 14003c240  size=91  callers=1 callees=5 ================= */
/* CALLERS: FUN_14003c2c0 */
/* CALLEES: FUN_14002b930, FUN_140051590, FUN_1400515f0, FUN_140051690, FUN_140051720 */

void FUN_14003c240(undefined8 param_1,int param_2,int param_3,longlong param_4)

{
  if (param_2 == 0) {
    if (param_3 == 0) {
      FUN_140051690(param_1,**(undefined8 **)(param_4 + 8),**(undefined4 **)(param_4 + 0x10));
      return;
    }
    if (param_3 == 1) {
      FUN_1400515f0(param_1,*(undefined8 *)(param_4 + 8));
      return;
    }
    if (param_3 == 2) {
      FUN_140051590();
      return;
    }
    if (param_3 == 3) {
      FUN_14002b930();
      return;
    }
    if (param_3 == 4) {
      FUN_140051720(param_1,**(undefined8 **)(param_4 + 8),**(undefined4 **)(param_4 + 0x10));
      return;
    }
  }
  return;
}


/* ================= FUN_14003c2a0 @ 14003c2a0  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003c2a0(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096cf0;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003c2c0 @ 14003c2c0  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003c240, qt_metacall */

int FUN_14003c2c0(QDialog *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QDialog::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 5) {
      FUN_14003c240(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -5;
  }
  return iVar1;
}


/* ================= FUN_14003c320 @ 14003c320  size=72  callers=0 callees=0 ================= */

QDialog * FUN_14003c320(QDialog *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QDialog *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QDialog *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"SymbolSelection" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003c361. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QDialog::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003c370 @ 14003c370  size=156  callers=1 callees=15 ================= */
/* CALLERS: FUN_14003c470 */
/* CALLEES: FUN_140053750, FUN_140053760, FUN_140053920, FUN_140053c10, FUN_140053c80, FUN_140053f20, FUN_140053f50, FUN_140054000, FUN_140054030, FUN_140054060, FUN_1400540f0, FUN_140054130, FUN_140054170, FUN_1400541b0, FUN_140054200 */

void FUN_14003c370(undefined8 param_1,int param_2,undefined4 param_3,longlong param_4)

{
  if (param_2 == 0) {
    switch(param_3) {
    case 0:
      FUN_140053920(param_1,*(undefined8 *)(param_4 + 8));
      return;
    case 1:
      FUN_140053c80(param_1,**(undefined8 **)(param_4 + 8));
      return;
    case 2:
      FUN_1400540f0();
      return;
    case 3:
      FUN_140054170();
      return;
    case 4:
      FUN_140054130();
      return;
    case 5:
      FUN_140054200();
      return;
    case 6:
      FUN_1400541b0();
      return;
    case 7:
      FUN_140054060();
      return;
    case 8:
      FUN_140053760();
      return;
    case 9:
      FUN_140053c10();
      return;
    case 10:
      FUN_140053750();
      return;
    case 0xb:
      FUN_140053f20(param_1,**(undefined8 **)(param_4 + 8));
      return;
    case 0xc:
      FUN_140054030();
      return;
    case 0xd:
      FUN_140053f50(param_1,**(undefined8 **)(param_4 + 8));
      return;
    case 0xe:
      FUN_140054000(param_1,**(undefined8 **)(param_4 + 8),**(undefined4 **)(param_4 + 0x10));
      return;
    }
  }
  return;
}


/* ================= FUN_14003c450 @ 14003c450  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003c450(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096d10;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003c470 @ 14003c470  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003c370, qt_metacall */

int FUN_14003c470(QWidget *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QWidget::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 0xf) {
      FUN_14003c370(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -0xf;
  }
  return iVar1;
}


/* ================= FUN_14003c4d0 @ 14003c4d0  size=72  callers=0 callees=0 ================= */

QWidget * FUN_14003c4d0(QWidget *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QWidget *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QWidget *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"TableWidget" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003c511. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QWidget::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003c520 @ 14003c520  size=26  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003c580 */
/* CALLEES: FUN_140037720, FUN_14003c540 */

void FUN_14003c520(undefined8 param_1,int param_2,int param_3)

{
  if (param_2 == 0) {
    if (param_3 == 0) {
      FUN_14003c540();
      return;
    }
    if (param_3 == 1) {
      FUN_140037720();
      return;
    }
  }
  return;
}


/* ================= FUN_14003c540 @ 14003c540  size=20  callers=2 callees=0 ================= */
/* CALLERS: FUN_140037720, FUN_14003c520 */

void FUN_14003c540(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003c54d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096d30,0,(void **)0x0);
  return;
}


/* ================= FUN_14003c560 @ 14003c560  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003c560(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096d30;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003c580 @ 14003c580  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003c520, qt_metacall */

int FUN_14003c580(QComboBox *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QComboBox::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 2) {
      FUN_14003c520(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -2;
  }
  return iVar1;
}


/* ================= FUN_14003c5e0 @ 14003c5e0  size=72  callers=0 callees=0 ================= */

QComboBox * FUN_14003c5e0(QComboBox *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QComboBox *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QComboBox *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"TIFSpeedComboBox" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003c621. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QComboBox::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003c630 @ 14003c630  size=107  callers=1 callees=11 ================= */
/* CALLERS: FUN_14003c6f0 */
/* CALLEES: FUN_140055dc0, FUN_140055e50, FUN_140055e90, FUN_140055ed0, FUN_140055f90, FUN_140055fc0, FUN_140056000, FUN_140056050, FUN_140056070, FUN_1400560d0, thunk_FUN_140056130 */

void FUN_14003c630(undefined8 param_1,int param_2,undefined4 param_3,longlong param_4)

{
  if (param_2 == 0) {
    switch(param_3) {
    case 0:
      FUN_140055e50();
      return;
    case 1:
      FUN_140055e90();
      return;
    case 2:
      thunk_FUN_140056130();
      return;
    case 3:
      FUN_140055f90();
      return;
    case 4:
      FUN_140055fc0();
      return;
    case 5:
      FUN_140056000();
      return;
    case 6:
      FUN_140055ed0();
      return;
    case 7:
      FUN_140055dc0();
      return;
    case 8:
      FUN_1400560d0(param_1,**(undefined4 **)(param_4 + 8));
      return;
    case 9:
      FUN_140056070(param_1,**(undefined4 **)(param_4 + 8));
      return;
    case 10:
      FUN_140056050(param_1,**(undefined4 **)(param_4 + 8));
      return;
    }
  }
  return;
}


/* ================= FUN_14003c6d0 @ 14003c6d0  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003c6d0(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096d50;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003c6f0 @ 14003c6f0  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003c630, qt_metacall */

int FUN_14003c6f0(QToolBar *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QToolBar::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 0xb) {
      FUN_14003c630(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -0xb;
  }
  return iVar1;
}


/* ================= FUN_14003c750 @ 14003c750  size=72  callers=0 callees=0 ================= */

QToolBar * FUN_14003c750(QToolBar *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QToolBar *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QToolBar *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"ToolBar" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003c791. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QToolBar::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003c7a0 @ 14003c7a0  size=90  callers=1 callees=5 ================= */
/* CALLERS: FUN_14003c820 */
/* CALLEES: FUN_1400570b0, FUN_140057120, FUN_140057190, FUN_140057200, thunk_FUN_140056da0 */

void FUN_14003c7a0(undefined8 param_1,int param_2,int param_3,longlong param_4)

{
  if (param_2 == 0) {
    if (param_3 == 0) {
      FUN_140057190(param_1,**(undefined1 **)(param_4 + 8));
      return;
    }
    if (param_3 == 1) {
      FUN_140057120(param_1,**(undefined1 **)(param_4 + 8));
      return;
    }
    if (param_3 == 2) {
      FUN_1400570b0(param_1,**(undefined1 **)(param_4 + 8));
      return;
    }
    if (param_3 == 3) {
      FUN_140057200(param_1,**(undefined4 **)(param_4 + 8));
      return;
    }
    if (param_3 == 4) {
      thunk_FUN_140056da0(param_1,*(undefined8 *)(param_4 + 8));
      return;
    }
  }
  return;
}


/* ================= FUN_14003c800 @ 14003c800  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003c800(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096d70;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003c820 @ 14003c820  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003c7a0, qt_metacall */

int FUN_14003c820(QWidget *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QWidget::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 5) {
      FUN_14003c7a0(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -5;
  }
  return iVar1;
}


/* ================= FUN_14003c880 @ 14003c880  size=72  callers=0 callees=0 ================= */

QWidget * FUN_14003c880(QWidget *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QWidget *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QWidget *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"TriggerWidget" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003c8c1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QWidget::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003c8d0 @ 14003c8d0  size=203  callers=1 callees=8 ================= */
/* CALLERS: FUN_14003ca40 */
/* CALLEES: FUN_140037c60, FUN_140038090, FUN_140038310, FUN_1400383c0, FUN_140038ce0, FUN_14003c9c0, FUN_14003c9e0, FUN_14003ca00 */

void FUN_14003c8d0(longlong *param_1,int param_2,undefined4 param_3,longlong param_4)

{
  if (param_2 == 0) {
    switch(param_3) {
    case 0:
      FUN_14003c9c0(param_1);
      return;
    case 1:
      FUN_14003c9e0(param_1);
      return;
    case 2:
      FUN_14003ca00(param_1);
      return;
    case 3:
                    /* WARNING: Could not recover jumptable at 0x00014003c919. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)(*param_1 + 0x240))(param_1);
      return;
    case 4:
      FUN_14003c9c0(param_1);
      return;
    case 5:
      FUN_14003c9e0(param_1);
      return;
    case 6:
      FUN_14003ca00(param_1);
      return;
    case 7:
      FUN_140038310(param_1,**(undefined4 **)(param_4 + 8),**(undefined4 **)(param_4 + 0x10),
                    **(undefined4 **)(param_4 + 0x18));
      return;
    case 8:
      FUN_140037c60(param_1);
      return;
    case 9:
      FUN_140038090(param_1,*(undefined8 *)(param_4 + 8));
      return;
    case 10:
      FUN_140038ce0(param_1,**(undefined1 **)(param_4 + 8));
      return;
    case 0xb:
      FUN_1400383c0(param_1,**(undefined1 **)(param_4 + 8));
      return;
    }
  }
  return;
}


/* ================= FUN_14003c9c0 @ 14003c9c0  size=20  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003c8d0 */

void FUN_14003c9c0(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003c9cd. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096d90,0,(void **)0x0);
  return;
}


/* ================= FUN_14003c9e0 @ 14003c9e0  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003c8d0 */

void FUN_14003c9e0(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003c9ee. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096d90,1,(void **)0x0);
  return;
}


/* ================= FUN_14003ca00 @ 14003ca00  size=21  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003c8d0 */

void FUN_14003ca00(QObject *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014003ca0e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096d90,2,(void **)0x0);
  return;
}


/* ================= FUN_14003ca20 @ 14003ca20  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003ca20(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096d90;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003ca40 @ 14003ca40  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003c8d0, qt_metacall */

int FUN_14003ca40(QHeaderView *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QHeaderView::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 0xc) {
      FUN_14003c8d0(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -0xc;
  }
  return iVar1;
}


/* ================= FUN_14003caa0 @ 14003caa0  size=72  callers=0 callees=0 ================= */

QHeaderView * FUN_14003caa0(QHeaderView *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QHeaderView *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QHeaderView *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"UtilFilterHeader" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003cae1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QHeaderView::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003caf0 @ 14003caf0  size=89  callers=1 callees=6 ================= */
/* CALLERS: FUN_14003cba0 */
/* CALLEES: FUN_140057820, FUN_1400578b0, FUN_140057a00, FUN_140057a40, FUN_140057a60, thunk_FUN_140057a70 */

void FUN_14003caf0(QObject *param_1,int param_2,undefined4 param_3)

{
  if (param_2 == 0) {
    switch(param_3) {
    case 0:
                    /* WARNING: Could not recover jumptable at 0x00014003cb6d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      QMetaObject::activate(param_1,(QMetaObject *)&DAT_140096db0,0,(void **)0x0);
      return;
    case 1:
      FUN_1400578b0();
      return;
    case 2:
      thunk_FUN_140057a70();
      return;
    case 3:
      FUN_140057820();
      return;
    case 4:
      FUN_140057a60();
      return;
    case 5:
      FUN_140057a00();
      return;
    case 6:
      FUN_140057a40();
      return;
    }
  }
  return;
}


/* ================= FUN_14003cb80 @ 14003cb80  size=23  callers=0 callees=0 ================= */

undefined * FUN_14003cb80(longlong param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  puVar1 = *(undefined **)(*(longlong *)(param_1 + 8) + 0x28);
  puVar2 = &DAT_140096db0;
  if (puVar1 != (undefined *)0x0) {
    puVar2 = puVar1;
  }
  return puVar2;
}


/* ================= FUN_14003cba0 @ 14003cba0  size=91  callers=0 callees=2 ================= */
/* CALLEES: FUN_14003caf0, qt_metacall */

int FUN_14003cba0(QDialog *param_1,Call param_2,int param_3,void **param_4)

{
  int iVar1;
  
  iVar1 = QDialog::qt_metacall(param_1,param_2,param_3,param_4);
  if ((-1 < iVar1) && (param_2 == 0)) {
    if (iVar1 < 7) {
      FUN_14003caf0(param_1,0,iVar1,param_4);
    }
    iVar1 = iVar1 + -7;
  }
  return iVar1;
}


/* ================= FUN_14003cc00 @ 14003cc00  size=72  callers=0 callees=0 ================= */

QDialog * FUN_14003cc00(QDialog *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  QDialog *pQVar4;
  
  if (param_2 == (char *)0x0) {
    return (QDialog *)0x0;
  }
  pcVar3 = param_2;
  do {
    cVar1 = *pcVar3;
    cVar2 = pcVar3[(longlong)"WelcomeDialog" - (longlong)param_2];
    if (cVar1 != cVar2) break;
    pcVar3 = pcVar3 + 1;
  } while (cVar2 != '\0');
  if (cVar1 == cVar2) {
    return param_1;
  }
                    /* WARNING: Could not recover jumptable at 0x00014003cc41. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  pQVar4 = QDialog::qt_metacast(param_1,param_2);
  return pQVar4;
}


/* ================= FUN_14003cc50 @ 14003cc50  size=508  callers=0 callees=17 ================= */
/* CALLEES: FUN_140028380, FUN_14003e990, FUN_14003f120, FUN_14003f1e0, FUN_14003f830, FUN_14003f930, FUN_140040e20, FUN_140041780, FUN_140041cd0, FUN_14004dca0, FUN_14004e150, FUN_14004e4b0, FUN_14004ea50, FUN_14004f7f0, FUN_14004f940, __security_check_cookie, thunk_FUN_14003ea60 */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003cc50(longlong param_1)

{
  undefined8 uVar1;
  longlong *plVar2;
  code *pcVar3;
  int iVar4;
  int iVar5;
  undefined8 uVar6;
  uint uVar7;
  bool bVar8;
  undefined1 auStack_158 [32];
  undefined1 local_138 [272];
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_158;
  uVar1 = *(undefined8 *)(param_1 + 0x10);
  plVar2 = *(longlong **)(*(longlong *)(param_1 + 8) + 0x28);
  pcVar3 = *(code **)(*(longlong *)(param_1 + 8) + 0x20);
  DAT_14285da18 = *(uint *)(plVar2 + 1);
  *(undefined1 *)((longlong)plVar2 + 0x12) = 0;
  iVar4 = FUN_14004f940();
  FUN_14003f930();
  uVar6 = thunk_FUN_14003ea60();
  FUN_140028380(local_138,0x104,"Software\\SEGGER\\%s\\",uVar6);
  FUN_14004e4b0(1,local_138,10);
  FUN_14004e150(DAT_14286d388);
  iVar5 = FUN_140041cd0((int)plVar2[1],*plVar2,2);
  if (0 < iVar5) {
    *(undefined1 *)((longlong)plVar2 + 0x12) = 1;
    goto LAB_14003ce19;
  }
  FUN_14003f830();
  FUN_140041780();
  FUN_14003e990();
  uVar7 = 1;
  if (1 < *(uint *)(plVar2 + 1)) {
    FUN_14003f830();
    if (1 < *(uint *)(plVar2 + 1)) {
      do {
        FUN_14003f1e0("[%d]: %s",uVar7,*(undefined8 *)(*plVar2 + (ulonglong)uVar7 * 8));
        uVar7 = uVar7 + 1;
      } while (uVar7 < *(uint *)(plVar2 + 1));
    }
    FUN_14003e990();
  }
  if (DAT_14285da18 < 2) {
    iVar5 = FUN_140040e20();
    if (iVar5 == 1) {
      *(undefined2 *)(plVar2 + 2) = 0;
      FUN_14004f7f0(local_138,0x103);
      if (*(char *)((longlong)plVar2 + 0x11) == '\0') goto LAB_14003cdc0;
    }
    else {
      if (iVar5 == 2) {
        *(undefined2 *)(plVar2 + 2) = 1;
        goto LAB_14003cdc0;
      }
      *(undefined2 *)(plVar2 + 2) = 0x100;
    }
  }
  else {
    bVar8 = DAT_14285da1e != '\0';
    *(undefined1 *)(plVar2 + 2) = 0;
    *(undefined1 *)((longlong)plVar2 + 0x11) = 0;
    *(uint *)((longlong)plVar2 + 0xc) = -(uint)bVar8 & 2;
LAB_14003cdc0:
    if (((char)plVar2[2] == '\0') && (*(int *)((longlong)plVar2 + 0xc) == 1)) {
      FUN_14004ea50(0);
    }
  }
  if (iVar4 < 0) {
    uVar6 = thunk_FUN_14003ea60();
    FUN_14003f120("Failed to initialize %s",uVar6);
  }
  else {
    iVar4 = FUN_140041cd0((int)plVar2[1],*plVar2,1);
    if (0 < iVar4) {
      *(undefined1 *)((longlong)plVar2 + 0x12) = 1;
    }
    if (DAT_14285da1d != '\0') {
      FUN_14004dca0(DAT_14286d388);
    }
  }
LAB_14003ce19:
  (*pcVar3)(uVar1);
  return;
}


/* ================= FUN_14003ce50 @ 14003ce50  size=79  callers=0 callees=5 ================= */
/* CALLEES: FUN_14003d7d0, FUN_14003eef0, FUN_14003ef10, FUN_14003ef20, FUN_14004e960 */

void FUN_14003ce50(longlong param_1)

{
  char cVar1;
  int iVar2;
  
  iVar2 = FUN_14004e960();
  if (iVar2 < 0) {
    *(undefined4 *)(param_1 + 0x1c) = 0;
    return;
  }
  *(undefined4 *)(param_1 + 0x1c) = 2;
  cVar1 = FUN_14003ef10();
  if (cVar1 != '\0') {
    FUN_14003d7d0(0);
  }
  cVar1 = FUN_14003eef0();
  if (cVar1 != '\0') {
    FUN_14003ef20();
    return;
  }
  return;
}


/* ================= FUN_14003cec0 @ 14003cec0  size=843  callers=0 callees=10 ================= */
/* CALLEES: FUN_14003e880, FUN_14003f830, FUN_14003fa30, FUN_140040980, FUN_14004e0b0, FUN_14004ec40, FUN_14004ed00, FUN_14004ed40, __security_check_cookie, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003cec0(longlong param_1)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  longlong lVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
  undefined8 *puVar6;
  longlong lVar7;
  longlong lVar8;
  undefined1 auStack_618 [32];
  undefined8 local_5f8;
  undefined8 local_5f0;
  undefined8 local_5e8;
  undefined8 local_5e0;
  undefined8 local_5d8;
  undefined8 uStack_5d0;
  undefined8 local_5c8;
  undefined8 uStack_5c0;
  undefined8 local_5b8;
  undefined8 uStack_5b0;
  undefined8 local_5a8;
  undefined8 uStack_5a0;
  undefined8 local_598;
  undefined8 uStack_590;
  undefined8 local_588;
  undefined8 uStack_580;
  undefined8 local_578;
  undefined8 uStack_570;
  undefined8 local_568;
  undefined8 uStack_560;
  undefined8 local_558 [32];
  undefined1 local_454 [520];
  undefined8 local_24c [32];
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
  undefined8 uStack_c0;
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
  undefined4 local_48;
  undefined4 local_44;
  undefined1 local_40;
  undefined2 local_3f;
  undefined1 local_3d;
  undefined4 local_3c;
  undefined4 local_38;
  char local_34;
  undefined1 local_33;
  char local_32;
  undefined1 local_31;
  undefined4 local_30;
  undefined4 local_2c;
  undefined8 local_28;
  undefined4 local_20;
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_618;
  FUN_14003f830("Init project settings");
  lVar3 = *(longlong *)(param_1 + 8);
  memset(local_454,0,0x208);
  local_c8 = 0;
  uStack_c0 = 0;
  lVar7 = 2;
  local_3f = 0;
  lVar8 = 2;
  local_3d = 0;
  local_31 = 0;
  local_28 = 0;
  local_20 = 0;
  local_b8 = 0;
  uStack_b0 = 0;
  local_a8 = 0;
  uStack_a0 = 0;
  local_98 = 0;
  uStack_90 = 0;
  local_88 = 0;
  uStack_80 = 0;
  local_78 = 0;
  uStack_70 = 0;
  local_68 = 0;
  uStack_60 = 0;
  local_58 = 0;
  uStack_50 = 0;
  local_5d8 = *(undefined8 *)(lVar3 + 0x28);
  uStack_5d0 = *(undefined8 *)(lVar3 + 0x30);
  local_5c8 = *(undefined8 *)(lVar3 + 0x38);
  uStack_5c0 = *(undefined8 *)(lVar3 + 0x40);
  local_5b8 = *(undefined8 *)(lVar3 + 0x48);
  uStack_5b0 = *(undefined8 *)(lVar3 + 0x50);
  local_5a8 = *(undefined8 *)(lVar3 + 0x58);
  uStack_5a0 = *(undefined8 *)(lVar3 + 0x60);
  local_598 = *(undefined8 *)(lVar3 + 0x68);
  uStack_590 = *(undefined8 *)(lVar3 + 0x70);
  local_588 = *(undefined8 *)(lVar3 + 0x78);
  uStack_580 = *(undefined8 *)(lVar3 + 0x80);
  local_578 = *(undefined8 *)(lVar3 + 0x88);
  uStack_570 = *(undefined8 *)(lVar3 + 0x90);
  local_568 = *(undefined8 *)(lVar3 + 0x98);
  uStack_560 = *(undefined8 *)(lVar3 + 0xa0);
  puVar5 = (undefined8 *)(lVar3 + 0xa8);
  puVar6 = local_558;
  do {
    puVar1 = puVar6 + 0x10;
    uVar4 = puVar5[1];
    puVar2 = puVar5 + 0x10;
    *puVar6 = *puVar5;
    puVar6[1] = uVar4;
    uVar4 = puVar5[3];
    puVar6[2] = puVar5[2];
    puVar6[3] = uVar4;
    uVar4 = puVar5[5];
    puVar6[4] = puVar5[4];
    puVar6[5] = uVar4;
    uVar4 = puVar5[7];
    puVar6[6] = puVar5[6];
    puVar6[7] = uVar4;
    uVar4 = puVar5[9];
    puVar6[8] = puVar5[8];
    puVar6[9] = uVar4;
    uVar4 = puVar5[0xb];
    puVar6[10] = puVar5[10];
    puVar6[0xb] = uVar4;
    uVar4 = puVar5[0xd];
    puVar6[0xc] = puVar5[0xc];
    puVar6[0xd] = uVar4;
    uVar4 = puVar5[0xf];
    puVar6[0xe] = puVar5[0xe];
    puVar6[0xf] = uVar4;
    lVar8 = lVar8 + -1;
    puVar5 = puVar2;
    puVar6 = puVar1;
  } while (lVar8 != 0);
  *(undefined4 *)puVar1 = *(undefined4 *)puVar2;
  puVar5 = (undefined8 *)(lVar3 + 0x3b4);
  puVar6 = local_24c;
  do {
    puVar1 = puVar6 + 0x10;
    uVar4 = puVar5[1];
    puVar2 = puVar5 + 0x10;
    *puVar6 = *puVar5;
    puVar6[1] = uVar4;
    uVar4 = puVar5[3];
    puVar6[2] = puVar5[2];
    puVar6[3] = uVar4;
    uVar4 = puVar5[5];
    puVar6[4] = puVar5[4];
    puVar6[5] = uVar4;
    uVar4 = puVar5[7];
    puVar6[6] = puVar5[6];
    puVar6[7] = uVar4;
    uVar4 = puVar5[9];
    puVar6[8] = puVar5[8];
    puVar6[9] = uVar4;
    uVar4 = puVar5[0xb];
    puVar6[10] = puVar5[10];
    puVar6[0xb] = uVar4;
    uVar4 = puVar5[0xd];
    puVar6[0xc] = puVar5[0xc];
    puVar6[0xd] = uVar4;
    uVar4 = puVar5[0xf];
    puVar6[0xe] = puVar5[0xe];
    puVar6[0xf] = uVar4;
    lVar7 = lVar7 + -1;
    puVar5 = puVar2;
    puVar6 = puVar1;
  } while (lVar7 != 0);
  *(undefined4 *)puVar1 = *(undefined4 *)puVar2;
  local_148 = *(undefined8 *)(lVar3 + 0x4b8);
  uStack_140 = *(undefined8 *)(lVar3 + 0x4c0);
  local_138 = *(undefined8 *)(lVar3 + 0x4c8);
  uStack_130 = *(undefined8 *)(lVar3 + 0x4d0);
  local_128 = *(undefined8 *)(lVar3 + 0x4d8);
  uStack_120 = *(undefined8 *)(lVar3 + 0x4e0);
  local_118 = *(undefined8 *)(lVar3 + 0x4e8);
  uStack_110 = *(undefined8 *)(lVar3 + 0x4f0);
  local_108 = *(undefined8 *)(lVar3 + 0x4f8);
  uStack_100 = *(undefined8 *)(lVar3 + 0x500);
  local_f8 = *(undefined8 *)(lVar3 + 0x508);
  uStack_f0 = *(undefined8 *)(lVar3 + 0x510);
  local_e8 = *(undefined4 *)(lVar3 + 0x518);
  uStack_e4 = *(undefined4 *)(lVar3 + 0x51c);
  uStack_e0 = *(undefined4 *)(lVar3 + 0x520);
  uStack_dc = *(undefined4 *)(lVar3 + 0x524);
  local_d8 = *(undefined4 *)(lVar3 + 0x528);
  uStack_d4 = *(undefined4 *)(lVar3 + 0x52c);
  uStack_d0 = *(undefined4 *)(lVar3 + 0x530);
  uStack_cc = *(undefined4 *)(lVar3 + 0x534);
  local_48 = *(undefined4 *)(lVar3 + 0x5b8);
  local_44 = *(undefined4 *)(lVar3 + 0x5bc);
  local_40 = *(undefined1 *)(lVar3 + 0x5c0);
  local_3c = *(undefined4 *)(lVar3 + 0x5c4);
  local_38 = *(undefined4 *)(lVar3 + 0x5c8);
  local_34 = *(char *)(lVar3 + 0x5cc);
  local_33 = *(undefined1 *)(lVar3 + 0x5cd);
  local_32 = *(char *)(lVar3 + 0x5ce);
  local_30 = *(undefined4 *)(lVar3 + 0x5d0);
  local_2c = *(undefined4 *)(lVar3 + 0x5d4);
  FUN_14004ec40(&local_5d8);
  local_5f8 = 0xf424000000101;
  FUN_14004ed40(&local_5f8);
  local_5e0 = 0;
  local_5e8 = 0;
  local_5f0 = 0;
  FUN_14004ed00(&local_5f0);
  FUN_14003fa30();
  FUN_140040980();
  FUN_14003e880();
  if ((local_34 == '\0') || (local_32 == '\x01')) {
    FUN_14004e0b0(DAT_14286d388);
  }
  return;
}


/* ================= FUN_14003d210 @ 14003d210  size=943  callers=0 callees=25 ================= */
/* CALLEES: CloseHandle, FUN_1400028c0, FUN_140002920, FUN_140002950, FUN_140002cf0, FUN_140002da0, FUN_140002f60, FUN_140027140, FUN_140027760, FUN_14003e880, FUN_14003f110, FUN_14003f830, FUN_14003fc40, FUN_14003fee0, FUN_1400404c0, FUN_140040a10, FUN_140040ab0, FUN_140040bd0, FUN_140043660, FUN_140043750, FUN_14004de60, FUN_14004df20, FUN_14004e0e0, FUN_14004e1f0, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003d210(longlong param_1)

{
  uint uVar1;
  int iVar2;
  longlong lVar3;
  undefined8 uVar4;
  longlong lVar5;
  undefined8 *puVar6;
  ulonglong uVar7;
  HANDLE hObject;
  uint uVar8;
  ulonglong uVar10;
  ulonglong uVar11;
  undefined1 auStack_418 [32];
  int local_3f8;
  undefined8 local_3f0;
  ulonglong local_3e8;
  undefined8 local_3e0;
  longlong local_3d8;
  undefined8 local_3c8;
  undefined8 uStack_3c0;
  longlong local_3b8;
  undefined8 uStack_3b0;
  undefined8 local_3a8;
  undefined8 uStack_3a0;
  undefined8 local_398;
  undefined8 uStack_390;
  undefined8 local_388;
  undefined8 uStack_380;
  undefined8 local_378;
  undefined8 uStack_370;
  undefined8 local_368;
  undefined8 uStack_360;
  undefined8 local_358;
  undefined8 uStack_350;
  undefined4 local_348;
  undefined4 uStack_344;
  undefined4 uStack_340;
  undefined4 uStack_33c;
  undefined4 local_338;
  undefined4 uStack_334;
  undefined4 uStack_330;
  undefined4 uStack_32c;
  undefined8 local_328;
  undefined1 local_318 [168];
  undefined8 local_270;
  undefined4 local_268;
  undefined2 local_264;
  undefined1 local_258 [272];
  undefined1 local_148 [272];
  ulonglong local_38;
  ulonglong uVar9;
  
  local_38 = DAT_140096fe8 ^ (ulonglong)auStack_418;
  FUN_14003f830("Parsing elf file");
  uVar7 = 0;
  DAT_14285da08 = 0;
  DAT_14285d9dc = 0;
  FUN_14004df20(DAT_14286d388);
  local_3e8 = DAT_14286d3b0;
  if (DAT_14286d3b0 == 0) {
    local_3e8 = 0;
  }
  else {
    DAT_14286d3b0 = 0;
  }
  lVar5 = *(longlong *)(param_1 + 8);
  local_3d8 = lVar5;
  lVar3 = FUN_140002f60(lVar5 + 0x20,1);
  if (lVar3 == 0) {
    FUN_14003f110("Could not open ELF file!");
  }
  else {
    uVar1 = FUN_140002950(lVar3);
    local_3f8 = 0;
    uVar4 = FUN_14004e1f0(DAT_14286d388,0,"Parsing ELF file ...",uVar1);
    uVar9 = uVar7;
    uVar10 = uVar7;
    uVar11 = uVar7;
    local_3e0 = uVar4;
    if (uVar1 != 0) {
      do {
        local_3f8 = (int)uVar9;
        FUN_14004e1f0(DAT_14286d388,uVar4,0,uVar1);
        lVar5 = FUN_140002920(lVar3,uVar9);
        if (lVar5 != 0) {
          puVar6 = (undefined8 *)FUN_140002da0(local_318,lVar5);
          local_3c8 = *puVar6;
          uStack_3c0 = puVar6[1];
          local_3b8 = puVar6[2];
          uStack_3b0 = puVar6[3];
          local_3a8 = puVar6[4];
          uStack_3a0 = puVar6[5];
          local_398 = puVar6[6];
          uStack_390 = puVar6[7];
          local_388 = puVar6[8];
          uStack_380 = puVar6[9];
          local_378 = puVar6[10];
          uStack_370 = puVar6[0xb];
          local_368 = puVar6[0xc];
          uStack_360 = puVar6[0xd];
          local_358 = puVar6[0xe];
          uStack_350 = puVar6[0xf];
          local_348 = *(undefined4 *)(puVar6 + 0x10);
          uStack_344 = *(undefined4 *)((longlong)puVar6 + 0x84);
          uStack_340 = *(undefined4 *)(puVar6 + 0x11);
          uStack_33c = *(undefined4 *)((longlong)puVar6 + 0x8c);
          local_338 = *(undefined4 *)(puVar6 + 0x12);
          uStack_334 = *(undefined4 *)((longlong)puVar6 + 0x94);
          uStack_330 = *(undefined4 *)(puVar6 + 0x13);
          uStack_32c = *(undefined4 *)((longlong)puVar6 + 0x9c);
          local_328 = puVar6[0x14];
          if (local_3b8 != 0) {
            uVar4 = FUN_140002cf0(lVar5);
            FUN_140027140(local_258,uVar4,0x104);
            iVar2 = FUN_140027760(local_148,local_258);
            FUN_140027140(local_148,local_258,0x104);
            if (iVar2 == 0) {
              FUN_14003fee0(uVar10);
              uVar10 = *(ulonglong *)(uVar10 + 0x3e0);
            }
            else {
              if (uVar11 == 0) {
                uVar7 = FUN_140040ab0(0);
                uVar11 = uVar7;
              }
              else {
                uVar7 = FUN_14003fee0(uVar11);
              }
              FUN_140027140(uVar7 + 0x210,local_258,0x104);
              *(undefined1 *)(uVar7 + 0x340) = 1;
              FUN_14003fc40(uVar7);
              uVar10 = *(ulonglong *)(uVar7 + 0x3d8);
            }
            local_3f0 = 0;
            local_3f8 = 0;
            FUN_1400404c0(lVar3,lVar5,uVar10,uVar7);
            uVar4 = local_3e0;
          }
        }
        uVar8 = (int)uVar9 + 1;
        uVar9 = (ulonglong)uVar8;
        lVar5 = local_3d8;
      } while (uVar8 < uVar1);
    }
    local_3f8 = 0xffffffff;
    FUN_14004e1f0(DAT_14286d388,uVar4,0,uVar1);
    FUN_1400028c0(lVar3);
    DAT_14286d3b0 = uVar11;
    hObject = (HANDLE)FUN_140043750(lVar5 + 0x20,0xd);
    iVar2 = FUN_140043660(hObject,0,&local_270);
    if (iVar2 == 0) {
      CloseHandle(hObject);
      DAT_14285da00 = local_268;
      DAT_14285da04 = local_264;
      DAT_14285d9f8 = local_270;
      if (*(char *)(lVar5 + 0x124) != '\0') {
        FUN_14004de60(DAT_14286d388);
        FUN_140040a10();
        FUN_14003e880();
        DAT_14286d3b8 = DAT_14285da08;
        DAT_14085d1e8 = DAT_14285d9dc;
        DAT_14285da08 = 0;
        FUN_140040bd0(local_3e8);
        FUN_14004e0e0(DAT_14286d388);
      }
    }
  }
  return;
}


/* ================= FUN_14003d5c0 @ 14003d5c0  size=503  callers=0 callees=20 ================= */
/* CALLEES: FUN_1400392d0, FUN_14003d7d0, FUN_14003e050, FUN_14003eee0, FUN_14003ef60, FUN_14003f110, FUN_14003f830, FUN_140040980, FUN_1400427b0, FUN_140042a60, FUN_140043990, FUN_140043d20, FUN_14004e0f0, FUN_14004e120, FUN_14004e9c0, FUN_14004ea20, FUN_14004ed00, JLINKARM_IsHalted, JLINK_RTTERMINAL_Control, memset */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14003d5c0(longlong param_1)

{
  char cVar1;
  int iVar2;
  undefined8 *puVar3;
  bool bVar4;
  
  FUN_14003f830("Read start");
  memset(&DAT_14285d420,0,0x5bc);
  DAT_14285d9e0 = 0;
  uRam000000014285d9e8 = 0;
  _DAT_14285d9f0 = 0;
  FUN_14004e9c0(&DAT_14285d420);
  FUN_14004ea20(&DAT_14285d9e0);
  DAT_14285d1f0 = FUN_140043990();
  _DAT_14285d9f0 = (uint5)_DAT_14285d9f0;
  FUN_14004ed00(&DAT_14285d9e0);
  iVar2 = 0;
  cVar1 = FUN_14003eee0();
  if ((cVar1 == '\0') && (iVar2 = FUN_14003ef60(), iVar2 < 0)) goto LAB_14003d791;
  FUN_140042a60();
  if (DAT_14085d182 == '\0') {
    if (DAT_140096dd4 != 0) goto LAB_14003d689;
  }
  else {
    FUN_140040980();
    DAT_140096dd4 = 1;
    _DAT_14285d410 = 1;
LAB_14003d689:
    FUN_14003e050();
    memset(&DAT_14085d1f0,0,0x2000000);
    DAT_140096dd4 = 0;
  }
  if (DAT_14285d9c4 != '\0') {
    JLINK_RTTERMINAL_Control(0,0);
  }
  if ((DAT_14285d9c6 != '\0') && (DAT_14285d414 != '\0')) {
    iVar2 = FUN_1400427b0();
  }
  if (DAT_14085d183 != '\0') {
    cVar1 = JLINKARM_IsHalted();
    DAT_14285d20c = (uint)(cVar1 == '\0');
  }
  if (-1 < iVar2) {
    while( true ) {
      DAT_14085d181 = 1;
      DAT_140096dd0 = 0;
      DAT_14085d182 = 0;
      FUN_14004e120(DAT_14286d388);
      bVar4 = DAT_14285da10 != (undefined8 *)0x0;
      *(undefined4 *)(param_1 + 0x1c) = 0;
      if (bVar4) {
        return;
      }
      puVar3 = (undefined8 *)FUN_140043d20(0x28,"APP: <ReadPeriodic>");
      DAT_14285da10 = puVar3;
      if (puVar3 != (undefined8 *)0x0) break;
      FUN_14003f110("Insufficient memory!");
    }
    *puVar3 = 0;
    puVar3[1] = 0;
    puVar3[2] = 0;
    puVar3[3] = 0;
    puVar3[4] = 0;
    puVar3[2] = FUN_140041710;
    *(undefined4 *)(puVar3 + 4) = 10;
    FUN_1400392d0(puVar3);
    return;
  }
LAB_14003d791:
  *(int *)(param_1 + 0x1c) = iVar2;
  FUN_14004e0f0(DAT_14286d388);
  FUN_14003d7d0(0);
  return;
}


/* ================= FUN_14003d7d0 @ 14003d7d0  size=341  callers=3 callees=9 ================= */
/* CALLERS: FUN_14003ce50, FUN_14003d5c0, FUN_14003f090 */
/* CALLEES: FUN_1400392f0, FUN_14003ef20, FUN_140043d10, FUN_140043e60, FUN_14004e120, FUN_14004e9c0, JLINK_HSS_Stop, JLINK_RTTERMINAL_Control, memset */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_14003d7d0(longlong param_1)

{
  undefined4 uVar1;
  longlong lVar2;
  longlong lVar3;
  
  lVar3 = 0;
  uVar1 = 0;
  if (param_1 != 0) {
    lVar3 = *(longlong *)(param_1 + 8);
  }
  if (DAT_14285da10 != 0) {
    FUN_1400392f0();
    FUN_140043e60(DAT_14285da10);
    DAT_14285da10 = 0;
  }
  lVar2 = FUN_140043d10(0x5bc);
  FUN_14004e9c0(lVar2);
  if (DAT_14085d183 == '\0') {
    uVar1 = 0xffffffff;
  }
  if (*(char *)(lVar2 + 0x5a4) == '\x01') {
    if (DAT_14085d183 != '\0') {
      uVar1 = JLINK_RTTERMINAL_Control(1);
    }
    if (DAT_14285d200 != 0) {
      FUN_140043e60();
      DAT_14285d200 = 0;
    }
  }
  else if ((*(char *)(lVar2 + 0x5b8) == '\0') && (DAT_14085d180 != '\0')) {
    uVar1 = JLINK_HSS_Stop();
  }
  FUN_140043e60(DAT_14285da70);
  DAT_14285da70 = 0;
  memset(&DAT_14285d420,0,0x5bc);
  DAT_14285d9e0 = 0;
  uRam000000014285d9e8 = 0;
  _DAT_14285d9f0 = 0;
  FUN_14003ef20();
  DAT_14285d20c = 0;
  DAT_14085d180 = 0;
  DAT_14085d181 = 0;
  DAT_140096dd0 = 1;
  DAT_14085d182 = 1;
  if (param_1 != 0) {
    *(undefined4 *)(lVar3 + 0x20) = uVar1;
  }
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_14003d950 @ 14003d950  size=79  callers=0 callees=1 ================= */
/* CALLEES: thunk_FUN_14004ea70 */

void FUN_14003d950(longlong param_1)

{
  undefined8 uVar1;
  longlong lVar2;
  code *pcVar3;
  undefined4 *puVar4;
  undefined4 uVar5;
  
  uVar1 = *(undefined8 *)(param_1 + 0x10);
  lVar2 = *(longlong *)(param_1 + 8);
  pcVar3 = *(code **)(lVar2 + 0x128);
  puVar4 = *(undefined4 **)(lVar2 + 0x130);
  uVar5 = thunk_FUN_14004ea70(lVar2 + 0x20);
  if (puVar4 != (undefined4 *)0x0) {
    *puVar4 = uVar5;
  }
  if (pcVar3 != (code *)0x0) {
    (*pcVar3)(uVar1);
  }
  return;
}


/* ================= FUN_14003d9b0 @ 14003d9b0  size=433  callers=0 callees=13 ================= */
/* CALLEES: CloseHandle, FUN_140028380, FUN_14003e4e0, FUN_14003e880, FUN_14003f4f0, FUN_140043660, FUN_140043750, FUN_140043d10, FUN_140043e60, FUN_14004e9c0, FUN_14004ea40, GetLastError, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003d9b0(void)

{
  int iVar1;
  DWORD DVar2;
  longlong lVar3;
  HANDLE hObject;
  undefined1 auStack_268 [32];
  undefined4 local_248;
  undefined4 local_240;
  longlong local_238;
  int local_230;
  short local_22c;
  undefined1 local_228 [528];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_268;
  lVar3 = FUN_140043d10(0x5bc);
  FUN_14004e9c0(lVar3);
  hObject = (HANDLE)0xffffffffffffffff;
  iVar1 = FUN_14004ea40();
  if ((iVar1 != 0) &&
     (((*(char *)(lVar3 + 0x5a4) != '\x01' || (*(char *)(lVar3 + 0x5a6) != '\0')) &&
      (DAT_14286d3b0 != 0)))) {
    hObject = (HANDLE)FUN_140043750(lVar3 + 0x80,0xd);
    if (hObject == (HANDLE)0xffffffffffffffff) {
      DVar2 = GetLastError();
      if (DVar2 == 2) {
        FUN_140028380(local_228,0x204,
                      "The elf file at\n%s\nwas deleted or moved to a different location.\n",
                      lVar3 + 0x80);
      }
      FUN_140043e60(lVar3);
      DAT_14286d378 = 0;
      return;
    }
    iVar1 = FUN_140043660(hObject,0,&local_238);
    if ((iVar1 == 0) &&
       (((local_238 != DAT_14285d9f8 || (local_230 != DAT_14285da00)) ||
        (local_22c != DAT_14285da04)))) {
      FUN_140028380(local_228,0x204,
                    "The elf file at\n%s\nwas modified.\nShould J-Scope reparse the elf file and try to match the selected symbols?"
                    ,lVar3 + 0x80);
      local_240 = 0xffffffff;
      local_248 = 0x400;
      iVar1 = FUN_14003f4f0(local_228,"Reparse & Match symbols","Reparse & Clear symbols",0);
      if (iVar1 == 1) {
        FUN_14003e880();
      }
      DAT_14286d379 = 1;
      FUN_14003e4e0(lVar3 + 0x80,1);
      FUN_140043e60(lVar3);
      goto LAB_14003db2d;
    }
  }
  FUN_140043e60(lVar3);
  if (hObject == (HANDLE)0xffffffffffffffff) {
    DAT_14286d378 = 0;
    return;
  }
LAB_14003db2d:
  CloseHandle(hObject);
  DAT_14286d378 = 0;
  return;
}


/* ================= FUN_14003db70 @ 14003db70  size=252  callers=0 callees=9 ================= */
/* CALLEES: FUN_14003e880, FUN_14003e990, FUN_14003ef60, FUN_14003f830, FUN_140040980, FUN_1400418d0, FUN_14004e9e0, FUN_14004ec80, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_14003db70(longlong param_1)

{
  longlong lVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  char cVar5;
  undefined1 auStack_628 [32];
  undefined1 local_608 [1452];
  char local_5c;
  char local_5a;
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_628;
  FUN_14003f830("Update project");
  lVar1 = *(longlong *)(param_1 + 8);
  FUN_14004e9e0(local_608);
  cVar5 = *(char *)(lVar1 + 0x5cc);
  if (cVar5 == local_5c) {
    cVar2 = *(char *)(lVar1 + 0x5ce);
    if (cVar2 == local_5a) goto LAB_14003dc05;
  }
  FUN_140040980();
  cVar2 = *(char *)(lVar1 + 0x5ce);
  cVar5 = *(char *)(lVar1 + 0x5cc);
LAB_14003dc05:
  iVar3 = FUN_1400418d0(cVar5,cVar2);
  iVar4 = FUN_1400418d0(local_5c,local_5a);
  if (iVar4 != iVar3) {
    FUN_14003e880();
  }
  FUN_14004ec80(lVar1 + 0x20);
  if (*(char *)(lVar1 + 0x5cc) != '\0') {
    FUN_14003ef60();
  }
  FUN_14003e990();
  return;
}

