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
#include "ctkRangeWidget.h"
#include <ctkDoubleRangeSliderEventPlayer.h>
#include <ctkDoubleRangeSliderEventTranslator.h>
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
  ctkRangeWidget* widget = reinterpret_cast<ctkRangeWidget*>(data);

  CTKCOMPARE(widget->maximumValue(), 72.00);
  CTKCOMPARE(widget->minimumValue(), 34.00);
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  ctkRangeWidget* widget = reinterpret_cast<ctkRangeWidget*>(data);

  CTKCOMPARE(widget->maximumValue(), -9.6);
  CTKCOMPARE(widget->minimumValue(), -35.00);
  }
}

//-----------------------------------------------------------------------------
int ctkRangeWidgetEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventPlayer(new ctkDoubleRangeSliderEventPlayer);
  etpWidget.addWidgetEventTranslator(new ctkDoubleRangeSliderEventTranslator);

  // Test case 1
  ctkRangeWidget* widget = new ctkRangeWidget() << ctkSetName("RangeWidget1");
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkRangeWidgetEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  ctkRangeWidget* widget2 = new ctkRangeWidget() << ctkSetName("RangeWidget2");
  widget2->setRange(-50.38,5.4);
  widget2->setSingleStep(5.00);
  etpWidget.addTestCase(widget2,
                        xmlDirectory + "ctkRangeWidgetEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

