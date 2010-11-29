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
#include <QTimer>

// CTK includes
#include "ctkVTKDataSetArrayComboBox.h"

// VTK includes
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKDataSetArrayComboBoxTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  vtkSmartPointer<vtkPolyData> dataSet =
      vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkIntArray> ints = vtkSmartPointer<vtkIntArray>::New();
  ints->SetName("Ints");
  dataSet->GetPointData()->AddArray(ints);
  vtkSmartPointer<vtkFloatArray> floats= vtkSmartPointer<vtkFloatArray>::New();
  floats->SetName("Floats");
  dataSet->GetCellData()->AddArray(floats);
  
  ctkVTKDataSetArrayComboBox comboBox;
  comboBox.setDataSet(dataSet);
  comboBox.show();

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(1000, &app, SLOT(quit()));
    }
  return app.exec();
}
