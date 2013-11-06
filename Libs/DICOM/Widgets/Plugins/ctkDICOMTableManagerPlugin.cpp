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
#include "ctkDICOMTableManagerPlugin.h"
#include "ctkDICOMTableManager.h"

//-----------------------------------------------------------------------------
ctkDICOMTableManagerPlugin::ctkDICOMTableManagerPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkDICOMTableManagerPlugin::createWidget(QWidget *parentForWidget)
{
  ctkDICOMTableManager* newWidget = new ctkDICOMTableManager(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkDICOMTableManagerPlugin::domXml() const
{
  return "<widget class=\"ctkDICOMTableManager\" \
          name=\"DICOMTableManager\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDICOMTableManagerPlugin::icon() const
{
  return QIcon(":/Icons/listview.png");
}

//-----------------------------------------------------------------------------
QString ctkDICOMTableManagerPlugin::includeFile() const
{
  return "ctkDICOMTableManager.h";
}

//-----------------------------------------------------------------------------
bool ctkDICOMTableManagerPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkDICOMTableManagerPlugin::name() const
{
  return "ctkDICOMTableManager";
}
