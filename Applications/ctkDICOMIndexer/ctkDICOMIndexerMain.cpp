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
#include <QTextStream>

// CTK includes
#include <ctkDICOMIndexer.h>
#include <ctkDICOMDatabase.h>

// STD includes
#include <cstdlib>
#include <iostream>
#include <fstream>

void print_usage()
{
  std::cerr << "Usage:\n";
  std::cerr << "  1. ctkDICOMIndexer --add <database.db> <sourceDir> [destDir]\n";
  std::cerr << "     Adds (or refreshes) sourceDir to the index of the database.\n";
  std::cerr << "     Creates the database if it is not valid..\n";
  std::cerr << "     If destDir is provided, images are copied there after import.\n";
  std::cerr << "  2. ctkDICOMIndexer --init <database.db> [sqlScript]\n";
  std::cerr << "     Reinitialize the database. Uses default schema or the provided sqlScript file.\n";
  std::cerr << "  3. ctkDICOMIndexer --cleanup <database.db>\n";
  std::cerr << "     Remove non-existent files from the database.\n";
  return;
}


/**
  *
*/
int main(int argc, char** argv)
{

  if (argc < 3)
  {
    print_usage();
    return EXIT_FAILURE;
  }

  QCoreApplication app(argc, argv);
  QTextStream out(stdout);

  ctkDICOMDatabase myCTK;
  ctkDICOMIndexer idx;
  idx.setDatabase(&myCTK);

  try
  {
    if (std::string("--add") == argv[1])
    {
      {
        myCTK.openDatabase( argv[2] );
        if (argc > 4)
        {
          idx.addDirectory(argv[3],argv[4]);
        }
        else
        {
          idx.addDirectory(argv[3]);
        }
      }
    }
    else if (std::string("--init") == argv[1])
    {
      myCTK.openDatabase( argv[2] );
      if (argc > 2)
      {
        myCTK.initializeDatabase(argv[2]);
      }
      else
      {
        myCTK.initializeDatabase();
      }
    }
    else if (std::string("--cleanup") == argv[1])
    {
      // TODO
    }
    else
    {
      print_usage();
      return EXIT_FAILURE;
    }
  }
  catch (std::exception e)
  {
    std::cerr << "Database error:" << qPrintable(myCTK.lastError());
    myCTK.closeDatabase();
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
