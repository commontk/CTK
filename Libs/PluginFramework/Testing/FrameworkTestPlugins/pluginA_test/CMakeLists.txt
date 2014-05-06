project(pluginA_test)

set(PLUGIN_export_directive "pluginA_test_EXPORT")

set(PLUGIN_SRCS
  ctkTestPluginA.cpp
  ctkTestPluginAActivator.cpp
  ctkTestPluginAService.h
)

set(PLUGIN_MOC_SRCS
  ctkTestPluginA_p.h
  ctkTestPluginAActivator_p.h
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
