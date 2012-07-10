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
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// VTK includes
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkVTKDataSetArrayComboBox.h"
#include "ctkEventTranslatorPlayerWidget.h"

// QtTesting includes
#include "pqTestUtility.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
QSignalSpy *Spy1;
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkVTKDataSetArrayComboBox* widget = reinterpret_cast<ctkVTKDataSetArrayComboBox*>(data);

  CTKCOMPARE(Spy1->count(), 3);
  CTKCOMPARE(widget->currentText(), "Floats");
  }
}

//-----------------------------------------------------------------------------
int ctkVTKDataSetArrayComboBoxEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Visualization/VTK/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  vtkSmartPointer<vtkPolyData> dataSet =
      vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkIntArray> ints = vtkSmartPointer<vtkIntArray>::New();
  ints->SetName("Ints");
  dataSet->GetPointData()->AddArray(ints);
  vtkSmartPointer<vtkFloatArray> floats= vtkSmartPointer<vtkFloatArray>::New();
  floats->SetName("Floats");
  dataSet->GetCellData()->AddArray(floats);

  ctkVTKDataSetArrayComboBox* widget = new ctkVTKDataSetArrayComboBox();
  widget->setDataSet(dataSet);
  widget->show();

  QSignalSpy spy1(widget, SIGNAL(currentArrayChanged(QString)));

  Spy1 = &spy1;

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkVTKDataSetArrayComboBoxEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

