/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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
#include "ctkPathListWidgetPlugin.h"
#include "ctkPathListWidget.h"

//-----------------------------------------------------------------------------
ctkPathListWidgetPlugin::ctkPathListWidgetPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkPathListWidgetPlugin::createWidget(QWidget* parentForWidget)
{
  ctkPathListWidget* newWidget = new ctkPathListWidget(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkPathListWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkPathListWidget\" \
          name=\"PathListWidget\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkPathListWidgetPlugin::icon() const
{
  return QIcon(":/Icons/listview.png");
}

//-----------------------------------------------------------------------------
QString ctkPathListWidgetPlugin::includeFile() const
{
  return "ctkPathListWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkPathListWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkPathListWidgetPlugin::name() const
{
  return "ctkPathListWidget";
}
