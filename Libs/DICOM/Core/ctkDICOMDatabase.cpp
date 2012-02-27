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
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QDate>
#include <QStringList>
#include <QSet>
#include <QFile>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include <QFileSystemWatcher>

// ctkDICOM includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMAbstractThumbnailGenerator.h"
#include "ctkDICOMDataset.h"

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
#include <dcmimage.h>

#include <dcmtk/dcmjpeg/djdecode.h>  /* for dcmjpeg decoders */
#include <dcmtk/dcmjpeg/djencode.h>  /* for dcmjpeg encoders */
#include <dcmtk/dcmdata/dcrledrg.h>  /* for DcmRLEDecoderRegistration */
#include <dcmtk/dcmdata/dcrleerg.h>  /* for DcmRLEEncoderRegistration */

//------------------------------------------------------------------------------
static ctkLogger logger("org.commontk.dicom.DICOMDatabase" );
//------------------------------------------------------------------------------

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

  // dataset must be set always
  // filePath has to be set if this is an import of an actual file
  void insert ( const ctkDICOMDataset& ctkDataset, const QString& filePath, bool storeFile = true, bool generateThumbnail = true);


  /// Name of the database file (i.e. for SQLITE the sqlite file)
  QString      DatabaseFileName;
  QString      LastError;
  QSqlDatabase Database;
  QMap<QString, QString> LoadedHeader;

  ctkDICOMAbstractThumbnailGenerator* thumbnailGenerator;
  
  /// these are for optimizing the import of image sequences
  /// since most information are identical for all slices
  QString lastPatientID;
  QString lastPatientsName;
  QString lastPatientsBirthDate;
  QString lastStudyInstanceUID;
  QString lastSeriesInstanceUID;
  int lastPatientUID;
};

//------------------------------------------------------------------------------
// ctkDICOMDatabasePrivate methods

