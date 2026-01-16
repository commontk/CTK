/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

// Qt includes
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QPixmap>
#include <QFont>
#include <QFontMetrics>
#include <QPalette>
#include <QRect>
#include <QLinearGradient>
#include <QDate>
#include <QDebug>
#include <QListView>
#include <QSvgRenderer>
#include <QIcon>
#include <QMouseEvent>

// CTK includes
#include "ctkDICOMPatientDelegate.h"
#include "ctkDICOMPatientModel.h"
#include "ctkDICOMPatientView.h"
#include "ctkDICOMStudyListView.h"

//------------------------------------------------------------------------------
class ctkDICOMPatientDelegatePrivate
{
public:
  ctkDICOMPatientDelegatePrivate();

  int Spacing;
  int CornerRadius;
  int TabModeHeight;
  int PatientHeaderHeight;
  int IconSize;
  int MaxTextWidth;
};

//------------------------------------------------------------------------------
ctkDICOMPatientDelegatePrivate::ctkDICOMPatientDelegatePrivate()
{
  this->Spacing = 4;
  this->CornerRadius = 8;
  this->TabModeHeight = 34;
  this->PatientHeaderHeight = 70;
  this->IconSize = 24;
  this->MaxTextWidth = 200;
}

//------------------------------------------------------------------------------
ctkDICOMPatientDelegate::ctkDICOMPatientDelegate(QObject* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMPatientDelegatePrivate)
{
}

//------------------------------------------------------------------------------
ctkDICOMPatientDelegate::~ctkDICOMPatientDelegate()
{
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return;
  }

  // Get display mode from view
  painter->setRenderHint(QPainter::Antialiasing, true);

  // Paint based on display mode
  ctkDICOMPatientView::DisplayMode displayMode = this->getDisplayMode(option);
  if (displayMode == ctkDICOMPatientView::TabMode)
  {
    this->paintTabMode(painter, option, index);
  }
  else // ListMode
  {
    this->paintListMode(painter, option, index);
  }
}

