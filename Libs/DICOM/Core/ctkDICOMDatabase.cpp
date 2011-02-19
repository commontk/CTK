/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

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
#include "ctkDICOMImage.h"

#include "ctkLogger.h"

// DCMTK includes
#ifndef WIN32
#define HAVE_CONFIG_H 
#endif
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/ofstd.h>        /* for class OFStandard */
#include <dcmtk/dcmdata/dcddirif.h>   /* for class DicomDirInterface */
#include <dcmimage.h>

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
  bool executeScript(const QString script);

  QString      DatabaseFileName;
  QString      LastError;
  QSqlDatabase Database;
};

//------------------------------------------------------------------------------
// ctkDICOMDatabasePrivate methods

//------------------------------------------------------------------------------
ctkDICOMDatabasePrivate::ctkDICOMDatabasePrivate(ctkDICOMDatabase& o): q_ptr(&o)
{

}

//------------------------------------------------------------------------------
void ctkDICOMDatabasePrivate::init(QString databaseFilename)
{
  Q_Q(ctkDICOMDatabase);
  q->openDatabase(databaseFilename);
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
  d->Database = QSqlDatabase::addDatabase("QSQLITE",connectionName);
  d->Database.setDatabaseName(databaseFile);
  if ( ! (d->Database.open()) )
    {
    d->LastError = d->Database.lastError().text();
    throw std::runtime_error(qPrintable(d->LastError));
    }
  if ( d->Database.tables().empty() )
    {
      initializeDatabase();
    }
  if (databaseFile != ":memory")
  {
    QFileSystemWatcher* watcher = new QFileSystemWatcher(QStringList(databaseFile),this);
    connect(watcher, SIGNAL( fileChanged(const QString&)),this, SIGNAL ( databaseChanged() ) );
  }
}



//------------------------------------------------------------------------------
// ctkDICOMDatabase methods

//------------------------------------------------------------------------------
ctkDICOMDatabase::ctkDICOMDatabase(QString databaseFile)
   : d_ptr(new ctkDICOMDatabasePrivate(*this))
{
  Q_D(ctkDICOMDatabase);
  d->init(databaseFile);
}

ctkDICOMDatabase::ctkDICOMDatabase()
   : d_ptr(new ctkDICOMDatabasePrivate(*this))
{
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
/*
void ctkDICOMDatabase::insert ( DcmDataset *dataset ) {
  this->insert ( dataset, QString() );
}
*/

//------------------------------------------------------------------------------
void ctkDICOMDatabase::insert ( DcmDataset *dataset, bool storeFile, bool createThumbnail )
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
  dataset->findAndGetOFString(DCM_PatientsName, patientsName);
  dataset->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID);
  dataset->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID);
  dataset->findAndGetOFString(DCM_PatientID, patientID);


  dataset->findAndGetOFString(DCM_PatientsBirthDate, patientsBirthDate);
  dataset->findAndGetOFString(DCM_PatientsBirthTime, patientsBirthTime);
  dataset->findAndGetOFString(DCM_PatientsSex, patientsSex);
  dataset->findAndGetOFString(DCM_PatientsAge, patientsAge);
  dataset->findAndGetOFString(DCM_PatientComments, patientComments);
  dataset->findAndGetOFString(DCM_StudyID, studyID);
  dataset->findAndGetOFString(DCM_StudyDate, studyDate);
  dataset->findAndGetOFString(DCM_StudyTime, studyTime);
  dataset->findAndGetOFString(DCM_AccessionNumber, accessionNumber);
  dataset->findAndGetOFString(DCM_ModalitiesInStudy, modalitiesInStudy);
  dataset->findAndGetOFString(DCM_InstitutionName, institutionName);
  dataset->findAndGetOFString(DCM_PerformingPhysiciansName, performingPhysiciansName);
  dataset->findAndGetOFString(DCM_ReferringPhysiciansName, referringPhysician);
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
      statement.bindValue ( 5, QString ( patientsAge.c_str() ) );
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

  if (createThumbnail)
  {
    QString thumbnailBaseDir = databaseDirectory() + "/thumbs/";
    QString thumbnailFilename = thumbnailBaseDir + "/" + pathForDataset(dataset) + ".png";
    QFileInfo thumbnailInfo(thumbnailFilename);
    if ( ! ( thumbnailInfo.exists() && thumbnailInfo.lastModified() < QFileInfo(filename).lastModified() ) )
    {
      QString studySeriesDirectory = QString(studyInstanceUID.c_str()) + "/" + seriesInstanceUID.c_str();
      QDir(thumbnailBaseDir).mkpath(studySeriesDirectory);
      // TODO: reuse dataset
      DicomImage dcmtkImage(filename.toAscii());
      ctkDICOMImage ctkImage(&dcmtkImage);
      QImage image( ctkImage.frame(0) );
      image.scaled(128,128,Qt::KeepAspectRatio).save(thumbnailFilename,"PNG");
    }
  }

  if (d->DatabaseFileName == ":memory:")
    {
      emit databaseChanged();
    }
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
