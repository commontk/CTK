/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkVTKWidgetsPlugins_h
#define __ctkVTKWidgetsPlugins_h

// Qt includes
#include <QDesignerCustomWidgetCollectionInterface>

// CTK includes
#include "CTKVisualizationVTKWidgetsPluginsExport.h"
#include "ctkVTKRenderViewPlugin.h"


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
    return plugins;
    }
};

#endif
