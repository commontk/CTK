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
#include "ctkDoubleSlider.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkDoubleSliderTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkDoubleSlider slider;
  std::cout << "Test setValue(18.54)" << std::endl;
  slider.setValue(18.54);
  if (!qFuzzyCompare(slider.value(), 18.54))
    {
    std::cerr << "ctkDoubleSlider::setValue failed." << slider.value() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test setMinimum(5.21)" << std::endl;
  slider.setMinimum(5.21);
  if (!qFuzzyCompare(slider.minimum(), 5.21) ||
      !qFuzzyCompare(slider.value(),18.54) ||
      !qFuzzyCompare(slider.sliderPosition(), 18.54))
    {
    std::cerr << "ctkDoubleSlider::setMinimum failed." << slider.value() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test setMaximum(1340.2)" << std::endl;
  slider.setMaximum(1340.2);
  if (!qFuzzyCompare(slider.maximum(), 1340.2) ||
      !qFuzzyCompare(slider.value(), 18.54) ||
      !qFuzzyCompare(slider.sliderPosition(), 18.54))
    {
    std::cerr << "ctkDoubleSlider::setMinimum failed." << slider.value() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test setSingleStep(0.15)" << std::endl;
  slider.setSingleStep(0.15);
  if (!qFuzzyCompare(slider.singleStep(), 0.15) ||
      !qFuzzyCompare(slider.value(), 18.54) ||
      !qFuzzyCompare(slider.sliderPosition(), 18.54))
    {
    std::cerr << "ctkDoubleSlider::setSingleStep failed."
              << "Step:" << slider.singleStep() 
              << " Val:" << slider.value() 
              << " Pos:" << slider.sliderPosition() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test setSingleStep(0.33)" << std::endl;
  slider.setSingleStep(0.33);
  if (!qFuzzyCompare(slider.singleStep(), 0.33) ||
      !qFuzzyCompare(slider.value(), 18.54) ||
      !qFuzzyCompare(slider.sliderPosition(), 18.54))
    {
    std::cerr << "ctkDoubleSlider::setSingleStep failed."
              << slider.value() << " " 
              << slider.sliderPosition() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test Step Add" << std::endl;
  slider.triggerAction (QAbstractSlider::SliderSingleStepAdd);
  if (!qFuzzyCompare(slider.value(), 18.87))
    {
    std::cerr << "ctkDoubleSlider:: Step Add failed" << slider.value() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test Step Sub" << std::endl;
  slider.triggerAction (QAbstractSlider::SliderSingleStepSub);
  if (!qFuzzyCompare(slider.value(), 18.54))
    {
    std::cerr << "ctkDoubleSlider:: Step Sub failed" << slider.value() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test setMaximum" << std::endl;
  slider.setMaximum(15.08);
  if (!qFuzzyCompare(slider.maximum(), 15.08) ||
      !qFuzzyCompare(slider.value(), 15.08))
    {
    std::cerr << "ctkDoubleSlider:: setMaximum failed" << slider.value() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test setMaximum" << std::endl;
  slider.setMaximum(5100000.333333);
  if (!qFuzzyCompare(slider.maximum(), 5100000.333333) ||
      !qFuzzyCompare(slider.value(), 15.08))
    {
    std::cerr << "ctkDoubleSlider:: setMaximum failed" << slider.value() << std::endl;
    return EXIT_FAILURE;
    }    
  std::cout << "Test setValue close to previous value" << std::endl;
  slider.setValue(15.081);
  if (!qFuzzyCompare(slider.value(), 15.081))
    {
    std::cerr << "ctkDoubleSlider:: setValue failed" << slider.value() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test setSingleStep" << std::endl;
  slider.setSingleStep(1.);
  if (!qFuzzyCompare(slider.value(),15.081) || 
      !qFuzzyCompare(slider.sliderPosition(), 15.081))
    {
    std::cerr << "ctkDoubleSlider:: setSingleStep failed: val" << slider.value() << " pos:" << slider.sliderPosition() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test setSingleStep " << std::endl;
  slider.setSingleStep(0.01);
  if (!qFuzzyCompare(slider.value(),15.081) || 
      !qFuzzyCompare(slider.sliderPosition(), 15.081))
    {
    std::cerr << "ctkDoubleSlider:: setSingleStep failed: val" << slider.value() << " pos:" << slider.sliderPosition() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test reach max" << std::endl;
  slider.setMaximum(510.333333);
  slider.setValue(1000);
  if (!qFuzzyCompare(slider.maximum(), 510.333333) ||
      !qFuzzyCompare(slider.value(), 510.333333))
    {
    std::cerr << "ctkDoubleSlider::setValue(1000) failed"
              << " val:" << slider.value() 
              << " pos: " << slider.sliderPosition() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Test change range" << std::endl;
  slider.setRange(-500.1, 10.21);
  if (!qFuzzyCompare(slider.maximum(), 10.21) ||
      !qFuzzyCompare(slider.value(), 10.21) ||
      !qFuzzyCompare(slider.sliderPosition(), 10.21))
    {
    std::cerr << "ctkDoubleSlider::setRange(-400.2, 10.21) failed"
              << " val:" << slider.value() 
              << " pos: " << slider.sliderPosition() << std::endl;
    return EXIT_FAILURE;
    }
  slider.setSingleStep(0.15);
  if (!qFuzzyCompare(slider.value(), 10.21) ||
      !qFuzzyCompare(slider.sliderPosition(), 10.21))
    {
    std::cerr << "ctkDoubleSlider::setSingleStep(0.15) failed"
              << " val:" << slider.value() 
              << " pos: " << slider.sliderPosition() << std::endl;
    return EXIT_FAILURE;
    }
  // FIXME 
  // slider.setSliderPosition(10.3333);
  // if (!qFuzzyCompare(slider.value(), 10.3333) ||
  //     !qFuzzyCompare(slider.sliderPosition(), 10.3333))
  //   {
  //   std::cerr << "ctkDoubleSlider::setSliderPosition(10.3333) failed"
  //             << " val:" << slider.value() 
  //             << " pos: " << slider.sliderPosition() << std::endl;
  //   return EXIT_FAILURE;
  //   }
  // slider.setTracking(false);
  // slider.setSliderPosition(10.123456789);
  // if (!qFuzzyCompare(slider.value(), 10.3333) ||
  //     !qFuzzyCompare(slider.sliderPosition(), 10.123456789))
  //   {
  //   std::cerr << "ctkDoubleSlider::setSliderPosition(10.123456789) failed"
  //             << " val:" << slider.value() 
  //             << " pos: " << slider.sliderPosition() << std::endl;
  //   return EXIT_FAILURE;
  //   }
  slider.setPageStep(1.);
  if (slider.pageStep() != 1.)
    {
    std::cerr << "ctkDoubleSlider::setPageStep(1.) failed"
              << " val: " << slider.pageStep() << std::endl;
    return EXIT_FAILURE;
    }
  slider.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(100, &app, SLOT(quit()));
    }
  return app.exec();
}

