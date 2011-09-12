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
#include "ctkDICOMQueryRetrievePlugin.h"
#include "ctkDICOMQueryRetrieve.h"

//-----------------------------------------------------------------------------
ctkDICOMQueryRetrievePlugin::ctkDICOMQueryRetrievePlugin(QObject* pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkDICOMQueryRetrievePlugin::createWidget(QWidget *parentForWidget)
{
  ctkDICOMQueryRetrieve* newWidget = new ctkDICOMQueryRetrieve(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkDICOMQueryRetrievePlugin::domXml() const
{
  return "<widget class=\"ctkDICOMQueryRetrieve\" \
          name=\"QueryRetrieve\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDICOMQueryRetrievePlugin::icon() const
{
  return QIcon(":/Icons/listview.png");
}

//-----------------------------------------------------------------------------
QString ctkDICOMQueryRetrievePlugin::includeFile() const
{
  return "ctkDICOMQueryRetrieve.h";
}

//-----------------------------------------------------------------------------
bool ctkDICOMQueryRetrievePlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkDICOMQueryRetrievePlugin::name() const
{
  return "ctkDICOMQueryRetrieve";
}
