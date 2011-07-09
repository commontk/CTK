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
#include <QDebug>
//-----------------------------------------------------------------------------
int ctkCollapsibleButtonTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkCollapsibleButton collapsibleButton;
  collapsibleButton.setText("Button");
  collapsibleButton.setCollapsed(true);

  QLabel* label0 = new QLabel("should be invisible", &collapsibleButton);
  QLabel* label1 = new QLabel("should be invisible", &collapsibleButton);
  QLabel* label2 = new QLabel("should be invisible", &collapsibleButton);
  QLabel* label3 = new QLabel("should be invisible", &collapsibleButton);
  QLabel* label4 = new QLabel("should be visible", &collapsibleButton);
  QLabel* label5 = new QLabel("should be visible", &collapsibleButton);
  QLabel* label6 = new QLabel("should be visible", &collapsibleButton);
  QLabel* label7 = new QLabel("should be visible", &collapsibleButton);
  QLabel* label8 = new QLabel("should be visible", &collapsibleButton);

  label0->setVisible(false);
  label7->setVisible(true);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setContentsMargins(0, 0, 0, 0);
  vbox->addWidget(label0);
  vbox->addWidget(label1);
  vbox->addWidget(label2);
  vbox->addWidget(label3);
  vbox->addWidget(label4);
  vbox->addWidget(label5);
  vbox->addWidget(label6);
  vbox->addWidget(label7);
  vbox->addWidget(label8);
  collapsibleButton.setLayout(vbox);

  label1->setVisible(false);

  collapsibleButton.show();

  if (label0->isVisible() ||
      label1->isVisible() ||
      label2->isVisible() ||
      label3->isVisible() ||
      label4->isVisible() ||
      label5->isVisible() ||
      label6->isVisible() ||
      label7->isVisible() ||
      label8->isVisible())
    {
    std::cout <<  __LINE__ << "Wrong child visibility: "
      << label0->isVisible()  << " "
      << label1->isVisible()  << " "
      << label2->isVisible() << " "
      << label3->isVisible() << " "
      << label4->isVisible() << " "
      << label5->isVisible() << " "
      << label6->isVisible() << " "
      << label7->isVisible() << " "
      << label8->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  label2->setVisible(false);

  if (label2->isVisible())
    {
    std::cout <<  __LINE__ << "Wrong child visibility: "
      << label2->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton.setCollapsed(false);

  if (label0->isVisible() ||
      label1->isVisible() ||
      label2->isVisible() ||
      !label3->isVisible() ||
      !label4->isVisible() ||
      !label5->isVisible() ||
      !label6->isVisible() ||
      !label7->isVisible() ||
      !label8->isVisible())
    {
    std::cout <<  __LINE__ << "Wrong child visibility: "
      << label0->isVisible()  << " "
      << label1->isVisible()  << " "
      << label2->isVisible() << " "
      << label3->isVisible() << " "
      << label4->isVisible() << " "
      << label5->isVisible() << " "
      << label6->isVisible() << " "
      << label7->isVisible() << " "
      << label8->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  label3->setVisible(false);
  label8->setVisible(false);
  
  if (label0->isVisible() ||
      label1->isVisible() ||
      label2->isVisible() ||
      label3->isVisible() ||
      !label4->isVisible() ||
      !label5->isVisible() ||
      !label6->isVisible() ||
      !label7->isVisible() ||
      label8->isVisible())
    {
    std::cout <<  __LINE__ << "Wrong child visibility: "
      << label0->isVisible()  << " "
      << label1->isVisible()  << " "
      << label2->isVisible() << " "
      << label3->isVisible() << " "
      << label4->isVisible() << " "
      << label5->isVisible() << " "
      << label6->isVisible() << " "
      << label7->isVisible() << " "
      << label8->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton.setCollapsed(true);

  if (label0->isVisible() ||
      label1->isVisible() ||
      label2->isVisible() ||
      label3->isVisible() ||
      label4->isVisible() ||
      label5->isVisible() ||
      label6->isVisible() ||
      label7->isVisible() ||
      label8->isVisible())
    {
    std::cout <<  __LINE__ << "Wrong child visibility: "
      << label0->isVisible()  << " "
      << label1->isVisible()  << " "
      << label2->isVisible() << " "
      << label3->isVisible() << " "
      << label4->isVisible() << " "
      << label5->isVisible() << " "
      << label6->isVisible() << " "
      << label7->isVisible() << " "
      << label8->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  label4->setVisible(false);
  label8->setVisible(true);

  if (label4->isVisible() ||
      label8->isVisible())
    {
    std::cout <<  __LINE__ << "Wrong child visibility: "
      << label4->isVisible() << " "
      << label8->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton.setCollapsed(false);

  if (label0->isVisible() ||
      label1->isVisible() ||
      label2->isVisible() ||
      label3->isVisible() ||
      label4->isVisible() ||
      !label5->isVisible() ||
      !label6->isVisible() ||
      !label7->isVisible() ||
      !label8->isVisible())
    {
    std::cout << __LINE__ << "Wrong child visibility: "
      << label0->isVisible()  << " "
      << label1->isVisible()  << " "
      << label2->isVisible() << " "
      << label3->isVisible() << " "
      << label4->isVisible() << " "
      << label5->isVisible() << " "
      << label6->isVisible() << " "
      << label7->isVisible() << " "
      << label8->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  label4->setVisible(true);

  if (!label4->isVisible())
    {
    std::cout << "Wrong child visibility: "
      << label4->isVisible() << std::endl;
    return EXIT_FAILURE;
    }

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
