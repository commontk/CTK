project(org_commontk_eventadmintest)

set(PLUGIN_export_directive "org_commontk_eventadmintest_EXPORT")

set(PLUGIN_SRCS
  ctkEventAdminTestActivator_p.h
  ctkEventAdminTestActivator.cpp
  ctkEAScenario1TestSuite_p.h
  ctkEAScenario1TestSuite.cpp
  ctkEAScenario2TestSuite_p.h
  ctkEAScenario2TestSuite.cpp
  ctkEAScenario3TestSuite_p.h
  ctkEAScenario3TestSuite.cpp
  ctkEAScenario4TestSuite_p.h
  ctkEAScenario4TestSuite.cpp
  ctkEATopicWildcardTestSuite_p.h
  ctkEATopicWildcardTestSuite.cpp
)

set(PLUGIN_MOC_SRCS
  ctkEventAdminTestActivator_p.h
  ctkEAScenario1TestSuite_p.h
  ctkEAScenario2TestSuite_p.h
  ctkEAScenario3TestSuite_p.h
  ctkEAScenario4TestSuite_p.h
  ctkEATopicWildcardTestSuite_p.h
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

set(eventadmin_test ${PROJECT_NAME} CACHE INTERNAL "Target name for the plugin containing unit tests for EventAdmin implementations")
