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
#include <QEventLoop>
#include <QTimer>

// CTK includes
#include "ctkAbstractJob.h"
#include "ctkJobScheduler.h"
#include "ctkAbstractWorker.h"

// --------------------------------------------------------------------------
ctkAbstractWorker::ctkAbstractWorker()
{
  this->setAutoDelete(false);
}

//----------------------------------------------------------------------------
ctkAbstractWorker::~ctkAbstractWorker() = default;

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
ctkAbstractJob* ctkAbstractWorker::job() const
{
  return this->Job.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkAbstractJob> ctkAbstractWorker::jobShared() const
{
  return this->Job;
}

//----------------------------------------------------------------------------
void ctkAbstractWorker::setJob(ctkAbstractJob &job)
{
  this->setJob(QSharedPointer<ctkAbstractJob>(&job, skipDelete));
}

//----------------------------------------------------------------------------
void ctkAbstractWorker::setJob(QSharedPointer<ctkAbstractJob> job)
{
  this->Job = job;
}

//----------------------------------------------------------------------------
ctkJobScheduler *ctkAbstractWorker::scheduler() const
{
  return this->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkJobScheduler> ctkAbstractWorker::schedulerShared() const
{
  return this->Scheduler;
}

//----------------------------------------------------------------------------
void ctkAbstractWorker::setScheduler(ctkJobScheduler &scheduler)
{
    this->Scheduler = QSharedPointer<ctkJobScheduler>(&scheduler, skipDelete);
}

//----------------------------------------------------------------------------
void ctkAbstractWorker::setScheduler(QSharedPointer<ctkJobScheduler> scheduler)
{
  this->Scheduler = scheduler;
}

//----------------------------------------------------------------------------
void ctkAbstractWorker::startNextJob()
{
  if (!this->Scheduler || !this->Job)
  {
    return;
  }

  ctkAbstractJob* newJob = this->Job->clone();
  newJob->setRetryCounter(newJob->retryCounter() + 1);
  this->Scheduler->addJob(newJob);
}

//----------------------------------------------------------------------------
void ctkAbstractWorker::onJobCanceled(const bool& wasCanceled)
{
  if (!this->Job)
  {
    return;
  }

  if (!wasCanceled)
  {
    if (this->Job->retryCounter() < this->Job->maximumNumberOfRetry())
    {
      QTimer timer;
      timer.setSingleShot(true);
      QEventLoop loop;
      connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
      timer.start(this->Job->retryDelay());

      this->startNextJob();
    }
    this->Job->setStatus(ctkAbstractJob::JobStatus::Failed);
  }
  else
  {
    this->Job->setStatus(ctkAbstractJob::JobStatus::Stopped);
  }
}
