project(pluginA2_test)

set(PLUGIN_export_directive "pluginA2_test_EXPORT")

set(PLUGIN_SRCS
  ctkTestPluginA2.cpp
  ctkTestPluginA2Activator.cpp
  ctkTestPluginA2Service.h
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
