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

// Qt includes
#include <QUuid>

// --------------------------------------------------------------------------
ctkAbstractJob::ctkAbstractJob()
{
  this->Status = JobStatus::Initialized;
  this->Persistent = false;
  this->JobUID = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
  this->RetryCounter = 0;
  this->RetryDelay = 100;
  this->MaximumNumberOfRetry = 3;
  this->MaximumConcurrentJobsPerType = 20;
  this->Priority = QThread::Priority::LowPriority;
  this->CreationDateTime = QDateTime::currentDateTime();
  this->DestroyAfterUse = false;
}

//----------------------------------------------------------------------------
ctkAbstractJob::~ctkAbstractJob()
{
}

//----------------------------------------------------------------------------
QString ctkAbstractJob::className() const
{
  if (!this->metaObject())
  {
    return "";
  }
  return this->metaObject()->className();
}

//----------------------------------------------------------------------------
QString ctkAbstractJob::jobUID() const
{
  return this->JobUID;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setJobUID(const QString &jobUID)
{
  this->JobUID = jobUID;
  emit this->jobUIDChanged();
}

//----------------------------------------------------------------------------
ctkAbstractJob::JobStatus ctkAbstractJob::status() const
{
  return this->Status;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setStatus(JobStatus status)
{
  this->Status = status;

  if (this->Status == JobStatus::Running)
  {
    this->StartDateTime = QDateTime::currentDateTime();
  }
  else if (this->Status > JobStatus::Running)
  {
    this->CompletionDateTime = QDateTime::currentDateTime();
  }

  if (this->Status == JobStatus::Running)
  {
    emit this->started();
  }
  else if (this->Status == JobStatus::UserStopped)
  {
    emit this->userStopped();
  }
  else if (this->Status == JobStatus::AttemptFailed)
  {
    emit this->attemptFailed();
  }
  else if (this->Status == JobStatus::Failed)
  {
    emit this->failed();
  }
  else if (this->Status == JobStatus::Finished)
  {
    emit this->finished();
  }
  emit this->statusChanged(status);
}

//----------------------------------------------------------------------------
bool ctkAbstractJob::isPersistent() const
{
  return this->Persistent;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setIsPersistent(bool persistent)
{
  this->Persistent = persistent;
  emit isPersistentChanged(persistent);
}

//----------------------------------------------------------------------------
int ctkAbstractJob::retryCounter() const
{
  return this->RetryCounter;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setRetryCounter(int retryCounter)
{
  this->RetryCounter = retryCounter;
  emit this->retryCounterChanged(retryCounter);
}

//----------------------------------------------------------------------------
int ctkAbstractJob::maximumConcurrentJobsPerType() const
{
  return this->MaximumConcurrentJobsPerType;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setMaximumConcurrentJobsPerType(int maximumConcurrentJobsPerType)
{
  this->MaximumConcurrentJobsPerType = maximumConcurrentJobsPerType;
  emit this->maximumConcurrentJobsPerTypeChanged(maximumConcurrentJobsPerType);
}

//----------------------------------------------------------------------------
int ctkAbstractJob::maximumNumberOfRetry() const
{
  return this->MaximumNumberOfRetry;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setMaximumNumberOfRetry(int maximumNumberOfRetry)
{
  this->MaximumNumberOfRetry = maximumNumberOfRetry;
  emit this->maximumNumberOfRetryChanged(maximumNumberOfRetry);
}

//----------------------------------------------------------------------------
int ctkAbstractJob::retryDelay() const
{
  return this->RetryDelay;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setRetryDelay(int retryDelay)
{
  this->RetryDelay = retryDelay;
  emit this->retryDelayChanged(retryDelay);
}

//----------------------------------------------------------------------------
QThread::Priority ctkAbstractJob::priority() const
{
  return this->Priority;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setPriority(const QThread::Priority &priority)
{
  this->Priority = priority;
  emit this->priorityChanged(priority);
}

//----------------------------------------------------------------------------
QDateTime ctkAbstractJob::creationDateTime() const
{
  return this->CreationDateTime;
}

//----------------------------------------------------------------------------
QDateTime ctkAbstractJob::startDateTime() const
{
  return this->StartDateTime;
}

//----------------------------------------------------------------------------
QDateTime ctkAbstractJob::completionDateTime() const
{
  return this->CompletionDateTime;
}

//----------------------------------------------------------------------------
QString ctkAbstractJob::runningThreadID() const
{
  return this->RunningThreadID;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setRunningThreadID(QString runningThreadID)
{
  this->RunningThreadID = runningThreadID;
  emit this->runningThreadIDChanged(runningThreadID);
}

//----------------------------------------------------------------------------
QString ctkAbstractJob::log() const
{
  return this->Log;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::addLog(QString log)
{
  this->Log += log;
}

//----------------------------------------------------------------------------
bool ctkAbstractJob::destroyAfterUse() const
{
  return this->DestroyAfterUse;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setDestroyAfterUse(bool destroyAfterUse)
{
  this->DestroyAfterUse = destroyAfterUse;
  emit this->destroyAfterUseChanged(destroyAfterUse);
}

//----------------------------------------------------------------------------
QVariant ctkAbstractJob::toVariant()
{
  return QVariant::fromValue(ctkJobDetail(*this));
}
