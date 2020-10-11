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
#include "ctkVTKWidgetsUtils.h"

// VTK includes
#include <vtkRenderWindow.h>
#include <vtkCallbackCommand.h>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <time.h>
#endif

void sleep_ms(int ms)
{
#ifdef Q_OS_WIN
  Sleep(ms);
#else
  struct timespec nanostep;
  nanostep.tv_sec = static_cast<time_t>(ms / 1000);
  nanostep.tv_nsec = ((ms % 1000) * 1000.0 * 1000.0);
  nanosleep(&nanostep, NULL);
#endif
}

unsigned int RenderCount = 0;

//-----------------------------------------------------------------------------
void onRenderEvent(vtkObject *vtkNotUsed(caller), unsigned long vtkNotUsed(eid), void *vtkNotUsed(clientData), void *vtkNotUsed(callData))
{
  ++RenderCount;
}

//-----------------------------------------------------------------------------
bool function2(ctkVTKAbstractView* view)
{
  view->pauseRender();
  view->scheduleRender();
  sleep_ms(100);
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
  ctk::vtkSetSurfaceDefaultFormat();

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
  sliceView.setMaximumUpdateRate(VTK_DOUBLE_MAX);
  sliceView.show();

  sliceView.scheduleRender();
  sleep_ms(100);
  sliceView.scheduleRender();

  // We expect that the rendering has been triggered at least once
  CHECK_BOOL(RenderCount == 0, false);

  bool sliceViewWasPaused = sliceView.pauseRender();
  RenderCount = 0;
  sliceView.scheduleRender();
  sleep_ms(100);
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
