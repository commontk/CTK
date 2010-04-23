/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// CTK includes
#include "ctkTreeComboBoxPlugin.h"
#include "ctkTreeComboBox.h"

//-----------------------------------------------------------------------------
ctkTreeComboBoxPlugin::ctkTreeComboBoxPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkTreeComboBoxPlugin::createWidget(QWidget *_parent)
{
  ctkTreeComboBox* _widget = new ctkTreeComboBox(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkTreeComboBoxPlugin::domXml() const
{
  return "<widget class=\"ctkTreeComboBox\" \
          name=\"CTKTreeComboBox\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkTreeComboBoxPlugin::icon() const
{
  return QIcon(":/Icons/combobox.png");
}

//-----------------------------------------------------------------------------
QString ctkTreeComboBoxPlugin::includeFile() const
{
  return "ctkTreeComboBox.h";
}

//-----------------------------------------------------------------------------
bool ctkTreeComboBoxPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkTreeComboBoxPlugin::name() const
{
  return "ctkTreeComboBox";
}
