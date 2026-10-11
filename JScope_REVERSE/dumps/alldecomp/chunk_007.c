
/* ================= FUN_1400532c0 @ 1400532c0  size=351  callers=2 callees=17 ================= */
/* CALLERS: FUN_140051790, FUN_140054250 */
/* CALLEES: FUN_140058d94, QBrush, QColor, QIcon, QPainter, QPixmap, __security_check_cookie, drawRects, end, fill, height, setBrush, setIcon, setPen, width, ~QBrush, ~QIcon */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_1400532c0(undefined8 param_1,QTableWidgetItem *param_2,QColor *param_3)

{
  int iVar1;
  QPixmap *pQVar2;
  QPainter *this;
  QIcon *pQVar3;
  undefined1 auStack_58 [32];
  QPixmap *local_38;
  int local_30;
  int local_2c;
  ulonglong local_28;
  
  local_28 = DAT_140096fe8 ^ (ulonglong)auStack_58;
  pQVar2 = (QPixmap *)FUN_140058d94(0x18);
  local_38 = pQVar2;
  if (pQVar2 == (QPixmap *)0x0) {
    pQVar2 = (QPixmap *)0x0;
  }
  else {
    QPixmap::QPixmap(pQVar2,0x10,0x10);
    *(undefined ***)pQVar2 = QPixmap::vftable;
  }
  local_38 = (QPixmap *)FUN_140058d94(8);
  if (local_38 == (QPixmap *)0x0) {
    this = (QPainter *)0x0;
  }
  else {
    this = (QPainter *)QPainter::QPainter((QPainter *)local_38,(QPaintDevice *)pQVar2);
  }
  QColor::QColor((QColor *)&local_38,0x13);
  QPixmap::fill(pQVar2,(QColor *)&local_38);
  QBrush::QBrush((QBrush *)&local_38,param_3,1);
  QPainter::setBrush(this,(QBrush *)&local_38);
  QBrush::~QBrush((QBrush *)&local_38);
  QPainter::setPen(this,0);
  iVar1 = QPixmap::height(pQVar2);
  local_30 = QPixmap::width(pQVar2);
  local_38 = (QPixmap *)0x0;
  local_30 = local_30 + -1;
  local_2c = iVar1 + -1;
  QPainter::drawRects(this,(QRect *)&local_38,1);
  QPainter::end(this);
  pQVar3 = (QIcon *)QIcon::QIcon((QIcon *)&local_38,pQVar2);
  QTableWidgetItem::setIcon(param_2,pQVar3);
  QIcon::~QIcon((QIcon *)&local_38);
  return;
}


/* ================= FUN_140053420 @ 140053420  size=548  callers=1 callees=5 ================= */
/* CALLERS: FUN_140054250 */
/* CALLEES: FUN_14003eaa0, FUN_14003ece0, FUN_140049710, QVariant, ~QVariant */

void FUN_140053420(undefined8 param_1,longlong *param_2,longlong param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  double *pdVar3;
  QVariant local_18 [16];
  
  iVar1 = FUN_140049710(DAT_14286d3a0,param_4);
  if (iVar1 == -1) {
    QVariant::QVariant(local_18,"N/A");
    (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
  }
  else {
    pdVar3 = (double *)FUN_14003ece0(param_3,iVar1);
    switch(*(undefined4 *)(param_3 + 0x314)) {
    case 1:
      QVariant::QVariant(local_18,*(float *)pdVar3);
      (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
      break;
    case 2:
      QVariant::QVariant(local_18,*pdVar3);
      (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
      break;
    default:
      goto switchD_1400534a3_caseD_3;
    case 5:
      if (*(char *)pdVar3 == '\0') {
        QVariant::QVariant(local_18,"false");
        (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
      }
      else {
        QVariant::QVariant(local_18,"true");
        (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
      }
      break;
    case 6:
      QVariant::QVariant(local_18,(uint)*(byte *)pdVar3);
      (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
      break;
    case 7:
      QVariant::QVariant(local_18,(uint)*(ushort *)pdVar3);
      (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
      break;
    case 8:
      QVariant::QVariant(local_18,(uint)*(float *)pdVar3);
      (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
      break;
    case 10:
      QVariant::QVariant(local_18,(int)*(char *)pdVar3);
      (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
      break;
    case 0xb:
      QVariant::QVariant(local_18,(int)*(short *)pdVar3);
      (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
      break;
    case 0xc:
      QVariant::QVariant(local_18,(int)*(float *)pdVar3);
      (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
      break;
    case 0xe:
      uVar2 = FUN_14003eaa0(*(float *)pdVar3,param_3);
      QVariant::QVariant(local_18,uVar2);
      (**(code **)(*param_2 + 0x18))(param_2,0,local_18);
    }
  }
  QVariant::~QVariant(local_18);
switchD_1400534a3_caseD_3:
  return;
}


/* ================= FUN_140053680 @ 140053680  size=194  callers=3 callees=4 ================= */
/* CALLERS: FUN_1400521c0, FUN_140053c80, FUN_140054250 */
/* CALLEES: FUN_140028380, QVariant, __security_check_cookie, ~QVariant */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140053680(undefined8 param_1,longlong *param_2,uint param_3)

{
  char *pcVar1;
  undefined1 auStack_148 [32];
  QVariant local_128 [16];
  char local_118 [256];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_148;
  if (param_3 < 1000000) {
    if (param_3 < 1000) {
      pcVar1 = "%i/div";
    }
    else {
      pcVar1 = "%ik/div";
      param_3 = param_3 / 1000;
    }
  }
  else {
    pcVar1 = "%iM/div";
    param_3 = param_3 / 1000000;
  }
  FUN_140028380(local_118,0x100,pcVar1,param_3);
  QVariant::QVariant(local_128,local_118);
  (**(code **)(*param_2 + 0x18))(param_2,0,local_128);
  QVariant::~QVariant(local_128);
  return;
}


/* ================= FUN_140053750 @ 140053750  size=12  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_14004e0b0 */

void FUN_140053750(void)

{
  FUN_14004e0b0(DAT_14286d388);
  return;
}


/* ================= FUN_140053760 @ 140053760  size=440  callers=1 callees=17 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_140058d94, QBrush, QColorDialog, QPalette, __security_check_cookie, color, exec, free, invalidate, operator=, palette, selectedColor, sender, setBrush, setPalette, ~QBrush, ~QPalette */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140053760(QObject *param_1)

{
  longlong lVar1;
  longlong *plVar2;
  int iVar3;
  QObject *this;
  QColorDialog *pQVar4;
  QPalette *pQVar5;
  QColor *pQVar6;
  QBrush *pQVar7;
  undefined1 auStack_68 [32];
  QPalette local_48 [16];
  undefined4 local_38;
  undefined8 local_34;
  undefined2 local_2c;
  QColorDialog *local_28 [2];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_68;
  QColor::invalidate((QColor *)&local_38);
  QPalette::QPalette(local_48);
  this = QObject::sender(param_1);
  pQVar4 = (QColorDialog *)FUN_140058d94(0x28);
  local_28[0] = pQVar4;
  if (pQVar4 == (QColorDialog *)0x0) {
    pQVar4 = (QColorDialog *)0x0;
  }
  else {
    pQVar5 = QWidget::palette((QWidget *)this);
    pQVar6 = QPalette::color(pQVar5,1);
    QColorDialog::QColorDialog(pQVar4,pQVar6,(QWidget *)param_1);
    *(undefined ***)pQVar4 = QColorDialog::vftable;
    *(undefined ***)(pQVar4 + 0x10) = QColorDialog::vftable;
  }
  iVar3 = QDialog::exec((QDialog *)pQVar4);
  if (iVar3 != 0) {
    pQVar6 = (QColor *)QColorDialog::selectedColor(pQVar4);
    QColor::operator=((QColor *)&local_38,pQVar6);
    pQVar5 = QWidget::palette((QWidget *)this);
    QPalette::operator=(local_48,pQVar5);
    pQVar7 = (QBrush *)QBrush::QBrush((QBrush *)local_28,(QColor *)&local_38,1);
    QPalette::setBrush(local_48,5,1,pQVar7);
    QBrush::~QBrush((QBrush *)local_28);
    QWidget::setPalette((QWidget *)this,local_48);
    for (plVar2 = DAT_14286d3b8; plVar2 != (longlong *)0x0; plVar2 = (longlong *)plVar2[1]) {
      lVar1 = *plVar2;
      if (*(QObject **)(lVar1 + 0x370) == this) {
        free(*(void **)(lVar1 + 0x378));
        local_28[0] = (QColorDialog *)FUN_140058d94(0x10);
        if (local_28[0] != (QColorDialog *)0x0) {
          *(undefined4 *)local_28[0] = local_38;
          *(undefined8 *)(local_28[0] + 4) = local_34;
          *(undefined2 *)(local_28[0] + 0xc) = local_2c;
        }
        *(QColorDialog **)(lVar1 + 0x378) = local_28[0];
      }
    }
  }
  QPalette::~QPalette(local_48);
  return;
}


/* ================= FUN_140053920 @ 140053920  size=751  callers=1 callees=16 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_140036150, FUN_140036210, FUN_14003ec10, FUN_14003ef10, FUN_140043d10, FUN_140043e60, FUN_14004e9c0, FUN_140052ea0, exec, mapFrom, mapToGlobal, setCheckable, setChecked, setEnabled, viewport, ~QModelIndex */

void FUN_140053920(QWidget *param_1)

{
  char cVar1;
  longlong lVar2;
  QWidget *this;
  undefined8 *puVar3;
  longlong lVar4;
  QMenu *this_00;
  undefined8 uVar5;
  QPoint *pQVar6;
  undefined8 local_res8;
  QWidget local_res18 [8];
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 uStack_38;
  QModelIndex local_30 [24];
  
  local_48 = 0xffffffffffffffff;
  uStack_40 = 0;
  uStack_38 = 0;
  lVar2 = FUN_140043d10(0x5bc);
  FUN_14004e9c0(lVar2);
  this = QAbstractScrollArea::viewport(*(QAbstractScrollArea **)(param_1 + 0x28));
  puVar3 = (undefined8 *)QWidget::mapFrom(this,local_res18,(QPoint *)param_1);
  local_res8 = *puVar3;
  puVar3 = (undefined8 *)
           (**(code **)(**(longlong **)(param_1 + 0x28) + 0x1c8))
                     (*(longlong **)(param_1 + 0x28),local_30,&local_res8);
  local_48 = *puVar3;
  uStack_40 = puVar3[1];
  uStack_38 = puVar3[2];
  QModelIndex::~QModelIndex(local_30);
  if (-1 < (int)local_48) {
    lVar4 = FUN_140052ea0(param_1);
    this_00 = (QMenu *)FUN_140036150(param_1,0,0);
    if (lVar4 != 0) {
      uVar5 = FUN_140036210(param_1,this_00,&DAT_1400756f0,0,param_1,
                            "1_SlotOnReleasedShowHide(void)");
      *(undefined8 *)(param_1 + 0x68) = uVar5;
      uVar5 = FUN_140036210(param_1,this_00,"Y-Offset up",0,param_1,
                            "1_SlotOnReleasedYOffsetUp(void)");
      *(undefined8 *)(param_1 + 0x70) = uVar5;
      uVar5 = FUN_140036210(param_1,this_00,"Y-Offset down",0,param_1,
                            "1_SlotOnReleasedYOffsetDown(void)");
      *(undefined8 *)(param_1 + 0x78) = uVar5;
      uVar5 = FUN_140036210(param_1,this_00,"Y-Resolution up",0,param_1,
                            "1_SlotOnReleasedYResUp(void)");
      *(undefined8 *)(param_1 + 0x80) = uVar5;
      uVar5 = FUN_140036210(param_1,this_00,"Y-Resolution down",0,param_1,
                            "1_SlotOnReleasedYResDown(void)");
      *(undefined8 *)(param_1 + 0x88) = uVar5;
      uVar5 = FUN_140036210(param_1,this_00,"Autoscale Y-Res",0,param_1,
                            "1_SlotOnReleasedAutoScale(void)");
      *(undefined8 *)(param_1 + 0x90) = uVar5;
      if ((*(char *)(lVar2 + 0x5a4) == '\0') || (*(char *)(lVar2 + 0x5a6) == '\x01')) {
        uVar5 = FUN_140036210(param_1,this_00,"Remove Symbol",0,param_1,
                              "1_SlotOnReleasedRemoveSymbol(void)");
        *(undefined8 *)(param_1 + 0x98) = uVar5;
      }
      QAction::setCheckable(*(QAction **)(param_1 + 0x68),true);
      QAction::setChecked(*(QAction **)(param_1 + 0x68),*(char *)(lVar4 + 0x35e) != '\0');
      if ((*(char *)(lVar2 + 0x5a4) == '\0') && (cVar1 = FUN_14003ec10(), cVar1 != '\0')) {
        QAction::setEnabled(*(QAction **)(param_1 + 0x98),false);
      }
      QAction::setCheckable(*(QAction **)(param_1 + 0x90),true);
      QAction::setChecked(*(QAction **)(param_1 + 0x90),*(char *)(lVar4 + 0x35c) == '\0');
      if ((*(char *)(lVar2 + 0x5a6) == '\x01') && (cVar1 = FUN_14003ef10(), cVar1 != '\0')) {
        QAction::setEnabled(*(QAction **)(param_1 + 0x98),false);
      }
    }
    pQVar6 = (QPoint *)QWidget::mapToGlobal(param_1,(QPoint *)local_res18);
    QMenu::exec(this_00,pQVar6,(QAction *)0x0);
    FUN_140043e60(lVar2);
  }
  QModelIndex::~QModelIndex((QModelIndex *)&local_48);
  return;
}


/* ================= FUN_140053c10 @ 140053c10  size=101  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_140052fd0, cellWidget, currentIndex */

void FUN_140053c10(longlong param_1)

{
  longlong *plVar1;
  int iVar2;
  QWidget *this;
  
  for (plVar1 = DAT_14286d3b8; plVar1 != (longlong *)0x0; plVar1 = (longlong *)plVar1[1]) {
    if (*plVar1 != 0) {
      iVar2 = FUN_140052fd0();
      this = QTableWidget::cellWidget(*(QTableWidget **)(param_1 + 0x28),iVar2,2);
      iVar2 = QComboBox::currentIndex((QComboBox *)this);
      *(int *)(*plVar1 + 0x348) = iVar2;
    }
  }
  return;
}


/* ================= FUN_140053c80 @ 140053c80  size=659  callers=1 callees=16 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_14004e120, FUN_140052f70, FUN_140053680, QRegExp, checkState, column, exactMatch, fromAscii_helper, number, row, setText, text, toDouble, toInt, ~QRegExp, ~QString */

void FUN_140053c80(longlong param_1,QTableWidgetItem *param_2)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  CheckState CVar4;
  longlong lVar5;
  QString *pQVar6;
  char cVar7;
  undefined8 uVar8;
  double extraout_XMM0_Qa;
  double dVar9;
  double dVar10;
  undefined4 local_res8 [4];
  QRegExp local_res18 [8];
  Data *local_res20;
  QString local_38 [16];
  
  local_res8[0] = 0;
  iVar2 = -1;
  local_res20 = QString::fromAscii_helper("[+-]?\\d*",-1);
  uVar8 = 0;
  QRegExp::QRegExp(local_res18,(QString *)&local_res20,1,0);
  QString::~QString((QString *)&local_res20);
  if (*(char *)(param_1 + 0xa0) != '\0') {
    if (*(QTableWidget **)(param_2 + 0x18) != (QTableWidget *)0x0) {
      iVar2 = QTableWidget::row(*(QTableWidget **)(param_2 + 0x18),param_2);
    }
    lVar5 = FUN_140052f70(iVar2);
    if ((lVar5 != 0) && (*(QTableWidget **)(param_2 + 0x18) != (QTableWidget *)0x0)) {
      iVar2 = QTableWidget::column(*(QTableWidget **)(param_2 + 0x18),param_2);
      if (iVar2 == 0) {
        CVar4 = QTableWidgetItem::checkState(param_2);
        *(bool *)(lVar5 + 0x35e) = CVar4 == 2;
      }
      else if (iVar2 == 0xe) {
        pQVar6 = (QString *)QTableWidgetItem::text(param_2);
        uVar3 = QString::toInt(pQVar6,(bool *)0x0,10);
        QString::~QString((QString *)local_res8);
        if ((*(uint *)(lVar5 + 0x358) == uVar3) || (uVar3 == 0)) goto LAB_140053efd;
        pQVar6 = (QString *)QTableWidgetItem::text(param_2);
        local_res8[0] = 1;
        bVar1 = QRegExp::exactMatch(local_res18,pQVar6);
        if ((bVar1) && (uVar3 < 2000000000)) {
          bVar1 = true;
        }
        else {
          bVar1 = false;
        }
        QString::~QString(local_38);
        if (bVar1) {
          *(uint *)(lVar5 + 0x358) = uVar3;
          *(undefined1 *)(lVar5 + 0x35c) = 1;
          FUN_140053680(param_1,param_2,uVar3);
        }
        else {
          FUN_140053680(param_1,param_2,*(undefined4 *)(lVar5 + 0x358));
        }
      }
      else {
        if (iVar2 != 0xf) goto LAB_140053efd;
        pQVar6 = (QString *)QTableWidgetItem::text(param_2);
        bVar1 = QRegExp::exactMatch(local_res18,pQVar6);
        cVar7 = (char)pQVar6;
        QString::~QString((QString *)local_res8);
        if (bVar1) {
          pQVar6 = (QString *)QTableWidgetItem::text(param_2);
          dVar9 = QString::toDouble(pQVar6,(bool *)0x0);
          dVar10 = *(double *)(lVar5 + 0x350);
          QString::~QString((QString *)local_res8);
          if (0.01 <= ABS(dVar10 - dVar9)) {
            pQVar6 = (QString *)QTableWidgetItem::text(param_2);
            dVar10 = QString::toDouble(pQVar6,(bool *)0x0);
            *(double *)(lVar5 + 0x350) = dVar10;
            QString::~QString((QString *)local_res8);
          }
        }
        else {
          pQVar6 = (QString *)
                   QString::number(extraout_XMM0_Qa,cVar7,
                                   (int)CONCAT71((int7)((ulonglong)uVar8 >> 8),0x67));
          QTableWidgetItem::setText(param_2,pQVar6);
          QString::~QString((QString *)local_res8);
        }
      }
      FUN_14004e120(DAT_14286d388);
    }
  }
LAB_140053efd:
  QRegExp::~QRegExp(local_res18);
  return;
}


/* ================= FUN_140053f20 @ 140053f20  size=33  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_14003ee00, FUN_14004e120, FUN_140052ea0 */

void FUN_140053f20(void)

{
  undefined8 uVar1;
  
  uVar1 = FUN_140052ea0();
  FUN_14003ee00(uVar1);
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_140053f50 @ 140053f50  size=168  callers=1 callees=7 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_140052f70, QVariant, column, number, row, ~QString, ~QVariant */

void FUN_140053f50(undefined8 param_1,QTableWidgetItem *param_2)

{
  int iVar1;
  longlong lVar2;
  QString *pQVar3;
  QString local_res10 [24];
  QVariant local_18 [16];
  
  if (*(QTableWidget **)(param_2 + 0x18) == (QTableWidget *)0x0) {
    iVar1 = -1;
  }
  else {
    iVar1 = QTableWidget::row(*(QTableWidget **)(param_2 + 0x18),param_2);
  }
  lVar2 = FUN_140052f70(iVar1);
  if ((lVar2 != 0) && (*(QTableWidget **)(param_2 + 0x18) != (QTableWidget *)0x0)) {
    iVar1 = QTableWidget::column(*(QTableWidget **)(param_2 + 0x18),param_2);
    if (iVar1 == 0xe) {
      *(undefined1 *)(lVar2 + 0x381) = 1;
      pQVar3 = (QString *)QString::number((uint)local_res10,*(int *)(lVar2 + 0x358));
      QVariant::QVariant(local_18,pQVar3);
      (**(code **)(*(longlong *)param_2 + 0x18))(param_2,0,local_18);
      QVariant::~QVariant(local_18);
      QString::~QString(local_res10);
    }
  }
  return;
}


/* ================= FUN_140054000 @ 140054000  size=36  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003c370 */

void FUN_140054000(void)

{
  longlong *plVar1;
  
  for (plVar1 = DAT_14286d3b8; plVar1 != (longlong *)0x0; plVar1 = (longlong *)plVar1[1]) {
    *(undefined1 *)(*plVar1 + 0x381) = 0;
  }
  return;
}


/* ================= FUN_140054030 @ 140054030  size=35  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_140052ea0 */

void FUN_140054030(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140052ea0();
  if (lVar1 != 0) {
    *(bool *)(lVar1 + 0x35c) = *(char *)(lVar1 + 0x35c) == '\0';
  }
  return;
}


/* ================= FUN_140054060 @ 140054060  size=139  callers=1 callees=8 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_14003ec10, FUN_14003ef10, FUN_14003f0a0, FUN_140043d10, FUN_140043e60, FUN_14004e120, FUN_14004e9c0, FUN_140052ea0 */

void FUN_140054060(undefined8 param_1)

{
  char cVar1;
  longlong lVar2;
  longlong lVar3;
  
  lVar2 = FUN_140043d10(0x5bc);
  FUN_14004e9c0(lVar2);
  if (*(char *)(lVar2 + 0x5a4) == '\0') {
    cVar1 = FUN_14003ec10();
    if (cVar1 != '\0') goto LAB_1400540d9;
  }
  if ((*(char *)(lVar2 + 0x5a4) != '\x01') || (*(char *)(lVar2 + 0x5a6) != '\0')) {
    if (*(char *)(lVar2 + 0x5a6) == '\x01') {
      cVar1 = FUN_14003ef10();
      if (cVar1 != '\0') goto LAB_1400540d9;
    }
    lVar3 = FUN_140052ea0(param_1);
    if (lVar3 != 0) {
      FUN_14003f0a0(lVar3);
      FUN_14004e120(DAT_14286d388);
    }
  }
LAB_1400540d9:
  FUN_140043e60(lVar2);
  return;
}


/* ================= FUN_1400540f0 @ 1400540f0  size=51  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_14004e120, FUN_140052ea0 */

void FUN_1400540f0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140052ea0();
  if (lVar1 != 0) {
    *(bool *)(lVar1 + 0x35e) = *(char *)(lVar1 + 0x35e) == '\0';
    FUN_14004e120(DAT_14286d388);
    return;
  }
  return;
}


/* ================= FUN_140054130 @ 140054130  size=59  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_14004e0f0, FUN_140052ea0 */

void FUN_140054130(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140052ea0();
  if (lVar1 != 0) {
    *(double *)(lVar1 + 0x350) = *(double *)(lVar1 + 0x350) - 0.5;
    FUN_14004e0f0(DAT_14286d388);
    return;
  }
  return;
}


/* ================= FUN_140054170 @ 140054170  size=59  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_14004e0f0, FUN_140052ea0 */

void FUN_140054170(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140052ea0();
  if (lVar1 != 0) {
    *(double *)(lVar1 + 0x350) = *(double *)(lVar1 + 0x350) + 0.5;
    FUN_14004e0f0(DAT_14286d388);
    return;
  }
  return;
}


/* ================= FUN_1400541b0 @ 1400541b0  size=67  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_14003e8e0, FUN_14004e0f0, FUN_140052ea0 */

void FUN_1400541b0(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140052ea0();
  if (lVar1 != 0) {
    if (*(char *)(lVar1 + 0x35c) == '\0') {
      *(undefined1 *)(lVar1 + 0x35c) = 1;
    }
    FUN_14003e8e0(lVar1,*(int *)(lVar1 + 0x358) + -1);
    FUN_14004e0f0(DAT_14286d388);
    return;
  }
  return;
}


/* ================= FUN_140054200 @ 140054200  size=67  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c370 */
/* CALLEES: FUN_14003ee40, FUN_14004e0f0, FUN_140052ea0 */

void FUN_140054200(void)

{
  longlong lVar1;
  
  lVar1 = FUN_140052ea0();
  if (lVar1 != 0) {
    if (*(char *)(lVar1 + 0x35c) == '\0') {
      *(undefined1 *)(lVar1 + 0x35c) = 1;
    }
    FUN_14003ee40(lVar1,*(int *)(lVar1 + 0x358) + 1);
    FUN_14004e0f0(DAT_14286d388);
    return;
  }
  return;
}


/* ================= FUN_140054250 @ 140054250  size=1337  callers=1 callees=24 ================= */
/* CALLERS: FUN_140052110 */
/* CALLEES: FUN_14003ec10, FUN_14003ef10, FUN_140043d10, FUN_140043e60, FUN_1400497c0, FUN_140049910, FUN_14004e9c0, FUN_140052fd0, FUN_1400532c0, FUN_140053420, FUN_140053680, QPalette, QVariant, hideColumn, item, number, setCheckState, setDisabled, setPalette, setText, showColumn, ~QPalette, ~QString, ~QVariant */

void FUN_140054250(longlong param_1)

{
  double dVar1;
  QWidget *this;
  longlong *plVar2;
  undefined8 uVar3;
  char cVar4;
  int iVar5;
  QTableWidgetItem *pQVar6;
  QPalette *pQVar7;
  QString *pQVar8;
  undefined8 uVar9;
  longlong lVar10;
  QVariant *pQVar11;
  CheckState CVar12;
  QString local_res10 [8];
  QVariant local_e8 [16];
  QVariant local_d8 [16];
  QVariant local_c8 [16];
  QVariant local_b8 [16];
  QVariant local_a8 [16];
  QVariant local_98 [16];
  QVariant local_88 [16];
  QVariant local_78 [16];
  QVariant local_68 [16];
  QPalette local_58 [48];
  
  uVar3 = DAT_14286d3a0;
  for (plVar2 = DAT_14286d3b8; plVar2 != (longlong *)0x0; plVar2 = (longlong *)plVar2[1]) {
    lVar10 = *plVar2;
    iVar5 = FUN_140052fd0(lVar10);
    cVar4 = (char)iVar5;
    if (-1 < iVar5) {
      pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,0);
      if (*(char *)(lVar10 + 0x35e) == '\0') {
        CVar12 = 0;
      }
      else {
        CVar12 = 2;
      }
      QTableWidgetItem::setCheckState(pQVar6,CVar12);
      if (*(QColor **)(lVar10 + 0x378) != (QColor *)0x0) {
        this = *(QWidget **)(lVar10 + 0x370);
        pQVar7 = (QPalette *)QPalette::QPalette(local_58,*(QColor **)(lVar10 + 0x378));
        QWidget::setPalette(this,pQVar7);
        QPalette::~QPalette(local_58);
      }
      if (*(char *)(lVar10 + 0x381) == '\0') {
        pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,0xe);
        FUN_140053680(param_1,pQVar6,*(undefined4 *)(lVar10 + 0x358));
      }
      uVar9 = 0;
      pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,0xf);
      dVar1 = *(double *)(lVar10 + 0x350);
      if ((-0.01 < dVar1) && (dVar1 < 0.01)) {
        *(undefined8 *)(lVar10 + 0x350) = 0;
      }
      pQVar8 = (QString *)
               QString::number(dVar1,cVar4,(int)CONCAT71((int7)((ulonglong)uVar9 >> 8),0x67));
      QTableWidgetItem::setText(pQVar6,pQVar8);
      QString::~QString(local_res10);
      pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,10);
      if (*(char *)(lVar10 + 0x3c4) == '\0') {
        QVariant::QVariant(local_c8,"N/A");
        (**(code **)(*(longlong *)pQVar6 + 0x18))(pQVar6,0,local_c8);
        pQVar11 = local_c8;
      }
      else if (*(int *)(lVar10 + 0x3c0) == 2) {
        QVariant::QVariant(local_e8,*(double *)(lVar10 + 0x3b8));
        (**(code **)(*(longlong *)pQVar6 + 0x18))(pQVar6,0,local_e8);
        pQVar11 = local_e8;
      }
      else {
        QVariant::QVariant(local_d8,*(__int64 *)(lVar10 + 0x3b8));
        (**(code **)(*(longlong *)pQVar6 + 0x18))(pQVar6,0,local_d8);
        pQVar11 = local_d8;
      }
      QVariant::~QVariant(pQVar11);
      cVar4 = FUN_140049910(uVar3,1);
      if (cVar4 == '\0') {
        QTableView::hideColumn(*(QTableView **)(param_1 + 0x28),8);
      }
      else {
        QTableView::showColumn(*(QTableView **)(param_1 + 0x28),8);
        uVar9 = FUN_1400497c0(uVar3,1);
        FUN_1400532c0(param_1,*(undefined8 *)(param_1 + 0xa8),uVar9);
      }
      cVar4 = FUN_140049910(uVar3,2);
      if (cVar4 == '\0') {
        QTableView::hideColumn(*(QTableView **)(param_1 + 0x28),9);
      }
      else {
        QTableView::showColumn(*(QTableView **)(param_1 + 0x28),9);
        uVar9 = FUN_1400497c0(uVar3,2);
        FUN_1400532c0(param_1,*(undefined8 *)(param_1 + 0xb0),uVar9);
      }
      if (*(char *)(lVar10 + 0x388) == '\0') {
        pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,0xb);
        QVariant::QVariant(local_b8,"N/A");
        (**(code **)(*(longlong *)pQVar6 + 0x18))(pQVar6,0,local_b8);
        QVariant::~QVariant(local_b8);
        pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,0xc);
        QVariant::QVariant(local_a8,"N/A");
        (**(code **)(*(longlong *)pQVar6 + 0x18))(pQVar6,0,local_a8);
        QVariant::~QVariant(local_a8);
        pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,0xd);
        QVariant::QVariant(local_98,"N/A");
        (**(code **)(*(longlong *)pQVar6 + 0x18))(pQVar6,0);
        pQVar11 = local_98;
      }
      else {
        pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,7);
        FUN_140053420(param_1,pQVar6,lVar10,0);
        pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,8);
        FUN_140053420(param_1,pQVar6,lVar10,1);
        pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,9);
        FUN_140053420(param_1,pQVar6,lVar10,2);
        pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,0xb);
        QVariant::QVariant(local_88,*(double *)(lVar10 + 0x3a0));
        (**(code **)(*(longlong *)pQVar6 + 0x18))(pQVar6,0,local_88);
        QVariant::~QVariant(local_88);
        pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,0xc);
        QVariant::QVariant(local_78,*(double *)(lVar10 + 0x398));
        (**(code **)(*(longlong *)pQVar6 + 0x18))(pQVar6,0,local_78);
        QVariant::~QVariant(local_78);
        pQVar6 = QTableWidget::item(*(QTableWidget **)(param_1 + 0x28),iVar5,0xd);
        QVariant::QVariant(local_68,*(double *)(lVar10 + 0x3a8));
        (**(code **)(*(longlong *)pQVar6 + 0x18))(pQVar6,0);
        pQVar11 = local_68;
      }
      QVariant::~QVariant(pQVar11);
    }
  }
  lVar10 = FUN_140043d10(0x5bc);
  FUN_14004e9c0(lVar10);
  if (*(char *)(lVar10 + 0x5a4) == '\0') {
    cVar4 = FUN_14003ec10();
    QWidget::setDisabled(*(QWidget **)(param_1 + 0x30),cVar4 != '\0');
  }
  if (*(char *)(lVar10 + 0x5a6) == '\x01') {
    cVar4 = FUN_14003ef10();
    QWidget::setDisabled(*(QWidget **)(param_1 + 0x30),cVar4 != '\0');
  }
  FUN_140043e60(lVar10);
  return;
}


