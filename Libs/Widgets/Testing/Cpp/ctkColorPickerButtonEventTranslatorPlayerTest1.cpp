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
#include <QVBoxLayout>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkColorPickerButton.h"
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
  ctkColorPickerButton* widget = reinterpret_cast<ctkColorPickerButton*>(data);

  CTKCOMPARE(widget->color(), QColor(98,127,80));
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  QWidget* parentWidget = reinterpret_cast<QWidget*>(data);
  QList<ctkColorPickerButton*> widget = parentWidget->findChildren<ctkColorPickerButton*>();

  if(widget.count())
    {
    CTKCOMPARE(widget[0]->color(), QColor(255,85,0));
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
  QList<ctkColorPickerButton*> widget = parentWidget->findChildren<ctkColorPickerButton*>();

  if(widget.count())
    {
    CTKCOMPARE(widget[0]->color(), QColor(191,191,191));
    CTKCOMPARE(widget[1]->color(), QColor(0,0,0));
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
  }
}


//-----------------------------------------------------------------------------
int ctkColorPickerButtonEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  ctkColorPickerButton* widget = new ctkColorPickerButton("Select a color");

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkColorPickerButtonEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  QWidget widget2(0);
  ctkColorPickerButton* colorButton1 = new ctkColorPickerButton("Select a color");

  QVBoxLayout *layout= new QVBoxLayout;
  layout->addWidget(colorButton1);
  widget2.setLayout(layout);

  etpWidget.addTestCase(&widget2,
                        xmlDirectory + "ctkColorPickerButtonEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // Test case 3
  QWidget widget3(0);
  ctkColorPickerButton* colorButton2 = new ctkColorPickerButton("button1");
  ctkColorPickerButton* colorButton3 = new ctkColorPickerButton("button2");

  QVBoxLayout *layout2= new QVBoxLayout;
  layout2->addWidget(colorButton2);
  layout2->addWidget(colorButton3);
  widget3.setLayout(layout2);

  etpWidget.addTestCase(&widget3,
                        xmlDirectory + "ctkColorPickerButtonEventTranslatorPlayerTest3.xml",
                        &checkFinalWidgetState3);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

