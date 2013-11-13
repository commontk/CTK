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
#include <QComboBox>
#include <QTimer>
#include <QDebug>

// CTK includes
#include "ctkVTKDataSetModel.h"

// VTK includes
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkStringArray.h>

// STD includes
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
bool checkItems(int line, const QList<vtkAbstractArray*>& expectedAttributeArrays, ctkVTKDataSetModel* dataSetModel)
{
  foreach(vtkAbstractArray* expectedDataArray, expectedAttributeArrays)
    {

    QList<QStandardItem*> items = dataSetModel->findItems(expectedDataArray->GetName());
    if(items.count() != 1)
      {
      std::cerr << "Line " << line << " - Problem with number of item matching name:" << expectedDataArray->GetName() << "\n"
                   "\tExpected count: 1\n"
                   "\tCurrent count: " << items.count() << "\n";
      return false;
      }
    vtkAbstractArray * currentDataArray = dataSetModel->arrayFromItem(items.at(0));
    if (currentDataArray != expectedDataArray)
      {
      std::cerr << "Line " << line << " - Problem with model - Incorrect array associated with item name:" << expectedDataArray->GetName() << "\n"
                   "\tExpected: " << expectedDataArray << "\n"
                   "\tCurrent: " << currentDataArray << "\n";
      return false;
      }
    }
  return true;
}
}

//-----------------------------------------------------------------------------
int ctkVTKDataSetModelTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  vtkNew<vtkPolyData> dataSet;

  vtkNew<vtkIntArray> ints;
  {
    ints->SetName("Ints");
    int added = dataSet->GetPointData()->AddArray(ints.GetPointer());
    if (added == -1)
      {
      std::cerr << "Line " << __LINE__ << " - Failed to add Ints array";
      return EXIT_FAILURE;
      }
  }

  vtkNew<vtkFloatArray> floats;
  {
    floats->SetName("Floats");
    int added = dataSet->GetCellData()->AddArray(floats.GetPointer());
    if (added == -1)
      {
      std::cerr << "Line " << __LINE__ << " - Failed to add Floats array";
      return EXIT_FAILURE;
      }
  }

  vtkNew<vtkStringArray> strings;
  {
    strings->SetName("Strings");
    int added = dataSet->GetCellData()->AddArray(strings.GetPointer());
    if (added == -1)
      {
      std::cerr << "Line " << __LINE__ << " - Failed to add Strings array";
      return EXIT_FAILURE;
      }
  }

  vtkNew<vtkFloatArray> scalars;
  {
    scalars->SetName("Scalars");
    scalars->SetNumberOfComponents(0);
    int added = dataSet->GetPointData()->SetScalars(scalars.GetPointer());
    if (added == -1)
      {
      std::cerr << "Line " << __LINE__ << " - Failed to add Scalars array";
      return EXIT_FAILURE;
      }
  }

  vtkNew<vtkFloatArray> vectors;
  {
    vectors->SetNumberOfComponents(3);
    vectors->SetName("Vectors");
    int added = dataSet->GetPointData()->SetVectors(vectors.GetPointer());
    if (added == -1)
      {
      std::cerr << "Line " << __LINE__ << " - Failed to add Vectors array";
      return EXIT_FAILURE;
      }
  }

  vtkNew<vtkFloatArray> normals;
  {
    normals->SetNumberOfComponents(3);
    normals->SetName("Normals");
    int added = dataSet->GetPointData()->SetNormals(normals.GetPointer());
    if (added == -1)
      {
      std::cerr << "Line " << __LINE__ << " - Failed to add Normals array";
      return EXIT_FAILURE;
      }
  }

  vtkNew<vtkFloatArray> tcoords;
  {
    tcoords->SetNumberOfComponents(3);
    tcoords->SetName("Tcoords");
    int added = dataSet->GetPointData()->SetTCoords(tcoords.GetPointer());
    if (added == -1)
      {
      std::cerr << "Line " << __LINE__ << " - Failed to add Tcoords array";
      return EXIT_FAILURE;
      }
  }

  vtkNew<vtkFloatArray> tensors;
  {
    tensors->SetNumberOfComponents(9);
    tensors->SetName("Tensors");
    int added = dataSet->GetPointData()->SetTensors(tensors.GetPointer());
    if (added == -1)
      {
      std::cerr << "Line " << __LINE__ << " - Failed to add Tensors array";
      return EXIT_FAILURE;
      }
  }

  vtkNew<vtkFloatArray> globalids;
  {
    globalids->SetNumberOfComponents(1);
    globalids->SetName("GlobalIDs");
    int added = dataSet->GetPointData()->SetGlobalIds(globalids.GetPointer());
    if (added == -1)
      {
      std::cerr << "Line " << __LINE__ << " - Failed to add GlobalIDs array";
      return EXIT_FAILURE;
      }
  }

  vtkNew<vtkFloatArray> pedigreeids;
  {
    pedigreeids->SetNumberOfComponents(1);
    pedigreeids->SetName("PedigreeIDs");
    int added = dataSet->GetPointData()->SetPedigreeIds(pedigreeids.GetPointer());
    if (added == -1)
      {
      std::cerr << "Line " << __LINE__ << " - Failed to add PedigreeIDs array";
      return EXIT_FAILURE;
      }
  }

  ctkVTKDataSetModel dataSetModel;
  dataSetModel.setDataSet(dataSet.GetPointer());

  QList<vtkAbstractArray*> notAttributeArrays;
  notAttributeArrays << ints.GetPointer() << floats.GetPointer();

  if (!checkItems(__LINE__, notAttributeArrays, &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::ScalarsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << scalars.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::NoAttribute | ctkVTKDataSetModel::ScalarsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << notAttributeArrays << scalars.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::VectorsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << vectors.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::VectorsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << vectors.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::NoAttribute | ctkVTKDataSetModel::VectorsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << notAttributeArrays << vectors.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::NormalsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << normals.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::NoAttribute | ctkVTKDataSetModel::NormalsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << notAttributeArrays << normals.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::TCoordsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << tcoords.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::NoAttribute | ctkVTKDataSetModel::TCoordsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << notAttributeArrays << tcoords.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::TensorsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << tensors.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::NoAttribute | ctkVTKDataSetModel::TensorsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << notAttributeArrays << tensors.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::GlobalIDsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << globalids.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::NoAttribute | ctkVTKDataSetModel::GlobalIDsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << notAttributeArrays << globalids.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::PedigreeIDsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << pedigreeids.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::NoAttribute | ctkVTKDataSetModel::PedigreeIDsAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << notAttributeArrays << pedigreeids.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  dataSetModel.setAttributeTypes(ctkVTKDataSetModel::AllAttribute);
  if (!checkItems(__LINE__, QList<vtkAbstractArray*>() << notAttributeArrays
                  << scalars.GetPointer() << vectors.GetPointer()
                  << normals.GetPointer() << tcoords.GetPointer()
                  << tensors.GetPointer() << globalids.GetPointer()
                  << pedigreeids.GetPointer(), &dataSetModel))
    {
    return EXIT_FAILURE;
    }

  QComboBox comboBox;
  comboBox.setModel(&dataSetModel);
  comboBox.show();

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(1000, &app, SLOT(quit()));
    }
  return app.exec();
}
