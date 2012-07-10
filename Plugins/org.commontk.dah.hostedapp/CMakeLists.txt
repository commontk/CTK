project(org_commontk_dah_hostedapp)

set(PLUGIN_export_directive "org_commontk_dah_hostedapp_EXPORT")

set(PLUGIN_SRCS
  ctkAppSoapMessageProcessor.cpp
  ctkDicomAbstractApp.cpp
  ctkDicomAppPlugin.cpp
  ctkDicomAppPlugin_p.h
  ctkDicomAppServer.cpp
  ctkDicomAppServer_p.h
  ctkDicomHostService.cpp
  ctkDicomHostService_p.h
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  ctkDicomAppPlugin_p.h
  ctkDicomAppServer_p.h
  ctkDicomHostService_p.h
  ctkDicomAbstractApp.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
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
