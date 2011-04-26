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
#include <QTimer>

// CTK includes
#include "ctkVTKMagnifyWidget.h"

// VTK includes
#include <QVTKWidget.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKMagnifyWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkVTKMagnifyWidget magnify;

  // check default values
  if (!magnify.showCursor() ||
      magnify.cursorPen().color() != magnify.palette().color(QPalette::Highlight) ||
      magnify.cursorPen().width() != 0 ||
      magnify.cursorPen().joinStyle() != Qt::MiterJoin ||
      magnify.cursorType() != ctkCursorPixmapWidget::CrossHairCursor ||
      magnify.marginColor() != magnify.palette().color(QPalette::Window) ||
      magnify.bullsEyeWidth() != 15 ||
      magnify.magnification() != 1.0 ||
      magnify.numberObserved() != 0)
    {
    std::cerr << "ctkVTKMagnifyWidget: Wrong default values. " << std::endl
              << " " << magnify.showCursor()
              << " " << qPrintable(magnify.cursorPen().color().name())
              << " " << magnify.cursorPen().width()
              << " " << static_cast<int>(magnify.cursorPen().joinStyle())
              << " " << magnify.cursorType()
              << " " << qPrintable(magnify.marginColor().name())
              << " " << magnify.bullsEyeWidth()
              << " " << magnify.magnification()
              << " " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Magnification
  magnify.setMagnification(10.5);
  if (magnify.magnification() != 10.5)
    {
    std::cerr << "ctkVTKMagnifyWidget:setMagnification failed. "
              << magnify.magnification() << std::endl;
    return EXIT_FAILURE;
    }

  // Adding / removing observed QVTKWidgets
  QList<QVTKWidget *> allVTKWidgets;
  int numVTKWidgets = 3;
  for (int i = 0; i < numVTKWidgets; i++)
    {
    allVTKWidgets.append(new QVTKWidget());
    }

  // Observe one widget
  magnify.observe(allVTKWidgets[0]);
  if (!magnify.isObserved(allVTKWidgets[0]) || magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 1)
    {
    std::cerr << "ctkVTKMagnifyWidget:observe(QVTKWidget*) failed. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Observe two more widgets
  magnify.observe(allVTKWidgets.mid(1,2));
  if (!magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      !magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 3)
    {
    std::cerr << "ctkVTKMagnifyWidget:observe(QList<QVTKWidget*>) failed. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Re-observe a widget
  magnify.observe(allVTKWidgets[2]);
  if (!magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      !magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 3)
    {
    std::cerr << "ctkVTKMagnifyWidget:observe(QVTKWidget*) failed on re-observe. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Re-observe a list of widgets
  magnify.observe(allVTKWidgets.mid(0,2));
  if (!magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      !magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 3)
    {
    std::cerr << "ctkVTKMagnifyWidget:observe(QList<QVTKWidget*>) failed on re-observe. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Remove a widget
  magnify.remove(allVTKWidgets[2]);
  if (!magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 2)
    {
    std::cerr << "ctkVTKMagnifyWidget:remove(QVTKWidget*) failed. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Re-remove a widget
  magnify.remove(allVTKWidgets[2]);
  if (!magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 2)
    {
    std::cerr << "ctkVTKMagnifyWidget:remove(QVTKWidget*) failed on re-remove. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Remove a list of widgets
  magnify.remove(allVTKWidgets.mid(0,2));
  if (magnify.isObserved(allVTKWidgets[0]) || magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 0)
    {
    std::cerr << "ctkVTKMagnifyWidget:remove(QList<QVTKWidget*>) failed. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Re-remove a list of widgets
  magnify.remove(allVTKWidgets.mid(1,2));
  if (magnify.isObserved(allVTKWidgets[0]) || magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 0)
    {
    std::cerr << "ctkVTKMagnifyWidget:remove(QList<QVTKWidget*>) failed on re-remove. "
              << "Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  // Observe a list of widgets of length one
  magnify.observe(allVTKWidgets.mid(1,1));
  if (magnify.isObserved(allVTKWidgets[0]) || !magnify.isObserved(allVTKWidgets[1]) ||
      magnify.isObserved(allVTKWidgets[2]) || magnify.numberObserved() != 1)
    {
    std::cerr << "ctkVTKMagnifyWidget:observe(QList<QVTKWidget*>) failed on lists of "
              << "length 1. Number observed = " << magnify.numberObserved() << std::endl;
    return EXIT_FAILURE;
    }

  magnify.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();

}

