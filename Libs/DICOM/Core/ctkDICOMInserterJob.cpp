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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

// ctkDICOMCore includes
#include "ctkDICOMInserterJob.h"
#include "ctkDICOMInserterWorker.h"
#include "ctkLogger.h"

static ctkLogger logger ( "org.commontk.dicom.ctkDICOMInserterJob" );

//------------------------------------------------------------------------------
// ctkDICOMInserterJob methods

//------------------------------------------------------------------------------
ctkDICOMInserterJob::ctkDICOMInserterJob()
{
  this->DatabaseFilename = "";
  this->MaximumConcurrentJobsPerType = 1;
}

//------------------------------------------------------------------------------
ctkDICOMInserterJob::~ctkDICOMInserterJob()
{
}

//------------------------------------------------------------------------------
QString ctkDICOMInserterJob::loggerReport(const QString &status) const
{
  switch (this->dicomLevel())
  {
    case ctkDICOMJob::DICOMLevels::Patients:
      return QString("ctkDICOMInserterJob: insert job at patients level %1.\n"
                     "JobUID: %2\n"
                     "PatientID: %3")
          .arg(status)
          .arg(this->jobUID())
          .arg(this->patientID());
    case ctkDICOMJob::DICOMLevels::Studies:
      return QString("ctkDICOMInserterJob: insert job at studies level %1.\n"
                     "JobUID: %2\n"
                     "PatientID: %3\n"
                     "StudyInstanceUID: %4")
          .arg(status)
          .arg(this->jobUID())
          .arg(this->patientID())
          .arg(this->studyInstanceUID());
    case ctkDICOMJob::DICOMLevels::Series:
      return QString("ctkDICOMInserterJob: insert job at series level %1.\n"
                     "JobUID: %2\n"
                     "PatientID: %3\n"
                     "StudyInstanceUID: %4\n"
                     "SeriesInstanceUID: %5")
          .arg(status)
          .arg(this->jobUID())
          .arg(this->patientID())
          .arg(this->studyInstanceUID())
          .arg(this->seriesInstanceUID());
    case ctkDICOMJob::DICOMLevels::Instances:
      return QString("ctkDICOMInserterJob: insert job at instances level %1.\n"
                     "JobUID: %2\n"
                     "PatientID: %3\n"
                     "StudyInstanceUID: %4\n"
                     "SeriesInstanceUID: %5 \n"
                     "SOPInstanceUID: %6.")
          .arg(status)
          .arg(this->jobUID())
          .arg(this->patientID())
          .arg(this->studyInstanceUID())
          .arg(this->seriesInstanceUID())
          .arg(this->sopInstanceUID());
    default:
      return QString("");
  }
}

//------------------------------------------------------------------------------
void ctkDICOMInserterJob::setDatabaseFilename(const QString &databaseFilename)
{
  this->DatabaseFilename = databaseFilename;
}

//------------------------------------------------------------------------------
QString ctkDICOMInserterJob::databaseFilename() const
{
  return this->DatabaseFilename;
}

//------------------------------------------------------------------------------
void ctkDICOMInserterJob::setTagsToPrecache(const QStringList &tagsToPrecache)
{
  this->TagsToPrecache = tagsToPrecache;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMInserterJob::tagsToPrecache() const
{
  return this->TagsToPrecache;
}

//------------------------------------------------------------------------------
void ctkDICOMInserterJob::setTagsToExcludeFromStorage(const QStringList &tagsToExcludeFromStorage)
{
  this->TagsToExcludeFromStorage = tagsToExcludeFromStorage;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMInserterJob::tagsToExcludeFromStorage() const
{
  return this->TagsToExcludeFromStorage;
}

//------------------------------------------------------------------------------
ctkDICOMJob* ctkDICOMInserterJob::generateCopy() const
{
  ctkDICOMInserterJob* newInserterJob = new ctkDICOMInserterJob;
  newInserterJob->setDICOMLevel(this->dicomLevel());
  newInserterJob->setPatientID(this->patientID());
  newInserterJob->setStudyInstanceUID(this->studyInstanceUID());
  newInserterJob->setSeriesInstanceUID(this->seriesInstanceUID());
  newInserterJob->setSOPInstanceUID(this->sopInstanceUID());
  newInserterJob->setMaximumNumberOfRetry(this->maximumNumberOfRetry());
  newInserterJob->setRetryDelay(this->retryDelay());
  newInserterJob->setRetryCounter(this->retryCounter());
  newInserterJob->setIsPersistent(this->isPersistent());
  newInserterJob->setMaximumConcurrentJobsPerType(this->maximumConcurrentJobsPerType());
  newInserterJob->setPriority(this->priority());
  newInserterJob->setDatabaseFilename(this->databaseFilename());
  newInserterJob->setTagsToPrecache(this->tagsToPrecache());
  newInserterJob->setTagsToExcludeFromStorage(this->tagsToExcludeFromStorage());

  return newInserterJob;
}

//------------------------------------------------------------------------------
ctkDICOMWorker *ctkDICOMInserterJob::createWorker()
{
  ctkDICOMInserterWorker* worker =
    new ctkDICOMInserterWorker;
  worker->setJob(*this);
  return worker;
}

