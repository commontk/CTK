project(org_commontk_dah_core)

#message(QtSOAP_LIBRARY_DIRS:${QtSOAP_LIBRARY_DIRS})

SET(PLUGIN_export_directive "org_commontk_dah_core_EXPORT")

set(PLUGIN_SRCS
  ctkDicomAbstractExchangeCache.cpp
  ctkDicomAppHostingCorePlugin.cpp
  ctkDicomAppHostingCorePlugin_p.h
  ctkDicomAppHostingTypes.cpp
  ctkDicomAppHostingTypesHelper.cpp
  ctkDicomAppInterface.h
  ctkDicomAvailableDataHelper.cpp
  ctkDicomAvailableDataHelper.h
  ctkDicomExchangeInterface.h
  ctkDicomExchangeService.cpp
  ctkDicomHostInterface.h
  ctkDicomObjectLocatorCache.cpp
  ctkExchangeSoapMessageProcessor.cpp
  ctkSimpleSoapClient.cpp
  ctkSimpleSoapServer.cpp
  ctkSoapConnectionRunnable.cpp
  ctkSoapConnectionRunnable_p.h
  ctkSoapMessageProcessor.cpp
  ctkSoapMessageProcessorList.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  ctkDicomAbstractExchangeCache.h
  ctkDicomAppHostingCorePlugin_p.h
  ctkSimpleSoapClient.h
  ctkSimpleSoapServer.h
  ctkSoapConnectionRunnable_p.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
  Resources/dah_wsdl.qrc
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

if(CTK_QT_VERSION VERSION_GREATER "4")
  list(APPEND PLUGIN_target_libraries Qt5::Network Qt5::Widgets)
endif()

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

