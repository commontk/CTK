/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QTimer>

// ctkDICOMCore includes
#include "ctkDICOMImage.h"
#include "ctkDICOMDatasetView.h"

// DCMTK includes
#include <dcmimage.h>

// STD includes
#include <iostream>

/* Test from build directory:
 ./CTK-build/bin/CTKDICOMWidgetsCxxTests ctkDICOMDatasetViewTest1 test.db ../CTK/Libs/DICOM/Core/Resources/dicom-sample.sql
*/

int ctkDICOMDatasetViewTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
  if (argc < 2)
    {
    std::cerr << "Usage: ctkDICOMDatasetViewTest1 dcmimage [-I]" << std::endl;
    return EXIT_FAILURE;
    }
  
  DicomImage    img(argv[1]);
  ctkDICOMImage dicomImage(&img);
  ctkDICOMImage dicomImage2(0);
  QImage image;
  QImage image2(200, 200, QImage::Format_RGB32);
  
  ctkDICOMDatasetView datasetView;
  datasetView.addImage(dicomImage);
  datasetView.addImage(dicomImage2);
  datasetView.addImage(image);
  datasetView.addImage(image2);
  datasetView.update( false, false );
  datasetView.update( false, true);
  datasetView.update( true, false);
  datasetView.update( true, true);
  datasetView.show();

  if (argc <= 2 || QString(argv[2]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
