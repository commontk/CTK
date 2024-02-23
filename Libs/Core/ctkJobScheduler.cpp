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
#include <QCoreApplication>
#include <QMutexLocker>
#include <QSharedPointer>
#include <QThreadPool>
#include <QUuid>

// CTK includes
#include "ctkAbstractJob.h"
#include "ctkJobScheduler.h"
#include "ctkJobScheduler_p.h"
#include "ctkAbstractWorker.h"
#include "ctkLogger.h"

static ctkLogger logger("org.commontk.core.AbstractScheduler");

// --------------------------------------------------------------------------
// ctkJobSchedulerPrivate methods

// --------------------------------------------------------------------------
ctkJobSchedulerPrivate::ctkJobSchedulerPrivate(ctkJobScheduler& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
ctkJobSchedulerPrivate::~ctkJobSchedulerPrivate() = default;

//---------------------------------------------------------------------------
void ctkJobSchedulerPrivate::init()
{
  Q_Q(ctkJobScheduler);

  QObject::connect(q, SIGNAL(queueJobs()),
                   q, SLOT(onQueueJobsInThreadPool()),
                   Qt::QueuedConnection);

  this->ThreadPool = QSharedPointer<QThreadPool>(new QThreadPool());
  this->ThreadPool->setMaxThreadCount(20);
}

//------------------------------------------------------------------------------
void ctkJobSchedulerPrivate::insertJob(QSharedPointer<ctkAbstractJob> job)
{
  Q_Q(ctkJobScheduler);

  if (!job)
  {
    return;
  }

  logger.debug(QString("ctkJobScheduler: creating job object %1 of type %2 in thread %3.\n")
    .arg(job->jobUID())
    .arg(job->className())
    .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

  QObject::connect(job.data(), SIGNAL(started()), q, SLOT(onJobStarted()));
  QObject::connect(job.data(), SIGNAL(canceled()), q, SLOT(onJobCanceled()));
  QObject::connect(job.data(), SIGNAL(failed()), q, SLOT(onJobFailed()));
  QObject::connect(job.data(), SIGNAL(finished()), q, SLOT(onJobFinished()));
  QObject::connect(job.data(), SIGNAL(progressJobDetail(QVariant)),
                   q, SIGNAL(progressJobDetail(QVariant)));

  QMutexLocker ml(&this->mMutex);
  this->JobsQueue.insert(job->jobUID(), job);
  emit q->queueJobs();
}

//------------------------------------------------------------------------------
void ctkJobSchedulerPrivate::removeJob(const QString& jobUID)
{
  Q_Q(ctkJobScheduler);

  logger.debug(QString("ctkJobScheduler: deleting job object %1 in thread %2.\n")
    .arg(jobUID)
    .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId()), 16)));

  QSharedPointer<ctkAbstractJob> job = this->JobsQueue.value(jobUID);
  if (!job)
  {
    return;
  }

  QObject::disconnect(job.data(), SIGNAL(started()), q, SLOT(onJobStarted()));
  QObject::disconnect(job.data(), SIGNAL(canceled()), q, SLOT(onJobCanceled()));
  QObject::disconnect(job.data(), SIGNAL(failed()), q, SLOT(onJobFailed()));
  QObject::disconnect(job.data(), SIGNAL(finished()), q, SLOT(onJobFinished()));
  QObject::disconnect(job.data(), SIGNAL(progressJobDetail(QVariant)), q, SIGNAL(progressJobDetail(QVariant)));

  this->JobsQueue.remove(jobUID);
  emit q->queueJobs();
}

//------------------------------------------------------------------------------
int ctkJobSchedulerPrivate::getSameTypeJobsInThreadPoolQueueOrRunning(QSharedPointer<ctkAbstractJob> job)
{
  int count = 0;
  foreach (QSharedPointer<ctkAbstractJob> queuedJob, this->JobsQueue)
  {
    if (queuedJob->jobUID() == job->jobUID())
    {
      continue;
    }

    if ((queuedJob->status() == ctkAbstractJob::JobStatus::Queued ||
         queuedJob->status() == ctkAbstractJob::JobStatus::Running) &&
        queuedJob->className() == job->className())
    {
      count++;
    }
  }

  return count;
}

//------------------------------------------------------------------------------
QString ctkJobSchedulerPrivate::generateUniqueJobUID()
{
  return QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
}

//---------------------------------------------------------------------------
// ctkJobScheduler methods

// --------------------------------------------------------------------------
ctkJobScheduler::ctkJobScheduler(QObject* parent)
  : QObject(parent)
  , d_ptr(new ctkJobSchedulerPrivate(*this))
{
  Q_D(ctkJobScheduler);
  d->init();
}

