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
#include <QCheckBox>
#include <QVBoxLayout>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkCheckablePushButton.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckablePushButtonTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget topLevel;
  ctkCheckablePushButton button1(QObject::tr("My very long text on button"));
  ctkCheckablePushButton button2(QObject::tr("Button2"));
  ctkCheckablePushButton button3(QObject::tr("Button3"));
  ctkCheckablePushButton button4(QObject::tr("Button4"));
  ctkCheckablePushButton button5(QObject::tr("Button5"));
  ctkCheckablePushButton button6(QObject::tr("Button6"));
  ctkCheckablePushButton button7(QObject::tr("Checkable PushButton"));

  QVBoxLayout *layout= new QVBoxLayout;
  layout->addWidget(&button1);
  layout->addWidget(&button2);
  layout->addWidget(&button3);
  layout->addWidget(&button4);
  layout->addWidget(&button5);
  layout->addWidget(&button6);
  layout->addWidget(&button7);
  topLevel.setLayout(layout);

  topLevel.show();
  button1.setButtonTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

  if (button1.buttonTextAlignment() != (Qt::AlignRight | Qt::AlignVCenter))
    {
    std::cerr << "ctkCheckablePushButton::setButtonTextAlignment failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  button2.setIndicatorAlignment(Qt::AlignRight);

  if (button2.indicatorAlignment() != Qt::AlignRight)
    {
    std::cerr << "ctkCheckablePushButton::setIndicatorAlignment failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  button3.setButtonTextAlignment(Qt::AlignCenter);
  button3.setIndicatorAlignment(Qt::AlignCenter);
  button3.setCheckable(true);
  
  button4.setCheckable(true);
  button4.toggle();
  
  button5.setButtonTextAlignment(Qt::AlignCenter);
  button5.setIndicatorAlignment(Qt::AlignRight);
  
  button6.setIndicatorAlignment(Qt::AlignTop);
  button7.setButtonTextAlignment(Qt::AlignCenter);
  button7.setIndicatorAlignment(Qt::AlignLeft);
  
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
