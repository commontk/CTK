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
#include "ctkDirectoryButtonPlugin.h"
#include "ctkDirectoryButton.h"

//-----------------------------------------------------------------------------
ctkDirectoryButtonPlugin::ctkDirectoryButtonPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkDirectoryButtonPlugin::createWidget(QWidget *_parent)
{
  ctkDirectoryButton* _widget = new ctkDirectoryButton(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkDirectoryButtonPlugin::domXml() const
{
  return "<widget class=\"ctkDirectoryButton\" \
          name=\"CTKDirectoryButton\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDirectoryButtonPlugin::icon() const
{
  return QIcon(":/Icons/pushbutton.png");
}

//-----------------------------------------------------------------------------
QString ctkDirectoryButtonPlugin::includeFile() const
{
  return "ctkDirectoryButton.h";
}

//-----------------------------------------------------------------------------
bool ctkDirectoryButtonPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkDirectoryButtonPlugin::name() const
{
  return "ctkDirectoryButton";
}
