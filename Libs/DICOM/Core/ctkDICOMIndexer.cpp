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

// ctkDICOM includes
#include "ctkLogger.h"
#include "ctkDICOMIndexer.h"
#include "ctkDICOMIndexer_p.h"
#include "ctkDICOMDatabase.h"

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

/// How many files to parse before inserting results into the database.
/// Increasing cache size increases maximum memory usage, very low cache size
/// slows down database insertion.
static int REQUEST_RESULTS_CACHE_MAXIMUM_SIZE = 5000;
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// ctkDICOMIndexerPrivate methods


//------------------------------------------------------------------------------
ctkDICOMIndexerPrivateWorker::ctkDICOMIndexerPrivateWorker(DICOMIndexingQueue* queue)
: RequestQueue(queue)
, TimePercentageIndexing(95.0)
, RemainingRequestCount(0)
, CompletedRequestCount(0)
{
}

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivateWorker::~ctkDICOMIndexerPrivateWorker()
{
  this->RequestQueue->setStopRequested(true);
}

//------------------------------------------------------------------------------
void ctkDICOMIndexerPrivateWorker::start()
{
  emit updatingDatabase(true);
  ctkDICOMDatabase database;
  database.openDatabase(this->RequestQueue->databaseFilename());
  database.setTagsToPrecache(this->RequestQueue->tagsToPrecache());
  database.setTagsToExcludeFromStorage(this->RequestQueue->tagsToExcludeFromStorage());

  int patientsCountBefore = database.patientsCount();
  int studiesCountBefore = database.studiesCount();
  int seriesCountBefore = database.seriesCount();
  int imagesCountBefore = database.imagesCount();
  int patientsCountAfter = patientsCountBefore;
  int studiesCountAfter = studiesCountBefore;
  int seriesCountAfter = seriesCountBefore;
  int imagesCountAfter = imagesCountBefore;

  do
  {
    emit progressStep("Parsing DICOM files");
    emit progress(0);
    // Make a local copy to avoid the need of frequent locking
    this->RequestQueue->modifiedTimeForFilepath(this->ModifiedTimeForFilepath);
    this->CompletedRequestCount = 0;
    do
    {
      if (this->RequestQueue->isStopRequested())
      {
        this->RequestQueue->clear();
        this->RequestQueue->setStopRequested(false);
      }
      DICOMIndexingQueue::IndexingRequest indexingRequest;
      this->RemainingRequestCount = this->RequestQueue->popIndexingRequest(indexingRequest);
      this->processIndexingRequest(indexingRequest, database);
      this->CompletedRequestCount++;
    } while (!this->RequestQueue->isEmpty());

    QTime timeProbe;
    timeProbe.start();

    // Update displayed fields according to inserted DICOM datasets
    emit progressStep("Updating database displayed fields");
    emit progress(this->TimePercentageIndexing);

    database.updateDisplayedFields();
    patientsCountAfter = database.patientsCount();
    studiesCountAfter = database.studiesCount();
    seriesCountAfter = database.seriesCount();
    imagesCountAfter = database.imagesCount();

    double elapsedTimeInSeconds = timeProbe.elapsed() / 1000.0;
    qDebug() << QString("DICOM indexer has updated display fields for %1 files [%2s]")
      .arg(imagesCountAfter-imagesCountBefore).arg(QString::number(elapsedTimeInSeconds, 'f', 2));

  // restart if new requests has been queued during displayed fields update
  } while (!this->RequestQueue->isEmpty());

  database.closeDatabase();
  emit updatingDatabase(false);

  this->RequestQueue->setIndexing(false);
  emit progress(100);
  emit progressStep("Indexing complete");
  emit indexingComplete(patientsCountAfter - patientsCountBefore, studiesCountAfter-studiesCountBefore,
    seriesCountAfter-seriesCountBefore, imagesCountAfter - imagesCountBefore);
}

