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
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMEchoWorker_p.h"
#include "ctkDICOMEchoJob.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"

static ctkLogger logger ("org.commontk.dicom.DICOMRetrieveWorker");

//------------------------------------------------------------------------------
// ctkDICOMEchoWorkerPrivate methods

//------------------------------------------------------------------------------
ctkDICOMEchoWorkerPrivate::ctkDICOMEchoWorkerPrivate(ctkDICOMEchoWorker* object)
 : q_ptr(object)
{
  this->Echo = QSharedPointer<ctkDICOMEcho>(new ctkDICOMEcho);
}

//------------------------------------------------------------------------------
ctkDICOMEchoWorkerPrivate::~ctkDICOMEchoWorkerPrivate() = default;

//------------------------------------------------------------------------------
void ctkDICOMEchoWorkerPrivate::setRetrieveParameters()
{
  Q_Q(ctkDICOMEchoWorker);

  QSharedPointer<ctkDICOMEchoJob> echoJob =
    qSharedPointerObjectCast<ctkDICOMEchoJob>(q->Job);
  if (!echoJob)
  {
    return;
  }

  ctkDICOMServer* server = echoJob->server();
  if (!server)
  {
    return;
  }

  this->Echo->setConnectionName(server->connectionName());
  this->Echo->setCallingAETitle(server->callingAETitle());
  this->Echo->setCalledAETitle(server->calledAETitle());
  this->Echo->setHost(server->host());
  this->Echo->setPort(server->port());
  this->Echo->setConnectionTimeout(server->connectionTimeout());
  this->Echo->setJobUID(echoJob->jobUID());
}

//------------------------------------------------------------------------------
// ctkDICOMEchoWorker methods

//------------------------------------------------------------------------------
ctkDICOMEchoWorker::ctkDICOMEchoWorker()
  : d_ptr(new ctkDICOMEchoWorkerPrivate(this))
{
}

//------------------------------------------------------------------------------
ctkDICOMEchoWorker::ctkDICOMEchoWorker(ctkDICOMEchoWorkerPrivate* pimpl)
  : d_ptr(pimpl)
{
}

//------------------------------------------------------------------------------
ctkDICOMEchoWorker::~ctkDICOMEchoWorker() = default;

//----------------------------------------------------------------------------
void ctkDICOMEchoWorker::requestCancel()
{
  Q_D(const ctkDICOMEchoWorker);
  d->Echo->cancel();
}

//----------------------------------------------------------------------------
void ctkDICOMEchoWorker::run()
{
  Q_D(const ctkDICOMEchoWorker);
  QSharedPointer<ctkDICOMEchoJob> echoJob =
    qSharedPointerObjectCast<ctkDICOMEchoJob>(this->Job);
  if (!echoJob)
  {
    return;
  }

  QSharedPointer<ctkDICOMScheduler> scheduler =
      qSharedPointerObjectCast<ctkDICOMScheduler>(this->Scheduler);
  ctkDICOMServer* server = echoJob->server();
  if (!scheduler
      || !server
      || d->Echo->wasCanceled())
  {
    this->onJobCanceled(d->Echo->wasCanceled());
    return;
  }

  echoJob->setStatus(ctkAbstractJob::JobStatus::Running);

  logger.debug(QString("ctkDICOMEchoWorker : running job %1 in thread %2.\n")
                       .arg(echoJob->jobUID())
                       .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

  if (!d->Echo->echo())
  {
    this->onJobCanceled(d->Echo->wasCanceled());
    return;
  }

  if (d->Echo->wasCanceled())
  {
    this->onJobCanceled(d->Echo->wasCanceled());
    return;
  }

  echoJob->setStatus(ctkAbstractJob::JobStatus::Finished);
}

//----------------------------------------------------------------------------
void ctkDICOMEchoWorker::setJob(QSharedPointer<ctkAbstractJob> job)
{
  Q_D(ctkDICOMEchoWorker);

  QSharedPointer<ctkDICOMEchoJob> echoJob =
    qSharedPointerObjectCast<ctkDICOMEchoJob>(job);
  if (!echoJob)
  {
    return;
  }

  this->Superclass::setJob(job);
  d->setRetrieveParameters();
}

//----------------------------------------------------------------------------
ctkDICOMEcho* ctkDICOMEchoWorker::echo() const
{
  Q_D(const ctkDICOMEchoWorker);
  return d->Echo.data();
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMEcho> ctkDICOMEchoWorker::echoShared() const
{
  Q_D(const ctkDICOMEchoWorker);
  return d->Echo;
}
