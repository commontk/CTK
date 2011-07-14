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
#include "ctkVTKDataSetArrayComboBoxPlugin.h"
#include "ctkVTKDataSetArrayComboBox.h"

//-----------------------------------------------------------------------------
ctkVTKDataSetArrayComboBoxPlugin::ctkVTKDataSetArrayComboBoxPlugin(QObject *_parent)
  : QObject(_parent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkVTKDataSetArrayComboBoxPlugin::createWidget(QWidget *_parent)
{
  ctkVTKDataSetArrayComboBox* _widget = new ctkVTKDataSetArrayComboBox(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkVTKDataSetArrayComboBoxPlugin::domXml() const
{
  return "<widget class=\"ctkVTKDataSetArrayComboBox\" \
          name=\"DataSetArrayComboBox\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkVTKDataSetArrayComboBoxPlugin::icon() const
{
  return QIcon(":/Icons/combobox.png");
}

//-----------------------------------------------------------------------------
QString ctkVTKDataSetArrayComboBoxPlugin::includeFile() const
{
  return "ctkVTKDataSetArrayComboBox.h";
}

//-----------------------------------------------------------------------------
bool ctkVTKDataSetArrayComboBoxPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkVTKDataSetArrayComboBoxPlugin::name() const
{
  return "ctkVTKDataSetArrayComboBox";
}
