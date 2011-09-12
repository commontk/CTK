PROJECT(${PROJECT_NAME}Plugins)

#
# See CTK/CMake/ctkMacroBuildQtDesignerPlugin.cmake for details
#

SET(PLUGIN_export_directive "CTK_DICOM_WIDGETS_PLUGINS_EXPORT")

# Source files
SET(PLUGIN_SRCS
  ctkDICOMWidgetsPlugins.cpp
  ctkDICOMWidgetsPlugins.h
  ctkDICOMWidgetsAbstractPlugin.cpp
  ctkDICOMWidgetsAbstractPlugin.h
  ctkQueryRetrieveWidgetPlugin.cpp
  ctkQueryRetrieveWidgetPlugin.h
  )

# Headers that should run through moc
SET(PLUGIN_MOC_SRCS
  ctkDICOMWidgetsPlugins.h

  ctkQueryRetrieveWidgetPlugin.h
  )

# Resources
SET(PLUGIN_resources
)

# Target libraries
SET(PLUGIN_target_libraries
  CTKDICOMWidgets
  )

ctkMacroBuildQtDesignerPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
