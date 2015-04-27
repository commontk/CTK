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
#include "ctkRangeSlider.h"
#include <ctkCallback.h>
#include <ctkConfig.h>
#include "ctkEventTranslatorPlayerWidget.h"

// QtTesting includes
#include "pqTestUtility.h"
#include <ctkRangeSliderEventPlayer.h>
#include <ctkRangeSliderEventTranslator.h>
#include "ctkSetName.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkRangeSlider* widget = reinterpret_cast<ctkRangeSlider*>(data);

  CTKCOMPARE(widget->maximumValue(), 100);
  CTKCOMPARE(widget->minimumValue(), -261);
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  ctkRangeSlider* widget = reinterpret_cast<ctkRangeSlider*>(data);

  CTKCOMPARE(widget->maximumValue(), 77);
  CTKCOMPARE(widget->minimumValue(), 14);
  }

}
//-----------------------------------------------------------------------------
int ctkRangeSliderEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventPlayer(new ctkRangeSliderEventPlayer);
  etpWidget.addWidgetEventTranslator(new ctkRangeSliderEventTranslator);

  // Test case 1
  ctkRangeSlider* widget = new ctkRangeSlider() << ctkSetName("RangeSlider1");
  widget->setSingleStep(5);
  widget->setRange(-500, 250);
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkRangeSliderEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 1
  ctkRangeSlider* widget2 = new ctkRangeSlider() << ctkSetName("RangeSlider2");
  widget2->setOrientation(Qt::Horizontal);
  etpWidget.addTestCase(widget2,
                        xmlDirectory + "ctkRangeSliderEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}
