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
#include <QEvent>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>

// CTK includes
#include "ctkDICOMSeriesDelegate.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMSeriesTableView.h"

// STD includes
#include <cmath>

//------------------------------------------------------------------------------
class ctkDICOMSeriesDelegatePrivate
{
public:
  ctkDICOMSeriesDelegatePrivate();

  int Spacing;
  int CornerRadius;
  int ContextIconSize;
  int ProgressBarHeight;
};

//------------------------------------------------------------------------------
ctkDICOMSeriesDelegatePrivate::ctkDICOMSeriesDelegatePrivate()
{
  this->Spacing = 4;
  this->CornerRadius = 8;
  this->ContextIconSize = 24;
  this->ProgressBarHeight = 6;
}

//------------------------------------------------------------------------------
ctkDICOMSeriesDelegate::ctkDICOMSeriesDelegate(QObject* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMSeriesDelegatePrivate)
{
}

//------------------------------------------------------------------------------
ctkDICOMSeriesDelegate::~ctkDICOMSeriesDelegate()
{
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return;
  }

  // Check if this cell has actual series data
  QString seriesInstanceUID = index.data(ctkDICOMSeriesModel::SeriesInstanceUIDRole).toString();
  if (seriesInstanceUID.isEmpty())
  {
    return;
  }

  painter->setRenderHint(QPainter::Antialiasing, true);

  // Get item rect
  QRect itemRect = option.rect;

  // Calculate content areas using the new layout system
  QRect selectionArea = this->selectionRect(itemRect);
  QRect thumbRect = this->thumbnailRect(itemRect, index);
  QRect progressBarRect = this->progressBarRect(itemRect, index);
  QRect textRect = this->textRect(itemRect, index);
  QRect statusButtonRect = this->statusButtonRect(itemRect, index);
  QRect contextMenuButtonRect = this->contextMenuButtonRect(itemRect, index);

  // Draw selection highlight
  this->paintSelection(painter, selectionArea, option);

  // Draw thumbnail
  this->paintThumbnail(painter, thumbRect, index);

  // Draw thumbnail overlay with series info
  this->paintThumbnailOverlay(painter, thumbRect, index);

  // Draw cloud/downloading status icon
  this->paintStatusButton(painter, statusButtonRect, index, option);

  // Draw progress bar if downloading
  this->paintProgressBar(painter, progressBarRect, index);

  // Draw text information
  this->paintText(painter, textRect, index, option);

  // Draw context menu button (3 dots) when hovering or when context menu is active
  this->paintContextMenuButton(painter, contextMenuButtonRect, index, option);
}

