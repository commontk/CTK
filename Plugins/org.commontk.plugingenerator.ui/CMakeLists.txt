PROJECT(org_commontk_plugingenerator_ui)

SET(PLUGIN_export_directive "org_commontk_plugingenerator_ui_EXPORT")

SET(PLUGIN_SRCS
  ctkPluginGeneratorAbstractUiExtension.cpp
  ctkPluginGeneratorMainExtension.cpp
  ctkPluginGeneratorUiPlugin.cpp
)

SET(PLUGIN_MOC_SRCS
  ctkPluginGeneratorAbstractUiExtension.h
  ctkPluginGeneratorMainExtension.h
  ctkPluginGeneratorUiPlugin_p.h
)

SET(PLUGIN_UI_FORMS
  ctkPluginGeneratorMainExtension.ui
  ctkPluginGeneratorManifestExtension.ui
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
