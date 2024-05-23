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

// Qt includes
#include <QThread>

// ctkCore includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMInserterJob.h"
#include "ctkDICOMInserterWorker_p.h"
#include "ctkDICOMJobResponseSet.h"

// DCMTK includes
#include <dcmtk/oflog/spi/logevent.h>

static ctkLogger logger ("org.commontk.dicom.DICOMInserterWorker");

//------------------------------------------------------------------------------
// ctkDICOMInserterWorkerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMInserterWorkerPrivate::ctkDICOMInserterWorkerPrivate(ctkDICOMInserterWorker* object)
 : q_ptr(object)
{
  this->Inserter = QSharedPointer<ctkDICOMInserter>(new ctkDICOMInserter);
}

//------------------------------------------------------------------------------
ctkDICOMInserterWorkerPrivate::~ctkDICOMInserterWorkerPrivate() = default;

//------------------------------------------------------------------------------
void ctkDICOMInserterWorkerPrivate::setInserterParameters()
{
  Q_Q(ctkDICOMInserterWorker);

  QSharedPointer<ctkDICOMInserterJob> inserterJob =
    qSharedPointerObjectCast<ctkDICOMInserterJob>(q->Job);
  if (!inserterJob)
  {
    return;
  }

  this->Inserter->setDatabaseFilename(inserterJob->databaseFilename());
  this->Inserter->setTagsToPrecache(inserterJob->tagsToPrecache());
  this->Inserter->setTagsToExcludeFromStorage(inserterJob->tagsToExcludeFromStorage());
}

//------------------------------------------------------------------------------
// ctkDICOMInserterWorker methods

//------------------------------------------------------------------------------
ctkDICOMInserterWorker::ctkDICOMInserterWorker()
  : d_ptr(new ctkDICOMInserterWorkerPrivate(this))
{
}

//------------------------------------------------------------------------------
ctkDICOMInserterWorker::ctkDICOMInserterWorker(ctkDICOMInserterWorkerPrivate* pimpl)
  : d_ptr(pimpl)
{
}

//------------------------------------------------------------------------------
ctkDICOMInserterWorker::~ctkDICOMInserterWorker() = default;

//----------------------------------------------------------------------------
void ctkDICOMInserterWorker::requestCancel()
{
  Q_D(const ctkDICOMInserterWorker);
  d->Inserter->cancel();
}

//----------------------------------------------------------------------------
void ctkDICOMInserterWorker::run()
{
  Q_D(const ctkDICOMInserterWorker);
  QSharedPointer<ctkDICOMInserterJob> inserterJob =
    qSharedPointerObjectCast<ctkDICOMInserterJob>(this->Job);
  if (!inserterJob)
  {
    return;
  }

  QString currentThread = dcmtk::log4cplus::thread::getCurrentThreadName().c_str();
  inserterJob->setRunningThreadID(currentThread);

  if (d->Inserter->wasCanceled())
  {
    this->onJobCanceled(d->Inserter->wasCanceled());
    return;
  }

  inserterJob->setStatus(ctkAbstractJob::JobStatus::Running);

  logger.debug(QString("ctkDICOMInserterWorker : running job %1 in thread %2.\n")
                       .arg(inserterJob->jobUID())
                       .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

  QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets = inserterJob->jobResponseSetsShared();
  d->Inserter->addJobResponseSets(jobResponseSets);

  if (d->Inserter->wasCanceled())
  {
    this->onJobCanceled(d->Inserter->wasCanceled());
    return;
  }

  foreach (QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet, jobResponseSets)
  {
    emit inserterJob->progressJobDetail(jobResponseSet->toVariant());
  }
  inserterJob->setStatus(ctkAbstractJob::JobStatus::Finished);
}

//----------------------------------------------------------------------------
void ctkDICOMInserterWorker::setJob(QSharedPointer<ctkAbstractJob> job)
{
  Q_D(ctkDICOMInserterWorker);

  QSharedPointer<ctkDICOMInserterJob> inserterJob =
    qSharedPointerObjectCast<ctkDICOMInserterJob>(job);
  if (!inserterJob)
  {
    return;
  }

  this->Superclass::setJob(job);
  d->setInserterParameters();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMInserter> ctkDICOMInserterWorker::inserterShared() const
{
  Q_D(const ctkDICOMInserterWorker);
  return d->Inserter;
}

//------------------------------------------------------------------------------
ctkDICOMInserter* ctkDICOMInserterWorker::inserter() const
{
  Q_D(const ctkDICOMInserterWorker);
  return d->Inserter.data();
}
