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

// QT includes
#include <QApplication>
#include <QDialog>
#include <QFrame>
#if QT_VERSION < QT_VERSION_CHECK(5,4,0)
#include <QGLWidget>
#else
#include <QOpenGLWidget>
#endif
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkWidgetsUtils.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkWidgetsUtilsTestGrabWidget(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QFrame parentWidget;
  parentWidget.setFrameStyle(QFrame::Panel | QFrame::Raised);
  parentWidget.setLineWidth(2);

#if QT_VERSION < QT_VERSION_CHECK(5,4,0)
  QGLWidget glWidget(&parentWidget);
#else
  QOpenGLWidget glWidget(&parentWidget);
#endif
  QVBoxLayout* layout = new QVBoxLayout(&parentWidget);
  layout->addWidget(&glWidget);
  parentWidget.setLayout(layout);

  parentWidget.resize(200, 200);
  parentWidget.show();
  // Add a dialog to cover vtkWidget to test if grabWidget works even when the
  // opengl view is covered.
  QDialog dialog(0);
  dialog.move(parentWidget.pos());
  dialog.show();

  QApplication::processEvents();

  QImage screenshot =
    ctk::grabWidget(&parentWidget);

  if (QColor(screenshot.pixel(100, 100)) != QColor(Qt::black))
    {
    std::cout << "Failed to grab QGLWidget, pixel at (100,100)="
              << std::hex << screenshot.pixel(100, 100) << " " << QColor(Qt::black).rgb() << std::endl;
    return EXIT_FAILURE;
    }

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(100, &app, SLOT(quit()));
    }
  return app.exec();
}
