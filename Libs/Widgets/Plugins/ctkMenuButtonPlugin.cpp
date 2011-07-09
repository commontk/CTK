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

// Qt includes
#include <QMenu>

// CTK includes
#include "ctkMenuButtonPlugin.h"
#include "ctkMenuButton.h"

// --------------------------------------------------------------------------
ctkMenuButtonPlugin::ctkMenuButtonPlugin(QObject *_parent)
        : QObject(_parent)
{
}

// --------------------------------------------------------------------------
QWidget *ctkMenuButtonPlugin::createWidget(QWidget *_parent)
{
  ctkMenuButton* _widget = new ctkMenuButton(_parent);
  QMenu* dummyMenu = new QMenu(_widget);
  dummyMenu->addAction("foo");
  dummyMenu->addAction("bar");
  dummyMenu->addAction("barbarbarbarbarbarbarbarbarbar");
  _widget->setMenu(dummyMenu);
  return _widget;
}

// --------------------------------------------------------------------------
QString ctkMenuButtonPlugin::domXml() const
{
  return "<widget class=\"ctkMenuButton\" name=\"MenuButton\">\n"
         " <property name=\"text\">"
         "  <string>MenuButton</string>"
         " </property>"
         "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkMenuButtonPlugin::icon() const
{
  return QIcon(":/Icons/combobox.png");
}

// --------------------------------------------------------------------------
QString ctkMenuButtonPlugin::includeFile() const
{
  return "ctkMenuButton.h";
}

// --------------------------------------------------------------------------
bool ctkMenuButtonPlugin::isContainer() const
{
  return false;
}

// --------------------------------------------------------------------------
QString ctkMenuButtonPlugin::name() const
{
  return "ctkMenuButton";
}
