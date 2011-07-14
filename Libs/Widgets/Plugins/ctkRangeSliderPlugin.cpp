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
