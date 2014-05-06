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
#include <QTimer>

// CTK includes
#include "ctkRangeWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

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
  sliderSpinBox.setValues(32.12,75.38);
  sliderSpinBox.setMinimum(10.0123);
  sliderSpinBox.setMaximum(2050.9876);
  sliderSpinBox.setSingleStep(0.1);
  sliderSpinBox.setMaximumValue(77.777);
  sliderSpinBox.setMinimum(80.5678);

  std::cout << "4" << std::endl;
  sliderSpinBox.setTickInterval(0.1);

  if (!qFuzzyCompare(sliderSpinBox.tickInterval(), 0.1) ||
      !checkSlider(sliderSpinBox, 80.5678, 80.5678, 80.5678, 2050.9876))
    {
    std::cerr << "ctkRangeWidget:: 6) setTickInterval"
              << sliderSpinBox.minimum() << " "
              << sliderSpinBox.minimumValue() << " "
              << sliderSpinBox.maximumValue() << " "
              << sliderSpinBox.maximum() << std::endl;
    return EXIT_FAILURE;
    }
    std::cout << "5" << std::endl;

  if (!qFuzzyCompare(sliderSpinBox.tickInterval(), 0.1) |
      !checkSlider(sliderSpinBox, 80.5678, 80.5678, 80.5678, 2050.9876))
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
      !checkSlider(sliderSpinBox, 80.5678, 80.5678, 80.5678, 2050.9876))
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
      !checkSlider(sliderSpinBox, 80.5678, 80.5678, 80.5678, 2050.9876))
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
      !checkSlider(sliderSpinBox, 80.5678, 80.5678, 80.5678, 2050.9876))
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
      !checkSlider(sliderSpinBox, 80.5678, 80.5678, 80.5678, 2050.9876))
    {
    std::cerr << "ctkRangeWidget:: 9) setSuffix"
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

