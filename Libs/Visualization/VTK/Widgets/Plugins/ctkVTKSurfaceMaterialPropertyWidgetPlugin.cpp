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
#include "ctkVTKSurfaceMaterialPropertyWidget.h"
#include "ctkVTKSurfaceMaterialPropertyWidgetPlugin.h"

//-----------------------------------------------------------------------------
ctkVTKSurfaceMaterialPropertyWidgetPlugin
::ctkVTKSurfaceMaterialPropertyWidgetPlugin(QObject* pluginParent)
 : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkVTKSurfaceMaterialPropertyWidgetPlugin
::createWidget(QWidget *parentWidget)
{
  ctkVTKSurfaceMaterialPropertyWidget* newWidget =
    new ctkVTKSurfaceMaterialPropertyWidget(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkVTKSurfaceMaterialPropertyWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkVTKSurfaceMaterialPropertyWidget\" \
          name=\"SurfaceMaterialPropertyWidget\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QString ctkVTKSurfaceMaterialPropertyWidgetPlugin::includeFile() const
{
  return "ctkVTKSurfaceMaterialPropertyWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkVTKSurfaceMaterialPropertyWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkVTKSurfaceMaterialPropertyWidgetPlugin::name() const
{
  return "ctkVTKSurfaceMaterialPropertyWidget";
}
