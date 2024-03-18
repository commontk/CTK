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
  QObject::connect(this, SIGNAL(queueJobs()),
                   this, SLOT(onQueueJobsInThreadPool()));

  this->ThreadPool = QSharedPointer<QThreadPool>(new QThreadPool());
  this->ThreadPool->setMaxThreadCount(20);
}

//------------------------------------------------------------------------------
void ctkJobSchedulerPrivate::onQueueJobsInThreadPool()
{
  Q_Q(ctkJobScheduler);

  if (this->FreezeJobsScheduling)
    {
    return;
    }

  this->QueueMutex.lock();
  foreach (QThread::Priority priority, (QList<QThread::Priority>()
                                        << QThread::Priority::HighestPriority
                                        << QThread::Priority::HighPriority
                                        << QThread::Priority::NormalPriority
                                        << QThread::Priority::LowPriority
                                        << QThread::Priority::LowestPriority))
  {
    foreach (QSharedPointer<ctkAbstractJob> job, this->JobsQueue)
    {
      if (job->priority() != priority)
      {
        continue;
      }

      if (job->status() != ctkAbstractJob::JobStatus::Initialized)
      {
        continue;
      }

      int numberOfRunningJobsWithSameType = this->getSameTypeJobsInThreadPoolQueueOrRunning(job);
      if (numberOfRunningJobsWithSameType >= job->maximumConcurrentJobsPerType())
      {
        continue;
      }

      logger.debug(QString("ctkDICOMScheduler: creating worker for job %1 in thread %2.\n")
                       .arg(job->jobUID())
                       .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

      job->setStatus(ctkAbstractJob::JobStatus::Queued);
      emit q->jobQueued(job->toVariant());

      QSharedPointer<ctkAbstractWorker> worker =
        QSharedPointer<ctkAbstractWorker>(job->createWorker());
      worker->setScheduler(*q);

      this->Workers.insert(job->jobUID(), worker);
      this->ThreadPool->start(worker.data(), job->priority());
    }
  }
  this->QueueMutex.unlock();
}

//------------------------------------------------------------------------------
bool ctkJobSchedulerPrivate::insertJob(QSharedPointer<ctkAbstractJob> job)
{
  Q_Q(ctkJobScheduler);

  if (!job)
  {
    return false;
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

  this->QueueMutex.lock();
  this->JobsQueue.insert(job->jobUID(), job);
  this->QueueMutex.unlock();

  emit q->jobInitialized(job->toVariant());

  if (this->FreezeJobsScheduling)
    {
    logger.debug(QString("ctkJobScheduler: job object %1 of type %2 in thread %3 "
                         "not added to the list since jobs are being stopped.\n")
      .arg(job->jobUID())
      .arg(job->className())
      .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));
    return false;
    }

  emit this->queueJobs();

  return true;
}

//------------------------------------------------------------------------------
bool ctkJobSchedulerPrivate::removeJob(const QString& jobUID)
{
  Q_Q(ctkJobScheduler);

  logger.debug(QString("ctkJobScheduler: deleting job object %1 in thread %2.\n")
    .arg(jobUID)
    .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId()), 16)));

  this->QueueMutex.lock();
  QSharedPointer<ctkAbstractJob> job = this->JobsQueue.value(jobUID);
  if (!job)
  {
    return false;
  }

  QObject::disconnect(job.data(), SIGNAL(started()), q, SLOT(onJobStarted()));
  QObject::disconnect(job.data(), SIGNAL(canceled()), q, SLOT(onJobCanceled()));
  QObject::disconnect(job.data(), SIGNAL(failed()), q, SLOT(onJobFailed()));
  QObject::disconnect(job.data(), SIGNAL(finished()), q, SLOT(onJobFinished()));
  QObject::disconnect(job.data(), SIGNAL(progressJobDetail(QVariant)), q, SIGNAL(progressJobDetail(QVariant)));

  this->JobsQueue.remove(jobUID);
  this->QueueMutex.unlock();

  emit this->queueJobs();

  return true;
}

