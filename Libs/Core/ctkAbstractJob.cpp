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

#include "ctkAbstractJob.h"

// Qt includes
#include <QRandomGenerator>
#include <QUuid>

// STD includes
#include <cmath>

// --------------------------------------------------------------------------
ctkAbstractJob::ctkAbstractJob(QObject* parent)
  : QObject(parent)
{
  this->Status = JobStatus::Initialized;
  this->Persistent = false;
  this->JobUID = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
  this->RetryCounter = 0;
  this->RetryEnabled = true;
  this->RetryDelay = 1000;
  this->RetryBackoffFactor = 2.5;
  this->MaximumRetryWait = 60000;
  this->AccumulatedRetryWait = 0;
  this->MaximumConcurrentJobsPerType = 20;
  this->MaximumConcurrentJobsPerGroup = 8;
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
}

//----------------------------------------------------------------------------
QString ctkAbstractJob::concurrencyGroup() const
{
  return QString();
}

//----------------------------------------------------------------------------
int ctkAbstractJob::maximumConcurrentJobsPerGroup() const
{
  return this->MaximumConcurrentJobsPerGroup;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setMaximumConcurrentJobsPerGroup(int maximumConcurrentJobsPerGroup)
{
  this->MaximumConcurrentJobsPerGroup = maximumConcurrentJobsPerGroup;
}

//----------------------------------------------------------------------------
bool ctkAbstractJob::retryEnabled() const
{
  return this->RetryEnabled;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setRetryEnabled(bool retryEnabled)
{
  this->RetryEnabled = retryEnabled;
}

//----------------------------------------------------------------------------
double ctkAbstractJob::retryBackoffFactor() const
{
  return this->RetryBackoffFactor;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setRetryBackoffFactor(double retryBackoffFactor)
{
  this->RetryBackoffFactor = retryBackoffFactor;
}

//----------------------------------------------------------------------------
int ctkAbstractJob::maximumRetryWait() const
{
  return this->MaximumRetryWait;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setMaximumRetryWait(int maximumRetryWait)
{
  this->MaximumRetryWait = maximumRetryWait;
}

//----------------------------------------------------------------------------
int ctkAbstractJob::accumulatedRetryWait() const
{
  return this->AccumulatedRetryWait;
}

//----------------------------------------------------------------------------
void ctkAbstractJob::setAccumulatedRetryWait(int accumulatedRetryWait)
{
  this->AccumulatedRetryWait = accumulatedRetryWait;
}

//----------------------------------------------------------------------------
int ctkAbstractJob::nextRetryDelay() const
{
  if (!this->RetryEnabled || this->MaximumRetryWait <= 0 || this->RetryDelay <= 0)
  {
    return -1;
  }

  int remainingWait = this->MaximumRetryWait - this->AccumulatedRetryWait;
  if (remainingWait <= 0)
  {
    return -1;
  }

  double factor = this->RetryBackoffFactor > 1. ? this->RetryBackoffFactor : 1.;
  double delay = this->RetryDelay * std::pow(factor, qMax(0, this->RetryCounter));

  // Randomize by +/-25%, so that all the jobs that failed together (typically every
  // job of a server that went down) do not come back to the server at the same time.
  delay *= 1. + (QRandomGenerator::global()->generateDouble() - 0.5) * 0.5;

  // Waiting longer than the remaining budget would overshoot the maximum waiting
  // time, so the last attempt is made as soon as the budget ends.
  return qMin(static_cast<int>(qMax(1., delay)), remainingWait);
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
}

//----------------------------------------------------------------------------
QVariant ctkAbstractJob::toVariant()
{
  return QVariant::fromValue(ctkJobDetail(*this));
}
