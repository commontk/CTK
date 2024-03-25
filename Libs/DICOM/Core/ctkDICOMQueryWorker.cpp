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

// ctkCore includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMQueryWorker_p.h"
#include "ctkDICOMQueryJob.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"

static ctkLogger logger ("org.commontk.dicom.DICOMQueryWorker");

//------------------------------------------------------------------------------
// ctkDICOMQueryWorkerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMQueryWorkerPrivate::ctkDICOMQueryWorkerPrivate(ctkDICOMQueryWorker* object)
 : q_ptr(object)
{
  this->Query = QSharedPointer<ctkDICOMQuery>(new ctkDICOMQuery);
}

//------------------------------------------------------------------------------
ctkDICOMQueryWorkerPrivate::~ctkDICOMQueryWorkerPrivate() = default;

//------------------------------------------------------------------------------
void ctkDICOMQueryWorkerPrivate::setQueryParameters()
{
  Q_Q(ctkDICOMQueryWorker);

  QSharedPointer<ctkDICOMQueryJob> queryJob =
    qSharedPointerObjectCast<ctkDICOMQueryJob>(q->Job);
  if (!queryJob)
  {
    return;
  }

  ctkDICOMServer* server = queryJob->server();
  if (!server)
  {
    return;
  }

  this->Query->setConnectionName(server->connectionName());
  this->Query->setCallingAETitle(server->callingAETitle());
  this->Query->setCalledAETitle(server->calledAETitle());
  this->Query->setHost(server->host());
  this->Query->setPort(server->port());
  this->Query->setConnectionTimeout(server->connectionTimeout());
  this->Query->setJobUID(queryJob->jobUID());
  this->Query->setFilters(queryJob->filters());
}

//------------------------------------------------------------------------------
// ctkDICOMQueryWorker methods

//------------------------------------------------------------------------------
ctkDICOMQueryWorker::ctkDICOMQueryWorker()
  : d_ptr(new ctkDICOMQueryWorkerPrivate(this))
{
}

//------------------------------------------------------------------------------
ctkDICOMQueryWorker::ctkDICOMQueryWorker(ctkDICOMQueryWorkerPrivate* pimpl)
  : d_ptr(pimpl)
{
}

//------------------------------------------------------------------------------
ctkDICOMQueryWorker::~ctkDICOMQueryWorker() = default;

//----------------------------------------------------------------------------
void ctkDICOMQueryWorker::requestCancel()
{
  Q_D(const ctkDICOMQueryWorker);
  d->Query->cancel();
}

//----------------------------------------------------------------------------
void ctkDICOMQueryWorker::run()
{
  Q_D(const ctkDICOMQueryWorker);
  QSharedPointer<ctkDICOMQueryJob> queryJob =
    qSharedPointerObjectCast<ctkDICOMQueryJob>(this->Job);
  if (!queryJob)
  {
    return;
  }

  QSharedPointer<ctkDICOMScheduler> scheduler =
    qobject_cast<QSharedPointer<ctkDICOMScheduler>>(this->Scheduler);
  if (!scheduler ||
      d->Query->wasCanceled())
  {
    this->onJobCanceled(d->Query->wasCanceled());
    return;
  }

  queryJob->setStatus(ctkAbstractJob::JobStatus::Running);

  logger.debug(QString("ctkDICOMQueryWorker : running job %1 in thread %2.\n")
                       .arg(queryJob->jobUID())
                       .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

  switch (queryJob->dicomLevel())
  {
    case ctkDICOMJob::DICOMLevels::None:
      logger.warn("ctkDICOMQueryWorker : DICOMLevels was not set.");
      this->Job->setStatus(ctkAbstractJob::JobStatus::Finished);
      return;
    case ctkDICOMJob::DICOMLevels::Patients:
      if (!d->Query->queryPatients())
      {
        this->onJobCanceled(d->Query->wasCanceled());
        return;
      }
      break;
    case ctkDICOMJob::DICOMLevels::Studies:
      if (!d->Query->queryStudies(queryJob->patientID()))
      {
        this->onJobCanceled(d->Query->wasCanceled());
        return;
      }
      break;
    case ctkDICOMJob::DICOMLevels::Series:
      if (!d->Query->querySeries(queryJob->patientID(),
                                 queryJob->studyInstanceUID()))
      {
        this->onJobCanceled(d->Query->wasCanceled());
        return;
      }
      break;
    case ctkDICOMJob::DICOMLevels::Instances:
      if (!d->Query->queryInstances(queryJob->patientID(),
                                    queryJob->studyInstanceUID(),
                                    queryJob->seriesInstanceUID()))
      {
        this->onJobCanceled(d->Query->wasCanceled());
        return;
      }
      break;
  }

  if (d->Query->wasCanceled())
  {
    this->onJobCanceled(d->Query->wasCanceled());
    return;
  }

  if (d->Query->jobResponseSetsShared().count() > 0)
  {
    queryJob->setReferenceInserterJobUID
      (scheduler->insertJobResponseSets(d->Query->jobResponseSetsShared()));
  }

  queryJob->setStatus(ctkAbstractJob::JobStatus::Finished);
}

//----------------------------------------------------------------------------
void ctkDICOMQueryWorker::setJob(QSharedPointer<ctkAbstractJob> job)
{
  Q_D(ctkDICOMQueryWorker);

  QSharedPointer<ctkDICOMQueryJob> queryJob =
    qSharedPointerObjectCast<ctkDICOMQueryJob>(job);
  if (!queryJob)
  {
    return;
  }

  this->Superclass::setJob(job);
  d->setQueryParameters();
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMQuery> ctkDICOMQueryWorker::querierShared() const
{
  Q_D(const ctkDICOMQueryWorker);
  return d->Query;
}

//------------------------------------------------------------------------------
ctkDICOMQuery* ctkDICOMQueryWorker::querier() const
{
  Q_D(const ctkDICOMQueryWorker);
  return d->Query.data();
}
