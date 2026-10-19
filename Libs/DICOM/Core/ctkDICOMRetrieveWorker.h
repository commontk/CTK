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

#ifndef __ctkDICOMRetrieveWorker_h
#define __ctkDICOMRetrieveWorker_h

// Qt includes
#include <QList>
#include <QObject>
#include <QSharedPointer>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkAbstractWorker.h"
class ctkDICOMJobResponseSet;
class ctkDICOMRetrieve;
class ctkDICOMRetrieveWorkerPrivate;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMRetrieveWorker : public ctkAbstractWorker
{
  Q_OBJECT

public:
  typedef ctkAbstractWorker Superclass;
  explicit ctkDICOMRetrieveWorker(QObject* parent = nullptr);
  virtual ~ctkDICOMRetrieveWorker();

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

  ///@{
  /// Retriever.
  /// These methods are not thread safe
  QSharedPointer<ctkDICOMRetrieve> retrieverShared() const;
  Q_INVOKABLE ctkDICOMRetrieve* retriever() const;
  ///@}

protected Q_SLOTS:
  /// Insert a batch of frames that the retriever accumulated, and drop them from
  /// the retriever so that their memory is released before the operation ends.
  /// Connected to ctkDICOMRetrieve::framesBatchReady with a direct connection, so
  /// it runs in the thread performing the retrieve.
  void onFramesBatchReady(const QList<QSharedPointer<ctkDICOMJobResponseSet>>& jobResponseSets);

protected:
  QScopedPointer<ctkDICOMRetrieveWorkerPrivate> d_ptr;

  /// Constructor allowing derived class to specify a specialized pimpl.
  ///
  /// \note You are responsible to call init() in the constructor of
  /// derived class. Doing so ensures the derived class is fully
  /// instantiated in case virtual method are called within init() itself.
  ctkDICOMRetrieveWorker(ctkDICOMRetrieveWorkerPrivate* pimpl);

private:
  Q_DECLARE_PRIVATE(ctkDICOMRetrieveWorker);
  Q_DISABLE_COPY(ctkDICOMRetrieveWorker);
};

#endif
