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

// ctkCore includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMThumbnailGenerator.h"
#include "ctkDICOMThumbnailGeneratorWorker_p.h"
#include "ctkDICOMThumbnailGeneratorJob.h"
#include "ctkDICOMScheduler.h"

static ctkLogger logger ("org.commontk.dicom.DICOMRetrieveWorker");

//------------------------------------------------------------------------------
// ctkDICOMThumbnailGeneratorWorkerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorWorkerPrivate::ctkDICOMThumbnailGeneratorWorkerPrivate(ctkDICOMThumbnailGeneratorWorker* object)
 : q_ptr(object)
{
  this->wasCancelled = false;
}

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorWorkerPrivate::~ctkDICOMThumbnailGeneratorWorkerPrivate() = default;

//------------------------------------------------------------------------------
// ctkDICOMThumbnailGeneratorWorker methods

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorWorker::ctkDICOMThumbnailGeneratorWorker()
  : d_ptr(new ctkDICOMThumbnailGeneratorWorkerPrivate(this))
{
}

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorWorker::ctkDICOMThumbnailGeneratorWorker(ctkDICOMThumbnailGeneratorWorkerPrivate* pimpl)
  : d_ptr(pimpl)
{
}

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorWorker::~ctkDICOMThumbnailGeneratorWorker() = default;

//----------------------------------------------------------------------------
void ctkDICOMThumbnailGeneratorWorker::requestCancel()
{
  Q_D(ctkDICOMThumbnailGeneratorWorker);
  d->wasCancelled = true;
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailGeneratorWorker::run()
{
  Q_D(const ctkDICOMThumbnailGeneratorWorker);
  QSharedPointer<ctkDICOMThumbnailGeneratorJob> thumbnailGeneratorJob =
    qSharedPointerObjectCast<ctkDICOMThumbnailGeneratorJob>(this->Job);
  if (!thumbnailGeneratorJob)
  {
    return;
  }

  QSharedPointer<ctkDICOMScheduler> scheduler =
      qSharedPointerObjectCast<ctkDICOMScheduler>(this->Scheduler);
  if (!scheduler)
  {
    this->onJobCanceled(d->wasCancelled);
    return;
  }

  if (d->wasCancelled)
  {
    this->onJobCanceled(d->wasCancelled);
    return;
  }

  thumbnailGeneratorJob->setStatus(ctkAbstractJob::JobStatus::Running);

  logger.debug(QString("ctkDICOMThumbnailGeneratorWorker : running job %1 in thread %2.\n")
                       .arg(thumbnailGeneratorJob->jobUID())
                       .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

  ctkDICOMDatabase database;
  QString dbConnectionName =
    "db_" + QString::number(reinterpret_cast<quint64>(QThread::currentThreadId()), 16);
  database.openDatabase(thumbnailGeneratorJob->databaseFilename(), dbConnectionName);
  QSharedPointer<ctkDICOMThumbnailGenerator> thumbnailGenerator =
    QSharedPointer<ctkDICOMThumbnailGenerator>(new ctkDICOMThumbnailGenerator);
  database.setThumbnailGenerator(thumbnailGenerator.data());
  database.storeThumbnailFile(thumbnailGeneratorJob->dicomFilePath(),
                              thumbnailGeneratorJob->studyInstanceUID(),
                              thumbnailGeneratorJob->seriesInstanceUID(),
                              thumbnailGeneratorJob->sopInstanceUID(),
                              thumbnailGeneratorJob->modality());
  database.closeDatabase();

  if (d->wasCancelled)
  {
    this->onJobCanceled(d->wasCancelled);
    return;
  }
  QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet =
    QSharedPointer<ctkDICOMJobResponseSet>(new ctkDICOMJobResponseSet);

  jobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::ThumbnailGenerator);
  jobResponseSet->setPatientID(thumbnailGeneratorJob->patientID());
  jobResponseSet->setStudyInstanceUID(thumbnailGeneratorJob->studyInstanceUID());
  jobResponseSet->setSeriesInstanceUID(thumbnailGeneratorJob->seriesInstanceUID());
  jobResponseSet->setSOPInstanceUID(thumbnailGeneratorJob->sopInstanceUID());
  jobResponseSet->setJobUID(thumbnailGeneratorJob->jobUID());

  thumbnailGeneratorJob->progressJobDetail(jobResponseSet->toVariant());
  thumbnailGeneratorJob->setStatus(ctkAbstractJob::JobStatus::Finished);
}

//----------------------------------------------------------------------------
void ctkDICOMThumbnailGeneratorWorker::setJob(QSharedPointer<ctkAbstractJob> job)
{
  QSharedPointer<ctkDICOMThumbnailGeneratorJob> ThumbnailGeneratorJob =
    qSharedPointerObjectCast<ctkDICOMThumbnailGeneratorJob>(job);
  if (!ThumbnailGeneratorJob)
  {
    return;
  }

  this->Superclass::setJob(job);
}
