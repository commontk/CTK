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
#include <QSettings>

// CTK includes
#include "ctkSettingsPanel.h"
#include "ctkSettingsPanelTest2Helper.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkSettingsPanelTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  Q_UNUSED(app);

  {
    // When QSettings goes out of scope, we are the settings file is up-to-date
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
    settings.clear();
    settings.setValue("list", QVariant(QStringList()));
  }

  // Regression: Reading empty QStringList property from settings should be handled properly

  ctkSettingsPanel settingsPanel;
  ctkSettingsPanelTest2Helper * list = new ctkSettingsPanelTest2Helper(&settingsPanel);
  settingsPanel.registerProperty("list", list, "list", SIGNAL(listChanged()));
  QSettings settings2(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
  settingsPanel.setSettings(&settings2);

  return EXIT_SUCCESS;
}

