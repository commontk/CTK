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
#include "ctkVTKDataSetArrayComboBox.h"
#include "ctkVTKDataSetModel.h"

// VTK includes
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKDataSetArrayComboBoxTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  vtkNew<vtkPolyData> dataSet;

  vtkNew<vtkIntArray> ints;
  ints->SetName("Ints");
  dataSet->GetPointData()->AddArray(ints.GetPointer());

  vtkNew<vtkFloatArray> floats;
  floats->SetName("Floats");
  dataSet->GetCellData()->AddArray(floats.GetPointer());

  ctkVTKDataSetArrayComboBox comboBox;
  comboBox.dataSetModel()->setAttributeTypes(ctkVTKDataSetModel::AllAttribute);
  comboBox.setDataSet(dataSet.GetPointer());

  if (comboBox.count()!=2)
    {
    std::cerr << "Line " << __LINE__ << " - Expected 2 items in the combobox\n"
                  "\tCurrent count: " << comboBox.count() << "\n";
    return EXIT_FAILURE;
    }

  comboBox.setNoneEnabled(true);
  if (comboBox.count()!=3)
    {
    std::cerr << "Line " << __LINE__ << " - Expected 3 items in the combobox\n"
                  "\tCurrent count: " << comboBox.count() << "\n";
    return EXIT_FAILURE;
    }
  if (!comboBox.itemText(0).isEmpty())
    {
    std::cerr << "Line " << __LINE__ << " - First combo box item text is expected to be empty\n";
    return EXIT_FAILURE;
    }

  comboBox.setNoneEnabled(false);
  if (comboBox.count()!=2)
    {
    std::cerr << "Line " << __LINE__ << " - Expected 2 items in the combobox\n"
                  "\tCurrent count: " << comboBox.count() << "\n";
    return EXIT_FAILURE;
    }

  comboBox.show();

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(1000, &app, SLOT(quit()));
    }
  return app.exec();
}
