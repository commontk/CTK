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
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// VTK includes
#include <vtkActor.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSplineWidget2.h>
#include <vtkBoxWidget.h>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkVTKRenderView.h"
#include "ctkVTKRenderViewEventPlayer.h"
#include "ctkVTKRenderViewEventTranslator.h"
#include "ctkVTKWidgetsUtils.h"
#include "ctkEventTranslatorPlayerWidget.h"
#include "ctkWidgetsUtils.h"

#include <pqTestUtility.h>
#include <pqEventTranslator.h>

// STD includes
#include <cstdlib>
#include <iostream>

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

namespace
{
QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Visualization/VTK/Widgets/Testing/Cpp/";
ctkCallback* Callback1;
ctkCallback* Callback2;

bool save = false;
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkVTKRenderView* widget = reinterpret_cast<ctkVTKRenderView*>(data);

  QImage actualImage = ctk::grabWidget(widget, widget->rect());
  actualImage.save(xmlDirectory + "ctkVTKRenderViewEventTranslatorPlayerTest1ScreenshotTest.png");
  CTKCOMPARE(actualImage,
             QImage(xmlDirectory + "ctkVTKRenderViewEventTranslatorPlayerTest1Screenshot.png"));
  }
//-----------------------------------------------------------------------------
void screenshot(void* data)
  {
  if (save)
    {
    ctkVTKRenderView* widget = reinterpret_cast<ctkVTKRenderView*>(data);
    QImage expectedImage = ctk::grabWidget(widget, widget->rect());
    expectedImage.save(xmlDirectory + "ctkVTKRenderViewEventTranslatorPlayerTest1Screenshot.png");
    save = false;
    }
  }
void screenshotAvailable(void* data)
  {
  Q_UNUSED(data);
  save = true;
  }
}

//-----------------------------------------------------------------------------
int ctkVTKRenderViewEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  ctk::vtkSetSurfaceDefaultFormat();

  QApplication app(argc, argv);

//  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Visualization/VTK/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventTranslator(new ctkVTKRenderViewEventTranslator("QVTKWidget"));
  etpWidget.addWidgetEventPlayer(new ctkVTKRenderViewEventPlayer("QVTKWidget"));

  // Test case 1
  ctkVTKRenderView* widget = new ctkVTKRenderView();
  widget->setBackgroundColor(QColor(Qt::red));
  widget->setBackgroundColor2(QColor(Qt::yellow));
  widget->setGradientBackground(true);
  widget->setCornerAnnotationText("CTK Rocks !");
  widget->show();

  vtkSmartPointer<vtkSplineWidget2> splineWidget =
    vtkSmartPointer<vtkSplineWidget2>::New();
  splineWidget->SetInteractor(widget->interactor());
  splineWidget->On();

  vtkSmartPointer<vtkBoxWidget> boxWidget =
    vtkSmartPointer<vtkBoxWidget>::New();
  boxWidget->SetInteractor(widget->interactor());
  boxWidget->SetPlaceFactor(1.0);
  boxWidget->PlaceWidget();
  boxWidget->On();

  widget->lookFromAxis(ctkAxesWidget::Right);

  ctkCallback callback1;
  callback1.setCallback(screenshotAvailable);
  callback1.setCallbackData(widget);
  QObject::connect(etpWidget.testUtility()->eventTranslator(), SIGNAL(started()),
                   &callback1, SLOT(invoke()));

  ctkCallback callback2;
  callback2.setCallback(screenshot);
  callback2.setCallbackData(widget);
  QObject::connect(etpWidget.testUtility()->eventTranslator(), SIGNAL(stopped()),
                   &callback2, SLOT(invoke()));

  Callback1 = &callback1;
  Callback2 = &callback2;

  etpWidget.addTestCase(widget,
#if CTK_USE_QVTKOPENGLWIDGET
                        xmlDirectory + "ctkVTKRenderViewEventTranslatorPlayerTest1_QVTKOpenGLWidget.xml",
#else
                        xmlDirectory + "ctkVTKRenderViewEventTranslatorPlayerTest1.xml",
#endif
                        &checkFinalWidgetState);

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

