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

#ifndef __ctkDICOMSeriesDelegate_h
#define __ctkDICOMSeriesDelegate_h

// Qt includes
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPixmap>
#include <QRect>

// CTK includes
#include "ctkDICOMWidgetsExport.h"

class ctkDICOMSeriesDelegatePrivate;

/// \ingroup DICOM_Widgets
/// \brief Custom delegate for rendering DICOM series in list/grid views
///
/// This delegate provides custom rendering for DICOM series items including:
/// - Thumbnail images
/// - Series description and metadata
/// - Progress indicators for loading operations
/// - Selection states and hover effects
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMSeriesDelegate : public QStyledItemDelegate
{
  Q_OBJECT
  Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
  Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)

public:
  typedef QStyledItemDelegate Superclass;
  explicit ctkDICOMSeriesDelegate(QObject* parent = nullptr);
  virtual ~ctkDICOMSeriesDelegate();

  /// Reimplemented from QStyledItemDelegate
  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

  /// Set spacing between elements
  /// \param spacing Spacing in pixels
  void setSpacing(int spacing);
  int spacing() const;

  /// Set corner radius for rounded rectangles
  /// \param radius Corner radius in pixels
  void setCornerRadius(int radius);
  int cornerRadius() const;

  /// Check if a point is within the context menu button area
  bool isContextMenuButtonAt(const QPoint& pos, const QRect& itemRect, const QModelIndex& index) const;

  /// Get the context menu button rect
  QRect contextMenuButtonRect(const QRect& itemRect, const QModelIndex& index) const;

  /// Check if a point is within the status button area
  bool isStatusButtonAt(const QPoint& pos, const QRect& itemRect, const QModelIndex& index) const;

  /// Get the status button rect
  QRect statusButtonRect(const QRect& itemRect, const QModelIndex& index) const;

Q_SIGNALS:
  /// Emitted when spacing changes
  void spacingChanged(int spacing);

  /// Emitted when corner radius changes
  void cornerRadiusChanged(int radius);

protected:
  /// Paint the thumbnail area
  void paintThumbnail(QPainter* painter, const QRect& rect, const QModelIndex& index) const;

  /// Paint the text information
  void paintText(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const;

  /// Paint overlay information on thumbnail
  void paintThumbnailOverlay(QPainter* painter, const QRect& rect, const QModelIndex& index) const;

  /// Paint cloud/downloading status button in top-right corner of thumbnail
  void paintStatusButton(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const;

  /// Paint progress bar below thumbnail when downloading
  void paintProgressBar(QPainter* painter, const QRect& rect, const QModelIndex& index) const;

  /// Paint the selection state
  void paintSelection(QPainter* painter, const QRect& rect, const QStyleOptionViewItem& option) const;

  /// Paint the context menu button (3 dots) when hovering or when context menu is active
  void paintContextMenuButton(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const;

  /// Get the selection highlight rect (contentRect minus selection margins)
  QRect selectionRect(const QRect& itemRect) const;

  /// Get the thumbnail rect within the item rect
  QRect thumbnailRect(const QRect& itemRect, const QModelIndex& index) const;

  /// Get the progress bar rect within the item rect
  QRect progressBarRect(const QRect& itemRect, const QModelIndex& index) const;

  /// Get the text rect within the item rect
  QRect textRect(const QRect& itemRect, const QModelIndex& index) const;

private:
  Q_DECLARE_PRIVATE(ctkDICOMSeriesDelegate);
  Q_DISABLE_COPY(ctkDICOMSeriesDelegate);

protected:
  QScopedPointer<ctkDICOMSeriesDelegatePrivate> d_ptr;
};

#endif
