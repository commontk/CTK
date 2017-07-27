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

#ifndef __ctkWidgetsPlugins_h
#define __ctkWidgetsPlugins_h

// Qt includes
#include <QtGlobal>
#ifndef HAVE_QT5
#include <QDesignerCustomWidgetCollectionInterface>
#else
#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>
#endif

// CTK includes
#include "ctkWidgetsPluginsExport.h"
#include "ctkActionsWidgetPlugin.h"
#include "ctkAxesWidgetPlugin.h"
#include "ctkCheckableComboBoxPlugin.h"
#include "ctkCheckablePushButtonPlugin.h"
#include "ctkCheckBoxPlugin.h"
#include "ctkCollapsibleButtonPlugin.h"
#include "ctkCollapsibleGroupBoxPlugin.h"
#include "ctkColorPickerButtonPlugin.h"
#include "ctkComboBoxPlugin.h"
#include "ctkCoordinatesWidgetPlugin.h"
#include "ctkCrosshairLabelPlugin.h"
#include "ctkDirectoryButtonPlugin.h"
#include "ctkDoubleRangeSliderPlugin.h"
#include "ctkDoubleSliderPlugin.h"
#include "ctkDoubleSpinBoxPlugin.h"
#include "ctkDynamicSpacerPlugin.h"
#include "ctkExpandButtonPlugin.h"
#include "ctkExpandableWidgetPlugin.h"
#include "ctkFittedTextBrowserPlugin.h"
#include "ctkFontButtonPlugin.h"
#include "ctkMaterialPropertyPreviewLabelPlugin.h"
#include "ctkMaterialPropertyWidgetPlugin.h"
#include "ctkMatrixWidgetPlugin.h"
#include "ctkMenuButtonPlugin.h"
#include "ctkModalityWidgetPlugin.h"
#include "ctkPathLineEditPlugin.h"
#include "ctkPathListButtonsWidgetPlugin.h"
#include "ctkPathListWidgetPlugin.h"
#include "ctkPopupWidgetPlugin.h"
#include "ctkPushButtonPlugin.h"
#include "ctkRangeSliderPlugin.h"
#include "ctkRangeWidgetPlugin.h"
#include "ctkSearchBoxPlugin.h"
#include "ctkSettingsPanelPlugin.h"
#include "ctkSettingsDialogPlugin.h"
#include "ctkSliderWidgetPlugin.h"
#include "ctkThumbnailLabelPlugin.h"
#include "ctkTransferFunctionViewPlugin.h"
#include "ctkTreeComboBoxPlugin.h"
#include "ctkWorkflowButtonBoxWidgetPlugin.h"
#include "ctkWorkflowWidgetStepPlugin.h"

/// \class Group the plugins in one library
class CTK_WIDGETS_PLUGINS_EXPORT ctkWidgetsPlugins
  : public QObject
  , public QDesignerCustomWidgetCollectionInterface
{
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
#ifdef HAVE_QT5
  Q_PLUGIN_METADATA(IID "org.commontk.Widgets")
#endif
public:
  QList<QDesignerCustomWidgetInterface*> customWidgets() const
    {
    QList<QDesignerCustomWidgetInterface *> plugins;
    plugins << new ctkActionsWidgetPlugin
            << new ctkAxesWidgetPlugin
            << new ctkCheckableComboBoxPlugin
            << new ctkCheckablePushButtonPlugin
            << new ctkCheckBoxPlugin
            << new ctkCollapsibleButtonPlugin
            << new ctkCollapsibleGroupBoxPlugin
            << new ctkColorPickerButtonPlugin
            << new ctkComboBoxPlugin
            << new ctkCoordinatesWidgetPlugin
            << new ctkCrosshairLabelPlugin
            << new ctkDirectoryButtonPlugin
            << new ctkDoubleRangeSliderPlugin
            << new ctkDoubleSliderPlugin
            << new ctkDoubleSpinBoxPlugin
            << new ctkDynamicSpacerPlugin
            << new ctkExpandButtonPlugin
            << new ctkExpandableWidgetPlugin
            << new ctkFittedTextBrowserPlugin
            << new ctkFontButtonPlugin
            << new ctkMaterialPropertyPreviewLabelPlugin
            << new ctkMaterialPropertyWidgetPlugin
            << new ctkMatrixWidgetPlugin
            << new ctkMenuButtonPlugin
            << new ctkModalityWidgetPlugin
            << new ctkPathLineEditPlugin
            << new ctkPathListButtonsWidgetPlugin
            << new ctkPathListWidgetPlugin
            << new ctkPopupWidgetPlugin
            << new ctkPushButtonPlugin
            << new ctkRangeSliderPlugin
            << new ctkRangeWidgetPlugin
            << new ctkSearchBoxPlugin
            << new ctkSettingsPanelPlugin
            << new ctkSettingsDialogPlugin
            << new ctkSliderWidgetPlugin
            << new ctkThumbnailLabelPlugin
            << new ctkTransferFunctionViewPlugin
            << new ctkTreeComboBoxPlugin
            << new ctkWorkflowButtonBoxWidgetPlugin
            << new ctkWorkflowWidgetStepPlugin;
    return plugins;
    }
};

#endif
