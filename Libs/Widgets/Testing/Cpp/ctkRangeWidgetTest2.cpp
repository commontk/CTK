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
int ctkRangeWidgetTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkRangeWidget sliderSpinBox;
  sliderSpinBox.setDecimals(2);
  sliderSpinBox.setRange(0, 99);
  
  sliderSpinBox.setValues(1., 10.);
  sliderSpinBox.setRange(-10., -0.10);

  sliderSpinBox.setMaximum(-11.);
  
  sliderSpinBox.setMinimum(101.);
  
  sliderSpinBox.setValues(0., 1000.);
  
  sliderSpinBox.setRange(-2002, 2002);

  sliderSpinBox.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();

}

