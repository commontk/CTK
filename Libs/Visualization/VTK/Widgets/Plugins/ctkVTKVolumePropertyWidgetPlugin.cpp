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
#include "ctkVTKVolumePropertyWidget.h"
#include "ctkVTKVolumePropertyWidgetPlugin.h"

//-----------------------------------------------------------------------------
ctkVTKVolumePropertyWidgetPlugin
::ctkVTKVolumePropertyWidgetPlugin(QObject *parentObject)
 : QObject(parentObject)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkVTKVolumePropertyWidgetPlugin::createWidget(QWidget *parentWidget)
{
  ctkVTKVolumePropertyWidget* newWidget =
    new ctkVTKVolumePropertyWidget(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkVTKVolumePropertyWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkVTKVolumePropertyWidget\" \
          name=\"VolumeProperty\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QString ctkVTKVolumePropertyWidgetPlugin::includeFile() const
{
  return "ctkVTKVolumePropertyWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkVTKVolumePropertyWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkVTKVolumePropertyWidgetPlugin::name() const
{
  return "ctkVTKVolumePropertyWidget";
}
