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
#include <QCursor>
#include <QHBoxLayout>
#include <QIcon>
#include <QSignalSpy>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkVTKMagnifyView.h"
#include "ctkCommandLineParser.h"
#include "ctkVTKSliceView.h"
#include "ctkWidgetsUtils.h"

// VTK includes
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
bool imageCompare(ctkVTKMagnifyView * magnify, QString baselineDirectory,
                  QString baselineFilename)
  {
  QImage output = ctk::grabWidget(magnify);
  QImage baseline(baselineDirectory + "/" + baselineFilename);
  return output == baseline;
  }

//-----------------------------------------------------------------------------
// (Used to create baselines, not during testing).
void imageSave(ctkVTKMagnifyView * magnify, QString baselineDirectory,
               QString baselineFilename)
  {
  QImage output = ctk::grabWidget(magnify);
  output.save(baselineDirectory + "/" + baselineFilename);
  }

//-----------------------------------------------------------------------------
bool runBaselineTest(int time, QApplication& app, ctkVTKMagnifyView * magnify,
                     QWidget * underWidget, bool shouldBeUnder,
                     QString baselineDirectory, QString testName,
                     QString testNumber, QString errorMessage)
{
  QTimer::singleShot(time, &app, SLOT(quit()));
  if (app.exec() == EXIT_FAILURE)
    {
    std::cerr << "ctkVTKMagnifyView exec failed when "
        << qPrintable(errorMessage) << std::endl;
    return false;
    }
  if (underWidget->underMouse() != shouldBeUnder)
    {
    std::cerr << "ctkMagnifyView mouse position failed when "
        << qPrintable(errorMessage) << std::endl;
    return false;
    }
  QString baselineFilename
      = "ctkVTKMagnifyViewTest2" + testNumber + testName + ".png";
  if (!imageCompare(magnify, baselineDirectory, baselineFilename))
    {
    std::cerr << "ctkVTKMagnifyView baseline comparison failed when "
              << qPrintable(errorMessage) << "." << std::endl;
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
int ctkVTKMagnifyViewTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  // Command line parser
  ctkCommandLineParser parser;
  parser.addArgument("", "-D", QVariant::String);
  parser.addArgument("", "-V", QVariant::String);
  parser.addArgument("", "-I", QVariant::String);
  parser.addArgument("", "-T", QVariant::String);
  parser.addArgument("", "-S", QVariant::String);
  parser.addArgument("", "-M", QVariant::String);
  bool ok = false;
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(app.arguments(), &ok);
  if (!ok)
    {
    std::cerr << qPrintable(parser.errorString()) << std::endl;
    return EXIT_FAILURE;
    }
  QString dataDirectory = parsedArgs["-D"].toString();
  QString baselineDirectory = parsedArgs["-V"].toString();
  QString testType = parsedArgs["-T"].toString();
  bool interactive = parsedArgs["-I"].toBool();
  int size = parsedArgs["-S"].toInt();
  double magnification = parsedArgs["-M"].toDouble();

  // Create the parent widget
  QWidget parentWidget;
  QHBoxLayout layout(&parentWidget);

  // Magnify widget parameters (we want an odd widget size and odd bullsEye)
  bool showCrosshair = true;
  QPen crosshairPen(Qt::yellow);
  crosshairPen.setJoinStyle(Qt::MiterJoin);
  ctkCrosshairLabel::CrosshairType crosshairType
      = ctkCrosshairLabel::BullsEyeCrosshair;
  double bullsEyeWidth = magnification + 2;
  QColor marginColor = Qt::magenta;
  bool observeRenderWindowEvents = false;
  int updateInterval = 0;

  // Create the magnify widget
  ctkVTKMagnifyView * magnify = new ctkVTKMagnifyView(&parentWidget);
  magnify->setMinimumSize(size,size);
  magnify->setMaximumSize(size,size);
  magnify->setShowCrosshair(showCrosshair);
  magnify->setCrosshairPen(crosshairPen);
  magnify->setCrosshairType(crosshairType);
  magnify->setBullsEyeWidth(bullsEyeWidth);
  magnify->setMarginColor(marginColor);
  magnify->setMagnification(magnification);
  magnify->setObserveRenderWindowEvents(observeRenderWindowEvents);
  magnify->setUpdateInterval(updateInterval);
  layout.addWidget(magnify);

  // Test magnify widget parameters
  if (magnify->showCrosshair() != showCrosshair)
    {
    std::cerr << "ctkVTKMagnifyView:setShowCrosshair failed. "
              << magnify->showCrosshair() << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->crosshairPen() != crosshairPen)
    {
    std::cerr << "ctkVTKMagnifyView:setCrosshairPen failed. "
              << qPrintable(magnify->crosshairPen().color().name()) << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->crosshairType() != crosshairType)
    {
    std::cerr << "ctkVTKMagnifyView:setCrosshairType failed. "
              << magnify->crosshairType() << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->bullsEyeWidth() != bullsEyeWidth)
    {
    std::cerr << "ctkVTKMagnifyView:setBullsEyeWidth failed. "
              << magnify->bullsEyeWidth() << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->marginColor() != marginColor)
    {
    std::cerr << "ctkVTKMagnifyView:setMarginColor failed. "
              << qPrintable(magnify->marginColor().name()) << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->magnification() != magnification)
    {
    std::cerr << "ctkVTKMagnifyView:setMagnification failed. "
              << magnify->magnification() << std::endl;
    return EXIT_FAILURE;
    }
  if (magnify->observeRenderWindowEvents() != observeRenderWindowEvents)
    {
    std::cerr << "ctkVTKMagnifyView:setObserveRenderWindowEvents failed. "
              << magnify->observeRenderWindowEvents() << std::endl;
    }
  if (magnify->updateInterval() != updateInterval)
    {
    std::cerr << "ctkVTKMagnifyView:setUpdateInterval failed. "
              << magnify->updateInterval() << std::endl;
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
    std::cerr << "ctkVTKMagnifyView:observe(ctkVTKOpenGLNativeWidget*) failed. "
              << "Number observed = " << magnify->numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  QString imageFilename = dataDirectory + "/" + "computerIcon.png";

  // Instanciate the reader factory
  vtkSmartPointer<vtkImageReader2Factory> imageFactory =
      vtkSmartPointer<vtkImageReader2Factory>::New();

  // Instanciate an image reader
  vtkSmartPointer<vtkImageReader2> imageReader;
  imageReader.TakeReference(imageFactory->CreateImageReader2(imageFilename.toUtf8()));
  if (!imageReader)
    {
    std::cerr << "Failed to instanciate image reader using: "
              << qPrintable(imageFilename) << std::endl;
    return EXIT_FAILURE;
    }

  // Read image
  imageReader->SetFileName(imageFilename.toUtf8());
  imageReader->Update();
  vtkAlgorithmOutput* imagePort = imageReader->GetOutputPort();

  // Setup the slice views
  for (int i = 0; i < numSliceViews; i++)
    {
    allSliceViews[i]->setRenderEnabled(true);
    allSliceViews[i]->setMinimumSize(350,350);
    allSliceViews[i]->setImageDataConnection(imagePort);
    allSliceViews[i]->setHighlightedBoxColor(Qt::yellow);
    allSliceViews[i]->scheduleRender();
    }

  int time = 200;

  // Get crosshair points of interest, used in the following tests
  parentWidget.move(0,0);
  parentWidget.show();
  QTimer::singleShot(time, &app, SLOT(quit()));
  if (app.exec() == EXIT_FAILURE)
    {
    std::cerr << "ctkVTKMagnifyView:show failed the first time." << std::endl;
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

  // Make sure the magnify widget magnifies right away when shown with the crosshair inside
  // an observed ctkVTKOpenGLNativeWidget
  QCursor::setPos(insideSlice0);
  parentWidget.show();
  if (!runBaselineTest(time, app, magnify, allSliceViews[0], true,
                       baselineDirectory, testType, "a",
                       "magnify widget first shown with crosshair inside observed widget"))
    {
    return EXIT_FAILURE;
    }
  parentWidget.hide();

  // Make sure the magnify widget shows blank right away when shown with the crosshair
  // outside the observed ctkVTKOpenGLNativeWidget(s)
  QCursor::setPos(outside);
  parentWidget.show();
  if (!runBaselineTest(time, app, magnify, &parentWidget, false,
                       baselineDirectory, testType, "b",
                       "magnify widget first shown with crosshair outside observed widget"))
    {
    return EXIT_FAILURE;
    }

  // Test magnification after move to allSliceViews[1]
  QCursor::setPos(insideSlice1);
  if (!runBaselineTest(time, app, magnify, allSliceViews[1], true,
                       baselineDirectory, testType, "c",
                       "crosshair moved inside 2nd observed widget the first time"))
    {
    return EXIT_FAILURE;
    }

  // Test magnification after move within allSliceViews[1] close to border
  QCursor::setPos(insideSlice1edge);
  if (!runBaselineTest(time, app, magnify, allSliceViews[1], true,
                       baselineDirectory, testType, "d",
                       "crosshair moved inside 2nd observed widget the second time"))
    {
    return EXIT_FAILURE;
    }

  // Test magnification after move outside an observed widget (should be blank)
  QCursor::setPos(insideSlice2);
  if (!runBaselineTest(time, app, magnify, allSliceViews[2], true,
                       baselineDirectory, testType, "e",
                       "crosshair moved inside unobserved widget"))
    {
    return EXIT_FAILURE;
    }

  // Test magnification after move back inside an observed widget (at extreme bottom
  // right corner)
  QCursor::setPos(insideSlice0bottomRightCorner);
  if (!runBaselineTest(time, app, magnify, allSliceViews[0], true,
                       baselineDirectory, testType, "f",
                       "crosshair moved to bottom-right corner of observed widget"))
    {
    return EXIT_FAILURE;
    }

  // Test magnification after move back inside an observed widget (at extreme top left
  // corner)
  QCursor::setPos(insideSlice0topLeftCorner);
  if (!runBaselineTest(time, app, magnify, allSliceViews[0], true,
                       baselineDirectory, testType, "g",
                       "crosshair moved to top-left corner of observed widget"))
    {
    return EXIT_FAILURE;
    }

  // Go back inside the widget
  QCursor::setPos(insideSlice0);
  if (!runBaselineTest(time, app, magnify, allSliceViews[0], true,
                       baselineDirectory, testType, "a",
                       "crosshair moved again inside observed widget"))
    {
    return EXIT_FAILURE;
    }

  // Test enabling observing render window events (trigger a render window event by
  // changing the image data)
  observeRenderWindowEvents = true;
  magnify->setObserveRenderWindowEvents(observeRenderWindowEvents);
  if (magnify->observeRenderWindowEvents() != observeRenderWindowEvents)
    {
    std::cerr << "ctkVTKMagnifyView:setObserveRenderWindowEvents failed. "
              << magnify->observeRenderWindowEvents() << std::endl;
    return EXIT_FAILURE;
    }

  vtkImageGaussianSmooth * gaussian = vtkImageGaussianSmooth::New();
  gaussian->SetInputConnection(imagePort);
  gaussian->SetRadiusFactors(5,5);
  gaussian->Update();
  allSliceViews[0]->setImageDataConnection(gaussian->GetOutputPort());
  allSliceViews[0]->scheduleRender();
  if (!runBaselineTest(time, app, magnify, allSliceViews[0], true,
                       baselineDirectory, testType, "h",
                       "after Gaussian blur when observing render window events"))
    {
    return EXIT_FAILURE;
    }

  // Test disabling observing render window events (trigger a render window event by
  // changing the image data)
  observeRenderWindowEvents = false;
  magnify->setObserveRenderWindowEvents(observeRenderWindowEvents);
  if (magnify->observeRenderWindowEvents() != observeRenderWindowEvents)
    {
    std::cerr << "ctkVTKMagnifyView:setObserveRenderWindowEvents failed. "
              << magnify->observeRenderWindowEvents() << std::endl;
    return EXIT_FAILURE;
    }

  gaussian->SetInputConnection(imageReader->GetOutputPort());
  gaussian->SetRadiusFactors(0,0);
  gaussian->Update();
  allSliceViews[0]->setImageDataConnection(gaussian->GetOutputPort());
  allSliceViews[0]->scheduleRender();
  if (!runBaselineTest(time, app, magnify, allSliceViews[0], true,
                       baselineDirectory, testType, "h",
                       "after Gaussian blur when not observing render window events"))
    {
    return EXIT_FAILURE;
    }

  // Test changing the update interval
  magnify->setUpdateInterval(time * 2);
  magnify->setObserveRenderWindowEvents(true);
  allSliceViews[0]->setImageDataConnection(imagePort);
  allSliceViews[0]->scheduleRender();
  QCursor::setPos(insideSlice0bottomRightCorner);
  // It should be waiting to update here
  if (!runBaselineTest(time, app, magnify, allSliceViews[0], true,
                       baselineDirectory, testType, "h",
                       "after changing update interval: updated too quickly"))
    {
    return EXIT_FAILURE;
    }
  // It should have updated by now
  if (!runBaselineTest(time + 50, app, magnify, allSliceViews[0], true,
                       baselineDirectory, testType, "f",
                       "after changing update interval: didn't update after waiting"))
    {
    return EXIT_FAILURE;
    }

  gaussian->Delete();

  return EXIT_SUCCESS;

}

