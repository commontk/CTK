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
#include <QDebug>
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
  QObject::connect(this, SIGNAL(queueJobsInThreadPool()),
                   this, SLOT(onQueueJobsInThreadPool()));

  this->ThreadPool = QSharedPointer<QThreadPool>(new QThreadPool(this));
  this->ThreadPool->setMaxThreadCount(20);
  this->ThrottleTimer = QSharedPointer<QTimer>(new QTimer(this));
  this->ThrottleTimer->setSingleShot(true);

  QObject::connect(this->ThrottleTimer.data(), SIGNAL(timeout()),
                   q, SLOT(emitThrottledSignals()));
}

//------------------------------------------------------------------------------
void ctkJobSchedulerPrivate::onQueueJobsInThreadPool()
{
  Q_Q(ctkJobScheduler);

  if (this->FreezeJobsScheduling)
  {
    return;
  }

  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&this->QueueMutex);


    foreach (QThread::Priority priority, (QList<QThread::Priority>()
                                          << QThread::Priority::HighestPriority
                                          << QThread::Priority::HighPriority
                                          << QThread::Priority::NormalPriority
                                          << QThread::Priority::LowPriority
                                          << QThread::Priority::LowestPriority))
    {
      foreach (QSharedPointer<ctkAbstractJob> job, this->JobsQueue)
      {
        if (this->FreezeJobsScheduling)
        {
          return;
        }

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
  }
}

//------------------------------------------------------------------------------
bool ctkJobSchedulerPrivate::insertJob(QSharedPointer<ctkAbstractJob> job)
{
  Q_Q(ctkJobScheduler);

  if (!job)
  {
    return false;
  }

  if (this->FreezeJobsScheduling)
  {
    logger.debug(QString("ctkJobScheduler: job object %1 of type %2 in thread %3 "
                         "not added to the job list since jobs are being stopped.\n")
      .arg(job->jobUID())
      .arg(job->className())
      .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));
    return false;
  }

  logger.debug(QString("ctkJobScheduler: creating job object %1 of type %2 in thread %3.\n")
    .arg(job->jobUID())
    .arg(job->className())
    .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId())), 16));

  QMetaObject::Connection startedConnection = QObject::connect(job.data(), &ctkAbstractJob::started, q, [q, job](){
    q->onJobStarted(job.data());
  });
  QMetaObject::Connection userStoppedConnection = QObject::connect(job.data(), &ctkAbstractJob::userStopped, q, [q, job](){
    q->onJobUserStopped(job.data());
  });
  QMetaObject::Connection finishedConnection = QObject::connect(job.data(), &ctkAbstractJob::finished, q, [q, job](){
    q->onJobFinished(job.data());
  });
  QMetaObject::Connection attemptFailedConnection = QObject::connect(job.data(), &ctkAbstractJob::attemptFailed, q, [q, job](){
    q->onJobAttemptFailed(job.data());
  });
  QMetaObject::Connection failedConnection = QObject::connect(job.data(), &ctkAbstractJob::failed, q, [q, job](){
    q->onJobFailed(job.data());
  });
  QMetaObject::Connection progressConnection =
    QObject::connect(job.data(), SIGNAL(progressJobDetail(QVariant)),
                     q, SLOT(onProgressJobDetail(QVariant)));

  QMap<QString, QMetaObject::Connection> connections =
  {
    {"started", startedConnection},
    {"userStopped", userStoppedConnection},
    {"finished", finishedConnection},
    {"attemptFailed", attemptFailedConnection},
    {"failed", failedConnection},
    {"progress", progressConnection},
  };

  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&this->QueueMutex);
    this->JobsQueue.insert(job->jobUID(), job);
    this->JobsConnections.insert(job->jobUID(), connections);
  }

  emit q->jobInitialized(job->toVariant());
  emit this->queueJobsInThreadPool();
  return true;
}

//------------------------------------------------------------------------------
bool ctkJobSchedulerPrivate::removeJob(const QString& jobUID)
{
  Q_Q(ctkJobScheduler);

  logger.debug(QString("ctkJobScheduler: deleting job object %1 in thread %2.\n")
    .arg(jobUID)
    .arg(QString::number(reinterpret_cast<quint64>(QThread::currentThreadId()), 16)));

  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&this->QueueMutex);
    QSharedPointer<ctkAbstractJob> job = this->JobsQueue.value(jobUID);
    if (!job || !this->JobsConnections.contains(jobUID))
    {
      return false;
    }

    QMap<QString, QMetaObject::Connection> connections = this->JobsConnections.value(jobUID);
    QObject::disconnect(connections.value("started"));
    QObject::disconnect(connections.value("userStopped"));
    QObject::disconnect(connections.value("finished"));
    QObject::disconnect(connections.value("attemptFailed"));
    QObject::disconnect(connections.value("failed"));
    QObject::disconnect(connections.value("progress"));

    this->JobsConnections.remove(jobUID);
    this->JobsQueue.remove(jobUID);
  }

  emit this->queueJobsInThreadPool();

  return true;
}

