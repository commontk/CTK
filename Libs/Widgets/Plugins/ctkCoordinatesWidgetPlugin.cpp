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
#include "ctkCoordinatesWidgetPlugin.h"
#include "ctkCoordinatesWidget.h"

// --------------------------------------------------------------------------
ctkCoordinatesWidgetPlugin::ctkCoordinatesWidgetPlugin(QObject *_parent)
        : QObject(_parent)
{
}

// --------------------------------------------------------------------------
QWidget *ctkCoordinatesWidgetPlugin::createWidget(QWidget *_parent)
{
  ctkCoordinatesWidget* _widget = new ctkCoordinatesWidget(_parent);
  return _widget;
}

// --------------------------------------------------------------------------
QString ctkCoordinatesWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkCoordinatesWidget\" \
          name=\"CTKCoordinatesWidget\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkCoordinatesWidgetPlugin::icon() const
{
  return QIcon(":/Icons/doublespinbox.png");
}

// --------------------------------------------------------------------------
QString ctkCoordinatesWidgetPlugin::includeFile() const
{
  return "ctkCoordinatesWidget.h";
}

// --------------------------------------------------------------------------
bool ctkCoordinatesWidgetPlugin::isContainer() const
{
  return false;
}

// --------------------------------------------------------------------------
QString ctkCoordinatesWidgetPlugin::name() const
{
  return "ctkCoordinatesWidget";
}