/* ================= FUN_140054790 @ 140054790  size=2993  callers=1 callees=40 ================= */
/* CALLERS: FUN_14004c6a0 */
/* CALLEES: FUN_140028380, FUN_140035d30, FUN_14003f420, FUN_140055480, FUN_140058d94, QAction, QComboBox, QIcon, QKeySequence, QLabel, QMenu, QString, QToolBar, QToolButton, QVariant, QWidget, __security_check_cookie, addAction, addSeparator, addWidget, append, connect, count, fromAscii, fromAscii_helper, insertItem, number, setCurrentIndex, setDefaultAction, setFocusPolicy, setMenu, setPopupMode, setShortcut, setSizePolicy, setText, setToolTip, ~QIcon, ~QKeySequence, ~QString, ~QVariant */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

QToolBar * FUN_140054790(QToolBar *param_1,QWidget *param_2)

{
  int iVar1;
  QAction *pQVar2;
  QKeySequence *pQVar3;
  QMenu *this;
  QToolButton *this_00;
  QWidget *pQVar4;
  QComboBox *this_01;
  QString *pQVar5;
  QIcon *pQVar6;
  QLabel *pQVar7;
  QComboBox *pQVar8;
  QWidget *pQVar9;
  QAction *pQVar10;
  QMenu *this_02;
  QToolButton *this_03;
  ulonglong uVar11;
  QLabel *pQVar12;
  QComboBox *pQVar13;
  char *pcVar14;
  longlong lVar15;
  uint *puVar16;
  undefined1 auStackY_d8 [32];
  Data *local_a8;
  QAction *local_a0;
  Data *local_98;
  undefined4 local_90;
  QIcon local_88 [8];
  QVariant local_80 [16];
  QToolBar *local_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStackY_d8;
  pQVar13 = (QComboBox *)0x0;
  local_90 = 0;
  local_70 = param_1;
  QToolBar::QToolBar(param_1,param_2);
  *(undefined ***)param_1 = ToolBar::vftable;
  *(undefined ***)(param_1 + 0x10) = ToolBar::vftable;
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
  *(QWidget **)(param_1 + 0x28) = param_2;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar10 = (QAction *)pQVar13;
  local_a0 = pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar10 = pQVar2;
  }
  *(QAction **)(param_1 + 0x30) = pQVar10;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar10 = (QAction *)pQVar13;
  local_a0 = pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar10 = pQVar2;
  }
  *(QAction **)(param_1 + 0x38) = pQVar10;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar10 = (QAction *)pQVar13;
  local_a0 = pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar10 = pQVar2;
  }
  *(QAction **)(param_1 + 0x50) = pQVar10;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar10 = (QAction *)pQVar13;
  local_a0 = pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar10 = pQVar2;
  }
  *(QAction **)(param_1 + 0x58) = pQVar10;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar10 = (QAction *)pQVar13;
  local_a0 = pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar10 = pQVar2;
  }
  *(QAction **)(param_1 + 0x60) = pQVar10;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar10 = (QAction *)pQVar13;
  local_a0 = pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar10 = pQVar2;
  }
  *(QAction **)(param_1 + 0x68) = pQVar10;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar10 = (QAction *)pQVar13;
  local_a0 = pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar10 = pQVar2;
  }
  *(QAction **)(param_1 + 0x70) = pQVar10;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar10 = (QAction *)pQVar13;
  local_a0 = pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar10 = pQVar2;
  }
  *(QAction **)(param_1 + 0x78) = pQVar10;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar10 = (QAction *)pQVar13;
  local_a0 = pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar10 = pQVar2;
  }
  *(QAction **)(param_1 + 0x40) = pQVar10;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar10 = (QAction *)pQVar13;
  local_a0 = pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar10 = pQVar2;
  }
  *(QAction **)(param_1 + 0x48) = pQVar10;
  local_a8 = QString::fromAscii_helper("Sampling:",-1);
  QAction::setText(*(QAction **)(param_1 + 0x30),(QString *)&local_a8);
  QString::~QString((QString *)&local_a8);
  local_a8 = QString::fromAscii_helper("Stop Sampling (F9)",-1);
  QAction::setToolTip(*(QAction **)(param_1 + 0x50),(QString *)&local_a8);
  QString::~QString((QString *)&local_a8);
  local_a8 = QString::fromAscii_helper("Zoom In",-1);
  QAction::setToolTip(*(QAction **)(param_1 + 0x58),(QString *)&local_a8);
  QString::~QString((QString *)&local_a8);
  local_a8 = QString::fromAscii_helper("Zoom Out",-1);
  QAction::setToolTip(*(QAction **)(param_1 + 0x60),(QString *)&local_a8);
  QString::~QString((QString *)&local_a8);
  local_a8 = QString::fromAscii_helper("Target:",-1);
  QAction::setText(*(QAction **)(param_1 + 0x68),(QString *)&local_a8);
  QString::~QString((QString *)&local_a8);
  local_a8 = QString::fromAscii_helper("Start Target",-1);
  QAction::setToolTip(*(QAction **)(param_1 + 0x70),(QString *)&local_a8);
  QString::~QString((QString *)&local_a8);
  local_a8 = QString::fromAscii_helper("Reset Target",-1);
  QAction::setToolTip(*(QAction **)(param_1 + 0x78),(QString *)&local_a8);
  QString::~QString((QString *)&local_a8);
  local_a8 = QString::fromAscii_helper("Attach to Debugger",-1);
  QAction::setText(*(QAction **)(param_1 + 0x40),(QString *)&local_a8);
  QString::~QString((QString *)&local_a8);
  local_a8 = QString::fromAscii_helper("Start Sampling",-1);
  QAction::setText(*(QAction **)(param_1 + 0x48),(QString *)&local_a8);
  QString::~QString((QString *)&local_a8);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)&local_98,0x1000034,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0x38),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)&local_98);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)&local_98,0x1000038,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0x50),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)&local_98);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)&local_98,0x1000013,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0x58),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)&local_98);
  pQVar3 = (QKeySequence *)QKeySequence::QKeySequence((QKeySequence *)&local_98,0x1000015,0,0,0);
  QAction::setShortcut(*(QAction **)(param_1 + 0x60),pQVar3);
  QKeySequence::~QKeySequence((QKeySequence *)&local_98);
  QObject::connect(*(QObject **)(param_1 + 0x50),"2triggered(void)",(QObject *)param_1,
                   "1_OnActionStopSampling(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x58),"2triggered(void)",(QObject *)param_1,
                   "1_OnActionZoomIn(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x60),"2triggered(void)",(QObject *)param_1,
                   "1_OnActionZoomOut(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x70),"2triggered(void)",(QObject *)param_1,
                   "1_OnActionStartStopTarget(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x78),"2triggered(void)",(QObject *)param_1,
                   "1_OnActionResetTarget(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x40),"2triggered(void)",(QObject *)param_1,
                   "1_OnActionStartSamplingNoReset(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x48),"2triggered(void)",(QObject *)param_1,
                   "1_OnActionStartSamplingNormal(void)",0);
  QObject::connect(*(QObject **)(param_1 + 0x38),"2triggered(void)",(QObject *)param_1,
                   "1_OnActionStartSamplingShortcut(void)",0);
  QWidget::addAction((QWidget *)param_1,*(QAction **)(param_1 + 0x30));
  this = (QMenu *)FUN_140058d94(0x28);
  this_02 = (QMenu *)pQVar13;
  local_a0 = (QAction *)this;
  if (this != (QMenu *)0x0) {
    QMenu::QMenu(this,(QWidget *)param_1);
    *(undefined ***)this = QMenu::vftable;
    *(undefined ***)(this + 0x10) = QMenu::vftable;
    this_02 = this;
  }
  *(QMenu **)(param_1 + 0xa8) = this_02;
  QWidget::addAction((QWidget *)this_02,*(QAction **)(param_1 + 0x40));
  QWidget::addAction(*(QWidget **)(param_1 + 0xa8),*(QAction **)(param_1 + 0x48));
  this_00 = (QToolButton *)FUN_140058d94(0x28);
  this_03 = (QToolButton *)pQVar13;
  local_a0 = (QAction *)this_00;
  if (this_00 != (QToolButton *)0x0) {
    QToolButton::QToolButton(this_00,(QWidget *)param_1);
    *(undefined ***)this_00 = QToolButton::vftable;
    *(undefined ***)(this_00 + 0x10) = QToolButton::vftable;
    this_03 = this_00;
  }
  *(QToolButton **)(param_1 + 0xa0) = this_03;
  QToolButton::setPopupMode(this_03,1);
  QToolButton::setMenu(*(QToolButton **)(param_1 + 0xa0),*(QMenu **)(param_1 + 0xa8));
  QToolButton::setDefaultAction(*(QToolButton **)(param_1 + 0xa0),*(QAction **)(param_1 + 0x48));
  QToolBar::addWidget(param_1,*(QWidget **)(param_1 + 0xa0));
  QWidget::addAction((QWidget *)param_1,*(QAction **)(param_1 + 0x50));
  QToolBar::addSeparator(param_1);
  QWidget::addAction((QWidget *)param_1,*(QAction **)(param_1 + 0x68));
  QWidget::addAction((QWidget *)param_1,*(QAction **)(param_1 + 0x70));
  QWidget::addAction((QWidget *)param_1,*(QAction **)(param_1 + 0x78));
  QWidget::addAction((QWidget *)param_1,*(QAction **)(param_1 + 0x38));
  pQVar4 = (QWidget *)FUN_140058d94(0x28);
  pQVar9 = (QWidget *)pQVar13;
  local_a0 = (QAction *)pQVar4;
  if (pQVar4 != (QWidget *)0x0) {
    local_a8 = (Data *)((ulonglong)local_a8 & 0xffffffff00000000);
    QWidget::QWidget(pQVar4,0,&local_a8);
    *(undefined ***)pQVar4 = QWidget::vftable;
    *(undefined ***)(pQVar4 + 0x10) = QWidget::vftable;
    pQVar9 = pQVar4;
  }
  QWidget::setSizePolicy(pQVar9,0x57);
  QToolBar::addWidget(param_1,pQVar9);
  this_01 = (QComboBox *)FUN_140058d94(0x28);
  pQVar8 = pQVar13;
  local_a0 = (QAction *)this_01;
  if (this_01 != (QComboBox *)0x0) {
    QComboBox::QComboBox(this_01,(QWidget *)param_1);
    *(undefined ***)this_01 = QComboBox::vftable;
    *(undefined ***)(this_01 + 0x10) = QComboBox::vftable;
    pQVar8 = this_01;
  }
  *(QComboBox **)(param_1 + 0x80) = pQVar8;
  local_68 = 0;
  uStack_60 = 0;
  local_58 = 0;
  uStack_50 = 0;
  puVar16 = &DAT_140096f40;
  lVar15 = 0x1b;
  do {
    if (*puVar16 < 1000) {
      uVar11 = 1;
      pcVar14 = " us/Div";
    }
    else if (*puVar16 < 1000000) {
      uVar11 = 1000;
      pcVar14 = " ms/Div";
    }
    else {
      uVar11 = 1000000;
      pcVar14 = " s/Div";
    }
    FUN_140028380(&local_68,0x20,pcVar14);
    QVariant::QVariant(local_80,*puVar16);
    pQVar5 = (QString *)QString::number((uint)&local_a0,(int)(*puVar16 / uVar11));
    QString::QString((QString *)&local_a8,pQVar5);
    local_90 = 2;
    pQVar5 = (QString *)QString::fromAscii((char *)&local_98,(int)&local_68);
    QString::append((QString *)&local_a8,pQVar5);
    QString::~QString((QString *)&local_98);
    pQVar8 = *(QComboBox **)(param_1 + 0x80);
    iVar1 = QComboBox::count(pQVar8);
    pQVar6 = (QIcon *)QIcon::QIcon(local_88);
    QComboBox::insertItem(pQVar8,iVar1,pQVar6,(QString *)&local_a8,local_80);
    QIcon::~QIcon(local_88);
    local_90 = 0;
    QString::~QString((QString *)&local_a8);
    QString::~QString((QString *)&local_a0);
    QVariant::~QVariant(local_80);
    puVar16 = puVar16 + 1;
    lVar15 = lVar15 + -1;
  } while (lVar15 != 0);
  QComboBox::setCurrentIndex(*(QComboBox **)(param_1 + 0x80),0x12);
  QWidget::setFocusPolicy(*(QWidget **)(param_1 + 0x80),0);
  QObject::connect(*(QObject **)(param_1 + 0x80),"2currentIndexChanged(int)",(QObject *)param_1,
                   "1_OnZoomCurrentIndexChanged(int)",0);
  QWidget::addAction((QWidget *)param_1,*(QAction **)(param_1 + 0x58));
  QWidget::addAction((QWidget *)param_1,*(QAction **)(param_1 + 0x60));
  QToolBar::addWidget(param_1,*(QWidget **)(param_1 + 0x80));
  QToolBar::addSeparator(param_1);
  pQVar7 = (QLabel *)FUN_140058d94(0x28);
  pQVar12 = (QLabel *)pQVar13;
  local_a0 = (QAction *)pQVar7;
  if (pQVar7 != (QLabel *)0x0) {
    local_98 = QString::fromAscii_helper("Target FPS: ",-1);
    local_90 = 1;
    local_a8 = (Data *)((ulonglong)local_a8 & 0xffffffff00000000);
    QLabel::QLabel(pQVar7,&local_98,0,&local_a8);
    *(undefined ***)pQVar7 = QLabel::vftable;
    *(undefined ***)(pQVar7 + 0x10) = QLabel::vftable;
    pQVar12 = pQVar7;
  }
  *(QLabel **)(param_1 + 0x88) = pQVar12;
  if (pQVar7 != (QLabel *)0x0) {
    QString::~QString((QString *)&local_98);
  }
  pQVar8 = (QComboBox *)FUN_140058d94(0x28);
  local_a0 = (QAction *)pQVar8;
  if (pQVar8 != (QComboBox *)0x0) {
    QComboBox::QComboBox(pQVar8,(QWidget *)param_1);
    *(undefined ***)pQVar8 = QComboBox::vftable;
    *(undefined ***)(pQVar8 + 0x10) = QComboBox::vftable;
    pQVar13 = pQVar8;
  }
  *(QComboBox **)(param_1 + 0x90) = pQVar13;
  puVar16 = &DAT_140096fb0;
  lVar15 = 0xb;
  do {
    QVariant::QVariant(local_80,*puVar16);
    pQVar5 = (QString *)QString::number((uint)local_88,*puVar16);
    pQVar13 = *(QComboBox **)(param_1 + 0x90);
    iVar1 = QComboBox::count(pQVar13);
    pQVar6 = (QIcon *)QIcon::QIcon((QIcon *)&local_a0);
    QComboBox::insertItem(pQVar13,iVar1,pQVar6,pQVar5,local_80);
    QIcon::~QIcon((QIcon *)&local_a0);
    QString::~QString((QString *)local_88);
    QVariant::~QVariant(local_80);
    puVar16 = puVar16 + 1;
    lVar15 = lVar15 + -1;
  } while (lVar15 != 0);
  QComboBox::setCurrentIndex(*(QComboBox **)(param_1 + 0x90),7);
  FUN_14003f420(0x1e);
  QWidget::setFocusPolicy(*(QWidget **)(param_1 + 0x90),0);
  QObject::connect(*(QObject **)(param_1 + 0x90),"2currentIndexChanged(int)",(QObject *)param_1,
                   "1_OnFPSCurrentIndexChanged(int)",0);
  QToolBar::addWidget(param_1,*(QWidget **)(param_1 + 0x88));
  QToolBar::addWidget(param_1,*(QWidget **)(param_1 + 0x90));
  QToolBar::addSeparator(param_1);
  pQVar9 = (QWidget *)FUN_140035d30(param_1,"Autoscroll","2stateChanged(int)",param_1);
  *(QWidget **)(param_1 + 0x98) = pQVar9;
  QToolBar::addWidget(param_1,pQVar9);
  *(undefined4 *)(param_1 + 0xb0) = 0;
  FUN_140055480(param_1);
  return param_1;
}


