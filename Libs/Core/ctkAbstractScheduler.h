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

#ifndef __ctkAbstractScheduler_h
#define __ctkAbstractScheduler_h

// Qt includes
#include <QObject>
#include <QSharedPointer>
#include <QVariant>
class QThreadPool;

// CTK includes
#include "ctkCoreExport.h"
class ctkAbstractJob;
class ctkAbstractSchedulerPrivate;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkAbstractScheduler : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  explicit ctkAbstractScheduler(QObject* parent = 0);
  virtual ~ctkAbstractScheduler();

  ///@{
  /// Jobs managment
  Q_INVOKABLE int numberOfJobs();
  Q_INVOKABLE int numberOfPersistentJobs();

  Q_INVOKABLE void addJob(ctkAbstractJob* job);

  Q_INVOKABLE virtual void deleteJob(const QString& jobUID);
  Q_INVOKABLE virtual void deleteWorker(const QString& jobUID);

  QSharedPointer<ctkAbstractJob> getJobSharedByUID(const QString& jobUID);
  Q_INVOKABLE ctkAbstractJob* getJobByUID(const QString& jobUID);

  Q_INVOKABLE void waitForFinish();
  Q_INVOKABLE void waitForDone(int msec);

  Q_INVOKABLE void stopAllJobs(bool stopPersistentJobs = false);
  ///@}

  ///@{
  /// Maximum number of concurrent QThreads spawned by the threadPool in the Job pool
  /// default: 20
  int maximumThreadCount() const;
  void setMaximumThreadCount(const int& maximumThreadCount);
  ///@}

  ///@{
  /// Maximum number of retries that the Job pool will try on each failed Job
  /// default: 3
  int maximumNumberOfRetry() const;
  void setMaximumNumberOfRetry(const int& maximumNumberOfRetry);
  ///@}

  ///@{
  /// Retry delay in millisec
  /// default: 100 msec
  int retryDelay() const;
  void setRetryDelay(const int& retryDelay);
  ///@}

  /// Return the threadPool.
  Q_INVOKABLE QThreadPool* threadPool() const;

  /// Return threadPool as a shared pointer
  /// (not Python-wrappable).
  QSharedPointer<QThreadPool> threadPoolShared() const;

  /// Utility method to transform/pass informations between threads by qt signals
  Q_INVOKABLE virtual QVariant jobToDetail(ctkAbstractJob* job);

Q_SIGNALS:
  void jobStarted(QVariant data);
  void jobFinished(QVariant data);
  void jobCanceled(QVariant data);
  void jobFailed(QVariant data);
  void queueJobs();
  void progressJobDetail(QVariant data);

public Q_SLOTS:
  virtual void onJobStarted();
  virtual void onJobFinished();
  virtual void onJobCanceled();
  virtual void onJobFailed();
  virtual void onQueueJobsInThreadPool();

protected:
  QScopedPointer<ctkAbstractSchedulerPrivate> d_ptr;
  ctkAbstractScheduler(ctkAbstractSchedulerPrivate* pimpl, QObject* parent);

private:
  Q_DECLARE_PRIVATE(ctkAbstractScheduler);
  Q_DISABLE_COPY(ctkAbstractScheduler)
};

#endif // ctkAbstractScheduler_h
