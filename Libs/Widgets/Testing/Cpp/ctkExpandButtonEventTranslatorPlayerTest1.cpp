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
#include <QHBoxLayout>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkExpandButton.h"
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
  QWidget* parentWidget = reinterpret_cast<QWidget*>(data);
  QList<ctkExpandButton*> widget = parentWidget->findChildren<ctkExpandButton*>();

  if(widget.count() != 0)
    {
    CTKCOMPARE(widget.at(0)->isChecked(), true);
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
  }
}

//-----------------------------------------------------------------------------
int ctkExpandButtonEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  QWidget* widget = new QWidget(0);

  // Expand button horizontal
  QHBoxLayout* hLayout = new QHBoxLayout(widget);
  ctkExpandButton* expandButton = new ctkExpandButton(widget);

  QPushButton* pushButton = new QPushButton("test");
  hLayout->addWidget(pushButton);
  pushButton->setVisible(false);
  QPushButton* pushButton2 = new QPushButton("foo");
  hLayout->addWidget(pushButton2);
  pushButton2->setVisible(false);
  hLayout->addWidget(expandButton);
  expandButton->setMirrorOnExpand(true);

  QObject::connect(expandButton, SIGNAL(clicked(bool)),
                   pushButton, SLOT(setVisible(bool)));
  QObject::connect(expandButton, SIGNAL(clicked(bool)),
                   pushButton2, SLOT(setVisible(bool)));

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkExpandButtonEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  QWidget* widget2 = new QWidget(0);

  ctkExpandButton* expandButton2 = new ctkExpandButton(widget2);
  QVBoxLayout* vLayout = new QVBoxLayout(widget2);
  QPushButton* pushButton3 = new QPushButton("test vertival");
  vLayout->addWidget(pushButton3);
  pushButton3->setVisible(false);
  QPushButton* pushButton4 = new QPushButton("foo vertical");
  vLayout->addWidget(pushButton4);
  pushButton4->setVisible(false);
  vLayout->addWidget(expandButton2);
  expandButton2->setMirrorOnExpand(true);
  expandButton2->setOrientation(Qt::Vertical);

  QObject::connect(expandButton2, SIGNAL(clicked(bool)),
                   pushButton3, SLOT(setVisible(bool)));
  QObject::connect(expandButton2, SIGNAL(clicked(bool)),
                   pushButton4, SLOT(setVisible(bool)));

  etpWidget.addTestCase(widget2,
                        xmlDirectory + "ctkExpandButtonEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

