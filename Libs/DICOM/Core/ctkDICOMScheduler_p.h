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

#ifndef __ctkDICOMQueryJobPrivate_h
#define __ctkDICOMQueryJobPrivate_h

// ctkDICOMCore includes
#include "ctkDICOMScheduler.h"

class ctkAbstractWorker;
class ctkAbstractJob;

struct ThumbnailUID
{
  QString studyInstanceUID;
  QString seriesInstanceUID;
  QString SOPInstanceUID;
} ;

//------------------------------------------------------------------------------
class ctkDICOMSchedulerPrivate : public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkDICOMScheduler);

protected:
  ctkDICOMScheduler* const q_ptr;

public:
  ctkDICOMSchedulerPrivate(ctkDICOMScheduler& obj);
  virtual ~ctkDICOMSchedulerPrivate();

  int getSameTypeJobsInThreadPoolQueueOrRunning(QSharedPointer<ctkAbstractJob> job);
  void insertJob(QSharedPointer<ctkAbstractJob> job);
  void removeJob(const QString& jobUID);
  QString generateUniqueJobUID();
  ctkDICOMServer* getServerFromProxyServersByConnectionName(const QString&);

  QSharedPointer<ctkDICOMDatabase> DicomDatabase;
  QSharedPointer<QThreadPool> ThreadPool;
  QList<QSharedPointer<ctkDICOMServer>> Servers;
  QMap<QString, QSharedPointer<ctkAbstractJob>> JobsQueue;
  QMap<QString, QSharedPointer<ctkAbstractWorker>> Workers;
  QMap<QString, QVariant> Filters;
  QMutex mMutex;
  int RetryDelay;
  int MaximumNumberOfRetry;
  int MaximumPatientsQuery;
};

#endif
