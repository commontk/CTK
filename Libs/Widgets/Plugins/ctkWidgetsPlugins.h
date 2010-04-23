/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
