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
#include "ctkFontButton.h"
#include "ctkFontButtonEventPlayer.h"
#include "ctkFontButtonEventTranslator.h"
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
  ctkFontButton* widget = reinterpret_cast<ctkFontButton*>(data);

  CTKCOMPARE(widget->font().style(), QFont::StyleOblique);
  CTKCOMPARE(widget->font().pointSize(), 9);
  CTKCOMPARE(widget->font().underline(), true);
  CTKCOMPARE(widget->font().strikeOut(), false);
  CTKCOMPARE(widget->font().family(), QString("Lohit Bengali"));
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  ctkFontButton* widget = reinterpret_cast<ctkFontButton*>(data);

  CTKCOMPARE(widget->font().style(), QFont::StyleItalic);
  CTKCOMPARE(widget->font().pointSize(), 36);
  CTKCOMPARE(widget->font().underline(), true);
  CTKCOMPARE(widget->font().strikeOut(), true);
  CTKCOMPARE(widget->font().family(), QString("Ubuntu"));
  }
}

//-----------------------------------------------------------------------------
int ctkFontButtonEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventPlayer(new ctkFontButtonEventPlayer);
  etpWidget.addWidgetEventTranslator(new ctkFontButtonEventTranslator);

  // Test case 1
  ctkFontButton* widget = new ctkFontButton();
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkFontButtonEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  ctkFontButton* widget2 = new ctkFontButton();
  etpWidget.addTestCase(widget2,
                        xmlDirectory + "ctkFontButtonEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

