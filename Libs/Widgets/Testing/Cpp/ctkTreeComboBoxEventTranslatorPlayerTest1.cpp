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
#include <QVBoxLayout>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkTreeComboBox.h"
#include "ctkTreeComboBoxEventTranslator.h"
#include "ctkTreeComboBoxEventPlayer.h"
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
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkTreeComboBox* widget = reinterpret_cast<ctkTreeComboBox*>(data);

  CTKCOMPARE(Spy1->count(), 1);
  CTKCOMPARE(widget->currentText(), "Test1.2");
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  QWidget* parentWidget = reinterpret_cast<QWidget*>(data);
  QList<ctkTreeComboBox*> widget = parentWidget->findChildren<ctkTreeComboBox*>();

  if(widget.count())
    {
    CTKCOMPARE(widget[0]->currentText(), "Test1.2");
    CTKCOMPARE(Spy2->count(), 3);
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
  }
}

//-----------------------------------------------------------------------------
int ctkTreeComboBoxEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventTranslator(new ctkTreeComboBoxEventTranslator);
  etpWidget.addWidgetEventPlayer(new ctkTreeComboBoxEventPlayer);

  // Test case 1
  ctkTreeComboBox* widget = new ctkTreeComboBox();
  QStandardItemModel model;
  model.appendRow(new QStandardItem("Test1"));
  model.item(0)->appendRow(new QStandardItem("Test1.1"));
  model.item(0)->appendRow(new QStandardItem("Test1.2"));
  model.item(0)->appendRow(new QStandardItem("Test1.3"));
  model.appendRow(new QStandardItem("Test2"));
  model.appendRow(new QStandardItem("Test3"));
  widget->setModel(&model);

  QSignalSpy spy1(widget, SIGNAL(currentIndexChanged(int)));
  Spy1 = &spy1;

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkTreeComboBoxEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  QWidget widget2(0);

  QVBoxLayout *layout= new QVBoxLayout;

  ctkTreeComboBox* combo = new ctkTreeComboBox();
  QStandardItemModel model2;
  model2.appendRow(new QStandardItem("Test1"));
  model2.item(0)->appendRow(new QStandardItem("Test1.1"));
  model2.item(0)->appendRow(new QStandardItem("Test1.2"));
  model2.item(0)->appendRow(new QStandardItem("Test1.3"));
  model2.appendRow(new QStandardItem("Test2"));
  model2.appendRow(new QStandardItem("Test3"));
  combo->setModel(&model2);
  layout->addWidget(combo);
  widget2.setLayout(layout);

  QSignalSpy spy2(combo, SIGNAL(currentIndexChanged(int)));
  Spy2 = &spy2;

  etpWidget.addTestCase(&widget2,
                        xmlDirectory + "ctkTreeComboBoxEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

