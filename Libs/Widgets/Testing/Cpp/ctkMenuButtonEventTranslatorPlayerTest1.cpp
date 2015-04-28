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
#include <QMenu>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkMenuButton.h"
#include "ctkEventTranslatorPlayerWidget.h"

// QtTesting includes
#include "pqTestUtility.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
QSignalSpy* Spy1;
QSignalSpy* Spy2;
QSignalSpy* Spy3;
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkMenuButton* widget = reinterpret_cast<ctkMenuButton*>(data);
  if(!widget)
    {
    QApplication::exit(EXIT_FAILURE);
    }

  CTKCOMPARE(Spy1->count(), 1);
  CTKCOMPARE(Spy2->count(), 1);
  CTKCOMPARE(Spy3->count(), 1);
  }
}

//-----------------------------------------------------------------------------
int ctkMenuButtonEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  QWidget* widget = new QWidget(0);
  ctkMenuButton* menuButton = new ctkMenuButton("ctkMenuButton", widget);

  QMenu* menu = new QMenu("menu", menuButton);
  QAction* action1 = new QAction("extra choice 1", menu);
  QAction* action2 = new QAction("extra choice 2", menu);
  QAction* action3 = new QAction("extra choice 3", menu);
  menu->addAction(action1);
  menu->addAction(action2);
  menu->addAction(action3);

  QSignalSpy spy1(action1, SIGNAL(triggered()));
  QSignalSpy spy2(action2, SIGNAL(triggered()));
  QSignalSpy spy3(action3, SIGNAL(triggered()));

  Spy1 = &spy1;
  Spy2 = &spy2;
  Spy3 = &spy3;

  menuButton->setMenu(menu);
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkMenuButtonEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

