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
#include <QCheckBox>
#include <QSettings>
#include <QTimer>

// CTK includes
#include "ctkSettingsPanel.h"

// STD includes
#include <stdlib.h>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkSettingsPanelTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
  settings.clear();

  ctkSettingsPanel settingsPanel;
  settingsPanel.setSettings(&settings);

  QCheckBox* box = new QCheckBox(&settingsPanel);

  settingsPanel.registerProperty("key 1", box, "checked",
                                  SIGNAL(toggled(bool)));
  
  QVariant boxVal = settings.value("key 1");
  if (!boxVal.isValid() || boxVal.toBool() != false)
    {
    std::cerr << "Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  box->setChecked(true);
  boxVal = settings.value("key 1");
  if (!boxVal.isValid() || boxVal.toBool() != true)
    {
    std::cerr << "Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }

  settingsPanel.show();
      
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

