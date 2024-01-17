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

#ifndef __ctkDICOMInserterJob_h
#define __ctkDICOMInserterJob_h

// Qt includes
#include <QObject>
#include <QSharedPointer>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"

#include <ctkDICOMJob.h>

class ctkDICOMServer;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMInserterJob : public ctkDICOMJob
{
  Q_OBJECT
  Q_PROPERTY(QString databaseFilename READ databaseFilename WRITE setDatabaseFilename);
  Q_PROPERTY(QStringList tagsToPrecache READ tagsToPrecache WRITE setTagsToPrecache);
  Q_PROPERTY(QStringList tagsToExcludeFromStorage READ tagsToExcludeFromStorage WRITE setTagsToExcludeFromStorage);

public:
  typedef ctkDICOMJob Superclass;
  explicit ctkDICOMInserterJob();
  virtual ~ctkDICOMInserterJob();

  /// Logger report string formatting for specific task
  Q_INVOKABLE QString loggerReport(const QString& status) const override;

  ///@{
  /// Database Filename
  void setDatabaseFilename(const QString& databaseFilename);
  QString databaseFilename() const;
  ///}@

  ///@{
  /// Database TagsToPrecache
  void setTagsToPrecache(const QStringList& tagsToPrecache);
  QStringList tagsToPrecache() const;
  ///}@

  ///@{
  /// Database TagsToPrecache
  void setTagsToExcludeFromStorage(const QStringList& tagsToExcludeFromStorage);
  QStringList tagsToExcludeFromStorage() const;
  ///}@

  /// Create a copy of this object
  Q_INVOKABLE ctkAbstractJob* generateCopy() const override;

  /// Generate worker for job
  Q_INVOKABLE ctkDICOMWorker* createWorker() override;

protected:
  QString DatabaseFilename;
  QStringList TagsToPrecache;
  QStringList TagsToExcludeFromStorage;

private:
  Q_DISABLE_COPY(ctkDICOMInserterJob);
};

#endif
