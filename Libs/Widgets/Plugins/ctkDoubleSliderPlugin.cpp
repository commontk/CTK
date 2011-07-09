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
  return "<widget class=\"ctkDoubleSlider\" name=\"DoubleSlider\">\n"
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
