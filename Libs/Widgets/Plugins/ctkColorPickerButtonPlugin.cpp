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
#include "ctkColorPickerButtonPlugin.h"
#include "ctkColorPickerButton.h"

//-----------------------------------------------------------------------------
ctkColorPickerButtonPlugin::ctkColorPickerButtonPlugin(QObject *_parent) : QObject(_parent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkColorPickerButtonPlugin::createWidget(QWidget *_parent)
{
  ctkColorPickerButton* _widget = new ctkColorPickerButton(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkColorPickerButtonPlugin::domXml() const
{
  return "<widget class=\"ctkColorPickerButton\" \
          name=\"ColorPickerButton\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon ctkColorPickerButtonPlugin::icon() const
{
  return QIcon(":/Icons/pushbutton.png");
}

//-----------------------------------------------------------------------------
QString ctkColorPickerButtonPlugin::includeFile() const
{
  return "ctkColorPickerButton.h";
}

//-----------------------------------------------------------------------------
bool ctkColorPickerButtonPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkColorPickerButtonPlugin::name() const
{
  return "ctkColorPickerButton";
}
