/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// Qt includes
#include <QDebug>

// CTK includes
#include "ctkCollapsibleGroupBoxPlugin.h"
#include "ctkCollapsibleGroupBox.h"

// --------------------------------------------------------------------------
ctkCollapsibleGroupBoxPlugin::ctkCollapsibleGroupBoxPlugin(QObject *_parent) : QObject(_parent)
{
}

// --------------------------------------------------------------------------
QWidget *ctkCollapsibleGroupBoxPlugin::createWidget(QWidget *_parent)
{
  ctkCollapsibleGroupBox* _widget = new ctkCollapsibleGroupBox(_parent);
  return _widget;
}

// --------------------------------------------------------------------------
QString ctkCollapsibleGroupBoxPlugin::domXml() const
{
  return "<widget class=\"ctkCollapsibleGroupBox\" \
          name=\"CTKCollapsibleGroupBox\">\n"
          " <property name=\"geometry\">\n"
          "  <rect>\n"
          "   <x>0</x>\n"
          "   <y>0</y>\n"
          "   <width>300</width>\n"
          "   <height>100</height>\n"
          "  </rect>\n"
          " </property>\n"
          " <property name=\"title\">"
          "  <string>GroupBox</string>"
          " </property>"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkCollapsibleGroupBoxPlugin::icon() const
{
  return QIcon(":/Icons/groupboxcollapsible.png");
}

// --------------------------------------------------------------------------
QString ctkCollapsibleGroupBoxPlugin::includeFile() const
{
  return "ctkCollapsibleGroupBox.h";
}

// --------------------------------------------------------------------------
bool ctkCollapsibleGroupBoxPlugin::isContainer() const
{
  return true;
}

// --------------------------------------------------------------------------
QString ctkCollapsibleGroupBoxPlugin::name() const
{
  return "ctkCollapsibleGroupBox";
}
