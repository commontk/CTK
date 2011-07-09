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
#include <QString>
#include <QTimer>

// CTK includes
#include "ctkDoubleRangeSlider.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
bool checkSlider(const ctkDoubleRangeSlider& slider)
{
  return slider.minimum()  <= slider.minimumValue() &&
    slider.minimumValue() <= slider.maximumValue() &&
    slider.maximumValue() <= slider.maximum();
}

//-----------------------------------------------------------------------------
bool checkSlider(const ctkDoubleRangeSlider& slider,
                 double min, double minVal, double maxVal, double max)
{
  return qFuzzyCompare(slider.minimum(), min) &&
    qFuzzyCompare(slider.minimumValue(), minVal) &&
    qFuzzyCompare(slider.maximumValue(), maxVal) &&
    qFuzzyCompare(slider.maximum(), max);
}

//-----------------------------------------------------------------------------
bool checkSlider(const ctkDoubleRangeSlider& slider,
                 double min, double minVal, double maxVal, double max, double minPos, double maxPos)
{
  return qFuzzyCompare(slider.minimum(), min) &&
    qFuzzyCompare(slider.minimumValue(), minVal) &&
    qFuzzyCompare(slider.maximumValue(), maxVal) &&
    qFuzzyCompare(slider.maximum(), max) &&
    qFuzzyCompare(slider.minimumPosition(), minPos) &&
    qFuzzyCompare(slider.maximumPosition(), maxPos);
}

//-----------------------------------------------------------------------------
int ctkDoubleRangeSliderTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkDoubleRangeSlider slider;
  if (!(slider.minimum()  <= slider.minimumValue() &&
        slider.minimumValue() <= slider.maximumValue() &&
        slider.maximumValue() <= slider.maximum() &&
        slider.minimum() <= slider.minimumPosition() &&
        slider.minimumPosition() <= slider.maximumPosition() &&
        slider.maximumPosition() <= slider.maximum()))
    {
    std::cerr << "ctkDoubleRangeSlider:: 1) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 1) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setMinimum(10.10);
  slider.setMaximum(3999.99);
  slider.setValues(40.1, 20.4);
  if (!checkSlider(slider, 10.10, 20.4, 40.1, 3999.99, 20.4, 40.1))
    {
    std::cerr << "ctkDoubleRangeSlider:: 2) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 2) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setValues(700.4, 20.99);
  if (!checkSlider(slider, 10.1, 20.99, 700.4, 3999.99, 20.99, 700.4))
    {
    std::cerr << "ctkDoubleRangeSlider:: 3) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 3) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setValues(800, 900);
  if (!checkSlider(slider, 10.1, 800, 900, 3999.99, 800, 900))
    {
    std::cerr << "ctkDoubleRangeSlider:: 4) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 4) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setValues(5000, 6000);
  if (!checkSlider(slider, 10.1, 3999.99, 3999.99, 3999.99, 3999.99, 3999.99))
    {
    std::cerr << "ctkDoubleRangeSlider:: 5) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 5) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setRange(-15.21, 25.49999);
  if (!checkSlider(slider, -15.21, 25.49999, 25.49999, 25.49999, 25.49999, 25.49999))
    {
    std::cerr << "ctkDoubleRangeSlider:: 6) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    return EXIT_FAILURE;
    }
  // nothing happen yet but it would be nice if it moves the 2 values at 
  // the same time.
  slider.triggerAction(QAbstractSlider::SliderSingleStepSub);
  if (!checkSlider(slider, -15.21, 25.49999, 25.49999, 25.49999, 25.49999, 25.49999))
    {
    std::cerr << "ctkDoubleRangeSlider:: 7) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 7) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setMaximumValue(10.2);
  if (!checkSlider(slider, -15.21, 10.2, 10.2, 25.49999, 10.2, 10.2))
    {
    std::cerr << "ctkDoubleRangeSlider:: 8) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 8) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setMinimumValue(17.7777);
  if (!checkSlider(slider, -15.21, 17.7777, 17.7777, 25.49999, 17.7777, 17.7777))
    {
    std::cerr << "ctkDoubleRangeSlider:: 9) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 9) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setMinimumValue(-20);
  if (!checkSlider(slider, -15.21, -15.21, 17.7777, 25.49999, -15.21, 17.7777))
    {
    std::cerr << "ctkDoubleRangeSlider:: 10) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 10) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setSingleStep(0.15);
  if (!checkSlider(slider, -15.21, -15.21, 17.7777, 25.49999, -15.21, 17.7777))
    {
    std::cerr << "ctkDoubleRangeSlider:: 10.5) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 10.5) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setSingleStep(0.33);
  if (!checkSlider(slider, -15.21, -15.21, 17.7777, 25.49999, -15.21, 17.7777))
    {
    std::cerr << "ctkDoubleRangeSlider:: 11) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 11) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setMinimumValue(-15.205);
  if (!checkSlider(slider, -15.21, -15.205, 17.7777, 25.49999, -15.205, 17.7777))
    {
    std::cerr << "ctkDoubleRangeSlider:: 12) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 12) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setSingleStep(1.);
  if (!checkSlider(slider, -15.21, -15.205, 17.7777, 25.49999, -15.205, 17.7777))
    {
    std::cerr << "ctkDoubleRangeSlider:: 13) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 13) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setSingleStep(0.01);
  if (!checkSlider(slider, -15.21, -15.205, 17.7777, 25.49999, -15.205, 17.7777))
    {
    std::cerr << "ctkDoubleRangeSlider:: 14) "
              << slider.minimum() << " " 
              << slider.minimumValue() << " " 
              << slider.maximumValue() << " " 
              << slider.maximum() << std::endl;
    std::cerr << "ctkDoubleRangeSlider:: 14) "
              << slider.minimumPosition() << " " 
              << slider.maximumPosition() << std::endl;
    return EXIT_FAILURE;
    }
  // FIXME
  //slider.setPositions(0.1, 20.5);
  // if (!checkSlider(slider, -15.21, 0.1, 20.5, 25.49999, 0.1, 20.5))
  //   {
  //   std::cerr << "ctkDoubleRangeSlider:: 15) "
  //             << slider.minimum() << " " 
  //             << slider.minimumValue() << " " 
  //             << slider.maximumValue() << " " 
  //             << slider.maximum() << std::endl;
  //   std::cerr << "ctkDoubleRangeSlider:: 15) "
  //             << slider.minimumPosition() << " " 
  //             << slider.maximumPosition() << std::endl;
  //   return EXIT_FAILURE;
  //   }
  // slider.setTracking(false); 
  // slider.setPositions(0.123456, 20.123465);
  // if (!checkSlider(slider, -15.21, 0.1, 20.5, 25.49999, 0.123456, 20.123456))
  //   {
  //   std::cerr << "ctkDoubleRangeSlider:: 16) "
  //             << slider.minimum() << " " 
  //             << slider.minimumValue() << " " 
  //             << slider.maximumValue() << " " 
  //             << slider.maximum() << std::endl;
  //   std::cerr << "ctkDoubleRangeSlider:: 16) "
  //             << slider.minimumPosition() << " " 
  //             << slider.maximumPosition() << std::endl;
  //   return EXIT_FAILURE;
  //   }
  slider.setSymmetricMoves(true);
  
  if (slider.symmetricMoves() != true)
    {
    std::cerr << "ctkDoubleRangeSlider::setSymmetricMoves failed" << std::endl;
    return EXIT_FAILURE;
    }

  slider.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
