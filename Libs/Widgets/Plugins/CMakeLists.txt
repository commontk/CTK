PROJECT(${PROJECT_NAME}Plugins)

#
# See CTK/CMake/ctkMacroBuildQtDesignerPlugin.cmake for details
#

SET(PLUGIN_export_directive "CTK_WIDGETS_PLUGINS_EXPORT")

# Source files
SET(PLUGIN_SRCS
  ctkWidgetsPlugins.cpp
  ctkWidgetsPlugins.h
  ctkWidgetsAbstractPlugin.cpp
  ctkWidgetsAbstractPlugin.h

  ctkActionsWidgetPlugin.cpp
  ctkActionsWidgetPlugin.h
  ctkCollapsibleButtonPlugin.cpp
  ctkCollapsibleButtonPlugin.h
  ctkCollapsibleGroupBoxPlugin.cpp
  ctkCollapsibleGroupBoxPlugin.h
  ctkColorPickerButtonPlugin.cpp
  ctkColorPickerButtonPlugin.h
  ctkComboBoxPlugin.cpp
  ctkComboBoxPlugin.h
  ctkCoordinatesWidgetPlugin.cpp
  ctkCoordinatesWidgetPlugin.h
  ctkDirectoryButtonPlugin.cpp
  ctkDirectoryButtonPlugin.h
  ctkDoubleRangeSliderPlugin.cpp
  ctkDoubleRangeSliderPlugin.h
  ctkDoubleSliderPlugin.cpp
  ctkDoubleSliderPlugin.h
  ctkDynamicSpacerPlugin.cpp
  ctkDynamicSpacerPlugin.h
  #ctkExtensionFactory.cpp
  #ctkExtensionFactory.h
  ctkFittedTextBrowserPlugin.cpp
  ctkFittedTextBrowserPlugin.h
  ctkMatrixWidgetPlugin.cpp
  ctkMatrixWidgetPlugin.h
  ctkMenuButtonPlugin.cpp
  ctkMenuButtonPlugin.h
  ctkRangeSliderPlugin.cpp
  ctkRangeSliderPlugin.h
  ctkRangeWidgetPlugin.cpp
  ctkRangeWidgetPlugin.h
  ctkTransferFunctionViewPlugin.cpp
  ctkTransferFunctionViewPlugin.h
  ctkTreeComboBoxPlugin.cpp
  ctkTreeComboBoxPlugin.h
  ctkSliderSpinBoxWidgetPlugin.cpp
  ctkSliderSpinBoxWidgetPlugin.h
  )

# Headers that should run through moc
SET(PLUGIN_MOC_SRCS
  ctkWidgetsPlugins.h

  ctkActionsWidgetPlugin.h
  ctkCollapsibleButtonPlugin.h
  ctkCollapsibleGroupBoxPlugin.h
  ctkColorPickerButtonPlugin.h
  ctkComboBoxPlugin.h
  ctkCoordinatesWidgetPlugin.h
  ctkDirectoryButtonPlugin.h
  ctkDoubleRangeSliderPlugin.h
  ctkDoubleSliderPlugin.h
  ctkDynamicSpacerPlugin.h
  #ctkExtensionFactory.h
  ctkFittedTextBrowserPlugin.h
  ctkMatrixWidgetPlugin.h
  ctkMenuButtonPlugin.h
  ctkRangeSliderPlugin.h
  ctkRangeWidgetPlugin.h
  ctkTransferFunctionViewPlugin.h
  ctkTreeComboBoxPlugin.h
  ctkSliderSpinBoxWidgetPlugin.h
  )

# Resources
SET(PLUGIN_resources
  Resources/CTKWidgetsPlugins.qrc
)

# Target libraries
SET(PLUGIN_target_libraries
  CTKWidgets
  )

ctkMacroBuildQtDesignerPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
