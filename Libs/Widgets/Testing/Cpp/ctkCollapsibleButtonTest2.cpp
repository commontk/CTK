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
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkCollapsibleButton.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCollapsibleButtonTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkCollapsibleButton collapsibleButton;
  collapsibleButton.setText("Button");

  QLabel* label0 = new QLabel("should be invisible", &collapsibleButton);
  QLabel* label1 = new QLabel("should be invisible", &collapsibleButton);
  QLabel* label2 = new QLabel("should be invisible", &collapsibleButton);
  QLabel* label3 = new QLabel("should be visible", &collapsibleButton);
  QLabel* label4 = new QLabel("should be visible", &collapsibleButton);
  QLabel* label5 = new QLabel("should be visible", &collapsibleButton);
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(label0);
  vbox->addWidget(label1);
  vbox->addWidget(label2);
  vbox->addWidget(label3);
  vbox->addWidget(label4);
  vbox->addWidget(label5);
  collapsibleButton.setLayout(vbox);

  label0->setVisible(false);

  collapsibleButton.show();

  label1->setVisible(false);
  
  if (label0->isVisible() ||
      label1->isVisible() ||
      !label2->isVisible() ||
      !label3->isVisible() ||
      !label4->isVisible() ||
      !label5->isVisible())
    {
    std::cout << "Wrong child visibility: "
      << label0->isVisible()  << " "
      << label1->isVisible()  << " "
      << label2->isVisible() << " "
      << label3->isVisible() << " "
      << label4->isVisible() << " "
      << label5->isVisible() << std::endl;
    return EXIT_FAILURE;
    }
  
  collapsibleButton.setCollapsed(true);
  
  if (label0->isVisible() ||
      label1->isVisible() ||
      label2->isVisible() ||
      label3->isVisible() ||
      label4->isVisible() ||
      label5->isVisible())
    {
    std::cout << "Wrong child visibility: "
      << label0->isVisible()  << " "
      << label1->isVisible()  << " "
      << label2->isVisible() << " "
      << label3->isVisible() << " "
      << label4->isVisible() << " "
      << label5->isVisible() << std::endl;
    return EXIT_FAILURE;
    }
  
  label2->setVisible(false);
  label3->setVisible(true);
  label4->setVisible(false);
  label5->setVisible(false);
  label5->setVisible(true);
  
  if (label0->isVisible() ||
      label1->isVisible() ||
      label2->isVisible() ||
      label3->isVisible() ||
      label4->isVisible() ||
      label5->isVisible())
    {
    std::cout << "Wrong child visibility: "
      << label0->isVisible()  << " "
      << label1->isVisible()  << " "
      << label2->isVisible() << " "
      << label3->isVisible() << " "
      << label4->isVisible() << " "
      << label5->isVisible() << std::endl;
    return EXIT_FAILURE;
    }
  
  collapsibleButton.setCollapsed(false);

  if (label0->isVisible() ||
      label1->isVisible() ||
      label2->isVisible() ||
      !label3->isVisible() ||
      label4->isVisible() ||
      !label5->isVisible())
    {
    std::cout << "Wrong child visibility: "
      << label0->isVisible()  << " "
      << label1->isVisible()  << " "
      << label2->isVisible() << " "
      << label3->isVisible() << " "
      << label4->isVisible() << " "
      << label5->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  label4->setVisible(true);

  if (label0->isVisible() ||
      label1->isVisible() ||
      label2->isVisible() ||
      !label3->isVisible() ||
      !label4->isVisible() ||
      !label5->isVisible())
    {
    std::cout << "Wrong child visibility: "
      << label0->isVisible()  << " "
      << label1->isVisible()  << " "
      << label2->isVisible() << " "
      << label3->isVisible() << " "
      << label4->isVisible() << " "
      << label5->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
