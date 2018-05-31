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
/// * Import
/// * Export
/// * Send
/// * Query
/// * Remove
/// * Repair
///
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMBrowser : public QWidget
{
  Q_OBJECT
  Q_ENUMS(ImportDirectoryMode)
  Q_PROPERTY(QString databaseDirectory READ databaseDirectory WRITE setDatabaseDirectory)
  Q_PROPERTY(int patientsAddedDuringImport READ patientsAddedDuringImport)
  Q_PROPERTY(int studiesAddedDuringImport READ studiesAddedDuringImport)
  Q_PROPERTY(int seriesAddedDuringImport READ seriesAddedDuringImport)
  Q_PROPERTY(int instancesAddedDuringImport READ instancesAddedDuringImport)
  Q_PROPERTY(QStringList tagsToPrecache READ tagsToPrecache WRITE setTagsToPrecache)
  Q_PROPERTY(bool displayImportSummary READ displayImportSummary WRITE setDisplayImportSummary)
  Q_PROPERTY(ctkDICOMBrowser::ImportDirectoryMode ImportDirectoryMode READ importDirectoryMode WRITE setImportDirectoryMode)

public:
  typedef ctkDICOMBrowser Self;

  typedef QWidget Superclass;
  explicit ctkDICOMBrowser(QWidget* parent=0);
  virtual ~ctkDICOMBrowser();

  /// Directory being used to store the dicom database
  QString databaseDirectory() const;

  /// Return settings key used to store the directory.
  Q_INVOKABLE static QString databaseDirectorySettingsKey();

  /// See ctkDICOMDatabase for description - these accessors
  /// delegate to the corresponding routines of the internal
  /// instance of the database.
  /// @see ctkDICOMDatabase
  void setTagsToPrecache(const QStringList tags);
  const QStringList tagsToPrecache();

  /// Updates schema of loaded database to match the one
  /// coded by the current version of ctkDICOMDatabase.
  /// Also provides a dialog box for progress
  Q_INVOKABLE void updateDatabaseSchemaIfNeeded();

  Q_INVOKABLE ctkDICOMDatabase* database();

  Q_INVOKABLE ctkDICOMTableManager* dicomTableManager();

  /// Option to show or not import summary dialog.
  /// Since the summary dialog is modal, we give the option
  /// of disabling it for batch modes or testing.
  void setDisplayImportSummary(bool);
  bool displayImportSummary();
  /// Accessors to status of last directory import operation
  int patientsAddedDuringImport();
  int studiesAddedDuringImport();
  int seriesAddedDuringImport();
  int instancesAddedDuringImport();

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

public Q_SLOTS:

  /// \brief Set value of ImportDirectoryMode settings.
  ///
  /// Setting the value will update the comboBox found at the bottom
  /// of the import dialog.
  ///
  /// \sa importDirectoryMode()
  void setImportDirectoryMode(ctkDICOMBrowser::ImportDirectoryMode mode);

  void setDatabaseDirectory(const QString& directory);
  void onFileIndexed(const QString& filePath);

  /// \brief Pop-up file dialog allowing to select and import one or multiple
  /// DICOM directories.
  ///
  /// The dialog is extented with two additional controls:
  ///
  /// * **ImportDirectoryMode** combox: Allow user to select "Add Link" or "Copy" mode.
  ///   Associated settings is stored using key `DICOM/ImportDirectoryMode`.
  void openImportDialog();

  void openExportDialog();
  void openQueryDialog();
  void onRemoveAction();
  void onRepairAction();

  void onTablesDensityComboBox(QString);

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

  /// \deprecated importDirectory() should be used
  void onImportDirectory(QString directory, ctkDICOMBrowser::ImportDirectoryMode mode = ImportDirectoryAddLink);

  /// slots to capture status updates from the database during an
  /// import operation
  void onPatientAdded(int, QString, QString, QString);
  void onStudyAdded(QString);
  void onSeriesAdded(QString);
  void onInstanceAdded(QString);

Q_SIGNALS:
  /// Emited when directory is changed
  void databaseDirectoryChanged(const QString&);
  /// Emited when query/retrieve operation has happened
  void queryRetrieveFinished();
  /// Emited when the directory import operation has completed
  void directoryImported();

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
    void exportSelectedSeries(QString dirPath, QStringList uids);
    /// Called to export the studies associated with the selected UIDs
    /// \sa exportSelectedSeries, exportSelectedPatients
    void exportSelectedStudies(QString dirPath, QStringList uids);
    /// Called to export the patients associated with the selected UIDs
    /// \sa exportSelectedStudies, exportSelectedSeries
    void exportSelectedPatients(QString dirPath, QStringList uids);

    /// To be called when dialog finishes
    void onQueryRetrieveFinished();

private:
  Q_DECLARE_PRIVATE(ctkDICOMBrowser);
  Q_DISABLE_COPY(ctkDICOMBrowser);
};

Q_DECLARE_METATYPE(ctkDICOMBrowser::ImportDirectoryMode)

#endif
