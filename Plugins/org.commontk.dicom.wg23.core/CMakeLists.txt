PROJECT(org_commontk_dicom_wg23_core)

FIND_PACKAGE(QtSOAP)
IF(NOT QtSOAP_FOUND)
  MESSAGE(FATAL_ERROR "error: QtSOAP package is required to build ${PROJECT_NAME}" )
ENDIF()

# TODO: Fix this after discussing include dependencies with Jc
SET(CTK_BASE_INCLUDE_DIRS ${CTK_BASE_INCLUDE_DIRS} ${QTSOAP_INCLUDE_DIR} CACHE INTERNAL "CTK includes" FORCE)

SET(PLUGIN_export_directive "org_commontk_dicom_wg23_core_EXPORT")

# Additional directories to include
SET(PLUGIN_include_directories

)

SET(PLUGIN_SRCS
  ctkDicomWG23CorePlugin.cpp
  ctkSimpleSoapServer.cpp
  ctkSoapConnectionRunnable.cpp
  ctkDicomServicePrivate.cpp
  ctkDicomWG23Types.h
  ctkDicomWG23TypesHelper.h
  ctkDicomExchangeService.cpp
  ctkSoapMessageProcessorList.cpp
  ctkExchangeSoapMessageProcessor.cpp
)

# Files which should be processed by Qts moc
SET(PLUGIN_MOC_SRCS

  ctkDicomAppInterface.h
  ctkDicomExchangeInterface.h
  ctkDicomHostInterface.h
  ctkDicomServicePrivate.h
  ctkDicomWG23CorePlugin_p.h
  ctkSimpleSoapServer.h
  ctkSoapConnectionRunnable_p.h
  ctkDicomExchangeService.h
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
  INCLUDE_DIRECTORIES ${PLUGIN_include_directories}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
