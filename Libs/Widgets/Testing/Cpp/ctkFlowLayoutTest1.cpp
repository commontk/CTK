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
#include <QPushButton>
#include <QSignalSpy>
#include <QTimer>

// CTK includes
#include "ctkFlowLayout.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkFlowLayoutTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget widget1(0);
  widget1.setWindowTitle("1) Horizontal");
  ctkFlowLayout* flowLayout1 = new ctkFlowLayout(&widget1);
  flowLayout1->setAlignItems(false);
  
  flowLayout1->addWidget(new QPushButton("1 text text text text"));
  flowLayout1->addWidget(new QPushButton("2 text text text text text text text text"));
  flowLayout1->addWidget(new QPushButton("3 text"));
  flowLayout1->addWidget(new QPushButton("4 text text text text text text"));
  flowLayout1->addWidget(new QPushButton("5 text text"));
  flowLayout1->addWidget(new QPushButton("6 text text text text"));
  flowLayout1->addWidget(new QPushButton("7 text text text text text text text"));
  flowLayout1->addWidget(new QPushButton("8 text text text"));
  flowLayout1->addWidget(new QPushButton("9"));
  flowLayout1->addWidget(new QPushButton("10 text text text text text text text text text"));
 
  widget1.setLayout(flowLayout1);
  widget1.show();

  QWidget widget1justified(0);
  widget1justified.setWindowTitle("1) Horizontal, justified");
  ctkFlowLayout* flowLayout1justified = new ctkFlowLayout(&widget1justified);
  flowLayout1justified->setAlignItems(false);
  flowLayout1justified->setAlignment(Qt::AlignJustify);

  flowLayout1justified->addWidget(new QPushButton("1 text text text text"));
  flowLayout1justified->addWidget(new QPushButton("2 text text text text text text text text"));
  flowLayout1justified->addWidget(new QPushButton("3 text"));
  flowLayout1justified->addWidget(new QPushButton("4 text text text text text text"));
  flowLayout1justified->addWidget(new QPushButton("5 text text"));
  flowLayout1justified->addWidget(new QPushButton("6 text text text text"));
  flowLayout1justified->addWidget(new QPushButton("7 text text text text text text text"));
  flowLayout1justified->addWidget(new QPushButton("8 text text text"));
  flowLayout1justified->addWidget(new QPushButton("9"));
  flowLayout1justified->addWidget(new QPushButton("10 text text text text text text text text text"));

  widget1justified.setLayout(flowLayout1justified);
  widget1justified.show();

  QWidget widget2(0);
  widget2.setWindowTitle("2) Horizontal");
  ctkFlowLayout* flowLayout2 = new ctkFlowLayout;
  
  flowLayout2->addWidget(new QPushButton("one"));
  flowLayout2->addWidget(new QPushButton("two"));
  flowLayout2->addWidget(new QPushButton("three"));
  flowLayout2->addWidget(new QPushButton("four"));
  flowLayout2->addWidget(new QPushButton("five"));
  flowLayout2->addWidget(new QPushButton("six"));
  flowLayout2->addWidget(new QPushButton("seven"));
  //flowLayout2->setHorizontalSpacing(20);
  
  widget2.setLayout(flowLayout2);
  widget2.show();
  
  QWidget widget3(0);
  widget3.setWindowTitle("3) Vertical");
  ctkFlowLayout* flowLayout3 = new ctkFlowLayout(Qt::Vertical);
  
  flowLayout3->addWidget(new QPushButton("one"));
  flowLayout3->addWidget(new QPushButton("two"));
  flowLayout3->addWidget(new QPushButton("three"));
  flowLayout3->addWidget(new QPushButton("four"));
  flowLayout3->addWidget(new QPushButton("five"));
  flowLayout3->addWidget(new QPushButton("six"));
  flowLayout3->addWidget(new QPushButton("seven"));
  flowLayout3->setHorizontalSpacing(20);
  widget3.setLayout(flowLayout3);
  widget3.show();
  
  QWidget widget4(0);
  widget4.setWindowTitle("4) Horizontal -> Vertical");
  ctkFlowLayout* flowLayout4 = new ctkFlowLayout();
  
  flowLayout4->addWidget(new QPushButton("one"));
  flowLayout4->addWidget(new QPushButton("two"));
  flowLayout4->addWidget(new QPushButton("three"));
  flowLayout4->addWidget(new QPushButton("four"));
  flowLayout4->addWidget(new QPushButton("five"));
  flowLayout4->addWidget(new QPushButton("six"));
  flowLayout4->addWidget(new QPushButton("seven"));
  
  widget4.setLayout(flowLayout4);
  widget4.show();
  flowLayout4->setOrientation(Qt::Vertical);
  flowLayout4->setVerticalSpacing(0);
  flowLayout4->setHorizontalSpacing(0);

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