//------------------------------------------------------------------------------
QSize ctkDICOMSeriesDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  Q_UNUSED(option);
  Q_D(const ctkDICOMSeriesDelegate);

  // Query the model for thumbnail size using the role
  QSize thumbnailSize(128, 128); // Default fallback
  if (index.model())
  {
    QVariant sizeData = index.model()->data(index, ctkDICOMSeriesModel::ThumbnailSizeRole);
    if (sizeData.isValid())
    {
      thumbnailSize = sizeData.toSize();
    }
  }

  // Calculate text height for series description
  QFont font;
  font.setPointSize(font.pointSize() - 2); // Match the font size used in paintText
  QFontMetrics metrics(font);
  int textHeight = metrics.height();

  // Define spacing constants - these will be used consistently across all rect methods
  int outerMargin = d->Spacing * 2; // Increased margin around the entire item for more space
  int thumbnailToTextSpacing = d->Spacing * 0.5; // Space between thumbnail and text
  int textPadding = int(d->Spacing * 0.75); // Vertical padding around text

  // progress bar
  int progressBarHeight = d->ProgressBarHeight + d->Spacing; // Progress bar height + spacing

  // Calculate total required size:
  // - Outer margins (top/bottom)
  // - Selection insets (top/bottom/left/right)
  // - Thumbnail
  // - Progress bar (if downloading)
  // - Spacing between thumbnail and text
  // - Text with padding (now includes 3 lines: description + series info + dimensions)
  int totalHeight = (outerMargin * 2) + thumbnailSize.height() + progressBarHeight +
                    thumbnailToTextSpacing + (textHeight * 3) + textPadding;
  int totalWidth = (outerMargin * 2) + thumbnailSize.width();
  if (thumbnailSize.width() == 0)
  {
    totalWidth += 128;
  }

  return QSize(totalWidth, totalHeight);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesDelegate::paintThumbnail(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesDelegate);

  // Quick validation - if rect is invalid, skip expensive operations
  if (rect.width() <= 0 || rect.height() <= 0)
  {
    return;
  }

  // Get thumbnail data
  QString thumbnailPath = index.data(ctkDICOMSeriesModel::ThumbnailPathRole).toString();
  QPixmap thumbnail;
  if (!thumbnailPath.isEmpty() && QFile::exists(thumbnailPath))
  {
    thumbnail = QPixmap(thumbnailPath);
  }
  else
  {
    QSize thumbnailSize(128, 128);
    if (index.model())
    {
      QVariant sizeData = index.model()->data(index, ctkDICOMSeriesModel::ThumbnailSizeRole);
      if (sizeData.isValid())
      {
        thumbnailSize = sizeData.toSize();
      }
    }
    QString modality = index.data(ctkDICOMSeriesModel::ModalityRole).toString();
    if (!modality.isEmpty())
    {
      // Create placeholder pixmap with modality text)
      thumbnail = QPixmap(thumbnailSize);
      thumbnail.fill(Qt::white);

      QPainter painter(&thumbnail);
      painter.setRenderHint(QPainter::Antialiasing);

      // Draw modality text with shadow
      QFont font = painter.font();
      font.setBold(true);
      font.setPixelSize(thumbnailSize.height() / 5);
      painter.setFont(font);

      QColor textColor = QColor(0, 120, 215); // Blue color

      // Draw text
      painter.setPen(textColor);
      painter.drawText(thumbnail.rect(), Qt::AlignCenter, modality);
    }
  }

  // Draw thumbnail background
  painter->save();

  // Always add subtle drop shadow for depth
  QRect shadowRect = rect.adjusted(2, 2, 2, 2);
  QPainterPath shadowPath;
  shadowPath.addRoundedRect(shadowRect, d->CornerRadius, d->CornerRadius);
  painter->fillPath(shadowPath, QColor(0, 0, 0, 30));

  // Create rounded clipping path for the entire rect
  QPainterPath path;
  path.addRoundedRect(rect, d->CornerRadius, d->CornerRadius);
  painter->setClipPath(path);

  // Draw thumbnail or placeholder
  if (!thumbnail.isNull() && thumbnail.width() > 0 && thumbnail.height() > 0)
  {
    // Get the fill color from the top-left pixel of the original thumbnail
    QColor fillColor = Qt::black; // Default fallback
    QImage image = thumbnail.toImage();
    if (!image.isNull())
    {
      fillColor = QColor(image.pixel(0, 0));
    }

    // Fill the entire rect with the background color first
    painter->fillRect(rect, fillColor);

    // Scale thumbnail to fit while maintaining aspect ratio
    QPixmap scaledThumbnail = thumbnail.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Center the thumbnail
    QRect thumbRect = rect;
    thumbRect.setSize(scaledThumbnail.size());
    thumbRect.moveCenter(rect.center());

    // Draw the scaled thumbnail centered over the filled background
    painter->drawPixmap(thumbRect, scaledThumbnail);
  }

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesDelegate::paintText(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMSeriesDelegate);

  painter->save();

  // Get series information
  QString seriesDescription = index.data(ctkDICOMSeriesModel::SeriesDescriptionRole).toString();
  if (seriesDescription.isEmpty())
  {
    seriesDescription = tr("Unknown Series");
  }

  QString seriesNumber = index.data(ctkDICOMSeriesModel::SeriesNumberRole).toString();
  int instanceCount = index.data(ctkDICOMSeriesModel::InstanceCountRole).toInt();
  int rows = index.data(ctkDICOMSeriesModel::RowsRole).toInt();
  int cols = index.data(ctkDICOMSeriesModel::ColumnsRole).toInt();
  QString modality = index.data(ctkDICOMSeriesModel::ModalityRole).toString();

  // Get thumbnail size to check if we need to show modality in text
  QSize thumbnailSize(128, 128);
  if (index.model())
  {
    QVariant sizeData = index.model()->data(index, ctkDICOMSeriesModel::ThumbnailSizeRole);
    if (sizeData.isValid())
    {
      thumbnailSize = sizeData.toSize();
    }
  }
  bool showModalityInText = (thumbnailSize.width() == 0);

  QColor textColor = option.palette.color(QPalette::Text);

  QFont descFont = option.font;
  descFont.setBold(false);
  descFont.setPointSize(descFont.pointSize() - 1);
  QFontMetrics descMetrics(descFont);

  QFont modalityFont = descFont;
  modalityFont.setBold(true);
  QFontMetrics modalityMetrics(modalityFont);

  QRect descRect = rect;
  descRect.setHeight(descMetrics.height());

  // Calculate total width needed for modality + description
  QString modalityText = "";
  int modalityWidth = 0;
  if (showModalityInText && !modality.isEmpty())
  {
    modalityText = modality;
    modalityWidth = modalityMetrics.horizontalAdvance(modalityText);
  }

  // Calculate available width and elide description if needed
  // Account for proper margins on both sides
  int horizontalMargin = d->Spacing * 2; // margins on both sides
  int availableWidth = descRect.width() - horizontalMargin;
  int descWidth = availableWidth - modalityWidth;

  // If we have modality, account for the separator " - "
  if (showModalityInText && !modality.isEmpty())
  {
    QString separator = " - ";
    int separatorWidth = descMetrics.horizontalAdvance(separator);
    descWidth -= separatorWidth;
  }

  QString elidedDesc = descMetrics.elidedText(seriesDescription, Qt::ElideRight, qMax(0, descWidth));
  int actualDescWidth = descMetrics.horizontalAdvance(elidedDesc);

  // Calculate starting X position for centered text
  int totalWidth = modalityWidth + actualDescWidth;
  if (showModalityInText && !modality.isEmpty() && !elidedDesc.isEmpty())
  {
    totalWidth += descMetrics.horizontalAdvance(" - ");
  }
  int startX = descRect.left() + (descRect.width() - totalWidth) / 2;

  // Draw modality in bold (if applicable)
  if (showModalityInText && !modality.isEmpty())
  {
    elidedDesc = " - " + elidedDesc;
    actualDescWidth = descMetrics.horizontalAdvance(elidedDesc);
    painter->setFont(modalityFont);
    painter->setPen(textColor);
    painter->drawText(startX, descRect.top(), modalityWidth, descRect.height(),
                     Qt::AlignLeft | Qt::AlignVCenter, modalityText);
    startX += modalityWidth;
  }

  // Draw series description
  painter->setFont(descFont);
  painter->setPen(textColor);
  painter->drawText(startX, descRect.top(), actualDescWidth, descRect.height(),
                   Qt::AlignLeft | Qt::AlignVCenter, elidedDesc);

  // Second line: series number and instance count with icons (centered)
  QFont infoFont = option.font;
  infoFont.setBold(false);
  infoFont.setPointSize(infoFont.pointSize() - 2);
  QFontMetrics infoMetrics(infoFont);

  painter->setFont(infoFont);
  painter->setPen(QColor(70, 130, 180)); // Steel blue

  QRect infoRect = rect;
  infoRect.setTop(descRect.bottom() + 1);
  infoRect.setHeight(infoMetrics.height());

  // Calculate positions for text and icons
  int iconSize = infoMetrics.height() - 2;
  int iconY = infoRect.top() + (infoRect.height() - iconSize) / 2;

  // Calculate total width for second line content
  int line2Width = 0;
  QString seriesText = "";
  if (!seriesNumber.isEmpty())
  {
    seriesText = QString("S:%1").arg(seriesNumber);
    line2Width += infoMetrics.horizontalAdvance(seriesText);
    if (instanceCount > 0)
    {
      line2Width += d->Spacing; // spacing between series number and icon
    }
  }

  if (instanceCount > 0)
  {
    line2Width += iconSize + d->Spacing * 0.5; // icon + spacing
    QString countText = QString("%1").arg(instanceCount);
    line2Width += infoMetrics.horizontalAdvance(countText);
  }

  // Center the second line content
  int currentX = infoRect.left() + (infoRect.width() - line2Width) / 2;

  // Series number
  if (!seriesNumber.isEmpty())
  {
    painter->drawText(currentX, infoRect.top(), infoMetrics.horizontalAdvance(seriesText), infoRect.height(),
                     Qt::AlignLeft | Qt::AlignVCenter, seriesText);
    currentX += infoMetrics.horizontalAdvance(seriesText) + d->Spacing;
  }

  // Stack/layers icon + instance count
  if (instanceCount > 0)
  {
    // Draw stack icon from QRC
    QIcon stackIcon(":/Icons/stack.svg");
    QPixmap stackPixmap = stackIcon.pixmap(iconSize, iconSize);
    painter->drawPixmap(currentX, iconY, stackPixmap);
    currentX += iconSize + d->Spacing * 0.5;

    // Draw instance count text
    QString countText = QString("%1").arg(instanceCount);
    painter->drawText(currentX, infoRect.top(), infoMetrics.horizontalAdvance(countText), infoRect.height(),
                     Qt::AlignLeft | Qt::AlignVCenter, countText);
  }

  // Third line: grid icon + dimensions (centered)
  if (cols > 0 && rows > 0)
  {
    QRect dimRect = rect;
    dimRect.setTop(infoRect.bottom() + 1);
    dimRect.setHeight(infoMetrics.height());

    int dimIconY = dimRect.top() + (dimRect.height() - iconSize) / 2;

    // Calculate total width for third line content
    QString dimText = QString("%1×%2").arg(cols).arg(rows);
    int line3Width = iconSize + d->Spacing * 0.5 + infoMetrics.horizontalAdvance(dimText);

    // Center the third line content
    currentX = dimRect.left() + (dimRect.width() - line3Width) / 2;

    // Draw grid icon from QRC
    QIcon gridIcon(":/Icons/grid.svg");
    QPixmap gridPixmap = gridIcon.pixmap(iconSize, iconSize);
    painter->drawPixmap(currentX, dimIconY, gridPixmap);
    currentX += iconSize + d->Spacing * 0.5;

    // Draw dimensions text
    painter->drawText(currentX, dimRect.top(), infoMetrics.horizontalAdvance(dimText), dimRect.height(),
                     Qt::AlignLeft | Qt::AlignVCenter, dimText);
  }

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesDelegate::paintThumbnailOverlay(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesDelegate);
  // Only paint modality overlay on thumbnail if thumbnail has been generated
  bool thumbnailGenerated = index.data(ctkDICOMSeriesModel::ThumbnailGeneratedRole).toBool();
  if (!thumbnailGenerated)
  {
    return;
  }

  painter->save();

  // Get modality
  QString modality = index.data(ctkDICOMSeriesModel::ModalityRole).toString();

  if (modality.isEmpty())
  {
    painter->restore();
    return;
  }

  // Set up font for overlay
  QFont overlayFont = painter->font();
  overlayFont.setPointSize(overlayFont.pointSize() + 1);
  overlayFont.setBold(true);

  QFontMetrics metrics(overlayFont);
  int modalityTextWidth = metrics.horizontalAdvance(modality);
  int modalityTextHeight = metrics.height();

  painter->setFont(overlayFont);
  QColor textColor = QColor(0, 120, 215); // Blue color

  // Tighter pill-like rounded rect, centered vertically to text
  QRect modalityRect(rect.left() + d->Spacing, rect.top() + d->Spacing,
                     modalityTextWidth + d->Spacing * 2, modalityTextHeight);

  QPainterPath bgPath;
  int bgRadius = d->CornerRadius * 0.5;
  bgPath.addRoundedRect(modalityRect, bgRadius, bgRadius);
  QColor bgColor = Qt::gray;
  bgColor.setAlpha(220);
  painter->fillPath(bgPath, bgColor);

  // Draw main text, centered vertically
  painter->setPen(textColor);
  painter->drawText(modalityRect, Qt::AlignCenter, modality);

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesDelegate::paintSelection(QPainter* painter, const QRect& rect, const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMSeriesDelegate);

  // Check for selection, hover, and focus (current item) state
  bool isSelected = option.state & QStyle::State_Selected;
  bool isHovered = option.state & QStyle::State_MouseOver;
  bool isCurrent = option.state & QStyle::State_HasFocus;

  painter->save();

  int borderWidth = 2;
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
    backgroundColor = QColor(70, 130, 180, 0);
    borderColor = QColor(220, 220, 220);
  }

  if (isCurrent)
  {
    borderColor = QColor(70, 130, 180);
  }

  painter->setBrush(QBrush(backgroundColor));
  painter->setPen(QPen(borderColor, borderWidth));
  painter->drawRoundedRect(rect, d->CornerRadius, d->CornerRadius);

  painter->restore();
}