//------------------------------------------------------------------------------
void ctkJobSchedulerPrivate::removeJobs(const QStringList &jobUIDs)
{
  Q_Q(ctkJobScheduler);

  QList<QVariant> datas;
  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&this->QueueMutex);

    foreach (QString jobUID, jobUIDs)
    {
      QSharedPointer<ctkAbstractJob> job = this->JobsQueue.value(jobUID);
      if (!job || !this->JobsConnections.contains(jobUID))
      {
        continue;
      }

      datas.append(job->toVariant());

      QMap<QString, QMetaObject::Connection> connections = this->JobsConnections.value(jobUID);
      QObject::disconnect(connections.value("started"));
      QObject::disconnect(connections.value("userStopped"));
      QObject::disconnect(connections.value("finished"));
      QObject::disconnect(connections.value("attemptFailed"));
      QObject::disconnect(connections.value("failed"));
      QObject::disconnect(connections.value("progress"));

      this->JobsConnections.remove(jobUID);
      this->JobsQueue.remove(jobUID);
    }
  }

  emit q->jobUserStopped(datas);
}

//------------------------------------------------------------------------------
void ctkJobSchedulerPrivate::removeAllJobs()
{
  Q_Q(ctkJobScheduler);

  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&this->QueueMutex);
    foreach (QSharedPointer<ctkAbstractJob> job, this->JobsQueue)
    {
      if (!job)
      {
        continue;
      }

      QString jobUID = job->jobUID();
      if (!this->JobsConnections.contains(jobUID))
      {
        continue;
      }

      QMap<QString, QMetaObject::Connection> connections = this->JobsConnections.value(jobUID);
      QObject::disconnect(connections.value("started"));
      QObject::disconnect(connections.value("userStopped"));
      QObject::disconnect(connections.value("finished"));
      QObject::disconnect(connections.value("attemptFailed"));
      QObject::disconnect(connections.value("failed"));
      QObject::disconnect(connections.value("progress"));

      this->JobsConnections.remove(jobUID);
      this->JobsQueue.remove(jobUID);
    }
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

//------------------------------------------------------------------------------
void ctkJobSchedulerPrivate::clearBactchedJobsLists()
{
  this->BatchedJobsStarted.clear();
  this->BatchedJobsUserStopped.clear();
  this->BatchedJobsFinished.clear();
  this->BatchedJobsAttemptFailed.clear();
  this->BatchedJobsFailed.clear();
  this->BatchedJobsProgress.clear();
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
  // The job scheduler currently waits all the jobs to be properly stopped.
  this->waitForFinish(true);
}

//------------------------------------------------------------------------------
CTK_SET_CPP(ctkJobScheduler, const bool&, setFreezeJobsScheduling, FreezeJobsScheduling);
CTK_GET_CPP(ctkJobScheduler, bool, freezeJobsScheduling, FreezeJobsScheduling)
CTK_SET_CPP(ctkJobScheduler, const int&, setMaximumNumberOfRetry, MaximumNumberOfRetry);
CTK_GET_CPP(ctkJobScheduler, int, maximumNumberOfRetry, MaximumNumberOfRetry)
CTK_SET_CPP(ctkJobScheduler, const int&, setRetryDelay, RetryDelay);
CTK_GET_CPP(ctkJobScheduler, int, retryDelay, RetryDelay)

//----------------------------------------------------------------------------
int ctkJobScheduler::numberOfJobs()
{
  Q_D(ctkJobScheduler);
  int numberOfJobs = 0;
  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&d->QueueMutex);
    numberOfJobs = d->JobsQueue.count();
  }
  return numberOfJobs;
}

//----------------------------------------------------------------------------
int ctkJobScheduler::numberOfPersistentJobs()
{
  Q_D(ctkJobScheduler);
  int numberOfPersistentJobs = 0;
  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&d->QueueMutex);
    foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
    {
      if (job->isPersistent())
      {
        numberOfPersistentJobs++;
      }
    }
  }

  return numberOfPersistentJobs;
}

