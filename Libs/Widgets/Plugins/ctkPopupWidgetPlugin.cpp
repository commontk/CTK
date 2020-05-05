/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// CTK includes
#include "ctkPopupWidgetPlugin.h"
#include "ctkPopupWidget.h"

//-----------------------------------------------------------------------------
ctkPopupWidgetPlugin::ctkPopupWidgetPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkPopupWidgetPlugin::createWidget(QWidget* widgetParent)
{
  ctkPopupWidget* newWidget = new ctkPopupWidget(widgetParent);
  // if the widget is a tooltip, it wouldn't accept children
  newWidget->setWindowFlags(0);
  // if the widget auto hides, it disappear from the workplace and don't allow
  // children anymore.
  newWidget->setAutoHide(false);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkPopupWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkPopupWidget\" name=\"PopupWidget\">\n</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon ctkPopupWidgetPlugin::icon() const
{
  return QIcon(":/Icons/widget.png");
}

//-----------------------------------------------------------------------------
QString ctkPopupWidgetPlugin::includeFile() const
{
  return "ctkPopupWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkPopupWidgetPlugin::isContainer() const
{
  return true;
}

//-----------------------------------------------------------------------------
QString ctkPopupWidgetPlugin::name() const
{
  return "ctkPopupWidget";
}
