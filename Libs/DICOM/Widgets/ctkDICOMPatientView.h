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

#ifndef __ctkDICOMPatientView_h
#define __ctkDICOMPatientView_h

// Qt includes
#include <QListView>

// CTK includes
#include "ctkDICOMWidgetsExport.h"

class ctkDICOMPatientViewPrivate;
class ctkDICOMPatientModel;
class ctkDICOMPatientDelegate;
class ctkDICOMStudyListView;
class ctkDICOMPatientFilterProxyModel;

/// \ingroup DICOM_Widgets
/// \brief Custom view for displaying DICOM patients
///
/// This view extends QListView to provide patient display with support for:
/// - Two display modes: TabMode and ListMode
/// - Single patient selection in TabMode, multi-selection in ListMode
/// - Integration with ctkDICOMPatientModel and ctkDICOMPatientDelegate
/// - Patient-based display with metadata (name, ID, birth date, sex, study count)
/// - Context menu integration
/// - Keyboard navigation
/// - Single embedded ctkDICOMStudyListView that dynamically updates based on patient selection
///
/// The view uses a single shared study list view that is reused for all patients,
/// improving performance and reducing memory overhead compared to per-patient views.
///
/// In ListMode, uses a QSplitter widget to properly separate patient list from study list.
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMPatientView : public QListView
{
  Q_OBJECT
  Q_PROPERTY(DisplayMode displayMode READ displayMode WRITE setDisplayMode NOTIFY displayModeChanged)

public:
  typedef QListView Superclass;

  /// Display mode options
  enum DisplayMode
  {
    TabMode = 0,  ///< Horizontal tab-like display (mimics QTabWidget)
    ListMode      ///< Vertical list of patient cards
  };
  Q_ENUM(DisplayMode)

  explicit ctkDICOMPatientView(QWidget* parent = nullptr);
  virtual ~ctkDICOMPatientView();

  /// Reimplemented from QAbstractItemView
  Q_INVOKABLE void setModel(QAbstractItemModel* model) override;

  /// \name Display mode
  ///@{
  /// Get/Set the display mode (TabMode or ListMode)
  DisplayMode displayMode() const;
  void setDisplayMode(DisplayMode mode);
  ///@}

  /// \name Selection
  ///@{
  /// Get selected patients UIDs
  Q_INVOKABLE QStringList selectedPatientUIDs() const;

  /// Get current patient UID
  Q_INVOKABLE QString currentPatientUID() const;

  /// Get currently selected patient ID
  Q_INVOKABLE QString currentPatientID() const;

  /// Get currently selected patient name
  Q_INVOKABLE QString currentPatientName() const;

  /// Select patients by patient UID
  Q_INVOKABLE void selectPatientUIDs(const QStringList& patientUIDs);

  /// Select patient by patient UID
  Q_INVOKABLE void selectPatientUID(const QString& patientUID,
                                    QItemSelectionModel::SelectionFlag flag = QItemSelectionModel::ClearAndSelect);

  /// Select patients by patient ID
  Q_INVOKABLE void selectPatientIDs(const QStringList& patientIDs);

  /// Select patient by patient ID
  Q_INVOKABLE void selectPatientID(const QString& patientID,
                                   QItemSelectionModel::SelectionFlag flag = QItemSelectionModel::ClearAndSelect);

  /// Clear selection
  Q_INVOKABLE void clearSelection();

  /// Get current selected index
  Q_INVOKABLE QModelIndex currentPatientIndex() const;

  /// Get selected series
  Q_INVOKABLE QStringList selectedSeriesInstanceUIDs() const;

  /// Select series
  Q_INVOKABLE void selectSeriesInstanceUIDs(const QStringList& seriesInstanceUIDs);
  ///@}

  /// \name Data access
  ///@{
  /// Get patient UID for a given index
  Q_INVOKABLE QString patientUID(const QModelIndex& index) const;

  /// Get patient ID for a given index
  Q_INVOKABLE QString patientID(const QModelIndex& index) const;

  /// Get index for a patient UID
  Q_INVOKABLE QModelIndex indexForPatientUID(const QString& patientUID) const;

  /// Get index for a patient ID
  Q_INVOKABLE QModelIndex indexForPatientID(const QString& patientID) const;
  ///@}

  /// \name Study list view management
  ///@{
  /// Get the shared study list view
  /// The view is owned and managed by this patient view.
  Q_INVOKABLE ctkDICOMStudyListView* studyListView() const;
  ///@}

  /// \name Convenience methods
  ///@{
  /// Get the patient model (cast from model())
  Q_INVOKABLE ctkDICOMPatientModel* patientModel() const;
  Q_INVOKABLE ctkDICOMPatientFilterProxyModel* patientFilterProxyModel() const;

  /// Scroll to patient by UID
  Q_INVOKABLE void scrollToPatientUID(const QString& patientUID);

  /// Refresh the view layout
  Q_INVOKABLE void refreshLayout();

  /// Clean the view and all child views (study view and series views)
  Q_INVOKABLE void clean();
  ///@}

  /// Check if a given index is currently hovered (for delegate rendering)
  Q_INVOKABLE bool isIndexHovered(const QModelIndex& index) const;

  /// Check if context menu button is pressed for a given index (for delegate rendering)
  Q_INVOKABLE bool isContextMenuButtonPressed(const QModelIndex& index) const;

  /// Check if context menu button is hovered for a given index (for delegate rendering)
  Q_INVOKABLE bool isContextMenuButtonHovered(const QModelIndex& index) const;

  /// Check if there is an active context menu for a given index (for delegate rendering)
  Q_INVOKABLE bool hasActiveContextMenu(const QModelIndex& index) const;

  /// Set the context menu button pressed index (for delegate to update visual state)
  Q_INVOKABLE void setContextMenuButtonPressed(const QModelIndex& index);

  /// Check if select all icon is pressed for a given index (for delegate rendering)
  Q_INVOKABLE bool isSelectAllIconPressed(const QModelIndex& index) const;

  /// Check if select all icon is hovered for a given index (for delegate rendering)
  Q_INVOKABLE bool isSelectAllIconHovered(const QModelIndex& index) const;

public slots:
  /// Called when context menu is requested from the delegate
  void onPatientContextMenuRequested(const QPoint& globalPos, const QModelIndex& clickedPatientIndex);

  /// Called when select all icon is clicked from the delegate
  void onSelectAllIconClicked(const QModelIndex& patientIndex);

  /// Called when the study view is entered (mouse hover)
  void onStudyViewEntered();

protected:
  /// Reimplemented from QListView
  void resizeEvent(QResizeEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void contextMenuEvent(QContextMenuEvent* event) override;
  void paintEvent(QPaintEvent* event) override;
  void changeEvent(QEvent* event) override;

  /// Event filter to handle display mode button events
  bool eventFilter(QObject* watched, QEvent* event) override;

  /// Get the delegate as ctkDICOMPatientDelegate
  ctkDICOMPatientDelegate* patientDelegate() const;

protected slots:
  /// Called when selection changes
  void onPatientSelectionChanged();

  /// Called when scrollbar value changes
  void onScrollBarValueChanged(int value);

  /// Called when a studies selection changes in the child study view
  void onStudiesSelectionChanged(const QStringList& selectedStudiesInstanceUIDs);

  /// Called when model data changes
  void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

  /// Called when rows are inserted
  void onRowsInserted(const QModelIndex& parent, int first, int last);

  /// Called when rows are removed
  void onRowsRemoved(const QModelIndex& parent, int first, int last);

  /// Called when model is about to be reset
  void onModelAboutToBeReset();

  /// Called when model is reset
  void onModelReset();

  /// Called when layout needs to be refreshed
  void onLayoutRefreshed();

  /// Called when study selection changes in the embedded study view
  void onStudySelectionChanged();

  /// Called when the display Mode button is clicked
  void onDisplayModeButtonClicked();

  /// Called when allowed servers combo box selection changes
  void onAllowedServersComboBoxChanged();

signals:
  /// Emitted when selection changes
  void studiesSelectionChanged(const QStringList& selectedStudiesInstanceUIDs);

  /// Emitted when context menu is requested for patient(s) (from 3-dot button or right-click)
  void patientContextMenuRequested(const QPoint& globalPos, const QStringList& selectedPatientUIDs);

  /// Emitted when display mode changes
  void displayModeChanged(DisplayMode mode);

  /// Emitted when view layout changes
  void layoutChanged();

private:
  Q_DECLARE_PRIVATE(ctkDICOMPatientView);
  Q_DISABLE_COPY(ctkDICOMPatientView);

protected:
  QScopedPointer<ctkDICOMPatientViewPrivate> d_ptr;
};

#endif