//----------------------------------------------------------------------------
int ctkJobScheduler::numberOfRunningJobs()
{
  Q_D(ctkJobScheduler);

  int numberOfRunningJobs = 0;
  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&d->QueueMutex);
    foreach (QSharedPointer<ctkAbstractJob> job, d->JobsQueue)
    {
      if (job->status() <= ctkAbstractJob::JobStatus::Running)
      {
        numberOfRunningJobs++;
      }
    }
  }

  return numberOfRunningJobs;
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

  QSharedPointer<ctkAbstractJob> job = nullptr;
  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&d->QueueMutex);
    QMap<QString, QSharedPointer<ctkAbstractJob>>::iterator it = d->JobsQueue.find(jobUID);
    if (it == d->JobsQueue.end())
    {
      return nullptr;
    }
    job = d->JobsQueue.value(jobUID);
  }
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
void ctkJobScheduler::waitForFinish(bool waitForPersistentJobs,
                                    bool processEvents)
{
  this->waitForDone(500);
  if (processEvents)
  {
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
  }

  int numberOfPersistentJobs = this->numberOfPersistentJobs();
  if (waitForPersistentJobs)
  {
    numberOfPersistentJobs = 0;
  }

  while (this->numberOfRunningJobs() > numberOfPersistentJobs)
  {
    this->waitForDone(500);
    if (processEvents)
    {
      qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
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
  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&d->QueueMutex);

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

      QString jobUID = job->jobUID();
      if (!d->JobsConnections.contains(jobUID))
      {
        continue;
      }

      // For this job, a worker has not beed started yet and the job is still in the main thread.
      // There is no worry that it will be started in meanwhile,
      // because only jobs with status Initialized will be started by the scheduler.
      // Therefore we set the status as stopped, because effettivelty the job has been stopped.
      // In addition, to speedup the cleaning of jobs, we remove them with one call removeJobs,
      // instead of using the signal
      QMap<QString, QMetaObject::Connection> connections = d->JobsConnections.value(jobUID);
      QObject::disconnect(connections.value("userStopped"));
      job->setStatus(ctkAbstractJob::JobStatus::UserStopped);
      initializedStoppedJobsUIDs.append(jobUID);
    }
  }

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
  {
    // The QMutexLocker is enclosed within brackets to restrict its scope and
    // prevent conflicts with other QMutexLockers within the scheduler's methods.
    QMutexLocker locker(&d->QueueMutex);

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

      QString jobUID = job->jobUID();
      if (jobUID.isEmpty() || !jobUIDs.contains(jobUID) ||
        !d->JobsConnections.contains(jobUID))
      {
        continue;
      }

      // For this job, a worker has not beed started yet and the job is still in the main thread.
      // There is no worry that it will be started in meanwhile,
      // because only jobs with status Initialized will be started by the scheduler.
      // Therefore we set the status as stopped, because effettivelty the job has been stopped.
      // In addition, to speedup the cleaning of jobs, we remove them with one call removeJobs,
      // instead of using the signal
      QMap<QString, QMetaObject::Connection> connections = d->JobsConnections.value(jobUID);
      QObject::disconnect(connections.value("userStopped"));
      job->setStatus(ctkAbstractJob::JobStatus::UserStopped);
      initializedStoppedJobsUIDs.append(job->jobUID());
    }
  }

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
int ctkJobScheduler::maximumThreadCount() const
{
  Q_D(const ctkJobScheduler);
  return d->ThreadPool->maxThreadCount();
}

