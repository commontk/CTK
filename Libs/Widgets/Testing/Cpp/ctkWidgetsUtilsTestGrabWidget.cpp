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
#include <QOpenGLContext>
#include <QOpenGLFunctions>
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

  QApplication::processEvents();

  // Detect software rendering (e.g. llvmpipe). grabFramebuffer() on a covered
  // QOpenGLWidget only works reliably with hardware GPU framebuffers.
  bool isSoftwareRenderer = false;
#if QT_VERSION >= QT_VERSION_CHECK(5,4,0)
  glWidget.makeCurrent();
  if (glWidget.context())
  {
    const char* renderer = reinterpret_cast<const char*>(
      glWidget.context()->functions()->glGetString(GL_RENDERER));
    if (renderer)
    {
      QString rendererStr = QString::fromLatin1(renderer).toLower();
      isSoftwareRenderer = rendererStr.contains("llvmpipe") ||
                           rendererStr.contains("softpipe") ||
                           rendererStr.contains("swrast");
      std::cout << "OpenGL renderer: " << renderer << std::endl;
    }
    glWidget.doneCurrent();
  }
#endif

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
    std::cout << "Pixel at (100,100)="
              << std::hex << screenshot.pixel(100, 100) << " expected " << QColor(Qt::black).rgb() << std::endl;
    if (isSoftwareRenderer)
    {
      std::cout << "Skipping covered-widget grab check on software renderer" << std::endl;
    }
    else
    {
      std::cout << "Failed to grab QGLWidget when covered by dialog" << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (argc < 2 || QString(argv[1]) != "-I")
  {
    QTimer::singleShot(100, &app, SLOT(quit()));
  }
  return app.exec();
}
