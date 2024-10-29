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
#include <QApplication>
#include <QDir>
#include <QLabel>


// ctkDICOMCore includes
#include "ctkDICOMImage.h"

// DCMTK includes
#include <dcmtk/dcmimgle/dcmimage.h>

// STD includes
#include <iostream>


int ctkDICOMImageTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();

  bool interactive = arguments.removeOne("-I");

  if (arguments.count() != 1)
  {
    std::cerr << "Warning, no dicom file given. Test stops" << std::endl;
    std::cerr << "Usage: " << qPrintable(testName)
              << " [-I] <path-to-dicom-file>" << std::endl;
    return EXIT_FAILURE;
  }

  QString dicomFilePath(arguments.at(0));

  DicomImage dcmtkImage(QDir::toNativeSeparators(dicomFilePath).toUtf8().data());
  ctkDICOMImage ctkImage(&dcmtkImage);

  QLabel qtImage;
  QPixmap pixmap = QPixmap::fromImage(ctkImage.frame(0),Qt::AvoidDither);
  if (pixmap.isNull())
  {
    std::cerr << "Failed to convert QImage to QPixmap" ;
    return EXIT_FAILURE;
  }
  qtImage.setPixmap(pixmap);
  qtImage.show();

  if (interactive)
  {
    return app.exec();
  }
  return EXIT_SUCCESS;
}
