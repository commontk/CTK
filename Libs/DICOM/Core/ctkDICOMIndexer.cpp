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
#include <QPixmap>


// ctkDICOM includes
#include "ctkLogger.h"
#include "ctkDICOMIndexer.h"
#include "ctkDICOMAbstractThumbnailGenerator.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/ofstd.h>        /* for class OFStandard */
#include <dcmtk/dcmdata/dcddirif.h>   /* for class DicomDirInterface */
#include <dcmtk/dcmimgle/dcmimage.h>  /* for class DicomImage */
#include <dcmtk/dcmimage/diregist.h>  /* include support for color images */


//------------------------------------------------------------------------------
static ctkLogger logger("org.commontk.dicom.DICOMIndexer" );
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class ctkDICOMIndexerPrivate
{
public:
  ctkDICOMIndexerPrivate();
  ~ctkDICOMIndexerPrivate();

  ctkDICOMAbstractThumbnailGenerator* thumbnailGenerator;

  /// these are for optimizing the import of image sequences
  /// since most information are identical for all slices
  OFString lastPatientID;
  OFString lastPatientsName;
  OFString lastPatientsBirthDate;
  OFString lastStudyInstanceUID;
  OFString lastSeriesInstanceUID;
  int lastPatientUID;
};

//------------------------------------------------------------------------------
// ctkDICOMIndexerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::ctkDICOMIndexerPrivate()
{
    this->thumbnailGenerator = NULL;

    this->lastPatientID = "";
    this->lastPatientsName = "";
    this->lastPatientsBirthDate = "";
    this->lastStudyInstanceUID = "";
    this->lastSeriesInstanceUID = "";
    this->lastPatientUID = -1;
}

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::~ctkDICOMIndexerPrivate()
{
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ctkDICOMIndexer methods

//------------------------------------------------------------------------------
ctkDICOMIndexer::ctkDICOMIndexer(QObject *parent):d_ptr(new ctkDICOMIndexerPrivate)
{
  Q_UNUSED(parent);
}

//------------------------------------------------------------------------------
ctkDICOMIndexer::~ctkDICOMIndexer()
{
}

//------------------------------------------------------------------------------
bool ctkDICOMIndexer::loggedExec(QSqlQuery& query)
{
  return (this->loggedExec(query, QString("")));
}

//------------------------------------------------------------------------------
bool ctkDICOMIndexer::loggedExec(QSqlQuery& query, const QString& queryString)
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
void ctkDICOMIndexer::addFile(ctkDICOMDatabase& ctkDICOMDatabase, 
                                   const QString& filePath,
                                   const QString& destinationDirectoryName,
                                   bool createHierarchy,
                                   bool createThumbnails)
{
  Q_D(ctkDICOMIndexer);

  logger.setDebug();

  DcmFileFormat fileformat;
  DcmDataset *dataset;


  std::string filename = filePath.toStdString();

  emit indexingFilePath(filePath);

  /// first we check if the file is already in the database
  QSqlQuery check_filename_query(ctkDICOMDatabase.database());
  check_filename_query.prepare("SELECT InsertTimestamp FROM Images WHERE Filename == ?");
  check_filename_query.bindValue(0,filePath);
  this->loggedExec(check_filename_query);
  if (
    check_filename_query.next() &&
    QFileInfo(filePath).lastModified() < QDateTime::fromString(check_filename_query.value(0).toString(),Qt::ISODate)
    )
    {
    logger.debug( "File " + filePath + " already added.");
    return;
    }
  check_filename_query.finish();

  logger.debug( "Processing " + filePath ); 
  OFCondition status = fileformat.loadFile(filename.c_str());

  dataset = fileformat.getDataset();

  if (!status.good())
  {
    logger.error( "Could not load " + filePath );
    logger.error( "DCMTK says: " + QString(status.text()) );
    return;
  }

  OFString patientsName, patientID, patientsBirthDate, patientsBirthTime, patientsSex,
    patientComments, patientsAge;

  OFString studyInstanceUID, studyID, studyDate, studyTime,
    accessionNumber, modalitiesInStudy, institutionName, performingPhysiciansName, referringPhysician, studyDescription;

  OFString seriesInstanceUID, seriesDate, seriesTime,
    seriesDescription, bodyPartExamined, frameOfReferenceUID,
    contrastAgent, scanningSequence;
  OFString instanceNumber, sopInstanceUID ;

  Sint32 seriesNumber = 0, acquisitionNumber = 0, echoNumber = 0, temporalPosition = 0;

  //The patient UID is a unique number within the database, generated by the sqlite autoincrement
  //Thus, this is _not_ the DICOM Patient ID.
  int patientUID = -1;

  //If the following fields can not be evaluated, cancel evaluation of the DICOM file
  if (!dataset->findAndGetOFString(DCM_PatientName, patientsName).good())
  {
    logger.error( "Could not read DCM_PatientName from " + filePath );
    return;
  }

  if (!dataset->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID).good())
  {
    logger.error( "Could not read DCM_StudyInstanceUID from " + filePath );
    return;
  }

  if (!dataset->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID).good())
  {
    logger.error( "Could not read DCM_SeriesInstanceUID from " + filePath );
    return;
  }

  if (!dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID).good())
  {
    logger.error( "Could not read DCM_SOPInstanceUID from " + filePath );
    return;
  }
  if (!dataset->findAndGetOFString(DCM_InstanceNumber, instanceNumber).good())
  {
    logger.error( "Could not read DCM_InstanceNumber from " + filePath );
    return;
  }


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

  logger.debug( "Adding new items to database:" );
  logger.debug( "studyID: " + QString(studyID.c_str()) );
  logger.debug( "seriesInstanceUID: " + QString(seriesInstanceUID.c_str()) );
  logger.debug( "Patient's Name: " + QString(patientsName.c_str()) );

  //-----------------------
  //Add Patient to Database
  //-----------------------

  //Speed up: Check if patient is the same as in last file; very probable, as all images belonging to a study have the same patient
  bool patientExists = false;
  if(d->lastPatientID.compare(patientID) || d->lastPatientsBirthDate.compare(patientsBirthDate) || d->lastPatientsName.compare(patientsName))
  {
    //Check if patient is already present in the db
    QSqlQuery check_exists_query(ctkDICOMDatabase.database());
    std::stringstream check_exists_query_string;
    check_exists_query_string << "SELECT * FROM Patients WHERE PatientID = '" << patientID << "'";
    this->loggedExec(check_exists_query, check_exists_query_string.str().c_str());

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
        patientExists = true;
        break;
      }
    }
    check_exists_query.finish();

    if(!patientExists)
    {

      QSqlQuery insert_patient_query(ctkDICOMDatabase.database());

      std::stringstream query_string;

      query_string << "INSERT INTO Patients VALUES( NULL,'"
      << patientsName << "','"
      << patientID << "','"
      << patientsBirthDate << "','"
      << patientsBirthTime << "','"
      << patientsSex << "','"
      << patientsAge << "','"
      << patientComments << "')";

      this->loggedExec(insert_patient_query, query_string.str().c_str());

      patientUID = insert_patient_query.lastInsertId().toInt();
      insert_patient_query.finish();
      QString patientUIDQString;
      patientUIDQString.setNum(patientUID);
      logger.debug( "New patient inserted: " + patientUIDQString );
    }
  }
  else
    {
    patientUID = d->lastPatientUID;
    }

  /// keep this for the next image
  d->lastPatientUID = patientUID;
  d->lastPatientID = patientID;
  d->lastPatientsBirthDate = patientsBirthDate;
  d->lastPatientsName = patientsName;

  //---------------------
  //Add Study to Database
  //---------------------

  if(d->lastStudyInstanceUID.compare(studyInstanceUID))
  {
    QSqlQuery check_exists_query(ctkDICOMDatabase.database());
    std::stringstream check_exists_query_string;
    check_exists_query_string << "SELECT * FROM Studies WHERE StudyInstanceUID = '" << studyInstanceUID << "'";
    this->loggedExec(check_exists_query, check_exists_query_string.str().c_str());

    logger.debug( "Checking for study: " + QString(studyInstanceUID.c_str()) );

    if(!check_exists_query.next())
    {

      QSqlQuery insert_query(ctkDICOMDatabase.database());
      std::stringstream query_string;

      // TODO: all INSERTS should be changed to use the prepare/bindValue methods
      // to avoid quoting issues
      insert_query.prepare("INSERT INTO Studies (StudyInstanceUID, PatientsUID, StudyID, StudyDate, StudyTime, AccessionNumber, ModalitiesInStudy, InstitutionName, ReferringPhysician, PerformingPhysiciansName, StudyDescription) VALUES (:StudyInstanceUID, :PatientsUID, :StudyID, :StudyDate, :StudyTime, :AccessionNumber, :ModalitiesInStudy, :InstitutionName, :ReferringPhysician, :PerformingPhysiciansName, :StudyDescription)");

      insert_query.bindValue(":StudyInstanceUID", QString(studyInstanceUID.c_str()));
      insert_query.bindValue(":PatientsUID", patientUID);
      insert_query.bindValue(":StudyID", QString(studyID.c_str()));
      insert_query.bindValue(":StudyDate", QDate::fromString(studyDate.c_str(), "yyyyMMdd").toString("yyyy-MM-dd"));
      insert_query.bindValue(":StudyTime", QString(studyTime.c_str()));
      insert_query.bindValue(":AccessionNumber", QString(accessionNumber.c_str()));
      insert_query.bindValue(":ModalitiesInStudy", QString(modalitiesInStudy.c_str()));
      insert_query.bindValue(":InstitutionName", QString(institutionName.c_str()));
      insert_query.bindValue(":ReferringPhysician", QString(referringPhysician.c_str()));
      insert_query.bindValue(":PerformingPhysiciansName", QString(performingPhysiciansName.c_str()));
      insert_query.bindValue(":StudyDescription", QString(studyDescription.c_str()));

      this->loggedExec(insert_query);
      logger.debug( "Inserted study: " + QString(studyInstanceUID.c_str()) );
    }
  }

  d->lastStudyInstanceUID = studyInstanceUID;

  //----------------------
  //Add Series to Database
  //----------------------

  if(d->lastSeriesInstanceUID.compare(seriesInstanceUID))
  {

    QSqlQuery check_exists_query(ctkDICOMDatabase.database());
    std::stringstream check_exists_query_string;
    check_exists_query_string << "SELECT * FROM Series WHERE SeriesInstanceUID = '" << seriesInstanceUID << "'";
    this->loggedExec(check_exists_query, check_exists_query_string.str().c_str());

    logger.debug( "Checking series: " + QString(seriesInstanceUID.c_str()) );

    if(!check_exists_query.next())
    {

      QSqlQuery insert_query(ctkDICOMDatabase.database());
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

      this->loggedExec(insert_query, query_string.str().c_str());
      logger.debug( "Inserted series: " + QString(seriesInstanceUID.c_str()) );
    }
  }

  d->lastSeriesInstanceUID = seriesInstanceUID;

  QString studySeriesDirectory = QString(studyInstanceUID.c_str()) + "/" + seriesInstanceUID.c_str();

  //----------------------------------
  //Move file to destination directory
  //----------------------------------

  QString finalFilePath(filePath);
  if (!destinationDirectoryName.isEmpty())
  {
    QFile currentFile( filePath );
    QDir destinationDir(destinationDirectoryName + "/dicom");
    finalFilePath = sopInstanceUID.c_str();
    if (createHierarchy)
    {
      destinationDir.mkpath(studySeriesDirectory);
      finalFilePath.prepend( destinationDir.absolutePath() + "/"  + studySeriesDirectory + "/" );
    }
    currentFile.copy(finalFilePath);
    logger.debug( "Copy file from: " + filePath );
    logger.debug( "Copy file to  : " + finalFilePath );
  }
  logger.debug(QString("finalFilePath: ") + finalFilePath);

  if (createThumbnails)
  {
    if(d->thumbnailGenerator)
    {
      QString thumbnailBaseDir =  ctkDICOMDatabase.databaseDirectory() + "/thumbs/";
      QString thumbnailFilename = thumbnailBaseDir + "/" + ctkDICOMDatabase.pathForDataset(dataset) + ".png";
      QFileInfo thumbnailInfo(thumbnailFilename);
      if ( ! ( thumbnailInfo.exists() && thumbnailInfo.lastModified() < QFileInfo(finalFilePath).lastModified() ) )
      {
        QDir(thumbnailBaseDir).mkpath(studySeriesDirectory);
        DicomImage dcmtkImage(QDir::toNativeSeparators(finalFilePath).toStdString().c_str());
        d->thumbnailGenerator->generateThumbnail(&dcmtkImage, thumbnailFilename);
      }
    }
  }

  //------------------------
  //Add Filename to Database
  //------------------------

