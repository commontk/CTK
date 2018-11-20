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
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// ctkDICOMIndexerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::ctkDICOMIndexerPrivate(ctkDICOMIndexer& o)
  : q_ptr(&o)
  , Canceled(false)
  , StartedIndexing(0)
{
}

//------------------------------------------------------------------------------
ctkDICOMIndexerPrivate::~ctkDICOMIndexerPrivate()
{
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ctkDICOMIndexer methods

//------------------------------------------------------------------------------
ctkDICOMIndexer::ctkDICOMIndexer(QObject *parent):d_ptr(new ctkDICOMIndexerPrivate(*this))
{
  Q_UNUSED(parent);
}

//------------------------------------------------------------------------------
ctkDICOMIndexer::~ctkDICOMIndexer()
{
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addFile(ctkDICOMDatabase& database,
                                   const QString filePath,
                                   const QString& destinationDirectoryName)
{
  ctkDICOMIndexer::ScopedIndexing indexingBatch(*this, database);
  emit indexingFilePath(filePath);
  // Ignoring destinationDirectoryName parameter, just taking it as indication we should copy
  bool copyFileToDatabase = !destinationDirectoryName.isEmpty();
  database.insert(filePath, copyFileToDatabase, true);
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addDirectory(ctkDICOMDatabase& database,
                                   const QString& directoryName,
                                   const QString& destinationDirectoryName,
                                   bool includeHidden/*=true*/)
{
  ctkDICOMIndexer::ScopedIndexing indexingBatch(*this, database);
  QStringList listOfFiles;
  QDir directory(directoryName);

  if(directory.exists("DICOMDIR"))
  {
    addDicomdir(database,directoryName,destinationDirectoryName);
  }
  else
  {
    QDir::Filters filters = QDir::Files;
    if (includeHidden)
    {
      filters |= QDir::Hidden;
    }
    QDirIterator it(directoryName, filters, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
      listOfFiles << it.next();
    }
    emit foundFilesToIndex(listOfFiles.count());
    addListOfFiles(database,listOfFiles,destinationDirectoryName);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::addListOfFiles(ctkDICOMDatabase& database,
                                     const QStringList& listOfFiles,
                                     const QString& destinationDirectoryName)
{
  Q_D(ctkDICOMIndexer);
  ctkDICOMIndexer::ScopedIndexing indexingBatch(*this, database);
  QTime timeProbe;
  timeProbe.start();
  d->Canceled = false;
  int CurrentFileIndex = 0;
  int lastReportedPercent = 0;
  foreach(QString filePath, listOfFiles)
  {
    int percent = ( 100 * CurrentFileIndex ) / listOfFiles.size();
    if (lastReportedPercent / 10 < percent / 10)
      {
      // Reporting progress has a huge overhead (pending events are processed,
      // database is updated), therefore only report progress at every 10% increase
      emit this->progress(percent);
      lastReportedPercent = percent;
      }
    this->addFile(database, filePath, destinationDirectoryName);
    CurrentFileIndex++;

    if( d->Canceled )
      {
      break;
      }
  }
  float elapsedTimeInSeconds = timeProbe.elapsed() / 1000.0;
  qDebug()
      << QString("DICOM indexer has successfully processed %1 files [%2s]")
         .arg(CurrentFileIndex)
         .arg(QString::number(elapsedTimeInSeconds,'f', 2));
}

//------------------------------------------------------------------------------
bool ctkDICOMIndexer::addDicomdir(ctkDICOMDatabase& database,
                 const QString& directoryName,
                 const QString& destinationDirectoryName
                 )
{
  ctkDICOMIndexer::ScopedIndexing indexingBatch(*this, database);
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
    emit foundFilesToIndex(listOfInstances.count());
    addListOfFiles(database,listOfInstances,destinationDirectoryName);
  }
  return success;
}

//------------------------------------------------------------------------------
void ctkDICOMIndexer::refreshDatabase(ctkDICOMDatabase& database, const QString& directoryName)
{
  Q_UNUSED(database);
  Q_UNUSED(directoryName);
  /*
   * Probably this should go to the database class as well
   * Or we have to extend the interface to make possible what we do here
   * without using SQL directly

  /// get all filenames from the database
  QSqlQuery allFilesQuery(database.database());
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
  */
  }

//------------------------------------------------------------------------------
void ctkDICOMIndexer::waitForImportFinished()
{
  // No-op - this had been used when the indexing was multi-threaded,
  // and has only been retained for API compatibility.
}

//----------------------------------------------------------------------------
void ctkDICOMIndexer::cancel()
{
  Q_D(ctkDICOMIndexer);
  d->Canceled = true;
}

//----------------------------------------------------------------------------
void ctkDICOMIndexer::startIndexing(ctkDICOMDatabase& database)
{
  Q_D(ctkDICOMIndexer);
  if (d->StartedIndexing == 0)
  {
    // Indexing has just been started
    database.prepareInsert();
  }
  d->StartedIndexing++;
}

//----------------------------------------------------------------------------
void ctkDICOMIndexer::endIndexing()
{
  Q_D(ctkDICOMIndexer);
  d->StartedIndexing--;
  if (d->StartedIndexing == 0)
  {
    // Indexing has just been completed
    emit this->indexingComplete();
  }
  if (d->StartedIndexing < 0)
  {
    qWarning() << QString("ctkDICOMIndexer::endIndexing called without matching startIndexing");
    d->StartedIndexing = 0;
  }
}
