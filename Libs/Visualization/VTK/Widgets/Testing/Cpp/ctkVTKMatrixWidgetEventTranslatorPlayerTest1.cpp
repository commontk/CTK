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
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// VTK includes
#include <vtkMatrix4x4.h>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkEventTranslatorPlayerWidget.h"

// QtTesting includes
#include "pqTestUtility.h"
#include "ctkMatrixWidgetEventPlayer.h"
#include "ctkMatrixWidgetEventTranslator.h"
#include "ctkVTKMatrixWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkVTKMatrixWidget* widget = reinterpret_cast<ctkVTKMatrixWidget*>(data);

  CTKCOMPARE(widget->value(1,0),2.00);
  CTKCOMPARE(widget->value(2,0),-0.14);
  CTKCOMPARE(widget->value(0,3),5.00);
  }
}

//-----------------------------------------------------------------------------
int ctkVTKMatrixWidgetEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Visualization/VTK/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventTranslator(new ctkMatrixWidgetEventTranslator);
  etpWidget.addWidgetEventPlayer(new ctkMatrixWidgetEventPlayer);

  // Test case 1
  ctkVTKMatrixWidget* widget = new ctkVTKMatrixWidget(0);
  widget->setEditable(true);
  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  widget->setMatrix(matrix);
  matrix->Delete();
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkVTKMatrixWidgetEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

