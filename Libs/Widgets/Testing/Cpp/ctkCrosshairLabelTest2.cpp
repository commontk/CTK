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
#include <QIcon>
#include <QSignalSpy>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkCrosshairLabel.h"
#include "ctkCommandLineParser.h"
#include "ctkWidgetsTestingUtilities.h"
#include "ctkWidgetsUtils.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
bool imageCompare(ctkCrosshairLabel& crosshair, QString baselineDirectory,
                  QString baselineFilename)
{
  QImage output = ctk::grabWidget(&crosshair);
  QImage baseline(baselineDirectory + "/" + baselineFilename);
  const float percentThreshold = 1.5f;
  return ctkWidgetsTestingUtilities::CheckImagesEqual(output, baseline, percentThreshold);
}

//-----------------------------------------------------------------------------
// (Used to create baselines, not during testing).
void imageSave(ctkCrosshairLabel& crosshair, QString baselineDirectory,
               QString baselineFilename)
{
  QImage output = ctk::grabWidget(&crosshair);
  output.save(baselineDirectory + "/" + baselineFilename);
}

//-----------------------------------------------------------------------------
bool runBaselineTest(ctkCrosshairLabel& crosshair,
                     QString baselineDirectory, QString baselineFilename,
                     QString errorMessage)
{
  QApplication::processEvents();
  if (!imageCompare(crosshair, baselineDirectory, baselineFilename))
    {
    std::cerr << "ctkCrosshairLabel baseline comparison failed when "
              << qPrintable(errorMessage) << "." << std::endl;
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
int ctkCrosshairLabelTest2(int argc, char * argv [] )
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

  QPen crosshairPen(Qt::yellow);
  crosshairPen.setJoinStyle(Qt::MiterJoin);

  {
    // Create the crosshair widget
    ctkCrosshairLabel crosshair;
    crosshair.setCrosshairPen(crosshairPen);
    crosshair.setMarginColor(Qt::blue);

    // Odd widget size
    crosshair.setMinimumSize(baseSize);
    crosshair.setPixmap(pixmap.scaled(baseSize));
    crosshair.show();

    // Test bullsEyeWidth and line width with odd widget size
    crosshair.setCrosshairType(ctkCrosshairLabel::BullsEyeCrosshair);

    ///
    crosshair.setBullsEyeWidth(15);
    crosshairPen.setWidth(1);
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                "ctkCrosshairLabelTest2a.png",
                "using bulls-eye crosshair (odd size, bullsEye 15, width 1)"))
      {
      return EXIT_FAILURE;
      }

    ///
    crosshairPen.setWidth(5);
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                "ctkCrosshairLabelTest2b.png",
                "using bulls-eye crosshair (odd size, bullsEye 15, width 5)"))
      {
      return EXIT_FAILURE;
      }

    ///
    crosshair.setBullsEyeWidth(14);
    crosshairPen.setWidth(0); // equivalent to 1
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                "ctkCrosshairLabelTest2c.png",
                "using bulls-eye crosshair (odd size, bullsEye 14, width 1)"))
      {
      return EXIT_FAILURE;
      }

    ///
    crosshairPen.setWidth(4);
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                "ctkCrosshairLabelTest2d.png",
                "using bulls-eye crosshair (odd size, bullsEye 14, width 4)"))
      {
      return EXIT_FAILURE;
      }

    // Crosshair not shown
    crosshair.resize(baseSize);
    crosshair.setShowCrosshair(false);
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                         "ctkCrosshairLabelTest2i.png",
                         "show crosshair false"))
      {
      return EXIT_FAILURE;
      }

    // Crosshair crosshair
    crosshair.setShowCrosshair(true);
    crosshair.setCrosshairType(ctkCrosshairLabel::SimpleCrosshair);
    crosshairPen.setWidth(0);
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                         "ctkCrosshairLabelTest2j.png",
                         "using cross-hair crosshair (odd size)"))
      {
      return EXIT_FAILURE;
      }
  }

  {
    // Create the crosshair widget
    ctkCrosshairLabel crosshair;
    crosshair.setCrosshairPen(crosshairPen);
    crosshair.setMarginColor(Qt::blue);

    // Even widget size
    crosshair.resize(baseSize.width() + 1, baseSize.height() + 1);
    crosshair.setPixmap(pixmap.scaled(baseSize));
    crosshair.show();

    ///
    crosshair.setBullsEyeWidth(14);
    crosshairPen.setWidth(1);
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                         "ctkCrosshairLabelTest2e.png",
                         "using bulls-eye crosshair (even size, bullsEye 14, width 1)"))
      {
      return EXIT_FAILURE;
      }

    ///
    crosshairPen.setWidth(4);
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                         "ctkCrosshairLabelTest2f.png",
                         "using bulls-eye crosshair (even size, bullsEye 14, width 4)"))
      {
      return EXIT_FAILURE;
      }

    ///
    crosshair.setBullsEyeWidth(15);
    crosshairPen.setWidth(0);
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                         "ctkCrosshairLabelTest2g.png",
                         "using bulls-eye crosshair (even size, bullsEye 15, width 1)"))
      {
      return EXIT_FAILURE;
      }

    ///
    crosshairPen.setWidth(5);
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                         "ctkCrosshairLabelTest2h.png",
                         "using bulls-eye crosshair (even size, bullsEye 15, width 5)"))
      {
      return EXIT_FAILURE;
      }

    crosshairPen.setWidth(1);
    crosshair.setCrosshairPen(crosshairPen);
    if (!runBaselineTest(crosshair, baselineDirectory,
                         "ctkCrosshairLabelTest2k.png",
                         "using cross-hair crosshair (even size)"))
      {
      return EXIT_FAILURE;
      }
  }

  // We already tested for interactive mode
  QTimer::singleShot(200, &app, SLOT(quit()));
  return app.exec();

}

