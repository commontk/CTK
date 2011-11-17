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

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// STD includes
#include <iostream>
#include <cstdlib>

int ctkDICOMCoreTest1(int argc, char * argv []) {

  QCoreApplication app(argc, argv);
  QTextStream out(stdout);
  try
  {
    ctkDICOMDatabase myCTK( argv[1] );
    out << "open db success\n";
    /// make sure it is empty and properly initialized
    if (! myCTK.initializeDatabase() ) {
       out << "ERROR: basic DB init failed";
       return EXIT_FAILURE;
    };
    /// insert some sample data
    if (! myCTK.initializeDatabase(argv[2]) ) {
       out << "ERROR: sample DB init failed";
       return EXIT_FAILURE;
    };
    myCTK.closeDatabase();
    }
  catch (std::exception e)
  {
    out << "ERROR: " << e.what();
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

