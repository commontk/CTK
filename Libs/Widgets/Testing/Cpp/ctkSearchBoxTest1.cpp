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
#include <QPalette>
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkSearchBox.h"

// STD includes
#include <cstdlib>
#include <iostream>

// ------------------------------------------------------------------------------
int ctkSearchBoxTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QPalette p;
  p.setColor(QPalette::ColorRole::Window, Qt::gray);
  p.setColor(QPalette::ColorRole::Base, Qt::gray);

  ctkSearchBox search;
  search.setShowSearchIcon(true);
  search.setPalette(p);
  search.show();

  QWidget topLevel;
  ctkSearchBox search3;
  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(&search3);
  topLevel.setLayout(layout);
  topLevel.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

