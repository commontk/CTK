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
#include "ctkWidgetsAbstractPlugin.h"

//-----------------------------------------------------------------------------
ctkWidgetsAbstractPlugin::ctkWidgetsAbstractPlugin()
{
  this->Initialized = false;
}

//-----------------------------------------------------------------------------
QString ctkWidgetsAbstractPlugin::group() const
{ 
  return "[CTK Widgets]";
}

//-----------------------------------------------------------------------------
QIcon ctkWidgetsAbstractPlugin::icon() const
{
  return QIcon(); 
}

//-----------------------------------------------------------------------------
QString ctkWidgetsAbstractPlugin::toolTip() const
{ 
  return QString(); 
}

//-----------------------------------------------------------------------------
QString ctkWidgetsAbstractPlugin::whatsThis() const
{
  return QString(); 
}

//-----------------------------------------------------------------------------
void ctkWidgetsAbstractPlugin::initialize(QDesignerFormEditorInterface *formEditor)
{
  Q_UNUSED(formEditor);
  if (this->Initialized)
    {
    return;
    }
  this->Initialized = true;
}
