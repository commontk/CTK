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

#ifndef __ctkScriptingPythonWidgetsAbstractPlugin_h
#define __ctkScriptingPythonWidgetsAbstractPlugin_h

// Qt includes
#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5,5,0)
#include <QDesignerCustomWidgetInterface>
#else
#include <QtUiPlugin/QDesignerCustomWidgetInterface>
#endif

// CTK includes
#include "ctkScriptingPythonWidgetsPluginsExport.h"

class CTK_SCRIPTING_PYTHON_WIDGETS_PLUGINS_EXPORT ctkScriptingPythonWidgetsAbstractPlugin :
  public QDesignerCustomWidgetInterface
{
  Q_INTERFACES(QDesignerCustomWidgetInterface);
public:

  ctkScriptingPythonWidgetsAbstractPlugin();
  
  // Do *NOT* reimplement this method.
  QString group() const;
  
  // You can reimplement these methods
  virtual QIcon icon() const;
  virtual QString toolTip() const;
  virtual QString whatsThis() const;
  virtual void initialize(QDesignerFormEditorInterface *formEditor);
  
protected:
  bool Initialized;
};

#endif
