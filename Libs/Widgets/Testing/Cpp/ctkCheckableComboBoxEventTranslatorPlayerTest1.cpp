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
#include <QSignalSpy>
#include <QTimer>
#include <QVBoxLayout>

// QtTesting includes
#include <pqTestUtility.h>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkCheckableComboBox.h"
#include "ctkCheckableComboBoxEventPlayer.h"
#include "ctkCheckableComboBoxEventTranslator.h"
#include "ctkEventTranslatorPlayerWidget.h"

// QtTesting includes
#include "pqTestUtility.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
QSignalSpy *Spy1;
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkCheckableComboBox* widget = reinterpret_cast<ctkCheckableComboBox*>(data);

  CTKCOMPARE(Spy1->count(), 15);
  CTKCOMPARE(widget->checkedIndexes().count(), 3);
  CTKCOMPARE(widget->noneChecked(), false);
//  CTKCOMPARE(widget->currentText(), "toto, titi, tutu");
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  QWidget* parentWidget = reinterpret_cast<QWidget*>(data);
  QList<ctkCheckableComboBox*> widget = parentWidget->findChildren<ctkCheckableComboBox*>();

  if(widget.count())
    {
    CTKCOMPARE(widget[0]->checkedIndexes().count(), 2);
    CTKCOMPARE(widget[0]->noneChecked(), false);
//    CTKCOMPARE(widget[0]->currentText(), "tata,titi");
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
  }
}

//-----------------------------------------------------------------------------
int ctkCheckableComboBoxEventTranslatorPlayerTest1(int argc, char * argv [])
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventTranslator(new ctkCheckableComboBoxEventTranslator);
  etpWidget.addWidgetEventPlayer(new ctkCheckableComboBoxEventPlayer);

  // Test case 1
  ctkCheckableComboBox* widget = new ctkCheckableComboBox();
  widget->addItem("toto");
  widget->addItem("tata");
  widget->addItem("titi");
  widget->addItem(widget->style()->standardIcon(QStyle::SP_FileIcon),"tutu");

  QSignalSpy spy1(widget, SIGNAL(checkedIndexesChanged()));
  Spy1 = &spy1;

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkCheckableComboBoxEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  QWidget widget2(0);
  ctkCheckableComboBox* comboBox2 = new ctkCheckableComboBox(&widget2);
  comboBox2->addItem("toto");
  comboBox2->addItem("tata");
  comboBox2->addItem("titi");
  comboBox2->addItem(comboBox2->style()->standardIcon(QStyle::SP_FileIcon),"tutu");

  etpWidget.addTestCase(&widget2,
                        xmlDirectory + "ctkCheckableComboBoxEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}