//------------------------------------------------------------------------------
ctkDICOMDatabasePrivate::ctkDICOMDatabasePrivate(ctkDICOMDatabase& o): q_ptr(&o)
{
    this->thumbnailGenerator = NULL;
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::init(QString databaseFilename)
{
  Q_Q(ctkDICOMDatabase);

  q->openDatabase(databaseFilename);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::registerCompressionLibraries(){
    logger.debug("Register compression libraries");
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
  logger.debug( "SQL worked!\n SQL: " + query.lastQuery());
    }
  return (success);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::openDatabase(const QString databaseFile, const QString& connectionName )
{
  Q_D(ctkDICOMDatabase);
  d->DatabaseFileName = databaseFile;
  d->Database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
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
  if (!isInMemory())
    {
    QFileSystemWatcher* watcher = new QFileSystemWatcher(QStringList(databaseFile),this);
    connect(watcher, SIGNAL(fileChanged(QString)),this, SIGNAL (databaseChanged()) );
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
      qDebug() << *it << "\n";
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
bool ctkDICOMDatabase::initializeDatabase(const char* sqlFileName)
{
  Q_D(ctkDICOMDatabase);
  return d->executeScript(sqlFileName);
}

//------------------------------------------------------------------------------
void ctkDICOMDatabase::closeDatabase()
{
  Q_D(ctkDICOMDatabase);
  d->Database.close();
}

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
      QString tag = QString("%1,%2").arg(
          dO->getGTag(),4,16,QLatin1Char('0')).arg(
          dO->getETag(),4,16,QLatin1Char('0'));
      std::ostringstream s;
      dO->print(s);
      d->LoadedHeader[tag] = QString(s.str().c_str());
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

//------------------------------------------------------------------------------
/*
void ctkDICOMDatabase::insert ( DcmDataset *dataset ) {
  this->insert ( dataset, QString() );
}
*/

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert( DcmDataset *dataset, bool storeFile, bool generateThumbnail)
{
  if (!dataset)
  {
    return;
  }
  ctkDICOMDataset ctkDataset;
  ctkDataset.InitializeFromDataset(dataset, false /* do not take ownership */);
  this->insert(ctkDataset,storeFile,generateThumbnail);
}
void ctkDICOMDatabase::insert( const ctkDICOMDataset& ctkDataset, bool storeFile, bool generateThumbnail)
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

  logger.debug( "Processing " + filePath );

  std::string filename = filePath.toStdString();

  DcmFileFormat fileformat;
  ctkDICOMDataset ctkDataset;

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
void ctkDICOMDatabasePrivate::insert( const ctkDICOMDataset& ctkDataset, const QString& filePath, bool storeFile, bool generateThumbnail)
{
  Q_Q(ctkDICOMDatabase);

  // Check to see if the file has already been loaded
  // TODO:
  // It could make sense to actually remove the dataset and re-add it. This needs the remove
  // method we still have to write.
  //

  QString sopInstanceUID ( ctkDataset.GetElementAsString(DCM_SOPInstanceUID) );

  QSqlQuery fileExists ( Database );
  fileExists.prepare("SELECT InsertTimestamp,Filename FROM Images WHERE SOPInstanceUID == ?");
  fileExists.bindValue(0,sopInstanceUID);
  fileExists.exec();
  if ( fileExists.next() && QFileInfo(fileExists.value(1).toString()).lastModified() < QDateTime::fromString(fileExists.value(0).toString(),Qt::ISODate) )
  {
    logger.debug ( "File " + fileExists.value(1).toString() + " already added" );
    return;
  }

  //If the following fields can not be evaluated, cancel evaluation of the DICOM file
  QString patientsName(ctkDataset.GetElementAsString(DCM_PatientName) );
  QString studyInstanceUID(ctkDataset.GetElementAsString(DCM_StudyInstanceUID) );
  QString seriesInstanceUID(ctkDataset.GetElementAsString(DCM_SeriesInstanceUID) );
  QString patientID(ctkDataset.GetElementAsString(DCM_PatientID) );
  if ( patientsName.isEmpty() && !patientID.isEmpty() )
  { // Use patient id as name if name is empty - can happen on anonymized datasets
    // see: http://www.na-mic.org/Bug/view.php?id=1643
    patientsName = patientID;
  }
  if ( patientsName.isEmpty() || studyInstanceUID.isEmpty() || patientID.isEmpty() )
  {
    logger.error("Dataset is missing necessary information!");
    return;
  } 

  QString patientsBirthDate(ctkDataset.GetElementAsString(DCM_PatientBirthDate) );
  QString patientsBirthTime(ctkDataset.GetElementAsString(DCM_PatientBirthTime) );
  QString patientsSex(ctkDataset.GetElementAsString(DCM_PatientSex) );
  QString patientsAge(ctkDataset.GetElementAsString(DCM_PatientAge) );
  QString patientComments(ctkDataset.GetElementAsString(DCM_PatientComments) );
  QString studyID(ctkDataset.GetElementAsString(DCM_StudyID) );
  QString studyDate(ctkDataset.GetElementAsString(DCM_StudyDate) );
  QString studyTime(ctkDataset.GetElementAsString(DCM_StudyTime) );
  QString accessionNumber(ctkDataset.GetElementAsString(DCM_AccessionNumber) );
  QString modalitiesInStudy(ctkDataset.GetElementAsString(DCM_ModalitiesInStudy) );
  QString institutionName(ctkDataset.GetElementAsString(DCM_InstitutionName) );
  QString performingPhysiciansName(ctkDataset.GetElementAsString(DCM_PerformingPhysicianName) );
  QString referringPhysician(ctkDataset.GetElementAsString(DCM_ReferringPhysicianName) );
  QString studyDescription(ctkDataset.GetElementAsString(DCM_StudyDescription) );

  QString seriesDate(ctkDataset.GetElementAsString(DCM_SeriesDate) );
  QString seriesTime(ctkDataset.GetElementAsString(DCM_SeriesTime) );
  QString seriesDescription(ctkDataset.GetElementAsString(DCM_SeriesDescription) );
  QString bodyPartExamined(ctkDataset.GetElementAsString(DCM_BodyPartExamined) );
  QString frameOfReferenceUID(ctkDataset.GetElementAsString(DCM_FrameOfReferenceUID) );
  QString contrastAgent(ctkDataset.GetElementAsString(DCM_ContrastBolusAgent) );
  QString scanningSequence(ctkDataset.GetElementAsString(DCM_ScanningSequence) );

  long seriesNumber(ctkDataset.GetElementAsInteger(DCM_SeriesNumber) );
  long acquisitionNumber(ctkDataset.GetElementAsInteger(DCM_AcquisitionNumber) );
  long echoNumber(ctkDataset.GetElementAsInteger(DCM_EchoNumbers) );
  long temporalPosition(ctkDataset.GetElementAsInteger(DCM_TemporalPositionIdentifier) );

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
      logger.debug ( "Saving file: " + filename );

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
      logger.debug( "Copy file from: " + filePath );
      logger.debug( "Copy file to  : " + filename );
    }
  }

  QSqlQuery checkPatientExistsQuery(Database);
  //The dbPatientID  is a unique number within the database, 
  //generated by the sqlite autoincrement
  //The patientID  is the (non-unique) DICOM patient id
  int dbPatientID = -1;

  if ( patientID != "" && patientsName != "" )
    {
    //Speed up: Check if patient is the same as in last file; 
    // very probable, as all images belonging to a study have the same patient
    if ( lastPatientID != patientID 
          || lastPatientsBirthDate != patientsBirthDate 
          || lastPatientsName != patientsName )
      {
      // Ok, something is different from last insert, let's insert him if he's not
      // already in the db.
      //

      // Check if patient is already present in the db
      // TODO: maybe add birthdate check for extra safety
      checkPatientExistsQuery.prepare ( "SELECT * FROM Patients WHERE PatientID = ? AND PatientsName = ?" );
      checkPatientExistsQuery.bindValue ( 0, patientID );
      checkPatientExistsQuery.bindValue ( 1, patientsName );
      loggedExec(checkPatientExistsQuery);

      if (checkPatientExistsQuery.next())
      {
        // we found him
        dbPatientID = checkPatientExistsQuery.value(checkPatientExistsQuery.record().indexOf("UID")).toInt();
      }
      else
        {
        // Insert it
        QSqlQuery insertPatientStatement ( Database );
        insertPatientStatement.prepare ( "INSERT INTO Patients ('UID', 'PatientsName', 'PatientID', 'PatientsBirthDate', 'PatientsBirthTime', 'PatientsSex', 'PatientsAge', 'PatientsComments' ) values ( NULL, ?, ?, ?, ?, ?, ?, ? )" );
        insertPatientStatement.bindValue ( 0, patientsName );
        insertPatientStatement.bindValue ( 1, patientID );
        insertPatientStatement.bindValue ( 2, patientsBirthDate );
        insertPatientStatement.bindValue ( 3, patientsBirthTime );
        insertPatientStatement.bindValue ( 4, patientsSex );
        // TODO: shift patient's age to study, 
        // since this is not a patient level attribute in images
        // insertPatientStatement.bindValue ( 5, patientsAge );
        insertPatientStatement.bindValue ( 6, patientComments );
        loggedExec(insertPatientStatement);
        dbPatientID = insertPatientStatement.lastInsertId().toInt();
        logger.debug ( "New patient inserted: " + QString().setNum ( dbPatientID ) );
        }
      /// keep this for the next image
      lastPatientUID = dbPatientID;
      lastPatientID = patientID;
      lastPatientsBirthDate = patientsBirthDate;
      lastPatientsName = patientsName;
      }

    // Patient is in now. Let's continue with the study

    if ( studyInstanceUID != "" && lastStudyInstanceUID != studyInstanceUID )
    {
      QSqlQuery checkStudyExistsQuery (Database);
      checkStudyExistsQuery.prepare ( "SELECT * FROM Studies WHERE StudyInstanceUID = ?" );
      checkStudyExistsQuery.bindValue ( 0, studyInstanceUID );
      checkStudyExistsQuery.exec();
      if(!checkStudyExistsQuery.next())
      {
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
          lastStudyInstanceUID = studyInstanceUID;
        }

      }
    }

    if ( seriesInstanceUID != "" && seriesInstanceUID != lastSeriesInstanceUID )
    {
      QSqlQuery checkSeriesExistsQuery (Database);
      checkSeriesExistsQuery.prepare ( "SELECT * FROM Series WHERE SeriesInstanceUID = ?" );
      checkSeriesExistsQuery.bindValue ( 0, seriesInstanceUID );
      logger.warn ( "Statement: " + checkSeriesExistsQuery.lastQuery() );
      loggedExec(checkSeriesExistsQuery);
      if(!checkSeriesExistsQuery.next())
      {
        QSqlQuery insertSeriesStatement ( Database );
        insertSeriesStatement.prepare ( "INSERT INTO Series ( 'SeriesInstanceUID', 'StudyInstanceUID', 'SeriesNumber', 'SeriesDate', 'SeriesTime', 'SeriesDescription', 'BodyPartExamined', 'FrameOfReferenceUID', 'AcquisitionNumber', 'ContrastAgent', 'ScanningSequence', 'EchoNumber', 'TemporalPosition' ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )" );
        insertSeriesStatement.bindValue ( 0, seriesInstanceUID );
        insertSeriesStatement.bindValue ( 1, studyInstanceUID );
        insertSeriesStatement.bindValue ( 2, static_cast<int>(seriesNumber) );
        insertSeriesStatement.bindValue ( 3, seriesDate );
        insertSeriesStatement.bindValue ( 4, QDate::fromString ( seriesTime, "yyyyMMdd" ) );
        insertSeriesStatement.bindValue ( 5, seriesDescription );
        insertSeriesStatement.bindValue ( 6, bodyPartExamined );
        insertSeriesStatement.bindValue ( 7, frameOfReferenceUID );
        insertSeriesStatement.bindValue ( 8, static_cast<int>(acquisitionNumber) );
        insertSeriesStatement.bindValue ( 9, contrastAgent );
        insertSeriesStatement.bindValue ( 10, scanningSequence );
        insertSeriesStatement.bindValue ( 11, static_cast<int>(echoNumber) );
        insertSeriesStatement.bindValue ( 12, static_cast<int>(temporalPosition) );
        if ( !insertSeriesStatement.exec() )
        {
          logger.error ( "Error executing statament: " 
            + insertSeriesStatement.lastQuery() 
            + " Error: " + insertSeriesStatement.lastError().text() );
          lastSeriesInstanceUID = "";
        }
        else
        {
          lastSeriesInstanceUID = seriesInstanceUID;
        }

      }
    }
    // TODO: what to do with imported files
    //
   if ( !filename.isEmpty() && !seriesInstanceUID.isEmpty() )
   {
     QSqlQuery checkImageExistsQuery (Database);
     checkImageExistsQuery.prepare ( "SELECT * FROM Images WHERE Filename = ?" );
     checkImageExistsQuery.bindValue ( 0, filename );
     checkImageExistsQuery.exec();
     if(!checkImageExistsQuery.next())
      {
        QSqlQuery insertImageStatement ( Database );
        insertImageStatement.prepare ( "INSERT INTO Images ( 'SOPInstanceUID', 'Filename', 'SeriesInstanceUID', 'InsertTimestamp' ) VALUES ( ?, ?, ?, ? )" );
        insertImageStatement.bindValue ( 0, sopInstanceUID );
        insertImageStatement.bindValue ( 1, filename );
        insertImageStatement.bindValue ( 2, seriesInstanceUID );
        insertImageStatement.bindValue ( 3, QDateTime::currentDateTime() );
        insertImageStatement.exec();
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
        DicomImage dcmImage(QDir::toNativeSeparators(filename).toAscii());
        thumbnailGenerator->generateThumbnail(&dcmImage, thumbnailPath);
        }
      }

    if (q->isInMemory())
      {
      emit q->databaseChanged();
      }
    }
}

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


bool ctkDICOMDatabase::isOpen() const
{
  Q_D(const ctkDICOMDatabase);
  return d->Database.isOpen();
}

bool ctkDICOMDatabase::isInMemory() const
{
  Q_D(const ctkDICOMDatabase);
  return d->DatabaseFileName == ":memory:";
}

bool ctkDICOMDatabase::removeSeries(const QString& seriesInstanceUID)
{
  Q_D(ctkDICOMDatabase);

  // get all images from series
  QSqlQuery fileExists ( d->Database );
  fileExists.prepare("SELECT Filename, SOPInstanceUID, StudyInstanceUID FROM Images,Series WHERE Series.SeriesInstanceUID = Images.SeriesInstanceUID AND Images.SeriesInstanceUID = :seriesID");
  fileExists.bindValue(":seriesID",seriesInstanceUID);
  bool success = fileExists.exec();
  if (!success)
  {
    logger.error("SQLITE ERROR: " + fileExists.lastError().driverText());
    return false;
  }

  QList< QPair<QString,QString> > removeList;
  while ( fileExists.next() )
  {
    QString dbFilePath = fileExists.value(fileExists.record().indexOf("Filename")).toString();
    QString sopInstanceUID = fileExists.value(fileExists.record().indexOf("SOPInstanceUID")).toString();
    QString studyInstanceUID = fileExists.value(fileExists.record().indexOf("StudyInstanceUID")).toString();
    QString internalFilePath = studyInstanceUID + "/" + seriesInstanceUID + "/" + sopInstanceUID;
    removeList << qMakePair(dbFilePath,internalFilePath);
  }

  QSqlQuery fileRemove ( d->Database );
  fileRemove.prepare("DELETE FROM Images WHERE SeriesInstanceUID == ?");
  fileRemove.bindValue(0,seriesInstanceUID);
  fileRemove.exec();
  
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
        logger.debug("Removed file " + dbFilePath );
      }
      else
      {
        logger.warn("Failed to remove file " + dbFilePath );
      }
    }
    if (QFile( thumbnailToRemove ).remove())
      {
        logger.debug("Removed thumbnail " + thumbnailToRemove);
      }
      else
      {
        logger.warn("Failed to remove thumbnail " + thumbnailToRemove);
      }
    }    

  this->cleanup();

  return true;
}

bool ctkDICOMDatabase::cleanup()
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery seriesCleanup ( d->Database );
  seriesCleanup.exec("DELETE FROM Series WHERE ( SELECT COUNT(*) FROM Images WHERE Images.SeriesInstanceUID = Series.SeriesInstanceUID ) = 0;");
  seriesCleanup.exec("DELETE FROM Studies WHERE ( SELECT COUNT(*) FROM Series WHERE Series.StudyInstanceUID = Studies.StudyInstanceUID ) = 0;");
  seriesCleanup.exec("DELETE FROM Patients WHERE ( SELECT COUNT(*) FROM Studies WHERE Studies.PatientsUID = Patients.UID ) = 0;");
  return true;
}

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
  return result;;
}

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
  return result;;
}

