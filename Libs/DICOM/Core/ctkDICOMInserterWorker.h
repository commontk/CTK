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

#ifndef __ctkDICOMInserterWorker_h
#define __ctkDICOMInserterWorker_h

// Qt includes 
#include <QObject>
#include <QMap>
#include <QVariant>
#include <QSharedPointer>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMWorker.h"

class ctkDICOMDatabase;
class ctkDICOMInserter;
class ctkDICOMInserterWorkerPrivate;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMInserterWorker : public ctkDICOMWorker
{
  Q_OBJECT

public:
  typedef ctkDICOMWorker Superclass;
  explicit ctkDICOMInserterWorker();
  virtual ~ctkDICOMInserterWorker();

  /// Execute worker
  void run();

  /// Cancel worker
  void cancel();

  /// Job
  Q_INVOKABLE void setJob(ctkAbstractJob& job);
  void setJob(QSharedPointer<ctkAbstractJob> job);

  /// Inserter
  QSharedPointer<ctkDICOMInserter> inserterShared() const;
  Q_INVOKABLE ctkDICOMInserter* inserter() const;

protected:
  QScopedPointer<ctkDICOMInserterWorkerPrivate> d_ptr;

  /// Constructor allowing derived class to specify a specialized pimpl.
  ///
  /// \note You are responsible to call init() in the constructor of
  /// derived class. Doing so ensures the derived class is fully
  /// instantiated in case virtual method are called within init() itself.
  ctkDICOMInserterWorker(ctkDICOMInserterWorkerPrivate* pimpl);

private:
  Q_DECLARE_PRIVATE(ctkDICOMInserterWorker);
  Q_DISABLE_COPY(ctkDICOMInserterWorker);
};

#endif
