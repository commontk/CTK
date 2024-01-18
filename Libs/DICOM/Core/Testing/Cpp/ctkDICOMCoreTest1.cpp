/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

// Qt includes
#include <QCoreApplication>
#include <QTextStream>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// STD includes
#include <iostream>
#include <cstdlib>

int ctkDICOMCoreTest1(int argc, char * argv []) {

  QCoreApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();

  if (arguments.count() != 2)
  {
    std::cerr << "Usage: " << qPrintable(testName)
              << " <dumpfile1.sql> <dumpfile2.sql>" << std::endl;
    return EXIT_FAILURE;
  }

  QString sqlFileName1(arguments.at(0));
  QString sqlFileName2(arguments.at(1));

  try
  {
    ctkDICOMDatabase myCTK(sqlFileName1);
    CHECK_BOOL(myCTK.initializeDatabase(), true);

    /// insert some sample data
    CHECK_BOOL(myCTK.initializeDatabase(sqlFileName2.toUtf8()), true);

    myCTK.closeDatabase();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error when opening the data base file: " << argv[1]
              << " error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
