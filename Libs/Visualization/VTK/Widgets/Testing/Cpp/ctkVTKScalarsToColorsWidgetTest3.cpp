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
#include "ctkCoreTestingMacros.h"
#include "ctkVTKScalarsToColorsView.h"
#include "ctkVTKScalarsToColorsWidget.h"
#include "ctkVTKWidgetsUtils.h"

// VTK includes
#include <vtkChartXY.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPlot.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKScalarsToColorsWidgetTest3(int argc, char * argv [] )
{
  ctk::vtkSetSurfaceDefaultFormat();

  QApplication app(argc, argv);

  // Transfer Function
  vtkSmartPointer<vtkColorTransferFunction> ctf =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  //
  ctf->AddRGBPoint(0. , 69./255., 199./255.,   186./255.);
  ctf->AddRGBPoint(0.2,  172./255., 3./255., 57./255.);
  ctf->AddRGBPoint(0.4,  169./255., 83./255., 58./255.);
  ctf->AddRGBPoint(0.6,  43./255., 32./255.,  161./255.);
  ctf->AddRGBPoint(0.8,  247./255., 158./255., 97./255.);
  ctf->AddRGBPoint(1.,  183./255., 7./255., 140./255.);

  ctkVTKScalarsToColorsWidget widget(0);

  // check default values
  CHECK_BOOL(widget.editColors(), true)
  CHECK_BOOL(widget.areTopWidgetsVisible(), true)
  CHECK_NULL(widget.currentControlPointsItem())

  // add transfer function item
  widget.view()->addColorTransferFunction(ctf);
  widget.view()->setAxesToChartBounds();
  widget.show();

  CHECK_NOT_NULL(widget.currentControlPointsItem())

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
