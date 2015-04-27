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
#include <QStandardItem>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

// QtTesting includes
#include <pqTestUtility.h>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkCheckablePushButton.h"
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
  ctkCheckablePushButton* widget = reinterpret_cast<ctkCheckablePushButton*>(data);

  CTKCOMPARE(widget->isChecked(), true);
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  QWidget* parentWidget = reinterpret_cast<QWidget*>(data);
  QList<ctkCheckablePushButton*> widget = parentWidget->findChildren<ctkCheckablePushButton*>();

  if(widget.count())
    {
    CTKCOMPARE(widget[0]->isChecked(), true);
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState3(void* data)
  {
  QWidget* parentWidget = reinterpret_cast<QWidget*>(data);
  QList<ctkCheckablePushButton*> widget = parentWidget->findChildren<ctkCheckablePushButton*>();

  if(widget.count())
    {
    CTKCOMPARE(widget[0]->isChecked(), true);
    CTKCOMPARE(widget[1]->isChecked(), false);
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
  }
}

//-----------------------------------------------------------------------------
int ctkCheckablePushButtonEventTranslatorPlayerTest1(int argc, char * argv [])
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  ctkCheckablePushButton* widget = new ctkCheckablePushButton();
  widget->setText("Foo");
  widget->setCheckable(true);
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkCheckablePushButtonEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  QWidget widget2(0);
  ctkCheckablePushButton button1(QObject::tr("Button1"));

  button1.setCheckable(true);

  QVBoxLayout *layout= new QVBoxLayout;
  layout->addWidget(&button1);
  widget2.setLayout(layout);

  etpWidget.addTestCase(&widget2,
                        xmlDirectory + "ctkCheckablePushButtonEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // Test case 3
  QWidget widget3(0);
  ctkCheckablePushButton button2(QObject::tr("Button1"));
  ctkCheckablePushButton button3(QObject::tr("Button2"));

  button2.setCheckable(true);
  button3.setCheckable(true);

  QVBoxLayout *layout2= new QVBoxLayout;
  layout2->addWidget(&button2);
  layout2->addWidget(&button3);
  widget3.setLayout(layout2);

  etpWidget.addTestCase(&widget3,
                        xmlDirectory + "ctkCheckablePushButtonEventTranslatorPlayerTest3.xml",
                        &checkFinalWidgetState3);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}
