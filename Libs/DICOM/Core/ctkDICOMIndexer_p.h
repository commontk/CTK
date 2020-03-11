/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef CTKDICOMINDEXERPRIVATE_H
#define CTKDICOMINDEXERPRIVATE_H

#include <QObject>

#include "ctkDICOMIndexer.h"
#include "ctkDICOMItem.h"

class ctkDICOMDatabase;
class ctkDataset;

class DICOMIndexingQueue
{
public:
  struct IndexingRequest
  {
    /// Either inputFolderPath or inputFilesPath is used
    QString inputFolderPath;
    QStringList inputFilesPath;
    /// If inputFolderPath is specified, includeHidden is used to decide
    /// if hidden files and folders are imported or not.
    bool includeHidden;
    /// Make a copy of the indexed file into the database.
    /// If false then only a link to the existing file is added.
    bool copyFile;
  };

  DICOMIndexingQueue()
    : IsIndexing(false)
    , StopRequested(false)
    , Mutex(QMutex::Recursive)
  {
  }

  virtual ~DICOMIndexingQueue()
  {
  }

  QString databaseFilename()
  {
    QMutexLocker locker(&this->Mutex);
    return this->DatabaseFilename;
  }

  void setDatabaseFilename(const QString& filename)
  {
    QMutexLocker locker(&this->Mutex);
    this->DatabaseFilename = filename;
  }

  QStringList tagsToPrecache()
  {
    QMutexLocker locker(&this->Mutex);
    return this->TagsToPrecache;
  }

  void setTagsToPrecache(const QStringList& tags)
  {
    QMutexLocker locker(&this->Mutex);
    this->TagsToPrecache = tags;
  }

  QStringList tagsToExcludeFromStorage()
  {
    QMutexLocker locker(&this->Mutex);
    return this->TagsToExcludeFromStorage;
  }

  void setTagsToExcludeFromStorage(const QStringList& tags)
  {
    QMutexLocker locker(&this->Mutex);
    this->TagsToExcludeFromStorage = tags;
  }

  void clear()
  {
    QMutexLocker locker(&this->Mutex);
    this->IndexingRequests.clear();
    this->IndexingResults.clear();
  }

  int popIndexingRequest(IndexingRequest& indexingRequest)
  {
    QMutexLocker locker(&this->Mutex);
    if (this->IndexingRequests.empty())
    {
      return -1;
    }
    indexingRequest = this->IndexingRequests.takeFirst();
    return this->IndexingRequests.count();
  }

  void pushIndexingRequest(const IndexingRequest& indexingRequest)
  {
    QMutexLocker locker(&this->Mutex);
    this->IndexingRequests.push_back(indexingRequest);
  }

  int indexingResultsCount()
  {
    QMutexLocker locker(&this->Mutex);
    return this->IndexingResults.size();
  }

  void popAllIndexingResults(QList<ctkDICOMDatabase::IndexingResult>& indexingResults)
  {
    QMutexLocker locker(&this->Mutex);
    indexingResults = this->IndexingResults;
    this->IndexingResults.clear();
  }

  int pushIndexingResult(const ctkDICOMDatabase::IndexingResult& indexingResult)
  {
    QMutexLocker locker(&this->Mutex);
    this->IndexingResults.push_back(indexingResult);
    return this->IndexingResults.size();
  }

  void modifiedTimeForFilepath(QMap<QString, QDateTime>& timesForPaths)
  {
    QMutexLocker locker(&this->Mutex);
    timesForPaths = this->ModifiedTimeForFilepath;
  }

  void setModifiedTimeForFilepath(const QMap<QString, QDateTime>& timesForPaths)
  {
    QMutexLocker locker(&this->Mutex);
    this->ModifiedTimeForFilepath = timesForPaths;
  }

  void setIndexing(bool indexing)
  {
    QMutexLocker locker(&this->Mutex);
    this->IsIndexing = indexing;
  }

  bool isIndexing()
  {
    QMutexLocker locker(&this->Mutex);
    return this->IsIndexing;
  }

  bool isEmpty()
  {
    QMutexLocker locker(&this->Mutex);
    return (this->IndexingRequests.isEmpty() && this->IndexingResults.isEmpty());
  }

  bool isIndexingRequestsEmpty()
  {
    QMutexLocker locker(&this->Mutex);
    return this->IndexingRequests.isEmpty();
  }

  bool isStopRequested()
  {
    return this->StopRequested;
  }

  void setStopRequested(bool stop)
  {
    this->StopRequested = stop;
  }

protected:
  // List of already indexed file paths and oldest file modified time in the database
  QMap<QString, QDateTime> ModifiedTimeForFilepath;

  QList<IndexingRequest> IndexingRequests;
  QList<ctkDICOMDatabase::IndexingResult> IndexingResults;

  QString DatabaseFilename;
  QStringList TagsToPrecache;
  QStringList TagsToExcludeFromStorage;

  bool IsIndexing;
  bool StopRequested;

  mutable QMutex Mutex;
};


class ctkDICOMIndexerPrivateWorker : public QObject
{
  Q_OBJECT

public:
  ctkDICOMIndexerPrivateWorker(DICOMIndexingQueue* queue);
  virtual ~ctkDICOMIndexerPrivateWorker();

public Q_SLOTS:
  void start();

Q_SIGNALS:
  void progress(int);
  void progressDetail(QString);
  void progressStep(QString);
  void updatingDatabase(bool);
  void indexingComplete(int, int, int, int);

private:

  void processIndexingRequest(DICOMIndexingQueue::IndexingRequest& request, ctkDICOMDatabase& database);
  void writeIndexingResultsToDatabase(ctkDICOMDatabase& database);

  DICOMIndexingQueue* RequestQueue;
  int NumberOfInstancesToInsert;
  int NumberOfInstancesInserted;

  double TimePercentageIndexing;

  int RemainingRequestCount; // the current request in progress is not included
  int CompletedRequestCount; // the current request in progress is not included

  // List of already indexed file paths and oldest file modified time in the database.
  // Cached here to avoid locking/unlocking a mutex each time a file is looked up.
  QMap<QString, QDateTime> ModifiedTimeForFilepath;
};


//------------------------------------------------------------------------------
class ctkDICOMIndexerPrivate : public QObject
{
  Q_OBJECT

  Q_DECLARE_PUBLIC(ctkDICOMIndexer);

protected:
  ctkDICOMIndexer* const q_ptr;

public:
  ctkDICOMIndexerPrivate(ctkDICOMIndexer&);
  ~ctkDICOMIndexerPrivate();

  void pushIndexingRequest(const DICOMIndexingQueue::IndexingRequest& request);

Q_SIGNALS:
  void startWorker();

//public Q_SLOTS:

public:
  DICOMIndexingQueue RequestQueue;
  QThread WorkerThread;
  ctkDICOMDatabase* Database;
  bool BackgroundImportEnabled;
};


#endif // CTKDICOMINDEXERPRIVATE_H
