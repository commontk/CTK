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
#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QTimer>

// CTK includes
#include "ctkWidgetsUtils.h"
#include "ctkVTKWidgetsUtils.h"

// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKScalarsToColorsUtilsTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  vtkSmartPointer<vtkColorTransferFunction> ctf =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  ctf->AddRGBPoint(-0.5, 194./255,  78./255,  68./255);
  ctf->AddRGBPoint(2.2, 194./255, 141./255,  68./255);
  ctf->AddRGBPoint(3.3,  68./255, 194./255,  78./255);
  ctf->AddRGBPoint(4.8,  68./255, 194./255, 141./255);
  ctf->AddRGBPoint(5.9,  78./255,  68./255, 194./255);
  ctf->AddRGBPoint(7.0, 141./255,  68./255, 194./255);
  
  QImage image = ctk::scalarsToColorsImage(ctf);

  QLabel label;
  label.setPixmap( QPixmap::fromImage(image));
  label.show();

  QLabel label2;
  label2.setText( QString("<img src=\"%1\">: text")
    .arg(ctk::base64HTMLImageTagSrc(image)));
  label2.show();
  
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