//------------------------------------------------------------------------------
QRect ctkDICOMSeriesDelegate::selectionRect(const QRect& itemRect) const
{
  Q_D(const ctkDICOMSeriesDelegate);
  return itemRect.marginsRemoved(QMargins(d->Spacing, d->Spacing, d->Spacing, d->Spacing));
}

//------------------------------------------------------------------------------
QRect ctkDICOMSeriesDelegate::thumbnailRect(const QRect& itemRect, const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesDelegate);
  // Query the model for thumbnail size using the role
  QSize thumbnailSize(128, 128);
  if (index.model())
  {
    QVariant sizeData = index.model()->data(index, ctkDICOMSeriesModel::ThumbnailSizeRole);
    if (sizeData.isValid())
    {
      thumbnailSize = sizeData.toSize();
    }
  }

  // Get the selection area (area where thumbnail and text live)
  QRect selectionArea = this->selectionRect(itemRect);

  // Thumbnail is positioned at the top of the selection area
  QRect thumbRect;
  thumbRect.setSize(thumbnailSize);
  // Center within the selection area
  thumbRect.moveLeft(selectionArea.left() + (selectionArea.width() - thumbRect.width()) / 2);
  thumbRect.moveTop(selectionArea.top() + d->Spacing);

  return thumbRect;
}

