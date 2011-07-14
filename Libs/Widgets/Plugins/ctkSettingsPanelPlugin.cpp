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

// CTK includes
#include "ctkSettingsPanelPlugin.h"
#include "ctkSettingsPanel.h"

//-----------------------------------------------------------------------------
ctkSettingsPanelPlugin::ctkSettingsPanelPlugin(QObject *pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkSettingsPanelPlugin::createWidget(QWidget* widgetParent)
{
  ctkSettingsPanel* newWidget = new ctkSettingsPanel(widgetParent);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkSettingsPanelPlugin::domXml() const
{
  return "<widget class=\"ctkSettingsPanel\" \
          name=\"SettingsPanel\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon ctkSettingsPanelPlugin::icon() const
{
  return QIcon(":/Icons/widget.png");
}

//-----------------------------------------------------------------------------
QString ctkSettingsPanelPlugin::includeFile() const
{
  return "ctkSettingsPanel.h";
}

//-----------------------------------------------------------------------------
bool ctkSettingsPanelPlugin::isContainer() const
{
  return true;
}

//-----------------------------------------------------------------------------
QString ctkSettingsPanelPlugin::name() const
{
  return "ctkSettingsPanel";
}
