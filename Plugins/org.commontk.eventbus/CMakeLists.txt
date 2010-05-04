PROJECT(org_commontk_eventbus)

SET(PLUGIN_export_directive "org_commontk_eventbus_EXPORT")

SET(PLUGIN_SRCS
  ctkEventBusImpl.cxx
  ctkEventBusPlugin.cxx
)

SET(PLUGIN_MOC_SRCS
  ctkEventBusImpl_p.h
  ctkEventBusPlugin_p.h
  ctkEventHandlerWrapper_p.h
)

SET(PLUGIN_resources
  org_commontk_eventbus.qrc
)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Plugins)

ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
