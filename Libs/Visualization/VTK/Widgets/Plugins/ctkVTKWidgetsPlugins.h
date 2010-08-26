/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

#ifndef __ctkVTKWidgetsPlugins_h
#define __ctkVTKWidgetsPlugins_h

// Qt includes
#include <QDesignerCustomWidgetCollectionInterface>

// CTK includes
#include "CTKVisualizationVTKWidgetsPluginsExport.h"
#include "ctkVTKRenderViewPlugin.h"
#include "ctkVTKScalarBarWidgetPlugin.h"
#include "ctkVTKScalarsToColorsViewPlugin.h"
#include "ctkVTKSliceViewPlugin.h"
#include "ctkVTKTextPropertyWidgetPlugin.h"

/// \class Group the plugins in one library
class CTK_VISUALIZATION_VTK_WIDGETS_PLUGINS_EXPORT ctkVTKWidgetsPlugins :
  public QObject,
  public QDesignerCustomWidgetCollectionInterface
{
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface);

public:
  QList<QDesignerCustomWidgetInterface*> customWidgets() const
    {
    QList<QDesignerCustomWidgetInterface *> plugins;
    plugins << new ctkVTKRenderViewPlugin;
    plugins << new ctkVTKScalarBarWidgetPlugin;
    plugins << new ctkVTKScalarsToColorsViewPlugin;
    plugins << new ctkVTKSliceViewPlugin;
    plugins << new ctkVTKTextPropertyWidgetPlugin;
    return plugins;
    }
};

#endif
