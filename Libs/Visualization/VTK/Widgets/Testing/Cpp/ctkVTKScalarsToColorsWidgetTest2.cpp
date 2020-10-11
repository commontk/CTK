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
int ctkVTKScalarsToColorsWidgetTest2(int argc, char * argv [] )
{
  ctk::vtkSetSurfaceDefaultFormat();

  QApplication app(argc, argv);

  // Transfer Function
  vtkSmartPointer<vtkColorTransferFunction> ctf =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  //
  ctf->AddRGBPoint(0. , 171./255., 113./255.,   38./255.);
  ctf->AddRGBPoint(0.2,   38./255., 171./255., 113./255.);
  ctf->AddRGBPoint(0.4,  113./255., 38./255., 171./255.);
  ctf->AddRGBPoint(0.6,  171./255., 38./255.,  140./255.);
  ctf->AddRGBPoint(0.8,  171./255., 46./255., 38./255.);

  // Opacity function
  vtkSmartPointer<vtkPiecewiseFunction> opacityFunction =
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  opacityFunction->AddPoint(0.,0.3);
  opacityFunction->AddPoint(0.2, 0.55);
  opacityFunction->AddPoint(0.4,0.87);
  opacityFunction->AddPoint(0.6, 1.);
  opacityFunction->AddPoint(0.8, 0.45);

  ctkVTKScalarsToColorsWidget widget(0);

  // check default values
  CHECK_BOOL(widget.editColors(), true)
  CHECK_BOOL(widget.areTopWidgetsVisible(), true)
  CHECK_NULL(widget.currentControlPointsItem())

  // add transfer function item
  widget.view()->addCompositeFunction(ctf, opacityFunction);
  widget.view()->setAxesToChartBounds();
  widget.show();

  CHECK_NOT_NULL(widget.currentControlPointsItem())

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
