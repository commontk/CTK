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
#include "ctkPathListButtonsWidgetPlugin.h"
#include "ctkPathListButtonsWidget.h"

//-----------------------------------------------------------------------------
ctkPathListButtonsWidgetPlugin::ctkPathListButtonsWidgetPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkPathListButtonsWidgetPlugin::createWidget(QWidget* parentForWidget)
{
  ctkPathListButtonsWidget* newWidget = new ctkPathListButtonsWidget(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkPathListButtonsWidget\" \
          name=\"PathListButtonsWidget\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkPathListButtonsWidgetPlugin::icon() const
{
  return QIcon();
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidgetPlugin::includeFile() const
{
  return "ctkPathListButtonsWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkPathListButtonsWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidgetPlugin::name() const
{
  return "ctkPathListButtonsWidget";
}
