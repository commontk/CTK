project(org_commontk_dah_cmdlinemoduleapp)

set(PLUGIN_export_directive "org_commontk_dah_cmdlinemoduleapp_EXPORT")

set(PLUGIN_SRCS
  ctkCommandLineModuleAppLogic_p.h
  ctkCommandLineModuleAppPlugin_p.h
  ctkCommandLineModuleAppLogic.cpp
  ctkCommandLineModuleAppPlugin.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  ctkCommandLineModuleAppLogic_p.h
  ctkCommandLineModuleAppPlugin_p.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
  ctkCommandLineModuleAppWidget.ui
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
#  resources/ctkCommandLineModuleApp.qrc
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
