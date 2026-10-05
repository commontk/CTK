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
  Q_PROPERTY(QString jobUID READ jobUID WRITE setJobUID);
  Q_PROPERTY(QString className READ className);
  Q_PROPERTY(JobStatus status READ status WRITE setStatus);
  Q_PROPERTY(bool persistent READ isPersistent WRITE setIsPersistent);
  Q_PROPERTY(int retryCounter READ retryCounter WRITE setRetryCounter);
  Q_PROPERTY(bool retryEnabled READ retryEnabled WRITE setRetryEnabled);
  Q_PROPERTY(int retryDelay READ retryDelay WRITE setRetryDelay);
  Q_PROPERTY(double retryBackoffFactor READ retryBackoffFactor WRITE setRetryBackoffFactor);
  Q_PROPERTY(int maximumRetryWait READ maximumRetryWait WRITE setMaximumRetryWait);
  Q_PROPERTY(int accumulatedRetryWait READ accumulatedRetryWait WRITE setAccumulatedRetryWait);
  Q_PROPERTY(int maximumConcurrentJobsPerType READ maximumConcurrentJobsPerType WRITE setMaximumConcurrentJobsPerType);
  Q_PROPERTY(QString concurrencyGroup READ concurrencyGroup);
  Q_PROPERTY(int maximumConcurrentJobsPerGroup READ maximumConcurrentJobsPerGroup WRITE setMaximumConcurrentJobsPerGroup);
  Q_PROPERTY(QThread::Priority priority READ priority WRITE setPriority);
  Q_PROPERTY(QDateTime creationDateTime READ creationDateTime);
  Q_PROPERTY(QDateTime startDateTime READ startDateTime);
  Q_PROPERTY(QDateTime completionDateTime READ completionDateTime);
  Q_PROPERTY(QString runningThreadID READ runningThreadID WRITE setRunningThreadID);
  Q_PROPERTY(QString log READ log);
  Q_PROPERTY(bool destroyAfterUse READ destroyAfterUse WRITE setDestroyAfterUse);

public:
  explicit ctkAbstractJob(QObject* parent = nullptr);
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

  /// Name of the group of jobs this job competes with for resources, typically
  /// the remote endpoint the job talks to (e.g. the connection name of a DICOM
  /// server). Jobs of the same group are limited by maximumConcurrentJobsPerGroup,
  /// in addition to the per job type limit.
  ///
  /// An empty group name (the default) means that the job does not belong to any
  /// group and is therefore only limited per job type.
  virtual QString concurrencyGroup() const;

  ///@{
  /// Set the maximum concurrent jobs sharing the same concurrency group.
  /// Ignored if the job has no concurrency group or if the value is zero or negative.
  /// Default value is 8.
  /// \sa concurrencyGroup()
  int maximumConcurrentJobsPerGroup() const;
  void setMaximumConcurrentJobsPerGroup(int maximumConcurrentJobsPerGroup);
  ///@}

  ///@{
  /// If false, a failed job is never reattempted.
  /// default: true
  bool retryEnabled() const;
  void setRetryEnabled(bool retryEnabled);
  ///@}

  ///@{
  /// Delay in millisec before the first retry. Each following retry waits
  /// retryDelay * retryBackoffFactor^retryCounter, with some randomness.
  /// default: 1000 msec
  /// \sa nextRetryDelay()
  int retryDelay() const;
  void setRetryDelay(int retryDelay);
  ///@}

  ///@{
  /// Factor by which the retry delay grows at each attempt.
  /// default: 2.5
  /// \sa nextRetryDelay()
  double retryBackoffFactor() const;
  void setRetryBackoffFactor(double retryBackoffFactor);
  ///@}

  ///@{
  /// Maximum total time in millisec spent waiting between the retries of a job.
  /// Retrying stops when this budget is exhausted, and the job fails.
  /// Zero or a negative value disables retrying.
  /// default: 60000 msec (1 minute)
  int maximumRetryWait() const;
  void setMaximumRetryWait(int maximumRetryWait);
  ///@}

  ///@{
  /// Time in millisec already spent waiting between the previous retries of this
  /// job. It is carried over when a job is reattempted, so that the sum of the
  /// delays of the whole chain of attempts is bounded by maximumRetryWait.
  int accumulatedRetryWait() const;
  void setAccumulatedRetryWait(int accumulatedRetryWait);
  ///@}

  /// Delay in millisec to wait before the next attempt of this job, or -1 if the
  /// job should not be reattempted (retrying disabled or retry budget exhausted).
  /// The delay grows exponentially with the number of attempts, is randomized by
  /// +/-25% so that the jobs of an unresponsive server do not all come back at the
  /// same time, and never exceeds the remaining part of maximumRetryWait.
  Q_INVOKABLE int nextRetryDelay() const;

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

protected:
  QString JobUID;
  JobStatus Status;
  bool Persistent;
  int RetryDelay;
  int RetryCounter;
  bool RetryEnabled;
  double RetryBackoffFactor;
  int MaximumRetryWait;
  int AccumulatedRetryWait;
  int MaximumConcurrentJobsPerType;
  int MaximumConcurrentJobsPerGroup;
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
    this->RetryCounter = job.retryCounter();
    this->AccumulatedRetryWait = job.accumulatedRetryWait();
    this->MaximumRetryWait = job.maximumRetryWait();
  }
  virtual ~ctkJobDetail() = default;

  QString JobClass;
  QString JobUID;
  QString CreationDateTime;
  QString StartDateTime;
  QString CompletionDateTime;
  QString RunningThreadID;
  QString Logging;

  /// Retry bookkeeping, so that the GUI can tell a job that failed once from a job
  /// that kept failing until its retry window elapsed.
  /// \sa ctkAbstractJob::nextRetryDelay
  int RetryCounter{0};
  int AccumulatedRetryWait{0};
  int MaximumRetryWait{0};

  /// True if the job failed after having spent its whole retry waiting time.
  bool retryWaitElapsed() const
  {
    return this->MaximumRetryWait > 0 &&
           this->RetryCounter > 0 &&
           this->AccumulatedRetryWait >= this->MaximumRetryWait;
  }
};
Q_DECLARE_METATYPE(ctkJobDetail);

#endif // ctkAbstractJob_h
