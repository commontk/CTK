PROJECT(pluginA_test)

SET(PLUGIN_export_directive "pluginA_test_EXPORT")

SET(PLUGIN_SRCS
  ctkTestPluginA.cpp
  ctkTestPluginAActivator.cpp
  ctkTestPluginAService.h
)

SET(PLUGIN_MOC_SRCS
  ctkTestPluginA_p.h
  ctkTestPluginAActivator_p.h
)

SET(PLUGIN_resources
  
)

ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  TEST_PLUGIN
)
