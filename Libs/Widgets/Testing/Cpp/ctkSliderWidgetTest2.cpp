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
#include "ctkSliderWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkSliderWidgetTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkSliderWidget sliderSpinBox;

  QSignalSpy spy(&sliderSpinBox, SIGNAL(valueChanged(double)));

  QObject::connect(&sliderSpinBox, SIGNAL(valueChanged(double)),
                   &sliderSpinBox, SLOT(setValue(double)));

  sliderSpinBox.setMinimum(150);

  if (spy.count() != 1)
    {
    std::cout << "ctkSliderWidget::valueChanged not fired: " << spy.count();
    return EXIT_FAILURE;
    }
  spy.clear();
  sliderSpinBox.setMaximum(1500);

  if (spy.count() != 0)
    {
    std::cout << "ctkSliderWidget::valueChanged was fired: " << spy.count();
    return EXIT_FAILURE;
    }

  sliderSpinBox.setPopupSlider(true);
  sliderSpinBox.setPopupSlider(false);

  sliderSpinBox.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(100, &app, SLOT(quit()));
    }
  return app.exec();
}

