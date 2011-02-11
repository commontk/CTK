PROJECT(org_commontk_eventadmintest)

SET(PLUGIN_export_directive "org_commontk_eventadmintest_EXPORT")

SET(PLUGIN_SRCS
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

SET(PLUGIN_MOC_SRCS
  ctkEventAdminTestActivator_p.h
  ctkEAScenario1TestSuite_p.h
  ctkEAScenario2TestSuite_p.h
  ctkEAScenario3TestSuite_p.h
  ctkEAScenario4TestSuite_p.h
  ctkEATopicWildcardTestSuite_p.h
)

SET(PLUGIN_UI_FORMS
  
)

SET(PLUGIN_resources
  
)

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

SET(eventadmin_test ${PROJECT_NAME} CACHE INTERNAL "Target name for the plugin containing unit tests for EventAdmin implementations")
