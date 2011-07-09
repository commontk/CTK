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
#include <QStyle>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>

// CTK includes
#include "ctkCollapsibleButton.h"
#include "ctkDynamicSpacer.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkDynamicSpacerTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  QWidget topLevel;
  ctkCollapsibleButton* button = new ctkCollapsibleButton("Collapsible button");
  QTreeView* view = new QTreeView;
  QVBoxLayout* collapsibleLayout = new QVBoxLayout;
  collapsibleLayout->addWidget(view);
  button->setLayout(collapsibleLayout);

  ctkDynamicSpacer* spacer = new ctkDynamicSpacer;
  
  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(button);
  layout->addWidget(spacer);
  topLevel.setLayout(layout);
  
  view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  spacer->setInactiveSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  spacer->setActiveSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
  QObject::connect(button, SIGNAL(contentsCollapsed(bool)),
                   spacer, SLOT(setActive(bool)));
  spacer->setPalette(QPalette(Qt::red));
  spacer->setAutoFillBackground(true);

  topLevel.show();
  
  if (spacer->height() != 0)
    {
    std::cerr << "ctkDynamicSpacer failed: " << spacer->height() << std::endl;
    return EXIT_FAILURE;
    }

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