//------------------------------------------------------------------------------
void ctkDICOMIndexerPrivateWorker::processIndexingRequest(DICOMIndexingQueue::IndexingRequest& indexingRequest, ctkDICOMDatabase& database)
{
  if (!indexingRequest.inputFolderPath.isEmpty())
  {
    QDir::Filters filters = QDir::Files;
    if (indexingRequest.includeHidden)
    {
      filters |= QDir::Hidden;
    }
    QDirIterator it(indexingRequest.inputFolderPath, filters, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
      indexingRequest.inputFilesPath << it.next();
    }
  }

  QTime timeProbe;
  timeProbe.start();

  int currentFileIndex = 0;
  int alreadyAddedFileCount = 0;
  QStringList alreadyAddedFiles;
  foreach(const QString& filePath, indexingRequest.inputFilesPath)
  {
    int percent = int(this->TimePercentageIndexing * (this->CompletedRequestCount + double(currentFileIndex++) / double(indexingRequest.inputFilesPath.size()))
      / double(this->CompletedRequestCount + this->RemainingRequestCount + 1));
    emit this->progress(percent);
    emit progressDetail(filePath);

    QDateTime fileModifiedTime = QFileInfo(filePath).lastModified();
    bool datasetAlreadyInDatabase = this->ModifiedTimeForFilepath.contains(filePath);
    if (datasetAlreadyInDatabase && this->ModifiedTimeForFilepath[filePath] >= fileModifiedTime)
    {
      alreadyAddedFileCount++;
      if (alreadyAddedFileCount < 10)
      {
        alreadyAddedFiles << filePath;
      }
      continue;
    }
    this->ModifiedTimeForFilepath[filePath] = fileModifiedTime;

    ctkDICOMDatabase::IndexingResult indexingResult;
    indexingResult.dataset = QSharedPointer<ctkDICOMItem>(new ctkDICOMItem);
    indexingResult.dataset->InitializeFromFile(filePath);
    if (indexingResult.dataset->IsInitialized())
    {
      indexingResult.filePath = filePath;
      indexingResult.copyFile = indexingRequest.copyFile;
      indexingResult.overwriteExistingDataset = datasetAlreadyInDatabase;
      int resultsCount = this->RequestQueue->pushIndexingResult(indexingResult);
      if (resultsCount >= REQUEST_RESULTS_CACHE_MAXIMUM_SIZE)
      {
        emit progressStep("Updating database fields");
        this->writeIndexingResultsToDatabase(database);
        emit progressStep("Parsing DICOM files");
      }
    }
    else
    {
      logger.warn(QString("Could not read DICOM file:") + filePath);
    }

    if (this->RequestQueue->isStopRequested())
    {
      break;
    }
  }

  if (alreadyAddedFileCount > 0)
  {
    logger.debug(QString("Skipped %1 files that were already in the database: %2...").arg(
      alreadyAddedFileCount).arg(alreadyAddedFiles.join(", ")));
  }

  if (this->RequestQueue->isIndexingRequestsEmpty())
  {
    emit progressStep("Updating database fields");
    this->writeIndexingResultsToDatabase(database);
    emit progressStep("Parsing DICOM files");
  }

  float elapsedTimeInSeconds = timeProbe.elapsed() / 1000.0;
  qDebug() << QString("DICOM indexer has successfully processed %1 files [%2s]")
    .arg(currentFileIndex).arg(QString::number(elapsedTimeInSeconds, 'f', 2));
}


//------------------------------------------------------------------------------
void ctkDICOMIndexerPrivateWorker::writeIndexingResultsToDatabase(ctkDICOMDatabase& database)
{
  QList<ctkDICOMDatabase::IndexingResult> indexingResults;
  this->RequestQueue->popAllIndexingResults(indexingResults);
  if (indexingResults.isEmpty())
  {
    return;
  }

  QTime timeProbe;
  timeProbe.start();

  this->NumberOfInstancesToInsert = indexingResults.size();
  this->NumberOfInstancesInserted = 0;
  database.insert(indexingResults);
  this->NumberOfInstancesToInsert = 0;
  this->NumberOfInstancesInserted = 0;

  float elapsedTimeInSeconds = timeProbe.elapsed() / 1000.0;
  qDebug() << QString("DICOM indexer has successfully inserted %1 files [%2s]")
    .arg(indexingResults.count()).arg(QString::number(elapsedTimeInSeconds, 'f', 2));

}

