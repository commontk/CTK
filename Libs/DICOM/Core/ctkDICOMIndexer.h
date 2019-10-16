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

#ifndef __ctkDICOMIndexer_h
#define __ctkDICOMIndexer_h

// Qt includes
#include <QObject>
#include <QSqlDatabase>

#include "ctkDICOMCoreExport.h"
#include "ctkDICOMDatabase.h"

class ctkDICOMIndexerPrivate;

/// \ingroup DICOM_Core
///
/// \brief Indexes DICOM images located in local directory into an Sql database
///
class CTK_DICOM_CORE_EXPORT ctkDICOMIndexer : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool backgroundImportEnabled READ isBackgroundImportEnabled WRITE setBackgroundImportEnabled)
  Q_PROPERTY(bool importing READ isImporting)

public:
  explicit ctkDICOMIndexer(QObject *parent = 0);
  virtual ~ctkDICOMIndexer();

  Q_INVOKABLE void setDatabase(ctkDICOMDatabase* database);
  Q_INVOKABLE ctkDICOMDatabase* database();

  /// If enabled, addDirectory and addFile... methods return immediately
  /// indexing is performed in a background thread,
  /// and progress and completion are indicated by signals.
  /// Disabled by default.
  void setBackgroundImportEnabled(bool);
  bool isBackgroundImportEnabled() const;

  /// Returns with true if background importing is currently in progress.
  bool isImporting();

  ///
  /// \brief Adds directory to database and optionally copies files to
  /// destinationDirectory.
  ///
  /// Scan the directory using Dcmtk and populate the database with all the
  /// DICOM images accordingly.
  ///
  /// If includeHidden is set to false then hidden files and folders are not added.
  /// DICOM folders may be created based on series or study name, which sometimes start
  /// with a . character, therefore it is advisable to include hidden files and folders.
  ///
  Q_INVOKABLE void addDirectory(const QString& directoryName, bool copyFile = false, bool includeHidden = true);
  /// Kept for backward compatibility
  Q_INVOKABLE void addDirectory(ctkDICOMDatabase* db, const QString& directoryName, bool copyFile = false, bool includeHidden = true);

  ///
  /// \brief Adds directory to database by using DICOMDIR and optionally copies files to
  /// destinationDirectory.
  /// Scan the directory using Dcmtk and populate the database with all the
  /// DICOM images accordingly.
  /// \return Returns false if there was an error while processing the DICOMDIR file.
  ///
  Q_INVOKABLE bool addDicomdir(const QString& directoryName, bool copyFile = false);
  /// Kept for backward compatibility
  Q_INVOKABLE bool addDicomdir(ctkDICOMDatabase* db, const QString& directoryName, bool copyFile = false);

  ///
  /// \brief Adds a QStringList containing the file path to database and optionally copies files to
  /// destinationDirectory.
  ///
  /// Scan the directory using Dcmtk and populate the database with all the
  /// DICOM images accordingly.
  ///
  Q_INVOKABLE void addListOfFiles(const QStringList& listOfFiles, bool copyFile = false);
  /// Kept for backward compatibility
  Q_INVOKABLE void addListOfFiles(ctkDICOMDatabase* db, const QStringList& listOfFiles, bool copyFile = false);

  ///
  /// \brief Adds a file to database and optionally copies the file to
  /// the database folder.
  /// If destinationDirectory is non-empty string then the file is copied
  /// to the database folder (exact value of destinationDirectoryName does not matter,
  /// only if the string is empty or not).
  ///
  /// Scan the file using Dcmtk and populate the database with all the
  /// DICOM fields accordingly.
  ///
  Q_INVOKABLE void addFile(const QString filePath, bool copyFile = false);
  /// Kept for backward compatibility
  Q_INVOKABLE void addFile(ctkDICOMDatabase* db, const QString filePath, bool copyFile = false);

  ///
  /// \brief Wait for all the indexing operations to complete
  /// This can be useful to ensure that importing is completed when background indexing is enabled.
  /// msecTimeout specifies a maximum timeout. If <0 then it means wait indefinitely.
  Q_INVOKABLE void waitForImportFinished(int msecTimeout = -1);

Q_SIGNALS:
  /// Description of current phase of the indexing (parsing, importing, ...)
  void progressStep(QString);
  /// Detailed information about the current progress (e.g., name of currently processed file)
  void progressDetail(QString);
  /// Progress in percentage
  void progress(int);
  /// Indexing is completed.
  void indexingComplete(int patientsAdded, int studiesAdded, int seriesAdded, int imagesAdded);
  void updatingDatabase(bool);

public Q_SLOTS:
  /// Stop indexing (all completed indexing results will be added to the database)
  void cancel();

protected Q_SLOTS:
  void databaseFilenameChanged();
  void tagsToPrecacheChanged();
  void tagsToExcludeFromStorageChanged();

protected:
  QScopedPointer<ctkDICOMIndexerPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMIndexer);
  Q_DISABLE_COPY(ctkDICOMIndexer);

};

#endif
