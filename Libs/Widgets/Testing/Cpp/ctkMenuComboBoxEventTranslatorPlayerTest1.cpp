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
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkMenuComboBox.h"
#include "ctkMenuComboBoxEventPlayer.h"
#include "ctkMenuComboBoxEventTranslator.h"
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
QSignalSpy* Spy4;

QSignalSpy* Spy5;
QSignalSpy* Spy6;
QSignalSpy* Spy7;
QSignalSpy* Spy8;
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkMenuComboBox* widget = reinterpret_cast<ctkMenuComboBox*>(data);
  Q_UNUSED(widget);

  CTKCOMPARE(Spy1->count(), 2);
  CTKCOMPARE(Spy2->count(), 2);
  CTKCOMPARE(Spy3->count(), 2);
  CTKCOMPARE(Spy4->count(), 2);
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  ctkMenuComboBox* widget = reinterpret_cast<ctkMenuComboBox*>(data);
  Q_UNUSED(widget);

  CTKCOMPARE(Spy5->count(), 1);
  CTKCOMPARE(Spy6->count(), 1);
  CTKCOMPARE(Spy7->count(), 1);
  CTKCOMPARE(Spy8->count(), 1);
  }
}

//-----------------------------------------------------------------------------
int ctkMenuComboBoxEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventPlayer(new ctkMenuComboBoxEventPlayer);
  etpWidget.addWidgetEventTranslator(new ctkMenuComboBoxEventTranslator);

  // Test case 1 - default behavior - Test Completer
  QWidget* widget = new QWidget(0);
  QMenu* menu = new QMenu("File", widget);
  QAction* action1 = new QAction("first", widget);
  menu->addAction(action1);
  QMenu* wizards = new QMenu("Wizards", menu);
  menu->addMenu(wizards);
  QMenu*informatics = new QMenu("Informatics", menu);
  menu->addMenu(informatics);

  QAction* action2 = new QAction("extra choice 1", widget);
  QAction* action3 = new QAction("extra choice 2", widget);
  QAction* action4 = new QAction("extra choice 3", widget);
  menu->addAction(action2);
  menu->addAction(wizards->menuAction());
  menu->addAction(informatics->menuAction());
  wizards->addAction(action3);
  informatics->addAction(action4);

  QSignalSpy spy1(action1, SIGNAL(triggered()));
  QSignalSpy spy2(action2, SIGNAL(triggered()));
  QSignalSpy spy3(action3, SIGNAL(triggered()));
  QSignalSpy spy4(action4, SIGNAL(triggered()));

  Spy1 = &spy1;
  Spy2 = &spy2;
  Spy3 = &spy3;
  Spy4 = &spy4;

  ctkMenuComboBox* menuComboBox = new ctkMenuComboBox(widget);
  menuComboBox->setMenu(menu);

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkMenuComboBoxEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2 - default behavior - test Menu
  QWidget* widget2 = new QWidget(0);
  ctkMenuComboBox* menuComboBox2 = new ctkMenuComboBox(widget2);
  QMenu* menu2 = new QMenu("File", menuComboBox2);
  QAction* action5 = new QAction("first", menu2);
  menu2->addAction(action5);
  QMenu* wizards2 = new QMenu("Wizards", menu2);
  menu2->addMenu(wizards2);
  QMenu*informatics2 = new QMenu("Informatics", menu2);
  menu2->addMenu(informatics2);

  QAction* action6 = new QAction("extra choice 1", menu2);
  QAction* action7 = new QAction("extra choice 2", menu2);
  QAction* action8 = new QAction("extra choice 3", menu2);
  menu2->addAction(action6);
//  menu2->addAction(wizards2->menuAction());
//  menu2->addAction(informatics2->menuAction());
  wizards2->addAction(action7);
  informatics2->addAction(action8);

  QSignalSpy spy5(action5, SIGNAL(triggered()));
  QSignalSpy spy6(action6, SIGNAL(triggered()));
  QSignalSpy spy7(action7, SIGNAL(triggered()));
  QSignalSpy spy8(action8, SIGNAL(triggered()));

  Spy5 = &spy5;
  Spy6 = &spy6;
  Spy7 = &spy7;
  Spy8 = &spy8;


  menuComboBox2->setMenu(menu2);

  etpWidget.addTestCase(widget2,
                        xmlDirectory + "ctkMenuComboBoxEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