//------------------------------------------------------------------------------
void ctkJobSchedulerPrivate::removeJobs(const QStringList &jobUIDs)
{
  Q_Q(ctkJobScheduler);

  QList<QVariant> datas;
  this->QueueMutex.lock();
  foreach (QString jobUID, jobUIDs)
  {
    QSharedPointer<ctkAbstractJob> job = this->JobsQueue.value(jobUID);
    if (!job)
    {
      continue;
    }

    datas.append(job->toVariant());

    QObject::disconnect(job.data(), SIGNAL(started()), q, SLOT(onJobStarted()));
    QObject::disconnect(job.data(), SIGNAL(canceled()), q, SLOT(onJobCanceled()));
    QObject::disconnect(job.data(), SIGNAL(failed()), q, SLOT(onJobFailed()));
    QObject::disconnect(job.data(), SIGNAL(finished()), q, SLOT(onJobFinished()));
    QObject::disconnect(job.data(), SIGNAL(progressJobDetail(QVariant)), q, SIGNAL(progressJobDetail(QVariant)));

    this->JobsQueue.remove(jobUID);
  }
  this->QueueMutex.unlock();

  foreach (QVariant data, datas)
  {
    emit q->jobCanceled(data);
  }
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
ctkJobScheduler::~ctkJobScheduler()
{
  this->setFreezeJobsScheduling(true);
  this->stopAllJobs(true);
  // stopAllJobs is not main thread blocking. Therefore we need actually
  // to wait the jobs to end (either finished or stopped) before closing the application.
  // Issue: waiting time for the jobs to stop vs waiting the application to close.
  // We should avoid the application crash at exiting.
  // Is 10 sec enough or too long?
  this->waitForDone(10000);
}

//----------------------------------------------------------------------------
int ctkJobScheduler::numberOfJobs()
{
  Q_D(ctkJobScheduler);
  d->QueueMutex.lock();
  int numberOfJobs = d->JobsQueue.count();
  d->QueueMutex.unlock();
  return numberOfJobs;
}

//----------------------------------------------------------------------------
int ctkJobScheduler::numberOfPersistentJobs()
{
  Q_D(ctkJobScheduler);
  int numberOfPersistentJobs = 0;
  d->QueueMutex.lock();
  foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
  {
    if (job->isPersistent())
    {
      numberOfPersistentJobs++;
    }
  }
  d->QueueMutex.unlock();

  return numberOfPersistentJobs;
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

  d->QueueMutex.lock();
  QMap<QString, QSharedPointer<ctkAbstractJob>>::iterator it = d->JobsQueue.find(jobUID);
  if (it == d->JobsQueue.end())
  {
    d->QueueMutex.unlock();
    return nullptr;
  }
  QSharedPointer<ctkAbstractJob> job =  d->JobsQueue.value(jobUID);
  d->QueueMutex.unlock();
  return job;
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

  QStringList initializedStoppedJobsUIDs;
  d->QueueMutex.lock();
  // Stops jobs without a worker (in waiting, still in main thread).
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

    // For this job, a worker has not beed started yet and the job is still in the main thread.
    // There is no worry that it will be started in meanwhile,
    // because only jobs with status Initialized will be started by the scheduler.
    // Therefore we set the status as stopped, because effettivelty the job has been stopped.
    // In addition, to speedup the cleaning of jobs, we remove them with one call removeJobs,
    // instead of using the signal canceled -> onJobCanceled
    QObject::disconnect(job.data(), SIGNAL(canceled()), this, SLOT(onJobCanceled()));
    job->setStatus(ctkAbstractJob::JobStatus::Stopped);
    initializedStoppedJobsUIDs.append(job->jobUID());
  }

  // Try to stop jobs with a worker, but still not running.
  // (in queue in the QThreadPool, still in the main thread)
  foreach (QSharedPointer<ctkAbstractWorker> worker, d->Workers)
  {
    QSharedPointer<ctkAbstractJob> job = worker->jobShared();
    if (!job)
    {
      continue;
    }

    // trytake stops workers not already running
    // these corresponds to jobs with status Queued
    if (d->ThreadPool->tryTake(worker.data()))
    {
      this->deleteWorker(job->jobUID());
      QObject::disconnect(job.data(), SIGNAL(canceled()), this, SLOT(onJobCanceled()));
      job->setStatus(ctkAbstractJob::JobStatus::Stopped);
      initializedStoppedJobsUIDs.append(job->jobUID());
    }
  }

  d->QueueMutex.unlock();
  d->removeJobs(initializedStoppedJobsUIDs);

  // Stops queued and running jobs
  foreach (QSharedPointer<ctkAbstractWorker> worker, d->Workers)
  {
    QSharedPointer<ctkAbstractJob> job = worker->jobShared();
    if (job->isPersistent() && !stopPersistentJobs)
    {
      continue;
    }

    worker->requestCancel();
  }
}