//----------------------------------------------------------------------------
void ctkJobScheduler::setMaximumThreadCount(const int& maximumThreadCount)
{
  Q_D(ctkJobScheduler);
  d->ThreadPool->setMaxThreadCount(maximumThreadCount);
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
void ctkJobScheduler::onJobStarted(ctkAbstractJob* job)
{
  Q_D(ctkJobScheduler);
  if (!job)
  {
    return;
  }

  logger.debug(job->loggerReport(tr("started")));

  d->BatchedJobsStarted.append(job->toVariant());
  if (!d->ThrottleTimer->isActive())
  {
    d->ThrottleTimer->start(d->ThrottleTimeInterval);
  }
}

//----------------------------------------------------------------------------
void ctkJobScheduler::onJobUserStopped(ctkAbstractJob* job)
{
  Q_D(ctkJobScheduler);
  if (!job)
  {
    return;
  }

  logger.debug(job->loggerReport(tr("user stopped")));

  QVariant data = job->toVariant();
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  d->BatchedJobsUserStopped.append(job->toVariant());
  if (!d->ThrottleTimer->isActive())
  {
    d->ThrottleTimer->start(d->ThrottleTimeInterval);
  }
}

//----------------------------------------------------------------------------
void ctkJobScheduler::onJobFinished(ctkAbstractJob* job)
{
  Q_D(ctkJobScheduler);
  if (!job)
  {
    return;
  }

  logger.debug(job->loggerReport(tr("finished")));

  QVariant data = job->toVariant();
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  d->BatchedJobsFinished.append(job->toVariant());
  if (!d->ThrottleTimer->isActive())
  {
    d->ThrottleTimer->start(d->ThrottleTimeInterval);
  }
}

//----------------------------------------------------------------------------
void ctkJobScheduler::onJobAttemptFailed(ctkAbstractJob* job)
{
  Q_D(ctkJobScheduler);
  if (!job)
  {
    return;
  }

  logger.debug(job->loggerReport(tr("attempt failed")));

  QVariant data = job->toVariant();
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  d->BatchedJobsAttemptFailed.append(job->toVariant());
  if (!d->ThrottleTimer->isActive())
  {
    d->ThrottleTimer->start(d->ThrottleTimeInterval);
  }
}

//----------------------------------------------------------------------------
void ctkJobScheduler::onJobFailed(ctkAbstractJob* job)
{
  Q_D(ctkJobScheduler);
  if (!job)
  {
    return;
  }

  logger.debug(job->loggerReport(tr("failed")));

  QVariant data = job->toVariant();
  QString jobUID = job->jobUID();
  this->deleteWorker(jobUID);
  this->deleteJob(jobUID);

  d->BatchedJobsFailed.append(job->toVariant());
  if (!d->ThrottleTimer->isActive())
  {
    d->ThrottleTimer->start(d->ThrottleTimeInterval);
  }
}

//----------------------------------------------------------------------------
void ctkJobScheduler::onProgressJobDetail(QVariant data)
{
  Q_D(ctkJobScheduler);

  d->BatchedJobsProgress.append(data);
  if (!d->ThrottleTimer->isActive())
  {
    d->ThrottleTimer->start(d->ThrottleTimeInterval);
  }
}

//----------------------------------------------------------------------------
void ctkJobScheduler::emitThrottledSignals()
{
  Q_D(ctkJobScheduler);

  int totalEmitted = 0;
  if (!d->BatchedJobsStarted.isEmpty() && totalEmitted < d->MaximumBatchedSignalsForTimeInterval)
  {
    int count = qMin(d->MaximumBatchedSignalsForTimeInterval - totalEmitted, d->BatchedJobsStarted.size());
    emit this->jobStarted(d->BatchedJobsStarted.mid(0, count));
    d->BatchedJobsStarted = d->BatchedJobsStarted.mid(count);
    totalEmitted += count;
  }
  if (!d->BatchedJobsUserStopped.isEmpty() && totalEmitted < d->MaximumBatchedSignalsForTimeInterval)
  {
    int count = qMin(d->MaximumBatchedSignalsForTimeInterval - totalEmitted, d->BatchedJobsUserStopped.size());
    emit this->jobUserStopped(d->BatchedJobsUserStopped.mid(0, count));
    d->BatchedJobsUserStopped = d->BatchedJobsUserStopped.mid(count);
    totalEmitted += count;
  }
  if (!d->BatchedJobsFinished.isEmpty() && totalEmitted < d->MaximumBatchedSignalsForTimeInterval)
  {
    int count = qMin(d->MaximumBatchedSignalsForTimeInterval - totalEmitted, d->BatchedJobsFinished.size());
    emit this->jobFinished(d->BatchedJobsFinished.mid(0, count));
    d->BatchedJobsFinished = d->BatchedJobsFinished.mid(count);
    totalEmitted += count;
  }
  if (!d->BatchedJobsAttemptFailed.isEmpty() && totalEmitted < d->MaximumBatchedSignalsForTimeInterval)
  {
    int count = qMin(d->MaximumBatchedSignalsForTimeInterval - totalEmitted, d->BatchedJobsAttemptFailed.size());
    emit this->jobAttemptFailed(d->BatchedJobsAttemptFailed.mid(0, count));
    d->BatchedJobsAttemptFailed = d->BatchedJobsAttemptFailed.mid(count);
    totalEmitted += count;
  }
  if (!d->BatchedJobsFailed.isEmpty() && totalEmitted < d->MaximumBatchedSignalsForTimeInterval)
  {
    int count = qMin(d->MaximumBatchedSignalsForTimeInterval - totalEmitted, d->BatchedJobsFailed.size());
    emit this->jobFailed(d->BatchedJobsFailed.mid(0, count));
    d->BatchedJobsFailed = d->BatchedJobsFailed.mid(count);
    totalEmitted += count;
  }

  emit this->progressJobDetail(d->BatchedJobsProgress);
  d->BatchedJobsProgress.clear();

  int numberOfSignalsNotSent = d->BatchedJobsStarted.size() + d->BatchedJobsUserStopped.size() +
    d->BatchedJobsFinished.size() + d->BatchedJobsAttemptFailed.size() +
    d->BatchedJobsFailed.size();
  if (numberOfSignalsNotSent != 0 && !d->ThrottleTimer->isActive())
  {
    d->ThrottleTimer->start(d->ThrottleTimeInterval);
  }
}
