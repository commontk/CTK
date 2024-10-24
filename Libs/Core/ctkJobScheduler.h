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

#ifndef __ctkJobScheduler_h
#define __ctkJobScheduler_h

// Qt includes
#include <QObject>
#include <QSharedPointer>
#include <QVariant>
class QThreadPool;

// CTK includes
#include "ctkCoreExport.h"
class ctkAbstractJob;
class ctkJobSchedulerPrivate;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkJobScheduler : public QObject
{
  Q_OBJECT
  Q_PROPERTY(int numberOfJobs READ numberOfJobs);
  Q_PROPERTY(int numberOfPersistentJobs READ numberOfPersistentJobs);
  Q_PROPERTY(int numberOfRunningJobs READ numberOfRunningJobs);
  Q_PROPERTY(int freezeJobsScheduling READ freezeJobsScheduling WRITE setFreezeJobsScheduling);
  Q_PROPERTY(int maximumThreadCount READ maximumThreadCount WRITE setMaximumThreadCount);
  Q_PROPERTY(int maximumNumberOfRetry READ maximumNumberOfRetry WRITE setMaximumNumberOfRetry);
  Q_PROPERTY(int retryDelay READ retryDelay WRITE setRetryDelay);

public:
  typedef QObject Superclass;
  explicit ctkJobScheduler(QObject* parent = 0);
  virtual ~ctkJobScheduler();

  ///@{
  /// Jobs managment
  int numberOfJobs();
  int numberOfPersistentJobs();
  int numberOfRunningJobs();
  Q_INVOKABLE virtual void addJob(ctkAbstractJob* job);
  Q_INVOKABLE virtual void resetJob(const QString& jobUID);
  Q_INVOKABLE virtual void deleteJob(const QString& jobUID);
  Q_INVOKABLE virtual void deleteJobs(const QStringList& jobUIDs);
  Q_INVOKABLE virtual void deleteWorker(const QString& jobUID);
  QSharedPointer<ctkAbstractJob> getJobSharedByUID(const QString& jobUID);
  Q_INVOKABLE ctkAbstractJob* getJobByUID(const QString& jobUID);
  Q_INVOKABLE void waitForFinish(bool waitForPersistentJobs = false,
                                 bool processEvents = false);
  Q_INVOKABLE void waitForDone(int msec = -1);
  Q_INVOKABLE QStringList stopAllJobs(bool stopPersistentJobs = false, bool removeJobs = true);
  Q_INVOKABLE void stopJobsByJobUIDs(const QStringList& jobUIDs, bool removeJobs = false);
  Q_INVOKABLE bool retryJob(const QString& jobUID);
  Q_INVOKABLE void retryJobs(const QStringList& jobUIDs);
  ///@}

  ///@{
  /// if set to true, new jobs will not be queued
  /// default: false
  bool freezeJobsScheduling() const;
  void setFreezeJobsScheduling(const bool& freezeJobsScheduling);
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

Q_SIGNALS:
  void jobInitialized(QVariant);
  void jobQueued(QVariant);
  void jobStarted(QList<QVariant>);
  void jobUserStopped(QList<QVariant>);
  void jobFinished(QList<QVariant>);
  void jobAttemptFailed(QList<QVariant>);
  void jobFailed(QList<QVariant>);
  void progressJobDetail(QList<QVariant>);

public Q_SLOTS:
  virtual void onJobStarted(ctkAbstractJob*);
  virtual void onJobUserStopped(ctkAbstractJob*);
  virtual void onJobFinished(ctkAbstractJob*);
  virtual void onJobAttemptFailed(ctkAbstractJob*);
  virtual void onJobFailed(ctkAbstractJob*);
  virtual void onProgressJobDetail(QVariant);
  virtual void emitThrottledSignals();

protected:
  QScopedPointer<ctkJobSchedulerPrivate> d_ptr;
  ctkJobScheduler(ctkJobSchedulerPrivate* pimpl, QObject* parent);

private:
  Q_DECLARE_PRIVATE(ctkJobScheduler);
  Q_DISABLE_COPY(ctkJobScheduler)
};

#endif // ctkJobScheduler_h
