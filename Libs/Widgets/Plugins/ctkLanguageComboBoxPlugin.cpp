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
#include "ctkLanguageComboBoxPlugin.h"
#include "ctkLanguageComboBox.h"

//-----------------------------------------------------------------------------
ctkLanguageComboBoxPlugin::ctkLanguageComboBoxPlugin(QObject *_parent)
  : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkLanguageComboBoxPlugin::createWidget(QWidget *_parent)
{
  ctkLanguageComboBox* _widget = new ctkLanguageComboBox(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkLanguageComboBoxPlugin::domXml() const
{
  return "<widget class=\"ctkLanguageComboBox\" \
          name=\"LanguageComboBox\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkLanguageComboBoxPlugin::icon() const
{
  return QIcon(":Icons/combobox.png");
}

//-----------------------------------------------------------------------------
QString ctkLanguageComboBoxPlugin::includeFile() const
{
  return "ctkLanguageComboBox.h";
}

//-----------------------------------------------------------------------------
bool ctkLanguageComboBoxPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkLanguageComboBoxPlugin::name() const
{
  return "ctkLanguageComboBox";
}
