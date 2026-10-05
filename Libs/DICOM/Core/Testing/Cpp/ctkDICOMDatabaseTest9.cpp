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

=========================================================================*/

// Qt includes
#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QMutex>
#include <QTemporaryDir>
#include <QThread>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// STD includes
#include <atomic>
#include <cstdlib>
#include <iostream>

namespace
{

/// Opens its own ctkDICOMDatabase on an existing database file and performs a
/// write on it, the way an inserter job running in a worker thread does.
class ctkDICOMDatabaseWriterThread : public QThread
{
public:
  ctkDICOMDatabaseWriterThread(const QString& databaseFilePath)
    : DatabaseFilePath(databaseFilePath)
    , Started(false)
    , Completed(false)
  {
  }

  void run() override
  {
    ctkDICOMDatabase database;
    // A connection of its own, as ctkDICOMInserter does for each worker thread
    database.openDatabase(this->DatabaseFilePath, "db_writer_thread");

    this->Started = true;

    // A write entry point: it takes ctkDICOMDatabase::writeMutex() around the
    // statements that delete the rows, so it cannot complete while the mutex is
    // held elsewhere. Removing a series that does not exist still issues them.
    database.removeSeries("1.2.3.4.5.6.7.8.9.does.not.exist");

    this->Completed = true;
    database.closeDatabase();
  }

  QString DatabaseFilePath;
  std::atomic<bool> Started;
  std::atomic<bool> Completed;
};

} // end of anonymous namespace

// Checks that writes to a DICOM database are serialized by the static write mutex,
// across different ctkDICOMDatabase instances opened on the same file.
int ctkDICOMDatabaseTest9( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  //
  // The mutex is static, so every instance reports the very same one
  //
  {
    ctkDICOMDatabase firstDatabase;
    ctkDICOMDatabase secondDatabase;
    if (&firstDatabase.writeMutex() != &secondDatabase.writeMutex() ||
        &firstDatabase.writeMutex() != &ctkDICOMDatabase::writeMutex())
    {
      std::cerr << "Line " << __LINE__ << " - ctkDICOMDatabase::writeMutex() is not shared "
                << "between instances: writes of different instances would not be serialized."
                << std::endl;
      return EXIT_FAILURE;
    }
  }

  //
  // The mutex is recursive, since the write entry points call one another
  //
  {
    QMutexLocker outerLocker(&ctkDICOMDatabase::writeMutex());
    QMutexLocker innerLocker(&ctkDICOMDatabase::writeMutex());
    // Reaching this line without deadlocking is the check itself
  }

  QTemporaryDir tempDirectory;
  CHECK_BOOL(tempDirectory.isValid(), true);

  QDir databaseDirectory(tempDirectory.path());
  QFileInfo databaseFile(databaseDirectory, QString("database.test"));
  QString databaseFilePath = databaseFile.absoluteFilePath();

  ctkDICOMDatabase database;
  database.openDatabase(databaseFilePath);
  if (!database.lastError().isEmpty())
  {
    std::cerr << "Line " << __LINE__ << " - ctkDICOMDatabase::openDatabase() failed: "
              << qPrintable(database.lastError()) << std::endl;
    return EXIT_FAILURE;
  }

  //
  // A write issued by another ctkDICOMDatabase on the same file has to wait while
  // the mutex is held
  //
  ctkDICOMDatabaseWriterThread writerThread(databaseFilePath);

  {
    QMutexLocker locker(&ctkDICOMDatabase::writeMutex());

    writerThread.start();

    // Let the thread reach the write. Waiting for Started only tells us that the
    // database was opened, so give it some time to get to the write itself.
    QElapsedTimer timer;
    timer.start();
    while (!writerThread.Started && timer.elapsed() < 5000)
    {
      QThread::msleep(10);
    }

    if (!writerThread.Started)
    {
      std::cerr << "Line " << __LINE__ << " - the writer thread did not start" << std::endl;
      writerThread.wait();
      return EXIT_FAILURE;
    }

    QThread::msleep(500);

    if (writerThread.Completed)
    {
      std::cerr << "Line " << __LINE__ << " - a write of another ctkDICOMDatabase instance "
                << "completed while the write mutex was held: writes are not serialized."
                << std::endl;
      writerThread.wait();
      return EXIT_FAILURE;
    }
  }

  //
  // Once released, the pending write goes through
  //
  if (!writerThread.wait(10000))
  {
    std::cerr << "Line " << __LINE__ << " - the write did not complete after the write mutex "
              << "was released" << std::endl;
    writerThread.terminate();
    writerThread.wait();
    return EXIT_FAILURE;
  }

  if (!writerThread.Completed)
  {
    std::cerr << "Line " << __LINE__ << " - the writer thread finished without completing "
              << "its write" << std::endl;
    return EXIT_FAILURE;
  }

  database.closeDatabase();

  std::cerr << "ctkDICOMDatabaseTest9 passed" << std::endl;
  return EXIT_SUCCESS;
}
