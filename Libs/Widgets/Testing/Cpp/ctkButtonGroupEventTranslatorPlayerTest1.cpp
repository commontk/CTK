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
#include <QPushButton>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkButtonGroup.h"
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
  QWidget* parentWidget = reinterpret_cast<QWidget*>(data);
  QList<ctkButtonGroup*> widget = parentWidget->findChildren<ctkButtonGroup*>();

  if(widget.count())
    {
    CTKCOMPARE(widget[0]->checkedButton()->text(), QString("button 1"));
    CTKCOMPARE(Spy1->count(), 3);
    CTKCOMPARE(Spy2->count(), 2);
    CTKCOMPARE(Spy3->count(), 4);
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
  }
}

//-----------------------------------------------------------------------------
int ctkButtonGroupEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  QWidget widget(0);

  QVBoxLayout* layout = new QVBoxLayout(&widget);

  QPushButton* button1 = new QPushButton("button 1", &widget);
  QPushButton* button2 = new QPushButton("button 2", &widget);
  QPushButton* button3 = new QPushButton("button 3", &widget);

  layout->addWidget(button1);
  layout->addWidget(button2);
  layout->addWidget(button3);

  widget.setLayout(layout);

  button1->setCheckable(true);
  button2->setCheckable(true);
  button3->setCheckable(false);

  ctkButtonGroup* buttonGroup = new ctkButtonGroup(&widget);
  buttonGroup->addButton(button1);
  buttonGroup->addButton(button2);
  buttonGroup->addButton(button3);

  QSignalSpy spy1(button1, SIGNAL(clicked()));
  QSignalSpy spy2(button2, SIGNAL(clicked()));
  QSignalSpy spy3(button3, SIGNAL(clicked()));

  Spy1 = &spy1;
  Spy2 = &spy2;
  Spy3 = &spy3;

  etpWidget.addTestCase(&widget,
                        xmlDirectory + "ctkButtonGroupEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

