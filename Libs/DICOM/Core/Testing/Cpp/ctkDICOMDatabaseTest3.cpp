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

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// STD includes
#include <iostream>
#include <cstdlib>


int ctkDICOMDatabaseTest3( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();

  if (arguments.count() != 1)
  {
    std::cerr << "Warning, no sql file given. Test stops" << std::endl;
    std::cerr << "Usage: " << qPrintable(testName)
              << " <dumpfile.sql>" << std::endl;
    return EXIT_FAILURE;
  }

  QString sqlFileName(arguments.at(0));

  QTemporaryDir tempDirectory;
  CHECK_BOOL(tempDirectory.isValid(), true);

  QDir databaseDirectory(tempDirectory.path());
  QFileInfo databaseFile(databaseDirectory, QString("database.test"));
  QString databaseFileName(databaseFile.absoluteFilePath());

  std::cerr << "Populating database " << databaseFileName.toStdString() << "\n";

  // first, create a database and initialize it with the old schema
  try
  {
    ctkDICOMDatabase myCTK( databaseFileName );

    if (!myCTK.initializeDatabase(sqlFileName.toUtf8()))
    {
      std::cerr << "Error when initializing the data base with: " << qPrintable(sqlFileName)
          << " error: " << myCTK.lastError().toStdString();
      return EXIT_FAILURE;
    }

    if ( myCTK.schemaVersionLoaded() != QString("") )
    {
      std::cerr << "Schema tag should be empty in old schema\n";
      std::cerr << "Instead we got: (" << myCTK.schemaVersionLoaded().toStdString() << ")\n";
      return EXIT_FAILURE;
    }

    myCTK.closeDatabase();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error when opening the data base file: " << databaseFileName.toStdString()
        << " error: " << e.what();
    return EXIT_FAILURE;
  }

  // now try opening it and updating the schema
  try
  {
    ctkDICOMDatabase myCTK( databaseFileName );

    if ( myCTK.schemaVersionLoaded() == myCTK.schemaVersion() )
    {
      std::cerr << "Schema version should Not match\n";
      return EXIT_FAILURE;
    }

    if ( !myCTK.updateSchema() )
    {
      std::cerr << "Could not update schema\n";
      return EXIT_FAILURE;
    }

    if ( myCTK.schemaVersionLoaded() != myCTK.schemaVersion() )
    {
      std::cerr << "Schema version should match\n";
      return EXIT_FAILURE;
    }

    myCTK.closeDatabase();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error when re-opening the data base file: " << databaseFileName.toStdString()
        << " error: " << e.what();
    return EXIT_FAILURE;
  }


  return EXIT_SUCCESS;
}
