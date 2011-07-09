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
#include "ctkActionsWidgetPlugin.h"
#include "ctkActionsWidget.h"

//-----------------------------------------------------------------------------
ctkActionsWidgetPlugin::ctkActionsWidgetPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkActionsWidgetPlugin::createWidget(QWidget *_parent)
{
  ctkActionsWidget* _widget = new ctkActionsWidget(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkActionsWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkActionsWidget\" \
          name=\"ActionsWidget\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkActionsWidgetPlugin::icon() const
{
  return QIcon(":/Icons/table.png");
}

//-----------------------------------------------------------------------------
QString ctkActionsWidgetPlugin::includeFile() const
{
  return "ctkActionsWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkActionsWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkActionsWidgetPlugin::name() const
{
  return "ctkActionsWidget";
}
