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
#include <QIcon>
#include <QSignalSpy>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkCursorPixmapWidget.h"
#include "ctkCommandLineParser.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
bool imageCompare(ctkCursorPixmapWidget& cursor, QString baselineDirectory,
                  QString baselineFilename)
{
  QImage output = QPixmap::grabWidget(&cursor).toImage();
  QImage baseline(baselineDirectory + "/" + baselineFilename);
  return output == baseline;
}

//-----------------------------------------------------------------------------
// (Used to create baselines, not during testing).
void imageSave(ctkCursorPixmapWidget& cursor, QString baselineDirectory,
               QString baselineFilename)
{
  QImage output = QPixmap::grabWidget(&cursor).toImage();
  output.save(baselineDirectory + "/" + baselineFilename);
}

//-----------------------------------------------------------------------------
bool runBaselineTest(int time, QApplication& app, ctkCursorPixmapWidget& cursor,
                     QString baselineDirectory, QString baselineFilename,
                     QString errorMessage)
{
  QTimer::singleShot(time, &app, SLOT(quit()));
  if (app.exec() == EXIT_FAILURE)
    {
    std::cerr << "ctkCursorPixmapWidget exec failed "
        << qPrintable(errorMessage) << std::endl;
    return false;
    }
  if (!imageCompare(cursor, baselineDirectory, baselineFilename))
    {
    std::cerr << "ctkCursorPixmapWidget baseline comparison failed when "
              << qPrintable(errorMessage) << "." << std::endl;
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
int ctkCursorPixmapWidgetTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  // Command line parser
  ctkCommandLineParser parser;
  parser.addArgument("", "-D", QVariant::String);
  parser.addArgument("", "-V", QVariant::String);
  parser.addArgument("", "-I", QVariant::String);
  bool ok = false;
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(app.arguments(), &ok);
  if (!ok)
    {
    std::cerr << qPrintable(parser.errorString()) << std::endl;
    return EXIT_FAILURE;
    }
  QString dataDirectory = parsedArgs["-D"].toString();
  QString baselineDirectory = parsedArgs["-V"].toString();
  bool interactive = parsedArgs["-I"].toBool();

  // The remainder is interactive, so abort now if command line args specify otherwise
  if (interactive)
    {
    return EXIT_SUCCESS;
    }

  // Create the cursor widget
  ctkCursorPixmapWidget cursor;
  QPen cursorPen(Qt::yellow);
  cursorPen.setJoinStyle(Qt::MiterJoin);
  cursor.setCursorPen(cursorPen);
  cursor.setMarginColor(Qt::blue);

  int time = 200;
  QPixmap pixmap(dataDirectory + "/" + "computerIcon.png");

  // Basesize is always odd
  QSize baseSize = pixmap.size();
  if (pixmap.width() % 2 == 0)
    {
    baseSize.setWidth(baseSize.width()+1);
    }
  if (pixmap.height() % 2 == 0)
    {
    baseSize.setHeight(baseSize.height()+1);
    }

  // Odd widget size
  cursor.setMinimumSize(baseSize);
  cursor.setPixmap(pixmap.scaled(baseSize));
  cursor.show();

  // Test bullsEyeWidth and line width with odd widget size
  cursor.setCursorType(ctkCursorPixmapWidget::BullsEyeCursor);

  ///
  cursor.setBullsEyeWidth(15);
  cursorPen.setWidth(1);
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2a.png",
                       "using bulls-eye cursor (odd size, bullsEye 15, width 1)"))
    {
    return EXIT_FAILURE;
    }

  ///
  cursorPen.setWidth(5);
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2b.png",
                       "using bulls-eye cursor (odd size, bullsEye 15, width 5)"))
    {
    return EXIT_FAILURE;
    }

  ///
  cursor.setBullsEyeWidth(14);
  cursorPen.setWidth(0); // equivalent to 1
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2c.png",
                       "using bulls-eye cursor (odd size, bullsEye 14, width 1)"))
    {
    return EXIT_FAILURE;
    }

  ///
  cursorPen.setWidth(4);
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2d.png",
                       "using bulls-eye cursor (odd size, bullsEye 14, width 4)"))
    {
    return EXIT_FAILURE;
    }

  // Test bullsEyeWidth and line width with even widget size
  cursor.resize(baseSize.width()+1, baseSize.height()+1);

  ///
  cursor.setBullsEyeWidth(14);
  cursorPen.setWidth(1);
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2e.png",
                       "using bulls-eye cursor (even size, bullsEye 14, width 1)"))
    {
    return EXIT_FAILURE;
    }

  ///
  cursorPen.setWidth(4);
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2f.png",
                       "using bulls-eye cursor (even size, bullsEye 14, width 4)"))
    {
    return EXIT_FAILURE;
    }

  ///
  cursor.setBullsEyeWidth(15);
  cursorPen.setWidth(0);
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2g.png",
                       "using bulls-eye cursor (even size, bullsEye 15, width 1)"))
    {
    return EXIT_FAILURE;
    }

  ///
  cursorPen.setWidth(5);
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2h.png",
                       "using bulls-eye cursor (even size, bullsEye 15, width 5)"))
    {
    return EXIT_FAILURE;
    }

  // Cursor not shown
  cursor.resize(baseSize);
  cursor.setShowCursor(false);
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2i.png",
                       "show cursor false"))
    {
    return EXIT_FAILURE;
    }

  // Crosshair cursor
  cursor.setShowCursor(true);
  cursor.setCursorType(ctkCursorPixmapWidget::CrossHairCursor);
  cursorPen.setWidth(0);
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2j.png",
                       "using cross-hair cursor (odd size)"))
    {
    return EXIT_FAILURE;
    }
  cursor.resize(baseSize.width()+1, baseSize.height()+1);
  cursorPen.setWidth(1);
  cursor.setCursorPen(cursorPen);
  if (!runBaselineTest(time, app, cursor, baselineDirectory,
                       "ctkCursorPixmapWidgetTest2k.png",
                       "using cross-hair cursor (even size)"))
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

