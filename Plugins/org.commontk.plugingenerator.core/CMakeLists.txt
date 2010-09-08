PROJECT(org_commontk_plugingenerator_core)

SET(PLUGIN_export_directive "org_commontk_plugingenerator_core_EXPORT")

SET(PLUGIN_SRCS
  ctkPluginGeneratorAbstractExtension.cpp
  ctkPluginGeneratorAbstractTemplate.cpp
  ctkPluginGeneratorConstants.cpp
  ctkPluginGeneratorCorePlugin.cpp
  ctkPluginGeneratorCMakeLists.cpp
  ctkPluginGeneratorCodeModel.cpp
  ctkPluginGeneratorCppPluginActivator
  ctkPluginGeneratorCppTemplate.cpp
  ctkPluginGeneratorHeaderTemplate.cpp
  ctkPluginGeneratorTargetLibraries.cpp
)

SET(PLUGIN_MOC_SRCS
  ctkPluginGeneratorAbstractExtension.h
  ctkPluginGeneratorAbstractTemplate.h
  ctkPluginGeneratorCodeModel.h
  ctkPluginGeneratorCorePlugin_p.h
)

SET(PLUGIN_UI_FORMS
  
)

SET(PLUGIN_resources
  
)

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
