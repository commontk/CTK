project(pluginA2_test)

set(PLUGIN_export_directive "pluginA2_test_EXPORT")

set(PLUGIN_SRCS
  ctkTestPluginA2.cpp
  ctkTestPluginA2Activator.cpp
  ctkTestPluginA2Service.h
)

set(PLUGIN_MOC_SRCS
  ctkTestPluginA2_p.h
  ctkTestPluginA2Activator_p.h
)

set(PLUGIN_resources

)

ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  TEST_PLUGIN
)
