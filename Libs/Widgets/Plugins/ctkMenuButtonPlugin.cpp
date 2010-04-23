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
  return "<widget class=\"ctkMenuButton\" name=\"CTKMenuButton\">\n"
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
