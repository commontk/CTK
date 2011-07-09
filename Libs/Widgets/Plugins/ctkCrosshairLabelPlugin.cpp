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
#include "ctkCrosshairLabelPlugin.h"
#include "ctkCrosshairLabel.h"

//-----------------------------------------------------------------------------
ctkCrosshairLabelPlugin::ctkCrosshairLabelPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkCrosshairLabelPlugin::createWidget(QWidget *_parent)
{
  ctkCrosshairLabel* _widget = new ctkCrosshairLabel(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkCrosshairLabelPlugin::domXml() const
{
  return "<widget class=\"ctkCrosshairLabel\" \
          name=\"CrosshairLabel\">\n"
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

// --------------------------------------------------------------------------
 QIcon ctkCrosshairLabelPlugin::icon() const
 {
   return QIcon();
 }

//-----------------------------------------------------------------------------
QString ctkCrosshairLabelPlugin::includeFile() const
{
  return "ctkCrosshairLabel.h";
}

//-----------------------------------------------------------------------------
bool ctkCrosshairLabelPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkCrosshairLabelPlugin::name() const
{
  return "ctkCrosshairLabel";
}
