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

set(Tests_MOC_CPP)
QT4_WRAP_CPP(Tests_MOC_CPP ${Tests_MOC_SRCS})
QT4_GENERATE_MOCS(
  ctkCmdLineModuleManagerTest.cpp
  ctkCmdLineModuleXmlProgressWatcherTest.cpp
)
set(Tests_UI_CPP)
if(TEST_UI_FORMS)
  QT4_WRAP_UI(Tests_UI_CPP ${Tests_UI_FORMS})
endif()
set(Tests_RESOURCES_SRCS)
QT4_ADD_RESOURCES(Tests_RESOURCES_SRCS ${Tests_RESOURCES})

add_executable(${KIT}CppTests ${Tests} ${Tests_SRCS} ${Tests_MOC_CPP} ${Tests_UI_CPP} ${Tests_RESOURCES_SRCS})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

#
# Add Tests
#
SIMPLE_TEST(ctkCmdLineModuleManagerTest)
SIMPLE_TEST(ctkCmdLineModuleXmlProgressWatcherTest)
SIMPLE_TEST(ctkCmdLineModuleDefaultPathBuilderTest ${CTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY})