/* ================= FUN_140055350 @ 140055350  size=205  callers=1 callees=0 ================= */
/* CALLERS: FUN_140055430 */

void FUN_140055350(QToolBar *param_1)

{
  longlong *plVar1;
  
  *(undefined ***)param_1 = ToolBar::vftable;
  *(undefined ***)(param_1 + 0x10) = ToolBar::vftable;
  plVar1 = *(longlong **)(param_1 + 0x30);
  if (plVar1 != (longlong *)0x0) {
    (**(code **)(*plVar1 + 0x18))(plVar1,1);
  }
  plVar1 = *(longlong **)(param_1 + 0x38);
  if (plVar1 != (longlong *)0x0) {
    (**(code **)(*plVar1 + 0x18))(plVar1,1);
  }
  plVar1 = *(longlong **)(param_1 + 0x50);
  if (plVar1 != (longlong *)0x0) {
    (**(code **)(*plVar1 + 0x18))(plVar1,1);
  }
  plVar1 = *(longlong **)(param_1 + 0x58);
  if (plVar1 != (longlong *)0x0) {
    (**(code **)(*plVar1 + 0x18))(plVar1,1);
  }
  plVar1 = *(longlong **)(param_1 + 0x60);
  if (plVar1 != (longlong *)0x0) {
    (**(code **)(*plVar1 + 0x18))(plVar1,1);
  }
  plVar1 = *(longlong **)(param_1 + 0x68);
  if (plVar1 != (longlong *)0x0) {
    (**(code **)(*plVar1 + 0x18))(plVar1,1);
  }
  plVar1 = *(longlong **)(param_1 + 0x70);
  if (plVar1 != (longlong *)0x0) {
    (**(code **)(*plVar1 + 0x18))(plVar1,1);
  }
  plVar1 = *(longlong **)(param_1 + 0x78);
  if (plVar1 != (longlong *)0x0) {
    (**(code **)(*plVar1 + 0x18))(plVar1,1);
  }
                    /* WARNING: Could not recover jumptable at 0x000140055416. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QToolBar::~QToolBar(param_1);
  return;
}


/* ================= FUN_140055420 @ 140055420  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140055430 */

void FUN_140055420(longlong param_1)

{
  FUN_140055430(param_1 + -0x10);
  return;
}


/* ================= FUN_140055430 @ 140055430  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_140055420 */
/* CALLEES: FUN_140055350, free */

void * FUN_140055430(void *param_1,ulonglong param_2)

{
  FUN_140055350();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140055470 @ 140055470  size=14  callers=8 callees=0 ================= */
/* CALLERS: FUN_140048e60, FUN_140049a10, FUN_14004a6e0, FUN_14004a750, FUN_14004a7c0, FUN_14004a840, FUN_14004bdc0, FUN_140056130 */

void FUN_140055470(longlong param_1,bool param_2)

{
                    /* WARNING: Could not recover jumptable at 0x000140055477. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QAbstractButton::setChecked(*(QAbstractButton **)(param_1 + 0x98),param_2);
  return;
}


/* ================= FUN_140055480 @ 140055480  size=322  callers=2 callees=3 ================= */
/* CALLERS: FUN_140054790, FUN_140055bb0 */
/* CALLEES: FUN_14002a320, setEnabled, setIcon */

void FUN_140055480(longlong param_1)

{
  QIcon *pQVar1;
  
  if (*(int *)(param_1 + 0xb8) != 1) {
    pQVar1 = (QIcon *)FUN_14002a320(10);
    QAction::setIcon(*(QAction **)(param_1 + 0x40),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(10);
    QAction::setIcon(*(QAction **)(param_1 + 0x48),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x11);
    QAction::setIcon(*(QAction **)(param_1 + 0x50),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x1d);
    QAction::setIcon(*(QAction **)(param_1 + 0x58),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x1f);
    QAction::setIcon(*(QAction **)(param_1 + 0x60),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(10);
    QAction::setIcon(*(QAction **)(param_1 + 0x70),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0xf);
    QAction::setIcon(*(QAction **)(param_1 + 0x78),pQVar1);
    QAction::setEnabled(*(QAction **)(param_1 + 0x30),false);
    QWidget::setEnabled(*(QWidget **)(param_1 + 0xa8),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x40),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x48),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x50),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x58),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x60),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x68),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x70),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x78),false);
    *(undefined4 *)(param_1 + 0xb8) = 1;
  }
  return;
}


/* ================= FUN_1400555d0 @ 1400555d0  size=348  callers=2 callees=6 ================= */
/* CALLERS: FUN_140055bb0, FUN_140056130 */
/* CALLEES: FUN_14002a320, fromAscii_helper, setEnabled, setIcon, setToolTip, ~QString */

void FUN_1400555d0(longlong param_1)

{
  QIcon *pQVar1;
  Data *local_res8 [4];
  
  if (*(int *)(param_1 + 0xb8) != 5) {
    QWidget::setEnabled(*(QWidget **)(param_1 + 0xa8),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x40),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x48),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x50),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x58),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x60),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x70),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x78),true);
    pQVar1 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x40),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x48),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x10);
    QAction::setIcon(*(QAction **)(param_1 + 0x50),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x1c);
    QAction::setIcon(*(QAction **)(param_1 + 0x58),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x1e);
    QAction::setIcon(*(QAction **)(param_1 + 0x60),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x70),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0xe);
    QAction::setIcon(*(QAction **)(param_1 + 0x78),pQVar1);
    local_res8[0] = QString::fromAscii_helper("Start Sampling (F5)",-1);
    QAction::setToolTip(*(QAction **)(param_1 + 0x38),(QString *)local_res8);
    QString::~QString((QString *)local_res8);
    *(undefined4 *)(param_1 + 0xb8) = 5;
  }
  return;
}


/* ================= FUN_140055730 @ 140055730  size=199  callers=1 callees=3 ================= */
/* CALLERS: FUN_140055bb0 */
/* CALLEES: FUN_14002a320, setEnabled, setIcon */

void FUN_140055730(longlong param_1)

{
  QIcon *pQVar1;
  
  if (*(int *)(param_1 + 0xb8) != 2) {
    QWidget::setEnabled(*(QWidget **)(param_1 + 0xa8),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x40),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x48),false);
    pQVar1 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x40),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x48),pQVar1);
    QAction::setEnabled(*(QAction **)(param_1 + 0x58),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x60),true);
    *(undefined4 *)(param_1 + 0xb8) = 2;
    pQVar1 = (QIcon *)FUN_14002a320(0x1c);
    QAction::setIcon(*(QAction **)(param_1 + 0x58),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x1e);
                    /* WARNING: Could not recover jumptable at 0x0001400557ea. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    QAction::setIcon(*(QAction **)(param_1 + 0x60),pQVar1);
    return;
  }
  return;
}


/* ================= FUN_140055800 @ 140055800  size=348  callers=1 callees=6 ================= */
/* CALLERS: FUN_140055bb0 */
/* CALLEES: FUN_14002a320, fromAscii_helper, setEnabled, setIcon, setToolTip, ~QString */

void FUN_140055800(longlong param_1)

{
  QIcon *pQVar1;
  Data *local_res8 [4];
  
  if (*(int *)(param_1 + 0xb8) != 4) {
    QWidget::setEnabled(*(QWidget **)(param_1 + 0xa8),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x40),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x48),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x50),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x58),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x60),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x70),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x78),true);
    pQVar1 = (QIcon *)FUN_14002a320(7);
    QAction::setIcon(*(QAction **)(param_1 + 0x40),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(7);
    QAction::setIcon(*(QAction **)(param_1 + 0x48),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x10);
    QAction::setIcon(*(QAction **)(param_1 + 0x50),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x1c);
    QAction::setIcon(*(QAction **)(param_1 + 0x58),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x1e);
    QAction::setIcon(*(QAction **)(param_1 + 0x60),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x70),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0xe);
    QAction::setIcon(*(QAction **)(param_1 + 0x78),pQVar1);
    local_res8[0] = QString::fromAscii_helper("Pause Sampling",-1);
    QAction::setToolTip(*(QAction **)(param_1 + 0x38),(QString *)local_res8);
    QString::~QString((QString *)local_res8);
    *(undefined4 *)(param_1 + 0xb8) = 4;
  }
  return;
}


/* ================= FUN_140055960 @ 140055960  size=348  callers=2 callees=6 ================= */
/* CALLERS: FUN_140055bb0, FUN_140055f90 */
/* CALLEES: FUN_14002a320, fromAscii_helper, setEnabled, setIcon, setToolTip, ~QString */

void FUN_140055960(longlong param_1)

{
  QIcon *pQVar1;
  Data *local_res8 [4];
  
  if (*(int *)(param_1 + 0xb8) != 6) {
    QWidget::setEnabled(*(QWidget **)(param_1 + 0xa8),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x40),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x48),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x50),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x58),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x60),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x70),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x78),false);
    pQVar1 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x40),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x48),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x11);
    QAction::setIcon(*(QAction **)(param_1 + 0x50),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x1c);
    QAction::setIcon(*(QAction **)(param_1 + 0x58),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x1e);
    QAction::setIcon(*(QAction **)(param_1 + 0x60),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(10);
    QAction::setIcon(*(QAction **)(param_1 + 0x70),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0xf);
    QAction::setIcon(*(QAction **)(param_1 + 0x78),pQVar1);
    local_res8[0] = QString::fromAscii_helper("Start Sampling (F5)",-1);
    QAction::setToolTip(*(QAction **)(param_1 + 0x38),(QString *)local_res8);
    QString::~QString((QString *)local_res8);
    *(undefined4 *)(param_1 + 0xb8) = 6;
  }
  return;
}


/* ================= FUN_140055ac0 @ 140055ac0  size=228  callers=1 callees=3 ================= */
/* CALLERS: FUN_140055bb0 */
/* CALLEES: FUN_14002a320, setEnabled, setIcon */

void FUN_140055ac0(longlong param_1)

{
  QIcon *pQVar1;
  
  if (*(int *)(param_1 + 0xb8) != 3) {
    QAction::setEnabled(*(QAction **)(param_1 + 0x58),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x60),true);
    pQVar1 = (QIcon *)FUN_14002a320(0x1c);
    QAction::setIcon(*(QAction **)(param_1 + 0x58),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x1e);
    QAction::setIcon(*(QAction **)(param_1 + 0x60),pQVar1);
    QWidget::setEnabled(*(QWidget **)(param_1 + 0xa8),true);
    pQVar1 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x40),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x48),pQVar1);
    QAction::setEnabled(*(QAction **)(param_1 + 0x40),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x48),true);
    pQVar1 = (QIcon *)FUN_14002a320(0x11);
    QAction::setIcon(*(QAction **)(param_1 + 0x50),pQVar1);
    QAction::setEnabled(*(QAction **)(param_1 + 0x50),false);
    *(undefined4 *)(param_1 + 0xb8) = 3;
  }
  return;
}


/* ================= FUN_140055bb0 @ 140055bb0  size=491  callers=1 callees=19 ================= */
/* CALLERS: FUN_14004d870 */
/* CALLEES: FUN_14002a320, FUN_14003eee0, FUN_14003ef00, FUN_14003f920, FUN_14004e9c0, FUN_14004ea40, FUN_140055480, FUN_1400555d0, FUN_140055730, FUN_140055800, FUN_140055960, FUN_140055ac0, actions, isVisible, qFree, setIcon, setMenu, setPopupMode, setVisible */

void FUN_140055bb0(longlong param_1)

{
  code *pcVar1;
  char cVar2;
  bool bVar3;
  int iVar4;
  QIcon *pQVar5;
  undefined8 *puVar6;
  undefined8 uVar7;
  uint uVar8;
  int iVar10;
  ulonglong uVar11;
  ulonglong uVar12;
  code *local_res10;
  int *local_res18;
  ulonglong uVar9;
  
  pcVar1 = shared_null_exref;
  local_res10 = shared_null_exref;
  LOCK();
  *(int *)shared_null_exref = *(int *)shared_null_exref + 1;
  UNLOCK();
  uVar9 = 0;
  iVar10 = 0;
  if ((DAT_14286d3b8 == (longlong *)0x0) || (*DAT_14286d3b8 == 0)) {
    iVar4 = FUN_14004ea40();
    if (iVar4 == 0) {
      FUN_140055480(param_1);
    }
    else {
      FUN_140055730();
    }
  }
  else {
    cVar2 = FUN_14003eee0();
    if (cVar2 == '\0') {
      FUN_140055960(param_1);
      *(undefined4 *)(param_1 + 0xb0) = 0;
      FUN_140055ac0(param_1);
    }
    else {
      cVar2 = FUN_14003ef00();
      if (cVar2 == '\0') {
        FUN_140055800(param_1);
        *(undefined4 *)(param_1 + 0xb0) = 1;
      }
      else {
        FUN_1400555d0();
        *(undefined4 *)(param_1 + 0xb0) = 0;
      }
    }
  }
  iVar4 = FUN_14003f920();
  uVar7 = 7;
  if (iVar4 == 0) {
    uVar7 = 9;
  }
  pQVar5 = (QIcon *)FUN_14002a320(uVar7);
  QAction::setIcon(*(QAction **)(param_1 + 0x70),pQVar5);
  iVar4 = FUN_14004ea40();
  if (iVar4 != 0) {
    FUN_14004e9c0(&DAT_14286c7d0);
    QAction::setVisible(*(QAction **)(param_1 + 0x40),DAT_14286cd76 != '\0');
    puVar6 = (undefined8 *)QWidget::actions(*(QWidget **)(param_1 + 0xa8));
    local_res10 = (code *)*puVar6;
    *puVar6 = pcVar1;
    LOCK();
    iVar4 = *local_res18;
    *local_res18 = *local_res18 + -1;
    UNLOCK();
    if (iVar4 == 1) {
      qFree(local_res18);
    }
    iVar4 = *(int *)(local_res10 + 8);
    uVar11 = uVar9;
    uVar12 = uVar9;
    if (0 < *(int *)(local_res10 + 0xc) - iVar4) {
      do {
        bVar3 = QAction::isVisible(*(QAction **)
                                    (local_res10 + ((longlong)iVar4 + uVar12) * 8 + 0x18));
        if (bVar3) {
          uVar11 = (ulonglong)((int)uVar11 + 1);
        }
        iVar10 = (int)uVar11;
        uVar8 = (int)uVar9 + 1;
        uVar9 = (ulonglong)uVar8;
        iVar4 = *(int *)(local_res10 + 8);
        uVar12 = uVar12 + 1;
      } while ((int)uVar8 < *(int *)(local_res10 + 0xc) - iVar4);
    }
    if (iVar10 < 2) {
      QToolButton::setMenu(*(QToolButton **)(param_1 + 0xa0),(QMenu *)0x0);
      QToolButton::setPopupMode(*(QToolButton **)(param_1 + 0xa0),0);
    }
    else {
      QToolButton::setMenu(*(QToolButton **)(param_1 + 0xa0),*(QMenu **)(param_1 + 0xa8));
      QToolButton::setPopupMode(*(QToolButton **)(param_1 + 0xa0),1);
    }
  }
  LOCK();
  iVar10 = *(int *)local_res10;
  *(int *)local_res10 = *(int *)local_res10 + -1;
  UNLOCK();
  if (iVar10 == 1) {
    qFree(local_res10);
  }
  return;
}


/* ================= FUN_140055dc0 @ 140055dc0  size=136  callers=1 callees=9 ================= */
/* CALLERS: FUN_14003c630 */
/* CALLEES: FUN_14002a320, FUN_14003ef10, FUN_14003f390, FUN_14004e9c0, FUN_140055f90, fromAscii_helper, setIcon, setToolTip, ~QString */

void FUN_140055dc0(longlong param_1)

{
  char cVar1;
  QIcon *pQVar2;
  Data *local_res8 [4];
  
  pQVar2 = (QIcon *)FUN_14002a320(9);
  QAction::setIcon(*(QAction **)(param_1 + 0x70),pQVar2);
  local_res8[0] = QString::fromAscii_helper("Start Target",-1);
  QAction::setToolTip(*(QAction **)(param_1 + 0x70),(QString *)local_res8);
  QString::~QString((QString *)local_res8);
  FUN_14003f390();
  FUN_14004e9c0(&DAT_14286c7d0);
  if (DAT_14286cd76 != '\0') {
    cVar1 = FUN_14003ef10();
    if (cVar1 != '\0') {
      FUN_140055f90(param_1);
      return;
    }
  }
  return;
}


/* ================= FUN_140055e50 @ 140055e50  size=49  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c630 */
/* CALLEES: FUN_14003f410, FUN_140056130, setDefaultAction */

void FUN_140055e50(longlong param_1)

{
  FUN_14003f410(1);
  QToolButton::setDefaultAction(*(QToolButton **)(param_1 + 0xa0),*(QAction **)(param_1 + 0x40));
  FUN_140056130(param_1);
  return;
}


/* ================= FUN_140055e90 @ 140055e90  size=46  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c630 */
/* CALLEES: FUN_14003f410, FUN_140056130, setDefaultAction */

void FUN_140055e90(longlong param_1)

{
  FUN_14003f410(0);
  QToolButton::setDefaultAction(*(QToolButton **)(param_1 + 0xa0),*(QAction **)(param_1 + 0x48));
  FUN_140056130(param_1);
  return;
}


/* ================= FUN_140055ed0 @ 140055ed0  size=184  callers=1 callees=8 ================= */
/* CALLERS: FUN_14003c630 */
/* CALLEES: FUN_14002a320, FUN_14003f890, FUN_14003f8d0, FUN_14003f920, fromAscii_helper, setIcon, setToolTip, ~QString */

void FUN_140055ed0(longlong param_1)

{
  int iVar1;
  QIcon *pQVar2;
  Data *local_res10 [3];
  
  iVar1 = FUN_14003f920();
  if (iVar1 != 0) {
    pQVar2 = (QIcon *)FUN_14002a320(9);
    QAction::setIcon(*(QAction **)(param_1 + 0x70),pQVar2);
    local_res10[0] = QString::fromAscii_helper("Start Target",-1);
    QAction::setToolTip(*(QAction **)(param_1 + 0x70),(QString *)local_res10);
    QString::~QString((QString *)local_res10);
    FUN_14003f8d0();
    return;
  }
  pQVar2 = (QIcon *)FUN_14002a320(7);
  QAction::setIcon(*(QAction **)(param_1 + 0x70),pQVar2);
  local_res10[0] = QString::fromAscii_helper("Stop Target",-1);
  QAction::setToolTip(*(QAction **)(param_1 + 0x70),(QString *)local_res10);
  QString::~QString((QString *)local_res10);
  FUN_14003f890();
  return;
}


/* ================= FUN_140055f90 @ 140055f90  size=43  callers=2 callees=3 ================= */
/* CALLERS: FUN_14003c630, FUN_140055dc0 */
/* CALLEES: FUN_14003e680, FUN_14004cf30, FUN_140055960 */

void FUN_140055f90(longlong param_1)

{
  FUN_140055960();
  FUN_14004cf30(*(undefined8 *)(param_1 + 0x28));
  *(undefined4 *)(param_1 + 0xb0) = 0;
  FUN_14003e680();
  return;
}


/* ================= FUN_140055fc0 @ 140055fc0  size=54  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003c630 */
/* CALLEES: currentIndex */

void FUN_140055fc0(longlong param_1)

