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
#include <QMenuBar>
#include <QStandardItemModel>
#include <QSignalSpy>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include <ctkCallback.h>
#include <ctkConfig.h>
#include "ctkEventTranslatorPlayerWidget.h"
#include "ctkSetName.h"

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
QSignalSpy* Spy9;
QSignalSpy* Spy10;

//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  QMenuBar* widget = reinterpret_cast<QMenuBar*>(data);

  Q_UNUSED(widget);
  CTKCOMPARE(Spy1->count(), 1);
  CTKCOMPARE(Spy2->count(), 1);
  CTKCOMPARE(Spy3->count(), 1);
  CTKCOMPARE(Spy4->count(), 1);
  CTKCOMPARE(Spy5->count(), 1);
  CTKCOMPARE(Spy6->count(), 1);
  CTKCOMPARE(Spy7->count(), 1);
  CTKCOMPARE(Spy8->count(), 1);
  CTKCOMPARE(Spy9->count(), 1);
  CTKCOMPARE(Spy10->count(), 1);
  }
}

//-----------------------------------------------------------------------------
int ctkMenuEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  QMenuBar menuBar;
  QMenu menu("MainMenu");
  menu.setObjectName("MainMenu");
  QMenu subMenuFile("subMenuFile");
  QMenu* menuOpen = new QMenu("Open...");
  menuOpen->setObjectName("subMenuOpen");
  QAction* volume = new QAction("&Volume...", menuOpen);
  QAction* data = new QAction("&TData...", menuOpen);
  QAction* files = new QAction("&Files...", 0);
  menuOpen->addAction(volume);
  menuOpen->addAction(data);
  menuOpen->addAction(files);

  QAction* save = new QAction("&Save...", &subMenuFile);
  QAction* add = new QAction("&Add...", &subMenuFile);
  subMenuFile.setObjectName("subMenu1");
  subMenuFile.addMenu(menuOpen);
  subMenuFile.addAction(save);
  subMenuFile.addAction(add);

  QMenu subMenuTxxx("subMenuTxxx");
  subMenuTxxx.setObjectName("subMenu2");
  QAction* toto = new QAction("&Toto...", &subMenuTxxx);
  QAction* titi = new QAction("&Titi...", &subMenuTxxx);
  QAction* tutu = new QAction("&Tutu...", &subMenuTxxx);
  subMenuTxxx.addAction(toto);
  subMenuTxxx.addAction(titi);
  subMenuTxxx.addAction(tutu);

  menu.addMenu(&subMenuFile);
  menu.addMenu(&subMenuTxxx);

  QAction* foo = new QAction("&Foo...", &menu);
  QAction* exit = new QAction("&Exit...", &menu);
  menu.addAction(foo);
  menu.addAction(exit);

  menuBar.addMenu(&menu);

  // Create QSignal Spy
  QSignalSpy spy1(toto, SIGNAL(triggered()));
  QSignalSpy spy2(titi, SIGNAL(triggered()));
  QSignalSpy spy3(tutu, SIGNAL(triggered()));
  QSignalSpy spy4(volume, SIGNAL(triggered()));
  QSignalSpy spy5(data, SIGNAL(triggered()));
  QSignalSpy spy6(files, SIGNAL(triggered()));
  QSignalSpy spy7(save, SIGNAL(triggered()));
  QSignalSpy spy8(add, SIGNAL(triggered()));
  QSignalSpy spy9(foo, SIGNAL(triggered()));
  QSignalSpy spy10(exit, SIGNAL(triggered()));

  Spy1 = &spy1;
  Spy2 = &spy2;
  Spy3 = &spy3;
  Spy4 = &spy4;
  Spy5 = &spy5;
  Spy6 = &spy6;
  Spy7 = &spy7;
  Spy8 = &spy8;
  Spy9 = &spy9;
  Spy10 = &spy10;

  etpWidget.addTestCase(&menuBar,
                        xmlDirectory + "ctkMenuEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

