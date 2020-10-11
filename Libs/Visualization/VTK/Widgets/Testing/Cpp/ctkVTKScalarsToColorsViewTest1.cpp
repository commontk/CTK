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
#include <QSharedPointer>
#include <QTimer>

// CTK includes
#include "ctkVTKScalarsToColorsView.h"
#include "ctkVTKWidgetsUtils.h"

// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKScalarsToColorsViewTest1(int argc, char * argv [] )
{
  ctk::vtkSetSurfaceDefaultFormat();

  QApplication app(argc, argv);

  vtkSmartPointer<vtkColorTransferFunction> ctf =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  //
  ctf->AddRGBPoint(0.2, 0.6941,0.76,0., 0.5, 0.);
  ctf->AddRGBPoint(0.4, 0.,0.6941,0.96);
  ctf->AddRGBPoint(0.8, 0.9686,0.76,0.);

  ctkVTKScalarsToColorsView view(0);
  view.addColorTransferFunction(ctf);
  view.setAxesToChartBounds();
  view.show();

  ctf->AddRGBPoint(0.6, 0.9686,0.0,0.6941);

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
