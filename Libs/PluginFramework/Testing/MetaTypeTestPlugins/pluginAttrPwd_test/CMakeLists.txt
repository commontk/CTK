project(pluginAttrPwd_test)

set(PLUGIN_export_directive "pluginAttrPwd_test_EXPORT")

set(PLUGIN_SRCS
  ctkTestPluginMTAttrPwdActivator_p.h
  ctkTestPluginMTAttrPwdActivator.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  ctkTestPluginMTAttrPwdActivator_p.h
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

set(PLUGIN_cached_resources
  CTK-INF/metatype/testfile.xml
)

set(PLUGIN_ts_files
  ${PROJECT_NAME}_de.ts
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS} ${PLUGIN_qm_files}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  CACHED_RESOURCEFILES ${PLUGIN_cached_resources}
  TRANSLATIONS ${PLUGIN_ts_files}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  TEST_PLUGIN
)
