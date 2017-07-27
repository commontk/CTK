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

#ifndef __ctkScriptingPythonWidgetsPlugins_h
#define __ctkScriptingPythonWidgetsPlugins_h

// Qt includes
#include <QtGlobal>
#ifndef HAVE_QT5
#include <QDesignerCustomWidgetCollectionInterface>
#else
#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>
#endif

// CTK includes
#include "ctkScriptingPythonWidgetsPluginsExport.h"
#include "ctkPythonConsolePlugin.h"

/// \class Group the plugins in one library
class CTK_SCRIPTING_PYTHON_WIDGETS_PLUGINS_EXPORT ctkScriptingPythonWidgetsPlugins
  : public QObject
  , public QDesignerCustomWidgetCollectionInterface
{
  Q_OBJECT
#ifdef HAVE_QT5
  Q_PLUGIN_METADATA(IID "org.commontk.Python")
#endif
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface);

public:
  QList<QDesignerCustomWidgetInterface*> customWidgets() const
    {
    QList<QDesignerCustomWidgetInterface *> plugins;
    plugins << new ctkPythonConsolePlugin;
    return plugins;
    }
};

#endif
