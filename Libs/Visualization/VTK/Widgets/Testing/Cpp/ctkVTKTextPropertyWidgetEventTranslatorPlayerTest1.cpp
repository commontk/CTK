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
#include "ctkVTKTextPropertyWidget.h"
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
  ctkVTKTextPropertyWidget* widget = reinterpret_cast<ctkVTKTextPropertyWidget*>(data);

  CTKCOMPARE(widget->isBold(), true);
  CTKCOMPARE(widget->isItalic(), true);
  CTKCOMPARE(widget->hasShadow(), false);
  CTKCOMPARE(widget->text(), "Test");
}
}

//-----------------------------------------------------------------------------
int ctkVTKTextPropertyWidgetEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Visualization/VTK/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  ctkVTKTextPropertyWidget* widget = new ctkVTKTextPropertyWidget(0);
  widget->setEnabled(true);
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkVTKTextPropertyWidgetEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (!app.arguments().contains("-I"))
  {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
  }

  etpWidget.show();
  return app.exec();
}
