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
// #include "qCTKCollapsibleButtonPlugin.h"
// #include "qCTKCollapsibleGroupBoxPlugin.h"
// #include "qCTKColorPickerButtonPlugin.h"
// #include "qCTKCoordinatesWidgetPlugin.h"
// #include "qCTKDoubleRangeSliderPlugin.h"
// #include "qCTKDoubleSliderPlugin.h"
// #include "qCTKDynamicSpacerPlugin.h"
// #include "qCTKFittedTextBrowserPlugin.h"
// #include "qCTKMenuButtonPlugin.h"
// #include "qCTKRangeSliderPlugin.h"
// #include "qCTKRangeWidgetPlugin.h"
// #include "qCTKTitleComboBoxPlugin.h"
// #include "qCTKTreeComboBoxPlugin.h"
// #include "qCTKSliderSpinBoxWidgetPlugin.h"

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
//     plugins << new qCTKCollapsibleButtonPlugin
//             << new qCTKCollapsibleGroupBoxPlugin
//             << new qCTKColorPickerButtonPlugin
//             << new qCTKCoordinatesWidgetPlugin
//             << new qCTKDoubleRangeSliderPlugin
//             << new qCTKDoubleSliderPlugin
//             << new qCTKDynamicSpacerPlugin
//             << new qCTKFittedTextBrowserPlugin
//             << new qCTKMenuButtonPlugin
//             << new qCTKRangeSliderPlugin
//             << new qCTKRangeWidgetPlugin
//             << new qCTKTitleComboBoxPlugin
//             << new qCTKTreeComboBoxPlugin
//             << new qCTKSliderSpinBoxWidgetPlugin;
    return plugins;
    }
};

#endif
