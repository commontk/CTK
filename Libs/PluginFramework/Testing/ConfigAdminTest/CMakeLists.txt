PROJECT(org_commontk_configadmintest)

SET(PLUGIN_export_directive "org_commontk_configadmintest_EXPORT")

SET(PLUGIN_SRCS
  ctkConfigAdminTestActivator.cpp
  ctkConfigurationAdminTestSuite.cpp
  ctkConfigurationListenerTestSuite.cpp
  ctkConfigurationPluginTestSuite.cpp
  ctkManagedServiceFactoryTestSuite.cpp
  ctkManagedServiceTestSuite.cpp

  ctkConfigAdminTestActivator_p.h
  ctkConfigurationAdminTestSuite_p.h
  ctkConfigurationListenerTestSuite_p.h
  ctkConfigurationPluginTestSuite_p.h
  ctkManagedServiceFactoryTestSuite_p.h
  ctkManagedServiceTestSuite_p.h
)

SET(PLUGIN_MOC_SRCS
  ctkConfigAdminTestActivator_p.h
  ctkConfigurationAdminTestSuite_p.h
  ctkConfigurationListenerTestSuite_p.h
  ctkConfigurationPluginTestSuite_p.h
  ctkManagedServiceFactoryTestSuite_p.h
  ctkManagedServiceTestSuite_p.h
)

SET(PLUGIN_UI_FORMS
  
)

SET(PLUGIN_resources
  
)

ctkMacroGetTargetLibraries(PLUGIN_target_libraries)

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

SET(configadmin_test ${PROJECT_NAME} CACHE INTERNAL "Target name for the plugin containing unit tests for ConfigAdmin implementations")
