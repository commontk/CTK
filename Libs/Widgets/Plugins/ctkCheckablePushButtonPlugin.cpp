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
#include "ctkCheckablePushButtonPlugin.h"
#include "ctkCheckablePushButton.h"

//-----------------------------------------------------------------------------
ctkCheckablePushButtonPlugin::ctkCheckablePushButtonPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkCheckablePushButtonPlugin::createWidget(QWidget* parentForWidget)
{
  ctkCheckablePushButton* newWidget =
    new ctkCheckablePushButton(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkCheckablePushButtonPlugin::domXml() const
{
  return "<widget class=\"ctkCheckablePushButton\" name=\"CheckBox\">\n"
         "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkCheckablePushButtonPlugin::icon() const
{
  return QIcon();
}

//-----------------------------------------------------------------------------
QString ctkCheckablePushButtonPlugin::includeFile() const
{
  return "ctkCheckablePushButton.h";
}

//-----------------------------------------------------------------------------
bool ctkCheckablePushButtonPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkCheckablePushButtonPlugin::name() const
{
  return "ctkCheckablePushButton";
}
