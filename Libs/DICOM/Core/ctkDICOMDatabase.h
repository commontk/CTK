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

public:
  explicit ctkDICOMDatabase(QObject *parent = 0);
  explicit ctkDICOMDatabase(QString databaseFile);
  virtual ~ctkDICOMDatabase();

  const QSqlDatabase& database() const;
  const QString lastError() const;
  const QString databaseFilename() const;

  ///
  /// Returns the absolute path of the database directory
  /// (where the database file resides in) in OS-prefered path format.
  /// @return Absolute path to database directory
  const QString databaseDirectory() const;

  ///
  /// Should be checked after trying to open the database
  /// @Returns true if database is open
  bool isOpen() const;

  ///
  /// Returns whether the database only resides in memory, i.e. the
  /// SQLITE DB is not written to stored to disk and DICOM objects are not
  /// stored to the file system.
  /// @return True if in memory mode, false otherwise.
  bool isInMemory() const;

  ///
  /// set thumbnail generator object
  Q_INVOKABLE void setThumbnailGenerator(ctkDICOMAbstractThumbnailGenerator* generator);
  ///
  /// get thumbnail genrator object
  Q_INVOKABLE ctkDICOMAbstractThumbnailGenerator* thumbnailGenerator();

  ///
  /// open the SQLite database in @param databaseFile . If the file does not
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

  ///
  /// close the database. It must not be used afterwards.
  Q_INVOKABLE void closeDatabase();
  ///
  /// delete all data and (re-)initialize the database.
  Q_INVOKABLE bool initializeDatabase(const char* schemaFile = ":/dicom/dicom-schema.sql");

  /// updates the database schema and reinserts all existing files
  Q_INVOKABLE bool updateSchema(const char* schemaFile = ":/dicom/dicom-schema.sql");

  /// updates the database schema only if the versions don't match
  /// Returns true if schema was updated
  Q_INVOKABLE bool updateSchemaIfNeeded(const char* schemaFile = ":/dicom/dicom-schema.sql");

  /// returns the schema version needed by the current version of this code
  Q_INVOKABLE QString schemaVersion();

  /// returns the schema version for the currently open database
  /// in order to support schema updating
  Q_INVOKABLE QString schemaVersionLoaded();

  ///
  /// \brief database accessors
  Q_INVOKABLE QStringList patients ();
  Q_INVOKABLE QStringList studiesForPatient (const QString patientUID);
  Q_INVOKABLE QStringList seriesForStudy (const QString studyUID);
  Q_INVOKABLE QStringList instancesForSeries(const QString seriesUID);
  Q_INVOKABLE QString studyForSeries(QString seriesUID);
  Q_INVOKABLE QString patientForStudy(QString studyUID);
  Q_INVOKABLE QStringList filesForSeries (const QString seriesUID);
  Q_INVOKABLE QHash<QString,QString> descriptionsForFile(QString fileName);
  Q_INVOKABLE QString descriptionForSeries(const QString seriesUID);
  Q_INVOKABLE QString descriptionForStudy(const QString studyUID);
  Q_INVOKABLE QString nameForPatient(const QString patientUID);
  Q_INVOKABLE QString fileForInstance (const QString sopInstanceUID);
  Q_INVOKABLE QString seriesForFile (QString fileName);
  Q_INVOKABLE QString instanceForFile (const QString fileName);
  Q_INVOKABLE QDateTime insertDateTimeForInstance (const QString fileName);

  Q_INVOKABLE QStringList allFiles ();
  ///
  /// \brief load the header from a file and allow access to elements
  /// @param sopInstanceUID A string with the uid for a given instance
  ///                       (corresponding file will be found via database)
  /// @param fileName Full path to a dicom file to load.
  /// @param key A group,element tag in zero-filled hex
  Q_INVOKABLE void loadInstanceHeader (const QString sopInstanceUID);
  Q_INVOKABLE void loadFileHeader (const QString fileName);
  Q_INVOKABLE QStringList headerKeys ();
  Q_INVOKABLE QString headerValue (const QString key);

  ///
  /// \brief application-defined tags of interest
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

  /// Insert into the database if not already exsting.
  /// @param dataset The dataset to store into the database. Usually, this is
  ///                is a complete DICOM object, like a complete image. However
  ///                the database also inserts partial objects, like studyl
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

  /// remove the series from the database, including images and
  /// thumbnails
  Q_INVOKABLE bool removeSeries(const QString& seriesInstanceUID);
  Q_INVOKABLE bool removeStudy(const QString& studyInstanceUID);
  Q_INVOKABLE bool removePatient(const QString& patientID);
  Q_INVOKABLE bool cleanup();

  ///
  /// \brief access element values for given instance
  /// @param sopInstanceUID A string with the uid for a given instance
  ///                       (corresponding file will be found via database)
  /// @param fileName Full path to a dicom file to load.
  /// @param key A group,element tag in zero-filled hex
  /// @param group The group portion of the tag as an integer
  /// @param element The element portion of the tag as an integer
  /// @Returns empty string if element is missing
  Q_INVOKABLE QString instanceValue (const QString sopInstanceUID, const QString tag);
  Q_INVOKABLE QString instanceValue (const QString sopInstanceUID, const unsigned short group, const unsigned short element);
  Q_INVOKABLE QString fileValue (const QString fileName, const QString tag);
  Q_INVOKABLE QString fileValue (const QString fileName, const unsigned short group, const unsigned short element);
  Q_INVOKABLE bool tagToGroupElement (const QString tag, unsigned short& group, unsigned short& element);
  Q_INVOKABLE QString groupElementToTag (const unsigned short& group, const unsigned short& element);

  ///
  /// \brief store values of previously requested instance elements
  /// These are meant to be internal methods used by the instanceValue and fileValue
  /// methods, but they can be used by calling classes to populate or access
  /// instance tag values as needed.
  /// @param sopInstanceUID A string with the uid for a given instance
  ///                       (corresponding file will be found via database)
  /// @param key A group,element tag in zero-filled hex
  /// @Returns empty string if element for uid is missing from cache
  ///
  /// Lightweight check of tag cache existence (once db check per runtime)
  Q_INVOKABLE bool tagCacheExists ();
  /// Create a tagCache in the current database.  Delete the existing one if it exists.
  Q_INVOKABLE bool initializeTagCache ();
  /// Return the value of a cached tag
  Q_INVOKABLE QString cachedTag (const QString sopInstanceUID, const QString tag);
  /// Insert an instance tag's value into to the cache
  Q_INVOKABLE bool cacheTag (const QString sopInstanceUID, const QString tag, const QString value);
  /// Insert lists of tags into the cache as a batch query operation
  Q_INVOKABLE bool cacheTags (const QStringList sopInstanceUIDs, const QStringList tags, const QStringList values);


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
  /// Indicates that an in-memory database has been updated
  void databaseChanged();
  /// Indicates that the schema is about to be updated and how many files will be processed
  void schemaUpdateStarted(int);
  /// Indicates progress in updating schema (int is file number, string is file name)
  void schemaUpdateProgress(int);
  void schemaUpdateProgress(QString);
  /// Indicates schema update finished
  void schemaUpdated();

protected:
  QScopedPointer<ctkDICOMDatabasePrivate> d_ptr;



private:
  Q_DECLARE_PRIVATE(ctkDICOMDatabase);
  Q_DISABLE_COPY(ctkDICOMDatabase);
};

#endif

