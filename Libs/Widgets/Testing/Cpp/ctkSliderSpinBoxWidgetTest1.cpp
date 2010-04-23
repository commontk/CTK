/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// Qt includes
#include <QApplication>

// CTK includes
#include "ctkSliderSpinBoxWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkSliderSpinBoxWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkSliderSpinBoxWidget sliderSpinBox;

  // the first part of the tests infer 2 decimals
  if (sliderSpinBox.decimals() != 2)
    {
    std::cerr << "ctkSliderSpinBoxWidget::decimals default value failed."
              << sliderSpinBox.decimals() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setValue(32.12);

  if (!qFuzzyCompare(sliderSpinBox.value(), 32.12))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setValue failed."
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setMinimum(10.0123);

  if (!qFuzzyCompare(sliderSpinBox.minimum(), 10.01) || 
      !qFuzzyCompare(sliderSpinBox.value(), 32.12))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setMinimum failed."
              << sliderSpinBox.minimum() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }
  
  sliderSpinBox.setMaximum(2050.9876);

  if (!qFuzzyCompare(sliderSpinBox.maximum(), 2050.99) || 
      !qFuzzyCompare(sliderSpinBox.value(), 32.12))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setMaximum failed."
              << sliderSpinBox.maximum() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setSingleStep(0.1);

  if (!qFuzzyCompare(sliderSpinBox.singleStep(), 0.1) || 
      !qFuzzyCompare(sliderSpinBox.value(), 32.12))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setMaximum failed."
              << sliderSpinBox.singleStep() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setDecimals(1);

  if (sliderSpinBox.decimals() != 1 || 
      !qFuzzyCompare(sliderSpinBox.value(), 32.1))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setDecimals failed."
              << sliderSpinBox.decimals() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }  

  sliderSpinBox.setValue(77.777);

  if (!qFuzzyCompare(sliderSpinBox.value(), 77.8))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setValue failed."
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }  

  sliderSpinBox.setTickInterval(0.1);

  if (!qFuzzyCompare(sliderSpinBox.tickInterval(), 0.1) || 
      !qFuzzyCompare(sliderSpinBox.value(), 77.8))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setTickInterval failed."
              << sliderSpinBox.tickInterval() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }
  
  sliderSpinBox.setMinimum(80.5678);

  if (!qFuzzyCompare(sliderSpinBox.minimum(), 80.6) || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setMinimum failed."
              << sliderSpinBox.minimum() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.reset();

  if (!qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderSpinBoxWidget::reset failed."
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }
  
  sliderSpinBox.setSpinBoxAlignment(Qt::AlignRight);

  if (sliderSpinBox.spinBoxAlignment() != Qt::AlignRight || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setMinimum failed."
              << sliderSpinBox.spinBoxAlignment() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setAutoSpinBoxWidth(false);

  if (sliderSpinBox.isAutoSpinBoxWidth() != false || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setMinimum failed."
              << sliderSpinBox.isAutoSpinBoxWidth() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setPrefix("$");

  if (sliderSpinBox.prefix() != "$" || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setPrefix failed."
              << sliderSpinBox.prefix().toLatin1().data() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setSuffix("mm");

  if (sliderSpinBox.suffix() != "mm" || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.6))
    {
    std::cerr << "ctkSliderSpinBoxWidget::setSuffix failed."
              << sliderSpinBox.suffix().toLatin1().data() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }


  // FIXME check that the correct signals are sent.
  return EXIT_SUCCESS;
}

