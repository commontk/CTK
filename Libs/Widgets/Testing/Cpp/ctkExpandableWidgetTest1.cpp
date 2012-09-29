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
#include <QGroupBox>
#include <QLabel>
#include <QStyle>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>
#include <QPlastiqueStyle>

// CTK includes
#include "ctkExpandableWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkExpandableWidgetTest1(int argc, char * argv [] )
{
  QApplication::setStyle(new QPlastiqueStyle);
  QApplication app(argc, argv);

  QWidget topLevel;
  QHBoxLayout* topLevelLayout = new QHBoxLayout;
  topLevel.setLayout(topLevelLayout);

  QWidget frame(&topLevel);
  //ctkExpandableWidget frame(&topLevel);
  topLevelLayout->addWidget(&frame);

  QGroupBox groupBox(&topLevel);
  topLevelLayout->addWidget(&groupBox);

  QLabel* label0 = new QLabel("should be invisible", &frame);
  QLabel* label1 = new QLabel("should be visible", &frame);
  QLabel* label2 = new QLabel("should be invisible", &frame);
  QLabel* label3 = new QLabel("should be visible", &frame);
  QLabel* label4 = new QLabel("should be invisible", &frame);
  QLabel* label5 = new QLabel("should be visible", &frame);
  QLabel* label6 = new QLabel("should be visible", &frame);
  QLabel* label7 = new QLabel("should be visible", &frame);
  QLabel* label8 = new QLabel("should be invisible", &frame);

  ctkExpandableWidget resizableFrame(&frame);
  resizableFrame.setSizeGripInside(false);
  QTreeView* treeView = new QTreeView(&resizableFrame);
  treeView->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
  QVBoxLayout* resizableFrameLayout = new QVBoxLayout(&resizableFrame);
  resizableFrameLayout->setContentsMargins(0,0,0,0);
  //resizableFrame.setSizeGripMargins(QSize(2, 2));
  resizableFrame.setLayout(resizableFrameLayout);
  resizableFrameLayout->addWidget(treeView);
  resizableFrame.setOrientations(Qt::Horizontal);

  label8->setVisible(false);
  label2->setVisible(false);
  label4->setVisible(false);
  label8->setVisible(false);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setContentsMargins(0, 0, 0, 0);
  vbox->addWidget(label0);
  vbox->addWidget(label1);
  vbox->addWidget(label2);
  vbox->addWidget(label3);
  vbox->addWidget(label4);
  vbox->addWidget(label5);
  vbox->addWidget(label6);
  vbox->addWidget(label7);
  vbox->addWidget(label8);
  vbox->addWidget(&resizableFrame);
  frame.setLayout(vbox);

  topLevel.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
