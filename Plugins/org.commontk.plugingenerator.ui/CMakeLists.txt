project(org_commontk_plugingenerator_ui)

set(PLUGIN_export_directive "org_commontk_plugingenerator_ui_EXPORT")

set(PLUGIN_SRCS
  ctkPluginGeneratorAbstractUiExtension.cpp
  ctkPluginGeneratorMainExtension.cpp
  ctkPluginGeneratorUiPlugin.cpp
)

set(PLUGIN_MOC_SRCS
  ctkPluginGeneratorAbstractUiExtension.h
  ctkPluginGeneratorMainExtension.h
  ctkPluginGeneratorUiPlugin_p.h
)

set(PLUGIN_UI_FORMS
  ctkPluginGeneratorMainExtension.ui
  ctkPluginGeneratorManifestExtension.ui
)

set(PLUGIN_resources

)

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
