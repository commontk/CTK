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
#include <QSignalSpy>
#include <QTimer>

// CTK includes
#include "ctkAxesWidget.h"
#include "ctkAxesWidgetEventPlayer.h"
#include "ctkAxesWidgetEventTranslator.h"
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
  ctkAxesWidget* widget = reinterpret_cast<ctkAxesWidget*>(data);

  CTKCOMPARE(widget->currentAxis(), ctkAxesWidget::Left);
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  ctkAxesWidget* widget = reinterpret_cast<ctkAxesWidget*>(data);

  CTKCOMPARE(widget->currentAxis(), ctkAxesWidget::None);
  }
}

//-----------------------------------------------------------------------------
int ctkAxesWidgetEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventPlayer(new ctkAxesWidgetEventPlayer);
  etpWidget.addWidgetEventTranslator(new ctkAxesWidgetEventTranslator);

  // Test case 1 --- autoReset = false
  ctkAxesWidget* widget = new ctkAxesWidget();
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkAxesWidgetEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2 --- autoReset = true
  ctkAxesWidget* widget2 = new ctkAxesWidget();
  widget2->setAutoReset(true);
  etpWidget.addTestCase(widget2,
                        xmlDirectory + "ctkAxesWidgetEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState2);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

