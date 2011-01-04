PROJECT(org_commontk_pluginfwtest)

SET(PLUGIN_export_directive "org_commontk_pluginfwtest_EXPORT")

SET(PLUGIN_SRCS
  ctkPluginFrameworkTestActivator.cpp
  ctkPluginFrameworkTestSuite.cpp
  ctkServiceListenerTestSuite.cpp
)

SET(PLUGIN_MOC_SRCS
  ctkPluginFrameworkTestActivator_p.h
  ctkPluginFrameworkTestSuite_p.h
  ctkServiceListenerTestSuite_p.h
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
  TARGET_LIBRARIES ${PLUGIN_target_libraries} ${fwtestutil_lib}
  TEST_PLUGIN
)

ADD_DEPENDENCIES(${PROJECT_NAME} ${fwtest_plugins})


# =========== Build the test executable ===============
SET(SRCS
  ctkPluginFrameworkTestMain.cpp
)

SET(MOC_SRCS )

SET(MY_MOC_CXX )

#QT4_WRAP_CPP(MY_MOC_CXX ${MOC_SRCS})

SET(test_executable ${fw_lib}CppTests)

ADD_EXECUTABLE(${test_executable} ${SRCS} ${MY_MOC_CXX})
TARGET_LINK_LIBRARIES(${test_executable}
  ${fw_lib}
  ${fwtestutil_lib}
)

ADD_DEPENDENCIES(${test_executable} ${PROJECT_NAME})

ADD_TEST(${fw_lib}Tests ${CPP_TEST_PATH}/${test_executable})
SET_PROPERTY(TEST ${fw_lib}Tests PROPERTY LABELS ${fw_lib})
