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
#include "ctkVTKScalarsToColorsWidget.h"
#include "ctkVTKScalarsToColorsWidgetPlugin.h"

//-----------------------------------------------------------------------------
ctkVTKScalarsToColorsWidgetPlugin
::ctkVTKScalarsToColorsWidgetPlugin(QObject *parentObject)
 : QObject(parentObject)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkVTKScalarsToColorsWidgetPlugin::createWidget(QWidget *parentWidget)
{
  ctkVTKScalarsToColorsWidget* newWidget =
    new ctkVTKScalarsToColorsWidget(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarsToColorsWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkVTKScalarsToColorsWidget\" \
          name=\"ScalarsToColorsWidget\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarsToColorsWidgetPlugin::includeFile() const
{
  return "ctkVTKScalarsToColorsWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkVTKScalarsToColorsWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarsToColorsWidgetPlugin::name() const
{
  return "ctkVTKScalarsToColorsWidget";
}
