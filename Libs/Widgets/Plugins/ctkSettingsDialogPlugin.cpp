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
#include "ctkSettingsDialogPlugin.h"
#include "ctkSettingsDialog.h"

//-----------------------------------------------------------------------------
ctkSettingsDialogPlugin::ctkSettingsDialogPlugin(QObject *pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkSettingsDialogPlugin::createWidget(QWidget* widgetParent)
{
  ctkSettingsDialog* newWidget = new ctkSettingsDialog(widgetParent);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkSettingsDialogPlugin::domXml() const
{
  return "<widget class=\"ctkSettingsDialog\" \
          name=\"SettingsDialog\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon ctkSettingsDialogPlugin::icon() const
{
  return QIcon(":/Icons/widget.png");
}

//-----------------------------------------------------------------------------
QString ctkSettingsDialogPlugin::includeFile() const
{
  return "ctkSettingsDialog.h";
}

//-----------------------------------------------------------------------------
bool ctkSettingsDialogPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkSettingsDialogPlugin::name() const
{
  return "ctkSettingsDialog";
}
