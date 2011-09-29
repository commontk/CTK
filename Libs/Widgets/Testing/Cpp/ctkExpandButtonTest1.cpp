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
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

// CTK includes
#include "ctkExpandButton.h"

//-----------------------------------------------------------------------------
int ctkExpandButtonTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QWidget widget(0);
  widget.show();
  QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, &widget);

  // Expand button horizontal
  QHBoxLayout* hLayout = new QHBoxLayout();
  layout->addLayout(hLayout);
  ctkExpandButton* hbutton = new ctkExpandButton(&widget);

  QPushButton* pushButton = new QPushButton("test");
  hLayout->addWidget(pushButton);
  pushButton->setVisible(false);
  QPushButton* pushButton2 = new QPushButton("foo");
  hLayout->addWidget(pushButton2);
  pushButton2->setVisible(false);
  hLayout->addWidget(hbutton);
  hbutton->setMirrorOnExpand(true);

  // Expand button vertical
  QVBoxLayout* vLayout = new QVBoxLayout();
  layout->addLayout(vLayout);
  ctkExpandButton* vbutton = new ctkExpandButton(&widget);

  QPushButton* pushButton3 = new QPushButton("test vertival");
  vLayout->addWidget(pushButton3);
  pushButton3->setVisible(false);
  QPushButton* pushButton4 = new QPushButton("foo vertical");
  vLayout->addWidget(pushButton4);
  pushButton4->setVisible(false);
  vLayout->addWidget(vbutton);
  vbutton->setMirrorOnExpand(true);
  vbutton->setOrientation(Qt::Vertical);


  QObject::connect(hbutton, SIGNAL(clicked(bool)),
                   pushButton, SLOT(setVisible(bool)));
  QObject::connect(hbutton, SIGNAL(clicked(bool)),
                   pushButton2, SLOT(setVisible(bool)));
  QObject::connect(vbutton, SIGNAL(clicked(bool)),
                   pushButton3, SLOT(setVisible(bool)));
  QObject::connect(vbutton, SIGNAL(clicked(bool)),
                   pushButton4, SLOT(setVisible(bool)));

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(500, &app, SLOT(quit()));
    }

  return app.exec();
}
