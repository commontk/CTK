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

#ifndef __ctkDICOMInserter_h
#define __ctkDICOMInserter_h

// Qt includes
#include <QObject>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"

class ctkDICOMInserterPrivate;
class ctkDICOMJobResponseSet;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMInserter : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString databaseFilename READ databaseFilename WRITE setDatabaseFilename);
  Q_PROPERTY(QStringList tagsToPrecache READ tagsToPrecache WRITE setTagsToPrecache);
  Q_PROPERTY(QStringList tagsToExcludeFromStorage READ tagsToExcludeFromStorage WRITE setTagsToExcludeFromStorage);

public:
  explicit ctkDICOMInserter(QObject* parent = 0);
  virtual ~ctkDICOMInserter();

  ///@{
  /// Database Filename
  void setDatabaseFilename(const QString& databaseFilename);
  QString databaseFilename() const;
  ///@}

  ///@{
  /// Database TagsToPrecache
  void setTagsToPrecache(const QStringList& tagsToPrecache);
  QStringList tagsToPrecache() const;
  ///@}

  ///@{
  /// Database TagsToPrecache
  void setTagsToExcludeFromStorage(const QStringList& tagsToExcludeFromStorage);
  QStringList tagsToExcludeFromStorage() const;
  ///@}

  /// Return true if the operation was canceled.
  Q_INVOKABLE bool wasCanceled();

  /// add JobResponseSets from queries and retrieves
  Q_INVOKABLE bool addJobResponseSets(QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets);

Q_SIGNALS:
  void updatingDatabase(bool);
  void done();

public Q_SLOTS:
  void cancel();

protected:
  QScopedPointer<ctkDICOMInserterPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMInserter);
  Q_DISABLE_COPY(ctkDICOMInserter);
};

#endif
