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

#include "ctkDICOMJob.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMWorker.h"

//------------------------------------------------------------------------------
ctkDICOMWorker::ctkDICOMWorker() = default;

//------------------------------------------------------------------------------
ctkDICOMWorker::~ctkDICOMWorker() = default;

//----------------------------------------------------------------------------
void ctkDICOMWorker::startNextJob()
{
  QSharedPointer<ctkDICOMScheduler> scheduler =
    qobject_cast<QSharedPointer<ctkDICOMScheduler>>(this->Scheduler);
  if (!scheduler)
    {
    return;
    }

  QSharedPointer<ctkDICOMJob> job =
    qobject_cast<QSharedPointer<ctkDICOMJob>>(this->Job);
  if (!job)
    {
    return;
    }

  ctkDICOMJob* newJob = job->generateCopy();
  newJob->setRetryCounter(newJob->retryCounter() + 1);
  scheduler->addJob(newJob);
}

//----------------------------------------------------------------------------
void ctkDICOMWorker::onJobCanceled()
{
  QSharedPointer<ctkDICOMJob> job =
    qobject_cast<QSharedPointer<ctkDICOMJob>>(this->Job);
  if (!job)
    {
    return;
    }

  if (job->retryCounter() < job->maximumNumberOfRetry() &&
      job->status() != ctkAbstractJob::JobStatus::Stopped)
    {
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(job->retryDelay());

    this->startNextJob();

    emit job->finished();
    }
  else if (job->status() != ctkAbstractJob::JobStatus::Stopped)
    {
    emit job->failed();
    }
  else
    {
    emit job->finished();
    }
}
