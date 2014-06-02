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
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSettings>
#include <QTimer>

// CTK includes
#include "ctkSettingsPanel.h"
#include "ctkSettingsDialog.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkSettingsDialogTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
  settings.remove("key 1");

  ctkSettingsDialog settingsDialog;

  // Get a reference to the DialogButtonBox
  QDialogButtonBox * buttonBox = settingsDialog.findChild<QDialogButtonBox*>("SettingsButtonBox");
  if (!buttonBox)
    {
    std::cerr << "Line " << __LINE__ << " - Failed to get a reference to the button box !" << std::endl;
    return EXIT_FAILURE;
    }

  // Reset button should be disabled by default
  if (buttonBox->button(QDialogButtonBox::Reset)->isEnabled())
    {
    std::cerr << "Line " << __LINE__ << " - Reset button should be disabled !" << std::endl;
    return EXIT_FAILURE;
    }

  settingsDialog.setSettings(&settings);

  // Reset button should be disabled after settings are set
  if (buttonBox->button(QDialogButtonBox::Reset)->isEnabled())
    {
    std::cerr << "Line " << __LINE__ << " - Reset button should be disabled !" << std::endl;
    return EXIT_FAILURE;
    }

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
  settingsDialog.addPanel("Panel 4 with long title", panel4, panel1);

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
  // Reset button should be enabled after settings are modified
  if (!buttonBox->button(QDialogButtonBox::Reset)->isEnabled())
    {
    std::cerr << "Line " << __LINE__ << " - Reset button should be enabled !" << std::endl;
    return EXIT_FAILURE;
    }
  settingsDialog.resetSettings();
  // Reset button should be disabled after settings are reset
  if (buttonBox->button(QDialogButtonBox::Reset)->isEnabled())
    {
    std::cerr << "Line " << __LINE__ << " - Reset button should be disabled !" << std::endl;
    return EXIT_FAILURE;
    }
  boxVal = settings.value("key 1");
  if (!boxVal.isValid() || boxVal.toBool() != false)
    {
    std::cerr << "Reset failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (box->isChecked())
    {
    std::cerr << "Reset failed" << std::endl;
    return EXIT_FAILURE;
    }

  // Reset button should be enabled after settings are modified
  box->setChecked(true);
  boxVal = settings.value("key 1");
  if (!boxVal.isValid() || boxVal.toBool() != true)
    {
    std::cerr << "Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (!buttonBox->button(QDialogButtonBox::Reset)->isEnabled())
    {
    std::cerr << "Line " << __LINE__ << " - Reset button should be enabled !" << std::endl;
    return EXIT_FAILURE;
    }
  // .. and reset button should be disabled if new settings object is set
  QSettings settings2(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
  settingsDialog.setSettings(&settings2);
  if (buttonBox->button(QDialogButtonBox::Reset)->isEnabled())
    {
    std::cerr << "Line " << __LINE__ << " - Reset button should be disabled !" << std::endl;
    return EXIT_FAILURE;
    }

  settings2.setValue("key 1", true);
  settingsDialog.reloadSettings();
  if (!box->isChecked())
    {
    std::cerr << "Reload failed" << std::endl;
    return EXIT_FAILURE;
    }

  settingsDialog.restoreDefaultSettings();
  boxVal = settings2.value("key 1");
  if (!boxVal.isValid() || boxVal.toBool() != false)
    {
    std::cerr << "Restore to default failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (box->isChecked())
    {
    std::cerr << "Restore to default failed" << std::endl;
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

