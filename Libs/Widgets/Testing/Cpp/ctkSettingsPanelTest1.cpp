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
#include <QLineEdit>
#include <QSettings>
#include <QTimer>
#include <QVariant>

// CTK includes
#include "ctkBooleanMapper.h"
#include "ctkSettingsPanel.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
class ctkSettingsPanelForTest : public ctkSettingsPanel
{
public:
  QVariant myDefaultPropertyValue(const QString& key) const
    {
    return this->defaultPropertyValue(key);
    }
  QVariant myPreviousPropertyValue(const QString& key) const
    {
    return this->previousPropertyValue(key);
    }
  QVariant myPropertyValue(const QString& key) const
    {
    return this->propertyValue(key);
    }
};

} // end of anonymous namespace

//-----------------------------------------------------------------------------
int ctkSettingsPanelTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
  settings.clear();

  ctkSettingsPanelForTest settingsPanel;
  settingsPanel.setSettings(&settings);

  QCheckBox* box = new QCheckBox(&settingsPanel);

  settingsPanel.registerProperty("key 1", box, "checked",
                                  SIGNAL(toggled(bool)));
  
  // Check settings value after a property is registered
  QVariant boxVal = settings.value("key 1");
  if (!boxVal.isValid() || boxVal.toBool() != false)
    {
    std::cerr << "Line " << __LINE__ << " - Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPreviousPropertyValue("key 1").toBool() != false)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPropertyValue("key 1").toBool() != false)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with propertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  // Update value using the object/widget API
  box->setChecked(true);

  // Check settings value after it has been updated using object/widget API
  boxVal = settings.value("key 1");
  if (!boxVal.isValid() || boxVal.toBool() != true)
    {
    std::cerr << "Line " << __LINE__ << " - Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPreviousPropertyValue("key 1").toBool() != false)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPropertyValue("key 1").toBool() != true)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with propertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }


  QLineEdit* lineEdit = new QLineEdit("default", &settingsPanel);
  settingsPanel.registerProperty("key 2", lineEdit, "text",
                                  SIGNAL(textChanged(QString)));

  // Check value after a property is registered
  QVariant lineEditVal = settings.value("key 2");
  if (!lineEditVal.isValid() || lineEditVal.toString() != "default")
    {
    std::cerr << "Line " << __LINE__ << " - Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPreviousPropertyValue("key 2").toString() != "default")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myDefaultPropertyValue("key 2").toString() != "default")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPropertyValue("key 2").toString() != "default")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with propertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }

  // Update value using the object/widget API
  lineEdit->setText("first edit");

  // Check settings value after it has been updated using object/widget API
  lineEditVal = settings.value("key 2");
  if (!lineEditVal.isValid() || lineEditVal.toString() != "first edit")
    {
    std::cerr << "Line " << __LINE__ << " - Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPreviousPropertyValue("key 2").toString() != "default")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myDefaultPropertyValue("key 2").toString() != "default")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPropertyValue("key 2").toString() != "first edit")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with propertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }

  // Check settings value after applySettings() has been called
  settingsPanel.applySettings();
  lineEditVal = settings.value("key 2");
  if (!lineEditVal.isValid() || lineEditVal.toString() != "first edit")
    {
    std::cerr << "Line " << __LINE__ << " - Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPreviousPropertyValue("key 2").toString() != "first edit")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myDefaultPropertyValue("key 2").toString() != "default")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPropertyValue("key 2").toString() != "first edit")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with propertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }

  // Update value using the object/widget API
  lineEdit->setText("second edit");

  // Check settings value after it has been updated using object/widget API
  lineEditVal = settings.value("key 2");
  if (!lineEditVal.isValid() || lineEditVal.toString() != "second edit")
    {
    std::cerr << "Line " << __LINE__ << " - Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPreviousPropertyValue("key 2").toString() != "first edit")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myDefaultPropertyValue("key 2").toString() != "default")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPropertyValue("key 2").toString() != "second edit")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with propertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }

  // Check settings value after applySettings() has been called
  settingsPanel.applySettings();
  lineEditVal = settings.value("key 2");
  if (!lineEditVal.isValid() || lineEditVal.toString() != "second edit")
    {
    std::cerr << "Line " << __LINE__ << " - Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPreviousPropertyValue("key 2").toString() != "second edit")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myDefaultPropertyValue("key 2").toString() != "default")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPropertyValue("key 2").toString() != "second edit")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with propertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }

  box->setChecked(false);
  settingsPanel.registerProperty("key complement",
                                 new ctkBooleanMapper(box, "checked", SIGNAL(toggled(bool))),
                                 "complement",
                                  SIGNAL(complementChanged(bool)));

  // Check settings value after a property is registered
  boxVal = settings.value("key complement");
  if (!boxVal.isValid() || boxVal.toBool() != true)
    {
    std::cerr << "Line " << __LINE__ << " - Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPreviousPropertyValue("key complement").toBool() != true)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPropertyValue("key complement").toBool() != true)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with propertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  // Update value using the object/widget API
  box->setChecked(true);

  // Check settings value after it has been updated using object/widget API
  boxVal = settings.value("key complement");
  if (!boxVal.isValid() || boxVal.toBool() != false)
    {
    std::cerr << "Line " << __LINE__ << " - Saving to settings failed" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPreviousPropertyValue("key complement").toBool() != true)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with previousPropertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }
  if (settingsPanel.myPropertyValue("key complement").toBool() != false)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with propertyValue()!" << std::endl;
    return EXIT_FAILURE;
    }

  settingsPanel.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

