PROJECT(org_commontk_cli)

SET(PLUGIN_export_directive "org_commontk_cli_EXPORT")

SET(PLUGIN_SRCS
  ctkCLIPlugin.cpp
)

SET(PLUGIN_MOC_SRCS
  ctkCLIPlugin.h
  ctkCLIRegistry.h
  ctkICLIManager.h
)

SET(PLUGIN_resources

)

SET(PLUGIN_cached_resourcefiles
  servicedescriptor.xml
)

ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  CACHED_RESOURCEFILES ${PLUGIN_cached_resourcefiles}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
