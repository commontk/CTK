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
#include "ctkVTKPropertyWidget.h"
#include "ctkVTKPropertyWidgetPlugin.h"

//-----------------------------------------------------------------------------
ctkVTKPropertyWidgetPlugin
::ctkVTKPropertyWidgetPlugin(QObject* pluginParent)
 : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkVTKPropertyWidgetPlugin
::createWidget(QWidget *parentWidget)
{
  ctkVTKPropertyWidget* newWidget =
    new ctkVTKPropertyWidget(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkVTKPropertyWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkVTKPropertyWidget\" \
          name=\"PropertyWidget\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QString ctkVTKPropertyWidgetPlugin::includeFile() const
{
  return "ctkVTKPropertyWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkVTKPropertyWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkVTKPropertyWidgetPlugin::name() const
{
  return "ctkVTKPropertyWidget";
}
