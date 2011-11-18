project(org_commontk_metatype)

set(PLUGIN_export_directive "org_commontk_metatype_EXPORT")

set(PLUGIN_SRCS
  ctkAttributeDefinitionImpl_p.h
  ctkAttributeDefinitionImpl.cpp
  ctkMetaTypeActivator_p.h
  ctkMetaTypeActivator.cpp
  ctkMetaTypeInformationImpl_p.h
  ctkMetaTypeInformationImpl.cpp
  ctkMetaTypeProviderImpl_p.h
  ctkMetaTypeProviderImpl.cpp
  ctkMetaTypeServiceImpl_p.h
  ctkMetaTypeServiceImpl.cpp
  ctkMTDataParser_p.h
  ctkMTDataParser.cpp
  ctkMTIcon_p.h
  ctkMTIcon.cpp
  ctkMTLocalizationElement_p.h
  ctkMTLocalizationElement.cpp
  ctkMTLogTracker_p.h
  ctkMTLogTracker.cpp
  ctkMTMsg_p.h
  ctkMTMsg.cpp
  ctkMTProviderTracker_p.h
  ctkMTProviderTracker.cpp
  ctkObjectClassDefinitionImpl_p.h
  ctkObjectClassDefinitionImpl.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  ctkMetaTypeActivator_p.h
  ctkMetaTypeServiceImpl_p.h
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

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
