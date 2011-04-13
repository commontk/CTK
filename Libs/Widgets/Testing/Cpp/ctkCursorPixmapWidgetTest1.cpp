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

// CTK includes
#include "ctkCursorPixmapWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCursorPixmapWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkCursorPixmapWidget cursor;

  // check default values
  if (!cursor.showCursor() ||
      cursor.cursorColor() != cursor.palette().color(QPalette::Highlight) ||
      cursor.cursorType() != ctkCursorPixmapWidget::CrossHairCursor ||
      cursor.marginColor() != cursor.palette().color(QPalette::Window) ||
      cursor.bullsEyeWidth() != 15)
    {
    std::cerr << "ctkCursorPixmapWidget: Wrong default values. " << std::endl
              << " " << cursor.showCursor()
              << " " << qPrintable(cursor.cursorColor().name())
              << " " << cursor.cursorType()
              << " " << qPrintable(cursor.marginColor().name())
              << " " << cursor.bullsEyeWidth() << std::endl;
    return EXIT_FAILURE;
    }

  // Show cursor
  cursor.setShowCursor(false);
  if (cursor.showCursor())
    {
    std::cerr << "ctkCursorPixmapWidget:setShowCursor failed. "
              << cursor.showCursor() << std::endl;
    return EXIT_FAILURE;
    }
  cursor.setShowCursor(true);

  // Cursor color
  QColor transparentBlue(Qt::blue);
  transparentBlue.setAlphaF(0.25);
  cursor.setCursorColor(transparentBlue);
  if (cursor.cursorColor() != transparentBlue)
    {
    std::cerr << "ctkCursorPixmapWidget:setCursorColor failed. "
              << qPrintable(cursor.cursorColor().name()) << std::endl;
    return EXIT_FAILURE;
    }

  // Cursor type
  cursor.setCursorType(ctkCursorPixmapWidget::BullsEyeCursor);
  if (cursor.cursorType() != ctkCursorPixmapWidget::BullsEyeCursor)
    {
    std::cerr << "ctkCursorPixmapWidget:setCursorType failed. "
              << cursor.cursorType() << std::endl;
    return EXIT_FAILURE;
    }

  // Margin color - invalid input
  QColor origColor = cursor.marginColor();
  cursor.setMarginColor(QColor());
  if (cursor.marginColor() != origColor)
    {
    std::cerr << "ctkCursorPixmapWidget:setMarginColor failed - invalid input. "
              << qPrintable(cursor.marginColor().name()) << std::endl;
    return EXIT_FAILURE;
    }

  // Margin color - valid input
  cursor.setMarginColor(transparentBlue);
  if (cursor.marginColor() != transparentBlue)
    {
      {
      std::cerr << "ctkCursorPixmapWidget:setMarginColor failed - valid input. "
                << qPrintable(cursor.marginColor().name()) << std::endl;
      return EXIT_FAILURE;
      }
    }

  // Bulls eye width
  cursor.setBullsEyeWidth(0);
  if (cursor.bullsEyeWidth() != 0)
    {
    std::cerr << "ctkCursorPixmapWidget:setBullsEyeWidth failed. "
              << cursor.bullsEyeWidth() << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

