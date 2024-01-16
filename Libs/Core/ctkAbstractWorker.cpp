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

#include "ctkAbstractJob.h"
#include "ctkAbstractScheduler.h"
#include "ctkAbstractWorker.h"

// --------------------------------------------------------------------------
ctkAbstractWorker::ctkAbstractWorker()
{
  this->Job = nullptr;
  this->Scheduler = nullptr;
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
ctkAbstractScheduler *ctkAbstractWorker::scheduler() const
{
  return this->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkAbstractScheduler> ctkAbstractWorker::schedulerShared() const
{
  return this->Scheduler;
}

//----------------------------------------------------------------------------
void ctkAbstractWorker::setScheduler(ctkAbstractScheduler &scheduler)
{
  this->Scheduler = QSharedPointer<ctkAbstractScheduler>(&scheduler, skipDelete);
}

//----------------------------------------------------------------------------
void ctkAbstractWorker::setScheduler(QSharedPointer<ctkAbstractScheduler> scheduler)
{
  this->Scheduler = scheduler;
}
