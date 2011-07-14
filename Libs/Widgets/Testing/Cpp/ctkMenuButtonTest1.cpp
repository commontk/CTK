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
#include <QVBoxLayout>
#include <QMenu>
#include <QTimer>

// CTK includes
#include "ctkMenuButton.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkMenuButtonTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget topLevelWidget(0);
  QMenu* menu = new QMenu("menu");
  menu->addAction("extra choice 1");
  menu->addAction("extra choice 2");
  menu->addAction("extra choice 3");

  QPushButton button1("QPushButton with no menu");
  QPushButton button2("QPushButton with menu");
  button2.setMenu(menu);  
  ctkMenuButton button3("ctkMenuButton with no menu");
  ctkMenuButton button4("ctkMenuButton with menu");  
  button4.setMenu(menu);
  
  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(&button1);
  layout->addWidget(&button2);
  layout->addWidget(&button3);
  layout->addWidget(&button4);
  
  topLevelWidget.setLayout(layout);
  topLevelWidget.show();
  topLevelWidget.resize(200, 100);

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

