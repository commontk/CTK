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

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkMaterialPropertyWidget.h"
#include "ctkEventTranslatorPlayerWidget.h"
#include "ctkSetName.h"

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
  ctkMaterialPropertyWidget* widget = reinterpret_cast<ctkMaterialPropertyWidget*>(data);

  CTKCOMPARE(widget->opacity(), 1.00);
  CTKCOMPARE(widget->ambient(), 0.10);
  CTKCOMPARE(widget->diffuse(), 0.60);
  CTKCOMPARE(widget->specular(), 0.50);
  CTKCOMPARE(widget->backfaceCulling(), false);
  CTKCOMPARE(widget->color(), QColor(0,255,255));
  }
void checkFinalWidgetState2(void* data)
  {
  ctkMaterialPropertyWidget* widget = reinterpret_cast<ctkMaterialPropertyWidget*>(data);

  CTKCOMPARE(widget->opacity(), 1.00);
  CTKCOMPARE(widget->ambient(), 0.10);
  CTKCOMPARE(widget->diffuse(), 0.60);
  CTKCOMPARE(widget->specular(), 0.50);
  CTKCOMPARE(widget->backfaceCulling(), false);
  CTKCOMPARE(widget->color(), QColor(255,255,255));
  }
}

//-----------------------------------------------------------------------------
int ctkMaterialPropertyWidgetEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  ctkMaterialPropertyWidget* widget = new ctkMaterialPropertyWidget() << ctkSetName("MaterialProperty1");
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkMaterialPropertyWidgetEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  ctkMaterialPropertyWidget* widget2 = new ctkMaterialPropertyWidget() << ctkSetName("MaterialProperty2");
  etpWidget.addTestCase(widget2,
                        xmlDirectory + "ctkMaterialPropertyWidgetEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

