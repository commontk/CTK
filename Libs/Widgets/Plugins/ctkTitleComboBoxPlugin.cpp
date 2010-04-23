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
