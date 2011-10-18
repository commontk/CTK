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

  ctkVTKDataSetArrayComboBoxPlugin.cpp
  ctkVTKDataSetArrayComboBoxPlugin.h
  ctkVTKMagnifyViewPlugin.cpp
  ctkVTKMagnifyViewPlugin.h
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

# Headers that should run through moc
SET(PLUGIN_MOC_SRCS
  ctkVTKWidgetsPlugins.h

  ctkVTKDataSetArrayComboBoxPlugin.h
  ctkVTKMagnifyViewPlugin.h
  ctkVTKRenderViewPlugin.h
  ctkVTKScalarBarWidgetPlugin.h
  ctkVTKSliceViewPlugin.h
  ctkVTKSurfaceMaterialPropertyWidgetPlugin.h
  ctkVTKTextPropertyWidgetPlugin.h
  ctkVTKThresholdWidgetPlugin.h
  )
  
IF(CTK_USE_CHARTS)
  SET(PLUGIN_SRCS
      ctkVTKChartViewPlugin.cpp
      ctkVTKChartViewPlugin.h
      ctkVTKScalarsToColorsViewPlugin.cpp
      ctkVTKScalarsToColorsViewPlugin.h
      ctkVTKScalarsToColorsWidgetPlugin.cpp
      ctkVTKScalarsToColorsWidgetPlugin.h
      ctkVTKVolumePropertyWidgetPlugin.cpp
      ctkVTKVolumePropertyWidgetPlugin.h
      ${PLUGIN_SRCS})
  SET(PLUGIN_MOC_SRCS
      ctkVTKChartViewPlugin.h
      ctkVTKScalarsToColorsViewPlugin.h
      ctkVTKScalarsToColorsWidgetPlugin.h
      ctkVTKVolumePropertyWidgetPlugin.h
      ${PLUGIN_MOC_SRCS})
ENDIF(CTK_USE_CHARTS)

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
