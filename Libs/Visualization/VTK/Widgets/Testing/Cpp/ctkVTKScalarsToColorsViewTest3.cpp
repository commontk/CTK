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
#include <vtkPiecewiseFunction.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKScalarsToColorsViewTest3(int argc, char * argv [] )
{
  ctk::vtkSetSurfaceDefaultFormat();

  QApplication app(argc, argv);

  // Transfer Function
  vtkSmartPointer<vtkColorTransferFunction> ctf =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  //
  ctf->AddRGBPoint(0. , 220./255., 173./255.,   3./255.);
  ctf->AddRGBPoint(0.2,   1./255., 152./255., 231./255.);
  ctf->AddRGBPoint(0.4,  79./255., 235./255., 237./255.);
  ctf->AddRGBPoint(0.6,  52./255., 193./255.,  72./255.);
  ctf->AddRGBPoint(0.8,  67./255., 136./255., 151./255.);
  ctf->AddRGBPoint(1. ,  78./255.,  87./255., 179./255.);

  // Opacity function
  vtkSmartPointer<vtkPiecewiseFunction> opacityFunction =
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  opacityFunction->AddPoint(0.,0.3, 0.5, 0.5);
  opacityFunction->AddPoint(0.2, 0.1, 0.5, 0.5);
  opacityFunction->AddPoint(0.4,0.87, 0.5, 0.5);
  opacityFunction->AddPoint(0.6, 1., 0.5, 0.5);
  opacityFunction->AddPoint(0.8, 0.5, 0.5, 0.5);
  opacityFunction->AddPoint(1.,0.8, 0.5, 0.5);

  ctkVTKScalarsToColorsView view(0);
  // add transfer function item
  view.addCompositeFunction(ctf, opacityFunction);
  view.setAxesToChartBounds();
  view.show();

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
