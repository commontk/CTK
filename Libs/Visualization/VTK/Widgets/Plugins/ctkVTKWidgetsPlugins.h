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

#ifndef __ctkVTKWidgetsPlugins_h
#define __ctkVTKWidgetsPlugins_h

// Qt includes
#include <QtGlobal>
#ifndef HAVE_QT5
#include <QDesignerCustomWidgetCollectionInterface>
#else
#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>
#endif

// CTK includes
#include "ctkVisualizationVTKWidgetsPluginsExport.h"
#include "ctkVTKDataSetArrayComboBoxPlugin.h"
#include "ctkVTKMagnifyViewPlugin.h"
#include "ctkVTKPropertyWidgetPlugin.h"
#include "ctkVTKRenderViewPlugin.h"
#include "ctkVTKScalarBarWidgetPlugin.h"
#ifdef CTK_USE_CHARTS
#include "ctkVTKChartViewPlugin.h"
#include "ctkVTKScalarsToColorsViewPlugin.h"
#include "ctkVTKScalarsToColorsWidgetPlugin.h"
#include "ctkVTKVolumePropertyWidgetPlugin.h"
#endif
#include "ctkVTKSliceViewPlugin.h"
#include "ctkVTKSurfaceMaterialPropertyWidgetPlugin.h"
#include "ctkVTKTextPropertyWidgetPlugin.h"
#include "ctkVTKThresholdWidgetPlugin.h"

/// \class Group the plugins in one library
class CTK_VISUALIZATION_VTK_WIDGETS_PLUGINS_EXPORT ctkVTKWidgetsPlugins
  : public QObject
  , public QDesignerCustomWidgetCollectionInterface
{
  Q_OBJECT
#ifdef HAVE_QT5
  Q_PLUGIN_METADATA(IID "org.commontk.VTKWidgets")
#endif
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface);

public:
  QList<QDesignerCustomWidgetInterface*> customWidgets() const
    {
    QList<QDesignerCustomWidgetInterface *> plugins;
    plugins << new ctkVTKDataSetArrayComboBoxPlugin;
    plugins << new ctkVTKMagnifyViewPlugin;
    plugins << new ctkVTKPropertyWidgetPlugin;
    plugins << new ctkVTKRenderViewPlugin;
    plugins << new ctkVTKScalarBarWidgetPlugin;
#ifdef CTK_USE_CHARTS
    plugins << new ctkVTKChartViewPlugin;
    plugins << new ctkVTKScalarsToColorsViewPlugin;
    plugins << new ctkVTKScalarsToColorsWidgetPlugin;
    plugins << new ctkVTKVolumePropertyWidgetPlugin;
#endif
    plugins << new ctkVTKSliceViewPlugin;
    plugins << new ctkVTKSurfaceMaterialPropertyWidgetPlugin;
    plugins << new ctkVTKTextPropertyWidgetPlugin;
    plugins << new ctkVTKThresholdWidgetPlugin;
    return plugins;
    }
};

#endif