//------------------------------------------------------------------------------
QRect ctkDICOMSeriesDelegate::progressBarRect(const QRect& itemRect, const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesDelegate);
  QRect thumbRect = this->thumbnailRect(itemRect, index);
  int progressBarHeight = d->ProgressBarHeight;

  QRect progressBarRect;
  progressBarRect.setLeft(thumbRect.left());
  progressBarRect.setRight(thumbRect.right());
  progressBarRect.setTop(thumbRect.bottom() + d->Spacing);
  progressBarRect.setHeight(progressBarHeight);

  return progressBarRect;
}

//------------------------------------------------------------------------------
QRect ctkDICOMSeriesDelegate::textRect(const QRect& itemRect, const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesDelegate);
  QRect progressBarRect = this->progressBarRect(itemRect, index);
  QRect selectionArea = this->selectionRect(itemRect);

  // Calculate text height
  QFont font;
  font.setPointSize(font.pointSize() - 2);
  QFontMetrics metrics(font);
  int textHeight = metrics.height();

  // Text area is positioned below progressBarRect with proper spacing

  QRect textRect;
  textRect.setLeft(selectionArea.left());
  textRect.setRight(selectionArea.right());
  textRect.setTop(progressBarRect.bottom() + d->Spacing * 0.25);
  textRect.setHeight((textHeight * 2) + d->Spacing * 0.75);

  return textRect;
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesDelegate::setSpacing(int spacing)
{
  Q_D(ctkDICOMSeriesDelegate);
  d->Spacing = spacing;
  emit this->spacingChanged(spacing);
}

