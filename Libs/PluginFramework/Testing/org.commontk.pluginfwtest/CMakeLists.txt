project(org_commontk_pluginfwtest)

set(PLUGIN_export_directive "org_commontk_pluginfwtest_EXPORT")

set(PLUGIN_SRCS
  ctkPluginFrameworkTestActivator.cpp
  ctkPluginFrameworkTestSuite.cpp
  ctkServiceListenerTestSuite.cpp
  ctkServiceTrackerTestSuite.cpp
)

set(PLUGIN_MOC_SRCS
  ctkPluginFrameworkTestActivator_p.h
  ctkPluginFrameworkTestSuite_p.h
  ctkServiceListenerTestSuite_p.h
  ctkServiceTrackerTestSuite_p.h
)

set(PLUGIN_UI_FORMS

)

set(PLUGIN_resources

)

ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries} ${fwtestutil_lib}
  TEST_PLUGIN
)

add_dependencies(${PROJECT_NAME} ${fwtest_plugins})


# =========== Build the test executable ===============
set(SRCS
  ctkPluginFrameworkTestMain.cpp
)

set(MOC_SRCS )

set(MY_MOC_CXX )

#QT4_WRAP_CPP(MY_MOC_CXX ${MOC_SRCS})

set(test_executable ${fw_lib}CppTests)

add_executable(${test_executable} ${SRCS} ${MY_MOC_CXX})
target_link_libraries(${test_executable}
  ${fw_lib}
  ${fwtestutil_lib}
)

add_dependencies(${test_executable} ${PROJECT_NAME})

add_test(${fw_lib}Tests ${CPP_TEST_PATH}/${test_executable})
set_property(TEST ${fw_lib}Tests PROPERTY LABELS ${fw_lib})
