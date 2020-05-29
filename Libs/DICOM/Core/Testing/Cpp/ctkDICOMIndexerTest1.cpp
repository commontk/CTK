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
#include "ctkDICOMIndexer.h"

// STD includes
#include <iostream>

int ctkDICOMIndexerTest1( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  // Get data directory from environment
  QDir dataDir = QDir(QProcessEnvironment::systemEnvironment().value("CTKData_DIR", ""));
  QString dicomDir = dataDir.filePath("Data/DICOM");
  if (!QDir(dicomDir).exists())
    {
    std::cerr << "Directory does not exist: " << qPrintable(dicomDir) << std::endl;
    std::cerr << "Make sure CTKData_DIR environment variable is set correctly" << std::endl;
    }

  ctkDICOMDatabase database;
  database.openDatabase(":memory:");
  ctkDICOMIndexer indexer;

  // Test ctkDICOMIndexer::addDirectory()
  // just check if it doesn't crash
  // Create block to test batch indexing using indexingBatch helper class.
  {
    indexer.addDirectory(&database, dicomDir, false);
    indexer.addDirectory(&database, dicomDir, true);
  }

  // ensure all concurrent inserts are complete
  indexer.waitForImportFinished();

  return EXIT_SUCCESS;
}
