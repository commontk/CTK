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
#include "ctkDirectoryButtonPlugin.h"
#include "ctkDirectoryButton.h"

//-----------------------------------------------------------------------------
ctkDirectoryButtonPlugin::ctkDirectoryButtonPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkDirectoryButtonPlugin::createWidget(QWidget* widgetParent)
{
  ctkDirectoryButton* newWidget = new ctkDirectoryButton(widgetParent);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkDirectoryButtonPlugin::domXml() const
{
  return "<widget class=\"ctkDirectoryButton\" \
          name=\"DirectoryButton\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDirectoryButtonPlugin::icon() const
{
  return QIcon(":/Icons/pushbutton.png");
}

//-----------------------------------------------------------------------------
QString ctkDirectoryButtonPlugin::includeFile() const
{
  return "ctkDirectoryButton.h";
}

//-----------------------------------------------------------------------------
bool ctkDirectoryButtonPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkDirectoryButtonPlugin::name() const
{
  return "ctkDirectoryButton";
}
