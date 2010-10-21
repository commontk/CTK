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
          name=\"CollapsibleGroupBox\">\n"
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
