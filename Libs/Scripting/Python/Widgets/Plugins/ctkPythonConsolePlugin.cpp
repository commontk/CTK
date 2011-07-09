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
#include "ctkPythonConsolePlugin.h"
#include "ctkPythonConsole.h"
#include "ctkAbstractPythonManager.h"

//-----------------------------------------------------------------------------
ctkPythonConsolePlugin::ctkPythonConsolePlugin(QObject* pluginParent)
  : QObject(pluginParent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkPythonConsolePlugin::createWidget(QWidget* widgetParent)
{
  ctkPythonConsole* console = new ctkPythonConsole(widgetParent);
  console->initialize(new ctkAbstractPythonManager(this));
  return console;
}

//-----------------------------------------------------------------------------
QString ctkPythonConsolePlugin::domXml() const
{
  return "<widget class=\"ctkPythonConsole\" \
          name=\"PythonConsole\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkPythonConsolePlugin::icon() const
{
  return QIcon(":/Icons/console.png");
}

//-----------------------------------------------------------------------------
QString ctkPythonConsolePlugin::includeFile() const
{
  return "ctkPythonConsole.h";
}

//-----------------------------------------------------------------------------
bool ctkPythonConsolePlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkPythonConsolePlugin::name() const
{
  return "ctkPythonConsole";
}
