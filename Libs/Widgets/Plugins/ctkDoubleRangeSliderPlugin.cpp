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
#include "ctkDoubleRangeSliderPlugin.h"
#include "ctkDoubleRangeSlider.h"

// --------------------------------------------------------------------------
ctkDoubleRangeSliderPlugin::ctkDoubleRangeSliderPlugin(QObject *_parent)
  : QObject(_parent)
{
}

// --------------------------------------------------------------------------
QWidget *ctkDoubleRangeSliderPlugin::createWidget(QWidget *_parent)
{
  ctkDoubleRangeSlider* _widget = new ctkDoubleRangeSlider(Qt::Horizontal, _parent);
  return _widget;
}

// --------------------------------------------------------------------------
QString ctkDoubleRangeSliderPlugin::domXml() const
{
  return "<widget class=\"ctkDoubleRangeSlider\" name=\"CTKSlider\">\n"
    "<property name=\"orientation\">\n"
    "  <enum>Qt::Horizontal</enum>\n"
    " </property>\n"
    "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDoubleRangeSliderPlugin::icon() const
{
  return QIcon(":/Icons/hrangeslider.png");
}

// --------------------------------------------------------------------------
QString ctkDoubleRangeSliderPlugin::includeFile() const
{
  return "ctkDoubleRangeSlider.h";
}

// --------------------------------------------------------------------------
bool ctkDoubleRangeSliderPlugin::isContainer() const
{
  return false;
}

// --------------------------------------------------------------------------
QString ctkDoubleRangeSliderPlugin::name() const
{
  return "ctkDoubleRangeSlider";
}