//------------------------------------------------------------------------------
// ctkDICOMIndexerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::ctkDICOMIndexerPrivate(ctkDICOMIndexer& o)
  : q_ptr(&o)
  , Database(nullptr)
  , BackgroundImportEnabled(false)
{
  ctkDICOMIndexerPrivateWorker* worker = new ctkDICOMIndexerPrivateWorker(&this->RequestQueue);
  worker->moveToThread(&this->WorkerThread);
  
  connect(&this->WorkerThread, &QThread::finished, worker, &QObject::deleteLater);
  connect(this, &ctkDICOMIndexerPrivate::startWorker, worker, &ctkDICOMIndexerPrivateWorker::start);

  // Progress report
  connect(worker, &ctkDICOMIndexerPrivateWorker::progress, q_ptr, &ctkDICOMIndexer::progress);
  connect(worker, &ctkDICOMIndexerPrivateWorker::progressDetail, q_ptr, &ctkDICOMIndexer::progressDetail);
  connect(worker, &ctkDICOMIndexerPrivateWorker::progressStep, q_ptr, &ctkDICOMIndexer::progressStep);
  connect(worker, &ctkDICOMIndexerPrivateWorker::updatingDatabase, q_ptr, &ctkDICOMIndexer::updatingDatabase);
  connect(worker, &ctkDICOMIndexerPrivateWorker::indexingComplete, q_ptr, &ctkDICOMIndexer::indexingComplete);

  this->WorkerThread.start();
}

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::~ctkDICOMIndexerPrivate()
{
  Q_Q(ctkDICOMIndexer);
  this->RequestQueue.setStopRequested(true);
  this->WorkerThread.quit();
  this->WorkerThread.wait();
  q->setDatabase(nullptr);
}

//------------------------------------------------------------------------------
void ctkDICOMIndexerPrivate::pushIndexingRequest(const DICOMIndexingQueue::IndexingRequest& request)
{
  Q_Q(ctkDICOMIndexer);
  this->RequestQueue.pushIndexingRequest(request);
  if (!this->RequestQueue.isIndexing())
  {
    // Start background indexing
    this->RequestQueue.setIndexing(true);
    QMap<QString, QDateTime> modifiedTimeForFilepath;
    this->Database->allFilesModifiedTimes(modifiedTimeForFilepath);
    this->RequestQueue.setModifiedTimeForFilepath(modifiedTimeForFilepath);
    emit startWorker();
  }
}

//------------------------------------------------------------------------------
CTK_GET_CPP(ctkDICOMIndexer, bool, isBackgroundImportEnabled, BackgroundImportEnabled);
CTK_SET_CPP(ctkDICOMIndexer, bool, setBackgroundImportEnabled, BackgroundImportEnabled);

//------------------------------------------------------------------------------
// ctkDICOMIndexer methods

//------------------------------------------------------------------------------
ctkDICOMIndexer::ctkDICOMIndexer(QObject *parent)
  : d_ptr(new ctkDICOMIndexerPrivate(*this))
{
  Q_UNUSED(parent);
}

