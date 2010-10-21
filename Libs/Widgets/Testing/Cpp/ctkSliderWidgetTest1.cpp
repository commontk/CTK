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

// CTK includes
#include "ctkSliderWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkSliderWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkSliderWidget sliderSpinBox;

  // the first part of the tests infer 2 decimals
  if (sliderSpinBox.decimals() != 2)
    {
    std::cerr << "ctkSliderWidget::decimals default value failed."
              << sliderSpinBox.decimals() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setValue(32.12);

  if (!qFuzzyCompare(sliderSpinBox.value(), 32.12))
    {
    std::cerr << "ctkSliderWidget::setValue failed."
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setMinimum(10.0123);

  if (!qFuzzyCompare(sliderSpinBox.minimum(), 10.01) || 
      !qFuzzyCompare(sliderSpinBox.value(), 32.12))
    {
    std::cerr << "ctkSliderWidget::setMinimum failed."
              << sliderSpinBox.minimum() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }
  
  sliderSpinBox.setMaximum(2050.9876);

  if (!qFuzzyCompare(sliderSpinBox.maximum(), 2050.99) || 
      !qFuzzyCompare(sliderSpinBox.value(), 32.12))
    {
    std::cerr << "ctkSliderWidget::setMaximum failed."
              << sliderSpinBox.maximum() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setSingleStep(0.1);

  if (!qFuzzyCompare(sliderSpinBox.singleStep(), 0.1) || 
      !qFuzzyCompare(sliderSpinBox.value(), 32.12))
    {
    std::cerr << "ctkSliderWidget::setSingleStep failed."
              << sliderSpinBox.singleStep() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setDecimals(1);

  if (sliderSpinBox.decimals() != 1 || 
      !qFuzzyCompare(sliderSpinBox.value(), 32.1) ||
      !qFuzzyCompare(sliderSpinBox.minimum(), 10.0) ||
      !qFuzzyCompare(sliderSpinBox.maximum(), 2051.0))
    {
    std::cerr << "ctkSliderWidget::setDecimals failed."
              << sliderSpinBox.decimals() << " "
              << sliderSpinBox.value() << " " 
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }  

  sliderSpinBox.setValue(77.777);

  if (!qFuzzyCompare(sliderSpinBox.value(), 77.8))
    {
    std::cerr << "ctkSliderWidget::setValue failed."
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }  

  sliderSpinBox.setTickInterval(0.1);

  if (!qFuzzyCompare(sliderSpinBox.tickInterval(), 0.1) || 
      !qFuzzyCompare(sliderSpinBox.value(), 77.8))
    {
    std::cerr << "ctkSliderWidget::setTickInterval failed."
              << sliderSpinBox.tickInterval() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }
  
  sliderSpinBox.setMinimum(80.5678);

  if (!qFuzzyCompare(sliderSpinBox.minimum(), 80.6) || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderWidget::setMinimum failed."
              << sliderSpinBox.minimum() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.reset();

  if (!qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderWidget::reset failed."
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }
  
  sliderSpinBox.setSpinBoxAlignment(Qt::AlignRight);

  if (sliderSpinBox.spinBoxAlignment() != Qt::AlignRight || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderWidget::setSpinBoxAlignment failed."
              << sliderSpinBox.spinBoxAlignment() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setAutoSpinBoxWidth(false);

  if (sliderSpinBox.isAutoSpinBoxWidth() != false || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderWidget::setAutoSpinBoxWidth failed."
              << sliderSpinBox.isAutoSpinBoxWidth() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setPrefix("$");

  if (sliderSpinBox.prefix() != "$" || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderWidget::setPrefix failed."
              << sliderSpinBox.prefix().toLatin1().data() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setSuffix("mm");

  if (sliderSpinBox.suffix() != "mm" || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderWidget::setSuffix failed."
              << sliderSpinBox.suffix().toLatin1().data() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setDecimals(0);

  if (!qFuzzyCompare(sliderSpinBox.value(), 81) ||
      !qFuzzyCompare(sliderSpinBox.minimum(), 81) ||
      !qFuzzyCompare(sliderSpinBox.maximum(), 2051))
    {
    std::cerr << "ctkSliderWidget::setDecimals failed."
              << sliderSpinBox.value() << " " 
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }

  // FIXME check that the correct signals are sent.
  return EXIT_SUCCESS;
}

