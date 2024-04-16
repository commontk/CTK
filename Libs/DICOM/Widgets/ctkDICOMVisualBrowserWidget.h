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
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

#ifndef __ctkDICOMVisualBrowserWidget_h
#define __ctkDICOMVisualBrowserWidget_h

// Qt includes
#include <QVariant>
#include <QWidget>

// ctkDICOMCore includes
#include <ctkDICOMModel.h>

// ctkDICOMWidgets includes
#include "ctkDICOMPatientItemWidget.h"
#include "ctkDICOMStudyItemWidget.h"
#include "ctkDICOMWidgetsExport.h"

// DCMTK includes
#include <dcmtk/oflog/oflog.h>

class ctkCollapsibleGroupBox;
class ctkDICOMVisualBrowserWidgetPrivate;
class ctkDICOMDatabase;
class ctkDICOMJobListWidget;
class ctkDICOMScheduler;
class ctkDICOMServer;
class ctkDICOMServerNodeWidget2;
class ctkDICOMJobResponseSet;
class ctkFileDialog;

/// \ingroup DICOM_Widgets
///
/// \brief The DICOM visual browser widget provides an interface to organize DICOM
/// data stored in a local/server ctkDICOMDatabases.
///
/// Using a local database avoids redundant calculations and speed up subsequent
/// access.
///
/// The operations are queued by the scheduler into jobs with a priority and
/// executed by workers in separate threads.
///
/// Supported operations are:
///
/// * Filtering and navigation with thumbnails of local database and servers results
/// * Import from file system to local database
/// * Query/Retrieve from servers (DIMSE C-GET/C-MOVE )
/// * Storage listener
/// * Send (emits only a signal for the moment, requires external implementation)
/// * Remove (only from local database, not from server)
/// * Metadata exploration
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMVisualBrowserWidget : public QWidget
{
  Q_OBJECT;
  Q_ENUMS(ImportDirectoryMode)
  Q_PROPERTY(QString databaseDirectory READ databaseDirectory WRITE setDatabaseDirectory)
  Q_PROPERTY(QString databaseDirectorySettingsKey READ databaseDirectorySettingsKey WRITE setDatabaseDirectorySettingsKey)
  Q_PROPERTY(QString databaseDirectoryBase READ databaseDirectoryBase WRITE setDatabaseDirectoryBase)
  Q_PROPERTY(QString filteringPatientID READ filteringPatientID WRITE setFilteringPatientID);
  Q_PROPERTY(QString filteringPatientName READ filteringPatientName WRITE setFilteringPatientName);
  Q_PROPERTY(int numberOfStudiesPerPatient READ numberOfStudiesPerPatient WRITE setNumberOfStudiesPerPatient);
  Q_PROPERTY(ctkDICOMStudyItemWidget::ThumbnailSizeOption thumbnailSize READ thumbnailSize WRITE setThumbnailSize);
  Q_PROPERTY(ctkDICOMVisualBrowserWidget::ImportDirectoryMode ImportDirectoryMode READ importDirectoryMode WRITE setImportDirectoryMode)
  Q_PROPERTY(bool sendActionVisible READ isSendActionVisible WRITE setSendActionVisible)
  Q_PROPERTY(bool deleteActionVisible READ isDeleteActionVisible WRITE setDeleteActionVisible)
  Q_PROPERTY(QString storageAETitle READ storageAETitle WRITE setStorageAETitle);
  Q_PROPERTY(int storagePort READ storagePort WRITE setStoragePort);

public:
  typedef QWidget Superclass;
  explicit ctkDICOMVisualBrowserWidget(QWidget* parent = nullptr);
  virtual ~ctkDICOMVisualBrowserWidget();

  /// Directory being used to store the dicom database
  QString databaseDirectory() const;

  /// Get settings key used to store DatabaseDirectory in application settings.
  QString databaseDirectorySettingsKey() const;

  /// Set settings key that stores DatabaseDirectory in application settings.
  /// Calling this method sets DatabaseDirectory from current value stored in the settings
  /// (overwriting current value of DatabaseDirectory).
  void setDatabaseDirectorySettingsKey(const QString& settingsKey);

  /// Get the directory that will be used as a basis if databaseDirectory is specified with a relative path.
  /// @see setDatabaseDirectoryBase, setDatabaseDirectory
  QString databaseDirectoryBase() const;

  /// Set the directory that will be used as a basis if databaseDirectory is specified with a relative path.
  /// If DatabaseDirectoryBase is empty (by default it is) then the current working directory is used as a basis.
  /// @see databaseDirectoryBase, setDatabaseDirectory
  void setDatabaseDirectoryBase(const QString& base);

  /// Return the task pool.
  Q_INVOKABLE ctkDICOMScheduler* scheduler() const;
  /// Return the task pool as a shared pointer
  /// (not Python-wrappable).
  QSharedPointer<ctkDICOMScheduler> schedulerShared() const;
  /// Set the task pool.
  Q_INVOKABLE void setScheduler(ctkDICOMScheduler& scheduler);
  /// Set the task pool as a shared pointer
  /// (not Python-wrappable).
  void setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler);

  /// Return the Dicom Database.
  Q_INVOKABLE ctkDICOMDatabase* dicomDatabase() const;
  /// Return Dicom Database as a shared pointer
  /// (not Python-wrappable).
  QSharedPointer<ctkDICOMDatabase> dicomDatabaseShared() const;

  ///@{
  /// See ctkDICOMDatabase for description - these accessors
  /// delegate to the corresponding routines of the internal
  /// instance of the database.
  /// @see ctkDICOMDatabase
  Q_INVOKABLE void setTagsToPrecache(const QStringList& tags);
  Q_INVOKABLE const QStringList tagsToPrecache();
  ///@}

  ///@{
  /// Storage AE title
  /// "CTKSTORE" by default
  void setStorageAETitle(const QString& storageAETitle);
  QString storageAETitle() const;
  ///@}

  ///@{
  /// Storage port
  /// 11112 by default
  void setStoragePort(int storagePort);
  int storagePort() const;
  ///@}

  ///@{
  /// Servers
  Q_INVOKABLE int serversCount();
  Q_INVOKABLE ctkDICOMServer* getNthServer(int id);
  Q_INVOKABLE ctkDICOMServer* getServer(const QString& connectionName);
  Q_INVOKABLE void addServer(ctkDICOMServer* server);
  Q_INVOKABLE void removeServer(const QString& connectionName);
  Q_INVOKABLE void removeNthServer(int id);
  Q_INVOKABLE void removeAllServers();
  Q_INVOKABLE QString getServerNameFromIndex(int id);
  Q_INVOKABLE int getServerIndexFromName(const QString& connectionName);
  Q_INVOKABLE ctkDICOMJobListWidget* jobListWidget();
  Q_INVOKABLE ctkCollapsibleGroupBox* jobListGroupBox();
  Q_INVOKABLE ctkDICOMServerNodeWidget2* serverSettingsWidget();
  Q_INVOKABLE ctkCollapsibleGroupBox* serverSettingsGroupBox();
  ///@}

  ///@{
  /// Query Filters
  /// Empty by default
  void setFilteringPatientID(const QString& filteringPatientID);
  QString filteringPatientID() const;
  ///@}

  ///@{
  /// Empty by default
  void setFilteringPatientName(const QString& filteringPatientName);
  QString filteringPatientName() const;
  ///@}

  ///@{
  /// Empty by default
  void setFilteringStudyDescription(const QString& filteringStudyDescription);
  QString filteringStudyDescription() const;
  ///@}

  ///@{
  /// Available values:
  /// Any,
  /// Today,
  /// Yesterday,
  /// LastWeek,
  /// LastMonth,
  /// LastYear.
  /// Any by default.
  /// \sa ctkDICOMPatientItemWidget::DateType
  void setFilteringDate(const ctkDICOMPatientItemWidget::DateType& filteringDate);
  ctkDICOMPatientItemWidget::DateType filteringDate() const;
  ///@]

  ///@{
  /// Empty by default
  void setFilteringSeriesDescription(const QString& filteringSeriesDescription);
  QString filteringSeriesDescription() const;
  ///@}

  ///@{
  /// ["Any", "CR", "CT", "MR", "NM", "US", "PT", "XA"] by default
  void setFilteringModalities(const QStringList& filteringModalities);
  QStringList filteringModalities() const;
  ///@}

  /// reset all the filters
  Q_INVOKABLE void resetFilters();

  ///@{
  /// Number of non collapsed studies per patient
  /// 2 by default
  void setNumberOfStudiesPerPatient(int numberOfStudiesPerPatient);
  int numberOfStudiesPerPatient() const;
  ///@}

  ///@{
  /// Set the thumbnail size: small, medium, large
  /// medium by default
  void setThumbnailSize(const ctkDICOMStudyItemWidget::ThumbnailSizeOption& thumbnailSize);
  ctkDICOMStudyItemWidget::ThumbnailSizeOption thumbnailSize() const;
  ///@}

  ///@{
  /// Set if send action on right click context menu is available
  /// false by default
  void setSendActionVisible(bool visible);
  bool isSendActionVisible() const;
  ///@}

  ///@{
  /// Set if cancel action on right click context menu is available
  /// true by default
  void setDeleteActionVisible(bool visible);
  bool isDeleteActionVisible() const;
  ///@}

  ///@{
  /// Add/Remove Patient item widget
  Q_INVOKABLE int addPatientItemWidget(const QString& patientItem);
  Q_INVOKABLE void removePatientItemWidget(const QString& patientItem);
  ///@}

  /// Get Patient item widget
  Q_INVOKABLE ctkDICOMPatientItemWidget* getPatientItemWidgetByPatientName(const QString& patientName);

  ///@{
  /// Accessors to status of last directory import operation
  int patientsAddedDuringImport();
  int studiesAddedDuringImport();
  int seriesAddedDuringImport();
  int instancesAddedDuringImport();
  ///@}

  /// Set counters of imported patients, studies, series, instances to zero.
  Q_INVOKABLE void resetItemsAddedDuringImportCounters();

  enum ImportDirectoryMode
  {
    ImportDirectoryCopy = 0,
    ImportDirectoryAddLink
  };

  /// \brief Get value of ImportDirectoryMode settings.
  ///
  /// \sa setImportDirectoryMode(ctkDICOMBrowser::ImportDirectoryMode)
  ImportDirectoryMode importDirectoryMode() const;

  /// \brief Return instance of import dialog.
  ///
  /// \internal
  Q_INVOKABLE ctkFileDialog* importDialog() const;

