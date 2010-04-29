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

#ifndef __ctkWidgetsPlugins_h
#define __ctkWidgetsPlugins_h

// Qt includes
#include <QDesignerCustomWidgetCollectionInterface>

// CTK includes
#include "CTKWidgetsPluginsExport.h"
#include "ctkCollapsibleButtonPlugin.h"
#include "ctkCollapsibleGroupBoxPlugin.h"
#include "ctkColorPickerButtonPlugin.h"
#include "ctkCoordinatesWidgetPlugin.h"
#include "ctkDirectoryButtonPlugin.h"
#include "ctkDoubleRangeSliderPlugin.h"
#include "ctkDoubleSliderPlugin.h"
#include "ctkDynamicSpacerPlugin.h"
#include "ctkFittedTextBrowserPlugin.h"
#include "ctkMenuButtonPlugin.h"
#include "ctkRangeSliderPlugin.h"
#include "ctkRangeWidgetPlugin.h"
#include "ctkTitleComboBoxPlugin.h"
#include "ctkTreeComboBoxPlugin.h"
#include "ctkSliderSpinBoxWidgetPlugin.h"

/// \class Group the plugins in one library
class CTK_WIDGETS_PLUGINS_EXPORT ctkWidgetsPlugins : public QObject,
      public QDesignerCustomWidgetCollectionInterface
{
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface);

public:
  QList<QDesignerCustomWidgetInterface*> customWidgets() const
    {
    QList<QDesignerCustomWidgetInterface *> plugins;
    plugins << new ctkCollapsibleButtonPlugin
            << new ctkCollapsibleGroupBoxPlugin
            << new ctkColorPickerButtonPlugin
            << new ctkCoordinatesWidgetPlugin
            << new ctkDirectoryButtonPlugin
            << new ctkDoubleRangeSliderPlugin
            << new ctkDoubleSliderPlugin
            << new ctkDynamicSpacerPlugin
            << new ctkFittedTextBrowserPlugin
            << new ctkMenuButtonPlugin
            << new ctkRangeSliderPlugin
            << new ctkRangeWidgetPlugin
            << new ctkTitleComboBoxPlugin
            << new ctkTreeComboBoxPlugin
            << new ctkSliderSpinBoxWidgetPlugin;
    return plugins;
    }
};

#endif
