PROJECT(org_commontk_b)

SET(PLUGIN_export_directive "org_commontk_b_EXPORT")

SET(PLUGIN_SRCS
  ctkClassB.cpp
  ctkBPlugin.cpp
)

SET(PLUGIN_MOC_SRCS
  ctkBPlugin.h
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
