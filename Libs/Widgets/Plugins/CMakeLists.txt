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

#   ctkCollapsibleButtonPlugin.cpp
#   ctkCollapsibleButtonPlugin.h
#   ctkCollapsibleGroupBoxPlugin.cpp
#   ctkCollapsibleGroupBoxPlugin.h
  ctkColorPickerButtonPlugin.cpp
  ctkColorPickerButtonPlugin.h
#   ctkCoordinatesWidgetPlugin.cpp
#   ctkCoordinatesWidgetPlugin.h
#   ctkDoubleRangeSliderPlugin.cpp
#   ctkDoubleRangeSliderPlugin.h
#   ctkDoubleSliderPlugin.cpp
#   ctkDoubleSliderPlugin.h
#   ctkDynamicSpacerPlugin.cpp
#   ctkDynamicSpacerPlugin.h
#   ctkExtensionFactory.cpp
#   ctkExtensionFactory.h
#   ctkFittedTextBrowserPlugin.cpp
#   ctkFittedTextBrowserPlugin.h
#   ctkMenuButtonPlugin.cpp
#   ctkMenuButtonPlugin.h
#   ctkRangeSliderPlugin.cpp
#   ctkRangeSliderPlugin.h
#   ctkRangeWidgetPlugin.cpp
#   ctkRangeWidgetPlugin.h
#   ctkTitleComboBoxPlugin.cpp
#   ctkTitleComboBoxPlugin.h
#   ctkTreeComboBoxPlugin.cpp
#   ctkTreeComboBoxPlugin.h
#   ctkSliderSpinBoxWidgetPlugin.cpp
#   ctkSliderSpinBoxWidgetPlugin.h
  )

# Headers that should run through moc
SET(PLUGIN_MOC_SRCS
  ctkWidgetsPlugins.h

#   ctkCollapsibleButtonPlugin.h
#   ctkCollapsibleGroupBoxPlugin.h
  ctkColorPickerButtonPlugin.h
#   ctkCoordinatesWidgetPlugin.h
#   ctkDoubleRangeSliderPlugin.h
#   ctkDoubleSliderPlugin.h
#   ctkDynamicSpacerPlugin.h
#   ctkExtensionFactory.h
#   ctkFittedTextBrowserPlugin.h
#   ctkMenuButtonPlugin.h
#   ctkRangeSliderPlugin.h
#   ctkRangeWidgetPlugin.h
#   ctkTitleComboBoxPlugin.h
#   ctkTreeComboBoxPlugin.h
#   ctkSliderSpinBoxWidgetPlugin.h
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

# TODO Move to a Slicer CMakeLists.txt
#qctk_install_designer_plugin(
#  NAME ${PROJECT_NAME}
#  INSTALL_BIN_DIR ${Slicer3_INSTALL_BIN_DIR}
#  INSTALL_LIB_DIR ${Slicer3_INSTALL_LIB_DIR}/designer
#  INSTALL_HEADERS_DIR ${Slicer3_INSTALL_INCLUDE_DIR}
#  )
