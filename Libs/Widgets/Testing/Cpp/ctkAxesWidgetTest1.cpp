/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QSignalSpy>
#include <QTimer>

// CTK includes
#include "ctkAxesWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkAxesWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkAxesWidget axes(0);
  if (axes.currentAxis() != ctkAxesWidget::None)
    {
    std::cerr << "ctkAxesWidget default axis is wrong: "
              << axes.currentAxis() << std::endl;
    return EXIT_FAILURE;
    }

  QSignalSpy spy(&axes, SIGNAL(currentAxisChanged(Axis)));
  axes.setCurrentAxis(ctkAxesWidget::Anterior);

  if (axes.currentAxis() != ctkAxesWidget::Anterior ||
      spy.count() != 1)
    {
    std::cerr << "ctkAxesWidget default axis is wrong: "
              << axes.currentAxis() << " " << spy.count() << std::endl;
    return EXIT_FAILURE;
    }
  if ( spy.takeFirst().at(0).toInt() == ctkAxesWidget::Anterior)
    {
    std::cerr << "ctkAxesWidget fired the wrong current axis : "
              << spy.takeFirst().at(0).toInt() << std::endl;
    return EXIT_FAILURE;
    }
  axes.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

