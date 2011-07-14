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
#include "ctkCollapsibleButtonPlugin.h"
#include "ctkCollapsibleButton.h"

//-----------------------------------------------------------------------------
ctkCollapsibleButtonPlugin::ctkCollapsibleButtonPlugin(QObject *_parent)
        : QObject(_parent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkCollapsibleButtonPlugin::createWidget(QWidget *_parent)
{
  ctkCollapsibleButton* _widget = new ctkCollapsibleButton(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkCollapsibleButtonPlugin::domXml() const
{
  return "<widget class=\"ctkCollapsibleButton\" \
          name=\"CollapsibleButton\">\n"
          " <property name=\"geometry\">\n"
          "  <rect>\n"
          "   <x>0</x>\n"
          "   <y>0</y>\n"
          "   <width>300</width>\n"
          "   <height>100</height>\n"
          "  </rect>\n"
          " </property>\n"
          " <property name=\"text\">"
          "  <string>CollapsibleButton</string>"
          " </property>"
          " <property name=\"contentsFrameShape\">"
          "  <enum>QFrame::StyledPanel</enum>"
          " </property>"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon ctkCollapsibleButtonPlugin::icon() const
{
  return QIcon(":/Icons/groupboxcollapsible.png");
}

//-----------------------------------------------------------------------------
QString ctkCollapsibleButtonPlugin::includeFile() const
{
  return "ctkCollapsibleButton.h";
}

//-----------------------------------------------------------------------------
bool ctkCollapsibleButtonPlugin::isContainer() const
{
  return true;
}

//-----------------------------------------------------------------------------
QString ctkCollapsibleButtonPlugin::name() const
{
  return "ctkCollapsibleButton";
}
