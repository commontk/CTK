set(KIT CTKCommandLineModules)
set(LIBRARY_NAME ${KIT})

set(_test_srcs)
set(_test_mocs)

if(CTK_LIB_CommandLineModules/Frontend/QtGui)
  if(CTK_QT_VERSION VERSION_LESS "5")
    set(QT_USE_QTUITOOLS 1)
    include(${QT_USE_FILE})
  endif()

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

set(Tests_MOC_CPP)
set(Tests_UI_CPP)
set(Tests_RESOURCES_SRCS)
if (CTK_QT_VERSION VERSION_GREATER "4")
  QT5_WRAP_CPP(Tests_MOC_CPP ${Tests_MOC_SRCS})
  if(_test_mocs)
    QT5_GENERATE_MOCS(${_test_mocs})
  endif()
  if(TEST_UI_FORMS)
    QT5_WRAP_UI(Tests_UI_CPP ${Tests_UI_FORMS})
  endif()
  QT5_ADD_RESOURCES(Tests_RESOURCES_SRCS ${Tests_RESOURCES})
else()
  QT4_WRAP_CPP(Tests_MOC_CPP ${Tests_MOC_SRCS})
  if(_test_mocs)
    QT4_GENERATE_MOCS(${_test_mocs})
  endif()
  if(TEST_UI_FORMS)
    QT4_WRAP_UI(Tests_UI_CPP ${Tests_UI_FORMS})
  endif()
  QT4_ADD_RESOURCES(Tests_RESOURCES_SRCS ${Tests_RESOURCES})
endif()

ctk_add_executable_utf8(${KIT}CppTests ${Tests} ${Tests_SRCS} ${Tests_MOC_CPP} ${Tests_UI_CPP} ${Tests_RESOURCES_SRCS})
target_link_libraries(${KIT}CppTests ${_additional_link_libraries})
add_dependencies(${KIT}CppTests ctkCmdLineTestModules)

if(CTK_QT_VERSION VERSION_GREATER "4")
  target_link_libraries(${KIT}CppTests Qt5::Gui Qt5::Test)
endif()

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
