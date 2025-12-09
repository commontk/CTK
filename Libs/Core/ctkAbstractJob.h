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

#ifndef __ctkAbstractJob_h
#define __ctkAbstractJob_h

// Qt includes
#include <QDateTime>
#include <QMetaEnum>
#include <QObject>
#include <QThread>
#include <QVariant>

// CTK includes
#include "ctkCoreExport.h"

class ctkAbstractWorker;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkAbstractJob : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString jobUID READ jobUID WRITE setJobUID NOTIFY jobUIDChanged);
  Q_PROPERTY(QString className READ className CONSTANT);
  Q_PROPERTY(JobStatus status READ status WRITE setStatus NOTIFY statusChanged);
  Q_PROPERTY(bool persistent READ isPersistent WRITE setIsPersistent NOTIFY isPersistentChanged);
  Q_PROPERTY(bool retryCounter READ retryCounter WRITE setRetryCounter NOTIFY retryCounterChanged);
  Q_PROPERTY(int maximumNumberOfRetry READ maximumNumberOfRetry WRITE setMaximumNumberOfRetry NOTIFY maximumNumberOfRetryChanged);

  Q_PROPERTY(int retryDelay READ retryDelay WRITE setRetryDelay NOTIFY retryDelayChanged);
  Q_PROPERTY(bool maximumConcurrentJobsPerType READ maximumConcurrentJobsPerType WRITE setMaximumConcurrentJobsPerType NOTIFY maximumConcurrentJobsPerTypeChanged);
  Q_PROPERTY(QThread::Priority priority READ priority WRITE setPriority NOTIFY priorityChanged);
  Q_PROPERTY(QDateTime creationDateTime READ creationDateTime CONSTANT);
  Q_PROPERTY(QDateTime startDateTime READ startDateTime CONSTANT);
  Q_PROPERTY(QDateTime completionDateTime READ completionDateTime CONSTANT);
  Q_PROPERTY(QString runningThreadID READ runningThreadID WRITE setRunningThreadID NOTIFY runningThreadIDChanged);
  Q_PROPERTY(QString log READ log CONSTANT);
  Q_PROPERTY(bool destroyAfterUse READ destroyAfterUse WRITE setDestroyAfterUse NOTIFY destroyAfterUseChanged);

public:
  explicit ctkAbstractJob();
  virtual ~ctkAbstractJob();

  ///@{
  /// Job UID
  QString jobUID() const;
  virtual void setJobUID(const QString& jobUID);
  ///@}

  /// Class name
  QString className() const;

  ///@{
  /// Status
  /// Initialized: the object has been created and inserted in the JobsQueue map in the ctkJobScheduler
  /// Queued: a worker is associated to the job and the worker has been inserted in the queue list of
  ///         the QThreadPool (object owned by the ctkJobScheduler) with a priority
  /// Running: the job is running in another thread by the associated worker.
  /// UserStopped: the job has been stopped externally (a cancel request from the worker)
  /// AttemptFailed: the job encountered an internal failure, however, the task will be reattempted
  ///                by a different job (as the logic returned false).
  /// Failed: the job failed internally (logic returns false).
  /// Finished: the job has been run successfully (logic returns true).
  enum JobStatus {
    Initialized = 0,
    Queued,
    Running,
    UserStopped,
    AttemptFailed,
    Failed,
    Finished,
  };
  Q_ENUM(JobStatus);
  JobStatus status() const;
  virtual void setStatus(JobStatus status);
  ///@}

  ///@{
  /// Persistent
  bool isPersistent() const;
  void setIsPersistent(bool persistent);
  ///@}

  ///@{
  /// Number of retries: current counter of how many times
  /// the task has been relunched on fails
  int retryCounter() const;
  void setRetryCounter(int retryCounter);
  ///@}

  ///@{
  /// Set the maximum concurrent jobs per job type.
  /// Default value is 20.
  int maximumConcurrentJobsPerType() const;
  void setMaximumConcurrentJobsPerType(int maximumConcurrentJobsPerType);
  ///@}

  ///@{
  /// Maximum number of retries that the Job pool will try on each failed Job
  /// default: 3
  int maximumNumberOfRetry() const;
  void setMaximumNumberOfRetry(int maximumNumberOfRetry);
  ///@}

  ///@{
  /// Retry delay in millisec
  /// default: 100 msec
  int retryDelay() const;
  void setRetryDelay(int retryDelay);
  ///@}

  ///@{
  /// Priority
  QThread::Priority priority() const;
  void setPriority(const QThread::Priority& priority);
  ///@}

  ///@{
  /// Creation Date Time
  QDateTime creationDateTime() const;
  ///@}

  ///@{
  /// Start Date Time
  QDateTime startDateTime() const;
  ///@}

  ///@{
  /// Completion Date Time
  QDateTime completionDateTime() const;
  ///@}

  ///@{
  /// Running ThreadID
  QString runningThreadID() const;
  void setRunningThreadID(QString runningThreadID);
  ///@}

  ///@{
  /// Logged Text
  QString log() const;
  void addLog(QString log);
  ///@}

  /// Generate worker for job
  Q_INVOKABLE virtual ctkAbstractWorker* createWorker() = 0;

  /// Create a copy of this job
  Q_INVOKABLE virtual ctkAbstractJob* clone() const = 0;

  /// Logger report string formatting for specific job
  Q_INVOKABLE virtual QString loggerReport(const QString& status) = 0;

  /// Return the QVariant value of this job.
  ///
  /// The value is set using the ctkJobDetail metatype and is used to pass
  /// information between threads using Qt signals.
  /// \sa ctkJobDetail
  Q_INVOKABLE virtual QVariant toVariant();

  /// Free used resources from job after worker is done
  Q_INVOKABLE virtual void releaseResources() = 0;

  ///@{
  /// Destroy job object after worker is done
  /// default: false
  bool destroyAfterUse() const;
  void setDestroyAfterUse(bool destroyAfterUse);
  ///@}

