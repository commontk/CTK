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
#include <QComboBox>
#include <QDebug>
#include <QIcon>
#include <QLineEdit>
#include <QMenu>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

// CTK includes
#include "ctkMenuComboBox.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkMenuComboBoxTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  QIcon plus(":Icons/plus.png");

  QMenu* file = new QMenu("File");
  file->addAction("first");
  QMenu* wizards = new QMenu("Wizards");
  file->addMenu(wizards);

  QMenu*informatics = new QMenu("Informatics");
  file->addMenu(informatics);

  QWidget topLevelWidget(0);

  /*ctkMenuComboBox* Menu = new ctkMenuComboBox();
  Menu->setMenu(file);
  Menu->setMinimumWidth(200);
  //Menu->setMinimumWidthComboBox(150);
  //Menu->setMinimumWidth(150);
  Menu->setEditableBehavior(ctkMenuComboBox::EditableOnFocus);
  Menu->show();*/

  ctkMenuComboBox* Menu2 = new ctkMenuComboBox(0);
  Menu2->setMenu(file);
  Menu2->setDefaultText("Search");
  Menu2->setAutoFillBackground(true);
  Menu2->setMinimumContentsLength(25);
  Menu2->setEditableBehavior(ctkMenuComboBox::EditableOnFocus);
  //Menu2->show();

/*
  ctkMenuComboBox* Menu3 = new ctkMenuComboBox();
  Menu3->setMenu(file);
  //Menu3->setMinimumWidth(150);
  //Menu3->setMinimumWidthComboBox(150);
  Menu3->setEditableBehavior(ctkMenuComboBox::NotEditable);
*/
  QVBoxLayout* layout = new QVBoxLayout;
  QToolBar bar;
  //QWidget bar;
  //QHBoxLayout* barLayout = new QHBoxLayout(&bar);

  layout->addWidget(&bar);
  bar.addWidget(Menu2);
  //bar.addWidget(Menu);
  //layout->addWidget(Menu3);

  file->addAction(plus, "Add ...");
  file->addAction("Saveeeeeeeeeeeeeeeeeeeeeee ...");
  wizards->addAction("tutu");
  wizards->addMenu(informatics);
  informatics->addAction("ddd");

  QAction* actionEnd = wizards->addAction("Quit");

  actionEnd->setShortcut(QKeySequence("Ctrl+Q"));

  QObject::connect(actionEnd, SIGNAL(triggered()), qApp, SLOT(quit()));

  topLevelWidget.setLayout(layout);
  topLevelWidget.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

