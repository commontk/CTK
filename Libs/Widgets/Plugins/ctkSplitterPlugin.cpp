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
#include "ctkSplitterPlugin.h"
#include "ctkSplitter.h"

//-----------------------------------------------------------------------------
ctkSplitterPlugin::ctkSplitterPlugin(QObject* parentObject)
  : QObject(parentObject)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkSplitterPlugin::createWidget(QWidget* parentWidget)
{
  ctkSplitter* newWidget = new ctkSplitter(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkSplitterPlugin::domXml() const
{
  return "<widget class=\"ctkSplitter\" name=\"Splitter\">\n"
         " <property name=\"orientation\">\n"
         "  <enum>Qt::Horizontal</enum>\n"
         " </property>\n"
         "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkSplitterPlugin::icon() const
{
  return QIcon(":/Icons/splitter.png");
}

//-----------------------------------------------------------------------------
QString ctkSplitterPlugin::includeFile() const
{
  return "ctkSplitter.h";
}

//-----------------------------------------------------------------------------
bool ctkSplitterPlugin::isContainer() const
{
  return true;
}

//-----------------------------------------------------------------------------
QString ctkSplitterPlugin::name() const
{
  return "ctkSplitter";
}
