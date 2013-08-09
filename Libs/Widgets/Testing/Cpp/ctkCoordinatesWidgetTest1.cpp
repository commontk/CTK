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
#include "ctkCoordinatesWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCoordinatesWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkCoordinatesWidget coordinatesWidget;

  if (coordinatesWidget.dimension() != 3)
    {
    std::cerr << "ctkCoordinatesWidget: bad default dimension" << std::endl;
    return EXIT_FAILURE;
    }

  coordinatesWidget.setDimension(4);

  if (coordinatesWidget.dimension() != 4)
    {
    std::cerr << "ctkCoordinatesWidget::setDimension failed" << std::endl;
    return EXIT_FAILURE;
    }

  coordinatesWidget.setDecimals(4); 

  if (coordinatesWidget.decimals() != 4)
    {
    std::cerr << "ctkCoordinatesWidget::setDecimals failed" << std::endl;
    return EXIT_FAILURE;
    }

  coordinatesWidget.setSingleStep(0.1); 

  if (coordinatesWidget.singleStep() != 0.1)
    {
    std::cerr << "ctkCoordinatesWidget::setSingleStep failed" << std::endl;
    return EXIT_FAILURE;
    }

  coordinatesWidget.setMinimum(-9999.); 

  if (coordinatesWidget.minimum() != -9999.)
    {
    std::cerr << "ctkCoordinatesWidget::setMinimum failed" << std::endl;
    return EXIT_FAILURE;
    }

  coordinatesWidget.setMaximum(9999.); 

  if (coordinatesWidget.maximum() != 9999.)
    {
    std::cerr << "ctkCoordinatesWidget::setMaximum failed" << std::endl;
    return EXIT_FAILURE;
    }

  double const* coordinates = coordinatesWidget.coordinates();

  if (coordinates[0] != 0. ||
      coordinates[1] != 0. ||
      coordinates[2] != 0. ||
      coordinates[3] != 0.)
    {
    std::cerr << "ctkCoordinatesWidget::coordinates() failed"
              << coordinates[0] << " "
              << coordinates[1] << " "
              << coordinates[2] << " "
              << coordinates[3] << " " << std::endl;
    return EXIT_FAILURE;
    }
  
  if (coordinatesWidget.coordinatesAsString() != "0,0,0,0")
    {
    std::cerr << "ctkCoordinatesWidget::coordinatesAsString() failed"
              << coordinatesWidget.coordinatesAsString().toStdString()
              << std::endl;
    return EXIT_FAILURE;
    }
  
  double newCoordinates[4] = {1.0, 10.00012, -541292., 0.4};
  coordinatesWidget.setCoordinates(newCoordinates);
  coordinates = coordinatesWidget.coordinates();
  
  if (coordinates[0] != 1. ||
      coordinates[1] != 10.00012 ||
      coordinates[2] != -9999. ||
      coordinates[3] != 0.4)
    {
    std::cerr << "ctkCoordinatesWidget::coordinates() failed"
              << coordinates[0] << " "
              << coordinates[1] << " "
              << coordinates[2] << " "
              << coordinates[3] << " " << std::endl;
    return EXIT_FAILURE;
    }
  
  if (coordinatesWidget.coordinatesAsString() != "1,10.0001,-9999,0.4")
    {
    std::cerr << "ctkCoordinatesWidget::coordinatesAsString() failed"
              << coordinatesWidget.coordinatesAsString().toStdString() << std::endl;
    return EXIT_FAILURE;
    }
  
  coordinatesWidget.setCoordinatesAsString("1.000000001, -2, 3.01, 40000.01");
  coordinates = coordinatesWidget.coordinates();
  if (!qFuzzyCompare(coordinates[0], 1.000000001) ||
      !qFuzzyCompare(coordinates[1], -2.) ||
      !qFuzzyCompare(coordinates[2], 3.01) ||
      !qFuzzyCompare(coordinates[3], 9999.))
    {
    std::cerr << "ctkCoordinatesWidget::coordinates() failed: "
              << coordinates[0] << " " << qFuzzyCompare(coordinates[0], 1.) << ", "
              << coordinates[1] << " " << qFuzzyCompare(coordinates[1], -2.) << ", "
              << coordinates[2] << " " << qFuzzyCompare(coordinates[2], 3.01) << ", "
              << coordinates[3] << " " << qFuzzyCompare(coordinates[3], 9999.) << ", "
              << std::endl;
    return EXIT_FAILURE;
    }
  
  if (coordinatesWidget.coordinatesAsString() != "1,-2,3.01,9999")
    {
    std::cerr << "ctkCoordinatesWidget::coordinatesAsString() failed"
              << coordinatesWidget.coordinatesAsString().toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  coordinatesWidget.setDimension(0);
  
  if (coordinatesWidget.dimension() != 4)
    {
    std::cerr << "ctkCoordinatesWidget::setDimension() failed: "
              << coordinatesWidget.dimension() << std::endl;
    return EXIT_FAILURE;
    }

  coordinatesWidget.setDimension(3);
  
  if (coordinatesWidget.dimension() != 3)
    {
    std::cerr << "ctkCoordinatesWidget::setDimension() failed: "
              << coordinatesWidget.dimension() << std::endl;
    return EXIT_FAILURE;
    }

  coordinatesWidget.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  
  return app.exec();
}