//------------------------------------------------------------------------------
int ctkDICOMSeriesDelegate::spacing() const
{
  Q_D(const ctkDICOMSeriesDelegate);
  return d->Spacing;
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesDelegate::setCornerRadius(int radius)
{
  Q_D(ctkDICOMSeriesDelegate);
  d->CornerRadius = radius;
  emit this->cornerRadiusChanged(radius);
}

//------------------------------------------------------------------------------
int ctkDICOMSeriesDelegate::cornerRadius() const
{
  Q_D(const ctkDICOMSeriesDelegate);
  return d->CornerRadius;
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesDelegate::paintContextMenuButton(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMSeriesDelegate);

  QSize thumbnailSize(128, 128);
  if (index.model())
  {
    QVariant sizeData = index.model()->data(index, ctkDICOMSeriesModel::ThumbnailSizeRole);
    if (sizeData.isValid())
    {
      thumbnailSize = sizeData.toSize();
    }
  }

  if (thumbnailSize.width() == 0)
  {
    // No status button when no thumbnail is generated
    return;
  }

  // Get the table view to check button state
  const ctkDICOMSeriesTableView* tableView = qobject_cast<const ctkDICOMSeriesTableView*>(option.widget);

  // Determine button state
  bool isPressed = false;
  bool isHovered = false;
  bool hasActiveContextMenu = false;
  if (tableView)
  {
    isPressed = tableView->isContextMenuButtonPressed(index);
    isHovered = tableView->isContextMenuButtonHovered(index);
    hasActiveContextMenu = tableView->hasActiveContextMenu(index);
  }

  bool seriesHoveredState = option.state & QStyle::State_MouseOver;
  if (!seriesHoveredState &&
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
    bgColor = Qt::darkGray;
    bgColor.setAlpha(220);
  }
  else if (isHovered)
  {
    bgColor = Qt::gray;
    bgColor.setAlpha(220);
  }
  else
  {
    bgColor = Qt::lightGray;
    bgColor.setAlpha(220);
  }

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
QRect ctkDICOMSeriesDelegate::contextMenuButtonRect(const QRect& itemRect, const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesDelegate);
  QRect thumbRect = this->thumbnailRect(itemRect, index);
  return QRect(thumbRect.right() - d->ContextIconSize - d->Spacing,
               thumbRect.bottom() - d->ContextIconSize - d->Spacing,
               d->ContextIconSize,
               d->ContextIconSize);
}

//------------------------------------------------------------------------------
bool ctkDICOMSeriesDelegate::isContextMenuButtonAt(const QPoint& pos, const QRect& itemRect, const QModelIndex& index) const
{
  QRect buttonRect = this->contextMenuButtonRect(itemRect, index);
  return buttonRect.contains(pos);
}

//------------------------------------------------------------------------------
QRect ctkDICOMSeriesDelegate::statusButtonRect(const QRect &itemRect, const QModelIndex &index) const
{
  Q_D(const ctkDICOMSeriesDelegate);
  QRect thumbRect = this->thumbnailRect(itemRect, index);
  return QRect(thumbRect.right() - d->ContextIconSize - d->Spacing,
               thumbRect.top() + d->Spacing,
               d->ContextIconSize,
               d->ContextIconSize);
}

//------------------------------------------------------------------------------
bool ctkDICOMSeriesDelegate::isStatusButtonAt(const QPoint &pos, const QRect &itemRect, const QModelIndex &index) const
{
  QRect buttonRect = this->statusButtonRect(itemRect, index);
  return buttonRect.contains(pos);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesDelegate::paintStatusButton(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const
{
  Q_D(const ctkDICOMSeriesDelegate);

  QSize thumbnailSize(128, 128);
  if (index.model())
  {
    QVariant sizeData = index.model()->data(index, ctkDICOMSeriesModel::ThumbnailSizeRole);
    if (sizeData.isValid())
    {
      thumbnailSize = sizeData.toSize();
    }
  }

  if (thumbnailSize.width() == 0)
  {
    // No status button when no thumbnail is generated
    return;
  }

  // Get the table view to check button state
  const ctkDICOMSeriesTableView* tableView = qobject_cast<const ctkDICOMSeriesTableView*>(option.widget);

  // Determine button state
  bool isPressed = false;
  bool isHovered = false;
  if (tableView)
  {
    isPressed = tableView->isOperationStatusButtonPressed(index);
    isHovered = tableView->isOperationStatusButtonHovered(index);
  }

  QColor bgColor;
  if (isPressed)
  {
    bgColor = Qt::darkGray;
    bgColor.setAlpha(220);
  }
  else if (isHovered)
  {
    bgColor = Qt::gray;
    bgColor.setAlpha(220);
  }
  else
  {
    bgColor = Qt::lightGray;
    bgColor.setAlpha(220);
  }

  // Determine which icon to use
  QIcon iconResource;
  int operationProgress = index.data(ctkDICOMSeriesModel::OperationProgressRole).toInt();
  int operationStatus = index.data(ctkDICOMSeriesModel::OperationStatusRole).toInt();
  bool isCloud = index.data(ctkDICOMSeriesModel::IsCloudRole).toBool();
  bool isLoaded = index.data(ctkDICOMSeriesModel::IsLoadedRole).toBool();
  if (operationStatus == ctkDICOMSeriesModel::NoOperation)
  {
    if (isCloud)
    {
      iconResource = QIcon(":/Icons/cloud.svg");
    }
    else if (isLoaded)
    {
      iconResource = QIcon(":/Icons/loaded.svg");
    }
    else
    {
      // No icon to draw
      return;
    }
  }
  else if (operationStatus == ctkDICOMSeriesModel::InProgress)
  {
    if (operationProgress > 0)
    {
      iconResource = QIcon(":/Icons/downloading.svg");
    }
    else
    {
      iconResource = QIcon(":/Icons/cloud.svg");
    }
  }
  else if (operationStatus == ctkDICOMSeriesModel::Failed)
  {
    iconResource = QIcon(":/Icons/error_red.svg");
  }
  else if (operationStatus == ctkDICOMSeriesModel::Completed)
  {
    if (isLoaded)
    {
      iconResource = QIcon(":/Icons/loaded.svg");
    }
    else
    {
      iconResource = QIcon(":/Icons/accept2.svg");
      bool seriesHoveredState = option.state & QStyle::State_MouseOver;
      if (!seriesHoveredState &&
          !isHovered &&
          !isPressed)
      {
        return;
      }
    }
  }

  painter->save();

  // Draw rounded background
  QPainterPath bgPath;
  int bgRadius = d->CornerRadius * 0.5;
  bgPath.addRoundedRect(rect, bgRadius, bgRadius);
  bgColor.setAlpha(220);
  painter->fillPath(bgPath, bgColor);

  // Draw icon
  iconResource.paint(painter, rect);

  painter->restore();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesDelegate::paintProgressBar(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesDelegate);

  // Check if series is in cloud and has active operation
  bool isCloud = index.data(ctkDICOMSeriesModel::IsCloudRole).toBool();
  int operationProgress = index.data(ctkDICOMSeriesModel::OperationProgressRole).toInt();
  if (!isCloud || operationProgress <= 0)
  {
    return;
  }

  painter->save();

  int numberOfInstance = index.data(ctkDICOMSeriesModel::InstanceCountRole).toInt();
  // Draw progress
  int progressWidth = (rect.width() * operationProgress) / numberOfInstance;
  if (progressWidth > 0)
  {
    QRect progressRect(rect.left(),
                       rect.top(),
                       progressWidth,
                       d->ProgressBarHeight);

    QPainterPath progressPath;
    progressPath.addRoundedRect(progressRect, d->CornerRadius * 0.5, d->CornerRadius * 0.5);
    QColor progressColor = QColor(70, 130, 180); // Steel blue
    painter->fillPath(progressPath, progressColor);
  }

  painter->restore();
}