//------------------------------------------------------------------------------
ctkDICOMIndexer::~ctkDICOMIndexer()
{
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::setDatabase(ctkDICOMDatabase* database)
{
  Q_D(ctkDICOMIndexer);
  if (d->Database == database)
  {
    return;
  }
  if (d->Database)
  {
    QObject::disconnect(d->Database, SIGNAL(opened()), this, SLOT(databaseFilenameChanged()));
    QObject::disconnect(d->Database, SIGNAL(tagsToPrecacheChanged()), this, SLOT(tagsToPrecacheChanged()));
    QObject::disconnect(d->Database, SIGNAL(tagsToExcludeFromStorageChanged()), this, SLOT(tagsToExcludeFromStorageChanged()));
  }
  d->Database = database;
  if (d->Database)
  {
    QObject::connect(d->Database, SIGNAL(opened()), this, SLOT(databaseFilenameChanged()));
    QObject::connect(d->Database, SIGNAL(tagsToPrecacheChanged()), this, SLOT(tagsToPrecacheChanged()));
    QObject::connect(d->Database, SIGNAL(tagsToExcludeFromStorageChanged()), this, SLOT(tagsToExcludeFromStorageChanged()));
    d->RequestQueue.setDatabaseFilename(d->Database->databaseFilename());
    d->RequestQueue.setTagsToPrecache(d->Database->tagsToPrecache());
    d->RequestQueue.setTagsToExcludeFromStorage(d->Database->tagsToExcludeFromStorage());
  }
  else
  {
    d->RequestQueue.setDatabaseFilename(QString());
    d->RequestQueue.setTagsToPrecache(QStringList());
    d->RequestQueue.setTagsToExcludeFromStorage(QStringList());
  }
}

//------------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMIndexer::database()
{
  Q_D(ctkDICOMIndexer);
  return d->Database;
}

 //------------------------------------------------------------------------------
 void ctkDICOMIndexer::databaseFilenameChanged()
 {
   Q_D(ctkDICOMIndexer);
   if (d->Database)
   {
     d->RequestQueue.setDatabaseFilename(d->Database->databaseFilename());
   }
   else
   {
     d->RequestQueue.setDatabaseFilename(QString());
   }
 }
 
 //------------------------------------------------------------------------------
 void ctkDICOMIndexer::tagsToPrecacheChanged()
 {
   Q_D(ctkDICOMIndexer);
   if (d->Database)
   {
     d->RequestQueue.setTagsToPrecache(d->Database->tagsToPrecache());
   }
   else
   {
     d->RequestQueue.setTagsToPrecache(QStringList());
   }
 }

 //------------------------------------------------------------------------------
 void ctkDICOMIndexer::tagsToExcludeFromStorageChanged()
 {
   Q_D(ctkDICOMIndexer);
   if (d->Database)
   {
     d->RequestQueue.setTagsToExcludeFromStorage(d->Database->tagsToExcludeFromStorage());
   }
   else
   {
     d->RequestQueue.setTagsToExcludeFromStorage(QStringList());
   }
 }

 //------------------------------------------------------------------------------
 void ctkDICOMIndexer::addFile(ctkDICOMDatabase* db, const QString filePath, bool copyFile/*=false*/)
 {
   this->setDatabase(db);
   this->addFile(filePath, copyFile);
 }
 
//------------------------------------------------------------------------------
void ctkDICOMIndexer::addFile(const QString filePath, bool copyFile/*=false*/)
{
  Q_D(ctkDICOMIndexer);
  DICOMIndexingQueue::IndexingRequest request;
  request.inputFilesPath << filePath;
  request.includeHidden = true;
  request.copyFile = copyFile;
  d->pushIndexingRequest(request);
  if (!d->BackgroundImportEnabled)
  {
    this->waitForImportFinished();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addDirectory(ctkDICOMDatabase* db, const QString& directoryName, bool copyFile/*=false*/, bool includeHidden/*=true*/)
{
  this->setDatabase(db);
  this->addDirectory(directoryName, copyFile, includeHidden);
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addDirectory(const QString& directoryName, bool copyFile/*=false*/, bool includeHidden/*=true*/)
{
  Q_D(ctkDICOMIndexer);

  QStringList listOfFiles;
  QDir directory(directoryName);
  if (directory.exists("DICOMDIR"))
  {
    this->addDicomdir(directoryName, copyFile);
  }
  else
  {
    DICOMIndexingQueue::IndexingRequest request;
    request.inputFolderPath = directoryName;
    request.includeHidden = includeHidden;
    request.copyFile = copyFile;
    d->pushIndexingRequest(request);
  }
  if (!d->BackgroundImportEnabled)
  {
    this->waitForImportFinished();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addListOfFiles(ctkDICOMDatabase* db, const QStringList& listOfFiles, bool copyFile/*=false*/)
{
  this->setDatabase(db);
  this->addListOfFiles(listOfFiles, copyFile);
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addListOfFiles(const QStringList& listOfFiles, bool copyFile/*=false*/)
{
  Q_D(ctkDICOMIndexer);
  DICOMIndexingQueue::IndexingRequest request;
  request.inputFilesPath = listOfFiles;
  request.includeHidden = true;
  request.copyFile = copyFile;
  d->pushIndexingRequest(request);
  if (!d->BackgroundImportEnabled)
  {
    this->waitForImportFinished();
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMIndexer::addDicomdir(ctkDICOMDatabase* db, const QString& directoryName, bool copyFile/*=false*/)
{
  this->setDatabase(db);
  return this->addDicomdir(directoryName, copyFile);
}

//------------------------------------------------------------------------------
bool ctkDICOMIndexer::addDicomdir(const QString& directoryName, bool copyFile/*=false*/)
{
  //Initialize dicomdir with directory path
  QString dcmFilePath = directoryName;
  dcmFilePath.append("/DICOMDIR");
  DcmDicomDir* dicomDir = new DcmDicomDir(dcmFilePath.toStdString().c_str());

  //Values to store records data at the moment only uid needed
  OFString patientsName, studyInstanceUID, seriesInstanceUID, sopInstanceUID, referencedFileName ;

  //Variables for progress operations
  QString instanceFilePath;
  QStringList listOfInstances;

  DcmDirectoryRecord* rootRecord = &(dicomDir->getRootRecord());
  DcmDirectoryRecord* patientRecord = NULL;
  DcmDirectoryRecord* studyRecord = NULL;
  DcmDirectoryRecord* seriesRecord = NULL;
  DcmDirectoryRecord* fileRecord = NULL;

  QTime timeProbe;
  timeProbe.start();

  /*Iterate over all records in dicomdir and setup path to the dataset of the filerecord
  then insert. the filerecord into the database.
  If any UID is missing the record and all of it's subelements won't be added to the database*/
  bool success = true;
  if(rootRecord != NULL)
  {
    while ((patientRecord = rootRecord->nextSub(patientRecord)) != NULL)
    {
      logger.debug( "Reading new Patient:" );
      if (patientRecord->findAndGetOFString(DCM_PatientName, patientsName).bad())
      {
        logger.warn( "DICOMDIR file at "+directoryName+" is invalid: patient name not found. All records belonging to this patient will be ignored.");
        success = false;
        continue;
      }
      logger.debug( "Patient's Name: " + QString(patientsName.c_str()) );
      while ((studyRecord = patientRecord->nextSub(studyRecord)) != NULL)
      {
        logger.debug( "Reading new Study:" );
        if (studyRecord->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID).bad())
        {
          logger.warn( "DICOMDIR file at "+directoryName+" is invalid: study instance UID not found for patient "+ QString(patientsName.c_str())+". All records belonging to this study will be ignored.");
          success = false;
          continue;
        }
        logger.debug( "Study instance UID: " + QString(studyInstanceUID.c_str()) );

        while ((seriesRecord = studyRecord->nextSub(seriesRecord)) != NULL)
        {
          logger.debug( "Reading new Series:" );
          if (seriesRecord->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID).bad())
          {
            logger.warn( "DICOMDIR file at "+directoryName+" is invalid: series instance UID not found for patient "+ QString(patientsName.c_str())+", study "+ QString(studyInstanceUID.c_str())+". All records belonging to this series will be ignored.");
            success = false;
            continue;
          }
          logger.debug( "Series instance UID: " + QString(seriesInstanceUID.c_str()) );

          while ((fileRecord = seriesRecord->nextSub(fileRecord)) != NULL)
          {
            if (fileRecord->findAndGetOFStringArray(DCM_ReferencedSOPInstanceUIDInFile, sopInstanceUID).bad()
              || fileRecord->findAndGetOFStringArray(DCM_ReferencedFileID,referencedFileName).bad())
            {
              logger.warn( "DICOMDIR file at "+directoryName+" is invalid: referenced SOP instance UID or file name is invalid for patient "
                + QString(patientsName.c_str())+", study "+ QString(studyInstanceUID.c_str())+", series "+ QString(seriesInstanceUID.c_str())+
                ". This file will be ignored.");
              success = false;
              continue;
            }

            //Get the filepath of the instance and insert it into a list
            instanceFilePath = directoryName;
            instanceFilePath.append("/");
            instanceFilePath.append(QString( referencedFileName.c_str() ));
            instanceFilePath.replace("\\","/");
            listOfInstances << instanceFilePath;
          }
        }
      }
    }
    float elapsedTimeInSeconds = timeProbe.elapsed() / 1000.0;
    qDebug()
        << QString("DICOM indexer has successfully processed DICOMDIR in %1 [%2s]")
           .arg(directoryName)
           .arg(QString::number(elapsedTimeInSeconds,'f', 2));
    this->addListOfFiles(listOfInstances, copyFile);
  }
  return success;
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::waitForImportFinished(int msecTimeout /*=-1*/)
{
  if (!this->isImporting())
  {
    return;
  }
  QTimer timer;
  timer.setSingleShot(true);
  QEventLoop loop;
  connect(this, &ctkDICOMIndexer::indexingComplete, &loop, &QEventLoop::quit);
  if (msecTimeout >= 0)
  {
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(msecTimeout);
  }
  if (!this->isImporting())
  {
    return;
  }
  loop.exec();
}

//------------------------------------------------------------------------------
bool ctkDICOMIndexer::isImporting()
{
  Q_D(ctkDICOMIndexer);
  return d->RequestQueue.isIndexing();
}

//----------------------------------------------------------------------------
void ctkDICOMIndexer::cancel()
{
  Q_D(ctkDICOMIndexer);
  d->RequestQueue.setStopRequested(true);
}
