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

#ifndef __ctkDICOMWorker_h
#define __ctkDICOMWorker_h

// Qt includes
#include <QSharedPointer>

// ctkDICOMCore includes
#include "ctkCoreExport.h"
#include "ctkAbstractWorker.h"

class ctkDICOMJob;
class ctkDICOMScheduler;

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkDICOMWorker : public ctkAbstractWorker
{
  Q_OBJECT

public:
  typedef ctkAbstractWorker Superclass;
  explicit ctkDICOMWorker();
  virtual ~ctkDICOMWorker();

  /// Execute worker
  virtual void run() = 0;

  /// Cancel worker
  virtual void cancel() = 0;

public slots:
  void startNextJob();
  void onJobCanceled();

private:
  Q_DISABLE_COPY(ctkDICOMWorker)
};


#endif // ctkDICOMWorker_h
