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
#include <QString>
#include <QTimer>

// CTK includes
#include "ctkDoubleSlider.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkDoubleSliderTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkDoubleSlider slider;
  slider.setRange(-1., 1.);
  slider.setValue(-0.5);
  slider.setRange(0.,1.);
  if (slider.value() != 0.)
    {
    std::cout << "ctkDoubleSlider::setRange() failed: " << slider.value() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setRange(2.0001, 2.0001);
  if (slider.value() != 2.0001)
    {
    std::cout << "ctkDoubleSlider::setRange() failed: " << slider.value() << std::endl;
    return EXIT_FAILURE;
    }
  slider.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(100, &app, SLOT(quit()));
    }
  return app.exec();
}

