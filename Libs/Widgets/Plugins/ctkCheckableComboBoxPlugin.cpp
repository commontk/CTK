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
#include "ctkCheckableComboBoxPlugin.h"
#include "ctkCheckableComboBox.h"

//-----------------------------------------------------------------------------
ctkCheckableComboBoxPlugin::ctkCheckableComboBoxPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkCheckableComboBoxPlugin::createWidget(QWidget* parentForWidget)
{
  ctkCheckableComboBox* newWidget = new ctkCheckableComboBox(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkCheckableComboBoxPlugin::domXml() const
{
  return "<widget class=\"ctkCheckableComboBox\" \
          name=\"CheckableComboBox\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkCheckableComboBoxPlugin::icon() const
{
  return QIcon(":/Icons/combobox.png");
}

//-----------------------------------------------------------------------------
QString ctkCheckableComboBoxPlugin::includeFile() const
{
  return "ctkCheckableComboBox.h";
}

//-----------------------------------------------------------------------------
bool ctkCheckableComboBoxPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkCheckableComboBoxPlugin::name() const
{
  return "ctkCheckableComboBox";
}
