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
#include "ctkDICOMTableViewPlugin.h"
#include "ctkDICOMTableView.h"

//-----------------------------------------------------------------------------
ctkDICOMTableViewPlugin::ctkDICOMTableViewPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkDICOMTableViewPlugin::createWidget(QWidget *parentForWidget)
{
  ctkDICOMTableView* newWidget = new ctkDICOMTableView(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkDICOMTableViewPlugin::domXml() const
{
  return "<widget class=\"ctkDICOMTableView\" \
          name=\"DICOMTableView\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDICOMTableViewPlugin::icon() const
{
  return QIcon(":/Icons/listview.png");
}

//-----------------------------------------------------------------------------
QString ctkDICOMTableViewPlugin::includeFile() const
{
  return "ctkDICOMTableView.h";
}

//-----------------------------------------------------------------------------
bool ctkDICOMTableViewPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkDICOMTableViewPlugin::name() const
{
  return "ctkDICOMTableView";
}
