PROJECT(org_commontk_dah_core)

SET(PLUGIN_export_directive "org_commontk_dah_core_EXPORT")

SET(PLUGIN_SRCS
  ctkDicomAppHostingCorePlugin.cpp
  ctkDicomAppHostingTypes.h
  ctkDicomAppHostingTypesHelper.h
  ctkDicomAppInterface.h
  ctkDicomExchangeService.cpp
  ctkDicomHostInterface.h
  ctkDicomServicePrivate.cpp
  ctkExchangeSoapMessageProcessor.cpp
  ctkSimpleSoapServer.cpp
  ctkSoapConnectionRunnable.cpp
  ctkSoapMessageProcessorList.cpp
)

# Files which should be processed by Qts moc
SET(PLUGIN_MOC_SRCS
  ctkDicomAppHostingCorePlugin_p.h
  ctkDicomAppInterface.h
  ctkDicomExchangeInterface.h
  ctkDicomHostInterface.h
  ctkDicomServicePrivate.h
  ctkSimpleSoapServer.h
  ctkSoapConnectionRunnable_p.h
)

# Qt Designer files which should be processed by Qts uic
SET(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
SET(PLUGIN_resources
  Resources/dah_wsdl.qrc
)

#Compute the plugin dependencies
ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
