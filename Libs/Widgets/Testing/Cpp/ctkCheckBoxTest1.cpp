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
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkCheckBox.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckBoxTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget topLevel;
  ctkCheckBox* checkBoxWithoutIcon = new ctkCheckBox(0);
  ctkCheckBox* checkBoxWithIcon = new ctkCheckBox(0);
  checkBoxWithIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
  ctkCheckBox* checkBoxWithIconAndText = new ctkCheckBox(0);
  QIcon icon;
  icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_DriveCDIcon),
                 QIcon::Normal,
                 QIcon::On);
  icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_DesktopIcon),
                 QIcon::Normal,
                 QIcon::Off);
  checkBoxWithoutIcon->setIndicatorIcon(icon);
//  checkBoxWithoutIcon->setIndicatorIconSize(QSize(15,15));
  checkBoxWithIcon->setIndicatorIcon(icon);
  if (checkBoxWithIcon->indicatorIcon().isNull())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with "
        << "the function indicatorIcon()" << std::endl;
    return EXIT_FAILURE;
    }

  icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_FileLinkIcon),
                 QIcon::Active,
                 QIcon::On);

  checkBoxWithIcon->setIndicatorIcon(QIcon());
  checkBoxWithIconAndText->setIndicatorIcon(icon);
  checkBoxWithIconAndText->setIndicatorIconSize(QSize(25,25));
  checkBoxWithIconAndText->setText("Test1");
  checkBoxWithIconAndText->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));

  ctkCheckBox* checkBoxWithoutIcon2 = new ctkCheckBox(0);
  ctkCheckBox* checkBoxWithIcon2 = new ctkCheckBox(0);
  checkBoxWithIcon2->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
  ctkCheckBox* checkBoxWithIconAndText2 = new ctkCheckBox(0);
  checkBoxWithIconAndText2->setText("Test1");
  checkBoxWithIconAndText2->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(checkBoxWithoutIcon);
  layout->addWidget(checkBoxWithIcon);
  layout->addWidget(checkBoxWithIconAndText);
  layout->addWidget(checkBoxWithoutIcon2);
  layout->addWidget(checkBoxWithIcon2);
  layout->addWidget(checkBoxWithIconAndText2);
  topLevel.setLayout(layout);
  topLevel.show();


  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
