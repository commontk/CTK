project(${PROJECT_NAME}Plugins)

#
# See CTK/CMake/ctkMacroBuildQtPlugin.cmake for details
#

set(PLUGIN_export_directive "CTK_VISUALIZATION_VTK_WIDGETS_PLUGINS_EXPORT")

# Source files
set(PLUGIN_SRCS
  ctkVTKWidgetsPlugins.cpp
  ctkVTKWidgetsPlugins.h
  ctkVTKWidgetsAbstractPlugin.cpp
  ctkVTKWidgetsAbstractPlugin.h

  ctkVTKDataSetArrayComboBoxPlugin.cpp
  ctkVTKDataSetArrayComboBoxPlugin.h
  ctkVTKMagnifyViewPlugin.cpp
  ctkVTKMagnifyViewPlugin.h
  ctkVTKPropertyWidgetPlugin.cpp
  ctkVTKPropertyWidgetPlugin.h
  ctkVTKRenderViewPlugin.cpp
  ctkVTKRenderViewPlugin.h
  ctkVTKScalarBarWidgetPlugin.cpp
  ctkVTKScalarBarWidgetPlugin.h
  ctkVTKSliceViewPlugin.cpp
  ctkVTKSliceViewPlugin.h
  ctkVTKSurfaceMaterialPropertyWidgetPlugin.cpp
  ctkVTKSurfaceMaterialPropertyWidgetPlugin.h
  ctkVTKTextPropertyWidgetPlugin.cpp
  ctkVTKTextPropertyWidgetPlugin.h
  ctkVTKThresholdWidgetPlugin.cpp
  ctkVTKThresholdWidgetPlugin.h
  )

if(CTK_LIB_Visualization/VTK/Widgets_USE_TRANSFER_FUNCTION_CHARTS)
  set(PLUGIN_SRCS
      ctkVTKChartViewPlugin.cpp
      ctkVTKChartViewPlugin.h
      ctkVTKScalarsToColorsViewPlugin.cpp
      ctkVTKScalarsToColorsViewPlugin.h
      ctkVTKScalarsToColorsWidgetPlugin.cpp
      ctkVTKScalarsToColorsWidgetPlugin.h
      ctkVTKVolumePropertyWidgetPlugin.cpp
      ctkVTKVolumePropertyWidgetPlugin.h
      ${PLUGIN_SRCS})
endif()

# Resources
set(PLUGIN_resources
  #Resources/CTKVisualizationVTKWidgetsPlugins.qrc
)

# Target libraries
set(PLUGIN_target_libraries
  CTKVisualizationVTKWidgets
  )

ctkMacroBuildQtDesignerPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
