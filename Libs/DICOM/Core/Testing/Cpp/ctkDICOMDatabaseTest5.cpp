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


int ctkDICOMDatabaseTest5( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  if (argc < 2)
    {
    std::cerr << "ctkDICOMDatabaseTest2: missing dicom filePath argument";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  QString dicomFilePath(argv[1]);

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


  //
  // First, copy the passed file to two temp directories
  //
  databaseDirectory.rmdir("firstInsert");
  databaseDirectory.rmdir("secondInsert");
  databaseDirectory.mkdir("firstInsert");
  databaseDirectory.mkdir("secondInsert");
  QString firstDestination = QDir::cleanPath(databaseDirectory.absolutePath() + QDir::separator() + "firstInsert" + QDir::separator() + "file.dcm");
  QString secondDestination = QDir::cleanPath(databaseDirectory.absolutePath() + QDir::separator() + "secondInsert" + QDir::separator() + "file.dcm");
  QFile::copy(dicomFilePath, firstDestination);
  std::cerr << "copied to: " << firstDestination.toStdString() << std::endl;
  QFile::copy(dicomFilePath, secondDestination);
  std::cerr << "copied to: " << secondDestination.toStdString() << std::endl;

  //
  // Insert the first instance
  //
  database.insert(firstDestination, false, false);

  //
  // Now delete the first instance and try inserting the second
  //
  QFile::remove(firstDestination);
  database.insert(secondDestination, false, false);

  //
  // At this point, the file associated with the instance UID should be
  // the secondDestination - it's an error if not.
  //
  QString instanceUID("1.2.840.113619.2.135.3596.6358736.4843.1115808177.83");
  QString filePathInDatabase = database.fileForInstance(instanceUID);

  if (filePathInDatabase != secondDestination)
    {
    std::cerr << "ctkDICOMDatabase thinks instance is in " << filePathInDatabase.toStdString() << std::endl;
    std::cerr << "But we just inserted it from " << secondDestination.toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Close and clean up
  //
  database.closeDatabase();


  return EXIT_SUCCESS;
}
