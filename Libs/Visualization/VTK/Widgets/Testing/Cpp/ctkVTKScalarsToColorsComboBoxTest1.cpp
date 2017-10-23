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
#include <QDebug>
#include <QTimer>

// CTK includes
#include "ctkVTKScalarsToColorsComboBox.h"

// VTK includes
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>

// STD includes

//-----------------------------------------------------------------------------
int ctkVTKScalarsToColorsComboBoxTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkVTKScalarsToColorsComboBox scalarsToColorsComboBox;
  //scalarsToColorsComboBox.setFixedHeight(25);
  //scalarsToColorsComboBox.setDefaultText("Select color transfer function ...");
  //scalarsToColorsComboBox.setCurrentIndex(-1);//Start with default

  //Dummy presets
  vtkNew<vtkDiscretizableColorTransferFunction> discretizableCTF;
  discretizableCTF->AddRGBPoint(0.0, 0, 0, 1.0);
  discretizableCTF->AddRGBPoint(255.0, 1.0, 0, 0);
  vtkNew<vtkPiecewiseFunction> piecewiseFunction;
  piecewiseFunction->AddPoint(0.0, 0);
  piecewiseFunction->AddPoint(255.0, 1.0);
  discretizableCTF->SetScalarOpacityFunction(piecewiseFunction.GetPointer());
  discretizableCTF->EnableOpacityMappingOn();
  vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  colorTransferFunction->AddRGBPoint(255, 0, 0, 1.0);
  colorTransferFunction->AddRGBPoint(0, 1.0, 0, 0);

  scalarsToColorsComboBox.addScalarsToColors(discretizableCTF.Get(), "ColorTransferFunctionTest1");
  scalarsToColorsComboBox.addScalarsToColors(colorTransferFunction.Get(), "ColorTransferFunctionTest2");

  if (scalarsToColorsComboBox.count() != 2)
  {
    std::cerr << "Line " << __LINE__ << " - Expected 2 items in the combobox\n"
      "\tCurrent count: " << scalarsToColorsComboBox.count() << "\n";
    return EXIT_FAILURE;
  }
  scalarsToColorsComboBox.addScalarsToColors(CTK_NULLPTR, "(none)");

  scalarsToColorsComboBox.show();

  if (argc < 2 || QString(argv[1]) != "-I")
  {
    QTimer::singleShot(1000, &app, SLOT(quit()));
  }

  return app.exec();
}
