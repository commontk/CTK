set(KIT CTKCommandLineModules)
set(LIBRARY_NAME ${KIT})

set(_test_srcs)
set(_test_mocs)

if(CTK_LIB_CommandLineModules/Frontend/QtGui)
  if(CTK_LIB_CommandLineModules/Backend/LocalProcess)
    set(_test_cpp_files
        ctkCmdLineModuleFutureTest.cpp
        ctkCmdLineModuleProcessXmlOutputTest.cpp
        )
    list(APPEND _test_srcs ${_test_cpp_files})
    list(APPEND _test_mocs ${_test_cpp_files})
  endif()
  if(CTK_LIB_CommandLineModules/Backend/FunctionPointer)
    list(APPEND _test_srcs ctkCmdLineModuleQtCustomizationTest.cpp)
    list(APPEND _test_mocs ctkCmdLineModuleQtCustomizationTest.cpp)
  endif()
endif()

create_test_sourcelist(Tests ${KIT}CppTests.cpp ${_test_srcs})

set(TestsToRun ${Tests})
remove(TestsToRun ${KIT}CppTests.cpp)

set(Tests_SRCS ${Tests_SRCS}
  ctkCmdLineModuleSignalTester.cpp
)
set(Tests_MOC_SRCS ${Tests_MOC_SRCS}
  ctkCmdLineModuleSignalTester.h
)
set(Tests_RESOURCES
  ctkCmdLineModuleTestResources.qrc
)

set(_base_src_include_dir ${CMAKE_SOURCE_DIR}/Libs/CommandLineModules)
set(_base_bin_include_dir ${CMAKE_BINARY_DIR}/Libs/CommandLineModules)

include_directories(
  ${CMAKE_SOURCE_DIR}/Libs/Testing
  ${CMAKE_CURRENT_BINARY_DIR}
  ${_base_src_include_dir}/Core
  ${_base_bin_include_dir}/Core
  )

set(_additional_link_libraries)

if(CTK_LIB_CommandLineModules/Backend/LocalProcess)
  include_directories(${_base_src_include_dir}/Backend/LocalProcess
                      ${_base_bin_include_dir}/Backend/LocalProcess)
  list(APPEND _additional_link_libraries CTKCommandLineModulesBackendLocalProcess)
endif()

if(CTK_LIB_CommandLineModules/Backend/FunctionPointer)
  include_directories(${_base_src_include_dir}/Backend/FunctionPointer
                      ${_base_bin_include_dir}/Backend/FunctionPointer)
  list(APPEND _additional_link_libraries CTKCommandLineModulesBackendFunctionPointer)
endif()

if(CTK_LIB_CommandLineModules/Frontend/QtGui)
  include_directories(${_base_src_include_dir}/Frontend/QtGui
                      ${_base_bin_include_dir}/Frontend/QtGui)
  list(APPEND _additional_link_libraries CTKCommandLineModulesFrontendQtGui)
endif()

ctk_add_executable_utf8(${KIT}CppTests ${Tests} ${Tests_SRCS} ${Tests_RESOURCES})
target_link_libraries(${KIT}CppTests ${_additional_link_libraries})
add_dependencies(${KIT}CppTests ctkCmdLineTestModules)

# Configure CMake Qt automatic code generation
set_target_properties(${KIT}CppTests PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  )

target_link_libraries(${KIT}CppTests
  Qt${CTK_QT_VERSION}::Gui
  Qt${CTK_QT_VERSION}::Test
  )

if(TARGET CTKCommandLineModulesCoreCppTests)
  add_dependencies(${KIT}CppTests CTKCommandLineModulesCoreCppTests)
endif()
if(TARGET CTKCommandLineModulesFrontendQtGuiCppTests)
  add_dependencies(${KIT}CppTests CTKCommandLineModulesFrontendQtGuiCppTests)
endif()

#
# Add Tests
#

foreach(_src ${_test_srcs})
  get_filename_component(_test_name ${_src} NAME_WE)
  SIMPLE_TEST(${_test_name})
endforeach()