{
  int iVar1;
  
  iVar1 = QComboBox::currentIndex(*(QComboBox **)(param_1 + 0x80));
  if (-1 < iVar1 + -1) {
                    /* WARNING: Could not recover jumptable at 0x000140055fe9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    QComboBox::setCurrentIndex(*(QComboBox **)(param_1 + 0x80),iVar1 + -1);
    return;
  }
  return;
}


/* ================= FUN_140056000 @ 140056000  size=72  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003c630 */
/* CALLEES: count, currentIndex, setCurrentIndex */

void FUN_140056000(longlong param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = QComboBox::currentIndex(*(QComboBox **)(param_1 + 0x80));
  iVar2 = QComboBox::count(*(QComboBox **)(param_1 + 0x80));
  if (iVar1 + 1 < iVar2) {
    QComboBox::setCurrentIndex(*(QComboBox **)(param_1 + 0x80),iVar1 + 1);
  }
  return;
}


/* ================= FUN_140056050 @ 140056050  size=17  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003c630 */
/* CALLEES: FUN_140049980 */

void FUN_140056050(undefined8 param_1,int param_2)

{
  FUN_140049980(DAT_14286d3a0,param_2 != 0);
  return;
}


/* ================= FUN_140056070 @ 140056070  size=82  callers=1 callees=5 ================= */
/* CALLERS: FUN_14003c630 */
/* CALLEES: FUN_14003f420, FUN_14004e120, itemData, toUInt, ~QVariant */

void FUN_140056070(longlong param_1,int param_2)

{
  QVariant *this;
  uint uVar1;
  QVariant local_18 [16];
  
  this = (QVariant *)QComboBox::itemData(*(QComboBox **)(param_1 + 0x90),(int)local_18,param_2);
  uVar1 = QVariant::toUInt(this,(bool *)0x0);
  QVariant::~QVariant(local_18);
  FUN_14003f420(uVar1);
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_1400560d0 @ 1400560d0  size=89  callers=1 callees=5 ================= */
/* CALLERS: FUN_14003c630 */
/* CALLEES: FUN_140049a10, FUN_14004e120, itemData, toUInt, ~QVariant */

void FUN_1400560d0(longlong param_1,int param_2)

{
  QVariant *this;
  uint uVar1;
  QVariant local_18 [16];
  
  this = (QVariant *)QComboBox::itemData(*(QComboBox **)(param_1 + 0x80),(int)local_18,param_2);
  uVar1 = QVariant::toUInt(this,(bool *)0x0);
  QVariant::~QVariant(local_18);
  FUN_140049a10(DAT_14286d3a0,uVar1);
  FUN_14004e120(DAT_14286d388);
  return;
}


/* ================= FUN_140056130 @ 140056130  size=114  callers=3 callees=7 ================= */
/* CALLERS: FUN_140055e50, FUN_140055e90, thunk_FUN_140056130 */
/* CALLEES: FUN_14003e5c0, FUN_14003e620, FUN_14004cf00, FUN_14004cf30, FUN_14004e9c0, FUN_140055470, FUN_1400555d0 */

void FUN_140056130(longlong param_1)

{
  if (*(int *)(param_1 + 0xb0) != 0) {
    FUN_1400555d0();
    FUN_14004cf30(*(undefined8 *)(param_1 + 0x28));
    FUN_14003e5c0();
    *(undefined4 *)(param_1 + 0xb0) = 0;
    return;
  }
  FUN_14004cf00(*(undefined8 *)(param_1 + 0x28));
  FUN_14004e9c0(&DAT_14286c7d0);
  if (DAT_14286cd75 != '\0') {
    FUN_140055470(param_1,1);
  }
  FUN_14003e620();
  *(undefined4 *)(param_1 + 0xb0) = 1;
  return;
}


/* ================= FUN_1400561b0 @ 1400561b0  size=1761  callers=1 callees=25 ================= */
/* CALLERS: FUN_14004c6a0 */
/* CALLEES: FUN_140035e00, FUN_1400360a0, FUN_140043d10, FUN_140056950, FUN_140058d94, QAction, QDoubleValidator, QHBoxLayout, QLabel, QToolBar, QWidget, addAction, addWidget, connect, fromAscii_helper, resize, setCheckable, setContentsMargins, setDisabled, setLayout, setMinimumContentsLength, setToolTip, setValidator, sizeHint, ~QString */

QWidget * FUN_1400561b0(QWidget *param_1,undefined8 param_2)

{
  QHBoxLayout *this;
  QLabel *pQVar1;
  QToolBar *this_00;
  QAction *pQVar2;
  QComboBox *this_01;
  undefined8 uVar3;
  QDoubleValidator *this_02;
  QWidget *this_03;
  QLabel *pQVar4;
  undefined8 *puVar5;
  QSize *pQVar6;
  QHBoxLayout *pQVar7;
  QToolBar *pQVar8;
  QAction *pQVar9;
  QDoubleValidator *pQVar10;
  QLabel *pQVar11;
  uint uVar12;
  bool bVar13;
  undefined8 local_res18;
  undefined4 local_res20 [2];
  undefined4 local_58 [2];
  Data *local_50;
  QLabel *local_48;
  
  pQVar11 = (QLabel *)0x0;
  local_res18 = (QDoubleValidator *)((ulonglong)local_res18 & 0xffffffff00000000);
  local_res20[0] = 0;
  QWidget::QWidget(param_1,param_2,local_res20);
  *(undefined ***)param_1 = TriggerWidget::vftable;
  *(undefined ***)(param_1 + 0x10) = TriggerWidget::vftable;
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
  this = (QHBoxLayout *)FUN_140058d94(0x20);
  pQVar7 = (QHBoxLayout *)pQVar11;
  local_res18 = (QDoubleValidator *)this;
  if (this != (QHBoxLayout *)0x0) {
    QHBoxLayout::QHBoxLayout(this,param_1);
    *(undefined ***)this = QHBoxLayout::vftable;
    *(undefined ***)(this + 0x10) = QHBoxLayout::vftable;
    pQVar7 = this;
  }
  *(QHBoxLayout **)(param_1 + 0x28) = pQVar7;
  pQVar1 = (QLabel *)FUN_140058d94(0x28);
  bVar13 = pQVar1 != (QLabel *)0x0;
  pQVar4 = pQVar11;
  local_48 = pQVar1;
  if (bVar13) {
    local_50 = QString::fromAscii_helper("Trigger control: ",-1);
    local_res18 = (QDoubleValidator *)CONCAT44(local_res18._4_4_,1);
    local_58[0] = 0;
    QLabel::QLabel(pQVar1,&local_50,0,local_58);
    *(undefined ***)pQVar1 = QLabel::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QLabel::vftable;
    pQVar4 = pQVar1;
  }
  uVar12 = (uint)bVar13;
  *(QLabel **)(param_1 + 0x30) = pQVar4;
  if (bVar13) {
    uVar12 = 0;
    QString::~QString((QString *)&local_50);
  }
  this_00 = (QToolBar *)FUN_140058d94(0x28);
  pQVar8 = (QToolBar *)pQVar11;
  local_res18 = (QDoubleValidator *)this_00;
  if (this_00 != (QToolBar *)0x0) {
    QToolBar::QToolBar(this_00,param_1);
    *(undefined ***)this_00 = QToolBar::vftable;
    *(undefined ***)(this_00 + 0x10) = QToolBar::vftable;
    pQVar8 = this_00;
  }
  *(QToolBar **)(param_1 + 0x38) = pQVar8;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar9 = (QAction *)pQVar11;
  local_res18 = (QDoubleValidator *)pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar9 = pQVar2;
  }
  *(QAction **)(param_1 + 0x40) = pQVar9;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar9 = (QAction *)pQVar11;
  local_res18 = (QDoubleValidator *)pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar9 = pQVar2;
  }
  *(QAction **)(param_1 + 0x48) = pQVar9;
  pQVar2 = (QAction *)FUN_140058d94(0x10);
  pQVar9 = (QAction *)pQVar11;
  local_res18 = (QDoubleValidator *)pQVar2;
  if (pQVar2 != (QAction *)0x0) {
    QAction::QAction(pQVar2,(QObject *)param_1);
    *(undefined ***)pQVar2 = QAction::vftable;
    pQVar9 = pQVar2;
  }
  *(QAction **)(param_1 + 0x50) = pQVar9;
  pQVar1 = (QLabel *)FUN_140058d94(0x28);
  pQVar4 = pQVar11;
  local_48 = pQVar1;
  if (pQVar1 != (QLabel *)0x0) {
    local_50 = QString::fromAscii_helper("Symbol: ",-1);
    local_res18 = (QDoubleValidator *)(CONCAT44(local_res18._4_4_,uVar12) | 2);
    local_58[0] = 0;
    QLabel::QLabel(pQVar1,&local_50,0,local_58);
    *(undefined ***)pQVar1 = QLabel::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QLabel::vftable;
    pQVar4 = pQVar1;
    uVar12 = uVar12 | 2;
  }
  *(QLabel **)(param_1 + 0x58) = pQVar4;
  if ((uVar12 & 2) != 0) {
    uVar12 = uVar12 & 0xfffffffd;
    QString::~QString((QString *)&local_50);
  }
  this_01 = (QComboBox *)FUN_140035e00(param_1,0,0);
  *(QComboBox **)(param_1 + 0x60) = this_01;
  QComboBox::setMinimumContentsLength(this_01,0x1e);
  pQVar1 = (QLabel *)FUN_140058d94(0x28);
  pQVar4 = pQVar11;
  local_48 = pQVar1;
  if (pQVar1 != (QLabel *)0x0) {
    local_50 = QString::fromAscii_helper("Threshold: ",-1);
    local_res18 = (QDoubleValidator *)(CONCAT44(local_res18._4_4_,uVar12) | 4);
    local_58[0] = 0;
    QLabel::QLabel(pQVar1,&local_50,0,local_58);
    *(undefined ***)pQVar1 = QLabel::vftable;
    *(undefined ***)(pQVar1 + 0x10) = QLabel::vftable;
    pQVar4 = pQVar1;
    uVar12 = uVar12 | 4;
  }
  *(QLabel **)(param_1 + 0x68) = pQVar4;
  if ((uVar12 & 4) != 0) {
    uVar12 = uVar12 & 0xfffffffb;
    QString::~QString((QString *)&local_50);
  }
  uVar3 = FUN_1400360a0(param_1,&DAT_14005f306,0);
  *(undefined8 *)(param_1 + 0x70) = uVar3;
  local_res18 = (QDoubleValidator *)QString::fromAscii_helper("Trigger Rising Edge",-1);
  QAction::setToolTip(*(QAction **)(param_1 + 0x40),(QString *)&local_res18);
  QString::~QString((QString *)&local_res18);
  local_res18 = (QDoubleValidator *)QString::fromAscii_helper("Trigger Falling Edge",-1);
  QAction::setToolTip(*(QAction **)(param_1 + 0x48),(QString *)&local_res18);
  QString::~QString((QString *)&local_res18);
  local_res18 = (QDoubleValidator *)QString::fromAscii_helper("Trigger Both Edges",-1);
  QAction::setToolTip(*(QAction **)(param_1 + 0x50),(QString *)&local_res18);
  QString::~QString((QString *)&local_res18);
  QAction::setCheckable(*(QAction **)(param_1 + 0x40),true);
  QAction::setCheckable(*(QAction **)(param_1 + 0x48),true);
  QAction::setCheckable(*(QAction **)(param_1 + 0x50),true);
  QWidget::addAction(*(QWidget **)(param_1 + 0x38),*(QAction **)(param_1 + 0x40));
  QWidget::addAction(*(QWidget **)(param_1 + 0x38),*(QAction **)(param_1 + 0x48));
  QWidget::addAction(*(QWidget **)(param_1 + 0x38),*(QAction **)(param_1 + 0x50));
  this_02 = (QDoubleValidator *)FUN_140058d94(0x28);
  pQVar10 = (QDoubleValidator *)pQVar11;
  local_res18 = this_02;
  if (this_02 != (QDoubleValidator *)0x0) {
    QDoubleValidator::QDoubleValidator(this_02,(QObject *)param_1);
    *(undefined ***)this_02 = QDoubleValidator::vftable;
    pQVar10 = this_02;
  }
  QLineEdit::setValidator(*(QLineEdit **)(param_1 + 0x70),(QValidator *)pQVar10);
  QLayout::setContentsMargins(*(QLayout **)(param_1 + 0x28),5,5,5,0);
  this_03 = (QWidget *)FUN_1400360a0(param_1,&DAT_14005f4e4,1);
  *(QWidget **)(param_1 + 0x80) = this_03;
  QWidget::setDisabled(this_03,true);
  pQVar4 = (QLabel *)FUN_140058d94(0x28);
  local_48 = pQVar4;
  if (pQVar4 != (QLabel *)0x0) {
    local_50 = QString::fromAscii_helper("Value:",-1);
    local_res18 = (QDoubleValidator *)(CONCAT44(local_res18._4_4_,uVar12) | 8);
    local_58[0] = 0;
    QLabel::QLabel(pQVar4,&local_50,0,local_58);
    *(undefined ***)pQVar4 = QLabel::vftable;
    *(undefined ***)(pQVar4 + 0x10) = QLabel::vftable;
    pQVar11 = pQVar4;
    uVar12 = 8;
  }
  *(QLabel **)(param_1 + 0x78) = pQVar11;
  if ((uVar12 & 8) != 0) {
    QString::~QString((QString *)&local_50);
  }
  local_res18 = (QDoubleValidator *)((ulonglong)local_res18 & 0xffffffff00000000);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x30),0,&local_res18);
  local_res18 = (QDoubleValidator *)((ulonglong)local_res18 & 0xffffffff00000000);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x38),0,&local_res18);
  local_res18 = (QDoubleValidator *)((ulonglong)local_res18 & 0xffffffff00000000);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x58),0,&local_res18);
  local_res18 = (QDoubleValidator *)((ulonglong)local_res18 & 0xffffffff00000000);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x60),0,&local_res18);
  local_res18 = (QDoubleValidator *)((ulonglong)local_res18 & 0xffffffff00000000);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x68),0,&local_res18);
  local_res18 = (QDoubleValidator *)((ulonglong)local_res18 & 0xffffffff00000000);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x70),0,&local_res18);
  local_res18 = (QDoubleValidator *)((ulonglong)local_res18 & 0xffffffff00000000);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x78),0,&local_res18);
  local_res18 = (QDoubleValidator *)((ulonglong)local_res18 & 0xffffffff00000000);
  QBoxLayout::addWidget
            (*(QBoxLayout **)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x80),0,&local_res18);
  QWidget::setLayout(param_1,*(QLayout **)(param_1 + 0x28));
  QObject::connect(*(QObject **)(param_1 + 0x40),"2toggled(bool)",(QObject *)param_1,
                   "1_SlotOnActionRising(bool)",0);
  QObject::connect(*(QObject **)(param_1 + 0x48),"2toggled(bool)",(QObject *)param_1,
                   "1_SlotOnActionFalling(bool)",0);
  QObject::connect(*(QObject **)(param_1 + 0x50),"2toggled(bool)",(QObject *)param_1,
                   "1_SlotOnActionBoth(bool)",0);
  QObject::connect(*(QObject **)(param_1 + 0x60),"2currentIndexChanged(int)",(QObject *)param_1,
                   "1_SlotOnCurrentIndexChanged(int)",0);
  QObject::connect(*(QObject **)(param_1 + 0x70),"2textChanged(const QString&)",(QObject *)param_1,
                   "1_SlotOnTextChanged(const QString&)",0);
  puVar5 = (undefined8 *)FUN_140043d10(0x18);
  *(undefined8 **)(param_1 + 0x88) = puVar5;
  *puVar5 = 0;
  *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x11) = 0;
  *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x14) = 0;
  *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x13) = 0;
  *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x10) = 0;
  *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x12) = 0;
  *(undefined8 *)(*(longlong *)(param_1 + 0x88) + 8) = 0;
  param_1[0x94] = (QWidget)0x1;
  FUN_140056950(param_1);
  pQVar6 = (QSize *)QWidget::sizeHint(param_1);
  QWidget::resize(param_1,pQVar6);
  return param_1;
}


/* ================= FUN_1400568a0 @ 1400568a0  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_140056910 */

void FUN_1400568a0(QWidget *param_1)

{
  *(undefined ***)param_1 = TriggerWidget::vftable;
  *(undefined ***)(param_1 + 0x10) = TriggerWidget::vftable;
                    /* WARNING: Could not recover jumptable at 0x0001400568b5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QWidget::~QWidget(param_1);
  return;
}


/* ================= FUN_1400568bc @ 1400568bc  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_140056910 */

void FUN_1400568bc(longlong param_1)

{
  FUN_140056910(param_1 + -0x10);
  return;
}


/* ================= FUN_1400568d0 @ 1400568d0  size=53  callers=0 callees=2 ================= */
/* CALLEES: free, ~QDoubleValidator */

QDoubleValidator * FUN_1400568d0(QDoubleValidator *param_1,uint param_2)

{
  QDoubleValidator::~QDoubleValidator(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140056910 @ 140056910  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_1400568bc */
/* CALLEES: FUN_1400568a0, free */

void * FUN_140056910(void *param_1,ulonglong param_2)

{
  FUN_1400568a0();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140056950 @ 140056950  size=164  callers=4 callees=4 ================= */
/* CALLERS: FUN_14004cf00, FUN_14004d870, FUN_1400561b0, FUN_140056ab0 */
/* CALLEES: FUN_14002a320, setDisabled, setEnabled, setIcon */

void FUN_140056950(longlong param_1)

{
  QIcon *pQVar1;
  
  if (*(char *)(param_1 + 0x94) != '\0') {
    QAction::setEnabled(*(QAction **)(param_1 + 0x40),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x48),false);
    QAction::setEnabled(*(QAction **)(param_1 + 0x50),false);
    pQVar1 = (QIcon *)FUN_14002a320(0x1a);
    QAction::setIcon(*(QAction **)(param_1 + 0x40),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x18);
    QAction::setIcon(*(QAction **)(param_1 + 0x48),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x16);
    QAction::setIcon(*(QAction **)(param_1 + 0x50),pQVar1);
    QWidget::setDisabled(*(QWidget **)(param_1 + 0x60),true);
    QWidget::setDisabled(*(QWidget **)(param_1 + 0x70),true);
    *(undefined1 *)(param_1 + 0x94) = 0;
  }
  return;
}


/* ================= FUN_140056a00 @ 140056a00  size=164  callers=2 callees=3 ================= */
/* CALLERS: FUN_14004cf30, FUN_14004d870 */
/* CALLEES: FUN_14002a320, setEnabled, setIcon */

void FUN_140056a00(longlong param_1)

{
  QIcon *pQVar1;
  
  if (*(char *)(param_1 + 0x94) == '\0') {
    QAction::setEnabled(*(QAction **)(param_1 + 0x40),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x48),true);
    QAction::setEnabled(*(QAction **)(param_1 + 0x50),true);
    pQVar1 = (QIcon *)FUN_14002a320(0x19);
    QAction::setIcon(*(QAction **)(param_1 + 0x40),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x17);
    QAction::setIcon(*(QAction **)(param_1 + 0x48),pQVar1);
    pQVar1 = (QIcon *)FUN_14002a320(0x15);
    QAction::setIcon(*(QAction **)(param_1 + 0x50),pQVar1);
    QWidget::setEnabled(*(QWidget **)(param_1 + 0x60),true);
    QWidget::setEnabled(*(QWidget **)(param_1 + 0x70),true);
    *(undefined1 *)(param_1 + 0x94) = 1;
  }
  return;
}


/* ================= FUN_140056ab0 @ 140056ab0  size=30  callers=1 callees=1 ================= */
/* CALLERS: FUN_14004d540 */
/* CALLEES: FUN_140056950 */

void FUN_140056ab0(longlong param_1)

{
  FUN_140056950();
                    /* WARNING: Could not recover jumptable at 0x000140056ac7. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QComboBox::clear(*(QComboBox **)(param_1 + 0x60));
  return;
}


/* ================= FUN_140056ad0 @ 140056ad0  size=720  callers=1 callees=22 ================= */
/* CALLERS: FUN_14004d870 */
/* CALLEES: FUN_1400277b0, FUN_1400277f0, FUN_14003ef10, FUN_14004ea20, FUN_140056da0, FUN_140056ef0, FUN_140056f60, QIcon, QVariant, __security_check_cookie, blockSignals, clear, count, fromAscii_helper, insertItem, resize, setChecked, setCurrentIndex, setText, ~QIcon, ~QString, ~QVariant */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140056ad0(longlong param_1)

{
  longlong *plVar1;
  longlong lVar2;
  QComboBox *this;
  char cVar3;
  int iVar4;
  int iVar5;
  QVariant *pQVar6;
  QIcon *pQVar7;
  QSize *pQVar8;
  QAction *this_00;
  undefined1 auStackY_98 [32];
  Data *local_68;
  QIcon local_60 [8];
  QVariant local_58 [16];
  char local_48 [16];
  ulonglong local_38;
  
  local_38 = DAT_140096fe8 ^ (ulonglong)auStackY_98;
  local_68 = QString::fromAscii_helper("",-1);
  QLineEdit::setText(*(QLineEdit **)(param_1 + 0x80),(QString *)&local_68);
  QString::~QString((QString *)&local_68);
  cVar3 = FUN_14003ef10();
  if (cVar3 == '\0') {
    FUN_14004ea20(*(undefined8 *)(param_1 + 0x88));
    FUN_140056da0(param_1);
    iVar4 = FUN_140056ef0(param_1);
    if (iVar4 != 0) {
      QComboBox::clear(*(QComboBox **)(param_1 + 0x60));
      plVar1 = DAT_14286d3b8;
      iVar4 = 0;
      QObject::blockSignals(*(QObject **)(param_1 + 0x60),true);
      for (; plVar1 != (longlong *)0x0; plVar1 = (longlong *)plVar1[1]) {
        lVar2 = *plVar1;
        local_68 = QString::fromAscii_helper(*(char **)(lVar2 + 0x108),-1);
        pQVar6 = (QVariant *)QVariant::QVariant(local_58);
        this = *(QComboBox **)(param_1 + 0x60);
        iVar5 = QComboBox::count(this);
        pQVar7 = (QIcon *)QIcon::QIcon(local_60);
        QComboBox::insertItem(this,iVar5,pQVar7,(QString *)&local_68,pQVar6);
        QIcon::~QIcon(local_60);
        QVariant::~QVariant(local_58);
        QString::~QString((QString *)&local_68);
        if (**(longlong **)(param_1 + 0x88) != 0) {
          iVar5 = FUN_1400277b0(*(undefined8 *)(lVar2 + 0x108));
          iVar5 = FUN_1400277f0(*(undefined8 *)(lVar2 + 0x108),
                                *(undefined8 *)(**(longlong **)(param_1 + 0x88) + 0x108),
                                (longlong)iVar5);
          if (iVar5 == 0) {
            QComboBox::setCurrentIndex(*(QComboBox **)(param_1 + 0x60),iVar4);
          }
        }
        iVar4 = iVar4 + 1;
      }
      QObject::blockSignals(*(QObject **)(param_1 + 0x60),false);
      pQVar8 = (QSize *)(**(code **)(**(longlong **)(param_1 + 0x60) + 0x60))
                                  (*(longlong **)(param_1 + 0x60),local_60);
      QWidget::resize(*(QWidget **)(param_1 + 0x60),pQVar8);
      lVar2 = *(longlong *)(param_1 + 0x88);
      if (*(char *)(lVar2 + 0x10) != '\0') {
        if (*(char *)(lVar2 + 0x12) == '\0') {
          if (*(char *)(lVar2 + 0x13) == '\0') {
            return;
          }
          this_00 = *(QAction **)(param_1 + 0x48);
        }
        else if (*(char *)(lVar2 + 0x13) == '\0') {
          this_00 = *(QAction **)(param_1 + 0x40);
        }
        else {
          this_00 = *(QAction **)(param_1 + 0x50);
        }
        QAction::setChecked(this_00,true);
      }
    }
  }
  else {
    FUN_14004ea20(*(undefined8 *)(param_1 + 0x88));
    plVar1 = *(longlong **)(param_1 + 0x88);
    if ((char)plVar1[2] != '\0') {
      if (*(char *)((longlong)plVar1 + 0x14) == '\x01') {
        local_68 = QString::fromAscii_helper("Triggered",-1);
        QLineEdit::setText(*(QLineEdit **)(param_1 + 0x80),(QString *)&local_68);
        QString::~QString((QString *)&local_68);
      }
      else if (*(longlong *)(*plVar1 + 0x3c8) != 0) {
        FUN_140056f60(*plVar1,local_48,0x10);
        local_68 = QString::fromAscii_helper(local_48,-1);
        QLineEdit::setText(*(QLineEdit **)(param_1 + 0x80),(QString *)&local_68);
        QString::~QString((QString *)&local_68);
      }
    }
  }
  return;
}


/* ================= FUN_140056da0 @ 140056da0  size=145  callers=6 callees=5 ================= */
/* CALLERS: FUN_14004cf00, FUN_140056ad0, FUN_1400570b0, FUN_140057120, FUN_140057190, thunk_FUN_140056da0 */
/* CALLEES: FUN_14004ed00, currentIndex, text, toDouble, ~QString */

void FUN_140056da0(longlong param_1)

{
  ulonglong uVar1;
  ulonglong *puVar2;
  int iVar3;
  QString *this;
  ulonglong uVar4;
  double dVar5;
  QString local_res8 [32];
  
  iVar3 = QComboBox::currentIndex(*(QComboBox **)(param_1 + 0x60));
  uVar4 = 0;
  uVar1 = uVar4;
  puVar2 = DAT_14286d3b8;
  if (-1 < iVar3) {
    for (; (puVar2 != (ulonglong *)0x0 && ((int)uVar1 <= iVar3));
        uVar1 = (ulonglong)((int)uVar1 + 1)) {
      uVar4 = *puVar2;
      puVar2 = (ulonglong *)puVar2[1];
    }
  }
  **(ulonglong **)(param_1 + 0x88) = uVar4;
  this = (QString *)QLineEdit::text(*(QLineEdit **)(param_1 + 0x70));
  dVar5 = QString::toDouble(this,(bool *)0x0);
  *(double *)(*(longlong *)(param_1 + 0x88) + 8) = dVar5;
  QString::~QString(local_res8);
  FUN_14004ed00(*(undefined8 *)(param_1 + 0x88));
  return;
}


/* ================= FUN_140056e40 @ 140056e40  size=172  callers=1 callees=4 ================= */
/* CALLERS: FUN_140056ef0 */
/* CALLEES: FUN_140043d10, FUN_140043e60, FUN_14004ea20, memcmp */

bool FUN_140056e40(longlong param_1)

{
  undefined8 *puVar1;
  int iVar2;
  undefined8 *puVar3;
  bool bVar4;
  
  puVar3 = (undefined8 *)FUN_140043d10(0x18);
  FUN_14004ea20(puVar3);
  puVar1 = *(undefined8 **)(param_1 + 0x88);
  bVar4 = false;
  if ((((*(char *)((longlong)puVar1 + 0x11) == *(char *)((longlong)puVar3 + 0x11)) &&
       (*(char *)((longlong)puVar1 + 0x13) == *(char *)((longlong)puVar3 + 0x13))) &&
      (*(char *)((longlong)puVar1 + 0x12) == *(char *)((longlong)puVar3 + 0x12))) &&
     (((*(char *)(puVar1 + 2) == *(char *)(puVar3 + 2) && ((double)puVar1[1] == (double)puVar3[1]))
      && ((void *)*puVar1 != (void *)0x0)))) {
    if ((void *)*puVar3 != (void *)0x0) {
      iVar2 = memcmp((void *)*puVar1,(void *)*puVar3,0x3f0);
      bVar4 = iVar2 != 0;
    }
  }
  else {
    bVar4 = true;
  }
  FUN_140043e60(puVar3);
  return bVar4;
}


/* ================= FUN_140056ef0 @ 140056ef0  size=98  callers=1 callees=3 ================= */
/* CALLERS: FUN_140056ad0 */
/* CALLEES: FUN_14003ed50, FUN_140056e40, count */

bool FUN_140056ef0(longlong param_1)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  
  iVar1 = FUN_14003ed50();
  iVar2 = QComboBox::count(*(QComboBox **)(param_1 + 0x60));
  if ((iVar2 == 0) || (*(int *)(param_1 + 0x90) != iVar1)) {
    bVar3 = true;
  }
  else {
    iVar2 = FUN_140056e40(param_1);
    bVar3 = iVar2 != 0;
  }
  *(int *)(param_1 + 0x90) = iVar1;
  return bVar3;
}


/* ================= FUN_140056f60 @ 140056f60  size=277  callers=1 callees=3 ================= */
/* CALLERS: FUN_140056ad0 */
/* CALLEES: FUN_14002c8f0, FUN_14003eaa0, snprintf */

void FUN_140056f60(longlong param_1,undefined8 param_2,undefined4 param_3)

{
  float *pfVar1;
  float fVar2;
  char *pcVar3;
  undefined *puVar4;
  
  pfVar1 = *(float **)(param_1 + 0x3c8);
  switch(*(undefined4 *)(param_1 + 0x314)) {
  case 1:
    snprintf(param_2,param_3,&DAT_140078810,(double)*pfVar1);
    return;
  case 2:
    snprintf(param_2,param_3,&DAT_140078810,*(undefined8 *)pfVar1);
    return;
  default:
    goto switchD_140056f9f_caseD_2;
  case 5:
    pcVar3 = "True";
    if (*(char *)pfVar1 == '\0') {
      pcVar3 = "False";
    }
    FUN_14002c8f0(param_2,param_3,pcVar3);
    return;
  case 6:
    fVar2 = (float)(uint)*(byte *)pfVar1;
    puVar4 = &DAT_14005f57c;
    goto LAB_140057060;
  case 7:
    fVar2 = (float)(uint)*(ushort *)pfVar1;
    puVar4 = &DAT_14005f57c;
    goto LAB_140057060;
  case 8:
    fVar2 = *pfVar1;
    puVar4 = &DAT_14005f57c;
    goto LAB_140057060;
  case 10:
    fVar2 = (float)(int)*(char *)pfVar1;
    break;
  case 0xb:
    fVar2 = (float)(int)*(short *)pfVar1;
    break;
  case 0xc:
    fVar2 = *pfVar1;
    break;
  case 0xe:
    fVar2 = (float)FUN_14003eaa0(*pfVar1);
  }
  puVar4 = &DAT_140060a50;
LAB_140057060:
  FUN_14002c8f0(param_2,param_3,puVar4,fVar2);
switchD_140056f9f_caseD_2:
  return;
}


/* ================= FUN_1400570b0 @ 1400570b0  size=108  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003c7a0 */
/* CALLEES: FUN_140056da0, setChecked */

void FUN_1400570b0(longlong param_1,char param_2)

{
  if (param_2 == '\x01') {
    QAction::setChecked(*(QAction **)(param_1 + 0x40),false);
    QAction::setChecked(*(QAction **)(param_1 + 0x48),false);
    *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x10) = 1;
    *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x12) = 1;
    *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x13) = 1;
    FUN_140056da0(param_1);
    return;
  }
  *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x10) = 0;
  FUN_140056da0(param_1);
  return;
}


