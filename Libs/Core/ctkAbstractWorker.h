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

#ifndef __ctkAbstractWorker_h
#define __ctkAbstractWorker_h

// Qt includes
#include <QMutex>
#include <QRunnable>
#include <QSharedPointer>

// CTK includes
#include "ctkCoreExport.h"

class ctkAbstractJob;
class ctkJobScheduler;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkAbstractWorker : public QObject, public QRunnable
{
  Q_OBJECT

public:
  explicit ctkAbstractWorker();
  virtual ~ctkAbstractWorker();

  /// Execute worker. This method is run by the QThreadPool and is thread safe
  virtual void run() = 0;

  /// Cancel worker. This method is thread safe
  virtual void requestCancel() = 0;

  ///@{
  /// Job.
  /// These methods are not thread safe
  Q_INVOKABLE ctkAbstractJob* job() const;
  QSharedPointer<ctkAbstractJob> jobShared() const;
  Q_INVOKABLE void setJob(ctkAbstractJob& job);
  virtual void setJob(QSharedPointer<ctkAbstractJob> job);
  ///@}

  ///@{
  /// Scheduler
  /// These methods are not thread safe
  Q_INVOKABLE ctkJobScheduler* scheduler() const;
  QSharedPointer<ctkJobScheduler> schedulerShared() const;
  Q_INVOKABLE void setScheduler(ctkJobScheduler& scheduler);
  void setScheduler(QSharedPointer<ctkJobScheduler> scheduler);
  ///@}

public slots:
  /// These slots are thread safe
  virtual void startNextJob();
  virtual void onJobCanceled(const bool& wasCanceled);

protected:
  QSharedPointer<ctkAbstractJob> Job;
  QSharedPointer<ctkJobScheduler> Scheduler;

private:
  Q_DISABLE_COPY(ctkAbstractWorker)
};


#endif // ctkAbstractWorker_h
