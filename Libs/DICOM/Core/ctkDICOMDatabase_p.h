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

#ifndef __ctkDICOMDatabase_p_h
#define __ctkDICOMDatabase_p_h

//
//  W A R N I N G
//  -------------
//
// This file is not part of the CTK API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

// ctkDICOM includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMDisplayedFieldGenerator.h"

class CTK_DICOM_CORE_EXPORT ctkDICOMDatabasePrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMDatabase);
protected:
  ctkDICOMDatabase* const q_ptr;

public:
  ctkDICOMDatabasePrivate(ctkDICOMDatabase&);
  ~ctkDICOMDatabasePrivate();
  void init(QString databaseFile);
  void registerCompressionLibraries();
  bool executeScript(const QString script);

  /// Run a query and prints debug output of status
  bool loggedExec(QSqlQuery& query);
  bool loggedExec(QSqlQuery& query, const QString& queryString);
  bool loggedExecBatch(QSqlQuery& query);
  bool LoggedExecVerbose;

  bool removeImage(const QString& sopInstanceUID);

  /// Read DICOM tag value from file and store it in the tag cache
  QString readValueFromFile(const QString& fileName, const QString& sopInstanceUID, const QString& tag);

  /// Store copy of the dataset in database folder.
  /// If the original file is available then that will be inserted. If not then a file is created from the dataset object.
  bool storeDatasetFile(const ctkDICOMItem& dataset, const QString& originalFilePath,
    const QString& studyInstanceUID, const QString& seriesInstanceUID, const QString& sopInstanceUID, QString& storedFilePath);

  /// Helper function that generates folders for storing an instance in the database.
  /// Folders are based on UIDs, but may be shortened.
  QString internalStoragePath(const QString& studyInstanceUID,
    const QString& seriesInstanceUID, const QString& sopInstanceUID);

  /// Returns false in case of an error
  bool indexingStatusForFile(const QString& filePath, const QString& sopInstanceUID, bool& datasetInDatabase, bool& datasetUpToDate, QString& databaseFilename);

  /// Retrieve thumbnail from file and store in database folder.
  bool storeThumbnailFile(const QString& originalFilePath,
    const QString& studyInstanceUID, const QString& seriesInstanceUID, const QString& sopInstanceUID);

  /// Get basic UIDs for a data set, return true if the data set has all the required tags
  bool uidsForDataSet(const ctkDICOMItem& dataset, QString& patientsName, QString& patientID, QString& studyInstanceUID, QString& seriesInstanceUID);
  bool uidsForDataSet(QString& patientsName, QString& patientID, QString& studyInstanceUID);

  /// Dataset must be set always
  /// \param filePath It has to be set if this is an import of an actual file
  void insert ( const ctkDICOMItem& dataset, const QString& filePath, bool storeFile = true, bool generateThumbnail = true);

  /// Copy the complete list of files to an extra table
  QStringList allFilesInDatabase();

  /// Update database tables from the displayed fields determined by the plugin rules
  /// \return Success flag
  bool applyDisplayedFieldsChanges( QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries,
                                    QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy,
                                    QMap<QString, QMap<QString, QString> > &displayedFieldsMapPatient );

  /// Find patient by composite patient ID and return its index and insert it in the given fields map
  /// \param displayedFieldsMapPatient Map of patient field maps (name, value pairs) to which the found patient
  ///   is inserted on success. Also contains the generated patient index
  /// \return The composite patient ID if successfully found, empty string otherwise
  QString getDisplayPatientFieldsKey(const QString& patientID, const QString& patientsName, const QString& patientsBirthDate,
    QMap<QString, QMap<QString, QString> >& displayedFieldsMapPatient);

  /// Find study by instance UID and insert it in the given fields map
  /// \param displayedFieldsMapStudy Map of study field maps (name, value pairs) to which the found study has been inserted on success
  /// \return The study instance UID if successfully found, empty string otherwise
  QString getDisplayStudyFieldsKey(QString studyInstanceUID, QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy);

  /// Find series by instance UID and insert it in the given fields map
  /// \param displayedFieldsMapSeries Map of series field maps (name, value pairs) to which the found series has been inserted on success
  /// \return The series instance UID if successfully found, empty string otherwise
  QString getDisplaySeriesFieldsKey(QString seriesInstanceUID, QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries);

  /// Get all Filename values from table
  QStringList filenames(QString table);

  int rowCount(const QString& tableName);

  /// Convert an internal path (absolute or relative to database folder) to an absolute path.
  QString absolutePathFromInternal(const QString& filename);
  /// Convert an absolute path to an internal path (absolute if outside database folder, relative if inside database folder).
  QString internalPathFromAbsolute(const QString& filename);

  /// Name of the database file (i.e. for SQLITE the sqlite file)
  QString DatabaseFileName;

  /// Name of the database folder (empty if in-memory database).
  /// Cached because it needs to be accessed each time a filename is converted
  /// between absolute and relative path.
  QString DatabaseDirectory;

  QString LastError;
  QSqlDatabase Database;
  QMap<QString, QString> LoadedHeader;
  bool DisplayedFieldsTableAvailable;

  bool UseShortStoragePath;

  ctkDICOMAbstractThumbnailGenerator* ThumbnailGenerator;

  ctkDICOMDisplayedFieldGenerator* DisplayedFieldGenerator;

  /// These are for optimizing the import of image sequences
  /// since most information are identical for all slices.
  /// It would be very expensive to check in the database
  /// presence of all these records on each slice insertion,
  /// therefore we cache recently added entries in memory.
  QMap<QString, int> InsertedPatientsCompositeIDCache; // map from composite patient ID to database ID
  QSet<QString> InsertedStudyUIDsCache;
  QSet<QString> InsertedSeriesUIDsCache;

  /// resets the variables to new inserts won't be fooled by leftover values
  void resetLastInsertedValues();

  /// tagCache table has been checked to exist
  bool TagCacheVerified;
  /// tag cache has independent database to avoid locking issue
  /// with other access to the database which need to be
  /// reading while the tag cache is writing
  QSqlDatabase TagCacheDatabase;
  QString TagCacheDatabaseFilename;
  QStringList TagsToPrecache;
  QStringList TagsToExcludeFromStorage;
  bool openTagCacheDatabase();
  void precacheTags(const ctkDICOMItem& dataset, const QString sopInstanceUID);

  // Return true if a new item is inserted
  bool insertPatientStudySeries(const ctkDICOMItem& dataset, const QString& patientID, const QString& patientsName);
  bool insertPatient(const ctkDICOMItem& dataset, int& databasePatientID);
  bool insertStudy(const ctkDICOMItem& dataset, int dbPatientID);
  bool insertSeries( const ctkDICOMItem& dataset, QString studyInstanceUID);

  /// Facilitate using custom schema with the database without subclassing
  QString SchemaVersion;
};

#endif
