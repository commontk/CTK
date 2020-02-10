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
#include "ctkSliderWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkSliderWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkSliderWidget sliderSpinBox;
  sliderSpinBox.setValue(32.12);
  sliderSpinBox.setMinimum(10.0123);
  sliderSpinBox.setMaximum(2050.9876);
  sliderSpinBox.setSingleStep(0.1);
  sliderSpinBox.setDecimals(1);
  sliderSpinBox.setValue(77.777);
  sliderSpinBox.setTickInterval(0.1);
  sliderSpinBox.setMinimum(80.5678);
  sliderSpinBox.reset();
  sliderSpinBox.setSpinBoxAlignment(Qt::AlignRight);

  if (sliderSpinBox.spinBoxAlignment() != Qt::AlignRight || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.5678))
    {
    std::cerr << "ctkSliderWidget::setSpinBoxAlignment failed."
              << sliderSpinBox.spinBoxAlignment() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setSynchronizeSiblings(ctkSliderWidget::NoSynchronize);

  if (sliderSpinBox.synchronizeSiblings().testFlag(
      ctkSliderWidget::SynchronizeWidth) != false ||
      !qFuzzyCompare(sliderSpinBox.value(), 80.5678))
    {
    std::cerr << "ctkSliderWidget::setAutoSpinBoxWidth failed."
              << sliderSpinBox.synchronizeSiblings() << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setPrefix("$");

  if (sliderSpinBox.prefix() != "$" || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.5678))
    {
    std::cerr << "ctkSliderWidget::setPrefix failed."
              << qPrintable(sliderSpinBox.prefix()) << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setSuffix("mm");

  if (sliderSpinBox.suffix() != "mm" || 
      !qFuzzyCompare(sliderSpinBox.value(), 80.5678))
    {
    std::cerr << "ctkSliderWidget::setSuffix failed."
              << qPrintable(sliderSpinBox.suffix()) << " "
              << sliderSpinBox.value() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.setPageStep(1.);
  if (sliderSpinBox.pageStep() != 1.)
    {
    std::cerr << "ctkSliderWidget::setPageStep(1.) failed"
              << " val: " << sliderSpinBox.pageStep() << std::endl;
    return EXIT_FAILURE;
    }

  sliderSpinBox.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(100, &app, SLOT(quit()));
    }
  return app.exec();
}

