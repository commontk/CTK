/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>

// CTK includes
#include "ctkPopupWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkPopupWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QPushButton base("Top level push button");
  
  ctkPopupWidget popup;
  QPushButton popupContent("popup");
  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(&popupContent);
  popup.setLayout(layout);

  popup.setBaseWidget(&base);
  //QObject::connect(&base, SIGNAL(clicked()), &popup, SLOT(showPopup()));
  base.show();

  QWidget topLevel;
  QPushButton button1("button");
  QVBoxLayout* vlayout = new QVBoxLayout;
  vlayout->addWidget(&button1);
  topLevel.setLayout(vlayout);
  topLevel.show();

  ctkPopupWidget popup2;
  popup2.setFrameStyle(QFrame::Box);
  popup2.setLineWidth(2);
  QPushButton popup2Content("popup2");
  QVBoxLayout* p2layout = new QVBoxLayout;
  p2layout->setContentsMargins(0,0,0,0);
  p2layout->addWidget(&popup2Content);
  popup2.setLayout(p2layout);
  popup2.setBaseWidget(&button1);
  
  
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}

