project(pluginSL3_test)

set(PLUGIN_export_directive "pluginSL3_test_EXPORT")

set(PLUGIN_SRCS
  ctkActivatorSL3.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  ctkActivatorSL3_p.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

# Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  TEST_PLUGIN
)