/* ================= FUN_140057120 @ 140057120  size=108  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003c7a0 */
/* CALLEES: FUN_140056da0, setChecked */

void FUN_140057120(longlong param_1,char param_2)

{
  if (param_2 == '\x01') {
    QAction::setChecked(*(QAction **)(param_1 + 0x40),false);
    QAction::setChecked(*(QAction **)(param_1 + 0x50),false);
    *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x10) = 1;
    *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x12) = 0;
    *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x13) = 1;
    FUN_140056da0(param_1);
    return;
  }
  *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x10) = 0;
  FUN_140056da0(param_1);
  return;
}


/* ================= FUN_140057190 @ 140057190  size=108  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003c7a0 */
/* CALLEES: FUN_140056da0, setChecked */

void FUN_140057190(longlong param_1,char param_2)

{
  if (param_2 == '\x01') {
    QAction::setChecked(*(QAction **)(param_1 + 0x48),false);
    QAction::setChecked(*(QAction **)(param_1 + 0x50),false);
    *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x10) = 1;
    *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x12) = 1;
    *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x13) = 0;
    FUN_140056da0(param_1);
    return;
  }
  *(undefined1 *)(*(longlong *)(param_1 + 0x88) + 0x10) = 0;
  FUN_140056da0(param_1);
  return;
}


/* ================= FUN_140057200 @ 140057200  size=10  callers=1 callees=0 ================= */
/* CALLERS: FUN_14003c7a0 */

void FUN_140057200(longlong param_1,int param_2)

{
  ulonglong uVar1;
  ulonglong *puVar2;
  int iVar3;
  QString *this;
  ulonglong uVar4;
  double dVar5;
  QString aQStackX_8 [32];
  
  if (param_2 == -1) {
    return;
  }
  iVar3 = QComboBox::currentIndex(*(QComboBox **)(param_1 + 0x60));
  uVar4 = 0;
  uVar1 = uVar4;
  puVar2 = DAT_14286d3b8;
  if (-1 < iVar3) {
    for (; (puVar2 != (ulonglong *)0x0 && ((int)uVar1 <= iVar3));
        uVar1 = (ulonglong)((int)uVar1 + 1)) {
      uVar4 = *puVar2;
      puVar2 = (ulonglong *)puVar2[1];
    }
  }
  **(ulonglong **)(param_1 + 0x88) = uVar4;
  this = (QString *)QLineEdit::text(*(QLineEdit **)(param_1 + 0x70));
  dVar5 = QString::toDouble(this,(bool *)0x0);
  *(double *)(*(longlong *)(param_1 + 0x88) + 8) = dVar5;
  QString::~QString(aQStackX_8);
  FUN_14004ed00(*(undefined8 *)(param_1 + 0x88));
  return;
}


/* ================= FUN_140057220 @ 140057220  size=1347  callers=1 callees=32 ================= */
/* CALLERS: FUN_14004d380 */
/* CALLEES: FUN_140028380, FUN_140032f90, FUN_140033060, FUN_140035cf0, FUN_140035d30, FUN_140035ff0, FUN_140036330, FUN_1400434e0, FUN_140043590, FUN_1400444f0, FUN_140057a70, FUN_140058d94, QDialog, QGridLayout, QVariant, __security_check_cookie, addWidget, connect, fromAscii_helper, hide, setDefault, setFixedSize, setItemData, setModal, setWindowFlags, setWindowTitle, show, sizeHint, thunk_FUN_14003ea60, windowFlags, ~QString, ~QVariant */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

QWidget * FUN_140057220(QWidget *param_1,undefined8 param_2,undefined8 *param_3,uint param_4)

{
  char *pcVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  QPushButton *this;
  QGridLayout *pQVar4;
  undefined4 *puVar5;
  QSize *pQVar6;
  uint uVar7;
  int iVar8;
  undefined1 auStackY_5b8 [32];
  undefined8 local_568;
  undefined4 local_558;
  undefined1 local_554 [4];
  QWidget *local_550;
  undefined1 local_548 [256];
  undefined1 local_448 [256];
  char local_348 [512];
  undefined1 local_148 [256];
  ulonglong local_48;
  
  local_48 = DAT_140096fe8 ^ (ulonglong)auStackY_5b8;
  local_558 = 0;
  local_550 = param_1;
  QDialog::QDialog((QDialog *)param_1,param_2,&local_558);
  *(undefined ***)param_1 = WelcomeDialog::vftable;
  *(undefined ***)(param_1 + 0x10) = WelcomeDialog::vftable;
  QWidget::hide(param_1);
  *(undefined8 *)(param_1 + 0x30) = 0;
  *(undefined8 *)(param_1 + 0x38) = 0;
  *(undefined8 *)(param_1 + 0x40) = 0;
  *(undefined8 *)(param_1 + 0x48) = 0;
  *(undefined8 *)(param_1 + 0x50) = 0;
  *(undefined8 *)(param_1 + 0x58) = 0;
  *(undefined8 *)(param_1 + 0x60) = 0;
  param_1[0x28] = (QWidget)0x0;
  *(undefined8 **)(param_1 + 0x58) = param_3;
  uVar2 = FUN_140035ff0(param_1,"Please select one of the following start options:");
  uVar3 = FUN_140036330(param_1,"Open recent project:",1,param_1);
  *(undefined8 *)(param_1 + 0x40) = uVar3;
  iVar8 = 0;
  local_568 = (QGridLayout *)FUN_140058d94(0x40);
  uVar3 = 0;
  if (local_568 != (QGridLayout *)0x0) {
    uVar3 = FUN_140032f90(local_568,param_1);
  }
  *(undefined8 *)(param_1 + 0x30) = uVar3;
  uVar7 = 0;
  if (param_4 != 0) {
    do {
      pcVar1 = (char *)*param_3;
      if (pcVar1 == (char *)0x0) break;
      iVar8 = iVar8 + 1;
      FUN_140043590(pcVar1,local_448,0x100);
      FUN_1400434e0(pcVar1,local_548,0x100);
      FUN_1400444f0(pcVar1,local_554,3,local_148);
      FUN_140028380(local_348,0x200,&DAT_1400631dc,local_448);
      FUN_140033060(*(undefined8 *)(param_1 + 0x30),local_348,pcVar1);
      QVariant::QVariant((QVariant *)&local_568,pcVar1);
      QComboBox::setItemData(*(QComboBox **)(param_1 + 0x30),uVar7,(QVariant *)&local_568,3);
      QVariant::~QVariant((QVariant *)&local_568);
      uVar7 = uVar7 + 1;
      param_3 = param_3 + 1;
    } while (uVar7 < param_4);
  }
  uVar3 = FUN_140035cf0(param_1,"Other...",param_1,"1_SlotOnBtnOther(void)");
  *(undefined8 *)(param_1 + 0x38) = uVar3;
  uVar3 = FUN_140036330(param_1,"Create new project",0,param_1);
  *(undefined8 *)(param_1 + 0x48) = uVar3;
  uVar3 = FUN_140035d30(param_1,"Do not show this message again.","2stateChanged(int)",param_1);
  *(undefined8 *)(param_1 + 0x50) = uVar3;
  this = (QPushButton *)FUN_140035cf0(param_1,"Start J-Scope",param_1,"1_SlotOnBtnStart(void)");
  QPushButton::setDefault(this,true);
  pQVar4 = (QGridLayout *)FUN_140058d94(0x20);
  local_568 = pQVar4;
  if (pQVar4 == (QGridLayout *)0x0) {
    pQVar4 = (QGridLayout *)0x0;
  }
  else {
    QGridLayout::QGridLayout(pQVar4,param_1);
    *(undefined ***)pQVar4 = QGridLayout::vftable;
    *(undefined ***)(pQVar4 + 0x10) = QGridLayout::vftable;
  }
  local_568 = (QGridLayout *)((ulonglong)local_568 & 0xffffffff00000000);
  QGridLayout::addWidget(pQVar4,uVar2,0,0);
  local_568 = (QGridLayout *)((ulonglong)local_568 & 0xffffffff00000000);
  QGridLayout::addWidget(pQVar4,*(undefined8 *)(param_1 + 0x40),1);
  uVar3 = 1;
  if (iVar8 != 0) {
    local_568 = (QGridLayout *)((ulonglong)local_568 & 0xffffffff00000000);
    QGridLayout::addWidget(pQVar4,*(undefined8 *)(param_1 + 0x30),1);
    uVar3 = 2;
  }
  local_568 = (QGridLayout *)((ulonglong)local_568 & 0xffffffff00000000);
  QGridLayout::addWidget(pQVar4,*(undefined8 *)(param_1 + 0x38),1,uVar3);
  local_568 = (QGridLayout *)((ulonglong)local_568 & 0xffffffff00000000);
  QGridLayout::addWidget(pQVar4,*(undefined8 *)(param_1 + 0x48),2);
  local_568 = (QGridLayout *)((ulonglong)local_568 & 0xffffffff00000000);
  QGridLayout::addWidget(pQVar4,*(undefined8 *)(param_1 + 0x50),3);
  local_568 = (QGridLayout *)((ulonglong)local_568 & 0xffffffff00000000);
  QGridLayout::addWidget(pQVar4,this,3);
  QObject::connect((QObject *)param_1,"2_SignalFinAsyncOpenProjFile(void)",(QObject *)param_1,
                   "1_SlotOnFinAsyncOpenProjFile(void)",2);
  puVar5 = (undefined4 *)QWidget::windowFlags(param_1);
  local_568 = (QGridLayout *)(CONCAT44(local_568._4_4_,*puVar5) & 0xfffffffffffeffff);
  QWidget::setWindowFlags(param_1,&local_568);
  FUN_140057a70(param_1);
  QDialog::setModal((QDialog *)param_1,true);
  uVar3 = thunk_FUN_14003ea60();
  FUN_140028380(local_348,0x200,"Welcome to %s",uVar3);
  local_568 = (QGridLayout *)QString::fromAscii_helper(local_348,-1);
  QWidget::setWindowTitle(param_1,(QString *)&local_568);
  QString::~QString((QString *)&local_568);
  pQVar6 = (QSize *)QDialog::sizeHint((QDialog *)param_1);
  QWidget::setFixedSize(param_1,pQVar6);
  QObject::connect((QObject *)param_1,"2rejected(void)",(QObject *)param_1,"1_SlotOnRejected(void)",
                   0);
  QWidget::show(param_1);
  return param_1;
}


/* ================= FUN_140057770 @ 140057770  size=28  callers=1 callees=0 ================= */
/* CALLERS: FUN_1400577a0 */

void FUN_140057770(QDialog *param_1)

{
  *(undefined ***)param_1 = WelcomeDialog::vftable;
  *(undefined ***)(param_1 + 0x10) = WelcomeDialog::vftable;
                    /* WARNING: Could not recover jumptable at 0x000140057785. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::~QDialog(param_1);
  return;
}


/* ================= FUN_14005778c @ 14005778c  size=9  callers=0 callees=1 ================= */
/* CALLEES: FUN_1400577a0 */

void FUN_14005778c(longlong param_1)

{
  FUN_1400577a0(param_1 + -0x10);
  return;
}


/* ================= FUN_1400577a0 @ 1400577a0  size=52  callers=1 callees=2 ================= */
/* CALLERS: FUN_14005778c */
/* CALLEES: FUN_140057770, free */

void * FUN_1400577a0(void *param_1,ulonglong param_2)

{
  FUN_140057770();
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_1400577f0 @ 1400577f0  size=43  callers=3 callees=1 ================= */
/* CALLERS: FUN_1400578b0, FUN_140057a40, FUN_140057a60 */
/* CALLEES: close */

void FUN_1400577f0(QWidget *param_1,int param_2)

{
  QWidget::close(param_1);
                    /* WARNING: Could not recover jumptable at 0x000140057814. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  QDialog::setResult((QDialog *)param_1,param_2);
  return;
}


/* ================= FUN_140057820 @ 140057820  size=142  callers=1 callees=3 ================= */
/* CALLERS: FUN_14003caf0 */
/* CALLEES: FUN_140036720, FUN_14003e420, __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_140057820(longlong param_1)

{
  int iVar1;
  undefined1 auStack_168 [32];
  undefined1 *local_148;
  char *local_140;
  undefined8 local_138;
  undefined1 local_128 [272];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_168;
  local_138 = 0;
  local_140 = "All files (*.*);;J-Scope project files (*.jscope)";
  local_148 = local_128;
  local_128[0] = 0;
  iVar1 = FUN_140036720(param_1,local_128,0x104,"Open J-Scope project file");
  if (-1 < iVar1) {
    FUN_14003e420(param_1,local_128,param_1 + 0x60,&DAT_140057ae0);
  }
  return;
}


/* ================= FUN_1400578b0 @ 1400578b0  size=322  callers=1 callees=13 ================= */
/* CALLERS: FUN_14003caf0 */
/* CALLEES: FUN_14002a410, FUN_1400331b0, FUN_140036720, FUN_14003e420, FUN_140043a00, FUN_140043d20, FUN_1400577f0, FUN_140058d94, __security_check_cookie, count, exec, isChecked, memset */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_1400578b0(longlong param_1)

{
  bool bVar1;
  int iVar2;
  void *_Dst;
  longlong *plVar3;
  QDialog *this;
  undefined1 *puVar4;
  undefined8 uVar5;
  undefined1 auStack_178 [32];
  undefined1 *local_158;
  char *local_150;
  char *local_148;
  longlong local_138;
  undefined1 local_128 [272];
  ulonglong local_18;
  
  local_18 = DAT_140096fe8 ^ (ulonglong)auStack_178;
  _Dst = (void *)FUN_140043d20(0x5e8);
  memset(_Dst,0,0x5e8);
  bVar1 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0x40));
  if (bVar1) {
    iVar2 = QComboBox::count(*(QComboBox **)(param_1 + 0x30));
    if (iVar2 == 0) {
      FUN_140043a00(local_128,0x104);
      local_148 = "J-Scope project files (*.jscope)";
      local_150 = "All files (*.*);;J-Scope project files (*.jscope)";
      local_158 = local_128;
      iVar2 = FUN_140036720(param_1,local_128,0x104,"Open J-Scope project file");
      if (iVar2 < 0) {
        uVar5 = 3;
        goto LAB_1400579c7;
      }
      puVar4 = local_128;
    }
    else {
      plVar3 = (longlong *)FUN_1400331b0(*(undefined8 *)(param_1 + 0x30));
      puVar4 = (undefined1 *)*plVar3;
    }
    FUN_14003e420(param_1,puVar4,param_1 + 0x60,&DAT_140057ae0);
  }
  else {
    uVar5 = 1;
    local_138 = FUN_140058d94(0x148);
    this = (QDialog *)0x0;
    if (local_138 != 0) {
      this = (QDialog *)FUN_14002a410(local_138,param_1,0);
    }
    iVar2 = QDialog::exec(this);
    if (iVar2 != 1) {
      return;
    }
LAB_1400579c7:
    FUN_1400577f0(param_1,uVar5);
  }
  return;
}


/* ================= FUN_140057a00 @ 140057a00  size=62  callers=1 callees=2 ================= */
/* CALLERS: FUN_14003caf0 */
/* CALLEES: FUN_14003e7a0, isChecked */

void FUN_140057a00(longlong param_1)

{
  bool bVar1;
  
  bVar1 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0x50));
  if (bVar1) {
    bVar1 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0x40));
    FUN_14003e7a0(2 - (uint)bVar1);
    return;
  }
  FUN_14003e7a0(0);
  return;
}


/* ================= FUN_140057a40 @ 140057a40  size=14  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003caf0 */
/* CALLEES: FUN_1400577f0 */

void FUN_140057a40(longlong param_1)

{
  if (*(int *)(param_1 + 0x60) == 0) {
    FUN_1400577f0(param_1,0);
    return;
  }
  return;
}


/* ================= FUN_140057a60 @ 140057a60  size=10  callers=1 callees=1 ================= */
/* CALLERS: FUN_14003caf0 */
/* CALLEES: FUN_1400577f0 */

void FUN_140057a60(undefined8 param_1)

{
  FUN_1400577f0(param_1,3);
  return;
}


/* ================= FUN_140057a70 @ 140057a70  size=111  callers=2 callees=3 ================= */
/* CALLERS: FUN_140057220, thunk_FUN_140057a70 */
/* CALLEES: count, isChecked, setFixedSize */

void FUN_140057a70(QWidget *param_1)

{
  bool bVar1;
  int iVar2;
  QSize *pQVar3;
  undefined8 uVar4;
  undefined1 local_res8 [8];
  
  bVar1 = QAbstractButton::isChecked(*(QAbstractButton **)(param_1 + 0x40));
  (**(code **)(**(longlong **)(param_1 + 0x38) + 0x58))(*(longlong **)(param_1 + 0x38),bVar1);
  iVar2 = QComboBox::count(*(QComboBox **)(param_1 + 0x30));
  if ((bVar1) && (iVar2 != 0)) {
    uVar4 = 1;
  }
  else {
    uVar4 = 0;
  }
  (**(code **)(**(longlong **)(param_1 + 0x30) + 0x58))(*(longlong **)(param_1 + 0x30),uVar4);
  pQVar3 = (QSize *)(**(code **)(*(longlong *)param_1 + 0x68))(param_1,local_res8);
  QWidget::setFixedSize(param_1,pQVar3);
  return;
}


/* ================= __GSHandlerCheck @ 140058a10  size=29  callers=0 callees=1 ================= */
/* CALLEES: __GSHandlerCheckCommon */

/* Library Function - Single Match
    __GSHandlerCheck
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined8
__GSHandlerCheck(undefined8 param_1,undefined8 param_2,undefined8 param_3,longlong param_4)

{
  __GSHandlerCheckCommon(param_2,param_4,*(undefined8 *)(param_4 + 0x38));
  return 1;
}


/* ================= __GSHandlerCheckCommon @ 140058a30  size=91  callers=2 callees=1 ================= */
/* CALLERS: FID_conflict:__GSHandlerCheck_EH, __GSHandlerCheck */
/* CALLEES: __security_check_cookie */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* Library Function - Single Match
    __GSHandlerCheckCommon
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

ulonglong __GSHandlerCheckCommon(undefined8 param_1,longlong param_2)

{
  ulonglong uVar1;
  ulonglong uVar2;
  
  uVar2 = (ulonglong)*(uint *)(*(longlong *)(param_2 + 0x10) + 8);
  uVar1 = *(ulonglong *)(param_2 + 8);
  if ((*(byte *)(uVar2 + 3 + uVar1) & 0xf) != 0) {
    uVar1 = (ulonglong)(*(byte *)(uVar2 + 3 + uVar1) & 0xfffffff0);
  }
  return uVar1;
}


