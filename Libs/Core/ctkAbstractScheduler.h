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
#include <QVariant>

// CTK includes
#include "ctkCoreExport.h"

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkAbstractScheduler : public QObject
{
  Q_OBJECT
public:
  explicit ctkAbstractScheduler(QObject* parent = 0);
  virtual ~ctkAbstractScheduler();

Q_SIGNALS:
  void jobStarted(QVariant data);
  void jobFinished(QVariant data);
  void jobCanceled(QVariant data);
  void jobFailed(QVariant data);

public Q_SLOTS:
  virtual void onJobStarted() = 0;
  virtual void onJobFinished() = 0;
  virtual void onJobCanceled() = 0;
  virtual void onJobFailed() = 0;

private:
  Q_DISABLE_COPY(ctkAbstractScheduler)
};


#endif // ctkAbstractScheduler_h
