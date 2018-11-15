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
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkVTKMagnifyView.h"
#include "ctkVTKOpenGLNativeWidget.h"
#include "ctkEventTranslatorPlayerWidget.h"

// QtTesting includes
#include "pqTestUtility.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  ctkVTKMagnifyView* widget = reinterpret_cast<ctkVTKMagnifyView*>(data);
  Q_UNUSED(widget);
  }
//-----------------------------------------------------------------------------
void checkFinalWidgetState2(void* data)
  {
  ctkVTKMagnifyView* widget = reinterpret_cast<ctkVTKMagnifyView*>(data);
  Q_UNUSED(widget);
  }
}

//-----------------------------------------------------------------------------
int ctkVTKMagnifyViewEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Visualization/VTK/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  ctkVTKMagnifyView* widget = new ctkVTKMagnifyView();

  QList<ctkVTKOpenGLNativeWidget *> allVTKWidgets;
  int numVTKWidgets = 3;
  for (int i = 0; i < numVTKWidgets; i++)
    {
    allVTKWidgets.append(new ctkVTKOpenGLNativeWidget());
    }
  widget->observe(allVTKWidgets[0]);

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkVTKMagnifyViewEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // Test case 2
  // Create the parent widget
  QWidget parentWidget;
  QHBoxLayout layout(&parentWidget);

  int size = 180;
  double magnification = 5.00;

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

  etpWidget.addTestCase(&parentWidget,
                        xmlDirectory + "ctkVTKMagnifyViewEventTranslatorPlayerTest2.xml",
                        &checkFinalWidgetState2);

  // ------------------------
  if (!app.arguments().contains("-I"))
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