/* ================= __security_check_cookie @ 140058aa0  size=30  callers=141 callees=1 ================= */
/* CALLERS: FUN_140002fb0, FUN_140004070, FUN_1400070e0, FUN_1400080f0, FUN_140009e60, FUN_14000a010, FUN_14000aaf0, FUN_14000b630, FUN_14000b830, FUN_14000bb50, FUN_14000cb60, FUN_14000ccf0, FUN_14000cf50, FUN_14000d0c0, FUN_14000d240, FUN_14000d9a0, FUN_14000e0d0, FUN_14000f390, FUN_14000fd90, FUN_140010610, FUN_1400125b0, FUN_14001b330, FUN_14001b3f0, FUN_1400241f0, FUN_1400273a0, FUN_140027a00, FUN_1400293b0, FUN_140029490, FUN_1400296f0, FUN_14002a1f0, FUN_14002b940, FUN_14002bb30, FUN_14002bc30, FUN_14002be90, FUN_14002c250, FUN_14002ce80, FUN_14002ea10, FUN_14002fad0, FUN_140030270, FUN_140030e00, FUN_140030f10, FUN_140033540, FUN_140033b90, FUN_140033e50, FUN_1400341e0, FUN_1400343e0, FUN_140034630, FUN_140034d30, FUN_140034fd0, FUN_1400351b0, FUN_1400352e0, FUN_1400368e0, FUN_140036f80, FUN_140037720, FUN_1400377e0, FUN_1400397c0, FUN_140039850, FUN_1400398d0, FUN_140039960, FUN_140039b60, FUN_14003aca0, FUN_14003cc50, FUN_14003cec0, FUN_14003d210, FUN_14003d9b0, FUN_14003db70, FUN_14003dcb0, FUN_14003dd90, FUN_14003f120, FUN_14003f1e0, FUN_14003f280, FUN_14003f580, FUN_14003f930, FUN_14003fc90, FUN_140040320, FUN_1400404c0, FUN_140040ed0, FUN_140041400, FUN_140041780, FUN_1400419f0, FUN_140041cd0, FUN_140041f30, FUN_1400423e0, FUN_1400427b0, FUN_140043210, FUN_140043430, FUN_1400434e0, FUN_140043590, FUN_140043660, FUN_140043a00, FUN_140043ad0, FUN_140043d20, FUN_140043fd0, FUN_140044520, FUN_140044750, FUN_140045670, FUN_140046e80, FUN_140047020, FUN_140047390, FUN_1400476d0, FUN_1400479a0, FUN_140048100, FUN_140048540, FUN_140048680, FUN_140048a70, FUN_140048bb0, FUN_140048d10, FUN_140048e60, FUN_14004a870, FUN_14004b290, FUN_14004b640, FUN_14004cf60, FUN_14004df50, FUN_14004e4b0, FUN_14004e610, FUN_14004ea70, FUN_14004eda0, FUN_14004efc0, FUN_14004f0c0, FUN_14004f1e0, FUN_14004f590, FUN_14004f6c0, FUN_14004f9b0, FUN_14004fa30, FUN_14004fab0, FUN_14004fb30, FUN_14004fd40, FUN_140050030, FUN_1400504d0, FUN_140050d00, FUN_1400515f0, FUN_140053000, FUN_1400532c0, FUN_140053680, FUN_140053760, FUN_140054790, FUN_140056ad0, FUN_140057220, FUN_140057820, FUN_1400578b0, __GSHandlerCheckCommon */
/* CALLEES: FUN_140059108 */

/* WARNING: This is an inlined function */

void __cdecl __security_check_cookie(uintptr_t _StackCookie)

{
  if ((_StackCookie == DAT_140096fe8) && ((short)(_StackCookie >> 0x30) == 0)) {
    return;
  }
  FUN_140059108(_StackCookie);
  return;
}


/* ================= __chkstk @ 140058ad0  size=78  callers=4 callees=0 ================= */
/* CALLERS: FUN_14000f390, FUN_140010610, FUN_1400241f0, FUN_1400419f0 */

/* WARNING: This is an inlined function */
/* Library Function - Single Match
    __chkstk
   
   Libraries: Visual Studio 2005, Visual Studio 2008, Visual Studio 2010, Visual Studio 2012 */

void __chkstk(void)

{
  undefined1 *in_RAX;
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined1 local_res8 [32];
  
  puVar1 = local_res8 + -(longlong)in_RAX;
  if (local_res8 < in_RAX) {
    puVar1 = (undefined1 *)0x0;
  }
  if (puVar1 < StackLimit) {
    puVar2 = StackLimit;
    do {
      puVar2 = puVar2 + -0x1000;
      *puVar2 = 0;
    } while ((undefined1 *)((ulonglong)puVar1 & 0xfffffffffffff000) != puVar2);
  }
  return;
}


/* ================= __isa_available_init @ 140058b20  size=417  callers=1 callees=0 ================= */
/* CALLERS: __scrt_initialize_crt */

/* WARNING: Removing unreachable block (ram,0x000140058bdf) */
/* WARNING: Removing unreachable block (ram,0x000140058b5a) */
/* WARNING: Removing unreachable block (ram,0x000140058b33) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Library Function - Single Match
    __isa_available_init
   
   Library: Visual Studio 2019 Release */

undefined8 __isa_available_init(void)

{
  int *piVar1;
  uint *puVar2;
  longlong lVar3;
  uint uVar4;
  undefined8 uVar5;
  byte bVar6;
  uint uVar7;
  uint uVar8;
  byte in_XCR0;
  
  piVar1 = (int *)cpuid_basic_info(0);
  uVar8 = 0;
  puVar2 = (uint *)cpuid_Version_info(1);
  uVar4 = puVar2[3];
  if ((piVar1[1] == 0x756e6547 && piVar1[3] == 0x6c65746e) && piVar1[2] == 0x49656e69) {
    _DAT_140096ff8 = 0xffffffffffffffff;
    uVar7 = *puVar2 & 0xfff3ff0;
    if ((((uVar7 == 0x106c0) || (uVar7 == 0x20660)) || (uVar7 == 0x20670)) ||
       ((uVar7 - 0x30650 < 0x21 &&
        ((0x100010001U >> ((ulonglong)(uVar7 - 0x30650) & 0x3f) & 1) != 0)))) {
      DAT_14286cd8c = DAT_14286cd8c | 1;
    }
  }
  if (6 < *piVar1) {
    lVar3 = cpuid_Extended_Feature_Enumeration_info(7);
    uVar8 = *(uint *)(lVar3 + 4);
    if ((uVar8 >> 9 & 1) != 0) {
      DAT_14286cd8c = DAT_14286cd8c | 2;
    }
  }
  _DAT_140096ff0 = 1;
  DAT_140096ff4 = 2;
  if ((uVar4 >> 0x14 & 1) != 0) {
    _DAT_140096ff0 = 2;
    DAT_140096ff4 = 6;
    if (((uVar4 >> 0x1b & 1) != 0) && ((uVar4 >> 0x1c & 1) != 0)) {
      uVar5 = xinuse(0);
      bVar6 = in_XCR0 & (byte)uVar5;
      if ((bVar6 & 6) == 6) {
        DAT_140096ff4 = 0xe;
        _DAT_140096ff0 = 3;
        if ((uVar8 & 0x20) != 0) {
          _DAT_140096ff0 = 5;
          DAT_140096ff4 = 0x2e;
          if (((uVar8 & 0xd0030000) == 0xd0030000) && ((bVar6 & 0xe0) == 0xe0)) {
            DAT_140096ff4 = 0x6e;
            _DAT_140096ff0 = 6;
          }
        }
      }
    }
  }
  return 0;
}


/* ================= `eh_vector_destructor_iterator' @ 140058cc4  size=107  callers=0 callees=1 ================= */
/* CALLEES: _guard_dispatch_icall */

/* WARNING: Function: _guard_dispatch_icall replaced with injection: guard_dispatch_icall */
/* Library Function - Single Match
    void __cdecl `eh vector destructor iterator'(void * __ptr64,unsigned __int64,unsigned
   __int64,void (__cdecl*)(void * __ptr64))
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl
_eh_vector_destructor_iterator_
          (void *param_1,__uint64 param_2,__uint64 param_3,_func_void_void_ptr *param_4)

{
  void *pvVar1;
  
  pvVar1 = (void *)(param_2 * param_3 + (longlong)param_1);
  while( true ) {
    if (param_3 == 0) break;
    pvVar1 = (void *)((longlong)pvVar1 - param_2);
    (*param_4)(pvVar1);
    param_3 = param_3 - 1;
  }
  return;
}


/* ================= __ArrayUnwind @ 140058d30  size=97  callers=1 callees=1 ================= */
/* CALLERS: FUN_14005b9ed */
/* CALLEES: _guard_dispatch_icall */

/* WARNING: Function: _guard_dispatch_icall replaced with injection: guard_dispatch_icall */
/* Library Function - Single Match
    void __cdecl __ArrayUnwind(void * __ptr64,unsigned __int64,unsigned __int64,void (__cdecl*)(void
   * __ptr64))
   
   Library: Visual Studio 2019 Release */

void __cdecl
__ArrayUnwind(void *param_1,__uint64 param_2,__uint64 param_3,_func_void_void_ptr *param_4)

{
  __uint64 _Var1;
  
  for (_Var1 = 0; _Var1 != param_3; _Var1 = _Var1 + 1) {
    param_1 = (void *)((longlong)param_1 - param_2);
    (*param_4)(param_1);
  }
  return;
}


/* ================= FUN_140058d94 @ 140058d94  size=58  callers=57 callees=4 ================= */
/* CALLERS: FUN_140029f00, FUN_140029fa0, FUN_14002a0c0, FUN_14002a340, FUN_14002a410, FUN_14002bc30, FUN_14002ce80, FUN_14002fe40, FUN_140032220, FUN_140034630, FUN_140035c20, FUN_140035d30, FUN_140035e00, FUN_140035e90, FUN_140035f30, FUN_140035ff0, FUN_1400360a0, FUN_140036150, FUN_140036210, FUN_140036330, FUN_140036410, FUN_140036640, FUN_1400373a0, FUN_140038090, FUN_140038440, FUN_1400388c0, FUN_140038ab0, FUN_140044bb0, FUN_1400479a0, FUN_140048350, FUN_140048490, FUN_1400487b0, FUN_140048bb0, FUN_140048e60, FUN_14004af10, FUN_14004b8b0, FUN_14004c520, FUN_14004c6a0, FUN_14004d0f0, FUN_14004d170, FUN_14004d290, FUN_14004d380, FUN_14004d4e0, FUN_14004d6c0, FUN_14004d9c0, FUN_140050140, FUN_1400504d0, FUN_1400507f0, FUN_140051790, FUN_1400521c0, FUN_140053000, FUN_1400532c0, FUN_140053760, FUN_140054790, FUN_1400561b0, FUN_140057220, FUN_1400578b0 */
/* CALLEES: FUN_140059720, FUN_140059740, _callnewh, malloc */

void FUN_140058d94(size_t param_1)

{
  int iVar1;
  void *pvVar2;
  
  do {
    pvVar2 = malloc(param_1);
    if (pvVar2 != (void *)0x0) {
      return;
    }
    iVar1 = _callnewh(param_1);
  } while (iVar1 != 0);
  if (param_1 == 0xffffffffffffffff) {
                    /* WARNING: Subroutine does not return */
    FUN_140059740();
  }
                    /* WARNING: Subroutine does not return */
  FUN_140059720();
}


/* ================= __scrt_acquire_startup_lock @ 140058dd8  size=57  callers=1 callees=1 ================= */
/* CALLERS: FUN_140059454 */
/* CALLEES: __scrt_is_ucrt_dll_in_use */

/* Library Function - Single Match
    __scrt_acquire_startup_lock
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined8 __scrt_acquire_startup_lock(void)

{
  void *pvVar1;
  int iVar2;
  void *pvVar3;
  undefined8 uVar4;
  bool bVar5;
  
  iVar2 = __scrt_is_ucrt_dll_in_use();
  if (iVar2 == 0) {
LAB_140058e06:
    uVar4 = 0;
  }
  else {
    do {
      pvVar3 = (void *)0x0;
      LOCK();
      bVar5 = DAT_14286cd98 == (void *)0x0;
      pvVar1 = StackBase;
      if (!bVar5) {
        pvVar3 = DAT_14286cd98;
        pvVar1 = DAT_14286cd98;
      }
      DAT_14286cd98 = pvVar1;
      UNLOCK();
      if (bVar5) goto LAB_140058e06;
    } while (StackBase != pvVar3);
    uVar4 = 1;
  }
  return uVar4;
}


/* ================= __scrt_initialize_crt @ 140058e14  size=73  callers=1 callees=2 ================= */
/* CALLERS: FUN_140059454 */
/* CALLEES: FUN_140059ab0, __isa_available_init */

/* Library Function - Single Match
    __scrt_initialize_crt
   
   Library: Visual Studio 2019 Release */

undefined8 __scrt_initialize_crt(int param_1)

{
  char cVar1;
  
  if (param_1 == 0) {
    DAT_14286cda0 = 1;
  }
  __isa_available_init();
  cVar1 = FUN_140059ab0();
  if (cVar1 != '\0') {
    cVar1 = FUN_140059ab0();
    if (cVar1 != '\0') {
      return 1;
    }
    FUN_140059ab0(0);
  }
  return 0;
}


/* ================= __scrt_initialize_onexit_tables @ 140058e60  size=139  callers=1 callees=3 ================= */
/* CALLERS: FUN_140059370 */
/* CALLEES: FUN_140059798, __scrt_is_ucrt_dll_in_use, _initialize_onexit_table */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Library Function - Single Match
    __scrt_initialize_onexit_tables
   
   Library: Visual Studio 2019 Release */

undefined8 __scrt_initialize_onexit_tables(uint param_1)

{
  code *pcVar1;
  int iVar2;
  undefined8 uVar3;
  
  if (DAT_14286cda1 == '\0') {
    if (1 < param_1) {
      FUN_140059798(5);
      pcVar1 = (code *)swi(3);
      uVar3 = (*pcVar1)();
      return uVar3;
    }
    iVar2 = __scrt_is_ucrt_dll_in_use();
    if ((iVar2 == 0) || (param_1 != 0)) {
      DAT_14286cda8 = 0xffffffffffffffff;
      uRam000000014286cdb0 = 0xffffffffffffffff;
      _DAT_14286cdb8 = 0xffffffffffffffff;
      _DAT_14286cdc0 = 0xffffffffffffffff;
      uRam000000014286cdc8 = 0xffffffffffffffff;
      _DAT_14286cdd0 = 0xffffffffffffffff;
    }
    else {
      iVar2 = _initialize_onexit_table(&DAT_14286cda8);
      if ((iVar2 != 0) || (iVar2 = _initialize_onexit_table(&DAT_14286cdc0), iVar2 != 0)) {
        return 0;
      }
    }
    DAT_14286cda1 = '\x01';
  }
  return 1;
}


/* ================= __scrt_is_nonwritable_in_current_image @ 140058eec  size=150  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059454 */

/* WARNING: Removing unreachable block (ram,0x000140058f79) */
/* WARNING: Enum "SectionFlags": Some values do not have unique names */
/* Library Function - Single Match
    __scrt_is_nonwritable_in_current_image
   
   Library: Visual Studio 2019 Release */

ulonglong __scrt_is_nonwritable_in_current_image(longlong param_1)

{
  ulonglong uVar1;
  uint7 uVar2;
  IMAGE_SECTION_HEADER *pIVar3;
  
  uVar1 = 0;
  for (pIVar3 = &IMAGE_SECTION_HEADER_140000218; pIVar3 != (IMAGE_SECTION_HEADER *)&DAT_140000308;
      pIVar3 = pIVar3 + 1) {
    if (((ulonglong)(uint)pIVar3->VirtualAddress <= param_1 - 0x140000000U) &&
       (uVar1 = (ulonglong)((pIVar3->Misc).PhysicalAddress + pIVar3->VirtualAddress),
       param_1 - 0x140000000U < uVar1)) goto LAB_140058f62;
  }
  pIVar3 = (IMAGE_SECTION_HEADER *)0x0;
LAB_140058f62:
  if (pIVar3 == (IMAGE_SECTION_HEADER *)0x0) {
    uVar1 = uVar1 & 0xffffffffffffff00;
  }
  else {
    uVar2 = (uint7)(uVar1 >> 8);
    if ((int)pIVar3->Characteristics < 0) {
      uVar1 = (ulonglong)uVar2 << 8;
    }
    else {
      uVar1 = CONCAT71(uVar2,1);
    }
  }
  return uVar1;
}


/* ================= __scrt_release_startup_lock @ 140058f84  size=36  callers=1 callees=1 ================= */
/* CALLERS: FUN_140059454 */
/* CALLEES: __scrt_is_ucrt_dll_in_use */

/* Library Function - Single Match
    __scrt_release_startup_lock
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __scrt_release_startup_lock(char param_1)

{
  int iVar1;
  
  iVar1 = __scrt_is_ucrt_dll_in_use();
  if ((iVar1 != 0) && (param_1 == '\0')) {
    LOCK();
    DAT_14286cd98 = 0;
    UNLOCK();
  }
  return;
}


/* ================= __scrt_uninitialize_crt @ 140058fa8  size=41  callers=1 callees=1 ================= */
/* CALLERS: FUN_140059454 */
/* CALLEES: FUN_140059ab0 */

/* Library Function - Single Match
    __scrt_uninitialize_crt
   
   Library: Visual Studio 2019 Release */

undefined1 __scrt_uninitialize_crt(undefined1 param_1,char param_2)

{
  if ((DAT_14286cda0 == '\0') || (param_2 == '\0')) {
    FUN_140059ab0();
    FUN_140059ab0(param_1);
  }
  return 1;
}


/* ================= _onexit @ 140058fd4  size=58  callers=1 callees=2 ================= */
/* CALLERS: atexit */
/* CALLEES: _crt_atexit, _register_onexit_function */

/* Library Function - Single Match
    _onexit
   
   Library: Visual Studio 2019 Release */

_onexit_t __cdecl _onexit(_onexit_t _Func)

{
  int iVar1;
  _onexit_t p_Var2;
  
  if (DAT_14286cda8 == -1) {
    iVar1 = _crt_atexit();
  }
  else {
    iVar1 = _register_onexit_function(&DAT_14286cda8);
  }
  p_Var2 = (_onexit_t)0x0;
  if (iVar1 == 0) {
    p_Var2 = _Func;
  }
  return p_Var2;
}


/* ================= atexit @ 140059010  size=23  callers=6 callees=1 ================= */
/* CALLERS: FUN_140001000, FUN_140001290, FUN_140001340, FUN_1400013a0, FUN_14004ef00, FUN_140059370 */
/* CALLEES: _onexit */

/* Library Function - Single Match
    atexit
   
   Library: Visual Studio 2019 Release */

int __cdecl atexit(_func_5014 *param_1)

{
  _onexit_t p_Var1;
  
  p_Var1 = _onexit((_onexit_t)param_1);
  return (p_Var1 != (_onexit_t)0x0) - 1;
}


/* ================= FUN_140059028 @ 140059028  size=43  callers=0 callees=1 ================= */
/* CALLEES: free */

undefined8 * FUN_140059028(undefined8 *param_1,ulonglong param_2)

