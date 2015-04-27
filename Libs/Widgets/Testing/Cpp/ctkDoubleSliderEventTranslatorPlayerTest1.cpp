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
#include "ctkDoubleSlider.h"
#include <ctkCallback.h>
#include <ctkConfig.h>
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
  ctkDoubleSlider* widget = reinterpret_cast<ctkDoubleSlider*>(data);

  CTKCOMPARE(widget->value(), 60.00);
  }
void checkFinalWidgetState3(void* data)
  {
  ctkDoubleSlider* widget = reinterpret_cast<ctkDoubleSlider*>(data);

  CTKCOMPARE(widget->value(), -27.82);
  }
}

//-----------------------------------------------------------------------------
int ctkDoubleSliderEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  ctkDoubleSlider* widget = new ctkDoubleSlider();
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkDoubleSliderEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  ctkDoubleSlider* widget2 = new ctkDoubleSlider();
  widget2->setOrientation(Qt::Horizontal);
  etpWidget.addTestCase(widget2,
                        xmlDirectory + "ctkDoubleSliderEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 3
  ctkDoubleSlider* widget3 = new ctkDoubleSlider();
  widget3->setOrientation(Qt::Horizontal);
  widget3->setRange(-52.89,10);
  widget3->setValue(-40.28);
  widget3->setSingleStep(0.89);
  etpWidget.addTestCase(widget3,
                        xmlDirectory + "ctkDoubleSliderEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState3);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}
