project(pluginA1_test)

set(PLUGIN_export_directive "pluginA1_test_EXPORT")

set(PLUGIN_SRCS
  ctkTestPluginA.cpp
  ctkTestPluginAActivator.cpp
  ctkTestPluginAService.h
)

set(PLUGIN_resources

)

ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  TEST_PLUGIN
)
