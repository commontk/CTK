PROJECT(${PROJECT_NAME}Plugins)

#
# See CTK/CMake/ctkMacroBuildQtDesignerPlugin.cmake for details
#

SET(PLUGIN_export_directive "CTK_VISUALIZATION_VTK_WIDGETS_PLUGINS_EXPORT")

# Source files
SET(PLUGIN_SRCS
  ctkVTKWidgetsPlugins.cpp
  ctkVTKWidgetsPlugins.h
  ctkVTKWidgetsAbstractPlugin.cpp
  ctkVTKWidgetsAbstractPlugin.h

  ctkVTKRenderViewPlugin.cpp
  ctkVTKRenderViewPlugin.h
  ctkVTKScalarBarWidgetPlugin.cpp
  ctkVTKScalarBarWidgetPlugin.h
  ctkVTKScalarsToColorsViewPlugin.cpp
  ctkVTKScalarsToColorsViewPlugin.h
  ctkVTKSliceViewPlugin.cpp
  ctkVTKSliceViewPlugin.h
  ctkVTKTextPropertyWidgetPlugin.cpp
  ctkVTKTextPropertyWidgetPlugin.h
  )

# Headers that should run through moc
SET(PLUGIN_MOC_SRCS
  ctkVTKWidgetsPlugins.h

  ctkVTKRenderViewPlugin.h
  ctkVTKScalarBarWidgetPlugin.h
  ctkVTKScalarsToColorsViewPlugin.h
  ctkVTKSliceViewPlugin.h
  ctkVTKTextPropertyWidgetPlugin.h
  )

# Resources
SET(PLUGIN_resources
  #Resources/CTKVisualizationVTKWidgetsPlugins.qrc
)

# Target libraries
SET(PLUGIN_target_libraries
  CTKVisualizationVTKWidgets
  )

ctkMacroBuildQtDesignerPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
