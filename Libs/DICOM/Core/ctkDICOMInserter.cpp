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

// Qt includes
#include <QDebug>

// ctkDICOMCore includes
#include "ctkLogger.h"
#include "ctkDICOMDatabase.h"
#include "ctkDICOMInserter.h"
#include "ctkDICOMJobResponseSet.h"

static ctkLogger logger ("org.commontk.dicom.DICOMInserter");

//------------------------------------------------------------------------------
class ctkDICOMInserterPrivate
{
public:
  ctkDICOMInserterPrivate();
  ~ctkDICOMInserterPrivate() = default;

  bool Canceled;
  QString DatabaseFilename;
  QStringList TagsToPrecache;
  QStringList TagsToExcludeFromStorage;
};

//------------------------------------------------------------------------------
// ctkDICOMInserterPrivate methods

//------------------------------------------------------------------------------
ctkDICOMInserterPrivate::ctkDICOMInserterPrivate()
{
  this->Canceled = false;
}

//------------------------------------------------------------------------------
// ctkDICOMInserter methods

//------------------------------------------------------------------------------
ctkDICOMInserter::ctkDICOMInserter(QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkDICOMInserterPrivate)
{
}

//------------------------------------------------------------------------------
ctkDICOMInserter::~ctkDICOMInserter() = default;

//------------------------------------------------------------------------------
CTK_SET_CPP(ctkDICOMInserter, const QString&, setDatabaseFilename, DatabaseFilename);
CTK_GET_CPP(ctkDICOMInserter, QString, databaseFilename, DatabaseFilename)
CTK_SET_CPP(ctkDICOMInserter, const QStringList&, setTagsToPrecache, TagsToPrecache);
CTK_GET_CPP(ctkDICOMInserter, QStringList, tagsToPrecache, TagsToPrecache)
CTK_SET_CPP(ctkDICOMInserter, const QStringList&, setTagsToExcludeFromStorage, TagsToExcludeFromStorage);
CTK_GET_CPP(ctkDICOMInserter, QStringList, tagsToExcludeFromStorage, TagsToExcludeFromStorage)

//------------------------------------------------------------------------------
bool ctkDICOMInserter::wasCanceled()
{
  Q_D(const ctkDICOMInserter);
  return d->Canceled;
}

//------------------------------------------------------------------------------
bool ctkDICOMInserter::addJobResponseSets(QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets)
{
  Q_D(const ctkDICOMInserter);
  if (d->Canceled)
  {
    return false;
  }

  emit updatingDatabase(true);

  ctkDICOMDatabase database;
  QString dbConnectionName =
    "db_" + QString::number(reinterpret_cast<quint64>(QThread::currentThreadId()), 16);

  database.openDatabase(d->DatabaseFilename, dbConnectionName);
  database.setTagsToPrecache(d->TagsToPrecache);
  database.setTagsToExcludeFromStorage(d->TagsToExcludeFromStorage);

  // To Do: We should ensure that only one write operation occurs at a time.
  // In the ctkDICOMScheduler, we ensure this by utilizing a job queue, preventing multiple inserter jobs from running concurrently.
  // Similarly, it would be necessary to implement a check in the insert method of ctkDICOMDatabase
  // to determine if any other process is currently writing (for example, a UI element writing the patient's name into the database).
  // Therefore, we propose the inclusion of a static variable in ctkDICOMDatabase that indicates ongoing write operations
  // for each DatabaseFilename, except in cases where it is an in-memory database.
  database.insert(jobResponseSets);
  database.updateDisplayedFields();

  database.closeDatabase();

  emit updatingDatabase(false);
  emit done();

  return true;
}

//----------------------------------------------------------------------------
void ctkDICOMInserter::cancel()
{
  Q_D(ctkDICOMInserter);
  d->Canceled = true;
}
