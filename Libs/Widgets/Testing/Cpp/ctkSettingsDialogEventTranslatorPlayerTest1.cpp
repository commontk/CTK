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
#include <QCheckBox>
#include <QDebug>
#include <QSettings>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkSettingsDialog.h"
#include "ctkSettingsPanel.h"
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
  ctkSettingsDialog* widget = reinterpret_cast<ctkSettingsDialog*>(data);

  CTKCOMPARE(widget->currentPanel()->windowTitle(), "Panel 4");
  }
}

//-----------------------------------------------------------------------------
int ctkSettingsDialogEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);

  // Test case 1
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
  settings.remove("key 1");

  ctkSettingsDialog* widget = new ctkSettingsDialog();
  ctkSettingsPanel* panel1 = new ctkSettingsPanel;
  widget->addPanel("Panel 1", panel1);
  widget->addPanel("Panel 2", new ctkSettingsPanel);
  widget->addPanel("Panel 3", new ctkSettingsPanel);
  ctkSettingsPanel* panel4 = new ctkSettingsPanel;
  widget->addPanel("Panel 4", panel4, panel1);
  widget->setCurrentPanel("Panel 4");

  QCheckBox* box = new QCheckBox(panel4);
  box->setChecked(true);

  QVariant boxVal = settings.value("key 1");
  boxVal = settings.value("key 1");

  panel4->registerProperty("key 1", box, "checked",
                           SIGNAL(toggled(bool)));

  etpWidget.addTestCase(widget,
                        xmlDirectory + "ctkSettingsDialogEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

