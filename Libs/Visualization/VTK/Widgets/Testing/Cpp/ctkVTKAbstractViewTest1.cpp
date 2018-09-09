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
#include <QDebug>
#include <QTimer>

// CTK includes
#include "ctkCommandLineParser.h"
#include "ctkVTKSliceView.h"
#include "ctkVTKRenderView.h"
#include "ctkCoreTestingMacros.h"

// VTK includes
#if CTK_USE_QVTKOPENGLWIDGET
#include <QVTKOpenGLWidget.h>
#endif
#include <vtkRenderWindow.h>
#include <vtkCallbackCommand.h>

unsigned int RenderCount = 0;

//-----------------------------------------------------------------------------
void onRenderEvent(vtkObject *caller, unsigned long vtkNotUsed(eid), void *clientData, void *vtkNotUsed(callData))
{
  ++RenderCount;
}

//-----------------------------------------------------------------------------
bool function2(ctkVTKAbstractView* view)
{
  view->pauseRender();
  view->scheduleRender();
  Sleep(100);
  view->scheduleRender();
  view->resumeRender();
  if (RenderCount != 0)
    {
    std::cerr << "function2: Render count " << RenderCount
              << " does not match expected value of " << 0 << std::endl;
    }
  return RenderCount == 0;
}

//-----------------------------------------------------------------------------
bool function1(ctkVTKAbstractView* view)
{
  RenderCount = 0;
  view->pauseRender();
  bool success = function2(view);
  view->resumeRender();
  if (RenderCount == 0)
    {
    std::cerr << "function1: Render count " << RenderCount
              << " should be greater than " << 0 << std::endl;
    success = false;
    }
  return success;
}

//-----------------------------------------------------------------------------
int ctkVTKAbstractViewTest1(int argc, char * argv [] )
{
#if CTK_USE_QVTKOPENGLWIDGET
    QSurfaceFormat format = QVTKOpenGLWidget::defaultFormat();
    format.setSamples(0);
    QSurfaceFormat::setDefaultFormat(format);
#endif

  QApplication app(argc, argv);

  // Command line parser
  ctkCommandLineParser parser;
  parser.addArgument("", "-I", QVariant::Bool);
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(app.arguments());
  bool interactive = parsedArgs["-I"].toBool();

  // Instantiate slice view
  ctkVTKSliceView sliceView;
  sliceView.setHighlightedBoxColor(QColor(Qt::yellow));
  sliceView.setCornerAnnotationText("SliceView");

  vtkNew<vtkCallbackCommand> renderEventCallback;
  renderEventCallback->SetCallback(onRenderEvent);
  sliceView.renderWindow()->AddObserver(vtkCommand::RenderEvent, renderEventCallback);
  sliceView.setMaximumUpdateRate(INFINITE);
  sliceView.show();

  sliceView.scheduleRender();
  Sleep(100);
  sliceView.scheduleRender();

  // We expect that the rendering has been triggered at least once
  CHECK_BOOL(RenderCount == 0, false);

  bool sliceViewWasPaused = sliceView.pauseRender();
  RenderCount = 0;
  sliceView.scheduleRender();
  Sleep(100);
  sliceView.scheduleRender();

  // We expect that the rendering has not been triggered
  CHECK_BOOL(RenderCount == 0, true);

  sliceView.resumeRender();

  // We expect that the rendering has been triggered at least once
  CHECK_BOOL(RenderCount == 0, false);

  // Nested functions that call pauseRender
  CHECK_BOOL(function1(&sliceView), true);

  if (!interactive)
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
