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

#ifndef __ctkDICOMEchoJob_h
#define __ctkDICOMEchoJob_h

// Qt includes
#include <QObject>
#include <QSharedPointer>

// ctkCore includes
class ctkAbstractWorker;

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMJob.h"
class ctkDICOMEchoJobPrivate;
class ctkDICOMServer;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMEchoJob : public ctkDICOMJob
{
  Q_OBJECT

public:
  typedef ctkDICOMJob Superclass;
  explicit ctkDICOMEchoJob();
  virtual ~ctkDICOMEchoJob();

  ///@{
  /// Server
  Q_INVOKABLE ctkDICOMServer* server() const;
  Q_INVOKABLE void setServer(const ctkDICOMServer& server);
  ///@}

  /// Logger report string formatting for specific task
  Q_INVOKABLE QString loggerReport(const QString& status) const override;

  /// \see ctkAbstractJob::clone()
  Q_INVOKABLE ctkAbstractJob* clone() const override;

  /// Generate worker for job
  Q_INVOKABLE ctkAbstractWorker* createWorker() override;

  /// Return the QVariant value of this job.
  ///
  /// The value is set using the ctkDICOMJobDetail metatype and is used to pass
  /// information between threads using Qt signals.
  /// \sa ctkDICOMJobDetail
  Q_INVOKABLE virtual QVariant toVariant() override;

  /// Return job type.
  Q_INVOKABLE virtual ctkDICOMJobResponseSet::JobType getJobType() const override;

protected:
  QScopedPointer<ctkDICOMEchoJobPrivate> d_ptr;

  /// Constructor allowing derived class to specify a specialized pimpl.
  ///
  /// \note You are responsible to call init() in the constructor of
  /// derived class. Doing so ensures the derived class is fully
  /// instantiated in case virtual method are called within init() itself.
  ctkDICOMEchoJob(ctkDICOMEchoJobPrivate* pimpl);

private:
  Q_DECLARE_PRIVATE(ctkDICOMEchoJob);
  Q_DISABLE_COPY(ctkDICOMEchoJob);
};

#endif
