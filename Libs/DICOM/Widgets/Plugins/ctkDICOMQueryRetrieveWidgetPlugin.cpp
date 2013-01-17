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
#include "ctkDICOMQueryRetrieveWidgetPlugin.h"
#include "ctkDICOMQueryRetrieveWidget.h"

//-----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidgetPlugin::ctkDICOMQueryRetrieveWidgetPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkDICOMQueryRetrieveWidgetPlugin::createWidget(QWidget *parentForWidget)
{
  ctkDICOMQueryRetrieveWidget* newWidget = new ctkDICOMQueryRetrieveWidget(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkDICOMQueryRetrieveWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkDICOMQueryRetrieveWidget\" \
          name=\"QueryRetrieve\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDICOMQueryRetrieveWidgetPlugin::icon() const
{
  return QIcon(":/Icons/listview.png");
}

//-----------------------------------------------------------------------------
QString ctkDICOMQueryRetrieveWidgetPlugin::includeFile() const
{
  return "ctkDICOMQueryRetrieveWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkDICOMQueryRetrieveWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkDICOMQueryRetrieveWidgetPlugin::name() const
{
  return "ctkDICOMQueryRetrieveWidget";
}
