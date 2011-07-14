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

// Qt includse
#include <QApplication>
#include <QTimer>

// CTK includes
#include "ctkMaterialPropertyWidget.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkMaterialPropertyWidgetTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkMaterialPropertyWidget materialWidget;
  if (!materialWidget.isColorVisible() ||
      !materialWidget.isOpacityVisible() ||
      !materialWidget.isBackfaceCullingVisible())
    {
    std::cout << "ctkMaterialPropertyWidget::ctkMaterialPropertyWidget(), "
              << "wrong default values" << std::endl;
    return EXIT_FAILURE;
    }

  // here for code coverage
  materialWidget.setColorVisible(true);
  materialWidget.setOpacityVisible(true);
  materialWidget.setBackfaceCullingVisible(true);

  materialWidget.setColorVisible(false);
  if (materialWidget.isColorVisible())
    {
    std::cout << "ctkMaterialPropertyWidget::setColorVisible failed"
              << std::endl;
    return EXIT_FAILURE;
    }
  materialWidget.setOpacityVisible(false);
  if (materialWidget.isOpacityVisible())
    {
    std::cout << "ctkMaterialPropertyWidget::setColorVisible failed"
              << std::endl;
    return EXIT_FAILURE;
    }
  materialWidget.setBackfaceCullingVisible(false);
  if (materialWidget.isBackfaceCullingVisible())
    {
    std::cout << "ctkMaterialPropertyWidget::setColorVisible failed"
              << std::endl;
    return EXIT_FAILURE;
    }
  
  materialWidget.setColorVisible(true);
  materialWidget.setOpacityVisible(true);
  materialWidget.setBackfaceCullingVisible(true);
  
  materialWidget.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

