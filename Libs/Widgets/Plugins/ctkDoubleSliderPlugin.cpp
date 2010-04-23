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
#include "ctkDoubleSliderPlugin.h"
#include "ctkDoubleSlider.h"

// --------------------------------------------------------------------------
ctkDoubleSliderPlugin::ctkDoubleSliderPlugin(QObject *_parent)
  : QObject(_parent)
{
}

// --------------------------------------------------------------------------
QWidget *ctkDoubleSliderPlugin::createWidget(QWidget *_parent)
{
  ctkDoubleSlider* _widget = new ctkDoubleSlider(Qt::Horizontal, _parent);
  return _widget;
}

// --------------------------------------------------------------------------
QString ctkDoubleSliderPlugin::domXml() const
{
  return "<widget class=\"ctkDoubleSlider\" name=\"CTKSlider\">\n"
    "<property name=\"orientation\">\n"
    "  <enum>Qt::Horizontal</enum>\n"
    " </property>\n"
    "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDoubleSliderPlugin::icon() const
{
  return QIcon(":/Icons/hslider.png");
}

// --------------------------------------------------------------------------
QString ctkDoubleSliderPlugin::includeFile() const
{
  return "ctkDoubleSlider.h";
}

// --------------------------------------------------------------------------
bool ctkDoubleSliderPlugin::isContainer() const
{
  return false;
}

// --------------------------------------------------------------------------
QString ctkDoubleSliderPlugin::name() const
{
  return "ctkDoubleSlider";
}
