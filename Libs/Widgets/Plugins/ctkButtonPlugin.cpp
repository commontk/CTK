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
#include "ctkButtonPlugin.h"
#include "ctkButton.h"

//-----------------------------------------------------------------------------
ctkButtonPlugin::ctkButtonPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkButtonPlugin::createWidget(QWidget *_parent)
{
  ctkButton* _widget = new ctkButton(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkButtonPlugin::domXml() const
{
  return "<widget class=\"ctkButton\" \
          name=\"Button\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkButtonPlugin::icon() const
{
  return QIcon(":/Icons/left.png");
}

//-----------------------------------------------------------------------------
QString ctkButtonPlugin::includeFile() const
{
  return "ctkButton.h";
}

//-----------------------------------------------------------------------------
bool ctkButtonPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkButtonPlugin::name() const
{
  return "ctkButton";
}
