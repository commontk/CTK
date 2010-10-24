PROJECT(org_commontk_dah_examplehost)

SET(PLUGIN_export_directive "org_commontk_dah_examplehost_EXPORT")

SET(PLUGIN_SRCS
  ctkExampleDicomHost.cpp
  ctkExampleDicomHostPlugin.cpp
  ctkHostedAppPlaceholderWidget.cpp
)

# Files which should be processed by Qts moc
SET(PLUGIN_MOC_SRCS
  ctkExampleDicomHost.h
  ctkExampleDicomHostPlugin_p.h
  ctkHostedAppPlaceholderWidget.h
)

# Qt Designer files which should be processed by Qts uic
SET(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
SET(PLUGIN_resources
)

# Additional directories to include - Note that CTK_INCLUDE_LIBRARIES is already included
SET(PLUGIN_include_directories
)
#Compute the plugin dependencies
ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  INCLUDE_DIRECTORIES ${PLUGIN_include_directories}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
