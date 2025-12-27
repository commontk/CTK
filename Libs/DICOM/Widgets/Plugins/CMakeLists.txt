project(${PROJECT_NAME}Plugins)

#
# See CTK/CMake/ctkMacroBuildQtPlugin.cmake for details
#

set(PLUGIN_export_directive "CTK_DICOM_WIDGETS_PLUGINS_EXPORT")

# Source files
set(PLUGIN_SRCS
  ctkDICOMWidgetsPlugins.cpp
  ctkDICOMWidgetsPlugins.h
  ctkDICOMWidgetsAbstractPlugin.cpp
  ctkDICOMWidgetsAbstractPlugin.h

  ctkDICOMJobListWidgetPlugin.cpp
  ctkDICOMJobListWidgetPlugin.h

  ctkDICOMPatientViewPlugin.cpp
  ctkDICOMPatientViewPlugin.h

  ctkDICOMQueryRetrieveWidgetPlugin.cpp
  ctkDICOMQueryRetrieveWidgetPlugin.h

  ctkDICOMServerNodeWidget2Plugin.cpp
  ctkDICOMServerNodeWidget2Plugin.h

  ctkDICOMSeriesTableViewPlugin.cpp
  ctkDICOMSeriesTableViewPlugin.h

  ctkDICOMStudyListViewPlugin.cpp
  ctkDICOMStudyListViewPlugin.h

  ctkDICOMTableManagerPlugin.cpp
  ctkDICOMTableManagerPlugin.h
  ctkDICOMTableViewPlugin.cpp
  ctkDICOMTableViewPlugin.h

  ctkDICOMVisualBrowserWidgetPlugin.cpp
  ctkDICOMVisualBrowserWidgetPlugin.h
  )

# Headers that should run through moc
set(PLUGIN_MOC_SRCS
  ctkDICOMWidgetsPlugins.h
  ctkDICOMJobListWidgetPlugin.h
  ctkDICOMPatientViewPlugin.h
  ctkDICOMQueryRetrieveWidgetPlugin.h
  ctkDICOMServerNodeWidget2Plugin.h
  ctkDICOMSeriesTableViewPlugin.h
  ctkDICOMStudyListViewPlugin.h
  ctkDICOMTableManagerPlugin.h
  ctkDICOMTableViewPlugin.h
  ctkDICOMVisualBrowserWidgetPlugin.h
  )

# Resources
set(PLUGIN_resources
)

# Target libraries
set(PLUGIN_target_libraries
  CTKDICOMWidgets
  )

ctkMacroBuildQtDesignerPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
