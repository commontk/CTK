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

#ifndef __ctkDICOMStorageListenerJob_h
#define __ctkDICOMStorageListenerJob_h

// Qt includes
#include <QObject>
#include <QSharedPointer>
#include <QString>

// ctkCore includes
class ctkAbstractWorker;

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMJob.h"
class ctkDICOMStorageListenerJobPrivate;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMStorageListenerJob : public ctkDICOMJob
{
  Q_OBJECT
  Q_PROPERTY(int port READ port WRITE setPort);
  Q_PROPERTY(QString AETitle READ AETitle WRITE setAETitle);
  Q_PROPERTY(int connectionTimeout READ connectionTimeout WRITE setConnectionTimeout);

public:
  typedef ctkDICOMJob Superclass;
  explicit ctkDICOMStorageListenerJob();
  virtual ~ctkDICOMStorageListenerJob();

  ///@{
  /// Port, default: 11112
  void setPort(const int& port);
  int port() const;
  ///@}

  ///@{
  /// AETitle, default: CTKSTORE
  void setAETitle(const QString& AETitle);
  QString AETitle() const;
  ///@}

  ///@{
  /// Connection timeout, default 1 sec.
  void setConnectionTimeout(const int& timeout);
  int connectionTimeout() const;
  ///@}

  /// Logger report string formatting for specific task
  Q_INVOKABLE QString loggerReport(const QString& status) const override;

  /// \see ctkAbstractJob::clone()
  Q_INVOKABLE ctkAbstractJob* clone() const override;

  /// Generate worker for job
  Q_INVOKABLE ctkAbstractWorker* createWorker() override;

  /// Return job type.
  Q_INVOKABLE virtual ctkDICOMJobResponseSet::JobType getJobType() const override;

protected:
  QScopedPointer<ctkDICOMStorageListenerJobPrivate> d_ptr;

  /// Constructor allowing derived class to specify a specialized pimpl.
  ///
  /// \note You are responsible to call init() in the constructor of
  /// derived class. Doing so ensures the derived class is fully
  /// instantiated in case virtual method are called within init() itself.
  ctkDICOMStorageListenerJob(ctkDICOMStorageListenerJobPrivate* pimpl);

private:
  Q_DECLARE_PRIVATE(ctkDICOMStorageListenerJob);
  Q_DISABLE_COPY(ctkDICOMStorageListenerJob);
};

#endif
