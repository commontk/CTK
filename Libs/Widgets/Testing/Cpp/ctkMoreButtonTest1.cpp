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
#include <QComboBox>
#include <QHBoxLayout>
#include <QTimer>
#include <QWidget>

// CTK includes
#include "ctkMoreButton.h"

// STD includes

//-----------------------------------------------------------------------------
int ctkMoreButtonTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QWidget widget(0);
  widget.show();

  QHBoxLayout* layout = new QHBoxLayout(&widget);
  ctkMoreButton* button = new ctkMoreButton(&widget);
  layout->addWidget(button);

  QPushButton* pushButton = new QPushButton("test");
  layout->addWidget(pushButton);
  pushButton->setVisible(false);
  QPushButton* pushButton2 = new QPushButton("foo");
  layout->addWidget(pushButton2);
  pushButton2->setVisible(false);

  button->setCustomBehavior(true);

  QObject::connect(button, SIGNAL(clicked(bool)),
                   pushButton, SLOT(setVisible(bool)));
  QObject::connect(button, SIGNAL(clicked(bool)),
                   pushButton2, SLOT(setVisible(bool)));
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(500, &app, SLOT(quit()));
    }

  return app.exec();
}
