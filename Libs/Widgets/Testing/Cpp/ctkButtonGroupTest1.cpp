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
#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalSpy>
#include <QTimer>

// CTK includes
#include "ctkButtonGroup.h"
#include "ctkCollapsibleButton.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkButtonGroupTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget widget(0);
  widget.show();
  QHBoxLayout* layout = new QHBoxLayout(&widget);
  QPushButton* button1 = new QPushButton(&widget);
  layout->addWidget(button1);
  QPushButton* button2 = new QPushButton(&widget);
  layout->addWidget(button2);
  QPushButton* button3 = new QPushButton(&widget);
  layout->addWidget(button3);
  ctkCollapsibleButton* button4 = new ctkCollapsibleButton(&widget);
  layout->addWidget(button4);
  widget.setLayout(layout);

  button1->setCheckable(true);
  button2->setCheckable(true);
  button3->setCheckable(false);
  button4->setCheckable(true);
  
  button2->setChecked(true);
  button4->setChecked(true);
  
  ctkButtonGroup buttonGroup(0);

  buttonGroup.addButton(button1);
  buttonGroup.addButton(button2);
  buttonGroup.addButton(button3);
  buttonGroup.addButton(button4);

  if (!button4->isChecked() || button2->isChecked())
    {
    std::cerr << "ctkButtonGroup::addButton failed"
              << button2->isChecked() << " " << button4->isChecked() 
              << std::endl;
    return EXIT_FAILURE;
    }

  // Click #1: check button1
  button1->click();
  
  if (!button1->isChecked() || button4->isChecked())
    {
    std::cerr << "ctkButtonGroup::click1 failed"
              << button1->isChecked() << " " << button4->isChecked() 
              << std::endl;
    return EXIT_FAILURE;
    }

  // Click #2: uncheck button1 
  button1->click();

  if (button1->isChecked() || button4->isChecked())
    {
    std::cerr << "ctkButtonGroup::click2 failed"
              << button1->isChecked() << " " << button4->isChecked() 
              << std::endl;
    return EXIT_FAILURE;
    }

  // Click #3: check button1 
  button1->click();

  if (!button1->isChecked() || button4->isChecked())
    {
    std::cerr << "ctkButtonGroup::click3 failed"
              << button1->isChecked() << " " << button4->isChecked() 
              << std::endl;
    return EXIT_FAILURE;
    }

  // Click #4: check button2 
  button2->click();

  if (!button2->isChecked() || button1->isChecked())
    {
    std::cerr << "ctkButtonGroup::click4 failed"
              << button2->isChecked() << " " << button1->isChecked() 
              << std::endl;
    return EXIT_FAILURE;
    }
  
  // Click #5: click button3 keep check on button2
  button3->click();

  if (!button2->isChecked() || button3->isChecked())
    {
    std::cerr << "ctkButtonGroup::click5 failed"
              << button2->isChecked() << " " << button3->isChecked() 
              << std::endl;
    return EXIT_FAILURE;
    }
  
  // Click #6: uncheck button2
  button2->click();

  if (button2->isChecked())
    {
    std::cerr << "ctkButtonGroup::click6 failed"
              << button2->isChecked() << std::endl;
    return EXIT_FAILURE;
    }
  qRegisterMetaType<QAbstractButton*>("QAbstractButton*");
  QSignalSpy spy(&buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)));
  QSignalSpy spyInt(&buttonGroup, SIGNAL(buttonClicked(int)));
  button1->click();
  if (spy.count() != 1 || spyInt.count() != 1)
    {
    std::cerr << "ctkButtonGroup::click7 failed"
              << button1->isChecked() << ", "
              << spy.count() << "clicks" << std::endl;
    return EXIT_FAILURE;
    }
  button4->click();
  if (spy.count() != 2 || spyInt.count() != 2)
    {
    std::cerr << "ctkButtonGroup::click8 failed"
              << button4->isChecked() << ", "
              << spy.count() << "clicks" << std::endl;
    return EXIT_FAILURE;
    }
  button4->click();
  if (spy.count() != 3 || spyInt.count() != 3)
    {
    std::cerr << "ctkButtonGroup::click9 failed"
              << button4->isChecked() << ", "
              << spy.count() << "clicks" << std::endl;
    return EXIT_FAILURE;
    }
  QTimer autoExit;
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(500);
    }
  return app.exec();
}

