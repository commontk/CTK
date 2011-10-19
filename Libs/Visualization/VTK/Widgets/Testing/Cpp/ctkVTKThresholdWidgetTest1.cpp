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

// QT includes
#include <QApplication>
#include <QTimer>

// qMRML includes
#ifdef CTK_USE_CHARTS
#include "ctkVTKScalarsToColorsView.h"
#endif
#include "ctkVTKThresholdWidget.h"

// VTK includes
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>

// STD includes
#include <cstdlib>
#include <iostream>

int ctkVTKThresholdWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  vtkNew<vtkPiecewiseFunction> function;

  ctkVTKThresholdWidget thresholdWidget;

  thresholdWidget.setPiecewiseFunction(function.GetPointer());
  if (thresholdWidget.piecewiseFunction() != function.GetPointer())
    {
    std::cerr << "ctkVTKThresholdWidget::setPiecewiseFunction() failed"
              << std::endl;
    return EXIT_FAILURE;
    }

  thresholdWidget.setRange(10., 20.);
  double range[2];
  thresholdWidget.range(range);
  if (range[0] != 10. || range[1] != 20.)
    {
    std::cerr << "ctkVTKThresholdWidget::setRange() failed: "
              << range[0] << " " << range[1] << std::endl;
    return EXIT_FAILURE;
    }

  thresholdWidget.setOpacity(1.);
  if (thresholdWidget.opacity() != 1.)
    {
    std::cerr << "ctkVTKThresholdWidget::setOpacity() failed: "
              << thresholdWidget.opacity() << std::endl;
    return EXIT_FAILURE;
    }

  thresholdWidget.setThresholdValues(11., 19.);
  double values[2];
  thresholdWidget.thresholdValues(values);
  if (values[0] != 11. || values[1] != 19.)
    {
    std::cerr << "ctkVTKThresholdWidget::setThresholdValues() failed: "
              << values[0] << " " << values[1] << std::endl;
    return EXIT_FAILURE;
    }

  thresholdWidget.show();

#ifdef CTK_USE_CHARTS
  ctkVTKScalarsToColorsView view;
  view.addOpacityFunction(function.GetPointer());
  view.show();
#endif

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
