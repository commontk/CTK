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
#include <QFile>
#include <QTemporaryDir>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcuid.h>

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{

const char* SOPInstanceUID = "1.2.826.0.1.3680043.10.14.1.1.1";

/// Messages logged by the database and by DCMTK when a file cannot be read
int NumberOfReadErrors = 0;
QtMessageHandler DefaultMessageHandler = nullptr;

//------------------------------------------------------------------------------
void countReadErrors(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
  if (message.contains("could not be initialized") || message.contains("Could not load"))
  {
    ++NumberOfReadErrors;
  }
  if (DefaultMessageHandler)
  {
    DefaultMessageHandler(type, context, message);
  }
}

//------------------------------------------------------------------------------
bool writeImage(const QString& filePath)
{
  DcmFileFormat fileFormat;
  DcmDataset* dataset = fileFormat.getDataset();
  dataset->putAndInsertString(DCM_PatientName, "Unreadable^File");
  dataset->putAndInsertString(DCM_PatientID, "UNREADABLE");
  dataset->putAndInsertString(DCM_StudyInstanceUID, "1.2.826.0.1.3680043.10.14.1");
  dataset->putAndInsertString(DCM_SeriesInstanceUID, "1.2.826.0.1.3680043.10.14.1.1");
  dataset->putAndInsertString(DCM_SOPInstanceUID, SOPInstanceUID);
  dataset->putAndInsertString(DCM_SOPClassUID, UID_MRImageStorage);
  dataset->putAndInsertString(DCM_Modality, "MR");
  dataset->putAndInsertString(DCM_SliceThickness, "1.5");
  return fileFormat.saveFile(filePath.toLocal8Bit().constData(), EXS_LittleEndianExplicit).good();
}

} // end of anonymous namespace

// Checks that a file that cannot be read (e.g. moved or deleted after it was linked
// in the database) is reported once, instead of each time one of its values is asked:
// the models of the visual browser ask them each time they are populated, and the
// reports piled up in the error log of the application.
int ctkDICOMDatabaseTest10(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  QTemporaryDir databaseDirectory;
  QTemporaryDir filesDirectory;
  CHECK_BOOL(databaseDirectory.isValid(), true);
  CHECK_BOOL(filesDirectory.isValid(), true);
  const QString filePath = QDir(filesDirectory.path()).filePath("image.dcm");
  CHECK_BOOL(writeImage(filePath), true);

  ctkDICOMDatabase database;
  database.openDatabase(QDir(databaseDirectory.path()).filePath("ctkDICOM.sql"));
  // The file is linked, not copied in the database
  database.insert(filePath, /*storeFile=*/false, /*generateThumbnail=*/false);
  CHECK_QSTRING(database.fileForInstance(SOPInstanceUID), filePath);

  DefaultMessageHandler = qInstallMessageHandler(countReadErrors);

  // The file disappears. Its slice thickness, which was never asked, is not in the
  // tag cache: the database reads it from the file.
  CHECK_BOOL(QFile::remove(filePath), true);
  for (int read = 0; read < 3; ++read)
  {
    CHECK_QSTRING(database.instanceValue(SOPInstanceUID, "0018,0050"), QString());
  }
  // "Could not load" (DCMTK) and "could not be initialized" (database), once
  std::cout << "Read errors reported for 3 reads of a missing file: " << NumberOfReadErrors << std::endl;
  CHECK_INT(NumberOfReadErrors, 2);

  // The file is restored: its values are read again
  CHECK_BOOL(writeImage(filePath), true);
  CHECK_QSTRING(database.instanceValue(SOPInstanceUID, "0018,0050"), QString("1.5"));
  CHECK_INT(NumberOfReadErrors, 2);

  qInstallMessageHandler(DefaultMessageHandler);
  database.closeDatabase();
  return EXIT_SUCCESS;
}