Q_SIGNALS:
  void started();
  void userStopped();
  void attemptFailed();
  void failed();
  void finished();
  void jobUIDChanged();
  void statusChanged(const ctkAbstractJob::JobStatus & status);
  void isPersistentChanged(bool persistent);
  void retryCounterChanged(bool retryCounter);
  void maximumConcurrentJobsPerTypeChanged(bool maximumConcurrentJobsPerType);
  void maximumNumberOfRetryChanged(int maximumNumberOfRetry);
  void retryDelayChanged(int retryDelay);
  void priorityChanged(const QThread::Priority & priority);
  void runningThreadIDChanged(const QString & runningThreadID);
  void destroyAfterUseChanged(bool destroyAfterUse);

protected:
  QString JobUID;
  JobStatus Status;
  bool Persistent;
  int RetryDelay;
  int RetryCounter;
  int MaximumNumberOfRetry;
  int MaximumConcurrentJobsPerType;
  QThread::Priority Priority;
  QDateTime CreationDateTime;
  QDateTime StartDateTime;
  QDateTime CompletionDateTime;
  QString RunningThreadID;
  QString Log;
  bool DestroyAfterUse;

private:
  Q_DISABLE_COPY(ctkAbstractJob)
};

//------------------------------------------------------------------------------
/// \ingroup Core
struct CTK_CORE_EXPORT ctkJobDetail {
  explicit ctkJobDetail(){}
  explicit ctkJobDetail(const ctkAbstractJob& job)
  {
    this->JobClass = job.className();
    this->JobUID = job.jobUID();
    this->CreationDateTime = job.creationDateTime().toString("HH:mm:ss.zzz ddd dd MMM yyyy");
    this->StartDateTime = job.startDateTime().toString("HH:mm:ss.zzz ddd dd MMM yyyy");
    this->CompletionDateTime = job.completionDateTime().toString("HH:mm:ss.zzz ddd dd MMM yyyy");
    this->RunningThreadID = job.runningThreadID();
    this->Logging = job.log();
  }
  virtual ~ctkJobDetail() = default;

  QString JobClass;
  QString JobUID;
  QString CreationDateTime;
  QString StartDateTime;
  QString CompletionDateTime;
  QString RunningThreadID;
  QString Logging;
};
Q_DECLARE_METATYPE(ctkJobDetail);

#endif // ctkAbstractJob_h
