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

// Qt includes
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QString>
class QVariant;

// ctkCore includes
#include <ctkJobScheduler_p.h>
class ctkAbstractJob;
class ctkAbstractWorker;
class ctkDICOMDatabase;
class ctkDICOMServer;

// ctkDICOMCore includes
#include "ctkDICOMScheduler.h"

//------------------------------------------------------------------------------
struct ThumbnailUID
{
  QString studyInstanceUID;
  QString seriesInstanceUID;
  QString SOPInstanceUID;
} ;

//------------------------------------------------------------------------------
class ctkDICOMSchedulerPrivate : public ctkJobSchedulerPrivate
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkDICOMScheduler);

public:
  ctkDICOMSchedulerPrivate(ctkDICOMScheduler& obj);
  virtual ~ctkDICOMSchedulerPrivate();

  bool isServerEnabled(ctkDICOMServer* server, const QStringList& enabledSevers);
  ctkDICOMServer* getServerFromProxyServersByConnectionName(const QString&);

  QSharedPointer<ctkDICOMDatabase> DicomDatabase;
  QList<QSharedPointer<ctkDICOMServer>> Servers;
  QMap<QString, QVariant> Filters;

  int MaximumPatientsQuery{25};
};

#endif
