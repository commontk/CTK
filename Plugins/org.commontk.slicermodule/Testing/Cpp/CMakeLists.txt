set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkSlicerModuleTest.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

add_executable(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

set(TEST_DATA ${${PROJECT_NAME}_SOURCE_DIR}/TestData)

#
# Add Tests
#

SIMPLE_TEST( ctkSlicerModuleTest ${TEST_DATA}/ParserTest1.xml )

