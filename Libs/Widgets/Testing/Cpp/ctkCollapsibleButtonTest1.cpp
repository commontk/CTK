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
#include <QPushButton>
#include <QVBoxLayout>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkCollapsibleButton.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCollapsibleButtonTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkCollapsibleButton collapsibleButton;
  collapsibleButton.setText("top button");
  QDoubleSpinBox *spinBox = new QDoubleSpinBox;
  QPushButton * button= new QPushButton(QObject::tr("Button"));
  ctkCollapsibleButton *collapsibleButton2 = new ctkCollapsibleButton(QObject::tr("Nested Collapsible Button"));
  ctkCollapsibleButton *collapsibleButton3 = new ctkCollapsibleButton(QObject::tr("CollapsibleButton"));
  // ctkCollapsibleButton::icon is not activated
  collapsibleButton3->setIcon(collapsibleButton3->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  QPushButton * button2 = new QPushButton(QObject::tr("Nested PushButton"));

  QVBoxLayout *nestedBox = new QVBoxLayout;
  nestedBox->addWidget(button2);
  collapsibleButton3->setLayout(nestedBox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(spinBox);
  vbox->addWidget(button);
  vbox->addWidget(collapsibleButton2);
  vbox->addWidget(collapsibleButton3);
  collapsibleButton.setLayout(vbox);

  collapsibleButton.show();

  if (collapsibleButton.collapsed())
    {
    std::cerr<< "Wrong default collapse state." << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton.setCollapsed(true);

  if (collapsibleButton.collapsed() != true)
    {
    std::cerr<< "ctkCollapsibleButton::setCollapsed failed." << std::endl;
    return EXIT_FAILURE;
    }

  if (spinBox->isVisible())
    {
    std::cerr << "ctkCollapsibleButton::setChecked failed. "
              << "Children are visible" << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton.setChecked(true);

  if (collapsibleButton.collapsed() != false)
    {
    std::cerr<< "ctkCollapsibleButton::setChecked failed." << std::endl;
    return EXIT_FAILURE;
    }
  
  collapsibleButton2->setCollapsedHeight(40);
  
  if (collapsibleButton2->collapsedHeight() != 40)
    {
    std::cerr << "ctkCollapsibleButton::setCollapsedHeight failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton.setContentsFrameShape(QFrame::Box);
  collapsibleButton2->setContentsFrameShape(QFrame::Box);

  if (collapsibleButton2->contentsFrameShape() != QFrame::Box)
    {
    std::cerr << "ctkCollapsibleButton::setContentsFrameShape failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton2->setContentsFrameShadow(QFrame::Raised);

  if (collapsibleButton2->contentsFrameShadow() != QFrame::Raised)
    {
    std::cerr << "ctkCollapsibleButton::setContentsFrameShadow failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton2->setContentsLineWidth(2);

  if (collapsibleButton2->contentsLineWidth() != 2)
    {
    std::cerr << "ctkCollapsibleButton::setContentsLineWidth failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton2->setContentsMidLineWidth(3);

  if (collapsibleButton2->contentsMidLineWidth() != 3)
    {
    std::cerr << "ctkCollapsibleButton::setContentsLineWidth failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton.setButtonTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

  if (collapsibleButton.buttonTextAlignment() != (Qt::AlignRight | Qt::AlignVCenter))
    {
    std::cerr << "ctkCollapsibleButton::setButtonTextAlignment failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  collapsibleButton2->setIndicatorAlignment(Qt::AlignRight);

  if (collapsibleButton2->indicatorAlignment() != Qt::AlignRight)
    {
    std::cerr << "ctkCollapsibleButton::setIndicatorAlignment failed."
              << std::endl;
    return EXIT_FAILURE;
    }

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
