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
#include "ctkWidgetsAbstractPlugin.h"

//-----------------------------------------------------------------------------
ctkWidgetsAbstractPlugin::ctkWidgetsAbstractPlugin()
{
  this->Initialized = false;
}

//-----------------------------------------------------------------------------
QString ctkWidgetsAbstractPlugin::group() const
{ 
  return "CTK [Widgets]";
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
