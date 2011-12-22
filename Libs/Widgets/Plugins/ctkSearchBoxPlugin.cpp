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
#include "ctkSearchBoxPlugin.h"
#include "ctkSearchBox.h"

//-----------------------------------------------------------------------------
ctkSearchBoxPlugin::ctkSearchBoxPlugin(QObject *_parent)
  : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkSearchBoxPlugin::createWidget(QWidget *_parent)
{
  ctkSearchBox* _widget = new ctkSearchBox(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkSearchBoxPlugin::domXml() const
{
  return "<widget class=\"ctkSearchBox\" \
          name=\"SearchBox\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkSearchBoxPlugin::icon() const
{
  return QIcon(":/Icons/search.svg");
}

//-----------------------------------------------------------------------------
QString ctkSearchBoxPlugin::includeFile() const
{
  return "ctkSearchBox.h";
}

//-----------------------------------------------------------------------------
bool ctkSearchBoxPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkSearchBoxPlugin::name() const
{
  return "ctkSearchBox";
}
