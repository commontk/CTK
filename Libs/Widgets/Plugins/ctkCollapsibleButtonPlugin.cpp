/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
          name=\"CTKCollapsibleButton\">\n"
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
