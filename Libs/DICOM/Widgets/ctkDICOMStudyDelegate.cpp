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
#include <QRadialGradient>
#include <QCheckBox>
#include <QIcon>
#include <QDate>
#include <QTime>
#include <QEvent>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>

// Qt includes
#include <QGridLayout>
#include <QWidget>

// CTK includes
#include "ctkDICOMStudyDelegate.h"
#include "ctkDICOMStudyModel.h"
#include "ctkDICOMStudyFilterProxyModel.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMSeriesFilterProxyModel.h"
#include "ctkDICOMStudyMergedFilterProxyModel.h"
#include "ctkDICOMSeriesDelegate.h"
#include "ctkDICOMSeriesTableView.h"
#include "ctkDICOMStudyListView.h"

// STD includes
#include <cmath>

//------------------------------------------------------------------------------
class ctkDICOMStudyDelegatePrivate
{
public:
  ctkDICOMStudyDelegatePrivate();

  int Spacing;
  int CornerRadius;
  int CollapsedHeight;
  int ExpandedHeight;
  int IconSize;
  int seriesInfoWidth;
};

//------------------------------------------------------------------------------
ctkDICOMStudyDelegatePrivate::ctkDICOMStudyDelegatePrivate()
{
  this->Spacing = 4;
  this->CornerRadius = 8;
  this->CollapsedHeight = 55;
  this->ExpandedHeight = 300;
  this->IconSize = 24;
  this->seriesInfoWidth = 150;
}

//------------------------------------------------------------------------------
ctkDICOMStudyDelegate::ctkDICOMStudyDelegate(QObject* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMStudyDelegatePrivate)
{
}

//------------------------------------------------------------------------------
ctkDICOMStudyDelegate::~ctkDICOMStudyDelegate()
{
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return;
  }

  painter->setRenderHint(QPainter::Antialiasing, true);

  // Get item rect with some margin for the card effect
  QRect itemRect = option.rect;

  // Get the view widget to check states
  const ctkDICOMStudyListView* listView = nullptr;
  // Try the widget from option first (most reliable)
  if (option.widget)
  {
    listView = qobject_cast<const ctkDICOMStudyListView*>(option.widget);
  }
  // If that fails, try parent
  if (!listView)
  {
    listView = qobject_cast<const ctkDICOMStudyListView*>(this->parent());
  }

  // Determine which icon to show based on selection state
  int numberOfSeriesSelected = 0;
  if (listView)
  {
    numberOfSeriesSelected = listView->numberOfSeriesSelectedByStudy(index);
  }

  // Paint card background
  QRect cardRect = this->studyCardRect(itemRect);
  this->paintCardBackground(painter, cardRect, index, option);

  // Paint study header
  QRect headerRect = this->studyHeaderRect(itemRect);
  this->paintStudyHeader(painter, headerRect, index, option, numberOfSeriesSelected);
}

//------------------------------------------------------------------------------
QSize ctkDICOMStudyDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  Q_D(const ctkDICOMStudyDelegate);
  Q_UNUSED(option);

  // Return appropriate height based on collapsed state
  int height;
  if (this->isStudyCollapsed(index))
  {
    height = d->CollapsedHeight;
  }
  else
  {
    // Calculate dynamic height based on series count and thumbnail size
    height = d->CollapsedHeight + this->calculateSeriesAreaHeight(index);
  }

  // Add margins for card effect and spacing between studies
  // Since list view spacing is now 0, we add extra margin for visual separation
  height += d->Spacing;

  // Width should fill the view
  return QSize(-1, height);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::setSpacing(int spacing)
{
  Q_D(ctkDICOMStudyDelegate);
  d->Spacing = spacing;
  emit this->spacingChanged(spacing);
}

