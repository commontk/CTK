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

#include <stdexcept>

// Qt includes
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QUuid>
#include <QVariant>

// ctkDICOM includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMAbstractThumbnailGenerator.h"
#include "ctkDICOMItem.h"

#include "ctkLogger.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/ofstd.h>        /* for class OFStandard */
#include <dcmtk/dcmdata/dcddirif.h>   /* for class DicomDirInterface */
#include <dcmtk/dcmimgle/dcmimage.h>

#include <dcmtk/dcmjpeg/djdecode.h>  /* for dcmjpeg decoders */
#include <dcmtk/dcmjpeg/djencode.h>  /* for dcmjpeg encoders */
#include <dcmtk/dcmdata/dcrledrg.h>  /* for DcmRLEDecoderRegistration */
#include <dcmtk/dcmdata/dcrleerg.h>  /* for DcmRLEEncoderRegistration */

//------------------------------------------------------------------------------
static ctkLogger logger("org.commontk.dicom.DICOMDatabase" );
//------------------------------------------------------------------------------

// Flag for tag cache to avoid repeated serarches for
// tags that do no exist.
static QString TagNotInInstance("__TAG_NOT_IN_INSTANCE__");
// Flag for tag cache indicating that the value
// really is the empty string
static QString ValueIsEmptyString("__VALUE_IS_EMPTY_STRING__");

//------------------------------------------------------------------------------
class ctkDICOMDatabasePrivate
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
  ///
  /// \brief runs a query and prints debug output of status
  ///
  bool loggedExec(QSqlQuery& query);
  bool loggedExec(QSqlQuery& query, const QString& queryString);
  bool loggedExecBatch(QSqlQuery& query);
  bool LoggedExecVerbose;

  ///
  /// \brief group several inserts into a single transaction
  ///
  void beginTransaction();
  void endTransaction();

  // dataset must be set always
  // filePath has to be set if this is an import of an actual file
  void insert ( const ctkDICOMItem& ctkDataset, const QString& filePath, bool storeFile = true, bool generateThumbnail = true);

  ///
  /// copy the complete list of files to an extra table
  ///
  void createBackupFileList();

  ///
  /// remove the extra table containing the backup
  ///
  void removeBackupFileList();


  ///
  /// get all Filename values from table
  QStringList filenames(QString table);

  /// Name of the database file (i.e. for SQLITE the sqlite file)
  QString      DatabaseFileName;
  QString      LastError;
  QSqlDatabase Database;
  QMap<QString, QString> LoadedHeader;

  ctkDICOMAbstractThumbnailGenerator* thumbnailGenerator;

  /// these are for optimizing the import of image sequences
  /// since most information are identical for all slices
  QString LastPatientID;
  QString LastPatientsName;
  QString LastPatientsBirthDate;
  QString LastStudyInstanceUID;
  QString LastSeriesInstanceUID;
  int LastPatientUID;

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
  bool openTagCacheDatabase();
  void precacheTags( const QString sopInstanceUID );

  int insertPatient(const ctkDICOMItem& ctkDataset);
  void insertStudy(const ctkDICOMItem& ctkDataset, int dbPatientID);
  void insertSeries( const ctkDICOMItem& ctkDataset, QString studyInstanceUID);
};

//------------------------------------------------------------------------------
// ctkDICOMDatabasePrivate methods

