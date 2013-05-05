project(org_commontk_eventadmintest_perf)

set(PLUGIN_export_directive "org_commontk_eventadmintest_perf_EXPORT")

set(PLUGIN_SRCS
  ctkEventAdminTestPerfActivator_p.h
  ctkEventAdminTestPerfActivator.cpp
  ctkEventAdminPerfTestSuite_p.h
  ctkEventAdminPerfTestSuite.cpp
)

set(PLUGIN_MOC_SRCS
  ctkEventAdminTestPerfActivator_p.h
  ctkEventAdminPerfTestSuite_p.h
)

set(PLUGIN_UI_FORMS

)

set(PLUGIN_resources

)

ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

if(UNIX AND NOT APPLE)
  list(APPEND PLUGIN_target_libraries rt)
endif()

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

set(eventadmin_perftest ${PROJECT_NAME} CACHE INTERNAL "Target name for the plugin containing performance tests for EventAdmin implementations")
