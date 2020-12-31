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

=========================================================================*/

#ifndef __ctkDICOMBrowser_h
#define __ctkDICOMBrowser_h

// Qt includes
#include <QItemSelection>
#include <QWidget>

#include "ctkDICOMWidgetsExport.h"

#include "ctkDICOMModel.h"

class ctkDICOMBrowserPrivate;
class ctkDICOMDatabase;
class ctkDICOMTableManager;
class ctkFileDialog;
class ctkThumbnailLabel;
class QMenu;
class QModelIndex;

/// \ingroup DICOM_Widgets
///
/// \brief The DICOM browser widget provides an interface to organize DICOM
/// data stored in a local ctkDICOMDatabase.
///
/// Using a local database avoids redundant calculations and speed up subsequent
/// access.
///
/// Supported operations are:
///
/// * Import from file system
/// * Export
/// * Query
/// * Send (emits signal only, requires external implementation)
/// * Repair
/// * Remove
/// * Metadata
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMBrowser : public QWidget
{
  Q_OBJECT
  Q_ENUMS(ImportDirectoryMode)
  Q_PROPERTY(QString databaseDirectory READ databaseDirectory WRITE setDatabaseDirectory)
  Q_PROPERTY(QString databaseDirectorySettingsKey READ databaseDirectorySettingsKey WRITE setDatabaseDirectorySettingsKey)
  Q_PROPERTY(QString databaseDirectoryBase READ databaseDirectoryBase WRITE setDatabaseDirectoryBase)
  Q_PROPERTY(int patientsAddedDuringImport READ patientsAddedDuringImport)
  Q_PROPERTY(int studiesAddedDuringImport READ studiesAddedDuringImport)
  Q_PROPERTY(int seriesAddedDuringImport READ seriesAddedDuringImport)
  Q_PROPERTY(int instancesAddedDuringImport READ instancesAddedDuringImport)
  Q_PROPERTY(QStringList tagsToPrecache READ tagsToPrecache WRITE setTagsToPrecache)
  Q_PROPERTY(bool displayImportSummary READ displayImportSummary WRITE setDisplayImportSummary)
  Q_PROPERTY(ctkDICOMBrowser::ImportDirectoryMode ImportDirectoryMode READ importDirectoryMode WRITE setImportDirectoryMode)
  Q_PROPERTY(bool confirmRemove READ confirmRemove WRITE setConfirmRemove)
  Q_PROPERTY(bool toolbarVisible READ isToolbarVisible WRITE setToolbarVisible)
  Q_PROPERTY(bool databaseDirectorySelectorVisible READ isDatabaseDirectorySelectorVisible WRITE setDatabaseDirectorySelectorVisible)
  Q_PROPERTY(bool sendActionVisible READ isSendActionVisible WRITE setSendActionVisible)

public:
  typedef ctkDICOMBrowser Self;

  typedef QWidget Superclass;
  explicit ctkDICOMBrowser(QWidget* parent=0);
  explicit ctkDICOMBrowser(QSharedPointer<ctkDICOMDatabase> sharedDatabase, QWidget* parent=0);
  virtual ~ctkDICOMBrowser();

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

  /// See ctkDICOMDatabase for description - these accessors
  /// delegate to the corresponding routines of the internal
  /// instance of the database.
  /// @see ctkDICOMDatabase
  void setTagsToPrecache(const QStringList tags);
  const QStringList tagsToPrecache();

  Q_INVOKABLE ctkDICOMDatabase* database();

  Q_INVOKABLE ctkDICOMTableManager* dicomTableManager();

  /// Option to show or not import summary.
  void setDisplayImportSummary(bool);
  bool displayImportSummary();
  /// Option to show dialog to confirm removal from the database (Remove action). Off by default.
  void setConfirmRemove(bool);
  bool confirmRemove();

  /// Accessors to status of last directory import operation
  int patientsAddedDuringImport();
  int studiesAddedDuringImport();
  int seriesAddedDuringImport();
  int instancesAddedDuringImport();

  /// Set counters of imported patients, studies, series, instances to zero.
  void resetItemsAddedDuringImportCounters();

  enum ImportDirectoryMode
  {
    ImportDirectoryCopy = 0,
    ImportDirectoryAddLink
  };

  /// \brief Get value of ImportDirectoryMode settings.
  ///
  /// \sa setImportDirectoryMode(ctkDICOMBrowser::ImportDirectoryMode)
  ctkDICOMBrowser::ImportDirectoryMode importDirectoryMode()const;

  /// \brief Return instance of import dialog.
  ///
  /// \internal
  Q_INVOKABLE ctkFileDialog* importDialog()const;

  void setToolbarVisible(bool state);
  bool isToolbarVisible() const;

  void setDatabaseDirectorySelectorVisible(bool visible);
  bool isDatabaseDirectorySelectorVisible() const;

  void setSendActionVisible(bool visible);
  bool isSendActionVisible() const;

public Q_SLOTS:

  /// \brief Set value of ImportDirectoryMode settings.
  ///
  /// Setting the value will update the comboBox found at the bottom
  /// of the import dialog.
  ///
  /// \sa importDirectoryMode()
  void setImportDirectoryMode(ctkDICOMBrowser::ImportDirectoryMode mode);

  void setDatabaseDirectory(const QString& directory);

  /// \brief Pop-up file dialog allowing to select and import one or multiple
  /// DICOM directories.
  ///
  /// The dialog is extended with two additional controls:
  ///
  /// * **ImportDirectoryMode** combox: Allow user to select "Add Link" or "Copy" mode.
  ///   Associated settings is stored using key `DICOM/ImportDirectoryMode`.
  void openImportDialog();

  void openExportDialog();
  void openSendDialog();
  void openQueryDialog();
  void onRemoveAction();
  void onRepairAction();

  /// \brief Import directories
  ///
  /// This can be used to externally trigger an import (i.e. for testing or to support drag-and-drop)
  ///
  /// By default, \a mode is ImportDirectoryMode::ImportDirectoryAddLink is set.
  ///
  /// \sa importDirectory(QString directory, int mode)
  void importDirectories(QStringList directories, ctkDICOMBrowser::ImportDirectoryMode mode = ImportDirectoryAddLink);

  /// \brief Import a directory
  ///
  /// This can be used to externally trigger an import (i.e. for testing or to support drag-and-drop)
  ///
  /// By default, \a mode is ImportDirectoryMode::ImportDirectoryAddLink is set.
  void importDirectory(QString directory, ctkDICOMBrowser::ImportDirectoryMode mode = ImportDirectoryAddLink);

  /// \brief Import a list of files
  ///
  /// This can be used to externally trigger an import (i.e. for testing or to support drag-and-drop)
  ///
  /// By default, \a mode is ImportDirectoryMode::ImportDirectoryAddLink is set.
  void importFiles(const QStringList& files, ctkDICOMBrowser::ImportDirectoryMode mode = ImportDirectoryAddLink);

  /// Wait for all import operations to complete.
  /// Number of imported patients, studies, series, images since the last resetItemsAddedDuringImportCounters
  /// can be retrieved by calling patientsAddedDuringImport(), studiesAddedDuringImport(), seriesAddedDuringImport(),
  ///  instancesAddedDuringImport() methods.
  void waitForImportFinished();

  /// \deprecated importDirectory() should be used
  void onImportDirectory(QString directory, ctkDICOMBrowser::ImportDirectoryMode mode = ImportDirectoryAddLink);

  /// slots to capture status updates from the database during an
  /// import operation
  void onIndexingProgress(int);
  void onIndexingProgressStep(const QString&);
  void onIndexingProgressDetail(const QString&);
  void onIndexingUpdatingDatabase(bool updating);
  void onIndexingComplete(int patientsAdded, int studiesAdded, int seriesAdded, int imagesAdded);

  /// Show pop-up window for the user to select database directory
  void selectDatabaseDirectory();

  /// Create new database directory.
  /// Current database directory used as a basis.
  void createNewDatabaseDirectory();

  /// Update database in-place to required schema version
  void updateDatabase();

  /// Show progress dialog for update displayed fields
  void showUpdateDisplayedFieldsDialog();

  QStringList fileListForCurrentSelection(ctkDICOMModel::IndexType level);

  /// Show window that displays DICOM fields of all selected items
  void showMetadata(const QStringList& fileList);

  void removeSelectedItems(ctkDICOMModel::IndexType level);

Q_SIGNALS:
  /// Emitted when directory is changed
  void databaseDirectoryChanged(const QString&);
  /// Emitted when query/retrieve operation has happened
  void queryRetrieveFinished();
  /// Emitted when the directory import operation has completed
  void directoryImported();
  /// Emitted when user requested network send. String list contains list of files to be exported.
  void sendRequested(const QStringList&);

protected:
    QScopedPointer<ctkDICOMBrowserPrivate> d_ptr;

    /// Confirm with the user that they wish to delete the selected uids.
    /// Add information about the selected UIDs to a message box, checks
    /// for patient name, series description, study description, if all
    /// empty, uses the UID.
    /// Returns true if the user confirms the delete, false otherwise.
    /// Remembers if the user doesn't want to show the confirmation again.
    bool confirmDeleteSelectedUIDs(QStringList uids);

protected Q_SLOTS:
    /// \brief Import directories
    ///
    /// This is used when user selected one or multiple
    /// directories from the Import Dialog.
    ///
    /// \sa importDirectories(QString directory, int mode)
    void onImportDirectoriesSelected(QStringList directories);

    void onImportDirectoryComboBoxCurrentIndexChanged(int index);

    void onModelSelected(const QItemSelection&, const QItemSelection&);

    /// Called when a right mouse click is made in the patients table
    void onPatientsRightClicked(const QPoint &point);

    /// Called when a right mouse click is made in the studies table
    void onStudiesRightClicked(const QPoint &point);

    /// Called when a right mouse click is made in the series table
    void onSeriesRightClicked(const QPoint &point);

    /// Called to export the series associated with the selected UIDs
    /// \sa exportSelectedStudies, exportSelectedPatients
    void exportSeries(QString dirPath, QStringList uids);

    /// Called to export the studies associated with the selected UIDs
    /// \sa exportSelectedSeries, exportSelectedPatients
    void exportSelectedItems(ctkDICOMModel::IndexType level);

    /// To be called when dialog finishes
    void onQueryRetrieveFinished();

private:
  Q_DECLARE_PRIVATE(ctkDICOMBrowser);
  Q_DISABLE_COPY(ctkDICOMBrowser);
};

Q_DECLARE_METATYPE(ctkDICOMBrowser::ImportDirectoryMode)

#endif
