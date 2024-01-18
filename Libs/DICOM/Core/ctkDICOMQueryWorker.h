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

#ifndef __ctkDICOMQueryWorker_h
#define __ctkDICOMQueryWorker_h

// Qt includes
#include <QMap>
#include <QObject>
#include <QSharedPointer>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMWorker.h"
class ctkDICOMQuery;
class ctkDICOMQueryWorkerPrivate;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMQueryWorker : public ctkDICOMWorker
{
  Q_OBJECT

public:
  typedef ctkDICOMWorker Superclass;
  explicit ctkDICOMQueryWorker();
  virtual ~ctkDICOMQueryWorker();

  /// Execute worker
  void run() override;

  /// Cancel worker
  void cancel() override;

  /// Job
  void setJob(QSharedPointer<ctkAbstractJob> job) override;
  using ctkAbstractWorker::setJob;

  ///@{
  /// Querier
  QSharedPointer<ctkDICOMQuery> querierShared() const;
  Q_INVOKABLE ctkDICOMQuery* querier() const;
  ///@}

protected:
  QScopedPointer<ctkDICOMQueryWorkerPrivate> d_ptr;

  /// Constructor allowing derived class to specify a specialized pimpl.
  ///
  /// \note You are responsible to call init() in the constructor of
  /// derived class. Doing so ensures the derived class is fully
  /// instantiated in case virtual method are called within init() itself.
  ctkDICOMQueryWorker(ctkDICOMQueryWorkerPrivate* pimpl);

private:
  Q_DECLARE_PRIVATE(ctkDICOMQueryWorker);
  Q_DISABLE_COPY(ctkDICOMQueryWorker);
};

#endif
