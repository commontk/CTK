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
#include "ctkTemplateWidgetPlugin.h"
#include "ctkTemplateWidget.h"

//-----------------------------------------------------------------------------
ctkTemplateWidgetPlugin::ctkTemplateWidgetPlugin(QObject* parentObject)
  : QObject(parentObject)
{
}

//-----------------------------------------------------------------------------
QWidget* ctkTemplateWidgetPlugin::createWidget(QWidget* parentWidget)
{
  ctkTemplateWidget* newWidget = new ctkTemplateWidget(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkTemplateWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkTemplateWidget\" \
          name=\"TemplateWidget\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon ctkTemplateWidgetPlugin::icon() const
{
  return QIcon(":/Icons/pushbutton.png");
}

//-----------------------------------------------------------------------------
QString ctkTemplateWidgetPlugin::includeFile() const
{
  return "ctkTemplateWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkTemplateWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkTemplateWidgetPlugin::name() const
{
  return "ctkTemplateWidget";
}
