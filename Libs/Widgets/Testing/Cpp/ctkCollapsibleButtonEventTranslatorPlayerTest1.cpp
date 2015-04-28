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
#include <QPushButton>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

// QtTesting includes
#include <pqTestUtility.h>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkCollapsibleButton.h"
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
  ctkCollapsibleButton* widget = reinterpret_cast<ctkCollapsibleButton*>(data);

  CTKCOMPARE(widget->collapsed(), true);
  CTKCOMPARE(widget->collapsedHeight(), 10);

  QApplication::exit(EXIT_SUCCESS);
  }
}

//-----------------------------------------------------------------------------
int ctkCollapsibleButtonEventTranslatorPlayerTest1(int argc, char * argv [])
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  ctkCollapsibleButton* widget = new ctkCollapsibleButton();
  widget->setText("top button");
  QPushButton * button= new QPushButton(QObject::tr("Button"));
  ctkCollapsibleButton *collapsibleButton2 = new ctkCollapsibleButton(QObject::tr("Nested Collapsible Button"));
  ctkCollapsibleButton *collapsibleButton3 = new ctkCollapsibleButton(QObject::tr("CollapsibleButton"));
  collapsibleButton3->setIcon(collapsibleButton3->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  QPushButton * button2 = new QPushButton(QObject::tr("Nested PushButton"));

  QVBoxLayout *nestedBox = new QVBoxLayout;
  nestedBox->addWidget(button2);
  collapsibleButton3->setLayout(nestedBox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(button);
  vbox->addWidget(collapsibleButton2);
  vbox->addWidget(collapsibleButton3);
  widget->setLayout(vbox);

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkCollapsibleButtonEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}
