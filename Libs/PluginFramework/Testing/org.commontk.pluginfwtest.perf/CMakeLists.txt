project(org_commontk_pluginfwtest_perf)

set(PLUGIN_export_directive "org_commontk_pluginfwtest_perf_EXPORT")

set(PLUGIN_SRCS
  ctkPluginFrameworkTestPerfActivator_p.h
  ctkPluginFrameworkTestPerfActivator.cpp
  ctkPluginFrameworkPerfRegistryTestSuite_p.h
  ctkPluginFrameworkPerfRegistryTestSuite.cpp
)

set(PLUGIN_MOC_SRCS
  ctkPluginFrameworkTestPerfActivator_p.h
  ctkPluginFrameworkPerfRegistryTestSuite_p.h
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

# =========== Build the test executable ===============
set(SRCS
  ctkPluginFrameworkTestPerfMain.cpp
)

set(test_executable ${PROJECT_NAME}CppTests)

add_executable(${test_executable} ${SRCS})
target_link_libraries(${test_executable}
  ${fw_lib}
  ${fwtestutil_lib}
)

add_dependencies(${test_executable} ${PROJECT_NAME})

add_test(${PROJECT_NAME}Tests ${CPP_TEST_PATH}/${test_executable})
set_property(TEST ${PROJECT_NAME}Tests PROPERTY LABELS ${PROJECT_NAME})
