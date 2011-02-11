PROJECT(org_commontk_dah_host)

SET(PLUGIN_export_directive "org_commontk_dah_host_EXPORT")

SET(PLUGIN_SRCS
  ctkDicomAbstractHost.cpp
  ctkDicomAppService.cpp
  ctkDicomHostPlugin.cpp
  ctkDicomHostPlugin_p.h
  ctkDicomHostServer.cpp
  ctkDicomHostServerPrivate.cpp
  ctkHostSoapMessageProcessor.cpp
  ctkHostSoapMessageProcessor_p.h
)

# Files which should be processed by Qts moc
SET(PLUGIN_MOC_SRCS
  ctkDicomHostPlugin_p.h
  ctkDicomHostServerPrivate.h
  ctkDicomAbstractHost.h
)

# Qt Designer files which should be processed by Qts uic
SET(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
SET(PLUGIN_resources
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
