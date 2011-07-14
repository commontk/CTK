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
#include "ctkScriptingPythonWidgetsAbstractPlugin.h"

//-----------------------------------------------------------------------------
ctkScriptingPythonWidgetsAbstractPlugin::ctkScriptingPythonWidgetsAbstractPlugin()
{
  this->Initialized = false;
}

//-----------------------------------------------------------------------------
QString ctkScriptingPythonWidgetsAbstractPlugin::group() const
{ 
  return "CTK [ScriptingPythonWidgets]";
}

//-----------------------------------------------------------------------------
QIcon ctkScriptingPythonWidgetsAbstractPlugin::icon() const
{
  return QIcon(); 
}

//-----------------------------------------------------------------------------
QString ctkScriptingPythonWidgetsAbstractPlugin::toolTip() const
{ 
  return QString(); 
}

//-----------------------------------------------------------------------------
QString ctkScriptingPythonWidgetsAbstractPlugin::whatsThis() const
{
  return QString(); 
}

//-----------------------------------------------------------------------------
void ctkScriptingPythonWidgetsAbstractPlugin::initialize(QDesignerFormEditorInterface *formEditor)
{
  Q_UNUSED(formEditor);
  if (this->Initialized)
    {
    return;
    }
  this->Initialized = true;
}
