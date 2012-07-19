set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkCmdLineModuleFutureTest.cpp
  ctkCmdLineModuleXmlProgressWatcherTest.cpp
  )

set(TestsToRun ${Tests})
remove(TestsToRun ${KIT}CppTests.cpp)

set(SRC_FILES ${Tests}
  ctkCmdLineModuleSignalTester.cpp
)

qt4_wrap_cpp(SRC_FILES ctkCmdLineModuleSignalTester.h)

set(LIBRARY_NAME ${PROJECT_NAME})

add_executable(${KIT}CppTests ${SRC_FILES})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})
add_dependencies(${KIT}CppTests ctkCmdLineTestModules)

#
# Add Tests
#

foreach(_test ${TestsToRun})
  SIMPLE_TEST(${_test})
endforeach()

