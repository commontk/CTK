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
#include "ctkPushButtonPlugin.h"
#include "ctkPushButton.h"

//-----------------------------------------------------------------------------
ctkPushButtonPlugin::ctkPushButtonPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkPushButtonPlugin::createWidget(QWidget* parentForWidget)
{
  ctkPushButton* newWidget =
    new ctkPushButton(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkPushButtonPlugin::domXml() const
{
  return "<widget class=\"ctkPushButton\" name=\"PushButton\">\n"
         "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkPushButtonPlugin::icon() const
{
  return QIcon(":/Icons/pushbutton.png");
}

//-----------------------------------------------------------------------------
QString ctkPushButtonPlugin::includeFile() const
{
  return "ctkPushButton.h";
}

//-----------------------------------------------------------------------------
bool ctkPushButtonPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkPushButtonPlugin::name() const
{
  return "ctkPushButton";
}
