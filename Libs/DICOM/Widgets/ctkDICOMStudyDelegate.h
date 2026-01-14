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

#ifndef __ctkDICOMStudyDelegate_h
#define __ctkDICOMStudyDelegate_h

// Qt includes
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPixmap>
#include <QRect>

// CTK includes
#include "ctkDICOMWidgetsExport.h"

class ctkDICOMStudyDelegatePrivate;
class ctkDICOMSeriesModel;

/// \ingroup DICOM_Widgets
/// \brief Custom delegate for rendering DICOM studies in list views
///
/// This delegate provides custom rendering for DICOM study items including:
/// - Study information (description, date, time, modalities)
/// - Series count and status indicators
/// - Collapsible state indicators (expand/collapse icons)
/// - Progress indicators for loading operations
/// - Selection states and hover effects
/// - Integrated series view for expanded studies
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMStudyDelegate : public QStyledItemDelegate
{
  Q_OBJECT
  Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
  Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)
  Q_PROPERTY(int collapsedHeight READ collapsedHeight WRITE setCollapsedHeight NOTIFY collapsedHeightChanged)
  Q_PROPERTY(int expandedHeight READ expandedHeight WRITE setExpandedHeight NOTIFY expandedHeightChanged)

public:
  typedef QStyledItemDelegate Superclass;
  explicit ctkDICOMStudyDelegate(QObject* parent = nullptr);
  virtual ~ctkDICOMStudyDelegate();

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

  /// Set heights for different states
  /// \param height Height in pixels for collapsed studies
  void setCollapsedHeight(int height);
  int collapsedHeight() const;

  /// \param height Height in pixels for expanded studies
  void setExpandedHeight(int height);
  int expandedHeight() const;

  /// Check if study is collapsed
  Q_INVOKABLE bool isStudyCollapsed(const QModelIndex& index) const;

  /// Get the series area rect within the item rect (when expanded)
  Q_INVOKABLE QRect seriesAreaRect(const QRect& itemRect, const QModelIndex& index) const;

  /// Calculate the required height for the series area based on series count, grid columns, and thumbnail size
  Q_INVOKABLE int calculateSeriesAreaHeight(const QModelIndex& studyIndex) const;

  /// Get the study header rect within the item rect
  Q_INVOKABLE QRect studyCardRect(const QRect& itemRect) const;

  /// Get the study header rect within the item rect
  Q_INVOKABLE QRect studyHeaderRect(const QRect& itemRect) const;

  /// Get the study info rect within the header rect
  Q_INVOKABLE QRect studyInfoRect(const QRect& headerRect) const;

  /// Get the collapse indicator rect within the header rect
  Q_INVOKABLE QRect collapseIndicatorRect(const QRect& headerRect) const;

  /// Get the series info rect within the header rect
  Q_INVOKABLE QRect seriesInfoRect(const QRect& headerRect) const;

  /// Get the select all icon rect within the header rect
  Q_INVOKABLE QRect selectAllIconRect(const QRect& headerRect) const;

  /// Get the context menu button rect within the header rect
  Q_INVOKABLE QRect contextMenuButtonRect(const QRect& headerRect) const;

  /// Get the operation status button rect within the header rect
  Q_INVOKABLE QRect operationStatusButtonRect(const QRect& headerRect) const;

  /// Check if a point is within the context menu button area
  Q_INVOKABLE bool isContextMenuButtonAt(const QPoint& pos, const QRect& itemRect) const;

Q_SIGNALS:
  /// Emitted when spacing changes
  void spacingChanged(int spacing);

  /// Emitted when corner radius changes
  void cornerRadiusChanged(int radius);

  /// Emitted when collapsed height changes
  void collapsedHeightChanged(int height);

  /// Emitted when expanded height changes
  void expandedHeightChanged(int height);

protected:
  /// Paint the study header area
  void paintStudyHeader(QPainter* painter,
                        const QRect& rect,
                        const QModelIndex& index,
                        const QStyleOptionViewItem& option,
                        int numberOfSeriesSelected) const;

  /// Paint the collapse/expand indicator
  void paintCollapseIndicator(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const;

  /// Paint the study information (description, date, modalities)
  void paintStudyInfo(QPainter* painter, const QRect& rect, const QModelIndex& index, const QStyleOptionViewItem& option) const;

  /// Paint the series count and status
  void paintSeriesInfo(QPainter* painter, const QRect& rect, const QModelIndex& index, int numberOfSeriesSelected) const;

  /// Paint the series count and status
  void paintSelectAllIcon(QPainter* painter,
                          const QRect& rect,
                          const QModelIndex& index,
                          const QStyleOptionViewItem& option) const;

  /// Paint the context menu button (3 dots) when hovering
  void paintContextMenuButton(QPainter* painter,
                              const QRect& rect,
                              const QModelIndex& index,
                              const QStyleOptionViewItem& option) const;

  /// Paint the operation status button (progress, error, etc.)
  void paintOperationStatusButton(QPainter* painter,
                                  const QRect& rect,
                                  const QModelIndex& index,
                                  const QStyleOptionViewItem& option) const;

  /// Paint the card background container
  void paintCardBackground(QPainter* painter,
                           const QRect& rect,
                           const QModelIndex& index,
                           const QStyleOptionViewItem& option) const;

  /// Get formatted study date
  QString formatStudyDate(const QString& date) const;

  /// Get formatted study time
  QString formatStudyTime(const QString& time) const;

private:
  Q_DECLARE_PRIVATE(ctkDICOMStudyDelegate);
  Q_DISABLE_COPY(ctkDICOMStudyDelegate);

protected:
  QScopedPointer<ctkDICOMStudyDelegatePrivate> d_ptr;
};

#endif
