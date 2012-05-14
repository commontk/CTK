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
#include <vtkScalarBarWidget.h>
#include <vtkSmartPointer.h>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkVTKScalarBarWidget.h"
#include "ctkEventTranslatorPlayerWidget.h"

// QtTesting includes
#include "pqTestUtility.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkVTKScalarBarWidget* widget = reinterpret_cast<ctkVTKScalarBarWidget*>(data);

  CTKCOMPARE(widget->title(), "Test QtTesting");
  CTKCOMPARE(widget->labelsFormat(), "%-#6.3g..ghtkd");
  CTKCOMPARE(widget->maxNumberOfColors(), 25);
  CTKCOMPARE(widget->numberOfLabels(), 2);
  }
}

//-----------------------------------------------------------------------------
int ctkVTKScalarBarWidgetEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Visualization/VTK/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  vtkSmartPointer<vtkScalarBarWidget> scalarBar =
    vtkSmartPointer<vtkScalarBarWidget>::New();

  ctkVTKScalarBarWidget widget;
  widget.setScalarBarWidget(scalarBar);
  widget.setScalarBarWidget(0);
  widget.setScalarBarWidget(scalarBar);

  etpWidget.addTestCase(&widget,
                        xmlDirectory + "ctkVTKScalarBarWidgetEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