{
  *param_1 = type_info::vftable;
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FID_conflict:__GSHandlerCheck_EH @ 140059054  size=127  callers=0 callees=2 ================= */
/* CALLEES: __CxxFrameHandler4, __GSHandlerCheckCommon */

/* Library Function - Multiple Matches With Different Base Names
    __GSHandlerCheck_EH
    __GSHandlerCheck_EH4
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void FID_conflict___GSHandlerCheck_EH
               (longlong param_1,undefined8 param_2,undefined8 param_3,longlong param_4)

{
  longlong lVar1;
  
  lVar1 = *(longlong *)(param_4 + 0x38);
  __GSHandlerCheckCommon(param_2,param_4);
  if ((*(uint *)(lVar1 + 4) & ((*(uint *)(param_1 + 4) & 0x66) != 0) + 1) != 0) {
    __CxxFrameHandler4(param_1,param_2,param_3,param_4);
  }
  return;
}


/* ================= __raise_securityfailure @ 1400590d4  size=52  callers=2 callees=3 ================= */
/* CALLERS: FUN_140059108, FUN_1400591f0 */
/* CALLEES: GetCurrentProcess, SetUnhandledExceptionFilter, UnhandledExceptionFilter */

/* Library Function - Single Match
    __raise_securityfailure
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __raise_securityfailure(_EXCEPTION_POINTERS *param_1)

{
  HANDLE pvVar1;
  
  SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)0x0);
  UnhandledExceptionFilter(param_1);
  pvVar1 = GetCurrentProcess();
                    /* WARNING: Could not recover jumptable at 0x000140059101. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  TerminateProcess(pvVar1,0xc0000409);
  return;
}


/* ================= FUN_140059108 @ 140059108  size=210  callers=1 callees=3 ================= */
/* CALLERS: __security_check_cookie */
/* CALLEES: IsProcessorFeaturePresent, __raise_securityfailure, capture_previous_context */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140059108(void)

{
  code *pcVar1;
  BOOL BVar2;
  undefined1 *puVar3;
  undefined1 auStack_38 [8];
  undefined1 auStack_30 [48];
  
  puVar3 = auStack_38;
  BVar2 = IsProcessorFeaturePresent(0x17);
  if (BVar2 != 0) {
    pcVar1 = (code *)swi(0x29);
    (*pcVar1)(2);
    puVar3 = auStack_30;
  }
  *(undefined8 *)(puVar3 + -8) = 0x140059133;
  capture_previous_context(&DAT_14286ce80);
  _DAT_14286cdf0 = *(undefined8 *)(puVar3 + 0x38);
  _DAT_14286cf18 = puVar3 + 0x40;
  _DAT_14286cf00 = *(undefined8 *)(puVar3 + 0x40);
  _DAT_14286cde0 = 0xc0000409;
  _DAT_14286cde4 = 1;
  _DAT_14286cdf8 = 1;
  DAT_14286ce00 = 2;
  *(undefined8 *)(puVar3 + 0x20) = DAT_140096fe8;
  *(undefined8 *)(puVar3 + 0x28) = DAT_140096fe0;
  *(undefined8 *)(puVar3 + -8) = 0x1400591d5;
  DAT_14286cf78 = _DAT_14286cdf0;
  __raise_securityfailure(&PTR_DAT_140078f50);
  return;
}


/* ================= FUN_1400591dc @ 1400591dc  size=19  callers=4 callees=1 ================= */
/* CALLERS: FUN_1400368e0, FUN_14003f280, FUN_1400423e0, FUN_140043210 */
/* CALLEES: FUN_1400591f0 */

void FUN_1400591dc(void)

{
  FUN_1400591f0(8);
  return;
}


/* ================= FUN_1400591f0 @ 1400591f0  size=156  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400591dc */
/* CALLEES: IsProcessorFeaturePresent, __raise_securityfailure, capture_current_context */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1400591f0(undefined4 param_1)

{
  code *pcVar1;
  BOOL BVar2;
  undefined1 *puVar3;
  undefined1 auStack_28 [8];
  undefined1 auStack_20 [32];
  
  puVar3 = auStack_28;
  BVar2 = IsProcessorFeaturePresent(0x17);
  if (BVar2 != 0) {
    pcVar1 = (code *)swi(0x29);
    (*pcVar1)(param_1);
    puVar3 = auStack_20;
  }
  *(undefined8 *)(puVar3 + -8) = 0x14005921b;
  capture_current_context(&DAT_14286ce80);
  _DAT_14286cdf0 = *(undefined8 *)(puVar3 + 0x28);
  _DAT_14286cf18 = puVar3 + 0x30;
  _DAT_14286cde0 = 0xc0000409;
  _DAT_14286cde4 = 1;
  _DAT_14286cdf8 = 1;
  DAT_14286ce00 = (ulonglong)*(uint *)(puVar3 + 0x30);
  *(undefined8 *)(puVar3 + -8) = 0x140059287;
  DAT_14286cf78 = _DAT_14286cdf0;
  __raise_securityfailure(&PTR_DAT_140078f50);
  return;
}


/* ================= capture_current_context @ 14005928c  size=109  callers=1 callees=3 ================= */
/* CALLERS: FUN_1400591f0 */
/* CALLEES: RtlCaptureContext, RtlLookupFunctionEntry, RtlVirtualUnwind */

/* Library Function - Single Match
    capture_current_context
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void capture_current_context(PCONTEXT param_1)

{
  DWORD64 ControlPc;
  PRUNTIME_FUNCTION FunctionEntry;
  DWORD64 local_res8;
  ulonglong local_res10;
  PVOID local_res18;
  
  RtlCaptureContext();
  ControlPc = param_1->Rip;
  FunctionEntry = RtlLookupFunctionEntry(ControlPc,&local_res8,(PUNWIND_HISTORY_TABLE)0x0);
  if (FunctionEntry != (PRUNTIME_FUNCTION)0x0) {
    RtlVirtualUnwind(0,local_res8,ControlPc,FunctionEntry,param_1,&local_res18,&local_res10,
                     (PKNONVOLATILE_CONTEXT_POINTERS)0x0);
  }
  return;
}


/* ================= capture_previous_context @ 1400592fc  size=113  callers=1 callees=3 ================= */
/* CALLERS: FUN_140059108 */
/* CALLEES: RtlCaptureContext, RtlLookupFunctionEntry, RtlVirtualUnwind */

/* Library Function - Single Match
    capture_previous_context
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void capture_previous_context(PCONTEXT param_1)

{
  DWORD64 ControlPc;
  PRUNTIME_FUNCTION FunctionEntry;
  int iVar1;
  DWORD64 local_res8;
  ulonglong local_res10;
  PVOID local_res18 [2];
  
  RtlCaptureContext();
  ControlPc = param_1->Rip;
  iVar1 = 0;
  do {
    FunctionEntry = RtlLookupFunctionEntry(ControlPc,&local_res8,(PUNWIND_HISTORY_TABLE)0x0);
    if (FunctionEntry == (PRUNTIME_FUNCTION)0x0) {
      return;
    }
    RtlVirtualUnwind(0,local_res8,ControlPc,FunctionEntry,param_1,local_res18,&local_res10,
                     (PKNONVOLATILE_CONTEXT_POINTERS)0x0);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 2);
  return;
}


/* ================= FUN_140059370 @ 140059370  size=182  callers=0 callees=19 ================= */
/* CALLEES: FUN_14005977c, FUN_140059798, FUN_140059928, FUN_140059a98, FUN_140059aa0, FUN_140059ab0, FUN_140059ad8, FUN_140059af4, __p__commode, __scrt_initialize_onexit_tables, __setusermatherr, _configthreadlocale, _configure_narrow_argv, _guard_check_icall, _initialize_narrow_environment, _set_app_type, _set_fmode, atexit, thunk_FUN_140059928 */

void FUN_140059370(void)

{
  code *pcVar1;
  undefined4 *puVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  
  _set_app_type(2);
  iVar4 = FUN_140059a98();
  _set_fmode(iVar4);
  uVar5 = FUN_140059928();
  puVar2 = (undefined4 *)__p__commode();
  *puVar2 = uVar5;
  cVar3 = __scrt_initialize_onexit_tables(1);
  if (cVar3 != '\0') {
    FUN_140059af4();
    atexit(FUN_140059b30);
    uVar5 = FUN_14005977c();
    iVar4 = _configure_narrow_argv(uVar5);
    if (iVar4 == 0) {
      FUN_140059aa0();
      iVar4 = FUN_140059ad8();
      if (iVar4 != 0) {
        __setusermatherr(FUN_140059928);
      }
      _guard_check_icall();
      _guard_check_icall();
      iVar4 = FUN_140059928();
      _configthreadlocale(iVar4);
      cVar3 = FUN_140059ab0();
      if (cVar3 != '\0') {
        _initialize_narrow_environment();
      }
      FUN_140059928();
      iVar4 = thunk_FUN_140059928();
      if (iVar4 == 0) {
        return;
      }
    }
  }
  FUN_140059798(7);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}


/* ================= FUN_140059428 @ 140059428  size=16  callers=0 callees=1 ================= */
/* CALLEES: FUN_140059abc */

undefined8 FUN_140059428(void)

{
  FUN_140059abc();
  return 0;
}


/* ================= FUN_140059438 @ 140059438  size=25  callers=0 callees=3 ================= */
/* CALLEES: FUN_140059928, FUN_140059980, _set_new_mode */

void FUN_140059438(void)

{
  undefined4 uVar1;
  
  FUN_140059980();
  uVar1 = FUN_140059928();
  _set_new_mode(uVar1);
  return;
}


/* ================= FUN_140059454 @ 140059454  size=335  callers=1 callees=18 ================= */
/* CALLERS: entry */
/* CALLEES: FUN_14004c670, FUN_140059798, FUN_140059ae4, FUN_140059aec, __scrt_acquire_startup_lock, __scrt_get_show_window_mode, __scrt_initialize_crt, __scrt_is_managed_app, __scrt_is_nonwritable_in_current_image, __scrt_release_startup_lock, __scrt_uninitialize_crt, _cexit, _get_narrow_winmain_command_line, _guard_dispatch_icall, _initterm, _initterm_e, _register_thread_local_exe_atexit_callback, exit */

/* WARNING: Function: _guard_dispatch_icall replaced with injection: guard_dispatch_icall */

int FUN_140059454(void)

{
  bool bVar1;
  char cVar2;
  undefined1 uVar3;
  undefined2 uVar4;
  int iVar5;
  longlong *plVar6;
  undefined8 uVar7;
  undefined8 unaff_RBX;
  
  iVar5 = (int)unaff_RBX;
  cVar2 = __scrt_initialize_crt(1);
  if (cVar2 == '\0') {
    FUN_140059798(7);
  }
  else {
    bVar1 = false;
    uVar3 = __scrt_acquire_startup_lock();
    iVar5 = (int)CONCAT71((int7)((ulonglong)unaff_RBX >> 8),uVar3);
    if (DAT_14286cd90 != 1) {
      if (DAT_14286cd90 == 0) {
        DAT_14286cd90 = 1;
        iVar5 = _initterm_e(&DAT_14005ea58,&DAT_14005ea70);
        if (iVar5 != 0) {
          return 0xff;
        }
        _initterm(&DAT_14005e958,&DAT_14005ea50);
        DAT_14286cd90 = 2;
      }
      else {
        bVar1 = true;
      }
      __scrt_release_startup_lock(uVar3);
      plVar6 = (longlong *)FUN_140059ae4();
      if ((*plVar6 != 0) && (cVar2 = __scrt_is_nonwritable_in_current_image(plVar6), cVar2 != '\0'))
      {
        (*(code *)*plVar6)(0,2);
      }
      plVar6 = (longlong *)FUN_140059aec();
      if ((*plVar6 != 0) && (cVar2 = __scrt_is_nonwritable_in_current_image(plVar6), cVar2 != '\0'))
      {
        _register_thread_local_exe_atexit_callback(*plVar6);
      }
      uVar4 = __scrt_get_show_window_mode();
      uVar7 = _get_narrow_winmain_command_line();
      iVar5 = FUN_14004c670(&IMAGE_DOS_HEADER_140000000,0,uVar7,uVar4);
      cVar2 = __scrt_is_managed_app();
      if (cVar2 != '\0') {
        if (!bVar1) {
          _cexit();
        }
        __scrt_uninitialize_crt(1,0);
        return iVar5;
      }
      goto LAB_1400595b5;
    }
  }
  FUN_140059798(7);
LAB_1400595b5:
                    /* WARNING: Subroutine does not return */
  exit(iVar5);
}


/* ================= entry @ 1400595c8  size=18  callers=0 callees=2 ================= */
/* CALLEES: FUN_140059454, __security_init_cookie */

void entry(void)

{
  __security_init_cookie();
  FUN_140059454();
  return;
}


/* ================= FUN_1400595dc @ 1400595dc  size=60  callers=0 callees=1 ================= */
/* CALLEES: __std_exception_copy */

undefined8 * FUN_1400595dc(undefined8 *param_1,longlong param_2)

{
  *param_1 = std::exception::vftable;
  param_1[1] = 0;
  param_1[2] = 0;
  __std_exception_copy(param_2 + 8);
  *param_1 = std::bad_alloc::vftable;
  return param_1;
}


/* ================= FUN_140059618 @ 140059618  size=30  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059720 */

undefined8 * FUN_140059618(undefined8 *param_1)

{
  param_1[2] = 0;
  param_1[1] = "bad allocation";
  *param_1 = std::bad_alloc::vftable;
  return param_1;
}


/* ================= FUN_140059638 @ 140059638  size=60  callers=0 callees=1 ================= */
/* CALLEES: __std_exception_copy */

undefined8 * FUN_140059638(undefined8 *param_1,longlong param_2)

{
  *param_1 = std::exception::vftable;
  param_1[1] = 0;
  param_1[2] = 0;
  __std_exception_copy(param_2 + 8);
  *param_1 = std::bad_array_new_length::vftable;
  return param_1;
}


/* ================= FUN_140059674 @ 140059674  size=30  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059740 */

undefined8 * FUN_140059674(undefined8 *param_1)

{
  param_1[2] = 0;
  param_1[1] = "bad array new length";
  *param_1 = std::bad_array_new_length::vftable;
  return param_1;
}


/* ================= exception @ 140059694  size=50  callers=0 callees=1 ================= */
/* CALLEES: __std_exception_copy */

/* Library Function - Single Match
    public: __cdecl std::exception::exception(class std::exception const & __ptr64) __ptr64
   
   Library: Visual Studio 2019 Release */

exception * __thiscall std::exception::exception(exception *this,exception *param_1)

{
  *(undefined ***)this = vftable;
  *(undefined8 *)(this + 8) = 0;
  *(undefined8 *)(this + 0x10) = 0;
  __std_exception_copy(param_1 + 8);
  return this;
}


/* ================= FUN_1400596dc @ 1400596dc  size=66  callers=0 callees=2 ================= */
/* CALLEES: __std_exception_destroy, free */

undefined8 * FUN_1400596dc(undefined8 *param_1,ulonglong param_2)

{
  *param_1 = std::exception::vftable;
  __std_exception_destroy(param_1 + 1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}


/* ================= FUN_140059720 @ 140059720  size=31  callers=1 callees=2 ================= */
/* CALLERS: FUN_140058d94 */
/* CALLEES: FUN_140059618, _CxxThrowException */

void FUN_140059720(void)

{
  undefined1 local_28 [40];
  
  FUN_140059618(local_28);
                    /* WARNING: Subroutine does not return */
  _CxxThrowException(local_28,(ThrowInfo *)&DAT_140084608);
}


/* ================= FUN_140059740 @ 140059740  size=31  callers=1 callees=2 ================= */
/* CALLERS: FUN_140058d94 */
/* CALLEES: FUN_140059674, _CxxThrowException */

void FUN_140059740(void)

{
  undefined1 local_28 [40];
  
  FUN_140059674(local_28);
                    /* WARNING: Subroutine does not return */
  _CxxThrowException(local_28,(ThrowInfo *)&DAT_140084690);
}


/* ================= FUN_140059760 @ 140059760  size=18  callers=0 callees=0 ================= */

char * FUN_140059760(longlong param_1)

{
  char *pcVar1;
  
  pcVar1 = "Unknown exception";
  if (*(longlong *)(param_1 + 8) != 0) {
    pcVar1 = *(char **)(param_1 + 8);
  }
  return pcVar1;
}


/* ================= FUN_14005977c @ 14005977c  size=6  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059370 */

undefined8 FUN_14005977c(void)

{
  return 1;
}


/* ================= __scrt_is_ucrt_dll_in_use @ 140059784  size=12  callers=3 callees=0 ================= */
/* CALLERS: __scrt_acquire_startup_lock, __scrt_initialize_onexit_tables, __scrt_release_startup_lock */

/* Library Function - Single Match
    __scrt_is_ucrt_dll_in_use
   
   Library: Visual Studio 2019 Release */

bool __scrt_is_ucrt_dll_in_use(void)

{
  return DAT_140097010 != 0;
}


/* ================= FUN_140059790 @ 140059790  size=8  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059798 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140059790(void)

{
  _DAT_14286d358 = 0;
  return;
}


/* ================= FUN_140059798 @ 140059798  size=331  callers=3 callees=9 ================= */
/* CALLERS: FUN_140059370, FUN_140059454, __scrt_initialize_onexit_tables */
/* CALLEES: FUN_140059790, IsDebuggerPresent, IsProcessorFeaturePresent, RtlCaptureContext, RtlLookupFunctionEntry, RtlVirtualUnwind, SetUnhandledExceptionFilter, UnhandledExceptionFilter, memset */

void FUN_140059798(undefined4 param_1)

{
  code *pcVar1;
  BOOL BVar2;
  LONG LVar3;
  PRUNTIME_FUNCTION FunctionEntry;
  undefined1 *puVar4;
  undefined8 unaff_retaddr;
  DWORD64 local_res10;
  undefined1 local_res18 [8];
  undefined1 local_res20 [8];
  undefined1 auStack_5c8 [8];
  undefined1 auStack_5c0 [232];
  undefined1 local_4d8 [152];
  undefined1 *local_440;
  DWORD64 local_3e0;
  
  puVar4 = auStack_5c8;
  BVar2 = IsProcessorFeaturePresent(0x17);
  if (BVar2 != 0) {
    pcVar1 = (code *)swi(0x29);
    (*pcVar1)(param_1);
    puVar4 = auStack_5c0;
  }
  *(undefined8 *)(puVar4 + -8) = 0x1400597cc;
  FUN_140059790(3);
  *(undefined8 *)(puVar4 + -8) = 0x1400597dd;
  memset(local_4d8,0,0x4d0);
  *(undefined8 *)(puVar4 + -8) = 0x1400597e7;
  RtlCaptureContext(local_4d8);
  *(undefined8 *)(puVar4 + -8) = 0x140059801;
  FunctionEntry = RtlLookupFunctionEntry(local_3e0,&local_res10,(PUNWIND_HISTORY_TABLE)0x0);
  if (FunctionEntry != (PRUNTIME_FUNCTION)0x0) {
    *(undefined8 *)(puVar4 + 0x38) = 0;
    *(undefined1 **)(puVar4 + 0x30) = local_res18;
    *(undefined1 **)(puVar4 + 0x28) = local_res20;
    *(undefined1 **)(puVar4 + 0x20) = local_4d8;
    *(undefined8 *)(puVar4 + -8) = 0x140059842;
    RtlVirtualUnwind(0,local_res10,local_3e0,FunctionEntry,*(PCONTEXT *)(puVar4 + 0x20),
                     *(PVOID **)(puVar4 + 0x28),*(PDWORD64 *)(puVar4 + 0x30),
                     *(PKNONVOLATILE_CONTEXT_POINTERS *)(puVar4 + 0x38));
  }
  local_440 = &stack0x00000008;
  *(undefined8 *)(puVar4 + -8) = 0x140059874;
  memset(puVar4 + 0x50,0,0x98);
  *(undefined8 *)(puVar4 + 0x60) = unaff_retaddr;
  *(undefined4 *)(puVar4 + 0x50) = 0x40000015;
  *(undefined4 *)(puVar4 + 0x54) = 1;
  *(undefined8 *)(puVar4 + -8) = 0x140059896;
  BVar2 = IsDebuggerPresent();
  *(undefined1 **)(puVar4 + 0x40) = puVar4 + 0x50;
  *(undefined1 **)(puVar4 + 0x48) = local_4d8;
  *(undefined8 *)(puVar4 + -8) = 0x1400598b7;
  SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)0x0);
  *(undefined8 *)(puVar4 + -8) = 0x1400598c2;
  LVar3 = UnhandledExceptionFilter((_EXCEPTION_POINTERS *)(puVar4 + 0x40));
  if ((LVar3 == 0) && (BVar2 != 1)) {
    *(undefined8 *)(puVar4 + -8) = 0x1400598d2;
    FUN_140059790(3);
  }
  return;
}


/* ================= __scrt_get_show_window_mode @ 1400598e4  size=58  callers=1 callees=2 ================= */
/* CALLERS: FUN_140059454 */
/* CALLEES: GetStartupInfoW, memset */

/* Library Function - Single Match
    __scrt_get_show_window_mode
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

WORD __scrt_get_show_window_mode(void)

{
  WORD WVar1;
  _STARTUPINFOW local_78;
  
  memset(&local_78,0,0x68);
  GetStartupInfoW(&local_78);
  WVar1 = 10;
  if (((byte)local_78.dwFlags & 1) != 0) {
    WVar1 = local_78.wShowWindow;
  }
  return WVar1;
}


/* ================= FUN_140059928 @ 140059928  size=3  callers=2 callees=0 ================= */
/* CALLERS: FUN_140059370, FUN_140059438 */

undefined8 FUN_140059928(void)

{
  return 0;
}


/* ================= __scrt_is_managed_app @ 14005992c  size=82  callers=1 callees=1 ================= */
/* CALLERS: FUN_140059454 */
/* CALLEES: GetModuleHandleW */

/* Library Function - Single Match
    __scrt_is_managed_app
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

ulonglong __scrt_is_managed_app(void)

{
  HMODULE pHVar1;
  ulonglong uVar2;
  int *piVar3;
  
  pHVar1 = GetModuleHandleW((LPCWSTR)0x0);
  if ((((pHVar1 == (HMODULE)0x0) || ((short)pHVar1->unused != 0x5a4d)) ||
      (piVar3 = (int *)((longlong)&pHVar1->unused + (longlong)pHVar1[0xf].unused), *piVar3 != 0x4550
      )) || (((pHVar1 = (HMODULE)0x20b, (short)piVar3[6] != 0x20b || ((uint)piVar3[0x21] < 0xf)) ||
             (piVar3[0x3e] == 0)))) {
    uVar2 = (ulonglong)pHVar1 & 0xffffffffffffff00;
  }
  else {
    uVar2 = 0x201;
  }
  return uVar2;
}


/* ================= FUN_140059980 @ 140059980  size=14  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059438 */

void FUN_140059980(void)

{
                    /* WARNING: Could not recover jumptable at 0x000140059987. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  SetUnhandledExceptionFilter(FUN_140059990);
  return;
}


/* ================= FUN_140059990 @ 140059990  size=90  callers=0 callees=3 ================= */
/* CALLEES: __current_exception, __current_exception_context, terminate */

undefined8 FUN_140059990(undefined8 *param_1)

{
  int *piVar1;
  undefined8 uVar2;
  undefined8 *puVar3;
  
  piVar1 = (int *)*param_1;
  if ((*piVar1 == -0x1f928c9d) && (piVar1[6] == 4)) {
    if ((piVar1[8] + 0xe66cfae0U < 3) || (piVar1[8] == 0x1994000)) {
      puVar3 = (undefined8 *)__current_exception();
      *puVar3 = piVar1;
      uVar2 = param_1[1];
      puVar3 = (undefined8 *)__current_exception_context();
      *puVar3 = uVar2;
                    /* WARNING: Subroutine does not return */
      terminate();
    }
  }
  return 0;
}


/* ================= __security_init_cookie @ 1400599ec  size=172  callers=1 callees=4 ================= */
/* CALLERS: entry */
/* CALLEES: GetCurrentProcessId, GetCurrentThreadId, GetSystemTimeAsFileTime, QueryPerformanceCounter */

/* Library Function - Single Match
    __security_init_cookie
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl __security_init_cookie(void)

{
  DWORD DVar1;
  _FILETIME local_res8;
  _FILETIME local_res10;
  LARGE_INTEGER local_res18;
  
  if (DAT_140096fe8 == 0x2b992ddfa232) {
    local_res10.dwLowDateTime = 0;
    local_res10.dwHighDateTime = 0;
    GetSystemTimeAsFileTime(&local_res10);
    local_res8 = local_res10;
    DVar1 = GetCurrentThreadId();
    local_res8 = (_FILETIME)((ulonglong)local_res8 ^ (ulonglong)DVar1);
    DVar1 = GetCurrentProcessId();
    local_res8 = (_FILETIME)((ulonglong)local_res8 ^ (ulonglong)DVar1);
    QueryPerformanceCounter(&local_res18);
    DAT_140096fe8 =
         ((ulonglong)local_res18.s.LowPart << 0x20 ^
          CONCAT44(local_res18.s.HighPart,local_res18.s.LowPart) ^ (ulonglong)local_res8 ^
         (ulonglong)&local_res8) & 0xffffffffffff;
    if (DAT_140096fe8 == 0x2b992ddfa232) {
      DAT_140096fe8 = 0x2b992ddfa233;
    }
  }
  DAT_140096fe0 = ~DAT_140096fe8;
  return;
}


/* ================= FUN_140059a98 @ 140059a98  size=6  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059370 */

undefined8 FUN_140059a98(void)

{
  return 0x4000;
}


/* ================= FUN_140059aa0 @ 140059aa0  size=14  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059370 */

void FUN_140059aa0(void)

{
                    /* WARNING: Could not recover jumptable at 0x000140059aa7. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  InitializeSListHead(&DAT_14286d360);
  return;
}


/* ================= FUN_140059ab0 @ 140059ab0  size=3  callers=3 callees=0 ================= */
/* CALLERS: FUN_140059370, __scrt_initialize_crt, __scrt_uninitialize_crt */

undefined1 FUN_140059ab0(void)

{
  return 1;
}


/* ================= FUN_140059ab4 @ 140059ab4  size=8  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059abc */

undefined * FUN_140059ab4(void)

{
  return &DAT_14286d370;
}


/* ================= FUN_140059abc @ 140059abc  size=27  callers=1 callees=2 ================= */
/* CALLERS: FUN_140059428 */
/* CALLEES: FUN_14000e420, FUN_140059ab4 */

void FUN_140059abc(void)

{
  ulonglong *puVar1;
  
  puVar1 = (ulonglong *)FUN_14000e420();
  *puVar1 = *puVar1 | 0x24;
  puVar1 = (ulonglong *)FUN_140059ab4();
  *puVar1 = *puVar1 | 2;
  return;
}


/* ================= FUN_140059ad8 @ 140059ad8  size=12  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059370 */

bool FUN_140059ad8(void)

{
  return DAT_14009700c == 0;
}


/* ================= FUN_140059ae4 @ 140059ae4  size=8  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059454 */

undefined * FUN_140059ae4(void)

{
  return &DAT_14286d3d8;
}


/* ================= FUN_140059aec @ 140059aec  size=8  callers=1 callees=0 ================= */
/* CALLERS: FUN_140059454 */

undefined * FUN_140059aec(void)

{
  return &DAT_14286d3d0;
}


/* ================= FUN_140059af4 @ 140059af4  size=60  callers=1 callees=1 ================= */
/* CALLERS: FUN_140059370 */
/* CALLEES: _guard_dispatch_icall */

/* WARNING: Function: _guard_dispatch_icall replaced with injection: guard_dispatch_icall */

void FUN_140059af4(void)

{
  undefined8 *puVar1;
  
  for (puVar1 = &DAT_14007c7d8; puVar1 < &DAT_14007c7d8; puVar1 = puVar1 + 1) {
    if ((code *)*puVar1 != (code *)0x0) {
      (*(code *)*puVar1)();
    }
  }
  return;
}


/* ================= FUN_140059b30 @ 140059b30  size=60  callers=0 callees=1 ================= */
/* CALLEES: _guard_dispatch_icall */

/* WARNING: Function: _guard_dispatch_icall replaced with injection: guard_dispatch_icall */

void FUN_140059b30(void)

{
  undefined8 *puVar1;
  
  for (puVar1 = &DAT_14007c7e8; puVar1 < &DAT_14007c7e8; puVar1 = puVar1 + 1) {
    if ((code *)*puVar1 != (code *)0x0) {
      (*(code *)*puVar1)();
    }
  }
  return;
}


/* ================= _guard_dispatch_icall @ 140059c70  size=2  callers=5 callees=0 ================= */
/* CALLERS: FUN_140059454, FUN_140059af4, FUN_140059b30, __ArrayUnwind, `eh_vector_destructor_iterator' */

/* WARNING: This is an inlined function */

void _guard_dispatch_icall(void)

{
  code *UNRECOVERED_JUMPTABLE;
  
                    /* WARNING: Could not recover jumptable at 0x000140059c70. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)();
  return;
}


/* ================= FUN_140059d70 @ 140059d70  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_140059d70(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x40));
  return;
}


/* ================= FUN_140059da0 @ 140059da0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_140059da0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x50));
  return;
}


/* ================= FUN_140059dd0 @ 140059dd0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_140059dd0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x38));
  return;
}


/* ================= FUN_140059df0 @ 140059df0  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QPixmap */

void FUN_140059df0(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x30) & 1) != 0) {
    *(uint *)(param_2 + 0x30) = *(uint *)(param_2 + 0x30) & 0xfffffffe;
    QPixmap::~QPixmap(*(QPixmap **)(param_2 + 0x60));
  }
  return;
}


