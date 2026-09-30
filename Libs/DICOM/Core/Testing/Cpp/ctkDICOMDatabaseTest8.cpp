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
#include "ctkDICOMItem.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdeftag.h>

// STD includes
#include <iostream>
#include <cstdlib>

namespace
{

//----------------------------------------------------------------------------
// Check that fileValues() returns the same values as calling fileValue() for each file
bool checkFileValues(ctkDICOMDatabase& database, const QStringList& fileNames, const QString& tag)
{
  QStringList values = database.fileValues(fileNames, tag);
  if (values.size() != fileNames.size())
  {
    std::cerr << "Line " << __LINE__ << " - fileValues() returned " << values.size()
              << " values for " << fileNames.size() << " files" << std::endl;
    return false;
  }
  for (int i = 0; i < fileNames.size(); ++i)
  {
    QString expectedValue = database.fileValue(fileNames[i], tag);
    if (values[i] != expectedValue)
    {
      std::cerr << "Line " << __LINE__ << " - fileValues() mismatch for tag " << qPrintable(tag)
                << " in file '" << qPrintable(fileNames[i]) << "': got '" << qPrintable(values[i])
                << "', expected '" << qPrintable(expectedValue) << "'" << std::endl;
      return false;
    }
  }
  return true;
}

} // namespace

