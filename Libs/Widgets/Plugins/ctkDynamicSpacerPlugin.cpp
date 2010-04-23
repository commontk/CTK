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
#include "ctkDynamicSpacerPlugin.h"
#include "ctkDynamicSpacer.h"

// --------------------------------------------------------------------------
ctkDynamicSpacerPlugin::ctkDynamicSpacerPlugin(QObject *_parent)
  : QObject(_parent)
{
}

// --------------------------------------------------------------------------
QWidget *ctkDynamicSpacerPlugin::createWidget(QWidget *_parent)
{
  ctkDynamicSpacer* _widget = new ctkDynamicSpacer(_parent);
  return _widget;
}

// --------------------------------------------------------------------------
QString ctkDynamicSpacerPlugin::domXml() const
{
  return "<widget class=\"ctkDynamicSpacer\" name=\"CTKSpacer\">\n"
    "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDynamicSpacerPlugin::icon() const
{
  return QIcon(":/Icons/vspacer.png");
}

// --------------------------------------------------------------------------
QString ctkDynamicSpacerPlugin::includeFile() const
{
  return "ctkDynamicSpacer.h";
}

// --------------------------------------------------------------------------
bool ctkDynamicSpacerPlugin::isContainer() const
{
  return false;
}

// --------------------------------------------------------------------------
QString ctkDynamicSpacerPlugin::name() const
{
  return "ctkDynamicSpacer";
}
