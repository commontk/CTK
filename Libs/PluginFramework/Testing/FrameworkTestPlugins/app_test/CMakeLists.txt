project(app_test)

set(PLUGIN_export_directive "app_test_EXPORT")

set(PLUGIN_SRCS
  ctkTestApp.cpp
  ctkTestAppActivator.cpp
)

set(PLUGIN_MOC_SRCS
  ctkTestApp_p.h
  ctkTestAppActivator_p.h
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
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  TEST_PLUGIN
)

# =========== Build a separate test executable ===============
set(SRCS
  ctkTestAppMain.cpp
)

set(MOC_SRCS )

set(MY_MOC_CXX )

if(CTK_QT_VERSION VERSION_EQUAL "5")
  qt5_wrap_cpp(MY_MOC_CXX ${MOC_SRCS})
else()
  message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
endif()
set(test_executable ${fw_lib}AppTests)

ctk_add_executable_utf8(${test_executable} ${SRCS} ${MY_MOC_CXX})
target_link_libraries(${test_executable}
  ${fw_lib}
)

target_link_libraries(${test_executable} Qt${CTK_QT_VERSION}::Test)

add_dependencies(${test_executable} ${PROJECT_NAME})

add_test(${fw_lib}AppTests ${CPP_TEST_PATH}/${test_executable})
set_property(TEST ${fw_lib}AppTests PROPERTY LABELS ${fw_lib})
set_property(TEST ${fw_lib}AppTests PROPERTY ENVIRONMENT "QT_FATAL_WARNINGS=1")
set_property(TEST ${fw_lib}AppTests PROPERTY RESOURCE_LOCK ctkPluginStorage)
