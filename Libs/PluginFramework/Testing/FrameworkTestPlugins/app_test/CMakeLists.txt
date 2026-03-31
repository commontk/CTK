project(app_test)

set(PLUGIN_export_directive "app_test_EXPORT")

set(PLUGIN_SRCS
  ctkTestApp.cpp
  ctkTestAppActivator.cpp
)

set(PLUGIN_resources

)

ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  TEST_PLUGIN
)

# =========== Build a separate test executable ===============
set(SRCS
  ctkTestAppMain.cpp
)

set(test_executable ${fw_lib}AppTests)

ctk_add_executable_utf8(${test_executable} ${SRCS})
target_link_libraries(${test_executable}
  ${fw_lib}
)

set_target_properties(${test_executable} PROPERTIES
  AUTOMOC ON
)

target_link_libraries(${test_executable} Qt${CTK_QT_VERSION}::Test)

add_dependencies(${test_executable} ${PROJECT_NAME})

add_test(${fw_lib}AppTests ${CPP_TEST_PATH}/${test_executable})
set_property(TEST ${fw_lib}AppTests PROPERTY LABELS ${fw_lib})
# QT_FATAL_WARNINGS=1 converts Qt warnings to abort(). In Qt6, a
# benign QThreadStorage cleanup-order warning fires during static
# destructor teardown (after main returns 0), causing a false abort.
# This warning bypasses Qt's message handler (printed via fprintf)
# so it cannot be filtered in code. Disable QT_FATAL_WARNINGS for
# Qt6 to avoid the false failure.
if(CTK_QT_VERSION VERSION_LESS "6")
  set_property(TEST ${fw_lib}AppTests PROPERTY ENVIRONMENT "QT_FATAL_WARNINGS=1")
endif()
set_property(TEST ${fw_lib}AppTests PROPERTY RESOURCE_LOCK ctkPluginStorage)
