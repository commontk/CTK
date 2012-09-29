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
#include "ctkExpandableWidgetPlugin.h"
#include "ctkExpandableWidget.h"

// --------------------------------------------------------------------------
ctkExpandableWidgetPlugin
::ctkExpandableWidgetPlugin(QObject* parentObject)
  : QObject(parentObject)
{
}

// --------------------------------------------------------------------------
QWidget *ctkExpandableWidgetPlugin
::createWidget(QWidget* futurParentWidget)
{
  ctkExpandableWidget* widget =
    new ctkExpandableWidget(futurParentWidget);
  return widget;
}

// --------------------------------------------------------------------------
QString ctkExpandableWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkExpandableWidget\" \
          name=\"ResizableFrame\">\n"
          " <property name=\"geometry\">\n"
          "  <rect>\n"
          "   <x>0</x>\n"
          "   <y>0</y>\n"
          "   <width>300</width>\n"
          "   <height>300</height>\n"
          "  </rect>\n"
          " </property>\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkExpandableWidgetPlugin::icon() const
{
  return QIcon(":/Icons/widget.png");
}

// --------------------------------------------------------------------------
QString ctkExpandableWidgetPlugin::includeFile() const
{
  return "ctkExpandableWidget.h";
}

// --------------------------------------------------------------------------
bool ctkExpandableWidgetPlugin::isContainer() const
{
  return true;
}

// --------------------------------------------------------------------------
QString ctkExpandableWidgetPlugin::name() const
{
  return "ctkExpandableWidget";
}
