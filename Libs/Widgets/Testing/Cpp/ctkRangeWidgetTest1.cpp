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
#include <QTimer>

// CTK includes
#include "ctkRangeWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
bool checkSlider(const ctkRangeWidget& slider)
{
  return slider.minimum()  <= slider.minimumValue() &&
    slider.minimumValue() <= slider.maximumValue() &&
    slider.maximumValue() <= slider.maximum();
}

//-----------------------------------------------------------------------------
bool checkSlider(const ctkRangeWidget& slider,
                 double min, double minVal, double maxVal, double max)
{
  return qFuzzyCompare(slider.minimum(), min) &&
    qFuzzyCompare(slider.minimumValue(), minVal) &&
    qFuzzyCompare(slider.maximumValue(), maxVal) &&
    qFuzzyCompare(slider.maximum(), max);
}
//-----------------------------------------------------------------------------
int ctkRangeWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkRangeWidget sliderSpinBox;

  if (!checkSlider(sliderSpinBox))
    {
    std::cerr << "ctkRangeSlider:: 0) "
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }
  // the first part of the tests infer 2 decimals
  if (sliderSpinBox.decimals() != 2)
    {
    std::cerr << "ctkRangeWidget::decimals default value failed."
              << sliderSpinBox.decimals() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setValues(32.12,75.38);
  if (!checkSlider(sliderSpinBox, 0., 32.12, 75.38, 99.))
    {
    std::cerr << "ctkRangeWidget:: 1) setValues"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setMinimum(10.0123);

  if (!checkSlider(sliderSpinBox, 10.01, 32.12, 75.38, 99.))
    {
    std::cerr << "ctkRangeWidget:: 2) setMinimum "
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }
  
  sliderSpinBox.setMaximum(2050.9876);

  if (!checkSlider(sliderSpinBox, 10.01, 32.12, 75.38, 2050.99))
    {
    std::cerr << "ctkRangeWidget:: 3) setMaximum "
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "1" << std::endl;
  sliderSpinBox.setSingleStep(0.1);

  if (!qFuzzyCompare(sliderSpinBox.singleStep(), 0.1) || 
      !checkSlider(sliderSpinBox, 10.01, 32.12, 75.38, 2050.99))
    {
    std::cerr << "ctkRangeWidget:: 4) SetSingleStep"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "2" << std::endl;
  sliderSpinBox.setDecimals(1);

  if (sliderSpinBox.decimals() != 1 || 
      !checkSlider(sliderSpinBox, 10.0, 32.1, 75.4, 2051.0))
    {
    std::cerr << "ctkRangeWidget:: 5) SetDecimals"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "3" << std::endl;
  sliderSpinBox.setMaximumValue(77.777);

  if (!checkSlider(sliderSpinBox, 10.0, 32.1, 77.8, 2051.0))
    {
    std::cerr << "ctkRangeWidget:: 6) SetMaximumValue"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "4" << std::endl;
  sliderSpinBox.setTickInterval(0.1);

  if (!qFuzzyCompare(sliderSpinBox.tickInterval(), 0.1) || 
      !checkSlider(sliderSpinBox, 10.0, 32.1, 77.8, 2051.0))
    {
    std::cerr << "ctkRangeWidget:: 6) setTickInterval"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }
    std::cout << "5" << std::endl;
  sliderSpinBox.setMinimum(80.5678);

  if (!qFuzzyCompare(sliderSpinBox.tickInterval(), 0.1) || 
      !checkSlider(sliderSpinBox, 80.6, 80.6, 80.6, 2051.0))
    {
    std::cerr << "ctkRangeWidget:: 6) setMinimum"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "6" << std::endl;
  //sliderSpinBox.reset();
  sliderSpinBox.setSpinBoxAlignment(Qt::AlignRight);

  if (sliderSpinBox.spinBoxAlignment() != Qt::AlignRight || 
      !checkSlider(sliderSpinBox, 80.6, 80.6, 80.6, 2051.0))
    {
    std::cerr << "ctkRangeWidget:: 7) setSpinBoxAlignment"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "7" << std::endl;
  sliderSpinBox.setAutoSpinBoxWidth(false);

  if (sliderSpinBox.isAutoSpinBoxWidth() != false || 
      !checkSlider(sliderSpinBox, 80.6, 80.6, 80.6, 2051.0))
    {
    std::cerr << "ctkRangeWidget:: 8) setAutoSpinBoxWidth"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "8" << std::endl;
  sliderSpinBox.setPrefix("$");

  if (sliderSpinBox.prefix() != "$" || 
      !checkSlider(sliderSpinBox, 80.6, 80.6, 80.6, 2051.0))
    {
    std::cerr << "ctkRangeWidget:: 8) setPrefix"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "9" << std::endl;
  sliderSpinBox.setSuffix("mm");

  if (sliderSpinBox.suffix() != "mm" || 
      !checkSlider(sliderSpinBox, 80.6, 80.6, 80.6, 2051.0))
    {
    std::cerr << "ctkRangeWidget:: 9) setSuffix"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "10" << std::endl;
  sliderSpinBox.setDecimals(0);

  if (!checkSlider(sliderSpinBox, 81., 81., 81., 2051.))
    {
    std::cerr << "ctkRangeWidget:: 10) setDecimals"
              << sliderSpinBox.minimum() << " " 
              << sliderSpinBox.minimumValue() << " " 
              << sliderSpinBox.maximumValue() << " " 
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setSymmetricMoves(true);
  
  if (sliderSpinBox.symmetricMoves() != true)
    {
    std::cerr << "ctkRangeWidget::setSymmetricMoves failed" << std::endl;
    return EXIT_FAILURE;
    }
  // FIXME check that the correct signals are sent.

  sliderSpinBox.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();

}

