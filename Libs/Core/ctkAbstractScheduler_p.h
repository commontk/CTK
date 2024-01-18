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
#include <QMutex>
#include <QSharedPointer>
class QThreadPool;

// ctkCore includes
#include "ctkCoreExport.h"
class ctkAbstractJob;
class ctkAbstractWorker;

// ctkDICOMCore includes
#include "ctkAbstractScheduler.h"

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

  virtual void insertJob(QSharedPointer<ctkAbstractJob> job);
  virtual void removeJob(const QString& jobUID);
  int getSameTypeJobsInThreadPoolQueueOrRunning(QSharedPointer<ctkAbstractJob> job);
  QString generateUniqueJobUID();

  QMutex mMutex;

  int RetryDelay{100};
  int MaximumNumberOfRetry{3};

  QSharedPointer<QThreadPool> ThreadPool;
  QMap<QString, QSharedPointer<ctkAbstractJob>> JobsQueue;
  QMap<QString, QSharedPointer<ctkAbstractWorker>> Workers;
};

#endif
