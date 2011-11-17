set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkSlicerModuleTest.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

add_executable(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME} ${CTK_BASE_LIBRARIES})

set( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

set(TEST_DATA ${${PROJECT_NAME}_SOURCE_DIR}/TestData)

macro( SIMPLE_TEST  TESTNAME DATA_FILE )
  add_test( ${TESTNAME} "${KIT_TESTS}" "${TESTNAME}" "${TEST_DATA}/${DATA_FILE}" )
  set_property(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
endmacro( SIMPLE_TEST  )

#
# Add Tests
#

SIMPLE_TEST( ctkSlicerModuleTest ParserTest1.xml )