//------------------------------------------------------------------------------
int ctkDICOMStudyDelegate::spacing() const
{
  Q_D(const ctkDICOMStudyDelegate);
  return d->Spacing;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::setCornerRadius(int radius)
{
  Q_D(ctkDICOMStudyDelegate);
  d->CornerRadius = radius;
  emit this->cornerRadiusChanged(radius);
}

//------------------------------------------------------------------------------
int ctkDICOMStudyDelegate::cornerRadius() const
{
  Q_D(const ctkDICOMStudyDelegate);
  return d->CornerRadius;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::setCollapsedHeight(int height)
{
  Q_D(ctkDICOMStudyDelegate);
  d->CollapsedHeight = height;
  emit this->collapsedHeightChanged(height);
}

//------------------------------------------------------------------------------
int ctkDICOMStudyDelegate::collapsedHeight() const
{
  Q_D(const ctkDICOMStudyDelegate);
  return d->CollapsedHeight;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::setExpandedHeight(int height)
{
  Q_D(ctkDICOMStudyDelegate);
  d->ExpandedHeight = height;
  emit this->expandedHeightChanged(height);
}

//------------------------------------------------------------------------------
int ctkDICOMStudyDelegate::expandedHeight() const
{
  Q_D(const ctkDICOMStudyDelegate);
  return d->ExpandedHeight;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::paintStudyHeader(QPainter* painter,
                                             const QRect& rect,
                                             const QModelIndex& index,
                                             const QStyleOptionViewItem& option,
                                             int numberOfSeriesSelected) const
{
  // Paint collapse indicator
  QRect collapseRect = this->collapseIndicatorRect(rect);
  this->paintCollapseIndicator(painter, collapseRect, index, option);

  // Paint study information
  QRect studyInfoRect = this->studyInfoRect(rect);
  this->paintStudyInfo(painter, studyInfoRect, index, option);

  // Paint series count and status
  QRect seriesInfoRect = this->seriesInfoRect(rect);
  this->paintSeriesInfo(painter, seriesInfoRect, index, numberOfSeriesSelected);

  // Paint select all icon
  QRect selectAllIconRect = this->selectAllIconRect(rect);
  this->paintSelectAllIcon(painter, selectAllIconRect, index, option);

  // Draw context menu button (3 dots) when hovering or when context menu is active
  QRect contextMenuIconRect = this->contextMenuButtonRect(rect);
  this->paintContextMenuButton(painter, contextMenuIconRect, index, option);

  /// Paint operation status button
  QRect operationStatusIconRect = this->operationStatusButtonRect(rect);
  this->paintOperationStatusButton(painter, operationStatusIconRect, index, option);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::paintCollapseIndicator(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMStudyDelegate);

  painter->save();

  bool collapseIconHovered = false;
  bool collapseIconPressed = false;
  // Get the view widget to check states
  const ctkDICOMStudyListView* listView = nullptr;
  // Try the widget from option first (most reliable)
  if (option.widget)
  {
    listView = qobject_cast<const ctkDICOMStudyListView*>(option.widget);
  }
  // If that fails, try parent
  if (!listView)
  {
    listView = qobject_cast<const ctkDICOMStudyListView*>(this->parent());
  }
  if (listView)
  {
    collapseIconHovered = listView->isCollapseIndicatorHovered(index);
    collapseIconPressed = listView->isCollapseIndicatorPressed(index);
  }

  // Apply hover and press effects
  if (collapseIconHovered || collapseIconPressed)
  {
    // Draw background for hover/press effect
    QColor bgColor;
    // Press effect - darker background
    if (collapseIconPressed)
    {
      bgColor = QColor(0, 0, 0, 40);
    }
    // Hover effect - lighter background
    else if (collapseIconHovered)
    {
      bgColor = QColor(0, 0, 0, 20);
    }

    // Draw rounded background using the exact icon rect
    QPainterPath bgPath;
    int bgRadius = d->CornerRadius * 0.5;
    bgPath.addRoundedRect(rect, bgRadius, bgRadius);
    painter->fillPath(bgPath, bgColor);
  }

  bool collapsed = this->isStudyCollapsed(index);

  // Load the appropriate icon
  QString iconResource = collapsed ? ":/Icons/expand_right.svg" : ":/Icons/expand_down.svg";
  QIcon selectIcon(iconResource);
  // Draw the icon
  selectIcon.paint(painter, rect);

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::paintStudyInfo(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMStudyDelegate);
  painter->save();

  // Get study data
  QString studyID = index.data(ctkDICOMStudyModel::StudyIDRole).toString();
  QString studyDescription = index.data(ctkDICOMStudyModel::StudyDescriptionRole).toString();
  QString studyDate = index.data(ctkDICOMStudyModel::StudyDateRole).toString();
  QString studyTime = index.data(ctkDICOMStudyModel::StudyTimeRole).toString();

  // Set up fonts
  QFont font = option.font;

  // Color scheme
  QColor textColor = option.palette.color(QPalette::Text);
  QColor secondaryTextColor = textColor;
  secondaryTextColor.setAlpha(180);

  // Layout variables
  int currentY = rect.top() + d->Spacing;
  int lineHeight = QFontMetrics(font).height();

  // Draw study title
  QString studyTitle;
  if (!studyID.isEmpty())
  {
    studyTitle = studyID;
  }

  if (!studyDescription.isEmpty())
  {
    studyTitle += (!studyTitle.isEmpty() ? " - " : "");
    studyTitle += studyDescription;
  }

  if (studyTitle.isEmpty())
  {
    studyTitle = tr("Anonomyzed study");
  }

  const QAbstractItemModel* model = index.model();
  const ctkDICOMStudyMergedFilterProxyModel* mergedProxyModel = qobject_cast<const ctkDICOMStudyMergedFilterProxyModel*>(model);
  if (mergedProxyModel)
  {
    // Get patient info from merged proxy model
    QString patientName = index.data(ctkDICOMStudyModel::PatientNameRole).toString();
    QString patientID = index.data(ctkDICOMStudyModel::PatientIDRole).toString();
    QString patientBirth = index.data(ctkDICOMStudyModel::PatientBirthDateRole).toString();

    QString patientInfo;
    if (!patientName.isEmpty())
    {
      patientInfo += patientName;
    }
    if (!patientID.isEmpty())
    {
      patientInfo += (!patientInfo.isEmpty() ? ", " : "") + patientID;
    }
    if (!patientBirth.isEmpty())
    {
      patientInfo += (!patientInfo.isEmpty() ? ", " : "") + this->formatStudyDate(patientBirth);
    }

    if (!patientInfo.isEmpty())
    {
      studyTitle = studyTitle + " (" + patientInfo + ")";;
    }
  }

  painter->setFont(font);
  painter->setPen(textColor);

  QRect descRect(rect.left(), currentY, rect.width(), lineHeight);
  QString elidedDesc = QFontMetrics(font).elidedText(studyTitle, Qt::ElideRight, descRect.width());
  painter->drawText(descRect, Qt::AlignLeft | Qt::AlignVCenter, elidedDesc);
  currentY += lineHeight + d->Spacing * 0.5;

  // Draw study date and time
  painter->setFont(font);
  painter->setPen(secondaryTextColor);
  lineHeight = QFontMetrics(font).height();

  QString dateTimeStr;
  if (!studyDate.isEmpty())
  {
    dateTimeStr = this->formatStudyDate(studyDate);
    if (!studyTime.isEmpty())
    {
      dateTimeStr += " " + this->formatStudyTime(studyTime);
    }
  }

  if (!dateTimeStr.isEmpty())
  {
    QRect dateRect(rect.left(), currentY, rect.width(), lineHeight);
    QString elidedDateTime = QFontMetrics(font).elidedText(dateTimeStr, Qt::ElideRight, dateRect.width());
    painter->drawText(dateRect, Qt::AlignLeft | Qt::AlignVCenter, elidedDateTime);
  }

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::paintSeriesInfo(QPainter* painter, const QRect& rect, const QModelIndex& index, int numberOfSeriesSelected) const
{
  painter->save();

  // Get series data
  int seriesCount = index.data(ctkDICOMStudyModel::SeriesCountRole).toInt();
  int filteredSeriesCount = index.data(ctkDICOMStudyModel::FilteredSeriesCountRole).toInt();

  // Set up font
  QFont font;
  font.setBold(true);
  painter->setFont(font);

  // Color
  painter->setPen(QColor(70, 130, 180)); // Steel blue

  // Create series count text
  QString seriesText;
  if (filteredSeriesCount != seriesCount)
  {
    seriesText = QString("%1/%2 series").arg(filteredSeriesCount).arg(seriesCount);
  }
  else
  {
    seriesText = QString("%1 series").arg(seriesCount);
  }

  if (numberOfSeriesSelected > 0)
  {
    seriesText += QString(" (%1)").arg(numberOfSeriesSelected);
  }

  // Calculate text metrics
  QFontMetrics fontMetrics(font);
  QRect textRect = fontMetrics.boundingRect(seriesText);

  // Draw series count text (left-aligned in the rect)
  QRect leftAlignedTextRect = rect;
  leftAlignedTextRect.setWidth(textRect.width() + 10); // Add some padding
  painter->drawText(leftAlignedTextRect, Qt::AlignLeft | Qt::AlignVCenter, seriesText);

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::paintSelectAllIcon(QPainter* painter,
                                               const QRect& rect,
                                               const QModelIndex& index,
                                               const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMStudyDelegate);

  bool isSelected = option.state & QStyle::State_Selected;

  // Get hover/press states for select all icon from the list view
  bool selectIconHovered = false;
  bool selectIconPressed = false;
  // Get the view widget to check states
  const ctkDICOMStudyListView* listView = nullptr;
  // Try the widget from option first (most reliable)
  if (option.widget)
  {
    listView = qobject_cast<const ctkDICOMStudyListView*>(option.widget);
  }
  // If that fails, try parent
  if (!listView)
  {
    listView = qobject_cast<const ctkDICOMStudyListView*>(this->parent());
  }
  if (listView)
  {
    selectIconHovered = listView->isSelectAllIconHovered(index);
    selectIconPressed = listView->isSelectAllIconPressed(index);
  }

  bool studyHoveredState = option.state & QStyle::State_MouseOver;
  if (!studyHoveredState &&
      !selectIconHovered &&
      !selectIconPressed)
  {
    return;
  }

  painter->save();


  QColor bgColor;
  if (selectIconPressed)
  {
    bgColor = Qt::gray;
  }
  else if (selectIconHovered)
  {
    bgColor = Qt::lightGray;
  }
  else
  {
    bgColor = Qt::white;
  }
  bgColor.setAlpha(200);

  // Draw rounded background using the exact icon rect
  QPainterPath bgPath;
  int bgRadius = d->CornerRadius * 0.5;
  bgPath.addRoundedRect(rect, bgRadius, bgRadius);
  painter->fillPath(bgPath, bgColor);

  QString iconResource = isSelected != 0 ? ":/Icons/deselect.svg" : ":/Icons/select_all.svg";
  QIcon selectIcon(iconResource);
  selectIcon.paint(painter, rect);

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::paintCardBackground(QPainter* painter,
                                                const QRect& rect,
                                                const QModelIndex& index,
                                                const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMStudyDelegate);
  Q_UNUSED(index);

  // Check for selection, hover, and focus (current item) state
  bool isSelected = option.state & QStyle::State_Selected;
  bool isHovered = option.state & QStyle::State_MouseOver;
  bool isCurrent = option.state & QStyle::State_HasFocus;

  painter->save();

  // Create card path with rounded corners
  QPainterPath cardPath;
  cardPath.addRoundedRect(rect, d->CornerRadius, d->CornerRadius);

  int borderWidth = 2;
  QColor backgroundColor;
  QColor borderColor;

  if (isSelected && isHovered)
  {
    backgroundColor = QColor(200, 200, 200, 75);
    borderColor = QColor(160, 160, 160);
  }
  else if (isSelected)
  {
    backgroundColor = QColor(200, 200, 200, 50);
    borderColor = QColor(180, 180, 180);
  }
  else if (isHovered)
  {
    backgroundColor = QColor(200, 200, 200, 25);
    borderColor = QColor(200, 200, 200);
  }
  else
  {
    backgroundColor = QColor(200, 200, 200, 0);
    borderColor = QColor(220, 220, 220);
  }

  if (isCurrent)
  {
    borderColor = QColor(70, 130, 180);
  }

  // Fill card background
  painter->fillPath(cardPath, backgroundColor);

  // Card border
  painter->setPen(QPen(borderColor, borderWidth));
  painter->setBrush(Qt::NoBrush);
  painter->drawPath(cardPath);

  painter->restore();
}

//------------------------------------------------------------------------------
QRect ctkDICOMStudyDelegate::studyCardRect(const QRect& itemRect) const
{
  Q_D(const ctkDICOMStudyDelegate);
  return QRect(itemRect.left() + d->Spacing * 0.5, itemRect.top() + d->Spacing * 0.5, itemRect.width() - d->Spacing * 2, itemRect.height() - d->Spacing);
}

//------------------------------------------------------------------------------
QRect ctkDICOMStudyDelegate::studyHeaderRect(const QRect& itemRect) const
{
  Q_D(const ctkDICOMStudyDelegate);
  return QRect(itemRect.left() + d->Spacing * 0.5, itemRect.top() + d->Spacing * 0.5, itemRect.width() - d->Spacing, d->CollapsedHeight);
}

//------------------------------------------------------------------------------
QRect ctkDICOMStudyDelegate::collapseIndicatorRect(const QRect& headerRect) const
{
  Q_D(const ctkDICOMStudyDelegate);
  return QRect(headerRect.left() + d->Spacing,
               headerRect.center().y() - d->IconSize * 0.5,
               d->IconSize, d->IconSize);
}

//------------------------------------------------------------------------------
QRect ctkDICOMStudyDelegate::studyInfoRect(const QRect& headerRect) const
{
  Q_D(const ctkDICOMStudyDelegate);
  QRect collapseRect = this->collapseIndicatorRect(headerRect);
  QRect seriesRect = this->seriesInfoRect(headerRect);

  // Study info takes the middle area
  int left = collapseRect.right() + d->Spacing * 3;
  int right = seriesRect.left() - d->Spacing;

  return QRect(left, headerRect.top(), right - left, headerRect.height());
}

//------------------------------------------------------------------------------
QRect ctkDICOMStudyDelegate::seriesInfoRect(const QRect& headerRect) const
{
  Q_D(const ctkDICOMStudyDelegate);
  return QRect(headerRect.right() - d->seriesInfoWidth - d->Spacing - d->IconSize * 2,
               headerRect.top(),
               d->seriesInfoWidth, headerRect.height());
}

//------------------------------------------------------------------------------
QRect ctkDICOMStudyDelegate::seriesAreaRect(const QRect& itemRect, const QModelIndex& index) const
{
  Q_D(const ctkDICOMStudyDelegate);
  if (this->isStudyCollapsed(index))
  {
    return QRect();
  }

  // Calculate the required height based on series count, grid columns, and thumbnail size
  int calculatedHeight = this->calculateSeriesAreaHeight(index);

  // Series area below the header
  int top = itemRect.top() + d->CollapsedHeight;

  // Center the series area horizontally with some padding
  int horizontalPadding = d->Spacing; // Add more padding for centering effect
  int centeredLeft = itemRect.left() + horizontalPadding;
  int centeredWidth = itemRect.width() - (horizontalPadding * 4);

  return QRect(centeredLeft, top + d->Spacing, centeredWidth, calculatedHeight);
}

//------------------------------------------------------------------------------
QRect ctkDICOMStudyDelegate::selectAllIconRect(const QRect& headerRect) const
{
  Q_D(const ctkDICOMStudyDelegate);
  return QRect(headerRect.right() - d->Spacing * 10 - d->IconSize * 2,
               headerRect.center().y() - d->IconSize * 0.5,
               d->IconSize,
               d->IconSize);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyDelegate::isStudyCollapsed(const QModelIndex& index) const
{
  return index.data(ctkDICOMStudyModel::IsCollapsedRole).toBool();
}

//------------------------------------------------------------------------------
int ctkDICOMStudyDelegate::calculateSeriesAreaHeight(const QModelIndex& studyIndex) const
{
  Q_D(const ctkDICOMStudyDelegate);
  if (!studyIndex.isValid())
  {
    return d->ExpandedHeight;
  }

  const QAbstractItemModel* m = studyIndex.model();
  // Use const_cast to avoid casting away constness warning
  ctkDICOMStudyModel* model = qobject_cast<ctkDICOMStudyModel*>(const_cast<QAbstractItemModel*>(m));
  ctkDICOMStudyFilterProxyModel* proxyModel = qobject_cast<ctkDICOMStudyFilterProxyModel*>(const_cast<QAbstractItemModel*>(m));
  ctkDICOMStudyMergedFilterProxyModel* mergedProxyModel = qobject_cast<ctkDICOMStudyMergedFilterProxyModel*>(const_cast<QAbstractItemModel*>(m));

  ctkDICOMSeriesModel* seriesModel = nullptr;
  ctkDICOMSeriesFilterProxyModel* seriesFilterProxyModel = nullptr;
  if (proxyModel)
  {
    model = qobject_cast<ctkDICOMStudyModel*>(proxyModel->sourceModel());
    if (!model)
    {
      return d->ExpandedHeight;
    }

    // Get series model for this study
    QModelIndex sourceStudyIndex = proxyModel->mapToSource(studyIndex);
    seriesModel = model->seriesModelForStudy(sourceStudyIndex);
    seriesFilterProxyModel = model->seriesFilterProxyModelForStudy(sourceStudyIndex);
  }
  else if (mergedProxyModel)
  {
    QPair<ctkDICOMStudyModel*, QModelIndex> sourceInfo = mergedProxyModel->mapToSource(studyIndex);
    model = sourceInfo.first;
    if (!model)
    {
      return d->ExpandedHeight;
    }

    // Get series model for this study
    seriesModel = model->seriesModelForStudy(sourceInfo.second);
    seriesFilterProxyModel = model->seriesFilterProxyModelForStudy(sourceInfo.second);
  }
  else if (model)
  {
    // Directly from the study model
    seriesModel = model->seriesModelForStudy(studyIndex);
    seriesFilterProxyModel = model->seriesFilterProxyModelForStudy(studyIndex);
  }
  else
  {
    return d->ExpandedHeight;
  }

  if (!seriesModel || !seriesFilterProxyModel)
  {
    return d->ExpandedHeight;
  }

  int tableRows = seriesFilterProxyModel->rowCount();
  int spacing = d->Spacing;
  if (tableRows == 0)
  {
    return spacing * 2;
  }

  // Get the full height needed for each row from the series delegate
  // This includes thumbnail + text + spacing
  int rowHeight = 0;

  // Try to get the actual row height from the series table view's delegate
  if (tableRows > 0 && seriesFilterProxyModel->columnCount() > 0)
  {
    QModelIndex firstSeriesIndex = seriesFilterProxyModel->index(0, 0);
    if (firstSeriesIndex.isValid())
    {
      // Create a series delegate to get the proper size hint
      ctkDICOMSeriesDelegate seriesDelegate;
      QStyleOptionViewItem option;
      QSize seriesItemSize = seriesDelegate.sizeHint(option, firstSeriesIndex);
      rowHeight = seriesItemSize.height();
    }
  }

  // Fallback to thumbnail size only if we can't get the delegate size
  if (rowHeight == 0)
  {
    rowHeight = seriesModel->thumbnailSize();
  }

  // Calculate height: table rows * full row height + spacing between rows + padding
  int seriesAreaHeight = tableRows * rowHeight + (tableRows + 2) * spacing;
  return seriesAreaHeight;
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyDelegate::formatStudyDate(const QString& date) const
{
  if (date.length() == 8) // YYYYMMDD
  {
    QDate studyDate = QDate::fromString(date, "yyyyMMdd");
    if (studyDate.isValid())
    {
      return studyDate.toString("dd MMM yyyy");
    }
  }
  return date;
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyDelegate::formatStudyTime(const QString& time) const
{
  if (time.length() >= 6) // HHMMSS or HHMMSS.fff
  {
    QTime studyTime = QTime::fromString(time.left(6), "hhmmss");
    if (studyTime.isValid())
    {
      return studyTime.toString("hh:mm:ss");
    }
  }
  return time;
}

//------------------------------------------------------------------------------
QRect ctkDICOMStudyDelegate::contextMenuButtonRect(const QRect& headerRect) const
{
  Q_D(const ctkDICOMStudyDelegate);
  return QRect(headerRect.right() - d->Spacing * 9 - d->IconSize,
               headerRect.center().y() - d->IconSize * 0.5,
               d->IconSize,
               d->IconSize);
}

//------------------------------------------------------------------------------
QRect ctkDICOMStudyDelegate::operationStatusButtonRect(const QRect& headerRect) const
{
  Q_D(const ctkDICOMStudyDelegate);
  return QRect(headerRect.right() - d->Spacing * 8,
                headerRect.center().y() - d->IconSize * 0.5,
                d->IconSize,
                d->IconSize);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyDelegate::isContextMenuButtonAt(const QPoint& pos, const QRect& itemRect) const
{
  QRect headerRect = this->studyHeaderRect(itemRect);
  QRect buttonRect = this->contextMenuButtonRect(headerRect);
  return buttonRect.contains(pos);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::paintContextMenuButton(QPainter* painter,
                                                   const QRect& rect,
                                                   const QModelIndex& index,
                                                   const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMStudyDelegate);

  // Check button state from the view
  bool isPressed = false;
  bool isHovered = false;
  bool hasActiveContextMenu = false;
  const ctkDICOMStudyListView* listView = nullptr;
  // Try the widget from option first (most reliable)
  if (option.widget)
  {
    listView = qobject_cast<const ctkDICOMStudyListView*>(option.widget);
  }
  // If that fails, try parent
  if (!listView)
  {
    listView = qobject_cast<const ctkDICOMStudyListView*>(this->parent());
  }
  if (listView)
  {
    isPressed = listView->isContextMenuButtonPressed(index);
    isHovered = listView->isContextMenuButtonHovered(index);
    hasActiveContextMenu = listView->hasActiveContextMenu(index);
  }

  bool studyHoveredState = option.state & QStyle::State_MouseOver;
  if (!studyHoveredState &&
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

  // Draw rounded background using the exact icon rect
  QPainterPath bgPath;
  int bgRadius = d->CornerRadius * 0.5;
  bgPath.addRoundedRect(rect, bgRadius, bgRadius);
  painter->fillPath(bgPath, bgColor);

  QString iconResource = ":/Icons/more_vert.svg";
  QIcon selectIcon(iconResource);
  selectIcon.paint(painter, rect);

  painter->restore();
}


//------------------------------------------------------------------------------
void ctkDICOMStudyDelegate::paintOperationStatusButton(QPainter* painter,
                                                       const QRect& rect,
                                                       const QModelIndex& index,
                                                       const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMStudyDelegate);

  // Get hover/press states for operationStatus all icon from the list view
  bool operationStatusIconHovered = false;
  bool operationStatusIconPressed = false;
  // Get the view widget to check states
  const ctkDICOMStudyListView* listView = nullptr;
  // Try the widget from option first (most reliable)
  if (option.widget)
  {
    listView = qobject_cast<const ctkDICOMStudyListView*>(option.widget);
  }
  // If that fails, try parent
  if (!listView)
  {
    listView = qobject_cast<const ctkDICOMStudyListView*>(this->parent());
  }
  if (listView)
  {
    operationStatusIconHovered = listView->isOperationStatusIconHovered(index);
    operationStatusIconPressed = listView->isOperationStatusIconPressed(index);
  }

  bool studyHoveredState = option.state & QStyle::State_MouseOver;
  if (!studyHoveredState &&
      !operationStatusIconHovered &&
      !operationStatusIconPressed)
  {
    return;
  }

  painter->save();

  QColor bgColor;
  if (operationStatusIconPressed)
  {
    bgColor = Qt::gray;
  }
  else if (operationStatusIconHovered)
  {
    bgColor = Qt::lightGray;
  }
  else
  {
    bgColor = Qt::white;
  }
  bgColor.setAlpha(200);

  int operationStatus = index.data(ctkDICOMStudyModel::OperationStatusRole).toInt();
  QIcon iconResource;
  if (operationStatus == ctkDICOMStudyModel::NoOperation)
  {
    painter->restore();
    return;
  }
  else if (operationStatus == ctkDICOMStudyModel::InProgress)
  {
    iconResource = QIcon(":/Icons/pending.svg");
  }
  else if (operationStatus == ctkDICOMStudyModel::Failed)
  {
    iconResource = QIcon(":/Icons/error_red.svg");
  }
  else if (operationStatus == ctkDICOMStudyModel::Completed)
  {
    iconResource = QIcon(":/Icons/accept.svg");
  }

  // Draw rounded background using the exact icon rect
  QPainterPath bgPath;
  int bgRadius = d->CornerRadius * 0.5;
  bgPath.addRoundedRect(rect, bgRadius, bgRadius);
  painter->fillPath(bgPath, bgColor);

  QIcon selectIcon(iconResource);
  selectIcon.paint(painter, rect);

  painter->restore();
}
