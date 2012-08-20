set(KIT CTKCommandLineModules)
set(LIBRARY_NAME ${KIT})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkCmdLineModuleFutureTest.cpp
  )

set(TestsToRun ${Tests})
remove(TestsToRun ${KIT}CppTests.cpp)

set(Tests_SRCS ${Tests_SRCS}
  ctkCmdLineModuleSignalTester.cpp
)
set(Tests_MOC_SRCS ${Tests_MOC_SRCS}
  ctkCmdLineModuleSignalTester.h
)

set(_base_src_include_dir ${CMAKE_SOURCE_DIR}/Libs/CommandLineModules)
set(_base_bin_include_dir ${CMAKE_BINARY_DIR}/Libs/CommandLineModules)

include_directories(
  ${CMAKE_SOURCE_DIR}/Libs/Testing
  ${CMAKE_CURRENT_BINARY_DIR}
  ${_base_src_include_dir}/Core
  ${_base_bin_include_dir}/Core
  ${_base_src_include_dir}/Backend/LocalProcess
  ${_base_bin_include_dir}/Backend/LocalProcess
  )

set(Tests_MOC_CPP)
QT4_WRAP_CPP(Tests_MOC_CPP ${Tests_MOC_SRCS})

set(Tests_UI_CPP)
if(TEST_UI_FORMS)
  QT4_WRAP_UI(Tests_UI_CPP ${Tests_UI_FORMS})
endif()
set(Tests_RESOURCES_SRCS)
QT4_ADD_RESOURCES(Tests_RESOURCES_SRCS ${Tests_RESOURCES})

add_executable(${KIT}CppTests ${Tests} ${Tests_SRCS} ${Tests_MOC_CPP} ${Tests_UI_CPP} ${Tests_RESOURCES_SRCS})
target_link_libraries(${KIT}CppTests CTKCommandLineModulesBackendLocalProcess)
add_dependencies(${KIT}CppTests ctkCmdLineTestModules)

if(TARGET CTKCommandLineModulesCoreCppTests)
  add_dependencies(${KIT}CppTests CTKCommandLineModulesCoreCppTests)
endif()
if(TARGET CTKCommandLineModulesFrontendQtGuiCppTests)
  add_dependencies(${KIT}CppTests CTKCommandLineModulesFrontendQtGuiCppTests)
endif()

#
# Add Tests
#

SIMPLE_TEST(ctkCmdLineModuleFutureTest)

