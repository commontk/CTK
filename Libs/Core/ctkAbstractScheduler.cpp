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
#include "ctkAbstractScheduler.h"
#include "ctkAbstractScheduler_p.h"
#include "ctkAbstractWorker.h"
#include "ctkLogger.h"

static ctkLogger logger ("org.commontk.core.AbstractScheduler");

// --------------------------------------------------------------------------
// ctkAbstractSchedulerPrivate methods

// --------------------------------------------------------------------------
ctkAbstractSchedulerPrivate::ctkAbstractSchedulerPrivate(ctkAbstractScheduler& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
ctkAbstractSchedulerPrivate::~ctkAbstractSchedulerPrivate() = default;

//---------------------------------------------------------------------------
void ctkAbstractSchedulerPrivate::init()
{
  Q_Q(ctkAbstractScheduler);

  QObject::connect(q, SIGNAL(queueJobs()),
                   q, SLOT(onQueueJobsInThreadPool()),
                   Qt::QueuedConnection);

  this->ThreadPool = QSharedPointer<QThreadPool> (new QThreadPool());
  this->ThreadPool->setMaxThreadCount(20);
}

//------------------------------------------------------------------------------
void ctkAbstractSchedulerPrivate::insertJob(QSharedPointer<ctkAbstractJob> job)
{
  Q_Q(ctkAbstractScheduler);

  if (!job)
    {
    return;
    }

  logger.debug(QString("ctkAbstractScheduler: creating job object %1 of type %2 in thread %3.\n")
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
void ctkAbstractSchedulerPrivate::removeJob(const QString& jobUID)
{
  Q_Q(ctkAbstractScheduler);

  logger.debug(QString("ctkAbstractScheduler: deleting job object %1 in thread %2.\n")
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
int ctkAbstractSchedulerPrivate::getSameTypeJobsInThreadPoolQueueOrRunning(QSharedPointer<ctkAbstractJob> job)
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
QString ctkAbstractSchedulerPrivate::generateUniqueJobUID()
{
  return QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
}

//---------------------------------------------------------------------------
// ctkAbstractScheduler methods

// --------------------------------------------------------------------------
ctkAbstractScheduler::ctkAbstractScheduler(QObject* parent)
  : QObject(parent)
  , d_ptr(new ctkAbstractSchedulerPrivate(*this))
{
  Q_D(ctkAbstractScheduler);
  d->init();
}

// --------------------------------------------------------------------------
ctkAbstractScheduler::ctkAbstractScheduler(ctkAbstractSchedulerPrivate* pimpl, QObject* parent)
  : Superclass(parent)
  , d_ptr(pimpl)
{
  // derived classes must call init manually. Calling init() here may results in
  // actions on a derived public class not yet finished to be created
}

// --------------------------------------------------------------------------
ctkAbstractScheduler::~ctkAbstractScheduler() = default;

//----------------------------------------------------------------------------
int ctkAbstractScheduler::numberOfJobs()
{
  Q_D(ctkAbstractScheduler);
  QMutexLocker ml(&d->mMutex);
  return d->JobsQueue.count();
}

//----------------------------------------------------------------------------
int ctkAbstractScheduler::numberOfPersistentJobs()
{
  Q_D(ctkAbstractScheduler);
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
void ctkAbstractScheduler::addJob(ctkAbstractJob *job)
{
  Q_D(ctkAbstractScheduler);

  QSharedPointer<ctkAbstractJob> jobShared = QSharedPointer<ctkAbstractJob>(job);
  d->insertJob(jobShared);
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::deleteJob(const QString& jobUID)
{
  Q_D(ctkAbstractScheduler);
  d->removeJob(jobUID);
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::deleteWorker(const QString& jobUID)
{
  Q_D(ctkAbstractScheduler);

  QMap<QString, QSharedPointer<ctkAbstractWorker>>::iterator it = d->Workers.find(jobUID);
  if (it == d->Workers.end())
    {
    return;
    }

  d->Workers.remove(jobUID);
}

//----------------------------------------------------------------------------
QSharedPointer<ctkAbstractJob> ctkAbstractScheduler::getJobSharedByUID(const QString& jobUID)
{
  Q_D(ctkAbstractScheduler);

  QMutexLocker ml(&d->mMutex);
  QMap<QString, QSharedPointer<ctkAbstractJob>>::iterator it = d->JobsQueue.find(jobUID);
  if (it == d->JobsQueue.end())
    {
    return nullptr;
    }

  return d->JobsQueue.value(jobUID);
}

//----------------------------------------------------------------------------
ctkAbstractJob *ctkAbstractScheduler::getJobByUID(const QString& jobUID)
{
  QSharedPointer<ctkAbstractJob> job = this->getJobSharedByUID(jobUID);
  if (!job)
    {
    return nullptr;
    }

  return job.data();
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::waitForFinish()
{
  Q_D(ctkAbstractScheduler);

  int numberOfPersistentJobs = this->numberOfPersistentJobs();
  while(this->numberOfJobs() > numberOfPersistentJobs)
    {
    QCoreApplication::processEvents();
    d->ThreadPool->waitForDone(300);
  }
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::waitForDone(int msec)
{
  Q_D(ctkAbstractScheduler);

  QCoreApplication::processEvents();
  d->ThreadPool->waitForDone(msec);
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::stopAllJobs(bool stopPersistentJobs)
{
  Q_D(ctkAbstractScheduler);

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
int ctkAbstractScheduler::maximumThreadCount() const
{
  Q_D(const ctkAbstractScheduler);
  return d->ThreadPool->maxThreadCount();
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::setMaximumThreadCount(int maximumThreadCount)
{
  Q_D(ctkAbstractScheduler);
  d->ThreadPool->setMaxThreadCount(maximumThreadCount);
}

//----------------------------------------------------------------------------
int ctkAbstractScheduler::maximumNumberOfRetry() const
{
  Q_D(const ctkAbstractScheduler);
  return d->MaximumNumberOfRetry;
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::setMaximumNumberOfRetry(int maximumNumberOfRetry)
{
  Q_D(ctkAbstractScheduler);
  d->MaximumNumberOfRetry = maximumNumberOfRetry;
}

//----------------------------------------------------------------------------
int ctkAbstractScheduler::retryDelay() const
{
  Q_D(const ctkAbstractScheduler);
  return d->RetryDelay;
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::setRetryDelay(int retryDelay)
{
  Q_D(ctkAbstractScheduler);
  d->RetryDelay = retryDelay;
}

//----------------------------------------------------------------------------
QThreadPool *ctkAbstractScheduler::threadPool() const
{
  Q_D(const ctkAbstractScheduler);
  return d->ThreadPool.data();
}

//----------------------------------------------------------------------------
QSharedPointer<QThreadPool> ctkAbstractScheduler::threadPoolShared() const
{
  Q_D(const ctkAbstractScheduler);
  return d->ThreadPool;
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
  emit this->jobStarted(job->toVariant());
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
  emit this->jobCanceled(job->toVariant());
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

  QVariant data = job->toVariant();
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

  QVariant data = job->toVariant();
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  emit this->jobFinished(data);
}

//----------------------------------------------------------------------------
void ctkAbstractScheduler::onQueueJobsInThreadPool()
{
  Q_D(ctkAbstractScheduler);

  QMutexLocker ml(&d->mMutex);
  foreach (QThread::Priority priority, (QList<QThread::Priority>()
                                        << QThread::Priority::HighestPriority
                                        << QThread::Priority::HighPriority
                                        << QThread::Priority::NormalPriority
                                        << QThread::Priority::LowPriority
                                        << QThread::Priority::LowestPriority))
    {
    foreach(QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
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

      QSharedPointer<ctkAbstractWorker> worker =
        QSharedPointer<ctkAbstractWorker>(job->createWorker());
      worker->setScheduler(*this);

      d->Workers.insert(job->jobUID(), worker);
      d->ThreadPool->start(worker.data(), job->priority());
      }
    }
}
