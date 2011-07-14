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
#include "ctkVTKScalarBarWidget.h"
#include "ctkVTKScalarBarWidgetPlugin.h"

//-----------------------------------------------------------------------------
ctkVTKScalarBarWidgetPlugin::ctkVTKScalarBarWidgetPlugin(QObject *_parent):QObject(_parent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkVTKScalarBarWidgetPlugin::createWidget(QWidget *parentWidget)
{
  ctkVTKScalarBarWidget* newWidget = new ctkVTKScalarBarWidget(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarBarWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkVTKScalarBarWidget\" \
          name=\"VTKScalarBar\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarBarWidgetPlugin::includeFile() const
{
  return "ctkVTKScalarBarWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkVTKScalarBarWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarBarWidgetPlugin::name() const
{
  return "ctkVTKScalarBarWidget";
}
