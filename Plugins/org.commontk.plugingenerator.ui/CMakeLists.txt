PROJECT(org_commontk_plugingenerator_ui)

SET(PLUGIN_export_directive "org_commontk_plugingenerator_ui_EXPORT")

SET(PLUGIN_SRCS
  ctkPluginGeneratorAbstractUiSection.cpp
  ctkPluginGeneratorMainSection.cpp
)

SET(PLUGIN_MOC_SRCS
  ctkPluginGeneratorAbstractUiSection.h
  ctkPluginGeneratorMainSection.h
)

SET(PLUGIN_UI_FORMS
  ctkPluginGeneratorMainSection.ui
  ctkPluginGeneratorManifestSection.ui
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