/* ================= FUN_140059e50 @ 140059e50  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_140059e50(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x70));
  return;
}


/* ================= FUN_140059ea0 @ 140059ea0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_140059ea0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x48));
  return;
}


/* ================= FUN_140059ec0 @ 140059ec0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_140059ec0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x50));
  return;
}


/* ================= FUN_140059ee0 @ 140059ee0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_140059ee0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x50));
  return;
}


/* ================= FUN_140059f00 @ 140059f00  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_140059f00(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x50));
  return;
}


/* ================= FUN_140059f30 @ 140059f30  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_140059f30(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x58));
  return;
}


/* ================= FUN_140059f50 @ 140059f50  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_140059f50(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x38) & 1) != 0) {
    *(uint *)(param_2 + 0x38) = *(uint *)(param_2 + 0x38) & 0xfffffffe;
    QString::~QString((QString *)(param_2 + 0x30));
  }
  return;
}


/* ================= FUN_140059fd0 @ 140059fd0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_140059fd0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x40));
  return;
}


/* ================= FUN_14005a230 @ 14005a230  size=38  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002d940 */

void FUN_14005a230(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x68) & 1) != 0) {
    *(uint *)(param_2 + 0x68) = *(uint *)(param_2 + 0x68) & 0xfffffffe;
    FUN_14002d940(param_2 + 0x50);
  }
  return;
}


/* ================= FUN_14005a260 @ 14005a260  size=38  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002d810 */

void FUN_14005a260(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x68) & 2) != 0) {
    *(uint *)(param_2 + 0x68) = *(uint *)(param_2 + 0x68) & 0xfffffffd;
    FUN_14002d810(param_2 + 0x38);
  }
  return;
}


/* ================= FUN_14005a2a0 @ 14005a2a0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005a2a0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x40));
  return;
}


/* ================= FUN_14005a2c0 @ 14005a2c0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005a2c0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x40));
  return;
}


/* ================= FUN_14005a2e0 @ 14005a2e0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005a2e0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x40));
  return;
}


/* ================= FUN_14005a340 @ 14005a340  size=47  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002d770 */

void FUN_14005a340(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 1) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xfffffffe;
    FUN_14002d770(param_2 + 0xa0);
  }
  return;
}


/* ================= FUN_14005a390 @ 14005a390  size=38  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002d810 */

void FUN_14005a390(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x50) & 1) != 0) {
    *(uint *)(param_2 + 0x50) = *(uint *)(param_2 + 0x50) & 0xfffffffe;
    FUN_14002d810(param_2 + 0x40);
  }
  return;
}


/* ================= FUN_14005a3c0 @ 14005a3c0  size=32  callers=0 callees=1 ================= */
/* CALLEES: _CxxThrowException */

void FUN_14005a3c0(undefined8 param_1,longlong param_2)

{
  int *piVar1;
  
  piVar1 = (int *)(**(longlong **)(param_2 + 0x30) + 0xc);
  *piVar1 = *piVar1 + -1;
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a3f0 @ 14005a3f0  size=35  callers=0 callees=2 ================= */
/* CALLEES: _CxxThrowException, _guard_check_icall */

void FUN_14005a3f0(void)

{
  _guard_check_icall();
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a420 @ 14005a420  size=35  callers=0 callees=2 ================= */
/* CALLEES: FUN_140032000, _CxxThrowException */

void FUN_14005a420(void)

{
  FUN_140032000();
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a460 @ 14005a460  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QModelIndex */

void FUN_14005a460(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x20) & 1) != 0) {
    *(uint *)(param_2 + 0x20) = *(uint *)(param_2 + 0x20) & 0xfffffffe;
    QModelIndex::~QModelIndex((QModelIndex *)(param_2 + 0x28));
  }
  return;
}


/* ================= FUN_14005a4a0 @ 14005a4a0  size=43  callers=0 callees=2 ================= */
/* CALLEES: _CxxThrowException, qFree */

void FUN_14005a4a0(undefined8 param_1,longlong param_2)

{
  undefined8 *puVar1;
  
  puVar1 = *(undefined8 **)(param_2 + 0x30);
  qFree((void *)*puVar1);
  *puVar1 = *(undefined8 *)(param_2 + 0x40);
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a4d0 @ 14005a4d0  size=35  callers=0 callees=2 ================= */
/* CALLEES: FUN_140031480, _CxxThrowException */

void FUN_14005a4d0(void)

{
  FUN_140031480();
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a500 @ 14005a500  size=35  callers=0 callees=2 ================= */
/* CALLEES: FUN_1400314d0, _CxxThrowException */

void FUN_14005a500(void)

{
  FUN_1400314d0();
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a530 @ 14005a530  size=84  callers=0 callees=4 ================= */
/* CALLEES: _CxxThrowException, _guard_check_icall, begin, qFree */

void FUN_14005a530(undefined8 param_1,longlong param_2)

{
  int iVar1;
  QListData *this;
  void **ppvVar2;
  void **ppvVar3;
  
  this = *(QListData **)(param_2 + 0x40);
  ppvVar2 = QListData::begin(this);
  iVar1 = *(int *)(param_2 + 0x48);
  ppvVar3 = QListData::begin(this);
  _guard_check_icall(this,ppvVar3,ppvVar2 + iVar1);
  qFree(*(void **)this);
  *(undefined8 *)this = *(undefined8 *)(param_2 + 0x58);
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a590 @ 14005a590  size=43  callers=0 callees=2 ================= */
/* CALLEES: _CxxThrowException, qFree */

void FUN_14005a590(undefined8 param_1,longlong param_2)

{
  undefined8 *puVar1;
  
  puVar1 = *(undefined8 **)(param_2 + 0x40);
  qFree((void *)*puVar1);
  *puVar1 = *(undefined8 *)(param_2 + 0x58);
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a5c0 @ 14005a5c0  size=84  callers=0 callees=4 ================= */
/* CALLEES: FUN_140031fc0, _CxxThrowException, begin, qFree */

void FUN_14005a5c0(undefined8 param_1,longlong param_2)

{
  int iVar1;
  QListData *this;
  void **ppvVar2;
  void **ppvVar3;
  
  this = *(QListData **)(param_2 + 0x40);
  ppvVar2 = QListData::begin(this);
  iVar1 = *(int *)(param_2 + 0x48);
  ppvVar3 = QListData::begin(this);
  FUN_140031fc0(this,ppvVar3,ppvVar2 + iVar1);
  qFree(*(void **)this);
  *(undefined8 *)this = *(undefined8 *)(param_2 + 0x58);
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a6f0 @ 14005a6f0  size=60  callers=0 callees=2 ================= */
/* CALLEES: FUN_14002de80, _CxxThrowException */

void FUN_14005a6f0(undefined8 param_1,longlong param_2)

{
  longlong lVar1;
  longlong lVar2;
  
  lVar1 = *(longlong *)(param_2 + 0x38);
  for (lVar2 = *(longlong *)(param_2 + 0x40); lVar2 != lVar1; lVar2 = lVar2 + -8) {
    FUN_14002de80();
  }
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a730 @ 14005a730  size=22  callers=0 callees=1 ================= */
/* CALLEES: _CxxThrowException */

void FUN_14005a730(void)

{
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a750 @ 14005a750  size=48  callers=0 callees=3 ================= */
/* CALLEES: FUN_140032030, _CxxThrowException, node_delete */

void FUN_14005a750(undefined8 param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = FUN_140032030();
  QMapData::node_delete
            (*(QMapData **)(param_2 + 0x38),*(Node ***)(param_2 + 0x40),iVar1,
             *(Node **)(param_2 + 0x48));
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a790 @ 14005a790  size=33  callers=0 callees=2 ================= */
/* CALLEES: FUN_14002de80, _CxxThrowException */

void FUN_14005a790(void)

{
  FUN_14002de80();
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a7c0 @ 14005a7c0  size=48  callers=0 callees=3 ================= */
/* CALLEES: FUN_140032030, _CxxThrowException, node_delete */

void FUN_14005a7c0(undefined8 param_1,longlong param_2)

{
  int iVar1;
  
  iVar1 = FUN_140032030();
  QMapData::node_delete
            (*(QMapData **)(param_2 + 0x48),*(Node ***)(param_2 + 0x50),iVar1,
             *(Node **)(param_2 + 0x28));
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005a840 @ 14005a840  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005a840(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x80));
  return;
}


/* ================= FUN_14005a860 @ 14005a860  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005a860(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x98));
  return;
}


/* ================= FUN_14005a880 @ 14005a880  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005a880(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x80));
  return;
}


/* ================= FUN_14005a8a0 @ 14005a8a0  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005a8a0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x80));
  return;
}


/* ================= FUN_14005a8c0 @ 14005a8c0  size=38  callers=0 callees=1 ================= */
/* CALLEES: FUN_14002d810 */

void FUN_14005a8c0(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x20) & 1) != 0) {
    *(uint *)(param_2 + 0x20) = *(uint *)(param_2 + 0x20) & 0xfffffffe;
    FUN_14002d810(*(undefined8 *)(param_2 + 0x48));
  }
  return;
}


/* ================= FUN_14005a970 @ 14005a970  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005a970(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x80));
  return;
}


/* ================= FUN_14005a990 @ 14005a990  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005a990(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x40) & 1) != 0) {
    *(uint *)(param_2 + 0x40) = *(uint *)(param_2 + 0x40) & 0xfffffffe;
    QString::~QString((QString *)(param_2 + 0x48));
  }
  return;
}


/* ================= FUN_14005a9c0 @ 14005a9c0  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QKeySequence */

void FUN_14005a9c0(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x40) & 2) != 0) {
    *(uint *)(param_2 + 0x40) = *(uint *)(param_2 + 0x40) & 0xfffffffd;
    QKeySequence::~QKeySequence((QKeySequence *)(param_2 + 0x50));
  }
  return;
}


/* ================= FUN_14005aa10 @ 14005aa10  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005aa10(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x20) & 1) != 0) {
    *(uint *)(param_2 + 0x20) = *(uint *)(param_2 + 0x20) & 0xfffffffe;
    QString::~QString(*(QString **)(param_2 + 0x40));
  }
  return;
}


/* ================= FUN_14005aa40 @ 14005aa40  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005aa40(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x58));
  return;
}


/* ================= FUN_14005aa60 @ 14005aa60  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005aa60(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x38));
  return;
}


/* ================= FUN_14005aa80 @ 14005aa80  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005aa80(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x38));
  return;
}


/* ================= FUN_14005aaa0 @ 14005aaa0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005aaa0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x58));
  return;
}


/* ================= FUN_14005aad0 @ 14005aad0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005aad0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x68));
  return;
}


/* ================= FUN_14005aaf0 @ 14005aaf0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005aaf0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x20));
  return;
}


/* ================= FUN_14005ab10 @ 14005ab10  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005ab10(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x78) & 1) != 0) {
    *(uint *)(param_2 + 0x78) = *(uint *)(param_2 + 0x78) & 0xfffffffe;
    QString::~QString((QString *)(param_2 + 0x68));
  }
  return;
}


/* ================= FUN_14005ac00 @ 14005ac00  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005ac00(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x58));
  return;
}


/* ================= FUN_14005ac70 @ 14005ac70  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005ac70(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xc0));
  return;
}


/* ================= FUN_14005aca0 @ 14005aca0  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005aca0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 200));
  return;
}


/* ================= FUN_14005ad00 @ 14005ad00  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005ad00(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x40));
  return;
}


/* ================= FUN_14005ad20 @ 14005ad20  size=35  callers=0 callees=2 ================= */
/* CALLEES: FUN_140038d80, _CxxThrowException */

void FUN_14005ad20(void)

{
  FUN_140038d80();
                    /* WARNING: Subroutine does not return */
  _CxxThrowException((void *)0x0,(ThrowInfo *)0x0);
}


/* ================= FUN_14005ad50 @ 14005ad50  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005ad50(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x38));
  return;
}


/* ================= FUN_14005ad90 @ 14005ad90  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005ad90(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x30));
  return;
}


/* ================= FUN_14005adb0 @ 14005adb0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005adb0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x30));
  return;
}


/* ================= FUN_14005add0 @ 14005add0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005add0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x50));
  return;
}


/* ================= FUN_14005adf0 @ 14005adf0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005adf0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x50));
  return;
}


/* ================= FUN_14005ae20 @ 14005ae20  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005ae20(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x20));
  return;
}


/* ================= FUN_14005ae40 @ 14005ae40  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005ae40(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x40) & 1) != 0) {
    *(uint *)(param_2 + 0x40) = *(uint *)(param_2 + 0x40) & 0xfffffffe;
    QString::~QString((QString *)(param_2 + 0x50));
  }
  return;
}


/* ================= FUN_14005ae70 @ 14005ae70  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005ae70(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 200));
  return;
}


/* ================= FUN_14005ae90 @ 14005ae90  size=48  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005ae90(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0xb0) & 1) != 0) {
    *(uint *)(param_2 + 0xb0) = *(uint *)(param_2 + 0xb0) & 0xfffffffe;
    QString::~QString((QString *)(param_2 + 0xc0));
  }
  return;
}


/* ================= FUN_14005aef0 @ 14005aef0  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005aef0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xe0));
  return;
}


/* ================= FUN_14005af60 @ 14005af60  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005af60(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x90));
  return;
}


/* ================= FUN_14005afa0 @ 14005afa0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005afa0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x40));
  return;
}


/* ================= FUN_14005afe0 @ 14005afe0  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005afe0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xc0));
  return;
}


/* ================= FUN_14005b000 @ 14005b000  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b000(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xc0));
  return;
}


/* ================= FUN_14005b020 @ 14005b020  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b020(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xc0));
  return;
}


/* ================= FUN_14005b040 @ 14005b040  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b040(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xc0));
  return;
}


/* ================= FUN_14005b060 @ 14005b060  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b060(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xc0));
  return;
}


/* ================= FUN_14005b080 @ 14005b080  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b080(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xc0));
  return;
}


/* ================= FUN_14005b0a0 @ 14005b0a0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b0a0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x40));
  return;
}


/* ================= FUN_14005b0c0 @ 14005b0c0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b0c0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x30));
  return;
}


/* ================= FUN_14005b0e0 @ 14005b0e0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b0e0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x30));
  return;
}


/* ================= FUN_14005b100 @ 14005b100  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b100(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x38));
  return;
}


/* ================= FUN_14005b120 @ 14005b120  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b120(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x38));
  return;
}


/* ================= FUN_14005b140 @ 14005b140  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b140(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x48));
  return;
}


/* ================= FUN_14005b170 @ 14005b170  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b170(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x50) & 1) != 0) {
    *(uint *)(param_2 + 0x50) = *(uint *)(param_2 + 0x50) & 0xfffffffe;
    QString::~QString((QString *)(param_2 + 0x60));
  }
  return;
}


/* ================= FUN_14005b1a0 @ 14005b1a0  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b1a0(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x50) & 2) != 0) {
    *(uint *)(param_2 + 0x50) = *(uint *)(param_2 + 0x50) & 0xfffffffd;
    QString::~QString((QString *)(param_2 + 0x58));
  }
  return;
}


/* ================= FUN_14005b1e0 @ 14005b1e0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b1e0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x40));
  return;
}


/* ================= FUN_14005b200 @ 14005b200  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b200(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x20));
  return;
}


/* ================= FUN_14005b240 @ 14005b240  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b240(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xb8));
  return;
}


/* ================= FUN_14005b290 @ 14005b290  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b290(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x168));
  return;
}


/* ================= FUN_14005b2b0 @ 14005b2b0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b2b0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x30));
  return;
}


/* ================= FUN_14005b310 @ 14005b310  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b310(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x30));
  return;
}


/* ================= FUN_14005b390 @ 14005b390  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b390(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x90));
  return;
}


/* ================= FUN_14005b3b0 @ 14005b3b0  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b3b0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x90));
  return;
}


/* ================= FUN_14005b3d0 @ 14005b3d0  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b3d0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x98));
  return;
}


/* ================= FUN_14005b3f0 @ 14005b3f0  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b3f0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x98));
  return;
}


/* ================= FUN_14005b410 @ 14005b410  size=48  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b410(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 1) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xfffffffe;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b440 @ 14005b440  size=48  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b440(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 2) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xfffffffd;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b470 @ 14005b470  size=48  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b470(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 4) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xfffffffb;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b4a0 @ 14005b4a0  size=48  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b4a0(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 8) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xfffffff7;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b4d0 @ 14005b4d0  size=48  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b4d0(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 0x10) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xffffffef;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b500 @ 14005b500  size=48  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b500(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 0x20) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xffffffdf;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b530 @ 14005b530  size=48  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b530(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 0x40) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xffffffbf;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b560 @ 14005b560  size=53  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b560(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 0x80) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xffffff7f;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b5a0 @ 14005b5a0  size=53  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b5a0(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 0x100) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xfffffeff;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b5e0 @ 14005b5e0  size=53  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b5e0(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 0x200) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xfffffdff;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b620 @ 14005b620  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b620(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xa8));
  return;
}


/* ================= FUN_14005b640 @ 14005b640  size=53  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b640(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 0x400) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xfffffbff;
    QString::~QString((QString *)(param_2 + 0x98));
  }
  return;
}


/* ================= FUN_14005b680 @ 14005b680  size=53  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b680(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x90) & 0x800) != 0) {
    *(uint *)(param_2 + 0x90) = *(uint *)(param_2 + 0x90) & 0xfffff7ff;
    QString::~QString((QString *)(param_2 + 0xa8));
  }
  return;
}


/* ================= FUN_14005b6e0 @ 14005b6e0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b6e0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x20));
  return;
}


/* ================= FUN_14005b700 @ 14005b700  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b700(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x20));
  return;
}


/* ================= FUN_14005b750 @ 14005b750  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b750(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x60) & 1) != 0) {
    *(uint *)(param_2 + 0x60) = *(uint *)(param_2 + 0x60) & 0xfffffffe;
    QString::~QString((QString *)(param_2 + 0x20));
  }
  return;
}


/* ================= FUN_14005b7e0 @ 14005b7e0  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b7e0(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x48) & 1) != 0) {
    *(uint *)(param_2 + 0x48) = *(uint *)(param_2 + 0x48) & 0xfffffffe;
    QString::~QString((QString *)(param_2 + 0x40));
  }
  return;
}


/* ================= FUN_14005b810 @ 14005b810  size=39  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b810(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0x48) & 2) != 0) {
    *(uint *)(param_2 + 0x48) = *(uint *)(param_2 + 0x48) & 0xfffffffd;
    QString::~QString((QString *)(param_2 + 0x30));
  }
  return;
}


/* ================= FUN_14005b870 @ 14005b870  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b870(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xa0));
  return;
}


/* ================= FUN_14005b890 @ 14005b890  size=45  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b890(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0xa0) & 1) != 0) {
    *(uint *)(param_2 + 0xa0) = *(uint *)(param_2 + 0xa0) & 0xfffffffe;
    QString::~QString((QString *)(param_2 + 0x38));
  }
  return;
}


/* ================= FUN_14005b8c0 @ 14005b8c0  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b8c0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xa0));
  return;
}


/* ================= FUN_14005b8e0 @ 14005b8e0  size=32  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b8e0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0xa0));
  return;
}


/* ================= FUN_14005b900 @ 14005b900  size=45  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b900(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0xa0) & 2) != 0) {
    *(uint *)(param_2 + 0xa0) = *(uint *)(param_2 + 0xa0) & 0xfffffffd;
    QString::~QString((QString *)(param_2 + 0x38));
  }
  return;
}


/* ================= FUN_14005b930 @ 14005b930  size=45  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b930(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0xa0) & 4) != 0) {
    *(uint *)(param_2 + 0xa0) = *(uint *)(param_2 + 0xa0) & 0xfffffffb;
    QString::~QString((QString *)(param_2 + 0x38));
  }
  return;
}


/* ================= FUN_14005b970 @ 14005b970  size=45  callers=0 callees=1 ================= */
/* CALLEES: ~QString */

void FUN_14005b970(undefined8 param_1,longlong param_2)

{
  if ((*(uint *)(param_2 + 0xa0) & 8) != 0) {
    *(uint *)(param_2 + 0xa0) = *(uint *)(param_2 + 0xa0) & 0xfffffff7;
    QString::~QString((QString *)(param_2 + 0x38));
  }
  return;
}


/* ================= FUN_14005b9b0 @ 14005b9b0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b9b0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x50));
  return;
}


/* ================= FUN_14005b9d0 @ 14005b9d0  size=29  callers=0 callees=1 ================= */
/* CALLEES: free */

void FUN_14005b9d0(undefined8 param_1,longlong param_2)

{
  free(*(void **)(param_2 + 0x40));
  return;
}


/* ================= FUN_14005b9ed @ 14005b9ed  size=43  callers=0 callees=1 ================= */
/* CALLEES: __ArrayUnwind */

void FUN_14005b9ed(undefined8 param_1,longlong param_2)

{
  if (*(char *)(param_2 + 0x20) == '\0') {
    __ArrayUnwind(*(void **)(param_2 + 0x60),*(__uint64 *)(param_2 + 0x68),
                  *(__uint64 *)(param_2 + 0x70),*(_func_void_void_ptr **)(param_2 + 0x78));
  }
  return;
}


/* ================= FUN_14005ba19 @ 14005ba19  size=93  callers=0 callees=3 ================= */
/* CALLEES: __current_exception, __current_exception_context, terminate */

undefined4 FUN_14005ba19(undefined8 param_1,longlong param_2)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  
  *(undefined8 *)(param_2 + 0x40) = param_1;
  *(undefined8 *)(param_2 + 0x30) = param_1;
  *(undefined8 *)(param_2 + 0x28) = **(undefined8 **)(param_2 + 0x30);
  if (**(int **)(param_2 + 0x28) != -0x1f928c9d) {
    *(undefined4 *)(param_2 + 0x20) = 0;
    return *(undefined4 *)(param_2 + 0x20);
  }
  puVar2 = (undefined8 *)__current_exception();
  *puVar2 = *(undefined8 *)(param_2 + 0x28);
  uVar1 = *(undefined8 *)(*(longlong *)(param_2 + 0x30) + 8);
  puVar2 = (undefined8 *)__current_exception_context();
  *puVar2 = uVar1;
                    /* WARNING: Subroutine does not return */
  terminate();
}


/* ================= FUN_14005ba78 @ 14005ba78  size=23  callers=0 callees=0 ================= */

bool FUN_14005ba78(undefined8 *param_1)

{
  return *(int *)*param_1 == -0x3ffffffb;
}