//    std::stringstream relativeFilePath;
//    relativeFilePath << seriesInstanceUID.c_str() << "/" << currentFilePath.getFileName();

  logger.debug(QString("Adding file path to dabase: ") + finalFilePath);

  QSqlQuery check_exists_query(ctkDICOMDatabase.database());
  std::stringstream check_exists_query_string;
//    check_exists_query_string << "SELECT * FROM Images WHERE Filename = '" << relativeFilePath.str() << "'";
  check_exists_query_string << "SELECT * FROM Images WHERE SOPInstanceUID = '" << sopInstanceUID << "'";
  this->loggedExec(check_exists_query, check_exists_query_string.str().c_str());

  if(!check_exists_query.next())
  {
    QSqlQuery insert_query(ctkDICOMDatabase.database());
    std::stringstream query_string;

    //To save absolute path: destDirectoryPath.str()
    query_string << "INSERT INTO Images VALUES('"
      << sopInstanceUID << "','" << finalFilePath.toStdString() << "','" << seriesInstanceUID << "','" << QDateTime::currentDateTime().toString(Qt::ISODate).toStdString() << "')";

    this->loggedExec(insert_query, query_string.str().c_str());
    logger.debug(QString("added file path to dabase: ") + query_string.str().c_str());
  }

}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addDirectory(ctkDICOMDatabase& ctkDICOMDatabase, 
                                   const QString& directoryName,
                                   const QString& destinationDirectoryName,
                                   bool createHierarchy,
                                   bool createThumbnails)
{
  const std::string src_directory(directoryName.toStdString());

  OFList<OFString> originalDcmtkFileNames;
  OFList<OFString> dcmtkFileNames;
  OFStandard::searchDirectoryRecursively( QDir::toNativeSeparators(src_directory.c_str()).toAscii().data(), originalDcmtkFileNames, "", "");

  // hack to reverse list of filenames (not neccessary when image loading works correctly)
  for ( OFListIterator(OFString) iter = originalDcmtkFileNames.begin(); iter != originalDcmtkFileNames.end(); ++iter )
  {
    dcmtkFileNames.push_front( *iter );
  }

  OFListIterator(OFString) iter = dcmtkFileNames.begin();
  OFListIterator(OFString) last = dcmtkFileNames.end();

  if(iter == last) return;

  emit foundFilesToIndex(dcmtkFileNames.size());

  /* iterate over all input filenames */
  int fileNumber = 0;
  while (iter != last)
  {
    emit indexingFileNumber(++fileNumber);
    QString filePath((*iter).c_str());
    this->addFile(ctkDICOMDatabase, filePath, destinationDirectoryName, createHierarchy, createThumbnails);
    ++iter;
  }
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::refreshDatabase(ctkDICOMDatabase& ctkDICOMDatabase, const QString& directoryName)
{
  /// get all filenames from the database
  QSqlQuery allFilesQuery(ctkDICOMDatabase.database());
  QStringList databaseFileNames;
  QStringList filesToRemove;
  this->loggedExec(allFilesQuery, "SELECT Filename from Images;");

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
  
  // TODO: it looks like this function was never finished...
  // 
  // I guess the next step is to remove all filesToRemove from the database
  // and also to add filesystemFiles into the database tables
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::setThumbnailGenerator(ctkDICOMAbstractThumbnailGenerator *generator){
    Q_D(ctkDICOMIndexer);
    d->thumbnailGenerator = generator;
}

//------------------------------------------------------------------------------
ctkDICOMAbstractThumbnailGenerator* ctkDICOMIndexer::thumbnailGenerator(){
    Q_D(ctkDICOMIndexer);
    return d->thumbnailGenerator;
}

