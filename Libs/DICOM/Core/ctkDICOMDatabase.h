/*=========================================================================

  Library:   CTK

  Copyright (c) 2010

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

#ifndef __ctkDICOMDatabase_h
#define __ctkDICOMDatabase_h

// Qt includes
#include <QObject>
#include <QStringList>
#include <QSqlDatabase>

#include "ctkDICOMItem.h"
#include "ctkDICOMCoreExport.h"

class QDateTime;
class ctkDICOMDatabasePrivate;
class DcmDataset;
class ctkDICOMAbstractThumbnailGenerator;
class ctkDICOMDisplayedFieldGenerator;

/// \ingroup DICOM_Core
///
/// Class handling a database of DICOM objects. So far, an underlying
/// SQLITE database is used for that. Usually, added DICOM objects are also
/// stored within the file system.
/// The SQLITE database file can be specified by the user. SQLITE (and this
/// class) also support a special in memory mode, where no database file is created
/// but the database is completely kept in memory (and after exiting the program,
/// vanishes). If in "memory mode", the objects are not written to disk,
/// otherwise they are stored in a subdirectory of the SQLITE database file
/// directory called "dicom". Inside, a folder structure created which contains
/// a directoy for each study, containing a directory for each series, containing
/// a file for each object. The corresponding UIDs are used as filenames.
/// Thumbnais for each image can be created; if so, they are stored in a directory
/// parallel to "dicom" directory called "thumbs".
class CTK_DICOM_CORE_EXPORT ctkDICOMDatabase : public QObject
{

  Q_OBJECT
  Q_PROPERTY(bool isOpen READ isOpen)
  Q_PROPERTY(bool isInMemory READ isInMemory)
  Q_PROPERTY(QString lastError READ lastError)
  Q_PROPERTY(QString databaseFilename READ databaseFilename)
  Q_PROPERTY(QString databaseDirectory READ databaseDirectory)
  Q_PROPERTY(QStringList tagsToPrecache READ tagsToPrecache WRITE setTagsToPrecache)
  Q_PROPERTY(QStringList tagsToExcludeFromStorage READ tagsToExcludeFromStorage WRITE setTagsToExcludeFromStorage)
  Q_PROPERTY(QStringList patientFieldNames READ patientFieldNames)
  Q_PROPERTY(QStringList studyFieldNames READ studyFieldNames)
  Q_PROPERTY(QStringList seriesFieldNames READ seriesFieldNames)
  Q_PROPERTY(bool useShortStoragePath READ useShortStoragePath WRITE setUseShortStoragePath)

public:
  struct IndexingResult
  {
    QString filePath;
    QSharedPointer<ctkDICOMItem> dataset;
    bool copyFile;
    bool overwriteExistingDataset;
  };

  explicit ctkDICOMDatabase(QObject *parent = 0);
  explicit ctkDICOMDatabase(QString databaseFile);
  virtual ~ctkDICOMDatabase();

  const QSqlDatabase& database() const;
  const QString lastError() const;
  const QString databaseFilename() const;

  static const char* defaultSchemaFile() { return ":/dicom/dicom-schema.sql"; };

  /// Return the absolute path of the database directory
  /// (where the database file resides in) in OS-preferred path format.
  /// @return Absolute path to database directory
  const QString databaseDirectory() const;

  /// Should be checked after trying to open the database
  /// @Returns true if database is open
  bool isOpen() const;

  /// Return whether the database only resides in memory, i.e. the
  /// SQLITE DB is not written to stored to disk and DICOM objects are not
  /// stored to the file system.
  /// @return True if in memory mode, false otherwise.
  bool isInMemory() const;

  /// Set thumbnail generator object
  Q_INVOKABLE void setThumbnailGenerator(ctkDICOMAbstractThumbnailGenerator* generator);
  /// Get thumbnail generator object
  Q_INVOKABLE ctkDICOMAbstractThumbnailGenerator* thumbnailGenerator();

  /// Open the SQLite database in @param databaseFile . If the file does not
  /// exist, a new database is created and initialized with the
  /// default schema
  ///
  /// @param databaseFile The file to store the SQLITE database should be
  ///        stored to. If specified with ":memory:", the database is not
  ///        written to disk at all but instead only kept in memory (and
  ///        thus expires after destruction of this object).
  /// @param connectionName The database connection name. If not specified
  ///        then a random name is generated (reusing a connection name
  ///        must be avoided as it breaks previously created database object
  ///        that used the same connection name).
  /// @param update the schema if it is found to be out of date
  Q_INVOKABLE virtual void openDatabase(const QString databaseFile,
                                        const QString& connectionName = "");

  /// Close the database. It must not be used afterwards.
  Q_INVOKABLE void closeDatabase();

  /// Delete all data and (re-)initialize the database.
  Q_INVOKABLE bool initializeDatabase(const char* schemaFile = ctkDICOMDatabase::defaultSchemaFile());

  /// Update the database schema and reinserts all existing files
  /// \param schemaFile SQL file containing schema definition
  /// \param newDatabaseDir Path of new database directory for the updated database.
  ///        Null by default, meaning directory will remain the same
  ///        Note: Need to switch database folders "on the fly", so that copying the
  ///        database can be done simply via createBackupFileList and the following insertions
  /// \return true if schema was updated
  Q_INVOKABLE bool updateSchema(
    const char* schemaFile = ctkDICOMDatabase::defaultSchemaFile(),
    const char* newDatabaseDir = nullptr);

  /// Update the database schema only if the versions don't match
  /// \param schemaFile SQL file containing schema definition
  /// \param newDatabaseDir Path of new database directory for the updated database.
  ///        Null by default, meaning directory will remain the same
  ///        Note: Need to switch database folders "on the fly", so that copying the
  ///        database can be done simply via createBackupFileList and the following insertions
  /// \return true if schema was updated
  Q_INVOKABLE bool updateSchemaIfNeeded(
    const char* schemaFile = ctkDICOMDatabase::defaultSchemaFile(),
    const char* newDatabaseDir = nullptr);

  /// Return the schema version needed by the current version of this code
  Q_INVOKABLE QString schemaVersion();

  /// Return the schema version for the currently open database
  /// in order to support schema updating
  Q_INVOKABLE QString schemaVersionLoaded();

  /// Set schema version externally in case a non-standard schema is used
  Q_INVOKABLE void setSchemaVersion(QString schemaVersion);

  /// \brief database accessors
  Q_INVOKABLE QStringList patients();
  Q_INVOKABLE QStringList studiesForPatient(const QString patientUID);
  Q_INVOKABLE QStringList seriesForStudy(const QString studyUID);
  /// Since a series may consists of many hundreds of instances, this method may be slow.
  /// If hits > 0 is specified then returned instances will be limited to that number.
  /// This is useful for retrieving first file, for example for getting access to fields within that file
  /// using instanceValue() method.
  Q_INVOKABLE QStringList instancesForSeries(const QString seriesUID, int hits = -1);
  Q_INVOKABLE QString studyForSeries(QString seriesUID);
  Q_INVOKABLE QString patientForStudy(QString studyUID);
  /// Since a series may consists of many hundreds of files, this method may be slow.
  /// If hits > 0 is specified then returned filenames will be limited to that number.
  /// This is useful for retrieving first file, for example for getting access to fields within that file
  /// using fileValue() method.
  Q_INVOKABLE QStringList filesForSeries(const QString seriesUID, int hits=-1);

  Q_INVOKABLE QHash<QString,QString> descriptionsForFile(QString fileName);
  Q_INVOKABLE QString descriptionForSeries(const QString seriesUID);
  Q_INVOKABLE QString descriptionForStudy(const QString studyUID);
  Q_INVOKABLE QString nameForPatient(const QString patientUID);
  Q_INVOKABLE QString displayedNameForPatient(const QString patientUID);
  Q_INVOKABLE QString fieldForPatient(const QString field, const QString patientUID);
  Q_INVOKABLE QString fieldForStudy(const QString field, const QString studyInstanceUID);
  Q_INVOKABLE QString fieldForSeries(const QString field, const QString seriesInstanceUID);

  QStringList patientFieldNames() const;
  QStringList studyFieldNames() const;
  QStringList seriesFieldNames() const;

  Q_INVOKABLE QString fileForInstance(const QString sopInstanceUID);
  Q_INVOKABLE QString seriesForFile(QString fileName);
  Q_INVOKABLE QString instanceForFile(const QString fileName);
  Q_INVOKABLE QDateTime insertDateTimeForInstance(const QString fileName);

  Q_INVOKABLE int patientsCount();
  Q_INVOKABLE int studiesCount();
  Q_INVOKABLE int seriesCount();
  Q_INVOKABLE int imagesCount();

  Q_INVOKABLE QStringList allFiles();

  bool allFilesModifiedTimes(QMap<QString, QDateTime>& modifiedTimeForFilepath);

  /// \brief Load the header from a file and allow access to elements
  /// @param sopInstanceUID A string with the uid for a given instance
  ///                       (corresponding file will be found via the database)
  /// @param fileName Full path to a dicom file to load.
  /// @param key A group,element tag in zero-filled hex
  Q_INVOKABLE void loadInstanceHeader(const QString sopInstanceUID);
  Q_INVOKABLE void loadFileHeader(const QString fileName);
  Q_INVOKABLE QStringList headerKeys();
  Q_INVOKABLE QString headerValue(const QString key);

  /// \brief Application-defined tags of interest
  /// This list of tags is added to the internal tag cache during import
  /// operations.  The list should be prepared by the application as
  /// a hint to the database that these tags are likely to be accessed
  /// later.  Internally, the database will cache the values of these
  /// tags so that subsequent calls to fileValue or instanceValue will
  /// be able to use the cache rather than re-reading the file.
  /// @param tags should be a list of ascii hex group/element tags
  ///  like "0008,0008" as in the instanceValue and fileValue calls
  void setTagsToPrecache(const QStringList tags);
  const QStringList tagsToPrecache();

  /// \brief Tags that must not be stored in the tag cache.
  /// Tag may be excluded from storage if it is not suitable for storage in the database (e.g., binary data)
  /// or if content of this field is usually very large.
  /// Presence of non-empty tag can still be checked using instanceValueExists or fileValueExists.
  /// By default, only PixelData tag is excluded from storage.
  void setTagsToExcludeFromStorage(const QStringList tags);
  const QStringList tagsToExcludeFromStorage();

  /// Insert into the database if not already existing.
  /// @param dataset The dataset to store into the database. Usually, this is
  ///                is a complete DICOM object, like a complete image. However
  ///                the database also inserts partial objects, like study
  ///                information to the database, even if no image data is
  ///                contained. This can be helpful to store results from
  ///                querying the PACS for patient/study/series or image
  ///                information, where a full hierarchy is only constructed
  ///                after some queries.
  /// @param storeFile If store file is set (default), then the dataset will
  ///                  be stored to disk. Note that in case of a memory-only
  ///                  database, this flag is ignored. Usually, this flag
  ///                  does only make sense if a full object is received.
  /// @param @generateThumbnail If true, a thumbnail is generated.
  ///
  Q_INVOKABLE void insert( const ctkDICOMItem& ctkDataset,
                              bool storeFile, bool generateThumbnail);
  void insert ( DcmItem *item,
                              bool storeFile = true, bool generateThumbnail = true);
  Q_INVOKABLE void insert ( const QString& filePath,
                            bool storeFile = true, bool generateThumbnail = true,
                            bool createHierarchy = true,
                            const QString& destinationDirectoryName = QString() );

  Q_INVOKABLE void insert(const QString& filePath, const ctkDICOMItem& ctkDataset,
    bool storeFile = true, bool generateThumbnail = true);

  Q_INVOKABLE void insert(const QList<ctkDICOMDatabase::IndexingResult>& indexingResults);

  /// When a DICOM file is stored in the database (insert is called with storeFile=true) then
  /// path is constructed from study, series, and SOP instance UID.
  /// If useShortStoragePath is false then the full UIDs are used as subfolder and file name.
  /// If useShortStoragePath is true (this is the default) then the path is shortened 
  /// to approximately 40 characters, by replacing UIDs with hashes generated from them.
  /// UIDs can be 40-60 characters long each, therefore the the total path (including database folder base path)
  /// can exceed maximum path length on some file systems. It is recommended to enable useShortStoragePath
  /// for better compatibility, unless it can be guaranteed that the file system can store full UIDs.
  void setUseShortStoragePath(bool useShort);
  bool useShortStoragePath()const;

  /// Update the fields in the database that are used for displaying information
  /// from information stored in the tag-cache.
  /// Displayed fields are useful if the raw DICOM tags are not human readable, or
  /// when we want to show a derived piece of information (such as image size or
  /// number of studies in a patient).
  Q_INVOKABLE virtual void updateDisplayedFields();

  /// Get if displayed fields are defined. It returns false for databases that were created with an old schema
  /// that did not contain ColumnDisplayProperties table.
  Q_INVOKABLE bool isDisplayedFieldsTableAvailable() const;
  /// Get displayed field generator in order to be able to set new rules externally
  ctkDICOMDisplayedFieldGenerator* displayedFieldGenerator() const;

  /// Reset cached item IDs to make sure previous
  /// inserts do not interfere with upcoming insert operations.
  /// Typically, it should be call just before a batch of files
  /// insertion is started.
  ///
  /// This has to be called before an insert() call if there is a chance
  /// that items have been deleted from the database since the
  /// the last insert() call. If there has been not been any insert() calls since
  /// connected to the database, then it should be called before the first
  /// insert().
  Q_INVOKABLE void prepareInsert();

  /// Check if file is already in database and up-to-date
  Q_INVOKABLE bool fileExistsAndUpToDate(const QString& filePath);

  /// Remove the series from the database, including images and thumbnails
  /// If clearCachedTags is set to true then cached tags associated with the series are deleted,
  /// if set to False the they are left in the database unchanged.
  /// By default clearCachedTags is disabled because it significantly increases deletion time
  /// on large databases.
  Q_INVOKABLE bool removeSeries(const QString& seriesInstanceUID, bool clearCachedTags=false);
  Q_INVOKABLE bool removeStudy(const QString& studyInstanceUID);
  Q_INVOKABLE bool removePatient(const QString& patientID);
  /// Remove all patients, studies, series, which do not have associated images.
  /// If vacuum is set to true then the whole database content is attempted to
  /// cleaned from remnants of all previously deleted data from the file.
  /// Vacuuming may fail if there are multiple connections to the database.
  Q_INVOKABLE bool cleanup(bool vacuum=false);

  /// \brief Access element values for given instance
  /// @param sopInstanceUID A string with the uid for a given instance
  ///                       (corresponding file will be found via the database)
  /// @param fileName Full path to a dicom file to load.
  /// @param group The group portion of the tag as an integer
  /// @param element The element portion of the tag as an integer
  /// @Returns empty string if element is missing or excluded from storage.
  Q_INVOKABLE QString instanceValue (const QString sopInstanceUID, const QString tag);
  Q_INVOKABLE QString instanceValue (const QString sopInstanceUID, const unsigned short group, const unsigned short element);
  Q_INVOKABLE QString fileValue (const QString fileName, const QString tag);
  Q_INVOKABLE QString fileValue (const QString fileName, const unsigned short group, const unsigned short element);
  Q_INVOKABLE bool tagToGroupElement (const QString tag, unsigned short& group, unsigned short& element);
  Q_INVOKABLE QString groupElementToTag (const unsigned short& group, const unsigned short& element);

  /// \brief Check if an element with the given attribute tag exists in the dataset and has a non-empty value.
  /// @param sopInstanceUID A string with the uid for a given instance
  ///                       (corresponding file will be found via the database)
  /// @param fileName Full path to a dicom file to load.
  /// @param group The group portion of the tag as an integer
  /// @param element The element portion of the tag as an integer
  /// @Returns true if tag exists and has non-empty value. Returns true even if the value is excluded from storage in the database.
  Q_INVOKABLE bool instanceValueExists(const QString sopInstanceUID, const QString tag);
  Q_INVOKABLE bool instanceValueExists(const QString sopInstanceUID, const unsigned short group, const unsigned short element);
  Q_INVOKABLE bool fileValueExists(const QString fileName, const QString tag);
  Q_INVOKABLE bool fileValueExists(const QString fileName, const unsigned short group, const unsigned short element);

  /// \brief Store values of previously requested instance elements
  /// These are meant to be internal methods used by the instanceValue and fileValue
  /// methods, but they can be used by calling classes to populate or access
  /// instance tag values as needed.
  /// @param sopInstanceUID A string with the uid for a given instance
  ///                       (corresponding file will be found via the database)
  /// @param key A group,element tag in zero-filled hex
  /// @Returns empty string if element for uid is missing from cache
  ///
  /// Lightweight check of tag cache existence (once db check per runtime)
  Q_INVOKABLE bool tagCacheExists ();
  /// Create a tagCache in the current database.  Delete the existing one if it exists.
  Q_INVOKABLE bool initializeTagCache ();
  /// Return the value of a cached tag
  Q_INVOKABLE QString cachedTag (const QString sopInstanceUID, const QString tag);
  /// Return the list of all cached tags and values for the specified sopInstanceUID. Returns with empty string if the tag is not present in the cache.
  Q_INVOKABLE void getCachedTags(const QString sopInstanceUID, QMap<QString, QString> &cachedTags);
  /// Insert an instance tag's value into to the cache
  Q_INVOKABLE bool cacheTag (const QString sopInstanceUID, const QString tag, const QString value);
  /// Insert lists of tags into the cache as a batch query operation
  Q_INVOKABLE bool cacheTags (const QStringList sopInstanceUIDs, const QStringList tags, const QStringList values);
  /// Remove all tags corresponding to a SOP instance UID
  void removeCachedTags(const QString sopInstanceUID);

  /// Get displayed name of a given field
  Q_INVOKABLE QString displayedNameForField(QString table, QString field) const;
  /// Set displayed name of a given field
  Q_INVOKABLE void setDisplayedNameForField(QString table, QString field, QString displayedName);
  /// Get visibility of a given field
  Q_INVOKABLE bool visibilityForField(QString table, QString field) const;
  /// Set visibility of a given field
  Q_INVOKABLE void setVisibilityForField(QString table, QString field, bool visibility);
  /// Get weight of a given field.
  /// Weight specifies the order of the field columns in the table. Smaller values are positioned towards the left ("heaviest sinks down")
  Q_INVOKABLE int weightForField(QString table, QString field) const;
  /// Set weight of a given field
  /// Weight specifies the order of the field columns in the table. Smaller values are positioned towards the left ("heaviest sinks down")
  Q_INVOKABLE void setWeightForField(QString table, QString field, int weight);
  /// Get format of a given field
  /// It contains a json document with the following fields:
  /// - resizeMode: column resize mode. Accepted values are: "interactive" (default), "stretch", or "resizeToContents".
  /// - sort: default sort order. Accepted values are: empty (default), "ascending" or "descending".
  ///   Only one column (or none) should have non-empty sort order in each table.
  Q_INVOKABLE QString formatForField(QString table, QString field) const;
  /// Set format of a given field
  Q_INVOKABLE void setFormatForField(QString table, QString field, QString format);

  /// There is no patient UID in DICOM, so we need to use use this composite ID to uniquely identify a patient with a string.
  /// Used when inserting a patient (InsertedPatientsCompositeIDCache) and in the display field update process.
  /// Note: It is not a problem that is somewhat more strict than the criteria that is used to decide if a study should be
  /// inserted under the same patient.
  Q_INVOKABLE static QString compositePatientID(const QString& patientID, const QString& patientsName, const QString& patientsBirthDate);

Q_SIGNALS:

  /// Things inserted to database.
  /// patientAdded arguments:
  ///  - int: database index of patient (unique) within CTK database
  ///  - QString: patient ID (not unique across institutions)
  ///  - QString: patient Name (not unique)
  ///  - QString: patient Birth Date (not unique)
  void patientAdded(int, QString, QString, QString);
  /// studyAdded arguments:
  ///  - studyUID (unique)
  void studyAdded(QString);
  /// seriesAdded arguments:
  ///  - seriesUID (unique)
  void seriesAdded(QString);
  /// instance UID is provided
  /// instanceAdded arguments:
  ///  - instanceUID (unique)
  void instanceAdded(QString);

  /// This signal is emitted when the database has been opened.
  void opened();

  /// This signal is emitted when the database has been closed.
  void closed();

  /// Indicate that an in-memory database has been updated
  void databaseChanged();

  /// Indicate that tagsToPrecache list changed
  void tagsToPrecacheChanged();

  /// Indicate that tagsToExcludeFromStorage list changed
  void tagsToExcludeFromStorageChanged();

  /// Indicate that the schema is about to be updated and how many files will be processed
  void schemaUpdateStarted(int);
  /// Indicate progress in updating schema (int is file number, string is file name)
  void schemaUpdateProgress(int);
  void schemaUpdateProgress(QString);
  /// Indicate schema update finished
  void schemaUpdated();

  /// Trigger showing progress dialog for displayed fields update
  void displayedFieldsUpdateStarted();
  /// Indicate progress in updating displayed fields (int is step number)
  void displayedFieldsUpdateProgress(int);
  /// Indicate displayed fields update finished
  void displayedFieldsUpdated();

protected:
  QScopedPointer<ctkDICOMDatabasePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMDatabase);
  Q_DISABLE_COPY(ctkDICOMDatabase);
};

#endif

