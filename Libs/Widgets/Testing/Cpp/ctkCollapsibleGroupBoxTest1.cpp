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
#include <QCheckBox>
#include <QRadioButton>
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkCollapsibleGroupBox.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCollapsibleGroupBoxTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget topLevel;
  ctkCollapsibleGroupBox* groupBox = new ctkCollapsibleGroupBox(QObject::tr("GroupBox"));
  QRadioButton *radio1 = new QRadioButton(QObject::tr("&Radio button 1"));
  QRadioButton *radio2 = new QRadioButton(QObject::tr("R&adio button 2"));
  QRadioButton *radio3 = new QRadioButton(QObject::tr("Ra&dio button 3"));
  ctkCollapsibleGroupBox* hiddenGroupBox = new ctkCollapsibleGroupBox;
  hiddenGroupBox->setTitle("Advanced...");

  radio1->setChecked(true);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(radio1);
  vbox->addWidget(radio2);
  vbox->addWidget(radio3);
  vbox->addWidget(hiddenGroupBox);
  vbox->addStretch(1);
  groupBox->setLayout(vbox);


  QCheckBox* checkBox = new QCheckBox("Check box");
  QVBoxLayout *vbox2 = new QVBoxLayout;
  vbox2->addWidget(checkBox);
  hiddenGroupBox->setLayout(vbox2);
  hiddenGroupBox->setCollapsed(true);

  hiddenGroupBox->setVisible(false);

  QVBoxLayout* topLevelVBox = new QVBoxLayout;
  topLevelVBox->addWidget(groupBox);
  topLevel.setLayout(topLevelVBox);

  topLevel.show();

  if (groupBox->collapsed())
    {
    std::cerr<< "Wrong default collapse state." << std::endl;
    return EXIT_FAILURE;
    }

  groupBox->setCollapsed(true);

  if (groupBox->collapsed() != true)
    {
    std::cerr<< "ctkCollapsibleGroupBox::setCollapsed failed." << std::endl;
    return EXIT_FAILURE;
    }

  if (radio1->isVisible())
    {
    std::cerr << "ctkCollapsibleGroupBox::setChecked failed. "
              << "Children are visible" << std::endl;
    return EXIT_FAILURE;
    }

  hiddenGroupBox->setVisible(true);
  if (hiddenGroupBox->isVisible())
    {
    std::cerr << "Nested widget in ctkCollapsibleGroupBox failed. "
              << "Child is visible" << std::endl;
    return EXIT_FAILURE;
    }

  groupBox->setChecked(true);

  if (groupBox->collapsed() != false)
    {
    std::cerr<< "ctkCollapsibleGroupBox::setChecked failed." << std::endl;
    return EXIT_FAILURE;
    }

  if (!hiddenGroupBox->isVisible())
    {
    std::cerr << "Nested widget in ctkCollapsibleGroupBox failed. "
              << "Child is not visible" << std::endl;
    return EXIT_FAILURE;
    }

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

