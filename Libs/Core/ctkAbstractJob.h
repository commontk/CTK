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
#include <QObject>
#include <QThread>

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
  enum JobStatus {
    Initialized = 0,
    Queued,
    Running,
    Stopped,
    Finished,
  };
  JobStatus status() const;
  virtual void setStatus(const JobStatus& status);
  ///@}

  ///@{
  /// Persistent
  bool isPersistent() const;
  void setIsPersistent(const bool& persistent);
  ///@}

  ///@{
  /// Number of retries: current counter of how many times
  /// the task has been relunched on fails
  int retryCounter() const;
  void setRetryCounter(const int& retryCounter);
  ///@}

  ///@{
  /// Set the maximum concurrent jobs per job type.
  /// Default value is 20.
  int maximumConcurrentJobsPerType() const;
  void setMaximumConcurrentJobsPerType(const int& maximumConcurrentJobsPerType);
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

  ///@{
  /// Priority
  QThread::Priority priority() const;
  void setPriority(const QThread::Priority& priority);
  ///@}

  /// Generate worker for job
  Q_INVOKABLE virtual ctkAbstractWorker* createWorker() = 0;

Q_SIGNALS:
  void started();
  void finished();
  void canceled();
  void failed();

protected:
  QString JobUID;
  JobStatus Status;
  bool Persistent;
  int RetryDelay;
  int RetryCounter;
  int MaximumNumberOfRetry;
  int MaximumConcurrentJobsPerType;
  QThread::Priority Priority;

private:
  Q_DISABLE_COPY(ctkAbstractJob)
};


#endif // ctkAbstractJob_h