// --------------------------------------------------------------------------
ctkJobScheduler::ctkJobScheduler(ctkJobSchedulerPrivate* pimpl, QObject* parent)
  : Superclass(parent)
  , d_ptr(pimpl)
{
  // derived classes must call init manually. Calling init() here may results in
  // actions on a derived public class not yet finished to be created
}

// --------------------------------------------------------------------------
ctkJobScheduler::~ctkJobScheduler() = default;

//----------------------------------------------------------------------------
int ctkJobScheduler::numberOfJobs()
{
  Q_D(ctkJobScheduler);
  QMutexLocker ml(&d->mMutex);
  return d->JobsQueue.count();
}

//----------------------------------------------------------------------------
int ctkJobScheduler::numberOfPersistentJobs()
{
  Q_D(ctkJobScheduler);
  int cont = 0;
  QMutexLocker ml(&d->mMutex);
  foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
  {
    if (job->isPersistent())
    {
      cont++;
    }
  }
  return cont;
}

//----------------------------------------------------------------------------
void ctkJobScheduler::addJob(ctkAbstractJob* job)
{
  Q_D(ctkJobScheduler);

  QSharedPointer<ctkAbstractJob> jobShared = QSharedPointer<ctkAbstractJob>(job);
  d->insertJob(jobShared);
}

//----------------------------------------------------------------------------
void ctkJobScheduler::deleteJob(const QString& jobUID)
{
  Q_D(ctkJobScheduler);
  d->removeJob(jobUID);
}

//----------------------------------------------------------------------------
void ctkJobScheduler::deleteWorker(const QString& jobUID)
{
  Q_D(ctkJobScheduler);

  QMap<QString, QSharedPointer<ctkAbstractWorker>>::iterator it = d->Workers.find(jobUID);
  if (it == d->Workers.end())
  {
    return;
  }

  d->Workers.remove(jobUID);
}

//----------------------------------------------------------------------------
QSharedPointer<ctkAbstractJob> ctkJobScheduler::getJobSharedByUID(const QString& jobUID)
{
  Q_D(ctkJobScheduler);

  QMutexLocker ml(&d->mMutex);
  QMap<QString, QSharedPointer<ctkAbstractJob>>::iterator it = d->JobsQueue.find(jobUID);
  if (it == d->JobsQueue.end())
  {
    return nullptr;
  }

  return d->JobsQueue.value(jobUID);
}

//----------------------------------------------------------------------------
ctkAbstractJob* ctkJobScheduler::getJobByUID(const QString& jobUID)
{
  QSharedPointer<ctkAbstractJob> job = this->getJobSharedByUID(jobUID);
  if (!job)
  {
    return nullptr;
  }

  return job.data();
}

//----------------------------------------------------------------------------
void ctkJobScheduler::waitForFinish(bool waitForPersistentJobs)
{
  Q_D(ctkJobScheduler);

  int numberOfPersistentJobs = this->numberOfPersistentJobs();
  if (waitForPersistentJobs)
  {
    numberOfPersistentJobs = 0;
  }
  while (this->numberOfJobs() > numberOfPersistentJobs)
  {
    d->ThreadPool->waitForDone(300);
  }
}

//----------------------------------------------------------------------------
void ctkJobScheduler::waitForDone(int msec)
{
  Q_D(ctkJobScheduler);
  d->ThreadPool->waitForDone(msec);
}

//----------------------------------------------------------------------------
void ctkJobScheduler::stopAllJobs(bool stopPersistentJobs)
{
  Q_D(ctkJobScheduler);

  QMutexLocker ml(&d->mMutex);

  // Stops jobs without a worker (in waiting)
  foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
  {
    if (job->isPersistent() && !stopPersistentJobs)
    {
      continue;
    }

    if (job->status() != ctkAbstractJob::JobStatus::Initialized)
    {
      continue;
    }

    job->setStatus(ctkAbstractJob::JobStatus::Stopped);
    this->deleteJob(job->jobUID());
  }

  // Stops queued and running jobs
  foreach (QSharedPointer<ctkAbstractWorker> worker, d->Workers)
  {
    QSharedPointer<ctkAbstractJob> job = worker->jobShared();
    if (job->isPersistent() && !stopPersistentJobs)
    {
      continue;
    }

    if (job->status() != ctkAbstractJob::JobStatus::Running &&
        job->status() != ctkAbstractJob::JobStatus::Queued)
    {
      continue;
    }

    job->setStatus(ctkAbstractJob::JobStatus::Stopped);
    worker->cancel();
  }
}

