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
#include "ctkVTKWidgetsAbstractPlugin.h"

//-----------------------------------------------------------------------------
ctkVTKWidgetsAbstractPlugin::ctkVTKWidgetsAbstractPlugin()
{
  this->Initialized = false;
}

//-----------------------------------------------------------------------------
QString ctkVTKWidgetsAbstractPlugin::group() const
{ 
  return "CTK [VTK Widgets]";
}

//-----------------------------------------------------------------------------
QIcon ctkVTKWidgetsAbstractPlugin::icon() const
{
  return QIcon(); 
}

//-----------------------------------------------------------------------------
QString ctkVTKWidgetsAbstractPlugin::toolTip() const
{ 
  return QString(); 
}

//-----------------------------------------------------------------------------
QString ctkVTKWidgetsAbstractPlugin::whatsThis() const
{
  return QString(); 
}

//-----------------------------------------------------------------------------
void ctkVTKWidgetsAbstractPlugin::initialize(QDesignerFormEditorInterface *formEditor)
{
  Q_UNUSED(formEditor);
  if (this->Initialized)
    {
    return;
    }
  this->Initialized = true;
}
