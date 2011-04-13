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
#include <QCursor>
#include <QHBoxLayout>
#include <QIcon>
#include <QSignalSpy>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkVTKMagnifyWidget.h"
#include "ctkCommandLineParser.h"
#include "ctkVTKSliceView.h"

// VTK includes
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
bool imageCompare(ctkVTKMagnifyWidget * magnify, QString baselineDirectory,
                  QString baselineFilename)
  {
  QImage output = QPixmap::grabWidget(magnify).toImage();
  QImage baseline(baselineDirectory + "/" + baselineFilename);
  return output == baseline;
  }

//-----------------------------------------------------------------------------
// (Used to create baselines, not during testing).
void imageSave(ctkVTKMagnifyWidget * magnify, QString baselineDirectory,
               QString baselineFilename)
  {
  QImage output = QPixmap::grabWidget(magnify).toImage();
  output.save(baselineDirectory + "/" + baselineFilename);
  }

//-----------------------------------------------------------------------------
bool runBaselineTest(int time, QApplication& app, ctkVTKMagnifyWidget * magnify,
                     QWidget * underWidget, bool shouldBeUnder,
                     QString baselineDirectory, QString baselineFilename,
                     QString errorMessage)
{
  QTimer::singleShot(time, &app, SLOT(quit()));
  if (app.exec() == EXIT_FAILURE)
    {
    std::cerr << "ctkVTKMagnifyWidget exec failed when "
        << qPrintable(errorMessage) << std::endl;
    return false;
    }
  if (underWidget->underMouse() != shouldBeUnder)
    {
    std::cerr << "ctkMagnifyWidget mouse position failed when "
        << qPrintable(errorMessage) << std::endl;
    return false;
    }
  if (!imageCompare(magnify, baselineDirectory, baselineFilename))
    {
    std::cerr << "ctkVTKMagnifyWidget baseline comparison failed when "
              << qPrintable(errorMessage) << "." << std::endl;

    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
int ctkVTKMagnifyWidgetTest2(int argc, char * argv [] )
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

  // Create the parent widget
  QWidget parentWidget;
  QHBoxLayout layout(&parentWidget);

  // Magnify widget parameters (we want an odd widget size and odd bullsEye)
  int size = 341;
  bool showCursor = true;
  QColor cursorColor = Qt::yellow;
  ctkCursorPixmapWidget::CursorType cursorType
      = ctkCursorPixmapWidget::BullsEyeCursor;
  double magnification = 17;
  QColor marginColor = Qt::magenta;

  // Create the magnify widget
  ctkVTKMagnifyWidget * magnify = new ctkVTKMagnifyWidget(&parentWidget);
  magnify->setMinimumSize(size,size);
  magnify->setMaximumSize(size,size);
  magnify->setShowCursor(showCursor);
  magnify->setCursorColor(cursorColor);
  magnify->setCursorType(cursorType);
  magnify->setBullsEyeWidth(magnification);
  magnify->setMarginColor(marginColor);
  magnify->setMagnification(magnification);
  layout.addWidget(magnify);

  // Test magnify widget parameters
  if (magnify->showCursor() != showCursor)
    {
    std::cerr << "ctkVTKMagnifyWidget:setShowCursor failed. "
              << magnify->showCursor() << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->cursorColor() != cursorColor)
    {
    std::cerr << "ctkVTKMagnifyWidget:setCursorColor failed. "
              << qPrintable(magnify->cursorColor().name()) << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->cursorType() != cursorType)
    {
    std::cerr << "ctkVTKMagnifyWidget:setCursorType failed. "
              << magnify->cursorType() << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->bullsEyeWidth() != magnification)
    {
    std::cerr << "ctkVTKMagnifyWidget:setBullsEyeWidth failed. "
              << magnify->bullsEyeWidth() << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->marginColor() != marginColor)
    {
    std::cerr << "ctkVTKMagnifyWidget:setMarginColor failed. "
              << qPrintable(magnify->marginColor().name()) << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->magnification() != magnification)
    {
    std::cerr << "ctkVTKMagnifyWidget:setMagnification failed. "
              << magnify->magnification() << std::endl;
    return EXIT_FAILURE;
    }

  // The remainder is interactive, so abort now if command line args specify otherwise
  if (interactive)
    {
    return EXIT_SUCCESS;
    }

  // Add observed ctkVTKSliceViews (there are three, and the first two are observed)
  QList<ctkVTKSliceView *> allSliceViews;
  int numSliceViews = 3;
  for (int i = 0; i < numSliceViews; i++)
    {
    allSliceViews.append(new ctkVTKSliceView(&parentWidget));
    layout.addWidget(allSliceViews[i]);
    }

  // Observe the first two widgets
  magnify->observe(allSliceViews[0]->VTKWidget());
  magnify->observe(allSliceViews[1]->VTKWidget());
  if (!magnify->isObserved(allSliceViews[0]->VTKWidget()) ||
      !magnify->isObserved(allSliceViews[1]->VTKWidget()) ||
      magnify->isObserved(allSliceViews[2]->VTKWidget()) ||
      magnify->numberObserved() != 2)
    {
    std::cerr << "ctkVTKMagnifyWidget:observe(QVTKWidget*) failed. "
              << "Number observed = " << magnify->numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  QString imageFilename = dataDirectory + "/" + "computerIcon.png";

  // Instanciate the reader factory
  vtkSmartPointer<vtkImageReader2Factory> imageFactory =
      vtkSmartPointer<vtkImageReader2Factory>::New();

  // Instanciate an image reader
  vtkSmartPointer<vtkImageReader2> imageReader;
  imageReader.TakeReference(imageFactory->CreateImageReader2(imageFilename.toLatin1()));
  if (!imageReader)
    {
    std::cerr << "Failed to instanciate image reader using: "
              << qPrintable(imageFilename) << std::endl;
    return EXIT_FAILURE;
    }

  // Read image
  imageReader->SetFileName(imageFilename.toLatin1());
  imageReader->Update();
  vtkSmartPointer<vtkImageData> image = imageReader->GetOutput();

  // Setup the slice views
  for (int i = 0; i < numSliceViews; i++)
    {
    allSliceViews[i]->setRenderEnabled(true);
    allSliceViews[i]->setMinimumSize(350,350);
    allSliceViews[i]->setImageData(image);
    allSliceViews[i]->setHighlightedBoxColor(Qt::yellow);
    allSliceViews[i]->scheduleRender();
    }

  int time = 200;

  // Get cursor points of interest, used in the following tests
  parentWidget.move(0,0);
  parentWidget.show();
  QTimer::singleShot(time, &app, SLOT(quit()));
  if (app.exec() == EXIT_FAILURE)
    {
    std::cerr << "ctkVTKMagnifyWidget:show failed the first time." << std::endl;
    return EXIT_FAILURE;
    }
  QPoint insideSlice0 = allSliceViews[0]->mapToGlobal(
        QPoint(allSliceViews[0]->width()/2 + 100, allSliceViews[0]->height()/2 + 100));
  QPoint outside = parentWidget.mapToGlobal(
        QPoint(parentWidget.width(), parentWidget.height()));
  QPoint insideSlice1 = allSliceViews[1]->mapToGlobal(
        QPoint(allSliceViews[1]->width()/2 - 50, allSliceViews[1]->height() - 50));
  QPoint insideSlice1edge = allSliceViews[1]->mapToGlobal(
        QPoint(allSliceViews[1]->width() - 5, allSliceViews[1]->height() - 100));
  QPoint insideSlice2 = allSliceViews[2]->mapToGlobal(
        QPoint(allSliceViews[2]->width()/2, allSliceViews[2]->height()/2));
  QPoint insideSlice0bottomRightCorner = allSliceViews[0]->mapToGlobal(
        QPoint(allSliceViews[0]->width()-1, allSliceViews[0]->height()-1));
  QPoint insideSlice0topLeftCorner = allSliceViews[0]->mapToGlobal(
        QPoint(0,0));
  parentWidget.hide();

  // Make sure the magnify widget magnifies right away when shown with the cursor inside
  // an observed QVTKWidget
  QCursor::setPos(insideSlice0);
  parentWidget.show();
  if (!runBaselineTest(time, app, magnify, allSliceViews[0], true,
                       baselineDirectory, "ctkVTKMagnifyWidgetTest2a.png",
                       "magnify widget first shown with cursor inside observed widget"))
    {
    return EXIT_FAILURE;
    }
  parentWidget.hide();

  // Make sure the magnify widget shows blank right away when shown with the cursor
  // outside the observed QVTKWidgets
  QCursor::setPos(outside);
  parentWidget.show();
  if (!runBaselineTest(time, app, magnify, &parentWidget, false,
                       baselineDirectory, "ctkVTKMagnifyWidgetTest2b.png",
                       "magnify widget first shown with cursor outside observed widget"))
    {
    return EXIT_FAILURE;
    }

  // Test magnification after move to allSliceViews[1]
  QCursor::setPos(insideSlice1);
  if (!runBaselineTest(time, app, magnify, allSliceViews[1], true,
                       baselineDirectory, "ctkVTKMagnifyWidgetTest2c.png",
                       "cursor moved inside 2nd observed widget the first time"))
    {
    return EXIT_FAILURE;
    }

  // Test magnification after move within allSliceViews[1] close to border
  QCursor::setPos(insideSlice1edge);
  if (!runBaselineTest(time, app, magnify, allSliceViews[1], true,
                       baselineDirectory, "ctkVTKMagnifyWidgetTest2d.png",
                       "cursor moved inside 2nd observed widget the second time"))
    {
    return EXIT_FAILURE;
    }

  // Test magnification after move outside an observed widget (should be blank)
  QCursor::setPos(insideSlice2);
  if (!runBaselineTest(time, app, magnify, allSliceViews[2], true,
                       baselineDirectory, "ctkVTKMagnifyWidgetTest2e.png",
                       "cursor moved inside unobserved widget"))
    {
    return EXIT_FAILURE;
    }

  // Test magnification after move back inside an observed widget (at extreme bottom
  // right corner)
  QCursor::setPos(insideSlice0bottomRightCorner);
  if (!runBaselineTest(time, app, magnify, allSliceViews[0], true,
                       baselineDirectory, "ctkVTKMagnifyWidgetTest2f.png",
                       "cursor moved to bottom-right corner of observed widget"))
    {
    return EXIT_FAILURE;
    }

  // Test magnification after move back inside an observed widget (at extreme top left
  // corner)
  QCursor::setPos(insideSlice0topLeftCorner);
  if (!runBaselineTest(time, app, magnify, allSliceViews[0], true,
                       baselineDirectory, "ctkVTKMagnifyWidgetTest2g.png",
                       "cursor moved to top-left corner of observed widget"))
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}