//----------------------------------------------------------------------------
void ctkJobScheduler::stopJobsByJobUIDs(const QStringList &jobUIDs)
{
  Q_D(ctkJobScheduler);

  if (jobUIDs.count() == 0)
  {
    return;
  }

  QStringList initializedStoppedJobsUIDs;
  d->QueueMutex.lock();
  // Stops jobs without a worker (in waiting, still in main thread)
  foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
  {
    if (!job)
    {
      continue;
    }

    if (job->status() != ctkAbstractJob::JobStatus::Initialized)
    {
      continue;
    }

    if ((!job->jobUID().isEmpty() && jobUIDs.contains(job->jobUID())))
    {
      // For this job, a worker has not beed started yet and the job is still in the main thread.
      // There is no worry that it will be started in meanwhile,
      // because only jobs with status Initialized will be started by the scheduler.
      // Therefore we set the status as stopped, because effettivelty the job has been stopped.
      // In addition, to speedup the cleaning of jobs, we remove them with one call removeJobs,
      // instead of using the signal canceled -> onJobCanceled
      QObject::disconnect(job.data(), SIGNAL(canceled()), this, SLOT(onJobCanceled()));
      job->setStatus(ctkAbstractJob::JobStatus::Stopped);
      initializedStoppedJobsUIDs.append(job->jobUID());
    }
  }

  // Try to stop jobs with a worker, but still not running.
  // (in queue in the QThreadPool, still in the main thread)
  foreach (QSharedPointer<ctkAbstractWorker> worker, d->Workers)
  {
    QSharedPointer<ctkAbstractJob> job = worker->jobShared();
    if (!job)
    {
      continue;
    }

    if ((!job->jobUID().isEmpty() && jobUIDs.contains(job->jobUID())))
    {
      // trytake stops workers not already running,
      // these corresponds to jobs with status Queued
      if (d->ThreadPool->tryTake(worker.data()))
      {
        this->deleteWorker(job->jobUID());
        QObject::disconnect(job.data(), SIGNAL(canceled()), this, SLOT(onJobCanceled()));
        job->setStatus(ctkAbstractJob::JobStatus::Stopped);
        initializedStoppedJobsUIDs.append(job->jobUID());
      }
    }
  }

  d->QueueMutex.unlock();
  d->removeJobs(initializedStoppedJobsUIDs);

  // Stops running jobs
  foreach (QSharedPointer<ctkAbstractWorker> worker, d->Workers)
  {
    QSharedPointer<ctkAbstractJob> job = worker->jobShared();
    if (!job)
    {
      continue;
    }

    if ((!job->jobUID().isEmpty() && jobUIDs.contains(job->jobUID())))
    {
      worker->requestCancel();
    }
  }
}

//----------------------------------------------------------------------------
bool ctkJobScheduler::freezeJobsScheduling() const
{
  Q_D(const ctkJobScheduler);
  return d->FreezeJobsScheduling;
}

//----------------------------------------------------------------------------
void ctkJobScheduler::setFreezeJobsScheduling(bool freezeJobsScheduling)
{
  Q_D(ctkJobScheduler);
  d->FreezeJobsScheduling = freezeJobsScheduling;
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
