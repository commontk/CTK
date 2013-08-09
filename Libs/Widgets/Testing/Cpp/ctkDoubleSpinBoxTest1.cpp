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
#include <QDebug>
#include <QSignalSpy>
#include <QTimer>

// CTK includes
#include "ctkDoubleSpinBox.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkDoubleSpinBoxTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkDoubleSpinBox spinBox;

  // ----------------------------------------
  // Regular SpinBox methods
  // ----------------------------------------

  qDebug()<<"Prefix:";
  spinBox.setPrefix("$");
  if (spinBox.prefix() != "$")
    {
    qDebug()<<"Prefix does not correspond. Got: "<<spinBox.prefix();
    return EXIT_FAILURE;
    }
  spinBox.setValue(3.0);
  if (spinBox.text() != "$3.00")
    {
    qDebug()<<"Text does not correspond. Got: "<<spinBox.text();
    return EXIT_FAILURE;
    }

  qDebug()<<"Suffix:";
  spinBox.setSuffix("#");
  if (spinBox.suffix() != "#")
    {
    qDebug()<<"suffix does not correspond. Got: "<<spinBox.suffix();
    return EXIT_FAILURE;
    }
  spinBox.setValue(8.69);
  if (spinBox.text() != "$8.69#")
    {
    qDebug()<<"Text does not correspond. Got: "<<spinBox.text();
    return EXIT_FAILURE;
    }

  qDebug()<<"cleanText:";
  if (spinBox.cleanText() != "8.69")
    {
    qDebug()<<"cleanText does not correspond. Got: "<<spinBox.cleanText();
    return EXIT_FAILURE;
    }

  qDebug()<<"SingleStep:";
  spinBox.setSingleStep(0.01);
  if (!qFuzzyCompare(spinBox.singleStep(), 0.01))
    {
    qDebug()<<"singleStep does not correspond. Got: "<<spinBox.singleStep();
    return EXIT_FAILURE;
    }
  spinBox.stepUp();
  if (!qFuzzyCompare(spinBox.value(), 8.70))
    {
    qDebug()<<"Value does not correspond. Got: "<<spinBox.value();
    return EXIT_FAILURE;
    }
  spinBox.setSingleStep(1.0);
  spinBox.stepDown();
  if (!qFuzzyCompare(spinBox.displayedValue(), 7.7))
    {
    qDebug()<<"Value does not correspond. Got: "<<spinBox.value();
    return EXIT_FAILURE;
    }

  qDebug()<<"Minimum:";
  spinBox.setMinimum(9.1);
  if (!qFuzzyCompare(spinBox.minimum(), 9.1))
    {
    qDebug()<<"minimum does not correspond. Got: "<<spinBox.minimum();
    return EXIT_FAILURE;
    }
  if (!qFuzzyCompare(spinBox.value(), 9.1))
    {
    qDebug()<<"Value does not correspond. Got: "<<spinBox.value();
    return EXIT_FAILURE;
    }

  qDebug()<<"Maximum:";
  spinBox.setValue(18.34);
  spinBox.setMaximum(15);
  if (!qFuzzyCompare(spinBox.maximum(), 15.0))
    {
    qDebug()<<"maximum does not correspond. Got: "<<spinBox.maximum();
    return EXIT_FAILURE;
    }
  if (!qFuzzyCompare(spinBox.value(), 15.0))
    {
    qDebug()<<"Value does not correspond. Got: "<<spinBox.value();
    return EXIT_FAILURE;
    }

  qDebug()<<"Range:";
  spinBox.setRange(-3.6, 42);
  if (!qFuzzyCompare(spinBox.maximum(), 42)
    || !qFuzzyCompare(spinBox.minimum(), -3.6))
    {
    qDebug()<<"Range does not correspond. Got: "
      <<spinBox.minimum()<<" : "<<spinBox.minimum();
    return EXIT_FAILURE;
    }

  qDebug()<<"Decimals:";
  spinBox.setDecimals(6);
  if (spinBox.decimals() != 6)
    {
    qDebug()<<"Decimals does not correspond. Got: "<<spinBox.cleanText();
    return EXIT_FAILURE;
    }
  if (spinBox.cleanText() != "15.000000")
    {
    qDebug()<<"Decimals does not correspond. Got: "<<spinBox.cleanText();
    return EXIT_FAILURE;
    }

  qDebug()<<"Round:";
  spinBox.setDecimals(3);
  double roundedValue = spinBox.round(6.67899156);
  if (!qFuzzyCompare(roundedValue, 6.679))
    {
    qDebug()<<"Round does not correspond. Got: "<<roundedValue;
    return EXIT_FAILURE;
    }

  QSignalSpy spy(&spinBox, SIGNAL(valueChanged(double)));

  qDebug()<<"SetValue:";
  spinBox.setValue(28.36);
  if (!qFuzzyCompare(spinBox.value(), 28.36))
    {
    qDebug()<<"setValue does not correspond. Got: "<<spinBox.value() ;
    return EXIT_FAILURE;
    }

  spinBox.setValue(28.366917352);
  if (!qFuzzyCompare(spinBox.value(), 28.366917352))
    {
    qDebug()<<"setValue does not correspond. Got: "<<spinBox.value() ;
    return EXIT_FAILURE;
    }

  qDebug()<<"SetValueIfDifferent:";
  spinBox.setValueIfDifferent(33.312587);
  if (!qFuzzyCompare(spinBox.value(), 33.312587))
    {
    qDebug()<<"setValueIfDifferent does not correspond. Got: "<<spinBox.value() ;
    return EXIT_FAILURE;
    }

  spinBox.setValueIfDifferent(33.312960134);
  if (!qFuzzyCompare(spinBox.value(), 33.312960134))
    {
    qDebug()<<"setValueIfDifferent does not correspond. Got: "<<spinBox.value() ;
    return EXIT_FAILURE;
    }

  if (spy.count() != 4)
    {
    qDebug()<<"spy got wrong number of signal sent : "<<spy.count() ;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  spinBox.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(100, &app, SLOT(quit()));
    }

  return app.exec();
}
