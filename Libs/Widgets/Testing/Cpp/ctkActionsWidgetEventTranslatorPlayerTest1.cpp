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

// CTK includes
#include "ctkActionsWidget.h"
#include <ctkCallback.h>
#include <ctkConfig.h>
#include "ctkEventTranslatorPlayerWidget.h"

#include "ctkSetName.h"

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
  ctkActionsWidget* widget = reinterpret_cast<ctkActionsWidget*>(data);

  CTKCOMPARE(widget->view()->currentIndex().row(), 2);
  }
}

//-----------------------------------------------------------------------------
int ctkActionsWidgetEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  ctkActionsWidget* widget = new ctkActionsWidget() << ctkSetName("actionsWidget");
  QIcon informationIcon = widget->style()->standardIcon(QStyle::SP_MessageBoxInformation);
  widget->addAction(new QAction(0), "category 1");
  widget->addAction(new QAction(qApp), "category 1");
  widget->addAction(new QAction("Action Text3", &etpWidget), "category 1");
  widget->addAction(new QAction(informationIcon, "Action Text4", qApp), "category 1");
  widget->addAction(new QAction(informationIcon, "Action Text5", qApp), "category 1");
  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkActionsWidgetEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}
