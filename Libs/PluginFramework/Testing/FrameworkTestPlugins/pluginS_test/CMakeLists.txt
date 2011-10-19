PROJECT(pluginS_test)

SET(PLUGIN_export_directive "pluginS_test_EXPORT")

SET(PLUGIN_SRCS
  ctkTestPluginS.cpp
  ctkTestPluginSActivator.cpp
  ctkTestPluginSService0.h
  ctkTestPluginSService1.h
  ctkTestPluginSService2.h
  ctkTestPluginSService3.h
)

SET(PLUGIN_MOC_SRCS
  ctkTestPluginS_p.h
  ctkTestPluginSActivator_p.h
)

SET(PLUGIN_resources
  
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