public Q_SLOTS:
  /// \brief Set value of ImportDirectoryMode settings.
  ///
  /// Setting the value will update the comboBox found at the bottom
  /// of the import dialog.
  ///
  /// \sa importDirectoryMode()
  void setImportDirectoryMode(ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode);

  void setDatabaseDirectory(const QString& directory);

  /// \brief Pop-up file dialog allowing to select and import one or multiple
  /// DICOM directories.
  ///
  /// The dialog is extended with two additional controls:
  ///
  /// * **ImportDirectoryMode** combox: Allow user to select "Add Link" or "Copy" mode.
  ///   Associated settings is stored using key `DICOM/ImportDirectoryMode`.
  void openImportDialog();

  /// \brief Import directories
  ///
  /// This can be used to externally trigger an import (i.e. for testing or to support drag-and-drop)
  ///
  /// By default, \a mode is ImportDirectoryMode::ImportDirectoryAddLink is set.
  ///
  /// \sa importDirectory(QString directory, int mode)
  void importDirectories(const QStringList& directories, ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode = ImportDirectoryAddLink);

  /// \brief Import a directory
  ///
  /// This can be used to externally trigger an import (i.e. for testing or to support drag-and-drop)
  ///
  /// By default, \a mode is ImportDirectoryMode::ImportDirectoryAddLink is set.
  void importDirectory(const QString& directory, ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode = ImportDirectoryAddLink);

  /// \brief Import a list of files
  ///
  /// This can be used to externally trigger an import (i.e. for testing or to support drag-and-drop)
  ///
  /// By default, \a mode is ImportDirectoryMode::ImportDirectoryAddLink is set.
  void importFiles(const QStringList& files, ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode = ImportDirectoryAddLink);

  /// Wait for all import operations to complete.
  /// Number of imported patients, studies, series, images since the last resetItemsAddedDuringImportCounters
  /// can be retrieved by calling patientsAddedDuringImport(), studiesAddedDuringImport(), seriesAddedDuringImport(),
  ///  instancesAddedDuringImport() methods.
  void waitForImportFinished();

  ///@{
  /// slots to capture status updates from the database during an
  /// import operation
  void onIndexingProgress(int);
  void onIndexingProgressStep(const QString&);
  void onIndexingProgressDetail(const QString&);
  void onIndexingComplete(int patientsAdded, int studiesAdded, int seriesAdded, int imagesAdded);
  ///@}

  /// Show pop-up window for the user to select database directory
  void selectDatabaseDirectory();

  /// Create new database directory.
  /// Current database directory used as a basis.
  void createNewDatabaseDirectory();

  /// Update database in-place to required schema version
  void updateDatabase();

  ///@{
  /// slots to capture change in the filtering parameters for the query
  void onFilteringPatientIDChanged();
  void onFilteringPatientNameChanged();
  void onFilteringStudyDescriptionChanged();
  void onFilteringSeriesDescriptionChanged();
  void onFilteringModalityCheckableComboBoxChanged();
  void onFilteringDateComboBoxChanged(int);
  ///@}

  /// start query/retrieve and show patients
  void onQueryPatients();
  /// toggle query/retrieve option on search button
  void onQueryRetrieveOptionToggled(bool);
  /// show patients
  void onShowPatients();

  ///@{
  /// update GUI after query/retrieve operations
  void updateGUIFromScheduler(const QVariant&);
  void onJobFailed(const QVariant&);
  ///@}

  /// stops all the operations
  void onStop(bool stopPersistentTasks = false);

  ///@{
  /// high level UI slots: close, load, warning
  void onClose();
  void onLoad();
  void onWarningPushButtonClicked();
  ///@}

  /// user change patient selection
  void onPatientItemChanged(int);

  /// server settings have been changed
  void onServersSettingsChanged();

