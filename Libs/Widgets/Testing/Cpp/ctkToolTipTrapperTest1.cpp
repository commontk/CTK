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
#include <QPushButton>
#include <QTimer>

// CTK includes
#include "ctkToolTipTrapper.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkToolTipTrapperTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkToolTipTrapper trapper;
  
  if (trapper.toolTipsTrapped() != true)
    {
    std::cerr << "ctkToolTipTrapper::toolTipsTrapped default value" << std::endl;
    return EXIT_FAILURE;
    }

  trapper.setToolTipsTrapped(false);

  if (trapper.toolTipsTrapped() != false)
    {
    std::cerr << "ctkToolTipTrapper::setToolTipsTrapped failed" << std::endl;
    return EXIT_FAILURE;
    }
  
  QPushButton button("button");
  button.setToolTip("Button tooltip text");
  button.setCheckable(true);
  QObject::connect(&button, SIGNAL(toggled(bool)),
                   &trapper, SLOT(setToolTipsTrapped(bool)));
  button.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

