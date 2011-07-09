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
#include "ctkTreeComboBoxPlugin.h"
#include "ctkTreeComboBox.h"

//-----------------------------------------------------------------------------
ctkTreeComboBoxPlugin::ctkTreeComboBoxPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkTreeComboBoxPlugin::createWidget(QWidget *parentForWidget)
{
  ctkTreeComboBox* newWidget = new ctkTreeComboBox(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkTreeComboBoxPlugin::domXml() const
{
  return "<widget class=\"ctkTreeComboBox\" \
          name=\"TreeComboBox\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkTreeComboBoxPlugin::icon() const
{
  return QIcon(":/Icons/combobox.png");
}

//-----------------------------------------------------------------------------
QString ctkTreeComboBoxPlugin::includeFile() const
{
  return "ctkTreeComboBox.h";
}

//-----------------------------------------------------------------------------
bool ctkTreeComboBoxPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkTreeComboBoxPlugin::name() const
{
  return "ctkTreeComboBox";
}
