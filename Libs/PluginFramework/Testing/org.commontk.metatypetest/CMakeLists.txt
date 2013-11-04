project(org_commontk_metatypetest)

set(PLUGIN_export_directive "org_commontk_metatypetest_EXPORT")

set(PLUGIN_SRCS
  ctkMetaTypeTestActivator.cpp
  ctkMTAttrPasswordTestSuite.cpp
  ctkMTLocaleTestSuite.cpp

  ctkMetaTypeTestActivator_p.h
  ctkMTAttrPasswordTestSuite_p.h
  ctkMTLocaleTestSuite_p.h
)

set(PLUGIN_MOC_SRCS
  ctkMetaTypeTestActivator_p.h
  ctkMTAttrPasswordTestSuite_p.h
  ctkMTLocaleTestSuite_p.h
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
  TARGET_LIBRARIES ${PLUGIN_target_libraries} ${fwtestutil_lib}
  TEST_PLUGIN
)

add_dependencies(${PROJECT_NAME} ${metatypetest_plugins})

set(metatype_test ${PROJECT_NAME} CACHE INTERNAL "Target name for the plugin containing unit tests for MetaType implementations")
