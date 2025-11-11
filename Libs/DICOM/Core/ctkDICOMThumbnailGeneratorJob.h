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

#ifndef __ctkDICOMThumbnailGeneratorJob_h
#define __ctkDICOMThumbnailGeneratorJob_h

// Qt includes
#include <QColor>
#include <QObject>
#include <QSharedPointer>

// ctkCore includes
class ctkAbstractWorker;

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMJob.h"
class ctkDICOMThumbnailGeneratorJobPrivate;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMThumbnailGeneratorJob : public ctkDICOMJob
{
  Q_OBJECT
  Q_PROPERTY(QString databaseFilename READ databaseFilename WRITE setDatabaseFilename NOTIFY databaseFilenameChanged);
  Q_PROPERTY(QString dicomFilePath READ dicomFilePath WRITE setDicomFilePath NOTIFY dicomFilePathChanged);
  Q_PROPERTY(QString modality READ modality WRITE setModality NOTIFY modalityChanged);
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged);
Q_SIGNALS:
  void databaseFilenameChanged(const QString &);
  void dicomFilePathChanged(const QString &);
  void modalityChanged(const QString &);
  void backgroundColorChanged(const QColor &);
public:
  typedef ctkDICOMJob Superclass;
  explicit ctkDICOMThumbnailGeneratorJob();
  virtual ~ctkDICOMThumbnailGeneratorJob();

  ///@{
  /// Database Filename
  void setDatabaseFilename(QString databaseFilename);
  QString databaseFilename() const;
  ///}@

  ///@{
  /// Dicom file path
  void setDicomFilePath(QString dicomFilePath);
  QString dicomFilePath() const;
  ///@}

  ///@{
  /// Modality
  void setModality(QString modality);
  QString modality() const;
  ///@}

  ///@{
  /// Background Color
  void setBackgroundColor(QColor backgroundColor);
  QColor backgroundColor() const;
  ///@}

  /// Logger report string formatting for specific task
  Q_INVOKABLE QString loggerReport(const QString& status) override;

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
  QScopedPointer<ctkDICOMThumbnailGeneratorJobPrivate> d_ptr;

  /// Constructor allowing derived class to specify a specialized pimpl.
  ///
  /// \note You are responsible to call init() in the constructor of
  /// derived class. Doing so ensures the derived class is fully
  /// instantiated in case virtual method are called within init() itself.
  ctkDICOMThumbnailGeneratorJob(ctkDICOMThumbnailGeneratorJobPrivate* pimpl);

private:
  Q_DECLARE_PRIVATE(ctkDICOMThumbnailGeneratorJob);
  Q_DISABLE_COPY(ctkDICOMThumbnailGeneratorJob);
};

#endif
