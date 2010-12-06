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
#include "ctkSettingsDialog.h"

// STD includes
#include <stdlib.h>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkSettingsDialogTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
  settings.remove("key 1");

  ctkSettingsDialog settingsDialog;
  settingsDialog.setSettings(&settings);

  ctkSettingsPanel* panel1 = new ctkSettingsPanel;
  settingsDialog.addPanel("Panel 1", panel1); 
  if (panel1->settings() != &settings)
    {
    std::cerr << "ctkSettingsDialog::addPanel settings failed" << panel1->settings() << std::endl;
    return EXIT_FAILURE;
    }
  settingsDialog.addPanel("Panel 2", new ctkSettingsPanel);
  settingsDialog.addPanel("Panel 3", new ctkSettingsPanel);
  ctkSettingsPanel* panel4 = new ctkSettingsPanel;
  settingsDialog.addPanel("Panel 4", panel4, panel1);

  QCheckBox* box = new QCheckBox(panel4);
  box->setChecked(false); // false by default but we just want to make sure
  panel4->registerProperty("key 1", box, "checked",
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
  settingsDialog.resetSettings();
  boxVal = settings.value("key 1");
  if (!boxVal.isValid() || boxVal.toBool() != false)
    {
    std::cerr << "Reset failed" << std::endl;
    return EXIT_FAILURE;
    }

  settingsDialog.setCurrentPanel("Panel 4");

  settingsDialog.show();
      
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

