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
#include "ctkPathLineEditPlugin.h"
#include "ctkPathLineEdit.h"

//-----------------------------------------------------------------------------
ctkPathLineEditPlugin::ctkPathLineEditPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkPathLineEditPlugin::createWidget(QWidget* widgetParent)
{
  ctkPathLineEdit* newWidget = new ctkPathLineEdit(widgetParent);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkPathLineEditPlugin::domXml() const
{
  return "<widget class=\"ctkPathLineEdit\" \
          name=\"PathLineEdit\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkPathLineEditPlugin::icon() const
{
  return QIcon(":/Icons/pushbutton.png");
}

//-----------------------------------------------------------------------------
QString ctkPathLineEditPlugin::includeFile() const
{
  return "ctkPathLineEdit.h";
}

//-----------------------------------------------------------------------------
bool ctkPathLineEditPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkPathLineEditPlugin::name() const
{
  return "ctkPathLineEdit";
}