//----------------------------------------------------------------------------
int ctkDICOMDatabaseTest8(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();

  if (arguments.count() < 2)
  {
    std::cerr << "Usage: " << qPrintable(testName)
              << " <path-to-dicom-file> <path-to-dicom-file> [...]" << std::endl;
    return EXIT_FAILURE;
  }
  QStringList dicomFilePaths = arguments;

  QTemporaryDir tempDirectory;
  CHECK_BOOL(tempDirectory.isValid(), true);

  ctkDICOMDatabase database;
  QDir databaseDirectory(tempDirectory.path());
  QFileInfo databaseFile(databaseDirectory, QString("database.test"));
  database.openDatabase(databaseFile.absoluteFilePath());
  CHECK_BOOL(database.initializeDatabase(), true);

  // Modality and series description are stored in the tag cache when files are inserted,
  // image position is not (it is read from the file when requested).
  const QString modalityTag = "0008,0060";
  const QString seriesDescriptionTag = "0008,103e"; // lowercase to test case insensitivity
  const QString imagePositionTag = "0020,0032";
  const QString missingTag = "00ff,eeee";
  database.setTagsToPrecache(QStringList() << modalityTag << seriesDescriptionTag.toUpper());

  for (const QString& dicomFilePath : dicomFilePaths)
  {
    database.insert(dicomFilePath, /*storeFile=*/false, /*generateThumbnail=*/false);
  }
  CHECK_INT(database.imagesCount(), dicomFilePaths.size());

  // Empty inputs
  CHECK_INT(database.fileValues(QStringList(), modalityTag).size(), 0);
  CHECK_INT(database.fileValues(dicomFilePaths, QString()).size(), dicomFilePaths.size());

  // Mix of files in the database, a file that does not exist, an empty file name, and a duplicate
  QStringList fileNames = dicomFilePaths;
  fileNames << "/tmp/file-that-does-not-exist" << QString() << dicomFilePaths[0];

  CHECK_BOOL(checkFileValues(database, fileNames, modalityTag), true);
  CHECK_BOOL(checkFileValues(database, fileNames, seriesDescriptionTag), true);
  CHECK_BOOL(checkFileValues(database, fileNames, imagePositionTag), true);
  CHECK_BOOL(checkFileValues(database, fileNames, missingTag), true);

  QStringList modalities = database.fileValues(dicomFilePaths, modalityTag);
  CHECK_QSTRING(modalities[0], QString("MR"));

  // Values that were read from files are now in the tag cache, retrieve them again
  CHECK_BOOL(checkFileValues(database, fileNames, imagePositionTag), true);

  // Test retrieval of values for more files than the batch size used in database queries (500).
  // Create copies of a file with different SOP instance UID and instance number.
  // Copies are created outside the database folder, because files that are inserted from within the
  // database folder without copying are not found by instanceForFile().
  QTemporaryDir copiesDirectory;
  CHECK_BOOL(copiesDirectory.isValid(), true);
  const int numberOfCopies = 520;
  const QString instanceNumberTag = "0020,0013";
  ctkDICOMItem item;
  item.InitializeFromFile(dicomFilePaths[0]);
  QStringList copiedFilePaths;
  for (int copyIndex = 0; copyIndex < numberOfCopies; ++copyIndex)
  {
    CHECK_BOOL(item.SetElementAsString(DCM_SOPInstanceUID, QString("2.25.1234567890123456789%1").arg(copyIndex)), true);
    CHECK_BOOL(item.SetElementAsString(DCM_InstanceNumber, QString::number(1000 + copyIndex)), true);
    QString copiedFilePath = QDir(copiesDirectory.path()).filePath(QString("copy%1.dcm").arg(copyIndex));
    CHECK_BOOL(item.SaveToFile(copiedFilePath), true);
    database.insert(copiedFilePath, /*storeFile=*/false, /*generateThumbnail=*/false);
    copiedFilePaths << copiedFilePath;
  }
  CHECK_INT(database.imagesCount(), dicomFilePaths.size() + numberOfCopies);

  QStringList allFilePaths = dicomFilePaths + copiedFilePaths;
  CHECK_BOOL(checkFileValues(database, allFilePaths, modalityTag), true);
  // Instance number is not in the tag cache (read from file), then it is (retrieved from the tag cache)
  CHECK_BOOL(checkFileValues(database, allFilePaths, instanceNumberTag), true);
  CHECK_BOOL(checkFileValues(database, allFilePaths, instanceNumberTag), true);
  QStringList copiedInstanceNumbers = database.fileValues(copiedFilePaths, instanceNumberTag);
  for (int copyIndex = 0; copyIndex < numberOfCopies; ++copyIndex)
  {
    CHECK_QSTRING(copiedInstanceNumbers[copyIndex], QString::number(1000 + copyIndex));
  }

  // instanceValues() returns values in the same order as the instance UIDs (for more instances than the batch size).
  // Empty string is returned for unknown or empty instance UIDs. Duplicate instance UIDs are allowed.
  QStringList copiedInstanceUIDs;
  for (const QString& copiedFilePath : copiedFilePaths)
  {
    copiedInstanceUIDs << database.instanceForFile(copiedFilePath);
  }
  QStringList instanceUIDs = copiedInstanceUIDs;
  instanceUIDs << "1.2.3.4.5.6.7.8.9.1234567890" << QString() << copiedInstanceUIDs[1];
  QStringList instanceNumbers = database.instanceValues(instanceUIDs, instanceNumberTag);
  CHECK_INT(instanceNumbers.size(), instanceUIDs.size());
  for (int copyIndex = 0; copyIndex < numberOfCopies; ++copyIndex)
  {
    CHECK_QSTRING(instanceNumbers[copyIndex], QString::number(1000 + copyIndex));
  }
  CHECK_QSTRING(instanceNumbers[numberOfCopies], QString());
  CHECK_QSTRING(instanceNumbers[numberOfCopies + 1], QString());
  CHECK_QSTRING(instanceNumbers[numberOfCopies + 2], QString::number(1001));
  // Values are the same as returned by instanceValue(), including values that are stored in the tag cache
  // as not present in the instance
  QString firstInstanceUID = database.instanceForFile(dicomFilePaths[0]);
  QStringList firstInstanceValues = database.instanceValues(QStringList() << firstInstanceUID << firstInstanceUID, modalityTag);
  CHECK_INT(firstInstanceValues.size(), 2);
  CHECK_QSTRING(firstInstanceValues[0], database.instanceValue(firstInstanceUID, modalityTag));
  CHECK_QSTRING(firstInstanceValues[1], QString("MR"));
  CHECK_QSTRING(database.instanceValues(QStringList() << firstInstanceUID, missingTag)[0], QString());
  CHECK_QSTRING(database.instanceValue(firstInstanceUID, missingTag), QString());
  // Empty inputs
  CHECK_INT(database.instanceValues(QStringList(), instanceNumberTag).size(), 0);
  QStringList valuesForEmptyTag = database.instanceValues(copiedInstanceUIDs, QString());
  CHECK_INT(valuesForEmptyTag.size(), numberOfCopies);
  CHECK_QSTRING(valuesForEmptyTag[0], QString());

  // Instance for file must not be returned after the image is removed from the database
  QString seriesUID = database.seriesForFile(dicomFilePaths[0]);
  CHECK_BOOL(database.instanceForFile(dicomFilePaths[0]).isEmpty(), false);
  CHECK_BOOL(database.removeSeries(seriesUID), true);
  CHECK_QSTRING(database.instanceForFile(dicomFilePaths[0]), QString());
  CHECK_BOOL(checkFileValues(database, fileNames, modalityTag), true);

  database.closeDatabase();

  return EXIT_SUCCESS;
}
