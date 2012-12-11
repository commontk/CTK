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


int ctkDICOMDatabaseTest4( int argc, char * argv [] )
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

  //
  // Basic test:
  // - insert the file specified on the command line
  // - ask for tag values and compare to known results
  //
  QString instanceUID("1.2.840.113619.2.135.3596.6358736.4843.1115808177.83");
  QString tag("0008,103e");
  QString badTag("9999,9999");

  //
  // Test the precache feature of the database
  //

  if (database.cachedTag(instanceUID, tag) != QString(""))
    {
    std::cerr << "ctkDICOMDatabase: tag cache should return empty string for unknown instance tag" << std::endl;
    return EXIT_FAILURE;
    }

  if (database.cachedTag(instanceUID, badTag) != QString(""))
    {
    std::cerr << "ctkDICOMDatabase: bad tag cache should return empty string for unknown instance tag" << std::endl;
    return EXIT_FAILURE;
    }

  QStringList tagsToPrecache;
  tagsToPrecache << tag;
  database.setTagsToPrecache(tagsToPrecache);

  if (database.tagsToPrecache() != tagsToPrecache)
    {
    std::cerr << "ctkDICOMDatabase: tags to precache not correct" << std::endl;
    return EXIT_FAILURE;
    }

  // check the insert timestamp
  QDateTime beforeInsert = QDateTime::currentDateTime();
  std::cerr << "Current dateTime " << beforeInsert.toString().toStdString() << std::endl;

  database.insert(dicomFilePath, false, false);

  QDateTime insertTimeStamp = database.insertDateTimeForInstance(instanceUID);
  std::cerr << "Instance inserted " << insertTimeStamp.toString().toStdString() << std::endl;

  QString filePath = database.fileForInstance(instanceUID);
  std::cerr << "Instance file " << filePath.toStdString() << std::endl;

  int elapsed = beforeInsert.secsTo(insertTimeStamp);
  if (elapsed > 1)
    {
    std::cerr << "ctkDICOMDatabase: Took more than a second to insert the file." << std::endl;
    return EXIT_FAILURE;
    }

  // check for series description in tag cache
  QString knownSeriesDescription("3D Cor T1 FAST IR-prepped GRE");

  QString cachedTag = database.cachedTag(instanceUID, tag);

  if (cachedTag != knownSeriesDescription)
    {
    std::cerr << "ctkDICOMDatabase: tag cache should return known value for instance" << std::endl;
    return EXIT_FAILURE;
    }

  if (database.instanceValue(instanceUID, tag) != knownSeriesDescription)
    {
    std::cerr << "ctkDICOMDatabase: database should return known value for instance" << std::endl;
    return EXIT_FAILURE;
    }

  if (database.instanceValue(instanceUID, badTag) != QString(""))
    {
    std::cerr << "ctkDICOMDatabase: bad tag should have empty value" << std::endl;
    return EXIT_FAILURE;
    }

  if (database.cachedTag(instanceUID, badTag) != QString("__TAG_NOT_IN_INSTANCE__"))
    {
    std::cerr << "ctkDICOMDatabase: bad tag should have sentinal value in cache" << std::endl;
    return EXIT_FAILURE;
    }

  database.closeDatabase();

  std::cerr << "Database is in " << databaseDirectory.path().toStdString() << std::endl;

  return EXIT_SUCCESS;
}
