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
#include "ctkVTKMatrixWidget.h"

// VTK includes
#include <vtkMatrix4x4.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKMatrixWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkVTKMatrixWidget matrixWidget(0);

  if (matrixWidget.isEnabled())
    {
    std::cerr << "No vtkMatrix4x4 provided, should be disabled."
              << std::endl;
    return EXIT_FAILURE;
    }

  vtkMatrix4x4* matrix = vtkMatrix4x4::New();

  matrixWidget.setMatrix(matrix);
  matrix->Delete();

  if (matrixWidget.matrix() != matrix)
    {
    std::cerr << "ctkVTKMatrixWidget::setMatrix() failed."
              << matrixWidget.matrix() << std::endl;
    return EXIT_FAILURE;
    }

  if (matrixWidget.rowCount() != 4 ||
      matrixWidget.columnCount() != 4)
    {
    std::cerr << "ctkVTKMatrixWidget wrong dimension" << std::endl;
    return EXIT_FAILURE;
    }

  if (matrixWidget.value(3,3) != 1. ||
      matrixWidget.value(2,3) != 0.)
    {
    std::cerr << "Wrong value" << matrixWidget.value(3,3)
              << " " << matrixWidget.value(2,3) << std::endl;
    return EXIT_FAILURE;
    }

  matrix->SetElement(1, 3, 50.);
  if (!qFuzzyCompare(matrix->GetElement(1,3), 50.) ||
      !qFuzzyCompare(matrixWidget.value(1,3), 50.))
    {
    std::cerr << "vtkMatrix4x4::SetValue() failed:"
              << matrix->GetElement(1,3) << " "
              << matrixWidget.value(1,3) << std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.setValue(2,2, -17.);

  if (!qFuzzyCompare(matrixWidget.value(2,2), -17.) ||
      !qFuzzyCompare(matrix->GetElement(2,2), -17.))
    {
    std::cerr << "ctkVTKMatrixWidget::setValue() failed:"
              << matrix->GetElement(2,2) << " "
              << matrixWidget.value(2,2) << std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

