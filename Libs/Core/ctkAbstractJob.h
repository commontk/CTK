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
  Q_ENUMS(JobStatus);
  Q_PROPERTY(QString jobUID READ jobUID WRITE setJobUID);
  Q_PROPERTY(QString className READ className);
  Q_PROPERTY(JobStatus status READ status WRITE setStatus);
  Q_PROPERTY(bool persistent READ isPersistent WRITE setIsPersistent);
  Q_PROPERTY(bool retryCounter READ retryCounter WRITE setRetryCounter);
  Q_PROPERTY(int maximumNumberOfRetry READ maximumNumberOfRetry WRITE setMaximumNumberOfRetry);
  Q_PROPERTY(int retryDelay READ retryDelay WRITE setRetryDelay);
  Q_PROPERTY(bool maximumConcurrentJobsPerType READ maximumConcurrentJobsPerType WRITE setMaximumConcurrentJobsPerType);
  Q_PROPERTY(QThread::Priority priority READ priority WRITE setPriority);
  Q_PROPERTY(QDateTime creationDateTime READ creationDateTime);
  Q_PROPERTY(QDateTime startDateTime READ startDateTime);
  Q_PROPERTY(QDateTime completionDateTime READ completionDateTime);

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
  /// Queued: a worker is associated to the job and the worker has been inserted in the queue list of the QThreadPool (object owned by the ctkJobScheduler) with a priority
  /// Running: the job is running in another thread by the associated worker.
  /// Stopped: the job has been stopped externally (a cancel request from the worker)
  /// Failed: the job failed internally (logic returns false).
  /// Finished: the job has been run successfully (logic returns true).
  enum JobStatus {
    Initialized = 0,
    Queued,
    Running,
    Stopped,
    Failed,
    Finished,
  };
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
  /// CreationDateTime
  QDateTime creationDateTime() const;
  ///@}

  ///@{
  /// StartDateTime
  QDateTime startDateTime() const;
  ///@}

  ///@{
  /// CompletionDateTime
  QDateTime completionDateTime() const;
  ///@}

  /// Generate worker for job
  Q_INVOKABLE virtual ctkAbstractWorker* createWorker() = 0;

  /// Create a copy of this job
  Q_INVOKABLE virtual ctkAbstractJob* clone() const = 0;

  /// Logger report string formatting for specific job
  Q_INVOKABLE virtual QString loggerReport(const QString& status) const = 0;

  /// Return the QVariant value of this job.
  ///
  /// The value is set using the ctkJobDetail metatype and is used to pass
  /// information between threads using Qt signals.
  /// \sa ctkJobDetail
  Q_INVOKABLE virtual QVariant toVariant();

Q_SIGNALS:
  void started();
  void canceled();
  void failed();
  void finished();

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
  }
  virtual ~ctkJobDetail() = default;

  QString JobClass;
  QString JobUID;
  QString CreationDateTime;
  QString StartDateTime;
  QString CompletionDateTime;
};
Q_DECLARE_METATYPE(ctkJobDetail);

#endif // ctkAbstractJob_h
