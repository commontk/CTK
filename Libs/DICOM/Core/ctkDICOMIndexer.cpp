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
#include <QVariant>
#include <QDate>
#include <QStringList>
#include <QSet>
#include <QFile>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>

// ctkDICOM includes
#include "ctkDICOMIndexer.h"

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

#define MITK_ERROR std::cout
#define MITK_INFO std::cout

//------------------------------------------------------------------------------
class ctkDICOMIndexerPrivate
{
public:
  ctkDICOMIndexerPrivate();
  ~ctkDICOMIndexerPrivate();

};

//------------------------------------------------------------------------------
// ctkDICOMIndexerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::ctkDICOMIndexerPrivate()
{
}

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::~ctkDICOMIndexerPrivate()
{
}

//------------------------------------------------------------------------------
// ctkDICOMIndexer methods

//------------------------------------------------------------------------------
ctkDICOMIndexer::ctkDICOMIndexer():d_ptr(new ctkDICOMIndexerPrivate)
{
}

//------------------------------------------------------------------------------
ctkDICOMIndexer::~ctkDICOMIndexer()
{
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addDirectory(QSqlDatabase database, const QString& directoryName,const QString& destinationDirectoryName)
{
  QSqlDatabase db = database;
  const std::string src_directory(directoryName.toStdString());

  OFList<OFString> originalDcmtkFileNames;
  OFList<OFString> dcmtkFileNames;
  OFStandard::searchDirectoryRecursively( src_directory.c_str(), originalDcmtkFileNames, "", "");

  // hack to reverse list of filenames (not neccessary when image loading works correctly)
  for ( OFListIterator(OFString) iter = originalDcmtkFileNames.begin(); iter != originalDcmtkFileNames.end(); ++iter )
  {
    dcmtkFileNames.push_front( *iter );
  }

  DcmFileFormat fileformat;

  OFListIterator(OFString) iter = dcmtkFileNames.begin();
  OFListIterator(OFString) last = dcmtkFileNames.end();

  if(iter == last) return;

  QSqlQuery query(database);


  /// these are for optimizing the import of image sequences
  /// since most information are identical for all slices
  OFString lastPatientID = "";
  OFString lastPatientsName = "";
  OFString lastPatientsBirthDate = "";
  OFString lastStudyInstanceUID = "";
  OFString lastSeriesInstanceUID = "";
  int lastPatientUID = -1;

  /* iterate over all input filenames */
  while (iter != last)
  {
    std::string filename((*iter).c_str());
    QString qfilename(filename.c_str()); 
    /// first we check if the file is already in the database
    QSqlQuery fileExists(database);
    fileExists.prepare("SELECT InsertTimestamp FROM Images WHERE Filename == ?"); 
    fileExists.bindValue(0,qfilename);
    fileExists.exec();
    if (
      fileExists.next() && 
      QFileInfo(qfilename).lastModified() < QDateTime::fromString(fileExists.value(0).toString(),Qt::ISODate)      
      )
      {
      MITK_INFO << "File " << filename << " already added.";
      continue;
      }

    MITK_INFO << filename << "\n";
    OFCondition status = fileformat.loadFile(filename.c_str());
    ++iter;

    if (!status.good())
    {
      MITK_ERROR << "Could not load " << filename << "\nDCMTK says: " << status.text();
      continue;
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

    //The patient UID is a unique number within the database, generated by the sqlite autoincrement
    int patientUID = -1;

    //If the following fields can not be evaluated, cancel evaluation of the DICOM file
    if (!fileformat.getDataset()->findAndGetOFString(DCM_PatientsName, patientsName).good())
    {
      MITK_ERROR << "Could not read DCM_PatientsName from " << filename;
      continue;
    }

    if (!fileformat.getDataset()->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID).good())
    {
      MITK_ERROR << "Could not read DCM_StudyInstanceUID from " << filename;
      continue;
    }

    if (!fileformat.getDataset()->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID).good())
    {
      MITK_ERROR << "Could not read DCM_SeriesInstanceUID from " << filename;
      continue;
    }
    if (!fileformat.getDataset()->findAndGetOFString(DCM_InstanceNumber, instanceNumber).good())
    {
      MITK_ERROR << "Could not read DCM_InstanceNumber from " << filename;
      continue;
    }


    fileformat.getDataset()->findAndGetOFString(DCM_PatientID, patientID);
    fileformat.getDataset()->findAndGetOFString(DCM_PatientsBirthDate, patientsBirthDate);
    fileformat.getDataset()->findAndGetOFString(DCM_PatientsBirthTime, patientsBirthTime);
    fileformat.getDataset()->findAndGetOFString(DCM_PatientsSex, patientsSex);
    fileformat.getDataset()->findAndGetOFString(DCM_PatientsAge, patientsAge);
    fileformat.getDataset()->findAndGetOFString(DCM_PatientComments, patientComments);
    fileformat.getDataset()->findAndGetOFString(DCM_StudyID, studyID);
    fileformat.getDataset()->findAndGetOFString(DCM_StudyDate, studyDate);
    fileformat.getDataset()->findAndGetOFString(DCM_StudyTime, studyTime);
    fileformat.getDataset()->findAndGetOFString(DCM_AccessionNumber, accessionNumber);
    fileformat.getDataset()->findAndGetOFString(DCM_ModalitiesInStudy, modalitiesInStudy);
    fileformat.getDataset()->findAndGetOFString(DCM_InstitutionName, institutionName);
    fileformat.getDataset()->findAndGetOFString(DCM_PerformingPhysiciansName, performingPhysiciansName);
    fileformat.getDataset()->findAndGetOFString(DCM_ReferringPhysiciansName, referringPhysician);
    fileformat.getDataset()->findAndGetOFString(DCM_StudyDescription, studyDescription);

    fileformat.getDataset()->findAndGetOFString(DCM_SeriesDate, seriesDate);
    fileformat.getDataset()->findAndGetOFString(DCM_SeriesTime, seriesTime);
    fileformat.getDataset()->findAndGetOFString(DCM_SeriesDescription, seriesDescription);
    fileformat.getDataset()->findAndGetOFString(DCM_BodyPartExamined, bodyPartExamined);
    fileformat.getDataset()->findAndGetOFString(DCM_FrameOfReferenceUID, frameOfReferenceUID);
    fileformat.getDataset()->findAndGetOFString(DCM_ContrastBolusAgent, contrastAgent);
    fileformat.getDataset()->findAndGetOFString(DCM_ScanningSequence, scanningSequence);

    fileformat.getDataset()->findAndGetSint32(DCM_SeriesNumber, seriesNumber);
    fileformat.getDataset()->findAndGetSint32(DCM_AcquisitionNumber, acquisitionNumber);
    fileformat.getDataset()->findAndGetSint32(DCM_EchoNumbers, echoNumber);
    fileformat.getDataset()->findAndGetSint32(DCM_TemporalPositionIdentifier, temporalPosition);

    MITK_INFO << "Adding new items to database:";
    MITK_INFO << "studyID: " << studyID;
    MITK_INFO << "seriesInstanceUID: " << seriesInstanceUID;
    MITK_INFO << "Patient's Name: " << patientsName;

    //-----------------------
    //Add Patient to Database
    //-----------------------

    //Speed up: Check if patient is the same as in last file; very probable, as all images belonging to a study have the same patient
    bool patientExists = false;
    if(lastPatientID.compare(patientID) || lastPatientsBirthDate.compare(patientsBirthDate) || lastPatientsName.compare(patientsName))
    {
      //Check if patient is already present in the db
      QSqlQuery check_exists_query(database);
      std::stringstream check_exists_query_string;
      check_exists_query_string << "SELECT * FROM Patients WHERE PatientID = '" << patientID << "'";
      check_exists_query.exec(check_exists_query_string.str().c_str());

      /// we check only patients with the same PatientID
      /// PatientID is not unique in DICOM, so we also compare Name and BirthDate
      /// and assume this is sufficient
      while (check_exists_query.next())
      {
        if (
            check_exists_query.record().value("PatientsName").toString() == patientsName.c_str() &&
            check_exists_query.record().value("PatientsBirthDate").toString() == patientsBirthDate.c_str()
           )
        {
          /// found it
          patientUID = check_exists_query.value(check_exists_query.record().indexOf("UID")).toInt();
          break;
        }
      }

      if(!patientExists)
      {

        std::stringstream query_string;

        query_string << "INSERT INTO Patients VALUES( NULL,'" 
        << patientsName << "','" 
        << patientID << "','" 
        << patientsBirthDate << "','"
        << patientsBirthTime << "','" 
        << patientsSex << "','" 
        << patientsAge << "','" 
        << patientComments << "')";

        query.exec(query_string.str().c_str());

        patientUID = query.lastInsertId().toInt();
        MITK_INFO << "New patient inserted: " << patientUID << "\n";
      }
    }
    else 
      {
      patientUID = lastPatientUID;
      }     
    
    /// keep this for the next image
    lastPatientUID = patientUID;
    lastPatientID = patientID;
    lastPatientsBirthDate = patientsBirthDate;
    lastPatientsName = patientsName;

    //---------------------
    //Add Study to Database
    //---------------------

    if(lastStudyInstanceUID.compare(studyInstanceUID))
    {
      QSqlQuery check_exists_query(database);
      std::stringstream check_exists_query_string;
      check_exists_query_string << "SELECT * FROM Studies WHERE StudyInstanceUID = '" << studyInstanceUID << "'";
      check_exists_query.exec(check_exists_query_string.str().c_str());

      if(!check_exists_query.next())
      {

        std::stringstream query_string;

        query_string << "INSERT INTO Studies VALUES('"
          << studyInstanceUID << "','" 
          << patientUID << "','" 
          << studyID << "','"
          << QDate::fromString(studyDate.c_str(), "yyyyMMdd").toString("yyyy-MM-dd").toStdString() << "','"
          << studyTime << "','" 
          << accessionNumber << "','" 
          << modalitiesInStudy << "','" 
          << institutionName << "','" 
          << referringPhysician << "','" 
          << performingPhysiciansName << "','" 
          << studyDescription << "')";

        query.exec(query_string.str().c_str());
      }
    }

    lastStudyInstanceUID = studyInstanceUID;

    //----------------------
    //Add Series to Database
    //----------------------

    if(lastSeriesInstanceUID.compare(seriesInstanceUID))
    {

      QSqlQuery check_exists_query(database);
      std::stringstream check_exists_query_string;
      check_exists_query_string << "SELECT * FROM Series WHERE SeriesInstanceUID = '" << seriesInstanceUID << "'";
      check_exists_query.exec(check_exists_query_string.str().c_str());

      if(!check_exists_query.next())
      {

        std::stringstream query_string;

        query_string << "INSERT INTO Series VALUES('"
          << seriesInstanceUID << "','"
          << studyInstanceUID << "','"
          << static_cast<int>(seriesNumber) << "','"
          << QDate::fromString(seriesDate.c_str(), "yyyyMMdd").toString("yyyy-MM-dd").toStdString() << "','"
          << seriesTime << "','"
          << seriesDescription << "','"
          << bodyPartExamined << "','"
          << frameOfReferenceUID << "','"
          << static_cast<int>(acquisitionNumber) << "','"
          << contrastAgent << "','"
          << scanningSequence << "','"
          << static_cast<int>(echoNumber) << "','"
          << static_cast<int>(temporalPosition) << "')";

        query.exec(query_string.str().c_str());
      }
    }

    lastSeriesInstanceUID = seriesInstanceUID;


    //----------------------------------
    //Move file to destination directory
    //----------------------------------

    if (!destinationDirectoryName.isEmpty())
      {
      QFile currentFile( qfilename );
      QDir destinationDir(destinationDirectoryName);

      QString uniqueDirName = QString(studyInstanceUID.c_str()) + "/" + seriesInstanceUID.c_str();
      qDebug() << "MKPath: " << uniqueDirName;
      destinationDir.mkpath(uniqueDirName);
      QString destFileName = destinationDir.absolutePath().append("/").append(instanceNumber.c_str());
      qDebug() << "Copy: " << qfilename << " -> " << destFileName;
      currentFile.copy(destFileName);
      //for testing only: copy file instead of moving
      //currentFile.copyTo(destDirectoryPath.str());
    }
    // */
    //------------------------
    //Add Filename to Database
    //------------------------

//    std::stringstream relativeFilePath;
//    relativeFilePath << seriesInstanceUID.c_str() << "/" << currentFilePath.getFileName();

    QSqlQuery check_exists_query(database);
    std::stringstream check_exists_query_string;
//    check_exists_query_string << "SELECT * FROM Images WHERE Filename = '" << relativeFilePath.str() << "'";
    check_exists_query_string << "SELECT * FROM Images WHERE Filename = '" << filename << "'";
    check_exists_query.exec(check_exists_query_string.str().c_str());

    if(!check_exists_query.next())
    {
      std::stringstream query_string;

      //To save absolute path: destDirectoryPath.str()
      query_string << "INSERT INTO Images VALUES('"
        << /*relativeFilePath.str()*/ filename << "','" << seriesInstanceUID << "','" << QDateTime::currentDateTime().toString(Qt::ISODate).toStdString() << "')";

      query.exec(query_string.str().c_str());
    }
  }

  // db.commit();
  // db.close();

}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::refreshDatabase(QSqlDatabase database, const QString& directoryName)
{
  /// get all filenames from the database
  QSqlQuery allFilesQuery(database);
  QStringList databaseFileNames;
  QStringList filesToRemove;
  allFilesQuery.exec("SELECT Filename from Images;");

  while (allFilesQuery.next())
    {
    QString fileName = allFilesQuery.value(0).toString();
    databaseFileNames.append(fileName);
    if (! QFile::exists(fileName) ) 
      {
      filesToRemove.append(fileName);
      }
    }

  QSet<QString> filesytemFiles;
  QDirIterator dirIt(directoryName);
  while (dirIt.hasNext()) 
    {
    filesytemFiles.insert(dirIt.next());
    }
}
