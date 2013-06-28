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
#include "ctkDoubleSpinBoxPlugin.h"
#include "ctkDoubleSpinBox.h"

// --------------------------------------------------------------------------
ctkDoubleSpinBoxPlugin::ctkDoubleSpinBoxPlugin(QObject *_parent)
  : QObject(_parent)
{
}

// --------------------------------------------------------------------------
QWidget *ctkDoubleSpinBoxPlugin::createWidget(QWidget *_parent)
{
  ctkDoubleSpinBox* newWidget = new ctkDoubleSpinBox(_parent);
  return newWidget;
}

// --------------------------------------------------------------------------
QString ctkDoubleSpinBoxPlugin::domXml() const
{
  return "<widget class=\"ctkDoubleSpinBox\" name=\"DoubleSpinBox\">\n"
    "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDoubleSpinBoxPlugin::icon() const
{
  return QIcon(":/Icons/doublespinbox.png");
}

// --------------------------------------------------------------------------
QString ctkDoubleSpinBoxPlugin::includeFile() const
{
  return "ctkDoubleSpinBox.h";
}

// --------------------------------------------------------------------------
bool ctkDoubleSpinBoxPlugin::isContainer() const
{
  return false;
}

// --------------------------------------------------------------------------
QString ctkDoubleSpinBoxPlugin::name() const
{
  return "ctkDoubleSpinBox";
}
