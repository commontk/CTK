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
#include "ctkVTKSliceViewPlugin.h"
#include "ctkVTKSliceView.h"

//-----------------------------------------------------------------------------
ctkVTKSliceViewPlugin::ctkVTKSliceViewPlugin(QObject *_parent):QObject(_parent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkVTKSliceViewPlugin::createWidget(QWidget *_parent)
{
  ctkVTKSliceView* _widget = new ctkVTKSliceView(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkVTKSliceViewPlugin::domXml() const
{
  return "<widget class=\"ctkVTKSliceView\" \
          name=\"VTKRenderView\">\n"
          " <property name=\"geometry\">\n"
          "  <rect>\n"
          "   <x>0</x>\n"
          "   <y>0</y>\n"
          "   <width>200</width>\n"
          "   <height>200</height>\n"
          "  </rect>\n"
          " </property>\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QString ctkVTKSliceViewPlugin::includeFile() const
{
  return "ctkVTKSliceView.h";
}

//-----------------------------------------------------------------------------
bool ctkVTKSliceViewPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkVTKSliceViewPlugin::name() const
{
  return "ctkVTKSliceView";
}
