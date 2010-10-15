PROJECT(org_commontk_eventbus)

SET(PLUGIN_export_directive "org_commontk_eventbus_EXPORT")

SET(PLUGIN_SRCS
  ctkEventBusImpl.cpp
  ctkEventBusPlugin.cpp
)

SET(PLUGIN_MOC_SRCS
  ctkEventBusImpl_p.h
  ctkEventBusPlugin_p.h
  ctkEventHandlerWrapper_p.h
)

SET(PLUGIN_resources
  #org_commontk_eventbus.qrc
)

SET(PLUGIN_CACHED_RESOURCEFILES
  servicedescriptor.xml
)

ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  CACHED_RESOURCEFILES ${PLUGIN_CACHED_RESOURCEFILES}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
