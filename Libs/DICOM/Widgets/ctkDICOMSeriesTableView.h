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

#ifndef __ctkDICOMSeriesTableView_h
#define __ctkDICOMSeriesTableView_h

// Qt includes
#include <QTableView>
#include <QtGlobal>
#include <QSize>

// CTK includes
#include "ctkDICOMWidgetsExport.h"

class ctkDICOMSeriesTableViewPrivate;
class ctkDICOMSeriesModel;
class ctkDICOMSeriesDelegate;
class ctkDICOMSeriesFilterProxyModel;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
class QEnterEvent;
#endif

/// \ingroup DICOM_Widgets
/// \brief Custom table view for displaying DICOM series as thumbnails
///
/// This view extends QTableView to provide a grid-like layout for DICOM series
/// with support for:
/// - Multi-row grid layout with configurable columns
/// - Integration with ctkDICOMSeriesModel and ctkDICOMSeriesDelegate
/// - Thumbnail-based series display with metadata
/// - Multi-selection support
/// - Context menu integration
/// - Keyboard navigation
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMSeriesTableView : public QTableView
{
  Q_OBJECT
  Q_PROPERTY (QString studyInstanceUID READ studyInstanceUID WRITE setStudyInstanceUID NOTIFY studyInstanceUIDChanged)

public:
  typedef QTableView Superclass;
  explicit ctkDICOMSeriesTableView(QWidget* parent = nullptr);
  virtual ~ctkDICOMSeriesTableView();

  /// Set/Get the Study Instance UID for which series are displayed
  void setStudyInstanceUID(const QString& studyInstanceUID);
  QString studyInstanceUID() const;

  /// Reimplemented from QAbstractItemView
  Q_INVOKABLE void setModel(QAbstractItemModel* model) override;
  Q_INVOKABLE void setItemDelegate(QAbstractItemDelegate* delegate);

  /// \name Selection
  ///@{
  /// Get selected series instance UIDs
  Q_INVOKABLE QStringList selectedSeriesInstanceUIDs() const;

  /// Get currently selected series instance UID (single selection)
  Q_INVOKABLE QString currentSeriesInstanceUID() const;

  /// Select series by instance UID
  Q_INVOKABLE void selectSeriesInstanceUID(const QString& seriesInstanceUID,
                                           QItemSelectionModel::SelectionFlag flag = QItemSelectionModel::Select);
  Q_INVOKABLE void selectSeriesInstanceUIDs(const QStringList& seriesInstanceUIDs,
                                            QItemSelectionModel::SelectionFlag flag = QItemSelectionModel::Select);

  /// Clear all selections
  Q_INVOKABLE void clearSelection();

  /// Get number of selected items
  Q_INVOKABLE int selectedCount() const;
  ///@}

  /// \name Data access
  ///@{
  /// Get series instance UID for a given index
  Q_INVOKABLE QString seriesInstanceUID(const QModelIndex& index) const;

  /// Get index for a series instance UID
  Q_INVOKABLE QModelIndex indexForSeriesInstanceUID(const QString& seriesInstanceUID) const;
  ///@}

  /// \name Convenience methods
  ///@{
  /// Get the series model (cast from model())
  Q_INVOKABLE ctkDICOMSeriesModel* seriesModel() const;
  Q_INVOKABLE ctkDICOMSeriesFilterProxyModel* seriesFilterProxyModel() const;

  /// Scroll to series by instance UID
  Q_INVOKABLE void scrollToSeriesInstanceUID(const QString& seriesInstanceUID);

  /// Refresh the view layout
  Q_INVOKABLE void refreshLayout(bool forceGridColumnsUpdate = false);

  /// Check if context menu button is hovered for a given index
  Q_INVOKABLE bool isContextMenuButtonHovered(const QModelIndex& index) const;

  /// Check if context menu button is pressed for a given index
  Q_INVOKABLE bool isContextMenuButtonPressed(const QModelIndex& index) const;

  /// Check if a series has an active context menu
  Q_INVOKABLE bool hasActiveContextMenu(const QModelIndex& index) const;

  /// Check if operation status button is hovered for a given index
  Q_INVOKABLE bool isOperationStatusButtonHovered(const QModelIndex& index) const;

  /// Check if operation status button is pressed for a given index
  Q_INVOKABLE bool isOperationStatusButtonPressed(const QModelIndex& index) const;
  ///@}

protected:
  /// Reimplemented from QTableView
  void resizeEvent(QResizeEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseDoubleClickEvent(QMouseEvent* event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEnterEvent* event) override;
#else
  void enterEvent(QEvent* event) override;
#endif
  void leaveEvent(QEvent* event) override;
  void contextMenuEvent(QContextMenuEvent* event) override;

  /// Reimplemented from QAbstractItemView
  void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;
  QSize viewportSizeHint() const override;
  int sizeHintForRow(int row) const override;
  int sizeHintForColumn(int col) const override;

  /// Update grid layout when view size changes
  void updateGridLayout(bool forceGridColumnsUpdate = false);

  /// Calculate optimal number of columns based on view width
  int calculateOptimalColumns() const;

  /// Get the delegate as ctkDICOMSeriesDelegate
  ctkDICOMSeriesDelegate* seriesDelegate() const;

protected slots:
  /// Called when selection changes
  void onSeriesSelectionChanged();

  /// Called when model data changes
  void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

  /// Called when rows are inserted
  void onRowsInserted(const QModelIndex& parent, int first, int last);

  /// Called when rows are removed
  void onRowsRemoved(const QModelIndex& parent, int first, int last);

  /// Called when model is reset
  void onModelReset();

  /// Called when delegate requests context menu
  void onDelegateContextMenuRequested(const QPoint& globalPos, const QModelIndex& index);

signals:
  /// Emitted when the mouse enters the study list view
  void seriesTableViewEntered();

  /// Emitted when Study Instance UID changes
  void studyInstanceUIDChanged(const QString& studyInstanceUID);

  /// Emitted when selection changes
  void seriesSelectionChanged(const QString& studyInstanceUID, const QStringList& selectedSeriesInstanceUIDs);

  /// Emitted when a series is activated (double-clicked)
  void seriesActivated(const QString& seriesInstanceUID);

  /// Emitted when context menu is requested
  void contextMenuRequested(const QPoint& globalPos, const QStringList& selectedSeriesInstanceUIDs);

  /// Emitted when view layout changes
  void layoutChanged();

private:
  Q_DECLARE_PRIVATE(ctkDICOMSeriesTableView);
  Q_DISABLE_COPY(ctkDICOMSeriesTableView);

protected:
  QScopedPointer<ctkDICOMSeriesTableViewPrivate> d_ptr;
};

#endif