//----------------------------------------------------------------------------
int ctkJobScheduler::maximumThreadCount() const
{
  Q_D(const ctkJobScheduler);
  return d->ThreadPool->maxThreadCount();
}

//----------------------------------------------------------------------------
void ctkJobScheduler::setMaximumThreadCount(int maximumThreadCount)
{
  Q_D(ctkJobScheduler);
  d->ThreadPool->setMaxThreadCount(maximumThreadCount);
}

//----------------------------------------------------------------------------
int ctkJobScheduler::maximumNumberOfRetry() const
{
  Q_D(const ctkJobScheduler);
  return d->MaximumNumberOfRetry;
}

//----------------------------------------------------------------------------
void ctkJobScheduler::setMaximumNumberOfRetry(int maximumNumberOfRetry)
{
  Q_D(ctkJobScheduler);
  d->MaximumNumberOfRetry = maximumNumberOfRetry;
}

//----------------------------------------------------------------------------
int ctkJobScheduler::retryDelay() const
{
  Q_D(const ctkJobScheduler);
  return d->RetryDelay;
}

//----------------------------------------------------------------------------
void ctkJobScheduler::setRetryDelay(int retryDelay)
{
  Q_D(ctkJobScheduler);
  d->RetryDelay = retryDelay;
}

//----------------------------------------------------------------------------
QThreadPool* ctkJobScheduler::threadPool() const
{
  Q_D(const ctkJobScheduler);
  return d->ThreadPool.data();
}

//----------------------------------------------------------------------------
QSharedPointer<QThreadPool> ctkJobScheduler::threadPoolShared() const
{
  Q_D(const ctkJobScheduler);
  return d->ThreadPool;
}

//----------------------------------------------------------------------------
void ctkJobScheduler::onJobStarted()
{
  ctkAbstractJob* job = qobject_cast<ctkAbstractJob*>(this->sender());
  if (!job)
  {
    return;
  }

  logger.debug(job->loggerReport("started"));
  emit this->jobStarted(job->toVariant());
}

//----------------------------------------------------------------------------
void ctkJobScheduler::onJobCanceled()
{
  ctkAbstractJob* job = qobject_cast<ctkAbstractJob*>(this->sender());
  if (!job)
  {
    return;
  }
  logger.debug(job->loggerReport("canceled"));

  QVariant data = job->toVariant();
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  emit this->jobCanceled(data);
}

//----------------------------------------------------------------------------
void ctkJobScheduler::onJobFailed()
{
  ctkAbstractJob* job = qobject_cast<ctkAbstractJob*>(this->sender());
  if (!job)
  {
    return;
  }

  logger.debug(job->loggerReport("failed"));

  QVariant data = job->toVariant();
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  emit this->jobFailed(data);
}

//----------------------------------------------------------------------------
void ctkJobScheduler::onJobFinished()
{
  ctkAbstractJob* job = qobject_cast<ctkAbstractJob*>(this->sender());
  if (!job)
  {
    return;
  }

  logger.debug(job->loggerReport("finished"));

  QVariant data = job->toVariant();
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  emit this->jobFinished(data);
}

//----------------------------------------------------------------------------
void ctkJobScheduler::onQueueJobsInThreadPool()
{
  Q_D(ctkJobScheduler);

  QMutexLocker ml(&d->mMutex);
  foreach (QThread::Priority priority, (QList<QThread::Priority>()
                                        << QThread::Priority::HighestPriority
                                        << QThread::Priority::HighPriority
                                        << QThread::Priority::NormalPriority
                                        << QThread::Priority::LowPriority
                                        << QThread::Priority::LowestPriority))
  {
    foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
    {
      if (job->priority() != priority)
      {
        continue;
      }

      if (job->status() != ctkAbstractJob::JobStatus::Initialized)
      {
        continue;
      }

      int numberOfRunningJobsWithSameType = d->getSameTypeJobsInThreadPoolQueueOrRunning(job);
      if (numberOfRunningJobsWithSameType >= job->maximumConcurrentJobsPerType())
      {
        continue;
      }

      logger.debug(QString("ctkDICOMScheduler: creating worker for job %1 in thread %2.\n")
                       .arg(job->jobUID())
                       .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

      job->setStatus(ctkAbstractJob::JobStatus::Queued);
      emit this->jobQueued(job->toVariant());

      QSharedPointer<ctkAbstractWorker> worker =
        QSharedPointer<ctkAbstractWorker>(job->createWorker());
      worker->setScheduler(*this);

      d->Workers.insert(job->jobUID(), worker);
      d->ThreadPool->start(worker.data(), job->priority());
    }
  }
}
