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
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

#ifndef __ctkDICOMThumbnailGeneratorWorker_h
#define __ctkDICOMThumbnailGeneratorWorker_h

// Qt includes
#include <QObject>
#include <QSharedPointer>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkAbstractWorker.h"
class ctkDICOMThumbnailGenerator;
class ctkDICOMThumbnailGeneratorWorkerPrivate;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMThumbnailGeneratorWorker : public ctkAbstractWorker
{
  Q_OBJECT

public:
  typedef ctkAbstractWorker Superclass;
  explicit ctkDICOMThumbnailGeneratorWorker();
  virtual ~ctkDICOMThumbnailGeneratorWorker();

  /// Execute worker. This method is run by the QThreadPool and is thread safe
  void run() override;

  /// Cancel worker. This method is thread safe
  void requestCancel() override;

  ///@{
  /// Job.
  /// These methods are not thread safe
  void setJob(QSharedPointer<ctkAbstractJob> job) override;
  using ctkAbstractWorker::setJob;
  ///@}

protected:
  QScopedPointer<ctkDICOMThumbnailGeneratorWorkerPrivate> d_ptr;

  /// Constructor allowing derived class to specify a specialized pimpl.
  ///
  /// \note You are responsible to call init() in the constructor of
  /// derived class. Doing so ensures the derived class is fully
  /// instantiated in case virtual method are called within init() itself.
  ctkDICOMThumbnailGeneratorWorker(ctkDICOMThumbnailGeneratorWorkerPrivate* pimpl);

private:
  Q_DECLARE_PRIVATE(ctkDICOMThumbnailGeneratorWorker);
  Q_DISABLE_COPY(ctkDICOMThumbnailGeneratorWorker);
};

#endif
