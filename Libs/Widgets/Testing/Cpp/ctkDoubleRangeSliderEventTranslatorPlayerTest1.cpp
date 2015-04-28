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

// QtTesting includes
#include <pqTestUtility.h>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkDoubleRangeSlider.h"
#include "ctkDoubleRangeSliderEventPlayer.h"
#include "ctkDoubleRangeSliderEventTranslator.h"
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
  ctkDoubleRangeSlider* widget = reinterpret_cast<ctkDoubleRangeSlider*>(data);

  CTKCOMPARE(widget->maximumValue(), 76.00);
  CTKCOMPARE(widget->minimumValue(), 19.00);
  }
void checkFinalWidgetState2(void* data)
  {
  ctkDoubleRangeSlider* widget = reinterpret_cast<ctkDoubleRangeSlider*>(data);

  CTKCOMPARE(widget->maximumValue(), 78.00);
  CTKCOMPARE(widget->minimumValue(), 17.00);
  }
void checkFinalWidgetState3(void* data)
  {
  ctkDoubleRangeSlider* widget = reinterpret_cast<ctkDoubleRangeSlider*>(data);

  CTKCOMPARE(widget->maximumValue(), -10.47);
  CTKCOMPARE(widget->minimumValue(), -49.79);
  }
}

//-----------------------------------------------------------------------------
int ctkDoubleRangeSliderEventTranslatorPlayerTest1(int argc, char * argv [])
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventTranslator(new ctkDoubleRangeSliderEventTranslator);
  etpWidget.addWidgetEventPlayer(new ctkDoubleRangeSliderEventPlayer);


  // Test case 1
  ctkDoubleRangeSlider* widget = new ctkDoubleRangeSlider() << ctkSetName("doubleRangeSlider1");
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkDoubleRangeSliderEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  ctkDoubleRangeSlider* widget2 = new ctkDoubleRangeSlider() << ctkSetName("doubleRangeSlider2");
  widget2->setOrientation(Qt::Horizontal);
  etpWidget.addTestCase(widget2,
                        xmlDirectory + "ctkDoubleRangeSliderEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // Test case 3
  ctkDoubleRangeSlider* widget3 = new ctkDoubleRangeSlider();
  widget3->setOrientation(Qt::Horizontal);
  widget3->setRange(-52.89,10);
  widget3->setMinimumPosition(-40.00);
  widget3->setSingleStep(0.89);
  etpWidget.addTestCase(widget3,
                        xmlDirectory + "ctkDoubleRangeSliderEventTranslatorPlayerTest3.xml",
                        &checkFinalWidgetState3);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}
