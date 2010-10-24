PROJECT(org_commontk_dah_app)

SET(PLUGIN_export_directive "org_commontk_dah_app_EXPORT")

SET(PLUGIN_SRCS
  ctkAppSoapMessageProcessor.cpp
  ctkDicomAbstractApp.cpp
  ctkDicomAppPlugin.cpp
  ctkDicomAppServer.cpp
  ctkDicomHostService.cpp
)

# Files which should be processed by Qts moc
SET(PLUGIN_MOC_SRCS
  ctkDicomAppPlugin_p.h
  ctkDicomAppServer_p.h
)

# Qt Designer files which should be processed by Qts uic
SET(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
SET(PLUGIN_resources
)

#Compute the plugin dependencies
ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  INCLUDE_DIRECTORIES ${QTSOAP_INCLUDE_DIR}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
