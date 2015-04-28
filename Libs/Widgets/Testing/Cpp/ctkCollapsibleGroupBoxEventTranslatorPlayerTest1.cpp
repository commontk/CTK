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
#include <QCheckBox>
#include <QDebug>
#include <QRadioButton>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkCollapsibleGroupBox.h"
#include "ctkEventTranslatorPlayerWidget.h"

// QtTesting includes
#include "pqTestUtility.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
QSignalSpy *Spy1;
QSignalSpy *Spy2;
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  QWidget* parentWidget = reinterpret_cast<QWidget*>(data);
  QList<ctkCollapsibleGroupBox*> widget = parentWidget->findChildren<ctkCollapsibleGroupBox*>();

  if(widget.count())
    {
    CTKCOMPARE(Spy1->count(), 2);
    CTKCOMPARE(Spy2->count(), 4);
    CTKCOMPARE(widget[0]->collapsed(), false);
    CTKCOMPARE(widget[1]->collapsed(), true);
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
  }
}

//-----------------------------------------------------------------------------
int ctkCollapsibleGroupBoxEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  QWidget widget(0);
  ctkCollapsibleGroupBox* groupBox = new ctkCollapsibleGroupBox(QObject::tr("GroupBox"));
  QRadioButton *radio1 = new QRadioButton(QObject::tr("&Radio button 1"));
  QRadioButton *radio2 = new QRadioButton(QObject::tr("R&adio button 2"));
  QRadioButton *radio3 = new QRadioButton(QObject::tr("Ra&dio button 3"));
  ctkCollapsibleGroupBox* hiddenGroupBox = new ctkCollapsibleGroupBox;
  hiddenGroupBox->setTitle("Advanced...");

  radio1->setChecked(true);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(radio1);
  vbox->addWidget(radio2);
  vbox->addWidget(radio3);
  vbox->addWidget(hiddenGroupBox);
  vbox->addStretch(1);
  groupBox->setLayout(vbox);


  QCheckBox* checkBox = new QCheckBox("Check box");
  QVBoxLayout *vbox2 = new QVBoxLayout;
  vbox2->addWidget(checkBox);
  hiddenGroupBox->setLayout(vbox2);
  hiddenGroupBox->setCollapsed(true);

  QVBoxLayout* topLevelVBox = new QVBoxLayout;
  topLevelVBox->addWidget(groupBox);
  widget.setLayout(topLevelVBox);

  QSignalSpy spy1(groupBox, SIGNAL(clicked()));
  QSignalSpy spy2(hiddenGroupBox, SIGNAL(clicked()));

  Spy1 = &spy1;
  Spy2 = &spy2;

  etpWidget.addTestCase(&widget,
                        xmlDirectory + "ctkCollapsibleGroupBoxEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

