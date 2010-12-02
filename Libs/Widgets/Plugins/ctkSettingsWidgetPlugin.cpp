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

// CTK includes
#include "ctkSettingsWidgetPlugin.h"
#include "ctkSettingsWidget.h"

//-----------------------------------------------------------------------------
ctkSettingsWidgetPlugin::ctkSettingsWidgetPlugin(QObject *pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkSettingsWidgetPlugin::createWidget(QWidget* widgetParent)
{
  ctkSettingsWidget* newWidget = new ctkSettingsWidget(widgetParent);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkSettingsWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkSettingsWidget\" \
          name=\"SettingsWidget\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon ctkSettingsWidgetPlugin::icon() const
{
  return QIcon(":/Icons/widget.png");
}

//-----------------------------------------------------------------------------
QString ctkSettingsWidgetPlugin::includeFile() const
{
  return "ctkSettingsWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkSettingsWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkSettingsWidgetPlugin::name() const
{
  return "ctkSettingsWidget";
}
