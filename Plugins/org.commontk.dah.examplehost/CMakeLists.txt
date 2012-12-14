project(org_commontk_dah_examplehost)

set(PLUGIN_export_directive "org_commontk_dah_examplehost_EXPORT")

set(PLUGIN_SRCS
  ctkExampleDicomHost.cpp
  ctkExampleDicomHostPlugin.cpp
  ctkExampleDicomHostPlugin_p.h
  ctkHostedAppPlaceholderWidget.cpp
  ctkExampleHostControlWidget.cpp
  ctkExampleHostLogic.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  ctkExampleDicomHost.h
  ctkExampleDicomHostPlugin_p.h
  ctkHostedAppPlaceholderWidget.h
  ctkExampleHostControlWidget.h
  ctkExampleHostLogic.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
  ctkExampleHostControlWidget.ui
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

# Additional directories to include - Note that CTK_INCLUDE_LIBRARIES is already included
set(PLUGIN_include_directories
)
#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

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
