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
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkDynamicSpacer.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkDynamicSpacerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  QWidget topLevel;
  ctkDynamicSpacer* spacer1 = new ctkDynamicSpacer;
  QPushButton* button= new QPushButton;
  button->setCheckable(true);
  ctkDynamicSpacer* spacer2 = new ctkDynamicSpacer;
  
  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(spacer1);
  layout->addWidget(button);
  layout->addWidget(spacer2);
  topLevel.setLayout(layout);
  
  if (spacer1->activeSizePolicy() != 
      QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred))
    {
    std::cerr << "ctkDynamicSpacer: wrong default values" << std::endl;
    return EXIT_FAILURE;
    }
  spacer1->setActiveSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  if (spacer1->activeSizePolicy() != 
      QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding))
    {
    std::cerr << "ctkDynamicSpacer::setActiveSizePolicy failed" << std::endl;
    return EXIT_FAILURE;
    }

  spacer1->setInactiveSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  if (spacer1->inactiveSizePolicy() != 
      QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum))
    {
    std::cerr << "ctkDynamicSpacer::setInactiveSizePolicy failed" << std::endl;
    return EXIT_FAILURE;
    }

  spacer2->setInactiveSizePolicy(spacer1->inactiveSizePolicy());
  spacer2->setActiveSizePolicy(spacer1->activeSizePolicy());
  
  QObject::connect(button, SIGNAL(toggled(bool)), spacer1, SLOT(setActive(bool)));
  QObject::connect(button, SIGNAL(toggled(bool)), spacer2, SLOT(setInactive(bool)));

  spacer1->setPalette(QPalette(Qt::red));
  spacer2->setPalette(QPalette(Qt::blue));
  spacer1->setAutoFillBackground(true);
  spacer2->setAutoFillBackground(true);

  topLevel.resize(50, 300);
  topLevel.show();
  QApplication::processEvents();
  
  // both inactive, they don't grow
  if (spacer1->height() != 0 || spacer2->height() != 0)
    {
    std::cerr << "ctkDynamicSpacer failed 1: " << spacer1->height() << " "
              << spacer2->height() << std::endl;
    return EXIT_FAILURE;
    }
  
  button->toggle();
  // need to repaint the widgets to query their size
  QApplication::processEvents();

  if (spacer1->height() <= 0 || spacer2->height() != 0)
    {
    std::cerr << "ctkDynamicSpacer failed 2: " << spacer1->height() << " "
              << spacer2->height() << std::endl;
    return EXIT_FAILURE;
    }

  button->toggle();
  // need to repaint the widgets to query their size
  QApplication::processEvents();

  if (spacer1->height() != 0 || spacer2->height() <= 0)
    {
    std::cerr << "ctkDynamicSpacer failed 3: " << spacer1->height() << " "
              << spacer2->height() << std::endl;
    return EXIT_FAILURE;
    }
  
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