//------------------------------------------------------------------------------
QSize ctkDICOMPatientDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  Q_D(const ctkDICOMPatientDelegate);
  ctkDICOMPatientView::DisplayMode displayMode = this->getDisplayMode(option);
  if (displayMode == ctkDICOMPatientView::TabMode)
  {
    QSize size;
    QRect patientsRect = this->patientsRect(option.rect, option, index);
    if (patientsRect.isValid())
    {
      size = QSize(patientsRect.width() + d->Spacing * 2, patientsRect.height());
    }
    return size;
  }
  else // ListMode
  {
    QSize size;
    QRect patientsRect =  this->patientsRect(option.rect, option, index);
    if (patientsRect.isValid())
    {
      size = QSize(patientsRect.width(), patientsRect.height() + d->Spacing * 1.25);
    }
    return size;
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::paintTabMode(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QRect itemRect = option.rect;
  QRect patientsRect = this->patientsRect(itemRect, option, index);
  this->paintPatientsTabs(painter, patientsRect, option, index);

  bool isSelected = option.state & QStyle::State_Selected;
  if (!isSelected)
  {
    return;
  }

  QRect patientHeaderRect = this->patientHeaderRect(patientsRect, option);
  this->paintPatientHeader(painter, patientHeaderRect, option, index);

  // Paint interactive icons in the patient header area
  this->paintSelectAllIcon(painter, itemRect, index, option);
  this->paintContextMenuButton(painter, itemRect, index, option);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::paintPatientsTabs(QPainter *painter, const QRect &rect,
                                                const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const
{
  Q_D(const ctkDICOMPatientDelegate);

  painter->save();

  // Determine colors based on selection state
  bool isSelected = option.state & QStyle::State_Selected;

  // Use view's custom hover tracking instead of Qt's internal hover state
  bool isHovered = false;
  const ctkDICOMPatientView* patientView = qobject_cast<const ctkDICOMPatientView*>(option.widget);
  if (patientView)
  {
    isHovered = patientView->isIndexHovered(index);
  }

  QColor backgroundColor;
  QColor borderColor;

  if (isSelected && isHovered)
  {
    backgroundColor = QColor(70, 130, 180, 100);
    borderColor = QColor(160, 160, 160);
  }
  else if (isSelected)
  {
    backgroundColor = QColor(70, 130, 180, 70);
    borderColor = QColor(180, 180, 180);
  }
  else if (isHovered)
  {
    backgroundColor = QColor(70, 130, 180, 35);
    borderColor = QColor(200, 200, 200);
  }
  else
  {
    backgroundColor = QColor(250, 250, 250);
    borderColor = QColor(220, 220, 220);
  }

  QColor textColor = QColor(0, 0, 0, 150);
  int borderWidth = 2;

  QPainterPath path;
  path.addRoundedRect(rect, d->CornerRadius, d->CornerRadius);
  painter->fillPath(path, backgroundColor);

  // Draw border
  painter->setPen(QPen(borderColor, borderWidth));
  painter->drawPath(path);

  // Draw patient icon from SVG
  int iconSize = d->IconSize;
  int iconMargin = d->IconSize * 0.5;
  QRect iconRect(rect.left() + iconMargin, rect.top() + (rect.height() - iconSize) / 2, iconSize, iconSize);

  // Try to load and render the patient SVG icon
  int operationStatus = index.data(ctkDICOMPatientModel::OperationStatusRole).toInt();
  QIcon patientIcon;
  if (operationStatus == ctkDICOMPatientModel::NoOperation)
  {
    patientIcon = QIcon(":/Icons/patient.svg");
  }
  else if (operationStatus == ctkDICOMPatientModel::InProgress)
  {
    patientIcon = QIcon(":/Icons/patient_pending.svg");
  }
  else if (operationStatus == ctkDICOMPatientModel::Failed)
  {
    patientIcon = QIcon(":/Icons/patient_failed.svg");
  }
  else if (operationStatus == ctkDICOMPatientModel::Completed)
  {
    patientIcon = QIcon(":/Icons/patient_success.svg");
  }
  if (!patientIcon.isNull())
  {
    // Render the icon with the appropriate color
    QPixmap iconPixmap = patientIcon.pixmap(QSize(iconSize, iconSize));

    // If we need to colorize the icon, we can do it here
    if (!iconPixmap.isNull())
    {
      painter->drawPixmap(iconRect, iconPixmap);
    }
  }

  // Draw patient name
  QString patientName = index.data(ctkDICOMPatientModel::PatientNameRole).toString();
  if (patientName.isEmpty())
  {
    patientName = "Anonymous";
  }

  painter->setPen(textColor);

  // Text starts after icon
  int textLeft = iconRect.right() + d->Spacing * 0.5;
  QRect textRect(textLeft, rect.top(), rect.right() - textLeft - iconMargin, rect.height());

  QString elidedtext = QFontMetrics(option.font).elidedText(patientName, Qt::ElideRight, textRect.width());
  painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, elidedtext);

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::paintPatientHeader(QPainter *painter, const QRect &rect,
                                                 const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) const
{
  painter->save();

  QColor bgColor = QColor(248, 249, 250);
  QColor borderColor = QColor(225, 228, 232);
  QColor textColor = QColor(0, 0, 0);
  QColor secondaryTextColor = QColor(108, 117, 125);

  // Draw background with subtle border
  int cornerRadius = 6;
  QPainterPath path;
  path.addRoundedRect(rect, cornerRadius, cornerRadius);
  painter->fillPath(path, bgColor);
  painter->setPen(QPen(borderColor, 1));
  painter->drawPath(path);

  // Get patient data from model
  QString patientName = index.data(ctkDICOMPatientModel::PatientNameRole).toString();
  QString patientID = index.data(ctkDICOMPatientModel::PatientIDRole).toString();
  QString patientBirthDate = index.data(ctkDICOMPatientModel::PatientBirthDateRole).toString();
  QString patientSex = index.data(ctkDICOMPatientModel::PatientSexRole).toString();
  QDateTime insertDateTime = index.data(ctkDICOMPatientModel::PatientInsertDateTimeRole).toDateTime();
  int studyCount = index.data(ctkDICOMPatientModel::StudyCountRole).toInt();
  int filteredStudyCount = index.data(ctkDICOMPatientModel::FilteredStudyCountRole).toInt();

  if (patientName.isEmpty())
  {
    patientName = "Unknown Patient";
  }

  // Calculate text areas with better padding
  QRect contentRect = rect.adjusted(16, 12, -16, -12);
  int currentY = contentRect.top();

  // Draw patient name (bold, slightly larger)
  QFont nameFont = painter->font();
  nameFont.setPointSize(nameFont.pointSize() + 1);
  nameFont.setBold(true);
  painter->setFont(nameFont);
  painter->setPen(textColor);

  QRect nameRect(contentRect.left(), currentY, contentRect.width(), 22);
  painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, patientName);
  currentY += 28;

  // Draw patient info line (ID, Birth Date, Sex, study count)
  QFont infoFont = painter->font();
  infoFont.setPointSize(nameFont.pointSize() - 1);
  infoFont.setBold(false);
  painter->setFont(infoFont);
  painter->setPen(secondaryTextColor);

  QStringList infoParts;
  if (!patientID.isEmpty())
  {
    infoParts << QString("MRN: %1").arg(patientID);
  }
  if (!patientBirthDate.isEmpty())
  {
    infoParts << QString("Birth Date: %1").arg(patientBirthDate);
  }
  if (!patientSex.isEmpty())
  {
    infoParts << QString("Sex: %1").arg(patientSex);
  }
  int selectedStudyCount = 0;
  const ctkDICOMPatientView* patientView = qobject_cast<const ctkDICOMPatientView*>(option.widget);
  if (patientView)
  {
    ctkDICOMStudyListView* studyListView = patientView->studyListView();
    if (studyListView)
    {
      selectedStudyCount = studyListView->selectedCount();
    }
  }
  // Compose study info string with selection
  QString studyInfo;
  if (filteredStudyCount != studyCount)
  {
    studyInfo = QString("%1/%2 %3").arg(filteredStudyCount).arg(studyCount).arg(studyCount == 1 ? "study" : "studies");
  }
  else
  {
    studyInfo = QString("%1 %2").arg(studyCount).arg(studyCount == 1 ? "study" : "studies");
  }
  if (selectedStudyCount > 0)
  {
    studyInfo += QString(" (%1)").arg(selectedStudyCount);
  }
  infoParts << studyInfo;

  // Add insert date time
  if (insertDateTime.isValid())
  {
    QString dateAddedText = QString("Date added: %1").arg(insertDateTime.toString("dd MMM yyyy hh:mm"));
    infoParts << dateAddedText;
  }

  QString infoText = infoParts.join("  |  ");
  QRect infoRect(contentRect.left(), currentY, contentRect.width(), 18);
  QFontMetrics infoFm(infoFont);
  if (infoFm.horizontalAdvance(infoText) > infoRect.width())
  {
    QString elidedInfoText = infoFm.elidedText(infoText, Qt::ElideRight, infoRect.width());
    elidedInfoText += "...";
    painter->drawText(infoRect, Qt::AlignLeft | Qt::AlignVCenter, elidedInfoText);
  }
  else
  {
    painter->drawText(infoRect, Qt::AlignLeft | Qt::AlignVCenter, infoText);
  }

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::paintListMode(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  Q_D(const ctkDICOMPatientDelegate);

  // In ListMode, the QSplitter will handle clipping to the patient area
  // We still need patientView for hover state and study list access
  const ctkDICOMPatientView* patientView = qobject_cast<const ctkDICOMPatientView*>(option.widget);

  painter->save();

  QRect itemRect = option.rect;

  // Determine colors based on selection state
  bool isSelected = option.state & QStyle::State_Selected;
  bool isCurrent = option.state & QStyle::State_HasFocus;

  // Use view's custom hover tracking instead of Qt's internal hover state
  bool isHovered = false;
  if (patientView)
  {
    isHovered = patientView->isIndexHovered(index);
  }

  QColor backgroundColor;
  QColor borderColor;

  if (isSelected && isHovered)
  {
    backgroundColor = QColor(70, 130, 180, 100);
    borderColor = QColor(160, 160, 160);
  }
  else if (isSelected)
  {
    backgroundColor = QColor(70, 130, 180, 70);
    borderColor = QColor(180, 180, 180);
  }
  else if (isHovered)
  {
    backgroundColor = QColor(70, 130, 180, 35);
    borderColor = QColor(200, 200, 200);
  }
  else
  {
    backgroundColor = QColor(250, 250, 250);
    borderColor = QColor(220, 220, 220);
  }

  if (isCurrent)
  {
    borderColor = QColor(70, 130, 180);
  }

  QColor textColor = QColor(0, 0, 0);
  QColor secondaryTextColor = QColor(108, 117, 125);
  int borderWidth = 2;

  // Calculate patient item rect (single line height)
  QRect patientItemRect = this->patientsRect(itemRect, option, index);

  // Draw card background with rounded corners
  QPainterPath path;
  path.addRoundedRect(patientItemRect, d->CornerRadius, d->CornerRadius);
  painter->fillPath(path, backgroundColor);

  // Draw border
  painter->setPen(QPen(borderColor, borderWidth));
  painter->drawPath(path);

  // Draw patient icon from SVG
  int iconSize = d->IconSize;
  int iconMargin = d->Spacing * 2;
  QRect iconRect(patientItemRect.left() + iconMargin,
                 patientItemRect.top() + (patientItemRect.height() - iconSize) / 2,
                 iconSize, iconSize);

  int operationStatus = index.data(ctkDICOMPatientModel::OperationStatusRole).toInt();
  QIcon patientIcon;
  if (operationStatus == ctkDICOMPatientModel::NoOperation)
  {
    patientIcon = QIcon(":/Icons/patient.svg");
  }
  else if (operationStatus == ctkDICOMPatientModel::InProgress)
  {
    patientIcon = QIcon(":/Icons/patient_pending.svg");
  }
  else if (operationStatus == ctkDICOMPatientModel::Failed)
  {
    patientIcon = QIcon(":/Icons/patient_failed.svg");
  }
  else if (operationStatus == ctkDICOMPatientModel::Completed)
  {
    patientIcon = QIcon(":/Icons/patient_success.svg");
  }


  if (!patientIcon.isNull())
  {
    QPixmap iconPixmap = patientIcon.pixmap(QSize(iconSize, iconSize));
    if (!iconPixmap.isNull())
    {
      painter->drawPixmap(iconRect, iconPixmap);
    }
  }

  // Get patient data from model
  QString patientName = index.data(ctkDICOMPatientModel::PatientNameRole).toString();
  QString patientID = index.data(ctkDICOMPatientModel::PatientIDRole).toString();
  QString patientBirthDate = index.data(ctkDICOMPatientModel::PatientBirthDateRole).toString();
  QString patientSex = index.data(ctkDICOMPatientModel::PatientSexRole).toString();
  int studyCount = index.data(ctkDICOMPatientModel::StudyCountRole).toInt();
  int filteredStudyCount = index.data(ctkDICOMPatientModel::FilteredStudyCountRole).toInt();

  if (patientName.isEmpty())
  {
    patientName = "Anonymous";
  }

  // Compose info text
  QStringList infoParts;
  if (!patientID.isEmpty())
  {
    infoParts << QString("MRN: %1").arg(patientID);
  }
  if (!patientBirthDate.isEmpty())
  {
    infoParts << QString("Birth Date: %1").arg(this->formatPatientBirthDate(patientBirthDate));
  }
  if (!patientSex.isEmpty())
  {
    infoParts << QString("Sex: %1").arg(patientSex);
  }
  QString studyInfo;
  if (filteredStudyCount != studyCount)
  {
    studyInfo = QString("%1/%2 %3").arg(filteredStudyCount).arg(studyCount).arg(studyCount == 1 ? "study" : "studies");
  }
  else
  {
    studyInfo = QString("%1 %2").arg(studyCount).arg(studyCount == 1 ? "study" : "studies");
  }
  int selectedStudyCount = 0;
  if (patientView)
  {
    ctkDICOMStudyListView* studyListView = patientView->studyListView();
    if (studyListView)
    {
      selectedStudyCount = studyListView->numberOfStudiesSelectedByPatient(patientView->patientUID(index));
    }
  }
  if (selectedStudyCount > 0)
  {
    studyInfo += QString(" (%1)").arg(selectedStudyCount);
  }
  infoParts << studyInfo;
  QDateTime insertDateTime = index.data(ctkDICOMPatientModel::PatientInsertDateTimeRole).toDateTime();
  if (insertDateTime.isValid())
  {
    infoParts << QString("Date added: %1").arg(insertDateTime.toString("dd MMM yyyy hh:mm"));
  }
  QString infoText = infoParts.join("  |  ");

  // Set font for all text
  QFont textFont = painter->font();
  painter->setFont(textFont);

  // Draw patient name (bold)
  QFont nameFont = textFont;
  nameFont.setBold(true);
  painter->setFont(nameFont);
  painter->setPen(textColor);

  QFontMetrics nameFm(nameFont);
  int nameWidth = nameFm.horizontalAdvance(patientName);
  QRect nameRect(iconRect.right() + d->Spacing * 2, patientItemRect.top() + patientItemRect.height() / 2 - nameFm.height() / 2, nameWidth, nameFm.height());
  painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, patientName);

  // Draw info text, wrap if needed
  painter->setFont(textFont);
  painter->setPen(secondaryTextColor);
  QFontMetrics fm(textFont);
  int infoX = nameRect.right() + d->Spacing * 3;
  int infoY = patientItemRect.top() + patientItemRect.height() / 2 - fm.height() / 2;
  int infoWidth = patientItemRect.right() - infoX - d->Spacing;
  QRect infoRect(infoX, infoY, infoWidth, fm.height());
  if (fm.horizontalAdvance(infoText) > infoRect.width())
  {
    QString elidedInfoText = fm.elidedText(infoText, Qt::ElideRight, infoRect.width());
    elidedInfoText += "...";
    painter->drawText(infoRect, Qt::AlignLeft | Qt::AlignVCenter, elidedInfoText);
  } else
  {
    painter->drawText(infoRect, Qt::AlignLeft | Qt::AlignVCenter, infoText);
  }

  painter->restore();

  // Draw interactive icons when hovering or when context menu is active
  this->paintSelectAllIcon(painter, itemRect, index, option);
  this->paintContextMenuButton(painter, itemRect, index, option);
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientDelegate::formatPatientBirthDate(const QString& date) const
{
  if (date.isEmpty())
  {
    return QString();
  }

  // Try to parse DICOM date format (YYYYMMDD or YYYY-MM-DD)
  QString cleanDate = date;
  cleanDate.remove('-');

  if (cleanDate.length() >= 8)
  {
    QDate parsedDate = QDate::fromString(cleanDate, "yyyyMMdd");
    if (parsedDate.isValid())
    {
      return parsedDate.toString("dd MMM yyyy");
    }
  }

  return date;
}

//------------------------------------------------------------------------------
ctkDICOMPatientView::DisplayMode ctkDICOMPatientDelegate::getDisplayMode(const QStyleOptionViewItem& option) const
{
  // Try to get the view from the option widget
  const QWidget* widget = option.widget;
  const ctkDICOMPatientView* patientView = qobject_cast<const ctkDICOMPatientView*>(widget);

  if (patientView)
  {
    return patientView->displayMode();
  }

  return ctkDICOMPatientView::TabMode;
}

//------------------------------------------------------------------------------
QRect ctkDICOMPatientDelegate::patientsRect(const QRect &itemRect,
                                            const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
  Q_D(const ctkDICOMPatientDelegate);
  ctkDICOMPatientView::DisplayMode displayMode = this->getDisplayMode(option);
  if (displayMode == ctkDICOMPatientView::TabMode)
  {
    QString patientName = index.data(ctkDICOMPatientModel::PatientNameRole).toString();
    if (patientName.isEmpty())
    {
      patientName = "Anonymous";
    }

    QFont font = option.font;
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(patientName);
    if (textWidth > d->MaxTextWidth)
    {
      textWidth = d->MaxTextWidth;
    }
    int width = d->IconSize * 2 + d->Spacing + textWidth;
    QRect tabRect = itemRect.adjusted(d->Spacing * 1.5, d->Spacing, 0, 0);
    tabRect.setHeight(d->TabModeHeight);
    tabRect.setWidth(qMax(width, 80));
    return tabRect;
  }
  else // ListMode
  {
    QRect listRect = itemRect.adjusted(d->Spacing, d->Spacing, -d->Spacing, 0);
    listRect.setHeight(d->TabModeHeight - d->Spacing);
    return listRect;
  }
}

//------------------------------------------------------------------------------
QRect ctkDICOMPatientDelegate::patientHeaderRect(const QRect& patientsRect,
                                                 const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMPatientDelegate);
  ctkDICOMPatientView::DisplayMode displayMode = this->getDisplayMode(option);
  if (displayMode == ctkDICOMPatientView::TabMode)
  {
    int top = patientsRect.bottom() + d->Spacing * 2;
    int viewportWidth = option.widget ? option.widget->width() : patientsRect.width();
    return QRect(d->Spacing * 1.5, top, viewportWidth - d->Spacing * 2.5, d->PatientHeaderHeight);
  }
  else // ListMode
  {
    // In ListMode, the patient header is the same as patientsListRect (single line)
    return patientsRect;
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::setSpacing(int spacing)
{
  Q_D(ctkDICOMPatientDelegate);
  d->Spacing = spacing;
  emit this->spacingChanged(spacing);
}

//------------------------------------------------------------------------------
int ctkDICOMPatientDelegate::spacing() const
{
  Q_D(const ctkDICOMPatientDelegate);
  return d->Spacing;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::setIconSize(int iconSize)
{
  Q_D(ctkDICOMPatientDelegate);
  d->IconSize = iconSize;
  emit this->iconSizeChanged(iconSize);
}

//------------------------------------------------------------------------------
int ctkDICOMPatientDelegate::iconSize() const
{
  Q_D(const ctkDICOMPatientDelegate);
  return d->IconSize;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::setCornerRadius(int radius)
{
  Q_D(ctkDICOMPatientDelegate);
  d->CornerRadius = radius;
  emit this->cornerRadiusChanged(radius);
}

//------------------------------------------------------------------------------
int ctkDICOMPatientDelegate::cornerRadius() const
{
  Q_D(const ctkDICOMPatientDelegate);
  return d->CornerRadius;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::setTabModeHeight(int height)
{
  Q_D(ctkDICOMPatientDelegate);
  d->TabModeHeight = height;
  emit this->tabModeHeightChanged(height);
}

//------------------------------------------------------------------------------
int ctkDICOMPatientDelegate::tabModeHeight() const
{
  Q_D(const ctkDICOMPatientDelegate);
  return d->TabModeHeight;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::setPatientHeaderHeight(int height)
{
  Q_D(ctkDICOMPatientDelegate);
  d->PatientHeaderHeight = height;
  emit this->patientHeaderHeightChanged(height);
}

//------------------------------------------------------------------------------
int ctkDICOMPatientDelegate::patientHeaderHeight() const
{
  Q_D(const ctkDICOMPatientDelegate);
  return d->PatientHeaderHeight;
}

//------------------------------------------------------------------------------
QRect ctkDICOMPatientDelegate::contextMenuButtonRect(const QRect& itemRect, const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMPatientDelegate);

  ctkDICOMPatientView::DisplayMode displayMode = this->getDisplayMode(option);

  if (displayMode == ctkDICOMPatientView::TabMode)
  {
    // For TabMode, position button at right edge of the patient header, vertically centered
    QRect tabRect = this->patientsRect(itemRect, option, QModelIndex());
    QRect patientHeaderRect = this->patientHeaderRect(tabRect, option);
    return QRect(patientHeaderRect.right() - d->IconSize - d->Spacing,
                patientHeaderRect.top() + (patientHeaderRect.height() - d->IconSize) / 2,
                d->IconSize,
                d->IconSize);
  }
  else // ListMode
  {
    // For ListMode, position button at top-right of the item card
    QRect listRect = this->patientsRect(itemRect, option, QModelIndex());
    return QRect(listRect.right() - d->IconSize - d->Spacing * 2,
                listRect.center().y() - d->IconSize * 0.5 + 1,
                d->IconSize,
                d->IconSize);
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientDelegate::isContextMenuButtonAt(const QPoint& pos, const QRect& itemRect, const QStyleOptionViewItem& option) const
{
  QRect buttonRect = this->contextMenuButtonRect(itemRect, option);
  return buttonRect.contains(pos);
}

//------------------------------------------------------------------------------
QRect ctkDICOMPatientDelegate::selectAllIconRect(const QRect& itemRect, const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMPatientDelegate);

  // Position select all icon to the left of context menu button
  QRect contextButtonRect = this->contextMenuButtonRect(itemRect, option);

  return QRect(contextButtonRect.left() - d->IconSize - d->Spacing * 0.5,
              contextButtonRect.top(),
              d->IconSize,
              d->IconSize);
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientDelegate::isSelectAllIconAt(const QPoint& pos, const QRect& itemRect, const QStyleOptionViewItem& option) const
{
  QRect iconRect = this->selectAllIconRect(itemRect, option);
  return iconRect.contains(pos);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::paintContextMenuButton(QPainter* painter,
                                                     const QRect& itemRect,
                                                     const QModelIndex& index,
                                                     const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMPatientDelegate);
  QRect buttonRect = this->contextMenuButtonRect(itemRect, option);

  // Check button state from the view
  bool isPressed = false;
  bool isHovered = false;
  bool hasActiveContextMenu = false;
  bool itemHoveredState = false;
  const ctkDICOMPatientView* patientView = nullptr;

  // Try the widget from option first (most reliable)
  if (option.widget)
  {
    patientView = qobject_cast<const ctkDICOMPatientView*>(option.widget);
  }
  // If that fails, try parent
  if (!patientView)
  {
    patientView = qobject_cast<const ctkDICOMPatientView*>(this->parent());
  }

  if (patientView)
  {
    if (patientView->displayMode() == ctkDICOMPatientView::TabMode)
    {
      // In TabMode, only show button for current patient
      if (index != patientView->currentPatientIndex())
      {
        return;
      }
    }
    isPressed = patientView->isContextMenuButtonPressed(index);
    isHovered = patientView->isContextMenuButtonHovered(index);
    hasActiveContextMenu = patientView->hasActiveContextMenu(index);
    itemHoveredState = patientView->isIndexHovered(index);
  }

  if (!itemHoveredState &&
      !hasActiveContextMenu &&
      !isPressed &&
      !isHovered)
  {
    return;
  }

  painter->save();

  QColor bgColor;
  if (isPressed || hasActiveContextMenu)
  {
    bgColor = Qt::gray;
  }
  else if (isHovered)
  {
    bgColor = Qt::lightGray;
  }
  else
  {
    bgColor = Qt::white;
  }
  bgColor.setAlpha(200);

  QPainterPath bgPath;
  int bgRadius = d->CornerRadius * 0.5;
  bgPath.addRoundedRect(buttonRect, bgRadius, bgRadius);
  painter->fillPath(bgPath, bgColor);

  QString iconResource = ":/Icons/more_vert.svg";
  QIcon selectIcon(iconResource);
  selectIcon.paint(painter, buttonRect);

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientDelegate::paintSelectAllIcon(QPainter* painter,
                                                 const QRect& itemRect,
                                                 const QModelIndex& index,
                                                 const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMPatientDelegate);
  QRect iconRect = this->selectAllIconRect(itemRect, option);

  // Check icon state from the view
  bool isPressed = false;
  bool isHovered = false;
  bool itemHoveredState = false;
  int selectedSeriesCount = 0;
  const ctkDICOMPatientView* patientView = nullptr;

  // Try the widget from option first (most reliable)
  if (option.widget)
  {
    patientView = qobject_cast<const ctkDICOMPatientView*>(option.widget);
  }
  // If that fails, try parent
  if (!patientView)
  {
    patientView = qobject_cast<const ctkDICOMPatientView*>(this->parent());
  }

  if (patientView)
  {
    if (patientView->displayMode() == ctkDICOMPatientView::TabMode)
    {
      // In TabMode, only show icon for current patient
      if (index != patientView->currentPatientIndex())
      {
        return;
      }
    }
    isPressed = patientView->isSelectAllIconPressed(index);
    isHovered = patientView->isSelectAllIconHovered(index);
    itemHoveredState = patientView->isIndexHovered(index);

    // Get study count information
    ctkDICOMStudyListView* studyListView = patientView->studyListView();
    if (studyListView)
    {
      selectedSeriesCount = studyListView->numberOfSeriesSelectedByPatient(patientView->patientUID(index));
    }
  }

  if (!itemHoveredState && !isPressed && !isHovered)
  {
    return;
  }

  painter->save();

  // Draw background when hovered or pressed
  QColor bgColor;
  if (isPressed)
  {
    bgColor = Qt::gray;
  }
  else if (isHovered)
  {
    bgColor = Qt::lightGray;
  }
  else
  {
    bgColor = Qt::white;
  }
  bgColor.setAlpha(200);

  QPainterPath bgPath;
  int bgRadius = d->CornerRadius * 0.5;
  bgPath.addRoundedRect(iconRect, bgRadius, bgRadius);
  painter->fillPath(bgPath, bgColor);

  // Determine which icon to show based on selection state
  QString iconResource;
  if (selectedSeriesCount > 0)
  {
    iconResource = ":/Icons/deselect.svg";
  }
  else
  {
    // Not all studies are selected - show select all icon
    iconResource = ":/Icons/select_all.svg";
  }

  QIcon icon(iconResource);
  icon.paint(painter, iconRect);

  painter->restore();
}