Q_SIGNALS:
  /// Emitted when directory is changed
  void databaseDirectoryChanged(const QString&);
  /// Emitted when retrieveSeries finish to retrieve the series.
  void seriesRetrieved(const QStringList& seriesInstanceUIDs);
  /// Emitted when user requested network send. String list contains list of files to be exported.
  void sendRequested(const QStringList&);
  /// Emitted when the directory import operation has completed
  void directoryImported();

protected:
  QScopedPointer<ctkDICOMVisualBrowserWidgetPrivate> d_ptr;

  /// Reimplemented for internal reasons
  virtual void closeEvent(QCloseEvent*);
  virtual void keyPressEvent(QKeyEvent* event);

  /// Confirm with the user that they wish to delete the selected uids.
  /// Add information about the selected UIDs to a message box, checks
  /// for patient name, series description, study description, if all
  /// empty, uses the UID.
  /// Returns true if the user confirms the delete, false otherwise.
  /// Remembers if the user doesn't want to show the confirmation again.
  bool confirmDeleteSelectedUIDs(const QStringList& uids);

  /// Get file list for right click selection
  QStringList fileListForCurrentSelection(ctkDICOMModel::IndexType level, const QList<QWidget*>& selectedWidget);
  /// Show window that displays DICOM fields of all selected items
  void showMetadata(const QStringList& fileList);
  /// Force redownload of selected series widgets
  void forceSeriesRetrieve(const QList<QWidget*>& selectedWidgets = QList<QWidget*>());
  /// Remove items (both database and widget)
  void removeSelectedItems(ctkDICOMModel::IndexType level, const QList<QWidget*>& selectedWidgets = QList<QWidget*>());
  /// Export the items associated with the selected widget
  void exportSelectedItems(ctkDICOMModel::IndexType level, const QList<QWidget*>& selectedWidgets);
  /// Export the series associated with the selected UIDs
  void exportSeries(const QString& dirPath, const QStringList& uids);

protected Q_SLOTS:
  ///@{
  /// \brief Import directories
  ///
  /// This is used when user selected one or multiple
  /// directories from the Import Dialog.
  ///
  /// \sa importDirectories(QString directory, int mode)
  void onImportDirectoriesSelected(const QStringList& directories);
  void onImportDirectoryComboBoxCurrentIndexChanged(int index);
  ///@}

  /// Called when a right mouse click is made on a tab of the patient tab widget
  void showPatientContextMenu(const QPoint& point);
  /// Called when a right mouse click is made in the studies table
  void showStudyContextMenu(const QPoint& point);
  /// Called when a right mouse click is made in the studies table
  void showSeriesContextMenu(const QPoint& point);
  /// Called when clicking patients tab menu
  void onPatientsTabMenuToolButtonClicked();

private:
  Q_DECLARE_PRIVATE(ctkDICOMVisualBrowserWidget);
  Q_DISABLE_COPY(ctkDICOMVisualBrowserWidget);
};

#endif
