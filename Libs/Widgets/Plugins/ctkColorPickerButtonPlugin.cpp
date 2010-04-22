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
          name=\"CTKColorPickerButton\">\n"
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
