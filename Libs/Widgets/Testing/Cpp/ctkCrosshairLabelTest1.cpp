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
#include "ctkCrosshairLabel.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCrosshairLabelTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkCrosshairLabel crosshair;

  // check default values
  if (!crosshair.showCrosshair() ||
      crosshair.crosshairPen().color()
      != crosshair.palette().color(QPalette::Highlight) ||
      crosshair.crosshairPen().width() != 0 ||
      crosshair.crosshairPen().joinStyle() != Qt::MiterJoin ||
      crosshair.crosshairType() != ctkCrosshairLabel::SimpleCrosshair ||
      crosshair.marginColor() != crosshair.palette().color(QPalette::Window) ||
      crosshair.bullsEyeWidth() != 15)
    {
    std::cerr << "ctkCrosshairLabel: Wrong default values. " << std::endl
              << " " << crosshair.showCrosshair()
              << " " << qPrintable(crosshair.crosshairPen().color().name())
              << " " << crosshair.crosshairPen().width()
              << " " << static_cast<int>(crosshair.crosshairPen().joinStyle())
              << " " << crosshair.crosshairType()
              << " " << qPrintable(crosshair.marginColor().name())
              << " " << crosshair.bullsEyeWidth() << std::endl;
    return EXIT_FAILURE;
    }

  // Show crosshair
  crosshair.setShowCrosshair(false);
  if (crosshair.showCrosshair())
    {
    std::cerr << "ctkCrosshairLabel:setShowCrosshair failed. "
              << crosshair.showCrosshair() << std::endl;
    return EXIT_FAILURE;
    }
  crosshair.setShowCrosshair(true);

  // Crosshair pen
  QPen crosshairPen(Qt::yellow);
  crosshairPen.setJoinStyle(Qt::MiterJoin);
  crosshair.setCrosshairPen(crosshairPen);
  if (crosshair.crosshairPen() != crosshairPen)
    {
    std::cerr << "ctkCrosshairLabel:setCrosshairPen failed. "
              << qPrintable(crosshair.crosshairPen().color().name()) << std::endl;
    return EXIT_FAILURE;
    }

  // Crosshair type
  crosshair.setCrosshairType(ctkCrosshairLabel::BullsEyeCrosshair);
  if (crosshair.crosshairType() != ctkCrosshairLabel::BullsEyeCrosshair)
    {
    std::cerr << "ctkCrosshairLabel:setCrosshairType failed. "
              << crosshair.crosshairType() << std::endl;
    return EXIT_FAILURE;
    }

  // Margin color - invalid input
  QColor transparentBlue(Qt::blue);
  transparentBlue.setAlphaF(0.25);
  QColor origColor = crosshair.marginColor();
  crosshair.setMarginColor(QColor());
  if (crosshair.marginColor() != origColor)
    {
    std::cerr << "ctkCrosshairLabel:setMarginColor failed - invalid input. "
              << qPrintable(crosshair.marginColor().name()) << std::endl;
    return EXIT_FAILURE;
    }

  // Margin color - should ignore alpha channel
  crosshair.setMarginColor(transparentBlue);
  if (crosshair.marginColor() != Qt::blue)
    {
      {
      std::cerr << "ctkCrosshairLabel:setMarginColor failed - valid input. "
                << qPrintable(crosshair.marginColor().name()) << std::endl;
      return EXIT_FAILURE;
      }
    }

  // Bulls eye width
  crosshair.setBullsEyeWidth(0);
  if (crosshair.bullsEyeWidth() != 0)
    {
    std::cerr << "ctkCrosshairLabel:setBullsEyeWidth failed. "
              << crosshair.bullsEyeWidth() << std::endl;
    return EXIT_FAILURE;
    }

  crosshair.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();

}

