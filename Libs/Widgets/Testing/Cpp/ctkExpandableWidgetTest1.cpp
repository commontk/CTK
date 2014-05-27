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
#include <QListView>
#include <QStyle>
#include <QTableView>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>

#if (QT_VERSION < 0x50000)
#include <QPlastiqueStyle>
#endif

// CTK includes
#include "ctkExpandableWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkExpandableWidgetTest1(int argc, char * argv [] )
{
#if (QT_VERSION < 0x50000)
  QApplication::setStyle(new QPlastiqueStyle);
#endif
  QApplication app(argc, argv);

  QWidget topLevel;
  QHBoxLayout* topLevelLayout = new QHBoxLayout;
  topLevel.setLayout(topLevelLayout);

  QWidget frame;
  topLevelLayout->addWidget(&frame);

  ctkExpandableWidget resizableFrame1(&frame);
  QListView* listView = new QListView(&resizableFrame1);
  listView->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
  QVBoxLayout* resizableFrameLayout1 = new QVBoxLayout(&resizableFrame1);
  resizableFrameLayout1->setContentsMargins(0,0,0,0);
  resizableFrame1.setSizeGripMargins(QSize(2, 2));
  resizableFrame1.setLayout(resizableFrameLayout1);
  resizableFrameLayout1->addWidget(listView);
  resizableFrame1.setOrientations(Qt::Horizontal | Qt::Vertical);

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

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setContentsMargins(0, 0, 0, 0);
  vbox->addWidget(&resizableFrame1);
  vbox->addWidget(&resizableFrame);
  frame.setLayout(vbox);

  ctkExpandableWidget resizableFrame0(&topLevel);
  QTableView* tableView = new QTableView(&resizableFrame0);
  tableView->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
  QVBoxLayout* resizableFrameLayout0 = new QVBoxLayout(&resizableFrame0);
  resizableFrameLayout0->setContentsMargins(0,0,0,0);
  resizableFrame0.setSizeGripMargins(QSize(2, 2));
  resizableFrame0.setLayout(resizableFrameLayout0);
  resizableFrameLayout0->addWidget(tableView);
  resizableFrame0.setOrientations(Qt::Vertical);
  topLevelLayout->addWidget(&resizableFrame0);

  topLevel.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
