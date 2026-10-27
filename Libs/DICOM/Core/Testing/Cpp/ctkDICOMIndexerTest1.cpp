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

  // The indexer worker thread opens the database by file name, therefore an in-memory database cannot be used
  QTemporaryDir tempDirectory;
  if (!tempDirectory.isValid())
  {
    std::cerr << "Failed to create temporary directory" << std::endl;
    return EXIT_FAILURE;
  }
  ctkDICOMDatabase database;
  database.openDatabase(QDir(tempDirectory.path()).filePath("database.test"));
  ctkDICOMIndexer indexer;

  // Lower-case tags must be found in the tag cache after indexing. Pixel data is excluded from storage
  // (only its presence is cached) and the private tag is not in the test files.
  const QString seriesDescriptionTag("0008,103e");
  const QString pixelDataTag("7fe0,0010");
  const QString missingPrivateTag("0019,100c");
  database.setTagsToPrecache(QStringList() << seriesDescriptionTag << pixelDataTag << missingPrivateTag);

  // Test ctkDICOMIndexer::addDirectory()
  // just check if it doesn't crash
  // Create block to test batch indexing using indexingBatch helper class.
  {
    indexer.addDirectory(&database, dicomDir, false);
    indexer.addDirectory(&database, dicomDir, true);
  }

  // ensure all concurrent inserts are complete
  indexer.waitForImportFinished();

  // Check that the precached tags are in the tag cache of the indexed instances
  QStringList filePaths;
  foreach (const QString& patient, database.patients())
  {
    foreach (const QString& study, database.studiesForPatient(patient))
    {
      foreach (const QString& series, database.seriesForStudy(study))
      {
        filePaths << database.filesForSeries(series);
      }
    }
  }
  if (filePaths.isEmpty())
  {
    std::cerr << "No files were indexed from " << qPrintable(dicomDir) << std::endl;
    return EXIT_FAILURE;
  }
  foreach (const QString& filePath, filePaths)
  {
    QString sopInstanceUID = database.instanceForFile(filePath);
    foreach (const QString& tag, QStringList() << seriesDescriptionTag << pixelDataTag << missingPrivateTag)
    {
      // cachedTag returns an empty string if the tag is not in the cache
      if (database.cachedTag(sopInstanceUID, tag).isEmpty())
      {
        std::cerr << "Precached tag " << qPrintable(tag) << " is not in the tag cache of " << qPrintable(filePath) << std::endl;
        return EXIT_FAILURE;
      }
    }
    if (!database.fileValueExists(filePath, pixelDataTag))
    {
      std::cerr << "Pixel data presence is not detected in " << qPrintable(filePath) << std::endl;
      return EXIT_FAILURE;
    }
    if (database.fileValueExists(filePath, missingPrivateTag))
    {
      std::cerr << "Missing private tag is detected as present in " << qPrintable(filePath) << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
