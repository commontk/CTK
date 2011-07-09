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
#include "ctkComboBoxPlugin.h"
#include "ctkComboBox.h"

//-----------------------------------------------------------------------------
ctkComboBoxPlugin::ctkComboBoxPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkComboBoxPlugin::createWidget(QWidget *_parent)
{
  ctkComboBox* _widget = new ctkComboBox(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkComboBoxPlugin::domXml() const
{
  return "<widget class=\"ctkComboBox\" \
          name=\"ComboBox\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkComboBoxPlugin::icon() const
{
  return QIcon(":/Icons/combobox.png");
}

//-----------------------------------------------------------------------------
QString ctkComboBoxPlugin::includeFile() const
{
  return "ctkComboBox.h";
}

//-----------------------------------------------------------------------------
bool ctkComboBoxPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkComboBoxPlugin::name() const
{
  return "ctkComboBox";
}
