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

  /// Name of the database file (i.e. for SQLITE the sqlite file)
  QString      DatabaseFileName;
  QString      LastError;
  QSqlDatabase Database;
  QMap<QString, QString> LoadedHeader;

  ctkDICOMAbstractThumbnailGenerator* thumbnailGenerator;
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

void ctkDICOMDatabase::setThumbnailGenerator(ctkDICOMAbstractThumbnailGenerator *generator){
    Q_D(ctkDICOMDatabase);
    d->thumbnailGenerator = generator;
}

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
QStringList ctkDICOMDatabase::studiesForPatient(QString patientUID)
{
  Q_D(ctkDICOMDatabase);
  QSqlQuery query(d->Database);
  query.prepare ( "SELECT StudyInstanceUID FROM Studies WHERE PatientsUID = ?" );
  query.bindValue ( 0, patientUID );
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
  d->LoadedHeader.clear();
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
  DcmFileFormat fileFormat;
  OFCondition status = fileFormat.loadFile(fileName.toLatin1().data());
  if (status.good())
    {
    DcmDataset *dataset = fileFormat.getDataset();
    DcmStack stack;
    while (dataset->nextObject(stack, true) == EC_Normal)
      {
      DcmObject *dO = stack.top();
      if (dO->isaString())
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

//------------------------------------------------------------------------------
/*
void ctkDICOMDatabase::insert ( DcmDataset *dataset ) {
  this->insert ( dataset, QString() );
}
*/

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert ( DcmDataset *dataset, bool storeFile, bool generateThumbnail)
{
  Q_D(ctkDICOMDatabase);

  if (!dataset)
    {
    return;
    }
  // Check to see if the file has already been loaded
  OFString sopInstanceUID ;
  dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);

  QSqlQuery fileExists ( d->Database );
  fileExists.prepare("SELECT InsertTimestamp,Filename FROM Images WHERE SOPInstanceUID == ?");
  fileExists.bindValue(0,QString(sopInstanceUID.c_str()));
  fileExists.exec();
  if ( fileExists.next() && QFileInfo(fileExists.value(1).toString()).lastModified() < QDateTime::fromString(fileExists.value(0).toString(),Qt::ISODate) )
    {
    logger.debug ( "File " + fileExists.value(1).toString() + " already added" );
    return;
    }

  OFString patientsName, patientID, patientsBirthDate, patientsBirthTime, patientsSex,
    patientComments, patientsAge;

  OFString studyInstanceUID, studyID, studyDate, studyTime,
    accessionNumber, modalitiesInStudy, institutionName, performingPhysiciansName, referringPhysician, studyDescription;

  OFString seriesInstanceUID, seriesDate, seriesTime,
    seriesDescription, bodyPartExamined, frameOfReferenceUID,
    contrastAgent, scanningSequence;
  OFString instanceNumber;

  Sint32 seriesNumber = 0, acquisitionNumber = 0, echoNumber = 0, temporalPosition = 0;

  //If the following fields can not be evaluated, cancel evaluation of the DICOM file
  dataset->findAndGetOFString(DCM_PatientName, patientsName);
  dataset->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID);
  dataset->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID);
  dataset->findAndGetOFString(DCM_PatientID, patientID);


  dataset->findAndGetOFString(DCM_PatientBirthDate, patientsBirthDate);
  dataset->findAndGetOFString(DCM_PatientBirthTime, patientsBirthTime);
  dataset->findAndGetOFString(DCM_PatientSex, patientsSex);
  dataset->findAndGetOFString(DCM_PatientAge, patientsAge);
  dataset->findAndGetOFString(DCM_PatientComments, patientComments);
  dataset->findAndGetOFString(DCM_StudyID, studyID);
  dataset->findAndGetOFString(DCM_StudyDate, studyDate);
  dataset->findAndGetOFString(DCM_StudyTime, studyTime);
  dataset->findAndGetOFString(DCM_AccessionNumber, accessionNumber);
  dataset->findAndGetOFString(DCM_ModalitiesInStudy, modalitiesInStudy);
  dataset->findAndGetOFString(DCM_InstitutionName, institutionName);
  dataset->findAndGetOFString(DCM_PerformingPhysicianName, performingPhysiciansName);
  dataset->findAndGetOFString(DCM_ReferringPhysicianName, referringPhysician);
  dataset->findAndGetOFString(DCM_StudyDescription, studyDescription);

  dataset->findAndGetOFString(DCM_SeriesDate, seriesDate);
  dataset->findAndGetOFString(DCM_SeriesTime, seriesTime);
  dataset->findAndGetOFString(DCM_SeriesDescription, seriesDescription);
  dataset->findAndGetOFString(DCM_BodyPartExamined, bodyPartExamined);
  dataset->findAndGetOFString(DCM_FrameOfReferenceUID, frameOfReferenceUID);
  dataset->findAndGetOFString(DCM_ContrastBolusAgent, contrastAgent);
  dataset->findAndGetOFString(DCM_ScanningSequence, scanningSequence);

  dataset->findAndGetSint32(DCM_SeriesNumber, seriesNumber);
  dataset->findAndGetSint32(DCM_AcquisitionNumber, acquisitionNumber);
  dataset->findAndGetSint32(DCM_EchoNumbers, echoNumber);
  dataset->findAndGetSint32(DCM_TemporalPositionIdentifier, temporalPosition);

  // store the file if the database is not in memomry
  QString filename;
  if ( storeFile && !this->isInMemory() )
  {
    DcmFileFormat* fileformat = new DcmFileFormat ( dataset );

    QString destinationDirectoryName = databaseDirectory() + "/dicom/";
    QDir destinationDir(destinationDirectoryName);
    QString studySeriesDirectory = QString(studyInstanceUID.c_str()) + "/" + seriesInstanceUID.c_str();
    destinationDir.mkpath(studySeriesDirectory);

    filename = databaseDirectory() + "/dicom/" + pathForDataset(dataset);
    logger.debug ( "Saving file: " + filename );
    OFCondition status = fileformat->saveFile ( filename.toAscii() );
    if ( !status.good() )
      {
      logger.error ( "Error saving file: " + filename + "\nError is " + status.text() );
      delete fileformat;
      return;
      }
    delete fileformat;
  }


  QSqlQuery check_exists_query(d->Database);
  //The patient UID is a unique number within the database, generated by the sqlite autoincrement
  int patientUID = -1;
  if ( patientID != "" && patientsName != "" )
    {
    //Check if patient is already present in the db
    check_exists_query.prepare ( "SELECT * FROM Patients WHERE PatientID = ? AND PatientsName = ?" );
    check_exists_query.bindValue ( 0, QString ( patientID.c_str() ) );
    check_exists_query.bindValue ( 1, QString ( patientsName.c_str() ) );
    check_exists_query.exec();
    
    if (check_exists_query.next())
      {
      patientUID = check_exists_query.value(check_exists_query.record().indexOf("UID")).toInt();
      }
    else
      {
      // Insert it
      QSqlQuery statement ( d->Database );
      statement.prepare ( "INSERT INTO Patients ('UID', 'PatientsName', 'PatientID', 'PatientsBirthDate', 'PatientsBirthTime', 'PatientsSex', 'PatientsAge', 'PatientsComments' ) values ( NULL, ?, ?, ?, ?, ?, ?, ? )" );
      statement.bindValue ( 0, QString ( patientsName.c_str() ) );
      statement.bindValue ( 1, QString ( patientID.c_str() ) );
      statement.bindValue ( 2, QString ( patientsBirthDate.c_str() ) );
      statement.bindValue ( 3, QString ( patientsBirthTime.c_str() ) );
      statement.bindValue ( 4, QString ( patientsSex.c_str() ) );
      // TODO: shift patient's age to study, since this is not a patient level attribute in images
      // statement.bindValue ( 5, QString ( patientsAge.c_str() ) );
      statement.bindValue ( 6, QString ( patientComments.c_str() ) );
      statement.exec ();
      patientUID = statement.lastInsertId().toInt();
      logger.debug ( "New patient inserted: " + QString().setNum ( patientUID ) );
      }
    }

  if ( studyInstanceUID != "" )
    {
    check_exists_query.prepare ( "SELECT * FROM Studies WHERE StudyInstanceUID = ?" );
    check_exists_query.bindValue ( 0, QString ( studyInstanceUID.c_str() ) );
    check_exists_query.exec();
    if(!check_exists_query.next())
      {
      QSqlQuery statement ( d->Database );
      statement.prepare ( "INSERT INTO Studies ( 'StudyInstanceUID', 'PatientsUID', 'StudyID', 'StudyDate', 'StudyTime', 'AccessionNumber', 'ModalitiesInStudy', 'InstitutionName', 'ReferringPhysician', 'PerformingPhysiciansName', 'StudyDescription' ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )" );
      statement.bindValue ( 0, QString ( studyInstanceUID.c_str() ) );
      statement.bindValue ( 1, patientUID );
      statement.bindValue ( 2, QString ( studyID.c_str() ) );
      statement.bindValue ( 3, QDate::fromString ( studyDate.c_str(), "yyyyMMdd" ) );
      statement.bindValue ( 4, QString ( studyTime.c_str() ) );
      statement.bindValue ( 5, QString ( accessionNumber.c_str() ) );
      statement.bindValue ( 6, QString ( modalitiesInStudy.c_str() ) );
      statement.bindValue ( 7, QString ( institutionName.c_str() ) );
      statement.bindValue ( 8, QString ( referringPhysician.c_str() ) );
      statement.bindValue ( 9, QString ( performingPhysiciansName.c_str() ) );
      statement.bindValue ( 10, QString ( studyDescription.c_str() ) );
      if ( !statement.exec() )
        {
        logger.error ( "Error executing statament: " + statement.lastQuery() + " Error: " + statement.lastError().text() );
        }
      }
    }

  if ( seriesInstanceUID != "" )
    {
    check_exists_query.prepare ( "SELECT * FROM Series WHERE SeriesInstanceUID = ?" );
    check_exists_query.bindValue ( 0, QString ( seriesInstanceUID.c_str() ) );
    logger.warn ( "Statement: " + check_exists_query.lastQuery() );
    check_exists_query.exec();
    if(!check_exists_query.next())
      {
      QSqlQuery statement ( d->Database );
      statement.prepare ( "INSERT INTO Series ( 'SeriesInstanceUID', 'StudyInstanceUID', 'SeriesNumber', 'SeriesDate', 'SeriesTime', 'SeriesDescription', 'BodyPartExamined', 'FrameOfReferenceUID', 'AcquisitionNumber', 'ContrastAgent', 'ScanningSequence', 'EchoNumber', 'TemporalPosition' ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )" );
      statement.bindValue ( 0, QString ( seriesInstanceUID.c_str() ) );
      statement.bindValue ( 1, QString ( studyInstanceUID.c_str() ) );
      statement.bindValue ( 2, static_cast<int>(seriesNumber) );
      statement.bindValue ( 3, QString ( seriesDate.c_str() ) );
      statement.bindValue ( 4, QDate::fromString ( seriesTime.c_str(), "yyyyMMdd" ) );
      statement.bindValue ( 5, QString ( seriesDescription.c_str() ) );
      statement.bindValue ( 6, QString ( bodyPartExamined.c_str() ) );
      statement.bindValue ( 7, QString ( frameOfReferenceUID.c_str() ) );
      statement.bindValue ( 8, static_cast<int>(acquisitionNumber) );
      statement.bindValue ( 9, QString ( contrastAgent.c_str() ) );
      statement.bindValue ( 10, QString ( scanningSequence.c_str() ) );
      statement.bindValue ( 11, static_cast<int>(echoNumber) );
      statement.bindValue ( 12, static_cast<int>(temporalPosition) );
      if ( !statement.exec() )
        {
        logger.error ( "Error executing statament: " + statement.lastQuery() + " Error: " + statement.lastError().text() );
        }
      }
    }
  if ( !filename.isEmpty() )
    {
    check_exists_query.prepare ( "SELECT * FROM Images WHERE Filename = ?" );
    check_exists_query.bindValue ( 0, filename );
    check_exists_query.exec();
    if(!check_exists_query.next())
      {
      QSqlQuery statement ( d->Database );
      statement.prepare ( "INSERT INTO Images ( 'SOPInstanceUID', 'Filename', 'SeriesInstanceUID', 'InsertTimestamp' ) VALUES ( ?, ?, ?, ? )" );
      statement.bindValue ( 0, QString ( sopInstanceUID.c_str() ) );
      statement.bindValue ( 1, filename );
      statement.bindValue ( 2, QString ( seriesInstanceUID.c_str() ) );
      statement.bindValue ( 3, QDateTime::currentDateTime() );
      statement.exec();
      }
    }

  if(generateThumbnail){
      if(d->thumbnailGenerator){
        QString studySeriesDirectory = QString(studyInstanceUID.c_str()) + "/" + QString(seriesInstanceUID.c_str());
        //Create thumbnail here
        QString thumbnailPath = databaseDirectory() +
                            "/thumbs/" + this->pathForDataset(dataset) + ".png";
                            //QString(studyInstanceUID.c_str()) + "/" +
                            //QString(seriesInstanceUID.c_str()) + "/" +
                            //QString(sopInstanceUID.c_str()) + ".png";
        QFileInfo thumbnailInfo(thumbnailPath);
        if(!(thumbnailInfo.exists() && (thumbnailInfo.lastModified() > QFileInfo(filename).lastModified()))){
            QDir(databaseDirectory() + "/thumbs/").mkpath(studySeriesDirectory);
            DicomImage dcmImage(QDir::toNativeSeparators(filename).toAscii());
            d->thumbnailGenerator->generateThumbnail(&dcmImage, thumbnailPath);
        }
      }
  }

  if (isInMemory())
    {
      emit databaseChanged();
    }
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


QString ctkDICOMDatabase::pathForDataset( DcmDataset *dataset)
{
  if (!dataset)
    {
    return QString();
    }
  OFString studyInstanceUID, seriesInstanceUID, sopInstanceUID;
  dataset->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID);
  dataset->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID);
  dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);

  return QString(studyInstanceUID.c_str()) + "/" + seriesInstanceUID.c_str() + "/" + sopInstanceUID.c_str();

}
