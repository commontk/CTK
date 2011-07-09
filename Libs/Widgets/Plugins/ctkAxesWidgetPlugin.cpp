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
#include "ctkAxesWidgetPlugin.h"
#include "ctkAxesWidget.h"

//-----------------------------------------------------------------------------
ctkAxesWidgetPlugin::ctkAxesWidgetPlugin(QObject* parentObject)
  : QObject(parentObject)
{
}

//-----------------------------------------------------------------------------
QWidget* ctkAxesWidgetPlugin::createWidget(QWidget* parentWidget)
{
  ctkAxesWidget* newWidget = new ctkAxesWidget(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkAxesWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkAxesWidget\" \
          name=\"AxesWidget\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon ctkAxesWidgetPlugin::icon() const
{
  return QIcon(":/Icons/pushbutton.png");
}

//-----------------------------------------------------------------------------
QString ctkAxesWidgetPlugin::includeFile() const
{
  return "ctkAxesWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkAxesWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkAxesWidgetPlugin::name() const
{
  return "ctkAxesWidget";
}
