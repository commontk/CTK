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
#include <QTimer>

// CTK includes
#include "ctkVTKDiscretizableColorTransferWidget.h"
#include "ctkVTKScalarsToColorsComboBox.h"
#include "vtkScalarsToColorsHistogramChart.h"

// VTK includes
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRTAnalyticSource.h>

//-----------------------------------------------------------------------------
int ctkVTKDiscretizableColorTransferWidgetTest1(int argc, char * argv[])
{
  QApplication app(argc, argv);

  ///Dummy presets
  vtkNew<vtkDiscretizableColorTransferFunction> dctf0;
  dctf0->AddRGBPoint(0.0, 0.0, 0.0, 1.0);
  dctf0->AddRGBPoint(255.0, 1.0, 0.0, 0.0);
  vtkNew<vtkPiecewiseFunction> pf0;
  pf0->AddPoint(0.0, 0.0);
  pf0->AddPoint(255.0, 1.0);
  dctf0->SetScalarOpacityFunction(pf0.Get());
  dctf0->EnableOpacityMappingOn();

  vtkNew<vtkDiscretizableColorTransferFunction> dctf1;
  dctf1->AddRGBPoint(0.0, 1.0, 1.0, 1.0);
  dctf1->AddRGBPoint(255.0, 0.0, 0.0, 0.0);
  vtkNew<vtkPiecewiseFunction> pf1;
  pf1->AddPoint(0.0, 0.0);
  pf1->AddPoint(255.0, 0.5);
  dctf1->SetScalarOpacityFunction(pf1.Get());
  dctf1->EnableOpacityMappingOn();

  vtkNew<vtkColorTransferFunction> ctf0;
  ctf0->AddRGBPoint(0.0, 1.0, 0.0, 1.0);
  ctf0->AddRGBPoint(255.0, 0.0, 1.0, 0.0);

  /// Create an image to test the widget histogram
  vtkNew<vtkRTAnalyticSource> imageSource;
  imageSource->SetWholeExtent(0, 9,
                              0, 9,
                              0, 9);

  /// Discretizable transfer function widget
  ctkVTKDiscretizableColorTransferWidget mWidget;
  mWidget.copyColorTransferFunction(dctf0.Get());
  mWidget.setHistogramConnection(imageSource->GetOutputPort());
  mWidget.setLeftAxisMode(vtkScalarsToColorsHistogramChart::MAXIMUM);
  mWidget.updateHistogram(true);
  mWidget.show();

  /// Add presets to the widget selector
  mWidget.scalarsToColorsSelector()->addScalarsToColors(dctf1.Get(), "White to Black");
  mWidget.scalarsToColorsSelector()->addScalarsToColors(ctf0.Get(), "Purple to Green");

  if (argc < 2 || QString(argv[1]) != "-I")
  {
    QTimer::singleShot(1000, &app, SLOT(quit()));
  }

  return app.exec();
}

