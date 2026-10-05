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
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

// ctkDICOMCore includes
#include "ctkDICOMInserterJob.h"
#include "ctkDICOMInserterWorker.h"
#include "ctkLogger.h"

static ctkLogger logger ("org.commontk.dicom.DICOMInserterJob");

//------------------------------------------------------------------------------
// ctkDICOMInserterJob methods

//------------------------------------------------------------------------------
ctkDICOMInserterJob::ctkDICOMInserterJob(QObject* parent)
  : Superclass(parent)
{
}

//------------------------------------------------------------------------------
ctkDICOMInserterJob::~ctkDICOMInserterJob() = default;

//------------------------------------------------------------------------------
QString ctkDICOMInserterJob::loggerReport(const QString& status)
{
  QString fullLogMsg;
  QString logMsg;

  // One line per response set, with the instances it carries
  QStringList responseSetReports;
  int numberOfInstances = 0;
  foreach (QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet, this->JobResponseSets)
  {
    QStringList instanceUIDs = jobResponseSet->datasets().keys();
    numberOfInstances += instanceUIDs.count();
    responseSetReports << QString("  %1: %2")
                              .arg(jobResponseSet->jobTypeString())
                              .arg(instanceUIDs.isEmpty() ? tr("no instance") : instanceUIDs.join(", "));
  }

  fullLogMsg = QString("ctkDICOMInserterJob: insert job %1.\n"
                       "JobUID: %2\n"
                       "Response sets: %3\n"
                       "Instances: %4\n"
                       "%5")
                      .arg(status)
                      .arg(this->jobUID())
                      .arg(this->JobResponseSets.count())
                      .arg(numberOfInstances)
                      .arg(responseSetReports.isEmpty() ? QString() : responseSetReports.join("\n") + "\n");

  // The per-job log is shown in the UI for every status change, so it stays a
  // one line summary: repeating the instance UIDs on each of them made the same
  // block appear once for the start and once for the completion of the job.
  logMsg = QString("Insert job %1. Response sets: %2, instances: %3.\n")
                  .arg(status)
                  .arg(this->JobResponseSets.count())
                  .arg(numberOfInstances);

  QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
  QString logHeader = currentDateTime + " INFO: ";
  this->Log += logHeader;
  this->Log += logMsg;
  return fullLogMsg;
}

//------------------------------------------------------------------------------
void ctkDICOMInserterJob::setDatabaseFilename(const QString& databaseFilename)
{
  this->DatabaseFilename = databaseFilename;
}

//------------------------------------------------------------------------------
QString ctkDICOMInserterJob::databaseFilename() const
{
  return this->DatabaseFilename;
}

//------------------------------------------------------------------------------
void ctkDICOMInserterJob::setTagsToPrecache(const QStringList& tagsToPrecache)
{
  this->TagsToPrecache = tagsToPrecache;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMInserterJob::tagsToPrecache() const
{
  return this->TagsToPrecache;
}

//------------------------------------------------------------------------------
void ctkDICOMInserterJob::setTagsToExcludeFromStorage(const QStringList& tagsToExcludeFromStorage)
{
  this->TagsToExcludeFromStorage = tagsToExcludeFromStorage;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMInserterJob::tagsToExcludeFromStorage() const
{
  return this->TagsToExcludeFromStorage;
}

//------------------------------------------------------------------------------
ctkAbstractJob* ctkDICOMInserterJob::clone() const
{
  ctkDICOMInserterJob* newInserterJob = new ctkDICOMInserterJob;
  newInserterJob->setDICOMLevel(this->dicomLevel());
  newInserterJob->setPatientID(this->patientID());
  newInserterJob->setStudyInstanceUID(this->studyInstanceUID());
  newInserterJob->setSeriesInstanceUID(this->seriesInstanceUID());
  newInserterJob->setSOPInstanceUID(this->sopInstanceUID());
  newInserterJob->setRetryEnabled(this->retryEnabled());
  newInserterJob->setMaximumRetryWait(this->maximumRetryWait());
  newInserterJob->setRetryBackoffFactor(this->retryBackoffFactor());
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
ctkAbstractWorker* ctkDICOMInserterJob::createWorker()
{
  ctkDICOMInserterWorker* worker =
    new ctkDICOMInserterWorker;
  worker->setJob(*this);
  return worker;
}

//------------------------------------------------------------------------------
ctkDICOMJobResponseSet::JobType ctkDICOMInserterJob::getJobType() const
{
  return ctkDICOMJobResponseSet::JobType::Inserter;
}
