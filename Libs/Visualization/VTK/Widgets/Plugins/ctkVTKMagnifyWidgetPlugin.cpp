/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// CTK includes
#include "ctkVTKMagnifyWidgetPlugin.h"
#include "ctkVTKMagnifyWidget.h"

//-----------------------------------------------------------------------------
ctkVTKMagnifyWidgetPlugin::ctkVTKMagnifyWidgetPlugin(QObject *_parent):QObject(_parent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkVTKMagnifyWidgetPlugin::createWidget(QWidget *_parent)
{
  ctkVTKMagnifyWidget* _widget = new ctkVTKMagnifyWidget(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkVTKMagnifyWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkVTKMagnifyWidget\" \
          name=\"VTKMagnify\">\n"
          " <property name=\"geometry\">\n"
          "  <rect>\n"
          "   <x>0</x>\n"
          "   <y>0</y>\n"
          "   <width>150</width>\n"
          "   <height>150</height>\n"
          "  </rect>\n"
          " </property>\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QString ctkVTKMagnifyWidgetPlugin::includeFile() const
{
  return "ctkVTKMagnifyWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkVTKMagnifyWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkVTKMagnifyWidgetPlugin::name() const
{
  return "ctkVTKMagnifyWidget";
}
