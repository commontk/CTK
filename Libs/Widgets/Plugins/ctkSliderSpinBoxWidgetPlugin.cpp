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
#include "ctkSliderSpinBoxWidgetPlugin.h"
#include "ctkSliderSpinBoxWidget.h"

//-----------------------------------------------------------------------------
ctkSliderSpinBoxWidgetPlugin::ctkSliderSpinBoxWidgetPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkSliderSpinBoxWidgetPlugin::createWidget(QWidget *_parent)
{
  ctkSliderSpinBoxWidget* _widget = new ctkSliderSpinBoxWidget(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkSliderSpinBoxWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkSliderSpinBoxWidget\" \
          name=\"ctkSliderSpinBoxWidget\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkSliderSpinBoxWidgetPlugin::icon() const
{
  return QIcon(":/Icons/sliderspinbox.png");
}

//-----------------------------------------------------------------------------
QString ctkSliderSpinBoxWidgetPlugin::includeFile() const
{
  return "ctkSliderSpinBoxWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkSliderSpinBoxWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkSliderSpinBoxWidgetPlugin::name() const
{
  return "ctkSliderSpinBoxWidget";
}
