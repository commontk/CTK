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
#include <QSignalSpy>
#include <QTimer>

// CTK includes
#include "ctkDoubleRangeSlider.h"

// STD includes
#include <cstdlib>
#include <iostream>


//-----------------------------------------------------------------------------
int ctkDoubleRangeSliderTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkDoubleRangeSlider slider(Qt::Horizontal);

  QSignalSpy spyRange(&slider, SIGNAL(rangeChanged(double,double)));

  slider.setRange(200., 499.01);
  
  if (spyRange.count() != 1 ||
      spyRange.first().at(0).toDouble() != 200. ||
      spyRange.first().at(1).toDouble() != 499.01)
    {
    std::cerr << "ctkDoubleRangeSlider::setRange() failed: "
              << spyRange.count() << " min: "
              << (spyRange.count() ? spyRange.first().at(0).toDouble() : -1.)
              << " max: "
              << (spyRange.count() ? spyRange.first().at(1).toDouble() : -1.)
              << std::endl;
    return EXIT_FAILURE; 
    }
  spyRange.clear();
  // set again and rangeChanged shouldn't be called 
  slider.setRange(200., 499.01);
  slider.setMinimum(200.);
  slider.setMaximum(499.01);

  if (spyRange.count() != 0)
    {
    std::cerr << "ctkDoubleRangeSlider::setRange() fired useless signal: "
              << spyRange.count() << " min: "
              << spyRange.first().at(0).toDouble() << " max: "
              << spyRange.first().at(1).toDouble() << std::endl;
    return EXIT_FAILURE; 
    }

  // while it might not change the underline (int) slider, we still need to fire
  // an event
  slider.setMaximum(499.00);
  
  if (spyRange.count() != 1 ||
      spyRange.first().at(0).toDouble() != 200. ||
      spyRange.first().at(1).toDouble() != 499.00)
    {
    std::cerr << "ctkDoubleRangeSlider::setMaximum() failed: "
              << spyRange.count() << " min: "
              << (spyRange.count() ? spyRange.first().at(0).toDouble() : -1.)
              << " max: "
              << (spyRange.count() ? spyRange.first().at(1).toDouble() : -1.)
              << std::endl;
    return EXIT_FAILURE; 
    }
  
  slider.show();
  
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
