PROJECT(org_commontk_a)

SET(PLUGIN_export_directive "org_commontk_a_EXPORT")

SET(PLUGIN_SRCS
  ctkAPlugin.cpp
)

SET(PLUGIN_MOC_SRCS
  ctkAPlugin.h
)

SET(PLUGIN_resources

)

ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
