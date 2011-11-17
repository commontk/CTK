set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cxx
  ctkDicomAppHostingTypesTest1.cpp
  ctkDicomObjectLocatorCacheTest1.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cxx)

set(LIBRARY_NAME ${PROJECT_NAME})

add_executable(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME})

set(KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

macro( SIMPLE_TEST  TESTNAME )
  add_test( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
  set_property(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
endmacro( SIMPLE_TEST )

#
# Add Tests
#

SIMPLE_TEST( ctkDicomAppHostingTypesTest1 )
SIMPLE_TEST( ctkDicomObjectLocatorCacheTest1 )
