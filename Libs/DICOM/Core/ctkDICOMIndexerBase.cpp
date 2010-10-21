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

// ctkDICOM includes
#include "ctkDICOMIndexerBase.h"

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

//------------------------------------------------------------------------------
static ctkLogger logger("org.commontk.dicom.DICOMIndexerBase" );
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class ctkDICOMIndexerBasePrivate
{
public:
  ctkDICOMIndexerBasePrivate();
  ~ctkDICOMIndexerBasePrivate();
  QSqlDatabase db;

};

//------------------------------------------------------------------------------
// ctkDICOMIndexerBasePrivate methods

//------------------------------------------------------------------------------
ctkDICOMIndexerBasePrivate::ctkDICOMIndexerBasePrivate()
{
}

//------------------------------------------------------------------------------
ctkDICOMIndexerBasePrivate::~ctkDICOMIndexerBasePrivate()
{
}

//------------------------------------------------------------------------------
// ctkDICOMIndexerBase methods

//------------------------------------------------------------------------------
ctkDICOMIndexerBase::ctkDICOMIndexerBase()
   : d_ptr(new ctkDICOMIndexerBasePrivate)
{
}

//------------------------------------------------------------------------------
ctkDICOMIndexerBase::~ctkDICOMIndexerBase()
{
}

//------------------------------------------------------------------------------
void ctkDICOMIndexerBase::setDatabase ( QSqlDatabase database ) {
  Q_D(ctkDICOMIndexerBase);
  d->db = database;
}

//------------------------------------------------------------------------------
const QSqlDatabase& ctkDICOMIndexerBase::database () const {
  Q_D(const ctkDICOMIndexerBase);
  return d->db;
}

//------------------------------------------------------------------------------
void ctkDICOMIndexerBase::insert ( DcmDataset *dataset ) {
  this->insert ( dataset, QString() );
}

//------------------------------------------------------------------------------
void ctkDICOMIndexerBase::insert ( DcmDataset *dataset, QString filename ) {
  Q_D(ctkDICOMIndexerBase);

  // Check to see if the file has already been loaded
  QSqlQuery fileExists ( d->db );
  fileExists.prepare("SELECT InsertTimestamp FROM Images WHERE Filename == ?"); 
  fileExists.bindValue(0,filename);
  fileExists.exec();
  if ( fileExists.next() && QFileInfo(filename).lastModified() < QDateTime::fromString(fileExists.value(0).toString(),Qt::ISODate) )
    {
    logger.debug ( "File " + filename + " already added" );
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

  QSqlQuery check_exists_query(d->db);
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
      QSqlQuery statement ( d->db );
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
      QSqlQuery statement ( d->db );
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
      QSqlQuery statement ( d->db );
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
      QSqlQuery statement ( d->db );
      statement.prepare ( "INSERT INTO Images ( 'Filename', 'SeriesInstanceUID', 'InsertTimestamp' ) VALUES ( ?, ?, ? )" );
      statement.bindValue ( 0, filename );
      statement.bindValue ( 1, QString ( seriesInstanceUID.c_str() ) );
      statement.bindValue ( 2, QDateTime::currentDateTime() );
      statement.exec();
      }
    }
}


