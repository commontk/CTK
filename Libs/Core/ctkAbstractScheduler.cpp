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

// CTK includes
#include "ctkAbstractJob.h"
#include "ctkAbstractScheduler.h"
#include "ctkLogger.h"

static ctkLogger logger ("org.commontk.core.AbstractScheduler");


//---------------------------------------------------------------------------
// ctkAbstractScheduler methods

// --------------------------------------------------------------------------
ctkAbstractScheduler::ctkAbstractScheduler(QObject* parent)
  : QObject(parent)
{
}

// --------------------------------------------------------------------------
ctkAbstractScheduler::~ctkAbstractScheduler() = default;

//----------------------------------------------------------------------------
QVariant ctkAbstractScheduler::jobToDetail(ctkAbstractJob* job)
{
  if (!job)
    {
    return QVariant();
    }

  QVariant data;
  data.setValue(job->jobUID());

  return data;
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::onJobStarted()
{
  ctkAbstractJob* job = qobject_cast<ctkAbstractJob*>(this->sender());
  if (!job)
    {
    return;
    }

  logger.debug(job->loggerReport("started"));
  emit this->jobStarted(this->jobToDetail(job));
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::onJobCanceled()
{
  ctkAbstractJob* job = qobject_cast<ctkAbstractJob*>(this->sender());
  if (!job)
    {
    return;
    }
  logger.debug(job->loggerReport("canceled"));
  emit this->jobCanceled(this->jobToDetail(job));
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::onJobFailed()
{
  ctkAbstractJob* job = qobject_cast<ctkAbstractJob*>(this->sender());
  if (!job)
    {
    return;
    }

  logger.debug(job->loggerReport("failed"));

  QVariant data = this->jobToDetail(job);
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  emit this->jobFailed(data);
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::onJobFinished()
{
  ctkAbstractJob* job = qobject_cast<ctkAbstractJob*>(this->sender());
  if (!job)
    {
    return;
    }

  logger.debug(job->loggerReport("finished"));

  QVariant data = this->jobToDetail(job);
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  emit this->jobFinished(data);
}
