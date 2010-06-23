/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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
          name=\"SliderSpinBoxWidget\">\n"
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
