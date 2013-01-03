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
#include <QFormLayout>
#include <QProxyStyle>
#include <QTimer>

// CTK includes
#include "ctkCheckBox.h"
#include "ctkProxyStyle.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckBoxTest1(int argc, char * argv [])
{
  //QPlastiqueStyle* style = new QPlastiqueStyle;
  //QApplication::setStyle(style);
  QApplication app(argc, argv);

  QPixmap onPixmap =
    QApplication::style()->standardPixmap(QStyle::SP_DriveCDIcon);
  QPixmap offPixmap =
    QApplication::style()->standardPixmap(QStyle::SP_DesktopIcon);
  QIcon icon;
  icon.addPixmap(offPixmap,
                 QIcon::Normal,
                 QIcon::On);
  icon.addPixmap(onPixmap,
                 QIcon::Normal,
                 QIcon::Off);

  QWidget topLevel;
  ctkCheckBox* checkBoxWithoutIcon = new ctkCheckBox(0);
  checkBoxWithoutIcon->setIndicatorIcon(icon);

  ctkCheckBox* checkBoxWithIcon = new ctkCheckBox(0);
  checkBoxWithIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));

  ctkCheckBox* checkBoxWithIconAndText = new ctkCheckBox(0);
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

  QFormLayout* layout = new QFormLayout(&topLevel);
  layout->addRow("Indicator:", checkBoxWithoutIcon);
  layout->addRow("Icon:", checkBoxWithIcon);
  layout->addRow("Indicator, Icon, Text:", checkBoxWithIconAndText);
  layout->addRow("Default:", checkBoxWithoutIcon2);
  layout->addRow("Icon:", checkBoxWithIcon2);
  layout->addRow("Icon, Text:", checkBoxWithIconAndText2);
  topLevel.setLayout(layout);
  topLevel.show();

  // Style check
  ctkProxyStyle* checkBoxStyle = qobject_cast<ctkProxyStyle*>(checkBoxWithoutIcon->style());
  if (!checkBoxStyle)
    {
    std::cerr << "Not a ctkProxyStyle" << std::endl;
    return EXIT_FAILURE;
    }
  checkBoxStyle->ensureBaseStyle();
  if (checkBoxStyle->baseStyle()->proxy() != checkBoxStyle)
    {
    std::cerr << "Wrong proxy: " << checkBoxStyle->baseStyle()->proxy() << std::endl;
    return EXIT_FAILURE;
    }

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
