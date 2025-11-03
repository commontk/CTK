set(KIT ${PROJECT_NAME})
set(LIBRARY_NAME ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkCmdLineModuleManagerTest.cpp
  ctkCmdLineModuleXmlProgressWatcherTest.cpp
  ctkCmdLineModuleDefaultPathBuilderTest.cpp
  )

set(TestsToRun ${Tests})
remove(TestsToRun ${KIT}CppTests.cpp)

set(Tests_SRCS ${Tests_SRCS}
  ctkCmdLineModuleSignalTester.cpp
)
set(Tests_MOC_SRCS ${Tests_MOC_SRCS}
  ctkCmdLineModuleSignalTester.h
)

include_directories(
  ${CMAKE_SOURCE_DIR}/Libs/Testing
  ${CMAKE_CURRENT_BINARY_DIR}
  )

ctk_add_executable_utf8(${KIT}CppTests ${Tests} ${Tests_SRCS} ${Tests_RESOURCES})

# Configure CMake Qt automatic code generation
set_target_properties(${KIT}CppTests PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  )

target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

target_link_libraries(${KIT}CppTests Qt${CTK_QT_VERSION}::Test)

#
# Add Tests
#
SIMPLE_TEST(ctkCmdLineModuleManagerTest)
SIMPLE_TEST(ctkCmdLineModuleXmlProgressWatcherTest)
SIMPLE_TEST(ctkCmdLineModuleDefaultPathBuilderTest ${CTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY})