//------------------------------------------------------------------------------
ctkDICOMDatabasePrivate::ctkDICOMDatabasePrivate(ctkDICOMDatabase& o): q_ptr(&o)
{
  this->thumbnailGenerator = NULL;
  this->LoggedExecVerbose = false;
  this->TagCacheVerified = false;
  this->resetLastInsertedValues();
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::resetLastInsertedValues()
{
  this->LastPatientID = QString("");
  this->LastPatientsName = QString("");
  this->LastPatientsBirthDate = QString("");
  this->LastStudyInstanceUID = QString("");
  this->LastSeriesInstanceUID = QString("");
  this->LastPatientUID = -1;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::init(QString databaseFilename)
{
  Q_Q(ctkDICOMDatabase);

  q->openDatabase(databaseFilename);

}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::registerCompressionLibraries(){
  // Register the JPEG libraries in case we need them
  //   (registration only happens once, so it's okay to call repeatedly)
  // register global JPEG decompression codecs
  DJDecoderRegistration::registerCodecs();
  // register global JPEG compression codecs
  DJEncoderRegistration::registerCodecs();
  // register RLE compression codec
  DcmRLEEncoderRegistration::registerCodecs();
  // register RLE decompression codec
  DcmRLEDecoderRegistration::registerCodecs();
}

//------------------------------------------------------------------------------
ctkDICOMDatabasePrivate::~ctkDICOMDatabasePrivate()
{
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::loggedExec(QSqlQuery& query)
{
  return (loggedExec(query, QString("")));
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::loggedExec(QSqlQuery& query, const QString& queryString)
{
  bool success;
  if (queryString.compare(""))
    {
      success = query.exec(queryString);
    }
  else
    {
      success = query.exec();
    }
  if (!success)
    {
      QSqlError sqlError = query.lastError();
      logger.debug( "SQL failed\n Bad SQL: " + query.lastQuery());
      logger.debug( "Error text: " + sqlError.text());
    }
  else
    {
      if (LoggedExecVerbose)
      {
      logger.debug( "SQL worked!\n SQL: " + query.lastQuery());
      }
    }
  return (success);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::loggedExecBatch(QSqlQuery& query)
{
  bool success;
  success = query.execBatch();
  if (!success)
    {
      QSqlError sqlError = query.lastError();
      logger.debug( "SQL failed\n Bad SQL: " + query.lastQuery());
      logger.debug( "Error text: " + sqlError.text());
    }
  else
    {
      if (LoggedExecVerbose)
      {
      logger.debug( "SQL worked!\n SQL: " + query.lastQuery());
      }
    }
  return (success);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::beginTransaction()
{
  QSqlQuery transaction( this->Database );
  transaction.prepare( "BEGIN TRANSACTION" );
  transaction.exec();
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::endTransaction()
{
  QSqlQuery transaction( this->Database );
  transaction.prepare( "END TRANSACTION" );
  transaction.exec();
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::createBackupFileList()
{
  QSqlQuery query(this->Database);
  loggedExec(query, "CREATE TABLE IF NOT EXISTS main.Filenames_backup (Filename TEXT PRIMARY KEY NOT NULL )" );
  loggedExec(query, "INSERT INTO Filenames_backup SELECT Filename FROM Images;" );
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::removeBackupFileList()
{
  QSqlQuery query(this->Database);
  loggedExec(query, "DROP TABLE main.Filenames_backup; " );
}



//------------------------------------------------------------------------------
void ctkDICOMDatabase::openDatabase(const QString databaseFile, const QString& connectionName )
{
  Q_D(ctkDICOMDatabase);
  d->DatabaseFileName = databaseFile;
  QString verifiedConnectionName = connectionName;
  if (verifiedConnectionName.isEmpty())
    {
    verifiedConnectionName = QUuid::createUuid().toString();
    }
  d->Database = QSqlDatabase::addDatabase("QSQLITE", verifiedConnectionName);
  d->Database.setDatabaseName(databaseFile);
  if ( ! (d->Database.open()) )
    {
      d->LastError = d->Database.lastError().text();
      return;
    }
  if ( d->Database.tables().empty() )
    {
      if (!initializeDatabase())
        {
          d->LastError = QString("Unable to initialize DICOM database!");
          return;
        }
    }
  d->resetLastInsertedValues();

  if (!isInMemory())
    {
      QFileSystemWatcher* watcher = new QFileSystemWatcher(QStringList(databaseFile),this);
      connect(watcher, SIGNAL(fileChanged(QString)),this, SIGNAL (databaseChanged()) );
    }

  //Disable synchronous writing to make modifications faster
  {
  QSqlQuery pragmaSyncQuery(d->Database);
  pragmaSyncQuery.exec("PRAGMA synchronous = OFF");
  pragmaSyncQuery.finish();
  }

  // set up the tag cache for use later
  QFileInfo fileInfo(d->DatabaseFileName);
  d->TagCacheDatabaseFilename = QString( fileInfo.dir().path() + "/ctkDICOMTagCache.sql" );
  d->TagCacheVerified = false;
  if ( !this->tagCacheExists() )
    {
    this->initializeTagCache();
    }
}

//------------------------------------------------------------------------------
// ctkDICOMDatabase methods

//------------------------------------------------------------------------------
ctkDICOMDatabase::ctkDICOMDatabase(QString databaseFile)
  : d_ptr(new ctkDICOMDatabasePrivate(*this))
{
  Q_D(ctkDICOMDatabase);
  d->registerCompressionLibraries();
  d->init(databaseFile);
}

ctkDICOMDatabase::ctkDICOMDatabase(QObject* parent)
  : d_ptr(new ctkDICOMDatabasePrivate(*this))
{
  Q_UNUSED(parent);
  Q_D(ctkDICOMDatabase);
  d->registerCompressionLibraries();
}

//------------------------------------------------------------------------------
ctkDICOMDatabase::~ctkDICOMDatabase()
{
}

//----------------------------------------------------------------------------

//------------------------------------------------------------------------------
const QString ctkDICOMDatabase::lastError() const {
  Q_D(const ctkDICOMDatabase);
  return d->LastError;
}

//------------------------------------------------------------------------------
const QString ctkDICOMDatabase::databaseFilename() const {
  Q_D(const ctkDICOMDatabase);
  return d->DatabaseFileName;
}

//------------------------------------------------------------------------------
const QString ctkDICOMDatabase::databaseDirectory() const {
  QString databaseFile = databaseFilename();
  if (!QFileInfo(databaseFile).isAbsolute())
    {
      databaseFile.prepend(QDir::currentPath() + "/");
    }
  return QFileInfo ( databaseFile ).absoluteDir().path();
}

//------------------------------------------------------------------------------
const QSqlDatabase& ctkDICOMDatabase::database() const {
  Q_D(const ctkDICOMDatabase);
  return d->Database;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setThumbnailGenerator(ctkDICOMAbstractThumbnailGenerator *generator){
  Q_D(ctkDICOMDatabase);
  d->thumbnailGenerator = generator;
}

//------------------------------------------------------------------------------
ctkDICOMAbstractThumbnailGenerator* ctkDICOMDatabase::thumbnailGenerator(){
  Q_D(const ctkDICOMDatabase);
  return d->thumbnailGenerator;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::executeScript(const QString script) {
  QFile scriptFile(script);
  scriptFile.open(QIODevice::ReadOnly);
  if  ( !scriptFile.isOpen() )
    {
      qDebug() << "Script file " << script << " could not be opened!\n";
      return false;
    }

  QString sqlCommands( QTextStream(&scriptFile).readAll() );
  sqlCommands.replace( '\n', ' ' );
  sqlCommands.remove( '\r' );
  sqlCommands.replace("; ", ";\n");

  QStringList sqlCommandsLines = sqlCommands.split('\n');

  QSqlQuery query(Database);

  for (QStringList::iterator it = sqlCommandsLines.begin(); it != sqlCommandsLines.end()-1; ++it)
    {
      if (! (*it).startsWith("--") )
        {
          if (LoggedExecVerbose)
            {
            qDebug() << *it << "\n";
            }
          query.exec(*it);
          if (query.lastError().type())
            {
              qDebug() << "There was an error during execution of the statement: " << (*it);
              qDebug() << "Error message: " << query.lastError().text();
              return false;
            }
        }
    }
  return true;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabasePrivate::filenames(QString table)
{
  /// get all filenames from the database
  QSqlQuery allFilesQuery(this->Database);
  QStringList allFileNames;
  loggedExec(allFilesQuery,QString("SELECT Filename from %1 ;").arg(table) );

  while (allFilesQuery.next())
  {
    allFileNames << allFilesQuery.value(0).toString();
  }
  return allFileNames;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::initializeDatabase(const char* sqlFileName)
{
  Q_D(ctkDICOMDatabase);

  d->resetLastInsertedValues();

  // remove any existing schema info - this handles the case where an
  // old schema should be loaded for testing.
  QSqlQuery dropSchemaInfo(d->Database);
  d->loggedExec( dropSchemaInfo, QString("DROP TABLE IF EXISTS 'SchemaInfo';") );
  return d->executeScript(sqlFileName);
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::schemaVersionLoaded()
{
  Q_D(ctkDICOMDatabase);
  /// look for the version info in the database
  QSqlQuery versionQuery(d->Database);
  if ( !d->loggedExec( versionQuery, QString("SELECT Version from SchemaInfo;") ) )
    {
    return QString("");
    }

  if (versionQuery.next())
    {
    return versionQuery.value(0).toString();
    }

  return QString("");
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::schemaVersion()
{
  // When changing schema version:
  // * make sure this matches the Version value in the
  //   SchemaInfo table defined in Resources/dicom-schema.sql
  // * make sure the 'Images' contains a 'Filename' column
  //   so that the ctkDICOMDatabasePrivate::filenames method
  //   still works.
  //
  return QString("0.5.3");
};

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::updateSchemaIfNeeded(const char* schemaFile)
{
  if ( schemaVersionLoaded() != schemaVersion() )
    {
    return this->updateSchema(schemaFile);
    }
  else
    {
    emit schemaUpdateStarted(0);
    emit schemaUpdated();
    return false;
    }
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::updateSchema(const char* schemaFile)
{
  // backup filelist
  // reinit with the new schema
  // reinsert everything

  Q_D(ctkDICOMDatabase);
  d->createBackupFileList();

  d->resetLastInsertedValues();
  this->initializeDatabase(schemaFile);

  QStringList allFiles = d->filenames("Filenames_backup");
  emit schemaUpdateStarted(allFiles.length());

  int progressValue = 0;
  foreach(QString file, allFiles)
  {
    emit schemaUpdateProgress(progressValue);
    emit schemaUpdateProgress(file);

    // TODO: use QFuture
    this->insert(file,false,false,true);

    progressValue++;
  }
  // TODO: check better that everything is ok
  d->removeBackupFileList();
  emit schemaUpdated();
  return true;

}


//------------------------------------------------------------------------------
void ctkDICOMDatabase::closeDatabase()
{
  Q_D(ctkDICOMDatabase);
  d->Database.close();
  d->TagCacheDatabase.close();
}

//
// Patient/study/series convenience methods
//

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::patients()
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT UID FROM Patients" );
  query.exec();
  QStringList result;
  while (query.next())
    {
      result << query.value(0).toString();
    }
  return( result );
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::studiesForPatient(QString dbPatientID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT StudyInstanceUID FROM Studies WHERE PatientsUID = ?" );
  query.bindValue ( 0, dbPatientID );
  query.exec();
  QStringList result;
  while (query.next())
    {
      result << query.value(0).toString();
    }
  return( result );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::studyForSeries(QString seriesUID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT StudyInstanceUID FROM Series WHERE SeriesInstanceUID= ?" );
  query.bindValue ( 0, seriesUID);
  query.exec();
  QString result;
  if (query.next())
    {
    result = query.value(0).toString();
    }
  return( result );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::patientForStudy(QString studyUID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT PatientsUID FROM Studies WHERE StudyInstanceUID= ?" );
  query.bindValue ( 0, studyUID);
  query.exec();
  QString result;
  if (query.next())
    {
    result = query.value(0).toString();
    }
  return( result );
}

//------------------------------------------------------------------------------
QHash<QString,QString> ctkDICOMDatabase::descriptionsForFile(QString fileName)
{
  Q_D(ctkDICOMDatabase);

  QString seriesUID(this->seriesForFile(fileName));
  QString studyUID(this->studyForSeries(seriesUID));
  QString patientID(this->patientForStudy(studyUID));

  QSqlQuery query(d->Database);
  query.prepare ( "SELECT SeriesDescription FROM Series WHERE SeriesInstanceUID= ?" );
  query.bindValue ( 0, seriesUID);
  query.exec();
  QHash<QString,QString> result;
  if (query.next())
  {
    result["SeriesDescription"] =  query.value(0).toString();
  }
  query.prepare ( "SELECT StudyDescription FROM Studies WHERE StudyInstanceUID= ?" );
  query.bindValue ( 0, studyUID);
  query.exec();
  if (query.next())
  {
    result["StudyDescription"] =  query.value(0).toString();
  }
  query.prepare ( "SELECT PatientsName FROM Patients WHERE UID= ?" );
  query.bindValue ( 0, patientID);
  query.exec();
  if (query.next())
  {
    result["PatientsName"] =  query.value(0).toString();
  }
  return( result );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::descriptionForSeries(const QString seriesUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;

  QSqlQuery query(d->Database);
  query.prepare ( "SELECT SeriesDescription FROM Series WHERE SeriesInstanceUID= ?" );
  query.bindValue ( 0, seriesUID);
  query.exec();
  if (query.next())
    {
    result = query.value(0).toString();
    }

  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::descriptionForStudy(const QString studyUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;

  QSqlQuery query(d->Database);
  query.prepare ( "SELECT StudyDescription FROM Studies WHERE StudyInstanceUID= ?" );
  query.bindValue ( 0, studyUID);
  query.exec();
  if (query.next())
    {
    result =  query.value(0).toString();
    }

  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::nameForPatient(const QString patientUID)
{
  Q_D(ctkDICOMDatabase);

  QString result;

  QSqlQuery query(d->Database);
  query.prepare ( "SELECT PatientsName FROM Patients WHERE UID= ?" );
  query.bindValue ( 0, patientUID);
  query.exec();
  if (query.next())
    {
    result =  query.value(0).toString();
    }

  return result;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::seriesForStudy(QString studyUID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT SeriesInstanceUID FROM Series WHERE StudyInstanceUID=?");
  query.bindValue ( 0, studyUID );
  query.exec();
  QStringList result;
  while (query.next())
    {
      result << query.value(0).toString();
    }
  return( result );
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::instancesForSeries(const QString seriesUID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare("SELECT SOPInstanceUID FROM Images WHERE SeriesInstanceUID= ?");
  query.bindValue(0, seriesUID);
  query.exec();
  QStringList result;
  while (query.next())
  {
    result << query.value(0).toString();
  }

  return result;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::filesForSeries(QString seriesUID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT Filename FROM Images WHERE SeriesInstanceUID=?");
  query.bindValue ( 0, seriesUID );
  query.exec();
  QStringList result;
  while (query.next())
    {
      result << query.value(0).toString();
    }
  return( result );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::fileForInstance(QString sopInstanceUID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT Filename FROM Images WHERE SOPInstanceUID=?");
  query.bindValue ( 0, sopInstanceUID );
  query.exec();
  QString result;
  if (query.next())
    {
    result = query.value(0).toString();
    }
  return( result );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::seriesForFile(QString fileName)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT SeriesInstanceUID FROM Images WHERE Filename=?");
  query.bindValue ( 0, fileName );
  query.exec();
  QString result;
  if (query.next())
    {
    result = query.value(0).toString();
    }
  return( result );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::instanceForFile(QString fileName)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT SOPInstanceUID FROM Images WHERE Filename=?");
  query.bindValue ( 0, fileName );
  query.exec();
  QString result;
  if (query.next())
    {
    result = query.value(0).toString();
    }
  return( result );
}

//------------------------------------------------------------------------------
QDateTime ctkDICOMDatabase::insertDateTimeForInstance(QString sopInstanceUID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT InsertTimestamp FROM Images WHERE SOPInstanceUID=?");
  query.bindValue ( 0, sopInstanceUID );
  query.exec();
  QDateTime result;
  if (query.next())
    {
    result = QDateTime::fromString(query.value(0).toString(), Qt::ISODate);
    }
  return( result );
}


//
// instance header methods
//

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::allFiles()
{
  Q_D(ctkDICOMDatabase);
  return d->filenames("Images");
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::loadInstanceHeader (QString sopInstanceUID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT Filename FROM Images WHERE SOPInstanceUID=?");
  query.bindValue ( 0, sopInstanceUID );
  query.exec();
  if (query.next())
    {
      QString fileName = query.value(0).toString();
      this->loadFileHeader(fileName);
    }
  return;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::loadFileHeader (QString fileName)
{
  Q_D(ctkDICOMDatabase);
  d->LoadedHeader.clear();
  DcmFileFormat fileFormat;
  OFCondition status = fileFormat.loadFile(fileName.toLatin1().data());
  if (status.good())
    {
      DcmDataset *dataset = fileFormat.getDataset();
      DcmStack stack;
      while (dataset->nextObject(stack, true) == EC_Normal)
        {
          DcmObject *dO = stack.top();
          if (dO)
            {
              QString tag = QString("%1,%2").arg(
                    dO->getGTag(),4,16,QLatin1Char('0')).arg(
                    dO->getETag(),4,16,QLatin1Char('0'));
              std::ostringstream s;
              dO->print(s);
              d->LoadedHeader[tag] = QString(s.str().c_str());
            }
        }
    }
  return;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDatabase::headerKeys ()
{
  Q_D(ctkDICOMDatabase);
  return (d->LoadedHeader.keys());
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::headerValue (QString key)
{
  Q_D(ctkDICOMDatabase);
  return (d->LoadedHeader[key]);
}

//
// instanceValue and fileValue methods
//

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::instanceValue(QString sopInstanceUID, QString tag)
{
  QString value = this->cachedTag(sopInstanceUID, tag);
  if (value == TagNotInInstance || value == ValueIsEmptyString)
    {
    return "";
    }
  if (value != "")
    {
    return value;
    }
  unsigned short group, element;
  this->tagToGroupElement(tag, group, element);
  return( this->instanceValue(sopInstanceUID, group, element) );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::instanceValue(const QString sopInstanceUID, const unsigned short group, const unsigned short element)
{
  QString tag = this->groupElementToTag(group,element);
  QString value = this->cachedTag(sopInstanceUID, tag);
  if (value == TagNotInInstance || value == ValueIsEmptyString)
    {
    return "";
    }
  if (value != "")
    {
    return value;
    }
  QString filePath = this->fileForInstance(sopInstanceUID);
  if (filePath != "" )
    {
    value = this->fileValue(filePath, group, element);
    return( value );
    }
  else
    {
    return ("");
    }
}


//------------------------------------------------------------------------------
QString ctkDICOMDatabase::fileValue(const QString fileName, QString tag)
{
  unsigned short group, element;
  this->tagToGroupElement(tag, group, element);
  QString sopInstanceUID = this->instanceForFile(fileName);
  QString value = this->cachedTag(sopInstanceUID, tag);
  if (value == TagNotInInstance || value == ValueIsEmptyString)
    {
    return "";
    }
  if (value != "")
    {
    return value;
    }
  return( this->fileValue(fileName, group, element) );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::fileValue(const QString fileName, const unsigned short group, const unsigned short element)
{
  // here is where the real lookup happens
  // - first we check the tagCache to see if the value exists for this instance tag
  // If not,
  // - for now we create a ctkDICOMItem and extract the value from there
  // - then we convert to the appropriate type of string
  //
  //As an optimization we could consider
  // - check if we are currently looking at the dataset for this fileName
  // - if so, are we looking for a group/element that is past the last one
  //   accessed
  //   -- if so, keep looking for the requested group/element
  //   -- if not, start again from the begining

  QString tag = this->groupElementToTag(group, element);
  QString sopInstanceUID = this->instanceForFile(fileName);
  QString value = this->cachedTag(sopInstanceUID, tag);
  if (value == TagNotInInstance || value == ValueIsEmptyString)
    {
    return "";
    }
  if (value != "")
    {
    return value;
    }

  ctkDICOMItem dataset;
  dataset.InitializeFromFile(fileName);
  if (!dataset.IsInitialized())
    {
    logger.error( "File " + fileName + " could not be initialized.");
    return "";
    }

  DcmTagKey tagKey(group, element);
  value = dataset.GetAllElementValuesAsString(tagKey);
  this->cacheTag(sopInstanceUID, tag, value);
  return value;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::tagToGroupElement(const QString tag, unsigned short& group, unsigned short& element)
{
  QStringList groupElement = tag.split(",");
  bool groupOK, elementOK;
  if (groupElement.length() != 2)
    {
    return false;
    }
  group = groupElement[0].toUInt(&groupOK, 16);
  element = groupElement[1].toUInt(&elementOK, 16);

  return( groupOK && elementOK );
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::groupElementToTag(const unsigned short& group, const unsigned short& element)
{
  return QString("%1,%2").arg(group,4,16,QLatin1Char('0')).arg(element,4,16,QLatin1Char('0'));
}

//
// methods related to insert
//

//------------------------------------------------------------------------------
void ctkDICOMDatabase::prepareInsert()
{
  Q_D(ctkDICOMDatabase);
  // Although resetLastInsertedValues is called when items are deleted through
  // this database connection, there may be concurrent database modifications
  // (even in the same application) through other connections.
  // Therefore, we cannot be sure that the last added patient, study, series,
  // items are still in the database. We clear cached Last... IDs to make sure
  // the patient, study, series items are created.
  d->resetLastInsertedValues();
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert( DcmItem *item, bool storeFile, bool generateThumbnail)
{
  if (!item)
    {
      return;
    }
  ctkDICOMItem ctkDataset;
  ctkDataset.InitializeFromItem(item, false /* do not take ownership */);
  this->insert(ctkDataset,storeFile,generateThumbnail);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert( const ctkDICOMItem& ctkDataset, bool storeFile, bool generateThumbnail)
{
  Q_D(ctkDICOMDatabase);
  d->insert(ctkDataset, QString(), storeFile, generateThumbnail);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert ( const QString& filePath, bool storeFile, bool generateThumbnail, bool createHierarchy, const QString& destinationDirectoryName)
{
  Q_D(ctkDICOMDatabase);
  Q_UNUSED(createHierarchy);
  Q_UNUSED(destinationDirectoryName);

  /// first we check if the file is already in the database
  if (fileExistsAndUpToDate(filePath))
    {
      logger.debug( "File " + filePath + " already added.");
      return;
    }

  if (d->LoggedExecVerbose)
    {
    logger.debug( "Processing " + filePath );
    }

  ctkDICOMItem ctkDataset;

  ctkDataset.InitializeFromFile(filePath);
  if ( ctkDataset.IsInitialized() )
    {
      d->insert( ctkDataset, filePath, storeFile, generateThumbnail );
    }
  else
    {
      logger.warn(QString("Could not read DICOM file:") + filePath);
    }
}

//------------------------------------------------------------------------------
int ctkDICOMDatabasePrivate::insertPatient(const ctkDICOMItem& ctkDataset)
{
  int dbPatientID;

  // Check if patient is already present in the db
  // TODO: maybe add birthdate check for extra safety
  QString patientID(ctkDataset.GetElementAsString(DCM_PatientID) );
  QString patientsName(ctkDataset.GetElementAsString(DCM_PatientName) );
  QString patientsBirthDate(ctkDataset.GetElementAsString(DCM_PatientBirthDate) );

  QSqlQuery checkPatientExistsQuery(Database);
  checkPatientExistsQuery.prepare ( "SELECT * FROM Patients WHERE PatientID = ? AND PatientsName = ?" );
  checkPatientExistsQuery.bindValue ( 0, patientID );
  checkPatientExistsQuery.bindValue ( 1, patientsName );
  loggedExec(checkPatientExistsQuery);

  if (checkPatientExistsQuery.next())
    {
      // we found him
      dbPatientID = checkPatientExistsQuery.value(checkPatientExistsQuery.record().indexOf("UID")).toInt();
      qDebug() << "Found patient in the database as UId: " << dbPatientID;
    }
  else
    {
      // Insert it

      QString patientsBirthTime(ctkDataset.GetElementAsString(DCM_PatientBirthTime) );
      QString patientsSex(ctkDataset.GetElementAsString(DCM_PatientSex) );
      QString patientsAge(ctkDataset.GetElementAsString(DCM_PatientAge) );
      QString patientComments(ctkDataset.GetElementAsString(DCM_PatientComments) );

      QSqlQuery insertPatientStatement ( Database );
      insertPatientStatement.prepare ( "INSERT INTO Patients ('UID', 'PatientsName', 'PatientID', 'PatientsBirthDate', 'PatientsBirthTime', 'PatientsSex', 'PatientsAge', 'PatientsComments' ) values ( NULL, ?, ?, ?, ?, ?, ?, ? )" );
      insertPatientStatement.bindValue ( 0, patientsName );
      insertPatientStatement.bindValue ( 1, patientID );
      insertPatientStatement.bindValue ( 2, QDate::fromString ( patientsBirthDate, "yyyyMMdd" ) );
      insertPatientStatement.bindValue ( 3, patientsBirthTime );
      insertPatientStatement.bindValue ( 4, patientsSex );
      // TODO: shift patient's age to study,
      // since this is not a patient level attribute in images
      // insertPatientStatement.bindValue ( 5, patientsAge );
      insertPatientStatement.bindValue ( 6, patientComments );
      loggedExec(insertPatientStatement);
      dbPatientID = insertPatientStatement.lastInsertId().toInt();
      logger.debug ( "New patient inserted: " + QString().setNum ( dbPatientID ) );
      qDebug() << "New patient inserted as : " << dbPatientID;
    }
    return dbPatientID;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::insertStudy(const ctkDICOMItem& ctkDataset, int dbPatientID)
{
  QString studyInstanceUID(ctkDataset.GetElementAsString(DCM_StudyInstanceUID) );
  QSqlQuery checkStudyExistsQuery (Database);
  checkStudyExistsQuery.prepare ( "SELECT * FROM Studies WHERE StudyInstanceUID = ?" );
  checkStudyExistsQuery.bindValue ( 0, studyInstanceUID );
  checkStudyExistsQuery.exec();
  if(!checkStudyExistsQuery.next())
    {
      qDebug() << "Need to insert new study: " << studyInstanceUID;

      QString studyID(ctkDataset.GetElementAsString(DCM_StudyID) );
      QString studyDate(ctkDataset.GetElementAsString(DCM_StudyDate) );
      QString studyTime(ctkDataset.GetElementAsString(DCM_StudyTime) );
      QString accessionNumber(ctkDataset.GetElementAsString(DCM_AccessionNumber) );
      QString modalitiesInStudy(ctkDataset.GetElementAsString(DCM_ModalitiesInStudy) );
      QString institutionName(ctkDataset.GetElementAsString(DCM_InstitutionName) );
      QString performingPhysiciansName(ctkDataset.GetElementAsString(DCM_PerformingPhysicianName) );
      QString referringPhysician(ctkDataset.GetElementAsString(DCM_ReferringPhysicianName) );
      QString studyDescription(ctkDataset.GetElementAsString(DCM_StudyDescription) );

      QSqlQuery insertStudyStatement ( Database );
      insertStudyStatement.prepare ( "INSERT INTO Studies ( 'StudyInstanceUID', 'PatientsUID', 'StudyID', 'StudyDate', 'StudyTime', 'AccessionNumber', 'ModalitiesInStudy', 'InstitutionName', 'ReferringPhysician', 'PerformingPhysiciansName', 'StudyDescription' ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )" );
      insertStudyStatement.bindValue ( 0, studyInstanceUID );
      insertStudyStatement.bindValue ( 1, dbPatientID );
      insertStudyStatement.bindValue ( 2, studyID );
      insertStudyStatement.bindValue ( 3, QDate::fromString ( studyDate, "yyyyMMdd" ) );
      insertStudyStatement.bindValue ( 4, studyTime );
      insertStudyStatement.bindValue ( 5, accessionNumber );
      insertStudyStatement.bindValue ( 6, modalitiesInStudy );
      insertStudyStatement.bindValue ( 7, institutionName );
      insertStudyStatement.bindValue ( 8, referringPhysician );
      insertStudyStatement.bindValue ( 9, performingPhysiciansName );
      insertStudyStatement.bindValue ( 10, studyDescription );
      if ( !insertStudyStatement.exec() )
        {
          logger.error ( "Error executing statament: " + insertStudyStatement.lastQuery() + " Error: " + insertStudyStatement.lastError().text() );
        }
      else
        {
          LastStudyInstanceUID = studyInstanceUID;
        }
    }
  else
    {
    qDebug() << "Used existing study: " << studyInstanceUID;
    }
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::insertSeries(const ctkDICOMItem& ctkDataset, QString studyInstanceUID)
{
  QString seriesInstanceUID(ctkDataset.GetElementAsString(DCM_SeriesInstanceUID) );
  QSqlQuery checkSeriesExistsQuery (Database);
  checkSeriesExistsQuery.prepare ( "SELECT * FROM Series WHERE SeriesInstanceUID = ?" );
  checkSeriesExistsQuery.bindValue ( 0, seriesInstanceUID );
  if (this->LoggedExecVerbose)
    {
    logger.warn ( "Statement: " + checkSeriesExistsQuery.lastQuery() );
    }
  checkSeriesExistsQuery.exec();
  if(!checkSeriesExistsQuery.next())
    {
      qDebug() << "Need to insert new series: " << seriesInstanceUID;

      QString seriesDate(ctkDataset.GetElementAsString(DCM_SeriesDate) );
      QString seriesTime(ctkDataset.GetElementAsString(DCM_SeriesTime) );
      QString seriesDescription(ctkDataset.GetElementAsString(DCM_SeriesDescription) );
      QString modality(ctkDataset.GetElementAsString(DCM_Modality) );
      QString bodyPartExamined(ctkDataset.GetElementAsString(DCM_BodyPartExamined) );
      QString frameOfReferenceUID(ctkDataset.GetElementAsString(DCM_FrameOfReferenceUID) );
      QString contrastAgent(ctkDataset.GetElementAsString(DCM_ContrastBolusAgent) );
      QString scanningSequence(ctkDataset.GetElementAsString(DCM_ScanningSequence) );
      long seriesNumber(ctkDataset.GetElementAsInteger(DCM_SeriesNumber) );
      long acquisitionNumber(ctkDataset.GetElementAsInteger(DCM_AcquisitionNumber) );
      long echoNumber(ctkDataset.GetElementAsInteger(DCM_EchoNumbers) );
      long temporalPosition(ctkDataset.GetElementAsInteger(DCM_TemporalPositionIdentifier) );

      QSqlQuery insertSeriesStatement ( Database );
      insertSeriesStatement.prepare ( "INSERT INTO Series ( 'SeriesInstanceUID', 'StudyInstanceUID', 'SeriesNumber', 'SeriesDate', 'SeriesTime', 'SeriesDescription', 'Modality', 'BodyPartExamined', 'FrameOfReferenceUID', 'AcquisitionNumber', 'ContrastAgent', 'ScanningSequence', 'EchoNumber', 'TemporalPosition' ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )" );
      insertSeriesStatement.bindValue ( 0, seriesInstanceUID );
      insertSeriesStatement.bindValue ( 1, studyInstanceUID );
      insertSeriesStatement.bindValue ( 2, static_cast<int>(seriesNumber) );
      insertSeriesStatement.bindValue ( 3, QDate::fromString ( seriesDate, "yyyyMMdd" ) );
      insertSeriesStatement.bindValue ( 4, seriesTime );
      insertSeriesStatement.bindValue ( 5, seriesDescription );
      insertSeriesStatement.bindValue ( 6, modality );
      insertSeriesStatement.bindValue ( 7, bodyPartExamined );
      insertSeriesStatement.bindValue ( 8, frameOfReferenceUID );
      insertSeriesStatement.bindValue ( 9, static_cast<int>(acquisitionNumber) );
      insertSeriesStatement.bindValue ( 10, contrastAgent );
      insertSeriesStatement.bindValue ( 11, scanningSequence );
      insertSeriesStatement.bindValue ( 12, static_cast<int>(echoNumber) );
      insertSeriesStatement.bindValue ( 13, static_cast<int>(temporalPosition) );
      if ( !insertSeriesStatement.exec() )
        {
          logger.error ( "Error executing statament: "
                         + insertSeriesStatement.lastQuery()
                         + " Error: " + insertSeriesStatement.lastError().text() );
          LastSeriesInstanceUID = "";
        }
      else
        {
          LastSeriesInstanceUID = seriesInstanceUID;
        }
    }
  else
    {
    qDebug() << "Used existing series: " << seriesInstanceUID;
    }
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::setTagsToPrecache( const QStringList tags)
{
  Q_D(ctkDICOMDatabase);
  d->TagsToPrecache = tags;
}

//------------------------------------------------------------------------------
const QStringList ctkDICOMDatabase::tagsToPrecache()
{
  Q_D(ctkDICOMDatabase);
  return d->TagsToPrecache;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabasePrivate::openTagCacheDatabase()
{
  // try to open the database if it's not already open
  if ( this->TagCacheDatabase.isOpen() )
    {
    return true;
    }
  this->TagCacheDatabase = QSqlDatabase::addDatabase(
        "QSQLITE", this->Database.connectionName() + "TagCache");
  this->TagCacheDatabase.setDatabaseName(this->TagCacheDatabaseFilename);
  if ( !this->TagCacheDatabase.open() )
    {
    qDebug() << "TagCacheDatabase would not open!\n";
    qDebug() << "TagCacheDatabaseFilename is: " << this->TagCacheDatabaseFilename << "\n";
    return false;
    }

  // Disable synchronous writing to make modifications faster
  QSqlQuery pragmaSyncQuery(this->TagCacheDatabase);
  pragmaSyncQuery.exec("PRAGMA synchronous = OFF");
  pragmaSyncQuery.finish();

  return true;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::precacheTags( const QString sopInstanceUID )
{
  Q_Q(ctkDICOMDatabase);

  ctkDICOMItem dataset;
  QString fileName = q->fileForInstance(sopInstanceUID);
  dataset.InitializeFromFile(fileName);


  QStringList sopInstanceUIDs, tags, values;
  foreach (const QString &tag, this->TagsToPrecache)
    {
    unsigned short group, element;
    q->tagToGroupElement(tag, group, element);
    DcmTagKey tagKey(group, element);
    QString value = dataset.GetAllElementValuesAsString(tagKey);
    sopInstanceUIDs << sopInstanceUID;
    tags << tag;
    values << value;
    }

  QSqlQuery transaction( this->TagCacheDatabase );
  transaction.prepare( "BEGIN TRANSACTION" );
  transaction.exec();

  q->cacheTags(sopInstanceUIDs, tags, values);

  transaction = QSqlQuery( this->TagCacheDatabase );
  transaction.prepare( "END TRANSACTION" );
  transaction.exec();
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::insert( const ctkDICOMItem& ctkDataset, const QString& filePath, bool storeFile, bool generateThumbnail)
{
  Q_Q(ctkDICOMDatabase);

  // this is the method that all other insert signatures end up calling
  // after they have pre-parsed their arguments

  // Check to see if the file has already been loaded
  // TODO:
  // It could make sense to actually remove the dataset and re-add it. This needs the remove
  // method we still have to write.
  //
  //

  QString sopInstanceUID ( ctkDataset.GetElementAsString(DCM_SOPInstanceUID) );

  QSqlQuery fileExistsQuery ( Database );
  fileExistsQuery.prepare("SELECT InsertTimestamp,Filename FROM Images WHERE SOPInstanceUID == :sopInstanceUID");
  fileExistsQuery.bindValue(":sopInstanceUID",sopInstanceUID);
  {
  bool success = fileExistsQuery.exec();
  if (!success)
    {
      logger.error("SQLITE ERROR: " + fileExistsQuery.lastError().driverText());
      return;
    }
  bool found = fileExistsQuery.next();
  if (this->LoggedExecVerbose)
    {
    qDebug() << "inserting filePath: " << filePath;
    }
  if (!found)
    {
    if (this->LoggedExecVerbose)
      {
      qDebug() << "database filename for " << sopInstanceUID << " is empty - we should insert on top of it";
      }
    }
  else
    {
      QString databaseFilename(fileExistsQuery.value(1).toString());
      QDateTime fileLastModified(QFileInfo(databaseFilename).lastModified());
      QDateTime databaseInsertTimestamp(QDateTime::fromString(fileExistsQuery.value(0).toString(),Qt::ISODate));

      if ( databaseFilename == filePath && fileLastModified < databaseInsertTimestamp )
        {
          logger.debug ( "File " + databaseFilename + " already added" );
          return;
        }
      else
        {
        QSqlQuery deleteFile ( Database );
        deleteFile.prepare("DELETE FROM Images WHERE SOPInstanceUID == :sopInstanceUID");
        deleteFile.bindValue(":sopInstanceUID",sopInstanceUID);
        bool success = deleteFile.exec();
        if (!success)
          {
            logger.error("SQLITE ERROR deleting old image row: " + deleteFile.lastError().driverText());
            return;
          }
        }
    }
  }

  //If the following fields can not be evaluated, cancel evaluation of the DICOM file
  QString patientsName(ctkDataset.GetElementAsString(DCM_PatientName) );
  QString studyInstanceUID(ctkDataset.GetElementAsString(DCM_StudyInstanceUID) );
  QString seriesInstanceUID(ctkDataset.GetElementAsString(DCM_SeriesInstanceUID) );
  QString patientID(ctkDataset.GetElementAsString(DCM_PatientID) );
  if ( patientID.isEmpty() && !studyInstanceUID.isEmpty() )
  { // Use study instance uid as patient id if patient id is empty - can happen on anonymized datasets
    // see: http://www.na-mic.org/Bug/view.php?id=2040
    logger.warn("Patient ID is empty, using studyInstanceUID as patient ID");
    patientID = studyInstanceUID;
  }
  if ( patientsName.isEmpty() && !patientID.isEmpty() )
    { // Use patient id as name if name is empty - can happen on anonymized datasets
      // see: http://www.na-mic.org/Bug/view.php?id=1643
      patientsName = patientID;
    }
  if ( patientsName.isEmpty() || studyInstanceUID.isEmpty() || patientID.isEmpty() )
    {
      logger.error("Dataset is missing necessary information (patient name, study instance UID, or patient ID)!");
      return;
    }

  // store the file if the database is not in memomry
  // TODO: if we are called from insert(file) we
  // have to do something else
  //
  QString filename = filePath;
  if ( storeFile && !q->isInMemory() && !seriesInstanceUID.isEmpty() )
    {
      // QString studySeriesDirectory = studyInstanceUID + "/" + seriesInstanceUID;
      QString destinationDirectoryName = q->databaseDirectory() + "/dicom/";
      QDir destinationDir(destinationDirectoryName);
      filename = destinationDirectoryName +
          studyInstanceUID + "/" +
          seriesInstanceUID + "/" +
          sopInstanceUID;

      destinationDir.mkpath(studyInstanceUID + "/" +
                            seriesInstanceUID);

      if(filePath.isEmpty())
        {
          if (this->LoggedExecVerbose)
            {
            logger.debug ( "Saving file: " + filename );
            }

          if ( !ctkDataset.SaveToFile( filename) )
            {
              logger.error ( "Error saving file: " + filename );
              return;
            }
        }
      else
        {
          // we're inserting an existing file

          QFile currentFile( filePath );
          currentFile.copy(filename);
          if (this->LoggedExecVerbose)
            {
            logger.debug("Copy file from: " + filePath + " to: " + filename);
            }
        }
    }

  //The dbPatientID  is a unique number within the database,
  //generated by the sqlite autoincrement
  //The patientID  is the (non-unique) DICOM patient id
  int dbPatientID = LastPatientUID;

  if ( patientID != "" && patientsName != "" )
    {
      //Speed up: Check if patient is the same as in last file;
      // very probable, as all images belonging to a study have the same patient
      QString patientsBirthDate(ctkDataset.GetElementAsString(DCM_PatientBirthDate) );
      if ( LastPatientID != patientID
           || LastPatientsBirthDate != patientsBirthDate
           || LastPatientsName != patientsName )
        {
          if (this->LoggedExecVerbose)
            {
            qDebug() << "This looks like a different patient from last insert: " << patientID;
            }
          // Ok, something is different from last insert, let's insert him if he's not
          // already in the db.

          dbPatientID = insertPatient( ctkDataset );

          // let users of this class track when things happen
          emit q->patientAdded(dbPatientID, patientID, patientsName, patientsBirthDate);

          /// keep this for the next image
          LastPatientUID = dbPatientID;
          LastPatientID = patientID;
          LastPatientsBirthDate = patientsBirthDate;
          LastPatientsName = patientsName;
        }

      if (this->LoggedExecVerbose)
        {
        qDebug() << "Going to insert this instance with dbPatientID: " << dbPatientID;
        }

      // Patient is in now. Let's continue with the study

      if ( studyInstanceUID != "" && LastStudyInstanceUID != studyInstanceUID )
        {
          insertStudy(ctkDataset,dbPatientID);

          // let users of this class track when things happen
          emit q->studyAdded(studyInstanceUID);
          qDebug() << "Study Added";
        }


      if ( seriesInstanceUID != "" && seriesInstanceUID != LastSeriesInstanceUID )
        {
          insertSeries(ctkDataset, studyInstanceUID);

          // let users of this class track when things happen
          emit q->seriesAdded(seriesInstanceUID);
          qDebug() << "Series Added";
        }
      // TODO: what to do with imported files
      //
      if ( !filename.isEmpty() && !seriesInstanceUID.isEmpty() )
        {
          QSqlQuery checkImageExistsQuery (Database);
          checkImageExistsQuery.prepare ( "SELECT * FROM Images WHERE Filename = ?" );
          checkImageExistsQuery.bindValue ( 0, filename );
          checkImageExistsQuery.exec();
          if (this->LoggedExecVerbose)
            {
            qDebug() << "Maybe add Instance";
            }
          if(!checkImageExistsQuery.next())
            {
              QSqlQuery insertImageStatement ( Database );
              insertImageStatement.prepare ( "INSERT INTO Images ( 'SOPInstanceUID', 'Filename', 'SeriesInstanceUID', 'InsertTimestamp' ) VALUES ( ?, ?, ?, ? )" );
              insertImageStatement.bindValue ( 0, sopInstanceUID );
              insertImageStatement.bindValue ( 1, filename );
              insertImageStatement.bindValue ( 2, seriesInstanceUID );
              insertImageStatement.bindValue ( 3, QDateTime::currentDateTime() );
              insertImageStatement.exec();

              // insert was needed, so cache any application-requested tags
              this->precacheTags(sopInstanceUID);

              // let users of this class track when things happen
              emit q->instanceAdded(sopInstanceUID);
              if (this->LoggedExecVerbose)
                {
                qDebug() << "Instance Added";
                }
            }
        }

      if( generateThumbnail && thumbnailGenerator && !seriesInstanceUID.isEmpty() )
        {
          QString studySeriesDirectory = studyInstanceUID + "/" + seriesInstanceUID;
          //Create thumbnail here
          QString thumbnailPath = q->databaseDirectory() +
              "/thumbs/" + studyInstanceUID + "/" + seriesInstanceUID
              + "/" + sopInstanceUID + ".png";
          QFileInfo thumbnailInfo(thumbnailPath);
          if( !(thumbnailInfo.exists()
                && (thumbnailInfo.lastModified() > QFileInfo(filename).lastModified())))
            {
              QDir(q->databaseDirectory() + "/thumbs/").mkpath(studySeriesDirectory);
              DicomImage dcmImage(QDir::toNativeSeparators(filename).toLatin1());
              thumbnailGenerator->generateThumbnail(&dcmImage, thumbnailPath);
            }
        }

      if (q->isInMemory())
        {
          emit q->databaseChanged();
        }
    }
  else
    {
    qDebug() << "No patient name or no patient id - not inserting!";
    }
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::fileExistsAndUpToDate(const QString& filePath)
{
  Q_D(ctkDICOMDatabase);
  bool result(false);

  QSqlQuery check_filename_query(database());
  check_filename_query.prepare("SELECT InsertTimestamp FROM Images WHERE Filename == ?");
  check_filename_query.bindValue(0,filePath);
  d->loggedExec(check_filename_query);
  if (
      check_filename_query.next() &&
      QFileInfo(filePath).lastModified() < QDateTime::fromString(check_filename_query.value(0).toString(),Qt::ISODate)
      )
    {
      result = true;
    }
  check_filename_query.finish();
  return result;
}


//------------------------------------------------------------------------------
bool ctkDICOMDatabase::isOpen() const
{
  Q_D(const ctkDICOMDatabase);
  return d->Database.isOpen();
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::isInMemory() const
{
  Q_D(const ctkDICOMDatabase);
  return d->DatabaseFileName == ":memory:";
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::removeSeries(const QString& seriesInstanceUID)
{
  Q_D(ctkDICOMDatabase);

  // get all images from series
  QSqlQuery fileExistsQuery ( d->Database );
  fileExistsQuery.prepare("SELECT Filename, SOPInstanceUID, StudyInstanceUID FROM Images,Series WHERE Series.SeriesInstanceUID = Images.SeriesInstanceUID AND Images.SeriesInstanceUID = :seriesID");
  fileExistsQuery.bindValue(":seriesID",seriesInstanceUID);
  bool success = fileExistsQuery.exec();
  if (!success)
    {
      logger.error("SQLITE ERROR: " + fileExistsQuery.lastError().driverText());
      return false;
    }

  QList< QPair<QString,QString> > removeList;
  while ( fileExistsQuery.next() )
    {
      QString dbFilePath = fileExistsQuery.value(fileExistsQuery.record().indexOf("Filename")).toString();
      QString sopInstanceUID = fileExistsQuery.value(fileExistsQuery.record().indexOf("SOPInstanceUID")).toString();
      QString studyInstanceUID = fileExistsQuery.value(fileExistsQuery.record().indexOf("StudyInstanceUID")).toString();
      QString internalFilePath = studyInstanceUID + "/" + seriesInstanceUID + "/" + sopInstanceUID;
      removeList << qMakePair(dbFilePath,internalFilePath);
    }

  QSqlQuery fileRemove ( d->Database );
  fileRemove.prepare("DELETE FROM Images WHERE SeriesInstanceUID == :seriesID");
  fileRemove.bindValue(":seriesID",seriesInstanceUID);
  logger.debug("SQLITE: removing seriesInstanceUID " + seriesInstanceUID);
  success = fileRemove.exec();
  if (!success)
    {
      logger.error("SQLITE ERROR: could not remove seriesInstanceUID " + seriesInstanceUID);
      logger.error("SQLITE ERROR: " + fileRemove.lastError().driverText());
    }

  QPair<QString,QString> fileToRemove;
  foreach (fileToRemove, removeList)
    {
      QString dbFilePath = fileToRemove.first;
      QString thumbnailToRemove = databaseDirectory() + "/thumbs/" + fileToRemove.second + ".png";

      // check that the file is below our internal storage
      if (dbFilePath.startsWith( databaseDirectory() + "/dicom/"))
        {
          if (!dbFilePath.endsWith(fileToRemove.second))
            {
              logger.error("Database inconsistency detected during delete!");
              continue;
            }
          if (QFile( dbFilePath ).remove())
            {
              if (d->LoggedExecVerbose)
                {
                logger.debug("Removed file " + dbFilePath );
                }
            }
          else
            {
              logger.warn("Failed to remove file " + dbFilePath );
            }
        }
      // Remove thumbnail (if exists)
      QFile thumbnailFile(thumbnailToRemove);
      if (thumbnailFile.exists())
        {
        if (!thumbnailFile.remove())
          {
          logger.warn("Failed to remove thumbnail " + thumbnailToRemove);
          }
        }
    }

  this->cleanup();

  d->resetLastInsertedValues();

  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::cleanup()
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery seriesCleanup ( d->Database );
  seriesCleanup.exec("DELETE FROM Series WHERE ( SELECT COUNT(*) FROM Images WHERE Images.SeriesInstanceUID = Series.SeriesInstanceUID ) = 0;");
  seriesCleanup.exec("DELETE FROM Studies WHERE ( SELECT COUNT(*) FROM Series WHERE Series.StudyInstanceUID = Studies.StudyInstanceUID ) = 0;");
  seriesCleanup.exec("DELETE FROM Patients WHERE ( SELECT COUNT(*) FROM Studies WHERE Studies.PatientsUID = Patients.UID ) = 0;");
  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::removeStudy(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMDatabase);

  QSqlQuery seriesForStudy( d->Database );
  seriesForStudy.prepare("SELECT SeriesInstanceUID FROM Series WHERE StudyInstanceUID = :studyID");
  seriesForStudy.bindValue(":studyID", studyInstanceUID);
  bool success = seriesForStudy.exec();
  if (!success)
    {
      logger.error("SQLITE ERROR: " + seriesForStudy.lastError().driverText());
      return false;
    }
  bool result = true;
  while ( seriesForStudy.next() )
    {
      QString seriesInstanceUID = seriesForStudy.value(seriesForStudy.record().indexOf("SeriesInstanceUID")).toString();
      if ( ! this->removeSeries(seriesInstanceUID) )
        {
          result = false;
        }
    }
  d->resetLastInsertedValues();
  return result;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::removePatient(const QString& patientID)
{
  Q_D(ctkDICOMDatabase);

  QSqlQuery studiesForPatient( d->Database );
  studiesForPatient.prepare("SELECT StudyInstanceUID FROM Studies WHERE PatientsUID = :patientsID");
  studiesForPatient.bindValue(":patientsID", patientID);
  bool success = studiesForPatient.exec();
  if (!success)
    {
      logger.error("SQLITE ERROR: " + studiesForPatient.lastError().driverText());
      return false;
    }
  bool result = true;
  while ( studiesForPatient.next() )
    {
      QString studyInstanceUID = studiesForPatient.value(studiesForPatient.record().indexOf("StudyInstanceUID")).toString();
      if ( ! this->removeStudy(studyInstanceUID) )
        {
          result = false;
        }
    }
  d->resetLastInsertedValues();
  return result;
}

///
/// Code related to the tagCache
///

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::tagCacheExists()
{
  Q_D(ctkDICOMDatabase);

  if (d->TagCacheVerified)
    {
    return true;
    }

  if (!d->openTagCacheDatabase())
    {
    return false;
    }

  if (d->TagCacheDatabase.tables().count() == 0)
    {
    return false;
    }

  // check that the table exists
  QSqlQuery cacheExists( d->TagCacheDatabase );
  cacheExists.prepare("SELECT * FROM TagCache LIMIT 1");
  bool success = d->loggedExec(cacheExists);
  if (success)
    {
    d->TagCacheVerified = true;
    return true;
    }
  qDebug() << "TagCacheDatabase NOT verified based on table check!\n";
  return false;
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::initializeTagCache()
{
  Q_D(ctkDICOMDatabase);

  // First, drop any existing table
  if ( this->tagCacheExists() )
    {
    qDebug() << "TagCacheDatabase drop existing table\n";
    QSqlQuery dropCacheTable( d->TagCacheDatabase );
    dropCacheTable.prepare( "DROP TABLE TagCache" );
    d->loggedExec(dropCacheTable);
    }

  // now create a table
  qDebug() << "TagCacheDatabase adding table\n";
  QSqlQuery createCacheTable( d->TagCacheDatabase );
  createCacheTable.prepare(
    "CREATE TABLE TagCache (SOPInstanceUID, Tag, Value, PRIMARY KEY (SOPInstanceUID, Tag))" );
  bool success = d->loggedExec(createCacheTable);
  if (!success)
    {
    return false;
    }

  d->TagCacheVerified = true;
  return true;
}

//------------------------------------------------------------------------------
QString ctkDICOMDatabase::cachedTag(const QString sopInstanceUID, const QString tag)
{
  Q_D(ctkDICOMDatabase);
  if ( !this->tagCacheExists() )
    {
    if ( !this->initializeTagCache() )
      {
      return( "" );
      }
    }
  QSqlQuery selectValue( d->TagCacheDatabase );
  selectValue.prepare( "SELECT Value FROM TagCache WHERE SOPInstanceUID = :sopInstanceUID AND Tag = :tag" );
  selectValue.bindValue(":sopInstanceUID",sopInstanceUID);
  selectValue.bindValue(":tag",tag);
  d->loggedExec(selectValue);
  QString result("");
  if (selectValue.next())
    {
    result = selectValue.value(0).toString();
    if (result == QString(""))
      {
      result = ValueIsEmptyString;
      }
    }
  return( result );
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::cacheTag(const QString sopInstanceUID, const QString tag, const QString value)
{
  QStringList sopInstanceUIDs, tags, values;
  sopInstanceUIDs << sopInstanceUID;
  tags << tag;
  values << value;
  return this->cacheTags(sopInstanceUIDs, tags, values);
}

//------------------------------------------------------------------------------
bool ctkDICOMDatabase::cacheTags(const QStringList sopInstanceUIDs, const QStringList tags, QStringList values)
{
  Q_D(ctkDICOMDatabase);
  if ( !this->tagCacheExists() )
    {
    if ( !this->initializeTagCache() )
      {
      return false;
      }
    }

  // replace empty strings with special flag string
  QStringList::iterator i;
  for (i = values.begin(); i != values.end(); ++i)
    {
    if (*i == "")
      {
      *i = TagNotInInstance;
      }
    }

  QSqlQuery insertTags( d->TagCacheDatabase );
  insertTags.prepare( "INSERT OR REPLACE INTO TagCache VALUES(?,?,?)" );
  insertTags.addBindValue(sopInstanceUIDs);
  insertTags.addBindValue(tags);
  insertTags.addBindValue(values);
  return d->loggedExecBatch(insertTags);
}
