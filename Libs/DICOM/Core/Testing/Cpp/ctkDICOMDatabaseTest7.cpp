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

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// STD includes
#include <iostream>
#include <cstdlib>


int ctkDICOMDatabaseTest7( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  ctkDICOMDatabase database;
  QDir databaseDirectory = QDir::temp();
  databaseDirectory.remove("ctkDICOMDatabase.sql");
  databaseDirectory.remove("ctkDICOMTagCache.sql");

  QFileInfo databaseFile(databaseDirectory, QString("database.test"));
  database.openDatabase(databaseFile.absoluteFilePath());

  bool res = database.initializeDatabase();

  if (!res)
    {
    std::cerr << "ctkDICOMDatabase::initializeDatabase() failed." << std::endl;
    return EXIT_FAILURE;
    }
  std::cerr << "Database is in " << databaseDirectory.path().toStdString() << std::endl;

  // try to call fileValue with bogus data: invalid filename and arbitrary tag
  // - should not trigger and exception
  // (see https://github.com/commontk/CTK/issues/706)
  // - result should be empty string
  // (see https://github.com/commontk/CTK/issues/749)

  QString result = database.fileValue("/tmp/file-that-does-not-exist", "00ff,eeee");
  if (result != "")
    {
    std::cerr << "ctkDICOMDatabase::fileValue() failed for file that doesn't exist." << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Close and clean up
  //
  database.closeDatabase();

  return EXIT_SUCCESS;
}
