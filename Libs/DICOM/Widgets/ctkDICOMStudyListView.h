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

#ifndef __ctkDICOMStudyListView_h
#define __ctkDICOMStudyListView_h

// Qt includes
#include <QListView>
#include <QtGlobal>

// CTK includes
#include "ctkDICOMWidgetsExport.h"
#include "ctkDICOMStudyModel.h"

class ctkDICOMStudyListViewPrivate;
class ctkDICOMStudyDelegate;
class ctkDICOMSeriesTableView;
class ctkDICOMSeriesDelegate;
class ctkDICOMStudyFilterProxyModel;
class ctkDICOMStudyMergedFilterProxyModel;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
class QEnterEvent;
#endif

/// \ingroup DICOM_Widgets
/// \brief Custom list view for displaying DICOM studies
///
/// This view extends QListView to provide a vertical list layout for DICOM studies
/// with support for:
/// - Integration with ctkDICOMStudyModel and ctkDICOMStudyDelegate
/// - Study-based display with collapsible state and metadata
/// - Multi-selection support
/// - Context menu integration
/// - Keyboard navigation
/// - Study collapse/expand functionality
/// - Series model integration for expanded studies
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMStudyListView : public QListView
{
  Q_OBJECT

public:
  typedef QListView Superclass;
  explicit ctkDICOMStudyListView(QWidget* parent = nullptr);
  virtual ~ctkDICOMStudyListView();

  /// Reimplemented from QAbstractItemView
  Q_INVOKABLE void setModel(QAbstractItemModel* model) override;

  /// \name Selection
  ///@{
  /// Get selected study instance UIDs
  Q_INVOKABLE QStringList selectedStudyInstanceUIDs() const;

  /// Get selected studies for a patient
  Q_INVOKABLE QStringList selectedStudiesInstanceUIDsByPatient(QString patientUID) const;

  /// Get current study instance UID
  Q_INVOKABLE QString currentStudyInstanceUID() const;

  /// Select study by instance UID
  Q_INVOKABLE void selectStudyInstanceUID(const QString& studyInstanceUID,
                                          QItemSelectionModel::SelectionFlag flag = QItemSelectionModel::Select);

  /// Return the numer of selected series for a patient
  Q_INVOKABLE int numberOfStudiesSelectedByPatient(QString patientUID) const;

  /// Get selected series for a patient
  Q_INVOKABLE QStringList selectedSeriesInstanceUIDsByPatient(QString patientUID) const;

  /// Get selected series for a study
  Q_INVOKABLE QStringList selectedSeriesInstanceUIDsByStudy(const QModelIndex& studyIndex) const;

  /// Get selected series
  Q_INVOKABLE QStringList selectedSeriesInstanceUIDs() const;

  /// Select all series for all patients/studies
  Q_INVOKABLE void selectAllSeries(QItemSelectionModel::SelectionFlags selectionMode = QItemSelectionModel::Select);

  /// Select select all series for a patient
  Q_INVOKABLE void selectAllSeriesByPatient(QString patientUID,
                                             QItemSelectionModel::SelectionFlags selectionMode = QItemSelectionModel::Select);

  /// Toggle select all series for a study
  Q_INVOKABLE void selectAllSeriesByStudy(const QModelIndex& studyIndex,
                                           QItemSelectionModel::SelectionFlags selectionMode = QItemSelectionModel::Select);

  /// Select series by their instance UIDs
  Q_INVOKABLE void selectSeriesInstanceUIDs(const QStringList& seriesInstanceUIDs);

  /// Return the numer of selected series for a patient
  Q_INVOKABLE int numberOfSeriesSelectedByPatient(QString patientUID) const;

  /// Return the numer of selected series for a study
  Q_INVOKABLE int numberOfSeriesSelectedByStudy(const QModelIndex& studyIndex) const;

  /// Clear all selections
  Q_INVOKABLE void clearSelection();

  /// Get number of selected items
  Q_INVOKABLE int selectedCount() const;
  ///@}

  /// \name Data access
  ///@{
  /// Get study instance UID for a given index
  Q_INVOKABLE QString studyInstanceUID(const QModelIndex& index) const;

  /// Get index for a study instance UID
  Q_INVOKABLE QModelIndex indexForStudyInstanceUID(const QString& studyInstanceUID) const;
  ///@}

  /// \name Interaction state
  ///@{
  /// Check if a collapse indicator is currently being hovered
  Q_INVOKABLE bool isCollapseIndicatorHovered(const QModelIndex& studyIndex) const;

  /// Check if a collapse indicator is currently being pressed
  Q_INVOKABLE bool isCollapseIndicatorPressed(const QModelIndex& studyIndex) const;

  /// Check if select all icon is hovered for a study
  Q_INVOKABLE bool isSelectAllIconHovered(const QModelIndex& studyIndex) const;

  /// Check if select all icon is pressed for a study
  Q_INVOKABLE bool isSelectAllIconPressed(const QModelIndex& studyIndex) const;

  /// Check if context menu button is hovered for a study
  Q_INVOKABLE bool isContextMenuButtonHovered(const QModelIndex& studyIndex) const;

  /// Check if context menu button is pressed for a study
  Q_INVOKABLE bool isContextMenuButtonPressed(const QModelIndex& studyIndex) const;

  /// Check if a study has an active context menu
  Q_INVOKABLE bool hasActiveContextMenu(const QModelIndex& studyIndex) const;

  /// Check if select all icon is hovered for a study
  Q_INVOKABLE bool isOperationStatusIconHovered(const QModelIndex& studyIndex) const;

  /// Check if select all icon is pressed for a study
  Q_INVOKABLE bool isOperationStatusIconPressed(const QModelIndex& studyIndex) const;
  ///@}

  /// \name Study state management
  ///@{
  /// Toggle study collapsed state
  Q_INVOKABLE void toggleStudyCollapsed(const QModelIndex& index);

  /// Set study collapsed state
  Q_INVOKABLE void setStudyCollapsed(const QModelIndex& index, bool collapsed);

  /// Get study collapsed state
  Q_INVOKABLE bool isStudyCollapsed(const QModelIndex& index) const;

  /// Collapse all studies
  Q_INVOKABLE void collapseAllStudies();

  /// Expand all studies
  Q_INVOKABLE void expandAllStudies();
  ///@}

  /// \name Convenience methods
  ///@{
  /// Get the study model (cast from model())
  Q_INVOKABLE ctkDICOMStudyModel* studyModel() const;
  Q_INVOKABLE ctkDICOMStudyFilterProxyModel* studyFilterProxyModel() const;
  Q_INVOKABLE ctkDICOMStudyMergedFilterProxyModel* studyMergedFilterProxyModel() const;

  /// Scroll to study by instance UID
  Q_INVOKABLE void scrollToStudyInstanceUID(const QString& studyInstanceUID);

  /// Refresh the view layout
  Q_INVOKABLE void refreshLayout(bool scheduleDelayedItemsLayout = true);
  Q_INVOKABLE void refreshSeriesLayout();

  /// Clean the view and all child series views
  Q_INVOKABLE void clean();

  /// Refresh all studies models
  Q_INVOKABLE void refreshStudiesModels(QList<ctkDICOMStudyModel*> models = QList<ctkDICOMStudyModel*>());
  ///@}

  /// \name Series view management
  ///@{
  /// Create a series table view for the given study
  /// The view is owned and managed by this study list view
  Q_INVOKABLE ctkDICOMSeriesTableView* createSeriesViewForStudy(const QModelIndex& studyIndex);
  Q_INVOKABLE ctkDICOMSeriesTableView* createSeriesViewForStudy(const QString& studyInstanceUID);
  Q_INVOKABLE QMap<QString, ctkDICOMSeriesTableView*> createAllSeriesView();

  /// Get a series table view for the given study
  Q_INVOKABLE ctkDICOMSeriesTableView* getSeriesViewForStudy(const QModelIndex& studyIndex);
  Q_INVOKABLE ctkDICOMSeriesTableView* getSeriesViewForStudy(const QString& studyInstanceUID);
  Q_INVOKABLE QMap<QString, ctkDICOMSeriesTableView*> getAllSeriesView();

  /// Remove and cleanup series view for a study
  Q_INVOKABLE void removeSeriesViewForStudy(const QModelIndex& studyIndex);
  Q_INVOKABLE void removeSeriesViewForStudy(const QString& studyInstanceUID);

  /// Check if a series view exists for the given study
  Q_INVOKABLE bool hasSeriesViewForStudy(const QModelIndex& studyIndex) const;
  Q_INVOKABLE bool hasSeriesViewForStudy(const QString& studyInstanceUID) const;
  ///@}

public slots:
  /// Called when number of opened studies changes
  void onNumberOfOpenedStudiesChanged(int count = 0);

  /// Set study collapsed state by studyInstanceUID
  void setStudyCollapsed(const QString& studyInstanceUID, bool collapsed);

  /// Called when study context menu is requested from delegate (3-dot button click)
  void onStudyContextMenuRequested(const QPoint& globalPos, const QModelIndex& studyIndex);

  /// Called when scroll bar value changes
  void onScrollBarValueChanged(int value);

protected:
  /// Reimplemented from QListView
  void resizeEvent(QResizeEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
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
  int sizeHintForColumn(int row) const override;

  /// Get the delegate as ctkDICOMStudyDelegate
  ctkDICOMStudyDelegate* studyDelegate() const;

protected slots:
  /// Called when selection changes
  void onStudiesSelectionChanged();

  /// Called when a series selection changes in a child series view
  void onSeriesSelectionChanged(const QString& studyInstanceUID, const QStringList& selectedSeriesInstanceUIDs);

  /// Called when model data changes
  void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

  /// Called when rows are inserted
  void onRowsInserted(const QModelIndex& parent, int first, int last);

  /// Called when rows are about to be removed
  void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);

  /// Called when rows are removed
  void onRowsRemoved(const QModelIndex& parent, int first, int last);

  /// Called when model is about to be reset
  void onModelAboutToBeReset();

  /// Called when model is reset
  void onModelReset();

  /// Called when series context menu is requested
  void onSeriesContextMenuRequested(const QPoint& globalPos, const QStringList& selectedSeriesInstanceUIDs);

  /// Called when series grid columns change (requires study item size hint update)
  void onSeriesGridColumnsChanged(const QString& studyInstanceUID);

  /// Called when the series view is entered (mouse hover)
  void onSeriesViewEntered();

  /// Called when load button is clicked
  void onLoadButtonClicked();

signals:
  /// Emitted when the mouse enters the study list view
  void studyListViewEntered();

  /// Emitted when selection changes
  void studySelectionChanged(const QStringList& selectedStudyInstanceUIDs);

  /// Emitted when selection changes
  void seriesSelectionChanged(const QString& studyInstanceUID, const QStringList& selectedSeriesInstanceUIDs);

  /// Emitted when a study is activated (double-clicked)
  void studyActivated(const QString& studyInstanceUID);

  /// Emitted when a series is activated (double-clicked)
  void seriesActivated(const QString& seriesInstanceUID);

  /// Emitted when study collapsed state changes
  void studyCollapsedChanged(const QString& studyInstanceUID, bool collapsed);

  /// Emitted when study header context menu is requested (passes selected study UIDs)
  void studyContextMenuRequested(const QPoint& globalPos, const QStringList& selectedStudyInstanceUIDs);

  /// Emitted when series context menu is requested
  void seriesContextMenuRequested(const QPoint& globalPos, const QStringList& selectedSeriesInstanceUIDs);

  /// Emitted when load button is clicked with selected series UIDs
  void loadSeriesRequested(const QStringList& seriesInstanceUIDs);

  /// Emitted when view layout changes
  void layoutChanged();

private:
  Q_DECLARE_PRIVATE(ctkDICOMStudyListView);
  Q_DISABLE_COPY(ctkDICOMStudyListView);

protected:
  QScopedPointer<ctkDICOMStudyListViewPrivate> d_ptr;
};

#endif
