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
#include <QTemporaryDir>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// STD includes
#include <iostream>
#include <cstdlib>


int ctkDICOMDatabaseTest2( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();

  if (arguments.count() != 1)
    {
    std::cerr << "Usage: " << qPrintable(testName)
              << " <path-to-dicom-file>" << std::endl;
    return EXIT_FAILURE;
    }

  QString dicomFilePath(arguments.at(0));

  QTemporaryDir tempDirectory;
  CHECK_BOOL(tempDirectory.isValid(), true);

  ctkDICOMDatabase database;
  QDir databaseDirectory(tempDirectory.path());
  databaseDirectory.remove("ctkDICOMDatabase.sql");
  databaseDirectory.remove("ctkDICOMTagCache.sql");

  QFileInfo databaseFile(databaseDirectory, QString("database.test"));
  database.openDatabase(databaseFile.absoluteFilePath());

  if (!database.lastError().isEmpty())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() failed: "
              << qPrintable(database.lastError()) << std::endl;
    return EXIT_FAILURE;
    }

  if (!database.database().isValid())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() failed: "
              << "invalid sql database" << std::endl;
    return EXIT_FAILURE;
    }

  if (database.isInMemory())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() failed: "
              << "database should not be in memory" << std::endl;
    return EXIT_FAILURE;
    }

  bool res = database.initializeDatabase();

  if (!res)
    {
    std::cerr << "ctkDICOMDatabase::initializeDatabase() failed." << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Test that the tag interface works to parse ascii
  //
  {
    unsigned short group, element;
    QString tag("0008,103E"); // upper case
    CHECK_BOOL(database.tagToGroupElement(tag, group, element), true);
    CHECK_INT(group, 0x8);
    CHECK_INT(element, 0x103E);
  }
  {
    unsigned short group, element;
    QString tag("0008,103e"); // lower case
    CHECK_BOOL(database.tagToGroupElement(tag, group, element), true);
    CHECK_INT(group, 0x8);
    CHECK_INT(element, 0x103E);
  }

  //
  // Test that conversion from uints to tag string works
  //
  CHECK_QSTRING(database.groupElementToTag(0x8, 0x103E), "0008,103E");

  //
  // Basic test:
  // - insert the file specified on the command line
  // - ask for tag values and compare to known results
  //
  database.insert(dicomFilePath, false, false);
  QString instanceUID("1.2.840.113619.2.135.3596.6358736.4843.1115808177.83");

  QString foundFile = database.fileForInstance(instanceUID);

  if (foundFile != dicomFilePath)
    {
    std::cerr << "ctkDICOMDatabase: didn't get back the original file path" << std::endl;
    return EXIT_FAILURE;
    }

  QString foundInstance = database.instanceForFile(dicomFilePath);

  if (foundInstance != instanceUID)
    {
    std::cerr << "ctkDICOMDatabase: didn't get back the original instance uid" << std::endl;
    return EXIT_FAILURE;
    }


  //
  // Test the tag cache
  //

  if (!database.tagCacheExists())
    {
    std::cerr << "ctkDICOMDatabase: tag cache should be configured when database opens" << std::endl;
    return EXIT_FAILURE;
    }

  if (!database.initializeTagCache())
    {
    std::cerr << "ctkDICOMDatabase: could not initialize tag cache" << std::endl;
    return EXIT_FAILURE;
    }

  if (!database.tagCacheExists())
    {
    std::cerr << "ctkDICOMDatabase: tag cache should exist but is not detected" << std::endl;
    return EXIT_FAILURE;
    }

  QString tag("0008,103E");
  if (database.cachedTag(instanceUID, tag) != QString(""))
    {
    std::cerr << "ctkDICOMDatabase: tag cache should return empty string for unknown instance tag" << std::endl;
    return EXIT_FAILURE;
    }

  QString knownSeriesDescription("3D Cor T1 FAST IR-prepped GRE");

  if (!database.cacheTag(instanceUID, tag, knownSeriesDescription))
    {
    std::cerr << "ctkDICOMDatabase: could not insert instance tag" << std::endl;
    return EXIT_FAILURE;
    }

  if (database.cachedTag(instanceUID, tag) != knownSeriesDescription)
    {
    std::cerr << "ctkDICOMDatabase: could not retrieve cached tag" << std::endl;
    return EXIT_FAILURE;
    }


  QString foundSeriesDescription = database.instanceValue(instanceUID, tag);

  if (foundSeriesDescription != knownSeriesDescription)
    {
    std::cerr << "ctkDICOMDatabase: invalid element value returned" << std::endl;
    return EXIT_FAILURE;
    }

  // now update the database
  database.updateSchema();

  // and repeat the above checks
  foundFile = database.fileForInstance(instanceUID);

  if (foundFile != dicomFilePath)
    {
    std::cerr << "ctkDICOMDatabase: didn't get back the original file path" << std::endl;
    return EXIT_FAILURE;
    }

  foundSeriesDescription = database.instanceValue(instanceUID, tag);

  if (foundSeriesDescription != knownSeriesDescription)
    {
    std::cerr << "ctkDICOMDatabase: invalid element value returned" << std::endl;
    return EXIT_FAILURE;
    }

  database.closeDatabase();
  database.initializeDatabase();

  return EXIT_SUCCESS;
}
