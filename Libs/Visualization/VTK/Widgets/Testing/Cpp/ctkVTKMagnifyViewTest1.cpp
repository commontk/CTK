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
#include <QSharedPointer>
#include <QTimer>

// CTK includes
#include "ctkVTKMagnifyView.h"
#include "ctkVTKOpenGLNativeWidget.h"
#include "ctkVTKWidgetsUtils.h"

// VTK includes
#if CTK_USE_QVTKOPENGLWIDGET
# include <vtkGenericOpenGLRenderWindow.h>
#endif
#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKMagnifyViewTest1(int argc, char * argv [] )
{
  ctk::vtkSetSurfaceDefaultFormat();

  QApplication app(argc, argv);

  ctkVTKMagnifyView magnify;

  // check default values
  if (!magnify.showCrosshair() ||
      magnify.crosshairPen().color() != magnify.palette().color(QPalette::Highlight) ||
      magnify.crosshairPen().width() != 0 ||
      magnify.crosshairPen().joinStyle() != Qt::MiterJoin ||
      magnify.crosshairType() != ctkCrosshairLabel::SimpleCrosshair ||
      magnify.marginColor() != magnify.palette().color(QPalette::Window) ||
      magnify.bullsEyeWidth() != 15 ||
      magnify.magnification() != 1.0 ||
      magnify.observeRenderWindowEvents() != true ||
      magnify.updateInterval() != 20 ||
      magnify.numberObserved() != 0)
    {
    std::cerr << "ctkVTKMagnifyView: Wrong default values. " << std::endl
              << " " << magnify.showCrosshair()
              << " " << qPrintable(magnify.crosshairPen().color().name())
              << " " << magnify.crosshairPen().width()
              << " " << static_cast<int>(magnify.crosshairPen().joinStyle())
              << " " << magnify.crosshairType()
              << " " << qPrintable(magnify.marginColor().name())
              << " " << magnify.bullsEyeWidth()
              << " " << magnify.magnification()
              << " " << magnify.observeRenderWindowEvents()
              << " " << magnify.updateInterval()
              << " " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Magnification
  magnify.setMagnification(10.5);
  if (magnify.magnification() != 10.5)
    {
    std::cerr << "ctkVTKMagnifyView:setMagnification failed. "
              << magnify.magnification() << std::endl;
    return EXIT_FAILURE;
    }

  // Observe render window events
  magnify.setObserveRenderWindowEvents(false);
  if (magnify.observeRenderWindowEvents() != false)
    {
    std::cerr << "ctkVTKMagnifyView:setObserveRenderWindowEvents failed. "
              << magnify.observeRenderWindowEvents() << std::endl;
    return EXIT_FAILURE;
    }

  // Update interval
  magnify.setUpdateInterval(0);
  if (magnify.updateInterval() != 0)
    {
    std::cerr << "ctkVTKMagnifyView:setUpdateInterval failed. "
              << magnify.updateInterval() << std::endl;
    return EXIT_FAILURE;
    }

  // Adding / removing observed QVTKWidgets
  QObject widgetParent;
  QList<ctkVTKOpenGLNativeWidget* > allVTKWidgets;
  QList<QSharedPointer<ctkVTKOpenGLNativeWidget> > widgetsToDelete;
  int numVTKWidgets = 3;
  for (int i = 0; i < numVTKWidgets; i++)
    {
    ctkVTKOpenGLNativeWidget* widget = new ctkVTKOpenGLNativeWidget();
    allVTKWidgets.append(widget);
    widgetsToDelete.append(QSharedPointer<ctkVTKOpenGLNativeWidget>(widget));

#if CTK_USE_QVTKOPENGLWIDGET
      vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow =
          vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
      widget->SetRenderWindow(renderWindow);
#endif

    vtkNew<vtkRenderer> renderer;
    widget->GetRenderWindow()->AddRenderer(renderer.GetPointer());
    double gray = static_cast<double>(i) / (numVTKWidgets-1);
    renderer->SetBackground( gray, gray, gray);
    renderer->SetBackground2( 0., 0., 1.);
    renderer->SetGradientBackground(true);
    widget->show();
    }

  // Observe one widget
  magnify.observe(allVTKWidgets[0]);
  if (!magnify.isObserved(allVTKWidgets[0]) || magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 1)
    {
    std::cerr << "ctkVTKMagnifyView:observe(QVTKWidget*) failed. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Observe two more widgets
  magnify.observe(allVTKWidgets.mid(1,2));
  if (!magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      !magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 3)
    {
    std::cerr << "ctkVTKMagnifyView:observe(QList<QVTKWidget*>) failed. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Re-observe a widget
  magnify.observe(allVTKWidgets[2]);
  if (!magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      !magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 3)
    {
    std::cerr << "ctkVTKMagnifyView:observe(QVTKWidget*) failed on re-observe. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Re-observe a list of widgets
  magnify.observe(allVTKWidgets.mid(0,2));
  if (!magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      !magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 3)
    {
    std::cerr << "ctkVTKMagnifyView:observe(QList<QVTKWidget*>) failed on re-"
              << "observe. Number observed = " << magnify.numberObserved()
              << std::endl;
    return EXIT_FAILURE;
    }

  // Remove a widget
  magnify.remove(allVTKWidgets[2]);
  if (!magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 2)
    {
    std::cerr << "ctkVTKMagnifyView:remove(QVTKWidget*) failed. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Re-remove a widget
  magnify.remove(allVTKWidgets[2]);
  if (!magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 2)
    {
    std::cerr << "ctkVTKMagnifyView:remove(QVTKWidget*) failed on re-remove. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Remove a list of widgets
  magnify.remove(allVTKWidgets.mid(0,2));
  if (magnify.isObserved(allVTKWidgets[0]) || magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 0)
    {
    std::cerr << "ctkVTKMagnifyView:remove(QList<QVTKWidget*>) failed. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Re-remove a list of widgets
  magnify.remove(allVTKWidgets.mid(1,2));
  if (magnify.isObserved(allVTKWidgets[0]) || magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 0)
    {
    std::cerr << "ctkVTKMagnifyView:remove(QList<QVTKWidget*>) failed on re-remove."
              << " Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Observe a list of widgets of length one
  magnify.observe(allVTKWidgets.mid(1,1));
  if (magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 1)
    {
    std::cerr << "ctkVTKMagnifyView:observe(QList<QVTKWidget*>) failed on lists of "
              << "length 1. Number observed = " << magnify.numberObserved()
              << std::endl;
    return EXIT_FAILURE;
    }

  magnify.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();

}

