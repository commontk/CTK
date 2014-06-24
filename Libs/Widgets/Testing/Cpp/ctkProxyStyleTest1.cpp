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
#include <QProxyStyle>
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkCheckBox.h"
#include "ctkCollapsibleButton.h"
#include "ctkCollapsibleGroupBox.h"
#include "ctkProxyStyle.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkProxyStyleTest1(int argc, char * argv [])
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
  QVBoxLayout* topLevelLayout = new QVBoxLayout(&topLevel);
  topLevel.setLayout(topLevelLayout);
  for (int i = 0; i < 2 ; ++i)
    {
    ctkCollapsibleButton* button = new ctkCollapsibleButton;
    QVBoxLayout* buttonLayout = new QVBoxLayout(button);
    button->setLayout(buttonLayout);
    for (int j = 0; j < 2; ++j)
      {
      ctkCollapsibleGroupBox* groupBox = new ctkCollapsibleGroupBox;
      QVBoxLayout* groupBoxLayout = new QVBoxLayout(groupBox);
      groupBox->setLayout(groupBoxLayout);
      for (int k = 0; k < 2; ++k)
        {
        ctkCheckBox* checkBox = new ctkCheckBox;
        checkBox->setIndicatorIcon(icon);
        groupBoxLayout->addWidget(checkBox);
        }
      buttonLayout->addWidget(groupBox);
      }
    topLevelLayout->addWidget(button);
    }

  topLevel.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
