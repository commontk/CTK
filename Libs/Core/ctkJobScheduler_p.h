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

=========================================================================*/

#ifndef __ctkJobSchedulerPrivate_h
#define __ctkJobSchedulerPrivate_h

// Qt includes
#include <QReadWriteLock>
#include <QSharedPointer>
#include <QTimer>
class QThreadPool;

// ctkCore includes
#include "ctkCoreExport.h"
class ctkAbstractJob;
class ctkAbstractWorker;

// ctkDICOMCore includes
#include "ctkJobScheduler.h"

//------------------------------------------------------------------------------
class CTK_CORE_EXPORT ctkJobSchedulerPrivate : public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkJobScheduler)

protected:
  ctkJobScheduler* const q_ptr;

public:
  ctkJobSchedulerPrivate(ctkJobScheduler& object);
  virtual ~ctkJobSchedulerPrivate();

  /// Convenient setup methods
  virtual void init();

  virtual bool insertJob(QSharedPointer<ctkAbstractJob> job);
  virtual bool cleanJob(const QString& jobUID);
  virtual void cleanJobs(const QStringList& jobUIDs);
  virtual bool removeJob(const QString& jobUID);
  virtual void removeJobs(const QStringList& jobUIDs);
  virtual int getSameTypeJobsInThreadPoolQueueOrRunning(QSharedPointer<ctkAbstractJob> job);
  virtual QString generateUniqueJobUID();
  virtual void queueJobsInThreadPool();
  virtual void clearBactchedJobsLists();

  QReadWriteLock QueueLock;

  int RetryDelay{100};
  int MaximumNumberOfRetry{3};
  bool FreezeJobsScheduling{false};

  QSharedPointer<QThreadPool> ThreadPool;
  QMap<QString, QSharedPointer<ctkAbstractJob>> JobsQueue;
  QMap<QString, QMap<QString, QMetaObject::Connection>> JobsConnections;
  QMap<QString, QSharedPointer<ctkAbstractWorker>> Workers;
  QMap<QString, int> RunningJobsByJobClass;
  QList<QVariant> BatchedJobsStarted;
  QList<QVariant> BatchedJobsUserStopped;
  QList<QVariant> BatchedJobsFinished;
  QList<QVariant> BatchedJobsAttemptFailed;
  QList<QVariant> BatchedJobsFailed;
  QList<QVariant> BatchedJobsProgress;
  QSharedPointer<QTimer> ThrottleTimer;
  int ThrottleTimeInterval{300};
  int MaximumBatchedSignalsForTimeInterval{20};
};

#endif
