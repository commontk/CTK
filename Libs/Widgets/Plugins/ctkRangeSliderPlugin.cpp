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
#include "ctkRangeSliderPlugin.h"
#include "ctkRangeSlider.h"

// --------------------------------------------------------------------------
ctkRangeSliderPlugin::ctkRangeSliderPlugin(QObject *_parent)
  : QObject(_parent)
{
}

// --------------------------------------------------------------------------
QWidget *ctkRangeSliderPlugin::createWidget(QWidget *_parent)
{
  ctkRangeSlider* _widget = new ctkRangeSlider(Qt::Horizontal, _parent);
  return _widget;
}

// --------------------------------------------------------------------------
QString ctkRangeSliderPlugin::domXml() const
{
  return "<widget class=\"ctkRangeSlider\" name=\"RangeSlider\">\n"
    "<property name=\"orientation\">\n"
    "  <enum>Qt::Horizontal</enum>\n"
    " </property>\n"
    "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkRangeSliderPlugin::icon() const
{
  return QIcon(":/Icons/hrangeslider.png");
}

// --------------------------------------------------------------------------
QString ctkRangeSliderPlugin::includeFile() const
{
  return "ctkRangeSlider.h";
}

// --------------------------------------------------------------------------
bool ctkRangeSliderPlugin::isContainer() const
{
  return false;
}

// --------------------------------------------------------------------------
QString ctkRangeSliderPlugin::name() const
{
  return "ctkRangeSlider";
}
