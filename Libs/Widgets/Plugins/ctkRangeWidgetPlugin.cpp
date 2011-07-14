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
#include "ctkRangeWidgetPlugin.h"
#include "ctkRangeWidget.h"

//-----------------------------------------------------------------------------
ctkRangeWidgetPlugin::ctkRangeWidgetPlugin(QObject* parentObject)
  : QObject(parentObject)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkRangeWidgetPlugin::createWidget(QWidget* parentWidget)
{
  ctkRangeWidget* newWidget = new ctkRangeWidget(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkRangeWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkRangeWidget\" name=\"RangeWidget\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkRangeWidgetPlugin::icon() const
{
  return QIcon(":/Icons/hrangeslider.png");
}

//-----------------------------------------------------------------------------
QString ctkRangeWidgetPlugin::includeFile() const
{
  return "ctkRangeWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkRangeWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkRangeWidgetPlugin::name() const
{
  return "ctkRangeWidget";
}
