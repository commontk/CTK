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
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QTimer>

// CTK includes
#include "ctkMatrixWidget.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkMatrixWidgetTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget topLevel;
  QHBoxLayout* layout = new QHBoxLayout(&topLevel);
  ctkMatrixWidget matrixWidget;
  // 4x4 by default, if not anymore, change the documentation
  if (matrixWidget.rowCount() != 4 ||
      matrixWidget.columnCount() != 4)
    {
    std::cerr << "Default constructor doesn't create a 4x4 matrix" << std::endl;
    return EXIT_FAILURE;
    }

  for (int i = 0; i != 4; ++i)
    {
    for (int j = 0; j != 4; ++j)
      {
      if ((i == j && matrixWidget.value(i,j) != 1.) ||
          (i != j && matrixWidget.value(i,j) != 0.))
        {
        std::cerr << "Not an identity matrix: (" << i << "," << j << ") = "
                  << matrixWidget.value(i,j) << std::endl;
        return EXIT_FAILURE;
        }
      }
    }
  matrixWidget.setValue(2,3, 15.352);

  if (matrixWidget.value(2,3) != 15.352)
    {
    std::cerr << "ctkMatrixWidget::setValue() failed: "
              << matrixWidget.value(2,3) << std::endl;
    return EXIT_FAILURE;
    }

  // resize the matrix:
  matrixWidget.setRowCount(8);

  if (matrixWidget.rowCount() != 8 ||
      matrixWidget.columnCount() != 4)
    {
    std::cerr << "ctkMatrixWidget::setRowCount() failed "
              << matrixWidget.rowCount() << std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.setColumnCount(6);

  if (matrixWidget.rowCount() != 8 ||
      matrixWidget.columnCount() != 6)
    {
    std::cerr << "ctkMatrixWidget::setColumnCount() failed: "
              << matrixWidget.columnCount() << std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.setEditable(false);

  if (matrixWidget.isEditable())
    {
    std::cerr << "ctkMatrixWidget::setEditable() failed" << std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.setMinimum(0.5);

  if (matrixWidget.minimum() != 0.5 ||
      matrixWidget.value(1,0) != 0.5 )
    {
    std::cerr << "ctkMatrixWidget::setMinimum() failed:"
              << matrixWidget.value(1,0) << std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.setMaximum(0.7);

  if (matrixWidget.maximum() != 0.7 ||
      matrixWidget.value(1,1) != 0.7 )
    {
    std::cerr << "ctkMatrixWidget::setMaximum() failed:"
              << matrixWidget.value(1,1) << std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.setMinimum(40.);

  if (matrixWidget.minimum() != 40. ||
      matrixWidget.maximum() != 40. ||
      matrixWidget.value(2,2) != 40. ||
      matrixWidget.value(1,2) != 40.)
    {
    std::cerr << "ctkMatrixWidget::setMinimum() failed:"
              << matrixWidget.maximum() << std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.setRowCount(3);
  matrixWidget.setColumnCount(3);

  if (matrixWidget.minimum() != 40. ||
      matrixWidget.maximum() != 40. ||
      matrixWidget.value(2,2) != 40. ||
      matrixWidget.value(1,2) != 40.)
    {
    std::cerr << "ctkMatrixWidget::setMinimum() failed:"
              << matrixWidget.maximum() << std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.setRange(250., 10.);

  if (matrixWidget.minimum() != 10. ||
      matrixWidget.maximum() != 250. ||
      matrixWidget.value(0,0) != 40. ||
      matrixWidget.value(2,1) != 40.)
    {
    std::cerr << "ctkMatrixWidget::setRange() failed:"
              << matrixWidget.minimum() << " "
              << matrixWidget.maximum() << std::endl;
    return EXIT_FAILURE;
    }

  QVector<double> items;
  items.push_back(200.);
  items.push_back(201.);
  items.push_back(202.);
  items.push_back(203.);
  items.push_back(204.);
  items.push_back(205.);
  items.push_back(206.);
  items.push_back(207.);
  items.push_back(208.);
  matrixWidget.setValues(items);

  if (matrixWidget.value(2,1) != 207.)
    {
    std::cerr << "ctkMatrixWidget::setValues() failed:"
              << matrixWidget.value(2,1) << std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.identity();
  if (matrixWidget.value(0,0) != 10. ||
      matrixWidget.value(1,0) != 10.)
    {
    std::cerr << "ctkMatrixWidget::identity() failed:"
              << matrixWidget.value(0,0) << " "
              << matrixWidget.value(1,0) <<std::endl;
    return EXIT_FAILURE;
    }

  matrixWidget.setRange(-100, 100.);
  matrixWidget.identity();

  if (matrixWidget.value(1,1) != 1. ||
      matrixWidget.value(0,2) != 0.)
    {
    std::cerr << "ctkMatrixWidget::identity() failed:"
              << matrixWidget.value(1,1) << " "
              << matrixWidget.value(0,2) <<std::endl;
    return EXIT_FAILURE;
    }
 
  matrixWidget.setSingleStep(1.);
  if (matrixWidget.singleStep() != 1.)
    {
    std::cerr << "ctkMatrixWidget::setSingleStep() failed:"
              << matrixWidget.singleStep() << std::endl;
    }

  matrixWidget.setDecimals(5);
  if (matrixWidget.decimals() != 5)
    {
    std::cerr << "ctkMatrixWidget::setDecimals() failed:"
              << matrixWidget.decimals() << std::endl;
    }


  matrixWidget.setDecimals(-1);
  if (matrixWidget.decimals() != 0)
    {
    std::cerr << "ctkMatrixWidget::setDecimals() failed:"
              << matrixWidget.decimals() << std::endl;
    }

  matrixWidget.setEditable(true);

  layout->addWidget(&matrixWidget);
  topLevel.setLayout(layout);
  topLevel.show();
  topLevel.resize(300, 401);

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
