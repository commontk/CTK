/*=========================================================================

  Library:   CTK
 
  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

// CTK includes
#include "ctkTitleComboBoxPlugin.h"
#include "ctkTitleComboBox.h"

//-----------------------------------------------------------------------------
ctkTitleComboBoxPlugin::ctkTitleComboBoxPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkTitleComboBoxPlugin::createWidget(QWidget *_parent)
{
  ctkTitleComboBox* _widget = new ctkTitleComboBox(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkTitleComboBoxPlugin::domXml() const
{
  return "<widget class=\"ctkTitleComboBox\" \
          name=\"CTKTitleComboBox\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkTitleComboBoxPlugin::icon() const
{
  return QIcon(":/Icons/combobox.png");
}

//-----------------------------------------------------------------------------
QString ctkTitleComboBoxPlugin::includeFile() const
{
  return "ctkTitleComboBox.h";
}

//-----------------------------------------------------------------------------
bool ctkTitleComboBoxPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkTitleComboBoxPlugin::name() const
{
  return "ctkTitleComboBox";
}
