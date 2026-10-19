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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

// Qt includes
#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <QTemporaryDir>
#include <QTemporaryFile>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMThumbnailGenerator.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>

// STD includes
#include <cstdio>
#include <functional>
#include <iostream>
#ifdef _WIN32
# include <io.h>
# define ctk_dup _dup
# define ctk_dup2 _dup2
# define ctk_close _close
#else
# include <unistd.h>
# define ctk_dup dup
# define ctk_dup2 dup2
# define ctk_close close
#endif

namespace
{

//------------------------------------------------------------------------------
/// What \a call writes on the standard error, where DCMTK logs
QString standardErrorOf(const std::function<void()>& call)
{
  QTemporaryFile capture;
  if (!capture.open())
  {
    call();
    return QString();
  }
  std::cerr.flush();
  fflush(stderr);
  const int standardError = ctk_dup(2);
  ctk_dup2(capture.handle(), 2);
  call();
  std::cerr.flush();
  fflush(stderr);
  ctk_dup2(standardError, 2);
  ctk_close(standardError);
  capture.seek(0);
  return QString::fromUtf8(capture.readAll());
}

} // end of anonymous namespace

//------------------------------------------------------------------------------
int ctkDICOMThumbnailGeneratorTest1(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();
  if (arguments.count() != 1)
  {
    std::cerr << "Usage: " << qPrintable(testName) << " <path-to-dicom-image>" << std::endl;
    return EXIT_FAILURE;
  }
  const QString imageFile = arguments.at(0);

  QTemporaryDir directory;
  CHECK_BOOL(directory.isValid(), true);

  // The same object, without its pixel data: like a structured report or an
  // encapsulated document, it has no image to render
  const QString noImageFile = QDir(directory.path()).filePath("no-pixel-data.dcm");
  DcmFileFormat fileFormat;
  CHECK_BOOL(fileFormat.loadFile(QDir::toNativeSeparators(imageFile).toUtf8().constData()).good(), true);
  CHECK_BOOL(fileFormat.getDataset()->findAndDeleteElement(DCM_PixelData).good(), true);
  CHECK_BOOL(fileFormat.saveFile(QDir::toNativeSeparators(noImageFile).toUtf8().constData()).good(), true);

  CHECK_BOOL(ctkDICOMAbstractThumbnailGenerator::hasPixelData(imageFile), true);
  CHECK_BOOL(ctkDICOMAbstractThumbnailGenerator::hasPixelData(noImageFile), false);

  ctkDICOMThumbnailGenerator generator;
  QImage thumbnail;
  CHECK_BOOL(generator.generateThumbnail(imageFile, thumbnail), true);
  CHECK_BOOL(thumbnail.isNull(), false);

  // No thumbnail for an object without image, and no error logged about it: DCMTK
  // logged "no pixel data found in DICOM dataset", and the generator a warning, for
  // each such object.
  bool generated = true;
  QString logged = standardErrorOf([&]()
    {
      QImage noThumbnail;
      generated = generator.generateThumbnail(noImageFile, noThumbnail);
      generated = generator.generateThumbnail(noImageFile, QDir(directory.path()).filePath("thumbnail.png")) || generated;
    });
  CHECK_BOOL(generated, false);
  if (!logged.isEmpty())
  {
    std::cerr << "Line " << __LINE__ << " - logged for an object without image: "
              << qPrintable(logged) << std::endl;
    return EXIT_FAILURE;
  }

  // The thumbnails stored by the database skip it as well
  ctkDICOMDatabase database;
  database.openDatabase(QDir(directory.path()).filePath("ctkDICOM.sql"));
  database.setThumbnailGenerator(&generator);
  logged = standardErrorOf([&]()
    {
      generated = database.storeThumbnailFile(noImageFile, "1.2.3", "1.2.3.4", "1.2.3.4.5", "MR");
    });
  CHECK_BOOL(generated, false);
  if (logged.contains("pixel data") || logged.contains("Rendering"))
  {
    std::cerr << "Line " << __LINE__ << " - logged for an object without image: "
              << qPrintable(logged) << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
